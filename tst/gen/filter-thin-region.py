"""A filter region clips by its geometric area, including a thin row.

Each expectation is a grid of whole pixels whose alpha is the independent
rectangle/pixel intersection area. There is no filter in the expectation.
Direct and transformed regions represent the same rectangle. A huge unused
filter width changes neither the source alpha nor its vertical clip.
"""
from math import floor, ceil

from unitgen import Unit


u = Unit('filter-thin-region', 128, 12,
         'Thin filter regions retain their fractional pixel coverage. '
         'Eight independent pixel-area expectations check direct and '
         'transformed regions, a huge region width, flood and source alpha, '
         'half and whole rows, a clip crossing two rows, and a thin column.',
         ground=False)
cases = [
    ('direct', 1, 1, 0, 0, 8, 17 / 256, 8, 2, False, (0, 0, 8, 17 / 256)),
    ('scaled', 1, 1 / 256, 0, 0, 8, 17, 8, 512, False, (0, 0, 8, 17 / 256)),
    ('huge-width', 1, 1 / 256, 0, 0, 4194305, 17, 8, 512, False, (0, 0, 8, 17 / 256)),
    ('flood', 1, 1 / 256, 0, 0, 8, 17, 8, 512, True, (0, 0, 8, 17 / 256)),
    ('half-row', 1, 1 / 256, 0, 0, 8, 128, 8, 512, True, (0, 0, 8, .5)),
    ('whole-row', 1, 1 / 256, 0, 0, 8, 256, 8, 512, True, (0, 0, 8, 1)),
    ('cross-row', 1, 1 / 256, 0, 192, 8, 128, 8, 512, True, (0, .75, 8, .5)),
    ('thin-column', 1 / 256, 1, 0, 0, 17, 8, 512, 8, True, (0, 0, 17 / 256, 8)),
]
for i, (name, sx, sy, fx, fy, fw, fh, sw, sh, flood, expected_box) in enumerate(cases):
    x = i * 16
    primitive = '<feFlood flood-color="black"/>' if flood else '<feGaussianBlur in="SourceAlpha" stdDeviation="0"/>'
    test = (f'<svg x="{x}" width="16" height="12" viewBox="0 0 16 12">'
            f'<defs><filter id="f{i}" filterUnits="userSpaceOnUse" '
            f'x="{fx}" y="{fy}" width="{fw}" height="{fh}">{primitive}</filter></defs>'
            f'<g transform="translate(2 2) scale({sx} {sy})">'
            f'<rect width="{sw}" height="{sh}" fill="red" filter="url(#f{i})"/>'
            '</g></svg>')
    bx, by, bw, bh = expected_box
    bx += x + 2
    by += 2
    plain = []
    for py in range(floor(by), ceil(by + bh)):
        for px in range(floor(bx), ceil(bx + bw)):
            area = (max(0, min(px + 1, bx + bw) - max(px, bx))
                    * max(0, min(py + 1, by + bh) - max(py, by)))
            plain.append(f'<rect x="{px}" y="{py}" width="1" height="1" '
                         f'fill="black" fill-opacity="{area}"/>')
    u.cell(name, (x, 0, 16, 12), test, ''.join(plain))
u.write()
