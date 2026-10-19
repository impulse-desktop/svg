"""transform-double: the CTM in double precision of SVG 2's conformance.
The unit is written by hand; this writes its expected drawing: each rect
where the transforms put it, computed exactly."""
from unitgen import expect

expect("transform-double", [
    ("viewbox-offset", (0, 0, 100, 10), '<rect x="25" y="0" width="50" height="10" fill="navy"/>'),
    ("css-transform", (0, 10, 100, 10), '<rect x="25" y="10" width="50" height="10" fill="green"/>'),
    ("nested-viewport", (0, 20, 30, 10), '<rect x="0.125" y="20" width="25" height="10" fill="maroon"/>'),
])
