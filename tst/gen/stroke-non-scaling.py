"""stroke-non-scaling: vector-effect non-scaling-stroke, of SVG 2. The unit
is written by hand; this writes its expected drawing: each stroke as the
lime its black outline encloses."""
from unitgen import outlined

outlined("stroke-non-scaling", [
    ("scale", (2, 2, 12, 12), [0]), ("skew", (17, 2, 18, 12), [1]), ("nested-viewbox", (38, 2, 24, 12), [2]),
    ("path-half", (3, 18, 58, 4), [3]), ("not-inherited", (1, 24, 26, 12), [4]),
], paint=' fill="lime" fill-rule="evenodd"')
