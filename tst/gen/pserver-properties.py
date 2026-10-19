"""pserver-properties: what properties reach paint servers, of SVG 2:
stop-color's initial value and inheritance; x1, x2 and a pattern's x, y,
width and height are attributes, which CSS does not set."""
from unitgen import Unit

unit = Unit("pserver-properties", 58, 16, """Each square matches its plain
twin in the expected drawing; no red. A stop with no stop-color is black,
even under a gradient or group whose stop-color is red, since stop-color is
not inherited, but inherit takes the gradient's. x1 and x2 set by CSS, and a
pattern's x, y, width and height set by CSS, do nothing: they are
attributes, not properties.""")
unit.common('<style>#css-vector { x1: 50%; x2: 0% } #css-tile { x: 1px; y: 1px; width: 2px; height: 2px }</style>'
            '<defs><g stop-color="red"><linearGradient id="black" stop-color="red"><stop offset="0"/><stop offset="1" stop-opacity=".5"/></linearGradient></g>'
            '<linearGradient id="inherit" stop-color="lime"><stop offset="0" style="stop-color: inherit"/><stop offset="1" stop-color="navy"/></linearGradient>'
            '<linearGradient id="css-vector"><stop offset="0" stop-color="navy"/><stop offset="1" stop-color="gold"/></linearGradient>'
            '<pattern id="css-tile" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="2" height="2" fill="lime"/></pattern></defs>',
            '<defs><linearGradient id="plain-black"><stop offset="0" stop-color="black"/><stop offset="1" stop-color="black" stop-opacity=".5"/></linearGradient>'
            '<linearGradient id="plain-inherit"><stop offset="0" stop-color="lime"/><stop offset="1" stop-color="navy"/></linearGradient>'
            '<linearGradient id="plain-vector"><stop offset="0" stop-color="navy"/><stop offset="1" stop-color="gold"/></linearGradient>'
            '<pattern id="plain-tile" patternUnits="userSpaceOnUse" width="4" height="4"><rect width="2" height="2" fill="lime"/></pattern></defs>')

for i, (name, paint) in enumerate((("stop-initial-black", "black"), ("stop-inherit", "inherit"), ("css-vector", "vector"), ("css-pattern-tile", "tile"))):
    x = 2 + i * 14
    test = "css-" + paint if paint in ("vector", "tile") else paint
    unit.cell(name, (x - 1, 1, 14, 14), f'<rect x="{x}" y="2" width="12" height="12" fill="url(#{test})"/>', f'<rect x="{x}" y="2" width="12" height="12" fill="url(#plain-{paint})"/>', f'<rect x="{x}" y="2" width="12" height="12"/>')

unit.write()
