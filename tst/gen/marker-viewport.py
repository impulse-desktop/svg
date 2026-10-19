"""marker-viewport: a marker's viewport, refX and refY and overflow, of SVG
2. The unit is written by hand; this writes its expected drawing. Where a
marker's content ends on its viewport's edge at half a pixel, the clip's
and the content's antialiasing meet, and the cell allows it."""
from unitgen import outlined

outlined("marker-viewport", [
    ("viewbox-fit", (2, 3, 8, 8), [0]), ("aspect-none", (12, 3, 8, 8), [1], "", 64), ("ref-percent", (23, 3, 8, 8), [2], "", 64), ("ref-keywords", (33, 3, 8, 8), [3]), ("ref-viewbox-units", (45, 3, 8, 8), [4], "", 64),
    ("overflow-hidden", (2, 22, 8, 8), [5]), ("overflow-visible", (14, 22, 8, 8), [6]), ("overflow-auto", (26, 22, 8, 8), [7]), ("overflow-scroll", (38, 22, 8, 8), [8]),
])
