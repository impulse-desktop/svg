"""stroke-linejoin-shapes: line joins and the miter limit, of SVG 2. The unit
is written by hand; this writes its expected drawing: each stroke as the
shape its black edge, as the unit draws it, encloses."""
import os
import re

from unitgen import ROOT, expect

text = open(os.path.join(ROOT, "tst", "unit", "stroke-linejoin-shapes.svg")).read()
EDGES = re.search(r'<g fill="none" stroke="black" stroke-width="\.3">.*?</g>', text, re.S).group(0)
SHAPES = re.findall(r'<path d="([^"]*)"/>', EDGES)
NAMES = ["miter", "round", "bevel", "miter-under-limit", "bevel-over-limit", "miter-clip", "negative-limit", "limit-under-one",
         "reverse-miter", "reverse-round", "arcs-as-miter-clip", "reverse-miter-clip", "miter-clip-under-one"]
assert len(SHAPES) == len(NAMES)


def box(d):
    numbers = [float(v) for v in re.findall(r"-?[\d.]+", d)]
    xs, ys = numbers[0::2], numbers[1::2]
    # Arcs' radii and flags are numbers too, and lie inside the box.
    x0, y0 = int(min(xs)) - 1, int(min(ys)) - 1

    return (x0, y0, int(max(xs)) + 2 - x0, int(max(ys)) + 2 - y0)


cells = [(name, box(d), f'<path d="{d}" fill="lime"/>') for name, d in zip(NAMES, SHAPES)]
cells.append(("edges", (0, 0, 0, 0), EDGES))
expect("stroke-linejoin-shapes", cells)
