"""Use an independently rasterized SourceGraphic for a reviewed noise filter.

Only a single translated group with one solid filled path covering the page
is supported. Freeze its computed filter region and replace the path with a
device-aligned external snapshot, keeping the filter graph unchanged.
"""
import base64
import copy
import math
import re
import xml.etree.ElementTree as ET

from halfturn_reference import matrix
from noise_reference import NS, properties, source as noise_source

HREF = '{http://www.w3.org/1999/xlink}href'


def source(data, width, height, scale, measure, rasterize):
    root = ET.fromstring(data)
    if root.tag != NS + 'svg' or 'viewBox' in root.attrib or root.get('style'):
        raise ValueError('source snapshot requires an absolute-size root without a viewBox or CSS')
    if b'<?xml-stylesheet' in data:
        raise ValueError('source snapshot does not resolve external stylesheets')
    natural = [float(root.get(k, '0')) for k in ['width', 'height']]
    if not all(math.isfinite(v) and v > 0 for v in [*natural, width, height, scale]):
        raise ValueError('source snapshot requires positive viewport dimensions')
    if any(k not in ['width', 'height', 'id', 'version'] and not k.startswith('{') for k in root.attrib):
        raise ValueError('unsupported source snapshot root attribute')
    groups, paths, filters = [list(root.iter(NS + tag)) for tag in ['g', 'path', 'filter']]
    if len(groups) != 1 or len(paths) != 1 or len(filters) != 1:
        raise ValueError('source snapshot requires one group, one path and one filter')
    group, path, original_filter = groups[0], paths[0], filters[0]
    group_id = group.get('id')
    if group not in list(root) or list(group) != [path] or not group.get('id'):
        raise ValueError('source snapshot requires a directly rendered path group with an id')
    if any(n.tag.startswith(NS) and n.tag not in [NS + tag for tag in ['g', 'defs', 'metadata', 'title', 'desc']] for n in root):
        raise ValueError('source snapshot has additional root content')
    if any(n.tag in [NS + tag for tag in ['style', 'use', 'image', 'text', 'rect', 'circle', 'ellipse',
                                         'polygon', 'polyline', 'line', 'clipPath', 'mask', 'marker', 'animate',
                                         'animateMotion', 'animateTransform', 'set', 'script', 'foreignObject']]
           or (n is not root and n.tag == NS + 'svg') for n in root.iter()):
        raise ValueError('unsupported source snapshot document structure')
    gp, pp = properties(group), properties(path)
    if set(gp) - {'id', 'style', 'filter', 'transform'}:
        raise ValueError('source snapshot requires a plain filtered group')
    if set(pp) - {'id', 'style', 'd', 'fill', 'fill-rule', 'fill-opacity', 'stroke'}:
        raise ValueError('source snapshot requires a solid filled path')
    if (pp.get('stroke', 'none') != 'none' or pp.get('fill-opacity', '1') != '1'
            or pp.get('fill-rule', 'nonzero') not in ['nonzero', 'evenodd']
            or not re.fullmatch(r'#[0-9a-fA-F]{6}|black|red|white', pp.get('fill', 'black'))):
        raise ValueError('unsupported source snapshot paint')
    if (gp.get('filter') != 'url(#' + original_filter.get('id', '') + ')'
            or original_filter.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
            or original_filter.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
            or properties(original_filter).get('color-interpolation-filters') != 'sRGB'):
        raise ValueError('source snapshot requires an object-bounded sRGB filter in user space')
    a, b, c, d, tx, ty = matrix(group.get('transform', ''))
    if (a, b, c, d) != (1, 0, 0, 1):
        raise ValueError('source snapshot supports only translated filter frames')

    plain = copy.deepcopy(root)
    next(plain.iter(NS + 'g')).set('style', 'filter:none')
    plain_data = ET.tostring(plain)
    x, y, w, h = measure(plain_data, group.get('id'))
    if not all(math.isfinite(v) for v in [x, y, w, h]) or max(abs(v) for v in [x + tx, y + ty, w - natural[0], h - natural[1]]) > 1e-3:
        raise ValueError('source snapshot geometry must coincide with the page bounds')
    png = rasterize(plain_data, width * scale, height * scale)

    # Apply the established noise-frame correction before replacing geometry.
    root = ET.fromstring(noise_source(data))
    f = next(root.iter(NS + 'filter'))

    def fraction(key, default):
        value = original_filter.get(key, default)
        result = float(value[:-1]) / 100 if value.endswith('%') else float(value)
        if not math.isfinite(result):
            raise ValueError('non-finite source snapshot filter region')
        return result

    # The normalized filter frame translates its source by (-.5, -.5).
    # Its region must retain the vector path's bounds, not the PNG's page box.
    f.set('filterUnits', 'userSpaceOnUse')
    for key, value in [('x', x - .5 + fraction('x', '-.1') * w),
                       ('y', y - .5 + fraction('y', '-.1') * h),
                       ('width', fraction('width', '1.2') * w),
                       ('height', fraction('height', '1.2') * h)]:
        f.set(key, str(value))
    if float(f.get('width')) <= 0 or float(f.get('height')) <= 0:
        raise ValueError('empty source snapshot filter region')
    group = next(n for n in root.iter(NS + 'g') if n.get('id') == group_id)
    group.remove(next(iter(group)))
    ET.SubElement(group, NS + 'image', {
        HREF: 'data:image/png;base64,' + base64.b64encode(png).decode(),
        'x': str(-tx), 'y': str(-ty), 'width': str(natural[0]), 'height': str(natural[1]),
        'preserveAspectRatio': 'none',
    })
    return ET.tostring(root)
