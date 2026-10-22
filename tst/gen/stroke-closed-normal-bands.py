"""A tight bend must not cancel a closed path's straight stroke band.

The reproducer is reduced from svg-stack/543c61951adff405. Every corner
of each probe projects strictly inside a literal line and is less than
half the stroke width from it. Convexity certifies the full rectangle,
independently of a competitor. Representation, transformation, opacity
and rasterization controls exercise the union before paint is applied.
The existing stroke-wider-than-shape controls preserve D49's circles.
"""
from math import cos, sin, pi

from unitgen import Unit


SEGMENTS = [
    [(0, 0), (-.03, -.04), (-.02, -.04), (.04, 0)],
    [(.04, 0), (-14, -7)],
    [(-14, -7), (0, 16)],
    [(0, 16), (0, .1)],
    [(0, .1), (0, 0)],
]


def mix(p, q, t):
    return tuple((1 - t) * a + t * b for a, b in zip(p, q))


def split(c, t):
    a, b, d, e = c
    ab, bd, de = mix(a, b, t), mix(b, d, t), mix(d, e, t)
    abd, bde = mix(ab, bd, t), mix(bd, de, t)
    p = mix(abd, bde, t)
    return [a, ab, abd, p], [p, bde, de, e]


def pieces(reverse=False, split_at=None, subline=False, start=0, cubic_line=False):
    out = [p[:] for p in SEGMENTS]
    if split_at is not None:
        out = list(split(out[0], split_at)) + out[1:]
    if subline or cubic_line:
        j = out.index([(0, 16), (0, .1)])
        a, b = out[j]
        if subline:
            out[j:j + 1] = [[a, (0, 8)], [(0, 8), b]]
        else:
            out[j] = [a, mix(a, b, 1 / 3), mix(a, b, 2 / 3), b]
    out = out[start:] + out[:start]
    return [list(reversed(p)) for p in reversed(out)] if reverse else out


def path_data(parts, angle=0, reflected=False):
    c, s = cos(angle * pi / 180), sin(angle * pi / 180)

    def point(p):
        x, y = p
        x = -x if reflected else x
        return f'{x * c - y * s:.15g} {x * s + y * c:.15g}'

    return ('M' + point(parts[0][0])
            + ''.join(('C' if len(p) == 4 else 'L')
                      + ' '.join(point(q) for q in p[1:]) for p in parts) + 'Z')


def probe(angle, reflected):
    c, s = cos(angle * pi / 180), sin(angle * pi / 180)
    if angle:
        cx, cy = -.2 * c - .5 * s, -.2 * s + .5 * c
        box = (cx - .12, cy - .12, .24, .24)
    else:
        box = (-.3 if reflected else -.7, .19, 1, .7)
    for x in [box[0], box[0] + box[2]]:
        for y in [box[1], box[1] + box[3]]:
            # Inverse rotation/reflection gives the literal vertical line.
            px, py = x * c + y * s, -x * s + y * c
            assert abs(px) < 1 and .1 < py < 16
    return box


variants = [
    ('closed', {}),
    ('reversed', {'reverse': True}),
    ('split-half', {'split_at': .5}),
    ('split-unequal', {'split_at': 1 / 1024}),
    ('straight-split', {'subline': True}),
    ('changed-start', {'start': 2}),
    ('rotated', {'angle': 37}),
    ('rotated-reversed', {'reverse': True, 'split_at': 1 / 1024,
                          'subline': True, 'angle': 127}),
    ('line-as-cubic', {'cubic_line': True}),
    ('translucent-overlap', {'copies': 2, 'alpha': .5}),
    ('crisp', {'crisp': True}),
    ('non-scaling', {'non_scaling': True}),
    ('reflected', {'reflected': True}),
    ('opposite-subpaths', {'opposite': True}),
    ('many-subpaths', {'copies': 64, 'alpha': .5}),
    ('many-crisp', {'copies': 64, 'alpha': .5, 'crisp': True}),
]
u = Unit('stroke-closed-normal-bands', 128, 96,
         'Each full lime tile is independently inside a straight stroke band. '
         'A tight bend of the closed path cannot subtract that band. '
         'Direction, exact subdivision, start point, straight cubic, rotation, '
         'reflection and non-scaling stroke preserve coverage. Coincident '
         'subpaths are united before transparent paint, also with crisp edges.',
         ground=False)
for i, (name, options) in enumerate(variants):
    args = {k: v for k, v in options.items()
            if k in ('reverse', 'split_at', 'subline', 'start', 'cubic_line')}
    angle, reflected = options.get('angle', 0), options.get('reflected', False)
    d = path_data(pieces(**args), angle, reflected) * options.get('copies', 1)
    if options.get('opposite'):
        d += path_data(pieces(reverse=True))
    x, y = (i % 4) * 32, (i // 4) * 24
    alpha = options.get('alpha', 1)
    hint = ' shape-rendering="crispEdges"' if options.get('crisp') else ''
    if options.get('non_scaling'):
        # The device-space line is x=22.4, y in [-2.88, 505.92], h=32.
        # Every corner of the 32x24 viewport lies strictly in that band.
        for px in (0, 32):
            for py in (0, 24):
                assert abs(px - 22.4) < 32 and -2.88 < py < 505.92
        child = (f'<path transform="translate(22.4 -6.08) scale(32)" d="{d}" '
                 'vector-effect="non-scaling-stroke" fill="none" '
                 'stroke="lime" stroke-width="64"/>')
        test = f'<svg x="{x}" y="{y}" width="32" height="24">{child}</svg>'
    else:
        view = ' '.join(f'{v:.15g}' for v in probe(angle, reflected))
        child = (f'<path d="{d}" fill="none" stroke="lime" stroke-width="2" '
                 f'stroke-opacity="{alpha}"{hint}/>')
        test = (f'<svg x="{x}" y="{y}" width="32" height="24" '
                f'viewBox="{view}" preserveAspectRatio="none">{child}</svg>')
    expected = (f'<rect x="{x}" y="{y}" width="32" height="24" '
                f'fill="lime" fill-opacity="{alpha}"/>')
    u.cell(name, (x, y, 32, 24), test, expected)
u.write()
