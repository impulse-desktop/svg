#!/usr/bin/env python3
"""Check subpixel fill coverage against exact unions of oriented rectangles."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def path(rectangles):
    commands = []
    for x0, y0, x1, y1, direction in rectangles:
        corners = [(x0, y0), (x1, y0), (x1, y1), (x0, y1)]
        if direction < 0:
            corners.reverse()
        commands.append('M' + 'L'.join(f'{x} {y}' for x, y in corners) + 'Z')
    return ' '.join(commands)


def coverage(rectangles, rule, x, y):
    xs = sorted({x, x + 1} | {v for r in rectangles for v in [r[0], r[2]] if x < v < x + 1})
    ys = sorted({y, y + 1} | {v for r in rectangles for v in [r[1], r[3]] if y < v < y + 1})
    area = 0
    for x0, x1 in zip(xs, xs[1:]):
        for y0, y1 in zip(ys, ys[1:]):
            mx, my = (x0 + x1) / 2, (y0 + y1) / 2
            winding = sum(d for a, b, c, e, d in rectangles if a < mx < c and b < my < e)
            inside = winding % 2 != 0 if rule == 'evenodd' else winding != 0
            if inside:
                area += (x1 - x0) * (y1 - y0)
    return round(255 * area)


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/fill_winding.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    cases = []
    for edge in [16.02, 16.125, 16.5, 16.875, 16.98]:
        cases.append([(4, 4, edge, 28, 1), (edge, 4, 28, 28, 1)])
    cases.extend([
        [(4.1, 4, 4.3, 28, 1), (4.4, 4, 4.7, 28, 1)],
        [(4, 4.01, 28, 4.06, 1), (4, 4.08, 28, 4.11, 1)],
        [(4, 4, 20.25, 28, 1), (12.75, 4, 28, 28, 1)],
        [(4, 4, 28, 28, 1), (8.25, 8.75, 23.25, 23.75, 1)],
    ])
    cases.append([(4 + i * 0.6, 16.13, 4 + i * 0.6 + 0.35, 16.36, 1) for i in range(40)])
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix='svg-fill-winding-') as temporary:
        source = Path(temporary) / 'case.svg'
        output = Path(temporary) / 'case.png'
        for index, rectangles in enumerate(cases):
            for transpose in [False, True]:
                for directions in [(1, 1), (1, -1), (-1, 1), (-1, -1)]:
                    shapes = [(b, a, e, c, directions[i % 2]) if transpose else (a, b, c, e, directions[i % 2])
                              for i, (a, b, c, e, _) in enumerate(rectangles)]
                    for rule in ['nonzero', 'evenodd']:
                        source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 32 32">'
                                          f'<path d="{path(shapes)}" fill-rule="{rule}"/></svg>')
                        expected = [coverage(shapes, rule, x, y) for y in range(32) for x in range(32)]
                        for side in [32, 64]:
                            subprocess.run([program, 'render', str(source), str(output), '32', '32', str(side)],
                                           check=True, timeout=30)
                            width, height, rgba = canon.decode(output)
                            if (width, height) != (32, 32):
                                sys.exit(f'wrong image size {width}x{height}')
                            error = max(abs(a - b) for a, b in zip(rgba[3::4], expected))
                            if error > 1:
                                failures.append(f'{index}/{transpose}/{directions}/{rule}/{side}: max alpha error {error}')
                            checked += 1
    if failures:
        sys.exit(f'{len(failures)} failed cases:\n' + '\n'.join(failures[:20]))
    print(f'checked {checked} exact rectangle fill cases')


if __name__ == '__main__':
    main()
