"""image-data-urls: data: URLs, their bodies percent-decoded before any
base64 is read."""
from unitgen import Unit, png

unit = Unit("image-data-urls", 58, 12, """Each lime square fills its black
outline; an empty outline holds nothing. A data: URL's body is
percent-decoded first, then read as base64 where it says so (the fetch
standard's data: URL processor, which Chromium and Firefox follow): a PNG
whose base64 writes some of its letters and its padding as %XY draws, as
an SVG written in %XY does. A % in base64 that is no %XY, at the end or
before what is no hex digit, is no base64, and the image draws nothing.""")

LIME = png([[(0, 1, 0, 1)] * 2] * 2)
HEAD, BODY = LIME.split(",", 1)
ENCODED = HEAD + "," + BODY.replace("A", "%41").replace("=", "%3D").replace("/", "%2f")
SVG = "data:image/svg+xml,%3Csvg%20xmlns%3D%22http%3A%2F%2Fwww.w3.org%2F2000%2Fsvg%22%20width%3D%222%22%20height%3D%222%22%3E%3Crect%20width%3D%222%22%20height%3D%222%22%20fill%3D%22lime%22%2F%3E%3C%2Fsvg%3E"
assert "%41" in ENCODED

CELLS = [
    ("base64-percent", ENCODED, True),
    ("svg-percent", SVG, True),
    ("base64-percent-not-hex", HEAD + "," + BODY[:8] + "%ZZ" + BODY[8:], False),
    ("base64-percent-half-hex", HEAD + "," + BODY[:8] + "%4Z" + BODY[8:], False),
    ("base64-percent-at-end", LIME + "%4", False),
]

for k, (name, href, draws) in enumerate(CELLS):
    x = 2 + k * 11
    outline = f'<rect x="{x}" y="2" width="8" height="8"/>'
    expected = f'<rect x="{x}" y="2" width="8" height="8" fill="lime"/>' if draws else ""
    unit.cell(name, (x - 1, 1, 10, 10), f'<image x="{x}" y="2" width="8" height="8" image-rendering="pixelated" href="{href}"/>', expected, outline)

unit.write()
