"""A nested SVG's viewport still clips its content when it is filtered."""
from unitgen import Unit

u = Unit('filter-nested-viewport', 128, 32,
         'A nested SVG clips to its viewport after viewBox placement, including when the SVG itself has a filter. Overflow visible is an explicit contrasting case. Literal circles and clip rectangles define the expected geometry.')
u.common('<defs><filter id="tiny"><feGaussianBlur stdDeviation=".001"/></filter></defs>', '')
normal = '<circle cx="4" cy="4" r="3" fill="red"/>'
plain = '<circle cx="8" cy="8" r="3" fill="red"/>'
cases = [
    ('filtered-shift', 'viewBox="-8 0 8 8" filter="url(#tiny)"', normal, ''),
    ('plain-shift', 'viewBox="-8 0 8 8"', normal, ''),
    ('visible-overflow', 'viewBox="-8 0 8 8" overflow="visible" filter="url(#tiny)"', normal, '<circle cx="16" cy="8" r="3" fill="red"/>'),
    ('inside', 'viewBox="0 0 8 8" filter="url(#tiny)"', normal, plain),
    ('shifted-inside', 'viewBox="8 0 8 8" filter="url(#tiny)"', '<circle cx="12" cy="4" r="3" fill="red"/>', plain),
    ('scaled', 'viewBox="0 0 16 16" filter="url(#tiny)"', '<circle cx="8" cy="8" r="6" fill="red"/>', plain),
    ('partial-clip', 'viewBox="-4 0 8 8" filter="url(#tiny)"', normal, '<defs><clipPath id="edge"><rect x="4" y="4" width="8" height="8"/></clipPath></defs><circle cx="12" cy="8" r="3" fill="red" clip-path="url(#edge)"/>'),
    ('filtered-child', 'viewBox="-8 0 8 8"', f'<g filter="url(#tiny)">{normal}</g>', ''),
]
for i, (name, attributes, content, expected) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 16
    ground = '<rect width="32" height="16" fill="lime"/>'
    u.cell(name, (x, y, 32, 16),
           f'<g transform="translate({x} {y})">{ground}<svg x="4" y="4" width="8" height="8" {attributes}>{content}</svg></g>',
           f'<g transform="translate({x} {y})">{ground}{expected}</g>')
u.write()
