"""Fractional pattern phase and shape coverage, from rational pixel planes.

The source has literal RGBA bytes. Each period contains fourteen source
pixels and spans fourteen output pixels, including the fractional user
space scale. Bilinear interpolation uses premultiplied samples. The host
triangle's pixel area is an exact quadratic integral of x+y < side.
Neither the expectation nor its ordinary tolerance depends on curves.
"""
import base64
from fractions import Fraction as F
from math import floor

from unitgen import Unit, png


def triangle_area(x, y, side):
    c = side - x - y
    return (max(F(0), c) ** 2 - 2 * max(F(0), c - 1) ** 2
            + max(F(0), c - 2) ** 2) / 2


pixels = [[tuple(F(v, 255) for v in
                 ((37 + 31*x) % 256, (119 + 17*y) % 256,
                  (211 + 13*x + 7*y) % 256, (0, 64, 128, 255)[(x + 2*y) % 4]))
           for x in range(14)] for y in range(14)]
premultiplied = [[tuple(c * p[3] for c in p[:3]) + (p[3],)
                  for p in row] for row in pixels]
png_source = png(pixels)
svg_source = ('<svg xmlns="http://www.w3.org/2000/svg" width="80" height="80" '
              'viewBox="0 0 14 14">' + ''.join(
                  f'<rect x="{x}" y="{y}" width="1" height="1" '
                  f'fill="rgb({round(255*p[0])},{round(255*p[1])},{round(255*p[2])})" '
                  f'fill-opacity="{float(p[3]):.17g}"/>'
                  for y, row in enumerate(pixels) for x, p in enumerate(row)) + '</svg>')
svg_source = 'data:image/svg+xml;base64,' + base64.b64encode(svg_source.encode()).decode()


def sample(x, y):
    # Pixel centres are half-integers. x and y already have that half
    # removed when mapping an output pixel centre back to source centres.
    ix, iy = floor(x), floor(y)
    tx, ty = x - ix, y - iy
    taps = [(ix, iy, (1-tx)*(1-ty)), (ix+1, iy, tx*(1-ty)),
            (ix, iy+1, (1-tx)*ty), (ix+1, iy+1, tx*ty)]
    return tuple(sum(premultiplied[yy % 14][xx % 14][c] * weight
                     for xx, yy, weight in taps) for c in range(4))


unit = Unit('pattern-coverage-product', 96, 128,
            'A fractionally phased pattern multiplies its premultiplied '
            'pixel samples by the host shape area and paint opacity. '
            'Literal RGBA bytes, exact rational bilinear interpolation '
            'and triangle/pixel integrals independently give the expected '
            'PNGs. Natural PNGs, fractionally scaled PNGs and embedded '
            'SVG images share the same fourteen-pixel period.', ground=False)
cases = [
    ('png-integer', 'png', F(0), F(0), F(28), F(1), F(1)),
    ('png-eighth', 'png', F(1, 8), F(1, 4), F(28), F(1), F(1)),
    ('png-fifteen', 'png', F('0.15'), F('0.15'), F('27.75'), F(1), F(1)),
    ('png-negative', 'png', F(-1, 4), F(-1, 8), F('27.5'), F(1), F(1)),
    ('scaled-integer', 'scaled', F(0), F(0), F(28), F(1), F(1)),
    ('scaled-eighth', 'scaled', F(1, 8), F(1, 4), F(28), F(1), F(1)),
    ('scaled-fifteen', 'scaled', F('0.15'), F('0.15'), F('27.75'), F(1), F(1)),
    ('scaled-negative', 'scaled', F(-1, 4), F(-1, 8), F('27.5'), F(1), F(1)),
    ('svg-integer', 'svg', F(0), F(0), F(28), F(1), F(1)),
    ('svg-fifteen', 'svg', F('0.15'), F('0.15'), F('27.75'), F(1), F(1)),
    ('svg-fill-opacity', 'svg', F(-1, 4), F(1, 8), F('27.5'), F('0.4'), F(1)),
    ('svg-group-opacity', 'svg', F(1, 8), F(-1, 4), F('27.75'), F('0.4'), F('0.5')),
]

for i, (name, kind, phase_x, phase_y, side, fill_opacity, group_opacity) in enumerate(cases):
    x, y = 2 + i % 3 * 32, 2 + i // 3 * 32
    scale = F(1) if kind == 'png' else F(7, 40)
    size = 14 / scale
    source = svg_source if kind == 'svg' else png_source
    test = (f'<svg x="{x}" y="{y}" width="28" height="28">'
            f'<defs><pattern id="p{i}" patternUnits="userSpaceOnUse" '
            f'x="{float(phase_x/scale):.17g}" y="{float(phase_y/scale):.17g}" '
            f'width="{float(size):.17g}" height="{float(size):.17g}" '
            f'patternTransform="scale({float(scale):.17g})">'
            f'<image width="{float(size):.17g}" height="{float(size):.17g}" '
            f'href="{source}"/></pattern></defs>'
            f'<g opacity="{float(group_opacity):.17g}">'
            f'<path d="M0 0 H{float(side):.17g} L0 {float(side):.17g} Z" '
            f'fill="url(#p{i})" fill-opacity="{float(fill_opacity):.17g}"/>'
            '</g></svg>')
    expected = []
    for row in range(28):
        line = []
        for column in range(28):
            value = sample(column-phase_x, row-phase_y)
            alpha = value[3] * triangle_area(column, row, side) * fill_opacity * group_opacity
            line.append(tuple(c / value[3] if value[3] else F(0) for c in value[:3]) + (alpha,))
        expected.append(line)
    unit.cell(name, (x, y, 28, 28), test,
              f'<image x="{x}" y="{y}" width="28" height="28" href="{png(expected)}"/>')

unit.write()
