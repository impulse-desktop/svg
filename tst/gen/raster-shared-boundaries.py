"""Coincident boundaries preserve winding and dense endpoint coverage.

All cells draw one parallelogram, its frame, or nothing. Tiling, repeated
and opposite contours, or additional positive interior triangles cannot
change those sets. Rational clipping gives independent pixel-area PNGs.
"""
from fractions import Fraction as F

from unitgen import Unit, png


POLYGON = [(F(1, 4), F(1, 4)), (F(31, 4), F(3, 4)),
           (F(29, 4), F(31, 4)), (F(-1, 4), F(29, 4))]
COUNT = 512


def point(u, v):
    a, b, _, d = POLYGON
    return tuple(a[k] + u * (b[k] - a[k]) + v * (d[k] - a[k])
                 for k in (0, 1))


def rectangle(u0, v0, u1, v1):
    return [point(u0, v0), point(u1, v0),
            point(u1, v1), point(u0, v1)]


def data(poly):
    return 'M' + 'L'.join(f'{float(x):.17g} {float(y):.17g}'
                         for x, y in poly) + 'Z'


def area(poly, x, y):
    for axis, boundary, lower in [(0, x, True), (0, x + 1, False),
                                  (1, y, True), (1, y + 1, False)]:
        clipped = []
        for a, b in zip(poly, poly[1:] + poly[:1]):
            inside_a = a[axis] >= boundary if lower else a[axis] <= boundary
            inside_b = b[axis] >= boundary if lower else b[axis] <= boundary
            if inside_a:
                clipped.append(a)
            if inside_a != inside_b:
                t = (boundary - a[axis]) / (b[axis] - a[axis])
                clipped.append(tuple(a[k] + t * (b[k] - a[k]) for k in (0, 1)))
        poly = clipped
    return abs(sum(a[0] * b[1] - b[0] * a[1]
                   for a, b in zip(poly, poly[1:] + poly[:1]))) / 2


tiles = [rectangle(F(i, COUNT), 0, F(i + 1, COUNT), 1)
         for i in range(COUNT)]
hole = rectangle(F(1, 4), F(1, 4), F(3, 4), F(3, 4))
hole_tiles = [list(reversed(rectangle(
    F(1, 4), F(1, 4) + F(i, 2 * COUNT),
    F(3, 4), F(1, 4) + F(i + 1, 2 * COUNT))))
    for i in range(COUNT)]
# Each triangle stays strictly inside the parallelogram and turns the
# same way. Different endpoint levels and slopes exercise dense rows.
interior = []
for i in range(COUNT):
    u = F(1, 5) + F(i % 32, 64)
    v = F(1, 5) + F(i // 32, 32) + F(i, 32 * COUNT)
    interior.append([point(u, v), point(u + F(1, 64), v + F(1, 128)),
                     point(u, v + F(1, 64))])

cases = [
    ('tiles', tiles, 'nonzero', F(1)),
    ('opposite-tiles', [p if i % 2 else list(reversed(p))
                        for i, p in enumerate(tiles)], 'nonzero', F(1)),
    ('evenodd-tiles', tiles, 'evenodd', F(1)),
    ('coincident-opacity', [POLYGON] * COUNT, 'nonzero', F(1, 2)),
    ('coincident-evenodd', [POLYGON] * COUNT, 'evenodd', F(0)),
    ('opposed', [POLYGON, list(reversed(POLYGON))] * COUNT, 'nonzero', F(0)),
    ('tiled-hole', [POLYGON] + hole_tiles, 'nonzero', F(1)),
    ('dense-interior', [POLYGON] + interior, 'nonzero', F(1)),
    ('dense-reflected', [POLYGON] + interior, 'nonzero', F(1)),
]
unit = Unit('raster-shared-boundaries', 32, 24,
            'Shared and coincident polygon edges preserve signed winding, '
            'evenodd parity, holes and per-object opacity. Dense interior '
            'contours preserve their enclosing parallelogram, including '
            'reflection and fractional edge pixels. Exact rational polygon '
            'clipping and shoelace areas define independent PNG expectations.',
            ground=False)
for i, (name, polygons, rule, opacity) in enumerate(cases):
    x, y = i % 4 * 8, i // 4 * 8
    shape = (f'<path d="{" ".join(data(p) for p in polygons)}" '
             f'fill="#255f9b" fill-rule="{rule}" '
             f'fill-opacity="{float(F(1, 2) if name == "coincident-opacity" else 1)}"/>')
    expected_poly = POLYGON
    if name == 'dense-reflected':
        shape = '<g transform="translate(8 0) scale(-1 1)">' + shape + '</g>'
        expected_poly = [(8 - a, b) for a, b in POLYGON]
    test = f'<svg x="{x}" y="{y}" width="8" height="8">{shape}</svg>'
    pixels = []
    for py in range(8):
        row = []
        for px in range(8):
            coverage = area(expected_poly, px, py)
            if name == 'tiled-hole':
                coverage -= area(hole, px, py)
            row.append((37 / 255, 95 / 255, 155 / 255, float(coverage * opacity)))
        pixels.append(row)
    expected = f'<image x="{x}" y="{y}" width="8" height="8" href="{png(pixels)}"/>'
    unit.cell(name, (x, y, 8, 8), test, expected)
unit.write()
