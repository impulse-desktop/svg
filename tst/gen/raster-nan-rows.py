"""Exact signed coverage of the transformed, distant-coordinate fixture.

The small contour becomes the parallelogram R below. The other contour
becomes (6e38, 0), (.5, 2.5), (1, 4), with opposite winding. Over an 8x8
canvas its far edges differ from horizontal lines by less than 1e-35
pixel. Thus its visible part is T below to much better than alpha
quantization. Nonzero fill is the symmetric difference R xor T: their
intersection has cancelling windings, and must be subtracted twice.
"""
from fractions import Fraction as F
from pathlib import Path

from unitgen import ROOT, png


def clip(polygon, a, b, c):
    """Intersect with a*x+b*y+c >= 0 using exact rational arithmetic."""
    result = []
    for p, q in zip(polygon, polygon[1:] + polygon[:1]):
        u = a * p[0] + b * p[1] + c
        v = a * q[0] + b * q[1] + c
        if u >= 0:
            result.append(p)
        if (u >= 0) != (v >= 0):
            t = u / (u - v)
            result.append(tuple(p[k] + t * (q[k] - p[k]) for k in (0, 1)))
    return result


def area(polygon, x, y):
    for plane in ((1, 0, -x), (-1, 0, x + 1),
                  (0, 1, -y), (0, -1, y + 1)):
        polygon = clip(polygon, *plane)
    return abs(sum(p[0] * q[1] - q[0] * p[1]
                   for p, q in zip(polygon, polygon[1:] + polygon[:1]))) / 2


rectangle = [(F(1, 2), F(1)), (F(7, 2), F(4)),
             (F(7, 2), F(5)), (F(1, 2), F(2))]
triangle = [(F(1, 2), F(5, 2)), (F(1), F(4)),
            (F(8), F(4)), (F(8), F(5, 2))]
overlap = rectangle
for plane in ((1, 0, -F(1, 2)), (0, 1, -F(5, 2)),
              (0, -1, 4), (3, -1, 1)):
    overlap = clip(overlap, *plane)

pixels = []
for y in range(8):
    row = []
    for x in range(8):
        coverage = area(rectangle, x, y) + area(triangle, x, y) - 2 * area(overlap, x, y)
        assert 0 <= coverage <= 1
        row.append((0, 128 / 255, 128 / 255, float(coverage)))
    pixels.append(row)
Path(ROOT, 'tst/expect/raster-nan-rows.svg').write_text(
    '<svg xmlns="http://www.w3.org/2000/svg" width="8" height="8">\n'
    '<!-- Exact pixel areas of two oppositely wound contours; overlap cancels. -->\n'
    '<g id="nonzero" data-box="0 0 8 8"><image width="8" height="8" '
    f'href="{png(pixels)}"/></g>\n</svg>\n')
