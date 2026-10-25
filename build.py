import fnmatch
import hashlib
import os
from pathlib import Path
import platform
import runpy

import build
import build.flags as flags


flags.allow({
    "filter": {"descr": "glob over the cases, as paths under tst/corpus (resvg/*, */*gradient*)", "default": ""},
    "group": {"descr": "zero-based partition of the tests to run", "default": ""},
    "group_count": {"descr": "total number of test partitions", "default": ""},
    "perf_side": {"descr": "longest side for the corpus performance comparison", "default": "512"},
    "perf_repeat": {"descr": "renders per file and binary, using their median", "default": "3"},
    "perf_timeout": {"descr": "seconds allowed for one file and binary", "default": "120"},
    "perf_jobs": {"descr": "parallel old/new file pairs in the performance comparison", "default": "24"},
    "perf_run": {"descr": "change this label to repeat an otherwise cached performance run", "default": "default"},
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

# svg.cpp asks x86-64 for SSE4.2: its pixel kernels take 16-bit lanes eight
# at a time and multiply 32-bit ones.
if platform.machine() in ("x86_64", "AMD64"):
    build.cxxflags += ["-march=x86-64-v2"]

warning_flags = ["-Wall", "-Wextra", "-Werror"]

png = pkg_config("libpng")
system = dependency(ldflags=["-lm"])

jpeg = pkg_config("libjpeg")
zlib = pkg_config("zlib")

svg = program(
    name="svg",
    output="$(B)/svg",
    srcs=["$(S)/svg.cpp", "$(S)/tst/main.cpp"],
    cflags=warning_flags,
    deps=[png, jpeg, zlib, system],
)

# Frozen last-green renderer; only the implementation differs between these
# binaries. The driver, decoders, compiler and optimization flags are shared.
svg_old = program(
    name="svg_old",
    output="$(B)/svg_old",
    srcs=["$(S)/svg_old.cpp", "$(S)/tst/main.cpp"],
    cflags=warning_flags,
    deps=[png, jpeg, zlib, system],
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
for check in ["viewport_units", "canvas_scale", "cascade_priority", "curve_area", "straight_curves", "circle_coverage", "perf_check", "scale_check", "svgz_check"]:
    checks.append(command(
        name="check_" + check,
        inputs=build.glob("$(S)/tst/*.py"),
        outputs=[f"$(B)/checks/{check}.txt"],
        deps=[svg],
        cmd=["sh", "-c", 'python3 "$0" "$1" > "$2"', f"$(S)/tst/{check}.py", "$(B)/svg", f"$(B)/checks/{check}.txt"],
        descr="CK",
        color="light-green",
    ))

# The C API from C99 against svg.cpp as C++17, the standards a program that
# copies the two files in builds them at.
c_api_main = library(
    name="c_api_main",
    srcs=["$(S)/tst/c_api.c"],
    cflags=["-std=c99", "-Wpedantic", *warning_flags],
)
c_api = program(
    name="c_api",
    output="$(B)/c_api",
    srcs=["$(S)/svg.cpp"],
    cflags=warning_flags,
    cxxflags=["-std=c++17", "-Wpedantic"],
    deps=[c_api_main, system],
)
checks.append(command(
    name="check_c_api",
    outputs=["$(B)/checks/c_api.txt"],
    deps=[c_api],
    cmd=["sh", "-c", '"$0" > "$1"', "$(B)/c_api", "$(B)/checks/c_api.txt"],
    descr="CK",
    color="light-green",
))

decompress_api = program(
    name="decompress_api",
    output="$(B)/decompress_api",
    srcs=["$(S)/svg.cpp", "$(S)/tst/decompress_api.cpp"],
    cflags=warning_flags,
    deps=[system],
)
checks.append(command(
    name="check_decompress_api",
    outputs=["$(B)/checks/decompress_api.txt"],
    deps=[decompress_api],
    cmd=["sh", "-c", '"$0" > "$1"', "$(B)/decompress_api", "$(B)/checks/decompress_api.txt"],
    descr="CK",
    color="light-green",
))

parser_api = program(
    name="parser_api",
    output="$(B)/parser_api",
    srcs=["$(S)/svg.cpp", "$(S)/tst/parser_api.cpp"],
    cflags=warning_flags,
    deps=[system],
)
checks.append(command(
    name="check_parser_api",
    outputs=["$(B)/checks/parser_api.txt"],
    deps=[parser_api],
    cmd=["sh", "-c", '"$0" > "$1"', "$(B)/parser_api", "$(B)/checks/parser_api.txt"],
    descr="CK",
    color="light-green",
))

render_limits = program(
    name="render_limits",
    output="$(B)/render_limits",
    srcs=["$(S)/svg.cpp", "$(S)/tst/render_limits.cpp"],
    cflags=warning_flags,
    deps=[system],
)
checks.append(command(
    name="check_render_limits",
    outputs=["$(B)/checks/render_limits.txt"],
    deps=[render_limits],
    cmd=["sh", "-c", '"$0" > "$1"', "$(B)/render_limits", "$(B)/checks/render_limits.txt"],
    descr="CK",
    color="light-green",
))

finite_paints = program(
    name="finite_paints",
    output="$(B)/finite_paints",
    srcs=["$(S)/tst/finite_paints.cpp"],
    cflags=warning_flags,
    deps=[system],
)
checks.append(command(
    name="check_finite_paints",
    outputs=["$(B)/checks/finite_paints.txt"],
    deps=[finite_paints],
    cmd=["sh", "-c", '"$0" > "$1"', "$(B)/finite_paints", "$(B)/checks/finite_paints.txt"],
    descr="CK",
    color="light-green",
))

# The statement ledgers: anchors in dev/spec, units, and the test cells
# every statement links to in the expected drawings.
checks.append(command(
    name="check_ledger",
    inputs=[
        *build.glob("$(S)/tst/spec/statements/*.md"),
        *build.glob("$(S)/tst/spec/statements/*/*.md"),
        *build.glob("$(S)/dev/spec/*.md"),
        *build.glob("$(S)/dev/spec/*/*.md"),
        *build.glob("$(S)/tst/unit/*.svg"),
        *build.glob("$(S)/tst/unit/*.svgz"),
        *build.glob("$(S)/tst/expect/*.svg"),
        "$(S)/tst/spec/ledger.py",
    ],
    outputs=["$(B)/checks/ledger.txt"],
    cmd=["sh", "-c", 'python3 "$0" > "$1"', "$(S)/tst/spec/ledger.py", "$(B)/checks/ledger.txt"],
    descr="CK",
    color="light-green",
))

group("checks", *checks)

# tst/sanitizer.svg takes every kind of paint, clip, mask, marker and filter
# through one small drawing.
smoke = command(
    name="smoke",
    inputs=["$(S)/tst/sanitizer.svg"],
    outputs=["$(B)/smoke.png"],
    deps=[svg],
    cmd=["sh", "-c", '"$0" render "$1" "$2" 192 128 && test -s "$2"', "$(B)/svg", "$(S)/tst/sanitizer.svg", "$(B)/smoke.png"],
    descr="SM",
    color="light-green",
)


# The frozen implementation draws the references without instrumentation.
# Recanonizing replaces svg_canon.cpp with the accepted svg.cpp.
root = os.path.dirname(os.path.abspath(__file__))
canon = import_build("$(S)/tst/canon_build.py", "svg", namespace="canon")

labels = ["1x", "2x"]
corpus = os.path.join(root, "tst", "corpus")
excluded = set()
for listed in ["EXCLUDED", "DESIGN"]:
    with open(os.path.join(corpus, listed)) as handle:
        excluded |= {line.split(":")[0].strip() + ".svg" for line in handle if ":" in line}
with open(os.path.join(corpus, "SIDE")) as handle:
    side = handle.read().strip()
cases = []
scale_cases = []
paths = sorted(build.glob("$(S)/tst/corpus/*/*.svg"))
sizes = {path: os.path.getsize(os.path.join(corpus, path[len("$(S)/tst/corpus/"):])) for path in paths}
for path in sorted(paths, key=lambda path: -sizes[path]):
    relative = path[len("$(S)/tst/corpus/"):]
    if relative in excluded or flags.filter and not fnmatch.fnmatch(relative, flags.filter):
        continue
    score = "$(B)/score/" + relative[:-len(".svg")] + ".txt"
    references = [f"$(B)/canon/corpus/{relative[:-len('.svg')]}.{label}.png" for label in labels]
    key = hashlib.sha256(relative.encode()).hexdigest()[:16]
    # Each file is drawn exactly twice by the current renderer. Scores use
    # the ordinary aggregator; failures remain visible at the top of it.
    scale_cases.append(command(
        name="scale_case_" + key,
        inputs=[path],
        outputs=[f"$(B)/scale/score/{relative[:-len('.svg')]}.txt"],
        deps=[svg],
        cmd=["sh", "-c", '"$0" scale "$1" "$2" || { status=$?; printf "mae 1\\nstruct 1\\naa 1\\nreference\\nours\\nerror exit %s\\n" "$status" > "$2"; }',
             "$(B)/svg", path, f"$(B)/scale/score/{relative[:-len('.svg')]}.txt"],
        descr="SC",
        color="cyan",
    ))
    drawn = command(
        name="reference_" + key,
        inputs=[path, "$(S)/tst/corpus/SIDE", "$(S)/tst/canon.py"],
        outputs=references,
        deps=[canon],
        cmd=["python3", "$(S)/tst/canon.py", "$(B)/canon/svg", path, side, *references],
        descr="RF",
        color="magenta",
    )
    cases.append(command(
        name="case_" + key,
        inputs=[path, "$(S)/tst/corpus/SIDE", *references],
        outputs=[score],
        deps=[svg, drawn],
        cmd=["sh", "-c", '"$0" test "$1" "$2" "$3" "$4" "$5" || printf "mae 1 1\\nstruct 1 1\\naa 1\\nreference\\nours\\n" > "$2"', "$(B)/svg", path, score, side, *references],
        descr="SV",
        color="cyan",
    ))

# Sanitizer shards already compute every corpus comparison. Publish their
# scores separately even when an unrelated unit made the tests group fail.
group("score_shard", *partition_tests(cases))

# CI downloads the five disjoint score artifacts here. Missing comparisons
# fail in score.py; aggregation never schedules another renderer invocation.
group("score_collected", command(
    name="collect_scores",
    inputs=[
        "$(S)/tst/score.py", "$(S)/tst/corpus/EXCLUDED", "$(S)/tst/corpus/DESIGN",
        *paths, *build.glob("$(S)/.build-collected/score/*/*.txt"),
    ],
    outputs=[f"$(B)/collected/{metric}.txt" for metric in ["all", "meanrms", "maxrms", "struct", "aa"]],
    cmd=["python3", "$(S)/tst/score.py", "$(S)/tst/corpus", "$(S)/.build-collected/score", flags.filter or "*", "reports", "$(B)/collected"],
    descr="OK",
    color="light-green",
))

for metric in ["meanrms", "maxrms", "struct", "aa", "all"]:
    group(metric, command(
        name="score_" + metric,
        inputs=["$(S)/tst/score.py", "$(S)/tst/corpus/EXCLUDED", "$(S)/tst/corpus/DESIGN"],
        outputs=[f"$(B)/{metric}.txt"],
        deps=cases,
        cmd=["python3", "$(S)/tst/score.py", "$(S)/tst/corpus", "$(B)/score", flags.filter or "*", metric, f"$(B)/{metric}.txt"],
        descr="OK",
        color="light-green",
    ))


# Prepare and read the per-file scores once for all rankings.
scale_report = command(
    name="scale_score",
    inputs=["$(S)/tst/score.py", "$(S)/tst/corpus/EXCLUDED", "$(S)/tst/corpus/DESIGN"],
    outputs=[f"$(B)/scale/{metric}.txt" for metric in ["all", "meanrms", "maxrms", "struct", "aa"]],
    deps=scale_cases,
    cmd=["python3", "$(S)/tst/score.py", "$(S)/tst/corpus", "$(B)/scale/score", flags.filter or "*", "reports", "$(B)/scale"],
    descr="OK",
    color="light-green",
)
group("scale", scale_report)
for metric in ["meanrms", "maxrms", "struct", "aa", "all"]:
    group("scale_" + metric, scale_report)

# Every tst/unit/X.svg or X.svgz is drawn at its own size and compared with the
# canon's drawing of it, $(B)/canon/unit/X.png, which the canon's svg makes
# as the same command, against itself; ours is left in $(B)/unit/X.png.
units = []
unit_paths = {Path(path).stem: path for path in build.glob("$(S)/tst/unit/*.svg") + build.glob("$(S)/tst/unit/*.svgz")}
for name, path in sorted(unit_paths.items()):
    # Frozen CANON has no decompressor. For the new SVGZ units it draws
    # the independent, plain-rectangle expectation instead of the input.
    reference_source = f"$(S)/tst/expect/{name}.svg" if name.startswith(("svgz-", "svgz.")) else path
    reference = f"$(B)/canon/unit/{name}.png"
    drawn = command(
        name="unit_reference_" + name,
        inputs=[reference_source],
        outputs=[reference],
        deps=[canon],
        cmd=["$(B)/canon/svg", "unit", reference_source, reference, reference],
        descr="RF",
        color="magenta",
    )
    units.append(command(
        name="unit_" + name,
        inputs=[path, reference],
        outputs=[f"$(B)/unit/{name}.png"],
        deps=[svg, drawn],
        cmd=["$(B)/svg", "unit", path, reference, f"$(B)/unit/{name}.png"],
        descr="UT",
        color="light-green",
    ))

# Every tst/expect/X.svg is what tst/unit/X.svg should look like drawn with
# plain means; tst/oracle.py compares the two, so a unit checks the
# statements its cells test, not only that it draws as the canon does.
oracles = []
for path in sorted(build.glob("$(S)/tst/expect/*.svg")):
    name = os.path.basename(path[:-len(".svg")])
    oracles.append(command(
        name="oracle_" + name,
        inputs=[path, unit_paths[name], "$(S)/tst/oracle.py", "$(S)/tst/canon.py"],
        outputs=[f"$(B)/oracle/{name}.txt"],
        deps=[svg],
        cmd=["python3", "$(S)/tst/oracle.py", "$(B)/svg", unit_paths[name], path, f"$(B)/oracle/{name}.txt"],
        descr="OR",
        color="light-green",
    ))

# API failure and root gzip stream tests are units too, including in the
# coverage run, which invokes this group rather than the whole corpus.
group("unit", *units, *oracles, *(check for check in checks if check.name in {"check_c_api", "check_decompress_api", "check_parser_api", "check_render_limits", "check_finite_paints", "check_svgz_check"}))

# Every case, check and unit and the smoke drawing, or one partition of them
# with -Dgroup and -Dgroup_count: the sanitizer runs of CI split them this way.
group("tests", *partition_tests(cases + checks + units + oracles + [smoke]))

# Keep timings separate from sanitizer shards and image-reference generation.
# The measurement succeeds even when a renderer fails: the following gate
# rejects incomplete measurements, and the report remains cached for inspection.
# Quick diagnosis uses one sample on a fixed third of the eligible corpus:
# nine times fewer timed renders than the default full comparison.
perf_selection = runpy.run_path(os.path.join(root, "tst", "perf.py"))["selection"]
for name, repeat, stride in [("perf", flags.perf_repeat, "1"), ("perf_quick", "1", "3")]:
    report = f"$(B)/{name}/report.json"
    perf_inputs = ["$(S)/tst/corpus/" + path.relative_to(corpus).as_posix()
                   for path in perf_selection(Path(corpus), flags.filter or "*", int(stride))]
    measured = command(
        name=name + "_report",
        inputs=["$(S)/tst/perf.py", *perf_inputs, "$(S)/tst/corpus/EXCLUDED", "$(S)/tst/corpus/DESIGN"],
        outputs=[report],
        deps=[svg_old, svg],
        cmd=["python3", "$(S)/tst/perf.py", "measure", "$(B)/svg_old", "$(B)/svg", "$(S)/tst/corpus", report,
             "--side", flags.perf_side, "--repeat", repeat, "--stride", stride, "--timeout", flags.perf_timeout,
             "--jobs", flags.perf_jobs, "--filter", flags.filter or "*", "--run", flags.perf_run],
        descr="PF",
        color="light-green",
    )
    command(
        name=name,
        inputs=["$(S)/tst/perf.py", report],
        outputs=[f"$(B)/{name}/check.txt"],
        deps=[measured],
        cmd=["python3", "$(S)/tst/perf.py", "check", report, f"$(B)/{name}/check.txt"],
        descr="PF",
        color="light-green",
    )
