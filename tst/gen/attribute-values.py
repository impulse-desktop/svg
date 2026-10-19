"""attribute-values: invalid attribute values and deferred range checks of
SVG 2's data types."""
from unitgen import Unit

unit = Unit("attribute-values", 50, 16, """Each lime drawing fills its black
outline, and nothing red shows. An invalid presentation attribute is
ignored as an invalid declaration is (D16), so overflow="invalid" leaves the
user agent's hidden and the nested svg clips; a transform scaling past a
float's range and back is checked only as a whole, so it draws. A value of
white space alone, or of a second number after the one it takes, is
invalid: a fill, an opacity, a pathLength, a stop's offset, which is 0
then; color() needs its space, path() for d its string; a viewBox number
from calc() takes no unit.""")
unit.cell("invalid-overflow", (1, 1, 14, 14), '<svg x="4" y="4" width="8" height="8" overflow="invalid"><rect x="-2" y="-2" width="12" height="12" fill="lime"/></svg>',
          '<rect x="4" y="4" width="8" height="8" fill="lime"/>', '<rect x="4" y="4" width="8" height="8"/>')
unit.cell("range-deferred", (17, 1, 14, 14), '<rect width="10" height="10" fill="lime" transform="translate(18 3) scale(1e30) scale(1e-30)"/>',
          '<rect x="18" y="3" width="10" height="10" fill="lime"/>', '<rect x="18" y="3" width="10" height="10"/>')

# Row 2: an invalid fill inherits lime, an invalid opacity is 1, an invalid
# d property leaves the attribute.
for k, value in enumerate(["", " ", "color()", "color(1 2 3)", "("]):
    x = 2 + k * 8
    unit.cell(f"fill-{k + 1}", (x - 1, 17, 8, 8), f'<g fill="lime"><rect x="{x}" y="18" width="6" height="6" fill="{value}"/></g>', f'<rect x="{x}" y="18" width="6" height="6" fill="lime"/>', f'<rect x="{x}" y="18" width="6" height="6"/>')
unit.cell("opacity-pair", (41, 17, 8, 8), '<rect x="42" y="18" width="6" height="6" fill="lime" opacity="0 1"/>', '<rect x="42" y="18" width="6" height="6" fill="lime"/>', '<rect x="42" y="18" width="6" height="6"/>')
unit.cell("d-path-number", (1, 25, 8, 8), '<path d="M2 26h6v6h-6z" fill="lime" style="d: path(1)"/>', '<rect x="2" y="26" width="6" height="6" fill="lime"/>', '<rect x="2" y="26" width="6" height="6"/>')

# Row 3: an invalid pathLength is none, the dashes in user units; an invalid
# offset 0.
for k, value in enumerate([" ", "1 2"]):
    x = 10 + k * 8
    unit.cell(f"path-length-{k + 1}", (x - 1, 25, 8, 8), f'<path d="M{x} 29h6" stroke="lime" stroke-width="6" stroke-dasharray="3 3" pathLength="{value}"/>', f'<rect x="{x}" y="26" width="3" height="6" fill="lime"/>', f'<rect x="{x}" y="26" width="3" height="6"/>')
for k, value in enumerate([" ", "0.5 0.5"]):
    x = 26 + k * 8
    unit.cell(f"offset-{k + 1}", (x - 1, 25, 8, 8), f'<linearGradient id="o{k}"><stop offset="{value}" stop-color="navy"/><stop offset=".5" stop-color="lime"/></linearGradient><rect x="{x}" y="26" width="6" height="6" fill="url(#o{k})"/>',
              f'<linearGradient id="r{k}"><stop offset="0" stop-color="navy"/><stop offset=".5" stop-color="lime"/></linearGradient><rect x="{x}" y="26" width="6" height="6" fill="url(#r{k})"/>')
unit.cell("viewbox-length", (1, 33, 8, 8), '<svg x="2" y="34" width="6" height="6" viewBox="0 0 calc(12px) 12"><rect width="6" height="6" fill="lime"/></svg>', '<rect x="2" y="34" width="6" height="6" fill="lime"/>', '<rect x="2" y="34" width="6" height="6"/>')
unit.height = 42
unit.write()
