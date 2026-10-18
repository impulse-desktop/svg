<!-- https://svgwg.org/svg2-draft/access.html, as of 2026-10-18; written by dev/spec_text.py -->

# Appendix C: Accessibility Support

*This appendix is informative, not normative.*

## <a id="AccessibilityAndSVG"></a>C.1. SVG Accessibility Features

This appendix highlights the accessibility features of SVG and accessibility related specifications used to support SVG and the associated W3C Web Content Accessibility Guidelines (WCAG) 2.0 [WCAG2] requirements they are designed to support.

**[WCAG2] Create content that can be presented in different ways (for example simpler layout) without losing information or structure: Information and Relationships.**
:

- Support for aria-labelledby for label relationships.
- Support for aria-describedby for description relationships.
- Support for aria-owns for structural relationships.
- Support for aria-controls for control relationships where an element controls the content and or behavior of another element.
- The ‘[g](struct.md#GElement)’ for defined groupings.
- Support for the WAI-ARIA group and radiogroup roles.

**[WCAG2] Keyboard Accessible: Make all functionality available from the keyboard.**
:

- [Focus support](interact.md#Focus) and the ‘[tabindex](struct.md#SVGElementTabindexAttribute)’ attribute for sequential focus navigation aligned with HTML
- [Keyboard Event attribute support along with Mouse Event support](interact.md#EventAttributes) for script authors
- [Keyboard Events](interact.md#SVGEvents) for script authors
- Script support for setting focus on each [SVG Element](types.md#InterfaceSVGElement) in the DOM
- Script support for acquiring the ‘[tabindex](struct.md#SVGElementTabindexAttribute)’ attribute on each [SVG Element](types.md#InterfaceSVGElement) in the DOM
- Script support for the HTML activeElement property in the [Document interface](struct.md#InterfaceDocumentExtensions)

**[WCAG2] Navigable: Provide ways to help users navigate, find content, and determine where they are. **
:

- To enable bypassing of blocks of content SVG supports the: WAI-ARIA landmark roles.
- Supports ‘[title](struct.md#TitleElement)’ to provide page titles.
- Supports ‘[tabindex](struct.md#SVGElementTabindexAttribute)’ to provide a sequential focus navigation order.
- Supports the ‘[a](linking.md#AElement)’ element, enabling authors to supply the link purpose both from its content.
- Supports headings and labels through the use of the heading role with aria-level, aria-labelledby and aria-label.
- Support visible focus by rendering visible focus on focused elements in the tab order.

**[WCAG2] Readable: Make text content readable and understandable.**
:

- Supports the language of the page as well as its parts through the ‘[lang](struct.md#LangAttribute)’ attribute.

**[WCAG2] Compatible: Maximize compatibility with current and future user agents, including assistive technologies. **
:

- Supports Name, Role, and Value through the use of [WAI-ARIA attributes](struct.md#WAIARIAAttributes) and the ‘[title](struct.md#TitleElement)’ element.

**WAI-ARIA Support and text alternatives**
:

- [WAI-ARIA attributes](struct.md#WAIARIAAttributes)
- [Desc and Title text element alternatives](struct.md#DescriptionAndTitleElements)

## <a id="SVGRelatedAccessibilityDocuments"></a>C.2. Supporting SVG Accessibility Specifications and Guidelines

This section enumerates the SVG accessibility-related specifications and authoring guidelines.

**Related SVG Accessibility Specifications for User Agents**
:

- The SVG Accessibility API Mappings [SVG-AAM] specification.
- The Core Accessibility API Mappings 1.1 [CORE-AAM]specification.
- The Accessible Name and Description: Computation and API Mappings 1.1 [ACCNAME-AAM] specification.

**Related Specifications for Content Authors**
:

- WAI-ARIA 1.1 [WAI-ARIA]
- W3C Web Content Accessibility Guidelines (WCAG) 2.0 [WCAG2]
