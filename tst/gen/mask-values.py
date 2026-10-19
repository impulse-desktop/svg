"""mask-values: mask values and the mask element of CSS Masking. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect


def rect(x, y, w, h, fill="lime"):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}"/>'


expect("mask-values", [
    ("luminance", (1, 1, 30, 10), rect(2, 2, 8, 8, "#49ff49") + rect(12, 2, 8, 8, "#7fff7f") + rect(22, 2, 8, 8, "#c8ffc8")),
    ("mask-type", (31, 1, 10, 10), rect(32, 2, 8, 8)),
    ("mode-override", (41, 1, 22, 10), rect(52, 2, 8, 8)),
    ("region", (0, 12, 25, 14), rect(0, 13, 24, 12)),
    ("zero-width", (25, 13, 12, 12), ""),
    ("content-units", (37, 13, 14, 12), rect(38, 14, 6, 10)),
    ("missing", (1, 27, 12, 12), rect(2, 28, 10, 10)),
    ("not-a-mask", (13, 27, 12, 12), ""),
    ("zero-height-box", (25, 27, 16, 12), rect(26, 30, 14, 6)),
    ("stacking-context", (41, 27, 12, 12), rect(42, 28, 10, 10, "blue") + rect(44, 30, 6, 6, "yellow")),
    ("border-alpha", (53, 27, 10, 10), rect(54, 28, 8, 8)),
    ("single-channel", (1, 41, 18, 10), rect(2, 42, 7, 8, "#7fff7f") + rect(11, 42, 7, 8)),
    ("three-channel", (19, 41, 18, 10), rect(29, 42, 7, 8)),
    ("inherit", (37, 41, 9, 10), rect(38, 42, 7, 8)),
    ("not-rendered", (46, 41, 9, 10), rect(47, 42, 7, 8)),
    ("negative-width", (55, 41, 9, 10), ""),
])
