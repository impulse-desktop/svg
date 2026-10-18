<!-- https://drafts.csswg.org/css-shapes-1/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Shapes Module Level 1

<a id="w3c-state"></a>Editor’s Draft, 8 October 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-shapes/
**Latest published version: **
: https://www.w3.org/TR/css-shapes/
**Previous Versions: **
: https://www.w3.org/TR/2025/CRD-css-shapes-1-20250612/
**Implementation Report: **
: https://wpt.fyi/results/css/css-shapes
**Feedback: **
: CSSWG Issues Repository
**Editors: **
: Rossen Atanassov (Microsoft Corporation)
: Alan Stearns (Adobe)
: Noam Rosenthal (Google)
**Former Editor: **
: Vincent Hardy
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/css-shapes/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

CSS Shapes describe geometric shapes for use in CSS. For Level 1, CSS Shapes can be applied to floats. A circle shape on a float will cause inline content to [wrap](#wrap) around the circle shape instead of the float’s bounding box.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-shapes” in the title, like this: “[css-shapes] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This section is not normative.*

Shapes define arbitrary geometries that can be used as CSS values. This specification defines properties to control the geometry of an element’s [float area](#float-area). The [shape-outside](#propdef-shape-outside) property uses shape values to define the float area for a float.

Note: Future levels of CSS Shapes will allow use of shapes on elements other than floats. Other CSS modules can make use of shapes as well, such as CSS Masking [[CSS-MASKING]](#biblio-css-masking) and CSS Exclusions [[CSS3-EXCLUSIONS]](#biblio-css3-exclusions).

Note: If a user agent implements both CSS Shapes and CSS Exclusions, the [shape-outside](#propdef-shape-outside) property defines the exclusion area for an exclusion.

Note: A future level of CSS Shapes will define a shape-inside property, which will define a shape to [wrap](#wrap) content within the element.

### <a id="module-interactions"></a>1.1. Module Interactions

This module extends the float features defined in [[CSS2]](#biblio-css2) chapter 9.

### <a id="values"></a>1.2. Values

This specification follows the CSS property definition conventions from [[CSS2]](#biblio-css2) using the value definition syntax from [[CSS-VALUES-3]](#biblio-css-values-3). Value types not defined in this specification are defined in CSS Values & Units [[CSS-VALUES-3]](#biblio-css-values-3). Combination with other CSS modules may expand the definitions of these value types.

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their property value. For readability they have not been repeated explicitly.

### <a id="terminology"></a>1.3. Terminology

*<a id="wrap"></a>Wrap*

This specification uses the term [wrap](#wrap) to refer to flowing content around the sides of a [float area](#float-area), defined in [[CSS2]](#biblio-css2) chapter 9. Content wraps around the right side of a left-floated box, and content wraps around the left side of a right-floated box. One result of this wrapping is that line boxes next to a float are shortened as necessary to avoid intersections with the float area.

*<a id="float-area"></a>Float area*

The area used for [wrapping](#wrap) content around a float element. The rules for float behavior use the sides of the [float area](#float-area) to determine where content flows. By default, the float area is the float element’s [margin box](css-box-4.md#margin-box) (note this can be different than the float area produced by the [margin-box](#valdef-shape-box-margin-box) value, which includes border-radius curvature). This specification’s [shape-outside](#propdef-shape-outside) and [shape-margin](#propdef-shape-margin) properties can be used to define an arbitrary, non-rectangular float area.

*<a id="direction-agnostic-size"></a>direction-agnostic size* The [direction-agnostic size](#direction-agnostic-size) of a box is equal to the length of the diagonal of the box, divided by sqrt(2).

Note: This is a method of averaging the width and height of a box used by SVG in some cases, when a percentage of a box’s size is desired but the context doesn’t specifically favor the width or the height. For square boxes, this is the same as the width/height.

## <a id="relation-to-box-model-and-float-behavior"></a>2. Relation to the box model and float behavior

While the boundaries used for [wrapping](#wrap) inline flow content outside a float can be defined using shapes, the actual box model does not change. If the element has specified margins, borders or padding they will be computed and rendered according to the [[CSS3BOX]](#biblio-css3box) module. Also, float positioning and stacking are not affected by defining a [float area](#float-area) with a shape.

When a shape is used to define a [float area](#float-area), the shape is clipped to the float’s margin box. In other words, a shape can only ever reduce a float area, not increase it. A reduced float area may have no effect on some line boxes that would normally be affected by the float. If a shape does not enclose any area, the shape’s edges are still used to define the float area.

A [float area](#float-area) defined by a shape may reduce the normal float area on all sides, but this does not allow content to [wrap](#wrap) on both sides of a float. Left floats with a [shape-outside](#propdef-shape-outside) still only allow content wrapping on the right side, and right floats only allow wrapping on the left.

<a id="example-ed716b1b"></a>

In the following example the left and right floating `img` elements specify a triangular shape using the [shape-outside](#propdef-shape-outside) property.

```
<img class="left" src="hand.svg"/>
<img class="right" src="hand.svg"/>
<p>
  Sometimes a web page's text content appears to be
  funneling your attention towards a spot on the page
  to drive you to follow a particular link. Sometimes
  you don't notice.
</p>

<style type="text/css">
  .left {
    shape-outside: polygon(0 0, 100% 100%, 0 100%);
    float: left;
    width: 40%;
    height: 12ex;
    transform: scaleX(-1);
  }

  .right {
    shape-outside: polygon(100% 0, 100% 100%, 0 100%);
    float: right;
    width: 40%;
    height: 12ex;
  }

  p {
    text-align: center;
  }
</style>
```

[image: Using the shape-outside property with floats]

<a id="example-c9556a71"></a>

Since shapes are clipped to the float’s margin box, adding this shape to the left float above would result in the same rendering.

```
shape-outside: polygon(0 0, 500% 500%, 0 500%);
```

<a id="example-e8eedd7c"></a>

A shape that does not enclose any area still has edges that contribute to the [float area](#float-area).

This inset shape is a vertical line positioned at the midpoint of the reference box. This midpoint edge is used as the edge of the float area for wrapping content.

```
shape-outside: inset(0% 50% 0% 50%);
```

If inset values add up to more than the width, [CSS Backgrounds 3 § 4.5 Overlapping Curves](css-backgrounds-3.md#corner-overlap) rules are used to determine the edges of the rectangle. This shape results in a vertical edge 25% from the left side of the reference box.

```
shape-outside: inset(0% 150% 50% 0%);
```

If the shape is only a horizontal line, then it is an empty float area and has no effect on wrapping. Note that in this example shape-margin must be 0px (otherwise the line would expand to enclose an area).

```
shape-outside: inset(50% 0% 0% 50%);
shape-margin: 0px;
```

<a id="example-105c1362"></a>

A [shape-outside](#propdef-shape-outside) can create open areas on both the left and right of a [float area](#float-area). Content still [wraps](#wrap) only on one side of a float in this case. In the picture, the shape is rendered in blue, and the content area outside the shape in mauve.

```
shape-outside: polygon(50px 0px, 100px 100px, 0px 100px);
```

[image: wrapping around right side of a left-float float area]

<a id="example-8937ace2"></a>

The following styling creates a shape much smaller than the float’s content area, and adds a margin-top to the float. In the picture, the shape is rendered in blue, the content area outside the shape in mauve, and the margin area of the float box in yellow. The inline content only [wraps](#wrap) around the shape, and otherwise overlays the rest of the float margin box.

```
.float-left {
  shape-outside: polygon(0% 50%, 50% 100%, 0 100%);
  float: left;
  width: 100px;
  height: 100px;
  margin-top: 20px;
}
```

[image: Adding margin-top to a float with a small shape-outside]

The next picture shows a possible result if two of these floats were stacked next to each other. Note that the floats are positioned using their margin boxes, not the [float area](#float-area).

[image: Stacking two floats with a small shape-outside]

## <a id="basic-shape-functions"></a>3. Basic Shapes

The *<a id="typedef-basic-shape"></a><basic-shape>* type can be specified using basic shape functions.

```
<basic-shape> = <basic-shape-rect> | <circle()> | <ellipse()> |  <polygon()> | <path()> | <shape()>
```

When using this syntax to define shapes, the *<a id="basic-shape-reference-box"></a>reference box* is defined by each property that uses [<basic-shape>](#typedef-basic-shape) values. The coordinate system for the shape has its origin on the top-left corner of the [reference box](#basic-shape-reference-box) with the x-axis running to the right and the y-axis running downwards. All the lengths expressed in percentages are resolved from the used dimensions of the reference box.

### <a id="supported-basic-shapes"></a>3.1. Supported Shapes

The [<basic-shape>](#typedef-basic-shape) functions are:

```
<inset()> = inset(
  <length-percentage>{1,4}
  [ round <'border-radius'> ]?
)

<xywh()> = xywh(
  <length-percentage>{2} <length-percentage [0,∞]>{2}
  [ round <'border-radius'> ]?
)

<rect()> = rect(
  [ <length-percentage> | auto ]{4}
  [ round <'border-radius'> ]?
)

<a id="typedef-basic-shape-rect"></a><basic-shape-rect> = <inset()> | <rect()> | <xywh()>

<circle()> = circle(
  <radial-size>?
  [ at <position> ]?
)

<ellipse()> = ellipse(
  <radial-size>?
  [ at <position> ]?
)

<polygon()> = polygon(
  <'fill-rule'>? 
  [ round <length> ]? ,
  [<length-percentage> <length-percentage>]#
)

<path()> = path(
  <'fill-rule'>? ,
  <string>
)

<shape()> = shape(
  <'fill-rule'>?
  from <position> ,
  <shape-command>#
)
```

***<a id="funcdef-basic-shape-inset"></a>inset()* **
: Defines an inset rectangle via insets from each edge of the [reference box](#basic-shape-reference-box).

If less than four [<length-percentage>](css-values-4.md#typedef-length-percentage) values are provided, the omitted values default in the same way as the margin shorthand: an omitted second or third value defaults to the first, and an omitted fourth value defaults to the second.

The four [<length-percentage>](css-values-4.md#typedef-length-percentage)s define the position of the top, right, bottom, and left edges of a rectangle, respectively, as insets from the corresponding edges of the [reference box](#basic-shape-reference-box).

A pair of insets in either dimension that add up to more than the used dimension (such as left and right insets of 75% apiece) use the [CSS Backgrounds 3 § 4.5 Overlapping Curves](css-backgrounds-3.md#corner-overlap) rules to proportionally reduce the inset effect to 100%.

<a id="example-4a20658e"></a> For example, specifying inset(75% 0 50% 0) has the top+bottom edges summing to 125% of the [reference box’s](#basic-shape-reference-box) height. They’re proportionally reduced to sum to 100%, identical to specifying inset(60% 0 40% 0).

The optional [<'border-radius'>](css-backgrounds-3.md#propdef-border-radius) argument(s) define rounded corners for the rectangle using the border-radius shorthand syntax.

***<a id="funcdef-basic-shape-xywh"></a>xywh()* **
: Defines a rectangle via offsets from the top and left edge of the [reference box](#basic-shape-reference-box), and a specified width and height.

The four [<length-percentage>](css-values-4.md#typedef-length-percentage)s define, respectively, the inset from the left edge of the [reference box](#basic-shape-reference-box), the inset from the top edge of the reference box, the width of the rectangle, and the height of the rectangle.

Note: This syntax is inspired by the `[viewBox](svg2/coords.md#TermViewBox)` attribute from SVG.

The optional [<'border-radius'>](css-backgrounds-3.md#propdef-border-radius) argument(s) define rounded corners for the inset rectangle using the border-radius shorthand syntax.

***<a id="funcdef-basic-shape-rect"></a>rect()* **
: Defines a rectangle via insets from the top and left edges of the [reference box](#basic-shape-reference-box).

The four [<length-percentage>](css-values-4.md#typedef-length-percentage)s define the position of the top, right, bottom, and left edges of a rectangle, respectively, as insets from the top edge of the [reference box](#basic-shape-reference-box) (for the first and third values) or the left edge of the reference box (for the second and fourth values).

An auto value makes the edge of the box coincide with the corresponding edge of the [reference box](#basic-shape-reference-box): it’s equivalent to 0% as the first (top) or fourth (left) value, and equivalent to 100% as the second (right) or third (bottom) value.

The second (right) and third (bottom) values are floored by the fourth (left) and second (top) values, respectively.

<a id="example-203cf4f2"></a> For example, specifying rect(10px 0 0 20px) would place the bottom edge higher than the top edge, and the right edge further left than the left edge, so both are corrected to not cross over the other edge, identical to specifying rect(10px 20px 10px 20px).

Note: This syntax is similar, but not quite identical, to the legacy [rect()](#funcdef-basic-shape-rect) function used solely by the [clip](css-masking-1.md#propdef-clip) property.

The optional [<'border-radius'>](css-backgrounds-3.md#propdef-border-radius) argument(s) define rounded corners for the rectangle using the border-radius shorthand syntax.)

***<a id="funcdef-basic-shape-circle"></a>circle()* **
:

-
The [<radial-size>](css-images-3.md#typedef-radial-size) argument defines the circle’s radius. Rather than referring to the gradient box, values are resolved against the [reference box](#basic-shape-reference-box). Two [<length-percentage>](css-values-4.md#typedef-length-percentage) values are invalid. If omitted it defaults to closest-side.
-
The <position> argument defines the center of the circle. Unless otherwise specified, this defaults to center if omitted.

***<a id="funcdef-basic-shape-ellipse"></a>ellipse()* **
:

-
The [<radial-size>](css-images-3.md#typedef-radial-size) argument defines the horizontal and vertical radiuses of the ellipse. Rather than referring to the gradient box, values are resolved against the [reference box](#basic-shape-reference-box). If omitted it defaults to closest-side.
-
The <position> argument defines the center of the ellipse. Unless otherwise specified, this defaults to center if omitted.

***<a id="funcdef-basic-shape-polygon"></a>polygon()* **
:

-
The [<'fill-rule'>](svg2/painting.md#FillRuleProperty) specifies the filling rule used to determine the interior. Defaults to [nonzero](css-masking-1.md#valdef-clip-rule-nonzero) if omitted.
-
An optional [<length>](css-values-4.md#length-value) after a round keyword defines rounding for each vertex of the polygon. The length is the radius of a circle whose center lies on the bisector of the smaller angle of the vertex, and that is tangential to both sides of the vertex.
<a id="example-1217097f"></a>
[image: rounding concave and convex polygon vertices]
Rounding polygon vertices that are both convex and concave.
To avoid rounding more than half of any line segment, the rounding of each vertex must be clamped separately such that the radius is never more than the smaller of `tan(angle/2) segment / 2` evaluated against both vertex line segments.
<a id="example-f443dfa5"></a>
[image: visualization of clamp formula]
This diagram shows the intent of the clamping formula.
-
Each [<length-percentage>](css-values-4.md#typedef-length-percentage) pair specifies a vertex of the polygon, as a horizontal and vertical offset from the left and top edges of the [reference box](#basic-shape-reference-box).

The UA must close a polygon by connecting the last vertex with the first vertex of the list.

***<a id="funcdef-basic-shape-path"></a>path()* **
:

-
The [<'fill-rule'>](svg2/painting.md#FillRuleProperty) specifies the filling rule used to determine the interior. Defaults to [nonzero](css-masking-1.md#valdef-clip-rule-nonzero) if omitted, unless the function is being used in a context such as SVG shapes where the fill-rule property is relevant. In that case an omitted value will use the computed value of the fill-rule property.
-
The [<string>](css-values-4.md#string-value) represents an [SVG Path data string](svg11/paths.md#PathData). A path data string that does not conform to the to the grammar and parsing rules of SVG 1.1, or that does conform but defines an empty path, is [invalid](css-syntax-3.md#css-invalid) and causes the entire [path()](#funcdef-basic-shape-path) to be invalid.
The initial position is defined by the first “move to” argument in the path string. For the initial direction follow SVG 1.1.

The UA must close a path with an implicit closepath command ("z" or "Z") if it is not present in the string for properties that require a closed loop (such as [shape-outside](#propdef-shape-outside) and [clip-path](css-masking-1.md#propdef-clip-path)).

***<a id="funcdef-basic-shape-shape"></a>shape()* **
: See [The shape() function](#shape-function).
#### <a id="shape-function"></a>3.1.1. The [shape()](#funcdef-basic-shape-shape) Function

While the [path()](#funcdef-basic-shape-path) function allows reuse of the SVG path syntax to define more arbitrary shapes than allowed by more specialized shape functions, it requires writing a path as a single string (which is not compatible with, for example, building a path piecemeal with var()), and inherits a number of limitations from SVG, such as implicitly only allowing the [px](css-values-4.md#px) unit.

The [shape()](#funcdef-basic-shape-shape) function uses a set of commands roughly equivalent to the ones used by [path()](#funcdef-basic-shape-path), but does so with more standard CSS syntax, and allows the full range of CSS functionality, such as additional units and math functions. The commands used by shape() are dynamically turned into path segments when it is used for rendering, e.g., when computing the rendered [clip-path](css-masking-1.md#propdef-clip-path).

In that sense, [shape()](#funcdef-basic-shape-shape) is a superset of [path()](#funcdef-basic-shape-path). A path() can be easily converted to a shape(), but to convert a shape() back to a path() or to SVG requires information about the CSS environment (e.g. current values of CSS custom properties, current font size for [em](css-values-4.md#em) units, etc).

The [<'fill-rule'>](svg2/painting.md#FillRuleProperty) is interpreted identically to the same argument in [path()](#funcdef-basic-shape-path).

The rest of the arguments define a list of path data commands, identical to that of an [SVG Path](svg11/paths.md#PathData), which the function represents.

The from [<coordinate-pair>](#typedef-shape-coordinate-pair) represents the starting point for the first shape-command. It adds an initial [absolute moveto](svg2/paths.md#PathDataMovetoCommands) to the list of path data commands.

The sequence of *<a id="typedef-shape-command"></a>[<shape-command>](#typedef-shape-command)*s represent further [path data commands](svg11/paths.md#PathData). Each command’s starting point is the previous command’s ending point.

```
<shape-command> = <move-command> | <line-command> | close |
                  <horizontal-line-command> | <vertical-line-command> |
                  <curve-command> | <smooth-command> | <arc-command>

<move-command> = move <command-end-point>
<line-command> = line <command-end-point>
<horizontal-line-command> = hline
        [ to [ <length-percentage> | left | center | right | x-start | x-end ]
        | by <length-percentage> ]
<vertical-line-command> = vline
        [ to [ <length-percentage> | top | center | bottom | y-start | y-end ]
        | by <length-percentage> ]
<curve-command> = curve
        [ [ to <position> with <control-point> [ / <control-point> ]? ]
        | [ by <coordinate-pair> with <relative-control-point> [ / <relative-control-point> ]? ] ]
<smooth-command> = smooth
        [ [ to <position> [ with <control-point> ]? ]
        | [ by <coordinate-pair> [ with <relative-control-point> ]? ] ]
<arc-command> = arc <command-end-point>
            [ [ of <length-percentage>{1,2} ]
              && <arc-sweep>? && <arc-size>? && [rotate <angle>]? ]

<command-end-point> = [ to <position> | by <coordinate-pair> ]
<control-point> = [ <position> | <relative-control-point> ]
<relative-control-point> = <coordinate-pair> [ from [ start | end | origin ] ]?
<coordinate-pair> = <length-percentage>{2}
<arc-sweep> = cw | ccw
<arc-size> = large | small
```

***<a id="typedef-shape-coordinate-pair"></a>[<coordinate-pair>](#typedef-shape-coordinate-pair)* = [<length-percentage>](css-values-4.md#typedef-length-percentage){2} **
: Defines a pair of coordinates, representing a rightward and downward offset, respectively, from a specified reference point. Percentages are resolved against the width or height, respectively, of the [reference box](#basic-shape-reference-box).
***<a id="typedef-shape-command-end-point"></a>[<command-end-point>](#typedef-shape-command-end-point)* = [ *<a id="valdef-shape-to"></a>to* <position> | *<a id="valdef-shape-by"></a>by* [<coordinate-pair>](#typedef-shape-coordinate-pair) ] **
: Every command can be specified in "absolute" or "relative" coordinates, determined by their [by](#valdef-shape-by) or [to](#valdef-shape-to) component. to indicates that any [<coordinate-pair>](#typedef-shape-coordinate-pair)s in the command are relative to the top-left corner of the [reference box](#basic-shape-reference-box), while by indicates that the <coordinate-pair>s are relative to the command’s starting point.

[<relative-control-point>](#typedef-shape-relative-control-point) defines how [by](#valdef-shape-by) and [to](#valdef-shape-to) are interpreted for curve control points, while [<horizontal-line-command>](#typedef-shape-horizontal-line-command) and [<vertical-line-command>](#typedef-shape-vertical-line-command) define how by and to are interpreted for horizontal and vertical lines, respectively.

When [to](#valdef-shape-to) is used, the coordinates can be specified as <position>s instead of [<coordinate-pair>](#typedef-shape-coordinate-pair)s.

Note: In either case, [<percentage>](css-values-4.md#percentage-value) values in [<coordinate-pair>](#typedef-shape-coordinate-pair)s are always computed relative to the [reference box’s](#basic-shape-reference-box) size.

***<a id="typedef-shape-move-command"></a>[<move-command>](#typedef-shape-move-command)* = *<a id="valdef-shape-move"></a>move* [<command-end-point>](#typedef-shape-command-end-point) **
: Adds a [moveto](svg2/paths.md#PathDataMovetoCommands) command to the list of path data commands, with an ending point specified by the [<coordinate-pair>](#typedef-shape-coordinate-pair).

This draws nothing, and merely "moves the pen" for the next command.

Note: This starts a new subpath, for the purpose of the [close](#valdef-shape-close) command.

***<a id="typedef-shape-line-command"></a>[<line-command>](#typedef-shape-line-command)* = *<a id="valdef-shape-line"></a>line* [<command-end-point>](#typedef-shape-command-end-point) **
: Adds a [lineto](svg2/paths.md#PathDataLinetoCommands) command to the list of path data commands, with an ending point specified by the [<coordinate-pair>](#typedef-shape-coordinate-pair).

This draws a straight line from the command’s starting point to its ending point.

***<a id="typedef-shape-horizontal-line-command"></a>[<horizontal-line-command>](#typedef-shape-horizontal-line-command)* = hline [ to [ [<length-percentage>](css-values-4.md#typedef-length-percentage) | left | center | right | x-start | x-end ] | by <length-percentage> ] **
: Adds a horizontal [lineto](svg2/paths.md#PathDataLinetoCommands) command to the list of path data commands.

This is equivalent to a [line](#valdef-shape-line) command with the [<length-percentage>](css-values-4.md#typedef-length-percentage) given as the horizontal component of the [<coordinate-pair>](#typedef-shape-coordinate-pair). Specifying the horizontal component of <position> instead of a <length-percentage> (left, center, right, x-start, or x-end), would draw a line to that <position>, with the <position>’s vertical component remaining the same as the starting point.

***<a id="typedef-shape-vertical-line-command"></a>[<vertical-line-command>](#typedef-shape-vertical-line-command)* = vline [ to [ [<length-percentage>](css-values-4.md#typedef-length-percentage) | top | center | bottom | y-start | y-end ] | by <length-percentage> ] **
: Adds a vertical [lineto](svg2/paths.md#PathDataLinetoCommands) command to the list of path data commands.

This is equivalent to a [line](#valdef-shape-line) command with the [<length-percentage>](css-values-4.md#typedef-length-percentage) given as the vertical component of the [<coordinate-pair>](#typedef-shape-coordinate-pair). Specifying the horizontal component of <position> (top, center, bottom, y-start, or y-end) instead of a <length-percentage>, would draw a line to that <position>, with the <position>’s horizontal component remaining the same as the starting point.

***<a id="typedef-shape-curve-command"></a>[<curve-command>](#typedef-shape-curve-command)* = *<a id="valdef-shape-curve"></a>curve* [ [ to <position> with [<control-point>](#typedef-shape-control-point) [ / <control-point> ]? ] | [ by [<coordinate-pair>](#typedef-shape-coordinate-pair) with [<relative-control-point>](#typedef-shape-relative-control-point) [ / <relative-control-point> ]? ] ] **
: Adds a Bézier curve command to the list of path data commands, ending at the point specified by the <position> following the [to](#valdef-shape-to) keyword, or the [<coordinate-pair>](#typedef-shape-coordinate-pair) following the [by](#valdef-shape-by) keyword, as specified by [<command-end-point>](#typedef-shape-command-end-point).

The with component specifies control points for the curve: if a single [<control-point>](#typedef-shape-control-point) or [<relative-control-point>](#typedef-shape-relative-control-point) is provided, the command specifies a [quadratic curve](svg2/paths.md#PathDataQuadraticBezierCommands); if two <control-point>s or <relative-control-point>s are provided, it specifies a [cubic curve](svg2/paths.md#PathDataCubicBezierCommands).

***<a id="typedef-shape-smooth-command"></a>[<smooth-command>](#typedef-shape-smooth-command)* = *<a id="valdef-shape-smooth"></a>smooth* [ [ to <position> [with [<control-point>](#typedef-shape-control-point) ]? ] | [ by [<coordinate-pair>](#typedef-shape-coordinate-pair) [ with [<relative-control-point>](#typedef-shape-relative-control-point) ]? ] ] **
: Adds a smooth Bézier curve command to the list of path data commands, ending at the point specified by the <position> following the [to](#valdef-shape-to) keyword, or the [<coordinate-pair>](#typedef-shape-coordinate-pair) following the [by](#valdef-shape-by) keyword, as specified by [<command-end-point>](#typedef-shape-command-end-point). The with component specifies control points for the curve: if it’s omitted, the command specifies a [smooth quadratic curve](svg2/paths.md#PathDataQuadraticBezierCommands); if it’s provided, if specifies a [smooth cubic curve](svg2/paths.md#PathDataCubicBezierCommands).

Note: A [smooth](#valdef-shape-smooth) command is equivalent to a [curve](#valdef-shape-curve) command with the first control point automatically specified as the reflection of the previous curve’s second control point around the starting point, or as the starting point if the previous path data command wasn’t a curve. This ensures G1 continuity with the previous command, so the curve appears to smoothly continue from the previous command, rather than possibly making a sudden direction change.

***<a id="typedef-shape-control-point"></a>[<control-point>](#typedef-shape-control-point)* = [ <position> | [<relative-control-point>](#typedef-shape-relative-control-point) ] **
: Provides a control point to a quadratic or cubic Bézier curve.
***<a id="typedef-shape-relative-control-point"></a>[<relative-control-point>](#typedef-shape-relative-control-point)* = [<coordinate-pair>](#typedef-shape-coordinate-pair) [ from [ start | end | origin ] ]? **
: Provides a control point to a quadratic or cubic Bézier curve. When a from keyword is specified followed by start, end, or origin, the given [<coordinate-pair>](#typedef-shape-coordinate-pair) is relative to the command’s starting point, the command’s end point, or the [reference box](#basic-shape-reference-box), respectively. If such component is not provided, the <coordinate-pair> is relative to the segment’s start.
***<a id="typedef-shape-arc-command"></a>[<arc-command>](#typedef-shape-arc-command)* = *<a id="valdef-shape-arc"></a>arc* [<command-end-point>](#typedef-shape-command-end-point) [of [<length-percentage>](css-values-4.md#typedef-length-percentage){1,2}] && [<arc-sweep>](#typedef-shape-arc-sweep)? && [<arc-size>](#typedef-shape-arc-size)? && rotate [<angle>](css-values-4.md#angle-value)? ] **
: Add an [elliptical arc](svg2/paths.md#PathDataEllipticalArcCommands) command to the list of path data commands, ending at the [<command-end-point>](#typedef-shape-command-end-point).

The of component specifies the size of the ellipse that the arc is taken from. The first [<length-percentage>](css-values-4.md#typedef-length-percentage) provides the horizontal radius of the ellipse and the second provides the vertical radius. Like for [<coordinate-pair>](#typedef-shape-coordinate-pair)s, [<percentage>](css-values-4.md#percentage-value) values are resolved against the width or height of the [reference box](#basic-shape-reference-box), as appropriate.

If only one [<length-percentage>](css-values-4.md#typedef-length-percentage) is provided, both radiuses use the provided value. In that case, [<percentage>](css-values-4.md#percentage-value) values are resolved against the [direction-agnostic size](#direction-agnostic-size) of the [reference box](#basic-shape-reference-box) (similar to the [circle()](#funcdef-basic-shape-circle) function).

Note that SVG has [some specific error-handling for the ellipse radiuses](svg2/paths.md#ArcOutOfRangeParameters):

-
if the endpoint is the same as the starting point, the command does nothing
-
if either radius is zero, the command is equivalent to a [<line-command>](#typedef-shape-line-command) to the ending point
-
if either radius is negative, its absolute value is used instead
-
if the radiuses don’t describe an ellipse large enough to intersect both the starting point and ending point (after rotation by the specified [<angle>](css-values-4.md#angle-value)), they are scaled up uniformly until the ellipse is just large enough to reach.

The ellipse described by the specified radiuses defaults to being axis-aligned, but can be rotated by specifying an [<angle>](css-values-4.md#angle-value). Similar to the [rotate()](css-transforms-1.md#funcdef-transform-rotate) transform function, positive angles specify a clockwise rotation, and negative angles specify a counterclockwise rotation. If omitted, this defaults to 0deg.

The ending point, radiuses, and angle, taken together, usually define two possible ellipses that intersect the starting point and ending point, and each ellipse can be traced in either direction, for a total of four possible arcs. The [<arc-sweep>](#typedef-shape-arc-sweep) and [<arc-size>](#typedef-shape-arc-size) components specify which of these arcs is desired:

-
*<a id="typedef-shape-arc-sweep"></a>[<arc-sweep>](#typedef-shape-arc-sweep)* can be *<a id="valdef-shape-cw"></a>cw* or *<a id="valdef-shape-ccw"></a>ccw*, indicating that the arc that is traced around the ellipse clockwise or counter-clockwise from the center, respectively, must be chosen. If omitted, this defaults to [ccw](#valdef-shape-ccw).
Note: In the SVG arc command, [cw](#valdef-shape-cw) corresponds to the value 1 for the sweep flag, and [ccw](#valdef-shape-ccw) to the value 0.
-
*<a id="typedef-shape-arc-size"></a>[<arc-size>](#typedef-shape-arc-size)* can be *<a id="valdef-shape-large"></a>large* or *<a id="valdef-shape-small"></a>small*, indicating that the larger or smaller, respectively, of the two possible arcs must be chosen. If omitted, this defaults to [small](#valdef-shape-small).
Note: In the SVG arc command, [large](#valdef-shape-large) corresponds to the value 1 for the large flag, and [small](#valdef-shape-small) to the 0.
Note: If the starting and ending points are on exactly opposite sides of the ellipse, both possible arcs are the same size, but also there is only one possible ellipse. In this case, the [<arc-sweep>](#typedef-shape-arc-sweep) distinguishes which of the two possible arcs will be chosen, and [<arc-size>](#typedef-shape-arc-size) has no effect.

[image: a depiction of four possible arcs given a start and end point]

A depiction of the two possible ellipses, and four possible arcs, that can be chosen between.

***<a id="valdef-shape-close"></a>close* **
: Adds a [closepath](svg2/paths.md#PathDataClosePathCommand) command to the list of path data commands.

Note: This is similar to a [line](#valdef-shape-line) command with its ending point set to the starting point of the subpath. When specifying a raw shape, they’re identical, but if the path is stroked, the ending point of the [close](#valdef-shape-close) command is smoothly joined with the start of the subpath, which affects how line-joins and line-caps are rendered.

##### <a id="shape-examples"></a>3.1.1.1. Using [shape()](#funcdef-basic-shape-shape) to create responsive, parametric speech bubble

<a id="example-c1f9dfe4"></a> The [shape()](#funcdef-basic-shape-shape) function enables shapes that are responsive, rather than scalable. While the [polygon()](#funcdef-basic-shape-polygon) shape is also responsive, it only support simple rounded corners and not complex curves.

To demonstrate, let’s start with a speech bubble, such as the following:

[image: A speech bubble shape]

Using this shape with a clip-path can be done by using the [path()](#funcdef-basic-shape-path) function:

```
.bubble { clip-path: path("m 5 0 H 95 Q 100 0 100 5 V 92 Q 100 97 95 97 H 70 l -2 3 l -2 -3 H 5 Q 0 97 0 92 V 5 Q 0 0 5 0") };
```

Altohugh this path can easily scale, the scaled results are not always desirable. e.g. when scaled to a small balloon, the arrow and corners are scaled to become almost invisible:

[image: scled-down speech bubble shape]

To construct this shape using the [shape()](#funcdef-basic-shape-shape) function, let’s start by turning all the pixel values from the path function to percentages. Note that the shape() function begins with from:

```
.bubble { clip-path: shape(	from 5% 0%,
              hline to 95%,
              curve to 100% 5% with 100% 0%,
              vline to 92%,
              curve to 95% 97% with 100% 97%,
              hline to 70%,
              line by -2% 3%,
              line by -2% -3%,
              hline to 5%,
              curve to 0% 92% with 0% 97%,
              vline to 5%,
              curve to 5% 0% with 0% 0%); }
```

To make this path responsive, as in, respond well to size changes, we will convert some of its units to [px](css-values-4.md#px) values, specifically the ones the control the curves and arrows:

```
.bubble { clip-path: shape(	from 5px 0%,
              hline to calc(100% - 5px),
              curve to 100% 5px with 100% 0%,
              vline to calc(100% - 8px),
              curve to calc(100% - 5px) calc(100% - 3px) with 100% calc(100% - 3px),
              hline to 70%,
              line by -2px 3px,
              line by -2px -3px,
              hline to 5px,
              curve to 0% calc(100% - 8px) with 0% calc(100% - 3px),
              vline to 5px,
              curve to 5px 0% with 0% 0%); }
```

When applied as clip-path, it would looks like the following:

[image: A speech bubble shape, using clip-path]

The whole speech bubble is scaled to the reference box, while the curves and arrows stay more constant.

Since [shape()](#funcdef-basic-shape-shape) uses CSS units, we can replace some of the edges with position values:

```
.bubble { clip-path: shape(from 5px 0,
              hline to calc(100% - 5px),
              curve to right 5px with right top,
              vline to calc(100% - 8px),
              curve to calc(100% - 5px) calc(100% - 3px) with right calc(100% - 3px),
              hline to 70%,
              line by -2px 3px,
              line by -2px -3px,
              hline to 5px,
              curve to left calc(100% - 8px) with left calc(100% - 3px),
              vline to 5px,
              curve to 5px top with left top); }
```

Another useful feature of [shape()](#funcdef-basic-shape-shape) is that it can be used alongside CSS properties. In this case, we can make the arrow and radius parametric:

```
:root {
  --radius: 5px;
  --arrow-length: 3px;
  --arrow-half-width: 2px;
  --arrow-position: 70%;
  --arrow-bottom-offset: calc(100% - var(--radius) - var(--arrow-length));
}

.bubble {
  animation: bubble 100ms;
  clip-path: shape(from var(---radius) top,
    hline to calc(100% - var(---radius)),
    curve to right var(---radius) with right top,
    vline to var(---arrow-bottom-offset),
    curve to calc(100% - var(---radius)) calc(100% - var(---arrow-length))
              with right calc(100% - var(---arrow-length)),
    hline to var(---arrow-position),
    line by var(---arrow-half-width) var(---arrow-length),
    line by var(---arrow-half-width) calc(0px - var(---arrow-length)),
    hline to var(---radius),
    curve to left var(---arrow-bottom-offset) with left calc(100% - var(---arrow-length)),
    vline to var(---radius),
    curve to var(---radius) top with left top); }
```

##### <a id="interpolating-shape"></a>3.1.1.2. Interpolating the [shape()](#funcdef-basic-shape-shape) Function

[shape()](#funcdef-basic-shape-shape) and [path()](#funcdef-basic-shape-path) functions can be [interpolated](css-values-4.md#interpolation) with each other if their associated list of path data commands is the same length and has the same commands, in order, with the first command of the path() function interpolating with the initial [<coordinate-pair>](#typedef-shape-coordinate-pair) in the shape() function.

Note: The first command of a [path()](#funcdef-basic-shape-path) function is guaranteed to be a [move](#valdef-shape-move), see [moveTo](svg2/paths.md#PathDataMovetoCommands) in the SVG spec.

If the starting and ending values are both [path()](#funcdef-basic-shape-path) functions, the interpolated value is a path() function; otherwise it’s a [shape()](#funcdef-basic-shape-shape) function. In either case, the interpolated value must represent the same list of path data commands, with each command having its numerical components interpolated between the corresponding components of the starting and ending list.

For this purpose, commands are "the same" if they use the same command keyword, and use the same <by-to> keyword. For [curve](#valdef-shape-curve) and [smooth](#valdef-shape-smooth), they also must have the same number of control points.

If an [arc](#valdef-shape-arc) command has different [<arc-sweep>](#typedef-shape-arc-sweep) between its starting and ending list, then the interpolated result uses [cw](#valdef-shape-cw) for any progress value between 0 and 1. If it has different [<arc-size>](#typedef-shape-arc-size) keywords, then the interpolated result uses [large](#valdef-shape-large) for any progress value between 0 and 1.

Note: The [arc](#valdef-shape-arc) keyword interpolation rules are meant to match existing SVG `path` interpolation rules.

### <a id="basic-shape-computed-values"></a>3.2. Computed Values of Basic Shapes

The values in a [<basic-shape>](#typedef-basic-shape) function are computed as specified, with these exceptions:

- Omitted values are included and compute to their defaults.
- A <position> value in [circle()](#funcdef-basic-shape-circle) or [ellipse()](#funcdef-basic-shape-ellipse) is computed as a pair of offsets (horizontal then vertical) from the top left origin, each given as a [<length-percentage>](css-values-4.md#typedef-length-percentage).
- A [<'border-radius'>](css-backgrounds-3.md#propdef-border-radius) value in a [<basic-shape-rect>](#typedef-basic-shape-rect) function is computed as an expanded list of all eight [<length-percentage>](css-values-4.md#typedef-length-percentage) values.
- All [<basic-shape-rect>](#typedef-basic-shape-rect) functions compute to the equivalent [inset()](#funcdef-basic-shape-inset) function.
Note: Given rect(t r b l), the equivalent function is inset(t calc(100% - r) calc(100% - b) l). Given xywh(x y w h), the equivalent function is inset(y calc(100% - x - w) calc(100% - y - h) x).

### <a id="basic-shape-serialization"></a>3.3. Serialization of Basic Shapes

To serialize the [<basic-shape>](#typedef-basic-shape) functions, serialize as per their individual grammars, in the order the grammars are written in, joining space-separated tokens with a single space, and following each serialized comma with a single space. For serializing [computed values](css-cascade-5.md#computed-value), component values are computed, and omitted when possible without changing the meaning.

<a id="example-1f2eff1c"></a>

As [specified value](css-cascade-5.md#specified-value) serialization of the shape functions are relatively trivial, here are some examples of [computed value](css-cascade-5.md#computed-value) serializations for [circle()](#funcdef-basic-shape-circle) notations when used in [shape-outside](#propdef-shape-outside):

-
<position> [serialization rules](css-values-4.md#position-serialization) mean that keywords [compute to](css-cascade-5.md#computed-value) percentages, and serialize in horizontal-vertical order.
```
circle(at bottom left)
/* serializes to */
circle(at 0% 100%)
```

-
Omitting optional components means that default values do not show up in the serialization.
```
circle(closest-side at center)
/* serializes to */
circle()
```

-
Value [computation](css-cascade-5.md#computed) means that some functions [canonicalize to a different form](#basic-shape-computed-values).
```
rect(10px 20px 30px 40px)
/* serializes to */
inset(10px calc(100% - 20px) calc(100% - 30px) 40px)
```

### <a id="basic-shape-interpolation"></a>3.4. Interpolation of Basic Shapes

For interpolating between one basic shape and a second, the rules below are applied. The values in the shape functions interpolate by computed value. The list values interpolate as length, percentage, or calc where possible. If list values are not one of those types but are identical (such as finding [nonzero](css-masking-1.md#valdef-clip-rule-nonzero) in the same list position in both lists) those values do interpolate.

- Both shapes must use the same [reference box](#basic-shape-reference-box).
- If both shapes are the same type, that type is [ellipse()](#funcdef-basic-shape-ellipse) or [circle()](#funcdef-basic-shape-circle), and the radiuses are specified as [<length-percentage>](css-values-4.md#typedef-length-percentage) (rather than keywords), interpolate between each value in the shape functions.
- If both shapes are of type [inset()](#funcdef-basic-shape-inset), interpolate between each value in the shape functions.
- If both shapes are of type [polygon()](#funcdef-basic-shape-polygon), both polygons have the same number of vertices, and use the same [<'fill-rule'>](svg2/painting.md#FillRuleProperty), interpolate between each value in the shape functions.
- In all other cases no interpolation is specified.

## <a id="shapes-from-image"></a>4. Shapes from Image

Another way of defining shapes is by specifying a source [<image>](css-images-3.md#typedef-image) whose alpha channel is used to compute the shape. The shape is computed to be the path or paths that enclose the area(s) where the opacity of the specified image is greater than the [shape-image-threshold](#propdef-shape-image-threshold) value. The absence of any pixels with an alpha value greater than the specified threshold results in an empty float area that will not affect wrapping. If the shape-image-threshold is not specified, the initial value to be considered is 0.0.

The image is sized and positioned as if it were a replaced element whose specified width and height are the same as the element’s used content box size.

For animated raster image formats (such as GIF), the first frame of the animation sequence is used.

<a id="example-12b6dfcf"></a>

An image is floating to the left of a paragraph. The image shows the 3D version of the CSS logo over a transparent background. The logo has a shadow using an alpha-channel.

The image defines its [float area](#float-area) through the [shape-outside](#propdef-shape-outside) property.

```
  <p>
    <img id="CSSlogo" src="CSS-logo1s.png"/>
    blah blah blah blah...
  </p>

  <style>
    #CSSlogo {
      float: left;
      shape-outside: attr(src url);
      shape-image-threshold: 0.1;
    }
  </style>
```

The [shape-outside](#propdef-shape-outside) property re-uses the url from the src attribute of the img element.

It is perfectly possible to display an image and use a different image for its [float area](#float-area).

In the figure below, the alpha-channel threshold is represented by the dotted line around the CSS logo.

It’s then possible to affect where the lines of the paragraph start in three ways:

1. Modifying the alpha channel in the image
2. Changing the value of the [shape-image-threshold](#propdef-shape-image-threshold) property
3. Changing the value of the [shape-margin](#propdef-shape-margin) property (see example 8)

[image: A float shape around an image using its alpha-channel]

A float shape around an image using its alpha-channel.

## <a id="shapes-from-box-values"></a>5. Shapes from Box Values

Shapes can be defined by reference to edges in the CSS Box Model. These edges include border-radius curvature [[CSS3BG]](#biblio-css3bg) from the used border-radius values. The [<shape-box>](#typedef-shape-box) value extends the [<visual-box>](css-box-4.md#typedef-visual-box) value to include [margin-box](#valdef-shape-box-margin-box). Its syntax is:

```
<a id="typedef-shape-box"></a><shape-box> = <visual-box> | margin-box | half-border-box
```

The definitions of the values are:

The *<a id="valdef-shape-box-margin-box"></a>margin-box* value defines the shape enclosed by the outside margin edge. The corner radii of this shape are determined by the corresponding border-radius and margin values. If the ratio of `border-radius/margin` is 1 or more, or margin is negative or zero, then the margin box corner radius is `max(border-radius + margin, 0)`. The margin box’s corners are [outset-adjusted](css-backgrounds-3.md#outset-adjusted-border-radius) given the element’s [border box](css-box-4.md#border-box), [border-radius](css-backgrounds-3.md#propdef-border-radius), and the margin.

The *<a id="valdef-shape-box-border-box"></a>border-box* value defines the shape enclosed by the outside border edge. This shape follows all of the normal border radius shaping rules for the outside of the border.

The *<a id="valdef-shape-box-padding-box"></a>padding-box* value defines the shape enclosed by the outside padding edge. This shape follows all of the normal border radius shaping rules for the inside of the border.

The *<a id="valdef-shape-box-content-box"></a>content-box* value defines the shape enclosed by the outside content edge. Each corner radius of this box is the larger of 0 or `border-radius - border-width - padding`.

The *<a id="valdef-shape-box-half-border-box"></a>half-border-box* value defines the shape enclosed by a box that is in the middle between the [padding-box](#valdef-shape-box-padding-box) and the [border-box](#valdef-shape-box-border-box).

<a id="example-3c169cbd"></a>

Given the 100px square below with 10px padding, border and margins, the box values define these shapes:

- [margin-box](#valdef-shape-box-margin-box): the shape containing all of the yellow pixels
- [border-box](#valdef-shape-box-border-box): the shape containing all of the black pixels
- [padding-box](#valdef-shape-box-padding-box): the shape containing all of the mauve pixels
- [content-box](#valdef-shape-box-content-box): the shape containing all of the blue pixels

[image: Colored boxes representing simple box edges]

Simple CSS Box Model Edges

And the same definitions apply to a more complex example with the same 100px square, but with these border, padding and margin properties:

```
  border-radius: 20px 20px 20px 40px;
  border-width: 30px 10px 20px 10px;
  padding: 10px 20px 10px 10px;
  margin: 20px 10px 10px 10px;
```

[image: Colored boxes representing complex box edges]

Complex CSS Box Model Edges

<a id="example-ae456cb1"></a>

The difference between normal float wrapping and wrapping around the shape defined by the margin-box value is that the margin-box shape includes corner shaping. Take the 100px square with 10px padding, border and margins, but with a border-radius of 60px. If you make a left float out of it, content normally wraps in this manner:

[image: Text wrapping around float with no shape]

Normal float wrapping

If you add a margin-box shape to the float, then content wraps around the rounded margin-box corners.

```
  shape-outside: margin-box;
```

[image: Text wrapping around float with margin-box shape]

Float wrapping with margin-box

## <a id="declaring-shapes"></a>6. Declaring Shapes

Shapes are declared with the [shape-outside](#propdef-shape-outside) property, with possible modifications from the [shape-margin](#propdef-shape-margin) property. The shape defined by the shape-outside and shape-margin properties changes the geometry of a float element’s [float area](#float-area).

### <a id="shape-outside-property"></a>6.1. Float Area Shape: the [shape-outside](#propdef-shape-outside) property

| Name: | *<a id="propdef-shape-outside"></a>shape-outside* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | none [\|](css-values-4.md#comb-one) [ [<basic-shape>](#typedef-basic-shape) [\|\|](css-values-4.md#comb-any) [<shape-box>](#typedef-shape-box) ] \| [<image>](css-images-3.md#typedef-image) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | floats and initial letter boxes |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as [defined](#basic-shape-computed-values) for [<basic-shape>](#typedef-basic-shape) (with [<shape-box>](#typedef-shape-box) following, if supplied); else the computed [<image>](css-images-3.md#typedef-image); else the keyword as specified |
| Canonical order: | per grammar |
| Animation type: | as [defined](#basic-shape-interpolation) for [<basic-shape>](#typedef-basic-shape), otherwise discrete |

The values of this property have the following meanings:

***<a id="valdef-shape-outside-none"></a>none* **
: The [float area](#float-area) is unaffected.
**[<shape-box>](#typedef-shape-box) **
: If one of these values is specified by itself the shape is computed based on one of [margin-box](#valdef-shape-box-margin-box), [border-box](#valdef-shape-box-border-box), [padding-box](#valdef-shape-box-padding-box) or [content-box](#valdef-shape-box-content-box) which use their respective boxes including curvature from border-radius, similar to [background-clip](css-backgrounds-3.md#propdef-background-clip) [[CSS3BG]](#biblio-css3bg).
***<a id="valdef-shape-outside-basic-shape"></a>[<basic-shape>](#typedef-basic-shape)* **
: The shape is computed based on one of the [<basic-shape>](#typedef-basic-shape) functions. If a [<shape-box>](#typedef-shape-box) is also supplied, this defines the [reference box](#basic-shape-reference-box) for the <basic-shape> function. If <shape-box> is not supplied, then the reference box defaults to [margin-box](#valdef-shape-box-margin-box).
***<a id="valdef-shape-outside-image"></a>[<image>](css-images-3.md#typedef-image)* **
: The shape is extracted and computed based on the alpha channel of the specified [<image>](css-images-3.md#typedef-image) as defined by [shape-image-threshold](#propdef-shape-image-threshold).

User agents must use the CORS protocol defined by the [[FETCH]](#biblio-fetch) specification for all URLs in a [shape-outside](#propdef-shape-outside) value. When fetching, user agents must use "Anonymous" mode, set the referrer source to the stylesheet’s URL and set the origin to the URL of the containing document. If this results in network errors such that there is no valid fallback image, the effect is as if the value [none](#valdef-shape-outside-none) had been specified.

### <a id="shape-image-threshold-property"></a>6.2. Choosing Image Pixels: the [shape-image-threshold](#propdef-shape-image-threshold) property

The [shape-image-threshold](#propdef-shape-image-threshold) defines the alpha channel threshold used to extract the shape using an image. A value of 0.5 means that the shape will enclose all the pixels that are more than 50% opaque.

| Name: | *<a id="propdef-shape-image-threshold"></a>shape-image-threshold* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<opacity-value>](css-color-4.md#typedef-opacity-opacity-value) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | floats |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | specified number, clamped to the range [0,1] |
| Canonical order: | per grammar |
| Animation type: | by computed value |

The values of this property have the following meanings:

***<a id="valdef-shape-image-threshold-number"></a>[<number>](css-values-4.md#number-value)* **
: Sets the threshold used for extracting a shape from an image. The shape is defined by the pixels whose alpha value is greater than the threshold. A threshold value outside the range 0.0 (fully transparent) to 1.0 (fully opaque) will be clamped to this range.
Note: A future level of CSS Shapes may define a switch to use the luminance data from an image instead of the alpha data. When this happens, shape-image-threshold will be extended to apply its threshold to either alpha or luminance, depending on the switch state.

### <a id="shape-margin-property"></a>6.3. Expanding a Shape: the [shape-margin](#propdef-shape-margin) property

The [shape-margin](#propdef-shape-margin) property adds a margin to a [shape-outside](#propdef-shape-outside). This defines a new shape that is the smallest contour (in the shrink-wrap sense) that includes all the points that are the shape-margin distance outward in the perpendicular direction from a point on the underlying shape. This includes any edge or line sections from the underlying shape. Note that at points where a perpendicular is not defined (e.g. sharp points or line ends) take all points on the circle centered at the point and with a radius of shape-margin.

The new shape produced by applying [shape-outside](#propdef-shape-outside) is what determines the [float area](#float-area), and must be constructed before making any [wrap](#wrap) decisions.

This property takes only non-negative values.

| Name: | *<a id="propdef-shape-margin"></a>shape-margin* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | floats and initial letter boxes |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to the inline size of the containing block |
| [Computed value:](css-cascade-5.md#computed) | computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value |
| Canonical order: | per grammar |
| Animation type: | by computed value |

***<a id="valdef-shape-margin-length-percentage-0"></a>[<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage)* **
: Sets the margin of the shape to the specified value.
Note: Adding a shape-margin does NOT allow a float area to extend outside a float’s margin box. Extra margin may need to be applied along with shape-margin to avoid clipping.

<a id="example-dde61751"></a>

A [shape-margin](#propdef-shape-margin) creating an offset from a polygonal [shape-outside](#propdef-shape-outside). The lighter blue area shows the shape in a 100x100px float, and the darker blue area shows the 10px offset.

```
  .float {
      width: 100px;
      height: 100px;
    shape-outside: polygon(10px 10px, 90px 50px, 40px 50px, 90px 90px, 10px 90px);
    shape-margin: 10px;
  }
```

[image: Example of a shape-margin offset]

<a id="example-dc83070e"></a>

If shape-margin is added to the CSS logo from example 6, the line boxes [wrapping](#wrap) around the shape are shortened further. In case the image’s alpha channel runs up to the right edge of the image, some extra margin-right should be applied to ensure the shape is not clipped by the margin box.

```
  #CSSlogo {
    shape-margin: 35px;
    margin-right: 35px;
  }
```

[image: A float shape around an image using its alpha-channel with a 35 pixels shape-margin]

A float shape around an image using its alpha-channel with a 35-pixel [shape-margin](#propdef-shape-margin)

## <a id="privacy"></a>7. Privacy Considerations

No privacy concerns have been raised against this specification.

## <a id="security"></a>8. Security Considerations

Since the [<image>](css-images-3.md#typedef-image) value of [shape-outside](#propdef-shape-outside) can expose some image data in a new way, use is limited to images with CORS approval.

## <a id="acknowledgments"></a> Acknowledgments

This specification is made possible by input from Tab Atkins Jr., Amelia Bellamy-Royds, Oriol Brufau, Andrei Bucur, Alexandru Chiculita, Boris Chiou, Emilio Cobos Álvarez, Elika Etemad, Arron Eicholz, Sylvain Galineau, Daniel Glazman, Arno Gourdol, Zoltan Horvath, Chris Jones, Bem Jones-Bey, Ian Kilpatrick, Guillaume Lebas, Ting-Yu Lin, Eric Meyer, Marcus Mielke, Alex Mogilevsky, Hans Muller, Mihnea Ovidenie, Virgil Palanciuc, Noam Rosenthal, Robert Sanderson, Dirk Schulze, Jen Simmons, Peter Sorotokin, Bear Travis, Lea Verou, Eugene Veselov, Brad Werth, Stephen Zilles and the CSS Working Group members.

## <a id="change-log"></a> Change Log

### <a id="20250612"></a> Since 2 June 2025

- none

### <a id="20221115"></a> Since 15 November 2022

- Added Web Platform Test annotations
- Changed fill-rule default for path() in SVG shapes
- Clarified shape-margin and float area interaction
- Added polygon rounding examples
- Included all basic shapes in property prose for issue #9728
- Added start of polygon vertex rounding
- Used opacity-value (the value) not opacity (the property) as value of shape-image-threshold for issue #8311
- Replaced <alpha-value> with <[opacity](css-color-4.md#propdef-opacity)>
- Clarified that shape-outside applies to initial letter boxes for issue #5160
- Rebased ellipse()/circle() definitions on top of <radial-size> from Images, for issue #824
- Made it clear that the position defaulting can be overridden by other specs (like Motion).
- Targeted fix of shape serialization section, fixed examples, for issue #8695
- Fixed xywh() computed value for issue #9053
- Rewrote syntax with implicitly optional comma PR #9650
- Cleanup <*-box> definitions PR #9505
- Added range notation in property definitions
- Added vertex rounding for polygon() for issue #9843
- Updated shape-outside prose to include all [<basic-shape>](#typedef-basic-shape)s for issue #9728
- Clarified the shape-margin contribution to float areas for issue #2949
- Added fill-rule default handling for SVG shapes from issue #3468
- Moved 'shape()' from level 2

### <a id="20140320"></a> Since March 20th 2014

- Clarified shape-margin computed value
- Clarified serialization of default position values, for issue #402
- Clarified empty circles and ellipses for issue #850
- Dropped the "Media:" entry from propdef tables, as with all CSS modules
- Updated box model references from CSS 2 to CSS Box Model 3
- Updated Computed Value and Animation Type in propdef tables
- Clarified computed value of shape-outside property, for issue #4042
- Clarified that shape-image-threshold can take a percentage value, like any alpha value, for issue #4102
- Moved path() back from level 2
- Added handling of negative margins for margin-box, for issue #675
- Removed special-case serialization of position values, for issue #2301
- Added clarifications to shape-margin examples
- Added margin=0 case for margin-box shape, for issue #675
- Changed rules about degenerate shapes to use shape edges, for issue #2375
- Clarified that invalid path strings make the [path()](#funcdef-basic-shape-path) invalid, for issue #392
- Rewrote definition of inset rectangles, added auto value, added examples
- Clarified computed value of basic shape rect functions
- Assorted markup fixes, including use of range notation and exporting defined terms for use in other specifications
- Split Privacy and Security considerations into separate sections

### <a id="20140211"></a> Since February 11th 2014

- Replaced divs with images in the first example
- Add 0px to last serialization example

### <a id="20131203"></a> Since December 3rd 2013

- Updated computed value and serialization of basic shapes
- Added a margin-box example
- Change auto to none for shape-outside
- Defined shape-box instead of redefining box
- Clarified that shape from image may produce more than one path

### <a id="20130620"></a> Since June 20th 2013

- Added shape from box value section
- Updated basic-shape interpolation
- Allow negative insets, disallow negative radii
- Changed relevant to reference
- Remove box-sizing dependency, add relevant box keywords
- Changed circle() and ellipse() to use radial gradient syntax
- Postponed rectangle() to level 2
- Clarified shape-from-image sizing and positioning
- Change inset-rectangle() to inset()
- Future-proof shape-image-threshold to possibly apply to luminance
- Added CORS fetching to shape-outside URLs
- Changed shape-outside value from <uri> to <image>
- Remove 'percentages based on auto-sizing resolve to 0'
- Change initial value of shape-image-threshold to 0.0
- Change float positioning to be unaffected by shape-outside
- Shapes on floats clipped to float’s margin box

### <a id="20120503"></a> Since May 3rd 2012

- Postpone shapes from SVG elements to a future Shapes level
- Postpone shape-inside to a future Shapes level
- split exclusions from shapes into separate modules
- added inset-rectangle() to basic shapes
- Changed shape-inside overflow diagrams to show exclusion behavior
- Changed shape-inside to contribute to the wrapping context
- Defined exclusion edges relative to wrapping content’s writing mode
- Made use of start, end, before and after consistent
- Added interpolation for basic shapes
- Changed basic shapes to depend on box specified with box-sizing
- Added overflow behavior for shape-inside.
- Added wrap-flow:minimum.
- Clarified processing model.
- Changed wrap-margin and wrap-padding to shape-margin and shape-padding.
- Removed wrap shorthand.

### <a id="20111213"></a> Since December 13th 2011

- Clarified processing model.
- Clarified interaction with floats.
- Clarified that an exclusion element establishes a new block formatting context.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-box-4"></a>[CSS-BOX-4] **
: Elika Etemad. [CSS Box Model Module Level 4](css-box-4.md). URL: [https://drafts.csswg.org/css-box-4/](css-box-4.md)
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-images-3"></a>[CSS-IMAGES-3] **
: Tab Atkins Jr.; Elika Etemad; Lea Verou. [CSS Images Module Level 3](css-images-3.md). URL: [https://drafts.csswg.org/css-images-3/](css-images-3.md)
**<a id="biblio-css-images-4"></a>[CSS-IMAGES-4] **
: Elika Etemad; Tab Atkins Jr.; Lea Verou. CSS Images Module Level 4. URL: https://drafts.csswg.org/css-images-4/
**<a id="biblio-css-inline-3"></a>[CSS-INLINE-3] **
: Elika Etemad. CSS Inline Layout Module Level 3. URL: https://drafts.csswg.org/css-inline-3/
**<a id="biblio-css-masking"></a>[CSS-MASKING] **
: Dirk Schulze; Brian Birtles; Tab Atkins Jr.. [CSS Masking Module Level 1](css-masking-1.md). URL: [https://drafts.csswg.org/css-masking-1/](css-masking-1.md)
**<a id="biblio-css-sizing-3"></a>[CSS-SIZING-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Box Sizing Module Level 3. URL: https://drafts.csswg.org/css-sizing-3/
**<a id="biblio-css-syntax-3"></a>[CSS-SYNTAX-3] **
: Tab Atkins Jr.; Simon Sapin. [CSS Syntax Module Level 3](css-syntax-3.md). URL: [https://drafts.csswg.org/css-syntax/](css-syntax-3.md)
**<a id="biblio-css-transforms-1"></a>[CSS-TRANSFORMS-1] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css-values-3"></a>[CSS-VALUES-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-values-5"></a>[CSS-VALUES-5] **
: Tab Atkins Jr.; Elika Etemad; Miriam Suzanne. CSS Values and Units Module Level 5. URL: https://drafts.csswg.org/css-values-5/
**<a id="biblio-css-variables-2"></a>[CSS-VARIABLES-2] **
: CSS Custom Properties for Cascading Variables Module Level 2. Editor's Draft. URL: https://drafts.csswg.org/css-variables-2/
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3bg"></a>[CSS3BG] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-css3box"></a>[CSS3BOX] **
: Elika Etemad. CSS Box Model Module Level 3. URL: https://drafts.csswg.org/css-box-3/
**<a id="biblio-fetch"></a>[FETCH] **
: Anne van Kesteren. Fetch Standard. Living Standard. URL: https://fetch.spec.whatwg.org/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-svg2"></a>[SVG2] **
: Karl Dubost; et al. [Scalable Vector Graphics (SVG) 2](svg2/index.md). URL: [https://w3c.github.io/svgwg/svg2-draft/](svg2/index.md)
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css3-exclusions"></a>[CSS3-EXCLUSIONS] **
: Rossen Atanassov; Vincent Hardy; Alan Stearns. CSS Exclusions Module Level 1. URL: https://drafts.csswg.org/css-exclusions/
**<a id="biblio-svg-paths"></a>[SVG-PATHS] **
: Cyril Concolato; Cameron McCormack; Doug Schepers. SVG Paths. URL: https://svgwg.org/specs/paths/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [shape-image-threshold](#propdef-shape-image-threshold) | <opacity-value> | 0 | floats | no | n/a | by computed value | per grammar | specified number, clamped to the range [0,1] |
| [shape-margin](#propdef-shape-margin) | <length-percentage [0,∞]> | 0 | floats and initial letter boxes | no | refer to the inline size of the containing block | by computed value | per grammar | computed <length-percentage> value |
| [shape-outside](#propdef-shape-outside) | none \| [ <basic-shape> \|\| <shape-box> ] \| <image> | none | floats and initial letter boxes | no | n/a | as defined for <basic-shape>, otherwise discrete | per grammar | as defined for <basic-shape> (with <shape-box> following, if supplied); else the computed <image>; else the keyword as specified |

**✔**MDN

**✔**MDN

**✔**MDN

**⚠**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN
