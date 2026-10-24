"""Convolution preserves input alpha coverage when preserveAlpha is true.

Every source/filter edge has an exact rational position. Uniform colours
and duplicate edge sampling make the colour kernels independent of tap
position; preserveAlpha still copies the raw input alpha. A second input
already clipped by feFlood and an interior subregion exercise both sides
of the hard-edge rule. Bias multiplies the input alpha as in the formula.
"""
from fractions import Fraction as F

from unitgen import Unit, png


u = Unit('fe-convolve-alpha-edges', 48, 72,
         'Convolution keeps copied input alpha at fractional edges. Exact '
         'rectangle areas specify opaque, translucent and SourceAlpha '
         'inputs, asymmetric kernels, bias and a previous result. Ordinary '
         'convolution still clips its newly sampled pixels; an interior '
         'primitive subregion still clips preserved alpha.', ground=False)

kernel_y = ' '.join(['0'] * 22 + ['1', '0'])
kernel_x = ' '.join(['0'] * 15 + ['1'] + ['0'] * 8)
cases = [
    ('center-opaque', 'order="1" kernelMatrix="1" preserveAlpha="true"', F(1), 'green', ''),
    ('center-translucent', 'order="1" kernelMatrix="1" preserveAlpha="true"', F(1, 2), 'green', ''),
    ('asymmetric-y', f'order="3 8" kernelMatrix="{kernel_y}" targetY="7" preserveAlpha="true"', F(1), 'green', ''),
    ('asymmetric-x', f'order="8 3" kernelMatrix="{kernel_x}" targetX="7" preserveAlpha="true"', F(1, 2), 'green', ''),
    ('transparent-taps', f'order="3 8" kernelMatrix="{kernel_y}" targetY="7" preserveAlpha="true" edgeMode="none"', F(1), 'black', ''),
    ('source-alpha', 'order="1" kernelMatrix="1" preserveAlpha="true" in="SourceAlpha"', F(1, 2), 'black', ''),
    ('bias-with-alpha', 'order="1" kernelMatrix="0" bias=".5" preserveAlpha="true"', F(1, 2), 'bias', ''),
    ('previous-result', 'order="1" kernelMatrix="1" preserveAlpha="true"', F(1, 2), 'green', 'flood'),
    ('wrong-kernel-count', 'order="3" kernelMatrix="1"', F(1), 'green', ''),
    ('wrong-count-preserved', 'order="3" kernelMatrix="1" preserveAlpha="true"', F(1, 2), 'green', ''),
    ('false-center', 'order="1" kernelMatrix="1" preserveAlpha="false"', F(1, 2), 'green', ''),
    ('false-shifted', 'order="3 1" kernelMatrix="1 0 0" targetX="1" preserveAlpha="false"', F(1), 'green', ''),
    ('interior-subregion', 'order="1" kernelMatrix="1" preserveAlpha="true"', F(1), 'green', 'inside'),
    ('transparent-input', 'order="1" kernelMatrix="1" preserveAlpha="true"', F(0), 'green', ''),
    ('zero-colour-kernel', 'order="1" kernelMatrix="0" preserveAlpha="true"', F(1), 'black', ''),
    ('initial-false', 'order="1" kernelMatrix="1"', F(1, 2), 'green', ''),
    ('none-center', 'order="1" kernelMatrix="1" edgeMode="none"', F(1, 2), 'green', ''),
    ('none-negative-bias', 'order="1" kernelMatrix="1" bias="-.25" edgeMode="none"', F(1), 'negative-bias', ''),
    ('none-positive-bias', 'order="1" kernelMatrix="0" bias=".5" edgeMode="none"', F(1), 'positive-bias', ''),
    ('mirror-center', 'order="1" kernelMatrix="1" edgeMode="mirror"', F(1, 2), 'green', ''),
    ('wrap-center', 'order="1" kernelMatrix="1" edgeMode="wrap"', F(1, 2), 'green', ''),
    ('wrap-expanded', 'order="1" kernelMatrix="1" edgeMode="wrap" x="0" y="0" width="12" height="12"', F(1, 2), 'green', 'wrap-expanded'),
]


def coverage(pixel, low, high):
    return max(F(0), min(F(pixel + 1), high) - max(F(pixel), low))


for i, (name, attributes, opacity, colour, variant) in enumerate(cases):
    x, y = i % 4 * 12, i // 4 * 12
    lo_x, lo_y, hi_x, hi_y = map(F, ['2.25', '2.125', '7.75', '6.875'])
    region = 'x="0" y="0" width="1" height="1"'
    prefix = ''
    if variant == 'inside':
        lo_x, lo_y, hi_x, hi_y = map(F, ['3.25', '3.125', '6.75', '5.875'])
        region = 'filterUnits="userSpaceOnUse" x="3.25" y="3.125" width="3.5" height="2.75"'
    elif variant == 'flood':
        prefix = '<feFlood flood-color="lime" flood-opacity=".5"/>'
    if variant == 'wrap-expanded':
        region = 'filterUnits="userSpaceOnUse" x="0" y="0" width="12" height="12"'
        prefix = ('<feFlood x="2.25" y="2.125" width="5.5" height="4.75" '
                  'flood-color="lime" flood-opacity=".5"/>')
    source = (f'<svg x="{x}" y="{y}" width="12" height="12">'
              f'<defs><filter id="c{i}" {region} color-interpolation-filters="sRGB">'
              f'{prefix}<feConvolveMatrix {attributes}/></filter></defs>'
              f'<rect x="2.25" y="2.125" width="5.5" height="4.75" '
              f'fill="lime" fill-opacity="{float(opacity)}" filter="url(#c{i})"/></svg>')
    pixels = []
    for py in range(12):
        row = []
        for px in range(12):
            ix, iy = (2 + (px - 2) % 6, 2 + (py - 2) % 5) if variant == 'wrap-expanded' else (px, py)
            area = coverage(ix, lo_x, hi_x) * coverage(iy, lo_y, hi_y)
            a = area * opacity
            if colour == 'empty':
                a = F(0)
            if colour == 'negative-bias':
                a = max(F(0), a - F(1, 4))
            elif colour == 'positive-bias':
                a = area / 2
            rgb = ((F(0), F(1), F(0)) if colour in ['green', 'negative-bias']
                   else (a / 2,) * 3 if colour == 'bias'
                   else (F(1, 2),) * 3 if colour == 'positive-bias'
                   else (F(0),) * 3)
            row.append((*map(float, rgb), float(a)))
        pixels.append(row)
    u.cell(name, (x, y, 12, 12), source,
           f'<image x="{x}" y="{y}" width="12" height="12" href="{png(pixels)}"/>')
u.write()
