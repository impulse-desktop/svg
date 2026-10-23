"""Interior primitive subregions keep their fractional pixel coverage.

SourceGraphic is opaque red with a smaller opaque green rectangle. The
primitive subregion equals the green rectangle and lies strictly inside
the full source. Exact rectangle/pixel areas give the green coverage C,
so the source RGB is (1-C,C,0) and alpha is one. Apply Porter-Duff or the
arithmetic equation in the selected colour space, then multiply alpha by
the subregion area C. No filtering or clipping supplies the expectations.
"""
from fractions import Fraction as F

from unitgen import Unit, png


def area(pixel, lo, hi):
    return max(F(0), min(F(pixel + 1), hi) - max(F(pixel), lo))


def linear(value):
    return value / 12.92 if value <= .04045 else ((value + .055) / 1.055) ** 2.4


def srgb(value):
    return value * 12.92 if value <= .0031308 else 1.055 * value ** (1 / 2.4) - .055


def expected(ux, uy, operator, space):
    pixels = []
    for y in range(20):
        row = []
        for x in range(20):
            coverage = area(x, ux + F('3.25'), ux + F('7.75')) * area(y, uy + F('3.5'), uy + 8)
            rgb = [1 - float(coverage), float(coverage), 0]
            if operator in ['k1', 'k4']:
                if space == 'linearRGB':
                    rgb = [linear(v) for v in rgb]
                rgb = [v * v if operator == 'k1' else .75 * v * v + .25 for v in rgb]
                if space == 'linearRGB':
                    rgb = [srgb(v) for v in rgb]
            row.append(tuple(rgb) + (coverage,))
        pixels.append(row)
    return png(pixels)


unit = Unit('filter-subregion-composite', 60, 120,
            'A primitive subregion strictly inside opaque SourceGraphic '
            'clips each boundary pixel by its exact fractional area. '
            'Nine Porter-Duff/arithmetic forms use the same red/green '
            'source, in linearRGB and sRGB, including fractional use '
            'placement. Independent rational areas and colour equations '
            'supply every expected pixel. The source alpha is one even '
            'at the interior green rectangle edge.', ground=False)
unit.common('<defs><g id="source"><rect width="12" height="12" fill="red"/>'
            '<rect x="3.25" y="3.5" width="4.5" height="4.5" fill="lime"/></g></defs>', '')
operators = [
    ('in', '<feComposite in2="SourceGraphic" operator="in"'),
    ('out', '<feFlood flood-opacity="0"/><feComposite in="SourceGraphic" operator="out"'),
    ('atop', '<feComposite in2="SourceGraphic" operator="atop"'),
    ('over', '<feComposite in2="SourceGraphic" operator="over"'),
    ('xor', '<feFlood flood-opacity="0"/><feComposite in="SourceGraphic" operator="xor"'),
    ('k1', '<feComposite in2="SourceGraphic" operator="arithmetic" k1="1"'),
    ('k2', '<feComposite in2="SourceGraphic" operator="arithmetic" k2="1"'),
    ('k3', '<feComposite in2="SourceGraphic" operator="arithmetic" k3="1"'),
    ('k4', '<feComposite in2="SourceGraphic" operator="arithmetic" k1=".75" k4=".25"'),
]
for mode, space in enumerate(['linearRGB', 'sRGB']):
    ux, uy = (F(2), F(2)) if not mode else (F('2.125'), F('2.75'))
    for index, (name, primitive) in enumerate(operators):
        slot = mode * 9 + index
        x, y = slot % 3 * 20, slot // 3 * 20
        test = (f'<svg x="{x}" y="{y}" width="20" height="20">'
                f'<defs><filter id="f{slot}" color-interpolation-filters="{space}">'
                f'{primitive} x="3.25" y="3.5" width="4.5" height="4.5"/>'
                '</filter></defs>'
                f'<use href="#source" x="{float(ux)}" y="{float(uy)}" filter="url(#f{slot})"/></svg>')
        reference = f'<image x="{x}" y="{y}" width="20" height="20" href="{expected(ux, uy, name, space)}"/>'
        unit.cell(f'{space}-{name}', (x, y, 20, 20), test, reference)
unit.write()
