"""css-selector-basics: type, universal, class and id selectors, the
combinators and specificity of Selectors 4."""
from unitgen import TARGET as T, Unit, squares

F = "<g/>"
unit = Unit("css-selector-basics", 0, 0, """Each lime square fills its black
outline, and nothing red shows. A type selector matches its element, * any,
and *.t is .t; a type or * goes first in a compound, once. Names and values
are matched as XML has them, case-sensitively. A class selector matches a
word of the class attribute split on white space, as [class~=] does, and
none of an empty one; an id selector every element of that id; . or #
alone invalidates the selector. A B matches a B at any depth under an A, A
> B a child, A + B the next element sibling, text and comments between
them aside, A ~ B any later one; a leading combinator invalidates the
selector. Specificity counts ids, then classes, attributes and
pseudo-classes, then types, * nothing; a list takes that of the selector
that matched; a tie goes to the later rule.""")
unless = "{c} > * { fill: red } {c} > .t { fill: lime }"
squares(unit, [
    ("type", [("{c} > rect { fill: lime }", T), ("{c} > circle { fill: red } {c} > .t { fill: lime }", T)]),
    ("universal", [("{c} > * { fill: lime }", T), ("{c} > *.t { fill: red } {c} > .t { fill: lime }", T), ("{c} > .t { fill: red } {c} > *.t { fill: lime }", T)]),
    ("compound-order", [("{c} > .t*, " + unless, T), ("{c} > rect*, " + unless, T), ("{c} > *rect, " + unless, T)]),
    ("case-sensitive", [("{c} > RECT { fill: red } {c} > rect { fill: lime }", T), ("{c} > .T { fill: red } {c} > .t { fill: lime }", T),
                        ("{c} > #X { fill: red } {c} > .t { fill: lime }", '<rect class="t" id="x" width="6" height="6"/>'), ("{c} > [DATA-X] { fill: red } {c} > .t { fill: lime }", '<rect class="t" data-x="" width="6" height="6"/>')]),
    ("class", [("{c} > .a { fill: lime }", '<rect class="b&#9;a&#10;c" width="6" height="6" fill="red"/>'), ("{c} > [class~=a] { fill: lime }", '<rect class=" a " width="6" height="6" fill="red"/>'),
               ("{c} > .a.c { fill: lime }", '<rect class="c b a" width="6" height="6" fill="red"/>'), ("{c} > .a { fill: red } {c} > rect { fill: lime }", '<rect class="ab" width="6" height="6"/>')]),
    ("class-empty", [("{c} > .\\20 { fill: red } {c} > rect { fill: lime }", '<rect class="  " width="6" height="6"/>'), ("{c} > :not(.t) { fill: lime }", '<rect class="" width="6" height="6" fill="red"/>')]),
    ("id", [("{c} > #s { fill: lime }", '<rect id="s" width="6" height="6" fill="red"/>'),
            ("{c} > #twin { fill: lime }", '<rect id="twin" width="3" height="6" fill="red"/><rect id="twin" x="3" width="3" height="6" fill="red"/>')]),
    ("empty-name", [("{c} > ., " + unless, T), ("{c} > #, " + unless, T)]),
    ("descendant", [("{c} .t { fill: lime }", f"<g><g>{T}</g></g>"), ("{c} circle .t { fill: red } {c} .t { fill: lime }", f"<g>{T}</g>")]),
    ("child", [("{c} > g > .t { fill: lime }", f"<g>{T}</g>"), ("{c} > .t { fill: red } {c} .t { fill: lime }", f"<g>{T}</g>")]),
    ("next-sibling", [("{c} > g + .t { fill: lime }", F + "<!-- c -->text" + T), ("{c} > circle + .t { fill: red } {c} > .t { fill: lime }", '<circle r="0"/>' + F + T)]),
    ("subsequent-sibling", [("{c} > circle ~ .t { fill: lime }", '<circle r="0"/>' + F + F + T), ("{c} > .t ~ g { fill: red } {c} > g ~ .t { fill: lime }", F + T)]),
    ("leading-combinator", [("> .t, " + unless, T), ("+ .t, " + unless, T), ("~ .t, " + unless, T)]),
    ("specificity", [("{c} > #s { fill: lime } {c} > .t.t.t.t.t { fill: red }", '<rect class="t" id="s" width="6" height="6"/>'), ("{c} > .t { fill: lime } {c} > rect { fill: red }", T),
                     ("{c} > [data-x] { fill: lime } {c} > g rect, {c} > rect { fill: red }", '<rect data-x="" width="6" height="6"/>'), ("{c} > :first-child { fill: lime } {c} > rect { fill: red }", T)]),
    ("specificity-universal", [("{c} > rect.t { fill: lime } {c} > *.t { fill: red }", T), ("{c} > g .t { fill: lime } {c} > * > .t { fill: red }", f"<g>{T}</g>")]),
    ("specificity-list", [("{c} > rect.t { fill: lime } {c} > #x, {c} > .t { fill: red }", T)]),
    ("specificity-order", [("{c} > .t { fill: red } {c} > .t { fill: lime }", T)]),
])
unit.write()
