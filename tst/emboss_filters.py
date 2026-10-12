#!/usr/bin/env python3
"""Check the restricted instances and clips used by float emboss references."""
import copy
import xml.etree.ElementTree as ET

import emboss_reference
from noise_reference import NS


def main():
    _, data, _ = next(emboss_reference.probes())
    template = ET.fromstring(data)
    prepared, clips = emboss_reference.prepare(data)
    root = ET.fromstring(prepared)
    assert not list(root.iter(NS+'use'))
    assert len({n.get('id') for n in root.iter(NS+'path')}) == 2
    assert clips == {'clipped': 'url(#clip)'}
    assert next(root.iter(NS+'rect')).get('opacity') == '.2'
    checked = 1
    for tag, key, value in [
        ('path', 'fill', 'currentColor'), ('path', 'style', 'fill:red'),
        ('path', 'transform', 'translate(1)'), ('path', 'filter', 'none'),
        ('use', 'href', '#clipped'), ('use', 'href', 'external.svg#shape'), ('use', 'x', '1'),
        ('g', 'clip-path', 'url(#missing)'), ('g', 'id', ''),
        ('g', 'style', 'clip-path:url(#clip)'),
        ('clipPath', 'clipPathUnits', 'objectBoundingBox'), ('clipPath', 'transform', 'translate(1)'),
        ('rect', 'rx', '1'), ('rect', 'transform', 'translate(1)'), ('rect', 'style', 'width:20px'),
        ('rect', 'width', '0'), ('rect', 'height', '-1'), ('rect', 'x', 'nan'), ('rect', 'y', '10%'),
    ]:
        root = copy.deepcopy(template)
        node = next(root.iter(NS+tag))
        node.set(key, value)
        if tag == 'g' and key == 'style':
            node.attrib.pop('clip-path')
        try:
            emboss_reference.prepare(ET.tostring(root))
        except ValueError:
            checked += 1
        else:
            raise AssertionError(f'accepted unsupported emboss {tag}/{key}')
    for tag in ['path', 'filter', 'rect', 'style']:
        root = copy.deepcopy(template)
        parent = next(root.iter(NS+'clipPath')) if tag == 'rect' else root
        ET.SubElement(parent, NS+tag)
        try:
            emboss_reference.prepare(ET.tostring(root))
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted extra emboss '+tag)
    print(f'checked {checked} emboss instance/clip scope cases')


if __name__ == '__main__':
    main()
