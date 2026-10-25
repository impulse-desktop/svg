"""Whole-pixel rectangles and their fractional boundaries, from exact areas."""
from fractions import Fraction as F

from unitgen import Unit, png


SIDE = 16
unit = Unit('raster-covered-rectangle', 96, 64,
            'A rectangle covering whole raster pixels needs no edge sampling. '
            'Exact area controls retain fractional edges, clipping, winding, '
            'opacity, repeated uses, holes and horizontal retraces.', ground=False)


def area(box, x, y):
    left, top, right, bottom = map(lambda n: F(str(n)), box)
    return max(0, min(x + 1, right) - max(x, left)) * max(0, min(y + 1, bottom) - max(y, top))


def cell(name, content, boxes, opacity=1, repeats=1):
    index = len(unit.cells)
    x, y = index % 6 * SIDE, index // 6 * SIDE
    pixels = []
    for row in range(SIDE):
        pixels.append([])
        for col in range(SIDE):
            covered = sum(sign * area(box, col, row) for sign, box in boxes)
            alpha = 1 - (1 - covered * F(str(opacity))) ** repeats
            pixels[-1].append((0, 1, 0, float(alpha)))
    unit.cell(name, (x, y, SIDE, SIDE),
              f'<svg x="{x}" y="{y}" width="{SIDE}" height="{SIDE}">{content}</svg>',
              f'<image x="{x}" y="{y}" width="{SIDE}" height="{SIDE}" href="{png(pixels)}"/>')


def rect(box, attributes=''):
    left, top, right, bottom = box
    return f'<rect x="{left}" y="{top}" width="{right-left}" height="{bottom-top}" fill="lime" {attributes}/>'


for name, box in [
    ('integer', (2, 3, 13, 12)),
    ('viewport', (0, 0, 16, 16)),
    ('clipped-left', (-2, 3, 13, 12)),
    ('clipped-top', (2, -3, 13, 12)),
    ('clipped-right', (2, 3, 20, 12)),
    ('clipped-bottom', (2, 3, 13, 20)),
    ('fractional-left', (2.25, 3, 13, 12)),
    ('fractional-top', (2, 3.25, 13, 12)),
    ('fractional-right', (2, 3, 12.75, 12)),
    ('fractional-bottom', (2, 3, 13, 11.75)),
    ('fractional-outside', (-.25, -.5, 16.75, 16.25)),
    ('empty', (20, 3, 24, 12)),
]:
    cell(name, rect(box), [(1, box)])

box = (2, 3, 13, 12)
for name, attributes, opacity in [
    ('aliased', 'shape-rendering="crispEdges"', 1),
    ('evenodd', 'fill-rule="evenodd"', 1),
    ('translucent', 'fill-opacity=".5"', .5),
    ('linear', 'color-interpolation="linearRGB"', 1),
]:
    cell(name, rect(box, attributes), [(1, box)], opacity)

cell('reversed', '<path d="M2 3V12H13V3Z" fill="lime"/>', [(1, box)])
cell('reflection', '<g transform="translate(16) scale(-1 1)">' + rect(box) + '</g>', [(1, (3, 3, 14, 12))])
cell('horizontal-retrace', '<path d="M2 3H15H13V12H2Z" fill="lime"/>', [(1, box)])
cell('hole', '<path d="M2 3H13V12H2Z M5 5V9H10V5Z" fill="lime"/>', [(1, box), (-1, (5, 5, 10, 9))])
cell('clip', '<defs><clipPath id="c">' + rect(box) + '</clipPath></defs><rect width="16" height="16" fill="lime" clip-path="url(#c)"/>', [(1, box)])
cell('mask', '<defs><mask id="m" mask-type="alpha">' + rect(box) + '</mask></defs><rect width="16" height="16" fill="lime" mask="url(#m)"/>', [(1, box)])

# Repeated integer rectangles retain exactly the same image. Fractional
# boundaries must still be composited on each repetition, not rounded away.
definitions = '<rect id="r0" x="2" y="3" width="11" height="9" fill="lime"/>'
for i in range(1, 15):
    definitions += f'<g id="r{i}"><use href="#r{i-1}"/><use href="#r{i-1}"/></g>'
cell('repeated-use', '<defs>' + definitions + '</defs><use href="#r14"/>', [(1, box)])
fractional = (2.25, 3.25, 12.75, 11.75)
cell('repeated-fractional', rect(fractional) * 2, [(1, fractional)], repeats=2)
unit.write()
