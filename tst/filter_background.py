#!/usr/bin/env python3
"""Check SVG 1.1 background inputs against exact flat-colour samples."""
import base64
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def drawing(body, enabled='enable-background="new"', space="sRGB", source="BackgroundImage"):
    return f'''<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" {enabled}>
  <defs><filter id="f" filterUnits="userSpaceOnUse" x="-64" y="-64" width="192" height="192"
                color-interpolation-filters="{space}">
    <feOffset in="{source}" dy="32"/>
  </filter></defs>{body}</svg>'''


RED = '<rect x="8" y="8" width="16" height="16" fill="red" fill-opacity=".5"/>'
BLUE = '<rect x="8" y="8" width="16" height="16" fill="blue" fill-opacity=".5"/>'
FILTER = '<rect x="40" y="8" width="8" height="8" fill="lime" filter="url(#f)"/>'


def cases():
    for space in ["sRGB", "linearRGB"]:
        for source, colour in [("BackgroundImage", [255, 0, 0, 128]), ("BackgroundAlpha", [0, 0, 0, 128])]:
            yield f"copy/{space}/{source}", drawing(RED + FILTER, space=space, source=source), [(12, 44, colour), (44, 44, [0]*4)]
            yield f"disabled/{space}/{source}", drawing(RED + FILTER, enabled="", space=space, source=source), [(12, 44, [0]*4), (44, 44, [0]*4)]

    yield "unknown-input", drawing(RED + FILTER, source="unknown"), [(44, 44, [0, 255, 0, 255])]
    yield "css", drawing(RED + FILTER, enabled='style="enable-background:new"'), [(12, 44, [255, 0, 0, 128])]
    yield "new-own-filter", drawing(RED + '<g enable-background="new" filter="url(#f)">' + BLUE + '</g>'), [(12, 44, [0]*4)]
    yield "nearest-scope", drawing(RED + '<g enable-background="new">' + BLUE + FILTER + '</g>'), [(12, 44, [0, 0, 255, 128])]
    yield "completed-opacity", drawing('<g opacity=".5">' + RED + '</g>' + FILTER), [(12, 44, [255, 0, 0, 64])]
    yield "ancestor-opacity", drawing(RED + '<g opacity=".5">' + BLUE + FILTER + '</g>'), [(12, 44, [85, 0, 170, 96])]
    yield "non-inherited", drawing(RED + '<g>' + FILTER + '</g>'), [(12, 44, [255, 0, 0, 128])]
    yield "explicit-inherit", drawing(RED + '<g enable-background="inherit">' + FILTER + '</g>'), [(12, 44, [0]*4)]
    for transform in ['matrix(-1 0 0 1 64 0)', 'matrix(1 .25 0 1 0 0)']:
        yield f"offscreen/{transform}", drawing(RED + f'<g transform="{transform}">' + FILTER + '</g>'), [(12, 44, [255, 0, 0, 128])]
    yield "region", drawing(RED + FILTER, enabled='enable-background="new 0 0 16 16"'), [(12, 44, [255, 0, 0, 128]), (20, 44, [0]*4)]
    yield "transformed-region", drawing(f'<g transform="translate(8)" enable-background="new 0 0 16 16">{RED}{FILTER}</g>', enabled=""), [(20, 44, [255, 0, 0, 128]), (28, 44, [0]*4)]
    for region in ['new 0 0 0 16', 'new 0 0', 'new 0 0 -1 16']:
        yield f"empty-region/{region}", drawing(RED + FILTER, enabled=f'enable-background="{region}"'), [(12, 44, [0]*4)]
    nested = f'<svg width="32" height="32" viewBox="0 0 64 64" enable-background="new 0 0 16 16">{RED}{FILTER}</svg>'
    yield "nested-viewport", drawing(nested), [(6, 22, [255, 0, 0, 128]), (10, 22, [0]*4)]
    # A filtered group's children belong to SourceGraphic, never its backdrop.
    yield "exclude-source", drawing(RED + '<g filter="url(#f)"><rect x="32" y="8" width="16" height="16" fill="blue"/></g>'), [(12, 44, [255, 0, 0, 128]), (36, 44, [0]*4)]

    # Saving a background must not isolate CSS blending on the visible canvas.
    opaque_red = '<rect x="8" y="8" width="16" height="16" fill="red"/>'
    blend = '<rect x="8" y="8" width="16" height="16" fill="blue" style="mix-blend-mode:multiply"/>'
    for outer in ["", '<g enable-background="new">']:
        body = outer + '<g enable-background="new">' + blend + FILTER + '</g>' + ('</g>' if outer else '')
        yield f"blend-through-background/{bool(outer)}", drawing(opaque_red + body), [(12, 12, [0, 0, 0, 255]), (12, 44, [0, 0, 255, 255])]
    yield "isolated-blend", drawing(opaque_red + '<g enable-background="new" style="isolation:isolate">' + blend + FILTER + '</g>'), [(12, 12, [0, 0, 255, 255]), (12, 44, [0, 0, 255, 255])]
    for enabled in ['enable-background="new"', ""]:
        nested = drawing(RED + FILTER, enabled=enabled)
        uri = 'data:image/svg+xml;base64,' + base64.b64encode(nested.encode()).decode()
        svg = drawing('<rect width="64" height="64" fill="lime"/><image href="' + uri + '" width="64" height="64"/>')
        expected = [128, 127, 0, 255] if enabled else [0, 255, 0, 255]
        yield f"embedded/{bool(enabled)}", svg, [(12, 44, expected)]
        # The outer document need not itself use a filter background.
        svg = '<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64"><image href="' + uri + '" width="64" height="64"/></svg>'
        yield f"embedded-only/{bool(enabled)}", svg, [(12, 44, [255, 0, 0, 128] if enabled else [0]*4)]


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/filter_background.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    failures = []
    checked = 0
    with tempfile.TemporaryDirectory(prefix="svg-filter-background-") as temporary:
        source = Path(temporary) / "case.svg"
        output = Path(temporary) / "case.png"
        for label, svg, samples in cases():
            source.write_text(svg)
            for side in [64, 256]:
                subprocess.run([program, "render", str(source), str(output), "64", "64", str(side)], check=True, timeout=30)
                width, height, pixels = canon.decode(output)
                if (width, height) != (64, 64):
                    sys.exit(f"{label}: wrong image size {width}x{height}")
                for x, y, expected in samples:
                    offset = (y * width + x) * 4
                    actual = list(pixels[offset:offset + 4])
                    if any(abs(a - b) > 1 for a, b in zip(actual, expected)):
                        failures.append(f"{label}/{side}/{x}/{y}: got {actual}, expected {expected}")
                    checked += 1
    if failures:
        sys.exit("\n".join(failures))
    print(f"checked {checked} filter background samples")


if __name__ == "__main__":
    main()
