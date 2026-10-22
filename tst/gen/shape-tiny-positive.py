"""Positive rectangle dimensions stay positive, even below float epsilon.

A rectangle thinner than its stroke has no inner hole: its mitered stroke
is the outer rectangle. The controls use those literal outer bounds.
Exact zero dimensions and negative height still disable rendering.
"""
from unitgen import Unit

u=Unit('shape-tiny-positive',64,32,
       'Finite positive rectangle dimensions are not rounded to zero. Literal outer rectangles check very thin mitered strokes, including a transformed rectangle; exact-zero and invalid negative dimensions draw nothing.',ground=False)
cases=[('height-tenth',.1,False),('height-small',.0001,False),('height-tiny',3.4173e-7,False),('width-tiny',3.4173e-7,True),('scaled',3.4173e-7,False),('height-zero',0,False),('width-zero',0,True),('negative',-1,False)]
for i,(name,size,vertical) in enumerate(cases):
 x,y=i%4*16,i//4*16
 if name=='scaled':
  test='<g transform="scale(.001)"><rect x="2000" y="4000" width="4000" height=".00034173" fill="none" stroke="#aeaeae" stroke-width="2000"/></g>'
 else:
  test=f'<rect x="{4 if vertical else 2}" y="{2 if vertical else 4}" width="{size if vertical else 4}" height="{4 if vertical else size}" fill="none" stroke="#aeaeae" stroke-width="2"/>'
 expected='' if size<=0 else (f'<rect x="3" y="1" width="{2+size}" height="6" fill="#aeaeae"/>' if vertical else f'<rect x="1" y="3" width="6" height="{2+size}" fill="#aeaeae"/>')
 u.cell(name,(x,y,16,16),f'<g transform="translate({x} {y})">{test}</g>',f'<g transform="translate({x} {y})">{expected}</g>')
u.write()
