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
`svg_old.cpp` remains the separate performance baseline.

## Performance regression check

`./build -j24 perf` builds the current renderer and `svg_old`, then compares
their total time on the corpus selected by `tests` (including its
`EXCLUDED` and `DESIGN` lists), except
`gecko/slow__layout__reftests__svg__explosive-use.svg`, which times out in both
versions and remains in functional tests. `svg_old.cpp` is the unchanged implementation
from the last green revision, `2799b420d0fb42f4154ecc2b66a6606a55fd64d1`;
both binaries use the current driver, raster decoders and identical build flags.

The default is a 512-pixel longest side, 24 parallel file pairs and three
samples per file and binary. A sample includes library creation, SVG parsing,
rendering, raster decoding and resource destruction. File reads, process
startup, size discovery and PNG encoding are outside the timer. Each sample
parses a fresh document. Both versions draw at the same dimensions; their
order alternates between files. The test sums each file's median elapsed time
and requires **new <= old × 1.05**. An error, timeout or missing result fails
the check; incomplete totals cannot pass.

The report records every file and both binary hashes. After a failing check,
`./build perf_report` publishes the cached report to `.build/perf/report.json`
without rendering again. Use the same `-B` and `-D` options for both commands.
Like other build targets, measurements are cached: change `-Dperf_run=LABEL`
to force a fresh measurement. `-Dperf_side`, `-Dperf_repeat`, `-Dperf_jobs`
and `-Dperf_timeout` control the run; timeout is per file and binary, across
all its samples. `-Dfilter='tabler/*'` measures only that subset and labels
the result accordingly. Performance checks are separate from sanitizer tests;
run them without sanitizers or competing benchmarks.

For diagnosis, `./build -j24 perf_quick` takes one sample per binary on
every third eligible file: zero-based index `% 3 == 0` in sorted path order,
after exclusions and `-Dfilter`. It uses the same dimensions, timer and
5% check, with nine times fewer renders than the default full test.
The selection is identical across revisions with the same corpus and filter.
Its result describes that sample; it does not establish full-corpus parity.
The report is `.build/perf_quick/report.json`; publish it after a failing
check with `./build perf_quick_report` and the same options. `-Dperf_repeat`
affects only the full test; the quick test always takes one sample.

## Use

`svg.h` is a C header.

```c
#include "svg.h"

static void fail(SvgError error) {
    /* throw or abort (longjmp will not work): fail does not return */
    abort();
}

SvgLibrary* library = svg_library_create(NULL, fail, NULL);
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

## Build

Copy `svg.h` and `svg.cpp` into your project. `svg.cpp` is C++17 and neither
throws nor catches; it depends on nothing but the C library and libm.
Link the program with the C++ runtime.

## License

MIT, see [LICENSE](LICENSE).
