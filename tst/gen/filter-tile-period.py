"""feTile keeps the reference subregion's period in its user space.

Fraction arithmetic selects whole output pixels strictly inside a known
opaque or transparent stripe. Neither raster rounding nor a sampling
kernel can move these probes across an edge. Constant subpixel tiles
also check that the clipped edge coverage is not repeated as a seam.
"""
from fractions import Fraction as F

from unitgen import Unit


def inverse(m, x, y):
    a, b, c, d, e, f = m
    det = a * d - b * c
    x, y = x - e, y - f
    return ((d * x - c * y) / det, (a * y - b * x) / det)


identity = (1, 0, 0, 1, 0, 0)
cases = [
    ('integer', 4, 12, 8, identity, 'stripe', 1),
    ('fractional-period', F('2.4'), F('12.8'), 8, identity, 'stripe', 1),
    ('fractional-origin', F('2.4'), 12, 8, identity, 'stripe', 1),
    ('negative-origin', F('-2.4'), F('12.8'), 8, identity, 'stripe', 1),
    ('scaled', F('1.25'), 16, 10, (F('.8'), 0, 0, F('.8'), 0, 0), 'stripe', 1),
    ('unequal-axes', F('1.25'), 16, 6, (F('.8'), 0, 0, F('1.2'), 2, 0), 'stripe', 1),
    ('reflected', F('2.4'), F('12.8'), 8, (-1, 0, 0, 1, 96, 0), 'stripe', 1),
    ('quarter-turn', F('2.4'), F('12.8'), 8, (0, 1, -1, 0, 30, -4), 'stripe', 1),
    ('rotated', F('2.4'), F('12.8'), 8, (F('.8'), F('.6'), F('-.6'), F('.8'), 16, -4), 'stripe', 1),
    ('sheared', F('2.4'), F('12.8'), 8, (1, F('.25'), F('.375'), 1, 5, -2), 'stripe', 1),
    ('subpixel-solid', F('.25'), F('.5'), F('.75'), identity, 'solid', 1),
    ('subpixel-translucent', F('.25'), F('.5'), F('.75'), identity, 'solid', F('.5')),
    ('zero-width', 4, 0, 8, identity, 'empty', 1),
    ('clipped-input', F('2.4'), F('12.8'), 8, identity, 'clipped', 1),
    ('offset-chain', F('2.4'), F('12.8'), 8, identity, 'offset', 1),
    ('subpixel-colour', F('.25'), F('.25'), F('.5'), identity, 'solid', F('.6')),
]

unit = Unit('filter-tile-period', 200, 224,
            'Whole-pixel probes of a periodically repeated reference stripe. '
            'Exact rational inverse transforms place each probe at least two '
            'user units from the reference edges. Opaque probes are lime; '
            'transparent probes leave the white ground. The reference origin '
            'and period survive fractional positions, scaling, reflection, '
            'rotation, shear, and clipping by the filter region. Constant '
            'tiles smaller than a pixel retain their opacity. A zero-width '
            'reference is empty. The offset chain matches a real SVG pipeline.')

for i, (name, origin, width, height, transform, kind, opacity) in enumerate(cases):
    origin, width, height, opacity = map(F, (origin, width, height, opacity))
    transform = tuple(map(F, transform))
    colour = '#804020' if name == 'subpixel-colour' else 'lime'
    clip_left = origin + 2 if kind == 'clipped' else F(-128)
    probes, plain = [], []
    green = clear = 0
    for y in range(2, 23, 3):
        for x in range(2, 95):
            corners = [inverse(transform, F(px), F(py))
                       for px, py in [(x, y), (x + 1, y), (x + 1, y + 1), (x, y + 1)]]
            if any(u < clip_left + 2 for u, v in corners):
                continue
            if kind in ('solid', 'empty'):
                lit = kind == 'solid'
            else:
                phases = [((u - origin) % width, v % height) for u, v in corners]
                inside = all(width / 2 + 2 <= u <= width - 2 and 2 <= v <= height - 2 for u, v in phases)
                outside = all(2 <= u <= width / 2 - 2 and 2 <= v <= height - 2 for u, v in phases)
                if not (inside or outside):
                    continue
                lit = inside
            probes.append(f'<rect x="{x}" y="{y}" width="1" height="1"/>')
            if lit:
                green += 1
                plain.append(f'<rect x="{x}" y="{y}" width="1" height="1" fill="{colour}" fill-opacity="{float(opacity)}"/>')
            else:
                clear += 1
    assert probes and (green or kind == 'empty'), name
    assert clear or kind == 'solid', name
    left, top = 2 + i % 2 * 100, 2 + i // 2 * 28
    matrix = ' '.join(str(float(v)) for v in transform)
    source_x = origin if kind in ('solid', 'empty', 'offset') else origin + width / 2
    source_width = width if kind in ('solid', 'empty', 'offset') else width / 2
    dx = width / 2 if kind == 'offset' else 0
    drawing = (f'<svg x="{left}" y="{top}" width="96" height="24" overflow="hidden">'
               f'<defs><clipPath id="probes{i}">' + ''.join(probes) + '</clipPath>'
               f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="{float(clip_left)}" y="-64" width="{float(256 - clip_left)}" height="256">'
               f'<feFlood x="{float(source_x)}" y="0" width="{float(source_width)}" height="{float(height)}" flood-color="{colour}" flood-opacity="{float(opacity)}" result="a"/>'
               f'<feOffset in="a" dx="{float(dx)}" x="{float(origin)}" y="0" width="{float(width)}" height="{float(height)}" result="b"/>'
               '<feTile in="b"/></filter></defs>'
               f'<g clip-path="url(#probes{i})"><g transform="matrix({matrix})">'
               f'<rect width="96" height="24" filter="url(#f{i})"/></g></g></svg>')
    expected = f'<g transform="translate({left} {top})">' + ''.join(plain) + '</g>'
    unit.cell(name, (left - 1, top - 1, 98, 26), drawing, expected)

unit.write()
