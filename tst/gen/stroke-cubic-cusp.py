"""A cubic's reversal sweeps a round end even with butt line caps."""
from unitgen import Unit


unit = Unit("stroke-cubic-cusp", 64, 32, """A cubic that doubles back inside
one segment has a round stroke tip at its cusp, as Chromium, resvg and
librsvg draw it. A line that reverses at a corner keeps the butt end.""")

unit.cell("cubic", (4, 2, 48, 12),
          '<path d="M 50 8 C 38 8 22 8 10 8 C 9.8 8 11 8 12 8 C 22 8 38 8 50 8" fill="none" stroke="black" stroke-width="10"/>',
          '<circle cx="10" cy="8" r="5" fill="black"/><rect x="10" y="3" width="40" height="10" fill="black"/>',
          tolerance=4)
unit.cell("line", (4, 18, 48, 12),
          '<path d="M 50 24 L 10 24 L 50 24" fill="none" stroke="black" stroke-width="10"/>',
          '<rect x="10" y="19" width="40" height="10" fill="black"/>')

unit.write()
