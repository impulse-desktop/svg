import fnmatch
import glob
import hashlib
import os
import shutil
import subprocess

import build
import build.flags as flags


flags.allow({
    "filter": {"descr": "glob over the cases, as paths under tst/corpus (resvg/*, */*gradient*)", "default": ""},
})

build.cxxflags += ["-std=c++26", "-O2", "-g"]

warning_flags = ["-Wall", "-Wextra", "-Werror"]

libstd = import_build(
    "ext/libstd/build.py",
    "libstd.a",
    extra_cflags=["-Wno-error", "-ffile-prefix-map=$(S)=.", "-ffile-prefix-map=$(B)=."],
)
png = pkg_config("libpng")
system = dependency(ldflags=["-lm"])

wasm2c = shutil.which("wasm2c")
if wasm2c is None:
    raise RuntimeError("wasm2c (wabt) is required: the test driver's image decoder is compiled from ext/decode/decode.simd128.wasm")
wabt_prefix = os.path.dirname(os.path.dirname(os.path.realpath(wasm2c)))
wasm_rt = os.path.join(wabt_prefix, "share", "wabt", "wasm2c")
wasm_rt_header = next(
    (d for d in (os.path.join(wabt_prefix, "include"), wasm_rt) if os.path.exists(os.path.join(d, "wasm-rt.h"))),
    None,
)
if wasm_rt_header is None or not os.path.exists(os.path.join(wasm_rt, "wasm-rt-impl.c")):
    raise RuntimeError(f"the wasm2c runtime (wasm-rt.h, wasm-rt-impl.c) is missing under {wabt_prefix}")
wabt_version = subprocess.check_output([wasm2c, "--version"], text=True).strip()

decode_runtime = sorted({
    *glob.glob(os.path.join(wasm_rt, "wasm-rt*")),
    *glob.glob(os.path.join(wasm_rt_header, "wasm-rt*.h")),
})
decode_defines = [
    "-DWASM_RT_USE_MMAP=1",
    "-DWASM_RT_MEMCHECK_BOUNDS_CHECK=1",
    "-DWASM_RT_MEMCHECK_GUARD_PAGES=0",
    "-DWASM_RT_MAX_CALL_STACK_DEPTH=250",
    "-DWASM_RT_TRAP_HANDLER=decodeTrapHandler",
]
decode_shards = 16
decode_dir = "$(B)/decode"
decode_sources = [f"{decode_dir}/decode_{i}.c" for i in range(decode_shards)]
decode_runtime_files = [f"{decode_dir}/{os.path.basename(path)}" for path in decode_runtime]
decode_c = command(
    name="decode_c",
    inputs=["$(S)/ext/decode/decode.simd128.wasm"],
    outputs=[*decode_sources, f"{decode_dir}/decode.h", f"{decode_dir}/decode-impl.h", *decode_runtime_files],
    cmd=[
        ["wasm2c", "$(S)/ext/decode/decode.simd128.wasm", "--module-name", "decode", "--num-outputs", str(decode_shards), "-o", f"{decode_dir}/decode.c"],
        ["cp", *decode_runtime, f"{decode_dir}/"],
    ],
    env={"WABT_VERSION": wabt_version},
    descr="WC",
)
decode = library(
    name="decode",
    srcs=[*decode_sources, *[path for path in decode_runtime_files if path.endswith(".c")]],
    cflags=["-O2", "-g0", "-w", "-fexceptions"],
    cppflags=decode_defines,
    includes=[decode_dir],
    public_cppflags=[f"-I{decode_dir}", *decode_defines],
    deps=[decode_c],
)

svg = program(
    name="svg",
    output="$(B)/svg",
    srcs=["$(S)/svg.cpp", "$(S)/main.cpp"],
    cflags=warning_flags,
    deps=[libstd, decode, png, system],
)

libfuzzer = os.environ.get("LIBFUZZER")
fuzzer_ldflags = (
    ["-Wl,--export-dynamic", "-Wl,--whole-archive", libfuzzer, "-Wl,--no-whole-archive", "-pthread", "-ldl"]
    if libfuzzer
    else ["-fsanitize=fuzzer"]
)

fuzz = program(
    name="fuzz",
    output="$(B)/fuzz",
    srcs=["$(S)/svg.cpp", "$(S)/fuzz.cpp"],
    cflags=warning_flags,
    cxxflags=["-fsanitize=fuzzer-no-link"],
    ldflags=fuzzer_ldflags,
    deps=[libstd, system],
)

install(svg)


labels = ["1x", "2x"]
corpus = os.path.join(os.path.dirname(os.path.abspath(__file__)), "tst", "corpus")
with open(os.path.join(corpus, "EXCLUDED")) as handle:
    excluded = {line.split(":")[0].strip() + ".svg" for line in handle if ":" in line}
with open(os.path.join(corpus, "SIDE")) as handle:
    side = handle.read().strip()
cases = []
for path in sorted(build.glob("$(S)/tst/corpus/*/*.svg")):
    relative = path[len("$(S)/tst/corpus/"):]
    if relative in excluded or flags.filter and not fnmatch.fnmatch(relative, flags.filter):
        continue
    stem = path[:-len(".svg")]
    score = "$(B)/score/" + relative[:-len(".svg")] + ".txt"
    references = [f"{stem}.{label}.png" for label in labels]
    cases.append(command(
        name="case_" + hashlib.sha256(relative.encode()).hexdigest()[:16],
        inputs=[path, "$(S)/tst/corpus/SIDE", *references],
        outputs=[score],
        deps=[svg],
        cmd=["sh", "-c", '"$0" test "$1" "$2" "$3" "$4" "$5" || printf "mae 1 1\\nstruct 1 1\\naa 1 1\\nreference\\nours\\n" > "$2"', "$(B)/svg", path, score, side, *references],
        descr="SV",
        color="cyan",
    ))

for metric in ["meanrms", "maxrms", "struct", "aa", "all"]:
    group(metric, command(
        name="score_" + metric,
        inputs=["$(S)/tst/score.py", "$(S)/tst/corpus/EXCLUDED"],
        outputs=[f"$(B)/{metric}.txt"],
        deps=cases,
        cmd=["python3", "$(S)/tst/score.py", "$(S)/tst/corpus", "$(B)/score", flags.filter or "*", metric, f"$(B)/{metric}.txt"],
        descr="OK",
        color="light-green",
    ))
