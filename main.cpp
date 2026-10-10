#include "svg.h"

#include <std/ios/sys.h>
#include <std/sys/throw.h>
#include <std/lib/buffer.h>
#include <std/str/builder.h>
#include <std/ios/fs_utils.h>
#include <std/mem/obj_pool.h>

#include <png.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define WASM_RT_CORE_TYPES_DEFINED
typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef float f32;
typedef double f64;

extern "C" {
#include <decode.h>
}

using namespace stl;

namespace {
    constexpr unsigned caseSeconds = 120;
    constexpr u64 maxPixels = 1u << 27;

    struct DriverError: Exception {
        Buffer text;

        explicit DriverError(StringView message);

        ExceptionKind kind() const noexcept override;
        StringView description() override;
    };

    struct Pixels {
        Buffer rgba;
        u32 width = 0;
        u32 height = 0;
    };

    struct DecodedImage final: SvgImage {
        Buffer rgba;
        u32 width_;
        u32 height_;

        DecodedImage(u32 width, u32 height);

        const void* data() const override;
        size_t length() const override;
        u32 width() const override;
        u32 height() const override;
    };

    struct RasterDecoder final: SvgDecoder {
        SvgImage* decode(ObjPool& pool, StringView bytes) override;
    };

    struct Decoder {
        w2c_decode wasm;

        Decoder();
        ~Decoder() noexcept;

        u8* at(u64 offset, u64 length);
        SvgImage* decode(ObjPool& pool, StringView bytes);
    };
}

DriverError::DriverError(StringView message)
    : text(message)
{
}

ExceptionKind DriverError::kind() const noexcept {
    return ExceptionKind::Verify;
}

StringView DriverError::description() {
    return StringView(text);
}

DecodedImage::DecodedImage(u32 width, u32 height)
    : width_(width)
    , height_(height)
{
    rgba.zero((size_t)width * height * 4);
}

const void* DecodedImage::data() const {
    return rgba.data();
}

size_t DecodedImage::length() const {
    return rgba.length();
}

u32 DecodedImage::width() const {
    return width_;
}

u32 DecodedImage::height() const {
    return height_;
}

extern "C" void decodeTrapHandler(wasm_rt_trap_t code) {
    throw DriverError(StringView(wasm_rt_strerror(code)));
}

Decoder::Decoder() {
    wasm2c_decode_instantiate(&wasm);
}

Decoder::~Decoder() noexcept {
    wasm2c_decode_free(&wasm);
}

u8* Decoder::at(u64 offset, u64 length) {
    wasm_rt_memory_t* memory = w2c_decode_memory(&wasm);

    if (offset + length > memory->size) {
        throw DriverError(StringView(u8"the decoder answered out of its memory"));
    }

    return (u8*)memory->data + offset;
}

SvgImage* Decoder::decode(ObjPool& pool, StringView bytes) {
    u32 length = (u32)bytes.length();
    u32 in = w2c_decode_malloc(&wasm, length);

    if (!in) {
        return nullptr;
    }

    memcpy(at(in, length), bytes.data(), length);

    u32 res = w2c_decode_decode(&wasm, in, length, 0, 0);

    if (!res) {
        return nullptr;
    }

    u32 header[2];

    memcpy(header, at(res, sizeof(header)), sizeof(header));

    u64 pixels = (u64)header[0] * header[1];

    if (!pixels || pixels > maxPixels) {
        return nullptr;
    }

    DecodedImage* out = pool.make<DecodedImage>(header[0], header[1]);

    memcpy(out->rgba.mutData(), at((u64)res + sizeof(header), pixels * 4), pixels * 4);

    return out;
}

SvgImage* RasterDecoder::decode(ObjPool& pool, StringView bytes) {
    if (bytes.empty() || bytes.length() > 0x7fffffffu) {
        return nullptr;
    }

    try {
        Decoder decoder;

        return decoder.decode(pool, bytes);
    } catch (const DriverError&) {
        return nullptr;
    }
}

namespace {
    [[noreturn]] static void fail(StringView what, const char* path) {
        throw DriverError(StringView(StringBuilder() << what << StringView(path)));
    }

    static void readPng(const char* path, Pixels& out) {
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

    static void writePng(const char* path, const Pixels& pixels) {
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

    static void draw(SvgDocument* document, ObjPool& pool, Pixels& pixels) {
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

    static SvgDocument* load(ObjPool& pool, const char* path) {
        Buffer name{StringView(path)};
        Buffer file;

        readFileContent(name, file);

        try {
            return SvgLibrary::create(pool, pool.make<RasterDecoder>())->parse(pool, StringView(file));
        } catch (...) {
            return nullptr;
        }
    }

    static double error(const Pixels& ours, const Pixels& reference) {
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

    static int render(int argc, char** argv) {
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

    static int test(int argc, char** argv) {
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

    static u64 nanoseconds() {
        timespec now;

        clock_gettime(CLOCK_MONOTONIC, &now);

        return (u64)now.tv_sec * 1000000000ull + (u64)now.tv_nsec;
    }

    static int bench(int argc, char** argv) {
        if (argc != 4) {
            sysE << StringView(u8"usage: svg bench LIST REPEAT") << endL;

            return 2;
        }

        FILE* list = fopen(argv[2], "rb");
        int repeat = atoi(argv[3]);
        char line[4096];

        if (!list) {
            fail(StringView(u8"cannot read "), argv[2]);
        }

        while (fgets(line, sizeof(line), list)) {
            char path[4000];
            unsigned width;
            unsigned height;

            if (sscanf(line, "%3999s %u %u", path, &width, &height) != 3) {
                continue;
            }

            Buffer name{StringView(path)};
            Buffer file;
            u64 best = ~0ull;
            bool ok = true;

            readFileContent(name, file);

            for (int r = 0; r < repeat; r++) {
                ObjPool::Ref pool = ObjPool::fromMemory();
                u64 start = nanoseconds();

                try {
                    SvgDocument* document = SvgLibrary::create(*pool, pool->make<RasterDecoder>())->parse(*pool, StringView(file));

                    ok = document && document->render(*pool, width, height);
                } catch (...) {
                    ok = false;
                }

                u64 spent = nanoseconds() - start;

                best = spent < best ? spent : best;
            }

            printf("%s %llu %d\n", path, (unsigned long long)best, ok ? 1 : 0);
        }

        fclose(list);

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
        } else if (argc >= 2 && StringView(argv[1]) == StringView(u8"bench")) {
            result = bench(argc, argv);
        } else {
            sysE << StringView(u8"usage: svg render|test|bench ...") << endL;
        }
    } catch (...) {
        sysE << StringView(u8"svg: ") << Exception::current() << endL;

        result = 1;
    }

    return result;
}
