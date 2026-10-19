"""fill-open-subpath: open subpaths fill as if closed, of SVG 2. The unit is
written by hand; this writes its expected drawing: the subpaths closed."""
from unitgen import expect

expect("fill-open-subpath", [
    ("polyline", (1, 1, 20, 20), '<path d="M 2 20 L 11 2 L 20 20 Z" fill="lime"/>'),
    ("path", (22, 1, 41, 21), '<path d="M 23 20 L 32 2 L 41 20 Z M 45 3 H 61 V 21 Z" fill="lime"/>'),
])
