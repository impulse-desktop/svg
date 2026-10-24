"""Coincident rectangle edges are compared in the raster's coordinates.

Image fitting and a translated clip can reach the same edge by different
floating-point operations. Literal rational rectangle areas independently
check the resulting coverage, including actual cuts and translucent paint.
"""
import base64
from fractions import Fraction as F

from unitgen import Unit, png


unit = Unit('clip-contained-raster-edges', 96, 80,
            'A containing rectangle keeps coverage once when image fitting '
            'and translated geometry reach the same raster edge. Exact '
            'rectangle areas check image, feImage and clipPath, far origins, '
            'reflections, opacity and genuine cuts.', ground=False)
inner = ('<svg xmlns="http://www.w3.org/2000/svg" width="67.438" height="67.5" '
         'viewBox="0 0 67.438 67.5">'
         '<rect width="67.438" height="67.5" fill="lime"/></svg>')
href = 'data:image/svg+xml;base64,' + base64.b64encode(inner.encode()).decode()
cases = [
    ('image-local', 'image', 0, 0, False, 1, False),
    ('image-far', 'image', '784.781', '772.75', False, 1, False),
    ('image-negative', 'image', '-784.781', '-772.75', False, 1, False),
    ('image-reflected', 'image', '75.375', '862.281', True, 1, False),
    ('feimage-local', 'feImage', 0, 0, False, 1, False),
    ('feimage-far', 'feImage', '784.781', '772.75', False, 1, False),
    ('feimage-negative', 'feImage', '-784.781', '-772.75', False, 1, False),
    ('feimage-reflected', 'feImage', '75.375', '862.281', True, 1, False),
    ('clip-local', 'clip', 0, 0, False, 1, False),
    ('clip-far', 'clip', '784.781', '772.75', False, 1, False),
    ('image-opacity', 'image', '75.375', '862.281', False, '.5', False),
    ('feimage-opacity', 'feImage', '784.781', '772.75', True, '.5', False),
    ('clip-inside', 'clip', 0, 0, False, 1, True),
    ('clip-far-inside', 'clip', '784.781', '772.75', False, '.5', True),
    ('image-inside', 'image', '75.375', '862.281', False, 1, True),
    ('feimage-inside', 'feImage', '784.781', '772.75', False, 1, True),
]


def number(v):
    return format(float(v), '.15g')


for i, (name, mode, ox, oy, reflected, opacity, cut) in enumerate(cases):
    ox, oy, opacity = F(ox), F(oy), F(opacity)
    left, top, width, height = map(F, ['2.281', '2.75', '17.438', '13.657'])
    image_rect = (left, top, left + width, top + height)
    # These cuts cross fully painted source pixels, so their exact area
    # does not depend on composing two independently antialiased edges.
    frame = (left + F('.75'), top + F('.5'), left + width - F('1.25'), top + height - F('1.25')) if cut else image_rect
    x, y = number(ox + left), number(oy + top)
    box = f'x="{x}" y="{y}" width="{number(width)}" height="{number(height)}"'
    ident = 'resource-' + str(i)
    drawing = f'<rect {box} fill="lime"/>'
    if mode == 'image':
        drawing = f'<image {box} preserveAspectRatio="none" href="{href}"/>'
    elif mode == 'feImage':
        drawing = (f'<defs><filter id="{ident}" filterUnits="userSpaceOnUse" {box}>'
                   f'<feImage {box} preserveAspectRatio="none" href="{href}"/>'
                   f'</filter></defs><rect {box} filter="url(#{ident})"/>')
    if mode == 'clip' or cut:
        clip = ident + '-clip'
        clip_box = (f'x="{number(ox + frame[0])}" y="{number(oy + frame[1])}" '
                    f'width="{number(frame[2] - frame[0])}" height="{number(frame[3] - frame[1])}"')
        drawing = (f'<defs><clipPath id="{clip}"><rect {clip_box}/></clipPath></defs>'
                   f'<g clip-path="url(#{clip})">{drawing}</g>')
    drawing = f'<g transform="translate({number(-ox)} {number(-oy)})" opacity="{number(opacity)}">{drawing}</g>'
    if reflected:
        drawing = f'<g transform="translate(24) scale(-1 1)">{drawing}</g>'
        frame = (24 - frame[2], frame[1], 24 - frame[0], frame[3])
    expected = []
    for py in range(20):
        row = []
        for px in range(24):
            area = max(F(0), min(px + 1, frame[2]) - max(px, frame[0])) * max(F(0), min(py + 1, frame[3]) - max(py, frame[1]))
            row.append((0, 1, 0, float(area * opacity)))
        expected.append(row)
    cx, cy = i % 4 * 24, i // 4 * 20
    unit.cell(name, (cx, cy, 24, 20),
              f'<svg x="{cx}" y="{cy}" width="24" height="20">{drawing}</svg>',
              f'<image x="{cx}" y="{cy}" width="24" height="20" href="{png(expected)}"/>')
unit.write()
