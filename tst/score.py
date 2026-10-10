#!/usr/bin/env python3
import fnmatch
import glob
import math
import os
import sys

FAILING = 0.001
TAIL = 0.01
CANDIDATES = 40


def rms(values):
    return math.sqrt(sum(value * value for value in values) / len(values))


def load(corpus, scores, pattern):
    with open(os.path.join(corpus, "EXCLUDED")) as handle:
        excluded = {line.split(":")[0].strip() + ".svg" for line in handle if ":" in line}
    cases = []
    for path in sorted(glob.glob(os.path.join(corpus, "*", "*.svg"))):
        relative = os.path.relpath(path, corpus)
        if relative in excluded or not fnmatch.fnmatch(relative, pattern):
            continue
        fields = {"mae": [], "struct": [], "reference": [], "ours": []}
        with open(os.path.join(scores, relative[:-len(".svg")] + ".txt")) as handle:
            for line in handle:
                words = line.split()
                if words and words[0] in fields:
                    fields[words[0]] = [float(word) for word in words[1:]]
        cases.append({
            "case": relative,
            "mae": fields["mae"],
            "struct": fields["struct"],
            "reference": max(fields["reference"], default=0.0),
            "ours": max(fields["ours"], default=0.0),
            "rms": rms(fields["mae"]),
            "share": max(fields["struct"]),
        })
    if not cases:
        sys.exit("no cases")
    return cases


def meanrms(cases):
    return rms([error for case in cases for error in case["mae"]])


def maxrms(cases):
    return max(cases, key=lambda case: case["rms"])


def structural(cases):
    failing = [case for case in cases if case["share"] > FAILING]
    worst = sorted(cases, key=lambda case: -case["share"])[:max(1, math.ceil(len(cases) * TAIL))]
    return failing, sum(case["share"] for case in worst) / len(worst)


def suspicious(case):
    return case["reference"] > FAILING and case["ours"] <= FAILING


def row(case):
    return (f"{case['rms']:.6f} " + " ".join(f"{value:.6f}" for value in case["mae"])
            + " | " + " ".join(f"{value:.6f}" for value in case["struct"])
            + f" | {case['reference']:.6f} {case['ours']:.6f} {case['case']}")


def summary(cases):
    worst = maxrms(cases)
    failing, tail = structural(cases)
    images = sum(len(case["mae"]) for case in cases)
    return [
        f"meanrms {meanrms(cases):.9f} over {images} images of {len(cases)} cases",
        f"maxrms {worst['rms']:.9f} at {worst['case']}",
        f"struct {len(failing)} of {len(cases)} cases have over {FAILING:.1%} of their pixels wrong; the worst {TAIL:.0%} have {tail:.2%} wrong on average",
    ]


def candidates(cases):
    suspect = sorted((case for case in cases if suspicious(case)), key=lambda case: -case["reference"])
    failing, _ = structural(cases)
    failing = sorted((case for case in failing if not suspicious(case)), key=lambda case: -case["share"])
    sources = {}
    for case in failing:
        source = case["case"].split("/")[0]
        sources[source] = sources.get(source, 0) + 1
    lines = ["", "failing cases by source: " + ", ".join(f"{source} {count}" for source, count in sorted(sources.items(), key=lambda item: -item[1]))]
    lines += ["", f"wrong drawings, the most wrong pixels first (wrong share at each size, error):"]
    lines += [f"  {' '.join(f'{value:.2%}' for value in case['struct'])}  {case['rms']:.4f}  {case['case']}" for case in failing[:CANDIDATES]]
    lines += ["", f"references Chromium draws differently at 1x and 2x while ours agree, {len(suspect)} cases (wrong share between them, error):"]
    lines += [f"  {case['reference']:.2%}  {case['rms']:.4f}  {case['case']}" for case in suspect[:CANDIDATES]]
    return lines


def main():
    metrics = ("meanrms", "maxrms", "struct", "all")
    if len(sys.argv) != 6 or sys.argv[4] not in metrics:
        sys.exit("usage: tst/score.py CORPUS_DIR SCORE_DIR FILTER meanrms|maxrms|struct|all REPORT")
    corpus, scores, pattern, metric, report = sys.argv[1:]
    cases = load(corpus, scores, pattern)
    if metric == "struct":
        cases.sort(key=lambda case: -case["share"])
        failing, tail = structural(cases)
        head = f"{len(failing)} {tail:.9f}"
        lines = summary(cases)[2:]
    else:
        cases.sort(key=lambda case: -case["rms"])
        if metric == "meanrms":
            head = f"{meanrms(cases):.9f}"
            lines = summary(cases)[:1]
        elif metric == "maxrms":
            head = f"{maxrms(cases)['rms']:.9f}"
            lines = summary(cases)[1:2]
        else:
            head = "\n".join(summary(cases))
            lines = summary(cases) + candidates(cases)
    with open(report, "w") as handle:
        handle.write(head + "\n")
        if metric == "all":
            handle.write("\n".join(lines[3:]) + "\n\n")
        for case in cases:
            handle.write(row(case) + "\n")
    if metric != "all":
        for case in cases[:10]:
            print(row(case))
    print("\n".join(lines))


if __name__ == "__main__":
    main()
