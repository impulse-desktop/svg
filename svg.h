#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct SvgImage SvgImage;
    typedef struct SvgImageVtable SvgImageVtable;
    typedef struct SvgDocument SvgDocument;
    typedef struct SvgLibrary SvgLibrary;

    // Width by height RGBA pixels, 8 bits a channel, not premultiplied, length
    // bytes of them. Whoever is done with an image calls its destroy.
    struct SvgImageVtable {
        void (*destroy)(SvgImage* image);
        const void* (*data)(const SvgImage* image);
        size_t (*length)(const SvgImage* image);
        uint32_t (*width)(const SvgImage* image);
        uint32_t (*height)(const SvgImage* image);
    };

    struct SvgImage {
        const SvgImageVtable* vtable;
    };

    // Call through the image's vtable.
    void svg_image_destroy(SvgImage* image);
    const void* svg_image_data(const SvgImage* image);
    size_t svg_image_length(const SvgImage* image);
    uint32_t svg_image_width(const SvgImage* image);
    uint32_t svg_image_height(const SvgImage* image);

    // Decodes the bytes of a raster image inside a document into an image of a
    // pixel or more, or returns NULL. The renderer destroys the image once it
    // has drawn it. A library without a decoder draws no raster images.
    typedef SvgImage* (*SvgDecode)(void* context, const void* bytes, size_t length);

    SvgLibrary* svg_library_create(SvgDecode decode, void* context);
    void svg_library_destroy(SvgLibrary* library);

    // The document keeps a copy of the file and outlives its library; an image
    // outlives its document.
    SvgDocument* svg_library_parse(SvgLibrary* library, const void* file, size_t length);

    void svg_document_destroy(SvgDocument* document);
    float svg_document_width(const SvgDocument* document);
    float svg_document_height(const SvgDocument* document);
    SvgImage* svg_document_render(SvgDocument* document, uint32_t width, uint32_t height, float scale);

#ifdef __cplusplus
}
#endif
