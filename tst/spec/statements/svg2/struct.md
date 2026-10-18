# SVG 2, Chapter 5: Document Structure

## SVG document fragment and namespace: struct-document-fragment

- [ ] svg2/struct.md#TermSVGDocumentFragment: a subtree rooted at an svg element is an SVG document fragment only if that svg is the document root, or its parent is not in the SVG namespace; an svg that is a direct child of another SVG-namespace element is not such a root.
- [ ] svg2/struct.md#Namespace: when SVG is parsed as XML, SVG elements must be declared in the SVG namespace (via an xmlns attribute); when parsed with the HTML parser, the namespace is assigned automatically.
- [ ] svg2/struct.md#TermOutermostSVGElement: the outermost svg element is the furthest svg ancestor reachable while passing only through SVG-namespaced container elements (the current SVG document fragment).

## Element categories: which elements draw, contain or reference: struct-element-categories

- [ ] svg2/struct.md#TermGraphicsElement: circle, ellipse, image, line, path, polygon and rect, polyline are the graphics elements that can cause content to be drawn onto the canvas.
- [ ] svg2/struct.md#TermContainerElement: a, clipPath, defs, g, marker, mask, pattern, svg, switch and symbol are container elements, able to hold graphics elements and other containers.
- [ ] svg2/struct.md#TermStructuralElement: defs, g, svg, symbol and use are the structural elements.
- [ ] svg2/struct.md#TermGraphicsReferencingElement: image and use are graphics-referencing elements: graphics elements that source their graphical content from a reference to another document or element.

## Positioning an embedded svg element: measure-nested-svg (exists)

- [ ] svg2/struct.md#SVGElement: the x and y properties position an embedded (non-outermost) svg element's rectangular region; they have no effect on an outermost svg element.
- [ ] svg2/struct.md#SVGElement: for an outermost svg, width and height give the document fragment's intrinsic size; for an embedded svg, they size the rectangular region it is placed into.

## The defs element: struct-defs

- [ ] svg2/struct.md#DefsElement: defs has the same content model as g; anything that can be a child of g can be a child of defs, and vice versa.
- [ ] svg2/struct.md#DefsElement: descendants of defs are not rendered directly (the UA style sheet forces display:none on defs with importance over any other rule), but they remain always referenceable by other elements regardless of that display value.

## The symbol element and refX/refY: symbol-position (exists)

- [ ] svg2/struct.md#SymbolElement: a symbol defines a template only instantiated through use; once instantiated it establishes a nested coordinate system and renders much like a nested svg element.
- [ ] svg2/struct.md#SymbolElement: x, y, width and height on symbol behave as they would on an svg element once instantiated by use; if width/height compute to auto and are not overridden by the use element, they are treated as 100%.
- [ ] svg2/struct.md#SymbolElementRefXAttribute: refX/refY place a reference point of the symbol exactly at its x,y position; values are <length>|left|center|right for refX and <length>|top|center|bottom for refY, with keywords resolving to 0%, 50% or 100%.
- [ ] svg2/struct.md#SymbolElementRefXAttribute: refX/refY are interpreted in the symbol's own content coordinate system, after its viewBox and preserveAspectRatio have been applied.
- [ ] svg2/struct.md#SymbolElementRefXAttribute: if refX or refY is omitted, no adjustment is made in that dimension and the top/left of the symbol's viewport region is placed at the x,y point instead; this differs from explicitly specifying a value of 0.
- [ ] svg2/struct.md#SymbolNotes: the UA style sheet sets symbol's overflow to hidden, clipping content that falls outside the symbol's own viewport unless overridden.
- [ ] svg2/struct.md#SymbolNotes: a symbol must never render directly (UA style sheet forces display:none with importance); the instance that is the direct referenced element of a use must always compute to display:inline (importance-enforced), rendering whenever the host use renders; any other symbol cloned deeper within a shadow tree is only a definition and must not render.

## Positioning and sizing the use element: use-geometry-instance (exists)

- [ ] svg2/struct.md#UseElement: use renders a clone of the referenced element in its place; if the referenced element is a container, the whole subtree rooted at it is cloned.
- [ ] svg2/struct.md#UseElement: cloned content inherits style from the use element itself, and style rules in scope at the referenced element also apply within the cloned shadow tree.
- [ ] svg2/struct.md#UseElement: x, y, width and height on use position the referenced content; width/height take effect only if the referenced element establishes a viewport (svg or symbol), in which case a non-auto use value overrides the corresponding property on the instance root.
- [ ] svg2/struct.md#UseElement: a negative width or height on use is invalid and ignored; a zero width or height, when it would have an effect on the referenced element, disables rendering of that element.
- [ ] svg2/struct.md#UseLayout: the computed x,y on use add a translate(x,y) appended after any other transforms on the use element itself, affecting the coordinate system used by userSpaceOnUse effects applied to the use element.
- [ ] svg2/struct.md#UseLayout: width/height on use have no effect at all if the referenced element does not establish a new viewport; use itself never establishes a viewport, so it never affects percentage interpretation in the reused graphics.

## Resolving the use reference and its errors: use-edge (exists)

- [ ] svg2/struct.md#UseShadowTree: if the resolved referenced element is not an SVG element, the reference is invalid and the use element is in error (not rendered).
- [ ] svg2/struct.md#UseShadowTree: if the referenced element is a (shadow-including) ancestor of the use element, this is an invalid circular reference and the use element is in error.
- [ ] svg2/struct.md#UseShadowTree: a use element with an unresolved or invalid reference is not rendered; for bounding-box purposes it behaves as an empty container element.
- [ ] svg2/struct.md#UseShadowTree: use-of-use chains are resolved recursively; a chain of references that is circular is in error and must not be rendered.
- [ ] svg2/struct.md#UseElementHrefAttribute: this specification does not allow cross-origin or data: URL resource requests for a use element's reference.
- [ ] svg2/struct.md#UseElementHrefAttribute: an href without a fragment identifier refers to the root element of the referenced document.

## Style scoping inside the use shadow tree: use-geometry-instance (exists)

- [ ] svg2/struct.md#UseStyleInheritance: elements in the use shadow tree inherit from the host use element; outer-document style rules do not match shadow-tree elements directly, only the shadow tree's own cloned stylesheets are matched against them.
- [ ] svg2/struct.md#UseStyleInheritance: presentation attributes and the style attribute are cloned onto each element instance like any other attribute.
- [ ] svg2/struct.md#UseStyleInheritance: a complex selector that matched an original element via its ancestors or siblings will not match the corresponding element instance when those ancestors/siblings were not themselves cloned into the shadow tree.
- [ ] svg2/struct.md#UseStyleInheritance: visibility:hidden on a use element does not guarantee the referenced content is hidden, since visibility does not apply directly to container elements; an explicit visibility:visible on a shadow-tree descendant still renders that descendant.

## The switch element: switch-conditions (exists)

- [ ] svg2/struct.md#SwitchElement: switch evaluates requiredExtensions/systemLanguage on its direct children in document order and renders only the first child for which all such tests are true; if none match, switch renders nothing.
- [ ] svg2/struct.md#SwitchElement: if the selected child is a container, its whole subtree is rendered; unselected children's subtrees are not rendered at all.
- [ ] svg2/struct.md#SwitchElement: the display and visibility properties have no effect on switch's true/false selection test.
- [ ] svg2/struct.md#SwitchElement: switch does not affect the processing of script or style elements.
- [ ] svg2/struct.md#ConditionalProcessingOverview: requiredFeatures, a conditional-processing attribute from SVG 1.1, is not defined in SVG 2 and has no effect.

## Conditional processing attributes: switch-conditions (exists)

- [ ] svg2/struct.md#ConditionalProcessingOverview: an element excluded by conditional processing is treated as if display computed to none: it blocks only direct rendering, not the element being referenced elsewhere (e.g. via use).
- [ ] svg2/struct.md#ConditionalProcessingOverview: conditional processing has no effect on never-rendered elements (style and script are unaffected), but it still determines whether child content of a never-rendered container (e.g. pattern, mask) contributes to that container's graphical effect.
- [ ] svg2/struct.md#ConditionalProcessingRequiredExtensionsAttribute: requiredExtensions evaluates true only if every listed extension URL is supported, otherwise the element and its children are skipped; the attribute is absent evaluates true; an empty-string value evaluates false.
- [ ] svg2/struct.md#ConditionalProcessingSystemLanguageAttribute: systemLanguage evaluates true if a user-preferred language tag case-insensitively matches, or is a case-insensitive prefix of (followed by '-'), one of its comma-separated tags; absent evaluates true; an empty-string value evaluates false.

## Foreign content and element identity: xml-foreign (exists)

- [ ] svg2/struct.md#ForeignNamespaces: unknown foreign-namespace elements are kept in the document but ignored and excluded from rendering (other than foreignObject content).
- [ ] svg2/struct.md#ForeignNamespaces: unknown attributes, including ones from foreign namespaces, are kept in the document but otherwise ignored for rendering.
- [ ] svg2/struct.md#IDAttribute: the id attribute must be unique within the node tree, must not be an empty string, and must not contain whitespace characters.

## Notes

