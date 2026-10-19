"""vector-effect-transforms: vector-effect's constrained transformations, of
SVG 2. The unit is written by hand; this writes its expected drawing: each
rect as the lime its black outline, placed by the spec's formulas,
encloses."""
from unitgen import outlined

outlined("vector-effect-transforms", [
    ("non-scaling-size", (0, 6, 15, 12), [0]), ("non-rotation", (33, 18, 18, 11), [1]), ("both", (64, 29, 13, 9), [2]),
    ("fixed-position", (1, 39, 21, 16), [3]), ("fixed-non-scaling-size", (22, 39, 15, 11), [4]),
    ("fixed-non-rotation", (45, 39, 17, 11), [5]), ("fixed-both", (65, 39, 12, 8), [6]),
    ("no-area", (0, 55, 12, 8), []),
])
