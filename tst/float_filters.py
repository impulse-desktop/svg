#!/usr/bin/env python3
"""Independent flat-light and constant-displacement checks for float references."""
import math
from pathlib import Path
import tempfile

import canon
import float_reference
import source_geometry_reference


def normalized(primitives, scale=1, hole=False):
    path = 'M8 12H40V44H8Z' + ('M16 20H32V36H16Z' if hole else '')
    data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">'
            f'<defs><filter id="f" color-interpolation-filters="sRGB">'
            f'<feTurbulence numOctaves="0" result="unused"/>{primitives}</filter></defs>'
            f'<g id="g" transform="translate(-8 -12)" filter="url(#f)">'
            f'<path fill="white" fill-rule="evenodd" d="{path}"/></g></svg>').encode()

    def snapshot(data, w, h):
        rgba = bytes(v for y in range(h) for x in range(w)
                     for v in ([0, 0, 0, 0] if hole and 8*scale <= x < 24*scale and 8*scale <= y < 24*scale
                               else [255, 255, 255, 255]))
        with tempfile.TemporaryDirectory() as directory:
            png = Path(directory)/'source.png'
            canon.encode(png, w, h, rgba)
            return png.read_bytes()
    return source_geometry_reference.source(data, 32, 32, scale, lambda *_: [8, 12, 32, 32], snapshot)


def main():
    print(float_reference.check())
    checked = 0
    for kind in ['Diffuse', 'Specular']:
        for elevation in [15, 35, 65]:
            primitive = (f'<fe{kind}Lighting in="SourceGraphic" specularExponent="8">'
                         f'<feDistantLight azimuth="30" elevation="{elevation}"/></fe{kind}Lighting>')
            if kind == 'Diffuse':
                primitive = primitive.replace(' specularExponent="8"', '')
            lz = math.sin(math.radians(elevation))
            expected = 255*(lz if kind == 'Diffuse' else ((1+lz)/math.sqrt(2+2*lz))**8)
            channel = 0 if kind == 'Diffuse' else 3
            for scale in [1, 2]:
                rgba = float_reference.render(normalized(primitive, scale), 32*scale, 32*scale)
                value = rgba[(16*scale*32*scale+16*scale)*4+channel]
                assert abs(value-expected) <= 1, (kind, elevation, scale, value, expected)
                checked += 1
    for amount in [-4, 4]:
        primitive = (f'<feFlood flood-color="rgb(255,0,0)" result="map"/>'
                     f'<feDisplacementMap in="SourceGraphic" in2="map" scale="{amount}" '
                     f'xChannelSelector="R" yChannelSelector="G"/>')
        for scale in [1, 2]:
            width = 32*scale
            rgba = float_reference.render(normalized(primitive, scale, True), width, width)
            for y in range(width):
                for x in range(width):
                    sx, sy = x+amount*scale//2, y-amount*scale//2
                    inside = 0 <= sx < width and 0 <= sy < width
                    hole = 8*scale <= sx < 24*scale and 8*scale <= sy < 24*scale
                    expected = 255 if inside and not hole else 0
                    assert abs(rgba[(y*width+x)*4+3]-expected) <= 1, (amount, scale, x, y)
                    checked += 1
    valid = normalized('<feGaussianBlur in="SourceGraphic" stdDeviation="1"/>')
    negatives = [
        valid.replace(b'stdDeviation="1"', b'stdDeviation="-1"'),
        valid.replace(b'stdDeviation="1"', b'stdDeviation="nan"'),
        valid.replace(b'stdDeviation="1"', b'stdDeviation="1" x="0"'),
        valid.replace(b'stdDeviation="1"', b'stdDeviation="1" color-interpolation-filters="linearRGB"'),
        valid.replace(b'stdDeviation="1"', b'stdDeviation="1" kernelUnitLength="2"'),
        valid.replace(b'preserveAspectRatio="none"', b'preserveAspectRatio="xMidYMid"'),
        valid.replace(b'translate(0.5 0.5)', b'scale(2)'),
        valid.replace(b'filterUnits="userSpaceOnUse"', b'filterUnits="objectBoundingBox"'),
        normalized('<feSpecularLighting in="SourceGraphic"><fePointLight/></feSpecularLighting>'),
        normalized('<feConvolveMatrix kernelMatrix="1"/>'),
        valid.replace(b'numOctaves="0"', b'numOctaves="0" stitchTiles="stitch"'),
    ]
    for data in negatives:
        try:
            float_reference.render(data, 32, 32)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('unsupported float construction accepted')
    print(f'checked {checked} lighting/displacement samples and rejected constructions')


if __name__ == '__main__':
    main()
