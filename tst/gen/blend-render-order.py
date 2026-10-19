"""blend-render-order: the order of filter, clipping, masking and blending
of Compositing 1."""
from unitgen import Unit

B, S = (51, 153, 230), (204, 77, 26)
unit = Unit("blend-render-order", 48, 16, """The lime drawing fills its black
outline; each other test runs on into the reference under it without a
step. A filter applies before the clip, so the clip cuts the shifted
drawing; a filter's colour, then a mask or opacity, apply before the
blend, which mixes only what they leave over the backdrop.""", ground=False)
unit.common('''<defs><filter id="o" filterUnits="userSpaceOnUse" x="0" y="0" width="48" height="16"><feOffset dx="4"/></filter>
<clipPath id="c"><rect x="4" y="2" width="6" height="12"/></clipPath>
<filter id="f" x="0" y="0" width="1" height="1"><feFlood flood-color="#cc4d1a"/></filter>
<mask id="h"><rect width="48" height="16" fill="white" fill-opacity=".5"/></mask></defs>''')


def hexed(c):
    return "#%02x%02x%02x" % tuple(round(v) for v in c)


multiplied = [x * y / 255 for x, y in zip(B, S)]
half = [x / 2 + y / 2 for x, y in zip(multiplied, B)]
unit.cell("filter-clip", (1, 1, 12, 14), '<rect x="0" y="2" width="8" height="12" fill="lime" filter="url(#o)" clip-path="url(#c)"/>', '<rect x="4" y="2" width="6" height="12" fill="lime"/>',
          '<rect x="4" y="2" width="6" height="12"/>')
for k, (name, attributes, reference) in enumerate([("filter-blend", 'fill="lime" filter="url(#f)"', multiplied), ("mask-blend", f'fill="{hexed(S)}" mask="url(#h)"', half), ("opacity-blend", f'fill="{hexed(S)}" opacity=".5"', half)]):
    x = 16 + 10 * k
    unit.cell(name, (x - 1, 1, 10, 14), f'<rect x="{x}" y="2" width="8" height="6" fill="{hexed(B)}"/><rect x="{x}" y="2" width="8" height="6" {attributes} style="mix-blend-mode: multiply"/>'
              f'<rect x="{x}" y="8" width="8" height="6" fill="{hexed(reference)}"/>', f'<rect x="{x}" y="2" width="8" height="12" fill="{hexed(reference)}"/>')
unit.write()
