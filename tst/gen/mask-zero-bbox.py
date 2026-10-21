"""A mask in object bounding box units is ignored on zero-sized geometry."""

from unitgen import Unit


unit = Unit(
    "mask-zero-bbox", 128, 64,
    "SVG 2 objectBoundingBox ignores the stroke width: a horizontal or "
    "vertical shape has a zero-sized box, so its mask effect is ignored. "
    "Positive boxes and user-space masks still mask normally.",
)
unit.common('''<defs>
  <mask id="black"><rect width="128" height="64" fill="black"/></mask>
  <mask id="white"><rect width="128" height="64" fill="white"/></mask>
  <mask id="user-black" maskUnits="userSpaceOnUse" x="0" y="0" width="128" height="64">
    <rect width="128" height="64" fill="black"/>
  </mask>
</defs>''', "")


def line(x1, y1, x2, y2, mask=""):
    return (f'<line x1="{x1}" y1="{y1}" x2="{x2}" y2="{y2}" '
            f'stroke="lime" stroke-width="4"{mask}/>')


unit.cell("horizontal", (0, 0, 32, 32),
          line(4, 16, 28, 16, ' mask="url(#black)"'),
          line(4, 16, 28, 16))
unit.cell("vertical", (32, 0, 32, 32),
          line(48, 4, 48, 28, ' mask="url(#black)"'),
          line(48, 4, 48, 28))
unit.cell("path", (64, 0, 32, 32),
          '<path d="M68 16 H92" stroke="lime" stroke-width="4" mask="url(#black)"/>',
          '<path d="M68 16 H92" stroke="lime" stroke-width="4"/>')
unit.cell("group", (96, 0, 32, 32),
          '<g mask="url(#black)">' + line(100, 16, 124, 16) + '</g>',
          line(100, 16, 124, 16))
unit.cell("diagonal", (0, 32, 32, 32),
          line(4, 36, 28, 60, ' mask="url(#black)"'), "")
unit.cell("rect", (32, 32, 32, 32),
          '<rect x="40" y="40" width="16" height="16" fill="lime" mask="url(#black)"/>', "")
unit.cell("user-space", (64, 32, 32, 32),
          line(68, 48, 92, 48, ' mask="url(#user-black)"'), "")
unit.cell("white", (96, 32, 32, 32),
          line(100, 48, 124, 48, ' mask="url(#white)"'),
          line(100, 48, 124, 48))
unit.write()
