# svg

[![CI](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml)
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

SvgLibrary* library = svg_library_create(NULL, NULL);
SvgDocument* document = svg_library_parse(library, bytes, length);

if (document) {
    SvgImage* image = svg_document_render(document, 512, 512, 1.f);

    if (image) {
        const uint8_t* rgba = svg_image_data(image);

        /* svg_image_width(image) by svg_image_height(image) pixels */
        svg_image_destroy(image);
    }

    svg_document_destroy(document);
}

svg_library_destroy(library);
```

`svg_library_parse` returns NULL for a file that is not well-formed SVG.
`svg_document_render(document, width, height, scale)` draws the document as
`<img src="file.svg">` laid out at `width / scale` by `height / scale` CSS
pixels, into `width` by `height` device pixels; it returns NULL for an empty
or oversized image. The pixels are RGBA, 8 bits a channel, not premultiplied.
`svg_document_width` and `svg_document_height` give the document's own size in
CSS pixels.

To draw raster images, pass `svg_library_create` a function that turns their
bytes into an `SvgImage` of your own, a struct that starts with an `SvgImage`
and answers through an `SvgImageVtable`; the renderer destroys it once drawn.
Without one, raster images are left out. The library, the document and the
image do not depend on one another and are released in any order.

## Build

Copy `svg.h` and `svg.cpp` into your project. `svg.cpp` is C++17 and uses
exceptions internally; it depends on nothing but the C library and libm.
Link the program with the C++ runtime.

## License

MIT, see [LICENSE](LICENSE).
