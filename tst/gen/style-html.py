"""style-html: an HTML style element in an SVG document."""
from unitgen import Unit

X = 'xmlns="http://www.w3.org/1999/xhtml"'
unit = Unit("style-html", 58, 10, """Each lime square fills its black outline,
and nothing red shows. A style element in the XHTML namespace is a style
sheet as SVG's is, as HTML processes it, wherever it is in the document: a
child of an SVG element, inside another element of that namespace, in a
foreignObject's content (D47); its media and type attributes are read as
an SVG style element's. Another element of that namespace is no style
sheet.""")


def square(k, name, markup, lime):
    x = 2 + k * 8
    fill = "red" if lime else "lime"
    test = f'{markup.format(c=f"h{k}")}<rect class="h{k}" x="{x}" y="2" width="6" height="6" fill="{fill}"/>'
    unit.cell(name, (x - 1, 1, 8, 8), test, f'<rect x="{x}" y="2" width="6" height="6" fill="lime"/>', f'<rect x="{x}" y="2" width="6" height="6"/>')


square(0, "child", f'<style {X}>.{{c}} {{{{ fill: lime }}}}</style>', True)
square(1, "nested", f'<defs {X}><div><style>.{{c}} {{{{ fill: lime }}}}</style></div></defs>', True)
square(2, "foreign-object", f'<foreignObject width="0" height="0"><body {X}><style>.{{c}} {{{{ fill: lime }}}}</style></body></foreignObject>', True)
square(3, "media", f'<style {X} media="print">.{{c}} {{{{ fill: red }}}}</style>', False)
square(4, "type", f'<style {X} type="text/plain">.{{c}} {{{{ fill: red }}}}</style>', False)
square(5, "other-element", f'<div {X}>.{{c}} {{{{ fill: red }}}}</div>', False)
square(6, "prefixed", '<h:style xmlns:h="http://www.w3.org/1999/xhtml">.{c} {{ fill: lime }}</h:style>', True)
unit.write()
