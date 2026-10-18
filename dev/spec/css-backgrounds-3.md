<!-- https://drafts.csswg.org/css-backgrounds-3/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Backgrounds and Borders Module Level 3

<a id="w3c-state"></a>Editor’s Draft, 16 December 2025

More details about this document

**This version: **
: https://drafts.csswg.org/css-backgrounds/
**Latest published version: **
: https://www.w3.org/TR/css-backgrounds-3/
**Previous Versions: **
: https://www.w3.org/TR/2024/CRD-css-backgrounds-3-20240311/
: https://www.w3.org/TR/2020/CR-css-backgrounds-3-20201222/
: https://www.w3.org/TR/2017/CR-css-backgrounds-3-20171017/
: https://www.w3.org/TR/2014/CR-css3-background-20140909/
**Implementation Report: **
: https://wpt.fyi/results/css/css-backgrounds
**Feedback: **
: CSSWG Issues Repository
**Editors: **
: Elika J. Etemad / fantasai (Apple)
: Brad Kemper (Invited Expert)
**Former Editor: **
: Bert Bos (W3C)
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/css-backgrounds/
Copyright © 2025 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

This draft contains the features of CSS relating to borders and backgrounds. The main extensions compared to level 2 are borders consisting of images, boxes with multiple backgrounds, boxes with rounded corners and boxes with shadows.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-backgrounds” in the title, like this: “[css-backgrounds] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

The following features are at-risk, and may be dropped during the CR period:

- animatability of [box-shadow](#propdef-box-shadow)
- applicability of [border](#propdef-border) and its longhands to ruby base containers and ruby annotation containers

“At-risk” is a W3C Process term-of-art, and does not necessarily imply that the feature is in danger of being dropped or delayed. It means that the WG believes the feature may have difficulty being interoperably implemented in a timely manner, and marking it as such allows the WG to drop the feature if necessary when transitioning to the Proposed Rec stage, without having to publish a new Candidate Rec without the feature first.

## <a id="introduction"></a>1. Introduction

*This subsection is not normative.*

When elements are rendered according to the CSS box model [[CSS-BOX-3]](#biblio-css-box-3), each element is either not displayed at all, or formatted as one or more rectangular boxes. Each box has a rectangular [content area](css-box-4.md#content-area), a band of [padding](css-box-4.md#padding) around the content, a [border](css-box-4.md#border) around the padding, and a [margin](css-box-4.md#margin) outside the border. (The margin may actually be negative, but margins have no influence on the background and border.)

[image: Diagram of a typical box, showing the content, padding, border and margin areas]

The various areas and edges of a typical box. (This diagram is explained in the CSS Box Model Module [[CSS-BOX-3]](#biblio-css-box-3).)

The properties of this module deal with the decoration of the [border area](css-box-4.md#border-area) and with the background of the [content](css-box-4.md#content-area), [padding](css-box-4.md#padding-area), and border areas. Additionally the box may be given a “drop-shadow” effect with the [box-shadow](#propdef-box-shadow) property.

If an element is broken into multiple box fragments, box-decoration-break [[CSS-BREAK-3]](#biblio-css-break-3) defines how the borders and background are divided over the various fragments. (An element can result in more than one fragment if it is broken at the end of a line, at the end of a column or at the end of a page; and continued in the next line, column or page.)

The relative stacking order of backgrounds, borders, and shadows is given in this module. For how these layers interact with other rendered content, see Appendix E “Elaborate description of Stacking Contexts” in [[CSS2]](#biblio-css2).

### <a id="placement"></a>1.1. Module Interactions

This module replaces and extends the background and border features defined in [[CSS2]](#biblio-css2) sections 8.5 and 14.2.

All properties in this module apply to the ::first-letter [pseudo-element](selectors-4.md#pseudo-element). The [background properties](#backgrounds) and [border-radius properties](#corners) also apply to the ::first-line pseudo-element. The UA may (but is not required to) apply the [border-image](#propdef-border-image) or [box-shadow](#propdef-box-shadow) properties to ::first-line. The UA must not apply the [border-color/style/width properties](#borders) to ::first-line. [[CSS2]](#biblio-css2)

### <a id="values"></a>1.2. Value Definitions

This specification follows the CSS property definition conventions from [[CSS2]](#biblio-css2) using the value definition syntax from [[CSS-VALUES-3]](#biblio-css-values-3). Value types not defined in this specification are defined in CSS Values & Units [[CSS-VALUES-3]](#biblio-css-values-3). Combination with other CSS modules may expand the definitions of these value types. For example, combining with [CSS Images](css-images-3.md) allows for using CSS gradients as [background-image](#propdef-background-image) or [border-image](#propdef-border-image) values. [[CSS-IMAGES-3]](#biblio-css-images-3)

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their property value. For readability they have not been repeated explicitly.

## <a id="backgrounds"></a>2. Backgrounds

Each box has a background layer that may be fully transparent (the default), or filled with a color and/or one or more images. The background properties specify what color ([background-color](#propdef-background-color)) and images ([background-image](#propdef-background-image)) to use, and how they are sized, positioned, tiled, etc.

The background properties are not inherited, but the parent box’s background will shine through by default because of the initial [transparent](css-color-4.md#valdef-color-transparent) value on [background-color](#propdef-background-color).

### <a id="layering"></a>2.1. Layering Multiple Background Images

The background of a box can have multiple *<a id="background-image-layer"></a>background image layers*. The number of layers is determined by the number of comma-separated values in the [background-image](#propdef-background-image) property. Note that a value of [none](#valdef-background-image-none) still creates a layer.

Each of the [background images](#background-images) is sized, positioned, and tiled according to the corresponding value in the other background properties. The lists are matched up from the first value: excess values at the end are not used. If a property doesn’t have enough comma-separated values to match the number of layers, the UA must calculate its [used value](css-cascade-5.md#used-value) by repeating the list of values until there are enough.

<a id="example-bef8b176"></a> For example, this set of declarations:

```
background-image: url(flower.png), url(ball.png), url(grass.png);
background-position: center center, 20% 80%, top left, bottom right;
background-origin: border-box, content-box;
background-repeat: no-repeat;
```

has exactly the same effect as this set, with the extra position dropped and the missing values for [background-origin](#propdef-background-origin) and [background-repeat](#propdef-background-repeat) filled in (emphasized for clarity):

```
background-image: url(flower.png), url(ball.png), url(grass.png);
background-position: center center, 20% 80%, top left;
background-origin: border-box, content-box, border-box;
background-repeat: no-repeat, no-repeat, no-repeat;
```

The first image in the list is the [layer](#background-image-layer) closest to the user, the next one is painted behind the first, and so on. The background color, if present, is painted below all of the other layers.

Note: The [border-image properties](#border-images) can also define a background image, which, if present, is painted on top of the background [layers](#background-image-layer) created by the background properties.

### <a id="background-color"></a>2.2. <a id="the-background-color"></a> Base Color: the [background-color](#propdef-background-color) property

| Name: | *<a id="propdef-background-color"></a>background-color* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <color> |
| [Initial:](css-cascade-5.md#initial-values) | transparent |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | computed color |
| Canonical order: | per grammar |
| Animation type: | by computed value |

This property sets the *<a id="background-color-layer"></a>background color* of a box. This color is drawn behind any background images.

<a id="example-ed26f60a"></a> Example:

```
h1 { background-color: #F00 } /* Sets background to red. */
```

The [background color](#background-color-layer) is clipped according to the [background-clip](#propdef-background-clip) value associated with the bottom-most [background image layer](#background-image-layer).

### <a id="background-image"></a>2.3. <a id="the-background-image"></a> Image Sources: the [background-image](#propdef-background-image) property

| Name: | *<a id="propdef-background-image"></a>background-image* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<bg-image>](#typedef-bg-image)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | list, each item either an [<image>](css-images-3.md#typedef-image) or the keyword [none](#valdef-background-image-none) |
| Canonical order: | per grammar |
| Animation type: | discrete |

This property specifies the *<a id="background-images"></a>background image(s)* of an element. Images are drawn with the first specified one on top (closest to the user) and each subsequent image behind the previous one. The property’s value is given as a comma-separated list of [<bg-image>](#typedef-bg-image) values where

```
<a id="typedef-bg-image"></a><bg-image> = <image> | none
```

A value of *<a id="valdef-background-image-none"></a>none* counts as a [background image layer](#background-image-layer) but draws nothing. An image that is empty (zero width or zero height), that fails to download, or that cannot be displayed (e.g., because it is not in a supported image format) likewise counts as a layer but draws nothing.

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-image](#propdef-background-image) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

When setting a background image, authors should also specify a [background-color](#propdef-background-color) that will preserve contrast with the text for when the image is unavailable.

For accessibility reasons, authors should not use background images as the sole method of conveying important information. See Web Content Accessibility Guideline F3 [[WCAG20]](#biblio-wcag20). Images are not accessible in non-graphical presentations, and background images specifically might be turned off in high-contrast display modes.

Note: Stylistic foreground images can be provided in CSS with the content property. Semantically-important foreground images should be provided in the document markup, e.g. with the <img> tag in HTML.

Note: Media fragments can be used to display a portion of an image. The [CSS Images](css-images-3.md) module will provide fallback syntax for image formats and include additional controls for image display.

<a id="example-b3c5ac2d"></a> Some examples specifying background images:

```
html { background-image: url("marble.svg") }
p { background-image: none }
div { background-image: url(tl.png), url(tr.png) }
main { background-image: radial-gradient(at bottom right, transparent, white); }
```

Implementations may optimize by not downloading and drawing images that are not visible (e.g., because they are behind other, fully opaque images).

### <a id="background-repeat"></a>2.4. <a id="the-background-repeat"></a> Tiling Images: the [background-repeat](#propdef-background-repeat) property

| Name: | *<a id="propdef-background-repeat"></a>background-repeat* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<repeat-style>](#typedef-repeat-style)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | repeat |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | list, each item a pair of keywords, one per dimension |
| Canonical order: | per grammar |
| Animation type: | discrete |

This property specifies how [background images](#background-images) are tiled after they have been [sized](#background-size) and [positioned](#background-position). The property’s value is given as a comma-separated list of [<repeat-style>](#typedef-repeat-style) values where

```
<a id="typedef-repeat-style"></a><repeat-style> = repeat-x | repeat-y | [repeat | space | round | no-repeat]{1,2}
```

Single values for [<repeat-style>](#typedef-repeat-style) have the following meanings:

***<a id="valdef-background-repeat-repeat-x"></a>repeat-x* **
: Computes to repeat no-repeat.
***<a id="valdef-background-repeat-repeat-y"></a>repeat-y* **
: Computes to no-repeat repeat.
**[repeat](#valdef-background-repeat-repeat) **
: Computes to repeat repeat.
**[space](#valdef-background-repeat-space) **
: Computes to space space
**[round](#valdef-background-repeat-round) **
: Computes to round round
**[no-repeat](#valdef-background-repeat-no-repeat) **
: Computes to no-repeat no-repeat
If a [<repeat-style>](#typedef-repeat-style) value has two keywords, the first one applies to the horizontal axis, the second to the vertical one, as follows:

***<a id="valdef-background-repeat-repeat"></a>repeat* **
: The [image](#background-images) is repeated in this direction as often as needed to cover the [background painting area](#background-painting-area).
***<a id="valdef-background-repeat-space"></a>space* **
: The [image](#background-images) is repeated as often as will fit within the [background positioning area](#background-positioning-area) without being clipped, and then the images are spaced out to fill the area. The first and last images touch the edges of the area. If the [background painting area](#background-painting-area) is larger than the background positioning area, then the pattern repeats to fill the background painting area. The value of [background-position](#propdef-background-position) for this direction is ignored unless there is not enough space for two copies of the image in this axis, in which case only one image is placed, and background-position determines its position in this axis.
***<a id="valdef-background-repeat-round"></a>round* **
: The [image](#background-images) is repeated as often as will fit within the [background positioning area](#background-positioning-area). If it doesn’t fit a whole number of times, it is rescaled so that it does. See the formula under [background-size](#propdef-background-size). If the [background painting area](#background-painting-area) is larger than the background positioning area, then the pattern repeats to fill the background painting area.
***<a id="valdef-background-repeat-no-repeat"></a>no-repeat* **
: The [image](#background-images) is placed once and not repeated in this direction.
Unless one of the two keywords is [no-repeat](#valdef-background-repeat-no-repeat), the whole [background painting area](#background-painting-area) will be tiled, i.e., not just one vertical strip and one horizontal strip.

<a id="example-f2ed7ece"></a>

```
body {
  background: white url("pendant.png");
  background-repeat: repeat-y;
  background-position: center;
}
```

[image: A centered background image, with copies repeated up and down the border, padding, and content areas.]

The effect of [repeat-y](#valdef-background-repeat-repeat-y): One copy of the background image is centered, and other copies are put above and below it to make a vertical band behind the element.

<a id="example-64e67bc4"></a>

```
body {
  background-image: url(dot.png) white;
  background-repeat: space
}
```

[image: Image of an element with a dotted background]

The effect of [space](#valdef-background-repeat-space): the image of a dot is tiled to cover the whole background and the images are equally spaced.

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-repeat](#propdef-background-repeat) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

### <a id="background-attachment"></a>2.5. <a id="the-background-attachment"></a> Affixing Images: the [background-attachment](#propdef-background-attachment) property

| Name: | *<a id="propdef-background-attachment"></a>background-attachment* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<attachment>](#typedef-attachment)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | scroll |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | list, each item the keyword as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |

If [background images](#background-images) are specified, this property specifies whether they are fixed with regard to the viewport ([fixed](#valdef-background-attachment-fixed)) or scroll along with the box ([scroll](#valdef-background-attachment-scroll)) or its contents ([local](#valdef-background-attachment-local)). The property’s value is given as a comma-separated list of [<attachment>](#typedef-attachment) keywords where

```
<a id="typedef-attachment"></a><attachment> = scroll | fixed | local
```

***<a id="valdef-background-attachment-fixed"></a>fixed* **
: The background is fixed with regard to the viewport. In paged media where there is no viewport, a [fixed](#valdef-background-attachment-fixed) background is fixed with respect to the page box and therefore replicated on every page.

Note: There is only one viewport per view. Even if an box is a scroll container, a [fixed](#valdef-background-attachment-fixed) background doesn’t move with the box.

***<a id="valdef-background-attachment-local"></a>local* **
: The background is fixed with regard to the box’s contents: if the box has a scrolling mechanism, the background scrolls with the box’s contents, and the [background painting area](#background-painting-area) and [background positioning area](#background-positioning-area) are relative to the scrollable overflow area of the box rather than to the border framing them. Because the scrollable overflow area does not include the [border area](css-box-4.md#border-area), for scroll containers the [border-box](#valdef-background-clip-border-box) value of [background-clip](#propdef-background-clip) may be treated the same as [padding-box](#valdef-background-clip-padding-box).
***<a id="valdef-background-attachment-scroll"></a>scroll* **
: The background is fixed with regard to the box itself and does not scroll with its contents. (It is effectively attached to the box’s border.)
Even if the image is fixed, it is still only visible when it is in the [background painting area](#background-painting-area) of the box or otherwise unclipped. (See [§ 2.11 Backgrounds of Special Elements](#special-backgrounds) for the cases when background images are not clipped.) Thus, unless the image is tiled, it may be invisible.

<a id="example-e97d52e0"></a> This example creates an infinite vertical band that remains “glued” to the viewport when the document is scrolled.

```
body {
  background: red url("pendant.gif");
  background-repeat: repeat-y;
  background-attachment: fixed;
}
```

Note: User agents that do not support [fixed](#valdef-background-attachment-fixed) backgrounds (for example due to limitations of the hardware platform) will ignore declarations with the keyword fixed. For example:

```
<a id="example-6e8f559e"></a>body {
  /* For all UAs: */
  background: white url(paper.png) scroll;
  /* For UAs that do fixed backgrounds: */
  background: white url(ledger.png) fixed;
}
h1 {
  /* For all UAs: */
  background: silver;
  /* For UAs that do fixed backgrounds: */
  background: url(stripe.png) fixed, white url(ledger.png) fixed;
}
```

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-attachment](#propdef-background-attachment) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

### <a id="background-position"></a>2.6. <a id="the-background-position"></a>Positioning Images: the [background-position](#propdef-background-position) property

| Name: | *<a id="propdef-background-position"></a>background-position* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<bg-position>](#typedef-bg-position)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | 0% 0% |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to size of [background positioning area](#background-positioning-area) *minus* size of background image; see text |
| [Computed value:](css-cascade-5.md#computed) | list, each item a pair of offsets (horizontal and vertical) from the top left origin each given as a computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value |
| Canonical order: | per grammar |
| Animation type: | repeatable list |

If [background images](#background-images) have been specified, this property specifies their initial position (after any [resizing](#background-size)) within their corresponding [background positioning area](#background-positioning-area).

The property’s value is given as a comma-separated list of [<bg-position>](#typedef-bg-position) values where

```
<a id="typedef-bg-position"></a><bg-position> = [
  [ left | center | right | top | bottom | <length-percentage> ]
|
  [ left | center | right | <length-percentage> ]
  [ top | center | bottom | <length-percentage> ]
|
  [ center | [ left | right ] <length-percentage>? ] &&
  [ center | [ top | bottom ] <length-percentage>? ]
]
```

If only one value is specified, the second value is assumed to be [center](#valdef-background-position-center). If two values are given, a [<length-percentage>](css-values-4.md#typedef-length-percentage) as the first value represents the horizontal position (or offset) and a <length-percentage> as the second value represents the vertical position (or offset). The <length-percentage> values here represent an offset of the top left corner of the background image from the top left corner of the [background positioning area](#background-positioning-area).

Note: A pair of keywords can be reordered, while a combination of keyword and length or percentage cannot. So center left is valid while 50% left is not.

If three or four values are given, then each [<length-percentage>](css-values-4.md#typedef-length-percentage) represents an offset and must be preceded by a keyword, which specifies from which edge the offset is given. For example, [background-position: bottom 10px right 20px](#propdef-background-position) represents a 10px vertical offset up from the bottom edge and a 20px horizontal offset leftward from the right edge. If three values are given, the missing offset is assumed to be zero.

Positive values represent an offset *inward* from the edge of the [background positioning area](#background-positioning-area). Negative values represent an offset *outward* from the edge of the background positioning area.

<a id="example-b28c564d"></a> The following declarations give the stated (horizontal, vertical) offsets from the top left corner:

```
background-position: left 10px top 15px;   /* 10px, 15px */
background-position: left      top     ;   /*  0px,  0px */
background-position:      10px     15px;   /* 10px, 15px */
background-position: left          15px;   /*  0px, 15px */
background-position:      10px top     ;   /* 10px,  0px */
background-position: left      top 15px;   /*  0px, 15px */
background-position: left 10px top     ;   /* 10px,  0px */
```

***<a id="valdef-background-position-percentage"></a>[<percentage>](css-values-4.md#percentage-value)* **
: A percentage for the horizontal offset is relative to (*width of [background positioning area](#background-positioning-area)* - *width of [background image](#background-images)*). A percentage for the vertical offset is relative to (*height of [background positioning area](#background-positioning-area)* - *height of [background image](#background-images)*), where the size of the image is the size given by [background-size](#propdef-background-size).

<a id="example-c27a0cea"></a> For example, with a value pair of 0% 0%, the upper left corner of the image is aligned with the upper left corner of, usually, the box’s [padding edge](css-box-4.md#padding-edge). A value pair of 100% 100% places the lower right corner of the image in the lower right corner of the area. With a value pair of 75% 50%, the point 75% across and 50% down the image is to be placed at the point 75% across and 50% down the area.

[image: Diagram of image position within element]

Diagram of the meaning of [background-position: 75% 50%](#propdef-background-position).

***<a id="valdef-background-position-length"></a>[<length>](css-values-4.md#length-value)* **
: A length value gives a fixed length as the offset. For example, with a value pair of 2cm 1cm, the upper left corner of the image is placed 2cm to the right and 1cm below the upper left corner of the [background positioning area](#background-positioning-area).
***<a id="valdef-background-position-top"></a>[top](#valdef-background-position-top)* **
: Computes to 0% for the vertical position if one or two values are given, otherwise specifies the top edge as the origin for the next offset.
***<a id="valdef-background-position-right"></a>[right](#valdef-background-position-right)* **
: Computes to 100% for the horizontal position if one or two values are given, otherwise specifies the right edge as the origin for the next offset.
***<a id="valdef-background-position-bottom"></a>[bottom](#valdef-background-position-bottom)* **
: Computes to 100% for the vertical position if one or two values are given, otherwise specifies the bottom edge as the origin for the next offset.
***<a id="valdef-background-position-left"></a>[left](#valdef-background-position-left)* **
: Computes to 0% for the horizontal position if one or two values are given, otherwise specifies the left edge as the origin for the next offset.
***<a id="valdef-background-position-center"></a>[center](#valdef-background-position-center)* **
: Computes to 50% (left 50%) for the horizontal position if the horizontal position is not otherwise specified, or 50% (top 50%) for the vertical position if it is.
<a id="example-4c5187aa"></a> The following [background](#propdef-background) shorthand declarations use keywords to set [background-position](#propdef-background-position) to the stated percentage values.

```
body { background: url("banner.jpeg") right top }    /* 100%   0% */
body { background: url("banner.jpeg") top center }   /*  50%   0% */
body { background: url("banner.jpeg") center }       /*  50%  50% */
body { background: url("banner.jpeg") bottom }       /*  50% 100% */
```

<a id="example-6c448699"></a> In the example below, the (single) image is placed in the lower-right corner of the viewport.

```
body {
  background-image: url("logo.png");
  background-attachment: fixed;
  background-position: 100% 100%;
  background-repeat: no-repeat;
}
```

<a id="example-1d05ddd8"></a> Background positions can also be relative to other corners than the top left. For example, the following puts the background image 10px from the bottom and 3em from the right:

```
background-position: right 3em bottom 10px
```

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-position](#propdef-background-position) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

#### <a id="bg-position-serialization"></a>2.6.1. Serialization of [background-position](#propdef-background-position) values

The [specified value](css-cascade-5.md#specified-value) and [computed value](css-cascade-5.md#computed-value) of the [<bg-position>](#typedef-bg-position) type serialize exactly as defined in [[CSS-VALUES-4]](#biblio-css-values-4) for <position>. For 3-value productions (which are not valid in <position>), the specified value serialization is identical to the equivalent 4-value syntax except that the omitted offset remains omitted.

### <a id="background-clip"></a>2.7. <a id="the-background-clip"></a> Painting Area: the [background-clip](#propdef-background-clip) property

| Name: | *<a id="propdef-background-clip"></a>background-clip* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<visual-box>](css-box-4.md#typedef-visual-box)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | border-box |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | list, each item a keyword as specified |
| Canonical order: | per grammar |
| Animation type: | repeatable list |

Determines the *<a id="background-painting-area"></a>background painting area*, which determines the area within which the background is painted. Values have the following meanings:

***<a id="valdef-background-clip-border-box"></a>border-box* **
: The background is painted within (clipped to) the [border box](css-box-4.md#border-box).
***<a id="valdef-background-clip-padding-box"></a>padding-box* **
: The background is painted within (clipped to) the [padding box](css-box-4.md#padding-box).
***<a id="valdef-background-clip-content-box"></a>content-box* **
: The background is painted within (clipped to) the [content box](css-box-4.md#content-box).
Note: The root element has a different [background painting area](#background-painting-area) and thus the [background-clip](#propdef-background-clip) property has no effect when specified on it. See [§ 2.11 Backgrounds of Special Elements](#special-backgrounds).

Note: The background is always drawn *behind* the border, if any. See “Elaborate description of Stacking Contexts” in [[CSS2]](#biblio-css2) Appendix E.

See [§ 4.2 Corner Shaping](#corner-shaping) for how [border-radius](#propdef-border-radius) affects the shape of the [background painting area](#background-painting-area).

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-clip](#propdef-background-clip) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

### <a id="background-origin"></a>2.8. <a id="the-background-origin"></a> Positioning Area: the [background-origin](#propdef-background-origin) property

| Name: | *<a id="propdef-background-origin"></a>background-origin* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<visual-box>](css-box-4.md#typedef-visual-box)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | padding-box |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | list, each item a keyword as specified |
| Canonical order: | per grammar |
| Animation type: | repeatable list |

This property determines the *<a id="background-positioning-area"></a>background positioning area*: the area within which any background images are positioned. For elements rendered as multiple box fragments (e.g., inline boxes on several lines, boxes on several pages), specifies which boxes box-decoration-break [[CSS-BREAK-3]](#biblio-css-break-3) operates on to determine the background positioning area(s).

***<a id="valdef-background-origin-padding-box"></a>padding-box* **
: The position is relative to the [padding box](css-box-4.md#padding-box). (For single boxes 0 0 is the upper left corner of the padding edge, 100% 100% is the lower right corner.)
***<a id="valdef-background-origin-border-box"></a>border-box* **
: The position is relative to the [border box](css-box-4.md#border-box).
***<a id="valdef-background-origin-content-box"></a>content-box* **
: The position is relative to the [content box](css-box-4.md#content-box).
If the [background-attachment](#propdef-background-attachment) value for this [layer](#background-image-layer) is [fixed](#valdef-background-attachment-fixed), then this property has no effect: in this case the [background positioning area](#background-positioning-area) is the initial containing block.

Note: If [background-clip](#propdef-background-clip) is [padding-box](#valdef-background-clip-padding-box), [background-origin](#propdef-background-origin) is [border-box](#valdef-background-origin-border-box), [background-position](#propdef-background-position) is top left (the initial value), and the element has a non-zero border, then the top and left edges of the [background image](#background-images) will be clipped.

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-origin](#propdef-background-origin) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

### <a id="background-size"></a>2.9. <a id="the-background-size"></a>Sizing Images: the [background-size](#propdef-background-size) property

| Name: | *<a id="propdef-background-size"></a>background-size* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<bg-size>](#typedef-bg-size)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | auto |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | see text |
| [Computed value:](css-cascade-5.md#computed) | list, each item a pair of sizes (one per axis) each represented as either a keyword or a computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value |
| Canonical order: | per grammar |
| Animation type: | repeatable list |

This property specifies the size of each [background image](#background-images). The property’s value is given as a comma-separated list of [<bg-size>](#typedef-bg-size) values where

```
<a id="typedef-bg-size"></a><bg-size> = [ <length-percentage [0,∞]> | auto ]{1,2} | cover | contain
```

Values have the following meanings:

***<a id="valdef-background-size-contain"></a>contain* **
: Scale the image, while preserving its [natural aspect ratio](css-images-3.md#natural-aspect-ratio) (if any), to the largest size such that both its width and its height can fit inside the [background positioning area](#background-positioning-area).
***<a id="valdef-background-size-cover"></a>cover* **
: Scale the image, while preserving its [natural aspect ratio](css-images-3.md#natural-aspect-ratio) (if any), to the smallest size such that both its width and its height can completely cover the [background positioning area](#background-positioning-area).
**[ *<a id="valdef-background-size-length-percentage-0"></a>[<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage)* | *<a id="valdef-background-size-auto"></a>auto* ]{1,2} **
: The first value gives the width of the corresponding image, the second value its height. If only one value is given the second is assumed to be [auto](#valdef-background-size-auto).

A [<percentage>](css-values-4.md#percentage-value) is relative to the [background positioning area](#background-positioning-area).

An [auto](#valdef-background-size-auto) value for one dimension is resolved by using the image’s [natural aspect ratio](css-images-3.md#natural-aspect-ratio) and the size of the other dimension, or failing that, using the image’s [natural size](css-images-3.md#natural-size), or failing that, treating it as 100%.

If both values are [auto](#valdef-background-size-auto) then the [natural width](css-images-3.md#natural-width) and/or [height](css-images-3.md#natural-height) of the image should be used, if any, the missing dimension (if any) behaving as auto as described above. If the image has neither [natural size](css-images-3.md#natural-size), its size is determined as for [contain](#valdef-background-size-contain).

Negative values are invalid.

<a id="example-bb1c7b0c"></a> Here are some examples. The first example stretches the background image independently in both dimensions to completely cover the content area:

```
div {
  background-image: url(plasma.png);
  background-repeat: no-repeat;
  background-size: 100% 100%;
  background-origin: content-box }
```

The second example stretches the image so that exactly two copies fit horizontally. The aspect ratio is preserved:

```
p {
  background-image: url(tubes.png);
  background-size: 50% auto;
  background-origin: border-box }
```

This example forces the background image to be 15 by 15 pixels:

```
p {
  background-size: 15px 15px;
  background-image: url(tile.png)}
```

This example uses the image’s natural size. Note that this is the only possible behavior in CSS level 1 and 2.

```
body {
background-size: auto;            /* default */
background-image: url(flower.png) }
```

The following example rounds the height of the image to 33.3%, up from the specified value of 30%. At 30%, three images would fit entirely and a fourth only partially. After rounding, three images fit exactly. The width of the image is 20% of the background positioning area width and is not rounded.

```
p {
  background-image: url(chain.png);
  background-repeat: no-repeat round;
  background-size: 20% 30% }
```

If [background-repeat](#propdef-background-repeat) is [round](#valdef-background-repeat-round) for one (or both) dimensions, there is a second step. The UA must scale the image in that dimension (or both dimensions) so that it fits a whole number of times in the [background positioning area](#background-positioning-area). In the case of the width (height is analogous):

If *X* ≠ 0 is the width of the image after step one and *W* is the width of the background positioning area, then the rounded width *X'* = *W* / round(*W* / *X*) where round() is a function that returns the nearest natural number (integer greater than zero).

If [background-repeat](#propdef-background-repeat) is [round](#valdef-background-repeat-round) for one dimension only and if [background-size](#propdef-background-size) is [auto](#valdef-background-size-auto) for the other dimension, then there is a third step: that other dimension is scaled so that the original aspect ratio is restored.

<a id="example-a61fb097"></a> In this example the background image is shown at its natural size:

```
div {
  background-image: url(image1.png);
  background-repeat: repeat;         /* default */
  background-size: auto }            /* default */
```

In the following example, the background is shown with a width of 3em and its height is scaled proportionally to keep the original aspect ratio:

```
div {
  background-image: url(image2.png);
  background-repeat: repeat;         /* default */
  background-size: 3em }             /* = '3em auto' */
```

In the following example, the background is shown with a width of approximately 3em: scaled so that it fits a whole number of times in the width of the background. The height is scaled proportionally to keep the original aspect ratio:

```
div {
  background-image: url(image3.png);
  background-repeat: round repeat;
  background-size: 3em auto }
```

In the following example, the background image is shown with a width of 3em and a height that is either the height corresponding to that width at the original aspect ratio or slightly less:

```
div {
  background-image: url(image4.png);
  background-repeat: repeat round;
  background-size: 3em auto }
```

In the following example, the background image is shown with a height of approximately 4em: scaled slightly so that it fits a whole number of times in the background height. The width is the approximately the width that corresponds to a 4em height at the original aspect ratio: scaled slightly so that it fits a whole number of times in the background width.

```
div {
  background-image: url(image5.png);
  background-repeat: round;
  background-size: auto 4em }
```

If the background image’s width or height resolves to zero, this causes the image not to be displayed. (The effect is the same as if it had been a transparent image.)

See [§ 2.1 Layering Multiple Background Images](#layering) for how [background-size](#propdef-background-size) interacts with other comma-separated background properties to form each [background image layer](#background-image-layer).

#### <a id="bg-size-serialization"></a>2.9.1. Serialization of [background-size](#propdef-background-size) values

The [specified value](css-cascade-5.md#specified-value) and [computed value](css-cascade-5.md#computed-value) of the [<bg-size>](#typedef-bg-size) type always serialize as two values, even when the second value is [auto](#valdef-background-size-auto), due to the shortest, most backwards-compatible serialization principle.

### <a id="background"></a>2.10. <a id="the-background"></a> Backgrounds Shorthand: the [background](#propdef-background) property

| Name: | *<a id="propdef-background"></a>background* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<bg-layer>](#typedef-bg-layer)[#](css-values-4.md#mult-comma)[?](css-values-4.md#mult-opt) [,](css-values-4.md#comb-comma) [<final-bg-layer>](#typedef-final-bg-layer) |
| [Initial:](css-cascade-5.md#initial-values) | see individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | see individual properties |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

The [background](#propdef-background) property is a [shorthand property](css-cascade-5.md#shorthand-property) for setting most background properties at the same place in the style sheet. The number of comma-separated items defines the number of [background image layers](#background-image-layer). Given a valid declaration, for each layer the shorthand first sets the corresponding value of each of [background-image](#propdef-background-image), [background-position](#propdef-background-position), [background-size](#propdef-background-size), [background-repeat](#propdef-background-repeat), [background-origin](#propdef-background-origin), [background-clip](#propdef-background-clip) and [background-attachment](#propdef-background-attachment) to that property’s [initial value](css-cascade-5.md#initial-value), then assigns any explicit values specified for this layer in the declaration. Finally [background-color](#propdef-background-color) is set to the specified color, if any, else set to its initial value.

This property’s value is given as a comma-separated list of values where

```
<a id="typedef-bg-layer"></a><bg-layer> = <bg-image> || <bg-position> [ / <bg-size> ]? || <repeat-style> || <attachment> || <visual-box> || <visual-box>
<a id="typedef-final-bg-layer"></a><final-bg-layer> =  <bg-image> || <bg-position> [ / <bg-size> ]? || <repeat-style> || <attachment> || <visual-box> || <visual-box> || <'background-color'>
```

Note: A color is permitted in [<final-bg-layer>](#typedef-final-bg-layer), but not in [<bg-layer>](#typedef-bg-layer).

If one [<visual-box>](css-box-4.md#typedef-visual-box) value is present then it sets both [background-origin](#propdef-background-origin) and [background-clip](#propdef-background-clip) to that value. If two values are present, then the first sets background-origin and the second background-clip.

<a id="example-15a7e3d5"></a> In the first rule of the following example, only a value for [background-color](#propdef-background-color) has been given and the other individual properties are set to their initial values. In the second rule, many individual properties have been specified.

```
body { background: red }
p { background: url("chess.png") 40% / 10em gray
                round fixed border-box; }
```

The first rule is equivalent to:

```
body {
    background-color: red;
    background-position: 0% 0%;
    background-size: auto;
    background-repeat: repeat;
    background-clip: border-box;
    background-origin: padding-box;
    background-attachment: scroll;
    background-image: none }
```

The second is equivalent to:

```
p {
    background-color: gray;
    background-position: 40% 50%;
    background-size: 10em auto;
    background-repeat: round;
    background-clip: border-box;
    background-origin: border-box;
    background-attachment: fixed;
    background-image: url(chess.png) }
```

<a id="example-ff3ed279"></a> The following example shows how a both a background color (#CCC) and a background image (url(metal.jpg)) are set. The image is rescaled to the full width of the element:

```
E { background: #CCC url("metal.jpg") top left / 100% auto no-repeat}
```

<a id="example-1a1113f1"></a> Another example shows equivalence:

```
div { background: padding-box url(paper.jpg) white center }
div {
    background-color: white;
    background-image: url(paper.jpg);
    background-repeat: repeat;
    background-attachment: scroll;
    background-position: center;
    background-clip: padding-box;
    background-origin: padding-box;
    background-size: auto auto }
```

<a id="example-3524ad83"></a> The following declaration with multiple, comma-separated values

```
background: url(a.png) top left no-repeat,
            url(b.png) center / 100% 100% no-repeat,
            url(c.png) white;
```

is equivalent to

```
background-image:      url(a.png),  url(b.png),          url(c.png);
background-position:   top left,    center,              top left;
background-repeat:     no-repeat,   no-repeat,           repeat;
background-clip:       border-box,  border-box,          border-box;
background-origin:     padding-box, padding-box,         padding-box;
background-size:       auto auto,   100% 100%,           auto auto;
background-attachment: scroll,      scroll,              scroll;
background-color:      white;
```

### <a id="special-backgrounds"></a>2.11. Backgrounds of Special Elements

The document canvas is the infinite surface over which the document is rendered. [[CSS2]](#biblio-css2) Since no element corresponds to the canvas, in order to allow styling of the canvas CSS propagates the background of the root element (or, in the case of HTML, the <body> element) as described below. However, if the element whose background would be used for the canvas is display: none, then the [canvas background](#canvas-background) is transparent.

If the [canvas background](#canvas-background) is not opaque, the *<a id="canvas-surface"></a>canvas surface* below it shows through. The texture of the [canvas surface](#canvas-surface) is UA-dependent (but is typically an opaque white).

#### <a id="root-background"></a>2.11.1. The Canvas Background and the Root Element

The background of the root element becomes the *<a id="canvas-background"></a>canvas background* and its [background painting area](#background-painting-area) extends to cover the entire canvas. However, any images are sized and positioned relative to the root element’s box as if they were painted for that element alone. (In other words, the [background *positioning* area](#background-positioning-area) is determined as for the root element.) The root element does not paint this background again, i.e., the [used value](css-cascade-5.md#used-value) of its background is [transparent](css-color-4.md#valdef-color-transparent).

#### <a id="body-background"></a>2.11.2. The Canvas Background and the HTML <body> Element

For documents whose root element is an HTML `HTML` element or an XHTML `html` element [[HTML]](#biblio-html): if the [computed value](css-cascade-5.md#computed-value) of [background-image](#propdef-background-image) on the root element is [none](#valdef-background-image-none) and its [background-color](#propdef-background-color) is [transparent](css-color-4.md#valdef-color-transparent), user agents must instead propagate the computed values of the background properties from that element’s first HTML `BODY` or XHTML `body` child element. The [used values](css-cascade-5.md#used-value) of that `BODY` element’s background properties are their [initial values](css-cascade-5.md#initial-value), and the propagated values are treated as if they were specified on the root element. It is recommended that authors of HTML documents specify the canvas background using the `BODY` element rather than the `HTML` element.

Note: Using containment disables this special handling of the HTML `body` element. See the CSS Containment 1 § 2 Strong Containment: the contain property for details.

<a id="example-8733c37a"></a> According to these rules, the canvas underlying the following HTML document will have a “marble” background:

```
<!DOCTYPE html PUBLIC '-//W3C//DTD HTML 4.0//EN'
  >
<html>
  <head>
    <title>Setting the canvas background</title>
    <style type="text/css">
       body { background: url("http://example.org/marble.png") }
    </style>
  </head>
  <body>
    <p>My background is marble.</p>
  </body>
</html>
```

#### <a id="first-line-background"></a>2.11.3. The ::first-line Pseudo-element‘s Background

The ::first-line pseudo-element is like an inline-level element for the purposes of the background (see section 5.12.1 of [[CSS2]](#biblio-css2)). That means, e.g., that in a left-justified first line, the background does not necessarily extend all the way to the right edge.

## <a id="borders"></a>3. Borders

The [border](css-box-4.md#border) can either be a predefined style (solid line, double line, dotted line, pseudo-3D border, etc.) or it can be an image. In the former case, various properties define the style ([border-style](#propdef-border-style)), color ([border-color](#propdef-border-color)), and thickness ([border-width](#propdef-border-width)) of the border.

### <a id="border-color"></a>3.1. <a id="the-border-color"></a> Line Colors: the [border-color](#propdef-border-color) properties

| Name: | *<a id="propdef-border-top-color"></a>border-top-color*, *<a id="propdef-border-right-color"></a>border-right-color*, *<a id="propdef-border-bottom-color"></a>border-bottom-color*, *<a id="propdef-border-left-color"></a>border-left-color* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <color> |
| [Initial:](css-cascade-5.md#initial-values) | [currentColor](css-color-4.md#currentcolor-color) |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | computed color |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Logical property group: | [border-color](#propdef-border-color) |

| Name: | *<a id="propdef-border-color"></a>border-color* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <color>[{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | see individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

These properties set the foreground *<a id="border-color-dfn"></a>color* of the [border](css-box-4.md#border) specified by the [border-style](#propdef-border-style) properties.

The [border-color](#propdef-border-color) property is a [shorthand property](css-cascade-5.md#shorthand-property) for setting [border-top-color](#propdef-border-top-color), [border-right-color](#propdef-border-right-color), [border-bottom-color](#propdef-border-bottom-color), and [border-left-color](#propdef-border-left-color) in a single declaration.

If there is only one component value, it applies to all sides. If there are two values, the top and bottom are set to the first value and the right and left are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

### <a id="border-style"></a>3.2. <a id="the-border-style"></a>Line Patterns: the [border-style](#propdef-border-style) properties

| Name: | *<a id="propdef-border-top-style"></a>border-top-style*, *<a id="propdef-border-right-style"></a>border-right-style*, *<a id="propdef-border-bottom-style"></a>border-bottom-style*, *<a id="propdef-border-left-style"></a>border-left-style* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<line-style>](#typedef-line-style) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | specified keyword |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Logical property group: | [border-style](#propdef-border-style) |

| Name: | *<a id="propdef-border-style"></a>border-style* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<line-style>](#typedef-line-style)[{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | see individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

These properties control whether a [border](css-box-4.md#border) appears, and if it does what *<a id="border-style-dfn"></a>style* it’s drawn in (if it is not overridden by a [border image](#border-images)).

The [border-style](#propdef-border-style) property is a [shorthand property](css-cascade-5.md#shorthand-property) for setting [border-top-style](#propdef-border-top-style), [border-right-style](#propdef-border-right-style), [border-bottom-style](#propdef-border-bottom-style), and [border-left-style](#propdef-border-left-style) in a single declaration.

If there is only one component value, it applies to all sides. If there are two values, the top and bottom are set to the first value and the right and left are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

The style is specified as a [<line-style>](#typedef-line-style) keyword, where

```
<a id="typedef-line-style"></a><line-style> = none | hidden | dotted | dashed | solid | double | groove | ridge | inset | outset
```

Values have the following meanings:

***<a id="valdef-line-style-none"></a>none* **
: No border. Color and width are ignored (i.e., the border has width 0). Note this means that the initial value of [border-image-width](#propdef-border-image-width) will also resolve to zero.
***<a id="valdef-line-style-hidden"></a>hidden* **
: Same as [none](#valdef-line-style-none), but has different behavior in the border conflict resolution rules for border-collapsed tables [[CSS2]](#biblio-css2).
***<a id="valdef-line-style-dotted"></a>dotted* **
: A series of round dots.
***<a id="valdef-line-style-dashed"></a>dashed* **
: A series of square-ended dashes.
***<a id="valdef-line-style-solid"></a>solid* **
: A single line segment.
***<a id="valdef-line-style-double"></a>double* **
: Two parallel solid lines with some space between them. (The thickness of the lines is not specified, but the sum of the lines and the space must equal [border-width](#propdef-border-width).)
***<a id="valdef-line-style-groove"></a>groove* **
: Looks as if it were carved in the canvas. (This is typically achieved by creating a “shadow” from two colors that are slightly lighter and darker than the specified [border-color](#propdef-border-color).)
***<a id="valdef-line-style-ridge"></a>ridge* **
: Looks as if it were coming out of the canvas.
***<a id="valdef-line-style-inset"></a>inset* **
: Looks as if the content on the inside of the border is sunken into the canvas. Treated as [ridge](#valdef-line-style-ridge) in the collapsing border model. [[CSS2]](#biblio-css2)
***<a id="valdef-line-style-outset"></a>outset* **
: Looks as if the content on the inside of the border is raised out of the canvas. Treated as [groove](#valdef-line-style-groove) in the collapsing border model. [[CSS2]](#biblio-css2)
Borders are drawn in front of the element’s background, but behind the element’s content (in case it overlaps).

Example renderings of the predefined border styles.

Note: Border colors close to black or white may need different color calculations than colors in between in order to create the required “3D” effect of [groove](#valdef-line-style-groove), [ridge](#valdef-line-style-ridge), [inset](#valdef-line-style-inset), or [outset](#valdef-line-style-outset).

Note: There is no control over the spacing of the dots and dashes, nor over the length of the dashes. Implementations are encouraged to choose a spacing that makes the corners symmetrical.

Note: This specification does not define how borders of different styles should be joined in the corner. Also note that rounded corners may cause the corners and the contents to overlap, if the padding is less than the radius of the corner.

### <a id="border-width"></a>3.3. <a id="the-border-width"></a> Line Thickness: the [border-width](#propdef-border-width) properties

| Name: | *<a id="propdef-border-top-width"></a>border-top-width*, *<a id="propdef-border-right-width"></a>border-right-width*, *<a id="propdef-border-bottom-width"></a>border-bottom-width*, *<a id="propdef-border-left-width"></a>border-left-width* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<line-width>](#typedef-line-width) |
| [Initial:](css-cascade-5.md#initial-values) | medium |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | absolute length, [snapped as a border width](css-values-4.md#snap-as-a-line-width) |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Logical property group: | [border-width](#propdef-border-width) |

| Name: | *<a id="propdef-border-width"></a>border-width* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<line-width>](#typedef-line-width)[{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | see individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | see individual properties |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

These properties specify the thickness of the [border](css-box-4.md#border), i.e. the *<a id="border-width-dfn"></a>border width*. Where

```
<a id="typedef-line-width"></a><line-width> = <length [0,∞]> | thin | medium | thick
```

Negative values are invalid. The *<a id="valdef-line-width-thin"></a>thin*, *<a id="valdef-line-width-medium"></a>medium*, and *<a id="valdef-line-width-thick"></a>thick* keywords are equivalent to 1px, 3px, and 5px, respectively.

The [border-width](#propdef-border-width) property is a [shorthand property](css-cascade-5.md#shorthand-property) for setting [border-top-width](#propdef-border-top-width), [border-right-width](#propdef-border-right-width), [border-bottom-width](#propdef-border-bottom-width), and [border-left-width](#propdef-border-left-width) in a single declaration.

If there is only one component value, it applies to all sides. If there are two values, the top and bottom are set to the first value and the right and left are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

The resolved value for the [border-width](#propdef-border-width) properties is the [used value](css-cascade-5.md#used-value).

If the [border-style](#propdef-border-style) corresponding to a given [border-width](#propdef-border-width) is [none](#valdef-line-style-none) or [hidden](#valdef-line-style-hidden), then the [used](css-cascade-5.md#used-value) width is 0.

Note: Although the [initial](css-cascade-5.md#initial-value) width is [medium](#valdef-line-width-medium), the initial style is [none](#valdef-line-style-none); therefore the [used](css-cascade-5.md#used-value) initial width is 0.

### <a id="border-shorthands"></a>3.4. <a id="the-border-shorthands"></a> Border Shorthand Properties

| Name: | *<a id="propdef-border-top"></a>border-top*, *<a id="propdef-border-right"></a>border-right*, *<a id="propdef-border-bottom"></a>border-bottom*, *<a id="propdef-border-left"></a>border-left* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<line-width>](#typedef-line-width) [\|\|](css-values-4.md#comb-any) [<line-style>](#typedef-line-style) \|\| <color> |
| [Initial:](css-cascade-5.md#initial-values) | See individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

These [shorthand properties](css-cascade-5.md#shorthand-property) set the [border-width](#propdef-border-width), [border-color](#propdef-border-color), and [border-style](#propdef-border-style) of the top, right, bottom, and left [borders](css-box-4.md#border) of a box. Omitted values are set to their [initial values](css-cascade-5.md#initial-value).

| Name: | *<a id="propdef-border"></a>border* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<line-width>](#typedef-line-width) [\|\|](css-values-4.md#comb-any) [<line-style>](#typedef-line-style) \|\| <color> |
| [Initial:](css-cascade-5.md#initial-values) | See individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except ruby base containers and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

The [border](#propdef-border) property is a [shorthand property](css-cascade-5.md#shorthand-property) for setting the same [border-width](#propdef-border-width), [border-color](#propdef-border-color), and [border-style](#propdef-border-style) for all four borders of a box. Unlike the shorthand [margin](css-box-4.md#propdef-margin) and [padding](css-box-4.md#propdef-padding) properties, the border property cannot set different values on the four borders. To do so, one or more of the other border properties must be used.

The [border](#propdef-border) shorthand also resets [border-image](#propdef-border-image) to its initial value. It is therefore recommended that authors use the border shorthand, rather than other shorthands or the individual properties, to override any border settings earlier in the cascade. This will ensure that border-image has also been reset to allow the new styles to take effect.

Note: The CSS Working Group intends for the [border](#propdef-border) shorthand to reset all border properties in future levels of CSS as well. For example, if a border-characters property is introduced in the future to allow glyphs as borders, it will also be reset by the border shorthand. By using the border shorthand to reset borders, authors can be guaranteed a “blank canvas” no matter what properties are introduced in the future.

<a id="example-e8ed94e0"></a> For example, the first rule below is equivalent to the set of five rules shown after it:

```
p { border: solid red }
p {
  border-top: solid red;
  border-right: solid red;
  border-bottom: solid red;
  border-left: solid red;
  border-image: none;
}
```

Since, to some extent, the properties have overlapping functionality, the order in which the rules are specified is important.

<a id="example-10fca894"></a> Consider this example:

```
blockquote {
  border-color: red;
  border-left: double;
  color: black
}
```

In the above example, the color of the left border is black, while the other borders are red. This is due to [border-left](#propdef-border-left) setting the width, style, and color. Since the color value is not given by the border-left property, it will be taken from the [color](css-color-4.md#propdef-color) property. The fact that the color property is set after the border-left property is not relevant.

## <a id="corners"></a>4. Rounded Corners

### <a id="border-radius"></a>4.1. <a id="the-border-radius"></a> Curve Radii: the [border-radius](#propdef-border-radius) properties

| Name: | *<a id="propdef-border-top-left-radius"></a>border-top-left-radius*, *<a id="propdef-border-top-right-radius"></a>border-top-right-radius*, *<a id="propdef-border-bottom-right-radius"></a>border-bottom-right-radius*, *<a id="propdef-border-bottom-left-radius"></a>border-bottom-left-radius* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage)[{1,2}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | all elements (but see prose) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | Refer to corresponding dimension of the [border box](css-box-4.md#border-box). |
| [Computed value:](css-cascade-5.md#computed) | pair of computed [<length-percentage>](css-values-4.md#typedef-length-percentage) values |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Logical property group: | [border-radius](#propdef-border-radius) |

| Name: | *<a id="propdef-border-radius"></a>border-radius* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage)[{1,4}](css-values-4.md#mult-num-range) [ / <length-percentage [0,∞]>{1,4} ][?](css-values-4.md#mult-opt) |
| [Initial:](css-cascade-5.md#initial-values) | see individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | all elements (but see prose) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | Refer to corresponding dimension of the [border box](css-box-4.md#border-box). |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | see individual properties |

The two [<length-percentage>](css-values-4.md#typedef-length-percentage) values of the border-*-radius properties define the *<a id="border-radii"></a>radii* of a quarter ellipse that defines the shape of the corner of the outer [border edge](css-box-4.md#border-edge) (see the diagram below). The first value is the horizontal radius, the second the vertical radius. If the second value is omitted it is copied from the first. If either length is zero, the corner is square, not rounded. Percentages for the horizontal radius refer to the width of the [border box](css-box-4.md#border-box), whereas percentages for the vertical radius refer to the height of the border box. Negative values are invalid.

[image: Diagram of the inscribed ellipse]

The two values of [border-top-left-radius: 55pt 25pt](#propdef-border-top-left-radius) define the curvature of the corner.

<a id="example-6b4b7b7d"></a> This example draws ovals of 15em wide and 10em high:

```
DIV.standout {
    width: 13em;
    height: 8em;
    border: solid black 1em;
    border-radius: 7.5em 5em }
```

The [border-radius](#propdef-border-radius) shorthand sets all four border-*-radius properties. If values are given before and after the slash, then the values before the slash set the horizontal radii and the values after the slash set the vertical radii. If there is no slash, then the values set the radii in both axes equally. The four values for each radii are given in the order top-left, top-right, bottom-right, bottom-left. If bottom-left is omitted it is the same as top-right. If bottom-right is omitted it is the same as top-left. If top-right is omitted it is the same as top-left.

<a id="example-19e399d6"></a>

```
border-radius: 4em;
```

is equivalent to

```
border-top-left-radius:     4em;
border-top-right-radius:    4em;
border-bottom-right-radius: 4em;
border-bottom-left-radius:  4em;
```

and

```
border-radius: 2em 1em 4em / 0.5em 3em;
```

is equivalent to

```
border-top-left-radius:     2em 0.5em;
border-top-right-radius:    1em 3em;
border-bottom-right-radius: 4em 0.5em;
border-bottom-left-radius:  1em 3em;
```

### <a id="corner-shaping"></a>4.2. Corner Shaping

The [padding edge](css-box-4.md#padding-edge) (inner border) radius is the outer border radius minus the corresponding border thickness. In the case where this results in a negative value, the inner radius is zero. (In such cases the center of the border’s inner curve might not coincide with that of its outer curve.) Likewise the [content edge](css-box-4.md#content-edge) radius is the padding edge radius minus the corresponding [padding](css-box-4.md#padding), or if that is negative, zero. The border and padding thicknesses in the curved region are thus interpolated from the adjoining sides, and when two adjoining borders are of different thicknesses the corner will show a smooth transition between the thicker and thinner borders.

All border styles ([solid](#valdef-line-style-solid), [dotted](#valdef-line-style-dotted), [inset](#valdef-line-style-inset), etc.) follow the curve of the border.

The effect of a rounded corner when the two borders it connects are of unequal thickness (left) and the effect of a rounded corner on borders that are thicker than the radius of the corner (right).

Note: If the center of a corner’s outer curve is past an opposite [padding edge](css-box-4.md#padding-edge) (in the [border area](css-box-4.md#border-area) of a side opposite the corner), the inner curve will not be a full quarter ellipse.

```
       p { width: 70px; height: 70px; border: solid 30px;
border-color: orange orange silver silver;
border-top-right-radius: 100%; }
```

|  | [image: The curved corner is an arc from the top left corner sweeping across the top right corner to the bottom right corner, describing a quarter-ellipse; but since the opposite sides have a border thickness the padding edge curve starts inward from the outer arc's endpoints.] |
| --- | --- |

Where the border-radius curve extends into the opposite sides' borders, the arc of the padding edge is less than 90°.

The [margin edge](css-box-4.md#margin-edge), being outside the [border edge](css-box-4.md#border-edge), calculates its radius by *adding* the corresponding margin thickness to each border radius, with the corresponding [outset-adjusted border radius](#outset-adjusted-border-radius) applied.

When expanding an [edge](css-box-4.md#box-box-edge) that has a [border radius](#border-radii), e.g. for computing the [margin edge](css-box-4.md#margin-edge), [box-shadow](#propdef-box-shadow) spread, or overflow-clip-margin, the different border radius values are adjusted so that a small rounded corner with a big outset does not appear to be disproportionally round.

This is done by computing the corresponding [outset-adjusted border radius](#outset-adjusted-border-radius).

To compute the *<a id="outset-adjusted-border-radius"></a>outset-adjusted border radius* given the 2-dimensional size’s *edge*, *radius*, and *outset*:

1.
Let *coverage* be `2 * min(*radius*’s width / *edge*’s width, *radius*’s height / *edge*’s height)`.
2.
Let *adustedRadiusWidth* be the [adjusted radius dimension](#adjusted-radius-dimension) given *coverage*, *radius*’s width, and *outset*’s width.
3.
Let *adustedRadiusHeight* be the [adjusted radius dimension](#adjusted-radius-dimension) given *coverage*, *radius*’s height, and *outset*’s height.
4.
Return (*adustedRadiusWidth*, *adustedRadiusHeight*).

To compute the *<a id="adjusted-radius-dimension"></a>adjusted radius dimension* given numbers *coverage*, *radius*, and *outset*:

1.
If *radius* is greater than *spread*, or if *coverage* is greater than 1, then return `*radius* + *outset*`.
2.
Let *ratio* be `*radius* / *outset*`.
3.
Return `*radius* + *outset* * (1 - (1 - *ratio*)^3 * (1 - *coverage*^3))`.

Note: this algorithm is designed to reduce the effect of the *outset* (or spread) on the shape of the corner. The *coverage* factor makes this reduction more pronounced for rectangular shapes (where the [border radius](#border-radii) is close to 0), and less pronounced for elliptical shapes (where the border radius is close to 50%).

### <a id="corner-clipping"></a>4.3. Corner Clipping

Although [border images](#border-image) are not affected by [border-radius](#propdef-border-radius), other effects that clip painting or event handling to the [border](css-box-4.md#border-edge), [padding](css-box-4.md#padding-edge), or [content](css-box-4.md#content-edge) edge must clip to their respective curves. For example, backgrounds clip to the curve specified by [background-clip](#propdef-background-clip), overflow values other than visible to the curved padding edge (when overflow on both axes is not visible), replaced element content to the curved content edge, pointer events to the curved border edge, etc.

Note: As [border-radius](#propdef-border-radius) reduces the interactive area of an element authors should make sure the remaining interactive area conforms to recommended minima for the platforms they target; in particular, conforming to recommended minimum touch target sizes may require larger widths and heights when border-radius is used.

<a id="example-b5486bac"></a> This example adds appropriate padding, so that the contents do not overflow the corners. Note that there is no border, but the background will still have rounded corners.

```
DIV {
    background: black;
    color: white;
    border-radius: 1em;
    padding: 1em }
```

### <a id="corner-transitions"></a>4.4. Color and Style Transitions

Color and style transitions must be contained within the segment of the border that intersects the smallest rectangle that contains both border radii as well as the center of the inner curve (which may be a point representing the corner of the padding edge, if the border radii are smaller than the [border width](#border-width-dfn)).

If one of these borders is zero-width, then the other border takes up the entire transitional area. Otherwise, the center of color and style transitions between adjoining borders is a point along the curve that is a continuous monotonic function of the ratio of the border widths. However it is not defined what these transitions look like or what function maps from this ratio to a point on the curve.

Given these corner shapes, color and style transitions must be contained within the green region. In case D the rectangle defined by the border radii does not include the center of the inner curve (which is a sharp corner), so the transition region is expanded to include that corner. Transitions may take up the entire transition region, but are not required to: For example, a gradient color transition between two solid border styles might take up only the region bounded by the tips of the outer radii and the tips of the inner radii (represented in case D by the dark green region).

### <a id="corner-overlap"></a>4.5. Overlapping Curves

Corner curves must not overlap: When the sum of any two adjacent border radii exceeds the size of the border box, UAs must proportionally reduce the used values of all border radii until none of them overlap. The algorithm for reducing radii is as follows:

Let *f* = min(*L_i*/*S_i*), where *i* ∈ {top, right, bottom, left}, *S_i* is the sum of the two corresponding radii of the corners on side *i*, and *L_top* = *L_bottom* = the width of the box, and *L_left* = *L_right* = the height of the box. If *f* < 1, then all corner radii are reduced by multiplying them by *f*.

Note: This formula ensures that quarter circles remain quarter circles and large radii remain larger than smaller ones, but it may reduce corners that were already small enough, which may make borders of nearby elements that should look the same look different.

If the curve interferes with UI elements such as scrollbars, the UA may further reduce the used value of the affected border radii (and only the affected border radii) as much as necessary, but no more.

<a id="example-e8de6616"></a> For example, the borders A of the [figure below](#reduced-radius) might be the result of

```
box-sizing: border-box;
width: 6em;
height: 2.5em;
border-radius: 0.5em 2em 0.5em 2em
```

The height (2.5em) is enough for the specified radii (0.5em plus 2.0em). However, if the height is only 2em,

```
box-sizing: border-box;
width: 6em;
height: 2em;
border-radius: 0.5em 2em 0.5em 2em
```

all corners need to be reduced by a factor 0.8 to make them fit. The used border radii thus are 0.4em (instead of 0.5em) and 1.6em (instead of 2em). See borders B in the figure.

<a id="reduced-radius"></a> [image: rectangle with two tiny rounded corners and two very large ones, on opposite corners]

These rounded corner might be the result of width: 6em; height: 2.5em; border-radius: 0.5em 2em 0.5em 2em for A; and ditto but with height: 2em for B.

### <a id="border-radius-tables"></a>4.6. Effect on Tables

The [border-radius](#propdef-border-radius) properties do apply to table, inline-table, and table-cell boxes in separated borders mode (border-collapse: separate). When border-collapse is collapse, they have no effect.

## <a id="border-images"></a>5. Border Images

Authors can specify an image to be used in place of the border styles. In this case, the border’s design is taken from the sides and corners of an image specified with [border-image-source](#propdef-border-image-source), whose pieces may be sliced, scaled, and stretched in various ways to fit the size of the [border image area](#border-image-area). The border-image properties do not affect layout: layout of the box, its content, and surrounding content is based on the [border-width](#propdef-border-width) and [border-style](#propdef-border-style) properties only.

<a id="example-8488cbd9"></a> This example creates a top and bottom border consisting of a whole number of orange diamonds and a left and right border of a single, stretched diamond. The corners are diamonds of a different color. The image to tile is as follows. Apart from the diamonds, it is transparent:

[image: Tile for border]

The image is 81 by 81 pixels and has to be divided into 9 equal parts. The style rules could thus be as follows:

```
DIV {
  border: double orange 1em;
  border-image: url("border.png") 27 round stretch;
 }
```

The result, when applied to a DIV of 12 by 5em, will be similar to this:

[image: Element with a diamond border]

<a id="example-28370333"></a> This shows a more complicated example, demonstrating how the border image corresponds to the fallback border-style but can also extend beyond the border area. The border image is a wavy green border with an extended corner effect:

[image: Diagram: The border image shows a wavy green border with more exaggerated waves towards the corners, which are capped by a disconnected green circle. Four cuts at 124px offsets from each side divide the image into 124px-wide square corners, 124px-wide but thin side slices, and a small center square.]

The [border-image-source](#propdef-border-image-source) image, with the four [border-image-slice](#propdef-border-image-slice) cuts at 124px dividing the image into nine parts.

The rest of the border properties then interact to lay out the tiles as follows:

[image: Diagram: The image-less (fallback) rendering has a green double border. The rendering with border-image shows the wavy green border, ith the waves getting longer as they reach the corners. The corner tiles render as 124px-wide squares and the side tiles repeat a whole number of times to fill the space in between. Because of the gradual corner effects, the tiles extend deep into the padding area. The whole border image effect is outset 31px, so that the troughs of the waves align just outside the padding edge.]

Diagram of all border-image properties and how they interact, and showing the rendering with and without the border-image in effect.

Here, even though the [border-width](#propdef-border-width) is 12px, the [border-image-width](#propdef-border-image-width) property computes to 124px. The [border image area](#border-image-area) is then outset 31px from the [border box](css-box-4.md#border-box) and into the [margin area](css-box-4.md#margin-area). If the border-image fails to load (or if border images are not supported by the UA), the fallback rendering uses a green double border.

<a id="border-image-border-shorthand-example"></a> Notice that the [border](#propdef-border) shorthand resets [border-image](#propdef-border-image). This makes it easy to turn off or reset all border effects:

```
.notebox {
  border: double orange;
  /* must set 'border' shorthand first, otherwise it erases 'border-image' */
  border-image: url("border.png") 30 round;
  /* but other 'border' properties can be set after */
  border-width: thin thick;
}
...
.sidebar .notebox {
  box-shadow: 0 0 5px gray;
  border-radius: 5px;
  border: none; /* turn off all borders */
  /* 'border' shorthand resets 'border-image' */
}
```

### <a id="border-image-source"></a>5.1. <a id="the-border-image-source"></a> Image Source: the [border-image-source](#propdef-border-image-source) property

| Name: | *<a id="propdef-border-image-source"></a>border-image-source* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | none [\|](css-values-4.md#comb-one) [<image>](css-images-3.md#typedef-image) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | All elements, except internal table elements when border-collapse is collapse |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | the keyword none or the computed [<image>](css-images-3.md#typedef-image) |
| Canonical order: | per grammar |
| Animation type: | discrete |

Specifies an image to use as a border in place of the rendering specified by the [border-style](#propdef-border-style) properties and, if given the [fill](#border-image-slice-fill) keyword in [border-image-slice](#propdef-border-image-slice), as an additional image backdrop for the element. If the value is none or if the image cannot be displayed (or the property doesn’t apply), the border styles will be used; otherwise the element’s border-style borders are not drawn and this *<a id="border-image-dfn"></a>border image* is drawn as described in the sections below.

### <a id="border-image-slice"></a>5.2. <a id="the-border-image-slice"></a> Image Slicing: the [border-image-slice](#propdef-border-image-slice) property

| Name: | *<a id="propdef-border-image-slice"></a>border-image-slice* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [[<number [0,∞]>](css-values-4.md#number-value) [\|](css-values-4.md#comb-one) [<percentage [0,∞]>](css-values-4.md#percentage-value)][{1,4}](css-values-4.md#mult-num-range) [&&](css-values-4.md#comb-all) fill[?](css-values-4.md#mult-opt) |
| [Initial:](css-cascade-5.md#initial-values) | 100% |
| [Applies to:](css-cascade-5.md#applies-to) | All elements, except internal table elements when border-collapse is collapse |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to size of the border image |
| [Computed value:](css-cascade-5.md#computed) | four values, each either a number or percentage; plus a [fill](#border-image-slice-fill) keyword if specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |

This property specifies inward offsets from the top, right, bottom, and left edges of the image, dividing it into nine regions: four corners, four edges and a middle. The middle image part is discarded (treated as fully transparent) unless the [fill](#border-image-slice-fill) keyword is present. (It is drawn over the background; see [Drawing the Border Image](#border-image-process).)

If there is only one component value, it applies to all sides. If there are two values, the top and bottom are set to the first value and the right and left are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

***<a id="valdef-border-image-slice-percentage-0"></a>[<percentage [0,∞]>](css-values-4.md#percentage-value)* **
: Percentages are relative to the size of the image: the width of the image for the horizontal offsets, the height for vertical offsets.
***<a id="valdef-border-image-slice-number-0"></a>[<number [0,∞]>](css-values-4.md#number-value)* **
: Numbers represent pixels in the image (if the image is a raster image) or vector coordinates (if the image is a vector image).
***<a id="border-image-slice-fill"></a>fill* **
: The [fill](#border-image-slice-fill) keyword, if present, causes the middle part of the border-image to be preserved. (By default it is discarded, i.e., treated as empty.)
Negative values are invalid. Computed values larger than the size of the image are interpreted as 100%.

The regions given by the [border-image-slice](#propdef-border-image-slice) values may overlap. However if the sum of the right and left widths is equal to or greater than the width of the image, the images for the top and bottom edge and the middle part are empty—​which has the same effect as if a nonempty transparent image had been specified for those parts. Analogously for the top and bottom values.

If the image must be sized to determine the slices (for example, for SVG images with no [natural dimensions](css-images-3.md#natural-dimensions)), then it is sized using the default sizing algorithm with no [specified size](css-images-3.md#specified-size) and the [border image area](#border-image-area) as the [default object size](css-images-3.md#default-object-size).

[image: Diagram: two horizontal cuts and two vertical cuts through an image]

Diagram illustrating the cuts corresponding to the value 25% 30% 12% 20%

### <a id="border-image-width"></a>5.3. <a id="the-border-image-width"></a> Drawing Areas: the [border-image-width](#propdef-border-image-width) property

| Name: | *<a id="propdef-border-image-width"></a>border-image-width* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [ [<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage) [\|](css-values-4.md#comb-one) [<number [0,∞]>](css-values-4.md#number-value) \| auto ][{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | 1 |
| [Applies to:](css-cascade-5.md#applies-to) | All elements, except internal table elements when border-collapse is collapse |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | Relative to width/height of the [border image area](#border-image-area) |
| [Computed value:](css-cascade-5.md#computed) | four values, each either a number, the keyword [auto](#valdef-border-image-width-auto), or a computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value |
| Canonical order: | per grammar |
| Animation type: | by computed value |

The [border image](#border-image-dfn) is drawn inside an area called the *<a id="border-image-area"></a>border image area*. This is an area whose boundaries by default correspond to the [border box](css-box-4.md#border-box), see [border-image-outset](#propdef-border-image-outset).

The four values of [border-image-width](#propdef-border-image-width) specify offsets that are used to divide the [border image area](#border-image-area) into nine *<a id="border-image-region"></a>regions*. The offsets represent inward distances from the top, right, bottom, and left sides of the area, respectively.

If there is only one component value, it applies to all sides. If there are two values, the top and bottom are set to the first value and the right and left are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

Values have the following meanings:

***<a id="valdef-border-image-width-length-percentage-0"></a>[<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage)* **
: Percentages refer to the size of the [border image area](#border-image-area): the width of the area for horizontal offsets, the height for vertical offsets.
***<a id="valdef-border-image-width-number-0"></a>[<number [0,∞]>](css-values-4.md#number-value)* **
: Numbers represent multiples of the corresponding computed [border-width](#border-width).
***<a id="valdef-border-image-width-auto"></a>auto* **
: If [auto](#valdef-border-image-width-auto) is specified then the used [border-image-width](#propdef-border-image-width) is the [natural](css-images-3.md#natural-size) width or height (whichever is applicable) of the corresponding image slice (see [border-image-slice](#propdef-border-image-slice)). If the image does not have the required [natural dimension](css-images-3.md#natural-dimensions) then the corresponding computed [border-width](#propdef-border-width) is used instead.
Negative values are invalid for any [border-image-width](#propdef-border-image-width) values.

If two opposite [border-image-width](#propdef-border-image-width) offsets are large enough that they overlap, then the [used values](css-cascade-5.md#used-value) of all border-image-width offsets are proportionally reduced until they no longer overlap. In mathematical notation: Given *L_width* as the width of the [border image area](#border-image-area), *L_height* as its height, and *W_*side** as the border-image-width offset for the *side* side, let *f* = min(*L_width*/(*W_left*+*W_right*), *L_height*/(*W_top*+*W_bottom*)). If *f* < 1, then all *W* are reduced by multiplying them by *f*.

### <a id="border-image-outset"></a>5.4. <a id="the-border-image-outset"></a> Edge Overhang: the [border-image-outset](#propdef-border-image-outset) property

| Name: | *<a id="propdef-border-image-outset"></a>border-image-outset* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [ [<length [0,∞]>](css-values-4.md#length-value) [\|](css-values-4.md#comb-one) [<number [0,∞]>](css-values-4.md#number-value) ][{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | All elements, except internal table elements when border-collapse is collapse |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | four values, each a number or absolute length |
| Canonical order: | per grammar |
| Animation type: | by computed value |

The values specify the amount by which the [border image area](#border-image-area) extends beyond the [border box](css-box-4.md#border-box).

If there is only one component value, it applies to all sides. If there are two values, the top and bottom are set to the first value and the right and left are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

**[<length [0,∞]>](css-values-4.md#length-value) **
: Represents an outset of the specified length.
**[<number [0,∞]>](css-values-4.md#number-value) **
: Represents an outset of the specified multiple of the corresponding computed [border-width](#border-width).
Negative values are invalid.

Portions of the border-image that are rendered outside the [border box](css-box-4.md#border-box) are ink overflow and do not trigger scrolling. Also such portions are invisible to mouse events and do not capture such events on behalf of the element.

Note: Even though they never cause a scrolling mechanism, outset images may still be clipped by an ancestor or by the viewport.

### <a id="border-image-repeat"></a>5.5. <a id="the-border-image-repeat"></a> Image Tiling: the [border-image-repeat](#propdef-border-image-repeat) property

| Name: | *<a id="propdef-border-image-repeat"></a>border-image-repeat* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [ stretch [\|](css-values-4.md#comb-one) repeat \| round \| space ][{1,2}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | stretch |
| [Applies to:](css-cascade-5.md#applies-to) | All elements, except internal table elements when border-collapse is collapse |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | two keywords, one per axis |
| Canonical order: | per grammar |
| Animation type: | discrete |

This property specifies how the images for the sides and the middle part of the [border image](#border-image-dfn) are scaled and tiled. The first keyword applies to the horizontal scaling and tiling of the top, middle and bottom parts, the second to the vertical scaling and tiling of the left, middle and right parts; see [Drawing the Border Image](#border-image-process). If the second keyword is absent, it is assumed to be the same as the first. Values have the following meanings:

***<a id="valdef-border-image-repeat-stretch"></a>stretch* **
: The image is stretched to fill its corresponding [region](#border-image-region).
***<a id="valdef-border-image-repeat-repeat"></a>repeat* **
: The image is tiled (repeated) to fill its corresponding [region](#border-image-region).
***<a id="valdef-border-image-repeat-round"></a>round* **
: The image is tiled (repeated) to fill its corresponding [region](#border-image-region). If it does not fill the area with a whole number of tiles, the image is rescaled so that it does.
***<a id="valdef-border-image-repeat-space"></a>space* **
: The image is tiled (repeated) to fill its corresponding [region](#border-image-region). If it does not fill the region with a whole number of tiles, the extra space is distributed around the tiles.
The exact process for scaling and tiling the border-image parts is given in the section below.

### <a id="border-image-process"></a>5.6. Drawing the Border Image

After the [border image](#border-image-dfn) given by [border-image-source](#propdef-border-image-source) is sliced by the [border-image-slice](#propdef-border-image-slice) values, the resulting nine images are scaled, positioned, and tiled into their corresponding [border image regions](#border-image-region) in four steps:

1. Scale to [border-image-width](#propdef-border-image-width).
  - The two images for the top and bottom edges are made as tall as the top and bottom [border image regions](#border-image-region), respectively, and their width is scaled proportionally.
  - The images for the left and right edges are made as wide as the left and right [border image regions](#border-image-region), respectively, and their height is scaled proportionally.
  - The corner images are scaled to be as wide and as tall as the their respective [border image regions](#border-image-region).
  - The middle image’s width is scaled by the same factor as the top image unless that factor is zero or infinity, in which case the scaling factor of the bottom is substituted, and failing that, the width is not scaled. The height of the middle image is scaled by the same factor as the left image unless that factor is zero or infinity, in which case the scaling factor of the right image is substituted, and failing that, the height is not scaled.
2. Scale to [border-image-repeat](#propdef-border-image-repeat).
  - If the first keyword is [stretch](#valdef-border-image-repeat-stretch), the top, middle and bottom images are further scaled to be as wide as the middle region of the [border image area](#border-image-area). The height is not changed any further.
  - If the first keyword is [round](#valdef-border-image-repeat-round), the top, middle and bottom images are resized in width, so that exactly a whole number of them fit in the middle region of the [border image area](#border-image-area), exactly as for [round](#valdef-background-repeat-round) in the [background-repeat](#propdef-background-repeat) property.
  - If the first keyword is [repeat](#valdef-border-image-repeat-repeat) or [space](#valdef-border-image-repeat-space), the top, middle, and bottom images are not changed any further.
  - The effects of [stretch](#valdef-border-image-repeat-stretch), [round](#valdef-border-image-repeat-round), [repeat](#valdef-border-image-repeat-repeat), and [space](#valdef-border-image-repeat-space) for the second keyword are analogous, acting on the height of the left, middle and right images.
3. Position the first tile.
  - If the first keyword is [repeat](#valdef-border-image-repeat-repeat), the top, middle, and bottom images are centered horizontally in their respective regions. Otherwise the images are placed at the left edge of their respective regions of the [border image area](#border-image-area).
  - If the second keyword is [repeat](#valdef-border-image-repeat-repeat), the left, middle, and right images are centered vertically in their respective regions. Otherwise the images are placed at the top edge of their respective regions of the [border image area](#border-image-area).
4. Tile and draw.
  - The images are then tiled to fill their respective regions.
  - In the case of [space](#valdef-border-image-repeat-space), any partial tiles are discarded and the extra space distributed before, after, and between the tiles. (I.e. the gap before the first tile, the gap after the last tile, and the gaps between tiles are equalized.) This can result in empty border-image side regions.
  - The images are drawn at the same stacking level as normal borders: immediately in front of the background layers.
  - The middle image is not drawn unless [fill](#border-image-slice-fill) was specified for [border-image-source](#propdef-border-image-source).

### <a id="border-image"></a>5.7. <a id="the-border-image"></a> Border Image Shorthand: the [border-image](#propdef-border-image) property

| Name: | *<a id="propdef-border-image"></a>border-image* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'border-image-source'>](#propdef-border-image-source) [\|\|](css-values-4.md#comb-any) [<'border-image-slice'>](#propdef-border-image-slice) [ / [<'border-image-width'>](#propdef-border-image-width) [\|](css-values-4.md#comb-one) / <'border-image-width'>[?](css-values-4.md#mult-opt) / [<'border-image-outset'>](#propdef-border-image-outset) ]? \|\| [<'border-image-repeat'>](#propdef-border-image-repeat) |
| [Initial:](css-cascade-5.md#initial-values) | See individual properties |
| [Applies to:](css-cascade-5.md#applies-to) | See individual properties |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | See individual properties |
| Canonical order: | per grammar |
| Animation type: | See individual properties |

This is a shorthand property for setting [border-image-source](#propdef-border-image-source), [border-image-slice](#propdef-border-image-slice), [border-image-width](#propdef-border-image-width), [border-image-outset](#propdef-border-image-outset), and [border-image-repeat](#propdef-border-image-repeat) in a single declaration. Omitted values are set to their [initial values](css-cascade-5.md#initial-value).

### <a id="border-image-tables"></a>5.8. Effect on Tables

The [border-image](#propdef-border-image) properties apply to the border of tables and inline tables that have border-collapse set to collapse. However, this specification does not define how such an image border is rendered. In particular, it does not define how the image border interacts with the borders of cells, rows and row groups at the edges of the table (see border conflict resolution in [[CSS2]](#biblio-css2)).

It is expected that a future specification will define the rendering. It is recommended that UAs do not apply border images to tables with collapsed borders until then.

## <a id="misc"></a>6. Miscellaneous Effects

Note: The box-decoration-break property, which defines how backgrounds and borders apply to a fragmented box, has been moved to the CSS Fragmentation Module. [[CSS-BREAK-3]](#biblio-css-break-3)

### <a id="box-shadow"></a>6.1. <a id="the-box-shadow"></a>Drop Shadows: the [box-shadow](#propdef-box-shadow) property

| Name: | *<a id="propdef-box-shadow"></a>box-shadow* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | none [\|](css-values-4.md#comb-one) [<shadow>](#typedef-shadow)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | either the keyword [none](#box-shadow-none) or a list, each item consisting of four absolute lengths plus a computed color and optionally also a [inset](#shadow-inset) keyword |
| Canonical order: | per grammar |
| Animation type: | by computed value, treating [none](#box-shadow-none) as a zero-item list and appending blank shadows (transparent 0 0 0 0) with a corresponding [inset](#shadow-inset) keyword as needed to match the longer list if the shorter list is otherwise compatible with the longer one |

The [box-shadow](#propdef-box-shadow) property attaches one or more drop shadows to the box. The property accepts either the *<a id="box-shadow-none"></a>none* value, which indicates no shadows, or a comma-separated list of shadows, ordered front to back.

Each shadow is given as a [<shadow>](#typedef-shadow), represented by 2-4 length values, an optional color, and an optional [inset](#shadow-inset) keyword. Omitted lengths are 0; omitted colors default to [currentColor](css-color-4.md#valdef-color-currentcolor).

```
<a id="typedef-shadow"></a><shadow> = <color>? && [ <length>{2} [ <length [0,∞]> <length>? ]? ] && inset?
```

The components of each [<shadow>](#typedef-shadow) are interpreted as follows:

***<a id="shadow-offset-x"></a>1st [<length>](css-values-4.md#length-value)* **
: Specifies the *<a id="box-shadow-horizontal-offset"></a>horizontal offset* of the shadow. A positive value draws a shadow that is offset to the right of the box, a negative length to the left.
***<a id="shadow-offset-y"></a>2nd [<length>](css-values-4.md#length-value)* **
: Specifies the *<a id="box-shadow-vertical-offset"></a>vertical offset* of the shadow. A positive value offsets the shadow down, a negative one up.
***<a id="shadow-blur-radius"></a>3rd [<length [0,∞]>](css-values-4.md#length-value)* **
: Specifies the *<a id="box-shadow-blur-radius"></a>blur radius*. Negative values are invalid. If the blur value is zero, the shadow’s edge is sharp. Otherwise, the larger the value, the more the shadow’s edge is blurred. See [Shadow Blurring](#shadow-blur), below.
***<a id="shadow-spread-distance"></a>4th [<length>](css-values-4.md#length-value)* **
: Specifies the *<a id="box-shadow-spread-distance"></a>spread distance*. Positive values cause the shadow to expand in all directions by the specified radius. Negative values cause the shadow to contract. See [Shadow Shape](#shadow-shape), below.

Note that for inner shadows, expanding the shadow (creating more shadow area) means contracting the shadow’s perimeter shape.

***<a id="shadow-color"></a><color>* **
: Specifies the color of the shadow. If the color is absent, it defaults to [currentColor](css-color-4.md#valdef-color-currentcolor).
***<a id="shadow-inset"></a>inset* **
: If present, the [inset](#shadow-inset) keyword changes the drop shadow from an *<a id="box-shadow-outer-box-shadow"></a>outer box-shadow* (one that shadows the box onto the canvas, as if it were lifted above the canvas) to an *<a id="box-shadow-inner-box-shadow"></a>inner box-shadow* (one that shadows the canvas onto the box, as if the box were cut out of the canvas and shifted behind it).
<a id="example-ad571670"></a> The example below demonstrates the effects of spread and blur on the shadow:

```
width: 100px; height: 100px;
border: 12px solid blue; background-color: orange;
border-top-left-radius: 60px 90px;
border-bottom-right-radius: 60px 90px;
box-shadow: 64px 64px 12px 40px rgba(0,0,0,0.4),
            12px 12px 0px 8px rgba(0,0,0,0.4) inset;
```

[image: The sample code above would create a 100px×100px orange box with a 12px blue border, whose top right / bottom left corners are sharp and tob left / bottom right corners are elliptically curved. Two shadows are created: an inner one, which due to its offset and spread creates a 20px-wide band of darker orange along the top and left sides of the box (curving to match the rounded top left border shape); and an outer one, creating a 204px×204px gray duplicate of the shape seemingly behind the box, offset 24px down and 24px to the right of the box's top and left edges. Applying the 12px blur radius to the outer shadow creates a gradual shift from the shadow color to transparent along its edges which is visibly apparent for 24px centered along the edge of the shadow.]

#### <a id="shadow-shape"></a>6.1.1. Shadow Shape, Spread, and Knockout

An [outer box-shadow](#box-shadow-outer-box-shadow) casts a shadow as if the border-box of the element were opaque. Assuming a spread distance of zero, its perimeter has the exact same size and shape as the border box. The shadow is drawn outside the border edge only: it is clipped inside the border-box of the element.

An [inner box-shadow](#box-shadow-inner-box-shadow) casts a shadow as if everything outside the padding edge were opaque. Assuming a spread distance of zero, its perimeter has the exact same size and shape as the padding box. The shadow is drawn inside the padding edge only: it is clipped outside the padding box of the element.

If a [spread distance](#box-shadow-spread-distance) is defined, the shadow perimeter defined above is expanded outward (for [outer box-shadows](#box-shadow-outer-box-shadow)) or contracted inward (for [inner box-shadows](#box-shadow-inner-box-shadow)) by outsetting (insetting, for inner shadows) the shadow’s straight edges by the spread distance (and flooring the resulting width/height at zero).

<a id="example-96985467"></a> Below are some examples of an orange box with a blue border being given a drop shadow.

<a id="box-shadow-samples"></a>

```
       border:5px solid blue;
background-color:orange;
width: 144px;
height: 144px;
```

```
border-radius: 20px;
```

```
border-radius: 0;
```

```
       box-shadow:
  rgba(0,0,0,0.4)
  10px 10px;
```

```
       box-shadow:
  rgba(0,0,0,0.4)
  10px 10px
  inset
```

```
       box-shadow:
  rgba(0,0,0,0.4)
  10px 10px 0
  10px /* spread */
```

```
       box-shadow:
  rgba(0,0,0,0.4)
  10px 10px 0
  10px /* spread */
  inset
```

|  |  |  |
| --- | --- | --- |
|  | [image: A round-cornered box with a light gray shadow the same shape as the border box offset 10px to the right and 10px down from directly underneath the box.] | [image: A square-cornered box with a light gray shadow the same shape as the border box offset 10px to the right and 10px down from directly underneath the box.] |
|  | [image: A round-cornered box with a light gray shadow the inverse shape of the padding box filling 10px in from the top and left edges (just inside the border).] | [image: A square-cornered box with a light gray shadow the inverse shape of the padding box filling 10px in from the top and left edges (just inside the border).] |
|  | [image: A round-cornered box with a light gray shadow the same shape as the box but 20px taller and wider and offset so that the top and left edges of the shadow are directly underneath the top and left edges of the box.] | [image: A square-cornered box with a light gray shadow the same shape as the box but 20px taller and wider and offset so that the top and left edges of the shadow are directly underneath the top and left edges of the box.] |
|  | [image: A round-cornered box with a light gray shadow the inverse shape of the box but 20px narrower and shorter filling 20px in from the top and left edges (just inside the border).] | [image: A round-cornered box with a light gray shadow the inverse shape of the box but 20px narrower and shorter filling 20px in from the top and left edges (just inside the border).] |

To preserve the box’s shape when spread is applied, the corner radii of the shadow are also increased (decreased, for inner shadows) from the border-box (padding-box) radii by adding (subtracting) the [spread distance](#box-shadow-spread-distance) (and flooring at zero). For outer shadows, the [border radius](#border-radii) is then [adjusted](#outset-adjusted-border-radius), independently in each dimension, to preserve the sharpness of rounded corners.

The [border-image](#propdef-border-image) does not affect the shape of the box-shadow.

#### <a id="shadow-blur"></a>6.1.2. Blurring Shadow Edges

A non-zero [blur radius](#box-shadow-blur-radius) indicates that the resulting shadow should be blurred, such as by a Gaussian filter. The exact algorithm is not defined; however the resulting shadow must approximate (with each pixel being within 5% of its expected value) the image that would be generated by applying to the shadow a Gaussian blur with a standard deviation equal to half the blur radius.

Note: This means for a long, straight shadow edge, the blur radius will create a visibly apparent color transition approximately the twice length of the blur radius that is perpendicular to and centered on the shadow’s edge, and that ranges from almost the full shadow color at the endpoint inside the shadow to almost fully transparent at the endpoint outside it.

#### <a id="shadow-layers"></a>6.1.3. Layering, Layout, and Other Details

The shadow effects are applied front-to-back: the first shadow is on top and the others are layered behind. Shadows do not influence layout and may overlap (or be overlapped by) other boxes and text or their shadows. In terms of stacking contexts and the painting order, the [outer box-shadows](#box-shadow-outer-box-shadow) of an element are drawn immediately below the background of that element, and the [inner shadows](#box-shadow-inner-box-shadow) of an element are drawn immediately above the background of that element (below the borders and border image, if any).

Unless otherwise specified, drop shadows are only applied to the principal box. If the affected box has multiple fragments, the shadows are applied as specified in box-decoration-break.

Shadows are ink overflow; they do not trigger scrolling or increase the size of the scrollable overflow area.

Outer shadows have no effect on internal table elements in the collapsing border model. If a shadow is defined for single border edge in the collapsing border model that has multiple border thicknesses (e.g. an outer shadow on a table where one row has thicker borders than the others, or an inner shadow on a rowspanning table cell that adjoins cells with different border thicknesses), the exact position and rendering of its shadows are undefined

## <a id="levels"></a>7. Levels

*This section is informative.* CSS has different levels of features, each a subset of the other. (See [[CSS-2017]](#biblio-css-2017) for a full explanation.) The lists below describe which features from this specification are in each level.

### <a id="level-1"></a>7.1. Level 1

- [background-color](#propdef-background-color)
- [background-image](#propdef-background-image) only one image (no layers)
- [background-repeat](#propdef-background-repeat): only [repeat](#valdef-background-repeat-repeat) | [repeat-x](#valdef-background-repeat-repeat-x) | [repeat-y](#valdef-background-repeat-repeat-y) | [no-repeat](#valdef-background-repeat-no-repeat)
- [background-attachment](#propdef-background-attachment): only [scroll](#valdef-background-attachment-scroll) | [fixed](#valdef-background-attachment-fixed)
- [background-position](#propdef-background-position): only one or two values allowed
- [background](#propdef-background) shorthand: only color, image, repeat, attachment and position
- [border-color](#propdef-border-color) properties
- [border-style](#propdef-border-style) properties
- [border-width](#propdef-border-width) properties
- [border-top](#propdef-border-top), [border-bottom](#propdef-border-bottom), [border-right](#propdef-border-right), [border-left](#propdef-border-left), and [border](#propdef-border) shorthands

### <a id="level-2"></a>7.2. Level 2

- [background-color](#propdef-background-color)
- [background-image](#propdef-background-image): only one image (no layers)
- [background-repeat](#propdef-background-repeat): only [repeat](#valdef-background-repeat-repeat) | [repeat-x](#valdef-background-repeat-repeat-x) | [repeat-y](#valdef-background-repeat-repeat-y) | [no-repeat](#valdef-background-repeat-no-repeat)
- [background-attachment](#propdef-background-attachment): only [scroll](#valdef-background-attachment-scroll) | [fixed](#valdef-background-attachment-fixed)
- [background-position](#propdef-background-position): only one or two values allowed
- [background](#propdef-background): only color, image, repeat, attachment and position
- [border-color](#propdef-border-color) properties
- [border-style](#propdef-border-style) properties
- [border-width](#propdef-border-width) properties
- [border-top](#propdef-border-top), [border-bottom](#propdef-border-bottom), [border-right](#propdef-border-right), [border-left](#propdef-border-left), and [border](#propdef-border) shorthands

### <a id="level-3"></a>7.3. Level 3

- All features described in the CSS Backgrounds and Borders Module Level 3

## <a id="changes"></a>8. Changes

### <a id="changes-2024-03"></a>8.1. Changes since the 11 March 2024 Candidate Recommendation Draft

- Changed the special case of [border-width](#propdef-border-width) having a value of 0 when [border-style](#propdef-border-style) is [none](#valdef-line-style-none) or [hidden](#valdef-line-style-hidden) from a computed-value effect to a resolved-value effect. (Issue 11494)
- Altered the border-radius outset adjustment, so the radius reduction factor in the minimum radius percentage of the corner, makes it less pronounced the closer the corner’s border-radius gets to 50%. (Issue 7103)
- Fixed syntax of [box-shadow](#propdef-box-shadow) to avoid allowing negative blur radii, aligning it with the prose.
- Cross linked to 'ink overflow' terminology

### <a id="changes-2023-02"></a>8.2. Changes since the 14 February 2023 Candidate Recommendation Snapshot

- Defined serialization of [background-position](#propdef-background-position) in [§ 2.6.1 Serialization of background-position values](#bg-position-serialization). (Issue 2274)
- Reverted [change moving <color> to the front of <final-bg-layer>](#bg-color-move) in the [background](#propdef-background) shorthand. (Issue 8496)
- Specified [rounding of border widths](css-values-4.md#snap-as-a-line-width) to device pixels. (Issue 7434)
- Specified interaction of [border-radius](#propdef-border-radius) and per-axis overflow longhands. (Issue 5210)
- Fixed an error in the Computed Value line of [background-image](#propdef-background-image). (Issue 8604)
- Removed the <box> definition (replacing it with a [<visual-box>](css-box-4.md#typedef-visual-box) reference), as the Box Module now defines these terms.
- Aligned property definition tables with the latest expectations to include “Animation Type” and “Logical Property Group”.
- Streamlined property grammar definitions using the latest [value definition syntax](css-values-4.md#css-value-definition-syntax).
- Fixed the definition for where box shadows apply. (Issue 9286)
- Defined serialization of [background-size](#propdef-background-size) in [§ 2.9.1 Serialization of background-size values](#bg-size-serialization). (Issue 7802)

### <a id="changes-2020-12"></a>8.3. Changes since the 22 December 2020 Candidate Recommendation Snapshot

- Clarified that the rule about not propagating backgrounds from the root when it doesn’t generate boxes only applies to display: none, not display: contents. (Issue 3779)
- Precisely defined the size of [border-width](#propdef-border-width) [thin](#valdef-line-width-thin), [medium](#valdef-line-width-medium), and [thick](#valdef-line-width-thick). (Issue 7254)
- Minor editorial improvements.

### <a id="changes-2017-10"></a>8.4. Changes since the 17 October 2017 Candidate Recommendation

- Inverted order of [<shadow>](#typedef-shadow) grammar to match browser serialization and text-shadow/[drop-shadow()](filter-effects-1.md#funcdef-filter-drop-shadow). (Issue 2305)
```
<shadow> = inset<color>? && <length>{2,4} && <color>inset?
```

- Spread radius adjustment is only applied to shadows and margins where the radius of curvature grows, rather than shrinks. (Issue 1900)
To preserve the box’s shape when spread is applied, the corner radii of the shadow are also increased (decreased, for inner shadows) from the border-box radii by adding (subtracting) the [spread distance](#box-shadow-spread-distance) (and flooring at zero). However, in order to create a sharper corner when the border radius is small (and thus ensure continuity between round and sharp corners) , when the [border radius](#border-radii) is less than the spread distance (or in the case of an inner shadow, less than the absolute value of a negative [spread distance](#box-shadow-spread-distance)) , the spread distance is multiplied by the proportion 1 + (*r*-1)^3, where *r* is the ratio of the border radius to the spread distance, in calculating the corner radii of the spread shadow shape.
- Clarified that an omitted <color> in a [<shadow>](#typedef-shadow) defaults to [currentColor](css-color-4.md#valdef-color-currentcolor), not some mysterious unnamed value with the same behavior. (2766)
If the color is absent, it defaults to [currentColor](css-color-4.md#valdef-color-currentcolor) the used color is taken from the [color](css-color-4.md#propdef-color) property.
- Cleaned up and regularized “Animation type” and “Computed value” lines in the property definition tables.
- Changed syntax to use the new [CSS bracketed range notation](css-values-4.md#css-bracketed-range-notation) to reflect the prose restrictions on negative values, and corrected a few grammar definition errors introduced in the previous publication.
- Minor editorial improvements.

### <a id="changes-2014-09"></a>8.5. Changes since the 9 September 2014 Candidate Recommendation

The following (non-trivial) changes were made to this specification since the 9 September 2014 Candidate Recommendation:

- Added missing definition of [box-shadow: none](#propdef-box-shadow). (Apparently this was obvious enough that nobody noticed it was missing until now.)
- <a id="bg-color-move"></a>Moved [<'background-color'>](#propdef-background-color) component of [<final-bg-layer>](#typedef-final-bg-layer) to the front for serialization because some authors seem to expect this even though it makes less sense?
- Dropped effect of [border-radius](#propdef-border-radius) from collapsed-borders tables.
- Renamed <position> back to [<bg-position>](#typedef-bg-position) since other properties will be eliding the three-value syntax.

### <a id="changes-2014-02"></a>8.6. Changes since the 4 February 2014 Last Call Working Draft

The following (non-trivial) changes were made to this specification since the 4 February 2014 Last Call Working Draft:

- Fixed spread radius and margin radius calculations to only apply adjustment factor when spread/margin is larger than border radius.
- Defined handling of canvas background when root element has display: none.

A full Disposition of Comments is available.

### <a id="changes-2012-07"></a>8.7. Changes since the 24 July 2012 Candidate Recommendation

The following (non-trivial) changes were made to this specification since the 24 July 2012 Candidate Recommendation:

- Allow [<'background-clip'>](#propdef-background-clip) and [<'background-origin'>](#propdef-background-origin) to be separated by other component values in the [background](#propdef-background) shorthand, since this is what is implemented.
- Allow <color> and [inset](#shadow-inset) to be interleaved in any order in [box-shadow](#propdef-box-shadow), since they are not ambiguous and CSS generally allows variant ordering where not ambiguous.
- Define gradually increasing corner radius formula for [box-shadow](#propdef-box-shadow) spread curvature to create continuity between sharp corners ([border-radius](#propdef-border-radius) = 0) and curved corners (border-radius > *spread distance*). This also gives better results for all intermediate states.
- Add definition for how the margin edge is curved in response to border-radius. (This is relevant for [[CSS-SHAPES]](#biblio-css-shapes), but does not change conformance to CSS Backgrounds and Borders Level 3.)
- Removed box-decoration-break; it is now part of [[CSS-BREAK-3]](#biblio-css-break-3).
- Tighten up the definition of spread for [box-shadow](#propdef-box-shadow).
- Clarify that a [border-style](#propdef-border-style) of [none](#valdef-line-style-none) also implies an initial [border-image-width](#propdef-border-image-width) of zero (since border-image-width is initially set to the computed [border-width](#propdef-border-width), which in this case is zero).
- Clarified how [background-attachment: local](#propdef-background-attachment) is affected by scrolling.
- Simplified computed value of [background-position](#propdef-background-position) to clarify that all background-position values are interpolable.
- Added “Animation Type” values to each property definition table.

### <a id="changes-2012-04"></a>8.8. Changes since the 17 April 2012 Candidate Recommendation

The following (non-editorial) changes were made to this specification since the 17 April 2012 Candidate Recommendation:

- Fix error in computed value of [background-position](#propdef-background-position): the computed value is a list of positions.
- Add a note pointing out that when [border-radius](#propdef-border-radius) reduces the interactive area, the width/height of the box might need to be increased.

### <a id="changes-2012LC"></a>8.9. Changes since the 14 February 2012 “Last Call” Working Draft

The following (non-editorial) changes were made to this specification since the 14 February 2012 “Last Call” Working Draft:

These changes were in response to comments received during the review period. For details, see the full Disposition of Comments.

- [Section 2.1](#placement): Defined which properties from this module apply to the ::first-line and ::first-letter pseudo-elements.
- [Section 3.4](#background-repeat): Fixed the incorrect definition of the computed value of [background-repeat](#propdef-background-repeat). The value is always a pair of keywords, never a single keyword.
- [Section 3.10](#background-image): Added the missing [background-image](#propdef-background-image) to the list of properties that the [background](#propdef-background) property reset.
- [Section 4.3](#border-width): Added that negative lengths are invalid on [border-width](#propdef-border-width).
- [Section 5.1](#border-radius): Added that negative lengths are invalid on [border-radius](#propdef-border-radius).
- [Section 6.8](#border-image-tables): Added a section about the effect of [border-image](#propdef-border-image) on tables with collapsed borders and added that the rendering will be defined later.

### <a id="changes-2011"></a>8.10. Changes Since the 15 February 2011 Candidate Recommendation

The following changes were made to this specification since the 15 February 2011 Candidate Recommendation:

- [Section 3.9](#background-size): Defined what happens if [background-size](#propdef-background-size) has two [auto](#valdef-background-size-auto) values and the image is missing a natural size.
If both values are [auto](#valdef-background-size-auto) then the natural width and/or height of the image should be used, if any , the missing dimension (if any) behaving as ‘auto’ as described above .
- [Section 5.4](#corner-transitions): Made center of [color and style transitions](#corner-transitions) undefined (within certain limits) on corner joins, since previous definition was wrong.
The center of color and style transitions between adjoining borders is at the point on the curve that is at an angle that is proportional to the ratio of the border widths. For example, if the top and right border widths are equal, that point is at a 45° angle from the horizontal, and if the top is twice the width of the right the point is at a 30° angle from the horizontal. The line demarcating this transition is drawn between the point at that angle on the outer arc and the point at that angle on the inner arc. If one of these borders is zero-width, then the other border takes up the entire transitional area. Otherwise, the center of color and style transitions between adjoining borders must be proportional to the ratio of the border widths such that a function of its location is continuous with respect to this ratio. However it is not defined what these transitions look like or how “proportional” maps to a point on the curve.
- [Section 6.2](#border-image-slice): Defined [slicing of border images](#border-image-slice) that must be sized first to determine slice positions. Added:
If the image must be sized to determine the slices (for example, for SVG images with no natural size), then it is sized as for an auto-sized background, using the border image area as the default object size in place of the background positioning area.
- [Section 7.1](#misc) Optionally added bidi-imposed breaks to the types of breaks affected by box-decoration-break. Added:
UAs may also apply ‘box-decoration-break’ to control rendering at bidi-imposed breaks, i.e. when bidi reordering causes an inline to split into non-contiguous fragments. Otherwise such breaks are always handled as ‘slice’.
- [Section 7.2](#box-shadow): Defined the default color of [box-shadow](#propdef-box-shadow).
The color is the color of the shadow. If the color is absent, the used color is taken from the ‘color’ property.
- [Section 3.6](#background-position): Clarified computed value of [background-position](#propdef-background-position).
If one or two values are specified, for a <length> the absolute length, otherwise a percentage. If three or four values are specified, two pairs of a keyword plus a length or percentage. Two keywords representing the origin and two offsets from that origin, each given as an absolute length (if given a [<length>](css-values-4.md#length-value)), otherwise as a percentage.
Changed Equivalent to Computes in definition of keywords.
- Added "Animation Type" line to property definition tables.
- [Section 3.6](#background-position): Renamed <bg-position> production to <position> for easier re-use in other specifications and recast the grammar to be more verbose but easier to understand.
```
bg-position<position> = [
  [ left | center | right | top | bottom | <length-percentage> ]
|
  [ left | center | right | <length-percentage> ]
  [ top | center | bottom | <length-percentage> ]?
|
  [ center | [ left | right ] <length-percentage>? ] &&
  [ center | [ top | bottom ] <length-percentage>? ]
]
```

- [Section 6.5](#border-image-repeat): Added [space](#valdef-border-image-repeat-space) keyword to [border-image-repeat](#propdef-border-image-repeat) property value table: it was included in the list of allowable values, but not in the syntax definition.
[ stretch | repeat | round | space ]{1,2}
- [Section 5.5](#corner-overlap): Corrected math error in border-radius example.
The height (2.5em) is enough for the specified radii (0.5em plus 2.5em 2.0em ).
- [Section 4.3](#border-width): Marked Percentages field as N/A for [border-width](#propdef-border-width), since they are not included in the property.

### <a id="changes-2009"></a>8.11. Changes Since the 17 December 2009 Candidate Recommendation

The following changes were made to this specification since the 17 December 2009 Candidate Recommendation:

- Addition of [content-box](#valdef-background-clip-content-box) value of [background-clip](#propdef-background-clip).
- Change to the [background](#propdef-background) shorthand syntax for [background-clip](#propdef-background-clip) and [background-origin](#propdef-background-origin).
- Removal of recommendation to use gradients for color transitions when [border-radius](#propdef-border-radius) produces a curve.
- (Re)Addition of box-shadow property.
- Various clarifications.

## <a id="acknowledgments"></a>9. Acknowledgments

Tapas Roy was editor of the Border Module, before it was merged with the Background Module.

Thanks to Ben Stucki for defining what happens with rounded corners if the two adjoining borders are of unequal thickness or one of them is zero; to Arjan Eising and Anne van Kesteren for the [border-radius](#propdef-border-radius) syntax; to Zack Weinberg for the corner transition regions diagram; and to Lea Verou, plinss, and dbaron for the corner radius adjustment formula (with special thanks to Lea for the live demo).

A set of properties for border images was initially proposed by *fantasai*. The current simplification (one image cut into nine parts) is due to Ian Hickson. (Though the original idea seems to originate with some anonymous Microsoft engineers.)

Finally, special thanks go to Brad Kemper for his feedback and suggestions for many of the features in the draft, for drawing all the box-shadow examples, and for proposing some radical changes to the [border-image](#propdef-border-image) property that solved a number of problems with the earlier definition.

## <a id="privacy"></a>10. Privacy Considerations

This specification introduces no new privacy considerations.

## <a id="security"></a>11. Security Considerations

This specification introduces no new security considerations.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-2023"></a>[CSS-2023] **
: Chris Lilley; et al. CSS Snapshot 2023. URL: https://drafts.csswg.org/css-2023/
**<a id="biblio-css-box-3"></a>[CSS-BOX-3] **
: Elika Etemad. CSS Box Model Module Level 3. URL: https://drafts.csswg.org/css-box-3/
**<a id="biblio-css-box-4"></a>[CSS-BOX-4] **
: Elika Etemad. [CSS Box Model Module Level 4](css-box-4.md). URL: [https://drafts.csswg.org/css-box-4/](css-box-4.md)
**<a id="biblio-css-break-4"></a>[CSS-BREAK-4] **
: Rossen Atanassov; Elika Etemad. CSS Fragmentation Module Level 4. URL: https://drafts.csswg.org/css-break-4/
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-color-5"></a>[CSS-COLOR-5] **
: Chris Lilley; Una Kravets; Lea Verou. CSS Color Module Level 5. URL: https://drafts.csswg.org/css-color-5/
**<a id="biblio-css-display-4"></a>[CSS-DISPLAY-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Display Module Level 4. URL: https://drafts.csswg.org/css-display-4/
**<a id="biblio-css-images-3"></a>[CSS-IMAGES-3] **
: Tab Atkins Jr.; Elika Etemad; Lea Verou. [CSS Images Module Level 3](css-images-3.md). URL: [https://drafts.csswg.org/css-images-3/](css-images-3.md)
**<a id="biblio-css-overflow-3"></a>[CSS-OVERFLOW-3] **
: Elika Etemad; Florian Rivoal. CSS Overflow Module Level 3. URL: https://drafts.csswg.org/css-overflow-3/
**<a id="biblio-css-overflow-4"></a>[CSS-OVERFLOW-4] **
: David Baron; Florian Rivoal; Elika Etemad. CSS Overflow Module Level 4. URL: https://drafts.csswg.org/css-overflow-4/
**<a id="biblio-css-ruby-1"></a>[CSS-RUBY-1] **
: Elika Etemad; et al. CSS Ruby Annotation Layout Module Level 1. URL: https://drafts.csswg.org/css-ruby-1/
**<a id="biblio-css-sizing-3"></a>[CSS-SIZING-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Box Sizing Module Level 3. URL: https://drafts.csswg.org/css-sizing-3/
**<a id="biblio-css-text-decor-4"></a>[CSS-TEXT-DECOR-4] **
: Elika Etemad; Koji Ishii. CSS Text Decoration Module Level 4. URL: https://drafts.csswg.org/css-text-decor-4/
**<a id="biblio-css-values-3"></a>[CSS-VALUES-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-cssom-1"></a>[CSSOM-1] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-filter-effects-1"></a>[FILTER-EFFECTS-1] **
: Dirk Schulze; Dean Jackson. [Filter Effects Module Level 1](filter-effects-1.md). URL: [https://drafts.csswg.org/filter-effects-1/](filter-effects-1.md)
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
**<a id="biblio-mediaqueries-5"></a>[MEDIAQUERIES-5] **
: Tab Atkins Jr.; et al. Media Queries Level 5. URL: https://drafts.csswg.org/mediaqueries-5/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-selectors-3"></a>[SELECTORS-3] **
: Tantek Çelik; et al. Selectors Level 3. URL: https://drafts.csswg.org/selectors-3/
**<a id="biblio-selectors-4"></a>[SELECTORS-4] **
: Elika Etemad; Tab Atkins Jr.. [Selectors Level 4](selectors-4.md). URL: [https://drafts.csswg.org/selectors/](selectors-4.md)
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css-2017"></a>[CSS-2017] **
: Tab Atkins Jr.; Elika Etemad; Florian Rivoal. CSS Snapshot 2017. URL: https://drafts.csswg.org/css-2017/
**<a id="biblio-css-break-3"></a>[CSS-BREAK-3] **
: Rossen Atanassov; Elika Etemad. CSS Fragmentation Module Level 3. URL: https://drafts.csswg.org/css-break/
**<a id="biblio-css-contain-1"></a>[CSS-CONTAIN-1] **
: Tab Atkins Jr.; Florian Rivoal. CSS Containment Module Level 1. URL: https://drafts.csswg.org/css-contain-1/
**<a id="biblio-css-contain-2"></a>[CSS-CONTAIN-2] **
: Tab Atkins Jr.; Florian Rivoal; Vladimir Levin. CSS Containment Module Level 2. URL: https://drafts.csswg.org/css-contain-2/
**<a id="biblio-css-content-3"></a>[CSS-CONTENT-3] **
: Elika Etemad; Mike Bremford. CSS Generated Content Module Level 3. URL: https://drafts.csswg.org/css-content-3/
**<a id="biblio-css-shapes"></a>[CSS-SHAPES] **
: Alan Stearns; Rossen Atanassov; Noam Rosenthal. [CSS Shapes Module Level 1](css-shapes-1.md). URL: [https://drafts.csswg.org/css-shapes/](css-shapes-1.md)
**<a id="biblio-wcag20"></a>[WCAG20] **
: Ben Caldwell; et al. Web Content Accessibility Guidelines (WCAG) 2.0. 11 December 2008. REC. URL: https://www.w3.org/TR/WCAG20/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order | Com­puted value |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [background](#propdef-background) | <bg-layer>#? , <final-bg-layer> | see individual properties | all elements | no | see individual properties | see individual properties | per grammar | see individual properties |  |
| [background-attachment](#propdef-background-attachment) | <attachment># | scroll | all elements | no | N/A | discrete | per grammar | list, each item the keyword as specified |  |
| [background-clip](#propdef-background-clip) | <visual-box># | border-box | all elements | no | N/A | repeatable list | per grammar | list, each item a keyword as specified |  |
| [background-color](#propdef-background-color) | <color> | transparent | all elements | no | N/A | by computed value | per grammar | computed color |  |
| [background-image](#propdef-background-image) | <bg-image># | none | all elements | no | N/A | discrete | per grammar | list, each item either an <image> or the keyword none |  |
| [background-origin](#propdef-background-origin) | <visual-box># | padding-box | all elements | no | N/A | repeatable list | per grammar | list, each item a keyword as specified |  |
| [background-position](#propdef-background-position) | <bg-position># | 0% 0% | all elements | no | refer to size of background positioning area minus size of background image; see text | repeatable list | per grammar | list, each item a pair of offsets (horizontal and vertical) from the top left origin each given as a computed <length-percentage> value |  |
| [background-repeat](#propdef-background-repeat) | <repeat-style># | repeat | all elements | no | N/A | discrete | per grammar | list, each item a pair of keywords, one per dimension |  |
| [background-size](#propdef-background-size) | <bg-size># | auto | all elements | no | see text | repeatable list | per grammar | list, each item a pair of sizes (one per axis) each represented as either a keyword or a computed <length-percentage> value |  |
| [border](#propdef-border) | <line-width> \|\| <line-style> \|\| <color> | See individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-bottom](#propdef-border-bottom) | <line-width> \|\| <line-style> \|\| <color> | See individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-bottom-color](#propdef-border-bottom-color) | <color> | currentColor | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | computed color | border-color |
| [border-bottom-left-radius](#propdef-border-bottom-left-radius) | <length-percentage [0,∞]>{1,2} | 0 | all elements (but see prose) | no | Refer to corresponding dimension of the border box. | by computed value | per grammar | pair of computed <length-percentage> values | border-radius |
| [border-bottom-right-radius](#propdef-border-bottom-right-radius) | <length-percentage [0,∞]>{1,2} | 0 | all elements (but see prose) | no | Refer to corresponding dimension of the border box. | by computed value | per grammar | pair of computed <length-percentage> values | border-radius |
| [border-bottom-style](#propdef-border-bottom-style) | <line-style> | none | all elements except ruby base containers and ruby annotation containers | no | N/A | discrete | per grammar | specified keyword | border-style |
| [border-bottom-width](#propdef-border-bottom-width) | <line-width> | medium | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | absolute length, snapped as a border width | border-width |
| [border-color](#propdef-border-color) | <color>{1,4} | see individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-image](#propdef-border-image) | <'border-image-source'> \|\| <'border-image-slice'> [ / <'border-image-width'> \| / <'border-image-width'>? / <'border-image-outset'> ]? \|\| <'border-image-repeat'> | See individual properties | See individual properties | no | N/A | See individual properties | per grammar | See individual properties |  |
| [border-image-outset](#propdef-border-image-outset) | [ <length [0,∞]> \| <number [0,∞]> ]{1,4} | 0 | All elements, except internal table elements when border-collapse is collapse | no | N/A | by computed value | per grammar | four values, each a number or absolute length |  |
| [border-image-repeat](#propdef-border-image-repeat) | [ stretch \| repeat \| round \| space ]{1,2} | stretch | All elements, except internal table elements when border-collapse is collapse | no | N/A | discrete | per grammar | two keywords, one per axis |  |
| [border-image-slice](#propdef-border-image-slice) | [<number [0,∞]> \| <percentage [0,∞]>]{1,4} && fill? | 100% | All elements, except internal table elements when border-collapse is collapse | no | refer to size of the border image | by computed value | per grammar | four values, each either a number or percentage; plus a fill keyword if specified |  |
| [border-image-source](#propdef-border-image-source) | none \| <image> | none | All elements, except internal table elements when border-collapse is collapse | no | N/A | discrete | per grammar | the keyword none or the computed <image> |  |
| [border-image-width](#propdef-border-image-width) | [ <length-percentage [0,∞]> \| <number [0,∞]> \| auto ]{1,4} | 1 | All elements, except internal table elements when border-collapse is collapse | no | Relative to width/height of the border image area | by computed value | per grammar | four values, each either a number, the keyword auto, or a computed <length-percentage> value |  |
| [border-left](#propdef-border-left) | <line-width> \|\| <line-style> \|\| <color> | See individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-left-color](#propdef-border-left-color) | <color> | currentColor | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | computed color | border-color |
| [border-left-style](#propdef-border-left-style) | <line-style> | none | all elements except ruby base containers and ruby annotation containers | no | N/A | discrete | per grammar | specified keyword | border-style |
| [border-left-width](#propdef-border-left-width) | <line-width> | medium | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | absolute length, snapped as a border width | border-width |
| [border-radius](#propdef-border-radius) | <length-percentage [0,∞]>{1,4} [ / <length-percentage [0,∞]>{1,4} ]? | see individual properties | all elements (but see prose) | no | Refer to corresponding dimension of the border box. | see individual properties | per grammar | see individual properties |  |
| [border-right](#propdef-border-right) | <line-width> \|\| <line-style> \|\| <color> | See individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-right-color](#propdef-border-right-color) | <color> | currentColor | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | computed color | border-color |
| [border-right-style](#propdef-border-right-style) | <line-style> | none | all elements except ruby base containers and ruby annotation containers | no | N/A | discrete | per grammar | specified keyword | border-style |
| [border-right-width](#propdef-border-right-width) | <line-width> | medium | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | absolute length, snapped as a border width | border-width |
| [border-style](#propdef-border-style) | <line-style>{1,4} | see individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-top](#propdef-border-top) | <line-width> \|\| <line-style> \|\| <color> | See individual properties | all elements except ruby base containers and ruby annotation containers | no | N/A | see individual properties | per grammar | see individual properties |  |
| [border-top-color](#propdef-border-top-color) | <color> | currentColor | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | computed color | border-color |
| [border-top-left-radius](#propdef-border-top-left-radius) | <length-percentage [0,∞]>{1,2} | 0 | all elements (but see prose) | no | Refer to corresponding dimension of the border box. | by computed value | per grammar | pair of computed <length-percentage> values | border-radius |
| [border-top-right-radius](#propdef-border-top-right-radius) | <length-percentage [0,∞]>{1,2} | 0 | all elements (but see prose) | no | Refer to corresponding dimension of the border box. | by computed value | per grammar | pair of computed <length-percentage> values | border-radius |
| [border-top-style](#propdef-border-top-style) | <line-style> | none | all elements except ruby base containers and ruby annotation containers | no | N/A | discrete | per grammar | specified keyword | border-style |
| [border-top-width](#propdef-border-top-width) | <line-width> | medium | all elements except ruby base containers and ruby annotation containers | no | N/A | by computed value | per grammar | absolute length, snapped as a border width | border-width |
| [border-width](#propdef-border-width) | <line-width>{1,4} | see individual properties | all elements except ruby base containers and ruby annotation containers | no | see individual properties | see individual properties | per grammar | see individual properties |  |
| [box-shadow](#propdef-box-shadow) | none \| <shadow># | none | all elements | no | N/A | by computed value, treating none as a zero-item list and appending blank shadows (transparent 0 0 0 0) with a corresponding inset keyword as needed to match the longer list if the shorter list is otherwise compatible with the longer one | per grammar | either the keyword none or a list, each item consisting of four absolute lengths plus a computed color and optionally also a inset keyword |  |

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
