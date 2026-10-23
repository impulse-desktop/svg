"""Zero-area object boxes ignore the filter, retaining the actual stroke.

Expectations are explicit geometry in host coordinates, without a filter
or vector effect. Bands use plain filled rectangles. The two round-cap
controls use untransformed host-space strokes so this filter test does not
compare two independently approximated circle outlines. Small positive
geometry boxes show the flood in their geometric box, excluding the stroke.
"""
from unitgen import Unit


u = Unit('filter-degenerate-nss', 180, 90,
         'Object-bounding-box filter or primitive units on zero-width or '
         'zero-height geometry ignore the filter, including parent groups '
         'and non-scaling round caps. Plain filled host-space bands and '
         'unfiltered host-space round strokes check transforms; positive-box and user-space controls '
         'still apply a green flood. Ordinary tolerance.', ground=False)
blue, green = '#207fc0', '#40bc30'
u.common('<defs><filter id="box" x="0" y="0" width="1" height="1">'
         f'<feFlood flood-color="{green}"/></filter>'
         '<filter id="primitive" filterUnits="userSpaceOnUse" '
         'primitiveUnits="objectBoundingBox" x="0" y="0" width="30" height="30">'
         f'<feFlood flood-color="{green}"/></filter>'
         '<filter id="user" filterUnits="userSpaceOnUse" '
         'x="0" y="0" width="30" height="30">'
         f'<feFlood flood-color="{green}"/></filter></defs>', '')

cases = [
    ('horizontal-group', (11, 15), (19, 15), 'group', 'butt', 1, 1, False, 'box'),
    ('vertical-group', (15, 11), (15, 19), 'group', 'butt', 1, 1, False, 'box'),
    ('horizontal-direct', (11, 15), (19, 15), 'direct', 'butt', 1, 1, False, 'box'),
    ('vertical-direct', (15, 11), (15, 19), 'direct', 'butt', 1, 1, False, 'box'),
    ('tiny-round-cap', (15, 15), (15, 15.008), 'group', 'round', 1, 1, False, 'box'),
    ('zero-round-cap', (15, 15), (15, 15), 'group', 'round', 1, 1, False, 'box'),
    ('quarter-scale', (11, 15), (19, 15), 'group', 'butt', .25, .25, False, 'box'),
    ('double-scale', (15, 11), (15, 19), 'group', 'butt', 2, 2, False, 'box'),
    ('unequal-scale', (11, 15), (19, 15), 'group', 'butt', 2, .5, False, 'box'),
    ('quarter-turn', (11, 15), (19, 15), 'group', 'butt', 1, 1, True, 'box'),
    ('primitive-box', (11, 15), (19, 15), 'group', 'butt', 1, 1, False, 'primitive'),
    ('user-space-active', (11, 15), (19, 15), 'group', 'butt', 1, 1, False, 'user'),
    ('positive-small-height', (11, 15), (19, 15.0001), 'group', 'butt', 1, 1, False, 'box'),
    ('positive-small-width', (15, 11), (15.0001, 19), 'group', 'butt', 1, 1, False, 'box'),
    ('positive-small-both', (15, 15), (15.0001, 15.0001), 'group', 'butt', 1, 1, False, 'box'),
    ('move-only', (15, 15), None, 'group', 'round', 1, 1, False, 'box'),
    ('no-stroke', (11, 15), (19, 15), 'group', 'butt', 1, 1, False, 'box'),
    ('positive-subpaths-box', (11, 11), (19, 19), 'group', 'butt', 1, 1, False, 'box'),
]


def rectangle(x, y, width, height, colour):
    return (f'<rect x="{x}" y="{y}" width="{width}" height="{height}" '
            f'fill="{colour}"/>')


for i, (name, p, q, target, cap, sx, sy, turn, server) in enumerate(cases):
    x, y = i % 6 * 30, i // 6 * 30
    d = f'M{p[0]} {p[1]}' + (f'L{q[0]} {q[1]}' if q is not None else '')
    if name == 'positive-subpaths-box':d = 'M11 11H19 M19 11V19'
    stroke = 'none' if name == 'no-stroke' else blue
    direct = f' filter="url(#{server})"' if target == 'direct' else ''
    shape = (f'<path d="{d}" fill="none" stroke="{stroke}" '
             f'stroke-width="4" stroke-linecap="{cap}" '
             f'vector-effect="non-scaling-stroke"{direct}/>')
    if target == 'group':shape = f'<g filter="url(#{server})">{shape}</g>'
    transform = f'translate(15 15) scale({sx} {sy}) '
    if turn:transform += 'rotate(90) '
    transform += 'translate(-15 -15)'
    test = f'<g transform="translate({x} {y})"><g transform="{transform}">{shape}</g></g>'

    def point(v):
        xx, yy = v[0]-15, v[1]-15
        if turn:xx, yy = -yy, xx
        return x+15+sx*xx, y+15+sy*yy

    if name in ['move-only', 'no-stroke']:
        expected = ''
    elif server == 'user':
        expected = rectangle(x, y, 30, 30, green)
    elif name.startswith('positive-'):
        expected = rectangle(x+min(p[0], q[0]), y+min(p[1], q[1]),
                             abs(q[0]-p[0]), abs(q[1]-p[1]), green)
    else:
        a, b = point(p), point(q)
        if cap == 'round':expected = (f'<path d="M{a[0]} {a[1]}L{b[0]} {b[1]}" '
                                     f'fill="none" stroke="{blue}" stroke-width="4" '
                                     'stroke-linecap="round"/>')
        elif a[1] == b[1]:expected = rectangle(min(a[0], b[0]), a[1]-2, abs(a[0]-b[0]), 4, blue)
        else:expected = rectangle(a[0]-2, min(a[1], b[1]), 4, abs(a[1]-b[1]), blue)
    u.cell(name, (x, y, 30, 30), test, expected)

u.write()
