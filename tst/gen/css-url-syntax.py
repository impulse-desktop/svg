"""css-url-syntax: url() and src() as css-values-4 has them."""
from unitgen import Unit, rows

unit = Unit("css-url-syntax", 72, 0, """Each lime drawing fills its black
outline, and nothing red shows. A url() takes its string quoted or not,
with white space around it and escapes undone; src() takes a quoted string
the same; an unquoted url with white space, a quote or a parenthesis in it
is no url, so the presentation attribute is ignored and the fill inherits;
an empty url is an invalid resource, so the fallback paints; of two
elements of one id the first in tree order is the one; a fragment is the
document's own whatever the base; clip-path, mask, mask-image, filter and
marker-start take the same urls. Nothing outside the document loads, a
url of another file or an @import (D15), the rest of the sheet applying.""")
unit.common('''<defs>
<linearGradient id="g"><stop stop-color="lime"/></linearGradient>
<linearGradient id="a(b)"><stop stop-color="lime"/></linearGradient>
<linearGradient id="ab"><stop stop-color="lime"/></linearGradient>
<linearGradient id="dup"><stop stop-color="lime"/></linearGradient>
<linearGradient id="dup"><stop stop-color="red"/></linearGradient>
<linearGradient id="r"><stop stop-color="red"/></linearGradient>
<clipPath id="c" clipPathUnits="objectBoundingBox"><rect x=".4" width=".6" height="1"/></clipPath>
<mask id="m" maskContentUnits="objectBoundingBox"><rect x=".4" width=".6" height="1" fill="white"/></mask>
<mask id="m 2" maskContentUnits="objectBoundingBox"><rect x=".4" width=".6" height="1" fill="white"/></mask>
<filter id="f" x="0" y="0" width="1" height="1"><feFlood flood-color="lime"/></filter>
<marker id="mk" markerWidth="6" markerHeight="6" refX="0" refY="0" markerUnits="userSpaceOnUse"><rect width="6" height="6" fill="lime"/></marker>
</defs>''')


def item(x, y, value):
    kind, test = value
    test = test.format(x=x, y=y)
    whole = f'<rect x="{x}" y="{y}" width="10" height="6"/>'

    if kind == "fill":
        return f'<rect x="{x}" y="{y}" width="10" height="6" {test}/>', whole.replace("/>", ' fill="lime"/>'), whole

    if kind == "ignored":
        return f'<g fill="lime"><rect x="{x}" y="{y}" width="10" height="6" fill="red"/><rect x="{x}" y="{y}" width="10" height="6" {test}/></g>', whole.replace("/>", ' fill="lime"/>'), whole

    if kind == "clipped":
        part = f'<rect x="{x + 4}" y="{y}" width="6" height="6"/>'

        return f'<rect x="{x}" y="{y}" width="10" height="6" fill="lime" {test}/>', part.replace("/>", ' fill="lime"/>'), part

    if kind == "filled":
        return f'<rect x="{x}" y="{y}" width="10" height="6" fill="red" {test}/>', whole.replace("/>", ' fill="lime"/>'), whole

    part = f'<rect x="{x}" y="{y}" width="6" height="6"/>'

    return test, part.replace("/>", ' fill="lime"/>'), part


y = rows(unit, 2, [
    ("quoted", [("fill", 'fill="url(#g)"'), ("fill", 'fill="url(\'#g\')"'), ("fill", "fill='url(\"#g\")'"), ("fill", 'fill="url( #g )"'), ("fill", 'style="fill: url(#g)"')]),
    ("src", [("fill", 'fill="src(\'#g\')"'), ("fill", 'style="fill: src(\'#g\')"')]),
    ("escapes", [("fill", 'fill="url(\\#g)"'), ("fill", 'fill="url(#a\\(b\\))"'), ("fill", 'fill="url(#\\61 b)"')]),
    ("empty", [("fill", 'fill="url() lime"'), ("fill", 'fill="url(\'\') lime"')]),
    ("first", [("fill", 'fill="url(#dup)"'), ("fill", 'fill="url(#g) red"')]),
    ("fragment", [("fill", 'fill="url(#g)" xml:base="http://example.com/"'), ("fill", 'fill="url(#ab) red" xml:base="other/"')]),
    ("unquoted-invalid", [("ignored", 'fill="url(#g x)"'), ("ignored", 'fill="src(#g)"'), ("ignored", 'fill="url(#g\'x)"'), ("ignored", 'fill="url(#g(x)"')]),
    ("properties", [("clipped", 'clip-path="url(\'#c\')"'), ("clipped", 'style="mask: src(\'#m\')"'), ("clipped", 'style="mask-image: url(#m\\ 2)"'), ("filled", 'style="filter: src(\'#f\')"'), ("marker", '<path d="M{x} {y}h4" style="marker-start: url(\'#mk\')"/>')]),
    ("external", [("fill", 'fill="url(other.svg#r) lime"'), ("fill", 'class="imported" fill="red"')]),
], 6, 12, 8, item)
unit.common("<style>@import url(other.css); .imported { fill: lime }</style>")
unit.height = y + 1
unit.write()
