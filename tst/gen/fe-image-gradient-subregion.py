"""Fractional feImage placement of a literal SVG gradient rectangle.

Straight stop colours and alpha interpolate separately. Exact rectangle
areas precede the independently stated in/over equations. A containing
primitive/filter frame does not apply the same edge coverage a second time.
"""
import base64
from unitgen import Unit,png

def linear(c):return c/12.92 if c<=.04045 else ((c+.055)/1.055)**2.4
def srgb(c):return 12.92*c if c<=.0031308 else 1.055*c**(1/2.4)-.055

w,h=13.5,9.25
inner=(f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}">'
       f'<defs><linearGradient id="g" gradientUnits="userSpaceOnUse" x1="{w}" y1="0" x2="0" y2="0">'
       '<stop offset="-.25" stop-color="red" stop-opacity="0"/>'
       '<stop offset="1.25" stop-color="blue"/></linearGradient></defs>'
       f'<rect width="{w}" height="{h}" fill="url(#g)" opacity=".5"/></svg>')
href='data:image/svg+xml;base64,'+base64.b64encode(inner.encode()).decode()
unit=Unit('fe-image-gradient-subregion',80,16,
          'An explicit fractional feImage subregion retains one exact '
          'rectangle coverage. Independent straight SVG stop interpolation '
          'and alpha, in and normal-over equations check sRGB and linearRGB '
          'filter spaces, out-of-range stop clamping and SVG rect opacity.',ground=False)
for i,(name,space,operation)in enumerate([
        ('image-srgb','sRGB','image'),('image-linear','linearRGB','image'),
        ('source-in','sRGB','in'),('source-over','linearRGB','over')]):
    expected=[]
    for y in range(16):
        row=[]
        for x in range(20):
            area=max(0,min(x+1,2.25+w)-max(x,2.25))*max(0,min(y+1,2.5+h)-max(y,2.5))
            t=max(0,min(1,1-(x+.5-2.25)/w));rgb=[1-t,0,t];alpha=.5*t*area
            if space=='linearRGB':rgb=[linear(c)for c in rgb]
            paint=[c*alpha for c in rgb]+[alpha]
            host=[92/255,107/255,121/255]
            if space=='linearRGB':host=[linear(c)for c in host]
            host=[c*area for c in host]+[area]
            if operation in ['in','over']:paint=[v*area for v in paint]
            if operation=='over':paint=[v+b*(1-paint[3])for v,b in zip(paint,host)]
            alpha=paint[3];rgb=[v/alpha if alpha else 0 for v in paint[:3]]
            if space=='linearRGB':rgb=[srgb(c)for c in rgb]
            row.append(tuple(rgb)+(alpha,))
        expected.append(row)
    nodes=f'<feImage x="2.25" y="2.5" width="{w}" height="{h}" preserveAspectRatio="none" href="{href}"/>'
    if operation in ['in','over']:nodes+='<feComposite operator="in" in2="SourceGraphic"/>'
    if operation=='over':nodes+='<feBlend in2="SourceGraphic"/>'
    x=i*20
    unit.cell(name,(x,0,20,16),
              f'<svg x="{x}" width="20" height="16"><defs><filter id="f{i}" '
              f'filterUnits="userSpaceOnUse" x="2.25" y="2.5" width="{w}" height="{h}" '
              f'color-interpolation-filters="{space}">{nodes}</filter></defs>'
              f'<rect x="2.25" y="2.5" width="{w}" height="{h}" fill="#5c6b79" filter="url(#f{i})"/></svg>',
              f'<image x="{x}" width="20" height="16" href="{png(expected)}"/>')
unit.write()
