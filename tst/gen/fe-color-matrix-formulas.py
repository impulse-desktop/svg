"""fe-color-matrix-formulas: feColorMatrix of CSS Filter Effects. The unit
is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "fe-color-matrix-formulas"
expect(U, [
    ("matrix", (1, 1, 10, 14), pair(U, 2, 2, 8)),
    ("saturate", (11, 1, 10, 14), pair(U, 12, 2, 8)),
    ("hue-rotate", (21, 1, 10, 14), pair(U, 22, 2, 8)),
    ("luminance-to-alpha", (31, 1, 10, 14), pair(U, 32, 2, 8)),
    ("value-counts", (41, 1, 10, 14), pair(U, 42, 2, 8)),
    ("defaults", (1, 15, 30, 14), pair(U, 2, 16, 22) + pair(U, 12, 16, 22) + pair(U, 22, 16, 22)),
    ("value-counts-typed", (31, 15, 20, 14), pair(U, 32, 16, 22) + pair(U, 42, 16, 22)),
])
