"""gradient-vector-interpolation: a gradient as the shape's geometry and its
interpolation, of SVG 2. The unit is written by hand; this writes its
expected drawing: each test as its reference, moved where the test is."""
from unitgen import expect, pair

NAME = "gradient-vector-interpolation"
V = '<rect x="{}" y="14" width="12" height="12" fill="url(#v)"/>'
expect(NAME, [
    ("unpremultiplied", (1, 1, 62, 10), pair(NAME, 2, 2, 6)),
    ("shape-transform", (1, 13, 26, 14), V.format(2) + V.format(14)), ("gradient-transform", (29, 13, 26, 14), V.format(30) + V.format(42)),
    ("user-space-transform", (1, 29, 22, 10), '<rect x="2" y="30" width="20" height="4" fill="url(#w)"/><rect x="2" y="34" width="20" height="4" fill="url(#w)"/>'),
])
