"""Fill the covered interiors of reviewed oversized non-scaling strokes.

SVG's ideal stroke covers every interior point when its width exceeds a
convex shape's bounding-box width or height. Its original fill is then hidden
by the opaque stroke. Painting that fill in the stroke's color preserves the
ideal result while avoiding the inward-offset loops of external strokers.
"""
import math
import re
import xml.etree.ElementTree as ET

from noise_reference import NS, properties
from halfturn_reference import matrix, multiply

NUMBER = r'[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?'
TOKEN = re.compile(r'[a-df-zA-DF-Z]|' + NUMBER)
INHERITED = {'stroke', 'stroke-width', 'stroke-opacity', 'stroke-dasharray', 'paint-order', 'shape-rendering'}
SOLID = re.compile(r'#[0-9a-fA-F]{6}|rgb\(\s*\d{1,3}\s*,\s*\d{1,3}\s*,\s*\d{1,3}\s*\)|red|black|blue|white')


def scalar(value):
    if not re.fullmatch(NUMBER, value):
        return None
    value = float(value)
    return value if math.isfinite(value) else None


def convex_bounds(node):
    tag = node.tag.removeprefix(NS)
    if tag in ['circle', 'ellipse']:
        x, y = scalar(node.get('cx', '0')), scalar(node.get('cy', '0'))
        rx = scalar(node.get('r' if tag == 'circle' else 'rx', '0'))
        ry = rx if tag == 'circle' else scalar(node.get('ry', '0'))
        if None in [x, y, rx, ry] or rx <= 0 or ry <= 0:
            return None
        return x - rx, y - ry, x + rx, y + ry
    if tag != 'path':
        return None
    data = node.get('d', '')
    if TOKEN.sub('', data).strip(' ,\t\r\n'):
        return None
    tokens = TOKEN.findall(data)
    if len(tokens) < 4 or tokens[0] != 'M' or tokens[-1].upper() != 'Z':
        return None
    points = []
    i = 0
    while i < len(tokens) - 1:
        command = tokens[i]
        count = {'M': 2, 'L': 2, 'C': 6}.get(command)
        if count is None or (i and command == 'M') or i + count >= len(tokens):
            return None
        values = [scalar(v) for v in tokens[i + 1:i + 1 + count]]
        if None in values:
            return None
        points.extend(zip(values[::2], values[1::2]))
        i += count + 1
    if i != len(tokens) - 1 or len(points) > 32:
        return None
    if points[-1] == points[0]:
        points.pop()
    # Every edge must support the whole control polygon on the same side.
    # This rejects concave outlines and self-intersections, including stars.
    sign = 0
    for a, b in zip(points, points[1:] + points[:1]):
        for c in points:
            cross = (b[0] - a[0])*(c[1] - a[1]) - (b[1] - a[1])*(c[0] - a[0])
            if cross:
                direction = 1 if cross > 0 else -1
                if sign and sign != direction:
                    return None
                sign = direction
    if not sign:
        return None
    return min(x for x, y in points), min(y for x, y in points), max(x for x, y in points), max(y for x, y in points)


def source(data, width, height, device_scale):
    root = ET.fromstring(data)
    view = [scalar(v) for v in re.split(r'[,\s]+', root.get('viewBox', '').strip())]
    if root.tag != NS + 'svg' or len(view) != 4 or None in view or view[2] <= 0 or view[3] <= 0:
        raise ValueError('wide strokes require a nonempty root viewBox')
    if root.get('preserveAspectRatio', 'xMidYMid meet') not in ['xMidYMid', 'xMidYMid meet']:
        raise ValueError('wide strokes require the default viewport alignment')
    if not all(math.isfinite(v) and v > 0 for v in [width, height, device_scale]):
        raise ValueError('wide strokes require positive viewport dimensions and device scale')
    if b'<?xml-stylesheet' in data:
        raise ValueError('wide strokes do not resolve stylesheets')
    for n in root.iter():
        if (n.tag in [NS + t for t in ['style', 'use', 'filter', 'marker', 'animate', 'animateTransform', 'set', 'script']]
                or (n is not root and n.tag == NS + 'svg')):
            raise ValueError('unsupported wide-stroke document structure')
        style = properties(ET.Element('style', {'style': n.get('style', '')}))
        if any(k in style for k in ['transform', 'transform-origin', 'transform-box', 'd', 'cx', 'cy', 'r', 'rx', 'ry']):
            raise ValueError('wide strokes do not resolve CSS geometry or transforms')
        if any(k in n.attrib for k in ['transform-origin', 'transform-box']):
            raise ValueError('wide strokes do not resolve transform origins')
        if '!important' in n.get('style', '').lower() or any('context-' in v.lower() for v in n.attrib.values()):
            raise ValueError('wide strokes do not resolve CSS overrides or context paints')
        if any(properties(n).get(k, 'none') != 'none' for k in ['filter', 'marker', 'marker-start', 'marker-mid', 'marker-end']):
            raise ValueError('wide strokes do not resolve filters or markers')
    scale = min(width / view[2], height / view[3])
    changed = 0

    def visit(n, outer, inherited):
        nonlocal changed
        own = properties(n)
        p = inherited | own
        m = multiply(outer, matrix(n.get('transform', '')))
        stroke = p.get('stroke', 'none')
        if (own.get('vector-effect') == 'non-scaling-stroke' and p.get('stroke-dasharray', 'none') == 'none'
                and p.get('paint-order', 'normal') == 'normal' and scalar(p.get('stroke-opacity', '1')) == 1
                and p.get('shape-rendering', 'auto') == 'auto'
                and SOLID.fullmatch(stroke)):
            bounds = convex_bounds(n)
            sw = scalar(p.get('stroke-width', '1'))
            if bounds and sw is not None:
                x0, y0, x1, y1 = bounds
                a, b, c, d, _, _ = m
                extent = min(abs(a)*(x1 - x0) + abs(c)*(y1 - y0), abs(b)*(x1 - x0) + abs(d)*(y1 - y0))
                # At least two raster pixels of half-width keep fill AA inside
                # the opaque stroke; a margin also avoids a borderline fit.
                if a*d != b*c and sw * device_scale >= 4 and sw >= extent * 1.001:
                    n.set('style', n.get('style', '') + ';fill:' + stroke + ';fill-opacity:1')
                    changed += 1
        for child in n:
            if child.tag not in [NS + 'defs', NS + 'mask', NS + 'clipPath']:
                visit(child, m, {k: v for k, v in p.items() if k in INHERITED})

    visit(root, (scale, 0, 0, scale, 0, 0), {})
    if not changed:
        raise ValueError('no covered convex non-scaling strokes')
    return ET.tostring(root)


def probes():
    """Known covered centers, including masks, curves and rotated reflections."""
    k = 8 * 4 * (math.sqrt(2) - 1) / 3
    shapes = [
        '<circle r="8"/>',
        '<ellipse rx="10" ry="8"/>',
        '<path d="M -8 -8 L 8 -8 L 8 8 L -8 8 Z"/>',
        f'<path d="M 8 0 C 8 {k} {k} 8 0 8 C {-k} 8 -8 {k} -8 0 '
        f'C -8 {-k} {-k} -8 0 -8 C {k} -8 8 {-k} 8 0 Z"/>',
    ]
    for shape in shapes:
        for transform in ['matrix(1 0 0 1 128 128)', 'matrix(-0.8660254037844386 -0.5 -0.5 0.8660254037844386 128 128)']:
            for masked in [False, True]:
                for stroke in [6, 8, 12]:
                    for device in [1, 2]:
                        mask = 'mask="url(#m)"' if masked else ''
                        painted = shape.replace('/>', f' fill="white" stroke="red" stroke-width="{stroke}" '
                                                f'vector-effect="non-scaling-stroke" {mask}/>')
                        mask_shape = shape.replace('/>', ' fill="white"/>')
                        data = (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256">'
                                f'<defs><mask id="m">{mask_shape}</mask></defs>'
                                f'<g transform="{transform}">{painted}</g></svg>').encode()
                        yield data, 64, 64, device
    # A small CSS stroke can still safely cover the fill at supersampled scale.
    for stroke, device in [(1, 4), (2, 2)]:
        yield (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256">'
               f'<circle cx="128" cy="128" r="{6 / device}" fill="white" stroke="red" '
               f'stroke-width="{stroke}" vector-effect="non-scaling-stroke"/></svg>').encode(), 64, 64, device
