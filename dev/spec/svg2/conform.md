<!-- https://svgwg.org/svg2-draft/conform.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 2: Conformance Criteria

## <a id="conformance-overview"></a>2.1. Overview

Graphics defined with SVG have many different applications. As a result, not all software that uses SVG will have the same features. Conformance to the SVG specification is therefore not a binary matter; software may be conforming within a restricted feature set.

Furthermore, SVG is designed to be integrated into other types of documents; depending on the type of integration, only a limited feature-set may be appropriate. There are various ways that an SVG document fragment can be referenced by or included in other documents and thereby be processed by a user agent. SVG documents can also be viewed directly, as the primary document. Each different method by which an SVG document fragment can be used implies a certain set of requirements on how the SVG document fragment must be processed.

This chapter therefore defines a number of *<a id="processing-mode"></a>processing modes* that encompass the different combinations of features which may be enabled or disabled in the document. In addition, it specifies normative requirements for which processing mode must be used when SVG documents reference or embed other SVG documents. The same set of processing modes may be used by reference in other specifications to describe how SVG documents should be processed.

This document does not place normative requirements on other specifications that can reference or include SVG documents, such as HTML and various CSS specifications. The intention is for these other specifications to normatively point to the appropriate processing mode from this document.

This chapter also outlines specific conformance requirements for [different types of SVG content](#DocumentConformanceClasses), and [different classes of software](#SoftwareConformanceClasses) that use or create SVG.

## <a id="processing-modes"></a>2.2. Processing modes

This section defines a standard set of [processing modes](#processing-modes) for SVG documents. Each processing mode specifies whether certain high level SVG features are enabled.

### <a id="features"></a>2.2.1. Features

The features that can be enabled or disabled depending on the processing mode are as follows:

**declarative animation**
:

Declarative animation includes both the animation elements in SVG – ‘animate’, ‘animateMotion’, ‘animateTransform’ and ‘set’ – and CSS Transitions and Animations (see the [Animation appendix](animate.md) for details). When declarative animations are disabled in an SVG document, any animation elements or CSS Transitions or Animations must not be applied or run.

This is not the same as pausing the document's animated state at 0s document time; if an animation is defined to begin at 0s, it still will not be applied.

**references to external resources**
:

References to [external resources](linking.md#TermExternalReference) are URLs references or network access requests made by markup, style properties, script or other Web platform features used in the document, except for:

- [same-document URL references](linking.md#TermSameDocumentURL), as defined in the [Linking chapter](linking.md)
- [data URL references](linking.md#TermDataURL), as defined by the "data" URL scheme [[rfc2397](refs.md#ref-rfc2397)]

When external references are disabled in an SVG document, any attempt to fetch a document through an external reference must instead be treated as if a network error occurred and no data was received.

When external references are enabled, user agents that support external file requests from the Internet must adhere to the restrictions on cross-origin resource fetching, as outlined in [the Linking chapter](linking.md#processingURL-fetch).

**script execution**
:

Script execution is the execution of any SVG ‘[script](interact.md#ScriptElement)’ elements, script found in [event attributes](interact.md#TermEventAttribute) (such as ‘[onclick](interact.md#EventAttributes)’ on SVG elements), or any other script defined by other Web platform features used in the document, such as any HTML ‘[script](interact.md#ScriptElement)’ elements. When script execution is disabled in an SVG document, no script in the document must be run.

**interaction**
:

Interaction refers to the delivery of DOM Events or the invocation of any user agent specific UI behaviors such as text selection, focus changing, link traversal, or animation or transition triggering that is done in response to user input such as mouse or keyboard activity. When interaction is disabled in an SVG document, any user input events that would be targetted at the document or any elements within the document must have no effect.

### <a id="dynamic-interactive-mode"></a>2.2.2. Dynamic interactive mode

This [processing mode](#processing-modes) imposes no restrictions on any feature of the SVG language.

| Dynamic Interactive Features |  |
| --- | --- |
| script execution | yes |
| external references | yes |
| declarative animation | yes |
| interactivity | yes |

### <a id="animated-mode"></a>2.2.3. Animated mode

This [processing mode](#processing-modes) is intended for circumstances where an SVG document is to be used as an animated image that is allowed to resolve external references, but which is not intended to be used as an interactive document.

| Animated Features |  |
| --- | --- |
| script execution | no |
| external references | yes |
| declarative animation | yes |
| interactivity | no |

### <a id="secure-animated-mode"></a>2.2.4. Secure animated mode

This [processing mode](#processing-modes) is intended for circumstances where an SVG document is to be used as an animated image that is not allowed to resolve external references, and which is not intended to be used as an interactive document. This mode might be used where image support has traditionally been limited to raster images (such as JPEG, PNG and GIF).

| Secure Animated Features |  |
| --- | --- |
| script execution | no |
| external references | no |
| declarative animation | yes |
| interactivity | no |

### <a id="static-mode"></a>2.2.5. Static mode

This [processing mode](#processing-modes) is intended for circumstances where an SVG document is to be used as a non-animated image that is allowed to resolve external references, but which is not intended to be used as an interactive document. For example, an SVG viewer that processes graphics for inclusion in print documents would likely use static mode.

| Static Features |  |
| --- | --- |
| script execution | no |
| external references | yes |
| declarative animation | no |
| interactivity | no |

### <a id="secure-static-mode"></a>2.2.6. Secure static mode

This [processing mode](#processing-modes) is intended for circumstances where an SVG document is to be used as a non-animated image that is not allowed to resolve external references, and which is not intended to be used as an interactive document. This mode might be used where image support has traditionally been limited to non-animated raster images (such as JPEG and PNG.)

| Secure Static Features |  |
| --- | --- |
| script execution | no |
| external references | no |
| declarative animation | no |
| interactivity | no |

## <a id="referencing-modes"></a>2.3. Processing modes for SVG sub-resource documents

When an SVG document is viewed directly, it is expected to be displayed using the most comprehensive [processing mode](#processing-modes) supported by the user agent. However, when an SVG is processed as a sub-resource or embedded document, the following restrictions must apply:

**<a id="image-document-mode"></a>‘[image](embedded.md#ImageElement)’ references**
:

An SVG embedded within an ‘[image](embedded.md#ImageElement)’ element must be processed in [secure animated mode](#secure-animated-mode) if the embedding document supports [declarative animation](#processing-modes), or in [secure static mode](#secure-static-mode) otherwise.

The same processing modes are expected to be used for other cases where SVG is used in place of a raster image, such as an HTML ‘img’ element or in any CSS property that takes an <image> data type. This is consistent with HTML's requirement that image sources must reference "a non-interactive, optionally animated, image resource that is neither paged nor scripted" [[HTML](refs.md#ref-html)]

**<a id="template-document-mode"></a>‘[use](struct.md#UseElement)’ element and other `'href'` references**
:

When SVG documents are loaded through ‘[use](struct.md#UseElement)’ element references or [paint server element](painting.md#TermPaintServerElement) cross-references they must be processed in [secure static mode](#secure-static-mode).

Note that animations do not run while processing the sub-resource document, for both performance reasons and because there is currently no context defined for resource documents to reference their timeline against. However, when elements from a sub-resource document are cloned into the current document because of a ‘[use](struct.md#UseElement)’ element reference or paint-server cross-reference, the cloned [element instances](struct.md#TermElementInstance) may be animated in the current document's timeline, as described in [Animations in use-element shadow trees](struct.md#UseAnimations), and may trigger the loading of additional sub-resource files.

**<a id="resource-document-mode"></a>Graphical effects references**
:

When SVG documents are loaded through any style property references that target specific elements in the document (as opposed to SVG as an image format), they must be processed in [secure static mode](#secure-static-mode).

Note that animations do not run in sub-resource documents, for both performance reasons and because there is currently no context defined for resource documents to reference their timeline against.

Some style properties may reference either specific elements or entire image files; the processing mode is more restrictive in the first case. For example, a reference to an SVG ‘[mask](../css-masking-1.md#MaskElement)’ element will not be animated, but an entire SVG file used as an image mask can be.

**<a id="font-document-mode"></a>SVG in fonts**
:

When SVG files are processed as part of a font reference, they must use the [secure animated mode](#secure-animated-mode) if animated glyphs are supported, or [secure static mode](#secure-static-mode) otherwise.

These restrictions are included in the OpenType specification for processing documents from the "SVG" table. OpenType also applies additional restrictions, in the form of a [user agent style sheet](styling.md#UAStyleSheet) that prevents rendering of text and foreign objects [[OPENTYPE](refs.md#ref-opentype)].

SVG document fragments that are included inline in a host document must use a [processing mode](#processing-modes) that matches that of the host document. SVG document fragments included as children of an SVG ‘[foreignObject](embedded.md#ForeignObjectElement)’ element must use the [processing mode](#processing-modes) of the surrounding SVG document; non-SVG foreign content must be processed with equivalent restrictions.

For example, if an SVG document is being used in [secure animated mode](#secure-animated-mode) due to being referenced by an HTML ‘img’ or SVG ‘[image](embedded.md#ImageElement)’ element, then any content within a ‘[foreignObject](embedded.md#ForeignObjectElement)’ element must have scripts, interactivity, and external file references disabled, but should have declarative animation enabled.

### <a id="examples"></a>2.3.1. Examples

Below are various methods of embedding SVG in an HTML page by reference, along with the expected processing mode and allowed features for each.

Each cell in the "Live Example" row should display a yellow smiley face. In each example below, clicking on the eyes tests link traversal, and clicking on the face tests declarative interactivity and script execution. The link should replace the image with a blue square (clicking it will revert it to the original image). The declarative interactivity uses the ‘set’ element to change the face from shades of yellow to shades of green, and uses CSS pseudoclasses to add a stroke to the interactive elements. The script should fill in the smile. Time-based (as opposed to interactivity-based) declarative animation is supported if the left eye is winking (using the ‘animate’ element) and if the eyes are dark blue with regular flashes of light blue (using CSS keyframe animation).

The expected processing modes and features outlined here are subject to any future changes in the corresponding HTML or CSS specification.

| Embedding method | object without sandboxing | img | CSS background |  |
| --- | --- | --- | --- | --- |
| Expected processing mode | dynamic interactive | dynamic interactive, with restrictions | secure animated | secure animated |
| Declarative, time-based animation (winking left eye, color-change in both eyes) | allowed | allowed | allowed | allowed |
| Declarative, interactive animation and style changes (face color changes when clicked, face/eyes outlined when hovered or focused) | allowed | allowed | disabled | disabled |
| Link navigation within the same browsing context, to the same domain (image changes when clicking eyes) | allowed | allowed | disabled | disabled |
| Scripted interaction (smile widens when clicking face) | allowed | disabled (because of sandboxing) | disabled | disabled |
| Live example |  |  | [image: smiley face, as an image]<a id="js-embed-img"></a> |  |

## <a id="DocumentConformanceClasses"></a>2.4. Document Conformance Classes

SVG is defined in terms of a document object model (DOM), rather than a particular file format or document type. For SVG content, therefore, conformance with this specification is defined by whether the content is or can generate a conforming DOM. Additional conformance classes depend on whether the content is also valid and well-formed XML [[xml](refs.md#ref-xml)].

### <a id="ConformingSVGDOMSubtrees"></a>2.4.1. Conforming SVG DOM Subtrees

A DOM node tree or subtree rooted at a given element is a *conforming SVG DOM subtree* if it forms a [SVG document fragment](struct.md#TermSVGDocumentFragment) that adheres to the specification described in this document ([Scalable Vector Graphics (SVG) Specification](index.md)). Specifically, it:

- is rooted by an ‘[svg](struct.md#SVGElement)’ element in the [SVG namespace](struct.md#Namespace),
- conforms to the content model and attributes rules for the elements defined in this document ([Scalable Vector Graphics (SVG) Specification](index.md)), and
- conforms to the content model and attributes rules defined by other specifications for any elements in the [SVG namespace](struct.md#Namespace) defined by those specifications (including: [[filter-effects-1](refs.md#ref-filter-effects-1)], [[css-masking-1](refs.md#ref-css-masking-1)], [[svg-animation](refs.md#ref-svg-animation)]).

SVG document fragments can be included within parent XML documents using the XML namespace facilities described in Namespaces in XML [[xml-names](refs.md#ref-xml-names)]. Note, however, that since a conforming SVG DOM subtree must have an ‘[svg](struct.md#SVGElement)’ element as its root, the use of an individual non-‘[svg](struct.md#SVGElement)’ element from the SVG namespace is disallowed. Thus, the SVG part of the following document is *not* conforming:

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

Instead, for the SVG part to become a [conforming SVG DOM subtree](#ConformingSVGDOMSubtrees), the file could be modified as follows:

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

The SVG language and these conformance criteria provide no designated size limits on any aspect of SVG content. There are no maximum values on the number of elements, the amount of character data, or the number of characters in attribute values.

### <a id="ConformingSVGFragments"></a>2.4.2. Conforming SVG Markup Fragments

A document or part of a document is a *conforming SVG markup fragment* if it can be parsed without error (other than network errors) by the appropriate parser for the document MIME type to form a [conforming SVG DOM subtree](#ConformingSVGDOMSubtrees), and in addition if:

- any CSS stylesheets included in the document conform to the core grammar of Cascading Style Sheets, level 2 revision 1 [[CSS2](refs.md#ref-css2)].

### <a id="ConformingSVGXMLFragments"></a>2.4.3. Conforming XML-Compatible SVG Markup Fragments

A [conforming SVG markup fragment](#ConformingSVGFragments) is also a *conforming XML-compatible SVG markup fragment* if it:

- meets all XML well-formedness constraints ([[xml](refs.md#ref-xml)]),
- conforms to the Namespaces in XML specification [[xml-names](refs.md#ref-xml-names)],
- all ‘[id](struct.md#IDAttribute)’ attributes are valid XML IDs ([[xml](refs.md#ref-xml)], section 3.3.1), and
- any `<?xml-stylesheet?>` processing instruction conforms to Associating stylesheets with XML documents [[xml-stylesheet](refs.md#ref-xml-stylesheet)].

### <a id="ConformingSVGXMLDOMSubtrees"></a>2.4.4. Conforming XML-Compatible SVG DOM Subtrees

A DOM node tree or subtree rooted at a given element is an *conforming XML-compatible SVG DOM subtree* if, once serialized to XML, it could form a [conforming XML-compatible SVG markup fragment](#ConformingSVGXMLFragments).

If the DOM subtree cannot be serialized to conforming XML without altering it, such as when an ‘[id](struct.md#IDAttribute)’ value is not a valid XML name, or when a Comment node's data contains the substring "--", then the subtree is not a conforming XML-compatible SVG DOM subtree.

### <a id="ConformingSVGStandAloneFiles"></a>2.4.5. Conforming SVG Stand-Alone Files

A document is a *conforming SVG stand-alone file* if:

- it is a well-formed XML document,
- its root element is an ‘[svg](struct.md#SVGElement)’ element,
- the SVG document fragment rooted at the document element is a [conforming XML-compatible SVG markup fragment](#ConformingSVGXMLFragments), and
- any other [SVG document fragments](struct.md#TermSVGDocumentFragment) within the document (such as those within a ‘[foreignObject](embedded.md#ForeignObjectElement)’) form a [conforming XML-compatible SVG markup fragment](#ConformingSVGXMLFragments).

### <a id="ErrorProcessing"></a>2.4.6. Error processing

There are various scenarios where an SVG document fragment is technically *in error*:

- The document or DOM subtree is not-conforming for its document type, as described in the previous sections.
- Other situations that are described as being *in error* in this specification, such as incorrect attribute values.

A dynamic document can go in and out of error over time. For example, document changes from the [SVG DOM](types.md#SVGDOMOverview) or from animation can cause a document to become *in error* and a further change can cause the document to become correct again.

User agents must use the following error processing rules whenever a document is in error, unless other sections of this specification define more specific rules for handling the particular error type:

- The document rendering shall continue after encountering element which has an error. The element or its part that is in error won't be rendered.
- If the user agent has access to an error reporting capability such as status bar or console, it is recommended that the user agent provide whatever additional detail it can to enable the user or developer to quickly find the source of the error. For example, the user agent might provide an error message along with a line number and character number at which the error was encountered.

Because of situations where a block of scripting changes might cause a given SVG document fragment to go into and out of error, the user agent should only apply error processing at times when document presentation (e.g., rendering to the display device) is updated.

## <a id="SoftwareConformanceClasses"></a>2.5. Software Conformance Classes

For software, the requirements for conformance depend on the category of program:

***<a id="TermSVGGenerator"></a>SVG generators***
: Any software that creates or makes available SVG content, either as markup or as a DOM (as is the case with client-side JavaScript libraries).
***<a id="TermSVGAuthoringTool"></a>SVG authoring tools***
: Any software that provides an interface for human content creators to manipulate graphics or code that will be used to generate SVG. SVG authoring tools are implicitly also [SVG generators](#TermSVGGenerator).
***<a id="TermSVGServer"></a>SVG servers***
: Any network or file server that makes available SVG content in response to requests from other software. SVG servers are implicitly also [SVG generators](#TermSVGGenerator).
***<a id="TermSVGInterpreter"></a>SVG interpreters***
: Any software that parses or processes SVG documents or markup fragments. An SVG interpreter is an [SVG user agent](#TermUserAgent) for the purpose of any sections of this specification that relate to the parsing or processing steps undertaken by the interpreter.
***<a id="TermSVGViewer"></a>SVG viewers***
: Any software that creates a rendered graphical representation after parsing or processing an SVG document or SVG markup fragment. SVG viewers are implicitly also [SVG interpreters](#TermSVGInterpreter). An SVG viewer is always an [SVG user agent](#TermUserAgent) for the purpose of this specification.
***<a id="TermSVGUserAgent"></a>SVG user agent***
: An SVG user agent is a [user agent](#TermUserAgent) that is able to retrieve and render SVG content.
***<a id="TermUserAgent"></a>user agent***
:

The general definition of a user agent is an application that retrieves and renders Web content, including text, graphics, sounds, video, images, and other content types. A user agent may require additional user agents that handle some types of content. For instance, a browser may run a separate program or plug-in to render sound or video. User agents include graphical desktop browsers, multimedia players, text browsers, voice browsers, and assistive technologies such as screen readers, screen magnifiers, speech synthesizers, onscreen keyboards, and voice input software.

In general terms, a "user agent" may or may not have the ability to retrieve and render SVG content; however, unless the context requires an alternative interpretation, all references to a "user agent" in this specification are assumed to be references to an [SVG user agent](#TermUserAgent) that retrieves and renders SVG content.

Many programs will fall under multiple software classes. For example, a graphical editor that can import and display SVG files, allow the user to modify them, and then export the modified graphic to file, is an SVG interpreter, an SVG viewer, an SVG authoring tool, and an SVG generator.

### <a id="ConformingSVGGenerators"></a>2.5.1. Conforming SVG Generators

A *conforming SVG generator* is a [SVG generator](#TermSVGGenerator) that:

- always creates a [conforming SVG DOM subtree](#ConformingSVGDOMSubtrees), a [conforming SVG markup fragment](#ConformingSVGFragments), or a [conforming SVG stand-alone file](#ConformingSVGStandAloneFiles);
- does not create documents containing non-conforming SVG document fragments;
- meets all normative requirements in this specification for SVG authors, as well as specific normative requirements for SVG generators.

SVG generators are strongly encouraged to use a Unicode character encoding by default, and to follow the other guidelines of the Character Model for the World Wide Web [[UNICODE](refs.md#ref-unicode)] [[charmod](refs.md#ref-charmod)].

SVG generators handling high-precision data are encouraged to follow the guidelines in the section [Notes on generating high-precision geometry](implnote.md#NumericPrecisionImplementationNotes).

### <a id="ConformingSVGAuthoringTools"></a>2.5.2. Conforming SVG Authoring Tools

An authoring tool, as defined in the Authoring Tool Accessibility Guidelines 2.0, is a *conforming SVG authoring tool* if it is a [conforming SVG generator](#ConformingSVGGenerators) and it also conforms to all relevant Level A requirements from that document [[atag20](refs.md#ref-atag20)]. Level AA and Level AAA requirements are encouraged but not required for conformance.

### <a id="ConformingSVGServers"></a>2.5.3. Conforming SVG Servers

A *conforming SVG server* must meet all the requirements of a [conforming SVG generator](#ConformingSVGGenerators). In addition, conforming SVG servers using HTTP or other protocols that use Internet Media types must serve SVG stand-alone files with the media type `"image/svg+xml"`.

Also, if the SVG file is compressed with gzip or deflate, conforming SVG Servers must indicate this with the appropriate header, according to what the protocol supports. Specifically, for content compressed by the server immediately prior to transfer, the server must use the "Transfer-Encoding: gzip" or "Transfer-Encoding: deflate" headers as appropriate. For content stored in a compressed format on the server (e.g. with the file extension *.svgz*), the server must use the "Content-Encoding: gzip" or "Content-Encoding: deflate" headers as appropriate.

In HTTP, compression of stored *content* (the "entity") is distinct from automatic compression of the *message body*, as defined in HTTP/1.1 TE/ Transfer Encoding ([[rfc2616](refs.md#ref-rfc2616)], sections 14.39 and 14.41). If this is poorly configured, and the compression specified in the HTTP headers does not match the used values, SVG user agents [are required to treat the document as being in error](#user-agent-compression-requirements).

Configuring a server to handle both SVG and SVGZ files means that it must be able to assign the same media type to both types of files, but with different compression headers. Some commonly used servers do not support this configuration in an easy or efficient way, because compression behavior is configured based on media type.

With most modern web servers, it is often easier to upload uncompressed SVG files instead of SVGZ files. Then, rely on the server to compress the file for transmission, and cache it for future request, using the same server instructions as for other text-based file formats such as HTML. This also allows the server to use newer compression methods, when they are supported by the user agent requesting the file.

Alternatively, the web server may be able to correctly process pre-compressed SVGZ files if they are first renamed to use the *.svg.gz* compound file extension. The server would still need to be configured to support static gzip-compressed files.

### <a id="ConformingSVGInterpreters"></a>2.5.4. Conforming SVG Interpreters

An [SVG interpreter](#TermSVGInterpreter) is a program which can parse and process SVG document fragments. Examples of SVG interpreters are server-side transcoding tools or optimizers (e.g., a tool which converts SVG content into modified SVG content) or analysis tools (e.g., a tool which extracts the text content from SVG content, or a validity checker). A transcoder from SVG into another graphics representation, such as an SVG-to-raster transcoder, represents a viewer, and thus viewer conformance criteria also apply.

A *conforming SVG interpreter* must be able to parse and process all XML constructs defined in XML 1.0 [[xml](refs.md#ref-xml)] and Namespaces in XML [[xml-names](refs.md#ref-xml-names)].

A [conforming SVG interpreter](#ConformingSVGInterpreters) must parse any [conforming XML-compatible SVG markup fragment](#ConformingSVGXMLFragments) in a manner that correctly respects the DOM structure (elements, attributes, text content, comments, etc.) of the content. The interpreter is not required to interpret the semantics of all features correctly.

If the SVG interpreter supports non-XML syntaxes (such as HTML), it must correctly parse any [conforming SVG markup fragment](#ConformingSVGFragments) in that syntax.

If the SVG interpreter runs scripts or fetches external resource files as a consequence of processing the SVG content, it must follow the restrictions described for [user agents](#TermUserAgent) in [Processing modes for SVG sub-resource documents](#referencing-modes) and in the [Linking chapter](linking.md).

### <a id="ConformingSVGViewers"></a>2.5.5. Conforming SVG Viewers

An [SVG viewer](#TermSVGViewer) is a program which can parse and process an SVG document fragment and render the contents of the document onto some sort of graphical output medium such as a display, printer, or engraver. Thus, an [SVG viewer](#TermSVGViewer) is also an [SVG interpreter](#TermSVGInterpreter) (in that it can parse and process SVG document fragments), but with the additional requirement of correct rendering.

A *conforming SVG viewer* must be a [conforming SVG interpreter](#ConformingSVGInterpreters), and must be able to support rendering output in at least one of the processing modes defined in this chapter:

- [dynamic interactive mode](#dynamic-interactive-mode)
- [animated mode](#animated-mode)
- [secure animated mode](#secure-animated-mode)
- [static mode](#static-mode)
- [secure static mode](#secure-static-mode)

A conforming SVG viewer must meet all normative requirements indicated in this specification for [user agents](#TermUserAgent), for all features supported by its processing mode(s).

Specific criteria that must apply to all [conforming SVG viewers](#ConformingSVGViewers):

- The viewer must be able to parse all CSS syntax features defined in Cascading Style Sheets, level 2 revision 1 [[CSS2](refs.md#ref-css2)], and must support CSS styling of SVG content including all [required properties](styling.md#RequiredProperties) and [required features](styling.md#RequiredCSSFeatures) listed in the [Styling](styling.md) chapter. The viewer may support other CSS language features, and any other properties that are defined by the corresponding specification to apply to SVG elements. The supported features from CSS 2.1 must be implemented in accordance with the conformance definitions from the CSS 2.1 specification ([[CSS2](refs.md#ref-css2)], section 3.2).
- The viewer must be able to apply styling properties to SVG content using [presentation attributes](styling.md#TermPresentationAttribute).
- Areas of an image of SVG content may have opacity less than 100%. The viewer must at least support Simple Alpha Compositing of the image of the SVG content onto the target canvas, as described in the Compositing and Blending Specification [[compositing-1](refs.md#ref-compositing-1)].
- The viewer must support [data URL references](linking.md#TermDataURL) containing base64-encoded or URL-encoded content, in conformance with the "data:" URL scheme [[rfc2397](refs.md#ref-rfc2397)], wherever a URI reference to another document is permitted within SVG content. When the encoded document is of MIME type `image/svg+xml`, it must be a well-formed, complete SVG document in order to be processed.
- The viewer must support JPEG and PNG image formats [[JPEG](refs.md#ref-jpeg)] [[PNG](refs.md#ref-png)].
Even if the viewer only supports secure processing modes, it is still required to support these image formats, in order to process [data URL references](linking.md#TermDataURL).
- Resampling of image data must be consistent with the specification of property [image-rendering](painting.md#ImageRenderingProperty).
- Whenever possible in the parent environment, the viewer must use information about physical device resolution and expected viewing conditions in order to accurately determine the initial scale in conformance with the rules described in CSS 2.1 ([[CSS2](refs.md#ref-css2)], section 4.3.2). In situations where this information is impossible to determine, the viewer or the parent environment must make a reasonable approximation for common target devices.
- All visual rendering must be accurate to within one device pixel or point of the mathematically correct result at the initial 1:1 zoom ratio. It is suggested that viewers attempt to keep a high degree of accuracy when zooming.
On lower-resolution display devices, support for anti-aliasing or other smoothing methods is highly recommended. It is a requirement for [conforming high-quality SVG viewers](#ConformingSVGViewers).
- If printing devices are supported, SVG content must be printable at printer resolutions with the same graphics features available as required for display (e.g., the specified colors must be rendered on color printers).
- On systems which support accurate sRGB [[SRGB](refs.md#ref-srgb)] color, all sRGB color computations and all resulting color values must be accurate to within one sRGB color component value, where sRGB color component values range from 0 to 255.
- <a id="user-agent-compression-requirements"></a>SVG implementations must correctly support gzip-encoded [[rfc1952](refs.md#ref-rfc1952)] and deflate-encoded [[rfc1951](refs.md#ref-rfc1951)] data streams, for any content type (including SVG, script files, images). SVG implementations that support HTTP must support these encodings according to the HTTP 1.1 specification [[rfc2616](refs.md#ref-rfc2616)]; in particular, the client must specify with an "Accept-Encoding:" request header [HTTP-ACCEPT-ENCODING] those encodings that it accepts, including at minimum gzip and deflate, and then decompress any gzip-encoded and deflate-encoded data streams that are downloaded from the server. When an SVG viewer retrieves compressed content (e.g., an *.svgz* file) over HTTP, if the "Content-Encoding" and "Transfer-Encoding" response headers are missing or specify a value that does not match the compression method that has been applied to the content, then the SVG viewer must not render the content and must treat the document as being [in error](#ErrorProcessing).
- The viewer must use at least single-precision floating point for intermediate calculations on any numerical operations for conversion of coordinates. However, in order to prevent the rounding error on coordinate transformation, at least double-precision floating point computation must be used on [CTM](../css-transforms-1.md#current-transformation-matrix) generation processing. Such minimum typical computation way is expressed with following formulas.
( single ) CTM = ( single ) ( ( double ) [ a 1 c 1 e 1 b 1 d 1 f 1 0 0 1 ] ⋅ ( double ) [ a 2 c 2 e 2 b 2 d 2 f 2 0 0 1 ] )
```
<a id="ctm-matrix"></a>

$$
(single)
{\large \rm CTM} =
(single)
\left(
  (double)
  \left[
    \begin{matrix}
       a_1 & c_1 & e_1 \\
       b_1 & d_1 & f_1 \\
       0 &   0 &   1
    \end{matrix}
  \right]
  \cdot
  (double)
  \left[
    \begin{matrix}
       a_2 & c_2 & e_2 \\
       b_2 & d_2 & f_2 \\
       0 &   0 &   1
    \end{matrix}
  \right]
\right)
$$

  
```

( single ) [ x viewport y viewport 1 ] = CTM ⋅ ( single ) [ x userspace y userspace 1 ]
```
<a id="ctm-matrix2"></a>
$$
(single)
\left[
\begin{matrix}
x_{\rm viewport} \\
y_{\rm viewport} \\
1
\end{matrix}
\right]
=
{\large \rm CTM} \cdot (single)
\left[
\begin{matrix}
x_{\rm userspace} \\
y_{\rm userspace} \\
1
\end{matrix}
\right]
$$
  
```

Furthermore, when it has nested viewport coordinate systems, the [ScreenCTM](types.md#__svg__SVGGraphicsElement__getScreenCTM) which is a transformation matrix produced by nested CTM for transforming user coordinates into the coordinates of an output device also must be generated by double-precision floating point computation.

A [conforming SVG viewer](#ConformingSVGViewers) that supports [processing modes](#processing-modes) that include [interaction](#processing-modes) must support the following additional features:

- For interactive user environments, facilities must exist for zooming and panning of stand-alone SVG documents or SVG document fragments embedded within parent XML documents.
- In environments that have appropriate user interaction facilities, the viewer must support the ability to activate hyperlinks.
- In Web browser environments, the viewer must have the ability to search and select text strings within SVG content.
- If display devices are supported, the viewer must have the ability to select and copy text from SVG content to the system clipboard.
- The viewer must meet all applicable Level A requirements of the User Agent Accessibility Guidelines 2.0 [[UAAG20](refs.md#ref-uaag20)]. Level AA and level AAA features are encouraged, but not required.

A [conforming SVG viewer](#ConformingSVGViewers) that supports [processing modes](#processing-modes) that include [script execution](#processing-modes) must support the following additional features:

- The viewer must be a conforming ECMAScript implementation of all the IDL fragments in this specification. [[WebIDL](refs.md#ref-webidl)]

If the user agent includes an HTML or XHTML viewing capability, or can apply CSS styling properties to XML documents, then a [conforming SVG viewer](#ConformingSVGViewers) must support resources of MIME type "image/svg+xml" wherever raster image external resources can be used, such as in the HTML or XHTML ‘img’ element and in CSS properties that can refer to raster image resources (e.g., ‘background-image’).

#### <a id="PrintingImplementationNotes"></a>2.5.5.1. Printing implementation notes

For user agents which support both zooming on display devices and printing, it is recommended that the default printing option produce printed output that reflects the display device's current view of the current SVG document fragment (assuming there is no media-specific styling), taking into account any zooming and panning done by the user, the current state of animation, and any document changes due to DOM and scripting.

Thus, if the user zooms into a particular area of a map on the display device and then requests a hardcopy, the hardcopy should show the same view of the map as appears on the display device. If a user pauses an animation and prints, the hardcopy should show the same graphics as the currently paused picture on the display device. If scripting has added or removed elements from the document, then the hardcopy should reflect the same changes that would be reflected on the display.

When an SVG document is rendered on a static-only device such as a printer which does not support SVG's animation and scripting and facilities, then the user agent shall ignore any animation and scripting elements in the document and render the remaining graphics elements according to the rules in this specification.

### <a id="ConformingHighQualitySVGViewers"></a>2.5.6. Conforming High-Quality SVG Viewer

In order for a [conforming SVG viewer](#ConformingSVGViewers) to be considered a *conforming high-quality SVG viewer*, it must support the following additional features:

- Professional-quality results with good processing and rendering performance and smooth, flicker-free animations.
- On low-resolution devices such as display devices at 150dpi or less, support for smooth edges on lines, curves and text. (Smoothing is often accomplished using anti-aliasing techniques.)
- Color management via ICC profile support [[ICC](refs.md#ref-icc)] (i.e., the ability to support colors defined using ICC profiles) [[css-color-4](refs.md#ref-css-color-4)]
- Resampling of image data must conform to the requirements for conforming high-quality SVG viewers as specified in the description of property [image-rendering](painting.md#ImageRenderingProperty).
- At least double-precision floating point computation on coordinate system transformation numerical calculations.

A [conforming high-quality SVG viewer](#ConformingSVGViewers) that supports [processing modes](#processing-modes) that include [script execution](#processing-modes), [declarative animation](#processing-modes), or [interaction](#processing-modes) must support the following additional features:

- Progressive rendering and animation effects (i.e., the start of the document will start appearing and animations will start running in parallel with downloading the rest of the document).
- Restricted screen updates (i.e., only required areas of the display are updated in response to redraw events).

A [conforming high-quality SVG viewer](#ConformingSVGViewers) that supports [processing modes](#processing-modes) that include [interaction](#processing-modes) must support the following additional features:

- Background downloading of images and fonts retrieved from a Web server, with updating of the display once the downloads are complete.
