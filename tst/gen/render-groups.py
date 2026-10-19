"""render-groups: groups, isolation and opacity of SVG 2's rendering model.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair


def rect(x, y, w, h, fill):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}"/>'


G, D = "#808080", "#008000"
expect("render-groups", [
    ("group-opacity", (1, 1, 14, 12), pair("render-groups", 2, 2, 7)),
    ("non-isolated", (17, 1, 24, 12), rect(18, 2, 22, 5, "lime") + rect(18, 2, 10, 5, D) + rect(30, 2, 10, 5, D) + rect(18, 7, 22, 5, D)),
    ("isolated", (41, 1, 22, 12), rect(40, 2, 2, 5, "lime") + rect(42, 2, 20, 5, G) + rect(40, 7, 22, 5, G)),
    ("marker-isolated", (1, 15, 22, 12), rect(2, 16, 20, 5, "lime") + rect(2, 16, 10, 5, G) + rect(2, 21, 10, 5, G)),
])
