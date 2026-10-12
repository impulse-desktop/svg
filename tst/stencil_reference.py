"""Float noise stencils inside an SVG 1.1 document composed by Inkscape.

Only selected filters with discrete transfer functions are replaced. Unused
class styles and one-stop opaque swatches are resolved before isolating their
source groups; unrelated filters and legacy backgrounds remain in the document.
"""
import math
import re
import xml.etree.ElementTree as ET

import layer_reference
from halfturn_reference import matrix
from noise_reference import NS, properties
from source_geometry_reference import HREF


def selected(f):
    return any(n.tag == NS+'feComponentTransfer' and any(c.get('type') == 'discrete' for c in n) for n in f)


def backgrounds(root, by_id):
    consumers = {}
    for node in root.iter():
        match = re.fullmatch(r'url\(#([^)]*)\)', properties(node).get('filter', ''))
        if match:
            consumers.setdefault(match[1], []).append(node)
    for name, f in by_id.items():
        if f.tag != NS+'filter':
            continue
        supported = True
        for primitive in f.iter():
            inputs = [(k, primitive.get(k)) for k in ['in', 'in2'] if primitive.get(k) in ['BackgroundImage', 'BackgroundAlpha']]
            if not inputs:
                continue
            supported &= ((primitive.tag == NS+'feBlend' and inputs == [('in2', 'BackgroundImage')]
                           and primitive.get('mode') in ['multiply', 'darken'])
                          or (primitive.tag == NS+'feComposite' and inputs == [('in', 'BackgroundImage')]
                              and primitive.get('operator') == 'arithmetic'))
        if not supported:
            # Inkscape 1.2.2 does not reliably supply BackgroundImage to a lone
            # feComposite's in2. An empty group's zero bounding box disables
            # its objectBoundingBox filter; otherwise reject this document.
            if (f.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
                    or any(n.tag != NS+'g' or len(n) for n in consumers.get(name, []))):
                raise ValueError('unsupported visible legacy background operation')


def prepare(data):
    root = ET.fromstring(data)
    parents = {c: n for n in root.iter() for c in n}
    styles = list(root.iter(NS+'style'))
    for style in styles:
        if (len(style) or not re.fullmatch(r'(?:\s*\.[A-Za-z_][\w-]*\s*\{[^{}]*\}\s*)*', style.text or '')
                or any(n.get('class') for n in root.iter())):
            raise ValueError('stencil source requires unused class-only stylesheets')
        parents[style].remove(style)
    by_id = {n.get('id'): n for n in root.iter() if n.get('id')}
    backgrounds(root, by_id)
    for node in root.iter():
        match = re.fullmatch(r'url\(#([^)]*)\)', properties(node).get('filter', ''))
        f = by_id.get(match[1]) if match else None
        if f is None or f.tag != NS+'filter' or not selected(f):
            continue
        for child in node.iter():
            for key in ['fill', 'stroke']:
                match = re.fullmatch(r'url\(#([^)]*)\)', properties(child).get(key, ''))
                if not match:
                    continue
                gradient = by_id[match[1]]
                if (gradient.tag != NS+'linearGradient' or len(gradient) != 1
                        or gradient[0].tag != NS+'stop' or gradient.get('href', gradient.get(HREF)) is not None):
                    raise ValueError('stencil paint requires one explicit gradient stop')
                a, b, c, d, x, y = matrix(gradient.get('gradientTransform', ''))
                if not all(math.isfinite(v) for v in [a,b,c,d,x,y]) or a*d-b*c == 0:
                    raise ValueError('invalid stencil swatch transform')
                stop = properties(gradient[0])
                color = stop.get('stop-color', '')
                if not re.fullmatch(r'#[0-9a-fA-F]{6}', color) or float(stop.get('stop-opacity', '1')) != 1:
                    raise ValueError('stencil swatch must be an opaque RGB color')
                child.set('style', child.get('style', '')+f';{key}:{color}')
    return ET.tostring(root)


def source(data, width, height, measure, rasterize):
    return layer_reference.source(prepare(data), width, height, measure, rasterize,
                                  affine=True, composed=True, select=selected)


def geometry_probes():
    # Analytic annuli at the reviewed source-grid sizes. Their areas expose
    # Cairo's curve flattening error before noise and thresholding are applied.
    for radius, thickness in [(117.1, 16.34), (72.91, 9.91)]:
        for phase in [0, .125, .37]:
            data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="335" height="335" viewBox="0 0 335 335">'
                    f'<circle cx="{167+phase}" cy="{167+phase}" r="{radius}" fill="none" '
                    f'stroke="#3771c8" stroke-width="{thickness}"/></svg>').encode()
            yield data, 2*math.pi*radius*thickness


def background_probes():
    for op, color, samples in [
        ('<feComposite in="BackgroundImage" in2="SourceGraphic" operator="arithmetic" k1="1" k2=".5"/>', '#0000ff',
         [(4, [255,0,0,255]), (12, [128,0,0,255]), (20, [0,0,0,0])]),
        ('<feBlend in="SourceGraphic" in2="BackgroundImage" mode="multiply"/>', '#80ff80',
         [(4, [255,0,0,255]), (12, [128,0,0,255]), (20, [128,255,128,255])]),
        ('<feBlend in="SourceGraphic" in2="BackgroundImage" mode="darken"/>', '#80ff80',
         [(4, [255,0,0,255]), (12, [128,0,0,255]), (20, [128,255,128,255])]),
    ]:
        data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 32 32" enable-background="new">'
                f'<defs><filter id="f" color-interpolation-filters="sRGB">{op}</filter></defs>'
                '<path fill="red" d="M0 0h16v32H0z"/>'
                f'<path fill="{color}" filter="url(#f)" d="M8 0h16v32H8z"/></svg>').encode()
        yield data, samples
