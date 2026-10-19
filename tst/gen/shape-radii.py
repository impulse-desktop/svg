"""shape-radii: rect, circle and ellipse radii of SVG 2. The unit is written
by hand; this writes its expected drawing."""
from unitgen import outlined

outlined("shape-radii", [
    ("rx-alone", (1, 1, 12, 26), [0]), ("ry-alone", (15, 1, 18, 12), [1]), ("negative-rx", (35, 1, 14, 12), [2]), ("percent-radii", (15, 15, 22, 18), [3]),
    ("percent-r", (39, 15, 14, 15), [4]), ("ellipse-one-radius", (3, 31, 36, 16), [5, 6]), ("zero-negative", (43, 33, 20, 14), []),
])
