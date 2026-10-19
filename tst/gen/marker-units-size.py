"""marker-units-size: markerUnits and a marker's size, of SVG 2. The unit is
written by hand; this writes its expected drawing: each marker as the lime
its black outline encloses, over the silver stroke where it shows."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("marker-units-size", [
    ("stroke-width", (2, 4, 8, 8), lime(3, 5, 6, 6)), ("stroke-none", (12, 4, 8, 8), lime(13, 5, 6, 6)),
    ("user-space", (24, 5, 4, 6), '<path d="M 26 8 h 1" stroke="silver" stroke-width="3"/>' + lime(25, 7, 2, 2)),
    # The marker's viewport clip and its content share edges at half a
    # pixel, whose antialiasing the two take both.
    ("default-size", (33, 5, 6, 6), lime(34.5, 6.5, 3, 3), 64), ("non-scaling", (45, 5, 6, 6), lime(46, 6, 4, 4)),
    ("scaled", (3, 13, 10, 10), lime(4, 14, 8, 8)),
])
