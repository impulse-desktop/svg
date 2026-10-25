#!/usr/bin/env python3
"""Unit checks of gzip documents and embedded SVGZ against literal pixels."""
import base64
import gzip
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zlib

import canon


DRAWING = b'<svg xmlns="http://www.w3.org/2000/svg" width="8" height="4"><rect width="4" height="4" fill="red"/><rect x="4" width="4" height="4" fill="lime"/></svg>'
EXPECTED = (bytes([255, 0, 0, 255]) * 4 + bytes([0, 255, 0, 255]) * 4) * 4


def member_with_headers(data):
    # FEXTRA, FNAME, FCOMMENT and FHCRC, including a non-ASCII filename.
    header = b'\x1f\x8b\x08\x1e' + b'\0' * 4 + b'\0\x03'
    header += struct.pack('<H', 6) + b'XY\x02\0ab' + b'caf\xe9.svg\0test comment\0'
    header += struct.pack('<H', zlib.crc32(header) & 0xffff)
    compressor = zlib.compressobj(wbits=-15)
    return header + compressor.compress(data) + compressor.flush() + struct.pack('<II', zlib.crc32(data), len(data))


def main():
    program = str(Path(sys.argv[1]).resolve())
    with tempfile.TemporaryDirectory(prefix='svgz-check-') as temporary:
        folder = Path(temporary)
        count = 0

        def render(name, data, expected=EXPECTED):
            nonlocal count
            source = folder / name
            source.write_bytes(data)
            png = folder / 'actual.png'
            subprocess.run([program, 'render', str(source), str(png), '8', '4'], check=True, timeout=20)
            w, h, pixels = canon.decode(png)
            assert (w, h) == (8, 4) and bytes(pixels) == expected, name
            count += 1

        packed = gzip.compress(DRAWING, mtime=0)
        # Check the new behavior first, so the old renderer fails here.
        render('drawing.svgz', packed)
        render('signature.svg', packed)
        render('plain.svgz', DRAWING)
        render('plain.svg', DRAWING)
        # Compression must not change the driver's choice of drawing scale.
        for data, suffix in [(DRAWING, ''), (DRAWING.replace(b'<svg ', b'<svg transform="translate(1)" '), ' straight')]:
            paths = [folder / 'sizing.svg', folder / 'sizing.svgz']
            paths[0].write_bytes(data)
            paths[1].write_bytes(gzip.compress(data, mtime=0))
            result = subprocess.run([program, 'size', *map(str, paths)], check=True, capture_output=True, text=True)
            assert result.stdout.splitlines() == [str(p) + ' 8.0000 4.0000' + suffix for p in paths]
        for level in (0, 1, 9):
            render(f'level-{level}.svgz', gzip.compress(DRAWING, compresslevel=level, mtime=0))
        render('headers.svgz', member_with_headers(DRAWING))
        render('members.svgz', gzip.compress(DRAWING[:37], mtime=0) + gzip.compress(DRAWING[37:], mtime=0))
        render('empty-members.svgz', gzip.compress(b'', mtime=0) + packed + gzip.compress(b'', mtime=0))
        render('utf8.svgz', gzip.compress(b'\xef\xbb\xbf<!--' + 'комментарий'.encode() + b'-->' + DRAWING, mtime=0))
        long_xml = b'<!--' + b'comment ' * 20000 + b'-->' + DRAWING
        render('many-output-chunks.svgz', gzip.compress(long_xml, mtime=0))

        def embedded(data, tag='image', nested=False):
            uri = 'data:image/svg+xml;base64,' + base64.b64encode(data).decode()
            if tag == 'feImage':
                content = f'<filter id="f" x="0" y="0" width="1" height="1"><feImage href="{uri}"/></filter><rect width="8" height="4" filter="url(#f)"/>'
            else:
                content = f'<image width="8" height="4" href="{uri}"/>'
            xml = f'<svg xmlns="http://www.w3.org/2000/svg" width="8" height="4">{content}</svg>'.encode()
            return embedded(gzip.compress(xml, mtime=0)) if nested else xml

        render('embedded.svg', embedded(packed))
        render('nested.svg', embedded(packed, nested=True))
        render('compressed-outer.svgz', gzip.compress(embedded(packed), mtime=0))
        render('feimage.svg', embedded(packed, tag='feImage'))
        # An image's decompression error must leave the rest of the document intact.
        broken_image = embedded(packed[:-4]).replace(b'</svg>', b'<rect width="8" height="4" fill="blue"/></svg>')
        render('bad-image.svg', broken_image, bytes([0, 0, 255, 255]) * 32)

        bad_crc = bytearray(packed)
        bad_crc[-8] ^= 1
        bad_size = bytearray(packed)
        bad_size[-4] ^= 1
        bad_header = bytearray(member_with_headers(DRAWING))
        bad_header[10] ^= 1
        bad_method = bytearray(packed)
        bad_method[2] = 0
        bad_flags = bytearray(packed)
        bad_flags[3] |= 0x20
        invalid = [bytes(bad_crc), bytes(bad_size), bytes(bad_header), bytes(bad_method), bytes(bad_flags),
                   packed + b'junk', packed + packed[:12], gzip.compress(b'', mtime=0),
                   gzip.compress(b'<html/>', mtime=0), gzip.compress(b'<svg>', mtime=0),
                   gzip.compress(packed, mtime=0), zlib.compress(DRAWING), b'\x1f\x00', b'\x1f\x8a']
        invalid += [packed[:n] for n in range(len(packed))]
        paths = []
        for i, data in enumerate(invalid):
            path = folder / f'bad-{i}.svgz'
            path.write_bytes(data)
            paths.append(str(path))
        result = subprocess.run([program, 'size', *paths], check=True, capture_output=True, text=True, timeout=20)
        assert result.stdout.splitlines() == [path + ' ? ?' for path in paths]
        render('bad-crc.svgz', bytes(bad_crc), bytes(128))
    print(f'checked {count} exact drawings and {len(invalid)} invalid streams: gzip headers, members, chunks, signatures, UTF-8, scale classification, embedded SVGZ, CRC/size and truncation')


if __name__ == '__main__':
    main()
