# SVG 2, Chapter 13: Painting: Filling, Stroking and Marker Symbols

## Paint value syntax: paint-fallback, paint-current-color, paint-context, color-functions, color-keywords

- [x] svg2/painting.md#DataTypePaint: <paint> = none | <color> | <url> [none | <color>]? | context-fill | context-stroke [paint-fallback#resolves paint-fallback#missing-fallback paint-current-color#fill paint-context#marker-fill]
- [x] svg2/painting.md#DataTypePaint: the <url> references a paint server element (linearGradient, pattern, radialGradient), optionally followed by a fallback <color> or none [paint-fallback#resolves paint-fallback#missing-fallback]
- [x] svg2/painting.md#DataTypePaint: the fallback is used only if the paint server reference cannot be resolved (points to a nonexistent element or one that is not a valid paint server) [paint-fallback#resolves paint-fallback#not-server use-symbol-context#missing-server]
- [x] svg2/painting.md#DataTypePaint: if a paint server reference is invalid and no fallback is given, no paint is rendered for that layer [paint-fallback#no-fallback]
- [x] svg2/painting.md#DataTypePaint: for any <color>, CSS Color Module 3 syntaxes (rgb(), rgba(), hsl(), hsla(), extended keywords, currentColor) must be supported [color-functions#rgb-forms color-functions#legacy color-functions#hsl-forms color-functions#named color-keywords#currentcolor]

## Context element paint: paint-context, use-symbol-context

- [x] svg2/painting.md#context-paint: context-fill and context-stroke reference the fill/stroke paint layers generated for the context element [paint-context#marker-stroke paint-context#marker-fill pattern-program-reuse#marker-fill pattern-program-reuse#marker-stroke]
- [x] svg2/painting.md#context-paint: the context element is the element referencing a marker (when painting inside that marker) or the use element (when inside a use shadow tree); otherwise there is no context element [paint-context#marker-fill paint-context#use-fill pattern-program-reuse#use-fill pattern-program-reuse#use-stroke]
- [x] svg2/painting.md#context-paint: if there is no context element and context-fill/context-stroke are used, no paint is applied for that layer [paint-context#no-context]
- [x] svg2/painting.md#context-paint: the coordinate space and bounding box used for context paint servers are those of the context element, so gradients/patterns stay continuous from shape to marker or across the use shadow tree [paint-context#marker-server-box paint-context#use-server-box paint-context#use-box-measured use-symbol-context#gradient use-symbol-context#pattern use-symbol-context#stroke use-symbol-context#whole-instance use-symbol-context#viewbox use-symbol-context#sized-percent use-symbol-context#default-size use-symbol-context#reference-point use-symbol-context#target-transform use-symbol-context#nested-sizes use-symbol-context#svg-size use-symbol-context#svg-default-size use-symbol-context#empty-gradient-box use-symbol-context#empty-pattern-box use-symbol-context#empty-mesh-box]
- [x] svg2/painting.md#context-paint: context-fill/context-stroke references are resolved recursively if the context element's own fill/stroke is itself context-fill/context-stroke [paint-context#use-recursive]

## color property currentColor: paint-current-color

- [x] svg2/painting.md#ColorProperty: the color property supplies the currentColor indirect value for fill, stroke, stop-color, flood-color and lighting-color only; it has no other effect on SVG elements [paint-current-color#fill paint-current-color#stroke paint-current-color#stop-color paint-current-color#flood-color paint-current-color#lighting-color]

## fill area: fill-open-subpath, paint-properties

- [x] svg2/painting.md#FillProperty: fill's initial value is black; applies to shapes and text content elements; is inherited [paint-properties#fill-initial paint-properties#fill-inherited]
- [x] svg2/painting.md#FillProperty: the painted area is everywhere inside the outline per the fill-rule; the zero-width geometric outline itself is included in the area painted [fill-open-subpath#polyline fill-rule-crossings#nonzero-same]
- [x] svg2/painting.md#FillProperty: fill treats each open subpath as if an extra closepath were added, so fill applies to open subpaths of path elements and to polyline elements the same as to closed ones [fill-open-subpath#polyline fill-open-subpath#path]

## fill-rule: fill-rule-crossings

- [x] svg2/painting.md#FillRuleProperty: fill-rule is nonzero | evenodd; initial nonzero; applies to shapes and text content elements; is inherited [fill-rule-crossings#nonzero-same fill-rule-crossings#evenodd-same]
- [x] svg2/painting.md#FillRuleProperty: nonzero sums signed ray crossings (+1 left-to-right, -1 right-to-left) from the point to infinity; zero sum is outside, nonzero is inside [fill-rule-crossings#nonzero-same fill-rule-crossings#nonzero-opposite fill-rule-crossings#nonzero-pentagram raster-nan-band-first#nonzero raster-nan-band#nonzero]
- [x] svg2/painting.md#FillRuleProperty: evenodd counts ray crossings; an odd count is inside, an even count is outside [fill-rule-crossings#evenodd-same fill-rule-crossings#evenodd-pentagram]
- [x] svg2/painting.md#FillRuleProperty: a ray tangent to or coincident with a path segment may be replaced by a different ray that avoids the problem intersection [fill-rule-crossings#nonzero-pentagram fill-rule-crossings#evenodd-pentagram]

## fill-opacity: paint-opacity-values

- [x] svg2/painting.md#FillOpacityProperty: fill-opacity is <number> | <percentage>; initial 1; applies to shapes and text content elements; is inherited; computed value is clamped to [0,1] [paint-opacity-values#fill-values paint-opacity-values#fill-clamp paint-properties#fill-opacity-inherited]
- [x] svg2/painting.md#FillOpacityProperty: values outside 0 (fully transparent) to 1 (fully opaque) must be clamped to that range; a percentage is of the 0-1 range [paint-opacity-values#fill-clamp paint-opacity-values#fill-values]

## stroke basics: stroke-basics, stroke-linecap-shapes, paint-opacity-values, paint-properties

- [x] svg2/painting.md#StrokeProperties: stroking properties affected by directionality (e.g. dash patterns) render starting at the same point the graphics element starts (for path, the first moveto point) [stroke-basics#rect-start stroke-basics#circle-start stroke-basics#ellipse-start stroke-basics#line-start]
- [x] svg2/painting.md#StrokeProperties: distance-along-the-outline computations for stroking use the standard distance-along-a-path algorithm [stroke-dasharray-values#odd-list path-direction#curve-tangent]
- [x] svg2/painting.md#StrokeProperties: stroking with a gradient or pattern paint server must equal filling the equivalent path (the element's geometry plus its stroking properties) with that same paint server [stroke-basics#server-box]
- [x] svg2/painting.md#StrokeProperty: stroke's initial value is none; applies to shapes and text content elements; is inherited [stroke-basics#none-default paint-properties#stroke-inherited]
- [x] svg2/painting.md#StrokeProperty: a subpath consisting of a moveto with no following line-drawing command is never stroked; any other zero-length subpath is not stroked when stroke-linecap is butt [stroke-linecap-shapes#moveto-alone stroke-linecap-shapes#zero-butt]
- [x] svg2/painting.md#StrokeOpacityProperty: stroke-opacity is defined exactly like fill-opacity (value, initial 1, clamped to [0,1]) but for the stroke paint [paint-opacity-values#stroke-values paint-opacity-values#stroke-clamp paint-properties#stroke-opacity-inherited]

## stroke-width: stroke-width-values

- [x] svg2/painting.md#StrokeWidthProperty: stroke-width is <length-percentage> | <number>; initial 1px; applies to shapes and text content elements; is inherited; percentages refer to the normalized diagonal of the current viewport [stroke-width-values#percent stroke-width-values#units stroke-width-values#em paint-properties#stroke-width-initial paint-properties#stroke-width-inherited stroke-percent-inherit#nested-width stroke-percent-inherit#group-width stroke-percent-inherit#calc-width stroke-percent-inherit#calc-em stroke-percent-inherit#inherit-keyword]
- [x] svg2/painting.md#StrokeWidthProperty: a zero stroke-width causes no stroke to be painted [stroke-width-values#zero]
- [x] svg2/painting.md#StrokeWidthProperty: a negative stroke-width is invalid [stroke-width-values#negative]
- [x] svg2/painting.md#StrokeWidthProperty: a <number> value is in user units [stroke-width-values#user-units]

## stroke-linecap: stroke-linecap-shapes

- [x] svg2/painting.md#StrokeLinecapProperty: stroke-linecap is butt | round | square; initial butt; applies to shapes and text content elements; is inherited; also controls the shape drawn for zero-length subpaths [stroke-linecap-shapes#caps paint-properties#linecap-initial paint-properties#linecap-inherited]
- [x] svg2/painting.md#StrokeLinecapProperty: butt extends the stroke no further than the endpoints, so a zero-length subpath with butt gets no stroke at all [stroke-linecap-shapes#caps stroke-linecap-shapes#zero-butt]
- [x] svg2/painting.md#StrokeLinecapProperty: round extends each end by a half circle of diameter stroke-width; a zero-length subpath with round draws a full circle centered at its point [stroke-linecap-shapes#caps stroke-linecap-shapes#zero-round]
- [x] svg2/painting.md#StrokeLinecapProperty: square extends each end by a rectangle of width stroke-width and length stroke-width/2 oriented to the tangent; a zero-length subpath with square draws a square of side stroke-width oriented by the tangent at that point [stroke-linecap-shapes#caps stroke-linecap-shapes#zero-square stroke-linecap-shapes#zero-direction]
- [x] svg2/painting.md#TermCapShape: the starting/ending cap shape is a semicircle or rectangle (per linecap) positioned with its straight edge perpendicular to the subpath at the cap's position, centered on the point on the subpath at that distance [stroke-linecap-shapes#caps stroke-linecap-shapes#zero-direction stroke-dash-positions#negative-offset]

## stroke-linejoin and stroke-miterlimit: stroke-linejoin-shapes, paint-properties

- [x] svg2/painting.md#StrokeLinejoinProperty: stroke-linejoin is miter | miter-clip | round | bevel | arcs; initial miter; applies to shapes and text content elements; is inherited [stroke-linejoin-shapes#miter stroke-linejoin-shapes#round stroke-linejoin-shapes#bevel stroke-linejoin-shapes#miter-clip paint-properties#linejoin-inherited]
- [x] svg2/painting.md#StrokeLinejoinProperty: miter extends the outer stroke edges to their intersection; if the miter limit is exceeded it falls back to bevel [stroke-linejoin-shapes#miter-under-limit stroke-linejoin-shapes#bevel-over-limit stroke-linejoin-shapes#reverse-miter]
- [x] svg2/painting.md#StrokeLinejoinProperty: miter-clip is as miter, but when the limit is exceeded it clips the miter at half the miterlimit times stroke-width from the segment intersection, instead of falling back to bevel [stroke-linejoin-shapes#miter-clip stroke-linejoin-shapes#reverse-miter-clip stroke-linejoin-shapes#miter-clip-under-one]
- [x] svg2/painting.md#StrokeLinejoinProperty: round draws a circular sector centered on the join point; bevel fills the triangle between the two stroked segment edges [stroke-linejoin-shapes#round stroke-linejoin-shapes#bevel stroke-linejoin-shapes#reverse-round]
- [x] svg2/painting.md#StrokeMiterlimitProperty: stroke-miterlimit is <number>; initial 4; applies to shapes and text content elements; is inherited; a negative value is invalid and must be ignored [stroke-linejoin-shapes#negative-limit paint-properties#miterlimit-inherited]
- [x] svg2/painting.md#math-miter-length: the miter length is stroke-width / sin(theta/2), theta the angle between the segments; once miter-length/stroke-width exceeds stroke-miterlimit, miter falls back to bevel and miter-clip clips at that limit [stroke-linejoin-shapes#miter-under-limit stroke-linejoin-shapes#bevel-over-limit stroke-linejoin-shapes#limit-under-one]
- [x] svg2/painting.md#TermLineJoinShape: no line-join shape is produced where the tangent at the end of a segment equals the tangent at the start of the next segment (e.g. two collinear same-direction line segments) [paint-properties#collinear stroke-tiny-smooth-corner#forward-0 stroke-tiny-smooth-corner#forward-1 stroke-tiny-smooth-corner#forward-2 stroke-tiny-smooth-corner#reverse-0 stroke-tiny-smooth-corner#reverse-1 stroke-tiny-smooth-corner#reverse-2]

## stroke-linejoin arcs: stroke-linejoin-shapes

- [-] svg2/painting.md#TermLineJoinShape: arcs extends the outer stroke edges with circles matching each segment's curvature at the join, then clips the result by the miter limit exactly as miter-clip does (D4) [stroke-linejoin-shapes#arcs-as-miter-clip]
- [x] svg2/painting.md#TermLineJoinShape: if both segment curvatures at the join are zero, arcs falls through to miter-clip [stroke-linejoin-shapes#arcs-as-miter-clip]
- [-] svg2/painting.md#TermLineJoinShape: if either curvature exceeds 2/stroke-width, arcs falls back to round (D4) [stroke-linejoin-shapes#arcs-as-miter-clip]
- [-] svg2/painting.md#CurvatureCalculation: a circle's radius from curvature κ is r=1/κ, then adjusted by plus-or-minus half the stroke width for the stroke edge; a straight segment (zero curvature) is extended by a line instead (D4) [stroke-linejoin-shapes#arcs-as-miter-clip]
- [-] svg2/painting.md#ArcsLinejoinFallback: when the two curvature circles do not intersect, both radii are changed by an equal amount (shrinking the enclosing one and growing the enclosed one, or growing both) until they touch (D4) [stroke-linejoin-shapes#arcs-as-miter-clip]
- [-] svg2/painting.md#ArcsLinejoinFallback: if the offset paths' tangents at the join are parallel so the circles can never be made to touch, the join is instead a rectangle of width stroke-width and length stroke-width times half of stroke-miterlimit (D4) [stroke-linejoin-shapes#arcs-as-miter-clip]

## stroke-dasharray and stroke-dashoffset: stroke-dasharray-values

- [x] svg2/painting.md#StrokeDasharrayProperty: stroke-dasharray is none | <dasharray>; initial none; applies to shapes and text content elements; is inherited; percentages refer to the normalized diagonal of the viewport [stroke-dasharray-values#percent paint-properties#dasharray-initial paint-properties#dasharray-inherited stroke-percent-inherit#dasharray]
- [x] svg2/painting.md#DataTypeDasharray: <dasharray> is a comma/white-space separated list of <number> | <length-percentage>; an odd-length list is repeated once to make it even [stroke-dasharray-values#odd-list stroke-dasharray-values#commas]
- [x] svg2/painting.md#DataTypeDasharray: the first and every second value after it is a dash length; every other value is a gap length; a <number> is in user units [stroke-dasharray-values#odd-list]
- [x] svg2/painting.md#DataTypeDasharray: a negative value anywhere in the list makes the whole stroke-dasharray value invalid [stroke-dasharray-values#negative-value]
- [x] svg2/painting.md#DataTypeDasharray: if every value in the list is zero, the stroke is rendered as a solid line with no dashing [stroke-dasharray-values#zero-sum]
- [x] svg2/painting.md#DataTypeDasharray: the pathLength attribute on a path element scales each dash/gap length relative to the author's declared path length [stroke-dasharray-values#path-length path-length-edges#moveto]
- [x] svg2/painting.md#StrokeDashoffsetProperty: stroke-dashoffset is <length-percentage> | <number>; initial 0; inherited; same percentage base as stroke-dasharray; also scaled by pathLength like stroke-dasharray [stroke-dasharray-values#offset stroke-dasharray-values#percent-offset stroke-dasharray-values#path-length paint-properties#dashoffset-inherited stroke-percent-inherit#dashoffset]
- [x] svg2/painting.md#math-dashoffset: a negative stroke-dashoffset is equivalent to offset d = sum − (abs(offset) mod sum), sum being the total of the dasharray values [stroke-dasharray-values#negative-offset stroke-dash-positions#negative-offset stroke-short-wide-dashes#horizontal-negative stroke-short-wide-dashes#vertical-negative]

## stroke shape: stroke-shape-union

- [x] svg2/painting.md#TermStrokeShape: the stroke shape is the union, for each subpath, of each dash's parallel-offset band, its start/end cap shapes, and the line-join shapes of the segments it spans [stroke-shape-union#miter-rect stroke-shape-union#round-rect stroke-shape-union#circle stroke-shape-union#bevel-triangle stroke-shape-union#curves stroke-shape-union#subpaths stroke-tight-ends#dash-end stroke-tight-ends#end-tight stroke-tight-ends#pie-sector stroke-folded-open#one-way-bend stroke-dash-dense#dense-zero-square stroke-dash-dense#sparse-zero-square stroke-offscreen-turn#extreme stroke-offscreen-turn#ordinary stroke-normal-witnesses#end-1 stroke-normal-witnesses#end-2 stroke-normal-witnesses#end-3 stroke-normal-witnesses#end-4 stroke-normal-witnesses#start-1 stroke-normal-witnesses#start-2 stroke-normal-witnesses#start-3 stroke-normal-witnesses#start-4 stroke-returning-cap-witnesses#cd053a93-end stroke-returning-cap-witnesses#cd053a93-start stroke-returning-cap-witnesses#2ed9dd82-end stroke-returning-cap-witnesses#2ed9dd82-start stroke-returning-cap-witnesses#339f6e2e-end stroke-returning-cap-witnesses#339f6e2e-start stroke-returning-cap-witnesses#ba84f08a-end stroke-returning-cap-witnesses#ba84f08a-start stroke-returning-cap-witnesses#f89fe40d-end stroke-returning-cap-witnesses#f89fe40d-start stroke-tiny-smooth-corner#forward-0 stroke-tiny-smooth-corner#forward-1 stroke-tiny-smooth-corner#forward-2 stroke-tiny-smooth-corner#forward-3 stroke-tiny-smooth-corner#reverse-0 stroke-tiny-smooth-corner#reverse-1 stroke-tiny-smooth-corner#reverse-2 stroke-tiny-smooth-corner#reverse-3 stroke-short-wide-dashes#horizontal stroke-short-wide-dashes#vertical stroke-short-wide-dashes#crossing]
- [x] svg2/painting.md#StrokeShape: the algorithm defines only an ideal/best-case stroke shape; implementations are explicitly given leeway to deviate from it for performance, and the result may differ across platforms at extreme curvature (D1, D49, D55) [stroke-shape-union#circle stroke-shape-union#curves stroke-tight-ends#start-tightening stroke-wider-than-shape#circle stroke-wider-than-shape#path stroke-wider-than-shape#open-path stroke-wider-than-shape#vanished stroke-hairpin-tip#end stroke-hairpin-tip#start stroke-hairpin-subdivision#end-unequal stroke-hairpin-subdivision#end-half stroke-hairpin-subdivision#start-unequal stroke-hairpin-subdivision#start-half stroke-folded-open#opposed-bends stroke-cubic-cusp#cubic stroke-cubic-cusp#line]

## dash positions algorithm: stroke-dash-positions

- [x] svg2/painting.md#TermDashPositions: the dash pattern is reset and restarts at the beginning of every subpath [stroke-dash-positions#restart]
- [x] svg2/painting.md#TermDashPositions: stroke-dashoffset is taken mod the sum of the dasharray values before locating the starting dash/gap index along the subpath [stroke-dasharray-values#offset stroke-dasharray-values#negative-offset stroke-dash-positions#offset-dot stroke-short-wide-dashes#horizontal-phase stroke-short-wide-dashes#vertical-phase]
- [x] svg2/painting.md#TermDashPositions: only even-indexed entries of the (possibly repeated) dasharray are emitted as painted dash spans; odd-indexed entries are unpainted gaps [stroke-dasharray-values#odd-list stroke-dash-positions#pattern-start]
- [x] svg2/painting.md#TermDashPositions: if the dasharray sum is zero, the whole subpath is treated as one dash spanning 0 to the subpath's length [stroke-dasharray-values#zero-sum]

## non-scaling-stroke: stroke-non-scaling

- [x] svg2/painting.md#non-scaling-stroke: non-scaling-stroke computes the stroke outline by transforming the shape's path into the furthest ancestor viewport's coordinate system first, computing the outline there, then transforming the outline back (D3) [stroke-non-scaling#scale stroke-non-scaling#skew stroke-non-scaling#nested-viewbox stroke-non-scaling#path-half]
- [x] svg2/painting.md#non-scaling-stroke: the resulting stroke outline is always filled with stroke paint in the current user coordinate system; the visual effect is that stroke width ignores the element's own scale, shear and zoom [stroke-non-scaling#scale stroke-non-scaling#skew stroke-non-scaling#not-inherited]

## marker properties: marker-vertices, marker-units-size, paint-properties

- [x] svg2/painting.md#MarkerStartProperty: marker-start, marker-mid, marker-end are none | <marker-ref> (a <url>); initial none; apply to shapes; are inherited; an invalid reference draws no marker at that vertex [marker-vertices#not-marker paint-properties#marker-inherited]
- [x] svg2/painting.md#VertexMarkerProperties: a polyline consisting of one point has a first and last vertex, so start and end markers apply there, in that order, and no mid marker applies; the same is true for a path consisting of one moveto. Empty points or path data have no vertex to mark. [marker-vertices#polyline-point-start marker-vertices#polyline-point-mid marker-vertices#polyline-point-end marker-vertices#polyline-point-order marker-vertices#moveto-start marker-vertices#moveto-end marker-vertices#empty-polyline marker-vertices#empty-path]
- [x] svg2/painting.md#MarkerProperty: the marker shorthand sets marker-start, marker-mid and marker-end to the same given value [marker-vertices#subpaths]
- [x] svg2/painting.md#MarkerUnitsAttribute: markerUnits is strokeWidth (default) or userSpaceOnUse; strokeWidth scales markerWidth/markerHeight/marker contents by the referencing element's painted stroke width in user units, userSpaceOnUse uses the current user coordinate system directly [marker-units-size#stroke-width marker-units-size#stroke-none marker-units-size#user-space marker-units-size#non-scaling marker-units-size#scaled]
- [x] svg2/painting.md#MarkerWidthAttribute: markerWidth and markerHeight default to 3; a value of zero on either means nothing is rendered for the marker; a negative value on either is an error [marker-units-size#default-size marker-vertices#zero-size paint-properties#marker-negative-size]

## marker refX/refY: marker-viewport

- [x] svg2/painting.md#MarkerElementRefXAttribute: refX/refY default to 0; lengths and numbers are in the marker content coordinate system after viewBox/preserveAspectRatio is applied [marker-viewport#ref-viewbox-units marker-viewport#viewbox-fit]
- [x] svg2/painting.md#MarkerElementRefXAttribute: percentages for refX are of the viewBox width, and for refY of the viewBox height [marker-viewport#ref-percent]
- [x] svg2/painting.md#MarkerElementRefXAttribute: the keywords left/center/right (refX) and top/center/bottom (refY) evaluate to 0%, 50%, 100% respectively [marker-viewport#ref-keywords]

## marker orient: marker-orient

- [x] svg2/painting.md#OrientAttribute: orient is auto | auto-start-reverse | <angle> | <number>; initial 0; a bare number is an angle in degrees [marker-orient#angles marker-orient#negative]
- [x] svg2/painting.md#OrientAttribute: auto-start-reverse is auto rotated 180° when the marker is placed by marker-start; for every other marker it is identical to auto [marker-orient#auto-start-reverse]
- [x] svg2/painting.md#RenderingMarkers: with orient auto, a marker at the start or end vertex of an open subpath is oriented in the path direction there; at any other vertex it is oriented halfway between the direction at the end of the preceding segment and the direction at the start of the following segment [marker-orient#auto-ends marker-orient#mid-bisect marker-orient#closed-start marker-orient#curve-ends]

## marker rendering coordinate system: marker-units-size, marker-viewport

- [x] svg2/painting.md#RenderingMarkers: the marker's temporary coordinate system is built by: rotating per orient, then scaling per markerUnits, then applying the marker's own viewBox/preserveAspectRatio transform [marker-viewport#viewbox-fit marker-units-size#scaled marker-orient#auto-ends]
- [x] svg2/painting.md#RenderingMarkers: when overflow on the marker element computes to a clipping value, an implicit clip path is set at the bounds of the marker's SVG viewport; the UA style sheet sets overflow:hidden on marker by default, so this clip applies unless an author rule overrides it [marker-viewport#overflow-hidden marker-viewport#overflow-visible marker-viewport#overflow-auto marker-viewport#overflow-scroll]

## marker display and cascade: marker-vertices, marker-group

- [x] svg2/painting.md#Markers: markers on one element paint bottom to top: any marker-start marker, then the marker-mid markers in order of position along the path, then any marker-end marker [marker-vertices#zero-length marker-vertices#rect]
- [x] svg2/painting.md#VertexMarkerProperties: for shapes other than path/polyline whose last vertex equals the first, both marker-start and marker-end render there if both are not none; for path, marker-start/marker-end apply only to the first/last vertex of the whole path data (not per subpath), and marker-mid to every other vertex, including the shared vertex at each closed subpath's join [marker-vertices#rect marker-vertices#circle marker-vertices#polygon marker-vertices#subpaths marker-vertices#ellipse]
- [x] svg2/painting.md#VertexMarkerProperties: on a circle, ellipse or rect, marker-mid paints the internal vertices of the equivalent path and leaves the shared first/last vertex unmarked. [marker-vertices#circle-mid-only marker-vertices#ellipse-mid-only marker-vertices#rect-mid-only]
- [x] svg2/painting.md#RenderingMarkers: properties do not inherit from the referencing element into the marker's contents; the marker's own cascade and inheritance apply instead, conceptually via a deep clone into a non-exposed tree [marker-vertices#inherit]
- [x] svg2/painting.md#RenderingMarkers: the marker's content is drawn as a group carrying all property values of the marker element itself, so its opacity, filter (its box the content's), clip-path and mask apply to the content as a whole. [marker-group#opacity marker-group#filter marker-group#clip-path marker-group#mask marker-group#in-group]
- [x] svg2/painting.md#RenderingMarkers: the UA style sheet forces display:none on marker with importance over any other rule or presentation attribute, yet a marker element remains available for reference regardless of display:none on itself or any ancestor [marker-vertices#display-none marker-vertices#not-rendered]

## paint-order: paint-order-values

- [x] svg2/painting.md#PaintOrderProperty: paint-order is normal | [fill || stroke || markers]; initial normal; applies to shapes and text content elements; is inherited [paint-order-values#normal paint-order-values#invalid-inherits paint-properties#paint-order-inherited]
- [x] svg2/painting.md#PaintOrderProperty: normal paints fill, then stroke, then markers, in that order [paint-order-values#normal]
- [x] svg2/painting.md#PaintOrderProperty: any keywords given are painted left to right in the order listed; keywords omitted from the value are painted after, in their normal relative order (so paint-order: stroke means stroke fill markers) [paint-order-values#stroke-first paint-order-values#markers-first paint-order-values#markers-stroke paint-order-values#invalid-value]

## color-interpolation: paint-color-interpolation

- [x] svg2/painting.md#ColorInterpolationProperty: color-interpolation is auto | sRGB | linearRGB; initial sRGB; applies to container elements, graphics elements, gradient elements, use and animate; is inherited [paint-color-interpolation#composite-inherited paint-color-interpolation#composite-child-srgb paint-color-interpolation#href-own-interpolation]
- [x] svg2/painting.md#ColorInterpolationProperty: it selects sRGB or linearRGB as the color space for interpolating gradient stops, color animation, and for compositing/blending a graphics element into its background [paint-color-interpolation#gradient-linear paint-color-interpolation#composite-own paint-color-interpolation#image paint-color-interpolation#blend paint-color-interpolation#opacity-group paint-color-interpolation#blend-screen paint-color-interpolation#clear-paint paint-color-interpolation#image-inner-opacity]
- [x] svg2/painting.md#ColorInterpolationProperty: when a child is blended into a background, the child's own color-interpolation value governs the blend, not the parent's [paint-color-interpolation#composite-child-srgb]
- [x] svg2/painting.md#ColorInterpolationProperty: the chosen compositing space also applies to fractional fill and stroke coverage, combined fill/element opacity, and successive colored layers; sRGB and linearRGB use their own component-wise source-over results [paint-color-interpolation#colored-linear paint-color-interpolation#colored-srgb paint-color-interpolation#colored-opacity paint-color-interpolation#colored-two-layers paint-color-interpolation#colored-stroke-linear paint-color-interpolation#colored-stroke-srgb]
- [x] svg2/painting.md#ColorInterpolationProperty: a gradient reached via href from another gradient uses the color-interpolation of the gradient directly referenced by the fill/stroke property, not that of the href-linked gradient [paint-color-interpolation#href-own-interpolation paint-color-interpolation#href-referencing-linear]

## shape-rendering: paint-rendering-hints

- [x] svg2/painting.md#ShapeRenderingProperty: shape-rendering is auto | optimizeSpeed | crispEdges | geometricPrecision; initial auto; applies to shapes; is inherited [paint-rendering-hints#shape-smooth paint-rendering-hints#shape-crisp paint-properties#shape-rendering-inherited]
- [x] svg2/painting.md#ShapeRenderingProperty: optimizeSpeed may disable anti-aliasing; crispEdges favors crisp edges over speed/precision; geometricPrecision favors precision over speed/crisp edges; auto balances all three favoring precision most; these hints allow retaining area coverage for subpixel strokes to avoid losing them to centre-only sampling. [paint-rendering-hints#shape-smooth paint-rendering-hints#shape-crisp stroke-crisp-subpixel#outline stroke-crisp-subpixel#curved stroke-crisp-subpixel#horizontal stroke-crisp-subpixel#vertical stroke-crisp-subpixel#scaled stroke-crisp-subpixel#non-scaling stroke-crisp-subpixel#skewed stroke-crisp-subpixel#reflected stroke-crisp-subpixel#unit-pixel stroke-crisp-subpixel#wide stroke-crisp-subpixel#filled-crisp stroke-crisp-subpixel#speed stroke-crisp-subpixel#zero stroke-crisp-pattern-coverage#clipped-grid stroke-crisp-pattern-coverage#outside-thin stroke-crisp-pattern-coverage#outside-crisp]

## image-rendering: paint-rendering-hints, image-cells

- [x] svg2/painting.md#ImageRenderingProperty: image-rendering is auto | optimizeQuality | optimizeSpeed; initial auto; applies to images; is inherited [paint-rendering-hints#image-smooth paint-rendering-hints#image-speed paint-properties#image-rendering-inherited]
- [x] svg2/painting.md#ImageRenderingProperty: auto and optimizeQuality require resampling at least as good as bilinear; optimizeSpeed allows resampling as low as nearest-neighbor [paint-rendering-hints#image-smooth paint-rendering-hints#image-speed image-cells#natural-size]
- [x] svg2/painting.md#ImageRenderingProperty: resampling must always be performed in a truecolor color space, even when the source or target is indexed color [paint-rendering-hints#image-smooth]

## Notes

- svg2/painting.md#issue592: miter-clip and arcs stroke-linejoin values are flagged "at risk" (no known browser implementations); the spec itself says these could still be dropped.
- svg2/painting.md#CurvatureCalculation: exact per-segment curvature-matched circles for the arcs join, plus the ArcsLinejoinFallback radius search when they don't intersect, are expensive to compute exactly per join; a close approximation is far cheaper and arcs is rare in content.

## Zero-length square dash direction: stroke-zero-dash-direction

- [x] svg2/painting.md#TermCapShape: each square cap is a half-width rectangle oriented by the path tangent; at a zero-length dash their union is a full square with that orientation, rather than a square aligned to device axes. [stroke-zero-dash-direction#east stroke-zero-dash-direction#north stroke-zero-dash-direction#northeast stroke-zero-dash-direction#northwest stroke-zero-dash-direction#west stroke-zero-dash-direction#south stroke-zero-dash-direction#southeast stroke-zero-dash-direction#southwest]
- [x] svg2/painting.md#StrokeShape: the two cap shapes are united into the stroke shape before that shape is painted, so a translucent zero-length dash is painted once. [stroke-zero-dash-direction#translucent]
- [x] svg2/painting.md#StrokeShape: the square cap is constructed in user space; a nonuniform transform acts on its geometry. [stroke-zero-dash-direction#stretched]

## Closed stroke straight bands: stroke-closed-normal-bands

- [x] svg2/painting.md#TermStrokeShape: the straight band of a closed subpath is united with its stroke shape; a folded curve offset cannot subtract points strictly inside that band. [stroke-closed-normal-bands#closed stroke-closed-normal-bands#reversed stroke-closed-normal-bands#split-half stroke-closed-normal-bands#split-unequal stroke-closed-normal-bands#straight-split stroke-closed-normal-bands#changed-start stroke-closed-normal-bands#rotated stroke-closed-normal-bands#rotated-reversed stroke-closed-normal-bands#line-as-cubic stroke-closed-normal-bands#crisp stroke-closed-normal-bands#non-scaling stroke-closed-normal-bands#reflected]
- [x] svg2/painting.md#TermStrokeShape: the stroke shape is united before its paint is applied, so coincident subpaths and overlapping straight bands do not multiply stroke-opacity. [stroke-closed-normal-bands#translucent-overlap stroke-closed-normal-bands#opposite-subpaths stroke-closed-normal-bands#many-subpaths stroke-closed-normal-bands#many-crisp]
