# SVG 2, Chapter 6: Styling

## The style element: style-sheets, render-not-rendered

- [x] svg2/styling.md#StyleElement: style's content is never directly rendered; the UA style sheet always sets display:none on it with importance over any other rule or presentation attribute. [render-not-rendered#not-rendered style-sheets#type]
- [x] svg2/styling.md#StyleElementTypeAttribute: the type attribute names the style sheet language as a media type, initial value text/css; if type is absent, the language is assumed to be CSS. [style-sheets#type]
- [x] svg2/styling.md#StyleElementMediaAttribute: the media attribute gives a media query that must match for the style sheet to apply; if absent, the style sheet applies unconditionally. [style-sheets#media]
- [x] svg2/styling.md#StyleElement: a style element of the XHTML namespace, anywhere in the document, a foreign subtree or a foreignObject's content too, is a style sheet as SVG's style element is, its media and type attributes the same; another element of that namespace is none (D47) [style-html#child style-html#nested style-html#foreign-object style-html#media style-html#type style-html#other-element style-html#prefixed]
- [-] svg2/styling.md#RequiredCSSFeatures: @media, @import, @charset and @font-face rules within a style sheet must be supported. (D15) [css-layers#import-nothing css-sheet-recovery#charset css-calc-contexts#media]

## XML stylesheet instructions: xml-stylesheet

- [x] svg2/styling.md#RequiredCSSFeatures: xml-stylesheet instructions in the prolog associate CSS sheets with the document; data: sheets are decoded and applied before its style elements, without creating selector-visible elements. [xml-stylesheet#percent xml-stylesheet#type-omitted xml-stylesheet#base64 xml-stylesheet#document-order xml-stylesheet#style-order xml-stylesheet#inline xml-stylesheet#important xml-stylesheet#structural xml-stylesheet#inside-root xml-stylesheet#after-root xml-stylesheet#after-doctype]
- [x] svg2/styling.md#RequiredCSSFeatures: stylesheet associations use media queries and the preferred title; an alternate sheet is enabled only as a member of that selected set. [xml-stylesheet#screen xml-stylesheet#print xml-stylesheet#empty-media xml-stylesheet#preferred xml-stylesheet#alternate xml-stylesheet#alternate-no-title xml-stylesheet#alternate-before-preferred xml-stylesheet#alternate-after-preferred xml-stylesheet#alternate-before-style]
- [x] svg2/styling.md#RequiredCSSFeatures: xml-stylesheet pseudo-attributes use quoted values, character references and predefined entities; malformed syntax, duplicate names and invalid references discard the instruction rather than the document, and a retrieved MIME type takes precedence over the advisory type. [xml-stylesheet#quoted-attributes xml-stylesheet#character-references xml-stylesheet#predefined-entities xml-stylesheet#duplicate xml-stylesheet#duplicate-unknown xml-stylesheet#unquoted xml-stylesheet#no-equals xml-stylesheet#unclosed-quote xml-stylesheet#no-space xml-stylesheet#bad-name xml-stylesheet#wrong-target xml-stylesheet#target-case xml-stylesheet#unknown-entity xml-stylesheet#empty-reference xml-stylesheet#invalid-digit xml-stylesheet#invalid-decimal xml-stylesheet#zero xml-stylesheet#control xml-stylesheet#surrogate xml-stylesheet#non-character xml-stylesheet#out-of-range xml-stylesheet#no-semicolon xml-stylesheet#literal-less-than xml-stylesheet#recovery xml-stylesheet#advisory-type xml-stylesheet#mime-parameters xml-stylesheet#wrong-mime]
- [-] svg2/styling.md#RequiredCSSFeatures: external stylesheet associations require fetching a resource; only data: sheets are available without a resource loader. (D15) [xml-stylesheet#external xml-stylesheet#bad-data xml-stylesheet#missing-href]

## Class and style attributes: style-sheets, css-classes-attributes (exists)

- [x] svg2/styling.md#ClassAttribute: the class attribute assigns one or more space-separated class names to an element, usable for selector matching. [css-selector-basics#class]
- [x] svg2/styling.md#StyleAttribute: the style attribute is parsed as a CSS declaration-list and applies directly to the element (as inline style, at the top of author-origin specificity). [style-sheets#cascade css-cascade-sort#sort-attachment]

## Presentation attributes: style-sheets, struct-fragment, geometry-properties, gradient-linear-geometry, render-root-background-attribute

- [x] svg2/styling.md#PresentationAttributes: a presentation attribute's name matches a CSS property and its value is parsed as a value of that property; it contributes to the author level of the cascade, after all other author style sheets, with specificity 0. [style-sheets#cascade css-cascade-sort#presentation]
- [x] svg2/styling.md#PresentationAttributes: the properties the table lists have presentation attributes, and no others: background-color and background-image, CSS properties an outermost svg takes, have none. [render-root-background-attribute#none]
- [x] svg2/styling.md#PresentationAttributes: an !important declaration is not permitted in a presentation attribute's value; using one makes the attribute's value invalid. [style-sheets#important-attribute]
- [x] svg2/styling.md#PresentationAttributes: only SVG-namespace elements support presentation attributes; an attribute that merely shares a property's name on an element not designated to carry it must not be parsed as a presentation attribute and must not affect CSS cascading or inheritance. [struct-fragment#fragment]
- [x] svg2/styling.md#PresentationAttributes: the geometry properties (cx, cy, r, rx, ry, x, y, width, height) have a presentation attribute only on the elements the spec designates for each property; elsewhere the identically-named XML attribute means something else (e.g. x on text, r on radialGradient). [geometry-properties#not-designated style-sheets#geometry]
- [x] svg2/styling.md#PresentationAttributes: the transform property's presentation attribute is named transform on most elements, but patternTransform on pattern and gradientTransform on linearGradient/radialGradient; all map onto the CSS transform property. [transform-paint-servers#pattern transform-paint-servers#gradient]
- [x] svg2/styling.md#PresentationAttributes: the fill presentation attribute is supported on any element in the SVG namespace (other than animation elements, which have an unrelated fill attribute). [css-cascade-sort#no-cascade]

## User agent style sheet: render-root-transform, render-root-rotate, render-not-rendered, symbol-geometry, viewport-nested

- [x] svg2/styling.md#UAStyleSheet: the UA style sheet sets overflow:hidden by default on every svg element other than the root, and on image, marker, pattern and symbol, even though overflow's own initial value is auto. [viewport-nested#clip symbol-geometry#overflow image-placement#default-size]
- [x] svg2/styling.md#UAStyleSheet: the UA style sheet sets display:none !important on every never-rendered element and on symbol; since a UA-stylesheet !important rule overrides all user and author styles, authors cannot make these elements render by changing display. [render-not-rendered#not-rendered]
- [x] svg2/styling.md#UAStyleSheet: a symbol renders only when it is the direct child of a shadow root whose host is a use element, and must then always render whenever that host use element renders. [symbol-geometry#display-none-symbol render-not-rendered#not-rendered]
- [x] svg2/styling.md#UAStyleSheet: SVG elements default transform-origin to 0 0 rather than CSS's normal 50% 50%, except a root svg element, or an svg that is a child of a foreignObject or of an element outside the SVG namespace, which keep the center as their default origin. [render-root-transform#root-origin render-root-transform#other-origin render-root-rotate#center]

## Notes

The linked XML stylesheet recommendation defines the pseudo-attribute parser
and prolog scope in [sections 3 and 4](https://www.w3.org/TR/xml-stylesheet/).

