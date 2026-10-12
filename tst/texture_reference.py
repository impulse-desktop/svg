"""Float noise textures on clipped rectangles under uniform rotations.

A finite window covers the viewport and every ancestor clip, with enough halo
for all displacement, convolution and lighting stencils. SourceGraphic coverage
is the exact area of its axis-aligned rectangle in each local pixel. Final PNGs
are composed with the original browser transforms and clips.
"""
import base64
import copy
import json
import math
import re
import xml.etree.ElementTree as ET

import numpy as np
import skia

import halfturn_reference
import layer_reference
from float_reference import IDENTITY, number, pair
from noise_reference import NS, properties
from source_geometry_reference import HREF

TOKEN = re.compile(r'[MmLlHhVvZz]|[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?')


def rectangle(data):
    if TOKEN.sub('', data).strip(' ,\t\r\n'):
        raise ValueError('texture source must use straight rectangle commands')
    tokens = TOKEN.findall(data)
    if not tokens or tokens[0] not in ['M', 'm']:
        raise ValueError('texture rectangle must start with a moveto')
    points, position, command = [], 0, None
    x, y = 0., 0.
    closed = False
    while position < len(tokens):
        if tokens[position].isalpha():
            command = tokens[position]
            position += 1
        if command in ['Z', 'z']:
            closed = position == len(tokens)
            break
        if command not in ['M', 'm', 'L', 'l', 'H', 'h', 'V', 'v']:
            raise ValueError('invalid texture rectangle')
        count = 1 if command.upper() in ['H', 'V'] else 2
        values = [float(v) for v in tokens[position:position+count]]
        if len(values) != count or not all(math.isfinite(v) for v in values):
            raise ValueError('invalid texture rectangle coordinate')
        position += count
        if command.upper() in ['M', 'L']:
            if command.upper() == 'M' and points:
                raise ValueError('texture source has multiple subpaths')
            x, y = (x+values[0], y+values[1]) if command.islower() else values
        elif command.upper() == 'H':
            x = x+values[0] if command.islower() else values[0]
        else:
            y = y+values[0] if command.islower() else values[0]
        points.append((x, y))
        if command.upper() == 'M':
            command = 'l' if command.islower() else 'L'
    if len(points) == 5 and np.allclose(points[0], points[-1], rtol=0, atol=1e-8):
        points.pop()
    if not closed or len(points) != 4:
        raise ValueError('texture source is not a closed rectangle')
    x0, y0 = min(x for x, y in points), min(y for x, y in points)
    x1, y1 = max(x for x, y in points), max(y for x, y in points)
    if x1 <= x0 or y1 <= y0:
        raise ValueError('empty texture rectangle')
    for index, (x, y) in enumerate(points):
        xx, yy = points[(index+1) % 4]
        if (min(abs(x-x0), abs(x-x1)) > 1e-8 or min(abs(y-y0), abs(y-y1)) > 1e-8
                or (abs(x-xx) > 1e-8 and abs(y-yy) > 1e-8) or (abs(x-xx) <= 1e-8 and abs(y-yy) <= 1e-8)):
            raise ValueError('texture source is not an axis-aligned rectangle')
    if abs(sum(points[i][0]*points[(i+1)%4][1]-points[(i+1)%4][0]*points[i][1] for i in range(4)))/2 < (x1-x0)*(y1-y0)*.999999:
        raise ValueError('texture source retraces its rectangle')
    return x0, y0, x1, y1


def coverage(box, color, width, height, scale, tx, ty):
    x0, y0, x1, y1 = box
    xs, ys = np.arange(width), np.arange(height)
    ax = np.maximum(0, np.minimum(xs+1, x1*scale+tx)-np.maximum(xs, x0*scale+tx))
    ay = np.maximum(0, np.minimum(ys+1, y1*scale+ty)-np.maximum(ys, y0*scale+ty))
    pixels = np.empty((height, width, 4), dtype=np.float32)
    pixels[:, :, 3] = ay[:, None]*ax[None, :]
    pixels[:, :, :3] = pixels[:, :, 3:4]*np.asarray(color)
    return skia.Image.fromarray(pixels, colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType,
                                colorSpace=skia.ColorSpace.MakeSRGB())


def prepare(f, scale):
    if (f.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
            or f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
            or properties(f).get('color-interpolation-filters') != 'sRGB'
            or not len(f) or f[-1].tag != NS+'feComposite' or f[-1].get('in2') != 'SourceGraphic'
            or f[-1].get('operator') != 'in'):
        raise ValueError('texture filter must clip an sRGB result to SourceGraphic')
    f = copy.deepcopy(f)
    halo = 4  # Pixel rounding and final bilinear composition.
    for index, node in enumerate(list(f)):
        tag = node.tag.removeprefix(NS)
        if tag == 'feComponentTransfer':
            if any(k not in ['id', 'in', 'result', 'baseFrequency'] and not k.startswith('{') for k in node.attrib):
                raise ValueError('unsupported texture transfer attribute')
            values, channels = IDENTITY.copy(), set()
            for child in node:
                if child.tag not in [NS+'feFunc'+c for c in 'RGBA']:
                    raise ValueError('unsupported texture transfer child')
                channel = 'RGBA'.index(child.tag[-1])
                if channel in channels or any(k not in ['id', 'type', 'slope', 'intercept'] and not k.startswith('{') for k in child.attrib):
                    raise ValueError('unsupported texture transfer function')
                channels.add(channel)
                kind = child.get('type', 'identity')
                if kind == 'linear':
                    values[channel*5+channel] = number(child, 'slope', '1')
                    values[channel*5+4] = number(child, 'intercept', '0')
                elif kind != 'identity':
                    raise ValueError('texture transfer requires identity or linear functions')
            replacement = ET.Element(NS+'feColorMatrix', {k: v for k, v in node.attrib.items() if k in ['id', 'in', 'result']})
            replacement.set('values', ' '.join(map(str, values)))
            f.remove(node)
            f.insert(index, replacement)
        elif tag == 'feTurbulence':
            # Old Inkscape presets carry attributes that SVG ignores on these
            # primitives. They have no coordinate or color semantics here.
            node.attrib.pop('scale', None)
        elif tag == 'feDisplacementMap':
            node.attrib.pop('baseFrequency', None)
            halo += math.ceil(abs(number(node, 'scale', '0'))*scale/2+.5)
        elif tag == 'feConvolveMatrix':
            x, y = pair(node, 'order', '3')
            px, py = number(node, 'targetX', str(int(x)//2)), number(node, 'targetY', str(int(y)//2))
            halo += math.ceil(max(px, x-1-px, py, y-1-py))
        elif tag in ['feDiffuseLighting', 'feSpecularLighting']:
            halo += 1
        elif tag not in ['feFlood', 'feColorMatrix', 'feComposite', 'feBlend']:
            raise ValueError('texture primitive has an unsupported neighborhood')
    return f, max(32, halo)


def measure(browser, data, names):
    return browser.evaluate(r'''(() => {
        const doc = new DOMParser().parseFromString(%s, 'image/svg+xml');
        const svg = document.importNode(doc.documentElement, true);
        svg.style.position = 'absolute'; svg.style.left = '-100000px';
        document.body.replaceChildren(svg);
        const inverse = svg.getCTM().inverse(), out = {};
        for (const name of %s) {
            const node = document.getElementById(name), bbox = node.getBBox();
            const m = inverse.multiply(node.getCTM()), clips = [];
            for (let ancestor=node; ancestor; ancestor=ancestor.parentElement) {
                const value = getComputedStyle(ancestor).clipPath;
                if (value && value !== 'none') {
                    const match = value.match(/^url\(["']?(?:[^#]*)#([^"')]+)["']?\)$/);
                    if (!match) throw Error('unsupported texture clip reference');
                    const cp = document.getElementById(match[1]);
                    if (!cp || cp.getAttribute('clipPathUnits') !== 'userSpaceOnUse' || cp.children.length !== 1 || cp.hasAttribute('transform'))
                        throw Error('unsupported texture clip');
                    const path = cp.children[0];
                    if (path.localName !== 'path' || path.hasAttribute('transform')) throw Error('unsupported texture clip path');
                    const b = path.getBBox(), cm = node.getCTM().inverse().multiply(ancestor.getCTM());
                    const points = [[b.x,b.y],[b.x+b.width,b.y],[b.x,b.y+b.height],[b.x+b.width,b.y+b.height]]
                        .map(([x,y]) => new DOMPoint(x,y).matrixTransform(cm));
                    clips.push([Math.min(...points.map(p=>p.x)), Math.min(...points.map(p=>p.y)),
                                Math.max(...points.map(p=>p.x)), Math.max(...points.map(p=>p.y))]);
                }
                if (ancestor === svg) break;
            }
            out[name] = {bbox: [bbox.x,bbox.y,bbox.width,bbox.height], matrix: [m.a,m.b,m.c,m.d,m.e,m.f], clips};
        }
        document.body.replaceChildren(); return out;
    })()''' % (json.dumps(data.decode()), json.dumps(names)))


def source(data, width, height, measure, padding=None):
    root = ET.fromstring(halfturn_reference.expand(data))
    natural = number(root, 'width', '0')
    if natural <= 0 or number(root, 'height', '0') != natural or width != height or root.get('transform', ''):
        raise ValueError('textures require an untransformed square viewport')
    if 'viewBox' in root.attrib and [float(v) for v in root.get('viewBox').split()] != [0, 0, natural, natural]:
        raise ValueError('texture viewBox differs from its natural viewport')
    root.set('viewBox', f'0 0 {natural} {natural}')
    filters = {n.get('id'): n for n in root.iter(NS+'filter')}
    parents = {c: n for n in root.iter() for c in n}
    ids = {n.get('id') for n in root.iter()}
    targets = []
    for node in root.iter():
        match = re.fullmatch(r'url\(#([^)]*)\)', properties(node).get('filter', ''))
        if match and match[1] in filters:
            ancestor = parents.get(node)
            while ancestor is not None and ancestor.tag not in [NS+'defs', NS+'clipPath']:
                ancestor = parents.get(ancestor)
            if ancestor is not None:
                continue
            name = 'canonicalTexture'+str(len(targets))
            if name in ids:
                raise ValueError('texture identifier collision')
            node.set('id', name)
            targets.append((node, filters[match[1]]))
    measured = measure(ET.tostring(root), [n.get('id') for n, _ in targets])
    zoom, count = width/natural, 0
    for node, f in targets:
        p = properties(node)
        style = properties(ET.Element('style', {'style': node.get('style', '')}))
        if (node.tag != NS+'path' or len(node) or node.get('transform') or not re.fullmatch(r'#[0-9a-fA-F]{6}', p.get('fill', ''))
                or 'd' in style
                or any(p.get(k, value) != value for k, value in [('stroke', 'none'), ('opacity', '1'), ('fill-opacity', '1'),
                       ('mask', 'none'), ('clip-path', 'none'), ('mix-blend-mode', 'normal'), ('display', 'inline'), ('visibility', 'visible'),
                       ('marker', 'none'), ('marker-start', 'none'), ('marker-mid', 'none'), ('marker-end', 'none')])):
            raise ValueError('texture source must be a solid opaque rectangle')
        bounds = rectangle(node.get('d', ''))
        ancestor = parents[node]
        while ancestor is not None:
            props = properties(ancestor)
            if any(props.get(k, value) != value for k, value in [('filter', 'none'), ('opacity', '1'), ('mask', 'none'),
                   ('mix-blend-mode', 'normal'), ('display', 'inline'), ('visibility', 'visible')]):
                raise ValueError('unsupported texture ancestor effect')
            ancestor = parents.get(ancestor)
        item = measured[node.get('id')]
        bx, by, bw, bh = item['bbox']
        if not np.allclose([bx, by, bx+bw, by+bh], bounds, rtol=2e-7, atol=2e-4):
            raise ValueError('measured texture bounds differ from its rectangle')
        a, b, c, d, e, ff = [v*zoom for v in item['matrix']]
        if not all(math.isfinite(v) for v in [bx, by, bw, bh, a, b, c, d, e, ff]):
            raise ValueError('non-finite texture frame')
        sx, sy = math.hypot(a, b), math.hypot(c, d)
        if (min(sx, sy) <= 0 or a*d-b*c <= 0 or abs(sx-sy) > 1e-6*max(sx, sy)
                or abs(a*c+b*d) > 1e-6*sx*sy):
            raise ValueError('texture frame must be a uniform rotation')
        scale = (sx+sy)/2
        prepared, halo = prepare(f, scale)
        if padding is not None:
            halo = max(halo, padding)
        inverse = np.linalg.inv(np.array([[a,c,e], [b,d,ff], [0,0,1]]))
        corners = [inverse@np.array([x,y,1]) for x in [0,width] for y in [0,height]]
        vx0, vy0 = min(p[0] for p in corners), min(p[1] for p in corners)
        vx1, vy1 = max(p[0] for p in corners), max(p[1] for p in corners)
        if max(vx0, bx) >= min(vx1, bx+bw) or max(vy0, by) >= min(vy1, by+bh):
            continue
        for x0, y0, x1, y1 in item['clips']:
            if not all(math.isfinite(v) for v in [x0, y0, x1, y1]):
                raise ValueError('non-finite texture clip')
            vx0, vy0, vx1, vy1 = max(vx0, x0), max(vy0, y0), min(vx1, x1), min(vy1, y1)
        if vx0 >= vx1 or vy0 >= vy1:
            continue
        if (abs(b) < 1e-6 and abs(c) < 1e-6) or (abs(a) < 1e-6 and abs(d) < 1e-6):
            qx, qy = round(a/sx)*e+round(b/sx)*ff, round(c/sy)*e+round(d/sy)*ff
        else:
            qx = qy = 0.
        left, top = math.floor(vx0*scale+qx)-halo, math.floor(vy0*scale+qy)-halo
        right, bottom = math.ceil(vx1*scale+qx)+halo, math.ceil(vy1*scale+qy)+halo
        w, h = right-left, bottom-top
        if w <= 0 or h <= 0 or w*h > 16*width*height:
            raise ValueError('unsupported texture window')
        ux, uy = (left-qx)/scale, (top-qy)/scale
        fx, fy = bx+number(f, 'x', '-.1')*bw, by+number(f, 'y', '-.1')*bh
        if (ux < fx or uy < fy or ux+w/scale > fx+number(f, 'width', '1.2')*bw
                or uy+h/scale > fy+number(f, 'height', '1.2')*bh):
            raise ValueError('texture window touches the filter boundary')
        color = [int(p['fill'][i:i+2], 16)/255 for i in [1,3,5]]
        image = coverage(bounds, color, w, h, scale, qx-left, qy-top)
        result = layer_reference.graph(prepared, image, scale, qx-left, qy-top)
        pixels = result.toarray(colorType=skia.kRGBA_8888_ColorType, alphaType=skia.kUnpremul_AlphaType)
        snapshot = skia.Image.fromarray(pixels, colorType=skia.kRGBA_8888_ColorType, alphaType=skia.kUnpremul_AlphaType)
        encoded = snapshot.encodeToData()
        if encoded is None:
            raise ValueError('could not encode the texture window')
        attrs = {HREF: 'data:image/png;base64,'+base64.b64encode(bytes(encoded)).decode(),
                 'x': str(ux), 'y': str(uy), 'width': str(w/scale), 'height': str(h/scale), 'preserveAspectRatio': 'none'}
        parent = parents[node]
        index = list(parent).index(node)
        parent.remove(node)
        parent.insert(index, ET.Element(NS+'image', attrs))
        count += 1
    if not count:
        raise ValueError('no visible texture windows')
    return ET.tostring(root)


def check():
    version = layer_reference.check()
    for scale in [.75, 1, 2]:
        box = [2.25, 3.125, 19.75, 22.875]
        image = coverage(box, [.2, .4, .8], 64, 64, scale, .25, .5)
        pixels = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)
        expected = (box[2]-box[0])*(box[3]-box[1])*scale*scale
        if abs(float(pixels[:, :, 3].astype(np.float64).sum())-expected) > 1e-4:
            raise ValueError('texture rectangle coverage lost area')
    for alpha in [.2, .5002, 1]:
        image = coverage([0,0,64,64], [.3,.6,.9], 64, 64, 1, 0, 0)
        array = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)*alpha
        image = skia.Image.fromarray(array, colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType,
                                    colorSpace=skia.ColorSpace.MakeSRGB())
        f = ET.fromstring('<filter xmlns="http://www.w3.org/2000/svg"><feConvolveMatrix kernelMatrix="0 -1 0 -1 4 -1 0 -1 0" bias=".4" preserveAlpha="true"/></filter>')
        result = layer_reference.graph(f, image, 1, 0, 0)
        actual = result.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)[32,32]
        if any(abs(float(a)-e) > 1e-5 for a, e in zip(actual, [.4,.4,.4,alpha])):
            raise ValueError('texture convolution did not preserve alpha or normalize its bias')
        f = ET.fromstring('''<filter xmlns="http://www.w3.org/2000/svg" color-interpolation-filters="sRGB">
           <feComponentTransfer baseFrequency=".1"><feFuncR type="linear" slope=".9"/>
           <feFuncG type="linear" slope="2" intercept="-.1"/><feFuncB type="identity"/>
           <feFuncA type="linear" slope="1.45" intercept=".1"/></feComponentTransfer>
           <feComposite in2="SourceGraphic" operator="in"/></filter>''')
        prepared, _ = prepare(f, 1)
        result = layer_reference.graph(prepared, image, 1, 0, 0)
        actual = result.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)[32,32]
        expected = [.27, 1, .9, alpha*min(1, 1.45*alpha+.1)]
        if any(abs(float(a)-e) > 1e-5 for a, e in zip(actual, expected)):
            raise ValueError('texture linear transfer differs from its channel equations')
    for scale in [.75, 1, 2]:
        image = coverage([0,0,64,64], [1,0,0], 64, 64, 1, 0, 0)
        for kind, expected in [('fractalNoise', .5), ('turbulence', 0)]:
            f = ET.fromstring(f'<filter xmlns="http://www.w3.org/2000/svg"><feTurbulence type="{kind}" baseFrequency="{scale}" numOctaves="10" seed="7"/></filter>')
            result = layer_reference.graph(f, image, scale, .5, .5)
            actual = result.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
            if any(abs(float(v)-expected) > .0005 for x,y in [(9,9),(12,12),(15,15)] for v in actual[y,x]):
                raise ValueError('texture ten-octave noise phase changed')
    return version


def probes(scale=1):
    for angle in range(0, 360, 30):
        radians = math.radians(angle)
        a, b, c, d = math.cos(radians), math.sin(radians), -math.sin(radians), math.cos(radians)
        e, f = 48-48*a-48*c, 48-48*b-48*d
        for kind in ['Diffuse', 'Specular', 'fractalNoise', 'turbulence']:
            if kind in ['Diffuse', 'Specular']:
                primitives = '<feTurbulence numOctaves="0" type="fractalNoise"/>'
                primitives += f'<fe{kind}Lighting surfaceScale="-5"'+(' specularExponent="8"' if kind == 'Specular' else '')+'><feDistantLight azimuth="30" elevation="40"/></fe'+kind+'Lighting>'
                value = round(255*math.sin(math.radians(40))) if kind == 'Diffuse' else round(255*((1+math.sin(math.radians(40)))/2)**4)
                expected = [value,value,value,255] if kind == 'Diffuse' else [255,255,255,value]
            else:
                # Twice the device scale puts every local pixel center on an
                # integer noise lattice vertex, including after a quarter turn.
                primitives = f'<feTurbulence baseFrequency="{2*scale}" type="{kind}" seed="7" numOctaves="10"/>'
                expected = [128]*4 if kind == 'fractalNoise' else [0]*4
            data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="96" height="96" xmlns:xlink="http://www.w3.org/1999/xlink">'
                    '<defs><filter id="f" color-interpolation-filters="sRGB">'+primitives
                    +'<feComposite in2="SourceGraphic" operator="in"/></filter>'
                    '<clipPath id="outer" clipPathUnits="userSpaceOnUse"><path d="M12 16h74v72h-74z"/></clipPath>'
                    '<clipPath id="clip" clipPathUnits="userSpaceOnUse"><path d="M20 20h56v56h-56z"/></clipPath>'
                    '<g id="tile"><path id="p" d="M-200 -200h400v400h-400z" fill="#112233" filter="url(#f)"/></g></defs>'
                    '<g clip-path="url(#outer)"><use xlink:href="#tile" clip-path="url(#clip)" '
                    f'transform="matrix({a} {b} {c} {d} {e} {f})"/></g></svg>').encode()
            yield data, expected
