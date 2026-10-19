"""style-sheets: style elements, attributes and presentation attributes of
SVG 2. The unit is written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w=10, h=10):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("style-sheets", [
    ("type", (1, 1, 36, 12), lime(2, 2) + lime(14, 2) + lime(26, 2)),
    ("media", (37, 1, 26, 12), lime(38, 2) + lime(50, 2)),
    ("cascade", (1, 13, 24, 12), lime(2, 14) + lime(14, 14)),
    ("important-attribute", (25, 13, 12, 12), lime(26, 14)),
    ("geometry", (37, 13, 26, 24), lime(38, 14) + '<circle cx="56" cy="19" r="5" fill="lime"/>' + lime(50, 26)),
])
