"""Short butt-capped dashes have the area of their offset rectangles.

A period of one pixel gives every pixel the same covered fraction,
regardless of the axis, phase or traversal direction. The expected drawing
uses solid squares with that exact area as alpha, without dashed strokes.
"""
from unitgen import Unit


u = Unit('stroke-short-wide-dashes', 128, 48,
         'Each eight-pixel square is covered by one short, wide dash per '
         'pixel. Its area is the dash length times the stroke width. '
         'Rotating, reversing or changing phase preserves pixel coverage. '
         'Crossing horizontal and vertical dashes cover their union.')
cases = [
    ('horizontal', 'M0 4H8', .03125, 0),
    ('vertical', 'M4 0V8', .03125, 0),
    ('horizontal-phase', 'M0 4H8', .03125, .5),
    ('vertical-phase', 'M4 0V8', .03125, .5),
    ('horizontal-negative', 'M0 4H8', .03125, -.25),
    ('vertical-negative', 'M4 0V8', .03125, -.25),
    ('horizontal-decimal', 'M0 4H8', .03, 0),
    ('vertical-decimal', 'M4 0V8', .03, 0),
    ('horizontal-reverse', 'M8 4H0', .03125, 0),
    ('vertical-reverse', 'M4 8V0', .03125, 0),
    ('crossing', 'M0 4H8M4 0V8', .03125, 0),
    ('no-gap', 'M0 4H8', 1, 0),
]
for i, (name, path, length, offset) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 16
    area = 2 * length - length * length if name == 'crossing' else length
    transform = f'translate({x + 2} {y + 2})'
    u.cell(name, (x, y, 32, 16),
           f'<g transform="{transform}"><path d="{path}" fill="none" '
           f'stroke="black" stroke-width="8" stroke-dasharray="{length} {1 - length}" '
           f'stroke-dashoffset="{offset}"/></g>',
           f'<rect x="{x + 2}" y="{y + 2}" width="8" height="8" '
           f'fill="black" fill-opacity="{area}"/>')
u.write()
