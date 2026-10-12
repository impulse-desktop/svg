"""Float paper lighting with the original clip, opacity and background context.

Only plain source rectangles/paths and fixed user-space clips are isolated.
Ancestor background blends retain their original vector bounding-box regions:
an embedded image's transparent padding must not enlarge those regions.
"""
import copy
import math
import re
import xml.etree.ElementTree as ET

import layer_reference
from float_reference import number
from noise_reference import NS, properties


def selected(f):
    return (any(n.tag == NS+'feTurbulence' for n in f)
            and any(n.tag in [NS+'feDiffuseLighting', NS+'feSpecularLighting'] for n in f))


def remove(node, keys):
    for key in keys:
        node.attrib.pop(key, None)
    style = [s for s in node.get('style', '').split(';') if ':' in s and s.split(':', 1)[0].strip() not in keys]
    node.attrib.pop('style', None)
    if style:
        node.set('style', ';'.join(style))


def source(data, width, height, measure, rasterize, verify_regions=None):
    root = ET.fromstring(data)
    parents = {c: n for n in root.iter() for c in n}
    by_id = {n.get('id'): n for n in root.iter() if n.get('id')}

    def reference(value, tag):
        match = re.fullmatch(r'url\(#([^)]*)\)', value)
        node = by_id.get(match[1]) if match else None
        if node is None or node.tag != NS+tag:
            raise ValueError('invalid paper '+tag+' reference')
        return node

    targets = []
    for node in root.iter():
        value = properties(node).get('filter', 'none')
        match = re.fullmatch(r'url\(#([^)]*)\)', value)
        f = by_id.get(match[1]) if match else None
        if f is not None and f.tag == NS+'filter' and selected(f):
            if not node.get('id'):
                raise ValueError('paper target has no identifier')
            targets.append(node)
    if not targets:
        return data
    ancestors = set()
    for node in targets:
        ancestor = parents[node]
        while ancestor is not root:
            if ancestor in targets or ancestor.tag != NS+'g':
                raise ValueError('paper sources require separate group ancestors')
            ancestors.add(ancestor)
            ancestor = parents[ancestor]
    contexts = {n for n in ancestors if any(properties(n).get(k, default) != default
                for k, default in [('filter', 'none'), ('clip-path', 'none'), ('opacity', '1')])}
    if any(not n.get('id') for n in contexts):
        raise ValueError('paper context has no identifier')
    measured = measure(data, [n.get('id') for n in targets+sorted(contexts, key=lambda n:n.get('id'))])
    clips, saved, regions = {}, {}, 0
    for node in targets+sorted(contexts, key=lambda n:n.get('id')):
        props = properties(node)
        if any('!' in v for v in props.values()):
            raise ValueError('paper context has a CSS override')
        clip = props.get('clip-path', 'none')
        if clip != 'none':
            cp = reference(clip, 'clipPath')
            if cp.get('clipPathUnits') != 'userSpaceOnUse':
                raise ValueError('paper clip must use a fixed user-space frame')
            if node in targets:
                clips[node.get('id')] = clip
        if node in contexts:
            opacity = number(ET.Element('paint', props), 'opacity', '1')
            if not 0 <= opacity <= 1:
                raise ValueError('invalid paper context opacity')
            value = props.get('filter', 'none')
            if value != 'none':
                f = reference(value, 'filter')
                allowed = {'id', 'filterUnits', 'primitiveUnits', 'x', 'y', 'width', 'height',
                           'style', 'color-interpolation-filters'}
                if (any(k not in allowed and not k.startswith('{') for k in f.attrib)
                        or set(properties(ET.Element('paint', {'style': f.get('style', '')})))-{'style', 'color-interpolation-filters'}
                        or f.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
                        or f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
                        or len(f) != 1 or f[0].tag != NS+'feBlend' or len(f[0])
                        or any(k not in ['id', 'in', 'in2', 'mode'] and not k.startswith('{') for k in f[0].attrib)
                        or f[0].get('in', 'SourceGraphic') != 'SourceGraphic'
                        or f[0].get('in2') != 'BackgroundImage' or f[0].get('mode') not in ['darken', 'multiply']):
                    raise ValueError('unsupported paper ancestor filter')
                bx, by, bw, bh = measured[node.get('id')]['bbox']
                if not all(math.isfinite(v) for v in [bx,by,bw,bh]) or min(bw,bh) <= 0:
                    raise ValueError('invalid paper ancestor bounding box')
                frozen = copy.deepcopy(f)
                name = 'canonicalPaperRegion'+str(len(saved))
                if name in by_id:
                    raise ValueError('paper region identifier collision')
                frozen.set('id', name)
                frozen[0].attrib.pop('id', None)
                frozen.set('filterUnits', 'userSpaceOnUse')
                for key, origin, extent, default in [('x',bx,bw,'-.1'), ('y',by,bh,'-.1'),
                                                   ('width',0,bw,'1.2'), ('height',0,bh,'1.2')]:
                    value = origin+extent*number(f, key, default)
                    if key in ['width','height'] and value <= 0:
                        raise ValueError('empty paper ancestor region')
                    frozen.set(key, str(value))
                parents[f].append(frozen)
                regions += 1
                remove(node, {'filter'})
                node.set('filter', 'url(#'+name+')')
            saved[node.get('id')] = dict(node.attrib)
            remove(node, {'filter', 'opacity', 'clip-path'})
        else:
            remove(node, {'clip-path'})

    def restore(tree):
        for node in tree.iter():
            name = node.get('id')
            if name in saved:
                node.attrib = dict(saved[name])
            if name in clips:
                node.set('clip-path', clips[name])
        return tree

    if regions:
        # Inkscape 1.2 can truncate an implicit BackgroundImage differently
        # from an explicit region, even without replacing the source. Require
        # an unchanged full-document export before making this conversion.
        frozen = ET.tostring(restore(copy.deepcopy(root)))
        if verify_regions is None or not verify_regions(data, frozen, width, height):
            raise ValueError('paper ancestor region changed the legacy composition')

    for node in targets:
        for rect in node.iter(NS+'rect'):
            style = properties(ET.Element('paint', {'style': rect.get('style', '')}))
            if len(rect) or any(k in style for k in ['x','y','width','height','rx','ry']):
                raise ValueError('paper rectangle has unsupported geometry')
            x, y = number(rect, 'x', '0'), number(rect, 'y', '0')
            w, h = number(rect, 'width', '0'), number(rect, 'height', '0')
            if min(w,h) <= 0 or number(rect, 'rx', '0') != 0 or number(rect, 'ry', '0') != 0:
                raise ValueError('paper rectangle must be plain and nonempty')
            rect.tag = NS+'path'
            for key in ['x','y','width','height','rx','ry']:
                rect.attrib.pop(key, None)
            rect.set('d', f'M{x} {y}h{w}v{h}h{-w}z')
    # All frames come from the original vector tree, before context detachment
    # and rectangle conversion. Transforms and source paints stay in place.
    result = ET.fromstring(layer_reference.source(ET.tostring(root), width, height,
                          lambda data, names: {name: measured[name] for name in names}, rasterize,
                          affine=True, composed=True, select=selected))
    return ET.tostring(restore(result))


def probes():
    """Compare flat colored lights with their closed-form vector replacement."""
    for phase, backdrop in [(0, 0), (.125, 0), (.375, 0), (0, .25), (.375, .25)]:
        for opacity in [1, .5]:
            for mode in ['darken', 'multiply']:
                for target_clip in [False, True]:
                    clip = 'clip-path="url(#clip)"'
                    data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="96" height="64" viewBox="0 0 96 64" enable-background="new">'
                        '<defs><filter id="f" x="-1" y="-1" width="3" height="3" color-interpolation-filters="sRGB">'
                        '<feTurbulence numOctaves="0"/><feDiffuseLighting lighting-color="#4080c0" diffuseConstant=".5" surfaceScale="0">'
                        '<feDistantLight elevation="90"/></feDiffuseLighting><feComposite operator="in" in2="SourceGraphic"/></filter>'
                        f'<filter id="bg" color-interpolation-filters="sRGB"><feBlend in2="BackgroundImage" mode="{mode}"/></filter>'
                        '<clipPath id="clip" clipPathUnits="userSpaceOnUse"><path d="M28 20h24v24H28z" '
                        f'transform="translate({phase} {phase})"/></clipPath></defs>'
                        f'<path fill="red" opacity="{backdrop}" d="M0 0h96v64H0z"/>'
                        f'<g id="context" filter="url(#bg)" opacity="{opacity}" {clip if not target_clip else ""}>'
                        f'<g id="target" filter="url(#f)" opacity=".75" {clip if target_clip else ""}>'
                        f'<rect x="24" y="16" width="32" height="32" transform="translate({phase} {phase})" fill="white"/>'
                        '</g></g></svg>').encode()
                    expected = ET.fromstring(data)
                    target = next(n for n in expected.iter() if n.get('id') == 'target')
                    target.attrib.pop('filter')
                    target[0].set('fill', '#204060')
                    yield f'{phase}/{backdrop}/{opacity}/{mode}/{target_clip}', data, ET.tostring(expected), not (backdrop and target_clip)
