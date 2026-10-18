# SVG 2, Chapter 10: Basic Shapes

## rect, circle and ellipse sizing and error handling: shape-radii

- [x] svg2/shapes.md#RectElement: rect's x,y are the left/top edges of the rectangle in the current user coordinate system.
- [x] svg2/shapes.md#RectElement: a negative rect width or height is invalid and ignored; a computed zero for either disables rendering of the element.
- [x] svg2/shapes.md#RectElement: a negative rx or ry on rect is invalid and ignored.
- [x] svg2/shapes.md#RectElement: if both rx and ry compute to auto, their used value is 0 (square corners); if exactly one is a length/percentage, the other's absolute value mirrors it, resolving rx against width and ry against height.
- [x] svg2/shapes.md#RectElement: used rx/ry are clamped so a radius greater than half the rectangle's width or height is reduced to that half.
- [x] svg2/shapes.md#RectElement: a computed rx or ry of zero for either dimension, or auto for both dimensions, yields a rectangle with no corner rounding.
- [x] svg2/shapes.md#CircleElement: circle's r is its radius; a negative value is invalid and ignored; a computed value of zero disables rendering.
- [x] svg2/shapes.md#EllipseElement: a negative ellipse rx or ry is invalid and ignored; a computed zero for either dimension, or auto for both, disables rendering.
- [x] svg2/shapes.md#EllipseElement: an auto value on just one of rx/ry derives that radius from the other (without rect's width/height clamping), giving a circle sized by a single percentage of the viewport's width or height.

## Basic shapes as equivalent paths: marker-vertices, stroke-basics, path-grammar

- [x] svg2/shapes.md#Introduction: every basic shape element (circle, ellipse, line, path, polygon, polyline, rect) is mathematically equivalent to, and shares every property of, a 'path' element drawing the same shape.
- [x] svg2/shapes.md#RectElement: rect maps to a path tracing its four sides with absolute moveto/lineto, replacing each corner with a quarter elliptical arc (sweep-flag 1) when the used rx and ry are both greater than zero, closed with a segment-completing close.
- [x] svg2/shapes.md#CircleElement: circle maps to a moveto at its 3-o'clock point followed by four clockwise quarter-circle elliptical arcs (rx=ry=r) and a segment-completing close.
- [x] svg2/shapes.md#EllipseElement: ellipse maps the same way as circle, but with independent rx and ry radii.
- [x] svg2/shapes.md#LineElement: line maps to an absolute moveto (x1,y1) then lineto (x2,y2); being one-dimensional, a line is never filled regardless of the fill property.
- [x] svg2/shapes.md#PolylineElement: polyline maps to an absolute moveto at its first point followed by an absolute lineto to each subsequent point, left open (no closepath).
- [x] svg2/shapes.md#PolygonElement: polygon maps the same way as polyline, but ends with a closepath, making it a closed shape.

## line and points attribute grammar: path-grammar, shape-points (exists)

- [x] svg2/shapes.md#LineElementX1Attribute: line's x1, y1, x2, y2 are each <length-percentage>|<number>, initial 0, settable only as attributes (not yet CSS properties).
- [x] svg2/shapes.md#DataTypePoints: points is [<number>+]#, a list of numbers in the user coordinate system, taken pairwise as coordinates.
- [x] svg2/shapes.md#DataTypePoints: an odd count of numbers in points is an error, handled like a malformed path: the user agent drops the last odd value and renders the rest.
- [x] svg2/shapes.md#PolylineElementPointsAttribute: points' initial value (none) means the polyline/polygon is valid but does not render.

## Notes

- svg2/shapes.md#EllipseElement: the ellipse's single-axis 'auto' radius is explicitly exempted from rect's half-width/half-height clamping — easy for an implementation sharing rect's rounding code to apply by mistake.
