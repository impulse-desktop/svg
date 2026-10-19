"""css-variables: custom properties and var() of CSS Custom Properties for
Cascading Variables 1, and env()."""
from unitgen import TARGET as T, Unit, squares

LIME = '<rect class="t" width="6" height="6" fill="lime"/>'


def lime(style):
    """A target lime by its fill attribute, a style on it."""
    return f'<rect class="t" width="6" height="6" fill="lime" style="{style}"/>'


def red(style):
    """A target red by its fill attribute, a style on it."""
    return f'<rect class="t" width="6" height="6" fill="red" style="{style}"/>'


unit = Unit("css-variables", 0, 0, """Each lime square fills its black outline,
and nothing red shows. A property whose name starts with two dashes is a
custom property: its name keeps its case, its value is the tokens written,
empty ones too, and it inherits; initial makes it the guaranteed-invalid
value, inherit, unset and revert its parent's. var() puts a custom
property's tokens in a value, in a sheet, a style attribute or a
presentation attribute, at computed-value time: its value, else its
fallback, substituted in turn, the name itself perhaps from a var(); tokens
stay tokens, so a number before an ident is no dimension. A value it leaves
invalid, a var() of nothing without a fallback, or a cycle among custom
properties, makes the declaration invalid at computed-value time: unset, no
lower declaration; a substitution of more than 16384 tokens, or past the
half million a document may write, gives the guaranteed-invalid value; a
var() or env() of no first argument makes it invalid as
it is read, so a lower one does win. A shorthand's longhands take its value once it is
substituted; all takes keywords alone, and resets no custom property. An instance takes its custom
properties from its use. env() has no variable here and takes its
fallback.""")
squares(unit, [
    ("declared", [("{c} { --c: lime } {c} > .t { fill: var(--c) }", T), ("{c} > .t { --c: lime; fill: var(--c) }", T), ("", red("--c: lime; fill: var(--c)")),
                  ("{c} > .t { --c: lime } {c} > .t { fill: var( --c ) }", T)]),
    ("inherited", [(":root { --root-c: lime } {c} > .t { fill: var(--root-c) }", T), ("", '<g style="--c: lime">' + red("fill: var(--c)") + "</g>"),
                   ("{c} > g { --c: red } {c} > g > .t { --c: lime } {c} .t { fill: var(--c) }", f"<g>{T}</g>")]),
    ("case", [("{c} { --C: red; --c: lime } {c} > .t { fill: var(--c) }", T), ("{c} { --c: red; --C: lime } {c} > .t { fill: var(--C) }", T)]),
    ("fallback", [("{c} > .t { fill: var(--none, lime) }", T), ("{c} > .t { fill: var(--none, var(--none2, lime)) }", T), ("{c} > .t { fill: var(--none,) lime }", T),
                  ("{c} > .t { fill: var(--none, rgb(0, 255, 0)) }", T), ("{c} { --c: lime } {c} > .t { fill: var(--c, red) }", T)]),
    ("presentation", [("{c} { --c: lime }", '<rect class="t" width="6" height="6" fill="var(--c)"/>'), ("{c} { --o: 0 }", '<rect class="t" width="6" height="6" fill="lime"/><rect width="6" height="6" fill="red" opacity="var(--o)"/>'),
                      ("{c} { --w: 6px }", '<rect width="0" height="6" fill="red"/><rect width="var(--w)" height="6" fill="lime"/>')]),
    ("tokens", [("{c} { --n: 6 } {c} .t { fill: lime; stroke: red; stroke-width: var(--n)px }", '<g stroke-width="0">' + T + "</g>"),
                ("{c} { --a: 0 255 } {c} > .t { fill: rgb(var(--a) 0) }", T), ("{c} { --e:; } {c} > .t { fill: var(--e) lime }", T),
                ("{c} { --l: li } {c} .t { fill: red; fill: var(--l)me }", '<g fill="lime">' + T + "</g>")]),
    ("names", [("{c} { --n: --c; --c: lime } {c} > .t { fill: var(var(--n)) }", T), ("{c} { --c: red } {c} > .t { fill: var(c, lime) }", T)]),
    ("invalid", [("{c} .t { fill: red } {c} .t.t { fill: var(--none) }", '<g fill="lime">' + T + "</g>"), ("{c} .t { fill: red; fill: var(--none) }", '<g fill="lime">' + T + "</g>"),
                 ("{c} { --c: 5px } {c} .t { fill: var(--c) }", '<g fill="lime">' + T + "</g>"), ("{c} > .t { opacity: var(--none) }", LIME),
                 ("{c} .t { fill: var(--none, red, blue) }", '<g fill="lime">' + T + "</g>")]),
    ("grammar", [("{c} > .t { fill: lime } {c} > .t.t { fill: var() }", T), ("{c} > .t { fill: lime } {c} > .t.t { fill: var(, red) }", T), ("{c} > .t { --c: lime } {c} > .t { --c: var(); fill: var(--c) }", T),
                 ("", '<rect class="t" width="6" height="6" fill="lime" style="fill: env()"/>'),
                 ("{c} > .t { --c: lime } {c} > .t { --c: red \"x\n; fill: var(--c) }", T), ("{c} > .t { --c: lime } {c} > .t { --c: red ! x; fill: var(--c) }", T),
                 ("{c} > .t { --c: lime } {c} > .t { --c: red ); fill: var(--c) }", T)]),
    ("keywords", [("{c} > g { --c: lime } {c} > g > .t { --c: red } {c} > g > .t.t { --c: inherit; fill: var(--c) }", f"<g>{T}</g>"), ("{c} { --c: red } {c} > .t { --c: initial; fill: var(--c, lime) }", T),
                  ("{c} { --c: lime } {c} > g { --c: red } {c} > g { --c: unset } {c} .t { fill: var(--c) }", f"<g>{T}</g>"), ("{c} { --c: lime } {c} > .t { --c: red } {c} > .t { --c: revert; fill: var(--c) }", T),
                  ("{c} .t { --k: inherit } {c} .t { fill: var(--k) }", '<g fill="lime">' + T + "</g>"),
                  ("{c} .t { fill: red } {c} .t.t { fill: var(--none, revert) }", '<g fill="lime">' + T + "</g>")]),
    ("cycles", [("{c} > .t { --a: var(--b); --b: var(--a); fill: var(--a, lime) }", T), ("{c} > .t { --a: var(--a, red); fill: var(--a, lime) }", T),
                ("{c} > .t { --a: var(--b, red); --b: var(--a, red); --x: var(--a, lime); fill: var(--x) }", T),
                ("{c} { --a: lime } {c} > .t { --a: var(--a); fill: var(--a, lime) }", T)]),
    ("cascade", [("{c} > .t { --c: lime !important } {c} > .t { --c: red }", lime("fill: var(--c)").replace('fill="lime"', 'fill="red"')),
                 ("@layer x { {c} > .t { --c: red } } {c} > .t { --c: lime } {c} > .t { fill: var(--c) }", T)]),
    ("shorthands", [("{c} { --m: none } {c} > .t { marker: var(--m) }", '<marker id="vm" markerWidth="6" markerHeight="6" markerUnits="userSpaceOnUse"><rect width="6" height="6" fill="red"/></marker><path class="t" d="M0 0h6v6h-6z" fill="lime" marker-start="url(#vm)"/>'),
                    ("{c} .t { all: var(--none, inherit) }", '<svg width="6" height="6" fill="lime"><rect class="t" fill="red"/></svg>')]),
    ("not-all", [("{c} .t { --c: lime } {c} .t { all: inherit; fill: var(--c) }", '<svg width="6" height="6"><rect class="t" fill="red"/></svg>')]),
    ("instance", [("", '<symbol id="vs"><rect width="6" height="6" style="fill: var(--c, red)"/></symbol><use href="#vs" style="--c: lime"/>'),
                  ("", '<g style="--c: lime"><use href="#vs"/></g>'), ("", '<use href="#vs" style="--c: red"/><use href="#vs" style="--c: lime"/>'),
                  ("", '<defs style="--c: red"><rect id="vr" width="6" height="6" style="fill: var(--c)"/></defs><use href="#vr" style="--c: lime"/>')]),
    ("long", [("{c} .t { --a0: x x x x x x x x x x x x x x x x; --a1: var(--a0) var(--a0); --a2: var(--a1) var(--a1); --a3: var(--a2) var(--a2); --a4: var(--a3) var(--a3); --a5: var(--a4) var(--a4); --a6: var(--a5) var(--a5); --a7: var(--a6) var(--a6); --a8: var(--a7) var(--a7); --a9: var(--a8) var(--a8); --a10: var(--a9) var(--a9); --a11: var(--a10) var(--a10); fill: var(--a11, lime) }", T), ("{c} .u { --a0: x x x x x x x x x x x x x x x x; --a1: var(--a0) var(--a0); --a2: var(--a1) var(--a1); --a3: var(--a2) var(--a2); --a4: var(--a3) var(--a3); --a5: var(--a4) var(--a4); --a6: var(--a5) var(--a5); --a7: var(--a6) var(--a6); --a8: var(--a7) var(--a7); --a9: var(--a8) var(--a8); --a10: var(--a9) var(--a9); --a11: var(--a10) var(--a10); fill: var(--a11, lime) }", "".join('<rect class="u" width="6" height="6" fill="red"/>' for k in range(20)))]),
    ("env", [("{c} > .t { fill: env(no-such-variable, lime) }", T), ("{c} .t { fill: env(no-such-variable) }", '<g fill="lime">' + T + "</g>")]),
])
unit.write()
