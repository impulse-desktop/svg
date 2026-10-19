"""viewport-content-percent: what establishes an SVG viewport, of SVG 2:
patterns, markers, clipPaths and masks do not, so percentages in their
content are of the viewport around them, not of their tile, marker box or
effect region."""
from unitgen import Unit

unit = Unit("viewport-content-percent", 64, 32, """Each lime square matches
its plain twin in the expected drawing. Pattern, marker, clipPath and mask
establish no viewport: a rect 25% wide and 75% high in their content is 16
by 24 of this 64 by 32 drawing, not a part of the tile, the marker's box or
the region, and covers the whole square. In content of bounding box units a
percentage is of the viewport still, then scaled with the box: 1.5625% wide
and high is 1 by 0.5, the square's top half.""")
PERCENT = 'width="25%" height="75%"'
unit.common(f'<defs><pattern id="tile" patternUnits="userSpaceOnUse" x="2" y="2" width="12" height="12"><rect {PERCENT} fill="lime"/></pattern>'
            f'<marker id="mark" markerWidth="12" markerHeight="12" markerUnits="userSpaceOnUse"><rect {PERCENT} fill="lime"/></marker>'
            f'<clipPath id="clip"><rect x="34" y="2" {PERCENT}/></clipPath>'
            f'<mask id="mask" maskUnits="userSpaceOnUse" x="0" y="0" width="64" height="16"><rect x="50" y="2" {PERCENT} fill="white"/></mask>'
            '<clipPath id="clip-box" clipPathUnits="objectBoundingBox"><rect width="1.5625%" height="1.5625%"/></clipPath>'
            '<mask id="mask-box" maskContentUnits="objectBoundingBox"><rect width="1.5625%" height="1.5625%" fill="white"/></mask></defs>', "")

for k, (name, test) in enumerate((
        ("pattern", '<rect x="2" y="2" width="12" height="12" fill="url(#tile)"/>'),
        ("marker", '<path d="M 18 2 h 1" marker-start="url(#mark)"/>'),
        ("clip-path", '<rect x="34" y="2" width="12" height="12" fill="lime" clip-path="url(#clip)"/>'),
        ("mask", '<rect x="50" y="2" width="12" height="12" fill="lime" mask="url(#mask)"/>'))):
    x = 2 + 16 * k
    unit.cell(name, (x - 1, 1, 14, 14), test, f'<rect x="{x}" y="2" width="12" height="12" fill="lime"/>', f'<rect x="{x}" y="2" width="12" height="12"/>')

for k, (name, effect) in enumerate((("clip-path-box", 'clip-path="url(#clip-box)"'), ("mask-box", 'mask="url(#mask-box)"'))):
    x = 2 + 16 * k
    unit.cell(name, (x - 1, 17, 14, 14), f'<rect x="{x}" y="18" width="12" height="12" fill="lime" {effect}/>', f'<rect x="{x}" y="18" width="12" height="6" fill="lime"/>', f'<rect x="{x}" y="18" width="12" height="6"/>')

unit.write()
