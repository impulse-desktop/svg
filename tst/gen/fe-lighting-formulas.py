"""fe-lighting-formulas: the lighting primitives of CSS Filter Effects,
against references computed from their formulas. The unit is written by
hand; this writes its expected drawing."""
import os
import re

from unitgen import ROOT, expect

U = "fe-lighting-formulas"
text = open(os.path.join(ROOT, "tst", "unit", U + ".svg")).read()
NAMES = ["diffuse-distant", "diffuse-point", "diffuse-spot", "specular-distant", "specular-point", "spot-focus", "spot-cone", "spot-edges"]
cells = []
for k, name in enumerate(NAMES):
    x = 2 + 12 * k
    image = re.search(r'<g transform="translate\(%d 2\)">.*?(<image y="8"[^>]*/>)</g>' % x, text).group(1)
    cells.append((name, (x - 1, 1, 12, 18), f'<g transform="translate({x} 2)">{image.replace(chr(32) + "y=" + chr(34) + "8" + chr(34), chr(32) + "y=" + chr(34) + "0" + chr(34))}{image}</g>'))
expect(U, cells)
