"""An arc with equal endpoints is omitted, unlike a zero-length line."""
from unitgen import Unit

u = Unit("path-arc-same-end", 64, 48,
         "SVG 2 ArcOutOfRangeParameters omits an arc whose endpoint equals its start, including zero radii. A real zero-length line keeps its round cap. Zero radii with distinct endpoints still give a line. Relative commands continue from the unchanged current point.")
cases = [
    ("arc-only", "M8 8A8 8 0 0 1 8 8", 6, "round", ""),
    ("zero-line", "M24 8L24 8", 6, "round", '<circle cx="24" cy="8" r="3" fill="navy"/>'),
    ("zero-rx-same", "M40 8A0 8 0 0 1 40 8", 6, "round", ""),
    ("zero-ry-same", "M56 8A8 0 0 1 0 56 8", 6, "round", ""),
    ("zero-rx-line", "M4 24A0 8 0 0 1 12 24", 4, "butt", '<rect x="4" y="22" width="8" height="4" fill="navy"/>'),
    ("end-of-line", "M20 24H28A8 8 0 0 1 28 24", 4, "round", '<path d="M20 24H28" fill="none" stroke="navy" stroke-width="4" stroke-linecap="round"/>'),
    ("relative-after-arc", "M36 24a8 8 0 0 1 0 0l8 0", 4, "butt", '<rect x="36" y="22" width="8" height="4" fill="navy"/>'),
    ("zero-ry-line", "M52 24A8 0 0 0 1 60 24", 4, "butt", '<rect x="52" y="22" width="8" height="4" fill="navy"/>'),
    ("repeated-same", "M8 40A8 8 0 0 1 8 40A8 8 0 0 1 8 40", 6, "round", ""),
    ("repeated-zero-rx", "M24 40A0 8 0 0 1 24 40A0 8 0 0 1 24 40", 6, "round", ""),
    ("close-after-arc", "M40 40A8 8 0 0 1 40 40Z", 6, "round", '<circle cx="40" cy="40" r="3" fill="navy"/>'),
    ("line-after-repeated", "M52 40A8 8 0 0 1 52 40a8 8 0 0 1 0 0h8", 4, "butt", '<rect x="52" y="38" width="8" height="4" fill="navy"/>'),
]
for i, (name, path, width, cap, expected) in enumerate(cases):
    u.cell(name, (i % 4 * 16, i // 4 * 16, 16, 16),
           f'<path d="{path}" fill="none" stroke="navy" stroke-width="{width}" stroke-linecap="{cap}"/>', expected)
u.write()
