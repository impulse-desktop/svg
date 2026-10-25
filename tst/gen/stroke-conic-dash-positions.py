"""True conic arclength places round zero-length dashes independently."""
import math

from unitgen import Unit, png

def primitive(x, r):
    x = max(0, min(x, r))
    return 0.5 * (x * math.sqrt(max(0, r * r - x * x)) + r * r * math.asin(x / r))

def quadrant(x, y, r):
    x, y = (min(abs(x), r), min(abs(y), r))
    at = min(x, math.sqrt(max(0, r * r - y * y)))
    return y * at + primitive(x, r) - primitive(at, r)

def disk(left, top, right, bottom, r):
    """Exact disk/pixel intersection from the antiderivative of sqrt."""

    def area(x, y):
        return math.copysign(1, x) * math.copysign(1, y) * quadrant(x, y, r)
    return area(right, bottom) - area(left, bottom) - area(right, top) + area(left, top)
assert abs(disk(-1, -1, 1, 1, 1) - math.pi) < 1e-12

def integral(rx, ry, end):
    """Ellipse arclength by adaptive Simpson integration, not cubic curves."""

    def speed(t):
        return math.hypot(rx * math.sin(t), ry * math.cos(t))

    def refine(a, b, fa, fm, fb, whole, error, depth):
        m = (a + b) / 2
        l = speed((a + m) / 2)
        r = speed((m + b) / 2)
        left = (m - a) * (fa + 4 * l + fm) / 6
        right = (b - m) * (fm + 4 * r + fb) / 6
        if depth == 0 or abs(left + right - whole) <= 15 * error:
            return left + right + (left + right - whole) / 15
        return refine(a, m, fa, l, fm, left, error / 2, depth - 1) + refine(m, b, fm, r, fb, right, error / 2, depth - 1)
    if end == 0:
        return 0.0
    a = 0.0
    b = end
    fa = speed(a)
    fm = speed((a + b) / 2)
    fb = speed(b)
    return refine(a, b, fa, fm, fb, (b - a) * (fa + 4 * fm + fb) / 6, 1e-10, 24)

def parameter(rx, ry, distance):
    a, b = (0.0, 2 * math.pi)
    for i in range(48):
        m = (a + b) / 2
        if integral(rx, ry, m) < distance:
            a = m
        else:
            b = m
    return (a + b) / 2

def fmt(x):
    return format(float(x), '.17g')


unit = Unit('stroke-conic-dash-positions', 64, 32,
            "Zero-length round dashes follow true conic arclength, from each basic "
            "shape or arc path's specified start. Analytic cap disks check negative "
            "and equivalent positive offsets, reflection, rotation, opacity, both "
            "arc sweeps, a fully rounded rect and a noncircular ellipse. Independent "
            "disk/pixel integrals use the existing D1 .025px bound. No renderer "
            "pixels supply positions or tolerances.", ground=False)
# Name, geometry, radii, sweep, offset sign, opacity, rotation, reflection.
cases = [
    ('circle-negative', 'circle', 80., 80., 1, -1, 1., 0., False),
    ('circle-positive', 'circle', 80., 80., 1, 1, 1., 0., False),
    ('circle-reflected', 'circle', 80., 80., 1, -1, 1., 0., True),
    ('circle-opacity-rotation', 'circle', 80., 80., 1, -1, .5, math.pi/7, False),
    ('four-arcs', 'arcs', 80., 80., 1, -1, 1., 0., False),
    ('four-arcs-reversed', 'arcs', 80., 80., -1, -1, 1., 0., False),
    ('fully-rounded-rect', 'rect', 80., 80., 1, -1, 1., 0., False),
    ('ellipse', 'ellipse', 80., 40., 1, -1, 1., 0., False),
]
for i, (name, kind, rx, ry, direction, offset_sign, opacity, rotate, reflect) in enumerate(cases):
    total = 2 * math.pi * rx if rx == ry else 4 * integral(rx, ry, math.pi / 2)
    theta = 1.8 * math.pi if rx == ry else parameter(rx, ry, 0.9 * total)
    theta *= direction
    if kind == 'rect':
        theta -= math.pi / 2
    source = (rx * math.cos(theta), ry * math.sin(theta))
    tangent = (-direction * rx * math.sin(theta), direction * ry * math.cos(theta))
    norm = math.hypot(*tangent)
    tangent = tuple((x / norm for x in tangent))
    cosine, sine = (math.cos(rotate), math.sin(rotate))
    transform = lambda p: (cosine * (-p[0] if reflect else p[0]) - sine * p[1], sine * (-p[0] if reflect else p[0]) + cosine * p[1])
    p = transform(source)
    n = transform(tangent)
    half = 20.0
    center = (7.5 - half * n[0], 7.5 - half * n[1])
    translation = (center[0] - p[0], center[1] - p[1])
    matrix = (-cosine if reflect else cosine, -sine if reflect else sine, -sine, cosine, *translation)
    # Other caps lie outside the cell, so one analytic disk suffices.
    for fraction in [0.1, 0.5, 0.7]:
        angle = (fraction * 2 * math.pi if rx == ry else parameter(rx, ry, fraction * total)) * direction
        if kind == 'rect':
            angle -= math.pi / 2
        other = transform((rx * math.cos(angle), ry * math.sin(angle)))
        other = tuple((other[k] + translation[k] for k in range(2)))
        distance = math.hypot(*(max(0, -value, value - 16) for value in other))
        assert distance > half + 0.025
    style = f'fill="none" stroke="black" stroke-width="40" stroke-linecap="round" stroke-opacity="{opacity}" stroke-dasharray="0 {fmt(0.2 * total)} 0 {fmt(0.2 * total)} 0 {fmt(0.2 * total)} 0 {fmt(0.4 * total)}" stroke-dashoffset="{fmt(offset_sign * 0.5 * total)}" transform="matrix(' + ','.join(map(fmt, matrix)) + ')"'
    if kind == 'circle':
        shape = f'<circle r="{fmt(rx)}" {style}/>'
    elif kind == 'ellipse':
        shape = f'<ellipse rx="{fmt(rx)}" ry="{fmt(ry)}" {style}/>'
    elif kind == 'rect':
        shape = f'<rect x="-80" y="-80" width="160" height="160" rx="80" ry="80" {style}/>'
    else:
        arcpoints = [(0, direction * ry), (-rx, 0), (0, -direction * ry), (rx, 0)]
        d = f'M{fmt(rx)} 0' + ''.join((f'A{fmt(rx)} {fmt(ry)} 0 0 {(1 if direction > 0 else 0)} {fmt(x)} {fmt(y)}' for x, y in arcpoints)) + 'Z'
        shape = f'<path d="{d}" {style}/>'
    pixels = []
    bound = 0.0
    for y in range(16):
        row = []
        for x in range(16):
            left, top = (x - center[0], y - center[1])
            exact = disk(left, top, left + 1, top + 1, half)
            lo = disk(left, top, left + 1, top + 1, half - 0.025)
            hi = disk(left, top, left + 1, top + 1, half + 0.025)
            bound = max(bound, opacity * (exact - lo), opacity * (hi - exact))
            row.append((0.0, 0.0, 0.0, opacity * max(0, min(1, exact))))
        pixels.append(row)
    x = i % 4 * 16
    y = i // 4 * 16
    unit.cell(name, (x, y, 16, 16), f'<svg x="{x}" y="{y}" width="16" height="16">' + shape + '</svg>', f'<image x="{x}" y="{y}" width="16" height="16" href="{png(pixels)}"/>', tolerance=math.ceil(255 * bound) + 1)
unit.write()
