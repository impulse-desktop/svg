"""Invalid convolution orders use 3 by 3; valid fractions truncate first."""
from unitgen import Unit


unit = Unit("fe-convolve-order", 144, 96, """An invalid order takes the initial
3 by 3 kernel. A center-only kernel doubles an opaque source's sRGB channels;
the expected drawing is a plain rectangle. Valid fractional orders truncate
toward zero. Large positive kernels stay outside the implementation's limit,
and must be checked before converting their order to an integer.""")

cases = [
    ("missing", None, 3, True),
    ("negative-x", "-3", 3, True),
    ("negative-y", "3 -1", 3, True),
    ("zero", "0", 3, True),
    ("zero-y", "3 0", 3, True),
    ("fraction-below-one", "0.9", 3, True),
    ("tiny-positive", "1e-30", 3, True),
    ("empty", "", 3, True),
    ("malformed", "bad", 3, True),
    ("trailing-token", "3 3 1", 3, True),
    ("length", "3px", 3, True),
    ("huge-negative", "-1e30", 3, True),
    ("negative-pair", "1 -1e30", 3, True),
    ("one", "1", 1, True),
    ("fraction", "1.9", 1, True),
    ("fraction-pair", "1.2 1.8", 1, True),
    ("fraction-three", "3.9", 3, True),
    ("oversized", "33", 3, False),
    ("huge-positive-x", "1e30 3", 3, False),
    ("huge-positive-y", "3 1e30", 3, False),
    ("maximum-float", "3.4e38", 3, False),
]

for i, (name, order, size, drawn) in enumerate(cases):
    x, y = i % 6 * 24 + 4, i // 6 * 24 + 4
    matrix = ["0"] * (size * size)
    matrix[len(matrix) // 2] = "2"
    attribute = f' order="{order}"' if order is not None else ""
    unit.cell(name, (x - 1, y - 1, 18, 18),
              f'<filter id="f{i}" x="0" y="0" width="1" height="1" color-interpolation-filters="sRGB">'
              f'<feConvolveMatrix{attribute} kernelMatrix="{" ".join(matrix)}" divisor="1"/></filter>'
              f'<rect x="{x}" y="{y}" width="16" height="16" fill="rgb(32,64,96)" filter="url(#f{i})"/>',
              f'<rect x="{x}" y="{y}" width="16" height="16" fill="rgb(64,128,192)"/>' if drawn else "")

unit.write()
