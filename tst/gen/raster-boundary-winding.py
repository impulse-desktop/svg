"""Dense interior contours do not change a window's boundary winding.

The control is a single rectangle (or rectangle with a rectangular hole).
All extra quadrilaterals lie in the filled region. Same-direction contours
leave nonzero filling unchanged; paired contours cancel for either rule.
"""
from unitgen import Unit


def polygon(points):
    return 'M' + 'L'.join(f'{x:.12g} {y:.12g}' for x, y in points) + 'Z'


unit = Unit('raster-boundary-winding', 32, 16,
            'Dense slanted contours retain the boundary winding of each pixel. '
            'Positive and negative winding, evenodd cancellation, holes, '
            'fractional boundaries, reflection and opacity have simple independent controls.',
            ground=False)
for index, mode in enumerate(['positive', 'negative', 'evenodd', 'cancel',
                              'hole', 'reflected', 'opacity', 'fractional']):
    x, y = index % 4 * 8, index // 4 * 8
    inset = .125 if mode == 'fractional' else 0
    outer = [(inset, inset), (8-inset, inset), (8-inset, 8-inset), (inset, 8-inset)]
    contours = [polygon(outer if mode != 'negative' else list(reversed(outer)))]
    for i in range(256):
        low = .25 + i / 512
        high = 7.5 - i / 1024
        right = 2.5 if mode == 'hole' else 7.5
        points = [(.25, low), (right, low+.125), (right-.125, high), (.375, high-.125)]
        if mode == 'negative':
            points.reverse()
        contours.append(polygon(points))
        if mode in ('evenodd', 'cancel'):
            # Same shape, different segment endpoints: cancellation must not
            # depend on exact whole-edge deduplication before rasterization.
            paired = points if mode == 'evenodd' else list(reversed(points))
            split = []
            for a, b in zip(paired, paired[1:]+paired[:1]):
                split.extend([a, ((a[0]+b[0])/2, (a[1]+b[1])/2)])
            contours.append(polygon(split))
    plain = polygon(outer)
    if mode == 'hole':
        hole = [(3, 2), (3, 6), (6, 6), (6, 2)]
        contours.append(polygon(hole))
        plain += polygon(hole)
    rule = 'evenodd' if mode == 'evenodd' else 'nonzero'
    opacity = '.375' if mode == 'opacity' else '1'
    transform = 'translate(8 0) scale(-1 1)' if mode == 'reflected' else ''
    attrs = f'fill="#285fa7" fill-rule="{rule}" fill-opacity="{opacity}" transform="{transform}"'
    drawn = f'<path d="{" ".join(contours)}" {attrs}/>'
    expected = f'<path d="{plain}" {attrs}/>'
    unit.cell(mode, (x, y, 8, 8),
              f'<svg x="{x}" y="{y}" width="8" height="8">{drawn}</svg>',
              f'<svg x="{x}" y="{y}" width="8" height="8">{expected}</svg>')
unit.write()
