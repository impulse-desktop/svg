"""gradient-linear-geometry: a linear gradient's vector and units, of SVG 2.
The unit is written by hand; this writes its expected drawing: each test
strip as the userSpaceOnUse reference under it."""
from unitgen import expect, pair

NAME = "gradient-linear-geometry"
expect(NAME, [
    ("defaults", (1, 1, 62, 10), pair(NAME, 2, 2, 6)), ("viewport-percent", (1, 11, 62, 10), pair(NAME, 2, 12, 16)),
    ("transform-after-box", (1, 21, 62, 10), pair(NAME, 2, 22, 26)),
    ("equal-ends", (1, 31, 62, 7), '<rect x="2" y="32" width="60" height="5" fill="lime"/>'), ("css-transform", (1, 38, 62, 10), pair(NAME, 2, 39, 43)),
])
