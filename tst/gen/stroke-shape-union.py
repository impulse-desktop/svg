"""stroke-shape-union: a stroke as the union of its bands, joins and caps, of
SVG 2. The unit is written by hand; this writes its expected drawing: each
stroke's outer edge filled, the shape's middle all covered."""
import math

from unitgen import expect


def bevelled(points, half):
    """The outer edge of a closed polygon's bevel-joined stroke: at each
    vertex the ends of the two edges' outward offsets."""
    n = len(points)
    area = sum(points[i][0] * points[(i + 1) % n][1] - points[(i + 1) % n][0] * points[i][1] for i in range(n))
    out = []

    for i in range(n):
        for a, b in ((points[i - 1], points[i]), (points[i], points[(i + 1) % n])):
            dx, dy = b[0] - a[0], b[1] - a[1]
            length = math.hypot(dx, dy)
            # Outward: to the left of the way round, for a positive area,
            # which runs clockwise on the screen.
            nx, ny = (dy / length, -dx / length) if area > 0 else (-dy / length, dx / length)
            out.append((points[i][0] + nx * half, points[i][1] + ny * half))

    return "M " + " L ".join("%.6f %.6f" % p for p in out) + " Z"


def quad(p0, p1, p2, t):
    return tuple((1 - t) ** 2 * a + 2 * t * (1 - t) * b + t * t * c for a, b, c in zip(p0, p1, p2))


def offset_loop(pieces, half, steps=400):
    """The outer offset of a smooth closed loop of quadratics, half out."""
    out = []

    for p0, p1, p2 in pieces:
        for k in range(steps):
            t = k / steps
            x, y = quad(p0, p1, p2, t)
            dx = 2 * (1 - t) * (p1[0] - p0[0]) + 2 * t * (p2[0] - p1[0])
            dy = 2 * (1 - t) * (p1[1] - p0[1]) + 2 * t * (p2[1] - p1[1])
            length = math.hypot(dx, dy)
            # The loop runs clockwise on the screen: outward is to the left.
            out.append((x + dy / length * half, y - dx / length * half))

    return "M " + " L ".join("%.6f %.6f" % p for p in out) + " Z"


LOOP = [((24, 28), (26, 26), (28, 28)), ((28, 28), (30, 30), (28, 32)), ((28, 32), (26, 34), (24, 32)), ((24, 32), (22, 30), (24, 28))]

expect("stroke-shape-union", [
    ("miter-rect", (3, 3, 14, 14), '<rect x="4" y="4" width="12" height="12" fill="lime"/>'),
    ("round-rect", (20, 0, 12, 20), '<rect x="21" y="0" width="10" height="20" rx="4" fill="lime"/>'),
    # A curve's stroke edge and a circle are both drawn as flattened
    # curves, which differ by the stroke's leeway (D1); the circle, tighter
    # than the half width all round, keeps its inner offset's hole (D49).
    ("circle", (39, 3, 14, 14), '<path fill-rule="evenodd" fill="lime" d="M 40 10 A 6 6 0 1 0 52 10 A 6 6 0 1 0 40 10 Z M 44 10 A 2 2 0 1 0 48 10 A 2 2 0 1 0 44 10 Z"/>', 4),
    ("bevel-triangle", (1, 20, 18, 17), f'<path d="{bevelled([(7, 31), (10, 26), (13, 31)], 4)}" fill="lime"/>'),
    ("curves", (20, 24, 12, 12), f'<path d="{offset_loop(LOOP, 3)}" fill="lime"/>', 4),
    ("subpaths", (37, 24, 16, 14), '<rect x="38.5" y="23.5" width="13" height="13" fill="lime"/>'),
])
