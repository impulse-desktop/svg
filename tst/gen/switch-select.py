"""switch-select: the switch element and conditional processing, of SVG 2.
The unit is written by hand; this writes its expected drawing: lime where
the chosen children draw."""
from unitgen import expect


def lime(x):
    return f'<rect x="{x}" y="2" width="10" height="10" fill="lime"/>'


expect("switch-select", [
    ("first-true", (1, 1, 12, 12), lime(2)), ("language-prefix", (13, 1, 12, 12), lime(14)), ("language-case", (25, 1, 12, 12), lime(26)),
    ("language-list", (37, 1, 12, 12), lime(38)), ("display-none-chosen", (49, 1, 12, 12), lime(50)),
    ("excluded", (1, 13, 62, 10), '<rect x="2" y="14" width="60" height="8" fill="lime"/>'),
])
