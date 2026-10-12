#!/usr/bin/env python3
"""Check anisotropic float graphs and the scope of affine source snapshots."""
import copy
import math
import xml.etree.ElementTree as ET

import numpy as np
import skia

import affine_lighting_reference as lights
import layer_reference as layers
from noise_reference import NS


def main():
    lights.check()
    y, x = np.indices((32, 32))
    pixels = np.stack([x/32, y/32, (x+y)/64, np.ones_like(x)], axis=2).astype(np.float32)
    image = lights.image(pixels)
    checked = 0
    for sx, sy in [(1.25, .75), (.75, 1.25), (.715628, .794)]:
        for alpha in [0, .25, 1]:
            values = [0,0,0,0,.25, 0,0,0,0,.75, 0,0,0,0,0, 0,0,0,0,alpha]
            channels = dict(zip('RGBA', [.25, .75, 0, alpha] if alpha else [0]*4))
            for xc in 'RGBA':
                for yc in 'RGBA':
                    # Avoid nearest-neighbor ties: backends may choose either
                    # adjacent sample at an exact half-pixel displacement.
                    f = ET.fromstring(f'<filter xmlns="{NS[1:-1]}"><feColorMatrix result="map" values="'+
                         ' '.join(map(str, values))+f'"/><feDisplacementMap in="SourceGraphic" in2="map" scale="7" '
                         f'xChannelSelector="{xc}" yChannelSelector="{yc}"/></filter>')
                    actual = layers.graph(f, image, sx, 0, 0, sy).toarray(
                        colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)
                    dx, dy = math.floor(7*sx*(channels[xc]-.5)+.5), math.floor(7*sy*(channels[yc]-.5)+.5)
                    expected = np.zeros_like(pixels)
                    valid = (x+dx >= 0) & (x+dx < 32) & (y+dy >= 0) & (y+dy < 32)
                    expected[valid] = pixels[(y+dy)[valid], (x+dx)[valid]]
                    if np.max(np.abs(actual-expected)) > 1e-6:
                        raise AssertionError(f'anisotropic displacement {sx}/{sy}/{alpha}/{xc}/{yc}')
                    checked += 1
        for octaves in [1, 5]:
            for kind, expected in [('fractalNoise', .5), ('turbulence', 0)]:
                f = ET.fromstring(f'<filter xmlns="{NS[1:-1]}"><feTurbulence type="{kind}" '
                                 f'baseFrequency="{2*sx} {2*sy}" seed="7" numOctaves="{octaves}"/></filter>')
                actual = layers.graph(f, image, sx, 0, 0, sy).toarray(
                    colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
                if np.max(np.abs(actual-expected)) > .001:
                    raise AssertionError('anisotropic integer-lattice noise phase')
                checked += 1
    template = ET.fromstring(f'<svg xmlns="{NS[1:-1]}" width="32" height="32" viewBox="0 0 32 32">'
                            '<defs><filter id="f" color-interpolation-filters="sRGB"><feTurbulence/>'
                            '<feColorMatrix in="SourceGraphic"/></filter></defs><g>'
                            '<path id="p" d="M0 0h32v32h-32z" fill="red" filter="url(#f)"/></g></svg>')

    def rasterize(data, width, height):
        pixels = np.ones((height, width, 4), dtype=np.float32)
        return bytes(lights.image(pixels).encodeToData())

    for matrix in [[1.25,0,0,.75,0,0], [0,-.95,-1.06,0,32,32], [-1,0,0,1,32,0], [1,.25,.125,1,0,0]]:
        def measure(data, names):
            return {name: {'bbox': [0,0,32,32], 'matrix': matrix} for name in names}
        result = layers.source(ET.tostring(template), 32, 32, measure, rasterize, affine=True)
        if len(list(ET.fromstring(result).iter(NS+'image'))) != 1:
            raise AssertionError('affine target was not replaced')
        checked += 1
    root = copy.deepcopy(template)
    ET.SubElement(root, NS+'ellipse', {'id': 'ellipse', 'cx': '16', 'cy': '16', 'rx': '8', 'ry': '12', 'filter': 'url(#f)'})
    result = ET.fromstring(layers.source(ET.tostring(root), 32, 32, measure, rasterize, affine=True))
    if len(list(result.iter(NS+'image'))) != 1 or len(list(result.iter(NS+'ellipse'))) != 1:
        raise AssertionError('changed the excluded ellipse coverage')
    checked += 1
    for matrix in [[0,0,0,0,0,0], [1,1,1,1,0,0], [1,0,0,1,math.nan,0], [math.inf,0,0,1,0,0]]:
        def measure(data, names):
            return {name: {'bbox': [0,0,32,32], 'matrix': matrix} for name in names}
        try:
            layers.source(ET.tostring(template), 32, 32, measure, rasterize, affine=True)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted singular or non-finite frame')
    node = ET.fromstring(f'<feDiffuseLighting xmlns="{NS[1:-1]}"><fePointLight/></feDiffuseLighting>')
    for tag, key, value in [('parent','lighting-color','red'), ('parent','diffuseConstant','-1'),
                            ('light','x','nan'), ('light','style','x:1'), ('light','elevation','50')]:
        bad = copy.deepcopy(node)
        (bad if tag == 'parent' else bad[0]).set(key, value)
        try:
            lights.lighting(bad, image, 1.25, .75, 0, 0)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted unsupported lighting material or source')
    print(f'checked {checked} affine graph and scope cases, plus 72 white and 540 colored lighting controls')


if __name__ == '__main__':
    main()
