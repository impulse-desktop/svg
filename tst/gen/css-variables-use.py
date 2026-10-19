"""css-variables-use: custom properties through use in a document whose
selectors are all of one compound, which keeps no instance cascades for
structure alone."""
from unitgen import Unit

unit = Unit("css-variables-use", 34, 10, """Each lime square fills its black
outline, and nothing red shows. With no selector of more than one compound
in the document, an instance still takes its custom properties from its
use: a use's own, one it inherits, each use of one symbol its own, one a
rule gives the use.""")
unit.common('<style>.vu { --c: lime }</style><symbol id="s"><rect width="6" height="6" style="fill: var(--c, red)"/></symbol>')
for k, (name, test) in enumerate([("own", '<use x="{x}" y="2" href="#s" style="--c: lime"/>'), ("inherited", '<g style="--c: lime"><use x="{x}" y="2" href="#s"/></g>'),
                                  ("twice", '<use x="{x}" y="2" href="#s" style="--c: red"/><use x="{x}" y="2" href="#s" style="--c: lime"/>'), ("rule", '<use class="vu" x="{x}" y="2" href="#s"/>')]):
    x = 2 + k * 8
    unit.cell(name, (x - 1, 1, 8, 8), test.format(x=x), f'<rect x="{x}" y="2" width="6" height="6" fill="lime"/>', f'<rect x="{x}" y="2" width="6" height="6"/>')
unit.write()
