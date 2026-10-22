"""Independent signed coverage of the large-coordinate sorting fixtures.

After scaling by two its first contour is a bow tie with two disjoint
triangular lobes. The second contour is a triangle with vertices
(.2,.2), (.2,6), (6e38,.2). Within the eight-pixel canvas it differs from
the rectangle [.2,8] x [.2,6] by less than 1e-36 of a pixel, below even
the renderer's internal alpha quantization. The bow tie's right lobe has
the opposite winding: it cancels that rectangle where they overlap. Below
y=6 both lobes add area. Integrate those signed regions directly. The
second fixture starts the huge triangle at y=3 instead of y=.2, exposing
the bow tie above it as well.
"""
from pathlib import Path

from unitgen import ROOT, png


def clip(poly, axis, value, side):
    result = []
    for a, b in zip(poly, poly[1:] + poly[:1]):
        va = side * (a[axis] - value)
        vb = side * (b[axis] - value)
        if va >= 0:
            result.append(a)
        if (va >= 0) != (vb >= 0):
            t = va / (va - vb)
            result.append(tuple(a[k] + t * (b[k] - a[k]) for k in (0, 1)))
    return result


for name, top in (('raster-nan-band-first', .2), ('raster-nan-band', 3)):
    pixels = []
    for y in range(8):
        row = []
        for x in range(8):
            area = max(0, min(x + 1, 8) - max(x, .2)) * max(0, min(y + 1, 6) - max(y, top))
            left = [(1, 1), (4, 4.5), (1, 8)]
            right = [(7, 1), (7, 8), (4, 4.5)]
            for triangle, low, high, sign in ((left, 0, top, 1), (right, 0, top, 1),
                                             (left, 6, 8, 1), (right, 6, 8, 1),
                                             (right, top, 6, -1)):
                p = triangle
                for axis, value, side in ((0, x, 1), (0, x + 1, -1),
                                          (1, max(y, low), 1), (1, min(y + 1, high), -1)):
                    p = clip(p, axis, value, side)
                area += sign * abs(sum(a[0] * b[1] - b[0] * a[1]
                                       for a, b in zip(p, p[1:] + p[:1]))) / 2
            row.append((0, 128 / 255, 128 / 255, area))
        pixels.append(row)
    Path(ROOT, f'tst/expect/{name}.svg').write_text(
        '<svg xmlns="http://www.w3.org/2000/svg" width="8" height="8">\n'
        '<!-- Nonzero winding of the bow tie and the visible part of the enormous triangle. -->\n'
        '<g id="nonzero" data-box="0 0 8 8"><image width="8" height="8" '
        f'href="{png(pixels)}"/></g>\n</svg>\n')
