"""transform-individual-properties: translate, rotate and scale of CSS
Transforms 2."""
from matrices import R, R3, S, T, chain, corners
from unitgen import Unit, rows

unit = Unit("transform-individual-properties", 122, 0, """Each lime drawing
fills its black outline. translate, rotate and scale move, turn and size an
element before its transform, about its origin: a percentage scale is its
number, a percentage translation of the reference box; rotate about z, by
keyword or vector, is the 2D rotate; none is no transform; a 3D rotate
about x by nothing, or a translation along z, changes no pixel, by 60deg,
the axis after the angle, it halves the height, so too about the vector
after the angle; four scales, a percentage along z, a scale of no number,
an axis of no name, three or five rotate values or a vector of no number
make the value invalid; a scale along z of 1 or 100% is the 2D scale, of 2
changes no pixel; a translation of one value moves along x, of zero along
z is 2D.""")
CENTRE = "transform-box: fill-box; transform-origin: center; "


def item(x, y, value):
    style, matrix, centred = value
    w, h = 8, 6
    m = chain(T(x + 4 + 6, y + 3 + 6), matrix, T(-(x + 4 + 6), -(y + 3 + 6))) if centred else matrix
    test = f'<rect x="{x + 6}" y="{y + 6}" width="{w}" height="{h}" fill="lime" style="{CENTRE if centred else ""}{style}"/>'
    d = corners(m, x + 6, y + 6, w, h)

    return test, f'<path fill="lime" d="{d}"/>', f'<path d="{d}"/>'


same = chain()
y = rows(unit, 2, [
    ("translate", [("translate: 4px 2px", T(4, 2), False), ("translate: 50% 0", T(4, 0), True), ("translate: 3px", T(3, 0), False), ("translate: 2px 2px 0px", T(2, 2), False)]),
    ("translate-z", [("rotate: x 0deg; translate: 0px 0px 5px", same, False), ("translate: 0 0 1px", same, False)]),
    ("translate-invalid", [("translate: 5px 5px 10%", same, False), ("translate: 1px 1px abc", same, False), ("translate: 1px 1px 0px%", same, False), ("translate: 1px 1px 0%", same, False)]),
    ("rotate", [("rotate: 30deg", R(30), True), ("rotate: z 45deg", R(45), True), ("rotate: 0 0 1 -30deg", R(-30), True), ("rotate: 0 0 -1 30deg", R(-30), True)]),
    ("rotate-3d", [("rotate: 60deg x", R3(1, 0, 0, 60), True), ("rotate: 60deg 1 0 0", R3(1, 0, 0, 60), True), ("rotate: 0 1 0 60deg", R3(0, 1, 0, 60), True), ("rotate: y 60deg", R3(0, 1, 0, 60), True)]),
    ("rotate-invalid", [("rotate: 10deg q", same, False), ("rotate: 1 2 3", same, False), ("rotate: 1 2 3 4 5", same, False), ("rotate: 1 a 0 30deg", same, False), ("rotate: 30deg a 0 0", same, False),
                        ("rotate: 1 0 a 30deg", same, False)]),
    ("scale", [("scale: 50%", S(.5), True), ("scale: 1.5 .5", S(1.5, .5), True), ("scale: 1.5 1 1", S(1.5, 1), True), ("scale: 1 -1 100%", S(1, -1), True)]),
    ("scale-z", [("scale: 1 1 2", same, False), ("scale: 1 1 2%", same, False)]),
    ("scale-invalid", [("scale: 1 2 3 4", same, False), ("scale: 1 1 abc", same, False), ("scale: 1 1 1%x", same, False), ("scale: 1 1 1px", same, False)]),
    ("none", [("translate: none; rotate: none; scale: none", same, False), ("translate: 3px 1px; transform: none", T(3, 1), False)]),
    ("order", [("translate: 2px; rotate: 90deg; scale: 2 1; transform: translate(1px)", chain(T(2, 0), R(90), S(2, 1), T(1, 0)), True)]),
], 6, 20, 18, item)
unit.height = y + 1
unit.write()
