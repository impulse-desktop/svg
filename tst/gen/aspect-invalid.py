"""aspect-invalid: a preserveAspectRatio value outside its grammar."""
from unitgen import Unit

unit = Unit("aspect-invalid", 102, 16, """Each viewport, 8 by 12, shows the
viewBox 0 0 10 10, lime with a navy top left quarter, within the black
outline its value gives. A value outside the grammar is as if not given,
xMidYMid meet, whole: a misspelt or miscased alignment, a word after it,
slice after a bad alignment (Chromium, librsvg and resvg alike). SVG 1.1's
defer before an alignment is read past; glued to it or alone it is
invalid. Spaces around and between the words do not count, and none takes
slice and stretches.""")

# The lime box and the navy quarter each value draws, by name of its
# transform.
DRAWN = {
    "default": ((0, 2, 8, 8), (0, 2, 4, 4)),
    "xMinYMin": ((0, 0, 8, 8), (0, 0, 4, 4)),
    "xMinYMin slice": ((0, 0, 8, 12), (0, 0, 6, 6)),
    "none": ((0, 0, 8, 12), (0, 0, 4, 6)),
}
CELLS = [
    ("misspelt", "xMadYMid", "default"),
    ("misspelt-slice", "xMadYMid slice", "default"),
    ("trailing-word", "xMinYMin foo", "default"),
    ("case", "xMinYmin", "default"),
    ("bad-slice", "xMinYMin slicex", "default"),
    ("defer", "defer xMinYMin", "xMinYMin"),
    ("defer-glued", "deferxMinYMin", "default"),
    ("defer-alone", "defer", "default"),
    ("spaces", " xMinYMin  slice ", "xMinYMin slice"),
    ("none-slice", "none slice", "none"),
]


def rect(x, y, box, fill=None):
    paint = f' fill="{fill}"' if fill else ""
    return f'<rect x="{x + box[0]}" y="{y + box[1]}" width="{box[2]}" height="{box[3]}"{paint}/>'


for k, (name, value, drawn) in enumerate(CELLS):
    x, y = 2 + k * 10, 2
    lime, navy = DRAWN[drawn]
    test = f'<svg x="{x}" y="{y}" width="8" height="12" viewBox="0 0 10 10" preserveAspectRatio="{value}"><rect width="10" height="10" fill="lime"/><rect width="5" height="5" fill="navy"/></svg>'
    unit.cell(name, (x - 1, y - 1, 10, 14), test, rect(x, y, lime, "lime") + rect(x, y, navy, "navy"), rect(x, y, lime))

unit.write()
