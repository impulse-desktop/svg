#!/usr/bin/env python3
"""Check circular strokes against exact disk/pixel intersections."""
import math
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def quadrant(x, y, radius):
    x, y = min(abs(x), radius), min(abs(y), radius)
    split = min(x, math.sqrt(max(0., radius * radius - y * y)))

    def primitive(t):
        return (t * math.sqrt(max(0., radius * radius - t * t))
                + radius * radius * math.asin(t / radius)) / 2

    return y * split + primitive(x) - primitive(split)


def integral(x, y, radius):
    return math.copysign(1, x) * math.copysign(1, y) * quadrant(x, y, radius)


def disk(x, y, cx, cy, radius):
    x0, y0 = x - cx, y - cy
    return (integral(x0 + 1, y0 + 1, radius) - integral(x0, y0 + 1, radius)
            - integral(x0 + 1, y0, radius) + integral(x0, y0, radius))


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/circle_coverage.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    checked, failures = 0, []
    with tempfile.TemporaryDirectory(prefix='svg-circle-coverage-') as temporary:
        source, output = Path(temporary) / 'ring.svg', Path(temporary) / 'ring.png'
        # The three circle radii and stroke width in openclipart/290266,
        # at its 128-pixel pattern tile scale on the corpus's device grid.
        for source_radius in [69.999992, 29.083881, 16.716486]:
            radius, width = source_radius * 128 / 140, 5 * 128 / 140
            for phase in [0, .125, .37]:
                center = 96 + phase
                exact = [max(0., min(1., disk(x, y, center, center, radius + width / 2)
                                        - disk(x, y, center, center, radius - width / 2)))
                         for y in range(192) for x in range(192)]
                if abs(sum(exact) - 2 * math.pi * radius * width) > 1e-7:
                    sys.exit('disk intersections do not conserve the exact annulus area')
                source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 192 192">'
                                  f'<circle cx="{center}" cy="{center}" r="{radius}" '
                                  f'fill="none" stroke="black" stroke-width="{width}"/></svg>')
                subprocess.run([program, 'render', str(source), str(output), '192', '192'],
                               check=True, timeout=30)
                w, h, rgba = canon.decode(output)
                if (w, h) != (192, 192):
                    sys.exit(f'wrong image size {w}x{h}')
                error = max(abs(a - 255 * b) for a, b in zip(rgba[3::4], exact))
                if error > 6:
                    failures.append(f'{source_radius}/{phase}: max alpha error {error} of 255')
                checked += 1
    if failures:
        sys.exit(f'{len(failures)} failed cases:\n' + '\n'.join(failures))
    print(f'checked {checked} circular strokes against exact pixel coverage')


if __name__ == '__main__':
    main()
