#include "svg.h"

#include <std/lib/buffer.h>
#include <std/mem/obj_pool.h>

using namespace stl;

namespace {
    struct ImageImpl final: SvgImage {
        Buffer rgba;
        u32 width_;
        u32 height_;

        ImageImpl(u32 width, u32 height);

        const void* data() const override;
        size_t length() const override;
        u32 width() const override;
        u32 height() const override;
    };

    struct DocumentImpl final: SvgDocument {
        float width() const override;
        float height() const override;
        SvgImage* render(ObjPool& pool, u32 width, u32 height) override;
    };

    struct LibraryImpl final: SvgLibrary {
        SvgDocument* parse(ObjPool& pool, StringView file) override;
    };
}

ImageImpl::ImageImpl(u32 width, u32 height)
    : width_(width)
    , height_(height)
{
    rgba.zero((size_t)width * height * 4);
}

const void* ImageImpl::data() const {
    return rgba.data();
}

size_t ImageImpl::length() const {
    return rgba.length();
}

u32 ImageImpl::width() const {
    return width_;
}

u32 ImageImpl::height() const {
    return height_;
}

float DocumentImpl::width() const {
    return 0.f;
}

float DocumentImpl::height() const {
    return 0.f;
}

SvgImage* DocumentImpl::render(ObjPool& pool, u32 width, u32 height) {
    return pool.make<ImageImpl>(width, height);
}

SvgDocument* LibraryImpl::parse(ObjPool& pool, StringView) {
    return pool.make<DocumentImpl>();
}

SvgLibrary* SvgLibrary::create(ObjPool& pool) {
    return pool.make<LibraryImpl>();
}
