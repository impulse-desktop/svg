"""paint-current-color: currentColor wherever a colour is taken, of SVG 2.
The unit is written by hand; this writes its expected drawing: lime
squares."""
from unitgen import expect


def lime(x):
    return f'<rect x="{x}" y="2" width="10" height="10" fill="lime"/>'


expect("paint-current-color", [
    ("fill", (1, 1, 12, 12), lime(2)), ("stroke", (13, 1, 12, 12), lime(14)), ("stop-color", (25, 1, 12, 12), lime(26)),
    ("flood-color", (37, 1, 12, 12), lime(38)), ("lighting-color", (49, 1, 12, 12), lime(50)),
])
