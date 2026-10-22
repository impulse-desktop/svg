"""Legacy transform names share their standard property's cascade slot.

Compatibility Standard, https://compat.spec.whatwg.org/#css-properties .
Expected shapes use literal coordinates and colors, not CSS transforms.
The perspective examples use 20/(20-10)=2 and the stated origin.
"""
from unitgen import Unit

rect = '<rect x="2" y="2" width="4" height="4" fill="lime" {}/>'
box = '<rect x="{}" y="2" width="4" height="4" fill="lime"/>'
cases = []
def styled(name, style, x, extra=''):
    cases.append((name, rect.format(f'style="{style}" {extra}'), box.format(x)))

styled('translate', '-webkit-transform:translate(4px)', 6)
styled('case', '-WEBKIT-TRANSFORM:translate(4px)', 6)
styled('alias-last', 'transform:translate(2px);-webkit-transform:translate(4px)', 6)
styled('standard-last', '-webkit-transform:translate(4px);transform:translate(2px)', 4)
styled('alias-important', '-webkit-transform:translate(4px)!important;transform:translate(2px)', 6)
styled('standard-important', 'transform:translate(2px)!important;-webkit-transform:translate(4px)', 4)
styled('important-order', '-webkit-transform:translate(4px)!important;transform:translate(2px)!important', 4)
styled('invalid', 'transform:translate(2px);-webkit-transform:translate(garbage)', 4)
styled('variable', '--t:translate(4px);-webkit-transform:var(--t)', 6)
styled('variable-fallback', '-webkit-transform:var(--missing,translate(4px))', 6)
styled('initial', 'transform:translate(4px);-webkit-transform:initial', 2)
styled('unset', 'transform:translate(4px);-webkit-transform:unset', 2)
cases.append(('inherit', '<g style="transform:translate(2px)">'+rect.format('style="-webkit-transform:inherit"')+'</g>', box.format(6)))
cases.append(('sheet', '<style>.alias-sheet{-webkit-transform:translate(4px)}</style>'+rect.format('class="alias-sheet"'), box.format(6)))
cases.append(('clip-child', '<defs><clipPath id="child"><rect width="16" height="16" style="-webkit-transform:scale(.5)"/></clipPath></defs><rect width="16" height="16" fill="lime" clip-path="url(#child)"/>', '<rect width="8" height="8" fill="lime"/>'))
cases.append(('clip-container', '<defs><clipPath id="container" style="-webkit-transform:translate(4px)"><rect width="8" height="8"/></clipPath></defs><rect width="16" height="16" fill="lime" clip-path="url(#container)"/>', '<rect x="4" width="8" height="8" fill="lime"/>'))
cases.append(('use', '<defs>'+rect.format('id="alias-tile"')+'</defs><use href="#alias-tile" style="-webkit-transform:translate(4px)"/>', box.format(6)))
cases.append(('origin', '<rect x="4" y="4" width="4" height="2" fill="lime" style="transform:rotate(90deg);transform-origin:0 0;-webkit-transform-origin:50% 50%"/>', '<rect x="10" y="4" width="2" height="4" fill="lime"/>'))
layers = '<rect x="4" y="4" width="8" height="8" fill="lime" style="transform:translateZ(2px)"/><rect x="4" y="4" width="8" height="8" fill="red"/>'
cases.append(('preserve-3d', '<g style="transform-style:flat;-webkit-transform-style:preserve-3d">'+layers+'</g>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'))
cases.append(('flat-last', '<g style="-webkit-transform-style:preserve-3d;transform-style:flat">'+layers+'</g>', '<rect x="4" y="4" width="8" height="8" fill="red"/>'))
cases.append(('backface-hidden', rect.format('style="transform:rotateY(180deg);transform-origin:4px 4px;backface-visibility:visible;-webkit-backface-visibility:hidden"'), ''))
cases.append(('backface-visible', rect.format('style="transform:rotateY(180deg);transform-origin:4px 4px;backface-visibility:hidden;-webkit-backface-visibility:visible"'), box.format(2)))
perspective_child = '<rect x="2" y="2" width="2" height="2" fill="lime" style="transform:translateZ(10px)"/>'
cases.append(('perspective', '<g style="-webkit-perspective:20px;perspective-origin:0 0">'+perspective_child+'</g>', '<rect x="4" y="4" width="4" height="4" fill="lime"/>'))
cases.append(('perspective-origin', '<g style="perspective:20px;perspective-origin:0 0;-webkit-perspective-origin:100% 0">'+perspective_child.replace('x="2"','x="10"')+'</g>', '<rect x="4" y="4" width="4" height="4" fill="lime"/>'))
cases.append(('perspective-reset', '<g style="perspective:20px;-webkit-perspective:none;perspective-origin:0 0">'+perspective_child+'</g>', '<rect x="2" y="2" width="2" height="2" fill="lime"/>'))
styled('unknown-vendor', '-vendor-transform:translate(4px)', 2)
styled('unknown-webkit', '-webkit-fill:red', 2)
styled('attribute-guard', '', 2, 'webkit-transform="translate(4)"')
unit=Unit('transform-webkit-aliases',64,112,
          'Legacy WebKit aliases of transform, origin, style, perspective, perspective-origin and backface-visibility share validation and cascade order with their standard properties. Literal rectangles independently test coordinates, clipping, inheritance, variables and 3D ordering. Unknown prefixes and presentation attributes stay unknown.', ground=False)
for i,(name,test,expected) in enumerate(cases):
    x,y=i%4*16,i//4*16
    unit.cell(name,(x,y,16,16),f'<svg x="{x}" y="{y}" width="16" height="16">{test}</svg>',f'<g transform="translate({x} {y})">{expected}</g>')
unit.write()
