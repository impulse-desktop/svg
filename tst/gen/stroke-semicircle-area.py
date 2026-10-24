"""Exact semicircular stroke areas, with the existing D1 geometric bound.

An open half circle with butt caps is an annulus intersected with a half
plane. Eroding/dilating its radii and half plane by .025 bounds the stroke
approximation, including its endpoints. No renderer pixels enter the math.
"""
import math
from unitgen import Unit, png


def primitive(x, radius):
    x = max(0, min(x, radius))
    return .5 * (x * math.sqrt(max(0, radius * radius - x * x))
                 + radius * radius * math.asin(x / radius))


def quadrant(x, y, radius):
    x, y = min(abs(x), radius), min(abs(y), radius)
    at = min(x, math.sqrt(max(0, radius * radius - y * y)))
    return y * at + primitive(x, radius) - primitive(at, radius)


def disk_area(left, top, right, bottom, radius):
    if left >= right or top >= bottom:
        return 0
    def signed(x, y):
        return math.copysign(1, x) * math.copysign(1, y) * quadrant(x, y, radius)
    return signed(right, bottom) - signed(left, bottom) - signed(right, top) + signed(left, top)


assert abs(disk_area(-1, -1, 1, 0, 1) - math.pi / 2) < 1e-12
assert abs(disk_area(0, 0, 1, 1, 1) - math.pi / 4) < 1e-12
assert abs(disk_area(-.25, -.25, .25, .25, 1) - .25) < 1e-12

unit = Unit('stroke-semicircle-area', 64, 16,
            'Open circular arcs paint the area of their annular half bands '
            'with butt caps. Independent disk/pixel integrals check both '
            'sweeps, small and larger radii and stroke opacity. D1 .025px '
            'erosion/dilation includes both curved boundaries and cap edges.',
            ground=False)
for i, (name, radius, half, lower, opacity) in enumerate([
        ('upper-small', .75, .125, False, 1),
        ('lower-small', .75, .125, True, 1),
        ('upper-large', 4, .5, False, 1),
        ('upper-half-opacity', .75, .125, False, .5)]):
    pixels, bound = [], 0
    for y in range(16):
        row = []
        for x in range(16):
            def area(inner, outer, cut):
                left, right = x - 8.5, x + 1 - 8.5
                top, bottom = y - 8.5, y + 1 - 8.5
                if lower:
                    top = max(top, cut)
                else:
                    bottom = min(bottom, cut)
                return max(0, disk_area(left, top, right, bottom, outer)
                           - disk_area(left, top, right, bottom, inner))
            exact = area(radius - half, radius + half, 0)
            lo = area(radius - half + .025, radius + half - .025,
                      .025 if lower else -.025)
            hi = area(radius - half - .025, radius + half + .025,
                      -.025 if lower else .025)
            bound = max(bound, opacity * max(abs(exact - lo), abs(hi - exact)))
            row.append((0, 0, 0, opacity * exact))
        pixels.append(row)
    x = i * 16
    unit.cell(name, (x, 0, 16, 16),
              f'<svg x="{x}" width="16" height="16">'
              f'<path d="M{8.5 - radius} 8.5A{radius} {radius} 0 0 {0 if lower else 1} {8.5 + radius} 8.5" '
              f'fill="none" stroke="black" stroke-width="{2 * half}" '
              f'stroke-linecap="butt" stroke-opacity="{opacity}"/></svg>',
              f'<image x="{x}" width="16" height="16" href="{png(pixels)}"/>',
              tolerance=math.ceil(255 * bound) + 1)
unit.write()
