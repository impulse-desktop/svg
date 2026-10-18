<!-- https://drafts.csswg.org/css-transforms-1/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Transforms Module Level 1

<a id="w3c-state"></a>Editor’s Draft, 30 November 2025

More details about this document

**This version: **
: https://drafts.csswg.org/css-transforms/
**Latest published version: **
: https://www.w3.org/TR/css-transforms-1/
**Previous Versions: **
: https://www.w3.org/TR/2019/CR-css-transforms-1-20190214/
: https://www.w3.org/TR/2018/WD-css-transforms-1-20181130/
: https://www.w3.org/TR/2017/WD-css-transforms-1-20171130/
: https://www.w3.org/TR/2013/WD-css-transforms-1-20131126/
: https://www.w3.org/TR/2012/WD-css3-transforms-20120911/
: https://www.w3.org/TR/2012/WD-css3-transforms-20120403/
**Feedback: **
: CSSWG Issues Repository
**Editors: **
: L. David Baron (Google)
: Simon Fraser (Apple Inc)
: Dean Jackson (Apple Inc)
: Theresa O'Connor (Apple Inc)
: Dirk Schulze (Adobe Inc)
**Former Editors: **
: David Hyatt (Apple Inc)
: Chris Marrin (Apple Inc)
: Aryeh Gregor (Mozilla)
**Suggest an Edit for this Spec: **
: GitHub Editor
Copyright © 2025 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

CSS transforms allows elements styled with CSS to be transformed in two-dimensional space. This specification is the convergence of the CSS 2D Transforms and SVG transforms specifications.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-transforms” in the title, like this: “[css-transforms] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This section is not normative.*

The CSS visual formatting model describes a coordinate system within each element is positioned. Positions and sizes in this coordinate space can be thought of as being expressed in pixels, starting in the origin of point with positive values proceeding to the right and down.

This coordinate space can be modified with the [transform](#propdef-transform) property. Using transform, elements can be translated, rotated and scaled.

### <a id="module-interactions"></a>1.1. Module Interactions

This module defines a set of CSS properties that affect the visual rendering of elements to which those properties are applied; these effects are applied after elements have been sized and positioned according to the visual formatting model from [[CSS2]](#biblio-css2). Some values of these properties result in the creation of a containing block, and/or the creation of a stacking context.

Transforms affect the rendering of backgrounds on elements with a value of [fixed](css-backgrounds-3.md#valdef-background-attachment-fixed) for the [background-attachment](css-backgrounds-3.md#propdef-background-attachment) property, which is specified in [[CSS3BG]](#biblio-css3bg).

Transforms affect the client rectangles returned by the Element Interface Extensions getClientRects() and getBoundingClientRect(), which are specified in [[CSSOM-VIEW]](#biblio-cssom-view).

Transforms affect the computation of the scrollable overflow region as described by [[CSS-OVERFLOW-3]](#biblio-css-overflow-3).

### <a id="css-values"></a>1.2. CSS Values

This specification follows the CSS property definition conventions from [[CSS2]](#biblio-css2) using the value definition syntax from [[CSS-VALUES-3]](#biblio-css-values-3). Value types not defined in this specification are defined in CSS Values & Units [[CSS-VALUES-3]](#biblio-css-values-3). Combination with other CSS modules may expand the definitions of these value types.

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their property value. For readability they have not been repeated explicitly. Terminology {#terminology}

==========================

When used in this specification, terms have the meanings assigned in this section.

***<a id="transformable-element"></a>transformable element* **
:

A transformable element is an element in one of these categories:

-
all elements whose layout is governed by the CSS box model except for non-replaced inline boxes, table-column boxes, and table-column-group boxes [[CSS2]](#biblio-css2),
-
all SVG [paint server elements](svg2/painting.md#TermPaintServerElement), the [clipPath](css-masking-1.md#elementdef-clippath) element and SVG [renderable elements](svg2/render.md#TermRenderableElement) with the exception of any descendant element of [text content elements](svg2/text.md#TermTextContentElement) [[SVG2]](#biblio-svg2).

***<a id="transformed-element"></a>transformed element* **
:

An element with a computed value other than none for the [transform](#propdef-transform) property.

***<a id="user-coordinate-system"></a>user coordinate system* **
***<a id="local-coordinate-system"></a>local coordinate system* **
:

In general, a coordinate system defines locations and distances on the current canvas. The current local coordinate system (also user coordinate system) is the coordinate system that is currently active and which is used to define how coordinates and lengths are located and computed, respectively, on the current canvas. The current user coordinate system has its origin at the top-left of a [reference box](#reference-box) specified by the [transform-box](#propdef-transform-box) property. Percentage values are relative to the dimension of this reference box. One unit equals one CSS pixel.

***<a id="transformation-matrix"></a>transformation matrix* **
:

A matrix that defines the mathematical mapping from one coordinate system into another. It is computed from the values of the [transform](#propdef-transform) and [transform-origin](#propdef-transform-origin) properties as described [below](#transformation-matrix-computation).

***<a id="current-transformation-matrix"></a>current transformation matrix* (CTM) **
:

A matrix that defines the mapping from the [local coordinate system](#local-coordinate-system) into the [viewport coordinate system](svg2/coords.md#TermViewportCoordinateSystem).

***<a id="2d-matrix"></a>2D matrix* **
:

A 3x2 transformation matrix, or a 4x4 matrix where the items m_31, m_32, m_13, m_23, m_43, m_14, m_24, m_34 are equal to 0 and m_33, m_44 are equal to 1.

***<a id="identity-transform-function"></a>identity transform function* **
:

A [transform function](#transform-functions) that is equivalent to a identity 4x4 matrix (see [Mathematical Description of Transform Functions](#mathematical-description)). Examples for identity transform functions are translate(0), translateX(0), translateY(0), scale(1), scaleX(1), scaleY(1), rotate(0), skew(0, 0), skewX(0), skewY(0) and matrix(1, 0, 0, 1, 0, 0).

***<a id="post-multiply"></a>post-multiply* **
***<a id="post-multiplied"></a>post-multiplied* **
:

Term *A* post-multiplied by term *B* is equal to *A* · *B*.

***<a id="pre-multiply"></a>pre-multiply* **
***<a id="pre-multiplied"></a>pre-multiplied* **
:

Term *A* pre-multiplied by term *B* is equal to *B* · *A*.

***<a id="multiply"></a>multiply* **
:

Multiply term *A* by term *B* is equal to *A* · *B*.

## <a id="transform-rendering"></a>2. The Transform Rendering Model

*This section is normative.*

Specifying a value other than none for the [transform](#propdef-transform) property establishes a new [local coordinate system](#local-coordinate-system) at the element that it is applied to. The mapping from where the element would have rendered into that local coordinate system is given by the element’s [transformation matrix](#transformation-matrix).

<a id="transformation-matrix-computation"></a> The [transformation matrix](#transformation-matrix) is computed from the [transform](#propdef-transform) and [transform-origin](#propdef-transform-origin) properties as follows:

1.
Start with the identity matrix.
2.
Translate by the computed X and Y of [transform-origin](#propdef-transform-origin)
3.
Multiply by each of the transform functions in [transform](#propdef-transform) property from left to right
4.
Translate by the negated computed X and Y values of [transform-origin](#propdef-transform-origin)

<a id="example-de7edca6"></a>

An element has a [transform](#propdef-transform) property that is not none.

```
div {
  transform-origin: 0 0;
  transform: translate(-10px, -20px) scale(2) rotate(45deg);
}
```

The [transform-origin](#propdef-transform-origin) property is set to 0 0 and can be omitted. The [transformation matrix](#transformation-matrix) *TM* gets computed by post-multiplying the [<translate()>](#funcdef-transform-translate), [<scale()>](#funcdef-transform-scale) and [<rotate()>](#funcdef-transform-rotate) [<transform-function>](#typedef-transform-function)s.

[image: TM = \begin{bmatrix} 1 & 0 & 0 & -10 \\ 0 & 1 & 0 & -20 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} 2 & 0 & 0 & 0 \\ 0 & 2 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} cos(45) & -sin(45) & 0 & 0 \\ sin(45) & cos(45) & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]

Transforms apply to [transformable elements](#transformable-element).

The coordinate space is a coordinate system with two axes: the X axis increases horizontally to the right; the Y axis increases vertically downwards.

Transformations are cumulative. That is, elements establish their local coordinate system within the coordinate system of their parent.

To map a point *p_local* with the coordinate pair *x_local* and *y_local* from the [local coordinate system](#local-coordinate-system) of an element into the parent’s coordinate system, post-multiply the [transformation matrix](#transformation-matrix) *TM* of the element by *p_local*. The result is the mapped point *p_parent* with the coordinate pair *x_parent* and *y_parent* in the parent’s local coordinate system.

[image: \begin{bmatrix} x_{parent} \\ y_{parent} \\ 0 \\ 1 \end{bmatrix} = TM \cdot \begin{bmatrix} x_{local} \\ y_{local} \\ 0 \\ 1 \end{bmatrix}]

From the perspective of the user, an element effectively accumulates all the [transform](#propdef-transform) properties of its ancestors as well as any local transform applied to it. The accumulation of these transforms defines a [current transformation matrix](#current-transformation-matrix) (CTM) for the element.

<a id="current-transformation-matrix-computation"></a> The [current transformation matrix](#current-transformation-matrix) is computed by post-multiplying all transformation matrices starting from the [viewport coordinate system](svg2/coords.md#TermViewportCoordinateSystem) and ending with the [transformation matrix](#transformation-matrix) of an element.

<a id="example-63675dc3"></a> This example has multiple, nested elements in an SVG document. Some elements get transformed by a [transformation matrix](#transformation-matrix).

```
<svg xmlns="http://www.w3.org/2000/svg">
  <g transform="translate(-10, 20)">
    <g transform="scale(2)">
      <rect width="200" height="200" transform="rotate(45)"/>
    </g>
  </g>
</svg>
```

-
translate(-10, 20) computes to the transformation matrix *T1*
-
scale(2) computes to the transformation matrix *T2*
-
rotate(45) computes to the transformation matrix *T3*

The CTM for the SVG [rect](svg2/shapes.md#elementdef-rect) element is the result of multiplying *T1*, *T2* and *T3* in order.

[image: CTM = \begin{bmatrix} 1 & 0 & 0 & -10 \\ 0 & 1 & 0 & -20 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} 2 & 0 & 0 & 0 \\ 0 & 2 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix} \cdot \begin{bmatrix} cos(45) & -sin(45) & 0 & 0 \\ sin(45) & cos(45) & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]

To map a point *p_local* with the coordinate pair *x_local* and *y_local* from the [local coordinate system](#local-coordinate-system) of the SVG [rect](svg2/shapes.md#elementdef-rect) element into the [viewport coordinate system](svg2/coords.md#TermViewportCoordinateSystem), post-multiply the [current transformation matrix](#current-transformation-matrix) *CTM* of the element by *p_local*. The result is the mapped point *p_viewport* with the coordinate pair *x_viewport* and *y_viewport* in the viewport coordinate system.

[image: \begin{bmatrix} x_{viewport} \\ y_{viewport} \\ 0 \\ 1 \end{bmatrix} = CTM \cdot \begin{bmatrix} x_{local} \\ y_{local} \\ 0 \\ 1 \end{bmatrix}]

Note: Transformations do affect the visual rendering, but have no effect on the CSS layout other than affecting overflow. Transforms are also taken into account when computing client rectangles exposed via the Element Interface Extensions, namely getClientRects() and getBoundingClientRect(), which are specified in [[CSSOM-VIEW]](#biblio-cssom-view).

<a id="example-76e5e4e0"></a>

```
div {
    transform: translate(100px, 100px);
}
```

This transform moves the element by 100 pixels in both the X and Y directions.

[image: The 100px translation in X and Y]

<a id="example-f319ce51"></a>

```
div {
  height: 100px; width: 100px;
  transform-origin: 50px 50px;
  transform: rotate(45deg);
}
```

The [transform-origin](#propdef-transform-origin) property moves the point of origin by 50 pixels in both the X and Y directions. The transform rotates the element clockwise by 45° about the point of origin. After all transform functions were applied, the translation of the origin gets translated back by -50 pixels in both the X and Y directions.

[image: The point of origin gets translated temporary]

<a id="example-d0510fef"></a>

```
div {
  height: 100px; width: 100px;
  transform: translate(80px, 80px) scale(1.5, 1.5) rotate(45deg);
}
```

The visual appearance is as if the div element gets translated by 80px to the bottom left direction, then scaled up by 150% and finally rotated by 45°.

Each [<transform-function>](#typedef-transform-function) can get represented by a corresponding 4x4 matrix. To map a point from the coordinate space of the div box to the coordinate space of the parent element, these transforms get multiplied in the reverse order:

1.
The rotation matrix gets [post-multiplied](#post-multiplied) by the scale matrix.
2.
The result of the previous multiplication is then [post-multiplied](#post-multiplied) by the translation matrix to create the accumulated transformation matrix.
3.
Finally, the point to map gets [pre-multiplied](#pre-multiplied) with the accumulated transformation matrix.

For more details see [The Transform Function Lists](#transform-function-lists).

[image: The transform specified above]

Note: The identical rendering can be obtained by nesting elements with the equivalent transforms:

```
<div style="transform: translate(80px, 80px)">
    <div style="transform: scale(1.5, 1.5)">
        <div style="transform: rotate(45deg)"></div>
    </div>
</div>
```

For elements whose layout is governed by the CSS box model, the transform property does not affect the flow of the content surrounding the transformed element. However, the extent of the overflow area takes into account transformed elements. This behavior is similar to what happens when elements are offset via relative positioning. Therefore, if the value of the overflow property is scroll or auto, scrollbars will appear as needed to see content that is transformed outside the visible area. Specifically, transforms can extend (but do not shrink) the size of the overflow area, which is computed as the union of the bounds of the elements before and after the application of transforms.

For elements whose layout is governed by the CSS box model, any value other than none for the [transform](#propdef-transform) property results in the creation of a stacking context. Implementations must paint the layer it creates, within its parent stacking context, at the same stacking order that would be used if it were a positioned element with z-index: 0. If an element with a transform is positioned, the z-index property applies as described in [[CSS2]](#biblio-css2), except that auto is treated as 0 since a new stacking context is always created.

For elements whose layout is governed by the CSS box model, any value other than none for the [transform](#propdef-transform) property also causes the element to establish a *<a id="containing-block-for-all-descendants"></a>containing block for all descendants*. Its padding box will be used to layout for all of its absolute-position descendants, fixed-position descendants, and descendant fixed background attachments.

<a id="example-544151e4"></a> To demonstrate the effect of [containing block for all descendants](#containing-block-for-all-descendants) on fixed-position descendants, the following code snippets should behave identically:

```
<style>
#container {
  width: 300px;
  height: 200px;
  border: 5px dashed black;
  padding: 5px;
  overflow: scroll;
}

#bloat {
  height: 1000px;
}

#child {
  right: 0;
  bottom: 0;
  width: 10%;
  height: 10%;
  background: green;
}
</style>

<div id="container" style="transform:translateX(5px);">
  <div id="bloat"></div>
  <div id="child" style="position:fixed;"></div>
</div>
```

versus

```
<div id="container" style="position:relative; z-index:0; left:5px;">
  <div id="bloat"></div>
  <div id="child" style="position:absolute;"></div>
</div>
```

When the background of an element is propagated to the canvas (see [CSS Backgrounds 3 § 2.11.1 The Canvas Background and the Root Element](css-backgrounds-3.md#root-background) and [CSS Backgrounds 3 § 2.11.2 The Canvas Background and the HTML <body> Element](css-backgrounds-3.md#body-background)), that background is not affected by any transform specified for that element or for the root element.

For elements that are effected by a transform (i.e. have a transform applied to them, or to any of their ancestor elements) and do not have their background propagated to the canvas, a value of [fixed](css-backgrounds-3.md#valdef-background-attachment-fixed) for the [background-attachment](css-backgrounds-3.md#propdef-background-attachment) property is treated as if it had a value of [scroll](css-backgrounds-3.md#valdef-background-attachment-scroll). The computed value of background-attachment is not affected.

## <a id="transform-property"></a>3. The [transform](#propdef-transform) Property

A transformation is applied to the coordinate system an element renders into through the [transform](#propdef-transform) property. This property contains a list of [transform functions](#transform-functions). The final transformation value for a coordinate system is obtained by converting each function in the list to its corresponding matrix like defined in [Mathematical Description of Transform Functions](#mathematical-description), then multiplying the matrices.

| Name: | *<a id="propdef-transform"></a>transform* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | none [\|](css-values-4.md#comb-one) [<transform-list>](#typedef-transform-list) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | [transformable elements](#transformable-element) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to the size of [reference box](#reference-box) |
| [Computed value:](css-cascade-5.md#computed) | as specified, but with lengths made absolute |
| Canonical order: | per grammar |
| Animation type: | transform list, see [interpolation rules](#interpolation-of-transforms) |

Any computed value other than none for the transform affects containing block and stacking context, as described in [§ 2 The Transform Rendering Model](#transform-rendering).

```
<a id="typedef-transform-list"></a><transform-list> = <transform-function>+
```

### <a id="serialization-of-transform-functions"></a>3.1. Serialization of [<transform-function>](#typedef-transform-function)s

To serialize the [<transform-function>](#typedef-transform-function)s, serialize as per their individual grammars, in the order the grammars are written in, avoiding [<calc()>](css-values-4.md#funcdef-calc) expressions where possible, avoiding <calc()> transformations, omitting components when possible without changing the meaning, joining space-separated tokens with a single space, and following each serialized comma with a single space.

### <a id="serialization-of-the-computed-value"></a>3.2. Resolved value of [transform](#propdef-transform)

The [transform](#propdef-transform) property is a resolved value special case property. [[CSSOM]](#biblio-cssom)

When the [computed value](css-cascade-5.md#computed-value) is a [<transform-list>](#typedef-transform-list), the resolved value is one [<matrix()>](#funcdef-transform-matrix) function computed by the following algorithm:

1.
Let *transform* be a 4x4 matrix initialized to the identity matrix. The elements *m11*, *m22*, *m33* and *m44* of *transform* must be set to 1; all other elements of *transform* must be set to 0.
2.
Post-multiply all [<transform-function>](#typedef-transform-function)s in [<transform-list>](#typedef-transform-list) to *transform*.
3.
Serialize *transform* to a [<matrix()>](#funcdef-transform-matrix) function.

For other computed values, the resolved value is the [computed value](css-cascade-5.md#computed-value).

## <a id="transform-origin-property"></a>4. The [transform-origin](#propdef-transform-origin) Property

| Name: | *<a id="propdef-transform-origin"></a>transform-origin* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [ left [\|](css-values-4.md#comb-one) center \| right \| top \| bottom \| [<length-percentage>](css-values-4.md#typedef-length-percentage) ] \| [ left \| center \| right \| <length-percentage> ] [ top \| center \| bottom \| <length-percentage> ] [<length>](css-values-4.md#length-value)[?](css-values-4.md#mult-opt) \| [ [ center \| left \| right ] [&&](css-values-4.md#comb-all) [ center \| top \| bottom ] ] <length>? |
| [Initial:](css-cascade-5.md#initial-values) | 50% 50% |
| [Applies to:](css-cascade-5.md#applies-to) | [transformable elements](#transformable-element) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to the size of [reference box](#reference-box) |
| [Computed value:](css-cascade-5.md#computed) | see [background-position](css-backgrounds-3.md#propdef-background-position) |
| Canonical order: | per grammar |
| Animation type: | by computed value |

The values of the [transform](#propdef-transform) and [transform-origin](#propdef-transform-origin) properties are used to compute the [transformation matrix](#transformation-matrix), as described above.

If only one value is specified, the second value is assumed to be [center](#valdef-transform-origin-center). If one or two values are specified, the third value is assumed to be 0px.

If two or more values are defined and either no value is a keyword, or the only used keyword is [center](#valdef-transform-origin-center), then the first value represents the horizontal position (or offset) and the second represents the vertical position (or offset). A third value always represents the Z position (or offset) and must be of type [<length>](css-values-4.md#length-value).

**[<length-percentage>](css-values-4.md#typedef-length-percentage) **
:

A percentage for the horizontal offset is relative to the width of the [reference box](#reference-box). A percentage for the vertical offset is relative to the height of the reference box. The value for the horizontal and vertical offset represent an offset from the top left corner of the reference box.

**[<length>](css-values-4.md#length-value) **
:

A length value gives a fixed length as the offset. The value for the horizontal and vertical offset represent an offset from the top left corner of the [reference box](#reference-box).

***<a id="valdef-transform-origin-top"></a>top* **
:

Computes to 0% for the vertical position.

***<a id="valdef-transform-origin-right"></a>right* **
:

Computes to 100% for the horizontal position.

***<a id="valdef-transform-origin-bottom"></a>bottom* **
:

Computes to 100% for the vertical position.

***<a id="valdef-transform-origin-left"></a>left* **
:

Computes to 0% for the horizontal position.

***<a id="valdef-transform-origin-center"></a>center* **
:

Computes to 50% (left 50%) for the horizontal position if the horizontal position is not otherwise specified, or 50% (top 50%) for the vertical position if it is.

For SVG elements without associated CSS layout box the initial [used value](css-cascade-5.md#used-value) is 0 0 as if the user agent style sheet contained:

```
*:not(svg), *:not(foreignObject) > svg {
    transform-origin: 0 0;
}
```

The [transform-origin](#propdef-transform-origin) property is a resolved value special case property like height. [[CSSOM]](#biblio-cssom)

## <a id="transform-box"></a>5. Transform reference box: the [transform-box](#propdef-transform-box) property

| Name: | *<a id="propdef-transform-box"></a>transform-box* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | content-box [\|](css-values-4.md#comb-one) border-box \| fill-box \| stroke-box \| view-box |
| [Initial:](css-cascade-5.md#initial-values) | view-box |
| [Applies to:](css-cascade-5.md#applies-to) | [transformable elements](#transformable-element) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | specified keyword |
| Canonical order: | per grammar |
| Animation type: | discrete |

All transformations defined by the [transform](#propdef-transform) and [transform-origin](#propdef-transform-origin) property are relative to the position and dimensions of the *<a id="reference-box"></a>reference box* of the element. The [reference box](#reference-box) is specified by one of the following:

***<a id="valdef-transform-box-content-box"></a>content-box* **
:

Uses the content box as reference box. The reference box of a table is the border box of its table wrapper box, not its table box.

***<a id="valdef-transform-box-border-box"></a>border-box* **
:

Uses the border box as reference box. The reference box of a table is the border box of its table wrapper box, not its table box.

***<a id="valdef-transform-box-fill-box"></a>fill-box* **
:

Uses the [object bounding box](svg2/coords.md#TermObjectBoundingBox) as reference box.

***<a id="valdef-transform-box-stroke-box"></a>stroke-box* **
:

Uses the [stroke bounding box](svg2/coords.md#TermStrokeBoundingBox) as reference box.

***<a id="valdef-transform-box-view-box"></a>view-box* **
:

Uses the nearest [SVG viewport](svg11/intro.md#TermSVGViewport) as reference box.

If a `[viewBox](svg2/types.md#__svg__SVGFitToViewBox__viewBox)` attribute is specified for the [SVG viewport](svg11/intro.md#TermSVGViewport) creating element:

-
The reference box is positioned at the origin of the coordinate system established by the `[viewBox](svg2/types.md#__svg__SVGFitToViewBox__viewBox)` attribute.
-
The dimension of the reference box is set to the *width* and *height* values of the `[viewBox](svg2/types.md#__svg__SVGFitToViewBox__viewBox)` attribute.

For the SVG `[pattern](svg2/pservers.md#elementdef-pattern)` element, the reference box gets defined by the `[patternUnits](svg2/pservers.md#PatternElementPatternUnitsAttribute)` attribute [[SVG2]](#biblio-svg2).

For the SVG [linearGradient](svg2/pservers.md#elementdef-linearGradient) and [radialGradient](svg2/pservers.md#elementdef-radialGradient) elements, the reference box gets defined by the `[gradientUnits](svg2/pservers.md#LinearGradientElementGradientUnitsAttribute)` attribute [[SVG2]](#biblio-svg2).

For the SVG [clipPath](css-masking-1.md#elementdef-clippath) element, the reference box gets defined by the `[clipPathUnits](css-masking-1.md#element-attrdef-clippath-clippathunits)` attribute [[CSS-MASKING]](#biblio-css-masking).

A reference box adds an additional offset to the origin specified by the [transform-origin](#propdef-transform-origin) property.

For SVG elements without associated CSS layout box, the [used value](css-cascade-5.md#used-value) for [content-box](#valdef-transform-box-content-box) is [fill-box](#valdef-transform-box-fill-box) and for [border-box](#valdef-transform-box-border-box) is [stroke-box](#valdef-transform-box-stroke-box).

For elements with associated CSS layout box, the [used value](css-cascade-5.md#used-value) for [fill-box](#valdef-transform-box-fill-box) is [content-box](#valdef-transform-box-content-box) and for [stroke-box](#valdef-transform-box-stroke-box) and [view-box](#valdef-transform-box-view-box) is [border-box](#valdef-transform-box-border-box).

## <a id="svg-transform"></a>6. The SVG [transform](svg11/coords.md#TransformAttribute) Attribute

### <a id="transform-attribute-specificity"></a>6.1. SVG presentation attributes

The [transform-origin](#propdef-transform-origin) CSS property is also a [presentation attribute](svg2/styling.md#TermPresentationAttribute) and extends the list of existing presentation attributes [[SVG2]](#biblio-svg2).

SVG 2 defines the [transform](svg11/coords.md#TransformAttribute), `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)`, `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` attributes as [presentation attributes](svg2/styling.md#TermPresentationAttribute), represented by the CSS [transform](#propdef-transform) property [[SVG2]](#biblio-svg2).

The participation in the CSS cascade is determined by the specificity of [presentation attributes](svg2/styling.md#TermPresentationAttribute) in the SVG specification. According to SVG, user agents conceptually insert a [new author style sheet](svg2/styling.md#PresentationAttributes) for presentation attributes, which is the first in the author style sheet collection [[SVG2]](#biblio-svg2).

<a id="example-ae528470"></a> This example shows the combination of the [transform](#propdef-transform) style property and the [transform](svg11/coords.md#TransformAttribute) attribute.

```
<svg xmlns="http://www.w3.org/2000/svg">
  <style>
  .container {
    transform: translate(100px, 100px);
  }
  </style>

  <g class="container" transform="translate(200 200)">
    <rect width="100" height="100" fill="blue" />
  </g>
</svg>
```

[image: Translated SVG container element.]

Because of the participation to the CSS cascade, the [transform](#propdef-transform) style property overrides the [transform](svg11/coords.md#TransformAttribute) attribute. Therefore the container gets translated by 100px in both the horizontal and the vertical directions, instead of 200px.

### <a id="svg-syntax"></a>6.2. Syntax of the SVG [transform](svg11/coords.md#TransformAttribute) attribute

For backwards compatibility reasons, the syntax of the [transform](svg11/coords.md#TransformAttribute), `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)`, `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` attributes differ from the syntax of the [transform](#propdef-transform) CSS property. For the attributes, there is no support for additional [<transform-function>](#typedef-transform-function)s defined for the CSS transform property. Specifically, [<translateX()>](#funcdef-transform-translatex), [<translateY()>](#funcdef-transform-translatey), [<scaleX()>](#funcdef-transform-scalex), [<scaleY()>](#funcdef-transform-scaley) and [<skew()>](#funcdef-transform-skew) are not supported by the transform, `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)`, `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` attributes.

The following list uses the Backus-Naur Form (BNF) to define values for the [transform](svg11/coords.md#TransformAttribute), [patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute) and [gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute) attributes followed by an informative rail road diagram. The following notation is used:

-
*: 0 or more
-
+: 1 or more
-
?: 0 or 1
-
(): grouping
-
|: separates alternatives
-
double quotes surround literals. Literals consists of [letter](css-syntax-3.md#letter)s [[CSS-SYNTAX-3]](#biblio-css-syntax-3), left parenthesis and right parenthesis.
-
[<number-token>](css-syntax-3.md#typedef-number-token) defined by the CSS Syntax module [[CSS-SYNTAX-3]](#biblio-css-syntax-3).

Note: The syntax reflects implemented behavior in user agents and differs from the syntax defined by SVG 1.1.

**left parenthesis ( **
: U+0028 LEFT PARENTHESIS
**right parenthesis ) **
: U+0029 RIGHT PARENTHESIS
**<a id="svg-comma"></a>comma **
: U+002C COMMA.
**<a id="svg-wsp"></a>wsp **
: Either a U+000A LINE FEED, U+000D CARRIAGE RETURN, U+0009 CHARACTER TABULATION, or U+0020 SPACE.
:

**<a id="svg-comma-wsp"></a>comma-wsp **
:

```
(wsp+ comma? wsp*) | (comma wsp*)
```

:

**<a id="svg-translate"></a>translate **
:

```
"translate" wsp* "(" wsp* number ( comma-wsp? number )? wsp* ")"
```

:

**<a id="svg-scale"></a>scale **
:

```
"scale" wsp* "(" wsp* number ( comma-wsp? number )? wsp* ")"
```

:

**<a id="svg-rotate"></a>rotate **
:

```
"rotate" wsp* "(" wsp* number ( comma-wsp? number comma-wsp? number )? wsp* ")"
```

:

**<a id="svg-skewX"></a>skewX **
:

```
"skewX" wsp* "(" wsp* number wsp* ")"
```

:

**<a id="svg-skewY"></a>skewY **
:

```
"skewY" wsp* "(" wsp* number wsp* ")"
```

:

**<a id="svg-matrix"></a>matrix **
:

```
"matrix" wsp* "(" wsp*
    number comma-wsp?
    number comma-wsp?
    number comma-wsp?
    number comma-wsp?
    number comma-wsp?
    number wsp* ")"
```

:

**<a id="svg-transform-function"></a>transform **
:

```
matrix
| translate
| scale
| rotate
| skewX
| skewY
```

:

**<a id="svg-transforms"></a>transforms **
:

```
transform
| transform comma-wsp? transforms
```

:

**<a id="svg-transform-list"></a>transform-list **
:

```
wsp* transforms? wsp*
```

:

### <a id="svg-transform-functions"></a>6.3. SVG transform functions

SVG transform functions of the [transform](svg11/coords.md#TransformAttribute), `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)`, `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` attributes defined by the syntax above are mapped to CSS [<transform-function>](#typedef-transform-function)s as follows:

<a id="term-matching"></a>

| SVG transform function | CSS [<transform-function>](#typedef-transform-function) |  |
| --- | --- | --- |
| [translate](#svg-translate) | [<translate()>](#funcdef-transform-translate) | Number values interpreted as CSS [<length>](css-values-4.md#length-value) types with [px](css-values-4.md#px) units. |
| [scale](#svg-scale) | [<scale()>](#funcdef-transform-scale) |  |
| [rotate](#svg-rotate) | [<rotate()>](#funcdef-transform-rotate) | Only single value version. Number value interpreted as CSS [<angle>](css-values-4.md#angle-value) type with [deg](css-values-4.md#deg) unit. |
| [skewX](#svg-skewX) | [<skewX()>](#funcdef-transform-skewx) | Number value interpreted as CSS [<angle>](css-values-4.md#angle-value) type with [deg](css-values-4.md#deg) unit. |
| [skewY](#svg-skewY) | [<skewY()>](#funcdef-transform-skewy) | Number value interpreted as CSS [<angle>](css-values-4.md#angle-value) type with [deg](css-values-4.md#deg) unit. |
| [matrix](#svg-matrix) | [<matrix()>](#funcdef-transform-matrix) |  |

The SVG transform function [rotate](#svg-rotate) with 3 values can not be mapped to a corresponding CSS [<transform-function>](#typedef-transform-function). The 2 optional number values represent a horizontal translation value cx followed by a vertical translation value cy. Both number values get interpreted as CSS [<length>](css-values-4.md#length-value) types with [px](css-values-4.md#px) units and define the origin for rotation. The behavior is equivalent to an initial translation by cx, cy, a rotation defined by the first number value interpreted as [<angle>](css-values-4.md#angle-value) type with [deg](css-values-4.md#deg) unit followed by a translation by -cx, -cy.

A [transform](svg11/coords.md#TransformAttribute) attribute can be the start or end value of a CSS Transition. If the value of a transform attribute is the start or end value of a CSS Transition and the SVG [transform list](#svg-transform-list) contains at least one [rotate](#svg-rotate) transform function with 3 values, the individual SVG transform functions must get [post-multiplied](#post-multiplied) and the resulting matrix must get mapped to a [<matrix()>](#funcdef-transform-matrix) CSS [<transform-function>](#typedef-transform-function) and used as start/end value of the CSS Transition.

### <a id="svg-user-coordinate-space"></a>6.4. User coordinate space

For the `[pattern](svg2/pservers.md#elementdef-pattern)` element, the `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)` attribute and [transform](#propdef-transform) property define an additional transformation in the pattern coordinate system. See `[patternUnits](svg2/pservers.md#PatternElementPatternUnitsAttribute)` attribute for details [[SVG2]](#biblio-svg2).

For the `[linearGradient](svg2/pservers.md#elementdef-linearGradient)` and `[radialGradient](svg2/pservers.md#elementdef-radialGradient)` elements, the `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` attribute and [transform](#propdef-transform) property define an additional transformation in the gradient coordinate system. See `[gradientUnits](svg2/pservers.md#LinearGradientElementGradientUnitsAttribute)` attribute for details [[SVG2]](#biblio-svg2).

For the `[clipPath](css-masking-1.md#elementdef-clippath)` element, the [transform](svg11/coords.md#TransformAttribute) attribute and [transform](#propdef-transform) property define an additional transformation in the clipping path coordinate space. See `[clipPathUnits](css-masking-1.md#element-attrdef-clippath-clippathunits)` attribute for details [[CSS-MASKING]](#biblio-css-masking).

For all other [transformable elements](#transformable-element) the [transform](svg11/coords.md#TransformAttribute) attribute and [transform](#propdef-transform) property define a transformation in the current user coordinate system of the parent. All percentage values of the transform attribute are relative to the element’s [reference box](#reference-box).

<a id="example-bb84e563"></a>

The [transform-origin](#propdef-transform-origin) property on the pattern in the following example specifies a 50% translation of the origin in the horizontal and vertical dimension. The [transform](#propdef-transform) property specifies a translation as well, but in absolute lengths.

```
<svg xmlns="http://www.w3.org/2000/svg">
  <style>
  pattern {
    transform: rotate(45deg);
    transform-origin: 50% 50%;
  }
  </style>

  <defs>
  <pattern id="pattern-1">
    <rect id="rect1" width="100" height="100" fill="blue" />
  </pattern>
  </defs>

  <rect width="200" height="200" fill="url(#pattern-1)" />
</svg>
```

An SVG `[pattern](svg2/pservers.md#elementdef-pattern)` element doesn’t have a bounding box. The [reference box](#reference-box) of the referencing `[rect](svg2/shapes.md#elementdef-rect)` element is used instead to solve the relative values of the [transform-origin](#propdef-transform-origin) property. Therefore the point of origin will get translated by 100 pixels temporarily to rotate the user space of the `[pattern](svg2/pservers.md#elementdef-pattern)` elements content.

### <a id="transform-attribute-dom"></a>6.5. SVG DOM interface for the [transform](svg11/coords.md#TransformAttribute) attribute

The SVG specification defines the "[SVGAnimatedTransformList](svg11/coords.md#InterfaceSVGAnimatedTransformList)" interface in the SVG DOM to provide access to the animated and the base value of the SVG [transform](svg11/coords.md#TransformAttribute), `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` and `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)` attributes. To ensure backwards compatibility, this API must still be supported by user agents.

`[baseVal](svg2/coords.md#__svg__SVGAnimatedPreserveAspectRatio__baseVal)` gives the author the possibility to access and modify the values of the SVG [transform](svg11/coords.md#TransformAttribute), `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)`, `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` attributes. To provide the necessary backwards compatibility to the SVG DOM, `[baseVal](svg2/coords.md#__svg__SVGAnimatedPreserveAspectRatio__baseVal)` must reflect the values of this author style sheet. All modifications to SVG DOM objects of `[baseVal](svg2/coords.md#__svg__SVGAnimatedPreserveAspectRatio__baseVal)` must affect this author style sheet immediately.

`[animVal](svg2/coords.md#__svg__SVGAnimatedPreserveAspectRatio__animVal)` represents the computed style of the [transform](#propdef-transform) property. Therefore it includes all applied CSS3 Transitions, CSS3 Animations or SVG Animations if any of those are underway. The computed style and SVG DOM objects of `[animVal](svg2/coords.md#__svg__SVGAnimatedPreserveAspectRatio__animVal)` can not be modified.

## <a id="transform-functions"></a>7. The Transform Functions

The value of the [transform](#propdef-transform) property is a list of *<a id="typedef-transform-function"></a><transform-function>*. The set of allowed transform functions is given below. In the following functions, a [<zero>](css-values-4.md#zero-value) behaves the same as 0deg ("unitless 0" angles are preserved for legacy compat). A percentage for horizontal translations is relative to the width of the [reference box](#reference-box). A percentage for vertical translations is relative to the height of the reference box.

### <a id="two-d-transform-functions"></a>7.1. 2D Transform Functions

***<a id="funcdef-transform-matrix"></a>matrix()* = matrix( [<number>](css-values-4.md#number-value)[#{6}](css-values-4.md#mult-comma) ) **
:

specifies a 2D transformation in the form of a [transformation matrix](#MatrixDefined) of the six values a, b, c, d, e, f.

***<a id="funcdef-transform-translate"></a>translate()* = translate( [<length-percentage>](css-values-4.md#typedef-length-percentage) [,](css-values-4.md#comb-comma) <length-percentage>[?](css-values-4.md#mult-opt) ) **
:

specifies a [2D translation](#TranslateDefined) by the vector [tx, ty], where tx is the first translation-value parameter and ty is the optional second translation-value parameter. If *<ty>* is not provided, ty has zero as a value.

***<a id="funcdef-transform-translatex"></a>translateX()* = translateX( [<length-percentage>](css-values-4.md#typedef-length-percentage) ) **
:

specifies a [translation](#TranslateDefined) by the given amount in the X direction.

***<a id="funcdef-transform-translatey"></a>translateY()* = translateY( [<length-percentage>](css-values-4.md#typedef-length-percentage) ) **
:

specifies a [translation](#TranslateDefined) by the given amount in the Y direction.

***<a id="funcdef-transform-scale"></a>scale()* = scale( [<number>](css-values-4.md#number-value) [,](css-values-4.md#comb-comma) <number>[?](css-values-4.md#mult-opt) ) **
:

specifies a [2D scale](#ScaleDefined) operation by the [sx,sy] scaling vector described by the 2 parameters. If the second parameter is not provided, it takes a value equal to the first. For example, scale(1, 1) would leave an element unchanged, while scale(2, 2) would cause it to appear twice as long in both the X and Y axes, or four times its typical geometric size.

***<a id="funcdef-transform-scalex"></a>scaleX()* = scaleX( [<number>](css-values-4.md#number-value) ) **
:

specifies a [2D scale](#ScaleDefined) operation using the [sx,1] scaling vector, where sx is given as the parameter.

***<a id="funcdef-transform-scaley"></a>scaleY()* = scaleY( [<number>](css-values-4.md#number-value) ) **
:

specifies a [2D scale](#ScaleDefined) operation using the [1,sy] scaling vector, where sy is given as the parameter.

***<a id="funcdef-transform-rotate"></a>rotate()* = rotate( [ [<angle>](css-values-4.md#angle-value) [|](css-values-4.md#comb-one) [<zero>](css-values-4.md#zero-value) ] ) **
:

specifies a [2D rotation](#RotateDefined) by the angle specified in the parameter about the origin of the element, as defined by the [transform-origin](#propdef-transform-origin) property. For example, rotate(90deg) would cause elements to appear rotated one-quarter of a turn in the clockwise direction.

***<a id="funcdef-transform-skew"></a>skew()* = skew( [ [<angle>](css-values-4.md#angle-value) [|](css-values-4.md#comb-one) [<zero>](css-values-4.md#zero-value) ] [,](css-values-4.md#comb-comma) [ <angle> | <zero> ][?](css-values-4.md#mult-opt) ) **
:

specifies a [2D skew](#SkewDefined) by [ax,ay] for X and Y. If the second parameter is not provided, it has a zero value.

** [skew()](#funcdef-transform-skew) exists for compatibility reasons, and should not be used in new content. Use [skewX()](#funcdef-transform-skewx) or [skewY()](#funcdef-transform-skewy) instead, noting that the behavior of skew() is different from multiplying skewX() with skewY().**

***<a id="funcdef-transform-skewx"></a>skewX()* = skewX( [ [<angle>](css-values-4.md#angle-value) [|](css-values-4.md#comb-one) [<zero>](css-values-4.md#zero-value) ] ) **
:

specifies a [2D skew transformation along the X axis](#SkewXDefined) by the given angle.

***<a id="funcdef-transform-skewy"></a>skewY()* = skewY( [ [<angle>](css-values-4.md#angle-value) [|](css-values-4.md#comb-one) [<zero>](css-values-4.md#zero-value) ] ) **
:

specifies a [2D skew transformation along the Y axis](#SkewYDefined) by the given angle.

### <a id="transform-primitives"></a>7.2. Transform function primitives and derivatives

Some transform functions can be represented by more generic transform functions. These transform functions are called derived transform functions, and the generic transform functions are called primitive transform functions. Two-dimensional primitives and their derived transform functions are:

**<a id="translate-primitive"></a>[translate()](#funcdef-transform-translate) **
: for [<translateX()>](#funcdef-transform-translatex), [<translateY()>](#funcdef-transform-translatey) and [<translate()>](#funcdef-transform-translate).
**<a id="scale-primitive"></a>[scale()](#funcdef-transform-scale) **
: for [<scaleX()>](#funcdef-transform-scalex), [<scaleY()>](#funcdef-transform-scaley) and [<scale()>](#funcdef-transform-scale).
## <a id="transform-function-lists"></a>8. The Transform Function Lists

If a list of [<transform-function>](#typedef-transform-function)s is provided, then the net effect is as if each transform function had been specified separately in the order provided.

That is, in the absence of other styling that affects position and dimensions, a nested set of transforms is equivalent to a single list of transform functions, applied from the coordinate system of the ancestor to the [local coordinate system](#local-coordinate-system) of a given element. The resulting transform is the matrix multiplication of the list of transforms.

<a id="example-5633487a"></a> For example,

```
<div style="transform: translate(-10px, -20px) scale(2) rotate(45deg)"/>
```

is functionally equivalent to:

```
<div style="transform: translate(-10px, -20px)" id="root">
  <div style="transform: scale(2)">
    <div style="transform: rotate(45deg)">
    </div>
  </div>
</div>
```

If a transform function causes the [current transformation matrix](#current-transformation-matrix) of an object to be non-invertible, the object and its content do not get displayed.

<a id="example-116f6ceb"></a>

The object in the following example gets scaled by 0.

```
<style>
.box {
  transform: scale(0);
}
</style>

<div class="box">
  Not visible
</div>
```

The scaling causes a non-invertible CTM for the coordinate space of the div box. Therefore neither the div box, nor the text in it get displayed.

## <a id="interpolation-of-transforms"></a>9. Interpolation of Transforms

[Interpolation](css-values-4.md#interpolation) of transform function lists is performed as follows:

- <a id="none-none-interpolation"></a> If both *V_a* and *V_b* are none:
  -
  *V_result* is none.
- <a id="transform-interpolation-length-fixup"></a> Treating none as a list of zero length, if *V_a* or *V_b* differ in length:
  -
  extend the shorter list to the length of the longer list, setting the function at each additional position to the [identity transform function](#identity-transform-function) matching the function at the corresponding position in the longer list. Both transform function lists are then interpolated following the next rule.
<a id="example-fa107865"></a> For example, if *V_a* is scale(2) and *V_b* is none then the value scale(1) will be used for *V_b* and interpolation will proceed using the next rule. Similarly, if *V_a* is scale(1) and *V_b* is scale(2) rotate(50deg) then the interpolation will be performed as if *V_a* were scale(1) rotate(0).
- Let *V_result* be an empty list. Beginning at the start of *V_a* and *V_b*, compare the corresponding functions at each position:
  -
  While the functions have either the same name, or are derivatives of the same [primitive transform function](#transform-primitives), interpolate the corresponding pair of functions as described in [§ 10 Interpolation of primitives and derived transform functions](#interpolation-of-transform-functions) and append the result to *V_result*.
  -
  If the pair do not have a common name or [primitive transform function](#transform-primitives), post-multiply the remaining transform functions in each of *V_a* and *V_b* respectively to produce two 4x4 matrices. [Interpolate](css-values-4.md#interpolation) these two matrices as described in [§ 11 Interpolation of Matrices](#matrix-interpolation), append the result to *V_result*, and cease iterating over *V_a* and *V_b*.
<a id="example-922432e3"></a> For example, if *V_a* is rotate(0deg) scale(1) translate(20px) and *V_b* is rotate(270deg) translate(10px) scale(2), the rotate(0deg) and rotate(360deg) functions will be interpolated according to [§ 10 Interpolation of primitives and derived transform functions](#interpolation-of-transform-functions) while the remainder of each list—​scale(1) translate(20px) and translate(10px) scale(2)—​will first be converted to equivalent 4x4 matrices and then interpolated as described in [§ 11 Interpolation of Matrices](#matrix-interpolation).
A previous version of this specification did not attempt to interpolate matching pairs of transform functions unless all functions in the list matched. As a result, the two lists in this example would be interpolated using matrix interpolation only and the rotate(360deg) component of the second list would be lost.

In some cases, an animation might cause a transformation matrix to be singular or non-invertible. For example, an animation in which scale moves from 1 to -1. At the time when the matrix is in such a state, the transformed element is not rendered.

## <a id="interpolation-of-transform-functions"></a>10. Interpolation of primitives and derived transform functions

Two transform functions with the same name and the same number of arguments are interpolated numerically without a former conversion. The calculated value will be of the same transform function type with the same number of arguments. Special rules apply to [<matrix()>](#funcdef-transform-matrix).

<a id="example-1356eb1f"></a>

The two transform functions translate(0) and translate(100px) are of the same type, have the same number of arguments and therefore can get interpolated numerically. translateX(100px) is not of the same type and translate(100px, 0) does not have the same number of arguments, therefore these transform functions can not get interpolated without a former conversion step.

Two different types of transform functions that share the same primitive, or transform functions of the same type with different number of arguments can be interpolated. Both transform functions need a former conversion to the common primitive first and get interpolated numerically afterwards. The computed value will be the primitive with the resulting interpolated arguments.

<a id="example-bd3fe5db"></a>

The following example describes a transition from translateX(100px) to translateY(100px) in 3 seconds on hovering over the div box. Both transform functions derive from the same primitive [translate()](#funcdef-transform-translate) and therefore can be interpolated.

```
div {
  transform: translateX(100px);
}

div:hover {
  transform: translateY(100px);
  transition: transform 3s;
}
```

For the time of the transition both transform functions get transformed to the common primitive. translateX(100px) gets converted to translate(100px, 0) and translateY(100px) gets converted to translate(0, 100px). Both transform functions can then get interpolated numerically.

If both transform functions share a primitive in the two-dimensional space, both transform functions get converted to the two-dimensional primitive. If one or both transform functions are three-dimensional transform functions, the common three-dimensional primitive is used.

<a id="example-24abc2ae"></a>

In this example a two-dimensional transform function gets animated to a three-dimensional transform function. The common primitive is [translate3d()](css-transforms-2.md#funcdef-translate3d).

```
div {
  transform: translateX(100px);
}

div:hover {
  transform: translateZ(100px);
  transition: transform 3s;
}
```

First translateX(100px) gets converted to translate3d(100px, 0, 0) and translateZ(100px) to translate3d(0, 0, 100px) respectively. Then both converted transform functions get interpolated numerically.

## <a id="matrix-interpolation"></a>11. Interpolation of Matrices

When interpolating between two matrices, each matrix is decomposed into the corresponding translation, rotation, scale, skew. Each corresponding component of the decomposed matrices gets interpolated numerically and recomposed back to a matrix in a final step.

<a id="example-3a6b960d"></a> In the following example the element gets translated by 100 pixel in both the X and Y directions and rotated by 1170° on hovering. The initial transformation is 45°. With the usage of transition, an author might expect a animated, clockwise rotation by three and a quarter turns (1170°).

```
<style>
div {
  transform: rotate(45deg);
}
div:hover {
  transform: translate(100px, 100px) rotate(1215deg);
  transition: transform 3s;
}
</style>

<div></div>
```

The number of transform functions on the source transform rotate(45deg) differs from the number of transform functions on the destination transform translate(100px, 100px) rotate(1125deg). According to the last rule of [Interpolation of Transforms](#interpolation-of-transforms), both transforms must be interpolated by matrix interpolation. With converting the transformation functions to matrices, the information about the three turns gets lost and the element gets rotated by just a quarter turn (90°).

To achieve the three and a quarter turns for the example above, source and destination transforms must fulfill the third rule of [Interpolation of Transforms](#interpolation-of-transforms). Source transform could look like translate(0, 0) rotate(45deg) for a linear interpolation of the transform functions.

In the following we differ between the [interpolation of two 2D matrices](#interpolation-of-2d-matrices) and the interpolation of two matrices where at least one matrix is not a [2D matrix](#2d-matrix).

If one of the matrices for interpolation is non-invertible, the used animation function must fall-back to a discrete animation according to the rules of the respective animation specification.

### <a id="supporting-functions"></a>11.1. Supporting functions

The pseudo code in the next subsections make use of the following supporting functions:

```
Supporting functions (point is a 3 component vector, matrix is a 4x4 matrix, vector is a 4 component vector):
  double  determinant(matrix)          returns the 4x4 determinant of the matrix
  matrix  inverse(matrix)              returns the inverse of the passed matrix
  matrix  transpose(matrix)            returns the transpose of the passed matrix
  point   multVecMatrix(point, matrix) multiplies the passed point by the passed matrix
                                       and returns the transformed point
  double  length(point)                returns the length of the passed vector
  point   normalize(point)             normalizes the length of the passed point to 1
  double  dot(point, point)            returns the dot product of the passed points
  double  sqrt(double)                 returns the root square of passed value
  double  max(double y, double x)      returns the bigger value of the two passed values
  double  dot(vector, vector)         returns the dot product of the passed vectors
  vector  multVector(vector, vector)  multiplies the passed vectors
  double  sqrt(double)                returns the root square of passed value
  double  max(double y, double x)     returns the bigger value of the two passed values
  double  min(double y, double x)     returns the smaller value of the two passed values
  double  cos(double)                 returns the cosines of passed value
  double  sin(double)                 returns the sine of passed value
  double  acos(double)                returns the inverse cosine of passed value
  double  abs(double)                  returns the absolute value of the passed value
  double  rad2deg(double)              transforms a value in radian to degree and returns it
  double  deg2rad(double)              transforms a value in degree to radian and returns it

Decomposition also makes use of the following function:
  point combine(point a, point b, double ascl, double bscl)
      result[0] = (ascl * a[0]) + (bscl * b[0])
      result[1] = (ascl * a[1]) + (bscl * b[1])
      result[2] = (ascl * a[2]) + (bscl * b[2])
      return result
```

### <a id="interpolation-of-2d-matrices"></a>11.2. Interpolation of 2D matrices

#### <a id="decomposing-a-2d-matrix"></a>11.2.1. Decomposing a 2D matrix

The pseudo code below is based upon the "unmatrix" method in "Graphics Gems II, edited by Jim Arvo".

Matrices in the pseudo code use the column-major order. The first index on a matrix entry represents the column and the second index represents the row.

```
Input:  matrix      ; a 4x4 matrix
Output: translation ; a 2 component vector
        scale       ; a 2 component vector
        angle       ; rotation
        m11         ; 1,1 coordinate of 2x2 matrix
        m12         ; 1,2 coordinate of 2x2 matrix
        m21         ; 2,1 coordinate of 2x2 matrix
        m22         ; 2,2 coordinate of 2x2 matrix
Returns false if the matrix cannot be decomposed, true if it can

double row0x = matrix[0][0]
double row0y = matrix[0][1]
double row1x = matrix[1][0]
double row1y = matrix[1][1]

translate[0] = matrix[3][0]
translate[1] = matrix[3][1]

scale[0] = sqrt(row0x * row0x + row0y * row0y)
scale[1] = sqrt(row1x * row1x + row1y * row1y)

// If determinant is negative, one axis was flipped.
double determinant = row0x * row1y - row0y * row1x
if (determinant < 0)
    // Flip axis with minimum unit vector dot product.
    if (row0x < row1y)
        scale[0] = -scale[0]
    else
        scale[1] = -scale[1]

// Renormalize matrix to remove scale.
if (scale[0])
    row0x *= 1 / scale[0]
    row0y *= 1 / scale[0]
if (scale[1])
    row1x *= 1 / scale[1]
    row1y *= 1 / scale[1]

// Compute rotation and renormalize matrix.
angle = atan2(row0y, row0x);

if (angle)
    // Rotate(-angle) = [cos(angle), sin(angle), -sin(angle), cos(angle)]
    //                = [row0x, -row0y, row0y, row0x]
    // Thanks to the normalization above.
    double sn = -row0y
    double cs = row0x
    double m11 = row0x
    double m12 = row0y
    double m21 = row1x
    double m22 = row1y
    row0x = cs * m11 + sn * m21
    row0y = cs * m12 + sn * m22
    row1x = -sn * m11 + cs * m21
    row1y = -sn * m12 + cs * m22

m11 = row0x
m12 = row0y
m21 = row1x
m22 = row1y

// Convert into degrees because our rotation functions expect it.
angle = rad2deg(angle)

return true
```

#### <a id="interpolation-of-decomposed-2d-matrix-values"></a>11.2.2. Interpolation of decomposed 2D matrix values

Before two decomposed 2D matrix values can be interpolated, the following

```
Input: translationA ; a 2 component vector
       scaleA       ; a 2 component vector
       angleA       ; rotation
       m11A         ; 1,1 coordinate of 2x2 matrix
       m12A         ; 1,2 coordinate of 2x2 matrix
       m21A         ; 2,1 coordinate of 2x2 matrix
       m22A         ; 2,2 coordinate of 2x2 matrix
       translationB ; a 2 component vector
       scaleB       ; a 2 component vector
       angleB       ; rotation
       m11B         ; 1,1 coordinate of 2x2 matrix
       m12B         ; 1,2 coordinate of 2x2 matrix
       m21B         ; 2,1 coordinate of 2x2 matrix
       m22B         ; 2,2 coordinate of 2x2 matrix

// If x-axis of one is flipped, and y-axis of the other,
// convert to an unflipped rotation.
if ((scaleA[0] < 0 && scaleB[1] < 0) || (scaleA[1] < 0 && scaleB[0] < 0))
    scaleA[0] = -scaleA[0]
    scaleA[1] = -scaleA[1]
    angleA += angleA < 0 ? 180 : -180

// Don't rotate the long way around.
if (!angleA)
    angleA = 360
if (!angleB)
    angleB = 360

if (abs(angleA - angleB) > 180)
    if (angleA > angleB)
        angleA -= 360
    else
        angleB -= 360
```

Afterwards, each component of the decomposed values translation, scale, angle, m11 to m22 of the source matrix get linearly interpolated with each corresponding component of the destination matrix.

#### <a id="recomposing-to-a-2d-matrix"></a>11.2.3. Recomposing to a 2D matrix

After interpolation, the resulting values are used to transform the elements user space. One way to use these values is to recompose them into a 4x4 matrix. This can be done following the pseudo code below.

Matrices in the pseudo code use the column-major order. The first index on a matrix entry represents the column and the second index represents the row.

```
Input:  translation ; a 2 component vector
        scale       ; a 2 component vector
        angle       ; rotation
        m11         ; 1,1 coordinate of 2x2 matrix
        m12         ; 1,2 coordinate of 2x2 matrix
        m21         ; 2,1 coordinate of 2x2 matrix
        m22         ; 2,2 coordinate of 2x2 matrix
Output: matrix      ; a 4x4 matrix initialized to identity matrix

matrix[0][0] = m11
matrix[0][1] = m12
matrix[1][0] = m21
matrix[1][1] = m22

// Translate matrix.
matrix[3][0] = translate[0] * m11 + translate[1] * m21
matrix[3][1] = translate[0] * m12 + translate[1] * m22

// Rotate matrix.
angle = deg2rad(angle);
double cosAngle = cos(angle);
double sinAngle = sin(angle);

// New temporary, identity initialized, 4x4 matrix rotateMatrix
rotateMatrix[0][0] = cosAngle
rotateMatrix[0][1] = sinAngle
rotateMatrix[1][0] = -sinAngle
rotateMatrix[1][1] = cosAngle

matrix = post-multiply(rotateMatrix, matrix)

// Scale matrix.
matrix[0][0] *= scale[0]
matrix[0][1] *= scale[0]
matrix[1][0] *= scale[1]
matrix[1][1] *= scale[1]
```

## <a id="mathematical-description"></a>12. Mathematical Description of Transform Functions

Mathematically, all transform functions can be represented as 4x4 transformation matrices of the following form:

[image: \begin{bmatrix} m11 & m21 & m31 & m41 \\ m12 & m22 & m32 & m42 \\ m13 & m23 & m33 & m43 \\ m14 & m24 & m34 & m44 \end{bmatrix}]

One translation unit on a matrix is equivalent to 1 pixel in the local coordinate system of the element.

- <a id="MatrixDefined"></a>
A 2D 3x2 matrix with six parameters *a*, *b*, *c*, *d*, *e* and *f* is equivalent to the matrix:
[image: \begin{bmatrix} a & c & 0 & e \\ b & d & 0 & f \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]
- <a id="TranslateDefined"></a>
A 2D translation with the parameters *tx* and *ty* is equivalent to a 3D translation where *tz* has zero as a value.
[image: \begin{bmatrix} 1 & 0 & 0 & tx \\ 0 & 1 & 0 & ty \\ 0 & 0 & 1 & tz \\ 0 & 0 & 0 & 1 \end{bmatrix}]
- <a id="ScaleDefined"></a>
A 2D scaling with the parameters *sx* and *sy* is equivalent to a 3D scale where *sz* has one as a value.
[image: \begin{bmatrix} sx & 0 & 0 & 0 \\ 0 & sy & 0 & 0 \\ 0 & 0 & sz & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]
- <a id="RotateDefined"></a>
A 2D rotation with the parameter *alpha* is equivalent to a 3D rotation with the vector [x,y,z] where *x* has zero as a value, *y* has zero as a value, *z* has one as a value, and the parameter *alpha*.
[image: \begin{bmatrix} 1 - 2 \cdot (y^2 + z^2) \cdot sq & 2 \cdot (x \cdot y \cdot sq - z \cdot sc) & 2 \cdot (x \cdot z \cdot sq + y \cdot sc) & 0 \\ 2 \cdot (x \cdot y \cdot sq + z \cdot sc) & 1 - 2 \cdot (x^2 + z^2) \cdot sq & 2 \cdot (y \cdot z \cdot sq - x \cdot sc) & 0 \\ 2 \cdot (x \cdot z \cdot sq - y \cdot sc) & 2 \cdot (y \cdot z \cdot sq + x \cdot sc) & 1 - 2 \cdot (x^2 + y^2) \cdot sq & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]
where:
[image: \newline sc = \sin (\alpha/2) \cdot \cos (\alpha/2) \newline sq = \sin^2 (\alpha/2)]
- <a id="SkewDefined"></a>
A 2D skew like transformation with the parameters *alpha* and *beta* is equivalent to the matrix:
[image: \begin{bmatrix} 1 & \tan(\alpha) & 0 & 0 \\ \tan(\beta) & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]
- <a id="SkewXDefined"></a>
A 2D skew transformation along the X axis with the parameter *alpha* is equivalent to the matrix:
[image: \begin{bmatrix} 1 & \tan(\alpha) & 0 & 0 \\ 0 & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]
- <a id="SkewYDefined"></a>
A 2D skew transformation along the Y axis with the parameter *beta* is equivalent to the matrix:
[image: \begin{bmatrix} 1 & 0 & 0 & 0 \\ \tan(\beta) & 1 & 0 & 0 \\ 0 & 0 & 1 & 0 \\ 0 & 0 & 0 & 1 \end{bmatrix}]

## <a id="privacy"></a>13. Privacy Considerations

UAs must implement transform operations in a way attackers can not infer information and mount a timing attack.

A timing attack is a method of obtaining information about content that is otherwise protected, based on studying the amount of time it takes for an operation to occur.

At this point there are no information about potential privacy concerns specific to this specification.

## <a id="security"></a>14. Security Considerations

At this point there are no information about potential security concerns specific to this specification.

## <a id="changes"></a>Changes

### <a id="CR20190214"></a>Since the 14 February 2019 Candidate Recommendation

-
Relax syntax of [transform](svg11/coords.md#TransformAttribute) attribute: Do not require commas between [<transform-list>](#typedef-transform-list) items.
-
Simplified grammar of transform functions. (No normative impact.)
-
Remove animation support of [transform](#propdef-transform) from animate and set elements.
-
State that transforms do not transform backgrounds that are propagated to the canvas, and clarify some other interactions with background propagation.
-
Split privacy and security section.
-
Editorial changes.

### <a id="WD20181130"></a>Since the 30 November 2018 Working Draft

-
No substantive changes
-
Boilerplate, styling updates for CR

### <a id="WD20171130"></a>Since the 30 November 2017 Working Draft

-
Remove specification text that makes [patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute), [gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute) presentation attributes representing the [transform](#propdef-transform) property. That is going to get specified by SVG 2 [[SVG2]](#biblio-svg2).
-
Added privacy and security section.
-
Use [[SVG2]](#biblio-svg2) definitions for [transformable elements](#transformable-element).
-
Added special syntax for [transform](svg11/coords.md#TransformAttribute), `[gradientTransform](svg11/pservers.md#LinearGradientElementGradientTransformAttribute)` and `[patternTransform](svg11/pservers.md#PatternElementPatternTransformAttribute)` attributes.
-
Clarify multiplication order by using terms [post-multiply](#post-multiply) and [pre-multiply](#pre-multiply).
-
Clarify index order of matrix entries in pseudo-code.
-
Clarify multiplication order in recomposition pseudo-code.
-
Clarify behavior of [transform](#propdef-transform) on overflow area.
-
Remove translateX(0), translateY(0), scaleX(0), scaleY(0) from the list of neutral elements.
-
Remove any reference of 3D transformations of transform function definitions.
-
Specify interpolation between [<transform-list>](#typedef-transform-list)s to match lengths and avoid matrix interpolation for the common prefix of the two lists.
-
No [transform](#propdef-transform) on non-replaced inline boxes, table-column boxes, and table-column-group boxes.
-
Define target coordinate space for transformations on `[pattern](svg2/pservers.md#elementdef-pattern)`, `[linearGradient](svg2/pservers.md#elementdef-linearGradient)`, `[radialGradient](svg2/pservers.md#elementdef-radialGradient)` and `[clipPath](css-masking-1.md#elementdef-clippath)` elements.
-
Remove 3-value [<rotate()>](#funcdef-transform-rotate) from transform function primitives.
-
Be more specific about computation of [transformation matrix](#transformation-matrix) and [current transformation matrix](#current-transformation-matrix).
-
Define reference box for paint servers and `[clipPath](css-masking-1.md#elementdef-clippath)` element.
-
Specify behavior of transform presentation attribute with 3-value-rotate as start or end value of a transition.
-
Add [stroke-box](#valdef-transform-box-stroke-box) and [content-box](#valdef-transform-box-content-box) to [transform-box](#propdef-transform-box). Align box mapping behavior across all specifications.
-
Editorial changes.

## <a id="acknowledgments"></a>Acknowledgments

The editors would like to thank Robert O’Callahan, Cameron McCormack, Tab Atkins, Gérard Talbot, L. David Baron, Rik Cabanier, Brian Birtles, Benoit Jacob, Ken Shoemake, Alan Gresley, Maciej Stochowiak, Sylvain Galineau, Rafal Pietrak, Shane Stephens, Matt Rakow, XiangHongAi, Fabio M. Costa, Nivesh Rajbhandari, Rebecca Hauck, Gregg Tavares, Graham Clift, Erik Dahlström, Alexander Zolotov, Amelia Bellamy-Royds and Boris Zbarsky for their careful reviews, comments, and corrections.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-masking"></a>[CSS-MASKING] **
: Dirk Schulze; Brian Birtles; Tab Atkins Jr.. [CSS Masking Module Level 1](css-masking-1.md). URL: [https://drafts.csswg.org/css-masking-1/](css-masking-1.md)
**<a id="biblio-css-overflow-3"></a>[CSS-OVERFLOW-3] **
: Elika Etemad; Florian Rivoal. CSS Overflow Module Level 3. URL: https://drafts.csswg.org/css-overflow-3/
**<a id="biblio-css-sizing-3"></a>[CSS-SIZING-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Box Sizing Module Level 3. URL: https://drafts.csswg.org/css-sizing-3/
**<a id="biblio-css-syntax-3"></a>[CSS-SYNTAX-3] **
: Tab Atkins Jr.; Simon Sapin. [CSS Syntax Module Level 3](css-syntax-3.md). URL: [https://drafts.csswg.org/css-syntax/](css-syntax-3.md)
**<a id="biblio-css-values-3"></a>[CSS-VALUES-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3bg"></a>[CSS3BG] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-cssom"></a>[CSSOM] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-svg-animations"></a>[SVG-ANIMATIONS] **
: SVG Animations Level 2. Editor's Draft. URL: https://svgwg.org/specs/animations/
**<a id="biblio-svg2"></a>[SVG2] **
: Karl Dubost; et al. [Scalable Vector Graphics (SVG) 2](svg2/index.md). URL: [https://w3c.github.io/svgwg/svg2-draft/](svg2/index.md)
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css-transforms-2"></a>[CSS-TRANSFORMS-2] **
: Tab Atkins Jr.; et al. [CSS Transforms Module Level 2](css-transforms-2.md). URL: [https://drafts.csswg.org/css-transforms-2/](css-transforms-2.md)
**<a id="biblio-cssom-view"></a>[CSSOM-VIEW] **
: Simon Fraser; Emilio Cobos Álvarez. CSSOM View Module. URL: https://drafts.csswg.org/cssom-view/
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [transform](#propdef-transform) | none \| <transform-list> | none | transformable elements | no | refer to the size of reference box | transform list, see interpolation rules | per grammar | as specified, but with lengths made absolute |
| [transform-box](#propdef-transform-box) | content-box \| border-box \| fill-box \| stroke-box \| view-box | view-box | transformable elements | no | N/A | discrete | per grammar | specified keyword |
| [transform-origin](#propdef-transform-origin) | [ left \| center \| right \| top \| bottom \| <length-percentage> ] \| [ left \| center \| right \| <length-percentage> ] [ top \| center \| bottom \| <length-percentage> ] <length>? \| [ [ center \| left \| right ] && [ center \| top \| bottom ] ] <length>? | 50% 50% | transformable elements | no | refer to the size of reference box | by computed value | per grammar | see background-position |

**✔**MDN

MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN

**⚠**MDN

**✔**MDN
