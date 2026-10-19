"""use-geometry: the use element's position, size and references, of SVG 2.
The unit is written by hand; this writes its expected drawing."""
from unitgen import outlined

outlined("use-geometry", [
    ("translate-after", (1, 1, 12, 12), [0]), ("clip-translated", (15, 1, 10, 10), [1]), ("size-svg", (25, 1, 10, 10), [2]), ("negative-size", (37, 1, 10, 10), [3]),
    ("zero-size", (49, 1, 12, 12), []), ("size-other", (1, 15, 12, 12), [4]), ("percent", (13, 15, 10, 10), [5]), ("foreign", (25, 15, 8, 10), []),
    ("ancestor", (33, 15, 6, 6), [6]), ("circular", (39, 15, 10, 10), []), ("indirect", (49, 15, 6, 6), [7]),
])
