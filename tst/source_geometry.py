#!/usr/bin/env python3
"""Check source-snapshot restrictions and preservation of the filter frame."""
import base64
import math
import xml.etree.ElementTree as ET

from noise_reference import NS
from source_geometry_reference import HREF, source


def main():
    data = '''<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">
<defs><filter id="f" x="-10%" y="-20%" width="120%" height="140%" color-interpolation-filters="sRGB">
<feTurbulence result="n"/><feComposite in="SourceGraphic" in2="SourceAlpha" operator="in"/>
</filter></defs><g id="g" filter="url(#f)" transform="translate(-12.3 -7.7)">
<path fill="#ff0000" d="M12.3 7.7L44.3 7.7L44.3 39.7L12.3 39.7Z"/></g></svg>'''

    def measure(svg, name):
        assert name == 'g'
        group = next(ET.fromstring(svg).iter(NS + 'g'))
        assert group.get('style') == 'filter:none'
        return 12.3, 7.7, 32, 32

    def rasterize(svg, width, height):
        assert (width, height) == (62, 64)
        assert len(list(ET.fromstring(svg).iter(NS + 'path'))) == 1
        return b'independent raster bytes'

    converted = ET.fromstring(source(data.encode(), 31, 32, 2, measure, rasterize))
    f = next(converted.iter(NS + 'filter'))
    assert f.get('filterUnits') == 'userSpaceOnUse'
    for key, value in [('x', 8.6), ('y', .8), ('width', 38.4), ('height', 44.8)]:
        assert math.isclose(float(f.get(key)), value, abs_tol=1e-12)
    assert [p.tag for p in f] == [NS + 'feTurbulence', NS + 'feComposite']
    assert f[1].get('in') == 'SourceGraphic' and f[1].get('in2') == 'SourceAlpha'
    image = next(converted.iter(NS + 'image'))
    assert (float(image.get('x')), float(image.get('y'))) == (12.3, 7.7)
    assert base64.b64decode(image.get(HREF).split(',', 1)[1]) == b'independent raster bytes'
    assert not list(converted.iter(NS + 'path'))

    negative = [
        data.replace('width="32" height="32"', 'width="0" height="32"', 1),
        data.replace('width="32" height="32"', 'width="32" height="32" viewBox="0 0 32 32"', 1),
        data.replace('<svg ', '<svg opacity=".5" '),
        data.replace('<svg ', '<svg style="fill:blue" '),
        data.replace('<path ', '<path stroke="black" '),
        data.replace('<path ', '<path opacity=".5" '),
        data.replace('<path ', '<path fill-opacity=".5" '),
        data.replace('fill="#ff0000"', 'fill="url(#paint)"'),
        data.replace('<g id="g"', '<g id="g" mask="url(#m)"'),
        data.replace('<g id="g"', '<g id="g" opacity=".5"'),
        data.replace('translate(-12.3 -7.7)', 'matrix(2 0 0 2 0 0)'),
        data.replace('<defs>', '<style>path {fill:blue}</style><defs>'),
        data.replace('<defs>', '<use href="#g"/><defs>'),
        data.replace('<defs>', '<rect width="32" height="32"/><defs>'),
        data.replace('<defs>', '<foreignObject/><defs>'),
        data.replace('<defs>', '<animateTransform/><defs>'),
        '<?xml-stylesheet href="external.css"?>' + data,
        data.replace('<filter id="f"', '<filter id="f" primitiveUnits="objectBoundingBox"'),
        data.replace('<filter id="f"', '<filter id="f" filterUnits="userSpaceOnUse"'),
        data.replace('sRGB', 'linearRGB'),
    ]
    for index, bad in enumerate(negative):
        try:
            source(bad.encode(), 31, 32, 2, measure, rasterize)
        except ValueError:
            continue
        raise AssertionError(f'unsupported source snapshot {index} was accepted')
    try:
        source(data.encode(), 31, 32, 2, lambda *_: [12.3, 7.7, 31, 32], rasterize)
    except ValueError:
        pass
    else:
        raise AssertionError('clipped source snapshot was accepted')
    print(f'checked {len(negative) + 1} rejected source snapshots and preserved filter coordinates')


if __name__ == '__main__':
    main()
