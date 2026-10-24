"""Image and flood introduce their colours independently of filter space.

Use literal native-size PNG samples and exact rectangular flood coverage.
Following spatial interpolation makes an unnecessary initial conversion
observable, rather than merely testing a colour-space round trip.
"""
import math

from unitgen import Unit, png


SIZE = 16
COLOURS = [(255, 0, 0, 255), (0, 0, 255, 128), (64, 192, 128, 255)]
PIXELS = [[tuple(v / 255 for v in COLOURS[(x + y) % 3])
           for x in range(6)] for y in range(6)]
IMAGE = png(PIXELS)
CASES = [
    ('image-default-offset', 'image', None, 'offset', None),
    ('image-srgb-offset', 'image', 'sRGB', 'offset', None),
    ('image-linear-offset', 'image', 'linearRGB', 'offset', None),
    ('image-default-offset-srgb', 'image', None, 'offset', 'sRGB'),
    ('image-srgb-offset-linear', 'image', 'sRGB', 'offset', 'linearRGB'),
    ('image-linear-offset-srgb', 'image', 'linearRGB', 'offset', 'sRGB'),
    ('image-local-offset', 'local', 'linearRGB', 'offset', 'sRGB'),
    ('image-default-displace', 'image', None, 'displace', None),
    ('image-srgb-displace', 'image', 'sRGB', 'displace', None),
    ('image-linear-displace', 'image', 'linearRGB', 'displace', 'sRGB'),
    ('image-direct-default', 'image', None, '', None),
    ('image-direct-linear', 'image', 'linearRGB', '', None),
    ('flood-default', 'flood', None, '', None),
    ('flood-srgb', 'flood', 'sRGB', '', None),
    ('flood-linear', 'flood', 'linearRGB', '', None),
    ('flood-offset-linear', 'flood', 'linearRGB', 'offset', 'linearRGB'),
]


def area(x, y):
    return (max(0, min(x + 1, 10.75) - max(x, 4.25))
            * max(0, min(y + 1, 9.875) - max(y, 3.125)))


unit = Unit('filter-source-space', 64, 64,
            'feImage and feFlood ignore color-interpolation-filters. '
            'Literal raster/local image colours stay in sRGB through '
            'following offset/displacement sampling, independent of both '
            'primitives requested spaces. Direct images and exact fractional '
            'translucent floods are pass-through controls.', ground=False)
for i, (name, kind, requested, operation, next_requested) in enumerate(CASES):
    def source_pixel(x, y):
        if kind == 'flood':
            a = area(x, y) * .5
            return [64 / 255 * a, 128 / 255 * a, 192 / 255 * a, a]
        if 4 <= x < 10 and 4 <= y < 10:
            p = PIXELS[y - 4][x - 4]
            return [v * p[3] for v in p[:3]] + [p[3]]
        return [0, 0, 0, 0]

    pixels = []
    for y in range(SIZE):
        row = []
        for x in range(SIZE):
            dx, dy = ((.5, .25) if operation == 'offset' else
                      (-.5, -.5) if operation == 'displace' else (0, 0))
            px, py = x - dx, y - dy
            ix, iy = math.floor(px), math.floor(py)
            tx, ty = px - ix, py - iy
            taps = [(source_pixel(ix, iy), (1 - tx) * (1 - ty)),
                    (source_pixel(ix + 1, iy), tx * (1 - ty)),
                    (source_pixel(ix, iy + 1), (1 - tx) * ty),
                    (source_pixel(ix + 1, iy + 1), tx * ty)]
            p = [sum(v[k] * w for v, w in taps) for k in range(4)]
            row.append(tuple(v / p[3] if p[3] else 0 for v in p[:3]) + (p[3],))
        pixels.append(row)
    requested_attr = f' color-interpolation-filters="{requested}"' if requested else ''
    next_attr = f' color-interpolation-filters="{next_requested}"' if next_requested else ''
    local = ''
    if kind == 'flood':
        first = (f'<feFlood x="4.25" y="3.125" width="6.5" height="6.75" '
                 f'flood-color="#4080c0" flood-opacity=".5" result="image"{requested_attr}/>')
    elif kind == 'local':
        local = f'<g id="local{i}"><image x="4" y="4" width="6" height="6" href="{IMAGE}"/></g>'
        first = (f'<feImage href="#local{i}" x="0" y="0" width="16" height="16" '
                 f'result="image"{requested_attr}/>')
    else:
        first = (f'<feImage href="{IMAGE}" x="4" y="4" width="6" height="6" '
                 f'result="image"{requested_attr}/>')
    if operation == 'offset':
        last = (f'<feOffset in="image" dx=".5" dy=".25" x="0" y="0" '
                f'width="16" height="16"{next_attr}/>')
    elif operation == 'displace':
        last = ('<feFlood flood-color="white" result="map"/>'
                f'<feDisplacementMap in="image" in2="map" scale="1" '
                f'x="0" y="0" width="16" height="16"{next_attr}/>')
    else:
        last = ''
    x, y = i % 4 * SIZE, i // 4 * SIZE
    drawing = (f'<svg x="{x}" y="{y}" width="16" height="16"><defs>{local}'
               f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="0" y="0" width="16" height="16">'
               f'{first}{last}</filter></defs><rect width="16" height="16" filter="url(#f{i})"/></svg>')
    unit.cell(name, (x, y, SIZE, SIZE), drawing,
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(pixels)}"/>')
unit.write()
