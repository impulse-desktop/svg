"""css-shape-function: shape() of CSS Shapes 1 as clip paths."""
from unitgen import Unit, rows

unit = Unit("css-shape-function", 122, 0, """Each lime drawing fills its
black outline, and nothing red shows; each is a 16 by 12 rect clipped by a
shape(), drawn in the expected drawing as the SVG path data it stands for.
shape() takes a fill rule, a start and commands, with CSS lengths,
percentages and calc(): move and line to a position or by a pair from the
start, hline and vline with their axis' keywords, curve to a position with
one control point, a quadratic, or two, a cubic, by a pair with controls
from the start, the end or the origin, smooth with the reflected control,
or the start after no curve, close; arc of one radius, a share of the
box's diagonal over sqrt(2), or two, counter-clockwise and small unless cw
or large, turned by rotate, as SVG's arc, which draws nothing between equal
points, a line for a zero radius, takes a negative one's size and scales
radii too small up. A shape() with a word out of its grammar is invalid,
so nothing is clipped.""")


def item(x, y, value):
    clip, d = value
    test = f'<rect x="{x}" y="{y}" width="16" height="12" fill="lime" style="clip-path: shape({clip})"/>'
    shape = f'<path d="{d}"/>' if "fill-rule" not in d else d

    return test, f'<g fill="lime" transform="translate({x} {y})">{shape}</g>', f'<g transform="translate({x} {y})">{shape}</g>'


y = rows(unit, 2, [
    ("syntax", [("from 0 0, line to 100% 0, line to 0 100%, close", "M0 0H16L0 12Z"), ("from calc(0% + 0px) 0, line to calc(50% + 8px) 0, line to 0 calc(100%), close", "M0 0H16L0 12Z"),
                ("evenodd from 0 0, hline to right, vline to bottom, hline to left, close, move to 4px 3px, hline by 8px, vline by 6px, hline by -8px, close", '<path fill-rule="evenodd" d="M0 0H16V12H0Z M4 3h8v6h-8Z"/>'),
                ("nonzero from 0 0, hline to right, vline to bottom, hline to left, close, move to 4px 3px, hline by 8px, vline by 6px, hline by -8px, close", "M0 0H16V12H0Z")]),
    ("to-by", [("from 2px 2px, line by 12px 0, line by 0 8px, line to 2px 10px, close", "M2 2h12v8H2Z"), ("from 2px 2px, line to 14px 2px, line by 0 8px, line by -12px 0, close", "M2 2H14V10H2Z")]),
    ("move-line", [("from 0 0, line to 6px 0, line to 0 6px, close, move to 10px 6px, line to 16px 6px, line to 10px 12px, close", "M0 0H6L0 6Z M10 6H16L10 12Z"),
                   ("from 0 0, line to 6px 0, line to 0 6px, close, move by 10px 6px, line by 6px 0, line by -6px 6px, close", "M0 0H6L0 6Z M10 6H16L10 12Z")]),
    ("hline-vline", [("from 2px 2px, hline to right, vline to bottom, hline to x-start, close", "M2 2H16V12H0Z"), ("from 0 0, hline to center, vline to y-end, hline to left, close", "M0 0H8V12H0Z"),
                     ("from 16px 12px, vline to top, hline by -8px, vline to center, hline to x-end, close", "M16 12V0H8V6H16Z")]),
    ("curve", [("from 0 12px, curve to 16px 12px with 8px 0, close", "M0 12Q8 0 16 12Z"), ("from 0 12px, curve to 16px 12px with 0 0 / 16px 0, close", "M0 12C0 0 16 0 16 12Z"),
               ("from 0 12px, curve by 16px 0 with 8px -12px, close", "M0 12Q8 0 16 12Z")]),
    ("smooth", [("from 0 6px, curve to 8px 6px with 4px 0, smooth to 16px 6px, line to 16px 12px, line to 0 12px, close", "M0 6Q4 0 8 6T16 6V12H0Z"),
                ("from 0 12px, line to 8px 6px, smooth to 16px 12px, close", "M0 12L8 6L16 12Z"),
                ("from 0 6px, curve to 8px 6px with 2px 0 / 6px 0, smooth to 16px 6px with 14px 0, line to 16px 12px, line to 0 12px, close", "M0 6C2 0 6 0 8 6S14 0 16 6V12H0Z")]),
    ("control", [("from 0 12px, curve by 16px 0 with 8px -12px from start, close", "M0 12Q8 0 16 12Z"), ("from 0 12px, curve by 16px 0 with -8px -12px from end, close", "M0 12Q8 0 16 12Z"),
                 ("from 0 12px, curve by 16px 0 with 8px 0 from origin, close", "M0 12Q8 0 16 12Z")]),
    ("arc", [("from 6px 6px, arc to 10px 6px of 3px, close", "M6 6A3 3 0 0 0 10 6Z"), ("from 6px 6px, arc to 10px 6px of 3px cw, close", "M6 6A3 3 0 0 1 10 6Z"),
             ("from 6px 6px, arc to 10px 6px of 3px large, close", "M6 6A3 3 0 1 0 10 6Z"), ("from 6px 6px, arc to 10px 6px of 3px large cw, close", "M6 6A3 3 0 1 1 10 6Z"),
             ("from 6px 6px, arc to 10px 6px of 4px 2px rotate 30deg large, close", "M6 6A4 2 30 1 0 10 6Z"), ("from 6px 6px, arc by 4px 0 of 25% cw, close", "M6 6A3.5355 3.5355 0 0 1 10 6Z")]),
    ("arc-edges", [("from 0 0, line to 16px 0, arc to 16px 0 of 4px, line to 0 12px, close", "M0 0H16L0 12Z"), ("from 0 0, line to 16px 0, arc to 16px 12px of 0, close", "M0 0H16V12Z"),
                   ("from 6px 6px, arc to 10px 6px of -3px large, close", "M6 6A3 3 0 1 0 10 6Z"), ("from 6px 6px, arc to 10px 6px of 1px, close", "M6 6A2 2 0 0 0 10 6Z")]),
    ("invalid", [("from 0 0, curve to 16px 12px with 8px 0 x, close", "M0 0H16V12H0Z"), ("from 0 0, curve by 16px 12px with 8px 0 x, close", "M0 0H16V12H0Z")]),
], 6, 20, 16, item)
unit.height = y + 1
unit.write()
