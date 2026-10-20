"""render-root-background-stripes, render-root-background-order,
render-root-background-place, render-root-background-sides,
render-root-background-layer, render-root-background-image,
render-root-background-element, render-root-background-invalid,
render-root-background-refused, render-root-background-attribute and
render-root-background-box: the outermost svg's background images, the
canvas's (CSS Backgrounds 3, backgrounds of special elements), laid out in
its box and painted over all of the canvas."""
from unitgen import Unit

CANVAS = '<rect width="16" height="8" fill="white"/>'
LIME = "linear-gradient(lime, lime)"

unit = Unit("render-root-background-stripes", 16, 8, """A gradient of
background-image, 4 by 4 by background-size, repeats over the canvas from
its corner, over the white of background-color: a blue stripe 1 wide every
4.""", root='style="background-color: white; background-size: 4px 4px; background-image: linear-gradient(to right, blue 1px, transparent 1px)"', plain_root="", ground=False)
unit.cell("tiles", (0, 0, 16, 8), "", CANVAS + "".join(f'<rect x="{x}" width="1" height="8" fill="blue"/>' for x in range(0, 16, 4)))
unit.write()

unit = Unit("render-root-background-order", 16, 8, """The first image of
background-image is on top: lime 8 by 8 over blue, which a size of auto
spreads over the root's box; one no-repeat repeats for both layers.""", root=f'style="background-image: {LIME}, linear-gradient(blue, blue); background-size: 8px 8px, auto; background-repeat: no-repeat"', plain_root="", ground=False)
unit.cell("first-on-top", (0, 0, 16, 8), "", '<rect width="16" height="8" fill="blue"/><rect width="8" height="8" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-place", 16, 8, """background-position
of four words, two sides each with its offset: a lime square 4 by 4 its
right 2 from the root's right, its bottom 1 from the bottom.""", root=f'style="background-color: white; background-image: {LIME}; background-size: 4px 4px; background-repeat: no-repeat; background-position: right 2px bottom 1px"', plain_root="", ground=False)
unit.cell("four", (0, 0, 16, 8), "", CANVAS + '<rect x="10" y="3" width="4" height="4" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-sides", 16, 8, """background-position
of three words, which mask-position has not: right 3px, then top alone, its
offset 0; and center for the axis no side names.""", root=f'style="background-color: white; background-image: {LIME}, {LIME}; background-size: 4px 4px; background-repeat: no-repeat; background-position: right 3px top, center left 1px"', plain_root="", ground=False)
unit.cell("three", (0, 0, 16, 8), "", CANVAS + '<rect x="9" width="4" height="4" fill="lime"/><rect x="1" y="2" width="4" height="4" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-layer", 16, 8, """The background
shorthand sets each layer's image, position, size after a slash, repeat,
attachment and box in any order, and the colour in its final layer: lime 4
by 4 at the right, over white.""", root=f'style="background: {LIME} right 2px top / 4px 4px no-repeat fixed padding-box white"', plain_root="", ground=False)
unit.cell("shorthand", (0, 0, 16, 8), "", CANVAS + '<rect x="10" width="4" height="4" fill="lime"/>')
unit.write()

IMAGE = "data:image/svg+xml,%3Csvg xmlns=%22http://www.w3.org/2000/svg%22 width=%224%22 height=%224%22%3E%3Crect width=%224%22 height=%224%22 fill=%22lime%22/%3E%3C/svg%3E"
unit = Unit("render-root-background-image", 16, 8, """An image of a url
is laid out at its own size: an SVG document 4 by 4, centred and drawn
once; an image that does not load is a layer that draws nothing.""", root=f'style="background: url(nothing.png), url(&quot;{IMAGE}&quot;) center no-repeat white"', plain_root="", ground=False)
unit.cell("natural", (0, 0, 16, 8), "", CANVAS + '<rect x="6" y="2" width="4" height="4" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-element", 16, 8, """A url of a
fragment alone names an element of the document, not an image
(css-values-4, fragment URLs): the gradient element it names draws nothing
over the white.""", root='style="background-color: white; background-image: url(#g)"', plain_root="", ground=False)
unit.common('<linearGradient id="g"><stop stop-color="red"/></linearGradient>', "")
unit.cell("fragment", (0, 0, 16, 8), "", CANVAS)
unit.write()

unit = Unit("render-root-background-invalid", 16, 8, """A background
shorthand its grammar refuses is dropped whole, its colour too: a colour
in a layer before the final one, a box of mask's, two attachments, two
colours and no-clip leave the white of the background-color before
them.""", root='style="background-color: white; background: lime, none; background: none fill-box red; background: fixed scroll lime; background: lime red; background: no-clip lime"', plain_root="", ground=False)
unit.cell("dropped", (0, 0, 16, 8), "", CANVAS)
unit.write()

unit = Unit("render-root-background-refused", 16, 8, """background-position
values its grammar refuses leave the 2px 2px before them: an offset with no
side before it, center twice, two sides of one axis, and three sides; and
background-clip takes no no-clip.""", root=f'style="background-color: white; background-image: {LIME}; background-size: 4px 4px; background-repeat: no-repeat; background-position: 2px 2px; background-position: left 1px 2px; background-position: center center 1px; background-position: left 1px right; background-position: left top center; background-clip: no-clip"', plain_root="", ground=False)
unit.cell("positions", (0, 0, 16, 8), "", CANVAS + '<rect x="2" y="2" width="4" height="4" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-attribute", 16, 8, """SVG gives
background-color and background-image no presentation attribute
(styling.md): the root's attributes of those names paint no canvas.""", root='background-color="red" background-image="linear-gradient(red, red)"', plain_root="", ground=False)
unit.cell("none", (0, 0, 16, 8), '<rect x="8" width="8" height="8" fill="lime"/>', '<rect x="8" width="8" height="8" fill="lime"/>')
unit.write()

unit = Unit("render-root-background-box", 16, 8, """The root's boxes are
all its box (D39): background-origin's content-box places the lime square
at the corner, and background-clip's content-box cuts nothing from the
canvas, which the painting area covers.""", root=f'style="background-color: white; background-image: {LIME}, linear-gradient(blue, blue); background-size: 4px 4px, 2px 2px; background-repeat: no-repeat, repeat; background-origin: content-box; background-clip: content-box; padding: 2px"', plain_root="", ground=False)
unit.cell("canvas", (0, 0, 16, 8), "", '<rect width="16" height="8" fill="blue"/><rect width="4" height="4" fill="lime"/>')
unit.write()
