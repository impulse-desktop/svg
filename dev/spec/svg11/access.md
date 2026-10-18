<!-- https://www.w3.org/TR/SVG11/access.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](conform.md) ⋅ [Next](i18n.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# Appendix H: Accessibility Support

*This appendix is informative, not normative.*

## <a id="AccessibilityAndSVG"></a>H.1 WAI Accessibility Guidelines

This appendix explains how accessibility guidelines published by W3C's Web Accessibility Initiative (WAI) apply to SVG.

1. The Web Content Accessibility Guidelines (WCAG) 2.0 [[WCAG2](refs.md#ref-WCAG2)] explains how authors can create Web content that is accessible to people with disabilities.
2. The Authoring Tool Accessibility Guidelines 1.0 [[ATAG](refs.md#ref-ATAG)] explains how developers can design accessible authoring tools such as SVG authoring tools. [To conform to the SVG specification](conform.md#ConformingSVGGenerators), an SVG authoring tool must conform to ATAG (priority 1). SVG support for element [grouping](struct.md#Groups) and [reuse](struct.md#UseElement) is relevant to designing accessible SVG authoring tools.
3. The User Agent Accessibility Guidelines 1.0 [[UAAG](refs.md#ref-UAAG)] explains how developers can design accessible user agents such as SVG-enabled browsers. To conform to the SVG specification, an SVG user agent should conform to UAAG. SVG support for scaling, style sheets, the DOM, and metadata are all relevant to designing accessible SVG user agents.

The W3C Note Accessibility Features of SVG [[SVG-ACCESS](refs.md#ref-SVG-ACCESS)] explains in detail how the requirements of the three guidelines apply to SVG.

## <a id="SVGAccessibilityGuidelines"></a>H.2 SVG Content Accessibility Guidelines

This section explains briefly how authors can create accessible SVG documents; it summarizes Accessibility Features of SVG [[SVG-ACCESS](refs.md#ref-SVG-ACCESS)].

**Provide text equivalents for graphics.**
:

- When the text content of a graphic (e.g., in a [‘text’](text.md#TextElement) element) explains its function, no text equivalent is required. Use the [‘title’](struct.md#TitleElement) child element to explain the function of [‘text’](text.md#TextElement) elements whose meaning is not clear from their text content.
- When a graphic does not include explanatory text content, it requires a text equivalent. If the equivalent is complex, use the [‘desc’](struct.md#DescElement) element, otherwise use the [‘title’](struct.md#TitleElement) child element.
- If a graphic is built from meaningful parts, build the description from meaningful parts.

**Do not rely on color alone.**
:

- Do not use color alone to convey information.
- Ensure adequate color contrast. Use style sheets so that users who require certain color combinations may apply them through user style sheets.

**Use markup and style sheets and do so properly.**
:

- Represent text as character data, not as images or curves. Style text with fonts. Authors may describe their own fonts in SVG.
- Separate structure from presentation.
- Use the [‘g’](struct.md#GElement) element and rich descriptions to structure SVG documents. Reuse named objects.
- Publish highly-structured documents, not just graphical representations. Documents that are rich in structure may be rendered graphically, as speech, or as braille. For example, express mathematical relationships in MathML [[MATHML](refs.md#ref-MATHML)] and use SVG for explanatory graphics.
- Author documents that validate to the SVG grammar.
- Use style sheets to specify graphical and aural presentation.
- Use relative units in style sheets.

**Clarify natural language usage.**
:

- Use [‘xml:lang’](struct.md#XMLLangAttribute) to identify the natural language of content and changes in natural language.

**Ensure that dynamic content is accessible.**
:

- Ensure that text equivalents for dynamic content are updated when the dynamic content changes.
- Ensure that SVG documents are usable when scripts or other programmatic objects are turned off or not supported.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](conform.md) ⋅ [Next](i18n.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
