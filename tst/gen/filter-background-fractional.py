"""Exact fractional restriction of stored BackgroundImage/BackgroundAlpha.

The backdrop is a literal half-alpha PNG, not a geometric edge. Its
fractional filter frame cuts through that constant pixel plane. Exact
rectangle areas and source-over equations give every expected component.
"""
import math
from unitgen import Unit,png

weights=[(k,math.exp(-k*k/(2*.5*.5)))for k in range(-2,3)]
total=sum(v for _,v in weights)
weights=[(k,v/total)for k,v in weights]

unit=Unit('filter-background-fractional',80,60,
          'BackgroundImage and BackgroundAlpha retain the exact fractional '
          'filter-frame coverage when their pixels are copied or blended. '
          'Literal half-alpha PNG input, rectangle/pixel intersections and '
          'source-over provide the independent reference. A shifted input, '
          'two readers, both colour spaces and a subpixel frame guard '
          'against applying that coverage twice.',ground=False)
for i,(name,kind,x0,y0,w,h)in enumerate([
        ('image-offset','image',2.25,2.5,9.5,8.25),
        ('alpha-offset','alpha',2.25,2.5,9.5,8.25),
        ('image-blend','blend',2.25,2.5,9.5,8.25),
        ('whole-frame','image',2,2,10,8),
        ('input-shift','shift',2.25,2.5,9.5,8.25),
        ('image-alpha-readers','both',2.25,2.5,9.5,8.25),
        ('tiny-frame','image',5.25,6.5,.5,.25),
        ('srgb-linear-readers','spaces',2.25,2.5,9.5,8.25),
        ('flood-alpha','flood',2.25,2.5,9.5,8.25),
        ('blur-input','blur',2.25,2.5,9.5,8.25),
        ('blur-alpha-readers','blur-alpha',2.25,2.5,9.5,8.25)]):
    colour=(32/255,191/255,64/255)if kind=='spaces'else(0,1,0)
    background=[[colour+(128/255,)for x in range(16)]for y in range(16)]
    expected=[]
    for y in range(16):
        row=[]
        for x in range(16):
            left=x0+1 if kind=='shift'else x0
            coverage=max(0,min(x+1,x0+w)-max(x,left))*max(0,min(y+1,y0+h)-max(y,y0))
            a=128/255;copied=a*coverage
            if kind=='both':copied*=copied
            if kind=='flood':copied*=coverage
            if kind in ['blur','blur-alpha']:
                def input_alpha(xx,yy):
                    return a*max(0,min(xx+1,x0+w)-max(xx,x0))*max(0,min(yy+1,y0+h)-max(yy,y0))
                copied=sum(input_alpha(x+dx,y+dy)*u*v for dx,u in weights for dy,v in weights)*coverage
                if kind=='blur-alpha':copied*=a*coverage
            out_alpha=copied+a*(1-copied)
            filtered_colour=(1,0,0)if kind=='flood'else(0,0,0)if kind=='alpha'else colour
            rgb=tuple((c*a*(1-copied)+f*copied)/out_alpha for c,f in zip(colour,filtered_colour))
            row.append(rgb+(out_alpha,))
        expected.append(row)
    if kind=='blend':
        # The transparent SourceGraphic leaves BackgroundImage unchanged.
        primitive='<feBlend in2="BackgroundImage" mode="screen"/>'
        fill='none'
    elif kind in ['blur','blur-alpha']:
        primitive='<feGaussianBlur in="BackgroundImage" stdDeviation=".5"/>'
        if kind=='blur-alpha':primitive+='<feComposite in2="BackgroundAlpha" operator="in"/>'
        fill='red'
    elif kind=='flood':
        primitive='<feFlood flood-color="red"/><feComposite in2="BackgroundAlpha" operator="in"/>'
        fill='red'
    elif kind=='both':
        primitive=('<feOffset in="BackgroundImage" result="image"/>'
                   '<feOffset in="BackgroundAlpha" result="alpha"/>'
                   '<feComposite in="image" in2="alpha" operator="in"/>')
        fill='red'
    elif kind=='spaces':
        primitive=('<feOffset in="BackgroundImage" color-interpolation-filters="linearRGB" result="linear"/>'
                   '<feOffset in="BackgroundImage" color-interpolation-filters="sRGB" result="srgb"/>'
                   '<feComposite in="linear" in2="srgb" operator="arithmetic" k2=".5" k3=".5" color-interpolation-filters="sRGB"/>')
        fill='red'
    else:
        source='BackgroundAlpha'if kind=='alpha'else'BackgroundImage'
        shift=' dx="1"'if kind=='shift'else''
        primitive=f'<feOffset in="{source}"{shift}/>';fill='red'
    x,y=i%4*20+2,i//4*20+2
    unit.cell(name,(x,y,16,16),
              f'<svg x="{x}" y="{y}" width="16" height="16"><defs>'
              f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="{x0}" y="{y0}" width="{w}" height="{h}">'
              f'{primitive}</filter></defs><image width="16" height="16" href="{png(background)}"/>'
              f'<rect x="{x0}" y="{y0}" width="{w}" height="{h}" fill="{fill}" filter="url(#f{i})"/></svg>',
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(expected)}"/>')
unit.write()
