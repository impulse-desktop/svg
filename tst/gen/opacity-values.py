"""opacity-values: the opacity property of CSS Color 4."""
from unitgen import Unit, rows

unit = Unit("opacity-values", 74, 0, """Each drawing fills its black outline as
the plain drawing beside it in the expected drawing: lime at the opacity
it should have. opacity takes a number or a percentage, clamped into 0 to
1; it applies to an element and its content as one, so overlapping
children are no darker where they overlap, and an opaque child hides what
is under it in the group; it is not inherited.""")


def item(x, y, value):
    test, alpha, width = value
    plain = f'<rect x="{x}" y="{y}" width="{width}" height="6" fill="lime" fill-opacity="{alpha}"/>' if alpha else ""

    return test.format(x=x, y=y, x3=x + 3), plain, f'<rect x="{x}" y="{y}" width="{width}" height="6"/>'


y = rows(unit, 2, [
    ("group", [('<g opacity=".5"><rect x="{x}" y="{y}" width="6" height="6" fill="lime"/><rect x="{x3}" y="{y}" width="6" height="6" fill="lime"/></g>', .5, 9)]),
    ("occlude", [('<g opacity=".5"><rect x="{x}" y="{y}" width="9" height="6" fill="red"/><rect x="{x}" y="{y}" width="9" height="6" fill="lime"/></g>', .5, 9)]),
    ("values", [('<rect x="{x}" y="{y}" width="9" height="6" fill="lime" opacity="50%"/>', .5, 9), ('<rect x="{x}" y="{y}" width="9" height="6" fill="lime" style="opacity: 25%"/>', .25, 9),
                ('<rect x="{x}" y="{y}" width="9" height="6" fill="lime" opacity=".75"/>', .75, 9)]),
    ("clamp", [('<rect x="{x}" y="{y}" width="9" height="6" fill="lime" opacity="1.5"/>', 1, 9), ('<rect x="{x}" y="{y}" width="9" height="6" fill="red" opacity="-1"/>', 0, 9),
               ('<rect x="{x}" y="{y}" width="9" height="6" fill="lime" style="opacity: 300%"/>', 1, 9)]),
    ("not-inherited", [('<g opacity=".5"><rect x="{x}" y="{y}" width="9" height="6" fill="lime"/></g>', .5, 9), ('<g style="opacity: .5"><rect x="{x}" y="{y}" width="9" height="6" fill="lime" style="opacity: inherit"/></g>', .25, 9)]),
], 6, 12, 8, item)
unit.height = y + 1
unit.write()
