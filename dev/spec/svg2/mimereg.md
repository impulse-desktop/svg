<!-- https://svgwg.org/svg2-draft/mimereg.html, as of 2026-10-18; written by dev/spec_text.py -->

# <a id="mimereg"></a>Appendix J: Media Type Registration for image/svg+xml

**This appendix is normative.**

## <a id="mime-intro"></a>J.1. Introduction

This appendix registers a new MIME media type, "image/svg+xml" in conformance with BCP 13 and W3CRegMedia.

## <a id="mime-registration"></a>J.2. Registration of media type image/svg+xml

** Type name:**
:

image

** Subtype name:**
:

svg+xml

** Required parameters:**
:

None.

** Optional parameters:**
:

charset

Same as application/xml media type, as specified in [[rfc7303](refs.md#ref-rfc7303)] or its successors.

** Encoding considerations:**
:

Same as for application/xml. See [[rfc7303](refs.md#ref-rfc7303)], section 3.2 or its successors.

** Security considerations:**
:

The results of the SVG working group's self assessment of security and privacy concerns is at https://github.com/w3c/svgwg/wiki/SVG-2-Security-&-Privacy-Review.

As with other XML types and as noted in [[rfc7303](refs.md#ref-rfc7303)] section 10, repeated expansion of maliciously constructed XML entities can be used to consume large amounts of memory, which may cause XML processors in constrained environments to fail.

Several SVG elements may cause arbitrary URIs to be referenced. In this case, the security issues of [[rfc3986](refs.md#ref-rfc3986)], section 7, should be considered.

In common with HTML, SVG documents may reference external media such as images, style sheets, and scripting languages. Scripting languages are executable content. In this case, the security considerations in the Media Type registrations for those formats shall apply.

In addition, because of the extensibility features for SVG and of XML in general, it is possible that "image/svg+xml" may describe content that has security implications beyond those described here. However, if the processor follows only the normative semantics of the published specification, this content will be outside the SVG namespace and shall be ignored. Only in the case where the processor recognizes and processes the additional content, or where further processing of that content is dispatched to other processors, would security issues potentially arise. And in that case, they would fall outside the domain of this registration document.

** Privacy considerations:**
:

The results of the SVG working group's self assessment of security and privacy concerns is at https://github.com/w3c/svgwg/wiki/SVG-2-Security-&-Privacy-Review.

SVG's ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’ and ‘[systemLanguage](struct.md#SystemLanguageAttribute)’ attributes may provide some opportunity for examining the configuration of a user agent's host environment. ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’ by determining whether custom extensions are supported by the user agent. ‘[systemLanguage](struct.md#SystemLanguageAttribute)’ by determining the preference of one language relative to another.

** Interoperability considerations:**
:

The published specification describes processing semantics that dictate behavior that must be followed when dealing with, among other things, unrecognized elements and attributes, both in the SVG namespace and in other namespaces.

Because SVG is extensible, conformant "image/svg+xml" processors must expect that content received is well-formed XML, but it cannot be guaranteed that the content is valid to a particular DTD or Schema or that the processor will recognize all of the elements and attributes in the document.

SVG has a published Test Suite and associated implementation report showing which implementations passed which tests at the time of the report. This information is periodically updated as new tests are added or as implementations improve.

** Published specification:**
:

This media type registration is extracted from Appendix P of the [SVG 1.1 specification](../svg11/index.md).

** Applications that use this media type:**
:

SVG is used by Web browsers, often in conjunction with HTML; by mobile phones and digital cameras, as a format for interchange of graphical assets in desktop publishing, for industrial process visualization, display signage, and many other applications which require scalable static or interactive graphical capability.

** Additional information:**
:

**Magic number(s):**
:
**File extension(s):**
: svg

Note that the extension 'svgz' is used as an alias for 'svg.gz' [[rfc1952](refs.md#ref-rfc1952)], *i.e.* octet streams of type image/svg+xml, subsequently compressed with gzip.

**Macintosh file type code(s):**
: "svg " (all lowercase, with a space character as the fourth letter).

Note that the Macintosh file type code 'svgz' (all lowercase) is used as an alias for GZIP [[rfc1952](refs.md#ref-rfc1952)] compressed "svg ", *i.e.* octet streams of type image/svg+xml, subsequently compressed with gzip.

**Macintosh Universal Type Identifier code: **
: `org.w3c.svg` conforms to `public.image` and to `public.xml`
**Windows Clipboard Name:**
: "SVG Image"
**Fragment Identifiers**
: For documents labeled as application/svg+xml, the fragment identifier notation is either Shorthand Pointers (formerly called barenames), the SVG-specific [SVG Views](linking.md#LinksIntoSVG) syntax or a Media Fragment Identifier; all described in the [fragment identifiers section of the SVG specification](linking.md#SVGFragmentIdentifiers).
** Person & email address to contact for further information:**
:

Chris Lilley (www-svg@w3.org) or raise an issue on GitHub: https://github.com/w3c/svgwg/issues/685

** Intended usage:**
:

COMMON

** Restrictions on usage:**
:

None

** Author:**
:

The SVG specification is a work product of the World Wide Web Consortium's SVG Working Group.

** Change controller:**
:

The W3C has change control over this specification.
