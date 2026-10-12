"""Float lighting for repeated paths inside fixed rectangular ancestor clips.

Expand plain path instances before evaluating their filters in each instance's
local frame. A user-space ancestor clip is independent of the replaced path's
bounding box: detach it for source isolation and restore it for composition.
"""
import re
import xml.etree.ElementTree as ET

import halfturn_reference
import layer_reference
from float_reference import number
from noise_reference import NS, properties


def prepare(data):
    original = ET.fromstring(data)
    paths, filters = list(original.iter(NS+'path')), list(original.iter(NS+'filter'))
    if len(paths) != 1 or len(filters) != 1:
        raise ValueError('emboss snapshots require one path and one shared filter')
    path, f = paths[0], filters[0]
    if (not path.get('id') or not f.get('id') or path.get('filter') != 'url(#'+f.get('id')+')'
            or set(path.attrib)-{'id', 'd', 'fill', 'filter'} or not re.fullmatch(r'#[0-9a-fA-F]{3}(?:[0-9a-fA-F]{3})?', path.get('fill', ''))
            or not any(n.tag == NS+'feSpecularLighting' for n in f)):
        raise ValueError('emboss snapshots require an explicitly filled, lit path')
    # Expansion is deliberately limited to direct instances of this path.
    for node in original.iter(NS+'use'):
        if node.get('href', node.get(halfturn_reference.HREF)) != '#'+path.get('id'):
            raise ValueError('emboss snapshots require direct path instances')
    root = ET.fromstring(halfturn_reference.expand(data))
    by_id = {n.get('id'): n for n in root.iter() if n.get('id')}
    clips = {}
    for node in root.iter():
        clip = properties(node).get('clip-path', 'none')
        if clip == 'none':
            continue
        match = re.fullmatch(r'url\(#([^)]*)\)', clip)
        cp = by_id.get(match[1]) if match else None
        if (node.tag != NS+'g' or clip != node.get('clip-path') or not node.get('id')
                or cp is None or cp.tag != NS+'clipPath' or cp.get('clipPathUnits') != 'userSpaceOnUse'
                or set(cp.attrib)-{'id', 'clipPathUnits'} or len(cp) != 1 or cp[0].tag != NS+'rect'):
            raise ValueError('emboss snapshot requires an explicit fixed ancestor clip')
        rect = cp[0]
        if set(rect.attrib)-{'id', 'x', 'y', 'width', 'height', 'fill', 'opacity'} or len(rect):
            raise ValueError('emboss ancestor clip must be a plain rectangle')
        number(rect, 'x', '0')
        number(rect, 'y', '0')
        if min(number(rect, 'width', '0'), number(rect, 'height', '0')) <= 0:
            raise ValueError('invalid emboss ancestor clip rectangle')
        clips[node.get('id')] = node.attrib.pop('clip-path')
    for index, node in enumerate(root.iter(NS+'path')):
        name = 'canonicalEmboss'+str(index)
        if name in by_id:
            raise ValueError('emboss snapshot identifier collision')
        node.set('id', name)
    return ET.tostring(root), clips


def source(data, width, height, measure, rasterize):
    prepared, clips = prepare(data)
    result = ET.fromstring(layer_reference.source(prepared, width, height, measure, rasterize,
                                                  affine=True, require_noise=False))
    for node in result.iter():
        if node.get('id') in clips:
            node.set('clip-path', clips[node.get('id')])
    return ET.tostring(result)


def probes():
    for phase in [0, .125, .375]:
        for a, d in [(1,1), (1,-1), (-1,1), (-1,-1)]:
            x, y = (48 if a == 1 else 96), (0 if d == 1 else 56)
            left, top, right, bottom = 16.25+phase, 16.5+phase, 80.25+phase, 41+phase
            # Specular alpha .5002 must survive before amplification to .2.
            data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="96" height="64" viewBox="0 0 96 64">'
                '<defs><filter id="f" x="0" y="0" width="1" height="1" color-interpolation-filters="sRGB">'
                '<feSpecularLighting surfaceScale="3" specularConstant=".5002" specularExponent="1">'
                '<feDistantLight elevation="90" azimuth="35"/></feSpecularLighting>'
                '<feColorMatrix values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1000 -500"/></filter>'
                f'<clipPath id="clip" clipPathUnits="userSpaceOnUse"><rect x="{left}" y="{top}" '
                f'width="{right-left}" height="{bottom-top}" opacity=".2" fill="red"/></clipPath></defs>'
                '<g id="clipped" clip-path="url(#clip)"><path id="shape" fill="#000" filter="url(#f)" d="M8 8h32v40H8z"/>'
                f'<use href="#shape" transform="matrix({a} 0 0 {d} {x} {y})"/></g></svg>').encode()
            yield f'{phase}/{a},{d}', data, (left, top, right, bottom)
