#!/usr/bin/env python3
import fnmatch
import glob
import math
import os
import sys


def main():
    if len(sys.argv) != 5:
        sys.exit("usage: tst/score.py CORPUS_DIR SCORE_DIR FILTER REPORT")
    corpus, scores, pattern, report = sys.argv[1:]
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
    total = math.sqrt(sum(error * error for error in errors) / len(errors))
    rows.sort(key=lambda row: -max(row[1]))
    with open(report, "w") as handle:
        handle.write(f"{total:.9f}\n")
        for relative, values in rows:
            handle.write(" ".join(f"{value:.6f}" for value in values) + f" {relative}\n")
    for relative, values in rows[:10]:
        print(" ".join(f"{value:.4f}" for value in values), relative)
    print(f"score {total:.9f} over {len(errors)} images of {len(rows)} cases")


if __name__ == "__main__":
    main()
