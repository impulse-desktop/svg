"""filter-function-equivalents: the filter functions of CSS Filter Effects
against their equivalent primitives. The unit is written by hand; this
writes its expected drawing."""
from unitgen import expect, pair

U = "filter-function-equivalents"
NAMES = ["grayscale", "sepia", "saturate", "hue-rotate", "invert", "opacity", "brightness", "contrast", "blur", "drop-shadow"]
cells = []
for k, name in enumerate(NAMES):
    x, y = 4 + 12 * (k % 5), 4 + 32 * (k // 5)
    cells.append((name, (x - 4, y - 4, 12, 30), pair(U, x, y, y + 14)))
expect(U, cells)
