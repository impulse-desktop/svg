# SVG 2, Chapter 5: Document Structure

## SVG document fragment and namespace: struct-fragment, struct-elements

- [x] svg2/struct.md#TermSVGDocumentFragment: a subtree rooted at an svg element is an SVG document fragment only if that svg is the document root, or its parent is not in the SVG namespace; an svg that is a direct child of another SVG-namespace element is not such a root. [struct-fragment#fragment viewport-nested#clip]
- [x] svg2/struct.md#Namespace: when SVG is parsed as XML, SVG elements must be declared in the SVG namespace (via an xmlns attribute); when parsed with the HTML parser, the namespace is assigned automatically. [struct-elements#no-namespace struct-fragment#fragment]
- [x] svg2/struct.md#TermOutermostSVGElement: the outermost svg element is the furthest svg ancestor reachable while passing only through SVG-namespaced container elements (the current SVG document fragment). [struct-fragment#fragment root-viewport#user-units-px]

## Element categories: which elements draw, contain or reference: struct-fragment, struct-elements, use-geometry

- [x] svg2/struct.md#TermGraphicsElement: circle, ellipse, image, line, path, polygon and rect, polyline are the graphics elements that can cause content to be drawn onto the canvas. [struct-elements#rect struct-elements#circle struct-elements#ellipse struct-elements#line struct-elements#polyline struct-elements#polygon struct-elements#path struct-elements#image]
- [x] svg2/struct.md#TermContainerElement: a, clipPath, defs, g, marker, mask, pattern, svg, switch and symbol are container elements, able to hold graphics elements and other containers. [link-a#href struct-elements#switch-container viewport-nested#clip symbol-geometry#position resource-display#clip resource-display#mask pserver-ancestry#pattern-content marker-vertices#subpaths struct-fragment#fragment]
- [x] svg2/struct.md#TermStructuralElement: defs, g, svg, symbol and use are the structural elements. [struct-fragment#fragment use-geometry#translate-after symbol-geometry#position viewport-nested#clip]
- [x] svg2/struct.md#TermGraphicsReferencingElement: image and use are graphics-referencing elements: graphics elements that source their graphical content from a reference to another document or element. [struct-elements#image use-geometry#translate-after]

## Positioning an embedded svg element: struct-fragment, viewport-nested

- [x] svg2/struct.md#SVGElement: the x and y properties position an embedded (non-outermost) svg element's rectangular region; they have no effect on an outermost svg element. [viewport-nested#clip struct-fragment#fragment]
- [x] svg2/struct.md#SVGElement: for an outermost svg, width and height give the document fragment's intrinsic size; for an embedded svg, they size the rectangular region it is placed into. [root-viewport#user-units-px viewport-nested#clip]

## The defs element: struct-fragment, resource-display

- [x] svg2/struct.md#DefsElement: defs has the same content model as g; anything that can be a child of g can be a child of defs, and vice versa. [struct-fragment#fragment]
- [x] svg2/struct.md#DefsElement: descendants of defs are not rendered directly (the UA style sheet forces display:none on defs with importance over any other rule), but they remain always referenceable by other elements regardless of that display value. [struct-fragment#fragment resource-display#clip struct-elements#duplicate-id]

## The symbol element and refX/refY: symbol-geometry

- [x] svg2/struct.md#SymbolElement: a symbol defines a template only instantiated through use; once instantiated it establishes a nested coordinate system and renders much like a nested svg element. [symbol-geometry#position]
- [x] svg2/struct.md#SymbolElement: x, y, width and height on symbol behave as they would on an svg element once instantiated by use; if width/height compute to auto and are not overridden by the use element, they are treated as 100%. [symbol-geometry#use-size symbol-geometry#auto-size]
- [x] svg2/struct.md#SymbolElementRefXAttribute: refX/refY place a reference point of the symbol exactly at its x,y position; values are <length>|left|center|right for refX and <length>|top|center|bottom for refY, with keywords resolving to 0%, 50% or 100%. [symbol-geometry#ref symbol-geometry#ref-keywords]
- [x] svg2/struct.md#SymbolElementRefXAttribute: refX/refY are interpreted in the symbol's own content coordinate system, after its viewBox and preserveAspectRatio have been applied. [symbol-geometry#ref-viewbox symbol-geometry#ref-slice]
- [x] svg2/struct.md#SymbolElementRefXAttribute: if refX or refY is omitted, no adjustment is made in that dimension and the top/left of the symbol's viewport region is placed at the x,y point instead; this differs from explicitly specifying a value of 0. [symbol-geometry#viewbox-origin]
- [x] svg2/struct.md#SymbolNotes: the UA style sheet sets symbol's overflow to hidden, clipping content that falls outside the symbol's own viewport unless overridden. [symbol-geometry#overflow]
- [x] svg2/struct.md#SymbolNotes: a symbol must never render directly (UA style sheet forces display:none with importance); the instance that is the direct referenced element of a use must always compute to display:inline (importance-enforced), rendering whenever the host use renders; any other symbol cloned deeper within a shadow tree is only a definition and must not render. [symbol-geometry#display-none-symbol]

## Positioning and sizing the use element: use-geometry

- [x] svg2/struct.md#UseElement: use renders a clone of the referenced element in its place; if the referenced element is a container, the whole subtree rooted at it is cloned. [use-geometry#translate-after use-geometry#indirect]
- [x] svg2/struct.md#UseElement: cloned content inherits style from the use element itself, and style rules in scope at the referenced element also apply within the cloned shadow tree. [use-shadow-styles#inherit-from-use use-shadow-styles#clone-rules]
- [x] svg2/struct.md#UseElement: x, y, width and height on use position the referenced content; width/height take effect only if the referenced element establishes a viewport (svg or symbol), in which case a non-auto use value overrides the corresponding property on the instance root. [use-geometry#size-svg use-geometry#size-other]
- [x] svg2/struct.md#UseElement: a negative width or height on use is invalid and ignored; a zero width or height, when it would have an effect on the referenced element, disables rendering of that element. [use-geometry#negative-size use-geometry#zero-size]
- [x] svg2/struct.md#UseLayout: the computed x,y on use add a translate(x,y) appended after any other transforms on the use element itself, affecting the coordinate system used by userSpaceOnUse effects applied to the use element. [use-geometry#translate-after use-geometry#clip-translated]
- [x] svg2/struct.md#UseLayout: width/height on use have no effect at all if the referenced element does not establish a new viewport; use itself never establishes a viewport, so it never affects percentage interpretation in the reused graphics. [use-geometry#size-other use-geometry#percent]

## Resolving the use reference and its errors: use-geometry, struct-elements, use-chain, measure-use-chain

- [x] svg2/struct.md#UseShadowTree: if the resolved referenced element is not an SVG element, the reference is invalid and the use element is in error (not rendered). [use-geometry#foreign]
- [x] svg2/struct.md#UseShadowTree: if the referenced element is a (shadow-including) ancestor of the use element, this is an invalid circular reference and the use element is in error. [use-geometry#ancestor struct-elements#use-empty-href]
- [x] svg2/struct.md#UseShadowTree: a use element with an unresolved or invalid reference is not rendered; for bounding-box purposes it behaves as an empty container element. [bbox-inputs#unresolved-use struct-elements#use-data-url]
- [x] svg2/struct.md#UseShadowTree: use-of-use chains are resolved recursively, at any depth: 40 uses each of the one before draw and measure whole, as an Inkscape drawing cloning its clones 35 deep needs; a chain of references that is circular is in error and must not be rendered. [use-geometry#indirect use-geometry#circular use-chain#deep measure-use-chain#measured]
- [x] svg2/struct.md#UseElementHrefAttribute: this specification does not allow cross-origin or data: URL resource requests for a use element's reference. [struct-elements#use-data-url]
- [x] svg2/struct.md#UseElementHrefAttribute: an href without a fragment identifier refers to the root element of the referenced document. [struct-elements#use-empty-href]

## Style scoping inside the use shadow tree: use-shadow-styles

- [x] svg2/struct.md#UseStyleInheritance: elements in the use shadow tree inherit from the host use element; outer-document style rules do not match shadow-tree elements directly, only the shadow tree's own cloned stylesheets are matched against them. [use-shadow-styles#inherit-from-use use-shadow-styles#clone-rules]
- [x] svg2/struct.md#UseStyleInheritance: presentation attributes and the style attribute are cloned onto each element instance like any other attribute. [use-shadow-styles#clone-rules use-shadow-styles#inherit-from-use]
- [x] svg2/struct.md#UseStyleInheritance: a complex selector that matched an original element via its ancestors or siblings will not match the corresponding element instance when those ancestors/siblings were not themselves cloned into the shadow tree. [use-shadow-styles#not-ancestors use-shadow-styles#not-siblings]
- [x] svg2/struct.md#UseStyleInheritance: visibility:hidden on a use element does not guarantee the referenced content is hidden, since visibility does not apply directly to container elements; an explicit visibility:visible on a shadow-tree descendant still renders that descendant. [use-shadow-styles#visibility]

## The switch element: switch-select, struct-elements

- [x] svg2/struct.md#SwitchElement: switch evaluates requiredExtensions/systemLanguage on its direct children in document order and renders only the first child for which all such tests are true; if none match, switch renders nothing. [switch-select#first-true switch-select#language-prefix]
- [x] svg2/struct.md#SwitchElement: if the selected child is a container, its whole subtree is rendered; unselected children's subtrees are not rendered at all. [struct-elements#switch-container switch-select#first-true]
- [x] svg2/struct.md#SwitchElement: the display and visibility properties have no effect on switch's true/false selection test. [switch-select#display-none-chosen]
- [x] svg2/struct.md#SwitchElement: switch does not affect the processing of script or style elements. [struct-elements#switch-style]
- [x] svg2/struct.md#ConditionalProcessingOverview: requiredFeatures, a conditional-processing attribute from SVG 1.1, is not defined in SVG 2 and has no effect. [switch-select#first-true]

## Conditional processing attributes: switch-select, resource-display

- [x] svg2/struct.md#ConditionalProcessingOverview: an element excluded by conditional processing is treated as if display computed to none: it blocks only direct rendering, not the element being referenced elsewhere (e.g. via use). [switch-select#excluded resource-display#conditional]
- [x] svg2/struct.md#ConditionalProcessingOverview: conditional processing has no effect on never-rendered elements (style and script are unaffected), but it still determines whether child content of a never-rendered container (e.g. pattern, mask) contributes to that container's graphical effect. [struct-elements#pattern-conditional resource-display#conditional]
- [x] svg2/struct.md#ConditionalProcessingRequiredExtensionsAttribute: requiredExtensions evaluates true only if every listed extension URL is supported, otherwise the element and its children are skipped; the attribute is absent evaluates true; an empty-string value evaluates false. [switch-select#first-true]
- [x] svg2/struct.md#ConditionalProcessingSystemLanguageAttribute: systemLanguage evaluates true if a user-preferred language tag case-insensitively matches, or is a case-insensitive prefix of (followed by '-'), one of its comma-separated tags; absent evaluates true; an empty-string value evaluates false. (D11) [switch-select#language-prefix switch-select#language-case switch-select#language-list switch-select#excluded]

## Foreign content and element identity: struct-fragment, struct-elements

- [x] svg2/struct.md#ForeignNamespaces: unknown foreign-namespace elements are kept in the document but ignored and excluded from rendering (other than foreignObject content). [struct-fragment#fragment]
- [x] svg2/struct.md#ForeignNamespaces: unknown attributes, including ones from foreign namespaces, are kept in the document but otherwise ignored for rendering. [struct-fragment#fragment]
- [x] svg2/struct.md#IDAttribute: the id attribute must be unique within the node tree, must not be an empty string, and must not contain whitespace characters. [struct-elements#duplicate-id struct-elements#use-empty-href]

## The lang and xml:lang attributes: css-lang

- [x] svg2/struct.md#LangSpaceAttrs: lang (no namespace) and xml:lang give the primary language of an element's contents; omitted, an element's language is its parent's. [css-lang#language]
- [x] svg2/struct.md#LangSpaceAttrs: with both lang and xml:lang on one element, xml:lang decides its language and lang is ignored. [css-lang#language]
- [x] svg2/struct.md#LangAttribute: lang is a BCP 47 tag or empty; empty says the language is unknown, not the parent's. [css-lang#language css-lang#untagged]

## Notes

