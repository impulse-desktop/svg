"""A one-stop gradient is solid paint; no gradient coordinate grid is used.

SVG 2 StopNotes says that at least two stops are needed for a gradient
effect. Check that explicit solid-paint rule on stroked zero-area geometry,
where objectBoundingBox cannot define a multi-stop gradient. Expectations
are literal coloured rectangles or transparent space, not other gradients.
"""
from unitgen import Unit


u = Unit('gradient-single-stop-box', 160, 80,
         'Exactly one stop paints its solid colour and opacity, including '
         'horizontal and vertical paths with zero-area object bounding '
         'boxes. No-stop and multi-stop controls retain their separate '
         'rules. Plain rectangles check templates, opacity, transforms '
         'and a user-space clip.', ground=False)
colour = '#26a8e0'
cases = [
    ('linear-horizontal', 'linear', 'h', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('linear-vertical', 'linear', 'v', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('radial-horizontal', 'radial', 'h', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('radial-vertical', 'radial', 'v', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('linear-template', 'linear', 'h', 1, 'objectBoundingBox', 1, 1, 1, True),
    ('radial-template', 'radial', 'v', 1, 'objectBoundingBox', 1, 1, 1, True),
    ('linear-user-space', 'linear', 'h', 1, 'userSpaceOnUse', 1, 1, 1, False),
    ('radial-user-space', 'radial', 'v', 1, 'userSpaceOnUse', 1, 1, 1, False),
    ('stop-opacity', 'linear', 'h', 1, 'objectBoundingBox', .5, 1, 1, False),
    ('stroke-opacity', 'radial', 'v', 1, 'objectBoundingBox', 1, .25, 1, False),
    ('element-opacity', 'linear', 'h', 1, 'objectBoundingBox', .5, .5, .5, False),
    ('no-stops', 'linear', 'h', 0, 'objectBoundingBox', 1, 1, 1, False),
    ('two-stop-horizontal', 'linear', 'h', 2, 'objectBoundingBox', 1, 1, 1, False),
    ('two-stop-vertical', 'radial', 'v', 2, 'objectBoundingBox', 1, 1, 1, False),
    ('two-stop-user-space', 'linear', 'h', 2, 'userSpaceOnUse', 1, 1, 1, False),
    ('user-space-clip', 'linear', 'h', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('scaled-horizontal', 'linear', 'h', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('scaled-vertical', 'radial', 'v', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('one-stop-fallback', 'linear', 'h', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('no-stop-fallback', 'linear', 'h', 0, 'objectBoundingBox', 1, 1, 1, False),
    ('empty-user-fallback', 'linear', 'h', 0, 'userSpaceOnUse', 1, 1, 1, False),
    ('empty-box-singular-fallback', 'linear', 'h', 0, 'objectBoundingBox', 1, 1, 1, False),
    ('empty-user-singular-fallback', 'linear', 'h', 0, 'userSpaceOnUse', 1, 1, 1, False),
    ('one-box-singular-fallback', 'linear', 'h', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('one-user-singular-fallback', 'linear', 'h', 1, 'userSpaceOnUse', 1, 1, 1, False),
    ('one-template-singular', 'linear', 'v', 1, 'objectBoundingBox', 1, 1, 1, True),
    ('two-user-singular-fallback', 'linear', 'h', 2, 'userSpaceOnUse', 1, 1, 1, False),
    ('two-box-fallback', 'linear', 'v', 2, 'objectBoundingBox', 1, 1, 1, False),
    ('radial-empty-singular-fallback', 'radial', 'h', 0, 'objectBoundingBox', 1, 1, 1, False),
    ('radial-one-singular-fallback', 'radial', 'v', 1, 'objectBoundingBox', 1, 1, 1, False),
    ('radial-two-singular-fallback', 'radial', 'h', 2, 'userSpaceOnUse', 1, 1, 1, False),
    ('radial-two-box-fallback', 'radial', 'h', 2, 'objectBoundingBox', 1, 1, 1, False),
]
for i, (name, kind, axis, count, units, stop_alpha, stroke_alpha, opacity, template) in enumerate(cases):
    x, y = i % 8 * 20, i // 8 * 20
    tag = 'linearGradient' if kind == 'linear' else 'radialGradient'
    stops = ''.join(f'<stop offset="{k}" stop-color="{colour}" '
                    f'stop-opacity="{stop_alpha}"/>' for k in range(count))
    inherited = f'<{tag} id="base{i}">{stops}</{tag}>' if template else ''
    href = f' href="#base{i}"' if template else ''
    singular = 'singular' in name
    extra = ' gradientTransform="scale(0)"' if singular else ''
    gradient = (f'<{tag} id="g{i}" gradientUnits="{units}"{href}{extra}>'
                + ('' if template else stops) + f'</{tag}>')
    scaled = name.startswith('scaled-')
    path = ('M3 5H7' if axis == 'h' else 'M5 3V7') if scaled else (
        'M2 10H18' if axis == 'h' else 'M10 2V18')
    transform = ' transform="scale(2)"' if scaled else ''
    fallback = ' red' if name.endswith('fallback') else ''
    clip = f' clip-path="url(#c{i})"' if name == 'user-space-clip' else ''
    test = (f'<g transform="translate({x} {y})"><defs>{inherited}{gradient}'
            f'<clipPath id="c{i}"><rect x="6" y="6" width="8" height="8"/>'
            '</clipPath></defs>'
            f'<g{clip}><path d="{path}" fill="none" '
            f'stroke="url(#g{i}){fallback}" stroke-width="{2 if scaled else 4}" '
            f'stroke-opacity="{stroke_alpha}" opacity="{opacity}"{transform}/>'
            '</g></g>')
    paints = count == 1 or (count > 1 and units == 'userSpaceOnUse' and not singular)
    fallback_paints = count > 1 and bool(fallback) and (singular or units == 'objectBoundingBox')
    expected_colour = 'red' if fallback_paints else colour
    paints |= fallback_paints
    if paints:
        if axis == 'h':left, top, width, height = (6, 8, 8, 4) if scaled or clip else (2, 8, 16, 4)
        else:left, top, width, height = (8, 6, 4, 8) if scaled else (8, 2, 4, 16)
        expected = (f'<rect x="{x+left}" y="{y+top}" width="{width}" '
                    f'height="{height}" fill="{expected_colour}" '
                    f'fill-opacity="{stop_alpha*stroke_alpha*opacity}"/>')
    else:expected = ''
    u.cell(name, (x, y, 20, 20), test, expected)

u.write()
