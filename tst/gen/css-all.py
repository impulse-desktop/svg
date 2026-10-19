"""css-all: the all shorthand of CSS Cascade 5."""
from unitgen import Unit, squares

unit = Unit("css-all", 0, 0, """Each lime square fills its black outline,
and nothing red shows. all sets every property, the geometry ones and the
transforms too, to initial, inherit, unset or revert, the only values it
takes, so a shape declared all initial draws nothing until its geometry is
given again.""")
squares(unit, [
    ("all-initial", [("", '<rect class="t" fill="red" style="all: initial; width: 6px; height: 6px; fill: lime"/>'),
                     ("{c} > .t { all: initial } {c} > .t { width: 6px; height: 6px; fill: lime }", '<rect class="t" width="6" height="6" fill="red" stroke="red"/>'),
                     ("", '<rect width="6" height="6" fill="lime"/><rect class="t" width="6" height="6" fill="red" style="all: initial"/>')]),
    ("all-inherit", [("", '<svg width="6" height="6" fill="lime"><rect fill="red" style="all: inherit"/></svg>')]),
    ("all-unset", [("", '<g fill="lime"><rect class="t" fill="red" opacity="0" style="all: unset; width: 6px; height: 6px"/></g>')]),
    ("all-keywords-only", [("", '<rect class="t" width="6" height="6" fill="lime" style="all: red"/>'), ("", '<rect class="t" width="6" height="6" fill="lime" style="all: initial lime"/>')]),
])
unit.write()
