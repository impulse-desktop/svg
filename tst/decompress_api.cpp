#include "../svg.h"

#include <stdio.h>
#include <string.h>

namespace {
    struct Failure {
        SvgError error;
    };

    struct HostFailure {};

    struct Source {
        const char* xml;
        int calls;
        int mode;
    };

    static const unsigned char gzip[] = {0x1f, 0x8b};
    static const char green[] = "<svg xmlns='http://www.w3.org/2000/svg' width='2' height='2'><rect width='2' height='2' fill='lime'/></svg>";
    static const char nested[] = "<svg xmlns='http://www.w3.org/2000/svg'><image href='data:image/svg+xml;base64,H4s='/></svg>";
    static const char nestedSized[] = "<svg xmlns='http://www.w3.org/2000/svg' width='2' height='2'><image width='2' height='2' href='data:image/svg+xml;base64,H4s='/></svg>";

    [[noreturn]] static void fail(SvgError error) {
        throw Failure{error};
    }

    static int decompress(void* context, const void* bytes, size_t length, int (*write)(void*, const void*, size_t), void* output) {
        Source& source = *(Source*)context;

        source.calls++;

        if (length != sizeof(gzip) || memcmp(bytes, gzip, sizeof(gzip))) {
            throw HostFailure{};
        }

        if (source.mode == 2) {
            // Even a host ignoring a rejected write cannot turn it into
            // successful decompression or make it overflow an allocation.
            write(output, "x", SIZE_MAX);

            return 1;
        }

        if (source.mode == 3) {
            write(output, nullptr, 1);

            return 1;
        }

        if (source.mode == 4) {
            write(output, source.xml, strlen(source.xml));

            throw HostFailure{};
        }

        return write(output, source.xml, strlen(source.xml)) && source.mode == 0;
    }

    static bool rejected(SvgDecompress callback, Source& source, const void* bytes, size_t length, SvgError expected) {
        SvgLibrary* library = svg_library_create(nullptr, callback, fail, &source);
        bool correct = false;

        try {
            svg_document_destroy(svg_library_parse(library, bytes, length));
        } catch (const Failure& error) {
            correct = error.error == expected;
        }

        svg_library_destroy(library);

        return correct;
    }

    static bool rendered(SvgDecompress callback, Source& source, const char* xml, unsigned char alpha) {
        SvgLibrary* library = svg_library_create(nullptr, callback, fail, &source);
        SvgDocument* document = svg_library_parse(library, xml, strlen(xml));

        svg_library_destroy(library);

        SvgImage* image = svg_document_render(document, 2, 2);
        const unsigned char* pixels = (const unsigned char*)svg_image_data(image);
        bool correct = svg_document_width(document) == 2 && svg_document_height(document) == 2;

        for (size_t i = 0; i < 16; i += 4) {
            correct = correct && pixels[i + 3] == alpha && (!alpha || (pixels[i] == 0 && pixels[i + 1] == 255 && pixels[i + 2] == 0));
        }

        if (!correct) {
            fprintf(stderr, "decompress_api: size %g x %g, first pixel %u %u %u %u, expected alpha %u, calls %d\n", svg_document_width(document), svg_document_height(document), pixels[0], pixels[1], pixels[2], pixels[3], alpha, source.calls);
        }

        svg_image_destroy(image);
        svg_document_destroy(document);

        return correct;
    }
}

int main() {
    Source source{green, 0, 0};

    if (!rendered(decompress, source, green, 255) || source.calls || !rejected(nullptr, source, gzip, sizeof(gzip), SVG_DECOMPRESS)) {
        return 1;
    }

    // Measurement and rendering both need the decompressor, including
    // after the library has been released.
    if (!rendered(decompress, source, nested, 255) || source.calls != 2 || !rendered(nullptr, source, nestedSized, 0)) {
        fprintf(stderr, "decompress_api: nested image, calls %d\n", source.calls);
        return 2;
    }

    for (int mode = 1; mode <= 3; mode++) {
        source.mode = mode;

        if (!rejected(decompress, source, gzip, sizeof(gzip), SVG_DECOMPRESS) || !rendered(decompress, source, nestedSized, 0)) {
            return 3;
        }
    }

    source.mode = 0;
    source.xml = "";

    if (!rejected(decompress, source, gzip, sizeof(gzip), SVG_XML_NO_ROOT)) {
        return 4;
    }

    source.xml = "<html/>";

    if (!rejected(decompress, source, gzip, sizeof(gzip), SVG_NOT_SVG)) {
        return 5;
    }

    source.xml = "<svg>";

    if (!rejected(decompress, source, gzip, sizeof(gzip), SVG_XML_UNTERMINATED_ELEMENT)) {
        return 6;
    }

    source.xml = green;
    source.mode = 4;
    SvgLibrary* library = svg_library_create(nullptr, decompress, fail, &source);
    bool propagated = false;

    try {
        svg_document_destroy(svg_library_parse(library, gzip, sizeof(gzip)));
    } catch (const HostFailure&) {
        propagated = true;
    }

    svg_library_destroy(library);

    if (!propagated) {
        return 7;
    }

    puts("checked decompressor absence, dispatch, lifetime, nested measurement, failed and rejected writes, XML errors and host exceptions");

    return 0;
}
