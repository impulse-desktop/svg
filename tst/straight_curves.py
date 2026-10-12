#!/usr/bin/env python3
"""A straight cubic and a line have the same stroke, including their joins."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/straight_curves.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    transforms = ['matrix(11.360492 0 0 10.29357 -2514.3711 -2369.03237)',
                  'matrix(-11.360492 0 0 10.29357 2914.3711 -2369.03237)',
                  'matrix(0 11.360492 -10.29357 0 2769.03237 -2514.3711)']
    # The endpoint coordinates are deliberately large relative to the curve.
    # Near a stationary end, rounding a subdivided chord can change its angle.
    start, delta, finish = 'm 232.97982 232.77202 ', '16.79309 17.60655', ' l -14.83661 15.56789'
    controls = [f'c {delta} {delta} {delta}', f'c 0 0 0 0 {delta}', f'c 0 0 {delta} {delta}']
    checked, failures = 0, []
    with tempfile.TemporaryDirectory(prefix='svg-straight-curves-') as temporary:
        source, output = Path(temporary) / 'case.svg', Path(temporary) / 'case.png'

        def render(svg, size, side):
            source.write_text(svg)
            subprocess.run([program, 'render', str(source), str(output), str(size), str(size),
                            str(side)], check=True, timeout=30)
            w, h, rgba = canon.decode(output)
            if (w, h) != (size, size):
                sys.exit(f'wrong image size {w}x{h}')
            return rgba

        for transform in transforms:
            for join in ['miter', 'bevel', 'round']:
                for size, side in [(192, 192), (256, 1024)]:
                    def drawing(segment):
                        return (f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 400 400">'
                                f'<path fill="none" stroke="black" stroke-width="3.61005044" '
                                f'stroke-linejoin="{join}" transform="{transform}" '
                                f'd="{start}{segment}{finish}"/></svg>')
                    expected = render(drawing('l ' + delta), size, side)
                    for control in controls:
                        actual = render(drawing(control), size, side)
                        error = max(abs(a - b) for a, b in zip(actual[3::4], expected[3::4]))
                        if error > 1:
                            failures.append(f'{transform}/{join}/{size}/{control}: max alpha error {error}')
                        checked += 1

        # Collinearity alone does not make a cubic replaceable by its chord:
        # a closed or reversing curve can cover points beyond that chord.
        for path, point in [('M16 32 C64 32 64 32 16 32', (44, 32)),
                            ('M16 32 C80 32 80 32 32 32', (48, 32)),
                            ('M32 32 C0 32 0 32 48 32', (16, 32))]:
            rgba = render(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 64 64">'
                          f'<path d="{path}" fill="none" stroke="black" stroke-width="4"/></svg>',
                          64, 64)
            x, y = point
            if rgba[(y * 64 + x) * 4 + 3] != 255:
                failures.append(f'collinear reversal lost: {path}')
            checked += 1
    if failures:
        sys.exit(f'{len(failures)} failed cases:\n' + '\n'.join(failures[:20]))
    print(f'checked {checked} straight cubic strokes and collinear reversals')


if __name__ == '__main__':
    main()
