"""Pattern content at one texel per pixel preserves authored PNG samples.

Every period is an integer number of device pixels and contains that many
PNG pixels. Independent expectations repeat the source RGBA values, with
integer phase/reflection, without patterns, transforms or resampling.
Fractional user-space periods exercise reciprocal coordinate conversions;
they must not create a spurious non-unit image scale inside the tile.
"""
from unitgen import Unit, png

u = Unit('pattern-image-native-pixels', 128, 96,
         'PNG pixels repeated at their natural device size retain their '
         'samples. Fractional pattern lengths, both reciprocal ratios, '
         'viewBox, integer phase, reflection and unequal axes agree with '
         'explicit numeric pixel repetition. Whole and binary-exact scales '
         'are controls.', ground=False)
cases = [
    ('fractional', 12.5, 12.5, 10, 10, .8, False, 0, False),
    ('thirds', 9, 9, 6, 6, 2/3, False, 0, False),
    ('sevenths', 10, 10, 7, 7, .7, False, 0, False),
    ('whole', 12, 12, 12, 12, 1, False, 0, False),
    ('half', 10, 10, 5, 5, .5, False, 0, False),
    ('view-box', 12.5, 12.5, 10, 10, .8, True, 0, False),
    ('phase', 12.5, 12.5, 10, 10, .8, False, 1, False),
    ('reflection', 12.5, 12.5, 10, 10, .8, False, 0, True),
    ('unequal-axes', 12.5, 7.5, 10, 6, .8, False, 0, False),
]
for i,(name,w,h,nx,ny,scale,viewbox,phase,reflect) in enumerate(cases):
    x,y = i%4*32,i//4*32
    pixels = [[((column%3)/2,(line%3)/2,((column+line)%3)/2,
                (1,.5,.25)[(column+2*line)%3])
               for column in range(nx)] for line in range(ny)]
    data = png(pixels)
    extra = f' viewBox="0 0 {nx} {ny}"' if viewbox else ''
    iw,ih = (nx,ny) if viewbox else (w,h)
    transform = 'scale(-1 1)' if reflect else ''
    test = (f'<svg x="{x}" y="{y}" width="32" height="32" viewBox="0 0 {32/scale:.17g} {32/scale:.17g}">'
            f'<defs><pattern id="p{i}" patternUnits="userSpaceOnUse" width="{w}" height="{h}" '
            f'x="{phase/scale:.17g}" y="{2*phase/scale:.17g}" patternTransform="{transform}"{extra}>'
            f'<image width="{iw}" height="{ih}" href="{data}"/></pattern></defs>'
            f'<rect width="{32/scale:.17g}" height="{32/scale:.17g}" fill="url(#p{i})"/></svg>')
    expected = [[pixels[(line-2*phase)%ny][((-column-1 if reflect else column)-phase)%nx]
                 for column in range(32)] for line in range(32)]
    u.cell(name,(x,y,32,32),test,
           f'<image x="{x}" y="{y}" width="32" height="32" href="{png(expected)}"/>')
u.write()
