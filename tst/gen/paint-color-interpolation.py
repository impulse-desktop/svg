"""paint-color-interpolation: color-interpolation for gradients and
compositing, of SVG 2. The unit is written by hand; this writes its expected
drawing: a linearRGB gradient and a circle composited in linear light as
pictures of the formula at the pixel centres, the rest as the references
under them."""
from unitgen import expect, pair, png

NAME = "paint-color-interpolation"


def encode(v):
    """sRGB's transfer function, from linear light."""
    return 12.92 * v if v <= 0.0031308 else 1.055 * v ** (1 / 2.4) - 0.055


def linear_strip(y):
    """Black to white across 2 to 62, interpolated in linear light."""
    row = [(encode(t),) * 3 + (1,) for t in ((i + .5) / 60 for i in range(60))]

    return f'<image x="2" y="{y}" width="60" height="4" image-rendering="pixelated" href="{png([row] * 4)}"/>'


def circle(cx, cy, r, x, y, size, samples=64):
    """Half white over black in linear light, at the coverage of a circle,
    each pixel's found by samples squared points."""
    rows = []

    for j in range(size):
        row = []

        for i in range(size):
            inside = sum((x + i + (a + .5) / samples - cx) ** 2 + (y + j + (b + .5) / samples - cy) ** 2 <= r * r for a in range(samples) for b in range(samples))
            row.append((encode(.5 * inside / samples ** 2),) * 3 + (1,))

        rows.append(row)

    return f'<image x="{x}" y="{y}" width="{size}" height="{size}" image-rendering="pixelated" href="{png(rows)}"/>'


REF = '<rect x="2" y="{}" width="60" height="4" fill="url(#ref)"/>'


def black(y, h):
    return f'<rect x="2" y="{y}" width="60" height="{h}" fill="black"/>'


expect(NAME, [
    ("gradient-linear", (1, 1, 62, 10), linear_strip(2) + REF.format(6)),
    ("href-own-interpolation", (1, 11, 62, 10), pair(NAME, 2, 12, 16)),
    ("href-referencing-linear", (1, 21, 62, 10), linear_strip(22) + REF.format(26)),
    ("composite-own", (3, 32, 14, 6), black(32, 6) + pair(NAME, 4, 33, 35)),
    ("composite-child-srgb", (19, 32, 14, 6), pair(NAME, 20, 33, 35)),
    ("composite-inherited", (35, 32, 14, 6), pair(NAME, 36, 33, 35)),
    ("image", (3, 40, 14, 6), black(40, 6) + pair(NAME, 4, 41, 43)),
    ("blend", (19, 40, 14, 6), pair(NAME, 20, 41, 43)),
    ("opacity-group", (6, 48, 15, 8), black(48, 8) + circle(10, 52, 3, 6, 48, 8) + '<rect x="16" y="50" width="4" height="4" fill="#bcbcbc"/>'),
    ("blend-screen", (25, 49, 22, 6), '<rect x="26" y="50" width="20" height="4" fill="white"/>'),
    ("clear-paint", (49, 49, 12, 6), '<rect x="55" y="50" width="5" height="4" fill="white"/>'),
    ("image-inner-opacity", (3, 58, 14, 6), black(58, 6) + pair(NAME, 4, 59, 61)),
])
