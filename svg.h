#pragma once

#include <stddef.h>
#include <stdint.h>

struct SvgImage {
    virtual ~SvgImage();

    virtual const void* data() const = 0;
    virtual size_t length() const = 0;
    virtual uint32_t width() const = 0;
    virtual uint32_t height() const = 0;
};

struct SvgDecoder {
    virtual SvgImage* decode(const void* bytes, size_t length) = 0;
};

struct SvgDocument {
    virtual ~SvgDocument();

    virtual float width() const = 0;
    virtual float height() const = 0;
    virtual SvgImage* render(uint32_t width, uint32_t height) = 0;
    virtual SvgImage* render(uint32_t width, uint32_t height, float scale) = 0;
};

struct SvgLibrary {
    virtual ~SvgLibrary();

    virtual SvgDocument* parse(const void* file, size_t length) = 0;

    static SvgLibrary* create(SvgDecoder* decoder);
};
