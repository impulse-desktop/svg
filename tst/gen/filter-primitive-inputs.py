"""filter-primitive-inputs: filter primitive inputs and subregions. The
unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("filter-primitive-inputs", [
    ("zero-subregion", (1, 1, 10, 14), ""),
    ("duplicate-result", (11, 1, 10, 14), lime(12, 2, 8, 12)),
    ("previous", (21, 1, 10, 14), lime(22, 2, 8, 12)),
    ("missing-result", (31, 1, 10, 14), lime(32, 2, 8, 12)),
    ("forward-result", (41, 1, 10, 14), lime(42, 2, 8, 12)),
    ("source-alpha", (51, 1, 10, 14), pair("filter-primitive-inputs", 52, 2, 8)),
    ("last-primitive", (13, 17, 10, 12), lime(14, 18, 8, 10)),
    ("empty-filter", (25, 17, 10, 12), ""),
    ("subregion", (1, 17, 10, 12), lime(4, 18, 6, 10)),
    ("subregion-sample", (40, 16, 10, 14), ""),
])
