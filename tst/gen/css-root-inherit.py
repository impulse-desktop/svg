"""css-root-inherit: inherit on the root element of CSS Cascade 5."""
from unitgen import Unit

unit = Unit("css-root-inherit", 10, 10, """The square is black and fills its
outline: the root inherits the initial value of fill and of opacity,
having no parent, and the square inherits the root's fill.""", root='style="fill: inherit; opacity: inherit"')
unit.cell("root", (1, 1, 8, 8), '<rect x="2" y="2" width="6" height="6"/>', '<rect x="2" y="2" width="6" height="6" fill="black"/>', '<rect x="2" y="2" width="6" height="6"/>')
unit.write()
