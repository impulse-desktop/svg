<!-- https://www.w3.org/TR/SVG11/linking.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](interact.md) ⋅ [Next](https://www.w3.org/TR/SVG11/script.html) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 17 Linking

## <a id="IRIReference"></a>17.1 References

### <a id="HeadOverview"></a>17.1.1 Overview

On the Internet, resources are identified using [IRI](intro.md#TermIRIReference)s (Internationalized Resource Identifiers). For example, an SVG file called someDrawing.svg located at http://example.com might have the following [IRI](intro.md#TermIRIReference):

```
http://example.com/someDrawing.svg
```

An [IRI](intro.md#TermIRIReference) can also address a particular element within an XML document by including an [IRI](intro.md#TermIRIReference) fragment identifier as part of the [IRI](intro.md#TermIRIReference). An [IRI](intro.md#TermIRIReference) which includes an [IRI](intro.md#TermIRIReference) fragment identifier consists of an optional base [IRI](intro.md#TermIRIReference), followed by a "#" character, followed by the [IRI](intro.md#TermIRIReference) fragment identifier. For example, the following [IRI](intro.md#TermIRIReference) can be used to specify the element whose ID is "Lamppost" within file someDrawing.svg:

```
http://example.com/someDrawing.svg#Lamppost
```

### <a id="IRIandURI"></a>17.1.2 IRIs and URIs

Internationalized Resource Identifiers ([IRI](intro.md#TermIRIReference)s) are a more generalized complement to Uniform Resource Identifiers (URIs). An [IRI](intro.md#TermIRIReference) is a sequence of characters from the Universal Character Set [[UNICODE](refs.md#ref-UNICODE)]. A URI is constructed from a much more restricted set of characters. All URIs are already conformant [IRI](intro.md#TermIRIReference)s. A mapping from [IRI](intro.md#TermIRIReference)s to URIs is defined by the [IRI](intro.md#TermIRIReference) specification, which means that IRIs can be used instead of URIs in XML documents, to identify resources. [IRI](intro.md#TermIRIReference)s can be converted to URIs for resolution on a network, if the protocol does not support [IRI](intro.md#TermIRIReference)s directly.

Previous versions of SVG, following XLink, defined an IRI reference type as a URI *or as a sequence of characters which must result in an IRI after a particular escaping procedure was applied*. The escaping procedure was repeated in the XLink 1.0 specification [[XLINK](refs.md#ref-XLINK)], and in the W3C XML Schema Part 2: Datatypes specification [[SCHEMA2](refs.md#ref-SCHEMA2)]. This copying introduced the possibility of error and divergence, but was done because the [IRI](intro.md#TermIRIReference) specification was not yet standardized.

In this specification, the correct term [IRI](intro.md#TermIRIReference) is used for this "URI or sequence of characters plus an algorithm" and the escaping method, which turns IRIs into URIs, is defined by reference to the IRI specification [[RFC3987](refs.md#ref-RFC3987)], which has since become an IETF Proposed Standard. Other W3C specifications are expected to be revised over time to remove these duplicate descriptions of the escaping procedure and to refer to [IRI](intro.md#TermIRIReference) directly.

### <a id="IRIforms"></a>17.1.3 Syntactic forms: IRI and FuncIRI

IRIs are used in the [‘xlink:href’](animate.md#HrefAttribute) attribute. Some attributes allow both IRIs and text strings as content. To disambiguate a text string from a relative IRI, the functional notation [<FuncIRI>](types.md#DataTypeFuncIRI) is used. This is simply an IRI delimited with a functional notation. **Note:** For historical reasons, the delimiters are "url(" and ")", for compatibility with the CSS specifications. The FuncIRI form is used in [presentation attributes](styling.md#StylingUsingPresentationAttributes) .

SVG makes extensive use of [IRI](intro.md#TermIRIReference) references, both absolute and relative, to other objects. For example, to [fill](intro.md#TermFill) a rectangle with a linear gradient, you first define a [‘linearGradient’](pservers.md#LinearGradients) element and give it an ID, as in:

```
<linearGradient xml:id="MyGradient">...</linearGradient>
```

You then reference the linear gradient as the value of the [‘fill’](painting.md#FillProperty) property for the rectangle, as in the following example:

```
<rect fill="url(#MyGradient)"/>
```

SVG supports two types of IRI references:

- [local IRI references](intro.md#TermLocalIRIReference), where the IRI reference does not contain an <absoluteIRI> or <relativeIRI> and thus only contains a fragment identifier (i.e., #<elementID> or #xpointer(id<elementID>))
- [non-local IRI references](intro.md#TermNonLocalIRIReference), where the IRI reference does contain an <absoluteIRI> or <relativeIRI>

### <a id="processingIRI"></a>17.1.4 Processing of IRI references

The following rules apply to the processing of IRI references:

- IRI references to nodes that do not exist shall be treated as invalid references.
- IRI references to elements which are inappropriate targets for the given reference shall be treated as invalid references (see list below for appropriate targets). For example, the [‘clip-path’](masking.md#ClipPathProperty) property can only refer to [‘clipPath’](masking.md#ClipPathElement) elements. The property setting clip-path:url(#MyElement) is an invalid reference if the referenced element is not a [‘clipPath’](masking.md#ClipPathElement).
- IRI references that directly or indirectly reference themselves are treated as invalid circular references.

The following list describes the elements and properties that allow IRI references and the valid target types for those references:

- the [‘a’](#AElement) element can reference any local or non-local resource
- the [‘altGlyph’](text.md#AltGlyphElement) element must reference either an [‘altGlyphDef’](text.md#AltGlyphDefElement) element or a [‘glyph’](fonts.md#GlyphElement) element
- the [‘animate’](animate.md#AnimateElement) element (see [Identifying the target element for an animation](animate.md#TargetElement) for reference rules)
- the [‘animateColor’](animate.md#AnimateColorElement) element (see [Identifying the target element for an animation](animate.md#TargetElement) for reference rules)
- the [‘animateMotion’](animate.md#AnimateMotionElement) element (see [Identifying the target element for an animation](animate.md#TargetElement) for reference rules)
- the [‘animateTransform’](animate.md#AnimateTransformElement) element (see [Identifying the target element for an animation](animate.md#TargetElement) for reference rules)
- the [‘clip-path’](masking.md#ClipPathProperty) property must reference a [‘clipPath’](masking.md#ClipPathElement) element
- the [‘color-profile’](color.md#ColorProfileElement) element must reference an ICC profile resource
- the [‘color-profile’](color.md#ColorProfileProperty) property must reference an ICC profile resource or a [‘color-profile’](color.md#ColorProfileElement) element
- the ['src'](color.md#ColorProfileSrcProperty) descriptor on an @color-profile definition must reference an ICC profile resource or a [‘color-profile’](color.md#ColorProfileElement) element
- the [‘cursor’](interact.md#CursorElement) element must reference a resource that can provide an image for the cursor graphic
- the [‘cursor’](interact.md#CursorProperty) property must reference a resource that can provide an image for the cursor graphic
- the [‘feImage’](filters.md#feImageElement) element must reference any local or non-local resource
- the [‘fill’](painting.md#FillProperty) property (see [Specifying paint](painting.md#SpecifyingPaint) for reference rules)
- the [‘filter’](filters.md#FilterElement) element must reference a [‘filter’](filters.md#FilterElement) element
- the [‘filter’](filters.md#FilterProperty) property must reference a [‘filter’](filters.md#FilterElement) element
- the [‘image’](struct.md#ImageElement) element must reference any local or non-local resource
- the [‘linearGradient’](pservers.md#LinearGradientElement) element must reference a [‘linearGradient’](pservers.md#LinearGradientElement) or [‘radialGradient’](pservers.md#RadialGradientElement) element
- the [‘marker’](painting.md#MarkerProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘marker-mid’](painting.md#MarkerMidProperty) and [‘marker-end’](painting.md#MarkerEndProperty) properties must reference a [‘marker’](painting.md#MarkerElement) element.
- the [‘mask’](masking.md#MaskProperty) property must reference a [‘mask’](masking.md#MaskElement) element
- the [‘pattern’](pservers.md#PatternElement) element must reference a [‘pattern’](pservers.md#PatternElement) element
- the [‘radialGradient’](pservers.md#RadialGradientElement) element must reference a [‘linearGradient’](pservers.md#LinearGradientElement) or [‘radialGradient’](pservers.md#RadialGradientElement) element
- the [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement) element must reference an external resource that provides the script content
- the [‘stroke’](painting.md#StrokeProperty) property (see [Specifying paint](painting.md#SpecifyingPaint) for reference rules)
- the [‘textPath’](text.md#TextPathElement) element must reference a [‘path’](paths.md#PathElement) element
- the [‘tref’](text.md#TRefElement) element can reference any SVG element
- the [‘set’](animate.md#SetElement) element (see [Identifying the target element for an animation](animate.md#TargetElement) for reference rules)
- the [‘use’](struct.md#UseElement) element can reference any local or non-local resource

The following rules apply to the processing of invalid IRI references:

- An invalid local IRI reference (i.e., an invalid references to a node within the current document) represents an error (see [Error processing](implnote.md#ErrorProcessing)), apart from the [‘xlink:href’](#AElementXLinkHrefAttribute) attribute on the [‘a’](#AElement) element and the properties that allow for backup values in the case where the IRI reference is invalid (see [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty)).
- An invalid circular IRI reference represents an error (see [Error processing](implnote.md#ErrorProcessing)).
- When attribute [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute) has been set to 'true' on the referencing element or one of its ancestors, then an unresolved external IRI reference (i.e., a resource that cannot be located) represents an error (see [Error processing](implnote.md#ErrorProcessing)).

### <a id="xlinkRefAttrs"></a>17.1.5 IRI reference attributes

<a id="xlinkRefAttrsEmbed"></a>[IRI](intro.md#TermIRIReference) references are normally specified with an ‘href’ attribute in the XLink [XLink] namespace. For example, if the prefix of 'xlink' is used for attributes in the XLink namespace, then the attribute is be specified as ‘xlink:href’. The value of this attribute forms a reference for the desired resource (or secondary resource, if there is a fragment identifier).

The value of the ‘href’ attribute must be an [Internationalized Resource Identifier](types.md#DataTypeIRI).

If the protocol, such as HTTP, does not support [IRI](intro.md#TermIRIReference)s directly, the [IRI](intro.md#TermIRIReference) is converted to a URI by the SVG implementation, as described in section 3.1 of the IRI specification [[RFC3987](refs.md#ref-RFC3987).

Because it is impractical for any application to check that a value is an [IRI](intro.md#TermIRIReference) reference, this specification follows the lead of the IRI Specification in this matter and imposes no such conformance testing requirement on SVG applications.

If the [IRI](intro.md#TermIRIReference) reference is relative, its absolute version must be computed by the method described in XML Base before use [[XML-BASE](refs.md#ref-XML-BASE)].

**<a id="XLinkTypeAttribute"></a> xlink:type = "simple"**
:

Identifies the type of XLink being used. In SVG 1.1, only simple links are available. Links are simple links by default, so the attribute xlink:type="simple" is optional and may be omitted on simple links. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="XLinkRoleAttribute"></a> xlink:role = "[<IRI>](types.md#DataTypeIRI)"**
:

An optional [IRI](intro.md#TermIRIReference) reference that identifies some resource that describes the intended property. The value must be an [IRI](intro.md#TermIRIReference) reference as defined in [RFC3987], except that if the [IRI](intro.md#TermIRIReference) scheme used is allowed to have absolute and relative forms, the [IRI](intro.md#TermIRIReference) portion must be absolute. When no value is supplied, no particular role value shall be inferred. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="XLinkArcRoleAttribute"></a> xlink:arcrole = "[<IRI>](types.md#DataTypeIRI)"**
:

An optional [IRI](intro.md#TermIRIReference) reference that identifies some resource that describes the intended property. The value must be an [IRI](intro.md#TermIRIReference) reference as defined in [RFC3987], except that if the [IRI](intro.md#TermIRIReference) scheme used is allowed to have absolute and relative forms, the [IRI](intro.md#TermIRIReference) portion must be absolute. When no value is supplied, no particular role value shall be inferred. The arcrole attribute corresponds to the [[RDF-PRIMER](refs.md#ref-RDF-PRIMER)] notion of a property, where the role can be interpreted as stating that "starting-resource HAS arc-role ending-resource." This contextual role can differ from the meaning of an ending resource when taken outside the context of this particular arc. For example, a resource might generically represent a "person," but in the context of a particular arc it might have the role of "mother" and in the context of a different arc it might have the role of "daughter." Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="XLinkTitleAttribute"></a> xlink:title = "[<anything>](types.md#DataTypeAnything)"**
:

The title attribute shall be used to describe the meaning of a link or resource in a human-readable fashion, along the same lines as the role or arcrole attribute. A value is optional; if a value is supplied, it shall contain a string that describes the resource. In general it is preferable to use a [‘title’](struct.md#TitleElement) child element rather than a ‘title’ attribute. The use of this information is highly dependent on the type of processing being done. It may be used, for example, to make titles available to applications used by visually impaired users, or to create a table of links, or to present help text that appears when a user lets a mouse pointer hover over a starting resource. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="XLinkShowAttribute"></a> xlink:show = "new' | 'replace' | 'embed' | 'other' | 'none'**
:

This attribute is provided for backwards compatibility with SVG 1.1. It provides documentation to XLink-aware processors. In case of a conflict, the target attribute has priority, since it can express a wider range of values. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="XLinkActuateAttribute"></a> xlink:actuate = "onLoad'**
:

This attribute is provided for backwards compatibility with SVG 1.1. It provides documentation to XLink-aware processors. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

In all cases, for compliance with either the "Namespaces in XML 1.0" or the "Namespaces in XML 1.1" Recommendation [XML-NS10][XML-NS], an explicit XLink namespace declaration must be provided whenever one of the above XLink attributes is used within SVG content. One simple way to provide such an XLink namespace declaration is to include an ‘xmlns’ attribute for the XLink namespace on the [‘svg’](struct.md#SVGElement) element for content that uses XLink attributes. For example:

```
<svg xmlns:xlink="http://www.w3.org/1999/xlink" ...>
  <image xlink:href="foo.png" .../>
</svg>
```

## <a id="Links"></a>17.2 Links out of SVG content: the ‘a’ element

SVG provides an [‘a’](#AElement) element, to indicate links (also known as *hyperlinks* or *Web links*). The [‘a’](#AElement) element may contain any element that its parent may contain, except itself.

SVG uses XLink ([XLink]) for all link definitions. SVG 1.1 only requires that user agents support XLink's notion of simple links. Each simple link associates exactly two resources, one local and one remote, with an arc going from the former to the latter.

A simple link is defined for each separate rendered element contained within the [‘a’](#AElement) element; thus, if the [‘a’](#AElement) element contains three [‘circle’](shapes.md#CircleElement) elements, a link is created for each circle. For each rendered element within an [‘a’](#AElement) element, the given rendered element is the local resource (the source anchor for the link).

The remote resource (the destination for the link) is defined by a [IRI](#IRIReference) specified by the XLink [‘xlink:href’](#AElementXLinkHrefAttribute) attribute on the [‘a’](#AElement) element. The remote resource may be any Web resource (e.g., an image, a video clip, a sound bite, a program, another SVG document, an HTML document, an element within the current document, an element within a different document, etc.). By activating these links (by clicking with the mouse, through keyboard input, voice commands, etc.), users may visit these resources.

<a id="ExampleLink01"></a>Example link01 assigns a link to an ellipse.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="5cm" height="3cm" viewBox="0 0 5 3" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>Example link01 - a link on an ellipse
  </desc>
  <rect x=".01" y=".01" width="4.98" height="2.98" 
        fill="none" stroke="blue"  stroke-width=".03"/>
  <a xlink:href="http://www.w3.org">
    <ellipse cx="2.5" cy="1.5" rx="2" ry="1"
             fill="red" />
  </a>
</svg>
```

Example link01

| [image: Example link01 — a link on an ellipse] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/linking/link01.svg)

If the above SVG file is viewed by a user agent that supports both SVG and HTML, then clicking on the ellipse will cause the current window or frame to be replaced by the W3C home page.

<a id="AElement"></a>

‘a’

**Categories:**
: [Container element](intro.md#TermContainerElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](struct.md#DefsElement), [‘g’](struct.md#GElement), [‘svg’](struct.md#SVGElement), [‘symbol’](struct.md#SymbolElement), [‘use’](struct.md#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement)
- [‘a’](#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](struct.md#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](struct.md#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](#ViewElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#AElementXLinkHrefAttribute), [‘xlink:show’](#XLinkShowAttribute), [‘xlink:actuate’](#AElementXLinkActuateAttribute), [‘xlink:type’](#XLinkTypeAttribute), [‘xlink:role’](#XLinkRoleAttribute), [‘xlink:arcrole’](#XLinkArcRoleAttribute), [‘xlink:title’](#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘xlink:href’](#AElementXLinkHrefAttribute)
- [‘xlink:show’](#XLinkShowAttribute)
- [‘xlink:actuate’](#AElementXLinkActuateAttribute)
- [‘target’](#AElementTargetAttribute)

**DOM Interfaces:**
:

- [SVGAElement](#InterfaceSVGAElement)

*Attribute definitions:*

**<a id="XLinkShowAttribute2"></a> xlink:show = "new" | "replace" **
:

This attribute provides documentation to XLink-aware processors. If target="_blank" then use xlink:show="new" else use 'replace'. In case of a conflict, the target attribute has priority, since it can express a wider range of values. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="AElementXLinkActuateAttribute"></a>xlink:actuate = "onRequest"**
:

This attribute provides documentation to XLink-aware processors that an application should traverse from the starting resource to the ending resource only on a post-loading event triggered for the purpose of traversal. Refer to the XML Linking Language (XLink) [[XLINK](refs.md#ref-XLINK)].

[Animatable](animate.md#Animatable): no.

**<a id="AElementXLinkHrefAttribute"></a>xlink:href = "[<IRI>](types.md#DataTypeIRI)"**
:

The location of the referenced object, expressed as an [IRI reference](intro.md#TermIRIReference).

[Animatable](animate.md#Animatable): yes.

**<a id="AElementTargetAttribute"></a>target = "_replace" | "_self" | "_parent" | "_top" | "_blank" | "[<XML-Name>](types.md#DataTypeXML-Name)" **
:

This attribute should be used when there are multiple possible targets for the ending resource, such as when the parent document is a multi-frame HTML or XHTML document. This attribute specifies the name or portion of the target window, frame, pane, tab, or other relevant presentation context (e.g., an HTML or XHTML frame, iframe, or object element) into which a document is to be opened when the link is activated:

**_replace**
: The current SVG image is replaced by the linked content in the same rectangular area in the same frame as the current SVG image.
**_self**
: The current SVG image is replaced by the linked content in the same frame as the current SVG image. If the attribute is not specified, '_self' is assumed.
**_parent**
: The immediate frameset parent of the SVG image is replaced by the linked content.
**_top**
: The content of the full window or tab, including any frames, is replaced by the linked content
**_blank**
: A new un-named window or tab is requested for the display of the linked content. If this fails, the result is the same as _top
**[<XML-Name>](types.md#DataTypeXML-Name)**
: Specifies the name of the frame, pane, or other relevant presentation context for display of the linked content. If this already exists, it is re-used, replacing the existing content. If it does not exist, it is created (the same as '_blank', except that it now has a name).
Note: The value '_new' is *not* a legal value for target (use '_blank').

[Animatable](animate.md#Animatable): yes.

## <a id="LinksIntoSVG"></a>17.3 Linking into SVG content: IRI fragments and SVG views

### <a id="IntroFragmentsViews"></a>17.3.1 Introduction: IRI fragments and SVG views

Because SVG content often represents a picture or drawing of something, a common need is to link into a particular view of the document, where a view indicates the initial transformations so as to present a closeup of a particular section of the document.

### <a id="SVGFragmentIdentifiers"></a>17.3.2 SVG fragment identifiers

To link into a particular view of an SVG document, the IRI fragment identifier needs to be a correctly formed SVG fragment identifier. An SVG fragment identifier defines the meaning of the "selector" or "fragment identifier" portion of IRIs that locate resources of MIME media type "image/svg+xml".

An SVG fragment identifier can come in two forms:

- Shorthand *bare name* form of addressing (e.g., MyDrawing.svg#MyView). This form of addressing, which allows addressing an SVG element by its ID, is compatible with the fragment addressing mechanism for older versions of HTML.
- <a id="SVGViewSpecification"></a>SVG view specification (e.g., MyDrawing.svg#svgView(viewBox(0,200,1000,1000))). This form of addressing specifies the desired view of the document (e.g., the region of the document to view, the initial zoom level) completely within the SVG fragment specification. The contents of the SVG view specification are the five parameter specifications, viewBox(...), preserveAspectRatio(...), transform(...), zoomAndPan(...) and viewTarget(...), whose parameters have the same meaning as the corresponding attributes on a [‘view’](#ViewElement) element, or, in the case of transform(...), the same meaning as the corresponding attribute has on a [‘g’](struct.md#GElement) element).

An SVG fragment identifier is defined as follows:

```
SVGFragmentIdentifier ::= BareName |
                          SVGViewSpec
                
BareName ::= XML_Name
SVGViewSpec ::= 'svgView(' SVGViewAttributes ')'
SVGViewAttributes ::= SVGViewAttribute |
                      SVGViewAttribute ';' SVGViewAttributes
                      
SVGViewAttribute ::= viewBoxSpec |
                     preserveAspectRatioSpec |
                     transformSpec |
                     zoomAndPanSpec |
                     viewTargetSpec
viewBoxSpec ::= 'viewBox(' ViewBoxParams ')'
preserveAspectRatioSpec = 'preserveAspectRatio(' AspectParams ')'
transformSpec ::= 'transform(' TransformParams ')'
zoomAndPanSpec ::= 'zoomAndPan(' ZoomAndPanParams ')'
viewTargetSpec ::= 'viewTarget(' ViewTargetParams ')'
```

where:

- ViewBoxParams corresponds to the parameter values for the [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [‘view’](#ViewElement) element. For example, viewBox(0,0,200,200).
- AspectParams corresponds to the parameter values for the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute on the [‘view’](#ViewElement) element. For example, preserveAspectRatio(xMidYMid).
- TransformParams corresponds to the parameter values for the [‘transform’](coords.md#TransformAttribute) attribute that is available on many elements. For example, transform(scale(5)).
- ZoomAndPanParams corresponds to the parameter values for the [‘zoomAndPan’](interact.md#ZoomAndPanAttribute) attribute on the [‘view’](#ViewElement) element. For example, zoomAndPan(magnify).
- ViewTargetParams corresponds to the parameter values for the [‘viewTarget’](#ViewElementViewTargetAttribute) attribute on the [‘view’](#ViewElement) element. For example, viewTarget(MyElementID).

Spaces are not allowed in fragment specifications; thus, commas are used to separate numeric values within an SVG view specification (e.g., #svgView(viewBox(0,0,200,200))) and semicolons are used to separate attributes (e.g., #svgView(viewBox(0,0,200,200);preserveAspectRatio(none))).

Semicolons used to separate 'SVGViewAttribute' in SVG fragments may be url-escaped (as %3B); this is useful when animating a (semi-colon separated) list of IRIs because otherwise the semicolon would be interpreted as a list separator.

The five types of SVGViewAttribute may occur in any order, but each type may only occur at most one time in a correctly formed SVGViewSpec.

When a source document performs a link into an SVG document, for example via an HTML anchor element ([[HTML4](refs.md#ref-HTML4)], section 12.2; i.e., <a href=...> element in HTML) or an XLink specification [[XLINK](refs.md#ref-XLINK)], then the SVG fragment identifier specifies the initial view into the SVG document, as follows:

- If no SVG fragment identifier is provided (e.g, the specified IRI did not contain a "#" character, such as MyDrawing.svg), then the initial view into the SVG document is established using the view specification attributes (i.e., [‘viewBox’](coords.md#ViewBoxAttribute), etc.) on the [outermost svg element](intro.md#TermOutermostSVGElement).
- If the SVG fragment identifier addresses a [‘view’](#ViewElement) element within an SVG document (e.g., MyDrawing.svg#MyView or MyDrawing.svg#xpointer(id('MyView'))) then the closest ancestor [‘svg’](struct.md#SVGElement) element is displayed in the viewport. Any view specification attributes included on the given [‘view’](#ViewElement) element override the corresponding view specification attributes on the closest ancestor [‘svg’](struct.md#SVGElement) element.
- If the SVG fragment identifier addresses specific SVG view (e.g., MyDrawing.svg#svgView(viewBox(0,200,1000,1000))), then the document fragment defined by the closest ancestor [‘svg’](struct.md#SVGElement) element is displayed in the viewport using the SVG view specification provided by the SVG fragment identifier.
- If the SVG fragment identifier addresses any element other than a [‘view’](#ViewElement) element, then the document defined by the closest ancestor [‘svg’](struct.md#SVGElement) element is displayed in the viewport using the view specification attributes on that [‘svg’](struct.md#SVGElement) element.

### <a id="ViewElement"></a>17.3.3 Predefined views: the ‘view’ element

The ‘view’ element is defined as follows:

‘view’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘viewBox’](coords.md#ViewBoxAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘zoomAndPan’](interact.md#ZoomAndPanAttribute)
- [‘viewTarget’](#ViewElementViewTargetAttribute)

**DOM Interfaces:**
:

- [SVGViewElement](#InterfaceSVGViewElement)

*Attribute definitions:*

**<a id="ViewElementViewTargetAttribute"></a>viewTarget = "*XML_Name [XML_NAME]**"**
: Indicates the target object associated with the view. [Animatable](animate.md#Animatable): no.
### <a id="ViewHighlight"></a>17.3.4 Highlighting views

It is helpful to users if the target element(s) are highlighted. The visual styling of this highlight should be decided by the document author, because the SVG User Agent has no way to determine what changes would make the elements more visible.

The CSS :target selector ([[SELECTORS](refs.md#ref-SELECTORS)], section 6.2.2) may be used in a stylesheet to provide alternate styling for elements which are the target of links. For example:

```
<style type="text/css">
#foo:target {filter: url(#glow)}
/* when the element with id foo is linked to, use a glow filter */

.bar :target {stroke: green; fill-opacity: 0.5}
/* when any descendants of elements with class bar are linked
   to, make the fill partly transparent and use a green stroke */

:target {stroke: red }
/* for everything else, just use a red stroke */ 
</style>
```

## <a id="DOMInterfaces"></a>17.4 DOM interfaces

### <a id="InterfaceSVGAElement"></a>17.4.1 Interface SVGAElement

The [SVGAElement](#InterfaceSVGAElement) interface corresponds to the [‘a’](#AElement) element.

```
interface SVGAElement : SVGElement,
                        SVGURIReference,
                        SVGTests,
                        SVGLangSpace,
                        SVGExternalResourcesRequired,
                        SVGStylable,
                        SVGTransformable {
  readonly attribute SVGAnimatedString target;
};
```

**Attributes:**
:

**<a id="__svg__SVGAElement__target"></a>**target** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘target’](#AElementTargetAttribute) on the given [‘a’](#AElement) element.

### <a id="InterfaceSVGViewElement"></a>17.4.2 Interface SVGViewElement

The [SVGViewElement](#InterfaceSVGViewElement) interface corresponds to the [‘view’](#ViewElement) element.

```
interface SVGViewElement : SVGElement,
                           SVGExternalResourcesRequired,
                           SVGFitToViewBox,
                           SVGZoomAndPan {
  readonly attribute SVGStringList viewTarget;
};
```

**Attributes:**
:

**<a id="__svg__SVGViewElement__viewTarget"></a>**viewTarget** (readonly [SVGStringList](types.md#InterfaceSVGStringList))**
:

Corresponds to attribute [‘viewTarget’](#ViewElementViewTargetAttribute) on the given [‘view’](#ViewElement) element. A list of DOMString values which contain the names listed in the [‘viewTarget’](#ViewElementViewTargetAttribute) attribute. Each of the DOMString values can be associated with the corresponding element using the getElementById() method call.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](interact.md) ⋅ [Next](https://www.w3.org/TR/SVG11/script.html) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
