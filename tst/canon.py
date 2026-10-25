#!/usr/bin/env python3
"""Draw a case's references with the canon's svg.

The canon is the frozen implementation in svg_canon.cpp. build.py builds it
with the current test driver and draws every reference a case is scored
against, at the sizes Chromium's references had, the longest side 256 and
512, at SIDE pixels a side unless the case is drawn straight.

usage: tst/canon.py SVG CASE.svg SIDE OUT_1X.png OUT_2X.png
"""
import struct
import subprocess
import sys
import zlib

SIDES = [256, 512]


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


def main():
    if len(sys.argv) != 6:
        sys.exit("usage: tst/canon.py SVG CASE.svg SIDE OUT_1X.png OUT_2X.png")
    program, case, side, outs = sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4:]
    fields = subprocess.run([program, "size", case], capture_output=True, text=True, check=True).stdout.split()
    width, height = float(fields[1]), float(fields[2])
    straight = len(fields) > 3 and fields[3] == "straight"
    longest = max(width, height)
    for out, target in zip(outs, SIDES):
        scale = target / longest
        size = [str(max(1, round(width * scale))), str(max(1, round(height * scale)))]
        subprocess.run([program, "render", case, out, *size] + ([] if straight else [side]), check=True)


if __name__ == "__main__":
    main()
