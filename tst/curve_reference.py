#!/usr/bin/env python3
"""Exact-area controls for the reviewed Inkscape cubic-band reference."""
import math


def area(points):
    # Green's theorem: integrate (x y' - y x') / 2 in the power basis.
    def coefficients(values):
        a, b, c, d = values
        return [a, 3 * (b - a), 3 * (a - 2 * b + c), -a + 3 * b - 3 * c + d]
    x = coefficients([p[0] for p in points])
    y = coefficients([p[1] for p in points])
    return sum((x[i] * j * y[j] - y[i] * j * x[j]) / (i + j)
               for i in range(4) for j in range(1, 4)) / 2


def arc(radius, count):
    segments = []
    step = math.pi / count
    k = 4 / 3 * math.tan(step / 4)
    for i in range(count):
        a, b = i * step, (i + 1) * step
        ca, sa, cb, sb = math.cos(a), math.sin(a), math.cos(b), math.sin(b)
        segments.append([(radius * ca, radius * sa),
                         (radius * (ca - k * sa), radius * (sa + k * ca)),
                         (radius * (cb + k * sb), radius * (sb - k * cb)),
                         (radius * cb, radius * sb)])
    return segments


def drawing(radius, width, phase, rule):
    # Different segment counts prevent errors in the two boundaries from
    # cancelling. These are cubic bands, not exact circular annuli.
    outer, inner = arc(radius, 2), arc(radius - width, 4)
    expected = sum(map(area, outer)) - sum(map(area, inner))
    inner = [list(reversed(s)) for s in reversed(inner)]

    def point(p):
        return f'{p[0] + 96 + phase:.12g},{p[1] + 24 + phase:.12g}'
    path = 'M' + point(outer[0][0])
    path += ''.join('C' + ' '.join(map(point, s[1:])) for s in outer)
    path += 'L' + point(inner[0][0])
    path += ''.join('C' + ' '.join(map(point, s[1:])) for s in inner) + 'Z'
    svg = f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 192 128"><path fill-rule="{rule}" d="{path}"/></svg>'
    return svg, expected


def probes():
    for radius in [24, 48, 72]:
        for width in [.3, .75, 1.92, 2.88]:
            for phase in [0, .125, .37]:
                yield drawing(radius, width, phase, "nonzero")
