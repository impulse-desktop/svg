"""css-length-units: the length units of css-values-4 as bar widths."""
from unitgen import Unit, rows

unit = Unit("css-length-units", 100, 0, """Each lime bar fills its black
outline, and nothing red shows; every bar is 12px. The length units of
css-values-4, ASCII case-insensitive: the absolute ones by their fixed
ratios to px, em and rem of the font size, ex and ch half an em and cap,
ic and lh one em as a font without metrics has them (D29), their root
forms of the root's, the viewport units of the 100 by 50 viewport, the
small, large and dynamic ones all the same, a percentage of the width, a
number as px. An em or an ex in font-size is of the parent's font size;
an em inherits as the length it computes to. A unit not known, digits
after a unit or white space before it is no length; zero needs no unit;
a negative width is out of its range and invalid.""", root='font-size="8"')


def bar(x, y, width):
    if width.startswith("<"):
        return width.format(x=x, y=y), f'<rect x="{x}" y="{y}" width="12" height="2" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="2"/>'

    if width.startswith("!"):
        return f'<rect x="{x}" y="{y}" width="{width[1:]}" height="2" fill="red"/>', "", ""

    return f'<rect x="{x}" y="{y}" width="{width}" height="2" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="2" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="2"/>'


root = '<g font-size="16"><rect x="{x}" y="{y}" width="%s" height="2" fill="lime"/></g>'
end = rows(unit, 2, [
    ("absolute", ["12px", ".125in", ".3175cm", "3.175mm", "12.7q", "9pt", ".75pc"]),
    ("case", ["12PX", "12.7Q", "1.5EM", "3Ex", "1.5REM", "12VW"]),
    ("number", ["12"]),
    ("zero", ['<rect x="{x}" y="{y}" width="12" height="2" fill="red" style="width: 0"/><rect x="{x}" y="{y}" width="12" height="2" fill="lime" stroke="red" style="stroke-width: 0"/>']),
    ("range", ['<rect x="{x}" y="{y}" width="12" height="2" fill="lime" style="width: -5px"/>', '<rect x="{x}" y="{y}" width="12" height="2" fill="lime" stroke="red" stroke-width="0" style="stroke-width: -1px"/>']),
    ("em", ["1.5em", '<g font-size="16"><rect x="{x}" y="{y}" width="1em" height="2" font-size=".75em" fill="lime"/></g>']),
    ("computed", ['<g font-size="2" style="stroke-width: 1em"><path d="M{x} {y}m0 1h12" stroke="lime" font-size="8"/></g>']),
    ("metrics", ["3ex", "3ch", "1.5cap", "1.5ic", "1.5lh", '<g font-size="16"><rect x="{x}" y="{y}" width="1em" height="2" font-size="1.5ex" fill="lime"/></g>']),
    ("rem", [root % w for w in ["1.5rem", "3rex", "3rch", "1.5rcap", "1.5ric", "1.5rlh"]]),
    ("viewport", ["12vw", "24vh", "12vi", "24vb", "24vmin", "12vmax", "12svw", "24lvh", "12dvi", "24svb", "24lvmin", "12dvmax"]),
    ("percent", ["12%", '<rect x="{x}" y="{y}" width="12" height="4%" fill="lime"/>']),
    ("bad", ["!12foo", "!12px5", "!12p x", "!12 px", "!1.5e", "!12svx"]),
], 6, 16, 3, bar)
unit.height = 50
assert end <= 50, end
unit.write()
