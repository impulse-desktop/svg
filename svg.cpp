#include "svg.h"

#include <std/lib/buffer.h>
#include <std/lib/vector.h>
#include <std/mem/obj_pool.h>

#include <math.h>
#include <string.h>

using namespace stl;

#define SVG_ATOMS(ATOM)                                            \
    ATOM(Svg, "svg")                                               \
    ATOM(G, "g")                                                   \
    ATOM(Path, "path")                                             \
    ATOM(Rect, "rect")                                             \
    ATOM(Circle, "circle")                                         \
    ATOM(Ellipse, "ellipse")                                       \
    ATOM(Line, "line")                                             \
    ATOM(Polyline, "polyline")                                     \
    ATOM(Polygon, "polygon")                                       \
    ATOM(Use, "use")                                               \
    ATOM(Defs, "defs")                                             \
    ATOM(Symbol, "symbol")                                         \
    ATOM(LinearGradient, "linearGradient")                         \
    ATOM(RadialGradient, "radialGradient")                         \
    ATOM(Stop, "stop")                                             \
    ATOM(ClipPath, "clipPath")                                     \
    ATOM(Pattern, "pattern")                                       \
    ATOM(Marker, "marker")                                         \
    ATOM(MarkerWidth, "markerWidth")                               \
    ATOM(MarkerHeight, "markerHeight")                             \
    ATOM(MarkerUnits, "markerUnits")                               \
    ATOM(RefX, "refX")                                             \
    ATOM(RefY, "refY")                                             \
    ATOM(Orient, "orient")                                         \
    ATOM(Style, "style")                                           \
    ATOM(A, "a")                                                   \
    ATOM(Switch, "switch")                                         \
    ATOM(Image, "image")                                           \
    ATOM(Id, "id")                                                 \
    ATOM(Class, "class")                                           \
    ATOM(X, "x")                                                   \
    ATOM(Y, "y")                                                   \
    ATOM(Width, "width")                                           \
    ATOM(Height, "height")                                         \
    ATOM(Rx, "rx")                                                 \
    ATOM(Ry, "ry")                                                 \
    ATOM(Cx, "cx")                                                 \
    ATOM(Cy, "cy")                                                 \
    ATOM(R, "r")                                                   \
    ATOM(Fx, "fx")                                                 \
    ATOM(Fy, "fy")                                                 \
    ATOM(Fr, "fr")                                                 \
    ATOM(X1, "x1")                                                 \
    ATOM(Y1, "y1")                                                 \
    ATOM(X2, "x2")                                                 \
    ATOM(Y2, "y2")                                                 \
    ATOM(Points, "points")                                         \
    ATOM(D, "d")                                                   \
    ATOM(Transform, "transform")                                   \
    ATOM(ViewBox, "viewBox")                                       \
    ATOM(View, "view")                                             \
    ATOM(PreserveAspectRatio, "preserveAspectRatio")               \
    ATOM(Href, "href")                                             \
    ATOM(Offset, "offset")                                         \
    ATOM(GradientUnits, "gradientUnits")                           \
    ATOM(GradientTransform, "gradientTransform")                   \
    ATOM(SpreadMethod, "spreadMethod")                             \
    ATOM(ClipPathUnits, "clipPathUnits")                           \
    ATOM(PatternUnits, "patternUnits")                             \
    ATOM(PatternContentUnits, "patternContentUnits")               \
    ATOM(PatternTransform, "patternTransform")                     \
    ATOM(TransformOrigin, "transform-origin")                      \
    ATOM(FilterUnits, "filterUnits")                               \
    ATOM(PrimitiveUnits, "primitiveUnits")                         \
    ATOM(In, "in")                                                 \
    ATOM(In2, "in2")                                               \
    ATOM(Result, "result")                                         \
    ATOM(StdDeviation, "stdDeviation")                             \
    ATOM(Dx, "dx")                                                 \
    ATOM(Dy, "dy")                                                 \
    ATOM(Mode, "mode")                                             \
    ATOM(Values, "values")                                         \
    ATOM(Operator, "operator")                                     \
    ATOM(K1, "k1")                                                 \
    ATOM(K2, "k2")                                                 \
    ATOM(K3, "k3")                                                 \
    ATOM(K4, "k4")                                                 \
    ATOM(Order, "order")                                           \
    ATOM(KernelMatrix, "kernelMatrix")                             \
    ATOM(Divisor, "divisor")                                       \
    ATOM(Bias, "bias")                                             \
    ATOM(TargetX, "targetX")                                       \
    ATOM(TargetY, "targetY")                                       \
    ATOM(EdgeMode, "edgeMode")                                     \
    ATOM(PreserveAlpha, "preserveAlpha")                           \
    ATOM(SurfaceScale, "surfaceScale")                             \
    ATOM(DiffuseConstant, "diffuseConstant")                       \
    ATOM(SpecularConstant, "specularConstant")                     \
    ATOM(SpecularExponent, "specularExponent")                     \
    ATOM(Azimuth, "azimuth")                                       \
    ATOM(Elevation, "elevation")                                   \
    ATOM(Z, "z")                                                   \
    ATOM(PointsAtX, "pointsAtX")                                   \
    ATOM(PointsAtY, "pointsAtY")                                   \
    ATOM(PointsAtZ, "pointsAtZ")                                   \
    ATOM(LimitingConeAngle, "limitingConeAngle")                   \
    ATOM(Scale, "scale")                                           \
    ATOM(XChannelSelector, "xChannelSelector")                     \
    ATOM(YChannelSelector, "yChannelSelector")                     \
    ATOM(BaseFrequency, "baseFrequency")                           \
    ATOM(NumOctaves, "numOctaves")                                 \
    ATOM(Seed, "seed")                                             \
    ATOM(StitchTiles, "stitchTiles")                               \
    ATOM(Radius, "radius")                                         \
    ATOM(TableValues, "tableValues")                               \
    ATOM(Slope, "slope")                                           \
    ATOM(Intercept, "intercept")                                   \
    ATOM(Amplitude, "amplitude")                                   \
    ATOM(Exponent, "exponent")                                     \
    ATOM(FeBlend, "feBlend")                                       \
    ATOM(FeColorMatrix, "feColorMatrix")                           \
    ATOM(FeComponentTransfer, "feComponentTransfer")               \
    ATOM(FeComposite, "feComposite")                               \
    ATOM(FeConvolveMatrix, "feConvolveMatrix")                     \
    ATOM(FeDiffuseLighting, "feDiffuseLighting")                   \
    ATOM(FeDisplacementMap, "feDisplacementMap")                   \
    ATOM(FeDistantLight, "feDistantLight")                         \
    ATOM(FeDropShadow, "feDropShadow")                             \
    ATOM(FeFlood, "feFlood")                                       \
    ATOM(FeFuncA, "feFuncA")                                       \
    ATOM(FeFuncB, "feFuncB")                                       \
    ATOM(FeFuncG, "feFuncG")                                       \
    ATOM(FeFuncR, "feFuncR")                                       \
    ATOM(FeGaussianBlur, "feGaussianBlur")                         \
    ATOM(FeImage, "feImage")                                       \
    ATOM(FeMerge, "feMerge")                                       \
    ATOM(FeMergeNode, "feMergeNode")                               \
    ATOM(FeMorphology, "feMorphology")                             \
    ATOM(FeOffset, "feOffset")                                     \
    ATOM(FePointLight, "fePointLight")                             \
    ATOM(FeSpecularLighting, "feSpecularLighting")                 \
    ATOM(FeSpotLight, "feSpotLight")                               \
    ATOM(FeTile, "feTile")                                         \
    ATOM(FeTurbulence, "feTurbulence")                             \
    ATOM(MaskUnits, "maskUnits")                                   \
    ATOM(MaskContentUnits, "maskContentUnits")                     \
    ATOM(SystemLanguage, "systemLanguage")                         \
    ATOM(RequiredExtensions, "requiredExtensions")                 \
    ATOM(Type, "type")                                             \
    ATOM(Fill, "fill")                                             \
    ATOM(FillOpacity, "fill-opacity")                              \
    ATOM(FillRule, "fill-rule")                                    \
    ATOM(Stroke, "stroke")                                         \
    ATOM(StrokeWidth, "stroke-width")                              \
    ATOM(StrokeOpacity, "stroke-opacity")                          \
    ATOM(StrokeLinecap, "stroke-linecap")                          \
    ATOM(StrokeLinejoin, "stroke-linejoin")                        \
    ATOM(StrokeMiterlimit, "stroke-miterlimit")                    \
    ATOM(StrokeDasharray, "stroke-dasharray")                      \
    ATOM(StrokeDashoffset, "stroke-dashoffset")                    \
    ATOM(Opacity, "opacity")                                       \
    ATOM(Display, "display")                                       \
    ATOM(Visibility, "visibility")                                 \
    ATOM(Color, "color")                                           \
    ATOM(StopColor, "stop-color")                                  \
    ATOM(StopOpacity, "stop-opacity")                              \
    ATOM(ClipPathProperty, "clip-path")                            \
    ATOM(ClipRule, "clip-rule")                                    \
    ATOM(FontSize, "font-size")                                    \
    ATOM(Mask, "mask")                                             \
    ATOM(MaskType, "mask-type")                                    \
    ATOM(FloodColor, "flood-color")                                \
    ATOM(FloodOpacity, "flood-opacity")                            \
    ATOM(LightingColor, "lighting-color")                          \
    ATOM(ColorInterpolationFilters, "color-interpolation-filters") \
    ATOM(Filter, "filter")                                         \
    ATOM(BackgroundColor, "background-color")                      \
    ATOM(Overflow, "overflow")                                     \
    ATOM(MarkerStart, "marker-start")                              \
    ATOM(MarkerMid, "marker-mid")                                  \
    ATOM(MarkerEnd, "marker-end")                                  \
    ATOM(VectorEffect, "vector-effect")                            \
    ATOM(PaintOrder, "paint-order")                                \
    ATOM(MixBlendMode, "mix-blend-mode")                           \
    ATOM(Isolation, "isolation")                                   \
    ATOM(ColorInterpolation, "color-interpolation")

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
    constexpr u32 propertyCount = AtomColorInterpolation - AtomFill + 1;
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
        float screenWidth;
        float screenHeight;
        float rootFontSize;
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

    struct Rule;

    struct Document {
        ObjPool* pool;
        Interner atoms;
        Node* root;
        u32 nodeCount;
        Vector<Node*> byId;
        Rule* rules;
        u32 ruleCount;
        bool structural;
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
        Not,
    };

    struct Compound;

    struct Condition {
        Test test;
        u32 name;
        StringView value;
        Compound* inner;
        u32 innerCount;
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
        Q,
        Rem,
        Vw,
        Vh,
        Vmin,
        Vmax,
        Ch,
        Lh,
        Ic,
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
        bool context;
    };

    enum Verb : u8 {
        VerbMove,
        VerbLine,
        VerbCubic,
        VerbClose,
        VerbArcCubic,
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

    enum class FilterFunction : u8 {
        Reference,
        Matrix,
        Blur,
        DropShadow,
    };

    struct FilterItem {
        FilterFunction kind;
        u32 reference;
        float values[20];
        Color color;
    };

    enum class ShapeKind : u8 {
        None,
        Circle,
        Ellipse,
        Inset,
        Polygon,
    };

    enum class GeometryBox : u8 {
        Fill,
        Stroke,
        View,
    };

    struct BasicShape {
        ShapeKind kind;
        GeometryBox box;
        u8 rule;
        Length values[4];
        Length* points;
        u32 pointCount;
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
        BasicShape* clipShape;
        u32 mask;
        u32 markers[3];
        bool nonScaling;
        u8 blend;
        bool isolate;
        bool linearColors;
        u8 paintOrder[3];
        u8 maskType;
        Color floodColor;
        float floodOpacity;
        Color lightingColor;
        bool linearFilters;
        FilterItem* filters;
        u32 filterCount;
        Color background;
        bool overflowVisible;
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
        Image,
        Pattern,
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
        const u8* image;
        u32 imageWidth;
        u32 imageHeight;
        bool smooth;
        u32 programFirst;
        u32 programCount;
        float* tile;
        float opacity;
    };

    struct ClipShape {
        u32 first;
        u32 count;
        u8 rule;
    };

    struct IntBox {
        int x0;
        int y0;
        int x1;
        int y1;
    };

    enum FilterInputs : int {
        InputSourceGraphic = -1,
        InputSourceAlpha = -2,
        InputNone = -3,
    };

    struct FilterPrimitive {
        Node* node;
        u32 kind;
        float params[20];
        int in1;
        int in2;
        int* inputs;
        u32 inputCount;
        Box user;
        IntBox box;
        bool linear;
        Color flood;
        Color lighting;
        u32 programFirst;
        u32 programCount;
    };

    struct Filter {
        Document* doc;
        Matrix ctm;
        Box bbox;
        bool boxPrimitives;
        IntBox region;
        FilterPrimitive* primitives;
        u32 count;
        Filter* next;
    };

    enum class OpKind : u8 {
        Fill,
        Begin,
        End,
        Layer,
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
        bool clipProgram;
        bool masked;
        u8 blend;
        u8 maskType;
        u32 maskFirst;
        u32 maskCount;
        u32 maskRegion;
        Filter* filter;
        u32 layerWidth;
        u32 layerHeight;
        Matrix layerMatrix;
    };

    struct Lowering {
        Document* doc;
        ObjPool* pool;
        Vector<Op> ops;
        Vector<Op> programs;
        Vector<Op>* out;
        Vector<Edge> edges;
        Vector<ClipShape> clips;
        Vector<Node*> active;
        Vector<ComputedStyle*> documentStyles;
        SvgDecoder* decoder;
        u32 width;
        u32 height;
        u32 depth;
        u32 imageDepth;
        Node* sized;
        float sizedWidth;
        float sizedHeight;
        float rootFontSize;
        bool hasContext;
        PaintValue contextFill;
        PaintValue contextStroke;
        Matrix contextMatrix;
        Box contextBox;
        u32** instance;
        Vector<Node*> instanceTargets;
        Vector<u32**> instanceMaps;
    };

    struct Raster {
        u32 width;
        u32 height;
        Vector<float*> layers;
        Vector<ObjPool*> pools;
        Vector<const Op*> begins;
        Vector<float> accumulation;
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

    const NamedColor systemColors[] = {
        {"accentcolor", 0x0075ffff},
        {"accentcolortext", 0xffffffff},
        {"activeborder", 0x000000ff},
        {"activecaption", 0xffffffff},
        {"activetext", 0xff0000ff},
        {"appworkspace", 0xffffffff},
        {"background", 0xffffffff},
        {"buttonborder", 0x000000ff},
        {"buttonface", 0xefefefff},
        {"buttonhighlight", 0xefefefff},
        {"buttonshadow", 0xefefefff},
        {"buttontext", 0x000000ff},
        {"canvas", 0xffffffff},
        {"canvastext", 0x000000ff},
        {"captiontext", 0x000000ff},
        {"field", 0xffffffff},
        {"fieldtext", 0x000000ff},
        {"graytext", 0x808080ff},
        {"highlight", 0x0041c6cc},
        {"highlighttext", 0xffffffff},
        {"inactiveborder", 0x000000ff},
        {"inactivecaption", 0xffffffff},
        {"inactivecaptiontext", 0x808080ff},
        {"infobackground", 0xffffffff},
        {"infotext", 0x000000ff},
        {"linktext", 0x0000eeff},
        {"mark", 0xffff00ff},
        {"marktext", 0x000000ff},
        {"menu", 0xffffffff},
        {"menutext", 0x000000ff},
        {"scrollbar", 0xffffffff},
        {"selecteditem", 0x1967d2ff},
        {"selecteditemtext", 0xffffffff},
        {"threeddarkshadow", 0x000000ff},
        {"threedface", 0xefefefff},
        {"threedhighlight", 0x000000ff},
        {"threedlightshadow", 0x000000ff},
        {"threedshadow", 0x000000ff},
        {"visitedtext", 0x551a8bff},
        {"window", 0xffffffff},
        {"windowframe", 0x000000ff},
        {"windowtext", 0x000000ff},
    };

    const u8 brokenImage[16 * 16 * 4] = {
        0x00,
        0x00,
        0x00,
        0x00,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0xa3,
        0xa3,
        0xa3,
        0xff,
        0x00,
        0x00,
        0x00,
        0x0a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xa2,
        0xa2,
        0xa3,
        0xff,
        0xf3,
        0xf7,
        0xfe,
        0xff,
        0xf1,
        0xf6,
        0xfe,
        0xff,
        0xf1,
        0xf6,
        0xfe,
        0xff,
        0xf1,
        0xf6,
        0xfe,
        0xff,
        0xf1,
        0xf6,
        0xfe,
        0xff,
        0xf1,
        0xf6,
        0xfe,
        0xff,
        0xf1,
        0xf6,
        0xfe,
        0xff,
        0xf2,
        0xf6,
        0xfd,
        0xff,
        0x96,
        0x96,
        0x96,
        0xff,
        0xa2,
        0xa2,
        0xa3,
        0xff,
        0x00,
        0x00,
        0x00,
        0x0a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xa1,
        0xa1,
        0xa2,
        0xff,
        0xe4,
        0xed,
        0xfd,
        0xff,
        0xda,
        0xe6,
        0xfb,
        0xff,
        0xda,
        0xe6,
        0xfb,
        0xff,
        0xda,
        0xe6,
        0xfb,
        0xff,
        0xda,
        0xe6,
        0xfb,
        0xff,
        0xda,
        0xe6,
        0xfb,
        0xff,
        0xda,
        0xe6,
        0xfb,
        0xff,
        0xe0,
        0xea,
        0xfa,
        0xff,
        0x96,
        0x96,
        0x96,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xa1,
        0xa1,
        0xa2,
        0xff,
        0x00,
        0x00,
        0x00,
        0x0a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xa0,
        0xa0,
        0xa0,
        0xff,
        0xdc,
        0xe9,
        0xfd,
        0xff,
        0xce,
        0xde,
        0xf9,
        0xff,
        0xce,
        0xde,
        0xf9,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xce,
        0xde,
        0xf9,
        0xff,
        0xce,
        0xde,
        0xf9,
        0xff,
        0xd8,
        0xe3,
        0xf8,
        0xff,
        0x96,
        0x96,
        0x96,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xa0,
        0xa0,
        0xa0,
        0xff,
        0x00,
        0x00,
        0x00,
        0x0a,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x9e,
        0x9e,
        0x9f,
        0xff,
        0xdb,
        0xe8,
        0xfd,
        0xff,
        0xcd,
        0xdd,
        0xf8,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xcd,
        0xdd,
        0xf8,
        0xff,
        0xd7,
        0xe3,
        0xf8,
        0xff,
        0x91,
        0x91,
        0x91,
        0xff,
        0x91,
        0x91,
        0x91,
        0xff,
        0x91,
        0x91,
        0x91,
        0xff,
        0x91,
        0x91,
        0x91,
        0xff,
        0x9e,
        0x9e,
        0x9f,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x9d,
        0x9d,
        0x9e,
        0xff,
        0xdb,
        0xe8,
        0xfd,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xff,
        0xcb,
        0xdb,
        0xf7,
        0xff,
        0xce,
        0xdb,
        0xf3,
        0xff,
        0xe5,
        0xed,
        0xfb,
        0xff,
        0xee,
        0xf3,
        0xfc,
        0xff,
        0xee,
        0xf3,
        0xfc,
        0xff,
        0xf3,
        0xf7,
        0xfe,
        0xff,
        0x9d,
        0x9d,
        0x9e,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x9b,
        0x9b,
        0x9c,
        0xff,
        0xdb,
        0xe7,
        0xfd,
        0xff,
        0xc9,
        0xda,
        0xf5,
        0xff,
        0xc9,
        0xda,
        0xf5,
        0xff,
        0xc9,
        0xda,
        0xf5,
        0xff,
        0xc9,
        0xda,
        0xf5,
        0xff,
        0xc9,
        0xda,
        0xf5,
        0xff,
        0xc9,
        0xda,
        0xf5,
        0xff,
        0xcc,
        0xdc,
        0xf6,
        0xff,
        0xd2,
        0xe0,
        0xf7,
        0xff,
        0xd5,
        0xe2,
        0xf7,
        0xff,
        0xd5,
        0xe2,
        0xf7,
        0xff,
        0xe3,
        0xec,
        0xfd,
        0xff,
        0x9b,
        0x9b,
        0x9c,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x9a,
        0x9a,
        0x9b,
        0xff,
        0xd9,
        0xe6,
        0xfc,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xc6,
        0xd8,
        0xf4,
        0xff,
        0xd9,
        0xe6,
        0xfc,
        0xff,
        0x9a,
        0x9a,
        0x9b,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x98,
        0x98,
        0x99,
        0xff,
        0xd8,
        0xe6,
        0xfc,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xc5,
        0xd6,
        0xf3,
        0xff,
        0xd8,
        0xe6,
        0xfc,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x96,
        0x96,
        0x97,
        0xff,
        0xd8,
        0xe5,
        0xfc,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0xc2,
        0xd4,
        0xf1,
        0xff,
        0xb1,
        0xce,
        0xd4,
        0xff,
        0xb4,
        0xcf,
        0xd9,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0xc3,
        0xd4,
        0xf3,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x95,
        0x95,
        0x96,
        0xff,
        0xd7,
        0xe5,
        0xfc,
        0xff,
        0xc0,
        0xd1,
        0xf0,
        0xff,
        0xc0,
        0xd1,
        0xf0,
        0xff,
        0x9f,
        0xc6,
        0xb7,
        0xff,
        0x56,
        0xa9,
        0x38,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x6d,
        0xb3,
        0x5f,
        0xff,
        0xb2,
        0xcc,
        0xd8,
        0xff,
        0xc0,
        0xd1,
        0xf0,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x93,
        0x93,
        0x94,
        0xff,
        0xd6,
        0xe4,
        0xfc,
        0xff,
        0xbe,
        0xd0,
        0xf0,
        0xff,
        0x91,
        0xc1,
        0xa0,
        0xff,
        0x58,
        0xae,
        0x3a,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x5d,
        0xae,
        0x42,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x93,
        0x93,
        0x94,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x92,
        0x92,
        0x93,
        0xff,
        0xd6,
        0xe4,
        0xfc,
        0xff,
        0x9d,
        0xc4,
        0xb4,
        0xff,
        0x58,
        0xae,
        0x3a,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0xd6,
        0xe4,
        0xfc,
        0xff,
        0x92,
        0x92,
        0x93,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x90,
        0x90,
        0x92,
        0xff,
        0xc7,
        0xdd,
        0xe6,
        0xff,
        0x5f,
        0xb0,
        0x46,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x6e,
        0xb4,
        0x63,
        0xff,
        0xd4,
        0xe3,
        0xfa,
        0xff,
        0x90,
        0x90,
        0x92,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x8f,
        0x8f,
        0x90,
        0xff,
        0x7d,
        0xbe,
        0x72,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x58,
        0xae,
        0x39,
        0xff,
        0x58,
        0xae,
        0x39,
        0xff,
        0x8d,
        0xc5,
        0x8c,
        0xff,
        0x8d,
        0x90,
        0x8e,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x86,
        0x86,
        0x87,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x83,
        0x83,
        0x84,
        0xff,
        0x8a,
        0x8b,
        0x8b,
        0xff,
        0x00,
        0x00,
        0x00,
        0x00,
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

    static int hexDigit(u8 c);

    static u32 fragmentAtom(const Document& doc, StringView fragment) {
        u32 atom = lookup(doc.atoms, fragment);

        if (atom || !memchr(fragment.data(), '%', fragment.length())) {
            return atom;
        }

        Buffer decoded;
        const u8* p = fragment.begin();

        while (p < fragment.end()) {
            int high = p + 2 < fragment.end() && *p == '%' ? hexDigit(p[1]) : -1;
            int low = high >= 0 ? hexDigit(p[2]) : -1;

            if (low >= 0) {
                u8 c = (u8)(high * 16 + low);

                decoded.append(&c, 1);
                p += 3;
            } else {
                decoded.append(p, 1);
                p++;
            }
        }

        return lookup(doc.atoms, StringView(decoded));
    }

    static Node* hrefTarget(const Document& doc, const Node* node) {
        u32 value = attribute(node, AtomHref);

        if (!value) {
            return nullptr;
        }

        StringView text = trim(textOf(doc, value));

        if (text.length() < 2 || text[0] != '#') {
            return nullptr;
        }

        return elementById(doc, fragmentAtom(doc, StringView(text.begin() + 1, text.end())));
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
            Condition condition{Test::Never, 0, StringView(), nullptr, 0};

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

                    if (!name.empty() && equalsIgnoreCase(name, StringView(u8"not")) && p < end && *p == '(') {
                        Vector<Compound> inner;
                        u32 best = 0;

                        p++;

                        while (true) {
                            Compound item;
                            u32 weight = 0;

                            while (p < end && isSpace(*p)) {
                                p++;
                            }

                            item.combinator = Combinator::None;

                            if (!parseCompound(doc, p, end, item, weight)) {
                                return false;
                            }

                            inner.pushBack(item);
                            best = weight > best ? weight : best;

                            while (p < end && isSpace(*p)) {
                                p++;
                            }

                            if (p < end && *p == ',') {
                                p++;
                                continue;
                            }

                            break;
                        }

                        if (p >= end || *p != ')') {
                            return false;
                        }

                        p++;
                        condition.test = Test::Not;
                        condition.innerCount = (u32)inner.length();
                        condition.inner = (Compound*)doc.pool->allocate(sizeof(Compound) * inner.length());
                        memcpy(condition.inner, inner.data(), sizeof(Compound) * inner.length());
                        specificity += best;
                        conditions.pushBack(condition);
                        any = true;
                        continue;
                    }

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

    static bool geometryProperty(u32 atom) {
        switch (atom) {
            case AtomX:
            case AtomY:
            case AtomWidth:
            case AtomHeight:
            case AtomCx:
            case AtomCy:
            case AtomR:
            case AtomRx:
            case AtomRy:
            case AtomD:
            case AtomTransform:
            case AtomTransformOrigin:
                return true;
            default:
                return false;
        }
    }

    static bool geometryApplies(u32 element, u32 property) {
        switch (property) {
            case AtomX:
            case AtomY:
                return element == AtomRect || element == AtomImage || element == AtomSvg || element == AtomUse;
            case AtomWidth:
            case AtomHeight:
                return element == AtomRect || element == AtomImage || element == AtomSvg || element == AtomUse || element == AtomSymbol;
            case AtomCx:
            case AtomCy:
                return element == AtomCircle || element == AtomEllipse;
            case AtomR:
                return element == AtomCircle;
            case AtomRx:
            case AtomRy:
                return element == AtomRect || element == AtomEllipse;
            case AtomD:
                return element == AtomPath;
            default:
                return true;
        }
    }

    static const u8* findFunction(StringView text, StringView name) {
        for (const u8* p = text.begin(); p + name.length() <= text.end(); p++) {
            if (equalsIgnoreCase(StringView(p, p + name.length()), name)) {
                return p;
            }
        }

        return nullptr;
    }

    static bool substituteFallbacks(StringView text, Buffer& out, u32 depth) {
        const u8* p = text.begin();

        while (p < text.end()) {
            const u8* var = findFunction(StringView(p, text.end()), StringView(u8"var("));
            const u8* env = findFunction(StringView(p, text.end()), StringView(u8"env("));
            const u8* at = var && (!env || var < env) ? var : env;

            if (!at) {
                out.append(p, (size_t)(text.end() - p));

                return true;
            }

            out.append(p, (size_t)(at - p));

            const u8* q = at + 4;
            const u8* comma = nullptr;
            int level = 0;

            while (q < text.end() && (level > 0 || *q != ')')) {
                if (*q == '(') {
                    level++;
                } else if (*q == ')') {
                    level--;
                } else if (*q == ',' && level == 0 && !comma) {
                    comma = q;
                }

                q++;
            }

            if (q >= text.end() || !comma || depth > 8) {
                return false;
            }

            if (!substituteFallbacks(trim(StringView(comma + 1, q)), out, depth + 1)) {
                return false;
            }

            p = q + 1;
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

            if (StringView(lowered, name.length()) == StringView(u8"-webkit-clip-path")) {
                property = AtomClipPathProperty;
            }

            if (!(property >= firstProperty && property < firstProperty + propertyCount) && !geometryProperty(property) && property != AtomMarker) {
                continue;
            }

            if (value.empty()) {
                continue;
            }

            Buffer substituted;

            if (findFunction(value, StringView(u8"var(")) || findFunction(value, StringView(u8"env("))) {
                if (!substituteFallbacks(value, substituted, 0)) {
                    continue;
                }

                value = trim(keep(*doc.pool, substituted.data(), substituted.length()));
            }

            if (property == AtomMarker) {
                if (!value.empty()) {
                    u32 atom = intern(doc.atoms, value);

                    out.pushBack(Declaration{AtomMarkerStart, atom, important});
                    out.pushBack(Declaration{AtomMarkerMid, atom, important});
                    out.pushBack(Declaration{AtomMarkerEnd, atom, important});
                }

                continue;
            }

            if (property == AtomD && startsWithIgnoreCase(value, StringView(u8"path(")) && value.back() == ')') {
                StringView inner = trim(StringView(value.begin() + 5, value.end() - 1));

                if (inner.length() >= 2 && (inner[0] == '"' || inner[0] == '\'') && inner.back() == inner[0]) {
                    value = StringView(inner.begin() + 1, inner.end() - 1);
                }
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

    static bool matchCondition(Document& doc, Node* node, const Condition& condition, const Node* boundary) {
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
                return node == doc.root && !boundary;
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

    static bool matchCompound(Document& doc, Node* node, const Compound& compound, const Node* boundary) {
        if (compound.type && node->name != compound.type) {
            return false;
        }

        for (u32 i = 0; i < compound.count; i++) {
            const Condition& condition = compound.conditions[i];

            if (condition.test == Test::Not) {
                for (u32 k = 0; k < condition.innerCount; k++) {
                    if (matchCompound(doc, node, condition.inner[k], boundary)) {
                        return false;
                    }
                }

                continue;
            }

            if (!matchCondition(doc, node, condition, boundary)) {
                return false;
            }
        }

        return true;
    }

    static bool matchFrom(Document& doc, Node* node, const Selector& selector, u32 index, const Node* boundary) {
        if (!matchCompound(doc, node, selector.compounds[index], boundary)) {
            return false;
        }

        if (index + 1 == selector.count) {
            return true;
        }

        switch (selector.compounds[index].combinator) {
            case Combinator::Descendant:
                for (Node* up = node == boundary ? nullptr : node->parent; up; up = up == boundary ? nullptr : up->parent) {
                    if (matchFrom(doc, up, selector, index + 1, boundary)) {
                        return true;
                    }
                }

                return false;
            case Combinator::Child:
                return node != boundary && node->parent && matchFrom(doc, node->parent, selector, index + 1, boundary);
            case Combinator::Adjacent:
                return node != boundary && node->prev && matchFrom(doc, node->prev, selector, index + 1, boundary);
            case Combinator::Sibling:
                for (Node* sibling = node == boundary ? nullptr : node->prev; sibling; sibling = sibling->prev) {
                    if (matchFrom(doc, sibling, selector, index + 1, boundary)) {
                        return true;
                    }
                }

                return false;
            default:
                return false;
        }
    }

    static void setAttribute(Document& doc, Node* node, u32 name, u32 value) {
        for (u32 i = 0; i < node->count; i++) {
            if (node->attributes[i].name == name) {
                node->attributes[i].value = value;

                return;
            }
        }

        Attribute* attributes = (Attribute*)doc.pool->allocate(sizeof(Attribute) * (node->count + 1));

        if (node->count) {
            memcpy(attributes, node->attributes, sizeof(Attribute) * node->count);
        }

        attributes[node->count] = Attribute{name, value};
        node->attributes = attributes;
        node->count++;
    }

    static void considerGeometry(Vector<Declaration>& geometry, Vector<u64>& priorities, const Declaration& declaration, u64 priority) {
        for (size_t i = 0; i < geometry.length(); i++) {
            if (geometry[i].property == declaration.property) {
                if (priority >= priorities[i]) {
                    geometry.mut(i) = declaration;
                    priorities.mut(i) = priority;
                }

                return;
            }
        }

        geometry.pushBack(declaration);
        priorities.pushBack(priority);
    }

    static void consider(Candidate* best, u32 property, u32 value, u64 priority) {
        if (property < firstProperty || property >= firstProperty + propertyCount) {
            return;
        }

        Candidate& slot = best[property - firstProperty];

        if (priority >= slot.priority) {
            slot.value = value;
            slot.priority = priority;
        }
    }

    static u32* cascadeNode(Document& doc, Node* node, const Node* boundary) {
        Candidate best[propertyCount];
        Vector<Declaration> geometry;
        Vector<u64> geometryPriorities;

        memset(best, 0, sizeof(best));

        for (u32 i = 0; i < node->count; i++) {
            u32 name = node->attributes[i].name;

            if (name >= firstProperty && name < firstProperty + propertyCount && name != AtomMixBlendMode && name != AtomIsolation) {
                u32 value = node->attributes[i].value;
                StringView text = textOf(doc, value);

                if (findFunction(text, StringView(u8"env("))) {
                    Buffer substituted;

                    value = substituteFallbacks(text, substituted, 0) ? intern(doc.atoms, trim(keep(*doc.pool, substituted.data(), substituted.length()))) : 0;
                }

                if (value) {
                    consider(best, name, value, 1);
                }
            }
        }

        for (u32 r = 0; r < doc.ruleCount; r++) {
            const Rule& rule = doc.rules[r];
            u32 specificity = 0;
            bool matched = false;

            for (u32 s = 0; s < rule.selectorCount; s++) {
                if (matchFrom(doc, node, rule.selectors[s], 0, boundary)) {
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

                u64 priority = (level << 56) | ((u64)specificity << 32) | ((u64)r << 12) | d;

                if (geometryProperty(declaration.property)) {
                    considerGeometry(geometry, geometryPriorities, declaration, priority);
                } else {
                    consider(best, declaration.property, declaration.value, priority);
                }
            }
        }

        if (u32 inlineStyle = attribute(node, AtomStyle)) {
            Vector<Declaration> declarations;
            Buffer cleaned;

            stripComments(textOf(doc, inlineStyle), cleaned);
            parseDeclarations(doc, keep(*doc.pool, cleaned.data(), cleaned.length()), declarations);

            for (u32 d = 0; d < declarations.length(); d++) {
                u64 level = declarations[d].important ? 4 : 2;
                u64 priority = (level << 56) | d;

                if (geometryProperty(declarations[d].property)) {
                    considerGeometry(geometry, geometryPriorities, declarations[d], priority);
                } else {
                    consider(best, declarations[d].property, declarations[d].value, priority);
                }
            }
        }

        for (const Declaration& declaration : geometry) {
            if (boundary) {
                break;
            }

            bool outer = node == doc.root && declaration.property != AtomTransform && declaration.property != AtomTransformOrigin;

            if (geometryApplies(node->name, declaration.property) && !outer) {
                u32 property = declaration.property;

                if (property == AtomTransform && (node->name == AtomLinearGradient || node->name == AtomRadialGradient)) {
                    property = AtomGradientTransform;
                } else if (property == AtomTransform && node->name == AtomPattern) {
                    property = AtomPatternTransform;
                }

                setAttribute(doc, node, property, declaration.value);
            }
        }

        u32* style = (u32*)doc.pool->allocate(sizeof(u32) * propertyCount);

        for (u32 i = 0; i < propertyCount; i++) {
            style[i] = best[i].value;
        }

        return style;
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

        doc.ruleCount = (u32)rules.length();
        doc.rules = (Rule*)doc.pool->allocate(sizeof(Rule) * (rules.length() + 1));
        doc.structural = false;

        if (rules.length()) {
            memcpy(doc.rules, rules.data(), sizeof(Rule) * rules.length());
        }

        for (u32 r = 0; r < doc.ruleCount; r++) {
            for (u32 k = 0; k < doc.rules[r].selectorCount; k++) {
                const Selector& selector = doc.rules[r].selectors[k];

                doc.structural = doc.structural || selector.count > 1;

                for (u32 c = 0; c < selector.count; c++) {
                    for (u32 d = 0; d < selector.compounds[c].count; d++) {
                        Test test = selector.compounds[c].conditions[d].test;

                        doc.structural = doc.structural || test == Test::Root || test == Test::FirstChild || test == Test::LastChild || test == Test::OnlyChild || test == Test::Not;
                    }
                }
            }
        }

        for (Node* node = doc.root; node; node = nextNode(node, doc.root)) {
            node->style = cascadeNode(doc, node, nullptr);
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
        } else if (s < end && isIdentStart(*s) && *s != '-') {
            const u8* unitEnd = s;

            while (unitEnd < end && ((*unitEnd >= 'a' && *unitEnd <= 'z') || (*unitEnd >= 'A' && *unitEnd <= 'Z'))) {
                unitEnd++;
            }

            StringView unit(s, unitEnd);

            static const struct {
                const char* name;
                Unit unit;
            } units[] = {
                {"px", Unit::Px},
                {"em", Unit::Em},
                {"ex", Unit::Ex},
                {"in", Unit::In},
                {"cm", Unit::Cm},
                {"mm", Unit::Mm},
                {"pt", Unit::Pt},
                {"pc", Unit::Pc},
                {"q", Unit::Q},
                {"rem", Unit::Rem},
                {"vw", Unit::Vw},
                {"vh", Unit::Vh},
                {"vi", Unit::Vw},
                {"vb", Unit::Vh},
                {"vmin", Unit::Vmin},
                {"vmax", Unit::Vmax},
                {"ch", Unit::Ch},
                {"lh", Unit::Lh},
                {"rlh", Unit::Lh},
                {"ic", Unit::Ic},
            };

            bool known = false;

            for (const auto& candidate : units) {
                if (equalsIgnoreCase(unit, StringView(candidate.name))) {
                    out.unit = candidate.unit;
                    known = true;
                    break;
                }
            }

            if (!known || (unitEnd < end && isIdentChar(*unitEnd))) {
                return false;
            }

            s = unitEnd;
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
                return length.value * fontSize * 0.459f;
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
            case Unit::Q:
                return length.value * 96.f / 101.6f;
            case Unit::Rem:
                return length.value * (vp.rootFontSize > 0.f ? vp.rootFontSize : 16.f);
            case Unit::Vw:
                return length.value * vp.screenWidth / 100.f;
            case Unit::Vh:
                return length.value * vp.screenHeight / 100.f;
            case Unit::Vmin:
                return length.value * minf(vp.screenWidth, vp.screenHeight) / 100.f;
            case Unit::Vmax:
                return length.value * maxf(vp.screenWidth, vp.screenHeight) / 100.f;
            case Unit::Ch:
                return length.value * fontSize * 0.5f;
            case Unit::Lh:
                return length.value * fontSize * 1.149f;
            case Unit::Ic:
                return length.value * fontSize;
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
        u32 percents = 0;

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
            } else {
                while (p < end && isSpace(*p)) {
                    p++;
                }

                const u8* start = p;

                if (!readComponent(p, end, scale, values[count])) {
                    return false;
                }

                if (rgb && count < 3 && p > start && p[-1] == '%') {
                    percents++;
                }
            }

            count++;
        }

        while (p < end && isSpace(*p)) {
            p++;
        }

        if (p != end || count < 3 || (percents != 0 && percents != 3)) {
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

        for (const NamedColor& named : systemColors) {
            if (equalsIgnoreCase(text, StringView(named.name))) {
                out = Color{((named.rgb >> 24) & 0xff) / 255.f, ((named.rgb >> 16) & 0xff) / 255.f, ((named.rgb >> 8) & 0xff) / 255.f, (named.rgb & 0xff) / 255.f};

                return true;
            }
        }

        return false;
    }

    static bool parsePaint(const Document& doc, StringView source, PaintValue& out) {
        StringView text = trim(source);

        out.server = 0;
        out.fallback = PaintKind::None;
        out.context = false;

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

            while (close < text.end() && isSpace(*close)) {
                close++;
            }

            if (close < text.end() && (*close == '"' || *close == '\'')) {
                u8 quote = *close++;

                while (close < text.end() && *close != quote) {
                    close++;
                }
            }

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
                out.server = fragmentAtom(doc, StringView(reference.begin() + 1, reference.end()));
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

        if (equalsIgnoreCase(trim(text), StringView(u8"none"))) {
            out = result;

            return true;
        }

        bool first = true;

        while (true) {
            bool separator = false;

            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p < end && *p == ',') {
                if (first) {
                    return false;
                }

                separator = true;
                p++;

                while (p < end && isSpace(*p)) {
                    p++;
                }
            }

            if (p >= end) {
                if (separator) {
                    return false;
                }

                break;
            }

            first = false;

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

                bool comma = false;

                const u8* unitStart = p;

                while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z'))) {
                    p++;
                }

                StringView unit(unitStart, p);

                if (equalsIgnoreCase(unit, StringView(u8"rad"))) {
                    args[count] *= 180.f / pi;
                } else if (equalsIgnoreCase(unit, StringView(u8"grad"))) {
                    args[count] *= 0.9f;
                } else if (equalsIgnoreCase(unit, StringView(u8"turn"))) {
                    args[count] *= 360.f;
                } else if (!unit.empty() && !equalsIgnoreCase(unit, StringView(u8"deg")) && !equalsIgnoreCase(unit, StringView(u8"px"))) {
                    return false;
                }

                count++;

                while (p < end && isSpace(*p)) {
                    p++;
                }

                if (p < end && *p == ',') {
                    comma = true;
                    p++;
                }

                while (p < end && isSpace(*p)) {
                    p++;
                }

                if (comma && (p >= end || *p == ')' || *p == ',')) {
                    return false;
                }
            }

            Matrix m;

            if (name == StringView(u8"translateX") && count == 1) {
                name = StringView(u8"translate");
            } else if (name == StringView(u8"translateY") && count == 1) {
                name = StringView(u8"translate");
                args[1] = args[0];
                args[0] = 0.f;
                count = 2;
            } else if (name == StringView(u8"scaleX") && count == 1) {
                name = StringView(u8"scale");
                args[1] = 1.f;
                count = 2;
            } else if (name == StringView(u8"scaleY") && count == 1) {
                name = StringView(u8"scale");
                args[1] = args[0];
                args[0] = 1.f;
                count = 2;
            } else if (name == StringView(u8"skew") && (count == 1 || count == 2)) {
                float ax = tanf(args[0] * pi / 180.f);
                float ay = count == 2 ? tanf(args[1] * pi / 180.f) : 0.f;

                result = multiply(result, Matrix{1.f, ay, ax, 1.f, 0.f, 0.f});
                continue;
            }

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

    static bool readOrigin(const u8*& p, const u8* end, float extentX, float extentY, float& x, float& y, int& axis) {
        while (p < end && isSpace(*p)) {
            p++;
        }

        if (p >= end) {
            return false;
        }

        static const struct {
            const char* name;
            int axis;
            float fraction;
        } keywords[] = {
            {"left", 0, 0.f},
            {"right", 0, 1.f},
            {"top", 1, 0.f},
            {"bottom", 1, 1.f},
            {"center", 2, 0.5f},
        };

        for (const auto& keyword : keywords) {
            size_t n = strlen(keyword.name);

            if ((size_t)(end - p) >= n && equalsIgnoreCase(StringView(p, p + n), StringView(keyword.name))) {
                p += n;
                axis = keyword.axis;
                x = keyword.fraction * extentX;
                y = keyword.fraction * extentY;

                return true;
            }
        }

        Length length;

        if (!readLength(p, end, length)) {
            return false;
        }

        axis = 3;
        x = length.unit == Unit::Percent ? length.value * extentX / 100.f : resolveLength(length, Axis::X, Viewport{extentX, extentY, extentX, extentY, 16.f}, 16.f);
        y = length.unit == Unit::Percent ? length.value * extentY / 100.f : x;

        return true;
    }

    static Point transformOrigin(StringView text, float extentX, float extentY) {
        const u8* p = text.begin();
        const u8* end = text.end();
        float x1;
        float y1;
        float x2;
        float y2;
        int a1;
        int a2;

        if (!readOrigin(p, end, extentX, extentY, x1, y1, a1)) {
            return Point{0.f, 0.f};
        }

        if (!readOrigin(p, end, extentX, extentY, x2, y2, a2)) {
            if (a1 == 1) {
                return Point{extentX / 2.f, y1};
            }

            return Point{x1, a1 == 0 ? extentY / 2.f : (a1 == 2 ? extentY / 2.f : extentY / 2.f)};
        }

        if (a1 == 1 || a2 == 0) {
            return Point{x2, y1};
        }

        return Point{x1, y2};
    }

    static Matrix originTransform(const Document& doc, const Node* node, u32 name, Viewport vp) {
        u32 value = attribute(node, name);
        Matrix m;

        if (!value || !parseTransform(textOf(doc, value), m)) {
            return identity();
        }

        u32 origin = attribute(node, AtomTransformOrigin);

        if (!origin) {
            return m;
        }

        Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

        return multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
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
            lineTo(path, p1);

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

            if (i + 1 < segments) {
                path.verbs.mutData()[path.verbs.length() - 1] = VerbArcCubic;
            }
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

            if (p >= end) {
                return;
            }

            if (!readNumber(p, end, point.x)) {
                out.clear();

                return;
            }

            skipSeparator(p, end);

            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end) {
                return;
            }

            if (!readNumber(p, end, point.y)) {
                out.clear();

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
                case VerbCubic:
                case VerbArcCubic: {
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

    static Path transformedPath(const Path& path, const Matrix& m) {
        Path out;

        out.verbs.append(path.verbs.data(), path.verbs.length());

        for (Point p : path.points) {
            out.points.pushBack(apply(m, p));
        }

        return out;
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
        style.paintOrder[0] = 0;
        style.paintOrder[1] = 1;
        style.paintOrder[2] = 2;
        style.opacity = 1.f;
        style.stopOpacity = 1.f;
        style.fontSize = 16.f;
        style.fillRule = WindingNonZero;
        style.clipRule = WindingNonZero;
        style.cap = CapButt;
        style.join = JoinMiter;
        style.visible = true;
        style.floodColor = Color{0.f, 0.f, 0.f, 1.f};
        style.floodOpacity = 1.f;
        style.lightingColor = Color{1.f, 1.f, 1.f, 1.f};
        style.linearFilters = true;

        return style;
    }

    static StringView cascaded(const Document& doc, const Node* node, u32 property) {
        u32 value = node->style ? node->style[property - firstProperty] : 0;

        return value ? textOf(doc, value) : StringView();
    }

    static bool readFilterAmount(StringView args, float& out) {
        const u8* p = args.begin();
        const u8* end = args.end();

        if (args.empty()) {
            out = 1.f;

            return true;
        }

        if (!readNumber(p, end, out)) {
            return false;
        }

        if (p < end && *p == '%') {
            out /= 100.f;
            p++;
        }

        return p == end && out >= 0.f;
    }

    static bool readFilterAngle(StringView args, float& out) {
        const u8* p = args.begin();
        const u8* end = args.end();

        out = 0.f;

        if (args.empty()) {
            return true;
        }

        if (!readNumber(p, end, out)) {
            return false;
        }

        StringView unit(p, end);

        if (equalsIgnoreCase(unit, StringView(u8"deg"))) {
            return true;
        }

        if (equalsIgnoreCase(unit, StringView(u8"rad"))) {
            out *= 180.f / pi;

            return true;
        }

        if (equalsIgnoreCase(unit, StringView(u8"grad"))) {
            out *= 0.9f;

            return true;
        }

        if (equalsIgnoreCase(unit, StringView(u8"turn"))) {
            out *= 360.f;

            return true;
        }

        return unit.empty() && out == 0.f;
    }

    static bool readFilterLength(StringView text, bool unitless, const ComputedStyle& style, Viewport vp, float& out) {
        Length length;

        if (!parseLength(text, length) || length.unit == Unit::Percent) {
            return false;
        }

        if (length.unit == Unit::Number && !unitless && length.value != 0.f) {
            return false;
        }

        out = resolveLength(length, Axis::Other, vp, style.fontSize);

        return true;
    }

    static void amountMatrix(StringView name, float a, float* m) {
        float b = 1.f - a;
        float identity[20] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};

        memcpy(m, identity, sizeof(identity));

        if (name == StringView(u8"grayscale")) {
            float t[20] = {0.2126f + 0.7874f * b, 0.7152f - 0.7152f * b, 0.0722f - 0.0722f * b, 0, 0, 0.2126f - 0.2126f * b, 0.7152f + 0.2848f * b, 0.0722f - 0.0722f * b, 0, 0, 0.2126f - 0.2126f * b, 0.7152f - 0.7152f * b, 0.0722f + 0.9278f * b, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(t));
        } else if (name == StringView(u8"sepia")) {
            float t[20] = {0.393f + 0.607f * b, 0.769f - 0.769f * b, 0.189f - 0.189f * b, 0, 0, 0.349f - 0.349f * b, 0.686f + 0.314f * b, 0.168f - 0.168f * b, 0, 0, 0.272f - 0.272f * b, 0.534f - 0.534f * b, 0.131f + 0.869f * b, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(t));
        } else if (name == StringView(u8"saturate")) {
            float t[20] = {0.213f + 0.787f * a, 0.715f - 0.715f * a, 0.072f - 0.072f * a, 0, 0, 0.213f - 0.213f * a, 0.715f + 0.285f * a, 0.072f - 0.072f * a, 0, 0, 0.213f - 0.213f * a, 0.715f - 0.715f * a, 0.072f + 0.928f * a, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(t));
        } else if (name == StringView(u8"invert")) {
            for (int k = 0; k < 3; k++) {
                m[k * 6] = 1.f - 2.f * a;
                m[k * 5 + 4] = a;
            }
        } else if (name == StringView(u8"opacity")) {
            m[18] = a;
        } else if (name == StringView(u8"brightness")) {
            for (int k = 0; k < 3; k++) {
                m[k * 6] = a;
            }
        } else if (name == StringView(u8"contrast")) {
            for (int k = 0; k < 3; k++) {
                m[k * 6] = a;
                m[k * 5 + 4] = 0.5f - 0.5f * a;
            }
        }
    }

    static void hueMatrix(float degrees, float* m) {
        float angle = degrees * pi / 180.f;
        float cs = cosf(angle);
        float sn = sinf(angle);
        float t[20] = {
            0.213f + cs * 0.787f - sn * 0.213f,
            0.715f - cs * 0.715f - sn * 0.715f,
            0.072f - cs * 0.072f + sn * 0.928f,
            0,
            0,
            0.213f - cs * 0.213f + sn * 0.143f,
            0.715f + cs * 0.285f + sn * 0.140f,
            0.072f - cs * 0.072f - sn * 0.283f,
            0,
            0,
            0.213f - cs * 0.213f - sn * 0.787f,
            0.715f - cs * 0.715f + sn * 0.715f,
            0.072f + cs * 0.928f + sn * 0.072f,
            0,
            0,
            0,
            0,
            0,
            1,
            0,
        };

        memcpy(m, t, sizeof(t));
    }

    static bool parseDropShadow(StringView args, const ComputedStyle& style, Viewport vp, FilterItem& item) {
        const u8* p = args.begin();
        const u8* end = args.end();
        StringView tokens[5];
        u32 count = 0;

        while (true) {
            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end) {
                break;
            }

            const u8* start = p;
            int depth = 0;

            while (p < end && (depth > 0 || !isSpace(*p))) {
                if (*p == '(') {
                    depth++;
                } else if (*p == ')') {
                    depth--;
                } else if (*p == ',' && depth == 0) {
                    return false;
                }

                p++;
            }

            if (count == 5) {
                return false;
            }

            tokens[count++] = StringView(start, p);
        }

        float lengths[4];
        u32 lengthCount = 0;
        bool hasColor = false;

        item.color = style.color;

        for (u32 k = 0; k < count; k++) {
            float value;

            if (readFilterLength(tokens[k], true, style, vp, value)) {
                if (lengthCount == 3 || (hasColor && k != lengthCount + 1)) {
                    return false;
                }

                lengths[lengthCount++] = value;

                continue;
            }

            if (hasColor || (k != 0 && k != count - 1)) {
                return false;
            }

            if (!equalsIgnoreCase(tokens[k], StringView(u8"currentcolor")) && !parseColor(tokens[k], item.color)) {
                return false;
            }

            hasColor = true;
        }

        if (lengthCount < 2 || (lengthCount == 3 && lengths[2] < 0.f)) {
            return false;
        }

        item.values[0] = lengths[0];
        item.values[1] = lengths[1];
        item.values[2] = lengthCount == 3 ? lengths[2] : 0.f;

        return true;
    }

    static bool parseFilterList(const Document& doc, StringView text, const ComputedStyle& style, Viewport vp, Vector<FilterItem>& out) {
        const u8* p = text.begin();
        const u8* end = text.end();

        while (true) {
            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end) {
                break;
            }

            const u8* start = p;

            while (p < end && ((*p >= 'a' && *p <= 'z') || (*p >= 'A' && *p <= 'Z') || *p == '-')) {
                p++;
            }

            StringView name(start, p);

            if (p >= end || *p != '(' || name.empty()) {
                return false;
            }

            p++;

            const u8* open = p;
            int depth = 0;

            while (p < end && (depth > 0 || *p != ')')) {
                if (*p == '(') {
                    depth++;
                } else if (*p == ')') {
                    depth--;
                }

                p++;
            }

            if (p >= end) {
                return false;
            }

            StringView args = trim(StringView(open, p));
            FilterItem item;

            p++;
            memset(&item, 0, sizeof(item));

            if (equalsIgnoreCase(name, StringView(u8"url"))) {
                if (args.length() >= 2 && (args[0] == '"' || args[0] == '\'') && args.back() == args[0]) {
                    args = trim(StringView(args.begin() + 1, args.end() - 1));
                }

                item.kind = FilterFunction::Reference;

                if (!args.empty() && args[0] == '#') {
                    item.reference = fragmentAtom(doc, StringView(args.begin() + 1, args.end()));
                }
            } else if (equalsIgnoreCase(name, StringView(u8"blur"))) {
                float sigma = 0.f;

                if (!args.empty() && (!readFilterLength(args, false, style, vp, sigma) || sigma < 0.f)) {
                    return false;
                }

                item.kind = FilterFunction::Blur;
                item.values[0] = sigma;
                item.values[1] = sigma;
            } else if (equalsIgnoreCase(name, StringView(u8"drop-shadow"))) {
                item.kind = FilterFunction::DropShadow;

                if (!parseDropShadow(args, style, vp, item)) {
                    return false;
                }
            } else if (equalsIgnoreCase(name, StringView(u8"hue-rotate"))) {
                float angle;

                if (!readFilterAngle(args, angle)) {
                    return false;
                }

                item.kind = FilterFunction::Matrix;
                hueMatrix(angle, item.values);
            } else {
                static const char* const amounts[] = {"grayscale", "sepia", "saturate", "invert", "opacity", "brightness", "contrast"};
                StringView known;
                float amount;

                for (const char* candidate : amounts) {
                    if (equalsIgnoreCase(name, StringView(candidate))) {
                        known = StringView(candidate);
                    }
                }

                if (known.empty() || !readFilterAmount(args, amount)) {
                    return false;
                }

                if (known != StringView(u8"saturate") && known != StringView(u8"brightness") && known != StringView(u8"contrast")) {
                    amount = amount > 1.f ? 1.f : amount;
                }

                item.kind = FilterFunction::Matrix;
                amountMatrix(known, amount, item.values);
            }

            out.pushBack(item);
        }

        return out.length() > 0;
    }

    static StringView shapeWord(const u8*& p, const u8* end) {
        while (p < end && (isSpace(*p) || *p == ',')) {
            p++;
        }

        const u8* start = p;

        while (p < end && !isSpace(*p) && *p != ',') {
            p++;
        }

        return StringView(start, p);
    }

    static bool parsePosition(StringView text, Length& x, Length& y) {
        const u8* p = text.begin();
        StringView words[2];
        u32 count = 0;

        x = Length{50.f, Unit::Percent};
        y = Length{50.f, Unit::Percent};

        while (count < 3) {
            StringView word = shapeWord(p, text.end());

            if (word.empty()) {
                break;
            }

            if (count == 2) {
                return false;
            }

            words[count++] = word;
        }

        bool xSet = false;
        bool ySet = false;

        for (u32 i = 0; i < count; i++) {
            StringView word = words[i];
            Length length;

            if (equalsIgnoreCase(word, StringView(u8"left")) || equalsIgnoreCase(word, StringView(u8"right"))) {
                if (xSet) {
                    return false;
                }

                x = Length{equalsIgnoreCase(word, StringView(u8"left")) ? 0.f : 100.f, Unit::Percent};
                xSet = true;
            } else if (equalsIgnoreCase(word, StringView(u8"top")) || equalsIgnoreCase(word, StringView(u8"bottom"))) {
                if (ySet) {
                    return false;
                }

                y = Length{equalsIgnoreCase(word, StringView(u8"top")) ? 0.f : 100.f, Unit::Percent};
                ySet = true;
            } else if (equalsIgnoreCase(word, StringView(u8"center"))) {
                if (i == 0 && !xSet) {
                    xSet = count == 1;
                }
            } else if (parseLength(word, length)) {
                if (i == 0 && !xSet) {
                    x = length;
                    xSet = true;
                } else if (!ySet) {
                    y = length;
                    ySet = true;
                } else {
                    return false;
                }
            } else {
                return false;
            }
        }

        return true;
    }

    static bool parseShapeFunction(ObjPool& pool, StringView name, StringView args, BasicShape& out);

    static bool parseBasicShape(ObjPool& pool, StringView text, BasicShape& out) {
        const u8* p = trim(text).begin();
        const u8* end = trim(text).end();
        StringView function;

        out.kind = ShapeKind::None;
        out.box = GeometryBox::Stroke;
        out.rule = WindingNonZero;
        out.points = nullptr;
        out.pointCount = 0;

        while (p < end) {
            while (p < end && isSpace(*p)) {
                p++;
            }

            if (p >= end) {
                break;
            }

            const u8* start = p;

            while (p < end && !isSpace(*p) && *p != '(') {
                p++;
            }

            StringView name(start, p);

            if (p < end && *p == '(') {
                if (!function.empty()) {
                    return false;
                }

                const u8* open = ++p;
                int depth = 0;

                while (p < end && (depth > 0 || *p != ')')) {
                    depth += *p == '(' ? 1 : (*p == ')' ? -1 : 0);
                    p++;
                }

                if (p >= end) {
                    return false;
                }

                function = name;

                StringView args = trim(StringView(open, p));

                p++;

                if (!parseShapeFunction(pool, name, args, out)) {
                    return false;
                }

                continue;
            }

            if (equalsIgnoreCase(name, StringView(u8"fill-box")) || equalsIgnoreCase(name, StringView(u8"content-box")) || equalsIgnoreCase(name, StringView(u8"padding-box"))) {
                out.box = GeometryBox::Fill;
            } else if (equalsIgnoreCase(name, StringView(u8"stroke-box")) || equalsIgnoreCase(name, StringView(u8"border-box")) || equalsIgnoreCase(name, StringView(u8"margin-box"))) {
                out.box = GeometryBox::Stroke;
            } else if (equalsIgnoreCase(name, StringView(u8"view-box"))) {
                out.box = GeometryBox::View;
            } else {
                return false;
            }
        }

        return out.kind != ShapeKind::None;
    }

    static bool parseShapeFunction(ObjPool& pool, StringView name, StringView args, BasicShape& out) {
        const u8* end = args.end();

        if (equalsIgnoreCase(name, StringView(u8"circle")) || equalsIgnoreCase(name, StringView(u8"ellipse"))) {
            bool circle = equalsIgnoreCase(name, StringView(u8"circle"));
            const u8* p = args.begin();
            u32 radii = 0;

            out.kind = circle ? ShapeKind::Circle : ShapeKind::Ellipse;
            out.values[0] = Length{-1.f, Unit::Number};
            out.values[1] = Length{-1.f, Unit::Number};

            while (true) {
                StringView word = shapeWord(p, end);

                if (word.empty()) {
                    break;
                }

                if (equalsIgnoreCase(word, StringView(u8"at"))) {
                    return parsePosition(StringView(p, end), out.values[2], out.values[3]);
                }

                if (radii == (circle ? 1u : 2u)) {
                    return false;
                }

                Length radius;

                if (equalsIgnoreCase(word, StringView(u8"closest-side"))) {
                    radius = Length{-1.f, Unit::Number};
                } else if (equalsIgnoreCase(word, StringView(u8"farthest-side"))) {
                    radius = Length{-2.f, Unit::Number};
                } else if (!parseLength(word, radius) || radius.value < 0.f) {
                    return false;
                }

                out.values[radii++] = radius;
            }

            if (!circle && radii == 1) {
                return false;
            }

            out.values[2] = Length{50.f, Unit::Percent};
            out.values[3] = Length{50.f, Unit::Percent};

            return true;
        }

        if (equalsIgnoreCase(name, StringView(u8"inset"))) {
            const u8* p = args.begin();
            u32 count = 0;

            out.kind = ShapeKind::Inset;

            while (true) {
                StringView word = shapeWord(p, end);

                if (word.empty() || equalsIgnoreCase(word, StringView(u8"round"))) {
                    break;
                }

                if (count == 4 || !parseLength(word, out.values[count])) {
                    return false;
                }

                count++;
            }

            if (count == 0) {
                return false;
            }

            out.values[1] = count > 1 ? out.values[1] : out.values[0];
            out.values[2] = count > 2 ? out.values[2] : out.values[0];
            out.values[3] = count > 3 ? out.values[3] : out.values[1];

            return true;
        }

        if (equalsIgnoreCase(name, StringView(u8"polygon"))) {
            const u8* p = args.begin();
            Vector<Length> points;

            out.kind = ShapeKind::Polygon;

            while (p < end && isSpace(*p)) {
                p++;
            }

            const u8* comma = p;

            while (comma < end && *comma != ',') {
                comma++;
            }

            StringView first = trim(StringView(p, comma));

            if (equalsIgnoreCase(first, StringView(u8"evenodd")) || equalsIgnoreCase(first, StringView(u8"nonzero"))) {
                out.rule = equalsIgnoreCase(first, StringView(u8"evenodd")) ? WindingEvenOdd : WindingNonZero;
                p = comma < end ? comma + 1 : end;
            }

            while (p < end) {
                const u8* stop = p;

                while (stop < end && *stop != ',') {
                    stop++;
                }

                const u8* q = p;
                StringView xWord = shapeWord(q, stop);
                StringView yWord = shapeWord(q, stop);
                Length x;
                Length y;

                if (!parseLength(xWord, x) || !parseLength(yWord, y) || !trim(StringView(q, stop)).empty()) {
                    return false;
                }

                points.pushBack(x);
                points.pushBack(y);
                p = stop < end ? stop + 1 : end;
            }

            out.pointCount = (u32)points.length() / 2;
            out.points = (Length*)pool.allocate(sizeof(Length) * (points.length() + 1));

            if (points.length()) {
                memcpy(out.points, points.data(), sizeof(Length) * points.length());
            }

            return out.pointCount > 0;
        }

        return false;
    }

    static float shapeLength(const Length& length, float reference, Viewport vp, float fontSize) {
        return length.unit == Unit::Percent ? length.value * reference / 100.f : resolveLength(length, Axis::Other, vp, fontSize);
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
        style.clipShape = nullptr;
        style.nonScaling = false;
        style.blend = 0;
        style.isolate = false;
        style.mask = 0;
        style.maskType = 0;
        style.floodColor = Color{0.f, 0.f, 0.f, 1.f};
        style.floodOpacity = 1.f;
        style.lightingColor = Color{1.f, 1.f, 1.f, 1.f};
        style.filters = nullptr;
        style.filterCount = 0;
        style.background = Color{0.f, 0.f, 0.f, 0.f};
        style.overflowVisible = false;

        for (u32 property = firstProperty; property < firstProperty + propertyCount; property++) {
            StringView value = l.instance && l.instance[node->index] ? (l.instance[node->index][property - firstProperty] ? textOf(doc, l.instance[node->index][property - firstProperty]) : StringView()) : cascaded(doc, node, property);

            if (value.empty()) {
                continue;
            }

            bool inherit = isKeyword(value, "inherit");
            bool reset = isKeyword(value, "initial");

            if (isKeyword(value, "unset")) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty && property != AtomMask && property != AtomMaskType && property != AtomFloodColor && property != AtomFloodOpacity && property != AtomLightingColor && property != AtomFilter && property != AtomBackgroundColor && property != AtomOverflow && property != AtomVectorEffect && property != AtomMixBlendMode && property != AtomIsolation;

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
            StringView value = l.instance && l.instance[node->index] ? (l.instance[node->index][property - firstProperty] ? textOf(doc, l.instance[node->index][property - firstProperty]) : StringView()) : cascaded(doc, node, property);

            if (value.empty() || property == AtomFontSize) {
                continue;
            }

            bool inherit = isKeyword(value, "inherit");
            bool reset = isKeyword(value, "initial");

            if (isKeyword(value, "unset")) {
                bool inherited = property != AtomOpacity && property != AtomDisplay && property != AtomStopColor && property != AtomStopOpacity && property != AtomClipPathProperty && property != AtomMask && property != AtomMaskType && property != AtomFloodColor && property != AtomFloodOpacity && property != AtomLightingColor && property != AtomFilter && property != AtomBackgroundColor && property != AtomOverflow && property != AtomVectorEffect && property != AtomMixBlendMode && property != AtomIsolation;

                inherit = inherited;
                reset = !inherited;
            }

            const ComputedStyle& source = inherit ? parent : initial;
            bool keyword = inherit || reset;

            switch (property) {
                case AtomFill:
                case AtomStroke: {
                    PaintValue& target = property == AtomFill ? style.fill : style.stroke;

                    bool contextFill = isKeyword(value, "context-fill");

                    if (keyword) {
                        target = property == AtomFill ? source.fill : source.stroke;
                    } else if (contextFill || isKeyword(value, "context-stroke")) {
                        target.kind = PaintKind::None;

                        if (l.hasContext) {
                            target = contextFill ? l.contextFill : l.contextStroke;
                            target.context = true;
                        }
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
                case AtomClipPathProperty:
                case AtomMask: {
                    u32& target = property == AtomMask ? style.mask : style.clipPath;

                    if (keyword) {
                        target = property == AtomMask ? source.mask : source.clipPath;

                        if (property != AtomMask) {
                            style.clipShape = source.clipShape;
                        }

                        break;
                    }

                    PaintValue reference;
                    BasicShape shape;

                    if (parsePaint(doc, value, reference) && reference.kind == PaintKind::Server) {
                        target = reference.server;
                    } else if (isKeyword(value, "none")) {
                        target = 0;
                    } else if (property != AtomMask && parseBasicShape(*l.pool, value, shape)) {
                        style.clipShape = l.pool->make<BasicShape>(shape);
                    }

                    break;
                }
                case AtomPaintOrder: {
                    if (keyword) {
                        memcpy(style.paintOrder, source.paintOrder, sizeof(style.paintOrder));
                        break;
                    }

                    u8 order[3];
                    u32 count = 0;
                    bool valid = true;
                    const u8* p = value.begin();

                    if (isKeyword(value, "normal")) {
                        order[0] = 0;
                        order[1] = 1;
                        order[2] = 2;
                        memcpy(style.paintOrder, order, sizeof(order));
                        break;
                    }

                    while (valid) {
                        StringView word = shapeWord(p, value.end());

                        if (word.empty()) {
                            break;
                        }

                        u8 layer = equalsIgnoreCase(word, StringView(u8"fill")) ? 0 : (equalsIgnoreCase(word, StringView(u8"stroke")) ? 1 : (equalsIgnoreCase(word, StringView(u8"markers")) ? 2 : 3));

                        for (u32 k = 0; k < count; k++) {
                            valid = valid && order[k] != layer;
                        }

                        valid = valid && layer < 3 && count < 3;

                        if (valid) {
                            order[count++] = layer;
                        }
                    }

                    if (!valid || count == 0) {
                        break;
                    }

                    for (u8 layer = 0; layer < 3; layer++) {
                        bool present = false;

                        for (u32 k = 0; k < count; k++) {
                            present = present || order[k] == layer;
                        }

                        if (!present) {
                            order[count++] = layer;
                        }
                    }

                    memcpy(style.paintOrder, order, sizeof(order));
                    break;
                }
                case AtomMixBlendMode: {
                    static const char* const modes[] = {"normal", "multiply", "screen", "overlay", "darken", "lighten", "color-dodge", "color-burn", "hard-light", "soft-light", "difference", "exclusion", "hue", "saturation", "color", "luminosity"};

                    if (keyword) {
                        style.blend = source.blend;
                        break;
                    }

                    for (u8 k = 0; k < sizeof(modes) / sizeof(modes[0]); k++) {
                        if (isKeyword(value, modes[k])) {
                            style.blend = k;
                        }
                    }

                    break;
                }
                case AtomColorInterpolation:
                    if (keyword) {
                        style.linearColors = source.linearColors;
                    } else if (isKeyword(value, "linearrgb")) {
                        style.linearColors = true;
                    } else if (isKeyword(value, "srgb") || isKeyword(value, "auto")) {
                        style.linearColors = false;
                    }

                    break;
                case AtomIsolation:
                    if (keyword) {
                        style.isolate = source.isolate;
                    } else if (isKeyword(value, "isolate")) {
                        style.isolate = true;
                    } else if (isKeyword(value, "auto")) {
                        style.isolate = false;
                    }

                    break;
                case AtomVectorEffect:
                    if (keyword) {
                        style.nonScaling = source.nonScaling;
                    } else if (isKeyword(value, "non-scaling-stroke")) {
                        style.nonScaling = true;
                    } else if (isKeyword(value, "none")) {
                        style.nonScaling = false;
                    }

                    break;
                case AtomMarkerStart:
                case AtomMarkerMid:
                case AtomMarkerEnd: {
                    u32& target = style.markers[property - AtomMarkerStart];
                    PaintValue reference;

                    if (keyword) {
                        target = source.markers[property - AtomMarkerStart];
                    } else if (isKeyword(value, "none")) {
                        target = 0;
                    } else if (parsePaint(doc, value, reference) && reference.kind == PaintKind::Server) {
                        target = reference.server;
                    }

                    break;
                }
                case AtomFloodColor:
                case AtomLightingColor: {
                    Color& target = property == AtomFloodColor ? style.floodColor : style.lightingColor;

                    if (keyword) {
                        target = property == AtomFloodColor ? source.floodColor : source.lightingColor;
                    } else if (isKeyword(value, "currentcolor")) {
                        target = style.color;
                    } else {
                        Color color;

                        if (parseColor(value, color)) {
                            target = color;
                        }
                    }

                    break;
                }
                case AtomFloodOpacity:
                    if (keyword) {
                        style.floodOpacity = source.floodOpacity;
                    } else {
                        parseOpacity(value, style.floodOpacity);
                    }

                    break;
                case AtomColorInterpolationFilters:
                    if (keyword) {
                        style.linearFilters = source.linearFilters;
                    } else if (isKeyword(value, "linearrgb")) {
                        style.linearFilters = true;
                    } else if (isKeyword(value, "srgb") || isKeyword(value, "auto")) {
                        style.linearFilters = false;
                    }

                    break;
                case AtomFilter: {
                    if (keyword) {
                        style.filters = source.filters;
                        style.filterCount = source.filterCount;
                        break;
                    }

                    if (isKeyword(value, "none")) {
                        style.filters = nullptr;
                        style.filterCount = 0;
                        break;
                    }

                    Vector<FilterItem> items;

                    if (parseFilterList(doc, value, style, vp, items)) {
                        style.filterCount = (u32)items.length();
                        style.filters = (FilterItem*)l.pool->allocate(sizeof(FilterItem) * items.length());
                        memcpy(style.filters, items.data(), sizeof(FilterItem) * items.length());
                    }

                    break;
                }
                case AtomBackgroundColor:
                    if (keyword) {
                        style.background = source.background;
                    } else if (isKeyword(value, "currentcolor")) {
                        style.background = style.color;
                    } else {
                        Color color;

                        if (parseColor(value, color)) {
                            style.background = color;
                        }
                    }

                    break;
                case AtomOverflow:
                    if (keyword) {
                        style.overflowVisible = source.overflowVisible;
                    } else {
                        style.overflowVisible = isKeyword(value, "visible") || isKeyword(value, "auto");
                    }

                    break;
                case AtomMaskType:
                    if (keyword) {
                        style.maskType = source.maskType;
                    } else if (isKeyword(value, "alpha")) {
                        style.maskType = 1;
                    } else if (isKeyword(value, "luminance")) {
                        style.maskType = 0;
                    }

                    break;
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

    static bool passesConditions(const Document& doc, const Node* node);

    static bool resourceUsable(Lowering& l, Node* node, Viewport vp) {
        for (Node* up = node; up && up != l.doc->root; up = up->parent) {
            if (documentStyle(l, up, vp).displayNone || (up != node && !passesConditions(*l.doc, up))) {
                return false;
            }
        }

        return true;
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
            l.out->pushBack(op);
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

    static u32 patternAttribute(const Document& doc, Node* pattern, u32 name, Node** owner) {
        Node* node = pattern;

        for (u32 hops = 0; node && hops < 16; hops++) {
            if (u32 value = attribute(node, name); value && !trim(textOf(doc, value)).empty()) {
                if (owner) {
                    *owner = node;
                }

                return value;
            }

            Node* next = hrefTarget(doc, node);

            if (!next || next->name != AtomPattern || next == node) {
                break;
            }

            node = next;
        }

        return 0;
    }

    static bool renderable(u32 name);

    static Node* patternContent(const Document& doc, Node* pattern) {
        Node* node = pattern;

        for (u32 hops = 0; node && hops < 16; hops++) {
            for (Node* child = node->first; child; child = child->next) {
                if (renderable(child->name)) {
                    return node;
                }
            }

            Node* next = hrefTarget(doc, node);

            if (!next || next->name != AtomPattern || next == node) {
                break;
            }

            node = next;
        }

        return nullptr;
    }

    static float patternLength(Lowering& l, Node* pattern, u32 name, bool boxUnits, Axis axis, Viewport vp, float fallback) {
        u32 value = patternAttribute(*l.doc, pattern, name, nullptr);
        Length length;

        if (!value || !parseLength(textOf(*l.doc, value), length)) {
            return fallback;
        }

        if (boxUnits) {
            return length.unit == Unit::Percent ? length.value / 100.f : length.value;
        }

        return resolveLength(length, axis, vp, 16.f);
    }

    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp);

    static DevicePaint* patternPaint(Lowering& l, Node* pattern, float opacity, Box bbox, const Matrix& ctm, Viewport vp) {
        const Document& doc = *l.doc;

        for (Node* active : l.active) {
            if (active == pattern) {
                return nullptr;
            }
        }

        u32 units = patternAttribute(doc, pattern, AtomPatternUnits, nullptr);
        u32 contentUnits = patternAttribute(doc, pattern, AtomPatternContentUnits, nullptr);
        bool boxUnits = !units || textOf(doc, units) != StringView(u8"userSpaceOnUse");
        bool boxContent = contentUnits && textOf(doc, contentUnits) == StringView(u8"objectBoundingBox");
        float bw = bbox.x1 - bbox.x0;
        float bh = bbox.y1 - bbox.y0;

        if ((boxUnits || boxContent) && (!boxValid(bbox) || bw <= 0.f || bh <= 0.f)) {
            return nullptr;
        }

        float x = patternLength(l, pattern, AtomX, boxUnits, Axis::X, vp, 0.f);
        float y = patternLength(l, pattern, AtomY, boxUnits, Axis::Y, vp, 0.f);
        float w = patternLength(l, pattern, AtomWidth, boxUnits, Axis::X, vp, 0.f);
        float h = patternLength(l, pattern, AtomHeight, boxUnits, Axis::Y, vp, 0.f);

        if (boxUnits) {
            x = bbox.x0 + x * bw;
            y = bbox.y0 + y * bh;
            w *= bw;
            h *= bh;
        }

        Node* content = patternContent(doc, pattern);

        if (w <= 0.f || h <= 0.f || !content) {
            return nullptr;
        }

        Matrix space = ctm;
        u32 transform = patternAttribute(doc, pattern, AtomPatternTransform, nullptr);
        Matrix local;

        if (transform && parseTransform(textOf(doc, transform), local)) {
            if (u32 origin = attribute(pattern, AtomTransformOrigin)) {
                Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

                local = multiply(multiply(translation(o.x, o.y), local), translation(-o.x, -o.y));
            }

            space = multiply(space, local);
        }

        space = multiply(space, translation(x, y));

        Matrix inner = identity();
        Node* viewBoxOwner = nullptr;
        u32 viewBoxValue = patternAttribute(doc, pattern, AtomViewBox, &viewBoxOwner);
        Box viewBox;

        if (viewBoxValue && parseViewBox(doc, viewBoxOwner, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return nullptr;
            }

            Node* ratioOwner = pattern;

            patternAttribute(doc, pattern, AtomPreserveAspectRatio, &ratioOwner);
            inner = viewBoxTransform(doc, ratioOwner, viewBox, w, h);
        } else if (boxContent) {
            inner = scaling(bw, bh);
        }

        float sx = sqrtf(space.a * space.a + space.b * space.b);
        float sy = sqrtf(space.c * space.c + space.d * space.d);
        float tw = ceilf(w * sx);
        float th = ceilf(h * sy);

        if (!(tw >= 1.f) || !(th >= 1.f)) {
            tw = maxf(tw, 1.f);
            th = maxf(th, 1.f);
        }

        tw = minf(tw, 2048.f);
        th = minf(th, 2048.f);

        DevicePaint* paint = l.pool->make<DevicePaint>();

        memset(paint, 0, sizeof(DevicePaint));
        paint->type = PaintType::Pattern;
        paint->imageWidth = (u32)tw;
        paint->imageHeight = (u32)th;
        paint->opacity = opacity;

        if (!invert(multiply(space, scaling(w / tw, h / th)), paint->inverse)) {
            return nullptr;
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;
        const ComputedStyle& contentStyle = documentStyle(l, content, vp);

        l.out = &program;
        l.active.pushBack(pattern);
        lowerChildren(l, content, contentStyle, multiply(scaling(tw / w, th / h), inner), vp);
        l.active.popBack();
        l.out = saved;
        paint->programFirst = (u32)l.programs.length();
        paint->programCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return paint;
    }

    static u32 gradientAttribute(const Document& doc, Node* gradient, u32 name) {
        Node* node = gradient;
        bool specific = name == AtomX1 || name == AtomY1 || name == AtomX2 || name == AtomY2 || name == AtomCx || name == AtomCy || name == AtomR || name == AtomFx || name == AtomFy || name == AtomFr;

        for (u32 hops = 0; node && hops < 16; hops++) {
            if (u32 value = attribute(node, name); value && !trim(textOf(doc, value)).empty() && (!specific || node->name == gradient->name)) {
                return value;
            }

            Node* next = hrefTarget(doc, node);

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

            Node* next = hrefTarget(doc, node);

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
                            p++;
                        }

                        if (p != text.end()) {
                            offset = 0.f;
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
        Matrix gradientMatrix = identity();
        u32 transform = gradientAttribute(doc, gradient, AtomGradientTransform);

        if (transform) {
            Matrix m;

            if (parseTransform(textOf(doc, transform), m)) {
                if (u32 origin = attribute(gradient, AtomTransformOrigin)) {
                    Point o = transformOrigin(textOf(doc, origin), vp.width, vp.height);

                    m = multiply(multiply(translation(o.x, o.y), m), translation(-o.x, -o.y));
                }

                gradientMatrix = m;
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

        space = multiply(space, gradientMatrix);

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
            paint->fr = maxf(gradientLength(l, gradient, AtomFr, boxUnits, Axis::Other, vp, 0.f), 0.f);

            if (paint->r <= 0.f) {
                return solidPaint(l, Color{last.r, last.g, last.b, 1.f}, last.a);
            }

            if (fabsf(paint->fr - paint->r) < 1e-6f * maxf(1.f, paint->r) && paint->fx == paint->cx && paint->fy == paint->cy && paint->spread == SpreadPad) {
                Color first = stops[0].color;
                GradientStop* ring = (GradientStop*)l.pool->allocate(sizeof(GradientStop) * 3);

                ring[0] = GradientStop{0.f, first};
                ring[1] = GradientStop{1.f, first};
                ring[2] = GradientStop{1.f, last};
                paint->stops = ring;
                paint->stopCount = 3;
                paint->fr = 0.f;
            }
        }

        return paint;
    }

    static bool degenerateBox(const Document& doc, Node* gradient, const Box& bbox) {
        u32 units = gradientAttribute(doc, gradient, AtomGradientUnits);
        bool boxUnits = !units || textOf(doc, units) != StringView(u8"userSpaceOnUse");

        return boxUnits && (!boxValid(bbox) || bbox.x1 - bbox.x0 <= 0.f || bbox.y1 - bbox.y0 <= 0.f);
    }

    static bool singularTransform(const Document& doc, Node* server) {
        u32 value = server->name == AtomPattern ? patternAttribute(doc, server, AtomPatternTransform, nullptr) : gradientAttribute(doc, server, AtomGradientTransform);
        Matrix m;
        Matrix inverse;

        return value && parseTransform(textOf(doc, value), m) && !invert(m, inverse);
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

                bool gradient = server && (server->name == AtomLinearGradient || server->name == AtomRadialGradient);
                bool pattern = server && server->name == AtomPattern;

                if ((gradient || pattern) && resourceUsable(l, server, vp) && !singularTransform(*l.doc, server)) {
                    DevicePaint* paint = gradient ? gradientPaint(l, server, opacity, bbox, ctm, vp) : patternPaint(l, server, opacity, bbox, ctm, vp);

                    if (paint || (gradient && !degenerateBox(*l.doc, server, bbox))) {
                        return paint;
                    }
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
            case AtomImage:
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

        if (node->name == AtomImage) {
            float fs = style.fontSize;
            float x = lengthOr(*l.doc, node, AtomX, Axis::X, vp, fs, 0.f);
            float y = lengthOr(*l.doc, node, AtomY, Axis::Y, vp, fs, 0.f);
            float w = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, 0.f);
            float h = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, 0.f);

            out = Box{x, y, x + w, y + h};

            return w > 0.f && h > 0.f;
        }

        Box box = emptyBox();
        auto add = [&](Node* child, const Matrix& m) {
            Box inner;

            if (elementBox(l, child, style, vp, inner, depth + 1)) {
                Matrix full = multiply(m, originTransform(*l.doc, child, AtomTransform, vp));

                if (child->name == AtomUse) {
                    full = multiply(full, translation(lengthOr(*l.doc, child, AtomX, Axis::X, vp, style.fontSize, 0.f), lengthOr(*l.doc, child, AtomY, Axis::Y, vp, style.fontSize, 0.f)));
                }
                Point corners[4] = {{inner.x0, inner.y0}, {inner.x1, inner.y0}, {inner.x1, inner.y1}, {inner.x0, inner.y1}};

                for (Point corner : corners) {
                    extend(box, apply(full, corner));
                }
            }
        };

        if (node->name == AtomUse) {
            Node* target = hrefTarget(*l.doc, node);

            if (target && target != node) {
                add(target, identity());
            }
        } else {
            for (Node* child = node->first; child; child = child->next) {
                add(child, identity());
            }
        }

        out = box;

        return boxValid(box);
    }

    static Op groupOp(float opacity) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Begin;
        op.opacity = opacity;

        return op;
    }

    static void endGroup(Lowering& l) {
        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::End;
        l.out->pushBack(op);
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

    struct ClipChild {
        Node* shape;
        Matrix space;
        ComputedStyle style;
    };

    static bool clipChild(Lowering& l, Node* child, const Matrix& space, Viewport vp, ClipChild& out) {
        const Document& doc = *l.doc;

        out.shape = child;
        out.space = multiply(space, transformOf(doc, child, AtomTransform));

        if (child->name == AtomUse) {
            out.shape = hrefTarget(doc, child);

            if (!out.shape || !isShape(out.shape->name)) {
                return false;
            }

            const ComputedStyle& useStyle = documentStyle(l, child, vp);

            if (useStyle.displayNone) {
                return false;
            }

            out.space = multiply(out.space, translation(lengthOr(doc, child, AtomX, Axis::X, vp, useStyle.fontSize, 0.f), lengthOr(doc, child, AtomY, Axis::Y, vp, useStyle.fontSize, 0.f)));
            out.space = multiply(out.space, transformOf(doc, out.shape, AtomTransform));
            out.style = computeStyle(l, out.shape, useStyle, vp);
        } else if (isShape(child->name)) {
            out.style = documentStyle(l, child, vp);
        } else {
            return false;
        }

        return !out.style.displayNone && out.style.visible;
    }

    static bool clipNeedsProgram(Lowering& l, Node* clip, Viewport vp) {
        if (documentStyle(l, clip, vp).clipPath) {
            return true;
        }

        for (Node* child = clip->first; child; child = child->next) {
            if ((child->name == AtomUse || isShape(child->name)) && documentStyle(l, child, vp).clipPath) {
                return true;
            }
        }

        return false;
    }

    static bool resolveClip(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const Document& doc = *l.doc;
        Node* clip = elementById(doc, style.clipPath);

        if (!clip || clip->name != AtomClipPath || !resourceUsable(l, clip, vp)) {
            return false;
        }

        for (Node* active : l.active) {
            if (active == clip) {
                return false;
            }
        }

        Matrix space = multiply(m, originTransform(doc, clip, AtomTransform, vp));
        u32 units = attribute(clip, AtomClipPathUnits);

        op.clipProgram = false;
        op.clipFirst = (u32)l.clips.length();
        op.clipCount = 0;

        if (units && textOf(doc, units) == StringView(u8"objectBoundingBox")) {
            Box box;

            if (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0) {
                return true;
            }

            space = multiply(space, Matrix{box.x1 - box.x0, 0.f, 0.f, box.y1 - box.y0, box.x0, box.y0});
        }

        if (!clipNeedsProgram(l, clip, vp)) {
            for (Node* child = clip->first; child; child = child->next) {
                ClipChild item;
                Path path;

                if (clipChild(l, child, space, vp, item) && buildShape(doc, item.shape, item.style, vp, path)) {
                    addClipShape(l, path, item.space, item.style.clipRule);
                }
            }

            op.clipCount = (u32)l.clips.length() - op.clipFirst;

            return true;
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;
        const ComputedStyle& clipStyle = documentStyle(l, clip, vp);
        Op outer = groupOp(1.f);

        l.active.pushBack(clip);
        l.out = &program;
        outer.clipped = clipStyle.clipPath && resolveClip(l, element, parent, clipStyle, m, vp, outer);

        if (outer.clipped) {
            program.pushBack(outer);
        }

        for (Node* child = clip->first; child; child = child->next) {
            ClipChild item;
            Path path;

            if (!clipChild(l, child, space, vp, item) || !buildShape(doc, item.shape, item.style, vp, path)) {
                continue;
            }

            const ComputedStyle& childStyle = documentStyle(l, child, vp);
            Op inner = groupOp(1.f);

            inner.clipped = childStyle.clipPath && resolveClip(l, child, clipStyle, childStyle, multiply(space, transformOf(doc, child, AtomTransform)), vp, inner);

            if (inner.clipped) {
                program.pushBack(inner);
            }

            Flat flat;

            flatten(path, item.space, tolerance, flat);
            emitFill(l, flat, item.style.clipRule, solidPaint(l, Color{1.f, 1.f, 1.f, 1.f}, 1.f));

            if (inner.clipped) {
                endGroup(l);
            }
        }

        if (outer.clipped) {
            endGroup(l);
        }

        l.out = saved;
        l.active.popBack();
        op.clipProgram = true;
        op.clipFirst = (u32)l.programs.length();
        op.clipCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return true;
    }

    static bool shapeClip(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const BasicShape& shape = *style.clipShape;
        Box box{0.f, 0.f, vp.width, vp.height};

        op.clipProgram = false;
        op.clipFirst = (u32)l.clips.length();
        op.clipCount = 0;

        if (shape.box != GeometryBox::View) {
            if (!elementBox(l, element, parent, vp, box, 0)) {
                return true;
            }

            if (shape.box == GeometryBox::Stroke && style.stroke.kind != PaintKind::None && isShape(element->name)) {
                float half = style.strokeWidth / 2.f;

                box = Box{box.x0 - half, box.y0 - half, box.x1 + half, box.y1 + half};
            }
        }

        float w = box.x1 - box.x0;
        float h = box.y1 - box.y0;
        float fs = style.fontSize;
        Path path;

        if (shape.kind == ShapeKind::Circle || shape.kind == ShapeKind::Ellipse) {
            float cx = box.x0 + shapeLength(shape.values[2], w, vp, fs);
            float cy = box.y0 + shapeLength(shape.values[3], h, vp, fs);
            float sides[2][2] = {{cx - box.x0, box.x1 - cx}, {cy - box.y0, box.y1 - cy}};
            float radii[2];

            for (int axis = 0; axis < 2; axis++) {
                const Length& r = shape.values[shape.kind == ShapeKind::Circle ? 0 : axis];
                float reference = shape.kind == ShapeKind::Circle ? sqrtf((w * w + h * h) / 2.f) : (axis ? h : w);

                if (r.unit == Unit::Number && r.value < 0.f) {
                    bool farthest = r.value < -1.5f;

                    if (shape.kind == ShapeKind::Circle) {
                        float a = minf(fabsf(sides[0][0]), fabsf(sides[0][1]));
                        float b = minf(fabsf(sides[1][0]), fabsf(sides[1][1]));
                        float c = maxf(fabsf(sides[0][0]), fabsf(sides[0][1]));
                        float d = maxf(fabsf(sides[1][0]), fabsf(sides[1][1]));

                        radii[axis] = farthest ? maxf(c, d) : minf(a, b);
                    } else {
                        radii[axis] = farthest ? maxf(fabsf(sides[axis][0]), fabsf(sides[axis][1])) : minf(fabsf(sides[axis][0]), fabsf(sides[axis][1]));
                    }
                } else {
                    radii[axis] = shapeLength(r, reference, vp, fs);
                }
            }

            ellipsePath(path, cx, cy, radii[0], radii[1]);
        } else if (shape.kind == ShapeKind::Inset) {
            float top = shapeLength(shape.values[0], h, vp, fs);
            float right = shapeLength(shape.values[1], w, vp, fs);
            float bottom = shapeLength(shape.values[2], h, vp, fs);
            float left = shapeLength(shape.values[3], w, vp, fs);
            float x0 = box.x0 + left;
            float y0 = box.y0 + top;
            float x1 = maxf(x0, box.x1 - right);
            float y1 = maxf(y0, box.y1 - bottom);

            moveTo(path, Point{x0, y0});
            lineTo(path, Point{x1, y0});
            lineTo(path, Point{x1, y1});
            lineTo(path, Point{x0, y1});
            closePath(path);
        } else {
            for (u32 i = 0; i < shape.pointCount; i++) {
                Point point{box.x0 + shapeLength(shape.points[i * 2], w, vp, fs), box.y0 + shapeLength(shape.points[i * 2 + 1], h, vp, fs)};

                if (i == 0) {
                    moveTo(path, point);
                } else {
                    lineTo(path, point);
                }
            }

            closePath(path);
        }

        addClipShape(l, path, m, shape.rule);
        op.clipCount = (u32)l.clips.length() - op.clipFirst;

        return true;
    }

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp);
    static bool loadDocument(Document& doc, ObjPool& pool, StringView source);
    static void cascade(Document& doc);
    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp);

    static float unitFraction(const Document& doc, const Node* node, u32 name, float fallback) {
        u32 value = attribute(node, name);
        Length length;

        if (!value || !parseLength(textOf(doc, value), length)) {
            return fallback;
        }

        return length.unit == Unit::Percent ? length.value / 100.f : length.value;
    }

    static bool resolveMask(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp, Op& op) {
        const Document& doc = *l.doc;
        Node* mask = elementById(doc, style.mask);

        if (!mask || mask->name != AtomMask || !resourceUsable(l, mask, vp)) {
            return false;
        }

        for (Node* active : l.active) {
            if (active == mask) {
                return false;
            }
        }

        u32 units = attribute(mask, AtomMaskUnits);
        u32 contentUnits = attribute(mask, AtomMaskContentUnits);
        bool boxRegion = !units || textOf(doc, units) != StringView(u8"userSpaceOnUse");
        bool boxContent = contentUnits && textOf(doc, contentUnits) == StringView(u8"objectBoundingBox");
        const ComputedStyle& maskStyle = documentStyle(l, mask, vp);
        Box box = emptyBox();
        float x;
        float y;
        float w;
        float h;

        op.masked = true;
        op.maskType = (u8)(maskStyle.maskType | (maskStyle.linearColors ? 2 : 0));
        op.maskFirst = (u32)l.programs.length();
        op.maskCount = 0;
        op.maskRegion = (u32)l.clips.length();

        if ((boxRegion || boxContent) && (!elementBox(l, element, parent, vp, box, 0) || box.x1 <= box.x0 || box.y1 <= box.y0)) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return true;
        }

        if (boxRegion) {
            float bw = box.x1 - box.x0;
            float bh = box.y1 - box.y0;

            x = box.x0 + unitFraction(doc, mask, AtomX, -0.1f) * bw;
            y = box.y0 + unitFraction(doc, mask, AtomY, -0.1f) * bh;
            w = unitFraction(doc, mask, AtomWidth, 1.2f) * bw;
            h = unitFraction(doc, mask, AtomHeight, 1.2f) * bh;
        } else {
            float fs = maskStyle.fontSize;

            x = lengthOr(doc, mask, AtomX, Axis::X, vp, fs, -0.1f * vp.width);
            y = lengthOr(doc, mask, AtomY, Axis::Y, vp, fs, -0.1f * vp.height);
            w = lengthOr(doc, mask, AtomWidth, Axis::X, vp, fs, 1.2f * vp.width);
            h = lengthOr(doc, mask, AtomHeight, Axis::Y, vp, fs, 1.2f * vp.height);
        }

        if (w <= 0.f || h <= 0.f) {
            addRectClip(l, m, 0.f, 0.f, 0.f, 0.f);

            return true;
        }

        addRectClip(l, m, x, y, w, h);

        Matrix content = m;

        if (boxContent) {
            content = multiply(content, Matrix{box.x1 - box.x0, 0.f, 0.f, box.y1 - box.y0, box.x0, box.y0});
        }

        Vector<Op> program;
        Vector<Op>* saved = l.out;

        l.out = &program;
        l.active.pushBack(mask);
        lowerChildren(l, mask, maskStyle, content, vp);
        l.active.popBack();
        l.out = saved;
        op.maskFirst = (u32)l.programs.length();
        op.maskCount = (u32)program.length();
        l.programs.append(program.data(), program.length());

        return true;
    }

    static bool filterPrimitive(u32 name) {
        switch (name) {
            case AtomFeBlend:
            case AtomFeColorMatrix:
            case AtomFeComponentTransfer:
            case AtomFeComposite:
            case AtomFeConvolveMatrix:
            case AtomFeDiffuseLighting:
            case AtomFeDisplacementMap:
            case AtomFeDropShadow:
            case AtomFeFlood:
            case AtomFeGaussianBlur:
            case AtomFeImage:
            case AtomFeMerge:
            case AtomFeMorphology:
            case AtomFeOffset:
            case AtomFeSpecularLighting:
            case AtomFeTile:
            case AtomFeTurbulence:
                return true;
            default:
                return false;
        }
    }

    static float filterLength(const Document& doc, u32 value, bool boxUnits, Axis axis, Viewport vp, float fallback) {
        Length length;

        if (!value || !parseLength(textOf(doc, value), length)) {
            return fallback;
        }

        if (boxUnits) {
            return length.unit == Unit::Percent ? length.value / 100.f : length.value;
        }

        return resolveLength(length, axis, vp, 16.f);
    }

    static Box deviceBox(const Matrix& m, const Box& box) {
        Box out = emptyBox();
        Point corners[4] = {{box.x0, box.y0}, {box.x1, box.y0}, {box.x1, box.y1}, {box.x0, box.y1}};

        for (Point corner : corners) {
            extend(out, apply(m, corner));
        }

        return out;
    }

    static IntBox pixelBox(const Box& box, const IntBox& limit) {
        IntBox out;
        float x0 = floorf(box.x0 + 1e-4f);
        float y0 = floorf(box.y0 + 1e-4f);
        float x1 = ceilf(box.x1 - 1e-4f);
        float y1 = ceilf(box.y1 - 1e-4f);

        out.x0 = x0 < (float)limit.x0 ? limit.x0 : (x0 > (float)limit.x1 ? limit.x1 : (int)x0);
        out.y0 = y0 < (float)limit.y0 ? limit.y0 : (y0 > (float)limit.y1 ? limit.y1 : (int)y0);
        out.x1 = x1 > (float)limit.x1 ? limit.x1 : (x1 < (float)out.x0 ? out.x0 : (int)x1);
        out.y1 = y1 > (float)limit.y1 ? limit.y1 : (y1 < (float)out.y0 ? out.y0 : (int)y1);

        return out;
    }

    static int filterInput(const Document& doc, const Node* node, u32 name, const Vector<FilterPrimitive>& done, int fallback) {
        u32 value = attribute(node, name);

        if (!value) {
            return fallback;
        }

        StringView text = trim(textOf(doc, value));

        if (text == StringView(u8"SourceGraphic")) {
            return InputSourceGraphic;
        }

        if (text == StringView(u8"SourceAlpha")) {
            return InputSourceAlpha;
        }

        for (size_t i = done.length(); i > 0; i--) {
            u32 result = attribute(done[i - 1].node, AtomResult);

            if (result && trim(textOf(doc, result)) == text) {
                return (int)(i - 1);
            }
        }

        return fallback;
    }

    static void lowerImageContent(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp, float x, float y, float w, float h);

    static Filter* lowerFilter(Lowering& l, Node* element, const ComputedStyle& parent, const Matrix& m, Viewport vp, u32 reference) {
        const Document& doc = *l.doc;
        Node* filterNode = elementById(doc, reference);

        if (!filterNode || filterNode->name != AtomFilter || !resourceUsable(l, filterNode, vp)) {
            return nullptr;
        }

        for (Node* active : l.active) {
            if (active == filterNode) {
                return nullptr;
            }
        }

        Filter* filter = l.pool->make<Filter>();
        u32 units = attribute(filterNode, AtomFilterUnits);
        u32 primitiveUnits = attribute(filterNode, AtomPrimitiveUnits);
        bool boxRegion = !units || textOf(doc, units) != StringView(u8"userSpaceOnUse");
        IntBox canvas{0, 0, (int)l.width, (int)l.height};
        Box bbox;

        filter->doc = l.doc;
        filter->ctm = m;
        filter->boxPrimitives = primitiveUnits && textOf(doc, primitiveUnits) == StringView(u8"objectBoundingBox");
        filter->region = IntBox{0, 0, 0, 0};
        filter->count = 0;
        filter->primitives = nullptr;
        filter->next = nullptr;

        bool hasBox = elementBox(l, element, parent, vp, bbox, 0) && bbox.x1 > bbox.x0 && bbox.y1 > bbox.y0;

        filter->bbox = hasBox ? bbox : Box{0.f, 0.f, 0.f, 0.f};

        if ((boxRegion || filter->boxPrimitives) && !hasBox) {
            return filter;
        }

        Box region;
        float bw = bbox.x1 - bbox.x0;
        float bh = bbox.y1 - bbox.y0;

        if (boxRegion) {
            region.x0 = bbox.x0 + filterLength(doc, attribute(filterNode, AtomX), true, Axis::X, vp, -0.1f) * bw;
            region.y0 = bbox.y0 + filterLength(doc, attribute(filterNode, AtomY), true, Axis::Y, vp, -0.1f) * bh;
            region.x1 = region.x0 + filterLength(doc, attribute(filterNode, AtomWidth), true, Axis::X, vp, 1.2f) * bw;
            region.y1 = region.y0 + filterLength(doc, attribute(filterNode, AtomHeight), true, Axis::Y, vp, 1.2f) * bh;
        } else {
            region.x0 = filterLength(doc, attribute(filterNode, AtomX), false, Axis::X, vp, -0.1f * vp.width);
            region.y0 = filterLength(doc, attribute(filterNode, AtomY), false, Axis::Y, vp, -0.1f * vp.height);
            region.x1 = region.x0 + filterLength(doc, attribute(filterNode, AtomWidth), false, Axis::X, vp, 1.2f * vp.width);
            region.y1 = region.y0 + filterLength(doc, attribute(filterNode, AtomHeight), false, Axis::Y, vp, 1.2f * vp.height);
        }

        if (region.x1 <= region.x0 || region.y1 <= region.y0) {
            return filter;
        }

        filter->region = pixelBox(deviceBox(m, region), canvas);

        Node* content = filterNode;
        Vector<FilterPrimitive> primitives;
        Vector<Box> userBoxes;

        l.active.pushBack(filterNode);

        for (Node* child = content->first; child; child = child->next) {
            if (!filterPrimitive(child->name)) {
                continue;
            }

            FilterPrimitive primitive;
            const ComputedStyle& childStyle = documentStyle(l, child, vp);
            int previous = primitives.length() ? (int)primitives.length() - 1 : InputSourceGraphic;

            memset(&primitive, 0, sizeof(primitive));
            primitive.node = child;
            primitive.kind = child->name;
            primitive.linear = childStyle.linearFilters;
            primitive.flood = childStyle.floodColor;
            primitive.flood.a *= childStyle.floodOpacity;
            primitive.lighting = childStyle.lightingColor;
            primitive.in1 = filterInput(doc, child, AtomIn, primitives, previous);
            primitive.in2 = filterInput(doc, child, AtomIn2, primitives, previous);

            Vector<int> inputs;

            if (child->name == AtomFeMerge) {
                for (Node* node = child->first; node; node = node->next) {
                    if (node->name == AtomFeMergeNode) {
                        inputs.pushBack(filterInput(doc, node, AtomIn, primitives, previous));
                    }
                }

                primitive.inputCount = (u32)inputs.length();
                primitive.inputs = (int*)l.pool->allocate(sizeof(int) * (inputs.length() + 1));

                if (inputs.length()) {
                    memcpy(primitive.inputs, inputs.data(), sizeof(int) * inputs.length());
                }
            } else if (child->name != AtomFeFlood && child->name != AtomFeImage && child->name != AtomFeTurbulence) {
                inputs.pushBack(primitive.in1);

                if (child->name == AtomFeBlend || child->name == AtomFeComposite || child->name == AtomFeDisplacementMap) {
                    inputs.pushBack(primitive.in2);
                }
            }

            Box user = region;
            bool standard = inputs.length() == 0 || child->name == AtomFeTile;

            for (int input : inputs) {
                if (input < 0) {
                    standard = true;
                }
            }

            if (!standard) {
                user = emptyBox();

                for (int input : inputs) {
                    const Box& b = userBoxes[input];

                    user.x0 = minf(user.x0, b.x0);
                    user.y0 = minf(user.y0, b.y0);
                    user.x1 = maxf(user.x1, b.x1);
                    user.y1 = maxf(user.y1, b.y1);
                }
            }

            u32 xValue = attribute(child, AtomX);
            u32 yValue = attribute(child, AtomY);
            u32 wValue = attribute(child, AtomWidth);
            u32 hValue = attribute(child, AtomHeight);
            bool boxPrimitives = filter->boxPrimitives;

            if (xValue) {
                float x = filterLength(doc, xValue, boxPrimitives, Axis::X, vp, 0.f);
                float width = user.x1 - user.x0;

                user.x0 = boxPrimitives ? bbox.x0 + x * bw : x;
                user.x1 = user.x0 + width;
            }

            if (yValue) {
                float y = filterLength(doc, yValue, boxPrimitives, Axis::Y, vp, 0.f);
                float height = user.y1 - user.y0;

                user.y0 = boxPrimitives ? bbox.y0 + y * bh : y;
                user.y1 = user.y0 + height;
            }

            if (wValue) {
                float width = filterLength(doc, wValue, boxPrimitives, Axis::X, vp, 0.f);

                user.x1 = user.x0 + (boxPrimitives ? width * bw : width);
            }

            if (hValue) {
                float height = filterLength(doc, hValue, boxPrimitives, Axis::Y, vp, 0.f);

                user.y1 = user.y0 + (boxPrimitives ? height * bh : height);
            }

            primitive.user = user;
            primitive.box = user.x1 > user.x0 && user.y1 > user.y0 ? pixelBox(deviceBox(m, user), filter->region) : IntBox{0, 0, 0, 0};

            if (child->name == AtomFeImage) {
                Vector<Op> program;
                Vector<Op>* saved = l.out;

                l.out = &program;
                lowerImageContent(l, child, childStyle, m, vp, user.x0, user.y0, user.x1 - user.x0, user.y1 - user.y0);
                l.out = saved;
                primitive.programFirst = (u32)l.programs.length();
                primitive.programCount = (u32)program.length();
                l.programs.append(program.data(), program.length());
            }

            primitives.pushBack(primitive);
            userBoxes.pushBack(user);
        }

        l.active.popBack();
        filter->count = (u32)primitives.length();
        filter->primitives = (FilterPrimitive*)l.pool->allocate(sizeof(FilterPrimitive) * (primitives.length() + 1));

        if (filter->count) {
            memcpy(filter->primitives, primitives.data(), sizeof(FilterPrimitive) * primitives.length());
        }

        return filter;
    }

    static Filter* functionFilter(Lowering& l, const FilterItem& item, const Matrix& m, bool linear) {
        Filter* filter = l.pool->make<Filter>();
        FilterPrimitive* primitive = (FilterPrimitive*)l.pool->allocate(sizeof(FilterPrimitive));

        memset(primitive, 0, sizeof(FilterPrimitive));
        primitive->kind = item.kind == FilterFunction::Blur ? AtomFeGaussianBlur : (item.kind == FilterFunction::DropShadow ? AtomFeDropShadow : AtomFeColorMatrix);
        primitive->in1 = InputSourceGraphic;
        primitive->in2 = InputSourceGraphic;
        primitive->box = IntBox{0, 0, (int)l.width, (int)l.height};
        primitive->flood = item.color;
        primitive->linear = linear;
        memcpy(primitive->params, item.values, sizeof(item.values));

        filter->doc = l.doc;
        filter->ctm = m;
        filter->bbox = Box{0.f, 0.f, 0.f, 0.f};
        filter->boxPrimitives = false;
        filter->region = primitive->box;
        filter->primitives = primitive;
        filter->count = 1;
        filter->next = nullptr;

        return filter;
    }

    static Filter* lowerFilters(Lowering& l, Node* element, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Filter* first = nullptr;
        Filter* last = nullptr;

        for (u32 i = 0; i < style.filterCount; i++) {
            const FilterItem& item = style.filters[i];
            bool linear = last && last->count && last->primitives[last->count - 1].linear;
            Filter* filter = item.kind == FilterFunction::Reference ? lowerFilter(l, element, parent, m, vp, item.reference) : functionFilter(l, item, m, linear);

            if (!filter) {
                continue;
            }

            if (last) {
                last->next = filter;
            } else {
                first = filter;
            }

            last = filter;
        }

        return first;
    }

    static void lowerChildren(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& ctm, Viewport vp) {
        for (Node* child = node->first; child; child = child->next) {
            lowerElement(l, child, style, ctm, vp);
        }
    }

    static u32** instanceStyles(Lowering& l, Node* target) {
        for (size_t i = 0; i < l.instanceTargets.length(); i++) {
            if (l.instanceTargets[i] == target) {
                return l.instanceMaps[i];
            }
        }

        Document& doc = *l.doc;
        u32** map = (u32**)l.pool->allocate(sizeof(u32*) * (doc.nodeCount + 1));

        memset(map, 0, sizeof(u32*) * (doc.nodeCount + 1));

        for (Node* node = target; node; node = nextNode(node, target)) {
            if (node->index < doc.nodeCount) {
                map[node->index] = cascadeNode(doc, node, target);
            }
        }

        l.instanceTargets.pushBack(target);
        l.instanceMaps.pushBack(map);

        return map;
    }

    static PaintValue contextPaint(const PaintValue& paint, const ComputedStyle& style) {
        PaintValue out = paint;

        out.context = false;

        if (out.kind == PaintKind::CurrentColor) {
            out.kind = PaintKind::Color;
            out.color = style.color;
        }

        if (out.fallback == PaintKind::CurrentColor) {
            out.fallback = PaintKind::Color;
            out.fallbackColor = style.color;
        }

        return out;
    }

    struct MarkerVertex {
        Point p;
        Point in;
        Point out;
        bool hasIn;
        bool hasOut;
    };

    static bool nonZero(Point d) {
        return d.x != 0.f || d.y != 0.f;
    }

    static Point direction(Point from, Point to) {
        return Point{to.x - from.x, to.y - from.y};
    }

    static void markerVertices(const Path& path, Vector<MarkerVertex>& out) {
        const Point* pts = path.points.data();
        size_t pi = 0;
        Point current{0.f, 0.f};
        size_t start = 0;
        bool arc = false;

        auto segment = [&](Point startDir, Point endDir, Point end) {
            if (out.length() == 0) {
                out.pushBack(MarkerVertex{current, Point{0.f, 0.f}, Point{0.f, 0.f}, false, false});
                start = 0;
            }

            MarkerVertex& last = out.mutData()[out.length() - 1];

            if (!arc && !last.hasOut) {
                last.out = startDir;
                last.hasOut = true;
            }

            out.pushBack(MarkerVertex{end, endDir, Point{0.f, 0.f}, true, false});
            current = end;
        };

        for (u8 verb : path.verbs) {
            switch (verb) {
                case VerbMove:
                    current = pts[pi++];
                    start = out.length();
                    out.pushBack(MarkerVertex{current, Point{0.f, 0.f}, Point{0.f, 0.f}, false, false});
                    arc = false;
                    break;
                case VerbLine: {
                    Point end = pts[pi++];
                    Point d = direction(current, end);

                    segment(d, d, end);
                    arc = false;
                    break;
                }
                case VerbCubic:
                case VerbArcCubic: {
                    Point c1 = pts[pi];
                    Point c2 = pts[pi + 1];
                    Point end = pts[pi + 2];
                    Point startDir = direction(current, c1);
                    Point endDir = direction(c2, end);

                    pi += 3;

                    if (!nonZero(startDir)) {
                        startDir = nonZero(direction(current, c2)) ? direction(current, c2) : direction(current, end);
                    }

                    if (!nonZero(endDir)) {
                        endDir = nonZero(direction(c1, end)) ? direction(c1, end) : direction(current, end);
                    }

                    if (arc) {
                        MarkerVertex& last = out.mutData()[out.length() - 1];

                        last.p = end;
                        last.in = endDir;
                        current = end;
                    } else {
                        segment(startDir, endDir, end);
                    }

                    arc = verb == VerbArcCubic;
                    break;
                }
                case VerbClose: {
                    if (out.length() == 0) {
                        break;
                    }

                    MarkerVertex first = out[start];
                    Point d = direction(current, first.p);

                    if (!nonZero(d)) {
                        d = out.back().in;
                    }

                    segment(d, d, first.p);

                    MarkerVertex& closing = out.mutData()[out.length() - 1];
                    MarkerVertex& opening = out.mutData()[start];

                    closing.out = first.out;
                    closing.hasOut = first.hasOut;
                    opening.in = d;
                    opening.hasIn = true;
                    arc = false;
                    break;
                }
                default:
                    break;
            }
        }
    }

    static float angleOf(Point d) {
        return atan2f(d.y, d.x) * 180.f / pi;
    }

    static float vertexAngle(const MarkerVertex& v) {
        if (v.hasIn && v.hasOut) {
            float a = angleOf(v.in);
            float b = angleOf(v.out);

            if (a - b <= -180.f) {
                a += 360.f;
            } else if (a - b > 180.f) {
                a -= 360.f;
            }

            return (a + b) / 2.f;
        }

        if (v.hasOut) {
            return angleOf(v.out);
        }

        if (v.hasIn) {
            return angleOf(v.in);
        }

        return 0.f;
    }

    static bool readAngle(StringView text, float& out) {
        const u8* p = text.begin();

        if (!readNumber(p, text.end(), out)) {
            return false;
        }

        StringView unit(p, text.end());

        if (unit.empty() || equalsIgnoreCase(unit, StringView(u8"deg"))) {
            return true;
        }

        if (equalsIgnoreCase(unit, StringView(u8"rad"))) {
            out *= 180.f / pi;
        } else if (equalsIgnoreCase(unit, StringView(u8"grad"))) {
            out *= 0.9f;
        } else if (equalsIgnoreCase(unit, StringView(u8"turn"))) {
            out *= 360.f;
        } else {
            return false;
        }

        return true;
    }

    static void lowerMarker(Lowering& l, Node* marker, const ComputedStyle& shapeStyle, const Matrix& userMatrix, Viewport vp, const MarkerVertex& userVertex, const MarkerVertex& deviceVertex, bool start) {
        const Document& doc = *l.doc;

        for (Node* active : l.active) {
            if (active == marker) {
                return;
            }
        }

        const ComputedStyle& markerStyle = documentStyle(l, marker, vp);
        float fs = markerStyle.fontSize;
        float w = lengthOr(doc, marker, AtomMarkerWidth, Axis::X, vp, fs, 3.f);
        float h = lengthOr(doc, marker, AtomMarkerHeight, Axis::Y, vp, fs, 3.f);

        if (w <= 0.f || h <= 0.f) {
            return;
        }

        float angle = 0.f;
        float autoAngle = vertexAngle(shapeStyle.nonScaling ? deviceVertex : userVertex);
        u32 orient = attribute(marker, AtomOrient);

        if (orient) {
            StringView text = trim(textOf(doc, orient));

            if (text == StringView(u8"auto")) {
                angle = autoAngle;
            } else if (text == StringView(u8"auto-start-reverse")) {
                angle = start ? autoAngle + 180.f : autoAngle;
            } else if (!readAngle(text, angle)) {
                angle = 0.f;
            }
        }

        u32 units = attribute(marker, AtomMarkerUnits);
        bool userUnits = units && trim(textOf(doc, units)) == StringView(u8"userSpaceOnUse");
        float scale = userUnits ? 1.f : shapeStyle.strokeWidth;
        bool device = shapeStyle.nonScaling && !userUnits;
        const MarkerVertex& vertex = device ? deviceVertex : userVertex;
        Matrix m = device ? identity() : userMatrix;
        Point at = vertex.p;
        float radians = angle * pi / 180.f;
        Matrix placed = multiply(m, translation(at.x, at.y));

        placed = multiply(placed, Matrix{cosf(radians), sinf(radians), -sinf(radians), cosf(radians), 0.f, 0.f});
        placed = multiply(placed, scaling(scale, scale));

        Matrix content = identity();
        Box viewBox;
        Viewport inside{w, h, (float)l.width, (float)l.height, l.rootFontSize};

        if (parseViewBox(doc, marker, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return;
            }

            content = viewBoxTransform(doc, marker, viewBox, w, h);
            inside = Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, (float)l.width, (float)l.height, l.rootFontSize};
        }

        Point ref = apply(content, Point{lengthOr(doc, marker, AtomRefX, Axis::X, inside, fs, 0.f), lengthOr(doc, marker, AtomRefY, Axis::Y, inside, fs, 0.f)});
        Matrix origin = multiply(placed, translation(-ref.x, -ref.y));
        bool clip = !markerStyle.overflowVisible;

        if (clip) {
            Op op = groupOp(1.f);

            op.clipped = true;
            op.clipFirst = (u32)l.clips.length();
            op.clipCount = 1;
            addRectClip(l, origin, 0.f, 0.f, w, h);
            l.out->pushBack(op);
        }

        bool hadContext = l.hasContext;
        PaintValue outerFill = l.contextFill;
        PaintValue outerStroke = l.contextStroke;

        l.hasContext = true;
        l.contextFill = contextPaint(shapeStyle.fill, shapeStyle);
        l.contextStroke = contextPaint(shapeStyle.stroke, shapeStyle);
        l.active.pushBack(marker);
        lowerChildren(l, marker, markerStyle, multiply(origin, content), inside);
        l.active.popBack();
        l.hasContext = hadContext;
        l.contextFill = outerFill;
        l.contextStroke = outerStroke;

        if (clip) {
            endGroup(l);
        }
    }

    static void lowerMarkers(Lowering& l, Node* node, const Path& path, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        if (node->name != AtomPath && node->name != AtomLine && node->name != AtomPolyline && node->name != AtomPolygon) {
            return;
        }

        Node* markers[3];
        bool any = false;

        for (int k = 0; k < 3; k++) {
            markers[k] = style.markers[k] ? elementById(*l.doc, style.markers[k]) : nullptr;

            if (markers[k] && markers[k]->name != AtomMarker) {
                markers[k] = nullptr;
            }

            any = any || markers[k];
        }

        if (!any) {
            return;
        }

        Vector<MarkerVertex> vertices;
        Vector<MarkerVertex> deviceVertices;

        markerVertices(path, vertices);
        markerVertices(style.nonScaling ? transformedPath(path, m) : path, deviceVertices);

        for (size_t i = 0; i < vertices.length() && i < deviceVertices.length(); i++) {
            int kind = i == 0 ? 0 : (i + 1 == vertices.length() ? 2 : 1);

            if (markers[kind]) {
                lowerMarker(l, markers[kind], style, m, vp, vertices[i], deviceVertices[i], kind == 0);
            }

            if (i == 0 && vertices.length() == 1 && markers[2]) {
                lowerMarker(l, markers[2], style, m, vp, vertices[i], deviceVertices[i], false);
            }
        }
    }

    static void lowerShape(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Path path;

        if (!buildShape(*l.doc, node, style, vp, path)) {
            return;
        }

        Flat userFlat;

        flatten(path, identity(), 0.01f, userFlat);

        Box box = flatBox(userFlat);

        for (u8 layer : style.paintOrder) {
            if (layer == 0 && style.visible) {
                if (DevicePaint* fill = devicePaint(l, style.fill, style.fillOpacity, style, style.fill.context ? l.contextBox : box, style.fill.context ? l.contextMatrix : m, vp)) {
                    Flat flat;

                    flatten(path, m, tolerance, flat);
                    emitFill(l, flat, style.fillRule, fill);
                }
            } else if (layer == 1 && style.visible && style.strokeWidth > 0.f) {
                if (DevicePaint* stroke = devicePaint(l, style.stroke, style.strokeOpacity, style, style.stroke.context ? l.contextBox : box, style.stroke.context ? l.contextMatrix : m, vp)) {
                    Flat outline;
                    Flat device;

                    if (style.nonScaling) {
                        strokePath(transformedPath(path, m), style, 1.f, device);
                    } else {
                        strokePath(path, style, matrixScale(m), outline);
                        device.ends.xchg(outline.ends);
                        device.closed.xchg(outline.closed);

                        for (Point p : outline.points) {
                            device.points.pushBack(apply(m, p));
                        }
                    }

                    emitFill(l, device, WindingNonZero, stroke);
                }
            } else if (layer == 2) {
                Matrix outerMatrix = l.contextMatrix;
                Box outerBox = l.contextBox;

                l.contextMatrix = m;
                l.contextBox = box;
                lowerMarkers(l, node, path, style, m, vp);
                l.contextMatrix = outerMatrix;
                l.contextBox = outerBox;
            }
        }
    }

    static void lowerViewport(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Node* content, float x, float y, float w, float h, bool clip) {
        if (w <= 0.f || h <= 0.f) {
            return;
        }

        Matrix inner = multiply(m, translation(x, y));
        Viewport inside{w, h, (float)l.width, (float)l.height, l.rootFontSize};
        Box viewBox;

        if (parseViewBox(*l.doc, node, viewBox)) {
            if (viewBox.x1 <= viewBox.x0 || viewBox.y1 <= viewBox.y0) {
                return;
            }

            inner = multiply(inner, viewBoxTransform(*l.doc, node, viewBox, w, h));
            inside = Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, (float)l.width, (float)l.height, l.rootFontSize};
        }

        if (clip) {
            u32 first = (u32)l.clips.length();

            Op op = groupOp(1.f);

            addRectClip(l, m, x, y, w, h);
            op.clipped = true;
            op.clipFirst = first;
            op.clipCount = 1;
            l.out->pushBack(op);
        }

        lowerChildren(l, content, style, inner, inside);

        if (clip) {
            endGroup(l);
        }
    }

    static bool intrinsicSize(const Document& doc, const Node* root, float& width, float& height) {
        Viewport none{0.f, 0.f, 0.f, 0.f, 16.f};
        Length w;
        Length h;
        Length size;
        StringView sizeText = cascaded(doc, root, AtomFontSize);
        float fontSize = 16.f;

        if (!sizeText.empty() && parseLength(sizeText, size) && size.value >= 0.f) {
            fontSize = size.unit == Unit::Percent ? 16.f * size.value / 100.f : resolveLength(size, Axis::Other, none, 16.f);
        }

        none.rootFontSize = fontSize;
        u32 widthValue = attribute(root, AtomWidth);
        u32 heightValue = attribute(root, AtomHeight);
        bool hasWidth = widthValue && parseLength(textOf(doc, widthValue), w) && w.unit != Unit::Percent && w.value > 0.f;
        bool hasHeight = heightValue && parseLength(textOf(doc, heightValue), h) && h.unit != Unit::Percent && h.value > 0.f;
        Box viewBox;
        bool hasViewBox = parseViewBox(doc, root, viewBox) && viewBox.x1 > viewBox.x0 && viewBox.y1 > viewBox.y0;
        float ratio = hasViewBox ? (viewBox.x1 - viewBox.x0) / (viewBox.y1 - viewBox.y0) : 0.f;

        width = hasWidth ? resolveLength(w, Axis::X, none, fontSize) : 0.f;
        height = hasHeight ? resolveLength(h, Axis::Y, none, fontSize) : 0.f;

        if (hasWidth && !hasHeight && ratio > 0.f) {
            height = width / ratio;
        } else if (hasHeight && !hasWidth && ratio > 0.f) {
            width = height * ratio;
        } else if (!hasWidth && !hasHeight && hasViewBox) {
            width = viewBox.x1 - viewBox.x0;
            height = viewBox.y1 - viewBox.y0;
        } else if (hasWidth && !hasHeight) {
            height = 150.f;
        } else if (hasHeight && !hasWidth) {
            width = 300.f;
        }

        return width > 0.f && height > 0.f;
    }

    static bool explicitSide(const Document& doc, const Node* root, u32 name) {
        u32 value = attribute(root, name);
        Length length;

        return value && parseLength(textOf(doc, value), length) && length.unit != Unit::Percent && length.value > 0.f;
    }

    static Matrix rootContent(Lowering& l, Node* node) {
        Box viewBox;
        float w;
        float h;

        if (parseViewBox(*l.doc, node, viewBox)) {
            return viewBoxTransform(*l.doc, node, viewBox, (float)l.width, (float)l.height);
        }

        if (intrinsicSize(*l.doc, node, w, h)) {
            w = explicitSide(*l.doc, node, AtomWidth) ? w : (float)l.width;
            h = explicitSide(*l.doc, node, AtomHeight) ? h : (float)l.height;

            return scaling((float)l.width / w, (float)l.height / h);
        }

        return identity();
    }

    static void lowerSvg(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        if (node == l.doc->root) {
            float w = (float)l.width;
            float h = (float)l.height;

            l.rootFontSize = style.fontSize;

            if (style.background.a > 0.f) {
                Path canvas;
                Flat flat;

                moveTo(canvas, Point{0.f, 0.f});
                lineTo(canvas, Point{w, 0.f});
                lineTo(canvas, Point{w, h});
                lineTo(canvas, Point{0.f, h});
                closePath(canvas);
                flatten(canvas, identity(), tolerance, flat);
                emitFill(l, flat, WindingNonZero, solidPaint(l, style.background, 1.f));
            }
            Box viewBox;

            if (!parseViewBox(*l.doc, node, viewBox)) {
                float iw;
                float ih;

                if (intrinsicSize(*l.doc, node, iw, ih)) {
                    iw = explicitSide(*l.doc, node, AtomWidth) ? iw : w;
                    ih = explicitSide(*l.doc, node, AtomHeight) ? ih : h;
                    lowerChildren(l, node, style, multiply(m, scaling(w / iw, h / ih)), Viewport{iw, ih, (float)l.width, (float)l.height, l.rootFontSize});

                    return;
                }
            }

            lowerViewport(l, node, style, m, node, 0.f, 0.f, w, h, false);

            return;
        }

        float fs = style.fontSize;
        float w = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, vp.width);
        float h = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, vp.height);

        if (l.sized == node) {
            w = l.sizedWidth >= 0.f ? l.sizedWidth : w;
            h = l.sizedHeight >= 0.f ? l.sizedHeight : h;
            l.sized = nullptr;
        }

        lowerViewport(l, node, style, m, node, lengthOr(*l.doc, node, AtomX, Axis::X, vp, fs, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, fs, 0.f), w, h, !style.overflowVisible);
    }

    static void lowerUse(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        Node* target = hrefTarget(*l.doc, node);

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
        const Matrix& placed = m;

        bool hadContext = l.hasContext;
        PaintValue outerFill = l.contextFill;
        PaintValue outerStroke = l.contextStroke;

        Matrix outerMatrix = l.contextMatrix;
        Box outerBox = l.contextBox;
        Box targetBox;
        u32** outerInstance = l.instance;

        if (l.doc->structural) {
            l.instance = instanceStyles(l, target);
        }

        l.hasContext = true;
        l.contextFill = contextPaint(style.fill, style);
        l.contextStroke = contextPaint(style.stroke, style);
        l.contextMatrix = m;
        l.contextBox = emptyBox();

        if (elementBox(l, target, style, vp, targetBox, 0)) {
            l.contextBox = deviceBox(originTransform(*l.doc, target, AtomTransform, vp), targetBox);
        }

        l.active.pushBack(target);

        if (target->name == AtomSymbol) {
            l.sized = target;
            l.sizedWidth = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, vp.width);
            l.sizedHeight = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, vp.height);
            lowerElement(l, target, style, placed, vp);
            l.sized = nullptr;
        } else {
            if (target->name == AtomSvg) {
                l.sized = target;
                l.sizedWidth = lengthOr(*l.doc, node, AtomWidth, Axis::X, vp, fs, -1.f);
                l.sizedHeight = lengthOr(*l.doc, node, AtomHeight, Axis::Y, vp, fs, -1.f);
            }

            lowerElement(l, target, style, placed, vp);
            l.sized = nullptr;
        }

        l.active.popBack();
        l.hasContext = hadContext;
        l.contextFill = outerFill;
        l.contextStroke = outerStroke;
        l.contextMatrix = outerMatrix;
        l.contextBox = outerBox;
        l.instance = outerInstance;
    }

    static int base64Value(u8 c) {
        if (c >= 'A' && c <= 'Z') {
            return c - 'A';
        }

        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 26;
        }

        if (c >= '0' && c <= '9') {
            return c - '0' + 52;
        }

        if (c == '+' || c == '-') {
            return 62;
        }

        if (c == '/' || c == '_') {
            return 63;
        }

        return -1;
    }

    static bool decodeDataUri(StringView href, StringView& mime, Buffer& out) {
        StringView text = trim(href);

        if (!startsWithIgnoreCase(text, StringView(u8"data:"))) {
            return false;
        }

        const u8* comma = text.begin() + 5;

        while (comma < text.end() && *comma != ',') {
            comma++;
        }

        if (comma >= text.end()) {
            return false;
        }

        StringView header(text.begin() + 5, comma);
        bool base64 = header.length() >= 7 && equalsIgnoreCase(StringView(header.end() - 7, header.end()), StringView(u8";base64"));
        const u8* semicolon = header.begin();

        while (semicolon < header.end() && *semicolon != ';') {
            semicolon++;
        }

        mime = trim(StringView(header.begin(), semicolon));

        const u8* p = comma + 1;

        if (base64) {
            u32 bits = 0;
            int count = 0;

            for (; p < text.end(); p++) {
                if (isSpace(*p) || *p == '=') {
                    continue;
                }

                int value = base64Value(*p);

                if (value < 0) {
                    return false;
                }

                bits = (bits << 6) | (u32)value;
                count += 6;

                if (count >= 8) {
                    count -= 8;

                    u8 byte = (u8)(bits >> count);

                    out.append(&byte, 1);
                }
            }

            return true;
        }

        while (p < text.end()) {
            if (*p == '%' && text.end() - p >= 3 && hexDigit(p[1]) >= 0 && hexDigit(p[2]) >= 0) {
                u8 byte = (u8)(hexDigit(p[1]) * 16 + hexDigit(p[2]));

                out.append(&byte, 1);
                p += 3;
            } else {
                out.append(p, 1);
                p++;
            }
        }

        return true;
    }

    static bool isAuto(const Document& doc, const Node* node, u32 name) {
        u32 value = attribute(node, name);

        return !value || equalsIgnoreCase(trim(textOf(doc, value)), StringView(u8"auto"));
    }

    struct ImageSource {
        Document* nested;
        SvgImage* raster;
        float width;
        float height;
        Box viewBox;
        bool sizeless;
        bool broken;
        Node* ratio;
    };

    static bool loadImage(Lowering& l, Node* node, ImageSource& out) {
        const Document& doc = *l.doc;
        u32 href = attribute(node, AtomHref);
        StringView mime;
        Buffer bytes;

        out.nested = nullptr;
        out.raster = nullptr;
        out.sizeless = false;
        out.broken = false;
        out.ratio = nullptr;

        if (!href) {
            return false;
        }

        StringView reference = trim(textOf(doc, href));
        bool remote = startsWithIgnoreCase(reference, StringView(u8"http:")) || startsWithIgnoreCase(reference, StringView(u8"https:")) || (!reference.empty() && reference[0] == '#');
        StringView fragment;

        if (startsWithIgnoreCase(reference, StringView(u8"data:"))) {
            for (const u8* p = reference.end(); p > reference.begin(); p--) {
                if (p[-1] == '#') {
                    fragment = StringView(p, reference.end());
                    reference = StringView(reference.begin(), p - 1);
                    break;
                }
            }
        }

        bool broken = remote || (decodeDataUri(reference, mime, bytes) && bytes.length() == 0);

        if (!remote && !broken && bytes.length() == 0) {
            return false;
        }

        StringView data = keep(*l.pool, bytes.data(), bytes.length());

        if (equalsIgnoreCase(mime, StringView(u8"image/svg+xml")) && !broken) {
            if (l.imageDepth >= 16) {
                return false;
            }

            Document* nested = l.pool->make<Document>();

            if (loadDocument(*nested, *l.pool, data) && nested->root && nested->root->name == AtomSvg) {
                cascade(*nested);
                out.nested = nested;

                bool sized = intrinsicSize(*nested, nested->root, out.width, out.height);

                if (!sized) {
                    out.width = 300.f;
                    out.height = 150.f;
                }

                if (!parseViewBox(*nested, nested->root, out.viewBox) || out.viewBox.x1 <= out.viewBox.x0 || out.viewBox.y1 <= out.viewBox.y0) {
                    out.viewBox = Box{0.f, 0.f, out.width, out.height};
                    out.sizeless = !sized && !attribute(nested->root, AtomWidth) && !attribute(nested->root, AtomHeight);
                }

                Node* view = fragment.empty() ? nullptr : elementById(*nested, lookup(nested->atoms, fragment));
                Box viewBox;

                if (view && view->name == AtomView && parseViewBox(*nested, view, viewBox) && viewBox.x1 > viewBox.x0 && viewBox.y1 > viewBox.y0) {
                    out.viewBox = viewBox;
                    out.sizeless = false;
                    out.ratio = view;
                }

                return true;
            }

            broken = true;
        }

        SvgImage* raster = !broken && l.decoder ? l.decoder->decode(*l.pool, data) : nullptr;

        if (!raster || raster->width() == 0 || raster->height() == 0 || raster->length() < (size_t)raster->width() * raster->height() * 4) {
            raster = l.pool->make<ImageImpl>((u8*)brokenImage, 16, 16);
            out.broken = true;
        }

        out.raster = raster;
        out.width = (float)raster->width();
        out.height = (float)raster->height();
        out.viewBox = Box{0.f, 0.f, out.width, out.height};

        return true;
    }

    static const u8* halveImage(ObjPool& pool, const u8* pixels, u32& width, u32& height) {
        u32 w = width > 1 ? width / 2 : 1;
        u32 h = height > 1 ? height / 2 : 1;
        u8* out = (u8*)pool.allocate((size_t)w * h * 4);

        for (u32 y = 0; y < h; y++) {
            u32 y0 = height > 1 ? y * 2 : 0;
            u32 y1 = height > 1 ? y * 2 + 1 : 0;

            for (u32 x = 0; x < w; x++) {
                u32 x0 = width > 1 ? x * 2 : 0;
                u32 x1 = width > 1 ? x * 2 + 1 : 0;
                const u8* taps[4] = {pixels + ((size_t)y0 * width + x0) * 4, pixels + ((size_t)y0 * width + x1) * 4, pixels + ((size_t)y1 * width + x0) * 4, pixels + ((size_t)y1 * width + x1) * 4};
                u32 alpha = 0;
                u32 sum[3] = {0, 0, 0};
                u8* d = out + ((size_t)y * w + x) * 4;

                for (const u8* t : taps) {
                    alpha += t[3];

                    for (int c = 0; c < 3; c++) {
                        sum[c] += (u32)t[c] * t[3];
                    }
                }

                for (int c = 0; c < 3; c++) {
                    d[c] = alpha ? (u8)((sum[c] + alpha / 2) / alpha) : 0;
                }

                d[3] = (u8)((alpha + 2) / 4);
            }
        }

        width = w;
        height = h;

        return out;
    }

    static void drawImage(Lowering& l, Node* node, const ImageSource& source, const Matrix& m, Viewport vp, float x, float y, float w, float h) {
        const Document& doc = *l.doc;
        u32 ratio = attribute(node, AtomPreserveAspectRatio);
        bool stretch = ratio && trim(textOf(doc, ratio)) == StringView(u8"none");
        Box viewBox = source.sizeless ? Box{0.f, 0.f, w, h} : source.viewBox;
        Matrix fit = multiply(translation(x, y), source.nested && !stretch ? viewBoxTransform(*source.nested, source.ratio ? source.ratio : source.nested->root, viewBox, w, h) : viewBoxTransform(doc, node, viewBox, w, h));
        Matrix place = multiply(m, fit);

        if (source.raster) {
            DevicePaint* paint = l.pool->make<DevicePaint>();
            Matrix inverse;

            memset(paint, 0, sizeof(DevicePaint));
            paint->type = PaintType::Image;
            paint->image = (const u8*)source.raster->data();
            paint->imageWidth = source.raster->width();
            paint->imageHeight = source.raster->height();
            paint->smooth = true;

            if (!invert(place, inverse)) {
                return;
            }

            float scale = minf(sqrtf(place.a * place.a + place.b * place.b), sqrtf(place.c * place.c + place.d * place.d));
            int levels = scale > 0.f && scale < 1.f ? (int)floorf(maxf(-log2f(scale) - 0.5f, 0.f) + 0.5f) : 0;

            for (int k = 0; k < levels && (paint->imageWidth > 1 || paint->imageHeight > 1); k++) {
                paint->image = halveImage(*l.pool, paint->image, paint->imageWidth, paint->imageHeight);
            }

            paint->inverse = multiply(scaling((float)paint->imageWidth / (float)source.raster->width(), (float)paint->imageHeight / (float)source.raster->height()), inverse);

            Point a = apply(fit, Point{viewBox.x0, viewBox.y0});
            Point b = apply(fit, Point{viewBox.x1, viewBox.y1});
            float rx0 = maxf(minf(a.x, b.x), x);
            float ry0 = maxf(minf(a.y, b.y), y);
            float rx1 = minf(maxf(a.x, b.x), x + w);
            float ry1 = minf(maxf(a.y, b.y), y + h);

            if (rx1 <= rx0 || ry1 <= ry0) {
                return;
            }

            Path rect;
            Flat flat;

            moveTo(rect, Point{rx0, ry0});
            lineTo(rect, Point{rx1, ry0});
            lineTo(rect, Point{rx1, ry1});
            lineTo(rect, Point{rx0, ry1});
            closePath(rect);
            flatten(rect, m, tolerance, flat);
            emitFill(l, flat, WindingNonZero, paint);

            return;
        }

        Op op = groupOp(1.f);

        op.clipped = true;
        op.clipFirst = (u32)l.clips.length();
        op.clipCount = 1;
        addRectClip(l, m, x, y, w, h);
        l.out->pushBack(op);

        Document* outer = l.doc;
        Document* nested = source.nested;
        Vector<ComputedStyle*> outerStyles;

        u32** outerInstance = l.instance;
        Vector<Node*> outerTargets;
        Vector<u32**> outerMaps;

        outerStyles.xchg(l.documentStyles);
        outerTargets.xchg(l.instanceTargets);
        outerMaps.xchg(l.instanceMaps);
        l.instance = nullptr;
        l.documentStyles.zero(nested->nodeCount);
        l.doc = nested;
        l.imageDepth++;

        ComputedStyle rootStyle = computeStyle(l, nested->root, initialStyle(), vp);
        float outerFontSize = l.rootFontSize;

        l.rootFontSize = rootStyle.fontSize;
        lowerChildren(l, nested->root, rootStyle, place, Viewport{viewBox.x1 - viewBox.x0, viewBox.y1 - viewBox.y0, source.sizeless ? w : source.width, source.sizeless ? h : source.height, rootStyle.fontSize});
        l.rootFontSize = outerFontSize;
        l.imageDepth--;
        l.doc = outer;
        l.documentStyles.xchg(outerStyles);
        l.instanceTargets.xchg(outerTargets);
        l.instanceMaps.xchg(outerMaps);
        l.instance = outerInstance;
        endGroup(l);
    }

    static void lowerImage(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        const Document& doc = *l.doc;
        ImageSource source;

        if (!style.visible || !loadImage(l, node, source)) {
            return;
        }

        float fs = style.fontSize;
        float x = lengthOr(doc, node, AtomX, Axis::X, vp, fs, 0.f);
        float y = lengthOr(doc, node, AtomY, Axis::Y, vp, fs, 0.f);
        bool autoWidth = isAuto(doc, node, AtomWidth);
        bool autoHeight = isAuto(doc, node, AtomHeight);
        float w = autoWidth ? source.width : lengthOr(doc, node, AtomWidth, Axis::X, vp, fs, 0.f);
        float h = autoHeight ? source.height : lengthOr(doc, node, AtomHeight, Axis::Y, vp, fs, 0.f);

        if (autoWidth && !autoHeight && source.height > 0.f) {
            w = h * source.width / source.height;
        } else if (autoHeight && !autoWidth && source.width > 0.f) {
            h = w * source.height / source.width;
        }

        if (w <= 0.f || h <= 0.f) {
            return;
        }

        drawImage(l, node, source, m, vp, x, y, w, h);
    }

    static bool relativeLengths(const Document& doc, const Node* node, u32 depth) {
        static const u32 lengths[] = {AtomX, AtomY, AtomWidth, AtomHeight, AtomCx, AtomCy, AtomR, AtomRx, AtomRy, AtomX1, AtomY1, AtomX2, AtomY2};

        for (u32 name : lengths) {
            u32 value = attribute(node, name);
            Length length;

            if (value && parseLength(textOf(doc, value), length) && length.unit == Unit::Percent) {
                return true;
            }
        }

        if (depth < 16) {
            for (Node* child = node->first; child; child = child->next) {
                if (relativeLengths(doc, child, depth + 1)) {
                    return true;
                }
            }
        }

        return false;
    }

    static void lowerImageContent(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp, float x, float y, float w, float h) {
        const Document& doc = *l.doc;
        u32 href = attribute(node, AtomHref);

        if (!href) {
            return;
        }

        StringView reference = trim(textOf(doc, href));

        if (reference.length() > 1 && reference[0] == '#') {
            Node* target = elementById(doc, fragmentAtom(doc, StringView(reference.begin() + 1, reference.end())));

            if (!target) {
                return;
            }

            for (Node* active : l.active) {
                if (active == target) {
                    return;
                }
            }

            const ComputedStyle& inherited = target->parent ? documentStyle(l, target->parent, vp) : style;

            l.active.pushBack(target);
            Matrix place = m;

            if (node->name == AtomFeImage) {
                place = multiply(m, translation(x, y));

                if (relativeLengths(doc, target, 0) && vp.width > 0.f && vp.height > 0.f) {
                    place = multiply(place, scaling(w / vp.width, h / vp.height));
                }
            }

            lowerElement(l, target, inherited, place, vp);
            l.active.popBack();

            return;
        }

        ImageSource source;

        if (w > 0.f && h > 0.f && loadImage(l, node, source) && !(source.broken && node->name == AtomFeImage)) {
            drawImage(l, node, source, m, vp, x, y, w, h);
        }
    }

    static bool passesConditions(const Document& doc, const Node* node) {
        u32 extensions = attribute(node, AtomRequiredExtensions);

        if (extensions) {
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

    static void lowerContent(Lowering& l, Node* node, const ComputedStyle& style, const Matrix& m, Viewport vp) {
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
            case AtomImage:
                lowerImage(l, node, style, m, vp);
                break;
            case AtomSymbol: {
                float w = l.sizedWidth;
                float h = l.sizedHeight;

                l.sized = nullptr;
                lowerViewport(l, node, style, m, node, 0.f, 0.f, w, h, !style.overflowVisible);
                break;
            }
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
    }

    static bool lowerFilteredLayer(Lowering& l, Node* node, const ComputedStyle& parent, const ComputedStyle& style, const Matrix& m, Viewport vp) {
        float sx = sqrtf(m.a * m.a + m.b * m.b);
        float sy = sqrtf(m.c * m.c + m.d * m.d);
        Matrix toLayer;

        if (sx <= 0.f || sy <= 0.f || !invert(multiply(m, scaling(1.f / sx, 1.f / sy)), toLayer)) {
            return false;
        }

        Box visible = deviceBox(toLayer, Box{0.f, 0.f, (float)l.width, (float)l.height});
        float margin = 0.25f * maxf(visible.x1 - visible.x0, visible.y1 - visible.y0);
        float x0 = floorf(visible.x0 - margin);
        float y0 = floorf(visible.y0 - margin);
        float lw = ceilf(visible.x1 + margin) - x0;
        float lh = ceilf(visible.y1 + margin) - y0;

        if (lw <= 0.f || lh <= 0.f || lw * lh > 16.f * (float)l.width * (float)l.height) {
            return false;
        }

        Matrix layer = multiply(translation(-x0, -y0), toLayer);
        Matrix inner = multiply(layer, m);
        u32 width = l.width;
        u32 height = l.height;

        l.width = (u32)lw;
        l.height = (u32)lh;

        Filter* filter = lowerFilters(l, node, parent, style, inner, vp);
        Vector<Op> content;
        Vector<Op>* saved = l.out;

        if (filter) {
            l.out = &content;
            lowerContent(l, node, style, inner, vp);
            l.out = saved;
        }

        l.width = width;
        l.height = height;

        if (!filter) {
            return false;
        }

        Op op;

        memset(&op, 0, sizeof(op));
        op.kind = OpKind::Layer;
        op.first = (u32)l.programs.length();
        op.count = (u32)content.length();
        op.filter = filter;
        op.layerWidth = (u32)lw;
        op.layerHeight = (u32)lh;
        op.layerMatrix = layer;
        l.programs.append(content.data(), content.length());
        l.out->pushBack(op);

        return true;
    }

    static void lowerElement(Lowering& l, Node* node, const ComputedStyle& parent, const Matrix& ctm, Viewport vp) {
        bool instance = node->name == AtomSymbol && l.sized == node;

        if ((!renderable(node->name) && !instance) || l.depth > 256 || (node != l.doc->root && !passesConditions(*l.doc, node))) {
            return;
        }

        ComputedStyle style = computeStyle(l, node, parent, vp);

        if ((style.displayNone && node != l.doc->root) || style.opacity <= 0.f) {
            return;
        }

        Matrix m = multiply(ctm, originTransform(*l.doc, node, AtomTransform, vp));
        Op op = groupOp(style.opacity);

        if (node->name == AtomUse) {
            m = multiply(m, translation(lengthOr(*l.doc, node, AtomX, Axis::X, vp, style.fontSize, 0.f), lengthOr(*l.doc, node, AtomY, Axis::Y, vp, style.fontSize, 0.f)));
        }

        op.clipped = style.clipPath ? resolveClip(l, node, parent, style, m, vp, op) : (style.clipShape && shapeClip(l, node, parent, style, m, vp, op));

        if (style.mask) {
            resolveMask(l, node, parent, style, m, vp, op);
        }

        bool complex = style.filterCount && node != l.doc->root && (fabsf(m.b) > 1e-6f || fabsf(m.c) > 1e-6f);

        if (style.filterCount && !complex) {
            op.filter = lowerFilters(l, node, parent, style, node == l.doc->root ? multiply(m, rootContent(l, node)) : m, vp);
        }

        op.blend = style.blend;

        bool layer = op.clipped || op.masked || op.filter || style.opacity < 1.f || style.blend || style.isolate;

        if (layer) {
            l.out->pushBack(op);
        }

        l.depth++;

        if (!complex || !lowerFilteredLayer(l, node, parent, style, m, vp)) {
            lowerContent(l, node, style, m, vp);
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

    static Color texel(const DevicePaint& paint, int x, int y) {
        x = x < 0 ? 0 : (x >= (int)paint.imageWidth ? (int)paint.imageWidth - 1 : x);
        y = y < 0 ? 0 : (y >= (int)paint.imageHeight ? (int)paint.imageHeight - 1 : y);

        const u8* t = paint.image + ((size_t)y * paint.imageWidth + x) * 4;
        float a = t[3] / 255.f;

        return Color{t[0] / 255.f * a, t[1] / 255.f * a, t[2] / 255.f * a, a};
    }

    static bool paintAt(const DevicePaint& paint, float x, float y, Color& out) {
        if (paint.type == PaintType::Solid) {
            out = paint.color;

            return true;
        }

        Point p = apply(paint.inverse, Point{x, y});

        if (paint.type == PaintType::Pattern) {
            int tw = (int)paint.imageWidth;
            int th = (int)paint.imageHeight;
            float fx = p.x - 0.5f;
            float fy = p.y - 0.5f;
            float x0 = floorf(fx);
            float y0 = floorf(fy);
            float tx = fx - x0;
            float ty = fy - y0;
            int ix = (int)fmodf(x0, (float)tw);
            int iy = (int)fmodf(y0, (float)th);

            ix = ix < 0 ? ix + tw : ix;
            iy = iy < 0 ? iy + th : iy;

            int jx = ix + 1 == tw ? 0 : ix + 1;
            int jy = iy + 1 == th ? 0 : iy + 1;
            const float* c00 = paint.tile + ((size_t)iy * tw + ix) * 4;
            const float* c10 = paint.tile + ((size_t)iy * tw + jx) * 4;
            const float* c01 = paint.tile + ((size_t)jy * tw + ix) * 4;
            const float* c11 = paint.tile + ((size_t)jy * tw + jx) * 4;
            float w00 = (1.f - tx) * (1.f - ty) * paint.opacity;
            float w10 = tx * (1.f - ty) * paint.opacity;
            float w01 = (1.f - tx) * ty * paint.opacity;
            float w11 = tx * ty * paint.opacity;

            out = Color{
                c00[0] * w00 + c10[0] * w10 + c01[0] * w01 + c11[0] * w11,
                c00[1] * w00 + c10[1] * w10 + c01[1] * w01 + c11[1] * w11,
                c00[2] * w00 + c10[2] * w10 + c01[2] * w01 + c11[2] * w11,
                c00[3] * w00 + c10[3] * w10 + c01[3] * w01 + c11[3] * w11,
            };

            return out.a > 0.f;
        }

        if (paint.type == PaintType::Image) {
            if (!paint.smooth) {
                out = texel(paint, (int)floorf(p.x), (int)floorf(p.y));

                return true;
            }

            float fx = p.x - 0.5f;
            float fy = p.y - 0.5f;
            float x0 = floorf(fx);
            float y0 = floorf(fy);
            float tx = fx - x0;
            float ty = fy - y0;
            Color c00 = texel(paint, (int)x0, (int)y0);
            Color c10 = texel(paint, (int)x0 + 1, (int)y0);
            Color c01 = texel(paint, (int)x0, (int)y0 + 1);
            Color c11 = texel(paint, (int)x0 + 1, (int)y0 + 1);
            float w00 = (1.f - tx) * (1.f - ty);
            float w10 = tx * (1.f - ty);
            float w01 = (1.f - tx) * ty;
            float w11 = tx * ty;

            out = Color{
                c00.r * w00 + c10.r * w10 + c01.r * w01 + c11.r * w11,
                c00.g * w00 + c10.g * w10 + c01.g * w01 + c11.g * w11,
                c00.b * w00 + c10.b * w10 + c01.b * w01 + c11.b * w11,
                c00.a * w00 + c10.a * w10 + c01.a * w01 + c11.a * w11,
            };

            return true;
        }

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

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, float* base);

    static void prepareTile(const Lowering& l, DevicePaint& paint) {
        if (paint.tile) {
            return;
        }

        size_t size = (size_t)paint.imageWidth * paint.imageHeight;
        Raster tile;

        paint.tile = (float*)l.pool->allocate(sizeof(float) * size * 4);
        memset(paint.tile, 0, sizeof(float) * size * 4);
        tile.width = paint.imageWidth;
        tile.height = paint.imageHeight;
        runProgram(tile, l, l.programs.data() + paint.programFirst, paint.programCount, paint.tile);
    }

    static void fillOp(Raster& r, const Lowering& l, const Op& op, float* target) {
        if (op.paint->type == PaintType::Pattern) {
            prepareTile(l, *op.paint);
        }

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

    static void accumulateShape(Raster& r, const Lowering& l, const ClipShape& shape, float* target) {
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
            return;
        }

        const float* acc = r.accumulation.data();
        u32 stride = w + 2;

        for (u32 y = 0; y < h; y++) {
            float sum = 0.f;
            const float* line = acc + (size_t)y * stride;
            float* row = target + (size_t)(iy0 + y) * r.width + ix0;

            for (u32 x = 0; x < w; x++) {
                sum += line[x];
                row[x] = maxf(row[x], coverage(sum, shape.rule));
            }
        }
    }

    static float* clipBuffer(Raster& r, const Lowering& l, const Op& op, ObjPool& pool) {
        size_t size = (size_t)r.width * r.height;
        float* mask = (float*)pool.allocate(sizeof(float) * size);

        memset(mask, 0, sizeof(float) * size);

        if (op.clipProgram) {
            float* content = (float*)pool.allocate(sizeof(float) * size * 4);

            memset(content, 0, sizeof(float) * size * 4);
            runProgram(r, l, l.programs.data() + op.clipFirst, op.clipCount, content);

            for (size_t i = 0; i < size; i++) {
                mask[i] = content[i * 4 + 3];
            }

            return mask;
        }

        for (u32 s = op.clipFirst; s < op.clipFirst + op.clipCount; s++) {
            accumulateShape(r, l, l.clips[s], mask);
        }

        return mask;
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, float* base);

    static float srgbToLinear(float c);

    static float* maskBuffer(Raster& r, const Lowering& l, const Op& op, ObjPool& pool) {
        size_t size = (size_t)r.width * r.height;
        float* content = (float*)pool.allocate(sizeof(float) * size * 4);
        float* mask = (float*)pool.allocate(sizeof(float) * size);

        memset(content, 0, sizeof(float) * size * 4);
        memset(mask, 0, sizeof(float) * size);
        runProgram(r, l, l.programs.data() + op.maskFirst, op.maskCount, content);
        accumulateShape(r, l, l.clips[op.maskRegion], mask);

        for (size_t i = 0; i < size; i++) {
            const float* c = content + i * 4;
            float value = (op.maskType & 1) ? c[3] : 0.2126f * c[0] + 0.7152f * c[1] + 0.0722f * c[2];

            if ((op.maskType & 3) == 2 && c[3] > 0.f) {
                value = c[3] * (0.2126f * srgbToLinear(clampUnit(c[0] / c[3])) + 0.7152f * srgbToLinear(clampUnit(c[1] / c[3])) + 0.0722f * srgbToLinear(clampUnit(c[2] / c[3])));
            }

            mask[i] *= value;
        }

        return mask;
    }

    struct FilterResult {
        float* data;
        bool linear;
    };

    struct FilterRun {
        Raster* raster;
        const Lowering* lowering;
        const Filter* filter;
        ObjPool* pool;
        float* source;
        float* sourceLinear;
        float* sourceAlpha;
        float* empty;
        Vector<FilterResult> results;
        Vector<float*> linearCache;
        Vector<float*> srgbCache;
        float scaleX;
        float scaleY;
        size_t size;
        int width;
    };

    static float* filterBuffer(FilterRun& run) {
        float* data = (float*)run.pool->allocate(sizeof(float) * run.size * 4);

        memset(data, 0, sizeof(float) * run.size * 4);

        return data;
    }

    static float srgbToLinear(float c) {
        return c <= 0.04045f ? c / 12.92f : powf((c + 0.055f) / 1.055f, 2.4f);
    }

    static float linearToSrgb(float c) {
        return c <= 0.0031308f ? c * 12.92f : 1.055f * powf(c, 1.f / 2.4f) - 0.055f;
    }

    static Color toSpace(Color c, bool linear) {
        if (!linear) {
            return c;
        }

        return Color{srgbToLinear(c.r), srgbToLinear(c.g), srgbToLinear(c.b), c.a};
    }

    static float* convertBuffer(FilterRun& run, const float* src, bool toLinear) {
        float* out = filterBuffer(run);
        const IntBox& b = run.filter->region;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                float a = src[i + 3];

                if (a <= 0.f) {
                    continue;
                }

                for (int c = 0; c < 3; c++) {
                    float v = clampUnit(src[i + c] / a);

                    out[i + c] = (toLinear ? srgbToLinear(v) : linearToSrgb(v)) * a;
                }

                out[i + 3] = a;
            }
        }

        return out;
    }

    static const float* filterInputBuffer(FilterRun& run, int index, bool linear) {
        if (index == InputSourceAlpha) {
            return run.sourceAlpha;
        }

        if (index == InputNone || index >= (int)run.results.length()) {
            return run.empty;
        }

        if (index == InputSourceGraphic) {
            if (!linear) {
                return run.source;
            }

            if (!run.sourceLinear) {
                run.sourceLinear = convertBuffer(run, run.source, true);
            }

            return run.sourceLinear;
        }

        const FilterResult& result = run.results[index];

        if (result.linear == linear) {
            return result.data;
        }

        Vector<float*>& cache = linear ? run.linearCache : run.srgbCache;

        if (!cache[index]) {
            cache.mut(index) = convertBuffer(run, result.data, linear);
        }

        return cache[index];
    }

    static IntBox inputBox(const FilterRun& run, int index) {
        if (index >= 0 && index < (int)run.results.length()) {
            return run.filter->primitives[index].box;
        }

        return run.filter->region;
    }

    static float numberOf(const Document& doc, const Node* node, u32 name, float fallback) {
        float value;
        u32 atom = attribute(node, name);

        return atom && parseNumberText(textOf(doc, atom), value) ? value : fallback;
    }

    static bool numberList(const Document& doc, const Node* node, u32 name, Vector<float>& out) {
        u32 atom = attribute(node, name);

        if (!atom) {
            return false;
        }

        StringView text = textOf(doc, atom);
        const u8* p = text.begin();

        while (true) {
            float value;

            while (p < text.end() && isSpace(*p)) {
                p++;
            }

            if (p >= text.end()) {
                return true;
            }

            if (!readNumber(p, text.end(), value)) {
                return false;
            }

            out.pushBack(value);
            skipSeparator(p, text.end());
        }
    }

    static bool numberPair(const Document& doc, const Node* node, u32 name, float& a, float& b) {
        Vector<float> values;

        if (!numberList(doc, node, name, values) || values.length() == 0 || values.length() > 2) {
            return false;
        }

        a = values[0];
        b = values.length() == 2 ? values[1] : values[0];

        return true;
    }

    static float userX(const Filter& f, float value) {
        return f.boxPrimitives ? value * (f.bbox.x1 - f.bbox.x0) : value;
    }

    static float userY(const Filter& f, float value) {
        return f.boxPrimitives ? value * (f.bbox.y1 - f.bbox.y0) : value;
    }

    static void filterFlood(FilterRun& run, const FilterPrimitive& p, float* out) {
        Color c = toSpace(p.flood, p.linear);
        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                float* d = out + ((size_t)y * run.width + x) * 4;

                d[0] = c.r * c.a;
                d[1] = c.g * c.a;
                d[2] = c.b * c.a;
                d[3] = c.a;
            }
        }
    }

    static void copyBox(FilterRun& run, const float* in, float* out, const IntBox& b) {
        for (int y = b.y0; y < b.y1; y++) {
            size_t i = ((size_t)y * run.width + b.x0) * 4;

            memcpy(out + i, in + i, sizeof(float) * 4 * (size_t)(b.x1 - b.x0));
        }
    }

    static void shiftBuffer(FilterRun& run, const float* in, float* out, const IntBox& b, float dx, float dy) {
        int ox = (int)floorf(dx + 0.5f);
        int oy = (int)floorf(dy + 0.5f);
        int height = (int)run.raster->height;

        for (int y = b.y0; y < b.y1; y++) {
            int sy = y - oy;

            if (sy < 0 || sy >= height) {
                continue;
            }

            for (int x = b.x0; x < b.x1; x++) {
                int sx = x - ox;

                if (sx < 0 || sx >= run.width) {
                    continue;
                }

                const float* s = in + ((size_t)sy * run.width + sx) * 4;
                float* d = out + ((size_t)y * run.width + x) * 4;

                d[0] = s[0];
                d[1] = s[1];
                d[2] = s[2];
                d[3] = s[3];
            }
        }
    }

    static void filterOffset(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        float dx = userX(f, numberOf(doc, p.node, AtomDx, 0.f));
        float dy = userY(f, numberOf(doc, p.node, AtomDy, 0.f));
        const float* in = filterInputBuffer(run, p.in1, p.linear);

        shiftBuffer(run, in, out, p.box, f.ctm.a * dx + f.ctm.c * dy, f.ctm.b * dx + f.ctm.d * dy);
    }

    static void boxBlurLine(const float* src, float* dst, int count, int stride, int left, int right) {
        int window = left + right + 1;
        float sum[4] = {0.f, 0.f, 0.f, 0.f};
        float scale = 1.f / (float)window;

        for (int i = -left; i <= right; i++) {
            if (i >= 0 && i < count) {
                for (int c = 0; c < 4; c++) {
                    sum[c] += src[(size_t)i * stride + c];
                }
            }
        }

        for (int i = 0; i < count; i++) {
            for (int c = 0; c < 4; c++) {
                dst[(size_t)i * stride + c] = sum[c] * scale;
            }

            int out = i - left;
            int in = i + right + 1;

            if (out >= 0 && out < count) {
                for (int c = 0; c < 4; c++) {
                    sum[c] -= src[(size_t)out * stride + c];
                }
            }

            if (in >= 0 && in < count) {
                for (int c = 0; c < 4; c++) {
                    sum[c] += src[(size_t)in * stride + c];
                }
            }
        }
    }

    static void gaussLine(const float* src, float* dst, int count, int stride, const float* kernel, int radius) {
        for (int i = 0; i < count; i++) {
            float sum[4] = {0.f, 0.f, 0.f, 0.f};

            for (int k = -radius; k <= radius; k++) {
                int j = i + k;

                if (j < 0 || j >= count) {
                    continue;
                }

                float w = kernel[k + radius];

                for (int c = 0; c < 4; c++) {
                    sum[c] += src[(size_t)j * stride + c] * w;
                }
            }

            for (int c = 0; c < 4; c++) {
                dst[(size_t)i * stride + c] = sum[c];
            }
        }
    }

    static void blurLine(const float* src, float* dst, float* scratch, int count, int stride, float sigma) {
        if (sigma <= 0.f) {
            for (int i = 0; i < count; i++) {
                for (int c = 0; c < 4; c++) {
                    dst[(size_t)i * stride + c] = src[(size_t)i * stride + c];
                }
            }

            return;
        }

        if (sigma < 2.f) {
            int radius = (int)ceilf(sigma * 3.f);
            float kernel[64];
            float total = 0.f;

            radius = radius > 31 ? 31 : radius;

            for (int k = -radius; k <= radius; k++) {
                kernel[k + radius] = expf(-(float)(k * k) / (2.f * sigma * sigma));
                total += kernel[k + radius];
            }

            for (int k = 0; k <= 2 * radius; k++) {
                kernel[k] /= total;
            }

            gaussLine(src, dst, count, stride, kernel, radius);

            return;
        }

        int d = (int)floorf(sigma * 3.f * sqrtf(2.f * pi) / 4.f + 0.5f);

        if (d % 2 == 1) {
            int r = (d - 1) / 2;

            boxBlurLine(src, dst, count, stride, r, r);
            boxBlurLine(dst, scratch, count, stride, r, r);
            boxBlurLine(scratch, dst, count, stride, r, r);
        } else {
            boxBlurLine(src, dst, count, stride, d / 2, d / 2 - 1);
            boxBlurLine(dst, scratch, count, stride, d / 2 - 1, d / 2);
            boxBlurLine(scratch, dst, count, stride, d / 2, d / 2);
        }
    }

    static void blurBuffer(FilterRun& run, const float* in, float* out, const IntBox& box, float sigmaX, float sigmaY) {
        int height = (int)run.raster->height;
        int width = run.width;
        float* horizontal = filterBuffer(run);
        Vector<float> line;
        Vector<float> lineOut;
        Vector<float> scratch;
        int maxLength = width > height ? width : height;

        line.zero((size_t)maxLength * 4);
        lineOut.zero((size_t)maxLength * 4);
        scratch.zero((size_t)maxLength * 4);

        for (int y = 0; y < height; y++) {
            const float* row = in + (size_t)y * width * 4;
            float* target = horizontal + (size_t)y * width * 4;

            blurLine(row, target, scratch.mutData(), width, 4, sigmaX);
        }

        for (int x = box.x0; x < box.x1; x++) {
            for (int y = 0; y < height; y++) {
                memcpy(line.mutData() + (size_t)y * 4, horizontal + ((size_t)y * width + x) * 4, sizeof(float) * 4);
            }

            blurLine(line.data(), lineOut.mutData(), scratch.mutData(), height, 4, sigmaY);

            for (int y = box.y0; y < box.y1; y++) {
                memcpy(out + ((size_t)y * width + x) * 4, lineOut.data() + (size_t)y * 4, sizeof(float) * 4);
            }
        }
    }

    static void filterBlur(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        float sx = 0.f;
        float sy = 0.f;
        const float* in = filterInputBuffer(run, p.in1, p.linear);

        bool valid = true;

        if (!p.node) {
            sx = p.params[0];
            sy = p.params[1];
        } else if (attribute(p.node, AtomStdDeviation)) {
            valid = numberPair(*f.doc, p.node, AtomStdDeviation, sx, sy);
        }

        if (!valid || sx < 0.f || sy < 0.f) {
            copyBox(run, in, out, p.box);

            return;
        }

        sx = userX(f, sx) * run.scaleX;
        sy = userY(f, sy) * run.scaleY;

        if (sx <= 0.f && sy <= 0.f) {
            copyBox(run, in, out, p.box);

            return;
        }

        blurBuffer(run, in, out, p.box, sx, sy);
    }

    static void filterDropShadow(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        float sx = 2.f;
        float sy = 2.f;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        float dx = p.node ? userX(f, numberOf(*f.doc, p.node, AtomDx, 2.f)) : p.params[0];
        float dy = p.node ? userY(f, numberOf(*f.doc, p.node, AtomDy, 2.f)) : p.params[1];

        if (!p.node) {
            sx = p.params[2];
            sy = p.params[2];
        } else if (attribute(p.node, AtomStdDeviation) && !numberPair(*f.doc, p.node, AtomStdDeviation, sx, sy)) {
            return;
        }

        if (sx < 0.f || sy < 0.f) {
            return;
        }

        float* shadow = filterBuffer(run);
        float* moved = filterBuffer(run);
        Color c = toSpace(p.flood, p.linear);
        IntBox all = run.filter->region;

        if (userX(f, sx) * run.scaleX > 0.f || userY(f, sy) * run.scaleY > 0.f) {
            blurBuffer(run, in, shadow, all, userX(f, sx) * run.scaleX, userY(f, sy) * run.scaleY);
        } else {
            copyBox(run, in, shadow, all);
        }

        shiftBuffer(run, shadow, moved, p.box, f.ctm.a * dx + f.ctm.c * dy, f.ctm.b * dx + f.ctm.d * dy);

        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                float a = moved[i + 3] * c.a;
                const float* s = in + i;
                float keep = 1.f - s[3];

                out[i] = s[0] + c.r * a * keep;
                out[i + 1] = s[1] + c.g * a * keep;
                out[i + 2] = s[2] + c.b * a * keep;
                out[i + 3] = s[3] + a * keep;
            }
        }
    }

    static void unpremultiply(const float* s, float* c) {
        float a = s[3];

        if (a <= 0.f) {
            c[0] = 0.f;
            c[1] = 0.f;
            c[2] = 0.f;
            c[3] = 0.f;

            return;
        }

        c[0] = clampUnit(s[0] / a);
        c[1] = clampUnit(s[1] / a);
        c[2] = clampUnit(s[2] / a);
        c[3] = a;
    }

    static void storePremultiplied(float* d, const float* c) {
        float a = clampUnit(c[3]);

        d[0] = clampUnit(c[0]) * a;
        d[1] = clampUnit(c[1]) * a;
        d[2] = clampUnit(c[2]) * a;
        d[3] = a;
    }

    static void filterColorMatrix(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Document& doc = *run.filter->doc;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        float m[20] = {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0};
        u32 type = p.node ? attribute(p.node, AtomType) : 0;
        StringView kind = type ? trim(textOf(doc, type)) : StringView(u8"matrix");
        Vector<float> values;
        bool hasValues = p.node && numberList(doc, p.node, AtomValues, values);

        if (kind != StringView(u8"saturate") && kind != StringView(u8"hueRotate") && kind != StringView(u8"luminanceToAlpha")) {
            kind = StringView(u8"matrix");
        }

        if (!p.node) {
            memcpy(m, p.params, sizeof(m));
        } else if (kind == StringView(u8"saturate")) {
            float s = hasValues && values.length() == 1 ? values[0] : 1.f;
            float t[20] = {0.213f + 0.787f * s, 0.715f - 0.715f * s, 0.072f - 0.072f * s, 0, 0, 0.213f - 0.213f * s, 0.715f + 0.285f * s, 0.072f - 0.072f * s, 0, 0, 0.213f - 0.213f * s, 0.715f - 0.715f * s, 0.072f + 0.928f * s, 0, 0, 0, 0, 0, 1, 0};

            memcpy(m, t, sizeof(m));
        } else if (kind == StringView(u8"hueRotate")) {
            float angle = (hasValues && values.length() == 1 ? values[0] : 0.f) * pi / 180.f;
            float cs = cosf(angle);
            float sn = sinf(angle);
            float t[20] = {
                0.213f + cs * 0.787f - sn * 0.213f,
                0.715f - cs * 0.715f - sn * 0.715f,
                0.072f - cs * 0.072f + sn * 0.928f,
                0,
                0,
                0.213f - cs * 0.213f + sn * 0.143f,
                0.715f + cs * 0.285f + sn * 0.140f,
                0.072f - cs * 0.072f - sn * 0.283f,
                0,
                0,
                0.213f - cs * 0.213f - sn * 0.787f,
                0.715f - cs * 0.715f + sn * 0.715f,
                0.072f + cs * 0.928f + sn * 0.072f,
                0,
                0,
                0,
                0,
                0,
                1,
                0,
            };

            memcpy(m, t, sizeof(m));
        } else if (kind == StringView(u8"luminanceToAlpha")) {
            float t[20] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0.2125f, 0.7154f, 0.0721f, 0, 0};

            memcpy(m, t, sizeof(m));
        } else if (kind == StringView(u8"matrix") && hasValues && values.length() == 20) {
            for (int i = 0; i < 20; i++) {
                m[i] = values[i];
            }
        }

        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                float c[4];
                float r[4];

                unpremultiply(in + i, c);

                for (int k = 0; k < 4; k++) {
                    r[k] = m[k * 5] * c[0] + m[k * 5 + 1] * c[1] + m[k * 5 + 2] * c[2] + m[k * 5 + 3] * c[3] + m[k * 5 + 4];
                }

                storePremultiplied(out + i, r);
            }
        }
    }

    struct TransferFunction {
        int type;
        Vector<float> table;
        float slope;
        float intercept;
        float amplitude;
        float exponent;
        float offset;
    };

    static float transfer(const TransferFunction& f, float c) {
        size_t n = f.table.length();

        switch (f.type) {
            case 1:
                if (n == 0) {
                    return c;
                }

                if (n == 1) {
                    return f.table[0];
                }

                {
                    size_t k = c >= 1.f ? n - 2 : (size_t)(c * (float)(n - 1));
                    float v0 = f.table[k];
                    float v1 = f.table[k + 1];

                    return v0 + (c * (float)(n - 1) - (float)k) * (v1 - v0);
                }
            case 2:
                if (n == 0) {
                    return c;
                }

                {
                    size_t k = c >= 1.f ? n - 1 : (size_t)(c * (float)n);

                    return f.table[k];
                }
            case 3:
                return f.slope * c + f.intercept;
            case 4:
                return f.amplitude * powf(c, f.exponent) + f.offset;
            default:
                return c;
        }
    }

    static void filterComponentTransfer(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Document& doc = *run.filter->doc;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        TransferFunction functions[4];
        u32 names[4] = {AtomFeFuncR, AtomFeFuncG, AtomFeFuncB, AtomFeFuncA};

        for (int k = 0; k < 4; k++) {
            functions[k].type = 0;
        }

        for (Node* child = p.node->first; child; child = child->next) {
            for (int k = 0; k < 4; k++) {
                if (child->name != names[k]) {
                    continue;
                }

                TransferFunction& f = functions[k];
                u32 type = attribute(child, AtomType);
                StringView kind = type ? trim(textOf(doc, type)) : StringView();

                f.table.clear();
                f.type = kind == StringView(u8"table") ? 1 : kind == StringView(u8"discrete") ? 2 : kind == StringView(u8"linear") ? 3 : kind == StringView(u8"gamma") ? 4 : 0;
                numberList(doc, child, AtomTableValues, f.table);
                f.slope = numberOf(doc, child, AtomSlope, 1.f);
                f.intercept = numberOf(doc, child, AtomIntercept, 0.f);
                f.amplitude = numberOf(doc, child, AtomAmplitude, 1.f);
                f.exponent = numberOf(doc, child, AtomExponent, 1.f);
                f.offset = numberOf(doc, child, AtomOffset, 0.f);
            }
        }

        const IntBox& b = p.box;

        for (int y = b.y0; y < b.y1; y++) {
            for (int x = b.x0; x < b.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                float c[4];

                unpremultiply(in + i, c);

                for (int k = 0; k < 4; k++) {
                    c[k] = transfer(functions[k], c[k]);
                }

                storePremultiplied(out + i, c);
            }
        }
    }

    static void filterComposite(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Document& doc = *run.filter->doc;
        const float* a = filterInputBuffer(run, p.in1, p.linear);
        const float* b = filterInputBuffer(run, p.in2, p.linear);
        u32 op = attribute(p.node, AtomOperator);
        StringView kind = op ? trim(textOf(doc, op)) : StringView(u8"over");
        float k1 = numberOf(doc, p.node, AtomK1, 0.f);
        float k2 = numberOf(doc, p.node, AtomK2, 0.f);
        float k3 = numberOf(doc, p.node, AtomK3, 0.f);
        float k4 = numberOf(doc, p.node, AtomK4, 0.f);
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                const float* s = a + i;
                const float* d = b + i;
                float* o = out + i;

                if (kind == StringView(u8"arithmetic")) {
                    float alpha = clampUnit(k1 * s[3] * d[3] + k2 * s[3] + k3 * d[3] + k4);

                    for (int c = 0; c < 3; c++) {
                        o[c] = minf(clampUnit(k1 * s[c] * d[c] + k2 * s[c] + k3 * d[c] + k4), alpha);
                    }

                    o[3] = alpha;
                    continue;
                }

                float fa = 1.f;
                float fb = 1.f - s[3];

                if (kind == StringView(u8"in")) {
                    fa = d[3];
                    fb = 0.f;
                } else if (kind == StringView(u8"out")) {
                    fa = 1.f - d[3];
                    fb = 0.f;
                } else if (kind == StringView(u8"atop")) {
                    fa = d[3];
                    fb = 1.f - s[3];
                } else if (kind == StringView(u8"xor")) {
                    fa = 1.f - d[3];
                    fb = 1.f - s[3];
                } else if (kind == StringView(u8"lighter")) {
                    fa = 1.f;
                    fb = 1.f;
                }

                for (int c = 0; c < 4; c++) {
                    o[c] = minf(s[c] * fa + d[c] * fb, 1.f);
                }
            }
        }
    }

    static float blendChannel(int mode, float cb, float cs) {
        switch (mode) {
            case 1:
                return cb * cs;
            case 2:
                return cb + cs - cb * cs;
            case 3:
                return cb <= 0.5f ? 2.f * cb * cs : 1.f - 2.f * (1.f - cb) * (1.f - cs);
            case 4:
                return minf(cb, cs);
            case 5:
                return maxf(cb, cs);
            case 6:
                return cb <= 0.f ? 0.f : (cs >= 1.f ? 1.f : minf(1.f, cb / (1.f - cs)));
            case 7:
                return cb >= 1.f ? 1.f : (cs <= 0.f ? 0.f : 1.f - minf(1.f, (1.f - cb) / cs));
            case 8:
                return cs <= 0.5f ? 2.f * cb * cs : 1.f - 2.f * (1.f - cb) * (1.f - cs);
            case 9: {
                if (cs <= 0.5f) {
                    return cb - (1.f - 2.f * cs) * cb * (1.f - cb);
                }

                float dcb = cb <= 0.25f ? ((16.f * cb - 12.f) * cb + 4.f) * cb : sqrtf(cb);

                return cb + (2.f * cs - 1.f) * (dcb - cb);
            }
            case 10:
                return fabsf(cb - cs);
            case 11:
                return cb + cs - 2.f * cb * cs;
            default:
                return cs;
        }
    }

    static float luminosity(const float* c) {
        return 0.3f * c[0] + 0.59f * c[1] + 0.11f * c[2];
    }

    static void clipColor(float* c) {
        float l = luminosity(c);
        float n = minf(c[0], minf(c[1], c[2]));
        float x = maxf(c[0], maxf(c[1], c[2]));

        for (int k = 0; k < 3; k++) {
            if (n < 0.f) {
                c[k] = l + (c[k] - l) * l / (l - n);
            }

            if (x > 1.f) {
                c[k] = l + (c[k] - l) * (1.f - l) / (x - l);
            }
        }
    }

    static void setLuminosity(float* c, float l) {
        float d = l - luminosity(c);

        c[0] += d;
        c[1] += d;
        c[2] += d;
        clipColor(c);
    }

    static float saturation(const float* c) {
        return maxf(c[0], maxf(c[1], c[2])) - minf(c[0], minf(c[1], c[2]));
    }

    static void setSaturation(float* c, float s) {
        int imax = 0;
        int imin = 0;

        for (int k = 1; k < 3; k++) {
            if (c[k] > c[imax]) {
                imax = k;
            }

            if (c[k] < c[imin]) {
                imin = k;
            }
        }

        if (imax == imin) {
            c[0] = 0.f;
            c[1] = 0.f;
            c[2] = 0.f;

            return;
        }

        int imid = 3 - imax - imin;
        float range = c[imax] - c[imin];

        c[imid] = (c[imid] - c[imin]) * s / range;
        c[imax] = s;
        c[imin] = 0.f;
    }

    static int blendMode(StringView text) {
        static const char* const modes[] = {"normal", "multiply", "screen", "overlay", "darken", "lighten", "color-dodge", "color-burn", "hard-light", "soft-light", "difference", "exclusion", "hue", "saturation", "color", "luminosity"};

        for (int k = 0; k < 16; k++) {
            if (text == StringView(modes[k])) {
                return k;
            }
        }

        return 0;
    }

    static void blendPixel(int mode, const float* src, const float* dst, float* out) {
        float cs[4];
        float cb[4];
        float mixed[3];

        unpremultiply(src, cs);
        unpremultiply(dst, cb);

        float as = src[3];
        float ab = dst[3];

        if (mode < 12) {
            for (int k = 0; k < 3; k++) {
                mixed[k] = blendChannel(mode, cb[k], cs[k]);
            }
        } else {
            float c[3] = {cs[0], cs[1], cs[2]};
            float b[3] = {cb[0], cb[1], cb[2]};

            if (mode == 12) {
                float t[3] = {c[0], c[1], c[2]};

                setSaturation(t, saturation(b));
                setLuminosity(t, luminosity(b));
                memcpy(mixed, t, sizeof(mixed));
            } else if (mode == 13) {
                float t[3] = {b[0], b[1], b[2]};

                setSaturation(t, saturation(c));
                setLuminosity(t, luminosity(b));
                memcpy(mixed, t, sizeof(mixed));
            } else if (mode == 14) {
                float t[3] = {c[0], c[1], c[2]};

                setLuminosity(t, luminosity(b));
                memcpy(mixed, t, sizeof(mixed));
            } else {
                float t[3] = {b[0], b[1], b[2]};

                setLuminosity(t, luminosity(c));
                memcpy(mixed, t, sizeof(mixed));
            }
        }

        for (int k = 0; k < 3; k++) {
            out[k] = src[k] * (1.f - ab) + dst[k] * (1.f - as) + as * ab * clampUnit(mixed[k]);
        }

        out[3] = as + ab - as * ab;
    }

    static void filterBlend(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Document& doc = *run.filter->doc;
        const float* a = filterInputBuffer(run, p.in1, p.linear);
        const float* b = filterInputBuffer(run, p.in2, p.linear);
        u32 modeValue = attribute(p.node, AtomMode);
        int mode = modeValue ? blendMode(trim(textOf(doc, modeValue))) : 0;
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;

                blendPixel(mode, a + i, b + i, out + i);
            }
        }
    }

    static void filterMerge(FilterRun& run, const FilterPrimitive& p, float* out) {
        const IntBox& box = p.box;

        for (u32 k = 0; k < p.inputCount; k++) {
            const float* in = filterInputBuffer(run, p.inputs[k], p.linear);

            for (int y = box.y0; y < box.y1; y++) {
                for (int x = box.x0; x < box.x1; x++) {
                    size_t i = ((size_t)y * run.width + x) * 4;
                    const float* s = in + i;
                    float* d = out + i;
                    float keep = 1.f - s[3];

                    d[0] = s[0] + d[0] * keep;
                    d[1] = s[1] + d[1] * keep;
                    d[2] = s[2] + d[2] * keep;
                    d[3] = s[3] + d[3] * keep;
                }
            }
        }
    }

    static void filterMorphology(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        u32 op = attribute(p.node, AtomOperator);
        bool dilate = op && trim(textOf(*f.doc, op)) == StringView(u8"dilate");
        float rx = 0.f;
        float ry = 0.f;

        bool valid = !attribute(p.node, AtomRadius) || numberPair(*f.doc, p.node, AtomRadius, rx, ry);

        if (!valid || rx < 0.f || ry < 0.f || (rx == 0.f && ry == 0.f)) {
            copyBox(run, in, out, p.box);

            return;
        }

        int ix = (int)floorf(userX(f, rx) * run.scaleX);
        int iy = (int)floorf(userY(f, ry) * run.scaleY);
        int height = (int)run.raster->height;
        float* pass = filterBuffer(run);
        const IntBox& box = p.box;

        for (int y = 0; y < height; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                float best[4] = {dilate ? 0.f : 1.f, dilate ? 0.f : 1.f, dilate ? 0.f : 1.f, dilate ? 0.f : 1.f};

                for (int k = x - ix; k <= x + ix; k++) {
                    const float* s = k >= 0 && k < run.width ? in + ((size_t)y * run.width + k) * 4 : nullptr;

                    for (int c = 0; c < 4; c++) {
                        float v = s ? s[c] : 0.f;

                        best[c] = dilate ? maxf(best[c], v) : minf(best[c], v);
                    }
                }

                memcpy(pass + ((size_t)y * run.width + x) * 4, best, sizeof(best));
            }
        }

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                float best[4] = {dilate ? 0.f : 1.f, dilate ? 0.f : 1.f, dilate ? 0.f : 1.f, dilate ? 0.f : 1.f};

                for (int k = y - iy; k <= y + iy; k++) {
                    const float* s = k >= 0 && k < height ? pass + ((size_t)k * run.width + x) * 4 : nullptr;

                    for (int c = 0; c < 4; c++) {
                        float v = s ? s[c] : 0.f;

                        best[c] = dilate ? maxf(best[c], v) : minf(best[c], v);
                    }
                }

                memcpy(out + ((size_t)y * run.width + x) * 4, best, sizeof(best));
            }
        }
    }

    static void filterTile(FilterRun& run, const FilterPrimitive& p, float* out) {
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        IntBox tile = inputBox(run, p.in1);

        if (p.in1 >= 0) {
            Box exact = deviceBox(run.filter->ctm, run.filter->primitives[p.in1].user);
            IntBox inner{(int)ceilf(exact.x0 - 1e-3f), (int)ceilf(exact.y0 - 1e-3f), (int)floorf(exact.x1 + 1e-3f), (int)floorf(exact.y1 + 1e-3f)};

            if (inner.x0 >= tile.x0 && inner.y0 >= tile.y0 && inner.x1 <= tile.x1 && inner.y1 <= tile.y1 && inner.x1 > inner.x0 && inner.y1 > inner.y0) {
                tile = inner;
            }
        }

        int tw = tile.x1 - tile.x0;
        int th = tile.y1 - tile.y0;
        const IntBox& box = p.box;

        if (tw <= 0 || th <= 0) {
            return;
        }

        for (int y = box.y0; y < box.y1; y++) {
            int sy = tile.y0 + (((y - tile.y0) % th) + th) % th;

            for (int x = box.x0; x < box.x1; x++) {
                int sx = tile.x0 + (((x - tile.x0) % tw) + tw) % tw;

                memcpy(out + ((size_t)y * run.width + x) * 4, in + ((size_t)sy * run.width + sx) * 4, sizeof(float) * 4);
            }
        }
    }

    static void filterConvolve(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Document& doc = *run.filter->doc;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        float ox = 3.f;
        float oy = 3.f;

        if (attribute(p.node, AtomOrder) && !numberPair(doc, p.node, AtomOrder, ox, oy)) {
            return;
        }

        int orderX = (int)ox;
        int orderY = (int)oy;

        if ((float)orderX != ox || (float)orderY != oy || orderX < 1 || orderY < 1 || orderX > 32 || orderY > 32) {
            return;
        }

        Vector<float> kernel;

        if (!numberList(doc, p.node, AtomKernelMatrix, kernel) || kernel.length() != (size_t)(orderX * orderY)) {
            return;
        }

        float sum = 0.f;

        for (float k : kernel) {
            sum += k;
        }

        float divisor = numberOf(doc, p.node, AtomDivisor, sum == 0.f ? 1.f : sum);

        if (divisor == 0.f) {
            divisor = 1.f;
        }

        float bias = numberOf(doc, p.node, AtomBias, 0.f);
        int targetX = (int)numberOf(doc, p.node, AtomTargetX, (float)(orderX / 2));
        int targetY = (int)numberOf(doc, p.node, AtomTargetY, (float)(orderY / 2));

        if (targetX < 0 || targetX >= orderX || targetY < 0 || targetY >= orderY) {
            return;
        }

        u32 edgeValue = attribute(p.node, AtomEdgeMode);
        StringView edge = edgeValue ? trim(textOf(doc, edgeValue)) : StringView(u8"duplicate");
        u32 preserveValue = attribute(p.node, AtomPreserveAlpha);
        bool preserve = preserveValue && trim(textOf(doc, preserveValue)) == StringView(u8"true");
        IntBox src = inputBox(run, p.in1);
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                float acc[4] = {0.f, 0.f, 0.f, 0.f};

                for (int j = 0; j < orderY; j++) {
                    for (int i = 0; i < orderX; i++) {
                        int sx = x - targetX + i;
                        int sy = y - targetY + j;
                        bool inside = sx >= src.x0 && sx < src.x1 && sy >= src.y0 && sy < src.y1;

                        if (!inside) {
                            if (edge == StringView(u8"none")) {
                                continue;
                            }

                            if (edge == StringView(u8"wrap")) {
                                int w = src.x1 - src.x0;
                                int h = src.y1 - src.y0;

                                if (w <= 0 || h <= 0) {
                                    continue;
                                }

                                sx = src.x0 + (((sx - src.x0) % w) + w) % w;
                                sy = src.y0 + (((sy - src.y0) % h) + h) % h;
                            } else {
                                sx = sx < src.x0 ? src.x0 : (sx >= src.x1 ? src.x1 - 1 : sx);
                                sy = sy < src.y0 ? src.y0 : (sy >= src.y1 ? src.y1 - 1 : sy);
                            }
                        }

                        if (sx < 0 || sy < 0 || sx >= run.width || sy >= (int)run.raster->height) {
                            continue;
                        }

                        float k = kernel[(size_t)(orderY - 1 - j) * orderX + (orderX - 1 - i)];
                        const float* s = in + ((size_t)sy * run.width + sx) * 4;

                        if (preserve) {
                            float c[4];

                            unpremultiply(s, c);

                            for (int ch = 0; ch < 3; ch++) {
                                acc[ch] += c[ch] * k;
                            }
                        } else {
                            for (int ch = 0; ch < 4; ch++) {
                                acc[ch] += s[ch] * k;
                            }
                        }
                    }
                }

                float* o = out + ((size_t)y * run.width + x) * 4;

                if (preserve) {
                    float a = in[((size_t)y * run.width + x) * 4 + 3];
                    float c[4] = {acc[0] / divisor + bias, acc[1] / divisor + bias, acc[2] / divisor + bias, a};

                    storePremultiplied(o, c);
                } else {
                    float a = clampUnit(acc[3] / divisor + bias);

                    for (int ch = 0; ch < 3; ch++) {
                        o[ch] = minf(clampUnit(acc[ch] / divisor + bias * a), a);
                    }

                    o[3] = a;
                }
            }
        }
    }

    struct Turbulence {
        int lattice[0x100 + 0x100 + 2];
        double gradient[4][0x100 + 0x100 + 2][2];
    };

    struct Stitch {
        int width;
        int height;
        int wrapX;
        int wrapY;
    };

    static long turbulenceRandom(long seed) {
        long result = 16807 * (seed % 127773) - 2836 * (seed / 127773);

        return result <= 0 ? result + 2147483647 : result;
    }

    static void turbulenceInit(Turbulence& t, long seed) {
        if (seed <= 0) {
            seed = -(seed % (2147483647 - 1)) + 1;
        }

        if (seed > 2147483647 - 1) {
            seed = 2147483647 - 1;
        }

        int i;
        int j;
        int k;

        for (k = 0; k < 4; k++) {
            for (i = 0; i < 0x100; i++) {
                t.lattice[i] = i;

                for (j = 0; j < 2; j++) {
                    seed = turbulenceRandom(seed);
                    t.gradient[k][i][j] = (double)((seed % (0x100 + 0x100)) - 0x100) / 0x100;
                }

                double s = sqrt(t.gradient[k][i][0] * t.gradient[k][i][0] + t.gradient[k][i][1] * t.gradient[k][i][1]);

                t.gradient[k][i][0] /= s;
                t.gradient[k][i][1] /= s;
            }
        }

        while (--i) {
            k = t.lattice[i];
            seed = turbulenceRandom(seed);
            j = (int)(seed % 0x100);
            t.lattice[i] = t.lattice[j];
            t.lattice[j] = k;
        }

        for (i = 0; i < 0x100 + 2; i++) {
            t.lattice[0x100 + i] = t.lattice[i];

            for (k = 0; k < 4; k++) {
                for (j = 0; j < 2; j++) {
                    t.gradient[k][0x100 + i][j] = t.gradient[k][i][j];
                }
            }
        }
    }

    static double sCurve(double t) {
        return t * t * (3. - 2. * t);
    }

    static double lerp(double t, double a, double b) {
        return a + t * (b - a);
    }

    static double noise2(const Turbulence& tb, int channel, double vx, double vy, const Stitch* stitch) {
        double t = vx + 0x1000;
        int bx0 = ((int)t) & 0xff;
        int bx1 = (bx0 + 1) & 0xff;
        double rx0 = t - (int)t;
        double rx1 = rx0 - 1.0;

        t = vy + 0x1000;

        int by0 = ((int)t) & 0xff;
        int by1 = (by0 + 1) & 0xff;
        double ry0 = t - (int)t;
        double ry1 = ry0 - 1.0;

        if (stitch) {
            if (bx0 >= stitch->wrapX) {
                bx0 -= stitch->width;
            }

            if (bx1 >= stitch->wrapX) {
                bx1 -= stitch->width;
            }

            if (by0 >= stitch->wrapY) {
                by0 -= stitch->height;
            }

            if (by1 >= stitch->wrapY) {
                by1 -= stitch->height;
            }
        }

        bx0 &= 0xff;
        bx1 &= 0xff;
        by0 &= 0xff;
        by1 &= 0xff;

        int i = tb.lattice[bx0];
        int j = tb.lattice[bx1];
        int b00 = tb.lattice[i + by0];
        int b10 = tb.lattice[j + by0];
        int b01 = tb.lattice[i + by1];
        int b11 = tb.lattice[j + by1];
        double sx = sCurve(rx0);
        double sy = sCurve(ry0);
        const double* q = tb.gradient[channel][b00];
        double u = rx0 * q[0] + ry0 * q[1];

        q = tb.gradient[channel][b10];

        double v = rx1 * q[0] + ry0 * q[1];
        double a = lerp(sx, u, v);

        q = tb.gradient[channel][b01];
        u = rx0 * q[0] + ry1 * q[1];
        q = tb.gradient[channel][b11];
        v = rx1 * q[0] + ry1 * q[1];

        double b = lerp(sx, u, v);

        return lerp(sy, a, b);
    }

    static double turbulenceAt(const Turbulence& tb, int channel, double px, double py, double fx, double fy, int octaves, bool fractal, bool stitching, double tileX, double tileY, double tileW, double tileH) {
        Stitch stitch;
        Stitch* active = nullptr;

        if (stitching) {
            if (fx != 0.0) {
                double low = floor(tileW * fx) / tileW;
                double high = ceil(tileW * fx) / tileW;

                fx = fx / low < high / fx ? low : high;
            }

            if (fy != 0.0) {
                double low = floor(tileH * fy) / tileH;
                double high = ceil(tileH * fy) / tileH;

                fy = fy / low < high / fy ? low : high;
            }

            active = &stitch;
            stitch.width = (int)(tileW * fx + 0.5);
            stitch.wrapX = (int)(tileX * fx + 0x1000 + stitch.width);
            stitch.height = (int)(tileH * fy + 0.5);
            stitch.wrapY = (int)(tileY * fy + 0x1000 + stitch.height);
        }

        double sum = 0.0;
        double vx = px * fx;
        double vy = py * fy;
        double ratio = 1.0;

        for (int octave = 0; octave < octaves; octave++) {
            double n = noise2(tb, channel, vx, vy, active);

            sum += fractal ? n / ratio : fabs(n) / ratio;
            vx *= 2;
            vy *= 2;
            ratio *= 2;

            if (active) {
                stitch.width *= 2;
                stitch.wrapX = 2 * stitch.wrapX - 0x1000;
                stitch.height *= 2;
                stitch.wrapY = 2 * stitch.wrapY - 0x1000;
            }
        }

        return sum;
    }

    static void filterTurbulence(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        float fx = 0.f;
        float fy = 0.f;

        if (attribute(p.node, AtomBaseFrequency) && !numberPair(doc, p.node, AtomBaseFrequency, fx, fy)) {
            return;
        }

        if (fx < 0.f || fy < 0.f) {
            return;
        }

        int octaves = (int)numberOf(doc, p.node, AtomNumOctaves, 1.f);
        long seed = (long)numberOf(doc, p.node, AtomSeed, 0.f);
        u32 type = attribute(p.node, AtomType);
        bool fractal = type && trim(textOf(doc, type)) == StringView(u8"fractalNoise");
        u32 stitchValue = attribute(p.node, AtomStitchTiles);
        bool stitching = stitchValue && trim(textOf(doc, stitchValue)) == StringView(u8"stitch");
        Matrix inverse;

        if (!invert(f.ctm, inverse)) {
            return;
        }

        Turbulence* tb = (Turbulence*)run.pool->allocate(sizeof(Turbulence));

        turbulenceInit(*tb, seed);

        const IntBox& box = p.box;
        float tileX = p.user.x0;
        float tileY = p.user.y0;
        float tileW = p.user.x1 - p.user.x0;
        float tileH = p.user.y1 - p.user.y0;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                Point point = apply(inverse, Point{(float)x + 0.5f, (float)y + 0.5f});
                float c[4];

                for (int channel = 0; channel < 4; channel++) {
                    double v = turbulenceAt(*tb, channel, point.x, point.y, fx, fy, octaves, fractal, stitching, tileX, tileY, tileW, tileH);

                    c[channel] = (float)(fractal ? (v + 1.0) / 2.0 : v);
                }

                storePremultiplied(out + ((size_t)y * run.width + x) * 4, c);
            }
        }
    }

    static void filterDisplacement(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        const float* map = filterInputBuffer(run, p.in2, p.linear);
        float scale = numberOf(doc, p.node, AtomScale, 0.f);
        float sx = userX(f, scale) * run.scaleX;
        float sy = userY(f, scale) * run.scaleY;
        auto channel = [&](u32 name) {
            u32 value = attribute(p.node, name);
            StringView text = value ? trim(textOf(doc, value)) : StringView(u8"A");

            return text == StringView(u8"R") ? 0 : (text == StringView(u8"G") ? 1 : (text == StringView(u8"B") ? 2 : 3));
        };
        int cx = channel(AtomXChannelSelector);
        int cy = channel(AtomYChannelSelector);
        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                float m[4];

                unpremultiply(map + i, m);

                int tx = (int)floorf((float)x + sx * (m[cx] - 0.5f) + 0.5f);
                int ty = (int)floorf((float)y + sy * (m[cy] - 0.5f) + 0.5f);

                if (tx < 0 || ty < 0 || tx >= run.width || ty >= (int)run.raster->height) {
                    continue;
                }

                memcpy(out + i, in + ((size_t)ty * run.width + tx) * 4, sizeof(float) * 4);
            }
        }
    }

    struct Vector3 {
        float x;
        float y;
        float z;
    };

    static Vector3 normalize3(Vector3 v) {
        float length = sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);

        return length > 0.f ? Vector3{v.x / length, v.y / length, v.z / length} : Vector3{0.f, 0.f, 0.f};
    }

    static float dot3(Vector3 a, Vector3 b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    static void filterLighting(FilterRun& run, const FilterPrimitive& p, float* out) {
        const Filter& f = *run.filter;
        const Document& doc = *f.doc;
        const float* in = filterInputBuffer(run, p.in1, p.linear);
        bool specular = p.kind == AtomFeSpecularLighting;
        float surfaceScale = numberOf(doc, p.node, AtomSurfaceScale, 1.f) * sqrtf(fabsf(f.ctm.a * f.ctm.d - f.ctm.b * f.ctm.c));
        float constant = numberOf(doc, p.node, specular ? AtomSpecularConstant : AtomDiffuseConstant, 1.f);
        float shininess = numberOf(doc, p.node, AtomSpecularExponent, 1.f);
        Node* light = nullptr;

        for (Node* child = p.node->first; child; child = child->next) {
            if (child->name == AtomFeDistantLight || child->name == AtomFePointLight || child->name == AtomFeSpotLight) {
                light = child;
                break;
            }
        }

        if (!light) {
            return;
        }

        constant = constant < 0.f ? 0.f : constant;
        shininess = shininess < 1.f ? 1.f : (shininess > 128.f ? 128.f : shininess);

        Color lightColor = toSpace(p.lighting, p.linear);
        float bw = f.bbox.x1 - f.bbox.x0;
        float bh = f.bbox.y1 - f.bbox.y0;
        auto position = [&](u32 nx, u32 ny, u32 nz) {
            float x = numberOf(doc, light, nx, 0.f);
            float y = numberOf(doc, light, ny, 0.f);
            float z = numberOf(doc, light, nz, 0.f);

            if (f.boxPrimitives) {
                x = f.bbox.x0 + x * bw;
                y = f.bbox.y0 + y * bh;
                z *= sqrtf((bw * bw + bh * bh) / 2.f);
            }

            Point mapped = apply(f.ctm, Point{x, y});
            float zx = f.ctm.a * z + f.ctm.c * z;
            float zy = f.ctm.b * z + f.ctm.d * z;

            return Vector3{mapped.x, mapped.y, (zx + zy) / 2.f};
        };
        Vector3 lightPosition{0.f, 0.f, 0.f};
        Vector3 direction{0.f, 0.f, 0.f};
        Vector3 spotDirection{0.f, 0.f, 0.f};
        float spotExponent = 1.f;
        float cosOuter = -1.f;
        float cosInner = -1.f;
        bool limited = false;

        if (light->name == AtomFeDistantLight) {
            float azimuth = numberOf(doc, light, AtomAzimuth, 0.f) * pi / 180.f;
            float elevation = numberOf(doc, light, AtomElevation, 0.f) * pi / 180.f;

            direction = Vector3{cosf(azimuth) * cosf(elevation), sinf(azimuth) * cosf(elevation), sinf(elevation)};
        } else {
            lightPosition = position(AtomX, AtomY, AtomZ);

            if (light->name == AtomFeSpotLight) {
                Vector3 target = position(AtomPointsAtX, AtomPointsAtY, AtomPointsAtZ);

                spotDirection = normalize3(Vector3{target.x - lightPosition.x, target.y - lightPosition.y, target.z - lightPosition.z});
                spotExponent = numberOf(doc, light, AtomSpecularExponent, 1.f);
                spotExponent = spotExponent < 1.f ? 1.f : (spotExponent > 128.f ? 128.f : spotExponent);

                if (attribute(light, AtomLimitingConeAngle) && numberOf(doc, light, AtomLimitingConeAngle, 0.f) != 0.f) {
                    float angle = fabsf(numberOf(doc, light, AtomLimitingConeAngle, 90.f));

                    limited = true;
                    cosOuter = cosf(angle * pi / 180.f);
                    cosInner = cosOuter + 0.016f;
                }
            }
        }

        const IntBox& box = p.box;
        int height = (int)run.raster->height;
        auto alphaAt = [&](int x, int y) {
            x = x < box.x0 ? box.x0 : (x >= box.x1 ? box.x1 - 1 : x);
            y = y < box.y0 ? box.y0 : (y >= box.y1 ? box.y1 - 1 : y);

            if (x < 0 || y < 0 || x >= run.width || y >= height) {
                return 0.f;
            }

            return in[((size_t)y * run.width + x) * 4 + 3];
        };

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                bool left = x == box.x0;
                bool right = x == box.x1 - 1;
                bool top = y == box.y0;
                bool bottom = y == box.y1 - 1;
                float nx;
                float ny;
                float factorX;
                float factorY;
                float a00 = alphaAt(x - 1, y - 1);
                float a01 = alphaAt(x, y - 1);
                float a02 = alphaAt(x + 1, y - 1);
                float a10 = alphaAt(x - 1, y);
                float a11 = alphaAt(x, y);
                float a12 = alphaAt(x + 1, y);
                float a20 = alphaAt(x - 1, y + 1);
                float a21 = alphaAt(x, y + 1);
                float a22 = alphaAt(x + 1, y + 1);

                if (box.x1 - box.x0 == 1 || box.y1 - box.y0 == 1) {
                    nx = 0.f;
                    ny = 0.f;
                    factorX = 0.f;
                    factorY = 0.f;
                } else if (top && left) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -2.f * a11 + 2.f * a12 - a21 + a22;
                    ny = -2.f * a11 - a12 + 2.f * a21 + a22;
                } else if (top && right) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -2.f * a10 + 2.f * a11 - a20 + a21;
                    ny = -a10 - 2.f * a11 + a20 + 2.f * a21;
                } else if (bottom && left) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -a01 + a02 - 2.f * a11 + 2.f * a12;
                    ny = -2.f * a01 - a02 + 2.f * a11 + a12;
                } else if (bottom && right) {
                    factorX = 2.f / 3.f;
                    factorY = 2.f / 3.f;
                    nx = -a00 + a01 - 2.f * a10 + 2.f * a11;
                    ny = -a00 - 2.f * a01 + a10 + 2.f * a11;
                } else if (top) {
                    factorX = 1.f / 3.f;
                    factorY = 1.f / 2.f;
                    nx = -2.f * a10 + 2.f * a12 - a20 + a22;
                    ny = -a10 - 2.f * a11 - a12 + a20 + 2.f * a21 + a22;
                } else if (bottom) {
                    factorX = 1.f / 3.f;
                    factorY = 1.f / 2.f;
                    nx = -a00 + a02 - 2.f * a10 + 2.f * a12;
                    ny = -a00 - 2.f * a01 - a02 + a10 + 2.f * a11 + a12;
                } else if (left) {
                    factorX = 1.f / 2.f;
                    factorY = 1.f / 3.f;
                    nx = -a01 + a02 - 2.f * a11 + 2.f * a12 - a21 + a22;
                    ny = -2.f * a01 - a02 + 2.f * a21 + a22;
                } else if (right) {
                    factorX = 1.f / 2.f;
                    factorY = 1.f / 3.f;
                    nx = -a00 + a01 - 2.f * a10 + 2.f * a11 - a20 + a21;
                    ny = -a00 - 2.f * a01 + a20 + 2.f * a21;
                } else {
                    factorX = 1.f / 4.f;
                    factorY = 1.f / 4.f;
                    nx = -a00 + a02 - 2.f * a10 + 2.f * a12 - a20 + a22;
                    ny = -a00 - 2.f * a01 - a02 + a20 + 2.f * a21 + a22;
                }

                Vector3 normal = normalize3(Vector3{-surfaceScale * factorX * nx, -surfaceScale * factorY * ny, 1.f});
                Vector3 toLight = direction;
                Color color = lightColor;

                if (light->name != AtomFeDistantLight) {
                    toLight = normalize3(Vector3{lightPosition.x - (float)x, lightPosition.y - (float)y, lightPosition.z - surfaceScale * a11});
                }

                if (light->name == AtomFeSpotLight) {
                    float cosAngle = -dot3(toLight, spotDirection);
                    float s = 0.f;

                    if (!limited || cosAngle >= cosOuter) {
                        s = cosAngle > 0.f ? powf(cosAngle, spotExponent) : 0.f;

                        if (limited && cosAngle < cosInner) {
                            s *= (cosAngle - cosOuter) / 0.016f;
                        }
                    }

                    color = Color{color.r * s, color.g * s, color.b * s, 1.f};
                }

                float factor;

                if (specular) {
                    Vector3 half = normalize3(Vector3{toLight.x, toLight.y, toLight.z + 1.f});
                    float nh = dot3(normal, half);

                    factor = constant * powf(maxf(nh, 0.f), shininess);
                } else {
                    factor = constant * dot3(normal, toLight);
                }

                float r = clampUnit(color.r * factor);
                float g = clampUnit(color.g * factor);
                float b = clampUnit(color.b * factor);
                float* o = out + ((size_t)y * run.width + x) * 4;

                if (specular) {
                    float a = maxf(r, maxf(g, b));

                    o[0] = r;
                    o[1] = g;
                    o[2] = b;
                    o[3] = a;
                } else {
                    o[0] = r;
                    o[1] = g;
                    o[2] = b;
                    o[3] = 1.f;
                }
            }
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, float* base);

    static void filterImage(FilterRun& run, const FilterPrimitive& p, float* out) {
        float* content = filterBuffer(run);

        runProgram(*run.raster, *run.lowering, run.lowering->programs.data() + p.programFirst, p.programCount, content);

        const IntBox& box = p.box;

        for (int y = box.y0; y < box.y1; y++) {
            for (int x = box.x0; x < box.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;
                float* o = out + i;
                const float* s = content + i;

                if (p.linear) {
                    float c[4];

                    unpremultiply(s, c);

                    for (int k = 0; k < 3; k++) {
                        c[k] = srgbToLinear(c[k]);
                    }

                    storePremultiplied(o, c);
                } else {
                    memcpy(o, s, sizeof(float) * 4);
                }
            }
        }
    }

    static float* applyFilter(Raster& r, const Lowering& l, const Filter& filter, float* layer, ObjPool& pool) {
        FilterRun run;

        run.raster = &r;
        run.lowering = &l;
        run.filter = &filter;
        run.pool = &pool;
        run.size = (size_t)r.width * r.height;
        run.width = (int)r.width;
        run.scaleX = sqrtf(filter.ctm.a * filter.ctm.a + filter.ctm.b * filter.ctm.b);
        run.scaleY = sqrtf(filter.ctm.c * filter.ctm.c + filter.ctm.d * filter.ctm.d);
        run.source = layer;
        run.sourceLinear = nullptr;
        run.empty = filterBuffer(run);
        run.sourceAlpha = filterBuffer(run);

        float* result = filterBuffer(run);
        const IntBox& region = filter.region;

        if (filter.count == 0 || region.x1 <= region.x0 || region.y1 <= region.y0) {
            return result;
        }

        for (int y = region.y0; y < region.y1; y++) {
            for (int x = region.x0; x < region.x1; x++) {
                size_t i = ((size_t)y * run.width + x) * 4;

                run.sourceAlpha[i + 3] = layer[i + 3];
            }
        }

        run.linearCache.zero(filter.count);
        run.srgbCache.zero(filter.count);

        for (u32 k = 0; k < filter.count; k++) {
            const FilterPrimitive& p = filter.primitives[k];
            float* out = filterBuffer(run);

            switch (p.kind) {
                case AtomFeFlood:
                    filterFlood(run, p, out);
                    break;
                case AtomFeOffset:
                    filterOffset(run, p, out);
                    break;
                case AtomFeGaussianBlur:
                    filterBlur(run, p, out);
                    break;
                case AtomFeDropShadow:
                    filterDropShadow(run, p, out);
                    break;
                case AtomFeColorMatrix:
                    filterColorMatrix(run, p, out);
                    break;
                case AtomFeComponentTransfer:
                    filterComponentTransfer(run, p, out);
                    break;
                case AtomFeComposite:
                    filterComposite(run, p, out);
                    break;
                case AtomFeBlend:
                    filterBlend(run, p, out);
                    break;
                case AtomFeMerge:
                    filterMerge(run, p, out);
                    break;
                case AtomFeMorphology:
                    filterMorphology(run, p, out);
                    break;
                case AtomFeTile:
                    filterTile(run, p, out);
                    break;
                case AtomFeConvolveMatrix:
                    filterConvolve(run, p, out);
                    break;
                case AtomFeTurbulence:
                    filterTurbulence(run, p, out);
                    break;
                case AtomFeDisplacementMap:
                    filterDisplacement(run, p, out);
                    break;
                case AtomFeDiffuseLighting:
                case AtomFeSpecularLighting:
                    filterLighting(run, p, out);
                    break;
                case AtomFeImage:
                    filterImage(run, p, out);
                    break;
                default:
                    break;
            }

            run.results.pushBack(FilterResult{out, p.linear});
        }

        const FilterResult& last = run.results.back();
        const float* final = last.linear ? convertBuffer(run, last.data, false) : last.data;

        for (int y = region.y0; y < region.y1; y++) {
            size_t i = ((size_t)y * run.width + region.x0) * 4;

            memcpy(result + i, final + i, sizeof(float) * 4 * (size_t)(region.x1 - region.x0));
        }

        return result;
    }

    static void compositeLayer(Raster& r, const Lowering& l, const Op& op, float* target) {
        Raster sub;
        ObjPool::Ref pool = ObjPool::fromMemory();
        size_t size = (size_t)op.layerWidth * op.layerHeight;
        float* content = (float*)pool->allocate(sizeof(float) * size * 4);
        Matrix toDevice;

        if (!invert(op.layerMatrix, toDevice)) {
            return;
        }

        sub.width = op.layerWidth;
        sub.height = op.layerHeight;
        memset(content, 0, sizeof(float) * size * 4);
        runProgram(sub, l, l.programs.data() + op.first, op.count, content);

        for (const Filter* filter = op.filter; filter; filter = filter->next) {
            content = applyFilter(sub, l, *filter, content, *pool);
        }

        int lw = (int)op.layerWidth;
        int lh = (int)op.layerHeight;
        auto texel = [&](int x, int y, int c) {
            return x < 0 || y < 0 || x >= lw || y >= lh ? 0.f : content[((size_t)y * lw + x) * 4 + c];
        };

        for (u32 y = 0; y < r.height; y++) {
            for (u32 x = 0; x < r.width; x++) {
                Point p = apply(op.layerMatrix, Point{(float)x + 0.5f, (float)y + 0.5f});
                float fx = p.x - 0.5f;
                float fy = p.y - 0.5f;
                float bx = floorf(fx);
                float by = floorf(fy);

                if (bx < -1.f || by < -1.f || bx >= (float)lw || by >= (float)lh) {
                    continue;
                }

                int ix = (int)bx;
                int iy = (int)by;
                float tx = fx - bx;
                float ty = fy - by;
                float sample[4];

                for (int c = 0; c < 4; c++) {
                    float top = texel(ix, iy, c) * (1.f - tx) + texel(ix + 1, iy, c) * tx;
                    float bottom = texel(ix, iy + 1, c) * (1.f - tx) + texel(ix + 1, iy + 1, c) * tx;

                    sample[c] = top * (1.f - ty) + bottom * ty;
                }

                if (sample[3] <= 0.f) {
                    continue;
                }

                float* d = target + ((size_t)y * r.width + x) * 4;
                float keep = 1.f - sample[3];

                for (int c = 0; c < 4; c++) {
                    d[c] = sample[c] + d[c] * keep;
                }
            }
        }
    }

    static void runProgram(Raster& r, const Lowering& l, const Op* ops, u32 count, float* base) {
        size_t size = (size_t)r.width * r.height;
        size_t depth = r.layers.length();

        r.layers.pushBack(base);

        for (u32 k = 0; k < count; k++) {
            const Op& op = ops[k];

            switch (op.kind) {
                case OpKind::Fill:
                    fillOp(r, l, op, r.layers.back());
                    break;
                case OpKind::Layer:
                    compositeLayer(r, l, op, r.layers.back());
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

                    for (const Filter* filter = begin->filter; filter; filter = filter->next) {
                        layer = applyFilter(r, l, *filter, layer, *pool);
                    }

                    const float* clip = begin->clipped ? clipBuffer(r, l, *begin, *pool) : nullptr;
                    const float* mask = begin->masked ? maskBuffer(r, l, *begin, *pool) : nullptr;

                    for (size_t i = 0; i < size; i++) {
                        float f = begin->opacity * (clip ? clip[i] : 1.f) * (mask ? mask[i] : 1.f);
                        const float* s = layer + i * 4;

                        if (f <= 0.f || s[3] <= 0.f) {
                            continue;
                        }

                        float* d = parent + i * 4;

                        if (begin->blend && d[3] > 0.f) {
                            float source[4] = {s[0] * f, s[1] * f, s[2] * f, s[3] * f};
                            float blended[4];

                            blendPixel(begin->blend, source, d, blended);
                            memcpy(d, blended, sizeof(blended));
                            continue;
                        }

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

        while (r.layers.length() > depth + 1) {
            r.layers.popBack();
            r.begins.popBack();
            delete r.pools.popBack();
        }

        r.layers.popBack();
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
        l.out = &l.ops;
        l.decoder = decoder;
        l.imageDepth = 0;
        l.sized = nullptr;
        l.hasContext = false;
        l.rootFontSize = 16.f;
        l.contextMatrix = identity();
        l.contextBox = emptyBox();
        l.instance = nullptr;
        l.documentStyles.zero(doc.nodeCount);
        lowerElement(l, doc.root, initialStyle(), identity(), Viewport{(float)width, (float)height, (float)width, (float)height, 16.f});

        Raster raster;

        raster.width = width;
        raster.height = height;
        runProgram(raster, l, l.ops.data(), (u32)l.ops.length(), base);
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

    cascade(doc);

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
