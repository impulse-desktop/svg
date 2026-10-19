"""viewbox-align: the viewBox transform and preserveAspectRatio, of SVG 2.
The unit is written by hand; this writes its expected drawing: each
viewport's lime viewBox and navy quarter as the rectangles its black
outlines give, inside its silver bounds."""
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "viewbox-align.svg")).read()
group = re.search(r'<g fill="none" stroke="black" stroke-width="\.25">(.*?)</g>', text, re.S).group(1)
shapes = re.findall(r"<(rect|path)\b([^>]*)/>", group)
bounds = re.findall(r'<rect x="(\d+)" y="(\d+)" width="12" height="24" fill="none" stroke="silver" stroke-width="\.3"/>', text)
assert len(shapes) == 38 and len(bounds) == 18

ALIGNS = [f"{x}{y}" for y in ("YMin", "YMid", "YMax") for x in ("xMin", "xMid", "xMax")]
NAMES = [f"meet-{a}" for a in ALIGNS] + [f"slice-{a}" for a in ALIGNS] + ["none-commas"]
cells = []

for k, name in enumerate(NAMES):
    box = dict(re.findall(r'(\w+)="([^"]*)"', shapes[2 * k][1]))
    x, y, w, h = (float(box[key]) for key in ("x", "y", "width", "height"))
    move = [float(v) for v in re.findall(r"-?[\d.]+", dict(re.findall(r'(\w+)="([^"]*)"', shapes[2 * k + 1][1]))["d"])]
    # The quarter's outline runs h then v: from the box's corner down, or
    # for none from the middle of its left side up.
    qx, qy, qw, qh = move[0], move[1], move[2], move[3]

    if qh < 0:
        quarter = f'<rect x="{x:g}" y="{y:g}" width="{qw:g}" height="{-qh:g}" fill="navy"/>'
    else:
        quarter = f'<rect x="{qx:g}" y="{qy:g}" width="{qw:g}" height="{qh:g}" fill="navy"/>'
    silver = f'<rect x="{bounds[k][0]}" y="{bounds[k][1]}" width="12" height="24" fill="none" stroke="silver" stroke-width=".3"/>' if k < 18 else ""
    region = (int(bounds[k][0]) - 1, int(bounds[k][1]) - 1, 14, 26) if k < 18 else (1, 61, 42, 10)
    cells.append((name, region, f'<rect x="{x:g}" y="{y:g}" width="{w:g}" height="{h:g}" fill="lime"/>{quarter}{silver}'))

# With no viewBox there is nothing to fit: the content is as drawn.
cells.append(("aspect-without-viewbox", (45, 61, 22, 10), '<rect x="46" y="62" width="10" height="4" fill="lime"/><rect x="56" y="66" width="10" height="4" fill="navy"/>'))
# preserveAspectRatio's default, xMidYMid meet: the viewBox scaled by .8
# to 16 by 8, centred across.
cells.append(("default", (69, 61, 22, 10), '<rect x="72" y="62" width="16" height="8" fill="lime"/><rect x="72" y="62" width="8" height="4" fill="navy"/>'))
expect("viewbox-align", cells)
