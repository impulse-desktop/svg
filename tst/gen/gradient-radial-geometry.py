"""gradient-radial-geometry: radial gradient circles of SVG 2. The unit is
written by hand; this writes its expected drawing: each square its
reference whole."""
from unitgen import expect


def square(x, y, w, h, fill):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="url(#{fill})"/>'


expect("gradient-radial-geometry", [
    ("defaults", (1, 1, 22, 22), square(2, 2, 20, 20, "dr")),
    ("focus-template", (23, 1, 18, 22), square(24, 2, 16, 20, "tbr")),
    ("focus-own", (43, 1, 18, 22), square(44, 2, 16, 20, "tdr")),
    ("fr", (1, 25, 22, 22), square(2, 26, 20, 20, "fref")),
    ("percent-r", (29, 41, 30, 22), square(30, 42, 28, 20, "pr")),
    ("negative-r", (1, 49, 12, 14), '<rect x="2" y="50" width="10" height="12" fill="lime"/>'),
    ("negative-fr", (13, 49, 12, 14), '<rect x="14" y="50" width="10" height="12" fill="lime"/>'),
])
