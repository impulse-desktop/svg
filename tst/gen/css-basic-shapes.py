"""css-basic-shapes: the basic shapes of CSS Shapes 1 as clip paths."""
import math

from unitgen import Unit, rows

unit = Unit("css-basic-shapes", 122, 0, """Each lime drawing fills its black
outline, and nothing red shows; each is a 16 by 12 rect clipped by a basic
shape, drawn plainly in the expected drawing. inset() takes one to four
insets as margin does, insets past the box shrink to nothing; xywh() a
position and a size; rect() its edges from the top and the left, auto the
box's own, an edge before the opposite one meeting it; all three draw the
same as the inset() they compute to, and round their corners. circle()
takes one radius, a percentage of the box's diagonal over sqrt(2), closest
side if none, and a centre, the box's if none; ellipse() two radii, each
of its axis. polygon() closes its points, nonzero unless evenodd.
A position is one or two
words, or a side of each axis with its offset from it, never three.
half-border-box is midway between the fill box and the stroke box, and
margin-box is the stroke box, as clip-path takes them.""")


def item(x, y, value):
    clip, shape = value
    colour = "red" if shape is None else "lime"
    test = f'<rect x="{x}" y="{y}" width="16" height="12" fill="{colour}" style="clip-path: {clip}"/>'

    if shape is None:
        return test, "", ""

    return test, f'<g fill="lime" transform="translate({x} {y})">{shape}</g>', f'<g transform="translate({x} {y})">{shape}</g>'


def box(x, y, w, h, r=0):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}"' + (f' rx="{r}"' if r else "") + "/>"


diagonal = math.hypot(16, 12) / math.sqrt(2)
y = rows(unit, 2, [
    ("inset", [("inset(2px)", box(2, 2, 12, 8)), ("inset(1px 3px)", box(3, 1, 10, 10)), ("inset(1px 2px 3px)", box(2, 1, 12, 8)), ("inset(1px 2px 3px 4px)", box(4, 1, 10, 8))]),
    ("inset-overflow", [("inset(12px 0 4px 0)", None), ("inset(0 10px 0 10px)", None)]),
    ("xywh", [("xywh(2px 3px 6px 4px)", box(2, 3, 6, 4)), ("xywh(25% 25% 50% 50%)", box(4, 3, 8, 6))]),
    ("rect", [("rect(2px 10px 8px 3px)", box(3, 2, 7, 6)), ("rect(25% 75% 75% 25%)", box(4, 3, 8, 6))]),
    ("rect-auto", [("rect(auto auto 6px auto)", box(0, 0, 16, 6)), ("rect(3px auto auto 4px)", box(4, 3, 12, 9))]),
    ("rect-crossed", [("rect(2px 3px 8px 6px)", None), ("rect(8px 10px 2px 3px)", None)]),
    ("equivalent", [("inset(2px 3px 4px 1px round 2px)", box(1, 2, 12, 6, 2)), ("rect(2px 13px 8px 1px round 2px)", box(1, 2, 12, 6, 2)), ("xywh(1px 2px 12px 6px round 2px)", box(1, 2, 12, 6, 2))]),
    ("round", [("inset(2px round 3px)", box(2, 2, 12, 8, 3)), ("inset(0 round 50%)", '<ellipse cx="8" cy="6" rx="8" ry="6"/>'), ("xywh(0 0 100% 100% round 4px 0 0 0)", '<path d="M0 4A4 4 0 0 1 4 0H16V12H0Z"/>')]),
    ("circle", [("circle(4px)", '<circle cx="8" cy="6" r="4"/>'), ("circle(4px at 4px 4px)", '<circle cx="4" cy="4" r="4"/>')]),
    ("circle-default", [("circle()", '<circle cx="8" cy="6" r="6"/>'), ("circle(closest-side at 4px 6px)", '<circle cx="4" cy="6" r="4"/>')]),
    ("circle-invalid", [("circle(4px 4px)", box(0, 0, 16, 12))]),
    ("circle-percent", [("circle(25%)", f'<circle cx="8" cy="6" r="{diagonal / 4:.4f}"/>')]),
    ("position", [("circle(3px at 3px 3px)", '<circle cx="3" cy="3" r="3"/>'), ("ellipse(4px 2px)", '<ellipse cx="8" cy="6" rx="4" ry="2"/>'), ("circle(3px at right top)", '<path d="M13 0A3 3 0 0 0 16 3V0Z"/>'),
                  ("circle(3px at 25% bottom)", '<path d="M1 12A3 3 0 0 1 7 12Z"/>')]),
    ("position-sides", [("ellipse(4px 2px at right 4px bottom 2px)", '<ellipse cx="12" cy="10" rx="4" ry="2"/>'), ("circle(3px at bottom 3px left 25%)", '<circle cx="4" cy="9" r="3"/>'),
                        ("circle(3px at left 3px top calc(50% - 1px))", '<circle cx="3" cy="5" r="3"/>')]),
    ("position-invalid", [("circle(3px at left 1px top)", box(0, 0, 16, 12)), ("circle(3px at left 1px right 2px)", box(0, 0, 16, 12)), ("circle(3px at center 1px top 2px)", box(0, 0, 16, 12)),
                          ("circle(3px at left top bottom 2px)", box(0, 0, 16, 12)), ("circle(3px at left 1px top 2px 3px)", box(0, 0, 16, 12)), ("circle(3px at top left)", '<path d="M3 0A3 3 0 0 1 0 3V0Z"/>')]),
    ("ellipse", [("ellipse(25% 50%)", '<ellipse cx="8" cy="6" rx="4" ry="6"/>'), ("ellipse()", '<ellipse cx="8" cy="6" rx="8" ry="6"/>'), ("ellipse(closest-side 3px at 4px 6px)", '<ellipse cx="4" cy="6" rx="4" ry="3"/>')]),
    ("polygon", [("polygon(0 0, 16px 0, 0 12px)", '<path d="M0 0H16L0 12Z"/>'), ("polygon(50% 0, 100% 100%, 0 100%)", '<path d="M8 0L16 12H0Z"/>')]),
    ("polygon-fill-rule", [("polygon(0 0, 16px 0, 16px 12px, 0 12px, 0 0, 4px 3px, 12px 3px, 12px 9px, 4px 9px, 4px 3px)", box(0, 0, 16, 12)),
                           ("polygon(evenodd, 0 0, 16px 0, 16px 12px, 0 12px, 0 0, 4px 3px, 12px 3px, 12px 9px, 4px 9px, 4px 3px)", '<path fill-rule="evenodd" d="M0 0H16V12H0Z M4 3H12V9H4Z"/>')]),
], 6, 20, 16, item)


def stroked(x, y, value):
    clip, shape = value
    test = f'<rect x="{x + 4}" y="{y + 2}" width="8" height="8" fill="lime" stroke="lime" stroke-width="4" style="clip-path: {clip}"/>'

    return test, f'<g fill="lime" transform="translate({x} {y})">{shape}</g>', f'<g transform="translate({x} {y})">{shape}</g>'


y = rows(unit, y, [
    ("half-border", [("half-border-box", box(3, 1, 10, 10)), ("inset(0) half-border-box", box(3, 1, 10, 10))]),
    ("shape-box", [("circle(50%) margin-box", '<circle cx="8" cy="6" r="6"/>'), ("circle(50%) half-border-box", '<circle cx="8" cy="6" r="5"/>'), ("circle(50%) fill-box", '<circle cx="8" cy="6" r="4"/>')]),
], 6, 20, 16, stroked)
unit.height = y + 1
unit.write()
