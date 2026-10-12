#!/usr/bin/env python3
"""Check the infinite FillPaint and StrokePaint filter inputs."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def cases():
    for space in ['sRGB', 'linearRGB']:
        for source, color in [('FillPaint', [255, 0, 0, 128]), ('StrokePaint', [0, 0, 255, 64])]:
            svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64">
<filter id="f" filterUnits="userSpaceOnUse" x="8" y="8" width="48" height="48" color-interpolation-filters="{space}"><feOffset in="{source}"/></filter>
<rect x="24" y="24" width="16" height="16" fill="red" fill-opacity=".5" stroke="blue" stroke-opacity=".25" filter="url(#f)"/>
</svg>'''
            yield source + '/' + space, svg, [(12, 12, color), (32, 32, color), (4, 4, [0]*4)]
            blurred = svg.replace('<feOffset in="' + source + '"/>', '<feGaussianBlur in="' + source + '" stdDeviation="32"/>')
            yield source + '/infinite-blur/' + space, blurred, [(8, 8, color), (32, 32, color), (55, 55, color)]
    for source, prop in [('FillPaint', 'fill'), ('StrokePaint', 'stroke')]:
        for paint, definitions, samples in [
            ('none', '', [(12, 12, [0]*4)]),
            ('url(#g)', '<linearGradient id="g"><stop stop-color="red" stop-opacity=".5"/><stop offset="1" stop-color="blue"/></linearGradient>', [(12, 12, [255, 0, 0, 128]), (48, 48, [0, 0, 255, 255])]),
            ('url(#p)', '<pattern id="p" patternUnits="userSpaceOnUse" width="8" height="8"><rect width="4" height="8" fill="lime"/></pattern>', [(12, 12, [0]*4), (16, 12, [0, 255, 0, 255])]),
        ]:
            svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64"><defs>{definitions}
<filter id="f" filterUnits="userSpaceOnUse" x="8" y="8" width="48" height="48"><feOffset in="{source}"/></filter></defs>
<g {prop}="{paint}" filter="url(#f)"><rect x="24" y="24" width="16" height="16" fill="white"/></g></svg>'''
            yield source + '/' + paint, svg, samples
            if paint == 'url(#g)':
                for space in ['sRGB', 'linearRGB']:
                    blurred = svg.replace('<feOffset in="' + source + '"/>',
                                          f'<feGaussianBlur in="{source}" stdDeviation="2" color-interpolation-filters="{space}"/>')
                    yield source + '/gradient-blur/' + space, blurred, [(8, 8, [255, 0, 0, 128]), (55, 55, [0, 0, 255, 255])]


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/filter_paint.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix='svg-filter-paint-') as work:
        src, out = Path(work) / 'case.svg', Path(work) / 'case.png'
        for name, svg, samples in cases():
            src.write_text(svg)
            for side in [64, 256]:
                subprocess.run([program, 'render', str(src), str(out), '64', '64', str(side)], check=True, timeout=30)
                width, height, pixels = canon.decode(out)
                if (width, height) != (64, 64):
                    sys.exit(f'{name}: wrong dimensions {width}x{height}')
                for x, y, expected in samples:
                    offset = (y * width + x) * 4
                    actual = list(pixels[offset:offset + 4])
                    if any(abs(a - b) > 1 for a, b in zip(actual, expected)):
                        failures.append(f'{name}/{side}/{x}/{y}: got {actual}, expected {expected}')
                    checked += 1
    if failures:
        sys.exit('\n'.join(failures))
    print(f'checked {checked} filter paint samples')


if __name__ == '__main__':
    main()
