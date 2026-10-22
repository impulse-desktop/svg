"""Interior ink in an open stroke is independently certified by its normals.

For every point P of a probe square, the dot product (B(t)-P).B'(t)
changes sign across the stored parameter interval. It is affine in P,
so checking the four corners proves a perpendicular for every point of
the square. The restricted Bezier control polygon stays within the half
width of all four corners, and its derivative has a component bounded
away from zero. Convexity and the intermediate value theorem therefore
prove that the entire square belongs to the ideal swept stroke.

The curve is the nonzero 22-segment tail of Sketchy-SVG
fb7665865da144f8. Chromium and resvg leave an offset-outline opening at
these positions. SVG 2 StrokeShape puts them inside the stroke; this
control preserves that requirement without adding another D49 exception.
"""
import math
from unitgen import Unit

SEGMENTS = [[[167.99998, 217.99994], [167.99914, 217.99994], [167.99331, 217.99994], [167.97747, 217.99994]], [[167.97747, 217.99994], [167.94665, 217.99994], [167.8958, 217.99994], [167.81998, 217.99994]], [[167.81998, 217.99994], [167.71415, 217.99994], [167.57331, 217.99957], [167.16664, 217.99994]], [[167.16664, 217.99994], [166.89247, 218.0002], [166.57318, 218.00028], [166.21414, 218.01344]], [[166.21414, 218.01344], [165.81981, 218.02791], [165.39497, 218.05491], [164.94664, 218.10794]], [[164.94664, 218.10794], [164.47647, 218.16356], [163.98297, 218.21716], [162.99997, 218.49994]], [[162.99997, 218.49994], [162.49387, 218.64552], [161.99309, 218.84828], [161.49997, 219.09393]], [[161.49997, 219.09393], [160.99246, 219.34674], [160.49034, 219.65306], [159.99997, 220.01193]], [[159.99997, 220.01193], [159.48956, 220.38545], [158.96574, 220.79761], [157.99997, 221.83326]], [[157.99997, 221.83326], [157.48186, 222.38886], [156.99995, 223.00951], [156.52248, 223.64527]], [[156.52248, 223.64527], [156.04657, 224.27894], [155.60181, 224.91498], [155.17997, 225.52926]], [[155.17997, 225.52926], [154.78361, 226.10642], [154.43987, 226.64787], [153.83331, 227.49993]], [[153.83331, 227.49993], [153.61291, 227.80953], [153.43832, 228.03501], [153.30381, 228.17643]], [[153.30381, 228.17643], [153.22616, 228.25808], [153.18165, 228.26881], [153.1973, 228.23193]], [[153.1973, 228.23193], [153.23722, 228.13789], [153.33591, 228.00592], [153.66665, 227.66661]], [[153.66665, 227.66661], [153.87786, 227.44992], [154.13394, 227.23086], [154.41064, 226.9991]], [[154.41064, 226.9991], [154.69921, 226.7574], [154.99905, 226.51956], [155.29864, 226.28661]], [[155.29864, 226.28661], [155.58842, 226.06128], [155.86282, 225.85173], [156.33331, 225.49994]], [[156.33331, 225.49994], [156.51355, 225.36517], [156.65071, 225.24805], [156.77131, 225.20744]], [[156.77131, 225.20744], [156.85267, 225.18005], [156.91665, 225.22911], [156.95731, 225.31994]], [[156.95731, 225.31994], [156.96553, 225.37021666666666], [156.97375, 225.42049333333333], [156.98197, 225.47077]], [[156.98197, 225.47077], [156.98619, 225.54270999999997], [156.99041, 225.61464999999998], [156.99463, 225.68659]]]

PROBES = [{'segment': 13, 'center': [148.529296875, 229.923828125], 'size': 1.0, 'interval': [0.7885605906132356, 0.8285605906132356]}, {'segment': 13, 'center': [164.037109375, 228.373046875], 'size': 1.0, 'interval': [0.8484110006246024, 0.8884110006246024]}, {'segment': 13, 'center': [156.283203125, 226.822265625], 'size': 1.0, 'interval': [0.757005514400396, 0.8370055144003961]}, {'segment': 13, 'center': [156.283203125, 236.126953125], 'size': 1.0, 'interval': [0.4426375084615473, 0.6026375084615473]}]

def mix(a, b, t):
    return tuple((1 - t) * x + t * y for x, y in zip(a, b))


def split(c, t):
    a, b, d, e = c
    ab, bd, de = mix(a, b, t), mix(b, d, t), mix(d, e, t)
    abd, bde = mix(ab, bd, t), mix(bd, de, t)
    p = mix(abd, bde, t)
    return [a, ab, abd, p], [p, bde, de, e]


def velocity(c, t):
    a, b, d, e = c
    return tuple(3 * (1 - t)**2 * (b[k] - a[k])
                 + 6 * (1 - t) * t * (d[k] - b[k])
                 + 3 * t*t * (e[k] - d[k]) for k in range(2))


def certify(probe):
    c = SEGMENTS[probe['segment']]
    center, size = probe['center'], probe['size']
    corners = [(center[0] + x * size / 2, center[1] + y * size / 2)
               for x, y in [(-1, -1), (-1, 1), (1, -1), (1, 1)]]
    a, b = probe['interval']
    pa, pb = split(c, a)[0][-1], split(c, b)[0][-1]
    va, vb = velocity(c, a), velocity(c, b)
    fa = [sum((pa[k] - p[k]) * va[k] for k in range(2)) for p in corners]
    fb = [sum((pb[k] - p[k]) * vb[k] for k in range(2)) for p in corners]
    assert (max(fa) < -1e-8 and min(fb) > 1e-8) or (min(fa) > 1e-8 and max(fb) < -1e-8)
    restricted = split(split(c, b)[0], a / b)[1]
    assert max(math.dist(q, p) for q in restricted for p in corners) < 23.145363476003382 / 2 - .1
    derivative = [[restricted[j+1][k] - restricted[j][k] for k in range(2)] for j in range(3)]
    assert any(min(d[k] for d in derivative) > 1e-10 or max(d[k] for d in derivative) < -1e-10 for k in range(2))


def path_data(reverse):
    pieces = [list(reversed(c)) for c in reversed(SEGMENTS)] if reverse else SEGMENTS
    def point(p):
        return ' '.join(format(v, '.15g') for v in p)
    return 'M' + point(pieces[0][0]) + ''.join('C' + ' '.join(point(p) for p in c[1:]) for c in pieces)


u = Unit('stroke-normal-witnesses', 64, 32,
         'Each complete lime square is inside the ideal open stroke. A sign change of the normal equation over a parameter interval, a nonzero tangent and a convex-hull distance bound certify every point of each probe square independently. Both path directions test the same geometry; competitor offset-outline openings do not remove its normal sweep.', ground=False)
for i, probe in enumerate(PROBES):
    certify(probe)
    cx, cy = probe['center']
    size = probe['size']
    view = f'{cx-size/2:.15g} {cy-size/2:.15g} {size} {size}'
    for reverse in [False, True]:
        x, y = i * 16, int(reverse) * 16
        test = f'<svg x="{x}" y="{y}" width="16" height="16" viewBox="{view}"><path d="{path_data(reverse)}" fill="none" stroke="lime" stroke-width="23.145363476003382"/></svg>'
        u.cell(f'{"start" if reverse else "end"}-{i+1}', (x, y, 16, 16), test,
               f'<rect x="{x}" y="{y}" width="16" height="16" fill="lime"/>')
u.write()
