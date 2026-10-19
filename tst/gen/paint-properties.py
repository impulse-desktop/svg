"""paint-properties: the painting properties' initial values and their
inheritance, of SVG 2, with a join of collinear segments and a marker of
negative size. Each test leaves a property to its initial value or to its
parent; its plain twin in the expected drawing sets that value itself."""
from unitgen import Unit, png

unit = Unit("paint-properties", 64, 50, """Each drawing matches its plain twin
in the expected drawing, which gives the value itself: fill is black unless
set and inherited; stroke, fill-opacity, stroke-opacity, stroke-width,
stroke-linecap, stroke-linejoin, stroke-miterlimit, stroke-dasharray,
stroke-dashoffset, the marker properties, paint-order, shape-rendering and
image-rendering are inherited; stroke-width is 1 and stroke-linecap butt
unless set, stroke-dasharray none. Two segments going on the same way meet
with no join; a marker of negative width is an error, drawn nowhere.""")
unit.common('<defs><marker id="m" markerWidth="4" markerHeight="4" refX="2" refY="2" markerUnits="userSpaceOnUse"><rect width="4" height="4" fill="navy"/></marker>'
            '<marker id="negative" markerWidth="-4" markerHeight="4" markerUnits="userSpaceOnUse"><rect width="4" height="4" fill="red"/></marker></defs>')

# A checker of four pixels, which optimizeSpeed may draw as blocks.
IMAGE = png([[(0, 1, 0, 1), (0, 0, .5, 1)], [(0, 0, .5, 1), (0, 1, 0, 1)]])


def cell(name, k, inherited, own, shape):
    """A cell at place k: shape under a group holding inherited, against
    shape holding own; shape takes x, y and the attributes given it."""
    x, y = 2 + k % 6 * 10, 2 + k // 6 * 12
    test = f"<g{inherited}>{shape.format(x=x, y=y, a='')}</g>" if inherited else shape.format(x=x, y=y, a="")
    unit.cell(name, (x - 1, y - 1, 10, 12), test, shape.format(x=x, y=y, a=own), "")


RECT = '<rect x="{x}" y="{y}" width="8" height="8"{a}/>'
OUTLINED = '<rect x="{x}" y="{y}" width="6" height="6" stroke="lime" stroke-width="2"{a} transform="translate(1 1)"/>'
LINE = '<path d="M {x} {y} m 1 2 v 6 h 6" fill="none" stroke="lime" stroke-width="2"{a}/>'

cells = [
    ("fill-initial", "", ' fill="black"', RECT),
    ("fill-inherited", ' fill="lime"', ' fill="lime"', RECT),
    ("stroke-inherited", ' stroke="navy"', ' stroke="navy"', '<rect x="{x}" y="{y}" width="6" height="6" fill="lime" transform="translate(1 1)"{a}/>'),
    ("fill-opacity-inherited", ' fill-opacity=".5"', ' fill-opacity=".5"', '<rect x="{x}" y="{y}" width="8" height="8" fill="navy"{a}/>'),
    ("stroke-opacity-inherited", ' stroke-opacity=".5"', ' stroke-opacity=".5"', OUTLINED),
    ("stroke-width-initial", "", ' stroke-width="1"', '<rect x="{x}" y="{y}" width="6" height="6" fill="none" stroke="navy" transform="translate(1 1)"{a}/>'),
    ("stroke-width-inherited", ' stroke-width="3"', ' stroke-width="3"', '<rect x="{x}" y="{y}" width="5" height="5" fill="none" stroke="navy" transform="translate(1.5 1.5)"{a}/>'),
    ("linecap-initial", "", ' stroke-linecap="butt"', LINE),
    ("linecap-inherited", ' stroke-linecap="square"', ' stroke-linecap="square"', LINE),
    ("linejoin-inherited", ' stroke-linejoin="round"', ' stroke-linejoin="round"', LINE),
    ("miterlimit-inherited", ' stroke-miterlimit="1"', ' stroke-miterlimit="1"', LINE),
    ("dasharray-initial", "", ' stroke-dasharray="none"', LINE),
    ("dasharray-inherited", ' stroke-dasharray="2 1"', ' stroke-dasharray="2 1"', LINE),
    ("dashoffset-inherited", ' stroke-dasharray="2 1" stroke-dashoffset="1"', ' stroke-dasharray="2 1" stroke-dashoffset="1"', LINE),
    ("marker-inherited", ' marker-start="url(#m)"', ' marker-start="url(#m)"', LINE),
    ("paint-order-inherited", ' paint-order="stroke"', ' paint-order="stroke"', '<rect x="{x}" y="{y}" width="6" height="6" fill="lime" stroke="navy" stroke-width="2" transform="translate(1 1)"{a}/>'),
    ("shape-rendering-inherited", ' shape-rendering="crispEdges"', ' shape-rendering="crispEdges"', '<circle cx="{x}" cy="{y}" r="3.7" fill="navy" transform="translate(4 4)"{a}/>'),
    ("image-rendering-inherited", ' image-rendering="optimizeSpeed"', ' image-rendering="optimizeSpeed"', '<image x="{x}" y="{y}" width="8" height="8" href="' + IMAGE + '"{a}/>'),
]

for k, (name, inherited, own, shape) in enumerate(cells):
    cell(name, k, inherited, own, shape)

# Collinear segments going on the same way: no join, so a dash ending
# where they meet ends there butt, with no round join past it; a marker of
# negative width draws nothing.
x, y = 2, 2 + 3 * 12
unit.cell("collinear", (x - 1, y - 1, 30, 6), f'<path d="M {x} {y + 2} H {x + 12} H {x + 28}" stroke="lime" stroke-width="4" stroke-linejoin="round" stroke-dasharray="12 100"/>',
          f'<rect x="{x}" y="{y}" width="12" height="4" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="4"/>')
unit.cell("marker-negative-size", (x + 31, y - 1, 10, 6), f'<path d="M {x + 34} {y + 2} h 4" stroke="lime" stroke-width="4" marker-start="url(#negative)"/>',
          f'<rect x="{x + 34}" y="{y}" width="4" height="4" fill="lime"/>', "")

unit.write()
