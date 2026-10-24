"""Independent inner-shadow arithmetic on literal premultiplied pixels.

The expected alpha is max(0, SourceAlpha - blur(offset(SourceAlpha))).
An explicit colour matrix makes it white at .35 alpha; the last merge
paints that over SourceGraphic in linearRGB. No SVG raster is an oracle.
"""
import math
from unitgen import Unit,png

def linear(c):
    return c/12.92 if c<=.04045 else ((c+.055)/1.055)**2.4

def srgb(c):
    return 12.92*c if c<=.0031308 else 1.055*c**(1/2.4)-.055

def kernel(s):
    if not s:return [(0,1)]
    r=math.ceil(3*s);raw=[(k,math.exp(-k*k/(2*s*s)))for k in range(-r,r+1)];total=sum(v for _,v in raw)
    return [(k,v/total)for k,v in raw]

unit=Unit('filter-inner-shadow-arithmetic',60,40,
          'Literal PNG inputs independently check a shifted small Gaussian, '
          'negative arithmetic alpha, white colour matrix and linearRGB merge. '
          'Cases cover zero blur, fractional and negative offsets, unequal '
          'deviations, translucent input and group opacity.',ground=False)
for i,(name,dx,dy,sx,sy,opacity)in enumerate([
        ('identity',0,0,0,0,1),('positive',1,1,.5,.5,1),
        ('fractional',.25,.5,.5,.5,1),('negative',-.25,-.5,.5,.5,1),
        ('unequal',.25,.5,.25,1.25,1),('group-half',.25,.5,.5,.5,.5)]):
    source=[]
    for y in range(16):
        row=[]
        for x in range(16):
            alpha=([255,128,64][(x+y)%3]/255 if 2<=x<=13 and 2<=y<=13 else 0)
            row.append((206/255,16/255,16/255,alpha))
        source.append(row)
    def alpha(x,y):return source[y][x][3]if 0<=x<16 and 0<=y<16 else 0
    shifted=[]
    for y in range(16):
        row=[]
        for x in range(16):
            xx,yy=x-dx,y-dy;ix,iy=math.floor(xx),math.floor(yy);fx,fy=xx-ix,yy-iy
            row.append(sum(alpha(ix+a,iy+b)*(fx if a else 1-fx)*(fy if b else 1-fy)for a in [0,1]for b in [0,1]))
        shifted.append(row)
    expected=[]
    for y in range(16):
        row=[]
        for x in range(16):
            blurred=sum(shifted[y+b][x+a]*u*v for a,u in kernel(sx)for b,v in kernel(sy)if 0<=x+a<16 and 0<=y+b<16)
            p=source[y][x];white=.35*max(0,p[3]-blurred);out_alpha=white+p[3]*(1-white)
            rgb=[srgb((white+linear(c)*p[3]*(1-white))/out_alpha)if out_alpha else 0 for c in p[:3]]
            row.append(tuple(rgb)+(out_alpha*opacity,))
        expected.append(row)
    x,y=i%3*20+2,i//3*20+2
    unit.cell(name,(x,y,16,16),
              f'<svg x="{x}" y="{y}" width="16" height="16"><defs>'
              f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="0" y="0" width="16" height="16">'
              f'<feOffset in="SourceAlpha" dx="{dx}" dy="{dy}"/>'
              f'<feGaussianBlur stdDeviation="{sx} {sy}"/>'
              '<feComposite operator="arithmetic" in2="SourceAlpha" k2="-1" k3="1"/>'
              '<feColorMatrix values="0 0 0 0 1 0 0 0 0 1 0 0 0 0 1 0 0 0 .35 0" result="white"/>'
              '<feMerge><feMergeNode in="SourceGraphic"/><feMergeNode in="white"/></feMerge>'
              f'</filter></defs><g opacity="{opacity}"><image width="16" height="16" filter="url(#f{i})" href="{png(source)}"/></g></svg>',
              f'<image x="{x}" y="{y}" width="16" height="16" href="{png(expected)}"/>')
unit.write()
