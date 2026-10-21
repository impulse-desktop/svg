"""A small SVG embedding a valid PNG just past the raster decoder's old cap."""

import base64
import struct
import zlib

from unitgen import Unit


WIDTH = 16385
HEIGHT = 8192
assert WIDTH * HEIGHT > 1 << 27


def chunk(kind, payload):
    return struct.pack(">I", len(payload)) + kind + payload + struct.pack(">I", zlib.crc32(kind + payload))


row = b"\0" + b"\0\xff\0" * WIDTH
compressor = zlib.compressobj(9)
compressed = b"".join(compressor.compress(row) for _ in range(HEIGHT)) + compressor.flush()
png = b"\x89PNG\r\n\x1a\n"
png += chunk(b"IHDR", struct.pack(">IIBBBBB", WIDTH, HEIGHT, 8, 2, 0, 0, 0))
png += chunk(b"IDAT", compressed) + chunk(b"IEND", b"")
href = "data:image/png;base64," + base64.b64encode(png).decode("ascii")

unit = Unit(
    "image-large-raster", 12, 12,
    "A valid raster resource above 2^27 source pixels still paints when "
    "the SVG image is rendered at a small size.",
)
unit.cell(
    "above-old-cap", (1, 1, 10, 10),
    f'<image x="2" y="2" width="8" height="8" preserveAspectRatio="none" image-rendering="pixelated" href="{href}"/>',
    '<rect x="2" y="2" width="8" height="8" fill="lime"/>',
)
unit.write()
