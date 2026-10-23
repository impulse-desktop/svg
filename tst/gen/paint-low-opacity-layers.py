"""Repeated faint fills retain source-over precision through a mask layer.

The expected PNGs use exact rational alpha/color equations, with a single
final byte conversion. No renderer supplies their pixels.
"""
from fractions import Fraction as F

from unitgen import Unit, png


unit = Unit('paint-low-opacity-layers', 98, 66,
            'One, two, four and eight grey fills at three percent opacity, '
            'over an opaque ground or transparency, directly or through a '
            'white alpha mask. Source-over gives alpha 1-(97/100)^n. '
            'The grey is 137 in every channel; an opaque background B gives '
            '137+(B-137)*(97/100)^n. Exact rational PNG pixels check that '
            'intermediate layers do not accumulate eight-bit rounding.',
            ground=False)

modes = [(True, False), (True, True), (False, False), (False, True)]
for row, (opaque, masked) in enumerate(modes):
    for column, count in enumerate([1, 2, 4, 8]):
        x, y = 2 + column * 24, 2 + row * 16
        backdrop = 'opaque' if opaque else 'transparent'
        name = f'{backdrop}-{"mask" if masked else "plain"}-{count}'
        paint = '<rect width="20" height="12" fill="#898989" fill-opacity=".03"/>' * count
        if masked:
            paint = (f'<defs><mask id="m{row}-{count}" mask-type="alpha" '
                     'maskUnits="userSpaceOnUse" x="0" y="0" width="20" height="12">'
                     '<rect width="20" height="12" fill="white"/></mask></defs>'
                     f'<g mask="url(#m{row}-{count})">{paint}</g>')
        if opaque:
            paint = '<rect width="20" height="12" fill="#f6f4f8"/>' + paint
        remaining = F(97, 100) ** count
        rgb = [(137 + (background - 137) * remaining) / 255
               for background in [246, 244, 248]] if opaque else [F(137, 255)] * 3
        rgba = tuple(rgb + [F(1) if opaque else 1 - remaining])
        expected = png([[rgba] * 20] * 12)
        unit.cell(name, (x - 1, y - 1, 22, 14),
                  f'<g transform="translate({x} {y})">{paint}</g>',
                  f'<image x="{x}" y="{y}" width="20" height="12" href="{expected}"/>')

unit.write()
