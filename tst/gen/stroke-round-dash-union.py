"""Independent integrals of periodic capsule unions, without raster samples.

In a gap the union's height is twice sqrt(r*r-x*x), where x is the
distance to the closer dash endpoint. Integrating it gives each pixel's
area, including overlapping round caps. Opacity applies to this union.
The D1 allowance is derived from the area between capsules of radius r
and r-.025: an inscribed arc within the existing tolerance contains the
smaller capsules. It does not use observed renderer differences.
"""
import math
from unitgen import Unit, png


def integral(x, radius):
    if radius <= 0:
        return 0
    x = max(0, min(x, radius))
    return .5*(x*math.sqrt(max(0, radius*radius-x*x))
               + radius*radius*math.asin(x/radius))


def cumulative(x, radius, pitch, dash, cap):
    if radius <= 0:
        return 0
    if cap != 'round':
        length = min(pitch, dash+(2*radius if cap == 'square' else 0))
        x += radius if cap == 'square' else 0
        cycles = math.floor(x/pitch)
        return 2*radius*(cycles*length+min(x-cycles*pitch, length))
    gap = pitch-dash
    total = 2*radius*dash+4*integral(gap/2, radius)
    cycles = math.floor(x/pitch)
    position = x-cycles*pitch
    if position <= dash:
        part = 2*radius*position
    elif position-dash <= gap/2:
        part = 2*radius*dash+2*integral(position-dash, radius)
    else:
        part = 2*radius*dash+4*integral(gap/2, radius)-2*integral(pitch-position, radius)
    return cycles*total+part


u = Unit('stroke-round-dash-union', 128, 64,
         'A dashed stroke paints the union of its bands and caps once. '
         'Exact capsule-area integrals cover overlapping and separated '
         'round caps, zero-length dashes, both axes, reversal, phase, '
         'three forms of opacity, square/butt caps and zero width. '
         'Round-cap tolerance follows from the existing D1 .025px '
         'geometric allowance; expected pixels remain exact integrals.', ground=False)
# name, width, period, dash, cap, axis, phase, reverse, opacity, opacity property
cases = [
    ('half', .5, .25, .125, 'round', 'x', 0, False, 1, 'opacity'),
    ('quarter', .25, .25, .125, 'round', 'x', 0, False, 1, 'opacity'),
    ('scaled-wide', 2.6774*8/45, 16/45, 8/45, 'round', 'x', 0, False, 1, 'opacity'),
    ('scaled-thin', 1.5161*8/45, 16/45, 8/45, 'round', 'x', 0, False, 1, 'opacity'),
    ('vertical', .5, .25, .125, 'round', 'y', 0, False, 1, 'opacity'),
    ('reverse', .5, .25, .125, 'round', 'x', 0, True, 1, 'opacity'),
    ('phase', .5, .25, .125, 'round', 'x', .0625, False, 1, 'opacity'),
    ('dots', .25, .25, 0, 'round', 'x', 0, False, 1, 'opacity'),
    ('separated', .25, .5, .125, 'round', 'x', 0, False, 1, 'opacity'),
    ('opacity', .5, .25, .125, 'round', 'x', 0, False, .3, 'opacity'),
    ('group-opacity', .5, .25, .125, 'round', 'x', 0, False, .3, 'group'),
    ('stroke-opacity', .5, .25, .125, 'round', 'x', 0, False, .3, 'stroke-opacity'),
    ('square-overlap', .5, .25, .125, 'square', 'x', 0, False, 1, 'opacity'),
    ('square-separated', .125, .5, .125, 'square', 'x', 0, False, 1, 'opacity'),
    ('butt', .5, .25, .125, 'butt', 'x', 0, False, 1, 'opacity'),
    ('zero-width', 0, .25, .125, 'round', 'x', 0, False, 1, 'opacity'),
]
for i, (name, width, pitch, dash, cap, axis, phase, reverse, opacity, prop) in enumerate(cases):
    x, y = i % 4*32, i//4*16
    path = 'M4.5 -4V20' if axis == 'y' else ('M36 4.5H-4' if reverse else 'M-4 4.5H36')
    paint = f' {prop}="{opacity}"' if prop != 'group' else ''
    mark = (f'<path d="{path}" fill="none" stroke="black" stroke-width="{width:.17g}" '
            f'stroke-linecap="{cap}" stroke-dasharray="{dash:.17g} {pitch-dash:.17g}" '
            f'stroke-dashoffset="{phase:.17g}"{paint}/>')
    if prop == 'group':
        mark = f'<g opacity="{opacity}">{mark}</g>'
    test = f'<svg x="{x}" y="{y}" width="32" height="16">{mark}</svg>'
    def areas(radius):
        def value(k):
            a, b = (36-k-1+phase, 36-k+phase) if reverse else (k+4+phase, k+5+phase)
            return cumulative(b, radius, pitch, dash, cap)-cumulative(a, radius, pitch, dash, cap)
        return [value(k) for k in range(16 if axis == 'y' else 32)]
    exact = areas(width/2)
    pixels = [[(0,0,0,0)]*32 for _ in range(16)]
    for k, area in enumerate(exact):
        pixels[k if axis == 'y' else 4][4 if axis == 'y' else k] = (0,0,0,area*opacity)
    tolerance = None
    if cap == 'round' and width:
        inner = areas(max(0, width/2-.025))
        tolerance = math.ceil(255*opacity*max(a-b for a,b in zip(exact,inner)))+1
    u.cell(name, (x,y,32,16), test,
           f'<image x="{x}" y="{y}" width="32" height="16" href="{png(pixels)}"/>',
           tolerance=tolerance)
u.write()
