"""The two caps of a zero-length dash make one tangent-oriented square.

Fixed diamonds/rectangles are the expectations. This guards the explicit
SVG cap geometry against the device-axis dots Chromium/resvg draw on the
real circle icons 21459, 26709 and 26299.
"""
from math import sqrt
from unitgen import Unit

u = Unit('stroke-zero-dash-direction', 100, 40,
         'A zero-length dash with square caps follows the path tangent. Cardinal directions make axis-aligned squares; diagonal directions make diamonds. A translucent dot is painted once, and a nonuniform transform acts on the square built in user space. Expected polygons and rectangles use fixed analytic vertices.')
cases=[('east',8,0),('north',0,-8),('northeast',8,-8),('northwest',-8,-8),('west',-8,0),('south',0,8),('southeast',8,8),('southwest',-8,8),('translucent',8,8),('stretched',8,8)]
for i,(name,dx,dy) in enumerate(cases):
 x,y=(i%5)*20,(i//5)*20;cx,cy=x+10,y+10;alpha=' stroke-opacity=".5"' if name=='translucent' else '';fillalpha=' fill-opacity=".5"' if name=='translucent' else ''
 scale=' scale(1.25 .75)' if name=='stretched' else ''
 test=f'<path transform="translate({cx} {cy}){scale}" d="M0 0l{dx} {dy}" fill="none" stroke="lime" stroke-width="6" stroke-linecap="square" stroke-dasharray="0 1000"{alpha}/>'
 if dx and dy:
  rx=3*sqrt(2)*(1.25 if scale else 1);ry=3*sqrt(2)*(.75 if scale else 1)
  plain=f'<path d="M{cx-rx:.9f} {cy}L{cx} {cy-ry:.9f}L{cx+rx:.9f} {cy}L{cx} {cy+ry:.9f}Z" fill="lime"{fillalpha}/>'
 else:plain=f'<rect x="{cx-3}" y="{cy-3}" width="6" height="6" fill="lime"/>'
 u.cell(name,(x,y,20,20),test,plain)
u.write()
