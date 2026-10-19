"""bbox-geometry: the object bounding box, of SVG 2. The unit is written by
hand; this writes its expected drawing: each box worked out by hand, its
objectBoundingBox effects in user space."""
from unitgen import expect, pair

NAME = "bbox-geometry"


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


# The curve's box runs from 5, its top, to 20: the stop at half its height
# falls at 12.5.
CURVE = ('<linearGradient id="half-user" gradientUnits="userSpaceOnUse" x2="0" y1="5" y2="20"><stop offset=".5" stop-color="navy"/><stop offset=".5" stop-color="gold"/></linearGradient>'
         '<path d="M 2 20 C 2 0 22 0 22 20 Z" fill="url(#half-user)"/><rect x="24" y="5" width="4" height="7.5" fill="navy"/><rect x="24" y="12.5" width="4" height="7.5" fill="gold"/>')
# The group's box runs from 2 to 38 across, the round dots' points taken,
# and 26 to 40 down, the line's: its left half is 2 to 20.
ZERO = ('<clipPath id="left-user"><rect x="2" y="26" width="18" height="14"/></clipPath><g clip-path="url(#left-user)" fill="lime">'
        '<rect x="10" y="26" width="10" height="10"/><path d="M 2 40 H 30" stroke="lime"/>'
        '<path d="M 2 30 h 0 M 38 30 L 38 30" stroke="lime" stroke-linecap="round" stroke-width="2"/></g>')

expect(NAME, [
    ("curve-box", (1, 1, 28, 20), CURVE),
    # The group's box runs from 34 to 64 across, the stroked line's own
    # line taken, not its stroke: its left half is 34 to 49.
    ("group-children", (33, 1, 30, 20), lime(34, 2, 8, 8) + lime(46, 2, 3, 5)),
    ("zero-size-child", (1, 25, 38, 17), ZERO),
    ("fractions", (1, 43, 22, 10), pair(NAME, 2, 44, 48)),
    ("zero-height-gradient", (1, 55, 30, 6), lime(2, 56, 28, 4)),
    ("zero-height-filter", (33, 55, 30, 6), lime(34, 56, 28, 4)),
    ("zero-height-clip", (33, 63, 30, 6), lime(34, 64, 28, 4)),
])
