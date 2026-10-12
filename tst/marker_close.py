#!/usr/bin/env python3
"""Check automatic marker directions at ordinary and zero-length closes."""
import math
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def drawing(path, kind, orient):
    return f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 200 200">
  <marker id="arrow" markerUnits="userSpaceOnUse" markerWidth="20" markerHeight="20"
          refX="0" refY="0" orient="{orient}" overflow="visible">
    <path d="M 0 -4 L 12 0 L 0 4 Z"/>
  </marker>
  <path d="{path}" fill="none" stroke="none" marker-{kind}="url(#arrow)"/>
</svg>'''


def cases():
    # A nonzero closing edge and the first edge determine the usual bisector.
    # A zero-length closing edge instead takes the first edge's tangent;
    # repeated closes must not retain the preceding closing edge's direction.
    for name, path, outgoing in [
        ("line", "M 100 100 L 140 40 L 170 140", math.degrees(math.atan2(-60, 40))),
        ("cubic", "M 100 100 C 100 40 160 40 160 100 L 170 140", -90),
    ]:
        incoming = math.degrees(math.atan2(-40, -70))
        yield name + "/ordinary", path + " Z", (incoming + outgoing) / 2
        yield name + "/two-closes", path + " Z Z", outgoing
        yield name + "/three-closes", path + " Z Z Z", outgoing
        yield name + "/return-before-close", path + " L 100 100 Z", outgoing
    yield "point", "M 100 100 Z", 0


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/marker_close.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix="svg-marker-close-") as work:
        work = Path(work)
        for name, path, angle in cases():
            for kind in ["start", "end"]:
                for side in [256, 512]:
                    pixels = []
                    for label, orient in [("automatic", "auto"), ("explicit", str(angle))]:
                        source, output = work / (label + ".svg"), work / (label + ".png")
                        source.write_text(drawing(path, kind, orient))
                        subprocess.run([program, "render", str(source), str(output), str(side), str(side), "1024"],
                                       check=True, timeout=30)
                        width, height, rgba = canon.decode(output)
                        if (width, height) != (side, side) or not any(rgba[3::4]):
                            sys.exit(f"{name}/{kind}/{side}: missing marker")
                        pixels.append(rgba)
                    if any(abs(a-b) > 1 for a, b in zip(*pixels)):
                        failures.append(f"{name}/{kind}/{side}: automatic direction differs from {angle:g} degrees")
                    checked += 1
    if failures:
        sys.exit("\n".join(failures))
    print(f"checked {checked} closing marker directions")


if __name__ == "__main__":
    main()
