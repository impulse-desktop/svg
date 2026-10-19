"""gradient-radial-focal: a radial gradient's start circle outside, over or
on its end circle, of SVG 2. The unit is written by hand; this writes its
expected drawing: each gradient as a picture of the spec's formula at the
pixel centres, the circles c(w) = f + w (c - f) of radius fr + w (r - fr),
w the largest whose radius is not negative, none where no circle passes;
past a focal point on the end circle, the weighted average of the colours
with repeat, the last stop with pad (RadialGradientNotes)."""
import math

from unitgen import expect, png

NAVY, GOLD = (0, 0, 128 / 255), (1, 215 / 255, 0)
BLACK, WHITE = (0, 0, 0), (1, 1, 1)


def mix(a, b, t):
    return tuple(x + (y - x) * t for x, y in zip(a, b))


def radial(px, py, cx, cy, r, fx, fy, fr=0.0):
    """The w of a point, or None."""
    cdx, cdy, dr = cx - fx, cy - fy, r - fr
    pdx, pdy = px - fx, py - fy
    a = cdx * cdx + cdy * cdy - dr * dr
    b = pdx * cdx + pdy * cdy + fr * dr
    c = pdx * pdx + pdy * pdy - fr * fr

    if abs(a) < 1e-9:
        roots = [c / (2 * b)] if b != 0 else []
    else:
        disc = b * b - a * c
        roots = [(b + math.sqrt(disc)) / a, (b - math.sqrt(disc)) / a] if disc >= 0 else []

    roots = [w for w in roots if fr + w * dr >= 0]

    return max(roots) if roots else None


def picture(x, y, w, h, colour):
    """The pixels of x, y, w, h, colour(px, py) at each centre, None for
    clear."""
    rows = [[colour(x + i + .5, y + j + .5) for i in range(w)] for j in range(h)]
    rows = [[(0, 0, 0, 0) if c is None else c + (1,) for c in row] for row in rows]

    return f'<image x="{x}" y="{y}" width="{w}" height="{h}" image-rendering="pixelated" href="{png(rows)}"/>'


def pad(a, b, beyond=None):
    def colour(t):
        return beyond if t is None else mix(a, b, min(1, max(0, t)))

    return colour


def repeat(a, b, beyond):
    def colour(t):
        return beyond if t is None else mix(a, b, t - math.floor(t))

    return colour


cone = pad(NAVY, GOLD)
edge = repeat(BLACK, WHITE, (128 / 255,) * 3)
edgepad = pad(NAVY, GOLD, GOLD)
LINES = '<path d="M -14.991 -10.000 L 40.000 14.000 L -14.991 38.000" fill="none" stroke="black" stroke-width=".3"/>'

expect("gradient-radial-focal", [
    ("cone", (2, 2, 60, 24), picture(2, 2, 60, 24, lambda px, py: cone(radial(px, py, 20, 14, 8, 40, 14))) + LINES),
    ("start-over-end", (2, 30, 20, 20), '<rect x="2" y="30" width="20" height="20" fill="lime"/>'),
    ("focal-on-circle-repeat", (26, 30, 36, 10), picture(26, 30, 36, 10, lambda px, py: edge(radial(px, py, 40, 40, 8, 48, 40)))),
    ("weighted-average", (49, 40, 13, 5), '<rect x="49" y="40" width="13" height="5" fill="#808080"/>'),
    ("focal-on-circle-pad", (26, 45, 36, 10), picture(26, 45, 36, 10, lambda px, py: edgepad(radial(px, py, 40, 40, 8, 48, 40)))),
    ("tangent-centres", (2, 51, 20, 4), picture(2, 51, 20, 4, lambda px, py: edgepad(radial(px, py, 10.5, 53.5, 8, 18.5, 53.5)))),
])
