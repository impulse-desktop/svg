"""Nonzero closing segments participate in the SVG 2 miter construction.

The apex uses 3-4-5 directions, so its offset lines, their intersections
and the stroke bands have rational coordinates. Exact polygon unions give
every expected pixel area. The other joins lie outside the displayed
viewport and cannot contribute to these pixels.
"""
from fractions import Fraction as F
from math import ceil, floor

from unitgen import Unit, png


def cross(a, b):
    return a[0] * b[1] - a[1] * b[0]


def subtract(a, b):
    return a[0] - b[0], a[1] - b[1]


def signed_area(polygon):
    return sum((cross(a, b) for a, b in zip(polygon, polygon[1:] + polygon[:1])), F(0)) / 2


def ccw(polygon):
    return polygon if signed_area(polygon) >= 0 else polygon[::-1]


def intersection(polygon, clip):
    for a, b in zip(clip, clip[1:] + clip[:1]):
        if not polygon:
            break
        result = []
        for p, q in zip(polygon, polygon[1:] + polygon[:1]):
            dp = cross(subtract(b, a), subtract(p, a))
            dq = cross(subtract(b, a), subtract(q, a))
            if dp >= 0:
                result.append(p)
            if (dp < 0) != (dq < 0):
                t = dp / (dp - dq)
                result.append((p[0] + t * (q[0] - p[0]), p[1] + t * (q[1] - p[1])))
        polygon = result
    return polygon


def band(p, q, normal):
    x, y = normal
    return ccw([(p[0] + x, p[1] + y), (p[0] - x, p[1] - y),
                (q[0] - x, q[1] - y), (q[0] + x, q[1] + y)])


def join(p, first, second, tip, limit):
    # Half-width is 2. SVG 2 first forms the bevel triangle; a miter
    # within its limit also includes the offset-line intersection.
    offsets = [first, tip, second] if sum(v * v for v in tip) <= (2 * limit) ** 2 else [first, second]
    return ccw([p] + [(p[0] + x, p[1] + y) for x, y in offsets])


def expected(polygons):
    # Inclusion-exclusion of intersections of the convex stroke pieces.
    # This unions their coverage once; it does not composite each piece.
    polygons = [ccw(p) for p in polygons if signed_area(p)]
    intersections = {}
    area = [[F(0) for _ in range(48)] for _ in range(14)]
    for mask in range(1, 1 << len(polygons)):
        bit = mask & -mask
        previous = mask ^ bit
        polygon = polygons[bit.bit_length() - 1]
        if previous:
            polygon = intersection(intersections[previous], polygon)
        intersections[mask] = polygon
        if not polygon or not signed_area(polygon):
            continue
        sign = 1 if mask.bit_count() % 2 else -1
        xs, ys = zip(*polygon)
        for y in range(max(0, floor(min(ys))), min(14, ceil(max(ys)))):
            for x in range(max(0, floor(min(xs))), min(48, ceil(max(xs)))):
                pixel = [(F(x), F(y)), (F(x + 1), F(y)),
                         (F(x + 1), F(y + 1)), (F(x), F(y + 1))]
                area[y][x] += sign * abs(signed_area(intersection(polygon, pixel)))
    assert all(0 <= value <= 1 for row in area for value in row)
    return png([[(F(0), F(1), F(0), value) for value in row] for row in area])


cases = [
    ('apex-zero', 'apex', F(0), 4),
    ('apex-short-positive', 'apex', F('.000002'), 4),
    ('apex-short-negative', 'apex', F('-.000002'), 4),
    ('apex-finite-positive', 'apex', F(1), 4),
    ('apex-bevel-over-limit', 'apex', F('.000002'), 3),
    ('apex-zero-bevel', 'apex', F(0), 1),
    ('corner-zero', 'corner', F(0), 4),
    ('corner-short-return', 'corner', F('-.0000014'), 4),
    ('corner-short-forward', 'corner', F('.0000014'), 4),
    ('corner-finite-return', 'corner', F(-1), 4),
]
unit = Unit('stroke-closing-direction', 100, 182,
            'A nonzero closing segment retains its own direction, even when '
            'very short. SVG2 offset-line intersections give the miter joins; '
            'a limit below the miter ratio gives a bevel. A zero closing '
            'segment instead joins the two remaining directions. Reversing '
            'the closed path changes no stroke area. Exact rational unions '
            'of straight bands and join polygons supply the expected PNGs.',
            ground=False)

for row, (name, kind, delta, limit) in enumerate(cases):
    p = (F(20 if kind == 'apex' else 12), F(8))
    q = (p[0] - delta, p[1])
    if kind == 'apex':
        points = [p, (F(4), F(20)), (F(4), F(32)), (F(36), F(32)),
                  (F(36) - delta, F(20)), q]
        pieces = [band(p, points[1], (F(-6, 5), F(-8, 5))),
                  band(points[-2], q, (F(6, 5), F(-8, 5)))]
        if delta > 0:
            pieces += [join(q, (F(-6, 5), F(8, 5)), (0, -2), (-6, -2), limit),
                       join(p, (0, -2), (F(6, 5), F(8, 5)), (6, -2), limit)]
        elif delta < 0:
            pieces += [join(q, (F(6, 5), F(-8, 5)), (0, -2), (F(2, 3), -2), limit),
                       join(p, (0, -2), (F(-6, 5), F(-8, 5)), (F(-2, 3), -2), limit)]
        else:
            pieces += [join(p, (F(6, 5), F(-8, 5)), (F(-6, 5), F(-8, 5)), (0, F(-5, 2)), limit)]
    else:
        points = [p, (F(60), F(8)), (F(60), F(32)), (q[0], F(32)), q]
        pieces = [band(p, points[1], (F(0), F(2))),
                  band(points[-2], q, (F(2), F(0)))]
        pieces += [join(q, (-2, 0), (0, -2), (-2, -2), limit) if delta >= 0 else
                   join(q, (2, 0), (0, -2), (2, -2), limit)]
        # For delta<0 the final join reverses direction. Its bevel is
        # collinear and contributes no area, as SVG2's construction gives.
    if delta:
        pieces.append(band(q, p, (F(0), F(2))))
    image = expected(pieces)
    for column in range(2):
        vertices = points if not column else [points[0]] + points[:0:-1]
        path = 'M' + ' L'.join(' '.join(format(float(v), '.17g') for v in point) for point in vertices) + ' Z'
        x, y = 1 + column * 50, 1 + row * 18
        direction = 'forward' if not column else 'reverse'
        unit.cell(f'{name}-{direction}', (x - 1, y - 1, 50, 16),
                  f'<svg x="{x}" y="{y}" width="48" height="14" overflow="hidden">'
                  f'<path d="{path}" fill="none" stroke="lime" stroke-width="4" '
                  f'stroke-linejoin="miter" stroke-miterlimit="{limit}"/></svg>',
                  f'<image x="{x}" y="{y}" width="48" height="14" href="{image}"/>')

unit.write()
