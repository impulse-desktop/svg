"""fill-rule-crossings: nonzero and evenodd, of SVG 2. The unit is written by
hand; this writes its expected drawing: what each rule fills as plain
shapes, a pentagram as its outline, the middle a hole with evenodd."""
from unitgen import expect


def cross(p, q, r, s):
    """Where segment pq meets rs, or None."""
    d = (q[0] - p[0]) * (s[1] - r[1]) - (q[1] - p[1]) * (s[0] - r[0])

    if d == 0:
        return None

    t = ((r[0] - p[0]) * (s[1] - r[1]) - (r[1] - p[1]) * (s[0] - r[0])) / d
    u = ((r[0] - p[0]) * (q[1] - p[1]) - (r[1] - p[1]) * (q[0] - p[0])) / d

    return (p[0] + t * (q[0] - p[0]), p[1] + t * (q[1] - p[1])) if 0 < t < 1 and 0 < u < 1 else None


def star(points):
    """The outline of a pentagram drawn through points in order, its tips
    and the crossings between them by angle round its middle, and its
    middle pentagon."""
    import math

    edges = [(points[i], points[(i + 1) % 5]) for i in range(5)]
    crossings = {c for i, (p, q) in enumerate(edges) for r, s in edges[i + 2:] for c in [cross(p, q, r, s)] if c}
    cx, cy = sum(x for x, _ in crossings) / 5, sum(y for _, y in crossings) / 5

    def turned(ps):
        return sorted(ps, key=lambda c: math.atan2(c[1] - cy, c[0] - cx))

    def d(ps):
        return "M " + " L ".join("%.6f %.6f" % c for c in ps) + " Z"

    assert len(crossings) == 5
    return d(turned(list(points) + list(crossings))), d(turned(crossings))


def ring(outer, inner):
    return f'<path d="M {outer} Z M {inner} Z" fill="lime" fill-rule="evenodd"/>'


nonzero_outline, _ = star([(11, 23), (16, 39), (3, 29), (19, 29), (6, 39)])
evenodd_outline, evenodd_middle = star([(43, 23), (48, 39), (35, 29), (51, 29), (38, 39)])

expect("fill-rule-crossings", [
    ("nonzero-same", (1, 1, 20, 20), '<rect x="2" y="2" width="18" height="18" fill="lime"/>'),
    ("nonzero-opposite", (21, 1, 20, 20), ring("22 2 H 40 V 20 H 22", "27 7 H 35 V 15 H 27") + '<rect x="28" y="8" width="6" height="6" fill="blue"/>'),
    ("evenodd-same", (41, 1, 20, 20), ring("42 2 H 60 V 20 H 42", "47 7 H 55 V 15 H 47") + '<rect x="48" y="8" width="6" height="6" fill="blue"/>'),
    ("nonzero-pentagram", (2, 22, 18, 18), f'<path d="{nonzero_outline}" fill="lime"/>'),
    ("evenodd-pentagram", (34, 22, 18, 18), f'<path d="{evenodd_outline} {evenodd_middle}" fill="lime" fill-rule="evenodd"/><rect x="41.5" y="30" width="3" height="3" fill="blue"/>'),
])
