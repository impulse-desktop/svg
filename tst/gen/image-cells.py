"""image-cells: an image's position and clip, of SVG 2's embedded content."""
from urllib.parse import quote

from unitgen import Unit, png

LIME = png([[(0, 1, 0, 1)] * 4] * 4)
OVER = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 4 4' overflow='visible'><rect x='-4' y='-4' width='12' height='12' fill='lime'/></svg>")
# A checker of lime and navy pixels.
CHECKER = [[(0, 1, 0, 1) if (x + y) % 2 else (0, 0, .5, 1) for x in range(4)] for y in range(4)]
unit = Unit("image-cells", 40, 168, """Each lime drawing fills its black outline.
CSS positioning does not move an image. By default, an image clips what
its document draws past its positioning rectangle. With overflow visible
it does not; overflow on the referenced root independently clips to the
image-rendering rectangle, which slice can make larger than the image's
positioning rectangle. A raster at its own
size on whole pixels gives its pixels back as they are, as bilinear
resampling would, the least image-rendering auto may do. An SVG's explicit
root size gives its intrinsic aspect ratio even without a viewBox; the
referencing image's preserveAspectRatio fits and aligns it, while the
embedded root's preserveAspectRatio is ignored.""")
unit.cell("css-position", (1, 1, 10, 10), f'<image x="2" y="2" width="8" height="8" href="{LIME}" style="position: relative; top: 4px; left: 4px; margin: 4px"/>',
          '<rect x="2" y="2" width="8" height="8" fill="lime"/>', '<rect x="2" y="2" width="8" height="8"/>')
unit.cell("image-overflow", (13, 1, 18, 10), f'<image x="20" y="2" width="8" height="8" href="{OVER}"/>', '<rect x="20" y="2" width="8" height="8" fill="lime"/>', '<rect x="20" y="2" width="8" height="8"/>')
unit.cell("natural-size", (31, 1, 6, 6), f'<image x="32" y="2" href="{png(CHECKER)}"/>',
          "".join(f'<rect x="{32 + x}" y="{2 + y}" width="1" height="1" fill="{"lime" if (x + y) % 2 else "navy"}"/>' for y in range(4) for x in range(4)), "")

HIDDEN = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 4 4' overflow='hidden'><rect x='-4' y='-4' width='12' height='12' fill='lime'/></svg>")
WIDE = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 4 2' overflow='hidden'><rect width='4' height='2' fill='lime'/></svg>")

def image(x, y, uri, overflow, aspect="none"):
    return f'<image x="{x}" y="{y}" width="4" height="4" overflow="{overflow}" preserveAspectRatio="{aspect}" href="{uri}"/>'

def rect(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'

unit.cell("visible-both", (1, 13, 18, 14), image(6, 18, OVER, "visible"), rect(2, 14, 12, 12))
unit.cell("hidden-image", (21, 13, 18, 14), image(26, 18, OVER, "hidden"), rect(26, 18, 4, 4))
unit.cell("hidden-root", (1, 33, 18, 14), image(6, 38, HIDDEN, "visible"), rect(6, 38, 4, 4))
unit.cell("slice-visible", (21, 33, 18, 14), image(26, 38, WIDE, "visible", "xMidYMid slice"), rect(24, 38, 8, 4))
unit.cell("slice-hidden", (1, 53, 18, 14), image(6, 58, WIDE, "hidden", "xMidYMid slice"), rect(6, 58, 4, 4))
unit.cell("visible-transformed", (21, 53, 18, 14), '<g transform="translate(30 58) rotate(90)">' + image(0, 0, OVER, "visible") + '</g>', rect(22, 54, 12, 12))
WIDE_RASTER = png([[(0, 1, 0, 1)] * 4] * 2)
unit.cell("raster-slice-visible", (1, 73, 18, 14), image(6, 78, WIDE_RASTER, "visible", "xMidYMid slice"), rect(4, 78, 8, 4))
unit.cell("raster-slice-hidden", (21, 73, 18, 14), image(26, 78, WIDE_RASTER, "hidden", "xMidYMid slice"), rect(26, 78, 4, 4))

# A two-colour 8x4 source has a 2:1 intrinsic ratio. In a 16x16 box,
# meet gives 16x8, slice gives 32x16, and none gives 16x16. Plain rectangles
# below give these independently calculated sizes and alignment offsets.
for i, (name, aspect, extra, box, navy_width) in enumerate([
    ("svg-size-meet", "", "", (0, 4, 16, 8), 4),
    ("svg-size-min-meet", "xMinYMin meet", "", (0, 0, 16, 8), 4),
    ("svg-size-max-meet", "xMaxYMax meet", "", (0, 8, 16, 8), 4),
    ("svg-size-none", "none", "", (0, 0, 16, 16), 4),
    ("svg-size-slice", "xMidYMid slice", "", (0, 0, 16, 16), 0),
    ("svg-size-viewbox", "", ' viewBox="0 0 8 4"', (0, 4, 16, 8), 4),
    ("svg-size-root-ratio-ignored", "", ' preserveAspectRatio="xMaxYMax slice"', (0, 4, 16, 8), 4),
]):
    x, y = 2 + i % 2 * 20, 90 + i // 2 * 20
    content = (f'<svg xmlns="http://www.w3.org/2000/svg" width="8" height="4"{extra}>'
               '<rect width="8" height="4" fill="lime"/>'
               '<rect width="2" height="4" fill="navy"/></svg>')
    uri = "data:image/svg+xml," + quote(content)
    ratio = f' preserveAspectRatio="{aspect}"' if aspect else ""
    test = f'<image x="{x}" y="{y}" width="16" height="16" href="{uri}"{ratio}/>'
    rx, ry, rw, rh = box
    expected = rect(x + rx, y + ry, rw, rh)
    if navy_width:
        expected += f'<rect x="{x + rx}" y="{y + ry}" width="{navy_width}" height="{rh}" fill="navy"/>'
    unit.cell(name, (x - 1, y - 1, 18, 18), test, expected)
unit.write()
