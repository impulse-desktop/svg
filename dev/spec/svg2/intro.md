<!-- https://svgwg.org/svg2-draft/intro.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 1: Introduction

## <a id="AboutSVG"></a>1.1. About SVG

This specification defines the features and syntax for Scalable Vector Graphics (SVG).

SVG is a language for describing two-dimensional graphics. As a standalone format or when mixed with other XML, it uses the XML syntax [[xml](refs.md#ref-xml)]. SVG code used inside HTML documents uses the HTML syntax [[HTML](refs.md#ref-html)]. SVG allows for three types of graphic objects: vector graphic shapes (e.g., paths consisting of straight lines and curves), images and text. Graphical objects can be grouped, styled, transformed and composited. The feature set includes nested transformations, clipping paths, alpha masks, filter effects and template objects.

SVG drawings can be [interactive](interact.md) and [dynamic](interact.md#ScriptElement). [Animations](animate.md) can be defined and triggered either declaratively (i.e., by embedding SVG animation elements in SVG content) or via scripting.

Sophisticated applications of SVG are possible by use of a supplemental scripting language which accesses [SVG Document Object Model (DOM)](types.md#SVGDOMOverview), which provides complete access to all elements, attributes and properties. A rich set of [event handlers](interact.md#SVGEvents) can be assigned to any SVG graphical object. Within a web page, the same scripts can work on both HTML and SVG elements. [Scripting](interact.md).

SVG is useful for rich graphical presentation of information, including a number of [accessibility features that, used correctly](access.md), ensure the content can be used by the widest possible audience. But a direct link to source data, where possible, is helpful for many people to understand the content provided.

## <a id="W3CCompatibility"></a>1.2. Compatibility with other standards efforts

SVG leverages and integrates with other W3C specifications and standards efforts, as described in the following:

- SVG can be integrated with HTML either by using SVG in HTML or by using HTML in SVG, in both cases either by inclusion or reference.
- SVG is an application of XML and is compatible with XML 1.0 and with the Namespaces in XML specification. However, when SVG content is included in HTML document, the HTML syntax applies and may not be compatible with XML. See SVG Integration for details.
- SVG content is styled with CSS. See [Styling with CSS](styling.md) for details.
- SVG includes a complete Document Object Model (DOM) and extends DOM4. The SVG DOM has a high level of compatibility and consistency with the HTML DOM. Additionally, the SVG DOM supports and incorporates many of the facilities described in the CSS object model and event handling [[dom-level-2-style](refs.md#ref-dom-level-2-style)] [[uievents](refs.md#ref-uievents)].
- SVG incorporates some features and approaches that are part of SMIL 3, including the ‘[switch](struct.md#SwitchElement)’ element and the ‘[systemLanguage](struct.md#SystemLanguageAttribute)’ attribute.
- SVG is compatible with W3C work on internationalization. References (W3C and otherwise) include: [[UNICODE](refs.md#ref-unicode)] and [[charmod](refs.md#ref-charmod)].
- SVG is compatible with W3C work on Web Accessibility. See [Accessibility Support](access.md).

## <a id="RelationshipToPrevious"></a>1.3. Relationship to previous versions of this standard

This edition of the SVG standard has been developed based on, and built upon, the 1.1 edition released in 2003. An intermediate version of SVG - named Tiny 1.2 - was released in 2008. However it did not receive wide acceptance and there have been very few implementations of its enhanced feature set. However there are some 1.2 features that have been implemented by many SVG implementations and those have been incorporated as part of this specification. But otherwise, the SVG Working Group consider version Tiny 1.2 to be a deprecated branch of the SVG standard.

## <a id="ConformanceTerms"></a>1.4. Normative Terminology

Within this specification, the key words "MUST", "MUST NOT", "REQUIRED", "SHALL", "SHALL NOT", "SHOULD", "SHOULD NOT", "RECOMMENDED", "MAY", and "OPTIONAL" are to be interpreted as described in Key words for use in RFCs to Indicate Requirement Levels [[rfc2119](refs.md#ref-rfc2119)]. However, for readability, these words do not appear in all uppercase letters in this specification.

At times, this specification recommends good practice for authors and user agents.
