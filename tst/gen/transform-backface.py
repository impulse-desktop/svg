"""transform-backface: backface-visibility of CSS Transforms 2 on a flat
drawing."""
from unitgen import Unit

unit = Unit("transform-backface", 122, 14, """Each lime drawing fills its
black outline, no red. backface-visibility hidden hides an element its 3D
transform turns past a right angle, by y or by x, by a transform or the
rotate property, its m33 negative; one turned less, or mirrored in 2D,
still shows, as does any whose backface is visible, set so, unset or of an
unknown word; the attribute of its name is no presentation attribute; a
child inheriting hidden is hidden.""")
CENTRE = "transform-box: fill-box; transform-origin: center; "


def rect(x, colour, style, attributes=""):
    return f'<rect x="{x}" y="2" width="8" height="10" fill="{colour}" {attributes}style="{CENTRE}{style}"/>'


hidden = [(2, "transform: rotateY(180deg); backface-visibility: hidden"), (22, "transform: rotateY(170deg); backface-visibility: hidden"), (42, "transform: rotateX(180deg); backface-visibility: hidden"),
          (62, "rotate: y 180deg; backface-visibility: hidden")]
unit.cell("hidden", (1, 1, 70, 12), "".join(rect(x, "red", style) for x, style in hidden)
          + f'<g style="backface-visibility: hidden">{rect(102, "red", "transform: rotateY(180deg); backface-visibility: inherit")}</g>', "")
shown = [(12, "transform: rotateY(180deg)", 8), (32, "transform: rotateY(60deg); backface-visibility: hidden", 4), (52, "transform: scale(-1, 1); backface-visibility: hidden", 8),
         (72, "transform: rotateY(180deg); backface-visibility: visible", 8), (82, "transform: rotateY(180deg); backface-visibility: unset", 8), (112, "transform: rotateY(180deg); backface-visibility: bogus", 8)]
plain = "".join(f'<rect x="{x + (8 - w) / 2:g}" y="2" width="{w}" height="10" fill="lime"/>' for x, _, w in shown) + '<rect x="92" y="2" width="8" height="10" fill="lime"/>'
unit.cell("shown", (11, 1, 110, 12), "".join(rect(x, "lime", style) for x, style, _ in shown) + rect(92, "lime", "transform: rotateY(180deg)", 'backface-visibility="hidden" '), plain, plain.replace(' fill="lime"', ""))
unit.write()
