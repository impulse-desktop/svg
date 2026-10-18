# SVG 2, Chapter 6: Styling

## The style element: css-sheet (exists)

- [ ] svg2/styling.md#StyleElement: style's content is never directly rendered; the UA style sheet always sets display:none on it with importance over any other rule or presentation attribute.
- [ ] svg2/styling.md#StyleElementTypeAttribute: the type attribute names the style sheet language as a media type, initial value text/css; if type is absent, the language is assumed to be CSS.
- [ ] svg2/styling.md#StyleElementMediaAttribute: the media attribute gives a media query that must match for the style sheet to apply; if absent, the style sheet applies unconditionally.
- [ ] svg2/styling.md#RequiredCSSFeatures: @media, @import, @charset and @font-face rules within a style sheet must be supported.

## Class and style attributes: css-classes-attributes (exists)

- [ ] svg2/styling.md#ClassAttribute: the class attribute assigns one or more space-separated class names to an element, usable for selector matching.
- [ ] svg2/styling.md#StyleAttribute: the style attribute is parsed as a CSS declaration-list and applies directly to the element (as inline style, at the top of author-origin specificity).

## Presentation attributes: css-geometry-elements (exists)

- [ ] svg2/styling.md#PresentationAttributes: a presentation attribute's name matches a CSS property and its value is parsed as a value of that property; it contributes to the author level of the cascade, after all other author style sheets, with specificity 0.
- [ ] svg2/styling.md#PresentationAttributes: an !important declaration is not permitted in a presentation attribute's value; using one makes the attribute's value invalid.
- [ ] svg2/styling.md#PresentationAttributes: only SVG-namespace elements support presentation attributes; an attribute that merely shares a property's name on an element not designated to carry it must not be parsed as a presentation attribute and must not affect CSS cascading or inheritance.
- [ ] svg2/styling.md#PresentationAttributes: the geometry properties (cx, cy, r, rx, ry, x, y, width, height) have a presentation attribute only on the elements the spec designates for each property; elsewhere the identically-named XML attribute means something else (e.g. x on text, r on radialGradient).
- [ ] svg2/styling.md#PresentationAttributes: the transform property's presentation attribute is named transform on most elements, but patternTransform on pattern and gradientTransform on linearGradient/radialGradient; all map onto the CSS transform property.
- [ ] svg2/styling.md#PresentationAttributes: the fill presentation attribute is supported on any element in the SVG namespace (other than animation elements, which have an unrelated fill attribute).

## User agent style sheet: transform-origin-root (exists)

- [ ] svg2/styling.md#UAStyleSheet: the UA style sheet sets overflow:hidden by default on every svg element other than the root, and on image, marker, pattern and symbol, even though overflow's own initial value is auto.
- [ ] svg2/styling.md#UAStyleSheet: the UA style sheet sets display:none !important on every never-rendered element and on symbol; since a UA-stylesheet !important rule overrides all user and author styles, authors cannot make these elements render by changing display.
- [ ] svg2/styling.md#UAStyleSheet: a symbol renders only when it is the direct child of a shadow root whose host is a use element, and must then always render whenever that host use element renders.
- [ ] svg2/styling.md#UAStyleSheet: SVG elements default transform-origin to 0 0 rather than CSS's normal 50% 50%, except a root svg element, or an svg that is a child of a foreignObject or of an element outside the SVG namespace, which keep the center as their default origin.

## Notes

