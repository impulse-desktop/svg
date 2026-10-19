"""filter-region: the filter element and its region, of CSS Filter Effects.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("filter-region", [
    ("default-region", (1, 1, 14, 14), lime(2, 2, 12, 12)),
    ("user-space", (13, 2, 8, 10), lime(14, 3, 6, 8)),
    ("cut", (21, 1, 14, 14), lime(28, 3, 5, 10)),
    ("primitive-units", (35, 1, 18, 10), lime(41, 3, 10, 6)),
    ("primitive-units-radius", (1, 17, 26, 16), lime(2, 19, 24, 12)),
    ("inherit", (29, 17, 10, 14), pair("filter-region", 30, 18, 24)),
    ("display", (42, 18, 14, 14), lime(43, 19, 12, 12)),
    ("filter-res", (57, 18, 14, 14), lime(58, 19, 12, 12)),
    ("group-region", (62, 2, 9, 14), ""),
])
