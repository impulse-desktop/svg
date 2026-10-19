"""path-length-edges: pathLength's edges, of SVG 2. The unit is written by
hand; this writes its expected drawing: each dashed line as the bars it
runs on into."""
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "path-length-edges.svg")).read()
bars = re.findall(r'<rect x="([\d.]+)" y="(\d+)" width="([\d.]+)" height="2"/>', text)


def row(y):
    return "".join(f'<rect x="{x}" y="{int(by) - 2}" width="{w}" height="4" fill="black"/>' for x, by, w in bars if int(by) == y)


expect("path-length-edges", [("zero", (1, 0, 62, 6), row(3)), ("negative", (1, 6, 62, 6), row(9)), ("moveto", (1, 12, 62, 6), row(15)), ("percent", (1, 18, 62, 6), row(21))])
