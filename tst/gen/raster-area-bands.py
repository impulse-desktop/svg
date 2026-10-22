"""Pixel coverage is the area of a band inside a pixel.

PNG controls contain the analytic alpha values, with no SVG geometry or
stroke processing shared with the unit. Viewport controls use the exact
meet scale and centered letterbox of an extremely thin document. Cubic
bands have x linear in the curve parameter, so their pixel areas are exact
polynomial integrals, independent of path flattening and scan conversion.
"""
from fractions import Fraction as F
from math import ceil

from unitgen import Unit, png

cases = [
    ('half-pixel', 'h', .75, .5, '#000000'),
    ('half-gray', 'h', .75, .5, '#636366'),
    ('whole-row', 'h', .5, 1, '#000000'),
    ('quarter-shift', 'h', 1.25, 1, '#000000'),
    ('fractional-width', 'h', 1.59538, 1.31163, '#bbbbbb'),
    ('fractional-center', 'h', 1.24609, 1, '#fff8f3'),
    ('wide-band', 'h', 2, 2.85, '#d976ff'),
    ('two-row-band', 'h', 1.18066, 2, '#40a9ff'),
    ('whole-column', 'v', .5, 1, '#dce3e6'),
    ('half-column', 'v', .75, .5, '#000000'),
    ('three-columns', 'v', 1.5, 3, '#000000'),
    ('fractional-column', 'v', 1.59538, 1.31163, '#bbbbbb'),
    ('fractional-rect', 'r', .8125, 1.1875, '#00d6c2'),
    ('partial-row', 'r', 0, .6, '#ffffff'),
    ('viewport-20', 'svg', 20, 0, '#e1eae7'),
    ('viewport-80', 'svg', 80, 0, '#ccd814'),
]
unit = Unit('raster-area-bands', 64, 24,
            'Default antialiasing preserves the area of straight and cubic bands, including fractional widths and positions. Expected alpha is the exact covered fraction of each pixel, from interval intersections or polynomial integrals. Cubic controls allow the existing 0.025px geometric approximation per boundary (D1). Thin SVG viewports retain their meet scale and letterbox.', ground=False)
for i, (name, kind, center, width, color) in enumerate(cases):
    x, y = i % 4 * 16, i // 4 * 4
    low, high = center - width / 2, center + width / 2
    if kind == 'h':
        test = f'<path d="M0 {center}H16" fill="none" stroke="{color}" stroke-width="{width}"/>'
    elif kind == 'v':
        test = f'<path d="M{center} 0V4" fill="none" stroke="{color}" stroke-width="{width}"/>'
    elif kind == 'r':
        low, high = center, center + width
        test = f'<rect y="{center}" width="16" height="{width}" fill="{color}"/>'
    else:
        height = 16 / center
        low, high = (1 - height) / 2, (1 + height) / 2
        test = f'<svg width="16" height="1" viewBox="0 0 {center} 1"><rect width="{center}" height="1" fill="{color}"/></svg>'
    rgb = tuple(int(color[j:j + 2], 16) / 255 for j in (1, 3, 5))
    pixels = []
    for py in range(4):
        row = []
        for px in range(16):
            at = px if kind == 'v' else py
            area = max(0, min(at + 1, high) - max(at, low))
            row.append((*rgb, area))
        pixels.append(row)
    unit.cell(name, (x, y, 16, 4),
              f'<g transform="translate({x} {y})">{test}</g>',
              f'<image x="{x}" y="{y}" width="16" height="4" href="{png(pixels)}"/>')


def power(ys):
    a, b, c, d = ys
    return (a, 3 * (b - a), 3 * (a - 2 * b + c), -a + 3 * b - 3 * c + d)


def split(points):
    # Exact subdivision at a quarter, with no rendering tolerance involved.
    levels = [points]
    for _ in range(3):
        row = levels[-1]
        levels.append([tuple((3 * a + b) / 4 for a, b in zip(p, q))
                       for p, q in zip(row, row[1:])])
    return ([row[0] for row in levels],
            [row[-1] for row in reversed(levels)])


def point(p):
    return ' '.join(format(float(v), '.17g') for v in p)


top = tuple(map(F, ('0.1', '0', '0.3', '0.2')))
bottom = tuple(map(F, ('0.85', '0.9', '1', '0.8')))
for i, name in enumerate(('forward', 'reverse', 'mirror', 'reflect-y',
                          'split-quarter', 'viewbox-scale', 'narrow', 'flat'), len(cases)):
    lo, hi = top, bottom
    if name == 'narrow':
        lo = tuple(map(F, ('0.1', '0.12', '0.09', '0.15')))
        hi = tuple(map(F, ('0.25', '0.4', '0.3', '0.21')))
    elif name == 'flat':
        lo, hi = (F(1, 4),) * 4, (F(3, 4),) * 4
    upper = [(F(16 * k, 3), v) for k, v in enumerate(lo)]
    lower = [(F(16 * k, 3), v) for k, v in enumerate(hi)]
    if name == 'reverse':
        upper, lower = lower, upper
    uppers = split(upper) if name == 'split-quarter' else [upper]
    lowers = split(lower) if name == 'split-quarter' else [lower]
    if name == 'viewbox-scale':
        uppers = [[tuple(4 * v for v in p) for p in row] for row in uppers]
        lowers = [[tuple(4 * v for v in p) for p in row] for row in lowers]
    d = 'M' + point(uppers[0][0])
    for row in uppers:
        d += 'C' + ' '.join(map(point, row[1:]))
    d += 'L' + point(lowers[-1][-1])
    for row in reversed(lowers):
        d += 'C' + ' '.join(map(point, reversed(row[:-1])))
    d += 'Z'
    transform = (' transform="translate(16) scale(-1 1)"' if name == 'mirror'
                 else ' transform="translate(0 1) scale(1 -1)"' if name == 'reflect-y' else '')
    test = f'<path d="{d}" fill="#ecb71e"{transform}/>'
    if name == 'viewbox-scale':
        test = '<svg width="16" height="1" viewBox="0 0 64 4">' + test + '</svg>'
    height = [b - a for a, b in zip(power(lo), power(hi))]
    areas = []
    for px in range(16):
        a, b = F(px, 16), F(px + 1, 16)
        areas.append(16 * sum(v * (b ** (j + 1) - a ** (j + 1)) / (j + 1)
                              for j, v in enumerate(height)))
    if name == 'mirror':
        areas.reverse()
    pixels = [[(236 / 255, 183 / 255, 30 / 255, float(a)) for a in areas]]
    pixels += [[(0, 0, 0, 0)] * 16 for _ in range(3)]
    x, y = i % 4 * 16, i // 4 * 4
    unit.cell('curve-' + name, (x, y, 16, 4),
              f'<g transform="translate({x} {y})">{test}</g>',
              f'<image x="{x}" y="{y}" width="16" height="4" href="{png(pixels)}"/>',
              # For x linear in t, a 0.025px geometric error is also a
              # vertical error. Two boundaries change a unit-wide pixel's
              # area by at most 2*0.025, plus one byte level for rounding.
              # This bound follows from the existing D1 approximation;
              # the expected image retains the exact polynomial integral.
              tolerance=None if name == 'flat' else ceil(255 * 2 * .025) + 1)
unit.write()
