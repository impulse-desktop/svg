# SVG 2, Chapter 9: Paths

## Path data grammar and error handling: path-grammar, path-data-commands

- [x] svg2/paths.md#TheDProperty: the 'd' property's value is none | <string>, initial none; none means the path has no data, does not render and does not contribute to an ancestor's bounding box. [path-data-commands#none]
- [x] svg2/paths.md#TheDProperty: every coordinate and length in path data is always in user units of the current user coordinate system (no unit identifiers or percentages). [path-data-commands#unit-error]
- [x] svg2/paths.md#TheDProperty: the <string> must match the svg-path EBNF grammar; if it contains no valid commands at all, the path behaves as if d were none. [path-data-commands#empty]
- [x] svg2/paths.md#PathDataGeneralInformation: the only decimal point allowed in path data is U+002E FULL STOP; no other delimiter (e.g. a thousands comma) may appear inside a number. [path-data-commands#comma-in-number path-data-commands#comma-separates]
- [x] svg2/paths.md#PathDataBNF: a number is fractional-constant exponent?, optionally signed, with fractional-constant being digits with an optional embedded "."; trailing bare decimal points (e.g. "23.") are disallowed by SVG2, though a UA may still accept them where unambiguous; the renderer accepts a lone point after digits alone, as SVG 1.1 had it, but stops at two adjacent points (D53). [path-data-commands#trailing-point path-data-commands#double-point-line path-data-commands#double-point-arc path-grammar#number-ends]
- [x] svg2/paths.md#PathDataBNF: EBNF productions consume as much as possible: "M 100-200" parses coordinates 100 and -200 (stopping at the sign); "M 0.6.5" parses 0.6 and .5 (stopping at the second decimal point). [path-grammar#number-ends]
- [x] svg2/paths.md#PathDataBNF: path data not matching the grammar is an error, handled per the error-handling rules. [path-grammar#error path-data-commands#unit-error path-data-commands#comma-after-close]
- [x] svg2/paths.md#PathDataErrorHandling: on an error, the path renders up to (but not including) the command containing the first error. [path-grammar#error]
- [x] svg2/paths.md#PathDataErrorHandling: a command with an incorrect parameter count renders up to and including its last correctly-defined segment (e.g. 'M 10,10 L 20,20,30' draws the line to 20,20 only). [path-grammar#short-command]

## Path commands: path-grammar, path-data-commands

- [x] svg2/paths.md#PathDataMovetoCommands: M/m sets a new initial point and current point; a moveto after the first starts a new subpath; extra coordinate pairs after it are implicit linetos, relative if m, absolute if M. [path-data-commands#moveto-pairs]
- [x] svg2/paths.md#PathDataMovetoCommands: a relative m as the very first command of a path is treated as absolute coordinates, but further pairs in that same command stay relative; a later relative m moves to (cpx+x, cpy+y). [path-data-commands#relative-first path-data-commands#moveto-pairs]
- [x] svg2/paths.md#PathDataClosePathCommand: Z/z draws a straight (possibly zero-length) line from the current point back to the subpath's initial point and joins the two ends using stroke-linejoin, unlike a manual lineto closure which caps both ends with stroke-linecap. [path-data-commands#closepath-join]
- [x] svg2/paths.md#PathDataClosePathCommand: after Z, a following M starts the next subpath there; a following non-moveto command starts the next subpath at the same initial point as the one just closed. [path-grammar#relative-closepath]
- [x] svg2/paths.md#PathDataLinetoCommands: H/h and V/v are L/l with y or x fixed at the current value (0 relative); multiple values may be given, with the current point ending at the last one. [path-data-commands#hv-lists]
- [x] svg2/paths.md#PathDataCubicBezierCommands: S/s's first control point is the reflection of the previous segment's final control point about the current point, or coincident with the current point if the previous command was not C/c/S/s. [path-grammar#smooth-cubic path-grammar#smooth-after-other]
- [x] svg2/paths.md#PathDataQuadraticBezierCommands: T/t's control point is likewise the reflection of the previous command's control point, or coincident with the current point if the previous command was not Q/q/T/t. [path-grammar#smooth-quadratic path-grammar#smooth-after-other]
- [x] svg2/paths.md#ReflectedControlPoints: the reflected control point is (2*curx - oldx2, 2*cury - oldy2), where (curx,cury) is the current point and (oldx2,oldy2) the previous segment's final control point. [path-grammar#smooth-cubic path-grammar#smooth-quadratic]
- [x] svg2/paths.md#PathDataEllipticalArcCommands: an elliptical arc's large-arc-flag picks a sweep of ≥180° (1) or ≤180° (0) of the two candidate ellipses' arcs; sweep-flag picks the positive-angle (1) or negative-angle (0) direction of traversal. [path-data-commands#arc-flags-00 path-data-commands#arc-flags-01 path-data-commands#arc-flags-10 path-data-commands#arc-flags-11]

## Out-of-range elliptical arc parameters: path-grammar, path-data-commands

- [x] svg2/paths.md#ArcOutOfRangeParameters: if an arc's endpoint equals the current point, the arc segment is omitted entirely. [path-grammar#arc-same-ends]
- [x] svg2/paths.md#ArcOutOfRangeParameters: if rx or ry is 0, the arc is treated as a straight lineto to the endpoint. [path-grammar#arc-zero]
- [x] svg2/paths.md#ArcOutOfRangeParameters: a negative rx or ry has its sign dropped; the absolute value is used. [path-grammar#arc-negative]
- [x] svg2/paths.md#ArcOutOfRangeParameters: if rx, ry and x-axis-rotation admit no solution reaching the endpoint, the ellipse is scaled up uniformly until exactly one solution exists. [path-grammar#arc-small]
- [x] svg2/paths.md#PathDataEllipticalArcCommands: an arc is a part of its ellipse at any size: radii far beyond the chord, 1e20 on a chord of 20, give the arc as good as straight that their ellipse draws, its centre that far off. [path-data-commands#arc-huge-radii]

## Zero-length path segments: marker-vertices, stroke-linecap-shapes

- [x] svg2/paths.md#ZeroLengthSegments: zero-length path segments are not invalid. [marker-vertices#zero-length stroke-linecap-shapes#zero-square]
- [x] svg2/paths.md#ZeroLengthSegments: a marker is drawn at every applicable vertex even when that vertex ends a zero-length segment, including consecutive moveto commands. [marker-vertices#zero-length marker-vertices#movetos]
- [x] svg2/paths.md#ZeroLengthSegments: stroke-linecap round or square must still be painted for zero-length subpaths. [stroke-linecap-shapes#zero-round stroke-linecap-shapes#zero-square stroke-linecap-shapes#zero-butt]

## Direction of a path: marker-orient, stroke-linecap-shapes, stroke-dash-positions, path-direction

- [x] svg2/paths.md#TermPathDirection: at distance 0 the path's direction is that at the start of its first segment; at the path's full length, it is that at the end of its last segment. [marker-orient#auto-ends marker-orient#curve-ends]
- [x] svg2/paths.md#TermPathDirection: at a segment boundary the direction is that at the start of the segment beginning there, skipping past zero-length segments to the later non-zero segment. [path-direction#boundary-skip stroke-dash-positions#negative-offset]
- [x] svg2/paths.md#TermPathDirection: mid-segment, direction is the curve's tangent there; at a discontinuity (a cusp) it is undefined, and should be taken between the incoming and outgoing directions. [path-direction#curve-tangent path-direction#cusp-fold]
- [x] svg2/paths.md#TermPathSegmentStartDirection: if the whole path has zero length, direction at a segment's start is the positive x-axis; if the segment itself is zero-length, it is the end direction of the closest preceding non-zero segment (or its own end direction if none precedes); otherwise it is the tangent out of the segment's start. [stroke-linecap-shapes#zero-no-direction stroke-linecap-shapes#zero-direction stroke-linecap-shapes#zero-after-closed stroke-linecap-shapes#zero-next]
- [x] svg2/paths.md#TermPathSegmentEndDirection: symmetrically, a zero-length whole path gives the positive x-axis; a zero-length segment takes the start direction of the closest following non-zero segment (or its own start direction if none follows); otherwise it is the tangent into the segment's end. [stroke-linecap-shapes#zero-no-direction stroke-linecap-shapes#zero-next marker-orient#zero-segment]

## The pathLength attribute: path-length-edges, stroke-dasharray-values

- [x] svg2/paths.md#PathLengthAttribute: pathLength is a <number>, no initial value, animatable; it is the author's computed total path length in user units. [stroke-dasharray-values#path-length path-length-edges#moveto]
- [x] svg2/paths.md#PathLengthAttribute: the UA scales all distance-along-a-path computations by the ratio of pathLength to its own computed total length. [stroke-dasharray-values#path-length path-length-edges#moveto]
- [x] svg2/paths.md#PathLengthAttribute: pathLength of 0 is valid and is treated as a scaling factor of infinity: 0 scaled by it stays 0, any other positive distance scaled by it becomes +Infinity. [path-length-edges#zero]
- [x] svg2/paths.md#PathLengthAttribute: a negative pathLength value is an error. [path-length-edges#negative]
- [-] svg2/paths.md#PathLengthAttribute: pathLength has no effect on percentage-based distance-along-a-path calculations. (D36) [path-length-edges#percent]
- [x] svg2/paths.md#DistanceAlongAPath: a moveto has zero length; only lineto, curveto and arcto commands contribute to a path's length. [path-length-edges#moveto]

## Notes

- svg2/paths.md#PathDataBNF: the spec disallows numbers with a trailing bare decimal point (e.g. "23.") but explicitly still permits a UA to accept them when parsing is unambiguous — a deliberate leeway, not a hard requirement.
- svg2/paths.md#DistanceAlongAPath: exact distance-along-a-path mathematics is "highly complex"; the spec itself recommends only "as precise as possible" approximations, sanctioning a faster inexact algorithm.
- svg2/paths.md#PathLengthAttribute: treating a zero pathLength as a scaling factor of infinity needs a special zero/non-zero branch per sample rather than a plain multiply.
