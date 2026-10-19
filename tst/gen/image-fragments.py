"""image-fragments: views of SVG images by fragment, of SVG 2's linking.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(*rects):
    return "".join(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>' for x, y, w, h in rects)


expect("image-fragments", [
    ("svg-view", (1, 1, 20, 10), lime((2, 2, 8, 8), (12, 2, 8, 8))), ("media-fragment", (21, 1, 20, 10), lime((22, 2, 8, 8), (32, 2, 8, 8))),
    ("view-element", (41, 1, 10, 10), lime((42, 2, 8, 8))), ("time-segment", (51, 1, 10, 10), lime((52, 2, 8, 8))),
    ("view-transform", (0, 13, 11, 10), lime((5, 14, 4, 8))), ("zero-view", (13, 13, 10, 10), ""), ("invalid-fragment", (25, 13, 30, 10), lime((26, 14, 8, 8), (36, 14, 8, 8), (46, 14, 8, 8))),
    ("unclosed", (1, 25, 8, 8), lime((2, 26, 6, 6))), ("zero-media", (11, 25, 8, 8), ""), ("mask-fragment", (21, 25, 8, 8), lime((22, 26, 6, 6))),
])
