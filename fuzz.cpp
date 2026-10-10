#include "svg.h"

#include <std/mem/obj_pool.h>

#include <stddef.h>
#include <stdint.h>

using namespace stl;

namespace {
    constexpr size_t maxInputSize = 1u << 20;
    constexpr u32 renderSize = 64;

    struct NullDecoder final: SvgDecoder {
        SvgImage* decode(ObjPool&, StringView) override {
            return nullptr;
        }
    };
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > maxInputSize) {
        return 0;
    }

    try {
        ObjPool::Ref pool = ObjPool::fromMemory();
        SvgLibrary* library = SvgLibrary::create(*pool, pool->make<NullDecoder>());
        SvgDocument* document = library->parse(*pool, StringView((const u8*)data, size));

        if (document) {
            document->render(*pool, renderSize, renderSize);
        }
    } catch (...) {
    }

    return 0;
}
