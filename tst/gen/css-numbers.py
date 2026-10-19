"""css-numbers: numbers and integers of css-values-4, as widths and octaves."""
from unitgen import Unit, rows

unit = Unit("css-numbers", 100, 0, """Each lime bar fills its black outline,
and nothing red shows; every bar is 12px. A number is an integer, or digits
with a point and more digits, signed or not, with an exponent of e or E
signed or not; a point with no digit after it, an exponent with no digits
or a fraction is no number. A coordinate may be negative. A number beyond
the floats' range is the largest one, which fills its cell; one too small
is 0. Below, each turbulence is drawn with the integer of octaves under
it: an integer signed or not, a calc() rounded half up, an integer past
the integers' range the largest, a number with a fraction no integer, so
the initial 1.""")


def bar(x, y, width):
    if width.startswith("<"):
        return width.format(x=x, y=y), f'<rect x="{x}" y="{y}" width="12" height="2" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="2"/>'

    if width.startswith("!"):
        return f'<rect x="{x}" y="{y}" width="{width[1:]}" height="2" fill="red"/>', "", ""

    return f'<rect x="{x}" y="{y}" width="{width}" height="2" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="2" fill="lime"/>', f'<rect x="{x}" y="{y}" width="12" height="2"/>'


full = '<svg x="{x}" y="{y}" width="12" height="2"><rect width="%s" height="2" fill="lime"/></svg>'
y = rows(unit, 2, [
    ("number", ["12", "+12", "1.2e1", "120E-1", ".12e+2", "12.0", "1200e-2", "12e0", "+1.2E+1"]),
    ("syntax", ["!12.", "!12.e0", "!1.-2", "!.e1", "!1e", "!1e1.5", "!+-12", "!1.2.3"]),
    ("negative", ['<rect x="{x}" y="{y}" width="12" height="2" fill="lime" transform="translate(4)" style="x: calc({x}px - 4px)"/>', '<g transform="translate({x} {y})"><rect x="-6" y="-1" width="12" height="2" fill="lime" transform="translate(6 1)"/></g>']),
    ("precision", [full % "1e39", full % "1e10000", full % "3.5e38", "!1e-50"]),
], 6, 16, 3, bar)
seed = 0


def octaves(x, y, value):
    global seed
    seed += 1
    test, plain = value
    region = f'filterUnits="userSpaceOnUse" x="{x}" y="{y}" width="12" height="6"'
    filter = '<filter id="{0}" %s><feTurbulence baseFrequency=".3" numOctaves="{1}" seed="%d"/></filter><rect x="%d" y="%d" width="12" height="6" filter="url(#{0})"/>' % (region, seed, x, y)

    return filter.format(f"t{seed}", test), filter.format(f"t{seed}", plain), ""


y = rows(unit, y + 1, [
    ("integer", [("+2", "2"), ("-0", "0"), ("02", "2")]),
    ("round", [("calc(1.5)", "2"), ("calc(2.5 - 1)", "2"), ("calc(-0.5)", "0")]),
    ("fraction", [("2.0", "1"), ("2e0", "1"), ("2.5", "1")]),
    ("large", [("99999999999", "2147483647"), ("-99999999999", "-2147483648")]),
], 6, 16, 7, octaves)
unit.height = y + 1
unit.write()
