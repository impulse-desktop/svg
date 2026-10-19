"""clip-path-element: the clipPath element of CSS Masking. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(*rects):
    return "".join(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>' for x, y, w, h in rects)


expect("clip-path-element", [
    ("union", (1, 1, 12, 12), lime((2, 2, 4, 10), (8, 2, 4, 4))),
    ("object-bounding-box", (17, 1, 14, 14), lime((18, 2, 6, 6))),
    ("clip-on-clippath", (33, 1, 14, 14), lime((34, 2, 4, 6))),
    ("clip-on-child", (49, 1, 14, 14), lime((50, 2, 4, 4), (50, 8, 12, 4))),
    ("clip-rule-child", (1, 17, 14, 14), '<path fill="lime" fill-rule="evenodd" d="M 2 18 H 14 V 30 H 2 Z M 5 21 H 11 V 27 H 5 Z"/>'),
    ("clip-rule-referencing", (17, 17, 14, 14), lime((18, 18, 12, 12))),
    ("use", (33, 17, 14, 14), lime((34, 18, 6, 6))),
    ("missing", (49, 17, 14, 14), lime((50, 18, 6, 6), (58, 18, 4, 6))),
    ("nested", (1, 33, 22, 14), lime((2, 34, 6, 6))),
    ("use-indirect", (25, 33, 16, 8), lime((26, 34, 6, 6), (34, 34, 6, 6))),
    ("inherit", (41, 33, 14, 14), '<path fill="lime" fill-rule="evenodd" d="M42 34h12v12h-12z M45 37h6v6h-6z"/>'),
    ("display", (55, 33, 8, 8), lime((56, 34, 4, 6))),
    ("use-text", (55, 41, 9, 7), ""),
    ("ancestor-clip", (25, 41, 14, 6), lime((26, 42, 12, 4))),
])
