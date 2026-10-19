"""fe-morphology-values: feMorphology of CSS Filter Effects. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w, h, fill="lime"):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}"/>'


expect("fe-morphology-values", [
    ("dilate", (1, 1, 12, 14), lime(2, 2, 10, 12)),
    ("erode", (13, 1, 12, 14), lime(16, 3, 6, 10)),
    ("two-radii", (25, 1, 12, 14), lime(26, 3, 10, 10)),
    ("default-erode", (37, 1, 12, 14), lime(40, 3, 6, 10)),
    ("zero-negative", (49, 1, 22, 14), lime(51, 2, 8, 12) + lime(61, 2, 8, 12)),
    ("channel-max", (3, 18, 14, 14), lime(4, 19, 5, 12, "red") + lime(9, 19, 2, 12, "yellow") + lime(11, 19, 5, 12)),
])
