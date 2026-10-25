// Exercise the sampled-paint boundary directly as well as through SVG:
// overflow in a transform must never become an integer pixel address.
#include "../svg.cpp"

#include <stdio.h>
#include <string>

namespace {
    struct Failure {};

    [[noreturn]] static void failed(SvgError) {
        throw Failure{};
    }

    static bool patternCoordinates() {
        u16 green[] = {0, 65535, 0, 65535};
        DevicePaint paint{};

        paint.type = PaintType::Pattern;
        paint.tile = green;
        paint.imageWidth = 1;
        paint.imageHeight = 1;
        paint.window = IntBox{0, 0, 1, 1};
        paint.opacity16 = 65535;
        paint.inverse = translation(0.f, 0.f);

        for (float coordinate : {0.f, -1.f, 1.f, -1e30f, 1e30f, INFINITY, -INFINITY, NAN}) {
            for (bool vertical : {false, true}) {
                u16 out[4] = {};
                bool drawn = paintAt(paint, vertical ? 0.f : coordinate, vertical ? coordinate : 0.f, out);
                bool finite = isfinite(coordinate);

                if (drawn != finite || (drawn && memcmp(out, green, sizeof(green)))) {
                    fprintf(stderr, "finite_paints: pattern coordinate %g, vertical %d, drawn %d\n", coordinate, vertical, drawn);

                    return false;
                }
            }
        }

        return true;
    }

    static bool meshBounds(SvgLibrary* library, const char* transform, bool visible) {
        std::string source = "<svg xmlns='http://www.w3.org/2000/svg' width='4' height='4'><meshgradient id='m' gradientUnits='userSpaceOnUse' gradientTransform='";

        source += transform;
        source += "'><meshrow><meshpatch><stop path='l 4 0' stop-color='lime'/><stop path='l 0 4' stop-color='lime'/><stop path='l -4 0' stop-color='lime'/><stop path='l 0 -4' stop-color='lime'/></meshpatch></meshrow></meshgradient><rect width='4' height='4' fill='url(#m)'/></svg>";

        SvgDocument* document = svg_library_parse(library, source.data(), source.size());
        SvgImage* image = svg_document_render(document, 4, 4);
        const unsigned char* pixels = (const unsigned char*)svg_image_data(image);
        bool correct = svg_image_length(image) == 64;

        for (size_t i = 0; i < svg_image_length(image); i += 4) {
            correct = correct && pixels[i + 3] == (visible ? 255 : 0);

            if (visible) {
                correct = correct && pixels[i] == 0 && pixels[i + 1] == 255 && pixels[i + 2] == 0;
            }
        }

        svg_image_destroy(image);
        svg_document_destroy(document);

        if (!correct) {
            fprintf(stderr, "finite_paints: mesh transform %s, expected visible %d\n", transform, visible);
        }

        return correct;
    }
}

int main() {
    SvgLibrary* library = svg_library_create(nullptr, nullptr, failed, nullptr);
    bool correct = patternCoordinates();

    for (const char* transform : {"translate(1e20 0)", "translate(-1e20 0)", "translate(0 1e20)", "translate(0 -1e20)"}) {
        correct = meshBounds(library, transform, false) && correct;
    }

    for (const char* transform : {"scale(1)", "scale(1e20)", "matrix(-1e20 0 0 1e20 4 0)"}) {
        correct = meshBounds(library, transform, true) && correct;
    }

    svg_library_destroy(library);

    if (correct) {
        puts("finite_paints: finite and nonfinite pattern coordinates and seven mesh bounds passed");
    }

    return correct ? 0 : 1;
}
