"""css-attribute-matchers: the attribute selectors of Selectors 4."""
from unitgen import TARGET as T, Unit, squares

unit = Unit("css-attribute-matchers", 0, 0, """Each lime square fills its
black outline, and nothing red shows. Attribute selectors: presence,
equality, a whole word, a value or its prefix before a -, a prefix, a
suffix, a substring; values compared exactly as XML has them unless an i
flag, of either case, asks for ASCII case-insensitivity, which an s flag
never does; no word, prefix, suffix or substring is empty or has white
space; no name, no value, an unknown operator or flag, or a second value
invalidates the selector.""")


def red(value):
    return f'<rect class="t" data-x="{value}" width="6" height="6" fill="red"/>'


def target(value):
    return f'<rect class="t" data-x="{value}" width="6" height="6"/>'


unless = "{c} > * { fill: red } {c} > .t { fill: lime }"
squares(unit, [
    ("presence-equality", [("{c} > [data-x] { fill: lime }", red("")), ("{c} > [data-x=ab] { fill: lime }", red("ab")), ('{c} > [ data-x = "a" ] { fill: lime }', red("a")),
                           ("{c} > [data-x=a] { fill: red } {c} > .t { fill: lime }", target("ab"))]),
    ("word", [("{c} > [data-x~=cd] { fill: lime }", red("ab cd ef")), ("{c} > [data-x~=c] { fill: red } {c} > .t { fill: lime }", target("ab cd")),
              ('{c} > [data-x~="a b"], {c} > [data-x~=""] { fill: red } {c} > .t { fill: lime }', target("a b"))]),
    ("dash", [("{c} > [data-x|=en] { fill: lime }", red("en-gb")), ("{c} > [data-x|=en] { fill: lime }", red("en")), ("{c} > [data-x|=en] { fill: red } {c} > .t { fill: lime }", target("english"))]),
    ("substrings", [("{c} > [data-x^=ab] { fill: lime }", red("abc")), ("{c} > [data-x$=bc] { fill: lime }", red("abc")), ("{c} > [data-x*=b] { fill: lime }", red("abc")),
                    ('{c} > [data-x^=""], {c} > [data-x$=""], {c} > [data-x*=""] { fill: red } {c} > .t { fill: lime }', target("a b"))]),
    ("case-sensitive", [("{c} > [data-x=AB] { fill: red } {c} > .t { fill: lime }", target("ab")), ("{c} > [DATA-X] { fill: red } {c} > .t { fill: lime }", target("ab"))]),
    ("case-i", [('{c} > [data-x="AB" i] { fill: lime }', red("ab")), ('{c} > [data-x="AB"i] { fill: lime }', red("ab")), ("{c} > [data-x~=CD i] { fill: lime }", red("ab cd")),
                ("{c} > [data-x|=EN i] { fill: lime }", red("en-gb")), ("{c} > [data-x|=EN i] { fill: lime }", red("En")), ("{c} > [data-x^=AB i] { fill: lime }", red("abc")),
                ("{c} > [data-x$=BC i] { fill: lime }", red("abc")), ("{c} > [data-x*=B i] { fill: lime }", red("abc")), ("{c} > [data-x*=B i] { fill: red } {c} > .t { fill: lime }", target("ac"))]),
    ("case-flags", [('{c} > [data-x="AB" I] { fill: lime }', red("ab")), ('{c} > [data-x="AB" s] { fill: red } {c} > .t { fill: lime }', target("ab")),
                    ('{c} > [data-x="AB" S] { fill: red } {c} > .t { fill: lime }', target("ab"))]),
    ("invalid", [("{c} > [], " + unless, T), ("{c} > [data-x=], " + unless, T), ("{c} > [data-x%=a], " + unless, T), ('{c} > [data-x="a" "b"], ' + unless, T),
                 ('{c} > [data-x="ab" q], ' + unless, target("ab"))]),
])
unit.write()
