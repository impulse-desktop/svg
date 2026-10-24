"""Genuine shears/scales retain reconstruction despite nearby texel knots.

Clip the image parallelogram by each pixel using exact rational polygons.
Sample with the independently stated Mitchell B=C=1/3 polynomial. This
checks both native-grid rejection axes and their individual scale/shear
conditions; source bytes and coverage do not come from renderer output.
"""
from fractions import Fraction as F
import math
from unitgen import Unit, png

pixels = [[((x % 3)*127, (y % 3)*127, ((x+y) % 3)*127,
            [255, 128, 64][(x+2*y) % 3]) for x in range(8)] for y in range(8)]
source = png([[tuple(v/255 for v in p) for p in row] for row in pixels])
cases = [
    ('shear-x', (1, 0, 2, 1, 2, 2)),
    ('shear-y', (1, 2, 0, 1, 2, 2)),
    ('small-shear-x', (1, 0, F(1, 4), 1, 4, 4)),
    ('small-shear-y', (1, F(1, 4), 0, 1, 4, 4)),
    ('scale-x', (2, 0, 0, 1, 4, 4)),
    ('scale-y', (1, 0, 0, 2, 4, 4)),
    ('reflected-phase', (-1, 0, 0, 1, F(65, 4), 4)),
    ('negative-shear', (1, -2, 0, 1, 4, 20)),
    ('quarter-turn-scale', (0, 2, -1, 0, 16, 4)),
    ('oblique-rotation', (F(3, 5), F(4, 5), F(-4, 5), F(3, 5), 14, 4)),
]
unit = Unit('image-affine-grid', 160, 64,
            'A shared dominant axis, genuine shear, non-unit scale and '
            'fractional phase keep independently reconstructed PNG values. '
            'Coverage is the exact clipped image parallelogram.', ground=False)


def clipped(polygon, axis, bound, sign):
    result = []
    for p, q in zip(polygon, polygon[1:]+polygon[:1]):
        inside, next_inside = sign*(p[axis]-bound) >= 0, sign*(q[axis]-bound) >= 0
        if inside: result.append(p)
        if inside != next_inside:
            t = (bound-p[axis])/(q[axis]-p[axis])
            result.append(tuple(a+t*(b-a) for a, b in zip(p, q)))
    return result


def coverage(polygon, x, y):
    for axis, bound, sign in [(0, x, 1), (0, x+1, -1), (1, y, 1), (1, y+1, -1)]:
        polygon = clipped(polygon, axis, F(bound), sign)
        if not polygon: return F(0)
    return abs(sum(p[0]*q[1]-p[1]*q[0]
                   for p, q in zip(polygon, polygon[1:]+polygon[:1])))/2


def mitchell(t):
    a = (1-9*t+15*t*t-7*t*t*t)/18
    c = (1+9*t+27*t*t-21*t*t*t)/18
    d = (7*t*t*t-6*t*t)/18
    return [a, 1-a-c-d, c, d]


for i, (name, transform) in enumerate(cases):
    a, b, c, d, e, f = map(F, transform)
    det = a*d-b*c
    polygon = [(a*x+c*y+e, b*x+d*y+f) for x, y in [(0, 0), (8, 0), (8, 8), (0, 8)]]
    expected = [[(0, 0, 0, 0) for _ in range(32)] for _ in range(32)]
    for py in range(32):
        for px in range(32):
            area = coverage(polygon, px, py)
            if not area: continue
            x, y = F(2*px+1, 2)-e, F(2*py+1, 2)-f
            qx, qy = (d*x-c*y)/det-F(1, 2), (a*y-b*x)/det-F(1, 2)
            ix, iy = math.floor(qx), math.floor(qy)
            wx, wy = mitchell(qx-ix), mitchell(qy-iy)
            value = [F(0) for _ in range(4)]
            for j in range(4):
                for k in range(4):
                    pixel = pixels[min(7, max(0, iy+j-1))][min(7, max(0, ix+k-1))]
                    weight = wx[k]*wy[j]
                    for channel in range(4):
                        v = F(pixel[channel], 255)
                        if channel < 3: v *= F(pixel[3], 255)
                        value[channel] += weight*v
            alpha = min(F(1), max(F(0), value[3]))
            colour = [min(alpha, max(F(0), v))/alpha if alpha else F(0) for v in value[:3]]
            expected[py][px] = tuple(map(float, (*colour, alpha*area)))
    x, y = i % 5*32, i // 5*32
    matrix = ' '.join(str(float(v)) for v in transform)
    unit.cell(name, (x, y, 32, 32),
              f'<svg x="{x}" y="{y}" width="32" height="32"><g transform="matrix({matrix})">'
              f'<image width="8" height="8" preserveAspectRatio="none" href="{source}"/></g></svg>',
              f'<image x="{x}" y="{y}" width="32" height="32" href="{png(expected)}"/>')
unit.write()
