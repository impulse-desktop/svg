"""css-logical-pseudo: :is(), :where(), :not() and :has() of Selectors 4."""
from unitgen import TARGET as T, Unit, squares

F = "<g/>"
unit = Unit("css-logical-pseudo", 0, 0, """Each lime square fills its black
outline, and nothing red shows. :is() and :where() take a forgiving
selector list, a selector they cannot read dropped and an empty list
matching nothing; :where() adds no specificity, and :is(), :not() and
:has() that of their most specific argument, :nth-child() of S its own and
that of S. :not() takes a list of complex selectors, an invalid or empty
one invalidating it, and matches what none of them does. :has() takes
relative selectors, a descendant one, or one after a >, + or ~, from the
element, and matches when one matches an element it reaches; :has() inside
:has() and a pseudo-element in any of them are invalid. A pseudo-element,
a function one too, matches nothing drawn.""")
red = "{c} > * { fill: red } {c} > .t { fill: lime }"
squares(unit, [
    ("is", [("{c} > :is(.t) { fill: lime }", T), ("{c} > :is(.t:first-child, g + .t) { fill: lime }", F + T), ("{c} > :IS(.t) { fill: lime }", T)]),
    ("is-forgiving", [("{c} > :is(rect, :unknown) { fill: lime }", T), ("{c} > :is() { fill: red } {c} > .t { fill: lime }", T), ("{c} > :not(:is(:foo), g) { fill: lime }", T)]),
    ("where", [("{c} > :where(.t, :unknown) { fill: lime }", T), ("{c} > :where(#x, .t) { fill: red } {c} > rect { fill: lime }", T)]),
    ("argument-specificity", [("{c} > :is(#x, .t) { fill: lime } {c} > rect.t { fill: red }", T), ("{c} > :not(#x) { fill: lime } {c} > rect.t { fill: red }", T),
                              ("{c} > :not(.a .b) { fill: lime } {c} > rect.t { fill: red }", T), ("{c} > .t:has(+ #x) { fill: lime } {c} > rect.t { fill: red }", T + '<g id="x"/>')]),
    ("nth-specificity", [("{c} > :nth-child(1 of .t) { fill: lime } {c} > rect.t { fill: red }", T)]),
    ("not", [("{c} > :not(g) { fill: lime }", T), ("{c} > :not(g, circle) { fill: lime }", T), ("{c} > :not(.t g) { fill: lime }", T), ("{c} > :not(circle > .t) { fill: lime }", T),
             ("{c} > :not(#x, .t) { fill: red } {c} > .t { fill: lime } {c} > :not(#x):not(g) { fill: lime }", T)]),
    ("not-invalid", [("{c} > :not(:unknown), " + red, T), ("{c} > :not(), " + red, T)]),
    ("has", [("{c} > :has(rect) { fill: red } {c} > rect:has(g) { fill: red } {c} > .t { fill: lime }", T), ("{c}:has(> .t) > .t { fill: lime }", T), ("{c}:has(> g > .t) > g > .t { fill: lime }", f"<g>{T}</g>"),
             ("{c} > .t:has(+ g) { fill: lime }", T + F), ("{c} > .t:has(~ circle) { fill: lime }", T + F + '<circle r="0"/>'), ("{c} > .t:has(+ circle) { fill: red } {c} > .t { fill: lime }", T + F + '<circle r="0"/>'),
             ("{c}:has(rect.t) .t { fill: lime }", f"<g>{T}</g>"), ("{c}:has(> circle, > .t) > .t { fill: lime }", T), ("{c} > .t:not(:has(circle)) { fill: lime }", T)]),
    ("has-nested", [("{c}:has(:has(.t)), " + red, T)]),
    ("pseudo-element", [("{c} > :is(::before, .t) { fill: lime }", T), ("{c} > :not(::before), " + red, T), ("{c}:has(::before), " + red, T), ("{c} > .t::part(x) { fill: red } {c} > .t { fill: lime }", T)]),
])
unit.write()
