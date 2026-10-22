"""Outline offsets against literal rectangular strips, not another outline.

All subjects have border box (12,12)-(36,28) and a 3px outline. Positive
offsets leave a gap; negative ones put ink inside the box. With a large
negative offset the outer outline stays at least 6px in either dimension,
independently. The coordinates below are fixed geometric expectations.
"""
from unitgen import Unit

RINGS = {
    'zero': [(9, 9, 30, 3), (9, 28, 30, 3), (9, 12, 3, 16), (36, 12, 3, 16)],
    'positive': [(5, 5, 38, 3), (5, 32, 38, 3), (5, 8, 3, 24), (40, 8, 3, 24)],
    'negative': [(11, 11, 26, 3), (11, 26, 26, 3), (11, 14, 3, 12), (34, 14, 3, 12)],
    'inside': [(12, 12, 24, 3), (12, 25, 24, 3), (12, 15, 3, 10), (33, 15, 3, 10)],
    'collapsed-y': [(17, 17, 14, 6)],
    'collapsed-both': [(21, 17, 6, 6)],
    'auto-gap': [(7, 7, 34, 3), (7, 30, 34, 3), (7, 10, 3, 20), (38, 10, 3, 20)],
    'wide-zero': [(7, 7, 34, 5), (7, 28, 34, 5), (7, 12, 5, 16), (36, 12, 5, 16)],
}

# name, declarations after the ordinary outline, expected ring, parent,
# extra attributes, optional complete outline declaration, expected color
CASES = [
    ('positive', 'outline-offset:4px', 'positive', '', '', '', 'red'),
    ('zero', 'outline-offset:0', 'zero', '', '', '', 'red'),
    ('negative', 'outline-offset:-2px', 'negative', '', '', '', 'red'),
    ('inside', 'outline-offset:-3px', 'inside', '', '', '', 'red'),
    ('collapse-one-axis', 'outline-offset:-8px', 'collapsed-y', '', '', '', 'red'),
    ('collapse-both-axes', 'outline-offset:-100px', 'collapsed-both', '', '', '', 'red'),
    ('not-inherited', '', 'zero', 'outline-offset:4px', '', '', 'red'),
    ('inherit', 'outline-offset:inherit', 'positive', 'outline-offset:4px', '', '', 'red'),
    ('initial', 'outline-offset:initial', 'zero', 'outline-offset:4px', '', '', 'red'),
    ('unset', 'outline-offset:unset', 'zero', 'outline-offset:4px', '', '', 'red'),
    ('shorthand-preserves-offset', '', 'positive', '', '', 'outline-offset:4px;outline:3px solid red', 'red'),
    ('invalid-percent', 'outline-offset:4px;outline-offset:50%', 'positive', '', '', '', 'red'),
    ('invalid-number', 'outline-offset:4px;outline-offset:2', 'positive', '', '', '', 'red'),
    ('font-relative', 'font-size:16px;outline-offset:.25em', 'positive', '', '', '', 'red'),
    ('variable', '--distance:-2px;outline-offset:var(--distance)', 'negative', '', '', '', 'red'),
    ('invalid-variable', '--distance:20%;outline-offset:4px;outline-offset:var(--distance)', 'zero', '', '', '', 'red'),
    ('not-presentation-attribute', '', 'zero', '', 'outline-offset="4px"', '', 'red'),
    ('auto', 'color:blue;outline-offset:2px', 'auto-gap', '', '', 'outline:auto', 'blue'),
    ('auto-ignores-width', 'outline-offset:2px', 'auto-gap', '', '', 'outline:20px auto red', 'red'),
    ('auto-inside', 'color:blue;outline-offset:-2px', 'negative', '', '', 'outline:auto', 'blue'),
    ('auto-color-after-solid', 'color:blue', 'wide-zero', '', '', 'outline:5px solid auto', 'blue'),
    ('auto-color-before-solid', 'color:blue', 'wide-zero', '', '', 'outline:auto 5px solid', 'blue'),
    ('calc', 'outline-offset:calc(1px + 3px)', 'positive', '', '', '', 'red'),
    ('translucent-inside', 'outline-offset:-2px', 'negative', '', '', 'outline:3px solid rgba(27,31,35,.06)', 'rgba(27,31,35,.06)'),
]

u = Unit('outline-offset', 320, 240,
         'Positive and negative outline offsets are compared with fixed solid rectangular strips. A negative offset clamps independently per axis so the outer shape remains at least twice the outline width. Cascading, invalid values, variables and font-relative units are checked separately. The outline shorthand preserves offset. Auto uses the permitted solid rendering with a fixed 3px width and current color; its specified width is ignored.')
for i, (name, after, expected, parent, attributes, outline, color) in enumerate(CASES):
    x, y = i % 5 * 64, i // 5 * 48
    style = (outline or 'outline:3px solid red') + ';' + after
    test = f'<g transform="translate({x} {y})"><g style="{parent}"><rect x="12" y="12" width="24" height="16" fill="lime" style="{style}" {attributes}/></g></g>'
    strips = ''.join(f'<rect x="{a}" y="{b}" width="{w}" height="{h}" fill="{color}"/>' for a, b, w, h in RINGS[expected])
    plain = f'<g transform="translate({x} {y})"><rect x="12" y="12" width="24" height="16" fill="lime"/>{strips}</g>'
    u.cell(name, (x, y, 64, 48), test, plain)
u.write()
