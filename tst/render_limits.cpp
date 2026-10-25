#include "../svg.h"

#include <string>
#include <stdio.h>

namespace {
    struct Failure {};

    [[noreturn]] static void fail(SvgError) {
        throw Failure{};
    }

    static bool repeatedGeometry(SvgLibrary* library, bool clip) {
        std::string source = "<svg xmlns='http://www.w3.org/2000/svg' width='4' height='4'><defs><path id='shape' d='";

        // One small path fits the budget. Repeated instancing must charge
        // its geometry as well as its nodes, including geometry in clips.
        for (int i = 0; i < 256; i++) {
            source += "M0 0H4V4H0Z";
        }

        source += "'/><clipPath id='clip'><use href='#shape'/></clipPath><g id='g0'>";
        source += clip ? "<rect width='4' height='4' clip-path='url(#clip)'/>" : "<use href='#shape'/>";
        source += "</g>";

        for (int i = 1; i <= 40; i++) {
            std::string previous = std::to_string(i - 1);

            source += "<g id='g" + std::to_string(i) + "'><use href='#g" + previous + "'/><use href='#g" + previous + "'/></g>";
        }

        source += "</defs><use href='#g40' fill='lime'/></svg>";

        SvgDocument* document = svg_library_parse(library, source.data(), source.size());
        SvgImage* image = svg_document_render(document, 4, 4);
        const unsigned char* data = (const unsigned char*)svg_image_data(image);
        bool correct = svg_image_length(image) == 64;

        for (size_t i = 0; i < svg_image_length(image); i += 4) {
            correct = correct && data[i] == 0 && data[i + 1] == 255 && data[i + 2] == 0 && data[i + 3] == 255;
        }

        svg_image_destroy(image);
        svg_document_destroy(document);

        return correct;
    }
}

int main() {
    SvgLibrary* library = svg_library_create(nullptr, nullptr, fail, nullptr);
    bool correct = repeatedGeometry(library, false) && repeatedGeometry(library, true);

    svg_library_destroy(library);

    if (!correct) {
        fprintf(stderr, "render_limits: repeated geometry lost the drawing before the work limit\n");

        return 1;
    }

    puts("render_limits: repeated fill and clip geometry passed");

    return 0;
}
