"""Express a single blur of FillPaint/StrokePaint using ordinary SVG painting.

For reviewed simple test drawings only. Paint the plane past the blur support,
blur it, then clip the result to the original filter region. The target's geometry
sets objectBoundingBox paint coordinates but does not restrict the paint plane.
"""
import copy
import re
import xml.etree.ElementTree as ET

NS = '{http://www.w3.org/2000/svg}'


def properties(node):
    return node.attrib | {k.strip(): v.strip() for part in node.get('style', '').split(';') if ':' in part for k, v in [part.split(':', 1)]}


def bounds(node):
    p = properties(node)
    if 'transform' in p:
        raise ValueError('paint normalization does not resolve transformed bounds')
    tag = node.tag.removeprefix(NS)
    if tag == 'circle':
        x, y, r = (float(p.get(k, '0')) for k in ['cx', 'cy', 'r'])
        return x-r, y-r, x+r, y+r
    if tag == 'rect':
        x, y, w, h = (float(p.get(k, '0')) for k in ['x', 'y', 'width', 'height'])
        return x, y, x+w, y+h
    if tag == 'g':
        boxes = [b for child in node if (b := bounds(child)) is not None]
        return (min(b[0] for b in boxes), min(b[1] for b in boxes), max(b[2] for b in boxes), max(b[3] for b in boxes)) if boxes else None
    if tag in ['title', 'desc', 'metadata', 'defs']:
        return None
    raise ValueError('unsupported bounds element ' + tag)


def source(data):
    root = ET.fromstring(data)
    if any(n.tag == NS+'style' for n in root.iter()):
        raise ValueError('paint normalization does not resolve stylesheets')
    parents = {c: n for n in root.iter() for c in n}
    by_id = {n.get('id'): n for n in root.iter() if n.get('id')}
    filters = {n.get('id'): n for n in root.iter() if n.tag == NS+'filter' and len(n) == 1 and n[0].tag == NS+'feGaussianBlur' and n[0].get('in') in ['FillPaint', 'StrokePaint']}
    definitions = ET.SubElement(root, NS+'defs')
    viewbox = [float(v) for v in root.get('viewBox', '').split()]
    if len(viewbox) != 4:
        raise ValueError('paint normalization requires a viewBox')
    count = 0
    for node in list(root.iter()):
        p = properties(node)
        match = re.fullmatch(r'url\(#([^)]*)\)', p.get('filter', ''))
        if not match or match[1] not in filters:
            continue
        original = filters[match[1]]
        if any(k in original[0].attrib for k in ['x', 'y', 'width', 'height']) or original.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse':
            raise ValueError('paint normalization requires an unrestricted blur in user space')
        bbox = bounds(node)
        box_units = original.get('filterUnits', 'objectBoundingBox') == 'objectBoundingBox'
        if box_units and bbox is None:
            raise ValueError('objectBoundingBox filter without bounds')
        def length(key, axis, default):
            value = original.get(key, default)
            number = float(value.rstrip('%'))
            if value.endswith('%'):
                number *= .01 * (1 if box_units else viewbox[axis+2])
            return number
        x, y = length('x', 0, '-10%'), length('y', 1, '-10%')
        w, h = length('width', 0, '120%'), length('height', 1, '120%')
        if box_units:
            bw, bh = bbox[2]-bbox[0], bbox[3]-bbox[1]
            x, y, w, h = bbox[0]+x*bw, bbox[1]+y*bh, w*bw, h*bh
        chain, current = [], node
        while current is not None:
            chain.append(current)
            current = parents.get(current)
        inherited = {'fill': 'black', 'stroke': 'none', 'fill-opacity': '1', 'stroke-opacity': '1', 'color': 'black'}
        for ancestor in reversed(chain):
            inherited.update({k: v for k, v in properties(ancestor).items() if k in inherited})
        prop = 'fill' if original[0].get('in') == 'FillPaint' else 'stroke'
        paint = inherited[prop]
        prefix = f'canonicalPaint{count}'
        if prefix in by_id:
            raise ValueError('paint normalization ID collision')
        paint_match = re.fullmatch(r'url\(#([^)]*)\)', paint)
        if paint_match:
            server = by_id[paint_match[1]]
            if server.tag == NS+'linearGradient' and server.get('gradientUnits', 'objectBoundingBox') == 'objectBoundingBox':
                if bbox is None:
                    raise ValueError('gradient without bounds')
                gradient = copy.deepcopy(server)
                gradient.set('id', prefix+'Gradient')
                gradient.set('gradientUnits', 'userSpaceOnUse')
                for key, default in [('x1', '0'), ('y1', '0'), ('x2', '1'), ('y2', '0')]:
                    value = gradient.get(key, default)
                    gradient.set(key, str(float(value[:-1])*.01) if value.endswith('%') else value)
                gradient.set('gradientTransform', f'matrix({bbox[2]-bbox[0]} 0 0 {bbox[3]-bbox[1]} {bbox[0]} {bbox[1]}) '+gradient.get('gradientTransform', ''))
                definitions.append(gradient)
                paint = 'url(#'+prefix+'Gradient)'
            elif server.tag == NS+'pattern' and server.get('patternUnits') != 'userSpaceOnUse':
                raise ValueError('paint normalization requires a user-space pattern')
            elif server.tag not in [NS+'linearGradient', NS+'pattern']:
                raise ValueError('unsupported paint server')
        sigma = [float(v) for v in original[0].get('stdDeviation', '0').replace(',', ' ').split()]
        pad = 4*max(sigma)+2
        blur = copy.deepcopy(original)
        blur.attrib.update({'id': prefix+'Blur', 'filterUnits': 'userSpaceOnUse', 'x': str(x-pad), 'y': str(y-pad), 'width': str(w+2*pad), 'height': str(h+2*pad)})
        blur[0].set('in', 'SourceGraphic')
        definitions.append(blur)
        clip = ET.SubElement(definitions, NS+'clipPath', {'id': prefix+'Clip', 'clipPathUnits': 'userSpaceOnUse'})
        ET.SubElement(clip, NS+'rect', {'x': str(x), 'y': str(y), 'width': str(w), 'height': str(h)})
        wrapper = ET.Element(NS+'g', {'clip-path': 'url(#'+prefix+'Clip)', 'opacity': p.get('opacity', '1')})
        ET.SubElement(wrapper, NS+'rect', {'x': str(x-pad), 'y': str(y-pad), 'width': str(w+2*pad), 'height': str(h+2*pad), 'fill': paint, 'fill-opacity': inherited[prop+'-opacity'], 'stroke': 'none', 'color': inherited['color'], 'filter': 'url(#'+prefix+'Blur)'})
        parent = parents[node]
        index = list(parent).index(node)
        parent.remove(node)
        parent.insert(index, wrapper)
        count += 1
    if not count:
        raise ValueError('no paint-only blur found')
    return ET.tostring(root)


def backgrounds(data):
    """Make backdrop snapshots explicit for drawings with untransformed ancestors."""
    import base64
    root = ET.fromstring(data)
    if any(n.tag == NS+'style' for n in root.iter()):
        raise ValueError('background normalization does not resolve stylesheets')
    parents = {c: n for n in root.iter() for c in n}
    effective = {}
    def walk(node, inherited='accumulate'):
        value = properties(node).get('enable-background', 'accumulate')
        effective[node] = inherited if value == 'inherit' else value
        for child in node:
            walk(child, effective[node])
    walk(root)
    filters = {n.get('id'): n for n in root.iter() if n.tag == NS+'filter' and any(v in ['BackgroundImage', 'BackgroundAlpha'] for c in n.iter() for k,v in c.attrib.items() if k in ['in','in2'])}
    consumers = {}
    for node in list(root.iter()):
        match = re.fullmatch(r'url\(#([^)]*)\)', properties(node).get('filter', ''))
        if match and match[1] in filters:
            consumers.setdefault(match[1], []).append(node)
    viewbox = [float(v) for v in root.get('viewBox', '').split()]
    if len(viewbox) != 4:
        raise ValueError('background normalization requires a viewBox')
    for count, (name, targets) in enumerate(consumers.items()):
        if len(targets) != 1:
            raise ValueError('background normalization requires a single filter consumer')
        target = targets[0]
        chain, node, scope = set(), target, None
        while node is not None:
            chain.add(node)
            p = properties(node)
            if p.get('transform', 'none') != 'none' or (node is not root and node.tag == NS+'svg'):
                raise ValueError('background normalization requires an untransformed filter frame')
            if scope is None and effective[node].split()[0] == 'new':
                scope = node
            node = parents.get(node)
        prefix = 'canonicalBackground'+str(count)
        original = filters[name]
        def trim(node, active=False):
            active = active or node is scope
            out = ET.Element(node.tag, node.attrib)
            # Active ancestors' effects are applied after their children finish.
            for key in ['filter','mask','clip-path','opacity']:
                out.attrib.pop(key, None)
            out.set('style', out.get('style', '')+';filter:none;mask:none;clip-path:none;opacity:1')
            for child in node:
                if child is target:
                    break
                if child in chain:
                    out.append(trim(child, active))
                    break
                if child.tag == NS+'defs' or active:
                    out.append(copy.deepcopy(child))
            return out
        if scope is not None and scope is not target:
            snapshot = trim(root)
            snapshot.set('width', str(viewbox[2]));snapshot.set('height', str(viewbox[3]))
            fields = effective[scope].split()
            if len(fields) > 1:
                if len(fields) != 5:
                    snapshot = ET.Element(root.tag, root.attrib)
                else:
                    x,y,w,h = map(float, fields[1:])
                    if w <= 0 or h <= 0:
                        snapshot = ET.Element(root.tag, root.attrib)
                    else:
                        defs = ET.SubElement(snapshot, NS+'defs')
                        clip = ET.SubElement(defs, NS+'clipPath', {'id':prefix+'Clip'})
                        ET.SubElement(clip, NS+'rect', dict(zip(['x','y','width','height'],map(str,[x,y,w,h]))))
                        group = ET.Element(NS+'g', {'clip-path':'url(#'+prefix+'Clip)'})
                        for child in list(snapshot):
                            if child is not defs:
                                snapshot.remove(child);group.append(child)
                        snapshot.append(group)
            uri = 'data:image/svg+xml;base64,'+base64.b64encode(ET.tostring(snapshot)).decode()
            image = ET.Element(NS+'feImage', {'{http://www.w3.org/1999/xlink}href':uri, 'x':str(viewbox[0]), 'y':str(viewbox[1]), 'width':str(viewbox[2]), 'height':str(viewbox[3]), 'color-interpolation-filters':'sRGB', 'result':prefix})
        else:
            image = ET.Element(NS+'feFlood', {'flood-opacity':'0', 'result':prefix})
        alpha = ET.Element(NS+'feColorMatrix', {'in':prefix, 'type':'matrix', 'values':'0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0', 'result':prefix+'Alpha'})
        first = original[0]
        first.attrib.setdefault('in', 'SourceGraphic')
        if first.tag in [NS+'feBlend', NS+'feComposite', NS+'feDisplacementMap']:
            first.attrib.setdefault('in2', 'SourceGraphic')
        if first.tag == NS+'feMerge':
            for child in first:
                child.attrib.setdefault('in', 'SourceGraphic')
        for child in original.iter():
            for key in ['in','in2']:
                if child.get(key) in ['BackgroundImage','BackgroundAlpha']:
                    child.set(key, prefix+('Alpha' if child.get(key)=='BackgroundAlpha' else ''))
        original.insert(0, alpha);original.insert(0, image)
    return ET.tostring(root)
