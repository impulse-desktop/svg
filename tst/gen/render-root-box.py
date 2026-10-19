"""render-root-box: the outermost svg's CSS box, which the renderer, with no
host to lay it out, does not draw (D39)."""
from unitgen import Unit

unit = Unit("render-root-box", 32, 16, """The lime square fills its black
outline, and nothing red shows. The outermost svg has no box of CSS drawn
around its viewport: its border, padding, margin, outline and shadow draw
nothing and move nothing.""", root='style="border: 4px solid red; padding: 3px; margin: 2px; outline: 2px solid red; box-shadow: 4px 4px red"', plain_root="")
unit.cell("box", (0, 0, 32, 16), '<rect x="8" y="4" width="16" height="8" fill="lime"/>', '<rect x="8" y="4" width="16" height="8" fill="lime"/>', '<rect x="8" y="4" width="16" height="8"/>')
unit.write()
