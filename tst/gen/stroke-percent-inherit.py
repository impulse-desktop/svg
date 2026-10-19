"""stroke-percent-inherit: stroke percentages inherited into another
viewport."""
from unitgen import Unit

unit = Unit("stroke-percent-inherit", 170, 24, """Each black line fills its
outline. A percentage in stroke-width, stroke-dasharray or
stroke-dashoffset stays one in the computed value and inherits so: each
viewport, 20 px square, holds the viewBox 0 0 10 10, whose normalized
diagonal is 10, and a line from 1 to 9 across its middle draws its stroke
as the percentage of that, though the svg element or group giving it lies
in the unit's own viewport. In a calc() the em is the giving element's;
inherit takes the percentage too.""")

CELLS = [
    # name, attributes of the inner svg, of a group around it, of the path, the stroked spans and width in viewBox units
    ("nested-width", 'stroke-width="10%"', "", "", [(1, 9)], 1),
    ("group-width", "", 'stroke-width="20%"', "", [(1, 9)], 2),
    ("calc-width", 'stroke-width="calc(10% + 0.5px)"', "", "", [(1, 9)], 1.5),
    ("calc-em", 'font-size="2px" stroke-width="calc(10% + 0.5em)"', "", 'font-size="8px"', [(1, 9)], 2),
    ("inherit-keyword", 'stroke-width="10%"', "", 'stroke-width="inherit"', [(1, 9)], 1),
    ("dasharray", 'stroke-dasharray="20%"', "", 'stroke-width="2"', [(1, 3), (5, 7)], 2),
    ("dashoffset", 'stroke-dasharray="2" stroke-dashoffset="10%"', "", 'stroke-width="2"', [(1, 2), (4, 6), (8, 9)], 2),
]

for k, (name, inner, group, path, spans, width) in enumerate(CELLS):
    x, y = 2 + k * 24, 2
    svg = f'<svg x="{x}" y="{y}" width="20" height="20" viewBox="0 0 10 10" {inner}><path d="M 1 5 H 9" {path}/></svg>'
    test = f'<g fill="none" stroke="black" {group}>{svg}</g>' if group else f'<g fill="none" stroke="black">{svg}</g>'
    rects = "".join(f'<rect x="{x + 2 * a:g}" y="{y + 10 - width:g}" width="{2 * (b - a):g}" height="{2 * width:g}"/>' for a, b in spans)
    unit.cell(name, (x - 1, y - 1, 22, 22), test, rects, rects)

unit.write()
