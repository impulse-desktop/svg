# SVG 2, Chapter 10: Basic Shapes

## rect, circle and ellipse sizing and error handling: shape-radii, shape-flat-rounded

- [x] svg2/shapes.md#RectElement: rect's x,y are the left/top edges of the rectangle in the current user coordinate system. [shape-radii#rx-alone geometry-properties#position-percent]
- [x] svg2/shapes.md#RectElement: a negative rect width or height is invalid and ignored; a computed zero for either disables rendering of the element. [shape-radii#zero-negative shape-tiny-positive#height-tenth shape-tiny-positive#height-small shape-tiny-positive#height-tiny shape-tiny-positive#width-tiny shape-tiny-positive#scaled shape-tiny-positive#height-zero shape-tiny-positive#width-zero shape-tiny-positive#negative]
- [x] svg2/shapes.md#RectElement: a negative rx or ry on rect is invalid and ignored. [shape-radii#negative-rx]
- [x] svg2/shapes.md#RectElement: if both rx and ry compute to auto, their used value is 0 (square corners); if exactly one is a length/percentage, the other's absolute value mirrors it, resolving rx against width and ry against height. [shape-radii#rx-alone shape-radii#ry-alone]
- [x] svg2/shapes.md#RectElement: used rx/ry are clamped so a radius greater than half the rectangle's width or height is reduced to that half. [shape-radii#rx-alone]
- [x] svg2/shapes.md#RectElement: a positive extent below float epsilon still permits a rounded rectangle's stroke; clamping its small radius to half that extent approaches a capsule, while an exactly zero extent disables rendering [shape-flat-rounded#height-0 shape-flat-rounded#width-0 shape-flat-rounded#height-1 shape-flat-rounded#width-1 shape-flat-rounded#height-2 shape-flat-rounded#width-2 shape-flat-rounded#height-3 shape-flat-rounded#width-3]
- [x] svg2/shapes.md#RectElement: a computed rx or ry of zero for either dimension, or auto for both dimensions, yields a rectangle with no corner rounding. [shape-radii#negative-rx css-calc-math#negative-rx]
- [x] svg2/shapes.md#CircleElement: circle's r is its radius; a negative value is invalid and ignored; a computed value of zero disables rendering. [shape-radii#percent-r shape-radii#zero-negative]
- [x] svg2/shapes.md#EllipseElement: a negative ellipse rx or ry is invalid and ignored; a computed zero for either dimension, or auto for both, disables rendering. [shape-radii#zero-negative]
- [x] svg2/shapes.md#EllipseElement: an auto value on just one of rx/ry derives that radius from the other (without rect's width/height clamping), giving a circle sized by a single percentage of the viewport's width or height. [shape-radii#ellipse-one-radius]

## Basic shapes as equivalent paths: marker-vertices, stroke-basics, path-grammar, geometry-properties

- [x] svg2/shapes.md#Introduction: every basic shape element (circle, ellipse, line, path, polygon, polyline, rect) is mathematically equivalent to, and shares every property of, a 'path' element drawing the same shape. [marker-vertices#rect marker-vertices#circle marker-vertices#line marker-vertices#polygon stroke-basics#rect-start]
- [x] svg2/shapes.md#RectElement: rect maps to a path tracing its four sides with absolute moveto/lineto, replacing each corner with a quarter elliptical arc (sweep-flag 1) when the used rx and ry are both greater than zero, closed with a segment-completing close. [marker-vertices#rect stroke-basics#rect-start stroke-conic-dash-positions#fully-rounded-rect]
- [x] svg2/shapes.md#CircleElement: circle maps to a moveto at its 3-o'clock point followed by four clockwise quarter-circle elliptical arcs (rx=ry=r) and a segment-completing close. [marker-vertices#circle stroke-basics#circle-start stroke-conic-dash-positions#circle-negative stroke-conic-dash-positions#circle-positive stroke-conic-dash-positions#circle-reflected stroke-conic-dash-positions#circle-opacity-rotation]
- [x] svg2/shapes.md#EllipseElement: ellipse maps the same way as circle, but with independent rx and ry radii. [marker-vertices#ellipse stroke-basics#ellipse-start stroke-conic-dash-positions#ellipse]
- [x] svg2/shapes.md#LineElement: line maps to an absolute moveto (x1,y1) then lineto (x2,y2); being one-dimensional, a line is never filled regardless of the fill property. [path-grammar#line-not-filled marker-vertices#line stroke-basics#line-start]
- [x] svg2/shapes.md#PolylineElement: polyline maps to an absolute moveto at its first point followed by an absolute lineto to each subsequent point, left open (no closepath). [geometry-properties#polyline-open]
- [x] svg2/shapes.md#PolygonElement: polygon maps the same way as polyline, but ends with a closepath, making it a closed shape. [path-grammar#points-odd marker-vertices#polygon]

## line and points attribute grammar: path-grammar, geometry-properties, shape-points (exists)

- [x] svg2/shapes.md#LineElementX1Attribute: line's x1, y1, x2, y2 are each <length-percentage>|<number>, initial 0, settable only as attributes (not yet CSS properties). [stroke-basics#line-start]
- [x] svg2/shapes.md#DataTypePoints: points is [<number>+]#, a list of numbers in the user coordinate system, taken pairwise as coordinates. [path-grammar#points-odd marker-vertices#polygon]
- [x] svg2/shapes.md#DataTypePoints: an odd count of numbers in points is an error, handled like a malformed path: the user agent drops the last odd value and renders the rest. [path-grammar#points-odd]
- [x] svg2/shapes.md#PolylineElementPointsAttribute: points' initial value (none) means the polyline/polygon is valid but does not render. [geometry-properties#points-none]

## Notes

- svg2/shapes.md#EllipseElement: the ellipse's single-axis 'auto' radius is explicitly exempted from rect's half-width/half-height clamping — easy for an implementation sharing rect's rounding code to apply by mistake.
