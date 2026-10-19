"""paint-order-values: paint-order of SVG 2. The unit is written by hand;
this writes its expected drawing: each test drawn as its layers, as the
reference beside it is."""
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "paint-order-values.svg")).read()
lines = text.split("\n")
start = next(i for i, l in enumerate(lines) if l.startswith('<g stroke-width="4">'))
references = lines[start + 1:start + 7]


def both(reference):
    return f'<g stroke-width="4">{reference}<g transform="translate(-16 0)">{reference}</g></g>'


NAMES = ["normal", "stroke-first", "markers-first", "markers-stroke", "invalid-inherits", "invalid-value"]
expect("paint-order-values", [(name, (32 * (k % 2), 14 * (k // 2), 32, 14 if k < 4 else 12), both(references[k])) for k, name in enumerate(NAMES)])
