"""resource-length-units: the lengths of filters, primitives, masks,
gradients and patterns in em of their own element, and in bounding box units
lengths of the box's user space."""
from unitgen import Unit


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


STOPS = '<stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="white"/>'
unit = Unit("resource-length-units", 80, 50, """Each lime drawing fills its
black outline, and nothing red shows. An em in the attributes of a filter,
a primitive, a mask, a gradient or a pattern is of that element's font-size,
a gradient template's own where it gives the attribute, not of the element
they paint; in bounding box units a length of a unit is one of the box's
user space, here 0.125em of a 4px font half the box.""")
unit.common('<defs>'
            '<filter id="fe" filterUnits="userSpaceOnUse" style="font-size: 4px" x="1em" y="1em" width="3em" height="2em"><feFlood flood-color="lime"/></filter>'
            '<filter id="fp" style="font-size: 4px"><feFlood flood-color="lime" x="1em" y="1em" width="2em" height="2em"/></filter>'
            '<mask id="mb" style="font-size: 4px" x="0" y="0" width="0.125em" height="1"><rect width="100" height="100" fill="white"/></mask>'
            f'<linearGradient id="ge" gradientUnits="userSpaceOnUse" x2="2em" style="font-size: 6px">{STOPS}</linearGradient>'
            '<pattern id="pe" patternUnits="userSpaceOnUse" width="2em" height="2em" style="font-size: 3px"><rect width="3" height="6" fill="lime"/></pattern>'
            f'<linearGradient id="gt" x2="2em" style="font-size: 6px">{STOPS}</linearGradient>'
            '<linearGradient id="gu" href="#gt" gradientUnits="userSpaceOnUse" style="font-size: 3px"/>'
            '</defs>', "")
CELLS = (
    ("filter-em", 2, 2, 'fill="red" filter="url(#fe)"', [(6, 6, 12, 8)]),
    ("primitive-em", 28, 2, 'fill="red" filter="url(#fp)"', [(32, 6, 8, 8)]),
    ("mask-box-em", 54, 2, 'fill="lime" mask="url(#mb)"', [(54, 2, 10, 20)]),
    ("gradient-em", 2, 28, 'fill="url(#ge)"', [(2, 28, 6, 20)]),
    ("pattern-em", 28, 28, 'fill="url(#pe)"', [(28, 28, 3, 20), (34, 28, 3, 20), (40, 28, 3, 20), (46, 28, 2, 20)]),
    ("gradient-template-em", 54, 28, 'fill="url(#gu)"', [(54, 28, 6, 20)]),
)

for name, x, y, paint, rects in CELLS:
    test = f'<g transform="translate({x} {y})" style="font-size: 10px"><rect width="20" height="20" {paint}/></g>'
    plain = "".join(lime(*r) for r in rects)
    unit.cell(name, (x - 1, y - 1, 22, 22), test, plain, plain.replace(' fill="lime"', ""))

unit.write()
