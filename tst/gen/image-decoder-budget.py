"""The test decoder's pixel storage budget, independently of image area.

An indexed one-bit PNG expands to eight-bit RGBA. The old area limit
rejected this real-file-sized image although its buffer is smaller than
the already permitted sixteen-bit PNG buffers. Uniform colors make the
visible expectation exact under any resampling. Hidden images test natural
bounds at the storage boundary without allocating their pixel buffers.
The PNGs are valid, generated a row at a time, and highly compressible.
"""
import base64
import struct
import zlib

from unitgen import Unit


def chunk(kind, data):
    return (struct.pack('>I', len(data)) + kind + data
            + struct.pack('>I', zlib.crc32(kind + data) & 0xffffffff))


def uniform(width, height, depth=1, color=(0, 255, 0)):
    indexed = depth == 1
    header = struct.pack('>IIBBBBB', width, height, depth, 3 if indexed else 0, 0, 0, 0)
    row = b'\0' * (1 + ((width + 7) // 8 if indexed else width * 2))
    compressor = zlib.compressobj(9)
    pieces = [compressor.compress(row) for _ in range(height)]
    pieces.append(compressor.flush())
    data = (b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', header)
            + (chunk(b'PLTE', bytes(color)) if indexed else b'')
            + chunk(b'IDAT', b''.join(pieces)) + chunk(b'IEND', b''))
    return 'data:image/png;base64,' + base64.b64encode(data).decode()


large = uniform(16384, 8193)
byte_edge = uniform(24576, 16384)
too_large = uniform(24576, 16385, color=(255, 0, 0))
wide_edge = uniform(16384, 8192, 16)
wide_over = uniform(16384, 8193, 16)
small_wide = uniform(2, 2, 16)

u = Unit('image-decoder-budget', 84, 12,
         'Six lime squares and one black square. The test decoder retains '
         'its former maximum pixel-buffer allowance: 1.5 GiB, formerly '
         'reached by a 2^27-pixel sixteen-bit PNG with RGBA and wide buffers. '
         'A larger-area indexed PNG needs only its eight-bit RGBA buffer. '
         'Hidden natural-size images let a flood expose header acceptance '
         'at and beyond the budget, without reading their pixels.')
u.common('<defs><filter id="lime" x="0" y="0" width="1" height="1">'
         '<feFlood flood-color="lime"/></filter>'
         '<filter id="red" x="0" y="0" width="1" height="1">'
         '<feFlood flood-color="red"/></filter></defs>', '')


def hidden(source, width, height, flood):
    return (f'<g transform="scale({6 / width:.17g} {6 / height:.17g})" '
            f'filter="url(#{flood})"><image href="{source}" '
            'visibility="hidden"/></g>')


cases = [
    ('large-indexed', f'<image href="{large}" width="6" height="6" preserveAspectRatio="none"/>', 'lime'),
    ('large-natural-bounds', hidden(large, 16384, 8193, 'lime'), 'lime'),
    ('eight-bit-at-budget', hidden(byte_edge, 24576, 16384, 'lime'), 'lime'),
    ('eight-bit-over-budget', '<rect width="6" height="6" fill="lime"/>'
     + hidden(too_large, 24576, 16385, 'red'), 'lime'),
    ('sixteen-bit-at-budget', hidden(wide_edge, 16384, 8192, 'lime'), 'lime'),
    ('sixteen-bit-over-budget', '<rect width="6" height="6" fill="lime"/>'
     + hidden(wide_over, 16384, 8193, 'red'), 'lime'),
    ('small-sixteen-bit', f'<image href="{small_wide}" width="6" height="6"/>', 'black'),
]
for i, (name, test, color) in enumerate(cases):
    x = i * 12
    u.cell(name, (x, 0, 12, 12), f'<g transform="translate({x + 3} 3)">{test}</g>',
           f'<rect x="{x + 3}" y="3" width="6" height="6" fill="{color}"/>')
u.write()
