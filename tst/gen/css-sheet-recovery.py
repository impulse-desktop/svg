"""css-sheet-recovery: the error recovery of CSS Syntax 3."""
from unitgen import TARGET as T, Unit, squares

unit = Unit("css-sheet-recovery", 0, 0, """Each lime square fills its black
outline, and nothing red shows. css-syntax-3's error recovery: the end of a
style sheet, a style attribute or a presentation attribute closes what is
open, a function, a block, an at-rule, a string, a url, a comment, but a
qualified rule whose block never opened is dropped; a newline ends a bad
string and drops its declaration, a quote or parenthesis in an unquoted
url drops its; an invalid selector drops its rule; CDO and CDC tokens are
nothing; @charset is an unknown rule; a declaration without a name and a
colon, an unknown property or a value its grammar refuses is dropped
alone; property names and keywords are ASCII case-insensitive;
!important, spaced, of any case or escaped, marks a declaration, ! and
another word invalidate it.""")
unit.common('<linearGradient id="gl"><stop stop-color="lime"/></linearGradient>')


def attribute(attributes):
    return f'<rect class="t" width="6" height="6" {attributes}/>'


def inherited(value):
    return f'<g fill="lime"><rect class="t" width="6" height="6" fill="{value}"/></g>'


squares(unit, [
    ("eof-close", [("!{c} > .t { fill: rgb(0 255 0", T), ("!{c} > .t { fill: lime", T), ('!{c} > .t { fill: lime; stroke: "abc', T), ("!{c} > .t { fill: url(#gl", T),
                   ("", attribute('fill="red" style="fill: rgb(0 255 0"')), ("", attribute('fill="rgb(0 255 0"')), ("", attribute('fill="url(#gl"')), ("", attribute("fill=\"url('#gl\""))]),
    ("eof-at-rule", [("!@media all { {c} > .t { fill: lime }", T)]),
    ("eof-prelude", [("!{c} > .t { fill: lime } {c} > .t", T)]),
    ("comment-eof", [("!{c} > .t { fill: lime } /* open", T), ("!{c} > .t { fill: lime /* open", T)]),
    ("comment", [("{c} > .t { fill: /* x */ lime }", T), ("{c}/**/>/**/.t/**/{/**/fill/**/:/**/lime/**/}", T)]),
    ("bad-string", [('{c} > .t { fill: lime } {c} > .t { fill: "x\n; }', T), ('{c} > .t { fill: "x\n; fill: lime }', T),
                    ('{c} > .t { fill: lime } {c} > .t { fill: \\6c ime "x\n; }', T), ('{c} > .t { fill: lime } {c} > .t { fill: \\6c ime "x\ny; }', T)]),
    ("bad-url", [('{c} > .t { fill: lime } {c} > .t { fill: url(a"b) red }', T), ("{c} > .t { fill: lime } {c} > .t { fill: url(a(b) red }", T), ("", attribute("fill=\"lime\" style=\"filter: url('(') invert(1)\""))]),
    ("invalid-selector", [("{c} > .t:unknown { fill: red } {c} > .t { fill: lime }", T)]),
    ("cdo-cdc", [("!&lt;!-- {c} > .t { fill: lime } --&gt;", T)]),
    ("charset", [('@charset "x"; {c} > .t { fill: lime }', T)]),
    ("bad-declaration", [("{c} > .t { : red; fill lime; fill: lime }", T), ("{c} > .t { fill: lime; foo; stroke: none }", T)]),
    ("invalid-declaration", [("{c} > .t { fill: lime; fill: ; fill: bogus; foo: red }", T), ("", inherited("lime/")), ("", inherited("'a&#10;b'")), ("", inherited("[a]")), ("", inherited("{a}")),
                             ("", inherited("xurl(#gl")), ("", attribute("fill=\"lime\" style=\"clip-path: path('M0 0 h6 v6 h-6 z (')\""))]),
    ("case", [("{c} > .t { FILL: lime }", T), ("{c} > .t { Fill: LIME }", T)]),
    ("important", [("{c} > .t { fill: lime !important } {c} > .t { fill: red }", T), ("{c} > .t { fill: lime ! important } {c} > .t { fill: red }", T),
                   ("{c} > .t { fill: lime !IMPORTANT } {c} > .t { fill: red }", T), ("{c} > .t { fill: lime !\\69mportant } {c} > .t { fill: red }", T)]),
    ("not-important", [("{c} > .t { fill: lime } {c} > .t { fill: red !foo }", T), ("{c} > .t { fill: lime } {c} > .t { fill: red ! }", T)]),
])
unit.write()
