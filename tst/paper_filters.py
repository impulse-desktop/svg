#!/usr/bin/env python3
"""Check the scope and preservation of paper snapshot contexts."""
import copy
import xml.etree.ElementTree as ET

import numpy as np

import paper_reference
from affine_lighting_reference import image
from noise_reference import NS, properties


def main():
    _, data, _, _ = next(paper_reference.probes())
    template = ET.fromstring(data)

    def measure(data, names):
        return {name: {'bbox': [24,16,32,32], 'matrix': [1,0,0,1,0,0]} for name in names}

    def rasterize(data, width, height):
        root = ET.fromstring(data)
        assert not list(root.iter(NS+'rect'))
        assert all(properties(n).get('clip-path', 'none') == 'none' for n in root.iter())
        assert all(properties(n).get('filter', 'none') == 'none' for n in root.iter())
        assert all(properties(n).get('opacity', '1') == '1' for n in root.iter())
        return bytes(image(np.ones((height, width, 4), dtype=np.float32)).encodeToData())

    checked = 0
    for _, data, _, _ in paper_reference.probes():
        original = ET.fromstring(data)
        result = ET.fromstring(paper_reference.source(data, 96, 64, measure, rasterize, lambda *args: True))
        ids = {n.get('id'): n for n in result.iter() if n.get('id')}
        before = {n.get('id'): n for n in original.iter() if n.get('id')}
        assert ids['target'].tag == NS+'image'
        for name in ['target', 'context']:
            for key, default in [('opacity','1'), ('clip-path','none')]:
                assert properties(ids[name]).get(key,default) == properties(before[name]).get(key,default)
        frozen = ids[properties(ids['context'])['filter'][5:-1]]
        assert frozen.get('filterUnits') == 'userSpaceOnUse'
        assert [float(frozen.get(k)) for k in ['x','y','width','height']] == [20.8,12.8,38.4,38.4]
        assert 'filterUnits' not in ids['bg'].attrib
        assert frozen[0].get('in2') == 'BackgroundImage'
        assert ET.tostring(ids['clip']) == ET.tostring(before['clip'])
        checked += 1
    mutations = [
        ('target','id',''), ('context','id',''), ('clip','clipPathUnits','objectBoundingBox'),
        ('context','opacity','nan'), ('context','opacity','1.1'), ('context','opacity','-.1'),
        ('context','mask','url(#clip)'), ('context','filter','url(#absent)'),
        ('context','filter','url(#f)'), ('context','style','opacity:.5 !important'),
        ('bg','primitiveUnits','objectBoundingBox'), ('bg','filterRes','16'),
        ('bg','style','x:0'),
        ('bg','width','0'), ('bg','height','-1'), ('bg','x','nan'), ('bg','x','10%'),
        ('rect','rx','1'), ('rect','ry','1'), ('rect','width','0'), ('rect','height','-1'),
        ('rect','x','nan'), ('rect','y','10%'), ('rect','style','width:32px'),
        ('rect','mask','url(#clip)'), ('rect','filter','url(#bg)'),
        ('blend','in','BackgroundImage'), ('blend','in2','SourceGraphic'), ('blend','mode','screen'),
        ('blend','x','0'), ('blend','style','mix-blend-mode:multiply'),
    ]
    for name, key, value in mutations:
        root = copy.deepcopy(template)
        node = next(root.iter(NS+{'rect':'rect', 'blend':'feBlend'}[name])) if name in ['rect','blend'] else next(n for n in root.iter() if n.get('id') == name)
        node.set(key, value)
        try:
            paper_reference.source(ET.tostring(root), 96, 64, measure, rasterize, lambda *args: True)
        except ValueError:
            checked += 1
        else:
            raise AssertionError(f'accepted unsupported paper {name}/{key}')
    for bbox in [[0,0,0,32], [0,0,32,-1], [0,0,float('nan'),32]]:
        def invalid(data, names):
            return {name: {'bbox': bbox, 'matrix': [1,0,0,1,0,0]} for name in names}
        try:
            paper_reference.source(data, 96, 64, invalid, rasterize)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted invalid paper ancestor bounds')
    for verify in [None, lambda *args: False]:
        try:
            paper_reference.source(data, 96, 64, measure, rasterize, verify)
        except ValueError as error:
            assert str(error) == 'paper ancestor region changed the legacy composition'
            checked += 1
        else:
            raise AssertionError('accepted an unverified paper region conversion')
    print(f'checked {checked} paper context and scope cases')


if __name__ == '__main__':
    main()
