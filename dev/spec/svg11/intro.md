<!-- https://www.w3.org/TR/SVG11/intro.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](index.md) ⋅ [Next](concepts.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 1 Introduction

## <a id="AboutSVG"></a>1.1 About SVG

This specification defines the features and syntax for Scalable Vector Graphics (SVG).

SVG is a language for describing two-dimensional graphics in XML [XML10]. SVG allows for three types of graphic objects: vector graphic shapes (e.g., paths consisting of straight lines and curves), images and text. Graphical objects can be grouped, styled, transformed and composited into previously rendered objects. The feature set includes nested transformations, clipping paths, alpha masks, filter effects and template objects.

SVG drawings can be [interactive](interact.md) and [dynamic](animate.md). [Animations](animate.md) can be defined and triggered either declaratively (i.e., by embedding SVG animation elements in SVG content) or via scripting.

Sophisticated applications of SVG are possible by use of a supplemental scripting language which accesses [SVG Document Object Model (DOM)](https://www.w3.org/TR/SVG11/svgdom.html), which provides complete access to all elements, attributes and properties. A rich set of [event handlers](interact.md#SVGEvents) such as [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute) and [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute) can be assigned to any SVG graphical object. Because of its [compatibility and leveraging of other Web standards](#W3CCompatibility), features like [scripting](https://www.w3.org/TR/SVG11/script.html) can be done on XHTML and SVG elements simultaneously within the same Web page.

SVG is a language for rich graphical content. For accessibility reasons, if there is an original source document containing higher-level structure and semantics, it is recommended that the higher-level information be made available somehow, either by making the original source document available, or making an alternative version available in an alternative format which conveys the higher-level information, or by using SVG's facilities to include the higher-level information within the SVG content. For suggested techniques in achieving greater accessibility, see [Accessibility](access.md).

SVG 1.1 is a modularization of SVG 1.0 [[SVG10](refs.md#ref-SVG10)]. See the [Document Type Definition](https://www.w3.org/TR/SVG11/svgdtd.html) appendix for details on how the DTD is structured to allow profiling and composition with other XML languages.

## <a id="MIMEType"></a>1.2 SVG MIME type, file name extension and Macintosh file type

The MIME type for SVG is "`image/svg+xml`" (see XML Media Types [[RFC3023](refs.md#ref-RFC3023)]). The registration of this MIME type is in progress at the W3C.

It is recommended that SVG files have the extension `".svg"` (all lowercase) on all platforms. It is recommended that gzip-compressed [[RFC1952](refs.md#ref-RFC1952)] SVG files have the extension `".svgz"` (all lowercase) on all platforms.

It is recommended that SVG files stored on Macintosh HFS file systems be given a file type of `"svg "` (all lowercase, with a space character as the fourth letter). It is recommended that gzip-compressed SVG files stored on Macintosh HFS file systems be given a file type of `"svgz"` (all lowercase).

## <a id="NamespaceAndDTDIdentifiers"></a>1.3 SVG Namespace, Public Identifier and System Identifier

The following are the SVG 1.1 namespace, public identifier and system identifier:

**<a id="Namespace"></a>SVG Namespace:**
: http://www.w3.org/2000/svg
**<a id="PublicID"></a>Public Identifier for SVG 1.1:**
: PUBLIC "-//W3C//DTD SVG 1.1//EN"
**<a id="SystemID"></a>System Identifier for the SVG 1.1 Recommendation:**
: http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd
The following is an example document type declaration for an SVG document:

```
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
         "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
```

Note that DTD listed in the System Identifier is a modularized DTD (i.e. its contents are spread over multiple files), which means that a validator may have to fetch the multiple modules in order to validate. For that reason, there is a single flattened DTD available that corresponds to the SVG 1.1 modularized DTD. It can be found at http://www.w3.org/Graphics/SVG/1.1/DTD/svg11-flat.dtd.

While a DTD is provided in this specification, the use of DTDs for validating XML documents is known to be problematic. In particular, DTDs do not handle namespaces gracefully. It is *not* recommended that a DOCTYPE declaration be included in SVG documents.

## <a id="W3CCompatibility"></a>1.4 Compatibility with Other Standards Efforts

SVG leverages and integrates with other W3C specifications and standards efforts. By leveraging and conforming to other standards, SVG becomes more powerful and makes it easier for users to learn how to incorporate SVG into their Web sites.

The following describes some of the ways in which SVG maintains compatibility with, leverages and integrates with other W3C efforts:

- SVG is an application of XML and is compatible with the Extensible Markup Language (XML) 1.0 Recommendation [[XML10](refs.md#ref-XML10)]
- SVG is compatible with the Namespaces in XML Recommendation [[XML-NS](refs.md#ref-XML-NS)]
- SVG utilizes XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)] for IRI referencing and requires support for base IRI specifications defined in XML Base [[XML-BASE](refs.md#ref-XML-BASE)].
- SVG content can be styled by either CSS (see Cascading Style Sheets (CSS) level 2 [[CSS2](refs.md#ref-CSS2)]) or XSLT (see XSL Transformations (XSLT) Version 1.0 [[XSLT](refs.md#ref-XSLT)] and XSL Transformations (XSLT) Version 2.0 [[XSLT2](refs.md#ref-XSLT2)]). See [Styling with CSS](styling.md#StylingWithCSS) and [Styling with XSL](styling.md#StylingWithXSL) for details.
- SVG supports relevant properties and approaches common to CSS and XSL, plus selected semantics and features of CSS (see [SVG's styling properties](styling.md#SVGStylingProperties) and [SVG's Use of Cascading Style Sheets](styling.md#SVGUseOfCSS)).
- External style sheets are referenced using the mechanism documented in Associating Style Sheets with XML documents Version 1.0 [[XML-SS](refs.md#ref-XML-SS)].
- SVG includes a complete Document Object Model (DOM) and conforms to the Document Object Model (DOM) Level 1 Recommendation [[DOM1](refs.md#ref-DOM1)]. The SVG DOM has a high level of compatibility and consistency with the HTML DOM that is defined in the DOM Level 1 specification. Additionally, the SVG DOM supports and incorporates many of the facilities described in DOM Level 2, including the CSS object model and event handling [[DOM2](refs.md#ref-DOM2)] [[DOM2STYLE](refs.md#ref-DOM2STYLE)] [[DOM2EVENTS](refs.md#ref-DOM2EVENTS)].
- SVG incorporates some features and approaches that are part of the Synchronized Multimedia Integration Language (SMIL) 3.0 Specification [[SMIL](refs.md#ref-SMIL)], including the [‘switch’](struct.md#SwitchElement) element and the [‘systemLanguage’](struct.md#SystemLanguageAttribute) attribute.
- SVG's animation features (see [Animation](animate.md)) were developed in collaboration with the W3C Synchronized Multimedia (SYMM) Working Group, developers of the Synchronized Multimedia Integration Language (SMIL) 3.0 Specification [[SMIL](refs.md#ref-SMIL)]. SVG's animation features incorporate and extend the general-purpose XML animation capabilities described in the SMIL Animation specification [[SMILANIM](refs.md#ref-SMILANIM)].
- SVG has been designed to allow SMIL to use animated or static SVG content as media components.
- SVG attempts to achieve maximum compatibility with both HTML 4 [[HTML4](refs.md#ref-HTML4)] and XHTML™ 1.0 [[XHTML](refs.md#ref-XHTML)]. Many of SVG's facilities are modeled directly after HTML, including its use of CSS [CSS2], its approach to event handling, and its approach to its Document Object Model [[DOM2](refs.md#ref-DOM2)].
- SVG is compatible with W3C work on internationalization. References (W3C and otherwise) include: [[UNICODE](refs.md#ref-UNICODE)] and [[CHARMOD](refs.md#ref-CHARMOD)]. Also, see [Internationalization Support](i18n.md).
- SVG is compatible with W3C work on Web Accessibility. Also, see [Accessibility Support](access.md).

In environments which support DOM 2 Core [[DOM2](refs.md#ref-DOM2)] for other XML grammars (e.g., XHTML [[XHTML](refs.md#ref-XHTML)]) and which also support SVG and the SVG DOM, a single scripting approach can be used simultaneously for both XML documents and SVG graphics, in which case interactive and dynamic effects will be possible on multiple XML namespaces using the same set of scripts.

## <a id="Terminology"></a>1.5 Terminology

Within this specification, the key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" are to be interpreted as described in Key words for use in RFCs to Indicate Requirement Levels [[RFC2119](refs.md#ref-RFC2119)]. However, for readability, these words do not appear in all uppercase letters in this specification.

At times, this specification recommends good practice for authors and user agents. These recommendations are not normative and conformance with this specification does not depend on their realization. These recommendations contain the expression "We recommend ...", "This specification recommends ...", or some similar wording.

## <a id="Definitions"></a>1.6 Definitions

**<a id="TermAnimationElement"></a>animation element**
: An animation element is an element that can be used to animate the attribute or property value of another element. The following elements are animation elements: [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘animate’](animate.md#AnimateElement) and [‘set’](animate.md#SetElement).
**<a id="TermAnimationEventAttribute"></a>animation event attribute**
: An animation event attribute is an [event attribute](#TermEventAttribute) that specifies script to run for a particular animation-related event. See [Animation event attributes](https://www.w3.org/TR/SVG11/script.html#AnimationEvents). The animation event attributes are [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute) and [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute).
**<a id="TermBasicShape"></a><a id="TermBasicShapeElement"></a>basic shape**
: Standard shapes which are predefined in SVG as a convenience for common graphical operations. Specifically: [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement) and [‘rect’](shapes.md#RectElement).
**<a id="TermCanvas"></a>canvas**
: A surface onto which graphics elements are drawn, which can be real physical media such as a display or paper or an abstract surface such as a allocated region of computer memory. See the discussion of the [SVG canvas](coords.md#SVGCanvas) in the chapter on [Coordinate Systems, Transformations and Units](coords.md).
**<a id="TermClippingPath"></a>clipping path**
: A combination of [‘path’](paths.md#PathElement), [‘text’](text.md#TextElement) and [basic shapes](#TermBasicShape) which serve as the outline of a (in the absence of anti-aliasing) 1-bit mask, where everything on the "inside" of the outline is allowed to show through but everything on the outside is masked out. See [Clipping paths](masking.md#ClippingPaths).
**<a id="TermContainerElement"></a>container element**
: An element which can have graphics elements and other container elements as child elements. Specifically: [‘a’](linking.md#AElement), [‘defs’](struct.md#DefsElement), [‘glyph’](fonts.md#GlyphElement), [‘g’](struct.md#GElement), [‘marker’](painting.md#MarkerElement), [‘mask’](masking.md#MaskElement), [‘missing-glyph’](fonts.md#MissingGlyphElement), [‘pattern’](pservers.md#PatternElement), [‘svg’](struct.md#SVGElement), [‘switch’](struct.md#SwitchElement) and [‘symbol’](struct.md#SymbolElement).
**<a id="TermConditionalProcessingAttribute"></a>conditional processing attribute**
: A conditional processing attribute is one that controls whether or not the element on which it appears is processed. Most elements, but not all, may have conditional processing attributes specified on them. See [Conditional processing](struct.md#ConditionalProcessing) for details. The conditional processing attributes defined in SVG 1.1 are [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute) and [‘systemLanguage’](struct.md#SystemLanguageAttribute).
**<a id="TermCoreAttributes"></a>core attributes**
: The core attributes are those attributes that can be specified on any SVG element. See [Common attributes](struct.md#CommonAttributes). The core attributes are [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute) and [‘xml:space’](struct.md#XMLSpaceAttribute).
**<a id="TermCurrentInnermostSVGDocumentFragment"></a> current innermost SVG document fragment**
: The XML document sub-tree which starts with the most immediate ancestor [‘svg’](struct.md#SVGElement) element of a given SVG element.
**<a id="TermCurrentSVGDocumentFragment"></a>current SVG document fragment**
: The XML document sub-tree which starts with the outermost ancestor [‘svg’](struct.md#SVGElement) element of a given SVG element, with the requirement that all container elements between the outermost [‘svg’](struct.md#SVGElement) and this element are all elements in the SVG language.
**<a id="TermCurrentTransformationMatrix"></a>current transformation matrix (<a id="TermCTM"></a>CTM)**
: Transformation matrices define the mathematical mapping from one coordinate system into another using a 3x3 matrix using the equation [x' y' 1] = [x y 1] * matrix. The *current transformation matrix* (CTM) defines the mapping from the user coordinate system into the viewport coordinate system. See [Coordinate system transformations](coords.md#EstablishingANewUserSpace).
**<a id="TermDescriptiveElement"></a>descriptive element**
: An element which provides supplementary descriptive information about its parent. Specifically, the following elements are descriptive elements: [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement) and [‘title’](struct.md#TitleElement).
**<a id="TermDocumentEventAttribute"></a>document event attribute**
: A document event attribute is an [event attribute](#TermEventAttribute) that specifies script to run for a particular document-wide event. See [Document-level event attributes](https://www.w3.org/TR/SVG11/script.html#DocumentEvents). The document event attributes are [‘onabort’](https://www.w3.org/TR/SVG11/script.html#OnAbortEventAttribute), [‘onerror’](https://www.w3.org/TR/SVG11/script.html#OnErrorEventAttribute), [‘onresize’](https://www.w3.org/TR/SVG11/script.html#OnResizeEventAttribute), [‘onscroll’](https://www.w3.org/TR/SVG11/script.html#OnScrollEventAttribute), [‘onunload’](https://www.w3.org/TR/SVG11/script.html#OnUnloadEventAttribute) and [‘onzoom’](https://www.w3.org/TR/SVG11/script.html#OnZoomEventAttribute).
**<a id="TermEventAttribute"></a>event attribute**
: An event attribute is one that specifies some script to run when an event of a certain type is dispatched to the element on which the attribute is specified. See [Event attributes](https://www.w3.org/TR/SVG11/script.html#EventAttributes).
**<a id="TermFill"></a>fill**
: The operation of [painting](#TermPaint) the interior of a [shape](#TermShape) or the interior of the character glyphs in a text string.
**<a id="TermFilterPrimitiveAttributes"></a>filter primitive attributes**
: The filter primitive attributes is the set of attributes that are common to all [filter primitive elements](#TermFilterPrimitiveElement). They are [‘height’](filters.md#FilterPrimitiveHeightAttribute), [‘result’](filters.md#FilterPrimitiveResultAttribute), [‘width’](filters.md#FilterPrimitiveWidthAttribute), [‘x’](filters.md#FilterPrimitiveXAttribute) and [‘y’](filters.md#FilterPrimitiveYAttribute).
**<a id="TermFilterPrimitiveElement"></a>filter primitive element**
: A filter primitive element is one that can be used as a child of a [‘filter’](filters.md#FilterElement) element to specify a node in the filter graph. The following elements are the filter primitive elements defined in SVG 1.1: [‘feBlend’](filters.md#feBlendElement), [‘feColorMatrix’](filters.md#feColorMatrixElement), [‘feComponentTransfer’](filters.md#feComponentTransferElement), [‘feComposite’](filters.md#feCompositeElement), [‘feConvolveMatrix’](filters.md#feConvolveMatrixElement), [‘feDiffuseLighting’](filters.md#feDiffuseLightingElement), [‘feDisplacementMap’](filters.md#feDisplacementMapElement), [‘feFlood’](filters.md#feFloodElement), [‘feGaussianBlur’](filters.md#feGaussianBlurElement), [‘feImage’](filters.md#feImageElement), [‘feMerge’](filters.md#feMergeElement), [‘feMorphology’](filters.md#feMorphologyElement), [‘feOffset’](filters.md#feOffsetElement), [‘feSpecularLighting’](filters.md#feSpecularLightingElement), [‘feTile’](filters.md#feTileElement) and [‘feTurbulence’](filters.md#feTurbulenceElement).
**<a id="TermFont"></a>font**
: A font represents an organized collection of [glyphs](#TermGlyph) in which the various glyph representations will share a common look or styling such that, when a string of characters is rendered together, the result is highly legible, conveys a particular artistic style and provides consistent inter-character alignment and spacing.
**<a id="TermGlyph"></a>glyph**
: A glyph represents a unit of rendered content within a [font](#TermFont). Often, there is a one-to-one correspondence between characters to be drawn and corresponding glyphs (e.g., often, the character "A" is rendered using a single glyph), but other times multiple glyphs are used to render a single character (e.g., use of accents) or a single glyph can be used to render multiple characters (e.g., ligatures). Typically, a glyph is defined by one or more [shapes](#TermShape) such as a [path](paths.md), possibly with additional information such as rendering hints that help a font engine to produce legible text in small sizes.
**<a id="TermGradientElement"></a>gradient element**
: A gradient element is one that defines a gradient paint server. SVG 1.1 defines the following gradient elements: [‘linearGradient’](pservers.md#LinearGradientElement) and [‘radialGradient’](pservers.md#RadialGradientElement).
**<a id="TermGraphicalEventAttribute"></a>graphical event attribute**
: A graphical event attribute is an [event attribute](#TermEventAttribute) that specifies script to run for a particular user interaction event. See [Event attributes on graphics and container elements](https://www.w3.org/TR/SVG11/script.html#GraphicsEvents). The graphical event attributes are [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute) and [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute).
**<a id="TermGraphicsElement"></a>graphics element**
: One of the element types that can cause graphics to be drawn onto the target canvas. Specifically: [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘image’](struct.md#ImageElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement), [‘text’](text.md#TextElement) and [‘use’](struct.md#UseElement).
**<a id="TermGraphicsReferencingElement"></a>graphics referencing element**
: A graphics element which uses a reference to a different document or element as the source of its graphical content. Specifically: [‘image’](struct.md#ImageElement) and [‘use’](struct.md#UseElement).
**<a id="TermHitTesting"></a>hit-testing**
: The process of determining whether a pointer intersects a given [graphics element](#TermGraphicsElement). Hit-testing is used in determining which element to dispatch a mouse event to, which might be done in response to the user moving the pointing device, or by changes in the position, shape and other attributes of elements in the document. Hit-testing is also known as *hit detection* or *picking*. See [hit-testing and processing order for user interface events](interact.md#pointer-processing) and the definition of the [‘pointer-events’](interact.md#PointerEventsProperty) property.
**<a id="TermIRIReference"></a>IRI reference**
: An IRI reference is an Internationalized Resource Identifier with an optional fragment identifier, as defined in Internationalized Resource Identifiers [[RFC3987](refs.md#ref-RFC3987)]. An IRI reference serves as a reference to a resource or (with a fragment identifier) to a secondary resource. See [References and the ‘defs’ element](struct.md#Head).
**<a id="TermLightSourceElement"></a>light source element**
: A light source element is one that can specify light source information for an [‘feDiffuseLighting’](filters.md#feDiffuseLightingElement) or [‘feSpecularLighting’](filters.md#feSpecularLightingElement) element. The following light source elements are defined in SVG 1.1: [‘feDistantLight’](filters.md#feDistantLightElement), [‘fePointLight’](filters.md#fePointLightElement) and [‘feSpotLight’](filters.md#feSpotLightElement).
**<a id="TermLocalIRIReference"></a>local IRI reference**
: An Internationalized Resource Identifier [[RFC3987](refs.md#ref-RFC3987)] that does not include an <absoluteIRI> or <relativeIRI> and thus represents a reference to an element within the current document. See [References and the ‘defs’ element](struct.md#Head).
**<a id="TermMask"></a>mask**
: A [container element](#TermContainerElement) which can contain [graphics elements](#TermGraphicsElement) or other container elements which define a set of graphics that is to be used as a semi-transparent mask for compositing foreground objects into the current background. See [Masks](masking.md#Masking).
**<a id="TermNonLocalIRIReference"></a>non-local IRI reference**
: An Internationalized Resource Identifier [[RFC3987](refs.md#ref-RFC3987)] that includes an <absoluteIRI> or <relativeIRI> and thus (usually) represents a reference to a different document or an element within a different document. See [References and the ‘defs’ element](struct.md#Head).
**<a id="TermOutermostSVGElement"></a> outermost svg element **
: The furthest [‘svg’](struct.md#SVGElement) ancestor element that remains in the [current SVG document fragment](#TermCurrentSVGDocumentFragment).
**<a id="TermPaint"></a>paint**
: A paint represents a way of putting color values onto the canvas. A paint might consist of both color values and associated alpha values which control the blending of colors against already existing color values on the canvas. SVG supports three types of built-in paint: [color](color.md#ColorIntroduction), [gradients](pservers.md#Gradients) and [patterns](pservers.md#Patterns).
**<a id="TermPresentationAttribute"></a>presentation attribute**
: An XML attribute on an SVG element which specifies a value for a given [property](#TermProperty) for that element. See [Styling](styling.md). Note that although any property may be *specified* on any element, not all properties will *apply to* (affect the rendering of) a given element. The definition of each property states to what set of elements it applies.
**<a id="TermProperty"></a>property**
: A parameter that helps specify how a document should be rendered. A complete list of SVG's properties can be found in [Property Index](propidx.md). Properties are assigned to elements in the SVG language either by [presentation attributes](#TermPresentationAttribute) on elements in the SVG language or by using a styling language such as CSS [CSS2]. See [Styling](styling.md).
**<a id="TermRootmostSVGElement"></a>rootmost ‘svg’ element**
:

The rootmost [‘svg’](struct.md#SVGElement) element is the furthest [‘svg’](struct.md#SVGElement) ancestor element that does not exit an [SVG context](#TermSVGContext). See also [SVG document fragment](#TermSVGDocumentFragment).

**<a id="TermShape"></a><a id="TermShapeElement"></a>shape**
: A graphics element that is defined by some combination of straight lines and curves. Specifically: [‘path’](paths.md#PathElement), [‘rect’](shapes.md#RectElement), [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) and [‘polygon’](shapes.md#PolygonElement).
**<a id="TermStroke"></a>stroke**
: The operation of [painting](#TermPaint) the outline of a [shape](#TermShape) or the outline of character glyphs in a text string.
**<a id="TermStructuralElement"></a>structural element**
: The structural elements are those which define the primary structure of an SVG document. Specifically, the following elements are structural elements: [‘defs’](struct.md#DefsElement), [‘g’](struct.md#GElement), [‘svg’](struct.md#SVGElement), [‘symbol’](struct.md#SymbolElement) and [‘use’](struct.md#UseElement).
**<a id="TermSVGCanvas"></a>SVG canvas**
: The [canvas](#TermCanvas) onto which the SVG content is rendered. See the discussion of the [SVG canvas](coords.md#SVGCanvas) in the chapter on [Coordinate Systems, Transformations and Units](coords.md).
**<a id="TermSVGContext"></a>SVG context**
:

An SVG context is a document fragment where all elements within the fragment must be subject to processing by an [SVG user agent](#TermSVGUserAgent) according to the rules in this specification.

If SVG content is embedded inline within parent XML (such as XHTML), the SVG context does not include the ancestors above the [rootmost ‘svg’ element](#TermRootmostSVGElement). If the SVG content contains any [‘foreignObject’](extend.md#ForeignObjectElement) elements which in turn contain non-SVG content, the SVG context does not include the contents of the [‘foreignObject’](extend.md#ForeignObjectElement) elements.

**<a id="TermSVGDocumentFragment"></a> SVG document fragment**
: The XML document sub-tree which starts with an [‘svg’](struct.md#SVGElement) element. An SVG document fragment can consist of a stand-alone SVG document, or a fragment of a parent XML document enclosed by an [‘svg’](struct.md#SVGElement) element. When an [‘svg’](struct.md#SVGElement) element is a descendant of another [‘svg’](struct.md#SVGElement) element, there are two SVG document fragments, one for each [‘svg’](struct.md#SVGElement) element. (One SVG document fragment is contained within another SVG document fragment.)
**<a id="TermSVGUserAgent"></a>SVG user agent**
: An SVG user agent is a [user agent](#TermUserAgent) that is able to retrieve and render SVG content.
**<a id="TermSVGViewport"></a>SVG viewport**
: The [viewport](#TermViewport) within the [SVG canvas](#TermSVGCanvas) which defines the rectangular region into which SVG content is rendered. See the discussion of the [SVG viewport](coords.md#SVGViewport) in the chapter on [Coordinate Systems, Transformations and Units](coords.md).
**<a id="TermTextContentElement"></a>text content element**
: A text content element is an SVG element that causes a text string to be rendered onto the canvas. The SVG 1.1 text content elements are the following: [‘altGlyph’](text.md#AltGlyphElement), [‘textPath’](text.md#TextPathElement), [‘text’](text.md#TextElement), [‘tref’](text.md#TRefElement) and [‘tspan’](text.md#TSpanElement)
**<a id="TermTextContentChildElement"></a>text content child element**
: A text content child element is a [text content element](#TermTextContentElement) that is allowed as a descendant of another [text content element](#TermTextContentElement). In SVG 1.1, the text content child elements are the following: [‘altGlyph’](text.md#AltGlyphElement), [‘textPath’](text.md#TextPathElement), [‘tref’](text.md#TRefElement) and [‘tspan’](text.md#TSpanElement)
**<a id="TermTextContentBlockElement"></a>text content block element**
: A text content block element is a [text content element](#TermTextContentElement) that serves as a standalone element for a unit of text, and which may optionally contain certain child [text content elements](#TermTextContentElement) (e.g. [‘tspan’](text.md#TSpanElement)). .
**<a id="TermTransformation"></a>transformation**
: A modification of the [current transformation matrix (CTM)](#TermCurrentTransformationMatrix) by providing a supplemental transformation in the form of a set of simple transformations specifications (such as scaling, rotation or translation) and/or one or more [transformation matrices](#TermTransformationMatrix). See [Coordinate system transformations](coords.md#EstablishingANewUserSpace).
**<a id="TermTransformationMatrix"></a>transformation matrix**
: Transformation matrices define the mathematical mapping from one coordinate system into another using a 3x3 matrix using the equation [x' y' 1] = [x y 1] * matrix. See [current transformation matrix (CTM)](#TermCurrentTransformationMatrix) and [Coordinate system transformations](coords.md#EstablishingANewUserSpace).
**<a id="TermUserAgent"></a>user agent**
:

The general definition of a user agent is an application that retrieves and renders Web content, including text, graphics, sounds, video, images, and other content types. A user agent may require additional user agents that handle some types of content. For instance, a browser may run a separate program or plug-in to render sound or video. User agents include graphical desktop browsers, multimedia players, text browsers, voice browsers, and assistive technologies such as screen readers, screen magnifiers, speech synthesizers, onscreen keyboards, and voice input software.

A "user agent" may or may not have the ability to retrieve and render SVG content; however, an "SVG user agent" retrieves and renders SVG content.

**<a id="TermUserCoordinateSystem"></a>user coordinate system**
: In general, a coordinate system defines locations and distances on the current [canvas](#TermCanvas). The current user coordinate system is the coordinate system that is currently active and which is used to define how coordinates and lengths are located and computed, respectively, on the current [canvas](#TermCanvas). See [initial user coordinate system](coords.md#SVGInitialUserCoordinateSystem) and [Coordinate system transformations](coords.md#EstablishingANewUserSpace).
**<a id="TermUserSpace"></a>user space**
: A synonym for [user coordinate system](#TermUserCoordinateSystem).
**<a id="TermUserUnits"></a>user units**
: A coordinate value or length expressed in user units represents a coordinate value or length in the current [user coordinate system](#TermUserCoordinateSystem). Thus, 10 user units represents a length of 10 units in the current user coordinate system.
**<a id="TermViewport"></a>viewport**
: A rectangular region within the current [canvas](#TermCanvas) onto which [graphics elements](#TermGraphicsElement) are to be rendered. See the discussion of the [SVG viewport](coords.md#SVGViewport) in the chapter on [Coordinate Systems, Transformations and Units](coords.md).
**<a id="TermViewportCoordinateSystem"></a>viewport coordinate system**
: In general, a coordinate system defines locations and distances on the current [canvas](#TermCanvas). The viewport coordinate system is the coordinate system that is active at the start of processing of an [‘svg’](struct.md#SVGElement) element, before processing the optional [‘viewBox’](coords.md#ViewBoxAttribute) attribute. In the case of an SVG document fragment that is embedded within a parent document which uses CSS to manage its layout, then the viewport coordinate system will have the same orientation and lengths as in CSS, with the origin at the top-left on the [viewport](#TermViewport). See [The initial viewport](coords.md#ViewportSpace) and [Establishing a new viewport](coords.md#EstablishingANewViewport).
**<a id="TermViewportSpace"></a>viewport space**
: A synonym for [viewport coordinate system](#TermViewportCoordinateSystem).
**<a id="TermViewportUnits"></a>viewport units**
: A coordinate value or length expressed in viewport units represents a coordinate value or length in the [viewport coordinate system](#TermViewportCoordinateSystem). Thus, 10 viewport units represents a length of 10 units in the viewport coordinate system.
**<a id="TermXLinkAttributes"></a>XLink attributes**
: The XLink attributes are the seven attributes defined in the XML Linking Language specification [[XLINK](refs.md#ref-XLINK)], which are used on various SVG elements that can reference resources. The most import XLink attribute is ‘xlink:href’, whose definition can be found on each element that allows it. The remaining XLink attributes are [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute) and [‘xlink:actuate’](linking.md#XLinkActuateAttribute).
SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](index.md) ⋅ [Next](concepts.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
