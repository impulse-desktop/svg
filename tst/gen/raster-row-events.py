"""Rational interval and polygon areas for busy rows.

Every rectangle has integer x bounds. At each pixel column the filled
region is a union (or odd-parity set) of rational y intervals. Their exact
integrals define expected alpha without path flattening or sampling. The
triangle grids use rational pixel clipping and shoelace areas instead.
"""
from fractions import Fraction as F

from unitgen import Unit, png


cases = [
    ('sparse', 0, 'normal'), ('remote', 128, 'normal'),
    ('more-remote', 512, 'normal'), ('many-remote', 2048, 'normal'),
    ('negative', 512, 'negative'), ('evenodd', 512, 'evenodd'),
    ('overlap', 192, 'overlap'), ('evenodd-overlap', 192, 'evenodd-overlap'),
    ('opposed-contours', 192, 'cancel'), ('reflected', 512, 'reflect'),
    ('subdivided', 256, 'subdivide'), ('translucent', 512, 'translucent'),
    ('following-row', 512, 'multirow'), ('nested-ends', 64, 'nested'),
    ('intersecting', 256, 'intersect'), ('source-order', 512, 'reorder'),
    ('many-endpoints', 8193, 'normal'), ('dense-starts', 1024, 'nested'),
    ('crisp-overlap', 512, 'crisp'), ('crisp-evenodd', 512, 'crisp-evenodd'),
    ('crisp-odd', 513, 'crisp-evenodd'),
]
unit = Unit('raster-row-events', 128, 32,
            'Busy rows retain the covered area of straight shapes. Remote contours, endpoints, winding, overlapping or cancelling subpaths, reflection, exact subdivisions and later rows have independent rational interval-area PNG controls. Slanted grids, opposite directions and holes have rational polygon-area controls, including clipping at the viewport.', ground=False)


def decimal(value):
    return format(float(value), '.12g')


def area(rectangles, x, y, evenodd):
    events = {F(y): 0, F(y + 1): 0}
    for left, bottom, right, top, weight in rectangles:
        if not left <= x < right:
            continue
        low, high = max(F(y), bottom), min(F(y + 1), top)
        if high <= low:
            continue
        events[low] = events.get(low, 0) + weight
        events[high] = events.get(high, 0) - weight
    cuts = sorted(events)
    winding, result = 0, F(0)
    for a, b in zip(cuts, cuts[1:]):
        winding += events[a]
        inside = bool(winding & 1) if evenodd else winding != 0
        if inside:
            result += b - a
    return result


for i, (name, count, mode) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 4
    low, high = F(101, 100), F(21, 20)
    if mode == 'multirow':
        low, high = F(3, 10), F(27, 10)
    elif mode == 'intersect':
        low, high = F(6, 5), F(31, 25)
    elif mode.startswith('crisp'):
        low, high = F(1), F(2)
    rectangles = [(0, low, 8, high, 1)]
    for j in range(count):
        step = F(4, 5) / max(1, count)
        bottom = F(11, 10) + j * step
        top = bottom + F(1, 4000)
        if mode in ('overlap', 'evenodd-overlap'):
            top = bottom + step * F(3, 2)
        elif mode == 'nested':
            bottom = F(11, 10)
            top = bottom + (j + 1) * step
        elif mode.startswith('crisp'):
            bottom, top = F(1), F(2)
        rectangle = (4 if mode == 'intersect' else 16, bottom,
                     20 if mode == 'intersect' else 24, top, 1)
        rectangles.append(rectangle)
        if mode == 'cancel':
            rectangles.append((*rectangle[:4], -1))
    if mode == 'negative':
        rectangles = [(*r[:4], -r[4]) for r in rectangles]
    if mode == 'reorder':
        rectangles.reverse()
    pieces = []
    for left, bottom, right, top, weight in rectangles:
        points = [(left, bottom), (right, bottom)]
        if mode == 'subdivide':
            points.append((right, (bottom + top) / 2))
        points.extend([(right, top), (left, top)])
        if mode == 'subdivide':
            points.append((left, (bottom + top) / 2))
        if weight < 0:
            points.reverse()
        pieces.append('M' + 'L'.join(f'{decimal(a)} {decimal(b)}' for a, b in points) + 'Z')
    evenodd = mode in ('evenodd', 'evenodd-overlap', 'crisp-evenodd')
    alpha = F(1, 5) if mode == 'translucent' else F(1)
    attributes = ' fill-opacity=".4" opacity=".5"' if mode == 'translucent' else ''
    if mode.startswith('crisp'):
        attributes += ' shape-rendering="crispEdges"'
    shape = f'<path d="{" ".join(pieces)}" fill="#3066b2" fill-rule="{"evenodd" if evenodd else "nonzero"}"{attributes}/>'
    if mode == 'reflect':
        shape = f'<g transform="translate(32 0) scale(-1 1)">{shape}</g>'
        rectangles = [(32 - r[2], r[1], 32 - r[0], r[3], r[4]) for r in rectangles]
    test = f'<g transform="translate({x} {y})">{shape}</g>'
    pixels = [[(48 / 255, 102 / 255, 178 / 255,
                float(area(rectangles, px, py, evenodd) * alpha))
               for px in range(32)] for py in range(4)]
    expected = f'<image x="{x}" y="{y}" width="32" height="4" href="{png(pixels)}"/>'
    unit.cell(name, (x, y, 32, 4), test, expected)


def clip_polygon(points, axis, boundary, lower):
    result = []
    if not points:
        return result
    for a, b in zip(points, points[1:] + points[:1]):
        inside_a = a[axis] >= boundary if lower else a[axis] <= boundary
        inside_b = b[axis] >= boundary if lower else b[axis] <= boundary
        if inside_a:
            result.append(a)
        if inside_a != inside_b:
            t = (boundary - a[axis]) / (b[axis] - a[axis])
            result.append(tuple(a[j] + t * (b[j] - a[j]) for j in range(2)))
    return result


def polygon_area(points, x, y):
    for axis, boundary, lower in [(0, x, True), (0, x + 1, False),
                                  (1, y, True), (1, y + 1, False)]:
        points = clip_polygon(points, axis, F(boundary), lower)
    if not points:
        return F(0)
    return abs(sum(a[0] * b[1] - b[0] * a[1]
                   for a, b in zip(points, points[1:] + points[:1]))) / 2


triangles = []
for row in range(8):
    for col in range(128):
        x = F(1, 10) + F(col, 4)
        y = F(row, 8) + F(1, 32) + F(col, 8192)
        triangles.append([(x, y), (x + F(1, 4), y + F(1, 32)),
                          (x, y + F(1, 16))])


def grid_areas(mode):
    pixels = [[F(0) for _ in range(32)] for _ in range(4)]
    for triangle in triangles:
        if mode == 'reflected':
            points = [(32 - x, y) for x, y in triangle]
        elif mode == 'clipped':
            points = [(x - 8, y - F(1, 16)) for x, y in triangle]
        elif mode == 'transposed':
            points = [(y, x / 32) for x, y in triangle]
        else:
            points = triangle
        low_x, high_x = min(p[0] for p in points), max(p[0] for p in points)
        low_y, high_y = min(p[1] for p in points), max(p[1] for p in points)
        for py in range(max(0, low_y.numerator // low_y.denominator),
                        min(4, -(-high_y.numerator // high_y.denominator))):
            for px in range(max(0, low_x.numerator // low_x.denominator),
                            min(32, -(-high_x.numerator // high_x.denominator))):
                pixels[py][px] += polygon_area(points, px, py)
    return pixels


grids = {mode: grid_areas(mode) for mode in ('ordinary', 'reflected', 'clipped', 'transposed')}
grid_cases = ['grid', 'grid-reverse', 'grid-opposed-winding', 'grid-evenodd',
              'grid-reflected', 'grid-holes', 'grid-evenodd-holes',
              'grid-cancel', 'grid-evenodd-cancel', 'grid-clipped', 'grid-transposed']
for i, name in enumerate(grid_cases, len(cases)):
    x, y = i % 4 * 32, i // 4 * 4
    evenodd = 'evenodd' in name
    holes = 'holes' in name
    cancelled = 'cancel' in name
    reflected = name == 'grid-reflected'
    pieces = ['M0 0H32V1H0Z'] if holes else []
    for j, triangle in enumerate(triangles):
        points = list(triangle)
        if name in ('grid-reverse', 'grid-holes') or name == 'grid-opposed-winding' and j % 2:
            points.reverse()
        loops = [points]
        if cancelled:
            loops.append(points if evenodd else list(reversed(points)))
        for loop in loops:
            pieces.append('M' + 'L'.join(f'{decimal(a)} {decimal(b)}' for a, b in loop) + 'Z')
    shape = f'<path d="{" ".join(pieces)}" fill="#3066b2" fill-rule="{"evenodd" if evenodd else "nonzero"}"/>'
    if reflected:
        shape = f'<g transform="translate(32 0) scale(-1 1)">{shape}</g>'
    elif name == 'grid-clipped':
        shape = f'<g transform="translate(-8 -.0625)">{shape}</g>'
    elif name == 'grid-transposed':
        shape = f'<g transform="matrix(0 .03125 1 0 0 0)">{shape}</g>'
    test = f'<svg x="{x}" y="{y}" width="32" height="4">{shape}</svg>'
    areas = grids.get(name.removeprefix('grid-'), grids['ordinary'])
    pixels = []
    for py in range(4):
        row = []
        for px in range(32):
            alpha = F(0) if cancelled else (F(1 if py == 0 else 0) - areas[py][px]) if holes else areas[py][px]
            row.append((48 / 255, 102 / 255, 178 / 255, float(alpha)))
        pixels.append(row)
    expected = f'<image x="{x}" y="{y}" width="32" height="4" href="{png(pixels)}"/>'
    unit.cell(name, (x, y, 32, 4), test, expected)
unit.write()
