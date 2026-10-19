"""clip-path-polygon-round: polygon() rounding of CSS Shapes 1."""
import math

from unitgen import Unit

unit = Unit("clip-path-polygon-round", 106, 34, """Each lime drawing fills
its black outline. polygon() rounds every vertex by "round" and a length,
with a circle tangent to both its sides whose radius is clamped at each
vertex to tan(angle / 2) times half of either side, so a thin box rounds
to a half circle; a concave vertex rounds the other way; a vertex of a
side of no length is not rounded; the fill rule comes before the
rounding. A rounding of no length, of a percentage, of a negative length
or with more after it, or a polygon of no points is invalid, and the rect
draws unclipped.""")


def rounded(points, radius):
    """The rounded polygon as SVG path data: each vertex a tangent circle,
    its radius clamped to tan(angle / 2) times half either side."""
    n = len(points)
    d = []

    for i in range(n):
        vx, vy = points[i]
        ax, ay = points[i - 1][0] - vx, points[i - 1][1] - vy
        bx, by = points[(i + 1) % n][0] - vx, points[(i + 1) % n][1] - vy
        la, lb = math.hypot(ax, ay), math.hypot(bx, by)
        cos = (ax * bx + ay * by) / (la * lb) if la > 0 and lb > 0 else -1
        half = math.sqrt(max(1 - cos, 0) / max(1 + cos, 1e-12))
        r = min(radius, half * la / 2, half * lb / 2)
        t = r / half if r > 0 else 0
        t1 = (vx + (ax / la * t if la else 0), vy + (ay / la * t if la else 0))
        t2 = (vx + (bx / lb * t if lb else 0), vy + (by / lb * t if lb else 0))
        d.append(("M" if i == 0 else "L") + f"{t1[0]:.4f} {t1[1]:.4f}")

        if t > 0:
            d.append(f"A{r:.4f} {r:.4f} 0 0 {1 if ax * by - ay * bx < 0 else 0} {t2[0]:.4f} {t2[1]:.4f}")

    return " ".join(d) + "Z"


def cell(name, places):
    tests, plains, outlines = [], [], []

    for (x, y), (points, radius, prefix) in places:
        argument = prefix + ", " + ", ".join(f"{px}px {py}px" for px, py in points)
        shape = f'<path fill-rule="{"evenodd" if "evenodd" in prefix else "nonzero"}" transform="translate({x} {y})" d="{rounded(points, radius)}"/>'
        tests.append(f'<rect x="{x}" y="{y}" width="12" height="12" fill="lime" style="clip-path: polygon({argument})"/>')
        plains.append(shape.replace("<path", '<path fill="lime"'))
        outlines.append(shape)

    x0 = min(x for (x, _), _ in places)
    y0 = min(y for (_, y), _ in places)
    x1 = max(x for (x, _), _ in places) + 12
    y1 = max(y for (_, y), _ in places) + 12
    unit.cell(name, (x0 - 1, y0 - 1, x1 - x0 + 2, y1 - y0 + 2), "".join(tests), "".join(plains), "".join(outlines))


square = [(0, 0), (12, 0), (12, 12), (0, 12)]
cell("convex", [((2, 2), (square, 3, "round 3px")), ((20, 2), ([(6, 0), (12, 12), (0, 12)], 2, "round 2px"))])
cell("concave", [((38, 2), ([(0, 0), (12, 0), (12, 4), (4, 4), (4, 12), (0, 12)], 1.5, "round 1.5px"))])
cell("clamped", [((56, 2), ([(0, 4), (12, 4), (12, 8), (0, 8)], 5, "round 5px"))])
cell("fill-rule", [((2, 18), ([(0, 0), (12, 6), (0, 12), (6, 6)], 2, "evenodd round 2px"))])
cell("zero", [((20, 18), (square, 0, "round 0")), ((38, 18), ([(0, 0), (12, 0), (12, 0), (12, 12), (0, 12)], 3, "round 3px"))])
tests, plains, outlines = [], [], []
for (x, y), argument in zip([(56, 18), (74, 2), (74, 18), (92, 2), (92, 18)], ["round, 0 0, 12px 0, 0 12px", "round 10%, 0 0, 12px 0, 0 12px", "round 2px", "round -2px, 0 0, 12px 0, 0 12px", "round 2px x, 0 0, 12px 0, 0 12px"]):
    tests.append(f'<rect x="{x}" y="{y}" width="12" height="12" fill="lime" style="clip-path: polygon({argument})"/>')
    plains.append(f'<rect x="{x}" y="{y}" width="12" height="12" fill="lime"/>')
    outlines.append(f'<rect x="{x}" y="{y}" width="12" height="12"/>')
unit.cell("invalid", (55, 1, 50, 30), "".join(tests), "".join(plains), "".join(outlines))
unit.write()
