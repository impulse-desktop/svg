#include "svg.h"

#include <std/lib/buffer.h>
#include <std/lib/vector.h>
#include <std/mem/obj_pool.h>

#include <math.h>
#include <string.h>

using namespace stl;

#define SVG_ATOMS(ATOM)                              \
    ATOM(Svg, "svg")                                 \
    ATOM(G, "g")                                     \
    ATOM(Path, "path")                               \
    ATOM(Rect, "rect")                               \
    ATOM(Circle, "circle")                           \
    ATOM(Ellipse, "ellipse")                         \
    ATOM(Line, "line")                               \
    ATOM(Polyline, "polyline")                       \
    ATOM(Polygon, "polygon")                         \
    ATOM(Use, "use")                                 \
    ATOM(Defs, "defs")                               \
    ATOM(Symbol, "symbol")                           \
    ATOM(LinearGradient, "linearGradient")           \
    ATOM(RadialGradient, "radialGradient")           \
    ATOM(Stop, "stop")                               \
    ATOM(ClipPath, "clipPath")                       \
    ATOM(Mask, "mask")                               \
    ATOM(Pattern, "pattern")                         \
    ATOM(Marker, "marker")                           \
    ATOM(Style, "style")                             \
    ATOM(A, "a")                                     \
    ATOM(Switch, "switch")                           \
    ATOM(Image, "image")                             \
    ATOM(Id, "id")                                   \
    ATOM(Class, "class")                             \
    ATOM(X, "x")                                     \
    ATOM(Y, "y")                                     \
    ATOM(Width, "width")                             \
    ATOM(Height, "height")                           \
    ATOM(Rx, "rx")                                   \
    ATOM(Ry, "ry")                                   \
    ATOM(Cx, "cx")                                   \
    ATOM(Cy, "cy")                                   \
    ATOM(R, "r")                                     \
    ATOM(Fx, "fx")                                   \
    ATOM(Fy, "fy")                                   \
    ATOM(Fr, "fr")                                   \
    ATOM(X1, "x1")                                   \
    ATOM(Y1, "y1")                                   \
    ATOM(X2, "x2")                                   \
    ATOM(Y2, "y2")                                   \
    ATOM(Points, "points")                           \
    ATOM(D, "d")                                     \
    ATOM(Transform, "transform")                     \
    ATOM(ViewBox, "viewBox")                         \
    ATOM(PreserveAspectRatio, "preserveAspectRatio") \
    ATOM(Href, "href")                               \
    ATOM(Offset, "offset")                           \
    ATOM(GradientUnits, "gradientUnits")             \
    ATOM(GradientTransform, "gradientTransform")     \
    ATOM(SpreadMethod, "spreadMethod")               \
    ATOM(ClipPathUnits, "clipPathUnits")             \
    ATOM(SystemLanguage, "systemLanguage")           \
    ATOM(RequiredExtensions, "requiredExtensions")   \
    ATOM(Type, "type")                               \
    ATOM(Fill, "fill")                               \
    ATOM(FillOpacity, "fill-opacity")                \
    ATOM(FillRule, "fill-rule")                      \
    ATOM(Stroke, "stroke")                           \
    ATOM(StrokeWidth, "stroke-width")                \
    ATOM(StrokeOpacity, "stroke-opacity")            \
    ATOM(StrokeLinecap, "stroke-linecap")            \
    ATOM(StrokeLinejoin, "stroke-linejoin")          \
    ATOM(StrokeMiterlimit, "stroke-miterlimit")      \
    ATOM(StrokeDasharray, "stroke-dasharray")        \
    ATOM(StrokeDashoffset, "stroke-dashoffset")      \
    ATOM(Opacity, "opacity")                         \
    ATOM(Display, "display")                         \
    ATOM(Visibility, "visibility")                   \
    ATOM(Color, "color")                             \
    ATOM(StopColor, "stop-color")                    \
    ATOM(StopOpacity, "stop-opacity")                \
    ATOM(ClipPathProperty, "clip-path")              \
    ATOM(ClipRule, "clip-rule")                      \
    ATOM(FontSize, "font-size")

namespace {
    enum Atom : u32 {
        AtomNone,
#define SVG_ATOM_ENUM(name, text) Atom##name,
        SVG_ATOMS(SVG_ATOM_ENUM)
#undef SVG_ATOM_ENUM
        AtomKnown,
    };

    const char* const knownAtoms[] = {
        "",
#define SVG_ATOM_TEXT(name, text) text,
        SVG_ATOMS(SVG_ATOM_TEXT)
#undef SVG_ATOM_TEXT
    };

    constexpr u32 firstProperty = AtomFill;
    constexpr u32 propertyCount = AtomFontSize - AtomFill + 1;
    constexpr float pi = 3.14159265358979323846f;
    constexpr double piDouble = 3.14159265358979323846;
    constexpr float tolerance = 0.1f;
    constexpr u32 maxDepth = 512;
    constexpr u32 maxEntityDepth = 16;
    constexpr size_t maxExpansion = 1 << 22;

    const StringView svgNamespace(u8"http://www.w3.org/2000/svg");
    const StringView xlinkNamespace(u8"http://www.w3.org/1999/xlink");
    const StringView xmlNamespace(u8"http://www.w3.org/XML/1998/namespace");

    struct Point {
        float x;
        float y;
    };

    struct Matrix {
        float a;
        float b;
        float c;
        float d;
        float e;
        float f;
    };

    struct Box {
        float x0;
        float y0;
        float x1;
        float y1;
    };

    struct Color {
        float r;
        float g;
        float b;
        float a;
    };

    struct Viewport {
        float width;
        float height;
    };

    struct Interner {
        Vector<StringView> texts;
        Vector<u32> slots;
    };

    struct Attribute {
        u32 name;
        u32 value;
    };

    struct Node {
        u32 name;
        u32 index;
        Node* parent;
        Node* first;
        Node* last;
        Node* next;
        Node* prev;
        Attribute* attributes;
        u32 count;
        StringView text;
        u32* style;
        u32* classes;
        u32 classCount;
        bool classesReady;
    };

    struct Document {
        ObjPool* pool;
        Interner atoms;
        Node* root;
        u32 nodeCount;
        Vector<Node*> byId;
    };

    struct XmlError {};

    struct Entity {
        StringView name;
        StringView value;
        bool external;
    };

    struct Scope {
        StringView prefix;
        StringView uri;
    };

    struct RawAttribute {
        StringView name;
        StringView value;
    };

    struct Cursor {
        const u8* p;
        const u8* end;
    };

    struct Xml {
        Document* doc;
        Vector<Entity> entities;
        Vector<Scope> scopes;
        Vector<RawAttribute> raw;
        size_t expanded;
        u32 entityDepth;
    };

    enum class Combinator : u8 {
        None,
        Descendant,
        Child,
        Adjacent,
        Sibling,
    };

    enum class Test : u8 {
        Id,
        Class,
        Exists,
        Equals,
        Includes,
        Dash,
        Prefix,
        Suffix,
        Substring,
        FirstChild,
        LastChild,
        OnlyChild,
        Root,
        Never,
    };

    struct Condition {
        Test test;
        u32 name;
        StringView value;
    };

    struct Compound {
        u32 type;
        Combinator combinator;
        Condition* conditions;
        u32 count;
    };

    struct Selector {
        Compound* compounds;
        u32 count;
        u32 specificity;
    };

    struct Declaration {
        u32 property;
        u32 value;
        bool important;
    };

    struct Rule {
        Selector* selectors;
        u32 selectorCount;
        Declaration* declarations;
        u32 declarationCount;
    };

    struct Candidate {
        u32 value;
        u64 priority;
    };

    enum class Unit : u8 {
        Number,
        Px,
        Em,
        Ex,
        In,
        Cm,
        Mm,
        Pt,
        Pc,
        Percent,
    };

    struct Length {
        float value;
        Unit unit;
    };

    enum class Axis : u8 {
        X,
        Y,
        Other,
    };

    enum class PaintKind : u8 {
        None,
        Color,
        CurrentColor,
        Server,
    };

    struct PaintValue {
        PaintKind kind;
        Color color;
        u32 server;
        PaintKind fallback;
        Color fallbackColor;
    };

    enum Verb : u8 {
        VerbMove,
        VerbLine,
        VerbCubic,
        VerbClose,
    };

    struct Path {
        Vector<u8> verbs;
        Vector<Point> points;
    };

    struct Flat {
        Vector<Point> points;
        Vector<u32> ends;
        Vector<u8> closed;
    };

    enum Cap : u8 {
        CapButt,
        CapRound,
        CapSquare,
    };

    enum Join : u8 {
        JoinMiter,
        JoinRound,
        JoinBevel,
    };

    enum Winding : u8 {
        WindingNonZero,
        WindingEvenOdd,
    };

    struct ComputedStyle {
        PaintValue fill;
        PaintValue stroke;
        Color color;
        Color stopColor;
        float fillOpacity;
        float strokeOpacity;
        float strokeWidth;
        float miterLimit;
        float dashOffset;
        float opacity;
        float stopOpacity;
        float fontSize;
        float* dashes;
        u32 dashCount;
        u32 clipPath;
        u8 fillRule;
        u8 clipRule;
        u8 cap;
        u8 join;
        bool visible;
        bool displayNone;
    };

    struct Edge {
        float x0;
        float y0;
        float x1;
        float y1;
    };

    enum class PaintType : u8 {
        Solid,
        Linear,
        Radial,
    };

    enum Spread : u8 {
        SpreadPad,
        SpreadReflect,
        SpreadRepeat,
    };

    struct GradientStop {
        float offset;
        Color color;
    };

    struct DevicePaint {
        PaintType type;
        Color color;
        Matrix inverse;
        float x1;
        float y1;
        float x2;
        float y2;
        float cx;
        float cy;
        float r;
        float fx;
        float fy;
        float fr;
        u8 spread;
        GradientStop* stops;
        u32 stopCount;
    };

    struct ClipShape {
        u32 first;
        u32 count;
        u8 rule;
    };

    enum class OpKind : u8 {
        Fill,
        Begin,
        End,
    };

    struct Op {
        OpKind kind;
        u8 rule;
        u32 first;
        u32 count;
        Box bounds;
        DevicePaint* paint;
        float opacity;
        bool clipped;
        u32 clipFirst;
        u32 clipCount;
    };

    struct Lowering {
        Document* doc;
        ObjPool* pool;
        Vector<Op> ops;
        Vector<Edge> edges;
        Vector<ClipShape> clips;
        Vector<Node*> active;
        Vector<ComputedStyle*> documentStyles;
        u32 width;
        u32 height;
        u32 depth;
    };

    struct Raster {
        u32 width;
        u32 height;
        Vector<float*> layers;
        Vector<ObjPool*> pools;
        Vector<const Op*> begins;
        Vector<float> accumulation;
        Vector<float> mask;
        Vector<float> shape;
    };

    struct NamedColor {
        const char* name;
        u32 rgb;
    };

    const NamedColor namedColors[] = {
        {"aliceblue", 0xf0f8ff},
        {"antiquewhite", 0xfaebd7},
        {"aqua", 0x00ffff},
        {"aquamarine", 0x7fffd4},
        {"azure", 0xf0ffff},
        {"beige", 0xf5f5dc},
        {"bisque", 0xffe4c4},
        {"black", 0x000000},
        {"blanchedalmond", 0xffebcd},
        {"blue", 0x0000ff},
        {"blueviolet", 0x8a2be2},
        {"brown", 0xa52a2a},
        {"burlywood", 0xdeb887},
        {"cadetblue", 0x5f9ea0},
        {"chartreuse", 0x7fff00},
        {"chocolate", 0xd2691e},
        {"coral", 0xff7f50},
        {"cornflowerblue", 0x6495ed},
        {"cornsilk", 0xfff8dc},
        {"crimson", 0xdc143c},
        {"cyan", 0x00ffff},
        {"darkblue", 0x00008b},
        {"darkcyan", 0x008b8b},
        {"darkgoldenrod", 0xb8860b},
        {"darkgray", 0xa9a9a9},
        {"darkgreen", 0x006400},
        {"darkgrey", 0xa9a9a9},
        {"darkkhaki", 0xbdb76b},
        {"darkmagenta", 0x8b008b},
        {"darkolivegreen", 0x556b2f},
        {"darkorange", 0xff8c00},
        {"darkorchid", 0x9932cc},
        {"darkred", 0x8b0000},
        {"darksalmon", 0xe9967a},
        {"darkseagreen", 0x8fbc8f},
        {"darkslateblue", 0x483d8b},
        {"darkslategray", 0x2f4f4f},
        {"darkslategrey", 0x2f4f4f},
        {"darkturquoise", 0x00ced1},
        {"darkviolet", 0x9400d3},
        {"deeppink", 0xff1493},
        {"deepskyblue", 0x00bfff},
        {"dimgray", 0x696969},
        {"dimgrey", 0x696969},
        {"dodgerblue", 0x1e90ff},
        {"firebrick", 0xb22222},
        {"floralwhite", 0xfffaf0},
        {"forestgreen", 0x228b22},
        {"fuchsia", 0xff00ff},
        {"gainsboro", 0xdcdcdc},
        {"ghostwhite", 0xf8f8ff},
        {"gold", 0xffd700},
        {"goldenrod", 0xdaa520},
        {"gray", 0x808080},
        {"green", 0x008000},
        {"greenyellow", 0xadff2f},
        {"grey", 0x808080},
        {"honeydew", 0xf0fff0},
        {"hotpink", 0xff69b4},
        {"indianred", 0xcd5c5c},
        {"indigo", 0x4b0082},
        {"ivory", 0xfffff0},
        {"khaki", 0xf0e68c},
        {"lavender", 0xe6e6fa},
        {"lavenderblush", 0xfff0f5},
        {"lawngreen", 0x7cfc00},
        {"lemonchiffon", 0xfffacd},
        {"lightblue", 0xadd8e6},
        {"lightcoral", 0xf08080},
        {"lightcyan", 0xe0ffff},
        {"lightgoldenrodyellow", 0xfafad2},
        {"lightgray", 0xd3d3d3},
        {"lightgreen", 0x90ee90},
        {"lightgrey", 0xd3d3d3},
        {"lightpink", 0xffb6c1},
        {"lightsalmon", 0xffa07a},
        {"lightseagreen", 0x20b2aa},
        {"lightskyblue", 0x87cefa},
        {"lightslategray", 0x778899},
        {"lightslategrey", 0x778899},
        {"lightsteelblue", 0xb0c4de},
        {"lightyellow", 0xffffe0},
        {"lime", 0x00ff00},
        {"limegreen", 0x32cd32},
        {"linen", 0xfaf0e6},
        {"magenta", 0xff00ff},
        {"maroon", 0x800000},
        {"mediumaquamarine", 0x66cdaa},
        {"mediumblue", 0x0000cd},
        {"mediumorchid", 0xba55d3},
        {"mediumpurple", 0x9370db},
        {"mediumseagreen", 0x3cb371},
        {"mediumslateblue", 0x7b68ee},
        {"mediumspringgreen", 0x00fa9a},
        {"mediumturquoise", 0x48d1cc},
        {"mediumvioletred", 0xc71585},
        {"midnightblue", 0x191970},
        {"mintcream", 0xf5fffa},
        {"mistyrose", 0xffe4e1},
        {"moccasin", 0xffe4b5},
        {"navajowhite", 0xffdead},
        {"navy", 0x000080},
        {"oldlace", 0xfdf5e6},
        {"olive", 0x808000},
        {"olivedrab", 0x6b8e23},
        {"orange", 0xffa500},
        {"orangered", 0xff4500},
        {"orchid", 0xda70d6},
        {"palegoldenrod", 0xeee8aa},
        {"palegreen", 0x98fb98},
        {"paleturquoise", 0xafeeee},
        {"palevioletred", 0xdb7093},
        {"papayawhip", 0xffefd5},
        {"peachpuff", 0xffdab9},
        {"peru", 0xcd853f},
        {"pink", 0xffc0cb},
        {"plum", 0xdda0dd},
        {"powderblue", 0xb0e0e6},
        {"purple", 0x800080},
        {"rebeccapurple", 0x663399},
        {"red", 0xff0000},
        {"rosybrown", 0xbc8f8f},
        {"royalblue", 0x4169e1},
        {"saddlebrown", 0x8b4513},
        {"salmon", 0xfa8072},
        {"sandybrown", 0xf4a460},
        {"seagreen", 0x2e8b57},
        {"seashell", 0xfff5ee},
        {"sienna", 0xa0522d},
        {"silver", 0xc0c0c0},
        {"skyblue", 0x87ceeb},
        {"slateblue", 0x6a5acd},
        {"slategray", 0x708090},
        {"slategrey", 0x708090},
        {"snow", 0xfffafa},
        {"springgreen", 0x00ff7f},
        {"steelblue", 0x4682b4},
        {"tan", 0xd2b48c},
        {"teal", 0x008080},
        {"thistle", 0xd8bfd8},
        {"tomato", 0xff6347},
        {"turquoise", 0x40e0d0},
        {"violet", 0xee82ee},
        {"wheat", 0xf5deb3},
        {"white", 0xffffff},
        {"whitesmoke", 0xf5f5f5},
        {"yellow", 0xffff00},
        {"yellowgreen", 0x9acd32},
    };

    struct ImageImpl final: SvgImage {
        u8* pixels;
        size_t length_;
        u32 width_;
        u32 height_;

        ImageImpl(u8* pixels, u32 width, u32 height);

        const void* data() const override;
        size_t length() const override;
        u32 width() const override;
        u32 height() const override;
    };

    struct DocumentImpl final: SvgDocument {
        SvgDecoder* decoder;
        StringView source;
        float width_;
        float height_;

        DocumentImpl(SvgDecoder* decoder, StringView source, float width, float height);

        float width() const override;
        float height() const override;
        SvgImage* render(ObjPool& pool, u32 width, u32 height) override;
    };

    struct LibraryImpl final: SvgLibrary {
        SvgDecoder* decoder;

        explicit LibraryImpl(SvgDecoder* decoder);

        SvgDocument* parse(ObjPool& pool, StringView file) override;
    };

    static float clampUnit(float value) {
        return value < 0.f ? 0.f : (value > 1.f ? 1.f : value);
    }

    static float minf(float a, float b) {
        return a < b ? a : b;
    }

    static float maxf(float a, float b) {
        return a > b ? a : b;
    }

    static Matrix identity() {
        return Matrix{1.f, 0.f, 0.f, 1.f, 0.f, 0.f};
    }

    static Matrix translation(float x, float y) {
        return Matrix{1.f, 0.f, 0.f, 1.f, x, y};
    }

    static Matrix scaling(float x, float y) {
        return Matrix{x, 0.f, 0.f, y, 0.f, 0.f};
    }

    static Matrix multiply(const Matrix& m, const Matrix& n) {
        return Matrix{
            m.a * n.a + m.c * n.b,
            m.b * n.a + m.d * n.b,
            m.a * n.c + m.c * n.d,
            m.b * n.c + m.d * n.d,
            m.a * n.e + m.c * n.f + m.e,
            m.b * n.e + m.d * n.f + m.f,
        };
    }

    static Point apply(const Matrix& m, Point p) {
        return Point{m.a * p.x + m.c * p.y + m.e, m.b * p.x + m.d * p.y + m.f};
    }

    static bool invert(const Matrix& m, Matrix& out) {
        double det = (double)m.a * m.d - (double)m.b * m.c;

        if (det == 0.0 || !isfinite(det)) {
            return false;
        }

        double inv = 1.0 / det;

        out.a = (float)(m.d * inv);
        out.b = (float)(-m.b * inv);
        out.c = (float)(-m.c * inv);
        out.d = (float)(m.a * inv);
        out.e = (float)((m.c * (double)m.f - m.d * (double)m.e) * inv);
        out.f = (float)((m.b * (double)m.e - m.a * (double)m.f) * inv);

        return true;
    }

    static float matrixScale(const Matrix& m) {
        float sx = sqrtf(m.a * m.a + m.b * m.b);
        float sy = sqrtf(m.c * m.c + m.d * m.d);

        return maxf(sx, sy);
    }

    static Box emptyBox() {
        return Box{INFINITY, INFINITY, -INFINITY, -INFINITY};
    }

    static void extend(Box& box, Point p) {
        box.x0 = minf(box.x0, p.x);
        box.y0 = minf(box.y0, p.y);
        box.x1 = maxf(box.x1, p.x);
        box.y1 = maxf(box.y1, p.y);
    }

    static bool boxValid(const Box& box) {
        return box.x0 <= box.x1 && box.y0 <= box.y1;
    }

    static u8 lowerAscii(u8 c) {
        return c >= 'A' && c <= 'Z' ? (u8)(c + 32) : c;
    }

    static bool equalsIgnoreCase(StringView a, StringView b) {
        if (a.length() != b.length()) {
            return false;
        }

        for (size_t i = 0; i < a.length(); i++) {
            if (lowerAscii(a[i]) != lowerAscii(b[i])) {
                return false;
            }
        }

        return true;
    }

    static bool startsWithIgnoreCase(StringView text, StringView prefix) {
        return text.length() >= prefix.length() && equalsIgnoreCase(StringView(text.begin(), text.begin() + prefix.length()), prefix);
    }

    static bool isSpace(u8 c) {
        return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f';
    }

    static StringView trim(StringView text) {
        const u8* b = text.begin();
        const u8* e = text.end();

        while (b < e && isSpace(*b)) {
            b++;
        }

        while (e > b && isSpace(e[-1])) {
            e--;
        }

        return StringView(b, e);
    }

    static StringView keep(ObjPool& pool, const void* data, size_t length) {
        u8* copy = (u8*)pool.allocate(length ? length : 1);

        memcpy(copy, data, length);

        return StringView(copy, length);
    }

    static u32 hashOf(StringView text) {
        return (u32)text.hash64();
    }

    static void rehash(Interner& interner) {
        size_t size = interner.slots.length() ? interner.slots.length() * 2 : 1024;
        Vector<u32> slots;

        slots.zero(size);

        for (u32 atom = 1; atom < interner.texts.length(); atom++) {
            size_t i = hashOf(interner.texts[atom]) & (size - 1);

            while (slots[i]) {
                i = (i + 1) & (size - 1);
            }

            slots.mut(i) = atom;
        }

        interner.slots.xchg(slots);
    }

    static u32 intern(Interner& interner, StringView text) {
        if ((interner.texts.length() + 1) * 2 > interner.slots.length()) {
            rehash(interner);
        }

        size_t mask = interner.slots.length() - 1;
        size_t i = hashOf(text) & mask;

        while (u32 atom = interner.slots[i]) {
            if (interner.texts[atom] == text) {
                return atom;
            }

            i = (i + 1) & mask;
        }

        u32 atom = (u32)interner.texts.length();

        interner.texts.pushBack(text);
        interner.slots.mut(i) = atom;

        return atom;
    }

    static u32 lookup(const Interner& interner, StringView text) {
        if (interner.slots.length() == 0) {
            return 0;
        }

        size_t mask = interner.slots.length() - 1;
        size_t i = hashOf(text) & mask;

        while (u32 atom = interner.slots[i]) {
            if (interner.texts[atom] == text) {
                return atom;
            }

            i = (i + 1) & mask;
        }

        return 0;
    }

    static void initAtoms(Interner& interner) {
        interner.texts.pushBack(StringView());
        rehash(interner);

        for (u32 i = 1; i < AtomKnown; i++) {
            intern(interner, StringView(knownAtoms[i]));
        }
    }

    static StringView textOf(const Document& doc, u32 atom) {
        return doc.atoms.texts[atom];
    }

    static u32 attribute(const Node* node, u32 name) {
        for (u32 i = 0; i < node->count; i++) {
            if (node->attributes[i].name == name) {
                return node->attributes[i].value;
            }
        }

        return 0;
    }

    static Node* elementById(const Document& doc, u32 atom) {
        return atom && atom < doc.byId.length() ? doc.byId[atom] : nullptr;
    }

    [[noreturn]] static void fail() {
        throw XmlError();
    }

    static bool startsWith(const Cursor& c, StringView text) {
        return (size_t)(c.end - c.p) >= text.length() && memcmp(c.p, text.data(), text.length()) == 0;
    }

    static bool skipSpaces(Cursor& c) {
        const u8* start = c.p;

        while (c.p < c.end && isSpace(*c.p)) {
            c.p++;
        }

        return c.p != start;
    }

    static bool isNameStart(u8 c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == ':' || c >= 0x80;
    }

    static bool isNameChar(u8 c) {
        return isNameStart(c) || (c >= '0' && c <= '9') || c == '-' || c == '.';
    }

    static StringView readName(Cursor& c) {
        const u8* start = c.p;

        if (c.p >= c.end || !isNameStart(*c.p)) {
            fail();
        }

        while (c.p < c.end && isNameChar(*c.p)) {
            c.p++;
        }

        return StringView(start, c.p);
    }

    static void skipPast(Cursor& c, StringView terminator) {
        while ((size_t)(c.end - c.p) >= terminator.length()) {
            if (memcmp(c.p, terminator.data(), terminator.length()) == 0) {
                c.p += terminator.length();

                return;
            }

            c.p++;
        }

        fail();
    }

    static StringView readQuoted(Cursor& c) {
        if (c.p >= c.end || (*c.p != '"' && *c.p != '\'')) {
            fail();
        }

        u8 quote = *c.p++;
        const u8* start = c.p;

        while (c.p < c.end && *c.p != quote) {
            c.p++;
        }

        if (c.p >= c.end) {
            fail();
        }

        return StringView(start, c.p++);
    }

    static void skipDeclaration(Cursor& c) {
        while (c.p < c.end && *c.p != '>') {
            if (*c.p == '"' || *c.p == '\'') {
                readQuoted(c);
            } else {
                c.p++;
            }
        }

        if (c.p >= c.end) {
            fail();
        }

        c.p++;
    }

    static void appendUtf8(Buffer& out, u32 code) {
        u8 bytes[4];
        size_t length;

        if (code < 0x80) {
            bytes[0] = (u8)code;
            length = 1;
        } else if (code < 0x800) {
            bytes[0] = (u8)(0xc0 | (code >> 6));
            bytes[1] = (u8)(0x80 | (code & 0x3f));
            length = 2;
        } else if (code < 0x10000) {
            bytes[0] = (u8)(0xe0 | (code >> 12));
            bytes[1] = (u8)(0x80 | ((code >> 6) & 0x3f));
            bytes[2] = (u8)(0x80 | (code & 0x3f));
            length = 3;
        } else {
            bytes[0] = (u8)(0xf0 | (code >> 18));
            bytes[1] = (u8)(0x80 | ((code >> 12) & 0x3f));
            bytes[2] = (u8)(0x80 | ((code >> 6) & 0x3f));
            bytes[3] = (u8)(0x80 | (code & 0x3f));
            length = 4;
        }

        out.append(bytes, length);
    }

    static u32 characterReference(StringView body) {
        u32 code = 0;
        size_t i = 1;
        bool hex = body.length() > 1 && body[1] == 'x';

        if (hex) {
            i = 2;
        }

        if (i >= body.length()) {
            fail();
        }

        for (; i < body.length(); i++) {
            u8 c = body[i];
            u32 digit;

            if (c >= '0' && c <= '9') {
                digit = c - '0';
            } else if (hex && lowerAscii(c) >= 'a' && lowerAscii(c) <= 'f') {
                digit = lowerAscii(c) - 'a' + 10;
            } else {
                fail();
            }

            code = code * (hex ? 16 : 10) + digit;

            if (code > 0x10ffff) {
                fail();
            }
        }

        if (code == 0 || (code >= 0xd800 && code <= 0xdfff)) {
            fail();
        }

        return code;
    }

    static int predefinedEntity(StringView name) {
        if (name == StringView(u8"lt")) {
            return '<';
        }

        if (name == StringView(u8"gt")) {
            return '>';
        }

        if (name == StringView(u8"amp")) {
            return '&';
        }

        if (name == StringView(u8"apos")) {
            return '\'';
        }

        if (name == StringView(u8"quot")) {
            return '"';
        }

        return -1;
    }

    static const Entity* findEntity(const Xml& xml, StringView name) {
        for (const Entity& entity : xml.entities) {
            if (entity.name == name) {
                return &entity;
            }
        }

        return nullptr;
    }

    static const u8* referenceEnd(const u8* p, const u8* end) {
        const u8* limit = end - p > 64 ? p + 64 : end;

        for (const u8* q = p; q < limit; q++) {
            if (*q == ';') {
                return q;
            }
        }

        fail();
    }

    static void enterEntity(Xml& xml, const Entity& entity) {
        xml.expanded += entity.value.length();
        xml.entityDepth++;

        if (xml.expanded > maxExpansion || xml.entityDepth > maxEntityDepth) {
            fail();
        }
    }

    static void decodeAttribute(Xml& xml, StringView raw, Buffer& out) {
        const u8* p = raw.begin();
        const u8* end = raw.end();

        while (p < end) {
            u8 c = *p;

            if (c == '<') {
                fail();
            }

            if (c == '&') {
                const u8* semicolon = referenceEnd(p + 1, end);
                StringView name(p + 1, semicolon);

                if (!name.empty() && name[0] == '#') {
                    appendUtf8(out, characterReference(name));
                } else if (int value = predefinedEntity(name); value >= 0) {
                    u8 byte = (u8)value;

                    out.append(&byte, 1);
                } else {
                    const Entity* entity = findEntity(xml, name);

                    if (!entity || entity->external) {
                        fail();
                    }

                    enterEntity(xml, *entity);
                    decodeAttribute(xml, entity->value, out);
                    xml.entityDepth--;
                }

                p = semicolon + 1;
                continue;
            }

            if (c == '\r' && p + 1 < end && p[1] == '\n') {
                p++;
                continue;
            }

            u8 byte = c == '\t' || c == '\n' || c == '\r' ? (u8)' ' : c;

            out.append(&byte, 1);
            p++;
        }
    }

    static u32 internAttribute(Xml& xml, StringView raw) {
        bool plain = true;

        for (u8 c : raw) {
            if (c == '&' || c == '<' || c == '\t' || c == '\n' || c == '\r') {
                plain = false;
                break;
            }
        }

        if (plain) {
            return intern(xml.doc->atoms, raw);
        }

        Buffer out;

        decodeAttribute(xml, raw, out);

        return intern(xml.doc->atoms, keep(*xml.doc->pool, out.data(), out.length()));
    }

    static StringView expandCharacterReferences(Xml& xml, StringView literal) {
        bool plain = true;

        for (u8 c : literal) {
            if (c == '&') {
                plain = false;
                break;
            }
        }

        if (plain) {
            return literal;
        }

        Buffer out;
        const u8* p = literal.begin();

        while (p < literal.end()) {
            if (*p == '&' && p + 1 < literal.end() && p[1] == '#') {
                const u8* semicolon = referenceEnd(p + 1, literal.end());

                appendUtf8(out, characterReference(StringView(p + 1, semicolon)));
                p = semicolon + 1;
            } else {
                out.append(p, 1);
                p++;
            }
        }

        return keep(*xml.doc->pool, out.data(), out.length());
    }

    static void parseEntity(Xml& xml, Cursor& c) {
        c.p += 8;
        skipSpaces(c);

        bool parameter = false;

        if (c.p < c.end && *c.p == '%') {
            parameter = true;
            c.p++;
            skipSpaces(c);
        }

        Entity entity{readName(c), StringView(), false};

        skipSpaces(c);

        if (c.p < c.end && (*c.p == '"' || *c.p == '\'')) {
            entity.value = expandCharacterReferences(xml, readQuoted(c));
        } else {
            entity.external = true;
        }

        skipDeclaration(c);

        if (!parameter && !findEntity(xml, entity.name)) {
            xml.entities.pushBack(entity);
        }
    }

    static void parseDoctype(Xml& xml, Cursor& c) {
        c.p += 9;
        skipSpaces(c);
        readName(c);
        skipSpaces(c);

        if (startsWith(c, StringView(u8"SYSTEM"))) {
            c.p += 6;
            skipSpaces(c);
            readQuoted(c);
            skipSpaces(c);
        } else if (startsWith(c, StringView(u8"PUBLIC"))) {
            c.p += 6;
            skipSpaces(c);
            readQuoted(c);
            skipSpaces(c);
            readQuoted(c);
            skipSpaces(c);
        }

        if (c.p < c.end && *c.p == '[') {
            c.p++;

            while (true) {
                skipSpaces(c);

                if (c.p >= c.end) {
                    fail();
                }

                if (*c.p == ']') {
                    c.p++;
                    break;
                }

                if (startsWith(c, StringView(u8"<!ENTITY"))) {
                    parseEntity(xml, c);
                } else if (startsWith(c, StringView(u8"<!--"))) {
                    c.p += 4;
                    skipPast(c, StringView(u8"-->"));
                } else if (startsWith(c, StringView(u8"<?"))) {
                    skipPast(c, StringView(u8"?>"));
                } else if (startsWith(c, StringView(u8"<!"))) {
                    skipDeclaration(c);
                } else if (*c.p == '%') {
                    skipPast(c, StringView(u8";"));
                } else {
                    fail();
                }
            }

            skipSpaces(c);
        }

        if (c.p >= c.end || *c.p != '>') {
            fail();
        }

        c.p++;
    }

    static void splitName(StringView name, StringView& prefix, StringView& local) {
        for (const u8* p = name.begin(); p < name.end(); p++) {
            if (*p == ':') {
                prefix = StringView(name.begin(), p);
                local = StringView(p + 1, name.end());

                return;
            }
        }

        prefix = StringView();
        local = name;
    }

    static bool resolvePrefix(const Xml& xml, StringView prefix, StringView& uri) {
        if (prefix == StringView(u8"xml")) {
            uri = xmlNamespace;

            return true;
        }

        for (size_t i = xml.scopes.length(); i > 0; i--) {
            if (xml.scopes[i - 1].prefix == prefix) {
                uri = xml.scopes[i - 1].uri;

                return true;
            }
        }

        uri = StringView();

        return prefix.empty();
    }

    static void attach(Node* parent, Node* node) {
        node->parent = parent;
        node->prev = parent->last;

        if (parent->last) {
            parent->last->next = node;
        } else {
            parent->first = node;
        }

        parent->last = node;
    }

    static void parseElement(Xml& xml, Cursor& c, Node* parent, bool foreign, u32 depth);

    static void parseContent(Xml& xml, Cursor& c, Node* parent, bool foreign, Buffer* text, u32 depth, bool inEntity) {
        while (true) {
            if (c.p >= c.end) {
                if (inEntity) {
                    return;
                }

                fail();
            }

            u8 ch = *c.p;

            if (ch == '<') {
                if (startsWith(c, StringView(u8"</"))) {
                    if (inEntity) {
                        fail();
                    }

                    return;
                }

                if (startsWith(c, StringView(u8"<!--"))) {
                    c.p += 4;
                    skipPast(c, StringView(u8"-->"));
                    continue;
                }

                if (startsWith(c, StringView(u8"<![CDATA["))) {
                    c.p += 9;

                    const u8* start = c.p;

                    skipPast(c, StringView(u8"]]>"));

                    if (text) {
                        text->append(start, c.p - 3 - start);
                    }

                    continue;
                }

                if (startsWith(c, StringView(u8"<?"))) {
                    skipPast(c, StringView(u8"?>"));
                    continue;
                }

                if (startsWith(c, StringView(u8"<!"))) {
                    fail();
                }

                parseElement(xml, c, parent, foreign, depth + 1);
                continue;
            }

            if (ch == '&') {
                const u8* semicolon = referenceEnd(c.p + 1, c.end);
                StringView name(c.p + 1, semicolon);

                if (!name.empty() && name[0] == '#') {
                    u32 code = characterReference(name);

                    if (text) {
                        appendUtf8(*text, code);
                    }
                } else if (int value = predefinedEntity(name); value >= 0) {
                    if (text) {
                        u8 byte = (u8)value;

                        text->append(&byte, 1);
                    }
                } else {
                    const Entity* entity = findEntity(xml, name);

                    if (!entity) {
                        fail();
                    }

                    if (!entity->external) {
                        Cursor inner{entity->value.begin(), entity->value.end()};

                        enterEntity(xml, *entity);
                        parseContent(xml, inner, parent, foreign, text, depth, true);
                        xml.entityDepth--;
                    }
                }

                c.p = semicolon + 1;
                continue;
            }

            const u8* start = c.p;

            while (c.p < c.end && *c.p != '<' && *c.p != '&') {
                c.p++;
            }

            if (text) {
                text->append(start, c.p - start);
            }
        }
    }

    static void parseElement(Xml& xml, Cursor& c, Node* parent, bool foreign, u32 depth) {
        if (depth > maxDepth) {
            fail();
        }

        c.p++;

        StringView name = readName(c);
        size_t rawBase = xml.raw.length();
        bool empty = false;

        while (true) {
            bool spaced = skipSpaces(c);

            if (c.p >= c.end) {
                fail();
            }

            if (*c.p == '/') {
                c.p++;

                if (c.p >= c.end || *c.p != '>') {
                    fail();
                }

                c.p++;
                empty = true;
                break;
            }

            if (*c.p == '>') {
                c.p++;
                break;
            }

            if (!spaced) {
                fail();
            }

            StringView attributeName = readName(c);

            skipSpaces(c);

            if (c.p >= c.end || *c.p != '=') {
                fail();
            }

            c.p++;
            skipSpaces(c);

            StringView value = readQuoted(c);

            for (size_t i = rawBase; i < xml.raw.length(); i++) {
                if (xml.raw[i].name == attributeName) {
                    fail();
                }
            }

            xml.raw.pushBack(RawAttribute{attributeName, value});
        }

        size_t scopeBase = xml.scopes.length();

        for (size_t i = rawBase; i < xml.raw.length(); i++) {
            StringView attributeName = xml.raw[i].name;

            if (attributeName == StringView(u8"xmlns")) {
                xml.scopes.pushBack(Scope{StringView(), textOf(*xml.doc, internAttribute(xml, xml.raw[i].value))});
            } else if (attributeName.length() > 6 && memcmp(attributeName.data(), "xmlns:", 6) == 0) {
                xml.scopes.pushBack(Scope{StringView(attributeName.begin() + 6, attributeName.end()), textOf(*xml.doc, internAttribute(xml, xml.raw[i].value))});
            }
        }

        StringView prefix;
        StringView local;
        StringView uri;

        splitName(name, prefix, local);

        if (!resolvePrefix(xml, prefix, uri)) {
            fail();
        }

        bool svg = !foreign && uri == svgNamespace;
        Node* node = nullptr;

        if (svg && (parent || !xml.doc->root)) {
            node = xml.doc->pool->make<Node>();
            node->name = intern(xml.doc->atoms, local);
            node->index = xml.doc->nodeCount++;

            if (parent) {
                attach(parent, node);
            } else {
                xml.doc->root = node;
            }

            Vector<Attribute> attributes;
            bool plainHref = false;

            for (size_t i = rawBase; i < xml.raw.length(); i++) {
                StringView attributeName = xml.raw[i].name;
                StringView attributePrefix;
                StringView attributeLocal;
                StringView attributeUri;

                splitName(attributeName, attributePrefix, attributeLocal);

                if (attributeName == StringView(u8"xmlns") || attributePrefix == StringView(u8"xmlns")) {
                    continue;
                }

                u32 atom;

                if (attributePrefix.empty()) {
                    atom = intern(xml.doc->atoms, attributeLocal);
                } else {
                    if (!resolvePrefix(xml, attributePrefix, attributeUri)) {
                        fail();
                    }

                    if (attributeUri != xlinkNamespace || attributeLocal != StringView(u8"href")) {
                        continue;
                    }

                    if (plainHref) {
                        continue;
                    }

                    atom = AtomHref;
                }

                u32 value = internAttribute(xml, xml.raw[i].value);

                if (atom == AtomHref) {
                    bool replaced = false;

                    for (size_t k = 0; k < attributes.length(); k++) {
                        if (attributes[k].name == AtomHref) {
                            if (attributePrefix.empty()) {
                                attributes.mut(k).value = value;
                            }

                            replaced = true;
                        }
                    }

                    if (attributePrefix.empty()) {
                        plainHref = true;
                    }

                    if (replaced) {
                        continue;
                    }
                }

                attributes.pushBack(Attribute{atom, value});
            }

            node->count = (u32)attributes.length();
            node->attributes = (Attribute*)xml.doc->pool->allocate(sizeof(Attribute) * (attributes.length() + 1));

            if (node->count) {
                memcpy(node->attributes, attributes.data(), sizeof(Attribute) * attributes.length());
            }
        }

        while (xml.raw.length() > rawBase) {
            xml.raw.popBack();
        }

        if (!empty) {
            Buffer text;
            bool collect = node && node->name == AtomStyle;

            parseContent(xml, c, node, !node, collect ? &text : nullptr, depth, false);
            c.p += 2;

            StringView endName = readName(c);

            if (endName != name) {
                fail();
            }

            skipSpaces(c);

            if (c.p >= c.end || *c.p != '>') {
                fail();
            }

            c.p++;

            if (collect) {
                node->text = keep(*xml.doc->pool, text.data(), text.length());
            }
        }

        while (xml.scopes.length() > scopeBase) {
            xml.scopes.popBack();
        }
    }

    static void skipMisc(Cursor& c) {
        while (true) {
            skipSpaces(c);

            if (startsWith(c, StringView(u8"<!--"))) {
                c.p += 4;
                skipPast(c, StringView(u8"-->"));
            } else if (startsWith(c, StringView(u8"<?"))) {
                skipPast(c, StringView(u8"?>"));
            } else {
                return;
            }
        }
    }

    static Node* nextNode(Node* node, Node* top) {
        if (node->first) {
            return node->first;
        }

        while (node && node != top) {
            if (node->next) {
                return node->next;
            }

            node = node->parent;
        }

        return nullptr;
    }

    static bool parseXml(Document& doc, StringView text) {
        Xml xml;

        xml.doc = &doc;
        xml.expanded = 0;
        xml.entityDepth = 0;

        Cursor c{text.begin(), text.end()};

        try {
            if (startsWith(c, StringView(u8"\xef\xbb\xbf"))) {
                c.p += 3;
            }

            while (true) {
                skipMisc(c);

                if (startsWith(c, StringView(u8"<!DOCTYPE"))) {
                    parseDoctype(xml, c);
                } else {
                    break;
                }
            }

            if (c.p >= c.end || *c.p != '<') {
                fail();
            }

            parseElement(xml, c, nullptr, false, 0);
            skipMisc(c);

            if (c.p != c.end) {
                fail();
            }
        } catch (const XmlError&) {
            return false;
        }

        doc.byId.zero(doc.atoms.texts.length());

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            u32 id = attribute(node, AtomId);

            if (id && !doc.byId[id]) {
                doc.byId.mut(id) = node;
            }
        }

        return true;
    }

    static bool isIdentStart(u8 c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '-' || c >= 0x80;
    }

    static bool isIdentChar(u8 c) {
        return isIdentStart(c) || (c >= '0' && c <= '9');
    }

    static StringView readIdent(const u8*& p, const u8* end) {
        const u8* start = p;

        if (p < end && *p == '-') {
            p++;
        }

        if (p >= end || !isIdentStart(*p) || *p == '-') {
            p = start;

            return StringView();
        }

        while (p < end && isIdentChar(*p)) {
            p++;
        }

        return StringView(start, p);
    }

    static const u8* skipCssString(const u8* p, const u8* end) {
        u8 quote = *p++;

        while (p < end && *p != quote) {
            if (*p == '\\' && p + 1 < end) {
                p++;
            }

            p++;
        }

        return p < end ? p + 1 : end;
    }

    static void stripComments(StringView text, Buffer& out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        while (p < end) {
            if (*p == '"' || *p == '\'') {
                const u8* next = skipCssString(p, end);

                out.append(p, next - p);
                p = next;
            } else if (*p == '/' && p + 1 < end && p[1] == '*') {
                p += 2;

                while (p + 1 < end && !(p[0] == '*' && p[1] == '/')) {
                    p++;
                }

                p = p + 1 < end ? p + 2 : end;
                out.append(" ", 1);
            } else {
                out.append(p, 1);
                p++;
            }
        }
    }

    static const u8* findOutside(const u8* p, const u8* end, u8 target) {
        int depth = 0;

        while (p < end) {
            u8 c = *p;

            if (c == '"' || c == '\'') {
                p = skipCssString(p, end);
                continue;
            }

            if (depth == 0 && c == target) {
                return p;
            }

            if (c == '(' || c == '[') {
                depth++;
            } else if ((c == ')' || c == ']') && depth > 0) {
                depth--;
            }

            p++;
        }

        return nullptr;
    }

    static const u8* matchingBrace(const u8* p, const u8* end) {
        int depth = 1;

        while (p < end) {
            u8 c = *p;

            if (c == '"' || c == '\'') {
                p = skipCssString(p, end);
                continue;
            }

            if (c == '{') {
                depth++;
            } else if (c == '}') {
                depth--;

                if (depth == 0) {
                    return p;
                }
            }

            p++;
        }

        return end;
    }

    static bool parseCompound(Document& doc, const u8*& p, const u8* end, Compound& compound, u32& specificity) {
        Vector<Condition> conditions;
        bool any = false;

        compound.type = 0;

        if (p < end && *p == '*') {
            p++;
            any = true;
        } else {
            StringView type = readIdent(p, end);

            if (!type.empty()) {
                compound.type = intern(doc.atoms, type);
                specificity += 1;
                any = true;
            }
        }

        while (p < end) {
            u8 c = *p;
            Condition condition{Test::Never, 0, StringView()};

            if (c == '#') {
                p++;

                StringView name = readIdent(p, end);

                if (name.empty()) {
                    return false;
                }

                condition.test = Test::Id;
                condition.name = intern(doc.atoms, name);
                specificity += 1 << 16;
            } else if (c == '.') {
                p++;

                StringView name = readIdent(p, end);

                if (name.empty()) {
                    return false;
                }

                condition.test = Test::Class;
                condition.name = intern(doc.atoms, name);
                specificity += 1 << 8;
            } else if (c == '[') {
                p++;

                while (p < end && isSpace(*p)) {
                    p++;
                }

                StringView name = readIdent(p, end);

                if (name.empty()) {
                    return false;
                }

                while (p < end && isSpace(*p)) {
                    p++;
                }

                condition.name = intern(doc.atoms, name);
                condition.test = Test::Exists;

                if (p < end && *p != ']') {
                    if (*p == '=') {
                        condition.test = Test::Equals;
                        p++;
                    } else if (p + 1 < end && p[1] == '=') {
                        switch (*p) {
                            case '~':
                                condition.test = Test::Includes;
                                break;
                            case '|':
                                condition.test = Test::Dash;
                                break;
                            case '^':
                                condition.test = Test::Prefix;
                                break;
                            case '$':
                                condition.test = Test::Suffix;
                                break;
                            case '*':
                                condition.test = Test::Substring;
                                break;
                            default:
                                return false;
                        }

                        p += 2;
                    } else {
                        return false;
                    }

                    while (p < end && isSpace(*p)) {
                        p++;
                    }

                    if (p < end && (*p == '"' || *p == '\'')) {
                        const u8* next = skipCssString(p, end);

                        condition.value = StringView(p + 1, next - 1);
                        p = next;
                    } else {
                        condition.value = readIdent(p, end);

                        if (condition.value.empty()) {
                            return false;
                        }
                    }

                    while (p < end && isSpace(*p)) {
                        p++;
                    }
                }

                if (p >= end || *p != ']') {
                    return false;
                }

                p++;
                specificity += 1 << 8;
            } else if (c == ':') {
                p++;

                if (p < end && *p == ':') {
                    p++;
                    readIdent(p, end);
                    condition.test = Test::Never;
                    specificity += 1;
                } else {
                    StringView name = readIdent(p, end);

                    if (name.empty() || (p < end && *p == '(')) {
                        return false;
                    }

                    specificity += 1 << 8;

                    if (equalsIgnoreCase(name, StringView(u8"first-child"))) {
                        condition.test = Test::FirstChild;
                    } else if (equalsIgnoreCase(name, StringView(u8"last-child"))) {
                        condition.test = Test::LastChild;
                    } else if (equalsIgnoreCase(name, StringView(u8"only-child"))) {
                        condition.test = Test::OnlyChild;
                    } else if (equalsIgnoreCase(name, StringView(u8"root"))) {
                        condition.test = Test::Root;
                    } else if (equalsIgnoreCase(name, StringView(u8"hover")) || equalsIgnoreCase(name, StringView(u8"active")) || equalsIgnoreCase(name, StringView(u8"focus")) || equalsIgnoreCase(name, StringView(u8"visited")) || equalsIgnoreCase(name, StringView(u8"target"))) {
                        condition.test = Test::Never;
                    } else {
                        return false;
                    }
                }
            } else {
                break;
            }

            conditions.pushBack(condition);
            any = true;
        }

        if (!any) {
            return false;
        }

        compound.count = (u32)conditions.length();
        compound.conditions = (Condition*)doc.pool->allocate(sizeof(Condition) * (conditions.length() + 1));

        if (compound.count) {
            memcpy(compound.conditions, conditions.data(), sizeof(Condition) * conditions.length());
        }

        return true;
    }

    static bool parseSelector(Document& doc, StringView text, Selector& selector) {
        const u8* p = text.begin();
        const u8* end = text.end();
        Vector<Compound> compounds;
        u32 specificity = 0;

        while (p < end && isSpace(*p)) {
            p++;
        }

        while (true) {
            Compound compound;

            compound.combinator = Combinator::None;

            if (!parseCompound(doc, p, end, compound, specificity)) {
                return false;
            }

            compounds.pushBack(compound);

            bool spaced = false;

            while (p < end && isSpace(*p)) {
                p++;
                spaced = true;
            }

            if (p >= end) {
                break;
            }

            Combinator combinator = Combinator::Descendant;

            if (*p == '>' || *p == '+' || *p == '~') {
                combinator = *p == '>' ? Combinator::Child : (*p == '+' ? Combinator::Adjacent : Combinator::Sibling);
                p++;

                while (p < end && isSpace(*p)) {
                    p++;
                }
            } else if (!spaced) {
                return false;
            }

            compounds.mutBack().combinator = combinator;
        }

        selector.count = (u32)compounds.length();
        selector.specificity = specificity;
        selector.compounds = (Compound*)doc.pool->allocate(sizeof(Compound) * compounds.length());

        for (u32 i = 0; i < selector.count; i++) {
            Compound compound = compounds[selector.count - 1 - i];

            compound.combinator = i + 1 < selector.count ? compounds[selector.count - 2 - i].combinator : Combinator::None;
            selector.compounds[i] = compound;
        }

        return true;
    }

    static void parseDeclarations(Document& doc, StringView block, Vector<Declaration>& out) {
        const u8* p = block.begin();
        const u8* end = block.end();

        while (p < end) {
            const u8* stop = findOutside(p, end, ';');

            if (!stop) {
                stop = end;
            }

            StringView item(p, stop);
            const u8* colon = findOutside(item.begin(), item.end(), ':');

            p = stop < end ? stop + 1 : end;

            if (!colon) {
                continue;
            }

            StringView name = trim(StringView(item.begin(), colon));
            StringView value = trim(StringView(colon + 1, item.end()));
            bool important = false;

            if (name.length() == 0 || name.length() > 64) {
                continue;
            }

            for (const u8* bang = value.end(); bang > value.begin(); bang--) {
                if (bang[-1] == '!') {
                    if (equalsIgnoreCase(trim(StringView(bang, value.end())), StringView(u8"important"))) {
                        important = true;
                        value = trim(StringView(value.begin(), bang - 1));
                    }

                    break;
                }
            }

            u8 lowered[64];

            for (size_t i = 0; i < name.length(); i++) {
                lowered[i] = lowerAscii(name[i]);
            }

            u32 property = lookup(doc.atoms, StringView(lowered, name.length()));

            if (property < firstProperty || property >= firstProperty + propertyCount || value.empty()) {
                continue;
            }

            out.pushBack(Declaration{property, intern(doc.atoms, value), important});
        }
    }

    static void parseSheet(Document& doc, Vector<Rule>& rules, StringView source) {
        Buffer cleaned;

        stripComments(source, cleaned);

        StringView text = keep(*doc.pool, cleaned.data(), cleaned.length());
        const u8* p = text.begin();
        const u8* end = text.end();

        while (p < end) {
            if (isSpace(*p)) {
                p++;
                continue;
            }

            if (end - p >= 4 && memcmp(p, "<!--", 4) == 0) {
                p += 4;
                continue;
            }

            if (end - p >= 3 && memcmp(p, "-->", 3) == 0) {
                p += 3;
                continue;
            }

            if (*p == '@') {
                const u8* q = p;

                while (q < end && *q != ';' && *q != '{') {
                    q = *q == '"' || *q == '\'' ? skipCssString(q, end) : q + 1;
                }

                if (q < end && *q == '{') {
                    q = matchingBrace(q + 1, end);
                }

                p = q < end ? q + 1 : end;
                continue;
            }

            const u8* open = findOutside(p, end, '{');

            if (!open) {
                break;
            }

            const u8* close = matchingBrace(open + 1, end);
            StringView selectors(p, open);
            StringView block(open + 1, close);

            p = close < end ? close + 1 : end;

            Vector<Selector> parsed;
            bool valid = true;
            const u8* s = selectors.begin();

            while (valid && s <= selectors.end()) {
                const u8* comma = findOutside(s, selectors.end(), ',');
                const u8* stop = comma ? comma : selectors.end();
                Selector selector;

                valid = parseSelector(doc, trim(StringView(s, stop)), selector);

                if (valid) {
                    parsed.pushBack(selector);
                }

                if (!comma) {
                    break;
                }

                s = comma + 1;
            }

            if (!valid || parsed.length() == 0) {
                continue;
            }

            Vector<Declaration> declarations;

            parseDeclarations(doc, block, declarations);

            Rule rule;

            rule.selectorCount = (u32)parsed.length();
            rule.selectors = (Selector*)doc.pool->allocate(sizeof(Selector) * parsed.length());
            memcpy(rule.selectors, parsed.data(), sizeof(Selector) * parsed.length());
            rule.declarationCount = (u32)declarations.length();
            rule.declarations = (Declaration*)doc.pool->allocate(sizeof(Declaration) * (declarations.length() + 1));

            if (rule.declarationCount) {
                memcpy(rule.declarations, declarations.data(), sizeof(Declaration) * declarations.length());
            }

            rules.pushBack(rule);
        }
    }

    static void prepareClasses(Document& doc, Node* node) {
        if (node->classesReady) {
            return;
        }

        node->classesReady = true;

        u32 value = attribute(node, AtomClass);

        if (!value) {
            return;
        }

        StringView text = textOf(doc, value);
        Vector<u32> classes;
        const u8* p = text.begin();

        while (p < text.end()) {
            while (p < text.end() && isSpace(*p)) {
                p++;
            }

            const u8* start = p;

            while (p < text.end() && !isSpace(*p)) {
                p++;
            }

            if (p > start) {
                classes.pushBack(intern(doc.atoms, StringView(start, p)));
            }
        }

        node->classCount = (u32)classes.length();
        node->classes = (u32*)doc.pool->allocate(sizeof(u32) * (classes.length() + 1));

        if (node->classCount) {
            memcpy(node->classes, classes.data(), sizeof(u32) * classes.length());
        }
    }

    static bool containsWord(StringView list, StringView word, bool dash) {
        if (dash) {
            return list == word || (list.length() > word.length() && memcmp(list.data(), word.data(), word.length()) == 0 && list[word.length()] == '-');
        }

        const u8* p = list.begin();

        while (p < list.end()) {
            while (p < list.end() && isSpace(*p)) {
                p++;
            }

            const u8* start = p;

            while (p < list.end() && !isSpace(*p)) {
                p++;
            }

            if (p > start && StringView(start, p) == word) {
                return true;
            }
        }

        return false;
    }

    static bool matchCondition(Document& doc, Node* node, const Condition& condition) {
        switch (condition.test) {
            case Test::Id:
                return attribute(node, AtomId) == condition.name;
            case Test::Class:
                prepareClasses(doc, node);

                for (u32 i = 0; i < node->classCount; i++) {
                    if (node->classes[i] == condition.name) {
                        return true;
                    }
                }

                return false;
            case Test::FirstChild:
                return !node->prev;
            case Test::LastChild:
                return !node->next;
            case Test::OnlyChild:
                return !node->prev && !node->next;
            case Test::Root:
                return node == doc.root;
            case Test::Never:
                return false;
            default:
                break;
        }

        u32 value = attribute(node, condition.name);

        if (!value) {
            return false;
        }

        StringView text = textOf(doc, value);
        StringView expected = condition.value;

        switch (condition.test) {
            case Test::Exists:
                return true;
            case Test::Equals:
                return text == expected;
            case Test::Includes:
                return containsWord(text, expected, false);
            case Test::Dash:
                return containsWord(text, expected, true);
            case Test::Prefix:
                return !expected.empty() && text.length() >= expected.length() && memcmp(text.data(), expected.data(), expected.length()) == 0;
            case Test::Suffix:
                return !expected.empty() && text.length() >= expected.length() && memcmp(text.end() - expected.length(), expected.data(), expected.length()) == 0;
            case Test::Substring:
                if (expected.empty() || text.length() < expected.length()) {
                    return false;
                }

                for (size_t i = 0; i + expected.length() <= text.length(); i++) {
                    if (memcmp(text.data() + i, expected.data(), expected.length()) == 0) {
                        return true;
                    }
                }

                return false;
            default:
                return false;
        }
    }

    static bool matchCompound(Document& doc, Node* node, const Compound& compound) {
        if (compound.type && node->name != compound.type) {
            return false;
        }

        for (u32 i = 0; i < compound.count; i++) {
            if (!matchCondition(doc, node, compound.conditions[i])) {
                return false;
            }
        }

        return true;
    }

    static bool matchFrom(Document& doc, Node* node, const Selector& selector, u32 index) {
        if (!matchCompound(doc, node, selector.compounds[index])) {
            return false;
        }

        if (index + 1 == selector.count) {
            return true;
        }

        switch (selector.compounds[index].combinator) {
            case Combinator::Descendant:
                for (Node* up = node->parent; up; up = up->parent) {
                    if (matchFrom(doc, up, selector, index + 1)) {
                        return true;
                    }
                }

                return false;
            case Combinator::Child:
                return node->parent && matchFrom(doc, node->parent, selector, index + 1);
            case Combinator::Adjacent:
                return node->prev && matchFrom(doc, node->prev, selector, index + 1);
            case Combinator::Sibling:
                for (Node* sibling = node->prev; sibling; sibling = sibling->prev) {
                    if (matchFrom(doc, sibling, selector, index + 1)) {
                        return true;
                    }
                }

                return false;
            default:
                return false;
        }
    }

    static void consider(Candidate* best, u32 property, u32 value, u64 priority) {
        Candidate& slot = best[property - firstProperty];

        if (priority >= slot.priority) {
            slot.value = value;
            slot.priority = priority;
        }
    }

    static void cascade(Document& doc) {
        Vector<Rule> rules;

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            if (node->name == AtomStyle && !node->text.empty()) {
                u32 type = attribute(node, AtomType);

                if (!type || textOf(doc, type).empty() || equalsIgnoreCase(trim(textOf(doc, type)), StringView(u8"text/css"))) {
                    parseSheet(doc, rules, node->text);
                }
            }
        }

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            Candidate best[propertyCount];

            memset(best, 0, sizeof(best));

            for (u32 i = 0; i < node->count; i++) {
                u32 name = node->attributes[i].name;

                if (name >= firstProperty && name < firstProperty + propertyCount) {
                    consider(best, name, node->attributes[i].value, 1);
                }
            }

            for (u32 r = 0; r < rules.length(); r++) {
                const Rule& rule = rules[r];
                u32 specificity = 0;
                bool matched = false;

                for (u32 s = 0; s < rule.selectorCount; s++) {
                    if (matchFrom(doc, node, rule.selectors[s], 0)) {
                        matched = true;

                        if (rule.selectors[s].specificity > specificity) {
                            specificity = rule.selectors[s].specificity;
                        }
                    }
                }

                if (!matched) {
                    continue;
                }

                for (u32 d = 0; d < rule.declarationCount; d++) {
                    const Declaration& declaration = rule.declarations[d];
                    u64 level = declaration.important ? 3 : 1;

                    consider(best, declaration.property, declaration.value, (level << 56) | ((u64)specificity << 32) | ((u64)r << 12) | d);
                }
            }

            if (u32 inlineStyle = attribute(node, AtomStyle)) {
                Vector<Declaration> declarations;

                parseDeclarations(doc, textOf(doc, inlineStyle), declarations);

                for (u32 d = 0; d < declarations.length(); d++) {
                    u64 level = declarations[d].important ? 4 : 2;

                    consider(best, declarations[d].property, declarations[d].value, (level << 56) | d);
                }
            }

            node->style = (u32*)doc.pool->allocate(sizeof(u32) * propertyCount);

            for (u32 i = 0; i < propertyCount; i++) {
                node->style[i] = best[i].value;
            }
        }
    }

    static bool readNumber(const u8*& p, const u8* end, float& out) {
        const u8* s = p;
        double sign = 1.0;
        double value = 0.0;
        bool digits = false;

        if (s < end && (*s == '+' || *s == '-')) {
            sign = *s == '-' ? -1.0 : 1.0;
            s++;
        }

        while (s < end && *s >= '0' && *s <= '9') {
            value = value * 10.0 + (*s - '0');
            digits = true;
            s++;
        }

        if (s < end && *s == '.') {
            double scale = 0.1;

            s++;

            while (s < end && *s >= '0' && *s <= '9') {
                value += (*s - '0') * scale;
                scale *= 0.1;
                digits = true;
                s++;
            }
        }

        if (!digits) {
            return false;
        }

        if (s < end && (*s == 'e' || *s == 'E')) {
            const u8* e = s + 1;
            int exponentSign = 1;
            int exponent = 0;
            bool exponentDigits = false;

            if (e < end && (*e == '+' || *e == '-')) {
                exponentSign = *e == '-' ? -1 : 1;
                e++;
            }

            while (e < end && *e >= '0' && *e <= '9') {
                exponent = exponent < 10000 ? exponent * 10 + (*e - '0') : exponent;
                exponentDigits = true;
                e++;
            }

            if (exponentDigits) {
                value *= pow(10.0, exponentSign * exponent);
                s = e;
            }
        }

        double result = sign * value;

        if (!isfinite(result) || fabs(result) > 3.0e38) {
            return false;
        }

        out = (float)result;
        p = s;

        return true;
    }

    static void skipSeparator(const u8*& p, const u8* end) {
        while (p < end && isSpace(*p)) {
            p++;
        }

        if (p < end && *p == ',') {
            p++;

            while (p < end && isSpace(*p)) {
                p++;
            }
        }
    }

    static bool parseNumberText(StringView text, float& out) {
        StringView body = trim(text);
        const u8* p = body.begin();

        return readNumber(p, body.end(), out) && p == body.end();
    }

    static bool readLength(const u8*& p, const u8* end, Length& out) {
        const u8* s = p;

        if (!readNumber(s, end, out.value)) {
            return false;
        }

        out.unit = Unit::Number;

        if (s < end && *s == '%') {
            out.unit = Unit::Percent;
            s++;
        } else if (end - s >= 2 && isIdentStart(*s) && *s != '-') {
            StringView unit(s, s + 2);

            if (equalsIgnoreCase(unit, StringView(u8"px"))) {
                out.unit = Unit::Px;
            } else if (equalsIgnoreCase(unit, StringView(u8"em"))) {
                out.unit = Unit::Em;
            } else if (equalsIgnoreCase(unit, StringView(u8"ex"))) {
                out.unit = Unit::Ex;
            } else if (equalsIgnoreCase(unit, StringView(u8"in"))) {
                out.unit = Unit::In;
            } else if (equalsIgnoreCase(unit, StringView(u8"cm"))) {
                out.unit = Unit::Cm;
            } else if (equalsIgnoreCase(unit, StringView(u8"mm"))) {
                out.unit = Unit::Mm;
            } else if (equalsIgnoreCase(unit, StringView(u8"pt"))) {
                out.unit = Unit::Pt;
            } else if (equalsIgnoreCase(unit, StringView(u8"pc"))) {
                out.unit = Unit::Pc;
            } else {
                return false;
            }

            s += 2;

            if (s < end && isIdentChar(*s)) {
                return false;
            }
        } else if (s < end && isIdentStart(*s) && *s != '-') {
            return false;
        }

        p = s;

        return true;
    }

    static bool parseLength(StringView text, Length& out) {
        StringView body = trim(text);
        const u8* p = body.begin();

        return readLength(p, body.end(), out) && p == body.end();
    }

    static float resolveLength(Length length, Axis axis, Viewport vp, float fontSize) {
        switch (length.unit) {
            case Unit::Number:
            case Unit::Px:
                return length.value;
            case Unit::Em:
                return length.value * fontSize;
            case Unit::Ex:
                return length.value * fontSize * 0.5f;
            case Unit::In:
                return length.value * 96.f;
            case Unit::Cm:
                return length.value * 96.f / 2.54f;
            case Unit::Mm:
                return length.value * 96.f / 25.4f;
            case Unit::Pt:
                return length.value * 4.f / 3.f;
            case Unit::Pc:
                return length.value * 16.f;
            case Unit::Percent:
                if (axis == Axis::X) {
                    return length.value * vp.width / 100.f;
                }

                if (axis == Axis::Y) {
                    return length.value * vp.height / 100.f;
                }

                return length.value * sqrtf((vp.width * vp.width + vp.height * vp.height) / 2.f) / 100.f;
        }

        return 0.f;
    }

    static bool lengthAttribute(const Document& doc, const Node* node, u32 name, Axis axis, Viewport vp, float fontSize, float& out) {
        u32 value = attribute(node, name);
        Length length;

        if (!value || !parseLength(textOf(doc, value), length)) {
            return false;
        }

        out = resolveLength(length, axis, vp, fontSize);

        return true;
    }

    static float lengthOr(const Document& doc, const Node* node, u32 name, Axis axis, Viewport vp, float fontSize, float fallback) {
        float value;

        return lengthAttribute(doc, node, name, axis, vp, fontSize, value) ? value : fallback;
    }

    static int hexDigit(u8 c) {
        if (c >= '0' && c <= '9') {
            return c - '0';
        }

        c = lowerAscii(c);

        if (c >= 'a' && c <= 'f') {
            return c - 'a' + 10;
        }

        return -1;
    }

    static bool readComponent(const u8*& p, const u8* end, float scale, float& out) {
        float value;

        while (p < end && isSpace(*p)) {
            p++;
        }

        if (!readNumber(p, end, value)) {
            return false;
        }

        if (p < end && *p == '%') {
            p++;
            out = value / 100.f;
        } else {
            out = value / scale;
        }

        return true;
    }

    static float hueToRgb(float m1, float m2, float h) {
        if (h < 0.f) {
            h += 1.f;
        }

        if (h > 1.f) {
            h -= 1.f;
        }

        if (h * 6.f < 1.f) {
            return m1 + (m2 - m1) * h * 6.f;
        }

        if (h * 2.f < 1.f) {
            return m2;
        }

        if (h * 3.f < 2.f) {
            return m1 + (m2 - m1) * (2.f / 3.f - h) * 6.f;
        }

        return m1;
    }

    static bool parseFunctionColor(StringView text, Color& out) {
        const u8* open = text.begin();

        while (open < text.end() && *open != '(') {
            open++;
        }

        if (open >= text.end() || text.back() != ')') {
            return false;
        }

        StringView name = trim(StringView(text.begin(), open));
        bool hsl = equalsIgnoreCase(name, StringView(u8"hsl")) || equalsIgnoreCase(name, StringView(u8"hsla"));
        bool rgb = equalsIgnoreCase(name, StringView(u8"rgb")) || equalsIgnoreCase(name, StringView(u8"rgba"));

        if (!hsl && !rgb) {
            return false;
        }

        const u8* p = open + 1;
        const u8* end = text.end() - 1;
        float values[4] = {0.f, 0.f, 0.f, 1.f};
        u32 count = 0;

        while (count < 4) {
            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end) {
                break;
            }

            if (count > 0 && (*p == ',' || *p == '/')) {
                p++;
            }

            float scale = 255.f;

            if (hsl) {
                scale = count == 0 ? 360.f : 100.f;
            }

            if (count == 3) {
                scale = 1.f;
            }

            if (hsl && count == 0) {
                while (p < end && isSpace(*p)) {
                    p++;
                }

                float hue;

                if (!readNumber(p, end, hue)) {
                    return false;
                }

                if (end - p >= 3 && equalsIgnoreCase(StringView(p, p + 3), StringView(u8"deg"))) {
                    p += 3;
                } else if (end - p >= 4 && equalsIgnoreCase(StringView(p, p + 4), StringView(u8"turn"))) {
                    p += 4;
                    hue *= 360.f;
                } else if (end - p >= 3 && equalsIgnoreCase(StringView(p, p + 3), StringView(u8"rad"))) {
                    p += 3;
                    hue *= 180.f / pi;
                }

                values[0] = hue / 360.f;
            } else if (!readComponent(p, end, scale, values[count])) {
                return false;
            }

            count++;
        }

        while (p < end && isSpace(*p)) {
            p++;
        }

        if (p != end || count < 3) {
            return false;
        }

        if (hsl) {
            float h = values[0] - floorf(values[0]);
            float s = clampUnit(values[1]);
            float l = clampUnit(values[2]);
            float m2 = l <= 0.5f ? l * (s + 1.f) : l + s - l * s;
            float m1 = l * 2.f - m2;

            out = Color{hueToRgb(m1, m2, h + 1.f / 3.f), hueToRgb(m1, m2, h), hueToRgb(m1, m2, h - 1.f / 3.f), clampUnit(values[3])};
        } else {
            out = Color{clampUnit(values[0]), clampUnit(values[1]), clampUnit(values[2]), clampUnit(values[3])};
        }

        return true;
    }

    static bool parseColor(StringView source, Color& out) {
        StringView text = trim(source);

        if (text.empty()) {
            return false;
        }

        if (text[0] == '#') {
            size_t length = text.length() - 1;
            int digits[8];

            if (length != 3 && length != 4 && length != 6 && length != 8) {
                return false;
            }

            for (size_t i = 0; i < length; i++) {
                digits[i] = hexDigit(text[i + 1]);

                if (digits[i] < 0) {
                    return false;
                }
            }

            if (length <= 4) {
                out = Color{digits[0] * 17 / 255.f, digits[1] * 17 / 255.f, digits[2] * 17 / 255.f, length == 4 ? digits[3] * 17 / 255.f : 1.f};
            } else {
                out = Color{(digits[0] * 16 + digits[1]) / 255.f, (digits[2] * 16 + digits[3]) / 255.f, (digits[4] * 16 + digits[5]) / 255.f, length == 8 ? (digits[6] * 16 + digits[7]) / 255.f : 1.f};
            }

            return true;
        }

        if (equalsIgnoreCase(text, StringView(u8"transparent"))) {
            out = Color{0.f, 0.f, 0.f, 0.f};

            return true;
        }

        if (text.back() == ')') {
            return parseFunctionColor(text, out);
        }

        for (const NamedColor& named : namedColors) {
            if (equalsIgnoreCase(text, StringView(named.name))) {
                out = Color{((named.rgb >> 16) & 0xff) / 255.f, ((named.rgb >> 8) & 0xff) / 255.f, (named.rgb & 0xff) / 255.f, 1.f};

                return true;
            }
        }

        return false;
    }

    static bool parsePaint(const Document& doc, StringView source, PaintValue& out) {
        StringView text = trim(source);

        out.server = 0;
        out.fallback = PaintKind::None;

        if (equalsIgnoreCase(text, StringView(u8"none"))) {
            out.kind = PaintKind::None;

            return true;
        }

        if (equalsIgnoreCase(text, StringView(u8"currentcolor"))) {
            out.kind = PaintKind::CurrentColor;

            return true;
        }

        if (startsWithIgnoreCase(text, StringView(u8"url("))) {
            const u8* close = text.begin() + 4;

            while (close < text.end() && *close != ')') {
                close++;
            }

            if (close >= text.end()) {
                return false;
            }

            StringView reference = trim(StringView(text.begin() + 4, close));

            if (reference.length() >= 2 && (reference[0] == '"' || reference[0] == '\'') && reference.back() == reference[0]) {
                reference = StringView(reference.begin() + 1, reference.end() - 1);
            }

            out.kind = PaintKind::Server;

            if (!reference.empty() && reference[0] == '#') {
                out.server = lookup(doc.atoms, StringView(reference.begin() + 1, reference.end()));
            }

            StringView fallback = trim(StringView(close + 1, text.end()));

            if (!fallback.empty()) {
                if (equalsIgnoreCase(fallback, StringView(u8"none"))) {
                    out.fallback = PaintKind::None;
                } else if (equalsIgnoreCase(fallback, StringView(u8"currentcolor"))) {
                    out.fallback = PaintKind::CurrentColor;
                } else if (parseColor(fallback, out.fallbackColor)) {
                    out.fallback = PaintKind::Color;
                } else {
                    return false;
                }
            }

            return true;
        }

        if (parseColor(text, out.color)) {
            out.kind = PaintKind::Color;

            return true;
        }

        return false;
    }

    static bool parseTransform(StringView text, Matrix& out) {
        const u8* p = text.begin();
        const u8* end = text.end();
        Matrix result = identity();

        while (true) {
            while (p < end && (isSpace(*p) || *p == ',')) {
                p++;
            }

            if (p >= end) {
                break;
            }

            const u8* start = p;

            while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))) {
                p++;
            }

            StringView name(start, p);

            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end || *p != '(') {
                return false;
            }

            p++;

            float args[6];
            u32 count = 0;

            while (true) {
                while (p < end && isSpace(*p)) {
                    p++;
                }

                if (p < end && *p == ')') {
                    p++;
                    break;
                }

                if (count == 6 || !readNumber(p, end, args[count])) {
                    return false;
                }

                count++;
                skipSeparator(p, end);
            }

            Matrix m;

            if (name == StringView(u8"matrix") && count == 6) {
                m = Matrix{args[0], args[1], args[2], args[3], args[4], args[5]};
            } else if (name == StringView(u8"translate") && (count == 1 || count == 2)) {
                m = translation(args[0], count == 2 ? args[1] : 0.f);
            } else if (name == StringView(u8"scale") && (count == 1 || count == 2)) {
                m = scaling(args[0], count == 2 ? args[1] : args[0]);
            } else if (name == StringView(u8"rotate") && (count == 1 || count == 3)) {
                float angle = args[0] * pi / 180.f;
                float cs = cosf(angle);
                float sn = sinf(angle);

                m = Matrix{cs, sn, -sn, cs, 0.f, 0.f};

                if (count == 3) {
                    m = multiply(multiply(translation(args[1], args[2]), m), translation(-args[1], -args[2]));
                }
            } else if (name == StringView(u8"skewX") && count == 1) {
                m = Matrix{1.f, 0.f, tanf(args[0] * pi / 180.f), 1.f, 0.f, 0.f};
            } else if (name == StringView(u8"skewY") && count == 1) {
                m = Matrix{1.f, tanf(args[0] * pi / 180.f), 0.f, 1.f, 0.f, 0.f};
            } else {
                return false;
            }

            result = multiply(result, m);
        }

        out = result;

        return true;
    }

    static Matrix transformOf(const Document& doc, const Node* node, u32 name) {
        u32 value = attribute(node, name);
        Matrix m;

        if (value && parseTransform(textOf(doc, value), m)) {
            return m;
        }

        return identity();
    }

    static bool parseViewBox(const Document& doc, const Node* node, Box& out) {
        u32 value = attribute(node, AtomViewBox);

        if (!value) {
            return false;
        }

        StringView text = textOf(doc, value);
        const u8* p = text.begin();
        const u8* end = text.end();
        float numbers[4];

        for (u32 i = 0; i < 4; i++) {
            while (p < end && isSpace(*p)) {
                p++;
            }

            if (!readNumber(p, end, numbers[i])) {
                return false;
            }

            skipSeparator(p, end);
        }

        if (p != end || numbers[2] < 0.f || numbers[3] < 0.f) {
            return false;
        }

        out = Box{numbers[0], numbers[1], numbers[0] + numbers[2], numbers[1] + numbers[3]};

        return true;
    }

    static Matrix viewBoxTransform(const Document& doc, const Node* node, const Box& viewBox, float width, float height) {
        float vw = viewBox.x1 - viewBox.x0;
        float vh = viewBox.y1 - viewBox.y0;
        float sx = width / vw;
        float sy = height / vh;
        int alignX = 1;
        int alignY = 1;
        bool slice = false;
        bool none = false;
        u32 value = attribute(node, AtomPreserveAspectRatio);

        if (value) {
            StringView text = trim(textOf(doc, value));
            const u8* p = text.begin();
            const u8* end = text.end();
            const u8* start = p;

            while (p < end && !isSpace(*p)) {
                p++;
            }

            StringView align(start, p);

            if (align == StringView(u8"defer")) {
                while (p < end && isSpace(*p)) {
                    p++;
                }

                start = p;

                while (p < end && !isSpace(*p)) {
                    p++;
                }

                align = StringView(start, p);
            }

            if (align == StringView(u8"none")) {
                none = true;
            } else if (align.length() == 8 && align[0] == 'x' && align[4] == 'Y') {
                StringView ax(align.begin() + 1, align.begin() + 4);
                StringView ay(align.begin() + 5, align.end());

                alignX = ax == StringView(u8"Min") ? 0 : (ax == StringView(u8"Mid") ? 1 : 2);
                alignY = ay == StringView(u8"Min") ? 0 : (ay == StringView(u8"Mid") ? 1 : 2);
            }

            while (p < end && isSpace(*p)) {
                p++;
            }

            slice = StringView(p, end) == StringView(u8"slice");
        }

        if (none) {
            return multiply(scaling(sx, sy), translation(-viewBox.x0, -viewBox.y0));
        }

        float s = slice ? maxf(sx, sy) : minf(sx, sy);
        float tx = -viewBox.x0 * s;
        float ty = -viewBox.y0 * s;

        tx += (width - vw * s) * alignX * 0.5f;
        ty += (height - vh * s) * alignY * 0.5f;

        return Matrix{s, 0.f, 0.f, s, tx, ty};
    }

    static void moveTo(Path& path, Point p) {
        path.verbs.pushBack(VerbMove);
        path.points.pushBack(p);
    }

    static void lineTo(Path& path, Point p) {
        path.verbs.pushBack(VerbLine);
        path.points.pushBack(p);
    }

    static void cubicTo(Path& path, Point c1, Point c2, Point p) {
        path.verbs.pushBack(VerbCubic);
        path.points.pushBack(c1);
        path.points.pushBack(c2);
        path.points.pushBack(p);
    }

    static void closePath(Path& path) {
        path.verbs.pushBack(VerbClose);
    }

    static void arcTo(Path& path, Point p0, float rxIn, float ryIn, float angle, bool large, bool sweep, Point p1) {
        if (p0.x == p1.x && p0.y == p1.y) {
            return;
        }

        double rx = fabs((double)rxIn);
        double ry = fabs((double)ryIn);

        if (rx == 0.0 || ry == 0.0) {
            lineTo(path, p1);

            return;
        }

        double phi = fmod((double)angle, 360.0) * piDouble / 180.0;
        double cs = cos(phi);
        double sn = sin(phi);
        double dx2 = (p0.x - p1.x) / 2.0;
        double dy2 = (p0.y - p1.y) / 2.0;
        double x1p = cs * dx2 + sn * dy2;
        double y1p = -sn * dx2 + cs * dy2;
        double lambda = (x1p * x1p) / (rx * rx) + (y1p * y1p) / (ry * ry);

        if (lambda > 1.0) {
            rx *= sqrt(lambda);
            ry *= sqrt(lambda);
        }

        double num = rx * rx * ry * ry - rx * rx * y1p * y1p - ry * ry * x1p * x1p;
        double den = rx * rx * y1p * y1p + ry * ry * x1p * x1p;
        double coef = den > 0.0 ? sqrt(num / den > 0.0 ? num / den : 0.0) : 0.0;

        if (large == sweep) {
            coef = -coef;
        }

        double cxp = coef * rx * y1p / ry;
        double cyp = -coef * ry * x1p / rx;
        double cx = cs * cxp - sn * cyp + (p0.x + p1.x) / 2.0;
        double cy = sn * cxp + cs * cyp + (p0.y + p1.y) / 2.0;
        double ux = (x1p - cxp) / rx;
        double uy = (y1p - cyp) / ry;
        double vx = (-x1p - cxp) / rx;
        double vy = (-y1p - cyp) / ry;
        double theta = atan2(uy, ux);
        double delta = atan2(ux * vy - uy * vx, ux * vx + uy * vy);

        if (!sweep && delta > 0.0) {
            delta -= 2.0 * piDouble;
        } else if (sweep && delta < 0.0) {
            delta += 2.0 * piDouble;
        }

        int segments = (int)ceil(fabs(delta) / (piDouble / 2.0) - 1e-7);

        if (segments < 1) {
            segments = 1;
        }

        double step = delta / segments;
        double k = 4.0 / 3.0 * tan(step / 4.0);

        for (int i = 0; i < segments; i++) {
            double a0 = theta + step * i;
            double a1 = a0 + step;
            double c0 = cos(a0);
            double s0 = sin(a0);
            double c1 = cos(a1);
            double s1 = sin(a1);
            double e1x = c0 - k * s0;
            double e1y = s0 + k * c0;
            double e2x = c1 + k * s1;
            double e2y = s1 - k * c1;
            Point q1{(float)(cx + rx * cs * e1x - ry * sn * e1y), (float)(cy + rx * sn * e1x + ry * cs * e1y)};
            Point q2{(float)(cx + rx * cs * e2x - ry * sn * e2y), (float)(cy + rx * sn * e2x + ry * cs * e2y)};
            Point q3{(float)(cx + rx * cs * c1 - ry * sn * s1), (float)(cy + rx * sn * c1 + ry * cs * s1)};

            if (i + 1 == segments) {
                q3 = p1;
            }

            cubicTo(path, q1, q2, q3);
        }
    }

    static bool readFlag(const u8*& p, const u8* end, bool& out) {
        while (p < end && isSpace(*p)) {
            p++;
        }

        if (p >= end || (*p != '0' && *p != '1')) {
            return false;
        }

        out = *p == '1';
        p++;
        skipSeparator(p, end);

        return true;
    }

    static void parsePathData(StringView text, Path& path) {
        const u8* p = text.begin();
        const u8* end = text.end();
        Point current{0.f, 0.f};
        Point start{0.f, 0.f};
        Point control{0.f, 0.f};
        u8 command = 0;
        u8 previous = 0;
        bool needMove = false;

        while (true) {
            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end) {
                break;
            }

            u8 c = *p;

            if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
                command = c;
                p++;
            } else if (command == 0 || command == 'z' || command == 'Z') {
                break;
            } else if (command == 'M') {
                command = 'L';
            } else if (command == 'm') {
                command = 'l';
            }

            if (path.verbs.length() == 0 && command != 'M' && command != 'm') {
                break;
            }

            bool relative = command >= 'a';
            u8 upper = relative ? (u8)(command - 32) : command;
            Point base = relative ? current : Point{0.f, 0.f};

            if (upper == 'Z') {
                closePath(path);
                current = start;
                needMove = true;
                previous = upper;
                skipSeparator(p, end);
                continue;
            }

            if (needMove && upper != 'M') {
                moveTo(path, current);
            }

            needMove = false;

            float v[7];
            u32 count;

            switch (upper) {
                case 'M':
                case 'L':
                case 'T':
                    count = 2;
                    break;
                case 'H':
                case 'V':
                    count = 1;
                    break;
                case 'C':
                    count = 6;
                    break;
                case 'S':
                case 'Q':
                    count = 4;
                    break;
                case 'A':
                    count = 7;
                    break;
                default:
                    return;
            }

            bool ok = true;
            bool largeArc = false;
            bool sweepArc = false;

            for (u32 i = 0; i < count && ok; i++) {
                while (p < end && isSpace(*p)) {
                    p++;
                }

                if (upper == 'A' && i == 3) {
                    ok = readFlag(p, end, largeArc);
                } else if (upper == 'A' && i == 4) {
                    ok = readFlag(p, end, sweepArc);
                } else {
                    ok = readNumber(p, end, v[i]);

                    if (ok) {
                        skipSeparator(p, end);
                    }
                }
            }

            if (!ok) {
                return;
            }

            switch (upper) {
                case 'M':
                    current = Point{base.x + v[0], base.y + v[1]};
                    start = current;
                    moveTo(path, current);
                    break;
                case 'L':
                    current = Point{base.x + v[0], base.y + v[1]};
                    lineTo(path, current);
                    break;
                case 'H':
                    current = Point{(relative ? current.x : 0.f) + v[0], current.y};
                    lineTo(path, current);
                    break;
                case 'V':
                    current = Point{current.x, (relative ? current.y : 0.f) + v[0]};
                    lineTo(path, current);
                    break;
                case 'C': {
                    Point c1{base.x + v[0], base.y + v[1]};
                    Point c2{base.x + v[2], base.y + v[3]};
                    Point to{base.x + v[4], base.y + v[5]};

                    cubicTo(path, c1, c2, to);
                    control = c2;
                    current = to;
                    break;
                }
                case 'S': {
                    Point c1 = previous == 'C' || previous == 'S' ? Point{2.f * current.x - control.x, 2.f * current.y - control.y} : current;
                    Point c2{base.x + v[0], base.y + v[1]};
                    Point to{base.x + v[2], base.y + v[3]};

                    cubicTo(path, c1, c2, to);
                    control = c2;
                    current = to;
                    break;
                }
                case 'Q':
                case 'T': {
                    Point q = upper == 'Q' ? Point{base.x + v[0], base.y + v[1]} : (previous == 'Q' || previous == 'T' ? Point{2.f * current.x - control.x, 2.f * current.y - control.y} : current);
                    Point to = upper == 'Q' ? Point{base.x + v[2], base.y + v[3]} : Point{base.x + v[0], base.y + v[1]};
                    Point c1{current.x + 2.f / 3.f * (q.x - current.x), current.y + 2.f / 3.f * (q.y - current.y)};
                    Point c2{to.x + 2.f / 3.f * (q.x - to.x), to.y + 2.f / 3.f * (q.y - to.y)};

                    cubicTo(path, c1, c2, to);
                    control = q;
                    current = to;
                    break;
                }
                case 'A': {
                    Point to{base.x + v[5], base.y + v[6]};

                    arcTo(path, current, v[0], v[1], v[2], largeArc, sweepArc, to);
                    current = to;
                    break;
                }
            }

            previous = upper;
        }
    }

    static void parsePoints(StringView text, Vector<Point>& out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        while (true) {
            Point point;

            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end || !readNumber(p, end, point.x)) {
                return;
            }

            skipSeparator(p, end);

            if (!readNumber(p, end, point.y)) {
                return;
            }

            skipSeparator(p, end);
            out.pushBack(point);
        }
    }

    static void ellipsePath(Path& path, float cx, float cy, float rx, float ry) {
        const float k = 0.5522847498f;

        moveTo(path, Point{cx + rx, cy});
        cubicTo(path, Point{cx + rx, cy + ry * k}, Point{cx + rx * k, cy + ry}, Point{cx, cy + ry});
        cubicTo(path, Point{cx - rx * k, cy + ry}, Point{cx - rx, cy + ry * k}, Point{cx - rx, cy});
        cubicTo(path, Point{cx - rx, cy - ry * k}, Point{cx - rx * k, cy - ry}, Point{cx, cy - ry});
        cubicTo(path, Point{cx + rx * k, cy - ry}, Point{cx + rx, cy - ry * k}, Point{cx + rx, cy});
        closePath(path);
    }

    static bool buildShape(const Document& doc, const Node* node, const ComputedStyle& style, Viewport vp, Path& path) {
        float fs = style.fontSize;

        switch (node->name) {
            case AtomPath: {
                u32 d = attribute(node, AtomD);

                if (d) {
                    parsePathData(textOf(doc, d), path);
                }

                break;
            }
            case AtomRect: {
                float x = lengthOr(doc, node, AtomX, Axis::X, vp, fs, 0.f);
                float y = lengthOr(doc, node, AtomY, Axis::Y, vp, fs, 0.f);
                float w = lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, 0.f);
                float h = lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, 0.f);
                float rx;
                float ry;
                bool hasRx = lengthAttribute(doc, node, AtomRx, Axis::X, vp, fs, rx) && rx >= 0.f;
                bool hasRy = lengthAttribute(doc, node, AtomRy, Axis::Y, vp, fs, ry) && ry >= 0.f;

                if (w <= 0.f || h <= 0.f) {
                    return false;
                }

                if (!hasRx) {
                    rx = hasRy ? ry : 0.f;
                }

                if (!hasRy) {
                    ry = hasRx ? rx : 0.f;
                }

                rx = minf(rx, w / 2.f);
                ry = minf(ry, h / 2.f);

                if (rx > 0.f && ry > 0.f) {
                    const float k = 0.5522847498f;

                    moveTo(path, Point{x + rx, y});
                    lineTo(path, Point{x + w - rx, y});
                    cubicTo(path, Point{x + w - rx + rx * k, y}, Point{x + w, y + ry - ry * k}, Point{x + w, y + ry});
                    lineTo(path, Point{x + w, y + h - ry});
                    cubicTo(path, Point{x + w, y + h - ry + ry * k}, Point{x + w - rx + rx * k, y + h}, Point{x + w - rx, y + h});
                    lineTo(path, Point{x + rx, y + h});
                    cubicTo(path, Point{x + rx - rx * k, y + h}, Point{x, y + h - ry + ry * k}, Point{x, y + h - ry});
                    lineTo(path, Point{x, y + ry});
                    cubicTo(path, Point{x, y + ry - ry * k}, Point{x + rx - rx * k, y}, Point{x + rx, y});
                    closePath(path);
                } else {
                    moveTo(path, Point{x, y});
                    lineTo(path, Point{x + w, y});
                    lineTo(path, Point{x + w, y + h});
                    lineTo(path, Point{x, y + h});
                    closePath(path);
                }

                break;
            }
            case AtomCircle: {
                float r = lengthOr(doc, node, AtomR, Axis::Other, vp, fs, 0.f);

                if (r <= 0.f) {
                    return false;
                }

                ellipsePath(path, lengthOr(doc, node, AtomCx, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomCy, Axis::Y, vp, fs, 0.f), r, r);
                break;
            }
            case AtomEllipse: {
                float rx;
                float ry;
                bool hasRx = lengthAttribute(doc, node, AtomRx, Axis::X, vp, fs, rx) && rx >= 0.f;
                bool hasRy = lengthAttribute(doc, node, AtomRy, Axis::Y, vp, fs, ry) && ry >= 0.f;

                if (!hasRx) {
                    rx = hasRy ? ry : 0.f;
                }

                if (!hasRy) {
                    ry = hasRx ? rx : 0.f;
                }

                if (rx <= 0.f || ry <= 0.f) {
                    return false;
                }

                ellipsePath(path, lengthOr(doc, node, AtomCx, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomCy, Axis::Y, vp, fs, 0.f), rx, ry);
                break;
            }
            case AtomLine:
                moveTo(path, Point{lengthOr(doc, node, AtomX1, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomY1, Axis::Y, vp, fs, 0.f)});
                lineTo(path, Point{lengthOr(doc, node, AtomX2, Axis::X, vp, fs, 0.f), lengthOr(doc, node, AtomY2, Axis::Y, vp, fs, 0.f)});
                break;
            case AtomPolyline:
            case AtomPolygon: {
                u32 value = attribute(node, AtomPoints);
                Vector<Point> points;

                if (value) {
                    parsePoints(textOf(doc, value), points);
                }

                if (points.length() == 0) {
                    return false;
                }

                moveTo(path, points[0]);

                for (size_t i = 1; i < points.length(); i++) {
                    lineTo(path, points[i]);
                }

                if (node->name == AtomPolygon) {
                    closePath(path);
                }

                break;
            }
            default:
                return false;
        }

        return path.verbs.length() > 0;
    }

    static void flatten(const Path& path, const Matrix& m, float tol, Flat& out) {
        const Point* pts = path.points.data();
        size_t pi = 0;
        bool open = false;
        Point start{0.f, 0.f};
        Point last{0.f, 0.f};

        auto finish = [&](bool closed) {
            if (open) {
                out.ends.pushBack((u32)out.points.length());
                out.closed.pushBack(closed ? 1 : 0);
                open = false;
            }
        };

        for (u8 verb : path.verbs) {
            switch (verb) {
                case VerbMove:
                    finish(false);
                    start = apply(m, pts[pi++]);
                    last = start;
                    out.points.pushBack(start);
                    open = true;
                    break;
                case VerbLine:
                    last = apply(m, pts[pi++]);
                    out.points.pushBack(last);
                    break;
                case VerbCubic: {
                    Point p0 = last;
                    Point p1 = apply(m, pts[pi]);
                    Point p2 = apply(m, pts[pi + 1]);
                    Point p3 = apply(m, pts[pi + 2]);
                    float ddx = maxf(fabsf(p0.x - 2.f * p1.x + p2.x), fabsf(p1.x - 2.f * p2.x + p3.x));
                    float ddy = maxf(fabsf(p0.y - 2.f * p1.y + p2.y), fabsf(p1.y - 2.f * p2.y + p3.y));
                    float dd = sqrtf(ddx * ddx + ddy * ddy);
                    int n = (int)ceilf(sqrtf(0.75f * dd / tol));

                    if (n < 1 || !isfinite(dd)) {
                        n = 1;
                    }

                    if (n > 512) {
                        n = 512;
                    }

                    for (int i = 1; i <= n; i++) {
                        float t = (float)i / n;
                        float u = 1.f - t;
                        float a = u * u * u;
                        float b = 3.f * u * u * t;
                        float c = 3.f * u * t * t;
                        float d = t * t * t;

                        out.points.pushBack(Point{a * p0.x + b * p1.x + c * p2.x + d * p3.x, a * p0.y + b * p1.y + c * p2.y + d * p3.y});
                    }

                    last = p3;
                    pi += 3;
                    break;
                }
                case VerbClose:
                    if (open) {
                        size_t count = out.points.length() - (out.ends.length() ? out.ends.back() : 0);

                        if (count == 1) {
                            out.points.pushBack(start);
                        }
                    }

                    finish(true);
                    last = start;
                    break;
            }
        }

        finish(false);
    }

    static Box flatBox(const Flat& flat) {
        Box box = emptyBox();

        for (Point p : flat.points) {
            extend(box, p);
        }

        return box;
    }

    static float signedArea(const Point* points, size_t count) {
        float area = 0.f;

        for (size_t i = 0; i < count; i++) {
            const Point& a = points[i];
            const Point& b = points[(i + 1) % count];

            area += a.x * b.y - b.x * a.y;
        }

        return area * 0.5f;
    }

    static void addPolygon(Flat& out, const Point* points, size_t count) {
        if (count < 3) {
            return;
        }

        bool reverse = signedArea(points, count) < 0.f;

        for (size_t i = 0; i < count; i++) {
            out.points.pushBack(points[reverse ? count - 1 - i : i]);
        }

        out.ends.pushBack((u32)out.points.length());
        out.closed.pushBack(1);
    }

    static int arcSegments(float angle, float radius, float tol) {
        if (radius <= tol) {
            return 4;
        }

        float step = 2.f * acosf(1.f - tol / radius);
        int n = (int)ceilf(fabsf(angle) / maxf(step, 0.01f));

        return n < 2 ? 2 : (n > 256 ? 256 : n);
    }

    static void addFan(Flat& out, Point center, float radius, float from, float sweep, float tol) {
        int n = arcSegments(sweep, radius, tol);
        Vector<Point> fan;

        fan.pushBack(center);

        for (int i = 0; i <= n; i++) {
            float a = from + sweep * i / n;

            fan.pushBack(Point{center.x + radius * cosf(a), center.y + radius * sinf(a)});
        }

        addPolygon(out, fan.data(), fan.length());
    }

    struct StrokeParams {
        float half;
        u8 cap;
        u8 join;
        float miterLimit;
        float tol;
    };

    static void addJoin(Flat& out, const StrokeParams& s, Point p, Point d0, Point d1) {
        float cross = d0.x * d1.y - d0.y * d1.x;
        float dot = d0.x * d1.x + d0.y * d1.y;

        if (fabsf(cross) < 1e-6f && dot > 0.f) {
            return;
        }

        float side = cross > 0.f ? -1.f : 1.f;
        Point n0{-d0.y * side, d0.x * side};
        Point n1{-d1.y * side, d1.x * side};
        Point a{p.x + n0.x * s.half, p.y + n0.y * s.half};
        Point b{p.x + n1.x * s.half, p.y + n1.y * s.half};

        if (s.join == JoinRound) {
            float from = atan2f(n0.y, n0.x);
            float to = atan2f(n1.y, n1.x);
            float sweep = to - from;

            while (sweep > pi) {
                sweep -= 2.f * pi;
            }

            while (sweep < -pi) {
                sweep += 2.f * pi;
            }

            addFan(out, p, s.half, from, sweep, s.tol);

            return;
        }

        if (s.join == JoinMiter) {
            float cosHalf = sqrtf(maxf(0.f, (1.f + dot) / 2.f));

            if (cosHalf > 1e-6f && 1.f / cosHalf <= s.miterLimit) {
                Point m{n0.x + n1.x, n0.y + n1.y};
                float length = sqrtf(m.x * m.x + m.y * m.y);

                if (length > 1e-6f) {
                    float reach = s.half / cosHalf;
                    Point tip{p.x + m.x / length * reach, p.y + m.y / length * reach};
                    Point quad[4] = {p, a, tip, b};

                    addPolygon(out, quad, 4);

                    return;
                }
            }
        }

        Point triangle[3] = {p, a, b};

        addPolygon(out, triangle, 3);
    }

    static void addCap(Flat& out, const StrokeParams& s, Point p, Point d) {
        Point n{-d.y, d.x};

        if (s.cap == CapSquare) {
            Point quad[4] = {
                Point{p.x + n.x * s.half, p.y + n.y * s.half},
                Point{p.x + n.x * s.half + d.x * s.half, p.y + n.y * s.half + d.y * s.half},
                Point{p.x - n.x * s.half + d.x * s.half, p.y - n.y * s.half + d.y * s.half},
                Point{p.x - n.x * s.half, p.y - n.y * s.half},
            };

            addPolygon(out, quad, 4);
        } else if (s.cap == CapRound) {
            addFan(out, p, s.half, atan2f(n.y, n.x), -pi, s.tol);
        }
    }

    static void strokePolyline(Flat& out, const StrokeParams& s, const Point* points, size_t count, bool closed) {
        Vector<Point> pts;

        for (size_t i = 0; i < count; i++) {
            if (pts.length() == 0 || pts.back().x != points[i].x || pts.back().y != points[i].y) {
                pts.pushBack(points[i]);
            }
        }

        if (closed && pts.length() > 1 && pts.back().x == pts[0].x && pts.back().y == pts[0].y) {
            pts.popBack();
        }

        if (pts.length() == 1) {
            if (count >= 2 && s.cap == CapRound) {
                addFan(out, pts[0], s.half, 0.f, 2.f * pi, s.tol);
            } else if (count >= 2 && s.cap == CapSquare) {
                Point p = pts[0];
                Point quad[4] = {Point{p.x - s.half, p.y - s.half}, Point{p.x + s.half, p.y - s.half}, Point{p.x + s.half, p.y + s.half}, Point{p.x - s.half, p.y + s.half}};

                addPolygon(out, quad, 4);
            }

            return;
        }

        size_t n = pts.length();
        size_t segments = closed ? n : n - 1;
        Vector<Point> dirs;

        for (size_t i = 0; i < segments; i++) {
            Point a = pts[i];
            Point b = pts[(i + 1) % n];
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float length = sqrtf(dx * dx + dy * dy);

            dirs.pushBack(Point{dx / length, dy / length});

            Point normal{-dy / length * s.half, dx / length * s.half};
            Point quad[4] = {
                Point{a.x + normal.x, a.y + normal.y},
                Point{b.x + normal.x, b.y + normal.y},
                Point{b.x - normal.x, b.y - normal.y},
                Point{a.x - normal.x, a.y - normal.y},
            };

            addPolygon(out, quad, 4);
        }

        for (size_t i = 1; i < segments; i++) {
            addJoin(out, s, pts[i], dirs[i - 1], dirs[i]);
        }

        if (closed) {
            addJoin(out, s, pts[0], dirs[segments - 1], dirs[0]);
        } else {
            addCap(out, s, pts[0], Point{-dirs[0].x, -dirs[0].y});
            addCap(out, s, pts[n - 1], dirs[segments - 1]);
        }
    }

    static void dashPolyline(Flat& out, const StrokeParams& s, const Point* points, size_t count, bool closed, const float* dashes, u32 dashCount, float offset) {
        Vector<Point> loop;

        loop.append(points, count);

        if (closed && count > 1) {
            loop.pushBack(points[0]);
        }

        float total = 0.f;

        for (u32 i = 0; i < dashCount; i++) {
            total += dashes[i];
        }

        float phase = fmodf(offset, total);

        if (phase < 0.f) {
            phase += total;
        }

        u32 index = 0;

        while (phase >= dashes[index]) {
            phase -= dashes[index];
            index = (index + 1) % dashCount;
        }

        float remaining = dashes[index] - phase;
        bool on = index % 2 == 0;
        Vector<Point> piece;

        if (on) {
            piece.pushBack(loop[0]);
        }

        for (size_t i = 0; i + 1 < loop.length(); i++) {
            Point a = loop[i];
            Point b = loop[i + 1];
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float length = sqrtf(dx * dx + dy * dy);
            float travelled = 0.f;

            while (length - travelled > remaining) {
                travelled += remaining;

                Point cut{a.x + dx * travelled / length, a.y + dy * travelled / length};

                if (on) {
                    piece.pushBack(cut);
                    strokePolyline(out, s, piece.data(), piece.length(), false);
                    piece.clear();
                } else {
                    piece.pushBack(cut);
                }

                on = !on;
                index = (index + 1) % dashCount;
                remaining = dashes[index];
            }

            remaining -= length - travelled;

            if (on) {
                piece.pushBack(b);
            }
        }

        if (on && piece.length() > 1) {
            strokePolyline(out, s, piece.data(), piece.length(), false);
        }
    }

    static void strokePath(const Path& path, const ComputedStyle& style, float scale, Flat& out) {
        Flat flat;
        StrokeParams s{style.strokeWidth / 2.f, style.cap, style.join, style.miterLimit, tolerance / maxf(scale, 1e-6f)};

        flatten(path, identity(), s.tol, flat);

        u32 begin = 0;

        for (size_t i = 0; i < flat.ends.length(); i++) {
            u32 end = flat.ends[i];
            const Point* points = flat.points.data() + begin;
            size_t count = end - begin;

            if (count >= 2) {
                if (style.dashCount) {
                    dashPolyline(out, s, points, count, flat.closed[i], style.dashes, style.dashCount, style.dashOffset);
                } else {
                    strokePolyline(out, s, points, count, flat.closed[i]);
                }
            }

            begin = end;
        }
    }

    static bool isKeyword(StringView text, const char* keyword) {
        return equalsIgnoreCase(trim(text), StringView(keyword));
    }

    static bool parseOpacity(StringView text, float& out) {
        StringView body = trim(text);
        const u8* p = body.begin();
        float value;

        if (!readNumber(p, body.end(), value)) {
            return false;
        }

        if (p < body.end() && *p == '%') {
            value /= 100.f;
            p++;
        }

        if (p != body.end()) {
            return false;
        }

        out = clampUnit(value);

        return true;
    }

    static ComputedStyle initialStyle() {
        ComputedStyle style;

        memset(&style, 0, sizeof(style));
        style.fill.kind = PaintKind::Color;
        style.fill.color = Color{0.f, 0.f, 0.f, 1.f};
        style.stroke.kind = PaintKind::None;
        style.color = Color{0.f, 0.f, 0.f, 1.f};
        style.stopColor = Color{0.f, 0.f, 0.f, 1.f};
        style.fillOpacity = 1.f;
        style.strokeOpacity = 1.f;
        style.strokeWidth = 1.f;
        style.miterLimit = 4.f;
        style.opacity = 1.f;
        style.stopOpacity = 1.f;
        style.fontSize = 16.f;
        style.fillRule = WindingNonZero;
        style.clipRule = WindingNonZero;
        style.cap = CapButt;
        style.join = JoinMiter;
        style.visible = true;

        return style;
    }

    static StringView cascaded(const Document& doc, const Node* node, u32 property) {
        u32 value = node->style ? node->style[property - firstProperty] : 0;

        return value ? textOf(doc, value) : StringView();
    }

    static ComputedStyle computeStyle(Lowering& l, const Node* node, const ComputedStyle& parent, Viewport vp) {
        const Document& doc = *l.doc;
        ComputedStyle initial = initialStyle();
        ComputedStyle style = parent;

        style.opacity = 1.f;
        style.displayNone = false;
        style.stopColor = initial.stopColor;
        style.stopOpacity = 1.f;
        style.clipPath = 0;

        for (u32 property = firstProperty; property < firstProperty + propertyCount; property++) {
            StringView value = cascaded(doc, node, property);

            if (value.empty()) {
                continue;
            }

            bool inherit = isKeyword(value, "inherit");
            bool reset = isKeyword(value, "initial");

            if (isKeyword(value, "unset")) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty;

                inherit = inherited;
                reset = !inherited;
            }

            const ComputedStyle& source = inherit ? parent : initial;

            switch (property) {
                case AtomFontSize:
                    if (inherit || reset) {
                        style.fontSize = source.fontSize;
                    } else {
                        Length length;

                        if (parseLength(value, length) && length.value >= 0.f) {
                            style.fontSize = length.unit == Unit::Percent ? parent.fontSize * length.value / 100.f : resolveLength(length, Axis::Other, vp, parent.fontSize);
                        }
                    }

                    break;
                default:
                    break;
            }
        }

        for (u32 property = firstProperty; property < firstProperty + propertyCount; property++) {
            StringView value = cascaded(doc, node, property);

            if (value.empty() || property == AtomFontSize) {
                continue;
            }

            bool inherit = isKeyword(value, "inherit");
            bool reset = isKeyword(value, "initial");

            if (isKeyword(value, "unset")) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty;

                inherit = inherited;
                reset = !inherited;
            }

            const ComputedStyle& source = inherit ? parent : initial;
            bool keyword = inherit || reset;

            switch (property) {
                case AtomFill:
                case AtomStroke: {
                    PaintValue& target = property == AtomFill ? style.fill : style.stroke;

                    if (keyword) {
                        target = property == AtomFill ? source.fill : source.stroke;
                    } else {
                        PaintValue paint;

                        if (parsePaint(doc, value, paint)) {
                            target = paint;
                        }
                    }

                    break;
                }
                case AtomColor:
                    if (keyword) {
                        style.color = source.color;
                    } else if (!isKeyword(value, "currentcolor")) {
                        Color color;

                        if (parseColor(value, color)) {
                            style.color = color;
                        }
                    }

                    break;
                case AtomFillOpacity:
                    if (keyword) {
                        style.fillOpacity = source.fillOpacity;
                    } else {
                        parseOpacity(value, style.fillOpacity);
                    }

                    break;
                case AtomStrokeOpacity:
                    if (keyword) {
                        style.strokeOpacity = source.strokeOpacity;
                    } else {
                        parseOpacity(value, style.strokeOpacity);
                    }

                    break;
                case AtomOpacity:
                    if (keyword) {
                        style.opacity = source.opacity;
                    } else {
                        parseOpacity(value, style.opacity);
                    }

                    break;
                case AtomStopOpacity:
                    if (keyword) {
                        style.stopOpacity = source.stopOpacity;
                    } else {
                        parseOpacity(value, style.stopOpacity);
                    }

                    break;
                case AtomFillRule:
                case AtomClipRule: {
                    u8& target = property == AtomFillRule ? style.fillRule : style.clipRule;

                    if (keyword) {
                        target = property == AtomFillRule ? source.fillRule : source.clipRule;
                    } else if (isKeyword(value, "nonzero")) {
                        target = WindingNonZero;
                    } else if (isKeyword(value, "evenodd")) {
                        target = WindingEvenOdd;
                    }

                    break;
                }
                case AtomStrokeWidth:
                    if (keyword) {
                        style.strokeWidth = source.strokeWidth;
                    } else {
                        Length length;

                        if (parseLength(value, length) && length.value >= 0.f) {
                            style.strokeWidth = resolveLength(length, Axis::Other, vp, style.fontSize);
                        }
                    }

                    break;
                case AtomStrokeLinecap:
                    if (keyword) {
                        style.cap = source.cap;
                    } else if (isKeyword(value, "butt")) {
                        style.cap = CapButt;
                    } else if (isKeyword(value, "round")) {
                        style.cap = CapRound;
                    } else if (isKeyword(value, "square")) {
                        style.cap = CapSquare;
                    }

                    break;
                case AtomStrokeLinejoin:
                    if (keyword) {
                        style.join = source.join;
                    } else if (isKeyword(value, "miter") || isKeyword(value, "miter-clip") || isKeyword(value, "arcs")) {
                        style.join = JoinMiter;
                    } else if (isKeyword(value, "round")) {
                        style.join = JoinRound;
                    } else if (isKeyword(value, "bevel")) {
                        style.join = JoinBevel;
                    }

                    break;
                case AtomStrokeMiterlimit:
                    if (keyword) {
                        style.miterLimit = source.miterLimit;
                    } else {
                        float limit;

                        if (parseNumberText(value, limit) && limit >= 1.f) {
                            style.miterLimit = limit;
                        }
                    }

                    break;
                case AtomStrokeDasharray:
                    if (keyword) {
                        style.dashes = source.dashes;
                        style.dashCount = source.dashCount;
                    } else if (isKeyword(value, "none")) {
                        style.dashes = nullptr;
                        style.dashCount = 0;
                    } else {
                        Vector<float> dashes;
                        const u8* p = value.begin();
                        const u8* end = value.end();
                        bool valid = true;
                        float sum = 0.f;

                        while (true) {
                            Length length;

                            while (p < end && isSpace(*p)) {
                                p++;
                            }

                            if (p >= end) {
                                break;
                            }

                            if (!readLength(p, end, length)) {
                                valid = false;
                                break;
                            }

                            float dash = resolveLength(length, Axis::Other, vp, style.fontSize);

                            if (dash < 0.f) {
                                valid = false;
                                break;
                            }

                            sum += dash;
                            dashes.pushBack(dash);
                            skipSeparator(p, end);
                        }

                        if (!valid) {
                            break;
                        }

                        if (dashes.length() == 0 || sum <= 0.f) {
                            style.dashes = nullptr;
                            style.dashCount = 0;
                            break;
                        }

                        size_t count = dashes.length() % 2 ? dashes.length() * 2 : dashes.length();

                        style.dashes = (float*)l.pool->allocate(sizeof(float) * count);

                        for (size_t i = 0; i < count; i++) {
                            style.dashes[i] = dashes[i % dashes.length()];
                        }

                        style.dashCount = (u32)count;
                    }

                    break;
                case AtomStrokeDashoffset:
                    if (keyword) {
                        style.dashOffset = source.dashOffset;
                    } else {
                        Length length;

                        if (parseLength(value, length)) {
                            style.dashOffset = resolveLength(length, Axis::Other, vp, style.fontSize);
                        }
                    }

                    break;
                case AtomDisplay:
                    style.displayNone = !keyword && isKeyword(value, "none");
                    break;
                case AtomVisibility:
                    if (keyword) {
                        style.visible = source.visible;
                    } else if (isKeyword(value, "visible")) {
                        style.visible = true;
                    } else if (isKeyword(value, "hidden") || isKeyword(value, "collapse")) {
                        style.visible = false;
                    }

                    break;
                case AtomStopColor:
                    if (keyword) {
                        style.stopColor = source.stopColor;
                    } else if (isKeyword(value, "currentcolor")) {
                        style.stopColor = style.color;
                    } else {
                        Color color;

                        if (parseColor(value, color)) {
                            style.stopColor = color;
                        }
                    }

                    break;
                case AtomClipPathProperty: {
                    if (keyword) {
                        style.clipPath = source.clipPath;
                        break;
                    }

                    PaintValue reference;

                    if (parsePaint(doc, value, reference) && reference.kind == PaintKind::Server) {
                        style.clipPath = reference.server;
                    }

                    break;
                }
                default:
                    break;
            }
        }

        return style;
    }

    static const ComputedStyle& documentStyle(Lowering& l, Node* node, Viewport vp) {
        if (ComputedStyle* known = l.documentStyles[node->index]) {
            return *known;
        }

        ComputedStyle parent = node->parent ? documentStyle(l, node->parent, vp) : initialStyle();
        ComputedStyle* style = l.pool->make<ComputedStyle>(computeStyle(l, node, parent, vp));

        l.documentStyles.mut(node->index) = style;

        return *style;
    }

    static void emitEdges(Lowering& l, const Flat& flat, Box& bounds) {
        u32 begin = 0;

        for (u32 end : flat.ends) {
            if (end - begin >= 2) {
                for (u32 i = begin; i < end; i++) {
                    Point a = flat.points[i];
                    Point b = flat.points[i + 1 < end ? i + 1 : begin];

                    extend(bounds, a);

                    if (a.y != b.y) {
                        l.edges.pushBack(Edge{a.x, a.y, b.x, b.y});
                    }
                }
            }

            begin = end;
        }
    }

    static void emitFill(Lowering& l, const Flat& flat, u8 rule, DevicePaint* paint) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Fill;
        op.rule = rule;
        op.paint = paint;
        op.first = (u32)l.edges.length();
        op.bounds = emptyBox();
        emitEdges(l, flat, op.bounds);
        op.count = (u32)l.edges.length() - op.first;

        if (op.count && boxValid(op.bounds)) {
            l.ops.pushBack(op);
        }
    }

    static DevicePaint* solidPaint(Lowering& l, Color color, float opacity) {
        float alpha = color.a * opacity;

        if (alpha <= 0.f) {
            return nullptr;
        }

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Solid;
        paint->color = Color{color.r * alpha, color.g * alpha, color.b * alpha, alpha};

        return paint;
    }

    static u32 gradientAttribute(const Document& doc, Node* gradient, u32 name) {
        Node* node = gradient;

        for (u32 hops = 0; node && hops < 16; hops++) {
            if (u32 value = attribute(node, name)) {
                return value;
            }

            Node* next = elementById(doc, attribute(node, AtomHref));

            if (!next || (next->name != AtomLinearGradient && next->name != AtomRadialGradient) || next == node) {
                break;
            }

            node = next;
        }

        return 0;
    }

    static Node* gradientStops(const Document& doc, Node* gradient) {
        Node* node = gradient;

        for (u32 hops = 0; node && hops < 16; hops++) {
            for (Node* child = node->first; child; child = child->next) {
                if (child->name == AtomStop) {
                    return node;
                }
            }

            Node* next = elementById(doc, attribute(node, AtomHref));

            if (!next || (next->name != AtomLinearGradient && next->name != AtomRadialGradient) || next == node) {
                break;
            }

            node = next;
        }

        return nullptr;
    }

    static float gradientLength(Lowering& l, Node* gradient, u32 name, bool boxUnits, Axis axis, Viewport vp, float fallback) {
        u32 value = gradientAttribute(*l.doc, gradient, name);
        Length length;

        if (!value || !parseLength(textOf(*l.doc, value), length)) {
            return fallback;
        }

        if (boxUnits) {
            return length.unit == Unit::Percent ? length.value / 100.f : length.value;
        }

        return resolveLength(length, axis, vp, 16.f);
    }

    static DevicePaint* gradientPaint(Lowering& l, Node* gradient, float opacity, Box bbox, const Matrix& ctm, Viewport vp) {
        const Document& doc = *l.doc;
        u32 units = gradientAttribute(doc, gradient, AtomGradientUnits);
        bool boxUnits = !units || textOf(doc, units) != StringView(u8"userSpaceOnUse");
        Node* stopsNode = gradientStops(doc, gradient);
        Vector<GradientStop> stops;

        if (stopsNode) {
            float previous = 0.f;

            for (Node* child = stopsNode->first; child; child = child->next) {
                if (child->name != AtomStop) {
                    continue;
                }

                float offset = 0.f;
                u32 value = attribute(child, AtomOffset);

                if (value) {
                    StringView text = trim(textOf(doc, value));
                    const u8* p = text.begin();

                    if (readNumber(p, text.end(), offset)) {
                        if (p < text.end() && *p == '%') {
                            offset /= 100.f;
                        }
                    } else {
                        offset = 0.f;
                    }
                }

                offset = maxf(clampUnit(offset), previous);
                previous = offset;

                const ComputedStyle& style = documentStyle(l, child, vp);
                Color color = style.stopColor;

                color.a *= style.stopOpacity * opacity;
                stops.pushBack(GradientStop{offset, color});
            }
        }

        if (stops.length() == 0) {
            return nullptr;
        }

        if (stops.length() == 1) {
            Color color = stops[0].color;

            return solidPaint(l, Color{color.r, color.g, color.b, 1.f}, color.a);
        }

        Matrix space = ctm;
        u32 transform = gradientAttribute(doc, gradient, AtomGradientTransform);

        if (transform) {
            Matrix m;

            if (parseTransform(textOf(doc, transform), m)) {
                space = multiply(space, m);
            }
        }

        if (boxUnits) {
            float bw = bbox.x1 - bbox.x0;
            float bh = bbox.y1 - bbox.y0;

            if (!boxValid(bbox) || bw <= 0.f || bh <= 0.f) {
                return nullptr;
            }

            space = multiply(space, Matrix{bw, 0.f, 0.f, bh, bbox.x0, bbox.y0});
        }

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));

        if (!invert(space, paint->inverse)) {
            return nullptr;
        }

        paint->stopCount = (u32)stops.length();
        paint->stops = (GradientStop*)l.pool->allocate(sizeof(GradientStop) * stops.length());
        memcpy(paint->stops, stops.data(), sizeof(GradientStop) * stops.length());

        u32 spread = gradientAttribute(doc, gradient, AtomSpreadMethod);

        paint->spread = SpreadPad;

        if (spread && textOf(doc, spread) == StringView(u8"reflect")) {
            paint->spread = SpreadReflect;
        } else if (spread && textOf(doc, spread) == StringView(u8"repeat")) {
            paint->spread = SpreadRepeat;
        }

        Color last = stops.back().color;

        if (gradient->name == AtomLinearGradient) {
            paint->type = PaintType::Linear;
            paint->x1 = gradientLength(l, gradient, AtomX1, boxUnits, Axis::X, vp, 0.f);
            paint->y1 = gradientLength(l, gradient, AtomY1, boxUnits, Axis::Y, vp, 0.f);
            paint->x2 = gradientLength(l, gradient, AtomX2, boxUnits, Axis::X, vp, boxUnits ? 1.f : vp.width);
            paint->y2 = gradientLength(l, gradient, AtomY2, boxUnits, Axis::Y, vp, 0.f);

            if (paint->x1 == paint->x2 && paint->y1 == paint->y2) {
                return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
            }
        } else {
            paint->type = PaintType::Radial;
            paint->cx = gradientLength(l, gradient, AtomCx, boxUnits, Axis::X, vp, boxUnits ? 0.5f : vp.width / 2.f);
            paint->cy = gradientLength(l, gradient, AtomCy, boxUnits, Axis::Y, vp, boxUnits ? 0.5f : vp.height / 2.f);
            paint->r = gradientLength(l, gradient, AtomR, boxUnits, Axis::Other, vp, boxUnits ? 0.5f : resolveLength(Length{50.f, Unit::Percent}, Axis::Other, vp, 16.f));
            paint->fx = gradientLength(l, gradient, AtomFx, boxUnits, Axis::X, vp, paint->cx);
            paint->fy = gradientLength(l, gradient, AtomFy, boxUnits, Axis::Y, vp, paint->cy);
            paint->fr = gradientLength(l, gradient, AtomFr, boxUnits, Axis::Other, vp, 0.f);

            if (paint->r <= 0.f) {
                return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
            }
        }

        return paint;
    }

    static DevicePaint* devicePaint(Lowering& l, const PaintValue& value, float opacity, const ComputedStyle& style, Box bbox, const Matrix& ctm, Viewport vp) {
        switch (value.kind) {
            case PaintKind::None:
                return nullptr;
            case PaintKind::Color:
                return solidPaint(l, value.color, opacity);
            case PaintKind::CurrentColor:
                return solidPaint(l, style.color, opacity);
            case PaintKind::Server: {
                Node* server = elementById(*l.doc, value.server);

                if (server && (server->name == AtomLinearGradient || server->name == AtomRadialGradient)) {
                    return gradientPaint(l, server, opacity, bbox, ctm, vp);
                }

                if (value.fallback == PaintKind::Color) {
                    return solidPaint(l, value.fallbackColor, opacity);
                }

                if (value.fallback == PaintKind::CurrentColor) {
                    return solidPaint(l, style.color, opacity);
                }

                return nullptr;
            }
        }

        return nullptr;
    }

    static bool renderable(u32 name) {
        switch (name) {
            case AtomSvg:
            case AtomG:
            case AtomA:
            case AtomSwitch:
            case AtomUse:
            case AtomPath:
            case AtomRect:
            case AtomCircle:
            case AtomEllipse:
            case AtomLine:
            case AtomPolyline:
            case AtomPolygon:
                return true;
            default:
                return false;
        }
    }

    static bool isShape(u32 name) {
        return name == AtomPath || name == AtomRect || name == AtomCircle || name == AtomEllipse || name == AtomLine || name == AtomPolyline || name == AtomPolygon;
    }

    static bool shapeBox(Lowering& l, Node* node, const ComputedStyle& style, Viewport vp, Box& out) {
        Path path;
        Flat flat;

        if (!buildShape(*l.doc, node, style, vp, path)) {
            return false;
        }

        flatten(path, identity(), 0.01f, flat);
        out = flatBox(flat);

        return boxValid(out);
    }

    static bool elementBox(Lowering& l, Node* node, const ComputedStyle& parent, Viewport vp, Box& out, u32 depth) {
        if (depth > 32 || !renderable(node->name)) {
            return false;
        }

        ComputedStyle style = computeStyle(l, node, parent, vp);

        if (style.displayNone) {
            return false;
        }

        if (isShape(node->name)) {
            return shapeBox(l, node, style, vp, out);
        }

        Box box = emptyBox();
        auto add = [&](Node* child, const Matrix& m) {
            Box inner;

            if (elementBox(l, child, style, vp, inner, depth + 1)) {
                Matrix full = multiply(m, transformOf(*l.doc, child, AtomTransform));
                Point corners[4] = {{inner.x0, inner.y0}, {inner.x1, inner.y0}, {inner.x1, inner.y1}, {inner.x0, inner.y1}};

                for (Point corner : corners) {
                    extend(box, apply(full, corner));
                }
            }
        };

        if (node->name == AtomUse) {
            Node* target = elementById(*l.doc, attribute(node, AtomHref));

            if (target && target != node) {
                add(target, translation(lengthOr(*l.doc, node, AtomX, Axis::X, vp, style.fontSize, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, style.fontSize, 0.f)));
            }
        } else {
            for (Node* child = node->first; child; child = child->next) {
                add(child, identity());
            }
        }

        out = box;

        return boxValid(box);
    }

    static void beginGroup(Lowering& l, float opacity, bool clipped, u32 clipFirst, u32 clipCount) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Begin;
        op.opacity = opacity;
        op.clipped = clipped;
        op.clipFirst = clipFirst;
        op.clipCount = clipCount;
        l.ops.pushBack(op);
    }

    static void endGroup(Lowering& l) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::End;
        l.ops.pushBack(op);
    }

    static void addClipShape(Lowering& l, const Path& path, const Matrix& m, u8 rule) {
        Flat flat;
        Box bounds = emptyBox();
        ClipShape shape;

        flatten(path, m, tolerance, flat);
        shape.first = (u32)l.edges.length();
        emitEdges(l, flat, bounds);
        shape.count = (u32)l.edges.length() - shape.first;
        shape.rule = rule;
        l.clips.pushBack(shape);
    }

    static void addRectClip(Lowering& l, const Matrix& m, float x, float y, float w, float h) {
        Path path;

        moveTo(path, Point{x, y});
        lineTo(path, Point{x + w, y});
        lineTo(path, Point{x + w, y + h});
        lineTo(path, Point{x, y + h});
        closePath(path);
        addClipShape(l, path, m, WindingNonZero);
    }

    static bool resolveClip(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, u32& first, u32& count) {
        const Document& doc = *l.doc;
        Node* clip = elementById(doc, style.clipPath);

        if (!clip || clip->name != AtomClipPath) {
            return false;
        }

        Matrix space = multiply(m, transformOf(doc, clip, AtomTransform));
        u32 units = attribute(clip, AtomClipPathUnits);

        first = (u32)l.clips.length();

        if (units && textOf(doc, units) == StringView(u8"objectBoundingBox")) {
            Box box;

            if (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0) {
                count = 0;

                return true;
            }

            space = multiply(space, Matrix{box.x1 - box.x0, 0.f, 0.f, box.y1 - box.y0, box.x0, box.y0});
        }

        for (Node* child = clip->first; child; child = child->next) {
            Node* shape = child;
            Matrix childSpace = multiply(space, transformOf(doc, child, AtomTransform));

            if (child->name == AtomUse) {
                shape = elementById(doc, attribute(child, AtomHref));

                if (!shape || !isShape(shape->name)) {
                    continue;
                }

                const ComputedStyle& useStyle = documentStyle(l, child, vp);

                if (useStyle.displayNone) {
                    continue;
                }

                childSpace = multiply(childSpace, translation(lengthOr(doc, child, AtomX, Axis::X, vp, useStyle.fontSize, 0.f), lengthOr(doc, child, AtomY, Axis::Y, vp, useStyle.fontSize, 0.f)));
                childSpace = multiply(childSpace, transformOf(doc, shape, AtomTransform));
            } else if (!isShape(child->name)) {
                continue;
            }

            const ComputedStyle& shapeStyle = documentStyle(l, shape, vp);

            if (shapeStyle.displayNone || !shapeStyle.visible) {
                continue;
            }

            Path path;

            if (buildShape(doc, shape, shapeStyle, vp, path)) {
                addClipShape(l, path, childSpace, shapeStyle.clipRule);
            }
        }

        count = (u32)l.clips.length() - first;

        return true;
    }

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp);

    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp) {
        for (Node* child = node->first; child; child = child->next) {
            lowerElement(l, child, style, ctm, vp);
        }
    }

    static void lowerShape(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        if (!style.visible) {
            return;
        }

        Path path;

        if (!buildShape(*l.doc, node, style, vp, path)) {
            return;
        }

        Flat userFlat;

        flatten(path, identity(), 0.01f, userFlat);

        Box box = flatBox(userFlat);

        if (DevicePaint* fill = devicePaint(l, style.fill, style.fillOpacity, style, box, m, vp)) {
            Flat flat;

            flatten(path, m, tolerance, flat);
            emitFill(l, flat, style.fillRule, fill);
        }

        if (style.strokeWidth > 0.f) {
            if (DevicePaint* stroke = devicePaint(l, style.stroke, style.strokeOpacity, style, box, m, vp)) {
                Flat outline;
                Flat device;

                strokePath(path, style, matrixScale(m), outline);
                device.ends.xchg(outline.ends);
                device.closed.xchg(outline.closed);

                for (Point p : outline.points) {
                    device.points.pushBack(apply(m, p));
                }

                emitFill(l, device, WindingNonZero, stroke);
            }
        }
    }

    static void lowerViewport(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Node* content, float x, float y, float w, float h, bool clip) {
        if (w <= 0.f || h <= 0.f) {
            return;
        }

        Matrix inner = multiply(m, translation(x, y));
        Viewport inside{w, h};
        Box viewBox;

        if (parseViewBox(*l.doc, node, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return;
            }

            inner = multiply(inner, viewBoxTransform(*l.doc, node, viewBox, w, h));
            inside = Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0};
        }

        if (clip) {
            u32 first = (u32)l.clips.length();

            addRectClip(l, m, x, y, w, h);
            beginGroup(l, 1.f, true, first, 1);
        }

        lowerChildren(l, content, style, inner, inside);

        if (clip) {
            endGroup(l);
        }
    }

    static bool intrinsicSize(const Document& doc, const Node* root, float& width, float& height) {
        Viewport none{0.f, 0.f};
        Length w;
        Length h;
        u32 widthValue = attribute(root, AtomWidth);
        u32 heightValue = attribute(root, AtomHeight);
        bool hasWidth = widthValue && parseLength(textOf(doc, widthValue), w) && w.unit != Unit::Percent && w.value > 0.f;
        bool hasHeight = heightValue && parseLength(textOf(doc, heightValue), h) && h.unit != Unit::Percent && h.value > 0.f;
        Box viewBox;
        bool hasViewBox = parseViewBox(doc, root, viewBox) && viewBox.x1 > viewBox.x0 && viewBox.y1 > viewBox.y0;
        float ratio = hasViewBox ? (viewBox.x1 - viewBox.x0) / (viewBox.y1 - viewBox.y0) : 0.f;

        width = hasWidth ? resolveLength(w, Axis::X, none, 16.f) : 0.f;
        height = hasHeight ? resolveLength(h, Axis::Y, none, 16.f) : 0.f;

        if (hasWidth && !hasHeight && ratio > 0.f) {
            height = width / ratio;
        } else if (hasHeight && !hasWidth && ratio > 0.f) {
            width = height * ratio;
        } else if (!hasWidth && !hasHeight && hasViewBox) {
            width = viewBox.x1 - viewBox.x0;
            height = viewBox.y1 - viewBox.y0;
        }

        return width > 0.f && height > 0.f;
    }

    static void lowerSvg(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        if (node == l.doc->root) {
            float w = (float)l.width;
            float h = (float)l.height;
            Box viewBox;

            if (!parseViewBox(*l.doc, node, viewBox)) {
                float iw;
                float ih;

                if (intrinsicSize(*l.doc, node, iw, ih)) {
                    Box synthesized{0.f, 0.f, iw, ih};

                    lowerChildren(l, node, style, multiply(m, viewBoxTransform(*l.doc, node, synthesized, w, h)), Viewport{iw, ih});

                    return;
                }
            }

            lowerViewport(l, node, style, m, node, 0.f, 0.f, w, h, false);

            return;
        }

        float fs = style.fontSize;

        lowerViewport(l, node, style, m, node, lengthOr(*l.doc, node, AtomX, Axis::X, vp, fs, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, fs, 0.f), lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, vp.width), lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, vp.height), true);
    }

    static void lowerUse(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Node* target = elementById(*l.doc, attribute(node, AtomHref));

        if (!target || l.active.length() > 32) {
            return;
        }

        for (Node* up = node; up; up = up->parent) {
            if (up == target) {
                return;
            }
        }

        for (Node* active : l.active) {
            if (active == target) {
                return;
            }
        }

        float fs = style.fontSize;
        Matrix placed = multiply(m, translation(lengthOr(*l.doc, node, AtomX, Axis::X, vp, fs, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, fs, 0.f)));

        l.active.pushBack(target);

        if (target->name == AtomSymbol) {
            ComputedStyle symbolStyle = computeStyle(l, target, style, vp);

            if (!symbolStyle.displayNone) {
                float w = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, vp.width);
                float h = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, vp.height);

                lowerViewport(l, target, symbolStyle, placed, target, 0.f, 0.f, w, h, true);
            }
        } else {
            lowerElement(l, target, style, placed, vp);
        }

        l.active.popBack();
    }

    static bool passesConditions(const Document& doc, const Node* node) {
        u32 extensions = attribute(node, AtomRequiredExtensions);

        if (extensions && !trim(textOf(doc, extensions)).empty()) {
            return false;
        }

        u32 languages = attribute(node, AtomSystemLanguage);

        if (!languages) {
            return true;
        }

        StringView text = textOf(doc, languages);
        const u8* p = text.begin();

        while (p < text.end()) {
            while (p < text.end() && (isSpace(*p) || *p == ',')) {
                p++;
            }

            const u8* start = p;

            while (p < text.end() && *p != ',' && !isSpace(*p)) {
                p++;
            }

            StringView language(start, p);

            if (equalsIgnoreCase(language, StringView(u8"en")) || startsWithIgnoreCase(language, StringView(u8"en-"))) {
                return true;
            }
        }

        return false;
    }

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp) {
        if (!renderable(node->name) || l.depth > 256) {
            return;
        }

        ComputedStyle style = computeStyle(l, node, parent, vp);

        if (style.displayNone || style.opacity <= 0.f) {
            return;
        }

        Matrix m = multiply(ctm, transformOf(*l.doc, node, AtomTransform));
        u32 clipFirst = 0;
        u32 clipCount = 0;
        bool clipped = style.clipPath && resolveClip(l, node, parent, style, m, vp, clipFirst, clipCount);
        bool layer = clipped || style.opacity < 1.f;

        if (layer) {
            beginGroup(l, style.opacity, clipped, clipFirst, clipCount);
        }

        l.depth++;

        switch (node->name) {
            case AtomG:
            case AtomA:
                lowerChildren(l, node, style, m, vp);
                break;
            case AtomSvg:
                lowerSvg(l, node, style, m, vp);
                break;
            case AtomUse:
                lowerUse(l, node, style, m, vp);
                break;
            case AtomSwitch:
                for (Node* child = node->first; child; child = child->next) {
                    if (passesConditions(*l.doc, child)) {
                        lowerElement(l, child, style, m, vp);
                        break;
                    }
                }

                break;
            default:
                lowerShape(l, node, style, m, vp);
                break;
        }

        l.depth--;

        if (layer) {
            endGroup(l);
        }
    }

    static void addLine(float* acc, u32 stride, u32 width, u32 height, float x0, float y0, float x1, float y1) {
        if (y0 == y1) {
            return;
        }

        float dir = 1.f;

        if (y0 > y1) {
            float tx = x0;
            float ty = y0;

            x0 = x1;
            y0 = y1;
            x1 = tx;
            y1 = ty;
            dir = -1.f;
        }

        if (y1 <= 0.f || y0 >= (float)height) {
            return;
        }

        float w = (float)width;
        float dxdy = (x1 - x0) / (y1 - y0);
        float x = x0;

        if (y0 < 0.f) {
            x -= y0 * dxdy;
        }

        u32 ystart = y0 < 0.f ? 0 : (u32)y0;
        float yceil = ceilf(y1);
        u32 yend = yceil > (float)height ? height : (u32)yceil;

        for (u32 y = ystart; y < yend; y++) {
            float* line = acc + (size_t)y * stride;
            float dy = minf((float)(y + 1), y1) - maxf((float)y, y0);
            float xnext = x + dxdy * dy;
            float d = dy * dir;
            float xa = minf(maxf(minf(x, xnext), 0.f), w);
            float xb = minf(maxf(maxf(x, xnext), 0.f), w);
            float x0floor = floorf(xa);
            int x0i = (int)x0floor;
            float x1ceil = ceilf(xb);
            int x1i = (int)x1ceil;

            if (x1i <= x0i + 1) {
                float xmf = 0.5f * (xa + xb) - x0floor;

                line[x0i] += d - d * xmf;
                line[x0i + 1] += d * xmf;
            } else {
                float s = 1.f / (xb - xa);
                float x0f = xa - x0floor;
                float a0 = 0.5f * s * (1.f - x0f) * (1.f - x0f);
                float x1f = xb - x1ceil + 1.f;
                float am = 0.5f * s * x1f * x1f;

                line[x0i] += d * a0;

                if (x1i == x0i + 2) {
                    line[x0i + 1] += d * (1.f - a0 - am);
                } else {
                    float a1 = s * (1.5f - x0f);

                    line[x0i + 1] += d * (a1 - a0);

                    for (int xi = x0i + 2; xi < x1i - 1; xi++) {
                        line[xi] += d * s;
                    }

                    float a2 = a1 + (float)(x1i - x0i - 3) * s;

                    line[x1i - 1] += d * (1.f - a2 - am);
                }

                line[x1i] += d * am;
            }

            x = xnext;
        }
    }

    static void addEdge(float* acc, u32 stride, u32 width, u32 height, Edge e) {
        float w = (float)width;
        float bounds[2] = {0.f, w};
        float ts[2];
        u32 cuts = 0;

        for (float bound : bounds) {
            if ((e.x0 < bound) != (e.x1 < bound) && e.x0 != e.x1) {
                float t = (bound - e.x0) / (e.x1 - e.x0);

                if (t > 0.f && t < 1.f) {
                    ts[cuts++] = t;
                }
            }
        }

        if (cuts == 2 && ts[0] > ts[1]) {
            float t = ts[0];

            ts[0] = ts[1];
            ts[1] = t;
        }

        u32 count = 0;
        float px[4];
        float py[4];

        px[count] = e.x0;
        py[count] = e.y0;
        count++;

        for (u32 i = 0; i < cuts; i++) {
            px[count] = e.x0 + (e.x1 - e.x0) * ts[i];
            py[count] = e.y0 + (e.y1 - e.y0) * ts[i];
            count++;
        }

        px[count] = e.x1;
        py[count] = e.y1;
        count++;

        for (u32 i = 0; i + 1 < count; i++) {
            float ax = px[i];
            float bx = px[i + 1];
            float mid = (ax + bx) * 0.5f;

            if (mid >= w) {
                continue;
            }

            if (mid <= 0.f) {
                ax = 0.f;
                bx = 0.f;
            }

            addLine(acc, stride, width, height, ax, py[i], bx, py[i + 1]);
        }
    }

    static float coverage(float value, u8 rule) {
        float a = fabsf(value);

        if (rule == WindingEvenOdd) {
            a = fmodf(a, 2.f);

            return a > 1.f ? 2.f - a : a;
        }

        return a > 1.f ? 1.f : a;
    }

    static float spreadT(float t, u8 spread) {
        if (spread == SpreadRepeat) {
            return t - floorf(t);
        }

        if (spread == SpreadReflect) {
            float m = fmodf(fabsf(t), 2.f);

            return m > 1.f ? 2.f - m : m;
        }

        return clampUnit(t);
    }

    static Color gradientColor(const DevicePaint& paint, float t) {
        const GradientStop* stops = paint.stops;
        u32 count = paint.stopCount;
        Color c;

        if (t <= stops[0].offset) {
            c = stops[0].color;
        } else if (t >= stops[count - 1].offset) {
            c = stops[count - 1].color;
        } else {
            u32 i = 1;

            while (i < count && stops[i].offset < t) {
                i++;
            }

            const GradientStop& a = stops[i - 1];
            const GradientStop& b = stops[i];
            float span = b.offset - a.offset;
            float f = span > 0.f ? (t - a.offset) / span : 1.f;

            c = Color{a.color.r + (b.color.r - a.color.r) * f, a.color.g + (b.color.g - a.color.g) * f, a.color.b + (b.color.b - a.color.b) * f, a.color.a + (b.color.a - a.color.a) * f};
        }

        return Color{c.r * c.a, c.g * c.a, c.b * c.a, c.a};
    }

    static bool paintAt(const DevicePaint& paint, float x, float y, Color& out) {
        if (paint.type == PaintType::Solid) {
            out = paint.color;

            return true;
        }

        Point p = apply(paint.inverse, Point{x, y});

        if (paint.type == PaintType::Linear) {
            float dx = paint.x2 - paint.x1;
            float dy = paint.y2 - paint.y1;
            float t = ((p.x - paint.x1) * dx + (p.y - paint.y1) * dy) / (dx * dx + dy * dy);

            out = gradientColor(paint, spreadT(t, paint.spread));

            return true;
        }

        float cdx = paint.cx - paint.fx;
        float cdy = paint.cy - paint.fy;
        float dr = paint.r - paint.fr;
        float pdx = p.x - paint.fx;
        float pdy = p.y - paint.fy;
        float a = cdx * cdx + cdy * cdy - dr * dr;
        float b = pdx * cdx + pdy * cdy + paint.fr * dr;
        float c = pdx * pdx + pdy * pdy - paint.fr * paint.fr;
        float t;

        if (fabsf(a) < 1e-9f) {
            if (b == 0.f) {
                return false;
            }

            t = c / (2.f * b);
        } else {
            float disc = b * b - a * c;

            if (disc < 0.f) {
                return false;
            }

            float root = sqrtf(disc);
            float t1 = (b + root) / a;
            float t2 = (b - root) / a;

            t = maxf(t1, t2);

            if (paint.fr + t * dr < 0.f) {
                t = minf(t1, t2);
            }
        }

        if (paint.fr + t * dr < 0.f) {
            return false;
        }

        out = gradientColor(paint, spreadT(t, paint.spread));

        return true;
    }

    static bool rasterize(Raster& r, const Lowering& l, u32 first, u32 count, Box bounds, u32& ix0, u32& iy0, u32& w, u32& h) {
        float bx0 = floorf(maxf(bounds.x0, 0.f));
        float by0 = floorf(maxf(bounds.y0, 0.f));
        float bx1 = ceilf(minf(bounds.x1, (float)r.width));
        float by1 = ceilf(minf(bounds.y1, (float)r.height));

        if (!(bx1 > bx0) || !(by1 > by0)) {
            return false;
        }

        ix0 = (u32)bx0;
        iy0 = (u32)by0;
        w = (u32)bx1 - ix0;
        h = (u32)by1 - iy0;

        u32 stride = w + 2;

        r.accumulation.zero((size_t)stride * h);

        float* acc = r.accumulation.mutData();

        for (u32 i = first; i < first + count; i++) {
            Edge e = l.edges[i];

            addEdge(acc, stride, w, h, Edge{e.x0 - ix0, e.y0 - iy0, e.x1 - ix0, e.y1 - iy0});
        }

        return true;
    }

    static void fillOp(Raster& r, const Lowering& l, const Op& op, float* target) {
        u32 ix0;
        u32 iy0;
        u32 w;
        u32 h;

        if (!rasterize(r, l, op.first, op.count, op.bounds, ix0, iy0, w, h)) {
            return;
        }

        const float* acc = r.accumulation.data();
        u32 stride = w + 2;
        const DevicePaint& paint = *op.paint;

        for (u32 y = 0; y < h; y++) {
            float sum = 0.f;
            const float* line = acc + (size_t)y * stride;
            float* row = target + ((size_t)(iy0 + y) * r.width + ix0) * 4;

            for (u32 x = 0; x < w; x++) {
                sum += line[x];

                float cov = coverage(sum, op.rule);

                if (cov <= 0.f) {
                    continue;
                }

                Color src;

                if (!paintAt(paint, (float)(ix0 + x) + 0.5f, (float)(iy0 + y) + 0.5f, src)) {
                    continue;
                }

                float* px = row + x * 4;
                float a = src.a * cov;
                float keep = 1.f - a;

                px[0] = src.r * cov + px[0] * keep;
                px[1] = src.g * cov + px[1] * keep;
                px[2] = src.b * cov + px[2] * keep;
                px[3] = a + px[3] * keep;
            }
        }
    }

    static void clipMask(Raster& r, const Lowering& l, const Op& op) {
        size_t size = (size_t)r.width * r.height;

        r.mask.zero(size);

        float* mask = r.mask.mutData();

        for (u32 s = op.clipFirst; s < op.clipFirst + op.clipCount; s++) {
            const ClipShape& shape = l.clips[s];
            Box bounds = emptyBox();

            for (u32 i = shape.first; i < shape.first + shape.count; i++) {
                extend(bounds, Point{l.edges[i].x0, l.edges[i].y0});
                extend(bounds, Point{l.edges[i].x1, l.edges[i].y1});
            }

            u32 ix0;
            u32 iy0;
            u32 w;
            u32 h;

            if (!boxValid(bounds) || !rasterize(r, l, shape.first, shape.count, bounds, ix0, iy0, w, h)) {
                continue;
            }

            const float* acc = r.accumulation.data();
            u32 stride = w + 2;

            for (u32 y = 0; y < h; y++) {
                float sum = 0.f;
                const float* line = acc + (size_t)y * stride;
                float* row = mask + (size_t)(iy0 + y) * r.width + ix0;

                for (u32 x = 0; x < w; x++) {
                    sum += line[x];
                    row[x] = maxf(row[x], coverage(sum, shape.rule));
                }
            }
        }
    }

    static void execute(Raster& r, const Lowering& l, float* base) {
        size_t size = (size_t)r.width * r.height;

        r.layers.pushBack(base);

        for (const Op& op : l.ops) {
            switch (op.kind) {
                case OpKind::Fill:
                    fillOp(r, l, op, r.layers.back());
                    break;
                case OpKind::Begin: {
                    ObjPool* pool = ObjPool::fromMemoryRaw();
                    float* layer = (float*)pool->allocate(sizeof(float) * size * 4);

                    memset(layer, 0, sizeof(float) * size * 4);
                    r.layers.pushBack(layer);
                    r.pools.pushBack(pool);
                    r.begins.pushBack(&op);
                    break;
                }
                case OpKind::End: {
                    float* layer = r.layers.popBack();
                    ObjPool* pool = r.pools.popBack();
                    const Op* begin = r.begins.popBack();
                    float* parent = r.layers.back();

                    if (begin->clipped) {
                        clipMask(r, l, *begin);
                    }

                    const float* mask = begin->clipped ? r.mask.data() : nullptr;

                    for (size_t i = 0; i < size; i++) {
                        float f = begin->opacity * (mask ? mask[i] : 1.f);
                        const float* s = layer + i * 4;

                        if (f <= 0.f || s[3] <= 0.f) {
                            continue;
                        }

                        float* d = parent + i * 4;
                        float keep = 1.f - s[3] * f;

                        d[0] = s[0] * f + d[0] * keep;
                        d[1] = s[1] * f + d[1] * keep;
                        d[2] = s[2] * f + d[2] * keep;
                        d[3] = s[3] * f + d[3] * keep;
                    }

                    delete pool;
                    break;
                }
            }
        }

        while (r.pools.length()) {
            delete r.pools.popBack();
        }
    }

    static u8 toByte(float value) {
        float v = value * 255.f + 0.5f;

        return v <= 0.f ? 0 : (v >= 255.f ? 255 : (u8)v);
    }

    static bool loadDocument(Document& doc, ObjPool& pool, StringView source) {
        doc.pool = &pool;
        doc.root = nullptr;
        doc.nodeCount = 0;
        initAtoms(doc.atoms);

        return parseXml(doc, source);
    }
}

ImageImpl::ImageImpl(u8* pixels_, u32 width, u32 height)
    : pixels(pixels_)
    , length_((size_t)width * height * 4)
    , width_(width)
    , height_(height)
{
}

const void* ImageImpl::data() const {
    return pixels;
}

size_t ImageImpl::length() const {
    return length_;
}

u32 ImageImpl::width() const {
    return width_;
}

u32 ImageImpl::height() const {
    return height_;
}

DocumentImpl::DocumentImpl(SvgDecoder* decoder_, StringView source_, float width, float height)
    : decoder(decoder_)
    , source(source_)
    , width_(width)
    , height_(height)
{
}

float DocumentImpl::width() const {
    return width_;
}

float DocumentImpl::height() const {
    return height_;
}

SvgImage* DocumentImpl::render(ObjPool& pool, u32 width, u32 height) {
    if (width == 0 || height == 0 || (u64)width * height > (1u << 26)) {
        return nullptr;
    }

    ObjPool::Ref scratch = ObjPool::fromMemory();
    Document doc;
    size_t size = (size_t)width * height;
    float* base = (float*)scratch->allocate(sizeof(float) * size * 4);
    u8* pixels = (u8*)pool.allocate(size * 4);

    memset(base, 0, sizeof(float) * size * 4);

    if (loadDocument(doc, *scratch, source) && doc.root && doc.root->name == AtomSvg) {
        Lowering l;

        cascade(doc);
        l.doc = &doc;
        l.pool = scratch.mutPtr();
        l.width = width;
        l.height = height;
        l.depth = 0;
        l.documentStyles.zero(doc.nodeCount);
        lowerElement(l, doc.root, initialStyle(), identity(), Viewport{(float)width, (float)height});

        Raster raster;

        raster.width = width;
        raster.height = height;
        execute(raster, l, base);
    }

    for (size_t i = 0; i < size; i++) {
        const float* s = base + i * 4;
        u8* d = pixels + i * 4;
        float a = s[3];

        if (a <= 0.f) {
            d[0] = 0;
            d[1] = 0;
            d[2] = 0;
            d[3] = 0;
            continue;
        }

        d[0] = toByte(s[0] / a);
        d[1] = toByte(s[1] / a);
        d[2] = toByte(s[2] / a);
        d[3] = toByte(a);
    }

    return pool.make<ImageImpl>(pixels, width, height);
}

LibraryImpl::LibraryImpl(SvgDecoder* decoder_)
    : decoder(decoder_)
{
}

SvgDocument* LibraryImpl::parse(ObjPool& pool, StringView file) {
    ObjPool::Ref scratch = ObjPool::fromMemory();
    Document doc;

    if (!loadDocument(doc, *scratch, file) || !doc.root || doc.root->name != AtomSvg) {
        return nullptr;
    }

    float width;
    float height;

    if (!intrinsicSize(doc, doc.root, width, height)) {
        width = 300.f;
        height = 150.f;
    }

    return pool.make<DocumentImpl>(decoder, keep(pool, file.data(), file.length()), width, height);
}

SvgLibrary* SvgLibrary::create(ObjPool& pool, SvgDecoder* decoder) {
    return pool.make<LibraryImpl>(decoder);
}
