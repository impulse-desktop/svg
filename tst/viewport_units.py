#!/usr/bin/env python3
"""Check viewport units inside nested viewports and markers under filtered layers.

Viewport units are in the pixels of the box, so the drawings are checked
at their own size, never supersampled."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon

WIDTH = 120
HEIGHT = 80
SHAPE = '<rect width="50vw" height="25vh"/>'
AREA = WIDTH / 2 * HEIGHT / 4
FILTER = f'<filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="{WIDTH}" height="{HEIGHT}"><feOffset/></filter>'

# Each container places the shape at 30,30, so that every transform below
# keeps it inside the canvas.
CONTAINERS = {
    "svg": ("", f'<svg x="30" y="30" width="60" height="20" overflow="visible">{SHAPE}</svg>'),
    "symbol": (f'<symbol id="s" overflow="visible">{SHAPE}</symbol>', '<use href="#s" x="30" y="30" width="60" height="20"/>'),
    "marker": (f'<marker id="m" markerUnits="userSpaceOnUse" markerWidth="60" markerHeight="20" overflow="visible">{SHAPE}</marker>',
               '<path d="M 30 30 h 1" stroke="none" marker-start="url(#m)"/>'),
}

# A rotated or reflected element is filtered on an intermediate layer whose
# size is not the canvas; its viewport units still refer to the canvas.
TRANSFORMS = ["", "rotate(90 60 40)", "rotate(180 60 40)", "rotate(30 60 40)", "matrix(-1 0 0 1 120 0)"]


def drawing(definitions, content, transform, filtered):
    attributes = (f' transform="{transform}"' if transform else "") + (' filter="url(#f)"' if filtered else "")
    return (f'<svg xmlns="http://www.w3.org/2000/svg" width="{WIDTH}" height="{HEIGHT}">'
            f'<defs>{FILTER}{definitions}</defs><g{attributes}>{content}</g></svg>')


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/viewport_units.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix="svg-viewport-units-") as work:
        source = Path(work) / "case.svg"
        output = Path(work) / "case.png"
        for name, (definitions, content) in CONTAINERS.items():
            for transform in TRANSFORMS:
                for filtered in [False, True]:
                    source.write_text(drawing(definitions, content, transform, filtered))
                    subprocess.run([program, "render", str(source), str(output), str(WIDTH), str(HEIGHT)], check=True, timeout=30)
                    width, height, rgba = canon.decode(output)
                    if (width, height) != (WIDTH, HEIGHT):
                        sys.exit(f"wrong image size {width}x{height}")
                    area = sum(rgba[3::4]) / 255
                    if abs(area - AREA) > 1:
                        label = f"{name}/{transform or 'none'}/{'filtered' if filtered else 'plain'}"
                        failures.append(f"{label}: area {area:.2f}, expected {AREA:g}")
                    checked += 1
    if failures:
        sys.exit(f"{len(failures)} failed cases:\n" + "\n".join(failures))
    print(f"checked {checked} viewport-unit areas")


if __name__ == "__main__":
    main()
