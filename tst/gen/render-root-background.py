"""render-root-background-opacity, render-root-background-blend,
render-root-background-shorthand and render-root-background-reset: the
outermost svg's background, the canvas's under it (CSS Backgrounds 3,
backgrounds of special elements), and the background shorthand's colour."""
from unitgen import Unit

CANVAS = '<rect width="16" height="8" fill="white"/>'

unit = Unit("render-root-background-opacity", 16, 8, """The outermost svg's
background is the canvas's, under the svg: its opacity of 0.5 fades the
lime square over the white, not the white.""", root='style="background-color: white" opacity="0.5"', plain_root="", ground=False)
unit.cell("canvas", (0, 0, 16, 8), '<rect x="8" width="8" height="8" fill="lime"/>', CANVAS + '<rect x="8" width="8" height="8" fill="lime" opacity="0.5"/>')
unit.write()

unit = Unit("render-root-background-blend", 16, 8, """The outermost svg's
content blends with its background, the canvas's: red in difference with
white is cyan.""", root='style="background-color: white; mix-blend-mode: difference"', plain_root="", ground=False)
unit.cell("canvas", (0, 0, 16, 8), '<rect x="8" width="8" height="8" fill="red"/>', CANVAS + '<rect x="8" width="8" height="8" fill="cyan"/>')
unit.write()

unit = Unit("render-root-background-shorthand", 16, 8, """The background
shorthand sets the canvas's colour from its final layer: white, past an
image and a repeat, over an earlier red background-color.""", root='style="background-color: red; background: url(#none) no-repeat white"', plain_root="", ground=False)
unit.cell("final-layer", (0, 0, 16, 8), '<rect x="8" width="8" height="8" fill="lime"/>', CANVAS + '<rect x="8" width="8" height="8" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-reset", 16, 8, """A background
shorthand with no colour resets the canvas's to transparent, its initial
value: none leaves no red of the earlier background-color.""", root='style="background-color: red; background: none"', plain_root="", ground=False)
unit.cell("no-colour", (0, 0, 16, 8), '<rect x="8" width="8" height="8" fill="lime"/>', '<rect x="8" width="8" height="8" fill="lime"/>')
unit.write()
