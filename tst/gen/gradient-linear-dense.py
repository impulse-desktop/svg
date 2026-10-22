"""Area means of repeating gradients, worked out independently of sampling.

Opaque ramps average their endpoints. With straight color and alpha both
varying linearly, their product is quadratic: red-transparent to blue-opaque
has premultiplied mean (1/6, 0, 1/3, 1/2), not the endpoint mean.
"""
import math

from unitgen import Unit

BW = '<stop offset="0"/><stop offset="1" stop-color="white"/>'
RB = '<stop offset="0" stop-color="red" stop-opacity="0"/><stop offset="1" stop-color="blue"/>'
HARD = '<stop offset="0" stop-color="red"/><stop offset=".25" stop-color="red"/><stop offset=".25" stop-color="blue"/><stop offset="1" stop-color="blue"/>'
NARROW = '<stop offset="0"/><stop offset=".3"/><stop offset=".3" stop-color="white"/><stop offset=".315" stop-color="white"/><stop offset=".315"/><stop offset="1"/>'
TAILS = '<stop offset=".1" stop-color="red"/><stop offset=".6" stop-color="blue"/>'
ALPHA_HARD = '<stop offset="0" stop-color="red" stop-opacity=".25"/><stop offset=".5" stop-color="red" stop-opacity=".25"/><stop offset=".5" stop-color="blue" stop-opacity=".75"/><stop offset="1" stop-color="blue" stop-opacity=".75"/>'

# Integrals of the sRGB encoding curve f(t) and t*f(t), with the exact
# piecewise power antiderivatives. The runtime does not use these formulas.
pivot, power = .0031308, 1 / 2.4
mean = 6.46 * pivot ** 2 + 1.055 * (1 - pivot ** (power + 1)) / (power + 1) - .055 * (1 - pivot)
alpha_mean = 12.92 * pivot ** 3 / 3 + 1.055 * (1 - pivot ** (power + 2)) / (power + 2) - .055 * (1 - pivot ** 2) / 2


def gray(value):
    byte = math.floor(value * 255 + .5)
    return '#' + f'{byte:02x}' * 3


cases = [
    ("repeat", BW, {}, '#808080', 1),
    ("reflect", BW, {'spreadMethod': 'reflect'}, '#808080', 1),
    ("weighted-stops", HARD, {}, '#4000bf', 1),
    ("straight-alpha", RB, {}, '#5500aa', .5),
    ("linear-rgb", BW, {'color-interpolation': 'linearRGB'}, gray(mean), 1),
    ("narrow-band", NARROW, {}, '#040404', 1),
    ("constant-tails", TAILS, {}, '#5900a6', 1),
    ("phase", BW, {'gradientTransform': 'translate(.0007)'}, '#808080', 1),
    ("vertical", BW, {'gradientTransform': 'rotate(90)'}, '#808080', 1),
    ("sheared", BW, {'gradientTransform': 'matrix(1 0 4 1 0 0)'}, '#808080', 1),
    ("object-box", BW, {'gradientUnits': 'objectBoundingBox', 'x2': '.00006103515625'}, '#808080', 1),
    ("stroke", BW, {}, '#808080', 1),
    ("ordinary-repeat", BW, {'x2': '4'}, None, 1),
    ("pad-step", '<stop offset="0" stop-color="green"/><stop offset="1" stop-color="blue"/>', {'spreadMethod': 'pad', 'x1': '16', 'x2': '16.001953125'}, None, 1),
    ("equal-ends", '<stop offset="0" stop-color="red"/><stop offset="1" stop-color="blue"/>', {'x2': '0'}, 'blue', 1),
    ("one-stop", '<stop offset=".5" stop-color="gold"/>', {}, 'gold', 1),
    ("reflected-transform", BW, {'gradientTransform': 'scale(-1 1)'}, '#808080', 1),
    ("alpha-hard-stop", ALPHA_HARD, {}, '#4000bf', .5),
    ("linear-alpha", '<stop offset="0" stop-opacity="0"/><stop offset="1" stop-color="white"/>', {'color-interpolation': 'linearRGB'}, gray(2 * alpha_mean), .5),
    ("filter-fill-paint", BW, {}, '#808080', 1),
    ("threshold-repeat", HARD, {"x2": ".00390625"}, '#4000bf', 1),
    ("threshold-reflect", HARD, {"x2": ".00390625", "spreadMethod": "reflect"}, '#4000bf', 1),
]

unit = Unit("gradient-linear-dense", 128, 96,
            "A repeating linear gradient at the conservative threshold of 256 t units per pixel is drawn by its area mean. Test stop weights, alpha, color space and transforms, with ordinary repeat, pad and coincident-end guards. Expected drawings are literal solid regions; the ordinary repeat uses its pixel-centre values.", ground=False)
for i, (name, stops, extra, color, opacity) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 16
    attrs = dict(gradientUnits='userSpaceOnUse', x1='0', y1='0', x2='.001953125', y2='0', spreadMethod='repeat')
    attrs.update(extra)
    gradient = f'<linearGradient id="g{i}" ' + ' '.join(f'{k}="{v}"' for k, v in attrs.items()) + f'>{stops}</linearGradient>'
    filter_def = f'<filter id="f{i}" filterUnits="userSpaceOnUse" x="0" y="0" width="32" height="16" color-interpolation-filters="sRGB"><feComposite in="FillPaint" in2="SourceGraphic" operator="in"/></filter>' if name == 'filter-fill-paint' else ''
    shape = f'<path d="M0 8H32" fill="none" stroke="url(#g{i})" stroke-width="4"/>' if name == 'stroke' else f'<rect width="32" height="16" fill="url(#g{i})"' + (f' filter="url(#f{i})"' if filter_def else '') + '/>'
    if name == 'ordinary-repeat':
        expected = ''.join(f'<rect x="{j}" width="1" height="16" fill="{gray((j % 4 + .5) / 4)}"/>' for j in range(32))
    elif name == 'pad-step':
        expected = '<rect width="16" height="16" fill="green"/><rect x="16" width="16" height="16" fill="blue"/>'
    else:
        expected = f'<rect y="{6 if name == "stroke" else 0}" width="32" height="{4 if name == "stroke" else 16}" fill="{color}" fill-opacity="{opacity}"/>'
    unit.cell(name, (x, y, 32, 16),
              f'<g transform="translate({x} {y})"><defs>{gradient}{filter_def}</defs>{shape}</g>',
              f'<g transform="translate({x} {y})">{expected}</g>')
unit.write()
