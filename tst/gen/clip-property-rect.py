"""clip-property-rect: the deprecated clip property of CSS Masking on SVG
viewports and patterns."""
from unitgen import Unit, rows

unit = Unit("clip-property-rect", 122, 0, """Each lime drawing fills its black
outline, and nothing red shows. clip cuts an svg viewport and a pattern's
tiles to rect(top, right, bottom, left), each side's offset from the box's
top or left edge, negative ones past it, auto the box's own edge; auto
clips nothing; a group is no viewport, so clip does nothing there; commas
or none, not a mix; clip is a presentation attribute, whose lengths may be
plain numbers, as in a declaration too (D43); a rect whose bottom is above its
top, or right left of its left, clips everything; a symbol's instance and a
mask are cut too.""")


def svg(attributes, content='<rect x="-4" y="-4" width="24" height="24" fill="lime"/>'):
    return '<svg x="{x}" y="{y}" width="16" height="16" overflow="visible" %s>%s</svg>' % (attributes, content)


def item(x, y, value):
    test, rects = value
    plain = "".join('<rect x="%g" y="%g" width="%g" height="%g" fill="lime"/>' % (x + rx, y + ry, rw, rh) for rx, ry, rw, rh in rects)

    return test.format(x=x, y=y), plain, plain.replace(' fill="lime"', "")


pattern = ('<pattern id="p{x}" x="{x}" y="{y}" width="8" height="8" patternUnits="userSpaceOnUse" style="clip: rect(auto, 6px, 6px, 1px)"><rect width="8" height="8" fill="lime"/></pattern>'
           '<rect x="{x}" y="{y}" width="16" height="16" fill="url(#p{x})"/>')
y = rows(unit, 2, [
    ("applies", [(svg('style="clip: rect(2px, 14px, 14px, 2px)"'), [(2, 2, 12, 12)]), (pattern, [(1, 0, 5, 6), (9, 0, 5, 6), (1, 8, 5, 6), (9, 8, 5, 6)]),
                 ('<g style="clip: rect(0, 0, 0, 0)"><rect x="{x}" y="{y}" width="16" height="16" fill="lime"/></g>', [(0, 0, 16, 16)])]),
    ("auto", [(svg('style="clip: auto"'), [(-4, -4, 24, 24)]), (svg('style="clip: rect(auto, auto, auto, auto)"'), [(0, 0, 16, 16)])]),
    ("offsets", [(svg('style="clip: rect(-2px, 18px, 18px, -2px)"'), [(-2, -2, 20, 20)]), (svg('style="clip: rect(4px, 12px, 8px, 2px)"'), [(2, 4, 10, 4)])]),
    ("auto-values", [(svg('style="clip: rect(auto, 10px, auto, 4px)"'), [(4, 0, 6, 16)]), (svg('style="clip: rect(3px, auto, 12px, auto)"'), [(0, 3, 16, 9)])]),
    ("commas", [(svg('style="clip: rect(2px, 14px, 14px, 2px)"'), [(2, 2, 12, 12)]), (svg('style="clip: rect(2px 14px 14px 2px)"'), [(2, 2, 12, 12)]),
                (svg('style="clip: rect(2px, 14px 14px, 2px)"'), [(-4, -4, 24, 24)]), (svg('style="clip: rect(2px 14px, 14px, 2px)"'), [(-4, -4, 24, 24)]),
                (svg('style="clip: rect(,2px, 14px, 14px, 2px)"'), [(-4, -4, 24, 24)]), (svg('style="clip: rect(2px, 14px, 14px, 2px,)"'), [(-4, -4, 24, 24)]),
                (svg('style="clip: rect(2px,, 14px, 14px, 2px)"'), [(-4, -4, 24, 24)])]),
    ("attribute", [(svg('clip="rect(2px, 14px, 14px, 2px)"'), [(2, 2, 12, 12)])]),
    ("numbers", [(svg('clip="rect(2, 14, 14, 2)"'), [(2, 2, 12, 12)]), (svg('style="clip: rect(2, 14, 14, 2)"'), [(2, 2, 12, 12)]), (svg('style="clip: rect(0, 14px, 14px, 0)"'), [(0, 0, 14, 14)])]),
    ("inverted", [(svg('style="clip: rect(5px, 4px, 3px, 2px)"'), []), (svg('style="clip: rect(2px, 4px, 14px, 6px)"'), [])]),
    ("symbol", [('<symbol id="s{x}" overflow="visible" style="clip: rect(2px, 14px, 14px, 2px)"><rect x="-4" y="-4" width="24" height="24" fill="lime"/></symbol><use href="#s{x}" x="{x}" y="{y}" width="16" height="16"/>', [(2, 2, 12, 12)])]),
    ("mask", [('<mask id="k{x}" maskUnits="userSpaceOnUse" x="{x}" y="{y}" width="16" height="16" style="clip: rect(2px, 14px, 14px, 2px)"><rect x="{x}" y="{y}" width="16" height="16" fill="white"/></mask>'
               '<rect x="{x}" y="{y}" width="16" height="16" fill="lime" mask="url(#k{x})"/>', [(2, 2, 12, 12)])]),
], 5, 24, 24, item, x=6)
unit.height = y + 4
unit.write()
