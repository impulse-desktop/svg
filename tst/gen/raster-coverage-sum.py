"""Disjoint slivers retain their summed rational area before rounding.

All slivers lie within a single pixel and share integer y endpoints. The
row has only one integration interval, even with thousands of edges. Each
width is 1/4000, so their union's area is n/4000. Reversed and coincident
contours do not change it; paired opposite contours cancel. PNG controls
check alpha and the independently computed RGB result over white.
"""
from fractions import Fraction as F

from unitgen import Unit, png


cases = [
    ('one', 1, 'normal'), ('few', 128, 'normal'), ('many', 2048, 'normal'),
    ('reverse', 2048, 'reverse'), ('evenodd', 2048, 'evenodd'),
    ('subdivided', 2048, 'split'), ('reflected', 2048, 'reflect'),
    ('coincident', 2048, 'double'), ('opposed', 2048, 'cancel'),
]
unit = Unit('raster-coverage-sum', 4, len(cases) * 2,
            'Disjoint thin slivers retain the sum of their exact areas before conversion to stored coverage. One integration interval isolates accumulation precision from row sampling. Independent rational PNG values cover alpha and RGB over white, direction, fill rule, exact subdivision, reflection and coincident contours.', ground=False)

for i, (name, count, mode) in enumerate(cases):
    paths = []
    for j in range(count):
        left, right = F(j, 2048), F(j, 2048) + F(1, 4000)
        points = [(left, 0), (right, 0)]
        if mode == 'split':
            points.append((right, F(1, 2)))
        points.extend([(right, 1), (left, 1)])
        if mode == 'split':
            points.append((left, F(1, 2)))
        if mode == 'reverse':
            points.reverse()
        loops = [points]
        if mode == 'double':
            loops.append(points)
        elif mode == 'cancel':
            loops.append(list(reversed(points)))
        for loop in loops:
            paths.append('M' + 'L'.join(f'{float(x):.12g} {float(y):.12g}' for x, y in loop) + 'Z')
    shape = f'<path d="{" ".join(paths)}" fill="#285fa7" fill-rule="{"evenodd" if mode == "evenodd" else "nonzero"}"/>'
    if mode == 'reflect':
        shape = f'<g transform="translate(1 0) scale(-1 1)">{shape}</g>'
    test = (f'<g transform="translate(0 {i * 2})"><g id="sum-{name}">{shape}</g>'
            f'<rect x="2" width="1" height="1" fill="white"/><use href="#sum-{name}" x="2"/></g>')
    alpha = 0 if mode == 'cancel' else float(count * F(1, 4000))
    color = (40 / 255, 95 / 255, 167 / 255)
    over_white = tuple(1 - alpha * (1 - channel) for channel in color)
    pixels = [[(*color, alpha), (0, 0, 0, 0), (*over_white, 1), (0, 0, 0, 0)],
              [(0, 0, 0, 0)] * 4]
    expected = f'<image y="{i * 2}" width="4" height="2" href="{png(pixels)}"/>'
    unit.cell(name, (0, i * 2, 4, 2), test, expected)
unit.write()
