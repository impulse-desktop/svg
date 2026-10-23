"""fe-color-matrix-formulas: feColorMatrix of CSS Filter Effects. The unit
is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "fe-color-matrix-formulas"
cells = [
    ("matrix", (1, 1, 10, 14), pair(U, 2, 2, 8)),
    ("saturate", (11, 1, 10, 14), pair(U, 12, 2, 8)),
    ("hue-rotate", (21, 1, 10, 14), pair(U, 22, 2, 8)),
    ("luminance-to-alpha", (31, 1, 10, 14), pair(U, 32, 2, 8)),
    ("value-counts", (41, 1, 10, 14), pair(U, 42, 2, 8)),
    ("defaults", (1, 15, 30, 14), pair(U, 2, 16, 22) + pair(U, 12, 16, 22) + pair(U, 22, 16, 22)),
    ("value-counts-typed", (31, 15, 20, 14), pair(U, 32, 16, 22) + pair(U, 42, 16, 22)),
]

# SourceAlpha's RGB is zero; only the alpha row contributes. These exact
# opacities exercise gain below/at/above saturation and per-primitive clamp.
for i, (name, alpha) in enumerate([
    ("zero", 0), ("quarter", .25), ("half", .5), ("one", 1),
    ("over", 1), ("before-next", .25), ("negative", 0),
    ("source-rgb", .5),
]):
    x, y = 2 + 13 * (i % 4), 32 + 14 * (i // 4)
    cells.append(("hard-alpha-" + name, (x - 1, y - 1, 12, 12),
                  f'<rect x="{x}" y="{y}" width="8" height="10" '
                  f'fill="black" fill-opacity="{alpha}"/>'))
expect(U, cells)
