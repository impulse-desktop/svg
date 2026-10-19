#!/usr/bin/env python3
"""Checks a unit against its expected drawing.

usage: oracle.py SVG UNIT.svg EXPECTED.svg OUT.txt

tst/unit/X.svg is drawn as the units are, by SVG's unit command at its own
size, and so is tst/expect/X.svg, what X should look like drawn with plain
means: plain values for computed ones, a path for a clip, a picture for a
formula. The two are compared as a unit is compared with the canon,
premultiplied, within 2 of 255, or within the tolerance the expected
drawing's root gives in data-tolerance. Every element of the expected
drawing with an id and a data-box, "x y width height", is a cell, the test
of a statement the ledgers link to; a pixel that differs is reported in the
cells whose box holds it. A cell may give a tolerance of its own, for what
it draws by a permitted approximation, in data-tolerance too. Writes each cell's verdict to OUT.txt and exits 1
on any difference.
"""
import os
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ElementTree

import canon


def draw(program, path, png):
    # The unit command draws the file and compares it with the png it has
    # just written, which always agrees.
    subprocess.run([program, "unit", path, png, png], check=True, timeout=60)

    return canon.decode(png)


def cells(path):
    found = []

    for element in ElementTree.parse(path).iter():
        name, box = element.get("id"), element.get("data-box")

        if name and box:
            x, y, w, h = (float(v) for v in box.split())
            found.append((name, x, y, w, h, float(element.get("data-tolerance", "0"))))

    return found


def main():
    if len(sys.argv) != 5:
        sys.exit("usage: oracle.py SVG UNIT.svg EXPECTED.svg OUT.txt")

    program, unit, expected, out = sys.argv[1:]
    tolerance = float(ElementTree.parse(expected).getroot().get("data-tolerance", "2"))
    boxes = cells(expected)
    lines = []

    with tempfile.TemporaryDirectory(prefix="svg-oracle-") as temporary:
        w, h, ours = draw(program, unit, os.path.join(temporary, "unit.png"))
        ew, eh, wanted = draw(program, expected, os.path.join(temporary, "expected.png"))

    if (w, h) != (ew, eh):
        sys.exit(f"{unit} is {w}x{h}, its expected drawing {ew}x{eh}")

    wrong = {name: [0, 0] for name, *_ in boxes}
    outside = 0

    for y in range(h):
        for x in range(w):
            i = (y * w + x) * 4
            a, b = ours[i:i + 4], wanted[i:i + 4]
            difference = max([abs(a[3] - b[3]) * 255] + [abs(a[c] * a[3] - b[c] * b[3]) for c in range(3)])

            if difference <= tolerance * 255:
                continue

            holding = [box for box in boxes if box[1] <= x + .5 < box[1] + box[3] and box[2] <= y + .5 < box[2] + box[4]]

            if difference <= max([tolerance] + [box[5] for box in holding]) * 255:
                continue

            for name, *_ in holding:
                wrong[name][0] += 1
                wrong[name][1] = max(wrong[name][1], (difference + 254) // 255)

            held = bool(holding)

            outside += not held

    for name, *_ in boxes:
        count, worst = wrong[name]
        lines.append(f"{name}: " + (f"{count} pixels differ, by up to {worst} of 255" if count else "ok"))

    if outside:
        lines.append(f"outside the cells: {outside} pixels differ")

    with open(out, "w") as handle:
        handle.write("".join(line + "\n" for line in lines))

    failed = outside or any(count for count, _ in wrong.values())

    if failed:
        sys.stderr.write(f"oracle: {unit} differs from its expected drawing\n" + "".join("    " + line + "\n" for line in lines if not line.endswith(": ok")))

    sys.exit(1 if failed else 0)


if __name__ == "__main__":
    main()
