"""A distant edge has the same local strip, independently integrated per pixel.

For slope s = +/-1/2 and stroke width 2/sqrt(5), the strip is exactly
|x - s*y - intercept| <= 1/2. Clip each pixel square by these two half
planes to obtain PNG alpha, without any SVG path or stroke in the oracle.
The visible stroke is far from both caps. Filled triangles converge to
the same strip with error below output quantization at these distances.
"""
from math import sqrt

from unitgen import Unit, png


def clipped(poly, slope, intercept, side, half_width):
    result = []
    for a, b in zip(poly, poly[1:] + poly[:1]):
        va = half_width - side * (a[0] - slope * a[1] - intercept)
        vb = half_width - side * (b[0] - slope * b[1] - intercept)
        if va >= 0:
            result.append(a)
        if (va >= 0) != (vb >= 0):
            t = va / (va - vb)
            result.append((a[0] + t * (b[0] - a[0]), a[1] + t * (b[1] - a[1])))
    return result


def pixels(slope, intercept, crisp, half_width):
    rows = []
    for y in range(8):
        row = []
        for x in range(8):
            if crisp:
                area = float(abs(x + .5 - slope * (y + .5) - intercept) < half_width)
            else:
                p = [(x, y), (x + 1, y), (x + 1, y + 1), (x, y + 1)]
                for side in (-1, 1):
                    p = clipped(p, slope, intercept, side, half_width)
                area = abs(sum(a[0] * b[1] - b[0] * a[1]
                               for a, b in zip(p, p[1:] + p[:1]))) / 2
            row.append((0, 0, 0, area))
        rows.append(row)
    return rows


cases = []
for exponent in (20, 26, 40):
    for direction in (-1, 1):
        cases.append((f'stroke-{direction}-{exponent}', exponent, direction, .5, False, False, False))
cases += [
    ('reverse-up', 26, -1, .5, True, False, False),
    ('reverse-down', 26, 1, .5, True, False, False),
    ('reflected-up', 26, -1, -.5, False, False, False),
    ('reflected-down', 26, 1, -.5, False, False, False),
    ('fill-up', 26, -1, .5, False, True, False),
    ('fill-down', 26, 1, .5, False, True, False),
    ('fill-farther-up', 40, -1, .5, False, True, False),
    ('fill-farther-down', 40, 1, .5, False, True, False),
    ('crisp-fill', 26, -1, .5, False, True, True),
    ('crisp-stroke', 26, -1, .5, False, False, True),
]
u = Unit('raster-distant-edges', 64, 64,
         'Offscreen endpoints preserve the area of their visible diagonal '
         'strip. Distance, traversal direction, reflection, stroke/fill and '
         'crisp rendering leave the mathematical strip unchanged.', ground=False)
for i, (name, exponent, direction, slope, reverse, filled, crisp) in enumerate(cases):
    x, y = i % 4 * 16, i // 4 * 16
    intercept = 2.125 if slope > 0 else 5.875
    near_y = 24 if direction < 0 else -16
    near_x = slope * near_y + intercept
    far_y = near_y + direction * 2 ** (exponent + 1)
    far_x = slope * far_y + intercept
    hint = ' shape-rendering="crispEdges"' if crisp else ''
    # Existing shape-rendering policy keeps area coverage below a pixel
    # (stroke-crisp-subpixel). Use a wider stroke to exercise crisp sampling.
    half_width = 1 if name == 'crisp-stroke' else .5
    if filled:
        path = (f'M{near_x - .5} {near_y}L{far_x} {far_y}'
                f'L{near_x + .5} {near_y}Z')
        attrs = 'fill="black"'
    else:
        a, b = (near_x, near_y), (far_x, far_y)
        if reverse:
            a, b = b, a
        path = f'M{a[0]} {a[1]}L{b[0]} {b[1]}'
        attrs = f'fill="none" stroke="black" stroke-width="{4 * half_width / sqrt(5):.17g}"'
    u.cell(name, (x, y, 16, 16),
           f'<svg x="{x + 1}" y="{y + 1}" width="8" height="8" viewBox="0 0 8 8">'
           f'<path d="{path}" {attrs}{hint}/></svg>',
           f'<image x="{x + 1}" y="{y + 1}" width="8" height="8" '
           f'href="{png(pixels(slope, intercept, crisp, half_width))}"/>')
u.write()
