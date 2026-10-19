"""viewport-nested: nested svg viewports of SVG 2. The unit is written by
hand; this writes its expected drawing."""
from unitgen import outlined

outlined("viewport-nested", [
    ("clip", (1, 1, 14, 14), [0]), ("overflow-visible", (17, 1, 14, 14), [1]), ("percent", (33, 1, 30, 16), [2, 3]), ("negative-viewbox", (1, 19, 14, 14), [4]),
    ("zero-viewbox", (17, 19, 14, 14), []), ("transform", (33, 19, 14, 14), [5]), ("symbol-viewport", (1, 39, 62, 24), [6, 7]),
])
