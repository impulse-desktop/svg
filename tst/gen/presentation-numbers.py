"""presentation-numbers: lengths as plain numbers, which SVG 2 lets a
presentation attribute give and not the property, but which a declaration
gives too in the files that exist, as renderers read them (D43)."""
from unitgen import Unit


def lime(x, y, w=6, h=6):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


unit = Unit("presentation-numbers", 76, 34, """Each lime drawing fills its
black outline. A length may be a plain number, of px, in a presentation
attribute, as SVG 2 has it, and in a declaration too, in a style attribute
or a style sheet, as the renderers of real files read it (D43): x, width,
r, cx, font-size and transform-origin; 0, a number of px and stroke-width's
numbers as anywhere. A style sheet's width or height of a plain number is
dropped, as Chromium and Firefox drop it, and the attribute's stays; 0 is a
length there too, and nothing draws.""")
unit.common("<style>.pn { width: 20; height: 2px } .pr { r: 3 } .pz { height: 0 }</style>", "")
unit.cell("declaration", (1, 1, 48, 8), '<rect x="30" y="2" width="1" height="6" fill="lime" style="x: 2; width: 6"/>' + '<rect x="10" y="2" width="0.25em" height="6" fill="lime" style="font-size: 24"/>'
          + '<circle cx="40" cy="5" r="1" fill="lime" style="r: 3; cx: 21"/>' + '<g transform="translate(26 2)"><rect width="6" height="6" fill="lime" transform="rotate(180)" style="transform-origin: 3 3"/></g>',
          lime(2, 2) + lime(10, 2) + '<circle cx="21" cy="5" r="3" fill="lime"/>' + lime(26, 2),
          lime(2, 2) + lime(10, 2) + '<circle cx="21" cy="5" r="3"/>' + lime(26, 2))
unit.cell("accepted", (1, 13, 48, 8), lime(20, 14).replace("/>", ' style="x: 2px"/>') + '<rect x="20" y="14" width="2" height="6" fill="lime" style="x: 0; width: 6px" transform="translate(10 0)"/>'
          + '<rect x="19" y="15" width="4" height="4" fill="none" stroke="lime" stroke-width="5" style="stroke-width: 2"/>' + lime(40, 14).replace("/>", ' style="x: calc(1px * 28)"/>'),
          lime(2, 14) + lime(10, 14) + '<rect x="19" y="15" width="4" height="4" fill="none" stroke="lime" stroke-width="2"/>' + lime(28, 14),
          lime(2, 14) + lime(10, 14) + lime(18, 14) + lime(28, 14))
unit.cell("sheet", (1, 25, 22, 8), '<rect class="pn" x="2" y="26" width="6" height="6" fill="lime"/>', lime(2, 26, 6, 2), lime(2, 26, 6, 2))
unit.cell("sheet-radius", (27, 25, 22, 8), '<circle class="pr" cx="31" cy="29" r="1" fill="lime"/>', '<circle cx="31" cy="29" r="3" fill="lime"/>', '<circle cx="31" cy="29" r="3"/>')
unit.cell("sheet-zero", (53, 25, 22, 8), '<rect class="pz" x="54" y="26" width="6" height="6" fill="red"/>', "", '<rect x="54" y="26" width="6" height="6"/>')
unit.write()
