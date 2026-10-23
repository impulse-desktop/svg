"""Fractional border slices stretch directly to their assigned regions.

Independent expectations draw all nine source rectangles with literal
affine maps and a user-space radial gradient. No CSS border layout or
intermediate repeating raster is used in the expected drawing.
"""
from fractions import Fraction as F
from math import sqrt
from unitgen import Unit

unit = Unit('mask-border-fractional-stretch', 256, 64,
            'A stretched mask-border part keeps the exact fractional size of its region. Percentages, lengths, exchanged axes and whole lengths agree with nine explicitly placed source rectangles and a radial SVG gradient.', ground=False)
for i, (name, widths, vertical, horizontal) in enumerate([
        ('percent', '20% 10%', F('2.8'), F('1.4')),
        ('lengths', '2.8px 1.4px', F('2.8'), F('1.4')),
        ('axes', '10% 20%', F('1.4'), F('2.8')),
        ('whole', '2px 1px', F(2), F(1))]):
    x = 64*i
    paint = '<rect x="17" y="17" width="14" height="14" fill="teal"'
    test = paint + f' style="mask-border:radial-gradient(black 30%, transparent 70%) 30% fill / {widths}"/>'
    source = [F(0), F('4.2'), F('9.8'), F(14)]
    dx, dy = [F(17), F(17)+horizontal, F(31)-horizontal, F(31)], [F(17), F(17)+vertical, F(31)-vertical, F(31)]
    parts = []
    def fmt(v):
        return format(float(v), '.17g')
    for r in range(3):
        for c in range(3):
            a, d = (dx[c+1]-dx[c])/(source[c+1]-source[c]), (dy[r+1]-dy[r])/(source[r+1]-source[r])
            e, f = dx[c]-a*source[c], dy[r]-d*source[r]
            matrix = ' '.join(map(fmt, [a, 0, 0, d, e, f]))
            parts.append(f'<rect x="{fmt(source[c])}" y="{fmt(source[r])}" width="{fmt(source[c+1]-source[c])}" height="{fmt(source[r+1]-source[r])}" transform="matrix({matrix})" fill="url(#g{i})"/>')
    expected = (f'<defs><radialGradient id="g{i}" gradientUnits="userSpaceOnUse" cx="7" cy="7" r="{7*sqrt(2):.17g}">'
                '<stop offset=".3"/><stop offset=".7" stop-opacity="0"/></radialGradient>'
                f'<mask id="m{i}" maskUnits="userSpaceOnUse" x="0" y="0" width="64" height="64" style="mask-type:alpha">'
                + ''.join(parts) + f'</mask></defs>' + paint + f' mask="url(#m{i})"/>')
    def frame(content):
        return f'<svg x="{x}" width="64" height="64">{content}</svg>'
    unit.cell(name, (x, 0, 64, 64), frame(test), frame(expected))
unit.write()
