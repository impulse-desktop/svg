"""render-root-rotate: a rotation about a point of its own on the outermost
svg, whose transform-origin, of SVG 2's user agent style sheet, is its
centre."""
from unitgen import Unit

unit = Unit("render-root-rotate", 16, 16, """rotate(90 8 8) on the outermost
svg turns about 8 8 from the transform-origin, the centre at 8 8, so about
16 16 in all: the lime bar drawn at 2 20 lands along the top, from 4 2 to
12 6, where its expected drawing has it; a turn about 8 8 alone would leave
it outside.""", root='transform="rotate(90 8 8)"', plain_root="", ground=False)
unit.cell("center", (0, 0, 16, 16), '<rect x="2" y="20" width="4" height="8" fill="lime"/>', '<rect x="4" y="2" width="8" height="4" fill="lime"/>')
unit.write()
