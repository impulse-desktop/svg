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


def decode(v):
    return v / 12.92 if v <= .04045 else ((v + .055) / 1.055) ** 2.4


def colored_coverage(name, x, y):
    """Exact rectangular areas, followed by source-over in the chosen space.

    A miter-joined rectangular stroke is the outer rectangle minus its
    inner rectangle. The two-layer cell uses two full rectangles, so no
    assumption about correlated subpixel coverage enters its expectation.
    """
    background = (24 / 255, 61 / 255, 103 / 255)
    source = (242 / 255, 154 / 255, 54 / 255)
    second = (43 / 255, 211 / 255, 173 / 255)
    linear = not name.endswith('srgb')

    def area(i, j, box):
        x0, y0, x1, y1 = box
        return max(0, min(i + 1, x1) - max(i, x0)) * max(0, min(j + 1, y1) - max(j, y0))

    def over(top, bottom, alpha):
        if linear:
            return tuple(encode(decode(s) * alpha + decode(d) * (1 - alpha))
                         for s, d in zip(top, bottom))
        return tuple(s * alpha + d * (1 - alpha) for s, d in zip(top, bottom))

    rows = []
    for j in range(10):
        row = []
        for i in range(12):
            if name == 'colored-two-layers':
                color = over(second, over(source, background, .375), .6)
            else:
                if 'stroke' in name:
                    coverage = (area(i, j, (1.5, .75, 7.5, 6.75))
                                - area(i, j, (3, 2.25, 6, 5.25)))
                else:
                    coverage = area(i, j, (1.25, 1.5, 8.75, 6.75))
                opacity = .6 * .375 if name == 'colored-opacity' else 1
                color = over(source, background, coverage * opacity)
            row.append((*color, 1))
        rows.append(row)
    return (name, (x, y, 12, 10),
            f'<image x="{x}" y="{y}" width="12" height="10" '
            f'image-rendering="pixelated" href="{png(rows)}"/>')


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
] + [colored_coverage(name, 2 + i % 3 * 20, 66 + i // 3 * 11)
     for i, name in enumerate(('colored-linear', 'colored-srgb', 'colored-opacity',
                               'colored-two-layers', 'colored-stroke-linear',
                               'colored-stroke-srgb'))])
