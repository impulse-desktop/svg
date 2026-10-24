"""A small Gaussian uses normalized samples of the specified G function.

The input is a literal PNG pixel plane, so input area rasterization and
image scaling do not enter the expectation. Every expected component is
an independent floating-point convolution in premultiplied sRGB, before
optional group opacity. The output PNG alone rounds to eight-bit samples.
"""
import math

from unitgen import Unit, png


def kernel(sigma):
    if not sigma:
        return [(0, 1.)]
    radius = math.ceil(3 * sigma)
    weights = [(i, math.exp(-i*i/(2*sigma*sigma))) for i in range(-radius, radius+1)]
    total = sum(value for _, value in weights)
    return [(i, value/total) for i, value in weights]


unit = Unit('filter-small-gaussian', 80, 80,
            'Below two device pixels the Gaussian kernel samples the '
            'specification exponential and normalizes its finite support. '
            'Independent premultiplied convolution of literal PNG inputs '
            'checks small and unequal deviations, zero axes, the limit '
            'below two, source alpha, post-filter group opacity and edge '
            'extension. Nonuniform transforms scale each deviation in its '
            'own axis and preserve the zero-blur identity. No curve '
            'approximation or special tolerance.', ground=False)
cases = [
    ('narrow', .125, .125, 'impulse', False, 1),
    ('half', .5, .5, 'impulse', False, 1),
    ('map-scale', .525638, .524788, 'impulse', False, 1),
    ('unequal', .75, 1.25, 'impulse', False, 1),
    ('x-only', 1.25, 0, 'impulse', False, 1),
    ('y-only', 0, 1.25, 'impulse', False, 1),
    ('below-two', 1.999, 1.999, 'impulse', False, 1),
    ('tiny', .001, .001, 'impulse', False, 1),
    ('premultiplied-pair', .7, 1.4, 'pair', False, 1),
    ('group-half', .5, .75, 'pair', False, .5),
    ('constant-duplicate', .8, 1.6, 'constant', True, 1),
    ('constant-none', .5, .5, 'constant', False, 1),
    ('scaled-x', .5, 1, 'pair', False, 1),
    ('scaled-y', 1, .5, 'pair', False, 1),
    ('scaled-zero', 0, 0, 'pair', False, 1),
]
for i, (name, sx, sy, kind, duplicate, opacity) in enumerate(cases):
    pixels = [[(0., 0., 0., 0.) for x in range(16)] for y in range(16)]
    if kind == 'constant':
        pixels = [[(37/255, 121/255, 209/255, 128/255) for x in range(16)] for y in range(16)]
    elif kind == 'pair':
        pixels[7][7] = (1., 0., 0., 64/255)
        pixels[8][8] = (0., 0., 1., 192/255)
    else:
        pixels[7][7] = (0., 1., 0., 1.)
    expected = []
    wx, wy = kernel(sx), kernel(sy)
    for y in range(16):
        row = []
        for x in range(16):
            value = [0., 0., 0., 0.]
            for dy, ay in wy:
                for dx, ax in wx:
                    xx, yy = x+dx, y+dy
                    if duplicate:
                        xx, yy = min(15, max(0, xx)), min(15, max(0, yy))
                    elif not (0 <= xx < 16 and 0 <= yy < 16):
                        continue
                    p = pixels[yy][xx]
                    for c in range(3):
                        value[c] += p[c]*p[3]*ax*ay
                    value[3] += p[3]*ax*ay
            alpha = value[3]
            row.append(tuple(c/alpha if alpha else 0. for c in value[:3]) + (alpha*opacity,))
        expected.append(row)
    x, y = i % 4 * 20 + 2, i // 4 * 20 + 2
    mode = 'duplicate' if duplicate else 'none'
    width, height, transform = 16, 16, ''
    local_sx, local_sy = sx, sy
    if name in ['scaled-x', 'scaled-zero']:
        width, transform = 32, 'scale(.5 1)'
    elif name == 'scaled-y':
        height, transform = 32, 'scale(1 .5)'
    if name in ['scaled-x', 'scaled-y']:
        local_sx = local_sy = 1
    transform_attr = f' transform="{transform}"' if transform else ''
    # Doubling the image's declared side cancels the parent's half scale,
    # so the literal PNG still maps exactly onto its native pixel grid.
    # The filter's one-unit deviations, however, become (.5, 1) or (1, .5).
    unit.cell(name, (x, y, 16, 16),
              f'<svg x="{x}" y="{y}" width="16" height="16">'
              f'<defs><filter id="f{i}" filterUnits="userSpaceOnUse" '
              f'x="0" y="0" width="{width}" height="{height}" color-interpolation-filters="sRGB">'
              f'<feGaussianBlur stdDeviation="{local_sx} {local_sy}" edgeMode="{mode}"/>'
              '</filter></defs>'
              f'<g opacity="{opacity}"{transform_attr}><image width="{width}" height="{height}" preserveAspectRatio="none" '
              f'filter="url(#f{i})" href="{png(pixels)}"/></g></svg>',
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(expected)}"/>')

unit.write()
