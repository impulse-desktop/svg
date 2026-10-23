"""A round join follows the signed tangent turn across atan2's cut.

At a reversal the join is a semicircle in front of the incoming segment.
An arbitrarily small change in either direction approaches that same
semicircle, rather than sweeping three half-turns. The displacement in
these near reversals is below 1e-8 device pixels; the literal outline of
a rectangle with a semicircular end gives an independent expectation (D1).
Openclipart 197846 exposed the rounded-angle subtraction at its close.
"""
from unitgen import Unit

u = Unit('stroke-round-reversal', 384, 384,
         'Round joins at exact and near reversals sweep one half-circle in front of the incoming segment. Both path directions, both sides of the angular cut and five headings agree with the literal outline of a rectangle with a semicircular end. The near-reversal displacement is below 1e-8 device pixels; raster approximation is allowed within 4 levels (D1).', tolerance=4)
for row, angle in enumerate([0, 90, 180, 270, 35]):
    for col, (offset, reverse) in enumerate((e, rev) for e in ['0', '.0000000001', '-.0000000001'] for rev in [False, True]):
        x, y = col * 64, row * 64
        via = '32' if offset == '0' else ('32.0000000001' if not offset.startswith('-') else '31.9999999999')
        data = f'M42 32L22 {via}L48 32' if not reverse else f'M48 32L22 {via}L42 32'
        frame = f'translate({x} {y}) rotate({angle} 32 32)'
        test = f'<path transform="{frame}" d="{data}" fill="none" stroke="black" stroke-width="12" stroke-linejoin="round"/>'
        expected = f'<path transform="{frame}" d="M22 26H48V38H22A6 6 0 0 1 22 26Z"/>'
        u.cell(f'heading-{angle}-side-{col//2}-{"reverse" if reverse else "forward"}', (x, y, 64, 64), test, expected)

# A two-vertex closed subpath joins both returning ends with semicircles.
for col, (angle, reverse) in enumerate([(0, False), (0, True), (35, False), (35, True)]):
    x, y = col*64, 320
    data = 'M48 32L22 32Z' if reverse else 'M22 32L48 32Z'
    frame = f'translate({x} {y}) rotate({angle} 32 32)'
    test = f'<path transform="{frame}" d="{data}" fill="none" stroke="black" stroke-width="12" stroke-linejoin="round"/>'
    plain = f'<path transform="{frame}" d="M22 26H48A6 6 0 0 1 48 38H22A6 6 0 0 1 22 26Z"/>'
    u.cell(f'closed-{angle}-{"reverse" if reverse else "forward"}', (x, y, 64, 64), test, plain)
u.write()
