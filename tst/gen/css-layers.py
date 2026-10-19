"""css-layers: cascade layers and @import of CSS Cascade 5."""
from unitgen import TARGET as R, Unit, squares

unit = Unit("css-layers", 0, 0, """Each lime square fills its black outline,
and nothing red shows; each square has a red fill attribute. Cascade layers
of css-cascade-5: a later layer beats an earlier one whatever their
specificity, and for important declarations the earlier one wins; layers
are ordered as first declared, by @layer statements, blocks or @import,
which loads nothing (D15) but places its layer, when it goes before every
rule but @charset and @layer statements; a sub-layer, named or dotted, goes
before its parent own rules; @layer { } makes a new anonymous layer each
time; a layer reopened keeps its place; what no layer holds is last, and a
presentation attribute below every author rule (D34); a CSS-wide keyword is
no layer name, and a statement with a bad name declares none.""")
squares(unit, [
    ("order", [("@layer a1, b1; @layer b1 { {c} > .t { fill: lime } } @layer a1 { {c} > rect.t.t { fill: red } }", R)]),
    ("reopen", [("@layer a5 { {c} > .t { fill: red } } @layer b5 { {c} > .t { fill: lime } } @layer a5 { {c} > rect.t.t { fill: red } }", R)]),
    ("important", [("@layer a2 { {c} > .t { fill: lime !important } } @layer b2 { {c} > .t { fill: red !important } } {c} > .t { fill: red !important }", R),
                   ("{c} > .t { fill: red } @layer v9 { {c} > .t { fill: lime !important } }", R)]),
    ("anonymous", [("@layer { {c} > .t { fill: red } } @layer { {c} > .t { fill: lime } }", R)]),
    ("nested", [("@layer a3 { @layer b3 { {c} > .t.t { fill: red } } {c} > .t { fill: lime } }", R), ("@layer a4.b4 { {c} > .t.t { fill: red } } @layer a4 { {c} > .t { fill: lime } }", R)]),
    ("names", [("@layer y6 { {c} > .t { fill: red } } @layer x6 { @layer y6 { {c} > .t { fill: lime } } } @layer y6 { {c} > rect.t.t { fill: red } }", R),
               ("@layer initial { {c} > .t { fill: red !important } } {c} > .t { fill: lime }", R)]),
    ("unlayered", [("{c} > .t { fill: lime } @layer z7 { {c} > rect.t.t { fill: red } }", R)]),
    ("layer-block", [("@layer w8 { {c} > .t { fill: lime } }", R), ("@layer m11 { @media all { {c} > .t { fill: lime } } }", R)]),
    ("statement", [("@layer x10, y10 z10; @layer y10 { {c} > .t { fill: red } } @layer x10 { {c} > .t { fill: lime } }", R),
                   ("@layer a20..b20, c20; @layer d20 { {c} > .t { fill: red } } @layer c20 { {c} > .t { fill: lime } }", R)]),
    ("import-layer", [("!@import url(none.css) layer(i12); @layer j12 { {c} > .t { fill: lime } } @layer i12 { {c} > .t { fill: red } }", R),
                      ("!@import url(none.css) layer; @layer { {c} > .t { fill: lime } }", R)]),
    ("import-string", [('!@layer n13; @import "none.css" layer(m13); @layer m13 { {c} > .t { fill: lime } } @layer n13 { {c} > .t { fill: red } }', R)]),
    ("import-position", [("!@layer q14 { {c} > .t { fill: red } } @import url(none.css) layer(r14); @layer s14 { {c} > .t { fill: red } } @layer r14 { {c} > .t { fill: lime } }", R),
                         ('!@charset "x"; @import url(none.css) layer(u16); @layer v16 { {c} > .t { fill: lime } } @layer u16 { {c} > .t { fill: red } }', R)]),
    ("import-invalid", [("!@import url(none.css; @layer t15 { {c} > .t { fill: lime } }", R), ("!@import foo layer(w17); @layer x17 { {c} > .t { fill: red } } @layer w17 { {c} > .t { fill: lime } }", R),
                        ("!@import url(none.css) layer(z19; @layer a19 { {c} > .t { fill: lime } }", R),
                        ("!@import url(none.css) layer(e23) { } @layer f23 { {c} > .t { fill: red } } @layer e23 { {c} > .t { fill: lime } }", R)]),
    ("import-nothing", [("!@import url(none.css); @layer y18 { {c} > .t { fill: lime } }", R), ("!@import url(none.css) print; {c} > .t { fill: lime }", R),
                        ("!@import url(none.css) supports(display: grid) screen; {c} > .t { fill: lime }", R)]),
])
unit.write()
