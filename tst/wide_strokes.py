#!/usr/bin/env python3
"""Check wide-stroke reference restrictions and unchanged native drawing."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon
import wide_stroke_reference as wide


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/wide_strokes.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    root = '<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256">'
    shape = ('<circle cx="128" cy="128" r="8" fill="white" stroke="red" '
             'stroke-width="6" vector-effect="non-scaling-stroke"/>')
    baseline = root + shape + '</svg>'
    negative = [
        baseline.replace('stroke-width="6"', 'stroke-width="2"'),
        baseline.replace('stroke-width="6"', 'stroke-width="2%"'),
        baseline.replace('r="8"', 'r="2"').replace('stroke-width="6"', 'stroke-width="3"'),
        baseline.replace('stroke-width="6"', 'stroke-width="1e309"'),
        baseline.replace('r="8"', 'r="NaN"'),
        baseline.replace('r="8"', 'r="80"'),
        baseline.replace('stroke="red"', 'stroke="rgba(255,0,0,.5)"'),
        baseline.replace('stroke="red"', 'stroke="url(#paint)"'),
        baseline.replace('stroke="red"', 'stroke="red" stroke-opacity=".5"'),
        baseline.replace('stroke="red"', 'stroke="red" stroke-dasharray="1 1"'),
        baseline.replace('stroke="red"', 'stroke="red" paint-order="stroke fill"'),
        baseline.replace('stroke="red"', 'stroke="red" shape-rendering="crispEdges"'),
        baseline.replace('vector-effect="non-scaling-stroke"', ''),
        root + '<g vector-effect="non-scaling-stroke">' + shape.replace('vector-effect="non-scaling-stroke"', '') + '</g></svg>',
        baseline.replace('<circle', '<circle transform="matrix(0 0 0 1 0 0)"'),
        baseline.replace('<circle', '<circle style="fill:white!important"'),
        baseline.replace('<circle', '<circle style="r:80"'),
        baseline.replace('<circle', '<circle style="transform:scale(2)"'),
        baseline.replace('<circle', '<circle transform-origin="128 128"'),
        baseline.replace('<circle', '<circle filter="url(external.svg#f)"'),
        baseline.replace('<circle', '<circle marker-start="url(external.svg#m)"'),
        baseline.replace('<circle', '<style>circle {r:80}</style><circle'),
        baseline.replace('<circle', '<use href="#x"/><circle'),
        baseline.replace('<circle', '<animate attributeName="r"/><circle'),
        baseline.replace('viewBox=', 'preserveAspectRatio="none" viewBox='),
        baseline.replace('<circle cx="128" cy="128" r="8"', '<path d="M 120 120 L 128 128 L 136 120 L 136 136 L 120 136 Z"'),
        baseline.replace('<circle cx="128" cy="128" r="8"', '<path d="M 120 120 L 136 120 L 136 136"'),
        baseline.replace('<circle cx="128" cy="128" r="8"', '<path d="m 120 120 l 16 0 l 0 16 z"'),
        baseline.replace('<circle cx="128" cy="128" r="8"', '<path d="M 128 120 L 133 136 L 120 126 L 136 126 L 123 136 Z"'),
    ]
    for i, data in enumerate(negative):
        try:
            wide.source(data.encode(), 64, 64, 1)
        except ValueError:
            continue
        sys.exit(f'unsupported case {i} was normalized')

    probes = list(wide.probes())
    # Stroke paint is inherited; vector-effect is not. Object opacity applies
    # to the completed fill and stroke together, so it preserves equivalence.
    probes.append(((root + '<g stroke="red" stroke-width="6" opacity=".5">'
                    + shape.replace('stroke="red"', '').replace('stroke-width="6"', '')
                    + '</g></svg>').encode(), 64, 64, 2))
    with tempfile.TemporaryDirectory(prefix='svg-wide-strokes-') as temporary:
        work = Path(temporary)
        for i, (data, width, height, scale) in enumerate(probes):
            normal = wide.source(data, width, height, scale)
            pixels = []
            for name, svg in [('original', data), ('normalized', normal)]:
                source = work / (name + '.svg')
                output = work / (name + '.png')
                source.write_bytes(svg)
                subprocess.run([program, 'render', str(source), str(output), str(width), str(height),
                                str(max(width, height) * scale)], check=True, timeout=30)
                pixels.append(canon.decode(output))
            if pixels[0] != pixels[1]:
                sys.exit(f'native wide-stroke case {i} changed after normalization')
    print(f'checked {len(negative)} rejected inputs and {len(probes)} unchanged native wide strokes')


if __name__ == '__main__':
    main()
