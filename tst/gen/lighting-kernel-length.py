"""Explicit lighting sample distance must not scale an affine bump's slope.

Literal PNG samples define A(x,y)=(32+4*x+2*y)/255. The specification's
interior Sobel factors give (2*dA/dx,2*dA/dy), independent of a positive
sampling distance. Expectations use that normal and the published Phong
formulas, not a second lighting filter. Further cells evaluate the nine
bilinear samples of a piecewise alpha plane with exact rational weights.
Crops exclude shape borders; large-distance cells exercise clamped taps.
"""
import math
from fractions import Fraction as F

from unitgen import Unit, png


unit = Unit('lighting-kernel-length', 240, 240,
            'Independent Phong values for an affine literal alpha plane. '
            'Positive kernelUnitLength changes sample positions without '
            'changing the surface slope. Check tiny, unequal and integral '
            'distances, invalid/default values, scale, both lighting '
            'primitives, flat surfaces and linearRGB. Ordinary tolerance.',
            ground=False)
cases = [
    ('diffuse-hundredth', 'diffuse', '.01', 1, 1, False, 8, False),
    ('diffuse-ten-thousandth', 'diffuse', '.0001', 1, 1, False, 8, False),
    ('diffuse-tiny', 'diffuse', '1e-8', 1, 1, False, 8, False),
    ('diffuse-below-double-ulp', 'diffuse', '1e-20', 1, 1, False, 8, False),
    ('diffuse-quarter', 'diffuse', '.25', 1, 1, False, 8, False),
    ('diffuse-unit', 'diffuse', '1', 1, 1, False, 8, False),
    ('diffuse-two', 'diffuse', '2', 1, 1, False, 8, False),
    ('diffuse-unequal-x', 'diffuse', '.01 2', 1, 1, False, 8, False),
    ('diffuse-unequal-y', 'diffuse', '2 .01', 1, 1, False, 8, False),
    ('diffuse-zero-invalid', 'diffuse', '0', 1, 1, False, 8, False),
    ('diffuse-negative-invalid', 'diffuse', '-1', 1, 1, False, 8, False),
    ('diffuse-default', 'diffuse', None, 1, 1, False, 8, False),
    ('specular-hundredth', 'specular', '.01', 1, 1, False, 8, False),
    ('specular-tiny', 'specular', '1e-8', 1, 1, False, 8, False),
    ('specular-below-double-ulp', 'specular', '1e-20', 1, 1, False, 8, False),
    ('specular-unequal', 'specular', '.01 2', 1, 1, False, 8, False),
    ('diffuse-half-scale', 'diffuse', '.01', .5, .5, False, 8, False),
    ('diffuse-double-scale', 'diffuse', '.01', 2, 2, False, 8, False),
    ('diffuse-unequal-scale', 'diffuse', '.01', 2, .5, False, 8, False),
    ('specular-half-scale', 'specular', '.01', .5, .5, False, 8, False),
    ('constant-alpha', 'diffuse', '.01', 1, 1, True, 8, False),
    ('zero-surface', 'specular', '.01', 1, 1, False, 0, False),
    ('diffuse-linearRGB', 'diffuse', '.01', 1, 1, False, 8, True),
    ('specular-linearRGB', 'specular', '.01', 1, 1, False, 8, True),
    ('below-pixel-spacing', 'diffuse', '.9999', 1, 1, False, 8, False),
    ('above-pixel-spacing', 'diffuse', '1.0001', 1, 1, False, 8, False),
    ('specular-two', 'specular', '2', 1, 1, False, 8, False),
    ('specular-zero-invalid', 'specular', '0', 1, 1, False, 8, False),
    ('mixed-zero-invalid', 'diffuse', '1 0', 1, 1, False, 8, False),
    ('mixed-negative-invalid', 'diffuse', '1 -1', 1, 1, False, 8, False),
    ('piecewise-small', 'diffuse', '.01', 1, 1, False, 8, False),
    ('piecewise-tiny', 'specular', '1e-20', 1, 1, False, 8, False),
    ('piecewise-unequal', 'diffuse', '.01 2', 1, 1, False, 8, False),
    ('piecewise-large', 'specular', '2.5 .5', 1, 1, False, 8, False),
    ('piecewise-clamped', 'diffuse', '16 8', 1, 1, False, 8, False),
    ('piecewise-distant-samples', 'specular', '1e20', 1, 1, False, 8, False),
]


def phong(gx, gy, kind, surface, linear):
    normal = (-surface*gx, -surface*gy, 1.)
    length = math.sqrt(sum(v*v for v in normal))
    normal = tuple(v/length for v in normal)
    azimuth, elevation = math.radians(225), math.radians(30)
    light = (math.cos(azimuth)*math.cos(elevation),
             math.sin(azimuth)*math.cos(elevation), math.sin(elevation))
    if kind == 'diffuse':
        value = max(0., min(1., sum(a*b for a, b in zip(normal, light))))
        if linear:
            value = 12.92*value if value <= .0031308 else 1.055*value**(1/2.4)-.055
        return value, value, value, 1.
    half = (light[0], light[1], light[2]+1)
    length = math.sqrt(sum(v*v for v in half))
    half = tuple(v/length for v in half)
    value = max(0., min(1., sum(a*b for a, b in zip(normal, half))))**8
    return 1., 1., 1., value


def sampled_normal(alpha, x, y, distance):
    # Evaluate all nine samples with exact rational bilinear weights.
    # This direct Sobel sum retains a step smaller than a floating-point
    # ulp, independently of the renderer's algebraic cancellation.
    values = list(map(F, distance.split()))
    dx, dy = values if len(values) == 2 else values*2

    def sample(xx, yy):
        xx, yy = min(F(19), max(F(0), xx)), min(F(19), max(F(0), yy))
        x0, y0 = math.floor(xx), math.floor(yy)
        x1, y1 = min(19, x0+1), min(19, y0+1)
        wx, wy = xx-x0, yy-y0
        return (alpha[y0][x0]*(1-wx)*(1-wy) + alpha[y0][x1]*wx*(1-wy)
                + alpha[y1][x0]*(1-wx)*wy + alpha[y1][x1]*wx*wy)

    samples = [[sample(F(x)+i*dx, F(y)+j*dy) for i in [-1, 0, 1]]
               for j in [-1, 0, 1]]
    gx = sum(weight*(samples[j][2]-samples[j][0])
             for j, weight in enumerate([1, 2, 1]))/(4*dx)
    gy = sum(weight*(samples[2][i]-samples[0][i])
             for i, weight in enumerate([1, 2, 1]))/(4*dy)
    return float(gx), float(gy)


for i, (name, kind, distance, sx, sy, constant, surface, linear) in enumerate(cases):
    alpha = [[128/255 if constant else (32+4*x+2*y)/255
              for x in range(20)] for y in range(20)]
    if name.startswith('piecewise-'):
        alpha = [[F(32 + (x*7+y*11+x*y*3) % 160, 255)
                  for x in range(20)] for y in range(20)]
    source = png([[(1., 1., 1., a) for a in row] for row in alpha])
    gx, gy = (0., 0.) if constant else (8/255, 4/255)
    exponent = '' if kind == 'diffuse' else ' specularExponent="8"'
    expected = phong(gx, gy, kind, surface, linear)
    pixels = [[expected for _ in range(int(8*sx))]
              for _ in range(int(8*sy))]
    if name.startswith('piecewise-'):
        pixels = [[phong(*sampled_normal(alpha, x, y, distance), kind, surface, linear)
                   for x in range(6, 14)] for y in range(6, 14)]
    reference = png(pixels)
    x, y = 40*(i % 6), 40*(i // 6)
    k = f' kernelUnitLength="{distance}"' if distance is not None else ''
    primitive = 'feDiffuseLighting' if kind == 'diffuse' else 'feSpecularLighting'
    space = 'linearRGB' if linear else 'sRGB'
    left, top = x+6*sx, y+6*sy
    width, height = 8*sx, 8*sy
    unit.cell(name, (left, top, width, height),
              f'<g transform="translate({x} {y}) scale({sx} {sy})">'
              f'<defs><filter id="f{i}" filterUnits="userSpaceOnUse" '
              'x="0" y="0" width="20" height="20" '
              f'color-interpolation-filters="{space}">'
              f'<{primitive} in="SourceAlpha" surfaceScale="{surface}"{k}{exponent}>'
              '<feDistantLight azimuth="225" elevation="30"/>'
              f'</{primitive}></filter>'
              f'<clipPath id="c{i}"><rect x="6" y="6" width="8" height="8"/>'
              '</clipPath></defs>'
              f'<g clip-path="url(#c{i})"><image width="20" height="20" '
              f'image-rendering="optimizeSpeed" filter="url(#f{i})" '
              f'href="{source}"/></g></g>',
              f'<image x="{left}" y="{top}" width="{width}" height="{height}" '
              f'image-rendering="optimizeSpeed" href="{reference}"/>')

unit.write()
