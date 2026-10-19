"""image-cells: an image's position and clip, of SVG 2's embedded content."""
from urllib.parse import quote

from unitgen import Unit, png

LIME = png([[(0, 1, 0, 1)] * 4] * 4)
OVER = "data:image/svg+xml," + quote("<svg xmlns='http://www.w3.org/2000/svg' viewBox='0 0 4 4' overflow='visible'><rect x='-4' y='-4' width='12' height='12' fill='lime'/></svg>")
unit = Unit("image-cells", 32, 12, """Each lime drawing fills its black outline.
CSS positioning does not move an image; an image clips what its document
draws past it, whatever that document's own overflow.""")
unit.cell("css-position", (1, 1, 10, 10), f'<image x="2" y="2" width="8" height="8" href="{LIME}" style="position: relative; top: 4px; left: 4px; margin: 4px"/>',
          '<rect x="2" y="2" width="8" height="8" fill="lime"/>', '<rect x="2" y="2" width="8" height="8"/>')
unit.cell("image-overflow", (13, 1, 18, 10), f'<image x="20" y="2" width="8" height="8" href="{OVER}"/>', '<rect x="20" y="2" width="8" height="8" fill="lime"/>', '<rect x="20" y="2" width="8" height="8"/>')
unit.write()
