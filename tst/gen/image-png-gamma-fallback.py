"""PNG gAMA transfer fallback when the viewer cannot use an ICC profile.

The expected colors come from the PNG power transfer and sRGB encoding,
not another decoder. The fixed sRGB ICC profile makes regeneration stable.
"""
import base64
import math
import struct
import zlib

from unitgen import Unit

PROFILE = base64.b64decode(
    "AAACTGxjbXMEQAAAbW50clJHQiBYWVogB+oACgAWAAcACAAfYWNzcEFQUEwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPbWAAEAAAAA0y1sY21zAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAALZGVzYwAAAQgAAAA2Y3BydAAAAUAAAABMd3RwdAAAAYwAAAAUY2hhZAAAAaAAAAAsclhZWgAAAcwAAAAUYlhZWgAAAeAAAAAUZ1hZWgAAAfQAAAAUclRSQwAAAggAAAAgZ1RSQwAAAggAAAAgYlRSQwAAAggAAAAgY2hybQAAAigAAAAkbWx1YwAAAAAAAAABAAAADGVuVVMAAAAaAAAAHABzAFIARwBCACAAYgB1AGkAbAB0AC0AaQBuAABtbHVjAAAAAAAAAAEAAAAMZW5VUwAAADAAAAAcAE4AbwAgAGMAbwBwAHkAcgBpAGcAaAB0ACwAIAB1AHMAZQAgAGYAcgBlAGUAbAB5WFlaIAAAAAAAAPbWAAEAAAAA0y1zZjMyAAAAAAABDEIAAAXe///zJQAAB5MAAP2Q///7of///aIAAAPcAADAblhZWiAAAAAAAABvoAAAOPUAAAOQWFlaIAAAAAAAACSfAAAPhAAAtsNYWVogAAAAAAAAYpcAALeHAAAY2XBhcmEAAAAAAAMAAAACZmYAAPKnAAANWQAAE9AAAApbY2hybQAAAAAAAwAAAACj1wAAVHsAAEzNAACZmgAAJmYAAA9c"
)


def chunk(kind, data):
    return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xffffffff)


def png(tags, palette=False, depth=8, gray=False, gamma=45455):
    color = 3 if palette else 4 if gray else 6
    values = (237, 128) if gray else (237, 28, 36, 128)
    pixel = bytes((0, 0)) if palette else bytes((0,)) + (struct.pack(">" + "H" * len(values), *(x * 257 for x in values)) if depth == 16 else bytes(values))
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, depth, color, 0, 0, 0))
    contents = {
        "gamma": (b"gAMA", struct.pack(">I", gamma)),
        "icc": (b"iCCP", b"sRGB\0\0" + zlib.compress(PROFILE)),
        "srgb": (b"sRGB", b"\0"),
        "cicp": (b"cICP", bytes((1, 13, 0, 1))),
        "srgb-chrm": (b"cHRM", struct.pack(">8I", 31270, 32900, 64000, 33000, 30000, 60000, 15000, 6000)),
        "other-chrm": (b"cHRM", struct.pack(">8I", 31270, 32900, 64000, 33000, 21000, 71000, 15000, 6000)),
    }
    for tag in tags:
        data += chunk(*contents[tag])
    if palette:
        data += chunk(b"PLTE", bytes((237, 28, 36))) + chunk(b"tRNS", bytes((128,)))
    return base64.b64encode(data + chunk(b"IDAT", zlib.compress(pixel)) + chunk(b"IEND", b"")).decode()


def transferred(sample, gamma):
    linear = (sample / 255) ** (100000 / gamma)
    encoded = 12.92 * linear if linear <= .0031308 else 1.055 * linear ** (1 / 2.4) - .055
    return math.floor(encoded * 255 + .5)


cases = [
    ("gamma-alone", ["gamma"], {}),
    ("icc-gamma", ["icc", "gamma"], {}),
    ("gamma-icc", ["gamma", "icc"], {}),
    ("icc-only", ["icc"], {}),
    ("srgb-over-gamma", ["gamma", "srgb"], {}),
    ("srgb-first", ["srgb", "gamma"], {}),
    ("cicp-over-gamma", ["gamma", "cicp"], {}),
    ("palette-icc", ["icc", "gamma", "srgb-chrm"], {"palette": True}),
    ("other-chrm", ["gamma", "other-chrm"], {}),
    ("wide-icc", ["gamma", "icc"], {"depth": 16}),
    ("gray-gamma", ["gamma"], {"gray": True}),
    ("linear-gamma", ["gamma"], {"gamma": 100000}),
]
unit = Unit("image-png-gamma-fallback", 64, 48,
            "PNG 3 gAMA is the transfer fallback when iCCP is not supported; cHRM does not suppress it. sRGB and cICP retain precedence. Alpha is linear and unchanged. Expected colors use the power transfer and sRGB equations.", ground=False)
for i, (name, tags, options) in enumerate(cases):
    x, y = i % 4 * 16, i // 4 * 16
    rgb = (237,) * 3 if options.get("gray") else (237, 28, 36)
    if "gamma" in tags and not {"srgb", "cicp"}.intersection(tags):
        rgb = tuple(transferred(v, options.get("gamma", 45455)) for v in rgb)
    color = "#" + "".join(f"{v:02x}" for v in rgb)
    unit.cell(name, (x, y, 16, 16),
              f'<image x="{x}" y="{y}" width="16" height="16" href="data:image/png;base64,{png(tags, **options)}"/>',
              f'<rect x="{x}" y="{y}" width="16" height="16" fill="{color}" fill-opacity="0.5019607843137255"/>')
unit.write()
