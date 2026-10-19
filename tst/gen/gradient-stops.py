"""gradient-stops: stop offsets, colours and counts, of SVG 2. The unit is
written by hand; this writes its expected drawing: each test strip as the
reference under it, and lime where the reference is lime."""
from unitgen import expect, pair

NAME = "gradient-stops"
expect(NAME, [
    ("offsets", (1, 1, 62, 10), pair(NAME, 2, 2, 6)), ("transparent", (1, 11, 62, 10), pair(NAME, 2, 12, 16)), ("opacity", (1, 21, 62, 10), pair(NAME, 2, 22, 26)),
    ("no-stops", (1, 31, 30, 20), '<rect x="2" y="32" width="28" height="18" fill="lime"/>'), ("one-stop", (33, 31, 30, 20), '<rect x="34" y="32" width="28" height="18" fill="lime"/>'),
])
