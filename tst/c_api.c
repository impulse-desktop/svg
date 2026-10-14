// A program that copies in svg.h and svg.cpp builds them at C99 and C++17:
// this one draws through the C API, with a decoder of its own.
#include "../svg.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    SvgImage image;
    uint8_t rgba[4];
} Solid;

static int destroyed;

static void solidDestroy(SvgImage* image) {
    destroyed++;
    free(image);
}

static const void* solidData(const SvgImage* image) {
    return ((const Solid*)image)->rgba;
}

static size_t solidLength(const SvgImage* image) {
    (void)image;

    return 4;
}

static uint32_t solidSide(const SvgImage* image) {
    (void)image;

    return 1;
}

static const SvgImageVtable solidVtable = {solidDestroy, solidData, solidLength, solidSide, solidSide};

// Every raster image decodes to one pixel of the color in the context.
static SvgImage* decode(void* context, const void* bytes, size_t length) {
    Solid* solid = (Solid*)malloc(sizeof(Solid));

    (void)bytes;
    (void)length;
    solid->image.vtable = &solidVtable;
    memcpy(solid->rgba, context, 4);

    return &solid->image;
}

static int pixel(const SvgImage* image, uint32_t x, uint32_t y, const uint8_t* expected) {
    const uint8_t* rgba = (const uint8_t*)svg_image_data(image) + ((size_t)y * svg_image_width(image) + x) * 4;

    return memcmp(rgba, expected, 4) == 0;
}

static int fail(const char* what) {
    fprintf(stderr, "c_api: %s\n", what);

    return 1;
}

// The drawing is SVG and every image has pixels: the renderer never fails.
static void failed(SvgError error) {
    fprintf(stderr, "c_api: the renderer failed with %d\n", (int)error);
    exit(1);
}

int main(void) {
    static const char drawing[] = "<svg xmlns=\"http://www.w3.org/2000/svg\" width=\"12\" height=\"4\">"
                                  "<rect width=\"4\" height=\"4\" fill=\"#f00\"/>"
                                  "<image x=\"8\" width=\"4\" height=\"4\" preserveAspectRatio=\"none\" image-rendering=\"pixelated\" href=\"data:image/png;base64,AAAA\"/>"
                                  "</svg>";
    static const uint8_t red[4] = {255, 0, 0, 255};
    static const uint8_t green[4] = {0, 255, 0, 255};
    static const uint8_t clear[4] = {0, 0, 0, 0};
    uint8_t color[4] = {0, 255, 0, 255};
    SvgLibrary* library = svg_library_create(decode, failed, color);
    SvgDocument* document = svg_library_parse(library, drawing, sizeof(drawing) - 1);

    if (svg_document_width(document) != 12.f || svg_document_height(document) != 4.f) {
        return fail("the document is not 12 by 4");
    }

    SvgImage* plain = svg_document_render(document, 12, 4, 1.f);
    SvgImage* doubled = svg_document_render(document, 24, 8, 2.f);

    // The images outlive their document and library.
    svg_document_destroy(document);
    svg_library_destroy(library);

    if (svg_image_length(plain) != 12 * 4 * 4 || !pixel(plain, 3, 3, red) || !pixel(plain, 4, 0, clear) || !pixel(plain, 8, 0, green)) {
        return fail("the drawing at 1x");
    }

    if (svg_image_width(doubled) != 24 || !pixel(doubled, 7, 7, red) || !pixel(doubled, 8, 0, clear) || !pixel(doubled, 16, 0, green)) {
        return fail("the drawing at 2x");
    }

    if (destroyed != 2) {
        return fail("the decoded images were not destroyed once drawn");
    }

    svg_image_destroy(plain);
    svg_image_destroy(doubled);
    puts("checked the C API from C99 against svg.cpp as C++17");

    return 0;
}
