#!/usr/bin/env python3
import os
import struct
import subprocess
import sys

LABELS = ["1x", "2x"]


def size(path):
    with open(path, "rb") as handle:
        return struct.unpack(">II", handle.read(24)[16:24])


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: tst/inkscape.py INKSCAPE CORPUS_DIR")
    inkscape, corpus = sys.argv[1], os.path.abspath(sys.argv[2])
    cases = []
    with open(os.path.join(corpus, "INKSCAPE")) as handle:
        for line in handle:
            case = line.split(":", 1)[0].strip()
            if case:
                cases.append(case)
    jobs = []
    for case in cases:
        svg = os.path.join(corpus, case + ".svg")
        for label in LABELS:
            png = os.path.join(corpus, f"{case}.{label}.png")
            width, height = size(png)
            jobs.append(
                f"file-open:{svg}; export-type:png; export-filename:{png}; export-width:{width}; export-height:{height}; "
                "export-png-color-mode:RGBA_8; export-background-opacity:0; export-do; file-close\n"
            )
    subprocess.run([inkscape, "--shell"], input="".join(jobs) + "quit\n", text=True, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    version = subprocess.run([inkscape, "--version"], capture_output=True, text=True).stdout.strip().splitlines()[0]
    with open(os.path.join(corpus, "INKSCAPE_VERSION"), "w") as handle:
        handle.write(version + "\n")
    print(f"drew {len(cases)} cases with {version}")


if __name__ == "__main__":
    main()
