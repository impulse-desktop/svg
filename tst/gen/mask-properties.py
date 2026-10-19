"""mask-properties: mask-image, mask-mode, the mask shorthand, the mask
element's geometry and clip-path on use, of CSS Masking."""
from unitgen import Unit, png, rows

G = "linear-gradient(black, black)"
LIME = png([[(0, 1, 0, 1)] * 4] * 2)
unit = Unit("mask-properties", 122, 0, """Each lime drawing fills its black
outline, and nothing red shows. none in a list of mask images is a
transparent layer, so intersected with another it leaves nothing and
added to one leaves that, and none alone, or only none, masks nothing
(D23); a mask image makes a stacking context, so a
blend in it meets nothing behind; mask-mode takes alpha, luminance or the
source's, an image's alpha; one box in the shorthand is the origin and the
clip, two the origin then the clip, no-clip clips nothing; a layer of a mask
element takes none of the position, size, repeat, origin and clip. A mask
element's region is in bounding box units unless userSpaceOnUse, its x -10%
when only y is given, its width 120% when only x is; mask-type is not
inherited. clip-path applies to use, whose x and y move its user space,
and a clipPath in user space units is in the user space of what it clips.
A stroke box takes no dashes nor the stroke's opacity, is a use's
content's, and an image's is its bounding box.""")
unit.common('''<defs><mask id="w"><rect x="-50" y="-50" width="300" height="300" fill="white"/></mask>
<mask id="u" maskUnits="userSpaceOnUse" x="4" y="4" width="8" height="200"><rect x="-50" y="-50" width="300" height="300" fill="white"/></mask>
<mask id="ox" y="0"><rect x="-50" y="-50" width="300" height="300" fill="white"/></mask>
<mask id="ow" x="0"><rect x="-50" y="-50" width="300" height="300" fill="white"/></mask>
<g mask-type="alpha"><mask id="ti"><rect x="-50" y="-50" width="300" height="300" fill="white"/></mask></g>
<rect id="r" width="12" height="12" fill="lime"/>
<clipPath id="cu"><rect x="2" y="2" width="6" height="12"/></clipPath>
<rect id="rs" x="1" y="1" width="6" height="6" fill="lime" stroke="lime" stroke-width="2"/></defs>''', "")


def item(x, y, value):
    test, plain = value
    keys = dict(x=x, y=y, x2=x + 2, y2=y + 2, x4=x + 4, y4=y + 4, xm=round(x + 2 - 1.2, 4), ym=round(y + 2 - 1.2, 4))
    test, plain = test.format(**keys), plain.format(**keys)

    return test, plain, plain.replace(' fill="lime"', "").replace(' fill="blue"', "").replace(' fill="yellow"', "")


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


square = '<rect x="{x2}" y="{y2}" width="12" height="12" fill="%s" style="%s"/>'
stroked = '<rect x="{x4}" y="{y4}" width="8" height="8" fill="lime" stroke="lime" stroke-width="4" style="%s"/>'
y = rows(unit, 2, [
    ("none-layer", [(square % ("red", f"mask-image: none, {G}; mask-composite: intersect"), ""), (square % ("red", f"mask-image: {G}, none; mask-composite: intersect"), ""),
                    (square % ("red", f"mask: none intersect, {G}"), ""), (square % ("lime", f"mask-image: none, {G}"), lime("{x2}", "{y2}", 12, 12)),
                    (square % ("lime", "mask-image: none"), lime("{x2}", "{y2}", 12, 12)), (square % ("lime", "mask-image: none, none"), lime("{x2}", "{y2}", 12, 12))]),
    ("stacking-context", [('<rect x="{x2}" y="{y2}" width="12" height="12" fill="blue"/><g style="mask-image: ' + G + '"><rect x="{x4}" y="{y4}" width="8" height="8" fill="yellow" style="mix-blend-mode: multiply"/></g>',
                           '<rect x="{x2}" y="{y2}" width="12" height="12" fill="blue"/><rect x="{x4}" y="{y4}" width="8" height="8" fill="yellow"/>')]),
    ("mode", [(square % ("lime", "mask: linear-gradient(white, white)"), lime("{x2}", "{y2}", 12, 12)), (square % ("red", "mask: linear-gradient(black, black) luminance"), ""),
              (square % ("lime", "mask: linear-gradient(black, black) alpha"), lime("{x2}", "{y2}", 12, 12)), (square % ("lime", "mask: url(#w) alpha"), lime("{x2}", "{y2}", 12, 12))]),
    ("shorthand-boxes", [(stroked % f"mask: {G} 0 0 / 4px 4px no-repeat stroke-box", lime("{x2}", "{y2}", 4, 4)), (stroked % f"mask: {G} 0 0 / 4px 4px no-repeat stroke-box fill-box", lime("{x4}", "{y4}", 2, 2)),
                         (stroked % f"mask: {G} -2px -2px / 100% 100% no-repeat fill-box no-clip", lime("{x2}", "{y2}", 8, 8))]),
    ("mask-element-layer", [(square % ("lime", "mask: url(#w) 4px 4px / 2px 2px no-repeat content-box"), lime("{x2}", "{y2}", 12, 12))]),
    ("mask-units", [('<rect x="0" y="{y2}" width="16" height="12" fill="lime" mask="url(#u)"/>', lime(4, "{y2}", 8, 12))]),
    ("mask-defaults", [('<rect x="{x2}" y="{y2}" width="12" height="12" fill="lime" stroke="lime" stroke-width="4" mask="url(#ox)"/>', '<rect x="{xm}" y="{y2}" width="14.4" height="14" fill="lime"/>'),
                       ('<rect x="{x2}" y="{y2}" width="12" height="12" fill="lime" stroke="lime" stroke-width="4" mask="url(#ow)"/>', '<rect x="{x2}" y="{ym}" width="14" height="14.4" fill="lime"/>')]),
    ("mask-type-inherit", [('<rect x="{x2}" y="{y2}" width="12" height="12" fill="lime" mask="url(#ti)"/>', lime("{x2}", "{y2}", 12, 12))]),
    ("use-clip", [('<use href="#r" x="{x2}" y="{y2}" style="clip-path: inset(0 50% 0 0)"/>', lime("{x2}", "{y2}", 6, 12)), ('<use href="#r" x="{x2}" y="{y2}" clip-path="url(#cu)"/>', lime("{x4}", "{y4}", 6, 10))]),
    ("user-space-clip", [('<g transform="translate({x} {y})"><rect width="16" height="16" fill="lime" clip-path="url(#cu)"/></g>', lime("{x2}", "{y2}", 6, 12))]),
    ("stroke-box-paint", [('<rect x="{x2}" y="{y2}" width="6" height="6" fill="lime" stroke="lime" stroke-width="2" stroke-dasharray="1 3" stroke-dashoffset="1" stroke-opacity=".5" style="transform-box: stroke-box; transform: translate(50%)"/>',
                           '<rect x="{x2}" y="{y2}" width="6" height="6" fill="lime" stroke="lime" stroke-width="2" stroke-dasharray="1 3" stroke-dashoffset="1" stroke-opacity=".5" transform="translate(4 0)"/>')]),
    ("stroke-box-use", [('<use href="#rs" x="{x}" y="{y}" style="transform-box: stroke-box; transform: translate(50%)"/>', '<rect x="{x}" y="{y}" width="6" height="6" fill="lime" stroke="lime" stroke-width="2" transform="translate(5 1)"/>')]),
    ("stroke-box-image", [('<image x="{x2}" y="{y2}" width="8" height="4" stroke="lime" stroke-width="10" preserveAspectRatio="none" href="' + LIME + '" style="transform-box: stroke-box; transform: translate(50%)"/>',
                           '<rect x="{x2}" y="{y2}" width="8" height="4" fill="lime" transform="translate(4 0)"/>')]),
], 6, 20, 20, item)
unit.height = y + 1
unit.write()
