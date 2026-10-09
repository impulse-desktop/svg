#include "svg.h"

#include <std/ios/sys.h>
#include <std/sys/throw.h>
#include <std/lib/buffer.h>
#include <std/str/builder.h>
#include <std/ios/fs_utils.h>
#include <std/mem/obj_pool.h>

#include <png.h>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <jpeglib.h>

using namespace stl;

namespace {
    constexpr unsigned caseSeconds = 120;

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

    struct JpegFailure {
        jpeg_error_mgr manager;
        jmp_buf jump;
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

static void jpegFailed(j_common_ptr info) {
    longjmp(((JpegFailure*)info->err)->jump, 1);
}

static void jpegQuiet(j_common_ptr) {
}

static SvgImage* decodeJpeg(ObjPool& pool, StringView bytes) {
    jpeg_decompress_struct info;
    JpegFailure failure;
    DecodedImage* volatile out = nullptr;

    info.err = jpeg_std_error(&failure.manager);
    failure.manager.error_exit = jpegFailed;
    failure.manager.output_message = jpegQuiet;

    if (setjmp(failure.jump)) {
        jpeg_destroy_decompress(&info);

        return nullptr;
    }

    jpeg_create_decompress(&info);
    jpeg_mem_src(&info, (const unsigned char*)bytes.data(), (unsigned long)bytes.length());
    jpeg_read_header(&info, TRUE);
    info.out_color_space = JCS_EXT_RGBA;
    jpeg_start_decompress(&info);

    if ((u64)info.output_width * info.output_height > (1u << 26)) {
        jpeg_destroy_decompress(&info);

        return nullptr;
    }

    out = pool.make<DecodedImage>(info.output_width, info.output_height);

    while (info.output_scanline < info.output_height) {
        JSAMPROW row = (JSAMPROW)out->rgba.mutData() + (size_t)info.output_scanline * info.output_width * 4;

        jpeg_read_scanlines(&info, &row, 1);
    }

    jpeg_finish_decompress(&info);
    jpeg_destroy_decompress(&info);

    return out;
}

SvgImage* RasterDecoder::decode(ObjPool& pool, StringView bytes) {
    if (bytes.length() >= 3 && bytes[0] == 0xff && bytes[1] == 0xd8 && bytes[2] == 0xff) {
        return decodeJpeg(pool, bytes);
    }

    png_image image;

    memset(&image, 0, sizeof(image));
    image.version = PNG_IMAGE_VERSION;

    if (!png_image_begin_read_from_memory(&image, bytes.data(), bytes.length())) {
        return nullptr;
    }

    image.format = PNG_FORMAT_RGBA;

    if ((u64)image.width * image.height > (1u << 26)) {
        png_image_free(&image);

        return nullptr;
    }

    DecodedImage* out = pool.make<DecodedImage>(image.width, image.height);

    if (!png_image_finish_read(&image, nullptr, out->rgba.mutData(), 0, nullptr)) {
        return nullptr;
    }

    return out;
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
