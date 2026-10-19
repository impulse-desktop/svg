"""attribute-values: invalid attribute values and deferred range checks of
SVG 2's data types."""
from unitgen import Unit

unit = Unit("attribute-values", 40, 16, """Each lime drawing fills its black
outline, and nothing red shows. An invalid presentation attribute is
ignored as an invalid declaration is (D16), so overflow="invalid" leaves the
user agent's hidden and the nested svg clips; a transform scaling past a
float's range and back is checked only as a whole, so it draws.""")
unit.cell("invalid-overflow", (1, 1, 14, 14), '<svg x="4" y="4" width="8" height="8" overflow="invalid"><rect x="-2" y="-2" width="12" height="12" fill="lime"/></svg>',
          '<rect x="4" y="4" width="8" height="8" fill="lime"/>', '<rect x="4" y="4" width="8" height="8"/>')
unit.cell("range-deferred", (17, 1, 14, 14), '<rect width="10" height="10" fill="lime" transform="translate(18 3) scale(1e30) scale(1e-30)"/>',
          '<rect x="18" y="3" width="10" height="10" fill="lime"/>', '<rect x="18" y="3" width="10" height="10"/>')
unit.write()
