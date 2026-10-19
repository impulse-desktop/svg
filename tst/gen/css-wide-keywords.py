"""css-wide-keywords: initial, inherit and unset as css-values-4 has them."""
from unitgen import Unit

unit = Unit("css-wide-keywords", 64, 28, """Each drawing fills its black
outline, black for the first and the fifth, lime else, and nothing red
shows. The CSS-wide keywords as the sole value of any property, in any
case: inherit takes the parent's, of a property that does not inherit too,
initial the initial value, unset either as the property inherits or not;
with any other component the declaration is invalid and ignored. Below,
opacity unset is its initial 1 in a half opaque group, as light as the
half opaque one beside it.""")
unit.common('<linearGradient id="u" style="stop-color: red"><stop style="stop-color: unset"/></linearGradient>'
            '<linearGradient id="i" style="stop-color: lime"><stop style="stop-color: inherit"/></linearGradient>')


def box(x, y, colour="lime", w=10, h=6):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{colour}"/>', f'<rect x="{x}" y="{y}" width="{w}" height="{h}"/>'


def cell(name, x, y, test, colour="lime", w=10, h=6):
    plain, outline = box(x, y, colour, w, h)
    unit.cell(name, (x - 1, y - 1, w + 2, h + 2), test, plain, outline)


cell("initial", 2, 2, '<g fill="red"><rect x="2" y="2" width="10" height="6" style="fill: initial"/></g>', "black")
cell("inherit", 14, 2, '<g fill="lime"><rect x="14" y="2" width="10" height="6" fill="red" style="fill: inherit"/></g>')
cell("unset-inherited", 26, 2, '<g fill="lime"><rect x="26" y="2" width="10" height="6" fill="red" style="fill: unset"/></g>')
cell("case", 38, 2, '<g fill="lime"><rect x="38" y="2" width="10" height="6" fill="red" style="fill: INHERIT"/></g>')
cell("unset-not-inherited", 50, 2, '<rect x="50" y="2" width="10" height="6" fill="url(#u)"/>', "black")
cell("inherit-not-inherited", 2, 12, '<rect x="2" y="12" width="10" height="6" fill="url(#i)"/>')
unit.cell("initial-stroke", (13, 13.5, 12, 3), '<g stroke-width="5"><path d="M14 15h10" stroke="lime" style="stroke-width: initial"/></g>', *box(14, 14.5, "lime", 10, 1))
cell("combined", 26, 12, '<g fill="lime"><rect x="26" y="12" width="10" height="6" style="fill: initial red"/></g>')
cell("combined-twice", 38, 12, '<g fill="lime"><rect x="38" y="12" width="10" height="6" style="fill: inherit inherit"/></g>')
cell("inherit-geometry", 50, 12, '<svg x="50" y="12" width="10" height="6"><rect fill="lime" style="width: inherit; height: inherit"/></svg>')
unit.cell("unset-opacity", (1, 20, 24, 7), '<g fill="lime" opacity=".5"><rect x="2" y="21" width="10" height="5" style="opacity: unset"/></g><rect x="14" y="21" width="10" height="5" fill="lime" opacity=".5"/>',
          '<rect x="2" y="21" width="10" height="5" fill="lime" opacity=".5"/><rect x="14" y="21" width="10" height="5" fill="lime" opacity=".5"/>')
unit.write()
