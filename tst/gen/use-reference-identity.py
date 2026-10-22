"""References resolve to elements, not to an ancestor's matching ID string.

The first matching element in document order remains the target when a
later group or use repeats its ID. The real files include a symbol chain,
masked paths and filtered paths inside such groups. Unique-ID controls
draw the same square. Actual ancestor and indirect cycles must still be
dropped. Expectations are independent, integer-aligned lime rectangles.
"""
from unitgen import Unit


u = Unit('use-reference-identity', 50, 20,
         'Each cell is a lime square. A later repeated ID does not replace '
         'the first target or turn an acyclic reference into a cycle. '
         'Real ancestor and indirect cycles are omitted; their valid '
         'siblings remain. All expectations are plain rectangles.')
square = '<rect width="6" height="6"/>'
red = '<rect width="6" height="6" fill="red"/>'
cases = [
    ('late-ancestor',
     '<defs><rect id="a0" width="6" height="6"/></defs>'
     f'<g id="a0">{red}<use href="#a0" fill="lime"/></g>'),
    ('nested-symbol',
     f'<defs><symbol id="a1">{square}</symbol>'
     '<symbol id="b1"><use href="#a1"/></symbol></defs>'
     f'<g id="a1">{red}<use href="#b1" fill="lime"/></g>'),
    ('late-use',
     '<defs><rect id="a2" width="6" height="6"/></defs>'
     f'{red}<use id="a2" href="#a2" fill="lime"/>'),
    ('mask-reference',
     '<defs><path id="a3" d="M0 0H6V6H0Z"/>'
     '<mask id="m3" maskUnits="userSpaceOnUse" x="0" y="0" width="6" height="6">'
     '<use href="#a3" fill="white"/></mask></defs>'
     f'<g id="a3">{red}<use href="#a3" fill="lime" mask="url(#m3)"/></g>'),
    ('filtered-reference',
     '<defs><path id="a4" d="M0 0H6V6H0Z"/>'
     '<filter id="f4" x="0" y="0" width="1" height="1">'
     '<feOffset dx="0" dy="0"/></filter></defs>'
     f'<g id="a4">{red}<use href="#a4" fill="lime" filter="url(#f4)"/></g>'),
    ('transformed-symbol',
     '<defs><symbol id="a5"><rect width="2" height="3"/></symbol>'
     '<symbol id="b5"><use href="#a5" transform="scale(3 2)"/></symbol></defs>'
     f'<g id="a5">{red}<use href="#b5" fill="lime"/></g>'),
    ('unique-control',
     f'<defs><symbol id="a6">{square}</symbol>'
     '<symbol id="b6"><use href="#a6"/></symbol></defs>'
     f'<g id="unique6">{red}<use href="#b6" fill="lime"/></g>'),
    ('first-not-last',
     '<defs><rect id="a7" width="6" height="6" fill="lime"/>'
     '<rect id="a7" width="6" height="6" fill="red"/></defs>'
     '<use href="#a7"/>'),
    ('actual-ancestor-cycle',
     f'<g fill="lime"><g id="a8">{square}'
     '<use href="#a8" fill="red"/></g></g>'),
    ('actual-indirect-cycle',
     f'<defs><g id="a9">{square}<use href="#b9" fill="red"/></g>'
     '<g id="b9"><use href="#a9"/></g></defs>'
     '<use href="#a9" fill="lime"/>'),
]
for i, (name, test) in enumerate(cases):
    x, y = i % 5 * 10, i // 5 * 10
    u.cell(name, (x, y, 10, 10),
           f'<g transform="translate({x + 2} {y + 2})">{test}</g>',
           f'<rect x="{x + 2}" y="{y + 2}" width="6" height="6" fill="lime"/>')
u.write()
