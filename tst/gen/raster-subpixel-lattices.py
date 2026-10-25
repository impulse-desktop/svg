"""Subpixel geometry keeps its area across phases and orientations.

Disjoint polygons have exact rational vertices. Clipping each polygon to
pixel squares and summing shoelace areas gives independent alpha values.
The controls include features thin in one or both directions, reflections,
opposite winding, even-odd fill and translucency.
"""
from fractions import Fraction as F

from unitgen import Unit, png


def rectangle(x, y, width, height):
    return [(x, y), (x + width, y), (x + width, y + height), (x, y + height)]


def clip(poly, axis, boundary, lower):
    result = []
    for a, b in zip(poly, poly[1:] + poly[:1]):
        inside_a = a[axis] >= boundary if lower else a[axis] <= boundary
        inside_b = b[axis] >= boundary if lower else b[axis] <= boundary
        if inside_a:
            result.append(a)
        if inside_a != inside_b:
            t = (boundary - a[axis]) / (b[axis] - a[axis])
            result.append(tuple(a[j] + t * (b[j] - a[j]) for j in (0, 1)))
    return result


def area(poly, x, y):
    for axis, boundary, lower in [(0, F(x), True), (0, F(x + 1), False),
                                  (1, F(y), True), (1, F(y + 1), False)]:
        poly = clip(poly, axis, boundary, lower)
        if not poly:
            return F(0)
    return abs(sum(a[0] * b[1] - a[1] * b[0]
                   for a, b in zip(poly, poly[1:] + poly[:1]))) / 2


def lattice(phase):
    return [rectangle(F(1) + F(x, 16) + phase,
                      F(1) + F(y, 16) + phase, F(1, 50), F(1, 50))
            for y in range(16) for x in range(16)]


strips = [rectangle(F(0), F(y) + F(1, 2000), F(4), F(3, 100))
          for y in (1, 2)]
grid = lattice(F(1, 4000))
cases = [
    ('horizontal', strips, 'nonzero', F(1)),
    ('vertical', [[(y, x) for x, y in p] for p in strips], 'nonzero', F(1)),
    ('grid', grid, 'nonzero', F(1)),
    ('shifted', lattice(F(1, 64)), 'nonzero', F(1)),
    ('reflected', [[(4 - x, y) for x, y in p] for p in grid], 'nonzero', F(1)),
    ('reverse', [list(reversed(p)) for p in grid], 'nonzero', F(1)),
    ('evenodd', grid, 'evenodd', F(1)),
    ('translucent', grid, 'nonzero', F(1, 2)),
]
unit = Unit('raster-subpixel-lattices', 32, 4,
            'Disjoint thin strips and square lattices retain their pixel areas across subpixel phase, axis exchange, reflection, winding and opacity. Exact rational polygon clipping defines the expected alpha; no rendering samples or curve approximations enter the oracle.', ground=False)
for i, (name, polygons, rule, opacity) in enumerate(cases):
    data = ' '.join('M' + 'L'.join(f'{float(x):.12g} {float(y):.12g}' for x, y in p) + 'Z'
                    for p in polygons)
    test = f'<g transform="translate({i * 4} 0)"><path d="{data}" fill="#3066b2" fill-rule="{rule}" fill-opacity="{float(opacity)}"/></g>'
    pixels = []
    for y in range(4):
        row = []
        for x in range(4):
            coverage = sum(area(p, x, y) for p in polygons)
            assert 0 <= coverage <= 1
            row.append((48 / 255, 102 / 255, 178 / 255, float(coverage * opacity)))
        pixels.append(row)
    expected = f'<image x="{i * 4}" width="4" height="4" href="{png(pixels)}"/>'
    unit.cell(name, (i * 4, 0, 4, 4), test, expected)
unit.write()
