"""css-angle-units: the angle units of css-values-4 as rotations."""
from unitgen import Unit, rows

unit = Unit("css-angle-units", 100, 0, """Each lime drawing fills its black
outline, and nothing red shows. A bar turned by a quarter of a circle about
its centre stands upright: 90deg, 100grad, pi/2 rad and .25turn, ASCII
case-insensitive, and a full circle more, 360deg, 400grad, 2pi rad, 1turn;
a calc() of mixed angle units; a gradient's .25turn the angle to the right
and a hue rotation's .5turn half a circle. A rotation by a bare number but
zero is invalid, so the bar lies flat; a bare zero is an angle where a
grammar takes it, in rotate(), so the square's transform attribute is
overridden, and in a gradient.""")


def turned(x, y, value):
    test, plain = value

    if test.startswith("rotate"):
        test = f'<rect x="{x}" y="{y + 4}" width="12" height="4" fill="lime" style="transform-box: fill-box; transform-origin: center; transform: {test}"/>'
        plain = f'<rect x="{x + 4}" y="{y}" width="4" height="12" fill="lime"/>' if plain is None else plain.format(x=x, y=y)
    else:
        test, plain = test.format(x=x, y=y), plain.format(x=x, y=y)

    return test, plain, "" if "#c84" in plain else plain.replace(' fill="lime"', "")


y = rows(unit, 2, [
    ("deg", [("rotate(90deg)", None), ("rotate(90DEG)", None), ("rotate(450deg)", None)]),
    ("grad", [("rotate(100grad)", None), ("rotate(100GRAD)", None), ("rotate(500grad)", None)]),
    ("rad", [("rotate(1.5707964rad)", None), ("rotate(1.5707964RAD)", None), ("rotate(7.853982rad)", None)]),
    ("turn", [("rotate(.25turn)", None), ("rotate(.25TURN)", None), ("rotate(1.25turn)", None)]),
    ("compat", [
        ("rotate(calc(50grad + 45deg))", None),
        ("rotate(calc(1turn / 4 - 0rad))", None),
        ('<rect x="{x}" y="{y}" width="12" height="12" fill="lime" style="mask-image: linear-gradient(.25turn, black 50%, transparent 50%)"/>', '<rect x="{x}" y="{y}" width="6" height="12" fill="lime"/>'),
        ('<rect x="{x}" y="{y}" width="12" height="12" fill="#c84" style="filter: hue-rotate(.5turn)"/>', '<rect x="{x}" y="{y}" width="12" height="12" fill="#c84" style="filter: hue-rotate(180deg)"/>'),
    ]),
    ("zero", [
        ("rotate(45)", '<rect x="{x}" y="{y}" width="12" height="4" fill="lime" transform="translate(0 4)"/>'),
        ('<rect x="{x}" y="{y}" width="12" height="12" fill="lime" transform="translate(50)" style="transform: translate(0px) rotate(0)"/>', '<rect x="{x}" y="{y}" width="12" height="12" fill="lime"/>'),
        ('<rect x="{x}" y="{y}" width="12" height="12" fill="lime" style="mask-image: linear-gradient(0, black 50%, transparent 50%)"/>', '<rect x="{x}" y="{y}" width="12" height="6" fill="lime" transform="translate(0 6)"/>'),
    ]),
], 6, 16, 14, turned)
unit.height = y + 1
unit.write()
