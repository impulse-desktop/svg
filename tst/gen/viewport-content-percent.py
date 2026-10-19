"""viewport-content-percent: what establishes an SVG viewport, of SVG 2:
patterns, markers, clipPaths and masks do not, so percentages in their
content are of the viewport around them, not of their tile, marker box or
effect region."""
from unitgen import Unit

unit = Unit("viewport-content-percent", 64, 16, """Each lime square matches
its plain twin in the expected drawing. Pattern, marker, clipPath and mask
establish no viewport: a rect 25% wide and 75% high in their content is 16
by 12 of this 64 by 16 drawing, not a part of the tile, the marker's box or
the region, and covers the whole square.""")
PERCENT = 'width="25%" height="75%"'
unit.common(f'<defs><pattern id="tile" patternUnits="userSpaceOnUse" x="2" y="2" width="12" height="12"><rect {PERCENT} fill="lime"/></pattern>'
            f'<marker id="mark" markerWidth="12" markerHeight="12" markerUnits="userSpaceOnUse"><rect {PERCENT} fill="lime"/></marker>'
            f'<clipPath id="clip"><rect x="34" y="2" {PERCENT}/></clipPath>'
            f'<mask id="mask" maskUnits="userSpaceOnUse" x="0" y="0" width="64" height="16"><rect x="50" y="2" {PERCENT} fill="white"/></mask></defs>', "")

for k, (name, test) in enumerate((
        ("pattern", '<rect x="2" y="2" width="12" height="12" fill="url(#tile)"/>'),
        ("marker", '<path d="M 18 2 h 1" marker-start="url(#mark)"/>'),
        ("clip-path", '<rect x="34" y="2" width="12" height="12" fill="lime" clip-path="url(#clip)"/>'),
        ("mask", '<rect x="50" y="2" width="12" height="12" fill="lime" mask="url(#mask)"/>'))):
    x = 2 + 16 * k
    unit.cell(name, (x - 1, 1, 14, 14), test, f'<rect x="{x}" y="2" width="12" height="12" fill="lime"/>', f'<rect x="{x}" y="2" width="12" height="12"/>')

unit.write()
