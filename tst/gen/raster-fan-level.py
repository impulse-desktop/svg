"""raster-fan-level: an edge nearly level whose line passes where others
meet, but that ends before it gets there, is not sorted with them."""
from unitgen import Unit

# The top of an outline a hundredth of a pixel thick at y 0.004, cut by
# two notches a pixel deep, each with a knot of edges a thousandth of a
# pixel across at its top. The edges of a knot meet at a point that the
# lines of the level edges either side pass within a thousandth of a
# pixel, at the height where one of those starts, seven pixels off.
KNOTTED = ("M29.0663 0L34.4215 .004L35.4705 1.0668L36.5331 .0036L36.5331 .0072L36.5333 .0072L36.5333 .0034"
           "L36.5331 .0036L36.5331 .0032L36.5333 .0034L36.5333 .0034L37.5963 1.067L38.6453 .004L43.4999 .0036"
           "L48.3547 .0042L49.4037 1.0672L50.4667 .0036L50.4667 .0036L50.4669 .0034L50.4669 .0038L50.4667 .0036"
           "L50.4667 .0074L50.4669 .0074L50.4669 .0038L51.5295 1.067L52.5785 .0042L57.9337 .0002L60 6L27 6Z")
PLAIN = ("M29.0663 0L34.4215 .004L35.4705 1.0668L36.5331 .0036L37.5963 1.067L38.6453 .004L43.4999 .0036"
         "L48.3547 .0042L49.4037 1.0672L50.4667 .0036L51.5295 1.067L52.5785 .0042L57.9337 .0002L60 6L27 6Z")

unit = Unit("raster-fan-level", 64, 8, """Two notches a pixel deep cut
the top of an outline, each with a knot of edges a thousandth of a pixel
across at its top. Where the edges of a knot meet, the lines of the level
edges either side pass within a thousandth of a pixel, but those edges end
pixels away: the notches stay open, as without the knots.""")
unit.cell("notches", (24, 0, 40, 8), f'<path d="{KNOTTED}"/>', f'<path d="{PLAIN}"/>')
unit.write()
