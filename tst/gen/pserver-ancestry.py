"""pserver-ancestry: paint servers' display and inheritance, of SVG 2. The
unit is written by hand; this writes its expected drawing: lime squares."""
from unitgen import expect


def lime(x):
    return f'<rect x="{x}" y="2" width="10" height="10" fill="lime"/>'


expect("pserver-ancestry", [
    ("display-none", (1, 1, 12, 12), lime(2)), ("own-ancestors", (13, 1, 12, 12), lime(14)), ("not-rendered", (25, 1, 12, 12), lime(26)),
    ("pattern-display-none", (37, 1, 12, 12), lime(38)), ("pattern-content", (49, 1, 12, 12), lime(50)),
])
