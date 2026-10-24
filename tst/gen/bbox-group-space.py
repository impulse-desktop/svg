"""Group object boxes are tight in the group's coordinate space.

Circle/ellipse supports under an affine map and one literal polynomial
curve give the bounds independently. A flood replaces all source pixels,
so exact rectangle/pixel areas alone determine the output references.
"""
import math

from unitgen import Unit, png


u = Unit('bbox-group-space', 64, 64,
         'Object bounding boxes enclose the transformed descendant '
         'geometry tightly in the referencing group coordinate system, '
         'rather than repeatedly mapping local axis-aligned rectangles. '
         'Independent affine circle/ellipse supports, a cubic polynomial '
         'and exact rectangular pixel areas check rotations, shear, '
         'cancelling nested transforms, paint-independent geometry and '
         'coordinate-space controls.', ground=False)
s2 = math.sqrt(2)
circle = '<circle r="2" fill="none"/>'
ellipse = '<ellipse rx="2" ry="1" fill="none"/>'
cases = [
    ('identity', circle, (-2, -2, 2, 2), 1),
    ('rotate-circle', '<g transform="rotate(45)">'+circle+'</g>', (-2, -2, 2, 2), 1),
    ('rotate-ellipse', '<g transform="rotate(45)">'+ellipse+'</g>',
     (-math.sqrt(2.5), -math.sqrt(2.5), math.sqrt(2.5), math.sqrt(2.5)), 1),
    ('shear-circle', '<g transform="matrix(1 0 1 1 0 0)">'+circle+'</g>', (-2*s2, -2, 2*s2, 2), 1),
    ('shear-ellipse', '<g transform="matrix(1 0 1 1 0 0)">'+ellipse+'</g>', (-math.sqrt(5), -1, math.sqrt(5), 1), 1),
    ('cancel-rotations', '<g transform="rotate(-45)"><g transform="rotate(45)">'+circle+'</g></g>', (-2, -2, 2, 2), 1),
    ('cancel-shears', '<g transform="matrix(1 0 -1 1 0 0)"><g transform="matrix(1 0 1 1 0 0)">'+circle+'</g></g>', (-2, -2, 2, 2), 1),
    ('reflection', '<g transform="scale(-1 .75)">'+ellipse+'</g>', (-2, -.75, 2, .75), 1),
    ('quarter-turn', '<g transform="rotate(90)">'+ellipse+'</g>', (-1, -2, 1, 2), 1),
    ('stroke-excluded', '<circle r="2" transform="rotate(45)" fill="none" stroke="red" stroke-width="9"/>', (-2, -2, 2, 2), 1),
    ('opacity-zero', '<circle r="2" transform="rotate(45)" opacity="0"/>', (-2, -2, 2, 2), 1),
    ('visibility-hidden', '<circle r="2" transform="rotate(45)" visibility="hidden"/>', (-2, -2, 2, 2), 1),
    ('display-none', '<circle r="1" fill="none"/><circle r="5" transform="rotate(45)" display="none"/>', (-1, -1, 1, 1), 1),
]
# After rotation, these control points give x(t)=6t(1-t), y(t)=4t-2.
# Thus x runs from0to1.5 and y from-2to2; closing the path adds no extent.
points = [(0, -2), (2, -2/3), (2, 2/3), (0, 2)]
local = [((x+y)/s2, (-x+y)/s2) for x, y in points]
d = 'M'+','.join(format(v, '.17g') for v in local[0])+'C'+' '.join(','.join(format(v, '.17g') for v in q) for q in local[1:])+'Z'
cases += [
    ('rotated-cubic', f'<path d="{d}" transform="rotate(45)" fill="none"/>', (0, -2, 1.5, 2), 1),
    ('multiple-children', '<circle cx="-2" r="1" transform="rotate(45)" fill="none"/><rect x="1" y="0" width="2" height="1" fill="none"/>', (-s2-1, -s2-1, 3, 1), 1),
    ('group-scale', '<g transform="rotate(45)">'+circle+'</g>', (-2, -2, 2, 2), .75),
]
assert len(cases) == 16
for i, (name, source, bounds, scale) in enumerate(cases):
    left, top, right, bottom = [8+v*scale for v in bounds]
    pixels = []
    for py in range(16):
        row = []
        for px in range(16):
            area = (max(0, min(px+1, right)-max(px, left))
                    * max(0, min(py+1, bottom)-max(py, top)))
            row.append((0, 1, 0, area))
        pixels.append(row)
    x, y = i % 4*16, i // 4*16
    u.cell(name, (x, y, 16, 16),
           f'<svg x="{x}" y="{y}" width="16" height="16">'
           f'<defs><filter id="b{i}" x="0" y="0" width="1" height="1">'
           '<feFlood flood-color="lime"/></filter></defs>'
           f'<g transform="translate(8 8) scale({scale})" filter="url(#b{i})">{source}</g></svg>',
           f'<image x="{x}" y="{y}" width="16" height="16" href="{png(pixels)}"/>')
u.write()
