"""marker-group: a marker's content drawn as a group carrying the marker's
own properties, of SVG 2's painting chapter."""
from unitgen import Unit

M = 'markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8"'
unit = Unit("marker-group", 42, 12, """Each lime drawing fills its black
outline, and nothing red shows. A marker's content draws as a group that
carries the marker's own properties: its opacity, a filter, which a box of
bounding box units takes from the content, a clip-path and a mask.""")
unit.common('<defs><filter id="mf" x="0" y="0" width="1" height="1"><feFlood flood-color="lime"/></filter>'
            '<clipPath id="mc"><rect width="4" height="8"/></clipPath>'
            '<mask id="mm" maskUnits="userSpaceOnUse" x="0" y="0" width="4" height="8"><rect width="8" height="8" fill="white"/></mask>'
            f'<marker id="mo" {M} opacity="0.5"><rect width="8" height="8" fill="lime"/></marker>'
            f'<marker id="mfl" {M} filter="url(#mf)"><rect x="2" y="2" width="4" height="4" fill="red"/></marker>'
            f'<marker id="mcl" {M} clip-path="url(#mc)"><rect width="8" height="8" fill="lime"/></marker>'
            f'<marker id="mma" {M} mask="url(#mm)"><rect width="8" height="8" fill="lime"/></marker></defs>', "")


def lime(x, y, w, h, extra=""):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"{extra}/>'


for name, x, marker, rect in (("opacity", 2, "mo", (2, 2, 8, 8, ' opacity="0.5"')), ("filter", 12, "mfl", (14, 4, 4, 4)),
                              ("clip-path", 22, "mcl", (22, 2, 4, 8)), ("mask", 32, "mma", (32, 2, 4, 8))):
    plain = lime(*rect)
    unit.cell(name, (x - 1, 1, 10, 10), f'<path d="M {x} 2 h 1" marker-start="url(#{marker})"/>', plain, lime(*rect[:4]).replace(' fill="lime"', ""))

unit.write()
