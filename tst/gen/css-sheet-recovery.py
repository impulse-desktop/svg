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
another word invalidate it. Blocks and functions match their closing
tokens as components: a } inside an unclosed function or parenthesis is
the function's, so an unclosed var( or url( with a space takes in the rest
of the sheet; a url( bad by a space takes in all to its ). In a style rule's block what is no declaration is a nested
rule, which ends at its own {} block or a ;, so a declaration after a {}
block of template text still applies. Tokens are CSS's: a number with an
exponent or a dimension whose unit starts with e, a hash of an identifier
alone an id, CDO and CDC nothing only at the top of a sheet, elsewhere a
rule's prelude that fails, every code
point past ASCII an identifier's (D45), a - or -- at the end a delim and
an identifier, a hyphen before an escape an identifier's start, a < ! or @
without what follows them delims; a control code point or a backslash
before a newline makes a url bad, and an escaped ) does not end its
remnants. A declaration needs its colon; !important needs its !; a
-WEBKIT-clip-path is clip-path, as browsers have it; -- names no custom
property; a rule needs a prelude, and an identifier and a block, -x or
--x, is a rule, one that matches nothing. A media query that does not
parse is false, an empty one in a list too, which leaves the list true
when another is.""")
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
    ("open-function", [("!{c} > .t { fill: lime; stroke: var(--x, red } {c} > .t { fill: red }", T), ("!{c} > .t { fill: lime; stroke: rgb(1 2 } {c} > .t { fill: red }", T),
                       ("!{c} > .t { fill: lime } {c} > .t { stroke: url(a b } {c} > .t { fill: red }", T), ("{c} > .t { fill: red } {c} > .t { stroke: url(a b) } {c} > .t { fill: lime }", T), ("!{c} > .t { fill: lime } {c} > .t { stroke: [a } {c} > .t { fill: red }", T)]),
    ("nested-blocks", [("{c} > .t { fill: red; {% x %} fill: lime }", T), ("{c} > .t { fill: red; {% if y %}fill: lime;{% endif %} }", T), ("{c} > .t { fill: red; a:b { c: d } fill: lime }", T),
                       ("{c} > .t { fill: red; a b; fill: lime }", T), ("{c} > .t { fill: red; @media all { fill: red } fill: lime }", T),
                       ("--x: y { {c} > .t { fill: red } } --z a { fill: red } {c} > .t { fill: lime }", T)]),
    ("numbers", [("{c} > .t { fill: lime; stroke: red; stroke-width: 1e3e } {c} > .t { stroke-width: 0 }", T), ("{c} > .t { width: 0.6e1px; fill: lime }", '<rect class="t" height="6" fill="red"/>'), ("{c} > .t { opacity: 0e3 }", '<rect width="6" height="6" fill="lime"/>' + T),
                 ("{c} > .t { fill: lime } {c} > #1a { fill: red }", T.replace('/>', ' id="1a"/>')), ("{c} > .t { fill: lime; stroke: red; stroke-width: 0 } {c} > .t { stroke-width: .0e0 }", T)]),
    ("cdo-cdc-nested", [("{c} > .t { fill: lime } @media all { &lt;!-- {c} > .t { fill: red } }", T), ("{c} > .t { fill: lime; &lt;!-- fill: red }", T)]),
    ("token-ends", [("", attribute('fill="lime" style="stroke: -"')), ("", attribute('fill="lime" style="stroke: --"')), ("!{c} > .t { fill: lime } /* x *", T), ("", inherited("url(#gl ")),
                    ("{c} > .-\\61 { fill: lime }", T.replace('class="t"', 'class="t -a"'))]),
    ("bad-url-tokens", [('{c} > .t { fill: lime } {c} > .t { stroke: url(a"\\); fill: red) }', T), ("{c} > .t { fill: lime } {c} > .t { fill: url(#gl\\\n) }", T),
                        ("", inherited("url(#gl&#x7f;)")), ("", inherited("url(#gl&#x1;)")), ("", inherited("url(#gl&#x10;)")), ("", inherited("url(#gl&#xb;)"))]),
    ("delim-tokens", [("{c} > .t { fill: lime } @1 {c} > .t { fill: red }", T), ("{c} > .t { fill: lime } <!x {c} > .t { fill: red }", T), ("{c} > .t { fill: lime } <!-x {c} > .t { fill: red }", T),
                      ("", attribute('fill="red" style="] ; fill: lime"')), ("", attribute('fill="lime" style="stroke: ] }"'))]),
    ("declaration-forms", [("", attribute('fill="lime" style="fill"')), ("", attribute('fill="lime" style="fill: important"')), ("", attribute('fill="lime" style="fill: red important"')),
                           ("", attribute('fill="lime" style="fill: {red}"')), ("", attribute('fill="red" style=": red; fill: lime"')), ("", attribute('fill="lime" style="--: red"')),
                           ("", attribute('fill="red" style="--c: url(a b); fill: var(--c, lime)"')), ("", attribute('fill="lime" style="-x-fill: red"')),
                           ("", '<rect width="6" height="6" fill="lime"/><rect width="6" height="6" fill="red" style="-WEBKIT-clip-path: inset(50%)"/>')]),
    ("rule-forms", [("!@layer a.; {c} > .t { fill: lime }", T), ("!@import; {c} > .t { fill: lime }", T), ("!{ fill: red } {c} > .t { fill: lime }", T),
                    ("!-x { fill: red } {c} > .t { fill: lime }", T), ("!--x { fill: red } {c} > .t { fill: lime }", T)]),
    ("media-errors", [("{c} > .t { fill: lime } @media (aspect-ratio:) { {c} > .t { fill: red } }", T), ("{c} > .t { fill: lime } @media (aspect-ratio: 1 2) { {c} > .t { fill: red } }", T),
                      ("{c} > .t { fill: lime } @media (aspect-ratio: 1/) { {c} > .t { fill: red } }", T), ("{c} > .t { fill: lime } @media () { {c} > .t { fill: red } }", T),
                      ("{c} > .t { fill: lime } @media 1 { {c} > .t { fill: red } }", T), ("{c} > .t { fill: lime } @media (" + "a" * 70 + ") { {c} > .t { fill: red } }", T),
                      ("{c} > .t { fill: red } @media screen, { {c} > .t { fill: lime } }", T), ("{c} > .t { fill: red } @media , screen { {c} > .t { fill: lime } }", T)]),
    ("non-ascii", [("{c} > .a×b { fill: lime }", T.replace('class="t"', 'class="t a×b"')), ("{c} > .a b { fill: lime }", T.replace('class="t"', 'class="t a b"')),
                   ("{c} > .a b { fill: lime }", T.replace('class="t"', 'class="t a b"'))]),
])
unit.write()
