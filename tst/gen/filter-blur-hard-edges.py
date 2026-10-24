"""A Gaussian identity, including an identity axis, keeps clipped coverage.

Literal rectangular stroke areas supply the input. Independent exponential
samples convolve that input; a zero/tiny axis cannot erase its existing
fractional edge. The small positive identity cases have less than 1e-7 byte
of off-centre mass, without relying on the renderer's fixed-point kernel.
"""
from fractions import Fraction as F
import math

from unitgen import Unit, png


def area(x, y, box):
    return (max(F(0), min(F(x+1), box[2])-max(F(x), box[0]))
            * max(F(0), min(F(y+1), box[3])-max(F(y), box[1])))


def weights(sigma, identity):
    if not sigma:
        return [(0, 1.)]
    radius = math.ceil(3*sigma)
    taps = [(k, math.exp(-k*k/(2*sigma*sigma)))
            for k in range(-radius, radius+1)]
    total = sum(v for _, v in taps)
    if identity:
        assert sum(v/total for k, v in taps if k)*255 < 1e-7
    return [(k, v/total) for k, v in taps]


u = Unit('filter-blur-hard-edges', 64, 96,
         'An identity Gaussian keeps the input hard edges, including a '
         'single identity axis. Exact rectangular stroke/region areas and '
         'independent exponential convolutions check fractional corners, '
         'tiny positive and underflowing deviations, zero axes, duplicate '
         'edges, SourceAlpha, transforms, primitive chains and opacity. '
         'No comparison with another renderer supplies the expectation.',
         ground=False)
cases = [
    ('zero', 0, 0, True, True, 'none'),
    ('tiny', .0001, .0001, True, True, 'none'),
    ('underflow', 1e-30, 1e-30, True, True, 'none'),
    ('sub-byte-kernel', .125, .125, True, True, 'none'),
    ('active-kernel', .25, .25, False, False, 'none'),
    ('zero-x', 0, .5, True, False, 'none'),
    ('zero-y', .5, 0, False, True, 'none'),
    ('tiny-x', .0001, .5, True, False, 'none'),
    ('tiny-y', .5, .0001, False, True, 'none'),
    ('duplicate-tiny', .0001, .0001, True, True, 'duplicate'),
    ('duplicate-zero-x', 0, .5, True, False, 'duplicate'),
    ('duplicate-zero-y', .5, 0, False, True, 'duplicate'),
    ('duplicate-tiny-x', .0001, .5, True, False, 'duplicate'),
    ('duplicate-tiny-y', .5, .0001, False, True, 'duplicate'),
    ('source-alpha', .0001, .0001, True, True, 'none'),
    ('group-opacity', .0001, .0001, True, True, 'none'),
    ('scaled', .0001, .0001, True, True, 'none'),
    ('reflected', .0001, .0001, True, True, 'none'),
    ('quarter-turn', .0001, .0001, True, True, 'none'),
    ('object-box', .0001, .0001, True, True, 'none'),
    ('group-filter', .0001, .0001, True, True, 'none'),
    ('two-primitives', .0001, .0001, True, True, 'none'),
    ('prior-flood', .0001, .0001, True, True, 'none'),
    ('linear-rgb', .0001, .0001, True, True, 'none'),
]
for i, (name, sx, sy, keep_x, keep_y, edge) in enumerate(cases):
    geometry = list(map(F, ['2.25', '2.25', '9.75', '9.75']))
    outer = list(map(F, ['1.25', '1.25', '10.75', '10.75']))
    inner = list(map(F, ['3.25', '3.25', '8.75', '8.75']))
    transform = ''
    if name == 'scaled':
        transform = ' transform="translate(.125 .25) scale(1.25 .75)"'
        for box in [geometry, outer, inner]:
            box[:] = [box[0]*F('1.25')+F('.125'), box[1]*F('.75')+F('.25'),
                      box[2]*F('1.25')+F('.125'), box[3]*F('.75')+F('.25')]
        sx, sy = sx*1.25, sy*.75
    elif name in ['reflected', 'quarter-turn']:
        transform = (' transform="translate(14) scale(-1 1)"' if name == 'reflected'
                     else ' transform="translate(14 0) rotate(90)"')
        for box in [geometry, outer, inner]:
            box[:] = [14-box[2], box[1], 14-box[0], box[3]]
    source = [[float(area(x, y, geometry)) if name == 'prior-flood'
               else float((area(x, y, outer)-area(x, y, inner))*area(x, y, geometry))
               for x in range(16)] for y in range(16)]
    def part(axis, pixel):
        return float(max(F(0), min(F(pixel+1), geometry[axis+2])
                         - max(F(pixel), geometry[axis])))
    def sample(x, y):
        if edge == 'duplicate':
            x = min(math.ceil(geometry[2])-1, max(math.floor(geometry[0]), x))
            y = min(math.ceil(geometry[3])-1, max(math.floor(geometry[1]), y))
        if not (0 <= x < 16 and 0 <= y < 16):
            return 0.
        value = source[y][x]
        if edge == 'duplicate':
            value /= (1 if keep_x else part(0, x))*(1 if keep_y else part(1, y))
        return value
    wx, wy = weights(sx, keep_x), weights(sy, keep_y)
    pixels = []
    for y in range(16):
        row = []
        for x in range(16):
            alpha = sum(ax*ay*sample(x+dx, y+dy)
                        for dx, ax in wx for dy, ay in wy)
            alpha *= (1 if keep_x else part(0, x))*(1 if keep_y else part(1, y))
            if not area(x, y, geometry):
                alpha = 0
            if name == 'group-opacity':
                alpha *= .4
            row.append((0., 0. if name == 'source-alpha' else 1., 0., alpha))
        pixels.append(row)
    region = ('x="0" y="0" width="1" height="1"' if name == 'object-box'
              else 'filterUnits="userSpaceOnUse" x="2.25" y="2.25" width="7.5" height="7.5"')
    # Transforms above scale the filter and source together; deviations in
    # the markup remain local values, while the independent model uses device values.
    local_sx, local_sy = cases[i][1:3]
    blur = (f'<feGaussianBlur stdDeviation="{local_sx} {local_sy}" edgeMode="{edge}"'
            + (' in="SourceAlpha"' if name == 'source-alpha' else '') + '/>')
    if name == 'two-primitives':
        blur += blur
    if name == 'prior-flood':
        blur = '<feFlood flood-color="lime"/>' + blur
    rect = '<rect x="2.25" y="2.25" width="7.5" height="7.5" fill="none" stroke="lime" stroke-width="2"'
    body = (f'<g filter="url(#f{i})">{rect}/></g>' if name == 'group-filter'
            else rect + f' filter="url(#f{i})"/>')
    if transform:
        body = '<g' + transform + '>' + body + '</g>'
    if name == 'group-opacity':
        body = '<g opacity=".4">' + body + '</g>'
    x, y = i % 4 * 16, i // 4 * 16
    space = 'linearRGB' if name == 'linear-rgb' else 'sRGB'
    u.cell(name, (x, y, 16, 16),
           f'<svg x="{x}" y="{y}" width="16" height="16"><defs>'
           f'<filter id="f{i}" {region} color-interpolation-filters="{space}">{blur}</filter>'
           f'</defs>{body}</svg>',
           f'<image x="{x}" y="{y}" width="16" height="16" href="{png(pixels)}"/>')
u.write()
