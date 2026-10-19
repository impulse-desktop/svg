"""render-display-visibility: visibility of SVG 2's rendering model. The
unit is written by hand; this writes its expected drawing."""
from unitgen import expect

expect("render-display-visibility", [("visibility", (1, 1, 24, 12), '<rect x="2" y="2" width="10" height="10" fill="lime"/>'), ("hidden-bbox", (1, 13, 62, 10), '<rect x="32" y="14" width="30" height="8" fill="lime"/>')])
