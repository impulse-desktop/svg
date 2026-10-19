"""transform-non-invertible: transform lists and the transforms that hide
what they cannot invert, CSS Transforms 1."""
from unitgen import Unit

unit = Unit("transform-non-invertible", 64, 16, """Each lime drawing fills
its black outline, no red. A transform list is the product of its
functions, scale(2) scale(.5) none at all; one that cannot be inverted, a
scale of zero, a singular matrix, in the attribute or the property, hides
the element and its content.""")
unit.cell("product", (1, 1, 10, 14), '<rect x="2" y="2" width="8" height="12" fill="lime" transform="scale(2) scale(.5)"/>', '<rect x="2" y="2" width="8" height="12" fill="lime"/>', '<rect x="2" y="2" width="8" height="12"/>')
unit.cell("hidden", (13, 1, 50, 14), '<rect x="14" y="2" width="8" height="12" fill="red" transform="scale(0)"/><rect x="26" y="2" width="8" height="12" fill="red" transform="matrix(1 2 2 4 0 0)"/>'
          '<g transform="scale(1 0)"><rect x="38" y="2" width="8" height="12" fill="red"/></g><rect x="50" y="2" width="8" height="12" fill="red" style="transform: scale(0) scale(1)"/>', "")
unit.write()
