"""image-placement: image sizing and preserveAspectRatio of SVG 2. The unit
is written by hand; this writes its expected drawing."""
from unitgen import outlined


def rect(x, y, w, h, fill):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}"/>'


outlined("image-placement", [
    ("natural", (1, 1, 10, 6), [0]), ("one-dimension", (13, 1, 46, 14), [1, 2]), ("ratio-width", (1, 17, 18, 10), [3]), ("default-size", (21, 17, 12, 12), [4]),
    ("zero", (35, 17, 10, 10), []), ("channels", (47, 17, 14, 12), [], rect(48, 18, 12, 4, "#808080") + rect(48, 24, 12, 4, "lime")), ("meet", (1, 31, 10, 10), [7]),
    ("not-inherited", (13, 31, 10, 10), [], rect(14, 32, 8, 8, "black")),
    ("svg-min-meet", (1, 49, 18, 10), [], rect(2, 50, 8, 8, "lime")),
    # (13.2 - 8) / 2 = 2.6: the image's default xMidYMid owns the fit,
    # including when the intrinsic root asks for another alignment/slice.
    ("svg-default-mid-min-root", (21, 49, 16, 10), [], rect(24.6, 50, 8, 8, "lime")),
    ("svg-default-mid-max-root", (39, 49, 16, 10), [], rect(42.6, 50, 8, 8, "lime")),
])
