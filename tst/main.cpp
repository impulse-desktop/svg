#include "../svg.h"

#include <png.h>
#include <math.h>
#include <memory>
#include <string>
#include <time.h>
#include <vector>
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdexcept>

extern "C" {
#include <jpeglib.h>
}

namespace {
    constexpr unsigned caseSeconds = 120;
    // The former 2^27-pixel limit already admitted this much pixel storage
    // for a sixteen-bit PNG: eight-bit RGBA plus its temporary wide pixels.
    // Apply the same byte allowance to every raster instead of rejecting
    // a smaller eight-bit buffer merely because it holds more pixels.
    constexpr uint64_t maxRasterStorage = (1ull << 27) * 12;
    constexpr int structTolerance = 32;
    constexpr int unitTolerance = 2;

    static bool rasterFits(uint64_t pixels, int depth = 8) {
        uint64_t bytesPerPixel = depth == 16 ? 12 : 4;

        return pixels && pixels <= maxRasterStorage / bytesPerPixel;
    }

    struct DriverError: std::runtime_error {
        using std::runtime_error::runtime_error;
    };

    // What the renderer could not do, thrown by its fail.
    struct SvgFailure {
        SvgError error;
    };

    struct Pixels {
        std::vector<uint8_t> rgba;
        uint32_t width = 0;
        uint32_t height = 0;
    };

    // A decoded image of the driver: its pixels, handed to the renderer.
    struct DecodedImage {
        SvgImage image;
        uint8_t* rgba;
        uint32_t width;
        uint32_t height;
    };

    // An image the renderer is handed with the size its header gives and
    // its pixels decoded at the first call of data, as the renderer reads
    // them only to draw: transparent if the rest does not decode.
    struct LazyImage {
        SvgImage image;
        std::string bytes;
        uint8_t* rgba;
        uint32_t width;
        uint32_t height;
    };

    struct DestroyImage {
        void operator()(SvgImage* image) const;
    };

    struct DestroyDocument {
        void operator()(SvgDocument* document) const;
    };

    struct DestroyLibrary {
        void operator()(SvgLibrary* library) const;
    };

    using Image = std::unique_ptr<SvgImage, DestroyImage>;
    using Document = std::unique_ptr<SvgDocument, DestroyDocument>;
    using Library = std::unique_ptr<SvgLibrary, DestroyLibrary>;

    // libpng reads the file from memory through this.
    struct PngSource {
        const uint8_t* bytes;
        size_t length;
        size_t position;
    };

    // libjpeg reports an error by returning here.
    struct JpegError {
        jpeg_error_mgr manager;
        jmp_buf escape;
    };

    static uint32_t pngNumber(const uint8_t* p) {
        return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
    }

    static bool pngChecksum(const uint8_t* chunk, uint32_t length) {
        uint32_t crc = ~0u;

        for (uint32_t k = 4; k < length + 8; k++) {
            crc ^= chunk[k];

            for (int bit = 0; bit < 8; bit++) {
                crc = (crc >> 1) ^ (0xedb88320u & (0u - (crc & 1)));
            }
        }

        return ~crc == pngNumber(chunk + length + 8);
    }

    static size_t pngGamma(const uint8_t* bytes, size_t length, double& gamma) {
        static const uint8_t signature[] = {137, 80, 78, 71, 13, 10, 26, 10};

        if (length < 33 || memcmp(bytes, signature, sizeof(signature)) || pngNumber(bytes + 8) != 13 || memcmp(bytes + 12, "IHDR", 4)) {
            return 0;
        }

        size_t found = 0;

        for (size_t offset = 8; length - offset >= 12;) {
            const uint8_t* chunk = bytes + offset;
            uint32_t chunkLength = pngNumber(chunk);

            if (chunkLength > length - offset - 12) {
                return 0;
            }

            // ICC profiles are not used by this viewer. PNG's gAMA is
            // their transfer fallback; sRGB and cICP still take priority.
            if (!memcmp(chunk + 4, "sRGB", 4) || !memcmp(chunk + 4, "cICP", 4)) {
                return 0;
            }

            // cHRM describes primaries, independently of the gAMA transfer.
            if (!memcmp(chunk + 4, "gAMA", 4)) {
                if (found || chunkLength != 4 || !pngNumber(chunk + 8) || !pngChecksum(chunk, chunkLength)) {
                    return 0;
                }

                gamma = pngNumber(chunk + 8) / 100000.0;
                found = offset + 8;
            }

            if (!memcmp(chunk + 4, "IDAT", 4)) {
                return found;
            }

            offset += (size_t)chunkLength + 12;
        }

        return 0;
    }
}

void DestroyImage::operator()(SvgImage* image) const {
    svg_image_destroy(image);
}

static void destroyDecoded(SvgImage* image) {
    DecodedImage* decoded = (DecodedImage*)image;

    delete[] decoded->rgba;
    delete decoded;
}

static const void* decodedData(const SvgImage* image) {
    return ((const DecodedImage*)image)->rgba;
}

static size_t decodedLength(const SvgImage* image) {
    const DecodedImage* decoded = (const DecodedImage*)image;

    return (size_t)decoded->width * decoded->height * 4;
}

static uint32_t decodedWidth(const SvgImage* image) {
    return ((const DecodedImage*)image)->width;
}

static uint32_t decodedHeight(const SvgImage* image) {
    return ((const DecodedImage*)image)->height;
}

static const SvgImageVtable decodedVtable = {destroyDecoded, decodedData, decodedLength, decodedWidth, decodedHeight};

void DestroyDocument::operator()(SvgDocument* document) const {
    svg_document_destroy(document);
}

void DestroyLibrary::operator()(SvgLibrary* library) const {
    svg_library_destroy(library);
}

static SvgImage* decodedImage(std::unique_ptr<uint8_t[]>& rgba, uint32_t width, uint32_t height) {
    DecodedImage* out = new DecodedImage{{&decodedVtable}, rgba.release(), width, height};

    return &out->image;
}

// The sRGB encoding of a linear value, as a byte.
static uint8_t srgbByte(double linear) {
    double srgb = linear <= 0.0031308 ? 12.92 * linear : 1.055 * pow(linear, 1.0 / 2.4) - 0.055;

    return (uint8_t)(srgb * 255.0 + 0.5);
}

static void pngRead(png_structp png, png_bytep out, png_size_t length) {
    PngSource* source = (PngSource*)png_get_io_ptr(png);

    if (length > source->length - source->position) {
        png_error(png, "the file ends inside the PNG");
    }

    memcpy(out, source->bytes + source->position, length);
    source->position += length;
}

static void pngWarning(png_structp, png_const_charp) {
}

// libpng and libjpeg leave an error by a longjmp into the frame of the
// function that made their calls, so each phase of a decoder runs in a
// function of its own whose frame holds nothing that the jump could skip
// or clobber; the buffers belong to the caller.
static bool pngInfo(png_structp png, png_infop info, PngSource& source, png_uint_32& width, png_uint_32& height, int& depth) {
    int color = 0;

    if (setjmp(png_jmpbuf(png))) {
        return false;
    }

    png_set_read_fn(png, &source, pngRead);
    png_read_info(png, info);
    png_get_IHDR(png, info, &width, &height, &depth, &color, nullptr, nullptr, nullptr);

    // Every image comes out as RGBA at the file's depth, 8 or 16 bits.
    png_set_expand(png);
    png_set_gray_to_rgb(png);
    png_set_filler(png, 0xffff, PNG_FILLER_AFTER);
    png_set_interlace_handling(png);
    png_read_update_info(png, info);
    depth = png_get_bit_depth(png, info);

    return png_get_rowbytes(png, info) == (size_t)width * 4 * (depth / 8);
}

static bool pngImage(png_structp png, png_bytep* rows) {
    if (setjmp(png_jmpbuf(png))) {
        return false;
    }

    png_read_image(png, rows);

    return true;
}

// Decodes a PNG with libpng into RGBA: the samples as stored, then, where
// pngGamma finds a gAMA that rules, its transfer function applied at the
// file's depth before the samples become bytes.
static SvgImage* decodePng(const uint8_t* bytes, size_t length) {
    PngSource source{bytes, length, 0};
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, pngWarning);
    png_infop info = png ? png_create_info_struct(png) : nullptr;
    std::unique_ptr<uint8_t[]> rgba;
    std::vector<uint8_t> wide;
    std::vector<png_bytep> rows;
    double gamma = 0.0;
    png_uint_32 width = 0;
    png_uint_32 height = 0;
    int depth = 0;

    if (!info) {
        png_destroy_read_struct(&png, nullptr, nullptr);

        return nullptr;
    }

    uint64_t pixels = pngInfo(png, info, source, width, height, depth) ? (uint64_t)width * height : 0;

    if (!rasterFits(pixels, depth)) {
        png_destroy_read_struct(&png, &info, nullptr);

        return nullptr;
    }

    size_t rowBytes = (size_t)width * 4 * (depth / 8);

    rgba.reset(new uint8_t[pixels * 4]);
    rows.resize(height);

    if (depth == 16) {
        wide.resize(rowBytes * height);
    }

    for (uint32_t y = 0; y < height; y++) {
        rows[y] = (depth == 16 ? wide.data() : rgba.get()) + (size_t)y * rowBytes;
    }

    bool read = pngImage(png, rows.data());

    png_destroy_read_struct(&png, &info, nullptr);

    if (!read) {
        return nullptr;
    }

    bool gammaRules = pngGamma(bytes, length, gamma) != 0;

    if (depth == 16) {
        for (uint64_t k = 0; k < pixels * 4; k++) {
            uint32_t value = (uint32_t)wide[k * 2] << 8 | wide[k * 2 + 1];

            rgba[k] = gammaRules && k % 4 != 3 ? srgbByte(pow(value / 65535.0, 1.0 / gamma)) : (uint8_t)(value * 255.0 / 65535.0 + 0.5);
        }
    } else if (gammaRules) {
        uint8_t table[256];

        for (int k = 0; k < 256; k++) {
            table[k] = srgbByte(pow(k / 255.0, 1.0 / gamma));
        }

        for (uint64_t k = 0; k < pixels * 4; k += 4) {
            for (int c = 0; c < 3; c++) {
                rgba[k + c] = table[rgba[k + c]];
            }
        }
    }

    return decodedImage(rgba, width, height);
}

static void jpegEscape(j_common_ptr info) {
    longjmp(((JpegError*)info->err)->escape, 1);
}

static void jpegSilent(j_common_ptr) {
}

static bool jpegHeader(jpeg_decompress_struct& info, JpegError& error, const uint8_t* bytes, size_t length, bool& cmyk) {
    if (setjmp(error.escape)) {
        return false;
    }

    jpeg_create_decompress(&info);
    jpeg_mem_src(&info, bytes, (unsigned long)length);
    jpeg_read_header(&info, TRUE);
    cmyk = info.jpeg_color_space == JCS_CMYK || info.jpeg_color_space == JCS_YCCK;
    info.out_color_space = cmyk ? JCS_CMYK : JCS_RGB;
    jpeg_start_decompress(&info);

    return true;
}

// Reads the scanlines into RGBA: grayscale and YCbCr through libjpeg's own
// conversion, CMYK and YCCK through the inversion Adobe files carry.
static bool jpegImage(jpeg_decompress_struct& info, JpegError& error, bool cmyk, uint8_t* row, uint8_t* rgba) {
    uint32_t width = info.output_width;
    int components = info.output_components;

    if (setjmp(error.escape)) {
        return false;
    }

    while (info.output_scanline < info.output_height) {
        JSAMPROW lines[1] = {row};
        uint8_t* out = rgba + (size_t)info.output_scanline * width * 4;

        jpeg_read_scanlines(&info, lines, 1);

        for (uint32_t x = 0; x < width; x++) {
            const uint8_t* s = row + (size_t)x * components;

            if (cmyk) {
                // Adobe stores CMYK inverted; the others as it is.
                int k = info.saw_Adobe_marker ? s[3] : 255 - s[3];

                for (int c = 0; c < 3; c++) {
                    out[x * 4 + c] = (uint8_t)((info.saw_Adobe_marker ? s[c] : 255 - s[c]) * k / 255);
                }
            } else {
                out[x * 4] = s[0];
                out[x * 4 + 1] = s[1];
                out[x * 4 + 2] = s[2];
            }

            out[x * 4 + 3] = 255;
        }
    }

    jpeg_finish_decompress(&info);

    return true;
}

// Decodes a JPEG with libjpeg into RGBA.
static SvgImage* decodeJpeg(const uint8_t* bytes, size_t length) {
    jpeg_decompress_struct info;
    JpegError error;
    std::unique_ptr<uint8_t[]> rgba;
    std::vector<uint8_t> row;
    bool cmyk = false;

    info.err = jpeg_std_error(&error.manager);
    error.manager.error_exit = jpegEscape;
    error.manager.output_message = jpegSilent;

    if (!jpegHeader(info, error, bytes, length, cmyk)) {
        jpeg_destroy_decompress(&info);

        return nullptr;
    }

    uint32_t width = info.output_width;
    uint32_t height = info.output_height;
    uint64_t pixels = (uint64_t)width * height;

    if (!rasterFits(pixels) || info.output_components != (cmyk ? 4 : 3)) {
        jpeg_destroy_decompress(&info);

        return nullptr;
    }

    rgba.reset(new uint8_t[pixels * 4]);
    row.resize((size_t)width * info.output_components);

    bool read = jpegImage(info, error, cmyk, row.data(), rgba.get());

    jpeg_destroy_decompress(&info);

    if (!read) {
        return nullptr;
    }

    return decodedImage(rgba, width, height);
}

[[noreturn]] static void failSvg(SvgError error) {
    throw SvgFailure{error};
}

// The raster images of the corpus are PNG and JPEG; nothing else decodes.
static SvgImage* decodeEager(const void* bytes, size_t length) {
    const uint8_t* p = (const uint8_t*)bytes;

    if (length >= 8 && memcmp(p, "\x89PNG\r\n\x1a\n", 8) == 0) {
        return decodePng(p, length);
    }

    if (length >= 3 && p[0] == 0xff && p[1] == 0xd8 && p[2] == 0xff) {
        return decodeJpeg(p, length);
    }

    return nullptr;
}

// The size a PNG's or JPEG's header gives, when its pixels would decode
// into an image of the driver; false for anything else.
static bool rasterSize(const void* bytes, size_t length, uint32_t& width, uint32_t& height) {
    const uint8_t* p = (const uint8_t*)bytes;

    if (length >= 8 && memcmp(p, "\x89PNG\r\n\x1a\n", 8) == 0) {
        PngSource source{p, length, 0};
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, pngWarning);
        png_infop info = png ? png_create_info_struct(png) : nullptr;
        png_uint_32 w = 0;
        png_uint_32 h = 0;
        int depth = 0;
        bool known = info && pngInfo(png, info, source, w, h, depth);

        png_destroy_read_struct(&png, info ? &info : nullptr, nullptr);
        width = (uint32_t)w;
        height = (uint32_t)h;

        return known && rasterFits((uint64_t)w * h, depth);
    }

    if (length >= 3 && p[0] == 0xff && p[1] == 0xd8 && p[2] == 0xff) {
        jpeg_decompress_struct info;
        JpegError error;
        bool cmyk = false;

        info.err = jpeg_std_error(&error.manager);
        error.manager.error_exit = jpegEscape;
        error.manager.output_message = jpegSilent;

        bool known = jpegHeader(info, error, p, length, cmyk) && info.output_components == (cmyk ? 4 : 3);

        width = info.output_width;
        height = info.output_height;
        jpeg_destroy_decompress(&info);

        return known && rasterFits((uint64_t)width * height);
    }

    return false;
}

static void destroyLazy(SvgImage* image) {
    LazyImage* lazy = (LazyImage*)image;

    delete[] lazy->rgba;
    delete lazy;
}

static const void* lazyData(const SvgImage* image) {
    LazyImage* lazy = (LazyImage*)image;

    if (!lazy->rgba) {
        DecodedImage* decoded = (DecodedImage*)decodeEager(lazy->bytes.data(), lazy->bytes.size());

        // The decoded pixels are taken over, or the image stays transparent.
        if (decoded && decoded->width == lazy->width && decoded->height == lazy->height) {
            lazy->rgba = decoded->rgba;
            decoded->rgba = nullptr;
        } else {
            lazy->rgba = new uint8_t[(size_t)lazy->width * lazy->height * 4]();
        }

        if (decoded) {
            svg_image_destroy(&decoded->image);
        }
    }

    return lazy->rgba;
}

static size_t lazyLength(const SvgImage* image) {
    const LazyImage* lazy = (const LazyImage*)image;

    return (size_t)lazy->width * lazy->height * 4;
}

static uint32_t lazyWidth(const SvgImage* image) {
    return ((const LazyImage*)image)->width;
}

static uint32_t lazyHeight(const SvgImage* image) {
    return ((const LazyImage*)image)->height;
}

static const SvgImageVtable lazyVtable = {destroyLazy, lazyData, lazyLength, lazyWidth, lazyHeight};

// A raster image the renderer is given at its size and decodes when it draws.
static SvgImage* decodeRaster(void*, const void* bytes, size_t length) {
    uint32_t width = 0;
    uint32_t height = 0;

    if (!rasterSize(bytes, length, width, height)) {
        return nullptr;
    }

    LazyImage* lazy = new LazyImage{{&lazyVtable}, std::string((const char*)bytes, length), nullptr, width, height};

    return &lazy->image;
}

namespace {
    [[noreturn]] static void fail(const char* what, const char* path) {
        throw DriverError(std::string(what) + path);
    }

    static void readFile(const char* path, std::string& out) {
        FILE* file = fopen(path, "rb");

        if (!file) {
            fail("cannot read ", path);
        }

        char bytes[65536];
        size_t length;

        out.clear();

        while ((length = fread(bytes, 1, sizeof(bytes), file))) {
            out.append(bytes, length);
        }

        bool failed = ferror(file);

        fclose(file);

        if (failed) {
            fail("cannot read ", path);
        }
    }

    static void readPng(const char* path, Pixels& out) {
        png_image image;

        memset(&image, 0, sizeof(image));
        image.version = PNG_IMAGE_VERSION;

        if (!png_image_begin_read_from_file(&image, path)) {
            fail("cannot read ", path);
        }

        image.format = PNG_FORMAT_RGBA;
        out.width = image.width;
        out.height = image.height;
        out.rgba.assign(PNG_IMAGE_SIZE(image), 0);

        if (!png_image_finish_read(&image, nullptr, out.rgba.data(), 0, nullptr)) {
            fail("cannot decode ", path);
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
            fail("cannot write ", path);
        }
    }

    static void draw(SvgDocument* document, Pixels& pixels) {
        pixels.rgba.assign((size_t)pixels.width * pixels.height * 4, 0);

        if (!document) {
            return;
        }

        try {
            Image image(svg_document_render(document, pixels.width, pixels.height));

            if (image && svg_image_width(image.get()) == pixels.width && svg_image_height(image.get()) == pixels.height && svg_image_length(image.get()) == pixels.rgba.size()) {
                memcpy(pixels.rgba.data(), svg_image_data(image.get()), pixels.rgba.size());
            }
        } catch (...) {
            pixels.rgba.assign(pixels.rgba.size(), 0);
        }
    }

    static Document load(const char* path, std::string& file, SvgDecode decode) {
        readFile(path, file);

        try {
            Library library(svg_library_create(decode, failSvg, nullptr));

            return Document(svg_library_parse(library.get(), file.data(), file.size()));
        } catch (...) {
            return nullptr;
        }
    }

    static bool startsWith(const uint8_t* p, const uint8_t* end, const char* word) {
        size_t length = strlen(word);

        return (size_t)(end - p) >= length && memcmp(p, word, length) == 0;
    }

    static bool space(uint8_t c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r';
    }

    // Whether a character continues a name or a number.
    static bool word(uint8_t c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
    }

    // Whether a clip path, mask, filter or transform is set in a tag's
    // attributes or a style sheet's rules: named as an attribute, before =,
    // or as a property, before :, not as a word in a value.
    static bool rootEffect(const uint8_t* p, const uint8_t* end) {
        static const char* const names[] = {"clip-path", "mask", "filter", "transform"};

        for (const uint8_t* begin = p; p < end; p++) {
            if (p > begin && word(p[-1])) {
                continue;
            }

            for (const char* name : names) {
                if (!startsWith(p, end, name)) {
                    continue;
                }

                const uint8_t* q = p + strlen(name);

                while (q < end && space(*q)) {
                    q++;
                }

                if (q < end && (*q == '=' || *q == ':')) {
                    return true;
                }
            }
        }

        return false;
    }

    // Whether a length in a viewport unit, vw, vh, vi, vb, vmin or vmax,
    // begins at the character: a number that the unit ends, between the
    // characters a value has around a length, not the letters, digits and
    // slashes of base64.
    static bool viewportLength(const uint8_t* begin, const uint8_t* q, const uint8_t* end) {
        static const char* const units[] = {"vw", "vh", "vi", "vb", "vmin", "vmax"};

        if (!((*q >= '0' && *q <= '9') || *q == '.') || (q > begin && !strchr(" \t\r\n\"'(,:;", q[-1]))) {
            return false;
        }

        const uint8_t* unit = q;

        while (unit < end && ((*unit >= '0' && *unit <= '9') || *unit == '.')) {
            unit++;
        }

        for (const char* name : units) {
            size_t length = strlen(name);

            if (startsWith(unit, end, name) && (unit + length == end || strchr(" \t\r\n\"'),;", unit[length]))) {
                return true;
            }
        }

        return false;
    }

    // The value of an attribute of a tag, or an empty string.
    static std::string attributeOf(const std::string& tag, const char* name) {
        std::string key = std::string(" ") + name;
        size_t at = 0;

        while ((at = tag.find(key, at)) != std::string::npos) {
            size_t p = at + key.size();

            while (p < tag.size() && space((uint8_t)tag[p])) {
                p++;
            }

            if (p < tag.size() && tag[p] == '=') {
                p++;

                while (p < tag.size() && space((uint8_t)tag[p])) {
                    p++;
                }

                if (p < tag.size() && (tag[p] == '"' || tag[p] == '\'')) {
                    size_t close = tag.find(tag[p], p + 1);

                    return close == std::string::npos ? std::string() : tag.substr(p + 1, close - p - 1);
                }
            }

            at = p;
        }

        return std::string();
    }

    // Whether a selector's subject, its last compound, is the root: svg,
    // :root, *, the root's id or one of its classes.
    static bool selectsRoot(const std::string& selector, const std::string& rootId, const std::string& rootClasses) {
        size_t end = selector.find_last_not_of(" \t\r\n");

        if (end == std::string::npos) {
            return false;
        }

        size_t begin = selector.find_last_of(" \t\r\n>+~", end);
        std::string subject = selector.substr(begin == std::string::npos ? 0 : begin + 1, end - (begin == std::string::npos ? 0 : begin + 1) + 1);

        if (subject == "*" || subject == ":root" || subject == "svg" || subject == "svg:root" || (subject.size() > 3 && subject.compare(0, 3, "svg") == 0 && strchr("[.#:", subject[3]))) {
            return true;
        }

        if (subject.size() > 1 && subject[0] == '#' && !rootId.empty() && subject.substr(1) == rootId) {
            return true;
        }

        if (subject.size() > 1 && subject[0] == '.') {
            std::string name = subject.substr(1);
            size_t at = 0;

            while (at < rootClasses.size()) {
                size_t stop = rootClasses.find_first_of(" \t\r\n", at);

                if (stop == std::string::npos) {
                    stop = rootClasses.size();
                }

                if (rootClasses.compare(at, stop - at, name) == 0) {
                    return true;
                }

                at = stop + 1;
            }
        }

        return false;
    }

    // Whether a style sheet sets a clip path, mask, filter or transform on
    // the root: in a rule whose subject is the root. A rule inside an
    // at-rule is read as any text, as are rules the sheet does not close.
    static bool sheetRootEffect(const std::string& sheet, const std::string& rootId, const std::string& rootClasses) {
        size_t at = 0;

        while (at < sheet.size()) {
            size_t open = sheet.find('{', at);

            if (open == std::string::npos) {
                break;
            }

            size_t close = sheet.find('}', open);

            if (close == std::string::npos) {
                return rootEffect((const uint8_t*)sheet.data() + open, (const uint8_t*)sheet.data() + sheet.size());
            }

            std::string selectors = sheet.substr(at, open - at);
            size_t start = selectors.find_first_not_of(" \t\r\n");
            bool effect = rootEffect((const uint8_t*)sheet.data() + open, (const uint8_t*)sheet.data() + close);

            if (effect && start != std::string::npos && selectors[start] == '@') {
                return true;
            }

            if (effect) {
                size_t from = 0;

                while (from <= selectors.size()) {
                    size_t comma = selectors.find(',', from);

                    if (selectsRoot(selectors.substr(from, (comma == std::string::npos ? selectors.size() : comma) - from), rootId, rootClasses)) {
                        return true;
                    }

                    if (comma == std::string::npos) {
                        break;
                    }

                    from = comma + 1;
                }
            }

            at = close + 1;
        }

        return false;
    }

    // Whether the file draws the same at every size, only scaled: not with
    // a non-scaling stroke, a viewport unit or a clip, mask, filter or
    // transform on the root, which are in the pixels of the box, so a
    // drawing of more pixels is not the drawing scaled. Such a file is drawn
    // straight at the size of its reference, never supersampled, and its
    // sizes share no drawing.
    static bool scalable(const std::string& text) {
        const uint8_t* p = (const uint8_t*)text.data();
        const uint8_t* end = p + text.size();

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

            // A document type's internal subset may declare entities whose
            // values are markup: the whole declaration goes.
            if (startsWith(p, end, "<!DOCTYPE")) {
                int depth = 0;
                uint8_t inside = 0;

                while (p < end && (depth || inside || *p != '>')) {
                    if (inside && *p == inside) {
                        inside = 0;
                    } else if (!inside && (*p == '"' || *p == '\'')) {
                        inside = *p;
                    } else if (!inside && *p == '[') {
                        depth++;
                    } else if (!inside && *p == ']') {
                        depth--;
                    }

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

        const uint8_t* name = p + 1;
        const uint8_t* tagEnd = name;
        uint8_t quote = 0;

        while (tagEnd < end && (quote || *tagEnd != '>')) {
            if (quote && *tagEnd == quote) {
                quote = 0;
            } else if (!quote && (*tagEnd == '"' || *tagEnd == '\'')) {
                quote = *tagEnd;
            }

            tagEnd++;
        }

        const uint8_t* nameEnd = name;

        while (nameEnd < tagEnd && !space(*nameEnd) && *nameEnd != '/' && *nameEnd != '>') {
            nameEnd++;
        }

        bool root = (nameEnd - name == 3 && memcmp(name, "svg", 3) == 0) || (nameEnd - name > 4 && memcmp(nameEnd - 4, ":svg", 4) == 0);

        if (!root || rootEffect(nameEnd, tagEnd)) {
            return false;
        }

        std::string tag((const char*)nameEnd, (size_t)(tagEnd - nameEnd));
        std::string rootId = attributeOf(tag, "id");
        std::string rootClasses = attributeOf(tag, "class");

        for (const uint8_t* q = (const uint8_t*)text.data(); q < end; q++) {
            if (startsWith(q, end, "non-scaling") || viewportLength((const uint8_t*)text.data(), q, end)) {
                return false;
            }

            if (startsWith(q, end, "<style") || startsWith(q, end, ":style")) {
                const uint8_t* close = q;

                while (close < end && !startsWith(close, end, "</")) {
                    close++;
                }

                if (sheetRootEffect(std::string((const char*)q, (size_t)(close - q)), rootId, rootClasses)) {
                    return false;
                }
            }
        }

        return true;
    }

    static double error(const Pixels& ours, const Pixels& reference) {
        const uint8_t* a = ours.rgba.data();
        const uint8_t* b = reference.rgba.data();
        size_t count = (size_t)reference.width * reference.height;
        uint64_t sum = 0;

        for (size_t i = 0; i < count; i++, a += 4, b += 4) {
            for (int c = 0; c < 3; c++) {
                int64_t delta = (int64_t)a[c] * a[3] - (int64_t)b[c] * b[3];

                sum += (uint64_t)(delta < 0 ? -delta : delta);
            }

            int64_t alpha = ((int64_t)a[3] - (int64_t)b[3]) * 255;

            sum += (uint64_t)(alpha < 0 ? -alpha : alpha);
        }

        return count ? (double)sum / ((double)count * 4.0 * 255.0 * 255.0) : 0.0;
    }

    static void premultiply(const Pixels& pixels, std::vector<uint8_t>& out) {
        size_t length = (size_t)pixels.width * pixels.height * 4;
        const uint8_t* p = pixels.rgba.data();

        out.assign(length, 0);

        uint8_t* q = out.data();

        for (size_t i = 0; i < length; i += 4) {
            uint32_t a = p[i + 3];

            for (int c = 0; c < 3; c++) {
                q[i + c] = (uint8_t)((p[i + c] * a + 127) / 255);
            }

            q[i + 3] = (uint8_t)a;
        }
    }

    static uint8_t least(uint8_t a, uint8_t b) {
        return a < b ? a : b;
    }

    static uint8_t most(uint8_t a, uint8_t b) {
        return a > b ? a : b;
    }

    static void envelopeRow(const uint8_t* s, size_t stride, uint8_t* lo, uint8_t* hi) {
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

    static void envelope(const uint8_t* src, uint32_t width, uint32_t height, std::vector<uint8_t>& lo, std::vector<uint8_t>& hi) {
        size_t stride = (size_t)width * 4;
        size_t length = stride * height;
        std::vector<uint8_t> rowLo(length);
        std::vector<uint8_t> rowHi(length);

        lo.assign(length, 0);
        hi.assign(length, 0);

        uint8_t* rl = rowLo.data();
        uint8_t* rh = rowHi.data();
        uint8_t* l = lo.data();
        uint8_t* h = hi.data();

        for (uint32_t y = 0; y < height; y++) {
            envelopeRow(src + y * stride, stride, rl + y * stride, rh + y * stride);
        }

        for (uint32_t y = 0; y < height; y++) {
            const uint8_t* l0 = rl + (y ? y - 1 : y) * stride;
            const uint8_t* l1 = rl + y * stride;
            const uint8_t* l2 = rl + (y + 1 < height ? y + 1 : y) * stride;
            const uint8_t* h0 = rh + (y ? y - 1 : y) * stride;
            const uint8_t* h1 = rh + y * stride;
            const uint8_t* h2 = rh + (y + 1 < height ? y + 1 : y) * stride;
            uint8_t* ol = l + y * stride;
            uint8_t* oh = h + y * stride;

            for (size_t i = 0; i < stride; i++) {
                ol[i] = least(least(l0[i], l1[i]), l2[i]);
                oh[i] = most(most(h0[i], h1[i]), h2[i]);
            }
        }
    }

    static void blur(const uint8_t* src, uint32_t width, uint32_t height, std::vector<uint8_t>& out) {
        size_t stride = (size_t)width * 4;
        size_t length = stride * height;
        std::vector<uint8_t> row(length);

        out.assign(length, 0);

        uint8_t* r = row.data();
        uint8_t* o = out.data();

        for (uint32_t y = 0; y < height; y++) {
            const uint8_t* s = src + y * stride;
            uint8_t* d = r + y * stride;

            if (stride <= 4) {
                memcpy(d, s, stride);

                continue;
            }

            for (size_t i = 0; i < 4; i++) {
                d[i] = (uint8_t)((3 * s[i] + s[i + 4] + 2) / 4);
                d[stride - 4 + i] = (uint8_t)((s[stride - 8 + i] + 3 * s[stride - 4 + i] + 2) / 4);
            }

            for (size_t i = 4; i + 4 < stride; i++) {
                d[i] = (uint8_t)((s[i - 4] + 2 * s[i] + s[i + 4] + 2) / 4);
            }
        }

        for (uint32_t y = 0; y < height; y++) {
            const uint8_t* r0 = r + (y ? y - 1 : y) * stride;
            const uint8_t* r1 = r + y * stride;
            const uint8_t* r2 = r + (y + 1 < height ? y + 1 : y) * stride;
            uint8_t* d = o + y * stride;

            for (size_t i = 0; i < stride; i++) {
                d[i] = (uint8_t)((r0[i] + 2 * r1[i] + r2[i] + 2) / 4);
            }
        }
    }

    static double structural(const uint8_t* a, const uint8_t* b, uint32_t width, uint32_t height) {
        size_t count = (size_t)width * height;
        std::vector<uint8_t> aLo;
        std::vector<uint8_t> aHi;
        std::vector<uint8_t> bLo;
        std::vector<uint8_t> bHi;
        std::vector<uint8_t> aBlur;
        std::vector<uint8_t> bBlur;

        envelope(a, width, height, aLo, aHi);
        envelope(b, width, height, bLo, bHi);
        blur(a, width, height, aBlur);
        blur(b, width, height, bBlur);

        const uint8_t* al = aLo.data();
        const uint8_t* ah = aHi.data();
        const uint8_t* bl = bLo.data();
        const uint8_t* bh = bHi.data();
        const uint8_t* fa = aBlur.data();
        const uint8_t* fb = bBlur.data();
        std::vector<uint8_t> flagBuffer(count * 4);
        uint8_t* flags = flagBuffer.data();

        for (size_t i = 0; i < count * 4; i++) {
            int va = a[i];
            int vb = b[i];
            int blurred = (int)fa[i] - (int)fb[i];
            int outside = (va - bh[i] > structTolerance) | (bl[i] - va > structTolerance) | (vb - ah[i] > structTolerance) | (al[i] - vb > structTolerance);
            int apart = (blurred > structTolerance) | (-blurred > structTolerance);

            flags[i] = (uint8_t)(outside | (apart << 1));
        }

        size_t bad = 0;

        for (size_t i = 0; i < count * 4; i += 4) {
            uint8_t any = flags[i] | flags[i + 1] | flags[i + 2] | flags[i + 3];

            bad += any == 3;
        }

        return count ? (double)bad / (double)count : 0.0;
    }

    static bool twice(const Pixels& small, const Pixels& large) {
        return large.width == 2 * small.width && large.height == 2 * small.height;
    }

    static void halve(const uint8_t* src, uint32_t width, uint32_t height, uint32_t outWidth, uint32_t outHeight, std::vector<uint8_t>& out) {
        out.assign((size_t)outWidth * outHeight * 4, 0);

        uint8_t* q = out.data();

        for (uint32_t y = 0; y < outHeight; y++) {
            for (uint32_t x = 0; x < outWidth; x++) {
                uint32_t sum[4] = {0, 0, 0, 0};
                uint32_t n = 0;

                for (uint32_t yy = 2 * y; yy < 2 * y + 2 && yy < height; yy++) {
                    for (uint32_t xx = 2 * x; xx < 2 * x + 2 && xx < width; xx++) {
                        for (int c = 0; c < 4; c++) {
                            sum[c] += src[((size_t)yy * width + xx) * 4 + c];
                        }

                        n++;
                    }
                }

                for (int c = 0; c < 4; c++) {
                    q[((size_t)y * outWidth + x) * 4 + c] = n ? (uint8_t)((sum[c] + n / 2) / n) : 0;
                }
            }
        }
    }

    template <uint32_t F>
    static void shrinkBy(const uint8_t* p, size_t stride, uint32_t width, uint32_t height, uint8_t* q) {
        std::vector<uint32_t> sumBuffer((size_t)width * 4);
        uint32_t* sum = sumBuffer.data();

        for (uint32_t y = 0; y < height; y++) {
            memset(sum, 0, (size_t)width * 4 * sizeof(uint32_t));

            for (uint32_t yy = 0; yy < F; yy++) {
                const uint8_t* s = p + ((size_t)y * F + yy) * stride;

                for (uint32_t x = 0; x < width; x++) {
                    uint32_t* t = sum + (size_t)x * 4;
                    const uint8_t* r = s + (size_t)x * F * 4;

                    for (uint32_t k = 0; k < F; k++) {
                        uint32_t alpha = r[k * 4 + 3];

                        t[0] += r[k * 4] * alpha;
                        t[1] += r[k * 4 + 1] * alpha;
                        t[2] += r[k * 4 + 2] * alpha;
                        t[3] += alpha;
                    }
                }
            }

            uint8_t* d = q + (size_t)y * width * 4;

            for (uint32_t x = 0; x < width; x++) {
                const uint32_t* t = sum + (size_t)x * 4;

                if (!t[3]) {
                    continue;
                }

                for (int c = 0; c < 3; c++) {
                    uint32_t v = (t[c] + t[3] / 2) / t[3];

                    d[x * 4 + c] = (uint8_t)(v > 255 ? 255 : v);
                }

                d[x * 4 + 3] = (uint8_t)((t[3] + F * F / 2) / (F * F));
            }
        }
    }

    static void shrink(const Pixels& large, uint32_t factor, Pixels& out) {
        out.width = large.width / factor;
        out.height = large.height / factor;
        out.rgba.assign((size_t)out.width * out.height * 4, 0);

        const uint8_t* p = large.rgba.data();
        uint8_t* q = out.rgba.data();
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
                std::vector<uint64_t> sumBuffer((size_t)out.width * 4);
                uint64_t* sum = sumBuffer.data();
                uint64_t count = (uint64_t)factor * factor;

                for (uint32_t y = 0; y < out.height; y++) {
                    memset(sum, 0, (size_t)out.width * 4 * sizeof(uint64_t));

                    for (uint32_t yy = y * factor; yy < (y + 1) * factor; yy++) {
                        const uint8_t* s = p + yy * stride;

                        for (uint32_t x = 0; x < out.width * factor; x++) {
                            uint64_t* t = sum + (size_t)(x / factor) * 4;
                            uint64_t alpha = s[x * 4 + 3];

                            t[0] += s[x * 4] * alpha;
                            t[1] += s[x * 4 + 1] * alpha;
                            t[2] += s[x * 4 + 2] * alpha;
                            t[3] += alpha;
                        }
                    }

                    uint8_t* d = q + (size_t)y * out.width * 4;

                    for (uint32_t x = 0; x < out.width; x++) {
                        const uint64_t* t = sum + (size_t)x * 4;

                        if (!t[3]) {
                            continue;
                        }

                        for (int c = 0; c < 3; c++) {
                            uint64_t v = (t[c] + t[3] / 2) / t[3];

                            d[x * 4 + c] = (uint8_t)(v > 255 ? 255 : v);
                        }

                        d[x * 4 + 3] = (uint8_t)((t[3] + count / 2) / count);
                    }
                }
            }
        }
    }

    static void drawFine(SvgDocument* document, Pixels& pixels, uint32_t factor, Pixels& large) {
        if (factor <= 1) {
            draw(document, pixels);

            return;
        }

        if (large.width != pixels.width * factor || large.height != pixels.height * factor) {
            large.width = pixels.width * factor;
            large.height = pixels.height * factor;
            draw(document, large);
        }

        shrink(large, factor, pixels);
    }

    static uint32_t factorOf(const char* text) {
        int factor = atoi(text);

        if (factor < 1 || factor > 64) {
            fail("bad factor ", text);
        }

        return (uint32_t)factor;
    }

    static uint32_t sideFactor(uint32_t side, uint32_t width, uint32_t height) {
        uint32_t longest = width > height ? width : height;

        return longest && side > longest ? side / longest : 1;
    }

    static int render(int argc, char** argv) {
        if (argc != 6 && argc != 7) {
            fprintf(stderr, "usage: svg render IN.svg OUT.png WIDTH HEIGHT [SIDE]\n");

            return 2;
        }

        std::string file;
        Document document = load(argv[2], file, decodeRaster);
        Pixels pixels;

        pixels.width = (uint32_t)atoi(argv[4]);
        pixels.height = (uint32_t)atoi(argv[5]);
        Pixels large;

        drawFine(document.get(), pixels, argc == 7 ? sideFactor((uint32_t)atoi(argv[6]), pixels.width, pixels.height) : 1, large);
        writePng(argv[3], pixels);

        return 0;
    }

    // Prints the own size of every file, in CSS pixels, as "FILE WIDTH
    // HEIGHT", with " straight" after it for a file the test draws straight
    // at the size of its reference, or "FILE ? ?" for a file the renderer
    // cannot parse.
    static int size(int argc, char** argv) {
        if (argc < 3) {
            fprintf(stderr, "usage: svg size IN.svg...\n");

            return 2;
        }

        for (int i = 2; i < argc; i++) {
            std::string file;
            Document document = load(argv[i], file, decodeRaster);

            if (document) {
                printf("%s %.4f %.4f%s\n", argv[i], svg_document_width(document.get()), svg_document_height(document.get()), scalable(file) ? "" : " straight");
            } else {
                printf("%s ? ?\n", argv[i]);
            }
        }

        return 0;
    }

    // The size a unit's name asks for, as in X.32x16.svg; false when it asks
    // for none.
    static bool unitSize(const char* path, uint32_t& width, uint32_t& height) {
        std::string name(path);
        size_t dot = name.rfind('.');
        size_t start = dot == std::string::npos ? std::string::npos : name.rfind('.', dot - 1);

        if (start == std::string::npos) {
            return false;
        }

        unsigned w;
        unsigned h;
        char tail;

        if (sscanf(name.c_str() + start, ".%ux%u%c", &w, &h, &tail) != 3 || tail != '.') {
            return false;
        }

        width = w;
        height = h;

        return true;
    }

    // Draws IN.svg at its own size rounded up, or at the size its name asks
    // for, writes the drawing to OUT.png and compares it with
    // CANON.png: they match when no premultiplied channel of a pixel differs
    // by more than unitTolerance of 255. A file the renderer fails to parse
    // or to draw draws one transparent pixel. A name
    // holding .nodecoder. draws with a library that has no decoder. With
    // CANON.png as OUT.png too it canonizes.
    static int unit(int argc, char** argv) {
        if (argc != 5) {
            fprintf(stderr, "usage: svg unit IN.svg CANON.png OUT.png\n");

            return 2;
        }

        std::string file;
        Document document = load(argv[2], file, strstr(argv[2], ".nodecoder.") ? nullptr : decodeRaster);
        Pixels pixels;

        pixels.width = 1;
        pixels.height = 1;
        pixels.rgba.assign(4, 0);

        if (document) {
            float width = ceilf(svg_document_width(document.get()));
            float height = ceilf(svg_document_height(document.get()));
            uint32_t w = 0;
            uint32_t h = 0;
            bool sized = unitSize(argv[2], w, h);

            if (!sized && width >= 0.f && width <= 1e6f && height >= 0.f && height <= 1e6f) {
                w = (uint32_t)width;
                h = (uint32_t)height;
                sized = true;
            }

            try {
                if (sized) {
                    Image image(svg_document_render(document.get(), w, h));
                    const uint8_t* data = (const uint8_t*)svg_image_data(image.get());

                    pixels.width = svg_image_width(image.get());
                    pixels.height = svg_image_height(image.get());
                    pixels.rgba.assign(data, data + svg_image_length(image.get()));
                }
            } catch (const SvgFailure&) {
            }
        }

        writePng(argv[4], pixels);

        Pixels canon;

        readPng(argv[3], canon);

        if (canon.width != pixels.width || canon.height != pixels.height) {
            fprintf(stderr, "svg: %s is %ux%u, the canon %ux%u\n", argv[2], pixels.width, pixels.height, canon.width, canon.height);

            return 1;
        }

        size_t differing = 0;
        int worst = 0;

        for (size_t i = 0; i < pixels.rgba.size(); i += 4) {
            const uint8_t* a = &pixels.rgba[i];
            const uint8_t* b = &canon.rgba[i];
            int difference = abs((int)a[3] - (int)b[3]) * 255;

            for (int c = 0; c < 3; c++) {
                int d = abs((int)a[c] * a[3] - (int)b[c] * b[3]);

                difference = d > difference ? d : difference;
            }

            worst = difference > worst ? difference : worst;
            differing += difference > unitTolerance * 255;
        }

        if (differing) {
            fprintf(stderr, "svg: %s differs from the canon in %zu pixels, by up to %d of 255\n", argv[2], differing, (worst + 254) / 255);

            return 1;
        }

        return 0;
    }

    static int shrinkCommand(int argc, char** argv) {
        if (argc != 5) {
            fprintf(stderr, "usage: svg shrink IN.png OUT.png FACTOR\n");

            return 2;
        }

        uint32_t factor = factorOf(argv[4]);
        Pixels large;
        Pixels small;

        readPng(argv[2], large);

        if (large.width % factor || large.height % factor) {
            fail("size is not a multiple of the factor in ", argv[2]);
        }

        shrink(large, factor, small);
        writePng(argv[3], small);

        return 0;
    }

    static int test(int argc, char** argv) {
        if (argc < 6) {
            fprintf(stderr, "usage: svg test IN.svg SCORE SIDE REFERENCE.png...\n");

            return 2;
        }

        uint32_t side = (uint32_t)atoi(argv[4]);

        alarm(caseSeconds);

        std::string file;
        Document document = load(argv[2], file, decodeRaster);
        bool reuse = scalable(file);
        Pixels large;
        std::string errors;
        std::string shares;
        std::string small;
        std::string referenceScaling;
        std::string oursScaling;
        Pixels first;
        std::vector<uint8_t> firstReference;
        std::vector<uint8_t> firstOurs;

        for (int i = 5; i < argc; i++) {
            Pixels reference;
            Pixels ours;
            Pixels fine;
            std::vector<uint8_t> referencePremultiplied;
            std::vector<uint8_t> oursPremultiplied;
            std::vector<uint8_t> finePremultiplied;

            readPng(argv[i], reference);

            uint32_t factor = reuse ? sideFactor(side, reference.width, reference.height) : 1;

            bool plain = i == 5 || factor <= 1;

            ours.width = reference.width;
            ours.height = reference.height;
            premultiply(reference, referencePremultiplied);

            if (plain) {
                draw(document.get(), ours);
                premultiply(ours, oursPremultiplied);
            }

            bool shared = reuse && factor > 1 && large.width == reference.width * factor && large.height == reference.height * factor;

            if (factor > 1) {
                fine.width = reference.width;
                fine.height = reference.height;

                if (!reuse) {
                    large.width = 0;
                }

                drawFine(document.get(), fine, factor, large);
                premultiply(fine, finePremultiplied);
            }

            const Pixels& scored = factor > 1 ? fine : ours;
            const std::vector<uint8_t>& scoredPremultiplied = factor > 1 ? finePremultiplied : oursPremultiplied;
            char text[32];

            snprintf(text, sizeof(text), " %.9f", error(scored, reference));
            errors += text;
            snprintf(text, sizeof(text), " %.9f", structural(scoredPremultiplied.data(), referencePremultiplied.data(), reference.width, reference.height));
            shares += text;
            if (i == 5) {
                snprintf(text, sizeof(text), " %.9f", error(ours, reference));
                small += text;
                first.width = reference.width;
                first.height = reference.height;
                firstReference = referencePremultiplied;
                firstOurs = scoredPremultiplied;
            } else if (twice(first, reference)) {
                std::vector<uint8_t> halved;

                halve(referencePremultiplied.data(), reference.width, reference.height, first.width, first.height, halved);
                snprintf(text, sizeof(text), " %.9f", structural(halved.data(), firstReference.data(), first.width, first.height));
                referenceScaling += text;

                if (shared) {
                    oursScaling += " 0";
                } else {
                    halve(scoredPremultiplied.data(), reference.width, reference.height, first.width, first.height, halved);
                    snprintf(text, sizeof(text), " %.9f", structural(halved.data(), firstOurs.data(), first.width, first.height));
                    oursScaling += text;
                }
            }
        }

        std::string scores = "mae" + errors + "\nstruct" + shares + "\naa" + small + "\nreference" + referenceScaling + "\nours" + oursScaling + "\n";

        FILE* out = fopen(argv[3], "wb");

        if (!out || fwrite(scores.data(), 1, scores.size(), out) != scores.size() || fclose(out) != 0) {
            fail("cannot write ", argv[3]);
        }

        return 0;
    }

    static uint64_t nanoseconds() {
        timespec now;

        clock_gettime(CLOCK_MONOTONIC, &now);

        return (uint64_t)now.tv_sec * 1000000000ull + (uint64_t)now.tv_nsec;
    }

    static int bench(int argc, char** argv) {
        if (argc != 4) {
            fprintf(stderr, "usage: svg bench LIST REPEAT\n");

            return 2;
        }

        FILE* list = fopen(argv[2], "rb");
        int repeat = atoi(argv[3]);
        char line[4096];

        if (!list) {
            fail("cannot read ", argv[2]);
        }

        while (fgets(line, sizeof(line), list)) {
            char path[4000];
            unsigned width;
            unsigned height;

            if (sscanf(line, "%3999s %u %u", path, &width, &height) != 3) {
                continue;
            }

            std::string file;
            uint64_t best = ~0ull;
            bool ok = true;

            readFile(path, file);

            for (int r = 0; r < repeat; r++) {
                Library library;
                Document document;
                Image image;
                uint64_t start = nanoseconds();

                try {
                    library.reset(svg_library_create(decodeRaster, failSvg, nullptr));
                    document.reset(svg_library_parse(library.get(), file.data(), file.size()));
                    image.reset(document ? svg_document_render(document.get(), width, height) : nullptr);
                    ok = image != nullptr;
                } catch (...) {
                    ok = false;
                }

                uint64_t spent = nanoseconds() - start;

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
        if (argc >= 2 && strcmp(argv[1], "render") == 0) {
            result = render(argc, argv);
        } else if (argc >= 2 && strcmp(argv[1], "test") == 0) {
            result = test(argc, argv);
        } else if (argc >= 2 && strcmp(argv[1], "unit") == 0) {
            result = unit(argc, argv);
        } else if (argc >= 2 && strcmp(argv[1], "shrink") == 0) {
            result = shrinkCommand(argc, argv);
        } else if (argc >= 2 && strcmp(argv[1], "bench") == 0) {
            result = bench(argc, argv);
        } else if (argc >= 2 && strcmp(argv[1], "size") == 0) {
            result = size(argc, argv);
        } else {
            fprintf(stderr, "usage: svg render|test|unit|shrink|bench|size ...\n");
        }
    } catch (const std::exception& error) {
        fprintf(stderr, "svg: %s\n", error.what());

        result = 1;
    } catch (...) {
        fprintf(stderr, "svg: unknown error\n");

        result = 1;
    }

    return result;
}
