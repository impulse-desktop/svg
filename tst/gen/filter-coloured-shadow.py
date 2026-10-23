"""A coloured source shifted, darkened, blurred and blended with itself.

Expected pixels use exact rectangle areas and the filter equations. No
renderer output or renderer kernel is used to construct the expectation.
"""
import math

from unitgen import Unit, png


def expected(linear, opacity, dx, dy, sigma_x, sigma_y, gain, shadow_alpha):
    size = 16
    source = [[max(0, min(x + 1, 10.75) - max(x, 3.25))
               * max(0, min(y + 1, 7.75) - max(y, 2.5)) * opacity
               for x in range(size)] for y in range(size)]

    def at(values, x, y):
        return values[y][x] if 0 <= x < size and 0 <= y < size else 0

    shifted = []
    for y in range(size):
        row = []
        for x in range(size):
            px, py = x - dx, y - dy
            ix, iy = math.floor(px), math.floor(py)
            fx, fy = px - ix, py - iy
            row.append((at(source, ix, iy) * (1 - fx) + at(source, ix + 1, iy) * fx) * (1 - fy)
                       + (at(source, ix, iy + 1) * (1 - fx) + at(source, ix + 1, iy + 1) * fx) * fy)
        shifted.append(row)

    blurred = shifted
    for horizontal, sigma in [(True, sigma_x), (False, sigma_y)]:
        weights = [(k, math.exp(-k * k / (2 * sigma * sigma))) for k in range(-32, 33)] if sigma else [(0, 1)]
        total = sum(weight for k, weight in weights)
        blurred = [[sum(at(blurred, x - k if horizontal else x, y if horizontal else y - k) * weight / total
                        for k, weight in weights) for x in range(size)] for y in range(size)]

    def decode(c):
        return c / 12.92 if c <= .04045 else ((c + .055) / 1.055) ** 2.4

    def encode(c):
        return 12.92 * c if c <= .0031308 else 1.055 * c ** (1 / 2.4) - .055

    colour = [1, 128 / 255, 64 / 255]
    working = [decode(c) for c in colour] if linear else colour
    result = []
    for y in range(size):
        row = []
        for x in range(size):
            a, b = source[y][x], blurred[y][x] * shadow_alpha
            alpha = a + b * (1 - a)
            # feColorMatrix changes straight RGB; the blur averages the
            # premultiplied channels, then feBlend composites SourceGraphic.
            rgb = [c * (a + gain * b * (1 - a)) / alpha if alpha else 0 for c in working]
            if linear:
                rgb = [encode(c) for c in rgb]
            row.append((*rgb, alpha))
        result.append(row)
    return result


unit = Unit('filter-coloured-shadow', 80, 60,
            'Exact rectangle coverage feeds feOffset, a diagonal RGB colour matrix, '
            'a normalized Gaussian and normal SourceGraphic blending. Independent '
            'expectations check default linearRGB, sRGB, translucent source, '
            'negative fractional offset, zero blur, a black shadow, zero shadow '
            'alpha, blur in only one axis and deviations just below two pixels.',
            ground=False)

for i, (name, linear, opacity, dx, dy, sx, sy, gain, alpha) in enumerate([
    ('linear-default', True, 1, .75, 1.25, .6, .75, .2, 1),
    ('srgb', False, 1, .75, 1.25, .6, .75, .2, 1),
    ('translucent', True, .5, .75, 1.25, .6, .75, .2, 1),
    ('negative-offset', True, 1, -1.25, -.5, .6, .75, .2, 1),
    ('zero-blur', True, 1, .75, 1.25, 0, 0, .2, 1),
    ('black-shadow', True, 1, .75, 1.25, .6, .75, 0, 1),
    ('zero-shadow', True, 1, .75, 1.25, .6, .75, .2, 0),
    ('one-axis', True, 1, .75, 1.25, 0, .75, .2, 1),
    ('near-two', True, 1, .75, 1.25, 1.95, 1.9, .2, 1),
]):
    x, y = 2 + i % 4 * 20, 2 + i // 4 * 20
    space = '' if linear else ' color-interpolation-filters="sRGB"'
    test = (f'<svg x="{x}" y="{y}" width="16" height="16" overflow="hidden">'
            f'<defs><filter id="f{i}" filterUnits="userSpaceOnUse" x="0" y="0" width="16" height="16"{space}>'
            f'<feOffset in="SourceGraphic" dx="{dx}" dy="{dy}" result="shift"/>'
            f'<feColorMatrix in="shift" values="{gain} 0 0 0 0 0 {gain} 0 0 0 0 0 {gain} 0 0 0 0 0 {alpha} 0" result="dark"/>'
            f'<feGaussianBlur in="dark" stdDeviation="{sx} {sy}" result="blur"/>'
            '<feBlend in="SourceGraphic" in2="blur" mode="normal"/></filter></defs>'
            f'<rect x="3.25" y="2.5" width="7.5" height="5.25" fill="#ff8040" fill-opacity="{opacity}" filter="url(#f{i})"/>'
            '</svg>')
    plain = (f'<image x="{x}" y="{y}" width="16" height="16" image-rendering="pixelated" '
             f'href="{png(expected(linear, opacity, dx, dy, sx, sy, gain, alpha))}"/>')
    unit.cell(name, (x - 1, y - 1, 18, 18), test, plain)

unit.write()
