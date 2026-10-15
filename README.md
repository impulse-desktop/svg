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
`svg_library_create`, which is never NULL, with an `SvgError` that says why: a
file that is not XML, or whose root is not SVG, an SVG image inside a document
that is not XML, an empty or oversized image.
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
