#!/usr/bin/env python3
"""Check that a drawing at another size lays the canvas's background out in its pixels.

The root's box is the raster (D39), its px the raster's pixels (D51): a
document drawn at another size lays its background images out as the same
document of that size of its own does at its own size, while its content
stretches. The two drawings are compared exactly, but for rounding."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon

WIDTH = 16
HEIGHT = 8
SCALES = [(2, 2), (3, 3), (2, 1), (1, 3)]
BACKGROUNDS = {
    "stripes": "background-color: white; background-size: 4px 4px; background-image: linear-gradient(to right, blue 1px, transparent 1px)",
    "placed": "background: linear-gradient(lime, lime) right 2px bottom 1px / 4px 4px no-repeat, linear-gradient(blue, blue) 0 0 / 2px 2px repeat-y white",
    "image": "background: url(&quot;data:image/svg+xml,%3Csvg xmlns=%22http://www.w3.org/2000/svg%22 width=%224%22 height=%222%22%3E%3Crect width=%222%22 height=%222%22 fill=%22lime%22/%3E%3C/svg%3E&quot;) 1px 3px",
}


def draw(program, source, output, width, height):
    subprocess.run([program, "render", str(source), str(output), str(width), str(height)], check=True, timeout=30)
    drawn = canon.decode(output)
    if drawn[:2] != (width, height):
        sys.exit(f"wrong image size {drawn[0]}x{drawn[1]}")
    return drawn[2]


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/canvas_scale.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix="svg-canvas-scale-") as work:
        source = Path(work) / "case.svg"
        output = Path(work) / "case.png"
        for name, style in BACKGROUNDS.items():
            for kx, ky in SCALES:
                width, height = WIDTH * kx, HEIGHT * ky
                source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{HEIGHT}" style="{style}"/>')
                scaled = draw(program, source, output, width, height)
                source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" style="{style}"/>')
                own = draw(program, source, output, width, height)
                worst = max(abs(a - b) for a, b in zip(scaled, own))
                if not any(own[i] < 100 and max(own[i + 1], own[i + 2]) > 200 for i in range(0, len(own), 4)):
                    failures.append(f"{name} at {kx}x{ky}: no background drawn")
                elif worst > 1:
                    failures.append(f"{name} at {kx}x{ky}: off by up to {worst}")
                checked += 1
    if failures:
        sys.exit(f"{len(failures)} failed cases:\n" + "\n".join(failures))
    print(f"checked {checked} canvases drawn at other sizes")


if __name__ == "__main__":
    main()
