"""css-revert: revert, revert-layer and revert-rule of CSS Cascade 5."""
from unitgen import TARGET as R, Unit, squares

L = '<rect class="t" width="6" height="6" fill="lime"/>'
P = '<rect class="t" width="6" height="6"/>'


def G(inner):
    return f'<g fill="lime">{inner}</g>'


unit = Unit("css-revert", 0, 0, """Each lime square fills its black outline,
and nothing red shows. revert rolls a property back past every author
declaration and presentation attribute, to an inherited or initial value;
revert-layer to the highest declaration of an earlier layer, then to a
presentation attribute, from an important one past everything between its
layer normal and important, from the style attribute to the sheets,
important or not; revert-rule as if its rule were not there, again while
what it reaches reverts; a geometry property, a shorthand and all revert
the same, the keywords in any case.""")
squares(unit, [
    ("revert", [("{c} .t { fill: red } {c} .t { fill: revert }", G(P)), ("{c} .t { fill: revert }", G(R))]),
    ("revert-layer", [("@layer a7 { {c} .t { fill: lime } } @layer b7 { {c} .t { fill: revert-layer } }", R), ("@layer a8 { {c} .t { fill: revert-layer } }", L),
                      ("@layer a9 { {c} .t { fill: lime } } {c} .t { fill: revert-layer }", R), ("{c} .t { fill: revert-layer }", G(P)), ("", G('<rect class="t" width="6" height="6" fill="revert-layer"/>'))]),
    ("revert-layer-important", [("@layer a10 { {c} .t { fill: red } } @layer b10 { {c} .t { fill: red } } @layer a10 { {c} .t { fill: revert-layer !important } }", L),
                                ("@layer a22 { {c} .t { fill: lime } } @layer b22 { {c} .t { fill: revert-layer } } {c} .t { fill: revert-layer !important }", R)]),
    ("revert-layer-style", [("{c} .t { fill: lime }", '<rect class="t" width="6" height="6" fill="red" style="fill: revert-layer"/>'),
                            ("{c} .t { fill: red } {c} .t { fill: lime !important }", '<rect class="t" width="6" height="6" fill="red" style="fill: revert-layer !important"/>')]),
    ("revert-rule", [("{c} .t { fill: lime } {c} .t { fill: revert-rule; stroke: none }", R), ("{c} .t { fill: lime } {c} .t { fill: revert-rule } {c} .t { fill: revert-rule }", R),
                     ("{c} .t { fill: lime } {c} .t { fill: red; fill: revert-rule }", R), ("{c} .t { fill: REVERT-RULE } {c} .t { fill: lime }", R),
                     ("{c} > rect.t { fill: lime } {c} .t { fill: red } {c} > rect.t.t { fill: revert-rule }", R)]),
    ("revert-geometry", [("{c} > .t { width: revert }", '<rect width="6" height="6" fill="lime"/><rect class="t" width="6" height="6" fill="red"/>'),
                         ("@layer g1 { {c} .t { width: 6px } } {c} .t { width: revert-layer }", '<rect class="t" width="2" height="6" fill="lime"/>'),
                         ("{c} .t { width: 2px; width: revert-rule }", '<rect class="t" width="6" height="6" fill="lime"/>')]),
    ("revert-shorthand", [("{c} .t { mask: url(#none) } {c} .t { mask: revert }", L), ("", G('<rect class="t" fill="red" style="all: revert; width: 6px; height: 6px"/>'))]),
    ("shorthand-keyword", [("{c} .t { mask: inherit }", L)]),
])
unit.write()
