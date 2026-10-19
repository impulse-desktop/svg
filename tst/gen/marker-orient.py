"""marker-orient: marker orientation of SVG 2. The unit is written by hand;
this writes its expected drawing: the paths without their markers, then
each arrow where its orientation turns it. A turned arrow's edge on its
marker's clipped viewport takes both antialiasings, which its cells
allow."""
import os
import re

from unitgen import ROOT, outlined

text = open(os.path.join(ROOT, "tst", "unit", "marker-orient.svg")).read()
PATHS = re.sub(r'\s*marker-(start|mid|end)="[^"]*"', "", re.search(r'<g fill="none" stroke="silver" stroke-width="\.5">.*?</g>', text, re.S).group(0))
outlined("marker-orient", [
    ("paths", (0, 0, 0, 0), [], PATHS), ("auto-ends", (1, 3, 18, 18), [0, 1], "", 48), ("auto-start-reverse", (25, 3, 20, 6), [2, 3]), ("mid-bisect", (49, 9, 9, 6), [4]),
    ("angles", (3, 19, 28, 9), [5, 6, 7, 8]), ("negative", (39, 21, 6, 6), [9]), ("closed-start", (2, 31, 8, 8), [10], "", 48), ("curve-ends", (21, 31, 16, 10), [11, 12]), ("zero-segment", (51, 37, 6, 6), [13]),
])
