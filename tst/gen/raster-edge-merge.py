"""Exact coincident-edge reduction, independently checked by rectangle areas.

Dense tilings have thousands of edges but describe one fractional rectangle.
Repeated contours check winding addition and parity without repainting opacity.
"""
from fractions import Fraction as F

from unitgen import Unit, png


unit = Unit('raster-edge-merge', 32, 24,
            'Coincident edges add signed winding, including cancellation and '
            'evenodd parity. Adjacent tiles form one rectangle; gaps and '
            'different endpoint extents remain distinct. Expected alpha is '
            'the exact rational intersection area of each pixel and rectangle.',
            ground=False)


def path(rect, reverse=False):
    x0, y0, x1, y1 = rect
    points = [(x0, y0), (x1, y0), (x1, y1), (x0, y1)]
    if reverse:
        points.reverse()
    return 'M' + 'L'.join(f'{float(x):.17g} {float(y):.17g}' for x, y in points) + 'Z'


rect = (F(1, 8), F(1, 8), F(63, 8), F(61, 8))
shape = path(rect)
opposite = path(rect, True)
# Binary rational coordinates make every shared endpoint exactly representable.
tiles = [(F(1, 8) + F(i, 256), rect[1],
          F(1, 8) + F(i + 1, 256), rect[3]) for i in range(1984)]
hole = (F(17, 8), F(13, 8), F(47, 8), F(45, 8))
left = (F(1, 8), F(1, 8), F(31, 8), F(61, 8))
right = (F(33, 8), F(1, 8), F(63, 8), F(61, 8))
short = (F(31, 8), F(9, 8), F(63, 8), F(53, 8))
cases = [
    ('positive', shape * 1025, 'nonzero', F(1), [(rect, 1)]),
    ('negative', opposite * 1025, 'nonzero', F(1), [(rect, 1)]),
    ('cancelled', (shape + opposite) * 1025, 'nonzero', F(1), []),
    ('cancelled-then-positive', (shape + opposite) * 512 + shape, 'nonzero', F(1), [(rect, 1)]),
    ('even', shape * 1024, 'evenodd', F(1), []),
    ('odd', shape * 1025, 'evenodd', F(1), [(rect, 1)]),
    ('opacity-once', shape * 1025, 'nonzero', F(1, 2), [(rect, 1)]),
    ('adjacent-tiles', ''.join(path(t) for t in tiles), 'nonzero', F(1), [(rect, 1)]),
    ('adjacent-evenodd', ''.join(path(t) for t in tiles), 'evenodd', F(1), [(rect, 1)]),
    ('hole', (shape + path(hole, True)) * 513, 'nonzero', F(1), [(rect, 1), (hole, -1)]),
    ('gap', (path(left) + path(right)) * 513, 'nonzero', F(1), [(left, 1), (right, 1)]),
    ('different-extents', (path(left) + path(short)) * 513, 'nonzero', F(1), [(left, 1), (short, 1)]),
]
for i, (name, data, rule, opacity, expected) in enumerate(cases):
    x, y = i % 4 * 8, i // 4 * 8
    test = (f'<svg x="{x}" y="{y}" width="8" height="8">'
            f'<path d="{data}" fill="#255f9b" fill-rule="{rule}" '
            f'fill-opacity="{float(opacity)}"/></svg>')
    pixels = []
    for py in range(8):
        row = []
        for px in range(8):
            area = sum(sign * max(F(0), min(px + 1, r[2]) - max(px, r[0]))
                       * max(F(0), min(py + 1, r[3]) - max(py, r[1]))
                       for r, sign in expected)
            row.append((37 / 255, 95 / 255, 155 / 255, float(area * opacity)))
        pixels.append(row)
    plain = f'<image x="{x}" y="{y}" width="8" height="8" href="{png(pixels)}"/>'
    unit.cell(name, (x, y, 8, 8), test, plain)
unit.write()
