"""href-forms: the forms of a reference, of SVG 2's linking: an href is a
URL, not a url(); a presentation attribute takes a url(), not a URL; href
outranks xlink:href."""
from unitgen import Unit

unit = Unit("href-forms", 52, 12, """Lime everywhere, no red. An href
holding url() refers to nothing; a presentation attribute holding a bare URL
is invalid, and the property inherits; href is used over xlink:href, in
either order, an empty href too; xlink:href serves alone, under any prefix
of its namespace.""", root='xmlns:xlink="http://www.w3.org/1999/xlink" xmlns:l="http://www.w3.org/1999/xlink"')
unit.common('<defs><rect id="red" width="6" height="6" fill="red"/><rect id="lime" width="6" height="6" fill="lime"/>'
            '<linearGradient id="g"><stop stop-color="red"/></linearGradient></defs>')

LIME = '<rect x="{}" y="{}" width="6" height="6" fill="lime"/>'
OUTLINE = '<rect x="{}" y="{}" width="6" height="6"/>'


def cell(name, x, y, test):
    unit.cell(name, (x - 1, y - 1, 8, 8), test, LIME.format(x, y), OUTLINE.format(x, y))


cell("url-in-href", 2, 2, f'{LIME.format(2, 2)}<use href="url(#red)" x="2" y="2"/>')
cell("bare-url-attribute", 10, 2, '<g fill="lime"><rect x="10" y="2" width="6" height="6" fill="#g"/></g>')
cell("href-first", 18, 2, '<use href="#lime" xlink:href="#red" x="18" y="2"/>')
cell("xlink-first", 26, 2, '<use xlink:href="#red" href="#lime" x="26" y="2"/>')
cell("empty-href", 34, 2, f'{LIME.format(34, 2)}<use href="" xlink:href="#red" x="34" y="2"/>')
cell("xlink-alone", 42, 2, '<use l:href="#lime" x="42" y="2"/>')

unit.write()
