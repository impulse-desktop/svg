"""An object-box filter on zero-width geometry leaves the source visible."""

from unitgen import Unit


unit = Unit(
    "filter-zero-bbox-group", 128, 64,
    "A filter in objectBoundingBox units is ignored for a stroked vertical "
    "or horizontal path, even when the filter is on its parent group and "
    "the round cap gives the stroke a visible area. A positive geometry "
    "box and a user-space filter still apply the filter.",
)
unit.common('''<defs>
  <filter id="red" x="-2" y="-2" width="5" height="5">
    <feFlood flood-color="red"/>
    <feComposite in2="SourceGraphic" operator="in"/>
  </filter>
  <filter id="red-user" filterUnits="userSpaceOnUse" x="0" y="0" width="128" height="64">
    <feFlood flood-color="red"/>
    <feComposite in2="SourceGraphic" operator="in"/>
  </filter>
</defs>''', "")


def dot(x, y, extra=""):
    return (f'<path d="M{x} {y} v.008" fill="none" stroke="lime" '
            f'stroke-width="12" stroke-linecap="round" {extra}/>' )


def circle(x, y, color="lime"):
    return f'<circle cx="{x}" cy="{y}" r="6" fill="{color}"/>'


unit.cell("direct", (0, 0, 32, 32), dot(16, 16, 'filter="url(#red)"'), circle(16, 16))
unit.cell("group", (32, 0, 32, 32), '<g filter="url(#red)">' + dot(48, 16) + '</g>', circle(48, 16))
unit.cell("non-scaling", (64, 0, 32, 32),
          '<g filter="url(#red)">' + dot(80, 16, 'vector-effect="non-scaling-stroke"') + '</g>',
          circle(80, 16))
unit.cell("horizontal", (96, 0, 32, 32),
          '<path d="M104 16 H120" fill="none" stroke="lime" stroke-width="4" filter="url(#red)"/>',
          '<rect x="104" y="14" width="16" height="4" fill="lime"/>')
unit.cell("positive-box", (0, 32, 32, 32),
          '<rect x="10" y="42" width="12" height="12" fill="lime" filter="url(#red)"/>',
          '<rect x="10" y="42" width="12" height="12" fill="red"/>')
unit.cell("user-space", (32, 32, 32, 32),
          dot(48, 48, 'filter="url(#red-user)"'), circle(48, 48, "red"))
unit.write()
