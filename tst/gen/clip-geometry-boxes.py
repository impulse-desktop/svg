"""clip-geometry-boxes: the reference boxes of clip-path. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("clip-geometry-boxes", [
    ("border-box-default", (1, 1, 14, 14), lime(2, 2, 12, 12)),
    ("fill-box", (17, 1, 14, 14), lime(20, 4, 8, 8)),
    ("box-alone", (33, 1, 30, 14), lime(36, 4, 8, 8) + lime(50, 2, 12, 12)),
    ("padding-box", (1, 17, 14, 14), lime(6, 22, 4, 4)),
    ("stroke-box", (17, 17, 14, 14), lime(21, 21, 6, 6)),
    ("circle-fill-box", (33, 17, 14, 14), '<circle cx="40" cy="24" r="4" fill="lime"/>'),
    ("view-box", (1, 33, 28, 14), lime(14, 34, 12, 12)),
    ("view-box-root", (33, 33, 22, 12), lime(34, 34, 20, 10)),
])
