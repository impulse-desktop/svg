"""Literal rectangle samples retain each already covered boundary once.

The displacement equation is evaluated on exact rational source pixels.
Uniform maps translate their hard rectangle; a map varying on one axis
still leaves the other axis unchanged. Real cuts remain fractional cuts.
"""
from fractions import Fraction as F
import math
from unitgen import Unit, png


SIZE = 16
SOURCE = tuple(map(F, ['3.25', '3.125', '11.75', '10.875']))
INNER = tuple(map(F, ['4.25', '4.125', '10.75', '9.875']))
CASES = [
    ('zero', 0, '.5', '.5', 'source', 'same'),
    ('neutral', 1, '.5', '.5', 'source', 'same'),
    ('neutral-negative', -2, '.5', '.5', 'source', 'same'),
    ('neutral-translucent', 2, '.5', '.5', 'half', 'same'),
    ('neutral-alpha', 2, '.5', '.5', 'alpha', 'same'),
    ('neutral-prior-result', 1, '.5', '.5', 'flood', 'same'),
    ('neutral-fill-paint', 1, '.5', '.5', 'fill', 'same'),
    ('neutral-smaller-region', 1, '.5', '.5', 'source', 'inner'),
    ('shift-left', 2, '1', '.5', 'source', 'same'),
    ('shift-right', 2, '0', '.5', 'source', 'same'),
    ('shift-up', 2, '.5', '1', 'source', 'same'),
    ('shift-down', 2, '.5', '0', 'source', 'same'),
    ('fractional-x', 2, '.75', '.5', 'source', 'same'),
    ('fractional-y', 2, '.5', '.25', 'source', 'same'),
    ('fractional-both', 2, '.75', '.25', 'source', 'same'),
    ('negative-scale', -2, '.75', '.25', 'source', 'same'),
    ('vary-x', 1, 'vary', '.5', 'source', 'same'),
    ('vary-y', 1, '.5', 'vary', 'source', 'same'),
    ('vary-both', 1, 'vary', 'vary', 'source', 'same'),
    ('vary-x-translucent', 1, 'vary', '.5', 'half', 'same'),
    ('vary-y-alpha', 1, '.5', 'vary', 'alpha', 'same'),
    ('vary-x-inner', 1, 'vary', '.5', 'source', 'inner'),
    ('neutral-shared-input', 1, '.5', '.5', 'shared', 'same'),
    ('neutral-chain', 1, '.5', '.5', 'chain', 'same'),
]
unit = Unit('filter-displacement-edges', 64, 96,
            'Exact rectangle samples through neutral, uniform and varying '
            'displacement maps. Preserve existing coverage on unchanged or '
            'translated hard sides, while real primitive cuts still apply. '
            'Source alpha, translucency, prior results, infinite paint, '
            'shared readers and a following primitive are independent controls.',
            ground=False)


def area(x, y, box):
    return max(F(0), min(x + 1, box[2]) - max(x, box[0])) * max(F(0), min(y + 1, box[3]) - max(y, box[1]))


for i, (name, scale, red, green, kind, frame) in enumerate(CASES):
    scale = F(scale)
    region = INNER if frame == 'inner' else SOURCE
    alpha = F(1, 2) if kind == 'half' else F(1)
    def sample(x, y):
        if not (0 <= x < SIZE and 0 <= y < SIZE):
            return F(0)
        return F(1) if kind == 'fill' else area(x, y, SOURCE) * alpha
    def channel(value, x, y, axis):
        return F((x if axis == 0 else y) % 2) if value == 'vary' else F(value)
    expected = []
    for y in range(SIZE):
        row = []
        for x in range(SIZE):
            dx = scale * (channel(red, x, y, 0) - F(1, 2))
            dy = scale * (channel(green, x, y, 1) - F(1, 2))
            px, py = F(x) + dx, F(y) + dy
            ix, iy = math.floor(px), math.floor(py)
            tx, ty = px - ix, py - iy
            value = (sample(ix, iy) * (1 - tx) * (1 - ty)
                     + sample(ix + 1, iy) * tx * (1 - ty)
                     + sample(ix, iy + 1) * (1 - tx) * ty
                     + sample(ix + 1, iy + 1) * tx * ty)
            for axis, (field, move, pixel) in enumerate([(red, dx, x), (green, dy, y)]):
                # Uniform maps translate the geometric side by -move.
                # A varying axis can move source samples across this frame,
                # so both sides of that axis remain genuine cuts.
                variable = field == 'vary'
                infinite = kind == 'fill'
                lo = region[axis] if variable or infinite or region[axis] > SOURCE[axis] - move else F(pixel)
                hi = region[axis + 2] if variable or infinite or region[axis + 2] < SOURCE[axis + 2] - move else F(pixel + 1)
                value *= max(F(0), min(pixel + 1, hi) - max(pixel, lo))
            colour = (0, 0, 0) if kind == 'alpha' else (0, 1, 0)
            row.append((*colour, float(value)))
        expected.append(row)
    varying = red == 'vary' or green == 'vary'
    if varying:
        uri = png([[(float(channel(red, x, y, 0)), float(channel(green, x, y, 1)), 0, 1)
                    for x in range(SIZE)] for y in range(SIZE)])
        map_node = f'<feImage x="0" y="0" width="16" height="16" href="{uri}" result="map"/>'
    else:
        colour = f'rgb({float(F(red) * 100)}%,{float(F(green) * 100)}%,0%)'
        map_node = f'<feFlood flood-color="{colour}" result="map"/>'
    source = 'SourceAlpha' if kind == 'alpha' else 'FillPaint' if kind == 'fill' else 'SourceGraphic'
    prior = ''
    if kind == 'flood':
        prior = '<feFlood x="3.25" y="3.125" width="8.5" height="7.75" flood-color="lime" result="paint"/>'
        source = 'paint'
    extra = '<feOffset result="source-reader"/>' if kind == 'shared' else ''
    tail = '<feOffset/>' if kind == 'chain' else ''
    x, y = i % 4 * SIZE, i // 4 * SIZE
    ident = 'f' + str(i)
    cut = f'x="{float(region[0])}" y="{float(region[1])}" width="{float(region[2] - region[0])}" height="{float(region[3] - region[1])}"'
    nodes = (prior + extra + map_node + f'<feDisplacementMap in="{source}" in2="map" scale="{float(scale)}" '
             f'xChannelSelector="R" yChannelSelector="G" {cut}/>' + tail)
    drawing = (f'<svg x="{x}" y="{y}" width="16" height="16"><defs><filter id="{ident}" '
               f'filterUnits="userSpaceOnUse" x="0" y="0" width="16" height="16" color-interpolation-filters="sRGB">'
               f'{nodes}</filter></defs><rect x="3.25" y="3.125" width="8.5" height="7.75" '
               f'fill="lime" fill-opacity="{float(alpha)}" filter="url(#{ident})"/></svg>')
    unit.cell(name, (x, y, SIZE, SIZE), drawing,
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(expected)}"/>')
unit.write()
