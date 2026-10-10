#!/usr/bin/env python3
import fnmatch
import glob
import math
import os
import sys


def rms(values):
    return math.sqrt(sum(value * value for value in values) / len(values))


def main():
    if len(sys.argv) != 6 or sys.argv[4] not in ("meanrms", "maxrms"):
        sys.exit("usage: tst/score.py CORPUS_DIR SCORE_DIR FILTER meanrms|maxrms REPORT")
    corpus, scores, pattern, metric, report = sys.argv[1:]
    rows = []
    for path in sorted(glob.glob(os.path.join(corpus, "*", "*.svg"))):
        relative = os.path.relpath(path, corpus)
        if not fnmatch.fnmatch(relative, pattern):
            continue
        with open(os.path.join(scores, relative[:-len(".svg")] + ".txt")) as handle:
            errors = [float(word) for word in handle.read().split()]
        rows.append((relative, errors))
    errors = [error for _, values in rows for error in values]
    if not errors:
        sys.exit("no cases")
    rows.sort(key=lambda row: -rms(row[1]))
    if metric == "meanrms":
        total = rms(errors)
    else:
        total = rms(rows[0][1])
    with open(report, "w") as handle:
        handle.write(f"{total:.9f}\n")
        for relative, values in rows:
            handle.write(f"{rms(values):.6f} " + " ".join(f"{value:.6f}" for value in values) + f" {relative}\n")
    for relative, values in rows[:10]:
        print(f"{rms(values):.4f}", " ".join(f"{value:.4f}" for value in values), relative)
    if metric == "meanrms":
        print(f"meanrms {total:.9f} over {len(errors)} images of {len(rows)} cases")
    else:
        print(f"maxrms {total:.9f} at {rows[0][0]} of {len(rows)} cases")


if __name__ == "__main__":
    main()
