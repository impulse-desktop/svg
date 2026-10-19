"""bbox-inputs: what an object bounding box takes in, of SVG 2: properties
that leave it alone, references that resolve to nothing, a child's clip, an
image's positioning rectangle, and percentages as fractions of it. Each
group's filter floods its box lime, worked out by hand in the expected
drawing."""
from unitgen import Unit, png

unit = Unit("bbox-inputs", 64, 40, """Each lime square matches its plain twin
in the expected drawing; no red. A group's filter floods its object
bounding box: a child hidden by visibility, of opacity 0, of fill none or
dashed still adds its box; a use whose reference resolves to nothing adds
none (D37), a shape whose paint does still adds its own; a child's clip does
not cut the box; an image's box is its positioning rectangle, whatever part
of it the picture fills. objectBoundingBox percentages are the fractions
they write.""")
TALL = png([[(0, 0, .5, 1)]] * 2)
unit.common('<defs><filter id="box" x="0" y="0" width="1" height="1"><feFlood flood-color="lime"/></filter>'
            '<clipPath id="corner" clipPathUnits="objectBoundingBox"><rect width=".3" height=".3"/></clipPath>'
            '<linearGradient id="percent" x1="25%" x2="75%"><stop offset="0" stop-color="navy"/><stop offset="1" stop-color="gold"/></linearGradient>'
            '<pattern id="tile" width="50%" height="50%"><rect width="3" height="3" fill="lime"/></pattern></defs>',
            '<defs><linearGradient id="fraction" x1=".25" x2=".75"><stop offset="0" stop-color="navy"/><stop offset="1" stop-color="gold"/></linearGradient>'
            '<pattern id="tile-fraction" width=".5" height=".5"><rect width="3" height="3" fill="lime"/></pattern></defs>')


def place(k):
    return 2 + k % 4 * 16, 2 + k // 4 * 14


def boxed(name, k, child, box=(0, 0, 12, 10)):
    """A group of a red square at the corner and child, given its place."""
    x, y = place(k)
    dx, dy, w, h = box
    unit.cell(name, (x - 1, y - 1, 14, 12), f'<g filter="url(#box)"><rect x="{x}" y="{y}" width="4" height="4" fill="red"/>{child.format(x=x, y=y, x8=x + 8, y6=y + 6)}</g>',
              f'<rect x="{x + dx}" y="{y + dy}" width="{w}" height="{h}" fill="lime"/>', "")


boxed("visibility-hidden", 0, '<rect x="{x8}" y="{y6}" width="4" height="4" visibility="hidden"/>')
boxed("opacity-zero", 1, '<rect x="{x8}" y="{y6}" width="4" height="4" opacity="0"/>')
boxed("fill-none", 2, '<rect x="{x8}" y="{y6}" width="4" height="4" fill="none"/>')
boxed("dashed", 3, '<path d="M {x} {y} H {x8} V {y6} h 4 v 4" fill="none" stroke="red" stroke-dasharray="1 100" stroke-opacity="0"/>')
boxed("unresolved-use", 4, '<use href="#nothing" x="{x8}" y="{y6}"/>', (0, 0, 4, 4))
boxed("unresolved-paint", 5, '<rect x="{x8}" y="{y6}" width="4" height="4" fill="url(#nothing)"/>')
boxed("child-clip", 6, '<rect x="{x}" y="{y}" width="12" height="10" fill="red" clip-path="url(#corner)"/>')
# A picture twice as tall as wide, met in a box wider than tall, fills a
# strip of its middle; the box is the whole rectangle.
boxed("image-box", 7, f'<image x="{{x}}" y="{{y}}" width="12" height="10" href="{TALL}"/>')

for k, (name, test, plain) in enumerate((("gradient-percent", "percent", "fraction"), ("pattern-percent", "tile", "tile-fraction")), 8):
    x, y = place(k)
    unit.cell(name, (x - 1, y - 1, 14, 12), f'<rect x="{x}" y="{y}" width="12" height="10" fill="url(#{test})"/>',
              f'<rect x="{x}" y="{y}" width="12" height="10" fill="url(#{plain})"/>', f'<rect x="{x}" y="{y}" width="12" height="10"/>')

unit.write()
