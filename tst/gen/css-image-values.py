"""css-image-values: image references and the <image> type of CSS Images 3,
as mask images."""
from urllib.parse import quote

from unitgen import Unit, png

BLACK = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg' width='4' height='4'><rect width='4' height='4'/></svg>")
unit = Unit("css-image-values", 92, 14, """Each lime drawing fills its black
outline, and nothing red shows. A url() image that cannot be decoded is an
invalid image, transparent, so a mask of it hides all; a fragment url is an
element of the document, a mask, or nothing, a transparent layer (D20);
one of another document is an element of it, which nothing loads (D15),
and so hides all too, unless it is an image, a data URI, whose fragment
names no element of ours. An <image> is a url or a gradient; a gradient
has no natural size and fills the size it is given.""")
unit.common('<mask id="m"><rect width="100" height="100" fill="white"/></mask>')


def rect(x, colour, style):
    return f'<rect x="{x}" y="2" width="8" height="10" fill="{colour}" style="{style}"/>'


def lime(x, w=8):
    return f'<rect x="{x}" y="2" width="{w}" height="10" fill="lime"/>'


unit.cell("invalid-image", (1, 1, 20, 12), rect(2, "red", "mask-image: url(data:image/png;base64,AAAA)") + rect(12, "red", "mask-image: url(data:image/png;base64,iVBORw0KGgo=)"), "")
unit.cell("fragment", (21, 1, 20, 12), rect(22, "lime", "mask-image: url(#m)") + rect(32, "red", "mask-image: url(#nothing)"), lime(22), lime(22).replace(' fill="lime"', ""))
unit.cell("other-document", (41, 1, 20, 12), rect(42, "red", "mask-image: url(other.svg#m)") + rect(52, "lime", f"mask-image: url('{BLACK}#m')"), lime(52), lime(52).replace(' fill="lime"', ""))
unit.cell("image-types", (61, 1, 10, 12), rect(62, "lime", f"mask-image: url('{BLACK}'), linear-gradient(transparent, transparent)"), lime(62), lime(62).replace(' fill="lime"', ""))
gradient = '<linearGradient id="{0}" gradientUnits="userSpaceOnUse" x1="72" x2="80"><stop stop-color="lime"/><stop offset="1" stop-color="lime" stop-opacity="0"/></linearGradient>'
unit.cell("gradient-box", (71, 1, 20, 12), f'<rect x="72" y="2" width="16" height="10" fill="lime" style="mask: linear-gradient(90deg, black, transparent) 0 0 / 8px 10px no-repeat"/>',
          gradient.format("gx") + '<rect x="72" y="2" width="8" height="10" fill="url(#gx)"/>', '<rect x="72" y="2" width="16" height="10"/>')
unit.write()
