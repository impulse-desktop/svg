"""A tiny open stroke whose two opposing bends cancel their signed turn."""
import base64
import zlib

from unitgen import Unit, png


# Grayscale Chromium reference at 256x256, compressed. Resvg and librsvg
# agree on the large opening. This path is translated from a Sketchy-SVG
# white stroke; the shorter form keeps the original curve coordinates.
REFERENCE = "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"

gray = zlib.decompress(base64.b64decode(REFERENCE))
assert len(gray) == 256 * 256
rows = [[(value / 255, value / 255, value / 255, 1) for value in gray[y * 256:(y + 1) * 256]]
        for y in range(256)]

unit = Unit("stroke-folded-open", 320, 256, """A short open path can bend
one way and then back, so its net signed turn is small although its offset
folds inside out. The nonzero outline leaves a large opening. A wide stroke
must keep that opening when the endpoint tangents resume the same heading.
Another tight path bends around once, with its endpoint tangents across
one another; the normal sweep covers the cropped region within its very
first cubic, so no hole belongs there (D49).""")

path = "m16.5,16.5c-1.564,-1.52802 -1.88824,-1.19034 -2,-0.29999c-0.08652,0.68918 -0.70532,-0.17233 -1,-0.74167l-0.29468,-0.60934"
unit.cell("opposed-bends", (0, 0, 256, 256),
          f'<svg width="256" height="256" viewBox="0 0 35 35"><path d="{path}" fill="none" stroke="black" stroke-width="20"/></svg>',
          f'<image width="256" height="256" href="{png(rows)}"/>', tolerance=80)

# A real path whose large absolute turn contains small oscillations. Its
# first near-horizontal cubic covers the 2 by 6 crop by the ideal stroke's
# normals. Keeping every inverted offset would punch an erroneous hole.
wide_bend = "m130.5,187.89999c1.8,0 2.70006,-0.01067 3.60001,0c0.87935,0.01042 1.53835,0.44937 2.116,1.08401c0.58859,0.64665 1.02182,1.27789 1.30933,2.06532c0.3035,0.83124 0.78368,1.39378 0.9955,2.35066c0.18719,0.84563 0.56425,1.64972 1.14584,2.33334c0.56659,0.666 1.30733,1.06042 2.03333,1.45067c0.85478,0.45947 1.60001,0.71532 2.5,0.71599c0.90001,0.00067 1.75833,0 1.3,0c-0.8,0 -1.48848,0.28796 -2.2215,0.87868c-0.64632,0.52084 -1.43111,0.92151 -2.2785,1.10049c-0.8808,0.18605 -1.88933,0.03151 -2.5,0.52083c-0.61067,0.48933 -1.55197,0.21092 -2.38933,0.51068c-0.73674,0.26373 -1.25836,0.94772 -2.09334,0.99066c-0.95045,0.04887 -1.50179,0.84436 -2.31866,0.99733c-0.82895,0.15523 -1.64482,-0.54623 -2.11334,-1.284c-0.43912,-0.69147 -0.08533,-1.71468 -0.08533,-2.51466c0,-0.90001 0,-1.8 0,-2.70001c0,-0.89999 0,-1.79999 0,-2.7l0,-0.89999l0,-0.90001"
unit.cell("one-way-bend", (256, 0, 64, 192),
          f'<svg x="256" width="64" height="192" viewBox="131 185 2 6"><path d="{wide_bend}" fill="none" stroke="black" stroke-width="39.84"/></svg>',
          '<rect x="256" width="64" height="192" fill="black"/>')
unit.write()
