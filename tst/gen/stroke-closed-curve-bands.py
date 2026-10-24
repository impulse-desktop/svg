"""Regular curved bands survive a closed path's folded offset outline.

A small reversed bend closes a much larger path. Each probe rectangle
belongs to the gently curved side: the normal equation changes sign for
all four corners, its tangent never vanishes, and its restricted control
polygon stays within the half width. This proves complete coverage
without deriving an expectation from a renderer or its offset outline.
The established stroke-wider-than-shape unit retains D49's tight circles.
"""
import math

from unitgen import Unit


SEGMENTS = [
    [(0, 0), (-.03, -.04), (-.02, -.04), (.04, 0)],
    [(.04, 0), (-14, -7)],
    [(-14, -7), (0, 16)],
    [(0, 16), (.15, 10), (-.15, 5), (0, .1)],
    [(0, .1), (0, 0)],
]


def mix(a, b, t):
    return tuple((1 - t) * x + t * y for x, y in zip(a, b))


def split(c, t):
    a, b, d, e = c
    ab, bd, de = mix(a, b, t), mix(b, d, t), mix(d, e, t)
    abd, bde = mix(ab, bd, t), mix(bd, de, t)
    p = mix(abd, bde, t)
    return [a, ab, abd, p], [p, bde, de, e]


def transform(p, angle, reflected, inverse=False):
    c, s = math.cos(math.radians(angle)), math.sin(math.radians(angle))
    x, y = p
    if inverse:
        x, y = x * c + y * s, -x * s + y * c
        return (-x if reflected else x, y)
    x = -x if reflected else x
    return x * c - y * s, x * s + y * c


def certify(box, angle=0, reflected=False):
    corners = [transform((x, y), angle, reflected, True)
               for x in (box[0], box[0] + box[2])
               for y in (box[1], box[1] + box[3])]
    a, b = .94, .99
    q = split(split(SEGMENTS[3], b)[0], a / b)[1]
    v = [[3 * (q[j + 1][k] - q[j][k]) for k in range(2)]
         for j in range(3)]
    assert max(d[1] for d in v) < 0
    before = [sum((q[0][k] - p[k]) * v[0][k] for k in range(2))
              for p in corners]
    after = [sum((q[-1][k] - p[k]) * v[-1][k] for k in range(2))
             for p in corners]
    assert max(before) < 0 and min(after) > 0
    # A fixed margin also covers the renderer's existing D1 flattening
    # target after each cell's viewport transform.
    assert max(math.dist(p, c) for p in q for c in corners) < .95


def path_data(reverse=False, split_at=None, split_bend=False, start=0,
              angle=0, reflected=False):
    pieces = [p[:] for p in SEGMENTS]
    if split_at is not None:
        pieces[3:4] = split(pieces[3], split_at)
    if split_bend:
        pieces[:1] = split(pieces[0], .5)
    pieces = pieces[start:] + pieces[:start]
    if reverse:
        pieces = [list(reversed(p)) for p in reversed(pieces)]

    def point(p):
        return ' '.join(f'{v:.15g}' for v in transform(p, angle, reflected))

    return ('M' + point(pieces[0][0])
            + ''.join(('C' if len(p) == 4 else 'L')
                      + ' '.join(point(q) for q in p[1:]) for p in pieces)
            + 'Z')


variants = [
    ('curved-band', {}),
    ('reversed', {'reverse': True}),
    ('curve-split-half', {'split_at': .5}),
    ('curve-split-unequal', {'split_at': 1 / 1024}),
    ('bend-split', {'split_bend': True}),
    ('changed-start', {'start': 2}),
    ('rotated', {'angle': 37}),
    ('reflected', {'reflected': True}),
    ('opposite-subpaths', {'opposite': True, 'alpha': .5}),
    ('many-subpaths', {'copies': 64, 'alpha': .5}),
    ('crisp', {'crisp': True}),
    ('non-scaling', {'non_scaling': True}),
]
u = Unit('stroke-closed-curve-bands', 128, 96,
         'Every full lime tile lies inside a regular curved stroke band. '
         'A sign change of the normal equation and a convex-hull distance '
         'bound certify coverage. A folded part of the closed outline '
         'cannot cancel that band. Direction, exact subdivision, start, '
         'rotation, reflection, transparent coincident subpaths, crisp '
         'edges and non-scaling stroke preserve coverage.', ground=False)
for i, (name, options) in enumerate(variants):
    angle, reflected = options.get('angle', 0), options.get('reflected', False)
    if angle or reflected:
        cx, cy = transform((-.2, .5), angle, reflected)
        box = (cx - .08, cy - .08, .16, .16)
    else:
        box = (-.5, .3, .6, .4)
    certify(box, angle, reflected)
    args = {k: v for k, v in options.items()
            if k in ('reverse', 'split_at', 'split_bend', 'start', 'angle',
                     'reflected')}
    d = path_data(**args) * options.get('copies', 1)
    if options.get('opposite'):
        d += path_data(reverse=True)
    x, y = i % 4 * 32, i // 4 * 24
    alpha = options.get('alpha', 1)
    hint = ' shape-rendering="crispEdges"' if options.get('crisp') else ''
    if options.get('non_scaling'):
        certify((-.45, .3125, .5, .375))
        child = (f'<path transform="translate(28.8 -20) scale(64)" d="{d}" '
                 'vector-effect="non-scaling-stroke" fill="none" '
                 'stroke="lime" stroke-width="128"/>')
        test = f'<svg x="{x}" y="{y}" width="32" height="24">{child}</svg>'
    else:
        view = ' '.join(f'{v:.15g}' for v in box)
        child = (f'<path d="{d}" fill="none" stroke="lime" stroke-width="2" '
                 f'stroke-opacity="{alpha}"{hint}/>')
        test = (f'<svg x="{x}" y="{y}" width="32" height="24" '
                f'viewBox="{view}" preserveAspectRatio="none">{child}</svg>')
    expected = (f'<rect x="{x}" y="{y}" width="32" height="24" '
                f'fill="lime" fill-opacity="{alpha}"/>')
    u.cell(name, (x, y, 32, 24), test, expected)

# An ellipse can have both regular and inverted offsets: for rx=3, ry=1,
# its radius of curvature ranges from 1/3 to 9, across half-width 2.
# At this central probe the regular upper arc supplies a complete normal
# band. The analytic ellipse and its explicit four-cubic representation
# both have ample room inside the half width.
box = (.15, -.05, .1, .1)
corners = [(x, y) for x in (.15, .25) for y in (-.05, .05)]
a, b = -math.pi / 2 + .02, -math.pi / 2 + .14
for t, sign in [(a, -1), (b, 1)]:
    p, v = (3 * math.cos(t), math.sin(t)), (-3 * math.sin(t), math.cos(t))
    assert all(sign * sum((p[k] - c[k]) * v[k] for k in (0, 1)) > 0
               for c in corners)
assert -3 * math.sin(b) > 0
assert max(math.dist(p, c)
           for p in [(3 * math.cos(t), math.sin(s))
                     for t in (a, b) for s in (a, b)]
           for c in corners) < 1.2
k = .5522847498
ellipse = [[(3, 0), (3, -k), (3*k, -1), (0, -1)],
           [(0, -1), (-3*k, -1), (-3, -k), (-3, 0)],
           [(-3, 0), (-3, k), (-3*k, 1), (0, 1)],
           [(0, 1), (3*k, 1), (3, k), (3, 0)]]
q = split(split(ellipse[0], .995)[0], .90 / .995)[1]
v = [[3 * (q[j+1][i] - q[j][i]) for i in (0, 1)] for j in range(3)]
assert max(d[0] for d in v) < 0
assert max(sum((q[0][i]-c[i])*v[0][i] for i in (0, 1)) for c in corners) < 0
assert min(sum((q[-1][i]-c[i])*v[-1][i] for i in (0, 1)) for c in corners) > 0
assert max(math.dist(p, c) for p in q for c in corners) < 1.2
parts = [list(reversed(p)) for p in reversed(ellipse)]
d = ('M' + ' '.join(map(str, parts[0][0]))
     + ''.join('C' + ' '.join(str(v) for p in q[1:] for v in p) for q in parts)
     + 'Z')
for j, (name, child) in enumerate([
        ('mixed-ellipse', '<ellipse rx="3" ry="1" fill="none" stroke="lime" stroke-width="4"/>'),
        ('mixed-cubic-reversed', f'<path d="{d}" fill="none" stroke="lime" stroke-width="4"/>')]):
    x = j * 32
    test = (f'<svg x="{x}" y="72" width="32" height="24" '
            'viewBox=".15 -.05 .1 .1" preserveAspectRatio="none">' + child + '</svg>')
    u.cell(name, (x, 72, 32, 24), test,
           f'<rect x="{x}" y="72" width="32" height="24" fill="lime"/>')

# Fully inverted offsets keep D49. For both shapes h=3 exceeds every
# radius of curvature, at most rx**2/ry=1.21. Their inner offset winds
# round the origin at distance at least h-max(rx,ry)=1.9; the complete
# probe is strictly in its hole, even with the old D1 approximation.
assert 3 > 1.1**2 and math.hypot(.25, .05) < 3 - 1.1 - .025
for j, (name, shape) in enumerate([
        ('D49-circle-hole', '<circle r="1"'),
        ('D49-ellipse-hole', '<ellipse rx="1.1" ry="1"')]):
    x = 64 + j * 32
    child = shape + ' fill="none" stroke="lime" stroke-width="6"/>'
    test = (f'<svg x="{x}" y="72" width="32" height="24" '
            'viewBox=".15 -.05 .1 .1" preserveAspectRatio="none">' + child + '</svg>')
    u.cell(name, (x, 72, 32, 24), test, '')
u.write()
