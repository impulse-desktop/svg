"""stroke-bounding-box: the stroke bounding box of CSS Masking. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


def at(column, row):
    return (24 * column, 24 * row, 24, 24)


expect("stroke-bounding-box", [
    ("miter", at(0, 0), lime(2, 4, 20, 16)),
    ("round", at(1, 0), lime(29, 7, 14, 10)),
    ("square-cap", (48, 0, 48, 24), lime(52.5858, 6.5858, 14.8284, 10.8284) + lime(76.5858, 6.5858, 14.8284, 10.8284)),
    ("miter-limit", at(0, 1), lime(4.8, 30.8, 14.4, 10.4)),
    ("bevel", at(1, 1), lime(29, 31, 14, 10)),
    ("miter-clip", at(2, 1), lime(50, 28, 20, 16)),
    ("no-stroke", (72, 24, 24, 24), lime(78, 32, 12, 8)),
    ("zero-width", at(0, 2), lime(6, 56, 12, 8)),
    ("limit-below-one", at(1, 2), lime(29, 55, 14, 10)),
    ("rect-cap", at(2, 2), lime(54, 56, 12, 8)),
    ("group", at(3, 2), lime(74.3431, 50, 16.6569, 20)),
    ("half-border", at(0, 3), lime(4, 78, 16, 12)),
    ("mask-origin", at(1, 3), lime(26, 76, 10, 8)),
    ("transform-box", at(2, 3), '<path d="M52 82 55 78 58 82" fill="none" stroke="lime"/>'),
    ("shapes-cap", at(3, 3), lime(73, 79, 10, 10) + lime(86, 78, 8, 12)),
])
