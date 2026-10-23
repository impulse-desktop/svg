"""A positive thin rounded rectangle approaches a stroked line capsule.

SVG 2 clamps the small corner radius to half the small extent. The
rectangle's centreline stays within that extent of a straight line; its
outer normal sweep gives round caps. A filled capsule gives the limiting
stroke union. At the tested extents the
displacement is below .00001 pixel, far below the existing D1 curve
approximation; no nonzero extent may be treated as zero.
"""
from unitgen import Unit

unit = Unit('shape-flat-rounded', 100, 200,
            'Positive thin rounded rectangles retain their stroke. SVG2 '
            'clamps each radius independently to half the corresponding '
            'extent. The limiting stroke is a single filled capsule. '
            'Exact zero extents draw nothing. '
            'Translation from a large coordinate preserves the positive '
            'extent. All coordinates and extents are authored independently '
            'of rendered output.', ground=False)
for row, extent in enumerate(['.0000001', '.000001', '.00001', '0']):
    for column, vertical in enumerate([False, True]):
        x, y = column * 50, row * 50
        geometry = (f'x="152" y="132" width="{extent}" height="36"' if vertical else
                    f'x="148" y="136" width="36" height="{extent}"')
        test = (f'<g transform="translate({x} {y})">'
                '<g transform="translate(-144 -128)">'
                f'<rect {geometry} '
                'rx="8" ry="8" fill="none" stroke="lime" stroke-width="4"/>'
                '</g></g>')
        path = ('M6 4 A2 2 0 0 1 10 4 V40 A2 2 0 0 1 6 40 Z' if vertical else
                'M4 6 H40 A2 2 0 0 1 40 10 H4 A2 2 0 0 1 4 6 Z')
        capsule = f'<path d="{path}" fill="lime"/>' if extent != '0' else ''
        reference = f'<g transform="translate({x} {y})">{capsule}</g>'
        axis = 'width' if vertical else 'height'
        unit.cell(f'{axis}-{row}', (x, y, 50, 50), test, reference)
unit.write()
