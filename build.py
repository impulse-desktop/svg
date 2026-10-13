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
    "group": {"descr": "zero-based partition of the tests to run", "default": ""},
    "group_count": {"descr": "total number of test partitions", "default": ""},
})


def parse_test_partition():
    if bool(flags.group) != bool(flags.group_count):
        raise RuntimeError("-Dgroup and -Dgroup_count must be specified together")
    if not flags.group:
        return None
    try:
        group_index = int(flags.group)
        group_count = int(flags.group_count)
    except ValueError as error:
        raise RuntimeError("-Dgroup and -Dgroup_count must be integers") from error
    if group_count <= 0 or group_index < 0 or group_index >= group_count:
        raise RuntimeError("test partition requires 0 <= group < group_count and group_count > 0")
    return group_index, group_count


# A test falls in the partition that a hash of its name picks, so it stays
# there when other tests come and go.
def partition_tests(targets):
    if test_partition is None:
        return targets
    group_index, group_count = test_partition
    return [target for target in targets if int.from_bytes(hashlib.sha256(target.name.encode()).digest()[:8], "big") % group_count == group_index]


test_partition = parse_test_partition()

build.cxxflags += ["-std=c++26", "-O2", "-g"]

warning_flags = ["-Wall", "-Wextra", "-Werror"]

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
    srcs=["$(S)/svg.cpp", "$(S)/tst/main.cpp"],
    cflags=warning_flags,
    deps=[decode, png, system],
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
    srcs=["$(S)/svg.cpp", "$(S)/tst/fuzz.cpp"],
    cflags=warning_flags,
    cxxflags=["-fsanitize=fuzzer-no-link"],
    ldflags=fuzzer_ldflags,
    deps=[system],
)

install(svg)

# A check imports canon, which imports the reference modules, so every
# script under tst is an input of every check.
checks = []
for check in ["viewport_units", "cascade_priority"]:
    checks.append(command(
        name="check_" + check,
        inputs=build.glob("$(S)/tst/*.py"),
        outputs=[f"$(B)/checks/{check}.txt"],
        deps=[svg],
        cmd=["sh", "-c", 'python3 "$0" "$1" > "$2"', f"$(S)/tst/{check}.py", "$(B)/svg", f"$(B)/checks/{check}.txt"],
        descr="CK",
        color="light-green",
    ))

group("checks", *checks)


labels = ["1x", "2x"]
corpus = os.path.join(os.path.dirname(os.path.abspath(__file__)), "tst", "corpus")
with open(os.path.join(corpus, "EXCLUDED")) as handle:
    excluded = {line.split(":")[0].strip() + ".svg" for line in handle if ":" in line}
with open(os.path.join(corpus, "SIDE")) as handle:
    side = handle.read().strip()
cases = []
paths = sorted(build.glob("$(S)/tst/corpus/*/*.svg"))
sizes = {path: os.path.getsize(os.path.join(corpus, path[len("$(S)/tst/corpus/"):])) for path in paths}
for path in sorted(paths, key=lambda path: -sizes[path]):
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
        cmd=["sh", "-c", '"$0" test "$1" "$2" "$3" "$4" "$5" || printf "mae 1 1\\nstruct 1 1\\naa 1\\nreference\\nours\\n" > "$2"', "$(B)/svg", path, score, side, *references],
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

# Every case and check, or one partition of them with -Dgroup and
# -Dgroup_count: the sanitizer runs of CI split them this way.
group("tests", *partition_tests(cases + checks))
