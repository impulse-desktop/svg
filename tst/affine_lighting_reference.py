"""SVG lighting on float alpha maps with independent x/y sample spacing.

The normal stencil is evaluated as separable differences and truncated weighted
averages, independently of the native renderer. Equations and edge kernels:
https://www.w3.org/TR/SVG11/filters.html#feDiffuseLightingElement
"""
import math
import re
import xml.etree.ElementTree as ET

import numpy as np
import skia

from noise_reference import NS
from float_reference import number


def lighting(node, source, sx, sy, tx, ty):
    if (min(sx, sy) <= 0 or not all(math.isfinite(v) for v in [sx, sy, tx, ty])
            or min(source.width(), source.height()) < 2):
        raise ValueError('invalid lighting sample grid')
    if (node.tag not in [NS+'feDiffuseLighting', NS+'feSpecularLighting'] or len(node) != 1
            or not re.fullmatch(r'white|#[0-9a-fA-F]{6}', node.get('lighting-color', 'white'))):
        raise ValueError('float lighting requires one explicit RGB light')
    color = node.get('lighting-color', 'white')
    rgb = np.ones(3) if color == 'white' else np.array([int(color[i:i+2], 16)/255 for i in [1,3,5]])
    light = node[0]
    attributes = {'feDistantLight': {'azimuth', 'elevation'}, 'fePointLight': {'x', 'y', 'z'}}
    kind = light.tag.removeprefix(NS)
    if kind not in attributes or len(light) or any(k not in attributes[kind] | {'id'}
            and not k.startswith('{') for k in light.attrib):
        raise ValueError('unsupported float light')
    constant = number(node, 'specularConstant' if node.tag == NS+'feSpecularLighting' else 'diffuseConstant', '1')
    exponent = number(node, 'specularExponent', '1')
    if constant < 0 or not 1 <= exponent <= 128:
        raise ValueError('invalid lighting material')
    alpha = source.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)[..., 3].astype(np.float64)
    # SVG's Sobel table is a first difference, with a 1:2:1 average in
    # the other direction. Truncated averages and one-sided differences
    # give its edge/corner kernels without a separate table of nine cases.
    def smooth(values, axis):
        weights = np.ones_like(values)*2
        result = values*2
        front, back = [slice(None)]*2, [slice(None)]*2
        front[axis], back[axis] = slice(1,None), slice(None,-1)
        front, back = tuple(front), tuple(back)
        result[front] += values[back]
        weights[front] += 1
        result[back] += values[front]
        weights[back] += 1
        return result/weights
    ss = number(node, 'surfaceScale', '1')
    nx = -ss*sx*smooth(2*np.gradient(alpha, axis=1), 0)
    ny = -ss*sy*smooth(2*np.gradient(alpha, axis=0), 1)
    norm = np.sqrt(nx*nx+ny*ny+1)
    nx, ny, nz = nx/norm, ny/norm, 1/norm
    if light.tag == NS+'feDistantLight':
        az, el = map(math.radians, [number(light, 'azimuth', '0'), number(light, 'elevation', '0')])
        lx, ly, lz = math.cos(az)*math.cos(el), math.sin(az)*math.cos(el), math.sin(el)
    else:
        y, x = np.indices(alpha.shape, dtype=np.float64)
        lx = number(light, 'x', '0')-(x+.5-tx)/sx
        ly = number(light, 'y', '0')-(y+.5-ty)/sy
        lz = number(light, 'z', '0')-ss*alpha
        length = np.sqrt(lx*lx+ly*ly+lz*lz)
        length = np.where(length == 0, 1, length)
        lx, ly, lz = lx/length, ly/length, lz/length
    specular = node.tag == NS+'feSpecularLighting'
    if specular:
        lz = lz+1
        length = np.sqrt(lx*lx+ly*ly+lz*lz)
        length = np.where(length == 0, 1, length)
        lx, ly, lz = lx/length, ly/length, lz/length
    level = np.maximum(0, nx*lx+ny*ly+nz*lz)
    if specular:
        level = level**exponent
    # Multiply each light channel before clamping. Clamping the white-light
    # intensity first would lose unsaturated channels when the material > 1.
    channels = np.clip(level[..., None]*constant*rgb, 0, 1)
    result = np.empty((*level.shape, 4), dtype=np.float32)
    result[..., :3] = channels
    result[..., 3] = np.max(channels, axis=2) if specular else 1
    return image(result)


def image(pixels):
    return skia.Image.fromarray(pixels, colorType=skia.kRGBA_F32_ColorType,
                               alphaType=skia.kPremul_AlphaType, colorSpace=skia.ColorSpace.MakeSRGB())


def check():
    from layer_reference import apply
    crop = skia.IRect.MakeWH(64, 48)
    random = np.random.default_rng(24).random((48, 64)).astype(np.float32)
    source = image(np.repeat(random[..., None], 4, axis=2))
    direction = [math.cos(math.radians(30))*math.cos(math.radians(50)),
                 math.sin(math.radians(30))*math.cos(math.radians(50)), math.sin(math.radians(50))]
    y, x = np.indices((48, 64), dtype=np.float64)
    for kind in ['Specular', 'Diffuse']:
        for ss in [-16, 0, 16]:
            for light in ['Distant', 'Point']:
                attributes = 'azimuth="30" elevation="50"' if light == 'Distant' else 'x="50" y="-20" z="80"'
                extra = ' specularExponent="8"' if kind == 'Specular' else ''
                node = ET.fromstring(f'<fe{kind}Lighting xmlns="{NS[1:-1]}" surfaceScale="{ss}"{extra}>'
                                     f'<fe{light}Light {attributes}/></fe{kind}Lighting>')
                args = [skia.Point3(*(direction if light == 'Distant' else [50, -20, 80])), skia.ColorWHITE, ss, 1]
                if kind == 'Specular':
                    args.append(8)
                stock = apply(source, getattr(skia.ImageFilters, light+'Lit'+kind)(*args, None, crop), crop)
                actual = lighting(node, source, 1, 1, 0, 0)
                def pixels(value):
                    return value.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)
                # Skia approximates pow, and clamps its Sobel boundary samples.
                # Compare interior pixels; analytic planes below cover all edges.
                if np.max(np.abs(pixels(stock)[1:-1, 1:-1]-pixels(actual)[1:-1, 1:-1])) > 1/512:
                    raise ValueError('float lighting disagrees with stock Skia on interior samples')
                for sx, sy in [(1, 1), (2, 2), (.715628, .794), (.86727, .916156), (1.25, .75)]:
                    tx, ty = 17.3, 12.7
                    u, v = (x+.5-tx)/sx, (y+.5-ty)/sy
                    alpha = .5+.003*u-.002*v
                    plane = image(np.repeat(alpha[..., None], 4, axis=2).astype(np.float32))
                    normal = np.array([-2*ss*.003, 2*ss*.002, 1])
                    normal /= np.linalg.norm(normal)
                    if light == 'Distant':
                        vector = np.broadcast_to(direction, (48, 64, 3)).copy()
                    else:
                        vector = np.stack([50-u, -20-v, 80-ss*alpha], axis=2)
                        vector /= np.linalg.norm(vector, axis=2)[..., None]
                    if kind == 'Specular':
                        vector[..., 2] += 1
                        vector /= np.linalg.norm(vector, axis=2)[..., None]
                    expected = np.maximum(0, vector @ normal)
                    if kind == 'Specular':
                        expected **= 8
                    actual = pixels(lighting(node, plane, sx, sy, tx, ty))
                    if np.max(np.abs(actual[..., 0]-expected)) > 1e-5:
                        raise ValueError('float lighting differs from an analytic plane or its edges')
                    if np.max(np.abs(actual[..., 3]-(expected if kind == 'Specular' else 1))) > 1e-5:
                        raise ValueError('float lighting alpha differs from its formula')
                    for color, rgb in [('#e9e6d7', [233,230,215]), ('#204080', [32,64,128]), ('#000000', [0,0,0])]:
                        for constant in [.5, 2, 4]:
                            colored = ET.fromstring(ET.tostring(node))
                            colored.set('lighting-color', color)
                            colored.set('specularConstant' if kind == 'Specular' else 'diffuseConstant', str(constant))
                            channels = np.minimum(expected[..., None]*constant*np.array(rgb)/255, 1)
                            actual = pixels(lighting(colored, plane, sx, sy, tx, ty))
                            alpha = np.max(channels, axis=2) if kind == 'Specular' else 1
                            if (np.max(np.abs(actual[..., :3]-channels)) > 4e-5
                                    or np.max(np.abs(actual[..., 3]-alpha)) > 4e-5):
                                raise ValueError('colored lighting differs from the analytic plane or per-channel saturation')
    return 'numpy '+np.__version__
