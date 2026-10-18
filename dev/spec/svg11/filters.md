<!-- https://www.w3.org/TR/SVG11/filters.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](masking.md) ⋅ [Next](interact.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 15 Filter Effects

## <a id="Introduction"></a>15.1 Introduction

This chapter describes SVG's declarative filter effects feature set, which when combined with the 2D power of SVG can describe much of the common artwork on the Web in such a way that client-side generation and alteration can be performed easily. In addition, the ability to apply filter effects to SVG [graphics elements](intro.md#TermGraphicsElement) and [container elements](intro.md#TermContainerElement) helps to maintain the semantic structure of the document, instead of resorting to images which aside from generally being a fixed resolution tend to obscure the original semantics of the elements they replace. This is especially true for effects applied to text.

A filter effect consists of a series of graphics operations that are applied to a given <a id="TermSourceGraphic"></a>source graphic to produce a modified graphical result. The result of the filter effect is rendered to the target device instead of the original source graphic. The following illustrates the process:

[image: Image showing source graphic transformed by filter effect]

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/filters00.svg)

Filter effects are defined by [‘filter’](#FilterElement) elements. To apply a filter effect to a [graphics element](intro.md#TermGraphicsElement) or a [container element](intro.md#TermContainerElement), you set the value of the [‘filter’](#FilterProperty) property on the given element such that it references the filter effect.

Each [‘filter’](#FilterElement) element contains a set of <a id="TermFilterPrimitive"></a>filter primitives as its children. Each filter primitive performs a single fundamental graphical operation (e.g., a blur or a lighting effect) on one or more inputs, producing a graphical result. Because most of the filter primitives represent some form of image processing, in most cases the output from a filter primitive is a single RGBA image.

The original source graphic or the result from a filter primitive can be used as input into one or more other filter primitives. A common application is to use the source graphic multiple times. For example, a simple filter could replace one graphic by two by adding a black copy of original source graphic offset to create a drop shadow. In effect, there are now two layers of graphics, both with the same original source graphics.

When applied to [container elements](intro.md#TermContainerElement) such as [‘g’](struct.md#GElement), the [‘filter’](#FilterProperty) property applies to the contents of the group as a whole. The group's children do not render to the screen directly; instead, the graphics commands necessary to render the children are stored temporarily. Typically, the graphics commands are executed as part of the processing of the referenced [‘filter’](#FilterElement) element via use of the keywords [SourceGraphic](#SourceGraphic) or [SourceAlpha](#SourceAlpha). Filter effects can be applied to [container elements](intro.md#TermContainerElement) with no content (e.g., an empty [‘g’](struct.md#GElement) element), in which case the [SourceGraphic](#SourceGraphic) or [SourceAlpha](#SourceAlpha) consist of a transparent black rectangle that is the size of the [filter effects region](#FilterEffectsRegion).

Sometimes filter primitives result in undefined pixels. For example, filter primitive [‘feOffset’](#feOffsetElement) can shift an image down and to the right, leaving undefined pixels at the top and left. In these cases, the undefined pixels are set to transparent black.

## <a id="AnExample"></a>15.2 An example

The following shows an example of a filter effect.

Example filters01 - introducing filter effects.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
              "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="7.5cm" height="5cm" viewBox="0 0 200 120"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>Example filters01.svg - introducing filter effects</title>
  <desc>An example which combines multiple filter primitives
        to produce a 3D lighting effect on a graphic consisting
        of the string "SVG" sitting on top of oval filled in red
        and surrounded by an oval outlined in red.</desc>
  <defs>
    <filter id="MyFilter" filterUnits="userSpaceOnUse" x="0" y="0" width="200" height="120">
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
  </defs>
  <rect x="1" y="1" width="198" height="118" fill="#888888" stroke="blue" />
  <g filter="url(#MyFilter)" >
	  <g>
      <path fill="none" stroke="#D90000" stroke-width="10" 
            d="M50,90 C0,90 0,30 50,30 L150,30 C200,30 200,90 150,90 z" />
      <path fill="#D90000" 
            d="M60,80 C30,80 30,40 60,40 L140,40 C170,40 170,80 140,80 z" />
      <g fill="#FFFFFF" stroke="black" font-size="45" font-family="Verdana" >
        <text x="52" y="76">SVG</text>
      </g>
    </g>
  </g>
</svg>
```

Example filters01

| [image: Example filters01] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/filters01.svg)

The filter effect used in the example above is repeated here with reference numbers in the left column before each of the six filter primitives:

```
 
 
1
2
3
 
 
 
 
4
5
 
6
 
 
 
```

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

|  |  |
| --- | --- |

The following pictures show the intermediate image results from each of the six filter elements:

| [image: filters01 - original source graphic] Source graphic |  | [image: filters01 - after filter element 1] After filter primitive 1 |  | [image: filters01 - after filter element 2] After filter primitive 2 |  | [image: filters01 - after filter element 3] After filter primitive 3 |
| --- | --- | --- | --- | --- | --- | --- |
|  |  |  |  |  |  |  |
|  |  | [image: filters01 - after filter element 4] After filter primitive 4 |  | [image: filters01 - after filter element 5] After filter primitive 5 |  | [image: filters01 - after filter element 6] After filter primitive 6 |

1. Filter primitive [‘feGaussianBlur’](#feGaussianBlurElement) takes input [SourceAlpha](#SourceAlpha), which is the alpha channel of the source graphic. The result is stored in a temporary buffer named "blur". Note that "blur" is used as input to both filter primitives 2 and 3.
2. Filter primitive [‘feOffset’](#feOffsetElement) takes buffer "blur", shifts the result in a positive direction in both x and y, and creates a new buffer named "offsetBlur". The effect is that of a drop shadow.
3. Filter primitive [‘feSpecularLighting’](#feSpecularLightingElement), uses buffer "blur" as a model of a surface elevation and generates a lighting effect from a single point source. The result is stored in buffer "specOut".
4. Filter primitive [‘feComposite’](#feCompositeElement) masks out the result of filter primitive 3 by the original source graphics alpha channel so that the intermediate result is no bigger than the original source graphic.
5. Filter primitive [‘feComposite’](#feCompositeElement) composites the result of the specular lighting with the original source graphic.
6. Filter primitive [‘feMerge’](#feMergeElement) composites two layers together. The lower layer consists of the drop shadow result from filter primitive 2. The upper layer consists of the specular lighting result from filter primitive 5.

## <a id="FilterElement"></a>15.3 The ‘filter’ element

The description of the [‘filter’](#FilterElement) element follows:

‘filter’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [filter primitive elements](intro.md#TermFilterPrimitiveElement) — [‘feBlend’](#feBlendElement), [‘feColorMatrix’](#feColorMatrixElement), [‘feComponentTransfer’](#feComponentTransferElement), [‘feComposite’](#feCompositeElement), [‘feConvolveMatrix’](#feConvolveMatrixElement), [‘feDiffuseLighting’](#feDiffuseLightingElement), [‘feDisplacementMap’](#feDisplacementMapElement), [‘feFlood’](#feFloodElement), [‘feGaussianBlur’](#feGaussianBlurElement), [‘feImage’](#feImageElement), [‘feMerge’](#feMergeElement), [‘feMorphology’](#feMorphologyElement), [‘feOffset’](#feOffsetElement), [‘feSpecularLighting’](#feSpecularLightingElement), [‘feTile’](#feTileElement), [‘feTurbulence’](#feTurbulenceElement)
- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#FilterElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘x’](#FilterElementXAttribute)
- [‘y’](#FilterElementYAttribute)
- [‘width’](#FilterElementWidthAttribute)
- [‘height’](#FilterElementHeightAttribute)
- [‘filterRes’](#FilterElementFilterResAttribute)
- [‘filterUnits’](#FilterElementFilterUnitsAttribute)
- [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute)
- [‘xlink:href’](#FilterElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGFilterElement](#InterfaceSVGFilterElement)

*Attribute definitions:*

**<a id="FilterElementFilterUnitsAttribute"></a>filterUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: See [Filter effects region](#FilterEffectsRegion).
**<a id="FilterElementPrimitiveUnitsAttribute"></a>primitiveUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Specifies the coordinate system for the various length values within the filter primitives and for the attributes that define the [filter primitive subregion](#FilterPrimitiveSubRegion). If primitiveUnits="userSpaceOnUse", any length values within the filter definitions represent values in the current user coordinate system in place at the time when the [‘filter’](#FilterElement) element is referenced (i.e., the user coordinate system for the element referencing the [‘filter’](#FilterElement) element via a [‘filter’](#FilterProperty) property). If primitiveUnits="objectBoundingBox", then any length values within the filter definitions represent fractions or percentages of the bounding box on the referencing element (see [Object bounding box units](coords.md#ObjectBoundingBox)). Note that if only one number was specified in a [<number-optional-number>](types.md#DataTypeNumberOptionalNumber) value this number is expanded out before the [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) computation takes place. If attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) is not specified, then the effect is as if a value of userSpaceOnUse were specified. [Animatable](animate.md#Animatable): yes.
**<a id="FilterElementXAttribute"></a>x = "*[<coordinate>](types.md#DataTypeCoordinate)*"**
: See [Filter effects region](#FilterEffectsRegion).
**<a id="FilterElementYAttribute"></a>y = "*[<coordinate>](types.md#DataTypeCoordinate)*"**
: See [Filter effects region](#FilterEffectsRegion).
**<a id="FilterElementWidthAttribute"></a>width = "*[<length>](types.md#DataTypeLength)*"**
: See [Filter effects region](#FilterEffectsRegion).
**<a id="FilterElementHeightAttribute"></a>height = "*[<length>](types.md#DataTypeLength)*"**
: See [Filter effects region](#FilterEffectsRegion).
**<a id="FilterElementFilterResAttribute"></a>filterRes = "*[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)*"**
: See [Filter effects region](#FilterEffectsRegion).
**<a id="FilterElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) to another [‘filter’](#FilterElement) element within the current SVG document fragment. Any attributes which are defined on the referenced [‘filter’](#FilterElement) element which are not defined on this element are inherited by this element. If this element has no defined filter nodes, and the referenced element has defined filter nodes (possibly due to its own [‘xlink:href’](#FilterElementHrefAttribute) attribute), then this element inherits the filter nodes defined from the referenced [‘filter’](#FilterElement) element. Inheritance can be indirect to an arbitrary level; thus, if the referenced [‘filter’](#FilterElement) element inherits attributes or its filter node specification due to its own [‘xlink:href’](#FilterElementHrefAttribute) attribute, then the current element can inherit those attributes or filter node specifications. [Animatable](animate.md#Animatable): yes.
[Properties](styling.md#SVGStylingProperties) inherit into the [‘filter’](#FilterElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘filter’](#FilterElement) element.

[‘filter’](#FilterElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘filter’](#FilterProperty) property. The [‘display’](painting.md#DisplayProperty) property does not apply to the [‘filter’](#FilterElement) element; thus, [‘filter’](#FilterElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘filter’](#FilterElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘filter’](#FilterElement) element or any of its ancestors is set to none.

## <a id="FilterProperty"></a>15.4 The ‘filter’ property

The description of the [‘filter’](#FilterProperty) property is as follows:

**‘filter’**
:

| *Value:* | [<funciri>](types.md#DataTypeFuncIRI) \| none \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [container elements](intro.md#TermContainerElement) (except [‘mask’](masking.md#MaskElement)) and [graphics elements](intro.md#TermGraphicsElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**<funciri>**
: An [Functional IRI reference](types.md#DataTypeFuncIRI) to a [‘filter’](#FilterElement) element which defines the filter effects that shall be applied to this element.
**none**
: Do not apply any filter effects to this element.
## <a id="FilterEffectsRegion"></a>15.5 Filter effects region

A [‘filter’](#FilterElement) element can define a region on the canvas to which a given filter effect applies and can provide a resolution for any intermediate continuous tone images used to process any raster-based filter primitives. The [‘filter’](#FilterElement) element has the following attributes which work together to define the filter effects region:

**<a id="FilterUnitsAttribute"></a>[‘filterUnits’](#FilterElementFilterUnitsAttribute)**
:

Defines the coordinate system for attributes [‘x’](#FilterElementXAttribute), [‘y’](#FilterElementYAttribute), [‘width’](#FilterElementWidthAttribute) and [‘height’](#FilterElementHeightAttribute).

If filterUnits="userSpaceOnUse", [‘x’](#FilterElementXAttribute), [‘y’](#FilterElementYAttribute), [‘width’](#FilterElementWidthAttribute) and [‘height’](#FilterElementHeightAttribute) represent values in the current user coordinate system in place at the time when the [‘filter’](#FilterElement) is referenced (i.e., the user coordinate system for the element referencing the [‘filter’](#FilterElement) via a [‘filter’](#FilterProperty) property).

If filterUnits="objectBoundingBox", then [‘x’](#FilterElementXAttribute), [‘y’](#FilterElementYAttribute), [‘width’](#FilterElementWidthAttribute) and [‘height’](#FilterElementHeightAttribute) represent fractions or percentages of the bounding box on the referencing element (see [Object bounding box units](coords.md#ObjectBoundingBox)).

If attribute [‘filterUnits’](#FilterElementFilterUnitsAttribute) is not specified, then the effect is if a value of 'objectBoundingBox' were specified.

[Animatable](animate.md#Animatable): yes.

**<a id="FilterRegionXYWidthHeightAttributes"></a>[‘x’](#FilterElementXAttribute), [‘y’](#FilterElementYAttribute), [‘width’](#FilterElementWidthAttribute) and [‘height’](#FilterElementHeightAttribute)**
:

These attributes define a rectangular region on the canvas to which this filter applies.

The amount of memory and processing time required to apply the filter are related to the size of this rectangle and the [‘filterRes’](#FilterElementFilterResAttribute) attribute of the filter.

The coordinate system for these attributes depends on the value for attribute [‘filterUnits’](#FilterElementFilterUnitsAttribute).

Negative values for [‘width’](#FilterElementWidthAttribute) or [‘height’](#FilterElementHeightAttribute) are an error (see [Error processing](implnote.md#ErrorProcessing)). Zero values disable rendering of the element which referenced the filter.

The bounds of this rectangle act as a hard clipping region for each [filter primitive](#FilterPrimitivesOverview) included with a given [‘filter’](#FilterElement) element; thus, if the effect of a given filter primitive would extend beyond the bounds of the rectangle (this sometimes happens when using a [‘feGaussianBlur’](#feGaussianBlurElement) filter primitive with a very large [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute)), parts of the effect will get clipped.

If [‘x’](#FilterElementXAttribute) or [‘y’](#FilterElementYAttribute) is not specified, the effect is as if a value of -10% were specified.

If [‘width’](#FilterElementWidthAttribute) or [‘height’](#FilterElementHeightAttribute) is not specified, the effect is as if a value of 120% were specified.

[Animatable](animate.md#Animatable): yes.

**[‘filterRes’](#FilterElementFilterResAttribute)**
:

This attribute takes the form `x-pixels [y-pixels]`, and indicates the width and height of the intermediate images in pixels. If not provided, then the user agent will use reasonable values to produce a high-quality result on the output device.

Care should be taken when assigning a non-default value to this attribute. Too small of a value may result in unwanted pixelation in the result. Too large of a value may result in slow processing and large memory usage.

Negative values are an error (see [Error processing](implnote.md#ErrorProcessing)). Zero values disable rendering of the element which referenced the filter.

Non-integer values are truncated, i.e rounded to the closest integer value towards zero.

[Animatable](animate.md#Animatable): yes.

Note that both of the two possible value for [‘filterUnits’](#FilterElementFilterUnitsAttribute) (i.e., 'objectBoundingBox' and 'userSpaceOnUse') result in a filter region whose coordinate system has its X-axis and Y-axis each parallel to the X-axis and Y-axis, respectively, of the user coordinate system for the element to which the filter will be applied.

Sometimes implementers can achieve faster performance when the [filter region](#FilterEffectsRegion) can be mapped directly to device pixels; thus, for best performance on display devices, it is suggested that authors define their region such that SVG user agent can align the [filter region](#FilterEffectsRegion) pixel-for-pixel with the background. In particular, for best filter effects performance, avoid rotating or skewing the user coordinate system. Explicit values for attribute [‘filterRes’](#FilterElementFilterResAttribute) can either help or harm performance. If [‘filterRes’](#FilterElementFilterResAttribute) is smaller than the automatic (i.e., default) filter resolution, then filter effect might have faster performance (usually at the expense of quality). If [‘filterRes’](#FilterElementFilterResAttribute) is larger than the automatic (i.e., default) filter resolution, then filter effects performance will usually be slower.

It is often necessary to provide padding space because the filter effect might impact bits slightly outside the tight-fitting bounding box on a given object. For these purposes, it is possible to provide negative percentage values for [‘x’](#FilterElementXAttribute) and [‘y’](#FilterElementYAttribute), and percentages values greater than 100% for [‘width’](#FilterElementWidthAttribute) and [‘height’](#FilterElementHeightAttribute). This, for example, is why the defaults for the filter effects region are x="-10%" y="-10%" width="120%" height="120%".

## <a id="AccessingBackgroundImage"></a>15.6 Accessing the background image

Two possible pseudo input images for filter effects are [BackgroundImage](#BackgroundImage) and [BackgroundAlpha](#BackgroundAlpha), which each represent an image snapshot of the canvas under the filter region at the time that the [‘filter’](#FilterElement) element is invoked. [BackgroundImage](#BackgroundImage) represents both the color values and alpha channel of the canvas (i.e., RGBA pixel values), whereas [BackgroundAlpha](#BackgroundAlpha) represents only the alpha channel.

Implementations of SVG user agents often will need to maintain supplemental background image buffers in order to support the [BackgroundImage](#BackgroundImage) and [BackgroundAlpha](#BackgroundAlpha) pseudo input images. Sometimes, the background image buffers will contain an in-memory copy of the accumulated painting operations on the current canvas.

Because in-memory image buffers can take up significant system resources, SVG content must explicitly indicate to the SVG user agent that the document needs access to the background image before [BackgroundImage](#BackgroundImage) and [BackgroundAlpha](#BackgroundAlpha) pseudo input images can be used. The property which enables access to the background image is [‘enable-background’](#EnableBackgroundProperty), defined below:

**<a id="EnableBackgroundProperty"></a>‘enable-background’**
:

| *Value:* | accumulate \| new [ <x> <y> <width> <height> ] \| inherit |
| --- | --- |
| *Initial:* | accumulate |
| *Applies to:* | [container elements](intro.md#TermContainerElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

[‘enable-background’](#EnableBackgroundProperty) is only applicable to [container elements](intro.md#TermContainerElement) and specifies how the SVG user agents manages the accumulation of the background image.

A value of new indicates two things:

- It enables the ability of children of the current [container element](intro.md#TermContainerElement) to access the background image.
- It indicates that a new (i.e., initially transparent black) background image canvas is established and that (in effect) all children of the current [container element](intro.md#TermContainerElement) shall be rendered into the new background image canvas in addition to being rendered onto the target device.

A meaning of enable-background: accumulate (the initial/default value) depends on context:

- If an ancestor [container element](intro.md#TermContainerElement) has a property value of enable-background: new, then all [graphics elements](intro.md#TermGraphicsElement) within the current [container element](intro.md#TermContainerElement) are rendered both onto the parent [container element](intro.md#TermContainerElement)'s background image canvas and onto the target device.
- Otherwise, there is no current background image canvas, so it is only necessary to render [graphics elements](intro.md#TermGraphicsElement) onto the target device. (No need to render to the background image canvas.)

If a filter effect specifies either the [BackgroundImage](#BackgroundImage) or the [BackgroundAlpha](#BackgroundAlpha) pseudo input images and no ancestor [container element](intro.md#TermContainerElement) has a property value of enable-background: new, then the background image request is technically in error. Processing will proceed without interruption (i.e., no error message) and a transparent black image shall be provided in response to the request.

The optional <x>,<y>,<width>,<height> parameters on the new value are [<number>](types.md#DataTypeNumber) values that indicate the subregion of the [container element](intro.md#TermContainerElement)'s [user space](intro.md#TermUserSpace) where access to the background image is allowed to happen. These parameters enable the SVG user agent potentially to allocate smaller temporary image buffers than the default values. Thus, the values <x>,<y>,<width>,<height> act as a clipping rectangle on the background image canvas. Negative values for <width> or <height> are an error (see [Error processing](implnote.md#ErrorProcessing)). If more than zero but less than four of the values <x>,<y>,<width> and <height> are specified or if zero values are specified for <width> or <height>, [BackgroundImage](#BackgroundImage) and [BackgroundAlpha](#BackgroundAlpha) are processed as if background image processing were not enabled.

Assume you have an element E in the document and that E has a series of ancestors A_1 (its immediate parent), A_2, etc. (Note: A_0 is E.) Each ancestor A_i will have a corresponding temporary background image offscreen buffer BUF_i. The contents of the *background image* available to a [‘filter’](#FilterElement) referenced by E is defined as follows:

- Find the element A_i with the smallest subscript i (including A_0=E) for which the [‘enable-background’](#EnableBackgroundProperty) property has the value new. (Note: if there is no such ancestor element, then there is no background image available to E, in which case a transparent black image will be used as E's background image.)
- For each A_i (from i=n to 1), initialize BUF_i to transparent black. Render all children of A_i up to but not including A_i-1 into BUF_i. The children are painted, then filtered, clipped, masked and composited using the various painting, filtering, clipping, masking and object opacity settings on the given child. Any filter effects, masking and group opacity that might be set on A_i do *not* apply when rendering the children of A_i into BUF_i. (Note that for the case of A_0=E, the graphical contents of E are not rendered into BUF_1 and thus are not part of the background image available to E. Instead, the graphical contents of E are available via the [SourceGraphic](#SourceGraphic) and [SourceAlpha](#SourceAlpha) pseudo input images.)
- Then, for each A_i (from i=1 to n-1), composite BUF_i into BUF_i+1.
- The accumulated result (i.e., BUF_n) represents the background image available to E.

Example enable-background-01 illustrates the rules for background image processing.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="13.5cm" height="2.7cm" viewBox="0 0 1350 270"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>Example enable-background01</title>
  <desc>This test case shows five pictures which illustrate the rules
        for background image processing.</desc>

  <defs>
    <filter id="ShiftBGAndBlur" 
            filterUnits="userSpaceOnUse" x="0" y="0" width="1200" height="400">
      <desc>
        This filter discards the SourceGraphic, if any, and just produces
        a result consisting of the BackgroundImage shifted down 125 units
        and then blurred.
      </desc>
      <feOffset in="BackgroundImage" dx="0" dy="125" />
      <feGaussianBlur stdDeviation="8" />
    </filter>
    <filter id="ShiftBGAndBlur_WithSourceGraphic" 
            filterUnits="userSpaceOnUse" x="0" y="0" width="1200" height="400">
      <desc>
        This filter takes the BackgroundImage, shifts it down 125 units, blurs it,
        and then renders the SourceGraphic on top of the shifted/blurred background.
      </desc>
      <feOffset in="BackgroundImage" dx="0" dy="125" />
      <feGaussianBlur stdDeviation="8" result="blur" />
      <feMerge>
        <feMergeNode in="blur"/>
        <feMergeNode in="SourceGraphic"/>
      </feMerge>
    </filter>
  </defs>

  <g transform="translate(0,0)">
    <desc>The first picture is our reference graphic without filters.</desc>
    <rect x="25" y="25" width="100" height="100" fill="red"/>
    <g opacity=".5">
      <circle cx="125" cy="75" r="45" fill="green"/>
      <polygon points="160,25 160,125 240,75" fill="blue"/>
    </g>
    <rect x="5" y="5" width="260" height="260" fill="none" stroke="blue"/>
  </g>

  <g enable-background="new" transform="translate(270,0)">
    <desc>The second adds an empty 'g' element which invokes ShiftBGAndBlur.</desc>
    <rect x="25" y="25" width="100" height="100" fill="red"/>
    <g opacity=".5">
      <circle cx="125" cy="75" r="45" fill="green"/>
      <polygon points="160,25 160,125 240,75" fill="blue"/>
    </g>
    <g filter="url(#ShiftBGAndBlur)"/>
    <rect x="5" y="5" width="260" height="260" fill="none" stroke="blue"/>
  </g>

  <g enable-background="new" transform="translate(540,0)">
    <desc>The third invokes ShiftBGAndBlur on the inner group.</desc>
    <rect x="25" y="25" width="100" height="100" fill="red"/>
    <g filter="url(#ShiftBGAndBlur)" opacity=".5">
      <circle cx="125" cy="75" r="45" fill="green"/>
      <polygon points="160,25 160,125 240,75" fill="blue"/>
    </g>
    <rect x="5" y="5" width="260" height="260" fill="none" stroke="blue"/>
  </g>

  <g enable-background="new" transform="translate(810,0)">
    <desc>The fourth invokes ShiftBGAndBlur on the triangle.</desc>
    <rect x="25" y="25" width="100" height="100" fill="red"/>
    <g opacity=".5">
      <circle cx="125" cy="75" r="45" fill="green"/>
      <polygon points="160,25 160,125 240,75" fill="blue"
               filter="url(#ShiftBGAndBlur)"/>
    </g>
    <rect x="5" y="5" width="260" height="260" fill="none" stroke="blue"/>
  </g>

  <g enable-background="new" transform="translate(1080,0)">
    <desc>The fifth invokes ShiftBGAndBlur_WithSourceGraphic on the triangle.</desc>
    <rect x="25" y="25" width="100" height="100" fill="red"/>
    <g opacity=".5">
      <circle cx="125" cy="75" r="45" fill="green"/>
      <polygon points="160,25 160,125 240,75" fill="blue"
               filter="url(#ShiftBGAndBlur_WithSourceGraphic)"/>
    </g>
    <rect x="5" y="5" width="260" height="260" fill="none" stroke="blue"/>
  </g>
</svg>
```

Example enable-background-01

| [image: Example enable-background-01 — illustrate the rules for background image processing] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/enable-background-01.svg)

The example above contains five parts, described as follows:

1. The first set is the reference graphic. The reference graphic consists of a red rectangle followed by a 50% transparent [‘g’](struct.md#GElement) element. Inside the [‘g’](struct.md#GElement) is a green circle that partially overlaps the rectangle and a a blue triangle that partially overlaps the circle. The three objects are then outlined by a rectangle stroked with a thin blue line. No filters are applied to the reference graphic.
2. The second set enables background image processing and adds an empty [‘g’](struct.md#GElement) element which invokes the ShiftBGAndBlur filter. This filter takes the current accumulated background image (i.e., the entire reference graphic) as input, shifts its offscreen down, blurs it, and then writes the result to the canvas. Note that the offscreen for the filter is initialized to transparent black, which allows the already rendered rectangle, circle and triangle to show through after the filter renders its own result to the canvas.
3. The third set enables background image processing and instead invokes the ShiftBGAndBlur filter on the inner [‘g’](struct.md#GElement) element. The accumulated background at the time the filter is applied contains only the red rectangle. Because the children of the inner [‘g’](struct.md#GElement) (i.e., the circle and triangle) are not part of the inner [‘g’](struct.md#GElement) element's background and because ShiftBGAndBlur ignores SourceGraphic, the children of the inner [‘g’](struct.md#GElement) do not appear in the result.
4. The fourth set enables background image processing and invokes the ShiftBGAndBlur on the [‘polygon’](shapes.md#PolygonElement) element that draws the triangle. The accumulated background at the time the filter is applied contains the red rectangle plus the green circle ignoring the effect of the [‘opacity’](masking.md#OpacityProperty) property on the inner [‘g’](struct.md#GElement) element. (Note that the blurred green circle at the bottom does not let the red rectangle show through on its left side. This is due to ignoring the effect of the [‘opacity’](masking.md#OpacityProperty) property.) Because the triangle itself is not part of the accumulated background and because ShiftBGAndBlur ignores SourceGraphic, the triangle does not appear in the result.
5. The fifth set is the same as the fourth except that filter ShiftBGAndBlur_WithSourceGraphic is invoked instead of ShiftBGAndBlur. ShiftBGAndBlur_WithSourceGraphic performs the same effect as ShiftBGAndBlur, but then renders the SourceGraphic on top of the shifted, blurred background image. In this case, SourceGraphic is the blue triangle; thus, the result is the same as in the fourth case except that the blue triangle now appears.

## <a id="FilterPrimitivesOverview"></a>15.7 Filter primitives overview

### <a id="FilterPrimitivesOverviewIntro"></a>15.7.1 Overview

This section describes the various filter primtives that can be assembled to achieve a particular filter effect.

Unless otherwise stated, all image filters operate on premultiplied RGBA samples. Filters which work more naturally on non-premultiplied data (feColorMatrix and feComponentTransfer) will temporarily undo and redo premultiplication as specified. All raster effect filtering operations take 1 to N input RGBA images, additional attributes as parameters, and produce a single output RGBA image.

The RGBA result from each filter primitive will be clamped into the allowable ranges for colors and opacity values. Thus, for example, the result from a given filter primitive will have any negative color values or opacity values adjusted up to color/opacity of zero.

The color space in which a particular filter primitive performs its operations is determined by the value of property [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty) on the given filter primitive. A different property, [‘color-interpolation’](painting.md#ColorInterpolationProperty) determines the color space for other color operations. Because these two properties have different initial values ([‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty) has an initial value of linearRGB whereas [‘color-interpolation’](painting.md#ColorInterpolationProperty) has an initial value of sRGB), in some cases to achieve certain results (e.g., when coordinating gradient interpolation with a filtering operation) it will be necessary to explicitly set [‘color-interpolation’](painting.md#ColorInterpolationProperty) to linearRGB or [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty) to sRGB on particular elements. Note that the examples below do not explicitly set either [‘color-interpolation’](painting.md#ColorInterpolationProperty) or [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), so the initial values for these properties apply to the examples.

### <a id="CommonAttributes"></a>15.7.2 Common attributes

With the exception of the [‘in’](#FilterPrimitiveInAttribute) attribute, all of the following attributes are available on all filter primitive elements:

*Attribute definitions:*

**<a id="FilterPrimitiveXAttribute"></a>x = "*[<coordinate>](types.md#DataTypeCoordinate)*"**
: The minimum x coordinate for the subregion which restricts calculation and rendering of the given filter primitive. See [filter primitive subregion](#FilterPrimitiveSubRegion). [Animatable](animate.md#Animatable): yes.
**<a id="FilterPrimitiveYAttribute"></a>y = "*[<coordinate>](types.md#DataTypeCoordinate)*"**
: The minimum y coordinate for the subregion which restricts calculation and rendering of the given filter primitive. See [filter primitive subregion](#FilterPrimitiveSubRegion). [Animatable](animate.md#Animatable): yes.
**<a id="FilterPrimitiveWidthAttribute"></a>width = "*[<length>](types.md#DataTypeLength)*"**
: The width of the subregion which restricts calculation and rendering of the given filter primitive. See [filter primitive subregion](#FilterPrimitiveSubRegion). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables the effect of the given filter primitive (i.e., the result is a transparent black image). [Animatable](animate.md#Animatable): yes.
**<a id="FilterPrimitiveHeightAttribute"></a>height = "*[<length>](types.md#DataTypeLength)*"**
: The height of the subregion which restricts calculation and rendering of the given filter primitive. See [filter primitive subregion](#FilterPrimitiveSubRegion). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables the effect of the given filter primitive (i.e., the result is a transparent black image). [Animatable](animate.md#Animatable): yes.
**<a id="FilterPrimitiveResultAttribute"></a>result = "*<filter-primitive-reference>*"**
: Assigned name for this filter primitive. If supplied, then graphics that result from processing this filter primitive can be referenced by an [‘in’](#FilterPrimitiveInAttribute) attribute on a subsequent filter primitive within the same [‘filter’](#FilterElement) element. If no value is provided, the output will only be available for re-use as the implicit input into the next filter primitive if that filter primitive provides no value for its [‘in’](#FilterPrimitiveInAttribute) attribute. Note that a *<filter-primitive-reference>* is not an XML ID; instead, a *<filter-primitive-reference>* is only meaningful within a given [‘filter’](#FilterElement) element and thus have only local scope. It is legal for the same *<filter-primitive-reference>* to appear multiple times within the same [‘filter’](#FilterElement) element. When referenced, the *<filter-primitive-reference>* will use the closest preceding filter primitive with the given result. [Animatable](animate.md#Animatable): yes.
**<a id="FilterPrimitiveInAttribute"></a>in = "*[SourceGraphic](#SourceGraphic) | [SourceAlpha](#SourceAlpha) | [BackgroundImage](#BackgroundImage) | [BackgroundAlpha](#BackgroundAlpha) | [FillPaint](#FillPaint) | [StrokePaint](#StrokePaint) | <filter-primitive-reference>*"**
: Identifies input for the given filter primitive. The value can be either one of six keywords or can be a string which matches a previous [‘result’](#FilterPrimitiveResultAttribute) attribute value within the same [‘filter’](#FilterElement) element. If no value is provided and this is the first filter primitive, then this filter primitive will use [SourceGraphic](#SourceGraphic) as its input. If no value is provided and this is a subsequent filter primitive, then this filter primitive will use the result from the previous filter primitive as its input. If the value for [‘result’](#FilterPrimitiveResultAttribute) appears multiple times within a given [‘filter’](#FilterElement) element, then a reference to that result will use the closest preceding filter primitive with the given value for attribute [‘result’](#FilterPrimitiveResultAttribute). Forward references to results are [an error](implnote.md#ErrorProcessing). Definitions for the six keywords:

**<a id="SourceGraphic"></a>SourceGraphic**
: This keyword represents the [graphics elements](intro.md#TermGraphicsElement) that were the original input into the [‘filter’](#FilterElement) element. For raster effects filter primitives, the [graphics elements](intro.md#TermGraphicsElement) will be rasterized into an initially clear RGBA raster in image space. Pixels left untouched by the original graphic will be left clear. The image is specified to be rendered in linear RGBA pixels. The alpha channel of this image captures any anti-aliasing specified by SVG. (Since the raster is linear, the alpha channel of this image will represent the exact percent coverage of each pixel.)
**<a id="SourceAlpha"></a>SourceAlpha**
: This keyword represents the [graphics elements](intro.md#TermGraphicsElement) that were the original input into the [‘filter’](#FilterElement) element. SourceAlpha has all of the same rules as [SourceGraphic](#SourceGraphic) except that only the alpha channel is used. The input image is an RGBA image consisting of implicitly black color values for the RGB channels, but whose alpha channel is the same as [SourceGraphic](#SourceGraphic). If this option is used, then some implementations might need to rasterize the [graphics elements](intro.md#TermGraphicsElement) in order to extract the alpha channel.
**<a id="BackgroundImage"></a>BackgroundImage**
: This keyword represents an image snapshot of the canvas under the [filter region](#FilterEffectsRegion) at the time that the [‘filter’](#FilterElement) element was invoked. See [Accessing the background image](#AccessingBackgroundImage).
**<a id="BackgroundAlpha"></a>BackgroundAlpha**
: Same as [BackgroundImage](#BackgroundImage) except only the alpha channel is used. See [SourceAlpha](#SourceAlpha) and [Accessing the background image](#AccessingBackgroundImage).
**<a id="FillPaint"></a>FillPaint**
: This keyword represents the value of the [‘fill’](painting.md#FillProperty) property on the target element for the filter effect. The FillPaint image has conceptually infinite extent. Frequently this image is opaque everywhere, but it might not be if the "paint" itself has alpha, as in the case of a gradient or pattern which itself includes transparent or semi-transparent parts.
**<a id="StrokePaint"></a>StrokePaint**
: This keyword represents the value of the [‘stroke’](painting.md#StrokeProperty) property on the target element for the filter effect. The StrokePaint image has conceptually infinite extent. Frequently this image is opaque everywhere, but it might not be if the "paint" itself has alpha, as in the case of a gradient or pattern which itself includes transparent or semi-transparent parts.
The [‘in’](#FilterPrimitiveInAttribute) attribute is available on all filter primitive elements that require an input.

[Animatable](animate.md#Animatable): yes.
### <a id="FilterPrimitiveSubRegion"></a>15.7.3 Filter primitive subregion

All filter primitives have attributes [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) which identify a subregion which restricts calculation and rendering of the given filter primitive. These attributes are defined according to the same rules as other filter primitives' coordinate and length attributes and thus represent values in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element.

[‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) default to the union (i.e., tightest fitting bounding box) of the subregions defined for all referenced nodes. If there are no referenced nodes (e.g., for [‘feImage’](#feImageElement) or [‘feTurbulence’](#feTurbulenceElement)), or one or more of the referenced nodes is a standard input (one of [SourceGraphic](#SourceGraphic), [SourceAlpha](#SourceAlpha), [BackgroundImage](#BackgroundImage), [BackgroundAlpha](#BackgroundAlpha), [FillPaint](#FillPaint) or [StrokePaint](#StrokePaint)), or for [‘feTile’](#feTileElement) (which is special because its principal function is to replicate the referenced node in X and Y and thereby produce a usually larger result), the default subregion is 0%,0%,100%,100%, where as a special-case the percentages are relative to the dimensions of the [filter region](#FilterEffectsRegion), thus making the the default [filter primitive subregion](#FilterPrimitiveSubRegion) equal to the [filter region](#FilterEffectsRegion).

[‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) act as a hard clip clipping rectangle on both the filter primitive's input image(s) and the filter primitive result.

All intermediate offscreens are defined to not exceed the intersection of [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) with the [filter region](#FilterEffectsRegion). The [filter region](#FilterEffectsRegion) and any of the [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) subregions are to be set up such that all offscreens are made big enough to accommodate any pixels which even partly intersect with either the [filter region](#FilterEffectsRegion) or the x,y,width,height subregions.

[‘feTile’](#feTileElement) references a previous filter primitive and then stitches the tiles together based on the [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) values of the referenced filter primitive in order to fill its own [filter primitive subregion](#FilterPrimitiveSubRegion).

Example primitive-subregion-01 demonstrates the effect of specifying a filter primitive subregion:

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

Example primitive-subregion-01

| [image: Example primitive-subregion-01] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/primitive-subregion-01.svg)

In the example above there are three rects that each have a cross and a circle in them. The circle element in each one has a different filter applied, but with the same [filter primitive subregion](#FilterPrimitiveSubRegion). The filter output should be limited to the [filter primitive subregion](#FilterPrimitiveSubRegion), so you should never see the circles themselves, just the rects that make up the [filter primitive subregion](#FilterPrimitiveSubRegion).

- The upper left rect shows an [‘feFlood’](#feFloodElement) with flood-opacity="75%" so the cross should be visible through the green rect in the middle.
- The lower left rect shows an [‘feMerge’](#feMergeElement) that merges SourceGraphic with FillPaint. Since the circle has fill-opacity="0.5" it will also be transparent so that the cross is visible through the green rect in the middle.
- The upper right rect shows an [‘feBlend’](#feBlendElement) that has mode="multiply". Since the circle in this case isn't transparent the result is totally opaque. The rect should be dark green and the cross should not be visible through it.

## <a id="LightSourceDefinitions"></a>15.8 Light source elements and properties

### <a id="LightSourceIntro"></a>15.8.1 Introduction

The following sections define the elements that define a light source, [‘feDistantLight’](#feDistantLightElement), [‘fePointLight’](#fePointLightElement) and [‘feSpotLight’](#feSpotLightElement), and property [‘lighting-color’](#LightingColorProperty), which defines the color of the light.

### <a id="feDistantLightElement"></a>15.8.2 Light source ‘feDistantLight’

‘feDistantLight’

**Categories:**
: [Light source element](intro.md#TermLightSourceElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘azimuth’](#feDistantLightAzimuthAttribute)
- [‘elevation’](#feDistantLightElevationAttribute)

**DOM Interfaces:**
:

- [SVGFEDistantLightElement](#InterfaceSVGFEDistantLightElement)

*Attribute definitions:*

**<a id="feDistantLightAzimuthAttribute"></a>azimuth = "*[<number>](types.md#DataTypeNumber)*"**
: Direction angle for the light source on the XY plane (clockwise), in degrees from the x axis. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feDistantLightElevationAttribute"></a>elevation = "*[<number>](types.md#DataTypeNumber)*"**
: Direction angle for the light source from the XY plane towards the z axis, in degrees. Note the positive Z-axis points towards the viewer of the content. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
The following diagram illustrates the angles which [‘azimuth’](#feDistantLightAzimuthAttribute) and [‘elevation’](#feDistantLightElevationAttribute) represent in an XYZ coordinate system.

[image: Angles which azimuth and elevation represent]

### <a id="fePointLightElement"></a>15.8.3 Light source ‘fePointLight’

‘fePointLight’

**Categories:**
: [Light source element](intro.md#TermLightSourceElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘x’](#fePointLightXAttribute)
- [‘y’](#fePointLightYAttribute)
- [‘z’](#fePointLightZAttribute)

**DOM Interfaces:**
:

- [SVGFEPointLightElement](#InterfaceSVGFEPointLightElement)

*Attribute definitions:*

**<a id="fePointLightXAttribute"></a>x = "*[<number>](types.md#DataTypeNumber)*"**
: X location for the light source in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="fePointLightYAttribute"></a>y = "*[<number>](types.md#DataTypeNumber)*"**
: Y location for the light source in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="fePointLightZAttribute"></a>z = "*[<number>](types.md#DataTypeNumber)*"**
: Z location for the light source in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element, assuming that, in the [initial coordinate system](coords.md#InitialCoordinateSystem), the positive Z-axis comes out towards the person viewing the content and assuming that one unit along the Z-axis equals [one unit in X and Y](coords.md#Units_viewport_percentage). If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
### <a id="feSpotLightElement"></a>15.8.4 Light source ‘feSpotLight’

‘feSpotLight’

**Categories:**
: [Light source element](intro.md#TermLightSourceElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘x’](#feSpotLightXAttribute)
- [‘y’](#feSpotLightYAttribute)
- [‘z’](#feSpotLightZAttribute)
- [‘pointsAtX’](#feSpotLightPointsAtXAttribute)
- [‘pointsAtY’](#feSpotLightPointsAtYAttribute)
- [‘pointsAtZ’](#feSpotLightPointsAtZAttribute)
- [‘specularExponent’](#feSpotLightSpecularExponentAttribute)
- [‘limitingConeAngle’](#feSpotLightLimitingConeAngleAttribute)

**DOM Interfaces:**
:

- [SVGFESpotLightElement](#InterfaceSVGFESpotLightElement)

*Attribute definitions:*

**<a id="feSpotLightXAttribute"></a>x = "*[<number>](types.md#DataTypeNumber)*"**
: X location for the light source in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightYAttribute"></a>y = "*[<number>](types.md#DataTypeNumber)*"**
: Y location for the light source in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightZAttribute"></a>z = "*[<number>](types.md#DataTypeNumber)*"**
: Z location for the light source in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element, assuming that, in the [initial coordinate system](coords.md#InitialCoordinateSystem), the positive Z-axis comes out towards the person viewing the content and assuming that one unit along the Z-axis equals [one unit in X and Y](coords.md#Units_viewport_percentage). If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightPointsAtXAttribute"></a>pointsAtX = "*[<number>](types.md#DataTypeNumber)*"**
: X location in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element of the point at which the light source is pointing. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightPointsAtYAttribute"></a>pointsAtY = "*[<number>](types.md#DataTypeNumber)*"**
: Y location in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element of the point at which the light source is pointing. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightPointsAtZAttribute"></a>pointsAtZ = "*[<number>](types.md#DataTypeNumber)*"**
: Z location in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element of the point at which the light source is pointing, assuming that, in the [initial coordinate system](coords.md#InitialCoordinateSystem), the positive Z-axis comes out towards the person viewing the content and assuming that one unit along the Z-axis equals [one unit in X and Y](coords.md#Units_viewport_percentage). If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightSpecularExponentAttribute"></a>specularExponent = "*[<number>](types.md#DataTypeNumber)*"**
: Exponent value controlling the focus for the light source. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpotLightLimitingConeAngleAttribute"></a>limitingConeAngle = "*[<number>](types.md#DataTypeNumber)*"**
: A limiting cone which restricts the region where the light is projected. No light is projected outside the cone. [‘limitingConeAngle’](#feSpotLightLimitingConeAngleAttribute) represents the angle in degrees between the spot light axis (i.e. the axis between the light source and the point to which it is pointing at) and the spot light cone. User agents should apply a smoothing technique such as anti-aliasing at the boundary of the cone. If no value is specified, then no limiting cone will be applied. [Animatable](animate.md#Animatable): yes.
### <a id="LightingColorProperty"></a>15.8.5 The ‘lighting-color’ property

The [‘lighting-color’](#LightingColorProperty) property defines the color of the light source for filter primitives [‘feDiffuseLighting’](#feDiffuseLightingElement) and [‘feSpecularLighting’](#feSpecularLightingElement).

**‘lighting-color’**
:

| *Value:* | currentColor \| [<color>](types.md#DataTypeColor) [[<icccolor>](types.md#DataTypeICCColor)] \| inherit |
| --- | --- |
| *Initial:* | white |
| *Applies to:* | [‘feDiffuseLighting’](#feDiffuseLightingElement) and [‘feSpecularLighting’](#feSpecularLightingElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

## <a id="feBlendElement"></a>15.9 Filter primitive ‘feBlend’

This filter composites two objects together using commonly used imaging software blending modes. It performs a pixel-wise combination of two input images.

‘feBlend’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘in2’](#feBlendIn2Attribute)
- [‘mode’](#feBlendModeAttribute)

**DOM Interfaces:**
:

- [SVGFEBlendElement](#InterfaceSVGFEBlendElement)

*Attribute definitions:*

**<a id="feBlendModeAttribute"></a>mode = "*normal | multiply | screen | darken | lighten*"**
: One of the image blending modes (see [table](#BlendingTable) below). If attribute [‘mode’](#feBlendModeAttribute) is not specified, then the effect is as if a value of normal were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feBlendIn2Attribute"></a>in2 = "*(see [‘in’](#FilterPrimitiveInAttribute) attribute)*"**
: The second input image to the blending operation. This attribute can take on the same values as the [‘in’](#FilterPrimitiveInAttribute) attribute. [Animatable](animate.md#Animatable): yes.
For all feBlend modes, the result opacity is computed as follows:

```
qr = 1 - (1-qa)*(1-qb)
```

For the compositing formulas below, the following definitions apply:

```
cr = Result color (RGB) - premultiplied 
qa = Opacity value at a given pixel for image A 
qb = Opacity value at a given pixel for image B 
ca = Color (RGB) at a given pixel for image A - premultiplied 
cb = Color (RGB) at a given pixel for image B - premultiplied 
```

<a id="BlendingTable"></a>The following table provides the list of available image blending modes:

| Image Blending Mode | Formula for computing result color |
| --- | --- |
| normal | cr = (1 - qa) * cb + ca |
| multiply | cr = (1-qa)*cb + (1-qb)*ca + ca*cb |
| screen | cr = cb + ca - ca * cb |
| darken | cr = Min ((1 - qa) * cb + ca, (1 - qb) * ca + cb) |
| lighten | cr = Max ((1 - qa) * cb + ca, (1 - qb) * ca + cb) |

'normal' blend mode is equivalent to [operator="over"](#feCompositeOperatorAttribute) on the [‘feComposite’](#feCompositeElement) filter primitive, matches the blending method used by [‘feMerge’](#feMergeElement) and matches the [simple alpha compositing](masking.md#SimpleAlphaBlending) technique used in SVG for all compositing outside of filter effects.

Example feBlend shows examples of the five blend modes.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
          "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="5cm" height="5cm" viewBox="0 0 500 500"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
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
  <g enable-background="new" >
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

Example feBlend

| [image: Example feBlend — Examples of feBlend modes] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feBlend.svg)

## <a id="feColorMatrixElement"></a>15.10 Filter primitive ‘feColorMatrix’

This filter applies a matrix transformation:

```
| R' |     | a00 a01 a02 a03 a04 |   | R |
| G' |     | a10 a11 a12 a13 a14 |   | G |
| B' |  =  | a20 a21 a22 a23 a24 | * | B |
| A' |     | a30 a31 a32 a33 a34 |   | A |
| 1  |     |  0   0   0   0   1  |   | 1 |
```

on the RGBA color and alpha values of every pixel on the input graphics to produce a result with a new set of RGBA color and alpha values.

The calculations are performed on non-premultiplied color values. If the input graphics consists of premultiplied color values, those values are automatically converted into non-premultiplied color values for this operation.

These matrices often perform an identity mapping in the alpha channel. If that is the case, an implementation can avoid the costly undoing and redoing of the premultiplication for all pixels with A = 1.

‘feColorMatrix’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘type’](#feColorMatrixTypeAttribute)
- [‘values’](#feColorMatrixValuesAttribute)

**DOM Interfaces:**
:

- [SVGFEColorMatrixElement](#InterfaceSVGFEColorMatrixElement)

*Attribute definitions:*

**<a id="feColorMatrixTypeAttribute"></a>type = "*matrix | saturate | hueRotate | luminanceToAlpha*"**
: Indicates the type of matrix operation. The keyword 'matrix' indicates that a full 5x4 matrix of values will be provided. The other keywords represent convenience shortcuts to allow commonly used color operations to be performed without specifying a complete matrix. If attribute [‘type’](#feColorMatrixTypeAttribute) is not specified, then the effect is as if a value of matrix were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feColorMatrixValuesAttribute"></a>values = "*list of [<number>](types.md#DataTypeNumber)s*"**
: The contents of [‘values’](#feColorMatrixValuesAttribute) depends on the value of attribute [‘type’](#feColorMatrixTypeAttribute):

- For type="matrix", [‘values’](#feColorMatrixValuesAttribute) is a list of 20 matrix values (a00 a01 a02 a03 a04 a10 a11 ... a34), separated by whitespace and/or a comma. For example, the identity matrix could be expressed as:
```
type="matrix" 
values="1 0 0 0 0  0 1 0 0 0  0 0 1 0 0  0 0 0 1 0"
```

- For type="saturate", [‘values’](#feColorMatrixValuesAttribute) is a single real number value (0 to 1). A saturate operation is equivalent to the following matrix operation:
```
| R' |     |0.213+0.787s  0.715-0.715s  0.072-0.072s 0  0 |   | R |
| G' |     |0.213-0.213s  0.715+0.285s  0.072-0.072s 0  0 |   | G |
| B' |  =  |0.213-0.213s  0.715-0.715s  0.072+0.928s 0  0 | * | B |
| A' |     |           0            0             0  1  0 |   | A |
| 1  |     |           0            0             0  0  1 |   | 1 |
```

- For type="hueRotate", [‘values’](#feColorMatrixValuesAttribute) is a single one real number value (degrees). A hueRotate operation is equivalent to the following matrix operation:
```
| R' |     | a00  a01  a02  0  0 |   | R |
| G' |     | a10  a11  a12  0  0 |   | G |
| B' |  =  | a20  a21  a22  0  0 | * | B |
| A' |     | 0    0    0    1  0 |   | A |
| 1  |     | 0    0    0    0  1 |   | 1 |
```

where the terms a00, a01, etc. are calculated as follows:
```
| a00 a01 a02 |    [+0.213 +0.715 +0.072]
| a10 a11 a12 | =  [+0.213 +0.715 +0.072] +
| a20 a21 a22 |    [+0.213 +0.715 +0.072]
                        [+0.787 -0.715 -0.072]
cos(hueRotate value) *  [-0.213 +0.285 -0.072] +
                        [-0.213 -0.715 +0.928]
                        [-0.213 -0.715+0.928]
sin(hueRotate value) *  [+0.143 +0.140-0.283]
                        [-0.787 +0.715+0.072]
```

Thus, the upper left term of the hue matrix turns out to be:
```
.213 + cos(hueRotate value)*.787 - sin(hueRotate value)*.213
```

- For type="luminanceToAlpha", [‘values’](#feColorMatrixValuesAttribute) is not applicable. A luminanceToAlpha operation is equivalent to the following matrix operation:
```
   | R' |     |      0        0        0  0  0 |   | R |
   | G' |     |      0        0        0  0  0 |   | G |
   | B' |  =  |      0        0        0  0  0 | * | B |
   | A' |     | 0.2125   0.7154   0.0721  0  0 |   | A |
   | 1  |     |      0        0        0  0  1 |   | 1 |
```

If the attribute is not specified, then the default behavior depends on the value of attribute [‘type’](#feColorMatrixTypeAttribute). If type="matrix", then this attribute defaults to the identity matrix. If type="saturate", then this attribute defaults to the value 1, which results in the identity matrix. If type="hueRotate", then this attribute defaults to the value 0, which results in the identity matrix. [Animatable](animate.md#Animatable): yes.
Example feColorMatrix shows examples of the four types of feColorMatrix operations.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
          "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="5cm" viewBox="0 0 800 500"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
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

Example feColorMatrix

| [image: Example feColorMatrix — Examples of feColorMatrix operations] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feColorMatrix.svg)

## <a id="feComponentTransferElement"></a>15.11 Filter primitive ‘feComponentTransfer’

This filter primitive performs component-wise remapping of data as follows:

```
R' = feFuncR( R )
G' = feFuncG( G )
B' = feFuncB( B )
A' = feFuncA( A )
```

for every pixel. It allows operations like brightness adjustment, contrast adjustment, color balance or thresholding.

The calculations are performed on non-premultiplied color values. If the input graphics consists of premultiplied color values, those values are automatically converted into non-premultiplied color values for this operation. (Note that the undoing and redoing of the premultiplication can be avoided if [feFuncA](#feFuncAElement) is the identity transform and all alpha values on the source graphic are set to 1.)

‘feComponentTransfer’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘feFuncA’](#feFuncAElement)
- [‘feFuncB’](#feFuncBElement)
- [‘feFuncG’](#feFuncGElement)
- [‘feFuncR’](#feFuncRElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)

**DOM Interfaces:**
:

- [SVGFEComponentTransferElement](#InterfaceSVGFEComponentTransferElement)

The child elements of a [‘feComponentTransfer’](#feComponentTransferElement) element specify the transfer functions for the four channels:

<a id="transferFuncElements"></a>
- [‘feFuncR’](#feFuncRElement) — transfer function for the red component of the input graphic
- [‘feFuncG’](#feFuncGElement) — transfer function for the green component of the input graphic
- [‘feFuncB’](#feFuncBElement) — transfer function for the blue component of the input graphic
- [‘feFuncA’](#feFuncAElement) — transfer function for the alpha component of the input graphic

The following rules apply to the processing of the [‘feComponentTransfer’](#feComponentTransferElement) element:

- If more than one [transfer function element](#transferFuncElements) of the same kind is specified, the last occurrence is to be used.
- If any of the [transfer function elements](#transferFuncElements) are unspecified, the [‘feComponentTransfer’](#feComponentTransferElement) must be processed as if those [transfer function elements](#transferFuncElements) were specified with their [‘type’](#feComponentTransferTypeAttribute) attributes set to 'identity'.

<a id="feFuncRElement"></a>

‘feFuncR’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [transfer function element attributes](#TransferFunctionElementAttributes) — [‘type’](#feComponentTransferTypeAttribute), [‘tableValues’](#feComponentTransferTableValuesAttribute), [‘slope’](#feComponentTransferSlopeAttribute), [‘intercept’](#feComponentTransferInterceptAttribute), [‘amplitude’](#feComponentTransferAmplitudeAttribute), [‘exponent’](#feComponentTransferExponentAttribute), [‘offset’](#feComponentTransferOffsetAttribute)

**DOM Interfaces:**
:

- [SVGFEFuncRElement](#InterfaceSVGFEFuncRElement)

<a id="feFuncGElement"></a>

‘feFuncG’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [transfer function element attributes](#TransferFunctionElementAttributes) — [‘type’](#feComponentTransferTypeAttribute), [‘tableValues’](#feComponentTransferTableValuesAttribute), [‘slope’](#feComponentTransferSlopeAttribute), [‘intercept’](#feComponentTransferInterceptAttribute), [‘amplitude’](#feComponentTransferAmplitudeAttribute), [‘exponent’](#feComponentTransferExponentAttribute), [‘offset’](#feComponentTransferOffsetAttribute)

**DOM Interfaces:**
:

- [SVGFEFuncGElement](#InterfaceSVGFEFuncGElement)

<a id="feFuncBElement"></a>

‘feFuncB’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [transfer function element attributes](#TransferFunctionElementAttributes) — [‘type’](#feComponentTransferTypeAttribute), [‘tableValues’](#feComponentTransferTableValuesAttribute), [‘slope’](#feComponentTransferSlopeAttribute), [‘intercept’](#feComponentTransferInterceptAttribute), [‘amplitude’](#feComponentTransferAmplitudeAttribute), [‘exponent’](#feComponentTransferExponentAttribute), [‘offset’](#feComponentTransferOffsetAttribute)

**DOM Interfaces:**
:

- [SVGFEFuncBElement](#InterfaceSVGFEFuncBElement)

<a id="feFuncAElement"></a>

‘feFuncA’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [transfer function element attributes](#TransferFunctionElementAttributes) — [‘type’](#feComponentTransferTypeAttribute), [‘tableValues’](#feComponentTransferTableValuesAttribute), [‘slope’](#feComponentTransferSlopeAttribute), [‘intercept’](#feComponentTransferInterceptAttribute), [‘amplitude’](#feComponentTransferAmplitudeAttribute), [‘exponent’](#feComponentTransferExponentAttribute), [‘offset’](#feComponentTransferOffsetAttribute)

**DOM Interfaces:**
:

- [SVGFEFuncAElement](#InterfaceSVGFEFuncAElement)

<a id="TransferFunctionElementAttributes"></a>The attributes below are the transfer function element attributes, which apply to sub-elements [‘feFuncR’](#feFuncRElement), [‘feFuncG’](#feFuncGElement), [‘feFuncB’](#feFuncBElement) and [‘feFuncA’](#feFuncAElement) that define the transfer functions.

*Attribute definitions:*

**<a id="feComponentTransferTypeAttribute"></a>type = "*identity | table | discrete | linear | gamma*"**
:

Indicates the type of component transfer function. The type of function determines the applicability of the other attributes.

In the following, C is the initial component (e.g., [‘feFuncR’](#feFuncRElement)), C' is the remapped component; both in the closed interval [0,1].

- For identity:
```
C' = C
```

- For table, the function is defined by linear interpolation between values given in the attribute [‘tableValues’](#feComponentTransferTableValuesAttribute). The table has *n+1* values (i.e., v_0 to v_n) specifying the start and end values for *n* evenly sized interpolation regions. Interpolations use the following formula:
For a value `C < 1` find `k` such that:
k/n <= C < (k+1)/n
The result `C'` is given by:
C' = v_k + (C - k/n)*n * (v_k+1 - v_k)
If `C = 1` then:
C' = v_n.
- For discrete, the function is defined by the step function given in the attribute [‘tableValues’](#feComponentTransferTableValuesAttribute), which provides a list of *n* values (i.e., v_0 to v_n-1) in order to identify a step function consisting of *n* steps. The step function is defined by the following formula:
For a value `C < 1` find `k` such that:
k/n <= C < (k+1)/n
The result `C'` is given by:
C' = v_k
If `C = 1` then:
C' = v_n-1.
- For linear, the function is defined by the following linear equation:
C' = [slope](#feComponentTransferSlopeAttribute) * C + [intercept](#feComponentTransferInterceptAttribute)
- For gamma, the function is defined by the following exponential function:
C' = [amplitude](#feComponentTransferAmplitudeAttribute) * pow(C, [exponent](#feComponentTransferExponentAttribute)) + [offset](#feComponentTransferOffsetAttribute)

[Animatable](animate.md#Animatable): yes.
**<a id="feComponentTransferTableValuesAttribute"></a>tableValues = "*(list of [<number>](types.md#DataTypeNumber)s)*"**
: When type="table", the list of [<number>](types.md#DataTypeNumber)s *v0,v1,...vn*, separated by white space and/or a comma, which define the lookup table. An empty list results in an identity transfer function. If the attribute is not specified, then the effect is as if an empty list were provided. [Animatable](animate.md#Animatable): yes.
**<a id="feComponentTransferSlopeAttribute"></a>slope = "*[<number>](types.md#DataTypeNumber)*"**
: When type="linear", the slope of the linear function. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feComponentTransferInterceptAttribute"></a>intercept = "*[<number>](types.md#DataTypeNumber)*"**
: When type="linear", the intercept of the linear function. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feComponentTransferAmplitudeAttribute"></a>amplitude = "*[<number>](types.md#DataTypeNumber)*"**
: When type="gamma", the amplitude of the gamma function. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feComponentTransferExponentAttribute"></a>exponent = "*[<number>](types.md#DataTypeNumber)*"**
: When type="gamma", the exponent of the gamma function. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feComponentTransferOffsetAttribute"></a>offset = "*[<number>](types.md#DataTypeNumber)*"**
: When type="gamma", the offset of the gamma function. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
Example feComponentTransfer shows examples of the four types of feComponentTransfer operations.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
          "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="4cm" viewBox="0 0 800 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
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

Example feComponentTransfer

| [image: Example feComponentTransfer — Examples of feComponentTransfer operations] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feComponentTransfer.svg)

## <a id="feCompositeElement"></a>15.12 Filter primitive ‘feComposite’

This filter performs the combination of the two input images pixel-wise in image space using one of the Porter-Duff [[PORTERDUFF](refs.md#ref-PORTERDUFF)] compositing operations: *over, in, atop, out, xor* [[SVG-COMPOSITING](refs.md#ref-SVG-COMPOSITING)]. Additionally, a component-wise *arithmetic* operation (with the result clamped between [0..1]) can be applied.

The *arithmetic* operation is useful for combining the output from the [‘feDiffuseLighting’](#feDiffuseLightingElement) and [‘feSpecularLighting’](#feSpecularLightingElement) filters with texture data. It is also useful for implementing *dissolve*. If the *arithmetic* operation is chosen, each result pixel is computed using the following formula:

```
result = k1*i1*i2 + k2*i1 + k3*i2 + k4
```

where:

- `i1` and `i2` indicate the corresponding pixel channel values of the input image, which map to in and in2 respectively
- `k1, k2, k3` and `k4` indicate the values of the attributes with the same name

For this filter primitive, the extent of the resulting image might grow as described in the section that describes the [filter primitive subregion](#FilterPrimitiveSubRegion).

‘feComposite’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘in2’](#feCompositeIn2Attribute)
- [‘operator’](#feCompositeOperatorAttribute)
- [‘k1’](#feCompositeK1Attribute)
- [‘k2’](#feCompositeK2Attribute)
- [‘k3’](#feCompositeK3Attribute)
- [‘k4’](#feCompositeK4Attribute)

**DOM Interfaces:**
:

- [SVGFECompositeElement](#InterfaceSVGFECompositeElement)

*Attribute definitions:*

**<a id="feCompositeOperatorAttribute"></a>operator = "*over | in | out | atop | xor | arithmetic*"**
: The compositing operation that is to be performed. All of the [‘operator’](#feCompositeOperatorAttribute) types except arithmetic match the corresponding operation as described in [[PORTERDUFF](refs.md#ref-PORTERDUFF)]. The arithmetic operator is described above. If attribute [‘operator’](#feCompositeOperatorAttribute) is not specified, then the effect is as if a value of over were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feCompositeK1Attribute"></a>k1 = "*[<number>](types.md#DataTypeNumber)*"**
: Only applicable if operator="arithmetic". If the attribute is not specified, the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feCompositeK2Attribute"></a>k2 = "*[<number>](types.md#DataTypeNumber)*"**
: Only applicable if operator="arithmetic". If the attribute is not specified, the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feCompositeK3Attribute"></a>k3 = "*[<number>](types.md#DataTypeNumber)*"**
: Only applicable if operator="arithmetic". If the attribute is not specified, the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feCompositeK4Attribute"></a>k4 = "*[<number>](types.md#DataTypeNumber)*"**
: Only applicable if operator="arithmetic". If the attribute is not specified, the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feCompositeIn2Attribute"></a>in2 = "*(see [‘in’](#FilterPrimitiveInAttribute) attribute)*"**
: The second input image to the compositing operation. This attribute can take on the same values as the [‘in’](#FilterPrimitiveInAttribute) attribute. [Animatable](animate.md#Animatable): yes.
Example feComposite shows examples of the six types of feComposite operations. It also shows two different techniques to using the [BackgroundImage](#BackgroundImage) as part of the compositing operation.

The first two rows render bluish triangles into the background. A filter is applied which composites reddish triangles into the bluish triangles using one of the compositing operations. The result from compositing is drawn onto an opaque white temporary surface, and then that result is written to the canvas. (The opaque white temporary surface obliterates the original bluish triangle.)

The last two rows apply the same compositing operations of reddish triangles into bluish triangles. However, the compositing result is directly blended into the canvas (the opaque white temporary surface technique is not used). In some cases, the results are different than when a temporary opaque white surface is used. The original bluish triangle from the background shines through wherever the compositing operation results in completely transparent pixel. In other cases, the result from compositing is blended into the bluish triangle, resulting in a different final color value.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
          "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="330" height="195" viewBox="0 0 1100 650" version="1.1"
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
    <g enable-background="new">
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
    <g transform="translate(0,325)" enable-background="new">
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

Example feComposite

| [image: Example feComposite — Examples of feComposite operations] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feComposite.svg)

## <a id="feConvolveMatrixElement"></a>15.13 Filter primitive ‘feConvolveMatrix’

feConvolveMatrix applies a matrix convolution filter effect. A convolution combines pixels in the input image with neighboring pixels to produce a resulting image. A wide variety of imaging operations can be achieved through convolutions, including blurring, edge detection, sharpening, embossing and beveling.

A matrix convolution is based on an n-by-m matrix (the convolution kernel) which describes how a given pixel value in the input image is combined with its neighboring pixel values to produce a resulting pixel value. Each result pixel is determined by applying the kernel matrix to the corresponding source pixel and its neighboring pixels. The basic convolution formula which is applied to each color value for a given pixel is:

<a id="feConvolveMatrixElementFormula"></a> COLOR_X,Y = (                SUM _I=0 to [[orderY](#feConvolveMatrixElementOrderAttribute)-1] {                  SUM _J=0 to [[orderX](#feConvolveMatrixElementOrderAttribute)-1] {                    SOURCE _X-[targetX](#feConvolveMatrixElementTargetXAttribute)+J, Y-[targetY](#feConvolveMatrixElementTargetYAttribute)+I *  [kernelMatrix](#feConvolveMatrixElementKernelMatrixAttribute)_[orderX](#feConvolveMatrixElementOrderAttribute)-J-1,  [orderY](#feConvolveMatrixElementOrderAttribute)-I-1                  }                }              ) /  [divisor](#feConvolveMatrixElementDivisorAttribute) +  [bias](#feConvolveMatrixElementBiasAttribute) * ALPHA_X,Y

where "orderX" and "orderY" represent the X and Y values for the [‘order’](#feConvolveMatrixElementOrderAttribute) attribute, "targetX" represents the value of the [‘targetX’](#feConvolveMatrixElementTargetXAttribute) attribute, "targetY" represents the value of the [‘targetY’](#feConvolveMatrixElementTargetYAttribute) attribute, "kernelMatrix" represents the value of the [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute) attribute, "divisor" represents the value of the [‘divisor’](#feConvolveMatrixElementDivisorAttribute) attribute, and "bias" represents the value of the [‘bias’](#feConvolveMatrixElementBiasAttribute) attribute.

Note in the above formulas that the values in the kernel matrix are applied such that the kernel matrix is rotated 180 degrees relative to the source and destination images in order to match convolution theory as described in many computer graphics textbooks.

To illustrate, suppose you have a input image which is 5 pixels by 5 pixels, whose color values for one of the color channels are as follows:

```
    0  20  40 235 235
  100 120 140 235 235
  200 220 240 235 235
  225 225 255 255 255
  225 225 255 255 255
```

and you define a 3-by-3 convolution kernel as follows:

```
  1 2 3
  4 5 6
  7 8 9
```

Let's focus on the color value at the second row and second column of the image (source pixel value is 120). Assuming the simplest case (where the input image's pixel grid aligns perfectly with the kernel's pixel grid) and assuming default values for attributes [‘divisor’](#feConvolveMatrixElementDivisorAttribute), [‘targetX’](#feConvolveMatrixElementTargetXAttribute) and [‘targetY’](#feConvolveMatrixElementTargetYAttribute), then resulting color value will be:

```
(9*  0 + 8* 20 + 7* 40 +
6*100 + 5*120 + 4*140 +
3*200 + 2*220 + 1*240) / (9+8+7+6+5+4+3+2+1)
```

Because they operate on pixels, matrix convolutions are inherently resolution-dependent. To make [‘feConvolveMatrix’](#feConvolveMatrixElement) produce resolution-independent results, an explicit value should be provided for either the [‘filterRes’](#FilterElementFilterResAttribute) attribute on the [‘filter’](#FilterElement) element and/or attribute [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute).

[‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute), in combination with the other attributes, defines an implicit pixel grid in the filter effects coordinate system (i.e., the coordinate system established by the [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) attribute). If the pixel grid established by [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute) is not scaled to match the pixel grid established by attribute [‘filterRes’](#FilterElementFilterResAttribute) (implicitly or explicitly), then the input image will be temporarily rescaled to match its pixels with [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute). The convolution happens on the resampled image. After applying the convolution, the image is resampled back to the original resolution.

When the image must be resampled to match the coordinate system defined by [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute) prior to convolution, or resampled to match the device coordinate system after convolution, it is recommended that [high quality viewers](conform.md#ConformingHighQualitySVGViewers) make use of appropriate interpolation techniques, for example bilinear or bicubic. Depending on the speed of the available interpolents, this choice may be affected by the [‘image-rendering’](painting.md#ImageRenderingProperty) property setting. Note that implementations might choose approaches that minimize or eliminate resampling when not necessary to produce proper results, such as when the document is zoomed out such that [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute) is considerably smaller than a device pixel.

‘feConvolveMatrix’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘order’](#feConvolveMatrixElementOrderAttribute)
- [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute)
- [‘divisor’](#feConvolveMatrixElementDivisorAttribute)
- [‘bias’](#feConvolveMatrixElementBiasAttribute)
- [‘targetX’](#feConvolveMatrixElementTargetXAttribute)
- [‘targetY’](#feConvolveMatrixElementTargetYAttribute)
- [‘edgeMode’](#feConvolveMatrixElementEdgeModeAttribute)
- [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute)
- [‘preserveAlpha’](#feConvolveMatrixElementPreserveAlphaAttribute)

**DOM Interfaces:**
:

- [SVGFEConvolveMatrixElement](#InterfaceSVGFEConvolveMatrixElement)

*Attribute definitions:*

**<a id="feConvolveMatrixElementOrderAttribute"></a>order = "[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)"**
: Indicates the number of cells in each dimension for [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute). The values provided must be [<integer>](types.md#DataTypeInteger)s greater than zero. The first number, <orderX>, indicates the number of columns in the matrix. The second number, <orderY>, indicates the number of rows in the matrix. If <orderY> is not provided, it defaults to <orderX>. A typical value is order="3". It is recommended that only small values (e.g., 3) be used; higher values may result in very high CPU overhead and usually do not produce results that justify the impact on performance. If the attribute is not specified, the effect is as if a value of 3 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementKernelMatrixAttribute"></a> kernelMatrix = "<list of numbers>"**
: The list of [<number>](types.md#DataTypeNumber)s that make up the kernel matrix for the convolution. Values are separated by space characters and/or a comma. The number of entries in the list must equal <orderX> times <orderY>. [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementDivisorAttribute"></a>divisor = "[<number>](types.md#DataTypeNumber)"**
: After applying the [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute) to the input image to yield a number, that number is divided by [‘divisor’](#feConvolveMatrixElementDivisorAttribute) to yield the final destination color value. A divisor that is the sum of all the matrix values tends to have an evening effect on the overall color intensity of the result. It is an error to specify a divisor of zero. The default value is the sum of all values in kernelMatrix, with the exception that if the sum is zero, then the divisor is set to 1. [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementBiasAttribute"></a>bias = "[<number>](types.md#DataTypeNumber)"**
: After applying the [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute) to the input image to yield a number and applying the [‘divisor’](#feConvolveMatrixElementDivisorAttribute), the [‘bias’](#feConvolveMatrixElementBiasAttribute) attribute is added to each component. One application of [‘bias’](#feConvolveMatrixElementBiasAttribute) is when it is desirable to have .5 gray value be the zero response of the filter. The bias property shifts the range of the filter. This allows representation of values that would otherwise be clamped to 0 or 1. If [‘bias’](#feConvolveMatrixElementBiasAttribute) is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementTargetXAttribute"></a>targetX = "[<integer>](types.md#DataTypeInteger)"**
: Determines the positioning in X of the convolution matrix relative to a given target pixel in the input image. The leftmost column of the matrix is column number zero. The value must be such that: 0 <= targetX < orderX. By default, the convolution matrix is centered in X over each pixel of the input image (i.e., targetX = floor ( orderX / 2 )). [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementTargetYAttribute"></a>targetY = "[<integer>](types.md#DataTypeInteger)"**
: Determines the positioning in Y of the convolution matrix relative to a given target pixel in the input image. The topmost row of the matrix is row number zero. The value must be such that: 0 <= targetY < orderY. By default, the convolution matrix is centered in Y over each pixel of the input image (i.e., targetY = floor ( orderY / 2 )). [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementEdgeModeAttribute"></a>edgeMode = "duplicate | wrap | none"**
:

Determines how to extend the input image as necessary with color values so that the matrix operations can be applied when the kernel is positioned at or near the edge of the input image.

"duplicate" indicates that the input image is extended along each of its borders as necessary by duplicating the color values at the given edge of the input image.

```
Original N-by-M image, where m=M-1 and n=N-1:
          11 12 ... 1m 1M
          21 22 ... 2m 2M
          .. .. ... .. ..
          n1 n2 ... nm nM
          N1 N2 ... Nm NM
Extended by two pixels using "duplicate":
  11 11   11 12 ... 1m 1M   1M 1M
  11 11   11 12 ... 1m 1M   1M 1M
  11 11   11 12 ... 1m 1M   1M 1M
  21 21   21 22 ... 2m 2M   2M 2M
  .. ..   .. .. ... .. ..   .. ..
  n1 n1   n1 n2 ... nm nM   nM nM
  N1 N1   N1 N2 ... Nm NM   NM NM
  N1 N1   N1 N2 ... Nm NM   NM NM
  N1 N1   N1 N2 ... Nm NM   NM NM
```

"wrap" indicates that the input image is extended by taking the color values from the opposite edge of the image.

```
Extended by two pixels using "wrap":
  nm nM   n1 n2 ... nm nM   n1 n2
  Nm NM   N1 N2 ... Nm NM   N1 N2
  1m 1M   11 12 ... 1m 1M   11 12
  2m 2M   21 22 ... 2m 2M   21 22
  .. ..   .. .. ... .. ..   .. ..
  nm nM   n1 n2 ... nm nM   n1 n2
  Nm NM   N1 N2 ... Nm NM   N1 N2
  1m 1M   11 12 ... 1m 1M   11 12
  2m 2M   21 22 ... 2m 2M   21 22
```

"none" indicates that the input image is extended with pixel values of zero for R, G, B and A.

If attribute [‘edgeMode’](#feConvolveMatrixElementEdgeModeAttribute) is not specified, then the effect is as if a value of duplicate were specified.

[Animatable](animate.md#Animatable): yes.

**<a id="feConvolveMatrixElementKernelUnitLengthAttribute"></a> kernelUnitLength = "[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)"**
: The first number is the <dx> value. The second number is the <dy> value. If the <dy> value is not specified, it defaults to the same value as <dx>. Indicates the intended distance in current filter units (i.e., units as determined by the value of attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute)) between successive columns and rows, respectively, in the [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute). By specifying value(s) for [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute), the kernel becomes defined in a scalable, abstract coordinate system. If [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute) is not specified, the default value is one pixel in the offscreen bitmap, which is a pixel-based coordinate system, and thus potentially not scalable. For some level of consistency across display media and user agents, it is necessary that a value be provided for at least one of [‘filterRes’](#FilterElementFilterResAttribute) and [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute). In some implementations, the most consistent results and the fastest performance will be achieved if the pixel grid of the temporary offscreen images aligns with the pixel grid of the kernel. A negative or zero value is an error (see [Error processing](implnote.md#ErrorProcessing)). [Animatable](animate.md#Animatable): yes.
**<a id="feConvolveMatrixElementPreserveAlphaAttribute"></a> preserveAlpha = "false | true"**
: A value of false indicates that the convolution will apply to all channels, including the alpha channel. In this case the `ALPHA_X,Y` of the [convolution formula](#feConvolveMatrixElementFormula) for a given pixel is:

ALPHA_X,Y = (                SUM _I=0 to [[orderY](#feConvolveMatrixElementOrderAttribute)-1] {                  SUM _J=0 to [[orderX](#feConvolveMatrixElementOrderAttribute)-1] {                    SOURCE _X-[targetX](#feConvolveMatrixElementTargetXAttribute)+J, Y-[targetY](#feConvolveMatrixElementTargetYAttribute)+I *  [kernelMatrix](#feConvolveMatrixElementKernelMatrixAttribute)_[orderX](#feConvolveMatrixElementOrderAttribute)-J-1,  [orderY](#feConvolveMatrixElementOrderAttribute)-I-1                  }                }              ) /  [divisor](#feConvolveMatrixElementDivisorAttribute) +  [bias](#feConvolveMatrixElementBiasAttribute)

A value of true indicates that the convolution will only apply to the color channels. In this case, the filter will temporarily unpremultiply the color component values, apply the kernel, and then re-premultiply at the end. In this case the `ALPHA_X,Y` of the [convolution formula](#feConvolveMatrixElementFormula) for a given pixel is:

ALPHA_X,Y = SOURCE_X,Y

If [‘preserveAlpha’](#feConvolveMatrixElementPreserveAlphaAttribute) is not specified, then the effect is as if a value of false were specified. [Animatable](animate.md#Animatable): yes.
## <a id="feDiffuseLightingElement"></a>15.14 Filter primitive ‘feDiffuseLighting’

This filter primitive lights an image using the alpha channel as a bump map. The resulting image is an RGBA opaque image based on the light color with alpha = 1.0 everywhere. The lighting calculation follows the standard diffuse component of the Phong lighting model. The resulting image depends on the light color, light position and surface geometry of the input bump map.

The light map produced by this filter primitive can be combined with a texture image using the multiply term of the *arithmetic* [‘feComposite’](#feCompositeElement) compositing method. Multiple light sources can be simulated by adding several of these light maps together before applying it to the texture image.

The formulas below make use of 3x3 filters. Because they operate on pixels, such filters are inherently resolution-dependent. To make [‘feDiffuseLighting’](#feDiffuseLightingElement) produce resolution-independent results, an explicit value should be provided for either the [‘filterRes’](#FilterElementFilterResAttribute) attribute on the [‘filter’](#FilterElement) element and/or attribute [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute).

[‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute), in combination with the other attributes, defines an implicit pixel grid in the filter effects coordinate system (i.e., the coordinate system established by the [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) attribute). If the pixel grid established by [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute) is not scaled to match the pixel grid established by attribute [‘filterRes’](#FilterElementFilterResAttribute) (implicitly or explicitly), then the input image will be temporarily rescaled to match its pixels with [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute). The 3x3 filters are applied to the resampled image. After applying the filter, the image is resampled back to its original resolution.

When the image must be resampled, it is recommended that [high quality viewers](conform.md#ConformingHighQualitySVGViewers) make use of appropriate interpolation techniques, for example bilinear or bicubic. Depending on the speed of the available interpolents, this choice may be affected by the [‘image-rendering’](painting.md#ImageRenderingProperty) property setting. Note that implementations might choose approaches that minimize or eliminate resampling when not necessary to produce proper results, such as when the document is zoomed out such that [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute) is considerably smaller than a device pixel.

For the formulas that follow, the `Norm(A_x,A_y,A_z)` function is defined as:

Norm(A_x,A_y,A_z) = sqrt(A_x^2+A_y^2+A_z^2)

The resulting RGBA image is computed as follows:

D_r = k_d * N.L * L_r D_g = k_d * N.L * L_g D_b = k_d * N.L * L_b D_a = 1.0

where

: k_d = diffuse lighting constant N = surface normal unit vector, a function of x and y L = unit vector pointing from surface to light, a function of x and y in the point and spot light cases L_r,L_g,L_b = RGB components of light, a function of x and y in the spot light case
N is a function of x and y and depends on the surface gradient as follows:

The surface described by the input alpha image I(x,y) is:

Z (x,y) = surfaceScale * I(x,y)

<a id="SurfaceNormalCalculations"></a>Surface normal is calculated using the Sobel gradient 3x3 filter. Different filter kernels are used depending on whether the given pixel is on the interior or an edge. For each case, the formula is:

N_x (x,y)= - surfaceScale * FACTOR_x *            (K_x(0,0)*I(x-dx,y-dy) + K_x(1,0)*I(x,y-dy) + K_x(2,0)*I(x+dx,y-dy) +             K_x(0,1)*I(x-dx,y)    + K_x(1,1)*I(x,y)    + K_x(2,1)*I(x+dx,y)    +             K_x(0,2)*I(x-dx,y+dy) + K_x(1,2)*I(x,y+dy) + K_x(2,2)*I(x+dx,y+dy)) N_y (x,y)= - surfaceScale * FACTOR_y *            (K_y(0,0)*I(x-dx,y-dy) + K_y(1,0)*I(x,y-dy) + K_y(2,0)*I(x+dx,y-dy) +             K_y(0,1)*I(x-dx,y)    + K_y(1,1)*I(x,y)    + K_y(2,1)*I(x+dx,y)    +             K_y(0,2)*I(x-dx,y+dy) + K_y(1,2)*I(x,y+dy) + K_y(2,2)*I(x+dx,y+dy)) N_z (x,y) = 1.0 N = (N_x, N_y, N_z) / Norm((N_x,N_y,N_z))

In these formulas, the `dx` and `dy` values (e.g., `I(x-dx,y-dy)`), represent deltas relative to a given `(x,y)` position for the purpose of estimating the slope of the surface at that point. These deltas are determined by the value (explicit or implicit) of attribute [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute).

| Top/left corner: FACTOR_x=2/(3*dx) K_x = \| 0 0 0 \| \| 0 -2 2 \| \| 0 -1 1 \| FACTOR_y=2/(3*dy) K_y = \| 0 0 0 \| \| 0 -2 -1 \| \| 0 2 1 \| | Top row: FACTOR_x=1/(3*dx) K_x = \| 0 0 0 \| \| -2 0 2 \| \| -1 0 1 \| FACTOR_y=1/(2*dy) K_y = \| 0 0 0 \| \| -1 -2 -1 \| \| 1 2 1 \| | Top/right corner: FACTOR_x=2/(3*dx) K_x = \| 0 0 0 \| \| -2 2 0 \| \| -1 1 0 \| FACTOR_y=2/(3*dy) K_y = \| 0 0 0 \| \| -1 -2 0 \| \| 1 2 0 \| |
| --- | --- | --- |
| Left column: FACTOR_x=1/(2*dx) K_x = \| 0 -1 1 \| \| 0 -2 2 \| \| 0 -1 1 \| FACTOR_y=1/(3*dy) K_y = \| 0 -2 -1 \| \| 0 0 0 \| \| 0 2 1 \| | Interior pixels: FACTOR_x=1/(4*dx) K_x = \| -1 0 1 \| \| -2 0 2 \| \| -1 0 1 \| FACTOR_y=1/(4*dy) K_y = \| -1 -2 -1 \| \| 0 0 0 \| \| 1 2 1 \| | Right column: FACTOR_x=1/(2*dx) K_x = \| -1 1 0\| \| -2 2 0\| \| -1 1 0\| FACTOR_y=1/(3*dy) K_y = \| -1 -2 0 \| \| 0 0 0 \| \| 1 2 0 \| |
| Bottom/left corner: FACTOR_x=2/(3*dx) K_x = \| 0 -1 1 \| \| 0 -2 2 \| \| 0 0 0 \| FACTOR_y=2/(3*dy) K_y = \| 0 -2 -1 \| \| 0 2 1 \| \| 0 0 0 \| | Bottom row: FACTOR_x=1/(3*dx) K_x = \| -1 0 1 \| \| -2 0 2 \| \| 0 0 0 \| FACTOR_y=1/(2*dy) K_y = \| -1 -2 -1 \| \| 1 2 1 \| \| 0 0 0 \| | Bottom/right corner: FACTOR_x=2/(3*dx) K_x = \| -1 1 0 \| \| -2 2 0 \| \| 0 0 0 \| FACTOR_y=2/(3*dy) K_y = \| -1 -2 0 \| \| 1 2 0 \| \| 0 0 0 \| |

L, the unit vector from the image sample to the light, is calculated as follows:

For Infinite light sources it is constant:

L_x = cos(azimuth)*cos(elevation) L_y = sin(azimuth)*cos(elevation) L_z = sin(elevation)

For Point and spot lights it is a function of position:

L_x = Light_x - x L_y = Light_y - y L_z = Light_z - Z(x,y) L = (L_x, L_y, L_z) / Norm(L_x, L_y, L_z)

where Light_x, Light_y, and Light_z are the input light position.

L_r,L_g,L_b, the light color vector, is a function of position in the spot light case only:

L_r = Light_r*pow((-L.S),specularExponent) L_g = Light_g*pow((-L.S),specularExponent) L_b = Light_b*pow((-L.S),specularExponent)

where S is the unit vector pointing from the light to the point (pointsAtX, pointsAtY, pointsAtZ) in the x-y plane:

S_x = pointsAtX - Light_x S_y = pointsAtY - Light_y S_z = pointsAtZ - Light_z S = (S_x, S_y, S_z) / Norm(S_x, S_y, S_z)

If L.S is positive, no light is present. (L_r = L_g = L_b = 0). If [‘limitingConeAngle’](#feSpotLightLimitingConeAngleAttribute) is specified, -L.S < cos(limitingConeAngle) also indicates that no light is present.

‘feDiffuseLighting’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of [descriptive elements](intro.md#TermDescriptiveElement) and exactly one [light source element](intro.md#TermLightSourceElement), in any order.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘surfaceScale’](#feDiffuseLightingSurfaceScaleAttribute)
- [‘diffuseConstant’](#feDiffuseLightingDiffuseConstantAttribute)
- [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute)

**DOM Interfaces:**
:

- [SVGFEDiffuseLightingElement](#InterfaceSVGFEDiffuseLightingElement)

*Attribute definitions:*

**<a id="feDiffuseLightingSurfaceScaleAttribute"></a>surfaceScale = "*[<number>](types.md#DataTypeNumber)*"**
: height of surface when A_in = 1. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feDiffuseLightingDiffuseConstantAttribute"></a>diffuseConstant = "*[<number>](types.md#DataTypeNumber)*"**
: kd in Phong lighting model. In SVG, this can be any non-negative number. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feDiffuseLightingKernelUnitLengthAttribute"></a> kernelUnitLength = "[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)"**
: The first number is the <dx> value. The second number is the <dy> value. If the <dy> value is not specified, it defaults to the same value as <dx>. Indicates the intended distance in current filter units (i.e., units as determined by the value of attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute)) for `dx` and `dy`, respectively, in the [surface normal calculation formulas](#SurfaceNormalCalculations). By specifying value(s) for [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute), the kernel becomes defined in a scalable, abstract coordinate system. If [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute) is not specified, the `dx` and `dy` values should represent very small deltas relative to a given `(x,y)` position, which might be implemented in some cases as one pixel in the intermediate image offscreen bitmap, which is a pixel-based coordinate system, and thus potentially not scalable. For some level of consistency across display media and user agents, it is necessary that a value be provided for at least one of [‘filterRes’](#FilterElementFilterResAttribute) and [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute). Discussion of intermediate images are in the [Introduction](#Introduction) and in the description of attribute [‘filterRes’](#FilterElementFilterResAttribute). A negative or zero value is an error (see [Error processing](implnote.md#ErrorProcessing)). [Animatable](animate.md#Animatable): yes.
The light source is defined by one of the child elements [‘feDistantLight’](#feDistantLightElement), [‘fePointLight’](#fePointLightElement) or [‘feSpotLight’](#feSpotLightElement). The light color is specified by property [‘lighting-color’](#LightingColorProperty).

## <a id="feDisplacementMapElement"></a>15.15 Filter primitive ‘feDisplacementMap’

This filter primitive uses the pixels values from the image from [‘in2’](#feDisplacementMapIn2Attribute) to spatially displace the image from [‘in’](#FilterPrimitiveInAttribute). This is the transformation to be performed:

```
P'(x,y) <- P( x + scale * (XC(x,y) - .5), y + scale * (YC(x,y) - .5))
```

where P(x,y) is the input image, [‘in’](#FilterPrimitiveInAttribute), and P'(x,y) is the destination. XC(x,y) and YC(x,y) are the component values of the channel designated by the *xChannelSelector* and *yChannelSelector*. For example, to use the R component of [‘in2’](#feDisplacementMapIn2Attribute) to control displacement in x and the G component of Image2 to control displacement in y, set *xChannelSelector* to "R" and *yChannelSelector* to "G".

The displacement map defines the inverse of the mapping performed.

The input image in is to remain premultiplied for this filter primitive. The calculations using the pixel values from [‘in2’](#feDisplacementMapIn2Attribute) are performed using non-premultiplied color values. If the image from [‘in2’](#feDisplacementMapIn2Attribute) consists of premultiplied color values, those values are automatically converted into non-premultiplied color values before performing this operation.

This filter can have arbitrary non-localized effect on the input which might require substantial buffering in the processing pipeline. However with this formulation, any intermediate buffering needs can be determined by *scale* which represents the maximum range of displacement in either x or y.

When applying this filter, the source pixel location will often lie between several source pixels. In this case it is recommended that [high quality viewers](conform.md#ConformingHighQualitySVGViewers) apply an interpolent on the surrounding pixels, for example bilinear or bicubic, rather than simply selecting the nearest source pixel. Depending on the speed of the available interpolents, this choice may be affected by the [‘image-rendering’](painting.md#ImageRenderingProperty) property setting.

The [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty) property only applies to the [‘in2’](#feDisplacementMapIn2Attribute) source image and does not apply to the [‘in’](#FilterPrimitiveInAttribute) source image. The [‘in’](#FilterPrimitiveInAttribute) source image must remain in its current color space.

‘feDisplacementMap’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘in2’](#feDisplacementMapIn2Attribute)
- [‘scale’](#feDisplacementMapScaleAttribute)
- [‘xChannelSelector’](#feDisplacementMapXChannelSelectorAttribute)
- [‘yChannelSelector’](#feDisplacementMapYChannelSelectorAttribute)

**DOM Interfaces:**
:

- [SVGFEDisplacementMapElement](#InterfaceSVGFEDisplacementMapElement)

*Attribute definitions:*

**<a id="feDisplacementMapScaleAttribute"></a>scale = "*[<number>](types.md#DataTypeNumber)*"**
: Displacement scale factor. The amount is expressed in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. When the value of this attribute is 0, this operation has no effect on the source image. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feDisplacementMapXChannelSelectorAttribute"></a> xChannelSelector = "*R | G | B | A*"**
: Indicates which channel from [‘in2’](#feDisplacementMapIn2Attribute) to use to displace the pixels in [‘in’](#FilterPrimitiveInAttribute) along the x-axis. If attribute [‘xChannelSelector’](#feDisplacementMapXChannelSelectorAttribute) is not specified, then the effect is as if a value of A were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feDisplacementMapYChannelSelectorAttribute"></a> yChannelSelector = "*R | G | B | A*"**
: Indicates which channel from [‘in2’](#feDisplacementMapIn2Attribute) to use to displace the pixels in [‘in’](#FilterPrimitiveInAttribute) along the y-axis. If attribute [‘yChannelSelector’](#feDisplacementMapYChannelSelectorAttribute) is not specified, then the effect is as if a value of A were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feDisplacementMapIn2Attribute"></a>in2 = "*(see [‘in’](#FilterPrimitiveInAttribute) attribute)*"**
: The second input image, which is used to displace the pixels in the image from attribute [‘in’](#FilterPrimitiveInAttribute). This attribute can take on the same values as the [‘in’](#FilterPrimitiveInAttribute) attribute. [Animatable](animate.md#Animatable): yes.
## <a id="feFloodElement"></a>15.16 Filter primitive ‘feFlood’

This filter primitive creates a rectangle filled with the color and opacity values from properties [‘flood-color’](#FloodColorProperty) and [‘flood-opacity’](#FloodOpacityProperty). The rectangle is as large as the [filter primitive subregion](#FilterPrimitiveSubRegion) established by the [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) attributes on the [‘feFlood’](#feFloodElement) element.

‘feFlood’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘animateColor’](animate.md#AnimateColorElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)

**DOM Interfaces:**
:

- [SVGFEFloodElement](#InterfaceSVGFEFloodElement)

The [‘flood-color’](#FloodColorProperty) property indicates what color to use to flood the current [filter primitive subregion](#FilterPrimitiveSubRegion). The keyword currentColor and ICC colors can be specified in the same manner as within a [<paint>](painting.md#SpecifyingPaint) specification for the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties.

**<a id="FloodColorProperty"></a>‘flood-color’**
:

| *Value:* | currentColor \| [<color>](types.md#DataTypeColor) [[<icccolor>](types.md#DataTypeICCColor)] \| inherit |
| --- | --- |
| *Initial:* | black |
| *Applies to:* | [‘feFlood’](#feFloodElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘flood-opacity’](#FloodOpacityProperty) property defines the opacity value to use across the entire [filter primitive subregion](#FilterPrimitiveSubRegion).

**<a id="FloodOpacityProperty"></a>‘flood-opacity’**
:

| *Value:* | <opacity-value> \| inherit |
| --- | --- |
| *Initial:* | 1 |
| *Applies to:* | [‘feFlood’](#feFloodElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

## <a id="feGaussianBlurElement"></a>15.17 Filter primitive ‘feGaussianBlur’

This filter primitive performs a Gaussian blur on the input image.

The Gaussian blur kernel is an approximation of the normalized convolution:

G(x,y) = H(x)I(y)

where

H(x) = exp(-x^2/ (2s^2)) / sqrt(2* pi*s^2)

and

I(y) = exp(-y^2/ (2t^2)) / sqrt(2* pi*t^2)

with 's' being the standard deviation in the x direction and 't' being the standard deviation in the y direction, as specified by [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute).

The value of [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute) can be either one or two numbers. If two numbers are provided, the first number represents a standard deviation value along the x-axis of the current coordinate system and the second value represents a standard deviation in Y. If one number is provided, then that value is used for both X and Y.

Even if only one value is provided for [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute), this can be implemented as a separable convolution.

For larger values of 's' (s >= 2.0), an approximation can be used: Three successive box-blurs build a piece-wise quadratic convolution kernel, which approximates the Gaussian kernel to within roughly 3%.

let d = floor(s * 3*sqrt(2*pi)/4 + 0.5)

... if d is odd, use three box-blurs of size 'd', centered on the output pixel.

... if d is even, two box-blurs of size 'd' (the first one centered on the pixel boundary between the output pixel and the one to the left, the second one centered on the pixel boundary between the output pixel and the one to the right) and one box blur of size 'd+1' centered on the output pixel.

Note: the approximation formula also applies correspondingly to 't'.

Frequently this operation will take place on alpha-only images, such as that produced by the built-in input, [SourceAlpha](#SourceAlpha). The implementation may notice this and optimize the single channel case. If the input has infinite extent and is constant (e.g [FillPaint](#FillPaint) where the fill is a solid color), this operation has no effect. If the input has infinite extent and the filter result is the input to an [‘feTile’](#feTileElement), the filter is evaluated with periodic boundary conditions.

‘feGaussianBlur’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute)

**DOM Interfaces:**
:

- [SVGFEGaussianBlurElement](#InterfaceSVGFEGaussianBlurElement)

*Attribute definitions:*

**<a id="feGaussianBlurStdDeviationAttribute"></a>stdDeviation = "*[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)*"**
: The standard deviation for the blur operation. If two [<number>](types.md#DataTypeNumber)s are provided, the first number represents a standard deviation value along the x-axis of the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. The second value represents a standard deviation in Y. If one number is provided, then that value is used for both X and Y. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables the effect of the given filter primitive (i.e., the result is the filter input image). If [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute) is 0 in only one of X or Y, then the effect is that the blur is only applied in the direction that has a non-zero value. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
[The example](#AnExample) at the start of this chapter makes use of the [‘feGaussianBlur’](#feGaussianBlurElement) filter primitive to create a drop shadow effect.

## <a id="feImageElement"></a>15.18 Filter primitive ‘feImage’

This filter primitive refers to a graphic external to this filter element, which is loaded or rendered into an RGBA raster and becomes the result of the filter primitive.

This filter primitive can refer to an external image or can be a reference to another piece of SVG. It produces an image similar to the built-in image source [SourceGraphic](#SourceGraphic) except that the graphic comes from an external source.

If the [‘xlink:href’](#feImageElementHrefAttribute) references a stand-alone image resource such as a JPEG, PNG or SVG file, then the image resource is rendered according to the behavior of the [‘image’](struct.md#ImageElement) element; otherwise, the referenced resource is rendered according to the behavior of the [‘use’](struct.md#UseElement) element. In either case, the current user coordinate system depends on the value of attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. The processing of the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute on the [‘feImage’](#feImageElement) element is identical to that of the [‘image’](struct.md#ImageElement) element.

When the referenced image must be resampled to match the device coordinate system, it is recommended that [high quality viewers](conform.md#ConformingHighQualitySVGViewers) make use of appropriate interpolation techniques, for example bilinear or bicubic. Depending on the speed of the available interpolents, this choice may be affected by the [‘image-rendering’](painting.md#ImageRenderingProperty) property setting.

‘feImage’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘animateTransform’](animate.md#AnimateTransformElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#feImageElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘xlink:href’](#feImageElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGFEImageElement](#InterfaceSVGFEImageElement)

*Attribute definitions:*

**<a id="feImageElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
:

An [IRI reference](linking.md#IRIReference) to the image source.

[Animatable](animate.md#Animatable): yes.

**<a id="feImageElementPreserveAspectRatioAttribute"></a> preserveAspectRatio = "[[defer] <align> [<meetOrSlice>]](coords.md#PreserveAspectRatioAttribute)" **
:

See [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute).

If attribute [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) is not specified, then the effect is as if a value of xMidYMid meet were specified.

[Animatable](animate.md#Animatable): yes.

Example feImage illustrates how images are placed relative to an object. From left to right:

- The default placement of an image. Note that the image is centered in the [filter region](#FilterEffectsRegion) and has the maximum size that will fit in the region consistent with preserving the aspect ratio.
- The image stretched to fit the bounding box of an object.
- The image placed using user coordinates. Note that the image is first centered in a box the size of the [filter region](#FilterEffectsRegion) and has the maximum size that will fit in the box consistent with preserving the aspect ratio. This box is then shifted by the given 'x' and 'y' values relative to the viewport the object is in.

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

Example feImage

| [image: Example feImage — Examples of feImage use] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feImage-01.svg)

## <a id="feMergeElement"></a>15.19 Filter primitive ‘feMerge’

This filter primitive composites input image layers on top of each other using the *over* operator with *Input1* (corresponding to the first [‘feMergeNode’](#feMergeNodeElement) child element) on the bottom and the last specified input, *InputN* (corresponding to the last [‘feMergeNode’](#feMergeNodeElement) child element), on top.

Many effects produce a number of intermediate layers in order to create the final output image. This filter allows us to collapse those into a single image. Although this could be done by using n-1 Composite-filters, it is more convenient to have this common operation available in this form, and offers the implementation some additional flexibility.

Each [‘feMerge’](#feMergeElement) element can have any number of [‘feMergeNode’](#feMergeNodeElement) subelements, each of which has an [‘in’](#FilterPrimitiveInAttribute) attribute.

The canonical implementation of feMerge is to render the entire effect into one RGBA layer, and then render the resulting layer on the output device. In certain cases (in particular if the output device itself is a continuous tone device), and since merging is associative, it might be a sufficient approximation to evaluate the effect one layer at a time and render each layer individually onto the output device bottom to top.

If the topmost image input is [SourceGraphic](#SourceGraphic) and this [‘feMerge’](#feMergeElement) is the last filter primitive in the filter, the implementation is encouraged to render the layers up to that point, and then render the [SourceGraphic](#SourceGraphic) directly from its vector description on top.

‘feMerge’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘feMergeNode’](#feMergeNodeElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)

**DOM Interfaces:**
:

- [SVGFEMergeElement](#InterfaceSVGFEMergeElement)

<a id="feMergeNodeElement"></a>

‘feMergeNode’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘in’](#FilterPrimitiveInAttribute)

**DOM Interfaces:**
:

- [SVGFEMergeNodeElement](#InterfaceSVGFEMergeNodeElement)

[The example](#AnExample) at the start of this chapter makes use of the [‘feMerge’](#feMergeElement) filter primitive to composite two intermediate filter results together.

## <a id="feMorphologyElement"></a>15.20 Filter primitive ‘feMorphology’

This filter primitive performs "fattening" or "thinning" of artwork. It is particularly useful for fattening or thinning an alpha channel.

The dilation (or erosion) kernel is a rectangle with a width of 2**x-radius* and a height of 2**y-radius*. In dilation, the output pixel is the individual component-wise maximum of the corresponding R,G,B,A values in the input image's kernel rectangle. In erosion, the output pixel is the individual component-wise minimum of the corresponding R,G,B,A values in the input image's kernel rectangle.

Frequently this operation will take place on alpha-only images, such as that produced by the built-in input, [SourceAlpha](#SourceAlpha). In that case, the implementation might want to optimize the single channel case.

If the input has infinite extent and is constant (e.g [FillPaint](#FillPaint) where the fill is a solid color), this operation has no effect. If the input has infinite extent and the filter result is the input to an [‘feTile’](#feTileElement), the filter is evaluated with periodic boundary conditions.

Because [‘feMorphology’](#feMorphologyElement) operates on premultipied color values, it will always result in color values less than or equal to the alpha channel.

‘feMorphology’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘operator’](#feMorphologyOperatorAttribute)
- [‘radius’](#feMorphologyRadiusAttribute)

**DOM Interfaces:**
:

- [SVGFEMorphologyElement](#InterfaceSVGFEMorphologyElement)

*Attribute definitions:*

**<a id="feMorphologyOperatorAttribute"></a>operator = "*erode | dilate*"**
: A keyword indicating whether to erode (i.e., thin) or dilate (fatten) the source graphic. If attribute [‘operator’](#feMorphologyOperatorAttribute) is not specified, then the effect is as if a value of erode were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feMorphologyRadiusAttribute"></a>radius = "*[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)*"**
: The radius (or radii) for the operation. If two [<number>](types.md#DataTypeNumber)s are provided, the first number represents a x-radius and the second value represents a y-radius. If one number is provided, then that value is used for both X and Y. The values are in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables the effect of the given filter primitive (i.e., the result is a transparent black image). If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
Example feMorphology shows examples of the four types of feMorphology operations.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
          "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="5cm" height="7cm" viewBox="0 0 700 500"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
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
  <g enable-background="new" >
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

Example feMorphology

| [image: Example feMorphology — Examples of erode and dilate] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feMorphology.svg)

## <a id="feOffsetElement"></a>15.21 Filter primitive ‘feOffset’

This filter primitive offsets the input image relative to its current position in the image space by the specified vector.

This is important for effects like drop shadows.

When applying this filter, the destination location may be offset by a fraction of a pixel in device space. In this case a [high quality viewer](conform.md#ConformingHighQualitySVGViewers) should make use of appropriate interpolation techniques, for example bilinear or bicubic. This is especially recommended for dynamic viewers where this interpolation provides visually smoother movement of images. For static viewers this is less of a concern. Close attention should be made to the [‘image-rendering’](painting.md#ImageRenderingProperty) property setting to determine the authors intent.

‘feOffset’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘dx’](#feOffsetDxAttribute)
- [‘dy’](#feOffsetDyAttribute)

**DOM Interfaces:**
:

- [SVGFEOffsetElement](#InterfaceSVGFEOffsetElement)

*Attribute definitions:*

**<a id="feOffsetDxAttribute"></a>dx = "*[<number>](types.md#DataTypeNumber)*"**
: The amount to offset the input graphic along the x-axis. The offset amount is expressed in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feOffsetDyAttribute"></a>dy = "*[<number>](types.md#DataTypeNumber)*"**
: The amount to offset the input graphic along the y-axis. The offset amount is expressed in the coordinate system established by attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the [‘filter’](#FilterElement) element. If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
[The example](#AnExample) at the start of this chapter makes use of the [‘feOffset’](#feOffsetElement) filter primitive to offset the drop shadow from the original source graphic.

## <a id="feSpecularLightingElement"></a>15.22 Filter primitive ‘feSpecularLighting’

This filter primitive lights a source graphic using the alpha channel as a bump map. The resulting image is an RGBA image based on the light color. The lighting calculation follows the standard specular component of the Phong lighting model. The resulting image depends on the light color, light position and surface geometry of the input bump map. The result of the lighting calculation is added. The filter primitive assumes that the viewer is at infinity in the z direction (i.e., the unit vector in the eye direction is (0,0,1) everywhere).

This filter primitive produces an image which contains the specular reflection part of the lighting calculation. Such a map is intended to be combined with a texture using the *add* term of the *arithmetic* [‘feComposite’](#feCompositeElement) method. Multiple light sources can be simulated by adding several of these light maps before applying it to the texture image.

The resulting RGBA image is computed as follows:

S_r = k_s * pow(N.H, specularExponent) * L_r S_g = k_s * pow(N.H, specularExponent) * L_g S_b = k_s * pow(N.H, specularExponent) * L_b S_a = max(S_r, S_g, S_b)

where

: k_s = specular lighting constant N = surface normal unit vector, a function of x and y H = "halfway" unit vector between eye unit vector and light unit vector L_r,L_g,L_b = RGB components of light
See [‘feDiffuseLighting’](#feDiffuseLightingElement) for definition of N and (L_r, L_g, L_b).

The definition of H reflects our assumption of the constant eye vector E = (0,0,1):

H = (L + E) / Norm(L+E)

where L is the light unit vector.

Unlike the [‘feDiffuseLighting’](#feDiffuseLightingElement), the [‘feSpecularLighting’](#feSpecularLightingElement) filter produces a non-opaque image. This is due to the fact that the specular result (S_r,S_g,S_b,S_a) is meant to be added to the textured image. The alpha channel of the result is the max of the color components, so that where the specular light is zero, no additional coverage is added to the image and a fully white highlight will add opacity.

The [‘feDiffuseLighting’](#feDiffuseLightingElement) and [‘feSpecularLighting’](#feSpecularLightingElement) filters will often be applied together. An implementation may detect this and calculate both maps in one pass, instead of two.

‘feSpecularLighting’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of [descriptive elements](intro.md#TermDescriptiveElement) and exactly one [light source element](intro.md#TermLightSourceElement), in any order.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)
- [‘surfaceScale’](#feSpecularLightingSurfaceScaleAttribute)
- [‘specularConstant’](#feSpecularLightingSpecularConstantAttribute)
- [‘specularExponent’](#feSpecularLightingSpecularExponentAttribute)
- [‘kernelUnitLength’](#feSpecularLightingKernelUnitLengthAttribute)

**DOM Interfaces:**
:

- [SVGFESpecularLightingElement](#InterfaceSVGFESpecularLightingElement)

*Attribute definitions:*

**<a id="feSpecularLightingSurfaceScaleAttribute"></a>surfaceScale = "*[<number>](types.md#DataTypeNumber)*"**
: height of surface when A_in = 1. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpecularLightingSpecularConstantAttribute"></a> specularConstant = "*[<number>](types.md#DataTypeNumber)*"**
: ks in Phong lighting model. In SVG, this can be any non-negative number. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpecularLightingSpecularExponentAttribute"></a> specularExponent = "*[<number>](types.md#DataTypeNumber)*"**
: Exponent for specular term, larger is more "shiny". Range 1.0 to 128.0. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feSpecularLightingKernelUnitLengthAttribute"></a> kernelUnitLength = "[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)"**
: The first number is the <dx> value. The second number is the <dy> value. If the <dy> value is not specified, it defaults to the same value as <dx>. Indicates the intended distance in current filter units (i.e., units as determined by the value of attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute)) for `dx` and `dy`, respectively, in the [surface normal calculation formulas](#SurfaceNormalCalculations). By specifying value(s) for [‘kernelUnitLength’](#feSpecularLightingKernelUnitLengthAttribute), the kernel becomes defined in a scalable, abstract coordinate system. If [‘kernelUnitLength’](#feSpecularLightingKernelUnitLengthAttribute) is not specified, the `dx` and `dy` values should represent very small deltas relative to a given `(x,y)` position, which might be implemented in some cases as one pixel in the intermediate image offscreen bitmap, which is a pixel-based coordinate system, and thus potentially not scalable. For some level of consistency across display media and user agents, it is necessary that a value be provided for at least one of [‘filterRes’](#FilterElementFilterResAttribute) and [‘kernelUnitLength’](#feSpecularLightingKernelUnitLengthAttribute). Discussion of intermediate images are in the [Introduction](#Introduction) and in the description of attribute [‘filterRes’](#FilterElementFilterResAttribute). A negative or zero value is an error (see [Error processing](implnote.md#ErrorProcessing)). [Animatable](animate.md#Animatable): yes.
The light source is defined by one of the child elements [‘feDistantLight’](#feDistantLightElement), [‘fePointLight’](#fePointLightElement) or [‘feDistantLight’](#feDistantLightElement). The light color is specified by property [‘lighting-color’](#LightingColorProperty).

[The example](#AnExample) at the start of this chapter makes use of the [‘feSpecularLighting’](#feSpecularLightingElement) filter primitive to achieve a highly reflective, 3D glowing effect.

## <a id="feTileElement"></a>15.23 Filter primitive ‘feTile’

This filter primitive fills a target rectangle with a repeated, tiled pattern of an input image. The target rectangle is as large as the [filter primitive subregion](#FilterPrimitiveSubRegion) established by the [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute) and [‘height’](#FilterPrimitiveHeightAttribute) attributes on the [‘feTile’](#feTileElement) element.

Typically, the input image has been defined with its own [filter primitive subregion](#FilterPrimitiveSubRegion) in order to define a reference tile. [‘feTile’](#feTileElement) replicates the reference tile in both X and Y to completely fill the target rectangle. The top/left corner of each given tile is at location `(x+i*width,y+j*height)`, where `(x,y)` represents the top/left of the input image's filter primitive subregion, `width` and `height` represent the width and height of the input image's filter primitive subregion, and `i` and `j` can be any integer value. In most cases, the input image will have a smaller [filter primitive subregion](#FilterPrimitiveSubRegion) than the [‘feTile’](#feTileElement) in order to achieve a repeated pattern effect.

Implementers must take appropriate measures in constructing the tiled image to avoid artifacts between tiles, particularly in situations where the user to device transform includes shear and/or rotation. Unless care is taken, interpolation can lead to edge pixels in the tile having opacity values lower or higher than expected due to the interaction of painting adjacent tiles which each have partial overlap with particular pixels.

‘feTile’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘in’](#FilterPrimitiveInAttribute)

**DOM Interfaces:**
:

- [SVGFETileElement](#InterfaceSVGFETileElement)

## <a id="feTurbulenceElement"></a>15.24 Filter primitive ‘feTurbulence’

This filter primitive creates an image using the Perlin turbulence function. It allows the synthesis of artificial textures like clouds or marble. For a detailed description the of the Perlin turbulence function, see "Texturing and Modeling", Ebert et al, AP Professional, 1994. The resulting image will fill the entire [filter primitive subregion](#FilterPrimitiveSubRegion) for this filter primitive.

It is possible to create bandwidth-limited noise by synthesizing only one octave.

The C code below shows the exact algorithm used for this filter effect.

For fractalSum, you get a turbFunctionResult that is aimed at a range of -1 to 1 (the actual result might exceed this range in some cases). To convert to a color value, use the formula `colorValue = ((turbFunctionResult * 255) + 255) / 2`, then clamp to the range 0 to 255.

For turbulence, you get a turbFunctionResult that is aimed at a range of 0 to 1 (the actual result might exceed this range in some cases). To convert to a color value, use the formula `colorValue = (turbFunctionResult * 255)`, then clamp to the range 0 to 255.

The following order is used for applying the pseudo random numbers. An initial seed value is computed based on attribute [‘seed’](#feTurbulenceSeedAttribute). Then the implementation computes the lattice points for R, then continues getting additional pseudo random numbers relative to the last generated pseudo random number and computes the lattice points for G, and so on for B and A.

The generated color and alpha values are in the color space determined by the value of property [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty):

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
      for (j = 0; j < 2; j++)
        fGradient[k][i][j] = (double)(((lSeed = random(lSeed)) % (BSize + BSize)) - BSize) / BSize;
      s = double(sqrt(fGradient[k][i][0] * fGradient[k][i][0] + fGradient[k][i][1] * fGradient[k][i][1]));
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

‘feTurbulence’

**Categories:**
: [Filter primitive element](intro.md#TermFilterPrimitiveElement)
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](#FilterProperty), [‘flood-color’](#FloodColorProperty), [‘flood-opacity’](#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [filter primitive attributes](intro.md#TermFilterPrimitiveAttributes) — [‘x’](#FilterPrimitiveXAttribute), [‘y’](#FilterPrimitiveYAttribute), [‘width’](#FilterPrimitiveWidthAttribute), [‘height’](#FilterPrimitiveHeightAttribute), [‘result’](#FilterPrimitiveResultAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘baseFrequency’](#feTurbulenceBaseFrequencyAttribute)
- [‘numOctaves’](#feTurbulenceNumOctavesAttribute)
- [‘seed’](#feTurbulenceSeedAttribute)
- [‘stitchTiles’](#feTurbulenceStitchTilesAttribute)
- [‘type’](#feTurbulenceTypeAttribute)

**DOM Interfaces:**
:

- [SVGFETurbulenceElement](#InterfaceSVGFETurbulenceElement)

*Attribute definitions:*

**<a id="feTurbulenceBaseFrequencyAttribute"></a>baseFrequency = "*[<number-optional-number>](types.md#DataTypeNumberOptionalNumber)*"**
: The base frequency (frequencies) parameter(s) for the noise function. If two [<number>](types.md#DataTypeNumber)s are provided, the first number represents a base frequency in the X direction and the second value represents a base frequency in the Y direction. If one number is provided, then that value is used for both X and Y. A negative value for base frequency is an error (see [Error processing](implnote.md#ErrorProcessing)). If the attribute is not specified, then the effect is as if a value of 0 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feTurbulenceNumOctavesAttribute"></a>numOctaves = "*[<integer>](types.md#DataTypeInteger)*"**
: The numOctaves parameter for the noise function. If the attribute is not specified, then the effect is as if a value of 1 were specified. [Animatable](animate.md#Animatable): yes.
**<a id="feTurbulenceSeedAttribute"></a>seed = "*[<number>](types.md#DataTypeNumber)*"**
: The starting number for the pseudo random number generator. If the attribute is not specified, then the effect is as if a value of 0 were specified. When the seed number is handed over to the algorithm above it must first be truncated, i.e. rounded to the closest integer value towards zero. [Animatable](animate.md#Animatable): yes.
**<a id="feTurbulenceStitchTilesAttribute"></a>stitchTiles = "*stitch | noStitch*"**
: If stitchTiles="noStitch", no attempt it made to achieve smooth transitions at the border of tiles which contain a turbulence function. Sometimes the result will show clear discontinuities at the tile borders. If stitchTiles="stitch", then the user agent will automatically adjust baseFrequency-x and baseFrequency-y values such that the feTurbulence node's width and height (i.e., the width and height of the current subregion) contains an integral number of the Perlin tile width and height for the first octave. The baseFrequency will be adjusted up or down depending on which way has the smallest relative (not absolute) change as follows: Given the frequency, calculate `lowFreq=floor(width*frequency)/width` and `hiFreq=ceil(width*frequency)/width`. If frequency/lowFreq < hiFreq/frequency then use lowFreq, else use hiFreq. While generating turbulence values, generate lattice vectors as normal for Perlin Noise, except for those lattice points that lie on the right or bottom edges of the active area (the size of the resulting tile). In those cases, copy the lattice vector from the opposite edge of the active area.

If attribute [‘stitchTiles’](#feTurbulenceStitchTilesAttribute) is not specified, then the effect is as if a value of noStitch were specified.

[Animatable](animate.md#Animatable): yes.
**<a id="feTurbulenceTypeAttribute"></a>type = "*fractalNoise | turbulence*"**
: Indicates whether the filter primitive should perform a noise or turbulence function. If attribute [‘type’](#feTurbulenceTypeAttribute) is not specified, then the effect is as if a value of turbulence were specified. [Animatable](animate.md#Animatable): yes.
Example feTurbulence shows the effects of various parameter settings for feTurbulence.

```
<?xml version="1.0"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
          "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="450px" height="325px" viewBox="0 0 450 325"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
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

Example feTurbulence

| [image: Example feTurbulence — Examples of feTurbulence operations] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/filters/feTurbulence.svg)

## <a id="DOMInterfaces"></a>15.25 DOM interfaces

### <a id="InterfaceSVGFilterElement"></a>15.25.1 Interface SVGFilterElement

The [SVGFilterElement](#InterfaceSVGFilterElement) interface corresponds to the [‘filter’](#FilterElement) element.

```
interface SVGFilterElement : SVGElement,
                             SVGURIReference,
                             SVGLangSpace,
                             SVGExternalResourcesRequired,
                             SVGStylable,
                             SVGUnitTypes {

  readonly attribute SVGAnimatedEnumeration filterUnits;
  readonly attribute SVGAnimatedEnumeration primitiveUnits;
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
  readonly attribute SVGAnimatedInteger filterResX;
  readonly attribute SVGAnimatedInteger filterResY;

  void setFilterRes(in unsigned long filterResX, in unsigned long filterResY) raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGFilterElement__filterUnits"></a>**filterUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘filterUnits’](#FilterElementFilterUnitsAttribute) on the given [‘filter’](#FilterElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGFilterElement__primitiveUnits"></a>**primitiveUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘primitiveUnits’](#FilterElementPrimitiveUnitsAttribute) on the given [‘filter’](#FilterElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGFilterElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#FilterElementXAttribute) on the given [‘filter’](#FilterElement) element.

**<a id="__svg__SVGFilterElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#FilterElementYAttribute) on the given [‘filter’](#FilterElement) element.

**<a id="__svg__SVGFilterElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#FilterElementWidthAttribute) on the given [‘filter’](#FilterElement) element.

**<a id="__svg__SVGFilterElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#FilterElementHeightAttribute) on the given [‘filter’](#FilterElement) element.

**<a id="__svg__SVGFilterElement__filterResX"></a>**filterResX** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘filterRes’](#FilterElementFilterResAttribute) on the given [‘filter’](#FilterElement) element. Contains the X component of attribute [‘filterRes’](#FilterElementFilterResAttribute).

**<a id="__svg__SVGFilterElement__filterResY"></a>**filterResY** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘filterRes’](#FilterElementFilterResAttribute) on the given [‘filter’](#FilterElement) element. Contains the Y component (possibly computed automatically) of attribute [‘filterRes’](#FilterElementFilterResAttribute).

**Operations:**
:

**<a id="__svg__SVGFilterElement__setFilterRes"></a>void **setFilterRes**(in unsigned long *filterResX*, in unsigned long *filterResY*)**
:

Sets the values for attribute [‘filterRes’](#FilterElementFilterResAttribute).

**Parameters**
:

1.
unsigned long *filterResX*
The X component of attribute [‘filterRes’](#FilterElementFilterResAttribute).
2.
unsigned long *filterResY*
The Y component of attribute [‘filterRes’](#FilterElementFilterResAttribute).

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGFilterPrimitiveStandardAttributes"></a>15.25.2 Interface SVGFilterPrimitiveStandardAttributes

This interface defines the set of DOM attributes that are common across the filter primitive interfaces.

```
interface SVGFilterPrimitiveStandardAttributes : SVGStylable {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
  readonly attribute SVGAnimatedString result;
};
```

**Attributes:**
:

**<a id="__svg__SVGFilterPrimitiveStandardAttributes__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#FilterPrimitiveXAttribute) on the given element.

**<a id="__svg__SVGFilterPrimitiveStandardAttributes__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#FilterPrimitiveYAttribute) on the given element.

**<a id="__svg__SVGFilterPrimitiveStandardAttributes__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#FilterPrimitiveWidthAttribute) on the given element.

**<a id="__svg__SVGFilterPrimitiveStandardAttributes__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#FilterPrimitiveHeightAttribute) on the given element.

**<a id="__svg__SVGFilterPrimitiveStandardAttributes__result"></a>**result** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘result’](#FilterPrimitiveResultAttribute) on the given element.

### <a id="InterfaceSVGFEBlendElement"></a>15.25.3 Interface SVGFEBlendElement

The [SVGFEBlendElement](#InterfaceSVGFEBlendElement) interface corresponds to the [‘feBlend’](#feBlendElement) element.

```
interface SVGFEBlendElement : SVGElement,
                              SVGFilterPrimitiveStandardAttributes {

  // Blend Mode Types
  const unsigned short SVG_FEBLEND_MODE_UNKNOWN = 0;
  const unsigned short SVG_FEBLEND_MODE_NORMAL = 1;
  const unsigned short SVG_FEBLEND_MODE_MULTIPLY = 2;
  const unsigned short SVG_FEBLEND_MODE_SCREEN = 3;
  const unsigned short SVG_FEBLEND_MODE_DARKEN = 4;
  const unsigned short SVG_FEBLEND_MODE_LIGHTEN = 5;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedString in2;
  readonly attribute SVGAnimatedEnumeration mode;
};
```

**Constants in group “Blend Mode Types”:**
:

**<a id="__svg__SVGFEBlendElement__SVG_FEBLEND_MODE_UNKNOWN"></a>**SVG_FEBLEND_MODE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFEBlendElement__SVG_FEBLEND_MODE_NORMAL"></a>**SVG_FEBLEND_MODE_NORMAL** (unsigned short)**
:

Corresponds to value 'normal'.

**<a id="__svg__SVGFEBlendElement__SVG_FEBLEND_MODE_MULTIPLY"></a>**SVG_FEBLEND_MODE_MULTIPLY** (unsigned short)**
:

Corresponds to value 'multiply'.

**<a id="__svg__SVGFEBlendElement__SVG_FEBLEND_MODE_SCREEN"></a>**SVG_FEBLEND_MODE_SCREEN** (unsigned short)**
:

Corresponds to value 'screen'.

**<a id="__svg__SVGFEBlendElement__SVG_FEBLEND_MODE_DARKEN"></a>**SVG_FEBLEND_MODE_DARKEN** (unsigned short)**
:

Corresponds to value 'darken'.

**<a id="__svg__SVGFEBlendElement__SVG_FEBLEND_MODE_LIGHTEN"></a>**SVG_FEBLEND_MODE_LIGHTEN** (unsigned short)**
:

Corresponds to value 'lighten'.

**Attributes:**
:

**<a id="__svg__SVGFEBlendElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feBlend’](#feBlendElement) element.

**<a id="__svg__SVGFEBlendElement__in2"></a>**in2** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in2’](#feBlendIn2Attribute) on the given [‘feBlend’](#feBlendElement) element.

**<a id="__svg__SVGFEBlendElement__mode"></a>**mode** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘mode’](#feBlendModeAttribute) on the given [‘feBlend’](#feBlendElement) element. Takes one of the SVG_FEBLEND_MODE_* constants defined on this interface.

### <a id="InterfaceSVGFEColorMatrixElement"></a>15.25.4 Interface SVGFEColorMatrixElement

The [SVGFEColorMatrixElement](#InterfaceSVGFEColorMatrixElement) interface corresponds to the [‘feColorMatrix’](#feColorMatrixElement) element.

```
interface SVGFEColorMatrixElement : SVGElement,
                                    SVGFilterPrimitiveStandardAttributes {

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
```

**Constants in group “Color Matrix Types”:**
:

**<a id="__svg__SVGFEColorMatrixElement__SVG_FECOLORMATRIX_TYPE_UNKNOWN"></a>**SVG_FECOLORMATRIX_TYPE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFEColorMatrixElement__SVG_FECOLORMATRIX_TYPE_MATRIX"></a>**SVG_FECOLORMATRIX_TYPE_MATRIX** (unsigned short)**
:

Corresponds to value 'matrix'.

**<a id="__svg__SVGFEColorMatrixElement__SVG_FECOLORMATRIX_TYPE_SATURATE"></a>**SVG_FECOLORMATRIX_TYPE_SATURATE** (unsigned short)**
:

Corresponds to value 'saturate'.

**<a id="__svg__SVGFEColorMatrixElement__SVG_FECOLORMATRIX_TYPE_HUEROTATE"></a>**SVG_FECOLORMATRIX_TYPE_HUEROTATE** (unsigned short)**
:

Corresponds to value 'hueRotate'.

**<a id="__svg__SVGFEColorMatrixElement__SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA"></a>**SVG_FECOLORMATRIX_TYPE_LUMINANCETOALPHA** (unsigned short)**
:

Corresponds to value 'luminanceToAlpha'.

**Attributes:**
:

**<a id="__svg__SVGFEColorMatrixElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feColorMatrix’](#feColorMatrixElement) element.

**<a id="__svg__SVGFEColorMatrixElement__type"></a>**type** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘type’](#feColorMatrixTypeAttribute) on the given [‘feColorMatrix’](#feColorMatrixElement) element. Takes one of the SVG_FECOLORMATRIX_TYPE_* constants defined on this interface.

**<a id="__svg__SVGFEColorMatrixElement__values"></a>**values** (readonly [SVGAnimatedNumberList](types.md#InterfaceSVGAnimatedNumberList))**
:

Corresponds to attribute [‘values’](#feColorMatrixValuesAttribute) on the given [‘feColorMatrix’](#feColorMatrixElement) element.

### <a id="InterfaceSVGFEComponentTransferElement"></a>15.25.5 Interface SVGFEComponentTransferElement

The [SVGFEComponentTransferElement](#InterfaceSVGFEComponentTransferElement) interface corresponds to the [‘feComponentTransfer’](#feComponentTransferElement) element.

```
interface SVGFEComponentTransferElement : SVGElement,
                                          SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedString in1;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEComponentTransferElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feComponentTransfer’](#feComponentTransferElement) element.

### <a id="InterfaceSVGComponentTransferFunctionElement"></a>15.25.6 Interface SVGComponentTransferFunctionElement

This interface defines a base interface used by the component transfer function interfaces.

```
interface SVGComponentTransferFunctionElement : SVGElement {

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

**Constants in group “Component Transfer Types”:**
:

**<a id="__svg__SVGComponentTransferFunctionElement__SVG_FECOMPONENTTRANSFER_TYPE_UNKNOWN"></a>**SVG_FECOMPONENTTRANSFER_TYPE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGComponentTransferFunctionElement__SVG_FECOMPONENTTRANSFER_TYPE_IDENTITY"></a>**SVG_FECOMPONENTTRANSFER_TYPE_IDENTITY** (unsigned short)**
:

Corresponds to value 'identity'.

**<a id="__svg__SVGComponentTransferFunctionElement__SVG_FECOMPONENTTRANSFER_TYPE_TABLE"></a>**SVG_FECOMPONENTTRANSFER_TYPE_TABLE** (unsigned short)**
:

Corresponds to value 'table'.

**<a id="__svg__SVGComponentTransferFunctionElement__SVG_FECOMPONENTTRANSFER_TYPE_DISCRETE"></a>**SVG_FECOMPONENTTRANSFER_TYPE_DISCRETE** (unsigned short)**
:

Corresponds to value 'discrete'.

**<a id="__svg__SVGComponentTransferFunctionElement__SVG_FECOMPONENTTRANSFER_TYPE_LINEAR"></a>**SVG_FECOMPONENTTRANSFER_TYPE_LINEAR** (unsigned short)**
:

Corresponds to value 'linear'.

**<a id="__svg__SVGComponentTransferFunctionElement__SVG_FECOMPONENTTRANSFER_TYPE_GAMMA"></a>**SVG_FECOMPONENTTRANSFER_TYPE_GAMMA** (unsigned short)**
:

Corresponds to value 'gamma'.

**Attributes:**
:

**<a id="__svg__SVGComponentTransferFunctionElement__type"></a>**type** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘type’](#feComponentTransferTypeAttribute) on the given element. Takes one of the SVG_FECOMPONENTTRANSFER_TYPE_* constants defined on this interface.

**<a id="__svg__SVGComponentTransferFunctionElement__tableValues"></a>**tableValues** (readonly [SVGAnimatedNumberList](types.md#InterfaceSVGAnimatedNumberList))**
:

Corresponds to attribute [‘tableValues’](#feComponentTransferTableValuesAttribute) on the given element.

**<a id="__svg__SVGComponentTransferFunctionElement__slope"></a>**slope** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘slope’](#feComponentTransferSlopeAttribute) on the given element.

**<a id="__svg__SVGComponentTransferFunctionElement__intercept"></a>**intercept** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘intercept’](#feComponentTransferInterceptAttribute) on the given element.

**<a id="__svg__SVGComponentTransferFunctionElement__amplitude"></a>**amplitude** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘amplitude’](#feComponentTransferAmplitudeAttribute) on the given element.

**<a id="__svg__SVGComponentTransferFunctionElement__exponent"></a>**exponent** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘exponent’](#feComponentTransferExponentAttribute) on the given element.

**<a id="__svg__SVGComponentTransferFunctionElement__offset"></a>**offset** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘offset’](#feComponentTransferOffsetAttribute) on the given element.

### <a id="InterfaceSVGFEFuncRElement"></a>15.25.7 Interface SVGFEFuncRElement

The [SVGFEFuncRElement](#InterfaceSVGFEFuncRElement) interface corresponds to the [‘feFuncR’](#feFuncRElement) element.

```
interface SVGFEFuncRElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFEFuncGElement"></a>15.25.8 Interface SVGFEFuncGElement

The [SVGFEFuncRElement](#InterfaceSVGFEFuncRElement) interface corresponds to the [‘feFuncG’](#feFuncGElement) element.

```
interface SVGFEFuncGElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFEFuncBElement"></a>15.25.9 Interface SVGFEFuncBElement

The [SVGFEFuncBElement](#InterfaceSVGFEFuncBElement) interface corresponds to the [‘feFuncB’](#feFuncBElement) element.

```
interface SVGFEFuncBElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFEFuncAElement"></a>15.25.10 Interface SVGFEFuncAElement

The [SVGFEFuncAElement](#InterfaceSVGFEFuncAElement) interface corresponds to the [‘feFuncA’](#feFuncAElement) element.

```
interface SVGFEFuncAElement : SVGComponentTransferFunctionElement {
};
```

### <a id="InterfaceSVGFECompositeElement"></a>15.25.11 Interface SVGFECompositeElement

The [SVGFECompositeElement](#InterfaceSVGFECompositeElement) interface corresponds to the [‘feComposite’](#feCompositeElement) element.

```
interface SVGFECompositeElement : SVGElement,
                                  SVGFilterPrimitiveStandardAttributes {

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
```

**Constants in group “Composite Operators”:**
:

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_UNKNOWN"></a>**SVG_FECOMPOSITE_OPERATOR_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_OVER"></a>**SVG_FECOMPOSITE_OPERATOR_OVER** (unsigned short)**
:

Corresponds to value 'over'.

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_IN"></a>**SVG_FECOMPOSITE_OPERATOR_IN** (unsigned short)**
:

Corresponds to value 'in'.

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_OUT"></a>**SVG_FECOMPOSITE_OPERATOR_OUT** (unsigned short)**
:

Corresponds to value 'out'.

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_ATOP"></a>**SVG_FECOMPOSITE_OPERATOR_ATOP** (unsigned short)**
:

Corresponds to value 'atop'.

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_XOR"></a>**SVG_FECOMPOSITE_OPERATOR_XOR** (unsigned short)**
:

Corresponds to value 'xor'.

**<a id="__svg__SVGFECompositeElement__SVG_FECOMPOSITE_OPERATOR_ARITHMETIC"></a>**SVG_FECOMPOSITE_OPERATOR_ARITHMETIC** (unsigned short)**
:

Corresponds to value 'arithmetic'.

**Attributes:**
:

**<a id="__svg__SVGFECompositeElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feComposite’](#feCompositeElement) element.

**<a id="__svg__SVGFECompositeElement__in2"></a>**in2** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in2’](#feCompositeIn2Attribute) on the given [‘feComposite’](#feCompositeElement) element.

**<a id="__svg__SVGFECompositeElement__operator"></a>**operator** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘operator’](#feCompositeOperatorAttribute) on the given [‘feComposite’](#feCompositeElement) element. Takes one of the SVG_FECOMPOSITE_OPERATOR_* constants defined on this interface.

**<a id="__svg__SVGFECompositeElement__k1"></a>**k1** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘k1’](#feCompositeK1Attribute) on the given [‘feComposite’](#feCompositeElement) element.

**<a id="__svg__SVGFECompositeElement__k2"></a>**k2** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘k2’](#feCompositeK2Attribute) on the given [‘feComposite’](#feCompositeElement) element.

**<a id="__svg__SVGFECompositeElement__k3"></a>**k3** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘k3’](#feCompositeK3Attribute) on the given [‘feComposite’](#feCompositeElement) element.

**<a id="__svg__SVGFECompositeElement__k4"></a>**k4** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘k4’](#feCompositeK4Attribute) on the given [‘feComposite’](#feCompositeElement) element.

### <a id="InterfaceSVGFEConvolveMatrixElement"></a>15.25.12 Interface SVGFEConvolveMatrixElement

The [SVGFEConvolveMatrixElement](#InterfaceSVGFEConvolveMatrixElement) interface corresponds to the [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

```
interface SVGFEConvolveMatrixElement : SVGElement,
                                       SVGFilterPrimitiveStandardAttributes {

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
```

**Constants in group “Edge Mode Values”:**
:

**<a id="__svg__SVGFEConvolveMatrixElement__SVG_EDGEMODE_UNKNOWN"></a>**SVG_EDGEMODE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFEConvolveMatrixElement__SVG_EDGEMODE_DUPLICATE"></a>**SVG_EDGEMODE_DUPLICATE** (unsigned short)**
:

Corresponds to value 'duplicate'.

**<a id="__svg__SVGFEConvolveMatrixElement__SVG_EDGEMODE_WRAP"></a>**SVG_EDGEMODE_WRAP** (unsigned short)**
:

Corresponds to value 'wrap'.

**<a id="__svg__SVGFEConvolveMatrixElement__SVG_EDGEMODE_NONE"></a>**SVG_EDGEMODE_NONE** (unsigned short)**
:

Corresponds to value 'none'.

**Attributes:**
:

**<a id="__svg__SVGFEConvolveMatrixElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__orderX"></a>**orderX** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘order’](#feConvolveMatrixElementOrderAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__orderY"></a>**orderY** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘order’](#feConvolveMatrixElementOrderAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__kernelMatrix"></a>**kernelMatrix** (readonly [SVGAnimatedNumberList](types.md#InterfaceSVGAnimatedNumberList))**
:

Corresponds to attribute [‘kernelMatrix’](#feConvolveMatrixElementKernelMatrixAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__divisor"></a>**divisor** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘divisor’](#feConvolveMatrixElementDivisorAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__bias"></a>**bias** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘bias’](#feConvolveMatrixElementBiasAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__targetX"></a>**targetX** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘targetX’](#feConvolveMatrixElementTargetXAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__targetY"></a>**targetY** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘targetY’](#feConvolveMatrixElementTargetYAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__edgeMode"></a>**edgeMode** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘edgeMode’](#feConvolveMatrixElementEdgeModeAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element. Takes one of the SVG_EDGEMODE_* constants defined on this interface.

**<a id="__svg__SVGFEConvolveMatrixElement__kernelUnitLengthX"></a>**kernelUnitLengthX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__kernelUnitLengthY"></a>**kernelUnitLengthY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘kernelUnitLength’](#feConvolveMatrixElementKernelUnitLengthAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

**<a id="__svg__SVGFEConvolveMatrixElement__preserveAlpha"></a>**preserveAlpha** (readonly [SVGAnimatedBoolean](types.md#InterfaceSVGAnimatedBoolean))**
:

Corresponds to attribute [‘preserveAlpha’](#feConvolveMatrixElementPreserveAlphaAttribute) on the given [‘feConvolveMatrix’](#feConvolveMatrixElement) element.

### <a id="InterfaceSVGFEDiffuseLightingElement"></a>15.25.13 Interface SVGFEDiffuseLightingElement

The [SVGFEDiffuseLightingElement](#InterfaceSVGFEDiffuseLightingElement) interface corresponds to the [‘feDiffuseLighting’](#feDiffuseLightingElement) element.

```
interface SVGFEDiffuseLightingElement : SVGElement,
                                        SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber surfaceScale;
  readonly attribute SVGAnimatedNumber diffuseConstant;
  readonly attribute SVGAnimatedNumber kernelUnitLengthX;
  readonly attribute SVGAnimatedNumber kernelUnitLengthY;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEDiffuseLightingElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feDiffuseLighting’](#feDiffuseLightingElement) element.

**<a id="__svg__SVGFEDiffuseLightingElement__surfaceScale"></a>**surfaceScale** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘surfaceScale’](#feDiffuseLightingSurfaceScaleAttribute) on the given [‘feDiffuseLighting’](#feDiffuseLightingElement) element.

**<a id="__svg__SVGFEDiffuseLightingElement__diffuseConstant"></a>**diffuseConstant** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘diffuseConstant’](#feDiffuseLightingDiffuseConstantAttribute) on the given [‘feDiffuseLighting’](#feDiffuseLightingElement) element.

**<a id="__svg__SVGFEDiffuseLightingElement__kernelUnitLengthX"></a>**kernelUnitLengthX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute) on the given [‘feDiffuseLighting’](#feDiffuseLightingElement) element.

**<a id="__svg__SVGFEDiffuseLightingElement__kernelUnitLengthY"></a>**kernelUnitLengthY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘kernelUnitLength’](#feDiffuseLightingKernelUnitLengthAttribute) on the given [‘feDiffuseLighting’](#feDiffuseLightingElement) element.

### <a id="InterfaceSVGFEDistantLightElement"></a>15.25.14 Interface SVGFEDistantLightElement

The [SVGFEDistantLightElement](#InterfaceSVGFEDistantLightElement) interface corresponds to the [‘feDistantLight’](#feDistantLightElement) element.

```
interface SVGFEDistantLightElement : SVGElement {
  readonly attribute SVGAnimatedNumber azimuth;
  readonly attribute SVGAnimatedNumber elevation;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEDistantLightElement__azimuth"></a>**azimuth** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘azimuth’](#feDistantLightAzimuthAttribute) on the given [‘feDistantLight’](#feDistantLightElement) element.

**<a id="__svg__SVGFEDistantLightElement__elevation"></a>**elevation** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘elevation’](#feDistantLightElevationAttribute) on the given [‘feDistantLight’](#feDistantLightElement) element.

### <a id="InterfaceSVGFEPointLightElement"></a>15.25.15 Interface SVGFEPointLightElement

The [SVGFEPointLightElement](#InterfaceSVGFEPointLightElement) interface corresponds to the [‘fePointLight’](#fePointLightElement) element.

```
interface SVGFEPointLightElement : SVGElement {
  readonly attribute SVGAnimatedNumber x;
  readonly attribute SVGAnimatedNumber y;
  readonly attribute SVGAnimatedNumber z;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEPointLightElement__x"></a>**x** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘x’](#fePointLightXAttribute) on the given [‘fePointLight’](#fePointLightElement) element.

**<a id="__svg__SVGFEPointLightElement__y"></a>**y** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘y’](#fePointLightYAttribute) on the given [‘fePointLight’](#fePointLightElement) element.

**<a id="__svg__SVGFEPointLightElement__z"></a>**z** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘z’](#fePointLightZAttribute) on the given [‘fePointLight’](#fePointLightElement) element.

### <a id="InterfaceSVGFESpotLightElement"></a>15.25.16 Interface SVGFESpotLightElement

The [SVGFESpotLightElement](#InterfaceSVGFESpotLightElement) interface corresponds to the [‘feSpotLight’](#feSpotLightElement) element.

```
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

**Attributes:**
:

**<a id="__svg__SVGFESpotLightElement__x"></a>**x** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘x’](#feSpotLightXAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__y"></a>**y** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘y’](#feSpotLightYAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__z"></a>**z** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘z’](#feSpotLightZAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__pointsAtX"></a>**pointsAtX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘pointsAtX’](#feSpotLightPointsAtXAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__pointsAtY"></a>**pointsAtY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘pointsAtY’](#feSpotLightPointsAtYAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__pointsAtZ"></a>**pointsAtZ** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘pointsAtZ’](#feSpotLightPointsAtZAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__specularExponent"></a>**specularExponent** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘specularExponent’](#feSpotLightSpecularExponentAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

**<a id="__svg__SVGFESpotLightElement__limitingConeAngle"></a>**limitingConeAngle** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘limitingConeAngle’](#feSpotLightLimitingConeAngleAttribute) on the given [‘feSpotLight’](#feSpotLightElement) element.

### <a id="InterfaceSVGFEDisplacementMapElement"></a>15.25.17 Interface SVGFEDisplacementMapElement

The [SVGFEDisplacementMapElement](#InterfaceSVGFEDisplacementMapElement) interface corresponds to the [‘feDisplacementMap’](#feDisplacementMapElement) element.

```
interface SVGFEDisplacementMapElement : SVGElement,
                                        SVGFilterPrimitiveStandardAttributes {

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
```

**Constants in group “Channel Selectors”:**
:

**<a id="__svg__SVGFEDisplacementMapElement__SVG_CHANNEL_UNKNOWN"></a>**SVG_CHANNEL_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFEDisplacementMapElement__SVG_CHANNEL_R"></a>**SVG_CHANNEL_R** (unsigned short)**
:

Corresponds to value 'R'.

**<a id="__svg__SVGFEDisplacementMapElement__SVG_CHANNEL_G"></a>**SVG_CHANNEL_G** (unsigned short)**
:

Corresponds to value 'G'.

**<a id="__svg__SVGFEDisplacementMapElement__SVG_CHANNEL_B"></a>**SVG_CHANNEL_B** (unsigned short)**
:

Corresponds to value 'B'.

**<a id="__svg__SVGFEDisplacementMapElement__SVG_CHANNEL_A"></a>**SVG_CHANNEL_A** (unsigned short)**
:

Corresponds to value 'A'.

**Attributes:**
:

**<a id="__svg__SVGFEDisplacementMapElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feDisplacementMap’](#feDisplacementMapElement) element.

**<a id="__svg__SVGFEDisplacementMapElement__in2"></a>**in2** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in2’](#feDisplacementMapIn2Attribute) on the given [‘feDisplacementMap’](#feDisplacementMapElement) element.

**<a id="__svg__SVGFEDisplacementMapElement__scale"></a>**scale** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘scale’](#feDisplacementMapScaleAttribute) on the given [‘feDisplacementMap’](#feDisplacementMapElement) element.

**<a id="__svg__SVGFEDisplacementMapElement__xChannelSelector"></a>**xChannelSelector** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘xChannelSelector’](#feDisplacementMapXChannelSelectorAttribute) on the given [‘feDisplacementMap’](#feDisplacementMapElement) element. Takes one of the SVG_CHANNEL_* constants defined on this interface.

**<a id="__svg__SVGFEDisplacementMapElement__yChannelSelector"></a>**yChannelSelector** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘yChannelSelector’](#feDisplacementMapYChannelSelectorAttribute) on the given [‘feDisplacementMap’](#feDisplacementMapElement) element. Takes one of the SVG_CHANNEL_* constants defined on this interface.

### <a id="InterfaceSVGFEFloodElement"></a>15.25.18 Interface SVGFEFloodElement

The [SVGFEFloodElement](#InterfaceSVGFEFloodElement) interface corresponds to the [‘feFlood’](#feFloodElement) element.

```
interface SVGFEFloodElement : SVGElement,
                              SVGFilterPrimitiveStandardAttributes {
};
```

### <a id="InterfaceSVGFEGaussianBlurElement"></a>15.25.19 Interface SVGFEGaussianBlurElement

The [SVGFEGaussianBlurElement](#InterfaceSVGFEGaussianBlurElement) interface corresponds to the [‘feGaussianBlur’](#feGaussianBlurElement) element.

```
interface SVGFEGaussianBlurElement : SVGElement,
                                     SVGFilterPrimitiveStandardAttributes {

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber stdDeviationX;
  readonly attribute SVGAnimatedNumber stdDeviationY;

  void setStdDeviation(in float stdDeviationX, in float stdDeviationY) raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGFEGaussianBlurElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feGaussianBlur’](#feGaussianBlurElement) element.

**<a id="__svg__SVGFEGaussianBlurElement__stdDeviationX"></a>**stdDeviationX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute) on the given [‘feGaussianBlur’](#feGaussianBlurElement) element. Contains the X component of attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute).

**<a id="__svg__SVGFEGaussianBlurElement__stdDeviationY"></a>**stdDeviationY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute) on the given [‘feGaussianBlur’](#feGaussianBlurElement) element. Contains the Y component (possibly computed automatically) of attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute).

**Operations:**
:

**<a id="__svg__SVGFEGaussianBlurElement__setStdDeviation"></a>void **setStdDeviation**(in float *stdDeviationX*, in float *stdDeviationY*)**
:

Sets the values for attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute).

**Parameters**
:

1.
float *stdDeviationX*
The X component of attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute).
2.
float *stdDeviationY*
The Y component of attribute [‘stdDeviation’](#feGaussianBlurStdDeviationAttribute).

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGFEImageElement"></a>15.25.20 Interface SVGFEImageElement

The [SVGFEImageElement](#InterfaceSVGFEImageElement) interface corresponds to the [‘feImage’](#feImageElement) element.

```
interface SVGFEImageElement : SVGElement,
                              SVGURIReference,
                              SVGLangSpace,
                              SVGExternalResourcesRequired,
                              SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedPreserveAspectRatio preserveAspectRatio;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEImageElement__preserveAspectRatio"></a>**preserveAspectRatio** (readonly [SVGAnimatedPreserveAspectRatio](coords.md#InterfaceSVGAnimatedPreserveAspectRatio))**
:

Corresponds to attribute [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) on the given [‘feImage’](#feImageElement) element.

### <a id="InterfaceSVGFEMergeElement"></a>15.25.21 Interface SVGFEMergeElement

The [SVGFEMergeElement](#InterfaceSVGFEMergeElement) interface corresponds to the [‘feMerge’](#feMergeElement) element.

```
interface SVGFEMergeElement : SVGElement,
                              SVGFilterPrimitiveStandardAttributes {
};
```

### <a id="InterfaceSVGFEMergeNodeElement"></a>15.25.22 Interface SVGFEMergeNodeElement

The [SVGFEMergeNodeElement](#InterfaceSVGFEMergeNodeElement) interface corresponds to the [‘feMergeNode’](#feMergeNodeElement) element.

```
interface SVGFEMergeNodeElement : SVGElement {
  readonly attribute SVGAnimatedString in1;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEMergeNodeElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feMergeNode’](#feMergeNodeElement) element.

### <a id="InterfaceSVGFEMorphologyElement"></a>15.25.23 Interface SVGFEMorphologyElement

The [SVGFEMorphologyElement](#InterfaceSVGFEMorphologyElement) interface corresponds to the [‘feMorphology’](#feMorphologyElement) element.

```
interface SVGFEMorphologyElement : SVGElement,
                                   SVGFilterPrimitiveStandardAttributes {

  // Morphology Operators
  const unsigned short SVG_MORPHOLOGY_OPERATOR_UNKNOWN = 0;
  const unsigned short SVG_MORPHOLOGY_OPERATOR_ERODE = 1;
  const unsigned short SVG_MORPHOLOGY_OPERATOR_DILATE = 2;

  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedEnumeration operator;
  readonly attribute SVGAnimatedNumber radiusX;
  readonly attribute SVGAnimatedNumber radiusY;
};
```

**Constants in group “Morphology Operators”:**
:

**<a id="__svg__SVGFEMorphologyElement__SVG_MORPHOLOGY_OPERATOR_UNKNOWN"></a>**SVG_MORPHOLOGY_OPERATOR_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFEMorphologyElement__SVG_MORPHOLOGY_OPERATOR_ERODE"></a>**SVG_MORPHOLOGY_OPERATOR_ERODE** (unsigned short)**
:

Corresponds to value 'erode'.

**<a id="__svg__SVGFEMorphologyElement__SVG_MORPHOLOGY_OPERATOR_DILATE"></a>**SVG_MORPHOLOGY_OPERATOR_DILATE** (unsigned short)**
:

Corresponds to value 'dilate'.

**Attributes:**
:

**<a id="__svg__SVGFEMorphologyElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feMorphology’](#feMorphologyElement) element.

**<a id="__svg__SVGFEMorphologyElement__operator"></a>**operator** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘operator’](#feMorphologyOperatorAttribute) on the given [‘feMorphology’](#feMorphologyElement) element. Takes one of the SVG_MORPHOLOGY_OPERATOR_* constants defined on this interface.

**<a id="__svg__SVGFEMorphologyElement__radiusX"></a>**radiusX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘radius’](#feMorphologyRadiusAttribute) on the given [‘feMorphology’](#feMorphologyElement) element.

**<a id="__svg__SVGFEMorphologyElement__radiusY"></a>**radiusY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘radius’](#feMorphologyRadiusAttribute) on the given [‘feMorphology’](#feMorphologyElement) element.

### <a id="InterfaceSVGFEOffsetElement"></a>15.25.24 Interface SVGFEOffsetElement

The [SVGFEOffsetElement](#InterfaceSVGFEOffsetElement) interface corresponds to the [‘feOffset’](#feOffsetElement) element.

```
interface SVGFEOffsetElement : SVGElement,
                               SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber dx;
  readonly attribute SVGAnimatedNumber dy;
};
```

**Attributes:**
:

**<a id="__svg__SVGFEOffsetElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feOffset’](#feOffsetElement) element.

**<a id="__svg__SVGFEOffsetElement__dx"></a>**dx** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘dx’](#feOffsetDxAttribute) on the given [‘feOffset’](#feOffsetElement) element.

**<a id="__svg__SVGFEOffsetElement__dy"></a>**dy** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘dy’](#feOffsetDyAttribute) on the given [‘feOffset’](#feOffsetElement) element.

### <a id="InterfaceSVGFESpecularLightingElement"></a>15.25.25 Interface SVGFESpecularLightingElement

The [SVGFESpecularLightingElement](#InterfaceSVGFESpecularLightingElement) interface corresponds to the [‘feSpecularLighting’](#feSpecularLightingElement) element.

```
interface SVGFESpecularLightingElement : SVGElement,
                                         SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedString in1;
  readonly attribute SVGAnimatedNumber surfaceScale;
  readonly attribute SVGAnimatedNumber specularConstant;
  readonly attribute SVGAnimatedNumber specularExponent;
  readonly attribute SVGAnimatedNumber kernelUnitLengthX;
  readonly attribute SVGAnimatedNumber kernelUnitLengthY;
};
```

**Attributes:**
:

**<a id="__svg__SVGFESpecularLightingElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feSpecularLighting’](#feSpecularLightingElement) element.

**<a id="__svg__SVGFESpecularLightingElement__surfaceScale"></a>**surfaceScale** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘surfaceScale’](#feSpecularLightingSurfaceScaleAttribute) on the given [‘feSpecularLighting’](#feSpecularLightingElement) element.

**<a id="__svg__SVGFESpecularLightingElement__specularConstant"></a>**specularConstant** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘specularConstant’](#feSpecularLightingSpecularConstantAttribute) on the given [‘feSpecularLighting’](#feSpecularLightingElement) element.

**<a id="__svg__SVGFESpecularLightingElement__specularExponent"></a>**specularExponent** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘specularExponent’](#feSpecularLightingSpecularExponentAttribute) on the given [‘feSpecularLighting’](#feSpecularLightingElement) element.

**<a id="__svg__SVGFESpecularLightingElement__kernelUnitLengthX"></a>**kernelUnitLengthX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘kernelUnitLength’](#feSpecularLightingKernelUnitLengthAttribute) on the given [‘feSpecularLighting’](#feSpecularLightingElement) element.

**<a id="__svg__SVGFESpecularLightingElement__kernelUnitLengthY"></a>**kernelUnitLengthY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘kernelUnitLength’](#feSpecularLightingKernelUnitLengthAttribute) on the given [‘feSpecularLighting’](#feSpecularLightingElement) element.

### <a id="InterfaceSVGFETileElement"></a>15.25.26 Interface SVGFETileElement

The [SVGFETileElement](#InterfaceSVGFETileElement) interface corresponds to the [‘feTile’](#feTileElement) element.

```
interface SVGFETileElement : SVGElement,
                             SVGFilterPrimitiveStandardAttributes {
  readonly attribute SVGAnimatedString in1;
};
```

**Attributes:**
:

**<a id="__svg__SVGFETileElement__in1"></a>**in1** (readonly [SVGAnimatedString](types.md#InterfaceSVGAnimatedString))**
:

Corresponds to attribute [‘in’](#FilterPrimitiveInAttribute) on the given [‘feTile’](#feTileElement) element.

### <a id="InterfaceSVGFETurbulenceElement"></a>15.25.27 Interface SVGFETurbulenceElement

The [SVGFETurbulenceElement](#InterfaceSVGFETurbulenceElement) interface corresponds to the [‘feTurbulence’](#feTurbulenceElement) element.

```
interface SVGFETurbulenceElement : SVGElement,
                                   SVGFilterPrimitiveStandardAttributes {

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
```

**Constants in group “Turbulence Types”:**
:

**<a id="__svg__SVGFETurbulenceElement__SVG_TURBULENCE_TYPE_UNKNOWN"></a>**SVG_TURBULENCE_TYPE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFETurbulenceElement__SVG_TURBULENCE_TYPE_FRACTALNOISE"></a>**SVG_TURBULENCE_TYPE_FRACTALNOISE** (unsigned short)**
:

Corresponds to value 'fractalNoise'.

**<a id="__svg__SVGFETurbulenceElement__SVG_TURBULENCE_TYPE_TURBULENCE"></a>**SVG_TURBULENCE_TYPE_TURBULENCE** (unsigned short)**
:

Corresponds to value 'turbulence'.

**Constants in group “Stitch Options”:**
:

**<a id="__svg__SVGFETurbulenceElement__SVG_STITCHTYPE_UNKNOWN"></a>**SVG_STITCHTYPE_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGFETurbulenceElement__SVG_STITCHTYPE_STITCH"></a>**SVG_STITCHTYPE_STITCH** (unsigned short)**
:

Corresponds to value 'stitch'.

**<a id="__svg__SVGFETurbulenceElement__SVG_STITCHTYPE_NOSTITCH"></a>**SVG_STITCHTYPE_NOSTITCH** (unsigned short)**
:

Corresponds to value 'noStitch'.

**Attributes:**
:

**<a id="__svg__SVGFETurbulenceElement__baseFrequencyX"></a>**baseFrequencyX** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘baseFrequency’](#feTurbulenceBaseFrequencyAttribute) on the given [‘feTurbulence’](#feTurbulenceElement) element. Contains the X component of the [‘baseFrequency’](#feTurbulenceBaseFrequencyAttribute) attribute.

**<a id="__svg__SVGFETurbulenceElement__baseFrequencyY"></a>**baseFrequencyY** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘baseFrequency’](#feTurbulenceBaseFrequencyAttribute) on the given [‘feTurbulence’](#feTurbulenceElement) element. Contains the Y component of the (possibly computed automatically) [‘baseFrequency’](#feTurbulenceBaseFrequencyAttribute) attribute.

**<a id="__svg__SVGFETurbulenceElement__numOctaves"></a>**numOctaves** (readonly [SVGAnimatedInteger](types.md#InterfaceSVGAnimatedInteger))**
:

Corresponds to attribute [‘numOctaves’](#feTurbulenceNumOctavesAttribute) on the given [‘feTurbulence’](#feTurbulenceElement) element.

**<a id="__svg__SVGFETurbulenceElement__seed"></a>**seed** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘seed’](#feTurbulenceSeedAttribute) on the given [‘feTurbulence’](#feTurbulenceElement) element.

**<a id="__svg__SVGFETurbulenceElement__stitchTiles"></a>**stitchTiles** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘stitchTiles’](#feTurbulenceStitchTilesAttribute) on the given [‘feTurbulence’](#feTurbulenceElement) element. Takes one of the SVG_STITCHTYPE_* constants defined on this interface.

**<a id="__svg__SVGFETurbulenceElement__type"></a>**type** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘type’](#feTurbulenceTypeAttribute) on the given [‘feTurbulence’](#feTurbulenceElement) element. Takes one of the SVG_TURBULENCE_TYPE_* constants defined on this interface.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](masking.md) ⋅ [Next](interact.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
