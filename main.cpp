#include "svg.h"

#include <std/ios/sys.h>
#include <std/sys/throw.h>
#include <std/lib/buffer.h>
#include <std/str/builder.h>
#include <std/ios/fs_utils.h>
#include <std/mem/obj_pool.h>

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace stl;

namespace {
    constexpr unsigned caseSeconds = 120;

    struct DriverError: Exception {
        Buffer text;

        explicit DriverError(StringView message)
            : text(message)
        {
        }

        ExceptionKind kind() const noexcept override {
            return ExceptionKind::Verify;
        }

        StringView description() override {
            return StringView(text);
        }
    };

    [[noreturn]] void fail(StringView what, const char* path) {
        throw DriverError(StringView(StringBuilder() << what << StringView(path)));
    }

    struct Pixels {
        Buffer rgba;
        u32 width = 0;
        u32 height = 0;
    };

    void readPng(const char* path, Pixels& out) {
        png_image image;

        memset(&image, 0, sizeof(image));
        image.version = PNG_IMAGE_VERSION;

        if (!png_image_begin_read_from_file(&image, path)) {
            fail(StringView(u8"cannot read "), path);
        }

        image.format = PNG_FORMAT_RGBA;
        out.width = image.width;
        out.height = image.height;
        out.rgba.zero(PNG_IMAGE_SIZE(image));

        if (!png_image_finish_read(&image, nullptr, out.rgba.mutData(), 0, nullptr)) {
            fail(StringView(u8"cannot decode "), path);
        }
    }

    void writePng(const char* path, const Pixels& pixels) {
        png_image image;

        memset(&image, 0, sizeof(image));
        image.version = PNG_IMAGE_VERSION;
        image.width = pixels.width;
        image.height = pixels.height;
        image.format = PNG_FORMAT_RGBA;

        if (!png_image_write_to_file(&image, path, 0, pixels.rgba.data(), 0, nullptr)) {
            fail(StringView(u8"cannot write "), path);
        }
    }

    void draw(SvgDocument* document, ObjPool& pool, Pixels& pixels) {
        pixels.rgba.zero((size_t)pixels.width * pixels.height * 4);

        if (!document) {
            return;
        }

        try {
            SvgImage* image = document->render(pool, pixels.width, pixels.height);

            if (image && image->width() == pixels.width && image->height() == pixels.height && image->length() == pixels.rgba.length()) {
                memcpy(pixels.rgba.mutData(), image->data(), pixels.rgba.length());
            }
        } catch (...) {
            pixels.rgba.zero(pixels.rgba.length());
        }
    }

    SvgDocument* load(ObjPool& pool, const char* path) {
        Buffer name{StringView(path)};
        Buffer file;

        readFileContent(name, file);

        try {
            return SvgLibrary::create(pool)->parse(pool, StringView(file));
        } catch (...) {
            return nullptr;
        }
    }

    double error(const Pixels& ours, const Pixels& reference) {
        const u8* a = (const u8*)ours.rgba.data();
        const u8* b = (const u8*)reference.rgba.data();
        size_t count = (size_t)reference.width * reference.height;
        u64 sum = 0;

        for (size_t i = 0; i < count; i++, a += 4, b += 4) {
            for (int c = 0; c < 3; c++) {
                i64 delta = (i64)a[c] * a[3] - (i64)b[c] * b[3];

                sum += (u64)(delta < 0 ? -delta : delta);
            }

            i64 alpha = ((i64)a[3] - (i64)b[3]) * 255;

            sum += (u64)(alpha < 0 ? -alpha : alpha);
        }

        return count ? (double)sum / ((double)count * 4.0 * 255.0 * 255.0) : 0.0;
    }

    int render(int argc, char** argv) {
        if (argc != 6) {
            sysE << StringView(u8"usage: svg render IN.svg OUT.png WIDTH HEIGHT") << endL;

            return 2;
        }

        ObjPool::Ref pool = ObjPool::fromMemory();
        Pixels pixels;

        pixels.width = (u32)atoi(argv[4]);
        pixels.height = (u32)atoi(argv[5]);
        draw(load(*pool, argv[2]), *pool, pixels);
        writePng(argv[3], pixels);

        return 0;
    }

    int test(int argc, char** argv) {
        if (argc < 5) {
            sysE << StringView(u8"usage: svg test IN.svg SCORE REFERENCE.png...") << endL;

            return 2;
        }

        alarm(caseSeconds);

        ObjPool::Ref pool = ObjPool::fromMemory();
        SvgDocument* document = load(*pool, argv[2]);
        StringBuilder scores;

        for (int i = 4; i < argc; i++) {
            Pixels reference;
            Pixels ours;

            readPng(argv[i], reference);
            ours.width = reference.width;
            ours.height = reference.height;
            draw(document, *pool, ours);

            char text[32];

            snprintf(text, sizeof(text), "%.9f", error(ours, reference));
            scores << (i > 4 ? StringView(u8" ") : StringView()) << StringView(text);
        }

        scores << StringView(u8"\n");

        FILE* out = fopen(argv[3], "wb");

        if (!out || fwrite(scores.data(), 1, scores.length(), out) != scores.length() || fclose(out) != 0) {
            fail(StringView(u8"cannot write "), argv[3]);
        }

        return 0;
    }
}

int main(int argc, char** argv) {
    int result = 2;

    try {
        if (argc >= 2 && StringView(argv[1]) == StringView(u8"render")) {
            result = render(argc, argv);
        } else if (argc >= 2 && StringView(argv[1]) == StringView(u8"test")) {
            result = test(argc, argv);
        } else {
            sysE << StringView(u8"usage: svg render|test ...") << endL;
        }
    } catch (...) {
        sysE << StringView(u8"svg: ") << Exception::current() << endL;

        result = 1;
    }

    return result;
}
