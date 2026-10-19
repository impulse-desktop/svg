"""paint-fallback: paint server references and their fallbacks, of SVG 2.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(*xs, y=2):
    return "".join(f'<rect x="{x}" y="{y}" width="10" height="10" fill="lime"/>' for x in xs)


expect("paint-fallback", [
    ("resolves", (1, 1, 12, 12), lime(2)), ("missing-fallback", (13, 1, 12, 12), lime(14)), ("not-server", (25, 1, 24, 12), lime(26, 38)), ("current-color", (49, 1, 12, 12), lime(50)),
    ("no-fallback", (1, 15, 62, 12), lime(2, 14, 26, 38, 50, y=16)),
])
