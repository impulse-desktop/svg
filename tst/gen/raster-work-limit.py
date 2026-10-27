"""raster-work-limit: rows whose exact bands would visit more samples than
the work limit allows are sampled on scanlines instead (D58)."""
from unitgen import Unit

DOTS = 'stroke="teal" stroke-opacity=".3" stroke-dasharray="0.0005" stroke-linecap="round"'
PLAIN = 'stroke="teal" stroke-opacity=".3" stroke-linecap="round"'

unit = Unit("raster-work-limit", 64, 48, """Dashes of 0.0005 with round
caps along a line are tens of thousands of dots, each overlapping the
thousand beside it, whose edges keep crossing within every row: the union
is the line with round caps, scalloped by less than a millionth of a
pixel. Exact bands would split without end in effect; past the work limit
a row is sampled on scanlines (D58). A row of dots across the canvas goes
through the rows transposed into pixel strips, a column of dots one pixel
wide through the bands themselves.""")
unit.cell("row", (0, 0, 48, 8), f'<line x1="0" y1="4" x2="48" y2="4" {DOTS}/>', f'<line x1="0" y1="4" x2="48" y2="4" {PLAIN}/>')
unit.cell("column", (56, 0, 8, 48), f'<line x1="56.5" y1="0" x2="56.5" y2="48" {DOTS}/>', f'<line x1="56.5" y1="0" x2="56.5" y2="48" {PLAIN}/>')
unit.write()
