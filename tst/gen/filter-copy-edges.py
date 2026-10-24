"""Hard alpha boundaries of copied and directionally sampled filter inputs.

Exact Fraction rectangle/pixel intersections supply the rasterized source.
Morphology applies literal per-channel neighbourhood minima/maxima; a zero
or invalid radius is the specified pass-through. Zero displacement and a
fully transparent drop shadow likewise copy the input. Only subregion sides
inside the result's painted hard boundaries apply new fractional coverage
(D2). Positive operators and smaller subregions prevent unconditional copies.
"""
from fractions import Fraction as F
import math
from unitgen import Unit, png

FRAME = tuple(map(F, ['2.25', '2.125', '9.75', '8.875']))
INNER = tuple(map(F, ['3.25', '3.125', '8.75', '7.875']))
IDENTITY = '<feColorMatrix values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0"/>'
cases = []


def add(name, primitive, radii=(0, 0), operator='copy', **kw):
    cases.append((name, primitive, radii, operator, kw))


for name, radius in [('missing', None), ('zero', '0'), ('zero-pair', '0 0'),
                     ('negative', '-1'), ('negative-x', '-1 2'),
                     ('negative-y', '2 -1'), ('malformed', 'bad'),
                     ('empty', ''), ('zero-x', '0 1'), ('zero-y', '1 0')]:
    attribute = '' if radius is None else f' radius="{radius}"'
    add('morph-' + name, '<feMorphology operator="dilate"' + attribute + '/>')
for operator in ['erode', 'dilate']:
    for name, radius, radii in [('rounded-copy', '.1', (0, 0)),
                               ('x-only', '1 .1', (1, 0)),
                               ('y-only', '.1 1', (0, 1)),
                               ('both', '1', (1, 1))]:
        add(operator + '-' + name,
            f'<feMorphology operator="{operator}" radius="{radius}"/>', radii, operator)
add('erode-removes-source', '<feMorphology radius="8"/>', (8, 8), 'erode')
add('invalid-operator-erodes', '<feMorphology operator="bad" radius="1"/>', (1, 1), 'erode')
add('morph-source-alpha', '<feMorphology in="SourceAlpha" radius="0"/>', black=True)
add('morph-translucent', '<feMorphology radius="0"/>', opacity=F(1, 2))
add('morph-same-source-edge', '<feMorphology radius="0"/>', coincident=True)
add('morph-previous-result', '<feFlood flood-color="lime"/><feMorphology radius="0"/>')
add('morph-subregion', '<feMorphology radius="0" x="3.25" y="3.125" width="5.5" height="4.75"/>', subregion=True)
add('erode-subregion-on-edge', '<feMorphology radius="1" x="3.25" y="3.125" width="5.5" height="4.75"/>', (1, 1), 'erode', subregion=True)
add('morph-empty-input', '<feMorphology radius="0"/>', empty=True)
for name, scale in [('missing', None), ('zero', '0'), ('negative-zero', '-0'), ('malformed', 'bad')]:
    attribute = '' if scale is None else f' scale="{scale}"'
    add('displace-' + name, '<feFlood flood-color="red" result="map"/>'
        '<feDisplacementMap in="SourceGraphic" in2="map"' + attribute + '/>')
add('displace-source-alpha', '<feDisplacementMap in="SourceAlpha" scale="0"/>', black=True)
add('displace-subregion', '<feDisplacementMap scale="0" x="3.25" y="3.125" width="5.5" height="4.75"/>', subregion=True)
add('displace-empty-input', '<feDisplacementMap scale="0"/>', empty=True)
add('displace-positive-control', '<feFlood flood-color="red" result="map"/>'
    '<feDisplacementMap in="SourceGraphic" in2="map" scale="1" xChannelSelector="R" yChannelSelector="R"/>', operator='displace')
add('shadow-zero-opacity', '<feDropShadow dx="3" dy="-2" stdDeviation="1" flood-opacity="0"/>')
add('shadow-transparent-colour', '<feDropShadow dx="-2" dy="3" stdDeviation="1" flood-color="transparent"/>')
add('shadow-source-alpha', '<feDropShadow in="SourceAlpha" stdDeviation="1" flood-opacity="0"/>', black=True)
add('shadow-translucent-input', '<feDropShadow stdDeviation="1" flood-opacity="0"/>', opacity=F(1, 2))
add('shadow-subregion', '<feDropShadow stdDeviation="1" flood-opacity="0" x="3.25" y="3.125" width="5.5" height="4.75"/>', subregion=True)
add('shadow-empty-input', '<feDropShadow stdDeviation="1" flood-opacity="0"/>', empty=True)
add('shadow-positive-control', '<feDropShadow dx="0" dy="0" stdDeviation="0" flood-color="lime" flood-opacity=".5"/>', operator='shadow')
add('morph-filter-chain', '<feMorphology radius="0"/>', chain=True)
add('shadow-css-function', IDENTITY, function=True)
add('morph-constant-fill-paint', '<feMorphology in="FillPaint" operator="erode" radius="2"/>')
add('morph-constant-stroke-paint', '<feMorphology in="StrokePaint" operator="erode" radius="2"/>')
add('morph-absent-stroke-paint', '<feMorphology in="StrokePaint" operator="erode" radius="2"/>', absent=True)
add('morph-gradient-paint', '<feMorphology in="FillPaint" operator="erode" radius="2"/>', gradient=True)


def area(x, y, box):
    return (max(F(0), min(F(x + 1), box[2]) - max(F(x), box[0]))
            * max(F(0), min(F(y + 1), box[3]) - max(F(y), box[1])))


def clip(alpha, hard, region, x, y):
    for axis, pixel in enumerate([x, y]):
        lo = region[axis] if region[axis] > hard[axis] else F(pixel)
        hi = region[axis + 2] if region[axis + 2] < hard[axis + 2] else F(pixel + 1)
        alpha *= max(F(0), min(F(pixel + 1), hi) - max(F(pixel), lo))
    return alpha


u = Unit('filter-copy-edges', 56, math.ceil(len(cases) / 4) * 14,
         'Exact rectangular source coverage tests preserved hard alpha edges '
         'through morphology, zero displacement and transparent shadows. '
         'Positive and one-axis kernels, smaller subregions, previous results '
         'and translucent sources distinguish sampling from pass-through.', ground=False)
for i, (name, primitive, radii, operator, options) in enumerate(cases):
    x, y = i % 4 * 14, i // 4 * 14
    opacity = options.get('opacity', F(1))
    source = ('<rect x="2.25" y="2.125" width="7.5" height="6.75" fill="lime"/>'
              if options.get('coincident') else
              f'<rect x="1" y="1" width="10" height="10" fill="lime" fill-opacity="{float(opacity)}"/>')
    if options.get('empty'):source = '<rect x="1" y="1" width="10" height="10" fill="none"/>'
    region = 'filterUnits="userSpaceOnUse" primitiveUnits="userSpaceOnUse" x="2.25" y="2.125" width="7.5" height="6.75" color-interpolation-filters="sRGB"'
    extra = f'<filter id="a{i}" {region}>{IDENTITY}</filter>' if options.get('chain') or options.get('function') else ''
    if options.get('gradient'):
        extra += (f'<linearGradient id="paint{i}" gradientUnits="userSpaceOnUse" x1="0" y1="0" x2="14" y2="0">'
                  '<stop stop-color="black"/><stop offset="1" stop-color="lime"/></linearGradient>')
    fill = f'url(#paint{i})' if options.get('gradient') else 'lime'
    stroke = 'none' if options.get('absent') else 'lime'
    filters = f'url(#a{i}) url(#f{i})' if options.get('chain') else f'url(#a{i}) drop-shadow(2px 3px 1px transparent) url(#f{i})' if options.get('function') else f'url(#f{i})'
    test = (f'<svg x="{x}" y="{y}" width="14" height="14"><defs>'
            f'<filter id="f{i}" {region}>{primitive}</filter>{extra}</defs>'
            f'<g fill="{fill}" stroke="{stroke}" stroke-width="0" filter="{filters}">{source}</g></svg>')

    def original(px, py):
        return F(0) if options.get('empty') or options.get('absent') else area(px, py, FRAME) * opacity

    pixels = []
    for py in range(14):
        row = []
        for px in range(14):
            hard = list(FRAME)
            if operator in ['erode', 'dilate']:
                rx, ry = radii
                samples = [original(px + dx, py + dy)
                           for dx in range(-rx, rx + 1) for dy in range(-ry, ry + 1)]
                alpha = (max if operator == 'dilate' else min)(samples)
                sign = 1 if operator == 'dilate' else -1
                hard = [FRAME[0] - sign * rx, FRAME[1] - sign * ry,
                        FRAME[2] + sign * rx, FRAME[3] + sign * ry]
            elif operator == 'shadow':
                alpha = original(px, py)
                alpha += alpha * F(1, 2) * (1 - alpha)
                hard = [F(-100), F(-100), F(100), F(100)]
            elif operator == 'displace':
                alpha = sum(original(px + dx, py + dy) for dx in [0, 1] for dy in [0, 1]) / 4
                hard = [F(-100), F(-100), F(100), F(100)]
            else:
                alpha = original(px, py)
            alpha = clip(alpha, hard, INNER if options.get('subregion') else FRAME, px, py)
            colour = (0., 0., 0.) if options.get('black') else (0., 1., 0.)
            if options.get('gradient'):
                # An infinite user-space black/lime gradient has alpha one.
                # Its minimum over a radius-two window is its leftmost tap.
                colour = (0., float(max(F(0), min(F(1), (F(px) - 2 + F(1, 2)) / 14))), 0.)
            row.append((*colour, float(alpha)))
        pixels.append(row)
    u.cell(name, (x, y, 14, 14), test,
           f'<image x="{x}" y="{y}" width="14" height="14" href="{png(pixels)}"/>')
u.write()
