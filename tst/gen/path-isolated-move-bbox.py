"""An isolated moveto is not part of a path's object bounding box."""

from unitgen import Unit


unit = Unit(
    "path-isolated-move-bbox", 192, 128,
    "A moveto-only subpath contributes no geometric shape to objectBoundingBox "
    "effects. A zero-length line segment does contribute (SVG 2 coords, "
    "ObjectBoundingBox).",
)
unit.common(
    '''<defs>
  <mask id="region"><rect width="192" height="128" fill="white"/></mask>
  <linearGradient id="object" x1="0" y1="0" x2="0" y2="1">
    <stop offset=".5" stop-color="navy"/><stop offset=".5" stop-color="gold"/>
  </linearGradient>
</defs>''',
    '''<defs>
  <clipPath id="leading"><rect x="6" y="8" width="48" height="24"/></clipPath>
  <clipPath id="trailing"><rect x="70" y="8" width="48" height="24"/></clipPath>
  <clipPath id="line"><rect x="134" y="6" width="48" height="48"/></clipPath>
  <linearGradient id="plain" gradientUnits="userSpaceOnUse" x1="0" y1="74" x2="0" y2="94">
    <stop offset=".5" stop-color="navy"/><stop offset=".5" stop-color="gold"/>
  </linearGradient>
</defs>''',
)


def stroke(path, effect):
    return f'<path d="{path}" fill="none" stroke="lime" stroke-width="20" {effect}/>'


unit.cell(
    "leading-move", (0, 0, 64, 64),
    stroke("M10 50 M10 10 L50 10 L50 30", 'mask="url(#region)"'),
    stroke("M10 10 L50 10 L50 30", 'clip-path="url(#leading)"'),
)
unit.cell(
    "trailing-move", (64, 0, 64, 64),
    stroke("M74 10 L114 10 L114 30 M74 50", 'mask="url(#region)"'),
    stroke("M74 10 L114 10 L114 30", 'clip-path="url(#trailing)"'),
)
unit.cell(
    "zero-length-segment", (128, 0, 64, 64),
    stroke("M138 50 L138 50 M138 10 L178 10 L178 30", 'mask="url(#region)"'),
    stroke("M138 50 L138 50 M138 10 L178 10 L178 30", 'clip-path="url(#line)"'),
)
unit.cell(
    "gradient-box", (0, 64, 64, 64),
    '<path d="M10 114 M10 74 L50 74 L50 94 Z" fill="url(#object)"/>',
    '<path d="M10 74 L50 74 L50 94 Z" fill="url(#plain)"/>',
)
unit.write()
