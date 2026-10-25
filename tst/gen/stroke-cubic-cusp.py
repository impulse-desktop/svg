"""A cubic's reversal sweeps a round end even with butt line caps."""
import math

from unitgen import Unit


unit = Unit("stroke-cubic-cusp", 64, 32, """A cubic that doubles back inside
one segment has a round stroke tip at its cusp, as Chromium, resvg and
librsvg draw it. A line that reverses at a corner keeps the butt end.""")

# M10 8 C9.8 8 11 8 12 8 reverses where x'(t)=0, not at x=10.
# Find that stationary point directly from the source polynomial.
p0, p1, p2, p3 = 10., 9.8, 11., 12.
a = p3 - 3*p2 + 3*p1 - p0
b = 3*(p2 - 2*p1 + p0)
c = 3*(p1 - p0)
t = -2*c / (2*b + math.sqrt(4*b*b - 12*a*c))
assert 0 < t < 1
cusp = p0 + t*(c + t*(b + t*a))

unit.cell("cubic", (4, 2, 48, 12),
          '<path d="M 50 8 C 38 8 22 8 10 8 C 9.8 8 11 8 12 8 C 22 8 38 8 50 8" fill="none" stroke="black" stroke-width="10"/>',
          f'<circle cx="{cusp:.15g}" cy="8" r="5" fill="black"/><rect x="{cusp:.15g}" y="3" width="{50-cusp:.15g}" height="10" fill="black"/>',
          tolerance=4)
unit.cell("line", (4, 18, 48, 12),
          '<path d="M 50 24 L 10 24 L 50 24" fill="none" stroke="black" stroke-width="10"/>',
          '<rect x="10" y="19" width="40" height="10" fill="black"/>')

unit.write()
