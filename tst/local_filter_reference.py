"""Render reviewed quarter-turn noise/lighting filters before rotating their PNGs.

Skia maps lighting Z through the average of the transformed (Z,Z) vector,
which can reverse or erase it under rotation. An unrotated local raster avoids
that problem. Its grid is aligned with the final device grid, so the rotation
is a pixel permutation. All rasters come from the external browser callback.
"""
import base64
import copy
import math
import re
import xml.etree.ElementTree as ET

import noise_reference
import svg11_reference

NS = noise_reference.NS
HREF = '{http://www.w3.org/1999/xlink}href'


def source(data, width, height, rasterize):
    root = ET.fromstring(data)
    if any(n.tag == NS + 'style' for n in root.iter()):
        raise ValueError('local filters do not resolve stylesheets')
    box = [float(v) for v in root.get('viewBox', '').split()]
    if len(box) != 4 or box[:2] != [0, 0] or min(box[2:]) <= 0:
        raise ValueError('local filters require a positive, zero-origin viewBox')
    scale = width / box[2]
    if not math.isclose(scale, height / box[3], rel_tol=1e-12):
        raise ValueError('local filters require uniform viewport scaling')
    filters = {f.get('id'): f for f in root.iter(NS + 'filter')}
    parents = {c: n for n in root.iter() for c in n}
    count = 0
    for node in list(root.iter()):
        props = noise_reference.properties(node)
        match = re.fullmatch(r'url\(#([^)]*)\)', props.get('filter', ''))
        f = filters.get(match[1]) if match else None
        if f is None or not any(n.tag == NS + 'feTurbulence' for n in f):
            continue
        if not any(n.tag in [NS + 'feDiffuseLighting', NS + 'feSpecularLighting'] for n in f):
            raise ValueError('unexpected noise filter without lighting')
        if (node.tag != NS + 'path' or 'url(' in props.get('fill', '')
                or any(props.get(k, v) != v for k, v in [('opacity', '1'), ('mask', 'none'),
                       ('clip-path', 'none'), ('stroke', 'none'), ('mix-blend-mode', 'normal'), ('isolation', 'auto')])):
            raise ValueError('local filter requires a plain filled path')
        rotation = re.fullmatch(r'rotate\(([^()]*)\)', node.get('transform', ''))
        values = [float(v) for v in re.split(r'[,\s]+', rotation[1].strip())] if rotation else []
        if len(values) != 3 or values[0] % 90:
            raise ValueError('local filter requires a quarter-turn rotation about a point')
        if 'transform' in noise_reference.properties(ET.Element('style', {'style': node.get('style', '')})):
            raise ValueError('local filter has a CSS transform')
        if any(n.get('href', n.get(HREF)) == '#' + node.get('id', '') for n in root.iter()):
            raise ValueError('local filter target is referenced')
        a, b, c, d = [(1, 0, 0, 1), (0, 1, -1, 0), (-1, 0, 0, -1), (0, -1, 1, 0)][int(values[0] / 90) % 4]
        cx, cy = values[1:]
        tx, ty = cx - a*cx - c*cy, cy - b*cx - d*cy
        corners = [(a*(x-tx) + b*(y-ty), c*(x-tx) + d*(y-ty))
                   for x in [0, box[2]] for y in [0, box[3]]]
        x0 = min(x for x, y in corners) - 4 / scale
        y0 = min(y for x, y in corners) - 4 / scale
        rw, rh = (width, height) if a else (height, width)
        rw, rh = rw + 8, rh + 8
        local = ET.Element(root.tag, root.attrib)
        local.set('width', str(rw))
        local.set('height', str(rh))
        local.set('viewBox', f'{x0} {y0} {rw/scale} {rh/scale}')
        for defs in root.findall(NS + 'defs'):
            local.append(copy.deepcopy(defs))
        definitions = {c: n for n in local.iter() for c in n}
        for other in list(local.iter(NS + 'filter')):
            if other.get('id') != f.get('id'):
                definitions[other].remove(other)
        chain, ancestor = [], parents.get(node)
        while ancestor is not None:
            p = noise_reference.properties(ancestor)
            if any(p.get(k, v) != v for k, v in [('transform', 'none'), ('opacity', '1'), ('mask', 'none'),
                   ('filter', 'none'), ('mix-blend-mode', 'normal'), ('isolation', 'auto')]):
                raise ValueError('local filter has a transformed or isolating ancestor')
            if ancestor is root and p.get('clip-path', 'none') != 'none':
                raise ValueError('local filter has a clipped root')
            if ancestor is not root:
                if ancestor.tag != NS + 'g':
                    raise ValueError('local filter has a non-group ancestor')
                chain.append(ancestor)
            ancestor = parents.get(ancestor)
        container = local
        for ancestor in reversed(chain):
            group = ET.SubElement(container, ancestor.tag, ancestor.attrib)
            group.attrib.pop('id', None)
            group.set('style', group.get('style', '') + ';clip-path:none')
            container = group
        painted = copy.deepcopy(node)
        painted.attrib.pop('transform')
        container.append(painted)
        png = rasterize(noise_reference.source(ET.tostring(local)), rw, rh)
        image = ET.Element(NS + 'image', {HREF: 'data:image/png;base64,' + base64.b64encode(png).decode(),
                           'x': str(x0), 'y': str(y0), 'width': str(rw/scale), 'height': str(rh/scale),
                           'transform': node.get('transform')})
        parent = parents[node]
        index = list(parent).index(node)
        parent.remove(node)
        parent.insert(index, image)
        count += 1
    if not count:
        raise ValueError('no local filters to rasterize')
    root = ET.fromstring(svg11_reference.backgrounds(ET.tostring(root)))
    for image in root.iter(NS + 'feImage'):
        uri = image.get(HREF, '')
        prefix = 'data:image/svg+xml;base64,'
        if uri.startswith(prefix) and image.get('result', '').startswith('canonicalBackground'):
            png = rasterize(base64.b64decode(uri[len(prefix):]), width, height)
            image.set(HREF, 'data:image/png;base64,' + base64.b64encode(png).decode())
    return ET.tostring(root)
