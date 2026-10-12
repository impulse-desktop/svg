#!/usr/bin/env python3
"""Check the restricted source groups accepted by the float reference."""
import copy
import xml.etree.ElementTree as ET

import layer_reference as layers
from noise_reference import NS


def main():
    _, data, _ = next(layers.group_probes())
    template = ET.fromstring(data)
    target = next(n for n in template.iter(NS+'g') if n.get('id') == 'p')
    layers.validate_source(target, template)
    checked = 1
    for key, value in [('filter', 'url(#f)'), ('opacity', '.5'), ('transform', 'translate(1)'),
                       ('style', 'transform:scale(2)'), ('style', 'fill:red!important'),
                       ('style', 'd:path("M0 0L1 1")'),
                       ('fill', 'url(#paint)'), ('fill', 'currentColor'), ('stroke-width', '10%'),
                       ('mask', 'url(#mask)'), ('clip-path', 'url(#clip)'), ('marker-start', 'url(#marker)'),
                       ('stroke-dasharray', '1 2'), ('vector-effect', 'non-scaling-stroke'),
                       ('visibility', 'hidden'), ('display', 'none'), ('mix-blend-mode', 'screen')]:
        root = copy.deepcopy(template)
        target = next(n for n in root.iter(NS+'g') if n.get('id') == 'p')
        next(target.iter(NS+'path')).set(key, value)
        try:
            layers.validate_source(target, root)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted unsupported group child '+key)
    for tag in ['ellipse', 'rect', 'image', 'text', 'use', 'svg', 'style', 'animate']:
        root = copy.deepcopy(template)
        target = next(n for n in root.iter(NS+'g') if n.get('id') == 'p')
        ET.SubElement(target, NS+tag)
        try:
            layers.validate_source(target, root)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted unsupported group element '+tag)
    root = copy.deepcopy(template)
    target = next(n for n in root.iter(NS+'g') if n.get('id') == 'p')
    next(target.iter(NS+'path')).set('id', 'nested')
    ET.SubElement(root, NS+'use', {'href': '#nested'})
    try:
        layers.validate_source(target, root)
    except ValueError:
        checked += 1
    else:
        raise AssertionError('accepted an externally referenced descendant')
    for box in ['nan 0 96 64', '0 0 inf 64', '0 0 0 64']:
        root = copy.deepcopy(template)
        root.set('viewBox', box)
        try:
            layers.source(ET.tostring(root), 96, 64, None, None)
        except ValueError:
            checked += 1
        else:
            raise AssertionError('accepted invalid viewBox '+box)
    print(f'checked {checked} source group scope cases')


if __name__ == '__main__':
    main()
