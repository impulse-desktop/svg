# SVG 2, Chapter 13: Painting: Filling, Stroking and Marker Symbols

## Paint value syntax: paint-fallback, paint-syntax (exists), colors (exists)

- [x] svg2/painting.md#DataTypePaint: <paint> = none | <color> | <url> [none | <color>]? | context-fill | context-stroke
- [x] svg2/painting.md#DataTypePaint: the <url> references a paint server element (linearGradient, pattern, radialGradient), optionally followed by a fallback <color> or none
- [x] svg2/painting.md#DataTypePaint: the fallback is used only if the paint server reference cannot be resolved (points to a nonexistent element or one that is not a valid paint server)
- [x] svg2/painting.md#DataTypePaint: if a paint server reference is invalid and no fallback is given, no paint is rendered for that layer
- [x] svg2/painting.md#DataTypePaint: for any <color>, CSS Color Module 3 syntaxes (rgb(), rgba(), hsl(), hsla(), extended keywords, currentColor) must be supported

## Context element paint: paint-context, use-symbol-context-servers (exists)

- [x] svg2/painting.md#context-paint: context-fill and context-stroke reference the fill/stroke paint layers generated for the context element
- [x] svg2/painting.md#context-paint: the context element is the element referencing a marker (when painting inside that marker) or the use element (when inside a use shadow tree); otherwise there is no context element
- [x] svg2/painting.md#context-paint: if there is no context element and context-fill/context-stroke are used, no paint is applied for that layer
- [x] svg2/painting.md#context-paint: the coordinate space and bounding box used for context paint servers are those of the context element, so gradients/patterns stay continuous from shape to marker or across the use shadow tree
- [x] svg2/painting.md#context-paint: context-fill/context-stroke references are resolved recursively if the context element's own fill/stroke is itself context-fill/context-stroke

## color property currentColor: paint-current-color

- [x] svg2/painting.md#ColorProperty: the color property supplies the currentColor indirect value for fill, stroke, stop-color, flood-color and lighting-color only; it has no other effect on SVG elements

## fill area: fill-open-subpath

- [x] svg2/painting.md#FillProperty: fill's initial value is black; applies to shapes and text content elements; is inherited
- [x] svg2/painting.md#FillProperty: the painted area is everywhere inside the outline per the fill-rule; the zero-width geometric outline itself is included in the area painted
- [x] svg2/painting.md#FillProperty: fill treats each open subpath as if an extra closepath were added, so fill applies to open subpaths of path elements and to polyline elements the same as to closed ones

## fill-rule: fill-rule-crossings

- [x] svg2/painting.md#FillRuleProperty: fill-rule is nonzero | evenodd; initial nonzero; applies to shapes and text content elements; is inherited
- [x] svg2/painting.md#FillRuleProperty: nonzero sums signed ray crossings (+1 left-to-right, -1 right-to-left) from the point to infinity; zero sum is outside, nonzero is inside
- [x] svg2/painting.md#FillRuleProperty: evenodd counts ray crossings; an odd count is inside, an even count is outside
- [x] svg2/painting.md#FillRuleProperty: a ray tangent to or coincident with a path segment may be replaced by a different ray that avoids the problem intersection

## fill-opacity: paint-opacity-values

- [x] svg2/painting.md#FillOpacityProperty: fill-opacity is <number> | <percentage>; initial 1; applies to shapes and text content elements; is inherited; computed value is clamped to [0,1]
- [x] svg2/painting.md#FillOpacityProperty: values outside 0 (fully transparent) to 1 (fully opaque) must be clamped to that range; a percentage is of the 0-1 range

## stroke basics: stroke-basics, stroke-linecap-shapes, paint-opacity-values, stroke-dash-cubic (exists)

- [x] svg2/painting.md#StrokeProperties: stroking properties affected by directionality (e.g. dash patterns) render starting at the same point the graphics element starts (for path, the first moveto point)
- [x] svg2/painting.md#StrokeProperties: distance-along-the-outline computations for stroking use the standard distance-along-a-path algorithm
- [x] svg2/painting.md#StrokeProperties: stroking with a gradient or pattern paint server must equal filling the equivalent path (the element's geometry plus its stroking properties) with that same paint server
- [x] svg2/painting.md#StrokeProperty: stroke's initial value is none; applies to shapes and text content elements; is inherited
- [x] svg2/painting.md#StrokeProperty: a subpath consisting of a moveto with no following line-drawing command is never stroked; any other zero-length subpath is not stroked when stroke-linecap is butt
- [x] svg2/painting.md#StrokeOpacityProperty: stroke-opacity is defined exactly like fill-opacity (value, initial 1, clamped to [0,1]) but for the stroke paint

## stroke-width: stroke-width-values

- [x] svg2/painting.md#StrokeWidthProperty: stroke-width is <length-percentage> | <number>; initial 1px; applies to shapes and text content elements; is inherited; percentages refer to the normalized diagonal of the current viewport
- [x] svg2/painting.md#StrokeWidthProperty: a zero stroke-width causes no stroke to be painted
- [x] svg2/painting.md#StrokeWidthProperty: a negative stroke-width is invalid
- [x] svg2/painting.md#StrokeWidthProperty: a <number> value is in user units

## stroke-linecap: stroke-linecap-shapes

- [x] svg2/painting.md#StrokeLinecapProperty: stroke-linecap is butt | round | square; initial butt; applies to shapes and text content elements; is inherited; also controls the shape drawn for zero-length subpaths
- [x] svg2/painting.md#StrokeLinecapProperty: butt extends the stroke no further than the endpoints, so a zero-length subpath with butt gets no stroke at all
- [x] svg2/painting.md#StrokeLinecapProperty: round extends each end by a half circle of diameter stroke-width; a zero-length subpath with round draws a full circle centered at its point
- [x] svg2/painting.md#StrokeLinecapProperty: square extends each end by a rectangle of width stroke-width and length stroke-width/2 oriented to the tangent; a zero-length subpath with square draws a square of side stroke-width oriented by the tangent at that point
- [x] svg2/painting.md#TermCapShape: the starting/ending cap shape is a semicircle or rectangle (per linecap) positioned with its straight edge perpendicular to the subpath at the cap's position, centered on the point on the subpath at that distance

## stroke-linejoin and stroke-miterlimit: stroke-linejoin-shapes

- [x] svg2/painting.md#StrokeLinejoinProperty: stroke-linejoin is miter | miter-clip | round | bevel | arcs; initial miter; applies to shapes and text content elements; is inherited
- [x] svg2/painting.md#StrokeLinejoinProperty: miter extends the outer stroke edges to their intersection; if the miter limit is exceeded it falls back to bevel
- [x] svg2/painting.md#StrokeLinejoinProperty: miter-clip is as miter, but when the limit is exceeded it clips the miter at half the miterlimit times stroke-width from the segment intersection, instead of falling back to bevel
- [x] svg2/painting.md#StrokeLinejoinProperty: round draws a circular sector centered on the join point; bevel fills the triangle between the two stroked segment edges
- [x] svg2/painting.md#StrokeMiterlimitProperty: stroke-miterlimit is <number>; initial 4; applies to shapes and text content elements; is inherited; a negative value is invalid and must be ignored
- [x] svg2/painting.md#math-miter-length: the miter length is stroke-width / sin(theta/2), theta the angle between the segments; once miter-length/stroke-width exceeds stroke-miterlimit, miter falls back to bevel and miter-clip clips at that limit
- [x] svg2/painting.md#TermLineJoinShape: no line-join shape is produced where the tangent at the end of a segment equals the tangent at the start of the next segment (e.g. two collinear same-direction line segments)

## stroke-linejoin arcs: stroke-linejoin-shapes

- [-] svg2/painting.md#TermLineJoinShape: arcs extends the outer stroke edges with circles matching each segment's curvature at the join, then clips the result by the miter limit exactly as miter-clip does (D4)
- [x] svg2/painting.md#TermLineJoinShape: if both segment curvatures at the join are zero, arcs falls through to miter-clip
- [-] svg2/painting.md#TermLineJoinShape: if either curvature exceeds 2/stroke-width, arcs falls back to round (D4)
- [-] svg2/painting.md#CurvatureCalculation: a circle's radius from curvature κ is r=1/κ, then adjusted by plus-or-minus half the stroke width for the stroke edge; a straight segment (zero curvature) is extended by a line instead (D4)
- [-] svg2/painting.md#ArcsLinejoinFallback: when the two curvature circles do not intersect, both radii are changed by an equal amount (shrinking the enclosing one and growing the enclosed one, or growing both) until they touch (D4)
- [-] svg2/painting.md#ArcsLinejoinFallback: if the offset paths' tangents at the join are parallel so the circles can never be made to touch, the join is instead a rectangle of width stroke-width and length stroke-width times half of stroke-miterlimit (D4)

## stroke-dasharray and stroke-dashoffset: stroke-dasharray-values

- [x] svg2/painting.md#StrokeDasharrayProperty: stroke-dasharray is none | <dasharray>; initial none; applies to shapes and text content elements; is inherited; percentages refer to the normalized diagonal of the viewport
- [x] svg2/painting.md#DataTypeDasharray: <dasharray> is a comma/white-space separated list of <number> | <length-percentage>; an odd-length list is repeated once to make it even
- [x] svg2/painting.md#DataTypeDasharray: the first and every second value after it is a dash length; every other value is a gap length; a <number> is in user units
- [x] svg2/painting.md#DataTypeDasharray: a negative value anywhere in the list makes the whole stroke-dasharray value invalid
- [x] svg2/painting.md#DataTypeDasharray: if every value in the list is zero, the stroke is rendered as a solid line with no dashing
- [x] svg2/painting.md#DataTypeDasharray: the pathLength attribute on a path element scales each dash/gap length relative to the author's declared path length
- [x] svg2/painting.md#StrokeDashoffsetProperty: stroke-dashoffset is <length-percentage> | <number>; initial 0; inherited; same percentage base as stroke-dasharray; also scaled by pathLength like stroke-dasharray
- [x] svg2/painting.md#math-dashoffset: a negative stroke-dashoffset is equivalent to offset d = sum − (abs(offset) mod sum), sum being the total of the dasharray values

## stroke shape: stroke-shape-union, stroke-dash-corner (exists), stroke-dash-cubic (exists)

- [x] svg2/painting.md#TermStrokeShape: the stroke shape is the union, for each subpath, of each dash's parallel-offset band, its start/end cap shapes, and the line-join shapes of the segments it spans
- [x] svg2/painting.md#StrokeShape: the algorithm defines only an ideal/best-case stroke shape; implementations are explicitly given leeway to deviate from it for performance, and the result may differ across platforms at extreme curvature (D1)

## dash positions algorithm: stroke-dash-positions

- [x] svg2/painting.md#TermDashPositions: the dash pattern is reset and restarts at the beginning of every subpath
- [x] svg2/painting.md#TermDashPositions: stroke-dashoffset is taken mod the sum of the dasharray values before locating the starting dash/gap index along the subpath
- [x] svg2/painting.md#TermDashPositions: only even-indexed entries of the (possibly repeated) dasharray are emitted as painted dash spans; odd-indexed entries are unpainted gaps
- [x] svg2/painting.md#TermDashPositions: if the dasharray sum is zero, the whole subpath is treated as one dash spanning 0 to the subpath's length

## non-scaling-stroke: stroke-non-scaling

- [x] svg2/painting.md#non-scaling-stroke: non-scaling-stroke computes the stroke outline by transforming the shape's path into the furthest ancestor viewport's coordinate system first, computing the outline there, then transforming the outline back (D3)
- [x] svg2/painting.md#non-scaling-stroke: the resulting stroke outline is always filled with stroke paint in the current user coordinate system; the visual effect is that stroke width ignores the element's own scale, shear and zoom

## marker properties: marker-vertices, marker-units-size

- [x] svg2/painting.md#MarkerStartProperty: marker-start, marker-mid, marker-end are none | <marker-ref> (a <url>); initial none; apply to shapes; are inherited; an invalid reference draws no marker at that vertex
- [x] svg2/painting.md#MarkerProperty: the marker shorthand sets marker-start, marker-mid and marker-end to the same given value
- [x] svg2/painting.md#MarkerUnitsAttribute: markerUnits is strokeWidth (default) or userSpaceOnUse; strokeWidth scales markerWidth/markerHeight/marker contents by the referencing element's painted stroke width in user units, userSpaceOnUse uses the current user coordinate system directly
- [x] svg2/painting.md#MarkerWidthAttribute: markerWidth and markerHeight default to 3; a value of zero on either means nothing is rendered for the marker; a negative value on either is an error

## marker refX/refY: marker-viewport

- [x] svg2/painting.md#MarkerElementRefXAttribute: refX/refY default to 0; lengths and numbers are in the marker content coordinate system after viewBox/preserveAspectRatio is applied
- [x] svg2/painting.md#MarkerElementRefXAttribute: percentages for refX are of the viewBox width, and for refY of the viewBox height
- [x] svg2/painting.md#MarkerElementRefXAttribute: the keywords left/center/right (refX) and top/center/bottom (refY) evaluate to 0%, 50%, 100% respectively

## marker orient: marker-orient

- [x] svg2/painting.md#OrientAttribute: orient is auto | auto-start-reverse | <angle> | <number>; initial 0; a bare number is an angle in degrees
- [x] svg2/painting.md#OrientAttribute: auto-start-reverse is auto rotated 180° when the marker is placed by marker-start; for every other marker it is identical to auto
- [x] svg2/painting.md#RenderingMarkers: with orient auto, a marker at the start or end vertex of an open subpath is oriented in the path direction there; at any other vertex it is oriented halfway between the direction at the end of the preceding segment and the direction at the start of the following segment

## marker rendering coordinate system: marker-units-size, marker-viewport

- [x] svg2/painting.md#RenderingMarkers: the marker's temporary coordinate system is built by: rotating per orient, then scaling per markerUnits, then applying the marker's own viewBox/preserveAspectRatio transform
- [x] svg2/painting.md#RenderingMarkers: when overflow on the marker element computes to a clipping value, an implicit clip path is set at the bounds of the marker's SVG viewport; the UA style sheet sets overflow:hidden on marker by default, so this clip applies unless an author rule overrides it

## marker display and cascade: marker-vertices

- [x] svg2/painting.md#Markers: markers on one element paint bottom to top: any marker-start marker, then the marker-mid markers in order of position along the path, then any marker-end marker
- [x] svg2/painting.md#VertexMarkerProperties: for shapes other than path/polyline whose last vertex equals the first, both marker-start and marker-end render there if both are not none; for path, marker-start/marker-end apply only to the first/last vertex of the whole path data (not per subpath), and marker-mid to every other vertex, including the shared vertex at each closed subpath's join
- [x] svg2/painting.md#RenderingMarkers: properties do not inherit from the referencing element into the marker's contents; the marker's own cascade and inheritance apply instead, conceptually via a deep clone into a non-exposed tree
- [x] svg2/painting.md#RenderingMarkers: the UA style sheet forces display:none on marker with importance over any other rule or presentation attribute, yet a marker element remains available for reference regardless of display:none on itself or any ancestor

## paint-order: paint-order-values

- [x] svg2/painting.md#PaintOrderProperty: paint-order is normal | [fill || stroke || markers]; initial normal; applies to shapes and text content elements; is inherited
- [x] svg2/painting.md#PaintOrderProperty: normal paints fill, then stroke, then markers, in that order
- [x] svg2/painting.md#PaintOrderProperty: any keywords given are painted left to right in the order listed; keywords omitted from the value are painted after, in their normal relative order (so paint-order: stroke means stroke fill markers)

## color-interpolation: paint-color-interpolation

- [x] svg2/painting.md#ColorInterpolationProperty: color-interpolation is auto | sRGB | linearRGB; initial sRGB; applies to container elements, graphics elements, gradient elements, use and animate; is inherited
- [x] svg2/painting.md#ColorInterpolationProperty: it selects sRGB or linearRGB as the color space for interpolating gradient stops, color animation, and for compositing/blending a graphics element into its background
- [x] svg2/painting.md#ColorInterpolationProperty: when a child is blended into a background, the child's own color-interpolation value governs the blend, not the parent's
- [x] svg2/painting.md#ColorInterpolationProperty: a gradient reached via href from another gradient uses the color-interpolation of the gradient directly referenced by the fill/stroke property, not that of the href-linked gradient

## shape-rendering: paint-rendering-hints

- [x] svg2/painting.md#ShapeRenderingProperty: shape-rendering is auto | optimizeSpeed | crispEdges | geometricPrecision; initial auto; applies to shapes; is inherited
- [x] svg2/painting.md#ShapeRenderingProperty: optimizeSpeed may disable anti-aliasing; crispEdges favors crisp edges over speed/precision; geometricPrecision favors precision over speed/crisp edges; auto balances all three favoring precision most

## image-rendering: paint-rendering-hints

- [x] svg2/painting.md#ImageRenderingProperty: image-rendering is auto | optimizeQuality | optimizeSpeed; initial auto; applies to images; is inherited
- [x] svg2/painting.md#ImageRenderingProperty: auto and optimizeQuality require resampling at least as good as bilinear; optimizeSpeed allows resampling as low as nearest-neighbor
- [x] svg2/painting.md#ImageRenderingProperty: resampling must always be performed in a truecolor color space, even when the source or target is indexed color

## Notes

- svg2/painting.md#issue592: miter-clip and arcs stroke-linejoin values are flagged "at risk" (no known browser implementations); the spec itself says these could still be dropped.
- svg2/painting.md#CurvatureCalculation: exact per-segment curvature-matched circles for the arcs join, plus the ArcsLinejoinFallback radius search when they don't intersect, are expensive to compute exactly per join; a close approximation is far cheaper and arcs is rare in content.
