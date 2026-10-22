"""Independent pixel values for straight-sided bilinear mesh patches.

The four corner weights are products of distance along the two axes.
Native PNG controls do not use mesh or gradient rendering. The affine,
object-box and clipping cases have the same known device-space square.
"""
from unitgen import Unit, png

RED, BLUE, WHITE, BLACK = (1, 0, 0, 1), (0, 0, 1, 1), (1, 1, 1, 1), (0, 0, 0, 1)
corners = [RED, BLUE, BLUE, RED]
cases = [
    ('horizontal', corners, '', 32),
    ('vertical', [WHITE, WHITE, BLUE, BLUE], '', 32),
    ('saddle', [WHITE, BLACK, WHITE, BLACK], '', 32),
    ('alpha', [(1, 0, 0, 0), BLUE, BLUE, (1, 0, 0, 0)], '', 32),
    ('affine', corners, 'gradientTransform="scale(.5)"', 64),
    ('object-box', corners, 'gradientUnits="objectBoundingBox"', 1),
    ('constant', [(.2, .4, .6, 1)] * 4, '', 32),
    ('clip', corners, '', 32),
]
unit = Unit('mesh-reference-bilinear', 128, 64,
            'Straight mesh patches interpolate the four corner colors and alpha bilinearly. Expected images use analytic pixel-center weights, including an affine transform, objectBoundingBox coordinates and a clipped referencing shape.', ground=False)
for i, (name, colors, extra, extent) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 32
    attributes = extra if 'gradientUnits' in extra else 'gradientUnits="userSpaceOnUse" ' + extra
    paths = [f'L {extent} 0', f'L {extent} {extent}', f'L 0 {extent}', 'L 0 0']
    stops = ''.join(f'<stop path="{path}" stop-color="rgb({r*100}% {g*100}% {b*100}%)" stop-opacity="{a}"/>'
                    for path, (r, g, b, a) in zip(paths, colors))
    mesh = f'<meshgradient id="m{i}" {attributes}><meshrow><meshpatch>{stops}</meshpatch></meshrow></meshgradient>'
    shape = '<rect x="4" y="4" width="24" height="24"' if name == 'clip' else '<rect width="32" height="32"'
    pixels = []
    for py in range(32):
        row = []
        for px in range(32):
            u, v = (px + .5) / 32, (py + .5) / 32
            weights = [(1-u)*(1-v), u*(1-v), u*v, (1-u)*v]
            color = tuple(sum(c[k]*weight for c, weight in zip(colors, weights)) for k in range(4))
            row.append((0, 0, 0, 0) if name == 'clip' and not (4 <= px < 28 and 4 <= py < 28) else color)
        pixels.append(row)
    unit.cell(name, (x, y, 32, 32),
              f'<g transform="translate({x} {y})"><defs>{mesh}</defs>{shape} fill="url(#m{i})"/></g>',
              f'<image x="{x}" y="{y}" width="32" height="32" href="{png(pixels)}"/>')
unit.write()
