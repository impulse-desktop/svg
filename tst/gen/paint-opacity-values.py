"""paint-opacity-values: fill-opacity and stroke-opacity of SVG 2. The unit
is written by hand; this writes its expected drawing."""
from unitgen import expect


def rect(x, y, fill, w=10):
    return f'<rect x="{x}" y="{y}" width="{w}" height="10" fill="{fill}"/>'


expect("paint-opacity-values", [
    ("fill-clamp", (1, 1, 24, 12), rect(2, 2, "lime") + rect(14, 2, "lime")),
    ("fill-values", (25, 1, 38, 12), rect(26, 2, "#808080") + rect(38, 2, "#bfbfbf") + rect(50, 2, "#808080")),
    ("stroke-clamp", (1, 17, 24, 12), rect(2, 18, "lime") + rect(14, 18, "lime")),
    ("stroke-values", (25, 17, 26, 12), rect(26, 18, "#808080") + rect(38, 18, "#bfbfbf")),
])
