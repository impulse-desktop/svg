"""Average supersampled source coverage directly into premultiplied F32.

Decode straight PNG channels without an 8-bit premultiplication round trip.
Integer block sums retain both fractional coverage and color before filtering.
"""
import xml.etree.ElementTree as ET

import numpy as np
import skia


def source(encoded, width, height):
    if not encoded.startswith(b'\x89PNG\r\n\x1a\n') or min(width, height) <= 0:
        raise ValueError('source coverage requires a PNG and positive dimensions')
    try:
        codec = skia.Codec.MakeFromData(encoded)
    except RuntimeError as error:
        raise ValueError('could not decode source coverage') from error
    if codec is None:
        raise ValueError('could not decode source coverage')
    iw, ih = codec.getInfo().width(), codec.getInfo().height()
    scale = iw//width
    if scale not in [1, 2, 4, 8] or iw != width*scale or ih != height*scale:
        raise ValueError('source coverage requires an integer square sample grid up to 8x')
    pixels = np.empty((ih, iw, 4), dtype=np.uint8)
    spec = skia.ImageInfo.Make(iw, ih, skia.kRGBA_8888_ColorType, skia.kUnpremul_AlphaType)
    if codec.getPixels(spec, pixels, iw*4) != skia.Codec.kSuccess:
        raise ValueError('incomplete source coverage PNG')
    result = np.empty((height, width, 4), dtype=np.float32)
    alpha = pixels[..., 3]
    shape = height, scale, width, scale
    result[..., 3] = alpha.reshape(shape).sum(axis=(1,3), dtype=np.uint32)/(255*scale*scale)
    for channel in range(3):
        premul = pixels[..., channel].astype(np.uint16)*alpha
        result[..., channel] = premul.reshape(shape).sum(axis=(1,3), dtype=np.uint32)/(255*255*scale*scale)
    return skia.Image.fromarray(result, colorType=skia.kRGBA_F32_ColorType,
                                alphaType=skia.kPremul_AlphaType, colorSpace=skia.ColorSpace.MakeSRGB())


def check():
    from layer_reference import graph

    def encode(pixels):
        image = skia.Image.fromarray(pixels, colorType=skia.kRGBA_8888_ColorType,
                                     alphaType=skia.kUnpremul_AlphaType)
        return bytes(image.encodeToData())

    def pixels(image):
        return image.toarray(colorType=skia.kRGBA_F32_ColorType, alphaType=skia.kPremul_AlphaType)

    for scale in [1, 2, 4, 8]:
        rgba = np.full((3*scale, 4*scale, 4), [17,91,203,127], dtype=np.uint8)
        expected = [17*127/255**2, 91*127/255**2, 203*127/255**2, 127/255]
        if np.max(np.abs(pixels(source(encode(rgba), 4, 3))-expected)) > 1e-7:
            raise ValueError('source decoder rounded premultiplied color')
        if scale > 1:
            rgba[:, ::2] = [255,0,0,255]
            rgba[:, 1::2] = [0,0,255,0]
            if np.max(np.abs(pixels(source(encode(rgba), 4, 3))-[.5,0,0,.5])) > 1e-7:
                raise ValueError('source averaging mixed transparent color into coverage')
    rgba = np.full((4,4,4), [255,0,0,127], dtype=np.uint8)
    rgba.reshape(-1,4)[:9,3] = 128
    image = source(encode(rgba), 1, 1)
    f = ET.fromstring('<filter xmlns="http://www.w3.org/2000/svg">'
                     '<feColorMatrix values="1 0 0 0 0 0 1 0 0 0 0 0 1 0 0 0 0 0 1000 -500"/></filter>')
    expected = 1000*(127+9/16)/255-500
    if abs(float(pixels(graph(f, image, 1, 0, 0))[0,0,3])-expected) > .0001:
        raise ValueError('source coverage lost sub-byte alpha before filtering')
    for encoded, width, height in [(b'invalid',1,1), (encode(rgba),0,1),
                                   (encode(rgba),3,1), (encode(rgba),2,1), (encode(rgba)[:40],1,1)]:
        try:
            source(encoded, width, height)
        except ValueError:
            pass
        else:
            raise ValueError('accepted an invalid source coverage grid')
    return 'checked 13 source coverage color, precision and rejection controls'
