"""mask-border-cells: the mask border of CSS Masking, with sources of one
alpha, so what each draws is a plain rect."""
from unitgen import Unit, rows

CLEAR, BLACK = "linear-gradient(transparent, transparent)", "linear-gradient(black, black)"
unit = Unit("mask-border-cells", 122, 0, """Each lime drawing fills its black
outline, and nothing red shows; each is a 16 by 12 rect masked by a mask
border. A transparent border image hides its border area and leaves the
middle, which is unmasked without fill and the image's with it; the width
in px, a percentage of the area, a number times the border width, 0 for
SVG, or auto, the slice's size, a gradient having none, so the border
width again; the outset grows the area, its values
copied as margin's; the repeat keywords, the second the first if none;
the mode alpha unless luminance, mask-mode and mask-type aside; an image
that does not load counts but masks nothing; a mask image masks too; the
shorthand resets what it leaves out.""")


def item(x, y, value):
    style, inner = value
    test = f'<rect x="{x}" y="{y}" width="16" height="12" fill="lime" style="{style}"/>'

    if inner is None:
        return test.replace('fill="lime"', 'fill="red"'), "", ""

    ix, iy, iw, ih = inner
    plain = '<rect x="%g" y="%g" width="%g" height="%g" fill="lime"/>' % (x + ix, y + iy, iw, ih)

    return test, plain, plain.replace(' fill="lime"', "")


full = (0, 0, 16, 12)
y = rows(unit, 2, [
    ("source-none", [("mask-border: none", full), ("mask-border-source: none; mask-border-width: 4px", full)]),
    ("slice-fill", [(f"mask-border: {CLEAR} 30% / 4px", (4, 4, 8, 4)), (f"mask-border: {CLEAR} 30% fill / 4px", None)]),
    ("width", [(f"mask-border: {CLEAR} 30% / 2px", (2, 2, 12, 8)), (f"mask-border: {CLEAR} 30% / 2px 4px", (4, 2, 8, 8))]),
    ("width-percent", [(f"mask-border: {CLEAR} 30% / 25%", (4, 3, 8, 6))]),
    ("width-number", [(f"mask-border: {CLEAR} 30% / 2", full)]),
    ("width-auto", [(f"mask-border: {CLEAR} 30%", full), (f"mask-border: {CLEAR} 30% / auto", full)]),
    ("outset", [(f"mask-border: {CLEAR} 30% / 3px / 1px", (2, 2, 12, 8)), (f"mask-border: {CLEAR} 30% / 3px / 1px 0", (3, 2, 10, 8)), (f"mask-border: {CLEAR} 30% / 3px / 0 0 1px", (3, 3, 10, 7))]),
    ("outset-invalid", [(f"mask-border: {CLEAR} 30% / 3px / -1px", full)]),
    ("repeat", [(f"mask-border: {CLEAR} 30% / 4px round", (4, 4, 8, 4)), (f"mask-border: {CLEAR} 30% / 4px stretch space", (4, 4, 8, 4)), (f"mask-border: {CLEAR} 30% / 4px repeat", (4, 4, 8, 4))]),
    ("mode", [(f"mask-border: {BLACK} 30% / 4px", full), (f"mask-border: {BLACK} 30% / 4px luminance", (4, 4, 8, 4)), (f"mask-border: {BLACK} 30% / 4px; mask-mode: luminance", full)]),
    ("invalid-source", [("mask-border: url(data:image/png;base64,AAAA) 30% / 4px", full)]),
    ("with-mask-image", [(f"mask-border: {CLEAR} 30% / 4px; mask-image: linear-gradient(to right, black 50%, transparent 50%)", (4, 4, 4, 4))]),
    ("shorthand-reset", [(f"mask-border-slice: 30%; mask-border-width: 4px; mask-border: {CLEAR}", full)]),
], 6, 20, 16, item)
unit.height = y + 1
unit.write()
