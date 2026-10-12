"""Float filter snapshots for uniformly scaled solid paths in reviewed SVGs.

Inkscape supplies SourceGraphic. Stock Skia evaluates each primitive using float
storage. Binary inputs share one float source atlas: embedding an image in a
Python Skia filter serializes it as an 8-bit PNG, and cloning a growing filter
DAG duplicates its branches. Neither occurs with primary inputs and offsets.
Other transforms retain their original browser rendering.
"""
import base64
import copy
import math
import re
import xml.etree.ElementTree as ET

import skia

from float_reference import IDENTITY, blur, check as check_float, info, number, pair
from noise_reference import NS, properties
from source_geometry_reference import HREF


ATTRIBUTES = {
    'feGaussianBlur': {'stdDeviation'},
    'feSpecularLighting': {'surfaceScale', 'specularConstant', 'specularExponent', 'lighting-color'},
    'feDiffuseLighting': {'surfaceScale', 'diffuseConstant', 'lighting-color'},
    'feComposite': {'operator', 'in2', 'k1', 'k2', 'k3', 'k4'},
    'feBlend': {'mode', 'in2'}, 'feMorphology': {'operator', 'radius'},
    'feColorMatrix': {'type', 'values'},
    'feTurbulence': {'baseFrequency', 'seed', 'numOctaves', 'type', 'stitchTiles'},
    'feDisplacementMap': {'in2', 'scale', 'xChannelSelector', 'yChannelSelector'},
    'feFlood': {'flood-color', 'flood-opacity'}, 'feOffset': {'dx', 'dy'},
    'feConvolveMatrix': {'order', 'kernelMatrix', 'divisor', 'bias', 'targetX', 'targetY', 'edgeMode', 'preserveAlpha'},
}


def color_filter(values, source, crop):
    if len(values) != 20 or not all(math.isfinite(v) for v in values):
        raise ValueError('invalid layer color matrix')
    return skia.ImageFilters.ColorFilter(skia.ColorFilters.Matrix(values), source, crop)


def apply(source, effect, bounds):
    if effect is None:
        raise ValueError('Skia rejected the layer primitive')
    subset, offset = skia.IRect(), skia.IPoint(0, 0)
    image = source.makeWithFilter(None, effect, skia.IRect.MakeWH(source.width(), source.height()),
                                  bounds, subset, offset)
    surface = skia.Surface.MakeRaster(info(bounds.width(), bounds.height()))
    canvas = surface.getCanvas()
    canvas.clear(0)
    # An empty primitive is a valid transparent result.
    if image is not None:
        if image.colorType() != skia.kRGBA_F32_ColorType:
            raise ValueError('Skia changed the layer precision')
        canvas.drawImageRect(image, skia.Rect(subset),
                             skia.Rect.MakeXYWH(offset.x(), offset.y(), subset.width(), subset.height()))
    return surface.makeImageSnapshot()


def graph(f, source, scale, tx, ty):
    if scale <= 0 or not all(math.isfinite(v) for v in [scale, tx, ty]):
        raise ValueError('invalid uniform layer frame')
    crop = skia.IRect.MakeWH(source.width(), source.height())
    alpha = color_filter([0]*18 + [1, 0], None, crop)
    results = {'SourceGraphic': source, 'SourceAlpha': apply(source, alpha, crop)}
    last = source
    for node in f:
        tag = node.tag.removeprefix(NS)
        if (tag not in ATTRIBUTES or any(k not in ATTRIBUTES[tag] | {'id', 'in', 'result'}
                and not k.startswith('{') for k in node.attrib)):
            raise ValueError('unsupported layer primitive or attribute: '+tag)
        image = results[node.get('in')] if node.get('in') else last
        a = None
        if tag in ['feComposite', 'feBlend', 'feDisplacementMap']:
            second = results[node.get('in2')] if node.get('in2') else last
            atlas = skia.Surface.MakeRaster(info(2*crop.width(), crop.height()))
            canvas = atlas.getCanvas()
            canvas.clear(0)
            canvas.drawImage(image, 0, 0)
            canvas.drawImage(second, crop.width(), 0)
            image = atlas.makeImageSnapshot()
            a = color_filter(IDENTITY, None, crop)
            b = skia.ImageFilters.Offset(-crop.width(), 0, None, crop)
        if tag == 'feGaussianBlur':
            x, y = pair(node, 'stdDeviation', '0')
            effect = blur(x*scale, y*scale, a, crop)
        elif tag == 'feTurbulence':
            x, y = pair(node, 'baseFrequency', '0')
            octaves = int(node.get('numOctaves', '1'))
            if not 0 <= octaves <= 8 or node.get('stitchTiles', 'noStitch') != 'noStitch':
                raise ValueError('unsupported layer noise')
            constructor = {'fractalNoise': skia.PerlinNoiseShader.MakeFractalNoise,
                           'turbulence': skia.PerlinNoiseShader.MakeTurbulence}[node.get('type', 'turbulence')]
            shader = constructor(x, y, octaves, number(node, 'seed', '0'))
            if shader is None:
                raise ValueError('Skia rejected the layer noise')
            recorder = skia.PictureRecorder()
            canvas = recorder.beginRecording(skia.Rect(crop))
            canvas.translate(tx, ty)
            canvas.scale(scale, scale)
            canvas.translate(.5, .5)
            canvas.drawPaint(skia.Paint(Shader=shader))
            effect = skia.ImageFilters.Picture(recorder.finishRecordingAsPicture())
        elif tag in ['feSpecularLighting', 'feDiffuseLighting']:
            if (len(node) != 1 or node[0].tag != NS+'feDistantLight'
                    or node.get('lighting-color', 'white') not in ['white', '#ffffff']):
                raise ValueError('layer lighting requires one white distant light')
            light = node[0]
            if any(k not in ['id', 'azimuth', 'elevation'] and not k.startswith('{') for k in light.attrib):
                raise ValueError('unsupported layer distant light')
            az = math.radians(number(light, 'azimuth', '0'))
            el = math.radians(number(light, 'elevation', '0'))
            direction = skia.Point3(scale*math.cos(el)*math.cos(az), scale*math.cos(el)*math.sin(az), scale*math.sin(el))
            ss = number(node, 'surfaceScale', '1')*scale
            if tag == 'feSpecularLighting':
                effect = skia.ImageFilters.DistantLitSpecular(direction, skia.ColorWHITE, ss,
                         number(node, 'specularConstant', '1'), number(node, 'specularExponent', '1'), a, crop)
            else:
                effect = skia.ImageFilters.DistantLitDiffuse(direction, skia.ColorWHITE, ss,
                         number(node, 'diffuseConstant', '1'), a, crop)
        elif tag in ['feComposite', 'feBlend']:
            mode = node.get('operator', 'over') if tag == 'feComposite' else node.get('mode', 'normal')
            if mode == 'arithmetic':
                ks = [number(node, k, '0') for k in ['k1', 'k2', 'k3', 'k4']]
                effect = skia.ImageFilters.Arithmetic(*ks, True, b, a, crop)
            else:
                modes = {'over': skia.BlendMode.kSrcOver, 'normal': skia.BlendMode.kSrcOver,
                         'in': skia.BlendMode.kSrcIn, 'out': skia.BlendMode.kSrcOut,
                         'atop': skia.BlendMode.kSrcATop, 'xor': skia.BlendMode.kXor,
                         'multiply': skia.BlendMode.kMultiply, 'screen': skia.BlendMode.kScreen,
                         'darken': skia.BlendMode.kDarken, 'lighten': skia.BlendMode.kLighten}
                effect = skia.ImageFilters.Xfermode(modes[mode], b, a, crop)
        elif tag == 'feColorMatrix':
            kind = node.get('type', 'matrix')
            if kind == 'matrix':
                values = [float(v) for v in node.get('values', ' '.join(map(str, IDENTITY))).replace(',', ' ').split()]
            elif kind == 'luminanceToAlpha':
                values = [0]*20
                values[15:18] = [.2125, .7154, .0721]
            else:
                raise ValueError('unsupported layer color matrix')
            effect = color_filter(values, a, crop)
        elif tag == 'feDisplacementMap':
            values = [0]*20
            for row, key in [(0, 'xChannelSelector'), (1, 'yChannelSelector')]:
                values[5*row+'RGBA'.index(node.get(key, 'A'))] = 1
            values[12] = values[18] = 1
            displacement = color_filter(values, b, crop)
            effect = skia.ImageFilters.DisplacementMap(skia.ColorChannel.kR, skia.ColorChannel.kG,
                       number(node, 'scale', '0')*scale, displacement, a, crop)
        elif tag == 'feMorphology':
            x, y = pair(node, 'radius', '0')
            op = {'dilate': skia.ImageFilters.Dilate, 'erode': skia.ImageFilters.Erode}[node.get('operator', 'erode')]
            effect = op(x*scale, y*scale, a, crop)
        elif tag == 'feFlood':
            color = node.get('flood-color', '')
            if not re.fullmatch(r'rgb\(\d+,\d+,\d+\)', color):
                raise ValueError('layer flood requires explicit integer RGB')
            rgb = [int(v)/255 for v in re.findall(r'\d+', color)]
            effect = color_filter([0,0,0,0,rgb[0], 0,0,0,0,rgb[1], 0,0,0,0,rgb[2],
                                   0,0,0,0,number(node, 'flood-opacity', '1')], None, crop)
        elif tag == 'feOffset':
            effect = skia.ImageFilters.Offset(number(node, 'dx', '0')*scale, number(node, 'dy', '0')*scale, a, crop)
        elif tag == 'feConvolveMatrix':
            x, y = pair(node, 'order', '3')
            if x != int(x) or y != int(y) or not 1 <= x <= 32 or not 1 <= y <= 32 or number(node, 'bias', '0') != 0:
                raise ValueError('unsupported layer convolution')
            x, y = int(x), int(y)
            kernel = [float(v) for v in node.get('kernelMatrix', '').replace(',', ' ').split()]
            if len(kernel) != x*y or not all(math.isfinite(v) for v in kernel):
                raise ValueError('invalid layer kernel')
            divisor = number(node, 'divisor', str(sum(kernel) or 1))
            px, py = number(node, 'targetX', str(x//2)), number(node, 'targetY', str(y//2))
            if divisor == 0 or px != int(px) or py != int(py) or not 0 <= px < x or not 0 <= py < y:
                raise ValueError('invalid layer convolution origin or divisor')
            edge = {'duplicate': skia.TileMode.kClamp, 'none': skia.TileMode.kDecal, 'wrap': skia.TileMode.kRepeat}[node.get('edgeMode', 'duplicate')]
            preserve = {'false': False, 'true': True}[node.get('preserveAlpha', 'false')]
            effect = skia.ImageFilters.MatrixConvolution(skia.ISize(x, y), list(reversed(kernel)), 1/divisor, 0,
                       skia.IPoint(x-1-int(px), y-1-int(py)), edge, not preserve, a, crop)
        last = apply(image, effect, crop)
        if node.get('result'):
            results[node.get('result')] = last
    return last


def source(data, width, height, measure, rasterize):
    root = ET.fromstring(data)
    if any(n.tag == NS+'style' for n in root.iter()) or any(n is not root and n.tag == NS+'svg' for n in root.iter()):
        raise ValueError('layer snapshots do not resolve stylesheets or nested viewports')
    if properties(root).get('transform', 'none') not in ['', 'none']:
        raise ValueError('layer snapshots do not resolve root transforms')
    box = [float(v) for v in root.get('viewBox', '').split()]
    if (len(box) != 4 or box[:2] != [0, 0] or min(box[2:]) <= 0
            or root.get('preserveAspectRatio', 'xMidYMid meet') not in ['xMidYMid', 'xMidYMid meet']):
        raise ValueError('layer snapshots require a zero-origin meet viewBox')
    zoom = min(width/box[2], height/box[3])
    ox, oy = (width-box[2]*zoom)/2, (height-box[3]*zoom)/2
    parents = {c: n for n in root.iter() for c in n}
    filters = {f.get('id'): f for f in root.iter(NS+'filter')}
    targets = []
    for node in root.iter():
        match = re.fullmatch(r'url\(#([^)]*)\)', properties(node).get('filter', ''))
        f = filters.get(match[1]) if match else None
        if f is not None and any(n.tag == NS+'feTurbulence' for n in f):
            if not node.get('id'):
                raise ValueError('layer target has no identifier')
            targets.append((node, f))
    measured = measure(data, [n.get('id') for n, _ in targets])
    count = 0
    for node, f in targets:
        item = measured[node.get('id')]
        bx, by, bw, bh = item['bbox']
        a, b, c, d, x, y = item['matrix']
        if not all(math.isfinite(v) for v in [bx, by, bw, bh, a, b, c, d, x, y]):
            raise ValueError('non-finite measured layer frame')
        # Nonuniform, rotated and degenerate targets remain browser-rendered.
        if bw <= 0 or bh <= 0 or a <= 0 or abs(b)+abs(c) > 1e-8 or abs(a-d) > 1e-8*max(a, d):
            continue
        p = properties(node)
        if any('%' in v for v in p.values()):
            raise ValueError('layer snapshots do not resolve percentage paints')
        if (node.tag != NS+'path' or len(node) or any('url(' in p.get(k, '') or 'currentColor' in p.get(k, '') for k in ['fill', 'stroke'])
                or any(p.get(k, default) != default for k, default in [('mask', 'none'), ('clip-path', 'none'),
                       ('mix-blend-mode', 'normal'), ('vector-effect', 'none'), ('stroke-dasharray', 'none'),
                       ('marker-start', 'none'), ('marker-mid', 'none'), ('marker-end', 'none'), ('display', 'inline'), ('visibility', 'visible')])):
            raise ValueError('layer snapshot requires a plain solid path')
        style = properties(ET.Element('style', {'style': node.get('style', '')}))
        if any(k in style for k in ['transform', 'transform-origin', 'transform-box']) or any('!' in v for v in p.values()):
            raise ValueError('unsupported layer CSS override')
        if any(n.get('href', n.get(HREF)) == '#'+node.get('id') for n in root.iter()):
            raise ValueError('referenced layer target')
        if (f.get('filterUnits', 'objectBoundingBox') != 'objectBoundingBox'
                or f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
                or properties(f).get('color-interpolation-filters') != 'sRGB'):
            raise ValueError('layer filter requires an sRGB bounding-box region')
        fx = bx+number(f, 'x', '-.1')*bw
        fy = by+number(f, 'y', '-.1')*bh
        fw, fh = number(f, 'width', '1.2')*bw, number(f, 'height', '1.2')*bh
        scale, qx, qy = zoom*a, zoom*x+ox, zoom*y+oy
        left, top = math.floor(fx*scale+qx), math.floor(fy*scale+qy)
        right, bottom = math.ceil((fx+fw)*scale+qx), math.ceil((fy+fh)*scale+qy)
        w, h = right-left, bottom-top
        if w <= 0 or h <= 0 or w*h > 16*width*height:
            raise ValueError('unsupported layer extent')
        ux, uy = (left-qx)/scale, (top-qy)/scale
        local = ET.Element(NS+'svg', {'width': str(w), 'height': str(h),
                           'viewBox': f'{ux} {uy} {w/scale} {h/scale}', 'preserveAspectRatio': 'none'})
        chain, ancestor = [], parents[node]
        while True:
            props = properties(ancestor)
            ancestor_style = properties(ET.Element('style', {'style': ancestor.get('style', '')}))
            if any(k in ancestor_style for k in ['transform', 'transform-origin', 'transform-box']):
                raise ValueError('layer snapshot has a CSS-transformed ancestor')
            if any('%' in v or 'url(' in v or '!' in v for k, v in props.items() if k not in ['id', 'width', 'height']):
                raise ValueError('layer snapshot has an unsupported ancestor paint')
            if any(props.get(k, default) != default for k, default in [('filter', 'none'), ('opacity', '1'),
                   ('mask', 'none'), ('clip-path', 'none'), ('mix-blend-mode', 'normal'),
                   ('display', 'inline'), ('visibility', 'visible'), ('vector-effect', 'none'),
                   ('marker-start', 'none'), ('marker-mid', 'none'), ('marker-end', 'none')]):
                raise ValueError('layer snapshot has an ancestor effect')
            if ancestor is root:
                for key in ['fill', 'stroke', 'fill-rule', 'fill-opacity', 'stroke-width', 'stroke-opacity',
                            'stroke-linecap', 'stroke-linejoin', 'stroke-miterlimit', 'color']:
                    if key in props:
                        local.set(key, props[key])
                break
            if ancestor.tag != NS+'g':
                raise ValueError('layer snapshot requires group ancestors')
            chain.append(ancestor)
            ancestor = parents[ancestor]
        container = local
        for ancestor in reversed(chain):
            attrs = dict(ancestor.attrib)
            attrs.pop('transform', None)
            attrs.pop('id', None)
            container = ET.SubElement(container, NS+'g', attrs)
        painted = copy.deepcopy(node)
        painted.attrib.pop('transform', None)
        painted.set('style', painted.get('style', '')+';filter:none;opacity:1')
        container.append(painted)
        png = rasterize(ET.tostring(local), w, h)
        image = skia.Image.MakeFromEncoded(png)
        if image is None or (image.width(), image.height()) != (w, h):
            raise ValueError('incorrect layer source size')
        surface = skia.Surface.MakeRaster(info(w, h))
        surface.getCanvas().clear(0)
        surface.getCanvas().drawImage(image, 0, 0)
        result = graph(f, surface.makeImageSnapshot(), scale, qx-left, qy-top)
        # This is the final snapshot boundary; PNG serialization is appropriate.
        pixels = result.toarray(colorType=skia.kRGBA_8888_ColorType, alphaType=skia.kUnpremul_AlphaType)
        snapshot = skia.Image.fromarray(pixels, colorType=skia.kRGBA_8888_ColorType, alphaType=skia.kUnpremul_AlphaType)
        encoded = snapshot.encodeToData()
        if encoded is None:
            raise ValueError('could not encode the filtered layer')
        attrs = {HREF: 'data:image/png;base64,'+base64.b64encode(bytes(encoded)).decode(),
                 'x': str(ux), 'y': str(uy), 'width': str(w/scale), 'height': str(h/scale),
                 'preserveAspectRatio': 'none', 'opacity': p.get('opacity', '1')}
        if node.get('transform'):
            attrs['transform'] = node.get('transform')
        parent = parents[node]
        index = list(parent).index(node)
        parent.remove(node)
        parent.insert(index, ET.Element(NS+'image', attrs))
        count += 1
    if not count:
        raise ValueError('no eligible uniform layer')
    return ET.tostring(root)


def check():
    version = check_float()
    surface = skia.Surface.MakeRaster(info(96, 96))
    surface.getCanvas().clear(skia.ColorRED)
    source = surface.makeImageSnapshot()

    def evaluate(markup, scale=1, tx=0, ty=0, image=source):
        f = ET.fromstring('<filter xmlns="http://www.w3.org/2000/svg">'+markup+'</filter>')
        result = graph(f, image, scale, tx, ty)
        return result.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)

    def alpha_matrix(gain, bias=0, **attributes):
        values = IDENTITY.copy()
        values[18:20] = [gain, bias]
        attrs = ' '.join(k+'="'+v+'"' for k, v in attributes.items())
        return '<feColorMatrix '+attrs+' values="'+' '.join(map(str, values))+'"/>'

    for level in [.5002, .5005, .5008]:
        for sigma in [1.5, 2, 3, 7.52]:
            markup = alpha_matrix(level)+f'<feGaussianBlur stdDeviation="{sigma}"/>'
            markup += '<feConvolveMatrix order="1" kernelMatrix="1"/>'+alpha_matrix(1000, -500)
            if abs(float(evaluate(markup)[48, 48, 3])-(1000*level-500)) > .002:
                raise ValueError('layer stage rounded fractional alpha')
    for u, v in [(.5002, .5008), (.00123, .99933), (.12345, .65432)]:
        start = alpha_matrix(u, result='a')+alpha_matrix(v, **{'in': 'SourceGraphic', 'result': 'b'})
        for op, expected in [('in', u*v), ('arithmetic', .3*u*v+.2*u+.1*v+.01)]:
            markup = start+f'<feComposite in="a" in2="b" operator="{op}" k1=".3" k2=".2" k3=".1" k4=".01"/>'
            actual = float(evaluate(markup)[48, 48, 3])
            if abs(actual-expected) > 1e-6:
                raise ValueError('layer atlas changed the binary inputs')
    for scale in [1, 2, 1.5]:
        noise = f'<feTurbulence type="fractalNoise" baseFrequency="{scale}" seed="7" numOctaves="7"/>'
        pixels = evaluate(noise, scale, .5, .5)
        for x, y in [(9, 9), (12, 12), (15, 15)]:
            if any(abs(float(v)-.5) > .0005 for v in pixels[y, x]):
                raise ValueError('layer noise phase changed')
    surface.getCanvas().clear(0)
    surface.getCanvas().drawRect(skia.Rect.MakeXYWH(48, 48, 1, 1), skia.Paint(Color4f=skia.Color4f(1, 0, 0, .25)))
    pixels = evaluate('<feConvolveMatrix kernelMatrix="3 0 0 0 2 0 0 0 -1" divisor="2"/>', image=surface.makeImageSnapshot())
    for y in range(96):
        for x in range(96):
            expected = .375 if (x, y) == (47, 47) else .25 if (x, y) == (48, 48) else 0
            if abs(float(pixels[y, x, 3])-expected) > 1e-6:
                raise ValueError('layer convolution changed kernel orientation or gain')
    # Negative surface heights occur in the selected layers. Their planar
    # normal is known independently of any filter implementation.
    for scale in [1, 2]:
        size = 96*scale
        plane = skia.Surface.MakeRaster(info(size, size))
        shader = skia.GradientShader.MakeLinear([skia.Point(16*scale, 0), skia.Point(80*scale, 0)], [0x00ffffff, skia.ColorWHITE])
        plane.getCanvas().drawPaint(skia.Paint(Shader=shader))
        lx = math.cos(math.radians(30))*math.cos(math.radians(50))
        lz = math.sin(math.radians(50))
        for kind in ['Specular', 'Diffuse']:
            markup = f'<fe{kind}Lighting surfaceScale="-16" specularExponent="8"><feDistantLight azimuth="30" elevation="50"/></fe{kind}Lighting>'
            if kind == 'Diffuse':
                markup = markup.replace(' specularExponent="8"', '')
            expected = ((.5*lx+lz+1)/math.sqrt(1.25*(2+2*lz)))**8 if kind == 'Specular' else (.5*lx+lz)/math.sqrt(1.25)
            actual = evaluate(markup, scale, image=plane.makeImageSnapshot())
            if abs(float(actual[48*scale, 48*scale, 3 if kind == 'Specular' else 0])-expected) > .001:
                raise ValueError('layer lighting differs from the calculated negative plane')
    return version
