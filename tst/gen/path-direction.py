"""path-direction: the direction of a path, of SVG 2, where a dash's caps
take it. The expected drawing draws each dashed stroke as the polygon its
dash, square caps and all, makes; an arc as its cubics."""
import math

from unitgen import Unit

unit = Unit("path-direction", 64, 20, """Each lime stroke fills its black
outline, the shape the direction of the path gives it. A dash starting at a
segment boundary goes along the segment starting there, past a zero length
one; a dash ending inside a curve takes its tangent there; a curve along a
line that turns back on itself ends square at the turn, its two directions
there opposite.""")

STROKE = '<path d="{}" fill="none" stroke="lime" stroke-width="2" stroke-linecap="square"{}/>'

unit.cell("boundary-skip", (2, 1, 16, 14), STROKE.format("M 4 4 H 14 L 14 4 V 14", ' stroke-dasharray="8 100" stroke-dashoffset="-10"'),
          '<rect x="13" y="3" width="2" height="10" fill="lime"/>', '<rect x="13" y="3" width="2" height="10"/>')


def at(cx, cy, r, angle, back=0.0):
    """The point at angle on the circle, moved back along its clockwise
    tangent."""
    return cx + r * math.cos(angle) + back * math.sin(angle), cy + r * math.sin(angle) - back * math.cos(angle)


def cubic(cx, cy, r, a, b):
    """The cubic of the arc from a to b, at most a quarter turn."""
    k = 4 / 3 * math.tan((b - a) / 4)
    p0, p3 = at(cx, cy, r, a), at(cx, cy, r, b)
    p1 = (p0[0] - k * r * math.sin(a), p0[1] + k * r * math.cos(a))
    p2 = (p3[0] + k * r * math.sin(b), p3[1] - k * r * math.cos(b))

    return "C " + " ".join("%.4f %.4f" % p for p in (p1, p2, p3))


# A quarter circle of radius 6 dashed to half its length: the dash ends at
# 45 degrees, its square cap along the tangent there.
cx, cy, r = 22, 4, 6
half = math.pi / 4
outline = ("M %.4f %.4f L %.4f %.4f " % (at(cx, cy, r + 1, 0, 1) + at(cx, cy, r + 1, 0)) + cubic(cx, cy, r + 1, 0, half)
           + " L %.4f %.4f L %.4f %.4f L %.4f %.4f " % (at(cx, cy, r + 1, half, -1) + at(cx, cy, r - 1, half, -1) + at(cx, cy, r - 1, half))
           + cubic(cx, cy, r - 1, half, 0) + " L %.4f %.4f Z" % at(cx, cy, r - 1, 0, 1))
unit.cell("curve-tangent", (21, 1, 14, 14), STROKE.format(f"M {cx + r} {cy} A {r} {r} 0 0 1 {cx} {cy + r}", ' stroke-dasharray="%.6f 100"' % (r * half)),
          f'<path d="{outline}" fill="lime"/>', f'<path d="{outline}"/>')

# A quadratic along a line from 40 out and back to 46: it turns at t = 4/7,
# where its speed is none, at 2632/49, and its stroke ends square there.
turn = (9 * 40 + 24 * 64 + 16 * 46) / 49
unit.cell("cusp-fold", (38, 6, 18, 8), '<path d="M 40 10 Q 64 10 46 10" fill="none" stroke="lime" stroke-width="4"/>',
          '<rect x="40" y="8" width="%.6f" height="4" fill="lime"/>' % (turn - 40), '<rect x="40" y="8" width="%.6f" height="4"/>' % (turn - 40))

unit.write()
