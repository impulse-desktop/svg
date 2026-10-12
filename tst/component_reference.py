"""Discrete SVG transfer functions on float channels, without byte lookup tables.

The intervals are defined in https://www.w3.org/TR/SVG11/filters.html#feComponentTransferElement.
"""
import math
import xml.etree.ElementTree as ET

import numpy as np
import skia

from affine_lighting_reference import image
from noise_reference import NS


def discrete(node, source):
    pixels = source.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
    result = pixels.copy()
    seen = set()
    for child in node:
        name = child.tag.removeprefix(NS+'feFunc')
        if (name not in ['R', 'G', 'B', 'A'] or name in seen or len(child)
                or any(k not in ['id', 'type', 'tableValues'] and not k.startswith('{') for k in child.attrib)):
            raise ValueError('unsupported or duplicate transfer function')
        seen.add(name)
        kind = child.get('type', 'identity')
        if kind == 'identity':
            continue
        if kind != 'discrete':
            raise ValueError('float component transfer requires identity or discrete functions')
        table = [float(v) for v in child.get('tableValues', '').replace(',', ' ').split()]
        if not all(math.isfinite(v) and 0 <= v <= 1 for v in table):
            raise ValueError('invalid discrete transfer table')
        if not table:
            continue
        channel = 'RGBA'.index(name)
        # Evaluate interval boundaries above float32 precision; C itself is F32.
        indices = np.minimum((np.clip(pixels[..., channel].astype(np.float64), 0, 1)*len(table)).astype(int), len(table)-1)
        result[..., channel] = np.asarray(table, dtype=np.float32)[indices]
    result[..., :3] *= result[..., 3, None]
    return image(result)


def check():
    from fractions import Fraction
    for count in [1, 3, 7, 10]:
        values = [np.float32(0), np.float32(1)]
        for i in range(1, count):
            value = np.float32(i/count)
            values += [np.nextafter(value, np.float32(0)), value, np.nextafter(value, np.float32(1))]
        values = np.array(values, dtype=np.float32)
        for channel in 'RGBA':
            pixels = np.ones((1, len(values), 4), dtype=np.float32)
            pixels[..., 'RGBA'.index(channel)] = values
            pixels[..., :3] *= pixels[..., 3, None]
            table = [i/max(1, count-1) for i in range(count)]
            node = ET.fromstring(f'<feComponentTransfer xmlns="{NS[1:-1]}"><feFunc{channel} type="discrete" tableValues="'+
                                 ' '.join(map(str, table))+'"/></feComponentTransfer>')
            actual = discrete(node, image(pixels)).toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
            boundaries = [Fraction(i, count) for i in range(1, count)]
            expected = [table[sum(Fraction(float(value)) >= boundary for boundary in boundaries)] for value in values]
            if np.max(np.abs(actual[0, :, 'RGBA'.index(channel)]-expected)) > 1e-6:
                raise ValueError('discrete float transfer changed a step boundary')
    # A sub-byte alpha boundary must remain distinguishable through the graph.
    from layer_reference import graph
    for alpha, expected in [(.285, 0), (.286, 1)]:
        f = ET.fromstring(f'<filter xmlns="{NS[1:-1]}"><feColorMatrix values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 {alpha} 0"/>'
                         '<feComponentTransfer><feFuncA type="discrete" tableValues="0 0 1 1 1 1 1"/></feComponentTransfer></filter>')
        actual = graph(f, image(np.ones((8, 8, 4), dtype=np.float32)), 1, 0, 0)
        if not np.all(actual.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)[..., 3] == expected):
            raise ValueError('float graph rounded the discrete threshold')
    return 'numpy '+np.__version__
