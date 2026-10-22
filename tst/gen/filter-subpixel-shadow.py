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


unit = Unit('filter-subpixel-shadow', 60, 24,
            'Exact source rectangle areas feed a fractional offset and normalized Gaussian. '
            'Atop preserves the blurred second input alpha, the color matrix scales it, '
            'then the gray foreground composites over the black shadow. Independent PNG '
            'expectations include line ends, filter edges, uniform and unequal transforms, '
            'an opaque row, negative offsets, translucent source and zero shadow opacity.',
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
unit.write()
