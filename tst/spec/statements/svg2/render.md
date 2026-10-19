# SVG 2, Chapter 3: Rendering Model

## Which elements are not rendered: render-not-rendered, render-display-visibility

- [x] svg2/render.md#TermNeverRenderedElement: clipPath, defs, desc, linearGradient, marker, mask, metadata, pattern, radialGradient, script, style and title are never directly rendered regardless of display; so is a symbol that is not the instance root of a use-element shadow tree. [render-not-rendered#not-rendered]
- [x] svg2/render.md#Rendered-vs-NonRendered: an element is not rendered if it is a never-rendered type, excluded by conditional-processing/switch, has computed display:none, is detached from the DOM, or is a descendant of another non-rendered element. [render-not-rendered#not-rendered resource-display#conditional]
- [x] svg2/render.md#Rendered-vs-NonRendered: a non-rendered element has no visual effect except when referenced by another element's rendering; it does not contribute to clipPath/mask geometry or to bounding-box calculations, but it still participates in style cascade and inheritance. [resource-display#clip resource-display#mask resource-display#filter]

## Controlling visibility: display and visibility: render-display-visibility, render-not-rendered

- [x] svg2/render.md#VisibilityControl: display:none removes a renderable element and all its descendants from the rendering tree; display is not an inherited property. [render-not-rendered#not-rendered]
- [x] svg2/render.md#VisibilityControl: display only applies to renderable elements; setting display:none on an already never-rendered or conditionally-excluded element has no effect. [render-not-rendered#not-rendered resource-display#mask]
- [x] svg2/render.md#VisibilityControl: visibility:hidden or collapse on a graphics or use element stops it being painted but it stays in the rendering tree: it still contributes to bounding box and clipping-path geometry. (D14) [render-display-visibility#hidden-bbox]
- [x] svg2/render.md#VisibilityControl: visibility only directly affects graphics and text-content elements, but being an inherited property its value still reaches descendants of a use or container element. [render-display-visibility#visibility use-shadow-styles#visibility]

## Shadow DOM reuse via use: render-groups, symbol-geometry, use-shadow-styles

- [x] svg2/render.md#ReusedGraphics: a use element's shadow-DOM content renders as if the use element were a container and the shadow content its descendants; style and geometry on shadow elements resolve independently of the corresponding source element. [use-shadow-styles#inherit-from-use use-geometry#translate-after]
- [x] svg2/render.md#ReusedGraphics: for blending purposes a use element forms a non-isolated group. [render-groups#non-isolated]
- [x] svg2/render.md#ReusedGraphics: display has its normal effect on shadow-DOM elements, except for the special rules that apply to a symbol element. [symbol-geometry#display-none-symbol]
- [x] svg2/render.md#TermRenderableElement: a symbol that is the instance root of a use-element shadow tree is a renderable element, so a transformable one (css-transforms-1.md#transformable-element): its transform applies inside the use's x and y, about its origin. [symbol-geometry#transform]

## Graphical-effect content as a self-contained fragment: render-groups, resource-display

- [x] svg2/render.md#ReusedGraphics: content reused as a graphical effect (marker, paint server, clipPath, mask) is rendered independently as its own stacking context and isolated group; the flattened result is treated as a vector image when composited with other paint layers. [render-groups#marker-isolated]
- [x] svg2/render.md#ReusedGraphics: display:none on child content of such an effect element excludes that subtree, but display:none on the effect element itself (or an ancestor) does not alter the effect. [resource-display#clip resource-display#mask resource-display#filter]

## Establishing a stacking context: render-groups

- [x] svg2/render.md#EstablishingStackingContex: a new stacking context is established at the root element; at an outermost svg, foreignObject, image, marker, mask, pattern, symbol or use element; at an inner svg whose computed overflow is not visible; at an element with computed clip other than auto, clip-path other than none, opacity other than 1, mask other than none, or filter other than none; or by a property from another spec defined to do so. [render-groups#marker-isolated render-groups#isolated]
- [x] svg2/render.md#EstablishingStackingContex: each element belongs to exactly one stacking context and is stacked in document order within it; a stacking context is atomic from its parent's point of view (ancestor-context elements cannot interleave with its contents). [paint-order-values#normal render-groups#group-opacity]
- [x] svg2/render.md#EstablishingStackingContex: except for foreignObject, the back-to-front order within a stacking context is: the element's own background/borders, then its descendants in tree order. [paint-order-values#normal]

## Rendering a compositing group (isolated vs non-isolated): render-groups, blend-isolation (exists)

- [x] svg2/render.md#Elements: an individual graphics element is itself treated as a non-isolated group of its fill, stroke and other paint components. [render-groups#non-isolated]
- [x] svg2/render.md#Grouping: applying opacity, a filter or a mask to a group makes that group isolated; such effects are applied to the group's rendered result before any transform on the group, which is applied before the group is blended with its backdrop. [render-groups#isolated blend-groups#opacity blend-groups#filter blend-groups#mask]
- [x] svg2/render.md#Grouping: an isolated group is rendered onto a fresh rgba(0,0,0,0) backdrop, its graphics/g children painted onto it in rendering order, then filters (in the filter operating coordinate space) are applied, then the group transform, then the result is blended with the group's own backdrop. [render-groups#isolated blend-groups#isolate]
- [x] svg2/render.md#Grouping: a non-isolated group paints its children directly onto the backdrop it inherits from its parent, in rendering order, applying the group's transform to each child as it is rendered. [render-groups#non-isolated]

## Object and group opacity: render-groups, opacity (exists)

- [x] svg2/render.md#ObjectAndGroupOpacityProperties: the opacity property renders the element or group to an offscreen RGBA image, then blends that whole image into the canvas with the opacity value applied uniformly; this is why opacity always isolates the group. [render-groups#group-opacity opacity-values#group]
- [x] svg2/render.md#ObjectAndGroupOpacityProperties: opacity applies to svg, g, symbol, marker, a, switch, use and graphics elements. [render-groups#group-opacity opacity-values#values blend-root#root-group]

## Filtering a paint operation: filter-root (exists)

- [x] svg2/render.md#FilteringPaintRegions: a filtered paint operation behaves as though the paint had first been applied to an intermediate canvas initialized to transparent black, which is then filtered. [filter-region#default-region filter-background-image#isolated]

## Root svg compositing against the page: render-root-transform

- [x] svg2/render.md#ParentCompositing: the svg element always forms an isolated group. [blend-root#root-group]
- [-] svg2/render.md#ParentCompositing: a root svg that is a top-level (non-embedded) document is the page group and is composited over an opaque white backdrop. (D13) [blend-root#root-group]
- [x] svg2/render.md#ParentCompositing: an embedded SVG document or fragment is instead composited into its parent document with its own opacity preserved. [image-self#self-copy]

## The overflow property: marker-viewport, viewport-nested, symbol-geometry

- [x] svg2/render.md#OverflowAndClipProperties: overflow's initial value is auto, but the UA style sheet sets it to hidden by default for a non-root svg, image, marker, pattern and symbol element (so visible content is clipped to the viewport unless overridden). [viewport-nested#clip symbol-geometry#overflow marker-viewport#overflow-hidden]
- [x] svg2/render.md#OverflowAndClipProperties: overflow:visible has no effect: no clipping rectangle is created. [viewport-nested#overflow-visible marker-viewport#overflow-visible]
- [x] svg2/render.md#OverflowAndClipProperties: overflow:hidden or scroll clips to a rectangle exactly the size of the SVG viewport. [marker-viewport#overflow-hidden marker-viewport#overflow-scroll]
- [x] svg2/render.md#OverflowAndClipProperties: overflow:auto on SVG content requires all child content to render visibly (via scrolling or no clip); if there is no scrolling mechanism, or scroll would be treated as hidden, auto must instead be treated as visible. [marker-viewport#overflow-auto]

## Shape and text painting order: paint-order-values

- [x] svg2/render.md#PaintingShapesAndText: a shape or text can be filled and stroked; a marker symbol used on it is painted as if its content were expanded into the document tree just after the element using it. [paint-order-values#normal marker-vertices#rect]
- [x] svg2/render.md#PaintingShapesAndText: by default fill is painted first, then stroke, then marker symbols (in order along the shape from start to end); the paint-order property can change this order. [paint-order-values#normal paint-order-values#stroke-first paint-order-values#markers-first]
- [x] svg2/render.md#PaintingShapesAndText: fill and stroke are independent operations, each with its own opacity. [paint-opacity-values#fill-values paint-opacity-values#stroke-values]

## Notes

