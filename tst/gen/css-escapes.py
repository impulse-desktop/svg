"""css-escapes: the escapes of CSS Syntax 3 wherever CSS is read."""
from unitgen import TARGET as T, Unit, squares


def red(attributes):
    return f'<rect {attributes} width="6" height="6" fill="red"/>'


unit = Unit("css-escapes", 0, 0, """Each lime square fills its black outline,
and nothing red shows. CSS escapes, of css-syntax-3: a backslash before a
character not a hex digit nor a newline is that character, before one to
six hex digits and a white space after them the code point, U+FFFD for
zero, a surrogate or one past the last; a backslash before a newline in a
string is nothing, at the end of the input U+FFFD; a newline escaped in an
identifier, one in a string or a lone - invalidate the selector. They go in
class, id, type, attribute and pseudo-class names, attribute values quoted
or not, property names and values, a style attribute, and presentation
attributes, which take comments too. A value is read from its tokens, so
an escaped parenthesis is part of an identifier, no parenthesis: rgb\28 0
255 0\29 is no colour (D32).""")
unless = "{c} > * { fill: red } {c} > .t { fill: lime }"
squares(unit, [
    ("literal", [("{c} > .a\\:b { fill: lime }", red('class="a:b"')), ("{c} > .t:fir\\st-child { fill: lime }", T), ("{c} > .t { fill: l\\ime }", T), ("{c} > .\\--x { fill: lime }", red('class="--x"')),
                 ('{c} > [data-x="a\\"b"] { fill: lime }', red("data-x='a\"b'"))]),
    ("hex", [("{c} > .\\61 b { fill: lime }", red('class="ab"')), ("{c} > #\\31 23-esc { fill: lime }", red('id="123-esc"')), ("{c} > \\72 ect { fill: lime }", T),
             ("{c} > [d\\61 ta-x] { fill: lime }", red('data-x=""')), ('{c} > [data-x="\\61 b"] { fill: lime }', red('data-x="ab"')), ("{c} > [data-x=\\61 b] { fill: lime }", red('data-x="ab"')),
             ("{c} > .t { f\\69ll: lime }", T), ("{c} > .t { fill: \\6c ime }", T), ("{c} > .x\\61y { fill: lime }", red('class="xay"')), ("{c} > .\\000061 b { fill: lime }", red('class="ab"')),
             ("{c} > .\\31 0 { fill: lime }", red('class="10"'))]),
    ("replacement", [("{c} > .z\\0 { fill: lime }", red('class="z&#xFFFD;"')), ("{c} > .s\\d800 { fill: lime }", red('class="s&#xFFFD;"')), ("{c} > .o\\110000 { fill: lime }", red('class="o&#xFFFD;"'))]),
    ("continuation", [('{c} > [data-x="a\\\nb"] { fill: lime }', red('data-x="ab"'))]),
    ("eof", [("", '<g fill="lime"><rect class="t" width="6" height="6" style="fill: red\\"/></g>'), ("!{c} > .t { fill: lime } {c} > .t { fill: red\\", T)]),
    ("invalid", [("{c} > .t\\\n, " + unless, T), ('{c} > [data-x="a\n], ' + unless, T), ("{c} > .-, " + unless, T)]),
    ("attributes", [("", '<rect class="t" width="6" height="6" fill="red" style="f\\69ll: \\6c ime"/>'), ("", '<rect class="t" width="6" height="6" fill="\\6c ime"/>'),
                    ("", '<rect class="t" width="6" height="6" fill="lime" style="fill: rgb\\28 0 255 0\\29"/>')]),
    ("comments", [("", '<rect width="6" height="6" fill="red"/><rect width="3" height="6" fill="lime"/><rect x="3" width="3\\px/**/" height="6" fill="lime"/>'),
                  ("", '<rect class="t" width="6" height="6" fill="/* c */lime/**/"/>')]),
])
unit.write()
