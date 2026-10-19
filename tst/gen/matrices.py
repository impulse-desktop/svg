"""Matrices for the transform units: 4 by 4 ones, rows of columns, as
CSS Transforms 2 has them, and the corners of a rect they project, so an
expected drawing can draw a transformed rect as the plain path of its
corners."""
import math


def mul(a, b):
    return [[sum(a[i][k] * b[k][j] for k in range(4)) for j in range(4)] for i in range(4)]


def identity():
    return [[1.0 if i == j else 0.0 for j in range(4)] for i in range(4)]


def chain(*ms):
    m = identity()

    for n in ms:
        m = mul(m, n)

    return m


def affine(a, b, c, d, e, f):
    """The 2D matrix(a, b, c, d, e, f)."""
    m = identity()
    m[0][0], m[1][0], m[0][1], m[1][1], m[0][3], m[1][3] = a, b, c, d, e, f

    return m


def T(x, y, z=0.0):
    m = identity()
    m[0][3], m[1][3], m[2][3] = x, y, z

    return m


def S(x, y=None, z=1.0):
    m = identity()
    m[0][0], m[1][1], m[2][2] = x, x if y is None else y, z

    return m


def R(degrees):
    """A rotation about z, clockwise on the screen."""
    r = math.radians(degrees)

    return affine(math.cos(r), math.sin(r), -math.sin(r), math.cos(r), 0, 0)


def R3(x, y, z, degrees):
    """rotate3d(x, y, z, angle) as CSS Transforms 2 defines it."""
    n = math.sqrt(x * x + y * y + z * z)
    x, y, z = x / n, y / n, z / n
    a = math.radians(degrees)
    sc, sq = math.sin(a / 2) * math.cos(a / 2), math.sin(a / 2) ** 2
    m = identity()
    m[0][0], m[1][0], m[2][0] = 1 - 2 * (y * y + z * z) * sq, 2 * (x * y * sq + z * sc), 2 * (x * z * sq - y * sc)
    m[0][1], m[1][1], m[2][1] = 2 * (x * y * sq - z * sc), 1 - 2 * (x * x + z * z) * sq, 2 * (y * z * sq + x * sc)
    m[0][2], m[1][2], m[2][2] = 2 * (x * z * sq + y * sc), 2 * (y * z * sq - x * sc), 1 - 2 * (x * x + y * y) * sq

    return m


def K(ax, ay=0.0):
    return affine(1, math.tan(math.radians(ay)), math.tan(math.radians(ax)), 1, 0, 0)


def P(d):
    m = identity()
    m[3][2] = -1.0 / d

    return m


def project(m, x, y):
    X = m[0][0] * x + m[0][1] * y + m[0][3]
    Y = m[1][0] * x + m[1][1] * y + m[1][3]
    W = m[3][0] * x + m[3][1] * y + m[3][3]

    return X / W, Y / W


def corners(m, x, y, w, h):
    """The path of a rect's corners through m."""
    points = [project(m, a, b) for a, b in [(x, y), (x + w, y), (x + w, y + h), (x, y + h)]]

    return "M" + " ".join(f"{a:.4f} {b:.4f}" for a, b in points) + "Z"


def visible(m, x, y, w, h, near=1e-4):
    """The path of what of a rect m keeps in front of the eye: the rect cut
    where its weight falls to near, projected; the cut edge goes far away,
    for a viewport around it to bound."""
    def weight(p):
        return m[3][0] * p[0] + m[3][1] * p[1] + m[3][3]

    kept = []
    points = [(x, y), (x + w, y), (x + w, y + h), (x, y + h)]

    for i, p in enumerate(points):
        q = points[(i + 1) % 4]
        a, b = weight(p) - near, weight(q) - near

        if a >= 0:
            kept.append(p)

        if (a >= 0) != (b >= 0):
            t = a / (a - b)
            kept.append((p[0] + (q[0] - p[0]) * t, p[1] + (q[1] - p[1]) * t))

    return "M" + " ".join("%.4f %.4f" % project(m, *p) for p in kept) + "Z" if kept else ""
