#include "../svg.h"

#include <stddef.h>
#include <stdint.h>

namespace {
    constexpr size_t maxInputSize = 1u << 20;
    constexpr uint32_t renderSize = 64;

    struct Failure {};

    [[noreturn]] void fail(SvgError) {
        throw Failure();
    }
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > maxInputSize) {
        return 0;
    }

    SvgLibrary* library = svg_library_create(nullptr, fail, nullptr);

    try {
        SvgDocument* document = svg_library_parse(library, data, size);

        try {
            svg_image_destroy(svg_document_render(document, renderSize, renderSize));
        } catch (const Failure&) {
        }

        svg_document_destroy(document);
    } catch (const Failure&) {
    }

    svg_library_destroy(library);

    return 0;
}
