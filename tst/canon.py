#!/usr/bin/env python3
"""Draw the references tst/corpus/CANON overrides with the renderer itself.

Chromium draws every reference of the corpus (tst/reference.py); a case
CANON lists, where Chromium differs from the renderer on purpose and the
renderer was shown right, takes the renderer's own drawing instead, at the
sizes of its references.

usage: tst/canon.py SVG CORPUS_DIR
"""
import os
import struct
import subprocess
import sys
import zlib

LABELS = ["1x", "2x"]


def size(path):
    with open(path, "rb") as handle:
        return struct.unpack(">II", handle.read(24)[16:24])


def decode(path):
    with open(path, "rb") as handle:
        data = handle.read()
    position = 8
    idat = b""
    while position < len(data):
        length_, kind = struct.unpack(">I4s", data[position:position + 8])
        body = data[position + 8:position + 8 + length_]
        if kind == b"IHDR":
            width, height, depth, color = struct.unpack(">IIBB", body[:10])
        elif kind == b"IDAT":
            idat += body
        position += 12 + length_
    if depth != 8 or color not in (2, 6):
        sys.exit(f"{path}: unsupported png")
    channels = 4 if color == 6 else 3
    raw = zlib.decompress(idat)
    stride = width * channels
    out = bytearray(width * height * 4)
    previous = bytearray(stride)
    for y in range(height):
        kind = raw[y * (stride + 1)]
        line = bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for x in range(stride):
            left = line[x - channels] if x >= channels else 0
            up = previous[x]
            corner = previous[x - channels] if x >= channels else 0
            if kind == 1:
                line[x] = (line[x] + left) & 255
            elif kind == 2:
                line[x] = (line[x] + up) & 255
            elif kind == 3:
                line[x] = (line[x] + (left + up) // 2) & 255
            elif kind == 4:
                guess = left + up - corner
                pa, pb, pc = abs(guess - left), abs(guess - up), abs(guess - corner)
                line[x] = (line[x] + (left if pa <= pb and pa <= pc else up if pb <= pc else corner)) & 255
        for x in range(width):
            out[(y * width + x) * 4:(y * width + x) * 4 + channels] = line[x * channels:(x + 1) * channels]
            if channels == 3:
                out[(y * width + x) * 4 + 3] = 255
        previous = line
    return width, height, out


def ours(program, jobs, side):
    # Native snapshots need geometric evidence independent of their pixels.
    tests = os.path.dirname(os.path.abspath(__file__))
    for test in ["curve_area.py", "straight_curves.py", "circle_coverage.py"]:
        subprocess.run([sys.executable, os.path.join(tests, test), program], check=True)
    for svg, png, width, height in jobs:
        # A file the test draws straight at its reference size is drawn so.
        straight = subprocess.run([program, "size", svg], capture_output=True, text=True, check=True).stdout.rstrip().endswith(" straight")
        subprocess.run([program, "render", svg, png, str(width), str(height)] + ([] if straight else [str(side)]), check=True)
    source = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    commit = subprocess.run(["git", "-C", source, "rev-parse", "--short", "HEAD"], capture_output=True, text=True).stdout.strip()
    return f"svg at {commit}"


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: tst/canon.py SVG CORPUS_DIR")
    program, corpus = os.path.abspath(sys.argv[1]), os.path.abspath(sys.argv[2])
    with open(os.path.join(corpus, "SIDE")) as handle:
        side = int(handle.read())
    jobs = []
    with open(os.path.join(corpus, "CANON")) as handle:
        for line in handle:
            fields = [field.strip() for field in line.split(":", 2)]
            if len(fields) != 3:
                continue
            if fields[1] != "ours":
                sys.exit(f"{fields[0]}: CANON names {fields[1]}; only ours draws references")
            for label in LABELS:
                png = os.path.join(corpus, f"{fields[0]}.{label}.png")
                width, height = size(png)
                jobs.append((os.path.join(corpus, fields[0] + ".svg"), png, width, height))
    if not jobs:
        sys.exit(f"no cases in {os.path.join(corpus, 'CANON')}")
    version = ours(program, jobs, side)
    missing = [png for _, png, width, height in jobs if not os.path.exists(png) or size(png) != (width, height)]
    if missing:
        sys.exit("ours did not draw " + " ".join(missing))
    with open(os.path.join(corpus, "OURS"), "w") as handle:
        handle.write(version + "\n")
    print(f"drew {len(jobs) // len(LABELS)} cases with {version}")


if __name__ == "__main__":
    main()
