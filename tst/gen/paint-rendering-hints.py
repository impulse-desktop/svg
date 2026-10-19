"""paint-rendering-hints: shape-rendering and image-rendering of SVG 2. The
unit is written by hand; this writes its expected drawing: smooth images
as the renderer's own smooth resampling, which the property leaves to it,
nearest ones and unsmoothed circles as images of the pixels they cover,
by their centres."""
import os
import re

from unitgen import ROOT, expect, png

U = "paint-rendering-hints"
text = open(os.path.join(ROOT, "tst", "unit", U + ".svg")).read()
CHECKER = re.search(r'href="(data:[^"]*)"', text).group(1)


def smooth(x):
    return f'<image x="{x}" y="2" width="12" height="12" image-rendering="smooth" href="{CHECKER}"/>'


def nearest(x):
    # The checker is black at its top left and bottom right.
    rows = [[(1, 1, 1, 1) if (i < 6) != (j < 6) else (0, 0, 0, 1) for i in range(12)] for j in range(12)]
    return f'<image x="{x}" y="2" width="12" height="12" image-rendering="pixelated" href="{png(rows)}"/>'


def aliased(cx, cy=23):
    rows = [[(0, 0, 0, 1) if (i + .5 - 6) ** 2 + (j + .5 - 6) ** 2 <= 5.3 ** 2 else (0, 0, 0, 0) for i in range(12)] for j in range(12)]
    return f'<image x="{cx - 6}" y="{cy - 6}" width="12" height="12" image-rendering="pixelated" href="{png(rows)}"/>'


expect(U, [
    ("image-smooth", (1, 1, 30, 14), smooth(2) + smooth(18)),
    ("image-speed", (33, 1, 30, 14), nearest(34) + nearest(50)),
    ("shape-smooth", (1, 16, 30, 15), '<circle cx="8" cy="23" r="5.3"/><circle cx="24" cy="23" r="5.3"/>'),
    ("shape-crisp", (33, 16, 30, 15), aliased(40) + aliased(56)),
])
