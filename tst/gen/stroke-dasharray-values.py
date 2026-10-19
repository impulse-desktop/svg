"""stroke-dasharray-values: stroke-dasharray and stroke-dashoffset of SVG 2.
The unit is written by hand; this writes its expected drawing: each
dashed line as the bars it runs on into."""
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "stroke-dasharray-values.svg")).read()
bars = re.findall(r'<rect x="([\d.]+)" y="(\d+)" width="([\d.]+)" height="2"/>', text)


def row(*ys):
    return "".join(f'<rect x="{x}" y="{int(by) - 2}" width="{w}" height="4" fill="black"/>' for x, by, w in bars if int(by) in ys)


expect("stroke-dasharray-values", [
    ("odd-list", (1, 1, 62, 5), row(4)), ("commas", (1, 6, 62, 5), row(9)), ("percent", (1, 11, 62, 5), row(14)), ("offset", (1, 16, 62, 5), row(19)),
    ("negative-offset", (1, 21, 62, 10), row(24, 29)), ("percent-offset", (1, 31, 62, 5), row(34)), ("path-length", (1, 36, 62, 5), row(39)),
    ("negative-value", (1, 41, 62, 5), row(44)), ("zero-sum", (1, 46, 62, 5), row(49)),
])
