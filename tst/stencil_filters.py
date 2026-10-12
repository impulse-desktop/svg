#!/usr/bin/env python3
"""Check discrete transfer precision and the scope of legacy noise snapshots."""
import copy
import xml.etree.ElementTree as ET

import numpy as np

import component_reference
import layer_reference
import stencil_reference
from affine_lighting_reference import image
from noise_reference import NS, properties


def main():
    component_reference.check()
    source = image(np.zeros((8, 8, 4), dtype=np.float32))
    node = ET.fromstring(f'<feComponentTransfer xmlns="{NS[1:-1]}">'+''.join(
        f'<feFunc{channel} type="discrete" tableValues="{value}"/>' for channel, value in zip('RGBA', [.2,.3,.4,1]))+'</feComponentTransfer>')
    import skia
    pixels = component_reference.discrete(node, source).toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)
    assert np.max(np.abs(pixels-[.2,.3,.4,1])) < 1e-6
    checked = 1
    for body in ['<feFuncA type="gamma"/>', '<feFuncA type="discrete" tableValues="nan"/>',
                 '<feFuncA type="discrete" tableValues="-1 1"/>', '<feFuncA type="discrete" tableValues="0 2"/>',
                 '<feFuncA/><feFuncA/>', '<feFuncX/>', '<feFuncA slope="2"/>', '<feFuncA><animate/></feFuncA>']:
        try:
            component_reference.discrete(ET.fromstring(f'<feComponentTransfer xmlns="{NS[1:-1]}">{body}</feComponentTransfer>'), source)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted unsupported transfer '+body)
    template = ET.fromstring(f'<svg xmlns="{NS[1:-1]}" viewBox="0 0 32 32"><defs>'
        '<style>.unused {fill:red;}</style><linearGradient id="paint"><stop offset="0" stop-color="#3771c8"/></linearGradient>'
        '<filter id="f" color-interpolation-filters="sRGB"><feTurbulence numOctaves="0"/>'
        '<feComponentTransfer><feFuncA type="discrete" tableValues="0 1"/></feComponentTransfer>'
        '<feColorMatrix in="SourceGraphic"/></filter>'
        '<filter id="untouched"><feTurbulence/></filter></defs>'
        '<g id="target" filter="url(#f)"><circle id="circle" cx="16" cy="16" r="8" fill="none" stroke="url(#paint)" '
        'stroke-width="2" transform="rotate(30 16 16)"/>'
        '<path fill="url(#paint)" opacity=".99" transform="translate(1 1)" d="M8 8h8v8H8z"/></g>'
        '<path id="other" filter="url(#untouched)" d="M0 0h4v4z"/></svg>')

    def measure(data, names):
        assert names and set(names) <= {'target', 'circle'}
        return {name: {'bbox': [8,8,16,16], 'matrix': [1,0,0,1,0,0]} for name in names}

    def rasterize(data, width, height):
        root = ET.fromstring(data)
        circle = next(root.iter(NS+'circle'))
        assert properties(circle)['stroke'] == '#3771c8' and circle.get('transform') == 'rotate(30 16 16)'
        assert next(root.iter(NS+'path')).get('opacity') == '.99'
        return bytes(image(np.ones((height, width, 4), dtype=np.float32)).encodeToData())

    result = ET.fromstring(stencil_reference.source(ET.tostring(template), 32, 32, measure, rasterize))
    assert len(list(result.iter(NS+'image'))) == 1
    assert next(n for n in result.iter() if n.get('id') == 'other').get('filter') == 'url(#untouched)'
    checked += 1
    for tag, key, value in [('style', 'text', 'path {fill:red;}'), ('g', 'class', 'unused'),
                            ('linearGradient', 'href', '#another'), ('stop', 'stop-opacity', '.5'),
                            ('stop', 'stop-color', 'currentColor'), ('circle', 'r', '-1'),
                            ('circle', 'transform', 'rotate(30 16)'), ('circle', 'transform', 'matrix(0 0 0 1 0 0)'),
                            ('circle', 'opacity', 'nan'), ('circle', 'filter', 'url(#f)'),
                            ('circle', 'style', 'r:10px'), ('path', 'clip-path', 'url(#clip)')]:
        root = copy.deepcopy(template)
        node = next(root.iter(NS+tag))
        if key == 'text':
            node.text = value
        else:
            node.set(key, value)
        try:
            stencil_reference.source(ET.tostring(root), 32, 32, measure, rasterize)
        except ValueError:
            checked += 1
        else:
            raise AssertionError(f'accepted unsupported stencil {tag}/{key}')
    root = copy.deepcopy(template)
    ET.SubElement(next(root.iter(NS+'linearGradient')), NS+'stop', {'offset': '1', 'stop-color': '#123456'})
    try:
        stencil_reference.prepare(ET.tostring(root))
    except ValueError:
        checked += 1
    else:
        raise AssertionError('accepted a nonconstant gradient')
    for empty in [False, True]:
        root = copy.deepcopy(template)
        f = next(n for n in root.iter(NS+'filter') if n.get('id') == 'untouched')
        f[:] = [ET.Element(NS+'feComposite', {'in': 'SourceGraphic', 'in2': 'BackgroundImage', 'operator': 'in'})]
        consumer = next(n for n in root.iter() if n.get('id') == 'other')
        if empty:
            consumer.tag = NS+'g'
            consumer.attrib.pop('d')
        try:
            stencil_reference.prepare(ET.tostring(root))
        except ValueError:
            assert not empty
        else:
            assert empty
        checked += 1
    print(f'checked {checked} stencil scope/transfer cases, plus 236 boundary samples and two sub-byte graph probes')


if __name__ == '__main__':
    main()
