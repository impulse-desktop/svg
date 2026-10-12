#!/usr/bin/env python3
"""Check float layer routing, displacement sampling and rejected snapshots."""
import copy
import xml.etree.ElementTree as ET

import numpy as np
import skia

import layer_reference as layers
from float_reference import info
from noise_reference import NS


def main():
    layers.check()
    pixels = np.zeros((32, 32, 4), dtype=np.float32)
    for y in range(32):
        for x in range(32):
            pixels[y, x] = [x/32, y/32, (x+y)/64, 1]
    image = skia.Image.fromarray(pixels, colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType,
                                colorSpace=skia.ColorSpace.MakeSRGB())
    checked = 0
    for xc in 'RGBA':
        for yc in 'RGBA':
            f = ET.fromstring(f'''<filter xmlns="http://www.w3.org/2000/svg">
              <feColorMatrix result="map" values="0 0 0 0 .25 0 0 0 0 .75 0 0 0 0 0 0 0 0 0 1"/>
              <feDisplacementMap in="SourceGraphic" in2="map" scale="8" xChannelSelector="{xc}" yChannelSelector="{yc}"/>
            </filter>''')
            result = layers.graph(f, image, 1, 0, 0)
            actual = result.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
            dx, dy = {'R': -2, 'G': 2, 'B': -4, 'A': 4}[xc], {'R': -2, 'G': 2, 'B': -4, 'A': 4}[yc]
            for y in range(32):
                for x in range(32):
                    expected = pixels[y+dy, x+dx] if 0 <= x+dx < 32 and 0 <= y+dy < 32 else np.zeros(4)
                    if np.max(np.abs(actual[y, x]-expected)) > 1e-6:
                        raise AssertionError(f'{xc}/{yc} displacement at {x},{y}: {actual[y,x]} != {expected}')
                    checked += 1
    template = ET.fromstring('''<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 32 32">
      <defs><filter id="f" color-interpolation-filters="sRGB"><feTurbulence/></filter></defs>
      <g id="g"><path id="p" d="M0 0h32v32h-32z" fill="red" filter="url(#f)"/></g>
    </svg>''')

    def measure(data, names):
        return {name: {'bbox': [0, 0, 32, 32], 'matrix': [1, 0, 0, 1, 0, 0]} for name in names}

    def rasterize(data, width, height):
        surface = skia.Surface.MakeRaster(info(width, height))
        surface.getCanvas().clear(skia.ColorRED)
        return bytes(surface.makeImageSnapshot().encodeToData())

    result = ET.fromstring(layers.source(ET.tostring(template), 32, 32, measure, rasterize))
    assert len(list(result.iter(NS+'image'))) == 1
    bad = [
        ('svg', 'viewBox', '1 1 32 32'), ('svg', 'transform', 'translate(1)'),
        ('svg', 'preserveAspectRatio', 'none'), ('svg', 'opacity', '.5'),
        ('g', 'filter', 'url(#f)'), ('g', 'style', 'transform:translate(1px)'),
        ('g', 'fill', 'url(#paint)'), ('g', 'opacity', '.5'), ('g', 'clip-path', 'url(#clip)'),
        ('path', 'fill', 'url(#paint)'), ('path', 'stroke-width', '5%'), ('path', 'stroke-dasharray', '1 2'),
        ('path', 'vector-effect', 'non-scaling-stroke'), ('path', 'marker-end', 'url(#marker)'),
        ('path', 'style', 'transform:scale(2)'), ('path', 'style', 'opacity:1!important'),
        ('filter', 'color-interpolation-filters', 'linearRGB'), ('filter', 'primitiveUnits', 'objectBoundingBox'),
        ('filter', 'filterUnits', 'userSpaceOnUse'), ('filter', 'width', '-1'),
        ('feTurbulence', 'stitchTiles', 'stitch'), ('feTurbulence', 'numOctaves', '11'),
        ('feTurbulence', 'baseFrequency', 'nan'), ('feTurbulence', 'x', '0'),
    ]
    for tag, key, value in bad:
        root = copy.deepcopy(template)
        next(root.iter(NS+tag)).set(key, value)
        try:
            layers.source(ET.tostring(root), 32, 32, measure, rasterize)
        except (ValueError, KeyError):
            checked += 1
        else:
            raise AssertionError(f'accepted unsupported {tag}/{key}={value}')
    for tag, attrs in [('style', {}), ('svg', {}), ('use', {'href': '#p'})]:
        root = copy.deepcopy(template)
        ET.SubElement(root, NS+tag, attrs)
        try:
            layers.source(ET.tostring(root), 32, 32, measure, rasterize)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted unsupported '+tag)
    for matrix in [[1.1, 0, 0, 1, 0, 0], [0, 1, -1, 0, 0, 0], [-1, 0, 0, 1, 0, 0]]:
        def transformed(data, names):
            return {name: {'bbox': [0, 0, 32, 32], 'matrix': matrix} for name in names}
        try:
            layers.source(ET.tostring(template), 32, 32, transformed, rasterize)
        except ValueError as error:
            assert str(error) == 'no eligible uniform layer'
            checked += 1
        else:
            raise AssertionError('accepted unsupported layer transform')
    print(f'checked {checked} layer displacement samples and rejected snapshots, plus float graph controls')


if __name__ == '__main__':
    main()
