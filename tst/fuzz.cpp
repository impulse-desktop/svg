#include "../svg.h"

#include <memory>
#include <stddef.h>
#include <stdint.h>

namespace {
    constexpr size_t maxInputSize = 1u << 20;
    constexpr uint32_t renderSize = 64;

    struct NullDecoder final: SvgDecoder {
        SvgImage* decode(const void* bytes, size_t length) override;
    };
}

SvgImage* NullDecoder::decode(const void*, size_t) {
    return nullptr;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > maxInputSize) {
        return 0;
    }

    NullDecoder decoder;

    try {
        std::unique_ptr<SvgLibrary> library(SvgLibrary::create(&decoder));
        std::unique_ptr<SvgDocument> document(library->parse(data, size));

        if (document) {
            std::unique_ptr<SvgImage> image(document->render(renderSize, renderSize));
        }
    } catch (...) {
    }

    return 0;
}
