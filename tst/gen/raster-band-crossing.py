"""Exact rational areas where a later edge crosses an earlier band group.

A vertical rectangle and a slanted band have disjoint x intervals at the
row's midpoint, but overlap near an end. Inclusion-exclusion of convex
polygon intersections gives all expected pixel areas without raster samples.
"""
from fractions import Fraction as F
from itertools import combinations

from unitgen import Unit, png


def cross(a, b):
    return a[0] * b[1] - a[1] * b[0]


def subtract(a, b):
    return a[0] - b[0], a[1] - b[1]


def signed_area(poly):
    return sum(cross(a, b) for a, b in zip(poly, poly[1:] + poly[:1])) / 2


def intersection(poly, clip):
    sign = 1 if signed_area(clip) > 0 else -1
    for a, b in zip(clip, clip[1:] + clip[:1]):
        result = []
        for p, q in zip(poly, poly[1:] + poly[:1]):
            u = sign * cross(subtract(b, a), subtract(p, a))
            v = sign * cross(subtract(b, a), subtract(q, a))
            if u >= 0:
                result.append(p)
            if (u >= 0) != (v >= 0):
                t = u / (u - v)
                result.append(tuple(p[i] + t * (q[i] - p[i]) for i in range(2)))
        poly = result
        if not poly:
            break
    return poly


def area(polygons, x, y, parity):
    pixel = [(F(x), F(y)), (F(x + 1), F(y)),
             (F(x + 1), F(y + 1)), (F(x), F(y + 1))]
    result = F(0)
    for count in range(1, len(polygons) + 1):
        coefficient = (-2 if parity else -1) ** (count - 1)
        for selected in combinations(polygons, count):
            part = pixel
            for polygon in selected:
                part = intersection(part, polygon)
            result += coefficient * abs(signed_area(part))
    assert 0 <= result <= 1
    return result


unit = Unit('raster-band-crossing', 16, 16,
            'Band groups include later edges that cross an earlier group. Exact rational convex-polygon intersections check union, cancellation, parity, both directions, source order, opacity, subdivision and later rows.', ground=False)
cases = ['union', 'reverse-order', 'reverse-winding', 'opposite-winding',
         'evenodd', 'reflected', 'turned', 'translucent',
         'two-bridges', 'following-row', 'shallow', 'subdivided',
         'small-crossings', 'many-events']
for i, name in enumerate(cases):
    rectangle = [(F(0), F(0)), (F(1, 2), F(0)),
                 (F(1, 2), F(1)), (F(0), F(1))]
    band = [(F(-1), F(0)), (F(-3, 4), F(0)),
            (F(13, 4), F(1)), (F(3), F(1))]
    if name == 'shallow':
        band = [(F(-1, 4), F(0)), (F(0), F(0)),
                (F(2), F(1)), (F(7, 4), F(1))]
    polygons = [rectangle, band]
    groups = None
    if name == 'small-crossings':
        groups = []
        narrow = [(F(-1, 2), F(0)), (F(0), F(0)),
                  (F(2), F(1)), (F(3, 2), F(1))]
        for j in range(24):
            offset = F(5 * j + 1, 128)
            groups.append([[(x / 128 + offset, y) for x, y in poly]
                           for poly in [rectangle, narrow]])
        # Their x ranges are disjoint; union areas therefore add exactly.
        polygons = [poly for group in groups for poly in group]
    elif name == 'many-events':
        polygons = [rectangle]
        for j in range(64):
            at = (F(j) + F(1, 8)) / 64
            left, right = -4096 * at - 8, -4096 * at
            polygons.append([(left, F(0)), (right, F(0)),
                             (right + 4096, F(1)), (left + 4096, F(1))])
    if name == 'two-bridges':
        polygons.append([(F(-1, 2), F(0)), (F(-3, 10), F(0)),
                         (F(27, 10), F(1)), (F(5, 2), F(1))])
    if name == 'reflected':
        polygons = [[(4 - x, y) for x, y in poly] for poly in polygons]
    elif name == 'turned':
        polygons = [[(x, 1 - y) for x, y in poly] for poly in polygons]
    elif name == 'following-row':
        polygons = [[(x, y + 1) for x, y in poly] for poly in polygons]
    source = [list(poly) for poly in polygons]
    if name == 'reverse-order':
        source.reverse()
    elif name == 'reverse-winding':
        source = [list(reversed(poly)) for poly in source]
    elif name == 'opposite-winding':
        source[0].reverse()
    elif name == 'subdivided':
        source = [[p for a, b in zip(poly, poly[1:] + poly[:1])
                   for p in [a, tuple((a[j] + b[j]) / 2 for j in range(2))]]
                  for poly in source]
    data = ' '.join('M' + 'L'.join(f'{float(x):.17g} {float(y):.17g}'
                                 for x, y in poly) + 'Z' for poly in source)
    alpha = F(1, 2) if name == 'translucent' else F(1)
    parity = name in ['opposite-winding', 'evenodd']
    def expected(x, y):
        if groups is not None:
            return sum(area(group, x, y, False) for group in groups)
        if name == 'many-events':
            # The parallel bands are disjoint; each can overlap only R.
            base = area([rectangle], x, y, False)
            return base + sum(area([rectangle, poly], x, y, False) - base
                              for poly in polygons[1:])
        return area(polygons, x, y, parity)

    pixels = [[(48 / 255, 102 / 255, 178 / 255, float(expected(x, y) * alpha))
               for x in range(4)] for y in range(4)]
    x, y = i % 4 * 4, i // 4 * 4
    test = (f'<svg x="{x}" y="{y}" width="4" height="4">'
            f'<path d="{data}" fill="#3066b2" opacity="{float(alpha)}" '
            f'fill-rule="{"evenodd" if name == "evenodd" else "nonzero"}"/></svg>')
    unit.cell(name, (x, y, 4, 4), test,
              f'<image x="{x}" y="{y}" width="4" height="4" href="{png(pixels)}"/>')

unit.write()
