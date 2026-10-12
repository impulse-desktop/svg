"""Use Batik's stitched noise, with device-centre sampling, in browser references.

Only the reviewed single-octave, first-primitive cases are supported. Batik
samples pixel corners, so its noise-only SVG viewBox advances half a device
pixel. The resulting PNG is placed at the original pixel-aligned coordinates.
The tile region remains in the original user space throughout the conversion.
"""
import base64
import copy
import math
import re
import xml.etree.ElementTree as ET

import noise_reference

NS = noise_reference.NS
HREF = '{http://www.w3.org/1999/xlink}href'


def noise_svg(primitive, region, pixels, scale, space):
    x, y, width, height = region
    left, top, rw, rh = pixels
    root = ET.Element(NS + 'svg', {'width': str(rw), 'height': str(rh),
                      'viewBox': f'{(left+.5)/scale} {(top+.5)/scale} {rw/scale} {rh/scale}'})
    f = ET.SubElement(root, NS + 'filter', {'id': 'noise', 'filterUnits': 'userSpaceOnUse',
                      'primitiveUnits': 'userSpaceOnUse', 'x': str(x), 'y': str(y),
                      'width': str(width), 'height': str(height), 'color-interpolation-filters': space})
    f.append(copy.deepcopy(primitive))
    ET.SubElement(root, NS + 'rect', {'x': str(x), 'y': str(y), 'width': str(width),
                  'height': str(height), 'filter': 'url(#noise)'})
    return ET.tostring(root)


def source(data, width, height, measure, rasterize):
    root = ET.fromstring(data)
    for n in root.iter(NS + 'style'):
        if re.sub(r'@font-face\s*\{[^}]*\}', '', n.text or '').strip():
            raise ValueError('stitched noise does not resolve stylesheets')
    if any(n.tag in [NS + 'text', NS + 'foreignObject'] for n in root.iter()):
        raise ValueError('stitched noise does not resolve text or foreign content')
    box = [float(v) for v in root.get('viewBox', '').split()]
    if len(box) != 4 or box[:2] != [0, 0] or min(box[2:]) <= 0:
        raise ValueError('stitched noise requires a positive zero-origin viewBox')
    scale = width / box[2]
    if not math.isclose(scale, height / box[3], rel_tol=1e-12):
        raise ValueError('stitched noise requires uniform viewport scaling')
    filters = {f.get('id'): f for f in root.iter(NS + 'filter')
               if any(n.tag == NS + 'feTurbulence' and n.get('stitchTiles') == 'stitch' for n in f)}
    targets = {}
    for n in root.iter():
        if 'data-canon-stitch' in n.attrib:
            raise ValueError('stitched noise marker collision')
        match = re.fullmatch(r'url\(#([^)]*)\)', noise_reference.properties(n).get('filter', ''))
        if match and match[1] in filters:
            if match[1] in targets:
                raise ValueError('stitched filter has multiple consumers')
            n.set('data-canon-stitch', match[1])
            targets[match[1]] = n
    if not filters or targets.keys() != filters.keys():
        raise ValueError('stitched noise did not resolve every filter consumer')
    measurements = measure(ET.tostring(root))
    for name, f in filters.items():
        if (not len(f) or f[0].tag != NS + 'feTurbulence' or f[0].get('stitchTiles') != 'stitch'
                or sum(n.tag == NS + 'feTurbulence' and n.get('stitchTiles') == 'stitch' for n in f) != 1
                or f.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
                or f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
                or any(k in n.attrib for n in [f, f[0]] for k in ['x', 'y', 'width', 'height', 'filterRes', 'href', HREF])):
            raise ValueError('stitched noise requires the first primitive and default bounding-box region')
        primitive = f[0]
        if primitive.get('numOctaves', '1').strip() != '1':
            raise ValueError('Batik octave limiting is only bypassed by single-octave inputs')
        frequency = [float(v) for v in primitive.get('baseFrequency', '0').replace(',', ' ').split()]
        if len(frequency) not in [1, 2] or min(frequency) <= 0 or max(frequency) > scale:
            raise ValueError('stitched frequency is outside the supported Batik sampling range')
        measured = measurements[name]
        if any(abs(a-b) > 1e-7 for a, b in zip(measured['matrix'], [1, 0, 0, 1, 0, 0])):
            raise ValueError('stitched noise requires an untransformed filter frame')
        space = {'srgb': 'sRGB', 'linearrgb': 'linearRGB'}.get(measured['space'].lower())
        if space not in ['sRGB', 'linearRGB']:
            raise ValueError('unsupported stitched noise colour space ' + repr(measured['space']))
        x, y, w, h = measured['bbox']
        x, y, w, h = x - .1*w, y - .1*h, 1.2*w, 1.2*h
        left, top = math.floor(x*scale), math.floor(y*scale)
        rw, rh = math.ceil((x+w)*scale)-left, math.ceil((y+h)*scale)-top
        if min(rw, rh) <= 0 or rw*rh > 16*width*height:
            raise ValueError('stitched noise raster exceeds the supported region')
        png = rasterize(noise_svg(primitive, (x, y, w, h), (left, top, rw, rh), scale, space), rw, rh)
        image = ET.Element(NS + 'feImage', {HREF: 'data:image/png;base64,' + base64.b64encode(png).decode(),
                           'x': str(left/scale), 'y': str(top/scale), 'width': str(rw/scale), 'height': str(rh/scale),
                           'preserveAspectRatio': 'none', 'color-interpolation-filters': space})
        if primitive.get('result'):
            image.set('result', primitive.get('result'))
        f.remove(primitive)
        f.insert(0, image)
        targets[name].attrib.pop('data-canon-stitch')
    data = ET.tostring(root)
    if any(n.tag == NS + 'feTurbulence' for n in root.iter()):
        data = noise_reference.source(data)
    return data
