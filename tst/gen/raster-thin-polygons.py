"""Exact pixel areas for polygons in very thin, ratio-preserving viewports.

The oracle clips polygons to pixel squares using rational coordinates and
computes their shoelace areas. No path flattening or edge rasterizer is
used to construct the expected RGBA samples.
"""
from fractions import Fraction as F
from unitgen import Unit, png


def clip(poly, axis, boundary, lower):
    out = []
    if not poly:
        return out
    for a, b in zip(poly, poly[1:] + poly[:1]):
        inside_a = a[axis] >= boundary if lower else a[axis] <= boundary
        inside_b = b[axis] >= boundary if lower else b[axis] <= boundary
        if inside_a:
            out.append(a)
        if inside_a != inside_b:
            t = (boundary - a[axis]) / (b[axis] - a[axis])
            out.append(tuple(a[j] + t * (b[j] - a[j]) for j in range(2)))
    return out


def area(poly, x, y):
    for axis, boundary, lower in [(0, x, True), (0, x + 1, False),
                                  (1, y, True), (1, y + 1, False)]:
        poly = clip(poly, axis, F(boundary), lower)
    if not poly:
        return 0
    return abs(sum(a[0] * b[1] - b[0] * a[1]
                   for a, b in zip(poly, poly[1:] + poly[:1]))) / 2


band = [(0, 0), (48, 0), (48, 1), (0, 1)]
taper = [(0, F(1, 2)), (4, 0), (44, 0), (48, F(1, 2)), (44, 1), (4, 1)]
triangle = [(0, 1), (48, 0), (48, 1)]
cases = [
    ('band', band, 'meet', F(3)),
    ('taper', taper, 'meet', F(3)),
    ('reverse', list(reversed(taper)), 'meet', F(3)),
    ('triangle', triangle, 'meet', F(3)),
    ('none', taper, 'none', F(3)),
    ('quarter-shift', taper, 'meet', F(13, 4)),
    ('half-shift', triangle, 'meet', F(7, 2)),
    ('vertical', [(F(y), F(x)) for x, y in taper], 'vertical', F(0)),
]
u = Unit('raster-thin-polygons', 64, 16,
         'Thin viewports keep the exact meet scale and letterbox. Polygon coverage is its area inside each pixel, including tapered tips, reversed winding, stretch, fractional placement and vertical orientation. Rational polygon clipping and shoelace areas define independent PNG expectations.', ground=False)
for i, (name, points, mode, top) in enumerate(cases):
    x, y = i % 4 * 16, i // 4 * 8
    if mode == 'vertical':
        attrs = 'x="7" width="1" height="8" viewBox="0 0 1 48"'
        mapped = [(F(7) + F(5, 12) + F(a) / 6, F(b) / 6) for a, b in points]
    else:
        attrs = f'y="{float(top)}" width="16" height="1" viewBox="0 0 48 1" preserveAspectRatio="{mode if mode == "none" else "xMidYMid meet"}"'
        mapped = [(F(a) / 3, top + (F(b) if mode == 'none' else F(1, 3) + F(b) / 3)) for a, b in points]
    data = ' '.join(f'{float(a)},{float(b)}' for a, b in points)
    shape = f'<polygon points="{data}" fill="#ff921e"/>'
    if name in ('quarter-shift', 'half-shift'):
        # Keep the fractional polygon inside an integer-aligned viewport.
        # A fractional viewport edge introduces a separate antialiased clip
        # mask, whose coverage product is not this polygon-area oracle.
        attrs = 'width="16" height="8" viewBox="0 0 48 24"'
        shape = f'<g transform="translate(0 {float(3 * top + 1)})">{shape}</g>'
    test = f'<g transform="translate({x} {y})"><svg {attrs}>{shape}</svg></g>'
    pixels = [[(1, 146 / 255, 30 / 255, float(area(mapped, px, py))) for px in range(16)] for py in range(8)]
    expected = f'<image x="{x}" y="{y}" width="16" height="8" href="{png(pixels)}"/>'
    u.cell(name, (x, y, 16, 8), test, expected)
u.write()
