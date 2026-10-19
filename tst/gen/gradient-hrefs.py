"""gradient-hrefs: a gradient's href, of SVG 2's paint servers and linking.
A gradient taking an attribute from its template is drawn in the expected
drawing by the template itself; one taking from a template of the other
kind as the gradient its attributes and stops spell out."""
from unitgen import Unit

unit = Unit("gradient-hrefs", 66, 74, """Each strip matches its plain twin
in the expected drawing; no red. A gradient with nothing but an href and the
attributes other than one takes that one from its template: x1, y1, x2, y2,
gradientTransform, gradientUnits and spreadMethod for a linear gradient,
cx, cy, r, fx, fy, fr and the same three for a radial one, and the
template's stops. A linear gradient takes a radial template's stops and
the attributes they share, and a radial one a linear template's; an href to
an element that is no gradient, or to the gradient itself, is ignored.""")

STOPS = '<stop offset="0" stop-color="navy"/><stop offset=".5" stop-color="lime"/><stop offset="1" stop-color="gold"/>'
defs = ['<g id="stops"><stop stop-color="red"/></g>']
plain_defs = []
cells = []


def place(k):
    return 2 + k % 4 * 16, 2 + k // 4 * 12


def copies(kind, name, attributes, k):
    x, y = place(k)
    attributes = {key: value.format(x=x, y=y, x7=x + 7, y5=y + 5) for key, value in attributes.items()}
    own = "".join(f' {key}="{value}"' for key, value in attributes.items())
    rest = "".join(f' {key}="{value}"' for key, value in attributes.items() if key != name)
    defs.append(f'<{kind} id="t-{kind}-{name}"{own}>{STOPS}</{kind}><{kind} id="c-{kind}-{name}" href="#t-{kind}-{name}"{rest}/>')
    plain_defs.append(f'<{kind} id="t-{kind}-{name}"{own}>{STOPS}</{kind}>')
    cells.append((f"{'linear' if kind == 'linearGradient' else 'radial'}-{name}", x, y, f"url(#c-{kind}-{name})", f"url(#t-{kind}-{name})"))


LINEAR = [
    ("x1", {"x1": ".3"}), ("y1", {"x2": "0", "y2": "1", "y1": ".5"}), ("x2", {"x2": ".5"}), ("y2", {"x2": "0", "y2": ".5"}),
    ("gradientTransform", {"gradientTransform": "rotate(30)"}),
    ("gradientUnits", {"gradientUnits": "userSpaceOnUse", "x1": "{x}", "x2": "{x7}"}),
    ("spreadMethod", {"x2": ".3", "spreadMethod": "reflect"}),
]
RADIAL = [
    ("cx", {"cx": ".3"}), ("cy", {"cy": ".3"}), ("r", {"r": ".3"}), ("fx", {"fx": ".3"}), ("fy", {"fy": ".3"}), ("fr", {"fr": ".2"}),
    ("gradientTransform", {"gradientTransform": "translate(.2 0)"}),
    ("gradientUnits", {"gradientUnits": "userSpaceOnUse", "cx": "{x7}", "cy": "{y5}", "r": "4"}),
    ("spreadMethod", {"r": ".2", "spreadMethod": "repeat"}),
]

for name, attributes in LINEAR:
    copies("linearGradient", name, attributes, len(cells))

for name, attributes in RADIAL:
    copies("radialGradient", name, attributes, len(cells))

# Templates of the other kind lend their stops and the attributes both
# kinds have; the plain twins spell them out.
defs.append(f'<radialGradient id="r-template" spreadMethod="reflect" gradientTransform="translate(.1 0)" cx=".2">{STOPS}</radialGradient>'
            '<linearGradient id="to-radial" href="#r-template" x2=".3"/>'
            f'<linearGradient id="l-template" spreadMethod="repeat" x2=".9">{STOPS}</linearGradient>'
            '<radialGradient id="to-linear" href="#l-template" r=".2"/>')
plain_defs.append(f'<linearGradient id="p-to-radial" spreadMethod="reflect" gradientTransform="translate(.1 0)" x2=".3">{STOPS}</linearGradient>'
                  f'<radialGradient id="p-to-linear" spreadMethod="repeat" r=".2">{STOPS}</radialGradient>')
cells.append(("linear-to-radial", *place(len(cells)), "url(#to-radial)", "url(#p-to-radial)"))
cells.append(("radial-to-linear", *place(len(cells)), "url(#to-linear)", "url(#p-to-linear)"))

# Ignored hrefs: one to an element holding stops lends none, so the
# gradient has none and paints nothing over the lime under it; one to the
# gradient itself leaves it its own stops.
defs.append('<linearGradient id="l-other" href="#stops"/><radialGradient id="r-other" href="#stops"/>'
            '<linearGradient id="l-self" href="#l-self"><stop stop-color="lime"/></linearGradient>'
            '<radialGradient id="r-self" href="#r-self"><stop stop-color="lime"/></radialGradient>')

for name in ("linear-other", "radial-other", "linear-self", "radial-self"):
    cells.append((name, *place(len(cells)), f"url(#{name[0]}-{name.split('-')[1]})", "lime"))

unit.common("<defs>" + "".join(defs) + "</defs>", "<defs>" + "".join(plain_defs) + "</defs>")

for name, x, y, test, plain in cells:
    under = '<rect x="{}" y="{}" width="14" height="10" fill="lime"/>'.format(x, y) if name.endswith("-other") else ""
    unit.cell(name, (x - 1, y - 1, 16, 12), under + f'<rect x="{x}" y="{y}" width="14" height="10" fill="{test}"/>',
              f'<rect x="{x}" y="{y}" width="14" height="10" fill="{plain}"/>', f'<rect x="{x}" y="{y}" width="14" height="10"/>')

unit.write()
