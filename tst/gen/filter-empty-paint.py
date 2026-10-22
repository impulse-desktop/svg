"""FillPaint/StrokePaint have infinite extent even on an empty group."""
from unitgen import Unit,png

u=Unit('filter-empty-paint',64,16,
       'Filter paint inputs are the target element paint with infinite extent. Empty SourceGraphic does not empty a solid or user-space paint server. Literal rectangles, checker cells and analytic ramp pixels define the expected drawing.',ground=False)
u.common('<defs><filter id="empty-fill" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feOffset in="FillPaint"/></filter><filter id="empty-stroke" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feOffset in="StrokePaint"/></filter><filter id="empty-source" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feOffset/></filter><pattern id="empty-checker" patternUnits="userSpaceOnUse" width="2" height="2"><rect width="1" height="1" fill="lime"/><rect x="1" y="1" width="1" height="1" fill="blue"/></pattern><linearGradient id="empty-ramp" gradientUnits="userSpaceOnUse" x2="8"><stop stop-color="red"/><stop offset="1" stop-color="blue"/></linearGradient><linearGradient id="empty-bbox"><stop stop-color="lime"/><stop offset="1" stop-color="lime"/></linearGradient></defs>','')
square='<rect width="8" height="8" fill="lime"/>'
checker=''.join(f'<rect x="{x}" y="{y}" width="1" height="1" fill="{"lime" if x%2==0 else "blue"}"/>' for y in range(8) for x in range(8) if x%2==y%2)
ramp=png([[((7.5-x)/8,0,(x+.5)/8,1) for x in range(8)] for _ in range(8)])
cases=[
 ('solid','<g fill="lime" filter="url(#empty-fill)"/>',square),
 ('stroke','<g stroke="lime" filter="url(#empty-stroke)"/>',square),
 ('pattern','<g fill="url(#empty-checker)" filter="url(#empty-fill)"/>',checker),
 ('gradient','<g fill="url(#empty-ramp)" filter="url(#empty-fill)"/>',f'<image width="8" height="8" href="{ramp}"/>'),
 ('bbox-empty','<g fill="url(#empty-bbox)" filter="url(#empty-fill)"/>',''),
 ('source-empty','<g fill="lime" filter="url(#empty-source)"/>',''),
 ('transparent-child','<g fill="lime" filter="url(#empty-fill)"><rect width="8" height="8" fill="none"/></g>',square),
 ('opacity','<g opacity=".5" fill="lime" filter="url(#empty-fill)"/>','<rect width="8" height="8" fill="lime" opacity=".5"/>'),
]
for i,(name,test,expected) in enumerate(cases):
 x,y=i%4*16,i//4*8
 u.cell(name,(x,y,16,8),f'<g transform="translate({x} {y})">{test}</g>',f'<g transform="translate({x} {y})">{expected}</g>')
u.write()
