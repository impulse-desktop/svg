"""image-fragment-views: an SVG image's view by its fragment, of SVG 2's
linking, where image-fragments leaves off: no fragment, a parameter an
svgView() leaves out, fragments malformed by repeating or by naming two
views, the transform's origin and time segments."""
from urllib.parse import quote

from unitgen import Unit

unit = Unit("image-fragment-views", 66, 18, """Lime everywhere, no red. An
SVG image with no fragment shows its root's own viewBox, and so does one
whose svgView() names no viewBox. An svgView() naming a parameter twice and
a fragment of two views at once are malformed, no view: the whole image
draws, and so does one with an empty part, a part unclosed, an alignment or
a transform that does not parse; a view of no height or width draws
nothing. An svgView() transform scales about the origin, no
transform-origin of its own; a time segment before a space segment or after
a bare name leaves it the view.""")

LIME = '<rect x="{}" y="{}" width="6" height="6" fill="lime"/>'
OUTLINE = '<rect x="{}" y="{}" width="6" height="6"/>'


def image(x, y, document, fragment):
    href = "data:image/svg+xml," + quote(document, safe="") + fragment.replace("&", "&amp;")

    return f'<image x="{x}" y="{y}" width="6" height="6" href="{href}"/>'


# Lime in the root's viewBox, red past it.
VIEWBOX = "<svg xmlns='http://www.w3.org/2000/svg' width='40' height='40' viewBox='0 0 10 10'><rect width='20' height='20' fill='red'/><rect width='10' height='10' fill='lime'/></svg>"
# Lime all over the whole image; each fragment, if taken, would show the
# empty square right of it.
WHOLE = "<svg xmlns='http://www.w3.org/2000/svg' width='20' height='20'><rect width='20' height='20' fill='lime'/></svg>"
# Lime in the top left quarter, red past it, and a view of that quarter.
SPLIT = "<svg xmlns='http://www.w3.org/2000/svg' width='20' height='20'><view id='v' viewBox='0 0 10 10'/><rect width='20' height='20' fill='red'/><rect width='10' height='10' fill='lime'/></svg>"

for i, (name, document, fragment) in enumerate((
        ("no-fragment", VIEWBOX, ""),
        ("omitted-kept", VIEWBOX, "#svgView(preserveAspectRatio(none))"),
        ("repeated", WHOLE, "#svgView(viewBox(20,0,10,10);viewBox(20,0,10,10))"),
        ("two-views", WHOLE, "#xywh=20,0,10,10&svgView(viewBox(20,0,10,10))"),
        ("no-origin", SPLIT, "#svgView(viewBox(0,0,20,20);transform(scale(2)))"),
        ("time-first", SPLIT, "#t=5&xywh=0,0,10,10"),
        ("bare-name-time", SPLIT, "#v&t=5"),
        ("empty-part", WHOLE, "#xywh=20,0,10,10&"),
        ("aspect-meet", SPLIT, "#svgView(viewBox(0,0,10,10);preserveAspectRatio(xMidYMid meet))"),
        ("aspect-slice", SPLIT, "#svgView(preserveAspectRatio(xMinYMax  slice);viewBox(0,0,10,10))"),
        ("aspect-invalid", WHOLE, "#svgView(viewBox(20,0,10,10);preserveAspectRatio(xMinYTop))"),
        ("aspect-bad-meet", WHOLE, "#svgView(viewBox(20,0,10,10);preserveAspectRatio(xMinYMin fit))"),
        ("part-unclosed", WHOLE, "#svgView(viewBox(20,0,10,10)"),
        ("transform-invalid", WHOLE, "#svgView(viewBox(20,0,10,10);transform(spin(5)))"),
        ("view-no-height", WHOLE, "#svgView(viewBox(0,0,10,0))"),
        ("media-no-width", WHOLE, "#xywh=0,0,0,10"))):
    x, y = 2 + i % 8 * 8, 2 + i // 8 * 8
    # A view of no width or height draws nothing.
    plain = "" if name in ("view-no-height", "media-no-width") else LIME.format(x, y)
    unit.cell(name, (x - 1, y - 1, 8, 8), image(x, y, document, fragment), plain, OUTLINE.format(x, y))

unit.write()
