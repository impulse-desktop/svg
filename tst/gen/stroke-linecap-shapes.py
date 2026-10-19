"""stroke-linecap-shapes: line caps and zero-length subpaths, of SVG 2. The
unit is written by hand; this writes its expected drawing: each stroke as
the shape the spec gives it, its black edges as the unit draws them."""
import math
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "stroke-linecap-shapes.svg")).read()
EDGES = re.findall(r'<g fill="none" stroke="black" stroke-width="\.3">.*?</g>', text, re.S)


def capped(x0, y0, x1, y1, w):
    """A segment's stroke with square caps, as a polygon."""
    length = math.hypot(x1 - x0, y1 - y0)
    dx, dy = (x1 - x0) / length * w / 2, (y1 - y0) / length * w / 2
    points = [(x0 - dx - dy, y0 - dy + dx), (x1 + dx - dy, y1 + dy + dx), (x1 + dx + dy, y1 + dy - dx), (x0 - dx + dy, y0 - dy - dx)]

    return "M" + " ".join("%.4f %.4f" % p for p in points) + "Z"


def diamond(x, y, half):
    r = half * math.sqrt(2)

    return f"M{x} {y - r:.4f}L{x + r:.4f} {y}L{x} {y + r:.4f}L{x - r:.4f} {y}Z"


def one(*parts):
    """Subpaths of one stroke, filled as one shape."""
    return '<path fill="lime" d="' + " ".join(parts) + '"/>'


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


ROUND = '<path fill="lime" d="M 28 5 H 36 A 3 3 0 0 1 36 11 H 28 A 3 3 0 0 1 28 5 Z"/>'
expect("stroke-linecap-shapes", [
    ("caps", (2, 2, 62, 12), lime(6, 5, 8, 6) + ROUND + lime(47, 5, 14, 6)),
    ("zero-butt", (1, 17, 14, 14), lime(2, 18, 12, 12)),
    ("zero-round", (17, 19, 10, 10), '<circle cx="22" cy="24" r="3" fill="lime"/><circle cx="50" cy="24" r="3" fill="lime"/>'),
    ("zero-square", (31, 19, 10, 10), lime(33, 21, 6, 6)),
    ("moveto-alone", (1, 33, 14, 18), lime(2, 34, 12, 16)),
    ("zero-direction", (14, 30, 49, 20), one(capped(18, 34, 24, 40, 6), diamond(30, 42.5, 3)) + lime(45, 39, 6, 6) + one(capped(56, 30, 56, 32, 4), "M56 40h4v4h-4Z")),
    ("zero-next", (1, 52, 20, 12), one(diamond(6, 60, 2), capped(13, 55, 18, 60, 4))),
    ("zero-after-closed", (22, 52, 22, 12), '<path d="M 26 54 h 4 v 4 Z" fill="none" stroke="lime" stroke-width="4"/>' + one(diamond(38, 60, 2))),
    ("zero-no-direction", (44, 56, 16, 6), lime(46, 58, 4, 4) + lime(54, 58, 4, 4)),
    ("zero-after-close", (1, 63, 20, 12), '<path d="M 4 66 h 4 v 4 L 4 66 Z" fill="none" stroke="lime" stroke-width="4"/>' + one(diamond(16, 70, 2))),
    ("edges", (0, 0, 0, 0), "".join(EDGES)),
])
