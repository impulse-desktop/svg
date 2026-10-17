#!/usr/bin/env python3
"""Check lighting on a planar alpha ramp under affine transforms."""
import math
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


TRANSFORMS = {
    "identity": "",
    "rotate": "matrix(0 1 -1 0 256 0)",
    "mirror": "matrix(-1 0 0 1 256 0)",
    "nonuniform": "matrix(1.25 0 0 .75 -32 32)",
    # These exceed the intermediate layer's area limit and exercise its fallback.
    "skew-x": "matrix(1 0 10 1 -1280 0)",
    "skew-y": "matrix(1 10 0 1 0 -1280)",
    "mirror-skew": "matrix(-1 0 10 1 -1024 0)",
    "rotate-skew": "matrix(0 1 -1 10 256 -1280)",
}


def drawing(kind, transform, root):
    attributes = f'transform="{transform}" transform-origin="0 0" filter="url(#light)"'
    outer = attributes if root else ""
    inner = "" if root else attributes
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="256" height="256" viewBox="0 0 256 256" {outer}>
  <defs>
    <linearGradient id="ramp" gradientUnits="userSpaceOnUse" x1="64" x2="192">
      <stop stop-color="white" stop-opacity="0"/>
      <stop offset="1" stop-color="white"/>
    </linearGradient>
    <filter id="light" color-interpolation-filters="sRGB">
      <fe{kind}Lighting surfaceScale="32" specularExponent="8">
        <feDistantLight azimuth="30" elevation="50"/>
      </fe{kind}Lighting>
    </filter>
  </defs>
  <rect x="64" y="64" width="128" height="128" fill="url(#ramp)"
        {inner}/>
</svg>'''


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/filter_transforms.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    # The SVG lighting stencil gives this plane the normal (-1/2, 0, 1).
    # Its user-space normal and distant light do not change with the transform.
    lx = math.cos(math.radians(30)) * math.cos(math.radians(50))
    lz = math.sin(math.radians(50))
    diffuse = round(255 * (-0.5 * lx + lz) / math.sqrt(1.25))
    specular = round(255 * ((-0.5 * lx + lz + 1) / math.sqrt(1.25 * (2 + 2 * lz))) ** 8)
    expected = {"Diffuse": [diffuse, diffuse, diffuse, 255], "Specular": [255, 255, 255, specular]}
    checked = 0
    failures = []
    # A filter or transform on the root is in the pixels of the box, so the
    # root is drawn straight at its own size alone, where they are its
    # user units.
    samples = [(False, 256, 1024), (False, 512, 1024), (True, 256, 256)]
    with tempfile.TemporaryDirectory(prefix="svg-filter-transforms-") as temporary:
        source = Path(temporary) / "ramp.svg"
        output = Path(temporary) / "ramp.png"
        for kind, target in expected.items():
            for name, transform in TRANSFORMS.items():
                for root, side, resolution in samples:
                    source.write_text(drawing(kind, transform, root))
                    label = f"{kind}/{name}/{'root' if root else 'shape'}/{side}/{resolution}"
                    subprocess.run([program, "render", str(source), str(output), str(side), str(side), str(resolution)],
                                   check=True, timeout=30)
                    width, height, pixels = canon.decode(output)
                    if (width, height) != (side, side):
                        sys.exit(f"{label}: wrong image size {width}x{height}")
                    offset = ((side // 2) * side + side // 2) * 4
                    actual = list(pixels[offset:offset + 4])
                    if any(abs(a - b) > 1 for a, b in zip(actual, target)):
                        failures.append(f"{label}: got {actual}, expected {target}")
                    checked += 1
    if failures:
        sys.exit("\n".join(failures))
    print(f"checked {checked} transformed lighting samples")


if __name__ == "__main__":
    main()
