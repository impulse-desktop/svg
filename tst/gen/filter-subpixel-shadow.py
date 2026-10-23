"""Subpixel source coverage carried through a shifted Gaussian shadow.

Expected pixels come from rectangle areas, interpolation and the Filter
Effects equations. The normalized Gaussian is summed over [-32, 32],
independently of the renderer's finite kernel and integer arithmetic.
"""
import math

from unitgen import Unit, png


def shadow(width, height, source_width, source_height, dx, dy, sx, sy,
           opacity, source_opacity):
    def coverage(count, size):
        return [max(0, min(i + 1, size) - max(i, 0)) for i in range(count)]

    def shifted(values, offset):
        def at(i):
            return values[i] if 0 <= i < len(values) else 0

        result = []
        for i in range(len(values)):
            position = i - offset
            left = math.floor(position)
            fraction = position - left
            result.append(at(left) * (1 - fraction) + at(left + 1) * fraction)
        return result

    def blurred(values, sigma):
        weights = {k: math.exp(-k * k / (2 * sigma * sigma)) for k in range(-32, 33)}
        total = sum(weights.values())
        return [sum(value * weights.get(i - j, 0) for j, value in enumerate(values)) / total
                for i in range(len(values))]

    x, y = coverage(width, source_width), coverage(height, source_height)
    bx, by = blurred(shifted(x, dx), sx), blurred(shifted(y, dy), sy)
    result = []
    for j in range(height):
        row = []
        for i in range(width):
            foreground = x[i] * y[j] * source_opacity
            # SourceAlpha atop the blurred alpha has the latter's alpha.
            # The color matrix makes it black and multiplies its alpha.
            backdrop = bx[i] * by[j] * opacity * source_opacity
            alpha = foreground + backdrop * (1 - foreground)
            gray = (215 / 255) * foreground / alpha if alpha else 0
            row.append((gray, gray, gray, alpha))
        result.append(row)
    return result


unit = Unit('filter-subpixel-shadow', 60, 36,
            'Exact source rectangle areas feed a fractional offset and normalized Gaussian. '
            'Atop preserves the blurred second input alpha, the color matrix scales it, '
            'then the gray foreground composites over the black shadow. Independent PNG '
            'expectations include line ends, filter edges, uniform and unequal transforms, '
            'an opaque row, negative offsets, translucent source, zero shadow opacity '
            'and clamped high-gain SourceAlpha before blur and out compositing.',
            ground=False)
cases = [
    ('quarter-row', 1, 1, .25, .625, .625, 1.25, 1.25, 89 / 255, 1),
    ('scaled', .25, .25, 1, 2.5, 2.5, 5, 5, 89 / 255, 1),
    ('unequal-axes', .5, 1 / 3, 1, 1.25, 2.5, 2.5, 5, 89 / 255, 1),
    ('whole-row', 1, 1, 1, .625, .625, 1.25, 1.25, 89 / 255, 1),
    ('negative-offset', 1, 1, .5, .8, .7, -.375, -.25, .7, .6),
    ('zero-shadow', 1, 1, .25, .625, .625, 1.25, 1.25, 0, 1),
]
for i, (name, sx, sy, height, sigma_x, sigma_y, dx, dy, opacity, source_opacity) in enumerate(cases):
    x, y = i % 3 * 20, i // 3 * 12
    source = (f'<rect id="s{i}" width="{13.25 / sx}" height="{height}" '
              f'fill="#d7d7d7" fill-opacity="{source_opacity}"/>')
    definition = (f'<defs>{source}<filter id="f{i}" filterUnits="userSpaceOnUse" '
                  f'x="0" y="0" width="{16 / sx}" height="{6 / sy}">'
                  f'<feOffset in="SourceAlpha" dx="{dx}" dy="{dy}" result="offset"/>'
                  f'<feGaussianBlur in="offset" stdDeviation="{sigma_x} {sigma_y}" result="blur"/>'
                  '<feComposite in="SourceAlpha" in2="blur" operator="atop"/>'
                  f'<feColorMatrix values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 {opacity} 0"/>'
                  '</filter></defs>')
    test = (definition + f'<g transform="translate({x + 2} {y + 2}) scale({sx} {sy})">'
            f'<use href="#s{i}" filter="url(#f{i})"/><use href="#s{i}"/></g>')
    pixels = shadow(16, 6, 13.25, height * sy, dx * sx, dy * sy,
                    sigma_x * sx, sigma_y * sy, opacity, source_opacity)
    expected = (f'<image x="{x + 2}" y="{y + 2}" width="16" height="6" '
                f'image-rendering="pixelated" href="{png(pixels)}"/>')
    unit.cell(name, (x, y, 20, 12), test, expected)


def hard_shadow(source_height, source_opacity, cutout):
    # The clamp is two-dimensional: at a partial end pixel, multiplying
    # independently clamped x and y would give a different answer.
    source = [[max(0, min(i + 1, 13.25) - i)
               * max(0, min(j + 1, source_height) - j) * source_opacity
               for i in range(16)] for j in range(6)]
    hard = [[min(1, 127 * a) for a in row] for row in source]

    def at(i, j):
        return hard[j][i] if 0 <= i < 16 and 0 <= j < 6 else 0

    shifted = []
    for j in range(6):
        row = []
        for i in range(16):
            x, y = i - 1.25, j - 1.25
            ix, iy = math.floor(x), math.floor(y)
            fx, fy = x - ix, y - iy
            row.append((at(ix, iy) * (1 - fx) + at(ix + 1, iy) * fx) * (1 - fy)
                       + (at(ix, iy + 1) * (1 - fx) + at(ix + 1, iy + 1) * fx) * fy)
        shifted.append(row)
    weights = {k: math.exp(-k * k / (2 * .625 ** 2)) for k in range(-32, 33)}
    total = sum(weights.values()) ** 2
    result = []
    for j in range(6):
        row = []
        for i in range(16):
            blurred = sum(shifted[y][x] * weights[i - x] * weights[j - y]
                          for y in range(6) for x in range(16)) / total
            backdrop = blurred * (1 - hard[j][i] if cutout else 1) * .25
            foreground = source[j][i]
            alpha = foreground + backdrop * (1 - foreground)
            gray = (215 / 255) * foreground / alpha if alpha else 0
            row.append((gray, gray, gray, alpha))
        result.append(row)
    return result


for i, (name, height, opacity, cutout) in enumerate([
    ('hard-alpha-row', .25, 1, False),
    ('hard-alpha-subbyte', 1, 1 / 254, False),
    ('hard-alpha-out', .25, 1, True),
]):
    x, y = i * 20, 24
    test = (f'<defs><filter id="h{i}" filterUnits="userSpaceOnUse" '
            'x="0" y="0" width="16" height="6" color-interpolation-filters="sRGB">'
            '<feColorMatrix in="SourceAlpha" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hard"/>'
            '<feOffset dx="1.25" dy="1.25"/>'
            '<feGaussianBlur stdDeviation=".625"/>'
            + ('<feComposite in2="hard" operator="out"/>' if cutout else '')
            + '<feColorMatrix values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 .25 0"/>'
            '<feBlend in="SourceGraphic"/></filter></defs>'
            f'<g transform="translate({x + 2} {y + 2})">'
            f'<rect width="13.25" height="{height}" fill="#d7d7d7" '
            f'fill-opacity="{opacity:.17g}" filter="url(#h{i})"/></g>')
    expected = (f'<image x="{x + 2}" y="{y + 2}" width="16" height="6" '
                f'image-rendering="pixelated" href="{png(hard_shadow(height, opacity, cutout))}"/>')
    unit.cell(name, (x, y, 20, 12), test, expected)
unit.write()
