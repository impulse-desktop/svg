import fnmatch
import hashlib

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
jpeg = pkg_config("libjpeg")
system = dependency(ldflags=["-lm"])

svg = program(
    name="svg",
    output="$(B)/svg",
    srcs=["$(S)/svg.cpp", "$(S)/main.cpp"],
    cflags=warning_flags,
    deps=[libstd, png, jpeg, system],
)

install(svg)


labels = ["1x", "2x"]
cases = []
for path in sorted(build.glob("$(S)/tst/corpus/*/*.svg")):
    relative = path[len("$(S)/tst/corpus/"):]
    if flags.filter and not fnmatch.fnmatch(relative, flags.filter):
        continue
    stem = path[:-len(".svg")]
    score = "$(B)/score/" + relative[:-len(".svg")] + ".txt"
    references = [f"{stem}.{label}.png" for label in labels]
    cases.append(command(
        name="case_" + hashlib.sha256(relative.encode()).hexdigest()[:16],
        inputs=[path, *references],
        outputs=[score],
        deps=[svg],
        cmd=["sh", "-c", '"$0" test "$1" "$2" "$3" "$4" || echo 1 1 > "$2"', "$(B)/svg", path, score, *references],
        descr="SV",
        color="cyan",
    ))

group("test", command(
    name="score",
    inputs=["$(S)/tst/score.py"],
    outputs=["$(B)/score.txt"],
    deps=cases,
    cmd=["python3", "$(S)/tst/score.py", "$(S)/tst/corpus", "$(B)/score", flags.filter or "*", "$(B)/score.txt"],
    descr="OK",
    color="light-green",
))
