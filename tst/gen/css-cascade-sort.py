"""css-cascade-sort: shorthands, value stages and the cascade sort of CSS
Cascade 5."""
from unitgen import TARGET as R, Unit, squares

# A lime target over red: the target must show, unmasked.
SHOW = '<rect width="6" height="6" fill="red"/><rect class="t" width="6" height="6" fill="lime"/>'
# A red target over lime: the target must be masked away.
HIDE = '<rect width="6" height="6" fill="lime"/><rect class="t" width="6" height="6" fill="red"/>'
CLEAR = "linear-gradient(transparent, transparent)"
unit = Unit("css-cascade-sort", 0, 0, """Each lime square fills its black
outline, and nothing red shows. A shorthand, mask here, sets every
longhand, one it leaves out to its initial value, and mask-border, which it
cannot set, too; a CSS-wide keyword sets them all and takes no other
component; an important shorthand makes every longhand important. A
property no declaration sets inherits or takes its initial value; one that
does not apply to an element, fill on a group, still inherits from it; a
url computes to its resource. Declarations sort by importance, then the
style attribute above the sheets, then layer, specificity and order, each
step settling only what the steps before left tied; author rules beat the
user agent's, and a presentation attribute is below every author rule
(D34).""")
squares(unit, [
    ("shorthand-sets", [("{c} > .t { mask: linear-gradient(black, black) 6px 0 / 6px 6px no-repeat }", HIDE), ("{c} > .t { mask: " + CLEAR + " }", HIDE)]),
    ("shorthand-reset", [("{c} > .t { mask-image: linear-gradient(black, black); mask-position: 6px 0; mask-repeat: no-repeat; mask-size: 1px 1px } {c} > .t { mask: linear-gradient(black, black) }", SHOW)]),
    ("shorthand-reset-only", [("{c} > .t { mask-border: " + CLEAR + " 0 fill / 0 } {c} > .t { mask: none }", SHOW)]),
    ("shorthand-keyword", [("{c} > .t { mask: " + CLEAR + " } {c} > .t { mask: initial }", SHOW), ("{c} > .t { mask-border: " + CLEAR + " 0 fill / 0 } {c} > .t { mask: initial }", SHOW),
                           ("{c} > .t { mask: initial " + CLEAR + " }", SHOW)]),
    ("shorthand-important", [("{c} > .t { mask: none !important } {c} > .t.t { mask-image: " + CLEAR + " }", SHOW)]),
    ("no-cascade", [("", '<g fill="lime"><rect class="t" width="6" height="6"/></g>'), ("{c} { fill: lime }", '<g><rect class="t" width="6" height="6"/></g>')]),
    ("applies-to", [("{c} > g { fill: lime }", '<g><rect class="t" width="6" height="6"/></g>')]),
    ("computed-url", [("", '<linearGradient id="gu"><stop stop-color="lime"/></linearGradient><g fill="url(#gu)"><rect class="t" width="6" height="6"/></g>')]),
    ("sort-importance", [("{c} > .t { fill: lime !important } {c} > rect.t.t.t { fill: red }", R), ("{c} > .t { fill: lime !important }", '<rect class="t" width="6" height="6" style="fill: red"/>')]),
    ("sort-attachment", [("{c} > rect.t.t.t { fill: red }", '<rect class="t" width="6" height="6" style="fill: lime"/>'), ("@layer s1 { {c} > .t { fill: red } }", '<rect class="t" width="6" height="6" style="fill: lime"/>'),
                         ("{c} > .t { fill: red !important }", '<rect class="t" width="6" height="6" style="fill: lime !important"/>')]),
    ("sort-layer", [("@layer s2, s3; @layer s3 { {c} > .t { fill: lime } } @layer s2 { {c} > rect.t.t { fill: red } }", R)]),
    ("origin-ua", [("{c} > svg { overflow: visible }", '<rect width="6" height="6" fill="red"/><svg width="2" height="6"><rect width="6" height="6" fill="lime"/></svg>')]),
    ("presentation", [("{c} > .t { fill: lime }", R), ("@layer p1 { {c} > .t { fill: lime } }", R), ("{c} > * { fill: lime }", R)]),
])
unit.write()
