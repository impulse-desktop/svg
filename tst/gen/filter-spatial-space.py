"""Spatial filter operations keep the input's existing colour space.

Literal native-size PNG pixels, transfer functions and bilinear weights
give independent expectations. Tile probes stay strictly inside a copy,
so this tests colour mixing without choosing a fractional seam algorithm.
"""
import math

from unitgen import Unit, png


SIZE = 16
COLOURS = [(255, 0, 0, 255), (0, 0, 255, 128), (64, 192, 128, 255)]
PIXELS = [[tuple(v / 255 for v in COLOURS[(x + y) % 3])
           for x in range(6)] for y in range(6)]
IMAGE = png(PIXELS)
IDENTITY = '1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0'

# name, prior space, requested space, x/y shift or tile period,
# following reader, input, group opacity, output cut
OFFSET = [
    ('offset-default', None, None, .5, .25, '', 'source', 1, ''),
    ('offset-srgb', None, 'sRGB', .5, .25, '', 'source', 1, ''),
    ('offset-linear', None, 'linearRGB', .5, .25, '', 'source', 1, ''),
    ('offset-prior-srgb', 'sRGB', 'linearRGB', .5, .25, '', 'source', 1, ''),
    ('offset-prior-linear', 'linearRGB', 'sRGB', .5, .25, '', 'source', 1, ''),
    ('offset-negative', None, 'linearRGB', -.5, -.25, '', 'source', 1, ''),
    ('offset-alpha', None, 'linearRGB', .5, .25, '', 'alpha', 1, ''),
    ('offset-opacity', None, 'linearRGB', .5, .25, '', 'source', .5, ''),
    ('offset-zero', None, 'linearRGB', 0, 0, '', 'source', 1, ''),
    ('offset-integer', None, 'linearRGB', 1, -1, '', 'source', 1, ''),
    ('offset-linear-reader', None, 'linearRGB', .5, .25, 'matrix', 'source', 1, ''),
    ('offset-linear-blend', None, 'linearRGB', .5, .25, 'linearRGB', 'source', 1, ''),
    ('offset-prior-linear-blend', 'linearRGB', 'sRGB', .5, .25, 'sRGB', 'source', 1, ''),
    ('offset-smaller-region', None, 'linearRGB', .5, .25, '', 'source', 1, 'inner'),
    ('offset-fill-paint', None, 'linearRGB', .5, .25, '', 'fill', 1, 'inner'),
    ('offset-shared-srgb-reader', None, 'linearRGB', .5, .25, 'sRGB', 'source', 1, ''),
]
TILE = [
    ('tile-default', None, None, 5.5, 5.5, '', 'source', 1, ''),
    ('tile-srgb', None, 'sRGB', 5.5, 5.5, '', 'source', 1, ''),
    ('tile-linear', None, 'linearRGB', 5.5, 5.5, '', 'source', 1, ''),
    ('tile-prior-srgb', 'sRGB', 'linearRGB', 5.5, 5.5, '', 'source', 1, ''),
    ('tile-prior-linear', 'linearRGB', 'sRGB', 5.5, 5.5, '', 'source', 1, ''),
    ('tile-alpha', None, 'linearRGB', 5.5, 5.5, '', 'alpha', 1, ''),
    ('tile-opacity', None, 'linearRGB', 5.5, 5.5, '', 'source', .5, ''),
    ('tile-integer', None, 'linearRGB', 6, 6, '', 'source', 1, ''),
    ('tile-integer-x', None, 'linearRGB', 6, 5.5, '', 'source', 1, ''),
    ('tile-integer-y', None, 'linearRGB', 5.5, 6, '', 'source', 1, ''),
    ('tile-linear-reader', None, 'linearRGB', 5.5, 5.5, 'matrix', 'source', 1, ''),
    ('tile-linear-blend', None, 'linearRGB', 5.5, 5.5, 'linearRGB', 'source', 1, ''),
    ('tile-prior-linear-blend', 'linearRGB', 'sRGB', 5.5, 5.5, 'sRGB', 'source', 1, ''),
    ('tile-prior-linear-default', 'linearRGB', None, 5.5, 5.5, '', 'source', 1, ''),
    ('tile-prior-srgb-default', 'sRGB', None, 5.5, 5.5, '', 'source', 1, ''),
    ('tile-shared-srgb-reader', None, 'linearRGB', 5.5, 5.5, 'sRGB', 'source', 1, ''),
]


def linear(v):
    return v / 12.92 if v <= .04045 else ((v + .055) / 1.055) ** 2.4


def srgb(v):
    return v * 12.92 if v <= .0031308 else 1.055 * max(0, v) ** (1 / 2.4) - .055


def convert(p, source, target):
    if source == target or not p[3]:
        return p[:]
    curve = linear if target == 'linearRGB' else srgb
    return [curve(v / p[3]) * p[3] for v in p[:3]] + [p[3]]


def original(x, y):
    if 4 <= x < 10 and 4 <= y < 10:
        p = PIXELS[y - 4][x - 4]
        return [v * p[3] for v in p[:3]] + [p[3]]
    return [0, 0, 0, 0]


unit = Unit('filter-spatial-space', 64, 128,
            'Offset and tile preserve the sampled image current space, '
            'regardless of color-interpolation-filters. Literal RGBA taps '
            'check fractional, negative, zero and integer offsets, tile '
            'periods, prior conversions, alpha, opacity, real subregion cuts, '
            'paint input, shared source readers and later conversions/blends.',
            ground=False)
for i, case in enumerate(OFFSET + TILE):
    name, prior, requested, dx, dy, tail, kind, opacity, cut = case
    tiled = i >= len(OFFSET)
    current = prior or 'sRGB'

    def source_pixel(x, y):
        if kind == 'fill':
            return [64 / 255, 128 / 255, 192 / 255, 1]
        p = convert(original(x, y), 'sRGB', current)
        return [0, 0, 0, p[3]] if kind == 'alpha' else p

    def expected(x, y):
        if tiled:
            if not (10 <= x < 13 and 10 <= y < 13):
                return (0, 0, 0, 0)
            px = 4 + ((x + .5 - 4) % dx) - .5
            py = 4 + ((y + .5 - 4) % dy) - .5
        else:
            px, py = x - dx, y - dy
        ix, iy = math.floor(px), math.floor(py)
        tx, ty = px - ix, py - iy
        taps = [(source_pixel(ix, iy), (1 - tx) * (1 - ty)),
                (source_pixel(ix + 1, iy), tx * (1 - ty)),
                (source_pixel(ix, iy + 1), (1 - tx) * ty),
                (source_pixel(ix + 1, iy + 1), tx * ty)]
        if tiled:
            assert 4 <= ix and ix + 1 < 4 + dx
            assert 4 <= iy and iy + 1 < 4 + dy
        p = [sum(v[k] * w for v, w in taps) for k in range(4)]
        if cut:
            area = max(0, min(x + 1, 8.75) - max(x, 5.25))
            area *= max(0, min(y + 1, 8.875) - max(y, 5.125))
            p = [v * area for v in p]
        space = 'linearRGB' if tail == 'matrix' else tail
        if tail:
            p = convert(p, current, space)
            if tail != 'matrix':
                backdrop = convert(original(x, y), 'sRGB', space)
                p = [v + b * (1 - p[3]) for v, b in zip(p, backdrop)]
            p = convert(p, space, 'sRGB')
        else:
            p = convert(p, current, 'sRGB')
        p = [v * opacity for v in p]
        return tuple(v / p[3] if p[3] else 0 for v in p[:3]) + (p[3],)

    pixels = [[expected(x, y) for x in range(SIZE)] for y in range(SIZE)]
    prefix = (f'<feColorMatrix values="{IDENTITY}" '
              f'color-interpolation-filters="{prior}" result="image"/>' if prior else '')
    input_name = ('SourceAlpha' if kind == 'alpha' else 'FillPaint' if kind == 'fill'
                  else 'image' if prior else 'SourceGraphic')
    attribute = f' color-interpolation-filters="{requested}"' if requested else ''
    if tiled:
        prefix += (f'<feOffset in="{input_name}" x="4" y="4" width="{dx}" height="{dy}" '
                   f'color-interpolation-filters="{current}" result="tile-input"/>')
        operation = f'<feTile in="tile-input"{attribute}/>'
    else:
        region = ' x="5.25" y="5.125" width="3.5" height="3.75"' if cut else ''
        operation = f'<feOffset in="{input_name}" dx="{dx}" dy="{dy}"{attribute}{region}/>'
    following = (f'<feColorMatrix values="{IDENTITY}" color-interpolation-filters="linearRGB"/>'
                 if tail == 'matrix' else
                 f'<feBlend in2="SourceGraphic" color-interpolation-filters="{tail}"/>' if tail else '')
    x, y = i % 4 * SIZE, i // 4 * SIZE
    probes = (f'<clipPath id="probes{i}"><rect x="10" y="10" width="3" height="3"/></clipPath>'
              if tiled else '')
    clip = f' clip-path="url(#probes{i})"' if tiled else ''
    drawing = (f'<svg x="{x}" y="{y}" width="16" height="16"><defs>{probes}'
               f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="0" y="0" width="16" height="16">'
               f'{prefix}{operation}{following}</filter></defs><g{clip} opacity="{opacity}">'
               f'<g fill="#4080c0" filter="url(#f{i})"><image x="4" y="4" width="6" height="6" '
               f'href="{IMAGE}"/></g></g></svg>')
    unit.cell(name, (x, y, SIZE, SIZE), drawing,
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(pixels)}"/>')
unit.write()
