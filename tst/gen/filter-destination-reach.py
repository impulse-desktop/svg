"""A clipped destination does not clip the inputs a filter reads.

The wide filter regions force separate filter layers. Their final results
are needed only in the small parent clip, but offsets, kernels, morphology,
displacement and tiles still need input beyond that destination. Plain
rectangles and a literal discrete Gaussian give independent expectations.
"""
import math

from unitgen import Unit, png


u = Unit('filter-destination-reach', 192, 240,
         'A filter drawn into a small clipped parent preserves its remote '
         'inputs. Wide regions, offsets, convolution, blur, morphology, '
         'displacement, tile, lighting, filter chains, fractional clipping, '
         'opacity and axis permutations have independent expectations.',
         ground=False)
green = '#20bf40'
cases = []


def rect(x=16, y=16, width=8, height=8, opacity=1):
    return (f'<rect x="{x}" y="{y}" width="{width}" height="{height}" '
            f'fill="{green}" opacity="{opacity}"/>')


def add(name, primitive, source=None, expected=None, transform='',
        opacity=1, clip=(16, 16, 8, 8), second=None, nested=False):
    i = len(cases)
    source = rect(0, 16) if source is None else source
    expected = rect() if expected is None else expected
    x, y, width, height = clip
    definitions = (f'<clipPath id="c{i}"><rect x="{x}" y="{y}" '
                   f'width="{width}" height="{height}"/></clipPath>')
    attributes = ('filterUnits="userSpaceOnUse" x="-64" y="-64" '
                  'width="192" height="192" '
                  'color-interpolation-filters="sRGB"')
    definitions += f'<filter id="f{i}" {attributes}>{primitive}</filter>'
    filters = f'url(#f{i})'
    if second is not None:
        definitions += f'<filter id="s{i}" {attributes}>{second}</filter>'
        filters += f' url(#s{i})'
    content = f'<g filter="{filters}">{source}</g>'
    if nested:
        definitions += f'<clipPath id="n{i}"><rect x="20" y="16" width="8" height="8"/></clipPath>'
        content = f'<g clip-path="url(#n{i})">{content}</g>'
    content = (f'<defs>{definitions}</defs><g transform="{transform}" '
               f'clip-path="url(#c{i})" opacity="{opacity}">{content}</g>')
    cases.append((name, content, expected))


add('offset-right', '<feOffset dx="16"/>')
add('offset-left', '<feOffset dx="-16"/>', rect(32, 16))
add('offset-down', '<feOffset dy="16"/>', rect(16, 0))
add('offset-up', '<feOffset dy="-16"/>', rect(16, 32))
add('offset-intermediate', '<feOffset dx="-12"/><feOffset dx="-12"/>', rect(40, 16))
add('convolve-remote-taps', '<feConvolveMatrix order="3 1" kernelMatrix="0 0 1" '
    'targetX="2" targetY="0" kernelUnitLength="4 1" edgeMode="none"/>', rect(8, 16))
add('convolve-wrap', f'<feFlood x="0" y="16" width="8" height="8" flood-color="{green}"/>'
    '<feConvolveMatrix x="16" y="16" width="8" height="8" order="3 1" '
    'kernelMatrix="1 0 0" targetX="0" targetY="0" kernelUnitLength="4 1" edgeMode="wrap"/>')
add('tile-remote-input', f'<feFlood x="0" y="16" width="4" height="8" flood-color="{green}"/>'
    '<feTile x="16" y="16" width="8" height="8"/>')
add('erosion-input-margin', '<feMorphology operator="erode" radius="4"/>', rect(12, 12, 16, 16))
add('displacement-input-margin', '<feFlood flood-color="black" result="map"/>'
    '<feDisplacementMap in="SourceGraphic" in2="map" scale="16" '
    'xChannelSelector="R" yChannelSelector="G"/>', rect(8, 0, 8, 40))
add('lighting-input-margin', '<feFlood result="height"/>'
    f'<feDiffuseLighting in="height" lighting-color="{green}" diffuseConstant="1">'
    '<feDistantLight azimuth="0" elevation="90"/></feDiffuseLighting>')
add('separate-filter-chain', '<feOffset dx="32"/>', second='<feOffset dx="-16"/>')
add('group-and-source-opacity', '<feOffset dx="16"/>', rect(0, 16, opacity=.5),
    rect(opacity=.25), opacity=.5)
add('fractional-destination', '<feOffset/>', rect(8, 8, 24, 24),
    rect(16.25, 16.5, 7.5, 7.25), clip=(16.25, 16.5, 7.5, 7.25))
add('nested-destination', '<feOffset dx="16"/>', rect(4, 16),
    rect(20, 16, 4, 8), nested=True)
add('quarter-turn-destination', '<feOffset dx="16"/>', transform='translate(40 0) rotate(90)')
add('reflected-destination', '<feOffset dx="16"/>', transform='translate(40 0) scale(-1 1)')

# Sample the normalized Gaussian of sigma=1 at integer tap distances. The
# source ends at x=16, so the destination's first sample sees taps -3..-1.
# There is no vertical blur; the source covers all rows needed by the clip.
weights = [(k, math.exp(-k*k/2)) for k in range(-3, 4)]
total = sum(v for k, v in weights)
alpha = [sum(v for k, v in weights if 8 <= 16+x+k < 16)/total for x in range(8)]
pixels = [[(32/255, 191/255, 64/255, a) for a in alpha] for _ in range(8)]
add('blur-input-margin', '<feGaussianBlur stdDeviation="1 0"/>', rect(8, 0, 8, 40),
    f'<image x="16" y="16" width="8" height="8" href="{png(pixels)}"/>')

assert len(cases) == 18
for i, (name, test, expected) in enumerate(cases):
    x, y = i % 4 * 48, i // 4 * 48
    u.cell(name, (x, y, 48, 48),
           f'<g transform="translate({x} {y})">{test}</g>',
           f'<g transform="translate({x} {y})">{expected}</g>')
u.write()
