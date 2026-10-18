<!-- https://www.w3.org/TR/SVG11/conform.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](implnote.md) ⋅ [Next](access.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# Appendix G: Conformance Criteria

**This appendix is normative.**

## <a id="Introduction"></a>G.1 Introduction

In order to ensure that SVG-family documents are maximally portable among SVG-family user agents, this specification rigidly defines conformance requirements for both, as well as for SVG-family document types. While the conformance definitions can be found in this appendix, they necessarily reference normative text within this document and within other related specifications. It is only possible to fully comprehend the conformance requirements of SVG through a complete reading of all normative references.

## <a id="ConformingSVGDocuments"></a>G.2 Conforming SVG Document Fragments

An SVG document fragment is a *Conforming SVG Document Fragment* if it adheres to the specification described in this document ([Scalable Vector Graphics (SVG) Specification](index.md)) and also:

- is XML well-formed ([[XML10](refs.md#ref-XML10)], section 2.1),
- conforms to the Namespaces in XML specification [[XML-NS](refs.md#ref-XML-NS)],
- any CSS stylesheets conform to the core grammar of Cascading Style Sheets, level 2 [[CSS2](refs.md#ref-CSS2)],
- any `<?xml-stylesheet?>` processing instruction conforms to Associating stylesheets with XML documents [[XML-SS](refs.md#ref-XML-SS)],
- and the document fragment is determined to be valid as follows:
  1. Let *E* be the encoding of the XML document in which the SVG document fragment resides.
  2. Let *V* be the XML version of the document in which the SVG document fragment resides.
  3. Let *D* be an XML document constructed by concatenating:
    - an XML declaration identifying version *V* and encoding *E* (that is, `<?xml version="*V*" encoding="*E*"?>`),
    - the DOCTYPE declaration `<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" SYSTEM "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">`, and
    - the SVG document fragment with any entities expanded.
  4. Remove from *D* any subtree rooted by an element that is not in the SVG namespace.
  5. Remove from *D* any attribute that is in a namespace that is not the XLink namespace or the Namespaces in XML namespace.
  6. Remove the prefix from the name of any element in *D* that uses one.
  7. Let *A* be the set of all attributes in *D* that are in the XLink namespace.
  8. Set the attributes xmlns="http://www.w3.org/2000/svg" and xmlns:xlink="http://www.w3.org/1999/xlink" on *D*'s document element and remove any other attributes in the Namespaces in XML namespace from *D*.
  9. Change the prefix of every attribute *A* to be `xlink`.
  10. The document fragment is valid if *D* is a valid XML document ([[XML10](refs.md#ref-XML10)], section 2.8).

SVG document fragments can be included within parent XML documents using the XML namespace facilities described in Namespaces in XML [[XML-XS](refs.md#ref-XML-NS)]. Note, however, that since a Conforming SVG Document Fragment must have an [‘svg’](struct.md#SVGElement) element as its root, the use of an individual non-[‘svg’](struct.md#SVGElement) element from the SVG namespace is disallowed. Thus, the SVG part of the following document is *not* conforming:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE SomeParentXMLGrammar PUBLIC "-//SomeParent" "http://SomeParentXMLGrammar.dtd">
<ParentXML>
  <!-- Elements from ParentXML go here -->
  <!-- The following is not conforming -->
  <z:rect xmlns:z="http://www.w3.org/2000/svg"
          x="0" y="0" width="10" height="10" />
  <!-- More elements from ParentXML go here -->
</ParentXML>
```

Instead, for the SVG part to become a Conforming SVG Document Fragment, the file could be modified as follows:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE SomeParentXMLGrammar PUBLIC "-//SomeParent" "http://SomeParentXMLGrammar.dtd">
<ParentXML>
  <!-- Elements from ParentXML go here -->
  <!-- The following is conforming -->
  <z:svg xmlns:z="http://www.w3.org/2000/svg"
         width="100px" height="100px">
    <z:rect x="0" y="0" width="10" height="10"/>
  </z:svg>
  <!-- More elements from ParentXML go here -->
</ParentXML>
```

The SVG language or these conformance criteria provide no designated size limits on any aspect of SVG content. There are no maximum values on the number of elements, the amount of character data, or the number of characters in attribute values.

## <a id="ConformingSVGStandAloneFiles"></a>G.3 Conforming SVG Stand-Alone Files

A file is a *Conforming SVG Stand-Alone File* if:

- it is an XML document,
- its root element is an [‘svg’](struct.md#SVGElement) element, and
- the SVG document fragment rooted at the document element is a [Conforming SVG Document Fragment](#ConformingSVGDocuments).

## <a id="ConformingSVGGenerators"></a>G.4 Conforming SVG Generators

A *Conforming SVG Generator* is a program which:

- always creates [Conforming SVG Document Fragments](#ConformingSVGDocuments) and/or [Conforming SVG Stand-Alone Files](#ConformingSVGStandAloneFiles).
- does not create non-conforming SVG document fragments of the above types.

Additionally, an authoring tool which is a Conforming SVG Generator conforms to all of the Priority 1 accessibility guidelines from the document Authoring Tool Accessibility Guidelines 1.0 [[ATAG](refs.md#ref-ATAG)] that are relevant to generators of SVG content. (Priorities 2 and 3 are encouraged but not required for conformance.)

SVG generators are encouraged to follow W3C developments in the area of internationalization. Of particular interest is the *W3C Character Model* and the concept of *Webwide Early Uniform Normalization*, which promises to enhance the interchangability of Unicode character data across users and applications. Future versions of the SVG specification are likely to require support of the *W3C Character Model* in Conforming SVG Generators.

## <a id="ConformingSVGServers"></a>G.5 Conforming SVG Servers

Conforming SVG Servers must meet all the requirements of a Conforming SVG Generator. In addition, Conforming SVG Servers using HTTP or other protocols that use Internet Media types must serve SVG stand-alone files with the media type `"image/svg+xml"`.

Also, if the SVG file is compressed with gzip or deflate, Conforming SVG Servers must indicate this with the appropriate header, according to what the protocol supports. Specifically, for content compressed by the server immediately prior to transfer, the server must use the "Transfer-Encoding: gzip" or "Transfer-Encoding: deflate" headers as appropriate, and for content stored in a compressed format on the server (e.g. with the file extension "svgz"), the server must use the "Content-Encoding: gzip" or "Content-Encoding: deflate" headers as appropriate.

*Note:* Compression of stored *content* (the "entity," in HTTP terms) is distinct from automatic compression of the *message body*, as defined in HTTP/1.1 TE/ Transfer Encoding ([[RFC2616](refs.md#ref-RFC2616)], sections 14.39 and 14.41).

## <a id="ConformingSVGDOMSubtree"></a>G.6 Conforming SVG DOM Subtree

A DOM subtree rooted at a given element is a *Conforming SVG DOM Subtree* if, once serialized to XML, is a [Conforming SVG Document Fragment](#ConformingSVGDocuments). If the DOM subtree cannot be serialized to XML, such as when a Comment node's data contains the substring "--", then the subtree is not a Conforming SVG DOM Subtree.

## <a id="ConformingSVGInterpreters"></a>G.7 Conforming SVG Interpreters

An SVG interpreter is a program which can parse and process SVG document fragments. Examples of SVG interpreters are server-side transcoding tools (e.g., a tool which converts SVG content into modified SVG content) or analysis tools (e.g., a tool which extracts the text content from SVG content). An [SVG viewer](#ConformingSVGViewers) also satisfies the requirements of an SVG interpreter in that it can parse and process SVG document fragments, where processing consists of rendering the SVG content to the target medium.

In a *Conforming SVG Interpreter*, the XML parser must be able to parse and process all XML constructs defined within XML 1.0 [[XML10](refs.md#ref-XML10)] and Namespaces in XML [[XML-NS](refs.md#ref-XML-NS)].

There are two sub-categories of *Conforming SVG Interpreters*:

- *Conforming Static SVG Interpreters* must be able to parse and process the static language features of SVG that correspond to the feature string "http://www.w3.org/TR/SVG11/feature#SVG-static" (see [Feature strings](https://www.w3.org/TR/SVG11/svgdom.html#FeatureStrings)).
- In addition to the requirements for the static category, *Conforming Dynamic SVG Interpreters* must be able to parse and process the language features of SVG that correspond to the feature string "http://www.w3.org/TR/SVG11/feature#SVG-dynamic" (see [Feature strings](https://www.w3.org/TR/SVG11/svgdom.html#FeatureStrings)) and which support all of the required features in the [SVG DOM](https://www.w3.org/TR/SVG11/svgdom.html) described in this specification.

A Conforming SVG Interpreter must parse any SVG document correctly. It is not required to interpret the semantics of all features correctly.

Note: A transcoder from SVG into another graphics representation, such as an SVG-to-raster transcoder, represents a viewer, and thus viewer conformance criteria apply. (See [Conforming SVG Viewers](#ConformingSVGViewers).)

## <a id="ConformingSVGViewers"></a>G.8 Conforming SVG Viewers

An SVG viewer is a program which can parse and process an SVG document fragment and render the contents of the document onto some sort of output medium such as a display or printer; thus, an *SVG Viewer* is also an *SVG Interpreter*.

There are two sub-categories of *Conforming SVG Viewers*:

- *Conforming Static SVG Viewers* support the static language features of SVG that correspond to the feature string "http://www.w3.org/TR/SVG11/feature#SVG-static" (see [Feature strings](https://www.w3.org/TR/SVG11/svgdom.html#FeatureStrings)). This category often corresponds to platforms and environments which only render static documents, such as printers.
- *Conforming Dynamic SVG Viewers* support the language features of SVG that correspond to the feature string "http://www.w3.org/TR/SVG11/feature#SVG-dynamic" (see [Feature strings](https://www.w3.org/TR/SVG11/svgdom.html#FeatureStrings)). This category often applies to platforms and environments such as common Web browsers which support user interaction and dynamic document content (i.e., documents whose content can change over time). (User interaction includes support for hyperlinking, events [e.g., mouse clicks], text selection, zooming and panning [see [Interactivity](interact.md)]. Dynamic document content can be achieved via [declarative animation](animate.md) or by scripts modifying the [SVG DOM](https://www.w3.org/TR/SVG11/svgdom.html).)

Specific criteria that apply to both *Conforming Static SVG Viewers* and *Conforming Dynamic SVG Viewers*:

- The program must also be a [Conforming SVG Interpreter](#ConformingSVGInterpreters),
- For interactive user environments, facilities must exist for zooming and panning of stand-alone SVG documents or SVG document fragments embedded within parent XML documents.
- In environments that have appropriate user interaction facilities, the viewer must support the ability to activate hyperlinks.
- If printing devices are supported, SVG content must be printable at printer resolutions with the same graphics features available as required for display (e.g., the specified colors must be rendered on color printers).
- On systems where this information is available, the parent environment must provide the viewer with information about physical device resolution. In situations where this information is impossible to determine, the parent environment shall pass a reasonable value for device resolution which tends to approximate most common target devices.
- The viewer must support JPEG and PNG image formats [[JPEG](refs.md#ref-JPEG)] [[PNG](refs.md#ref-PNG)].
- Resampling of image data must be consistent with the specification of property [‘image-rendering’](painting.md#ImageRenderingProperty).
- The viewer must support alpha channel blending of the image of the SVG content onto the target canvas.
- SVG implementations must correctly support gzip-encoded [[RFC1952](refs.md#ref-RFC1952)] and deflate-encoded [[RFC1951](refs.md#ref-RFC1951)] data streams, for any content type (including SVG, script files, images). SVG implementations that support HTTP must support these encodings according to the HTTP 1.1 specification [[RFC2616](refs.md#ref-RFC2616)]; in particular, the client must specify with an "Accept-Encoding:" request header [HTTP-ACCEPT-ENCODING] those encodings that it accepts, including at minimum gzip and deflate, and then decompress any gzip-encoded and deflate-encoded data streams that are downloaded from the server. When an SVG viewer retrieves compressed content (e.g., an .svgz file) over HTTP, if the "Content-Encoding" and "Transfer-Encoding" response headers are missing or specify a value that does not match the compression method that has been applied to the content, then the SVG viewer must not render the content and must treat the document as being [in error](implnote.md#ErrorProcessing).
- The viewer must support base64 encoded content using the "data:" URL scheme [[RFC2397](refs.md#ref-RFC2397)] wherever URI referencing of whole documents (such as raster images, SVG documents, fonts and color profiles) is permitted within SVG content. (Note: fragments of SVG content which do not constitute an entire SVG document are not available using the "data:" URL scheme.)
- The viewer must support the following W3C Recommendations with regard to SVG content:
  - complete support for the XML 1.0 specification [[XML10](refs.md#ref-XML10)].
  - complete support for inclusion of non-SVG namespaces within SVG content as defined in Namespaces in XML [[XML-NS](refs.md#ref-XML-NS)]. (Note that data from non-SVG namespaces are included in the DOM but are otherwise ignored.)
- All visual rendering must be accurate to within one device pixel (px unit) to the mathematically correct result at the initial 1:1 zoom ratio. It is suggested that viewers attempt to keep a high degree of accuracy when zooming.
- On systems which support accurate sRGB [[SRGB](refs.md#ref-SRGB)] color, all sRGB color computations and all resulting color values must be accurate to within one sRGB color component value, where sRGB color component values range from 0 to 255.

Although anti-aliasing support is not a strict requirement for a Conforming SVG Viewer, it is highly recommended for display devices. Lack of anti-aliasing support will generally result in poor results on display devices.

Specific criteria that apply to only *Conforming Dynamic SVG Viewers*:

- In Web browser environments, the viewer must have the ability to search and select text strings within SVG content.
- If display devices are supported, the viewer must have the ability to select and copy text from SVG content to the system clipboard.
- The viewer must have complete support for an ECMAScript binding of the [SVG Document Object Model](https://www.w3.org/TR/SVG11/svgdom.html) [[ECMA-262](refs.md#ref-ECMA-262)].

The Web Accessibility Initiative is defining User Agent Accessibility Guidelines 1.0 [[UAAG](refs.md#ref-UAAG)]. Viewers are encouraged to conform to the Priority 1 accessibility guidelines defined in this document, and preferably also Priorities 2 and 3. Once the guidelines are completed, a future version of this specification is likely to require conformance to the Priority 1 guidelines in Conforming SVG Viewers.

<a id="ConformingHighQualitySVGViewers"></a>A higher order concept is that of a *Conforming High-Quality SVG Viewer*, with sub-categories *Conforming High-Quality Static SVG Viewer* and *Conforming High-Quality Dynamic SVG Viewer*.

Both a *Conforming High-Quality Static SVG Viewer* and a *Conforming High-Quality Dynamic SVG Viewer* must support the following additional features:

- Professional-quality results with good processing and rendering performance and smooth, flicker-free animations.
- On low-resolution devices such as display devices at 150dpi or less, support for smooth edges on lines, curves and text. (Smoothing is often accomplished using anti-aliasing techniques.)
- Color management via ICC profile support (i.e., the ability to support colors defined using ICC profiles).
- Resampling of image data must conform to the requirements for Conforming High-Quality SVG Viewers as specified in the description of property [‘image-rendering’](painting.md#ImageRenderingProperty).
- At least double-precision floating point computation on coordinate system transformation numerical calculations.

A *Conforming High-Quality Dynamic SVG Viewer* must support the following additional features:

- Progressive rendering and animation effects (i.e., the start of the document will start appearing and animations will start running in parallel with downloading the rest of the document).
- Restricted screen updates (i.e., only required areas of the display are updated in response to redraw events).
- Background downloading of images and fonts retrieved from a Web server, with updating of the display once the downloads are complete.

A *Conforming SVG Viewer* must be able to apply styling properties to SVG content using [presentation attributes](styling.md#AlternativesForStyling).

If the user agent supports Cascading Style Sheets, level 2 [[CSS2](refs.md#ref-CSS2)], a *Conforming SVG Viewer* must support CSS styling of SVG content and must support all features from CSS2 that are described in this specification as applying to SVG (see [properties shared with CSS and XSL](styling.md#PropertiesFromCSS2), [Styling with CSS](styling.md#StylingWithCSS) and [Facilities from CSS and XSL used by SVG](styling.md#SVGUseOfCSS)). The supported features from CSS2 must be implemented in accordance with the conformance definitions from the CSS2 specification ([[CSS2](refs.md#ref-CSS2)], section 3.2).

If the user agent includes an HTML or XHTML viewing capability or can apply CSS/XSL styling properties to XML documents, then a *Conforming SVG Viewer* must support resources of MIME type "image/svg+xml" wherever raster image external resources can be used, such as in the HTML or XHTML ‘img’ element and in CSS/XSL properties that can refer to raster image resources (e.g., ‘background-image’).

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](implnote.md) ⋅ [Next](access.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
