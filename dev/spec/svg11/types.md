<!-- https://www.w3.org/TR/SVG11/types.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](render.md) ⋅ [Next](struct.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 4 Basic Data Types and Interfaces

## <a id="syntax"></a>4.1 Syntax

The EBNF grammar is as used in the XML specification, with the addition of **~**, a *case-insensitive literal*: characters in the ASCII range (only) are declared to be case-insensitive. For example, ~"Hello" will match (H|h)(e|e)(l|L)(l|L)(o|O). This makes the productions much easier to read.

| ? | optional, zero or one |
| --- | --- |
| + | one or more |
| * | zero or more |
| \| | alternation |
| "string" | literal |
| ~"string" | case-insensitive literal |
| [] | a character range |
| [^] | excluded character range |
| () | grouping |

## <a id="BasicDataTypes"></a>4.2 Basic data types

This section defines a number of common data types used in the definitions of SVG properties and attributes. Some data types that are not referenced by multiple properties and attributes are defined inline in subsequent chapters.

Note that, as noted below, the specification of some types is different for CSS property values in style sheets (in the [‘style’](styling.md#StyleAttribute) attribute, [‘style’](styling.md#StyleElement) element or an external style sheet) than it is for for XML attribute values (including [presentation attribute](intro.md#TermPresentationAttribute)s). This is due to restrictions in the CSS grammar. For example, scientific notation is allowed in attributes, including presentation attributes, but not in style sheets.

**<a id="DataTypeAngle"></a><angle>**
:

Angles are specified in one of two ways depending upon whether they are used in CSS [property](intro.md#TermProperty) syntax or SVG [presentation attribute](intro.md#TermPresentationAttribute) syntax:

-
When an <angle> is used in a style sheet or with a [property](intro.md#TermProperty) in a [‘style’](styling.md#StyleAttribute) attribute, the syntax must match the following pattern:
```
<a id="Angle"></a>angle ::= number (~"deg" | ~"grad" | ~"rad")?
```

where deg indicates degrees, grad indicates grads and rad indicates radians. The unit identifier may be in lower (recommended) or upper case.
For properties defined in CSS2 [CSS2], an angle unit identifier must be provided (for non-zero values). For SVG-specific [properties](intro.md#TermProperty) the angle unit identifier is optional. If a unit is not provided, the angle value is assumed to be in degrees.
-
When an <angle> is used in an SVG [presentation attribute](intro.md#TermPresentationAttribute), the syntax must match the following pattern:
```
angle ::= number ("deg" | "grad" | "rad")?
```

The unit identifier, if present, must be in lower case; if not present, the angle value is assumed to be in degrees.

In the SVG DOM, <angle> values are represented using [SVGAngle](#InterfaceSVGAngle) or [SVGAnimatedAngle](#InterfaceSVGAnimatedAngle) objects.

**<a id="DataTypeAnything"></a><anything>**
:

The basic type <anything> is a sequence of zero or more characters. Specifically:

```
<a id="Anything"></a>anything ::= Char*
```

where Char is the production for a character, as defined in XML 1.0 ([[XML10](refs.md#ref-XML10)], section 2.2).

**<a id="DataTypeColor"></a><color>**
:

The basic type <color> is a CSS2 compatible specification for a color in the sRGB color space [[SRGB](refs.md#ref-SRGB)]. <color> applies to SVG's use of the [‘color’](color.md#ColorProperty) property and is a component of the definitions of properties [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘flood-color’](filters.md#FloodColorProperty) and [‘lighting-color’](filters.md#LightingColorProperty), which also offer optional ICC-based color specifications.

SVG supports all of the syntax alternatives for <color> defined in CSS2 syntax and basic data types ([[CSS2](refs.md#ref-CSS2)], section 4.3.6), with the exception that SVG allows an expanded list of [recognized color keywords names](#ColorKeywords).

A <color> is either a keyword (see [Recognized color keyword names](#ColorKeywords)) or a numerical RGB specification.

In addition to these color keywords, users may specify keywords that correspond to the colors used by objects in the user's environment. The normative definition of these keywords is found in User preferences for colors ([[CSS2](refs.md#ref-CSS2)], section 18.2).

The format of an RGB value in hexadecimal notation is a "#" immediately followed by either three or six hexadecimal characters. The three-digit RGB notation (#rgb) is converted into six-digit form (#rrggbb) by replicating digits, not by adding zeros. For example, #fb0 expands to #ffbb00. This ensures that white (#ffffff) can be specified with the short notation (#fff) and removes any dependencies on the color depth of the display. The format of an RGB value in the functional notation is an RGB start-function followed by a comma-separated list of three numerical values (either three integer values or three percentage values) followed by ")". An RGB start-function is the case-insensitive string "rgb(", for example "RGB(" or "rGb(". For compatibility, the all-lowercase form "rgb(" is preferred. The integer value 255 corresponds to 100%, and to F or FF in the hexadecimal notation: rgb(255,255,255) = rgb(100%,100%,100%) = #FFF. White space characters are allowed around the numerical values. All RGB colors are specified in the sRGB color space [[SRGB](refs.md#ref-SRGB)]. Using sRGB provides an unambiguous and objectively measurable definition of the color, which can be related to international standards (see [[COLORIMETRY](refs.md#ref-COLORIMETRY)]).

```
<a id="Color"></a>color    ::= "#" hexdigit hexdigit hexdigit (hexdigit hexdigit hexdigit)?
             | "rgb(" wsp* integer comma integer comma integer wsp* ")"
             | "rgb(" wsp* integer "%" comma integer "%" comma integer "%" wsp* ")"
             | color-keyword
<a id="HexDigit"></a>hexdigit ::= [0-9A-Fa-f]
<a id="Comma"></a>comma    ::= wsp* "," wsp*
    
```

where *color-keyword* matches (case insensitively) one of the color keywords listed in [Recognized color keyword names](#ColorKeywords) below, or one of the system color keywords listed in User preferences for colors ([[CSS2](refs.md#ref-CSS2)], section 18.2).

The corresponding SVG DOM interface definitions for <color> are defined in Document Object Model CSS; in particular, see RGBColor ([[DOM2STYLE](refs.md#ref-DOM2STYLE)], section 2.2). SVG's extension to color, including the ability to specify ICC-based colors, are represented using DOM interface [SVGColor](#InterfaceSVGColor).

**<a id="DataTypeCoordinate"></a><coordinate>**
:

A <coordinate> is a length in the user coordinate system that is the given distance from the origin of the user coordinate system along the relevant axis (the x-axis for X coordinates, the y-axis for Y coordinates). Its syntax is the same as that for [<length>](#DataTypeLength).

```
coordinate ::= length
```

Within the SVG DOM, a <coordinate> is represented as an [SVGLength](#InterfaceSVGLength) or an [SVGAnimatedLength](#InterfaceSVGAnimatedLength).

**<a id="DataTypeFrequency"></a><frequency>**
:

Frequency values are used with aural properties. As defined in CSS2, a frequency value is a <number> immediately followed by a frequency unit identifier. The frequency unit identifiers are:

- Hz: Hertz
- kHz: kilo Hertz

Frequency values may not be negative.

In the SVG DOM, <frequency> values are represented using the CSSPrimitiveValue interface defined in Document Object Model CSS ([[DOM2STYLE](refs.md#ref-DOM2STYLE)], section 2.2).

**<a id="DataTypeFuncIRI"></a><FuncIRI>**
: Functional notation for an [IRI](intro.md#TermIRIReference): "url(" [<IRI>](#DataTypeIRI) ")".
**<a id="DataTypeICCColor"></a><icccolor>**
:

An <icccolor> is an ICC color specification. In SVG 1.1, an ICC color specification is given by a name, which references a [‘color-profile’](color.md#ColorProfileElement) element, and one or more color component values. The grammar is as follows:

```
<a id="ICCColor"></a>icccolor ::= "icc-color(" name (comma-wsp number)+ ")"
name     ::= [^,()#x20#x9#xD#xA] /* any char except ",", "(", ")" or wsp */
```

The corresponding SVG DOM interface for <icccolor> is [SVGICCColor](#InterfaceSVGICCColor).

**<a id="DataTypeInteger"></a><integer>**
:

An <integer> is specified as an optional sign character ("+" or "-") followed by one or more digits "0" to "9":

```
<a id="Integer"></a>integer ::= [+-]? [0-9]+
```

If the sign character is not present, the number is non-negative.

Unless stated otherwise for a particular attribute or [property](intro.md#TermProperty), the range for an <integer> encompasses (at a minimum) -2147483648 to 2147483647.

Within the SVG DOM, an <integer> is represented as a long or an [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger).

**<a id="DataTypeIRI"></a><IRI>**
:

An Internationalized Resource Identifier (see [IRI](intro.md#TermIRIReference)). For the specification of [IRI](intro.md#TermIRIReference) references in SVG, see [IRI references](linking.md#IRIReference).

**<a id="DataTypeLength"></a><length>**
:

A length is a distance measurement, given as a number along with a unit which may be optional. Lengths are specified in one of two ways depending upon whether they are used in CSS [property](intro.md#TermProperty) syntax or SVG [presentation attribute](intro.md#TermPresentationAttribute) syntax:

-
When a <length> is used in a style sheet or with a [property](intro.md#TermProperty) in a [‘style’](styling.md#StyleAttribute) attribute, the syntax must match the following pattern:
```
<a id="Length"></a>length ::= number (~"em" | ~"ex" | ~"px" | ~"in" | ~"cm" | ~"mm" | ~"pt" | ~"pc")?
```

See the CSS2 specification for the meanings of the unit identifiers. The unit identifier may be in lower (recommended) or upper case.
For properties defined in CSS2 [CSS2], a length unit identifier must be provided (for non-zero values). For SVG-specific [properties](intro.md#TermProperty), the length unit identifier is optional. If a unit is not provided, the length value represents a distance in the current user coordinate system.
-
When a <length> is used in an SVG [presentation attribute](intro.md#TermPresentationAttribute), the syntax must match the following pattern:
```
length ::= number ("em" | "ex" | "px" | "in" | "cm" | "mm" | "pt" | "pc" | "%")?
```

The unit identifier, if present, must be in lower case; if not present, the length value represents a distance in the current user coordinate system.
Note that the non-property <length> definition also allows a percentage unit identifier. The meaning of a percentage length value depends on the attribute for which the percentage length value has been specified. Two common cases are: (a) when a percentage length value represents a percentage of the viewport width or height (refer to [the section that discusses units in general](coords.md#Units)), and (b) when a percentage length value represents a percentage of the bounding box width or height on a given object (refer to [the section that describes object bounding box units](coords.md#ObjectBoundingBox)).

In the SVG DOM, <length> values are represented using [SVGLength](#InterfaceSVGLength) or [SVGAnimatedLength](#InterfaceSVGAnimatedLength) objects.

**<a id="DataTypeListOfFamilyNames"></a><list-of-family-names>**
:

A <list-of-family-names> is a list of font family names using the same syntax as the [‘font-family’](text.md#FontFamilyProperty) property, excluding the <generic-family> and 'inherit' values.

**<a id="DataTypeListOfStrings"></a><list-of-strings>**
:

A <list-of-strings> consists of a separated sequence of <string>s. String lists are white space-separated, where white space is defined as one or more of the following consecutive characters: "space" (U+0020), "tab" (U+0009), "line feed" (U+000A) and "carriage return" (U+000D).

The following is an EBNF grammar describing the <list-of-strings> syntax:

```
list-of-strings ::= string
                    | string wsp list-of-strings
string          ::= [^#x9#xA#xD#x20]*
wsp             ::= [#x9#xA#xD#x20]+
```

**<a id="DataTypeList"></a> <a id="DataTypeListOfIRI"></a> <a id="DataTypeNumbers"></a> <a id="DataTypeCoordinates"></a> <a id="DataTypeLengths"></a> <list-of-*T*s> **
:

(Where *T* is a type other than <string> and <family-name>.) A list consists of a separated sequence of values. Unless explicitly described differently, lists within SVG's XML attributes can be either comma-separated, with optional white space before or after the comma, or white space-separated.

White space in lists is defined as one or more of the following consecutive characters: "space" (U+0020), "tab" (U+0009), "line feed" (U+000A), "carriage return" (U+000D) and "form-feed" (U+000C).

The following is a template for an EBNF grammar describing the <list-of-*T*s> syntax:

```
<a id="ListOfTs"></a>list-of-Ts ::= T
               | T comma-wsp list-of-Ts
<a id="CommaWSP"></a>comma-wsp  ::= (wsp+ ","? wsp*) | ("," wsp*)
<a id="WSP"></a>wsp        ::= (#x20 | #x9 | #xD | #xA)
```

Within the SVG DOM, values of a <list-of-*T*s> type are represented by an interface specific for the particular type *T*. For example, a <list-of-lengths> is represented in the SVG DOM using an [SVGLengthList](#InterfaceSVGLengthList) or [SVGAnimatedLengthList](#InterfaceSVGAnimatedLengthList) object.

**<a id="DataTypeName"></a><name>**
:

A name, which is a string where a few characters of syntactic significance are disallowed.

```
<a id="Name"></a>name  ::= [^,()#x20#x9#xD#xA] /* any char except ",", "(", ")" or wsp */
```

**<a id="DataTypeNumber"></a><number>**
:

Real numbers are specified in one of two ways. When used in a style sheet, a <number> is defined as follows:

```
<a id="Number"></a>number ::= integer
           | [+-]? [0-9]* "." [0-9]+
```

This syntax is the same as the definition in CSS ([[CSS2](refs.md#ref-CSS2)], section 4.3.1).

When used in an SVG attribute, a <number> is defined differently, to allow numbers with large magnitudes to be specified more concisely:

```
number ::= integer ([Ee] integer)?
           | [+-]? [0-9]* "." [0-9]+ ([Ee] integer)?
```

Within the SVG DOM, a <number> is represented as a float, [SVGNumber](#InterfaceSVGNumber) or a [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber).

**<a id="DataTypeNumberOptionalNumber"></a><number-optional-number>**
:

A pair of [<number>](#DataTypeNumber)s, where the second [<number>](#DataTypeNumber) is optional.

```
number-optional-number ::= number
                           | number comma-wsp number
```

In the SVG DOM, a <number-optional-number> is represented using a pair of [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) or [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) objects.

**<a id="DataTypePaint"></a><paint>**
:

The values for properties [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) are specifications of the type of paint to use when filling or stroking a given graphics element. The available options and syntax for <paint> are described in [Specifying paint](painting.md#SpecifyingPaint).

Within the SVG DOM, <paint> values are represented using [SVGPaint](painting.md#InterfaceSVGPaint) objects.

**<a id="DataTypePercentage"></a><percentage>**
:

Percentages are specified as a number followed by a "%" character:

```
<a id="Percentage"></a>percentage ::= number "%"
```

Note that the definition of [<number>](#Number) depends on whether the percentage is specified in a style sheet or in an attribute that is not also a [presentation attribute](intro.md#TermPresentationAttribute).

Percentage values are always relative to another value, for example a length. Each attribute or [property](intro.md#TermProperty) that allows percentages also defines the reference distance measurement to which the percentage refers.

Within the SVG DOM, a <percentage> is represented using an [SVGNumber](#InterfaceSVGNumber) or [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) object.

**<a id="DataTypeTime"></a><time>**
:

A time value is a <number> immediately followed by a time unit identifier. The time unit identifiers are:

- ms: milliseconds
- s: seconds

In the SVG DOM, <time> values are represented using the CSSPrimitiveValue interface defined in Document Object Model CSS ([[DOM2STYLE](refs.md#ref-DOM2STYLE)], section 2.2).

**<a id="DataTypeTransformList"></a><transform-list>**
:

A <transform-list> is used to specify a list of coordinate system transformations. A detailed description of the possible values for a <transform-list> is given in [Modifying the User Coordinate System: the transform attribute](coords.md#TransformAttribute).

Within the SVG DOM, a <transform-list> value is represented using an [SVGTransformList](coords.md#InterfaceSVGTransformList) or [SVGAnimatedTransformList](coords.md#InterfaceSVGAnimatedTransformList) object.

**<a id="DataTypeXML-Name"></a><XML-Name>**
:

An XML name, as defined by the Name production in Extensible Markup Language (XML) 1.0 ([[XML10](refs.md#ref-XML10)], section 2.3).

## <a id="Precision"></a>4.3 Real number precision

Unless stated otherwise for a particular attribute or [property](intro.md#TermProperty), a [<number>](#DataTypeNumber) has the capacity for at least a single-precision floating point number and has a range (at a minimum) of -3.4e+38F to +3.4e+38F.

It is recommended that higher precision floating point storage and computation be performed on operations such as coordinate system transformations to provide the best possible precision and to prevent round-off errors.

[Conforming High-Quality SVG Viewers](conform.md#ConformingHighQualitySVGViewers) are required to use at least double-precision floating point for intermediate calculations on certain numerical operations.

## <a id="ColorKeywords"></a>4.4 Recognized color keyword names

The following is the list of recognized color keywords that can be used as a keyword value for data type [<color>](#DataTypeColor):

|  | aliceblue | rgb(240, 248, 255) |
| --- | --- | --- |
|  | antiquewhite | rgb(250, 235, 215) |
|  | aqua | rgb( 0, 255, 255) |
|  | aquamarine | rgb(127, 255, 212) |
|  | azure | rgb(240, 255, 255) |
|  | beige | rgb(245, 245, 220) |
|  | bisque | rgb(255, 228, 196) |
|  | black | rgb( 0, 0, 0) |
|  | blanchedalmond | rgb(255, 235, 205) |
|  | blue | rgb( 0, 0, 255) |
|  | blueviolet | rgb(138, 43, 226) |
|  | brown | rgb(165, 42, 42) |
|  | burlywood | rgb(222, 184, 135) |
|  | cadetblue | rgb( 95, 158, 160) |
|  | chartreuse | rgb(127, 255, 0) |
|  | chocolate | rgb(210, 105, 30) |
|  | coral | rgb(255, 127, 80) |
|  | cornflowerblue | rgb(100, 149, 237) |
|  | cornsilk | rgb(255, 248, 220) |
|  | crimson | rgb(220, 20, 60) |
|  | cyan | rgb( 0, 255, 255) |
|  | darkblue | rgb( 0, 0, 139) |
|  | darkcyan | rgb( 0, 139, 139) |
|  | darkgoldenrod | rgb(184, 134, 11) |
|  | darkgray | rgb(169, 169, 169) |
|  | darkgreen | rgb( 0, 100, 0) |
|  | darkgrey | rgb(169, 169, 169) |
|  | darkkhaki | rgb(189, 183, 107) |
|  | darkmagenta | rgb(139, 0, 139) |
|  | darkolivegreen | rgb( 85, 107, 47) |
|  | darkorange | rgb(255, 140, 0) |
|  | darkorchid | rgb(153, 50, 204) |
|  | darkred | rgb(139, 0, 0) |
|  | darksalmon | rgb(233, 150, 122) |
|  | darkseagreen | rgb(143, 188, 143) |
|  | darkslateblue | rgb( 72, 61, 139) |
|  | darkslategray | rgb( 47, 79, 79) |
|  | darkslategrey | rgb( 47, 79, 79) |
|  | darkturquoise | rgb( 0, 206, 209) |
|  | darkviolet | rgb(148, 0, 211) |
|  | deeppink | rgb(255, 20, 147) |
|  | deepskyblue | rgb( 0, 191, 255) |
|  | dimgray | rgb(105, 105, 105) |
|  | dimgrey | rgb(105, 105, 105) |
|  | dodgerblue | rgb( 30, 144, 255) |
|  | firebrick | rgb(178, 34, 34) |
|  | floralwhite | rgb(255, 250, 240) |
|  | forestgreen | rgb( 34, 139, 34) |
|  | fuchsia | rgb(255, 0, 255) |
|  | gainsboro | rgb(220, 220, 220) |
|  | ghostwhite | rgb(248, 248, 255) |
|  | gold | rgb(255, 215, 0) |
|  | goldenrod | rgb(218, 165, 32) |
|  | gray | rgb(128, 128, 128) |
|  | grey | rgb(128, 128, 128) |
|  | green | rgb( 0, 128, 0) |
|  | greenyellow | rgb(173, 255, 47) |
|  | honeydew | rgb(240, 255, 240) |
|  | hotpink | rgb(255, 105, 180) |
|  | indianred | rgb(205, 92, 92) |
|  | indigo | rgb( 75, 0, 130) |
|  | ivory | rgb(255, 255, 240) |
|  | khaki | rgb(240, 230, 140) |
|  | lavender | rgb(230, 230, 250) |
|  | lavenderblush | rgb(255, 240, 245) |
|  | lawngreen | rgb(124, 252, 0) |
|  | lemonchiffon | rgb(255, 250, 205) |
|  | lightblue | rgb(173, 216, 230) |
|  | lightcoral | rgb(240, 128, 128) |
|  | lightcyan | rgb(224, 255, 255) |
|  | lightgoldenrodyellow | rgb(250, 250, 210) |
|  | lightgray | rgb(211, 211, 211) |
|  | lightgreen | rgb(144, 238, 144) |
|  | lightgrey | rgb(211, 211, 211) |

|  | lightpink | rgb(255, 182, 193) |
| --- | --- | --- |
|  | lightsalmon | rgb(255, 160, 122) |
|  | lightseagreen | rgb( 32, 178, 170) |
|  | lightskyblue | rgb(135, 206, 250) |
|  | lightslategray | rgb(119, 136, 153) |
|  | lightslategrey | rgb(119, 136, 153) |
|  | lightsteelblue | rgb(176, 196, 222) |
|  | lightyellow | rgb(255, 255, 224) |
|  | lime | rgb( 0, 255, 0) |
|  | limegreen | rgb( 50, 205, 50) |
|  | linen | rgb(250, 240, 230) |
|  | magenta | rgb(255, 0, 255) |
|  | maroon | rgb(128, 0, 0) |
|  | mediumaquamarine | rgb(102, 205, 170) |
|  | mediumblue | rgb( 0, 0, 205) |
|  | mediumorchid | rgb(186, 85, 211) |
|  | mediumpurple | rgb(147, 112, 219) |
|  | mediumseagreen | rgb( 60, 179, 113) |
|  | mediumslateblue | rgb(123, 104, 238) |
|  | mediumspringgreen | rgb( 0, 250, 154) |
|  | mediumturquoise | rgb( 72, 209, 204) |
|  | mediumvioletred | rgb(199, 21, 133) |
|  | midnightblue | rgb( 25, 25, 112) |
|  | mintcream | rgb(245, 255, 250) |
|  | mistyrose | rgb(255, 228, 225) |
|  | moccasin | rgb(255, 228, 181) |
|  | navajowhite | rgb(255, 222, 173) |
|  | navy | rgb( 0, 0, 128) |
|  | oldlace | rgb(253, 245, 230) |
|  | olive | rgb(128, 128, 0) |
|  | olivedrab | rgb(107, 142, 35) |
|  | orange | rgb(255, 165, 0) |
|  | orangered | rgb(255, 69, 0) |
|  | orchid | rgb(218, 112, 214) |
|  | palegoldenrod | rgb(238, 232, 170) |
|  | palegreen | rgb(152, 251, 152) |
|  | paleturquoise | rgb(175, 238, 238) |
|  | palevioletred | rgb(219, 112, 147) |
|  | papayawhip | rgb(255, 239, 213) |
|  | peachpuff | rgb(255, 218, 185) |
|  | peru | rgb(205, 133, 63) |
|  | pink | rgb(255, 192, 203) |
|  | plum | rgb(221, 160, 221) |
|  | powderblue | rgb(176, 224, 230) |
|  | purple | rgb(128, 0, 128) |
|  | red | rgb(255, 0, 0) |
|  | rosybrown | rgb(188, 143, 143) |
|  | royalblue | rgb( 65, 105, 225) |
|  | saddlebrown | rgb(139, 69, 19) |
|  | salmon | rgb(250, 128, 114) |
|  | sandybrown | rgb(244, 164, 96) |
|  | seagreen | rgb( 46, 139, 87) |
|  | seashell | rgb(255, 245, 238) |
|  | sienna | rgb(160, 82, 45) |
|  | silver | rgb(192, 192, 192) |
|  | skyblue | rgb(135, 206, 235) |
|  | slateblue | rgb(106, 90, 205) |
|  | slategray | rgb(112, 128, 144) |
|  | slategrey | rgb(112, 128, 144) |
|  | snow | rgb(255, 250, 250) |
|  | springgreen | rgb( 0, 255, 127) |
|  | steelblue | rgb( 70, 130, 180) |
|  | tan | rgb(210, 180, 140) |
|  | teal | rgb( 0, 128, 128) |
|  | thistle | rgb(216, 191, 216) |
|  | tomato | rgb(255, 99, 71) |
|  | turquoise | rgb( 64, 224, 208) |
|  | violet | rgb(238, 130, 238) |
|  | wheat | rgb(245, 222, 179) |
|  | white | rgb(255, 255, 255) |
|  | whitesmoke | rgb(245, 245, 245) |
|  | yellow | rgb(255, 255, 0) |
|  | yellowgreen | rgb(154, 205, 50) |
|  |  |  |

## <a id="BasicDOMInterfaces"></a>4.5 Basic DOM interfaces

### <a id="InterfaceSVGElement"></a>4.5.1 Interface SVGElement

All of the SVG DOM interfaces that correspond directly to elements in the SVG language (such as the [SVGPathElement](paths.md#InterfaceSVGPathElement) interface for the [‘path’](paths.md#PathElement) element) derive from the [SVGElement](#InterfaceSVGElement) interface.

```
interface SVGElement : Element {
           attribute DOMString id setraises(DOMException);
           attribute DOMString xmlbase setraises(DOMException);
  readonly attribute SVGSVGElement ownerSVGElement;
  readonly attribute SVGElement viewportElement;
};
```

**Attributes:**
:

**<a id="__svg__SVGElement__id"></a>**id** (DOMString)**
:

The value of the [‘id’](struct.md#IDAttribute) attribute on the given element, or the empty string if [‘id’](struct.md#IDAttribute) is not present.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGElement__xmlbase"></a>**xmlbase** (DOMString)**
:

Corresponds to attribute [‘xml:base’](struct.md#XMLBaseAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGElement__ownerSVGElement"></a>**ownerSVGElement** (readonly [SVGSVGElement](struct.md#InterfaceSVGSVGElement))**
:

The nearest ancestor [‘svg’](struct.md#SVGElement) element. Null if the given element is the [outermost svg element](intro.md#TermOutermostSVGElement).

**<a id="__svg__SVGElement__viewportElement"></a>**viewportElement** (readonly [SVGElement](#InterfaceSVGElement))**
:

The element which established the current viewport. Often, the nearest ancestor [‘svg’](struct.md#SVGElement) element. Null if the given element is the [outermost svg element](intro.md#TermOutermostSVGElement).

### <a id="InterfaceSVGAnimatedBoolean"></a>4.5.2 Interface SVGAnimatedBoolean

Used for attributes of type boolean which can be animated.

```
interface SVGAnimatedBoolean {
           attribute boolean baseVal setraises(DOMException);
  readonly attribute boolean animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedBoolean__baseVal"></a>**baseVal** (boolean)**
:

The base value of the given attribute before applying any animations.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGAnimatedBoolean__animVal"></a>**animVal** (readonly boolean)**
:

If the given attribute or property is being animated, contains the current animated value of the attribute or property. If the given attribute or property is not currently being animated, contains the same value as [baseVal](#__svg__SVGAnimatedBoolean__baseVal).

### <a id="InterfaceSVGAnimatedString"></a>4.5.3 Interface SVGAnimatedString

Used for attributes of type DOMString which can be animated.

```
interface SVGAnimatedString {
           attribute DOMString baseVal setraises(DOMException);
  readonly attribute DOMString animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedString__baseVal"></a>**baseVal** (DOMString)**
:

The base value of the given attribute before applying any animations.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGAnimatedString__animVal"></a>**animVal** (readonly DOMString)**
:

If the given attribute or property is being animated, contains the current animated value of the attribute or property. If the given attribute or property is not currently being animated, contains the same value as [baseVal](#__svg__SVGAnimatedString__baseVal).

### <a id="InterfaceSVGStringList"></a>4.5.4 Interface SVGStringList

This interface defines a list of DOMString values.

[SVGStringList](#InterfaceSVGStringList) has the same attributes and methods as other SVGxxxList interfaces. Implementers may consider using a single base class to implement the various SVGxxxList interfaces.

```
interface SVGStringList {

  readonly attribute unsigned long numberOfItems;

  void clear() raises(DOMException);
  DOMString initialize(in DOMString newItem) raises(DOMException);
  DOMString getItem(in unsigned long index) raises(DOMException);
  DOMString insertItemBefore(in DOMString newItem, in unsigned long index) raises(DOMException);
  DOMString replaceItem(in DOMString newItem, in unsigned long index) raises(DOMException);
  DOMString removeItem(in unsigned long index) raises(DOMException);
  DOMString appendItem(in DOMString newItem) raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGStringList__numberOfItems"></a>**numberOfItems** (readonly unsigned long)**
:

The number of items in the list.

**Operations:**
:

**<a id="__svg__SVGStringList__clear"></a>void **clear**()**
:

Clears all existing current items from the list, with the result being an empty list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**<a id="__svg__SVGStringList__initialize"></a>DOMString **initialize**(in DOMString *newItem*)**
:

Clears all existing current items from the list and re-initializes the list to hold the single item specified by the parameter.

**Parameters**
:

1.
DOMString *newItem*
The item which should become the only member of the list.

**Returns**
:

The item being inserted into the list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**<a id="__svg__SVGStringList__getItem"></a>DOMString **getItem**(in unsigned long *index*)**
:

Returns the specified item from the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item from the list which is to be returned. The first item is number 0.

**Returns**
:

The selected item.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGStringList__numberOfItems).
**<a id="__svg__SVGStringList__insertItemBefore"></a>DOMString **insertItemBefore**(in DOMString *newItem*, in unsigned long *index*)**
:

Inserts a new item into the list at the specified position. The first item is number 0.

**Parameters**
:

1.
DOMString *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item before which the new item is to be inserted. The first item is number 0. If the index is equal to 0, then the new item is inserted at the front of the list. If the index is greater than or equal to [numberOfItems](#__svg__SVGStringList__numberOfItems), then the new item is appended to the end of the list.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**<a id="__svg__SVGStringList__replaceItem"></a>DOMString **replaceItem**(in DOMString *newItem*, in unsigned long *index*)**
:

Replaces an existing item in the list with a new item.

**Parameters**
:

1.
DOMString *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item which is to be replaced. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGStringList__numberOfItems).
**<a id="__svg__SVGStringList__removeItem"></a>DOMString **removeItem**(in unsigned long *index*)**
:

Removes an existing item from the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item which is to be removed. The first item is number 0.

**Returns**
:

The removed item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGStringList__numberOfItems).
**<a id="__svg__SVGStringList__appendItem"></a>DOMString **appendItem**(in DOMString *newItem*)**
:

Inserts a new item at the end of the list.

**Parameters**
:

1.
DOMString *newItem*
The item which is to be inserted. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
### <a id="InterfaceSVGAnimatedEnumeration"></a>4.5.5 Interface SVGAnimatedEnumeration

Used for attributes whose value must be a constant from a particular enumeration and which can be animated.

```
interface SVGAnimatedEnumeration {
           attribute unsigned short baseVal setraises(DOMException);
  readonly attribute unsigned short animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedEnumeration__baseVal"></a>**baseVal** (unsigned short)**
:

The base value of the given attribute before applying any animations.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGAnimatedEnumeration__animVal"></a>**animVal** (readonly unsigned short)**
:

If the given attribute or property is being animated, contains the current animated value of the attribute or property. If the given attribute or property is not currently being animated, contains the same value as [baseVal](#__svg__SVGAnimatedEnumeration__baseVal).

### <a id="InterfaceSVGAnimatedInteger"></a>4.5.6 Interface SVGAnimatedInteger

Used for attributes of basic type [<integer>](#DataTypeInteger) which can be animated.

```
interface SVGAnimatedInteger {
           attribute long baseVal setraises(DOMException);
  readonly attribute long animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedInteger__baseVal"></a>**baseVal** (long)**
:

The base value of the given attribute before applying any animations.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGAnimatedInteger__animVal"></a>**animVal** (readonly long)**
:

If the given attribute or property is being animated, contains the current animated value of the attribute or property. If the given attribute or property is not currently being animated, contains the same value as [baseVal](#__svg__SVGAnimatedInteger__baseVal).

### <a id="InterfaceSVGNumber"></a>4.5.7 Interface SVGNumber

Used for attributes of basic type [<number>](#DataTypeNumber).

```
interface SVGNumber {
  attribute float value setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGNumber__value"></a>**value** (float)**
:

The value of the given attribute.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGAnimatedNumber"></a>4.5.8 Interface SVGAnimatedNumber

Used for attributes of basic type [<number>](#DataTypeNumber) which can be animated.

```
interface SVGAnimatedNumber {
           attribute float baseVal setraises(DOMException);
  readonly attribute float animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedNumber__baseVal"></a>**baseVal** (float)**
:

The base value of the given attribute before applying any animations.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGAnimatedNumber__animVal"></a>**animVal** (readonly float)**
:

If the given attribute or property is being animated, contains the current animated value of the attribute or property. If the given attribute or property is not currently being animated, contains the same value as [baseVal](#__svg__SVGAnimatedNumber__baseVal).

### <a id="InterfaceSVGNumberList"></a>4.5.9 Interface SVGNumberList

This interface defines a list of SVGNumber objects.

[SVGNumberList](#InterfaceSVGNumberList) has the same attributes and methods as other SVGxxxList interfaces. Implementers may consider using a single base class to implement the various SVGxxxList interfaces.

<a id="ReadOnlyNumberList"></a>An [SVGNumberList](#InterfaceSVGNumberList) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

```
interface SVGNumberList {

  readonly attribute unsigned long numberOfItems;

  void clear() raises(DOMException);
  SVGNumber initialize(in SVGNumber newItem) raises(DOMException);
  SVGNumber getItem(in unsigned long index) raises(DOMException);
  SVGNumber insertItemBefore(in SVGNumber newItem, in unsigned long index) raises(DOMException);
  SVGNumber replaceItem(in SVGNumber newItem, in unsigned long index) raises(DOMException);
  SVGNumber removeItem(in unsigned long index) raises(DOMException);
  SVGNumber appendItem(in SVGNumber newItem) raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGNumberList__numberOfItems"></a>**numberOfItems** (readonly unsigned long)**
:

The number of items in the list.

**Operations:**
:

**<a id="__svg__SVGNumberList__clear"></a>void **clear**()**
:

Clears all existing current items from the list, with the result being an empty list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyNumberList).
**<a id="__svg__SVGNumberList__initialize"></a>[SVGNumber](#InterfaceSVGNumber) **initialize**(in [SVGNumber](#InterfaceSVGNumber) *newItem*)**
:

Clears all existing current items from the list and re-initializes the list to hold the single item specified by the parameter. If the inserted item is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy.

**Parameters**
:

1.
[SVGNumber](#InterfaceSVGNumber) *newItem*
The item which should become the only member of the list.

**Returns**
:

The item being inserted into the list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyNumberList).
**<a id="__svg__SVGNumberList__getItem"></a>[SVGNumber](#InterfaceSVGNumber) **getItem**(in unsigned long *index*)**
:

Returns the specified item from the list. The returned item is the item itself and not a copy. Any changes made to the item are immediately reflected in the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item from the list which is to be returned. The first item is number 0.

**Returns**
:

The selected item.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGNumberList__numberOfItems).
**<a id="__svg__SVGNumberList__insertItemBefore"></a>[SVGNumber](#InterfaceSVGNumber) **insertItemBefore**(in [SVGNumber](#InterfaceSVGNumber) *newItem*, in unsigned long *index*)**
:

Inserts a new item into the list at the specified position. The first item is number 0. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy. If the item is already in this list, note that the index of the item to insert before is *before* the removal of the item.

**Parameters**
:

1.
[SVGNumber](#InterfaceSVGNumber) *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item before which the new item is to be inserted. The first item is number 0. If the index is equal to 0, then the new item is inserted at the front of the list. If the index is greater than or equal to [numberOfItems](#__svg__SVGNumberList__numberOfItems), then the new item is appended to the end of the list.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyNumberList).
**<a id="__svg__SVGNumberList__replaceItem"></a>[SVGNumber](#InterfaceSVGNumber) **replaceItem**(in [SVGNumber](#InterfaceSVGNumber) *newItem*, in unsigned long *index*)**
:

Replaces an existing item in the list with a new item. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy. If the item is already in this list, note that the index of the item to replace is *before* the removal of the item.

**Parameters**
:

1.
[SVGNumber](#InterfaceSVGNumber) *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item which is to be replaced. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyNumberList).
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGNumberList__numberOfItems).
**<a id="__svg__SVGNumberList__removeItem"></a>[SVGNumber](#InterfaceSVGNumber) **removeItem**(in unsigned long *index*)**
:

Removes an existing item from the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item which is to be removed. The first item is number 0.

**Returns**
:

The removed item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGNumberList__numberOfItems).
**<a id="__svg__SVGNumberList__appendItem"></a>[SVGNumber](#InterfaceSVGNumber) **appendItem**(in [SVGNumber](#InterfaceSVGNumber) *newItem*)**
:

Inserts a new item at the end of the list. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy.

**Parameters**
:

1.
[SVGNumber](#InterfaceSVGNumber) *newItem*
The item which is to be inserted. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
### <a id="InterfaceSVGAnimatedNumberList"></a>4.5.10 Interface SVGAnimatedNumberList

Used for attributes which take a list of numbers and which can be animated.

```
interface SVGAnimatedNumberList {
  readonly attribute SVGNumberList baseVal;
  readonly attribute SVGNumberList animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedNumberList__baseVal"></a>**baseVal** (readonly [SVGNumberList](#InterfaceSVGNumberList))**
:

The base value of the given attribute before applying any animations.

**<a id="__svg__SVGAnimatedNumberList__animVal"></a>**animVal** (readonly [SVGNumberList](#InterfaceSVGNumberList))**
:

A [read only](#ReadOnlyNumberList) [SVGNumberList](#InterfaceSVGNumberList) representing the current animated value of the given attribute. If the given attribute is not currently being animated, then the [SVGNumberList](#InterfaceSVGNumberList) will have the same contents as [baseVal](#__svg__SVGAnimatedNumberList__baseVal). The object referenced by [animVal](#__svg__SVGAnimatedNumberList__animVal) will always be distinct from the one referenced by [baseVal](#__svg__SVGAnimatedNumberList__baseVal), even when the attribute is not animated.

### <a id="InterfaceSVGLength"></a>4.5.11 Interface SVGLength

The [SVGLength](#InterfaceSVGLength) interface corresponds to the [<length>](#DataTypeLength) basic data type.

<a id="ReadOnlyLength"></a>An [SVGLength](#InterfaceSVGLength) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

```
interface SVGLength {

  // Length Unit Types
  const unsigned short SVG_LENGTHTYPE_UNKNOWN = 0;
  const unsigned short SVG_LENGTHTYPE_NUMBER = 1;
  const unsigned short SVG_LENGTHTYPE_PERCENTAGE = 2;
  const unsigned short SVG_LENGTHTYPE_EMS = 3;
  const unsigned short SVG_LENGTHTYPE_EXS = 4;
  const unsigned short SVG_LENGTHTYPE_PX = 5;
  const unsigned short SVG_LENGTHTYPE_CM = 6;
  const unsigned short SVG_LENGTHTYPE_MM = 7;
  const unsigned short SVG_LENGTHTYPE_IN = 8;
  const unsigned short SVG_LENGTHTYPE_PT = 9;
  const unsigned short SVG_LENGTHTYPE_PC = 10;

  readonly attribute unsigned short unitType;
           attribute float value setraises(DOMException);
           attribute float valueInSpecifiedUnits setraises(DOMException);
           attribute DOMString valueAsString setraises(DOMException);

  void newValueSpecifiedUnits(in unsigned short unitType, in float valueInSpecifiedUnits) raises(DOMException);
  void convertToSpecifiedUnits(in unsigned short unitType) raises(DOMException);
};
```

**Constants in group “Length Unit Types”:**
:

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN"></a>**SVG_LENGTHTYPE_UNKNOWN** (unsigned short)**
:

The unit type is not one of predefined unit types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_NUMBER"></a>**SVG_LENGTHTYPE_NUMBER** (unsigned short)**
:

No unit type was provided (i.e., a unitless value was specified), which indicates a value in user units.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_PERCENTAGE"></a>**SVG_LENGTHTYPE_PERCENTAGE** (unsigned short)**
:

A percentage value was specified.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_EMS"></a>**SVG_LENGTHTYPE_EMS** (unsigned short)**
:

A value was specified using the em units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_EXS"></a>**SVG_LENGTHTYPE_EXS** (unsigned short)**
:

A value was specified using the ex units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_PX"></a>**SVG_LENGTHTYPE_PX** (unsigned short)**
:

A value was specified using the px units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_CM"></a>**SVG_LENGTHTYPE_CM** (unsigned short)**
:

A value was specified using the cm units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_MM"></a>**SVG_LENGTHTYPE_MM** (unsigned short)**
:

A value was specified using the mm units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_IN"></a>**SVG_LENGTHTYPE_IN** (unsigned short)**
:

A value was specified using the in units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_PT"></a>**SVG_LENGTHTYPE_PT** (unsigned short)**
:

A value was specified using the pt units defined in CSS2.

**<a id="__svg__SVGLength__SVG_LENGTHTYPE_PC"></a>**SVG_LENGTHTYPE_PC** (unsigned short)**
:

A value was specified using the pc units defined in CSS2.

**Attributes:**
:

**<a id="__svg__SVGLength__unitType"></a>**unitType** (readonly unsigned short)**
:

The type of the value as specified by one of the SVG_LENGTHTYPE_* constants defined on this interface.

**<a id="__svg__SVGLength__value"></a>**value** (float)**
:

The value as a floating point value, in user units. Setting this attribute will cause [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits) and [valueAsString](#__svg__SVGLength__valueAsString) to be updated automatically to reflect this setting.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the length corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLength).
**<a id="__svg__SVGLength__valueInSpecifiedUnits"></a>**valueInSpecifiedUnits** (float)**
:

The value as a floating point value, in the units expressed by [unitType](#__svg__SVGLength__unitType). Setting this attribute will cause [value](#__svg__SVGLength__value) and [valueAsString](#__svg__SVGLength__valueAsString) to be updated automatically to reflect this setting.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the length corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLength).
**<a id="__svg__SVGLength__valueAsString"></a>**valueAsString** (DOMString)**
:

The value as a string value, in the units expressed by [unitType](#__svg__SVGLength__unitType). Setting this attribute will cause [value](#__svg__SVGLength__value), [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits) and [unitType](#__svg__SVGLength__unitType) to be updated automatically to reflect this setting.

**Exceptions on setting**
:

**DOMException, code SYNTAX_ERR**
: Raised if the assigned string cannot be parsed as a valid [<length>](#DataTypeLength).
**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the length corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLength).
**Operations:**
:

**<a id="__svg__SVGLength__newValueSpecifiedUnits"></a>void **newValueSpecifiedUnits**(in unsigned short *unitType*, in float *valueInSpecifiedUnits*)**
:

Reset the value as a number with an associated [unitType](#__svg__SVGLength__unitType), thereby replacing the values for all of the attributes on the object.

**Parameters**
:

1.
unsigned short *unitType*
The unit type for the value (e.g., [SVG_LENGTHTYPE_MM](#__svg__SVGLength__SVG_LENGTHTYPE_MM)).
2.
float *valueInSpecifiedUnits*
The new value.

**Exceptions**
:

**DOMException, code NOT_SUPPORTED_ERR**
: Raised if unitType is SVG_LENGTHTYPE_UNKNOWN or not a valid unit type constant (one of the other SVG_LENGTHTYPE_* constants defined on this interface).
**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the length corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLength).
**<a id="__svg__SVGLength__convertToSpecifiedUnits"></a>void **convertToSpecifiedUnits**(in unsigned short *unitType*)**
:

Preserve the same underlying stored value, but reset the stored unit identifier to the given *unitType*. Object attributes [unitType](#__svg__SVGLength__unitType), [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits) and [valueAsString](#__svg__SVGLength__valueAsString) might be modified as a result of this method. For example, if the original value were "0.5cm" and the method was invoked to convert to millimeters, then the [unitType](#__svg__SVGLength__unitType) would be changed to [SVG_LENGTHTYPE_MM](#__svg__SVGLength__SVG_LENGTHTYPE_MM), [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits) would be changed to the numeric value 5 and [valueAsString](#__svg__SVGLength__valueAsString) would be changed to "5mm".

**Parameters**
:

1.
unsigned short *unitType*
The unit type to switch to (e.g., [SVG_LENGTHTYPE_MM](#__svg__SVGLength__SVG_LENGTHTYPE_MM)).

**Exceptions**
:

**DOMException, code NOT_SUPPORTED_ERR**
: Raised if unitType is SVG_LENGTHTYPE_UNKNOWN or not a valid unit type constant (one of the other SVG_LENGTHTYPE_* constants defined on this interface).
**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the length corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLength).
### <a id="InterfaceSVGAnimatedLength"></a>4.5.12 Interface SVGAnimatedLength

Used for attributes of basic type [<length>](#DataTypeLength) which can be animated.

```
interface SVGAnimatedLength {
  readonly attribute SVGLength baseVal;
  readonly attribute SVGLength animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedLength__baseVal"></a>**baseVal** (readonly [SVGLength](#InterfaceSVGLength))**
:

The base value of the given attribute before applying any animations.

**<a id="__svg__SVGAnimatedLength__animVal"></a>**animVal** (readonly [SVGLength](#InterfaceSVGLength))**
:

A [read only](#ReadOnlyLength) [SVGLength](#InterfaceSVGLength) representing the current animated value of the given attribute. If the given attribute is not currently being animated, then the [SVGLength](#InterfaceSVGLength) will have the same contents as [baseVal](#__svg__SVGAnimatedLength__baseVal). The object referenced by [animVal](#__svg__SVGAnimatedLength__animVal) will always be distinct from the one referenced by [baseVal](#__svg__SVGAnimatedLength__baseVal), even when the attribute is not animated.

### <a id="InterfaceSVGLengthList"></a>4.5.13 Interface SVGLengthList

This interface defines a list of SVGLength objects.

[SVGLengthList](#InterfaceSVGLengthList) has the same attributes and methods as other SVGxxxList interfaces. Implementers may consider using a single base class to implement the various SVGxxxList interfaces.

<a id="ReadOnlyLengthList"></a>An [SVGLengthList](#InterfaceSVGLengthList) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

```
interface SVGLengthList {

  readonly attribute unsigned long numberOfItems;

  void clear() raises(DOMException);
  SVGLength initialize(in SVGLength newItem) raises(DOMException);
  SVGLength getItem(in unsigned long index) raises(DOMException);
  SVGLength insertItemBefore(in SVGLength newItem, in unsigned long index) raises(DOMException);
  SVGLength replaceItem(in SVGLength newItem, in unsigned long index) raises(DOMException);
  SVGLength removeItem(in unsigned long index) raises(DOMException);
  SVGLength appendItem(in SVGLength newItem) raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGLengthList__numberOfItems"></a>**numberOfItems** (readonly unsigned long)**
:

The number of items in the list.

**Operations:**
:

**<a id="__svg__SVGLengthList__clear"></a>void **clear**()**
:

Clears all existing current items from the list, with the result being an empty list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
**<a id="__svg__SVGLengthList__initialize"></a>[SVGLength](#InterfaceSVGLength) **initialize**(in [SVGLength](#InterfaceSVGLength) *newItem*)**
:

Clears all existing current items from the list and re-initializes the list to hold the single item specified by the parameter. If the inserted item is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy.

**Parameters**
:

1.
[SVGLength](#InterfaceSVGLength) *newItem*
The item which should become the only member of the list.

**Returns**
:

The item being inserted into the list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
**<a id="__svg__SVGLengthList__getItem"></a>[SVGLength](#InterfaceSVGLength) **getItem**(in unsigned long *index*)**
:

Returns the specified item from the list. The returned item is the item itself and not a copy. Any changes made to the item are immediately reflected in the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item from the list which is to be returned. The first item is number 0.

**Returns**
:

The selected item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
**<a id="__svg__SVGLengthList__insertItemBefore"></a>[SVGLength](#InterfaceSVGLength) **insertItemBefore**(in [SVGLength](#InterfaceSVGLength) *newItem*, in unsigned long *index*)**
:

Inserts a new item into the list at the specified position. The first item is number 0. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy. If the item is already in this list, note that the index of the item to insert before is *before* the removal of the item.

**Parameters**
:

1.
[SVGLength](#InterfaceSVGLength) *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item before which the new item is to be inserted. The first item is number 0. If the index is equal to 0, then the new item is inserted at the front of the list. If the index is greater than or equal to [numberOfItems](#__svg__SVGLengthList__numberOfItems), then the new item is appended to the end of the list.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
**<a id="__svg__SVGLengthList__replaceItem"></a>[SVGLength](#InterfaceSVGLength) **replaceItem**(in [SVGLength](#InterfaceSVGLength) *newItem*, in unsigned long *index*)**
:

Replaces an existing item in the list with a new item. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy. If the item is already in this list, note that the index of the item to replace is *before* the removal of the item.

**Parameters**
:

1.
[SVGLength](#InterfaceSVGLength) *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item which is to be replaced. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGLengthList__numberOfItems).
**<a id="__svg__SVGLengthList__removeItem"></a>[SVGLength](#InterfaceSVGLength) **removeItem**(in unsigned long *index*)**
:

Removes an existing item from the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item which is to be removed. The first item is number 0.

**Returns**
:

The removed item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGLengthList__numberOfItems).
**<a id="__svg__SVGLengthList__appendItem"></a>[SVGLength](#InterfaceSVGLength) **appendItem**(in [SVGLength](#InterfaceSVGLength) *newItem*)**
:

Inserts a new item at the end of the list. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy.

**Parameters**
:

1.
[SVGLength](#InterfaceSVGLength) *newItem*
The item which is to be inserted. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyLengthList).
### <a id="InterfaceSVGAnimatedLengthList"></a>4.5.14 Interface SVGAnimatedLengthList

Used for attributes of type [SVGLengthList](#InterfaceSVGLengthList) which can be animated.

```
interface SVGAnimatedLengthList {
  readonly attribute SVGLengthList baseVal;
  readonly attribute SVGLengthList animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedLengthList__baseVal"></a>**baseVal** (readonly [SVGLengthList](#InterfaceSVGLengthList))**
:

The base value of the given attribute before applying any animations.

**<a id="__svg__SVGAnimatedLengthList__animVal"></a>**animVal** (readonly [SVGLengthList](#InterfaceSVGLengthList))**
:

A [read only](#ReadOnlyLengthList) [SVGLengthList](#InterfaceSVGLengthList) representing the current animated value of the given attribute. If the given attribute is not currently being animated, then the [SVGLengthList](#InterfaceSVGLengthList) will have the same contents as [baseVal](#__svg__SVGAnimatedLengthList__baseVal). The object referenced by [animVal](#__svg__SVGAnimatedLengthList__animVal) will always be distinct from the one referenced by [baseVal](#__svg__SVGAnimatedLengthList__baseVal), even when the attribute is not animated.

### <a id="InterfaceSVGAngle"></a>4.5.15 Interface SVGAngle

The [SVGAngle](#InterfaceSVGAngle) interface corresponds to the [<angle>](#DataTypeAngle) basic data type.

<a id="ReadOnlyAngle"></a>An [SVGAngle](#InterfaceSVGAngle) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

```
interface SVGAngle {

  // Angle Unit Types
  const unsigned short SVG_ANGLETYPE_UNKNOWN = 0;
  const unsigned short SVG_ANGLETYPE_UNSPECIFIED = 1;
  const unsigned short SVG_ANGLETYPE_DEG = 2;
  const unsigned short SVG_ANGLETYPE_RAD = 3;
  const unsigned short SVG_ANGLETYPE_GRAD = 4;

  readonly attribute unsigned short unitType;
           attribute float value setraises(DOMException);
           attribute float valueInSpecifiedUnits setraises(DOMException);
           attribute DOMString valueAsString setraises(DOMException);

  void newValueSpecifiedUnits(in unsigned short unitType, in float valueInSpecifiedUnits) raises(DOMException);
  void convertToSpecifiedUnits(in unsigned short unitType) raises(DOMException);
};
```

**Constants in group “Angle Unit Types”:**
:

**<a id="__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN"></a>**SVG_ANGLETYPE_UNKNOWN** (unsigned short)**
:

The unit type is not one of predefined unit types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGAngle__SVG_ANGLETYPE_UNSPECIFIED"></a>**SVG_ANGLETYPE_UNSPECIFIED** (unsigned short)**
:

No unit type was provided (i.e., a unitless value was specified). For angles, a unitless value is treated the same as if degrees were specified.

**<a id="__svg__SVGAngle__SVG_ANGLETYPE_DEG"></a>**SVG_ANGLETYPE_DEG** (unsigned short)**
:

The unit type was explicitly set to degrees.

**<a id="__svg__SVGAngle__SVG_ANGLETYPE_RAD"></a>**SVG_ANGLETYPE_RAD** (unsigned short)**
:

The unit type is radians.

**<a id="__svg__SVGAngle__SVG_ANGLETYPE_GRAD"></a>**SVG_ANGLETYPE_GRAD** (unsigned short)**
:

The unit type is radians.

**Attributes:**
:

**<a id="__svg__SVGAngle__unitType"></a>**unitType** (readonly unsigned short)**
:

The type of the value as specified by one of the SVG_ANGLETYPE_* constants defined on this interface.

**<a id="__svg__SVGAngle__value"></a>**value** (float)**
:

The angle value as a floating point value, in degrees. Setting this attribute will cause [valueInSpecifiedUnits](#__svg__SVGAngle__valueInSpecifiedUnits) and [valueAsString](#__svg__SVGAngle__valueAsString) to be updated automatically to reflect this setting.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the angle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyAngle).
**<a id="__svg__SVGAngle__valueInSpecifiedUnits"></a>**valueInSpecifiedUnits** (float)**
:

The angle value as a floating point value, in the units expressed by [unitType](#__svg__SVGAngle__unitType). Setting this attribute will cause [value](#__svg__SVGAngle__value) and [valueAsString](#__svg__SVGAngle__valueAsString) to be updated automatically to reflect this setting.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the angle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyAngle).
**<a id="__svg__SVGAngle__valueAsString"></a>**valueAsString** (DOMString)**
:

The angle value as a string value, in the units expressed by [unitType](#__svg__SVGAngle__unitType). Setting this attribute will cause [value](#__svg__SVGAngle__value), [valueInSpecifiedUnits](#__svg__SVGAngle__valueInSpecifiedUnits) and [unitType](#__svg__SVGAngle__unitType) to be updated automatically to reflect this setting.

**Exceptions on setting**
:

**DOMException, code SYNTAX_ERR**
: Raised if the assigned string cannot be parsed as a valid [<angle>](#DataTypeAngle).
**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the angle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyAngle).
**Operations:**
:

**<a id="__svg__SVGAngle__newValueSpecifiedUnits"></a>void **newValueSpecifiedUnits**(in unsigned short *unitType*, in float *valueInSpecifiedUnits*)**
:

Reset the value as a number with an associated [unitType](#__svg__SVGAngle__unitType), thereby replacing the values for all of the attributes on the object.

**Parameters**
:

1.
unsigned short *unitType*
The unit type for the value (e.g., [SVG_ANGLETYPE_DEG](#__svg__SVGAngle__SVG_ANGLETYPE_DEG)).
2.
float *valueInSpecifiedUnits*
The angle value.

**Exceptions**
:

**DOMException, code NOT_SUPPORTED_ERR**
: Raised if unitType is SVG_ANGLETYPE_UNKNOWN or not a valid unit type constant (one of the other SVG_ANGLETYPE_* constants defined on this interface).
**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the angle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyAngle).
**<a id="__svg__SVGAngle__convertToSpecifiedUnits"></a>void **convertToSpecifiedUnits**(in unsigned short *unitType*)**
:

Preserve the same underlying stored value, but reset the stored unit identifier to the given *unitType*. Object attributes [unitType](#__svg__SVGAngle__unitType), [valueInSpecifiedUnits](#__svg__SVGAngle__valueInSpecifiedUnits) and [valueAsString](#__svg__SVGAngle__valueAsString) might be modified as a result of this method.

**Parameters**
:

1.
unsigned short *unitType*
The unit type to switch to (e.g., [SVG_ANGLETYPE_DEG](#__svg__SVGAngle__SVG_ANGLETYPE_DEG)).

**Exceptions**
:

**DOMException, code NOT_SUPPORTED_ERR**
: Raised if unitType is SVG_ANGLETYPE_UNKNOWN or not a valid unit type constant (one of the other SVG_ANGLETYPE_* constants defined on this interface).
**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the angle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyAngle).
### <a id="InterfaceSVGAnimatedAngle"></a>4.5.16 Interface SVGAnimatedAngle

Used for attributes of basic data type [<angle>](#DataTypeAngle) that can be animated.

```
interface SVGAnimatedAngle {
  readonly attribute SVGAngle baseVal;
  readonly attribute SVGAngle animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedAngle__baseVal"></a>**baseVal** (readonly [SVGAngle](#InterfaceSVGAngle))**
:

The base value of the given attribute before applying any animations.

**<a id="__svg__SVGAnimatedAngle__animVal"></a>**animVal** (readonly [SVGAngle](#InterfaceSVGAngle))**
:

A [read only](#ReadOnlyAngle) [SVGAngle](#InterfaceSVGAngle) representing the current animated value of the given attribute. If the given attribute is not currently being animated, then the [SVGAngle](#InterfaceSVGAngle) will have the same contents as [baseVal](#__svg__SVGAnimatedAngle__baseVal). The object referenced by [animVal](#__svg__SVGAnimatedAngle__animVal) will always be distinct from the one referenced by [baseVal](#__svg__SVGAnimatedAngle__baseVal), even when the attribute is not animated.

### <a id="InterfaceSVGColor"></a>4.5.17 Interface SVGColor

The [SVGColor](#InterfaceSVGColor) interface corresponds to color value definition for properties [‘stop-color’](pservers.md#StopColorProperty), [‘flood-color’](filters.md#FloodColorProperty) and [‘lighting-color’](filters.md#LightingColorProperty) and is a base class for interface [SVGPaint](painting.md#InterfaceSVGPaint). It incorporates SVG's extended notion of color, which incorporates ICC-based color specifications.

Interface [SVGColor](#InterfaceSVGColor) does *not* correspond to the [<color>](#DataTypeColor) basic data type. For the [<color>](#DataTypeColor) basic data type, the applicable DOM interfaces are defined in DOM Level 2 Style; in particular, see the RGBColor interface ([[DOM2STYLE](refs.md#ref-DOM2STYLE)], section 2.2).

Note: The [SVGColor](#InterfaceSVGColor) interface is deprecated, and may be dropped from future versions of the SVG specification.

```
interface SVGColor : CSSValue {

  // Color Types
  const unsigned short SVG_COLORTYPE_UNKNOWN = 0;
  const unsigned short SVG_COLORTYPE_RGBCOLOR = 1;
  const unsigned short SVG_COLORTYPE_RGBCOLOR_ICCCOLOR = 2;
  const unsigned short SVG_COLORTYPE_CURRENTCOLOR = 3;

  readonly attribute unsigned short colorType;
  readonly attribute RGBColor rgbColor;
  readonly attribute SVGICCColor iccColor;

  void setRGBColor(in DOMString rgbColor) raises(SVGException);
  void setRGBColorICCColor(in DOMString rgbColor, in DOMString iccColor) raises(SVGException);
  void setColor(in unsigned short colorType, in DOMString rgbColor, in DOMString iccColor) raises(SVGException);
};
```

**Constants in group “Color Types”:**
:

**<a id="__svg__SVGColor__SVG_COLORTYPE_UNKNOWN"></a>**SVG_COLORTYPE_UNKNOWN** (unsigned short)**
:

The color type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGColor__SVG_COLORTYPE_RGBCOLOR"></a>**SVG_COLORTYPE_RGBCOLOR** (unsigned short)**
:

An sRGB color has been specified without an alternative ICC color specification.

**<a id="__svg__SVGColor__SVG_COLORTYPE_RGBCOLOR_ICCCOLOR"></a>**SVG_COLORTYPE_RGBCOLOR_ICCCOLOR** (unsigned short)**
:

An sRGB color has been specified along with an alternative ICC color specification.

**<a id="__svg__SVGColor__SVG_COLORTYPE_CURRENTCOLOR"></a>**SVG_COLORTYPE_CURRENTCOLOR** (unsigned short)**
:

Corresponds to when keyword currentColor has been specified.

**Attributes:**
:

**<a id="__svg__SVGColor__colorType"></a>**colorType** (readonly unsigned short)**
:

The type of the value as specified by one of the SVG_COLORTYPE_* constants defined on this interface.

**<a id="__svg__SVGColor__rgbColor"></a>**rgbColor** (readonly RGBColor)**
:

The color specified in the sRGB color space.

**<a id="__svg__SVGColor__iccColor"></a>**iccColor** (readonly [SVGICCColor](#InterfaceSVGICCColor))**
:

The alternate ICC color specification.

**Operations:**
:

**<a id="__svg__SVGColor__setRGBColor"></a>void **setRGBColor**(in DOMString *rgbColor*)**
:

Modifies the color value to be the specified sRGB color without an alternate ICC color specification.

**Parameters**
:

1.
DOMString *rgbColor*
A string that matches [<color>](#DataTypeColor), which specifies the new sRGB color value.

**Exceptions**
:

**[SVGException](https://www.w3.org/TR/SVG11/svgdom.html#ExceptionSVGException), code SVG_INVALID_VALUE_ERR**
: Raised if *rgbColor* does not match [<color>](#DataTypeColor).
**<a id="__svg__SVGColor__setRGBColorICCColor"></a>void **setRGBColorICCColor**(in DOMString *rgbColor*, in DOMString *iccColor*)**
:

Modifies the color value to be the specified sRGB color with an alternate ICC color specification.

**Parameters**
:

1.
DOMString *rgbColor*
A string that matches [<color>](#DataTypeColor), which specifies the new sRGB color value.
2.
DOMString *iccColor*
A string that matches [<icccolor>](#DataTypeICCColor), which specifies the alternate ICC color specification.

**Exceptions**
:

**[SVGException](https://www.w3.org/TR/SVG11/svgdom.html#ExceptionSVGException), code SVG_INVALID_VALUE_ERR**
: Raised if *rgbColor* does not match [<color>](#DataTypeColor) or if *iccColor* does not match [<icccolor>](#DataTypeICCColor).
**<a id="__svg__SVGColor__setColor"></a>void **setColor**(in unsigned short *colorType*, in DOMString *rgbColor*, in DOMString *iccColor*)**
:

Sets the color value as specified by the parameters. If *colorType* requires an RGBColor, then *rgbColor* must be a string that matches [<color>](#DataTypeColor); otherwise, *rgbColor*. must be null. If *colorType* requires an [SVGICCColor](#InterfaceSVGICCColor), then *iccColor* must be a string that matches [<icccolor>](#DataTypeICCColor); otherwise, *iccColor* must be null.

**Parameters**
:

1.
unsigned short *colorType*
One of the defined constants for [colorType](#__svg__SVGColor__colorType).
2.
DOMString *rgbColor*
The specification of an sRGB color, or null.
3.
DOMString *iccColor*
The specification of an ICC color, or null.

**Exceptions**
:

**[SVGException](https://www.w3.org/TR/SVG11/svgdom.html#ExceptionSVGException), code SVG_INVALID_VALUE_ERR**
: Raised if one of the parameters has an invalid value.
### <a id="InterfaceSVGICCColor"></a>4.5.18 Interface SVGICCColor

The [SVGICCColor](#InterfaceSVGICCColor) interface expresses an ICC-based color specification.

Note: The [SVGICCColor](#InterfaceSVGICCColor) interface is deprecated, and may be dropped from future versions of the SVG specification.

```
interface SVGICCColor {
           attribute DOMString colorProfile setraises(DOMException);
  readonly attribute SVGNumberList colors;
};
```

**Attributes:**
:

**<a id="__svg__SVGICCColor__colorProfile"></a>**colorProfile** (DOMString)**
:

The name of the color profile, which is the first parameter of an ICC color specification.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGICCColor__colors"></a>**colors** (readonly [SVGNumberList](#InterfaceSVGNumberList))**
:

The list of color values that define this ICC color. Each color value is an arbitrary floating point number.

### <a id="InterfaceSVGRect"></a>4.5.19 Interface SVGRect

Represents rectangular geometry. Rectangles are defined as consisting of a (x,y) coordinate pair identifying a minimum X value, a minimum Y value, and a width and height, which are usually constrained to be non-negative.

<a id="ReadOnlyRect"></a>An [SVGRect](#InterfaceSVGRect) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

```
interface SVGRect {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float width setraises(DOMException);
  attribute float height setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGRect__x"></a>**x** (float)**
:

The *x* coordinate of the rectangle, in user units.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the rectangle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyRect).
**<a id="__svg__SVGRect__y"></a>**y** (float)**
:

The *y* coordinate of the rectangle, in user units.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the rectangle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyRect).
**<a id="__svg__SVGRect__width"></a>**width** (float)**
:

The *width* coordinate of the rectangle, in user units.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the rectangle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyRect).
**<a id="__svg__SVGRect__height"></a>**height** (float)**
:

The *height* coordinate of the rectangle, in user units.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the rectangle corresponds to a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes) or when the object itself is [read only](#ReadOnlyRect).
### <a id="InterfaceSVGAnimatedRect"></a>4.5.20 Interface SVGAnimatedRect

Used for attributes of type [SVGRect](#InterfaceSVGRect) which can be animated.

```
interface SVGAnimatedRect {
  readonly attribute SVGRect baseVal;
  readonly attribute SVGRect animVal;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedRect__baseVal"></a>**baseVal** (readonly [SVGRect](#InterfaceSVGRect))**
:

The base value of the given attribute before applying any animations.

**<a id="__svg__SVGAnimatedRect__animVal"></a>**animVal** (readonly [SVGRect](#InterfaceSVGRect))**
:

A [read only](#ReadOnlyRect) [SVGRect](#InterfaceSVGRect) representing the current animated value of the given attribute. If the given attribute is not currently being animated, then the [SVGRect](#InterfaceSVGRect) will have the same contents as [baseVal](#__svg__SVGAnimatedRect__baseVal). The object referenced by [animVal](#__svg__SVGAnimatedRect__animVal) will always be distinct from the one referenced by [baseVal](#__svg__SVGAnimatedRect__baseVal), even when the attribute is not animated.

### <a id="InterfaceSVGUnitTypes"></a>4.5.21 Interface SVGUnitTypes

The [SVGUnitTypes](#InterfaceSVGUnitTypes) interface defines a commonly used set of constants and is a base interface used by [SVGGradientElement](pservers.md#InterfaceSVGGradientElement), [SVGPatternElement](pservers.md#InterfaceSVGPatternElement), [SVGClipPathElement](masking.md#InterfaceSVGClipPathElement), [SVGMaskElement](masking.md#InterfaceSVGMaskElement) and [SVGFilterElement](filters.md#InterfaceSVGFilterElement).

```
interface SVGUnitTypes {
  // Unit Types
  const unsigned short SVG_UNIT_TYPE_UNKNOWN = 0;
  const unsigned short SVG_UNIT_TYPE_USERSPACEONUSE = 1;
  const unsigned short SVG_UNIT_TYPE_OBJECTBOUNDINGBOX = 2;
};
```

**Constants in group “Unit Types”:**
:

**<a id="__svg__SVGUnitTypes__SVG_UNIT_TYPE_UNKNOWN"></a>**SVG_UNIT_TYPE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGUnitTypes__SVG_UNIT_TYPE_USERSPACEONUSE"></a>**SVG_UNIT_TYPE_USERSPACEONUSE** (unsigned short)**
:

Corresponds to value 'userSpaceOnUse'.

**<a id="__svg__SVGUnitTypes__SVG_UNIT_TYPE_OBJECTBOUNDINGBOX"></a>**SVG_UNIT_TYPE_OBJECTBOUNDINGBOX** (unsigned short)**
:

Corresponds to value 'objectBoundingBox'.

### <a id="InterfaceSVGStylable"></a>4.5.22 Interface SVGStylable

The [SVGStylable](#InterfaceSVGStylable) interface is implemented on all objects corresponding to SVG elements that can have [‘style’](styling.md#StyleAttribute), [‘class’](styling.md#ClassAttribute) and [presentation attributes](intro.md#TermPresentationAttribute) specified on them. It is thus an ancestor interface for many of the interfaces defined in this specification.

```
interface SVGStylable {

  readonly attribute SVGAnimatedString className;
  readonly attribute CSSStyleDeclaration style;

  CSSValue getPresentationAttribute(in DOMString name);
};
```

**Attributes:**
:

**<a id="__svg__SVGStylable__className"></a>**className** (readonly [SVGAnimatedString](#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘class’](styling.md#ClassAttribute) on the given element.

**<a id="__svg__SVGStylable__style"></a>**style** (readonly CSSStyleDeclaration)**
:

Corresponds to attribute [‘style’](styling.md#StyleAttribute) on the given element. If the user agent does not support [styling with CSS](styling.md#StylingWithCSS), then this attribute must always have the value of null.

**Operations:**
:

**<a id="__svg__SVGStylable__getPresentationAttribute"></a>CSSValue **getPresentationAttribute**(in DOMString *name*)**
:

Returns the base (i.e., static) value of a given [presentation attribute](intro.md#TermPresentationAttribute) as an object of type CSSValue. The returned object is live; changes to the objects represent immediate changes to the objects to which the CSSValue is attached.

Note: The `getPresentationAttribute` method is deprecated, and may be dropped from future versions of the SVG specification.

**Parameters**
:

1.
DOMString *name*
The name of the presentation attribute whose value is to be returned.

**Returns**
:

The static/base value of the given [presentation attribute](intro.md#TermPresentationAttribute) as a CSSValue, or null if the given attribute does not have a specified value.

### <a id="InterfaceSVGLocatable"></a>4.5.23 Interface SVGLocatable

Interface [SVGLocatable](#InterfaceSVGLocatable) is for all elements which either have a [‘transform’](coords.md#TransformAttribute) attribute or don't have a [‘transform’](coords.md#TransformAttribute) attribute but whose content can have a bounding box in current user space.

```
interface SVGLocatable {

  readonly attribute SVGElement nearestViewportElement;
  readonly attribute SVGElement farthestViewportElement;

  SVGRect getBBox();
  SVGMatrix getCTM();
  SVGMatrix getScreenCTM();
  SVGMatrix getTransformToElement(in SVGElement element) raises(SVGException);
};
```

**Attributes:**
:

**<a id="__svg__SVGLocatable__nearestViewportElement"></a>**nearestViewportElement** (readonly [SVGElement](#InterfaceSVGElement))**
:

The element which established the current viewport. Often, the nearest ancestor [‘svg’](struct.md#SVGElement) element. Null if the current element is the [outermost svg element](intro.md#TermOutermostSVGElement).

**<a id="__svg__SVGLocatable__farthestViewportElement"></a>**farthestViewportElement** (readonly [SVGElement](#InterfaceSVGElement))**
:

The farthest ancestor [‘svg’](struct.md#SVGElement) element. Null if the current element is the [outermost svg element](intro.md#TermOutermostSVGElement).

**Operations:**
:

**<a id="__svg__SVGLocatable__getBBox"></a>[SVGRect](#InterfaceSVGRect) **getBBox**()**
:

Returns the tight bounding box in current user space (i.e., after application of the [‘transform’](coords.md#TransformAttribute) attribute, if any) on the geometry of all contained graphics elements, exclusive of stroking, clipping, masking and filter effects). Note that getBBox must return the actual bounding box at the time the method was called, even in case the element has not yet been rendered.

**Returns**
:

An [SVGRect](#InterfaceSVGRect) object that defines the bounding box.

**<a id="__svg__SVGLocatable__getCTM"></a>[SVGMatrix](coords.md#InterfaceSVGMatrix) **getCTM**()**
:

Returns the transformation matrix from current user units (i.e., after application of the [‘transform’](coords.md#TransformAttribute) attribute, if any) to the viewport coordinate system for the [nearestViewportElement](#__svg__SVGLocatable__nearestViewportElement).

**Returns**
:

An [SVGMatrix](coords.md#InterfaceSVGMatrix) object that defines the CTM.

**<a id="__svg__SVGLocatable__getScreenCTM"></a>[SVGMatrix](coords.md#InterfaceSVGMatrix) **getScreenCTM**()**
:

Returns the transformation matrix from current user units (i.e., after application of the [‘transform’](coords.md#TransformAttribute) attribute, if any) to the parent user agent's notice of a "pixel". For display devices, ideally this represents a physical screen pixel. For other devices or environments where physical pixel sizes are not known, then an algorithm similar to the CSS2 definition of a "pixel" can be used instead. Note that null is returned if this element is not hooked into the document tree. This method would have been more aptly named as `getClientCTM`, but the name `getScreenCTM` is kept for historical reasons.

**Returns**
:

An [SVGMatrix](coords.md#InterfaceSVGMatrix) object that defines the given transformation matrix.

**<a id="__svg__SVGLocatable__getTransformToElement"></a>[SVGMatrix](coords.md#InterfaceSVGMatrix) **getTransformToElement**(in [SVGElement](#InterfaceSVGElement) *element*)**
:

Returns the transformation matrix from the user coordinate system on the current element (after application of the [‘transform’](coords.md#TransformAttribute) attribute, if any) to the user coordinate system on parameter *element* (after application of its [‘transform’](coords.md#TransformAttribute) attribute, if any).

**Parameters**
:

1.
[SVGElement](#InterfaceSVGElement) *element*
The target element.

**Returns**
:

An [SVGMatrix](coords.md#InterfaceSVGMatrix) object that defines the transformation.

**Exceptions**
:

**[SVGException](https://www.w3.org/TR/SVG11/svgdom.html#ExceptionSVGException), code SVG_MATRIX_NOT_INVERTABLE**
: Raised if the currently defined transformation matrices make it impossible to compute the given matrix (e.g., because one of the transformations is singular).
### <a id="InterfaceSVGTransformable"></a>4.5.24 Interface SVGTransformable

Interface [SVGTransformable](#InterfaceSVGTransformable) contains properties and methods that apply to all elements which have attribute [‘transform’](coords.md#TransformAttribute).

```
interface SVGTransformable : SVGLocatable {
  readonly attribute SVGAnimatedTransformList transform;
};
```

**Attributes:**
:

**<a id="__svg__SVGTransformable__transform"></a>**transform** (readonly [SVGAnimatedTransformList](coords.md#InterfaceSVGAnimatedTransformList))**
:

Corresponds to attribute [‘transform’](coords.md#TransformAttribute) on the given element.

### <a id="InterfaceSVGTests"></a>4.5.25 Interface SVGTests

Interface [SVGTests](#InterfaceSVGTests) defines an interface which applies to all elements which have attributes [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute) and [‘systemLanguage’](struct.md#SystemLanguageAttribute).

```
interface SVGTests {

  readonly attribute SVGStringList requiredFeatures;
  readonly attribute SVGStringList requiredExtensions;
  readonly attribute SVGStringList systemLanguage;

  boolean hasExtension(in DOMString extension);
};
```

**Attributes:**
:

**<a id="__svg__SVGTests__requiredFeatures"></a>**requiredFeatures** (readonly [SVGStringList](#InterfaceSVGStringList))**
:

Corresponds to attribute [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute) on the given element.

**<a id="__svg__SVGTests__requiredExtensions"></a>**requiredExtensions** (readonly [SVGStringList](#InterfaceSVGStringList))**
:

Corresponds to attribute [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute) on the given element.

**<a id="__svg__SVGTests__systemLanguage"></a>**systemLanguage** (readonly [SVGStringList](#InterfaceSVGStringList))**
:

Corresponds to attribute [‘systemLanguage’](struct.md#SystemLanguageAttribute) on the given element.

**Operations:**
:

**<a id="__svg__SVGTests__hasExtension"></a>boolean **hasExtension**(in DOMString *extension*)**
:

Returns true if the user agent supports the given extension, specified by a URI.

**Parameters**
:

1.
DOMString *extension*
The name of the extension, expressed as a URI.

**Returns**
:

True or false, depending on whether the given extension is supported.

### <a id="InterfaceSVGLangSpace"></a>4.5.26 Interface SVGLangSpace

Interface [SVGLangSpace](#InterfaceSVGLangSpace) defines an interface which applies to all elements which have attributes [‘xml:lang’](struct.md#XMLLangAttribute) and [‘xml:space’](struct.md#XMLSpaceAttribute).

```
interface SVGLangSpace {
  attribute DOMString xmllang setraises(DOMException);
  attribute DOMString xmlspace setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGLangSpace__xmllang"></a>**xmllang** (DOMString)**
:

Corresponds to attribute [‘xml:lang’](struct.md#XMLLangAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGLangSpace__xmlspace"></a>**xmlspace** (DOMString)**
:

Corresponds to attribute [‘xml:space’](struct.md#XMLSpaceAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGExternalResourcesRequired"></a>4.5.27 Interface SVGExternalResourcesRequired

Interface [SVGExternalResourcesRequired](#InterfaceSVGExternalResourcesRequired) defines an interface which applies to all elements where this element or one of its descendants can reference an external resource.

```
interface SVGExternalResourcesRequired {
  readonly attribute SVGAnimatedBoolean externalResourcesRequired;
};
```

**Attributes:**
:

**<a id="__svg__SVGExternalResourcesRequired__externalResourcesRequired"></a>**externalResourcesRequired** (readonly [SVGAnimatedBoolean](#InterfaceSVGAnimatedBoolean))**
:

Corresponds to attribute [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute) on the given element. Note that the SVG DOM defines the attribute [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute) as being of type [SVGAnimatedBoolean](#InterfaceSVGAnimatedBoolean), whereas the SVG language definition says that [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute) is not animated. Because the SVG language definition states that [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute) cannot be animated, the [animVal](#__svg__SVGAnimatedBoolean__animVal) will always be the same as the [baseVal](#__svg__SVGAnimatedBoolean__baseVal).

### <a id="InterfaceSVGFitToViewBox"></a>4.5.28 Interface SVGFitToViewBox

Interface [SVGFitToViewBox](#InterfaceSVGFitToViewBox) defines DOM attributes that apply to elements which have XML attributes [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute).

```
interface SVGFitToViewBox {
  readonly attribute SVGAnimatedRect viewBox;
  readonly attribute SVGAnimatedPreserveAspectRatio preserveAspectRatio;
};
```

**Attributes:**
:

**<a id="__svg__SVGFitToViewBox__viewBox"></a>**viewBox** (readonly [SVGAnimatedRect](#InterfaceSVGAnimatedRect))**
:

Corresponds to attribute [‘viewBox’](coords.md#ViewBoxAttribute) on the given element.

**<a id="__svg__SVGFitToViewBox__preserveAspectRatio"></a>**preserveAspectRatio** (readonly [SVGAnimatedPreserveAspectRatio](coords.md#InterfaceSVGAnimatedPreserveAspectRatio))**
:

Corresponds to attribute [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) on the given element.

### <a id="InterfaceSVGZoomAndPan"></a>4.5.29 Interface SVGZoomAndPan

The [SVGZoomAndPan](#InterfaceSVGZoomAndPan) interface defines attribute [zoomAndPan](#__svg__SVGZoomAndPan__zoomAndPan) and associated constants.

```
interface SVGZoomAndPan {

  // Zoom and Pan Types
  const unsigned short SVG_ZOOMANDPAN_UNKNOWN = 0;
  const unsigned short SVG_ZOOMANDPAN_DISABLE = 1;
  const unsigned short SVG_ZOOMANDPAN_MAGNIFY = 2;

  attribute unsigned short zoomAndPan setraises(DOMException);
};
```

**Constants in group “Zoom and Pan Types”:**
:

**<a id="__svg__SVGZoomAndPan__SVG_ZOOMANDPAN_UNKNOWN"></a>**SVG_ZOOMANDPAN_UNKNOWN** (unsigned short)**
:

The enumeration was set to a value that is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGZoomAndPan__SVG_ZOOMANDPAN_DISABLE"></a>**SVG_ZOOMANDPAN_DISABLE** (unsigned short)**
:

Corresponds to value 'disable'.

**<a id="__svg__SVGZoomAndPan__SVG_ZOOMANDPAN_MAGNIFY"></a>**SVG_ZOOMANDPAN_MAGNIFY** (unsigned short)**
:

Corresponds to value 'magnify'.

**Attributes:**
:

**<a id="__svg__SVGZoomAndPan__zoomAndPan"></a>**zoomAndPan** (unsigned short)**
:

Corresponds to attribute [‘zoomAndPan’](interact.md#ZoomAndPanAttribute) on the given element. The value must be one of the SVG_ZOOMANDPAN_* constants defined on this interface.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGViewSpec"></a>4.5.30 Interface SVGViewSpec

The interface corresponds to an SVG View Specification.

```
interface SVGViewSpec : SVGZoomAndPan,
                        SVGFitToViewBox {
  readonly attribute SVGTransformList transform;
  readonly attribute SVGElement viewTarget;
  readonly attribute DOMString viewBoxString;
  readonly attribute DOMString preserveAspectRatioString;
  readonly attribute DOMString transformString;
  readonly attribute DOMString viewTargetString;
};
```

**Attributes:**
:

**<a id="__svg__SVGViewSpec__transform"></a>**transform** (readonly [SVGTransformList](coords.md#InterfaceSVGTransformList))**
:

Corresponds to the transform setting on the SVG View Specification.

**<a id="__svg__SVGViewSpec__viewTarget"></a>**viewTarget** (readonly [SVGElement](#InterfaceSVGElement))**
:

Corresponds to the viewTarget setting on the SVG View Specification.

**<a id="__svg__SVGViewSpec__viewBoxString"></a>**viewBoxString** (readonly DOMString)**
:

Corresponds to the viewBox setting on the SVG View Specification.

**<a id="__svg__SVGViewSpec__preserveAspectRatioString"></a>**preserveAspectRatioString** (readonly DOMString)**
:

Corresponds to the preserveAspectRatio setting on the SVG View Specification.

**<a id="__svg__SVGViewSpec__transformString"></a>**transformString** (readonly DOMString)**
:

Corresponds to the transform setting on the SVG View Specification.

**<a id="__svg__SVGViewSpec__viewTargetString"></a>**viewTargetString** (readonly DOMString)**
:

Corresponds to the viewTarget setting on the SVG View Specification.

### <a id="InterfaceSVGURIReference"></a>4.5.31 Interface SVGURIReference

Interface [SVGURIReference](#InterfaceSVGURIReference) defines an interface which applies to all elements which have the collection of XLink attributes, such as ‘xlink:href’, which define a URI reference.

```
interface SVGURIReference {
  readonly attribute SVGAnimatedString href;
};
```

**Attributes:**
:

**<a id="__svg__SVGURIReference__href"></a>**href** (readonly [SVGAnimatedString](#InterfaceSVGAnimatedString))**
:

Corresponds to attribute ‘xlink:href’ on the given element.

### <a id="InterfaceSVGCSSRule"></a>4.5.32 Interface SVGCSSRule

SVG extends interface CSSRule with interface [SVGCSSRule](#InterfaceSVGCSSRule) by adding an [SVGColorProfileRule](color.md#InterfaceSVGColorProfileRule) rule to allow for specification of ICC-based color.

It is likely that this extension will become part of a future version of CSS and DOM.

```
interface SVGCSSRule : CSSRule {
  const unsigned short COLOR_PROFILE_RULE = 7;
};
```

**Constants:**
:

**<a id="__svg__SVGCSSRule__COLOR_PROFILE_RULE"></a>**COLOR_PROFILE_RULE** (unsigned short)**
:

The rule is an @color-profile.

### <a id="InterfaceSVGRenderingIntent"></a>4.5.33 Interface SVGRenderingIntent

The [SVGRenderingIntent](#InterfaceSVGRenderingIntent) interface defines the enumerated list of possible values for [‘rendering-intent’](color.md#ColorProfileElementRenderingIntentAttribute) attributes or descriptors.

```
interface SVGRenderingIntent {
  // Rendering Intent Types
  const unsigned short RENDERING_INTENT_UNKNOWN = 0;
  const unsigned short RENDERING_INTENT_AUTO = 1;
  const unsigned short RENDERING_INTENT_PERCEPTUAL = 2;
  const unsigned short RENDERING_INTENT_RELATIVE_COLORIMETRIC = 3;
  const unsigned short RENDERING_INTENT_SATURATION = 4;
  const unsigned short RENDERING_INTENT_ABSOLUTE_COLORIMETRIC = 5;
};
```

**Constants in group “Rendering Intent Types”:**
:

**<a id="__svg__SVGRenderingIntent__RENDERING_INTENT_UNKNOWN"></a>**RENDERING_INTENT_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGRenderingIntent__RENDERING_INTENT_AUTO"></a>**RENDERING_INTENT_AUTO** (unsigned short)**
:

Corresponds to a value of 'auto'.

**<a id="__svg__SVGRenderingIntent__RENDERING_INTENT_PERCEPTUAL"></a>**RENDERING_INTENT_PERCEPTUAL** (unsigned short)**
:

Corresponds to a value of 'perceptual'.

**<a id="__svg__SVGRenderingIntent__RENDERING_INTENT_RELATIVE_COLORIMETRIC"></a>**RENDERING_INTENT_RELATIVE_COLORIMETRIC** (unsigned short)**
:

Corresponds to a value of 'relative-colorimetric'.

**<a id="__svg__SVGRenderingIntent__RENDERING_INTENT_SATURATION"></a>**RENDERING_INTENT_SATURATION** (unsigned short)**
:

Corresponds to a value of 'saturation'.

**<a id="__svg__SVGRenderingIntent__RENDERING_INTENT_ABSOLUTE_COLORIMETRIC"></a>**RENDERING_INTENT_ABSOLUTE_COLORIMETRIC** (unsigned short)**
:

Corresponds to a value of 'absolute-colorimetric'.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](render.md) ⋅ [Next](struct.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
