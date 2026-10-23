"""Root CSS corner clipping, with independently fixed SVG shape controls.

Each data-URL image is a separate root so its CSS box is the renderer's
viewport, not a graphic descendant's object bounding box. Expected shapes
use literal radii/arc endpoints; they never parse CSS or normalize radii.
"""
import base64
from unitgen import Unit

u = Unit('root-border-radius', 240, 252,
         'Each root viewport clips its graphic content to its CSS corner curve. Radius lengths and percentages, the slash and one-to-four expansion, longhand cascade, invalid declarations and CSS-wide keywords are tested against fixed rectangles and arc paths. Internal SVG graphics acquire no CSS border box.', ground=False)

rect = '<rect width="32" height="20" fill="lime"/>'
r4 = '<rect width="32" height="20" rx="4" fill="lime"/>'
r8_4 = '<rect width="32" height="20" rx="8" ry="4" fill="lime"/>'
ellipse = '<ellipse cx="16" cy="10" rx="16" ry="10" fill="lime"/>'
# The fixed endpoints give clockwise TL, TR, BR, BL quarter ellipses.
asym = '<path fill="lime" d="M2 0H28A4 4 0 0 1 32 4V14A6 6 0 0 1 26 20H8A8 8 0 0 1 0 12V2A2 2 0 0 1 2 0Z"/>'
two = '<path fill="lime" d="M2 0H28A4 4 0 0 1 32 4V18A2 2 0 0 1 30 20H4A4 4 0 0 1 0 16V2A2 2 0 0 1 2 0Z"/>'
three = '<path fill="lime" d="M2 0H28A4 4 0 0 1 32 4V14A6 6 0 0 1 26 20H4A4 4 0 0 1 0 16V2A2 2 0 0 1 2 0Z"/>'
# 24+16 exceeds the 20-high right side: all eight radii shrink by 1/2.
overlap = '<path fill="lime" d="M4 0H20A12 12 0 0 1 32 12V12A8 8 0 0 1 24 20H2A2 2 0 0 1 0 18V4A4 4 0 0 1 4 0Z"/>'
tl = '<path fill="lime" d="M8 0H32V20H0V4A8 4 0 0 1 8 0Z"/>'

cases = [
 ('length', 'border-radius:4px', r4),
 ('two', 'border-radius:2px 4px', two),
 ('three', 'border-radius:2px 4px 6px', three),
 ('four', 'border-radius:2px 4px 6px 8px', asym),
 ('slash', 'border-radius:8px/4px', r8_4),
 ('percent-bases', 'border-radius:25% 20%', '<path fill="lime" d="M8 0H25.6A6.4 4 0 0 1 32 4V15A8 5 0 0 1 24 20H6.4A6.4 4 0 0 1 0 16V5A8 5 0 0 1 8 0Z"/>'),
 ('percent-ellipse', 'border-radius:50%', ellipse),
 ('overlap', 'border-radius:8px 24px 16px 4px', overlap),
 ('large-uniform', 'border-radius:9999px', '<rect width="32" height="20" rx="10" fill="lime"/>'),
 ('zero-axis', 'border-radius:0/4px', rect),
 ('longhand-pair', 'border-top-left-radius:8px 4px', tl),
 ('longhand-override', 'border-radius:4px;border-top-left-radius:8px 4px', '<path fill="lime" d="M8 0H28A4 4 0 0 1 32 4V16A4 4 0 0 1 28 20H4A4 4 0 0 1 0 16V4A8 4 0 0 1 8 0Z"/>'),
 ('shorthand-reset', 'border-top-left-radius:8px 4px;border-radius:4px', r4),
 ('initial', 'border-radius:4px;border-radius:initial', rect),
 ('unset', 'border-radius:4px;border-radius:unset', rect),
 ('inherit-root', 'border-radius:4px;border-radius:inherit', rect),
 ('invalid-negative', 'border-radius:4px;border-radius:-2px', r4),
 ('invalid-unitless', 'border-radius:4px;border-radius:2', r4),
 ('invalid-slash', 'border-radius:4px;border-radius:2px/', r4),
 ('root-font-rem', 'font-size:8px;border-radius:.5rem', r4),
 ('font-length', 'font-size:8px;border-radius:.5em', r4),
 ('calc', 'border-radius:calc(25% - 4px)/calc(50% - 6px)', '<rect width="32" height="20" rx="4" ry="4" fill="lime"/>'),
 ('variable', '--r:8px/4px;border-radius:var(--r)', r8_4),
 ('variable-invalid', '--r:-2px;border-radius:4px;border-radius:var(--r)', rect),
 ('longhand-variable', '--r:8px 4px;border-top-left-radius:var(--r)', tl),
 ('overflow-visible', 'border-radius:50%;overflow:visible', ellipse),
 ('border-no-layout', 'border:10px solid red;padding:3px;border-radius:50%', ellipse),
 ('not-presentation', '', rect),
 ('importance', 'border-radius:4px!important;border-radius:0', r4),
 ('stylesheet', '', r4),
 ('graphic-no-css-box', '', rect),
 ('negative-math-clamp', 'border-radius:calc(-4px)', rect),
 ('longhand-initial', 'border-radius:4px;border-top-left-radius:initial', '<path fill="lime" d="M0 0H28A4 4 0 0 1 32 4V16A4 4 0 0 1 28 20H4A4 4 0 0 1 0 16Z"/>'),
 ('invalid-longhand', 'border-radius:4px;border-top-left-radius:2px 3px 4px', r4),
 ('shorthand-variable-reset', '--r:4px;border-top-left-radius:8px;border-radius:var(--r)', r4),
 ('zero-number', 'border-radius:4px;border-radius:0', rect),
 ('huge-finite-radius', 'border-radius:3e38px', '<rect width="32" height="20" rx="10" fill="lime"/>'),
 ('slash-four', 'border-radius:2px 4px 6px 8px/4px 2px 8px 6px', '<path fill="lime" d="M2 0H28A4 2 0 0 1 32 2V12A6 8 0 0 1 26 20H8A8 6 0 0 1 0 14V4A2 4 0 0 1 2 0Z"/>'),
 ('invalid-longhand-first-number', 'border-radius:4px;border-top-left-radius:2', r4),
 ('invalid-longhand-second-number', 'border-radius:4px;border-top-left-radius:0 2', r4),
 ('invalid-longhand-number-math', 'border-radius:4px;border-top-left-radius:calc(0)', r4),
 ('longhand-zero-number', 'border-radius:4px;border-top-left-radius:0', '<path fill="lime" d="M0 0H28A4 4 0 0 1 32 4V16A4 4 0 0 1 28 20H4A4 4 0 0 1 0 16Z"/>'),
 ('right-longhand-pair', 'border-top-right-radius:8px 4px', '<path fill="lime" d="M0 0H24A8 4 0 0 1 32 4V20H0Z"/>'),
 ('bottom-right-longhand-pair', 'border-bottom-right-radius:8px 4px', '<path fill="lime" d="M0 0H32V16A8 4 0 0 1 24 20H0Z"/>'),
 ('bottom-left-longhand-pair', 'border-bottom-left-radius:8px 4px', '<path fill="lime" d="M0 0H32V20H8A8 4 0 0 1 0 16Z"/>'),
 ('invalid-repeated-slash', 'border-radius:4px;border-radius:2px/3px/4px', r4),
 ('invalid-leading-slash', 'border-radius:4px;border-radius:/2px', r4),
 ('invalid-fifth-radius', 'border-radius:4px;border-radius:1px 2px 3px 4px 5px', r4),
 ('graphic-inherit-no-css-box', 'border-radius:4px', r4),
]
assert len(cases) == 49
for i, (name, style, expected) in enumerate(cases):
 x, y = (i % 6)*40, (i//6)*28
 extra = ' border-radius="50%"' if name == 'not-presentation' else ''
 content = '<style>:root{border-radius:4px}</style>'+rect if name == 'stylesheet' else '<g style="border-radius:50%">'+rect+'</g>' if name == 'graphic-no-css-box' else rect
 if name == 'graphic-inherit-no-css-box':
  content = '<g style="border-radius:inherit"><g style="border-top-left-radius:inherit;border-top-right-radius:inherit;border-bottom-left-radius:inherit;border-bottom-right-radius:inherit">'+rect+'</g></g>'
 svg = f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="20" style="{style}"{extra}>{content}</svg>'
 uri = 'data:image/svg+xml;base64,'+base64.b64encode(svg.encode()).decode()
 u.cell(name, (x, y, 40, 28), f'<image x="{x+4}" y="{y+4}" width="32" height="20" href="{uri}"/>', f'<g transform="translate({x+4} {y+4})">{expected}</g>')
u.write()
