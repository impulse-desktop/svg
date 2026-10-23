"""Exact rational dash areas after long, axis-aligned offscreen paths.

All joins stay outside the viewport. The visible stroke is one pixel wide,
so its covered areas are intersections of rational dash and pixel intervals.
"""
from fractions import Fraction as F

from unitgen import Unit, png


unit = Unit('dash-distance', 128, 12,
            'Dash positions retain their distance after many offscreen segments. Rational interval areas check length, subdivision, reflection, offsets, odd patterns and a vertical stroke.', ground=False)
cases = [('short', 0, 'normal'), ('long', 128, 'normal'),
         ('longer', 256, 'normal'), ('subdivided', 128, 'subdivide'),
         ('reflected', 256, 'reflect'), ('offset', 128, 'offset'),
         ('odd-pattern', 128, 'odd'), ('vertical', 256, 'vertical'),
         ('negative-offset', 128, 'negative-offset')]


def dash_area(low, high, pattern, offset):
    if len(pattern) % 2:
        pattern = pattern * 2
    period = sum(pattern)
    low, high = low + offset, high + offset
    first, last = low // period, high // period
    result = F(0)
    for cycle in range(first, last + 1):
        start = cycle * period
        for i, length in enumerate(pattern):
            end = start + length
            if i % 2 == 0:
                result += max(F(0), min(high, end) - max(low, start))
            start = end
    return result


for i, (name, count, mode) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 4
    points = [(-100, -100)]
    for _ in range(count):
        if mode == 'subdivide':
            points.extend((px, -100) for px in (-80, -60, -40, -20, -40, -60, -80, -100))
        else:
            points.extend([(-20, -100), (-100, -100)])
    points.extend([(-100, 1.5), (32, 1.5)])
    path = 'M' + 'L'.join(f'{a} {b}' for a, b in points)
    pattern = [F(1, 10), F(13, 100)]
    if mode == 'odd':
        pattern.append(F(7, 100))
    offset = F(7, 100) if mode == 'offset' else -F(7, 100) if mode == 'negative-offset' else F(0)
    shape = f'<path d="{path}" fill="none" stroke="#3066b2" stroke-width="1" stroke-dasharray="{" ".join(str(float(v)) for v in pattern)}" stroke-dashoffset="{float(offset)}"/>'
    if mode == 'reflect':
        shape = f'<g transform="translate(32 0) scale(-1 1)">{shape}</g>'
    elif mode == 'vertical':
        shape = f'<g transform="matrix(0 1 1 0 0 0)">{shape}</g>'
    test = f'<svg x="{x}" y="{y}" width="32" height="4">{shape}</svg>'
    prefix = 160 * count + F(403, 2)
    pixels = []
    for py in range(4):
        row = []
        for px in range(32):
            at = py if mode == 'vertical' else 31 - px if mode == 'reflect' else px
            covered = px == 1 if mode == 'vertical' else py == 1
            alpha = dash_area(prefix + at, prefix + at + 1, pattern, offset) if covered else F(0)
            row.append((48 / 255, 102 / 255, 178 / 255, float(alpha)))
        pixels.append(row)
    expected = f'<image x="{x}" y="{y}" width="32" height="4" href="{png(pixels)}"/>'
    unit.cell(name, (x, y, 32, 4), test, expected)

unit.write()
