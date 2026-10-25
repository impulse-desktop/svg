#!/usr/bin/env python3
"""Boundary cases entered through the CLI, checked against literal pixels."""
import base64
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zlib

import canon


SIDE = 8
GREEN = bytes((0, 255, 0, 255)) * (SIDE * SIDE)
CLEAR = bytes(4 * SIDE * SIDE)
RECT = '<rect width="8" height="8" fill="lime"/>'


def document(content, attributes=''):
    return f'<svg xmlns="http://www.w3.org/2000/svg" width="{SIDE}" height="{SIDE}" {attributes}>{content}</svg>'


def green_png():
    def chunk(kind, data):
        return struct.pack('>I', len(data)) + kind + data + struct.pack('>I', zlib.crc32(kind + data))

    pixels = (b'\0' + bytes((0, 255, 0, 255)) * 32) * 32
    png = b'\x89PNG\r\n\x1a\n' + chunk(b'IHDR', struct.pack('>IIBBBBB', 32, 32, 8, 6, 0, 0, 0))
    return 'data:image/png;base64,' + base64.b64encode(png + chunk(b'IDAT', zlib.compress(pixels)) + chunk(b'IEND', b'')).decode()


def cases():
    # The opening '?' cannot close its own processing instruction.
    # Exercise the prolog, epilog, content, DTD and entity parsers by render.
    root = f'<svg xmlns="http://www.w3.org/2000/svg" width="{SIDE}" height="{SIDE}">'
    drawing = root + RECT + '</svg>'
    for pi in ['<?', '<?>', '<?target>', '<?target?', '<?target?>', '<?target >?>', '<?target ? ?>']:
        expected = GREEN if pi.endswith('?>') and pi != '<?>' else CLEAR
        for at, source in enumerate([
            pi + drawing,
            drawing + pi,
            root + pi + RECT + '</svg>',
            '<!DOCTYPE svg [' + pi + ']>' + drawing,
            '<!DOCTYPE svg [<!ENTITY content "' + pi + '">]>' + root + '&content;' + RECT + '</svg>',
        ]):
            yield f'processing-instruction-{pi}-{at}', source.encode(), expected

    # Invalid declarations must leave the previous valid declaration intact.
    for value in ['bogus', 'calc(0)', 'calc(1 - 1)', '10%', '2', 'var(--bad)']:
        yield 'outline-offset-' + value, RECT.replace('/>', f' style="--bad:10%;outline-offset:0;outline-offset:{value}"/>'), GREEN
    for attr in ['border-top-left-radius', 'border-top-right-radius', 'border-bottom-right-radius', 'border-bottom-left-radius']:
        yield 'presentation-' + attr, RECT.replace('/>', f' {attr}="8px"/>'), GREEN
        yield 'unset-' + attr, f'<g style="{attr}:8px">' + RECT.replace('/>', f' style="{attr}:unset"/>') + '</g>', GREEN
    yield 'empty-radius-variable', RECT.replace('/>', ' style="--r:;border-radius:var(--r)"/>'), GREEN
    yield 'flat-radius', RECT.replace('/>', ' style="border-radius:4px / 0"/>'), GREEN
    yield 'root-flat-radius', document(RECT, 'style="border-radius:4px / 0"'), GREEN
    yield 'root-geometry-inherit', document('<circle style="r:inherit" fill="lime"/>', 'style="r:inherit"'), CLEAR
    yield 'instance-font-parent', '<defs><rect id="instance" style="width:inherit;--inside:var(--host)" height="8" fill="lime"/></defs><use href="#instance" style="width:1em;font-size:8px;--host:1"/>', GREEN
    yield 'instance-font-source', '<defs><g id="instance" style="width:1em;font-size:8px;fill:var(--host,lime)"><rect style="width:inherit" height="8"/></g></defs><use href="#instance" style="--host:lime"/>', GREEN

    # Inherited geometry computes font-relative lengths at the source node.
    for length in ['2ex', 'calc(1em)', 'calc(50% + 4px)']:
        yield 'inherited-' + length, f'<g style="width:{length};font-size:8px"><rect style="width:inherit;font-size:32px" height="8" fill="lime"/></g>', GREEN
    yield 'root-inherit-no-source', '<g style="r:inherit"><circle style="r:inherit" fill="lime"/></g>', CLEAR
    # The renderer's nesting limit is 256; include drawable and capped trees.
    for depth in [252, 257]:
        yield f'deep-font-{depth}', '<g font-size="8px">' + '<g>' * depth + '<g style="width:1em"><rect style="width:inherit" height="8" fill="lime"/></g>' + '</g>' * depth + '</g>', GREEN if depth == 252 else CLEAR
        yield f'deep-geometry-{depth}', '<g style="width:8px">' + '<g style="width:inherit">' * depth + '<rect style="width:inherit" height="8" fill="lime"/>' + '</g>' * (depth + 1), GREEN if depth == 252 else CLEAR

    # The four distant turns exercise culling without changing either
    # visible vertical leg. Here all legs are outside the canvas.
    for x, y in [(-100, 4), (100, 4), (4, -100), (4, 100)]:
        yield f'distant-stroke-{x}-{y}', f'<path d="M{x - 1} {y - 1}L{x} {y}L{x + 1} {y - 1}" fill="none" stroke="lime"/>', CLEAR
    yield 'zero-stroke-scale', '<path d="M0 0H8" transform="scale(0)" stroke="lime" shape-rendering="crispEdges"/>', CLEAR
    # This tiny cusp has less than half an 8-bit alpha unit of area.
    yield 'tiny-cusp-chords', '<path d="M3 3C3.125 3.125 3 3.125 3.125 3" fill="none" stroke="lime" stroke-width=".00001"/>', CLEAR
    # The closing vertical segment alone covers the canvas. A folded curve
    # elsewhere must not cancel its straight band, including short corners
    # whose two individually valid inner cuts overlap.
    yield 'fold-with-overlapping-cuts', '<path d="M0 0C0 -1.28 1.28 -1.28 1.28 0L32 0L32 32L0 32Z" fill="none" stroke="lime" stroke-width="48"/>', GREEN
    yield 'fold-with-wide-regular-arc', '<path d="M0 0C-.03 -.04 -.02 -.04 .04 0C1000000 0 1000000 1000000 0 1000000Z" fill="none" stroke="lime" stroke-width="400000"/>', GREEN
    yield 'negative-number-saturation', '<rect x="-1e300" width="8" height="8" fill="lime"/>', CLEAR
    # Closed retraces have no area. Many reversed crossings plus a finite
    # input that overflows after transformation exercise bounded sorting
    # and its NaN barriers without relying on a rasterizer as the oracle.
    for count in [80, 520]:
        retraces = ''.join(f'M{k} {-1 - k / 100}L{count - k} 8Z ' for k in range(count))
        for extreme in ['M3e38 0L0 8Z', 'M0 0L0 3e38Z']:
            yield f'retrace-sort-{count}-{extreme}', f'<path d="{retraces}{extreme}" transform="scale(2)" fill="lime"/>', CLEAR
    retraces = ''.join(f'M{k} {-1 - k / 100}L{80 - k} 8Z ' for k in range(80))
    yield 'retrace-sort-last-numeric-run', f'<path d="M3e38 0L0 8Z {retraces}" transform="scale(2)" fill="lime"/>', CLEAR
    yield 'retrace-sort-early-nan', f'<path d="M3e38 -8L0 80Z {retraces}" transform="scale(2)" fill="lime"/>', CLEAR
    yield 'rounded-away-opacity', '<g opacity=".00000001">' + RECT + '</g>', CLEAR

    # A referenced marker can have DOM ancestors deeper than the rendering
    # stack. Font and geometry inheritance have their own bounded walks.
    for inherited, expected in [(False, GREEN), (True, CLEAR)]:
        groups = ('<g style="width:inherit">' if inherited else '<g>') * 300
        marker = '<marker id="m" markerUnits="userSpaceOnUse" markerWidth="8" markerHeight="8" style="width:' + ('inherit' if inherited else '1em') + ';font-size:8px"><rect style="width:inherit" height="8" fill="lime"/></marker>'
        yield f'deep-marker-{inherited}', '<defs style="width:8px">' + groups + marker + '</g>' * 300 + '</defs><path d="M0 0H1" marker-start="url(#m)"/>', expected

    # Rectangular clip programs may nest, be empty, or have several children.
    clip = '<clipPath id="base">' + RECT + '</clipPath>'
    for content, expected, name in [
        ('<rect width="8" height="8" clip-path="url(#base)"/>', GREEN, 'one'),
        ('<rect width="8" height="8" clip-path="url(#base)"/>' * 2, GREEN, 'siblings'),
        ('<rect width="0" height="8" clip-path="url(#base)"/>', CLEAR, 'zero-width'),
        ('<rect width="8" height="0" clip-path="url(#base)"/>', CLEAR, 'zero-height'),
        ('<rect y="16" width="8" height="8" clip-path="url(#base)"/>', CLEAR, 'disjoint-y'),
        ('<path d="M0 0H8V8H0Z M-4 0H0" clip-path="url(#base)"/>', GREEN, 'left-retrace'),
        ('<path d="M0 0H8V8H0Z M0 -4V0" clip-path="url(#base)"/>', GREEN, 'top-retrace'),
        ('<path d="M0 0H8V8H0Z M0 8V12" clip-path="url(#base)"/>', GREEN, 'bottom-retrace'),
        ('<path d="M0 0H8V8H0Z M0 -4H4" clip-path="url(#base)"/>', GREEN, 'top-horizontal-retrace'),
        ('<path d="M0 0H8V8H0Z M0 12H4" clip-path="url(#base)"/>', GREEN, 'bottom-horizontal-retrace'),
        ('<path d="M0 0V8" clip-path="url(#base)"/>', CLEAR, 'vertical-retrace'),
        ('<path d="M0 0H8" clip-path="url(#base)"/>', CLEAR, 'no-fill'),
        ('', CLEAR, 'empty'),
    ]:
        yield 'clip-' + name, '<defs>' + clip + '<clipPath id="outer" clip-path="url(#base)">' + content + '</clipPath></defs>' + RECT.replace('/>', ' clip-path="url(#outer)"/>'), expected
    yield 'empty-clip-program', '<defs>' + clip + '<clipPath id="empty"><path clip-path="url(#base)"/></clipPath></defs>' + RECT.replace('/>', ' clip-path="url(#empty)"/>'), CLEAR
    nested = '<clipPath id="a"><rect width="8" height="8"/><rect width="8" height="8"/></clipPath><clipPath id="b" clip-path="url(#a)"><rect width="8" height="8" clip-path="url(#base)"/></clipPath>'
    yield 'nested-clip-program', '<defs>' + clip + nested + '<clipPath id="c" clip-path="url(#b)"><rect width="8" height="8"/></clipPath></defs>' + RECT.replace('/>', ' clip-path="url(#c)"/>'), GREEN

    # Finite input transforms can overflow their inverse or mapped vertices.
    for transform in ['scale(1e-40 1)', 'scale(1 1e-40)', 'translate(1e38 0) scale(1e-10 1)', 'translate(0 1e38) scale(1 1e-10)']:
        yield 'pattern-' + transform, f'<defs><pattern id="p" patternUnits="userSpaceOnUse" width="8" height="8" patternTransform="{transform}">{RECT}</pattern></defs>' + RECT.replace('lime', 'url(#p)'), CLEAR
    stops = '<stop path="l 8 0" stop-color="lime"/><stop path="l 0 8" stop-color="lime"/><stop path="l -8 0" stop-color="lime"/><stop path="l 0 -8" stop-color="lime"/>'
    for transform, visible in [('translate(1e20 0)', False), ('translate(-1e20 0)', False), ('translate(0 1e20)', False), ('translate(0 -1e20)', False), ('scale(1)', True), ('scale(1e20)', True), ('matrix(-1e20 0 0 1e20 8 0)', True), ('scale(1e38)', False)]:
        yield 'mesh-' + transform, f'<defs><meshgradient id="m" gradientUnits="userSpaceOnUse" gradientTransform="{transform}"><meshrow><meshpatch>{stops}</meshpatch></meshrow></meshgradient></defs>' + RECT.replace('lime', 'url(#m)'), GREEN if visible else CLEAR

    pattern = '<defs><pattern id="p" patternUnits="userSpaceOnUse" width="8" height="8">' + RECT + '</pattern></defs>'
    yield 'pattern-3d-context', pattern + '<g style="transform-style:preserve-3d">' + RECT.replace('lime', 'url(#p)') + '</g>', GREEN
    yield 'pattern-3d-children', pattern + '<g style="transform-style:preserve-3d">' + RECT.replace('/>', ' style="transform:translateZ(1px)"/>').replace('lime', 'url(#p)') + '</g>', GREEN

    # Empty primitive regions and transparent inputs must stay transparent.
    for attr in ['width="0"', 'height="0"']:
        yield 'empty-displacement-' + attr, f'<defs><filter id="f"><feDisplacementMap scale="2" {attr}/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), CLEAR
    yield 'empty-displacement-input', '<defs><filter id="f"><feFlood flood-opacity="0"/><feDisplacementMap scale="2"/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), CLEAR
    for name, inputs in [
        ('empty-source', '<feDisplacementMap in="SourceGraphic" scale="2"/>'),
        ('disjoint-source-y', '<feFlood y="20" height="8"/><feDisplacementMap scale="2"/>'),
        ('disjoint-source-x', '<feFlood x="20" width="8"/><feDisplacementMap scale="2"/>'),
    ]:
        yield 'displacement-' + name, f'<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8">{inputs}</filter></defs><rect width="8" height="8" fill="none" filter="url(#f)"/>', CLEAR
    for dimension in ['width', 'height']:
        yield 'tile-empty-' + dimension, f'<defs><filter id="f"><feFlood flood-color="lime" {dimension}="0"/><feTile/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), CLEAR
    yield 'tile-singular-transform', '<defs><filter id="f"><feFlood flood-color="lime"/><feTile/></filter></defs>' + RECT.replace('/>', ' transform="matrix(1 1 1 1 0 0)" filter="url(#f)"/>'), CLEAR
    yield 'tile-outside-plane', '<defs><filter id="f" filterUnits="userSpaceOnUse" x="-8" y="-8" width="24" height="24"><feTile/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), GREEN
    yield 'tile-reference-clipped-right', '<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feFlood width="16" flood-color="lime"/><feTile/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), GREEN
    # The FillPaint buffer covers the canvas, beyond this thin sheared
    # subregion. A repeated boundary tap can land on a pixel with no
    # overlap with the clip; its zero area must never divide the colour.
    yield 'tile-zero-area-tap', '<defs><filter id="f" filterUnits="userSpaceOnUse" x="3.1" y="3.000001" width="4" height=".000001"><feTile in="FillPaint"/></filter></defs><rect width="8" height="8" fill="none" transform="matrix(.3 .25 .1 .1 0 0)" filter="url(#f)"/>', CLEAR
    yield 'tile-wrapped-y-overflow', '<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feFlood y="-1.5e38" height="1" flood-opacity="0" result="far"/><feFlood y="0" height="8" flood-opacity="0" result="near"/><feMerge><feMergeNode in="far"/><feMergeNode in="near"/></feMerge><feTile/></filter></defs>' + RECT.replace('/>', ' transform="scale(1 3)" filter="url(#f)"/>'), CLEAR
    yield 'displacement-one-vanishing-axis', '<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="1e31" height="8"><feDisplacementMap scale="2"/></filter></defs><rect width="1e31" height="8" fill="lime" transform="scale(1e-30 1)" filter="url(#f)"/>', bytes((0, 255, 0, 255)) * 56 + bytes(32)
    for axis in ['x', 'y']:
        width, height = ('3e38', '8') if axis == 'x' else ('8', '3e38')
        scale = '1e-40 1' if axis == 'x' else '1 1e-40'
        yield 'tile-inverse-overflow-' + axis, f'<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="{width}" height="{height}"><feFlood flood-color="lime"/><feTile/></filter></defs><rect width="{width}" height="{height}" transform="scale({scale})" filter="url(#f)"/>', CLEAR
    for interpolation in ['sRGB', 'linearRGB']:
        yield 'displacement-map-' + interpolation, f'<defs><filter id="f" filterUnits="userSpaceOnUse" x="-8" y="-8" width="24" height="24"><feFlood flood-color="lime"/><feDisplacementMap in2="SourceGraphic" scale="0.01" color-interpolation-filters="{interpolation}"/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), GREEN
    for paint in ['FillPaint', 'StrokePaint']:
        yield 'linear-paint-reuse-' + paint, f'<defs><filter id="f" x="0" y="0" width="1" height="1"><feBlend in="{paint}" in2="{paint}"/></filter></defs><rect width="8" height="8" fill="lime" stroke="lime" stroke-width="0" filter="url(#f)"/>', GREEN
    yield 'empty-measured-child', '<g transform-box="stroke-box" transform-origin="50% 50%" transform="rotate(0)"><g/>' + RECT + '</g>', GREEN
    for y, height in [(2, 6), (0, 6)]:
        yield f'convolve-wrap-y-{y}', f'<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feFlood flood-color="lime" x="0" y="{y}" width="8" height="{height}"/><feConvolveMatrix order="1" kernelMatrix="1" edgeMode="wrap" x="0" y="0" width="8" height="8"/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), GREEN

    # A pattern must retain what it drew before exhausting the work budget.
    repeated = '<g id="g0">' + RECT + '</g>'
    for k in range(1, 21):
        repeated += f'<g id="g{k}"><use href="#g{k - 1}"/><use href="#g{k - 1}"/></g>'
    yield 'pattern-work-budget', '<defs>' + repeated + '<pattern id="p" patternUnits="userSpaceOnUse" width="8" height="8"><use href="#g20"/></pattern></defs>' + RECT.replace('lime', 'url(#p)'), GREEN
    # Instancing charges emitted edges as well as nodes, for fills and clips.
    for clip in [False, True]:
        shape = '<path id="shape" d="' + 'M0 0H8V8H0Z' * 256 + '"/>'
        content = RECT.replace('/>', ' clip-path="url(#clip)"/>') if clip else '<use href="#shape"/>'
        repeated = shape + '<clipPath id="clip"><use href="#shape"/></clipPath><g id="g0">' + content + '</g>'
        for k in range(1, 41):
            repeated += f'<g id="g{k}"><use href="#g{k - 1}"/><use href="#g{k - 1}"/></g>'
        yield f'repeated-geometry-budget-{clip}', '<defs>' + repeated + '</defs><use href="#g40" fill="lime"/>', GREEN
    # Each instance has a distinct custom-property set. Its invalid fill
    # consumes 8192 replacement tokens, without drawing any geometry.
    payload = 'lime,' * 4096
    uses = ''.join(f'<use href="#empty" style="--id:{k}"/>' for k in range(64))
    yield 'pattern-substitution-budget', f'<defs><g id="empty" style="fill:var(--payload)"/><pattern id="p" patternUnits="userSpaceOnUse" width="8" height="8" style="--payload:{payload}">{RECT}{uses}</pattern></defs>' + RECT.replace('lime', 'url(#p)'), GREEN
    repeated = ''.join('<g>' * depth + RECT.replace('lime', 'url(#p)') + '</g>' * depth for depth in range(200))
    yield 'pattern-cache-depths', pattern + repeated, GREEN
    repeated = ''.join(f'<rect width="80" height="80" transform="scale({k / 10})" fill="url(#p)"/>' for k in range(1, 201))
    yield 'pattern-cache-matrices', pattern + repeated, GREEN
    masks = ''.join(f'<mask id="m{k}" mask-type="alpha">' + RECT.replace('lime', 'url(#p)') + '</mask>' for k in range(256))
    masked = ''.join(RECT.replace('/>', f' mask="url(#m{k})"/>') for k in range(256))
    yield 'pattern-cache-active-chains', pattern + '<defs>' + masks + '</defs>' + masked, GREEN
    outer = ''.join(f'<mask id="n{k}" mask-type="alpha">' + RECT.replace('/>', f' mask="url(#m{k})"/>') + '</mask>' for k in range(256))
    nested = ''.join(RECT.replace('/>', f' mask="url(#m{k})"/>') + RECT.replace('/>', f' mask="url(#n{k})"/>') for k in range(256))
    yield 'pattern-cache-chain-lengths', pattern + '<defs>' + masks + outer + '</defs>' + nested, GREEN
    for operation in ['', '<feDisplacementMap x="0" y="0" width="8" height="8" scale="2"/>']:
        empty_filter = '<defs><filter id="f" filterUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><feFlood flood-color="lime" y="20" height="8"/>' + operation + '</filter><clipPath id="c">' + RECT + '</clipPath></defs>'
        yield 'empty-y-filter-' + operation, empty_filter + RECT.replace('/>', ' filter="url(#f)" clip-path="url(#c)"/>'), CLEAR
        yield 'empty-y-layer-' + operation, empty_filter + RECT.replace('/>', ' filter="url(#f)" transform="rotate(20)"/>'), CLEAR

    yield 'collapsed-css-repeat', document('', 'style="background:repeating-linear-gradient(30deg,lime 0px,lime 1e-44px)"'), GREEN

    # An opaque image is constant under these small grid perturbations;
    # its edges are well outside the canvas. Large translations are empty.
    uri = green_png()
    for transform, expected in [
        ('matrix(0 1 -1 0 .0001 0)', GREEN),
        ('matrix(0 1 -1 0 0 .0001)', GREEN),
        ('matrix(1 .00000001 0 1 0 0)', GREEN),
        ('matrix(1 0 .00000001 1 0 0)', GREEN),
        ('translate(1e20 0)', CLEAR),
        ('translate(0 1e20)', CLEAR),
        ('matrix(-1 0 0 1 1e20 0)', CLEAR),
    ]:
        yield 'native-grid-' + transform, f'<image x="-12" y="-12" width="32" height="32" transform="{transform}" href="{uri}"/>', expected
    for transform in ['matrix(1 .00000001 0 1 0 0)', 'matrix(1 0 .00000001 1 0 0)']:
        yield 'native-grid-far-' + transform, f'<image width="32" height="32" transform="{transform}" href="{uri}"/>', GREEN

    # Vertical separation makes a mask's support empty in y alone.
    for layers in [1, 2]:
        masks = '<mask id="a" maskUnits="userSpaceOnUse" x="0" y="0" width="8" height="8"><rect y="12" width="8" height="8" fill="white"/></mask>'
        masks += '<mask id="b"><rect width="8" height="8" fill="white"/></mask>'
        style = 'mask:url(#a)' if layers == 1 else 'mask-image:url(#a),url(#b);mask-composite:intersect'
        yield f'disjoint-mask-y-{layers}', '<defs>' + masks + '</defs>' + RECT.replace('/>', f' style="{style}"/>'), CLEAR
    masks = '<mask id="a" maskUnits="userSpaceOnUse" x="0" y="0" width="8" height="2"><rect y="4" width="8" height="2" fill="white"/></mask>'
    yield 'mask-region-disjoint-y', '<defs>' + masks + '</defs>' + RECT.replace('/>', ' mask="url(#a)"/>'), CLEAR
    masks = '<mask id="a"><rect width="8" height="2" fill="white"/></mask><mask id="b"><rect y="4" width="8" height="2" fill="white"/></mask>'
    yield 'mask-intersection-disjoint-y', '<defs>' + masks + '</defs>' + RECT.replace('/>', ' style="mask-image:url(#a),url(#b);mask-composite:intersect"/>'), CLEAR

    # feImage starts in sRGB even if the primitive requests linearRGB.
    # No conversion is needed until a later colour operation reads it.
    for interpolation in ['sRGB', 'linearRGB']:
        yield 'feimage-' + interpolation, f'<defs><rect id="image" width="8" height="8" fill="lime"/><filter id="f" x="0" y="0" width="1" height="1" color-interpolation-filters="{interpolation}"><feImage href="#image"/></filter></defs>' + RECT.replace('/>', ' filter="url(#f)"/>'), GREEN


def main():
    binary = str(Path(sys.argv[1]).resolve())
    failures = []
    count = 0
    with tempfile.TemporaryDirectory(prefix='svg-render-boundaries-') as temporary:
        source = Path(temporary) / 'case.svg'
        output = Path(temporary) / 'case.png'
        for name, content, expected in cases():
            if isinstance(content, bytes):
                source.write_bytes(content)
            else:
                source.write_text(content if content.startswith('<svg ') else document(content))
            subprocess.run([binary, 'render', str(source), str(output), str(SIDE), str(SIDE)], check=True, timeout=30)
            width, height, pixels = canon.decode(output)
            if (width, height) != (SIDE, SIDE) or bytes(pixels) != expected:
                failures.append(name)
            count += 1
    if failures:
        sys.exit('render_boundaries failed: ' + ', '.join(failures))
    print(f'checked {count} CLI renders against exact RGBA pixels')


if __name__ == '__main__':
    main()
