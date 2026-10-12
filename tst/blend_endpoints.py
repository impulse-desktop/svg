#!/usr/bin/env python3
"""Check nonseparable blending against achromatic and endpoint identities."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def drawing(mode, source, alpha, chromatic, filtered):
    rows = []
    backgrounds = []
    for gray in range(256):
        color = (gray, 255 - gray, gray // 2) if chromatic else (gray,) * 3
        backgrounds.append(color)
        rows.append(f'<rect y="{gray}" width="16" height="1" fill="rgb{color}"/>')
    background = ''.join(rows)
    if filtered:
        content = f'''<filter id="blend" x="0" y="0" width="1" height="1"
                             color-interpolation-filters="sRGB">
          <feFlood flood-color="rgb({source},{source},{source})" flood-opacity="{alpha}"/>
          <feBlend in2="SourceGraphic" mode="{mode}"/>
        </filter><g filter="url(#blend)">{background}</g>'''
    else:
        content = f'''{background}<rect width="16" height="256"
          fill="rgb({source},{source},{source})" opacity="{alpha}"
          style="mix-blend-mode:{mode}"/>'''
    return f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 16 256">{content}</svg>', backgrounds


def main():
    if len(sys.argv) != 2:
        sys.exit('usage: tst/blend_endpoints.py SVG')
    program = str(Path(sys.argv[1]).resolve())
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix='svg-blend-endpoints-') as temporary:
        source, output = Path(temporary) / 'case.svg', Path(temporary) / 'case.png'
        for filtered in [False, True]:
            for mode in ['hue', 'saturation', 'color', 'luminosity']:
                for foreground in [0, 128, 255]:
                    for chromatic in [False, True]:
                        # Black/white luminosity clips every backdrop to black/white.
                        # For a gray backdrop all other modes preserve its gray.
                        if chromatic and (mode != 'luminosity' or foreground == 128):
                            continue
                        for alpha in [0.4, 1]:
                            svg, backgrounds = drawing(mode, foreground, alpha, chromatic, filtered)
                            source.write_text(svg)
                            subprocess.run([program, 'render', str(source), str(output), '16', '256', '256'],
                                           check=True, timeout=30)
                            width, height, rgba = canon.decode(output)
                            if (width, height) != (16, 256):
                                sys.exit(f'wrong image size {width}x{height}')
                            for row, backdrop in enumerate(backgrounds):
                                blended = (foreground,) * 3 if mode == 'luminosity' else backdrop
                                expected = [round(b * (1-alpha) + c * alpha)
                                            for b, c in zip(backdrop, blended)] + [255]
                                offset = (row * width + width // 2) * 4
                                actual = list(rgba[offset:offset+4])
                                if any(abs(a-b) > 1 for a, b in zip(actual, expected)):
                                    failures.append(f'{"filter" if filtered else "css"}/{mode}/{foreground}/'
                                                    f'{chromatic}/{alpha}/{row}: {actual}, expected {expected}')
                                checked += 1
    if failures:
        sys.exit(f'{len(failures)} failed samples:\n' + '\n'.join(failures[:20]))
    print(f'checked {checked} blend endpoint and achromatic samples')


if __name__ == '__main__':
    main()
