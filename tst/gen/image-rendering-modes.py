"""image-rendering-modes: the image-rendering keywords of CSS Images 3, each
image drawn over the image the expected drawing draws in its place,
resampled here as the keyword asks."""
import math

from unitgen import Unit, png, rows

SOURCE = [[(1, 0, 0, 1), (0, 1, 0, 1), (0, 0, 1, 1), (1, 1, 0, 1)], [(0, 1, 1, 1), (1, 0, 1, 1), (1, 1, 1, 1), (0, 0, 0, 1)], [(1, .5, 0, 1), (0, .5, 1, 1), (.5, 0, 1, 1), (.5, 1, 0, 1)]]
NW, NH = 4, 3
IMAGE = png(SOURCE)
unit = Unit("image-rendering-modes", 74, 0, """Each test runs on into the
reference under it without a step. image-rendering pixelated scales an
image by nearest neighbour to the whole multiple of its size nearest the
drawn one, each axis apart, and smooths the rest of the way, here 2.25 by
2.33 from 2, 1.75 by 2.67 from 2 by 3, whole multiples, which are the
nearest neighbour, and 2 by 2.33, whole along x alone; crisp-edges is the
nearest neighbour; optimizeSpeed draws as crisp-edges, optimizeQuality as
smooth; auto, smooth and high-quality smooth alike; the property is
inherited. A reference is drawn at its own size, pixelated, which is
exact; the smoothing past pixelated's whole multiple is the renderer's
own, so its cell allows it some leeway.""")


def pixelated(dw, dh, whole=True):
    """Nearest neighbour to the nearest whole multiple, then bilinear; with
    no whole multiple, bilinear alone, which is smooth."""
    sx, sy = dw / NW, dh / NH
    kx, ky = (max(1, round(sx)), max(1, round(sy))) if whole else (1, 1)
    out = []

    for j in range(dh):
        row = []

        for i in range(dw):
            qx, qy = (i + .5) / sx * kx - .5, (j + .5) / sy * ky - .5
            x0, y0 = math.floor(qx), math.floor(qy)
            tx, ty = qx - x0, qy - y0
            c = [0] * 4

            for ix, iy, w in [(x0, y0, (1 - tx) * (1 - ty)), (x0 + 1, y0, tx * (1 - ty)), (x0, y0 + 1, (1 - tx) * ty), (x0 + 1, y0 + 1, tx * ty)]:
                v = SOURCE[min(NH - 1, max(0, math.floor(iy / ky)))][min(NW - 1, max(0, math.floor(ix / kx)))]
                c = [a + b * w for a, b in zip(c, v)]

            row.append(c)

        out.append(row)

    return out


def nearest(dw, dh):
    return [[SOURCE[min(NH - 1, math.floor((j + .5) * NH / dh))][min(NW - 1, math.floor((i + .5) * NW / dw))] for i in range(dw)] for j in range(dh)]


def item(x, y, value):
    """A test over its reference: a PNG resampled as the keyword asks drawn
    at its own size, exactly, or the image drawn smooth."""
    dw, dh, test, reference = value
    image = '<image x="{0}" y="{1}" width="%d" height="%d" preserveAspectRatio="none" {2}href="{3}"/>' % (dw, dh)
    exact = 'image-rendering="pixelated" ' if reference else 'image-rendering="smooth" '
    tested = test + image.format(x, y, "", IMAGE) + "</g>" if test.startswith("<g") else image.format(x, y, test + " ", IMAGE)
    plain = image.format(x, y + dh, exact, reference or IMAGE)

    return tested + plain, image.format(x, y, exact, reference or IMAGE) + plain, ""


y = rows(unit, 2, [
    ("pixelated", [(9, 7, 'image-rendering="pixelated"', png(pixelated(9, 7))), (7, 8, 'image-rendering="pixelated"', png(pixelated(7, 8))), (12, 6, 'image-rendering="pixelated"', png(pixelated(12, 6))),
                   (8, 7, 'style="image-rendering: pixelated"', png(pixelated(8, 7)))]),
    ("crisp-edges", [(10, 5, 'image-rendering="crisp-edges"', png(nearest(10, 5))), (7, 8, 'style="image-rendering: crisp-edges"', png(nearest(7, 8)))]),
    ("legacy", [(10, 5, 'image-rendering="optimizeSpeed"', png(nearest(10, 5))), (10, 5, 'image-rendering="optimizeQuality"', None)]),
    ("smooth", [(10, 5, 'image-rendering="auto"', None), (10, 5, 'image-rendering="high-quality"', None), (10, 5, 'style="image-rendering: smooth"', None)]),
    ("inherited", [(10, 5, '<g image-rendering="crisp-edges">', png(nearest(10, 5))), (7, 8, '<g style="image-rendering: crisp-edges">', png(nearest(7, 8)))]),
], 5, 14, 18, item, tolerances={"pixelated": 32})

# Shrinking and unequal axis scales still preserve the source palette for
# crisp-edges and its optimizeSpeed alias. Expected samples use pixel
# centers in the untransformed source, independently of the renderer.
y = rows(unit, y + 2, [
    ("crisp-down", [(3, 2, 'image-rendering="crisp-edges"', png(nearest(3, 2))),
                    (7, 2, 'image-rendering="crisp-edges"', png(nearest(7, 2))),
                    (2, 5, 'style="image-rendering: crisp-edges"', png(nearest(2, 5)))]),
    ("legacy-down", [(3, 2, 'image-rendering="optimizeSpeed"', png(nearest(3, 2))),
                     (7, 2, 'style="image-rendering: optimizeSpeed"', png(nearest(7, 2))),
                     (2, 5, 'image-rendering="optimizeSpeed"', png(nearest(2, 5)))]),
], 5, 14, 18, item)
test, expected = [], []
for x, mode in [(2, "crisp-edges"), (16, "optimizeSpeed")]:
    test.append(f'<g transform="translate({x} {y + 2})">'
                '<image width="4" height="3" transform="scale(.75 .6666666666666667)" '
                f'image-rendering="{mode}" href="{IMAGE}"/></g>')
    expected.append(f'<image x="{x}" y="{y + 2}" width="3" height="2" '
                    f'image-rendering="pixelated" href="{png(nearest(3, 2))}"/>')
unit.cell("transform-down", (2, y + 1, 28, 10), ''.join(test), ''.join(expected))
unit.height = y + 12
unit.write()
