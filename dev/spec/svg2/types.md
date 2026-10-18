<!-- https://svgwg.org/svg2-draft/types.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 4: Basic Data Types and Interfaces

## <a id="definitions"></a>4.1. Definitions

***<a id="TermInitialValue"></a>initial value***
:

The initial value of an attribute or property is the value used when that attribute or property is not specified, or when it has an [invalid value](#TermInvalidValue). This value is to be used for the purposes of rendering, calculating animation values, and when accessing the attribute or property via DOM interfaces.

***<a id="TermInvalidValue"></a>invalid value***
: An invalid value specified for a [property](styling.md#TermProperty), either in a style sheet or a [presentation attribute](styling.md#TermPresentationAttribute), is one that is either not allowed according to the grammar defining the property's values, or is allowed by the grammar but subsequently disallowed in prose. A CSS declaration with an invalid value is [ignored](../css-syntax-3.md#css-ignored).
## <a id="syntax"></a>4.2. Attribute syntax

In this specification, attributes are defined with an attribute definition table, which looks like this:

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *exampleattr* | <length> \| none | none | yes |

In the Value column is a description of the attribute's syntax. There are six methods for describing an attribute's syntax:

1. Using the [CSS Value Definition Syntax](../css-values-4.md#value-defs) [[css-values](refs.md#ref-css-values-3)]. This is the notation used to define the syntax for most attributes in this specification and is the default.
2. By reference to an EBNF symbol defined in this or another specification [[xml](refs.md#ref-xml)]. For external definitions, this is indicated by [EBNF] appearing in the Value column.
3. By reference to an ABNF symbol defined in another specification [[rfc5234](refs.md#ref-rfc5234)]. This is indicated by [ABNF] appearing in the Value column.
4. As a URL as defined by the URL Standard [[URL](refs.md#ref-URL)]. This is indicated by [URL] appearing in the Value column.
5. As a type as defined by the HTML Standard [[HTML](refs.md#ref-html)]. This is indicated by [HTML] appearing in the Value column.
6. In prose, below the attribute definition table. This is indicated by the text "(see below)" appearing in the Value column.

<a id="presentation-attribute-css-value"></a>When a [presentation attribute](styling.md#TermPresentationAttribute) defined using the CSS Value Definition Syntax is parsed, this is done as follows:

1. Let *value* be the value of the attribute.
2. Let *grammar* be the grammar given in the attribute definition table's Value column.
3. Replace all instances of <length> in *grammar* with [<length> | <number>].
4. Replace all instances of [<length-percentage>](../css-values-4.md#typedef-length-percentage) in *grammar* with [[<length-percentage>](../css-values-4.md#typedef-length-percentage) | <number>].
5. Replace all instances of [<angle>](../css-values-4.md#angles) in *grammar* with [[<angle>](../css-values-4.md#angles) | <number>].
6. Return the result of [parsing *value* with *grammar*](../css-syntax-3.md#parse-grammar).

The insertion of the <number> symbols allows for unitless length and angles to be used in presentation attribute while disallowing them in corresponding property values.

Note that all [presentation attributes](styling.md#TermPresentationAttribute), since they are defined by reference to their corresponding CSS properties, are defined using the CSS Value Definition Syntax.

<a id="attribute-css-value"></a>When any other attribute defined using the CSS Value Definition Syntax is parsed, this is done by [parsing the attribute's value according to the grammar given in attribute definition table](../css-syntax-3.md#parse-grammar).

Note that this allows CSS comments and escapes to be used in such attributes. For example, a value of '10\px/**/' would successfully parse as '10px' in the ‘x’ presentation attribute of the ‘[rect](shapes.md#RectElement)’ element.

<a id="attribute-url"></a>When an attribute defined as a URL is parsed, this is done by invoking the URL parser with the attribute's value as *input* and the document's URL as *base* [[URL](refs.md#ref-URL)].

The Initial value column gives the [initial value](#TermInitialValue) for the attribute. When an attribute fails to parse according to the specified CSS Value Definition Syntax, ABNF or EBNF grammar, or if parsing according to the URL Standard or by the prose describing how to parse the attribute indicates failure, the attribute is assumed to have been specified as the given [initial value](#TermInitialValue).

The initial value of a [presentation attribute](styling.md#TermPresentationAttribute) is its corresponding property's initial value. Since the use of an [invalid value](#TermInvalidValue) in a [presentation attribute](styling.md#TermPresentationAttribute) will be treated as if the initial value was specified, this value can override values that come from lower priority style sheet rules, such as those from the user agent style sheet.

For example, although the user agent style sheet sets the value of the [overflow](render.md#OverflowAndClipProperties) property to hidden for ‘[svg](struct.md#SVGElement)’ elements, specifying an invalid presentation attribute such as overflow="invalid" will result in a rule setting [overflow](render.md#OverflowAndClipProperties) to visible, overriding the user agent style sheet value.

The Animatable column indicates whether the attribute can be animated using animation elements as defined in the SVG Animation module.

### <a id="Precision"></a>4.2.1. Real number precision

Unless stated otherwise, numeric values in SVG attributes and in properties that are defined to have an effect on SVG elements must support at least all finite single-precision values supported by the host architecture.

It is recommended that higher precision floating point storage and computation be performed on operations such as coordinate system transformations to provide the best possible precision and to prevent round-off errors.

[conforming SVG viewers](conform.md#ConformingSVGViewers) are required to perform numerical computation in accordance with their conformance class, as described in [Conformance Criteria](conform.md).

### <a id="RangeClamping"></a>4.2.2. Clamping values which are restricted to a particular range

Some numeric attribute and property values have restricted ranges. Other values will be restricted by the capabilities of the device. If not otherwise specified, the user agent shall defer any out-of-range error checking until as late as possible in the rendering process. This is particularly important for device limitations, as compound operations might produce intermediate values which are out-of-range but final values which are within range.

## <a id="SVGDOMOverview"></a>4.3. SVG DOM overview

Conforming [SVG viewers](conform.md#TermSVGViewer) or [SVG interpreters](conform.md#TermSVGInterpreter) that support [script execution](conform.md#processing-modes) must implement SVG DOM interfaces as defined throughout this specification, with the specific requirements and dependencies listed in this section.

### <a id="SVGDOMDependencies"></a>4.3.1. Dependencies for SVG DOM support

The SVG DOM is defined in terms of Web IDL interfaces. All IDL fragments in this specification must be interpreted as required for conforming IDL fragments, as described in the Web IDL specification. [[WebIDL](refs.md#ref-webidl)]

The SVG DOM builds upon a number of DOM specifications. In particular:

- The SVG DOM requires full support for the DOM, as defined by the most recent Review Draft of the DOM living standard at the time this specification was published, except for any features that have been removed or deprecated since. SVG software with DOM support should implement the latest DOM standard wherever possible [[DOM](refs.md#ref-dom)]
- The SVG DOM requires support for relevant aspects of UI Events and Clipboard API and events ([[uievents](refs.md#ref-uievents)], [[clipboard-apis](refs.md#ref-clipboard-apis)]). (For the specific features that are required, see [Relationship with UI Events](interact.md#RelationshipWithUIEVENTS).)
- The SVG DOM requires, at a minimum, complete support for all interfaces from DOM Level 2 Style ([[dom-level-2-style](refs.md#ref-dom-level-2-style)]) that have not been dropped in the CSS Object Model (CSSOM) specification. SVG software should implement the latest version of CSS Object Model (CSSOM) wherever possible ([[cssom-1](refs.md#ref-cssom-1)])
- The SVG DOM requires complete support for Geometry Interfaces Module Level 1 ([[geometry-1](refs.md#ref-geometry-1)]).

### <a id="SVGDOMNamingConventions"></a>4.3.2. Naming conventions

The SVG DOM follows similar naming conventions to HTML and DOM standards ([[HTML](refs.md#ref-html)], [[DOM](refs.md#ref-dom)]).

All names are defined as one or more English words concatenated together to form a single string. Property or method names start with the initial keyword in lowercase, and each subsequent word starts with a capital letter. For example, a property that returns document meta information such as the date the file was created might be named "fileDateCreated".

Interface names defined in this specification nearly all start with "SVG". Interfaces that represent the DOM Element object for an SVG-namespaced element follow the format `SVG*ElementName*Element`, where `*ElementName*` is the element's tag name with the initial letter capitalized. So [SVGRadialGradientElement](pservers.md#InterfaceSVGRadialGradientElement) is the interface for an ‘[radialGradient](pservers.md#RadialGradientElement)’ element.

An exception to this casing convention is [SVGSVGElement](struct.md#InterfaceSVGSVGElement), in which the entire tag name is capitalized.

### <a id="ElementsInTheSVGDOM"></a>4.3.3. Elements in the SVG DOM

Any SVG software that is required to support the SVG DOM must enhance the DOM elements created for [SVG document fragments](struct.md#TermSVGDocumentFragment) as follows:

-
Every Element object that corresponds to a supported SVG element (that is, an element with namespace URI "http://www.w3.org/2000/svg" and a local name that is one of the elements defined in this specification or another specification implemented by the software) must also implement the DOM interface identified in the element definition.
-
Elements in the SVG namespace whose local name does not match an element defined in any specification supported by the software must nonetheless implement the [SVGElement](#InterfaceSVGElement) interface.

In [The ‘rect’ element](shapes.md#RectElement), the [SVGRectElement](shapes.md#InterfaceSVGRectElement) interface is identified. This means that every Element object whose namespace URI is "http://www.w3.org/2000/svg" and whose local name is "rect" must also implement [SVGRectElement](shapes.md#InterfaceSVGRectElement).

### <a id="ReflectingAttributes"></a>4.3.4. Reflecting content attributes in the DOM

Many SVG DOM properties (IDL attributes) *<a id="TermReflect"></a>reflect* a content attribute or property on the corresponding element, meaning that content and IDL attributes represent the same underlying data. For example, the [SVGAnimatedLength](#InterfaceSVGAnimatedLength) [ry](shapes.md#__svg__SVGRectElement__ry) in an [SVGRectElement](shapes.md#InterfaceSVGRectElement) reflects the [ry](geometry.md#RyProperty) presentation attribute on the associated ‘[rect](shapes.md#RectElement)’ element.

The way this reflection is done depends on the type of the IDL attribute:

- If the type of the reflecting IDL attribute is a primitive type (such as **long**, as used by tabIndex on [SVGElement](#InterfaceSVGElement)) or **DOMString** (as used by [title](styling.md#__svg__SVGStyleElement__title) on [SVGStyleElement](styling.md#InterfaceSVGStyleElement)), then the rules for reflecting content attributes in IDL attributes in HTML are used.
- If the type of the reflecting IDL attribute is an [SVGAnimatedBoolean](#InterfaceSVGAnimatedBoolean), [SVGAnimatedString](#InterfaceSVGAnimatedString), [SVGAnimatedEnumeration](#InterfaceSVGAnimatedEnumeration), [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber), [SVGAnimatedNumberList](#InterfaceSVGAnimatedNumberList), [SVGAnimatedLength](#InterfaceSVGAnimatedLength), [SVGAnimatedLengthList](#InterfaceSVGAnimatedLengthList), [SVGAnimatedAngle](#InterfaceSVGAnimatedAngle), [SVGAnimatedRect](#InterfaceSVGAnimatedRect) or [SVGAnimatedPreserveAspectRatio](coords.md#InterfaceSVGAnimatedPreserveAspectRatio), then the rules in the corresponding section defining the reflecting interface are used.
At a high level, the object's `baseVal` is used to reflect the value of the content attribute. For objects that reflect a CSS property, the `baseVal` is used to reflect the presentation attribute.

This relationship is live, and values must be synchronized (following the rules in [Synchronizing reflected values](#SynchronizingReflectedValues)) when either the attribute or its reflected property is modified.

If the attribute hasn't been specified explicitly in the document markup, the reflected object is nonetheless initialized upon access, to the attribute's initial value. If the attribute's initial value is (none), the object is initialized as defined in [Reflecting an empty initial value](#SVGObjectInitialization). This newly constructed object does not generate an attribute on the element until it is modified for the first time. Modifications made to the corresponding attribute are immediately reflected in the object.

If `lineElement.x1.baseVal` is accessed (where `lineElement` is an instance of [SVGLineElement](shapes.md#InterfaceSVGLineElement)) and the ‘[x1](shapes.md#LineElementX1Attribute)’ attribute was not specified in the document, the returned [SVGLength](#InterfaceSVGLength) object would represent the value 0 user units, because the initial value for the attribute is 0.

### <a id="SynchronizingReflectedValues"></a>4.3.5. Synchronizing reflected values

Whenever a reflected content attribute's base value changes, then the reflecting object must be *<a id="TermSynchronize"></a>synchronized*, immediately after the value changed, by running the following steps:

1. If the reflecting object is a [list interface](#TermListInterface) object, then run the steps for [synchronizing a list interface object](#TermSynchronizeList).
2. Otherwise, update the object's value to be the base value of the reflected content attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
This will, for example, update the [value](#LengthValue) of an [SVGLength](#InterfaceSVGLength) object.

When a [reflected](#TermReflect) content attribute is to be *<a id="TermReserialize"></a>reserialized*, optionally using a specific value, the following steps must be performed:

1. Let *value* be the specific value given, or the value of the content attribute's reflecting IDL attribute if a specific value was not provided.
2. Depending on *value*'s type:
**[SVGAnimatedBoolean](#InterfaceSVGAnimatedBoolean)**
**[SVGAnimatedNumber](#InterfaceSVGAnimatedNumber)**
**[SVGAnimatedLength](#InterfaceSVGAnimatedLength)**
**[SVGAnimatedAngle](#InterfaceSVGAnimatedAngle)**
**[SVGAnimatedRect](#InterfaceSVGAnimatedRect)**
**[SVGAnimatedString](#InterfaceSVGAnimatedString)**
**[SVGAnimatedNumberList](#InterfaceSVGAnimatedNumberList)**
**[SVGAnimatedLengthList](#InterfaceSVGAnimatedLengthList)**
**[SVGAnimatedTransformList](coords.md#InterfaceSVGAnimatedTransformList)**
: [Reserialize](#TermReserialize) the content attribute using *value*'s baseVal member.
**[SVGAnimatedEnumeration](#InterfaceSVGAnimatedEnumeration)**
:
  1. Let *number* be the value of *value*'s baseVal member.
  2. Let *keyword* be the content attribute's keyword value corresponding to *number*, or the empty string if *number* is 0.
  This means that if the enumeration value is somehow set to the "unknown" value, the content attribute will be set to the empty string. However, this unknown value can never be set directly on the `SVGAnimatedEnumeration` object; it represents an unknown attribute value set in the markup.
  3. Set the content attribute to *keyword*.
**boolean**
: Set the content attribute to "true" if *value* is true, and "false" otherwise.
**float**
**double**
: Set the content attribute to an implementation specific string that, if parsed as a <number> using CSS syntax, would return the number value closest to *value*, given the implementation's supported [real number precision](#Precision).
**[SVGLength](#InterfaceSVGLength)**
: Set the content attribute to the value that would be returned from getting *value*'s [valueAsString](#__svg__SVGLength__valueAsString) member.
**[SVGAngle](#InterfaceSVGAngle)**
: Set the content attribute to the value that would be returned from getting *value*'s [valueAsString](#__svg__SVGAngle__valueAsString) member.
**DOMRect**
:
  1. Let *components* be a list of four values, being the values of the x, y, width and height members of *value*.
  2. Let *serialized components* be a list of four strings, where each is an implementation specific string that, if parsed as a <number> using CSS syntax, would return the number value closest to the corresponding value in *components*, given the implementation's supported [real number precision](#Precision).
  3. Set the content attribute to a string consisting of the strings in *serialized components* joined and separated by single U+0020 SPACE characters.
**DOMString**
: Set the content attribute to *value*.
**[SVGNumberList](#InterfaceSVGNumberList)**
**[SVGLengthList](#InterfaceSVGLengthList)**
**[SVGPointList](shapes.md#InterfaceSVGPointList)**
**[SVGTransformList](coords.md#InterfaceSVGTransformList)**
**[SVGStringList](#InterfaceSVGStringList)**
:
  1. Let *elements* be the list of values in *value*.
  The values will be [SVGNumber](#InterfaceSVGNumber), [SVGLength](#InterfaceSVGLength), DOMPoint or [SVGTransform](coords.md#InterfaceSVGTransform) objects, or **DOMString** values, depending on *value*'s type.
  2. Let *serialized elements* be a list of strings, where each string is formed based on the corresponding value in *elements* and its type:
  **an [SVGNumber](#InterfaceSVGNumber) object**
  : The string is an implementation specific string that, if parsed as a <number> using CSS syntax, would return the number value closest to the [SVGNumber](#InterfaceSVGNumber) object's [value](#__svg__SVGNumber__value) member, given the implementation's supported [real number precision](#Precision).
  **an [SVGLength](#InterfaceSVGLength) object**
  : The string is the value that would be returned from getting the value's [valueAsString](#__svg__SVGLength__valueAsString) member.
  **a DOMPoint object**
  : The string value is computed as follows:
    1. Let *string* be an empty string.
    2. Let *x* and *y* be the values of the DOMPoint object's x and y coordinates, respectively.
    3. Append to *string* an implementation specific string that, if parsed as <number> using CSS syntax, would return the number value closest to *x*, given the implementation's supported [real number precision](#Precision).
    4. Append a single U+002C COMMA character to *string*.
    5. Append to *string* an implementation specific string that, if parsed as <number> using CSS syntax, would return the number value closest to *y*, given the implementation's supported [real number precision](#Precision).
    6. The string is *string*.
  **a [SVGTransform](coords.md#InterfaceSVGTransform) object**
  : The string is the [serialization](../css-transforms-1.md#serialization-of-transform-functions) of the [SVGTransform](coords.md#InterfaceSVGTransform) object's [matrix object](coords.md#TransformMatrixObject).
  **a **DOMString****
  : The string is the **DOMString**'s value itself.
  3. Set the content attribute to a string consisting of the strings in *serialized elements* joined and separated by single U+0020 SPACE characters.

### <a id="SVGObjectInitialization"></a>4.3.6. Reflecting an empty initial value

When initializing an SVG DOM attribute that [reflects](#TermReflect) a null or empty initial value, then the property must be initialized according to its data type, as defined in this section. This occurs only if there is no explicit value for the reflected content attribute, and the initial value in the attribute's definition table is (none).

If an interface is not listed below that means that the object initialization shall be done using the values for the objects that the interface contains, e.g., an [SVGAnimatedString](#InterfaceSVGAnimatedString) consists of two DOMString members, while a DOMRect consists of many doubles.

<a id="SVGObjectInitValues"></a>

**DOMString**
: Initialized as the empty string ("").
**float**
**long**
**short**
**Any other numeric type defined in WebIDL**
: Initialized as 0.
**boolean**
: Initialized as false.
**[SVGLength](#InterfaceSVGLength)**
: Initialized as 0 user units ([SVG_LENGTHTYPE_NUMBER](#__svg__SVGLength__SVG_LENGTHTYPE_NUMBER)).
**[SVGLengthList](#InterfaceSVGLengthList)**
**[SVGNumberList](#InterfaceSVGNumberList)**
**[SVGPointList](shapes.md#InterfaceSVGPointList)**
**[SVGStringList](#InterfaceSVGStringList)**
**[SVGTransformList](coords.md#InterfaceSVGTransformList)**
: Initialized as the empty list.
**[SVGAngle](#InterfaceSVGAngle)**
: Initialized as 0 in unspecified units ([SVG_ANGLETYPE_UNSPECIFIED](#__svg__SVGAngle__SVG_ANGLETYPE_UNSPECIFIED)).
**[SVGPreserveAspectRatio](coords.md#InterfaceSVGPreserveAspectRatio)**
: Initialized as 'xMidYMid meet'.
If `textElement.dx.baseVal` is accessed (where `textElement` is an instance of [SVGTextElement](text.md#InterfaceSVGTextElement)) and the ‘[dx](text.md#TextElementDXAttribute)’ attribute was not specified in the document, the returned [SVGLengthList](#InterfaceSVGLengthList) object would be empty.

### <a id="InvalidValues"></a>4.3.7. Invalid values

If a script sets a [reflected](#TermReflect) DOM attribute to an [invalid value](#TermInvalidValue) for the content attribute (e.g., a negative number for an attribute that requires a non-negative number), unless this specification indicates otherwise, no exception shall be raised on setting, but the given document fragment shall become technically *in error* as described in [Error processing](conform.md#ErrorProcessing).

DOM attributes that reflect enumerated values using integer constants are an exception: these throw a TypeError when set to an out-of-range integer, or to the constant (0) that represents an unknown attribute value. This is consistent with the behavior of the WebIDL enumeration type [[WebIDL](refs.md#ref-webidl)].

## <a id="DOMInterfacesForSVGElements"></a>4.4. DOM interfaces for SVG elements

### <a id="InterfaceSVGElement"></a>4.4.1. Interface SVGElement

All of the SVG DOM interfaces that correspond directly to elements in the SVG language (such as the [SVGPathElement](paths.md#InterfaceSVGPathElement) interface for the ‘[path](paths.md#PathElement)’ element) derive from the [SVGElement](#InterfaceSVGElement) interface.

The CSSOM specification augments SVGElement with a style IDL attribute, so that the ‘[style](styling.md#StyleAttribute)’ attribute can be accessed in the same way as on HTML elements.

```
[Exposed=Window]
interface SVGElement : Element {

  [SameObject] readonly attribute SVGAnimatedString className;

  readonly attribute SVGSVGElement? ownerSVGElement;
  readonly attribute SVGElement? viewportElement;
};

SVGElement includes GlobalEventHandlers;
SVGElement includes SVGElementInstance;
SVGElement includes HTMLOrSVGElement;
```

The **<a id="__svg__SVGElement__className"></a>className** IDL attribute [reflects](#TermReflect) the ‘[class](styling.md#ClassAttribute)’ attribute.

This attribute is deprecated and may be removed in a future version of this specification. Authors are advised to use Element.classList instead.

The [className](#__svg__SVGElement__className) attribute on [SVGElement](#InterfaceSVGElement) overrides the correspond attribute on Element, following the WebIDL rules for inheritance.

The **<a id="__svg__SVGElement__ownerSVGElement"></a>ownerSVGElement** IDL attribute represents the nearest ancestor ‘[svg](struct.md#SVGElement)’ element. On getting [ownerSVGElement](#__svg__SVGElement__ownerSVGElement), the nearest ancestor ‘[svg](struct.md#SVGElement)’ element is returned; if the current element is the [outermost svg element](struct.md#TermOutermostSVGElement), then null is returned.

The **<a id="__svg__SVGElement__viewportElement"></a>viewportElement** IDL attribute represents the element that provides the SVG viewport for the current element. On getting [viewport](#__svg__SVGElement__viewportElement), the nearest ancestor element that establishes an SVG viewport is returned; if the current element is the [outermost svg element](struct.md#TermOutermostSVGElement), then null is returned.

### <a id="InterfaceSVGGraphicsElement"></a>4.4.2. Interface SVGGraphicsElement

The [SVGGraphicsElement](#InterfaceSVGGraphicsElement) interface represents SVG elements whose primary purpose is to directly render graphics into a group.

```
dictionary <a id="SVGBoundingBoxOptions"></a>SVGBoundingBoxOptions {
  boolean fill = true;
  boolean stroke = false;
  boolean markers = false;
  boolean clipped = false;
};

[Exposed=Window]
interface SVGGraphicsElement : SVGElement {
  [SameObject] readonly attribute SVGAnimatedTransformList transform;

  DOMRect getBBox(optional SVGBoundingBoxOptions options = {});
  DOMMatrix? getCTM();
  DOMMatrix? getScreenCTM();
};

SVGGraphicsElement includes SVGTests;
```

The **<a id="__svg__SVGGraphicsElement__transform"></a>transform** IDL attribute [reflects](#TermReflect) the computed value of the [transform](coords.md#TransformProperty) property and its corresponding ‘transform’ presentation attribute.

The **<a id="__svg__SVGGraphicsElement__getBBox"></a>getBBox** method is used to compute the bounding box of the current element. When the getBBox(*options*) method is called, the [bounding box algorithm](coords.md#BoundingBoxes) is invoked for the current element, with *fill*, *stroke*, *markers* and *clipped* members of the *options* dictionary argument used to control which parts of the element are included in the bounding box, using the element's user coordinate system as the coordinate system to return the bounding box in. A newly created DOMRect object that defines the computed bounding box is returned. If [getBBox](#__svg__SVGGraphicsElement__getBBox) gets called on a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to compute the geometry of the element, then throw an InvalidStateError.

The **<a id="__svg__SVGGraphicsElement__getCTM"></a>getCTM** method is used to get the matrix that transforms the current element's coordinate system to its SVG viewport's coordinate system. When getCTM() is called, the following steps are run:

1. If the current element is not in the document, then return null.
2. If the current element is a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to resolve the style of the element, then return null.
3. Let *ctm* be a matrix determined based on what the current element is:
**the current element is the [outermost svg element](struct.md#TermOutermostSVGElement)**
: *ctm* is a matrix that transforms the coordinate space of the ‘[svg](struct.md#SVGElement)’ (including its [transform](coords.md#TransformProperty) property) to the coordinate space of the document's viewport. The matrix includes the transforms produced by the ‘[viewBox](coords.md#ViewBoxAttribute)’ and ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attributes, the [transform](coords.md#TransformProperty) property, and any transform due to [currentScale](struct.md#__svg__SVGSVGElement__currentScale) and [currentTranslate](struct.md#__svg__SVGSVGElement__currentTranslate) properties on the [SVGSVGElement](struct.md#InterfaceSVGSVGElement).
**any other element**
: *ctm* is a matrix that transforms the coordinate space of the current element (including its [transform](coords.md#TransformProperty) property) to the coordinate space of its closest ancestor viewport-establishing element (also including its [transform](coords.md#TransformProperty) property).
4. Return a newly created, [detached](coords.md#MatrixMode) DOMMatrix object that represents the same matrix as *ctm*.

The **<a id="__svg__SVGGraphicsElement__getScreenCTM"></a>getScreenCTM** method is used to get the matrix that transforms the current element's coordinate system to the coordinate system of the SVG viewport for the SVG document fragment. When getScreenCTM() is called, the following steps are run:

1. If the current element is not in the document, then return null.
2. If the current element is a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to resolve the style of the element, then return null.
3. Let *ctm* be a matrix that transforms the coordinate space of the current element (including its [transform](coords.md#TransformProperty) property) to the coordinate space of the document's viewport.
This will include:
  - all of the transforms from the current element up to the [outermost svg element](struct.md#TermOutermostSVGElement)
  - any transforms due to ‘[viewBox](coords.md#ViewBoxAttribute)’, ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’, the [transform](coords.md#TransformProperty) property and any transform due to [currentScale](struct.md#__svg__SVGSVGElement__currentScale) and [currentTranslate](struct.md#__svg__SVGSVGElement__currentTranslate) properties on the [SVGSVGElement](struct.md#InterfaceSVGSVGElement) for the [outermost svg element](struct.md#TermOutermostSVGElement)
  - any transforms from the SVG viewport's coordinate space to the document's viewport, i.e. taking into account the positions of all of the CSS boxes from the [outermost svg element](struct.md#TermOutermostSVGElement)'s box to the initial containing block when the SVG document fragment is inline in an HTML document
4. Return a newly created, [detached](coords.md#MatrixMode) DOMMatrix object that represents the same matrix as *ctm*.

This method would have been more aptly named as `getClientCTM`, but the name `getScreenCTM` is kept for historical reasons.

### <a id="InterfaceSVGGeometryElement"></a>4.4.3. Interface SVGGeometryElement

Interface [SVGGeometryElement](#InterfaceSVGGeometryElement) represents SVG elements whose rendering is defined by geometry with an [equivalent path](paths.md#TermEquivalentPath), and which can be filled and stroked. This includes paths and the basic shapes.

```
[Exposed=Window]
interface SVGGeometryElement : SVGGraphicsElement {
  [SameObject] readonly attribute SVGAnimatedNumber pathLength;

  boolean isPointInFill(optional DOMPointInit point = {});
  boolean isPointInStroke(optional DOMPointInit point = {});
  float getTotalLength();
  DOMPoint getPointAtLength(float distance);
};
```

The **<a id="__svg__SVGGeometryElement__isPointInFill"></a>isPointInFill** method, when invoked, must return true if the point given by *point* passed to the method, in the coordinate space of an element, is inside the intended path as determined by the winding rule indicated by the [fill-rule](painting.md#FillRuleProperty) property of an element; and must return false otherwise. Open subpaths must be implicitly closed when computing the area inside the path, without affecting the actual subpaths. Points on the path itself must be considered to be inside the path. The returned value is independent of any visual CSS property but [fill-rule](painting.md#FillRuleProperty) If either of the *x* or *y* properties on *point* are infinite or NaN, then the method must return false. If current element is a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to compute the geometry of the element, then throw an InvalidStateError.

[isPointInFill](#__svg__SVGGeometryElement__isPointInFill) takes the winding rule indicated by the [fill-rule](painting.md#FillRuleProperty) property of an element even if the element is a child of a ‘[clipPath](../css-masking-1.md#ClipPathElement)’ element.

[isPointInFill](#__svg__SVGGeometryElement__isPointInFill) is aligned with the isPointInPath method on the CanvasDrawPath mixin as much as the SVG context allows it to be.

The **<a id="__svg__SVGGeometryElement__isPointInStroke"></a>isPointInStroke** method, when invoked, must return true if the point given by *point* passed to the method, in the coordinate space of an element, is in or on the outline path of an applied stroke on an element; and must return false otherwise. The outline path must take the stroke properties [stroke-width](painting.md#StrokeWidthProperty), [stroke-linecap](painting.md#StrokeLinecapProperty), [stroke-linejoin](painting.md#StrokeLinejoinProperty), [stroke-miterlimit](painting.md#StrokeMiterlimitProperty), [stroke-dasharray](painting.md#StrokeDasharrayProperty), [stroke-dashoffset](painting.md#StrokeDashoffsetProperty) and [vector-effect](coords.md#VectorEffectProperty) of an element into account. See sections [Computing the shape of the stroke](painting.md#StrokeShape) and [Vector effects](painting.md#PaintingVectorEffects) for details. The returned value is independent of any visual CSS property but the listed stroke properties. If either of the *x* or *y* properties on *point* are infinite or NaN, then the method must return false. If current element is a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to compute the geometry of the element, then throw an InvalidStateError.

[isPointInStroke](#__svg__SVGGeometryElement__isPointInStroke) is aligned with the isPointInStroke method on the CanvasDrawPath mixin as much as the SVG context allows it to be.

The **<a id="__svg__SVGGeometryElement__pathLength"></a>pathLength** IDL attribute [reflects](#TermReflect) the ‘[pathLength](paths.md#PathLengthAttribute)’ content attribute.

The **<a id="__svg__SVGGeometryElement__getTotalLength"></a>getTotalLength** method is used to compute the length of the path. When getTotalLength() is called, the user agent's computed value for the total length of the path, in user units, is returned. If current element is a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to compute the total length of the path, then throw an InvalidStateError.

The user agent's computed path length does not take the ‘[pathLength](paths.md#PathLengthAttribute)’ attribute into account.

The **<a id="__svg__SVGGeometryElement__getPointAtLength"></a>getPointAtLength** method is used to return the point at a given distance along the path. When getPointAtLength(*distance*) is called, the following steps are run:

1. If current element is a [non-rendered element](render.md#TermNonRenderedElement), and the UA is not able to compute the total length of the path, then throw an InvalidStateError.
2. Let *length* be the user agent's computed value for the total length of the path, in user units.
As with [getTotalLength](#__svg__SVGGeometryElement__getTotalLength), this does not take into account the ‘[pathLength](paths.md#PathLengthAttribute)’ attribute.
3. Clamp *distance* to [0, *length*].
4. Let (*x*, *y*) be the point on the path at distance *distance*.
5. Return a newly created, [detached](shapes.md#PointMode) DOMPoint object representing the point (*x*, *y*).

## <a id="DOMInterfacesForBasicDataTypes"></a>4.5. DOM interfaces for basic data types

### <a id="InterfaceSVGNumber"></a>4.5.1. Interface SVGNumber

The [SVGNumber](#InterfaceSVGNumber) interface is used primarily to represent a <number> value that is a part of an [SVGNumberList](#InterfaceSVGNumberList). Individual [SVGNumber](#InterfaceSVGNumber) objects can also be created by script.

<a id="ReadOnlyNumber"></a>An [SVGNumber](#InterfaceSVGNumber) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below. [SVGNumber](#InterfaceSVGNumber) objects reflected through the animVal IDL attribute are always *read only*.

<a id="NumberAssociatedElement"></a>An [SVGNumber](#InterfaceSVGNumber) object can be *associated* with a particular element. The associated element is used to determine which element's content attribute to update if the object [reflects](#TermReflect) an attribute. Unless otherwise described, an [SVGNumber](#InterfaceSVGNumber) object is not associated with any element.

<a id="NumberMode"></a>Every [SVGNumber](#InterfaceSVGNumber) object operates in one of two modes. It can:

1. *reflect an element of the base value* of a [reflected](#TermReflect) animatable attribute (being exposed through the methods on the [baseVal](#__svg__SVGAnimatedNumberList__baseVal) member of an [SVGAnimatedNumberList](#InterfaceSVGAnimatedNumberList)),
2. *be detached*, which is the case for [SVGNumber](#InterfaceSVGNumber) objects created with [createSVGNumber](struct.md#__svg__SVGSVGElement__createSVGNumber).

An [SVGNumber](#InterfaceSVGNumber) object maintains an internal number value, which is called its *<a id="NumberValue"></a>value*.

```
[Exposed=Window]
interface SVGNumber {
  attribute float value;
};
```

The **<a id="__svg__SVGNumber__value"></a>value** IDL attribute represents the number. On getting [value](#__svg__SVGNumber__value), the [SVGNumber](#InterfaceSVGNumber)'s [value](#NumberValue) is returned.

On setting [value](#__svg__SVGNumber__value), the following steps are run:

1. If the [SVGNumber](#InterfaceSVGNumber) is [read only](#ReadOnlyNumber), then throw a NoModificationAllowedError.
2. Set the [SVGNumber](#InterfaceSVGNumber)'s [value](#NumberValue) to the value being assigned to the [value](#__svg__SVGNumber__value) member.
3. If the [SVGNumber](#InterfaceSVGNumber) [reflects an element of the base value](#NumberMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

### <a id="InterfaceSVGLength"></a>4.5.2. Interface SVGLength

The [SVGLength](#InterfaceSVGLength) interface is used to represent a value that can be a <length>, <percentage> or <number> value.

<a id="ReadOnlyLength"></a>An [SVGLength](#InterfaceSVGLength) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below. [SVGLength](#InterfaceSVGLength) objects reflected through the animVal IDL attribute are always *read only*.

<a id="LengthAssociatedElement"></a>An [SVGLength](#InterfaceSVGLength) object can be *associated* with a particular element, as well as being designated with a *directionality*: horizontal, vertical or unspecified. The associated element and the directionality of the length are used to resolve percentage values to user units and is also used to determine which element's content attribute to update if the object [reflects](#TermReflect) an attribute. Unless otherwise described, an [SVGLength](#InterfaceSVGLength) object is not associated with any element and has unspecified directionality.

<a id="LengthMode"></a>Every [SVGLength](#InterfaceSVGLength) object operates in one of four modes. It can:

1. *reflect the base value* of a [reflected](#TermReflect) animatable attribute (being exposed through the [baseVal](#__svg__SVGAnimatedLength__baseVal) member of an [SVGAnimatedLength](#InterfaceSVGAnimatedLength)),
2. *reflect a presentation attribute value* (such as by [SVGRectElement.width.baseVal](shapes.md#__svg__SVGRectElement__width)),
3. *reflect an element of the base value* of a [reflected](#TermReflect) animatable attribute (being exposed through the methods on the [baseVal](#__svg__SVGAnimatedLengthList__baseVal) member of an [SVGAnimatedLengthList](#InterfaceSVGAnimatedLengthList)), or
4. *be detached*, which is the case for [SVGLength](#InterfaceSVGLength) objects created with [createSVGLength](struct.md#__svg__SVGSVGElement__createSVGLength).

An [SVGLength](#InterfaceSVGLength) object maintains an internal <length> or <percentage> or <number> value, which is called its *<a id="LengthValue"></a>value*.

```
[Exposed=Window]
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
           attribute float value;
           attribute float valueInSpecifiedUnits;
           attribute DOMString valueAsString;

  undefined newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits);
  undefined convertToSpecifiedUnits(unsigned short unitType);
};
```

The numeric length unit type constants defined on [SVGLength](#InterfaceSVGLength) are used to represent the type of an [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue). Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_NUMBER"></a>SVG_LENGTHTYPE_NUMBER** | A unitless <number> interpreted as a value in px. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_PERCENTAGE"></a>SVG_LENGTHTYPE_PERCENTAGE** | A <percentage>. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_EMS"></a>SVG_LENGTHTYPE_EMS** | A <length> with an em unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_EXS"></a>SVG_LENGTHTYPE_EXS** | A <length> with an ex unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_PX"></a>SVG_LENGTHTYPE_PX** | A <length> with a px unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_CM"></a>SVG_LENGTHTYPE_CM** | A <length> with a cm unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_MM"></a>SVG_LENGTHTYPE_MM** | A <length> with a mm unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_IN"></a>SVG_LENGTHTYPE_IN** | A <length> with an in unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_PT"></a>SVG_LENGTHTYPE_PT** | A <length> with a pt unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_PC"></a>SVG_LENGTHTYPE_PC** | A <length> with a pc unit. |
| **<a id="__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN"></a>SVG_LENGTHTYPE_UNKNOWN** | Some other type of value. |

The use of numeric length unit type constants is an anti-pattern and new constant values will not be introduced for any other units or length types supported by [SVGLength](#InterfaceSVGLength). If other types of lengths are supported and used, the [SVGLength](#InterfaceSVGLength) uses the [SVG_LENGTHTYPE_UNKNOWN](#__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN) unit type. See below for details on how the other properties of an [SVGLength](#InterfaceSVGLength) operate with these types of lengths.

The **<a id="__svg__SVGLength__unitType"></a>unitType** IDL attribute represents the type of value that the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) is. On getting [unitType](#__svg__SVGLength__unitType), the following steps are run:

1. If the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) is a unitless <number>, a <percentage>, or a <length> with an em, ex, px, cm, mm, in, pt or pc unit, then return the corresponding constant value from the length unit type table above.
2. Otherwise, return [SVG_LENGTHTYPE_UNKNOWN](#__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN).
For example, for a <length> with a ch unit or one that has a non-scalar value such as calc(), [SVG_LENGTHTYPE_UNKNOWN](#__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN) would be returned.

The **<a id="__svg__SVGLength__value"></a>value** IDL attribute represents the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) in user units. On getting [value](#__svg__SVGLength__value), the following steps are run:

1. Let *value* be the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue).
2. If *value* is a <number>, return that number.
3. Let *viewport size* be a basis to resolve percentages against, based on the [SVGLength](#InterfaceSVGLength)'s [associated element](#LengthAssociatedElement) and [directionality](#LengthAssociatedElement):
**has no [associated element](#LengthAssociatedElement)**
: *size* is 100
**has an [associated element](#LengthAssociatedElement) and horizontal [directionality](#LengthAssociatedElement)**
: *size* is the width of the [associated element](#LengthAssociatedElement)'s SVG viewport
**has an [associated element](#LengthAssociatedElement) and vertical [directionality](#LengthAssociatedElement)**
: *size* is the height of the [associated element](#LengthAssociatedElement)'s SVG viewport
**has an [associated element](#LengthAssociatedElement) and unspecified [directionality](#LengthAssociatedElement)**
: *size* is the length of the [associated element](#LengthAssociatedElement)'s SVG viewport diagonal (see [Units](coords.md#Units))
4. Let *font size* be a basis to resolve font size values against, based on the [SVGLength](#InterfaceSVGLength)'s [associated element](#LengthAssociatedElement):
**has no [associated element](#LengthAssociatedElement)**
: *font size* is the absolute length of the initial value of the font-size property
**has an [associated element](#LengthAssociatedElement)**
: *size* is the computed value of the [associated element](#LengthAssociatedElement)'s font-size property
5. Return the result of converting *value* to an absolute length, using *viewport size* and *font size* as percentage and font size bases. If the conversion is not possible due to the lack of an [associated element](#LengthAssociatedElement), return 0.

On setting [value](#__svg__SVGLength__value), the following steps are run:

1. If the [SVGLength](#InterfaceSVGLength) object is [read only](#ReadOnlyLength), then throw a NoModificationAllowedError.
2. Let *value* be the value being assigned to [value](#__svg__SVGLength__value).
3. Set the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) to a <number> whose value is *value*.
4. If the [SVGLength](#InterfaceSVGLength) [reflects the base value](#LengthMode) of a [reflected](#TermReflect) attribute, [reflects a presentation attribute](#LengthMode), or [reflects an element of the base value](#LengthMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGLength__valueInSpecifiedUnits"></a>valueInSpecifiedUnits** IDL attribute represents the numeric factor of the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue). On getting [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits), the following steps are run:

1. Let *value* be the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue).
2. If *value* is a <number>, return that number.
3. Otherwise, if *value* is a <percentage> or any scalar <length> value, return the numeric factor before its unit.
4. Otherwise, return 0.
Thus [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits) would return 12 for both '12%' and 12em, but 0 would be returned for non-scalar values like calc(12px + 5%).

On setting [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits), the following steps are run:

1. If the [SVGLength](#InterfaceSVGLength) object is [read only](#ReadOnlyLength), then throw a NoModificationAllowedError.
2. Let *value* be the value being assigned to [valueInSpecifiedUnits](#__svg__SVGLength__valueInSpecifiedUnits).
3. If the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) is a <number>, then update its value to *value*.
4. Otherwise, if the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) is a <percentage> or a scalar-valued <length>, then update its numeric factor to *value*.
5. Otherwise, the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) is of some other type. Set it to a <number> whose value is *value*.
6. If the [SVGLength](#InterfaceSVGLength) [reflects the base value](#LengthMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#LengthMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGLength__valueAsString"></a>valueAsString** IDL attribute represents the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) as a string. On getting [valueAsString](#__svg__SVGLength__valueAsString), the following steps are run:

1. Let *value* be the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue).
2. Let *string* be an empty string.
3. If *value* is a <number>, <percentage> or scalar <length> value, then:
  1. Let *factor* be *value*'s numeric factor, if it is a <percentage> or <length>, or *value* itself it is a <number>.
  2. Append to *string* an implementation specific string that, if parsed as a <number> using CSS syntax, would return the number value closest to *factor*, given the implementation's supported [real number precision](#Precision).
  3. If *value* is a <percentage> then append to *string* a single U+0025 PERCENT SIGN character.
  4. Otherwise, if *value* is a <length>, then append to *string* the canonical spelling of *value*'s unit.
  5. Return *string*.
4. Otherwise, return an implementation specific string that, if parsed as a <length>, would return the closest length value to *value*, given the implementation's supported [real number precision](#Precision).

On setting [valueAsString](#__svg__SVGLength__valueAsString), the following steps are run:

1. If the [SVGLength](#InterfaceSVGLength) object is [read only](#ReadOnlyLength), then throw a NoModificationAllowedError.
2. Let *value* be the value being assigned to [valueAsString](#__svg__SVGLength__valueAsString).
3. Parse *value* using the CSS syntax [ <number> | <length> | <percentage> ].
4. If parsing failed, then throw a SyntaxError.
5. Otherwise, parsing succeeded. Set [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) to the parsed value.
6. If the [SVGLength](#InterfaceSVGLength) [reflects the base value](#LengthMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#LengthMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGLength__newValueSpecifiedUnits"></a>newValueSpecifiedUnits** method is used to set the [SVGLength](#InterfaceSVGLength)'s value in a typed manner. When newValueSpecifiedUnits(unitType, valueInSpecifiedUnits) is called, the following steps are run:

1. If the [SVGLength](#InterfaceSVGLength) object is [read only](#ReadOnlyLength), then throw a NoModificationAllowedError.
2. If *unitType* is [SVG_LENGTHTYPE_UNKNOWN](#__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN) or is a value that does not appear in the length unit type table above, then throw a NotSupportedError.
3. Set [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) depending on the value of *unitType*:
**[SVG_LENGTHTYPE_NUMBER](#__svg__SVGLength__SVG_LENGTHTYPE_NUMBER)**
: a <number> whose value is *valueInSpecifiedUnits*
**[SVG_LENGTHTYPE_PERCENTAGE](#__svg__SVGLength__SVG_LENGTHTYPE_PERCENTAGE)**
: a <percentage> whose numeric factor is *valueInSpecifiedUnits*
**anything else**
: a <length> whose numeric factor is *valueInSpecifiedUnits* and whose unit is as indicated by the length unit type table above
4. If the [SVGLength](#InterfaceSVGLength) [reflects the base value](#LengthMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#LengthMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGLength__convertToSpecifiedUnits"></a>convertToSpecifiedUnits** method is used to convert the [SVGLength](#InterfaceSVGLength)'s value to a specific type. When convertToSpecifiedUnits(unitType) is called, the following steps are run:

1. If the [SVGLength](#InterfaceSVGLength) object is [read only](#ReadOnlyLength), then throw a NoModificationAllowedError.
2. If *unitType* is [SVG_LENGTHTYPE_UNKNOWN](#__svg__SVGLength__SVG_LENGTHTYPE_UNKNOWN) or is a value that does not appear in the length unit type table above, then throw a NotSupportedError.
3. Let *absolute* be the value that would be returned from the [value](#__svg__SVGLength__value) member.
4. If *unitType* is [SVG_LENGTHTYPE_NUMBER](#__svg__SVGLength__SVG_LENGTHTYPE_NUMBER), then:
  1. Set the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) to a <number> whose value is *absolute*.
5. Otherwise, if *unitType* is [SVG_LENGTHTYPE_PERCENTAGE](#__svg__SVGLength__SVG_LENGTHTYPE_PERCENTAGE), then:
  1. Let *viewport size* be a basis to resolve percentages against, based on the [SVGLength](#InterfaceSVGLength)'s [associated element](#LengthAssociatedElement) and [directionality](#LengthAssociatedElement):
  **has no [associated element](#LengthAssociatedElement)**
  : *size* is 100
  **has an [associated element](#LengthAssociatedElement) and horizontal [directionality](#LengthAssociatedElement)**
  : *size* is the width of the [associated element](#LengthAssociatedElement)'s SVG viewport
  **has an [associated element](#LengthAssociatedElement) and vertical [directionality](#LengthAssociatedElement)**
  : *size* is the height of the [associated element](#LengthAssociatedElement)'s SVG viewport
  **has an [associated element](#LengthAssociatedElement) and unspecified [directionality](#LengthAssociatedElement)**
  : *size* is the length of the [associated element](#LengthAssociatedElement)'s SVG viewport diagonal (see [Units](coords.md#Units))
  2. Set the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) to the result of converting *absolute* to a <percentage>, using *viewport size* as the percentage basis.
6. Otherwise, if *unitType* is [SVG_LENGTHTYPE_EMS](#__svg__SVGLength__SVG_LENGTHTYPE_EMS) or [SVG_LENGTHTYPE_EXS](#__svg__SVGLength__SVG_LENGTHTYPE_EXS), then:
  1. Let *font size* be a basis to resolve font size values against, based on the [SVGLength](#InterfaceSVGLength)'s [associated element](#LengthAssociatedElement):
  **has no [associated element](#LengthAssociatedElement)**
  : *font size* is the absolute length of the initial value of the font-size property
  **has an [associated element](#LengthAssociatedElement)**
  : *size* is the computed value of the [associated element](#LengthAssociatedElement)'s font-size property
  2. Set the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) to the result of converting *absolute* to a <length> with an em or ex unit (depending on *unitType*), using *font size* as the font-size basis.
7. Otherwise:
  1. Set the [SVGLength](#InterfaceSVGLength)'s [value](#LengthValue) to the result of converting *absolute* to a <length> with the unit found by looking up *unitType* in the length unit type table above.
8. If the [SVGLength](#InterfaceSVGLength) [reflects the base value](#LengthMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#LengthMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

### <a id="InterfaceSVGAngle"></a>4.5.3. Interface SVGAngle

The [SVGAngle](#InterfaceSVGAngle) interface is used to represent a value that can be an [<angle>](../css-values-4.md#angles) or <number> value.

<a id="ReadOnlyAngle"></a>An [SVGAngle](#InterfaceSVGAngle) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below. An [SVGAngle](#InterfaceSVGAngle) reflected through the animVal attribute is always *read only*.

<a id="AngleAssociatedElement"></a>An [SVGAngle](#InterfaceSVGAngle) object can be *associated* with a particular element. The associated element is used to determine which element's content attribute to update if the object [reflects](#TermReflect) an attribute. Unless otherwise described, an [SVGAngle](#InterfaceSVGAngle) object is not associated with any element.

<a id="AngleMode"></a>Every [SVGAngle](#InterfaceSVGAngle) object operates in one of two modes. It can:

1. *reflect the base value* of a [reflected](#TermReflect) animatable attribute (being exposed through the [baseVal](#__svg__SVGAnimatedAngle__baseVal) member of an [SVGAnimatedAngle](#InterfaceSVGAnimatedAngle)),
2. *be detached*, which is the case for [SVGAngle](#InterfaceSVGAngle) objects created with [createSVGAngle](struct.md#__svg__SVGSVGElement__createSVGAngle).

An [SVGAngle](#InterfaceSVGAngle) object maintains an internal [<angle>](../css-values-4.md#angles) or <number> value, which is called its *<a id="AngleValue"></a>value*.

```
[Exposed=Window]
interface SVGAngle {

  // Angle Unit Types
  const unsigned short SVG_ANGLETYPE_UNKNOWN = 0;
  const unsigned short SVG_ANGLETYPE_UNSPECIFIED = 1;
  const unsigned short SVG_ANGLETYPE_DEG = 2;
  const unsigned short SVG_ANGLETYPE_RAD = 3;
  const unsigned short SVG_ANGLETYPE_GRAD = 4;

  readonly attribute unsigned short unitType;
           attribute float value;
           attribute float valueInSpecifiedUnits;
           attribute DOMString valueAsString;

  undefined newValueSpecifiedUnits(unsigned short unitType, float valueInSpecifiedUnits);
  undefined convertToSpecifiedUnits(unsigned short unitType);
};
```

The numeric angle unit type constants defined on [SVGAngle](#InterfaceSVGAngle) are used to represent the type of an [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue). Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGAngle__SVG_ANGLETYPE_UNSPECIFIED"></a>SVG_ANGLETYPE_UNSPECIFIED** | A unitless <number> interpreted as a value in degrees. |
| **<a id="__svg__SVGAngle__SVG_ANGLETYPE_DEG"></a>SVG_ANGLETYPE_DEG** | An [<angle>](../css-values-4.md#angles) with a deg unit. |
| **<a id="__svg__SVGAngle__SVG_ANGLETYPE_RAD"></a>SVG_ANGLETYPE_RAD** | An [<angle>](../css-values-4.md#angles) with a rad unit. |
| **<a id="__svg__SVGAngle__SVG_ANGLETYPE_GRAD"></a>SVG_ANGLETYPE_GRAD** | An [<angle>](../css-values-4.md#angles) with a grad unit. |
| **<a id="__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN"></a>SVG_ANGLETYPE_UNKNOWN** | Some other type of value. |

The use of numeric angle unit type constants is an anti-pattern and new constant values will not be introduced for any other units or angle types supported by [SVGAngle](#InterfaceSVGAngle). If other types of angles are supported and used, the [SVGAngle](#InterfaceSVGAngle) uses the [SVG_ANGLETYPE_UNKNOWN](#__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN) unit type. See below for details on how the other properties of an [SVGAngle](#InterfaceSVGAngle) operate with these types of angles.

The **<a id="__svg__SVGAngle__unitType"></a>unitType** IDL attribute represents the type of value that the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) is. On getting [unitType](#__svg__SVGAngle__unitType), the following steps are run:

1. If the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) is a unitless <number> or a <length> with a deg, rad or grad unit, then return the corresponding constant value from the angle unit type table above.
2. Otherwise, return [SVG_ANGLETYPE_UNKNOWN](#__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN).
For example, for an [<angle>](../css-values-4.md#angles) with a turn unit, [SVG_ANGLETYPE_UNKNOWN](#__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN) would be returned.

The **<a id="__svg__SVGAngle__value"></a>value** IDL attribute represents the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) in degrees. On getting [value](#__svg__SVGAngle__value), the following steps are run:

1. Let *value* be the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue).
2. If *value* is a <number>, return that number.
3. Return the result of converting *value* to an angle in degrees.

On setting [value](#__svg__SVGAngle__value), the following steps are run:

1. If the [SVGAngle](#InterfaceSVGAngle) object is [read only](#ReadOnlyAngle), then throw a NoModificationAllowedError.
2. Let *value* be the value being assigned to [value](#__svg__SVGAngle__value).
3. Set the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) to a <number> whose value is *value*.
4. If the [SVGAngle](#InterfaceSVGAngle) [reflects the base value](#AngleMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGAngle__valueInSpecifiedUnits"></a>valueInSpecifiedUnits** IDL attribute represents the numeric factor of the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue). On getting [valueInSpecifiedUnits](#__svg__SVGAngle__valueInSpecifiedUnits), the following steps are run:

1. Let *value* be the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue).
2. If *value* is a <number>, return that number.
3. Otherwise, *value* is an [<angle>](../css-values-4.md#angles) value. Return the numeric factor before its unit.

On setting [valueInSpecifiedUnits](#__svg__SVGAngle__valueInSpecifiedUnits), the following steps are run:

1. If the [SVGAngle](#InterfaceSVGAngle) object is [read only](#ReadOnlyAngle), then throw a NoModificationAllowedError.
2. Let *value* be the value being assigned to [valueInSpecifiedUnits](#__svg__SVGAngle__valueInSpecifiedUnits).
3. If the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) is a <number>, then update its value to *value*.
4. Otherwise, if the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) is an [<angle>](../css-values-4.md#angles), then update its numeric factor to *value*.
5. If the [SVGAngle](#InterfaceSVGAngle) [reflects the base value](#AngleMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#AngleMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGAngle__valueAsString"></a>valueAsString** IDL attribute represents the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) as a string. On getting [valueAsString](#__svg__SVGAngle__valueAsString), the following steps are run:

1. Let *value* be the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue).
2. Let *string* be an empty string.
3. Let *factor* be *value*'s numeric factor, if it is an [<angle>](../css-values-4.md#angles), or *value* itself it is a <number>.
4. Append to *string* an implementation specific string that, if parsed as a <number> using CSS syntax, would return the number value closest to *factor*, given the implementation's supported [real number precision](#Precision).
5. If *value* is an [<angle>](../css-values-4.md#angles), then append to *string* the canonical spelling of *value*'s unit.
6. Return *string*.

On setting [valueAsString](#__svg__SVGAngle__valueAsString), the following steps are run:

1. If the [SVGAngle](#InterfaceSVGAngle) object is [read only](#ReadOnlyAngle), then throw a NoModificationAllowedError.
2. Let *value* be the value being assigned to [valueAsString](#__svg__SVGAngle__valueAsString).
3. Parse *value* using the CSS syntax [ <number> | [<angle>](../css-values-4.md#angles) ].
4. If parsing failed, then throw a SyntaxError.
5. Otherwise, parsing succeeded. Set [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) to the parsed value.
6. If the [SVGAngle](#InterfaceSVGAngle) [reflects the base value](#AngleMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#AngleMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGAngle__newValueSpecifiedUnits"></a>newValueSpecifiedUnits** method is used to set the [SVGAngle](#InterfaceSVGAngle)'s value in a typed manner. When newValueSpecifiedUnits(unitType, valueInSpecifiedUnits) is called, the following steps are run:

1. If the [SVGAngle](#InterfaceSVGAngle) object is [read only](#ReadOnlyAngle), then throw a NoModificationAllowedError.
2. If *unitType* is [SVG_ANGLETYPE_UNKNOWN](#__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN) or is a value that does not appear in the angle unit type table above, then throw a NotSupportedError.
3. Set [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) depending on the value of *unitType*:
**[SVG_ANGLETYPE_UNSPECIFIED](#__svg__SVGAngle__SVG_ANGLETYPE_UNSPECIFIED)**
: a <number> whose value is *valueInSpecifiedUnits*
**anything else**
: an [<angle>](../css-values-4.md#angles) whose numeric factor is *valueInSpecifiedUnits* and whose unit is as indicated by the angle unit type table above
4. If the [SVGAngle](#InterfaceSVGAngle) [reflects the base value](#AngleMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#AngleMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGAngle__convertToSpecifiedUnits"></a>convertToSpecifiedUnits** method is used to convert the [SVGAngle](#InterfaceSVGAngle)'s value to a specific type. When convertToSpecifiedUnits(unitType) is called, the following steps are run:

1. If the [SVGAngle](#InterfaceSVGAngle) object is [read only](#ReadOnlyAngle), then throw a NoModificationAllowedError.
2. If *unitType* is [SVG_ANGLETYPE_UNKNOWN](#__svg__SVGAngle__SVG_ANGLETYPE_UNKNOWN) or is a value that does not appear in the angle unit type table above, then throw a NotSupportedError.
3. Let *degrees* be the value that would be returned from the [value](#__svg__SVGAngle__value) member.
4. If *unitType* is [SVG_ANGLETYPE_UNSPECIFIED](#__svg__SVGAngle__SVG_ANGLETYPE_UNSPECIFIED), then:
  1. Set the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) to a <number> whose value is *degrees*.
5. Otherwise:
  1. Set the [SVGAngle](#InterfaceSVGAngle)'s [value](#AngleValue) to the result of converting *degrees* to an [<angle>](../css-values-4.md#angles) with the unit found by looking up *unitType* in the angle unit type table above.
6. If the [SVGAngle](#InterfaceSVGAngle) [reflects the base value](#AngleMode) of a [reflected](#TermReflect) attribute or [reflects an element of the base value](#AngleMode) of a [reflected](#TermReflect) attribute, then [reserialize](#TermReserialize) the reflected attribute.

### <a id="ListInterfaces"></a>4.5.4. List interfaces

Some SVG attributes contain lists of values, and to represent these values there are a number of SVG DOM *<a id="TermListInterface"></a>list interfaces*, one for each required element type – [SVGNumberList](#InterfaceSVGNumberList), [SVGLengthList](#InterfaceSVGLengthList), [SVGPointList](shapes.md#InterfaceSVGPointList), [SVGTransformList](coords.md#InterfaceSVGTransformList) and [SVGStringList](#InterfaceSVGStringList). The first four are used to represent the base and animated components of [SVGAnimatedNumberList](#InterfaceSVGAnimatedNumberList), [SVGAnimatedLengthList](#InterfaceSVGAnimatedLengthList), [SVGAnimatedPoints](shapes.md#InterfaceSVGAnimatedPoints) and [SVGTransformList](coords.md#InterfaceSVGTransformList) objects, while the fifth, [SVGStringList](#InterfaceSVGStringList), is used to [reflect](#TermReflect) a few unanimated attributes that take a list of strings.

Most [list interfaces](#TermListInterface) take the following form:

```
interface SVGNameList {

  readonly attribute unsigned long length;
  readonly attribute unsigned long numberOfItems;

  undefined clear();
  Type initialize(Type newItem);
  getter Type getItem(unsigned long index);
  Type insertItemBefore(Type newItem, unsigned long index);
  Type replaceItem(Type newItem, unsigned long index);
  Type removeItem(unsigned long index);
  Type appendItem(Type newItem);
  setter undefined (unsigned long index, Type newItem);
};
```

where *Name* is a descriptive name for the list element's ("Number", "Length", "Point", "Transform" or "String") and *Type* is the IDL type of the list's elements ([SVGNumber](#InterfaceSVGNumber), [SVGLength](#InterfaceSVGLength), DOMPoint, [SVGTransform](coords.md#InterfaceSVGTransform) or **DOMString**).

The [SVGTransformList](coords.md#InterfaceSVGTransformList) interface takes the above form but has two additional methods on it.

All [list interface](#TermListInterface) objects apart from [SVGTransformList](coords.md#InterfaceSVGTransformList) reflect the base value of a reflected content attribute. [SVGTransformList](coords.md#InterfaceSVGTransformList) objects reflect a presentation attribute (‘transform’, ‘[gradientTransform](pservers.md#LinearGradientElementGradientTransformAttribute)’ or ‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’). All [list interface](#TermListInterface) objects are associated with a particular element. Unlike [SVGLength](#InterfaceSVGLength) and similar objects, there are no "detached" [list interface](#TermListInterface) objects.

A [list interface](#TermListInterface) object maintains an internal list of elements, which is referred to in the text below simply as "the list". The IDL attributes and methods are used to inspect and manipulate elements of the list. The list can also be changed in response to changes to the reflected content attribute and to animation of the content attribute (or, for [SVGTransformList](coords.md#InterfaceSVGTransformList) objects, in response to changes to the computed value of the [transform](coords.md#TransformProperty) property).

<a id="ReadOnlyList"></a>A [list interface](#TermListInterface) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below. [list interface](#TermListInterface) objects reflected through the animVal IDL attribute are always *read only*.

<a id="ListSynchronize"></a>A [list interface](#TermListInterface) object is *<a id="TermSynchronizeList"></a>synchronized* by running the following steps:

1. Let *value* be the base value of the reflected content attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
2. Let *length* be the number of items in the list.
3. Let *new length* be the number of values in *value*. If *value* is the keyword none (as supported by the [transform](coords.md#TransformProperty) property), *new length* is 0.
4. If the list element type is [SVGNumber](#InterfaceSVGNumber), [SVGLength](#InterfaceSVGLength), DOMPoint or [SVGTransform](coords.md#InterfaceSVGTransform), then:
  1. If *length* > *new length*, then:
    1. [Detach](#TermDetach) each object in the list at an index greater than or equal to *new length*.
    2. Truncate the list to length *new length*.
    3. Set *length* to *new length*.
  2. While *length* < *new length*:
    1. Let *item* be a newly created object of the list element type.
    2. [Attach](#TermAttach) *item* to this [list interface](#TermListInterface) object.
    3. Append *item* to the list.
    4. Set *length* to *length* + 1.
  3. Let *index* be 0.
  4. While *index* < *length*:
    1. Let *item* be the object in the list at index *index*.
    2. Let *v* be the value in *value* at index *index*.
    3. Set *item*'s value to *v*.
    4. If *item* is an [SVGTransform](coords.md#InterfaceSVGTransform) object, then set the components of its [matrix object](coords.md#TransformMatrixObject) to match the new transform function value.
    5. Set *index* to *index* + 1.
5. Otherwise, the list element type is **DOMString**:
  1. Replace the list with a new list consisting of the values in *value*.

Whenever a list element object is to be *<a id="TermDetach"></a>detached*, the following steps are run, depending on the list element type:

**[SVGNumber](#InterfaceSVGNumber)**
: Set the [SVGNumber](#InterfaceSVGNumber) to no longer be [associated](#NumberAssociatedElement) with any element. If the [SVGNumber](#InterfaceSVGNumber) is [read only](#ReadOnlyNumber), set it to be no longer read only.
**[SVGLength](#InterfaceSVGLength)**
: Set the [SVGLength](#InterfaceSVGLength) to no longer be [associated](#LengthAssociatedElement) with any element. If the [SVGLength](#InterfaceSVGLength) is [read only](#ReadOnlyLength), set it to be no longer read only. Set the [SVGLength](#InterfaceSVGLength) to have unspecified [directionality](#LengthAssociatedElement).
**DOMPoint**
: Set the DOMPoint to no longer be [associated](shapes.md#PointAssociatedElement) with any element. If the DOMPoint is [read only](shapes.md#ReadOnlyPoint), set it to be no longer read only.
**[SVGTransform](coords.md#InterfaceSVGTransform)**
: Set the [SVGTransform](coords.md#InterfaceSVGTransform) to no longer be [associated](coords.md#TransformAssociatedElement) with any element. If the [SVGTransform](coords.md#InterfaceSVGTransform) is [read only](coords.md#ReadOnlyTransform), set it to be no longer read only.
****DOMString****
: Nothing is done.
Whenever a list element object is to be *<a id="TermAttach"></a>attached*, the following steps are run, depending on the list element type:

**[SVGNumber](#InterfaceSVGNumber)**
: [Associate](#NumberAssociatedElement) the [SVGNumber](#InterfaceSVGNumber) with the element that the [list interface](#TermListInterface) object is associated with. Additionally, depending on which IDL attribute the [list interface](#TermListInterface) object is reflected through:

**baseVal**
: Set the [SVGNumber](#InterfaceSVGNumber) to [reflect an element of the base value](#NumberMode).
**animVal**
: Set the [SVGNumber](#InterfaceSVGNumber) to [reflect an element of the base value](#NumberMode).
**[SVGLength](#InterfaceSVGLength)**
: [Associate](#LengthAssociatedElement) the [SVGLength](#InterfaceSVGLength) with the element that the [list interface](#TermListInterface) object is associated with and set its [directionality](#LengthAssociatedElement) to that specified by the attribute being reflected. Additionally, depending on which IDL attribute the [list interface](#TermListInterface) object is reflected through:

**baseVal**
: Set the [SVGLength](#InterfaceSVGLength) to [reflect an element of the base value](#LengthMode).
**animVal**
: Set the [SVGLength](#InterfaceSVGLength) to [reflect an element of the base value](#LengthMode). Set the [SVGLength](#InterfaceSVGLength) to be *read only*.
**DOMPoint**
: [Associate](shapes.md#PointAssociatedElement) the DOMPoint with the element that the [list interface](#TermListInterface) object is associated with. Additionally, depending on which IDL attribute the [list interface](#TermListInterface) object is reflected through:

**baseVal**
: Set the DOMPoint to [reflect an element of the base value](shapes.md#PointMode).
**animVal**
: Set the DOMPoint to [reflect an element of the base value](shapes.md#PointMode).
**[SVGTransform](coords.md#InterfaceSVGTransform)**
: [Associate](coords.md#TransformAssociatedElement) the [SVGTransform](coords.md#InterfaceSVGTransform) with the element that the [list interface](#TermListInterface) object is associated with. Set the [SVGTransform](coords.md#InterfaceSVGTransform) to [reflect an element of a presentation attribute value](coords.md#TransformMode).
****DOMString****
: Nothing is done.
The supported property indices of a [list interface](#TermListInterface) object is the set of all non-negative integers less than the length of the list.

The **<a id="__svg__SVGNameList__length"></a>length** and **<a id="__svg__SVGNameList__numberOfItems"></a>numberOfItems** IDL attributes represents the length of the list, and on getting simply return the length of the list.

The **<a id="__svg__SVGNameList__clear"></a>clear** method is used to remove all items in the list. When clear() is called, the following steps are run:

1. If the list is [read only](#ReadOnlyList), then throw a NoModificationAllowedError.
2. [Detach](#TermDetach) and then remove all elements in the list.
3. If the list [reflects](#TermReflect) an attribute, or represents the base value of an object that [reflects](#TermReflect) an attribute, then [reserialize](#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGNameList__initialize"></a>initialize** method is used to clear the list and add a single, specified value to it. When initialize(*newItem*) is called, the following steps are run:

1. If the list is [read only](#ReadOnlyList), then throw a NoModificationAllowedError.
2. [Detach](#TermDetach) and then remove all elements in the list.
3. If *newItem* is an object type, and *newItem* is not a detached object, then set *newItem* to be a newly created object of the same type as *newItem* and which has the same (number or length) value.
4. [Attach](#TermAttach) *newItem* to the [list interface](#TermListInterface) object.
5. Append *newItem* to this list.
6. If the list [reflects](#TermReflect) an attribute, or represents the base value of an object that [reflects](#TermReflect) an attribute, then [reserialize](#TermReserialize) the reflected attribute.
7. Return *newItem*.

The **<a id="__svg__SVGNameList__getItem"></a>getItem** method is used to get an item from the list at the specified position. When getItem(*index*) is called, the following steps are run:

1. If *index* is greater than or equal to the length of the list, then throw an IndexSizeError.
2. Return the element in the list at position *index*.
Note that if the list's element type is an object type, such as [SVGLength](#InterfaceSVGLength), then a reference to that object and not a copy of it is returned.

The **<a id="__svg__SVGNameList__insertItemBefore"></a>insertItemBefore** method is used to insert an element into the list at a specific position. When insertItemBefore(*newItem*, *index*) is called, the following steps are run:

1. If the list is [read only](#ReadOnlyList), then throw a NoModificationAllowedError.
2. If *newItem* is an object type, and *newItem* is not a detached object, then set *newItem* to be a newly created object of the same type as *newItem* and which has the same (number or length) value.
3. If *index* is greater than the length of the list, then set *index* to be the list length.
4. Insert *newItem* into the list at index *index*.
5. Attach *newItem* to the [list interface](#TermListInterface) object.
6. If the list [reflects](#TermReflect) an attribute, or represents the base value of an object that [reflects](#TermReflect) an attribute, then [reserialize](#TermReserialize) the reflected attribute.
7. Return *newItem*.

The **<a id="__svg__SVGNameList__replaceItem"></a>replaceItem** method is used to replace an existing item in the list with a new item. When replaceItem(*newItem*, *index*) is called, the following steps are run:

1. If the list is [read only](#ReadOnlyList), then throw a NoModificationAllowedError.
2. If *index* is greater than or equal to the length of the list, then throw an IndexSizeError.
3. If *newItem* is an object type, and *newItem* is not a detached object, then set *newItem* to be a newly created object of the same type as *newItem* and which has the same (number or length) value.
4. [Detach](#TermDetach) the element in the list at index *index*.
5. Replace the element in the list at index *index* with *newItem*.
6. [Attach](#TermAttach) *newItem* to the [list interface](#TermListInterface) object.
7. If the list [reflects](#TermReflect) an attribute, or represents the base value of an object that [reflects](#TermReflect) an attribute, then [reserialize](#TermReserialize) the reflected attribute.
8. Return *newItem*.

The **<a id="__svg__SVGNameList__removeItem"></a>removeItem** method is used to remove an item from the list. When removeItem(*index*) is called, the following steps are run:

1. If the list is [read only](#ReadOnlyList), then throw a NoModificationAllowedError.
2. If *index* is greater than or equal to the length of the list, then throw an IndexSizeError with code.
3. Let *item* be the list element at index *index*.
4. [Detach](#TermDetach) *item*.
5. Remove the list element at index *index*.
6. Return *item*.

The **<a id="__svg__SVGNameList__appendItem"></a>appendItem** method is used to append an item to the end of the list. When appendItem(*newItem*) is called, the following steps are run:

1. If the list is [read only](#ReadOnlyList), then throw a NoModificationAllowedError.
2. If *newItem* is an object type, and *newItem* is not a detached object, then set *newItem* to be a newly created object of the same type as *newItem* and which has the same (number or length) value.
3. Let *index* be the length of the list.
4. Append *newItem* to the end of the list.
5. [Attach](#TermAttach) *newItem* to the [list interface](#TermListInterface) object.
6. If the list [reflects](#TermReflect) an attribute, or represents the base value of an object that [reflects](#TermReflect) an attribute, then [reserialize](#TermReserialize) the reflected attribute.
7. Return *newItem*.

The behavior of the **<a id="__svg__SVGNameList__setter"></a>indexed property setter** is the same as that for the [replaceItem](#__svg__SVGNameList__replaceItem) method.

### <a id="InterfaceSVGNumberList"></a>4.5.5. Interface SVGNumberList

The [SVGNumberList](#InterfaceSVGNumberList) interface is a [list interface](#TermListInterface) whose elements are [SVGNumber](#InterfaceSVGNumber) objects. An [SVGNumberList](#InterfaceSVGNumberList) object represents a list of numbers.

```
[Exposed=Window]
interface SVGNumberList {

  readonly attribute unsigned long length;
  readonly attribute unsigned long numberOfItems;

  undefined clear();
  SVGNumber initialize(SVGNumber newItem);
  getter SVGNumber getItem(unsigned long index);
  SVGNumber insertItemBefore(SVGNumber newItem, unsigned long index);
  SVGNumber replaceItem(SVGNumber newItem, unsigned long index);
  SVGNumber removeItem(unsigned long index);
  SVGNumber appendItem(SVGNumber newItem);
  setter undefined (unsigned long index, SVGNumber newItem);
};
```

The behavior of all of the interface members of [SVGNumberList](#InterfaceSVGNumberList) are defined in the [List interfaces](#ListInterfaces) section above.

### <a id="InterfaceSVGLengthList"></a>4.5.6. Interface SVGLengthList

The [SVGLengthList](#InterfaceSVGLengthList) interface is a [list interface](#TermListInterface) whose elements are [SVGLength](#InterfaceSVGLength) objects. An [SVGLengthList](#InterfaceSVGLengthList) object represents a list of lengths.

```
[Exposed=Window]
interface SVGLengthList {

  readonly attribute unsigned long length;
  readonly attribute unsigned long numberOfItems;

  undefined clear();
  SVGLength initialize(SVGLength newItem);
  getter SVGLength getItem(unsigned long index);
  SVGLength insertItemBefore(SVGLength newItem, unsigned long index);
  SVGLength replaceItem(SVGLength newItem, unsigned long index);
  SVGLength removeItem(unsigned long index);
  SVGLength appendItem(SVGLength newItem);
  setter undefined (unsigned long index, SVGLength newItem);
};
```

The behavior of all of the interface members of [SVGLengthList](#InterfaceSVGLengthList) are defined in the [List interfaces](#ListInterfaces) section above.

### <a id="InterfaceSVGStringList"></a>4.5.7. Interface SVGStringList

The [SVGStringList](#InterfaceSVGStringList) interface is a [list interface](#TermListInterface) whose elements are **DOMString** values. An [SVGStringList](#InterfaceSVGStringList) object represents a list of strings.

```
[Exposed=Window]
interface SVGStringList {

  readonly attribute unsigned long length;
  readonly attribute unsigned long numberOfItems;

  undefined clear();
  DOMString initialize(DOMString newItem);
  getter DOMString getItem(unsigned long index);
  DOMString insertItemBefore(DOMString newItem, unsigned long index);
  DOMString replaceItem(DOMString newItem, unsigned long index);
  DOMString removeItem(unsigned long index);
  DOMString appendItem(DOMString newItem);
  setter undefined (unsigned long index, DOMString newItem);
};
```

The behavior of all of the interface members of [SVGStringList](#InterfaceSVGStringList) are defined in the [List interfaces](#ListInterfaces) section above.

## <a id="DOMInterfacesForReflectingSVGAttributes"></a>4.6. DOM interfaces for reflecting animatable SVG attributes

The following interfaces are used to represent the reflected value of animatable content attributes. They each consist of two component objects, representing the same data: `baseVal` and `animVal`. The `baseVal` (base value) object is modifiable, to update the corresponding attribute value.

In SVG 1.1, the `animVal` attribute of the SVG DOM interfaces represented the current animated value of the reflected attribute. In this version of SVG, `animVal` no longer represents the current animated value and is instead an alias of `baseVal`.

### <a id="InterfaceSVGAnimatedBoolean"></a>4.6.1. Interface SVGAnimatedBoolean

An [SVGAnimatedBoolean](#InterfaceSVGAnimatedBoolean) object is used to [reflect](#TermReflect) an animatable attribute that takes a boolean value.

```
[Exposed=Window]
interface SVGAnimatedBoolean {
           attribute boolean baseVal;
  readonly attribute boolean animVal;
};
```

The **<a id="__svg__SVGAnimatedBoolean__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedBoolean__animVal"></a>animVal** IDL attributes both represent the current non-animated value of the reflected attribute. On getting [baseVal](#__svg__SVGAnimatedBoolean__baseVal) or [animVal](#__svg__SVGAnimatedBoolean__animVal), the following steps are run:

1. Let *value* be the value of the reflected attribute, or the empty string if it is not present.
2. If *value* is not "true" or "false", then set *value* to the reflected attribute's [initial value](#TermInitialValue).
3. Return true if *value* is "true", and false otherwise.

On setting [baseVal](#__svg__SVGAnimatedBoolean__baseVal), the reflected attribute is set to "true" if the value is true, and "false" otherwise.

### <a id="InterfaceSVGAnimatedEnumeration"></a>4.6.2. Interface SVGAnimatedEnumeration

An [SVGAnimatedEnumeration](#InterfaceSVGAnimatedEnumeration) object is used to [reflect](#TermReflect) an animatable attribute that takes a keyword value (such as the ‘[method](text.md#TextPathElementMethodAttribute)’ attribute on ‘[textPath](text.md#TextPathElement)’) or to reflect the type of value that an animatable attribute has (done only by the [orientType](painting.md#__svg__SVGMarkerElement__orientType) IDL attribute for the ‘[marker](painting.md#MarkerElement)’ element's ‘[orient](painting.md#OrientAttribute)’ attribute).

```
[Exposed=Window]
interface SVGAnimatedEnumeration {
           attribute unsigned short baseVal;
  readonly attribute unsigned short animVal;
};
```

For [SVGAnimatedEnumeration](#InterfaceSVGAnimatedEnumeration) objects that [reflect](#TermReflect) an animatable attribute that takes only a keyword value, the **<a id="__svg__SVGAnimatedEnumeration__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedEnumeration__animVal"></a>animVal** IDL attributes represents the current non-animated value of the reflected attribute. For [orientType](painting.md#__svg__SVGMarkerElement__orientType), they represent the type of the current non-animated value of the reflected ‘[orient](painting.md#OrientAttribute)’ attribute. On getting [baseVal](#__svg__SVGAnimatedEnumeration__baseVal) or [animVal](#__svg__SVGAnimatedEnumeration__animVal), the following steps are run:

1. Let *value* be the value of the reflected attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
2. Return the *<a id="TermNumericTypeValue"></a>numeric type value* for *value*, according to the reflecting IDL attribute's definition.

On setting [baseVal](#__svg__SVGAnimatedEnumeration__baseVal), the following steps are run:

1. Let *value* be the value being assigned to [baseVal](#__svg__SVGAnimatedEnumeration__baseVal).
2. If *value* is 0 or is not the [numeric type value](#TermNumericTypeValue) for any value of the reflected attribute, then throw a TypeError.
3. Otherwise, if the reflecting IDL attribute is [orientType](painting.md#__svg__SVGMarkerElement__orientType) and *value* is [SVG_MARKER_ORIENT_ANGLE](painting.md#__svg__SVGMarkerElement__SVG_MARKER_ORIENT_ANGLE), then set the reflected attribute to the string "0".
4. Otherwise, *value* is the [numeric type value](#TermNumericTypeValue) for a specific, single keyword value for the reflected attribute. Set the reflected attribute to that value.

### <a id="InterfaceSVGAnimatedInteger"></a>4.6.3. Interface SVGAnimatedInteger

An [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) object is used to [reflect](#TermReflect) an animatable attribute that takes an integer value (such as ‘[numOctaves](../filter-effects-1.md#element-attrdef-feturbulence-numoctaves)’ on ‘[feTurbulence](../filter-effects-1.md#feTurbulenceElement)’). It is also used to reflect one part of an animatable attribute that takes an integer followed by an optional second integer (such as ‘[order](../filter-effects-1.md#element-attrdef-order)’ on ‘[feConvolveMatrix](../filter-effects-1.md#feConvolveMatrixElement)’).

This [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) interface is not used in this specification, however the [Filter Effects](../filter-effects-1.md) specification has a number of uses of it.

```
[Exposed=Window]
interface SVGAnimatedInteger {
           attribute long baseVal;
  readonly attribute long animVal;
};
```

For [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) objects that [reflect](#TermReflect) an animatable attribute that takes a single integer value, the **<a id="__svg__SVGAnimatedInteger__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedInteger__animVal"></a>animVal** IDL attributes represent the current non-animated value of the reflected attribute. For those that reflect one integer of an attribute that takes an integer followed by an optional second integer, they represent the current non-animated value of one of the two integers. On getting [baseVal](#__svg__SVGAnimatedInteger__baseVal) or [animVal](#__svg__SVGAnimatedInteger__animVal), the following steps are run:

1. Let *value* be the value of the reflected attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
2. If the reflected attribute is defined to take an integer followed by an optional second integer, then:
  1. If this [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) object reflects the first integer, then return the first value in *value*.
  2. Otherwise, this [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) object reflects the second integer. Return the second value in *value* if it has been explicitly specified, and if not, return the implicit value as described in the definition of the attribute.
  For example, the definition of ‘[order](../filter-effects-1.md#element-attrdef-order)’ says that the implicit second integer is the same as the explicit first integer.
3. Otherwise, the reflected attribute is defined to take a single integer value. Return *value*.

On setting [baseVal](#__svg__SVGAnimatedInteger__baseVal), the following steps are run:

1. Let *value* be the value being assigned to [baseVal](#__svg__SVGAnimatedInteger__baseVal).
2. Let *new* be a list of integers.
3. If the reflected attribute is defined to take an integer followed by an optional second integer, then:
  1. Let *current* be the value of the reflected attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
  2. Let *first* be the first integer in *current*.
  3. Let *second* be the second integer in *current* if it has been explicitly specified, and if not, the implicit value as described in the definition of the attribute.
  4. If this [SVGAnimatedInteger](#InterfaceSVGAnimatedInteger) object reflects the first integer, then set *first* to *value*. Otherwise, set *second* to *value*.
  5. Append *first* to *new*.
  6. Append *second* to *new*.
4. Otherwise, the reflected attribute is defined to take a single integer value. Append *value* to *new*.
5. Set the content attribute to a string consisting of each integer in *new* serialized to an implementation specific string that, if parsed as an <number> using CSS syntax, would return that integer, joined and separated by a single U+0020 SPACE character.

### <a id="InterfaceSVGAnimatedNumber"></a>4.6.4. Interface SVGAnimatedNumber

An [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) object is used to [reflect](#TermReflect) an animatable attribute that takes a number value (such as ‘[pathLength](paths.md#PathLengthAttribute)’ on ‘[path](paths.md#PathElement)’). It is also used to reflect one part of an animatable attribute that takes an number followed by an optional second number (such as ‘[kernelUnitLength](../filter-effects-1.md#element-attrdef-fediffuselighting-kernelunitlength)’ on ‘[feDiffuseLighting](../filter-effects-1.md#feDiffuseLightingElement)’).

```
[Exposed=Window]
interface SVGAnimatedNumber {
           attribute float baseVal;
  readonly attribute float animVal;
};
```

For [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) objects that [reflect](#TermReflect) an animatable attribute that takes a single number value, the **<a id="__svg__SVGAnimatedNumber__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedNumber__animVal"></a>animVal** IDL attributes represent the current non-animated value of the reflected attribute. For those that reflect one number of an attribute that takes a number followed by an optional second number, they represent the current non-animated value of one of the two numbers. On getting [baseVal](#__svg__SVGAnimatedNumber__baseVal) or [animVal](#__svg__SVGAnimatedNumber__animVal), the following steps are run:

1. Let *value* be the value of the reflected attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
2. If the reflected attribute is defined to take an number followed by an optional second number, then:
  1. If this [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) object reflects the first number, then return the first value in *value*.
  2. Otherwise, this [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) object reflects the second number. Return the second value in *value* if it has been explicitly specified, and if not, return the implicit value as described in the definition of the attribute.
  For example, the definition of ‘[kernelUnitLength](../filter-effects-1.md#element-attrdef-fediffuselighting-kernelunitlength)’ says that the implicit second number is the same as the explicit first number.
3. Otherwise, the reflected attribute is defined to take a single number value. Return *value*.

On setting [baseVal](#__svg__SVGAnimatedNumber__baseVal), the following steps are run:

1. Let *value* be the value being assigned to [baseVal](#__svg__SVGAnimatedNumber__baseVal).
2. Let *new* be a list of numbers.
3. If the reflected attribute is defined to take an number followed by an optional second number, then:
  1. Let *current* be the value of the reflected attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
  2. Let *first* be the first number in *current*.
  3. Let *second* be the second number in *current* if it has been explicitly specified, and if not, the implicit value as described in the definition of the attribute.
  4. If this [SVGAnimatedNumber](#InterfaceSVGAnimatedNumber) object reflects the first number, then set *first* to *value*. Otherwise, set *second* to *value*.
  5. Append *first* to *new*.
  6. Append *second* to *new*.
4. Otherwise, the reflected attribute is defined to take a single number value. Append *value* to *new*.
5. Set the content attribute to a string consisting of each number in *new* serialized to an implementation specific string that, if parsed as an <number> using CSS syntax, would return the value closest to the number (given the implementation's supported [Precision](#Precision)real number precision), joined and separated by a single U+0020 SPACE character.

### <a id="InterfaceSVGAnimatedLength"></a>4.6.5. Interface SVGAnimatedLength

An [SVGAnimatedLength](#InterfaceSVGAnimatedLength) object is used to [reflect](#TermReflect) either (a) an animatable attribute that takes a <length>, <percentage> or <number> value, or (b) a CSS property that takes one of these values and its corresponding presentation attribute.

```
[Exposed=Window]
interface SVGAnimatedLength {
  [SameObject] readonly attribute SVGLength baseVal;
  [SameObject] readonly attribute SVGLength animVal;
};
```

The **<a id="__svg__SVGAnimatedLength__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedLength__animVal"></a>animVal** IDL attributes represent the current value of the reflected content attribute. On getting [baseVal](#__svg__SVGAnimatedLength__baseVal) or [animVal](#__svg__SVGAnimatedLength__animVal), an [SVGLength](#InterfaceSVGLength) object is returned that:

- either [reflects the base value](#LengthMode) of the reflected attribute or [reflects the given presentation attribute](#LengthMode),
- is [associated with](#LengthAssociatedElement) the SVG element that the object with the reflecting IDL attribute of type [SVGAnimatedLength](#InterfaceSVGAnimatedLength) was obtained from, and
- has a [directionality](#LengthAssociatedElement) as defined by the specific reflected attribute.

### <a id="InterfaceSVGAnimatedAngle"></a>4.6.6. Interface SVGAnimatedAngle

An [SVGAnimatedAngle](#InterfaceSVGAnimatedAngle) object is used to [reflect](#TermReflect) the [<angle>](../css-values-4.md#angles) value of the animated ‘[orient](painting.md#OrientAttribute)’ attribute on ‘[marker](painting.md#MarkerElement)’, through the [orientAngle](painting.md#__svg__SVGMarkerElement__orientAngle) IDL attribute.

```
[Exposed=Window]
interface SVGAnimatedAngle {
  [SameObject] readonly attribute SVGAngle baseVal;
  [SameObject] readonly attribute SVGAngle animVal;
};
```

The **<a id="__svg__SVGAnimatedAngle__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedAngle__animVal"></a>animVal** IDL attributes represent the current non-animated [<angle>](../css-values-4.md#angles) value of the reflected ‘[orient](painting.md#OrientAttribute)’ attribute. On getting [baseVal](#__svg__SVGAnimatedAngle__baseVal) or [animVal](#__svg__SVGAnimatedAngle__animVal), an [SVGAngle](#InterfaceSVGAngle) object is returned that:

- [reflects the base value](#AngleMode) of the reflected ‘[orient](painting.md#OrientAttribute)’ attribute, and
- is [associated with](#AngleAssociatedElement) the SVG ‘[marker](painting.md#MarkerElement)’ element that the object with the reflecting IDL attribute of type [SVGAnimatedAngle](#InterfaceSVGAnimatedAngle) was obtained from.

### <a id="InterfaceSVGAnimatedString"></a>4.6.7. Interface SVGAnimatedString

An [SVGAnimatedString](#InterfaceSVGAnimatedString) object is used to [reflect](#TermReflect) an animatable attribute that takes a string value. It can optionally be defined to additionally reflect a second, deprecated attribute.

```
[Exposed=Window]
interface SVGAnimatedString {
           attribute (DOMString or TrustedScriptURL) baseVal;
  readonly attribute DOMString animVal;
};
```

The **<a id="__svg__SVGAnimatedString__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedString__animVal"></a>animVal** IDL attributes represent the current non-animated value of the reflected attribute. On getting [baseVal](#__svg__SVGAnimatedString__baseVal) or [animVal](#__svg__SVGAnimatedString__animVal), the following steps are run:

1. If the reflected attribute is not present, then:
  1. If the [SVGAnimatedString](#InterfaceSVGAnimatedString) object is defined to additionally reflect a second, deprecated attribute, and that attribute is present, then return its value.
  2. Otherwise, if the reflected attribute has an [initial value](#TermInitialValue), then return it.
  3. Otherwise, return the empty string.
2. Otherwise, the reflected attribute is present. Return its value.

For the [href](#__svg__SVGURIReference__href) member on the [SVGURIReference](#InterfaceSVGURIReference) interface, this will result in the deprecated ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute being returned if it is present and the ‘href’ attribute is not, and in the ‘href’ attribute being returned in all other cases.

On setting [baseVal](#__svg__SVGAnimatedString__baseVal), the following steps are run:

1. If the reflected attribute’s element is an [SVGScriptElement](interact.md#InterfaceSVGScriptElement), let *value* be the result of executing the Get Trusted Type compliant string algorithm, with TrustedScriptURL, reflected attribute’s Document's relevant global object, 'SVGScriptElement href', and 'script'.
2. Otherwise, let value be the specified value.
3. If the reflected attribute is not present, the [SVGAnimatedString](#InterfaceSVGAnimatedString) object is defined to additionally reflect a second, deprecated attribute, and that deprecated attribute is present, then set that deprecated attribute to value.
4. Otherwise, set the reflected attribute to value.

SVG does not have a complete script processing model yet. Trusted Types assumes that the attribute and text body modification protections behave similarly to ones for HTML scripts.

For the [href](#__svg__SVGURIReference__href) member on the [SVGURIReference](#InterfaceSVGURIReference) interface, this will result in the deprecated ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute being set if it is present and the ‘href’ attribute is not, and in the ‘href’ attribute being set in all other cases.

### <a id="InterfaceSVGAnimatedRect"></a>4.6.8. Interface SVGAnimatedRect

An [SVGAnimatedRect](#InterfaceSVGAnimatedRect) object is used to [reflect](#TermReflect) an animatable attribute that takes a rectangle value as specified by an *x*, *y*, *width* and *height*.

In this specification the only attribute to be reflected as an [SVGAnimatedRect](#InterfaceSVGAnimatedRect) is ‘[viewBox](coords.md#ViewBoxAttribute)’.

```
[Exposed=Window]
interface SVGAnimatedRect {
  [SameObject] readonly attribute DOMRect baseVal;
  [SameObject] readonly attribute DOMRectReadOnly animVal;
};
```

The **<a id="__svg__SVGAnimatedRect__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedRect__animVal"></a>animVal** IDL attributes represent the current non-animated rectangle value of the reflected attribute. On getting [baseVal](#__svg__SVGAnimatedRect__baseVal) or [animVal](#__svg__SVGAnimatedRect__animVal), a DOMRect object is returned.

Upon creation of the [baseVal](#__svg__SVGAnimatedRect__baseVal) or [animVal](#__svg__SVGAnimatedRect__animVal) DOMRect objects, and afterwards whenever the reflected content attribute is added, removed, or changed, the following steps are run:

1. Let *value* be the value of the reflected attribute (using the attribute's [initial value](#TermInitialValue) if it is not present or invalid).
2. Let *x*, *y*, *width* and *height* be those corresponding components of *value*.
3. Set the DOMRect object's x coordinate, y coordinate, width and height to *x*, *y*, *width* and *height*, respectively.

Whenever the x coordinate, y coordinate, width or height property of the [baseVal](#__svg__SVGAnimatedRect__baseVal) or [animVal](#__svg__SVGAnimatedRect__animVal) DOMRect object changes, except as part of the previous algorithm that reflects the value of the content attribute into the DOMRect, the reflected content attribute must be [reserialized](#TermReserialize).

### <a id="InterfaceSVGAnimatedNumberList"></a>4.6.9. Interface SVGAnimatedNumberList

An [SVGAnimatedNumberList](#InterfaceSVGAnimatedNumberList) object is used to [reflect](#TermReflect) an animatable attribute that takes a list of <number> values.

```
[Exposed=Window]
interface SVGAnimatedNumberList {
  [SameObject] readonly attribute SVGNumberList baseVal;
  [SameObject] readonly attribute SVGNumberList animVal;
};
```

The **<a id="__svg__SVGAnimatedNumberList__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedNumberList__animVal"></a>animVal** IDL attributes represent the current non-animated value of the reflected attribute. On getting [baseVal](#__svg__SVGAnimatedNumberList__baseVal) or [animVal](#__svg__SVGAnimatedNumberList__animVal), an [SVGNumberList](#InterfaceSVGNumberList) object is returned that reflects the base value of the reflected attribute.

### <a id="InterfaceSVGAnimatedLengthList"></a>4.6.10. Interface SVGAnimatedLengthList

An [SVGAnimatedLengthList](#InterfaceSVGAnimatedLengthList) object is used to [reflect](#TermReflect) an animatable attribute that takes a list of <length>, <percentage> or <number> values.

```
[Exposed=Window]
interface SVGAnimatedLengthList {
  [SameObject] readonly attribute SVGLengthList baseVal;
  [SameObject] readonly attribute SVGLengthList animVal;
};
```

The **<a id="__svg__SVGAnimatedLengthList__baseVal"></a>baseVal** or **<a id="__svg__SVGAnimatedLengthList__animVal"></a>animVal** IDL attributes represent the current non-animated value of the reflected attribute. On getting [baseVal](#__svg__SVGAnimatedLengthList__baseVal) or [animVal](#__svg__SVGAnimatedLengthList__animVal), an [SVGLengthList](#InterfaceSVGLengthList) object is returned that reflects the base value of the reflected attribute.

## <a id="OtherDOMInterfaces"></a>4.7. Other DOM interfaces

### <a id="InterfaceSVGUnitTypes"></a>4.7.1. Interface SVGUnitTypes

The [SVGUnitTypes](#InterfaceSVGUnitTypes) interface defines a commonly used set of constants used for reflecting ‘[gradientUnits](pservers.md#LinearGradientElementGradientUnitsAttribute)’, ‘[patternContentUnits](pservers.md#PatternElementPatternContentUnitsAttribute)’ and other similar attributes.

```
[Exposed=Window]
interface SVGUnitTypes {
  // Unit Types
  const unsigned short SVG_UNIT_TYPE_UNKNOWN = 0;
  const unsigned short SVG_UNIT_TYPE_USERSPACEONUSE = 1;
  const unsigned short SVG_UNIT_TYPE_OBJECTBOUNDINGBOX = 2;
};
```

The unit type constants defined on [SVGUnitTypes](#InterfaceSVGUnitTypes) have the following meanings:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGUnitTypes__SVG_UNIT_TYPE_USERSPACEONUSE"></a>SVG_UNIT_TYPE_USERSPACEONUSE** | Corresponds to the 'userSpaceOnUse' attribute value. |
| **<a id="__svg__SVGUnitTypes__SVG_UNIT_TYPE_OBJECTBOUNDINGBOX"></a>SVG_UNIT_TYPE_OBJECTBOUNDINGBOX** | Corresponds to the 'objectBoundingBox' attribute value. |
| **<a id="__svg__SVGUnitTypes__SVG_UNIT_TYPE_UNKNOWN"></a>SVG_UNIT_TYPE_UNKNOWN** | Some other type of value. |

### <a id="InterfaceSVGTests"></a>4.7.2. Mixin SVGTests

The [SVGTests](#InterfaceSVGTests) interface is used to reflect [conditional processing attributes](struct.md#TermConditionalProcessingAttribute), and is mixed in to other interfaces for elements that support these attributes.

```
interface mixin SVGTests {
  [SameObject] readonly attribute SVGStringList requiredExtensions;
  [SameObject] readonly attribute SVGStringList systemLanguage;
};
```

The **<a id="__svg__SVGTests__requiredExtensions"></a>requiredExtensions** IDL attribute [reflects](#TermReflect) the ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’ content attribute.

The **<a id="__svg__SVGTests__systemLanguage"></a>systemLanguage** IDL attribute [reflects](#TermReflect) the ‘[systemLanguage](struct.md#SystemLanguageAttribute)’ content attribute.

### <a id="InterfaceSVGFitToViewBox"></a>4.7.3. Mixin SVGFitToViewBox

The [SVGFitToViewBox](#InterfaceSVGFitToViewBox) interface is used to reflect the ‘[viewBox](coords.md#ViewBoxAttribute)’ and ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attributes, and is mixed in to other interfaces for elements that support these two attributes.

```
interface mixin SVGFitToViewBox {
  [SameObject] readonly attribute SVGAnimatedRect viewBox;
  [SameObject] readonly attribute SVGAnimatedPreserveAspectRatio preserveAspectRatio;
};
```

The **<a id="__svg__SVGFitToViewBox__viewBox"></a>viewBox** IDL attribute [reflects](#TermReflect) the ‘[viewBox](coords.md#ViewBoxAttribute)’ content attribute.

The **<a id="__svg__SVGFitToViewBox__preserveAspectRatio"></a>preserveAspectRatio** IDL attribute [reflects](#TermReflect) the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ content attribute.

### <a id="InterfaceSVGURIReference"></a>4.7.4. Mixin SVGURIReference

The [SVGURIReference](#InterfaceSVGURIReference) interface is used to reflect the ‘href’ attribute and the deprecated ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute.

```
interface mixin SVGURIReference {
  [SameObject] readonly attribute SVGAnimatedString href;
};
```

The **<a id="__svg__SVGURIReference__href"></a>href** IDL attribute represents the value of the ‘href’ attribute, and, on elements that are defined to support it, the deprecated ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute. On getting [href](#__svg__SVGURIReference__href), an [SVGAnimatedString](#InterfaceSVGAnimatedString) object is returned that:

- reflects the ‘href’ attribute, and
- if the element is defined to support the deprecated ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute, additionally reflects that deprecated attribute.

The [SVGAnimatedString](#InterfaceSVGAnimatedString) interface is defined to reflect, through its [baseVal](#__svg__SVGAnimatedString__baseVal) and [animVal](#__svg__SVGAnimatedString__animVal) members, the deprecated ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute, if that attribute is present and the ‘href’ is not, and to reflect the ‘href’ attribute in all other circumstances. Animation elements treat attributeName='xlink:href' as being an alias for targeting the ‘href’ attribute.
