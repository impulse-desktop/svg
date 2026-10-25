#include "../svg.h"

#include <string>
#include <stdio.h>

namespace {
    struct Failure {
        SvgError error;
    };

    [[noreturn]] static void fail(SvgError error) {
        throw Failure{error};
    }

    static bool check(SvgLibrary* library, const std::string& source, bool terminated) {
        try {
            SvgDocument* document = svg_library_parse(library, source.data(), source.size());

            svg_document_destroy(document);

            if (terminated) {
                return true;
            }

            fprintf(stderr, "parser_api: accepted an unterminated processing instruction: %s\n", source.c_str());
        } catch (const Failure& failure) {
            if (!terminated && failure.error == SVG_XML_UNTERMINATED_PI) {
                return true;
            }

            fprintf(stderr, "parser_api: unexpected error %d: %s\n", (int)failure.error, source.c_str());
        }

        return false;
    }
}

int main() {
    SvgLibrary* library = svg_library_create(nullptr, nullptr, fail, nullptr);
    const std::string root = "<svg xmlns='http://www.w3.org/2000/svg'>";
    const std::string empty = root + "</svg>";
    bool correct = true;
    size_t count = 0;

    // The '?' opening a PI cannot also close it. Check every parser that
    // skips instructions, including trailing miscellany and entity content.
    for (const char* instruction : {"<?", "<?>", "<?target>", "<?target?", "<?target?>", "<?target >?>", "<?target ? ?>"}) {
        std::string pi(instruction);
        bool terminated = pi.size() >= 2 && pi.compare(pi.size() - 2, 2, "?>") == 0 && pi != "<?>";

        for (const std::string& source : {
                 pi + empty,
                 empty + pi,
                 root + pi + "</svg>",
                 "<!DOCTYPE svg [" + pi + "]>" + empty,
                 "<!DOCTYPE svg [<!ENTITY content '" + pi + "'>]>" + root + "&content;</svg>",
             }) {
            correct = check(library, source, terminated) && correct;
            count++;
        }
    }

    svg_library_destroy(library);

    if (correct) {
        printf("parser_api: %zu processing instruction cases passed\n", count);
    }

    return correct ? 0 : 1;
}
