#!/usr/bin/env python3
"""Compare the corpus's total parse-and-render time with the frozen renderer.

Both executables use the same driver, raster decoders and build flags. Each
file uses identical dimensions, derived outside the timer with the current
renderer. Native timers exclude process startup, file reads and PNG output.
Fresh parse/render/destroy timings are summed across files. The full test
uses three samples and their median; the quick test uses one sample and
every third eligible file in sorted path order.
"""
import argparse
import concurrent.futures
import fnmatch
import hashlib
import json
import math
from pathlib import Path
import subprocess
import tempfile
import time


LIMIT_NUMERATOR = 105
LIMIT_DENOMINATOR = 100
# Times out in both versions; excluded only from the performance comparison.
PERF_EXCLUDED = {"gecko/slow__layout__reftests__svg__explosive-use.svg"}


def digest(path):
    with open(path, "rb") as source:
        return hashlib.file_digest(source, "sha256").hexdigest()


def selection(corpus, pattern, stride=1):
    if stride < 1:
        raise ValueError("stride must be positive")
    excluded = set(PERF_EXCLUDED)
    for name in ("EXCLUDED", "DESIGN"):
        for line in (corpus / name).read_text().splitlines():
            if ":" in line:
                excluded.add(line.split(":", 1)[0].strip() + ".svg")
    paths = sorted(corpus.glob("*/*.svg"))
    eligible = [p for p in paths if p.relative_to(corpus).as_posix() not in excluded
                and fnmatch.fnmatch(p.relative_to(corpus).as_posix(), pattern)]
    return eligible[::stride]


def run_process(argv, timeout):
    try:
        result = subprocess.run(argv, capture_output=True, text=True, timeout=timeout)
    except subprocess.TimeoutExpired:
        return None, f"timeout after {timeout:g}s"
    if result.returncode:
        return None, f"exit {result.returncode}: {result.stderr.strip()[:500]}"
    return result.stdout, None


def measure_case(index, path, args):
    case = {"file": path.relative_to(args.corpus).as_posix(), "sha256": digest(path)}
    # Whitespace is not representable in the native LIST protocol. Do not
    # silently benchmark a different file or accept a partial list.
    if any(c.isspace() for c in str(path)) or len(str(path).encode()) >= 3999:
        return {**case, "error": "path cannot be represented in the benchmark list"}
    stdout, error = run_process([args.new, "size", str(path)], args.timeout)
    if error:
        return {**case, "error": "size: " + error}
    try:
        fields = stdout.split()
        width, height = map(float, fields[1:3])
        if not all(math.isfinite(n) and n > 0 for n in (width, height)):
            raise ValueError("nonpositive or nonfinite size")
        scale = args.side / max(width, height)
        width, height = max(1, round(width * scale)), max(1, round(height * scale))
    except (ValueError, IndexError):
        return {**case, "error": "renderer did not report a finite positive size"}
    case.update(width=width, height=height)
    with tempfile.NamedTemporaryFile(mode="w", suffix=".list") as manifest:
        manifest.write(f"{path} {width} {height}\n")
        manifest.flush()
        # Alternate which binary goes first, so the old one does not always
        # pay for cold input pages or precede a thermally warmer machine.
        order = ("old", "new") if index % 2 == 0 else ("new", "old")
        for version in order:
            stdout, error = run_process([getattr(args, version), "bench", manifest.name, str(args.repeat)], args.timeout)
            if error:
                case[version] = {"error": error}
                continue
            try:
                lines = stdout.splitlines()
                if len(lines) != 1:
                    raise ValueError("expected one result")
                name, elapsed, ok = lines[0].rsplit(" ", 2)
                elapsed = int(elapsed)
                if name != str(path) or ok != "1" or elapsed <= 0:
                    raise ValueError("incomplete render")
                case[version] = {"ns": elapsed}
            except ValueError:
                case[version] = {"error": "invalid or unsuccessful benchmark result: " + stdout[:300]}
    return case


def totals(cases):
    complete = [r for r in cases if all(r.get(v, {}).get("ns", 0) > 0 for v in ("old", "new"))]
    old = sum(r["old"]["ns"] for r in complete)
    new = sum(r["new"]["ns"] for r in complete)
    return {"complete": len(complete), "errors": len(cases) - len(complete),
            "old_ns": old, "new_ns": new, "ratio": new / old if old else None}


def accepted(report):
    cases = report["cases"]
    total = totals(cases)
    return (bool(cases) and len(cases) == report["selected"] and not total["errors"]
            and total["old_ns"] > 0
            and total["new_ns"] * LIMIT_DENOMINATOR <= total["old_ns"] * LIMIT_NUMERATOR)


def summary(report):
    total = totals(report["cases"])
    ratio = f'{total["ratio"]:.4f}x ({(total["ratio"] - 1) * 100:+.2f}%)' if total["ratio"] is not None else "n/a"
    scope = "FULL CORPUS" if report["filter"] == "*" else "SUBSET " + report["filter"]
    stride = report.get("stride", 1)
    if stride > 1:
        scope = f"SAMPLE index % {stride} == 0 of " + scope
    sampling = "one sample" if report["repeat"] == 1 else f"median of {report['repeat']}"
    lines = [f"perf {scope}: {len(report['cases'])}/{report['selected']} files; {total['errors']} errors",
             f"parse + render + destroy; side {report['side']}; {sampling}; {report['jobs']} workers",
             f"old {total['old_ns'] / 1e9:.6f}s; new {total['new_ns'] / 1e9:.6f}s; {ratio}; limit 1.05x",
             "PASS" if accepted(report) else "FAIL"]
    if total["errors"]:
        lines.append("Totals above cover successful pairs only; the selected comparison is incomplete and cannot pass.")
    for r in report["cases"]:
        errors = [r["error"]] if "error" in r else [f"{v}: {r[v]['error']}" for v in ("old", "new") if "error" in r.get(v, {})]
        if errors:
            lines.append(r["file"] + ": " + "; ".join(errors))
    complete = [r for r in report["cases"] if "ns" in r.get("old", {}) and "ns" in r.get("new", {})]
    lines.append("Largest increases in absolute time:")
    for r in sorted(complete, key=lambda r: r["new"]["ns"] - r["old"]["ns"], reverse=True)[:20]:
        old, new = r["old"]["ns"], r["new"]["ns"]
        lines.append(f"  {(new - old) / 1e6:+.3f}ms  {old / 1e6:.3f} -> {new / 1e6:.3f}ms  {r['file']}")
    return "\n".join(lines) + "\n"


def measure(args):
    args.corpus = args.corpus.resolve()
    args.old, args.new = str(Path(args.old).resolve()), str(Path(args.new).resolve())
    if args.side < 1 or args.side > 4096 or args.repeat < 1 or args.repeat > 31 or args.repeat % 2 != 1 or args.timeout <= 0 or args.jobs < 1:
        raise ValueError("side must be 1..4096, repeat odd in 1..31, timeout and jobs positive")
    paths = selection(args.corpus, args.filter, args.stride)
    if not paths:
        raise ValueError("no corpus files selected")
    jobs = min(args.jobs, len(paths))
    started = time.monotonic()
    # Scatter providers deterministically, retaining stable old/new ordering.
    paths.sort(key=lambda p: hashlib.sha256(p.relative_to(args.corpus).as_posix().encode()).digest())
    report = {"version": 2, "selected": len(paths), "filter": args.filter, "side": args.side,
              "stride": args.stride, "selection": "sorted eligible paths, zero-based index % stride == 0",
              "repeat": args.repeat, "timeout": args.timeout, "jobs": jobs, "run": args.run,
              "limit": 1.05, "old_sha256": digest(args.old), "new_sha256": digest(args.new), "cases": []}
    print(f"Comparing {len(paths)} corpus files, side {args.side}, {args.repeat} samples, {jobs} workers", flush=True)
    with concurrent.futures.ThreadPoolExecutor(max_workers=jobs) as pool:
        futures = [pool.submit(measure_case, i, path, args) for i, path in enumerate(paths)]
        for future in concurrent.futures.as_completed(futures):
            report["cases"].append(future.result())
            done = len(report["cases"])
            if done % 1000 == 0 or done == len(paths):
                print(f"perf {done}/{len(paths)} in {time.monotonic() - started:.1f}s", flush=True)
    report["cases"].sort(key=lambda r: r["file"])
    report["elapsed_seconds"] = time.monotonic() - started
    report["totals"] = totals(report["cases"])
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2) + "\n")
    print(summary(report), flush=True)
    # Preserve the report in build's CAS even if the subsequent gate fails.
    return 0


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest="command", required=True)
    run = sub.add_parser("measure")
    run.add_argument("old")
    run.add_argument("new")
    run.add_argument("corpus", type=Path)
    run.add_argument("output", type=Path)
    run.add_argument("--side", type=int, default=512)
    run.add_argument("--repeat", type=int, default=3)
    run.add_argument("--stride", type=int, default=1)
    run.add_argument("--timeout", type=float, default=120)
    run.add_argument("--jobs", type=int, default=24)
    run.add_argument("--filter", default="*")
    run.add_argument("--run", default="default")
    check = sub.add_parser("check")
    check.add_argument("report", type=Path)
    check.add_argument("output", type=Path)
    args = parser.parse_args()
    if args.command == "measure":
        return measure(args)
    report = json.loads(args.report.read_text())
    text = summary(report)
    print(text, end="", flush=True)
    if not accepted(report):
        target = "perf_quick_report" if report.get("stride", 1) == 3 and report["repeat"] == 1 else "perf_report"
        print(f"The report is retained in build's cache; use the same flags with ./build {target} to publish it.")
        return 1
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(text)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
