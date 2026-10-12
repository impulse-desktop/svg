#!/usr/bin/env python3
"""Check filled cubic bands against exact areas, including the corpus raster grid."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon
import curve_reference


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/curve_area.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    checked, failures = 0, []
    with tempfile.TemporaryDirectory(prefix='svg-curve-area-') as temporary:
        source = Path(temporary) / 'band.svg'
        output = Path(temporary) / 'band.png'
        for radius in [24, 48, 72]:
            for width in [.3, .75, 1.92, 2.88]:
                for phase in [0, .125, .37]:
                    for rule in ['nonzero', 'evenodd']:
                        svg, expected = curve_reference.drawing(radius, width, phase, rule)
                        for size, height, side in [(192, 128, 192), (256, 256, 1024), (512, 512, 1024)]:
                            # The square viewport puts both corpus output sizes on
                            # the same 1024-device-pixel grid. Compare in SVG units.
                            direct = size == 192
                            source.write_text(svg if direct else svg.replace('0 0 192 128', '0 0 256 256'))
                            scale = 1 if direct else size / 256
                            subprocess.run([program, 'render', str(source), str(output),
                                            str(size), str(height), str(side)], check=True, timeout=30)
                            w, h, rgba = canon.decode(output)
                            if (w, h) != (size, height):
                                sys.exit(f'wrong image size {w}x{h}')
                            actual = sum(rgba[3::4]) / (255 * scale * scale)
                            if abs(actual - expected) > .5:
                                failures.append(f'{radius}/{width}/{phase}/{rule}/{size}/{side}: '
                                                f'area {actual}, expected {expected}')
                            checked += 1
    if failures:
        sys.exit(f'{len(failures)} failed cases:\n' + '\n'.join(failures[:20]))
    print(f'checked {checked} exact cubic band areas, including SIDE=1024')


if __name__ == '__main__':
    main()
