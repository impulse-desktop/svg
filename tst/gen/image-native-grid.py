"""A raster sampled on its own pixel grid retains its literal PNG values.

The declared ratios are exact rational numbers. Their products map the
8x8 PNG to device pixels4..12, including signed axis permutations. The
expectation repeats or permutes source bytes, independently of any SVG
image sampler. Real fractional phases/scales use literal Mitchell weights
and exact rectangular coverage, preventing indiscriminate nearest sampling.
"""
from fractions import Fraction as F
import base64
import math
from unitgen import Unit, png

source = [[((x % 3)/2, (y % 3)/2, ((x + y) % 3)/2,
            [1, .5, .25][(x + 2*y) % 3])
           for x in range(8)] for y in range(8)]
data = png(source)
cases = [
    ('direct', 32, '1', '8', '4', 'plain'),
    ('factor-45', 18, '.45', '10', '5', 'plain'),
    ('factor-70', 28, '.7', '10', '5', 'plain'),
    ('factor-30', 12, '.3', '10', '5', 'plain'),
    ('factor-90', 36, '.9', '10', '5', 'plain'),
    ('factor-60', 24, '.6', '10', '5', 'plain'),
    ('factor-75', 30, '.75', '10', '5', 'plain'),
    ('reflect-x', 18, '.45', '10', '5', 'reflect-x'),
    ('reflect-y', 18, '.45', '10', '5', 'reflect-y'),
    ('reflect-both', 18, '.45', '10', '5', 'reflect-both'),
    ('quarter-turn', 18, '.45', '10', '5', 'quarter-turn'),
    ('three-quarter-turn', 18, '.45', '10', '5', 'three-quarter-turn'),
    ('swap-axes', 18, '.45', '10', '5', 'swap-axes'),
    ('quarter-phase', 32, '1', '8', '4.25', 'fractional'),
    ('small-x-phase', 32, '1', '8', '4.0001', 'fractional'),
    ('small-y-phase', 32, '1', '8', '4', 'fractional-y'),
    ('real-nonunit-scale', 32, '1.0001', '8', '4', 'fractional'),
    ('document-no-viewbox', 18, '.45', '10', '5', 'document'),
    ('document-with-viewbox', 18, '.45', '10', '5', 'document-viewbox'),
    ('single-tile-nonunit', 32, '1', '8', '4', 'single-tile'),
]
unit = Unit('image-native-grid', 128, 32*math.ceil(len(cases)/4),
            'Literal PNG samples survive a native pixel grid through '
            'factored decimal scales, reflections and quarter turns. '
            'Fractional phases and a genuine non-unit scale retain '
            'independent Mitchell reconstruction and fractional coverage.',
            ground=False)


def mitchell(t):
    w0 = (1-9*t+15*t*t-7*t*t*t)/18
    w2 = (1+9*t+27*t*t-21*t*t*t)/18
    w3 = (7*t*t*t-6*t*t)/18
    return [w0, 1-w0-w2-w3, w2, w3]


def quantized(pixel):
    return tuple(math.floor(v*255+.5)/255 for v in pixel)


for i, (name, view, scale, extent, origin, mode) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 32
    s = F(32, view)*F(scale)
    transforms = {
        'plain': '', 'fractional': '', 'fractional-y': '',
        'document': '', 'document-viewbox': '', 'single-tile': '',
        'reflect-x': 'matrix(-1 0 0 1 20 0)',
        'reflect-y': 'matrix(1 0 0 -1 0 20)',
        'reflect-both': 'matrix(-1 0 0 -1 20 20)',
        'quarter-turn': 'matrix(0 1 -1 0 20 0)',
        'three-quarter-turn': 'matrix(0 -1 1 0 0 20)',
        'swap-axes': 'matrix(0 1 1 0 0 0)',
    }
    iy = '4.0001' if mode == 'fractional-y' else origin
    test = (f'<svg x="{x}" y="{y}" width="32" height="32" viewBox="0 0 {view} {view}">'
            f'<g transform="scale({scale})"><g transform="{transforms[mode]}">'
            f'<image x="{origin}" y="{iy}" width="{extent}" height="{extent}" '
            f'preserveAspectRatio="none" href="{data}"/></g></g></svg>')
    if mode.startswith('document'):
        vb = f' viewBox="0 0 {view} {view}"' if mode == 'document-viewbox' else ''
        document = (f'<svg xmlns="http://www.w3.org/2000/svg" width="{view}" height="{view}"{vb}>'
                    f'<g transform="scale({scale})"><image x="{origin}" y="{origin}" '
                    f'width="{extent}" height="{extent}" preserveAspectRatio="none" href="{data}"/></g></svg>')
        href = 'data:image/svg+xml;base64,' + base64.b64encode(document.encode()).decode()
        test = f'<image x="{x}" y="{y}" width="32" height="32" href="{href}"/>'
    expected = [[(0, 0, 0, 0) for _ in range(32)] for _ in range(32)]
    if mode == 'single-tile':
        # The one-pixel tile's sole sample happens to land at a PNG knot,
        # although its image is genuinely reduced by5/7. A native-grid
        # decision must also require a unit linear map.
        test = (f'<svg x="{x}" y="{y}" width="32" height="32"><defs>'
                f'<pattern id="p{i}" patternUnits="userSpaceOnUse" width="1" height="1">'
                f'<image x="0.14285714285714285" y="0.14285714285714285" '
                f'width="5.714285714285714" height="5.714285714285714" '
                f'preserveAspectRatio="none" href="{data}"/></pattern></defs>'
                f'<rect width="32" height="32" fill="url(#p{i})"/></svg>')
        weights = mitchell(F(0))
        value = [0., 0., 0., 0.]
        for j in range(4):
            for k in range(4):
                rgba = quantized(source[max(0, j-1)][max(0, k-1)])
                for channel in range(4):
                    value[channel] += float(weights[k]*weights[j])*(rgba[channel]*rgba[3] if channel < 3 else rgba[3])
        alpha = min(1., max(0., value[3]))
        colour = [min(alpha, max(0., v))/alpha if alpha else 0. for v in value[:3]]
        expected = [[(*colour, alpha*36/49) for _ in range(32)] for _ in range(32)]
    elif not mode.startswith('fractional'):
        assert s*F(extent) == 8 and s*F(origin) == 4
        for py in range(8):
            for px in range(8):
                tx, ty = px, py
                if mode == 'reflect-x': tx = 7-px
                elif mode == 'reflect-y': ty = 7-py
                elif mode == 'reflect-both': tx, ty = 7-px, 7-py
                elif mode == 'quarter-turn': tx, ty = py, 7-px
                elif mode == 'three-quarter-turn': tx, ty = 7-py, px
                elif mode == 'swap-axes': tx, ty = py, px
                expected[py+4][px+4] = source[ty][tx]
    else:
        left, top = s*F(origin), s*F(iy)
        size = s*F(extent)
        for py in range(32):
            for px in range(32):
                area = (max(F(0), min(F(px+1), left+size)-max(F(px), left))
                        * max(F(0), min(F(py+1), top+size)-max(F(py), top)))
                if not area: continue
                qx = (F(px)+F(1, 2)-left)*8/size-F(1, 2)
                qy = (F(py)+F(1, 2)-top)*8/size-F(1, 2)
                ix, iy0 = math.floor(qx), math.floor(qy)
                wx, wy = mitchell(qx-ix), mitchell(qy-iy0)
                value = [0., 0., 0., 0.]
                for j in range(4):
                    for k in range(4):
                        rgba = quantized(source[min(7, max(0, iy0+j-1))][min(7, max(0, ix+k-1))])
                        weight = float(wx[k]*wy[j])
                        for channel in range(4):
                            value[channel] += weight*(rgba[channel]*rgba[3] if channel < 3 else rgba[3])
                alpha = min(1., max(0., value[3]))
                colour = [min(alpha, max(0., v))/alpha if alpha else 0. for v in value[:3]]
                expected[py][px] = (*colour, alpha*float(area))
    unit.cell(name, (x, y, 32, 32), test,
              f'<image x="{x}" y="{y}" width="32" height="32" href="{png(expected)}"/>')
unit.write()

# The size suffix exercises the public render-size path, rather than only
# an embedded viewport. Plain exact rectangles avoid testing a PNG sampler
# against another PNG sampler which could share the same precision bug.
root = Unit('image-native-grid-root.32x32', 18, 18,
            'Rendering a root without viewBox at32x32 keeps the exact '
            '32/18 camera ratio through a .45 transform. Literal PNG '
            'pixels equal plainly drawn rational pixel rectangles.',
            ground=False)
rects = []
for py in range(8):
    for px in range(8):
        r, g, b, a = quantized(source[py][px])
        colour = f'rgb({round(r*255)},{round(g*255)},{round(b*255)})'
        rects.append(f'<rect x="{(px+4)*18/32}" y="{(py+4)*18/32}" '
                     f'width="{18/32}" height="{18/32}" fill="{colour}" fill-opacity="{a}"/>')
root.cell('root-factored-camera', (0, 0, 32, 32),
          f'<g transform="scale(.45)"><image x="5" y="5" width="10" height="10" '
          f'preserveAspectRatio="none" href="{data}"/></g>', ''.join(rects))
root.write()
