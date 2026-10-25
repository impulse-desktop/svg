# svg

[![CI](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml)
[![codecov](https://codecov.io/gh/impulse-desktop/svg/branch/main/graph/badge.svg)](https://app.codecov.io/gh/impulse-desktop/svg/tree/main)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

An SVG renderer for the impulse desktop: parser, document model and
rasterizer, written from scratch and checked against analytic controls and a
corpus of thousands of files drawn by several other renderers. It draws static
SVG without text: no `<text>`, no SMIL or CSS animation, no script, no
`foreignObject`. Raster images inside a document are decoded by the program
that embeds the renderer.

## Render canon

`svg_canon.cpp` is the frozen implementation used to draw references for
the corpus and units. It is built with the current `svg.h`, test driver and
raster decoders, without sanitizers or coverage instrumentation. Building
references does not require Git history. The initial snapshot is from
`4ef64535522c0304f440b42df70b90a9b675f14f`.

After reviewing the rendering changes, recanonize by copying the accepted
implementation:

```sh
cp svg.cpp svg_canon.cpp
```

Or take it from a specific revision:

```sh
git show REVISION:svg.cpp > svg_canon.cpp
```

The build tracks the snapshot and redraws references when it changes.

## Render scale comparison

`./build -j24 scale` draws every selected corpus file with the current
renderer at a 256-pixel longest side, then at four times those rounded
dimensions (1024 on the longest side). It reduces the large image by four
using the existing alpha-aware box average and compares the two small
images. Each file has its own cached build node and is rendered only twice.
The corpus exclusions and `-Dfilter` work as in the ordinary scores.

Reports are `.build/scale/{all,meanrms,maxrms,struct,aa}.txt`; target aliases
are `scale_all`, `scale_meanrms`, `scale_maxrms`, `scale_struct` and
`scale_aa`. One aggregator reads the cached per-file scores once and writes
all five reports. The metrics
and ranking are those of `tst/score.py`: a normalized premultiplied RGBA
mean absolute error per pair, aggregated as meanrms/maxrms, and the usual
structural wrong-pixel share. With one pair per file, its reported RMS and
AA error both equal that pair's error. Processing failures and the existing
120-second per-file timeout are marked as errors and ranked at the top.

This is a diagnostic group, separate from `tests`: differences do not fail
the build. Both resolutions are rendered even for size-dependent SVGs,
such as non-scaling strokes; their differences can be intentional.
The group needs neither the canon nor Chromium. To save a pair for review:

```sh
./svg scale input.svg score.txt direct.png reduced.png
```

## Use

`svg.h` is a C header.

```c
#include "svg.h"

static void fail(SvgError error) {
    /* throw or abort (longjmp will not work): fail does not return */
    abort();
}

SvgLibrary* library = svg_library_create(NULL, NULL, fail, NULL);
SvgDocument* document = svg_library_parse(library, bytes, length);
SvgImage* image = svg_document_render(document, 512, 512);
const uint8_t* rgba = svg_image_data(image);

/* svg_image_width(image) by svg_image_height(image) pixels */
svg_image_destroy(image);
svg_document_destroy(document);
svg_library_destroy(library);
```

No call returns NULL. What a call cannot do goes to the `fail` given to
`svg_library_create`, which is never NULL, with an `SvgError` that says why:
an input file is not XML or its root is not SVG, or the output image is empty
or oversized.
An invalid SVG used as an image leaves that image empty and preserves the
rest of the document.
`fail` does not return; a C++ program may throw from it.
`svg_document_width` and `svg_document_height` give the document's own size
in CSS pixels: the width and height its root gives, one of them with the
viewBox's ratio, or the viewBox's size. A side the root leaves open, giving
none or a percentage, is the right or bottom edge of what the document draws,
so a document that draws nothing past the origin has a side of zero and draws
nothing. `svg_document_render(document, width, height)` draws the document
into `width` by `height` pixels, its own size stretched to them; a viewBox
fits them as its `preserveAspectRatio` says. The pixels are RGBA, 8 bits a
channel, not premultiplied.

To draw raster images, pass `svg_library_create` a function that turns their
bytes into an `SvgImage` of your own, a struct that starts with an `SvgImage`
and answers through an `SvgImageVtable`; the renderer destroys it once drawn.
Without one, raster images are left out. The library, the document and the
image do not depend on one another and are released in any order.

The second argument of `svg_library_create` is an optional `SvgDecompress`
callback for SVGZ. The library recognizes gzip by its signature, including
in embedded SVG images, and calls the decompressor with the same context
as the image decoder. It copies the chunks the callback writes; a successful
callback returns nonzero only after validating the complete gzip stream.
With a NULL callback, gzip input fails with `SVG_DECOMPRESS`; an invalid
compressed image leaves only that image empty. Plain SVG needs no callback.
The test driver supplies a zlib implementation, with a 256 MiB limit on
decompressed bytes per resource. The renderer itself has no zlib dependency.
SVGZ fixtures in `tst/unit` have independent drawings in `tst/expect`;
the frozen CANON renders those plain drawings because it ignores the new
decompressor argument. `./build unit` also runs the C API and gzip stream
unit checks, so the coverage run includes them.

## Build

Copy `svg.h` and `svg.cpp` into your project. `svg.cpp` is C++17 and neither
throws nor catches; it depends on nothing but the C library and libm.
Link the program with the C++ runtime.

## License

MIT, see [LICENSE](LICENSE).
