<!-- https://drafts.csswg.org/filter-effects-1/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>Filter Effects Module Level 1

<a id="w3c-state"></a>Editor’s Draft, 22 September 2026

More details about this document

**This version: **
: https://drafts.csswg.org/filter-effects-1/
**Latest published version: **
: https://www.w3.org/TR/filter-effects-1/
**Previous Versions: **
: https://www.w3.org/TR/2018/WD-filter-effects-1-20181124/
: https://www.w3.org/TR/2014/WD-filter-effects-1-20141125/
: https://www.w3.org/TR/2013/WD-filter-effects-1-20131126/
: https://www.w3.org/TR/2013/WD-filter-effects-20130523/
: https://www.w3.org/TR/2012/WD-filter-effects-20121025/
**Test Suite: **
: https://wpt.fyi/css/filter-effects
**Feedback: **
: CSSWG Issues Repository
: [Inline In Spec](#issues-index)
**Editors: **
: Dirk Schulze (Adobe Inc.)
: Chris Harrelson (Google)
**Former Editors: **
: Dean Jackson (Apple Inc.)
: Vincent Hardy
: Erik Dahlström (Invited Expert)
**Suggest an Edit for this Spec: **
: GitHub Editor
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

Filter effects are a way of processing an element’s rendering before it is displayed in the document. Typically, rendering an element via CSS or SVG can conceptually be described as if the element, including its children, are drawn into a buffer (such as a raster image) and then that buffer is composited into the elements parent. Filters apply an effect before the compositing stage. Examples of such effects are blurring, changing color intensity and warping the image.

Although originally designed for use in SVG, filter effects are a set of operations to apply on an image buffer and therefore can be applied to nearly any presentational environment, including CSS. They are triggered by a style instruction (the [filter](#propdef-filter) property). This specification describes filters in a manner that allows them to be used in content styled by CSS, such as HTML and SVG. It also defines a CSS property value function that produces a CSS [<image>](css-images-3.md#typedef-image) value.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “filter-effects” in the title, like this: “[filter-effects] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This section is not normative*

A filter effect is a graphical operation that is applied to an element as it is drawn into the document. It is an image-based effect, in that it takes zero or more images as input, a number of parameters specific to the effect, and then produces an image as output. The output image is either rendered into the document instead of the original element, used as an input image to another filter effect, or provided as a CSS image value.

A simple example of a filter effect is a “flood”. It takes no image inputs but has a parameter defining a color. The effect produces an output image that is completely filled with the given color. A slightly more complex example is an “inversion” which takes a single image input (typically an image of the element as it would normally be rendered into its parent) and adjusts each pixel such that they have the opposite color values.

Filter effects are exposed with two levels of complexity:

1.
A small set of canned filter functions that are given by name. While not particularly powerful, these are convenient and easily understood and provide a simple approach to achieving common effects, such as blurring. The canned filters can also be animated by [[CSS3-ANIMATIONS]](#biblio-css3-animations).
2.
A graph of individual filter effects described in markup that define an overall effect. The graph is agnostic to its input in that the effect can be applied to any content. While such graphs are the combination of effects that may be simple in isolation, the graph as a whole can produce complex effects. An example is given below.

<a id="example-c621d171"></a> In this example, an image is filtered with the [grayscale()](#funcdef-filter-grayscale) filter function.

```
#image {
    filter: grayscale(100%);
}
```

[image: Example for grayscale filter applied to image]

An image without filter (left) and the same filter with a 100% grayscale filter (right).

<a id="example-f7874f10"></a> The following shows an example of graph of individual filter effects.

[image: Example Filter]

Initial example for a filtered object.

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/filters01.svg)

The filter effect used in the example above is repeated here with reference numbers in the left column before each of the six filter primitives:

```
       <filter id="MyFilter" filterUnits="userSpaceOnUse" x="0" y="0" width="200" height="120">
  <desc>Produces a 3D lighting effect.</desc>
  <feGaussianBlur in="SourceAlpha" stdDeviation="4" result="blur"/>
  <feOffset in="blur" dx="4" dy="4" result="offsetBlur"/>
  <feSpecularLighting in="blur" surfaceScale="5" specularConstant=".75"
                      specularExponent="20" lighting-color="#bbbbbb"
                      result="specOut">
    <fePointLight x="-5000" y="-10000" z="20000"/>
  </feSpecularLighting>
  <feComposite in="specOut" in2="SourceAlpha" operator="in" result="specOut"/>
  <feComposite in="SourceGraphic" in2="specOut" operator="arithmetic"
               k1="0" k2="1" k3="1" k4="0" result="litPaint"/>
  <feMerge>
    <feMergeNode in="offsetBlur"/>
    <feMergeNode in="litPaint"/>
  </feMerge>
</filter>
```

| 1 2 3 4 5 6 |  |
| --- | --- |

The following pictures show the intermediate image results from each of the six filter elements:

[image: filters01 - original source graphic]

Source graphic

[image: filters01 - after filter element 1]

After filter primitive 1

[image: filters01 - after filter element 2]

After filter primitive 2

[image: filters01 - after filter element 3]

After filter primitive 3

[image: filters01 - after filter element 4]

After filter primitive 4

[image: filters01 - after filter element 5]

After filter primitive 5

[image: filters01 - after filter element 6]

After filter primitive 6

1.
Filter primitive [feGaussianBlur](#elementdef-fegaussianblur) takes input [SourceAlpha](#attr-valuedef-in-sourcealpha), which is the alpha channel of the source graphic. The result is stored in a temporary buffer named "blur". Note that "blur" is used as input to both filter primitives 2 and 3.
2.
Filter primitive [feOffset](#elementdef-feoffset) takes buffer "blur", shifts the result in a positive direction in both x and y, and creates a new buffer named "offsetBlur". The effect is that of a drop shadow.
3.
Filter primitive [feSpecularLighting](#elementdef-fespecularlighting), uses buffer "blur" as a model of a surface elevation and generates a lighting effect from a single point source. The result is stored in buffer "specOut".
4.
Filter primitive [feComposite](#elementdef-fecomposite) masks out the result of filter primitive 3 by the original source graphics alpha channel so that the intermediate result is no bigger than the original source graphic.
5.
Filter primitive [feComposite](#elementdef-fecomposite) composites the result of the specular lighting with the original source graphic.
6.
Filter primitive [feMerge](#elementdef-femerge) composites two layers together. The lower layer consists of the drop shadow result from filter primitive 2. The upper layer consists of the specular lighting result from filter primitive 5.

## <a id="placement"></a>2. Module interactions

This specification defines a set of CSS properties that affect the visual rendering of elements to which those properties are applied; these effects are applied after elements have been sized and positioned according to the Visual formatting model from [[CSS21]](#biblio-css21). Some values of these properties result in the creation of a containing block, and/or the creation of a stacking context.

The compositing model follows the SVG compositing model [[SVG11]](#biblio-svg11): first any filter effect is applied, then any clipping, masking and opacity [[CSS3COLOR]](#biblio-css3color). These effects all apply after any other CSS effects such as [border](css-backgrounds-3.md#propdef-border) [[CSS3BG]](#biblio-css3bg).

Some property and element definitions in this specification require an SVG 1.1 implementation [[SVG11]](#biblio-svg11). UAs without support for SVG must not implement the [color-interpolation-filters](#propdef-color-interpolation-filters), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity) and [lighting-color](#propdef-lighting-color) properties as well as the [filter](#elementdef-filter) element, the [feMergeNode](#elementdef-femergenode) element, the [transfer function elements](#transfer-function-element) and the [filter primitive](#filter-primitive) elements.

## <a id="values"></a>3. Values

This specification follows the CSS property definition conventions from [[CSS21]](#biblio-css21). Value types not defined in these specifications are defined in CSS Values and Units Module Level 3 [[CSS3VAL]](#biblio-css3val).

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the inherit keyword as their property value. For readability it has not been repeated explicitly.

## <a id="definitions"></a>4. Terminology

When used in this specification, terms have the meanings assigned in this section.

***<a id="filter-primitive"></a>filter primitive*, *<a id="elementdef-filter-primitive"></a>`filter-primitive`* **
:

The set of elements that control the output of a [filter](#elementdef-filter) element, particularly: [feSpotLight](#elementdef-fespotlight), [feBlend](#elementdef-feblend), [feColorMatrix](#elementdef-fecolormatrix), [feComponentTransfer](#elementdef-fecomponenttransfer), [feComposite](#elementdef-fecomposite), [feConvolveMatrix](#elementdef-feconvolvematrix), [feDiffuseLighting](#elementdef-fediffuselighting), [feDisplacementMap](#elementdef-fedisplacementmap), [feDropShadow](#elementdef-fedropshadow), [feFlood](#elementdef-feflood), [feGaussianBlur](#elementdef-fegaussianblur), [feImage](#elementdef-feimage), [feMerge](#elementdef-femerge), [feMorphology](#elementdef-femorphology), [feOffset](#elementdef-feoffset), [feSpecularLighting](#elementdef-fespecularlighting), [feTile](#elementdef-fetile), [feTurbulence](#elementdef-feturbulence).

***<a id="pass-through-filter"></a>pass through filter* **
:

The pass through filter output is equal to the primary input of the filter primitive.

## <a id="FilterProperty"></a>5. Graphic filters: the [filter](#propdef-filter) property

| Name: | *<a id="propdef-filter"></a>filter* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | none [\|](css-values-4.md#comb-one) [<filter-value-list>](#typedef-filter-value-list) |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg2/struct.md#container-element) without the [defs](svg2/struct.md#elementdef-defs) element, all [graphics elements](svg2/struct.md#graphics-element) and the [use](svg2/struct.md#elementdef-use) element. |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | See prose in [Animation of Filters](#animation-of-filters). |
| Media: | visual |

The [filter](#propdef-filter) property applies a filter, specified either by an SVG reference or by a [filter function](#filter-function), to an element, altering how it paints.

```
<a id="typedef-filter-value-list"></a><filter-value-list> = [ <filter-function> | <url> ]+
```

***<a id="typedef-filter-url"></a><url>* **
:

A filter reference to a [filter](#elementdef-filter) element. For example url(commonfilters.svg#filter). If the filter references a non-existent object or the referenced object is not a filter element, then the whole filter chain is ignored. No filter is applied to the object.

**[<filter-function>](#typedef-filter-function) **
:

See [filter functions](#filter-function).

**none **
:

No filter effect gets applied.

A value other than none for the [filter](#propdef-filter) property results in the creation of a containing block for absolute and fixed positioned descendants unless the element it applies to is a document root element in the current browsing context. The list of functions are applied in the order provided.

The first filter function or [filter](#elementdef-filter) reference in the list takes the element ([SourceGraphic](#attr-valuedef-in-sourcegraphic)) as the input image. Subsequent operations take the output from the previous filter function or filter reference as the input image. filter element reference functions can specify an alternate input, but still uses the previous output as its SourceGraphic.

[color-interpolation-filters](#propdef-color-interpolation-filters) has no affect for Filter Functions. Filter Functions must operate in the sRGB color space.

A computed value of other than none results in the creation of a stacking context [[CSS21]](#biblio-css21) the same way that CSS [opacity](css-color-4.md#propdef-opacity) does. All the elements descendants are rendered together as a group with the filter effect applied to the group as a whole.

The [filter](#propdef-filter) property has no effect on the geometry of the target element’s CSS boxes, although filter can contribute to the element’s ink overflow area.

Conceptually, any parts of the drawing are effected by filter operations. This includes any content, background, borders, text decoration, outline and visible scrolling mechanism of the element to which the filter is applied, and those of its descendants. The filter operations are applied in the element’s [local coordinate system](css-transforms-1.md#local-coordinate-system).

The compositing model follows the [SVG compositing model](svg11/render.md#Introduction) [[SVG11]](#biblio-svg11): first any filter effect is applied, then any clipping, masking and opacity. As per SVG, the application of [filter](#propdef-filter) has no effect on hit-testing.

The [filter](#propdef-filter) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

<a id="issue-95bcf4be"></a> How does filter behave on fixed background images? [Issue #238]

## <a id="filter-functions"></a>6. Filter Functions

The following *<a id="filter-function"></a>filter functions* specify a variety of simple filters, equivalent to particular SVG `[filter](#elementdef-filter)` elements.

### <a id="supported-filter-functions"></a>6.1. Supported Filter Functions

```
<a id="typedef-filter-function"></a><filter-function> = <blur()> | <brightness()> | <contrast()> | <drop-shadow()> |
  <grayscale()> | <hue-rotate()> | <invert()> | <opacity()> | <sepia()> | <saturate()>
```

Unless defined otherwise, omitted values default to the [initial value](svg2/types.md#TermInitialValue) for interpolation.

Note: For some filter functions the default value for omitted values differes from their [initial value](svg2/types.md#TermInitialValue) for interpolation. For the convenience of content creators, the default value for omitted values for [grayscale()](#funcdef-filter-grayscale), [sepia()](#funcdef-filter-sepia) and [invert()](#funcdef-filter-invert) is 1 (apply the effect to 100%) while the initial value for interpolation is 0 (no effect).

<a id="FilterFunction"></a>

**

```
<a id="funcdef-filter-blur"></a>blur() = blur( <length>? )
```

**
:

Applies a Gaussian blur to the input image. The passed parameter defines the value of the standard deviation to the Gaussian function. The parameter is specified a CSS length, but does not accept percentage values. The markup equivalent of this function is [given below](#blurEquivalent).

Negative values are not allowed.

Default value when omitted is 0px.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 0px.

Note: Standard deviation is different to [box-shadow](css-backgrounds-3.md#propdef-box-shadow) s blur radius.

The ink overflow rectangle for a Gaussian blur is the axis-aligned boundary of the area in which the implementation draws the blur.

Note: A true Gaussian blur has theoretically infinite extent, but in practice all implementations use a finite-area approximation of a Gaussian blur. At the time of writing (January 2024) all major implementations use the familiar three-pass box blur approximation, which has extent: `((3 * sqrt(2 * π) / 4) * σ)`.

**

```
<a id="funcdef-filter-brightness"></a>brightness() = brightness( [ <number> |  <percentage> ]? )
```

**
:

Applies a linear multiplier to input image, making it appear more or less bright. A value of 0% will create an image that is completely black. A value of 100% leaves the input unchanged. Other values are linear multipliers on the effect. Values of amount over 100% are allowed, providing brighter results. The markup equivalent of this function is [given below](#brightnessEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 1.

**

```
<a id="funcdef-filter-contrast"></a>contrast() = contrast( [ <number> |  <percentage> ]? )
```

**
:

Adjusts the contrast of the input. A value of 0% will create an image that is completely gray. A value of 100% leaves the input unchanged. Values of amount over 100% are allowed, providing results with more contrast. The markup equivalent of this function is [given below](#contrastEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 1.

**

```
<a id="funcdef-filter-drop-shadow"></a>drop-shadow() = drop-shadow( [ <color>? && <length>{2,3} ] )
```

**
:

Applies a drop shadow effect to the input image. A drop shadow is effectively a blurred, offset version of the input image’s alpha mask drawn in a particular color, composited below the image. Values are interpreted as for [box-shadow](css-backgrounds-3.md#propdef-box-shadow) [[CSS3BG]](#biblio-css3bg) but with the optional 3rd [<length>](css-values-4.md#length-value) value being the standard deviation instead of blur radius. The markup equivalent of this function is [given below](#dropshadowEquivalent).

The default value for omitted values is missing length values set to 0 and the missing used color is taken from the color property.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is all length values set to 0 and the used color set to [transparent](css-color-4.md#valdef-color-transparent).

Note: Spread values or multiple shadows are not accepted for this level of the specification.

Note: Standard deviation is different to [box-shadow](css-backgrounds-3.md#propdef-box-shadow) s blur radius.

The ink overflow rectangle for a drop shadow is the extent of the offsets, plus the extent of the blur (if any) as described for [blur()](#funcdef-filter-blur).

**

```
<a id="funcdef-filter-grayscale"></a>grayscale() = grayscale( [ <number> |  <percentage> ]? )
```

**
:

Converts the input image to grayscale. The passed parameter defines the proportion of the conversion. A value of 100% is completely grayscale. A value of 0% leaves the input unchanged. Values between 0% and 100% are linear multipliers on the effect. Values of amount over 100% are allowed but UAs must clamp the values to 1. The markup equivalent of this function is [given below](#grayscaleEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 0.

**

```
<a id="funcdef-filter-hue-rotate"></a>hue-rotate() = hue-rotate( [ <angle> | <zero> ]? )
```

**
:

Applies a hue rotation on the input image. The passed parameter defines the number of degrees around the color circle the input samples will be adjusted. A value of 0deg leaves the input unchanged. Implementations must not normalize this value in order to allow animations beyond 360deg. The markup equivalent of this function is [given below](#huerotateEquivalent).

The unit identifier may be omitted if the [<angle>](css-values-4.md#angle-value) is zero.

Default value when omitted is 0deg.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 0deg.

**

```
<a id="funcdef-filter-invert"></a>invert() = invert( [ <number> |  <percentage> ]? )
```

**
:

Inverts the samples in the input image. The passed parameter defines the proportion of the conversion. A value of 100% is completely inverted. A value of 0% leaves the input unchanged. Values between 0% and 100% are linear multipliers on the effect. Values of amount over 100% are allowed but UAs must clamp the values to 1. The markup equivalent of this function is [given below](#invertEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 0.

**

```
<a id="funcdef-filter-opacity"></a>opacity() = opacity( [ <number> |  <percentage> ]? )
```

**
:

Applies transparency to the samples in the input image. The passed parameter defines the proportion of the conversion. A value of 0% is completely transparent. A value of 100% leaves the input unchanged. Values between 0% and 100% are linear multipliers on the effect. This is equivalent to multiplying the input image samples by amount. Values of amount over 100% are allowed but UAs must clamp the values to 1. The markup equivalent of this function is [given below](#opacityEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 1.

Note: The opacity filter function is not meant to be a shorthand of the [opacity](css-color-4.md#propdef-opacity) property. Furthermore, it allows setting the transparency of intermediate filter primitive results before passing to the next filter primitive. If the opacity filter function is set as last filter primitive, the value of the opacity property is multiplied on top of the value of the filter function, which may result in a more transparent content.

**

```
<a id="funcdef-filter-saturate"></a>saturate() = saturate( [ <number> |  <percentage> ]? )
```

**
:

Saturates the input image. The passed parameter defines the proportion of the conversion. A value of 0% is completely un-saturated. A value of 100% leaves the input unchanged. Other values are linear multipliers on the effect. Values of amount over 100% are allowed, providing super-saturated results. The markup equivalent of this function is [given below](#saturateEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 1.

**

```
<a id="funcdef-filter-sepia"></a>sepia() = sepia( [ <number> |  <percentage> ]? )
```

**
:

Converts the input image to sepia. The passed parameter defines the proportion of the conversion. A value of 100% is completely sepia. A value of 0% leaves the input unchanged. Values between 0% and 100% are linear multipliers on the effect. Values of amount over 100% are allowed but UAs must clamp the values to 1. The markup equivalent of this function is [given below](#sepiaEquivalent).

Negative values are not allowed.

Default value when omitted is 1.

The [initial value](svg2/types.md#TermInitialValue) for interpolation is 0.

### <a id="computed-values-of-filter-functions"></a>6.2. Computed Values of Filter Functions

The values in a [<filter-function>](#typedef-filter-function) are computed as specified, with these exceptions:

-
Omitted values are included and compute to their defaults.
-
[drop-shadow()](#funcdef-filter-drop-shadow) starts with the computed value of <color> followed by the computed value of the [<length>](css-values-4.md#length-value) values.

### <a id="serialization-of-filter-functions"></a>6.3. Serialization of Filter Functions

To serialize the [<filter-function>](#typedef-filter-function), serialize as per their individual grammars, in the order the grammars are written in, avoiding [calc()](css-values-4.md#funcdef-calc) expressions where possible, serialize filter arguments as specified, avoiding calc() transformations, joining space-separated tokens with a single space, and following each serialized comma with a single space.

### <a id="interpolation-of-filter-functions"></a>6.4. Interpolation of Filter Functions

For interpolation of values in [<filter-function>](#typedef-filter-function)s, the steps corresponding to the first matching condition in the following list must be run:

**[blur()](#funcdef-filter-blur) **
:

Interpolate values as length.

**[brightness()](#funcdef-filter-brightness) **
**[contrast()](#funcdef-filter-contrast) **
**[grayscale()](#funcdef-filter-grayscale) **
**[invert()](#funcdef-filter-invert) **
**[opacity()](#funcdef-filter-opacity) **
**[saturate()](#funcdef-filter-saturate) **
**[sepia()](#funcdef-filter-sepia) **
:

Convert percentage values to numbers with 0% being relative to 0 and 100% relative to 1. Interpolate values number.

**[hue-rotate()](#funcdef-filter-hue-rotate) **
:

Interpolate values as number.

**[drop-shadow()](#funcdef-filter-drop-shadow) **
:

Interpolate values as shadow list.

## <a id="FilterElement"></a>7. SVG Filter Sources: the [filter](#elementdef-filter) element

| Name: | *<a id="elementdef-filter"></a>`filter`* |
| --- | --- |
| Categories: | None. |
| Content model: | Any number of the following elements, in any order: - [descriptive](svg11/intro.md#TermDescriptiveElement) — [desc](svg2/struct.md#elementdef-desc), [title](svg2/struct.md#elementdef-title), [metadata](svg2/struct.md#elementdef-metadata) - [filter primitive](#elementdef-filter-primitive) — [feBlend](#elementdef-feblend), [feFlood](#elementdef-feflood), [feColorMatrix](#elementdef-fecolormatrix), [feComponentTransfer](#elementdef-fecomponenttransfer), [feComposite](#elementdef-fecomposite), [feConvolveMatrix](#elementdef-feconvolvematrix), [feDiffuseLighting](#elementdef-fediffuselighting), [feDisplacementMap](#elementdef-fedisplacementmap), [feDropShadow](#elementdef-fedropshadow), [feGaussianBlur](#elementdef-fegaussianblur), [feImage](#elementdef-feimage), [feMerge](#elementdef-femerge), [feMorphology](#elementdef-femorphology), [feOffset](#elementdef-feoffset), [feSpecularLighting](#elementdef-fespecularlighting), [feTile](#elementdef-fetile), [feTurbulence](#elementdef-feturbulence) - animate - [script](svg2/interact.md#elementdef-script) - set |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [externalResourcesRequired](svg11/struct.md#ExternalResourcesRequiredAttribute) - [x](#element-attrdef-filter-x) - [y](#element-attrdef-filter-y) - [width](#element-attrdef-filter-width) - [height](#element-attrdef-filter-height) - [filterUnits](#element-attrdef-filter-filterunits) - [primitiveUnits](#element-attrdef-filter-primitiveunits) |
| DOM Interfaces: | [SVGFilterElement](#InterfaceSVGFilterElement) |

The description of the [filter](#elementdef-filter) element follows:

*<a id="typedef-number-optional-number"></a><number-optional-number>* = [<number>](css-values-4.md#number-value) <number>?

*Attribute definitions:*

***<a id="element-attrdef-filter-filterunits"></a>`filterUnits`* = "*<a id="attr-valuedef-filterunits-userspaceonuse"></a>`userSpaceOnUse`* | *<a id="attr-valuedef-filterunits-objectboundingbox"></a>`objectBoundingBox`*" **
:

See [filter region](#filter-region).

***<a id="element-attrdef-filter-primitiveunits"></a>`primitiveUnits`* = "*<a id="attr-valuedef-primitiveunits-userspaceonuse"></a>`userSpaceOnUse`* | *<a id="attr-valuedef-primitiveunits-objectboundingbox"></a>`objectBoundingBox`*" **
:

Specifies the coordinate system for the various length values within the [filter primitives](#filter-primitive) and for the attributes that define the [filter primitive subregion](#filter-primitive-subregion).

If [primitiveUnits](#element-attrdef-filter-primitiveunits) is equal to [userSpaceOnUse](#attr-valuedef-primitiveunits-userspaceonuse), any length values within the filter definitions represent values in the current [local coordinate system](css-transforms-1.md#local-coordinate-system) in place at the time when the [filter](#elementdef-filter) element is referenced (i.e., the user coordinate system for the element referencing the filter element via a [filter](#propdef-filter) property).

If [primitiveUnits](#element-attrdef-filter-primitiveunits) is equal to [objectBoundingBox](#attr-valuedef-primitiveunits-objectboundingbox), then any length values within the filter definitions represent fractions or percentages of the bounding box on the referencing element (see [object bounding box units](svg2/coords.md#ObjectBoundingBoxUnits)). Note that if only one number was specified in a [<number-optional-number>](#typedef-number-optional-number) value this number is expanded out before the primitiveUnits computation takes place.

The [initial value](svg2/types.md#TermInitialValue) for [primitiveUnits](#element-attrdef-filter-primitiveunits) is [userSpaceOnUse](#attr-valuedef-primitiveunits-userspaceonuse).

Animatable: yes.

***<a id="element-attrdef-filter-x"></a>`x`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

See [filter region](#filter-region).

***<a id="element-attrdef-filter-y"></a>`y`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

See [filter region](#filter-region).

***<a id="element-attrdef-filter-width"></a>`width`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

See [filter region](#filter-region).

***<a id="element-attrdef-filter-height"></a>`height`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

See [filter region](#filter-region).

***<a id="element-attrdef-filter-filterres"></a>`filterRes`* = "[<number-optional-number>](#typedef-number-optional-number)" **
:

The *filterRes* attribute was removed from the specification. See SVG 1.1 specification for the defintion [[SVG11]](#biblio-svg11).

Properties inherit into the [filter](#elementdef-filter) element from its ancestors; properties do *not* inherit from the element referencing the filter element.

[filter](#elementdef-filter) elements are never rendered directly; their only usage is as something that can be referenced using the [filter](#propdef-filter) property. The display property does not apply to the filter element; thus, filter elements are not directly rendered even if the display property is set to a value other than none, and filter elements are available for referencing even when the display property on the filter element or any of its ancestors is set to none.

## <a id="FilterEffectsRegion"></a>8. Filter Region

A [filter](#elementdef-filter) element can define a *<a id="filter-region"></a>filter region* on the canvas to which a given filter effect applies and can provide a resolution for any intermediate continuous tone images used to process any raster-based [filter primitives](#filter-primitive). The filter element has the following attributes which work together to define the filter region:

**[filterUnits](#element-attrdef-filter-filterunits) **
:

Defines the coordinate system for attributes [x](#element-attrdef-filter-x), [y](#element-attrdef-filter-y), [width](#element-attrdef-filter-width), [height](#element-attrdef-filter-height).

If [filterUnits](#element-attrdef-filter-filterunits) is equal to [userSpaceOnUse](#attr-valuedef-filterunits-userspaceonuse), [x](#element-attrdef-filter-x), [y](#element-attrdef-filter-y), [width](#element-attrdef-filter-width), [height](#element-attrdef-filter-height) represent values in the current user coordinate system in place at the time when the [filter](#elementdef-filter) element is referenced (i.e., the user coordinate system for the element referencing the filter element via a [filter](#propdef-filter) property).

If [filterUnits](#element-attrdef-filter-filterunits) is equal to [objectBoundingBox](#attr-valuedef-filterunits-objectboundingbox), then [x](#element-attrdef-filter-x), [y](#element-attrdef-filter-y), [width](#element-attrdef-filter-width), [height](#element-attrdef-filter-height) represent fractions or percentages of the bounding box on the referencing element (see [object bounding box units](svg2/coords.md#ObjectBoundingBoxUnits)).

The [initial value](svg2/types.md#TermInitialValue) for [filterUnits](#element-attrdef-filter-filterunits) is [objectBoundingBox](#attr-valuedef-filterunits-objectboundingbox).

Animatable: yes.

**[x](#element-attrdef-filter-x), [y](#element-attrdef-filter-y), [width](#element-attrdef-filter-width), [height](#element-attrdef-filter-height) **
:

These attributes define a rectangular region on the canvas to which this filter applies.

The coordinate system for these attributes depends on the value for attribute [filterUnits](#element-attrdef-filter-filterunits).

The bounds of this rectangle act as a hard clipping region for each [filter primitive](#filter-primitive) included with a given [filter](#elementdef-filter) element; thus, if the effect of a given filter primitive would extend beyond the bounds of the rectangle (this sometimes happens when using a [feGaussianBlur](#elementdef-fegaussianblur) filter primitive with a very large [stdDeviation](#element-attrdef-fegaussianblur-stddeviation)), parts of the effect will get clipped.

The [initial value](svg2/types.md#TermInitialValue) for [x](#element-attrdef-filter-x) and [y](#element-attrdef-filter-y) is -10%.

The [initial value](svg2/types.md#TermInitialValue) for [width](#element-attrdef-filter-width) and [height](#element-attrdef-filter-height) is 120%.

ng of the element which referenced the filter.

Animatable: yes.

Note: Both of the two possible value for [filterUnits](#element-attrdef-filter-filterunits) (i.e., [objectBoundingBox](#attr-valuedef-filterunits-objectboundingbox) and [userSpaceOnUse](#attr-valuedef-filterunits-userspaceonuse)) result in a [filter region](#filter-region) whose coordinate system has its X-axis and Y-axis each parallel to the X-axis and Y-axis, respectively, of the [local coordinate system](css-transforms-1.md#local-coordinate-system) for the element to which the filter will be applied.

Note: Sometimes implementers can achieve faster performance when the filter region can be mapped directly to device pixels; thus, for best performance on display devices, it is suggested that authors define their region such that the user agent can align the [filter region](#filter-region) pixel-for-pixel with the background. In particular, for best filter effects performance, avoid rotating or skewing the user coordinate system.

Note: It is often necessary to provide padding space because the filter effect might impact bits slightly outside the tight-fitting [bounding box](svg2/coords.md#bounding-box) on a given object. For these purposes, it is possible to provide negative percentage values for [x](#element-attrdef-filter-x), [y](#element-attrdef-filter-y) and percentages values greater than 100% for [width](#element-attrdef-filter-width), [height](#element-attrdef-filter-height). This, for example, is why the defaults for the filter region are `x="-10%" y="-10%" width="120%" height="120%"`.

## <a id="FilterPrimitivesOverview"></a>9. Filter primitives

### <a id="FilterPrimitivesOverviewIntro"></a>9.1. Overview

This section describes the various filter primitives that can be assembled to achieve a particular filter effect.

Unless otherwise stated, all image filters operate on premultiplied RGBA samples. Some filters like [feColorMatrix](#elementdef-fecolormatrix) and [feComponentTransfer](#elementdef-fecomponenttransfer) work more naturally on non-premultiplied data. For the time of the filter operation, all color values must temporarily be transformed to the required color multiplication of the current filter.

Note: All input images are assumed to be in premultiplied RGBA. User agents may optimize performance by using non-premultiplied data buffering.

All raster effect filtering operations take 1 to N input RGBA images, additional attributes as parameters, and produce a single output RGBA image.

The RGBA result from each filter primitive will be clamped into the allowable ranges for colors and opacity values. Thus, for example, the result from a given [filter primitive](#filter-primitive) will have any negative color values or opacity values adjusted up to color/opacity of zero.

<a id="filtersColorSpace"></a>The color space in which a particular [filter primitive](#filter-primitive) performs its operations is determined by the value of the property [color-interpolation-filters](#propdef-color-interpolation-filters) on the given filter primitive. A different property, [color-interpolation](svg2/painting.md#ColorInterpolationProperty) determines the color space for other color operations. Because these two properties have different initial values (color-interpolation-filters has an initial value of [linearRGB](#valdef-color-interpolation-filters-linearrgb) whereas color-interpolation has an initial value of [sRGB](#valdef-color-interpolation-filters-srgb)), in some cases to achieve certain results (e.g., when coordinating gradient interpolation with a filtering operation) it will be necessary to explicitly set color-interpolation to linearRGB or color-interpolation-filters to sRGB on particular elements. Note that the examples below do not explicitly set either color-interpolation or color-interpolation-filters, so the initial values for these properties apply to the examples.

Sometimes [filter primitives](#filter-primitive) result in undefined pixels. For example, filter primitive [feOffset](#elementdef-feoffset) can shift an image down and to the right, leaving undefined pixels at the top and left. In these cases, the undefined pixels are set to transparent black.

To provide high quality rendering, all filter primitives should operate in a device dependent coordinate space, the *<a id="operating-coordinate-space"></a>operating coordinate space*, taking device pixel density, user space transformations and zooming into account. To provide a platform independent alignment, attribute and property values are often relative to a coordinate system described by the [primitiveUnits](#element-attrdef-filter-primitiveunits) attribute. User agents must scale these relative attributes and properties to the [operating coordinate space](#operating-coordinate-space).

Note: On high resolution devices, attribute and property values that are relative to the [primitiveUnits](#element-attrdef-filter-primitiveunits) usually need to be scaled up. User agents may reduce the resolution of filter primitives on limited platform resources.

Note: Some attribute or property values from the filter primitives [feConvolveMatrix](#elementdef-feconvolvematrix) and [light sources](#light-source) can not be mapped from the coordinate space defined by the [primitiveUnits](#element-attrdef-filter-primitiveunits) attribute to the [operating coordinate space](#operating-coordinate-space).

### <a id="CommonAttributes"></a>9.2. Common filter primitive attributes

The following *<a id="filter-primitive-attributes"></a>filter primitive attributes* are available for all filter primitives:

*Attribute definitions:*

***<a id="element-attrdef-filter-primitive-x"></a>`x`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The minimum x coordinate for the subregion which restricts calculation and rendering of the given [filter primitive](#filter-primitive). See [filter primitive subregion](#filter-primitive-subregion).

The [initial value](svg2/types.md#TermInitialValue) for [x](#element-attrdef-filter-primitive-x) is 0%.

Animatable: yes.

***<a id="element-attrdef-filter-primitive-y"></a>`y`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The minimum y coordinate for the subregion which restricts calculation and rendering of the given [filter primitive](#filter-primitive). See [filter primitive subregion](#filter-primitive-subregion).

The [initial value](svg2/types.md#TermInitialValue) for [y](#element-attrdef-filter-primitive-y) is 0%.

Animatable: yes.

***<a id="element-attrdef-filter-primitive-width"></a>`width`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The width of the subregion which restricts calculation and rendering of the given [filter primitive](#filter-primitive). See [filter primitive subregion](#filter-primitive-subregion).

A negative or zero value disables the effect of the given filter primitive (i.e., the result is a transparent black image).

The [initial value](svg2/types.md#TermInitialValue) for [width](#element-attrdef-filter-primitive-width) is 100%.

Animatable: yes.

***<a id="element-attrdef-filter-primitive-height"></a>`height`* = "[<length-percentage>](css-values-4.md#typedef-length-percentage)" **
:

The height of the subregion which restricts calculation and rendering of the given [filter primitive](#filter-primitive). See [filter primitive subregion](#filter-primitive-subregion).

A negative or zero value must disable the effect of the given filter primitive (i.e., the result is a transparent black image).

The [initial value](svg2/types.md#TermInitialValue) for [height](#element-attrdef-filter-primitive-height) is 100%.

Animatable: yes.

***<a id="element-attrdef-filter-primitive-result"></a>`result`* = "**<a id="typedef-result-filter-primitive-reference"></a><filter-primitive-reference>**" **
:

[<filter-primitive-reference>](#typedef-result-filter-primitive-reference) is an [<custom-ident>](css-values-4.md#identifier-value) [[CSS3VAL]](#biblio-css3val) and an assigned name for this [filter primitive](#filter-primitive). If supplied, then graphics that result from processing this filter primitive can be referenced by an [in](#element-attrdef-filter-primitive-in) attribute on a subsequent filter primitive within the same [filter](#elementdef-filter) element. If no value is provided, the output will only be available for re-use as the implicit input into the next filter primitive if that filter primitive provides no value for its in attribute.

Most filter primitives take other filter primitives as input. The following attribute is representative for all input attributes to reference other filter primitives:

*Attribute definitions:*

***<a id="element-attrdef-filter-primitive-in"></a>`in`* = "*[SourceGraphic](#attr-valuedef-in-sourcegraphic) | [SourceAlpha](#attr-valuedef-in-sourcealpha) | [BackgroundImage](#attr-valuedef-in-backgroundimage) | [BackgroundAlpha](#attr-valuedef-in-backgroundalpha) | [FillPaint](#attr-valuedef-in-fillpaint) | [StrokePaint](#attr-valuedef-in-strokepaint) | [<filter-primitive-reference>](#typedef-result-filter-primitive-reference)*" **
:

Identifies input for the given filter primitive. The value can be either one of six keywords or can be a string which matches a previous [result](#element-attrdef-filter-primitive-result) attribute value within the same [filter](#elementdef-filter) element. If no value is provided and this is the first [filter primitive](#filter-primitive), then this filter primitive will use [SourceGraphic](#attr-valuedef-in-sourcegraphic) as its input. If no value is provided and this is a subsequent filter primitive, then this filter primitive will use the result from the previous filter primitive as its input.

If the value for [result](#element-attrdef-filter-primitive-result) appears multiple times within a given [filter](#elementdef-filter) element, then a reference to that result will use the closest preceding [filter primitive](#filter-primitive) with the given value for attribute result.

Forward references to results are not allowed, and will be treated as if no result was specified.

References to non-existent results will be treated as if no result was specified.

Definitions for the six keywords:

***<a id="attr-valuedef-in-sourcegraphic"></a>`SourceGraphic`* **
:

This keyword represents the graphics elements that were the original input into the [filter](#elementdef-filter) element. For raster effects [filter primitives](#filter-primitive), the graphics elements will be rasterized into an initially clear RGBA raster in image space. Pixels left untouched by the original graphic will be left clear. The image is specified to be rendered in linear RGBA pixels. The alpha channel of this image captures any anti-aliasing specified by SVG. (Since the raster is linear, the alpha channel of this image will represent the exact percent coverage of each pixel.)

***<a id="attr-valuedef-in-sourcealpha"></a>`SourceAlpha`* **
:

This keyword represents the graphics elements that were the original input into the [filter](#elementdef-filter) element. [SourceAlpha](#attr-valuedef-in-sourcealpha) has all of the same rules as [SourceGraphic](#attr-valuedef-in-sourcegraphic) except that only the alpha channel is used. The input image is an RGBA image consisting of implicitly black color values for the RGB channels, but whose alpha channel is the same as SourceGraphic.

Note: If this option is used, then some implementations might need to rasterize the graphics elements in order to extract the alpha channel.

***<a id="attr-valuedef-in-backgroundimage"></a>`BackgroundImage`* **
:

This keyword represents the back drop defined by the current isolation group behind the [filter region](#filter-region) at the time that the [filter](#elementdef-filter) element was invoked. See isolation property [[COMPOSITING-1]](#biblio-compositing-1).

***<a id="attr-valuedef-in-backgroundalpha"></a>`BackgroundAlpha`* **
:

Same as [BackgroundImage](#attr-valuedef-in-backgroundimage) except only the alpha channel is used. See [SourceAlpha](#attr-valuedef-in-sourcealpha) and the isolation property [[COMPOSITING-1]](#biblio-compositing-1).

***<a id="attr-valuedef-in-fillpaint"></a>`FillPaint`* **
:

This keyword represents the value of the [fill](svg2/painting.md#FillProperty) property on the target element for the filter effect. The [FillPaint](#attr-valuedef-in-fillpaint) image has conceptually infinite extent. Frequently this image is opaque everywhere, but it might not be if the "paint" itself has alpha, as in the case of a gradient or pattern which itself includes transparent or semi-transparent parts. If fill references a paint server, then the coordinate space of the paint server is the coordinate space defined for the filtered object. E.g if the paint server requires to use the [objectBoundingBox](#attr-valuedef-primitiveunits-objectboundingbox) of the object, the object bounding box of the filtered object defines the reference size of the paint server. If the paint server requires to use the [userSpaceOnUse](#attr-valuedef-primitiveunits-userspaceonuse), the nearest viewport in the [local coordinate system](css-transforms-1.md#local-coordinate-system) of the filtered object defines the reference size of the paint server.

***<a id="attr-valuedef-in-strokepaint"></a>`StrokePaint`* **
:

This keyword represents the value of the [stroke](svg2/painting.md#StrokeProperty) property on the target element for the filter effect. The [StrokePaint](#attr-valuedef-in-strokepaint) image has conceptually infinite extent. See [FillPaint](#attr-valuedef-in-fillpaint) above for more details.

Animatable: yes.
### <a id="FilterPrimitiveTree"></a>9.3. Filter primitive tree

Filter primitives with no or one filter primitive input can be linked together to a filter chain. E.g. the filter primitive representation of a [<filter-value-list>](#typedef-filter-value-list) with two or more [<filter-function>](#typedef-filter-function)s is an example of a filter chain. Every filter primitive takes the result of the previous filter primitive as input.

<a id="example-fd38116b"></a> A simple example of a [filter](#elementdef-filter) element with its filter primitive children.

```
<filter id="filter">
  <feColorMatrix type="hueRotate" values="45"/>
  <feOffset dx="10" dy="10"/>
  <feGaussianBlur stdDeviation="3"/>
</filter>
```

[feColorMatrix](#elementdef-fecolormatrix), [feOffset](#elementdef-feoffset) and [feGaussianBlur](#elementdef-fegaussianblur) create a filter chain.

[feColorMatrix](#elementdef-fecolormatrix) takes SourceGraphic as input. The result is the input of [feOffset](#elementdef-feoffset) with its result being the input of [feGaussianBlur](#elementdef-fegaussianblur).

Some filter primitives may have more than one filter primitive inputs. With the use of the [in](#element-attrdef-filter-primitive-in) and [result](#element-attrdef-filter-primitive-result) attributes it is possible to combine multiple filter primitives to a complex filter structure. Due to the non-forward reference restriction of filter primitives, every filter structure can be represented as a tree, the *<a id="filter-primitive-tree"></a>filter primitive tree*. The root filter primitive of the filter primitive tree is the most subsequential primitive of [filter](#elementdef-filter) elements filter primitive children.

A filter chain is one possible filter structure that can also be represented in a filter primitive tree. Therefore, filter chains are referred to as filter primitive trees onwards as well.

A [filter](#elementdef-filter) element may have one or more filter primitive trees. The filter primitive tree whose subsequent filter primitive is the last filter primitive child of the filter elements is the *<a id="primary-filter-primitive-tree"></a>primary filter primitive tree*.

Only the primary filter primitive tree contributes to the filter process. Implementations may chose to ignore all other possible filter primitive trees.

If a [filter](#elementdef-filter) element has no filter primitive tree then the element the filter applies to does not get rendered.

<a id="example-38d53387"></a> An example of multiple filter primitive trees:

```
<filter id="filter">
  <!-- The first filter primitive tree. Ignored for filter process. -->
  <feColorMatrix type="hueRotate" values="45"/>
  <feOffset dx="10" dy="10"/>
  <feGaussianBlur stdDeviation="3"/>
  <!-- The primary filter primitive tree. -->
  <feFlood flood-color="green" result="flood"/>
  <feComposite operator="in" in="SourceAlpha" in2="flood"/>
</filter>
```

The above filter has 2 filter primitive trees with the filter primitives:

1.
[feColorMatrix](#elementdef-fecolormatrix), [feOffset](#elementdef-feoffset) and [feGaussianBlur](#elementdef-fegaussianblur) (with feGaussianBlur being the root filter primitive of the tree) as well as
2.
[feFlood](#elementdef-feflood) and [feComposite](#elementdef-fecomposite) (with feComposite as the root filter primitive of the tree).

Both filter primitive trees are not connected. Only the 2nd, the primary filter primitive tree contributes to the filter process. The first tree can get ignored by implementations.

### <a id="FilterPrimitiveSubRegion"></a>9.4. Filter primitive subregion

All [filter primitives](#filter-primitive) have attributes [x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width) and [height](#element-attrdef-filter-primitive-height) which together identify a *<a id="filter-primitive-subregion"></a>filter primitive subregion* which restricts calculation and rendering of the given filter primitive. The x, y, width and height attributes are defined according to the same rules as other filter primitives coordinate and length attributes and thus represent values in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width) and [height](#element-attrdef-filter-primitive-height) default to the union (i.e., tightest fitting bounding box) of the subregions defined for all referenced nodes. If there are no referenced nodes (e.g., for [feImage](#elementdef-feimage) or [feTurbulence](#elementdef-feturbulence)), or one or more of the referenced nodes is a standard input (one of [SourceGraphic](#attr-valuedef-in-sourcegraphic), [SourceAlpha](#attr-valuedef-in-sourcealpha), [BackgroundImage](#attr-valuedef-in-backgroundimage), [BackgroundAlpha](#attr-valuedef-in-backgroundalpha), [FillPaint](#attr-valuedef-in-fillpaint) or [StrokePaint](#attr-valuedef-in-strokepaint)), or for [feTile](#elementdef-fetile) (which is special because its principal function is to replicate the referenced node in X and Y and thereby produce a usually larger result), the default subregion is 0%, 0%, 100%, 100%, where as a special-case the percentages are relative to the dimensions of the [filter region](#filter-region), thus making the default [filter primitive subregion](#filter-primitive-subregion) equal to the filter region.

If the [filter primitive subregion](#filter-primitive-subregion) has a negative or zero width or height, the effect of the filter primitive is disabled.

The [filter region](#filter-region) acts as a hard clip clipping rectangle on the filter primitive’s input image(s).

The [filter primitive subregion](#filter-primitive-subregion) acts as a hard clip clipping rectangle on the filter primitive result.

All intermediate offscreens are defined to not exceed the intersection of the [filter primitive subregion](#filter-primitive-subregion) with the [filter region](#filter-region). The filter region and any of the filter primitive subregions are to be set up such that all offscreens are made big enough to accommodate any pixels which even partly intersect with either the filter region or the filter primitive subregions.

<a id="example-ee9442c9"></a> [feTile](#elementdef-fetile) references a previous filter primitive and then stitches the tiles together based on the [filter primitive subregion](#filter-primitive-subregion) of the referenced filter primitive in order to fill its own filter primitive subregion.

```
<svg width="400" height="400" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <filter id="flood" x="0" y="0" width="100%" height="100%" primitiveUnits="objectBoundingBox">
       <feFlood x="25%" y="25%" width="50%" height="50%"
          flood-color="green" flood-opacity="0.75"/>
    </filter>
    <filter id="blend" primitiveUnits="objectBoundingBox">
       <feBlend x="25%" y="25%" width="50%" height="50%"
          in2="SourceGraphic" mode="multiply"/>
    </filter>
    <filter id="merge" primitiveUnits="objectBoundingBox">
       <feMerge x="25%" y="25%" width="50%" height="50%">
        <feMergeNode in="SourceGraphic"/>
        <feMergeNode in="FillPaint"/>
       </feMerge>
    </filter>
  </defs>

  <g fill="none" stroke="blue" stroke-width="4">
     <rect width="200" height="200"/>
     <line x2="200" y2="200"/>
     <line x1="200" y2="200"/>
  </g>
  <circle fill="green" filter="url(#flood)" cx="100" cy="100" r="90"/>

  <g transform="translate(200 0)">
    <g fill="none" stroke="blue" stroke-width="4">
       <rect width="200" height="200"/>
       <line x2="200" y2="200"/>
       <line x1="200" y2="200"/>
    </g>
    <circle fill="green" filter="url(#blend)" cx="100" cy="100" r="90"/>
  </g>

  <g transform="translate(0 200)">
    <g fill="none" stroke="blue" stroke-width="4">
       <rect width="200" height="200"/>
       <line x2="200" y2="200"/>
       <line x1="200" y2="200"/>
    </g>
    <circle fill="green" fill-opacity="0.5" filter="url(#merge)" cx="100" cy="100" r="90"/>
  </g>
</svg>
```

[image: Example for subregions]

Example for subregions

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/filtersubregion00.svg)

In the example above there are three rectangles that each have a cross and a circle in them. The circle element in each one has a different filter applied, but with the same [filter primitive subregion](#filter-primitive-subregion). The filter output should be limited to the filter primitive subregion so you should never see the circles themselves, just the rectangles that make up the filter primitive subregion.

-
The upper left rectangle shows an [feFlood](#elementdef-feflood) with [flood-opacity: 75%](#propdef-flood-opacity) so the cross should be visible through the green rect in the middle.
-
The lower left rectangle shows an [feMerge](#elementdef-femerge) that merges [SourceGraphic](#attr-valuedef-in-sourcegraphic) with [FillPaint](#attr-valuedef-in-fillpaint). Since the circle has `fill-opacity="0.5"` it will also be transparent so that the cross is visible through the green rect in the middle.
-
The upper right rectangle shows an [feBlend](#elementdef-feblend) that has `mode="multiply"`. Since the circle in this case isn’t transparent the result is totally opaque. The rect should be dark green and the cross should not be visible through it.

### <a id="feBlendElement"></a>9.5. Filter primitive [feBlend](#elementdef-feblend)

| Name: | *<a id="elementdef-feblend"></a>`feBlend`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [in2](#element-attrdef-feblend-in2) - [mode](#element-attrdef-feblend-mode) |
| DOM Interfaces: | [SVGFEBlendElement](#InterfaceSVGFEBlendElement) |

This filter blends two objects together using commonly used imaging software blending modes. It performs a pixel-wise combination of two input images. (See [[COMPOSITING-1]](#biblio-compositing-1).)

*Attribute definitions:*

***<a id="element-attrdef-feblend-mode"></a>`mode`* = "<blend-mode>" **
:

One of the blend modes defined by “Compositing and Blending Level 1” [[COMPOSITING-1]](#biblio-compositing-1) with the input [in](#element-attrdef-filter-primitive-in) representing the source `Cs` and the second input [in2](#element-attrdef-feblend-in2) representing the backdrop `Cb`. The output of this filter primitive `Cm` is the result of blending `Cs` with `Cb`.

The [initial value](svg2/types.md#TermInitialValue) for [mode](#element-attrdef-feblend-mode) is [normal](compositing-1.md#valdef-blend-mode-normal).

Animatable: yes.

***<a id="element-attrdef-feblend-no-composite"></a>`no-composite`* = "*<a id="attr-valuedef-no-composite-no-composite"></a>`no-composite`*" **
:

If the [no-composite](#element-attrdef-feblend-no-composite) attribute is present, the specified blend mode must not apply alpha compositing. See [Blending](compositing-1.md#blending) [[COMPOSITING-1]](#biblio-compositing-1) for the "mixing" formula without compositing. Otherwise, implementations must combine the blend mode specified by [mode](#element-attrdef-feblend-mode) with the [Source Over](compositing-1.md#porterduffcompositingoperators_srcover) composite operator. See [Blending](compositing-1.md#blending) [[COMPOSITING-1]](#biblio-compositing-1) for the "mixing" formula with compositing.

Note: This attribute is an addition to the [feBlend](#elementdef-feblend) element defintion in SVG 1.1. [no-composite](#element-attrdef-feblend-no-composite), when specified, is meant to avoid "double-compositing" effects when blending an input source with the backdrop of the filtered object (E.g. using the [BackgroundImage](#attr-valuedef-in-backgroundimage) filter primitive). For the majority of use cases authors will not need to specify the no-composite attribute.

Animatable: no.

***<a id="element-attrdef-feblend-in2"></a>`in2`* = "*(see [in](#element-attrdef-filter-primitive-in) attribute)*" **
:

The second input image to the blending operation.

Animatable: yes.

The [normal](compositing-1.md#valdef-blend-mode-normal) blend mode with alpha compositing is equivalent to `operator="over"` on the [feComposite](#elementdef-fecomposite) filter primitive, matches the blending method used by [feMerge](#elementdef-femerge) and matches the [simple alpha compositing](svg11/masking.md#SimpleAlphaBlending) technique used in SVG for all compositing outside of filter effects.

<a id="example-50c05d26"></a>

```
<svg width="5cm" height="5cm" viewBox="0 0 500 500"
     xmlns="http://www.w3.org/2000/svg">
  <title>Example feBlend - Examples of feBlend modes</title>
  <desc>Five text strings blended into a gradient,
        with one text string for each of the five feBlend modes.</desc>
  <defs>
    <linearGradient id="MyGradient" gradientUnits="userSpaceOnUse"
            x1="100" y1="0" x2="300" y2="0">
      <stop offset="0" stop-color="#000000" />
      <stop offset=".33" stop-color="#ffffff" />
      <stop offset=".67" stop-color="#ff0000" />
      <stop offset="1" stop-color="#808080" />
    </linearGradient>
    <filter id="Normal">
      <feBlend mode="normal" in2="BackgroundImage" in="SourceGraphic"/>
    </filter>
    <filter id="Multiply">
      <feBlend mode="multiply" in2="BackgroundImage" in="SourceGraphic"/>
    </filter>
    <filter id="Screen">
      <feBlend mode="screen" in2="BackgroundImage" in="SourceGraphic"/>
    </filter>
    <filter id="Darken">
      <feBlend mode="darken" in2="BackgroundImage" in="SourceGraphic"/>
    </filter>
    <filter id="Lighten">
      <feBlend mode="lighten" in2="BackgroundImage" in="SourceGraphic"/>
    </filter>
  </defs>
  <rect fill="none" stroke="blue"
        x="1" y="1" width="498" height="498"/>
  <g isolation="isolate" >
    <rect x="100" y="20" width="300" height="460" fill="url(#MyGradient)" />
    <g font-family="Verdana" font-size="75" fill="#888888" fill-opacity=".6" >
      <text x="50" y="90" filter="url(#Normal)" >Normal</text>
      <text x="50" y="180" filter="url(#Multiply)" >Multiply</text>
      <text x="50" y="270" filter="url(#Screen)" >Screen</text>
      <text x="50" y="360" filter="url(#Darken)" >Darken</text>
      <text x="50" y="450" filter="url(#Lighten)" >Lighten</text>
    </g>
  </g>
</svg>
```

[image: Example of feBlend]

Example of feBlend

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feBlend.svg)

### <a id="feColorMatrixElement"></a>9.6. Filter primitive [feColorMatrix](#elementdef-fecolormatrix)

| Name: | *<a id="elementdef-fecolormatrix"></a>`feColorMatrix`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [type](#element-attrdef-fecolormatrix-type) - [values](#element-attrdef-fecolormatrix-values) |
| DOM Interfaces: | [SVGFEColorMatrixElement](#InterfaceSVGFEColorMatrixElement) |

This filter applies a matrix transformation:

R ' G ' B ' A ' 1 = a 00 a 01 a 02 a 03 a 04 a 10 a 11 a 12 a 13 a 14 a 20 a 21 a 22 a 23 a 24 a 30 a 31 a 32 a 33 a 34 0 0 0 0 1 ⋅ R G B A 1 left [ stack {R' # G' # B' # A' # 1} right ] = left [ matrix { a_00 # a_01 # a_02 # a_03 # a_04 ## a_10 # a_11 # a_12 # a_13 # a_14 ## a_20 # a_21 # a_22 # a_23 # a_24 ## a_30 # a_31 # a_32 # a_33 # a_34 ## 0 # 0 # 0 # 0 # 1 } right ] cdot left[ stack { R # G # B # A # 1 } right]

on the RGBA color and alpha values of every pixel on the input graphics to produce a result with a new set of RGBA color and alpha values.

The calculations are performed on non-premultiplied color values.

*Attribute definitions:*

***<a id="element-attrdef-fecolormatrix-type"></a>`type`* = "*<a id="attr-valuedef-type-matrix"></a>`matrix`* | *<a id="attr-valuedef-type-saturate"></a>`saturate`* | *<a id="attr-valuedef-type-huerotate"></a>`hueRotate`* | *<a id="attr-valuedef-type-luminancetoalpha"></a>`luminanceToAlpha`*" **
:

Indicates the type of matrix operation. The keyword [matrix](#attr-valuedef-type-matrix) indicates that a full 5x4 matrix of values will be provided. The other keywords represent convenience shortcuts to allow commonly used color operations to be performed without specifying a complete matrix.

The [initial value](svg2/types.md#TermInitialValue) for [type](#element-attrdef-fecolormatrix-type) is [matrix](#attr-valuedef-type-matrix).

Animatable: yes.

***<a id="element-attrdef-fecolormatrix-values"></a>`values`* = "*list of [<number>](css-values-4.md#number-value)s*" **
:

The contents of [values](#element-attrdef-fecolormatrix-values) depends on the value of attribute [type](#element-attrdef-fecolormatrix-type):

-
For `type="matrix"`, [values](#element-attrdef-fecolormatrix-values) is a list of 20 matrix values (a00 a01 a02 a03 a04 a10 a11 ... a34), separated by whitespace and/or a comma. For example, the identity matrix could be expressed as:
```
type="matrix"
values="1 0 0 0 0  0 1 0 0 0  0 0 1 0 0  0 0 0 1 0"
```

-
For `type="saturate"`, [values](#element-attrdef-fecolormatrix-values) is a single real number value. A [saturate](#attr-valuedef-type-saturate) operation is equivalent to the following matrix operation:
R ' G ' B ' A ' 1 = 0.213 + 0.787s 0.715 − 0.715s 0.072 − 0.072s 0 0 0.213 − 0.213s 0.715 + 0.285s 0.072 − 0.072s 0 0 0.213 − 0.213s 0.715 − 0.715s 0.072 + 0.928s 0 0 0 0 0 1 0 0 0 0 0 1 ⋅ R G B A 1 left [ stack {R' # G' # B' # A' # 1} right ] = left [ matrix { 0.213 + 0.787s # 0.715 - 0.715s # 0.072 - 0.072s # 0 # 0 ## 0.213 - 0.213s # 0.715 + 0.285s # 0.072 - 0.072s # 0 # 0 ## 0.213 - 0.213s # 0.715 - 0.715s # 0.072 + 0.928s # 0 # 0 ## 0 # 0 # 0 # 1 # 0 ## 0 # 0 # 0 # 0 # 1 } right ] cdot left[ stack { R # G # B # A # 1 } right]
Note: A value of 0 produces a fully desaturated (grayscale) filter result, while a value of 1 passes the filter input image through unchanged. Values outside the 0..1 range under- or oversaturates the filter input image respectively.
Note: The precision of the luminance coefficients increased in comparison to previous specification texts [[Cmam]](#biblio-cmam).
-
For `type="hueRotate"`, [values](#element-attrdef-fecolormatrix-values) is a single one real number value (degrees). A [hueRotate](#attr-valuedef-type-huerotate) operation is equivalent to the following matrix operation:
R ' G ' B ' A ' 1 = a 00 a 01 a 02 0 0 a 10 a 11 a 12 0 0 a 20 a 21 a 22 0 0 0 0 0 1 0 0 0 0 0 1 ⋅ R G B A 1 left [ stack {R' # G' # B' # A' # 1} right ] = left [ matrix { a_00 # a_01 # a_02 # 0 # 0 ## a_10 # a_11 # a_12 # 0 # 0 ## a_20 # a_21 # a_22 # 0 # 0 ## 0 # 0 # 0 # 1 # 0 ## 0 # 0 # 0 # 0 # 1 } right ] cdot left[ stack { R # G # B # A # 1 } right]
where the terms a00, a01, etc. are calculated as follows:
a 00 a 01 a 02 a 10 a 11 a 12 a 20 a 21 a 22 = + 0.213 + 0.715 + 0.072 + 0.213 + 0.715 + 0.072 + 0.213 + 0.715 + 0.072 + cos ( hueRotate value ) ⋅ + 0.787 − 0.715 − 0.072 − 0.213 + 0.285 − 0.072 − 0.213 − 0.715 + 0.928 + sin ( hueRotate value ) ⋅ − 0.213 − 0.715 + 0.928 + 0.143 + 0.140 − 0.283 − 0.787 + 0.715 + 0.072 left[ matrix { a_00 # a_01 # a_02 ## a_10 # a_11 # a_12 ## a_20 # a_21 # a_22 } right] = left[ matrix { +0.213 # +0.715 # +0.072 ## +0.213 # +0.715 # +0.072 ## +0.213 # +0.715 # +0.072 } right] + cos(hueRotate value)cdot left[ matrix { +0.787 # -0.715 # -0.072 ## -0.213 # +0.285 # -0.072 ## -0.213 # -0.715 # +0.928 } right] + sin(hueRotate value) cdot left[ matrix { -0.213 # -0.715 # +0.928 ## +0.143 # +0.140 # -0.283 ## -0.787 # +0.715 # +0.072 } right] Thus, the upper left term of the hue matrix turns out to be: a 00 = 0.2127 + cos ( hueRotate value ) ⋅ 0.7873 − sin ( hueRotate value ) ⋅ 0.2127 a_00 = 0.2127 + cos(hueRotate value) cdot 0.7873 - sin(hueRotate value) cdot 0.2127
-
For `type="luminanceToAlpha"`, [values](#element-attrdef-fecolormatrix-values) is not applicable. A [luminanceToAlpha](#attr-valuedef-type-luminancetoalpha) operation is equivalent to the following matrix operation:
R ' G ' B ' A ' 1 = 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0.2126 0.7152 0.0722 0 0 0 0 0 0 1 ⋅ R G B A 1 left [ stack {R' # G' # B' # A' # 1} right ] = left [ matrix { 0 # 0 # 0 # 0 # 0 ## 0 # 0 # 0 # 0 # 0 ## 0 # 0 # 0 # 0 # 0 ## 0.2126 # 0.7152 # 0.0722 # 0 # 0 ## 0 # 0 # 0 # 0 # 1 } right ] cdot left[ stack { R # G # B # A # 1 } right]

The [initial value](svg2/types.md#TermInitialValue) for [values](#element-attrdef-fecolormatrix-values)

**if `type="matrix"` **
:

defaults to the identity matrix

**if `type="saturate"` **
:

defaults to the value 1

**if `type="hueRotate"` **
:

defaults to the value 0 which results in the identity matrix.

If the number of entries in the [values](#element-attrdef-fecolormatrix-values) list does not match the required number of entries by the [type](#element-attrdef-fecolormatrix-type), the filter primitive acts as a [pass through filter](#pass-through-filter).

Animatable: yes.

<a id="example-05772ccd"></a>

```
<svg width="8cm" height="5cm" viewBox="0 0 800 500"
     xmlns="http://www.w3.org/2000/svg">
  <title>Example feColorMatrix - Examples of feColorMatrix operations</title>
  <desc>Five text strings showing the effects of feColorMatrix:
        an unfiltered text string acting as a reference,
        use of the feColorMatrix matrix option to convert to grayscale,
        use of the feColorMatrix saturate option,
        use of the feColorMatrix hueRotate option,
        and use of the feColorMatrix luminanceToAlpha option.</desc>
  <defs>
    <linearGradient id="MyGradient" gradientUnits="userSpaceOnUse"
            x1="100" y1="0" x2="500" y2="0">
      <stop offset="0" stop-color="#ff00ff" />
      <stop offset=".33" stop-color="#88ff88" />
      <stop offset=".67" stop-color="#2020ff" />
      <stop offset="1" stop-color="#d00000" />
    </linearGradient>
    <filter id="Matrix" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feColorMatrix type="matrix" in="SourceGraphic"
           values=".33 .33 .33 0 0
                   .33 .33 .33 0 0
                   .33 .33 .33 0 0
                   .33 .33 .33 0 0"/>
    </filter>
    <filter id="Saturate40" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feColorMatrix type="saturate" in="SourceGraphic" values="0.4"/>
    </filter>
    <filter id="HueRotate90" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feColorMatrix type="hueRotate" in="SourceGraphic" values="90"/>
    </filter>
    <filter id="LuminanceToAlpha" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feColorMatrix type="luminanceToAlpha" in="SourceGraphic" result="a"/>
      <feComposite in="SourceGraphic" in2="a" operator="in" />
    </filter>
  </defs>
  <rect fill="none" stroke="blue"
        x="1" y="1" width="798" height="498"/>
  <g font-family="Verdana" font-size="75"
            font-weight="bold" fill="url(#MyGradient)" >
    <rect x="100" y="0" width="500" height="20" />
    <text x="100" y="90">Unfiltered</text>
    <text x="100" y="190" filter="url(#Matrix)" >Matrix</text>
    <text x="100" y="290" filter="url(#Saturate40)" >Saturate</text>
    <text x="100" y="390" filter="url(#HueRotate90)" >HueRotate</text>
    <text x="100" y="490" filter="url(#LuminanceToAlpha)" >Luminance</text>
  </g>
</svg>
```

[image: Example]

Example of feColorMatrix

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feColorMatrix.svg)

### <a id="feComponentTransferElement"></a>9.7. Filter primitive [feComponentTransfer](#elementdef-fecomponenttransfer)

| Name: | *<a id="elementdef-fecomponenttransfer"></a>`feComponentTransfer`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), [feFuncR](#elementdef-fefuncr), [feFuncG](#elementdef-fefuncg), [feFuncB](#elementdef-fefuncb), [feFuncA](#elementdef-fefunca), [script](svg2/interact.md#elementdef-script) elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) |
| DOM Interfaces: | [SVGFEComponentTransferElement](#InterfaceSVGFEComponentTransferElement) |

This filter primitive performs component-wise remapping of data as follows:

```
R' = feFuncR( R )
G' = feFuncG( G )
B' = feFuncB( B )
A' = feFuncA( A )
```

for every pixel. It allows operations like brightness adjustment, contrast adjustment, color balance or thresholding.

The calculations are performed on non-premultiplied color values.

The child elements of a [feComponentTransfer](#elementdef-fecomponenttransfer) element specify the transfer functions for the four channels:

-
[feFuncR](#elementdef-fefuncr) - transfer function for the red component of the input graphic
-
[feFuncG](#elementdef-fefuncg) - transfer function for the green component of the input graphic
-
[feFuncB](#elementdef-fefuncb) - transfer function for the blue component of the input graphic
-
[feFuncA](#elementdef-fefunca) - transfer function for the alpha component of the input graphic

The set of [feFuncR](#elementdef-fefuncr), [feFuncG](#elementdef-fefuncg), [feFuncB](#elementdef-fefuncb), [feFuncA](#elementdef-fefunca) elements are also called *<a id="transfer-function-element"></a>transfer function element*s.

The following rules apply to the processing of the [feComponentTransfer](#elementdef-fecomponenttransfer) element:

-
If more than one [transfer function element](#transfer-function-element) of the same kind is specified, the last occurrence is to be used.
-
If any of the [transfer function elements](#transfer-function-element) are unspecified, the [feComponentTransfer](#elementdef-fecomponenttransfer) must be processed as if those transfer function elements were specified with their [type](#element-attrdef-fecomponenttransfer-type) attributes set to identity.

#### <a id="feFuncRElement"></a>9.7.1. Transfer function [feFuncR](#elementdef-fefuncr)

| Name: | *<a id="elementdef-fefuncr"></a>`feFuncR`* |
| --- | --- |
| Categories: | [transfer function element](#transfer-function-element) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [transfer function element attributes](#transfer-function-element-attributes) — [type](#element-attrdef-fecomponenttransfer-type), [tableValues](#element-attrdef-fecomponenttransfer-tablevalues), [slope](#element-attrdef-fecomponenttransfer-slope), [intercept](#element-attrdef-fecomponenttransfer-intercept), [amplitude](#element-attrdef-fecomponenttransfer-amplitude), [exponent](#element-attrdef-fecomponenttransfer-exponent), [offset](#element-attrdef-fecomponenttransfer-offset) |
| DOM Interfaces: | [SVGFEFuncRElement](#InterfaceSVGFEFuncRElement) |

The attributes below are the *<a id="transfer-function-element-attributes"></a>transfer function element attributes*, which apply to the [transfer function elements](#transfer-function-element).

*Attribute definitions:*

***<a id="element-attrdef-fecomponenttransfer-type"></a>`type`* = "[identity](#attr-valuedef-type-identity) | [table](#attr-valuedef-type-table) | [discrete](#attr-valuedef-type-discrete) | [linear](#attr-valuedef-type-linear) | [gamma](#attr-valuedef-type-gamma)" **
:

Indicates the type of component transfer function. The type of function determines the applicability of the other attributes.

In the following, C is the initial component (e.g., [feFuncR](#elementdef-fefuncr)), C' is the remapped component; both in the closed interval [0,1].

-
For *<a id="attr-valuedef-type-identity"></a>`identity`*:
```
C' = C
```

-
For *<a id="attr-valuedef-type-table"></a>`table`*, the function is defined by linear interpolation between values given in the attribute [tableValues](#element-attrdef-fecomponenttransfer-tablevalues). The table has *n+1* values (i.e., v_0 to v_n) specifying the start and end values for *n* evenly sized interpolation regions. Interpolations use the following formula:
For a value `C < 1` find `k` such that:
`k/n <= C < (k+1)/n`
The result `C'` is given by:
`C' = v_k + (C - k/n)*n * (v_k+1 - v_k)`
If `C = 1` then:
`C' = v_n.`
-
For *<a id="attr-valuedef-type-discrete"></a>`discrete`*, the function is defined by the step function given in the attribute [tableValues](#element-attrdef-fecomponenttransfer-tablevalues), which provides a list of *n* values (i.e., v_0 to v_n-1) in order to identify a step function consisting of *n* steps. The step function is defined by the following formula:
For a value `C < 1` find `k` such that:
`k/n <= C < (k+1)/n`
The result `C'` is given by:
`C' = v_k`
If `C = 1` then:
`C' = v_n-1.`
-
For *<a id="attr-valuedef-type-linear"></a>`linear`*, the function is defined by the following linear equation:
`C' = [slope](#element-attrdef-fecomponenttransfer-slope) * C + [intercept](#element-attrdef-fecomponenttransfer-intercept)`
-
For *<a id="attr-valuedef-type-gamma"></a>`gamma`*, the function is defined by the following exponential function:
`C' = [amplitude](#element-attrdef-fecomponenttransfer-amplitude) * pow(C, [exponent](#element-attrdef-fecomponenttransfer-exponent)) + [offset](#element-attrdef-fecomponenttransfer-offset)`

The [initial value](svg2/types.md#TermInitialValue) for [type](#element-attrdef-fecomponenttransfer-type) is [identity](#attr-valuedef-type-identity).

Animatable: yes.

***<a id="element-attrdef-fecomponenttransfer-tablevalues"></a>`tableValues`* = "*(list of [<number>](css-values-4.md#number-value)s)*" **
:

When `type="table"`, the list of [<number>](css-values-4.md#number-value) s *v0,v1,...vn*, separated by white space and/or a comma, which define the lookup table. An empty list results in an identity transfer function.

If the attribute is not specified, then the effect is as if an empty list were provided.

Animatable: yes.

***<a id="element-attrdef-fecomponenttransfer-slope"></a>`slope`* = "*[<number>](css-values-4.md#number-value)*" **
:

When `type="linear"`, the slope of the linear function.

The [initial value](svg2/types.md#TermInitialValue) for [slope](#element-attrdef-fecomponenttransfer-slope) is 1.

Animatable: yes.

***<a id="element-attrdef-fecomponenttransfer-intercept"></a>`intercept`* = "*[<number>](css-values-4.md#number-value)*" **
:

When `type="linear"`, the intercept of the linear function.

The [initial value](svg2/types.md#TermInitialValue) for [intercept](#element-attrdef-fecomponenttransfer-intercept) is 0.

Animatable: yes.

***<a id="element-attrdef-fecomponenttransfer-amplitude"></a>`amplitude`* = "*[<number>](css-values-4.md#number-value)*" **
:

When `type="gamma"`, the amplitude of the gamma function.

The [initial value](svg2/types.md#TermInitialValue) for [amplitude](#element-attrdef-fecomponenttransfer-amplitude) is 1.

Animatable: yes.

***<a id="element-attrdef-fecomponenttransfer-exponent"></a>`exponent`* = "*[<number>](css-values-4.md#number-value)*" **
:

When `type="gamma"`, the exponent of the gamma function.

The [initial value](svg2/types.md#TermInitialValue) for [exponent](#element-attrdef-fecomponenttransfer-exponent) is 1.

Animatable: yes.

***<a id="element-attrdef-fecomponenttransfer-offset"></a>`offset`* = "*[<number>](css-values-4.md#number-value)*" **
:

When `type="gamma"`, the offset of the gamma function.

The [initial value](svg2/types.md#TermInitialValue) for [offset](#element-attrdef-fecomponenttransfer-offset) is 0.

Animatable: yes.

<a id="example-49a7ba07"></a>

```
<svg width="8cm" height="4cm" viewBox="0 0 800 400"
     xmlns="http://www.w3.org/2000/svg">
  <title>Example feComponentTransfer - Examples of feComponentTransfer operations</title>
  <desc>Four text strings showing the effects of feComponentTransfer:
        an identity function acting as a reference,
        use of the feComponentTransfer table option,
        use of the feComponentTransfer linear option,
        and use of the feComponentTransfer gamma option.</desc>
  <defs>
    <linearGradient id="MyGradient" gradientUnits="userSpaceOnUse"
            x1="100" y1="0" x2="600" y2="0">
      <stop offset="0" stop-color="#ff0000" />
      <stop offset=".33" stop-color="#00ff00" />
      <stop offset=".67" stop-color="#0000ff" />
      <stop offset="1" stop-color="#000000" />
    </linearGradient>
    <filter id="Identity" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feComponentTransfer>
        <feFuncR type="identity"/>
        <feFuncG type="identity"/>
        <feFuncB type="identity"/>
        <feFuncA type="identity"/>
      </feComponentTransfer>
    </filter>
    <filter id="Table" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feComponentTransfer>
        <feFuncR type="table" tableValues="0 0 1 1"/>
        <feFuncG type="table" tableValues="1 1 0 0"/>
        <feFuncB type="table" tableValues="0 1 1 0"/>
      </feComponentTransfer>
    </filter>
    <filter id="Linear" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feComponentTransfer>
        <feFuncR type="linear" slope=".5" intercept=".25"/>
        <feFuncG type="linear" slope=".5" intercept="0"/>
        <feFuncB type="linear" slope=".5" intercept=".5"/>
      </feComponentTransfer>
    </filter>
    <filter id="Gamma" filterUnits="objectBoundingBox"
            x="0%" y="0%" width="100%" height="100%">
      <feComponentTransfer>
        <feFuncR type="gamma" amplitude="2" exponent="5" offset="0"/>
        <feFuncG type="gamma" amplitude="2" exponent="3" offset="0"/>
        <feFuncB type="gamma" amplitude="2" exponent="1" offset="0"/>
      </feComponentTransfer>
    </filter>
  </defs>
  <rect fill="none" stroke="blue"
        x="1" y="1" width="798" height="398"/>
  <g font-family="Verdana" font-size="75"
            font-weight="bold" fill="url(#MyGradient)" >
    <rect x="100" y="0" width="600" height="20" />
    <text x="100" y="90">Identity</text>
    <text x="100" y="190" filter="url(#Table)" >TableLookup</text>
    <text x="100" y="290" filter="url(#Linear)" >LinearFunc</text>
    <text x="100" y="390" filter="url(#Gamma)" >GammaFunc</text>
  </g>
</svg>
```

[image: Example for feComponentTransfer]

Example for feComponentTransfer

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feComponentTransfer.svg)

#### <a id="feFuncGElement"></a>9.7.2. Transfer function [feFuncG](#elementdef-fefuncg)

| Name: | *<a id="elementdef-fefuncg"></a>`feFuncG`* |
| --- | --- |
| Categories: | [transfer function element](#transfer-function-element) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [transfer function element attributes](#transfer-function-element-attributes) — [type](#element-attrdef-fecomponenttransfer-type), [tableValues](#element-attrdef-fecomponenttransfer-tablevalues), [slope](#element-attrdef-fecomponenttransfer-slope), [intercept](#element-attrdef-fecomponenttransfer-intercept), [amplitude](#element-attrdef-fecomponenttransfer-amplitude), [exponent](#element-attrdef-fecomponenttransfer-exponent), [offset](#element-attrdef-fecomponenttransfer-offset) |
| DOM Interfaces: | [SVGFEFuncGElement](#InterfaceSVGFEFuncGElement) |

See [feFuncR](#elementdef-fefuncr) for the definitions of the attribute values.

#### <a id="feFuncBElement"></a>9.7.3. Transfer function [feFuncB](#elementdef-fefuncb)

| Name: | *<a id="elementdef-fefuncb"></a>`feFuncB`* |
| --- | --- |
| Categories: | [transfer function element](#transfer-function-element) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [transfer function element attributes](#transfer-function-element-attributes) — [type](#element-attrdef-fecomponenttransfer-type), [tableValues](#element-attrdef-fecomponenttransfer-tablevalues), [slope](#element-attrdef-fecomponenttransfer-slope), [intercept](#element-attrdef-fecomponenttransfer-intercept), [amplitude](#element-attrdef-fecomponenttransfer-amplitude), [exponent](#element-attrdef-fecomponenttransfer-exponent), [offset](#element-attrdef-fecomponenttransfer-offset) |
| DOM Interfaces: | [SVGFEFuncBElement](#InterfaceSVGFEFuncBElement) |

See [feFuncR](#elementdef-fefuncr) for the definitions of the attribute values.

#### <a id="feFuncAElement"></a>9.7.4. Transfer function [feFuncA](#elementdef-fefunca)

| Name: | *<a id="elementdef-fefunca"></a>`feFuncA`* |
| --- | --- |
| Categories: | [transfer function element](#transfer-function-element) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [transfer function element attributes](#transfer-function-element-attributes) — [type](#element-attrdef-fecomponenttransfer-type), [tableValues](#element-attrdef-fecomponenttransfer-tablevalues), [slope](#element-attrdef-fecomponenttransfer-slope), [intercept](#element-attrdef-fecomponenttransfer-intercept), [amplitude](#element-attrdef-fecomponenttransfer-amplitude), [exponent](#element-attrdef-fecomponenttransfer-exponent), [offset](#element-attrdef-fecomponenttransfer-offset) |
| DOM Interfaces: | [SVGFEFuncAElement](#InterfaceSVGFEFuncAElement) |

See [feFuncR](#elementdef-fefuncr) for the definitions of the attribute values.

### <a id="feCompositeElement"></a>9.8. Filter primitive [feComposite](#elementdef-fecomposite)

| Name: | *<a id="elementdef-fecomposite"></a>`feComposite`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [in2](#element-attrdef-fecomposite-in2) - [operator](#element-attrdef-fecomposite-operator) - [k1](#element-attrdef-fecomposite-k1) - [k2](#element-attrdef-fecomposite-k2) - [k3](#element-attrdef-fecomposite-k3) - [k4](#element-attrdef-fecomposite-k4) |
| DOM Interfaces: | [SVGFECompositeElement](#InterfaceSVGFECompositeElement) |

This filter performs the combination of the two input images pixel-wise in image space using one of the Porter-Duff [[PORTERDUFF]](#biblio-porterduff) compositing operations: [over](#attr-valuedef-fecomposite-operator-over), in, atop, out, xor, lighter [[COMPOSITING-1]](#biblio-compositing-1). Additionally, a component-wise *arithmetic* operation (with the result clamped between [0..1]) can be applied.

The *arithmetic* operation is useful for combining the output from the [feDiffuseLighting](#elementdef-fediffuselighting) and [feSpecularLighting](#elementdef-fespecularlighting) filters with texture data. It is also useful for implementing *dissolve*. If the *arithmetic* operation is chosen, each result pixel is computed using the following formula:

```
result = k1*i1*i2 + k2*i1 + k3*i2 + k4
```

where:

-
`i1` and `i2` indicate the corresponding pixel channel values of the input image, which map to [in](#element-attrdef-filter-primitive-in) and [in2](#element-attrdef-fecomposite-in2) respectively
-
`k1, k2, k3` and `k4` indicate the values of the attributes with the same name

For this filter primitive, the extent of the resulting image might grow as described in the section that describes the [filter primitive subregion](#filter-primitive-subregion).

*Attribute definitions:*

***<a id="element-attrdef-fecomposite-operator"></a>`operator`* = "*<a id="attr-valuedef-fecomposite-operator-over"></a>`over`* | *<a id="attr-valuedef-fecomposite-operator-in"></a>`in`* | *<a id="attr-valuedef-fecomposite-operator-out"></a>`out`* | *<a id="attr-valuedef-fecomposite-operator-atop"></a>`atop`* | *<a id="attr-valuedef-fecomposite-operator-xor"></a>`xor`* | *<a id="attr-valuedef-fecomposite-operator-lighter"></a>`lighter`* | *<a id="attr-valuedef-fecomposite-operator-arithmetic"></a>`arithmetic`*" **
:

The compositing operation that is to be performed. All of the [operator](#element-attrdef-fecomposite-operator) types except arithmetic match the corresponding operation as described in [[COMPOSITING-1]](#biblio-compositing-1) with [in](#element-attrdef-filter-primitive-in) representing the source and [in2](#element-attrdef-fecomposite-in2) representing the destination. The arithmetic operator is described above.

The [initial value](svg2/types.md#TermInitialValue) for [operator](#element-attrdef-fecomposite-operator) is [over](#attr-valuedef-fecomposite-operator-over).

Animatable: yes.

***<a id="element-attrdef-fecomposite-k1"></a>`k1`* = "[<number>](css-values-4.md#number-value)" **
:

Only applicable if `operator="arithmetic"`.

The [initial value](svg2/types.md#TermInitialValue) for [k1](#element-attrdef-fecomposite-k1) is 0.

Animatable: yes.

***<a id="element-attrdef-fecomposite-k2"></a>`k2`* = "*[<number>](css-values-4.md#number-value)*" **
:

Only applicable if `operator="arithmetic"`.

The [initial value](svg2/types.md#TermInitialValue) for [k2](#element-attrdef-fecomposite-k2) is 0.

Animatable: yes.

***<a id="element-attrdef-fecomposite-k3"></a>`k3`* = "[<number>](css-values-4.md#number-value)" **
:

Only applicable if `operator="arithmetic"`.

The [initial value](svg2/types.md#TermInitialValue) for [k3](#element-attrdef-fecomposite-k3) is 0.

Animatable: yes.

***<a id="element-attrdef-fecomposite-k4"></a>`k4`* = "[<number>](css-values-4.md#number-value)" **
:

Only applicable if `operator="arithmetic"`.

The [initial value](svg2/types.md#TermInitialValue) for [k4](#element-attrdef-fecomposite-k4) is 0.

Animatable: yes.

***<a id="element-attrdef-fecomposite-in2"></a>`in2`* = "*(see [in](#element-attrdef-filter-primitive-in) attribute)*" **
:

The second input image to the compositing operation.

Animatable: yes.

Note: Compositing and Blending [[COMPOSITING-1]](#biblio-compositing-1) defines more compositing keywords. The functionality of the additional keywords can be archived by switching the input filter primitives [in](#element-attrdef-filter-primitive-in) and [in2](#element-attrdef-fecomposite-in2).

<a id="example-5847e5f6"></a>

```
<svg width="330" height="195" viewBox="0 0 1100 650"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <title>Example feComposite - Examples of feComposite operations</title>
  <desc>Four rows of six pairs of overlapping triangles depicting
        the six different feComposite operators under different
        opacity values and different clearing of the background.</desc>
  <defs>
    <desc>Define two sets of six filters for each of the six compositing operators.
          The first set wipes out the background image by flooding with opaque white.
          The second set does not wipe out the background, with the result
          that the background sometimes shines through and is other cases
          is blended into itself (i.e., "double-counting").</desc>
    <filter id="overFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feFlood flood-color="#ffffff" flood-opacity="1" result="flood"/>
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="over" result="comp"/>
      <feMerge> <feMergeNode in="flood"/> <feMergeNode in="comp"/> </feMerge>
    </filter>
    <filter id="inFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feFlood flood-color="#ffffff" flood-opacity="1" result="flood"/>
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="in" result="comp"/>
      <feMerge> <feMergeNode in="flood"/> <feMergeNode in="comp"/> </feMerge>
    </filter>
    <filter id="outFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feFlood flood-color="#ffffff" flood-opacity="1" result="flood"/>
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="out" result="comp"/>
      <feMerge> <feMergeNode in="flood"/> <feMergeNode in="comp"/> </feMerge>
    </filter>
    <filter id="atopFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feFlood flood-color="#ffffff" flood-opacity="1" result="flood"/>
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="atop" result="comp"/>
      <feMerge> <feMergeNode in="flood"/> <feMergeNode in="comp"/> </feMerge>
    </filter>
    <filter id="xorFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feFlood flood-color="#ffffff" flood-opacity="1" result="flood"/>
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="xor" result="comp"/>
      <feMerge> <feMergeNode in="flood"/> <feMergeNode in="comp"/> </feMerge>
    </filter>
    <filter id="arithmeticFlood" filterUnits="objectBoundingBox"
            x="-5%" y="-5%" width="110%" height="110%">
      <feFlood flood-color="#ffffff" flood-opacity="1" result="flood"/>
      <feComposite in="SourceGraphic" in2="BackgroundImage" result="comp"
                   operator="arithmetic" k1=".5" k2=".5" k3=".5" k4=".5"/>
      <feMerge> <feMergeNode in="flood"/> <feMergeNode in="comp"/> </feMerge>
    </filter>
    <filter id="overNoFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="over" result="comp"/>
    </filter>
    <filter id="inNoFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="in" result="comp"/>
    </filter>
    <filter id="outNoFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="out" result="comp"/>
    </filter>
    <filter id="atopNoFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="atop" result="comp"/>
    </filter>
    <filter id="xorNoFlood" filterUnits="objectBoundingBox" x="-5%" y="-5%" width="110%" height="110%">
      <feComposite in="SourceGraphic" in2="BackgroundImage" operator="xor" result="comp"/>
    </filter>
    <filter id="arithmeticNoFlood" filterUnits="objectBoundingBox"
            x="-5%" y="-5%" width="110%" height="110%">
      <feComposite in="SourceGraphic" in2="BackgroundImage" result="comp"
                   operator="arithmetic" k1=".5" k2=".5" k3=".5" k4=".5"/>
    </filter>
    <path id="Blue100" d="M 0 0 L 100 0 L 100 100 z" fill="#00ffff" />
    <path id="Red100" d="M 0 0 L 0 100 L 100 0 z" fill="#ff00ff" />
    <path id="Blue50" d="M 0 125 L 100 125 L 100 225 z" fill="#00ffff" fill-opacity=".5" />
    <path id="Red50" d="M 0 125 L 0 225 L 100 125 z" fill="#ff00ff" fill-opacity=".5" />
    <g id="TwoBlueTriangles">
      <use xlink:href="#Blue100"/>
      <use xlink:href="#Blue50"/>
    </g>
    <g id="BlueTriangles">
      <use transform="translate(275,25)" xlink:href="#TwoBlueTriangles"/>
      <use transform="translate(400,25)" xlink:href="#TwoBlueTriangles"/>
      <use transform="translate(525,25)" xlink:href="#TwoBlueTriangles"/>
      <use transform="translate(650,25)" xlink:href="#TwoBlueTriangles"/>
      <use transform="translate(775,25)" xlink:href="#TwoBlueTriangles"/>
      <use transform="translate(900,25)" xlink:href="#TwoBlueTriangles"/>
    </g>
  </defs>

  <rect fill="none" stroke="blue" x="1" y="1" width="1098" height="648"/>
  <g font-family="Verdana" font-size="40" shape-rendering="crispEdges">
    <desc>Render the examples using the filters that draw on top of
          an opaque white surface, thus obliterating the background.</desc>
    <g isolation="isolate">
      <text x="15" y="75">opacity 1.0</text>
      <text x="15" y="115" font-size="27">(with feFlood)</text>
      <text x="15" y="200">opacity 0.5</text>
      <text x="15" y="240" font-size="27">(with feFlood)</text>
      <use xlink:href="#BlueTriangles"/>
      <g transform="translate(275,25)">
        <use xlink:href="#Red100" filter="url(#overFlood)" />
        <use xlink:href="#Red50" filter="url(#overFlood)" />
        <text x="5" y="275">over</text>
      </g>
      <g transform="translate(400,25)">
        <use xlink:href="#Red100" filter="url(#inFlood)" />
        <use xlink:href="#Red50" filter="url(#inFlood)" />
        <text x="35" y="275">in</text>
      </g>
      <g transform="translate(525,25)">
        <use xlink:href="#Red100" filter="url(#outFlood)" />
        <use xlink:href="#Red50" filter="url(#outFlood)" />
        <text x="15" y="275">out</text>
      </g>
      <g transform="translate(650,25)">
        <use xlink:href="#Red100" filter="url(#atopFlood)" />
        <use xlink:href="#Red50" filter="url(#atopFlood)" />
        <text x="10" y="275">atop</text>
      </g>
      <g transform="translate(775,25)">
        <use xlink:href="#Red100" filter="url(#xorFlood)" />
        <use xlink:href="#Red50" filter="url(#xorFlood)" />
        <text x="15" y="275">xor</text>
      </g>
      <g transform="translate(900,25)">
        <use xlink:href="#Red100" filter="url(#arithmeticFlood)" />
        <use xlink:href="#Red50" filter="url(#arithmeticFlood)" />
        <text x="-25" y="275">arithmetic</text>
      </g>
    </g>
    <g transform="translate(0,325)" isolation="isolate">
    <desc>Render the examples using the filters that do not obliterate
          the background, thus sometimes causing the background to continue
          to appear in some cases, and in other cases the background
          image blends into itself ("double-counting").</desc>
      <text x="15" y="75">opacity 1.0</text>
      <text x="15" y="115" font-size="27">(without feFlood)</text>
      <text x="15" y="200">opacity 0.5</text>
      <text x="15" y="240" font-size="27">(without feFlood)</text>
      <use xlink:href="#BlueTriangles"/>
      <g transform="translate(275,25)">
        <use xlink:href="#Red100" filter="url(#overNoFlood)" />
        <use xlink:href="#Red50" filter="url(#overNoFlood)" />
        <text x="5" y="275">over</text>
      </g>
      <g transform="translate(400,25)">
        <use xlink:href="#Red100" filter="url(#inNoFlood)" />
        <use xlink:href="#Red50" filter="url(#inNoFlood)" />
        <text x="35" y="275">in</text>
      </g>
      <g transform="translate(525,25)">
        <use xlink:href="#Red100" filter="url(#outNoFlood)" />
        <use xlink:href="#Red50" filter="url(#outNoFlood)" />
        <text x="15" y="275">out</text>
      </g>
      <g transform="translate(650,25)">
        <use xlink:href="#Red100" filter="url(#atopNoFlood)" />
        <use xlink:href="#Red50" filter="url(#atopNoFlood)" />
        <text x="10" y="275">atop</text>
      </g>
      <g transform="translate(775,25)">
        <use xlink:href="#Red100" filter="url(#xorNoFlood)" />
        <use xlink:href="#Red50" filter="url(#xorNoFlood)" />
        <text x="15" y="275">xor</text>
      </g>
      <g transform="translate(900,25)">
        <use xlink:href="#Red100" filter="url(#arithmeticNoFlood)" />
        <use xlink:href="#Red50" filter="url(#arithmeticNoFlood)" />
        <text x="-25" y="275">arithmetic</text>
      </g>
    </g>
  </g>
</svg>
```

[image: Example of feComposite]

Example of feComposite

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feComposite.svg)

### <a id="feConvolveMatrixElement"></a>9.9. Filter primitive [feConvolveMatrix](#elementdef-feconvolvematrix)

| Name: | *<a id="elementdef-feconvolvematrix"></a>`feConvolveMatrix`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [order](#element-attrdef-order) - [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix) - [divisor](#element-attrdef-feconvolvematrix-divisor) - [bias](#element-attrdef-feconvolvematrix-bias) - [targetX](#element-attrdef-feconvolvematrix-targetx) - [targetY](#element-attrdef-feconvolvematrix-targety) - [edgeMode](#element-attrdef-feconvolvematrix-edgemode) - [kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength) - [preserveAlpha](#element-attrdef-feconvolvematrix-preservealpha) |
| DOM Interfaces: | [SVGFEConvolveMatrixElement](#InterfaceSVGFEConvolveMatrixElement) |

feConvolveMatrix applies a matrix convolution filter effect. A convolution combines pixels in the input image with neighboring pixels to produce a resulting image. A wide variety of imaging operations can be achieved through convolutions, including blurring, edge detection, sharpening, embossing and beveling.

A matrix convolution is based on an n-by-m matrix (the convolution kernel) which describes how a given pixel value in the input image is combined with its neighboring pixel values to produce a resulting pixel value. Each result pixel is determined by applying the kernel matrix to the corresponding source pixel and its neighboring pixels. The basic convolution formula which is applied to each color value for a given pixel is:

<a id="feConvolveMatrixElementFormula"></a> color X , Y = ∑ i = 0 orderY − 1 ∑ j = 0 orderX − 1 source x − targetX + j , y − targetY + i ⋅ kernelMatrix orderX − j − 1, orderY − i − 1 divisor + bias ⋅ alpha x , y func color_{X , Y} = { sum from{i=0} to{func orderY -1} sum from{j=0} to{func orderX -1} func source_{x - func targetX + j , y - targetY + i} cdot func kernelMatrix_{func orderX - j - 1, func orderY - i - 1 }} over {func divisor} + func bias cdot func alpha_{x,y}

where "orderX" and "orderY" represent the X and Y values for the [order](#element-attrdef-order) attribute, "targetX" represents the value of the [targetX](#element-attrdef-feconvolvematrix-targetx) attribute, "targetY" represents the value of the [targetY](#element-attrdef-feconvolvematrix-targety) attribute, "kernelMatrix" represents the value of the [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix) attribute, "divisor" represents the value of the [divisor](#element-attrdef-feconvolvematrix-divisor) attribute, and "bias" represents the value of the [bias](#element-attrdef-feconvolvematrix-bias) attribute.

In the above formulas the values in the kernel matrix are applied such that the kernel matrix is rotated 180 degrees relative to the source and destination images in order to match convolution theory as described in many computer graphics textbooks.

To illustrate, suppose you have a input image which is 5 pixels by 5 pixels, whose color values for one of the color channels are as follows:

0 20 40 235 235 100 120 140 235 235 200 220 240 235 235 255 255 255 255 255 255 255 255 255 255 left[ matrix {0 # 20 # 40 # 235 # 235 ## 100 # 120 # 140 # 235 # 235 ## 200 # 220 # 240 # 235 # 235 ## 255 # 255 # 255 # 255 # 255 ## 255 # 255 # 255 # 255 # 255} right]

and you define a 3-by-3 convolution kernel as follows:

1 2 3 4 5 6 7 8 9 left[ matrix {1 # 2 # 3 ## 4 # 5 # 6 ## 7 # 8 # 9} right]

Let’s focus on the color value at the second row and second column of the image (source pixel value is 120). Assuming the simplest case (where the input image’s pixel grid aligns perfectly with the kernel’s pixel grid) and assuming default values for attributes [divisor](#element-attrdef-feconvolvematrix-divisor), [targetX](#element-attrdef-feconvolvematrix-targetx) and [targetY](#element-attrdef-feconvolvematrix-targety), then resulting color value will be:

resultChannel 2,2 = 9 ⋅ 0 + 8 ⋅ 20 + 7 ⋅ 40 + 6 ⋅ 100 + 5 ⋅ 120 + 4 ⋅ 140 + 3 ⋅ 200 + 2 ⋅ 220 + 1 ⋅ 240 9 + 8 + 7 + 6 + 5 + 4 + 3 + 2 + 1 func resultChannel_2,2 = {{9 cdot 0} + {8 cdot 20} + {7 cdot 40} + {6 cdot 100} + {5 cdot 120} + {4 cdot 140} + {3 cdot 200} + {2 cdot 220} + {1 cdot 240}} over {9 + 8 + 7 + 6 + 5 + 4 + 3 + 2 + 1}

Because they operate on pixels, matrix convolutions are inherently resolution-dependent. To make [feConvolveMatrix](#elementdef-feconvolvematrix) produce resolution-independent results, an explicit value should be provided for the attribute [kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength).

[kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength), in combination with the other attributes, defines an implicit pixel grid in the filter effects coordinate system (i.e., the coordinate system established by the [primitiveUnits](#element-attrdef-filter-primitiveunits) attribute). The input image will be temporarily rescaled to match its pixels with kernelUnitLength. The convolution happens on the resampled image. After applying the convolution, the image is resampled back to the original resolution.

When the image must be resampled to match the coordinate system defined by [kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength) prior to convolution, or resampled to match the device coordinate system after convolution, it is recommended that high quality viewers make use of appropriate interpolation techniques, for example bilinear or bicubic. Depending on the speed of the available interpolents, this choice may be affected by the [image-rendering](css-images-3.md#propdef-image-rendering) property setting. Note that implementations might choose approaches that minimize or eliminate resampling when not necessary to produce proper results, such as when the document is zoomed out such that kernelUnitLength is considerably smaller than a device pixel.

*Attribute definitions:*

***<a id="element-attrdef-order"></a>`order`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

Indicates the number of cells in each dimension for [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix). The values provided must be [<integer>](css-values-4.md#integer-value) s greater than zero. Values that are not integers will be truncated, i.e. rounded to the closest integer value towards zero. The first number, <orderX>, indicates the number of columns in the matrix. The second number, <orderY>, indicates the number of rows in the matrix. If <orderY> is not provided, it defaults to <orderX>.

It is recommended that only small values (e.g., 3) be used; higher values may result in very high CPU overhead and usually do not produce results that justify the impact on performance.

The [initial value](svg2/types.md#TermInitialValue) for [order](#element-attrdef-order) is 3.

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-kernelmatrix"></a>`kernelMatrix`* = "*<list of numbers>*" **
:

The list of [<number>](css-values-4.md#number-value) s that make up the kernel matrix for the convolution. Values are separated by space characters and/or a comma. The number of entries in the list must equal <orderX> times <orderY>.

If the result of `orderX * orderY` is not equal to the the number of entries in the value list, the filter primitive acts as a [pass through filter](#pass-through-filter).

<a id="issue-4fd9d6f6"></a> How to behave on invalid number of entries in the value list? [Issue #237]

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-divisor"></a>`divisor`* = "*[<number>](css-values-4.md#number-value)*" **
:

After applying the [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix) to the input image to yield a number, that number is divided by [divisor](#element-attrdef-feconvolvematrix-divisor) to yield the final destination color value. A divisor that is the sum of all the matrix values tends to have an evening effect on the overall color intensity of the result. If the specified divisor is 0 then the default value will be used instead.

The [initial value](svg2/types.md#TermInitialValue) is the sum of all values in [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix), with the exception that if the sum is zero, then the divisor is set to 1.

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-bias"></a>`bias`* = "*[<number>](css-values-4.md#number-value)*" **
:

After applying the [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix) to the input image to yield a number and applying the [divisor](#element-attrdef-feconvolvematrix-divisor), the [bias](#element-attrdef-feconvolvematrix-bias) attribute is added to each component. One application of bias is when it is desirable to have .5 gray value be the zero response of the filter. The bias property shifts the range of the filter. This allows representation of values that would otherwise be clamped to 0 or 1.

The [initial value](svg2/types.md#TermInitialValue) for [bias](#element-attrdef-feconvolvematrix-bias) is 0.

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-targetx"></a>`targetX`* = "[<integer>](css-values-4.md#integer-value)" **
:

Determines the positioning in X of the convolution matrix relative to a given target pixel in the input image. The leftmost column of the matrix is column number zero. The value must be such that: 0 <= targetX < orderX. By default, the convolution matrix is centered in X over each pixel of the input image (i.e., targetX = floor ( orderX / 2 )).

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-targety"></a>`targetY`* = "[<integer>](css-values-4.md#integer-value)" **
:

Determines the positioning in Y of the convolution matrix relative to a given target pixel in the input image. The topmost row of the matrix is row number zero. The value must be such that: 0 <= targetY < orderY. By default, the convolution matrix is centered in Y over each pixel of the input image (i.e., targetY = floor ( orderY / 2 )).

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-edgemode"></a>`edgeMode`* = "[duplicate](#attr-valuedef-fegaussianblur-edgemode-duplicate) | [wrap](#attr-valuedef-fegaussianblur-edgemode-wrap) | [mirror](#attr-valuedef-feguassianblur-edgemode-mirror) | none" **
:

Determines how to extend the input image as necessary with color values so that the matrix operations can be applied when the kernel is positioned at or near the edge of the input image.

duplicate indicates that the input image is extended along each of its borders as necessary by duplicating the color values at the given edge of the input image.

Original N-by-M image, where m=M-1 and n=N-1:

11 12 . . . 1 m 1 M 21 22 . . . 2 m 2 M . . . . . . . . . . . . . . . n 1 n 2 . . . nm nM N 1 N 2 . . . Nm NM left[ matrix {11 # 12 # { "." "." "."} # {1 m} # {1 M} ## 21 # 22 # { "." "." "."} # {2 m} # {2 M} ## { "." "." "."} # { "." "." "."} # { "." "." "."} # { "." "." "."} # { "." "." "."} ## {n 1} # {n 2} # { "." "." "."} # func nm # func nM ## {N 1} # {N 2} # { "." "." "."} # func Nm # func NM} right]

Extended by two pixels using duplicate:

11 11 11 12 … 1 m 1 M 1 M 1 M 11 11 11 12 … 1 m 1 M 1 M 1 M 11 11 11 12 … 1 m 1 M 1 M 1 M 21 21 21 22 … 2 m 2 M 2 M 2 M … … … … … … … … … n 1 n 1 n 1 n 2 … nm nM nM nM N 1 N 1 N 1 N 2 … Nm NM NM NM N 1 N 1 N 1 N 2 … Nm NM NM NM N 1 N 1 N 1 N 2 … Nm NM NM NM left[ matrix {11 # 11 # 11 # 12 # dotslow # {1 m} # {1 M} # {1 M} # {1 M} ## 11 # 11 # 11 # 12 # dotslow # {1 m} # {1 M} # {1 M} # {1 M} ## 11 # 11 # 11 # 12 # dotslow # {1 m} # {1 M} # {1 M} # {1 M} ## 21 # 21 # 21 # 22 # dotslow # {2 m} # {2 M} # {2 M} # {2 M} ## dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow ## {n 1} # {n 1} # {n 1} # {n 2} # dotslow # func nm # func nM # func nM # func nM ## {N 1} # {N 1} # {N 1} # {N 2} # dotslow # func Nm # func NM # func NM # func NM ## {N 1} # {N 1} # {N 1} # {N 2} # dotslow # func Nm # func NM # func NM # func NM ## {N 1} # {N 1} # {N 1} # {N 2} # dotslow # func Nm # func NM # func NM # func NM} right]

[wrap](#attr-valuedef-fegaussianblur-edgemode-wrap) indicates that the input image is extended by taking the color values from the opposite edge of the image.

Extended by two pixels using [wrap](#attr-valuedef-fegaussianblur-edgemode-wrap):

nm nM n1 n2 … nm nM n1 n2 Nm NM N1 N2 … Nm NM N1 N2 1m 1M 11 12 … 1m 1M 11 12 2m 2M 21 22 … 2m 2M 21 22 … … … … … … … … … nm nM n1 n2 … nm nM n1 n2 Nm NM N1 N2 … Nm NM N1 N2 1m 1M 11 12 … 1m 1M 11 12 2m 2M 21 22 … 2m 2M 21 22 left[ matrix { nm # nM # n1 # n2 # dotslow # nm # nM # n1 # n2 ## Nm # NM # N1 # N2 # dotslow # Nm # NM # N1 # N2 ## 1m # 1M # 11 # 12 # dotslow # 1m # 1M # 11 # 12 ## 2m # 2M # 21 # 22 # dotslow # 2m # 2M # 21 # 22 ## dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow ## nm # nM # n1 # n2 # dotslow # nm # nM # n1 # n2 ## Nm # NM # N1 # N2 # dotslow # Nm # NM # N1 # N2 ## 1m # 1M # 11 # 12 # dotslow # 1m # 1M # 11 # 12 ## 2m # 2M # 21 # 22 # dotslow # 2m # 2M # 21 # 22 } right]

[mirror](#attr-valuedef-feguassianblur-edgemode-mirror) indicates that the input image is extended by taking color values mirrored across the edge being sampled beyond.

Extended by two pixels using [mirror](#attr-valuedef-feguassianblur-edgemode-mirror):

22 21 21 22 … 2m 2M 2M 2m 12 11 11 12 … 1m 1M 1M 1m 12 11 11 12 … 1m 1M 1M 1m 22 21 21 22 … 2m 2M 2M 2m … … … … … … … … … n2 n1 n1 n2 … nm nM nM nm N2 N1 N1 N2 … Nm NM NM Nm N2 N1 N1 N2 … Nm NM NM Nm n2 n1 n1 n2 … nm nM nM nm left[ matrix { 22 # 21 # 21 # 22 # dotslow # 2m # 2M # 2M # 2m ## 12 # 11 # 11 # 12 # dotslow # 1m # 1M # 1M # 1m ## 11 # 11 # 11 # 12 # dotslow # 1m # 1M # 1M # 1m ## 22 # 21 # 21 # 22 # dotslow # 2m # 2M # 2M # 2m ## dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow # dotslow ## n2 # n1 # n1 # n2 # dotslow # nm # nM # nM # nm ## N2 # N1 # N1 # N2 # dotslow # Nm # NM # NM # Nm ## N2 # N1 # N1 # N2 # dotslow # Nm # NM # NM # Nm ## n2 # n2 # n2 # n2 # dotslow # nm # nM # nM # nm } right]

The value none indicates that the input image is extended with pixel values of zero for R, G, B and A.

The [initial value](svg2/types.md#TermInitialValue) for [edgeMode](#element-attrdef-feconvolvematrix-edgemode) is duplicate.

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-kernelunitlength"></a>`kernelUnitLength`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The first number is the <dx> value. The second number is the <dy> value. If the <dy> value is not specified, it defaults to the same value as <dx>. Indicates the intended distance in current filter units (i.e., units as determined by the value of attribute [primitiveUnits](#element-attrdef-filter-primitiveunits)) between successive columns and rows, respectively, in the [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix). By specifying value(s) for [kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength), the kernel becomes defined in a scalable, abstract coordinate system. If kernelUnitLength is not specified, the default value is one pixel in the offscreen bitmap, which is a pixel-based coordinate system, and thus potentially not scalable. For some level of consistency across display media and user agents, it is necessary that a value be provided for kernelUnitLength. In some implementations, the most consistent results and the fastest performance will be achieved if the pixel grid of the temporary off-screen images aligns with the pixel grid of the kernel.

If a negative or zero value is specified the default value will be used instead.

Note: This attribute is deprecated and will be removed. It does not provide a reliable way to create platform independent results. Future versions of this specification will cover this use case.

Animatable: yes.

***<a id="element-attrdef-feconvolvematrix-preservealpha"></a>`preserveAlpha`* = "*false | true*" **
:

A value of false indicates that the convolution will apply to all channels, including the alpha channel. In this case the `ALPHA_X,Y` of the [convolution formula](#feConvolveMatrixElementFormula) for a given pixel is:

` ALPHA_X,Y = (                SUM _I=0 to [[orderY](#element-attrdef-order)-1] {                  SUM _J=0 to [[orderX](#element-attrdef-order)-1] {                    SOURCE _X-[targetX](#element-attrdef-feconvolvematrix-targetx)+J, Y-[targetY](#element-attrdef-feconvolvematrix-targety)+I *  [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix)_[orderX](#element-attrdef-order)-J-1,  [orderY](#element-attrdef-order)-I-1                  }                }              ) /  [divisor](#element-attrdef-feconvolvematrix-divisor) +  [bias](#element-attrdef-feconvolvematrix-bias)  `

A value of "true" indicates that the convolution will only apply to the color channels. In this case, the filter will temporarily unpremultiply the color component values and apply the kernel. In this case the `ALPHA_X,Y` of the [convolution formula](#feConvolveMatrixElementFormula) for a given pixel is:

`ALPHA_X,Y = SOURCE_X,Y`

The [initial value](svg2/types.md#TermInitialValue) for [preserveAlpha](#element-attrdef-feconvolvematrix-preservealpha) is false.

Animatable: yes.

### <a id="feDiffuseLightingElement"></a>9.10. Filter primitive [feDiffuseLighting](#elementdef-fediffuselighting)

| Name: | *<a id="elementdef-fediffuselighting"></a>`feDiffuseLighting`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), [script](svg2/interact.md#elementdef-script) and exactly one [light sources](#light-source) element, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [surfaceScale](#element-attrdef-fediffuselighting-surfacescale) - [diffuseConstant](#element-attrdef-fediffuselighting-diffuseconstant) - [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength) |
| DOM Interfaces: | [SVGFEDiffuseLightingElement](#InterfaceSVGFEDiffuseLightingElement) |

This filter primitive lights an image using the alpha channel as a bump map. The resulting image is an RGBA opaque image based on the light color with alpha = 1.0 everywhere. The lighting calculation follows the standard diffuse component of the Phong lighting model. The resulting image depends on the light color, light position and surface geometry of the input bump map.

The light map produced by this filter primitive can be combined with a texture image using the multiply term of the *arithmetic* [feComposite](#elementdef-fecomposite) compositing method. Multiple [light sources](#light-source) can be simulated by adding several of these light maps together before applying it to the texture image.

The formulas below make use of 3x3 filters. Because they operate on pixels, such filters are inherently resolution-dependent. To make [feDiffuseLighting](#elementdef-fediffuselighting) produce resolution-independent results, an explicit value should be provided for the attribute [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength).

[kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength), in combination with the other attributes, defines an implicit pixel grid in the filter effects coordinate system (i.e., the coordinate system established by the [primitiveUnits](#element-attrdef-filter-primitiveunits) attribute). The input image will be temporarily rescaled to match its pixels with kernelUnitLength. The 3x3 filters are applied to the resampled image. After applying the filter, the image is resampled back to its original resolution.

Note: Depending on the speed of the available interpolates, this choice may be affected by the [image-rendering](css-images-3.md#propdef-image-rendering) property setting.

Note: Implementations might choose approaches that minimize or eliminate resampling when not necessary to produce proper results, such as when the document is zoomed out such that [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength) is considerably smaller than a device pixel.

For the formulas that follow, the `Norm(A_x,A_y,A_z)` function is defined as:

Norm ( A x , A y , A z ) = A x 2 + A y 2 + A z 2 Norm(A_x,A_y,A_z) = sqrt { A_x^2+A_y^2+A_z^2}

Note: User agents may use the the "fast inverse square root" to optimize the equation and avoid time differences on extrema color values. See [§ 15 Privacy Considerations](#privacy) section for more details about timing attacks.

The resulting RGBA image is computed as follows:

` D_r = k_d * N.L * L_r D_g = k_d * N.L * L_g D_b = k_d * N.L * L_b D_a = 1.0 `

where

k_d = diffuse lighting constant N = surface normal unit vector, a function of x and y L = unit vector pointing from surface to light, a function of x and y in the point and spot light cases L_r,L_g,L_b = RGB components of light, a function of x and y in the spot light case

N is a function of x and y and depends on the surface gradient as follows:

The surface described by the input alpha image I(x,y) is:

`Z (x,y) = surfaceScale * I(x,y)`

<a id="SurfaceNormalCalculations"></a>Surface normal is calculated using the Sobel gradient 3x3 filter. Different filter kernels are used depending on whether the given pixel is on the interior or an edge. For each case, the formula is:

` N_x (x,y) = - surfaceScale * FACTOR_x *            (K_x(0,0)*I(x-dx,y-dy) + K_x(1,0)*I(x,y-dy) + K_x(2,0)*I(x+dx,y-dy) +             K_x(0,1)*I(x-dx,y)    + K_x(1,1)*I(x,y)    + K_x(2,1)*I(x+dx,y)    +             K_x(0,2)*I(x-dx,y+dy) + K_x(1,2)*I(x,y+dy) + K_x(2,2)*I(x+dx,y+dy)) N_y (x,y) = - surfaceScale * FACTOR_y *            (K_y(0,0)*I(x-dx,y-dy) + K_y(1,0)*I(x,y-dy) + K_y(2,0)*I(x+dx,y-dy) +             K_y(0,1)*I(x-dx,y)    + K_y(1,1)*I(x,y)    + K_y(2,1)*I(x+dx,y)    +             K_y(0,2)*I(x-dx,y+dy) + K_y(1,2)*I(x,y+dy) + K_y(2,2)*I(x+dx,y+dy)) N_z (x,y) = 1.0 N = (N_x, N_y, N_z) / Norm((N_x,N_y,N_z)) `

In these formulas, the `dx` and `dy` values (e.g., `I(x-dx,y-dy)`), represent deltas relative to a given `(x,y)` position for the purpose of estimating the slope of the surface at that point. These deltas are determined by the value (explicit or implicit) of attribute [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength).

| Top/left corner: | Top row: | Top/right corner: |
| --- | --- | --- |
| FACTOR_x=2/(3*dx) K_x = \| 0 0 0 \| \| 0 -2 2 \| \| 0 -1 1 \| FACTOR_y=2/(3*dy) K_y = \| 0 0 0 \| \| 0 -2 -1 \| \| 0 2 1 \| | FACTOR_x=1/(3*dx) K_x = \| 0 0 0 \| \| -2 0 2 \| \| -1 0 1 \| FACTOR_y=1/(2*dy) K_y = \| 0 0 0 \| \| -1 -2 -1 \| \| 1 2 1 \| | FACTOR_x=2/(3*dx) K_x = \| 0 0 0 \| \| -2 2 0 \| \| -1 1 0 \| FACTOR_y=2/(3*dy) K_y = \| 0 0 0 \| \| -1 -2 0 \| \| 1 2 0 \| |
| Left column: | Interior pixels: | Right column: |
| FACTOR_x=1/(2*dx) K_x = \| 0 -1 1 \| \| 0 -2 2 \| \| 0 -1 1 \| FACTOR_y=1/(3*dy) K_y = \| 0 -2 -1 \| \| 0 0 0 \| \| 0 2 1 \| | FACTOR_x=1/(4*dx) K_x = \| -1 0 1 \| \| -2 0 2 \| \| -1 0 1 \| FACTOR_y=1/(4*dy) K_y = \| -1 -2 -1 \| \| 0 0 0 \| \| 1 2 1 \| | FACTOR_x=1/(2*dx) K_x = \| -1 1 0\| \| -2 2 0\| \| -1 1 0\| FACTOR_y=1/(3*dy) K_y = \| -1 -2 0 \| \| 0 0 0 \| \| 1 2 0 \| |
| Bottom/left corner: | Bottom row: | Bottom/right corner: |
| FACTOR_x=2/(3*dx) K_x = \| 0 -1 1 \| \| 0 -2 2 \| \| 0 0 0 \| FACTOR_y=2/(3*dy) K_y = \| 0 -2 -1 \| \| 0 2 1 \| \| 0 0 0 \| | FACTOR_x=1/(3*dx) K_x = \| -1 0 1 \| \| -2 0 2 \| \| 0 0 0 \| FACTOR_y=1/(2*dy) K_y = \| -1 -2 -1 \| \| 1 2 1 \| \| 0 0 0 \| | FACTOR_x=2/(3*dx) K_x = \| -1 1 0 \| \| -2 2 0 \| \| 0 0 0 \| FACTOR_y=2/(3*dy) K_y = \| -1 -2 0 \| \| 1 2 0 \| \| 0 0 0 \| |

L, the unit vector from the image sample to the light, is calculated as follows:

For Infinite [light sources](#light-source) it is constant:

` L_x = cos(azimuth)*cos(elevation) L_y = sin(azimuth)*cos(elevation) L_z = sin(elevation) `

For Point and spot lights it is a function of position:

` L_x = Light_x - x L_y = Light_y - y L_z = Light_z - Z(x,y) L = (L_x, L_y, L_z) / Norm(L_x, L_y, L_z) `

where Light_x, Light_y, and Light_z are the input light position.

L_r,L_g,L_b, the light color vector, is a function of position in the spot light case only:

` L_r = Light_r*pow((-L.S),specularExponent) L_g = Light_g*pow((-L.S),specularExponent) L_b = Light_b*pow((-L.S),specularExponent) `

where S is the unit vector pointing from the light to the point (pointsAtX, pointsAtY, pointsAtZ) in the x-y plane:

` S_x = pointsAtX - Light_x S_y = pointsAtY - Light_y S_z = pointsAtZ - Light_z S = (S_x, S_y, S_z) / Norm(S_x, S_y, S_z) `

If L.S is positive, no light is present. (L_r = L_g = L_b = 0). If [limitingConeAngle](#element-attrdef-fespotlight-limitingconeangle) is specified, -L.S < cos(limitingConeAngle) also indicates that no light is present.

*Attribute definitions:*

***<a id="element-attrdef-fediffuselighting-surfacescale"></a>`surfaceScale`* = "*[<number>](css-values-4.md#number-value)*" **
:

height of surface when A_in = 1.

If the attribute is not specified, then the effect is as if a value of 1 were specified.

Animatable: yes.

***<a id="element-attrdef-fediffuselighting-diffuseconstant"></a>`diffuseConstant`* = "*[<number>](css-values-4.md#number-value)*" **
:

kd in Phong lighting model. In SVG, this can be any non-negative number.

If the attribute is not specified, then the effect is as if a value of 1 were specified.

Animatable: yes.

***<a id="element-attrdef-fediffuselighting-kernelunitlength"></a>`kernelUnitLength`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The first number is the <dx> value. The second number is the <dy> value. If the <dy> value is not specified, it defaults to the same value as <dx>. Indicates the intended distance in current filter units (i.e., units as determined by the value of attribute [primitiveUnits](#element-attrdef-filter-primitiveunits)) for `dx` and `dy`, respectively, in the [surface normal calculation formulas](#SurfaceNormalCalculations). By specifying value(s) for [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength), the kernel becomes defined in a scalable, abstract coordinate system. If kernelUnitLength is not specified, the `dx` and `dy` values should represent very small deltas relative to a given `(x,y)` position, which might be implemented in some cases as one pixel in the intermediate image offscreen bitmap, which is a pixel-based coordinate system, and thus potentially not scalable. For some level of consistency across display media and user agents, it is necessary that a value be provided for kernelUnitLength.

If a negative or zero value is specified the default value will be used instead.

Note: This attribute is deprecated and will be removed. It does not provide a reliable way to create platform independent results. Future versions of this specification will cover this use case.

Animatable: yes.

The [light source](#light-source) is defined by one of the child elements [feDistantLight](#elementdef-fedistantlight), [fePointLight](#elementdef-fepointlight) or [feSpotLight](#elementdef-fespotlight). The light color is specified by property [lighting-color](#propdef-lighting-color).

### <a id="feDisplacementMapElement"></a>9.11. Filter primitive [feDisplacementMap](#elementdef-fedisplacementmap)

| Name: | *<a id="elementdef-fedisplacementmap"></a>`feDisplacementMap`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [in2](#element-attrdef-fedisplacementmap-in2) - [scale](#element-attrdef-fedisplacementmap-scale) - [xChannelSelector](#element-attrdef-fedisplacementmap-xchannelselector) - [yChannelSelector](#element-attrdef-fedisplacementmap-ychannelselector) |
| DOM Interfaces: | [SVGFEDisplacementMapElement](#InterfaceSVGFEDisplacementMapElement) |

<a id="issue-1d845580"></a> Implementations do not match specification. [Issue #113]

This filter primitive uses the pixels values from the image from [in2](#element-attrdef-fedisplacementmap-in2) to spatially displace the image from [in](#element-attrdef-filter-primitive-in). This is the transformation to be performed:

```
P'(x,y) ← P( x + scale * (XC(x,y) - .5), y + scale * (YC(x,y) - .5))
```

where P(x,y) is the input image, [in](#element-attrdef-filter-primitive-in), and P'(x,y) is the destination. XC(x,y) and YC(x,y) are the component values of the channel designated by the [xChannelSelector](#element-attrdef-fedisplacementmap-xchannelselector) and [yChannelSelector](#element-attrdef-fedisplacementmap-ychannelselector). For example, to use the R component of [in2](#element-attrdef-fedisplacementmap-in2) to control displacement in x and the G component of Image2 to control displacement in y, set xChannelSelector to "R" and yChannelSelector to "G".

The displacement map, [in2](#element-attrdef-fedisplacementmap-in2), defines the inverse of the mapping performed.

The input image [in](#element-attrdef-filter-primitive-in) is to remain premultiplied for this filter primitive. The calculations using the pixel values from [in2](#element-attrdef-fedisplacementmap-in2) are performed using non-premultiplied color values.

This filter can have arbitrary non-localized effect on the input which might require substantial buffering in the processing pipeline. However with this formulation, any intermediate buffering needs can be determined by [scale](#element-attrdef-fedisplacementmap-scale) which represents the maximum range of displacement in either x or y.

When applying this filter, the source pixel location will often lie between several source pixels.

Note: Depending on the speed of the available interpolents, this choice may be affected by the [image-rendering](css-images-3.md#propdef-image-rendering) property setting.

Note: A future version of this spec will define the interpolation method to be used when distorting the source image making UAs rendering result more interoperable.

The [color-interpolation-filters](#propdef-color-interpolation-filters) property only applies to the [in2](#element-attrdef-fedisplacementmap-in2) source image and does not apply to the [in](#element-attrdef-filter-primitive-in) source image. The in source image must remain in its current color space.

*Attribute definitions:*

***<a id="element-attrdef-fedisplacementmap-scale"></a>`scale`* = "*[<number>](css-values-4.md#number-value)*" **
:

Displacement scale factor. The amount is expressed in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

When the value of this attribute is 0, this operation has no effect on the source image.

The [initial value](svg2/types.md#TermInitialValue) for [scale](#element-attrdef-fedisplacementmap-scale) is 0.

Animatable: yes.

***<a id="element-attrdef-fedisplacementmap-xchannelselector"></a>`xChannelSelector`* = "*R | G | B | A*" **
:

Indicates which channel from [in2](#element-attrdef-fedisplacementmap-in2) to use to displace the pixels in [in](#element-attrdef-filter-primitive-in) along the x-axis.

The [initial value](svg2/types.md#TermInitialValue) for [xChannelSelector](#element-attrdef-fedisplacementmap-xchannelselector) is A.

Animatable: yes.

***<a id="element-attrdef-fedisplacementmap-ychannelselector"></a>`yChannelSelector`* = "*R | G | B | A*" **
:

Indicates which channel from [in2](#element-attrdef-fedisplacementmap-in2) to use to displace the pixels in [in](#element-attrdef-filter-primitive-in) along the y-axis.

The [initial value](svg2/types.md#TermInitialValue) for [yChannelSelector](#element-attrdef-fedisplacementmap-ychannelselector) is A.

Animatable: yes.

***<a id="element-attrdef-fedisplacementmap-in2"></a>`in2`* = "*(see [in](#element-attrdef-filter-primitive-in) attribute)*" **
:

The second input image, which is used to displace the pixels in the image from attribute [in](#element-attrdef-filter-primitive-in). See defintion for in attribute.

Animatable: yes.

### <a id="feDropShadowElement"></a>9.12. Filter primitive [feDropShadow](#elementdef-fedropshadow)

| Name: | *<a id="elementdef-fedropshadow"></a>`feDropShadow`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [stdDeviation](#element-attrdef-fedropshadow-stddeviation) - [dx](#element-attrdef-fedropshadow-dx) - [dy](#element-attrdef-fedropshadow-dy) |
| DOM Interfaces: | [SVGFEDropShadowElement](#InterfaceSVGFEDropShadowElement) |

This filter creates a drop shadow of the input image. It is a shorthand filter, and is defined in terms of combinations of other [filter primitives](#filter-primitive). The expectation is that it can be optimized more easily by implementations.

The result of a [feDropShadow](#elementdef-fedropshadow) filter primitive is equivalent to the following:

```
<feGaussianBlur in="alpha-channel-of-feDropShadow-in"   stdDeviation="stdDeviation-of-feDropShadow"/>
<feOffset dx="dx-of-feDropShadow"   dy="dy-of-feDropShadow" result="offsetblur"/>
<feFlood flood-color="flood-color-of-feDropShadow"  flood-opacity="flood-opacity-of-feDropShadow"/>
<feComposite in2="offsetblur" operator="in"/>
<feMerge>
  <feMergeNode/>
  <feMergeNode in="in-of-feDropShadow"/>
</feMerge>
```

The above divided into steps:

1.
Take the alpha channel of the input to the [feDropShadow](#elementdef-fedropshadow) filter primitive and the [stdDeviation](#element-attrdef-fedropshadow-stddeviation) on the feDropShadow and do processing as if the following [feGaussianBlur](#elementdef-fegaussianblur) was applied:
```
<feGaussianBlur in="alpha-channel-of-feDropShadow-in" stdDeviation="stdDeviation-of-feDropShadow"/>
```

2.
Offset the result of step 1 by [dx](#element-attrdef-fedropshadow-dx) and [dy](#element-attrdef-fedropshadow-dy) as specified on the [feDropShadow](#elementdef-fedropshadow) element, equivalent to applying an [feOffset](#elementdef-feoffset) with these parameters:
```
<feOffset dx="dx-of-feDropShadow" dy="dy-of-feDropShadow" result="offsetblur"/>
```

3.
Do processing as if an [feFlood](#elementdef-feflood) element with [flood-color](#propdef-flood-color) and [flood-opacity](#propdef-flood-opacity) as specified on the [feDropShadow](#elementdef-fedropshadow) was applied:
```
<feFlood flood-color="flood-color-of-feDropShadow" flood-opacity="flood-opacity-of-feDropShadow"/>
```

4.
Composite the result of the [feFlood](#elementdef-feflood) in step 3 with the result of the [feOffset](#elementdef-feoffset) in step 2 as if an [feComposite](#elementdef-fecomposite) filter primitive with `operator="in"` was applied:
```
<feComposite in2="offsetblur" operator="in"/>
```

5.
Finally merge the result of the previous step, doing processing as if the following [feMerge](#elementdef-femerge) was performed:
```
<feMerge>
  <feMergeNode/>
  <feMergeNode in="in-of-feDropShadow"/>
</feMerge>
```

Note: that while the definition of the [feDropShadow](#elementdef-fedropshadow) filter primitive says that it can be expanded into an equivalent tree it is not required that it is implemented like that. The expectation is that user agents can optimize the handling by not having to do all the steps separately.

Beyond the DOM interface [SVGFEDropShadowElement](#InterfaceSVGFEDropShadowElement) there is no way of accessing the internals of the [feDropShadow](#elementdef-fedropshadow) filter primitive, meaning <a id="assert_dropShadowShadowTrees"></a>if the filter primitive is implemented as an equivalent tree then that tree must not be exposed to the DOM.

*Attribute definitions:*

***<a id="element-attrdef-fedropshadow-dx"></a>`dx`* = "*[<number>](css-values-4.md#number-value)*" **
:

The x offset of the drop shadow.

The [initial value](svg2/types.md#TermInitialValue) for [dx](#element-attrdef-fedropshadow-dx) is 2.

This attribute is then forwarded to the [dx](#element-attrdef-feoffset-dx) attribute of the internal [feOffset](#elementdef-feoffset) element.

Animatable: yes.

***<a id="element-attrdef-fedropshadow-dy"></a>`dy`* = "*[<number>](css-values-4.md#number-value)*" **
:

The y offset of the drop shadow.

The [initial value](svg2/types.md#TermInitialValue) for [dy](#element-attrdef-fedropshadow-dy) is 2.

This attribute is then forwarded to the [dy](#element-attrdef-feoffset-dy) attribute of the internal [feOffset](#elementdef-feoffset) element.

Animatable: yes.

***<a id="element-attrdef-fedropshadow-stddeviation"></a>`stdDeviation`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The standard deviation for the blur operation in the drop shadow.

The [initial value](svg2/types.md#TermInitialValue) for [stdDeviation](#element-attrdef-fedropshadow-stddeviation) is 2.

This attribute is then forwarded to the [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) attribute of the internal [feGaussianBlur](#elementdef-fegaussianblur) element.

Animatable: yes.

### <a id="feFloodElement"></a>9.13. Filter primitive [feFlood](#elementdef-feflood)

| Name: | *<a id="elementdef-feflood"></a>`feFlood`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) |
| DOM Interfaces: | [SVGFEFloodElement](#InterfaceSVGFEFloodElement) |

This filter primitive creates a rectangle filled with the color and opacity values from properties [flood-color](#propdef-flood-color) and [flood-opacity](#propdef-flood-opacity). The rectangle is as large as the [filter primitive subregion](#filter-primitive-subregion) established by the [feFlood](#elementdef-feflood) element.

#### <a id="FloodColorProperty"></a>9.13.1. The [flood-color](#propdef-flood-color) property

| Name: | *<a id="propdef-flood-color"></a>flood-color* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <color> |
| [Initial:](css-cascade-5.md#initial-values) | black |
| [Applies to:](css-cascade-5.md#applies-to) | [feFlood](#elementdef-feflood) and [feDropShadow](#elementdef-fedropshadow) elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

The [flood-color](#propdef-flood-color) property indicates what color to used to flood the current [filter primitive subregion](#filter-primitive-subregion).

The [flood-color](#propdef-flood-color) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

#### <a id="FloodOpacityProperty"></a>9.13.2. The [flood-opacity](#propdef-flood-opacity) property

| Name: | *<a id="propdef-flood-opacity"></a>flood-opacity* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'opacity'>](css-color-4.md#propdef-opacity) |
| [Initial:](css-cascade-5.md#initial-values) | 1 |
| [Applies to:](css-cascade-5.md#applies-to) | [feFlood](#elementdef-feflood) and [feDropShadow](#elementdef-fedropshadow) elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | the specified value converted to a number, clamped to the range [0,1] |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

The [flood-opacity](#propdef-flood-opacity) property defines the opacity value to use across the entire [filter primitive subregion](#filter-primitive-subregion). If the [flood-color](#propdef-flood-color) value includes an alpha channel, the alpha channel gets multiplied with the computed value of the flood-opacity property.

The [flood-opacity](#propdef-flood-opacity) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

### <a id="feGaussianBlurElement"></a>9.14. Filter primitive [feGaussianBlur](#elementdef-fegaussianblur)

| Name: | *<a id="elementdef-fegaussianblur"></a>`feGaussianBlur`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) - [edgeMode](#element-attrdef-fegaussianblur-edgemode) |
| DOM Interfaces: | [SVGFEGaussianBlurElement](#InterfaceSVGFEGaussianBlurElement) |

This filter primitive performs a Gaussian blur on the input image.

The Gaussian blur kernel is an approximation of the normalized convolution:

`G(x,y) = H(x)I(y)`

where

`H(x) = exp(-x^2/ (2s^2)) / sqrt(2π * s^2)`

and

`I(y) = exp(-y^2/ (2t^2)) / sqrt(2π * t^2)`

with "s" being the standard deviation in the x direction and "t" being the standard deviation in the y direction, as specified by [stdDeviation](#element-attrdef-fegaussianblur-stddeviation).

The value of [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) can be either one or two numbers. If two numbers are provided, the first number represents a standard deviation value along the x-axis of the current coordinate system and the second value represents a standard deviation in Y. If one number is provided, then that value is used for both X and Y.

Even if only one value is provided for [stdDeviation](#element-attrdef-fegaussianblur-stddeviation), this can be implemented as a separable convolution.

For larger values of "s" (s >= 2.0), an approximation can be used: Three successive box-blurs build a piece-wise quadratic convolution kernel, which approximates the Gaussian kernel to within roughly 3%.

`let d = floor(s * 3 * sqrt(2 * π) / 4 + 0.5)`

... if d is odd, use three box-blurs of size "d", centered on the output pixel.

... if d is even, two box-blurs of size "d" (the first one centered on the pixel boundary between the output pixel and the one to the left, the second one centered on the pixel boundary between the output pixel and the one to the right) and one box blur of size "d+1" centered on the output pixel.

The approximation formula also applies correspondingly to "t".

Frequently this operation will take place on alpha-only images, such as that produced by the built-in input, [SourceAlpha](#attr-valuedef-in-sourcealpha). The implementation may notice this and optimize the single channel case. This optimization must be omitted if it leads to privacy concerns of any matter. (See section [§ 15 Privacy Considerations](#privacy) for more details about timing attacks.) If the input has infinite extent and is constant (e.g [FillPaint](#attr-valuedef-in-fillpaint) where the fill is a solid color), this operation has no effect. If the input has infinite extent and the filter result where the fill is a solid color) is the input to an [feTile](#elementdef-fetile), the filter is evaluated with periodic boundary conditions.

*Attribute definitions:*

***<a id="element-attrdef-fegaussianblur-stddeviation"></a>`stdDeviation`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The standard deviation for the blur operation. If two [<number>](css-values-4.md#number-value) s are provided, the first number represents a standard deviation value along the x-axis of the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element. The second value represents a standard deviation in Y. If one number is provided, then that value is used for both X and Y.

A negative value or a value of zero disables the effect of the given filter primitive (i.e., the result is the filter input image).

If [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) is 0 in only one of X or Y, then the effect is that the blur is only applied in the direction that has a non-zero value.

The [initial value](svg2/types.md#TermInitialValue) for [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) is 0.

Animatable: yes.

***<a id="element-attrdef-fegaussianblur-edgemode"></a>`edgeMode`* = "[duplicate](#attr-valuedef-fegaussianblur-edgemode-duplicate) | [wrap](#attr-valuedef-fegaussianblur-edgemode-wrap) | [mirror](#attr-valuedef-feguassianblur-edgemode-mirror) | none" **
:

Determines how to extend the input image as necessary with color values so that the matrix operations can be applied when the kernel is positioned at or near the edge of the input image.

*<a id="attr-valuedef-fegaussianblur-edgemode-duplicate"></a>`duplicate`* indicates that the input image is extended along each of its borders as necessary by duplicating the color values at the given edge of the input image.

Original N-by-M image, where m=M-1 and n=N-1:

*<a id="attr-valuedef-fegaussianblur-edgemode-wrap"></a>`wrap`* indicates that the input image is extended by taking the color values from the opposite edge of the image.

*<a id="attr-valuedef-feguassianblur-edgemode-mirror"></a>`mirror`* indicates that the input image is extended by taking color values mirrored across the edge being sampled beyond.

The value none indicates that the input image is extended with pixel values of zero for R, G, B and A.

The [initial value](svg2/types.md#TermInitialValue) for [edgeMode](#element-attrdef-fegaussianblur-edgemode) is none.

Animatable: yes.

[The example](#intro) at the start of this chapter makes use of the [feGaussianBlur](#elementdef-fegaussianblur) filter primitive to create a drop shadow effect.

### <a id="feImageElement"></a>9.15. Filter primitive [feImage](#elementdef-feimage)

| Name: | *<a id="elementdef-feimage"></a>`feImage`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, animateTransform, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [externalResourcesRequired](svg11/struct.md#ExternalResourcesRequiredAttribute) - [preserveAspectRatio](#element-attrdef-feimage-preserveaspectratio) - [xlink:href](#element-attrdef-feimage-xlinkhref) - [href](#element-attrdef-feimage-href) - [crossorigin](#element-attrdef-feimage-crossorigin) |
| DOM Interfaces: | [SVGFEImageElement](#InterfaceSVGFEImageElement) |

This filter primitive refers to a graphic external to this filter element, which is loaded or rendered into an RGBA raster and becomes the result of the filter primitive.

This filter primitive can refer to an external image or can be a reference to another piece of SVG. It produces an image similar to the built-in image source [SourceGraphic](#attr-valuedef-in-sourcegraphic) except that the graphic comes from an external source.

If the [href](#element-attrdef-feimage-href) references a stand-alone image resource such as a JPEG, PNG or SVG file, then the image resource is rendered according to the behavior of the [image](svg2/embedded.md#elementdef-image) element; otherwise, the referenced resource is rendered according to the behavior of the [use](svg2/struct.md#elementdef-use) element. In either case, the current user coordinate system depends on the value of attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element. The processing of the [preserveAspectRatio](#element-attrdef-feimage-preserveaspectratio) attribute on the [feImage](#elementdef-feimage) element is identical to that of the image element.

A [href](#element-attrdef-feimage-href) reference that is an empty image (zero width or zero height), that fails to download, is non-existent, or that cannot be displayed (e.g. because it is not in a supported image format) fills the filter primitive subregion with transparent black.

<a id="assert_hqImageResampling"></a>When the referenced image must be resampled to match the device coordinate system, it is recommended that high quality viewers make use of appropriate interpolation techniques, for example bilinear or bicubic. Depending on the speed of the available interpolents, this choice may be affected by the [image-rendering](css-images-3.md#propdef-image-rendering) property setting.

*Attribute definitions:*

***<a id="element-attrdef-feimage-xlinkhref"></a>`xlink:href`* = "[<url>](#typedef-filter-url)" **
:

See [href](#element-attrdef-feimage-href) attribute.

Animatable: yes.

Note: This *xlink:href* attribute is deprecated and should not be used in new content, it’s included for backwards compatibility reasons only. Authors should use the [href](#element-attrdef-feimage-href) attribute instead.

***<a id="element-attrdef-feimage-href"></a>`href`* = "[<url>](#typedef-filter-url)" **
:

An [<url>](#typedef-filter-url) to an image resource or to an element. If both, the [xlink:href](#element-attrdef-feimage-xlinkhref) and the [href](#element-attrdef-feimage-href) attribute are specified, the latter overrides the first definition.

Animatable: yes.

***<a id="element-attrdef-feimage-preserveaspectratio"></a>`preserveAspectRatio`* = "*[defer] <align> [<meetOrSlice>]*" **
:

See [preserveAspectRatio](#element-attrdef-feimage-preserveaspectratio).

The [initial value](svg2/types.md#TermInitialValue) for [preserveAspectRatio](#element-attrdef-feimage-preserveaspectratio) is xMidYMid meet.

Animatable: yes.

***<a id="element-attrdef-feimage-crossorigin"></a>`crossorigin`* = "*anonymous* | *use-credentials*" **
:

The crossorigin attribute is a CORS settings attribute. Its purpose is to allow images from third-party sites that allow cross-origin access to be used with [feDisplacementMap](#elementdef-fedisplacementmap). For the defintion see crossorigin attribute for the img tag [[HTML]](#biblio-html) and the [§ 15 Privacy Considerations](#privacy) section in this specification.

Animatable: no.

<a id="example-d975b48d"></a> The following example illustrates how images are placed relative to an object. From left to right:

-
The default placement of an image. Note that the image is centered in the [filter region](#filter-region) and has the maximum size that will fit in the region consistent with preserving the aspect ratio.
-
The image stretched to fit the bounding box of an object.
-
The image placed using user coordinates. Note that the image is first centered in a box the size of the [filter region](#filter-region) and has the maximum size that will fit in the box consistent with preserving the aspect ratio. This box is then shifted by the given [x](#element-attrdef-filter-primitive-x) and [y](#element-attrdef-filter-primitive-y) values relative to the viewport the object is in.

```
<svg width="600" height="250" viewBox="0 0 600 250"
     xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink">
  <title>Example feImage - Examples of feImage use</title>
  <desc>Three examples of using feImage, the first showing the
        default rendering, the second showing the image fit
        to a box and the third showing the image
        shifted and clipped.</desc>
  <defs>
    <filter id="Default">
      <feImage xlink:href="smiley.png" />
    </filter>
    <filter id="Fitted" primitiveUnits="objectBoundingBox">
      <feImage xlink:href="smiley.png"
         x="0" y="0" width="100%" height="100%"
         preserveAspectRatio="none"/>
    </filter>
    <filter id="Shifted">
      <feImage xlink:href="smiley.png"
         x="500" y="5"/>
    </filter>
  </defs>
  <rect fill="none" stroke="blue"
        x="1" y="1" width="598" height="248"/>
  <g>
    <rect x="50"  y="25" width="100" height="200" filter="url(#Default)"/>
    <rect x="50"  y="25" width="100" height="200" fill="none" stroke="green"/>
    <rect x="250" y="25" width="100" height="200" filter="url(#Fitted)"/>
    <rect x="250" y="25" width="100" height="200" fill="none" stroke="green"/>
    <rect x="450" y="25" width="100" height="200" filter="url(#Shifted)"/>
    <rect x="450" y="25" width="100" height="200" fill="none" stroke="green"/>
  </g>
</svg>
```

[image: Example feImage — Examples of feImage use]

Example of feImage

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feImage-01.svg)

### <a id="feMergeElement"></a>9.16. Filter primitive [feMerge](#elementdef-femerge)

| Name: | *<a id="elementdef-femerge"></a>`feMerge`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), [feMergeNode](#elementdef-femergenode), [script](svg2/interact.md#elementdef-script) elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) |
| DOM Interfaces: | [SVGFEMergeElement](#InterfaceSVGFEMergeElement) |

This filter primitive composites input image layers on top of each other using the *over* operator with *Input1* (corresponding to the first [feMergeNode](#elementdef-femergenode) child element) on the bottom and the last specified input, *InputN* (corresponding to the last feMergeNode child element), on top.

Many effects produce a number of intermediate layers in order to create the final output image. This filter allows us to collapse those into a single image. Although this could be done by using n-1 Composite-filters, it is more convenient to have this common operation available in this form, and offers the implementation some additional flexibility.

Each [feMerge](#elementdef-femerge) element can have any number of [feMergeNode](#elementdef-femergenode) subelements, each of which has an [in](#element-attrdef-filter-primitive-in) attribute.

The canonical implementation of feMerge is to render the entire effect into one RGBA layer, and then render the resulting layer on the output device. In certain cases (in particular if the output device itself is a continuous tone device), and since merging is associative, it might be a sufficient approximation to evaluate the effect one layer at a time and render each layer individually onto the output device bottom to top.

If the topmost image input is [SourceGraphic](#attr-valuedef-in-sourcegraphic) and this [feMerge](#elementdef-femerge) is the last filter primitive in the filter, the implementation is encouraged to render the layers up to that point, and then render the SourceGraphic directly from its vector description on top.

[The example](#intro) at the start of this chapter makes use of the [feMerge](#elementdef-femerge) filter primitive to composite two intermediate filter results together.

#### <a id="feMergeNodeElement"></a>9.16.1. Merge node [feMergeNode](#elementdef-femergenode)

| Name: | *<a id="elementdef-femergenode"></a>`feMergeNode`* |
| --- | --- |
| Categories: | None. |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [in](#element-attrdef-filter-primitive-in) |
| DOM Interfaces: | [SVGFEMergeNodeElement](#InterfaceSVGFEMergeNodeElement) |

### <a id="feMorphologyElement"></a>9.17. Filter primitive [feMorphology](#elementdef-femorphology)

| Name: | *<a id="elementdef-femorphology"></a>`feMorphology`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [operator](#element-attrdef-femorphology-operator) - [radius](#element-attrdef-femorphology-radius) |
| DOM Interfaces: | [SVGFEMorphologyElement](#InterfaceSVGFEMorphologyElement) |

This filter primitive performs "fattening" or "thinning" of artwork. It is particularly useful for fattening or thinning an alpha channel.

The dilation (or erosion) kernel is a rectangle with a width of 2**x-radius* and a height of 2**y-radius*. In dilation, the output pixel is the individual component-wise maximum of the corresponding R,G,B,A values in the input image’s kernel rectangle. In erosion, the output pixel is the individual component-wise minimum of the corresponding R,G,B,A values in the input image’s kernel rectangle.

Frequently this operation will take place on alpha-only images, such as that produced by the built-in input, [SourceAlpha](#attr-valuedef-in-sourcealpha). In that case, the implementation might want to optimize the single channel case. This optimization must be omitted if it leads to privacy concerns of any matter. (See section [§ 15 Privacy Considerations](#privacy) for more details.)

If the input has infinite extent and is constant (e.g [FillPaint](#attr-valuedef-in-fillpaint) where the fill is a solid color), this operation has no effect. If the input has infinite extent and the filter result is the input to an [feTile](#elementdef-fetile), the filter is evaluated with periodic boundary conditions.

Because [feMorphology](#elementdef-femorphology) operates on premultipied color values, it will always result in color values less than or equal to the alpha channel.

*Attribute definitions:*

***<a id="element-attrdef-femorphology-operator"></a>`operator`* = "*erode | dilate*" **
:

A keyword indicating whether to erode (i.e., thin) or dilate (fatten) the source graphic.

The [initial value](svg2/types.md#TermInitialValue) for [operator](#element-attrdef-femorphology-operator) is erode.

Animatable: yes.

***<a id="element-attrdef-femorphology-radius"></a>`radius`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The radius (or radii) for the operation. If two [<number>](css-values-4.md#number-value) s are provided, the first number represents a x-radius and the second value represents a y-radius. If one number is provided, then that value is used for both X and Y. The values are in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

A negative or zero value disables the effect of the given filter primitive (i.e., the result is the filter input image).

The [initial value](svg2/types.md#TermInitialValue) for [radius](#element-attrdef-femorphology-radius) is 0.

Animatable: yes.

<a id="example-131071e3"></a>

```
<svg width="5cm" height="7cm" viewBox="0 0 700 500"
     xmlns="http://www.w3.org/2000/svg">
  <title>Example feMorphology - Examples of erode and dilate</title>
  <desc>Five text strings drawn as outlines.
        The first is unfiltered. The second and third use 'erode'.
        The fourth and fifth use 'dilate'.</desc>
  <defs>
    <filter id="Erode3">
      <feMorphology operator="erode" in="SourceGraphic" radius="3" />
    </filter>
    <filter id="Erode6">
      <feMorphology operator="erode" in="SourceGraphic" radius="6" />
    </filter>
    <filter id="Dilate3">
      <feMorphology operator="dilate" in="SourceGraphic" radius="3" />
    </filter>
    <filter id="Dilate6">
      <feMorphology operator="dilate" in="SourceGraphic" radius="6" />
    </filter>
  </defs>
  <rect fill="none" stroke="blue" stroke-width="2"
        x="1" y="1" width="698" height="498"/>
  <g isolation="isolate" >
    <g font-family="Verdana" font-size="75"
              fill="none" stroke="black" stroke-width="6" >
      <text x="50" y="90">Unfiltered</text>
      <text x="50" y="180" filter="url(#Erode3)" >Erode radius 3</text>
      <text x="50" y="270" filter="url(#Erode6)" >Erode radius 6</text>
      <text x="50" y="360" filter="url(#Dilate3)" >Dilate radius 3</text>
      <text x="50" y="450" filter="url(#Dilate6)" >Dilate radius 6</text>
    </g>
  </g>
</svg>
```

[image: Example of feMorphology]

Example of feMorphology

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feMorphology.svg)

### <a id="feOffsetElement"></a>9.18. Filter primitive [feOffset](#elementdef-feoffset)

| Name: | *<a id="elementdef-feoffset"></a>`feOffset`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [dx](#element-attrdef-feoffset-dx) - [dy](#element-attrdef-feoffset-dy) |
| DOM Interfaces: | [SVGFEOffsetElement](#InterfaceSVGFEOffsetElement) |

This filter primitive offsets the input image relative to its current position in the image space by the specified vector.

This is important for effects like drop shadows.

When applying this filter, the destination location may be offset by a fraction of a pixel in device space. <a id="assert_hqFeOffsetInterpolation"></a>In this case a high quality viewer should make use of appropriate interpolation techniques, for example bilinear or bicubic. This is especially recommended for dynamic viewers where this interpolation provides visually smoother movement of images. For static viewers this is less of a concern. Close attention should be made to the [image-rendering](css-images-3.md#propdef-image-rendering) property setting to determine the authors intent.

*Attribute definitions:*

***<a id="element-attrdef-feoffset-dx"></a>`dx`* = "*[<number>](css-values-4.md#number-value)*" **
:

The amount to offset the input graphic along the x-axis. The offset amount is expressed in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

The [initial value](svg2/types.md#TermInitialValue) for [dx](#element-attrdef-feoffset-dx) is 0.

Animatable: yes.

***<a id="element-attrdef-feoffset-dy"></a>`dy`* = "*[<number>](css-values-4.md#number-value)*" **
:

The amount to offset the input graphic along the y-axis. The offset amount is expressed in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

The [initial value](svg2/types.md#TermInitialValue) for [dy](#element-attrdef-feoffset-dy) is 0.

Animatable: yes.

[The example](#intro) at the start of this chapter makes use of the [feOffset](#elementdef-feoffset) filter primitive to offset the drop shadow from the original source graphic.

### <a id="feSpecularLightingElement"></a>9.19. Filter primitive [feSpecularLighting](#elementdef-fespecularlighting)

| Name: | *<a id="elementdef-fespecularlighting"></a>`feSpecularLighting`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), [script](svg2/interact.md#elementdef-script) and exactly one [light sources](#light-source) element, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) - [surfaceScale](#element-attrdef-fespecularlighting-surfacescale) - [specularConstant](#element-attrdef-fespecularlighting-specularconstant) - [specularExponent](#element-attrdef-fespecularlighting-specularexponent) - [kernelUnitLength](#element-attrdef-fespecularlighting-kernelunitlength) |
| DOM Interfaces: | [SVGFESpecularLightingElement](#InterfaceSVGFESpecularLightingElement) |

This filter primitive lights a source graphic using the alpha channel as a bump map. The resulting image is an RGBA image based on the light color. The lighting calculation follows the standard specular component of the Blinn-Phong lighting model. The resulting image depends on the light color, light position and surface geometry of the input bump map. The result of the lighting calculation is added. The filter primitive assumes that the viewer is at infinity in the z direction (i.e., the unit vector in the eye direction is (0,0,1) everywhere).

Note: This filter primitive produces an image which contains the specular reflection part of the lighting calculation. Such a map is intended to be combined with a texture from a second filter primitive using the *add* term of the *arithmetic* [feComposite](#elementdef-fecomposite) method. Multiple [light sources](#light-source) can be simulated by adding several of these light maps before applying it to the texture image.

The resulting RGBA image is computed as follows:

` S_r = k_s * pow(N.H, specularExponent) * L_r S_g = k_s * pow(N.H, specularExponent) * L_g S_b = k_s * pow(N.H, specularExponent) * L_b S_a = max(S_r, S_g, S_b) `

where

k_s = specular lighting constant N = surface normal unit vector, a function of x and y H = "halfway" unit vector between eye unit vector and light unit vector L_r,L_g,L_b = RGB components of light

See [feDiffuseLighting](#elementdef-fediffuselighting) for definition of N and (L_r, L_g, L_b).

The definition of H reflects our assumption of the constant eye vector E = (0,0,1):

` H = (L + E) / Norm(L+E) `

where L is the light unit vector.

Unlike the [feDiffuseLighting](#elementdef-fediffuselighting), the [feSpecularLighting](#elementdef-fespecularlighting) filter produces a non-opaque image. This is due to the fact that the specular result (S_r,S_g,S_b,S_a) is meant to be added to the textured image. The alpha channel of the result is the max of the color components, so that where the specular light is zero, no additional coverage is added to the image and a fully white highlight will add opacity.

The [feDiffuseLighting](#elementdef-fediffuselighting) and [feSpecularLighting](#elementdef-fespecularlighting) filters will often be applied together. An implementation may detect this and calculate both maps in one pass, instead of two.

*Attribute definitions:*

***<a id="element-attrdef-fespecularlighting-surfacescale"></a>`surfaceScale`* = "*[<number>](css-values-4.md#number-value)*" **
:

height of surface when A_in = 1.

The [initial value](svg2/types.md#TermInitialValue) for [surfaceScale](#element-attrdef-fespecularlighting-surfacescale) is 1.

Animatable: yes.

***<a id="element-attrdef-fespecularlighting-specularconstant"></a>`specularConstant`* = "*[<number>](css-values-4.md#number-value)*" **
:

ks in Phong lighting model. In SVG, this can be any non-negative number.

The [initial value](svg2/types.md#TermInitialValue) for [specularConstant](#element-attrdef-fespecularlighting-specularconstant) is 1.

Animatable: yes.

***<a id="element-attrdef-fespecularlighting-specularexponent"></a>`specularExponent`* = "*[<number>](css-values-4.md#number-value)*" **
:

Exponent for specular term, larger is more "shiny".

The [initial value](svg2/types.md#TermInitialValue) for [specularExponent](#element-attrdef-fespecularlighting-specularexponent) is 1.

Animatable: yes.

***<a id="element-attrdef-fespecularlighting-kernelunitlength"></a>`kernelUnitLength`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The first number is the <dx> value. The second number is the <dy> value.

If the <dy> value is not specified, it defaults to the same value as <dx>. Indicates the intended distance in current filter units (i.e., units as determined by the value of attribute [primitiveUnits](#element-attrdef-filter-primitiveunits)) for `dx` and `dy`, respectively, in the [surface normal calculation formulas](#SurfaceNormalCalculations). By specifying value(s) for [kernelUnitLength](#element-attrdef-fespecularlighting-kernelunitlength), the kernel becomes defined in a scalable, abstract coordinate system.

If [kernelUnitLength](#element-attrdef-fespecularlighting-kernelunitlength) is not specified, the `dx` and `dy` values should represent very small deltas relative to a given `(x,y)` position, which might be implemented in some cases as one pixel in the intermediate image offscreen bitmap, which is a pixel-based coordinate system, and thus potentially not scalable. For some level of consistency across display media and user agents, it is necessary that a value be provided for kernelUnitLength.

Animatable: yes.

The light source is defined by one of the child elements [feDistantLight](#elementdef-fedistantlight), [fePointLight](#elementdef-fepointlight) or [feSpotLight](#elementdef-fespotlight). The light color is specified by property [lighting-color](#propdef-lighting-color).

[The example](#intro) at the start of this chapter makes use of the [feSpecularLighting](#elementdef-fespecularlighting) filter primitive to achieve a highly reflective, 3D glowing effect.

### <a id="feTileElement"></a>9.20. Filter primitive [feTile](#elementdef-fetile)

| Name: | *<a id="elementdef-fetile"></a>`feTile`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [in](#element-attrdef-filter-primitive-in) |
| DOM Interfaces: | [SVGFETileElement](#InterfaceSVGFETileElement) |

This filter primitive fills a target rectangle with a repeated, tiled pattern of an input image. The target rectangle is as large as the [filter primitive subregion](#filter-primitive-subregion) established by the [feTile](#elementdef-fetile) element.

Typically, the input image has been defined with its own [filter primitive subregion](#filter-primitive-subregion) in order to define a reference tile. [feTile](#elementdef-fetile) replicates the reference tile in both X and Y to completely fill the target rectangle. The top/left corner of each given tile is at location `(x + i*width, y + j*height)`, where `(x,y)` represents the top/left of the input image’s filter primitive subregion, `width` and `height` represent the width and height of the input image’s filter primitive subregion, and `i` and `j` can be any integer value. In most cases, the input image will have a smaller filter primitive subregion than the feTile in order to achieve a repeated pattern effect.

Implementers must take appropriate measures in constructing the tiled image to avoid artifacts between tiles, particularly in situations where the user to device transform includes shear and/or rotation. Unless care is taken, interpolation can lead to edge pixels in the tile having opacity values lower or higher than expected due to the interaction of painting adjacent tiles which each have partial overlap with particular pixels.

### <a id="feTurbulenceElement"></a>9.21. Filter primitive [feTurbulence](#elementdef-feturbulence)

| Name: | *<a id="elementdef-feturbulence"></a>`feTurbulence`* |
| --- | --- |
| Categories: | [filter primitive](#filter-primitive) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - presentation attributes — alignment-baseline, baseline-shift, [clip](css-masking-1.md#propdef-clip), [clip-path](css-masking-1.md#propdef-clip-path), [clip-rule](css-masking-1.md#propdef-clip-rule), [color](css-color-4.md#propdef-color), [color-interpolation](svg2/painting.md#ColorInterpolationProperty), [color-interpolation-filters](#propdef-color-interpolation-filters), color-rendering, cursor, direction, display, dominant-baseline, enable-background, [fill](svg2/painting.md#FillProperty), [fill-opacity](svg2/painting.md#FillOpacityProperty), [fill-rule](svg2/painting.md#FillRuleProperty), [filter](#propdef-filter), [flood-color](#propdef-flood-color), [flood-opacity](#propdef-flood-opacity), font, font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, glyph-orientation-vertical, [image-rendering](css-images-3.md#propdef-image-rendering), isolation, kerning, letter-spacing, [lighting-color](#propdef-lighting-color), [marker](svg2/painting.md#MarkerProperty), [marker-end](svg2/painting.md#MarkerEndProperty), [marker-mid](svg2/painting.md#MarkerMidProperty), [marker-start](svg2/painting.md#MarkerStartProperty), [mask](css-masking-1.md#propdef-mask), [opacity](css-color-4.md#propdef-opacity), overflow, pointer-events, [shape-rendering](svg2/painting.md#ShapeRenderingProperty), [stop-color](svg2/pservers.md#StopColorProperty), [stop-opacity](svg2/pservers.md#StopOpacityProperty), [stroke](svg2/painting.md#StrokeProperty), [stroke-dasharray](svg2/painting.md#StrokeDasharrayProperty), [stroke-dashoffset](svg2/painting.md#StrokeDashoffsetProperty), [stroke-linecap](svg2/painting.md#StrokeLinecapProperty), [stroke-linejoin](svg2/painting.md#StrokeLinejoinProperty), [stroke-miterlimit](svg2/painting.md#StrokeMiterlimitProperty), [stroke-opacity](svg2/painting.md#StrokeOpacityProperty), [stroke-width](svg2/painting.md#StrokeWidthProperty), [text-anchor](svg2/text.md#TextAnchorProperty), text-decoration, [text-rendering](svg2/painting.md#TextRenderingProperty), unicode-bidi, visibility, word-spacing, writing-mode - [filter primitive attributes](#filter-primitive-attributes) —​[x](#element-attrdef-filter-primitive-x), [y](#element-attrdef-filter-primitive-y), [width](#element-attrdef-filter-primitive-width), [height](#element-attrdef-filter-primitive-height), [result](#element-attrdef-filter-primitive-result) - [class](svg11/styling.md#ClassAttribute) - [style](svg11/styling.md#StyleAttribute) - [baseFrequency](#element-attrdef-feturbulence-basefrequency) - [numOctaves](#element-attrdef-feturbulence-numoctaves) - [seed](#element-attrdef-feturbulence-seed) - [stitchTiles](#element-attrdef-feturbulence-stitchtiles) - [type](#element-attrdef-feturbulence-type) |
| DOM Interfaces: | [SVGFETurbulenceElement](#InterfaceSVGFETurbulenceElement) |

This filter primitive creates an image using the Perlin turbulence function. It allows the synthesis of artificial textures like clouds or marble. For a detailed description the of the Perlin turbulence function, see "Texturing and Modeling" [[TaM]](#biblio-tam). The resulting image will fill the entire [filter primitive subregion](#filter-primitive-subregion) for this filter primitive.

It is possible to create bandwidth-limited noise by synthesizing only one octave.

The C code below shows the exact algorithm used for this filter effect. The [filter primitive subregion](#filter-primitive-subregion) is to be passed as the arguments fTileX, fTileY, fTileWidth and fTileHeight.

For fractalSum, you get a turbFunctionResult that is aimed at a range of -1 to 1 (the actual result might exceed this range in some cases). To convert to a color or alpha value, use the formula `colorValue = (turbFunctionResult + 1) / 2`, then clamp to the range 0 to 1.

For turbulence, you get a turbFunctionResult that is aimed at a range of 0 to 1 (the actual result might exceed this range in some cases). To convert to a color or alpha value, use the formula `colorValue = turbFunctionResult`, then clamp to the range 0 to 1.

The following order is used for applying the pseudo random numbers. An initial seed value is computed based on the [seed](#element-attrdef-feturbulence-seed) attribute. Then the implementation computes the lattice points for R, then continues getting additional pseudo random numbers relative to the last generated pseudo random number and computes the lattice points for G, and so on for B and A.

The generated color and alpha values are in the color space determined by the [color-interpolation-filters](#propdef-color-interpolation-filters) property:

```
/* Produces results in the range [1, 2**31 - 2].
Algorithm is: r = (a * r) mod m
where a = 16807 and m = 2**31 - 1 = 2147483647
See [Park & Miller], CACM vol. 31 no. 10 p. 1195, Oct. 1988
To test: the algorithm should produce the result 1043618065
as the 10,000th generated number if the original seed is 1.
*/
#define RAND_m 2147483647 /* 2**31 - 1 */
#define RAND_a 16807 /* 7**5; primitive root of m */
#define RAND_q 127773 /* m / a */
#define RAND_r 2836 /* m % a */
long setup_seed(long lSeed)
{
  if (lSeed <= 0) lSeed = -(lSeed % (RAND_m - 1)) + 1;
  if (lSeed > RAND_m - 1) lSeed = RAND_m - 1;
  return lSeed;
}
long random(long lSeed)
{
  long result;
  result = RAND_a * (lSeed % RAND_q) - RAND_r * (lSeed / RAND_q);
  if (result <= 0) result += RAND_m;
  return result;
}
#define BSize 0x100
#define BM 0xff
#define PerlinN 0x1000
#define NP 12 /* 2^PerlinN */
#define NM 0xfff
static uLatticeSelector[BSize + BSize + 2];
static double fGradient[4][BSize + BSize + 2][2];
struct StitchInfo
{
  int nWidth; // How much to subtract to wrap for stitching.
  int nHeight;
  int nWrapX; // Minimum value to wrap.
  int nWrapY;
};
static void init(long lSeed)
{
  double s;
  int i, j, k;
  lSeed = setup_seed(lSeed);
  for(k = 0; k < 4; k++)
  {
    for(i = 0; i < BSize; i++)
    {
      uLatticeSelector[i] = i;
      do {
         for (j = 0; j < 2; j++)
           fGradient[k][i][j] = (double)(((lSeed = random(lSeed)) % (BSize + BSize)) - BSize) / BSize;
      } while(fGradient[k][i][0] == 0 && fGradient[k][i][1] == 0);
      s = double(sqrt(fGradient[k][i][0] * fGradient[k][i][0] + fGradient[k][i][1] * fGradient[k][i][1]));
      if (s > 1) {
          i--; // discard the current random vector; try it again.
          continue;
      }
      fGradient[k][i][0] /= s;
      fGradient[k][i][1] /= s;
    }
  }
  while(--i)
  {
    k = uLatticeSelector[i];
    uLatticeSelector[i] = uLatticeSelector[j = (lSeed = random(lSeed)) % BSize];
    uLatticeSelector[j] = k;
  }
  for(i = 0; i < BSize + 2; i++)
  {
    uLatticeSelector[BSize + i] = uLatticeSelector[i];
    for(k = 0; k < 4; k++)
      for(j = 0; j < 2; j++)
        fGradient[k][BSize + i][j] = fGradient[k][i][j];
  }
}
#define s_curve(t) ( t * t * (3. - 2. * t) )
#define lerp(t, a, b) ( a + t * (b - a) )
double noise2(int nColorChannel, double vec[2], StitchInfo *pStitchInfo)
{
  int bx0, bx1, by0, by1, b00, b10, b01, b11;
  double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
  register i, j;
  t = vec[0] + PerlinN;
  bx0 = (int)t;
  bx1 = bx0+1;
  rx0 = t - (int)t;
  rx1 = rx0 - 1.0f;
  t = vec[1] + PerlinN;
  by0 = (int)t;
  by1 = by0+1;
  ry0 = t - (int)t;
  ry1 = ry0 - 1.0f;
  // If stitching, adjust lattice points accordingly.
  if(pStitchInfo != NULL)
  {
    if(bx0 >= pStitchInfo->nWrapX)
      bx0 -= pStitchInfo->nWidth;
    if(bx1 >= pStitchInfo->nWrapX)
      bx1 -= pStitchInfo->nWidth;
    if(by0 >= pStitchInfo->nWrapY)
      by0 -= pStitchInfo->nHeight;
    if(by1 >= pStitchInfo->nWrapY)
      by1 -= pStitchInfo->nHeight;
  }
  bx0 &= BM;
  bx1 &= BM;
  by0 &= BM;
  by1 &= BM;
  i = uLatticeSelector[bx0];
  j = uLatticeSelector[bx1];
  b00 = uLatticeSelector[i + by0];
  b10 = uLatticeSelector[j + by0];
  b01 = uLatticeSelector[i + by1];
  b11 = uLatticeSelector[j + by1];
  sx = double(s_curve(rx0));
  sy = double(s_curve(ry0));
  q = fGradient[nColorChannel][b00]; u = rx0 * q[0] + ry0 * q[1];
  q = fGradient[nColorChannel][b10]; v = rx1 * q[0] + ry0 * q[1];
  a = lerp(sx, u, v);
  q = fGradient[nColorChannel][b01]; u = rx0 * q[0] + ry1 * q[1];
  q = fGradient[nColorChannel][b11]; v = rx1 * q[0] + ry1 * q[1];
  b = lerp(sx, u, v);
  return lerp(sy, a, b);
}
double turbulence(int nColorChannel, double *point, double fBaseFreqX, double fBaseFreqY,
          int nNumOctaves, bool bFractalSum, bool bDoStitching,
          double fTileX, double fTileY, double fTileWidth, double fTileHeight)
{
  StitchInfo stitch;
  StitchInfo *pStitchInfo = NULL; // Not stitching when NULL.
  // Adjust the base frequencies if necessary for stitching.
  if(bDoStitching)
  {
    // When stitching tiled turbulence, the frequencies must be adjusted
    // so that the tile borders will be continuous.
    if(fBaseFreqX != 0.0)
    {
      double fLoFreq = double(floor(fTileWidth * fBaseFreqX)) / fTileWidth;
      double fHiFreq = double(ceil(fTileWidth * fBaseFreqX)) / fTileWidth;
      if(fBaseFreqX / fLoFreq < fHiFreq / fBaseFreqX)
        fBaseFreqX = fLoFreq;
      else
        fBaseFreqX = fHiFreq;
    }
    if(fBaseFreqY != 0.0)
    {
      double fLoFreq = double(floor(fTileHeight * fBaseFreqY)) / fTileHeight;
      double fHiFreq = double(ceil(fTileHeight * fBaseFreqY)) / fTileHeight;
      if(fBaseFreqY / fLoFreq < fHiFreq / fBaseFreqY)
        fBaseFreqY = fLoFreq;
      else
        fBaseFreqY = fHiFreq;
    }
    // Set up initial stitch values.
    pStitchInfo = &stitch;
    stitch.nWidth = int(fTileWidth * fBaseFreqX + 0.5f);
    stitch.nWrapX = fTileX * fBaseFreqX + PerlinN + stitch.nWidth;
    stitch.nHeight = int(fTileHeight * fBaseFreqY + 0.5f);
    stitch.nWrapY = fTileY * fBaseFreqY + PerlinN + stitch.nHeight;
  }
  double fSum = 0.0f;
  double vec[2];
  vec[0] = point[0] * fBaseFreqX;
  vec[1] = point[1] * fBaseFreqY;
  double ratio = 1;
  for(int nOctave = 0; nOctave < nNumOctaves; nOctave++)
  {
    if(bFractalSum)
      fSum += double(noise2(nColorChannel, vec, pStitchInfo) / ratio);
    else
      fSum += double(fabs(noise2(nColorChannel, vec, pStitchInfo)) / ratio);
    vec[0] *= 2;
    vec[1] *= 2;
    ratio *= 2;
    if(pStitchInfo != NULL)
    {
      // Update stitch values. Subtracting PerlinN before the multiplication and
      // adding it afterward simplifies to subtracting it once.
      stitch.nWidth *= 2;
      stitch.nWrapX = 2 * stitch.nWrapX - PerlinN;
      stitch.nHeight *= 2;
      stitch.nWrapY = 2 * stitch.nWrapY - PerlinN;
    }
  }
  return fSum;
}
```

*Attribute definitions:*

***<a id="element-attrdef-feturbulence-basefrequency"></a>`baseFrequency`* = "*[<number-optional-number>](#typedef-number-optional-number)*" **
:

The base frequency (frequencies) parameter(s) for the noise function. If two [<number>](css-values-4.md#number-value)s are provided, the first number represents a base frequency in the X direction and the second value represents a base frequency in the Y direction. If one number is provided, then that value is used for both X and Y.

The [initial value](svg2/types.md#TermInitialValue) for [baseFrequency](#element-attrdef-feturbulence-basefrequency) is 0.

Negative values are unsupported.

Animatable: yes.

***<a id="element-attrdef-feturbulence-numoctaves"></a>`numOctaves`* = "*[<integer>](css-values-4.md#integer-value)*" **
:

The numOctaves parameter for the noise function.

The [initial value](svg2/types.md#TermInitialValue) for [numOctaves](#element-attrdef-feturbulence-numoctaves) is 1.

Negative values are unsupported.

Animatable: yes.

Note: The contribution of each additional octave to the color and alpha values in the final image is one-half of the preceding octave. At some point, the contribution of additional octaves becomes smaller than the color resolution for a given color depth. UAs may clamp the specified value for numOctaves during the processing depending on the supported color depth. (For example: For a color depth of 8 bits per channel, the UA may clamp the value of numOctaves to 9.)

***<a id="element-attrdef-feturbulence-seed"></a>`seed`* = "*[<number>](css-values-4.md#number-value)*" **
:

The starting number for the pseudo random number generator.

The [initial value](svg2/types.md#TermInitialValue) for [seed](#element-attrdef-feturbulence-seed) is 0.

When the seed number is handed over to the algorithm above it must first be truncated, i.e. rounded to the closest integer value towards zero.

Animatable: yes.

***<a id="element-attrdef-feturbulence-stitchtiles"></a>`stitchTiles`* = "*stitch | noStitch*" **
:

If `stitchTiles="noStitch"`, no attempt is made to achieve smooth transitions at the border of tiles which contain a turbulence function. Sometimes the result will show clear discontinuities at the tile borders.

If `stitchTiles="stitch"`, then the user agent will automatically adjust baseFrequency-x and baseFrequency-y values such that the [feTurbulence](#elementdef-feturbulence) node’s width and height (i.e., the width and height of the current subregion) contains an integral number of the Perlin tile width and height for the first octave. The baseFrequency will be adjusted up or down depending on which way has the smallest relative (not absolute) change as follows: Given the frequency, calculate `lowFreq=floor(width*frequency)/width` and `hiFreq=ceil(width*frequency)/width`. If frequency/lowFreq < hiFreq/frequency then use lowFreq, else use hiFreq. While generating turbulence values, generate lattice vectors as normal for Perlin Noise, except for those lattice points that lie on the right or bottom edges of the active area (the size of the resulting tile). In those cases, copy the lattice vector from the opposite edge of the active area.

The [initial value](svg2/types.md#TermInitialValue) for [stitchTiles](#element-attrdef-feturbulence-stitchtiles) is noStitch.

Animatable: yes.

***<a id="element-attrdef-feturbulence-type"></a>`type`* = "*fractalNoise | turbulence*" **
:

Indicates whether the filter primitive should perform a noise or turbulence function.

The [initial value](svg2/types.md#TermInitialValue) for [type](#element-attrdef-feturbulence-type) is turbulence.

Animatable: yes.

<a id="example-a47584c3"></a>

```
<svg width="450px" height="325px" viewBox="0 0 450 325"
     xmlns="http://www.w3.org/2000/svg">
  <title>Example feTurbulence - Examples of feTurbulence operations</title>
  <desc>Six rectangular areas showing the effects of
        various parameter settings for feTurbulence.</desc>
  <g  font-family="Verdana" text-anchor="middle" font-size="10" >
    <defs>
      <filter id="Turb1" filterUnits="objectBoundingBox"
              x="0%" y="0%" width="100%" height="100%">
        <feTurbulence type="turbulence" baseFrequency="0.05" numOctaves="2"/>
      </filter>
      <filter id="Turb2" filterUnits="objectBoundingBox"
              x="0%" y="0%" width="100%" height="100%">
        <feTurbulence type="turbulence" baseFrequency="0.1" numOctaves="2"/>
      </filter>
      <filter id="Turb3" filterUnits="objectBoundingBox"
              x="0%" y="0%" width="100%" height="100%">
        <feTurbulence type="turbulence" baseFrequency="0.05" numOctaves="8"/>
      </filter>
      <filter id="Turb4" filterUnits="objectBoundingBox"
              x="0%" y="0%" width="100%" height="100%">
        <feTurbulence type="fractalNoise" baseFrequency="0.1" numOctaves="4"/>
      </filter>
      <filter id="Turb5" filterUnits="objectBoundingBox"
              x="0%" y="0%" width="100%" height="100%">
        <feTurbulence type="fractalNoise" baseFrequency="0.4" numOctaves="4"/>
      </filter>
      <filter id="Turb6" filterUnits="objectBoundingBox"
              x="0%" y="0%" width="100%" height="100%">
        <feTurbulence type="fractalNoise" baseFrequency="0.1" numOctaves="1"/>
      </filter>
    </defs>

    <rect x="1" y="1" width="448" height="323"
          fill="none" stroke="blue" stroke-width="1"  />

    <rect x="25" y="25" width="100" height="75" filter="url(#Turb1)"  />
    <text x="75" y="117">type=turbulence</text>
    <text x="75" y="129">baseFrequency=0.05</text>
    <text x="75" y="141">numOctaves=2</text>

    <rect x="175" y="25" width="100" height="75" filter="url(#Turb2)"  />
    <text x="225" y="117">type=turbulence</text>
    <text x="225" y="129">baseFrequency=0.1</text>
    <text x="225" y="141">numOctaves=2</text>

    <rect x="325" y="25" width="100" height="75" filter="url(#Turb3)"  />
    <text x="375" y="117">type=turbulence</text>
    <text x="375" y="129">baseFrequency=0.05</text>
    <text x="375" y="141">numOctaves=8</text>

    <rect x="25" y="180" width="100" height="75" filter="url(#Turb4)"  />
    <text x="75" y="272">type=fractalNoise</text>
    <text x="75" y="284">baseFrequency=0.1</text>
    <text x="75" y="296">numOctaves=4</text>

    <rect x="175" y="180" width="100" height="75" filter="url(#Turb5)"  />
    <text x="225" y="272">type=fractalNoise</text>
    <text x="225" y="284">baseFrequency=0.4</text>
    <text x="225" y="296">numOctaves=4</text>

    <rect x="325" y="180" width="100" height="75" filter="url(#Turb6)"  />
    <text x="375" y="272">type=fractalNoise</text>
    <text x="375" y="284">baseFrequency=0.1</text>
    <text x="375" y="296">numOctaves=1</text>
  </g>
</svg>
```

[image: Example of feTurbulence]

Example of feTurbulence

[View this example as SVG](https://drafts.csswg.org/filter-effects-1/examples/feTurbulence.svg)

## <a id="ColorInterpolationFiltersProperty"></a>10. The [color-interpolation-filters](#propdef-color-interpolation-filters) property

The description of the [color-interpolation-filters](#propdef-color-interpolation-filters) property is as follows:

| Name: | *<a id="propdef-color-interpolation-filters"></a>color-interpolation-filters* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | auto [\|](css-values-4.md#comb-one) sRGB \| linearRGB |
| [Initial:](css-cascade-5.md#initial-values) | linearRGB |
| [Applies to:](css-cascade-5.md#applies-to) | All [filter primitives](#filter-primitive) |
| [Inherited:](css-cascade-5.md#inherited-property) | yes |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

***<a id="valdef-color-interpolation-filters-auto"></a>auto* **
:

Indicates that the user agent can choose either the [sRGB](#valdef-color-interpolation-filters-srgb) or [linearRGB](#valdef-color-interpolation-filters-linearrgb) spaces for filter effects color operations. This option indicates that the author doesn’t require that color operations occur in a particular color space.

***<a id="valdef-color-interpolation-filters-srgb"></a>sRGB* **
:

Indicates that filter effects color operations should occur in the gamma-encoded [sRGB](css-color-4.md#sRGB-space) color space.

***<a id="valdef-color-interpolation-filters-linearrgb"></a>linearRGB* **
:

Indicates that filter effects color operations should occur in the [linear-light sRGB](css-color-4.md#sRGB-linear-space) color space.

The [color-interpolation-filters](#propdef-color-interpolation-filters) property specifies the color space for imaging operations performed via filter effects.

Note: The [color-interpolation-filters](#propdef-color-interpolation-filters) property just has an affect on filter operations. Therefore, it has no effect on filter primitives like [feOffset](#elementdef-feoffset), [feImage](#elementdef-feimage), [feTile](#elementdef-fetile) or [feFlood](#elementdef-feflood).

Note: The [color-interpolation-filters](#propdef-color-interpolation-filters) has a different initial value than [color-interpolation](svg2/painting.md#ColorInterpolationProperty). color-interpolation-filters has an initial value of [linearRGB](#valdef-color-interpolation-filters-linearrgb), where as color-interpolation has an initial value of [sRGB](#valdef-color-interpolation-filters-srgb). Thus, in the default case, filter effects operations occur in the linearRGB color space, whereas all other color interpolations occur by default in the sRGB color space.

Note: The [color-interpolation-filters](#propdef-color-interpolation-filters) property has no affect on [filter functions](#filter-function), which operate in the sRGB color space.

The [color-interpolation-filters](#propdef-color-interpolation-filters) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

## <a id="LightSourceDefinitions"></a>11. Light source elements and properties

### <a id="LightSourceIntro"></a>11.1. Introduction

The following sections define the elements that define a *<a id="light-source"></a>light source*, [feDistantLight](#elementdef-fedistantlight), [fePointLight](#elementdef-fepointlight) and [feSpotLight](#elementdef-fespotlight), and property [lighting-color](#propdef-lighting-color), which defines the color of the light.

### <a id="feDistantLightElement"></a>11.2. Light source [feDistantLight](#elementdef-fedistantlight)

| Name: | *<a id="elementdef-fedistantlight"></a>`feDistantLight`* |
| --- | --- |
| Categories: | [light source](#light-source) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [azimuth](#element-attrdef-fedistantlight-azimuth) - [elevation](#element-attrdef-fedistantlight-elevation) |
| DOM Interfaces: | [SVGFEDistantLightElement](#InterfaceSVGFEDistantLightElement) |

*Attribute definitions:*

***<a id="element-attrdef-fedistantlight-azimuth"></a>`azimuth`* = "*[<number>](css-values-4.md#number-value)*" **
:

Direction angle for the light source on the XY plane (clockwise), in degrees from the x axis.

The [initial value](svg2/types.md#TermInitialValue) for [azimuth](#element-attrdef-fedistantlight-azimuth) is 0.

Animatable: yes.

***<a id="element-attrdef-fedistantlight-elevation"></a>`elevation`* = "*[<number>](css-values-4.md#number-value)*" **
:

Direction angle for the light source from the XY plane towards the Z-axis, in degrees. Note that the positive Z-axis points towards the viewer.

The [initial value](svg2/types.md#TermInitialValue) for [elevation](#element-attrdef-fedistantlight-elevation) is 0.

Animatable: yes.

The following diagram illustrates the angles which [azimuth](#element-attrdef-fedistantlight-azimuth) and [elevation](#element-attrdef-fedistantlight-elevation) represent in an XYZ coordinate system.

[image: Angles which azimuth and elevation represent]

Angles which azimuth and elevation represent

### <a id="fePointLightElement"></a>11.3. Light source [fePointLight](#elementdef-fepointlight)

| Name: | *<a id="elementdef-fepointlight"></a>`fePointLight`* |
| --- | --- |
| Categories: | [light source](#light-source) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [x](#element-attrdef-fepointlight-x) - [y](#element-attrdef-fepointlight-y) - [z](#element-attrdef-fepointlight-z) |
| DOM Interfaces: | [SVGFEPointLightElement](#InterfaceSVGFEPointLightElement) |

*Attribute definitions:*

***<a id="element-attrdef-fepointlight-x"></a>`x`* = "*[<number>](css-values-4.md#number-value)*" **
:

X location for the light source in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

The [initial value](svg2/types.md#TermInitialValue) for [x](#element-attrdef-fepointlight-x) is 0.

Animatable: yes.

***<a id="element-attrdef-fepointlight-y"></a>`y`* = "*[<number>](css-values-4.md#number-value)*" **
:

Y location for the light source in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

The [initial value](svg2/types.md#TermInitialValue) for [y](#element-attrdef-fepointlight-y) is 0.

Animatable: yes.

***<a id="element-attrdef-fepointlight-z"></a>`z`* = "*[<number>](css-values-4.md#number-value)*" **
:

Z location for the light source in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element, assuming that, in the initial [local coordinate system](css-transforms-1.md#local-coordinate-system) , the positive Z-axis comes out towards the person viewing the content and assuming that one unit along the Z-axis equals [one unit in X and Y](svg11/coords.md#Units_viewport_percentage).

The [initial value](svg2/types.md#TermInitialValue) for [z](#element-attrdef-fepointlight-z) is 0.

Animatable: yes.

### <a id="feSpotLightElement"></a>11.4. Light source [feSpotLight](#elementdef-fespotlight)

| Name: | *<a id="elementdef-fespotlight"></a>`feSpotLight`* |
| --- | --- |
| Categories: | [light source](#light-source) |
| Content model: | Any number of [descriptive elements](svg2/struct.md#TermDescriptiveElement), animate, [script](svg2/interact.md#elementdef-script), set elements, in any order. |
| Attributes: | - [core attributes](svg11/intro.md#TermCoreAttributes) — [id](svg11/struct.md#IDAttribute), [xml:base](svg11/struct.md#XMLBaseAttribute), [xml:lang](svg11/struct.md#XMLLangAttribute), [xml:space](svg11/struct.md#XMLSpaceAttribute) - [x](#element-attrdef-fespotlight-x) - [y](#element-attrdef-fespotlight-y) - [z](#element-attrdef-fespotlight-z) - [pointsAtX](#element-attrdef-fespotlight-pointsatx) - [pointsAtY](#element-attrdef-fespotlight-pointsaty) - [pointsAtZ](#element-attrdef-fespotlight-pointsatz) - [specularExponent](#element-attrdef-fespotlight-specularexponent) - [limitingConeAngle](#element-attrdef-fespotlight-limitingconeangle) |
| DOM Interfaces: | [SVGFESpotLightElement](#InterfaceSVGFESpotLightElement) |

*Attribute definitions:*

***<a id="element-attrdef-fespotlight-x"></a>`x`* = "*[<number>](css-values-4.md#number-value)*" **
:

X location for the light source in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

The [initial value](svg2/types.md#TermInitialValue) for [x](#element-attrdef-fespotlight-x) is 0.

Animatable: yes.

***<a id="element-attrdef-fespotlight-y"></a>`y`* = "*[<number>](css-values-4.md#number-value)*" **
:

Y location for the light source in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element.

The [initial value](svg2/types.md#TermInitialValue) for [y](#element-attrdef-fespotlight-y) is 0.

Animatable: yes.

***<a id="element-attrdef-fespotlight-z"></a>`z`* = "*[<number>](css-values-4.md#number-value)*" **
:

Z location for the light source in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element, assuming that, in the initial [local coordinate system](css-transforms-1.md#local-coordinate-system), the positive Z-axis comes out towards the person viewing the content and assuming that one unit along the Z-axis equals [one unit in X and Y](svg11/coords.md#Units_viewport_percentage).

The [initial value](svg2/types.md#TermInitialValue) for [z](#element-attrdef-fespotlight-z) is 0.

Animatable: yes.

***<a id="element-attrdef-fespotlight-pointsatx"></a>`pointsAtX`* = "*[<number>](css-values-4.md#number-value)*" **
:

X location in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element of the point at which the light source is pointing.

The [initial value](svg2/types.md#TermInitialValue) for [pointsAtX](#element-attrdef-fespotlight-pointsatx) is 0.

Animatable: yes.

***<a id="element-attrdef-fespotlight-pointsaty"></a>`pointsAtY`* = "*[<number>](css-values-4.md#number-value)*" **
:

Y location in the coordinate system established by attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element of the point at which the light source is pointing.

The [initial value](svg2/types.md#TermInitialValue) for [pointsAtY](#element-attrdef-fespotlight-pointsaty) is 0.

Animatable: yes.

***<a id="element-attrdef-fespotlight-pointsatz"></a>`pointsAtZ`* = "*[<number>](css-values-4.md#number-value)*" **
:

Z location in the coordinate system established by the attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the [filter](#elementdef-filter) element of the point at which the light source is pointing, assuming that, in the initial [local coordinate system](css-transforms-1.md#local-coordinate-system), the positive Z-axis comes out towards the person viewing the content and assuming that one unit along the Z-axis equals [one unit in X and Y](svg11/coords.md#Units_viewport_percentage).

The [initial value](svg2/types.md#TermInitialValue) for [pointsAtZ](#element-attrdef-fespotlight-pointsatz) is 0.

Animatable: yes.

***<a id="element-attrdef-fespotlight-specularexponent"></a>`specularExponent`* = "*[<number>](css-values-4.md#number-value)*" **
:

Exponent value controlling the focus for the light source.

The [initial value](svg2/types.md#TermInitialValue) for [specularExponent](#element-attrdef-fespotlight-specularexponent) is 1.

See section [Filter primitive <feDiffuseLighting>](#feDiffuseLightingElement) for how to use [specularExponent](#element-attrdef-fespotlight-specularexponent).

Note: [specularExponent](#element-attrdef-fespotlight-specularexponent) for [feSpotLight](#elementdef-fespotlight) serves a different use case than [specularExponent](#element-attrdef-fespecularlighting-specularexponent) for [feSpecularLighting](#elementdef-fespecularlighting).

Animatable: yes.

***<a id="element-attrdef-fespotlight-limitingconeangle"></a>`limitingConeAngle`* = "*[<number>](css-values-4.md#number-value)*" **
:

A limiting cone which restricts the region where the light is projected. No light is projected outside the cone. [limitingConeAngle](#element-attrdef-fespotlight-limitingconeangle) represents the angle in degrees between the spot light axis (i.e. the axis between the light source and the point to which it is pointing at) and the spot light cone. <a id="assert_userAgentLightingConeSmoothing"></a>User agents should apply a smoothing technique such as anti-aliasing at the boundary of the cone.

If no value is specified, then no limiting cone will be applied.

Animatable: yes.

### <a id="LightingColorProperty"></a>11.5. The [lighting-color](#propdef-lighting-color) property

| Name: | *<a id="propdef-lighting-color"></a>lighting-color* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | <color> |
| [Initial:](css-cascade-5.md#initial-values) | white |
| [Applies to:](css-cascade-5.md#applies-to) | [feDiffuseLighting](#elementdef-fediffuselighting) and [feSpecularLighting](#elementdef-fespecularlighting) elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | by computed value |
| Media: | visual |

The [lighting-color](#propdef-lighting-color) property defines the color of the light source for [filter primitives](#filter-primitive) [feDiffuseLighting](#elementdef-fediffuselighting) and [feSpecularLighting](#elementdef-fespecularlighting).

The [lighting-color](#propdef-lighting-color) property is a [presentation attribute](svg11/intro.md#TermPresentationAttribute) for SVG elements.

## <a id="FilterCSSImageValue"></a>12. Filter CSS [<image>](css-images-3.md#typedef-image) values

CSS [<image>](css-images-3.md#typedef-image) values can be filtered with the filter functions specified for the CSS [filter](#propdef-filter) property. This specification introduces the <image> [filter()](#funcdef-filter) function with the following syntax:

```
<a id="funcdef-filter"></a>filter() = filter( [ <image> | <string> ], <filter-value-list> )
```

The [filter()](#funcdef-filter) function takes two arguments. The first argument is an [<image>](css-images-3.md#typedef-image). The second is a filter function list as specified for the CSS [filter](#propdef-filter) property. The function takes the <image> parameter and applies the filter rules, returning a processing image. Filter- and filter effect regions are sized according to the [concrete object size](css-images-3.md#concrete-object-size) of the input <image>.

Note: Since the dimension and origin of the original image must be preserved, some filter effects like [drop-shadow()](#funcdef-filter-drop-shadow) on a fully opaque image may not have any affect.

For the [blur()](#funcdef-filter-blur) function the [edgeMode](#element-attrdef-fegaussianblur-edgemode) attribute on the [feGaussianBlur](#elementdef-fegaussianblur) element is set to duplicate. This produces more pleasant results on the edges of the filtered input image.

### <a id="interpolating-filter-image"></a>12.1. Interpolating filter()

If both the starting and ending image are [filter()](#funcdef-filter)s which may only differ by their used filter functions, they must be interpolated by interpolating their filter function lists as described in section [Interpolation of Filters](#interpolation-of-filters). Otherwise, they must be interpolated as generic [<image>](css-images-3.md#typedef-image)s. If the filter function interpolation can not be performed, the images must be interpolated as generic <image>s.

## <a id="ShorthandEquivalents"></a>13. Shorthands defined in terms of the [filter](#elementdef-filter) element

### <a id="FilterPrimitiveRepresentation"></a>13.1. Filter primitive representation

Below are the equivalents for each of the filter functions expressed in terms of the 'filter element' element. The parameters from the function are labeled with brackets in the following style: [amount]. In the case of parameters that are percentage values, they are converted to real numbers.

#### <a id="grayscaleEquivalent"></a>13.1.1. grayscale

```
<filter id="grayscale">
  <feColorMatrix type="matrix"
             values="
    (0.2126 + 0.7874 * [1 - amount]) (0.7152 - 0.7152  * [1 - amount]) (0.0722 - 0.0722 * [1 - amount]) 0 0
    (0.2126 - 0.2126 * [1 - amount]) (0.7152 + 0.2848  * [1 - amount]) (0.0722 - 0.0722 * [1 - amount]) 0 0
    (0.2126 - 0.2126 * [1 - amount]) (0.7152 - 0.7152  * [1 - amount]) (0.0722 + 0.9278 * [1 - amount]) 0 0
    0 0 0 1 0"/>
</filter>
```

#### <a id="sepiaEquivalent"></a>13.1.2. sepia

```
<filter id="sepia">
  <feColorMatrix type="matrix"
             values="
    (0.393 + 0.607 * [1 - amount]) (0.769 - 0.769 * [1 - amount]) (0.189 - 0.189 * [1 - amount]) 0 0
    (0.349 - 0.349 * [1 - amount]) (0.686 + 0.314 * [1 - amount]) (0.168 - 0.168 * [1 - amount]) 0 0
    (0.272 - 0.272 * [1 - amount]) (0.534 - 0.534 * [1 - amount]) (0.131 + 0.869 * [1 - amount]) 0 0
    0 0 0 1 0"/>
</filter>
```

#### <a id="saturateEquivalent"></a>13.1.3. saturate

```
<filter id="saturate">
  <feColorMatrix type="saturate" values="[amount]"/>
</filter>
```

#### <a id="huerotateEquivalent"></a>13.1.4. hue-rotate

```
<filter id="hue-rotate">
  <feColorMatrix type="hueRotate" values="[angle]"/>
</filter>
```

#### <a id="invertEquivalent"></a>13.1.5. invert

```
<filter id="invert">
  <feComponentTransfer>
      <feFuncR type="table" tableValues="[amount] (1 - [amount])"/>
      <feFuncG type="table" tableValues="[amount] (1 - [amount])"/>
      <feFuncB type="table" tableValues="[amount] (1 - [amount])"/>
  </feComponentTransfer>
</filter>
```

#### <a id="opacityEquivalent"></a>13.1.6. opacity

```
<filter id="opacity">
  <feComponentTransfer>
      <feFuncA type="table" tableValues="0 [amount]"/>
  </feComponentTransfer>
</filter>
```

#### <a id="brightnessEquivalent"></a>13.1.7. brightness

```
<filter id="brightness">
  <feComponentTransfer>
      <feFuncR type="linear" slope="[amount]"/>
      <feFuncG type="linear" slope="[amount]"/>
      <feFuncB type="linear" slope="[amount]"/>
  </feComponentTransfer>
</filter>
```

#### <a id="contrastEquivalent"></a>13.1.8. contrast

```
<filter id="contrast">
  <feComponentTransfer>
      <feFuncR type="linear" slope="[amount]" intercept="-(0.5 * [amount]) + 0.5"/>
      <feFuncG type="linear" slope="[amount]" intercept="-(0.5 * [amount]) + 0.5"/>
      <feFuncB type="linear" slope="[amount]" intercept="-(0.5 * [amount]) + 0.5"/>
  </feComponentTransfer>
</filter>
```

#### <a id="blurEquivalent"></a>13.1.9. blur

```
<filter id="blur">
  <feGaussianBlur stdDeviation="[radius radius]" edgeMode="[edge mode]" >
</filter>
```

Where edge mode computes to none for the [filter](#propdef-filter) property and to duplicate for the CSS Image [filter()](#funcdef-filter) function.

Note: The [blur()](#funcdef-filter-blur) function may increase the UA defined filter region. See [Filter region for shorthands](#filter-region-for-shorthands).

#### <a id="dropshadowEquivalent"></a>13.1.10. drop-shadow

```
<filter id="drop-shadow">
  <feGaussianBlur in="[alpha-channel-of-input]" stdDeviation="[radius]"/>
  <feOffset dx="[offset-x]" dy="[offset-y]" result="offsetblur"/>
  <feFlood flood-color="[color]"/>
  <feComposite in2="offsetblur" operator="in"/>
  <feMerge>
    <feMergeNode/>
    <feMergeNode in="[input-image]"/>
  </feMerge>
</filter>
```

Note: The [drop-shadow()](#funcdef-filter-drop-shadow) function may increase the UA defined filter region. See [Filter region for shorthands](#filter-region-for-shorthands).

### <a id="filter-region-for-shorthands"></a>13.2. Filter region for shorthands

All shorthand filters implemented with filter primitives in the previous subsection must have a UA defined [filter region](#filter-region). The filter region must cover the visual content of an element including overflowing content, graphical control elements such as scrollbars, [border](css-backgrounds-3.md#propdef-border)/[border-image](css-backgrounds-3.md#propdef-border-image), [box-shadow](css-backgrounds-3.md#propdef-box-shadow), text-shadow and outline. Furthermore, if a shorthand filter expands this visible area like it is the case for [blur()](#funcdef-filter-blur) or [drop-shadow()](#funcdef-filter-drop-shadow) the filter region must cover this area as well.

Note: For the handling of [filter sources](#FilterElement) see section [Filter region](#FilterEffectsRegion).

## <a id="animation-of-filters"></a>14. Animation of Filters

### <a id="interpolation-of-filters"></a>14.1. Interpolation of Filter Function Lists

For interpolation between one filter and a second, the steps corresponding to the first matching condition in the following list must be run:

**<a id="from-to-animation-equal"></a>If both filters have a [<filter-value-list>](#typedef-filter-value-list) of same length without [<url>](#typedef-filter-url) and for each [<filter-function>](#typedef-filter-function) for which there is a corresponding item in each list **
: Interpolate each [<filter-function>](#typedef-filter-function) pair following the rules in section [Interpolation of Filter Functions](#interpolation-of-filter-functions).
**<a id="from-to-animation-unequal"></a>If both filters have a [<filter-value-list>](#typedef-filter-value-list) of different length without [<url>](#typedef-filter-url) and for each [<filter-function>](#typedef-filter-function) for which there is a corresponding item in each list **
:

1. Append the missing equivalent [<filter-function>](#typedef-filter-function)s from the longer list to the end of the shorter list. The new added <filter-function>s must be initialized to their initial values for interpolation.
2. Interpolate each [<filter-function>](#typedef-filter-function) pair following the rules in section [Interpolation of Filter Functions](#interpolation-of-filter-functions).

**<a id="interpolation-none-filter-functions"></a>If one filter is none and the other is a [<filter-value-list>](#typedef-filter-value-list) without [<url>](#typedef-filter-url) **
:

1. Replace none with the corresponding [<filter-value-list>](#typedef-filter-value-list) of the other filter. The new [<filter-function>](#typedef-filter-function)s must be initialized to their initial values for interpolation.
2. Interpolate each [<filter-function>](#typedef-filter-function) pair following the rules in section [Interpolation of Filter Functions](#interpolation-of-filter-functions).

**<a id="no-interpolation"></a>Otherwise **
: Use discrete interpolation.
<a id="issue-f7e128e9"></a> Compute distance of filter functions. [Issue #91]

### <a id="addition"></a>14.2. Addition

It is possible to combine independent animations of [<filter-value-list>](#typedef-filter-value-list)s [[SVG11]](#biblio-svg11).

Given two filter values representing an base value (*base filter list*) and a value to add (*added filter list*), returns the concatenation of the the two lists: ‘*base filter list* *added filter list*’.

<a id="example-39a9a3b0"></a> The following SVG animation has two animate elements animating [filter](#propdef-filter) property of the [rect](svg2/shapes.md#elementdef-rect) element. Both specified animations are additive and have a duration of *10s*.

```
<rect width="200px" filter="none" ...>
  <animate attributeName="filter" from="blur(0px)" to="blur(10px)" dur="10s"
           additive="sum"/>
  <animate attributeName="filter" from="sepia(0)" to="sepia(1)" dur="10s"
           additive="sum"/>
</rect>
```

After *5s*, the [used value](css-cascade-5.md#used-value) of [filter](#propdef-filter) is blur(5px) sepia(0.5).

### <a id="accumulation"></a>14.3. Accumulation

Accumulation of [<filter-value-list>](#typedef-filter-value-list)s follows the same matching and extending rules as [interpolation](#interpolation-of-filters), falling back to replace behavior if the lists do not match. However instead of interpolating the matching [<filter-function>](#typedef-filter-function) pairs, their arguments are arithmetically added together - except in the case of <filter-function>s whose initial value for interpolation is 1, which combine using one-based addition:

*V_result* = *V_a* + *V_b* - 1

## <a id="privacy"></a>15. Privacy Considerations

### <a id="tainted-filter-primitives"></a>15.1. Tainted Filter Primitives

It is important that the timing of any filter operation is independent of pixel values derived from the filtered content or other sources potentially containing privacy-sensitive information.

The following [filter primitives](#filter-primitive) may have access to pixel values that potentially contain privacy-sensitive information, either from the filtered object itself or other sources such as CSS styling. These primitives must be flagged as "tainted".

1.
[feFlood](#elementdef-feflood) when the [specified value](css-cascade-5.md#specified-value) of the [flood-color](#propdef-flood-color) property computes to currentColor,
2.
[feDropShadow](#elementdef-fedropshadow) when the [specified value](css-cascade-5.md#specified-value) value of the [flood-color](#propdef-flood-color) property computes to currentColor,
3.
[feDiffuseLighting](#elementdef-fediffuselighting), when the [specified value](css-cascade-5.md#specified-value) value of the [lighting-color](#propdef-lighting-color) property computes to currentColor
4.
[feSpecularLighting](#elementdef-fespecularlighting) when the [specified value](css-cascade-5.md#specified-value) value of the [lighting-color](#propdef-lighting-color) property computes to currentColor,
5.
[feImage](#elementdef-feimage), when the [<url>](#typedef-filter-url) reference points to an element or fetches a resource with the fetching mode *No-CORS* and
6.
the filter primitives: [SourceGraphic](#attr-valuedef-in-sourcegraphic), [SourceAlpha](#attr-valuedef-in-sourcealpha), [BackgroundImage](#attr-valuedef-in-backgroundimage), [BackgroundAlpha](#attr-valuedef-in-backgroundalpha), [FillPaint](#attr-valuedef-in-fillpaint) and [StrokePaint](#attr-valuedef-in-strokepaint).

[feFlood](#elementdef-feflood), [feDropShadow](#elementdef-fedropshadow), [feDiffuseLighting](#elementdef-fediffuselighting) and [feSpecularLighting](#elementdef-fespecularlighting) are primitives with one or more CSS properties that take <color> as property value. <color> consists of (amongst others) the currentColor keyword. The [used value](css-cascade-5.md#used-value) for currentColor derives from the [color](css-color-4.md#propdef-color) property. Since color can be set by the [:visited](selectors-4.md#visited-pseudo) pseudo selector, it potentially contains privacy-sensitive information and therefore these primitives must be marked as tainted.

[feImage](#elementdef-feimage) can reference cross-domain images as well as document fragments such as SVG [graphics elements](svg2/struct.md#graphics-element). These references potentially contain privacy-sensitive information and therefore the primitive must be marked as tainted.

The filter primitives [SourceGraphic](#attr-valuedef-in-sourcegraphic), [SourceAlpha](#attr-valuedef-in-sourcealpha), [BackgroundImage](#attr-valuedef-in-backgroundimage), [BackgroundAlpha](#attr-valuedef-in-backgroundalpha), [FillPaint](#attr-valuedef-in-fillpaint) and [StrokePaint](#attr-valuedef-in-strokepaint) either reference document fragments such as SVG [graphics elements](svg2/struct.md#graphics-element) or style information that may derive directly or indirectly from the [color](css-color-4.md#propdef-color) property. Therefore these primitives must be marked as tainted.

Every [filter primitive](#filter-primitive) that has a "tainted" flagged filter primitive as input must be flagged as "tainted" as well.

Filter operations must be implemented in such a way that they always take the same amount of time regardless of the pixel values if one of the input filter primitives is flagged as "tainted".

Note: This specification aggravates the restrictions to filter primitives based on implementation feedback from user agents.

### <a id="fedisplacemnentmap-restrictions"></a>15.2. [feDisplacementMap](#elementdef-fedisplacementmap) Restrictions

If [feDisplacementMap](#elementdef-fedisplacementmap) has a "tainted" flagged filter primitive as input and this input filter primitive is used as displacement map (referenced by [in2](#element-attrdef-fedisplacementmap-in2)), then feDisplacementMap must not proceed with the filter operation and acts as a [pass through filter](#pass-through-filter).

### <a id="origin-restrictions"></a>15.3. Origin Restrictions

User agents must use the CORS request defined by the [[Fetch]](#biblio-fetch) specification for the [filter](#propdef-filter) property. When fetching, user agents must use "Anonymous" mode, set the referrer source to the stylesheet’s URL and set the origin to the URL of the containing document. If this results in network errors, the effect is as if the value none had been specified.

### <a id="timing-attack"></a>15.4. Timing Attacks

If any of the above rules are not followed, an attacker could infer information and mount a timing attack.

A timing attack is a method of obtaining information about content that is otherwise protected, based on studying the amount of time it takes for an operation to occur. If, for example, red pixels took longer to draw than green pixels, one might be able to reconstruct a rough image of the element being rendered, without ever having access to the content of the element. Security studies show that timing differences on arithmetic operations can be caused by the hardware architecture or compiler [[ArTD]](#biblio-artd).

### <a id="clickjacking-attack"></a>15.5. Clickjacking Attacks

User agents should not apply SVG reference filters (e.g., [filter](#elementdef-filter) or backdrop-filter) to pixel data from cross-origin/restricted iframes or web plugins. If this is permitted, it allows the manipulation of otherwise inaccessable frames, potentially causing users to take actions they might not otherwise. See the Clickjacking section of HTML Spec.

## <a id="security"></a>16. Security Considerations

Besides the security implications of the privacy issues noted above, no additional security implications are noted for this spec.

## <a id="AccessBackgroundImage"></a>Appendix A: The deprecated enable-background property

SVG 1.1 introduced the [enable-background](svg11/filters.md#EnableBackgroundProperty) property [[SVG11]](#biblio-svg11). The property defined the back drop under the [filter region](#filter-region) at the time that the [filter](#elementdef-filter) element was invoked. The concept defined by this property was identified to be incompatible with the model of stacking context in CSS at the time writing this specification. UAs can choose to implement the [enable-background](svg11/filters.md#EnableBackgroundProperty) property as defined in SVG 1.1 but will not be compatible to this specification or to CSS Compositing and Blending [[COMPOSITING-1]](#biblio-compositing-1).

This specification does not support the [enable-background](svg11/filters.md#EnableBackgroundProperty) property. UAs must support the isolation property instead [[COMPOSITING-1]](#biblio-compositing-1).

## <a id="DOMInterfaces"></a>Appendix B: DOM interfaces

### <a id="InterfaceSVGFilterElement"></a>Interface SVGFilterElement

The *<a id="svgfilterelement"></a>`SVGFilterElement`* interface corresponds to the [filter](#elementdef-filter) element.

```
[Exposed=Window]
interface SVGFilterElement : SVGElement {
  readonly attribute SVGAnimatedEnumeration filterUnits;
  readonly attribute SVGAnimatedEnumeration primitiveUnits;
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
};
```

**Attributes: **
:

***<a id="dom-svgfilterelement-filterunits"></a>`filterUnits`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [filterUnits](#element-attrdef-filter-filterunits) on the given [filter](#elementdef-filter) element. Takes one of the constants defined in [SVGUnitTypes](svg2/types.md#svgunittypes).

***<a id="dom-svgfilterelement-primitiveunits"></a>`primitiveUnits`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [primitiveUnits](#element-attrdef-filter-primitiveunits) on the given [filter](#elementdef-filter) element. Takes one of the constants defined in [SVGUnitTypes](svg2/types.md#svgunittypes).

***<a id="dom-svgfilterelement-x"></a>`x`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [x](#element-attrdef-filter-x) on the given [filter](#elementdef-filter) element.

***<a id="dom-svgfilterelement-y"></a>`y`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [y](#element-attrdef-filter-y) on the given [filter](#elementdef-filter) element.

***<a id="dom-svgfilterelement-width"></a>`width`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [width](#element-attrdef-filter-width) on the given [filter](#elementdef-filter) element.

***<a id="dom-svgfilterelement-height"></a>`height`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [height](#element-attrdef-filter-height) on the given [filter](#elementdef-filter) element.

### <a id="InterfaceSVGFilterPrimitiveStandardAttributes"></a>Interface SVGFilterPrimitiveStandardAttributes

```
interface mixin <a id="svgfilterprimitivestandardattributes"></a>SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
  readonly attribute SVGAnimatedString result;
};
```

**Attributes: **
:

***<a id="dom-svgfilterprimitivestandardattributes-x"></a>`x`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [x](#element-attrdef-filter-primitive-x) on the given element.

***<a id="dom-svgfilterprimitivestandardattributes-y"></a>`y`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [y](#element-attrdef-filter-primitive-y) on the given element.

***<a id="dom-svgfilterprimitivestandardattributes-width"></a>`width`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [width](#element-attrdef-filter-primitive-width) on the given element.

***<a id="dom-svgfilterprimitivestandardattributes-height"></a>`height`*, of type [SVGAnimatedLength](svg2/types.md#svganimatedlength), readonly **
:

Corresponds to attribute [height](#element-attrdef-filter-primitive-height) on the given element.

***<a id="dom-svgfilterprimitivestandardattributes-result"></a>`result`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [result](#element-attrdef-filter-primitive-result) on the given element.

### <a id="InterfaceSVGFEBlendElement"></a>Interface SVGFEBlendElement

The *<a id="svgfeblendelement"></a>`SVGFEBlendElement`* interface corresponds to the [feBlend](#elementdef-feblend) element.

```
[Exposed=Window]
interface SVGFEBlendElement : SVGElement {

  // Blend Mode Types
  const unsigned short SVG_FEBLEND_MODE_UNKNOWN = 0;
  const unsigned short SVG_FEBLEND_MODE_NORMAL = 1;
  const unsigned short SVG_FEBLEND_MODE_MULTIPLY = 2;
  const unsigned short SVG_FEBLEND_MODE_SCREEN = 3;
  const unsigned short SVG_FEBLEND_MODE_DARKEN = 4;
  const unsigned short SVG_FEBLEND_MODE_LIGHTEN = 5;
  const unsigned short SVG_FEBLEND_MODE_OVERLAY = 6;
  const unsigned short SVG_FEBLEND_MODE_COLOR_DODGE = 7;
  const unsigned short SVG_FEBLEND_MODE_COLOR_BURN = 8;
  const unsigned short SVG_FEBLEND_MODE_HARD_LIGHT = 9;
  const unsigned short SVG_FEBLEND_MODE_SOFT_LIGHT = 10;
  const unsigned short SVG_FEBLEND_MODE_DIFFERENCE = 11;
  const unsigned short SVG_FEBLEND_MODE_EXCLUSION = 12;
  const unsigned short SVG_FEBLEND_MODE_HUE = 13;
  const unsigned short SVG_FEBLEND_MODE_SATURATION = 14;
  const unsigned short SVG_FEBLEND_MODE_COLOR = 15;
  const unsigned short SVG_FEBLEND_MODE_LUMINOSITY = 16;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedString in2;
  readonly attribute SVGAnimatedEnumeration mode;
};

SVGFEBlendElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Blend Mode Types”: **
:

***<a id="dom-svgfeblendelement-svg_feblend_mode_unknown"></a>`SVG_FEBLEND_MODE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfeblendelement-svg_feblend_mode_normal"></a>`SVG_FEBLEND_MODE_NORMAL`* **
:

Corresponds to value [normal](compositing-1.md#valdef-blend-mode-normal).

***<a id="dom-svgfeblendelement-svg_feblend_mode_multiply"></a>`SVG_FEBLEND_MODE_MULTIPLY`* **
:

Corresponds to value multiply.

***<a id="dom-svgfeblendelement-svg_feblend_mode_screen"></a>`SVG_FEBLEND_MODE_SCREEN`* **
:

Corresponds to value [screen](compositing-1.md#valdef-blend-mode-screen).

***<a id="dom-svgfeblendelement-svg_feblend_mode_darken"></a>`SVG_FEBLEND_MODE_DARKEN`* **
:

Corresponds to value darken.

***<a id="dom-svgfeblendelement-svg_feblend_mode_lighten"></a>`SVG_FEBLEND_MODE_LIGHTEN`* **
:

Corresponds to value lighten.

***<a id="dom-svgfeblendelement-svg_feblend_mode_overlay"></a>`SVG_FEBLEND_MODE_OVERLAY`* **
:

Corresponds to value overlay.

***<a id="dom-svgfeblendelement-svg_feblend_mode_color_dodge"></a>`SVG_FEBLEND_MODE_COLOR_DODGE`* **
:

Corresponds to value color-dodge.

***<a id="dom-svgfeblendelement-svg_feblend_mode_color_burn"></a>`SVG_FEBLEND_MODE_COLOR_BURN`* **
:

Corresponds to value color-burn.

***<a id="dom-svgfeblendelement-svg_feblend_mode_hard_light"></a>`SVG_FEBLEND_MODE_HARD_LIGHT`* **
:

Corresponds to value hard-light.

***<a id="dom-svgfeblendelement-svg_feblend_mode_soft_light"></a>`SVG_FEBLEND_MODE_SOFT_LIGHT`* **
:

Corresponds to value soft-light.

***<a id="dom-svgfeblendelement-svg_feblend_mode_difference"></a>`SVG_FEBLEND_MODE_DIFFERENCE`* **
:

Corresponds to value difference.

***<a id="dom-svgfeblendelement-svg_feblend_mode_exclusion"></a>`SVG_FEBLEND_MODE_EXCLUSION`* **
:

Corresponds to value exclusion.

***<a id="dom-svgfeblendelement-svg_feblend_mode_hue"></a>`SVG_FEBLEND_MODE_HUE`* **
:

Corresponds to value hue.

***<a id="dom-svgfeblendelement-svg_feblend_mode_saturation"></a>`SVG_FEBLEND_MODE_SATURATION`* **
:

Corresponds to value [saturation](compositing-1.md#valdef-blend-mode-saturation).

***<a id="dom-svgfeblendelement-svg_feblend_mode_color"></a>`SVG_FEBLEND_MODE_COLOR`* **
:

Corresponds to value [color](compositing-1.md#valdef-blend-mode-color).

***<a id="dom-svgfeblendelement-svg_feblend_mode_luminosity"></a>`SVG_FEBLEND_MODE_LUMINOSITY`* **
:

Corresponds to value luminosity.

**Attributes: **
:

***<a id="dom-svgfeblendelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feBlend](#elementdef-feblend) element.

***<a id="dom-svgfeblendelement-in2"></a>`in2`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in2](#element-attrdef-feblend-in2) on the given [feBlend](#elementdef-feblend) element.

***<a id="dom-svgfeblendelement-mode"></a>`mode`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [mode](#element-attrdef-feblend-mode) on the given [feBlend](#elementdef-feblend) element. Takes one of the SVG_FEBLEND_MODE_* constants defined on this interface.

### <a id="InterfaceSVGFEColorMatrixElement"></a>Interface SVGFEColorMatrixElement

The *<a id="svgfecolormatrixelement"></a>`SVGFEColorMatrixElement`* interface corresponds to the [feColorMatrix](#elementdef-fecolormatrix) element.

```
[Exposed=Window]
interface SVGFEColorMatrixElement : SVGElement {

  // Color Matrix Types
  const unsigned short SVG_FECOLORMATRIX_TYPE_UNKNOWN = 0;
  const unsigned short SVG_FECOLORMATRIX_TYPE_MATRIX = 1;
  const unsigned short SVG_FECOLORMATRIX_TYPE_SATURATE = 2;
  const unsigned short SVG_FECOLORMATRIX_TYPE_HUEROTATE = 3;
  const unsigned short SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA = 4;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedEnumeration type;
  readonly attribute SVGAnimatedNumberList values;
};

SVGFEColorMatrixElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Color Matrix Types”: **
:

***<a id="dom-svgfecolormatrixelement-svg_fecolormatrix_type_unknown"></a>`SVG_FECOLORMATRIX_TYPE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfecolormatrixelement-svg_fecolormatrix_type_matrix"></a>`SVG_FECOLORMATRIX_TYPE_MATRIX`* **
:

Corresponds to value matrix.

***<a id="dom-svgfecolormatrixelement-svg_fecolormatrix_type_saturate"></a>`SVG_FECOLORMATRIX_TYPE_SATURATE`* **
:

Corresponds to value saturate.

***<a id="dom-svgfecolormatrixelement-svg_fecolormatrix_type_huerotate"></a>`SVG_FECOLORMATRIX_TYPE_HUEROTATE`* **
:

Corresponds to value hueRotate.

***<a id="dom-svgfecolormatrixelement-svg_fecolormatrix_type_luminancetoalpha"></a>`SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA`* **
:

Corresponds to value luminanceToAlpha.

**Attributes: **
:

***<a id="dom-svgfecolormatrixelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feColorMatrix](#elementdef-fecolormatrix) element.

***<a id="dom-svgfecolormatrixelement-type"></a>`type`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [type](#element-attrdef-fecolormatrix-type) on the given [feColorMatrix](#elementdef-fecolormatrix) element. Takes one of the SVG_FECOLORMATRIX_TYPE_* constants defined on this interface.

***<a id="dom-svgfecolormatrixelement-values"></a>`values`*, of type [SVGAnimatedNumberList](svg2/types.md#svganimatednumberlist), readonly **
:

Corresponds to attribute [values](#element-attrdef-fecolormatrix-values) on the given [feColorMatrix](#elementdef-fecolormatrix) element.

### <a id="InterfaceSVGFEComponentTransferElement"></a>Interface SVGFEComponentTransferElement

The *<a id="svgfecomponenttransferelement"></a>`SVGFEComponentTransferElement`* interface corresponds to the [feComponentTransfer](#elementdef-fecomponenttransfer) element.

```
[Exposed=Window]
interface SVGFEComponentTransferElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
};

SVGFEComponentTransferElement includes SVGFilterPrimitiveStandardAttributes;
```

**Attributes: **
:

***<a id="dom-svgfecomponenttransferelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element.

### <a id="InterfaceSVGComponentTransferFunctionElement"></a>Interface SVGComponentTransferFunctionElement

This interface defines a base interface used by the component transfer function interfaces.

```
[Exposed=Window]
interface <a id="svgcomponenttransferfunctionelement"></a>SVGComponentTransferFunctionElement : SVGElement {

  // Component Transfer Types
  const unsigned short SVG_FECOMPONENTTRANSFER_TYPE_UNKNOWN = 0;
  const unsigned short SVG_FECOMPONENTTRANSFER_TYPE_IDENTITY = 1;
  const unsigned short SVG_FECOMPONENTTRANSFER_TYPE_TABLE = 2;
  const unsigned short SVG_FECOMPONENTTRANSFER_TYPE_DISCRETE = 3;
  const unsigned short SVG_FECOMPONENTTRANSFER_TYPE_LINEAR = 4;
  const unsigned short SVG_FECOMPONENTTRANSFER_TYPE_GAMMA = 5;

  readonly attribute SVGAnimatedEnumeration type;
  readonly attribute SVGAnimatedNumberList tableValues;
  readonly attribute SVGAnimatedNumber slope;
  readonly attribute SVGAnimatedNumber intercept;
  readonly attribute SVGAnimatedNumber amplitude;
  readonly attribute SVGAnimatedNumber exponent;
  readonly attribute SVGAnimatedNumber offset;
};
```

**Constants in group “Component Transfer Types”: **
:

***<a id="dom-svgcomponenttransferfunctionelement-svg_fecomponenttransfer_type_unknown"></a>`SVG_FECOMPONENTTRANSFER_TYPE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgcomponenttransferfunctionelement-svg_fecomponenttransfer_type_identity"></a>`SVG_FECOMPONENTTRANSFER_TYPE_IDENTITY`* **
:

Corresponds to value identity.

***<a id="dom-svgcomponenttransferfunctionelement-svg_fecomponenttransfer_type_table"></a>`SVG_FECOMPONENTTRANSFER_TYPE_TABLE`* **
:

Corresponds to value table.

***<a id="dom-svgcomponenttransferfunctionelement-svg_fecomponenttransfer_type_discrete"></a>`SVG_FECOMPONENTTRANSFER_TYPE_DISCRETE`* **
:

Corresponds to value discrete.

***<a id="dom-svgcomponenttransferfunctionelement-svg_fecomponenttransfer_type_linear"></a>`SVG_FECOMPONENTTRANSFER_TYPE_LINEAR`* **
:

Corresponds to value linear.

***<a id="dom-svgcomponenttransferfunctionelement-svg_fecomponenttransfer_type_gamma"></a>`SVG_FECOMPONENTTRANSFER_TYPE_GAMMA`* **
:

Corresponds to value gamma.

**Attributes: **
:

***<a id="dom-svgcomponenttransferfunctionelement-type"></a>`type`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element. Takes one of the SVG_FECOMPONENTTRANSFER_TYPE_* constants defined on this interface.

***<a id="dom-svgcomponenttransferfunctionelement-tablevalues"></a>`tableValues`*, of type [SVGAnimatedNumberList](svg2/types.md#svganimatednumberlist), readonly **
:

Corresponds to attribute [tableValues](#element-attrdef-fecomponenttransfer-tablevalues) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element. Takes one of the SVG_FECOLORMATRIX_TYPE_* constants defined on this interface.

***<a id="dom-svgcomponenttransferfunctionelement-slope"></a>`slope`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [slope](#element-attrdef-fecomponenttransfer-slope) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element.

***<a id="dom-svgcomponenttransferfunctionelement-intercept"></a>`intercept`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [intercept](#element-attrdef-fecomponenttransfer-intercept) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element.

***<a id="dom-svgcomponenttransferfunctionelement-amplitude"></a>`amplitude`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [amplitude](#element-attrdef-fecomponenttransfer-amplitude) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element.

***<a id="dom-svgcomponenttransferfunctionelement-exponent"></a>`exponent`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [exponent](#element-attrdef-fecomponenttransfer-exponent) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element.

***<a id="dom-svgcomponenttransferfunctionelement-offset"></a>`offset`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [offset](#element-attrdef-fecomponenttransfer-offset) on the given [feComponentTransfer](#elementdef-fecomponenttransfer) element.

### <a id="InterfaceSVGFEFuncRElement"></a>Interface SVGFEFuncRElement

The *<a id="svgfefuncrelement"></a>`SVGFEFuncRElement`* interface corresponds to the [feFuncR](#elementdef-fefuncr) element.

```
[Exposed=Window]
interface SVGFEFuncRElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFEFuncGElement"></a>Interface SVGFEFuncGElement

The *<a id="svgfefuncgelement"></a>`SVGFEFuncGElement`* interface corresponds to the [feFuncG](#elementdef-fefuncg) element.

```
[Exposed=Window]
interface SVGFEFuncGElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFEFuncBElement"></a>Interface SVGFEFuncBElement

The *<a id="svgfefuncbelement"></a>`SVGFEFuncBElement`* interface corresponds to the [feFuncB](#elementdef-fefuncb) element.

```
[Exposed=Window]
interface SVGFEFuncBElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFEFuncAElement"></a>Interface SVGFEFuncAElement

The *<a id="svgfefuncaelement"></a>`SVGFEFuncAElement`* interface corresponds to the [feFuncA](#elementdef-fefunca) element.

```
[Exposed=Window]
interface SVGFEFuncAElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFECompositeElement"></a>Interface SVGFECompositeElement

The *<a id="svgfecompositeelement"></a>`SVGFECompositeElement`* interface corresponds to the [feComposite](#elementdef-fecomposite) element.

```
[Exposed=Window]
interface SVGFECompositeElement : SVGElement {

  // Composite Operators
  const unsigned short SVG_FECOMPOSITE_OPERATOR_UNKNOWN = 0;
  const unsigned short SVG_FECOMPOSITE_OPERATOR_OVER = 1;
  const unsigned short SVG_FECOMPOSITE_OPERATOR_IN = 2;
  const unsigned short SVG_FECOMPOSITE_OPERATOR_OUT = 3;
  const unsigned short SVG_FECOMPOSITE_OPERATOR_ATOP = 4;
  const unsigned short SVG_FECOMPOSITE_OPERATOR_XOR = 5;
  const unsigned short SVG_FECOMPOSITE_OPERATOR_ARITHMETIC = 6;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedString in2;
  readonly attribute SVGAnimatedEnumeration operator;
  readonly attribute SVGAnimatedNumber k1;
  readonly attribute SVGAnimatedNumber k2;
  readonly attribute SVGAnimatedNumber k3;
  readonly attribute SVGAnimatedNumber k4;
};

SVGFECompositeElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Composite Operators”: **
:

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_unknown"></a>`SVG_FECOMPOSITE_OPERATOR_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_over"></a>`SVG_FECOMPOSITE_OPERATOR_OVER`* **
:

Corresponds to value [over](#attr-valuedef-fecomposite-operator-over).

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_in"></a>`SVG_FECOMPOSITE_OPERATOR_IN`* **
:

Corresponds to value [in](#attr-valuedef-fecomposite-operator-in).

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_out"></a>`SVG_FECOMPOSITE_OPERATOR_OUT`* **
:

Corresponds to value [out](#attr-valuedef-fecomposite-operator-out).

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_atop"></a>`SVG_FECOMPOSITE_OPERATOR_ATOP`* **
:

Corresponds to value [atop](#attr-valuedef-fecomposite-operator-atop).

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_xor"></a>`SVG_FECOMPOSITE_OPERATOR_XOR`* **
:

Corresponds to value [xor](#attr-valuedef-fecomposite-operator-xor).

***<a id="dom-svgfecompositeelement-svg_fecomposite_operator_arithmetic"></a>`SVG_FECOMPOSITE_OPERATOR_ARITHMETIC`* **
:

Corresponds to value [arithmetic](#attr-valuedef-fecomposite-operator-arithmetic).

**Attributes: **
:

***<a id="dom-svgfecompositeelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feComposite](#elementdef-fecomposite) element.

***<a id="dom-svgfecompositeelement-in2"></a>`in2`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in2](#element-attrdef-fecomposite-in2) on the given [feComposite](#elementdef-fecomposite) element.

***<a id="dom-svgfecompositeelement-operator"></a>`operator`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [operator](#element-attrdef-fecomposite-operator) on the given [feComposite](#elementdef-fecomposite) element.

***<a id="dom-svgfecompositeelement-k1"></a>`k1`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [k1](#element-attrdef-fecomposite-k1) on the given [feComposite](#elementdef-fecomposite) element.

***<a id="dom-svgfecompositeelement-k2"></a>`k2`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [k2](#element-attrdef-fecomposite-k2) on the given [feComposite](#elementdef-fecomposite) element.

***<a id="dom-svgfecompositeelement-k3"></a>`k3`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [k3](#element-attrdef-fecomposite-k3) on the given [feComposite](#elementdef-fecomposite) element.

***<a id="dom-svgfecompositeelement-k4"></a>`k4`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [k4](#element-attrdef-fecomposite-k4) on the given [feComposite](#elementdef-fecomposite) element.

### <a id="InterfaceSVGFEConvolveMatrixElement"></a>Interface SVGFEConvolveMatrixElement

The *<a id="svgfeconvolvematrixelement"></a>`SVGFEConvolveMatrixElement`* interface corresponds to the [feConvolveMatrix](#elementdef-feconvolvematrix) element.

```
[Exposed=Window]
interface SVGFEConvolveMatrixElement : SVGElement {

  // Edge Mode Values
  const unsigned short SVG_EDGEMODE_UNKNOWN = 0;
  const unsigned short SVG_EDGEMODE_DUPLICATE = 1;
  const unsigned short SVG_EDGEMODE_WRAP = 2;
  const unsigned short SVG_EDGEMODE_NONE = 3;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedInteger orderX;
  readonly attribute SVGAnimatedInteger orderY;
  readonly attribute SVGAnimatedNumberList kernelMatrix;
  readonly attribute SVGAnimatedNumber divisor;
  readonly attribute SVGAnimatedNumber bias;
  readonly attribute SVGAnimatedInteger targetX;
  readonly attribute SVGAnimatedInteger targetY;
  readonly attribute SVGAnimatedEnumeration edgeMode;
  readonly attribute SVGAnimatedNumber kernelUnitLengthX;
  readonly attribute SVGAnimatedNumber kernelUnitLengthY;
  readonly attribute SVGAnimatedBoolean preserveAlpha;
};

SVGFEConvolveMatrixElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Edge Mode Values”: **
:

***<a id="dom-svgfeconvolvematrixelement-svg_edgemode_unknown"></a>`SVG_EDGEMODE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfeconvolvematrixelement-svg_edgemode_duplicate"></a>`SVG_EDGEMODE_DUPLICATE`* **
:

Corresponds to value [duplicate](#attr-valuedef-fegaussianblur-edgemode-duplicate).

***<a id="dom-svgfeconvolvematrixelement-svg_edgemode_wrap"></a>`SVG_EDGEMODE_WRAP`* **
:

Corresponds to value [wrap](#attr-valuedef-fegaussianblur-edgemode-wrap).

***<a id="dom-svgfeconvolvematrixelement-svg_edgemode_none"></a>`SVG_EDGEMODE_NONE`* **
:

Corresponds to value none.

**Attributes: **
:

***<a id="dom-svgfeconvolvematrixelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-orderx"></a>`orderX`*, of type [SVGAnimatedInteger](svg2/types.md#svganimatedinteger), readonly **
:

Corresponds to attribute [order](#element-attrdef-order) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-ordery"></a>`orderY`*, of type [SVGAnimatedInteger](svg2/types.md#svganimatedinteger), readonly **
:

Corresponds to attribute [order](#element-attrdef-order) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-kernelmatrix"></a>`kernelMatrix`*, of type [SVGAnimatedNumberList](svg2/types.md#svganimatednumberlist), readonly **
:

Corresponds to attribute [kernelMatrix](#element-attrdef-feconvolvematrix-kernelmatrix) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-divisor"></a>`divisor`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [divisor](#element-attrdef-feconvolvematrix-divisor) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-bias"></a>`bias`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [bias](#element-attrdef-feconvolvematrix-bias) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-targetx"></a>`targetX`*, of type [SVGAnimatedInteger](svg2/types.md#svganimatedinteger), readonly **
:

Corresponds to attribute [targetX](#element-attrdef-feconvolvematrix-targetx) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-targety"></a>`targetY`*, of type [SVGAnimatedInteger](svg2/types.md#svganimatedinteger), readonly **
:

Corresponds to attribute [targetY](#element-attrdef-feconvolvematrix-targety) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-edgemode"></a>`edgeMode`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [edgeMode](#element-attrdef-feconvolvematrix-edgemode) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-kernelunitlengthx"></a>`kernelUnitLengthX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-kernelunitlengthy"></a>`kernelUnitLengthY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [kernelUnitLength](#element-attrdef-feconvolvematrix-kernelunitlength) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

***<a id="dom-svgfeconvolvematrixelement-preservealpha"></a>`preserveAlpha`*, of type [SVGAnimatedBoolean](svg2/types.md#svganimatedboolean), readonly **
:

Corresponds to attribute [preserveAlpha](#element-attrdef-feconvolvematrix-preservealpha) on the given [feConvolveMatrix](#elementdef-feconvolvematrix) element.

### <a id="InterfaceSVGFEDiffuseLightingElement"></a>Interface SVGFEDiffuseLightingElement

The *<a id="svgfediffuselightingelement"></a>`SVGFEDiffuseLightingElement`* interface corresponds to the [feDiffuseLighting](#elementdef-fediffuselighting) element.

```
[Exposed=Window]
interface SVGFEDiffuseLightingElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber surfaceScale;
  readonly attribute SVGAnimatedNumber diffuseConstant;
  readonly attribute SVGAnimatedNumber kernelUnitLengthX;
  readonly attribute SVGAnimatedNumber kernelUnitLengthY;
};

SVGFEDiffuseLightingElement includes SVGFilterPrimitiveStandardAttributes;
```

**Attributes: **
:

***<a id="dom-svgfediffuselightingelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feDiffuseLighting](#elementdef-fediffuselighting) element.

***<a id="dom-svgfediffuselightingelement-surfacescale"></a>`surfaceScale`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [surfaceScale](#element-attrdef-fediffuselighting-surfacescale) on the given [feDiffuseLighting](#elementdef-fediffuselighting) element.

***<a id="dom-svgfediffuselightingelement-diffuseconstant"></a>`diffuseConstant`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [diffuseConstant](#element-attrdef-fediffuselighting-diffuseconstant) on the given [feDiffuseLighting](#elementdef-fediffuselighting) element.

***<a id="dom-svgfediffuselightingelement-kernelunitlengthx"></a>`kernelUnitLengthX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength) on the given [feDiffuseLighting](#elementdef-fediffuselighting) element.

***<a id="dom-svgfediffuselightingelement-kernelunitlengthy"></a>`kernelUnitLengthY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [kernelUnitLength](#element-attrdef-fediffuselighting-kernelunitlength) on the given [feDiffuseLighting](#elementdef-fediffuselighting) element.

### <a id="InterfaceSVGFEDistantLightElement"></a>Interface SVGFEDistantLightElement

The *<a id="svgfedistantlightelement"></a>`SVGFEDistantLightElement`* interface corresponds to the [feDistantLight](#elementdef-fedistantlight) element.

```
[Exposed=Window]
interface SVGFEDistantLightElement : SVGElement {
  readonly attribute SVGAnimatedNumber azimuth;
  readonly attribute SVGAnimatedNumber elevation;
};
```

**Attributes: **
:

***<a id="dom-svgfedistantlightelement-azimuth"></a>`azimuth`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [azimuth](#element-attrdef-fedistantlight-azimuth) on the given [feDistantLight](#elementdef-fedistantlight) element.

***<a id="dom-svgfedistantlightelement-elevation"></a>`elevation`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [elevation](#element-attrdef-fedistantlight-elevation) on the given [feDistantLight](#elementdef-fedistantlight) element.

### <a id="InterfaceSVGFEPointLightElement"></a>Interface SVGFEPointLightElement

The *<a id="svgfepointlightelement"></a>`SVGFEPointLightElement`* interface corresponds to the [fePointLight](#elementdef-fepointlight) element.

```
[Exposed=Window]
interface SVGFEPointLightElement : SVGElement {
  readonly attribute SVGAnimatedNumber x;
  readonly attribute SVGAnimatedNumber y;
  readonly attribute SVGAnimatedNumber z;
};
```

**Attributes: **
:

***<a id="dom-svgfepointlightelement-x"></a>`x`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [x](#element-attrdef-fepointlight-x) on the given [fePointLight](#elementdef-fepointlight) element.

***<a id="dom-svgfepointlightelement-y"></a>`y`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [y](#element-attrdef-fepointlight-y) on the given [fePointLight](#elementdef-fepointlight) element.

***<a id="dom-svgfepointlightelement-z"></a>`z`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [z](#element-attrdef-fepointlight-z) on the given [fePointLight](#elementdef-fepointlight) element.

### <a id="InterfaceSVGFESpotLightElement"></a>Interface SVGFESpotLightElement

The *<a id="svgfespotlightelement"></a>`SVGFESpotLightElement`* interface corresponds to the [feSpotLight](#elementdef-fespotlight) element.

```
[Exposed=Window]
interface SVGFESpotLightElement : SVGElement {
  readonly attribute SVGAnimatedNumber x;
  readonly attribute SVGAnimatedNumber y;
  readonly attribute SVGAnimatedNumber z;
  readonly attribute SVGAnimatedNumber pointsAtX;
  readonly attribute SVGAnimatedNumber pointsAtY;
  readonly attribute SVGAnimatedNumber pointsAtZ;
  readonly attribute SVGAnimatedNumber specularExponent;
  readonly attribute SVGAnimatedNumber limitingConeAngle;
};
```

**Attributes: **
:

***<a id="dom-svgfespotlightelement-x"></a>`x`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [x](#element-attrdef-fespotlight-x) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-y"></a>`y`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [y](#element-attrdef-fespotlight-y) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-z"></a>`z`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [z](#element-attrdef-fespotlight-z) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-pointsatx"></a>`pointsAtX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [pointsAtX](#element-attrdef-fespotlight-pointsatx) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-pointsaty"></a>`pointsAtY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [pointsAtY](#element-attrdef-fespotlight-pointsaty) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-pointsatz"></a>`pointsAtZ`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [pointsAtZ](#element-attrdef-fespotlight-pointsatz) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-specularexponent"></a>`specularExponent`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [specularExponent](#element-attrdef-fespotlight-specularexponent) on the given [feSpotLight](#elementdef-fespotlight) element.

***<a id="dom-svgfespotlightelement-limitingconeangle"></a>`limitingConeAngle`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [limitingConeAngle](#element-attrdef-fespotlight-limitingconeangle) on the given [feSpotLight](#elementdef-fespotlight) element.

### <a id="InterfaceSVGFEDisplacementMapElement"></a>Interface SVGFEDisplacementMapElement

The *<a id="svgfedisplacementmapelement"></a>`SVGFEDisplacementMapElement`* interface corresponds to the [feDisplacementMap](#elementdef-fedisplacementmap) element.

```
[Exposed=Window]
interface SVGFEDisplacementMapElement : SVGElement {

  // Channel Selectors
  const unsigned short SVG_CHANNEL_UNKNOWN = 0;
  const unsigned short SVG_CHANNEL_R = 1;
  const unsigned short SVG_CHANNEL_G = 2;
  const unsigned short SVG_CHANNEL_B = 3;
  const unsigned short SVG_CHANNEL_A = 4;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedString in2;
  readonly attribute SVGAnimatedNumber scale;
  readonly attribute SVGAnimatedEnumeration xChannelSelector;
  readonly attribute SVGAnimatedEnumeration yChannelSelector;
};

SVGFEDisplacementMapElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Channel Selectors”: **
:

***<a id="dom-svgfedisplacementmapelement-svg_channel_unknown"></a>`SVG_CHANNEL_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfedisplacementmapelement-svg_channel_r"></a>`SVG_CHANNEL_R`* **
:

Corresponds to value 1 (mapping to the "R" channel).

***<a id="dom-svgfedisplacementmapelement-svg_channel_g"></a>`SVG_CHANNEL_G`* **
:

Corresponds to value 2 (mapping to the "G" channel).

***<a id="dom-svgfedisplacementmapelement-svg_channel_b"></a>`SVG_CHANNEL_B`* **
:

Corresponds to value 3 (mapping to the "B" channel).

***<a id="dom-svgfedisplacementmapelement-svg_channel_a"></a>`SVG_CHANNEL_A`* **
:

Corresponds to value 4 (mapping to the "A" channel).

**Attributes: **
:

***<a id="dom-svgfedisplacementmapelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feDisplacementMap](#elementdef-fedisplacementmap) element.

***<a id="dom-svgfedisplacementmapelement-in2"></a>`in2`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in2](#element-attrdef-fedisplacementmap-in2) on the given [feDisplacementMap](#elementdef-fedisplacementmap) element.

***<a id="dom-svgfedisplacementmapelement-scale"></a>`scale`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [scale](#element-attrdef-fedisplacementmap-scale) on the given [feDisplacementMap](#elementdef-fedisplacementmap) element.

***<a id="dom-svgfedisplacementmapelement-xchannelselector"></a>`xChannelSelector`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [xChannelSelector](#element-attrdef-fedisplacementmap-xchannelselector) on the given [feDisplacementMap](#elementdef-fedisplacementmap) element. Takes one of the SVG_CHANNEL_* constants defined on this interface.

***<a id="dom-svgfedisplacementmapelement-ychannelselector"></a>`yChannelSelector`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [yChannelSelector](#element-attrdef-fedisplacementmap-ychannelselector) on the given [feDisplacementMap](#elementdef-fedisplacementmap) element. Takes one of the SVG_CHANNEL_* constants defined on this interface.

### <a id="InterfaceSVGFEDropShadowElement"></a>Interface SVGFEDropShadowElement

The *<a id="svgfedropshadowelement"></a>`SVGFEDropShadowElement`* interface corresponds to the [feDropShadow](#elementdef-fedropshadow) element.

```
[Exposed=Window]
interface SVGFEDropShadowElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber dx;
  readonly attribute SVGAnimatedNumber dy;
  readonly attribute SVGAnimatedNumber stdDeviationX;
  readonly attribute SVGAnimatedNumber stdDeviationY;

  undefined setStdDeviation(float stdDeviationX, float stdDeviationY);
};

SVGFEDropShadowElement includes SVGFilterPrimitiveStandardAttributes;
```

**Attributes: **
:

***<a id="dom-svgfedropshadowelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feDropShadow](#elementdef-fedropshadow) element.

***<a id="dom-svgfedropshadowelement-dx"></a>`dx`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [dx](#element-attrdef-fedropshadow-dx) on the given [feDropShadow](#elementdef-fedropshadow) element.

***<a id="dom-svgfedropshadowelement-dy"></a>`dy`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [dy](#element-attrdef-fedropshadow-dy) on the given [feDropShadow](#elementdef-fedropshadow) element.

***<a id="dom-svgfedropshadowelement-stddeviationx"></a>`stdDeviationX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [stdDeviation](#element-attrdef-fedropshadow-stddeviation) on the given [feDropShadow](#elementdef-fedropshadow) element. Contains the X component of attribute stdDeviation.

***<a id="dom-svgfedropshadowelement-stddeviationy"></a>`stdDeviationY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [stdDeviation](#element-attrdef-fedropshadow-stddeviation) on the given [feDropShadow](#elementdef-fedropshadow) element. Contains the Y component of attribute stdDeviation.

**Methods: **
:

***<a id="dom-svgfedropshadowelement-setstddeviation"></a>`setStdDeviation(stdDeviationX, stdDeviationY)`* **
:

Sets the values for attribute [stdDeviation](#element-attrdef-fedropshadow-stddeviation).

***<a id="dom-svgfedropshadowelement-setstddeviation-stddeviationx-stddeviationy-stddeviationx"></a>`stdDeviationX`* **
:

The X component of attribute [stdDeviation](#element-attrdef-fedropshadow-stddeviation).

***<a id="dom-svgfedropshadowelement-setstddeviation-stddeviationx-stddeviationy-stddeviationy"></a>`stdDeviationY`* **
:

The Y component of attribute [stdDeviation](#element-attrdef-fedropshadow-stddeviation).

### <a id="InterfaceSVGFEFloodElement"></a>Interface SVGFEFloodElement

The *<a id="svgfefloodelement"></a>`SVGFEFloodElement`* interface corresponds to the [feFlood](#elementdef-feflood) element.

```
[Exposed=Window]
interface SVGFEFloodElement : SVGElement {
};

SVGFEFloodElement includes SVGFilterPrimitiveStandardAttributes;
```

### <a id="InterfaceSVGFEGaussianBlurElement"></a>Interface SVGFEGaussianBlurElement

The *<a id="svgfegaussianblurelement"></a>`SVGFEGaussianBlurElement`* interface corresponds to the [feGaussianBlur](#elementdef-fegaussianblur) element.

```
[Exposed=Window]
interface SVGFEGaussianBlurElement : SVGElement {

  // Edge Mode Values
  const unsigned short SVG_EDGEMODE_UNKNOWN = 0;
  const unsigned short SVG_EDGEMODE_DUPLICATE = 1;
  const unsigned short SVG_EDGEMODE_WRAP = 2;
  const unsigned short SVG_EDGEMODE_NONE = 3;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber stdDeviationX;
  readonly attribute SVGAnimatedNumber stdDeviationY;
  readonly attribute SVGAnimatedEnumeration edgeMode;

  undefined setStdDeviation(float stdDeviationX, float stdDeviationY);
};

SVGFEGaussianBlurElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Edge Mode Values”: **
:

***<a id="dom-svgfegaussianblurelement-svg_edgemode_unknown"></a>`SVG_EDGEMODE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfegaussianblurelement-svg_edgemode_duplicate"></a>`SVG_EDGEMODE_DUPLICATE`* **
:

Corresponds to value duplicate.

***<a id="dom-svgfegaussianblurelement-svg_edgemode_wrap"></a>`SVG_EDGEMODE_WRAP`* **
:

Corresponds to value [wrap](#attr-valuedef-fegaussianblur-edgemode-wrap).

***<a id="dom-svgfegaussianblurelement-svg_edgemode_none"></a>`SVG_EDGEMODE_NONE`* **
:

Corresponds to value none.

**Attributes: **
:

***<a id="dom-svgfegaussianblurelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feGaussianBlur](#elementdef-fegaussianblur) element.

***<a id="dom-svgfegaussianblurelement-stddeviationx"></a>`stdDeviationX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) on the given [feGaussianBlur](#elementdef-fegaussianblur) element. Contains the X component of attribute stdDeviation.

***<a id="dom-svgfegaussianblurelement-stddeviationy"></a>`stdDeviationY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [stdDeviation](#element-attrdef-fegaussianblur-stddeviation) on the given [feGaussianBlur](#elementdef-fegaussianblur) element. Contains the Y component of attribute stdDeviation.

***<a id="dom-svgfegaussianblurelement-edgemode"></a>`edgeMode`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [edgeMode](#element-attrdef-fegaussianblur-edgemode) on the given [feGaussianBlur](#elementdef-fegaussianblur) element. Takes one of the SVG_EDGEMODE_* constants defined on this interface.

**Methods: **
:

***<a id="dom-svgfegaussianblurelement-setstddeviation"></a>`setStdDeviation(stdDeviationX, stdDeviationY)`* **
:

Sets the values for attribute [stdDeviation](#element-attrdef-fegaussianblur-stddeviation).

***<a id="dom-svgfegaussianblurelement-setstddeviation-stddeviationx-stddeviationy-stddeviationx"></a>`stdDeviationX`* **
:

The X component of attribute [stdDeviation](#element-attrdef-fegaussianblur-stddeviation).

***<a id="dom-svgfegaussianblurelement-setstddeviation-stddeviationx-stddeviationy-stddeviationy"></a>`stdDeviationY`* **
:

The Y component of attribute [stdDeviation](#element-attrdef-fegaussianblur-stddeviation).

### <a id="InterfaceSVGFEImageElement"></a>Interface SVGFEImageElement

The *<a id="svgfeimageelement"></a>`SVGFEImageElement`* interface corresponds to the [feImage](#elementdef-feimage) element.

```
[Exposed=Window]
interface SVGFEImageElement : SVGElement {
  readonly attribute SVGAnimatedPreserveAspectRatio preserveAspectRatio;
  readonly attribute SVGAnimatedString crossOrigin;
};

SVGFEImageElement includes SVGFilterPrimitiveStandardAttributes;
SVGFEImageElement includes SVGURIReference;
```

**Attributes: **
:

***<a id="dom-svgfeimageelement-preserveaspectratio"></a>`preserveAspectRatio`*, of type [SVGAnimatedPreserveAspectRatio](svg2/coords.md#svganimatedpreserveaspectratio), readonly **
:

Corresponds to attribute [preserveAspectRatio](#element-attrdef-feimage-preserveaspectratio) on the given [feImage](#elementdef-feimage) element.

***<a id="dom-svgfeimageelement-crossorigin"></a>`crossOrigin`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

The crossOrigin IDL attribute must reflect the [crossorigin](#element-attrdef-feimage-crossorigin) content attribute, limited to only known values.

### <a id="InterfaceSVGFEMergeElement"></a>Interface SVGFEMergeElement

The *<a id="svgfemergeelement"></a>`SVGFEMergeElement`* interface corresponds to the [feMerge](#elementdef-femerge) element.

```
[Exposed=Window]
interface SVGFEMergeElement : SVGElement {
};

SVGFEMergeElement includes SVGFilterPrimitiveStandardAttributes;
```

### <a id="InterfaceSVGFEMergeNodeElement"></a>Interface SVGFEMergeNodeElement

The *<a id="svgfemergenodeelement"></a>`SVGFEMergeNodeElement`* interface corresponds to the [feMergeNode](#elementdef-femergenode) element.

```
[Exposed=Window]
interface SVGFEMergeNodeElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
};
```

**Attributes: **
:

***<a id="dom-svgfemergenodeelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feMergeNode](#elementdef-femergenode) element.

### <a id="InterfaceSVGFEMorphologyElement"></a>Interface SVGFEMorphologyElement

The *<a id="svgfemorphologyelement"></a>`SVGFEMorphologyElement`* interface corresponds to the [feMorphology](#elementdef-femorphology) element.

```
[Exposed=Window]
interface SVGFEMorphologyElement : SVGElement {

  // Morphology Operators
  const unsigned short SVG_MORPHOLOGY_OPERATOR_UNKNOWN = 0;
  const unsigned short SVG_MORPHOLOGY_OPERATOR_ERODE = 1;
  const unsigned short SVG_MORPHOLOGY_OPERATOR_DILATE = 2;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedEnumeration operator;
  readonly attribute SVGAnimatedNumber radiusX;
  readonly attribute SVGAnimatedNumber radiusY;
};

SVGFEMorphologyElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Morphology Operators”: **
:

***<a id="dom-svgfemorphologyelement-svg_morphology_operator_unknown"></a>`SVG_MORPHOLOGY_OPERATOR_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfemorphologyelement-svg_morphology_operator_erode"></a>`SVG_MORPHOLOGY_OPERATOR_ERODE`* **
:

Corresponds to value erode.

***<a id="dom-svgfemorphologyelement-svg_morphology_operator_dilate"></a>`SVG_MORPHOLOGY_OPERATOR_DILATE`* **
:

Corresponds to value dilate.

**Attributes: **
:

***<a id="dom-svgfemorphologyelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feMorphology](#elementdef-femorphology) element.

***<a id="dom-svgfemorphologyelement-operator"></a>`operator`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [operator](#element-attrdef-femorphology-operator) on the given [feMorphology](#elementdef-femorphology) element. Takes one of the SVG_MORPHOLOGY_OPERATOR_* constants defined on this interface.

***<a id="dom-svgfemorphologyelement-radiusx"></a>`radiusX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [radius](#element-attrdef-femorphology-radius) on the given [feMorphology](#elementdef-femorphology) element. Contains the X component of attribute radius.

***<a id="dom-svgfemorphologyelement-radiusy"></a>`radiusY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [radius](#element-attrdef-femorphology-radius) on the given [feMorphology](#elementdef-femorphology) element. Contains the Y component of attribute radius.

### <a id="InterfaceSVGFEOffsetElement"></a>Interface SVGFEOffsetElement

The *<a id="svgfeoffsetelement"></a>`SVGFEOffsetElement`* interface corresponds to the [feOffset](#elementdef-feoffset) element.

```
[Exposed=Window]
interface SVGFEOffsetElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber dx;
  readonly attribute SVGAnimatedNumber dy;
};

SVGFEOffsetElement includes SVGFilterPrimitiveStandardAttributes;
```

**Attributes: **
:

***<a id="dom-svgfeoffsetelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feOffset](#elementdef-feoffset) element.

***<a id="dom-svgfeoffsetelement-dx"></a>`dx`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [dx](#element-attrdef-feoffset-dx) on the given [feOffset](#elementdef-feoffset) element.

***<a id="dom-svgfeoffsetelement-dy"></a>`dy`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [dy](#element-attrdef-feoffset-dy) on the given [feOffset](#elementdef-feoffset) element.

### <a id="InterfaceSVGFESpecularLightingElement"></a>Interface SVGFESpecularLightingElement

The *<a id="svgfespecularlightingelement"></a>`SVGFESpecularLightingElement`* interface corresponds to the [feSpecularLighting](#elementdef-fespecularlighting) element.

```
[Exposed=Window]
interface SVGFESpecularLightingElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber surfaceScale;
  readonly attribute SVGAnimatedNumber specularConstant;
  readonly attribute SVGAnimatedNumber specularExponent;
  readonly attribute SVGAnimatedNumber kernelUnitLengthX;
  readonly attribute SVGAnimatedNumber kernelUnitLengthY;
};

SVGFESpecularLightingElement includes SVGFilterPrimitiveStandardAttributes;
```

**Attributes: **
:

***<a id="dom-svgfespecularlightingelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feSpecularLighting](#elementdef-fespecularlighting) element.

***<a id="dom-svgfespecularlightingelement-surfacescale"></a>`surfaceScale`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [surfaceScale](#element-attrdef-fespecularlighting-surfacescale) on the given [feSpecularLighting](#elementdef-fespecularlighting) element.

***<a id="dom-svgfespecularlightingelement-specularconstant"></a>`specularConstant`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [specularConstant](#element-attrdef-fespecularlighting-specularconstant) on the given [feSpecularLighting](#elementdef-fespecularlighting) element.

***<a id="dom-svgfespecularlightingelement-specularexponent"></a>`specularExponent`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [specularExponent](#element-attrdef-fespecularlighting-specularexponent) on the given [feSpecularLighting](#elementdef-fespecularlighting) element.

***<a id="dom-svgfespecularlightingelement-kernelunitlengthx"></a>`kernelUnitLengthX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [kernelUnitLength](#element-attrdef-fespecularlighting-kernelunitlength) on the given [feSpecularLighting](#elementdef-fespecularlighting) element.

***<a id="dom-svgfespecularlightingelement-kernelunitlengthy"></a>`kernelUnitLengthY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [kernelUnitLength](#element-attrdef-fespecularlighting-kernelunitlength) on the given [feSpecularLighting](#elementdef-fespecularlighting) element.

### <a id="InterfaceSVGFETileElement"></a>Interface SVGFETileElement

The *<a id="svgfetileelement"></a>`SVGFETileElement`* interface corresponds to the [feTile](#elementdef-fetile) element.

```
[Exposed=Window]
interface SVGFETileElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
};

SVGFETileElement includes SVGFilterPrimitiveStandardAttributes;
```

**Attributes: **
:

***<a id="dom-svgfetileelement-in1"></a>`in1`*, of type [SVGAnimatedString](svg2/types.md#svganimatedstring), readonly **
:

Corresponds to attribute [in](#element-attrdef-filter-primitive-in) on the given [feTile](#elementdef-fetile) element.

### <a id="InterfaceSVGFETurbulenceElement"></a>Interface SVGFETurbulenceElement

The *<a id="svgfeturbulenceelement"></a>`SVGFETurbulenceElement`* interface corresponds to the [feTurbulence](#elementdef-feturbulence) element.

```
[Exposed=Window]
interface SVGFETurbulenceElement : SVGElement {

  // Turbulence Types
  const unsigned short SVG_TURBULENCE_TYPE_UNKNOWN = 0;
  const unsigned short SVG_TURBULENCE_TYPE_FRACTALNOISE = 1;
  const unsigned short SVG_TURBULENCE_TYPE_TURBULENCE = 2;

  // Stitch Options
  const unsigned short SVG_STITCHTYPE_UNKNOWN = 0;
  const unsigned short SVG_STITCHTYPE_STITCH = 1;
  const unsigned short SVG_STITCHTYPE_NOSTITCH = 2;

  readonly attribute SVGAnimatedNumber baseFrequencyX;
  readonly attribute SVGAnimatedNumber baseFrequencyY;
  readonly attribute SVGAnimatedInteger numOctaves;
  readonly attribute SVGAnimatedNumber seed;
  readonly attribute SVGAnimatedEnumeration stitchTiles;
  readonly attribute SVGAnimatedEnumeration type;
};

SVGFETurbulenceElement includes SVGFilterPrimitiveStandardAttributes;
```

**Constants in group “Turbulence Types”: **
:

***<a id="dom-svgfeturbulenceelement-svg_turbulence_type_unknown"></a>`SVG_TURBULENCE_TYPE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfeturbulenceelement-svg_turbulence_type_fractalnoise"></a>`SVG_TURBULENCE_TYPE_FRACTALNOISE`* **
:

Corresponds to value fractalNoise.

***<a id="dom-svgfeturbulenceelement-svg_turbulence_type_turbulence"></a>`SVG_TURBULENCE_TYPE_TURBULENCE`* **
:

Corresponds to value turbulence.

**Constants in group “Stitch Options”: **
:

***<a id="dom-svgfeturbulenceelement-svg_stitchtype_unknown"></a>`SVG_STITCHTYPE_UNKNOWN`* **
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

***<a id="dom-svgfeturbulenceelement-svg_stitchtype_stitch"></a>`SVG_STITCHTYPE_STITCH`* **
:

Corresponds to value stitch.

***<a id="dom-svgfeturbulenceelement-svg_stitchtype_nostitch"></a>`SVG_STITCHTYPE_NOSTITCH`* **
:

Corresponds to value noStitch.

**Attributes: **
:

***<a id="dom-svgfeturbulenceelement-basefrequencyx"></a>`baseFrequencyX`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [baseFrequency](#element-attrdef-feturbulence-basefrequency) on the given [feTurbulence](#elementdef-feturbulence) element. Contains the X component of the baseFrequency attribute.

***<a id="dom-svgfeturbulenceelement-basefrequencyy"></a>`baseFrequencyY`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [baseFrequency](#element-attrdef-feturbulence-basefrequency) on the given [feTurbulence](#elementdef-feturbulence) element. Contains the Y component of the baseFrequency attribute.

***<a id="dom-svgfeturbulenceelement-numoctaves"></a>`numOctaves`*, of type [SVGAnimatedInteger](svg2/types.md#svganimatedinteger), readonly **
:

Corresponds to attribute [numOctaves](#element-attrdef-feturbulence-numoctaves) on the given [feTurbulence](#elementdef-feturbulence) element.

***<a id="dom-svgfeturbulenceelement-seed"></a>`seed`*, of type [SVGAnimatedNumber](svg2/types.md#svganimatednumber), readonly **
:

Corresponds to attribute [seed](#element-attrdef-feturbulence-seed) on the given [feTurbulence](#elementdef-feturbulence) element.

***<a id="dom-svgfeturbulenceelement-stitchtiles"></a>`stitchTiles`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [stitchTiles](#element-attrdef-feturbulence-stitchtiles) on the given [feTurbulence](#elementdef-feturbulence) element. Takes one of the SVG_TURBULENCE_TYPE_* constants defined on this interface.

***<a id="dom-svgfeturbulenceelement-type"></a>`type`*, of type [SVGAnimatedEnumeration](svg2/types.md#svganimatedenumeration), readonly **
:

Corresponds to attribute [type](#element-attrdef-feturbulence-type) on the given [feTurbulence](#elementdef-feturbulence) element. Takes one of the SVG_STITCHTYPE_* constants defined on this interface.

## <a id="changes"></a>Changes

The following significant changes were made since the 18 December 2018 Working Draft.

-
Removed `SVGURIInterface` from `[SVGFilterElement](#svgfilterelement)`, per SVGWG and CSSWG resolution. (Issue #13949)

The following significant changes were made since the 25 November 2014 Working Draft.

-
Editorial changes.
-
Add description elements to content model of all elements.
-
Clarify that crossorigin is not animatable.
-
[hue-rotate()](#funcdef-filter-hue-rotate) takes unitless zero.
-
Allow author to change order of <color> and [<length>](css-values-4.md#length-value) values of [drop-shadow()](#funcdef-filter-drop-shadow). Change grammar to put <color> first. Differentiate between initial value for interpolation and default values for omitted values.
-
Define animation type of CSS properties.
-
Make [feColorMatrix](#elementdef-fecolormatrix) and [feConvolveMatrix](#elementdef-feconvolvematrix) a pass through on unfulfilled pre-conditions.
-
Make [feTurbulence](#elementdef-feturbulence) algorithms respect uniformity.
-
Apply properties that apply to all graphics elements to the use element as well.
-
Corrected filter primitive representation of [saturate()](#funcdef-filter-saturate).
-
Extend SVG DOM SVGFEBlendElement enumerations with new blend modes.

The following significant changes were made since the 26 November 2013 Working Draft.

-
Removed Custom Filters.
-
Allow the [script](svg2/interact.md#elementdef-script) element in the content model everywhere.
-
Support all blend modes from CSS Blending specification for [feBlend](#elementdef-feblend).
-
Support all non-duplicated compositing modes from CSS Blending specification for [feComposite](#elementdef-fecomposite).
-
Added no-composite attribute to [feBlend](#elementdef-feblend) to avoid double compositing.
-
Corrections on shorthands syntax.
-
Added definition for shorthand filter regions.

The following significant changes were made since the 25 October 2012 Working Draft.

-
Correction of brightness short hand filter.
-
New syntax for Custom Filter function.
-
Add at-function rule for Custom Filters.
-
Allow Custom Filter function to be used as extension for future filter features.
-
Remove unnecessary attributes and uniforms on shaders.
-
Redefine origin of shader coordinate space to bottom left.
-
Remove now unnecessary filter-margin properties.

See more detailed and longterm changes in the [ChangeLog](https://drafts.csswg.org/filter-effects-1/ChangeLog).

## <a id="acknowledgments"></a>Acknowledgments

The editors would like to thank Robert O’Callahan, Coralie Mercier, Chris Lilley, Nikos Andronikos, Stephen Chenney, Simon Fraser, Tavmjong Bah, Robert Longson, Cameron McCormack, Brad Kemper, Tab Atkins, Brian Birtles, Michael Mullany, Rik Cabanier, Anne van Kesteren, Boris Zbarsky, Kristopher Giesing, Stephen White, Jasper van de Gronde, Kang-Hao Lu, Paul LeBeau, Debarshi Ray, Jarek Foksa, Sebastian Zartner, Yuqian Li, Amelia Bellamy-Royds and Max Vujovic for their careful reviews, comments, and corrections.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-compositing-1"></a>[COMPOSITING-1] **
: Chris Harrelson. [Compositing and Blending Level 1](compositing-1.md). URL: [https://drafts.csswg.org/compositing-1/](compositing-1.md)
**<a id="biblio-compositing-2"></a>[COMPOSITING-2] **
: Compositing and Blending Module Level 2. Editor's Draft. URL: https://drafts.csswg.org/compositing-2/
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-color-5"></a>[CSS-COLOR-5] **
: Chris Lilley; Una Kravets; Lea Verou. CSS Color Module Level 5. URL: https://drafts.csswg.org/css-color-5/
**<a id="biblio-css-display-3"></a>[CSS-DISPLAY-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Display Module Level 3. URL: https://drafts.csswg.org/css-display/
**<a id="biblio-css-display-4"></a>[CSS-DISPLAY-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Display Module Level 4. URL: https://drafts.csswg.org/css-display-4/
**<a id="biblio-css-fonts-4"></a>[CSS-FONTS-4] **
: Chris Lilley. CSS Fonts Module Level 4. URL: https://drafts.csswg.org/css-fonts-4/
**<a id="biblio-css-fonts-5"></a>[CSS-FONTS-5] **
: Chris Lilley. CSS Fonts Module Level 5. URL: https://drafts.csswg.org/css-fonts-5/
**<a id="biblio-css-images-3"></a>[CSS-IMAGES-3] **
: Tab Atkins Jr.; Elika Etemad; Lea Verou. [CSS Images Module Level 3](css-images-3.md). URL: [https://drafts.csswg.org/css-images-3/](css-images-3.md)
**<a id="biblio-css-inline-3"></a>[CSS-INLINE-3] **
: Elika Etemad. CSS Inline Layout Module Level 3. URL: https://drafts.csswg.org/css-inline-3/
**<a id="biblio-css-masking-1"></a>[CSS-MASKING-1] **
: Dirk Schulze; Brian Birtles; Tab Atkins Jr.. [CSS Masking Module Level 1](css-masking-1.md). URL: [https://drafts.csswg.org/css-masking-1/](css-masking-1.md)
**<a id="biblio-css-overflow-3"></a>[CSS-OVERFLOW-3] **
: Elika Etemad; Florian Rivoal. CSS Overflow Module Level 3. URL: https://drafts.csswg.org/css-overflow-3/
**<a id="biblio-css-text-4"></a>[CSS-TEXT-4] **
: Elika Etemad; et al. CSS Text Module Level 4. URL: https://drafts.csswg.org/css-text-4/
**<a id="biblio-css-text-decor-4"></a>[CSS-TEXT-DECOR-4] **
: Elika Etemad; Koji Ishii. CSS Text Decoration Module Level 4. URL: https://drafts.csswg.org/css-text-decor-4/
**<a id="biblio-css-transforms-1"></a>[CSS-TRANSFORMS-1] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css-transitions-1"></a>[CSS-TRANSITIONS-1] **
: Chris Marrin; et al. CSS Transitions Module Level 1. URL: https://drafts.csswg.org/css-transitions/
**<a id="biblio-css-ui-4"></a>[CSS-UI-4] **
: Tab Atkins Jr.; Florian Rivoal. CSS Basic User Interface Module Level 4. URL: https://drafts.csswg.org/css-ui-4/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-writing-modes-3"></a>[CSS-WRITING-MODES-3] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 3. URL: https://drafts.csswg.org/css-writing-modes-3/
**<a id="biblio-css-writing-modes-4"></a>[CSS-WRITING-MODES-4] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 4. URL: https://drafts.csswg.org/css-writing-modes-4/
**<a id="biblio-css21"></a>[CSS21] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3bg"></a>[CSS3BG] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-css3val"></a>[CSS3VAL] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-fetch"></a>[Fetch] **
: Anne van Kesteren. Fetch Standard. Living Standard. URL: https://fetch.spec.whatwg.org/
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
**<a id="biblio-mediaqueries-5"></a>[MEDIAQUERIES-5] **
: Tab Atkins Jr.; et al. Media Queries Level 5. URL: https://drafts.csswg.org/mediaqueries-5/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-selectors-4"></a>[SELECTORS-4] **
: Elika Etemad; Tab Atkins Jr.. [Selectors Level 4](selectors-4.md). URL: [https://drafts.csswg.org/selectors/](selectors-4.md)
**<a id="biblio-svg-animations"></a>[SVG-ANIMATIONS] **
: SVG Animations Level 2. Editor's Draft. URL: https://svgwg.org/specs/animations/
**<a id="biblio-svg11"></a>[SVG11] **
: Erik Dahlström; et al. [Scalable Vector Graphics (SVG) 1.1 (Second Edition)](svg11/index.md). 16 August 2011. REC. URL: [https://www.w3.org/TR/SVG11/](svg11/index.md)
**<a id="biblio-svg2"></a>[SVG2] **
: Karl Dubost; et al. [Scalable Vector Graphics (SVG) 2](svg2/index.md). URL: [https://w3c.github.io/svgwg/svg2-draft/](svg2/index.md)
**<a id="biblio-web-animations-1"></a>[WEB-ANIMATIONS-1] **
: Brian Birtles; et al. Web Animations. URL: https://drafts.csswg.org/web-animations-1/
**<a id="biblio-webidl"></a>[WEBIDL] **
: Edgar Chen; Timothy Gu. Web IDL Standard. Living Standard. URL: https://webidl.spec.whatwg.org/
### <a id="informative"></a>Non-Normative References

**<a id="biblio-artd"></a>[ArTD] **
: B. Jacob. Arithmetic Timing Differences. URL: https://wiki.mozilla.org/User:Bjacob/ArithmeticTimingDifferences
**<a id="biblio-cmam"></a>[Cmam] **
: IEC. IEC 61966-2-1:1999 Colour measurement and management - Part 2-1: Colour management - Default RGB colour space - sRGB. URL: https://webstore.iec.ch/publication/6169
**<a id="biblio-css3-animations"></a>[CSS3-ANIMATIONS] **
: David Baron; et al. CSS Animations Level 1. URL: https://drafts.csswg.org/css-animations/
**<a id="biblio-css3color"></a>[CSS3COLOR] **
: Tantek Çelik; Chris Lilley; David Baron. CSS Color Module Level 3. URL: https://drafts.csswg.org/css-color-3/
**<a id="biblio-porterduff"></a>[PORTERDUFF] **
: Thomas Porter; Tom Duff. Compositing digital images.
**<a id="biblio-tam"></a>[TaM] **
: Ebert et al, AP Professional. Texturing and Modeling. 1994.
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order | Com­puted value |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [color-interpolation-filters](#propdef-color-interpolation-filters) | auto \| sRGB \| linearRGB | linearRGB | All filter primitives | yes | n/a | discrete | per grammar | as specified | visual |
| [filter](#propdef-filter) | none \| <filter-value-list> | none | All elements. In SVG, it applies to container elements without the defs element, all graphics elements and the use element. | no | n/a | See prose in Animation of Filters. | per grammar | as specified | visual |
| [flood-color](#propdef-flood-color) | <color> | black | feFlood and feDropShadow elements | no | n/a | by computed value | per grammar | as specified | visual |
| [flood-opacity](#propdef-flood-opacity) | <'opacity'> | 1 | feFlood and feDropShadow elements | no | n/a | by computed value | per grammar | the specified value converted to a number, clamped to the range [0,1] | visual |
| [lighting-color](#propdef-lighting-color) | <color> | white | feDiffuseLighting and feSpecularLighting elements | no | n/a | by computed value | per grammar | as specified | visual |
