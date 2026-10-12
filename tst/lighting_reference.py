"""Flatten reviewed Y reflections of a solid path and its distant lighting.

Skia maps lighting Z through the average of the transformed (Z,Z) vector,
which vanishes under a Y reflection. Reflect the path coordinates and light
azimuth instead, leaving only translations on its use instances. The other
supported primitives commute with reflection; no intermediate raster is used.
"""
import copy
import re
import xml.etree.ElementTree as ET

import noise_reference

NS = noise_reference.NS
HREF = '{http://www.w3.org/1999/xlink}href'
NUMBER = r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?'


def reflected_path(data):
    token = re.compile(r'[a-df-zA-DF-Z]|' + NUMBER)
    if token.sub('', data).strip(' \t\r\n,'):
        raise ValueError('unsupported path syntax')
    output, command, count = [], '', 0

    def complete():
        return (command.lower() in ['m', 'l'] and count >= 2 and count % 2 == 0
                or command.lower() == 'c' and count >= 6 and count % 6 == 0
                or command.lower() == 'z' and count == 0)

    for item in token.findall(data):
        if item.isalpha():
            if item.lower() not in ['m', 'l', 'c', 'z'] or command and not complete():
                raise ValueError('lighting reflection supports only moves, lines, cubics and closes')
            command, count = item, 0
        else:
            if not command or command.lower() == 'z':
                raise ValueError('path coordinate has no command')
            if count % 2:
                item = item[1:] if item.startswith('-') else '-' + item.lstrip('+')
            count += 1
        output.append(item)
    if not complete():
        raise ValueError('incomplete path command')
    return ' '.join(output)


def source(data):
    root = ET.fromstring(data)
    paths, filters = list(root.iter(NS + 'path')), list(root.iter(NS + 'filter'))
    defs = root.find(NS + 'defs')
    if len(paths) != 1 or len(filters) != 1 or defs is None or any(n.tag == NS + 'style' for n in root.iter()):
        raise ValueError('lighting reflection requires one path, one filter and no stylesheets')
    path, f = paths[0], filters[0]
    if (set(path.attrib) - {'id', 'd', 'fill', 'filter'} or 'url(' in path.get('fill', '')
            or not path.get('id') or not path.get('fill') or path.get('filter') != 'url(#' + f.get('id', '') + ')'):
        raise ValueError('lighting reflection requires an explicitly filled, untransformed path')
    if (f.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
            or f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
            or tuple(f.get(k) for k in ['x', 'y', 'width', 'height']) != ('0', '0', '1', '1')):
        raise ValueError('lighting reflection requires a filter covering the object bounding box')
    allowed = ['feGaussianBlur', 'feComposite', 'feColorMatrix', 'feSpecularLighting']
    for p in f:
        if (p.tag not in [NS + name for name in allowed]
                or any(k in p.attrib for k in ['x', 'y', 'width', 'height'])
                or any(p.get(k) in ['BackgroundImage', 'BackgroundAlpha', 'FillPaint', 'StrokePaint'] for k in ['in', 'in2'])):
            raise ValueError('filter primitive does not support lighting reflection')
        if p.tag == NS + 'feSpecularLighting' and (len(p) != 1 or p[0].tag != NS + 'feDistantLight'):
            raise ValueError('lighting reflection requires a distant light')
    if not any(p.tag == NS + 'feSpecularLighting' for p in f):
        raise ValueError('no distant lighting to normalize')
    ids = {n.get('id') for n in root.iter()}

    def unique(name):
        while name in ids:
            name += '_'
        ids.add(name)
        return name

    mirrored, mf = copy.deepcopy(path), copy.deepcopy(f)
    mirrored.set('id', unique('canonicalReflectedPath'))
    mirrored.set('d', reflected_path(path.get('d', '')))
    mf.set('id', unique('canonicalReflectedFilter'))
    for n in mf.iter():
        if n is not mf:
            n.attrib.pop('id', None)
        if n.tag == NS + 'feDistantLight':
            n.set('azimuth', str(-float(n.get('azimuth', '0'))))
    mirrored.set('filter', 'url(#' + mf.get('id') + ')')
    changed = 0
    for n in root.iter():
        transform = n.get('transform', '')
        if not transform or re.fullmatch(r'translate\([^()]*\)', transform):
            continue
        if n.tag != NS + 'use' or not re.fullmatch(r'matrix\([^()]*\)', transform):
            raise ValueError('lighting reflection requires translated groups and reflected use instances')
        values = transform[7:-1]
        if re.sub(NUMBER, '', values).strip(' \t\r\n,'):
            raise ValueError('unsupported reflection matrix')
        matrix = [float(v) for v in re.findall(NUMBER, values)]
        if (len(matrix) != 6 or matrix[:4] != [1, 0, 0, -1] or n.get(HREF) != '#' + path.get('id')
                or n.get('x', '0') != '0' or n.get('y', '0') != '0'
                or set(n.attrib) - {'id', HREF, 'transform', 'x', 'y', 'width', 'height'}):
            raise ValueError('unsupported reflected path instance')
        n.set('transform', f'translate({matrix[4]} {matrix[5]})')
        n.set(HREF, '#' + mirrored.get('id'))
        changed += 1
    if not changed:
        raise ValueError('no reflected lighting instances')
    defs.extend([mf, mirrored])
    return ET.tostring(root)
