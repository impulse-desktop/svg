"""marker-vertices: markers on the vertices of SVG 2's equivalent paths.
The unit is written by hand; this writes its expected drawing: a lime
square on each vertex."""
from unitgen import expect


def at(points, size=4):
    return "".join(f'<rect x="{x - size / 2:g}" y="{y - size / 2:g}" width="{size}" height="{size}" fill="lime"/>' for x, y in points)


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("marker-vertices", [
    ("subpaths", (1, 3, 34, 14), at([(4, 6), (14, 6), (14, 14), (22, 6), (32, 6), (32, 14)])),
    ("zero-length", (40, 4, 8, 8), at([(44, 8)], 6)),
    ("rect", (48, 0, 16, 16), at([(52, 4)], 6) + at([(60, 4), (60, 12), (52, 12)])),
    ("circle", (3, 21, 18, 18), at([(18, 30), (12, 36), (6, 30), (12, 24)])),
    ("line", (21, 27, 16, 6), at([(24, 30), (34, 30)])),
    ("polygon", (37, 21, 16, 16), at([(40, 24), (50, 24), (45, 34)])),
    ("not-presentation", (53, 24, 10, 12), lime(54, 25, 8, 10)),
    ("not-marker", (1, 45, 12, 12), lime(2, 46, 10, 10)),
    ("zero-size", (13, 45, 12, 12), lime(14, 46, 10, 10)),
    ("inherit", (28, 48, 6, 6), at([(31, 51)])),
    ("display-none", (40, 48, 6, 6), at([(43, 51)])),
    ("not-rendered", (51, 45, 12, 12), lime(52, 46, 10, 10)),
    ("ellipse", (1, 61, 22, 14), at([(20, 68), (12, 72), (4, 68), (12, 64)])),
])
