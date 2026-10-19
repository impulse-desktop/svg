"""fe-blur-values: feGaussianBlur's stdDeviation of CSS Filter Effects. The
unit is written by hand; this writes its expected drawing, the blurred
ones as images of a true Gaussian, which the module lets a renderer
approximate by three box blurs, so their cells allow some leeway."""
import math

from unitgen import expect, png

# The rect at 4 by 12 in a region 8 by 18 from 2 and 3 back.
X0, Y0, W, H, RW, RH = 2, 3, 4, 12, 8, 18


def gaussian(sx, sy):
    def share(c, lo, hi, s):
        if s == 0:
            return 1.0 if lo <= c < hi else 0.0

        return (math.erf((hi - c) / (s * math.sqrt(2))) - math.erf((lo - c) / (s * math.sqrt(2)))) / 2

    rows = []

    for j in range(RH):
        row = []

        for i in range(RW):
            a = share(i + .5, X0, X0 + W, sx) * share(j + .5, Y0, Y0 + H, sy)
            row.append((0, 1, 0, a))

        rows.append(row)

    return png(rows)


def image(x, sx, sy):
    return f'<image x="{x - 2}" y="3" width="{RW}" height="{RH}" image-rendering="pixelated" href="{gaussian(sx, sy)}"/>'


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("fe-blur-values", [
    ("one-deviation", (2, 2, 10, 20), image(5, 2, 2), 16),
    ("x-then-y", (15, 2, 10, 20), image(18, 3, 0), 16),
    ("zero-axis", (28, 2, 10, 20), image(31, 0, 3), 16),
    ("pass-through", (41, 2, 36, 20), lime(44, 6, 4, 12) + lime(57, 6, 4, 12) + lime(70, 6, 4, 12)),
    ("fill-paint", (80, 2, 10, 20), lime(81, 3, 8, 18)),
])
