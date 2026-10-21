"""A use's context paints take the box of its sized symbol instance.
The expected drawings use solid rectangles, without context paints."""
from unitgen import Unit


unit = Unit("use-symbol-context", 64, 208, """Context paint servers use the
geometry of the whole symbol instance, with its viewport size, viewBox,
reference point and transforms. Measuring one instance leaves the size
and context of the next intact.""")
unit.common('''<defs>
<linearGradient id="half"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="blue"/></linearGradient>
<linearGradient id="ends"><stop offset=".2" stop-color="lime"/><stop offset=".2" stop-color="red"/><stop offset=".8" stop-color="red"/><stop offset=".8" stop-color="lime"/></linearGradient>
<pattern id="tiles" width=".5" height="1" patternContentUnits="objectBoundingBox"><rect width=".25" height="1" fill="lime"/><rect x=".25" width=".25" height="1" fill="blue"/></pattern>
<symbol id="plain"><rect width="16" height="8" fill="context-fill"/></symbol>
<symbol id="stroke" overflow="visible"><path d="M 0,0 H 16 V 8 H 0 Z" fill="none" stroke="context-stroke" stroke-width="2"/></symbol>
<symbol id="two"><rect width="4" height="8" fill="context-fill"/><rect x="20" width="4" height="8" fill="context-fill"/></symbol>
<symbol id="view" viewBox="10 20 24 8"><rect x="10" y="20" width="4" height="8" fill="context-fill"/><rect x="30" y="20" width="4" height="8" fill="context-fill"/></symbol>
<symbol id="percent" width="40" height="20"><rect width="100%" height="100%" fill="context-fill"/></symbol>
<symbol id="ref" viewBox="0 0 16 8" refX="8" refY="4" overflow="visible"><rect width="16" height="8" fill="context-fill"/></symbol>
<symbol id="transformed" transform="translate(4 0) scale(2)" overflow="visible"><rect width="16" height="8" fill="context-fill"/></symbol>
<symbol id="nested"><use href="#percent" width="8" height="8" fill="url(#half) red"/><rect x="16" width="16" height="8" fill="context-fill"/></symbol>
<svg id="svg" width="40" height="20" viewBox="0 0 16 8"><rect width="16" height="8" fill="context-fill"/></svg>
</defs>''', "")


def rect(x, y, w, h, color="lime"):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{color}"/>'


def halves(x, y, w=16, h=8):
    return rect(x, y, w / 2, h) + rect(x + w / 2, y, w / 2, h, "blue")


def cell(name, row, test, expected):
    y = row * 16 + 4
    unit.cell(name, (0, row * 16, 64, 16), f'<g transform="translate(4 {y})">{test}</g>', f'<g transform="translate(4 {y})">{expected}</g>')


cell("gradient", 0, '<use href="#plain" fill="url(#half) red"/>', halves(0, 0))
cell("pattern", 1, '<use href="#plain" fill="url(#tiles) red"/>', "".join(rect(i * 4, 0, 4, 8, "blue" if i % 2 else "lime") for i in range(4)))
# The stroke's geometry box is 16 by 8, independent of its width. Clip the
# outline into the two solid colours at its midpoint in the plain drawing.
unit.common('<defs><clipPath id="left"><rect x="-2" y="-2" width="10" height="12"/></clipPath><clipPath id="right"><rect x="8" y="-2" width="10" height="12"/></clipPath></defs>')
outline = '<path d="M 0,0 H 16 V 8 H 0 Z" fill="none" stroke="{color}" stroke-width="2" clip-path="url(#{side})"/>'
cell("stroke", 2, '<use href="#stroke" stroke="url(#half) red"/>', outline.format(color="lime", side="left") + outline.format(color="blue", side="right"))
cell("whole-instance", 3, '<use href="#two" fill="url(#ends) red"/>', rect(0, 0, 4, 8) + rect(20, 0, 4, 8))
cell("viewbox", 4, '<use href="#view" width="48" height="8" fill="url(#ends) red"/>', rect(12, 0, 4, 8) + rect(32, 0, 4, 8))
cell("sized-percent", 5, '<use href="#percent" width="16" height="8" fill="url(#half) red"/>', halves(0, 0))
cell("default-size", 6, '<use href="#percent" fill="url(#half) red" transform="scale(.5)"/>', halves(0, 0, 20, 10))
cell("reference-point", 7, '<use href="#ref" x="8" y="4" width="16" height="8" fill="url(#half) red"/>', halves(0, 0))
cell("target-transform", 8, '<use href="#transformed" fill="url(#half) red" transform="scale(.5)"/>', halves(2, 0))
cell("nested-sizes", 9, '<use href="#nested" fill="url(#half) red"/>', halves(0, 0, 8) + rect(16, 0, 16, 8, "blue"))
cell("svg-size", 10, '<use href="#svg" width="16" height="8" fill="url(#half) red"/>', halves(0, 0))
cell("missing-server", 11, '<use href="#plain" fill="url(#missing) lime"/>', rect(0, 0, 16, 8))
cell("svg-default-size", 12, '<use href="#svg" fill="url(#half) red" transform="scale(.5)"/>', halves(0, 0, 20, 10))
unit.write()
