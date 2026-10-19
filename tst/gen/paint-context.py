"""paint-context: context-fill and context-stroke, of SVG 2. The unit is
written by hand; this writes its expected drawing: lime where the context
paints, in the context element's bounding box."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("paint-context", [
    ("marker-stroke", (1, 1, 10, 10), lime(2, 2, 8, 8)), ("marker-fill", (13, 1, 10, 10), lime(14, 2, 8, 8)),
    ("use-fill", (25, 1, 10, 10), lime(26, 2, 8, 8)), ("use-recursive", (37, 1, 10, 10), lime(38, 2, 8, 8)),
    ("no-context", (49, 1, 10, 10), lime(50, 2, 8, 8)),
    # The path's box runs from 4 to 57: its gradient turns lime past 46.4.
    # The marker's viewport clip and its content share an edge at half a
    # pixel, whose antialiasing the two take both.
    ("marker-server-box", (53, 19, 8, 9), lime(54, 20.5, 6, 6), 64),
    # The use's box runs from 2 to 26: lime to 6.8 and from 21.2.
    ("use-server-box", (1, 29, 26, 10), lime(2, 30, 4, 8) + lime(22, 30, 4, 8)),
])
