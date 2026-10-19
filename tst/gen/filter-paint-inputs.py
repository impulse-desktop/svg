"""filter-paint-inputs: FillPaint and StrokePaint of CSS Filter Effects. The
unit is written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("filter-paint-inputs", [("fill-paint", (2, 0, 14, 20), lime(3, 1, 12, 18)), ("stroke-paint", (22, 0, 14, 20), lime(23, 1, 12, 18)), ("gradient-box", (41, 0, 16, 20), lime(42, 1, 8, 18))])
