"""symbol-geometry: symbol and use viewports of SVG 2. The unit is written
by hand; this writes its expected drawing."""
from unitgen import outlined

outlined("symbol-geometry", [
    ("position", (1, 1, 12, 8), [0]), ("use-size", (17, 1, 14, 6), [1]), ("auto-size", (33, 1, 30, 18), [2]), ("ref", (5, 21, 26, 14), [3, 4]),
    ("viewbox-origin", (33, 21, 10, 10), [5]), ("ref-viewbox", (49, 21, 10, 10), [6]), ("overflow", (1, 43, 8, 8), [7]), ("display-none-symbol", (13, 43, 8, 8), [8]),
    ("transform", (1, 55, 8, 8), [9]), ("ref-keywords", (13, 53, 34, 10), [10, 11, 12]), ("ref-slice", (49, 51, 10, 9), [13]),
    ("filter-box", (1, 65, 10, 10), [14]), ("filter-box-ref", (13, 65, 10, 10), [15]),
])
