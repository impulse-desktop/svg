#include "../svg.h"

#include <stddef.h>
#include <stdint.h>

namespace {
    constexpr size_t maxInputSize = 1u << 20;
    constexpr uint32_t renderSize = 64;
}

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size) {
    if (size > maxInputSize) {
        return 0;
    }

    SvgLibrary* library = svg_library_create(nullptr, nullptr);
    SvgDocument* document = svg_library_parse(library, data, size);
    SvgImage* image = document ? svg_document_render(document, renderSize, renderSize, 1.f) : nullptr;

    if (image) {
        image->vtable->destroy(image);
    }

    svg_document_destroy(document);
    svg_library_destroy(library);

    return 0;
}
