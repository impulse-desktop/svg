"""A rectangular clip containing the complete drawing has no effect.

Expected drawings are literal rectangles and rectangular differences. The
fractional edges keep their single area coverage; clipping that actually
removes paint, holes, and nonrectangular clips remain effective.
"""
import base64

from unitgen import Unit


unit = Unit('clip-contained-rectangle', 96, 96,
            'A rectangular clip containing all painted geometry is an identity, '
            'including a coincident fractional edge. Preserve its area once '
            'for groups, nested SVG images and feImage. Real cuts, holes, '
            'nonrectangular clips and group opacity retain their effects.',
            ground=False)
paint = '<rect x="2" y="2" width="20.5" height="14.5" fill="lime"/>'
inner = ('<svg xmlns="http://www.w3.org/2000/svg" width="20.5" height="14.5" '
         'viewBox="0 0 20.5 14.5"><rect width="20.5" height="14.5" fill="lime"/></svg>')
href = 'data:image/svg+xml;base64,' + base64.b64encode(inner.encode()).decode()
cases = [
    ('same', '<rect x="2" y="2" width="20.5" height="14.5"/>', paint, paint, ''),
    ('contains', '<rect x="1" y="1" width="23" height="17"/>', paint, paint, ''),
    ('touches-bottom', '<rect x="1" y="1" width="23" height="15.5"/>', paint, paint, ''),
    ('cuts', '<rect x="2" y="2" width="20.5" height="14.5"/>',
     '<rect width="32" height="24" fill="lime"/>', paint, ''),
    ('hole', '<path clip-rule="evenodd" d="M1 1H24V18H1Z M8 6H12V10H8Z"/>',
     '<rect x="2" y="2" width="20" height="14" fill="lime"/>',
     '<path fill="lime" d="M2 2H22V6H2Z M2 10H22V16H2Z M2 6H8V10H2Z M12 6H22V10H12Z"/>', ''),
    ('triangle', '<path d="M0 0H16L0 16Z"/>',
     '<rect x="8" y="8" width="4" height="4" fill="lime"/>', '', ''),
    ('horizontal-retrace', '<path d="M0 0H32H4V4H0Z"/>',
     '<rect x="8" width="4" height="4" fill="lime"/>', '', ''),
    ('opacity', '<rect x="2" y="2" width="20.5" height="14.5"/>',
     paint, '<rect x="2" y="2" width="20.5" height="14.5" fill="lime" opacity=".5"/>', 'opacity=".5"'),
]
for i, (name, clip, drawing, expected, attributes) in enumerate(cases):
    x, y = i % 3 * 32, i // 3 * 24
    ident = 'clip-' + name
    content = (f'<defs><clipPath id="{ident}">{clip}</clipPath></defs>'
               f'<g clip-path="url(#{ident})" {attributes}>{drawing}</g>')
    unit.cell(name, (x, y, 32, 24),
              f'<svg x="{x}" y="{y}" width="32" height="24">{content}</svg>',
              f'<g transform="translate({x} {y})">{expected}</g>')
for i, mode in enumerate(['image', 'feImage'], len(cases)):
    x, y = i % 3 * 32, i // 3 * 24
    if mode == 'image':
        drawing = f'<image x="2" y="2" width="20.5" height="14.5" href="{href}"/>'
    else:
        drawing = (f'<defs><filter id="image-filter" filterUnits="userSpaceOnUse" '
                   'x="2" y="2" width="20.5" height="14.5">'
                   f'<feImage x="2" y="2" width="20.5" height="14.5" href="{href}"/>'
                   '</filter></defs><rect x="2" y="2" width="20.5" height="14.5" filter="url(#image-filter)"/>')
    unit.cell(mode, (x, y, 32, 24),
              f'<svg x="{x}" y="{y}" width="32" height="24">{drawing}</svg>',
              f'<g transform="translate({x} {y})">{paint}</g>')
unit.write()
