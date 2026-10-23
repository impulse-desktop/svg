"""Exactly representable affine inputs must retain small output geometry.

All large coordinates, matrix components and translations are binary32
values already. Products and cancellation have exact binary64 results.
Expected corners below are literal rational affine products, independent
of the renderer's transform or rasterizer implementation.
"""
from unitgen import Unit

unit = Unit(
    "transform-cancellation", 192, 128,
    "An affine transform can map large, exactly representable coordinates "
    "into a small visible shape. Compute the products and their sum before "
    "rounding the final point. Casting each product to single precision "
    "first loses several output pixels. Plain rectangles and a four-corner "
    "polygon give the exact expected geometry, including alpha at its edges.",
    ground=False,
)
cases = [
    ("positive", "39.5 0 0 39.5 -197500000 -197500000", "5000000.5", "5000000.5",
     '<rect x="19.75" y="19.75" width="19.75" height="19.75" fill="lime"/>'),
    ("reflected", "-39.5 0 0 39.5 197500064 -197500000", "5000000.5", "5000000.5",
     '<rect x="24.5" y="19.75" width="19.75" height="19.75" fill="lime"/>'),
    ("unequal-axes", "39.5 0 0 63 -197500000 -315000000", "5000000.5", "5000000.5",
     '<rect x="19.75" y="31.5" width="19.75" height="31.5" fill="lime"/>'),
    ("negative", "39.5 0 0 39.5 197500064 197500064", "-5000001", "-5000001",
     '<rect x="24.5" y="24.5" width="19.75" height="19.75" fill="lime"/>'),
    ("skew", "19.5 0 19.5 39.5 -195000000 -197500000", "5000000.5", "5000000.5",
     '<path d="M19.5 19.75H29.25L39 39.5H29.25Z" fill="lime"/>'),
    ("quarter-turn", "0 39.5 -39.5 0 197500064 -197500000", "5000000.5", "5000000.5",
     '<rect x="24.5" y="19.75" width="19.75" height="19.75" fill="lime"/>'),
]
for i, (name, matrix, x, y, expected) in enumerate(cases):
    left, top = i % 3 * 64, i // 3 * 64
    placement = f'transform="translate({left} {top})"'
    unit.cell(name, (left, top, 64, 64),
              f'<g {placement}><g transform="matrix({matrix})">'
              f'<rect x="{x}" y="{y}" width=".5" height=".5" fill="lime"/>'
              '</g></g>', f'<g {placement}>{expected}</g>')
unit.write()
