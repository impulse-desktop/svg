#include "svg.h"

#include <std/ios/sys.h>
#include <std/sys/throw.h>
#include <std/lib/buffer.h>
#include <std/str/builder.h>
#include <std/ios/fs_utils.h>
#include <std/mem/obj_pool.h>

#include <png.h>
#include <math.h>
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
    constexpr int structTolerance = 32;

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

    static u32 pngNumber(const u8* p) {
        return (u32)p[0] << 24 | (u32)p[1] << 16 | (u32)p[2] << 8 | p[3];
    }

    static bool pngChecksum(const u8* chunk, u32 length) {
        u32 crc = ~0u;

        for (u32 k = 4; k < length + 8; k++) {
            crc ^= chunk[k];

            for (int bit = 0; bit < 8; bit++) {
                crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1)));
            }
        }

        return ~crc == pngNumber(chunk + length + 8);
    }

    static size_t pngGamma(StringView bytes, double& gamma) {
        static const u8 signature[] = {137, 80, 78, 71, 13, 10, 26, 10};
        static const u32 srgbChromaticities[] = {31270, 32900, 64000, 33000, 30000, 60000, 15000, 6000};

        if (bytes.length() < 33 || memcmp(bytes.data(), signature, sizeof(signature)) || pngNumber(bytes.data() + 8) != 13 || memcmp(bytes.data() + 12, "IHDR", 4) || bytes[24] > 8) {
            return 0;
        }

        size_t found = 0;

        for (size_t offset = 8; bytes.length() - offset >= 12;) {
            const u8* chunk = bytes.data() + offset;
            u32 length = pngNumber(chunk);

            if (length > bytes.length() - offset - 12) {
                return 0;
            }

            if (!memcmp(chunk + 4, "iCCP", 4) || !memcmp(chunk + 4, "sRGB", 4) || !memcmp(chunk + 4, "cICP", 4)) {
                return 0;
            }

            if (!memcmp(chunk + 4, "cHRM", 4)) {
                if (length != 32 || !pngChecksum(chunk, length)) {
                    return 0;
                }

                for (int k = 0; k < 8; k++) {
                    if (pngNumber(chunk + 8 + k * 4) != srgbChromaticities[k]) {
                        return 0;
                    }
                }
            }

            if (!memcmp(chunk + 4, "gAMA", 4)) {
                if (found || length != 4 || !pngNumber(chunk + 8) || !pngChecksum(chunk, length)) {
                    return 0;
                }

                gamma = pngNumber(chunk + 8) / 100000.0;
                found = offset + 8;
            }

            if (!memcmp(chunk + 4, "IDAT", 4)) {
                return found;
            }

            offset += (size_t)length + 12;
        }

        return 0;
    }
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

    double gamma = 0.0;
    size_t gammaOffset = pngGamma(bytes, gamma);

    if (gammaOffset) {
        // The module treats non-linear PNG samples as sRGB and linear ones as
        // RGB. Decode the stored samples unchanged, then apply the actual curve.
        static const u8 neutralGamma[] = {0, 0, 177, 143, 11, 252, 97, 5};

        memcpy(at((u64)in + gammaOffset, sizeof(neutralGamma)), neutralGamma, sizeof(neutralGamma));
    }

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

    if (gammaOffset) {
        u8 table[256];

        for (int k = 0; k < 256; k++) {
            double linear = pow(k / 255.0, 1.0 / gamma);
            double srgb = linear <= 0.0031308 ? 12.92 * linear : 1.055 * pow(linear, 1.0 / 2.4) - 0.055;

            table[k] = (u8)(srgb * 255.0 + 0.5);
        }

        u8* rgba = (u8*)out->rgba.mutData();

        for (u64 k = 0; k < pixels * 4; k += 4) {
            for (int c = 0; c < 3; c++) {
                rgba[k + c] = table[rgba[k + c]];
            }
        }
    }

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

    static void draw(SvgDocument* document, ObjPool& pool, Pixels& pixels, float scale) {
        pixels.rgba.zero((size_t)pixels.width * pixels.height * 4);

        if (!document) {
            return;
        }

        try {
            SvgImage* image = document->render(pool, pixels.width, pixels.height, scale);

            if (image && image->width() == pixels.width && image->height() == pixels.height && image->length() == pixels.rgba.length()) {
                memcpy(pixels.rgba.mutData(), image->data(), pixels.rgba.length());
            }
        } catch (...) {
            pixels.rgba.zero(pixels.rgba.length());
        }
    }

    static SvgDocument* load(ObjPool& pool, const char* path, Buffer& file) {
        Buffer name{StringView(path)};

        readFileContent(name, file);

        try {
            return SvgLibrary::create(pool, pool.make<RasterDecoder>())->parse(pool, StringView(file));
        } catch (...) {
            return nullptr;
        }
    }

    static SvgDocument* load(ObjPool& pool, const char* path) {
        Buffer file;

        return load(pool, path, file);
    }

    static bool startsWith(const u8* p, const u8* end, const char* word) {
        size_t length = strlen(word);

        return (size_t)(end - p) >= length && memcmp(p, word, length) == 0;
    }

    static bool space(u8 c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    static const u8* skipSpace(const u8* p, const u8* end) {
        while (p < end && space(*p)) {
            p++;
        }

        return p;
    }

    static bool readNumber(const u8*& p, const u8* end, float& out) {
        char text[64];
        size_t n = 0;

        while (p + n < end && n < sizeof(text) - 1 && strchr("+-.0123456789eE", p[n])) {
            text[n] = (char)p[n];
            n++;
        }

        text[n] = 0;

        char* stop = nullptr;

        out = strtof(text, &stop);

        if (stop == text) {
            return false;
        }

        p += stop - text;

        return true;
    }

    static bool attributeValue(const u8* tag, const u8* end, const char* name, const u8*& value, const u8*& valueEnd) {
        for (const u8* p = tag; p < end; p++) {
            if (!space(*p) || !startsWith(p + 1, end, name)) {
                continue;
            }

            const u8* q = skipSpace(p + 1 + strlen(name), end);

            if (q >= end || *q != '=') {
                continue;
            }

            q = skipSpace(q + 1, end);

            if (q >= end || (*q != '"' && *q != '\'')) {
                continue;
            }

            u8 quote = *q;

            value = q + 1;
            valueEnd = value;

            while (valueEnd < end && *valueEnd != quote) {
                valueEnd++;
            }

            return true;
        }

        return false;
    }

    static bool viewBox(const u8* p, const u8* end) {
        float numbers[4];

        for (int k = 0; k < 4; k++) {
            p = skipSpace(p, end);

            if (k && p < end && *p == ',') {
                p = skipSpace(p + 1, end);
            }

            if (!readNumber(p, end, numbers[k])) {
                return false;
            }
        }

        return skipSpace(p, end) == end && numbers[2] > 0.f && numbers[3] > 0.f;
    }

    static bool absolute(const u8* p, const u8* end) {
        float value;

        p = skipSpace(p, end);

        if (!readNumber(p, end, value) || !(value > 0.f)) {
            return false;
        }

        while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))) {
            p++;
        }

        return skipSpace(p, end) == end;
    }

    static bool rootEffect(const u8* p, const u8* end) {
        for (; p < end; p++) {
            if (startsWith(p, end, "clip-path") || startsWith(p, end, "mask") || startsWith(p, end, "filter") || startsWith(p, end, "transform")) {
                return true;
            }
        }

        return false;
    }

    static bool scalable(StringView text) {
        const u8* p = text.begin();
        const u8* end = text.end();

        for (const u8* q = p; q < end; q++) {
            if (startsWith(q, end, "non-scaling")) {
                return false;
            }

            if (((*q >= '0' && *q <= '9') || *q == '.') && (startsWith(q + 1, end, "vw") || startsWith(q + 1, end, "vh") || startsWith(q + 1, end, "vi") || startsWith(q + 1, end, "vb") || startsWith(q + 1, end, "vmin") || startsWith(q + 1, end, "vmax"))) {
                return false;
            }

            if (startsWith(q, end, "<style") || startsWith(q, end, ":style")) {
                const u8* close = q;

                while (close < end && !startsWith(close, end, "</")) {
                    close++;
                }

                if (rootEffect(q, close)) {
                    return false;
                }
            }
        }

        while (true) {
            while (p < end && *p != '<') {
                p++;
            }

            if (p >= end) {
                return false;
            }

            if (startsWith(p, end, "<!--")) {
                while (p < end && !startsWith(p, end, "-->")) {
                    p++;
                }

                continue;
            }

            if (startsWith(p, end, "<?") || startsWith(p, end, "<!")) {
                p++;

                continue;
            }

            break;
        }

        const u8* name = p + 1;
        const u8* tagEnd = name;
        u8 quote = 0;

        while (tagEnd < end && (quote || *tagEnd != '>')) {
            if (quote && *tagEnd == quote) {
                quote = 0;
            } else if (!quote && (*tagEnd == '"' || *tagEnd == '\'')) {
                quote = *tagEnd;
            }

            tagEnd++;
        }

        const u8* nameEnd = name;

        while (nameEnd < tagEnd && !space(*nameEnd) && *nameEnd != '/' && *nameEnd != '>') {
            nameEnd++;
        }

        bool root = (nameEnd - name == 3 && memcmp(name, "svg", 3) == 0) || (nameEnd - name > 4 && memcmp(nameEnd - 4, ":svg", 4) == 0);
        const u8* value;
        const u8* valueEnd;

        if (!root || rootEffect(nameEnd, tagEnd)) {
            return false;
        }

        if (attributeValue(nameEnd, tagEnd, "viewBox", value, valueEnd) && viewBox(value, valueEnd)) {
            return true;
        }

        bool width = attributeValue(nameEnd, tagEnd, "width", value, valueEnd) && absolute(value, valueEnd);
        bool height = attributeValue(nameEnd, tagEnd, "height", value, valueEnd) && absolute(value, valueEnd);

        return width && height;
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

    static void premultiply(const Pixels& pixels, Buffer& out) {
        size_t length = (size_t)pixels.width * pixels.height * 4;
        const u8* p = (const u8*)pixels.rgba.data();

        out.zero(length);

        u8* q = (u8*)out.mutData();

        for (size_t i = 0; i < length; i += 4) {
            u32 a = p[i + 3];

            for (int c = 0; c < 3; c++) {
                q[i + c] = (u8)((p[i + c] * a + 127) / 255);
            }

            q[i + 3] = (u8)a;
        }
    }

    static u8 least(u8 a, u8 b) {
        return a < b ? a : b;
    }

    static u8 most(u8 a, u8 b) {
        return a > b ? a : b;
    }

    static void envelopeRow(const u8* s, size_t stride, u8* lo, u8* hi) {
        if (stride <= 4) {
            memcpy(lo, s, stride);
            memcpy(hi, s, stride);

            return;
        }

        for (size_t i = 0; i < 4; i++) {
            lo[i] = least(s[i], s[i + 4]);
            hi[i] = most(s[i], s[i + 4]);
            lo[stride - 4 + i] = least(s[stride - 8 + i], s[stride - 4 + i]);
            hi[stride - 4 + i] = most(s[stride - 8 + i], s[stride - 4 + i]);
        }

        for (size_t i = 4; i + 4 < stride; i++) {
            lo[i] = least(least(s[i - 4], s[i]), s[i + 4]);
            hi[i] = most(most(s[i - 4], s[i]), s[i + 4]);
        }
    }

    static void envelope(const u8* src, u32 width, u32 height, Buffer& lo, Buffer& hi) {
        size_t stride = (size_t)width * 4;
        size_t length = stride * height;
        Buffer rowLo;
        Buffer rowHi;

        rowLo.zero(length);
        rowHi.zero(length);
        lo.zero(length);
        hi.zero(length);

        u8* rl = (u8*)rowLo.mutData();
        u8* rh = (u8*)rowHi.mutData();
        u8* l = (u8*)lo.mutData();
        u8* h = (u8*)hi.mutData();

        for (u32 y = 0; y < height; y++) {
            envelopeRow(src + y * stride, stride, rl + y * stride, rh + y * stride);
        }

        for (u32 y = 0; y < height; y++) {
            const u8* l0 = rl + (y ? y - 1 : y) * stride;
            const u8* l1 = rl + y * stride;
            const u8* l2 = rl + (y + 1 < height ? y + 1 : y) * stride;
            const u8* h0 = rh + (y ? y - 1 : y) * stride;
            const u8* h1 = rh + y * stride;
            const u8* h2 = rh + (y + 1 < height ? y + 1 : y) * stride;
            u8* ol = l + y * stride;
            u8* oh = h + y * stride;

            for (size_t i = 0; i < stride; i++) {
                ol[i] = least(least(l0[i], l1[i]), l2[i]);
                oh[i] = most(most(h0[i], h1[i]), h2[i]);
            }
        }
    }

    static void blur(const u8* src, u32 width, u32 height, Buffer& out) {
        size_t stride = (size_t)width * 4;
        size_t length = stride * height;
        Buffer row;

        row.zero(length);
        out.zero(length);

        u8* r = (u8*)row.mutData();
        u8* o = (u8*)out.mutData();

        for (u32 y = 0; y < height; y++) {
            const u8* s = src + y * stride;
            u8* d = r + y * stride;

            if (stride <= 4) {
                memcpy(d, s, stride);

                continue;
            }

            for (size_t i = 0; i < 4; i++) {
                d[i] = (u8)((3 * s[i] + s[i + 4] + 2) / 4);
                d[stride - 4 + i] = (u8)((s[stride - 8 + i] + 3 * s[stride - 4 + i] + 2) / 4);
            }

            for (size_t i = 4; i + 4 < stride; i++) {
                d[i] = (u8)((s[i - 4] + 2 * s[i] + s[i + 4] + 2) / 4);
            }
        }

        for (u32 y = 0; y < height; y++) {
            const u8* r0 = r + (y ? y - 1 : y) * stride;
            const u8* r1 = r + y * stride;
            const u8* r2 = r + (y + 1 < height ? y + 1 : y) * stride;
            u8* d = o + y * stride;

            for (size_t i = 0; i < stride; i++) {
                d[i] = (u8)((r0[i] + 2 * r1[i] + r2[i] + 2) / 4);
            }
        }
    }

    static double structural(const u8* a, const u8* b, u32 width, u32 height) {
        size_t count = (size_t)width * height;
        Buffer aLo;
        Buffer aHi;
        Buffer bLo;
        Buffer bHi;
        Buffer aBlur;
        Buffer bBlur;

        envelope(a, width, height, aLo, aHi);
        envelope(b, width, height, bLo, bHi);
        blur(a, width, height, aBlur);
        blur(b, width, height, bBlur);

        const u8* al = (const u8*)aLo.data();
        const u8* ah = (const u8*)aHi.data();
        const u8* bl = (const u8*)bLo.data();
        const u8* bh = (const u8*)bHi.data();
        const u8* fa = (const u8*)aBlur.data();
        const u8* fb = (const u8*)bBlur.data();
        Buffer flagBuffer;

        flagBuffer.zero(count * 4);

        u8* flags = (u8*)flagBuffer.mutData();

        for (size_t i = 0; i < count * 4; i++) {
            int va = a[i];
            int vb = b[i];
            int blurred = (int)fa[i] - (int)fb[i];
            int outside = (va - bh[i] > structTolerance) | (bl[i] - va > structTolerance) | (vb - ah[i] > structTolerance) | (al[i] - vb > structTolerance);
            int apart = (blurred > structTolerance) | (-blurred > structTolerance);

            flags[i] = (u8)(outside | (apart << 1));
        }

        size_t bad = 0;

        for (size_t i = 0; i < count * 4; i += 4) {
            u8 any = flags[i] | flags[i + 1] | flags[i + 2] | flags[i + 3];

            bad += any == 3;
        }

        return count ? (double)bad / (double)count : 0.0;
    }

    static bool twice(const Pixels& small, const Pixels& large) {
        return large.width == 2 * small.width && large.height == 2 * small.height;
    }

    static void halve(const u8* src, u32 width, u32 height, u32 outWidth, u32 outHeight, Buffer& out) {
        out.zero((size_t)outWidth * outHeight * 4);

        u8* q = (u8*)out.mutData();

        for (u32 y = 0; y < outHeight; y++) {
            for (u32 x = 0; x < outWidth; x++) {
                u32 sum[4] = {0, 0, 0, 0};
                u32 n = 0;

                for (u32 yy = 2 * y; yy < 2 * y + 2 && yy < height; yy++) {
                    for (u32 xx = 2 * x; xx < 2 * x + 2 && xx < width; xx++) {
                        for (int c = 0; c < 4; c++) {
                            sum[c] += src[((size_t)yy * width + xx) * 4 + c];
                        }

                        n++;
                    }
                }

                for (int c = 0; c < 4; c++) {
                    q[((size_t)y * outWidth + x) * 4 + c] = n ? (u8)((sum[c] + n / 2) / n) : 0;
                }
            }
        }
    }

    template <u32 F>
    static void shrinkBy(const u8* p, size_t stride, u32 width, u32 height, u8* q) {
        Buffer sumBuffer;

        sumBuffer.zero((size_t)width * 4 * sizeof(u32));

        u32* sum = (u32*)sumBuffer.mutData();

        for (u32 y = 0; y < height; y++) {
            memset(sum, 0, (size_t)width * 4 * sizeof(u32));

            for (u32 yy = 0; yy < F; yy++) {
                const u8* s = p + ((size_t)y * F + yy) * stride;

                for (u32 x = 0; x < width; x++) {
                    u32* t = sum + (size_t)x * 4;
                    const u8* r = s + (size_t)x * F * 4;

                    for (u32 k = 0; k < F; k++) {
                        u32 alpha = r[k * 4 + 3];

                        t[0] += r[k * 4] * alpha;
                        t[1] += r[k * 4 + 1] * alpha;
                        t[2] += r[k * 4 + 2] * alpha;
                        t[3] += alpha;
                    }
                }
            }

            u8* d = q + (size_t)y * width * 4;

            for (u32 x = 0; x < width; x++) {
                const u32* t = sum + (size_t)x * 4;

                if (!t[3]) {
                    continue;
                }

                for (int c = 0; c < 3; c++) {
                    u32 v = (t[c] + t[3] / 2) / t[3];

                    d[x * 4 + c] = (u8)(v > 255 ? 255 : v);
                }

                d[x * 4 + 3] = (u8)((t[3] + F * F / 2) / (F * F));
            }
        }
    }

    static void shrink(const Pixels& large, u32 factor, Pixels& out) {
        out.width = large.width / factor;
        out.height = large.height / factor;
        out.rgba.zero((size_t)out.width * out.height * 4);

        const u8* p = (const u8*)large.rgba.data();
        u8* q = (u8*)out.rgba.mutData();
        size_t stride = (size_t)large.width * 4;

        switch (factor) {
            case 1:
                memcpy(q, p, (size_t)out.width * out.height * 4);
                break;
            case 2:
                shrinkBy<2>(p, stride, out.width, out.height, q);
                break;
            case 4:
                shrinkBy<4>(p, stride, out.width, out.height, q);
                break;
            case 8:
                shrinkBy<8>(p, stride, out.width, out.height, q);
                break;
            default: {
                Buffer sumBuffer;

                sumBuffer.zero((size_t)out.width * 4 * sizeof(u64));

                u64* sum = (u64*)sumBuffer.mutData();
                u64 count = (u64)factor * factor;

                for (u32 y = 0; y < out.height; y++) {
                    memset(sum, 0, (size_t)out.width * 4 * sizeof(u64));

                    for (u32 yy = y * factor; yy < (y + 1) * factor; yy++) {
                        const u8* s = p + yy * stride;

                        for (u32 x = 0; x < out.width * factor; x++) {
                            u64* t = sum + (size_t)(x / factor) * 4;
                            u64 alpha = s[x * 4 + 3];

                            t[0] += s[x * 4] * alpha;
                            t[1] += s[x * 4 + 1] * alpha;
                            t[2] += s[x * 4 + 2] * alpha;
                            t[3] += alpha;
                        }
                    }

                    u8* d = q + (size_t)y * out.width * 4;

                    for (u32 x = 0; x < out.width; x++) {
                        const u64* t = sum + (size_t)x * 4;

                        if (!t[3]) {
                            continue;
                        }

                        for (int c = 0; c < 3; c++) {
                            u64 v = (t[c] + t[3] / 2) / t[3];

                            d[x * 4 + c] = (u8)(v > 255 ? 255 : v);
                        }

                        d[x * 4 + 3] = (u8)((t[3] + count / 2) / count);
                    }
                }
            }
        }
    }

    static void drawFine(SvgDocument* document, ObjPool& pool, Pixels& pixels, u32 factor, Pixels& large) {
        if (factor <= 1) {
            draw(document, pool, pixels, 1.f);

            return;
        }

        if (large.width != pixels.width * factor || large.height != pixels.height * factor) {
            large.width = pixels.width * factor;
            large.height = pixels.height * factor;
            draw(document, pool, large, (float)factor);
        }

        shrink(large, factor, pixels);
    }

    static u32 factorOf(const char* text) {
        int factor = atoi(text);

        if (factor < 1 || factor > 64) {
            fail(StringView(u8"bad factor "), text);
        }

        return (u32)factor;
    }

    static u32 sideFactor(u32 side, u32 width, u32 height) {
        u32 longest = width > height ? width : height;

        return longest && side > longest ? side / longest : 1;
    }

    static int render(int argc, char** argv) {
        if (argc != 6 && argc != 7) {
            sysE << StringView(u8"usage: svg render IN.svg OUT.png WIDTH HEIGHT [SIDE]") << endL;

            return 2;
        }

        ObjPool::Ref pool = ObjPool::fromMemory();
        Pixels pixels;

        pixels.width = (u32)atoi(argv[4]);
        pixels.height = (u32)atoi(argv[5]);
        Pixels large;

        drawFine(load(*pool, argv[2]), *pool, pixels, argc == 7 ? sideFactor((u32)atoi(argv[6]), pixels.width, pixels.height) : 1, large);
        writePng(argv[3], pixels);

        return 0;
    }

    static int shrinkCommand(int argc, char** argv) {
        if (argc != 5) {
            sysE << StringView(u8"usage: svg shrink IN.png OUT.png FACTOR") << endL;

            return 2;
        }

        u32 factor = factorOf(argv[4]);
        Pixels large;
        Pixels small;

        readPng(argv[2], large);

        if (large.width % factor || large.height % factor) {
            fail(StringView(u8"size is not a multiple of the factor in "), argv[2]);
        }

        shrink(large, factor, small);
        writePng(argv[3], small);

        return 0;
    }

    static int test(int argc, char** argv) {
        if (argc < 6) {
            sysE << StringView(u8"usage: svg test IN.svg SCORE SIDE REFERENCE.png...") << endL;

            return 2;
        }

        u32 side = (u32)atoi(argv[4]);

        alarm(caseSeconds);

        ObjPool::Ref pool = ObjPool::fromMemory();
        Buffer file;
        SvgDocument* document = load(*pool, argv[2], file);
        bool reuse = scalable(StringView(file));
        Pixels large;
        StringBuilder errors;
        StringBuilder shares;
        StringBuilder small;
        StringBuilder referenceScaling;
        StringBuilder oursScaling;
        Pixels first;
        Buffer firstReference;
        Buffer firstOurs;

        for (int i = 5; i < argc; i++) {
            Pixels reference;
            Pixels ours;
            Pixels fine;
            Buffer referencePremultiplied;
            Buffer oursPremultiplied;
            Buffer finePremultiplied;

            readPng(argv[i], reference);

            u32 factor = sideFactor(side, reference.width, reference.height);

            bool plain = i == 5 || factor <= 1;

            ours.width = reference.width;
            ours.height = reference.height;
            premultiply(reference, referencePremultiplied);

            if (plain) {
                draw(document, *pool, ours, 1.f);
                premultiply(ours, oursPremultiplied);
            }

            bool shared = reuse && factor > 1 && large.width == reference.width * factor && large.height == reference.height * factor;

            if (factor > 1) {
                fine.width = reference.width;
                fine.height = reference.height;

                if (!reuse) {
                    large.width = 0;
                }

                drawFine(document, *pool, fine, factor, large);
                premultiply(fine, finePremultiplied);
            }

            const Pixels& scored = factor > 1 ? fine : ours;
            const Buffer& scoredPremultiplied = factor > 1 ? finePremultiplied : oursPremultiplied;
            char text[32];

            snprintf(text, sizeof(text), " %.9f", error(scored, reference));
            errors << StringView(text);
            snprintf(text, sizeof(text), " %.9f", structural((const u8*)scoredPremultiplied.data(), (const u8*)referencePremultiplied.data(), reference.width, reference.height));
            shares << StringView(text);
            if (i == 5) {
                snprintf(text, sizeof(text), " %.9f", error(ours, reference));
                small << StringView(text);
                first.width = reference.width;
                first.height = reference.height;
                firstReference = referencePremultiplied;
                firstOurs = scoredPremultiplied;
            } else if (twice(first, reference)) {
                Buffer halved;

                halve((const u8*)referencePremultiplied.data(), reference.width, reference.height, first.width, first.height, halved);
                snprintf(text, sizeof(text), " %.9f", structural((const u8*)halved.data(), (const u8*)firstReference.data(), first.width, first.height));
                referenceScaling << StringView(text);

                if (shared) {
                    oursScaling << StringView(u8" 0");
                } else {
                    halve((const u8*)scoredPremultiplied.data(), reference.width, reference.height, first.width, first.height, halved);
                    snprintf(text, sizeof(text), " %.9f", structural((const u8*)halved.data(), (const u8*)firstOurs.data(), first.width, first.height));
                    oursScaling << StringView(text);
                }
            }
        }

        StringBuilder scores;

        scores << StringView(u8"mae") << StringView(errors) << StringView(u8"\nstruct") << StringView(shares) << StringView(u8"\naa") << StringView(small) << StringView(u8"\nreference") << StringView(referenceScaling) << StringView(u8"\nours") << StringView(oursScaling) << StringView(u8"\n");

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
        } else if (argc >= 2 && StringView(argv[1]) == StringView(u8"shrink")) {
            result = shrinkCommand(argc, argv);
        } else if (argc >= 2 && StringView(argv[1]) == StringView(u8"bench")) {
            result = bench(argc, argv);
        } else {
            sysE << StringView(u8"usage: svg render|test|shrink|bench ...") << endL;
        }
    } catch (...) {
        sysE << StringView(u8"svg: ") << Exception::current() << endL;

        result = 1;
    }

    return result;
}
