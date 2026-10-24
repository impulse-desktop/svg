"""Literal alpha pixels through morphology, offset, blur and an outer shadow.

The source is a PNG. Integer-neighbourhood extrema, bilinear translation,
normalized Gaussian samples and SourceAlpha-out are evaluated separately
from the renderer. Pixelated integer scaling supplies known device pixels.
"""
import math
from unitgen import Unit, png

SIZE = 16
SOURCE = [[0.0 for _ in range(7)] for _ in range(7)]
for y in range(1, 4):
    for x in range(1, 4):
        SOURCE[y][x] = 128 / 255
for y in range(3, 6):
    for x in range(3, 6):
        SOURCE[y][x] = 1.0
SOURCE[2][4] = 1.0
URI = png([[(0, 0, 0, a) for a in row] for row in SOURCE])


def empty():
    return [[0.0 for _ in range(SIZE)] for _ in range(SIZE)]


def shifted(image, dx, dy):
    return [[image[y - dy][x - dx] if 0 <= x - dx < SIZE and 0 <= y - dy < SIZE else 0.0
             for x in range(SIZE)] for y in range(SIZE)]


def translated(image, dx, dy):
    ix, iy = math.floor(dx), math.floor(dy)
    tx, ty = dx - ix, dy - iy
    result = empty()
    for xoff, yoff, weight in [(ix, iy, (1 - tx) * (1 - ty)),
                                (ix + 1, iy, tx * (1 - ty)),
                                (ix, iy + 1, (1 - tx) * ty),
                                (ix + 1, iy + 1, tx * ty)]:
        part = shifted(image, xoff, yoff)
        for y in range(SIZE):
            for x in range(SIZE):
                result[y][x] += part[y][x] * weight
    return result


def kernel(sigma):
    if not sigma:
        return [1.0]
    if sigma < 2:
        radius = math.ceil(3 * sigma)
        values = [math.exp(-x * x / (2 * sigma * sigma)) for x in range(-radius, radius + 1)]
        return [v / sum(values) for v in values]
    size = math.floor(sigma * 3 * math.sqrt(2 * math.pi) / 4 + .5)
    result = [1.0]
    for width in [size, size, size if size % 2 else size + 1]:
        following = [0.0] * (len(result) + width - 1)
        for i, a in enumerate(result):
            for j in range(width):
                following[i + j] += a / width
        result = following
    return result


def blurred(image, sx, sy):
    for horizontal, sigma in [(True, sx), (False, sy)]:
        values = kernel(sigma)
        half = len(values) // 2
        result = empty()
        for i, weight in enumerate(values):
            part = shifted(image, i - half if horizontal else 0, 0 if horizontal else i - half)
            for y in range(SIZE):
                for x in range(SIZE):
                    result[y][x] += part[y][x] * weight
        image = result
    return image


cases = [
    ('rounded-copy', 'dilate', .36, .36, 0, .36, .24, .24, 1, 1, .203070367, 1),
    ('below-half', 'dilate', .49, .49, .5, -.25, .6, .6, 1, 1, .75, 1),
    ('above-half', 'dilate', .51, .51, .25, .75, .6, .6, 1, 1, .75, 1),
    ('erode', 'erode', .51, .51, -.25, .5, .6, .6, 1, 1, .75, 1),
    ('unequal', 'dilate', .49, 1.51, -.5, .25, .6, .9, 1, 1, .75, 1),
    ('large-blur', 'dilate', 1, 1, -.5, .5, 2, 2, 1, 1, .75, 1),
    ('scaled-axes', 'dilate', .26, .49, .25, .5, .4, .7, 2, 1, .75, 1),
    ('group-opacity', 'dilate', .75, .75, 1.25, -.75, .8, .8, 1, 1, .75, .5),
]
unit = Unit('filter-morphology-shadow', 64, 32,
            'Literal alpha pixels pass through rectangular morphology, '
            'a fractional offset, Gaussian blur, SourceAlpha-out and a '
            'shadow-alpha matrix. Independent pixel operations cover '
            'sampled radii below and above half a device pixel, erosion, '
            'unequal axes, the existing large Gaussian approximation, '
            'nonuniform scale and final group opacity.', ground=False)
for i, (name, operator, rx, ry, dx, dy, bx, by, sx, sy, strength, opacity) in enumerate(cases):
    source = empty()
    for y in range(7):
        for x in range(7):
            for yy in range(sy):
                for xx in range(sx):
                    source[(y + 1) * sy + yy][(x + 1) * sx + xx] = SOURCE[y][x]
    ix, iy = math.floor(rx * sx + .5), math.floor(ry * sy + .5)
    neighbours = [shifted(source, x, y) for y in range(-iy, iy + 1) for x in range(-ix, ix + 1)]
    extreme = max if operator == 'dilate' else min
    morph = [[extreme(a[y][x] for a in neighbours) for x in range(SIZE)] for y in range(SIZE)]
    blur = blurred(translated(morph, dx * sx, dy * sy), bx * sx, by * sy)
    expected = png([[(0, 0, 0, blur[y][x] * (1 - source[y][x]) * strength * opacity)
                     for x in range(SIZE)] for y in range(SIZE)])
    x, y = i % 4 * SIZE, i // 4 * SIZE
    f = (f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="0" y="0" '
         f'width="{SIZE / sx}" height="{SIZE / sy}" color-interpolation-filters="sRGB">'
         f'<feMorphology in="SourceAlpha" operator="{operator}" radius="{rx} {ry}" result="spread"/>'
         f'<feOffset in="spread" dx="{dx}" dy="{dy}" result="offset"/>'
         f'<feGaussianBlur in="offset" stdDeviation="{bx} {by}" result="blur"/>'
         '<feComposite in="blur" in2="SourceAlpha" operator="out"/>'
         f'<feColorMatrix values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 {strength} 0"/></filter>')
    unit.cell(name, (x, y, SIZE, SIZE),
              f'<svg x="{x}" y="{y}" width="16" height="16"><defs>{f}</defs>'
              f'<g opacity="{opacity}" transform="scale({sx} {sy})">'
              f'<image x="1" y="1" width="7" height="7" image-rendering="pixelated" '
              f'href="{URI}" filter="url(#f{i})"/></g></svg>',
              f'<image x="{x}" y="{y}" width="16" height="16" href="{expected}"/>')
unit.write()
