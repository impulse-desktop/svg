#pragma once

#include <std/str/view.h>
#include <std/sys/types.h>

namespace stl {
    class ObjPool;
}

struct SvgImage {
    virtual const void* data() const = 0;
    virtual size_t length() const = 0;
    virtual u32 width() const = 0;
    virtual u32 height() const = 0;
};

struct SvgDocument {
    virtual float width() const = 0;
    virtual float height() const = 0;
    virtual SvgImage* render(stl::ObjPool& pool, u32 width, u32 height) = 0;
};

struct SvgLibrary {
    virtual SvgDocument* parse(stl::ObjPool& pool, stl::StringView file) = 0;

    static SvgLibrary* create(stl::ObjPool& pool);
};
