<!-- https://drafts.csswg.org/css-images-3/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Images Module Level 3

<a id="w3c-state"></a>Editor’s Draft, 8 October 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-images-3/
**Latest published version: **
: https://www.w3.org/TR/css-images-3/
**Previous Versions: **
: https://www.w3.org/TR/2019/CR-css-images-3-20191010/
: https://www.w3.org/TR/2012/CR-css3-images-20120417/
**Implementation Report: **
: https://wpt.fyi/results/css/css-images?label=experimental&label=master&aligned
**Feedback: **
: CSSWG Issues Repository
: [Inline In Spec](#issues-index)
**Editors: **
: Tab Atkins Jr. (Google)
: Elika J. Etemad / fantasai (Apple)
: Lea Verou (Invited Expert)
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/css-images/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

This module contains the features of CSS level 3 relating to the [<image>](#typedef-image) type and some replaced elements. It includes and extends the functionality of CSS level 2 [[CSS2]](#biblio-css2). The main extensions compared to CSS2.1 are the generalization of the [<url>](css-values-4.md#url-value) type to the <image> type, several additions to the <image> type, a generic sizing algorithm for images and other replaced content in CSS, definitions for interpolating several <image> types, and several properties controlling the interaction of replaced elements and CSS’s layout models.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-images” in the title, like this: “[css-images] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

The following features are at-risk, and may be dropped during the CR period:

- [image-orientation](#propdef-image-orientation)

“At-risk” is a W3C Process term-of-art, and does not necessarily imply that the feature is in danger of being dropped or delayed. It means that the WG believes the feature may have difficulty being interoperably implemented in a timely manner, and marking it as such allows the WG to drop the feature if necessary when transitioning to the Proposed Rec stage, without having to publish a new Candidate Rec without the feature first.

## <a id="intro"></a>1. Introduction

In CSS Levels 1 and 2, image values, such as those used in the [background-image](css-backgrounds-3.md#propdef-background-image) property, could only be given by a single URL value. This module introduces additional ways of representing 2D images, for example as [a gradient](#gradients).

This module also defines several properties for [manipulating raster images](#image-processing) and for [sizing](#the-object-fit) or [positioning](#the-object-position) replaced elements such as images within the box determined by the CSS layout algorithms. It also defines in a generic way CSS’s [sizing algorithm](#sizing) for images and other similar replaced elements.

*This subsection (above) is not normative.*

### <a id="placement"></a>1.1. Module Interactions

This module defines and extends the [<image>](#typedef-image) value type defined in [[CSS-VALUES-3]](#biblio-css-values-3). It also replaces the [<url>](css-values-4.md#url-value) type with <image> in the [background-image](css-backgrounds-3.md#propdef-background-image), cursor, and list-style-image definitions in CSS1 and CSS2 and adds <image> as an alternative to <url> in the content property’s value. It is presumed that CSS specifications beyond CSS2.1 will use the <image> notation in place of <url> where 2D images are expected. (See e.g. [[CSS3BG]](#biblio-css3bg).)

None of the properties defined in this module, only [image-rendering](#propdef-image-rendering) applies to `::first-line` and `::first-letter`.

### <a id="values"></a>1.2. Value Definitions

This specification follows the CSS property definition conventions from [[CSS2]](#biblio-css2). using the value definition syntax from [[CSS-VALUES-3]](#biblio-css-values-3). Value types not defined in this specification are defined in CSS Level 2 Revision 1 [[CSS2]](#biblio-css2). Other CSS modules may expand the definitions of these value types: for example [[CSS-VALUES-3]](#biblio-css-values-3), when combined with this module, adds the [initial](css-cascade-5.md#valdef-all-initial) keyword as a possible property value.

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their property value. For readability they have not been repeated explicitly.

## <a id="image-values"></a>2. Image Values: the [<image>](#typedef-image) type

The [<image>](#typedef-image) value type denotes a 2D image. It can be a [url reference](#url-notation) or a [color gradient](#gradients). Its syntax is:

```
<a id="typedef-image"></a><image> = <url> | <gradient>
```

An [<image>](#typedef-image) can be used in many CSS properties, including the [background-image](css-backgrounds-3.md#propdef-background-image), list-style-image, cursor properties [[CSS2]](#biblio-css2) (where it replaces the [<url>](css-values-4.md#url-value) component in the property’s value).

In some cases an image is invalid, such as a [<url>](css-values-4.md#url-value) pointing to a resource that is not a valid image format or that has failed to load. An *<a id="invalid-image"></a>invalid image* is rendered as a solid-color [transparent](css-color-4.md#valdef-color-transparent) image with no [natural dimensions](#natural-dimensions). However, [invalid images](#invalid-image) can trigger error-handling clauses in some contexts. For example, an invalid image in list-style-image it is treated as none, allowing the list-style-type to render in its place. [[CSS2]](#biblio-css2)

While an image is loading, is a *<a id="loading-image"></a>loading image*. [Loading images](#loading-image) are *not* [invalid images](#invalid-image), but have similar behavior: they are rendered as a solid-color [transparent](css-color-4.md#valdef-color-transparent) image with no [natural dimensions](#natural-dimensions), and may trigger fallback rendering in contexts that offer it, but must not trigger loading of fallback resources. Alternately, if a loading image happens to be replacing an already-loaded image (for example due to changes in the document or style sheet) and the UA is tracking this information, it may continue to render the already-loaded image in place of the loading image.

Partially-loaded images (whose [natural dimensions](#natural-dimensions) are known, but whose image data is not fully loaded) may be either treated as [loading images](#loading-image) or as loaded images rendered with partial data. For example, a UA may render an interlaced GIF in place as soon as its first pass of pixel data has loaded or even as soon as the image header (which contains sizing data) has parsed and refresh the rendering as more data loads; or it may wait until the entire image has loaded before using it.

A *<a id="computed-image"></a>[computed](css-cascade-5.md#computed-value) [<image>](#typedef-image)* value is the [specified value](css-cascade-5.md#specified-value) with any [<url>](css-values-4.md#url-value)s, <color>s, and [<length>](css-values-4.md#length-value)s computed.

### <a id="url-notation"></a>2.1. Image References: the [url()](css-values-4.md#funcdef-url) notation

The simplest way to indicate an image is to reference an image file by URL. This can be done with the [url()](css-values-4.md#funcdef-url) notation, defined in [[CSS-VALUES-3]](#biblio-css-values-3).

<a id="example-4bc4e081"></a> In the example below, a background image is specified with [url()](css-values-4.md#funcdef-url)syntax:

```
background-image: url(wavy.png);
```

If the UA cannot download, parse, or otherwise successfully display the contents at the URL as an image (i.e. if the image is not fully fully decodable) it must be treated as an [invalid image](#invalid-image).

#### <a id="ambiguous-urls"></a>2.1.1. Ambiguous Reference-or-Image URLs

URLs are used in many contexts for many types of resources, and therefore can be interpreted in many ways. Usually the context the URL appears in makes it clear how to interpret the resource, but in some instances it can be ambiguous. For example, a [mask-image](css-masking-1.md#propdef-mask-image) [<url>](css-values-4.md#url-value) value pointing to an SVG file could be interpreted as a reference to an element in the file or as an [<image>](#typedef-image).

An *<a id="css-ambiguous-image-url"></a>ambiguous image URL* is a [<url>](css-values-4.md#url-value) value that can be interpreted as either an [<image>](#typedef-image) or an element reference. If an [ambiguous image URL](#css-ambiguous-image-url) is a fragment-only URL, then it must be treated as an element reference. Otherwise, if the ambiguous image URL has a fragment that references an element in the resource that is an appropriate type of element for the context in which the <url> appears (such as a `[mask](css-masking-1.md#elementdef-mask)` element for the [mask-image](css-masking-1.md#propdef-mask-image) property), it is interpreted as an element reference. Otherwise, it is treated as an <image>.

Specs using the [ambiguous image URL](#css-ambiguous-image-url) concept must define what elements are valid references for the URL, and any additional conditions that might apply.

<a id="example-73bb4dd5"></a> For example, a reference like [mask-image: url(icon.svg#foo)](css-masking-1.md#propdef-mask-image) might be pointing to a `<mask id="foo">` element in the SVG document, *or* be pointing to a `<g id="foo">` element and depending on the [:target](selectors-4.md#target-pseudo) pseudo-class to change how it renders as an image.

When this occurs, the "icon.svg" file is loaded up and examined; if the #foo element is indeed a `[mask](css-masking-1.md#elementdef-mask)`, the [url()](css-values-4.md#funcdef-url) is treated as a reference to that element; otherwise, it’s interpreted as an image.

#### <a id="url-metadata"></a>2.1.2. Image Metadata

Images can contain metadata such as color space, resolution and orientation which specifies how to render the image. Some image formats are flexible in where this metadata can be placed in the file; however, if the metadata occurs *after* the actual image data, it harms the UA’s ability to “progressively decode” the image and display it as the image’s data streams in.

To reduce the impact of this issue:

-
If the choice exists for a given image format, authors *must* produce their images so that such metadata occurs before the image data in the image. (Note: This is the default for most images already.)
-
User agents *should* ignore any layout-impacting metadata (such as orientation or resolution) that occurs after the image data begins.
If a user agent cannot ignore the metadata based its location in the image (for example, if the decoder being used does not report where in the image the metadata was located), it *must* use the metadata in all cases. (In particular, it is not valid to use the metadata only when the image is "small" and the entire image is downloaded quickly, but to ignore it if the image is large and the metadata isn’t downloaded until well after the image starts being displayed.)

## <a id="gradients"></a>3. Gradients

A gradient is an image that smoothly fades from one color to another. These are commonly used for subtle shading in background images, buttons, and many other things. The *<a id="gradient-function"></a>gradient functions* described in this section allow an author to specify such an image in a terse syntax, so that the UA can generate the image automatically when rendering the page. The syntax of a [<gradient>](#typedef-gradient) is:

```
<a id="typedef-gradient"></a><gradient> =
  <linear-gradient()> | <repeating-linear-gradient()> |
  <radial-gradient()> | <repeating-radial-gradient()>
```

<a id="example-3bdc7fad"></a>

As with the other [<image>](#typedef-image) types defined in this specification, gradients can be used in any property that accepts images. For example:

-
`background: linear-gradient(white, gray);`
-
`list-style-image: radial-gradient(circle, #006, #00a 90%, #0000af 100%, white 100%)`

A gradient is drawn into a box with the dimensions of the [concrete object size](#concrete-object-size), referred to as the *<a id="gradient-box"></a>gradient box*. However, the gradient itself has no [natural dimensions](#natural-dimensions).

<a id="example-aa187bec"></a> For example, if you use a gradient as a background, by default the gradient will draw into a [gradient box](#gradient-box) the size of the element’s padding box. If [background-size](css-backgrounds-3.md#propdef-background-size) is explicitly set to a value such as 100px 200px, then the gradient box will be 100px wide and 200px tall. Similarly, for a gradient used as a list-style-image, the box would be a 1em square, which is the [default object size](#default-object-size) for that property.

Gradients are specified by defining the *<a id="starting-point"></a>starting point* and *<a id="ending-point"></a>ending point* of a *<a id="gradient-line"></a>gradient line* (which, depending on the type of gradient, may geometrically be a line, or a ray, or a spiral), and then specifying colors at points along this line. The colors are smoothly blended to fill in the rest of the line, and then each type of gradient defines how to use the color of the [gradient line](#gradient-line) to produce the actual gradient.

### <a id="linear-gradients"></a>3.1. Linear Gradients: the [linear-gradient()](#funcdef-linear-gradient) notation

A linear gradient is created by specifying a straight [gradient line](#gradient-line), and then several colors placed along that line. The image is constructed by creating an infinite canvas and painting it with lines perpendicular to the gradient line, with the color of the painted line being the color of the gradient line where the two intersect. This produces a smooth fade from each color to the next, progressing in the specified direction.

#### <a id="linear-gradient-syntax"></a>3.1.1. linear-gradient() syntax

The *<a id="funcdef-linear-gradient"></a>linear-gradient()* notation specifies a linear gradient in CSS. Its syntax is as follows:

```
<linear-gradient()> = linear-gradient( [ <linear-gradient-syntax> ] )

<a id="typedef-linear-gradient-syntax"></a><linear-gradient-syntax> = [ <angle> | <zero> | to <side-or-corner> ]? , <color-stop-list>

<a id="typedef-side-or-corner"></a><side-or-corner> = [left | right] || [top | bottom]
```

The first argument to the function specifies the [gradient line](#gradient-line), which gives the gradient a direction and determines how color-stops are positioned. It may be omitted; if so, it defaults to to bottom.

The [gradient line’s](#gradient-line) direction may be specified in two ways:

**using [<angle>](css-values-4.md#angle-value) **
:

For the purpose of this argument, 0deg points upward, and positive angles represent clockwise rotation, so 90deg point toward the right.

The unit identifier may be omitted if the [<angle>](css-values-4.md#angle-value) is zero.

**using keywords **
:

If the argument is to top, to right, to bottom, or to left, the angle of the [gradient line](#gradient-line) is 0deg, 90deg, 180deg, or 270deg, respectively.

If the argument instead specifies a corner of the box such as to top left, the [gradient line](#gradient-line) must be angled such that it points into the same quadrant as the specified corner, and is perpendicular to a line intersecting the two neighboring corners of the [gradient box](#gradient-box). This causes a color-stop at 50% to intersect the two neighboring corners (see [example](#corner-gradient-example)).

Starting from the center of the [gradient box](#gradient-box), extend a line at the specified angle in both directions. The ending point is the point on the [gradient line](#gradient-line) where a line drawn perpendicular to the gradient line would intersect the corner of the gradient box in the specified direction. The starting point is determined identically, but in the opposite direction.

Note: It is expected that the next level of this module will provide the ability to define the gradient’s direction relative to the current text direction and writing-mode.

<a id="example-b6f5099c"></a>

[image: [An image showing a box with a background shading gradually from white in the bottom-left corner to black in the top-right corner. There is a line, illustrating the gradient line, angled at 45 degrees and passing through the center of the box. The starting point and ending point of the gradient line are indicated by the intersection of the gradient line with two additional lines that pass through the bottom-left and top-right corners of the box.]]

This example illustrates visually how to calculate the [gradient line](#gradient-line) from the rules above. This shows the starting and ending point of the gradient line, along with the actual gradient, produced by an element with [background: linear-gradient(45deg, white, black);](css-backgrounds-3.md#propdef-background).

Notice how, though the starting point and ending point are outside of the box, they’re positioned precisely right so that the gradient is pure white *exactly* at the corner, and pure black *exactly* at the opposite corner. That’s intentional, and will always be true for linear gradients.

Given:

-
*A* the angle (in any quadrant) defining the gradient line’s direction such that 0 degrees points upwards and positive angles represent clockwise rotation,
-
*W* the width of the gradient box,
-
*H* the height of the gradient box,

The length of the gradient line (between the [starting point](#starting-point) and [ending point](#ending-point)) is:

`abs(*W* * sin(*A*)) + abs(*H* * cos(*A*))`

The gradient’s color stops are typically placed between the [starting point](#starting-point) and [ending point](#ending-point) on the [gradient line](#gradient-line), but this isn’t required: the gradient line extends infinitely in both directions. The starting point and ending point are merely arbitrary location markers, the starting point defining where 0%, 0px, etc are located when specifying color-stops, and the ending point defines where 100% is located. Color-stops are allowed to have positions before 0% or after 100%.

The color of a linear gradient at any point is determined by finding the unique line passing through that point that is perpendicular to the [gradient line](#gradient-line). The point’s color is the color of the gradient line at the point where this line intersects it.

#### <a id="linear-gradient-examples"></a>3.1.2. Linear Gradient Examples

All of the following [linear-gradient()](#funcdef-linear-gradient) examples are presumed to be backgrounds applied to a box that is 200px wide and 100px tall.

<a id="example-369769bb"></a> Below are various ways of specifying a basic vertical gradient:

```
linear-gradient(yellow, blue);
linear-gradient(to bottom, yellow, blue);
linear-gradient(180deg, yellow, blue);
linear-gradient(to top, blue, yellow);
linear-gradient(to bottom, yellow 0%, blue 100%);
```

<a id="example-ef19999d"></a> This demonstrates the use of an angle in the gradient. Note that, though the angle is not exactly the same as the angle between the corners, the [gradient line](#gradient-line) is still sized so as to make the gradient yellow exactly at the upper-left corner, and blue exactly at the lower-right corner.

```
linear-gradient(135deg, yellow, blue);
linear-gradient(-45deg, blue, yellow);
```

<a id="example-08549192"></a>

This demonstrates a 3-color gradient, and how to specify the location of a stop explicitly:

```
linear-gradient(yellow, blue 20%, #0f0);
```

<a id="corner-gradient-example"></a> This demonstrates a corner-to-corner gradient specified with keywords. Note how the gradient is red and blue exactly in the bottom-left and top-right corners, respectively, exactly like the second example. Additionally, the angle of the gradient is automatically computed so that the color at 50% (in this case, white) stretches across the top-left and bottom-right corners.

```
linear-gradient(to top right, red, white, blue)
```

### <a id="radial-gradients"></a>3.2. Radial Gradients: the [radial-gradient()](#funcdef-radial-gradient) notation

In a radial gradient, rather than colors smoothly fading from one side of the [gradient box](#gradient-box) to the other as with linear gradients, they instead emerge from a single point and smoothly spread outward in a circular or elliptical shape.

The *<a id="funcdef-radial-gradient"></a>radial-gradient()* notation specifies a radial gradient by indicating the center of the gradient (where the 0% ellipse will be) and the size and shape of the *<a id="ending-shape"></a>ending shape* (the 100% ellipse). Color stops are given as a list, just as for [linear-gradient()](#funcdef-linear-gradient). Starting from the [gradient center](#radial-gradient-gradient-center) and progressing towards (and potentially beyond) the [ending shape](#ending-shape), uniformly-scaled concentric ellipses are drawn and colored according to the specified color stops.

#### <a id="radial-gradient-syntax"></a>3.2.1. radial-gradient() Syntax

The radial gradient syntax is:

```
<radial-gradient()> = radial-gradient( [ <radial-gradient-syntax> ] )

<a id="typedef-radial-gradient-syntax"></a><radial-gradient-syntax> =
  [ <radial-shape> || <radial-size> ]? [ at <position> ]? ,
  <color-stop-list>

<a id="typedef-radial-size"></a><radial-size> = <radial-extent> | <length [0,∞]> | <length-percentage [0,∞]>{2}

<a id="typedef-radial-extent"></a><radial-extent> = closest-corner | closest-side | farthest-corner | farthest-side

<a id="typedef-radial-shape"></a><radial-shape> = circle | ellipse
```

<a id="example-c8f2f477"></a> Here is an example of a circular radial gradient 5em wide and positioned with its center in the top left corner:

```
radial-gradient(5em circle at top left, yellow, blue)
```

Note: A future level may add the ability to move the focus of the gradient, as in the original -webkit-gradient() function. See proposal tracked in Issue 1575 for "from <position>" and "from offset <offset>".

The arguments are defined as follows:

***<a id="valdef-radial-gradient-position"></a><position>* **
: Determines the *<a id="radial-gradient-gradient-center"></a>center* of the gradient. The <position> value type (which is also used for [background-position](css-backgrounds-3.md#propdef-background-position)) is defined in [[CSS-VALUES-3]](#biblio-css-values-3), and is resolved using the center-point as the object area and the [gradient box](#gradient-box) as the positioning area. If this argument is omitted, it defaults to center.
***<a id="valdef-radial-gradient-radial-shape"></a>[<radial-shape>](#typedef-radial-shape)* **
: Can be either *<a id="valdef-radial-shape-circle"></a>circle* or *<a id="valdef-radial-shape-ellipse"></a>ellipse*; determines whether the gradient’s [ending shape](#ending-shape) is a circle or an ellipse, respectively. If [<radial-shape>](#typedef-radial-shape) is omitted, the ending shape defaults to a circle if the [<radial-size>](#typedef-radial-size) is a single [<length>](css-values-4.md#length-value), and to an ellipse otherwise.
***<a id="valdef-radial-gradient-radial-size"></a>[<radial-size>](#typedef-radial-size)* **
: Determines the size of the gradient’s [ending shape](#ending-shape). If omitted it defaults to [farthest-corner](#valdef-radial-extent-farthest-corner). It can be given explicitly or by keyword. For the purpose of the keyword definitions, consider the [gradient box](#gradient-box) edges as extending infinitely in both directions, rather than being finite line segments.

If the ending-shape is an ellipse, its axises are aligned with the horizontal and vertical axises.

Both [circle](#valdef-radial-shape-circle) and [ellipse](#valdef-radial-shape-ellipse) gradients accept the following [<radial-extent>](#typedef-radial-extent) values:

***<a id="valdef-radial-extent-closest-side"></a>closest-side* **
: The [ending shape](#ending-shape) is sized so that it exactly meets the side of the [gradient box](#gradient-box) closest to the gradient’s center. If the shape is an ellipse, it exactly meets the closest side in each dimension.
***<a id="valdef-radial-extent-farthest-side"></a>farthest-side* **
: Same as [closest-side](#valdef-radial-extent-closest-side), except the [ending shape](#ending-shape) is sized based on the farthest side(s).
***<a id="valdef-radial-extent-closest-corner"></a>closest-corner* **
: The [ending shape](#ending-shape) is sized so that it passes through the corner of the [gradient box](#gradient-box) closest to the gradient’s center. If the shape is an ellipse, the ending shape is given the same aspect-ratio it would have if [closest-side](#valdef-radial-extent-closest-side) were specified.
***<a id="valdef-radial-extent-farthest-corner"></a>farthest-corner* **
: Same as [closest-corner](#valdef-radial-extent-closest-corner), except the [ending shape](#ending-shape) is sized based on the farthest corner. If the shape is an ellipse, the ending shape is given the same aspect ratio it would have if [farthest-side](#valdef-radial-extent-farthest-side) were specified.
If [<radial-shape>](#typedef-radial-shape) is specified as [circle](#valdef-radial-shape-circle) or is omitted, the [<radial-size>](#typedef-radial-size) may be given explicitly as:

***<a id="valdef-radial-size-length-0"></a><length [0,∞]>* **
: Gives the radius of the circle explicitly. Negative values are invalid.

Note: Percentages are *not* allowed here; they can only be used to specify the size of an elliptical gradient, not a circular one. This restriction exists because there is are multiple reasonable answers as to which dimension the percentage should be relative to. A future level of this module may provide the ability to size circles with percentages, perhaps with more explicit controls over which dimension is used.

If [<radial-shape>](#typedef-radial-shape) is specified as [ellipse](#valdef-radial-shape-ellipse) or is omitted, [<radial-size>](#typedef-radial-size) may instead be given explicitly as:

***<a id="valdef-radial-size-length-percentage-0-2"></a>[<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage){2}* **
: Gives the size of the ellipse explicitly. The first value represents the horizontal radius, the second the vertical radius. Percentages values are relative to the corresponding dimension of the [gradient box](#gradient-box). Negative values are invalid.
Expanded with the above definitions, the grammar becomes:

```
radial-gradient() = radial-gradient(
  [ [ circle               || <length [0,∞]> ]                          [ at <position> ]? , |
    [ ellipse              || <length-percentage [0,∞]>{2} ]            [ at <position> ]? , |
    [ [ circle | ellipse ] || <radial-extent> ]                     [ at <position> ]? , |
    at <position>,
  ]?
  <color-stop-list>
)
```

#### <a id="radial-color-stops"></a>3.2.2. Placing Color Stops

Color-stops are placed on a [gradient line](#gradient-line) shaped like a ray (a line that starts at one point, and extends infinitely in a one direction), similar to the gradient line of linear gradients. The gradient line’s [starting point](#starting-point) is at the center of the gradient, and it extends toward the right, with the [ending point](#ending-point) on the point where the gradient line intersects the [ending shape](#ending-shape). A color-stop can be placed at a location before 0%; though the negative region of the gradient line is never directly consulted for rendering, color stops placed there can affect the color of non-negative locations on the gradient line through interpolation or repetition (see [repeating gradients](#repeating-gradients)). For example, radial-gradient(red -50px, yellow 100px) produces an elliptical gradient that starts with a reddish-orange color in the center (specifically, #f50) and transitions to yellow. Locations greater than 100% simply specify a location a correspondingly greater distance from the center of the gradient.

The color of the gradient at any point is determined by first finding the unique ellipse passing through that point with the same center, orientation, and ratio between major and minor axises as the ending-shape. The point’s color is then the color of the positive section of the [gradient line](#gradient-line) at the location where this ellipse intersects it.

#### <a id="degenerate-radials"></a>3.2.3. Degenerate Radial Gradients

Some combinations of position, size, and shape will produce a circle or ellipse with a radius of 0. This will occur, for example, if the center is on a [gradient box](#gradient-box) edge and [closest-side](#valdef-radial-extent-closest-side) or [closest-corner](#valdef-radial-extent-closest-corner) is specified or if the size and shape are given explicitly and either of the radiuses is zero. In these degenerate cases, the gradient must be rendered as follows:

**If the [ending shape](#ending-shape) is a circle with zero radius: **
:

Render as if the [ending shape](#ending-shape) was a circle whose radius was an arbitrary very small number greater than zero. This will make the gradient continue to look like a circle.

**If the [ending shape](#ending-shape) has zero width (regardless of the height): **
:

Render as if the [ending shape](#ending-shape) was an ellipse whose height was an arbitrary very large number and whose width was an arbitrary very small number greater than zero. This will make the gradient look similar to a horizontal linear gradient that is mirrored across the center of the ellipse. It also means that all color-stop positions specified with a percentage resolve to 0px.

**Otherwise, if the [ending shape](#ending-shape) has zero height: **
:

Render as if the [ending shape](#ending-shape) was an ellipse whose width was an arbitrary very large number and whose height was an arbitrary very small number greater than zero. This will make the gradient look like a solid-color image equal to the color of the last color-stop, or equal to the average color of the gradient if it’s repeating.

#### <a id="radial-gradient-examples"></a>3.2.4. Radial Gradient Examples

All of the following examples are applied to a box that is 200px wide and 100px tall.

<a id="example-21e5434c"></a> These examples demonstrate different ways to write the basic syntax for radial gradients:

```
radial-gradient(yellow, green);
radial-gradient(ellipse at center, yellow 0%, green 100%);
radial-gradient(farthest-corner at 50% 50%, yellow, green);
```

```
radial-gradient(circle, yellow, green);
```

```
radial-gradient(red, yellow, green);
```

<a id="example-efad25ac"></a> This image shows a gradient originating from somewhere other than the center of the box:

```
radial-gradient(farthest-side at left bottom, red, yellow 50px, green);
```

<a id="example-fe6adef0"></a> Here we illustrate a [closest-side](#valdef-radial-extent-closest-side) gradient.

```
radial-gradient(closest-side at 20px 30px, red, yellow, green);
radial-gradient(20px 30px at 20px 30px, red, yellow, green);
```

```
radial-gradient(closest-side circle at 20px 30px, red, yellow, green);
radial-gradient(20px 20px at 20px 30px, red, yellow, green);
```

### <a id="repeating-gradients"></a>3.3. Repeating Gradients: the [repeating-linear-gradient()](#funcdef-repeating-linear-gradient) and [repeating-radial-gradient()](#funcdef-repeating-radial-gradient) notations

In addition to [linear-gradient()](#funcdef-linear-gradient) and [radial-gradient()](#funcdef-radial-gradient), this specification defines *<a id="funcdef-repeating-linear-gradient"></a>repeating-linear-gradient()* and *<a id="funcdef-repeating-radial-gradient"></a>repeating-radial-gradient()* values. These notations take the same values and are interpreted the same as their respective non-repeating siblings defined previously.

```
<repeating-linear-gradient()> = repeating-linear-gradient( [ <linear-gradient-syntax> ] )
<repeating-radial-gradient()> = repeating-radial-gradient( [ <radial-gradient-syntax> ] )
```

When rendered, however, the color-stops are repeated infinitely in both directions, with their positions shifted by multiples of the difference between the last specified color-stop’s position and the first specified color-stop’s position. For example, repeating-linear-gradient(red 10px, blue 50px) is equivalent to linear-gradient(..., red -30px, blue 10px, red 10px, blue 50px, red 50px, blue 90px, ...). Note that the last color-stop and first color-stop will always coincide at the boundaries of each group, which will produce sharp transitions if the gradient does not start and end with the same color.

<a id="example-0122a133"></a> Repeating gradient syntax is identical to that of non-repeating gradients:

```
repeating-linear-gradient(red, blue 20px, red 40px)
```

```
repeating-radial-gradient(red, blue 20px, red 40px)
```

```
repeating-radial-gradient(circle closest-side at 20px 30px, red, yellow, green 100%, yellow 150%, red 200%)
```

If the distance between the first and last color-stops is non-zero, but is small enough that the implementation knows that the physical resolution of the output device is insufficient to faithfully render the gradient, the implementation must [find the average color of the gradient](#gradient-average-color) and render the gradient as a solid-color image equal to the average color.

If the distance between the first and last color-stops is zero (or rounds to zero due to implementation limitations), the implementation must [find the average color of a gradient](#gradient-average-color) with the same number and color of color-stops, but with the first and last color-stop an arbitrary non-zero distance apart, and the remaining color-stops equally spaced between them. Then it must render the gradient as a solid-color image equal to that average color.

If the width of the ending shape of a repeating radial gradient is non-zero and the height is zero, or is close enough to zero that the implementation knows that the physical resolution of the output device is insufficient to faithfully render the gradient, the implementation must [find the average color of the gradient](#gradient-average-color) and render the gradient as a solid-color image equal to the average color.

Note: The [Degenerate Radial Gradients](#degenerate-radials) section describes how the ending shape is adjusted when its width is zero.

To *<a id="gradient-average-color"></a>find the average color of a gradient*, run these steps:

1.
Define *list* as an initially-empty list of premultiplied RGBA colors, and *total-length* as the distance between first and last color stops.
2.
For each adjacent pair of color-stops, define *weight* as half the distance between the two color-stops, divided by *total-length*. Add two entries to *list*, the first obtained by representing the color of the first color-stop in premultiplied sRGBA and scaling all of the components by *weight*, and the second obtained in the same way with the second color-stop.
3.
Sum the entries of *list* component-wise to produce the average color, and return it.

Note: As usual, implementations may use whatever algorithm they wish, so long as it produces the same result as the above.

<a id="example-179d6dec"></a> For example, the following gradient is rendered as a solid light-purple image (equal to `rgb(75%,50%,75%)`):

```
repeating-linear-gradient(red 0px, white 0px, blue 0px);
```

The following gradient would render the same as the previous under normal circumstances (because desktop monitors can’t faithfully render color-stops 1/10th of a pixel apart), but would render as a normal repeating gradient if, for example, the author applied "zoom:100;" to the element on which the gradient appears:

```
repeating-linear-gradient(red 0px, white .1px, blue .2px);
```

### <a id="gradient-colors"></a>3.4. Defining Gradient Color

The colors in gradients are specified using *<a id="color-stop"></a>color stops* (a <color> and a corresponding position on the [gradient line](#gradient-line)) and *<a id="color-transition-hint"></a>color transition hints* (a position between two [color stops](#color-stop) representing the halfway point in the color transition) which are placed on the gradient line, defining the color at every point of the line. (Each [gradient function](#gradient-function) defines the shape and length of the gradient line, along with its [starting point](#starting-point) and [ending point](#ending-point); see above.)

Colors throughout the gradient field are then determined by tying them to specific points along the [gradient line](#gradient-line) as specified by the gradient function. UAs may “dither” gradient colors slightly (randomly alternate individual pixels with nearby colors on the gradient line) to effect a smoother gradient.

#### <a id="color-stop-syntax"></a>3.4.1. Color Stop Lists

[Color stops](#color-stop) and [transition hints](#color-transition-hint) are specified in a *<a id="color-stop-list"></a>color stop list*, which is a list of one or more color stops interleaved with optional transition hints:

```
<a id="typedef-color-stop-list"></a><color-stop-list> =
  <linear-color-stop> , [ <linear-color-hint>? , <linear-color-stop> ]#?
<a id="typedef-linear-color-stop"></a><linear-color-stop> = <color> <length-percentage>?
<a id="typedef-linear-color-hint"></a><linear-color-hint> = <length-percentage>
```

Percentages are resolved against the length of the [gradient line](#gradient-line) between the [starting point](#starting-point) and [ending point](#ending-point), with 0% being at the starting point and 100% being at the ending point. Lengths are measured along the gradient line from the starting point in the direction of the ending point.

[Color stop](#color-stop) and [transition hint](#color-transition-hint) positions are usually placed between the [starting point](#starting-point) and [ending point](#ending-point), but that’s not required: the gradient line extends infinitely in both directions, and positions can be specified anywhere on the [gradient line](#gradient-line).

When the position of a [color stop](#color-stop) is omitted, it is automatically assigned a position. The first or last color stop in the [color stop list](#color-stop-list) is assigned the [gradient line’s](#gradient-line) [starting point](#starting-point) or [ending point](#ending-point) (respectively). Otherwise, it’s assigned the position halfway between the two surrounding stops. If multiple stops in a row lack a position, they space themselves out equally between the surrounding positioned stops. See [§ 3.4.3 Color Stop “Fixup”](#color-stop-fixup) for details.

#### <a id="coloring-gradient-line"></a>3.4.2. Coloring the Gradient Line

At each [color stop](#color-stop) position, the [gradient line](#gradient-line) is the color of the color stop. Before the first color stop, the gradient line is the color of the first color stop, and after the last color stop, the gradient line is the color of the last color stop. Between two color stops, the gradient line’s color is interpolated between the colors of the two color stops, with the interpolation taking place in [premultiplied RGBA space](#premultiplied).

By default, this interpolation is linear—​at 25%, 50%, or 75% of the distance between two [color stops](#color-stop), the color is a 25%, 50%, or 75% blend of the colors of the two stops.

However, if a [transition hint](#color-transition-hint) was provided between two [color stops](#color-stop), the interpolation is non-linear, and controlled by the hint:

1. Determine the location of the [transition hint](#color-transition-hint) as a percentage of the distance between the two [color stops](#color-stop), denoted as a number between 0 and 1, where 0 indicates the hint is placed right on the first color stop, and 1 indicates the hint is placed right on the second color stop. Let this percentage be *H*.
2. For any given point between the two color stops, determine the point’s location as a percentage of the distance between the two [color stops](#color-stop), in the same way as the previous step. Let this percentage be *P*.
3. Let *C*, the color weighting at that point, be equal to `*P*^log_*H*(.5)`.
4. The color at that point is then a linear blend between the colors of the two [color stops](#color-stop), blending `(1 - *C*)` of the first stop and *C* of the second stop.

Note: The [transition hint](#color-transition-hint) specifies where the “halfway color”—​the 50% blend between the colors of the two surrounding color stops—​should be placed. When the hint is exactly halfway between the two surrounding color stops, the above interpolation algorithm happens to produce the ordinary linear interpolation. If the hint is placed anywhere else, it produces a smooth exponential curve between the surrounding color stops, with the “halfway color” occurring exactly where the hint specifies.

<a id="issue-7a141efc"></a> Add a visual example of a color hint being used.

If multiple [color stops](#color-stop) have the same position, they produce an infinitesimal transition from the one specified first in the list to the one specified last. In effect, the color suddenly changes at that position rather than smoothly transitioning.

<a id="premultiplied"></a>

What does “pre-multiplied” mean?

A “pre-multiplied” color is written in a form where the alpha channel is multiplied into the color channels, rather than being processed independently. For example, a partially-transparent blue may be given as `rgba(0, 0, 255, .5)`, which would then be expressed as `[0, 0, 127.5, .5]` in its premultiplied representation.

Interpolating colors using the premultiplied representations rather than the plain rgba representations tends to produce more attractive transitions, particularly when transitioning from a fully opaque color to fully transparent.

Note that transitions where either the transparency or the color are held constant (for example, transitioning between `rgba(255, 0, 0, 100%)` (opaque red) and `rgba(0,0,255,100%)` (opaque blue), or `rgba(255,0,0,100%)` (opaque red) and `rgba(255,0,0,0%)` (transparent red)) have identical results whether the color interpolation is done in premultiplied or non-premultiplied color-space. Differences only arise when *both* the color and transparency differ between the two endpoints.

<a id="example-77596609"></a> The following example illustrates the difference between a gradient transitioning in pre-multiplied sRGBA and one transitioning (incorrectly) in non-premultiplied. In both of these example, the gradient is drawn over a white background. Both gradients could be written with the following value:

```
linear-gradient(90deg, red, transparent, blue)
```

With premultiplied colors, transitions to or from "transparent" always look nice:

On the other hand, if a gradient were to incorrectly transition in non-premultiplied space, the center of the gradient would be a noticeably grayish color, because "transparent" is actually a shorthand for rgba(0,0,0,0), or transparent black, meaning that the red transitions to a black as it loses opacity, and similarly with the blue’s transition:

#### <a id="color-stop-fixup"></a>3.4.3. Color Stop “Fixup”

When resolving the [used](css-cascade-5.md#used-value) positions of each [color stop](#color-stop), the following steps must be applied *in order*:

1. If the first [color stop](#color-stop) does not have a position, set its position to 0%.
2. If the last [color stop](#color-stop) does not have a position, set its position to 100%.
3. If a [color stop](#color-stop) or [transition hint](#color-transition-hint) has a position that is less than the specified position of any color stop or transition hint before it in the list, set its position to be equal to the largest specified position of any color stop or transition hint before it.
4. If any [color stop](#color-stop) still does not have a position, then, for each run of adjacent color stops without positions, set their positions so that they are evenly spaced between the preceding and following color stops with positions.

After applying these rules, all [color stops](#color-stop) and [transition hints](#color-transition-hint) will have a definite position and color and they will be in ascending order.

Note: It is recommended that authors exercise caution when mixing different types of units, such as px, em, or %, as this can cause a [color stop](#color-stop) to unintentionally try to move before an earlier one. For example, the rule [background-image: linear-gradient(yellow 100px, blue 50%)](css-backgrounds-3.md#propdef-background-image) wouldn’t trigger any fix-up while the background area is at least 200px tall. If it was 150px tall, however, the blue color stop’s position would be equivalent to 75px, which precedes the yellow color stop, and would be corrected to a position of 100px. Additionally, since the relative ordering of such color stops cannot be determined without performing layout, they will not interpolate smoothly in animations or transitions.

<a id="example-996853e0"></a> Below are several pairs of gradients. The latter of each pair is a manually “fixed-up” version of the former, obtained by applying the above rules. For each pair, both gradients will render identically. The numbers in each arrow specify which fixup steps are invoked in the transformation.

```
1. linear-gradient(red, white 20%, blue)
   =1=>
   linear-gradient(red 0%, white 20%, blue 100%)

2. linear-gradient(red 40%, white, black, blue)
   =1,3=>
   linear-gradient(red 40%, white 60%, black 80%, blue 100%)

3. linear-gradient(red -50%, white, blue)
   =1,3=>
   linear-gradient(red -50%, white 25%, blue 100%)

4. linear-gradient(red -50px, white, blue)
   =1,3=>
   linear-gradient(red -50px, white calc(-25px + 50%), blue 100%)

5. linear-gradient(red 20px, white 0px, blue 40px)
   =2=>
   linear-gradient(red 20px, white 20px, blue 40px)

6. linear-gradient(red, white -50%, black 150%, blue)
   =1,2=>
   linear-gradient(red 0%, white 0%, black 150%, blue 150%)

7. linear-gradient(red 80px, white 0px, black, blue 100px)
   =2,3=>
   linear-gradient(red 80px, white 80px, black 90px, blue 100px)
```

## <a id="sizing"></a>4. Sizing Images and Objects in CSS

Images used in CSS may come from a number of sources: from binary image formats (such as gif, jpeg, etc), dedicated markup formats (such as SVG), and CSS-specific formats (such as the linear-gradient() value type defined in this specification). As well, a document may contain many other types of objects, such as video, plugins, or nested documents. These images and objects (just *<a id="objects"></a>objects* hereafter) may offer many types of sizing information to CSS, or none at all. This section defines generically the size negotiation model between the object and the CSS layout algorithms.

### <a id="sizing-terms"></a>4.1. Object-Sizing Terminology

In order to define this handling, we define a few terms, to make it easier to refer to various concepts:

***<a id="natural-dimensions"></a><a id="intrinsic-dimensions"></a>natural dimensions* **
: The term [natural dimensions](#natural-dimensions) refers to the set of the *<a id="natural-height"></a><a id="intrinsic-height"></a>natural height*, *<a id="natural-width"></a><a id="intrinsic-width"></a>natural width*, and *<a id="natural-aspect-ratio"></a><a id="intrinsic-aspect-ratio"></a>natural aspect ratio* (the ratio between the width and height), each of which may or may not exist for a given [object](#objects). These natural dimensions represent the preferred sizing intrinsic to the object itself; that is, they are not a function of the context in which the object is used. CSS does not define how the natural dimensions are found in general.

Raster images are an example of an [object](#objects) with all three natural dimensions. SVG images designed to scale might have only a [natural aspect ratio](#natural-aspect-ratio); SVG images can also be created with only a [natural width](#natural-width) or [height](#natural-height). CSS gradients, defined in this specification, are an example of an object with no [natural dimensions](#natural-dimensions) at all. Another example of this is embedded documents, such as the `iframe` element in HTML. Many objects, such as most images, cannot have only two natural dimensions, as any two automatically define the third. However some types of replaced elements, such as form controls, can have a natural width and a natural height, but no natural aspect ratio.

If an [object](#objects) has a [degenerate](css-values-4.md#degenerate-ratio) [natural aspect ratio](#natural-aspect-ratio) (at least one part being zero or infinity), it is treated as having no natural aspect ratio.

If an [object](#objects) (such as an icon) has multiple sizes, then the largest size (by area) is taken as its [natural dimensions](#natural-dimensions). If it has multiple aspect ratios at that size, or has multiple aspect ratios and no size, then the aspect ratio closest to the aspect ratio of the [default object size](#default-object-size) is used. Determine this by seeing which aspect ratio produces the largest area when fitting it within the default object size using a [contain constraint](#contain-constraint) fit; if multiple sizes tie for the largest area, the widest size is chosen as its natural dimensions.

The [natural width](#natural-width) and [natural height](#natural-height) are collectively referred to the *<a id="natural-size"></a>natural sizes*.

***<a id="specified-size"></a>specified size* **
: The specified size of an [object](#objects) is given by CSS, such as through the width and height or [background-size](css-backgrounds-3.md#propdef-background-size) properties. The specified size can be a definite width and height, a set of constraints, or a combination thereof.
***<a id="concrete-object-size"></a>concrete object size* **
: The [concrete object size](#concrete-object-size) is the result of combining an [object’s](#objects) [natural dimensions](#natural-dimensions) and [specified size](#specified-size) with the [default object size](#default-object-size) of the context it’s used in, producing a rectangle with an absolute width and height.
***<a id="default-object-size"></a>default object size* **
: The [default object size](#default-object-size) is a rectangle with an absolute height and width used to determine the [concrete object size](#concrete-object-size) when both the [natural dimensions](#natural-dimensions) and [specified size](#specified-size) are missing dimensions.
### <a id="object-negotiation"></a>4.2. CSS⇋Object Negotiation

[Objects](#objects) in CSS are sized and rendered by the *<a id="object-size-negotiation"></a>object size negotiation* algorithm as follows:

1.
When an [object](#objects) is specified in a document, such as through a [url()](css-values-4.md#funcdef-url) value in a [background-image](css-backgrounds-3.md#propdef-background-image) property or a `src` attribute on an `img` element, CSS queries the object for its [natural dimensions](#natural-dimensions).
2.
Using the [natural dimensions](#natural-dimensions), the [specified size](#specified-size), and the [default object size](#default-object-size) for the context the [object](#objects) is used in, CSS then computes a [concrete object size](#concrete-object-size). (See the [following section](#default-sizing).) This defines the size and position of the region the object will render in.
3.
CSS asks the [object](#objects) to render itself at the [concrete object size](#concrete-object-size). CSS does not define how objects render when the concrete object size is different from the object’s [natural dimensions](#natural-dimensions). The object may adjust itself to match the concrete object size in some way, or even render itself larger or smaller than the concrete object size to satisfy sizing constraints of its own.
4.
Unless otherwise specified by CSS, the [object](#objects) is then clipped to the [concrete object size](#concrete-object-size).

### <a id="concrete-size-resolution"></a>4.3. Concrete Object Size Resolution

Currently the rules for sizing [objects](#objects) are described in each context that such objects are used. This section defines some common sizing constraints and how to resolve them so that future specs can refer to them instead of redefining size resolution in each instance.

#### <a id="default-sizing"></a>4.3.1. Default Sizing Algorithm

The *<a id="default-sizing-algorithm"></a>default sizing algorithm* is a set of rules commonly used to find an [object’s](#objects) [concrete object size](#concrete-object-size). It resolves the simultaneous constraints presented by the object’s [natural dimensions](#natural-dimensions) and either an unconstrained [specified size](#specified-size) or one consisting of only a definite width and/or height.

Some [object](#objects) sizing rules (such as those for list-style-image) correspond exactly to the [default sizing algorithm](#default-sizing-algorithm). Others (such as those for [border-image](css-backgrounds-3.md#propdef-border-image)) invoke the default sizing algorithm but also apply additional sizing rules before arriving at a final [concrete object size](#concrete-object-size).

The [default sizing algorithm](#default-sizing-algorithm) is defined as follows:

-
If the [specified size](#specified-size) is a definite width and height, the [concrete object size](#concrete-object-size) is given that width and height.
-
If the [specified size](#specified-size) is only a width or height (but not both) then the [concrete object size](#concrete-object-size) is given that specified width or height. The other dimension is calculated as follows:
  1.
  If the [object](#objects) has a [natural aspect ratio](#natural-aspect-ratio), the missing dimension of the [concrete object size](#concrete-object-size) is calculated using that aspect ratio and the present dimension.
  2.
  Otherwise, if the missing dimension is present in the object’s [natural dimensions](#natural-dimensions), the missing dimension is taken from the [object’s](#objects) natural dimensions.
  3.
  Otherwise, the missing dimension of the [concrete object size](#concrete-object-size) is taken from the [default object size](#default-object-size).
-
If the [specified size](#specified-size) has no constraints:
  1.
  If the [object](#objects) has a [natural](#natural-dimensions) height or width, its size is resolved as if its natural dimensions were given as the [specified size](#specified-size).
  2.
  Otherwise, its size is resolved as a [contain constraint](#contain-constraint) against the [default object size](#default-object-size).

#### <a id="cover-contain"></a>4.3.2. Cover and Contain Constraint Sizing

Two other common specified sizes are the [contain constraint](#contain-constraint) and the [cover constraint](#cover-constraint), both of which are resolved against a specified *<a id="constraint-rectangle"></a>constraint rectangle* using the [object’s](#objects) preferred aspect ratio:

-
A *<a id="contain-constraint"></a>contain constraint* is resolved by setting the [concrete object size](#concrete-object-size) to the largest rectangle that has the [object’s](#objects) [natural aspect ratio](#natural-aspect-ratio) and additionally has neither width nor height larger than the constraint rectangle’s width and height, respectively.
-
A *<a id="cover-constraint"></a>cover constraint* is resolved by setting the [concrete object size](#concrete-object-size) to the smallest rectangle that has the [object’s](#objects) [natural aspect ratio](#natural-aspect-ratio) and additionally has neither width nor height smaller than the constraint rectangle’s width and height, respectively.

In both cases, if the object doesn’t have a [natural aspect ratio](#natural-aspect-ratio), the [concrete object size](#concrete-object-size) is the specified constraint rectangle.

### <a id="object-sizing-examples"></a>4.4. Examples of CSS Object Sizing

<a id="example-47bacc07"></a> The following examples show how the CSS 2.1 and CSS3 Backgrounds & Borders sizing algorithms correspond to concepts defined in this specification.

**[background-image](css-backgrounds-3.md#propdef-background-image) **
: The rules for calculating the [concrete object size](#concrete-object-size) of a background are defined in CSS2.1§14.2.1 and CSS3BG§3.9. CSS2.1 uses the [default sizing algorithm](#default-sizing-algorithm) with no [specified size](#specified-size) and the [background positioning area](css-backgrounds-3.md#background-positioning-area) as the [default object size](#default-object-size). [[CSS2]](#biblio-css2) In CSS3, [background-size](css-backgrounds-3.md#propdef-background-size) property can give a sizing constraint, invoking either the default sizing algorithm or one of the [contain](#contain-constraint) or [cover](#cover-constraint) constraints. The concrete object size is further adjusted in later steps if [background-repeat](css-backgrounds-3.md#propdef-background-repeat) has a [round](css-backgrounds-3.md#valdef-background-repeat-round) value. [[CSS3BG]](#biblio-css3bg)
**list-style-image **
: The rules for calculating the [concrete object size](#concrete-object-size) of a list-style image are defined in CSS2.1§12.5.1. They use the [default sizing algorithm](#default-sizing-algorithm) with no [specified size](#specified-size) and a [default object size](#default-object-size) of 1em square.
**[border-image](css-backgrounds-3.md#propdef-border-image) **
: Border images are sized twice: first the entire image is sized to determine the slice points, then the slices are sized to decorate the border. The first sizing operation is defined in CSS3BG§6.2 and uses the [default sizing algorithm](#default-sizing-algorithm) with no [specified size](#specified-size), and the border image area as the [default object size](#default-object-size). The second operation is defined in CSS3BG§6.2: the default sizing algorithm is used to determine an initial size for each slice with the corresponding [border image area](css-backgrounds-3.md#border-image-area) part as the default object size. By default the specified size matches this default object size; however the [border-image-repeat](css-backgrounds-3.md#propdef-border-image-repeat) property can drop the specified size in one or more directions and may also apply an additional rounding step. [[CSS3BG]](#biblio-css3bg)
**cursor **
: The rules for calculating the [concrete object size](#concrete-object-size) of a cursor are defined in CSS2.1 § 18.1: Cursors. The [default object size](#default-object-size) is a UA-defined size that should be based on the size of a typical cursor on the UA’s operating system. [[CSS2]](#biblio-css2)
**content **
: Objects inserted via the CSS2.1 content property are anonymous replaced elements, and are sized the same way. [[CSS2]](#biblio-css2) Note that such anonymous elements have all their non-inherited properties (including width, height, etc.) set to their initial values.
**replaced elements **
: CSS 2.1 defines the sizing of replaced elements (including those inserted as generated content via content) in sections 10.3.2, 10.4, 10.6.2, and 10.7. [[CSS2]](#biblio-css2) The [object-fit](#propdef-object-fit) property defined below defines how the [concrete object size](#concrete-object-size) corresponds to the element’s used width and height; by default they coincide.
### <a id="the-object-fit"></a>4.5. Sizing Objects: the [object-fit](#propdef-object-fit) property

| Name: | *<a id="propdef-object-fit"></a>object-fit* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | fill [\|](css-values-4.md#comb-one) contain \| cover \| none \| scale-down |
| [Initial:](css-cascade-5.md#initial-values) | fill |
| [Applies to:](css-cascade-5.md#applies-to) | replaced elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | specified keyword |
| Canonical order: | per grammar |
| Animation type: | discrete |

The [object-fit](#propdef-object-fit) property specifies how the contents of a replaced element should be fitted to the box established by its used height and width.

***<a id="valdef-object-fit-fill"></a>fill* **
: The replaced content is sized to fill the element’s content box: the object’s [concrete object size](#concrete-object-size) is the element’s used width and height.
***<a id="valdef-object-fit-contain"></a>contain* **
: The replaced content is sized to maintain its [natural aspect ratio](#natural-aspect-ratio) while fitting within the element’s content box: its [concrete object size](#concrete-object-size) is resolved as a [contain constraint](#contain-constraint) against the element’s used width and height.
***<a id="valdef-object-fit-cover"></a>cover* **
: The replaced content is sized to maintain its [natural aspect ratio](#natural-aspect-ratio) while filling the element’s entire content box: its [concrete object size](#concrete-object-size) is resolved as a [cover constraint](#cover-constraint) against the element’s used width and height.
***<a id="valdef-object-fit-none"></a>none* **
: The replaced content is not resized to fit inside the element’s content box: determine the object’s [concrete object size](#concrete-object-size) using the [default sizing algorithm](#default-sizing-algorithm) with no specified size, and a [default object size](#default-object-size) equal to the replaced element’s used width and height.
***<a id="valdef-object-fit-scale-down"></a>scale-down* **
: Size the content as if [none](#valdef-object-fit-none) or [contain](#valdef-object-fit-contain) were specified, whichever would result in a smaller [concrete object size](#concrete-object-size).

Note: Both [none](#valdef-object-fit-none) and [contain](#valdef-object-fit-contain) respect the content’s [natural aspect ratio](#natural-aspect-ratio), so the concept of "smaller" is well-defined.

If the content does not completely fill the replaced element’s content box, the unfilled space shows the replaced element’s background. Since replaced elements always clip their contents to the content box, the content will never overflow. See the [object-position](#propdef-object-position) property for positioning the object with respect to the content box.

An example showing how four of the values of [object-fit](#propdef-object-fit) cause the replaced element (blue figure) to be scaled to fit its height/width box (shown with a green background), using the initial value for [object-position](#propdef-object-position). The fifth value, [scale-down](#valdef-object-fit-scale-down), in this case looks identical to [contain](#valdef-object-fit-contain).

Note: The [object-fit](#propdef-object-fit) property has similar semantics to the `fit` attribute in [[SMIL10]](#biblio-smil10) and the <meetOrSlice> parameter on the [`preserveAspectRatio` attribute](svg11/coords.md#PreserveAspectRatioAttribute) in [[SVG11]](#biblio-svg11).

Note: Per the [object size negotiation](#object-size-negotiation) algorithm, the [concrete object size](#concrete-object-size) (or, in this case, the size of the content) does not directly scale the object itself - it is merely passed to the object as information about the size of the visible canvas. How to then draw into that size is up to the image format. In particular, raster images always scale to the given size, while SVG uses the given size as the size of the "SVG Viewport" (a term defined by SVG) and then uses the values of several attributes on the root `<svg>` element to determine how to draw itself.

### <a id="the-object-position"></a>4.6. Positioning Objects: the [object-position](#propdef-object-position) property

| Name: | *<a id="propdef-object-position"></a>object-position* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <position> |
| [Initial:](css-cascade-5.md#initial-values) | 50% 50% |
| [Applies to:](css-cascade-5.md#applies-to) | replaced elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to width and height of element itself |
| [Computed value:](css-cascade-5.md#computed) | as for [background-position](css-backgrounds-3.md#propdef-background-position) |
| Canonical order: | the horizontal component of the <position>, followed by the vertical component |
| Animation type: | as for [background-position](css-backgrounds-3.md#propdef-background-position) |

The [object-position](#propdef-object-position) property determines the alignment of the replaced element inside its box. The <position> value type (which is also used for [background-position](css-backgrounds-3.md#propdef-background-position)) is defined in [[CSS-VALUES-3]](#biblio-css-values-3), and is resolved using the [concrete object size](#concrete-object-size) as the object area and the content box as the positioning area.

Note: Areas of the box not covered by the replaced element will show the element’s background.

## <a id="image-processing"></a>5. Image Processing

### <a id="the-image-orientation"></a>5.1. Orienting an Image on the Page: the [image-orientation](#propdef-image-orientation) property

If a picture is taken with a camera turned on its side, or a document isn’t positioned correctly within a scanner, the resultant image may be "sideways" or even upside-down. The [image-orientation](#propdef-image-orientation) property provides a way to apply an "out-of-band" rotation to image source data to correctly orient an image.

| Name: | *<a id="propdef-image-orientation"></a>image-orientation* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | from-image [\|](css-values-4.md#comb-one) none \| [ [<angle>](css-values-4.md#angle-value) [\|\|](css-values-4.md#comb-any) flip ] |
| [Initial:](css-cascade-5.md#initial-values) | from-image |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | yes |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | the specified keyword, or an [<angle>](css-values-4.md#angle-value), rounded and normalized (see text), plus optionally a [flip](#valdef-image-orientation-angle) keyword |
| Canonical order: | per grammar |
| Animation type: | discrete |

**This property is *optional* for implementations.**

This property specifies an orthogonal rotation to be applied to the element’s images before they are used in the document. It applies to content images (e.g. replaced elements and generated content) and image sources referenced by SVG elements (such as `[feImage](filter-effects-1.md#elementdef-feimage)`), as well as to decorative images applied via CSS rules (such as [background-image](css-backgrounds-3.md#propdef-background-image) and other [<image>](#typedef-image) properties). It does not apply to the rendering of images outside the document, e.g. favicons in the UA’s navigation toolbars or menus, etc.

Note: This property is not intended to specify layout transformations such as arbitrary rotation or flipping the image in the horizontal or vertical direction. (See [[CSS-TRANSFORMS-1]](#biblio-css-transforms-1) for a feature designed to do that.) It is also not needed to correctly orient an image when printing in landscape versus portrait orientation, as that rotation is done as part of layout. (See [[CSS3PAGE]](#biblio-css3page).) It should only be used to correct incorrectly-oriented images.

Values have the following meanings:

***<a id="valdef-image-orientation-none"></a>none* **
: No additional rotation is applied: the image is oriented as encoded.
***<a id="valdef-image-orientation-from-image"></a>from-image* **
: If the image has an orientation specified in its metadata, such as EXIF, this value computes to the angle that the metadata specifies is necessary to correctly orient the image. If necessary, this angle is then rounded and normalized as described above for an [<angle>](css-values-4.md#angle-value) value. If there is no orientation specified in its metadata, this value computes to [none](#valdef-image-orientation-none).

Note: [§ 2.1.2 Image Metadata](#url-metadata) imposes some restrictions on what metadata can be used.

***<a id="valdef-image-orientation-angle"></a>[<angle>](css-values-4.md#angle-value) || flip* **
: Positive [<angle>](css-values-4.md#angle-value) values cause the image to be rotated to the right (in a clockwise direction), while negative values cause a rotation to the left. If the <angle> is omitted, it defaults to 0deg.

If [flip](#valdef-image-orientation-angle) is specified, after rotation the image is flipped horizontally.

This value only applies to content images; decorative images continue to behave as [from-image](#valdef-image-orientation-from-image). This value is deprecated and is optional for implementations except those conforming to [[CSS-PRINT]](#biblio-css-print).

Note: This value allows all 8 possible EXIF orientations that [from-image](#valdef-image-orientation-from-image) can produce to be manually reproduced.

The computed value of the property is calculated by rounding the [<angle>](css-values-4.md#angle-value) to the nearest quarter turn, rounding towards positive infinity when that’s ambiguous, then moduloing the value by 1turn (so that it lies in the half-open range [0turn, 1turn)).

Values other than [none](#valdef-image-orientation-none) and [from-image](#valdef-image-orientation-from-image) are *optional* to implement and *deprecated* in CSS.

All CSS layout and rendering processes use the image *after* rotation, exactly as if the image were originally encoded in its rotated form. This implies, for example:

-
The [natural](#natural-dimensions) height and width are derived from the rotated rather than the original image dimensions.
-
The height (width) property applies to the vertical (horizontal) dimension of the image, *after* rotation.
-
The hotspot coordinates of an image cursor are relative to the image after rotation.
-
Border images (see [border-image](css-backgrounds-3.md#propdef-border-image)) are sliced after rotation.
-
Other transformations, such as those in [[CSS-TRANSFORMS-1]](#biblio-css-transforms-1), are applied to the image *after* [image-orientation](#propdef-image-orientation) is applied.

<a id="example-5e0592b8"></a> The following example rotates the image 90 degrees clockwise:

```
img.ninety     { image-orientation: 90deg }
...
<img class="ninety" src=...>
```

The same effect could be achieved with, for example, an angle of -270deg or 450deg.

Note: This property previously used [none](#valdef-image-orientation-none) as its initial value. It is believed that using [from-image](#valdef-image-orientation-from-image) as the initial value will produce a generally better user experience, and minimal breakage, but future compat data as UAs attempt to make the change will confirm that. If that is confirmed, then it is likely that this property will be removed from CSS unless use cases other than “correct for incorrect orientation” are raised for its other values.

### <a id="the-image-rendering"></a>5.2. Determining How To Scale an Image: the [image-rendering](#propdef-image-rendering) property

| Name: | *<a id="propdef-image-rendering"></a>image-rendering* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | auto [\|](css-values-4.md#comb-one) smooth \| high-quality \| pixelated \| crisp-edges |
| [Initial:](css-cascade-5.md#initial-values) | auto |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | yes |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | specified keyword |
| Canonical order: | per grammar |
| Animation type: | discrete |

The [image-rendering](#propdef-image-rendering) property provides a hint to the user-agent about what aspects of an image are most important to preserve when the image is scaled, to aid the user-agent in the choice of an appropriate scaling algorithm. When specified on an element, it applies to all images given in properties for the element, such as background images, list-style images, or the content of replaced elements when they represent an image that must be scaled. The values of the image-rendering property are interpreted as follows:

***<a id="valdef-image-rendering-auto"></a>auto* **
: The scaling algorithm is UA-dependent.
***<a id="valdef-image-rendering-smooth"></a>smooth* **
: The image should be scaled with an algorithm that maximizes the appearance of the image. In particular, scaling algorithms that "smooth" colors are acceptable, such as bilinear interpolation. This is intended for images such as photos.
***<a id="valdef-image-rendering-high-quality"></a>high-quality* **
:

Identical to [smooth](#valdef-image-rendering-smooth), but with a preference for higher-quality scaling. If system resources are constrained, images with [high-quality](#valdef-image-rendering-high-quality) should be prioritized over those with any other value, when considering which images to degrade the quality of and to what degree.

This value *does not* prevent the image quality from being degraded when the system resources are constrained. It merely expresses a preference that these images should receive extra scaling resources relative to the [auto](#valdef-image-rendering-auto) images. If all images on the page have [high-quality](#valdef-image-rendering-high-quality) applied, it’s equivalent to all of them having auto applied—​they’re all treated the same.

To get the most value out of [high-quality](#valdef-image-rendering-high-quality), only apply it to the most important images on the page.

***<a id="valdef-image-rendering-pixelated"></a>pixelated* **
:

The image is scaled in a way that preserves the pixelation of the original as much as possible, but allows minor smoothing as necessary to avoid distorting the image when the target size is not a clean multiple of the original.

For each axis independently, first determine the integer multiple of its natural size that puts it closest to the target size and is greater than zero. Scale it to this integer-multiple-size using [nearest neighbor](#nearest-neighbor), then scale it the rest of the way to the target size as for [smooth](#valdef-image-rendering-smooth).

Note: At integer multiples of the natural size, this gives the same results as [crisp-edges](#valdef-image-rendering-crisp-edges). At non-integer multiples, this usually gives better visual results, even for pixel art, but it does incur a performance penalty due to the "two-step" rendering requirement.

***<a id="valdef-image-rendering-crisp-edges"></a>crisp-edges* **
:

The image is scaled in a way that preserves contrast and edges, and which avoids smoothing colors or introducing blur to the image in the process. This is intended for images such as line drawings.

The image *may* be scaled using [nearest neighbor](#nearest-neighbor) or any other UA-chosen algorithm that does not blur edges or blend colors from the source image. It can, however, detect diagonal or curved lines and render them as such (rather than as jagged-looking “giant pixels”).

Note: If the new size is not an integer multiple of the original size, the [nearest neighbor](#nearest-neighbor) algorithm can introduce significant “aliasing” bugs; lines that were the same thickness in the original image might be a pixel thinner or thicker in the scaled image depending on where they appear, etc. For most purposes, [pixelated](#valdef-image-rendering-pixelated) will produce a more suitable rendering than nearest neighbor.

Other than the first step of [pixelated](#valdef-image-rendering-pixelated), this property does not dictate any particular scaling algorithm to be used. For example, with [image-rendering: auto](#propdef-image-rendering), a user agent could scale images with bilinear interpolation by default, switch to nearest-neighbor interpolation in high-load situations, and switch to a higher-quality scaling algorithm like Lanczos interpolation for static images that aren’t moving or changing.

<a id="example-30207ff9"></a> For example, given the following small image:

A small pixel-art image.

Scaling it up 3x might look like the following, depending on the value of [image-rendering](#propdef-image-rendering):

The image scaled 3x with [smooth](#valdef-image-rendering-smooth)

The image scaled 3x with [pixelated](#valdef-image-rendering-pixelated)

The image scaled 3x with [crisp-edges](#valdef-image-rendering-crisp-edges) interpreted with an edge-preserving algorithm. (Interpreting as [nearest neighbor](#nearest-neighbor) would give the same results as [pixelated](#valdef-image-rendering-pixelated) in this case.)

The *<a id="nearest-neighbor"></a>nearest neighbor* (or NN) image scaling algorithm treats the source image’s pixels as literal rectangles of color on the source canvas, then colors each destination pixel by choosing one point in the pixel’s area (usually either the center or top-left) and using the color of the corresponding point on the source canvas.

When the target size is an integer multiple of the source, this results in “big pixels”, as if you’d merely zoomed in on the source image. When the target size is a non-integer multiple, it still produces crisp pixels entirely out of source-image colors (no blending or blurring), but can produce aliasing quirks where the “pixel grid” can appear somewhat irregular.

For example, using [nearest neighbor](#nearest-neighbor) to scale up an image by 2.5x will result in each pixel of the source image being used for two or three pixels of the destination image, in an alternating fashion, while scaling an image down to 0.5x will skip every second pixel in the source image.

<a id="example-b8e65805"></a> At 3x scaling as in the preceding example, both [pixelated](#valdef-image-rendering-pixelated) and pure [nearest neighbor](#nearest-neighbor) give identical results. At scale ratios between integer multiples, however, they’ll act differently:

The same small pixel-art image as before

The image scaled 2.5x with [pixelated](#valdef-image-rendering-pixelated)

The image scaled 2.5x with [crisp-edges](#valdef-image-rendering-crisp-edges) interpreted as [nearest neighbor](#nearest-neighbor)

The [pixelated](#valdef-image-rendering-pixelated) version maintains the overall *look* of simply scaling the pixels up, at the cost of *slight* blurring, though much less blurring than the [smooth](#valdef-image-rendering-smooth) scaling gives. Meanwhile, [nearest neighbor](#nearest-neighbor) avoids introducing any blurring at all, at the cost of aliasing artifacts making the “pixels” look irregularly sized.

This property previously accepted the values optimizeSpeed and optimizeQuality. These are now deprecated; a user agent must accept them as valid values but must treat them as having the same behavior as [crisp-edges](#valdef-image-rendering-crisp-edges) and [smooth](#valdef-image-rendering-smooth) respectively, and authors must not use them.

## <a id="interpolation"></a>6. Interpolation

Interpolation of [<image>](#typedef-image) values is not defined in this level. Implementations must abruptly transition them (at 50% transition progress, like other unsupported interpolations), unless otherwise defined by a future specification.

Note: CSS Images Level 4 is expected to define specialized interpolation for gradients, and define that all other images interpolate by cross-fading.

## <a id="serialization"></a>7. Serialization

This section describes the serialization of all new properties and value types introduced in this specification, for the purpose of interfacing with the CSS Object Model [[CSSOM]](#biblio-cssom).

To serialize any function defined in this module, serialize it per its individual grammar, in the order its grammar is written in, omitting components when possible without changing the meaning, joining space-separated tokens with a single space, and following each serialized comma with a single space.

<a id="example-33854bc0"></a> For example, a gradient specified as:

```
Linear-Gradient( to bottom, red 0%,yellow,black 100px)
```

must serialize as:

```
linear-gradient(red, yellow, black 100px)
```

## <a id="privacy"></a> Privacy Considerations

This specification introduces no new privacy concerns.

## <a id="security"></a> Security Considerations

This specification allows rendering of cross-origin images by default, which exposes some information of those images programmatically—​specifically, the [natural dimensions](#natural-dimensions) and resolution of those images.

## <a id="acknowledgments"></a> Acknowledgments

Thanks to the Webkit team, Brad Kemper, Brian Manthos, and Alan Gresley for their contributions to the definition of gradients; to Melinda Grant for her work on [object-fit](#propdef-object-fit), [object-position](#propdef-object-position), and [image-orientation](#propdef-image-orientation); and to L. David Baron, Kang-Hao Lu, Leif Arne Storset, Erik Dahlstrom, and Øyvind Stenhaug for their careful review, comments, and corrections.

## <a id="changes"></a> Changes

### <a id="changes-20191010"></a> Changes Since the 10 October 2019 Candidate Recommendation

Significant changes since the 18 December 2023 CRD:

-
Allow just one color stop (Issue 10092)
-
Separate first and last stop fixing into separate steps, so it’s clearer what happens with a single stop (Issue 10092)
-
Remove non-normative note about color space metadata; prefer "image" to "file" (Issue 10075)
-
No longer require image metadata to be at the start of the image, if the format doesn’t allow it (Issue 8543)

Significant changes since the 17 December 2020 CRD:

-
Remove && which allows reordering from [<linear-color-stop>](#typedef-linear-color-stop). (Issue 8021)
-
Specify [pixelated](#valdef-image-rendering-pixelated) to use smooth scaling from a [nearest neighbor](#nearest-neighbor) scale to the nearest pixel multiple, in order to avoid distortion. (Issue 5837)
-
Explicitly allow [nearest neighbor](#nearest-neighbor) as the implementation for [crisp-edges](#valdef-image-rendering-crisp-edges), since this is what implementations currently do. (Issue 6038)
-
Various editorial / typo fixes in grammar productions.

Significant changes since the 10 October 2019 CR:

-
Define handling of degenerate aspect ratios (Issue 4572)
-
Define that layout-affecting metadata occurring after the image data should be ignored (Issue 5165)
-
Explicitly allow dithering in gradients (Issue 4793)
-
Define that [image-orientation](#propdef-image-orientation) applies to both decorative and content images (Issue 5245)
-
Rename “intrinsic dimensions” to [natural dimensions](#natural-dimensions) to avoid confusion with intrinsic sizes. (Issue 4961)

### <a id="changes-20120407"></a> Changes Since the 17 April 2012 Candidate Recommendation

Major changes include:

-
The image() notation has been deferred to Level 4.
-
The image-resolution property has been deferred to Level 4.
-
The [image-orientation](#propdef-image-orientation) property has been marked as deprecated, optional, and at-risk. Additionally:
  -
  Added the [from-image](#valdef-image-orientation-from-image) and [none](#valdef-image-orientation-none) keywords
  -
  Added the flip values
  -
  Swapped to "mod then round" ordering.
-
Added the [image-rendering](#propdef-image-rendering) property.
-
Moved the [<resolution>](css-values-4.md#resolution-value) type to [[CSS-VALUES-3]](#biblio-css-values-3).
-
Better defined the handling of invalid and partially-loaded images.
-
Defined the general computed form of [<image>](#typedef-image).
-
Defined concept and handling of URLs that are ambiguous between being images or element references.
-
Defined that the unit can be omitted for zero angles in [linear-gradient()](#funcdef-linear-gradient) due to compat.
-
Slightly clarified handling of degenerate repeating radial gradients.
-
Added [transition hints](#color-transition-hint) to [<color-stop-list>](#typedef-color-stop-list), and editorially rewrote section on gradient color stops to better accommodate the new prose.
-
Added "Canonical Order" and "Animation Type" to all property definition tables.
-
Defined interpolation and serialization of [<image>](#typedef-image) values.
-
Various minor clarifications.

A Disposition of Comments is available.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-color-5"></a>[CSS-COLOR-5] **
: Chris Lilley; Una Kravets; Lea Verou. CSS Color Module Level 5. URL: https://drafts.csswg.org/css-color-5/
**<a id="biblio-css-content-3"></a>[CSS-CONTENT-3] **
: Elika Etemad; Mike Bremford. CSS Generated Content Module Level 3. URL: https://drafts.csswg.org/css-content-3/
**<a id="biblio-css-lists-3"></a>[CSS-LISTS-3] **
: Elika Etemad; Tab Atkins Jr.. CSS Lists and Counters Module Level 3. URL: https://drafts.csswg.org/css-lists-3/
**<a id="biblio-css-masking-1"></a>[CSS-MASKING-1] **
: Dirk Schulze; Brian Birtles; Tab Atkins Jr.. [CSS Masking Module Level 1](css-masking-1.md). URL: [https://drafts.csswg.org/css-masking-1/](css-masking-1.md)
**<a id="biblio-css-print"></a>[CSS-PRINT] **
: Elika Etemad; Melinda Grant. CSS Print Profile. 14 March 2013. NOTE. URL: https://www.w3.org/TR/css-print/
**<a id="biblio-css-sizing-3"></a>[CSS-SIZING-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Box Sizing Module Level 3. URL: https://drafts.csswg.org/css-sizing-3/
**<a id="biblio-css-sizing-4"></a>[CSS-SIZING-4] **
: Tab Atkins Jr.; Elika Etemad; Jen Simmons. CSS Box Sizing Module Level 4. URL: https://drafts.csswg.org/css-sizing-4/
**<a id="biblio-css-ui-4"></a>[CSS-UI-4] **
: Tab Atkins Jr.; Florian Rivoal. CSS Basic User Interface Module Level 4. URL: https://drafts.csswg.org/css-ui-4/
**<a id="biblio-css-values-3"></a>[CSS-VALUES-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-values-5"></a>[CSS-VALUES-5] **
: Tab Atkins Jr.; Elika Etemad; Miriam Suzanne. CSS Values and Units Module Level 5. URL: https://drafts.csswg.org/css-values-5/
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3bg"></a>[CSS3BG] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-filter-effects-1"></a>[FILTER-EFFECTS-1] **
: Dirk Schulze; Dean Jackson. [Filter Effects Module Level 1](filter-effects-1.md). URL: [https://drafts.csswg.org/filter-effects-1/](filter-effects-1.md)
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css-transforms-1"></a>[CSS-TRANSFORMS-1] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css3page"></a>[CSS3PAGE] **
: Elika Etemad. CSS Paged Media Module Level 3. URL: https://drafts.csswg.org/css-page-3/
**<a id="biblio-cssom"></a>[CSSOM] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-selectors-4"></a>[SELECTORS-4] **
: Elika Etemad; Tab Atkins Jr.. [Selectors Level 4](selectors-4.md). URL: [https://drafts.csswg.org/selectors/](selectors-4.md)
**<a id="biblio-smil10"></a>[SMIL10] **
: Philipp Hoschka. Synchronized Multimedia Integration Language (SMIL) 1.0 Specification. URL: https://www.w3.org/TR/1998/REC-smil-19980615/
**<a id="biblio-svg11"></a>[SVG11] **
: Erik Dahlström; et al. [Scalable Vector Graphics (SVG) 1.1 (Second Edition)](svg11/index.md). 16 August 2011. REC. URL: [https://www.w3.org/TR/SVG11/](svg11/index.md)
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [image-orientation](#propdef-image-orientation) | from-image \| none \| [ <angle> \|\| flip ] | from-image | all elements | yes | n/a | discrete | per grammar | the specified keyword, or an <angle>, rounded and normalized (see text), plus optionally a flip keyword |
| [image-rendering](#propdef-image-rendering) | auto \| smooth \| high-quality \| pixelated \| crisp-edges | auto | all elements | yes | n/a | discrete | per grammar | specified keyword |
| [object-fit](#propdef-object-fit) | fill \| contain \| cover \| none \| scale-down | fill | replaced elements | no | n/a | discrete | per grammar | specified keyword |
| [object-position](#propdef-object-position) | <position> | 50% 50% | replaced elements | no | refer to width and height of element itself | as for background-position | the horizontal component of the <position>, followed by the vertical component | as for background-position |
