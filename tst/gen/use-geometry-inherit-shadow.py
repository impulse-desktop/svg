"""CSS geometry inherit on a use clone takes values from the use host."""

from unitgen import Unit


unit = Unit(
    "use-geometry-inherit-shadow", 128, 80,
    "A referenced rect's explicit width:inherit and height:inherit "
    "resolve against the use host in the shadow tree, not the defs "
    "ancestor of the referenced original.",
)
unit.common(
    '<defs style="width:60px;height:48px">'
    '<rect id="template" style="width:inherit;height:inherit" fill="lime"/>'
    '</defs>',
    '',
)
unit.cell(
    "first-host", (0, 0, 40, 40),
    '<use href="#template" x="4" y="4" style="width:12px;height:14px"/>',
    '<rect x="4" y="4" width="12" height="14" fill="lime"/>',
)
unit.cell(
    "second-host", (40, 0, 40, 40),
    '<use href="#template" x="44" y="4" style="width:18px;height:10px"/>',
    '<rect x="44" y="4" width="18" height="10" fill="lime"/>',
)
unit.cell(
    "ordinary-parent", (80, 56, 48, 24),
    '<g style="width:10px;height:12px">'
    '<rect x="84" y="60" style="width:inherit;height:inherit" fill="lime"/>'
    '</g>',
    '<rect x="84" y="60" width="10" height="12" fill="lime"/>',
)
unit.write()
