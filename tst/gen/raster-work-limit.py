"""raster-work-limit: rows whose exact bands would visit more samples than
the work limit allows are sampled on scanlines instead (D58)."""
from unitgen import Unit

DOTS = 'stroke="teal" stroke-opacity=".3" stroke-dasharray="0.0005" stroke-linecap="round"'
PLAIN = 'stroke="teal" stroke-opacity=".3" stroke-linecap="round"'

unit = Unit("raster-work-limit", 64, 56, """Dashes of 0.0005 with round
caps along a line are tens of thousands of dots, each overlapping the
thousand beside it, whose edges keep crossing within every row: the union
is the line with round caps, scalloped by less than a millionth of a
pixel. An exact sweep would cross without end in effect; past the work
limit a row is sampled on scanlines (D58). A row of dots across the
canvas, its caps past both sides, has too many edges to sort; a column of
dots one pixel wide crosses past the limit in every row. A wide line
dashed every 0.0002 is a hundred thousand slivers side by side, half
covering the rows it spans, whose edges all pass from row to row: too
many to sort into an order, each row under it is sampled.""")
unit.cell("row", (0, 0, 64, 8), f'<line x1="0" y1="4" x2="64" y2="4" {DOTS}/>', f'<line x1="0" y1="4" x2="64" y2="4" {PLAIN}/>')
unit.cell("column", (56, 8, 8, 48), f'<line x1="56.5" y1="8" x2="56.5" y2="56" {DOTS}/>', f'<line x1="56.5" y1="8" x2="56.5" y2="56" {PLAIN}/>')
unit.cell("crowd", (0, 16, 48, 16), '<line x1="0" y1="24" x2="48" y2="24" stroke="teal" stroke-width="10" stroke-dasharray="0.0002"/>', '<rect y="19" width="48" height="10" fill="teal" fill-opacity=".5"/>')
unit.write()
