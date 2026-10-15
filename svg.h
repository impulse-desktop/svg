#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

    typedef struct SvgImage SvgImage;
    typedef struct SvgImageVtable SvgImageVtable;
    typedef struct SvgDocument SvgDocument;
    typedef struct SvgLibrary SvgLibrary;

    // Width by height RGBA pixels, 8 bits a channel, not premultiplied, length
    // bytes of them. Whoever is done with an image calls its destroy.
    struct SvgImageVtable {
        void (*destroy)(SvgImage* image);
        const void* (*data)(const SvgImage* image);
        size_t (*length)(const SvgImage* image);
        uint32_t (*width)(const SvgImage* image);
        uint32_t (*height)(const SvgImage* image);
    };

    struct SvgImage {
        const SvgImageVtable* vtable;
    };

    // Call through the image's vtable.
    void svg_image_destroy(SvgImage* image);
    const void* svg_image_data(const SvgImage* image);
    size_t svg_image_length(const SvgImage* image);
    uint32_t svg_image_width(const SvgImage* image);
    uint32_t svg_image_height(const SvgImage* image);

    // Decodes the bytes of a raster image inside a document into an image of a
    // pixel or more, or returns NULL. The renderer destroys the image once it
    // has drawn it. A library without a decoder draws no raster images.
    typedef SvgImage* (*SvgDecode)(void* context, const void* bytes, size_t length);

    // Why a call cannot do what it is asked: the file it parses is no XML
    // document, or no SVG one, or the image it draws is empty or too large.
    typedef enum SvgError {
        SVG_XML_NO_ROOT,                  // no element begins the document
        SVG_XML_TRAILING,                 // more than space after the root element
        SVG_XML_NAME,                     // no name where one begins
        SVG_XML_QUOTE_EXPECTED,           // no quote opens a literal
        SVG_XML_UNTERMINATED_LITERAL,     // no quote closes a literal
        SVG_XML_UNTERMINATED_COMMENT,     // no --> closes a comment
        SVG_XML_UNTERMINATED_PI,          // no ?> closes a processing instruction
        SVG_XML_UNTERMINATED_CDATA,       // no ]]> closes a CDATA section
        SVG_XML_UNTERMINATED_DECLARATION, // no > closes a declaration
        SVG_XML_UNTERMINATED_REFERENCE,   // no ; closes a reference soon enough
        SVG_XML_UNTERMINATED_DOCTYPE,     // no ] closes the internal subset
        SVG_XML_DOCTYPE_MARKUP,           // the internal subset holds something else
        SVG_XML_DOCTYPE_CLOSE,            // no > closes the document type
        SVG_XML_REFERENCE_EMPTY,          // a character reference without digits
        SVG_XML_REFERENCE_DIGIT,          // a character reference with a non-digit
        SVG_XML_REFERENCE_CHARACTER,      // a character reference to no character
        SVG_XML_UNKNOWN_ENTITY,           // a reference to an entity never declared
        SVG_XML_EXTERNAL_ENTITY,          // an external entity in an attribute value
        SVG_XML_ENTITY_LIMIT,             // entities expand too deep or too far
        SVG_XML_ENTITY_END_TAG,           // an end tag inside an entity
        SVG_XML_CONTENT_DECLARATION,      // a declaration among an element's content
        SVG_XML_DEPTH,                    // elements nest too deep
        SVG_XML_UNTERMINATED_TAG,         // the file ends inside a start tag
        SVG_XML_SLASH,                    // a / in a start tag not before its >
        SVG_XML_ATTRIBUTE_SPACE,          // no space before an attribute
        SVG_XML_ATTRIBUTE_EQUALS,         // no = after an attribute name
        SVG_XML_ATTRIBUTE_DUPLICATE,      // an attribute twice in one element
        SVG_XML_ATTRIBUTE_LT,             // a < in an attribute value
        SVG_XML_UNDECLARED_PREFIX,        // a namespace prefix never declared
        SVG_XML_UNTERMINATED_ELEMENT,     // the file ends inside an element
        SVG_XML_END_TAG_MISMATCH,         // an end tag of another element
        SVG_XML_END_TAG_CLOSE,            // no > closes an end tag
        SVG_NOT_SVG,                      // the root element is no SVG element
        SVG_RENDER_SIZE,                  // an image of no pixel or over 2^26
        SVG_XML_INCLUDE_PARSE,            // an xi:include to parse neither as xml nor as text
    } SvgError;

    // Called with why a call of the library, or of a document or image it
    // made, cannot return what it is asked for; an SVG image inside a document
    // that is no XML document calls it too. It does not return: it throws or
    // aborts, as a longjmp would skip what the library releases on the way
    // out. No call returns NULL.
    typedef void (*SvgFail)(SvgError error);

    // The decoder may be NULL, the fail never is.
    SvgLibrary* svg_library_create(SvgDecode decode, SvgFail fail, void* context);
    void svg_library_destroy(SvgLibrary* library);

    // The document keeps the file parsed and outlives its library; an image
    // outlives its document.
    SvgDocument* svg_library_parse(SvgLibrary* library, const void* file, size_t length);

    void svg_document_destroy(SvgDocument* document);

    // The document's own size in CSS pixels: the width and height its root
    // gives, one of them with the viewBox's ratio, or the viewBox's size. A
    // side the root leaves open, giving none or a percentage, is the right
    // or bottom edge of what the document draws, as far as it draws past
    // the origin; a document that draws nothing there has a side of zero
    // and draws nothing at any size.
    float svg_document_width(const SvgDocument* document);
    float svg_document_height(const SvgDocument* document);
    // The document drawn into width by height pixels, its own size
    // stretched to them; a viewBox fits them as its preserveAspectRatio
    // says.
    SvgImage* svg_document_render(SvgDocument* document, uint32_t width, uint32_t height);

#ifdef __cplusplus
}
#endif
