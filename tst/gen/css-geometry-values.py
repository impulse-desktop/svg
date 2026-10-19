"""css-geometry-values: geometry properties declared alike on many
elements, each value its own."""
from unitgen import Unit

unit = Unit("css-geometry-values", 10, 10, """The lime square fills its black
outline. Four hundred rects of no width each take x from a style attribute
of a value of its own, and a path takes d from a declaration after them:
each value set by a declaration is held as the attribute it sets, read as
CSS, however many there are.""")
rects = "".join(f'<rect width="0" height="6" style="x: {k}px"/>' for k in range(400))
unit.cell("many", (1, 1, 8, 8), rects + '<path fill="lime" style="d: path(\'M2 2h6v6h-6z\')"/>', '<rect x="2" y="2" width="6" height="6" fill="lime"/>', '<rect x="2" y="2" width="6" height="6"/>')
unit.write()
