"""image-object-fit: object-fit and object-position of CSS Images 3 on
images."""
from urllib.parse import quote

from unitgen import Unit, png

LIME = png([[(0, 1, 0, 1)] * 4] * 2)
BIG = png([[(0, 1, 0, 1)] * 40] * 20)
RATIO = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 2 1'><rect width='2' height='1' fill='lime'/></svg>")
NONE = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg'><rect width='100%' height='100%' fill='lime'/></svg>")
EMPTY = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg'/>")
unit = Unit("image-object-fit", 82, 0, """Each lime drawing fills its black
outline. An image's object-fit sizes its content by its natural ratio in
the box, fill leaving it to preserveAspectRatio, contain inside it, cover
over it and cut, none at its natural size, scale-down the smaller of none
and contain; object-position places it, by keywords, percentages of the
room left, or lengths; with a fit other than fill preserveAspectRatio does
nothing; an image larger than the box scales down; a document of a
viewBox alone has its measured size (D10), one of neither has no ratio
and is stretched, an empty one draws nothing; both properties inherit when asked.""")
k = 0
y0 = 2


def cell(name, items):
    global k, y0
    tests, plains = [], []
    first = k

    for style, (rx, ry, rw, rh), href, parent, extra in items:
        x, y = 2 + 16 * (k % 5), 2 + 16 * (k // 5)
        k += 1
        image = f'<image x="{x}" y="{y}" width="12" height="12"{extra}' + (f' style="{style}"' if style else "") + f' href="{href}"/>'
        tests.append(f'<g style="{parent}">{image}</g>' if parent else image)
        plains.append(f'<rect x="{x + rx}" y="{y + ry}" width="{rw}" height="{rh}" fill="lime"/>' if rw else "")

    last = k - 1
    top, bottom = 2 + 16 * (first // 5), 2 + 16 * (last // 5) + 16
    left = 2 + 16 * (first % 5) if first // 5 == last // 5 else 2
    right = 2 + 16 * (last % 5) + 16 if first // 5 == last // 5 else 82
    unit.cell(name, (left - 1, top - 1, right - left, bottom - top), "".join(tests), "".join(plains), "".join(plains).replace(' fill="lime"', ""))
    k = (k + 4) // 5 * 5 if name.endswith("!") else k


def one(style, box, href=LIME, parent="", extra=""):
    return (style, box, href, parent, extra)


cell("fill", [one("", (0, 3, 12, 6)), one("object-fit: fill", (0, 0, 12, 12), extra=' preserveAspectRatio="none"')])
cell("contain", [one("object-fit: contain", (0, 3, 12, 6)), one("object-fit: contain; object-position: left top", (0, 0, 12, 6))])
cell("cover", [one("object-fit: cover", (0, 0, 12, 12)), one("object-fit: cover; object-position: 0 100%", (0, 0, 12, 12))])
cell("none", [one("object-fit: none", (4, 5, 4, 2)), one("object-fit: none; object-position: 2px 3px", (2, 3, 4, 2)), one("object-fit: none", (4, 5, 4, 2), extra=' preserveAspectRatio="none"')])
cell("scale-down", [one("object-fit: scale-down", (4, 5, 4, 2)), one("object-fit: scale-down", (0, 3, 12, 6), BIG)])
cell("position", [one("object-fit: none; object-position: right 1px bottom 2px", (7, 8, 4, 2)), one("object-fit: none; object-position: 25% 100%", (2, 10, 4, 2))])
cell("ratio", [one("object-fit: none", (5, 5.5, 2, 1), RATIO), one("object-fit: cover", (0, 0, 12, 12), NONE), one("object-fit: cover", (0, 0, 0, 0), EMPTY)])
cell("inherit", [one("object-fit: inherit; object-position: inherit", (0, 0, 12, 12), parent="object-fit: cover; object-position: 100% 0")])
unit.height = 2 + 16 * ((k + 4) // 5) + 1
unit.write()
