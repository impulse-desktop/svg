<!-- https://drafts.csswg.org/css-masking-1/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Masking Module Level 1

<a id="w3c-state"></a>Editor’s Draft, 28 April 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-masking-1/
**Latest published version: **
: https://www.w3.org/TR/css-masking-1/
**Previous Versions: **
: https://www.w3.org/TR/2021/CRD-css-masking-1-20210805/
**Implementation Report: **
: https://test.csswg.org/harness/results/css-masking-1_dev/grouped/
**Test Suites: **
: https://test.csswg.org/suites/css-masking/nightly-unstable/
: https://wpt.fyi/results/css/css-masking/
**Feedback: **
: CSSWG Issues Repository
: GitHub
: [Inline In Spec](#issues-index)
**Editors: **
: Dirk Schulze (Adobe Inc.)
: Brian Birtles (Mozilla Japan)
: Tab Atkins Jr. (Google)
**Suggest an Edit for this Spec: **
: GitHub Editor
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

CSS Masking provides two means for partially or fully hiding portions of visual elements: masking and clipping.

Masking describes how to use another graphical element or image as a luminance or alpha mask. Typically, rendering an element via CSS or SVG can conceptually be described as if the element, including its children, are drawn into a buffer and then that buffer is composited into the element’s parent. Luminance and alpha masks influence the transparency of this buffer before the compositing stage.

Clipping describes the visible region of visual elements. The region can be described by using certain SVG graphics elements or basic shapes. Anything outside of this region is not rendered.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-masking” in the title, like this: “[css-masking] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This section is not normative.*

This specification defines two different graphical operations which both fully or partly hide portions of an object: clipping and masking.

### <a id="clipping"></a>1.1. Clipping

A closed vector path, shape or polygon defines a so called *<a id="clipping-path"></a>clipping path*. This clipping path is a region (in the absence of anti-aliasing) where everything on the “inside” of this region is allowed to show through but everything on the outside is “clipped out” and does not appear on the canvas.

<a id="example-8d45bf9b"></a>

[image: Example Mask]

A clipping path (middle) is applied on a polygon shaded with different colors (left). This results in a “clipped out” shape (right).

The [clip-path](#propdef-clip-path) property can use specified basic shapes as clipping path or reference an [clipPath](#elementdef-clippath) element with [graphics elements](svg2/struct.md#graphics-element) to be used as clipping path.

Clipping *does* affect hit testing; there are no hits outside the clip region, only inside.

### <a id="masking"></a>1.2. Masking

The effect of applying a mask to a graphical object is as if the graphical object will be painted onto the background through a mask, thus completely or partially masking out parts of the graphical object.

<a id="example-c9ae2e2a"></a>

[image: Example Mask]

A luminance mask (middle) is applied on a shape filled with a gradient (left). This results in a masked shape (right).

Masks are applied using the [mask-image](#propdef-mask-image) or [mask-border-source](#propdef-mask-border-source) properties.

The [mask-image](#propdef-mask-image) property may reference a [mask](#elementdef-mask) element. The content of the mask element serves as the mask.

Alternatively, for many simple uses, the [mask-image](#propdef-mask-image) property may refer directly to images to be used as mask, forgoing the need for an explicit [mask](#elementdef-mask) element. This mask can then be sized and positioned just like CSS background images using the [mask-position](#propdef-mask-position), [mask-size](#propdef-mask-size) and other characterizing properties.

The [mask-border-source](#propdef-mask-border-source) property splits a mask into 9 pieces. The pieces may be sliced, scaled and stretched in various ways to fit the size of the [mask border image area](#mask-border-image-area). The [mask-border](#propdef-mask-border) property serves as a shorthand property for mask-border-source and other characterizing properties.

The [mask](#propdef-mask) property serves as a shorthand property for all [mask-border](#propdef-mask-border) and [mask-image](#propdef-mask-image) affiliated properties.

Note: While masking gives many possibilities for enhanced graphical effects and in general provides more control over the “visible portions” of the content, clipping paths can perform better and basic shapes are easier to interpolate.

Masking *does not* affect hit testing.

## <a id="placement"></a>2. Module interactions

This specification defines a set of CSS properties that affect the visual rendering of elements to which those properties are applied. These effects are applied after elements have been sized and positioned according to the Visual formatting model from [[CSS21]](#biblio-css21). Some values of these properties result in the creation of a stacking context. Furthermore, this specification replaces the section Clipping: the clip property from [[CSS21]](#biblio-css21).

The compositing model follows the SVG compositing model [[SVG11]](#biblio-svg11): First the element is styled under absence of filter effects, masking, clipping and opacity. Then the element and its descendants are drawn on a temporary canvas. In a last step the following effects are applied to the element in order: filter effects [[FILTER-EFFECTS]](#biblio-filter-effects), clipping, masking and opacity.

This specification allows compositing multiple mask layers with the Porter Duff compositing operators defined in CSS Compositing and Blending [[COMPOSITING-1]](#biblio-compositing-1).

The term [object bounding box](svg2/coords.md#TermObjectBoundingBox) follows the definition in SVG 1.1 [[SVG11]](#biblio-svg11).

## <a id="values"></a>3. Values

This specification follows the CSS property definition conventions from [[CSS21]](#biblio-css21). Basic shapes are defined in CSS Shapes Module Level 1 [[CSS-SHAPES]](#biblio-css-shapes). Value types not defined in these specifications are defined in CSS Values and Units Module Level 3 [[CSS3VAL]](#biblio-css3val).

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept CSS-wide keywords such as inherit as their property value [[CSS3VAL]](#biblio-css3val). For readability it has not been repeated explicitly.

## <a id="terminology"></a>4. Terminology

Definitions of CSS properties and values in this specification are analogous to definitions in CSS Backgrounds and Borders [[CSS3BG]](#biblio-css3bg). To avoid redundancy, this specification relies on descriptions and definitions of CSS Backgrounds and Borders. The following terms in CSS Backgrounds and Borders have the following meaning in this specification:

<a id="term-matching"></a>

| Term in CSS Masking |  |
| --- | --- |
| [mask layer image](#mask-layer-image) | background images |
| [mask painting area](#mask-painting-area) | [background painting area](css-backgrounds-3.md#background-painting-area) |
| *<a id="mask-size"></a>mask-size* | background-size |
| *<a id="mask-position"></a>mask-position* | background-position |
| [mask positioning area](#mask-positioning-area) | [background positioning area](css-backgrounds-3.md#background-positioning-area) |
| [mask border image](#mask-border-image) | border-image |
| [mask border image area](#mask-border-image-area) | [border image area](css-backgrounds-3.md#border-image-area) |

## <a id="clipping-paths"></a>5. Clipping Paths

The clipping path restricts the region to which paint can be applied, the so-called *<a id="clipping-region"></a>clipping region*. Conceptually, any parts of the drawing that lie outside of this region are not drawn. This includes any content, background, borders, text decoration, outline and visible scrolling mechanism of the element to which the clipping path is applied, and those of its descendants.

An element’s ancestors may also clip portions of their content (e.g., via their own [clip](#propdef-clip) or [clip-path](#propdef-clip-path) properties and/or if their overflow property is not visible). What is rendered is the cumulative intersection.

If the clipping region exceeds the bounds of the UA’s document window, content may be clipped to that window by the native operating environment.

A clipping path affects the rendering of an element. It does not affect the element’s inherent geometry. The geometry of a clipped element (i.e. an element which references a [clipPath](#elementdef-clippath) element via a [clip-path](#propdef-clip-path) property, or a child of the referencing element) must remain the same as if it were not clipped.

<a id="example-b04372df"></a> Consider a shape that is clipped by a clipping path applied to an ancestor:

```
<g clip-path="circle()">
  <path id="shape" d="M0,0 L10,10, L 20,0 z"/>
</g>
```

The shape is referenced by a [use](svg2/struct.md#elementdef-use) element:

```
<use xlink:href="#shape"/>
```

The geometry of the shape is not influenced by the circular clipping path.

By default, pointer-events must not be dispatched on the clipped-out (non-visible) regions of a shape. For example, an element with a dimension of 10px to 10px which is clipped to a circle with a radius of 5px will not receive click events outside the [clipping region](#clipping-region).

### <a id="the-clip-path"></a>5.1. Clipping Shape: the [clip-path](#propdef-clip-path) property

| Name: | *<a id="propdef-clip-path"></a>clip-path* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<clip-source>](#typedef-clip-source) [\|](css-values-4.md#comb-one) [ [<basic-shape>](css-shapes-1.md#typedef-basic-shape) [\|\|](css-values-4.md#comb-any) [<geometry-box>](#typedef-geometry-box) ] \| none |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified, but with [<url>](css-values-4.md#url-value) values made absolute |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

Specifies a basic shape or references a [clipPath](#elementdef-clippath) element to create a [clipping path](#clipping-path).

```
<a id="typedef-clip-source"></a><clip-source> = <url>
```

```
<a id="typedef-geometry-box"></a><geometry-box> = <shape-box> | fill-box | stroke-box | view-box
```

**[<basic-shape>](css-shapes-1.md#typedef-basic-shape) **
:

A basic shape function as defined in the CSS Shapes module [[CSS-SHAPES]](#biblio-css-shapes). A basic shape makes use of the specified reference box to size and position the basic shape. If no reference box is specified, the [border-box](#valdef-mask-clip-border-box) will be used as reference box.

**[<geometry-box>](#typedef-geometry-box) **
:

If specified in combination with a [<basic-shape>](css-shapes-1.md#typedef-basic-shape) it provides the reference box for the <basic-shape>.

If specified by itself, uses the edges of the specified box, including any corner shaping (e.g. defined by [border-radius](css-backgrounds-3.md#propdef-border-radius) [[CSS3BG]](#biblio-css3bg)), as clipping path. See also [“Shapes from box values”](css-shapes-1.md#shapes-from-box-values) [[CSS-SHAPES]](#biblio-css-shapes).

***<a id="valdef-clip-path-fill-box"></a>fill-box* **
:

Uses the [object bounding box](svg2/coords.md#TermObjectBoundingBox) as reference box.

***<a id="valdef-clip-path-stroke-box"></a>stroke-box* **
:

Uses the [stroke bounding box](#stroke-bounding-box) as reference box.

***<a id="valdef-clip-path-view-box"></a>view-box* **
:

Uses the nearest [SVG viewport](svg11/intro.md#TermSVGViewport) as reference box.

If a [viewBox](svg2/coords.md#ViewBoxAttribute) attribute is specified for the [SVG viewport](svg11/intro.md#TermSVGViewport) creating element:

-
The reference box is positioned at the origin of the coordinate system established by the [viewBox](svg2/coords.md#ViewBoxAttribute) attribute.
-
The dimension of the reference box is set to the *width* and *height* values of the [viewBox](svg2/coords.md#ViewBoxAttribute) attribute.

**none **
:

No [clipping path](#clipping-path) gets created.

For SVG elements without associated CSS layout box, the [used value](css-cascade-5.md#used-value) for [content-box](#valdef-mask-clip-content-box) and [padding-box](#valdef-mask-clip-padding-box) is [fill-box](#valdef-clip-path-fill-box) and for [border-box](#valdef-mask-clip-border-box) and margin-box is [stroke-box](#valdef-clip-path-stroke-box).

For elements with associated CSS layout box, the [used value](css-cascade-5.md#used-value) for [fill-box](#valdef-clip-path-fill-box) is [content-box](#valdef-mask-clip-content-box) and for [stroke-box](#valdef-clip-path-stroke-box) and [view-box](#valdef-clip-path-view-box) is [border-box](#valdef-mask-clip-border-box).

A computed value of other than none results in the creation of a stacking context [[CSS21]](#biblio-css21) the same way that CSS [opacity](css-color-4.md#propdef-opacity) [[CSS3COLOR]](#biblio-css3color) does for values other than 1.

If the URI reference is not valid (e.g it points to an object that doesn’t exist or the object is not a [clipPath](#elementdef-clippath) element), no clipping is applied.

<a id="example-b11e2c6d"></a> This example demonstrates the use of the basic shape [<polygon()>](css-shapes-1.md#funcdef-basic-shape-polygon) as clipping path. Each space separated length pair represents one point of the polygon. The visualized clipping path can be seen in the [introduction](#clipping).

```
clip-path: polygon(15px 99px, 30px 87px, 65px 99px, 85px 55px,
        122px 57px, 184px 73px, 198px 105px, 199px 150px,
        145px 159px, 155px 139px, 126px 120px, 112px 138px,
        80px 128px, 39px 126px, 24px 104px);
```

<a id="example-32370653"></a> In this example, the [clip-path](#propdef-clip-path) property references an SVG [clipPath](#elementdef-clippath) element. Each comma separated length pair represents one point of the polygon. As for the previous example, the visualized clipping path can be seen in the [introduction](#clipping).

```
clip-path: url("#clip1");
```

```
<clipPath id="clip1">
    <polygon points="15,99 30,87 65,99 85,55 122,57 184,73 198,105
        199,150 145,159 155,139 126,120 112,138 80,128 39,126 24,104"/>
</clipPath>
```

The [clip-path](#propdef-clip-path) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

## <a id="svg-clipping-paths"></a>6. SVG Clipping Path Sources

### <a id="ClipPathElement"></a>6.1. The [clipPath](#elementdef-clippath) element

| Name: | *<a id="elementdef-clippath"></a>`clipPath`* |
| --- | --- |
| Categories: | [container elements](svg2/struct.md#container-element), [never-rendered element](svg2/render.md#TermNeverRenderedElement) |
| Content model: | Any number of the following elements, in any order: - [descriptive](svg11/intro.md#TermDescriptiveElement) — [desc](svg2/struct.md#elementdef-desc), [title](svg2/struct.md#elementdef-title), [metadata](svg2/struct.md#elementdef-metadata) - [animation](svg11/intro.md#TermAnimationElement) — animate, animateMotion, animateTransform, set - [shape](svg11/intro.md#TermShapeElement) — [circle](svg2/shapes.md#elementdef-circle), [ellipse](svg2/shapes.md#elementdef-ellipse), [line](svg2/shapes.md#elementdef-line), [path](svg2/paths.md#elementdef-path), [polygon](svg2/shapes.md#elementdef-polygon), [polyline](svg2/shapes.md#elementdef-polyline), [rect](svg2/shapes.md#elementdef-rect) - [text](svg2/text.md#elementdef-text) - [use](svg2/struct.md#elementdef-use) - [script](svg2/interact.md#elementdef-script) |
| Attributes: | - [conditional processing attributes](svg11/intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](svg11/struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](svg11/struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](svg11/struct.md#SystemLanguageAttribute) - [core attributes](svg11/intro.md#TermCoreAttributes) — [‘id’](svg11/struct.md#IDAttribute), [‘xml:base’](svg11/struct.md#XMLBaseAttribute), [‘xml:lang’](svg11/struct.md#XMLLangAttribute), [‘xml:space’](svg11/struct.md#XMLSpaceAttribute) - [presentation attributes](svg11/intro.md#TermPresentationAttribute) — alignment-baseline, baseline-shift, [clip](#propdef-clip), [clip-path](#propdef-clip-path), [clip-rule](#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](filter-effects-1.md#propdef-color-interpolation-filters), [color-profile](svg11/color.md#ColorProfileProperty), color-rendering, cursor, direction, display, dominant-baseline, [enable-background](svg11/filters.md#EnableBackgroundProperty), [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](filter-effects-1.md#propdef-filter), [flood-color](filter-effects-1.md#propdef-flood-color), [flood-opacity](filter-effects-1.md#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, [glyph-orientation-horizontal](svg11/text.md#GlyphOrientationHorizontalProperty), glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), [kerning](svg11/text.md#KerningProperty), letter-spacing, [lighting-color](filter-effects-1.md#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [‘class’](svg11/styling.md#ClassAttribute) - [‘style’](svg11/styling.md#StyleAttribute) - [‘externalResourcesRequired’](svg11/struct.md#ExternalResourcesRequiredAttribute) - [‘transform’](svg11/coords.md#TransformAttribute) - ‘[clipPathUnits](#element-attrdef-clippath-clippathunits)’ |
| DOM Interfaces: | [SVGClipPathElement](#InterfaceSVGClipPathElement) |

*Attribute definitions:*

***<a id="element-attrdef-clippath-clippathunits"></a>`clipPathUnits`* = "[userSpaceOnUse](#valdef-clippathunits-userspaceonuse) | [objectBoundingBox](#valdef-clippathunits-objectboundingbox)" **
:

Defines the coordinate system for the contents of the [clipPath](#elementdef-clippath).

***<a id="valdef-clippathunits-userspaceonuse"></a>userSpaceOnUse* **
:

The contents of the [clipPath](#elementdef-clippath) represent values in the current [user coordinate system](css-transforms-1.md#user-coordinate-system) in place at the time when the clipPath element is referenced (i.e., the user coordinate system for the element referencing the clipPath element via the [clip-path](#propdef-clip-path) property).

***<a id="valdef-clippathunits-objectboundingbox"></a>objectBoundingBox* **
:

The coordinate system has its origin at the top left corner of the [bounding box](svg2/coords.md#bounding-box) of the element to which the clipping path applies to and the same width and height of this bounding box. [User coordinates](svg2/coords.md#Units) are sized equivalently to the CSS [px](css-values-4.md#px) unit.

If attribute [clipPathUnits](#element-attrdef-clippath-clippathunits) is not specified, then the effect is as if a value of [userSpaceOnUse](#valdef-clippathunits-userspaceonuse) were specified.

Animatable: yes.

CSS properties inherit into the [clipPath](#elementdef-clippath) element from its ancestors; properties do *not* inherit from the element referencing the clipPath element.

[clipPath](#elementdef-clippath) elements are never rendered directly; their only usage is as something that can be referenced using the [clip-path](#propdef-clip-path) property. The display property does not apply to the clipPath element; thus, clipPath elements are not directly rendered even if the display property is set to a value other than none, and clipPath elements are available for referencing even when the display property on the clipPath element or any of its ancestors is set to none.

A [clipPath](#elementdef-clippath) element can contain [path](svg2/paths.md#elementdef-path) elements, [text](svg2/text.md#elementdef-text) elements, [basic shapes](svg2/shapes.md#basic-shape) (such as [circle](svg2/shapes.md#elementdef-circle)) or a [use](svg2/struct.md#elementdef-use) element. If a use element is a child of a clipPath element, it must directly reference path, text or basic shapes elements. Indirect references are an error and the clipPath element must be ignored.

<a id="issue-d0c561ed"></a> Firefox disables rendering of elements referencing clipPaths with violated content model. No browser ignores clipPath on use with indirect reference. [Issue #17]

The raw geometry of each child element exclusive of rendering properties such as [fill](svg2/painting.md#FillProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty) within a [clipPath](#elementdef-clippath) conceptually defines a 1-bit mask (with the possible exception of anti-aliasing along the edge of the geometry) which represents the silhouette of the graphics associated with that element. Anything outside the outline of the object is masked out. If a child element is made invisible by display or visibility it does not contribute to the clipping path. When the clipPath element contains multiple child elements, the silhouettes of the child elements are logically OR’d together to create a single silhouette which is then used to restrict the region onto which paint can be applied. Thus, a point is inside the clipping path if it is inside any of the children of the clipPath.

<a id="issue-1eaffdcc"></a> Define raw geometry with regards to CSS properties that affect it. Especially on text. [Issue #170]

For a given [graphics element](svg2/struct.md#graphics-element), the actual clipping path used will be the intersection of the clipping path specified by its [clip-path](#propdef-clip-path) property (if any) with any clipping paths on its ancestors, as specified by the clip-path property on the elements which establish a new viewport. (See [[SVG11]](#biblio-svg11))

A couple of additions:

-
The [clipPath](#elementdef-clippath) element itself and its child elements do *not* inherit clipping paths from the ancestors of the clipPath element.
-
The [clipPath](#elementdef-clippath) element or any of its children can specify property [clip-path](#propdef-clip-path). If a valid clip-path reference is placed on a clipPath element, the resulting clipping path is the intersection of the contents of the clipPath element with the referenced clipping path. If a valid clip-path reference is placed on one of the children of a clipPath element, then the given child element is clipped by the referenced clipping path before OR’ing the silhouette of the child element with the silhouettes of the other child elements.
-
An empty clipping path will completely clip away the element that had the [clip-path](#propdef-clip-path) property applied.

### <a id="the-clip-rule"></a>6.2. Winding Rules: the [clip-rule](#propdef-clip-rule) property

| Name: | *<a id="propdef-clip-rule"></a>clip-rule* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | nonzero [\|](css-values-4.md#comb-one) evenodd |
| [Initial:](css-cascade-5.md#initial-values) | nonzero |
| [Applies to:](css-cascade-5.md#applies-to) | Applies to SVG [graphics elements](svg2/struct.md#graphics-element) |
| [Inherited:](css-cascade-5.md#inherited-property) | yes |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The [clip-rule](#propdef-clip-rule) property indicates the algorithm which is to be used to determine whether a given point is inside a shape for a [clipping region](#clipping-region) created with a [graphics element](svg2/struct.md#graphics-element). The definition of the algorithms and the clip-rule values follows the definition of the [fill-rule](svg2/painting.md#FillRuleProperty) property. See section [“Fill Properties”](svg11/painting.md#FillProperties) in SVG 1.1 [[SVG11]](#biblio-svg11).

***<a id="valdef-clip-rule-nonzero"></a>nonzero* **
:

See description of [fill-rule](svg2/painting.md#FillRuleProperty) property [[SVG11]](#biblio-svg11).

***<a id="valdef-clip-rule-evenodd"></a>evenodd* **
:

See description of [fill-rule](svg2/painting.md#FillRuleProperty) property [[SVG11]](#biblio-svg11).

The [clip-rule](#propdef-clip-rule) property only applies to [graphics elements](svg2/struct.md#graphics-element) that are contained within a [clipPath](#elementdef-clippath) element.

Note: The [clip-rule](#propdef-clip-rule) property does not apply to [<basic-shape>](css-shapes-1.md#typedef-basic-shape)s.

<a id="example-3153479a"></a>

The following drawing illustrates the [nonzero](#valdef-clip-rule-nonzero) rule:

[image: Shape with nonzero rule.]

Three shapes from left to right: Star with 5 points drawn in one continuous, overlapping line; 2 clockwise drawn circles, one contains the other and both are subpaths of the same shape; 2 circles, one containing the other with the bigger one drawn in a clockwise direction and the smaller one in a counter-clockwise direction and both belonging to the same shape. Only the last shape has a "hole".

The following drawing illustrates the [evenodd](#valdef-clip-rule-evenodd) rule:

[image: Shape with even-odd rule.]

Three shapes from left to right: Star with 5 points drawn in one continuous, overlapping line; 2 clockwise drawn circles, one contains the other and both are subpaths of the same shape; 2 circles, one containing the other with the bigger one drawn in a clockwise direction and the smaller one in a counter-clockwise direction and both belonging to the same shape. All 3 shapes have a "hole".

<a id="example-4b844436"></a> The following fragment of code will cause an even-odd clipping rule to be applied to the clipping path because [clip-rule](#propdef-clip-rule) is specified on the [path](svg2/paths.md#elementdef-path) element that defines the clipping shape:

```
<g clip-rule="nonzero">
  <clipPath id="MyClip">
    <path d="..." clip-rule="evenodd" />
  </clipPath>
  <rect clip-path="url(#MyClip)" ... />
</g>
```

whereas the following fragment of code will *not* cause an evenodd clipping rule to be applied because the [clip-rule](#propdef-clip-rule) is specified on the referencing element, not on the object defining the clipping shape:

```
<g clip-rule="nonzero">
  <clipPath id="MyClip">
    <path d="..." />
  </clipPath>
  <rect clip-path="url(#MyClip)" clip-rule="evenodd" ... />
</g>
```

The [clip-rule](#propdef-clip-rule) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

## <a id="positioned-masks"></a>7. Positioned Masks

### <a id="the-mask-image"></a>7.1. Mask Image Source: the [mask-image](#propdef-mask-image) property

| Name: | *<a id="propdef-mask-image"></a>mask-image* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<mask-reference>](#typedef-mask-reference)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item the keyword none, a computed [<image>](css-images-3.md#typedef-image), or a computed [<url>](css-values-4.md#url-value) |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

This property sets the *<a id="mask-layer-image"></a>mask layer image* of an element. Where:

```
<a id="typedef-mask-reference"></a><mask-reference> = none | <image> | <mask-source>
```

```
<a id="typedef-mask-source"></a><mask-source> = <url>
```

***<a id="valdef-mask-image-url"></a><url>* **
:

A URL reference to a [mask](#elementdef-mask) element (for example url(commonmasks.svg#mask)) or to a CSS image.

**none **
:

A value of none counts as a transparent black image layer.

A computed value of other than none results in the creation of a stacking context [[CSS21]](#biblio-css21) the same way that CSS [opacity](css-color-4.md#propdef-opacity) [[CSS3COLOR]](#biblio-css3color) does for values other than 1.

A mask reference that is an empty image (zero width or zero height), that fails to download, is not a reference to an [mask](#elementdef-mask) element, is non-existent, or that cannot be displayed (e.g. because it is not in a supported image format) still counts as an image layer of transparent black.

See the section [“Mask processing”](#MaskValues) for how to process a [mask layer image](#mask-layer-image).

Note: A value of none in a list of [<mask-reference>](#typedef-mask-reference)s may influence the masking operation depending on the used compositing operator specified by [mask-composite](#propdef-mask-composite).

Note: A [<mask-source>](#typedef-mask-source) counts as mask layer and can be combined in a repeatable [<mask-reference>](#typedef-mask-reference) list with [<image>](css-images-3.md#typedef-image) or further <mask-source> list items.

Note: An element can also be masked with [mask-border-source](#propdef-mask-border-source). See mask-border-source for the interaction of that property with [mask-image](#propdef-mask-image).

<a id="example-4c7f99bf"></a> Examples for mask references:

```
body { mask-image: linear-gradient(black 0%, transparent 100%) }
p { mask-image: none }
div { mask-image: url(resources.svg#mask2) }
```

See the section [“Layering multiple mask layer images”](#layering) for how [mask-image](#propdef-mask-image) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-mode"></a>7.2. Mask Image Interpretation: the [mask-mode](#propdef-mask-mode) property

| Name: | *<a id="propdef-mask-mode"></a>mask-mode* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<masking-mode>](#typedef-masking-mode)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | match-source |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item the keyword as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The [mask-mode](#propdef-mask-mode) property indicates whether the [<mask-reference>](#typedef-mask-reference) is treated as luminance mask or alpha mask. (See [Mask processing](#MaskValues).)

```
<a id="typedef-masking-mode"></a><masking-mode> = alpha | luminance | match-source
```

Values have the following meanings:

***<a id="valdef-mask-mode-alpha"></a>alpha* **
:

A value of [alpha](#valdef-mask-mode-alpha) indicates that the alpha values of the [mask layer image](#mask-layer-image) should be used as the mask values. See [Calculating mask values](#MaskValues).

***<a id="valdef-mask-mode-luminance"></a>luminance* **
:

A value of [luminance](#valdef-mask-mode-luminance) indicates that the luminance values of the [mask layer image](#mask-layer-image) should be used as the mask values. See [Calculating mask values](#MaskValues).

***<a id="valdef-mask-mode-match-source"></a>match-source* **
:

If the [<mask-reference>](#typedef-mask-reference) of the [mask-image](#propdef-mask-image) property is of type [<mask-source>](#typedef-mask-source) the value specified by the referenced [mask](#elementdef-mask) element’s [mask-type](#propdef-mask-type) property must be used.

If the [<mask-reference>](#typedef-mask-reference) of the [mask-image](#propdef-mask-image) property is of type [<image>](css-images-3.md#typedef-image) the alpha values of the [mask layer image](#mask-layer-image) should be used as the mask values.

<a id="example-ec098273"></a> In the following example, the [mask-type](#propdef-mask-type) property sets the mask type value for the [mask](#elementdef-mask) element to [alpha](#valdef-mask-type-alpha). The [mask-image](#propdef-mask-image) property has a reference to this mask element and the [mask-mode](#propdef-mask-mode) property has a value of [luminance](#valdef-mask-mode-luminance). The mask-mode property will override the definition of mask-type to [luminance](#valdef-mask-type-luminance).

The [mask-mode](#propdef-mask-mode) property must not affect the masking mode of [mask-border-source](#propdef-mask-border-source).

```
<mask id="SVGMask" mask-type="alpha" maskContentUnits="objectBoundingBox">
  <radialGradient id="radialFill">
    <stop stop-color="white" offset="0"/>
    <stop stop-color="black" offset="1"/>
  </radialGradient>
  <circle fill="url(#radialFill)" cx="0.5" cy="0.5" r="0.5"/>
</mask>

<style>
  rect {
    mask-image: url(#SVGMask);
    mask-mode: luminance;
  }
</style>

<rect width="200" height="200" fill="green"/>
```

See the section [“Layering multiple mask layer images”](#layering) for how [mask-mode](#propdef-mask-mode) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-repeat"></a>7.3. Tiling Mask Images: the [mask-repeat](#propdef-mask-repeat) property

| Name: | *<a id="propdef-mask-repeat"></a>mask-repeat* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<repeat-style>](css-backgrounds-3.md#typedef-repeat-style)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | repeat |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item a pair of keywords, one per dimension |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

Specifies how [mask layer images](#mask-layer-image) are tiled after they have been [sized](#the-mask-size) and [positioned](#the-mask-position).

See [background-repeat](css-backgrounds-3.md#propdef-background-repeat) property [[CSS3BG]](#biblio-css3bg) for the definitions of the property values.

<a id="example-6b3f7f82"></a>

```
body {
    background-color: blue;
    mask-image: url(dot-mask.png) luminance;
    mask-repeat: space;
}
```

[image: Image of an element with a dotted mask.]

The effect of space: the [mask layer image](#mask-layer-image) of a dot is tiled to cover the whole [mask painting area](#mask-painting-area) and the mask layer images are equally spaced.

See the section [“Layering multiple mask layer images”](#layering) for how [mask-repeat](#propdef-mask-repeat) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-position"></a>7.4. Positioning Mask Images: the [mask-position](#propdef-mask-position) property

| Name: | *<a id="propdef-mask-position"></a>mask-position* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <position>[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | 0% 0% |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to size of [mask painting area](#mask-painting-area) *minus* size of [mask layer image](#mask-layer-image); see text [background-position](css-backgrounds-3.md#propdef-background-position) [[CSS3BG]](#biblio-css3bg) |
| [Computed value:](css-cascade-5.md#computed) | list, each item consists of two keywords representing the origin and two offsets from that origin, each given as an absolute length (if given a [<length>](css-values-4.md#length-value)), otherwise as a percentage. |
| Canonical order: | per grammar |
| Animation type: | repeatable list |
| Media: | visual |

See the [background-position](css-backgrounds-3.md#propdef-background-position) property [[CSS3BG]](#biblio-css3bg) for the definitions of the property values.

<a id="example-80025e44"></a> In the example below, the (single) image is placed in the lower-right corner of the viewport.

```
body {
    mask-image: url("logo.png");
    mask-position: 100% 100%;
    mask-repeat: no-repeat;
}
```

<a id="example-dc3e5c49"></a> Mask positions can also be relative to other corners than the top left. E.g., the following puts the background image 10px from the bottom and 3em from the right:

```
mask-position: right 3em bottom 10px
```

See the section [“Layering multiple mask layer images”](#layering) for how [mask-position](#propdef-mask-position) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-clip"></a>7.5. Masking Area: the [mask-clip](#propdef-mask-clip) property

| Name: | *<a id="propdef-mask-clip"></a>mask-clip* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [ [<coord-box>](css-box-4.md#typedef-coord-box) [\|](css-values-4.md#comb-one) no-clip ][#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | border-box |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item the keyword as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

For [mask layer images](#mask-layer-image) that do not reference a [mask](#elementdef-mask) element, [mask-clip](#propdef-mask-clip) determines the *<a id="mask-painting-area"></a>mask painting area*, which determines the area that is affected by the mask. The painted content of an element must be restricted to this area.

The [mask-clip](#propdef-mask-clip) property has no affect on a [mask layer image](#mask-layer-image) that references a [mask](#elementdef-mask) element. The [x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y), [width](#element-attrdef-mask-width), [height](#element-attrdef-mask-height) and [maskUnits](#element-attrdef-mask-maskunits) attributes on the mask element determine the [mask painting area](#mask-painting-area) for mask references.

Values have the following meanings:

***<a id="valdef-mask-clip-content-box"></a>content-box* **
:

The painted content is restricted to (clipped to) the *content box*.

***<a id="valdef-mask-clip-padding-box"></a>padding-box* **
:

The painted content is restricted to (clipped to) the *padding box*.

***<a id="valdef-mask-clip-border-box"></a>border-box* **
:

The painted content is restricted to (clipped to) the *border box*.

***<a id="valdef-mask-clip-fill-box"></a>fill-box* **
:

The painted content is restricted to (clipped to) the [object bounding box](svg2/coords.md#TermObjectBoundingBox).

***<a id="valdef-mask-clip-stroke-box"></a>stroke-box* **
:

The painted content is restricted to (clipped to) the [stroke bounding box](#stroke-bounding-box).

***<a id="valdef-mask-clip-view-box"></a>view-box* **
:

Uses the nearest [SVG viewport](svg11/intro.md#TermSVGViewport) as reference box.

If a [viewBox](svg2/coords.md#ViewBoxAttribute) attribute is specified for the [SVG viewport](svg11/intro.md#TermSVGViewport) creating element:

-
The reference box is positioned at the origin of the coordinate system established by the [viewBox](svg2/coords.md#ViewBoxAttribute) attribute.
-
The dimension of the reference box is set to the *width* and *height* values of the [viewBox](svg2/coords.md#ViewBoxAttribute) attribute.

***<a id="valdef-mask-clip-no-clip"></a>no-clip* **
:

The painted content is not restricted (not clipped).

For SVG elements without associated CSS layout box, the [used value](css-cascade-5.md#used-value)s for [content-box](#valdef-mask-clip-content-box) and [padding-box](#valdef-mask-clip-padding-box) compute to [fill-box](#valdef-mask-clip-fill-box) and for [border-box](#valdef-mask-clip-border-box) compute to [stroke-box](#valdef-mask-clip-stroke-box).

For elements with associated CSS layout box, the [used value](css-cascade-5.md#used-value)s for [fill-box](#valdef-mask-clip-fill-box) compute to [content-box](#valdef-mask-clip-content-box) and for [stroke-box](#valdef-mask-clip-stroke-box) and [view-box](#valdef-mask-clip-view-box) compute to [border-box](#valdef-mask-clip-border-box).

See the section [“Layering multiple mask layer images”](#layering) for how [mask-clip](#propdef-mask-clip) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-origin"></a>7.6. Positioning Area: the [mask-origin](#propdef-mask-origin) property

| Name: | *<a id="propdef-mask-origin"></a>mask-origin* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<coord-box>](css-box-4.md#typedef-coord-box)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | border-box |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item the keyword as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

For elements rendered as a single box, specifies the *<a id="mask-positioning-area"></a>mask positioning area*. For elements rendered as multiple boxes (e.g., inline boxes on several lines, boxes on several pages) specifies which boxes box-decoration-break operates on to determine the mask positioning area.

***<a id="valdef-mask-origin-content-box"></a>content-box* **
:

The position is relative to the *content box*.

***<a id="valdef-mask-origin-padding-box"></a>padding-box* **
:

The position is relative to the *padding box*. (For single boxes 0 0 is the upper left corner of the padding edge, 100% 100% is the lower right corner.)

***<a id="valdef-mask-origin-border-box"></a>border-box* **
:

The position is relative to the *border box*.

***<a id="valdef-mask-origin-fill-box"></a>fill-box* **
:

The position is relative to the [object bounding box](svg2/coords.md#TermObjectBoundingBox).

***<a id="valdef-mask-origin-stroke-box"></a>stroke-box* **
:

The position is relative to the [stroke bounding box](#stroke-bounding-box).

***<a id="valdef-mask-origin-view-box"></a>view-box* **
:

Uses the nearest [SVG viewport](svg11/intro.md#TermSVGViewport) as reference box.

If a [viewBox](svg2/coords.md#ViewBoxAttribute) attribute is specified for the [SVG viewport](svg11/intro.md#TermSVGViewport) creating element:

-
The reference box is positioned at the origin of the coordinate system established by the [viewBox](svg2/coords.md#ViewBoxAttribute) attribute.
-
The dimension of the reference box is set to the *width* and *height* values of the [viewBox](svg2/coords.md#ViewBoxAttribute) attribute.

For SVG elements without associated CSS layout box, the values [content-box](#valdef-mask-origin-content-box), [padding-box](#valdef-mask-origin-padding-box) and [border-box](#valdef-mask-origin-border-box) compute to [fill-box](#valdef-mask-origin-fill-box).

For elements with associated CSS layout box, the values [fill-box](#valdef-mask-origin-fill-box), [stroke-box](#valdef-mask-origin-stroke-box) and [view-box](#valdef-mask-origin-view-box) compute to the initial value of [mask-origin](#propdef-mask-origin).

Note: The [mask-origin](#propdef-mask-origin) property is similar to the [background-origin](css-backgrounds-3.md#propdef-background-origin) property [[CSS3BG]](#biblio-css3bg), but it has a different set of values, and a different initial value.

Note: If [mask-clip](#propdef-mask-clip) is [padding-box](#valdef-mask-origin-padding-box), [mask-origin](#propdef-mask-origin) is [border-box](#valdef-mask-origin-border-box), [mask-position](#propdef-mask-position) is top left (the initial value), and the element has a non-zero border, then the top and left of the [mask layer image](#mask-layer-image) will be clipped.

See the section [“Layering multiple mask layer images”](#layering) for how [mask-origin](#propdef-mask-origin) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-size"></a>7.7. Sizing Mask Images: the [mask-size](#propdef-mask-size) property

| Name: | *<a id="propdef-mask-size"></a>mask-size* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<bg-size>](css-backgrounds-3.md#typedef-bg-size)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | auto |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item as specified, but with lengths made absolute |
| Canonical order: | per grammar |
| Animation type: | repeatable list |
| Media: | visual |

Specifies the size of the [mask layer images](#mask-layer-image).

See [background-size](css-backgrounds-3.md#propdef-background-size) property [[CSS3BG]](#biblio-css3bg) for the definitions of the property values.

See the section [“Layering multiple mask layer images”](#layering) for how [mask-size](#propdef-mask-size) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask-composite"></a>7.8. Compositing mask layers: the [mask-composite](#propdef-mask-composite) property

| Name: | *<a id="propdef-mask-composite"></a>mask-composite* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<compositing-operator>](#typedef-compositing-operator)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | add |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) without the [defs](svg2/struct.md#elementdef-defs) element and all [graphics elements](svg2/struct.md#graphics-element) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | list, each item the keyword as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

```
<a id="typedef-compositing-operator"></a><compositing-operator> = add | subtract | intersect | exclude
```

Each keyword represents a Porter-Duff compositing operator [[COMPOSITING-1]](#biblio-compositing-1) which defines the compositing operation used on the current mask layer with the mask layers below it.

In the following, the current mask layer is referred to *<a id="source"></a>source*, all mask layers below it (with the corresponding compositing operators applied) are referred to *<a id="destination"></a>destination*.

***<a id="valdef-mask-composite-add"></a>add* **
:

The [source](#source) is placed over the [destination](#destination). (See Porter-Duff compositing operator [source over](compositing-1.md#porterduffcompositingoperators_srcover) for more details.)

***<a id="valdef-mask-composite-subtract"></a>subtract* **
:

The [source](#source) is placed, where it falls outside of the [destination](#destination). (See Porter-Duff compositing operator [source out](compositing-1.md#porterduffcompositingoperators_srcout) for more details.)

***<a id="valdef-mask-composite-intersect"></a>intersect* **
:

The parts of [source](#source) that overlap the [destination](#destination), replace the destination. (See Porter-Duff compositing operator [source in](compositing-1.md#porterduffcompositingoperators_srcin) .)

***<a id="valdef-mask-composite-exclude"></a>exclude* **
:

The non-overlapping regions of [source](#source) and [destination](#destination) are combined. (See Porter-Duff compositing operator [XOR](compositing-1.md#porterduffcompositingoperators_xor).)

If there is no further mask layer, the compositing operator must be ignored. Mask layers must not composite with the element’s content or the content behind the element, instead they must act as if they are rendered into an isolated group.

All mask layers below the current mask layer must be composited before applying the compositing operation for the current mask layer.

<a id="example-065f6c44"></a> This example uses two [mask layer images](#mask-layer-image): *circle.svg* and *rect.svg*.

[image: Example of source-over compositing of mask layers]

Both [mask layer images](#mask-layer-image) are references with the [mask-image](#propdef-mask-image) property:

```
mask-image: circle.svg, rect.svg;
```

The mask layer with *rect.svg* is below the mask layer with *circle.svg*. That means *circle.svg* is closer to the user than *rect.svg*.

With the property [mask-composite](#propdef-mask-composite) the author may choose different ways to combine multiple mask layers.

-
[add](#valdef-mask-composite-add) paints the *circle.svg* on top of *rect.svg*. The behavior is described by the compositing operator [source over](compositing-1.md#porterduffcompositingoperators_srcover).
```
mask-composite: add;
```

[image: Example of source-over compositing of mask layers]
-
[subtract](#valdef-mask-composite-subtract) paints portions of *circle.svg* that do not overlap *rect.svg*. The behavior is described by the compositing operator [source out](compositing-1.md#porterduffcompositingoperators_srcout).
```
mask-composite: subtract;
```

[image: Example of source-over compositing of mask layers]
-
[intersect](#valdef-mask-composite-intersect) paints portions of *circle.svg* that overlap *rect.svg*. The behavior is described by the compositing operator [source in](compositing-1.md#porterduffcompositingoperators_srcin).
```
mask-composite: intersect;
```

[image: Example of source-over compositing of mask layers]
-
[exclude](#valdef-mask-composite-exclude) paints portions of *circle.svg* and *rect.svg* that do not overlap. The behavior is described by the compositing operator [XOR](compositing-1.md#porterduffcompositingoperators_xor).
```
mask-composite: exclude;
```

[image: Example of source-over compositing of mask layers]

<a id="example-9bff19b4"></a> The following example specifies two mask layers and two compositing operators.

```
mask-image: rect.svg, circle.svg;
mask-composite: add, exclude;
```

*rect.svg* and *circle.svg* make use of the [add](#valdef-mask-composite-add) compositing operator. There is no further mask layer to use [exclude](#valdef-mask-composite-exclude) and therefore, exclude is ignored.

<a id="example-2266eac2"></a> This is an example of 3 mask layers with different compositing operators.

```
mask-image: trapeze.svg, circle.svg, rect.svg;
mask-composite: subtract, add;
```

First, *circle.svg* is “added” to *rect.svg*. In a second step, only portions of *trapeze.svg* that are not overlapping the compositing result of the previous two layers is visible.

[image: Example of source-over compositing of mask layers]

See the section [“Layering multiple mask layer images”](#layering) for how [mask-composite](#propdef-mask-composite) interacts with other comma-separated mask properties to form each mask layer.

### <a id="the-mask"></a>7.9. Mask Shorthand: the [mask](#propdef-mask) property

| Name: | *<a id="propdef-mask"></a>mask* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<mask-layer>](#typedef-mask-layer)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | see individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | see individual properties |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |
| Media: | visual |

```
<a id="typedef-mask-layer"></a><mask-layer> =
  <mask-reference> ||
  <position> [ / <bg-size> ]? ||
  <repeat-style> ||
  <geometry-box> ||
  [ <geometry-box> | no-clip ] ||
  <compositing-operator> ||
  <masking-mode>
```

If one [<geometry-box>](#typedef-geometry-box) value and the [no-clip](#valdef-mask-clip-no-clip) keyword are present then <geometry-box> sets [mask-origin](#propdef-mask-origin) and no-clip sets [mask-clip](#propdef-mask-clip) to that value.

If one [<geometry-box>](#typedef-geometry-box) value and no [no-clip](#valdef-mask-clip-no-clip) keyword are present then <geometry-box> sets both [mask-origin](#propdef-mask-origin) and [mask-clip](#propdef-mask-clip) to that value.

If two [<geometry-box>](#typedef-geometry-box) values are present, then the first sets [mask-origin](#propdef-mask-origin) and the second [mask-clip](#propdef-mask-clip).

The [used value](css-cascade-5.md#used-value) of the properties [mask-repeat](#propdef-mask-repeat), [mask-position](#propdef-mask-position), [mask-clip](#propdef-mask-clip), [mask-origin](#propdef-mask-origin) and [mask-size](#propdef-mask-size) must have no effect if [<mask-reference>](#typedef-mask-reference) references a [mask](#elementdef-mask) element. In this case the element defines position, sizing and clipping of the [mask layer image](#mask-layer-image).

The [mask](#propdef-mask) shorthand also resets [mask-border](#propdef-mask-border) to its initial value. It is therefore recommended that authors use the mask shorthand, rather than other shorthands or the individual properties, to override any mask settings earlier in the cascade. This will ensure that mask-border has also been reset to allow the new styles to take effect.

### <a id="the-mask-image-rendering-model"></a>7.10. The Mask Image Rendering Model

The application of the [mask-image](#propdef-mask-image) property with a value other than none to an element formatted with the CSS box model establishes a stacking context in the same way that CSS [opacity](css-color-4.md#propdef-opacity) [[CSS3COLOR]](#biblio-css3color) does, and all the element’s descendants are rendered together as a group with the masking applied to the group as a whole.

The [mask-image](#propdef-mask-image) property has no effect on the geometry or hit-testing of any element’s CSS boxes.

The [mask](#propdef-mask) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

#### <a id="MaskValues"></a>7.10.1. Mask processing

In the following section, *<a id="mask-image"></a>mask image* refers either to a [mask layer image](#mask-layer-image) or to a [mask border image](#mask-border-image).

A mask image may be interpreted using one of two different methods with regards to calculating the mask values that will be multiplied with the target alpha values.

The first and simplest method of calculating the mask values is to use the alpha channel of the [mask image](#mask-image). In this case the mask value at a given point is simply the value of the alpha channel at that point. The color channels do not contribute to the mask value.

The second method of calculating the mask values is to use the luminance of the mask image. In this case the mask value at a given point is computed from the color channel values and alpha channel value using the following procedure.

1.
Compute a luminance value from the color channel values.
  -
  If the computed value of [color-interpolation](svg2/painting.md#ColorInterpolationProperty) on the [mask](#elementdef-mask) element is [linearRGB](filter-effects-1.md#valdef-color-interpolation-filters-linearrgb), convert the original image color values (potentially in the sRGB color space) to the linearRGB color space.
  -
  Then, using non-premultiplied RGB color values, apply the luminance-to-alpha coefficients (as defined in the [feColorMatrix](filter-effects-1.md#elementdef-fecolormatrix) filter primitive [[SVG11]](#biblio-svg11)) to convert the RGB color values to luminance values.
2.
Multiply the computed luminance value by the corresponding alpha value to produce the mask value.

Regardless of the method used, the procedure for calculating mask values assumes the content of the mask is a four-channel RGBA graphics object. For other types of graphics objects, special handling is required as follows.

For a three-channel RGB graphics object that is used in a mask (e.g., when referencing a three-channel image file), the effect is as if the object were converted into a four-channel RGBA image with the alpha channel uniformly set to 1.

For a single-channel image that is used in a mask (e.g., when referencing a single-channel grayscale image file), the effect is as if the object were converted into a four-channel RGBA image, where the single channel from the referenced object is used to compute the three color channels and the alpha channel is uniformly set to 1.

Note: When referencing a grayscale image file, the transfer curve relating the encoded grayscale values to linear light values must be taken into account when computing the color channels.

Note: SVG [graphics elements](svg2/struct.md#graphics-element) (e.g., [circle](svg2/shapes.md#elementdef-circle) or [text](svg2/text.md#elementdef-text)) are all treated as four-channel RGBA images for the purposes of masking operations.

The effect of a mask is identical to what would have happened if there were no mask but instead the alpha channel of the given object were multiplied with the mask’s resulting mask values.

Regions not covered by a [mask image](#mask-image) are treated as transparent black. The mask value is 0.

Note: Masks with repeating [mask image](#mask-image) tiles may have an offset to each other. The space between the mask images is treated as a transparent black mask.

#### <a id="layering"></a>7.10.2. Layering Multiple Mask Images

The mask of a box can have multiple layers. The number of layers is determined by the number of comma-separated values for the [mask-image](#propdef-mask-image) property. A value of none in a list of values with other [<mask-reference>](#typedef-mask-reference)s still creates a layer.

See Layering Multiple Background Images [[CSS3BG]](#biblio-css3bg).

[mask-mode](#propdef-mask-mode) and [mask-composite](#propdef-mask-composite) do not have counterparts in CSS Backgrounds and Borders [[CSS3BG]](#biblio-css3bg). Just like for the mask properties that do have a counterpart, the list of values are matched up from the first value: excess values at the end are not used. If a property doesn’t have enough comma-separated values to match the number of layers, the UA must calculate its used value by repeating the list of values until there are enough.

All [mask layer images](#mask-layer-image) are transformed to alpha masks (if necessary see [Mask processing](#MaskValues)) and combined by compositing taking the compositing operators specified by [mask-composite](#propdef-mask-composite) into account.

## <a id="mask-borders"></a>8. Border-Box Mask

With [mask-border](#propdef-mask-border) an image can be split into nine pieces: four corners, four edges and the middle piece as demonstrated in the figure below.

[image: pieces of a mask border image]

Pieces of a [mask border image](#mask-border-image).

These pieces may be sliced, scaled and stretched in various ways to fit the size of the [mask border image area](#mask-border-image-area). This distorted image is then used as a mask. The syntax of [mask-border](#propdef-mask-border) corresponds to the [border-image](css-backgrounds-3.md#propdef-border-image) property of CSS Background and Borders [[CSS3BG]](#biblio-css3bg).

<a id="example-f8023813"></a> The [mask border image](#mask-border-image) in the following example is split into four corners with dimensions of 75 pixels, four edges and the middle piece that is stretched and scaled.

[image: Example for 'mask-border']

Example for [mask-border](#propdef-mask-border). The object on the left is the object to mask. The second image is the alpha mask and the last image the masked object.

```
div {
    background: linear-gradient(bottom, #F27BAA 0%, #FCC8AD 100%);
    mask-border-slice: 25 fill;
    mask-border-repeat: stretch;
    mask-border-source: url(mask.png);
}
```

### <a id="the-mask-border-source"></a>8.1. Mask Border Image Source: the [mask-border-source](#propdef-mask-border-source) property

| Name: | *<a id="propdef-mask-border-source"></a>mask-border-source* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'border-image-source'>](css-backgrounds-3.md#propdef-border-image-source) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | they keyword none or the computed [<image>](css-images-3.md#typedef-image) |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

Specifies an image to be used as *<a id="mask-border-image"></a>mask border image*.

An image that is an empty image (zero width or zero height), that fails to download, is non-existent, or that cannot be displayed (e.g. because it is not in a supported image format) is ignored. It still counts as an [mask border image](#mask-border-image) but does not mask the element.

See “[Mask processing](#MaskValues)” on how to process the [mask border image](#mask-border-image).

A computed value of other than none results in the creation of a stacking context [[CSS21]](#biblio-css21) the same way that CSS [opacity](css-color-4.md#propdef-opacity) [[CSS3COLOR]](#biblio-css3color) does for values other than 1.

[mask-border-source](#propdef-mask-border-source) and [mask-image](#propdef-mask-image) can be specified independent of each other. If both properties have a value other than none, the element is masked by both masking operations one after the other.

Note: It does not matter if [mask-image](#propdef-mask-image) is applied to the element before or after [mask-border-source](#propdef-mask-border-source). Both operation orders result in the same rendering.

### <a id="the-mask-border-mode"></a>8.2. Mask Border Image Interpretation: the [mask-border-mode](#propdef-mask-border-mode) property

| Name: | *<a id="propdef-mask-border-mode"></a>mask-border-mode* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | luminance [\|](css-values-4.md#comb-one) alpha |
| [Initial:](css-cascade-5.md#initial-values) | alpha |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The [mask-border-mode](#propdef-mask-border-mode) property indicates whether the [<image>](css-images-3.md#typedef-image) value for [mask-border-source](#propdef-mask-border-source) is treated as luminance mask or alpha mask. (See [Mask processing](#MaskValues).)

Values have the following meanings:

***<a id="valdef-mask-border-mode-alpha"></a>alpha* **
:

A value of [alpha](#valdef-mask-border-mode-alpha) indicates that the alpha values of the [mask border image](#mask-border-image) should be used as the mask values. See [Calculating mask values](#MaskValues).

***<a id="valdef-mask-border-mode-luminance"></a>luminance* **
:

A value of [luminance](#valdef-mask-border-mode-luminance) indicates that the luminance values of the [mask border image](#mask-border-image) should be used as the mask values. See [Calculating mask values](#MaskValues).

The [mask-mode](#propdef-mask-mode) and [mask-type](#propdef-mask-type) properties must have no affect on the [mask border image](#mask-border-image) type.

### <a id="the-mask-border-slice"></a>8.3. Mask Border Image Slicing: the [mask-border-slice](#propdef-mask-border-slice) property

| Name: | *<a id="propdef-mask-border-slice"></a>mask-border-slice* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'border-image-slice'>](css-backgrounds-3.md#propdef-border-image-slice) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to size of the [mask border image](#mask-border-image) |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

This property specifies inward offsets from the top, right, bottom, and left edges of the [mask border image](#mask-border-image), dividing it into nine regions: four corners, four edges and a middle. The middle image part is discarded and treated as fully opaque white (the content covered by the middle part is not masked and shines through) unless the *<a id="valdef-mask-border-slice-fill"></a>fill* keyword is present.

See the [border-image-slice](css-backgrounds-3.md#propdef-border-image-slice) property [[CSS3BG]](#biblio-css3bg) for the definitions of the property values.

### <a id="the-mask-border-width"></a>8.4. Masking Areas: the [mask-border-width](#propdef-mask-border-width) property

| Name: | *<a id="propdef-mask-border-width"></a>mask-border-width* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'border-image-width'>](css-backgrounds-3.md#propdef-border-image-width) |
| [Initial:](css-cascade-5.md#initial-values) | auto |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | relative to width/height of the [mask border image area](#mask-border-image-area) |
| [Computed value:](css-cascade-5.md#computed) | all [<length>](css-values-4.md#length-value)s made absolute, otherwise as specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

The [mask border image](#mask-border-image) is drawn inside an area called the *<a id="mask-border-image-area"></a>mask border image area*. This is an area whose boundaries by default correspond to the border box, see [mask-border-outset](#propdef-mask-border-outset).

See the [border-image-width](css-backgrounds-3.md#propdef-border-image-width) property [[CSS3BG]](#biblio-css3bg) for the definitions of the property values.

Note: For SVG elements without an associated layout box the [border-width](css-backgrounds-3.md#propdef-border-width) is considered to be 0.

### <a id="the-mask-border-outset"></a>8.5. Edge Overhang: the [mask-border-outset](#propdef-mask-border-outset) property

| Name: | *<a id="propdef-mask-border-outset"></a>mask-border-outset* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'border-image-outset'>](css-backgrounds-3.md#propdef-border-image-outset) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | all [<length>](css-values-4.md#length-value)s made absolute, otherwise as specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

The values specify the amount by which the [mask border image area](#mask-border-image-area) extends beyond the border box. If it has four values, they set the outsets on the top, right, bottom and left sides in that order. If the left is missing, it is the same as the right; if the bottom is missing, it is the same as the top; if the right is missing, it is the same as the top.

As with [mask-border-width](#propdef-mask-border-width), a [<number>](css-values-4.md#number-value) represents a multiple of the corresponding [border-width](css-backgrounds-3.md#propdef-border-width). Negative values are not allowed for any of the [mask-border-outset](#propdef-mask-border-outset) values.

Note: For SVG elements without associated layout box the [border-width](css-backgrounds-3.md#propdef-border-width) is considered to be 0.

### <a id="the-mask-border-repeat"></a>8.6. Mask Border Image Tiling: the [mask-border-repeat](#propdef-mask-border-repeat) property

| Name: | *<a id="propdef-mask-border-repeat"></a>mask-border-repeat* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'border-image-repeat'>](css-backgrounds-3.md#propdef-border-image-repeat) |
| [Initial:](css-cascade-5.md#initial-values) | stretch |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) excluding the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

This property specifies how the images for the sides and the middle part of the [mask border image](#mask-border-image) are scaled and tiled. The first keyword applies to the horizontal sides, the second to the vertical ones. If the second keyword is absent, it is assumed to be the same as the first.

See the [border-image-repeat](css-backgrounds-3.md#propdef-border-image-repeat) property [[CSS3BG]](#biblio-css3bg) for the definitions of the property values.

The exact process for scaling and tiling the [mask border image](#mask-border-image) parts is given in the section [Masking with the mask border image](#masking-with-the-mask-border-image)

### <a id="the-mask-border"></a>8.7. Mask Border Image Shorthand: the [mask-border](#propdef-mask-border) property

| Name: | *<a id="propdef-mask-border"></a>mask-border* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'mask-border-source'>](#propdef-mask-border-source) [\|\|](css-values-4.md#comb-any) [<'mask-border-slice'>](#propdef-mask-border-slice) [ / [<'mask-border-width'>](#propdef-mask-border-width)[?](css-values-4.md#mult-opt) [ / [<'mask-border-outset'>](#propdef-mask-border-outset) ]? ]? \|\| [<'mask-border-repeat'>](#propdef-mask-border-repeat) \|\| [<'mask-border-mode'>](#propdef-mask-border-mode) |
| [Initial:](css-cascade-5.md#initial-values) | See individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | See individual properties |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | See individual properties |
| Canonical order: | per grammar |
| Animation type: | See individual properties |
| Media: | visual |

This is a shorthand property for setting [mask-border-source](#propdef-mask-border-source), [mask-border-slice](#propdef-mask-border-slice), [mask-border-width](#propdef-mask-border-width), [mask-border-outset](#propdef-mask-border-outset), [mask-border-repeat](#propdef-mask-border-repeat) and [mask-border-mode](#propdef-mask-border-mode). Omitted values are set to their initial values.

Note: The [mask](#propdef-mask) shorthand resets the properties [mask-border](#propdef-mask-border), [mask-border-source](#propdef-mask-border-source), [mask-border-slice](#propdef-mask-border-slice), [mask-border-width](#propdef-mask-border-width), [mask-border-outset](#propdef-mask-border-outset), [mask-border-repeat](#propdef-mask-border-repeat) and [mask-border-mode](#propdef-mask-border-mode).

### <a id="masking-with-the-mask-border-image"></a>8.8. Masking with the mask border image

After the [mask border image](#mask-border-image) given by [mask-border-source](#propdef-mask-border-source) is sliced by the [mask-border-slice](#propdef-mask-border-slice) values, the resulting nine images are scaled, positioned, and tiled into their corresponding mask border image regions in four steps as described in the section Drawing the Border Image [[CSS3BG]](#biblio-css3bg).

The application of the [mask-border-source](#propdef-mask-border-source) property to an element formatted with the CSS box model establishes a stacking context in the same way that CSS [opacity](css-color-4.md#propdef-opacity) [[CSS3COLOR]](#biblio-css3color) does, and all the element’s descendants are rendered together as a group with the masking applied to the group as a whole.

The [mask-border-source](#propdef-mask-border-source) property has no effect on the geometry or hit-testing of any element’s CSS boxes.

## <a id="svg-masks"></a>9. SVG Mask Sources

### <a id="MaskElement"></a>9.1. The [mask](#elementdef-mask) element

| Name: | *<a id="elementdef-mask"></a>`mask`* |
| --- | --- |
| Categories: | [container elements](svg2/struct.md#container-element), [never-rendered element](svg2/render.md#TermNeverRenderedElement) |
| Content model: | Any number of the following elements, in any order: - [animation](svg11/intro.md#TermAnimationElement) — animate, animateMotion, animateTransform, set - [descriptive](svg11/intro.md#TermDescriptiveElement) — [desc](svg2/struct.md#elementdef-desc), [title](svg2/struct.md#elementdef-title), [metadata](svg2/struct.md#elementdef-metadata) - [shape](svg11/intro.md#TermShapeElement) — [circle](svg2/shapes.md#elementdef-circle), [ellipse](svg2/shapes.md#elementdef-ellipse), [line](svg2/shapes.md#elementdef-line), [path](svg2/paths.md#elementdef-path), [polygon](svg2/shapes.md#elementdef-polygon), [polyline](svg2/shapes.md#elementdef-polyline), [rect](svg2/shapes.md#elementdef-rect) - [structural](svg11/intro.md#TermStructuralElement) — [defs](svg2/struct.md#elementdef-defs), [g](svg2/struct.md#elementdef-g), [svg](svg2/struct.md#elementdef-svg), [symbol](svg2/struct.md#elementdef-symbol), [use](svg2/struct.md#elementdef-use) - [gradient](svg11/intro.md#TermGradientElement) — [linearGradient](svg2/pservers.md#elementdef-linearGradient), [radialGradient](svg2/pservers.md#elementdef-radialGradient) - [a](svg2/linking.md#elementdef-a) - [clipPath](#elementdef-clippath) - [color-profile](svg11/color.md#ColorProfileElement) - [cursor](svg11/interact.md#CursorElement) - [filter](filter-effects-1.md#elementdef-filter) - [font](svg11/fonts.md#FontElement) - [font-face](svg11/fonts.md#FontFaceElement) - [foreignObject](svg2/embedded.md#elementdef-foreignObject) - [image](svg2/embedded.md#elementdef-image) - [marker](svg2/painting.md#elementdef-marker) - [mask](#elementdef-mask) - [pattern](svg2/pservers.md#elementdef-pattern) - [script](svg2/interact.md#elementdef-script) - [style](svg2/styling.md#elementdef-style) - [switch](svg2/struct.md#elementdef-switch) - [view](svg2/linking.md#elementdef-view) - [text](svg2/text.md#elementdef-text) - [altGlyphDef](svg11/text.md#AltGlyphDefElement) |
| Attributes: | - [conditional processing attributes](svg11/intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](svg11/struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](svg11/struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](svg11/struct.md#SystemLanguageAttribute) - [core attributes](svg11/intro.md#TermCoreAttributes) — [‘id’](svg11/struct.md#IDAttribute), [‘xml:base’](svg11/struct.md#XMLBaseAttribute), [‘xml:lang’](svg11/struct.md#XMLLangAttribute), [‘xml:space’](svg11/struct.md#XMLSpaceAttribute) - [presentation attributes](svg11/intro.md#TermPresentationAttribute) — alignment-baseline, baseline-shift, [clip](#propdef-clip), [clip-path](#propdef-clip-path), [clip-rule](#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](filter-effects-1.md#propdef-color-interpolation-filters), [color-profile](svg11/color.md#ColorProfileProperty), color-rendering, cursor, direction, display, dominant-baseline, [enable-background](svg11/filters.md#EnableBackgroundProperty), [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](filter-effects-1.md#propdef-filter), [flood-color](filter-effects-1.md#propdef-flood-color), [flood-opacity](filter-effects-1.md#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, [glyph-orientation-horizontal](svg11/text.md#GlyphOrientationHorizontalProperty), glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), [kerning](svg11/text.md#KerningProperty), letter-spacing, [lighting-color](filter-effects-1.md#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [‘class’](svg11/styling.md#ClassAttribute) - [‘style’](svg11/styling.md#StyleAttribute) - ‘[x](#element-attrdef-mask-x)’ - ‘[y](#element-attrdef-mask-y)’ - ‘[width](#element-attrdef-mask-width)’ - ‘[height](#element-attrdef-mask-height)’ - ‘[maskUnits](#element-attrdef-mask-maskunits)’ - ‘[maskContentUnits](#element-attrdef-mask-maskcontentunits)’ |
| DOM Interfaces: | [SVGMaskElement](#InterfaceSVGMaskElement) |

*Attribute definitions:*

***<a id="element-attrdef-mask-maskunits"></a>`maskUnits`* = "[userSpaceOnUse](#valdef-maskunits-userspaceonuse) | [objectBoundingBox](#valdef-maskunits-objectboundingbox)" **
:

Defines the coordinate system for attributes [x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y), [width](#element-attrdef-mask-width) and [height](#element-attrdef-mask-height).

***<a id="valdef-maskunits-userspaceonuse"></a>userSpaceOnUse* **
:

[x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y), [width](#element-attrdef-mask-width) and [height](#element-attrdef-mask-height) represent values in the current [user coordinate system](css-transforms-1.md#user-coordinate-system) [[CSS3-TRANSFORMS]](#biblio-css3-transforms) in place at the time when the [mask](#elementdef-mask) element is referenced (i.e., the user coordinate system for the element referencing the mask element via the [mask](#propdef-mask) property).

***<a id="valdef-maskunits-objectboundingbox"></a>objectBoundingBox* **
:

[x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y), [width](#element-attrdef-mask-width) and [height](#element-attrdef-mask-height) represent fractions or percentages of the [object bounding box](svg2/coords.md#TermObjectBoundingBox) of the element to which the mask is applied. [User coordinates](svg2/coords.md#Units) are sized equivalently to the CSS [px](css-values-4.md#px) unit.

If attribute [maskUnits](#element-attrdef-mask-maskunits) is not specified, then the effect is as if a value of [objectBoundingBox](#valdef-maskunits-objectboundingbox) were specified.

Animatable: yes.

***<a id="element-attrdef-mask-maskcontentunits"></a>`maskContentUnits`* = "[userSpaceOnUse](#valdef-maskcontentunits-userspaceonuse) | [objectBoundingBox](#valdef-maskcontentunits-objectboundingbox)" **
:

Defines the coordinate system for the contents of the [mask](#elementdef-mask).

***<a id="valdef-maskcontentunits-userspaceonuse"></a>userSpaceOnUse* **
:

The [user coordinate system](css-transforms-1.md#user-coordinate-system) for the contents of the [mask](#elementdef-mask) element is the current user coordinate system in place at the time when the mask element is referenced (i.e., the user coordinate system for the element referencing the mask element via the [mask](#propdef-mask) property).

***<a id="valdef-maskcontentunits-objectboundingbox"></a>objectBoundingBox* **
:

The coordinate system has its origin at the top left corner of the [bounding box](svg2/coords.md#bounding-box) of the element to which the clipping path applies to and the same width and height of this bounding box. [User coordinates](svg2/coords.md#Units) are sized equivalently to the CSS [px](css-values-4.md#px) unit.

If attribute [maskContentUnits](#element-attrdef-mask-maskcontentunits) is not specified, then the effect is as if a value of [userSpaceOnUse](#valdef-maskcontentunits-userspaceonuse) were specified.

Animatable: yes.

***<a id="element-attrdef-mask-x"></a>`x`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The x-axis coordinate of one corner of the rectangle for the largest possible offscreen buffer. If the attribute is not specified but at least one of the attributes [y](#element-attrdef-mask-y), [width](#element-attrdef-mask-width) or [height](#element-attrdef-mask-height) are specified, the effect is as if a value of -10% were specified.

Animatable: yes.

***<a id="element-attrdef-mask-y"></a>`y`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The y-axis coordinate of one corner of the rectangle for the largest possible offscreen buffer. If the attribute is not specified but at least one of the attributes [x](#element-attrdef-mask-x), [width](#element-attrdef-mask-width) or [height](#element-attrdef-mask-height) are specified, the effect is as if a value of -10% were specified.

Animatable: yes.

***<a id="element-attrdef-mask-width"></a>`width`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The width of the largest possible offscreen buffer. A negative value or a value of zero disables rendering of the element. If the attribute is not specified but at least one of the attributes [x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y) or [height](#element-attrdef-mask-height) are specified, the effect is as if a value of 120% were specified.

Animatable: yes.

***<a id="element-attrdef-mask-height"></a>`height`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The height of the largest possible offscreen buffer. A negative value or a value of zero disables rendering of the element. If the attribute is not specified but at least one of the attributes [x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y) or [width](#element-attrdef-mask-width) are specified, the effect is as if a value of 120% were specified.

Animatable: yes.

If at least one of the attributes [x](#element-attrdef-mask-x), [y](#element-attrdef-mask-y), [width](#element-attrdef-mask-width) or [height](#element-attrdef-mask-height) are specified, the given object and the rectangle defined by x, y, width and height establish a current clipping path. The rendered content of the mask must be clipped by this current clipping path.

CSS properties inherit into the [mask](#elementdef-mask) element from its ancestors; properties do *not* inherit from the element referencing the mask element.

[mask](#elementdef-mask) elements are never rendered directly; their only usage is as something that can be referenced using the [mask](#propdef-mask) property. The [opacity](css-color-4.md#propdef-opacity), [filter](filter-effects-1.md#propdef-filter) and display properties do not apply to the mask element; thus, mask elements are not directly rendered even if the display property is set to a value other than none, and mask elements are available for referencing even when the display property on the mask element or any of its ancestors is set to none.

### <a id="the-mask-type"></a>9.2. Mask Source Interpretation: the [mask-type](#propdef-mask-type) property

| Name: | *<a id="propdef-mask-type"></a>mask-type* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | luminance [\|](css-values-4.md#comb-one) alpha |
| [Initial:](css-cascade-5.md#initial-values) | luminance |
| [Applies to:](css-cascade-5.md#applies-to) | [mask](#elementdef-mask) elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The [mask-type](#propdef-mask-type) property defines whether the content of the [mask](#elementdef-mask) element is treated as as luminance mask or alpha mask, as described in [Calculating mask values](#MaskValues).

Values have the following meanings:

***<a id="valdef-mask-type-luminance"></a>luminance* **
:

Indicates that the luminance values of the mask should be used.

***<a id="valdef-mask-type-alpha"></a>alpha* **
:

Indicates that the alpha values of the mask should be used.

The [mask-type](#propdef-mask-type) property allows the author of the [mask](#elementdef-mask) element to specify the preferred masking mode. However, the author can override this preference by setting the [mask-mode](#propdef-mask-mode) value to something different than [match-source](#valdef-mask-mode-match-source) on the masked content.

<a id="example-943c513c"></a> In the following example the computed value of [mask-type](#propdef-mask-type) is [luminance](#valdef-mask-type-luminance) and the computed value of [mask-mode](#propdef-mask-mode) is [match-source](#valdef-mask-mode-match-source). The UA must follow the preferred masking mode defined on the [mask](#elementdef-mask) element.

```
<svg>
  <mask style="mask-type: luminance;" id="mask">
    ...
  </mask>
</svg>

<p style="mask-image: url(#mask); mask-mode: match-source;">
  This is the masked content.
</p>
```

In the next example the computed value of [mask-mode](#propdef-mask-mode) is [alpha](#valdef-mask-mode-alpha) and overrides the preference on the [mask](#elementdef-mask) element that is computed to [luminance](#valdef-mask-type-luminance). The [mask layer image](#mask-layer-image) is used as an alpha mask.

```
<svg>
 <mask style="mask-type: luminance;" id="mask2">
   ...
 </mask>
</svg>

<p style="mask-image: url(#mask2); mask-mode: alpha;">
 This is the masked content.
</p>
```

The [mask-type](#propdef-mask-type) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

## <a id="priv"></a>10. Privacy Considerations

It is important that the timing to the masking operations is independent of the source and destination pixel. Masking operations must be implemented in such a way that they always take the same amount of time regardless of the pixel values. If this rule is not followed, an attacker could infer information and mount a timing attack.

A timing attack is a method of obtaining information about content that is otherwise protected, based on studying the amount of time it takes for an operation to occur. If, for example, red pixels took longer to draw than green pixels, one might be able to reconstruct a rough image of the element being rendered, without ever having access to the content of the element.

While CSS capabilities like those defined in this module can be used to hide content from a site visitor, Web developers should not use these features to hide *sensitive* content from users or page scripts. Content that is hidden from a user’s display via CSS can still be accessed and read from page scripts or form submissions. Web developers should treat the capabilities in this spec (as with all CSS specs) as cosmetic changes only, and not imposing or defending a privacy boundary.

## <a id="sec"></a>11. Security Considerations

[<mask-source>](#typedef-mask-source)s and [<clip-source>](#typedef-clip-source)s have special requirements on fetching resources.

User agents must use the potentially CORS-enabled fetch method defined by the [[FETCH]](#biblio-fetch) specification for all [<mask-source>](#typedef-mask-source), [<clip-source>](#typedef-clip-source) and [<image>](css-images-3.md#typedef-image) values on the [mask-image](#propdef-mask-image), [mask-border-source](#propdef-mask-border-source) and [clip-path](#propdef-clip-path) properties. When fetching, user agents must use “Anonymous” mode, set the referrer source to the stylesheet’s URL and set the origin to the URL of the containing document. If this results in network errors, the effect is as if the value none had been specified.

## <a id="clip-property"></a>Appendix A: The deprecated [clip](#propdef-clip) property

| Name: | *<a id="propdef-clip"></a>clip* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<rect()>](#funcdef-clip-rect) [\|](css-values-4.md#comb-one) auto |
| [Initial:](css-cascade-5.md#initial-values) | auto |
| [Applies to:](css-cascade-5.md#applies-to) | Absolutely positioned elements. In SVG, it applies to [elements which establish a new viewport](svg2/coords.md#EstablishingANewSVGViewport), [pattern](svg2/pservers.md#elementdef-pattern) elements and [mask](#elementdef-mask) elements. |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

With this specification the [clip](#propdef-clip) property is deprecated. Authors are encouraged to use the [clip-path](#propdef-clip-path) property instead. UAs must support the clip property.

The [clip](#propdef-clip) property applies only to absolutely positioned elements. In SVG, it applies to [elements which establish a new viewport](svg2/coords.md#EstablishingANewSVGViewport), [pattern](svg2/pservers.md#elementdef-pattern) elements and [mask](#elementdef-mask) elements. Values have the following meanings:

**auto **
:

The element does not clip.

***<a id="funcdef-clip-rect"></a>rect()* = rect( [<top>](#typedef-clip-top), [<right>](#typedef-clip-right), [<bottom>](#typedef-clip-bottom), [<left>](#typedef-clip-left) ) **
:

[<top>](#typedef-clip-top) and [<bottom>](#typedef-clip-bottom) specify offsets from the top border edge of the box, and [<right>](#typedef-clip-right), and [<left>](#typedef-clip-left) specify offsets from the left border edge of the box. Authors should separate offset values with commas. User agents must support separation with commas, but may also support separation without commas (but not a combination), because a previous revision of this specification was ambiguous in this respect.

*<a id="typedef-clip-top"></a><top>*, *<a id="typedef-clip-right"></a><right>*, *<a id="typedef-clip-bottom"></a><bottom>*, and *<a id="typedef-clip-left"></a><left>* may either have a [<length>](css-values-4.md#length-value) value or auto. Negative lengths are permitted. The value auto means that a given edge of the clipping region will be the same as the edge of the element’s generated border box (i.e., auto means the same as 0 for [<top>](#typedef-clip-top) and [<left>](#typedef-clip-left), the same as the [used value](css-cascade-5.md#used-value) of the height plus the sum of vertical padding and border widths for [<bottom>](#typedef-clip-bottom), and the same as the used value of the width plus the sum of the horizontal padding and border widths for [<right>](#typedef-clip-right), such that four auto values result in the clipping region being the same as the element’s border box).

When coordinates are rounded to pixel coordinates, care should be taken that no pixels remain visible when [<left>](#typedef-clip-left) and [<right>](#typedef-clip-right) have the same value (or [<top>](#typedef-clip-top) and [<bottom>](#typedef-clip-bottom) have the same value), and conversely that no pixels within the element’s border box remain hidden when these values are auto.

The [clip](#propdef-clip) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

<a id="example-550001e5"></a> Example: The following two rules:

```
p#one { clip: rect(5px, 40px, 45px, 5px); }
p#two { clip: rect(5px, 55px, 45px, 5px); }
```

and assuming both Ps are 50 by 55 pixel, will create, respectively, the rectangular clipping regions delimited by the dashed lines in the following illustrations:

[image: Values for rect shape]

This diagram illustrates two block boxes, one next to the other, with rectangular clipping regions of different dimensions. (See long description.)

## <a id="compute-stroke-bounding-box"></a>Appendix B: Compute stroke bounding box

The algorithm to compute the *<a id="stroke-bounding-box"></a>stroke bounding box* is as follows, depending on the type of *element*:

**a [graphics element](svg2/struct.md#graphics-element) without [use](svg2/struct.md#elementdef-use) or [image](svg2/embedded.md#elementdef-image) **
**an [a](svg2/linking.md#elementdef-a) element with a [text content element](svg2/text.md#TermTextContentElement) **
:

1.
Let *box* be a rectangle initialized to the [object bounding box](svg2/coords.md#TermObjectBoundingBox) of *element*.
2.
If the [used value](css-cascade-5.md#used-value) of [stroke-width](svg2/painting.md#StrokeWidthProperty) <= 0 or the used value of [stroke](svg2/painting.md#StrokeProperty) is none return *box*.
3.
Let *delta* be the inflation value initialized to the half of the [stroke-width](svg2/painting.md#StrokeWidthProperty).
4.
If *element* is not [rect](svg2/shapes.md#elementdef-rect), [ellipse](svg2/shapes.md#elementdef-ellipse), [circle](svg2/shapes.md#elementdef-circle) or [image](svg2/embedded.md#elementdef-image) just follow one of the following conditions in the order they apply:
**the used value for [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty) is miter **
:
  1.
  Let *miter* be the used value of [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty).
  2.
  If *miter* is smaller than the square root of 2 and if the used value for [stroke-linecap](svg2/painting.md#StrokeLinecapProperty) is square, multiply *delta* with the square root of 2. Otherwise, multiply *delta* with *miter*.
**the used value for [stroke-linecap](svg2/painting.md#StrokeLinecapProperty) is square **
:
  1.
  Multiply *delta* with the square root of 2.
5.
Inflate *box* with the value of *delta*.
6.
Return *box*.

Note: The values of the [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty) and [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty) do not affect the calculation of the stroke bounding box.

**a [container element](svg2/struct.md#container-element) **
**[use](svg2/struct.md#elementdef-use) **
:

1.
Let *parent* be the [container element](svg2/struct.md#container-element) if it is one, or the root of the [use](svg2/struct.md#elementdef-use) element’s shadow tree otherwise.
2.
For each child *child* of *parent*
  1.
  Invoke the stroke bounding box algorithm with *child*.
  2.
  Let *childBox* be the returned box value of the invoked algorithm.
  3.
  Map *childBox* from the coordinate space of *child* to the coordinate space of *parent*.
3.
Let *box* be the union of all *childBox*es.
4.
Return *box*.

**[image](svg2/embedded.md#elementdef-image) **
:

1.
Return the [object bounding box](svg2/coords.md#TermObjectBoundingBox) of *element*.

Note: A future version of the SVG specification may override this section.

## <a id="DOMInterfaces"></a>Appendix C: DOM interfaces

### <a id="InterfaceSVGClipPathElement"></a>Interface SVGClipPathElement

The *<a id="svgclippathelement"></a>`SVGClipPathElement`* interface corresponds to the [clipPath](#elementdef-clippath) element.

```
[Exposed=Window]
interface SVGClipPathElement : SVGElement {
  readonly attribute SVGAnimatedEnumeration clipPathUnits;
  readonly attribute SVGAnimatedTransformList transform;
};
```

**Attributes: **
:

***<a id="dom-svgclippathelement-clippathunits"></a>`clipPathUnits`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [clipPathUnits](#element-attrdef-clippath-clippathunits) on the given [clipPath](#elementdef-clippath) element. Takes one of the constants defined in [SVGUnitTypes](svg2/types.md#svgunittypes).

***<a id="dom-svgclippathelement-transform"></a>`transform`*, of type [SVGAnimatedTransformList](svg2/coords.md#svganimatedtransformlist), readonly **
:

Corresponds to presentation attribute [transform](css-transforms-1.md#propdef-transform) on the given element.

### <a id="InterfaceSVGMaskElement"></a>Interface SVGMaskElement

The *<a id="svgmaskelement"></a>`SVGMaskElement`* interface corresponds to the [mask](#elementdef-mask) element.

```
[Exposed=Window]
interface SVGMaskElement : SVGElement {
  readonly attribute SVGAnimatedEnumeration maskUnits;
  readonly attribute SVGAnimatedEnumeration maskContentUnits;
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
};
```

**Attributes: **
:

***<a id="dom-svgmaskelement-maskunits"></a>`maskUnits`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [maskUnits](#element-attrdef-mask-maskunits) on the given [mask](#elementdef-mask) element. Takes one of the constants defined in [SVGUnitTypes](svg2/types.md#svgunittypes).

***<a id="dom-svgmaskelement-maskcontentunits"></a>`maskContentUnits`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [maskContentUnits](#element-attrdef-mask-maskcontentunits) on the given [mask](#elementdef-mask) element. Takes one of the constants defined in [SVGUnitTypes](svg2/types.md#svgunittypes).

***<a id="dom-svgmaskelement-x"></a>`x`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [x](#element-attrdef-mask-x) on the given [mask](#elementdef-mask) element.

***<a id="dom-svgmaskelement-y"></a>`y`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [y](#element-attrdef-mask-y) on the given [mask](#elementdef-mask) element.

***<a id="dom-svgmaskelement-width"></a>`width`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [width](#element-attrdef-mask-width) on the given [mask](#elementdef-mask) element.

***<a id="dom-svgmaskelement-height"></a>`height`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [height](#element-attrdef-mask-height) on the given [mask](#elementdef-mask) element.

## <a id="changes"></a>Changes since last publication

The following changes were made since the 5 August 2021 Candidate Recommendation Draft.

-
Added a privacy concern regarding hiding sensitive content
-
Separated Security and Privacy sections
-
Minor editorial and markup changes
-
Added Web Platform Tests coverage

The following changes were made since the 26 August 2014 Candidate Recommendation.

-
Allowed the [<'mask-mode'>](#propdef-mask-mode) value in the [mask](#propdef-mask) shorthand to appear anywhere other than between [<'mask-position'>](#propdef-mask-position) and [<'mask-size'>](#propdef-mask-size).
-
Removed Implements SVGUnitTypes on clipPath and mask elements.
-
Apply properties that apply to all graphics elements to the use element as well.
-
Change initial value of [mask-position](#propdef-mask-position) to 0% 0% and of [mask-repeat](#propdef-mask-repeat) to repeat.
-
Remove *margin-box* as possible value from [mask-origin](#propdef-mask-origin) and [mask-clip](#propdef-mask-clip).
-
Clarify that [mask-clip](#propdef-mask-clip) has no affect on [mask layer image](#mask-layer-image)s that reference a [mask](#elementdef-mask) element.
-
Clarify [mask](#propdef-mask) shorthand behavior on appearance of one [<geometry-box>](#typedef-geometry-box) and the [no-clip](#valdef-mask-clip-no-clip) keyword.
-
[mask-mode](#propdef-mask-mode) on a [mask layer image](#mask-layer-image) that is a reference to a [mask](#elementdef-mask) element with a value of [match-source](#valdef-mask-mode-match-source) should take the value of the [mask-type](#propdef-mask-type) property on this mask element.
-
Mapping of boxes for SVG elements with and without associated CSS layout box changed to match mapping in Fill and Stroke spec.
-
Editorial changes.

The following changes were made since the 22 May 2014 Working Draft.

-
Change the inital value of [mask-size](#propdef-mask-size) from border-box to auto.

The following changes were made since the 13 February 2014 Working Draft.

-
Renamed mask-box* properties and terms to mask-border*.
-
Added support for multiple mask layers. (Similar to multiple background layers for [background](css-backgrounds-3.md#propdef-background).)
-
Added the [mask-composite](#propdef-mask-composite) property to control compositing of multiple [mask layer images](#mask-layer-image) with the keywords [add](#valdef-mask-composite-add), [subtract](#valdef-mask-composite-subtract), [intersect](#valdef-mask-composite-intersect) and [exclude](#valdef-mask-composite-exclude).
-
[mask-border-slice](#propdef-mask-border-slice) without keyword [fill](#valdef-mask-border-slice-fill) does not clip middle piece of content anymore. Changed initial value from 0 fill to 0.
-
Better description for [clip-rule](#propdef-clip-rule), [mask-mode](#propdef-mask-mode) and [mask-type](#propdef-mask-type).
-
Rename [fill](#valdef-mask-border-slice-fill) and stroke keywords to [fill-box](#valdef-clip-path-fill-box) and [stroke-box](#valdef-clip-path-stroke-box).
-
Added definition for [stroke bounding box](#stroke-bounding-box).
-
Better differentiation between mask images: [mask layer image](#mask-layer-image) and [mask border image](#mask-border-image).

The following changes were made since the 29 October 2013 Last Call Working Draft.

-
Remove note that a future version of the spec will allow controlling hit testing on clipping.
-
Changed order of sections within the document.
-
Make clear that the [used value](css-cascade-5.md#used-value) of the properties [mask-image](#propdef-mask-image), [mask-repeat](#propdef-mask-repeat), [mask-position](#propdef-mask-position), [mask-clip](#propdef-mask-clip), [mask-origin](#propdef-mask-origin) and [mask-size](#propdef-mask-size) must be ignored for [<mask-source>](#typedef-mask-source), not the properties.
-
"Animatable" section of [mask-size](#propdef-mask-size), [mask-position](#propdef-mask-position) described as repeatable list of lists. Changed to a single list.
-
Computed value of [mask-repeat](#propdef-mask-repeat) and [mask-position](#propdef-mask-position) was described as list of items. Changed to have just one value.
-
Change link to ED from https://drafts.csswg.org/masking/ to https://drafts.csswg.org/css-masking-1/

The following significant changes were made since the 20 June 2013 Working Draft.

-
[mask](#propdef-mask) resets mask-box properties.
-
Initial values for [mask-repeat](#propdef-mask-repeat), [mask-position](#propdef-mask-position) and [mask-origin](#propdef-mask-origin) changed to no-repeat, center and [border-box](#valdef-mask-origin-border-box).
-
Multiple layers of mask images were deferred to a future level of this specification.
-
Added security model for pixel operations and fetching of masking and clipping resources.
-
Deferred "child" and select() function to next level.

The following significant changes were made since the 15 November 2012 Working Draft.

-
Better integration with terms and definitions of CSS Backgrounds and Borders module.
-
Syntax changes on [mask](#propdef-mask) shorthand property to be conform with [background](css-backgrounds-3.md#propdef-background) shorthand property.
-
Define how the implementation can differ between an SVG resource ([mask](#elementdef-mask), [clipPath](#elementdef-clippath)) and an image resource.
-
Added [mask-mode](#propdef-mask-mode) property to alter between luminance and alpha mask on [mask-image](#propdef-mask-image).
-
Adapt IDL definition of SVGMaskElement and SVGClipPathElement to WebIDL.
-
Further editorial changes.

See detailed list of changes in the [ChangeLog](https://drafts.csswg.org/css-masking-1/ChangeLog).

## <a id="acknowledgments"></a>Acknowledgments

Thanks to Elika J. Etemad, Cameron McCormack, Liam R. E. Quin, Björn Höhrmann, Alan Stearns, Jarek Foksa, David Baron, Boris Zbarsky, Markus Stange and Sara Soueidan for their careful reviews, comments, and corrections. Special thanks to CJ Gammon for graphical assets.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-compositing-1"></a>[COMPOSITING-1] **
: Chris Harrelson. [Compositing and Blending Level 1](compositing-1.md). URL: [https://drafts.csswg.org/compositing-1/](compositing-1.md)
**<a id="biblio-css-box-4"></a>[CSS-BOX-4] **
: Elika Etemad. [CSS Box Model Module Level 4](css-box-4.md). URL: [https://drafts.csswg.org/css-box-4/](css-box-4.md)
**<a id="biblio-css-break-4"></a>[CSS-BREAK-4] **
: Rossen Atanassov; Elika Etemad. CSS Fragmentation Module Level 4. URL: https://drafts.csswg.org/css-break-4/
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-display-4"></a>[CSS-DISPLAY-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Display Module Level 4. URL: https://drafts.csswg.org/css-display-4/
**<a id="biblio-css-fonts-4"></a>[CSS-FONTS-4] **
: Chris Lilley. CSS Fonts Module Level 4. URL: https://drafts.csswg.org/css-fonts-4/
**<a id="biblio-css-images-3"></a>[CSS-IMAGES-3] **
: Tab Atkins Jr.; Elika Etemad; Lea Verou. [CSS Images Module Level 3](css-images-3.md). URL: [https://drafts.csswg.org/css-images-3/](css-images-3.md)
**<a id="biblio-css-inline-3"></a>[CSS-INLINE-3] **
: Elika Etemad. CSS Inline Layout Module Level 3. URL: https://drafts.csswg.org/css-inline-3/
**<a id="biblio-css-overflow-3"></a>[CSS-OVERFLOW-3] **
: Elika Etemad; Florian Rivoal. CSS Overflow Module Level 3. URL: https://drafts.csswg.org/css-overflow-3/
**<a id="biblio-css-shapes"></a>[CSS-SHAPES] **
: Alan Stearns; Rossen Atanassov; Noam Rosenthal. [CSS Shapes Module Level 1](css-shapes-1.md). URL: [https://drafts.csswg.org/css-shapes/](css-shapes-1.md)
**<a id="biblio-css-text-4"></a>[CSS-TEXT-4] **
: Elika Etemad; et al. CSS Text Module Level 4. URL: https://drafts.csswg.org/css-text-4/
**<a id="biblio-css-text-decor-4"></a>[CSS-TEXT-DECOR-4] **
: Elika Etemad; Koji Ishii. CSS Text Decoration Module Level 4. URL: https://drafts.csswg.org/css-text-decor-4/
**<a id="biblio-css-ui-4"></a>[CSS-UI-4] **
: Tab Atkins Jr.; Florian Rivoal. CSS Basic User Interface Module Level 4. URL: https://drafts.csswg.org/css-ui-4/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-values-5"></a>[CSS-VALUES-5] **
: Tab Atkins Jr.; Elika Etemad; Miriam Suzanne. CSS Values and Units Module Level 5. URL: https://drafts.csswg.org/css-values-5/
**<a id="biblio-css-writing-modes-3"></a>[CSS-WRITING-MODES-3] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 3. URL: https://drafts.csswg.org/css-writing-modes-3/
**<a id="biblio-css-writing-modes-4"></a>[CSS-WRITING-MODES-4] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 4. URL: https://drafts.csswg.org/css-writing-modes-4/
**<a id="biblio-css21"></a>[CSS21] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3-transforms"></a>[CSS3-TRANSFORMS] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css3bg"></a>[CSS3BG] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-css3val"></a>[CSS3VAL] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-fetch"></a>[FETCH] **
: Anne van Kesteren. Fetch Standard. Living Standard. URL: https://fetch.spec.whatwg.org/
**<a id="biblio-fill-stroke-3"></a>[FILL-STROKE-3] **
: Elika Etemad; Tab Atkins Jr.. CSS Fill and Stroke Module Level 3. URL: https://drafts.csswg.org/fill-stroke/
**<a id="biblio-filter-effects"></a>[FILTER-EFFECTS] **
: Dirk Schulze; Dean Jackson. [Filter Effects Module Level 1](filter-effects-1.md). URL: [https://drafts.csswg.org/filter-effects-1/](filter-effects-1.md)
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-svg-animations"></a>[SVG-ANIMATIONS] **
: SVG Animations Level 2. Editor's Draft. URL: https://svgwg.org/specs/animations/
**<a id="biblio-svg11"></a>[SVG11] **
: Erik Dahlström; et al. [Scalable Vector Graphics (SVG) 1.1 (Second Edition)](svg11/index.md). 16 August 2011. REC. URL: [https://www.w3.org/TR/SVG11/](svg11/index.md)
**<a id="biblio-svg2"></a>[SVG2] **
: Karl Dubost; et al. [Scalable Vector Graphics (SVG) 2](svg2/index.md). URL: [https://w3c.github.io/svgwg/svg2-draft/](svg2/index.md)
**<a id="biblio-webidl"></a>[WEBIDL] **
: Edgar Chen; Timothy Gu. Web IDL Standard. Living Standard. URL: https://webidl.spec.whatwg.org/
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css3color"></a>[CSS3COLOR] **
: Tantek Çelik; Chris Lilley; David Baron. CSS Color Module Level 3. URL: https://drafts.csswg.org/css-color-3/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order | Com­puted value |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [clip](#propdef-clip) | <rect()> \| auto | auto | Absolutely positioned elements. In SVG, it applies to elements which establish a new viewport, pattern elements and mask elements. | no | n/a | by computed value | per grammar | as specified | visual |
| [clip-path](#propdef-clip-path) | <clip-source> \| [ <basic-shape> \|\| <geometry-box> ] \| none | none | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | by computed value | per grammar | as specified, but with <url> values made absolute | visual |
| [clip-rule](#propdef-clip-rule) | nonzero \| evenodd | nonzero | Applies to SVG graphics elements | yes | n/a | discrete | per grammar | as specified | visual |
| [mask](#propdef-mask) | <mask-layer># | see individual properties | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | see individual properties | see individual properties | per grammar | see individual properties | visual |
| [mask-border](#propdef-mask-border) | <'mask-border-source'> \|\| <'mask-border-slice'> [ / <'mask-border-width'>? [ / <'mask-border-outset'> ]? ]? \|\| <'mask-border-repeat'> \|\| <'mask-border-mode'> | See individual properties | See individual properties | no | n/a | See individual properties | per grammar | See individual properties | visual |
| [mask-border-mode](#propdef-mask-border-mode) | luminance \| alpha | alpha | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | as specified | visual |
| [mask-border-outset](#propdef-mask-border-outset) | <'border-image-outset'> | 0 | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | by computed value | per grammar | all <length>s made absolute, otherwise as specified | visual |
| [mask-border-repeat](#propdef-mask-border-repeat) | <'border-image-repeat'> | stretch | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | as specified | visual |
| [mask-border-slice](#propdef-mask-border-slice) | <'border-image-slice'> | 0 | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | refer to size of the mask border image | by computed value | per grammar | as specified | visual |
| [mask-border-source](#propdef-mask-border-source) | <'border-image-source'> | none | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | they keyword none or the computed <image> | visual |
| [mask-border-width](#propdef-mask-border-width) | <'border-image-width'> | auto | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | relative to width/height of the mask border image area | by computed value | per grammar | all <length>s made absolute, otherwise as specified | visual |
| [mask-clip](#propdef-mask-clip) | [ <coord-box> \| no-clip ]# | border-box | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | list, each item the keyword as specified | visual |
| [mask-composite](#propdef-mask-composite) | <compositing-operator># | add | All elements. In SVG, it applies to container elements without the defs element and all graphics elements | no | n/a | discrete | per grammar | list, each item the keyword as specified | visual |
| [mask-image](#propdef-mask-image) | <mask-reference># | none | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | list, each item the keyword none, a computed <image>, or a computed <url> | visual |
| [mask-mode](#propdef-mask-mode) | <masking-mode># | match-source | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | list, each item the keyword as specified | visual |
| [mask-origin](#propdef-mask-origin) | <coord-box># | border-box | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | list, each item the keyword as specified | visual |
| [mask-position](#propdef-mask-position) | <position># | 0% 0% | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | refer to size of mask painting area minus size of mask layer image; see text background-position [CSS3BG] | repeatable list | per grammar | list, each item consists of two keywords representing the origin and two offsets from that origin, each given as an absolute length (if given a <length>), otherwise as a percentage. | visual |
| [mask-repeat](#propdef-mask-repeat) | <repeat-style># | repeat | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | discrete | per grammar | list, each item a pair of keywords, one per dimension | visual |
| [mask-size](#propdef-mask-size) | <bg-size># | auto | All elements. In SVG, it applies to container elements excluding the defs element, all graphics elements and the use element | no | n/a | repeatable list | per grammar | list, each item as specified, but with lengths made absolute | visual |
| [mask-type](#propdef-mask-type) | luminance \| alpha | luminance | mask elements | no | n/a | discrete | per grammar | as specified | visual |
