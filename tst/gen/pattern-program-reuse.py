"""Finite pattern graphs keep their complete drawing and caller context.

Plain rectangles independently describe every cell. The final graph has
six levels of ten repeated references, followed by an opaque lime square;
compiling it as a tree exhausts the work budget before that last square.
The preceding cells distinguish opacity, reflection, tile size, viewBox,
bbox content units, use/marker context paints and invalid references.
"""
from unitgen import Unit


u = Unit('pattern-program-reuse', 128, 64,
         'Each pattern matches independently placed colored rectangles. '
         'Repeated references retain opacity, transforms, reference boxes, '
         'use and marker paints, and fallback. The last finite graph ends '
         'in an opaque lime square; none of its earlier red paint remains.',
         ground=False)
definitions = [
    '<pattern id="half" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="2" height="4" fill="lime"/></pattern>',
    '<pattern id="solid" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="4" height="4" fill="lime"/></pattern>',
    '<pattern id="template" patternUnits="userSpaceOnUse" width="4" height="4" viewBox="0 0 4 4"><rect width="2" height="4" fill="lime"/></pattern>',
    '<pattern id="shifted" href="#template" viewBox="1 0 4 4"/>',
    '<pattern id="boxed" patternUnits="userSpaceOnUse" patternContentUnits="objectBoundingBox" width="4" height="4"><rect width=".5" height="1" fill="lime"/></pattern>',
    '<pattern id="cf" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="4" height="4" fill="context-fill"/></pattern>',
    '<pattern id="cs" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="4" height="4" fill="context-stroke"/></pattern>',
    '<g id="use-fill"><rect width="4" height="4" fill="url(#cf)" stroke-width="0"/></g>',
    '<g id="use-stroke"><rect width="4" height="4" fill="url(#cs)" stroke-width="0"/></g>',
    '<pattern id="empty" patternUnits="userSpaceOnUse" width="0" height="4"><rect width="4" height="4" fill="red"/></pattern>',
    '<pattern id="cycle" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="4" height="4" fill="url(#cycle) lime"/></pattern>',
]
# At the minimum tile size each rectangular stroke covers a known fraction
# of the only pixel. Its mitered outline is an outer rectangle minus an
# inner rectangle. Source-over of these exact areas defines its color.
palette = [(0, 128, 0), (165, 42, 42), (255, 192, 203), (128, 128, 128), (0, 255, 255)] * 2
leaf = []
pixel = [0., 0., 0., 0.]


def clipped_area(x0, y0, x1, y1):
    return max(0, min(20, x1) - max(0, x0)) * max(0, min(20, y1) - max(0, y0))


for x, color in enumerate(palette):
    y = min(x, 3)
    alpha = (clipped_area(x - .5, y - .5, x + 20.5, y + 20.5)
             - clipped_area(x + .5, y + .5, x + 19.5, y + 19.5)) / 400
    for c in range(3):
        pixel[c] = color[c] / 255 * alpha + pixel[c] * (1 - alpha)
    pixel[3] = alpha + pixel[3] * (1 - alpha)
    leaf.append(f'<rect x="{x}" y="{y}" width="20" height="20" fill="none" '
                f'stroke="rgb({color[0]},{color[1]},{color[2]})"/>')
definitions.append('<pattern id="leaf" patternUnits="userSpaceOnUse" width="20" height="20">'
                   + ''.join(leaf) + '</pattern>')
leaf_color = 'rgb(%d,%d,%d)' % tuple(round(c / pixel[3] * 255) for c in pixel[:3])
for name, pattern in [('marker-fill', 'cf'), ('marker-stroke', 'cs')]:
    definitions.append(f'<marker id="{name}" markerUnits="userSpaceOnUse" '
                       'markerWidth="4" markerHeight="4" refX="0" refY="2" '
                       'viewBox="0 0 4 4" orient="0">'
                       f'<rect width="4" height="4" fill="url(#{pattern})"/></marker>')
definitions.append('<pattern id="d0" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="4" height="4" fill="red"/></pattern>')
for level in range(1, 7):
    definitions.append(f'<pattern id="d{level}" patternUnits="userSpaceOnUse" width="4" height="4">'
                       + f'<rect width="4" height="4" fill="url(#d{level - 1})"/>' * 10
                       + '</pattern>')
definitions.append('<pattern id="last" patternUnits="userSpaceOnUse" width="4" height="4">'
                   '<rect width="4" height="4" fill="url(#d6)"/>'
                   '<rect width="4" height="4" fill="lime"/></pattern>')
# Many distinct finite servers, followed by repeated early references,
# must preserve each server's paint as the program index grows.
indexed_colors = [f'#{i * 5:02x}{(i * 29 + 17) % 256:02x}{(i * 53 + 23) % 256:02x}'
                  for i in range(40)]
for i in range(40):
    definitions.append(f'<pattern id="indexed-{i}" patternUnits="userSpaceOnUse" '
                       'width="1" height="1">'
                       f'<rect width="1" height="1" fill="{indexed_colors[i]}"/></pattern>')
u.common('<defs>' + ''.join(definitions) + '</defs>', '')


def rect(x, y, w, h, color='lime', opacity=1):
    return (f'<rect x="{x}" y="{y}" width="{w}" height="{h}" '
            f'fill="{color}" fill-opacity="{opacity}"/>')


def strips(values):
    return ''.join(rect(x, 0, w, 4) for x, w in values)


def markers(name):
    return (f'<path d="M0 2H4" fill="lime" stroke="lime" stroke-width="4" marker-end="url(#{name})"/>'
            f'<path d="M12 2H16" fill="blue" stroke="blue" stroke-width="4" marker-end="url(#{name})"/>')


cases = [
    ('opacity', '<rect width="4" height="4" fill="url(#solid)" fill-opacity=".25"/>'
     '<rect x="8" width="4" height="4" fill="url(#solid)" fill-opacity=".75"/>',
     rect(0, 0, 4, 4, opacity=.25) + rect(8, 0, 4, 4, opacity=.75)),
    ('reflected', '<rect width="8" height="4" fill="url(#half)"/>'
     '<g transform="translate(20) scale(-1 1)"><rect width="8" height="4" fill="url(#half)"/></g>',
     strips([(0, 2), (4, 2), (14, 2), (18, 2)])),
    ('tile-scale', '<rect width="8" height="4" fill="url(#half)"/>'
     '<g transform="translate(12) scale(2 1)"><rect width="8" height="4" fill="url(#half)"/></g>',
     strips([(0, 2), (4, 2), (12, 4), (20, 4)])),
    ('template-viewbox', '<rect width="8" height="4" fill="url(#template)"/>'
     '<rect x="12" width="8" height="4" fill="url(#shifted)"/>',
     strips([(0, 2), (4, 2), (12, 1), (16, 1)])),
    ('content-box', '<rect width="4" height="4" fill="url(#boxed)"/>'
     '<rect x="12" width="6" height="4" fill="url(#boxed)"/>',
     strips([(0, 2), (12, 3), (16, 2)])),
    ('use-fill', '<use href="#use-fill" fill="lime"/>'
     '<use href="#use-fill" x="8" fill="blue"/>',
     rect(0, 0, 4, 4) + rect(8, 0, 4, 4, 'blue')),
    ('use-stroke', '<use href="#use-stroke" stroke="lime"/>'
     '<use href="#use-stroke" x="8" stroke="blue"/>',
     rect(0, 0, 4, 4) + rect(8, 0, 4, 4, 'blue')),
    ('marker-fill', markers('marker-fill'), rect(0, 0, 8, 4) + rect(12, 0, 8, 4, 'blue')),
    ('marker-stroke', markers('marker-stroke'), rect(0, 0, 8, 4) + rect(12, 0, 8, 4, 'blue')),
    ('empty-fallback', '<rect width="4" height="4" fill="url(#empty) lime"/>', rect(0, 0, 4, 4)),
    ('cycle-fallback', '<rect width="4" height="4" fill="url(#cycle)"/>', rect(0, 0, 4, 4)),
    ('minimum-tile', '<svg width="1" height="1" viewBox="0 0 20 20">'
     '<rect width="20" height="20" fill="url(#leaf)"/></svg>',
     rect(0, 0, 1, 1, leaf_color, pixel[3])),
    ('branching-late-fill', '<rect width="8" height="8" fill="url(#last)"/>', rect(0, 0, 8, 8)),
    ('many-servers',
     ''.join(rect(i % 8 * 3, i // 8 * 2, 2, 1, f'url(#indexed-{server})')
             for i, server in enumerate([*range(40), *range(16)])),
     ''.join(rect(i % 8 * 3, i // 8 * 2, 2, 1, indexed_colors[server])
             for i, server in enumerate([*range(40), *range(16)]))),
]
for i, (name, test, expected) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 16
    u.cell(name, (x, y, 32, 16), f'<g transform="translate({x + 2} {y + 2})">{test}</g>',
           f'<g transform="translate({x + 2} {y + 2})">{expected}</g>')
u.write()
