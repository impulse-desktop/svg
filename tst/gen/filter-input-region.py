"""The filter region clips inputs; a primitive subregion clips its result.

FilterPrimitiveSubRegion explicitly distinguishes these two clips. Integer
input boundaries avoid raster-edge ambiguity. Plain rectangles give exact
shift, chain and erosion expectations. Literal-pixel Gaussian/convolution
references first clip the input to the filter region, apply the operation,
then crop the result to its primitive subregion. No filtered drawing supplies
an expectation, and the ordinary tolerance is unchanged.
"""
import math
from unitgen import Unit, png

u = Unit('filter-input-region', 144, 120,
         'The filter region is a hard clip on primitive inputs, including '
         'SourceGraphic and earlier filters in a chain. The primitive '
         'subregion separately clips its result. Independent rectangles '
         'and literal-pixel Gaussian/convolution equations cover both clips, '
         'source alpha, backdrop isolation and explicit input references.', ground=False)
green, purple = '#20bf40', '#8030bc'
cases = []


def rect(x=8, y=8, width=8, height=8, fill=green, opacity=1):
    return (f'<rect x="{x}" y="{y}" width="{width}" height="{height}" '
            f'fill="{fill}" opacity="{opacity}"/>')


def add(name, primitive, expected='', source=None, opacity=1, before='', isolated=False):
    i = len(cases)
    source = rect(0, 0, 24, 24) if source is None else source
    content = (f'<defs><filter id="f{i}" filterUnits="userSpaceOnUse" '
               'x="8" y="8" width="8" height="8" color-interpolation-filters="sRGB">'
               f'{primitive}</filter></defs>'
               f'<g filter="url(#f{i})" opacity="{opacity}">{source}</g>')
    if isolated:content = '<g style="isolation:isolate">'+content+'</g>'
    cases.append((name, before+content, expected))


for name, dx, dy in [('right-empty', 8, 0), ('left-empty', -8, 0),
                     ('down-empty', 0, 8), ('up-empty', 0, -8)]:
    add(name, f'<feOffset dx="{dx}" dy="{dy}"/>')
add('right-partial', '<feOffset dx="3"/>', rect(11, 8, 5, 8))
add('fractional-shift', '<feOffset dx="3.25" dy="-2.5"/>', rect(11.25, 8, 4.75, 5.5))
add('source-alpha', '<feOffset in="SourceAlpha" dx="3"/>', rect(11, 8, 5, 8, 'black', .5),
    source=rect(0, 0, 24, 24, opacity=.5))
add('post-group-opacity', '<feOffset dx="3"/>', rect(11, 8, 5, 8, opacity=.5), opacity=.5)
add('outside-input-stays-empty', '<feOffset dx="8"/>', source=rect(0, 8, 4, 8))
add('off-viewport-input-stays-empty', '<feOffset dx="12"/>', source=rect(-4, 8, 4, 8))
add('erosion-input-edge', '<feMorphology operator="erode" radius="1"/>', rect(9, 9, 6, 6))
add('explicit-source-after-flood', '<feFlood flood-color="blue"/><feOffset in="SourceGraphic" dx="3"/>', rect(11, 8, 5, 8))
add('previous-result', f'<feFlood flood-color="{green}" result="small"/><feOffset in="small" dx="3"/>', rect(11, 8, 5, 8))
background = rect(0, 0, 24, 24)+rect(0, 0, 8, 24, purple)
add('background-image', '<feOffset in="BackgroundImage" dx="3"/>', background,
    before=background, source=rect(0, 0, 24, 24, 'none'))
add('background-alpha', '<feOffset in="BackgroundAlpha" dx="3"/>', background+rect(11, 8, 5, 8, 'black'),
    before=background, source=rect(0, 0, 24, 24, 'none'))
add('isolated-backdrop', '<feOffset in="BackgroundImage" dx="3"/>', background,
    before=background, source=rect(0, 0, 24, 24, 'none'), isolated=True)

for chained in [False, True]:
    i = len(cases)
    first = (f'<filter id="a{i}" filterUnits="userSpaceOnUse" x="16" y="8" width="8" height="8">'
             '<feFlood flood-color="red"/></filter>') if chained else ''
    second = (f'<filter id="b{i}" filterUnits="userSpaceOnUse" x="8" y="8" width="8" height="8">'
              f'<feFlood result="green" flood-color="{green}"/>'
              '<feOffset in="SourceGraphic" dx="-8" result="red"/>'
              '<feMerge><feMergeNode in="green"/><feMergeNode in="red"/></feMerge></filter>')
    filters = (f'url(#a{i}) ' if chained else '')+f'url(#b{i})'
    cases.append(('chain-input-clip' if chained else 'original-input-clip',
                  '<defs>'+first+second+'</defs><g filter="'+filters+'">'+rect(0, 0, 24, 24, 'red')+'</g>', rect()))


def kernel(sigma):
    if not sigma:return [(0, 1.)]
    radius = math.ceil(3*sigma)
    values = [(i, math.exp(-i*i/(2*sigma*sigma))) for i in range(-radius, radius+1)]
    total = sum(v for _, v in values)
    return [(i, v/total) for i, v in values]


for name, sx, sy, alpha_only, coloured, fractional, convolution in [
        ('blur-input-edge', 1., 1., False, False, False, False),
        ('blur-unequal', 1.5, .5, False, False, False, False),
        ('blur-source-alpha', 1., 1., True, False, False, False),
        ('blur-colours', 1.25, .75, False, True, False, False),
        ('primitive-result-clip', 1., 1., False, False, True, False),
        ('convolution-input-edge', 0., 0., False, False, False, True)]:
    colour = (32/255, 191/255, 64/255)
    pixels = [[((1., 0., 0., 1.) if x < 12 else (0., 0., 1., 1.)) if coloured else colour+(1.,)
               for x in range(24)] for y in range(24)]
    wx, wy = ([(k, 1/3) for k in [-1, 0, 1]], [(0, 1.)]) if convolution else (kernel(sx), kernel(sy))
    crop = (8.25, 8.5, 15.75, 15.75) if fractional else (8, 8, 16, 16)
    expected = []
    for y in range(24):
        line = []
        for x in range(24):
            values = [0., 0., 0., 0.]
            for dy, ay in wy:
                for dx, ax in wx:
                    xx, yy = x+dx, y+dy
                    if 8 <= xx < 16 and 8 <= yy < 16:
                        p = pixels[yy][xx]
                        if not alpha_only:
                            for c in range(3):values[c] += p[c]*p[3]*ax*ay
                        values[3] += p[3]*ax*ay
            coverage = max(0, min(x+1, crop[2])-max(x, crop[0]))*max(0, min(y+1, crop[3])-max(y, crop[1]))
            a = values[3]
            line.append(tuple(v/a if a else 0 for v in values[:3])+(a*coverage,))
        expected.append(line)
    primitive = ('<feConvolveMatrix order="3 1" kernelMatrix="1 1 1" divisor="3" edgeMode="none"'
                 if convolution else f'<feGaussianBlur stdDeviation="{sx} {sy}"')
    if fractional:primitive += ' x="8.25" y="8.5" width="7.5" height="7.25"'
    if alpha_only:primitive += ' in="SourceAlpha"'
    add(name, primitive+'/>', '<image width="24" height="24" href="'+png(expected)+'"/>',
        source='<image width="24" height="24" href="'+png(pixels)+'"/>')
for name, dy in [('inner-offset-quarter', .25), ('inner-offset-half', .5)]:
    add(name, f'<feOffset in="SourceAlpha" dy="{dy}" result="shifted"/>'
        '<feComposite in="shifted" in2="SourceAlpha" operator="arithmetic" k2="-1" k3="1"/>'
        '<feColorMatrix type="matrix" values="0 0 0 0 .5 0 0 0 0 .5 0 0 0 0 .5 0 0 0 1 0"/>',
        rect(8, 8, 8, 1, '#808080', dy))
assert len(cases) == 26
for i, (name, test, expected) in enumerate(cases):
    x, y = i % 6 * 24, i // 6 * 24
    u.cell(name, (x, y, 24, 24),
           f'<svg x="{x}" y="{y}" width="24" height="24">{test}</svg>',
           f'<g transform="translate({x} {y})">{expected}</g>')
u.write()
