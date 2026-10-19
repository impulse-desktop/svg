"""path-data-commands: the d property, path data and its commands, of SVG 2. The
expected drawing spells each path out in absolute movetos and linetos, an
arc as the cubics of its circle found by the spec's conversion from end
points to a center (implnote F.6.5)."""
import math

from unitgen import Unit

LINE = '<path d="{}" fill="none" stroke="lime" stroke-width="2"/>'
BOX = '<filter id="{}" x="0" y="0" width="1" height="1"><feFlood flood-color="lime"/></filter>'

unit = Unit("path-data-commands", 64, 52, """Each lime stroke follows the same path
spelled out in black, and each lime square covers its red one alone; no red.
A d of none, as a property too, and data with no valid command draw nothing
and add nothing to the group's bounding box, which the square fills; a unit
on a number, a comma in it and a point that ends it are errors, the path
drawn up to the segment before. A first relative moveto is absolute, the
pairs after a moveto are linetos, H and V take lists, a closepath joins
where a lineto back leaves butt ends, and the arc flags pick one of four
arcs.""")
unit.common(f"<defs>{BOX.format('none-box')}{BOX.format('empty-box')}</defs>")


def boxed(name, box, x, y, paths):
    """Paths that should draw nothing and measure nothing: drawn red on
    their own, and in a group with a red square whose filter floods its
    bounding box lime."""
    unit.cell(name, box, f'<g fill="none" stroke="red" stroke-width="2">{paths.format(x=x + 3, y=y + 3)}</g><g filter="url(#{name}-box)"><rect x="{x}" y="{y}" width="6" height="6" fill="red"/>{paths.format(x=0, y=0)}</g>',
              f'<rect x="{x}" y="{y}" width="6" height="6" fill="lime"/>', f'<rect x="{x}" y="{y}" width="6" height="6"/>')


def stroke(name, box, test, plain):
    unit.cell(name, box, LINE.format(test), LINE.format(plain), f'<path d="{plain}"/>')


# Each path, if it drew or measured, would run from x, y: inside the cell
# drawn on its own, from the origin in the filtered group.
boxed("none", (40, 1, 8, 8), 41, 2, '<path d="none"/><path/><path d="M {x} {y} h 2" style="d: none"/>')
boxed("empty", (52, 1, 8, 8), 53, 2, '<path d="L {x} {y} h 2"/><path d="X {x} {y} h 2"/><path d="{x} {y} h 2"/><path d=""/>')
stroke("unit-error", (1, 1, 10, 4), "M 2 3 h 4 L 10px 3 h 2", "M 2 3 H 6")
stroke("comma-separates", (12, 1, 10, 4), "M 13 3 h 1,2", "M 13 3 H 16")
stroke("trailing-point", (24, 1, 12, 4), "M 25 3 h 4. 3", "M 25 3 H 29")
stroke("comma-in-number", (1, 6, 10, 4), "M 2 8 h 1,000 2", "M 2 8 H 3 H 3 H 5")
stroke("moveto-pairs", (1, 12, 14, 8), "M 2 14 6 14 6 18 m 2 -4 4 0 0 4", "M 2 14 L 6 14 L 6 18 M 8 14 L 12 14 L 12 18")
stroke("relative-first", (16, 12, 8, 8), "m 17 14 4 0 0 4", "M 17 14 L 21 14 L 21 18")
stroke("hv-lists", (25, 12, 14, 12), "M 26 14 H 30 34 V 18 22 h -2 -2 v -2 -2", "M 26 14 L 30 14 L 34 14 L 34 18 L 34 22 L 32 22 L 30 22 L 30 20 L 30 18")
# A rectangle closed by Z joins at its first corner, mitred; closed by a
# lineto back, its two ends there are butt, leaving the corner's square out.
unit.cell("closepath-join", (40, 12, 23, 10),
          '<g fill="none" stroke="lime" stroke-width="2"><path d="M 42 14 h 8 v 6 h -8 Z"/><path d="M 54 14 h 8 v 6 h -8 V 14"/></g>',
          '<path d="M 41 13 H 51 V 21 H 41 Z M 43 15 V 19 H 49 V 15 Z M 54 13 H 63 V 21 H 53 V 14 H 54 Z M 55 15 V 19 H 61 V 15 Z" fill="lime"/>',
          '<path d="M 41 13 H 51 V 21 H 41 Z M 54 13 H 63 V 21 H 53 V 14 H 54 Z"/>')


def arc(x0, y0, rx, ry, phi, large, sweep, x1, y1):
    """The cubics of an arc, by the end point to center conversion of
    implnote F.6.5 for a circle (rx = ry, no rotation, in range)."""
    assert rx == ry and phi == 0
    r = rx
    dx, dy = (x0 - x1) / 2, (y0 - y1) / 2
    root = math.sqrt(max(0.0, (r * r * r * r - r * r * dy * dy - r * r * dx * dx) / (r * r * dy * dy + r * r * dx * dx)))
    sign = -1 if large == sweep else 1
    cx, cy = sign * root * dy + (x0 + x1) / 2, sign * root * -dx + (y0 + y1) / 2
    start = math.atan2(y0 - cy, x0 - cx)
    turn = math.atan2(y1 - cy, x1 - cx) - start

    if sweep and turn < 0:
        turn += 2 * math.pi
    elif not sweep and turn > 0:
        turn -= 2 * math.pi

    pieces = math.ceil(abs(turn) / (math.pi / 2) - 1e-9)
    step = turn / pieces
    k = 4 / 3 * math.tan(step / 4)
    d = f"M {x0} {y0}"

    for i in range(pieces):
        a, b = start + i * step, start + (i + 1) * step
        p0 = (cx + r * math.cos(a), cy + r * math.sin(a))
        p3 = (cx + r * math.cos(b), cy + r * math.sin(b))
        p1 = (p0[0] - k * r * math.sin(a), p0[1] + k * r * math.cos(a))
        p2 = (p3[0] + k * r * math.sin(b), p3[1] - k * r * math.cos(b))
        d += " C " + " ".join("%.4f %.4f" % p for p in (p1, p2, p3))

    return d


for i, (large, sweep) in enumerate(((0, 0), (0, 1), (1, 0), (1, 1))):
    x = 4 + i * 15
    y = 40
    stroke(f"arc-flags-{large}{sweep}", (x - 3, 30, 15, 20), f"M {x} {y} A 5 5 0 {large} {sweep} {x + 8} {y}", arc(x, y, 5, 5, 0, large, sweep, x + 8, y))

unit.write()
