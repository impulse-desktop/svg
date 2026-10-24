"""Displacement preserves in's space; only in2 follows filter interpolation.

Literal PNG samples are converted and bilinearly interpolated independently.
Upstream primitives and downstream readers distinguish actual colour space
from a primitive's requested displacement-map space.
"""
import math
from unitgen import Unit, png


SIZE = 16
COLOURS = [(255, 0, 0, 255), (0, 0, 255, 128), (64, 192, 128, 255)]
literal = [[tuple(v / 255 for v in COLOURS[(x + y) % 3]) for x in range(6)] for y in range(6)]
uri = png(literal)
CASES = [
    ('source-default', None, None, 1, '', 'source', 'white', 1),
    ('source-srgb-map', None, 'sRGB', 1, '', 'source', 'white', 1),
    ('source-linear-map', None, 'linearRGB', 1, '', 'source', 'white', 1),
    ('prior-srgb', 'sRGB', 'linearRGB', 1, '', 'source', 'white', 1),
    ('prior-linear', 'linearRGB', 'sRGB', 1, '', 'source', 'white', 1),
    ('prior-linear-map-linear', 'linearRGB', 'linearRGB', 1, '', 'source', 'white', 1),
    ('source-alpha', None, 'linearRGB', 1, '', 'alpha', 'white', 1),
    ('negative-scale', None, 'linearRGB', -1, '', 'source', 'white', 1),
    ('zero-scale', None, 'linearRGB', 0, '', 'source', 'white', 1),
    ('following-linear-reader', None, 'linearRGB', 1, 'matrix', 'source', 'white', 1),
    ('following-linear-blend', None, 'linearRGB', 1, 'blend', 'source', 'white', 1),
    ('prior-linear-following-blend', 'linearRGB', 'sRGB', 1, 'blend', 'source', 'white', 1),
    ('source-as-srgb-map', 'linearRGB', 'sRGB', 1, '', 'source', 'source', 1),
    ('source-as-linear-map', 'sRGB', 'linearRGB', 1, '', 'source', 'source', 1),
    ('group-opacity', None, 'linearRGB', 1, '', 'source', 'white', .5),
    ('default-alpha-selectors', None, 'linearRGB', 1, '', 'source', 'default', 1),
]
unit = Unit('filter-displacement-space', 64, 64,
            'Keep the displaced image in its own current colour space. '
            'Only the map follows color-interpolation-filters. Literal '
            'premultiplied PNG taps check source and earlier primitive '
            'inputs, alpha, signed and zero scale, shared source/map reads, '
            'following conversions/blends and group opacity.', ground=False)


def linear(v):
    return v / 12.92 if v <= .04045 else ((v + .055) / 1.055) ** 2.4


def srgb(v):
    return 12.92 * v if v <= .0031308 else 1.055 * max(0, v) ** (1 / 2.4) - .055


def convert(v, source, target):
    if source == target or not v[3]:
        return v[:]
    transfer = linear if target == 'linearRGB' else srgb
    return [transfer(c / v[3]) * v[3] for c in v[:3]] + [v[3]]


def source_pixel(x, y):
    if 4 <= x < 10 and 4 <= y < 10:
        p = literal[y - 4][x - 4]
        return [p[k] * p[3] for k in range(3)] + [p[3]]
    return [0, 0, 0, 0]


identity = '1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0'
for i, (name, prior, map_space, scale, tail, image_kind, map_kind, opacity) in enumerate(CASES):
    current = prior or 'sRGB'
    def input_pixel(x, y):
        p = convert(source_pixel(x, y), 'sRGB', current)
        return [0, 0, 0, p[3]] if image_kind == 'alpha' else p
    def displaced(x, y):
        if map_kind == 'source':
            m = convert(source_pixel(x, y), 'sRGB', map_space or 'linearRGB')
            red, green = (m[0] / m[3], m[2] / m[3]) if m[3] else (0, 0)
        else:
            red = green = 1
        px, py = x + scale * (red - .5), y + scale * (green - .5)
        ix, iy = math.floor(px), math.floor(py)
        tx, ty = px - ix, py - iy
        samples = [(input_pixel(ix, iy), (1 - tx) * (1 - ty)),
                   (input_pixel(ix + 1, iy), tx * (1 - ty)),
                   (input_pixel(ix, iy + 1), (1 - tx) * ty),
                   (input_pixel(ix + 1, iy + 1), tx * ty)]
        return [sum(v[k] * weight for v, weight in samples) for k in range(4)]
    expected = []
    for y in range(SIZE):
        row = []
        for x in range(SIZE):
            p = displaced(x, y)
            if tail:
                p = convert(p, current, 'linearRGB')
                if tail == 'blend':
                    backdrop = convert(source_pixel(x, y), 'sRGB', 'linearRGB')
                    p = [v + b * (1 - p[3]) for v, b in zip(p, backdrop)]
                p = convert(p, 'linearRGB', 'sRGB')
            else:
                p = convert(p, current, 'sRGB')
            p = [v * opacity for v in p]
            row.append(tuple(v / p[3] if p[3] else 0 for v in p[:3]) + (p[3],))
        expected.append(row)
    prefix = (f'<feColorMatrix values="{identity}" color-interpolation-filters="{prior}" result="image"/>' if prior else '')
    input_name = 'SourceAlpha' if image_kind == 'alpha' else 'image' if prior else 'SourceGraphic'
    map_node = '' if map_kind == 'source' else '<feFlood flood-color="white" result="map"/>'
    map_name = 'SourceGraphic' if map_kind == 'source' else 'map'
    selectors = '' if map_kind == 'default' else ' xChannelSelector="R" yChannelSelector="B"'
    last = (f'<feColorMatrix values="{identity}" color-interpolation-filters="linearRGB"/>' if tail == 'matrix' else
            '<feBlend in2="SourceGraphic" color-interpolation-filters="linearRGB"/>' if tail == 'blend' else '')
    nodes = prefix + map_node + f'<feDisplacementMap in="{input_name}" in2="{map_name}" scale="{scale}"{selectors}/>' + last
    space = f' color-interpolation-filters="{map_space}"' if map_space else ''
    x, y = i % 4 * SIZE, i // 4 * SIZE
    drawing = (f'<svg x="{x}" y="{y}" width="16" height="16"><defs><filter id="f{i}" '
               f'filterUnits="userSpaceOnUse" x="0" y="0" width="16" height="16"{space}>{nodes}</filter></defs>'
               f'<image x="4" y="4" width="6" height="6" image-rendering="pixelated" '
               f'href="{uri}" filter="url(#f{i})" opacity="{opacity}"/></svg>')
    unit.cell(name, (x, y, SIZE, SIZE), drawing,
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(expected)}"/>')
unit.write()
