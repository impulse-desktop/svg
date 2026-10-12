"""Compensate Skia's reversed lighting Z under uniform negative axis scales.

This is a browser-specific reference correction, not an equivalent SVG rewrite.
Expand the reviewed group instances so their lighting can be adjusted separately.
"""
import copy
import math
import re
import xml.etree.ElementTree as ET

from noise_reference import NS, properties

HREF = '{http://www.w3.org/1999/xlink}href'
IDENTITY = (1, 0, 0, 1, 0, 0)


def multiply(m, n):
    a, b, c, d, e, f = m
    A, B, C, D, E, F = n
    return (a*A + c*B, b*A + d*B, a*C + c*D, b*C + d*D, a*E + c*F + e, b*E + d*F + f)


def matrix(value):
    result = IDENTITY
    position = 0
    for match in re.finditer(r'(matrix|translate)\s*\(([^()]*)\)', value):
        if value[position:match.start()].strip(' ,\t\r\n'):
            raise ValueError('unsupported half-turn transform')
        values = [float(v) for v in re.split(r'[,\s]+', match[2].strip())]
        if not all(math.isfinite(v) for v in values):
            raise ValueError('non-finite half-turn transform')
        if match[1] == 'matrix' and len(values) == 6:
            transform = tuple(values)
        elif match[1] == 'translate' and len(values) in [1, 2]:
            transform = (1, 0, 0, 1, values[0], values[1] if len(values) == 2 else 0)
        else:
            raise ValueError('unsupported half-turn transform arguments')
        result = multiply(result, transform)
        position = match.end()
    if value[position:].strip():
        raise ValueError('unsupported half-turn transform')
    return result


def source(data):
    root = ET.fromstring(data)
    for node in root.iter():
        if node.tag in [NS + 'style', NS + 'image', NS + 'text', NS + 'symbol'] or (node is not root and node.tag == NS + 'svg'):
            raise ValueError('half-turn correction requires plain paths and group instances')
        style = properties(ET.Element('style', {'style': node.get('style', '')}))
        if any(k in style for k in ['transform', 'transform-origin', 'transform-box']):
            raise ValueError('half-turn correction does not resolve CSS transforms')
        if node.get('preserveAspectRatio') == 'none':
            raise ValueError('half-turn correction requires uniform viewport scaling')
    ids = {n.get('id'): n for n in root.iter() if n.get('id')}
    parents = {c: n for n in root.iter() for c in n}
    for node in list(root.iter(NS + 'use')):
        href = node.get('href', node.get(HREF, ''))
        target = ids.get(href[1:]) if href.startswith('#') else None
        if target is None or any(n.tag not in [NS + 'g', NS + 'path'] for n in target.iter()):
            raise ValueError('half-turn instances must reference only plain paths and groups')
        if node.get('x', '0') != '0' or node.get('y', '0') != '0':
            raise ValueError('half-turn instances require zero x and y')
        replacement = ET.Element(NS + 'g', {k: v for k, v in node.attrib.items()
                                           if k not in ['href', HREF, 'x', 'y', 'width', 'height', 'id']})
        target = copy.deepcopy(target)
        for child in target.iter():
            child.attrib.pop('id', None)
        replacement.append(target)
        parent = parents[node]
        index = list(parent).index(node)
        parent.remove(node)
        parent.insert(index, replacement)

    filters = {f.get('id'): f for f in root.iter(NS + 'filter')}
    defs = root.find(NS + 'defs')
    if defs is None:
        raise ValueError('half-turn correction requires filter definitions')
    count = 0

    def visit(node, outer):
        nonlocal count
        transform = multiply(outer, matrix(node.get('transform', '')))
        match = re.fullmatch(r'url\(#([^)]*)\)', properties(node).get('filter', ''))
        a, b, c, d, _, _ = transform
        # Skia decomposes non-axis transforms differently. Do not round their
        # small off-diagonal coefficients into the negative-scale case.
        if match and a < 0 and a == d and b == 0 and c == 0:
            original = filters.get(match[1])
            if original is None:
                raise ValueError('unresolved half-turn filter')
            if any(n.tag in [NS + 'feDiffuseLighting', NS + 'feSpecularLighting'] for n in original):
                clone = copy.deepcopy(original)
                for child in clone.iter():
                    child.attrib.pop('id', None)
                name = 'canonicalHalfTurnFilter' + str(count)
                if name in ids:
                    raise ValueError('half-turn filter id collision')
                clone.set('id', name)
                for primitive in clone:
                    if primitive.tag in [NS + 'feDiffuseLighting', NS + 'feSpecularLighting']:
                        if len(primitive) != 1 or primitive[0].tag != NS + 'feDistantLight':
                            raise ValueError('half-turn correction requires distant lights')
                        primitive.set('surfaceScale', str(-float(primitive.get('surfaceScale', '1'))))
                        light = primitive[0]
                        light.set('elevation', str(-float(light.get('elevation', '0'))))
                defs.append(clone)
                node.set('style', node.get('style', '') + ';filter:url(#' + name + ')')
                count += 1
        for child in list(node):
            if child.tag != NS + 'defs':
                visit(child, transform)

    visit(root, IDENTITY)
    if not count:
        raise ValueError('no negative uniform lighting frames')
    return ET.tostring(root)
