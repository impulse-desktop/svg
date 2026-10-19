"""css-lang: the :lang() pseudo-class of Selectors 4 over the lang and
xml:lang attributes of SVG 2."""
from unitgen import TARGET as T, Unit, squares


def tagged(language, inner=T, attribute="xml:lang"):
    return f'<g {attribute}="{language}">{inner}</g>'


unit = Unit("css-lang", 0, 0, """Each lime square fills its black outline,
and nothing red shows. :lang() takes a list of language ranges, idents or
strings, and matches an element whose language, its xml:lang, else its lang,
else its parent's, an empty one none, falls in a range by extended
filtering: ASCII case-insensitive, the first subtags alike or *, each later
subtag found in turn, past others but never past a singleton; "*" matches
any language but none, "" no language alone; an ill-formed range or
language matches nothing, yet the selector stays valid. It weighs as a
class. An instance's language past its root is its use's.""")
squares(unit, [
    ("language", [("{c} .t:lang(de) { fill: lime }", tagged("de")), ("{c} .t:lang(de) { fill: lime }", tagged("de", attribute="lang")),
                  ("{c} .t:lang(de) { fill: lime }", tagged("de", f"<g><g>{T}</g></g>")), ("{c} .t:lang(de) { fill: lime }", T.replace("/>", ' xml:lang="de"/>')),
                  ("{c} .t { fill: lime } {c} .t:lang(fr) { fill: red }", '<g xml:lang="de" lang="fr">' + T + "</g>"),
                  ("{c} .t:lang(de) { fill: lime }", '<g lang="fr" xml:lang="de">' + T + "</g>"),
                  ("{c} .t { fill: lime } {c} .t:lang(de) { fill: red }", tagged("de", tagged("fr"))), ("{c} .t { fill: lime } {c} .t:lang(de) { fill: red }", tagged("de", tagged("")))]),
    ("filtering", [("{c} .t:lang(en) { fill: lime }", tagged("en-US")), ("{c} .t { fill: lime } {c} .t:lang(en-US) { fill: red }", tagged("en")),
                   ("{c} .t { fill: lime } {c} .t:lang(en-GB) { fill: red }", tagged("en-US")), ("{c} .t:lang(de-DE) { fill: lime }", tagged("de-Latn-DE-1996")),
                   ("{c} .t { fill: lime } {c} .t:lang(de-DE) { fill: red }", tagged("de-x-DE")), ("{c} .t:lang(DE-ch) { fill: lime }", tagged("de-CH")),
                   ("{c} .t { fill: lime } {c} .t:lang(de) { fill: red }", tagged("den")), ("{c} .t:lang(fr, de) { fill: lime }", tagged("de"))]),
    ("wildcards", [('{c} .t:lang("*-CH") { fill: lime }', tagged("de-CH")), ("{c} .t:lang(\\*-CH) { fill: lime }", tagged("it-CH")),
                   ('{c} .t:lang("de-*-DE") { fill: lime }', tagged("de-Latn-DE")), ('{c} .t:lang("*") { fill: lime }', tagged("und")),
                   ('{c} .t { fill: lime } {c} .t:lang("*") { fill: red }', tagged("")), ('{c} .t { fill: lime } {c} .t:lang("*") { fill: red }', T)]),
    ("untagged", [('{c} .t:lang("") { fill: lime }', T), ('{c} .t:lang("") { fill: lime }', tagged("")), ('{c} .t { fill: lime } {c} .t:lang("") { fill: red }', tagged("de"))]),
    ("ill-formed", [("{c} .t { fill: lime } {c} .t:lang(åå) { fill: red }", tagged("åå")), ("{c} .t:lang(åå), {c} .t { fill: lime }", T),
                    ("{c} .t { fill: lime } {c} .t:lang('de,fr') { fill: red }", tagged("de")), ("{c} .t { fill: lime } {c} .t:lang(abcdefghi) { fill: red }", tagged("abcdefghi")),
                    ("{c} .t { fill: lime } {c} .t:lang(de) { fill: red }", tagged("de--x")), ("{c} .t { fill: lime } {c} .t:lang(\\31 de) { fill: red }", tagged("1de")),
                    ("{c} .t:lang(de-1996) { fill: lime }", tagged("de-1996"))]),
    ("syntax", [("{c} .t { fill: lime } {c} .t:lang() { fill: red }", tagged("de")), ("{c} .t { fill: lime } {c} .t:lang(,de) { fill: red }", tagged("de")),
                ("{c} .t { fill: lime } {c} .t:lang(de fr) { fill: red }", tagged("de")), ("{c} .t { fill: lime } {c} .t:lang(5) { fill: red }", tagged("de")),
                ('!{c} .t { fill: lime } {c} .t:lang("de\n) { fill: red }', tagged("de"))]),
    ("specificity", [("{c} .t:lang(de) { fill: lime } {c} .t { fill: red }", tagged("de"))]),
    ("instance", [(".lu { fill: red } .lu:lang(de) { fill: lime }", '<defs xml:lang="fr"><rect id="lu" class="lu" width="6" height="6"/></defs>' + tagged("de", '<use href="#lu"/>')),
                  (".lv { fill: red } .lv:lang(fr) { visibility: hidden } .lv:lang(de) { fill: lime }",
                   '<defs><rect id="lv" class="lv" width="6" height="6"/></defs><use href="#lv" xml:lang="fr"/><use href="#lv" xml:lang="de"/>')]),
])
unit.write()
