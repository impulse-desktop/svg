"""geometry-properties: the geometry properties' initial values, percentages
and auto, of SVG 2."""
from unitgen import Unit

unit = Unit("geometry-properties", 64, 32, """Each lime drawing fills its black
outline, and nothing red shows. cx and x percentages are of the viewport's
width, cy and y of its height; cx and cy are 0 when not given; r is not
inherited; width and height auto are 100% on a nested svg, 0 on a rect;
max-width and min-width do nothing (D17); x, y and width on a group, cx and r
on a rect, are no geometry properties and do nothing. A polyline is left
open; a polygon or polyline of no points draws nothing.""")


def cell(name, box, test, plain):
    unit.cell(name, box, test, plain.replace("/>", ' fill="lime"/>'), plain)


cell("center-percent", (11, 11, 10, 10), '<circle cx="25%" cy="50%" r="4" fill="lime"/>', '<circle cx="16" cy="16" r="4"/>')
cell("initial-zero", (1, 1, 8, 8), '<svg x="2" y="2" width="6" height="6" overflow="visible"><circle r="3" fill="lime" transform="translate(3 3)"/></svg>', '<circle cx="5" cy="5" r="3"/>')
cell("not-inherited", (23, 1, 8, 8), '<g style="r: 6px"><circle cx="27" cy="5" r="2" fill="lime"/></g>', '<circle cx="27" cy="5" r="2"/>')
cell("position-percent", (31, 7, 6, 6), '<rect x="50%" y="25%" width="4" height="4" fill="lime"/>', '<rect x="32" y="8" width="4" height="4"/>')
cell("svg-auto", (39, 1, 24, 14), '<svg x="40" y="2" width="auto" height="auto" viewBox="0 0 64 32" preserveAspectRatio="none"><rect width="22" height="12" fill="lime"/></svg>', '<rect x="40" y="2" width="22" height="12"/>')
cell("other-auto", (1, 17, 10, 10), '<rect x="2" y="18" width="auto" height="8" fill="red"/>', "")
cell("max-width", (23, 17, 14, 10), '<rect x="24" y="18" width="12" height="8" fill="lime" style="max-width: 2px; min-width: 20px"/>', '<rect x="24" y="18" width="12" height="8"/>')
cell("not-designated", (39, 17, 12, 12), '<g x="20" y="10" width="0"><rect x="40" y="18" width="10" height="10" fill="lime" cx="9" r="9"/></g>', '<rect x="40" y="18" width="10" height="10"/>')
unit.cell("polyline-open", (52, 17, 11, 14), '<polyline points="54,28 61,28 61,20" fill="none" stroke="lime" stroke-width="2"/>', '<path d="M54 28H61V20" fill="none" stroke="lime" stroke-width="2"/>')
unit.cell("points-none", (11, 21, 10, 10), '<polygon fill="red" stroke="red"/><polyline points="" fill="red" stroke="red" transform="translate(12 22)"/>', "")
unit.write()
