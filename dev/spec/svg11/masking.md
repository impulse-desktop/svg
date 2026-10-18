<!-- https://www.w3.org/TR/SVG11/masking.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](pservers.md) ⋅ [Next](filters.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 14 Clipping, Masking and Compositing

## <a id="Introduction"></a>14.1 Introduction

SVG supports the following clipping/masking features:

- [clipping paths](#ClippingPaths), which uses any combination of [‘path’](paths.md#PathElement), [‘text’](text.md#TextElement) and [basic shapes](intro.md#TermBasicShape) to serve as the outline of a (in the absence of anti-aliasing) 1-bit mask, where everything on the "inside" of the outline is allowed to show through but everything on the outside is masked out
- [masks](#Masking), which are [container elements](intro.md#TermContainerElement) which can contain [graphics elements](intro.md#TermGraphicsElement) or other container elements which define a set of graphics that is to be used as a semi-transparent mask for compositing foreground objects into the current background.

One key distinction between a [clipping path](#ClippingPaths) and a [mask](#Masking) is that clipping paths are hard masks (i.e., the silhouette consists of either fully opaque pixels or fully transparent pixels, with the possible exception of anti-aliasing along the edge of the silhouette) whereas masks consist of an image where each pixel value indicates the degree of transparency vs. opacity. In a mask, each pixel value can range from fully transparent to fully opaque.

SVG supports only simple alpha blending compositing (see [Simple Alpha Compositing](#SimpleAlphaBlending)).

## <a id="SimpleAlphaBlending"></a>14.2 Simple alpha compositing

Graphics elements are blended into the elements already rendered on the canvas using simple alpha compositing, in which the resulting color and opacity at any given pixel on the canvas is the result of the following formulas (all color values use premultiplied alpha):

```
Er, Eg, Eb    - Element color value
Ea            - Element alpha value
Cr, Cg, Cb    - Canvas color value (before blending)
Ca            - Canvas alpha value (before blending)
Cr', Cg', Cb' - Canvas color value (after blending)
Ca'           - Canvas alpha value (after blending)
Ca' = 1 - (1 - Ea) * (1 - Ca)
Cr' = (1 - Ea) * Cr + Er
Cg' = (1 - Ea) * Cg + Eg
Cb' = (1 - Ea) * Cb + Eb
```

The following rendering properties, which provide information about the color space in which to perform the compositing operations, apply to compositing operations:

- [‘color-interpolation’](painting.md#ColorInterpolationProperty)
- [‘color-rendering’](painting.md#ColorRenderingProperty)

## <a id="ClippingPaths"></a>14.3 Clipping paths

### <a id="ClippingPathsIntro"></a>14.3.1 Introduction

The clipping path restricts the region to which paint can be applied. Conceptually, any parts of the drawing that lie outside of the region bounded by the currently active clipping path are not drawn. A clipping path can be thought of as a mask wherein those pixels outside the clipping path are black with an alpha value of zero and those pixels inside the clipping path are white with an alpha value of one (with the possible exception of anti-aliasing along the edge of the silhouette).

### <a id="InitialClippingPath"></a>14.3.2 The initial clipping path

When an [‘svg’](struct.md#SVGElement) element is either the root element in the document or is embedded within a document whose layout is determined according to the layout rules of CSS or XSL, then the user agent must establish an initial clipping path for the SVG document fragment. The [‘overflow’](#OverflowProperty) and [‘clip’](#ClipProperty) properties along with additional SVG user agent processing rules determine the initial clipping path which the user agent establishes for the SVG document fragment:

### <a id="OverflowAndClipProperties"></a>14.3.3 The ‘overflow’ and ‘clip’ properties

**<a id="OverflowProperty"></a>‘overflow’**
:

| *Value:* | visible \| hidden \| scroll \| auto \| inherit |
| --- | --- |
| *Initial:* | see prose |
| *Applies to:* | [elements which establish a new viewport](coords.md#ElementsThatEstablishViewports), [‘pattern’](pservers.md#PatternElement) elements and [‘marker’](painting.md#MarkerElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘overflow’](#OverflowProperty) property has the same parameter values and has the same meaning as defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 11.1.1); however, the following additional points apply:

- The [‘overflow’](#OverflowProperty) property applies to [elements that establish new viewports](coords.md#ElementsThatEstablishViewports) (e.g., [‘svg’](struct.md#SVGElement) elements), [‘pattern’](pservers.md#PatternElement) elements and [‘marker’](painting.md#MarkerElement) elements. For all other elements, the property has no effect (i.e., a clipping rectangle is not created).
- For those elements to which the [‘overflow’](#OverflowProperty) property can apply, if the [‘overflow’](#OverflowProperty) property has the value hidden or scroll, the effect is that a new clipping path in the shape of a rectangle is created. The result is equivalent to defining a [‘clipPath’](#ClipPathElement) element whose content is a [‘rect’](shapes.md#RectElement) element which defines the equivalent rectangle, and then specifying the <uri> of this [‘clipPath’](#ClipPathElement) element on the [‘clip-path’](#ClipPathProperty) property for the given element.
- If the [‘overflow’](#OverflowProperty) property has a value other than hidden or scroll, the property has no effect (i.e., a clipping rectangle is not created).
- Within SVG content, the value auto is equivalent to the value visible.
- When an [outermost svg element](intro.md#TermOutermostSVGElement) is embedded inline within a parent XML grammar which uses CSS layout ([[CSS2](refs.md#ref-CSS2)], chapter 9) or XSL formatting [[XSL](refs.md#ref-XSL)], if the [‘overflow’](#OverflowProperty) property has the value hidden or scroll, then the user agent will establish an initial clipping path equal to the bounds of the initial [viewport](coords.md#SVGViewport); otherwise, the initial clipping path is set according to the clipping rules as defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 11.1.1).
- When an [outermost svg element](intro.md#TermOutermostSVGElement) is stand-alone or embedded inline within a parent XML grammar which does not use CSS layout or XSL formatting, the [‘overflow’](#OverflowProperty) property on the [outermost svg element](intro.md#TermOutermostSVGElement) is ignored for the purposes of visual rendering and the initial clipping path is set to the bounds of the initial [viewport](coords.md#SVGViewport).
- The initial value for ‘overflow’ as defined in [CSS2-overflow] is 'visible', and this applies also to the root [‘svg’](struct.md#SVGElement) element; however, for child elements of an SVG document, SVG's [user agent style sheet](styling.md#UAStyleSheet) overrides this initial value and sets the ‘overflow’ property on [elements that establish new viewports](coords.md#ElementsThatEstablishViewports) (e.g., [‘svg’](struct.md#SVGElement) elements), [‘pattern’](pservers.md#PatternElement) elements and [‘marker’](painting.md#MarkerElement) elements to the value 'hidden'.

As a result of the above, the default behavior of SVG user agents is to establish a clipping path to the bounds of the initial [viewport](coords.md#SVGViewport) and to establish a new clipping path for each [element which establishes a new viewport](coords.md#ElementsThatEstablishViewports) and each [‘pattern’](pservers.md#PatternElement) and [‘marker’](painting.md#MarkerElement) element.

For related information, see [Clip to viewport vs. clip to ‘viewBox’](#AutoClipAtViewportNotViewBox).

**<a id="ClipProperty"></a>‘clip’**
:

| *Value:* | <shape> \| auto \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [elements which establish a new viewport](coords.md#ElementsThatEstablishViewports), [‘pattern’](pservers.md#PatternElement) elements and [‘marker’](painting.md#MarkerElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘clip’](#ClipProperty) property has the same parameter values as defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 11.1.2). Unitless values, which indicate current user coordinates, are permitted on the coordinate values on the <shape>. The value of auto defines a clipping path along the bounds of the viewport created by the given element.

### <a id="AutoClipAtViewportNotViewBox"></a>14.3.4 Clip to viewport vs. clip to ‘viewBox’

It is important to note that initial values for the [‘overflow’](#OverflowProperty) and [‘clip’](#ClipProperty) properties and the [user agent style sheet](styling.md#UAStyleSheet) will result in an initial clipping path that is set to the bounds of the initial viewport. When attributes [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attributes are specified, it is sometime desirable that the clipping path be set to the bounds of the [‘viewBox’](coords.md#ViewBoxAttribute) instead of the viewport (or reference rectangle, in the case of [‘marker’](painting.md#MarkerElement) and [‘pattern’](pservers.md#PatternElement) elements), particularly when [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) specifies uniform scaling and the aspect ratio of the [‘viewBox’](coords.md#ViewBoxAttribute) does not match the aspect ratio of the viewport.

To set the initial clipping path to the bounds of the [‘viewBox’](coords.md#ViewBoxAttribute), set the bounds of [‘clip’](#ClipProperty) property to the same rectangle as specified on the [‘viewBox’](coords.md#ViewBoxAttribute) attribute. (Note that the parameters do not match. [‘clip’](#ClipProperty) takes values <top>, <right>,<bottom> and <left>, whereas [‘viewBox’](coords.md#ViewBoxAttribute) takes values <min-x>, <min-y>, <width> and <height>.)

### <a id="EstablishingANewClippingPath"></a>14.3.5 Establishing a new clipping path: the ‘clipPath’ element

<a id="ClipPathElement"></a>A clipping path is defined with a [‘clipPath’](#ClipPathElement) element. A clipping path is used/referenced using the [‘clip-path’](#ClipPathProperty) property.

A [‘clipPath’](#ClipPathElement) element can contain [‘path’](paths.md#PathElement) elements, [‘text’](text.md#TextElement) elements, [basic shapes](shapes.md) (such as [‘circle’](shapes.md#CircleElement)) or a [‘use’](struct.md#UseElement) element. If a [‘use’](struct.md#UseElement) element is a child of a [‘clipPath’](#ClipPathElement) element, it must directly reference [‘path’](paths.md#PathElement), [‘text’](text.md#TextElement) or [basic shape](shapes.md) elements. Indirect references are an error (see [Error processing](implnote.md#ErrorProcessing)).

The raw geometry of each child element exclusive of rendering properties such as [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-width’](painting.md#StrokeWidthProperty) within a [‘clipPath’](#ClipPathElement) conceptually defines a 1-bit mask (with the possible exception of anti-aliasing along the edge of the geometry) which represents the silhouette of the graphics associated with that element. Anything outside the outline of the object is masked out. If a child element is made invisible by [‘display’](painting.md#DisplayProperty) or [‘visibility’](painting.md#VisibilityProperty) it does not contribute to the clipping path. When the [‘clipPath’](#ClipPathElement) element contains multiple child elements, the silhouettes of the child elements are logically OR'd together to create a single silhouette which is then used to restrict the region onto which paint can be applied. Thus, a point is inside the clipping path if it is inside any of the children of the [‘clipPath’](#ClipPathElement).

For a given graphics element, the actual clipping path used will be the intersection of the clipping path specified by its [‘clip-path’](#ClipPathProperty) property (if any) with any clipping paths on its ancestors, as specified by the [‘clip-path’](#ClipPathProperty) property on the ancestor elements, or by the [‘overflow’](#OverflowProperty) property on ancestor [elements which establish a new viewport](coords.md#ElementsThatEstablishViewports). Also, see the discussion of [the initial clipping path](#InitialClippingPath).)

A couple of notes:

- The [‘clipPath’](#ClipPathElement) element itself and its child elements do *not* inherit clipping paths from the ancestors of the [‘clipPath’](#ClipPathElement) element.
- The [‘clipPath’](#ClipPathElement) element or any of its children can specify property [‘clip-path’](#ClipPathProperty). If a valid [‘clip-path’](#ClipPathProperty) reference is placed on a [‘clipPath’](#ClipPathElement) element, the resulting clipping path is the intersection of the contents of the [‘clipPath’](#ClipPathElement) element with the referenced clipping path. If a valid [‘clip-path’](#ClipPathProperty) reference is placed on one of the children of a [‘clipPath’](#ClipPathElement) element, then the given child element is clipped by the referenced clipping path before OR'ing the silhouette of the child element with the silhouettes of the other child elements.
- An empty clipping path will completely clip away the element that had the [‘clip-path’](#ClipPathProperty) property applied.

‘clipPath’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [‘text’](text.md#TextElement)
- [‘use’](struct.md#UseElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](#ClipProperty), [‘clip-path’](#ClipPathProperty), [‘clip-rule’](#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](#MaskProperty), [‘opacity’](#OpacityProperty), [‘overflow’](#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘clipPathUnits’](#ClipPathElementClipPathUnitsAttribute)

**DOM Interfaces:**
:

- [SVGClipPathElement](#InterfaceSVGClipPathElement)

*Attribute definitions:*

**<a id="ClipPathElementClipPathUnitsAttribute"></a>clipPathUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for the contents of the [‘clipPath’](#ClipPathElement). If clipPathUnits="userSpaceOnUse", the contents of the [‘clipPath’](#ClipPathElement) represent values in the current user coordinate system in place at the time when the [‘clipPath’](#ClipPathElement) element is referenced (i.e., the user coordinate system for the element referencing the [‘clipPath’](#ClipPathElement) element via the [‘clip-path’](#ClipPathProperty) property). If clipPathUnits="objectBoundingBox", then the user coordinate system for the contents of the [‘clipPath’](#ClipPathElement) element is established using the bounding box of the element to which the clipping path is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)). If attribute [‘clipPathUnits’](#ClipPathElementClipPathUnitsAttribute) is not specified, then the effect is as if a value of 'userSpaceOnUse' were specified. [Animatable](animate.md#Animatable): yes.
[Properties](styling.md#SVGStylingProperties) inherit into the [‘clipPath’](#ClipPathElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘clipPath’](#ClipPathElement) element.

[‘clipPath’](#ClipPathElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘clip-path’](#ClipPathProperty) property. The [‘display’](painting.md#DisplayProperty) property does not apply to the [‘clipPath’](#ClipPathElement) element; thus, [‘clipPath’](#ClipPathElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘clipPath’](#ClipPathElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘clipPath’](#ClipPathElement) element or any of its ancestors is set to none.

**<a id="ClipPathProperty"></a>‘clip-path’**
:

| *Value:* | [<funciri>](types.md#DataTypeFuncIRI) \| none \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [container elements](intro.md#TermContainerElement), [graphics elements](intro.md#TermGraphicsElement) and [‘clipPath’](#ClipPathElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**[<funciri>](types.md#DataTypeFuncIRI)**
: An [IRI reference](linking.md#IRIReference) to another graphical object within the same SVG document fragment which will be used as the clipping path. If the [IRI reference](linking.md#IRIReference) is not valid (e.g it points to an object that doesn't exist or the object is not a [‘clipPath’](#ClipPathElement) element) the [‘clip-path’](#ClipPathProperty) property must be treated as if it hadn't been specified.
**<a id="ClipRuleProperty"></a>‘clip-rule’**
:

| *Value:* | nonzero \| evenodd \| inherit |
| --- | --- |
| *Initial:* | nonzero |
| *Applies to:* | graphics elements within a [‘clipPath’](#ClipPathElement) element |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**nonzero**
: See description of [‘fill-rule’](painting.md#FillRuleProperty) property.
**evenodd**
: See description of [‘fill-rule’](painting.md#FillRuleProperty) property.
The [‘clip-rule’](#ClipRuleProperty) property only applies to graphics elements that are contained within a [‘clipPath’](#ClipPathElement) element. The following fragment of code will cause an evenodd clipping rule to be applied to the clipping path because [‘clip-rule’](#ClipRuleProperty) is specified on the [‘path’](paths.md#PathElement) element that defines the clipping shape:

```
<g clip-rule="nonzero">
  <clipPath id="MyClip">
    <path d="..." clip-rule="evenodd" />
  </clipPath>
  <rect clip-path="url(#MyClip)" ... />
</g>
```

whereas the following fragment of code will *not* cause an evenodd clipping rule to be applied because the [‘clip-rule’](#ClipRuleProperty) is specified on the referencing element, not on the object defining the clipping shape:

```
<g clip-rule="nonzero">
  <clipPath id="MyClip">
    <path d="..." />
  </clipPath>
  <rect clip-path="url(#MyClip)" clip-rule="evenodd" ... />
</g>
```

### <a id="clipPath-geometry"></a>14.3.6 Clipping paths, geometry, and pointer events

A clipping path is conceptually equivalent to a custom viewport for the referencing element. Thus, it affects the rendering of an element, but not the element's inherent geometry. The bounding box of a clipped element (that is, an element which references a ‘clipPath’ element via a ‘clip-path’ property, or a child of the referencing element) must remain the same as if it were not clipped.

By default, [pointer-events](interact.md#PointerEventsProperty) must not be dispatched on the clipped (non-visible) regions of a shape. For example, a circle with a radius of 10 which is clipped to a circle with a radius of 5 will not receive 'click' events outside the smaller radius. Later versions of SVG may define new properties to enable fine-grained control over the interactions between hit testing and clipping.

## <a id="Masking"></a>14.4 Masking

<a id="MaskElement"></a>In SVG, you can specify that any other graphics object or [‘g’](struct.md#GElement) element can be used as an alpha mask for compositing the current object into the background.

A mask is defined with a [‘mask’](#MaskElement) element. A mask is used/referenced using the [‘mask’](#MaskProperty) property.

A [‘mask’](#MaskElement) can contain any graphical elements or [container elements](intro.md#TermContainerElement) such as a [‘g’](struct.md#GElement).

It is an error if the [‘mask’](#MaskProperty) property references a non-existent object or if the referenced object is not a [‘mask’](#MaskElement) element (see [Error Processing](implnote.md#ErrorProcessing)).

The effect is as if the child elements of the [‘mask’](#MaskElement) are rendered into an offscreen image which has been initialized to transparent black. Any graphical object which uses/references the given [‘mask’](#MaskElement) element will be painted onto the background through the mask, thus completely or partially masking out parts of the graphical object.

For any graphics object that is used as a mask, the mask value at any point is computed from the color channel values and alpha channel value as follows. First a luminance value is computed from the color channel values:

- If the computed value of [‘color-interpolation’](painting.md#ColorInterpolationProperty) on the [‘mask’](#MaskElement) element is linearRGB, first convert the original image color values (potentially in the sRGB color space) to the linear RGB color space (see [Rendering properties](painting.md#RenderingProperties)). Then, using non-premultiplied linear RGB color values, apply the luminance-to-alpha coefficients (as defined in the [‘feColorMatrix’](filters.md#feColorMatrixElement) filter primitive) to convert the linear RGB color values to linear luminance values.
- If the computed value of [‘color-interpolation’](painting.md#ColorInterpolationProperty) on the [‘mask’](#MaskElement) element is sRGB then the luminance value is calculated by taking the non-premultiplied RGB color values, applying the luminance-to-alpha coefficients (as defined in the [‘feColorMatrix’](filters.md#feColorMatrixElement) filter primitive) to convert the RGB color values to luminance values.

Finally if the graphics object also includes an alpha channel, then the computed luminance value is multiplied by the corresponding alpha value to produce the mask value.

For a four-channel RGBA graphics object that is used as a mask, both the color channels and the alpha channel of the mask contribute to the masking operation. The alpha mask that is used to composite the current object into the background represents the product of the luminance of the color channels (see previous paragraph) and the alpha channel from the mask.

For a three-channel RGB graphics object that is used as a mask (e.g., when referencing a 3-channel image file), the effect is as if the object were converted into a 4-channel RGBA image with the alpha channel uniformly set to 1.

For a single-channel image that is used as a mask (e.g., when referencing a 1-channel grayscale image file), the effect is as if the object were converted into a 4-channel RGBA image, where the single channel from the referenced object is used to compute the three color channels and the alpha channel is uniformly set to 1. Note that when referencing a grayscale image file, the transfer curve relating the encoded grayscale values to linear light values must be taken into account when computing the color channels.

The effect of a mask is identical to what would have happened if there were no mask but instead the alpha channel of the given object were multiplied with the mask's resulting alpha values (i.e., the product of the mask's luminance from its color channels multiplied by the mask's alpha channel).

Note that SVG [‘path’](paths.md#PathElement)s, shapes (e.g., [‘circle’](shapes.md#CircleElement)) and [‘text’](text.md#TextElement) are all treated as four-channel RGBA images for the purposes of masking operations.

<a id="ExampleMask01"></a>Example mask01 uses an image to mask a rectangle.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="3cm" viewBox="0 0 800 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>Example mask01 - blue text masked with gradient against red background
  </desc>
  <defs>
    <linearGradient id="Gradient" gradientUnits="userSpaceOnUse"
                    x1="0" y1="0" x2="800" y2="0">
      <stop offset="0" stop-color="white" stop-opacity="0" />
      <stop offset="1" stop-color="white" stop-opacity="1" />
    </linearGradient>
    <mask id="Mask" maskUnits="userSpaceOnUse"
          x="0" y="0" width="800" height="300">
      <rect x="0" y="0" width="800" height="300" fill="url(#Gradient)"  />
    </mask>
    <text id="Text" x="400" y="200" 
          font-family="Verdana" font-size="100" text-anchor="middle" >
      Masked text
    </text>
  </defs>

  <!-- Draw a pale red rectangle in the background -->
  <rect x="0" y="0" width="800" height="300" fill="#FF8080" />
  
  <!-- Draw the text string twice. First, filled blue, with the mask applied.
       Second, outlined in black without the mask. -->
  <use xlink:href="#Text" fill="blue" mask="url(#Mask)" />
  <use xlink:href="#Text" fill="none" stroke="black" stroke-width="2" />
</svg>
```

Example mask01

| [image: Example mask01 — blue text masked with gradient against red background] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/masking/mask01.svg)

‘mask’

**Categories:**
: [Container element](intro.md#TermContainerElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](struct.md#DefsElement), [‘g’](struct.md#GElement), [‘svg’](struct.md#SVGElement), [‘symbol’](struct.md#SymbolElement), [‘use’](struct.md#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement)
- [‘a’](linking.md#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](struct.md#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](struct.md#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](#ClipProperty), [‘clip-path’](#ClipPathProperty), [‘clip-rule’](#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](#MaskProperty), [‘opacity’](#OpacityProperty), [‘overflow’](#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘x’](#MaskElementXAttribute)
- [‘y’](#MaskElementYAttribute)
- [‘width’](#MaskElementWidthAttribute)
- [‘height’](#MaskElementHeightAttribute)
- [‘maskUnits’](#MaskElementMaskUnitsAttribute)
- [‘maskContentUnits’](#MaskElementMaskContentUnitsAttribute)

**DOM Interfaces:**
:

- [SVGMaskElement](#InterfaceSVGMaskElement)

*Attribute definitions:*

**<a id="MaskElementMaskUnitsAttribute"></a>maskUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for attributes [‘x’](#MaskElementXAttribute), [‘y’](#MaskElementYAttribute), [‘width’](#MaskElementWidthAttribute) and [‘height’](#MaskElementHeightAttribute). If maskUnits="userSpaceOnUse", [‘x’](#MaskElementXAttribute), [‘y’](#MaskElementYAttribute), [‘width’](#MaskElementWidthAttribute) and [‘height’](#MaskElementHeightAttribute) represent values in the current user coordinate system in place at the time when the [‘mask’](#MaskElement) element is referenced (i.e., the user coordinate system for the element referencing the [‘mask’](#MaskElement) element via the [‘mask’](#MaskProperty) property). If maskUnits="objectBoundingBox", [‘x’](#MaskElementXAttribute), [‘y’](#MaskElementYAttribute), [‘width’](#MaskElementWidthAttribute) and [‘height’](#MaskElementHeightAttribute) represent fractions or percentages of the bounding box of the element to which the mask is applied. (See [Object bounding box units](coords.md#ObjectBoundingBox).) If attribute [‘maskUnits’](#MaskElementMaskUnitsAttribute) is not specified, then the effect is as if a value of 'objectBoundingBox' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MaskElementMaskContentUnitsAttribute"></a>maskContentUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for the contents of the [‘mask’](#MaskElement). If maskContentUnits="userSpaceOnUse", the user coordinate system for the contents of the [‘mask’](#MaskElement) element is the current user coordinate system in place at the time when the [‘mask’](#MaskElement) element is referenced (i.e., the user coordinate system for the element referencing the [‘mask’](#MaskElement) element via the [‘mask’](#MaskProperty) property). If maskContentUnits="objectBoundingBox", the user coordinate system for the contents of the [‘mask’](#MaskElement) is established using the bounding box of the element to which the mask is applied. (See [Object bounding box units](coords.md#ObjectBoundingBox).) If attribute [‘maskContentUnits’](#MaskElementMaskContentUnitsAttribute) is not specified, then the effect is as if a value of 'userSpaceOnUse' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MaskElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of one corner of the rectangle for the largest possible offscreen buffer. Note that the clipping path used to render any graphics within the mask will consist of the intersection of the current clipping path associated with the given object and the rectangle defined by [‘x’](#MaskElementXAttribute), [‘y’](#MaskElementYAttribute), [‘width’](#MaskElementWidthAttribute) and [‘height’](#MaskElementHeightAttribute). If the attribute is not specified, the effect is as if a value of '-10%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MaskElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of one corner of the rectangle for the largest possible offscreen buffer. If the attribute is not specified, the effect is as if a value of '-10%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MaskElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: The width of the largest possible offscreen buffer. Note that the clipping path used to render any graphics within the mask will consist of the intersection of the current clipping path associated with the given object and the rectangle defined by [‘x’](#MaskElementXAttribute), [‘y’](#MaskElementYAttribute), [‘width’](#MaskElementWidthAttribute) and [‘height’](#MaskElementHeightAttribute). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. If the attribute is not specified, the effect is as if a value of '120%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MaskElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: The height of the largest possible offscreen buffer. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. If the attribute is not specified, the effect is as if a value of '120%' were specified. [Animatable](animate.md#Animatable): yes.
[Properties](styling.md#SVGStylingProperties) inherit into the [‘mask’](#MaskElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘mask’](#MaskElement) element.

[‘mask’](#MaskElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘mask’](#MaskProperty) property. The [‘opacity’](#OpacityProperty), [‘filter’](filters.md#FilterProperty) and [‘display’](painting.md#DisplayProperty) properties do not apply to the [‘mask’](#MaskElement) element; thus, [‘mask’](#MaskElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘mask’](#MaskElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘mask’](#MaskElement) element or any of its ancestors is set to none.

The following is a description of the [‘mask’](#MaskProperty) property.

**<a id="MaskProperty"></a>‘mask’**
:

| *Value:* | [<funciri>](types.md#DataTypeFuncIRI) \| none \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [container elements](intro.md#TermContainerElement) and [graphics elements](intro.md#TermGraphicsElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**[<funciri>](types.md#DataTypeFuncIRI)**
: A [IRI reference](linking.md#IRIReference) to another graphical object which will be used as the mask.
## <a id="ObjectAndGroupOpacityProperties"></a>14.5 Object and group opacity: the ‘opacity’ property

There are several opacity properties within SVG:

- [‘fill-opacity’](painting.md#FillOpacityProperty), which specifies the opacity of a fill operation;
- [‘stroke-opacity’](painting.md#StrokeOpacityProperty), which specifies the opacity of a stroking operation;
- [‘stop-opacity’](pservers.md#StopOpacityProperty), which specifies the opacity of a gradient stop; and
- [‘opacity’](#OpacityProperty), which specifies object/group opacity and which is described in this section.

Except for object/group opacity (described just below), all other opacity properties are involved in intermediate rendering operations. Object/group opacity can be thought of conceptually as a postprocessing operation. Conceptually, after the object/group is rendered into an RGBA offscreen image, the object/group opacity setting specifies how to blend the offscreen image into the current background.

**<a id="OpacityProperty"></a>‘opacity’**
:

| *Value:* | <opacity-value> \| inherit |
| --- | --- |
| *Initial:* | 1 |
| *Applies to:* | [container elements](intro.md#TermContainerElement) (except [‘mask’](#MaskElement)) and [graphics elements](intro.md#TermGraphicsElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**<opacity-value>**
: The uniform opacity setting to be applied across an entire object, as a [<number>](types.md#DataTypeNumber). Any values outside the range 0.0 (fully transparent) to 1.0 (fully opaque) will be clamped to this range. (See [Clamping values which are restricted to a particular range](implnote.md#RangeClamping).) If the object is a container element such as a [‘g’](struct.md#GElement), then the effect is as if the contents of the [‘g’](struct.md#GElement) were blended against the current background using a mask where the value of each pixel of the mask is <opacity-value>. (See [Simple alpha compositing](#SimpleAlphaBlending).)
<a id="ExampleOpacity01"></a>Example opacity01 illustrates various usage of the [‘opacity’](#OpacityProperty) property on elements and groups.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3.5cm" viewBox="0 0 1200 350"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example opacity01 - opacity property</desc>

  <rect x="1" y="1" width="1198" height="348"
        fill="none" stroke="blue" />

  <!-- Background blue rectangle -->
  <rect x="100" y="100" width="1000" height="150" fill="#0000ff"  />

  <!-- Red circles going from opaque to nearly transparent -->
  <circle cx="200" cy="100" r="50" fill="red" opacity="1"  />
  <circle cx="400" cy="100" r="50" fill="red" opacity=".8"  />
  <circle cx="600" cy="100" r="50" fill="red" opacity=".6"  />
  <circle cx="800" cy="100" r="50" fill="red" opacity=".4"  />
  <circle cx="1000" cy="100" r="50" fill="red" opacity=".2"  />

  <!-- Opaque group, opaque circles -->
  <g opacity="1" >
    <circle cx="182.5" cy="250" r="50" fill="red" opacity="1"  />
    <circle cx="217.5" cy="250" r="50" fill="green" opacity="1"  />
  </g>
  <!-- Group opacity: .5, opacity circles -->
  <g opacity=".5" >
    <circle cx="382.5" cy="250" r="50" fill="red" opacity="1"  />
    <circle cx="417.5" cy="250" r="50" fill="green" opacity="1"  />
  </g>
  <!-- Opaque group, semi-transparent green over red -->
  <g opacity="1" >
    <circle cx="582.5" cy="250" r="50" fill="red" opacity=".5"  />
    <circle cx="617.5" cy="250" r="50" fill="green" opacity=".5"  />
  </g>
  <!-- Opaque group, semi-transparent red over green -->
  <g opacity="1" >
    <circle cx="817.5" cy="250" r="50" fill="green" opacity=".5"  />
    <circle cx="782.5" cy="250" r="50" fill="red" opacity=".5"  />
  </g>
  <!-- Group opacity .5, semi-transparent green over red -->
  <g opacity=".5" >
    <circle cx="982.5" cy="250" r="50" fill="red" opacity=".5"  />
    <circle cx="1017.5" cy="250" r="50" fill="green" opacity=".5"  />
  </g>
</svg>
```

Example opacity01

| [image: Example opacity01 — opacity property] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/masking/opacity01.svg)

In the example above, the top row of circles have differing opacities, ranging from 1.0 to 0.2. The bottom row illustrates five [‘g’](struct.md#GElement) elements, each of which contains overlapping red and green circles, as follows:

- The first group shows the opaque case for reference. The group has opacity of 1, as do the circles.
- The second group shows group opacity when the elements in the group are opaque.
- The third and fourth group show that opacity is not commutative. In the third group (which has opacity of 1), a semi-transparent green circle is drawn on top of a semi-transparent red circle, whereas in the fourth group a semi-transparent red circle is drawn on top of a semi-transparent green circle. Note that area where the two circles intersect display different colors. The third group shows more green color in the intersection area, whereas the fourth group shows more red color.
- The fifth group shows the multiplicative effect of opacity settings. Both the circles and the group itself have opacity settings of .5. The result is that the portion of the red circle which does not overlap with the green circle (i.e., the top/right of the red circle) will blend into the blue rectangle with accumulative opacity of .25 (i.e., .5*.5), which, after blending into the blue rectangle, results in a blended color which is 25% red and 75% blue.

## <a id="DOMInterfaces"></a>14.6 DOM interfaces

### <a id="InterfaceSVGClipPathElement"></a>14.6.1 Interface SVGClipPathElement

The [SVGClipPathElement](#InterfaceSVGClipPathElement) interface corresponds to the [‘clipPath’](#ClipPathElement) element.

```
interface SVGClipPathElement : SVGElement,
                               SVGTests,
                               SVGLangSpace,
                               SVGExternalResourcesRequired,
                               SVGStylable,
                               SVGTransformable,
                               SVGUnitTypes {
  readonly attribute SVGAnimatedEnumeration clipPathUnits;
};
```

**Attributes:**
:

**<a id="__svg__SVGClipPathElement__clipPathUnits"></a>**clipPathUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘clipPathUnits’](#ClipPathElementClipPathUnitsAttribute) on the given [‘clipPath’](#ClipPathElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

### <a id="InterfaceSVGMaskElement"></a>14.6.2 Interface SVGMaskElement

The [SVGMaskElement](#InterfaceSVGMaskElement) interface corresponds to the [‘mask’](#MaskElement) element.

```
interface SVGMaskElement : SVGElement,
                           SVGTests,
                           SVGLangSpace,
                           SVGExternalResourcesRequired,
                           SVGStylable,
                           SVGUnitTypes {
  readonly attribute SVGAnimatedEnumeration maskUnits;
  readonly attribute SVGAnimatedEnumeration maskContentUnits;
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
};
```

**Attributes:**
:

**<a id="__svg__SVGMaskElement__maskUnits"></a>**maskUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘maskUnits’](#MaskElementMaskUnitsAttribute) on the given [‘mask’](#MaskElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGMaskElement__maskContentUnits"></a>**maskContentUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘maskContentUnits’](#MaskElementMaskContentUnitsAttribute) on the given [‘mask’](#MaskElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGMaskElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#MaskElementXAttribute) on the given [‘mask’](#MaskElement) element.

**<a id="__svg__SVGMaskElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#MaskElementYAttribute) on the given [‘mask’](#MaskElement) element.

**<a id="__svg__SVGMaskElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#MaskElementWidthAttribute) on the given [‘mask’](#MaskElement) element.

**<a id="__svg__SVGMaskElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#MaskElementHeightAttribute) on the given [‘mask’](#MaskElement) element.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](pservers.md) ⋅ [Next](filters.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
