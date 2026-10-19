"""fe-blend-formulas: feBlend of CSS Filter Effects. The unit is written by
hand; this writes its expected drawing."""
from unitgen import expect, pair

MODES = ["normal", "multiply", "screen", "overlay", "color-dodge", "soft-light", "difference", "hue", "color", "luminosity"]
cells = []
for k, mode in enumerate(MODES):
    x, y = 2 + 9 * (k % 7), 2 + 14 * (k // 7)
    cells.append((mode, (x - 1, y - 1, 9, 14), pair("fe-blend-formulas", x, y, y + 6)))
cells.append(("no-composite", (28, 15, 36, 14), "".join(pair("fe-blend-formulas", x, 16, 22) for x in (29, 38, 47, 56))))
expect("fe-blend-formulas", cells)
