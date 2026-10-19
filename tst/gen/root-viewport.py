"""root-viewport: the initial viewport and coordinate system, of SVG 2. This
writes both drawings: the unit's root gives its width and height in user
units, the expected drawing's in px; the expected drawing is a picture of
the pixels, row by row from the top, so its y runs down by construction."""
import os

from unitgen import ROOT, comment, expect, png

NAME = "root-viewport"
W, H = 20, 12
WHITE, NAVY, LIME = (1, 1, 1, 1), (0, 0, .5, 1), (0, 1, 0, 1)

with open(os.path.join(ROOT, "tst", "unit", NAME + ".svg"), "w") as handle:
    handle.write("\n".join([
        f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}" height="{H}">',
        comment("""The outermost svg's width and height in user units are px: the
        drawing is 20 by 12 pixels, its expected one 20px by 12px. The user
        space is the viewport's, its origin at the top left, x right, y down,
        a unit a pixel: the navy square sits at the top left, the lime one
        right of it and lower."""),
        f'<rect width="{W}" height="{H}" fill="white"/>',
        '<rect x="2" y="2" width="4" height="4" fill="navy"/><rect x="8" y="5" width="10" height="5" fill="lime"/>',
        "</svg>"]) + "\n")


def pixel(x, y):
    if 2 <= x < 6 and 2 <= y < 6:
        return NAVY

    return LIME if 8 <= x < 18 and 5 <= y < 10 else WHITE


picture = png([[pixel(x, y) for x in range(W)] for y in range(H)])
expect(NAME, [
    ("user-units-px", (0, 0, W, H), ""),
    ("y-down", (0, 0, W, H), f'<image width="{W}" height="{H}" image-rendering="pixelated" href="{picture}"/>'),
], root=f'<svg xmlns="http://www.w3.org/2000/svg" width="{W}px" height="{H}px">')
