"""css-box-edges: the box-edge keywords of CSS Box 4 for SVG elements."""
from unitgen import Unit

unit = Unit("css-box-edges", 92, 36, """Each lime drawing fills its black
outline, and nothing red shows. The box edges of a rect 8 by 8 with a
stroke 4 wide: content-box and padding-box are its fill box, border-box and
margin-box its stroke box, as fill-box and stroke-box are, here clipping
with inset(0); view-box is the nearest viewport's size at the origin of its
coordinates, so a rect in a 10 by 10 svg is clipped to it. transform-box
takes no margin-box, so a translate of -50% after it stays of the fill
box.""")
SHAPE = '<rect x="{x}" y="{y}" width="8" height="8" fill="lime" stroke="lime" stroke-width="4" style="clip-path: inset(0) {box}"/>'
FILL = '<rect x="{x}" y="{y}" width="8" height="8"/>'
STROKE = '<rect x="{0}" y="{1}" width="12" height="12"/>'
for k, (name, boxes) in enumerate([("content", ["content-box", "padding-box"]), ("border", ["border-box", "margin-box"]), ("paint", ["fill-box", "stroke-box"])]):
    tests, plains, outlines = [], [], []

    for j, box in enumerate(boxes):
        x, y = 4 + (2 * k + j) * 14, 4
        shape = FILL.format(x=x, y=y) if box in ("content-box", "padding-box", "fill-box") else STROKE.format(x - 2, y - 2)
        tests.append(SHAPE.format(x=x, y=y, box=box))
        plains.append(shape.replace("/>", ' fill="lime"/>'))
        outlines.append(shape)

    unit.cell(name, (1 + 2 * k * 14, 1, 28, 15), "".join(tests), "".join(plains), "".join(outlines))
unit.cell("view-box", (1, 19, 16, 16), '<svg x="4" y="22" width="10" height="10" overflow="visible"><rect x="2" y="2" width="8" height="8" fill="lime" stroke="lime" stroke-width="4" style="clip-path: inset(0) view-box"/></svg>',
          '<rect x="4" y="22" width="10" height="10" fill="lime"/>', '<rect x="4" y="22" width="10" height="10"/>')
unit.cell("grammar", (19, 19, 20, 16), '<rect x="28" y="24" width="8" height="8" fill="lime" stroke="lime" stroke-width="4" style="transform-box: fill-box; transform-box: margin-box; transform: translate(-50%)"/>',
          '<rect x="22" y="22" width="12" height="12" fill="lime"/>', '<rect x="22" y="22" width="12" height="12"/>')
unit.write()
