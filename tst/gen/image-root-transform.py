"""SVG document roots inside images transform in their viewport box.

Each embedded SVG is an independent document root. Literal rectangles
give its quarter-turn results without any transform or viewBox in the
expected drawing. A shifted viewBox and letterboxing leave the pivot at
the border-box center; an explicit zero origin replaces that default.
"""
import base64

from unitgen import Unit


unit = Unit('image-root-transform', 72, 80,
            'The root has a CSS layout box. Its view-box transform reference '
            'box has the used border-box value and its default origin is '
            '50% 50%. Quarter turns retain the viewport center for negative '
            'and positive viewBox minima, while an explicit zero origin '
            'and letterboxing have their literal geometric effects.',
            ground=False)
cases = [
    ('center-quarter', '-8 -4 16 8', '-3 -2', 90, 8, '', (9, 1, 1, 2)),
    ('center-three-quarters', '-8 -4 16 8', '-3 -2', 270, 8, '', (6, 5, 1, 2)),
    ('positive-viewbox', '10 20 16 8', '15 22', 90, 8, '', (9, 1, 1, 2)),
    ('half-turn', '-8 -4 16 8', '-3 -2', 180, 8, '', (9, 5, 2, 1)),
    ('explicit-origin', '-8 -4 16 8', '-3 -2', 90, 8, 'transform-origin="0 0"', None),
    ('letterbox', '-8 -4 16 8', '-3 -2', 90, 12, '', (9, 3, 1, 2)),
    ('letterbox-origin', '-8 -4 16 8', '-3 -2', 90, 12, 'transform-origin="8 2"', (7, 1, 1, 2)),
    ('unequal-axes', '-8 -4 16 8', '-3 -2', 90, 12, '', (9.5, 3, 1.5, 2)),
    ('individual-rotate', '-8 -4 16 8', '-3 -2', 0, 8, 'style="rotate:90deg"', (9, 1, 1, 2)),
    ('root-hidden', '-8 -4 16 8', '7 -2', 90, 8, 'overflow="hidden"', None),
    ('root-visible', '-8 -4 16 8', '7 -2', 90, 8, 'overflow="visible"', (9, 11, 1, 2)),
    ('host-hidden', '-8 -4 16 8', '7 -2', 90, 8, 'overflow="visible"', None),
]
for i, (name, viewbox, position, angle, height, origin, expected) in enumerate(cases):
    x, y = i % 3 * 24, i // 3 * 20
    px, py = position.split()
    svg = (f'<svg xmlns="http://www.w3.org/2000/svg" width="16" height="8" '
           f'viewBox="{viewbox}" transform="rotate({angle})" {origin}>'
           f'<rect x="{px}" y="{py}" width="2" height="1" fill="lime"/></svg>')
    href = 'data:image/svg+xml;base64,' + base64.b64encode(svg.encode()).decode()
    image_attrs = ('preserveAspectRatio="none"' if name == 'unequal-axes' else 'overflow="visible"' if name in ['root-hidden', 'root-visible'] else '')
    test = f'<image {image_attrs} x="{x + 4}" y="{y + 4}" width="16" height="{height}" href="{href}"/>'
    if expected:
        left, top, w, h = expected
        plain = f'<rect x="{x + 4 + left}" y="{y + 4 + top}" width="{w}" height="{h}" fill="lime"/>'
    else:
        plain = ''
    unit.cell(name, (x, y, 24, 20), test, plain)
unit.write()
