"""transform-3d-functions: the 3D transform functions of CSS Transforms 2
drawn flat, and perspective."""
from matrices import P, R, R3, S, T, affine, chain, corners, identity, visible
from unitgen import Unit, rows

unit = Unit("transform-3d-functions", 122, 0, """Each lime drawing fills its
black outline, and nothing red shows; each rect turns about its centre and
is drawn flat, with no 3D rendering context. matrix3d() is in column-major
order, its weight dividing; translate3d() and translateZ(), scale3d() and
scaleZ() change nothing along z; rotate3d() turns about its normalized
vector, of none not at all, rotateX(), rotateY() and rotateZ() about their
axes; perspective() divides by its distance along z, below 1px 1px, none
none; a zero angle needs no unit, a scale takes percentages. A 3D function
with too few or too many arguments makes the list invalid. A later element
paints over an earlier one whatever their z. In perspective a rect behind
the eye draws nothing, and one partly behind it what is in front, out to
the horizon, here cut by a viewport; a perspective drawing resamples its
layer, so its edges are softer than a path's.""")
CENTRE = "transform-box: fill-box; transform-origin: center; "


def around(m):
    return chain(T(10, 10), m, T(-10, -10))


def item(x, y, value):
    style, m = value

    if m is None:
        test = f'<rect x="5" y="5" width="10" height="10" fill="red" style="{CENTRE}{style}"/>'
        return f'<g transform="translate({x} {y})">{test}</g>', "", ""

    if isinstance(m, tuple):
        test, plain = m
        return f'<g transform="translate({x} {y})">{test}</g>', f'<g fill="lime" transform="translate({x} {y})">{plain}</g>', f'<g transform="translate({x} {y})">{plain}</g>'

    d = corners(around(m), 5, 5, 10, 10)
    test = f'<rect x="5" y="5" width="10" height="10" fill="lime" style="{CENTRE}{style}"/>'

    return f'<g transform="translate({x} {y})">{test}</g>', f'<path fill="lime" transform="translate({x} {y})" d="{d}"/>', f'<path transform="translate({x} {y})" d="{d}"/>'


weight = identity()
weight[3][3] = 2
horizon = chain(T(10, 18), P(12), R3(1, 0, 0, 70), T(-10, -18))
y = rows(unit, 2, [
    ("matrix3d", [("transform: matrix3d(1,0,0,0, .5,1,0,0, 0,0,1,0, 2,4,0,1)", affine(1, 0, .5, 1, 2, 4)), ("transform: matrix3d(1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,2)", weight)]),
    ("translate3d", [("transform: translate3d(2px, 3px, 40px)", T(2, 3)), ("transform: translateZ(5px)", identity())]),
    ("scale3d", [("transform: scale3d(.5, 1, 9)", S(.5, 1)), ("transform: scaleZ(3)", identity())]),
    ("rotate3d", [("transform: rotate3d(1, 1, 0, 50deg)", R3(1, 1, 0, 50)), ("transform: rotate3d(0, 0, 1, 30deg)", R(30)), ("transform: rotate3d(0, 0, 0, 50deg)", identity())]),
    ("rotate-axes", [("transform: rotateX(60deg)", R3(1, 0, 0, 60)), ("transform: rotateY(60deg)", R3(0, 1, 0, 60)), ("transform: rotateZ(30deg)", R(30))]),
    ("perspective", [("transform: perspective(30px) translateZ(10px)", chain(P(30), T(0, 0, 10))), ("transform: perspective(none) translateZ(10px)", identity()),
                     ("transform: perspective(0) translateZ(-5px)", chain(P(1), T(0, 0, -5))), ("transform: perspective(.5px) translateZ(-1px)", chain(P(1), T(0, 0, -1)))]),
    ("zero-angle", [("transform: rotateX(0) translateX(2px)", T(2, 0)), ("transform: scale3d(50%, 100%, 1)", S(.5, 1))]),
    ("flat-context", [("", ('<rect x="5" y="5" width="10" height="10" fill="red" style="transform: translateZ(10px)"/><rect x="5" y="5" width="10" height="10" fill="lime" style="transform: translateZ(-10px)"/>',
                            '<rect x="5" y="5" width="10" height="10"/>'))]),
    ("arity", [(f"transform: {f}", identity()) for f in ["translateZ(1px, 2px)", "scale3d(1, 2)", "scaleZ(1, 2)", "rotate3d(1, 0, 0)", "rotateX(10deg, 2deg)", "perspective(1px, 2px)",
                                                         "translate3d(1px, 2px, 3px, 4px)", "matrix3d(1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0)", "rotateY()", "matrix3d(1,0,0,0)"]]),
    ("projection", [("transform: perspective(40px) rotateY(40deg)", chain(P(40), R3(0, 1, 0, 40))), ("transform: perspective(30px) rotate3d(1, -1, 0, 40deg)", chain(P(30), R3(1, -1, 0, 40)))]),
    ("behind", [("transform: perspective(10px) translateZ(20px)", None)]),
    ("horizon", [("", ('<svg width="20" height="20"><rect x="4" y="2" width="12" height="16" fill="lime" style="transform-origin: 10px 18px; transform: perspective(12px) rotateX(70deg)"/></svg>',
                       f'<svg width="20" height="20"><path d="{visible(horizon, 4, 2, 12, 16)}"/></svg>'))]),
], 6, 20, 20, item, tolerances={"projection": 64, "horizon": 64})
unit.height = y + 1
unit.write()
