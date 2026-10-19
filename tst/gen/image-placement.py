"""image-placement: image sizing and preserveAspectRatio of SVG 2. The unit
is written by hand; this writes its expected drawing."""
from unitgen import outlined


def rect(x, y, w, h, fill):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}"/>'


outlined("image-placement", [
    ("natural", (1, 1, 10, 6), [0]), ("one-dimension", (13, 1, 46, 14), [1, 2]), ("ratio-width", (1, 17, 18, 10), [3]), ("default-size", (21, 17, 12, 12), [4]),
    ("zero", (35, 17, 10, 10), []), ("channels", (47, 17, 14, 12), [], rect(48, 18, 12, 4, "#808080") + rect(48, 24, 12, 4, "lime")), ("meet", (1, 31, 10, 10), [7]),
    ("not-inherited", (13, 31, 10, 10), [], rect(14, 32, 8, 8, "black")),
])
