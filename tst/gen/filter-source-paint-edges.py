"""Filter source edges come from painted input, including stroke extent.

Exact rational rectangle/ring areas give SourceGraphic alpha before any
filter. An identity primitive clips a side strictly inside that painted
support; a side on an existing hard edge keeps its stored coverage (D2).
Sequential filters carry those hard edges along with their output. No
renderer pixels or geometric bounding-box shortcuts supply expectations.
"""
from fractions import Fraction as F
import math

from unitgen import Unit, png


u = Unit('filter-source-paint-edges', 48, 156,
         'A filter region may equal the geometry box while cutting through '
         'its stroke. Exact source fill/stroke areas and fractional clipping '
         'test identity primitives, groups, use, SourceAlpha, filter chains, '
         'empty source and transformed layers.', ground=False)
identity = '<feColorMatrix values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0"/>'
primitives = {
    'matrix': identity,
    'alpha': '<feColorMatrix in="SourceAlpha" values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1 0"/>',
    'offset': '<feOffset dx="0" dy="0"/>',
    'composite': '<feComposite in="SourceGraphic" in2="SourceGraphic" operator="atop"/>',
    'transfer': '<feComponentTransfer/>',
    'convolve': '<feConvolveMatrix order="1" kernelMatrix="1" preserveAlpha="true"/>',
    'flood': '<feFlood flood-color="lime"/>',
    'offset-right': '<feOffset dx=".5"/>',
    'offset-left': '<feOffset dx="-.5"/>',
    'offset-down': '<feOffset dy=".5"/>',
    'offset-up': '<feOffset dy="-.5"/>',
    'alpha-offset': '<feOffset in="SourceAlpha" dx=".5" dy=".5"/>',
    'blur': '<feGaussianBlur stdDeviation=".5"/>',
    'alpha-blur': '<feGaussianBlur in="SourceAlpha" stdDeviation=".5"/>',
    'offset-cross': '<feOffset dx=".875" dy=".875"/>',
    'zero-blur': '<feGaussianBlur stdDeviation="0"/>',
    'invalid-blur': '<feGaussianBlur stdDeviation="bad"/>',
    'negative-blur': '<feGaussianBlur stdDeviation="-1"/>',
    'flood-zero': '<feFlood flood-color="lime"/><feGaussianBlur stdDeviation="0"/>',
    'flood-invalid': '<feFlood flood-color="lime"/><feGaussianBlur stdDeviation="bad"/>',
    'flood-negative': '<feFlood flood-color="lime"/><feGaussianBlur stdDeviation="-1"/>',
    'fill-paint-zero': '<feGaussianBlur in="FillPaint" stdDeviation="0"/>',
    'fill-paint-blur': '<feGaussianBlur in="FillPaint" stdDeviation="1"/>',
}
cases = [
    ('plain-fill', 'fill', 'matrix'),
    ('filled-stroke', 'stroke', 'matrix'),
    ('stroke-only', 'ring', 'matrix'),
    ('group-stroke', 'group', 'matrix'),
    ('use-stroke', 'use', 'matrix'),
    ('source-alpha', 'stroke', 'alpha'),
    ('offset-input', 'stroke', 'offset'),
    ('composite-input', 'stroke', 'composite'),
    ('transfer-input', 'stroke', 'transfer'),
    ('convolve-input', 'stroke', 'convolve'),
    ('chain-same-region', 'chain', 'matrix'),
    ('chain-wide-first', 'wide-chain', 'matrix'),
    ('function-chain', 'function-chain', 'matrix'),
    ('empty-source', 'empty', 'matrix'),
    ('flood-empty-source', 'empty', 'flood'),
    ('translucent-stroke', 'translucent', 'matrix'),
    ('region-at-paint-edge', 'outer-frame', 'matrix'),
    ('region-inside-paint', 'inner-frame', 'matrix'),
    ('translated-source', 'translated', 'matrix'),
    ('scaled-source', 'scaled', 'matrix'),
    ('quarter-turn-layer', 'quarter', 'matrix'),
    ('reflected-layer', 'reflect', 'matrix'),
    ('zero-geometry-height', 'line', 'matrix'),
    ('nested-wide-filter', 'nested-fill', 'matrix'),
    ('nested-wide-stroke', 'nested-stroke', 'matrix'),
    ('nested-clipped-source', 'nested-clip', 'matrix'),
    ('nested-clip-program', 'nested-clip-program', 'matrix'),
    ('nested-masked-source', 'nested-mask', 'matrix'),
    ('nested-mask-program', 'nested-mask-program', 'matrix'),
    ('nested-mask-intersect', 'nested-mask-intersect', 'matrix'),
    ('nested-mask-add', 'nested-mask-add', 'matrix'),
    ('nested-mask-exclude', 'nested-mask-exclude', 'matrix'),
    ('nested-mask-subtract', 'nested-mask-subtract', 'matrix'),
    ('zero-opacity-sibling', 'transparent-sibling', 'matrix'),
    ('nested-offset-source', 'nested-offset', 'matrix'),
    ('thin-geometry-width', 'thin', 'matrix'),
    ('source-offset-right', 'stroke', 'offset-right'),
    ('source-offset-left', 'stroke', 'offset-left'),
    ('source-offset-down', 'stroke', 'offset-down'),
    ('source-offset-up', 'stroke', 'offset-up'),
    ('source-alpha-offset', 'stroke', 'alpha-offset'),
    ('source-blur-input', 'stroke', 'blur'),
    ('source-alpha-blur-input', 'stroke', 'alpha-blur'),
    ('source-offset-cross-pixel', 'stroke', 'offset-cross'),
    ('zero-blur-keeps-edge', 'stroke', 'zero-blur'),
    ('invalid-blur-keeps-edge', 'stroke', 'invalid-blur'),
    ('negative-blur-keeps-edge', 'stroke', 'negative-blur'),
    ('prior-result-zero-blur', 'stroke', 'flood-zero'),
    ('prior-result-invalid-blur', 'stroke', 'flood-invalid'),
    ('prior-result-negative-blur', 'stroke', 'flood-negative'),
    ('fill-paint-zero-blur', 'stroke', 'fill-paint-zero'),
    ('constant-fill-paint-blur', 'stroke', 'fill-paint-blur'),
]


def area(x, y, box):
    x0, y0, x1, y1 = box
    return (max(F(0), min(F(x + 1), x1) - max(F(x), x0))
            * max(F(0), min(F(y + 1), y1) - max(F(y), y0)))


def clipped(alpha, support, region, x, y):
    factor = F(1)
    for axis, pixel in enumerate([x, y]):
        lo = region[axis] if region[axis] > support[axis] else F(pixel)
        hi = region[axis + 2] if region[axis + 2] < support[axis + 2] else F(pixel + 1)
        factor *= max(F(0), min(F(pixel + 1), hi) - max(F(pixel), lo))
    return alpha * factor


for i, (name, kind, primitive) in enumerate(cases):
    x, y = i % 4 * 12, i // 4 * 12
    geometry = list(map(F, ['2.25', '2.125', '7.75', '6.875']))
    outer = list(map(F, ['1.25', '1.125', '8.75', '7.875']))
    inner = list(map(F, ['3.25', '3.125', '6.75', '5.875']))
    transform = ''
    sx = sy = F(1)
    dx = dy = F(0)
    if kind == 'translated':
        dx, dy = F('.125'), F('.25')
        transform = ' transform="translate(.125 .25)"'
    elif kind == 'scaled':
        sx, sy = F('1.25'), F('.75')
        transform = ' transform="scale(1.25 .75)"'
    elif kind == 'quarter':
        transform = ' transform="translate(12 0) rotate(90)"'
    elif kind == 'reflect':
        transform = ' transform="translate(12 0) scale(-1 1)"'
    if kind in ['translated', 'scaled']:
        for box in [geometry, outer, inner]:
            box[:] = [box[0] * sx + dx, box[1] * sy + dy,
                      box[2] * sx + dx, box[3] * sy + dy]
    support = geometry if kind in ['fill', 'nested-fill', 'transparent-sibling'] else outer
    frame = geometry
    region = 'x="0" y="0" width="1" height="1"'
    if kind in ['outer-frame', 'inner-frame', 'line', 'transparent-sibling', 'nested-offset']:
        frame = outer if kind == 'outer-frame' else inner if kind == 'inner-frame' else geometry
        region = (f'filterUnits="userSpaceOnUse" x="{float(frame[0])}" y="{float(frame[1])}" '
                  f'width="{float(frame[2] - frame[0])}" height="{float(frame[3] - frame[1])}"')
    if kind == 'line':
        support = list(map(F, ['1', '2', '11', '10']))
    paint = ('fill="none" stroke="none"' if kind == 'empty'
             else 'fill="lime" stroke="none"' if kind in ['fill', 'nested-fill', 'transparent-sibling', 'nested-offset']
             else 'fill="none" stroke="lime" stroke-width="2"' if kind == 'ring'
             else 'fill="lime" stroke="lime" stroke-width="2"')
    if kind == 'translucent':
        paint += ' fill-opacity=".5" stroke-opacity=".5"'
    filtered = f' filter="url(#f{i})"'
    extra = ''
    if kind in ['chain', 'wide-chain']:
        wide = 'x="-.5" y="-.5" width="2" height="2"' if kind == 'wide-chain' else region
        extra = f'<filter id="a{i}" {wide}>{identity}</filter>'
        filtered = f' filter="url(#a{i}) url(#f{i})"'
    elif kind == 'function-chain':
        filtered = f' filter="brightness(1) url(#f{i})"'
    rect = f'<rect x="2.25" y="2.125" width="5.5" height="4.75" {paint}'
    if kind == 'group':
        body = f'<g{filtered}>{rect}/></g>'
    elif kind == 'use':
        extra += f'{rect} id="s{i}"/>'
        body = f'<use href="#s{i}"{filtered}/>'
    elif kind == 'line':
        body = f'<path d="M1 6H11" fill="none" stroke="lime" stroke-width="8"{filtered}/>'
    elif kind == 'thin':
        body = f'<path d="M6 2l.00001 7z" fill="none" stroke="lime" stroke-width="2"{filtered}/>'
    elif kind in ['nested-fill', 'nested-stroke', 'nested-offset']:
        operation = '<feOffset dx=".5"/>' if kind == 'nested-offset' else identity
        extra += f'<filter id="a{i}" x="-.5" y="-.5" width="2" height="2">{operation}</filter>'
        body = f'<g{filtered}>{rect} filter="url(#a{i})"/></g>'
        if kind == 'nested-offset':
            support = [geometry[0] + F('.5'), geometry[1], geometry[2] + F('.5'), geometry[3]]
    elif kind.startswith('nested-clip'):
        clip_rect = '<rect x="2.25" y="2.125" width="5.5" height="4.75"/>'
        if kind == 'nested-clip-program':
            extra += f'<clipPath id="c{i}b">{clip_rect}</clipPath>'
            clip_rect = f'<rect x="2.25" y="2.125" width="5.5" height="4.75" clip-path="url(#c{i}b)"/>'
        extra += f'<clipPath id="c{i}">{clip_rect}</clipPath>'
        body = f'<g{filtered}>{rect} clip-path="url(#c{i})"/></g>'
        support = geometry
    elif kind.startswith('nested-mask'):
        mask_rect = '<rect x="2.25" y="2.125" width="5.5" height="4.75" fill="white"/>'
        if kind == 'nested-mask-program':
            extra += f'<filter id="a{i}" x="-.5" y="-.5" width="2" height="2">{identity}</filter>'
            mask_rect = f'<g filter="url(#a{i})">{mask_rect}</g>'
        extra += (f'<mask id="m{i}" maskUnits="userSpaceOnUse" x="0" y="0" width="12" height="12" '
                  f'mask-type="alpha">{mask_rect}</mask>')
        mask = f' mask="url(#m{i})"'
        if kind in ['nested-mask-intersect', 'nested-mask-add', 'nested-mask-exclude', 'nested-mask-subtract']:
            operation = kind.rsplit('-', 1)[1]
            mask = f' style="mask-image: url(#m{i}), url(#m{i}); mask-composite: {operation}"'
        body = f'<g{filtered}>{rect}{mask}/></g>'
        support = geometry
    elif kind == 'transparent-sibling':
        body = f'<g{filtered}>{rect}/><rect width="12" height="12" opacity="0" fill="lime"/></g>'
    else:
        body = rect + transform + filtered + '/>'
    source = (f'<svg x="{x}" y="{y}" width="12" height="12">'
              f'<defs><filter id="f{i}" {region}>{primitives[primitive]}</filter>{extra}</defs>'
              f'{body}</svg>')
    pixels = []

    def stroke_source(px, py):
        fill = area(px, py, geometry)
        stroke = area(px, py, outer) - area(px, py, inner)
        return clipped(fill + stroke * (1 - fill), outer, geometry, px, py)

    for py in range(12):
        row = []
        for px in range(12):
            ix, iy = (py, 11 - px) if kind == 'quarter' else (11 - px, py) if kind == 'reflect' else (px, py)
            fill = area(ix, iy, geometry)
            stroke = area(ix, iy, outer) - area(ix, iy, inner)
            if kind in ['fill', 'nested-fill', 'transparent-sibling']:
                alpha = fill
            elif kind == 'nested-offset':
                # feOffset translates the already rasterized source. A half
                # pixel is the average of its two neighbouring samples.
                alpha = (area(ix, iy, geometry) + area(ix - 1, iy, geometry)) / 2
            elif kind == 'ring':
                alpha = stroke
            elif kind == 'empty':
                alpha = F(0)
            elif kind == 'line':
                alpha = area(ix, iy, support)
            else:
                opacity = F(1, 2) if kind == 'translucent' else F(1)
                alpha = fill * opacity + stroke * opacity * (1 - fill * opacity)
            if kind.startswith('nested-clip') or kind.startswith('nested-mask'):
                cover = fill
                # The clip program's rectangle is clipped by the identical
                # rectangle: a geometric identity, not another alpha mask.
                # Intersecting real masks still multiplies their alpha.
                if kind == 'nested-mask-intersect':
                    cover *= fill
                elif kind == 'nested-mask-add':
                    cover = 2 * fill - fill * fill
                elif kind == 'nested-mask-exclude':
                    cover = 2 * fill * (1 - fill)
                elif kind == 'nested-mask-subtract':
                    cover = fill * (1 - fill)
                alpha *= cover
            if primitive.startswith('flood') or primitive.startswith('fill-paint'):
                alpha = area(ix, iy, frame)
            elif primitive.startswith('offset-') or primitive == 'alpha-offset':
                ox = F('.875') if primitive == 'offset-cross' else F('.5') if primitive in ['offset-right', 'alpha-offset'] else F('-.5') if primitive == 'offset-left' else F(0)
                oy = F('.875') if primitive == 'offset-cross' else F('.5') if primitive in ['offset-down', 'alpha-offset'] else F('-.5') if primitive == 'offset-up' else F(0)
                kx, ky = math.floor(ox), math.floor(oy)
                fx, fy = ox - kx, oy - ky
                alpha = sum(stroke_source(ix - kx - dx, iy - ky - dy) * wx * wy
                            for dx, wx in [(0, 1 - fx), (1, fx)]
                            for dy, wy in [(0, 1 - fy), (1, fy)])
                hard = [geometry[0] + ox, geometry[1] + oy,
                        geometry[2] + ox, geometry[3] + oy]
                alpha = clipped(alpha, hard, frame, ix, iy)
            elif primitive in ['blur', 'alpha-blur']:
                kernel = [(k, math.exp(-k * k / .5)) for k in range(-2, 3)]
                total = sum(weight for _, weight in kernel)
                alpha = sum(float(stroke_source(ix + dx, iy + dy)) * wx * wy / total ** 2
                            for dx, wx in kernel for dy, wy in kernel)
                alpha *= float(area(ix, iy, frame))
            else:
                alpha = clipped(alpha, support, frame, ix, iy)
            if kind == 'thin':
                # Its geometric filter width is 1e-5: even a fully opaque
                # source contributes less than .003 byte per boundary pixel.
                alpha = F(0)
            rgb = (0., 0., 0.) if primitive in ['alpha', 'alpha-offset', 'alpha-blur'] else (0., 1., 0.)
            row.append((*rgb, float(alpha)))
        pixels.append(row)
    u.cell(name, (x, y, 12, 12), source,
           f'<image x="{x}" y="{y}" width="12" height="12" href="{png(pixels)}"/>')
u.write()
