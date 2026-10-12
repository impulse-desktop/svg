"""Compensate Skia's extra half-user-unit turbulence phase in external references.

Translate the filter frame by (+.5,+.5) and its source geometry by (-.5,-.5).
The geometry stays in place while noise coordinates cancel Skia's extra offset
before sampling. Preserve post-filter effects outside that frame, and translate
explicit primitive/light positions with it. Only reviewed CANON inputs are used;
reject constructions whose coordinate semantics this conversion does not cover.
"""
import re
import xml.etree.ElementTree as ET

NS = '{http://www.w3.org/2000/svg}'


def properties(node):
    return node.attrib | {k.strip(): v.strip() for part in node.get('style', '').split(';') if ':' in part for k, v in [part.split(':', 1)]}


def background_blend(data):
    """Replace a single background multiply with CSS for reviewed opaque backdrops.

    With an opaque backdrop, multiplying SourceGraphic by BackgroundImage and
    then applying opacity gives the same composite as CSS multiply. CANON cases
    must establish that opacity independently; reject other filter operations,
    restricted regions and ancestors that would change the CSS backdrop.
    """
    root = ET.fromstring(data)
    if any(n.tag == NS + 'style' for n in root.iter()):
        raise ValueError('background blend normalization does not resolve stylesheets')
    parents = {c: n for n in root.iter() for c in n}
    filters = {}
    for f in root.iter(NS + 'filter'):
        if not any(n.get(k) in ['BackgroundImage', 'BackgroundAlpha'] for n in f.iter() for k in ['in', 'in2']):
            continue
        if (len(f) != 1 or f[0].tag != NS + 'feBlend' or f[0].get('mode') != 'multiply'
                or f[0].get('in', 'SourceGraphic') != 'SourceGraphic' or f[0].get('in2') != 'BackgroundImage'
                or properties(f).get('color-interpolation-filters') != 'sRGB'
                or any(k in n.attrib for n in [f, f[0]] for k in ['x', 'y', 'width', 'height', 'href', '{http://www.w3.org/1999/xlink}href'])):
            raise ValueError('background blend normalization requires an unrestricted sRGB source multiply')
        filters[f.get('id')] = f
    found = set()
    for n in root.iter():
        p = properties(n)
        match = re.fullmatch(r'url\(#([^)]*)\)', p.get('filter', ''))
        if not match or match[1] not in filters:
            continue
        if n is root or p.get('mix-blend-mode', 'normal') != 'normal':
            raise ValueError('background blend target already blends or is the root')
        parent, scope = parents.get(n), None
        while parent is not None:
            p = properties(parent)
            if any(p.get(k, default) != default for k, default in
                   [('opacity', '1'), ('filter', 'none'), ('mask', 'none'), ('mix-blend-mode', 'normal'), ('isolation', 'auto')]):
                raise ValueError('background blend requires non-isolating ancestors')
            if p.get('enable-background', 'accumulate') != 'accumulate':
                if parent is not root or p['enable-background'] != 'new':
                    raise ValueError('background blend requires an unrestricted root background')
                scope = parent
            parent = parents.get(parent)
        if scope is not root:
            raise ValueError('background blend has no root background')
        n.set('style', n.get('style', '') + ';filter:none;mix-blend-mode:multiply')
        found.add(match[1])
    if not found or found != filters.keys():
        raise ValueError('background blend did not resolve every changed filter')
    for f in filters.values():
        parents[f].remove(f)
    return ET.tostring(root)


def source(data):
    root = ET.fromstring(data)
    for n in root.iter(NS + 'style'):
        if re.sub(r'@font-face\s*\{[^}]*\}', '', n.text or '').strip():
            raise ValueError('stylesheets other than font declarations are not supported')
    if any(n.get(k) in ['BackgroundImage', 'BackgroundAlpha'] for n in root.iter() for k in ['in', 'in2']):
        raise ValueError('document contains SVG 1.1 background inputs')
    filters = {f.get('id'): f for f in root.iter(NS + 'filter') if any(n.tag == NS + 'feTurbulence' for n in f)}
    if not filters:
        raise ValueError('no turbulence filter')
    for f in filters.values():
        if f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse':
            raise ValueError('non-user-space primitives')
        if f.get('filterUnits') == 'userSpaceOnUse':
            raise ValueError('explicit user-space filter region')
        for n in f.iter():
            if n.tag == NS + 'feImage':
                raise ValueError('image input')
            if any(n.get(k) in ['FillPaint', 'StrokePaint', 'BackgroundImage', 'BackgroundAlpha'] for k in ['in', 'in2']):
                raise ValueError('SVG 1.1 background or paint input')
            if n.get('stitchTiles') == 'stitch':
                raise ValueError('stitched noise')
            if n is not f:
                for k in ['x', 'y']:
                    if k in n.attrib:
                        n.set(k, str(float(n.get(k)) - .5))
                    elif n.tag in [NS + 'fePointLight', NS + 'feSpotLight']:
                        n.set(k, '-.5')
                if n.tag == NS + 'feSpotLight':
                    for k in ['pointsAtX', 'pointsAtY']:
                        n.set(k, str(float(n.get(k, '0')) - .5))
    parents = {c: n for n in root.iter() for c in n}
    found = set()
    for n in list(root.iter()):
        p = properties(n)
        match = re.fullmatch(r'url\(#([^)]*)\)', p.get('filter', ''))
        if not match or match[1] not in filters:
            continue
        if n is root or n.tag in [NS + 'svg', NS + 'use', NS + 'image']:
            raise ValueError('viewport, use or image target')
        effects = [('opacity', '1'), ('clip-path', 'none'), ('mask', 'none'), ('mix-blend-mode', 'normal'), ('isolation', 'auto')]
        if any('!' in p.get(k, '') for k, _ in effects):
            raise ValueError('important target effect')
        style = properties(ET.Element('style', {'style': n.get('style', '')}))
        if any(k in style for k in ['transform', 'transform-origin', 'transform-box']):
            raise ValueError('CSS transform on target')
        if any(c.get('href', c.get('{http://www.w3.org/1999/xlink}href')) == '#' + n.get('id', '') for c in root.iter()):
            raise ValueError('referenced target')
        parent = parents[n]
        i = list(parent).index(n)
        outer = ET.Element(NS + 'g', {'transform': n.get('transform', '')})
        outer.set('style', ';'.join(k + ':' + p.get(k, default) for k, default in effects) + ';display:' + p.get('display', 'inline'))
        group = ET.SubElement(outer, NS + 'g', {'filter': match[0], 'transform': 'translate(.5 .5)'})
        n.attrib.pop('filter', None)
        n.set('style', n.get('style', '') + ';filter:none;' + ';'.join(k + ':' + default for k, default in effects))
        n.set('transform', 'translate(-.5 -.5)')
        parent.remove(n)
        group.append(n)
        parent.insert(i, outer)
        found.add(match[1])
    if not found:
        raise ValueError('no rendered filter target')
    return ET.tostring(root)
