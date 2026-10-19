"""css-nth: An+B of CSS Syntax 3 and the child-index pseudo-classes of
Selectors 4."""
from unitgen import TARGET as T, Unit, squares

F = "<g/>"
unit = Unit("css-nth", 0, 0, """Each lime square fills its black outline,
and nothing red shows. An+B of css-syntax-3: odd, even, An+B with B signed
by itself or by a + or - apart, white space around the parts but not
between a sign and its n or digits, A and B of either sign or none, N and
ODD in any case; 0n+0 matches no position, and a form not in the grammar
invalidates its selector. The child-index pseudo-classes of selectors-4
count element siblings from 1, text and comments not: :nth-child() and
:nth-last-child(), "of S" counting only the siblings S matches,
:nth-of-type() and :nth-last-of-type() of the same element type, the
first, last and only forms, :empty with no child element nor text (white
space, an entity or a character reference count; a comment, a processing
instruction or an empty CDATA section do not), :root.""")
red = "{c} > * { fill: red } {c} > .t { fill: lime }"
plain = "{c} > :empty { fill: red } {c} > .t { fill: lime }"
squares(unit, [
    ("anb-keywords", [("{c} > :nth-child(odd) { fill: lime }", T), ("{c} > :nth-child(even) { fill: lime }", F + T), ("{c} > :nth-child(ODD) { fill: lime }", T),
                      ("{c} > :nth-child(3) { fill: lime }", F + F + T), ("{c} > :nth-child(+2) { fill: lime }", F + T)]),
    ("anb-forms", [("{c} > :nth-child(2n+1) { fill: lime }", F + F + T), ("{c} > :nth-child(2n-1) { fill: lime }", T), ("{c} > :nth-child( 3n + 1 ) { fill: lime }", F + F + F + T),
                   ("{c} > :nth-child(n) { fill: lime }", F + T), ("{c} > :nth-child(+n) { fill: lime }", T), ("{c} > :nth-child(2N+1) { fill: lime }", F + F + T),
                   ("{c} > :nth-child(n- 1) { fill: lime }", T), ("{c} > :nth-child(2n -1) { fill: lime }", T), ("{c} > :nth-child(2n+ 1) { fill: lime }", T),
                   ("{c} > :nth-child(2n - 1) { fill: lime }", T)]),
    ("anb-positions", [("{c} > :nth-child(-n+3) { fill: lime }", F + F + T), ("{c} > :nth-child(-2n+3) { fill: lime }", F + F + T),
                       ("{c} > :nth-child(-n+1) { fill: lime } {c} > :nth-child(-n+0) { fill: red }", T), ("{c} > :nth-child(3), {c} > :nth-child(1) { fill: red } {c} > * { fill: lime }", F + T)]),
    ("anb-zero", [("{c} > :nth-child(0n+0), {c} > :nth-child(-n+0) { fill: red } {c} > * { fill: lime }", T)]),
    ("anb-invalid", [("{c} > :nth-child(+ n), " + red, T), ("{c} > :nth-child(- 1), " + red, T), ("{c} > :nth-child(2n- -1), " + red, T),
                     ("{c} > :nth-child(2n 1), " + red, T), ("{c} > :nth-child(n+), " + red, T), ("{c} > :nth-child(oddx), " + red, T),
                     ("{c} > :nth-child(n-1a), " + red, T), ("{c} > :nth-child( ), " + red, T), ("{c} > :nth-child(2.5n), " + red, T), ("{c} > :nth-child(+), " + red, T),
                     ("{c} > :nth-child(+-n), " + red, T), ("{c} > :nth-child(n + 1.5), " + red, T), ("{c} > :nth-child(n+ +1), " + red, T), ("{c} > :nth-child(n-), " + red, T),
                     ("{c} > :nth-child(n- 1.5), " + red, T), ("{c} > :nth-child(nx), " + red, T)]),
    ("child-index", [("{c} > :nth-child(2) { fill: lime }", F + "<!-- c -->x<?p?>" + T), ("{c} > :nth-child(1) { fill: lime }", "text" + T)]),
    ("nth-last-child", [("{c} > :nth-last-child(1) { fill: lime }", F + T), ("{c} > :nth-last-child(2n) { fill: lime }", T + F)]),
    ("nth-of", [("{c} > :nth-child(1 of .t) { fill: lime }", F + F + T), ("{c} > :nth-child(1 of.t) { fill: lime }", F + F + T), ("{c} > :nth-child(2 of g, .t) { fill: lime }", F + T + F), ("{c} > :nth-last-child(1 of .t) { fill: lime }", T + F),
                ("{c} > :nth-child(1 of circle) { fill: red } {c} > .t { fill: lime }", T)]),
    ("nth-of-type", [("{c} > :nth-of-type(1) { fill: lime }", F + T), ("{c} > :nth-last-of-type(2) { fill: lime }", T + '<rect width="0" height="0"/>')]),
    ("typed-shorthands", [("{c} > :first-of-type { fill: lime }", F + F + T), ("{c} > :last-of-type { fill: lime }", T + F), ("{c} > :only-of-type { fill: lime }", F + T + F),
                          ("{c} > :only-of-type { fill: red } {c} > .t { fill: lime }", '<rect width="0" height="0"/>' + T)]),
    ("first-last-only", [("{c} > :first-child { fill: lime }", T + F), ("{c} > :last-child { fill: lime }", F + T), ("{c} > :only-child { fill: lime }", T),
                         ("{c} > :only-child { fill: red } {c} > .t { fill: lime }", T + F)]),
    ("empty", [("{c} > :empty { fill: lime }", T), ("{c} > :empty { fill: lime }", '<rect class="t" width="6" height="6" fill="red"><!-- c --><?p?><![CDATA[]]></rect>')]),
    ("not-empty", [(plain, '<rect class="t" width="6" height="6"> </rect>'), (plain, '<rect class="t" width="6" height="6">x</rect>'), (plain, '<rect class="t" width="6" height="6"><desc/></rect>'),
                   (plain, '<rect class="t" width="6" height="6">&amp;</rect>'), (plain, '<rect class="t" width="6" height="6">&#x20;</rect>'), (plain, '<rect class="t" width="6" height="6"><![CDATA[x]]></rect>')]),
    ("root", [(":root > {c} > .t { fill: lime }", T), ("{c} > :root { fill: red } {c} > .t { fill: lime }", T)]),
    ("nth-overflow", [("{c} > :nth-child(99999999999) { fill: red } {c} > .t { fill: lime }", T), ("{c} > :nth-child(-99999999999n+1) { fill: lime }", T),
                      ("{c} > :nth-child(n-99999999999) { fill: lime }", F + T)]),
    ("nth-invalid", [("{c} > :nth-child(1 of :unknown), " + red, T), ("{c} > :foo(x), " + red, T), ("{c} > .t), " + red, T)]),
])
unit.write()
