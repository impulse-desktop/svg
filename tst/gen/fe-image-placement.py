"""fe-image-placement: feImage of CSS Filter Effects. The unit is written by
hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("fe-image-placement", [("image-meet", (1, 1, 10, 14), lime(2, 6, 8, 4)), ("image-stretch", (13, 1, 10, 14), lime(14, 2, 8, 12)), ("element", (25, 1, 14, 14), lime(28, 3, 4, 8)),
                              ("missing", (41, 1, 10, 14), ""), ("href-wins", (53, 1, 10, 14), lime(54, 2, 8, 12)),
                              ("element-box", (1, 17, 14, 14), lime(2, 18, 6, 12)), ("symbol", (17, 17, 14, 14), lime(18, 18, 6, 12)), ("svg", (33, 17, 14, 14), lime(34, 18, 6, 6)),
                              ("inherits", (49, 17, 14, 14), lime(50, 18, 6, 6))])
