"""Pixel coverage of axis-aligned bands is their intersection length.

PNG controls contain the analytic alpha values, with no SVG geometry or
stroke processing shared with the unit. Viewport controls use the exact
meet scale and centered letterbox of an extremely thin document.
"""
from unitgen import Unit, png

cases = [
    ('half-pixel', 'h', .75, .5, '#000000'),
    ('half-gray', 'h', .75, .5, '#636366'),
    ('whole-row', 'h', .5, 1, '#000000'),
    ('quarter-shift', 'h', 1.25, 1, '#000000'),
    ('fractional-width', 'h', 1.59538, 1.31163, '#bbbbbb'),
    ('fractional-center', 'h', 1.24609, 1, '#fff8f3'),
    ('wide-band', 'h', 2, 2.85, '#d976ff'),
    ('two-row-band', 'h', 1.18066, 2, '#40a9ff'),
    ('whole-column', 'v', .5, 1, '#dce3e6'),
    ('half-column', 'v', .75, .5, '#000000'),
    ('three-columns', 'v', 1.5, 3, '#000000'),
    ('fractional-column', 'v', 1.59538, 1.31163, '#bbbbbb'),
    ('fractional-rect', 'r', .8125, 1.1875, '#00d6c2'),
    ('partial-row', 'r', 0, .6, '#ffffff'),
    ('viewport-20', 'svg', 20, 0, '#e1eae7'),
    ('viewport-80', 'svg', 80, 0, '#ccd814'),
]
unit = Unit('raster-area-bands', 64, 16,
            'Default antialiasing preserves the area of horizontal and vertical bands, including fractional widths and positions. Expected alpha is the exact covered fraction of each pixel. Thin SVG viewports retain their meet scale and letterbox.', ground=False)
for i, (name, kind, center, width, color) in enumerate(cases):
    x, y = i % 4 * 16, i // 4 * 4
    low, high = center - width / 2, center + width / 2
    if kind == 'h':
        test = f'<path d="M0 {center}H16" fill="none" stroke="{color}" stroke-width="{width}"/>'
    elif kind == 'v':
        test = f'<path d="M{center} 0V4" fill="none" stroke="{color}" stroke-width="{width}"/>'
    elif kind == 'r':
        low, high = center, center + width
        test = f'<rect y="{center}" width="16" height="{width}" fill="{color}"/>'
    else:
        height = 16 / center
        low, high = (1 - height) / 2, (1 + height) / 2
        test = f'<svg width="16" height="1" viewBox="0 0 {center} 1"><rect width="{center}" height="1" fill="{color}"/></svg>'
    rgb = tuple(int(color[j:j + 2], 16) / 255 for j in (1, 3, 5))
    pixels = []
    for py in range(4):
        row = []
        for px in range(16):
            at = px if kind == 'v' else py
            area = max(0, min(at + 1, high) - max(at, low))
            row.append((*rgb, area))
        pixels.append(row)
    unit.cell(name, (x, y, 16, 4),
              f'<g transform="translate({x} {y})">{test}</g>',
              f'<image x="{x}" y="{y}" width="16" height="4" href="{png(pixels)}"/>')
unit.write()
