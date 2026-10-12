#!/usr/bin/env python3
"""Check input clipping and turbulence sampling analytically."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def drawing(primitives, space="sRGB", region="", viewbox="0 0 32 32"):
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="{viewbox}">
  <filter id="f" color-interpolation-filters="{space}" {region}>
    {primitives}
  </filter>
  <rect x="-32" y="-32" width="96" height="96" fill="#804020" filter="url(#f)"/>
</svg>'''


def cases():
    for space in ["sRGB", "linearRGB"]:
        # The source is clipped to [8,16] before an offset of 12. Nothing can
        # return to that region, in either filter colour space.
        region = 'filterUnits="userSpaceOnUse" x="8" y="8" width="8" height="8"'
        for source in ["SourceGraphic", "SourceAlpha"]:
            primitive = f'<feOffset in="{source}" dx="12"/>'
            yield f"clip/{space}/{source}", drawing(primitive, space, region), [0, 0, 0, 0]

    # At lattice vertices every gradient's dot product is zero. Doubling the
    # frequency for another octave stays on the lattice, regardless of seed.
    # Thus fractalNoise is exactly 1/2 and turbulence exactly 0. The viewBox
    # places device-pixel centres on those vertices, without reproducing the
    # implementation's pseudo-random generator or interpolation code.
    region = 'filterUnits="userSpaceOnUse" x="-.5" y="-.5" width="32" height="32"'
    for kind in ["fractalNoise", "turbulence"]:
        for seed in [0, 7, -20]:
            for octaves in [1, 3, 7]:
                primitive = f'<feTurbulence type="{kind}" seed="{seed}" numOctaves="{octaves}" baseFrequency="1"/>'
                expected = [128] * 4 if kind == "fractalNoise" else [0] * 4
                yield f"noise/{kind}/{seed}/{octaves}", drawing(primitive, region=region, viewbox="-.5 -.5 32 32"), expected

    # Signed axis permutations preserve the device pixel grid. Half-unit
    # translations put all inverse-transformed sample centres on noise lattice
    # vertices; rendering a separate layer must not change that sampling phase.
    transforms = [(0, 1, -1, 0, 32.5, .5), (-1, 0, 0, -1, 32.5, 32.5),
                  (-1, 0, 0, 1, 32.5, .5), (0, -1, 1, 0, .5, 32.5),
                  (0, 1, 1, 0, .5, .5)]
    for scale in [1, 2]:
        for transform in transforms:
            a, b, c, d, x, y = transform
            matrix = f"matrix({scale*a} {scale*b} {scale*c} {scale*d} {x} {y})"
            for kind in ["fractalNoise", "turbulence"]:
                for seed in [0, 7]:
                    primitive = f'<feTurbulence type="{kind}" seed="{seed}" numOctaves="7" baseFrequency="{scale}"/>'
                    svg = drawing(primitive).replace('<rect ', f'<rect transform="{matrix}" ')
                    expected = [128] * 4 if kind == "fractalNoise" else [0] * 4
                    yield f"transformed-noise/{scale}/{transform}/{kind}/{seed}", svg, expected


def periodic_drawing(kind, octaves, offset):
    # The lattice permutation repeats after 256 cells. Translating by 8192
    # units at frequency 1 must leave all octaves unchanged, on either side
    # of zero; this tests fractional cell coordinates, not only vertices.
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32"
                   viewBox="{offset} {offset} 32 32">
  <filter id="f" filterUnits="userSpaceOnUse" x="{offset}" y="{offset}"
          width="32" height="32" color-interpolation-filters="sRGB">
    <feTurbulence baseFrequency="1" numOctaves="{octaves}" type="{kind}" seed="7"/>
  </filter>
  <rect x="{offset}" y="{offset}" width="32" height="32" filter="url(#f)"/>
</svg>'''


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/filter_inputs.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    failures = []
    checked = 0
    with tempfile.TemporaryDirectory(prefix="svg-filter-inputs-") as temporary:
        source = Path(temporary) / "case.svg"
        output = Path(temporary) / "case.png"
        for label, svg, expected in cases():
            source.write_text(svg)
            subprocess.run([program, "render", str(source), str(output), "32", "32", "32"],
                           check=True, timeout=30)
            width, height, pixels = canon.decode(output)
            if (width, height) != (32, 32):
                sys.exit(f"{label}: wrong image size {width}x{height}")
            for x, y in [(9, 9), (12, 12), (15, 15)]:
                offset = (y * width + x) * 4
                actual = list(pixels[offset:offset + 4])
                if any(abs(a - b) > 1 for a, b in zip(actual, expected)):
                    failures.append(f"{label}/{x}/{y}: got {actual}, expected {expected}")
                checked += 1
        for kind in ["fractalNoise", "turbulence"]:
            for octaves in [1, 3, 7]:
                original = None
                for offset in [0, -8192, 8192]:
                    source.write_text(periodic_drawing(kind, octaves, offset))
                    subprocess.run([program, "render", str(source), str(output), "32", "32", "32"], check=True, timeout=30)
                    pixels = canon.decode(output)[2]
                    if original is None:
                        original = pixels
                    else:
                        if pixels != original:
                            failures.append(f"periodicity/{kind}/{octaves}/{offset}: translated noise changed")
                        checked += 1
    if failures:
        sys.exit("\n".join(failures))
    print(f"checked {checked} filter clipping and noise samples")


if __name__ == "__main__":
    main()
