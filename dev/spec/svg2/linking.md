<!-- https://svgwg.org/svg2-draft/linking.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 16: Linking

## <a id="URLReference"></a>16.1. References

### <a id="HeadOverview"></a>16.1.1. Overview

On the Internet, resources are identified using [URLs](#TermURLReference) (Internationalized Resource Identifiers). For example, an SVG file called someDrawing.svg located at http://example.com might have the following [URL](#TermURLReference):

```
http://example.com/someDrawing.svg
```

An [URL](#TermURLReference) can also address a particular element within an XML document by including an [URL](#TermURLReference) fragment identifier as part of the [URL](#TermURLReference). An [URL](#TermURLReference) which includes an [URL](#TermURLReference) fragment identifier consists of an optional base [URL](#TermURLReference), followed by a "#" character, followed by the [URL](#TermURLReference) fragment identifier. For example, the following [URL](#TermURLReference) can be used to specify the element whose ID is "Lamppost" within file someDrawing.svg:

```
http://example.com/someDrawing.svg#Lamppost
```

### <a id="definitions"></a>16.1.2. Definitions

***<a id="TermURLReference"></a>URL reference***
: An URL reference is an Internationalized Resource Identifier, as defined in Internationalized Resource Identifiers [[rfc3987](refs.md#ref-rfc3987)]. See [References](#URLReference) and [References and the ‘defs’ element](struct.md#Head).
***<a id="TermURLReferenceWithFragmentIdentifier"></a>URL reference with fragment identifier***
: An Internationalized Resource Identifier [[rfc3987](refs.md#ref-rfc3987)] that can include an <absoluteURL> or <relativeURL> and a identifier of the fragment in that resource. See [References and the ‘defs’ element](struct.md#Head). URL reference with fragment identifiers are commonly used to reference [paint servers](pservers.md).
***<a id="TermExternalReference"></a>external file reference***
: A [URL reference](#TermURLReference) or [URL reference with fragment identifier](#TermURLReferenceWithFragmentIdentifier) which refers to a resource that is not part of the current document.
***<a id="TermSameDocumentURL"></a>same-document URL reference***
: A [URL reference with fragment identifier](#TermURLReferenceWithFragmentIdentifier) where the non-fragment part of the URL refers to the current document.
***<a id="TermDataURL"></a>data URL***
: A [URL reference](#TermURLReference) to an embedded document specified using the "data" URL scheme [[rfc2397](refs.md#ref-rfc2397)]. Data URL references are neither [external file references](#TermExternalReference) nor [same-document URL references](#TermSameDocumentURL).
***<a id="TermCircularReference"></a>circular reference***
: [URL references](#TermURLReference) that directly or indirectly reference themselves are treated as invalid circular references. What constitutes a circular reference will depend on how the referenced resource is used, and may include a reference to an ancestor of the current element.
***<a id="TermUnresolvedReference"></a>unresolved reference***
: A reference that is still being processed, and has not yet resulted in either an error or an identified resource.
***<a id="TermInvalidReference"></a>invalid reference***
:

Any of the following are invalid references:

- A circular reference.
- A [URL reference](#TermURLReference) that results in an error during [processing](#processingURL).
- A [URL reference](#TermURLReference) that cannot be resolved.
- A [URL references](#TermURLReference) to elements which are inappropriate targets for the given reference shall be treated as invalid references (see [Valid URL targets](#processingURL-validity) for appropriate targets). For example, the [clip-path](../css-masking-1.md#the-clip-path) property can only refer to ‘[clipPath](../css-masking-1.md#ClipPathElement)’ elements. The property setting clip-path:url(#MyElement) is an invalid reference if the referenced element is not a ‘[clipPath](../css-masking-1.md#ClipPathElement)’.

Invalid references may or may not be an error (see [Error processing](conform.md#ErrorProcessing)), depending on whether the referencing property or attribute defines fallback behavior.

### <a id="URLandURI"></a>16.1.3. URLs and URIs

Internationalized Resource Identifiers ([URLs](#TermURLReference)) are a more generalized complement to Uniform Resource Identifiers (URIs). An [URL](#TermURLReference) is a sequence of characters from the Universal Character Set [[UNICODE](refs.md#ref-unicode)]. A URI is constructed from a much more restricted set of characters. All URIs are already conformant [URLs](#TermURLReference). A mapping from [URLs](#TermURLReference) to URIs is defined by the [URL](#TermURLReference) specification, which means that URLs can be used instead of URIs in XML documents, to identify resources. [URLs](#TermURLReference) can be converted to URIs for resolution on a network, if the protocol does not support [URLs](#TermURLReference) directly.

Previous versions of SVG, following XLink, defined an URL reference type as a URI *or as a sequence of characters which must result in an URL after a particular escaping procedure was applied*. The escaping procedure was repeated in the XLink 1.0 specification [[xlink](refs.md#ref-xlink)], and in the W3C XML Schema Part 2: Datatypes specification [[xmlschema-2](refs.md#ref-xmlschema-2)]. This copying introduced the possibility of error and divergence, but was done because the [URL](#TermURLReference) specification was not yet standardized.

In this specification, the correct term [URL](#TermURLReference) is used for this "URI or sequence of characters plus an algorithm" and the escaping method, which turns URLs into URIs, is defined by reference to the URL specification [[rfc3987](refs.md#ref-rfc3987)], which has since become an IETF Proposed Standard. Other W3C specifications are expected to be revised over time to remove these duplicate descriptions of the escaping procedure and to refer to [URL](#TermURLReference) directly.

### <a id="URLforms"></a>16.1.4. Syntactic forms: URL and <url>

In SVG, most structural relationships between two elements are specified using a [URL](#TermURLReference) value in an [‘href’](#linkRefAttrs) attribute.

To describe linking relationships, this specification uses two different data types in attribute and property values: [URL](#TermURLReference) and <url>. The linking guidelines in this chapter apply to URLs specified with either syntax.

The [URL](#TermURLReference) data type is a simple URL string. In SVG, most structural relationships between two elements are specified using a [URL](#TermURLReference) value in an [‘href’](#linkRefAttrs) attribute.

<url> is different from [URL](#TermURLReference) and represents a CSS `url()` function value. (See CSS Values and Units for further details [[css-values](refs.md#ref-css-values-3)]). <url>s may be used for [presentation attributes](styling.md#TermPresentationAttribute) and their corresponding CSS properties [[css-values](refs.md#ref-css-values-3)].

[URL](#TermURLReference) is not a valid value for [presentation attributes](styling.md#TermPresentationAttribute) for structural relationships purposes. No non-[presentation attribute](styling.md#TermPresentationAttribute) allows <url> as value.

SVG makes extensive use of [URL](#TermURLReference) references, both absolute and relative, to other objects. For example, a ‘[linearGradient](pservers.md#LinearGradientElement)’ element may be based on another gradient element, so that only the differences between the two need to be specified, by referencing the source gradient with a URL in the ‘[href](pservers.md#LinearGradientElementHrefAttribute)’ attribute:

```
<linearGradient id="SourceGradient">...</linearGradient>
<linearGradient id="MyGradient"
                href="#SourceGradient">...</linearGradient>
```

To fill a rectangle with that gradient, the value of the rectangle's [fill](painting.md#FillProperty) property may be set so as to include a URL reference to the relevant ‘[linearGradient](pservers.md#LinearGradientElement)’ element; here is an example:

```
<rect fill="url(#MyGradient)"/>
```

### <a id="linkRefAttrs"></a>16.1.5. URL reference attributes

<a id="linkRefAttrsEmbed"></a>[URL references](#TermURLReference) are normally specified with an ‘href’ attribute. The value of this attribute forms a reference for the desired resource (or secondary resource, if there is a fragment identifier). The value of the ‘href’ attribute must be a URL.

Because it is impractical for any application to check that a value is an [URL reference](#TermURLReference), this specification follows the lead of the URL Specification in this matter and imposes no such conformance testing requirement on [SVG authoring tools](conform.md#TermSVGAuthoringTool). An invalid URL does not make an SVG document non-conforming. SVG user agents are only required to process URLs when needed, as specified in [Processing of URL references](#processingURL).

### <a id="XLinkRefAttrs"></a>16.1.6. Deprecated XLink URL reference attributes

In previous versions of SVG, the ‘href’ attribute was specified in the XLink namespace [[xlink](refs.md#ref-xlink)] namespace. This usage is now deprecated and instead [URL references](#TermURLReference) should be specified using the ‘href’ attribute without a namespace.

For backwards compatibility, the deprecated ‘[xlink:href](#XLinkHrefAttribute)’ attribute is defined below along with the ‘[xlink:title](#XLinkTitleAttribute)’ attribute which has also been deprecated.

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="XLinkHrefAttribute"></a>xlink:href* | URL [[URL]](types.md#attribute-url) | (none) | (see below) |

**
:

For backwards compatibility, elements with an ‘href’ attribute also recognize an ‘href’ attribute in the XLink namespace [[xlink](refs.md#ref-xlink)].

When the ‘href’ attribute is present in *both* the XLink namespace and without a namespace, the value of the attribute without a namespace shall be used. The attribute in the XLink namespace shall be ignored.

A [conforming SVG generator](conform.md#ConformingSVGGenerators) must generate ‘href’ attributes without a namespace. However, it may *also* generate ‘href’ attributes in the XLink namespace to provide backwards compatibility.

This attribute is Animatable if and only if the corresponding ‘href’ attribute is defined to be animatable.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="XLinkTitleAttribute"></a>xlink:title* | <anything> | (none) | no |

**
:

Deprecated attribute to describe the meaning of a link or resource in a human-readable fashion. New content should use a [‘title’](struct.md#TitleElement) child element rather than a ‘xlink:title’ attribute.

The use of this information is highly dependent on the type of processing being done. It may be used, for example, to make titles available to applications used by visually impaired users, or to create a table of links, or to present help text that appears when a user lets a mouse pointer hover over a starting resource.

The ‘title’ attribute, if used, must be in the XLink namespace. Refer to the XML Linking Language (XLink) [[xlink](refs.md#ref-xlink)].

When using the deprecated XLink attributes ‘[xlink:href](#XLinkHrefAttribute)’ or ‘[xlink:title](#XLinkTitleAttribute)’ an explicit XLink namespace declaration must be provided [[xml-names](refs.md#ref-xml-names)], One simple way to provide such an XLink namespace declaration is to include an ‘xmlns’ attribute for the XLink namespace on the ‘[svg](struct.md#SVGElement)’ element for content that uses XLink attributes. For example:

```
<svg xmlns:xlink="http://www.w3.org/1999/xlink" ...>
  <image xlink:href="foo.png" .../>
</svg>
```

### <a id="processingURL"></a>16.1.7. Processing of URL references

URLs are processed to identify a resource at the time they are needed, as follows:

- For the ‘[href](#AElementHrefAttribute)’ attribute of the ‘[a](#AElement)’ element, at the time the link is activated by the user.
- For all other ‘href’ attributes, at the time the element is connected to a document, or at the time when the attribute is set, whichever is later.
- For source URLs on embedded HTML media elements, as required based on source selection rules in the HTML specification [[HTML](refs.md#ref-html)].
- For all presentation attributes and style properties, at the time the property is required for rendering an element.

Legacy ‘[xlink:href](#XLinkHrefAttribute)’ attributes are processed at the time a corresponding ‘href’ attribute would be processed, but only if no such ‘href’ attribute exists on the element.

Processing a URL involves three steps: generating the absolute URL; fetching the document (if required); identifying the target element (if required).

A URL reference is [unresolved](#TermUnresolvedReference) until processing either results in an [invalid reference](#TermInvalidReference) or in the identification of the target resource. Unresolved references in the non-presentation attributes of [structurally external elements](struct.md#TermStructurallyExternalElement) prevent the [load event](interact.md#LoadEvent) from firing. User agents may place time limits on the resolution of references that are not [same-document URL references](#TermSameDocumentURL), after which the reference is treated as a network error (and therefore as an [invalid reference](#TermInvalidReference)).

For [same-document URL references](#TermSameDocumentURL) in a dynamic document, modifications or animations of attributes or properties, or removal of elements from the DOM, may cause an URL reference to return to the [unresolved](#TermUnresolvedReference) state. The user agent must once again attempt to resolve the URI to identify the referenced resource.

#### <a id="processingURL-absolute"></a>16.1.7.1. Generating the absolute URL

If the [URL reference](#TermURLReference) is relative, its absolute version must be computed before use. The absolute URL should be generated using one of the following methods:

- as described in [CSS Values and Units](../css-values-4.md#local-urls) if the reference occurs in a CSS file or in a [presentation attribute](styling.md#TermPresentationAttribute) [[css-values](refs.md#ref-css-values-3)]
- as described in XML Base before use [[xmlbase](refs.md#ref-xmlbase)] if the reference occurs in any other attribute in an XML document
- as described in the HTML specification if the reference occurs in a non-presentation attribute in an HTML document that is not an XML document [[HTML](refs.md#ref-html)]

The ‘xml:base’ attribute will only have an effect in XML documents; this includes SVG documents and XHTML documents but not HTML documents that are not XML. In contrast, a `base` element affects relative URLs in any SVG or HTML document, by altering the document base URL.

If the protocol, such as HTTP, does not support [URLs](#TermURLReference) directly, the [URL](#TermURLReference) must be converted to a URI by the user agent, as described in section 3.1 of the URL specification [[rfc3987](refs.md#ref-rfc3987)].

After generating the absolute URL:

-
If the URL is being processed following the activation of a link, the user agent must follow the algorithm for navigating to a URL described in the HTML specification [[HTML](refs.md#ref-html)]. The outcome of this algorithm varies depending on the ‘[target](#AElementTargetAttribute)’ browsing context and security restrictions between browsing contexts, and on whether the link is to the same document as is currently contained in that browsing context (in which case the fragment is navigated without reloading the document). If the document that was navigated was an SVG document, then adjust the target behavior as described in [Linking into SVG content](#LinksIntoSVG).
-
If the URL being processed is only [valid](#processingURL-validity) if it refers to a complete document file (such as the ‘href’ attribute of an ‘[image](embedded.md#ImageElement)’ and ‘[script](interact.md#ScriptElement)’ element), continue as indicated in [Fetching the document](#processingURL-fetch) (regardless of whether the URL is to the same document or not).
-
In all other cases, the URL is for a resource to be used in this SVG document. The user agent must parse the URL to separate out the target fragment from the rest of the URL, and compare it with the document base URL. If all parts other than the target fragment are equal, this is a [same-document URL reference](#TermSameDocumentURL), and processing the URL must continue as indicated in [Identifying the target element](#processingURL-target) with the current document as the referenced document.
-
Otherwise, the URL references a separate document, and the user agent must continue processing the URL as indicated in [Fetching the document](#processingURL-fetch).

As defined in [CSS Values and Units](../css-values-4.md#local-urls), a fragment-only URL in a style property must be treated as a [same-document URL reference](#TermSameDocumentURL), regardless of the file in which the property was declared.

#### <a id="processingURL-fetch"></a>16.1.7.2. Fetching the document

SVG properties and attributes may reference other documents. When processing such a URL, the user agent should fetch the referenced document as described in this section, except under the following conditions:

-
If the URL reference is from the href attribute on SVG animation elements, only [same-document URL](#TermSameDocumentURL) references are allowed [[svg-animation](refs.md#ref-svg-animation)]. A URL referring to a different document is invalid and the document must not be fetched.
-
If the document containing the reference is being processed in [secure static mode](conform.md#secure-static-mode) or [secure animated mode](conform.md#secure-animated-mode), [external file references](#TermExternalReference) are disallowed. Unless the reference is a [data URL](#TermDataURL), the user agent must treat the reference as if there was a network error, making this an [invalid reference](#TermInvalidReference).
-
If any other security restrictions on the browsing context or user agent prevent accessing the external file, then the user agent must treat the reference as if there was a network error.

When fetching external resources from the Internet, user agents must use a potentially CORS-enabled request as defined in HTML [[HTML](refs.md#ref-html)] with the *corsAttributeState* as follows:

- For an ‘href’ reference on an ‘[image](embedded.md#ImageElement)’ element or ‘[script](interact.md#ScriptElement)’ element, the CORS state specified by the ‘[crossorigin](embedded.md#ImageElementCrossoriginAttribute)’ attribute.
- For a reference from a style property or presentation attribute, the "anonymous" state.
- For all other references, the "no-cors" state.

`base`

The request's *origin* is computed using the same rules as HTML, with an SVG ‘[script](interact.md#ScriptElement)’ element treated like an HTML `script` element, and an SVG ‘[image](embedded.md#ImageElement)’ element treated like an HTML `img` element. The *default origin behaviour* must be set to *taint*.

A future SVG specification may enable CORS references on other SVG elements with ‘href’ attributes.

If the fetching algorithm results in an error or an empty response body, the reference URL is treated as an [invalid reference](#TermInvalidReference).

If a valid response is returned, and the [valid URL targets](#processingURL-validity) for the reference include specific element types, the user agent must continue by [Processing the subresource document](#processingURL-parsing). Otherwise (if only entire-document the URL references are valid), then the fetched document is the referenced resource.

#### <a id="processingURL-parsing"></a>16.1.7.3. Processing the subresource document

Otherwise, the subresource must be parsed to identify the target element. If the fetched document is a type that the user agent can parse to create a document object model, it must process it in [secure static mode](conform.md#secure-static-mode) (meaning, do not fetch any additional external resources and do not run scripts or play animations). The document model generated for an external subresource reference must be immutable (read-only) and cannot be modified.

If a document object model can be generated from the fetched file, processing the URL must continue as indicated in [Identifying the target element](#processingURL-target) with the parsed subresource document as the referenced document. The user agent may commence the target-identification process prior to completely parsing the document.

User agents may maintain a list of external resource URLs and their associated parsed documents, and may re-use the documents for subsequent references, so long as doing so does not violate the processing mode, caching, and CORS requirements on the resource.

#### <a id="processingURL-target"></a>16.1.7.4. Identifying the target element

For URL references to a specific element, whether the reference is valid depends on whether the element can be located within the referenced document and whether it is of an allowed type.

Using the referenced document identified in previous processing steps (either an external subresource document or the current document), the target element is identified as follows:

-
If the URL does not specify a specific element in a target fragment, the target element is the root element of the referenced document.
-
Otherwise, the URL targets a specific element. If a matching element currently exists in the referenced document, then it is the target element.
-
Otherwise, there is no currently matching element. If the referenced document is immutable, then the URL reference is [invalid](types.md#TermInvalidValue). An external subresource document is always immutable once fully parsed; the current document is also immutable once parsed if it is being processed in any mode other than [dynamic interactive mode](conform.md#dynamic-interactive-mode).
-
Otherwise, observe mutations to the referenced document until the URL can be successfully resolved to define a target element, or until the document becomes immutable (e.g., a non-dynamic document finishes parsing).

The target element provides the referenced resource if (and only if) it is a [valid URL target](#processingURL-validity) for the reference.

#### <a id="processingURL-validity"></a>16.1.7.5. Valid URL targets

The valid target element types for ‘href’ (or ‘[xlink:href](#XLinkHrefAttribute)’) attributes are based on the element that has the attribute, as follows:

- the ‘[a](#AElement)’ element can reference any local or non-local resource
- the ‘[image](embedded.md#ImageElement)’ element must reference a document that can be processed as an image
- the ‘[linearGradient](pservers.md#LinearGradientElement)’ element must reference a different ‘[linearGradient](pservers.md#LinearGradientElement)’ or ‘[radialGradient](pservers.md#RadialGradientElement)’ element
- the ‘[pattern](pservers.md#PatternElement)’ element must reference another ‘[pattern](pservers.md#PatternElement)’ element
- the ‘[radialGradient](pservers.md#RadialGradientElement)’ element must reference a ‘[linearGradient](pservers.md#LinearGradientElement)’ or another ‘[radialGradient](pservers.md#RadialGradientElement)’ element
- the ‘[script](interact.md#ScriptElement)’ element must reference an external document that provides the script content
- the ‘[textPath](text.md#TextPathElement)’ element must reference an element type that implements the [SVGGeometryElement](types.md#InterfaceSVGGeometryElement) interface
- the ‘[use](struct.md#UseElement)’ element must reference an SVG-namespaced element

The valid target element types for style properties defined in this specification are as follows:

- the [fill](painting.md#FillProperty) property (see [Specifying paint](painting.md#SpecifyingPaint) for reference rules)
- the [marker](painting.md#MarkerProperty), [marker-start](painting.md#MarkerStartProperty), [marker-mid](painting.md#MarkerMidProperty) and [marker-end](painting.md#MarkerEndProperty) properties must reference a ‘[marker](painting.md#MarkerElement)’ element.
- the [shape-inside](text.md#TextShapeInside) and [shape-subtract](text.md#TextShapeSubtract) properties must reference an element type that implements the [SVGGeometryElement](types.md#InterfaceSVGGeometryElement) interface, or a document that can be processed as an image
- the [stroke](painting.md#StrokeProperty) property (see [Specifying paint](painting.md#SpecifyingPaint) for reference rules)

For references that allow either a reference to a target element, or to an image file (such as the [shape-inside](text.md#TextShapeInside), [shape-subtract](text.md#TextShapeSubtract), and [mask](../css-masking-1.md#propdef-mask) properties), the user agent must identify the target element and determine whether it is a valid target. If the resolved target element is not an allowed element type, the referenced resource is the entire document file; the target fragment is used in processing that file as with any other image.

In all other cases, if the resolved target element type (or document type) is not allowed for the URL reference, it is an [invalid reference](#TermInvalidReference).

## <a id="Links"></a>16.2. Links out of SVG content: the ‘a’ element

SVG provides an ‘[a](#AElement)’ element, to indicate links (also known as *hyperlinks* or *Web links*). An ‘[a](#AElement)’ element forms a link if it has a ‘[href](#AElementHrefAttribute)’ or ‘[xlink:href](#XLinkHrefAttribute)’ attribute; without these attributes the ‘[a](#AElement)’ element is an inactive placeholder for a link.

SVG 1.1 defined links in terms of the XLink specification ([XLink]), using attributes defined in the XLink namespace. SVG 2 uses an alternative set of attributes in the default namespace that are consistent with HTML links, and [deprecates the XLink attributes](#XLinkRefAttrs).

The ‘[a](#AElement)’ element may contain any element that its parent may contain, except for another ‘[a](#AElement)’ element; the same element is used for both graphical and textual linked content. Links may not be nested; if an ‘[a](#AElement)’ element is a descendent of another hyperlink element (whether in the SVG namespace or another namespace), user agents must ignore its href attribute and treat it as inactive. The invalid ‘[a](#AElement)’ element must still be rendered as a generic container element.

<a id="issue18"></a>The rendering of invalid nested links is at risk, and will likely be synchronized with any decisions regarding the rendering of unknown elements.

For pointer events processing, a linked hit region is defined for each separate rendered element contained within the ‘[a](#AElement)’ element (according to the value of their [pointer-events](interact.md#PointerEventsProperty) property), rather than for the bounding box of the ‘[a](#AElement)’ element itself. User agents must also ensure that all links are [focusable](interact.md#TermFocusable) and can be activated by keyboard commands.

The remote resource (the destination for the link) is defined by a [URL](#TermURLReference) specified by the ‘[href](#AElementHrefAttribute)’ attribute on the ‘[a](#AElement)’ element. The remote resource may be any Web resource (e.g., an image, a video clip, a sound bite, a program, another SVG document, an HTML document, an element within the current document, an element within a different document, etc.). In response to user activation of a link (by clicking with the mouse, through keyboard input, voice commands, etc.), user agents should attempt to fetch the specified resource document and either display it or make it available as a downloaded file.

<a id="ExampleLink01"></a>Example link01 assigns a link to an ellipse.

```
<?xml version="1.0" standalone="no"?>
<svg width="5cm" height="3cm" viewBox="0 0 5 3" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Example link01 - a link on an ellipse
  </desc>
  <rect x=".01" y=".01" width="4.98" height="2.98" 
        fill="none" stroke="blue"  stroke-width=".03"/>
  <a href="http://www.w3.org">
    <ellipse cx="2.5" cy="1.5" rx="2" ry="1"
             fill="red" />
  </a>
</svg>
```

[image: Example link01 — a link on an ellipse]

Example link01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/linking/link01.svg)

If the above SVG file is viewed by a user agent that supports both SVG and HTML, then clicking on the ellipse will cause the current window or frame to be replaced by the W3C home page.

<a id="AElement"></a>

‘*<a id="elementdef-a"></a>a*’

**Categories:**
: [Container element](struct.md#TermContainerElement), [renderable element](render.md#TermRenderableElement)
**Content model:**
: Descriptive content, plus any element or text allowed by its parent's content model, except for another a element. If the parent is a switch element, use the content model of the nearest ancestor that isn't a switch.
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- [deprecated xlink attributes](#XLinkRefAttrs) — ‘[xlink:href](#XLinkHrefAttribute)’, ‘[xlink:title](#XLinkTitleAttribute)’
- ‘[href](#AElementHrefAttribute)’
- ‘[target](#AElementTargetAttribute)’
- ‘[download](#AElementDownloadAttribute)’
- ‘[ping](#AElementPingAttribute)’
- ‘[rel](#AElementRelAttribute)’
- ‘[hreflang](#AElementHreflangAttribute)’
- ‘[type](#AElementTypeAttribute)’
- ‘[referrerpolicy](#AElementReferrerpolicyAttribute)’

**DOM Interfaces:**
:

- [SVGAElement](#InterfaceSVGAElement)

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="AElementHrefAttribute"></a>href* | URL [[URL]](types.md#attribute-url) | (none) | yes |

**
: The location of the referenced object, expressed as an [URL reference](#TermURLReference). Refer to the common handling defined for [URL reference attributes](#linkRefAttrs).
**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="AElementTargetAttribute"></a>target* | _self \| _parent \| _top \| _blank \| <XML-Name> | _self | yes |

**
:

This attribute should be used when there are multiple possible targets for the ending resource, such as when the parent document is embedded within an HTML or XHTML document, or is viewed with a tabbed browser. This attribute specifies the name of the browsing context (e.g., a browser tab or an (X)HTML iframe or object element) into which a document is to be opened when the link is activated:

**_self**
: The current SVG image is replaced by the linked content in the same browsing context as the current SVG image.
**_parent**
: The immediate parent browsing context of the SVG image is replaced by the linked content, if it exists and can be securely accessed from this document.
**_top**
: The content of the full active window or tab is replaced by the linked content, if it exists and can be securely accessed from this document
**_blank**
: A new un-named window or tab is requested for the display of the linked content, if this document can securely do so. If the user agent does not support multiple windows/tabs, the result is the same as _top.
**<XML-Name>**
: Specifies the name of the browsing context (tab, inline frame, object, etc.) for display of the linked content. If a context with this name already exists, and can be securely accessed from this document, it is re-used, replacing the existing content. If it does not exist, it is created (the same as '_blank', except that it now has a name). The name must be a valid XML Name [XML11], and should not start with an underscore (U+005F LOW LINE character), to meet the requirements of a valid browsing context name from HTML.
The normative definitions for browsing contexts and security restrictions on navigation actions between browsing contexts is HTML [[HTML](refs.md#ref-html)], specifically the chapter on loading web pages.

Previous versions of SVG defined the special target value '_replace'. It was never well implemented, and the distinction between '_replace' and '_self' has been made redundant by changes in the HTML definition of browsing contexts. Use '_self' to replace the current SVG document.

The value '_new' is *not* a legal value for target. Use '_blank' to open a document in a new tab/window.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="AElementDownloadAttribute"></a>download* | any value (if non-empty, value represents a suggested file name) | (none) | no |
| *<a id="AElementPingAttribute"></a>ping* | space-separated valid non-empty URL tokens [HTML] | (none) | no |
| *<a id="AElementRelAttribute"></a>rel* | space-separated keyword tokens [HTML] | (none) | no |
| *<a id="AElementHreflangAttribute"></a>hreflang* | A BCP 47 language tag string [HTML] | (none) | no |
| *<a id="AElementTypeAttribute"></a>type* | A MIME type string [HTML] | (none) | no |
| *<a id="AElementReferrerpolicyAttribute"></a>referrerpolicy* | A referrer policy string [REFERRERPOLICY] | (none) | no |

**
: These attributes further describe the targetted resource and its relationship to the current document. Allowed values and meaning are as defined for the `a` element in HTML.
## <a id="LinksIntoSVG"></a>16.3. Linking into SVG content: URL fragments and SVG views

Because SVG content often represents a picture or drawing of something, a common need is to link into a particular *view* of the document, where a view indicates the initial transformations so as to present a closeup of a particular section of the document.

### <a id="SVGFragmentIdentifiers"></a>16.3.1. SVG fragment identifiers

To link into a particular view of an SVG document, the [URL reference with fragment identifier](#TermURLReferenceWithFragmentIdentifier) needs to be a correctly formed *<a id="svg-fragment-identifier"></a>SVG fragment identifier*. An SVG fragment identifier defines the meaning of the "selector" or "fragment identifier" portion of URLs that locate resources of MIME media type "image/svg+xml".

An SVG fragment identifier can come in the following forms:

- Shorthand *bare name* form of addressing (e.g., MyDrawing.svg#MyView). This form of addressing, which allows addressing an SVG element by its ID, is compatible with the fragment addressing mechanism for older versions of HTML.
- An *<a id="SVGViewSpecification"></a>SVG view specification* (e.g., MyDrawing.svg#svgView(viewBox(0,200,1000,1000))). This form of addressing specifies the desired view of the document (e.g., the region of the document to view, the initial zoom level) completely within the SVG fragment specification. The contents of the SVG view specification are defined in [SVG fragment identifiers definitions](#SVGFragmentIdentifiersDefinitions) section.
- <a id="MediaFragments"></a>*Basic media fragments identifiers* of type spatial or temporal (e.g., MyDrawing.svg#xywh=0,200,1000,1000 or MyAnimation.svg#t=10)) as defined in [[Media Fragments URI 1.0 (basic)](refs.md#ref-media-frags)] and possibly combined using the & sign (e.g. MyDrawing.svg#xywh=0,200,1000,1000&t=10).

### <a id="SVGFragmentIdentifiersDefinitions"></a>16.3.2. SVG fragment identifiers definitions

An SVG fragment identifier is defined as follows:

```
SVGFragmentIdentifier ::= BareName *( "&" timesegment ) |
                          SVGViewSpec *( "&" timesegment ) |
                          spacesegment *( "&" timesegment ) |
                          timesegment *( "&" spacesegment )

BareName ::= XML_Name
SVGViewSpec ::= 'svgView(' SVGViewAttributes ')'
SVGViewAttributes ::= SVGViewAttribute |
                      SVGViewAttribute ';' SVGViewAttributes

SVGViewAttribute ::= viewBoxSpec |
                     preserveAspectRatioSpec |
                     transformSpec
viewBoxSpec ::= 'viewBox(' ViewBoxParams ')'
preserveAspectRatioSpec = 'preserveAspectRatio(' AspectParams ')'
transformSpec ::= 'transform(' TransformParams ')'
```

where:

- *ViewBoxParams* corresponds to the parameter values for the ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute on the ‘[view](#ViewElement)’ element. For example, viewBox(0,0,200,200).
- *AspectParams* corresponds to the parameter values for the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute on the ‘[view](#ViewElement)’ element. For example, preserveAspectRatio(xMidYMid).
- *TransformParams* corresponds to the parameter values for the [transform](coords.md#TransformProperty) property that is available on many elements. For example, transform(scale(5)). Currently additional transform styles and parameters (e.g. transform-origin, perspective) are not supported.

SVG view box parameters are applied in order, as defined in CSS Transforms specification (e.g. SVG view is transformed as defined in *ViewBoxParams*, then as defined in *TransformParams*).

Spaces are allowed in fragment specifications. Commas are used to separate numeric values within an SVG view specification (e.g., #svgView(viewBox(0,0,200,200))) and semicolons are used to separate attributes (e.g., #svgView(viewBox(0,0,200,200);preserveAspectRatio(none))).

Fragment identifiers may be url-escaped according to the rules defined in CSS Object Model (CSSOM) specification. For example semicolons can be escaped as %3B to allow animating a (semi-colon separated) list of URLs because otherwise the semicolon would be interpreted as a list separator.

The four types of *SVGViewAttribute* may occur in any order, but each type may only occur at most one time in a correctly formed *SVGViewSpec*.

When a source document performs a link into an SVG document, for example via an HTML anchor element ([[HTML](refs.md#ref-html)]; i.e., <a href=...> element in HTML) or an XLink specification [[xlink](refs.md#ref-xlink)], then the SVG fragment identifier specifies the initial view into the SVG document, as follows:

- If no SVG fragment identifier is provided (e.g, the specified URL did not contain a "#" character, such as MyDrawing.svg), then the initial view into the SVG document is established using the view specification attributes (i.e., ‘[viewBox](coords.md#ViewBoxAttribute)’, etc.) on the [outermost svg element](struct.md#TermOutermostSVGElement).
- If the SVG fragment identifier addresses a space segment (e.g., MyDrawing.svg#xywh=0,0,100,100),then the initial view into the SVG document is established using the view specification attributes on the [outermost svg element](struct.md#TermOutermostSVGElement) where the 'viewBox' is overriden by the x, y, width and height values provided by the fragment identifier.
- Media fragments can be specified as "pixel:" (default) and "percent:". In the latter case the resulting 'viewBox' transformation is calculated against referenced SVG resolved size (width and height). When the host document cannot resolve the width and height of the SVG document, the used values for width and height should be calculated according to CSS rules for calculating width and ratio for inline replaced elements.
- If the SVG fragment identifier addresses a time segment (e.g., MyDrawing.svg#t=10),then the initial view into the SVG document is established as if no fragment identifier was provided. The rendering of the SVG Document shall be as if the setCurrentTime method on the SVG Document element had been called with the begin time value from the fragment identifier. Additionally, if an end time value is provided in the fragment identifier, the effect is equivalent to calling the pauseAnimations method on the SVG Document when the document time reaches the end time of the fragment identifier.
- If the SVG fragment identifier addresses a ‘[view](#ViewElement)’ element within an SVG document (e.g., MyDrawing.svg#MyView) then the root ‘[svg](struct.md#SVGElement)’ element is displayed in the SVG viewport. Any view specification attributes included on the given ‘[view](#ViewElement)’ element override the corresponding view specification attributes on the root ‘[svg](struct.md#SVGElement)’ element.
- If the SVG fragment identifier addresses specific SVG view (e.g., MyDrawing.svg#svgView(viewBox(0,200,1000,1000))), then the document fragment defined by the root ‘[svg](struct.md#SVGElement)’ element is displayed in the SVG viewport using the SVG view specification provided by the SVG fragment identifier. Parameters of the svgView specification override the parameters defined on the root ‘[svg](struct.md#SVGElement)’ element of the referenced document. Unspecified parameters of the svgView specification don't reset the values defined on the root ‘[svg](struct.md#SVGElement)’ element of the referenced document.
- If the SVG fragment identifier addresses a combination of the above non-time related identifiers with a time-related identifier (i.e. a timesegment), the behavior of each identifier is applied.

### <a id="ViewElement"></a>16.3.3. Predefined views: the ‘view’ element

The ‘view’ element is defined as follows:

‘*<a id="elementdef-view"></a>view*’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’

[script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- ‘[viewBox](coords.md#ViewBoxAttribute)’
- ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’

**DOM Interfaces:**
:

- [SVGViewElement](#InterfaceSVGViewElement)

## <a id="DOMInterfaces"></a>16.4. DOM interfaces

### <a id="InterfaceSVGAElement"></a>16.4.1. Interface SVGAElement

An [SVGElement](types.md#InterfaceSVGElement) object represents an ‘[a](#AElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGAElement : SVGGraphicsElement {
  [SameObject] readonly attribute SVGAnimatedString target;
  attribute DOMString download;
  attribute USVString ping;
  attribute DOMString rel;
  [SameObject, PutForwards=value] readonly attribute DOMTokenList relList;
  attribute DOMString hreflang;
  attribute DOMString type;

  attribute DOMString referrerPolicy;
};

SVGAElement includes SVGURIReference;
SVGAElement includes HTMLHyperlinkElementUtils;
```

The **<a id="__svg__SVGAElement__target"></a>target**, **<a id="__svg__SVGAElement__download"></a>download**, **<a id="__svg__SVGAElement__ping"></a>ping**, **<a id="__svg__SVGAElement__rel"></a>rel**, **<a id="__svg__SVGAElement__hreflang"></a>hreflang**, **<a id="__svg__SVGAElement__type"></a>type**, IDL attributes [reflect](types.md#TermReflect) the content attributes of the same name.

The **<a id="__svg__SVGAElement__relList"></a>relList** IDL attribute [reflects](types.md#TermReflect) the ‘[rel](#AElementRelAttribute)’ content attribute.

The **<a id="__svg__SVGAElement__referrerPolicy"></a>referrerPolicy** IDL attribute [reflects](types.md#TermReflect) the ‘[referrerpolicy](#AElementReferrerpolicyAttribute)’ content attribute, limited to only known values.

### <a id="InterfaceSVGViewElement"></a>16.4.2. Interface SVGViewElement

An [SVGViewElement](#InterfaceSVGViewElement) object represents a ‘[view](#ViewElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGViewElement : SVGElement {};

SVGViewElement includes SVGFitToViewBox;
```
