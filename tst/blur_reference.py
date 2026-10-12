"""Separable SVG box-approximation blur for a tail of solid rectangles.

Chromium supplies the unchanged drawing prefix. The rectangular source coverage,
blur and final source-over operations retain float precision until PNG export.
Kernel counts use the distribution of three independent discrete uniforms,
independently of the native renderer's sliding windows and browser sigma limits.
https://www.w3.org/TR/SVG11/filters.html#feGaussianBlurElement
"""
import math
import re
import xml.etree.ElementTree as ET
from fractions import Fraction

import numpy as np

from noise_reference import NS, properties


def number(node, name, default):
    value = float(node.get(name, default))
    if not math.isfinite(value):
        raise ValueError('non-finite rectangle blur parameter')
    return value


def kernel(sigma):
    if not math.isfinite(sigma) or sigma < 2:
        raise ValueError('rectangle blur requires sigma >= 2 device pixels')
    window = math.floor(sigma*3*math.sqrt(2*math.pi)/4+.5)
    if window > 65536:
        raise ValueError('rectangle blur kernel is too large')
    lengths = [window, window, window if window % 2 else window+1]
    index = np.arange(sum(lengths)-2, dtype=np.int64)
    counts = np.zeros_like(index)
    # Inclusion-exclusion counts triples with sum=index and 0 <= n_i < L_i.
    for subset in range(8):
        remaining = index-sum(lengths[i] for i in range(3) if subset & (1 << i))
        term = np.where(remaining >= 0, (remaining+1)*(remaining+2)//2, 0)
        counts += (-1 if subset.bit_count() % 2 else 1)*term
    return counts.astype(np.float64)/math.prod(lengths)


def axis(left, right, extent, sigma):
    if not all(math.isfinite(v) for v in [left,right]) or right <= left or extent <= 0:
        raise ValueError('invalid rectangle blur interval')
    weights = kernel(sigma)
    reach = (len(weights)-1)//2
    cumulative = np.concatenate(([0.], np.cumsum(weights)))

    def step(edge):
        boundary = math.floor(edge)
        indices = np.arange(extent)-boundary+reach
        full = cumulative[-1]-cumulative[np.clip(indices+1, 0, len(weights))]
        partial = weights[np.clip(indices, 0, len(weights)-1)]
        partial = np.where((indices >= 0) & (indices < len(weights)), partial, 0)
        return full+(edge-boundary)*partial

    # Pixel coverage of a finite interval is the difference of two half-lines.
    return np.clip(step(right)-step(left), 0, 1)


def prepare(data, width, height):
    root = ET.fromstring(data)
    box = [float(v) for v in re.split(r'[,\s]+', root.get('viewBox', '').strip()) if v]
    if (len(box) != 4 or not all(math.isfinite(v) for v in box) or min(box[2:]) <= 0
            or min(width,height) <= 0 or root.get('preserveAspectRatio', 'xMidYMid meet') not in ['xMidYMid', 'xMidYMid meet']
            or list(root.iter(NS+'style')) or any(n is not root and n.tag == NS+'svg' for n in root.iter())):
        raise ValueError('rectangle blur requires a plain finite meet viewport')
    scale = min(width/box[2], height/box[3])
    ox, oy = (width-box[2]*scale)/2-box[0]*scale, (height-box[3]*scale)/2-box[1]*scale
    parents = {c:n for n in root.iter() for c in n}
    filters = {f.get('id'):f for f in root.iter(NS+'filter')}
    targets = [n for n in root.iter(NS+'rect') if properties(n).get('filter', 'none') != 'none']
    if not targets or any(parents[n] is not parents[targets[0]] for n in targets):
        raise ValueError('rectangle blur requires one consecutive tail of filtered rectangles')
    parent = parents[targets[0]]
    if list(parent)[list(parent).index(targets[0]):] != targets:
        raise ValueError('rectangle blur would change the drawing order')
    ancestor = parent
    while True:
        p = properties(ancestor)
        if (ancestor.tag not in [NS+'svg', NS+'g']
                or any(p.get(k, default) != default for k,default in [('filter','none'), ('clip-path','none'),
                       ('mask','none'), ('opacity','1'), ('fill-opacity','1'), ('mix-blend-mode','normal'),
                       ('display','inline'), ('visibility','visible'), ('stroke','none')]) or p.get('transform', '') not in ['', 'none']):
            raise ValueError('rectangle blur has an unsupported ancestor context')
        if ancestor is root:
            break
        container = parents[ancestor]
        if any(n.tag.startswith(NS) and n.tag not in [NS+'defs', NS+'title', NS+'desc', NS+'metadata']
               for n in list(container)[list(container).index(ancestor)+1:]):
            raise ValueError('rectangle blur has later painted siblings')
        ancestor = container
    layers = []
    for node in targets:
        p = properties(node)
        match = re.fullmatch(r'url\(#([^)]*)\)', p.get('filter', ''))
        f = filters.get(match[1]) if match else None
        style = properties(ET.Element('paint', {'style':node.get('style','')}))
        if (len(node) or any(k in style for k in ['x','y','width','height','rx','ry','transform','transform-origin','transform-box'])
                or p.get('transform', '') not in ['', 'none'] or any('!' in v for v in p.values())
                or not re.fullmatch(r'#[0-9a-fA-F]{6}', p.get('fill', ''))
                or any(p.get(k, default) != default for k,default in [('stroke','none'), ('clip-path','none'),
                       ('mask','none'), ('mix-blend-mode','normal'), ('display','inline'), ('visibility','visible')])
                or number(node,'rx','0') != 0 or number(node,'ry','0') != 0):
            raise ValueError('rectangle blur requires plain explicitly filled rectangles')
        allowed = {'id','x','y','width','height','filterUnits','primitiveUnits','style','color-interpolation-filters'}
        if (f is None or len(f) != 1 or f[0].tag != NS+'feGaussianBlur' or len(f[0])
                or any(k not in allowed and not k.startswith('{') for k in f.attrib)
                or set(properties(ET.Element('paint', {'style':f.get('style','')})))-{'style','color-interpolation-filters'}
                or f.get('filterUnits','objectBoundingBox') != 'objectBoundingBox'
                or f.get('primitiveUnits','userSpaceOnUse') != 'userSpaceOnUse'
                or properties(f).get('color-interpolation-filters') != 'sRGB'
                or f[0].get('in','SourceGraphic') != 'SourceGraphic'
                or any(k not in ['id','stdDeviation','in','result'] and not k.startswith('{') for k in f[0].attrib)):
            raise ValueError('rectangle blur requires one sRGB Gaussian primitive')
        deviations = [float(v) for v in re.split(r'[,\s]+', f[0].get('stdDeviation','0').strip()) if v]
        if len(deviations) not in [1,2] or any(not math.isfinite(v) or v*scale < 2 for v in deviations):
            raise ValueError('rectangle blur requires positive wide deviations')
        x,y,w,h = [number(node,k,'0') for k in ['x','y','width','height']]
        if min(w,h) <= 0:
            raise ValueError('rectangle blur source is empty')
        fx,fy = x+number(f,'x','-.1')*w, y+number(f,'y','-.1')*h
        fw,fh = number(f,'width','1.2')*w, number(f,'height','1.2')*h
        if fx > x or fy > y or fx+fw < x+w or fy+fh < y+h:
            raise ValueError('rectangle blur region must contain its source')
        material = ET.Element('paint',p)
        opacity, fill = number(material,'opacity','1'), number(material,'fill-opacity','1')
        if not 0 <= opacity <= 1 or not 0 <= fill <= 1:
            raise ValueError('invalid rectangle blur opacity')
        layers.append({'box': [x*scale+ox,y*scale+oy,(x+w)*scale+ox,(y+h)*scale+oy],
                       'sigma': [deviations[0]*scale,deviations[-1]*scale], 'alpha':opacity*fill,
                       'color': [int(p['fill'][i:i+2],16)/255 for i in [1,3,5]],
                       'clip': [math.floor(fx*scale+ox),math.floor(fy*scale+oy),
                                math.ceil((fx+fw)*scale+ox),math.ceil((fy+fh)*scale+oy)]})
        parent.remove(node)
    return ET.tostring(root), layers


def over(pixels, alpha, color):
    pixels[..., :3] = np.asarray(color)*alpha[..., None]+pixels[..., :3]*(1-alpha[..., None])
    pixels[..., 3] = alpha+pixels[..., 3]*(1-alpha)


def render(data, width, height, rasterize):
    prefix, layers = prepare(data, width, height)
    raw = rasterize(prefix, width, height)
    if len(raw) != width*height*4:
        raise ValueError('incorrect rectangle blur prefix size')
    pixels = np.frombuffer(raw, dtype=np.uint8).reshape(height,width,4).astype(np.float64)/255
    pixels[..., :3] *= pixels[..., 3:4]
    for layer in layers:
        x,y,right,bottom = layer['box']
        sx,sy = layer['sigma']
        horizontal, vertical = axis(x,right,width,sx), axis(y,bottom,height,sy)
        left,top,right,bottom = layer['clip']
        horizontal[(np.arange(width) < left) | (np.arange(width) >= right)] = 0
        vertical[(np.arange(height) < top) | (np.arange(height) >= bottom)] = 0
        alpha = vertical[:,None]*horizontal[None,:]*layer['alpha']
        over(pixels, alpha, layer['color'])
    pixels[..., :3] = np.divide(pixels[..., :3], pixels[..., 3:4], out=np.zeros_like(pixels[..., :3]), where=pixels[..., 3:4] > 0)
    return np.clip(np.floor(pixels*255+.5),0,255).astype(np.uint8).tobytes()


def probes():
    colors = ['#f358f4','#fc5818','#ef6e0c','#efe70c','#ef1212','#ef0e12']
    for alpha, background in [(a,b) for a in [.66500005, .001, .125] for b in [0,1]]:
        for count in [1,6,24]:
            data = ('<svg xmlns="http://www.w3.org/2000/svg" width="64" height="48" viewBox="0 0 64 48">'
                    '<defs><filter id="f" color-interpolation-filters="sRGB"><feGaussianBlur stdDeviation="2 3"/></filter></defs>'
                    f'<path fill="#115bcb" opacity="{background}" d="M0 0h64v48H0z"/>')
            data += ''.join(f'<rect x="-128" y="-128" width="320" height="304" fill="{colors[i%6]}" opacity="{alpha}" '
                            'filter="url(#f)"/>' for i in range(count))
            yield f'{alpha}/{background}/{count}', (data+'</svg>').encode(), colors, alpha, count, background


def expected_color(colors, alpha, count, background=1):
    opacity = Fraction(background)
    rgb = [Fraction(v,255)*opacity for v in [17,91,203]]
    a = Fraction(str(alpha))
    for i in range(count):
        color = [int(colors[i%len(colors)][j:j+2],16) for j in [1,3,5]]
        rgb = [Fraction(c,255)*a+v*(1-a) for c,v in zip(color,rgb)]
        opacity = a+opacity*(1-a)
    straight = [v/opacity if opacity else 0 for v in rgb]+[opacity]
    return bytes(math.floor(v*255+Fraction(1,2)) for v in straight)


def check():
    for sigma in [2,2.5,3,4,7.52,135,532,621.552034857,1000]:
        weights = kernel(sigma)
        d = math.floor(sigma*3*math.sqrt(2*math.pi)/4+.5)
        lengths = [d,d,d if d%2 else d+1]
        reach = (len(weights)-1)//2
        positions = np.arange(-reach,reach+1,dtype=np.float64)
        variance = sum((length*length-1)/12 for length in lengths)
        if (np.min(weights) < 0 or abs(np.sum(weights)-1) > 1e-12
                or np.max(abs(weights-weights[::-1])) > 1e-15
                or abs(np.sum(weights*positions)) > 1e-10
                or abs(np.sum(weights*positions*positions)-variance) > 1e-7):
            raise ValueError('rectangle blur lost kernel mass, symmetry or variance')
        if d < 20:
            enumerated = np.ones(1)
            for length in lengths:
                enumerated = np.convolve(enumerated, np.ones(length)/length)
            if np.max(abs(weights-enumerated)) > 1e-15:
                raise ValueError('rectangle blur triple counts differ from direct convolution')
        for phase in [0,.125,.371]:
            left,right,extent = reach+16+phase,reach+39.75+phase,2*reach+64
            values = axis(left,right,extent,sigma)
            if abs(np.sum(values)-(right-left)) > 1e-8:
                raise ValueError('rectangle blur changed the fractional source area')
            # Literal source samples and direct convolution independently check
            # the cumulative half-line integration and its pixel-centre phase.
            if d < 20:
                x = np.arange(extent)
                source = np.maximum(0,np.minimum(x+1,right)-np.maximum(x,left))
                direct = np.convolve(source,weights)[reach:reach+extent]
                if np.max(abs(values-direct)) > 1e-12:
                    raise ValueError('rectangle blur changed the source coverage phase')
    for alpha, background in [(a,b) for a in ['.001','.125','.66500005'] for b in [Fraction(0),Fraction(1,3),Fraction(1)]]:
        for count in [1,6,24]:
            values = np.array([[[Fraction(17,255)*background,Fraction(91,255)*background,
                                Fraction(203,255)*background,background]]],dtype=np.float64)
            colors = ['#f358f4','#fc5818','#ef6e0c','#efe70c','#ef1212','#ef0e12']
            for i in range(count):
                color = [int(colors[i%6][j:j+2],16)/255 for j in [1,3,5]]
                over(values,np.array([[float(alpha)]]),color)
            values[...,:3] /= values[...,3:4]
            actual = np.floor(values*255+.5).astype(np.uint8).tobytes()
            if actual != expected_color(colors,alpha,count,background):
                raise ValueError('rectangle composition differs from rational source-over')
    return 'numpy '+np.__version__
