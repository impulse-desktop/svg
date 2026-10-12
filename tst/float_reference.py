"""Float filter frames for restricted Inkscape source snapshots.

Input is the temporary SVG from source_geometry_reference.source(), including
its explicit vector filter region and noise-frame correction. No native pixels
are used. Noise is a recorded shader, since ImageFilters.Image rounds F32 inputs
to bytes before filtering. The shared staged graph retains fractional alpha
and uses the SVG lighting equations without approximate powers.
"""
import base64
import importlib.metadata
import math
import re
import xml.etree.ElementTree as ET

import skia

from halfturn_reference import matrix
from noise_reference import NS, properties
from source_geometry_reference import HREF

IDENTITY = [1,0,0,0,0, 0,1,0,0,0, 0,0,1,0,0, 0,0,0,1,0]


def number(node, key, default):
    value = float(node.get(key, default))
    if not math.isfinite(value):
        raise ValueError('non-finite float filter parameter')
    return value


def pair(node, key, default):
    values = [float(v) for v in node.get(key, default).replace(',', ' ').split()]
    if len(values) not in [1, 2] or not all(math.isfinite(v) and v >= 0 for v in values):
        raise ValueError('invalid float filter pair')
    return values[0], values[-1]


def info(width, height):
    return skia.ImageInfo.Make(width, height, skia.kRGBA_F32_ColorType,
                               skia.kPremul_AlphaType, skia.ColorSpace.MakeSRGB())


def filtered(source, effect, bounds):
    subset, offset = skia.IRect(), skia.IPoint(0, 0)
    image = source.makeWithFilter(None, effect, bounds, bounds, subset, offset)
    if image is None or image.colorType() != skia.kRGBA_F32_ColorType:
        raise ValueError('Skia did not produce a float filter result')
    surface = skia.Surface.MakeRaster(info(bounds.width(), bounds.height()))
    canvas = surface.getCanvas()
    canvas.clear(0)
    canvas.drawImageRect(image, skia.Rect(subset),
                         skia.Rect.MakeXYWH(offset.x(), offset.y(), subset.width(), subset.height()))
    return surface.makeImageSnapshot()


def blur(sx, sy, source, crop):
    # SVG 1.1's three-box Gaussian approximation. Use stock convolutions so
    # selecting float storage does not also select Skia's different blur kernel.
    stages = []
    for vertical, sigma in [(False, sx), (True, sy)]:
        if sigma <= 0:
            continue
        if sigma < 2:
            radius = math.ceil(3 * sigma)
            weights = [math.exp(-i*i/(2*sigma*sigma)) for i in range(-radius, radius+1)]
            stages.append((vertical, weights, 1/sum(weights), radius))
        else:
            size = math.floor(sigma * 3 * math.sqrt(2*math.pi) / 4 + .5)
            boxes = ([(size, size//2)] * 3 if size % 2 else
                     [(size, size//2), (size, size//2-1), (size+1, size//2)])
            for length, offset in boxes:
                stages.append((vertical, [1.] * length, 1/length, offset))
    for i, (vertical, weights, gain, offset) in enumerate(stages):
        size = skia.ISize(1, len(weights)) if vertical else skia.ISize(len(weights), 1)
        origin = skia.IPoint(0, offset) if vertical else skia.IPoint(offset, 0)
        source = skia.ImageFilters.MatrixConvolution(size, weights, gain, 0, origin,
                 skia.TileMode.kDecal, True, source, crop if i == len(stages)-1 else None)
    if not stages:
        source = skia.ImageFilters.ColorFilter(skia.ColorFilters.Matrix(IDENTITY), source, crop)
    return source


def render(data, width, height):
    root = ET.fromstring(data)
    filters, images = list(root.iter(NS+'filter')), list(root.iter(NS+'image'))
    if len(filters) != 1 or len(images) != 1 or 'viewBox' in root.attrib:
        raise ValueError('float filters require one normalized source snapshot')
    f, image_node = filters[0], images[0]
    if (f.get('filterUnits') != 'userSpaceOnUse' or f.get('primitiveUnits', 'userSpaceOnUse') != 'userSpaceOnUse'
            or properties(f).get('color-interpolation-filters') != 'sRGB'):
        raise ValueError('float filters require an explicit sRGB region in user space')
    sx, sy = width/number(root, 'width', '0'), height/number(root, 'height', '0')
    parents = {c: n for n in root.iter() for c in n}
    frames = [n for n in root.iter(NS+'g') if properties(n).get('filter') == 'url(#'+f.get('id')+')']
    if len(frames) != 1:
        raise ValueError('float filter frame is not unique')
    tx, ty, node = 0., 0., frames[0]
    while node is not root:
        a, b, c, d, x, y = matrix(node.get('transform', ''))
        if (a, b, c, d) != (1, 0, 0, 1):
            raise ValueError('float filter frames must be translated')
        tx += x
        ty += y
        node = parents[node]
    ix, iy, node, in_frame = number(image_node, 'x', '0'), number(image_node, 'y', '0'), parents[image_node], False
    while node is not root:
        in_frame = in_frame or node is frames[0]
        a, b, c, d, x, y = matrix(node.get('transform', ''))
        if (a, b, c, d) != (1, 0, 0, 1):
            raise ValueError('float snapshot must be translated')
        ix += x
        iy += y
        node = parents[node]
    if (not in_frame or abs(ix) > 1e-8 or abs(iy) > 1e-8
            or image_node.get('preserveAspectRatio') != 'none'
            or number(image_node, 'width', '0') != number(root, 'width', '0')
            or number(image_node, 'height', '0') != number(root, 'height', '0')):
        raise ValueError('float snapshot does not cover the viewport grid')
    x, y = sx*(tx+number(f, 'x', '0')), sy*(ty+number(f, 'y', '0'))
    right, bottom = x+sx*number(f, 'width', '0'), y+sy*number(f, 'height', '0')
    margin = max(1, -x, -y, right-width, bottom-height)
    pad = 2 ** math.ceil(math.log2(margin))
    fw, fh = width+2*pad, height+2*pad
    if sx <= 0 or sy <= 0 or right <= x or bottom <= y or fw*fh > 16*width*height:
        raise ValueError('unsupported float filter extent')
    crop = skia.IRect.MakeLTRB(math.floor(pad+x), math.floor(pad+y), math.ceil(pad+right), math.ceil(pad+bottom))
    uri = image_node.get(HREF, '')
    if not uri.startswith('data:image/png;base64,'):
        raise ValueError('float filter source must be an embedded PNG')
    encoded = base64.b64decode(uri.split(',', 1)[1], validate=True)
    from coverage_reference import source as coverage_source
    image = coverage_source(encoded, width, height)
    surface = skia.Surface.MakeRaster(info(fw, fh))
    canvas = surface.getCanvas()
    canvas.clear(0)
    canvas.drawImage(image, pad, pad)
    source = surface.makeImageSnapshot()
    # The normalized source frame already includes the noise half-unit
    # correction. Undo that addition here: graph() applies its own phase.
    # Crop into a zero-origin primary source so every primitive retains F32,
    # including the independent SVG lighting equations used by other layers.
    from layer_reference import graph
    for node in f:
        if (node.tag in [NS+'feSpecularLighting', NS+'feDiffuseLighting']
                and (len(node) != 1 or node[0].tag != NS+'feDistantLight')):
            raise ValueError('full-page float lighting requires a distant light')
    result = graph(f, source.makeSubset(crop), sx,
                   pad+sx*tx-crop.left()-.5*sx, pad+sy*ty-crop.top()-.5*sy, sy)
    viewport = skia.Surface.MakeRaster(info(width, height))
    viewport.getCanvas().clear(0)
    viewport.getCanvas().drawImage(result, crop.left()-pad, crop.top()-pad)
    return viewport.makeImageSnapshot().toarray(colorType=skia.kRGBA_8888_ColorType, alphaType=skia.kUnpremul_AlphaType).tobytes()


def check():
    bounds = skia.IRect.MakeWH(96, 96)
    surface = skia.Surface.MakeRaster(info(96, 96))
    surface.getCanvas().clear(skia.ColorRED)
    source = surface.makeImageSnapshot()
    identity = IDENTITY
    for level in [.5002, .5005, .5008]:
        for sigma in [1.5, 2, 3, 7.52]:
            m = identity.copy()
            m[18] = level
            first = skia.ImageFilters.ColorFilter(skia.ColorFilters.Matrix(m), None, bounds)
            first = blur(sigma, sigma, first, bounds)
            m[18], m[19] = 1000, -500
            effect = skia.ImageFilters.ColorFilter(skia.ColorFilters.Matrix(m), first, bounds)
            image = filtered(source, effect, bounds)
            array = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
            if abs(float(array[48, 48, 3]) - (1000*level-500)) > .002:
                raise ValueError('Skia rounded intermediate alpha or changed the blur kernel')
    surface.getCanvas().clear(0)
    surface.getCanvas().drawRect(skia.Rect.MakeXYWH(48, 48, 1, 1), skia.Paint(Color=skia.ColorRED))
    impulse = surface.makeImageSnapshot()
    for sigma in [1.5, 2, 3, 7.52]:
        image = filtered(impulse, blur(sigma, sigma, None, bounds), bounds)
        array = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
        weights = array[:, :, 3].sum(axis=0)
        mass = float(weights.sum())
        centroid = sum(float(v)*(x-48) for x, v in enumerate(weights))
        variance = sum(float(v)*(x-48)**2 for x, v in enumerate(weights))
        if sigma < 2:
            radius = math.ceil(3*sigma)
            kernel = [(x, math.exp(-x*x/(2*sigma*sigma))) for x in range(-radius, radius+1)]
            expected_variance = sum(x*x*v for x, v in kernel)/sum(v for _, v in kernel)
        else:
            size = math.floor(sigma * 3 * math.sqrt(2*math.pi) / 4 + .5)
            expected_variance = ((3*(size*size-1)) if size % 2 else (2*(size*size-1)+(size+1)**2-1))/12
        if abs(mass-1) > 1e-5 or abs(centroid) > 1e-4 or abs(variance-expected_variance) > .001:
            raise ValueError('Skia float blur changed its mass, alignment or variance')
    for sx, sy in [(1, 1), (2, 2), (1.25, 1.5)]:
        recorder = skia.PictureRecorder()
        canvas = recorder.beginRecording(skia.Rect(bounds))
        canvas.scale(sx, sy)
        canvas.translate(.5+.5/sx, .5+.5/sy)
        canvas.drawPaint(skia.Paint(Shader=skia.PerlinNoiseShader.MakeFractalNoise(sx, sy, 7, 7)))
        effect = skia.ImageFilters.Picture(recorder.finishRecordingAsPicture())
        image = filtered(source, effect, bounds)
        array = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
        for x, y in [(9, 9), (12, 12), (15, 15)]:
            if any(abs(float(v)-.5) > .0005 for v in array[y, x]):
                raise ValueError('Skia float noise sampling phase changed')
    # Compare a recorded noise shader with its direct float drawing. Amplify
    # sub-byte alpha twice to catch an unnoticed byte intermediate in the graph.
    recorder = skia.PictureRecorder()
    canvas = recorder.beginRecording(skia.Rect(bounds))
    paint = skia.Paint(Shader=skia.PerlinNoiseShader.MakeFractalNoise(.09, .028, 1, 0))
    canvas.drawPaint(paint)
    picture = recorder.finishRecordingAsPicture()
    surface.getCanvas().clear(0)
    surface.getCanvas().drawPicture(picture)
    expected_image = surface.makeImageSnapshot()
    expected = expected_image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
    effect = skia.ImageFilters.Picture(picture)
    m = identity.copy()
    m[18], m[19] = .001, .4995
    effect = skia.ImageFilters.ColorFilter(skia.ColorFilters.Matrix(m), effect, bounds)
    m[18], m[19] = 1000, -499.5
    effect = skia.ImageFilters.ColorFilter(skia.ColorFilters.Matrix(m), effect, bounds)
    image = filtered(source, effect, bounds)
    actual = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
    if abs(actual[:, :, 3]-expected[:, :, 3]).max() > .0005:
        raise ValueError('Skia rounded float noise before filtering')
    # A linear alpha ramp has an analytically known SVG lighting normal.
    # Check both light types after scaling the source and the filter parameters.
    lx = math.cos(math.radians(30))*math.cos(math.radians(50))
    ly = math.sin(math.radians(30))*math.cos(math.radians(50))
    lz = math.sin(math.radians(50))
    for scale in [1, 2]:
        size = 96*scale
        ramp_surface = skia.Surface.MakeRaster(info(size, size))
        shader = skia.GradientShader.MakeLinear([skia.Point(16*scale, 0), skia.Point(80*scale, 0)],
                                                [0x00ffffff, skia.ColorWHITE])
        ramp_surface.getCanvas().drawPaint(skia.Paint(Shader=shader))
        ramp = ramp_surface.makeImageSnapshot()
        direction = skia.Point3(scale*lx, scale*ly, scale*lz)
        for specular in [False, True]:
            if specular:
                effect = skia.ImageFilters.DistantLitSpecular(direction, skia.ColorWHITE, 16*scale, 1, 8)
                expected = ((-.5*lx+lz+1)/math.sqrt(1.25*(2+2*lz)))**8
            else:
                effect = skia.ImageFilters.DistantLitDiffuse(direction, skia.ColorWHITE, 16*scale, 1)
                expected = (-.5*lx+lz)/math.sqrt(1.25)
            image = filtered(ramp, effect, skia.IRect.MakeWH(size, size))
            actual = image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kUnpremul_AlphaType)
            if abs(float(actual[48*scale, 48*scale, 3 if specular else 0])-expected) > .001:
                raise ValueError('Skia float lighting differs from the calculated planar ramp')
    return 'skia-python '+importlib.metadata.version('skia-python')
