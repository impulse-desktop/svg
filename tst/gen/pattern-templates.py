"""pattern-templates: a pattern's href and its errors, of SVG 2's paint
servers and linking. A pattern taking an attribute from its template is
drawn in the expected drawing by the template itself; one ignoring its href
as the tiles of its own drawn out."""
from unitgen import Unit

unit = Unit("pattern-templates", 68, 68, """Each square matches its plain
twin in the expected drawing; no red. A pattern's href to an element that
is no pattern, a gradient or itself is ignored: its own tiles, lime squares
every 4, paint. A pattern with nothing but an href and the attributes
other than one takes that one from its template, x, y, width, height,
viewBox, preserveAspectRatio, patternTransform, patternUnits or
patternContentUnits, and its children too. A negative width or height is an
error: the fallback paints, lime, and without one nothing does.""")

OWN = '<rect width="2" height="2" fill="lime"/>'
defs = ['<rect id="no" width="4" height="4" fill="red"/><linearGradient id="lg"><stop stop-color="red"/></linearGradient>',
        f'<pattern id="not-pattern" href="#no" patternUnits="userSpaceOnUse" width="4" height="4">{OWN}</pattern>',
        f'<pattern id="to-gradient" href="#lg" patternUnits="userSpaceOnUse" width="4" height="4">{OWN}</pattern>',
        f'<pattern id="self" href="#self" patternUnits="userSpaceOnUse" width="4" height="4">{OWN}</pattern>']

# Each attribute: the template's attributes, the one it lends, its child.
USER = {"patternUnits": "userSpaceOnUse", "width": "4", "height": "4"}
TEMPLATES = [
    ("x", dict(USER, x="1"), OWN),
    ("y", dict(USER, y="1"), OWN),
    ("width", dict(USER, width="6"), OWN),
    ("height", dict(USER, height="6"), OWN),
    ("viewBox", dict(USER, viewBox="0 0 8 8"), '<rect width="4" height="4" fill="lime"/>'),
    ("preserveAspectRatio", dict(USER, width="8", viewBox="0 0 4 4", preserveAspectRatio="xMinYMid meet"), '<rect width="4" height="4" fill="lime"/>'),
    ("patternTransform", dict(USER, patternTransform="translate(1 1)"), OWN),
    ("patternUnits", dict(USER), OWN),
    ("patternContentUnits", dict(USER, patternContentUnits="objectBoundingBox"), '<rect width=".1666667" height=".1666667" fill="lime"/>'),
]

for name, attributes, child in TEMPLATES:
    own = "".join(f' {k}="{v}"' for k, v in attributes.items())
    rest = "".join(f' {k}="{v}"' for k, v in attributes.items() if k != name)
    defs.append(f'<pattern id="t-{name}"{own}>{child}</pattern><pattern id="c-{name}" href="#t-{name}"{rest}/>')

defs.append('<pattern id="nw" patternUnits="userSpaceOnUse" width="-4" height="4"><rect width="20" height="20" fill="red"/></pattern>')
defs.append('<pattern id="nh" patternUnits="userSpaceOnUse" width="4" height="-4"><rect width="20" height="20" fill="red"/></pattern>')
unit.common("<defs>" + "".join(defs) + "</defs>", "<defs>" + "".join(defs[1 + 3:-2]) + "</defs>")


def square(x, y, paint):
    return f'<rect x="{x}" y="{y}" width="12" height="12" fill="{paint}"/>'


cells = [(name, f"url(#{name})", "".join(f'<rect x="{{x}}" y="{{y}}" width="2" height="2" fill="lime" transform="translate({4 * i} {4 * j})"/>' for i in range(3) for j in range(3)))
         for name in ("not-pattern", "to-gradient", "self")]
cells += [(f"copies-{name}", f"url(#c-{name})", square("{x}", "{y}", f"url(#t-{name})")) for name, *_ in TEMPLATES]

for k, (name, paint, plain) in enumerate(cells):
    x, y = 4 + k % 4 * 16, 4 + k // 4 * 16
    unit.cell(name, (x - 1, y - 1, 14, 14), square(x, y, paint), plain.format(x=x, y=y), square(x, y, "none"))

for k, (name, under, paint) in enumerate((("negative-width", "red", "url(#nw) lime"), ("negative-height", "red", "url(#nh) lime"), ("negative-none", "lime", "url(#nw)")), len(cells)):
    x, y = 4 + k % 4 * 16, 4 + k // 4 * 16
    unit.cell(name, (x - 1, y - 1, 14, 14), square(x, y, under) + square(x, y, paint), square(x, y, "lime"), square(x, y, "none"))

unit.write()
