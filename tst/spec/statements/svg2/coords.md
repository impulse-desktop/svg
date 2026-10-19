# SVG 2, Chapter 8: Coordinate Systems, Transformations and Units

## Establishing a new SVG viewport: viewport-nested, viewport-content-percent

- [x] svg2/coords.md#TermSVGViewport: every SVG viewport has a size (width, height) and an origin, measured in user units. [viewport-nested#clip viewport-nested#symbol-viewport]
- [x] svg2/coords.md#TermCanvas: all SVG viewports are embedded in the canvas, a drawing region infinitely large in every relevant dimension. [viewport-nested#overflow-visible]
- [x] svg2/coords.md#EstablishingANewSVGViewport: an 'svg' element inside content creates a new SVG viewport, viewport coordinate system and user coordinate system, and a new meaning for percentage units within it. [viewport-nested#percent]
- [x] svg2/coords.md#EstablishingANewSVGViewport: the new viewport's bounds come from the 'x','y','width','height' attributes of the element establishing it; both new coordinate systems originate at (x,y). [viewport-nested#clip viewport-nested#overflow-visible]
- [x] svg2/coords.md#EstablishingANewSVGViewport: the new coordinate systems' orientation and unit size match the current user coordinate system of the element establishing the viewport. [viewport-nested#clip viewport-nested#transform]
- [x] svg2/coords.md#ElementsThatEstablishViewports: only the 'svg' element, and a 'symbol' element instanced by 'use', establish new SVG viewports. [viewport-nested#symbol-viewport viewport-content-percent#pattern viewport-content-percent#marker]
- [x] svg2/coords.md#ElementsThatEstablishViewports: 'pattern' and 'marker' accept a viewBox but establish no new viewport, nor do 'clipPath' or 'mask'; percentage lengths in their content are not proportional to the effect region. [viewport-content-percent#pattern viewport-content-percent#marker viewport-content-percent#clip-path viewport-content-percent#mask]
- [x] svg2/coords.md#ElementsThatEstablishViewports: an 'image' element establishes a new document viewport for the referenced document, which gets its own SVG viewport if it is SVG. [image-fragment-views#no-fragment image-fragments#view-element]
- [x] svg2/coords.md#ElementsThatEstablishViewports: whether a new SVG viewport also clips its content is decided by the 'overflow' property on the viewport-establishing element. [viewport-nested#clip viewport-nested#overflow-visible]

## The initial viewport: root-viewport

- [x] svg2/coords.md#ViewportSpace: the initial viewport's width is the 'width' presentation attribute on the outermost svg element, unless CSS positioning properties on a host document establish it instead. [root-viewport#user-units-px]
- [x] svg2/coords.md#ViewportSpace: likewise the initial viewport's height is the 'height' presentation attribute on the outermost svg element, unless host CSS positioning properties establish it. [root-viewport#user-units-px]
- [x] svg2/coords.md#ViewportSpace: a 'width' or 'height' on the outermost svg element given in user units (no unit identifier) is assumed equivalent to the same number of px units. [root-viewport#user-units-px]
- [x] svg2/coords.md#InitialCoordinateSystem: the outermost svg element's initial viewport and user coordinate systems are identical, origin at the viewport's origin, x pointing right, y pointing down, one unit equal to one CSS px. [root-viewport#y-down]

## Computing the viewBox transform: viewbox-align

- [x] svg2/coords.md#ComputingAViewportsTransform: scale-x and scale-y start as element-width/viewBox-width and element-height/viewBox-height. [viewbox-align#none-commas viewbox-align#meet-xMinYMin]
- [x] svg2/coords.md#ComputingAViewportsTransform: if align is not none and meetOrSlice is meet, the larger of scale-x/scale-y is set to the smaller (uniform scale, fits inside). [viewbox-align#meet-xMinYMin viewbox-align#meet-xMidYMid viewbox-align#meet-xMaxYMax]
- [x] svg2/coords.md#ComputingAViewportsTransform: if align is not none and meetOrSlice is slice, the smaller of scale-x/scale-y is set to the larger (uniform scale, covers viewport). [viewbox-align#slice-xMinYMin viewbox-align#slice-xMidYMid viewbox-align#slice-xMaxYMax]
- [x] svg2/coords.md#ComputingAViewportsTransform: translate-x/translate-y start at e-x - vb-x*scale-x and e-y - vb-y*scale-y; the xMid/xMax and yMid/yMax align keywords then add half or all of the leftover (e-size - vb-size*scale) to the translation. [viewbox-align#meet-xMinYMid viewbox-align#meet-xMidYMid viewbox-align#meet-xMaxYMax viewbox-align#slice-xMidYMin viewbox-align#slice-xMaxYMin viewbox-align#slice-xMidYMid viewbox-align#slice-xMaxYMax]
- [x] svg2/coords.md#PreserveAspectRatioAttribute: preserveAspectRatio's default value is 'xMidYMid meet'; its align keyword is none or one of the 9 xMin/xMid/xMax times YMin/YMid/YMax combinations, meetOrSlice is meet (default) or slice. [viewbox-align#default viewbox-align#meet-xMinYMax viewbox-align#slice-xMaxYMid]
- [x] svg2/coords.md#DataTypeAlign: align=none forces non-uniform scaling to exactly fill the viewport and makes meetOrSlice ignored. [viewbox-align#none-commas]
- [x] svg2/coords.md#PreserveAspectRatioAttribute: preserveAspectRatio applies only to viewport-establishing elements plus 'image', 'marker', 'pattern', 'view', and only when the same element also has a 'viewBox'; otherwise it is ignored. [viewbox-align#aspect-without-viewbox]

## viewBox grammar and its interaction with transform: viewport-nested, viewbox-align, transform-functions

- [x] svg2/coords.md#ViewBoxAttribute: viewBox is a list of four numbers min-x, min-y, width, height, separated by whitespace and/or a comma. [viewbox-align#none-commas viewbox-align#meet-xMinYMin]
- [x] svg2/coords.md#ViewBoxAttribute: a negative width or height is an error and invalidates the whole viewBox attribute. [viewport-nested#negative-viewbox]
- [x] svg2/coords.md#ViewBoxAttribute: a width or height of zero disables rendering of the element. [viewport-nested#zero-viewbox]
- [x] svg2/coords.md#TransformProperty: the 'transform' property and presentation attribute on SVG elements must be supported as defined by CSS Transforms 1. [transform-functions#attribute-functions transform-functions#matrix transform-functions#rotate]
- [x] svg2/coords.md#ViewBoxAttribute: when both transform and viewBox apply to an element, two new coordinate systems result: transform's for the element itself, then viewBox's for its descendants, with transform's post-multiplied by viewBox's. [viewport-nested#transform]
- [x] svg2/coords.md#ViewBoxAttributeEffectOnSiblingAttributes: viewBox's automatic transform does not affect the element's own x, y, width, height (or a marker's markerWidth/markerHeight), though it does establish a new coordinate system for descendants and other attributes. [viewport-nested#percent marker-viewport#viewbox-fit]

## Percentage length resolution: viewport-nested, stroke-width-values

- [x] svg2/coords.md#Units: SVG attributes, presentation attributes and properties use the common value types of the CSS Values and Units module. [css-length-units#absolute css-length-units#percent]
- [x] svg2/coords.md#Units: percentages resolve against the user coordinate system of the nearest ancestral viewport-defining element; nesting viewports redefines what percentages mean inside them. [viewport-nested#percent viewport-nested#symbol-viewport]
- [x] svg2/coords.md#Units: an x-coordinate or width percentage of the SVG viewport resolves against the viewBox width if one applies, else the viewport's own width. [viewport-nested#percent]
- [x] svg2/coords.md#Units: a y-coordinate or height percentage of the SVG viewport resolves against the viewBox height if one applies, else the viewport's own height. [viewport-nested#percent]
- [x] svg2/coords.md#Units_viewport_percentage: any other length's percentage of the SVG viewport resolves against the normalized diagonal sqrt(width²+height²)/sqrt(2) of the viewBox if one applies, else of the viewport itself. [viewport-nested#percent stroke-width-values#percent]

## Bounding box computation: bbox-geometry, bbox-inputs

- [x] svg2/coords.md#TermBoundingBox: an element's bounding box is the tightest axis-aligned (in its own user coordinate system) rectangle enclosing it and its descendants. [bbox-geometry#curve-box]
- [x] svg2/coords.md#TermObjectBoundingBox: the object bounding box contains only the fill geometry and, unqualified, "bounding box" means this one; the stroke bounding box adds the stroke shape (as if undashed); the decorated bounding box further adds markers. [bbox-geometry#group-children stroke-bounding-box#miter stroke-bounding-box#group]
- [x] svg2/coords.md#TermBoundingBox: opacity, visibility, fill, fill-opacity, fill-rule, stroke-dasharray and stroke-dashoffset have no effect on an element's bounding box. [bbox-inputs#visibility-hidden bbox-inputs#opacity-zero bbox-inputs#fill-none bbox-inputs#dashed]
- [x] svg2/coords.md#TermBoundingBox: for curved shapes the bounding box must enclose the whole curve edge, not just its endpoints; a Bézier control point not lying on the curve must not enlarge the box, even though it may fall inside it. [bbox-geometry#curve-box]
- [x] svg2/coords.md#TermBoundingBox: an element outside the rendering tree (display:none, inside defs, an uninstanced symbol) still has its own bounding box but contributes nothing to an ancestor's. [bbox-geometry#group-children]
- [x] svg2/coords.md#TermBoundingBox: a zero-width, zero-height, or both-zero element still has a bounding box, with 0 used for each zero dimension. [bbox-geometry#zero-size-child]
- [x] svg2/coords.md#TermBoundingBox: a 'path' with d:none has no position and is treated as positioned at (0,0) for its bounding box. [path-data-commands#none]
- [-] svg2/coords.md#TermBoundingBox: an element referencing an unresolved resource still has a bounding box, from its own geometry attributes or their initial values (e.g. use href="#bad" x="10" y="10" has bbox x=10,y=10,w=0,h=0). (D37) [bbox-inputs#unresolved-use bbox-inputs#unresolved-paint]
- [x] svg2/coords.md#ContainerElementBoudingBoxComputation: a container element's (or use's shadow tree's) bounding box is the union of its rendered children's boxes, each computed with the same fill/stroke/markers/clipped inputs. [bbox-geometry#group-children bbox-geometry#zero-size-child]
- [x] svg2/coords.md#ContainerElementBoudingBoxComputation: when clipped is true, the container's box is further intersected with its own clip-path, and then its overflow bounds and 'clip' rectangle if they apply. [bbox-inputs#child-clip]
- [x] svg2/coords.md#BoundingBoxes: for 'foreignObject' and 'image', the bounding box is the positioning rectangle given by x, y, width, height, regardless of the fill/stroke/markers inputs. [bbox-inputs#image-box]
- [x] svg2/coords.md#BoundingBoxes: unioning a box with an empty shape at (0,0,0,0) leaves the box unchanged. [path-data-commands#none]

## Object bounding box units: bbox-geometry, bbox-inputs, mask-values

- [x] svg2/coords.md#ObjectBoundingBox: setting gradientUnits, patternUnits/patternContentUnits, clipPathUnits, maskUnits/maskContentUnits or filterUnits/primitiveUnits to objectBoundingBox makes their coordinates fractions of the applicable element's object bounding box. [bbox-geometry#fractions bbox-geometry#group-children gradient-linear-geometry#defaults pattern-tiles#box-fractions mask-values#content-units]
- [x] svg2/coords.md#ObjectBoundingBoxUnits: objectBoundingBox acts as an extra transform mapping (0,0) to the box's (minx,miny) corner and (1,1) to its (maxx,maxy) corner: matrix [(maxx-minx) 0 0 (maxy-miny) minx miny]. [bbox-geometry#fractions]
- [x] svg2/coords.md#ObjectBoundingBoxUnits: a percentage used with these attributes equals the same decimal (50% = 0.5); percentages inside the referencing element's own content instead follow the ordinary Units percentage rules. [bbox-inputs#gradient-percent bbox-inputs#pattern-percent]
- [x] svg2/coords.md#ObjectBoundingBoxUnits: fractions or percentages below 0 or above 1 (100%) are permitted and not clamped. [bbox-geometry#fractions]
- [x] svg2/coords.md#ObjectBoundingBoxUnits: objectBoundingBox must not be used, and its effect is ignored, when the applicable element's geometry has no width or no height (stroke width is ignored for this check). [bbox-geometry#zero-height-gradient bbox-geometry#zero-height-filter bbox-geometry#zero-height-clip]

## Intrinsic size of SVG as a replaced image: root-viewport

- [-] svg2/coords.md#SizingSVGInCSS: the concrete object size of SVG content follows the CSS Images 3 Default Sizing Algorithm, with specified/intrinsic sizes taken from the svg element's used width/height. (D10) [root-viewport#user-units-px image-fragment-views#no-fragment]
- [-] svg2/coords.md#SizingSVGInCSS: an unspecified width or height's used value is the initial 'auto'; 'auto' and percentage lengths are never used to determine an intrinsic width or height. (D10) [root-viewport#user-units-px image-fragment-views#no-fragment]
- [-] svg2/coords.md#SizingSVGInCSS: unlike a raster image, SVG's intrinsic width/height adapt to equal the specified size rather than staying fixed. (D10) [root-viewport#user-units-px image-fragment-views#no-fragment]
- [-] svg2/coords.md#SizingSVGInCSS: the intrinsic aspect ratio is width/height if both are absolute lengths; else the active SVG view's viewBox ratio if any; else the svg element's own correctly-specified viewBox ratio; else there is none. (D10) [root-viewport#user-units-px image-fragment-views#no-fragment]
- [-] svg2/coords.md#SizingSVGInCSS: wherever an intrinsic aspect ratio exists it must be respected when sizing the SVG viewport, in every host context. (D10) [root-viewport#user-units-px image-fragment-views#no-fragment]

## Vector effects: vector-effect-transforms, stroke-non-scaling

- [x] svg2/coords.md#VectorEffectProperty: vector-effect is none | non-scaling-stroke | non-scaling-size | non-rotation | fixed-position (combinable), initial none, applies to graphics elements and 'use', not inherited. [vector-effect-transforms#non-scaling-size stroke-non-scaling#not-inherited]
- [x] svg2/coords.md#VectorEffectProperty: non-scaling-size keeps the user coordinate system's scale fixed against CTM changes from the host coordinate space, without suppressing rotation or skew; it implies non-scaling-stroke too. (D3) [vector-effect-transforms#non-scaling-size vector-effect-transforms#fixed-non-scaling-size]
- [x] svg2/coords.md#VectorEffectProperty: non-rotation suppresses rotation and skew of the user coordinate system against CTM changes, without suppressing scaling. [vector-effect-transforms#non-rotation vector-effect-transforms#fixed-non-rotation]
- [x] svg2/coords.md#VectorEffectProperty: fixed-position fixes the placement of the user coordinate system against CTM changes; it consumes the element's own transform property, using its e/f translation components as the fixed origin. [vector-effect-transforms#fixed-position vector-effect-transforms#fixed-both]
- [x] svg2/coords.md#VectorEffects: the host coordinate space for vector-effect is the viewport coordinate system of the furthest ancestral SVG viewport. (D3) [stroke-non-scaling#nested-viewbox vector-effect-transforms#fixed-position]
- [-] svg2/coords.md#VectorEffectsCalculation: vector-effect has no effect on transforms performed within a 3D rendering context. (D38) [vector-effect-transforms#non-scaling-size vector-effect-transforms#both]
- [x] svg2/coords.md#VectorEffectsCalculation: when |det(CTM)| is 0 and non-scaling-size is specified, the vector-effect becomes invalid, i.e. none. [vector-effect-transforms#no-area]

## Notes

- svg2/coords.md#issue31: values of vector-effect other than none and non-scaling-stroke are marked by the spec itself as at risk of being dropped for lack of implementations.
- svg2/coords.md#TermDecoratedBoundingBox: an exact curve-enclosing bounding box needs each curve's true per-axis extrema, not just its control points' hull; costly to compute exactly for every stroked/filled curve.
