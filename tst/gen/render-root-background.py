"""render-root-background-opacity and render-root-background-blend: the
outermost svg's background, the canvas's under it (CSS Backgrounds 3,
backgrounds of special elements)."""
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
