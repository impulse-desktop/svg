# CSS Shapes Module Level 1

## Basic shape rectangles: inset/xywh/rect: css-basic-shapes, clip-path-functions (exists)

- [x] css-shapes-1.md#funcdef-basic-shape-inset: inset() takes 1-4 length-percentages for the top/right/bottom/left insets from the reference box's edges; a missing 2nd/3rd value defaults from the 1st, a missing 4th from the 2nd, as in the margin shorthand [css-basic-shapes#inset]
- [x] css-shapes-1.md#funcdef-basic-shape-inset: insets on opposite sides that sum to more than the reference box's dimension are proportionally reduced to sum to 100% [css-basic-shapes#inset-overflow]
- [x] css-shapes-1.md#funcdef-basic-shape-xywh: xywh(x y w h) gives a rectangle offset x,y from the reference box's top-left corner, with the given width and height [css-basic-shapes#xywh]
- [x] css-shapes-1.md#funcdef-basic-shape-rect: rect(top right bottom left) gives each edge as an inset from the reference box's top edge (1st/3rd values) or left edge (2nd/4th values); auto is 0% for the top/left values and 100% for the right/bottom values [css-basic-shapes#rect css-basic-shapes#rect-auto]
- [x] css-shapes-1.md#funcdef-basic-shape-rect: in rect(), the right value is floored by the left value and the bottom value is floored by the top value, so the edges never cross [css-basic-shapes#rect-crossed]
- [x] css-shapes-1.md#basic-shape-computed-values: inset(), rect() and xywh() all compute to an equivalent inset() function [css-basic-shapes#equivalent]
- [x] css-shapes-1.md#funcdef-basic-shape-inset: inset()/xywh()/rect() each accept an optional "round <border-radius>" to round the rectangle's corners [css-basic-shapes#round css-basic-shapes#equivalent]

## Basic shapes: circle/ellipse/polygon: css-basic-shapes, clip-path-polygon-round, clip-path-boxes (exists)

- [x] css-shapes-1.md#funcdef-basic-shape-circle: circle()'s radial-size argument gives the radius, resolved against the reference box rather than a gradient box; two length-percentage values are invalid; an omitted radius defaults to closest-side [css-basic-shapes#circle css-basic-shapes#circle-invalid css-basic-shapes#circle-default]
- [x] css-shapes-1.md#funcdef-basic-shape-circle: a single percentage radius for circle() resolves against the reference box's direction-agnostic size (css-shapes-1.md#direction-agnostic-size: the box diagonal's length divided by sqrt(2), equal to width/height for a square box) [css-basic-shapes#circle-percent]
- [x] css-shapes-1.md#funcdef-basic-shape-circle: the position argument gives the shape's center, defaulting to center if omitted; css-shapes-1.md#funcdef-basic-shape-ellipse: likewise for ellipse() [css-basic-shapes#position css-basic-shapes#position-sides css-basic-shapes#position-invalid]
- [x] css-shapes-1.md#funcdef-basic-shape-ellipse: ellipse()'s radial-size gives the horizontal and vertical radii, each resolved against the reference box; an omitted radius defaults to closest-side [css-basic-shapes#ellipse]
- [x] css-shapes-1.md#funcdef-basic-shape-polygon: polygon()'s optional fill-rule defaults to nonzero; each length-percentage pair gives a vertex as an offset from the reference box's top-left; the UA closes the polygon by joining the last vertex to the first [css-basic-shapes#polygon css-basic-shapes#polygon-fill-rule]
- [x] css-shapes-1.md#funcdef-basic-shape-polygon: an optional "round <length>" after fill-rule rounds every vertex with a circle tangent to both adjacent sides, clamped per vertex to at most half of either adjacent segment [clip-path-polygon-round#convex clip-path-polygon-round#concave clip-path-polygon-round#clamped clip-path-polygon-round#fill-rule clip-path-polygon-round#zero clip-path-polygon-round#invalid]

## shape(): basic commands: css-shape-function, clip-path-shape (exists)

- [x] css-shapes-1.md#shape-function: shape() takes an optional fill-rule, then "from <position>" for the starting point, then a comma-separated list of shape-commands; it is a superset of path() using CSS syntax (units, calc(), custom properties) instead of an SVG path string [css-shape-function#syntax]
- [x] css-shapes-1.md#typedef-shape-command-end-point: each command's end point is given either "to <position>" (absolute, from the reference box's top-left) or "by <coordinate-pair>" (relative to the command's starting point) [css-shape-function#to-by]
- [x] css-shapes-1.md#typedef-shape-move-command: move draws nothing and starts a new subpath; css-shapes-1.md#typedef-shape-line-command: line draws a straight line to its end point [css-shape-function#move-line]
- [x] css-shapes-1.md#typedef-shape-horizontal-line-command: hline moves only the horizontal coordinate, keeping the vertical one from the start point, with "to" accepting left/center/right/x-start/x-end; css-shapes-1.md#typedef-shape-vertical-line-command: vline does likewise for the vertical coordinate with top/center/bottom/y-start/y-end [css-shape-function#hline-vline]
- [x] css-shapes-1.md#typedef-shape-curve-command: curve draws a quadratic Bezier with one control point, or a cubic Bezier with two [css-shape-function#curve]
- [x] css-shapes-1.md#typedef-shape-smooth-command: smooth draws a smooth Bezier whose first control point is the reflection of the previous curve's last control point about the start point, or the start point itself if the previous command was not a curve [css-shape-function#smooth]
- [x] css-shapes-1.md#typedef-shape-relative-control-point: a control point's coordinate-pair is relative to the segment's start by default, or to its end point or the reference box's origin when "from end" or "from origin" is given [css-shape-function#control]
- [x] css-shapes-1.md#valdef-shape-close: close adds a closepath, equivalent to a line back to the subpath's start, except its join is rendered as a closed join rather than a cap when stroked [css-shape-function#syntax css-shape-function#move-line]
- [x] css-shapes-1.md#typedef-shape-arc-command: arc draws an elliptical arc to its end point; the required "of" radius (one value resolved against the direction-agnostic size for both axes, or two for horizontal/vertical) and the optional sweep (cw/ccw, default ccw), size (large/small, default small) and rotate (default 0deg, positive clockwise) select among the 4 possible arcs [css-shape-function#arc]
- [x] css-shapes-1.md#typedef-shape-arc-command: arc reuses SVG's elliptical-arc error handling: identical endpoints draw nothing, a zero radius degrades to a line, a negative radius uses its absolute value, and radii too small to connect the endpoints are scaled up uniformly until they do [css-shape-function#arc-edges]

## path() and basic-shape-rect edge cases: clip-path-path-validity, css-basic-shapes, clip-path-syntax-edges (exists)

- [x] css-shapes-1.md#funcdef-basic-shape-path: path()'s optional fill-rule defaults to nonzero, except in a context (such as SVG shapes) where fill-rule is relevant, in which case an omitted value uses the computed fill-rule property's value [clip-path-path-validity#fill-rule clip-path-path-validity#nonzero clip-path-path-validity#d-fill-rule]
- [x] css-shapes-1.md#funcdef-basic-shape-path: path()'s string must be a valid SVG path data string; a non-conforming string, or one that parses to an empty path, makes the whole path() invalid [clip-path-path-validity#empty clip-path-path-validity#no-data clip-path-path-validity#error clip-path-path-validity#d-invalid]
- [x] css-shapes-1.md#funcdef-basic-shape-path: for a property requiring a closed loop (such as clip-path), the UA appends an implicit closepath to a path() string that lacks one [clip-path-path-validity#closed]
- [x] css-shapes-1.md#funcdef-basic-shape-rect: rect()'s auto is 0% for the top/left edges and 100% for the right/bottom edges [css-basic-shapes#rect-auto]

## half-border-box: css-basic-shapes, clip-path-half-border (exists)

- [x] css-shapes-1.md#valdef-shape-box-half-border-box: half-border-box is the box exactly midway between the padding-box and the border-box (for SVG, midway between fill-box and stroke-box) [css-basic-shapes#half-border]
- [x] css-shapes-1.md#typedef-shape-box: <shape-box> extends <visual-box> with margin-box and half-border-box, for properties such as clip-path that accept this type [css-basic-shapes#shape-box]

## Notes

- css-shapes-1.md#funcdef-basic-shape-polygon: the per-vertex rounding clamp ("never more than the smaller of tan(angle/2)*segment/2 evaluated against both adjacent segments") is an exact geometric construction for arbitrary concave/convex polygons that costs real per-vertex work to follow literally.
- css-shapes-1.md#typedef-shape-arc-command: the arc grammar groups "of <length-percentage>{1,2}" with the optional sweep/size/rotate under one && combinator, but only the sweep/size/rotate branches carry their own "?"; read literally this makes "of" mandatory even though the prose describes defaults for every component, which is easy to misread as all-optional.
