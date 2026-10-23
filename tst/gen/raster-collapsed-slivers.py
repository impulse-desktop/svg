"""Tiny finite contours round to zero coverage, including in busy rows.

Each horizontal contour has area6e-8 square pixels. The dense vertical
contours have width1e-8; even the sum of all their unclipped areas is below
0.5/255. Every affected output alpha must therefore round to zero. The
opaque last row independently checks that later paints still run.
"""
from fractions import Fraction as F

from unitgen import Unit


unit = Unit('raster-collapsed-slivers', 40, 8,
            'Finite subpixel contours with separated bounds retain zero stored coverage after their exact areas round down. Direction, parity and a dense row with many endpoints are tested; an independent opaque final row must remain intact.',
            ground=False)
for i, name in enumerate(['horizontal', 'reverse', 'evenodd', 'dense-vertical', 'offscreen-tip']):
    if name == 'offscreen-tip':
        paths = ['M-1e38 .25L.5 1L-1e38 .5Z',
                 'M-1e38 .25L2.5 1L-1e38 .5Z']
        for j in range(128):
            y = (j + 1) / 512
            paths.append(f'M-100 {y:.12g}h1v.25h-1z')
        # Remote rectangles are outside. Both visible tips together have
        # area below1e-38; their long sides meet at the bottom of this row.
        area_bound = F(1, 10**38)
    elif name == 'dense-vertical':
        paths = []
        for j in range(128):
            x = 1 + 4 * (j % 2)
            y = (j + 1) / 512
            paths.append(f'M{x} {y:.12g}h.00000001v.25h-.00000001z')
        area_bound = 128 * F(1, 100000000) * F(1, 4)
    else:
        paths = [f'M1 {y}' + ('v.00000001h6v-.00000001z' if name == 'reverse'
                              else 'h6v.00000001h-6z') for y in [1, 5]]
        area_bound = 2 * 6 * F(1, 100000000)
    assert 255 * area_bound < F(1, 2)
    x = i * 8
    witness = '<rect y="7" width="8" height="1" fill="navy"/>'
    shape = (f'<path d="{" ".join(paths)}" fill="red" '
             f'fill-rule="{"evenodd" if name == "evenodd" else "nonzero"}"/>')
    unit.cell(name, (x, 0, 8, 8),
              f'<svg x="{x}" width="8" height="8">{shape}{witness}</svg>',
              f'<rect x="{x}" y="7" width="8" height="1" fill="navy"/>', tolerance=0)
unit.write()
