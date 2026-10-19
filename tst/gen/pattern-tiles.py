"""pattern-tiles: a pattern's tiles, units and content, of SVG 2. The unit
is written by hand; this writes its expected drawing: each pattern fill as
the squares drawn out beside it, and lime where a pattern of no size leaves
its fallback or what is under it."""
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "pattern-tiles.svg")).read()
groups = re.findall(r'<g fill="lime" transform="translate\(32 0\)">(.*?)</g>', text)
assert len(groups) == 8


def drawn(i):
    """The squares beside the i-th fill, where they stand and where the
    fill is."""
    return f'<g fill="lime">{groups[i]}</g><g fill="lime" transform="translate(32 0)">{groups[i]}</g>'


names = ["box-fractions", "user-space", "content-box-units", "view-box", "transform", "overflow-hidden", "template", "css-transform"]
cells = [(name, (1, 1 + 14 * i, 62, 14), drawn(i)) for i, name in enumerate(names)]
cells.append(("zero-size", (1, 113, 62, 14), '<rect x="2" y="114" width="28" height="12" fill="lime"/><rect x="34" y="114" width="13" height="12" fill="lime"/><rect x="49" y="114" width="13" height="12" fill="lime"/>'))
expect("pattern-tiles", cells)
