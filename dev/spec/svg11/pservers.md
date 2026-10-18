<!-- https://www.w3.org/TR/SVG11/pservers.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](color.md) ⋅ [Next](masking.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 13 Gradients and Patterns

## <a id="Introduction"></a>13.1 Introduction

With SVG, you can fill (i.e., paint the interior) or stroke (i.e., paint the outline) of shapes and text using one of the following:

- [color](color.md) (using <color>)
- [gradients](#Gradients) (linear or radial)
- [patterns](#Patterns) (vector or image, possibly tiled)

SVG uses the general notion of a **paint server**. Gradients and patterns are just specific types of built-in paint servers.

Paint servers are referenced using an [IRI reference](linking.md#IRIReference) on a [‘fill’](painting.md#FillProperty) or [‘stroke’](painting.md#StrokeProperty) property.

## <a id="Gradients"></a>13.2 Gradients

### <a id="GradientsIntroduction"></a>13.2.1 Introduction

Gradients consist of continuously smooth color transitions along a vector from one color to another, possibly followed by additional transitions along the same vector to other colors. SVG provides for two types of gradients: [**linear gradients**](#LinearGradients) and [**radial gradients**](#RadialGradients).

Once defined, gradients are then referenced using [‘fill’](painting.md#FillProperty) or [‘stroke’](painting.md#StrokeProperty) properties on a given [graphics element](intro.md#TermGraphicsElement) to indicate that the given element shall be filled or stroked with the referenced gradient.

The angle of the color transitions along the gradient vector is defined by the gradient normal. Before any transforms are applied to the gradient or its referencing [graphics element](intro.md#TermGraphicsElement), the gradient normal is perpendicular with the gradient vector. If a [graphics element](intro.md#TermGraphicsElement) references a gradient, conceptually the [graphics element](intro.md#TermGraphicsElement) should take a copy of the gradient vector and gradient normal and treat it as part of its own geometry. Any transformations applied to the [graphics element](intro.md#TermGraphicsElement) geometry also apply to the copied gradient vector and gradient normal. Any gradient transforms that are specified on the reference gradient are applied before any [graphics element](intro.md#TermGraphicsElement) transformations are applied to the gradient.

[image: linear and radial gradient vectors and normals]

### <a id="LinearGradients"></a>13.2.2 Linear gradients

<a id="LinearGradientElement"></a>Linear gradients are defined by a [‘linearGradient’](#LinearGradientElement) element.

‘linearGradient’

**Categories:**
: [Gradient element](intro.md#TermGradientElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [‘animate’](animate.md#AnimateElement)
- [‘animateTransform’](animate.md#AnimateTransformElement)
- [‘set’](animate.md#SetElement)
- [‘stop’](#StopElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](#StopColorProperty), [‘stop-opacity’](#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#LinearGradientElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘x1’](#LinearGradientElementX1Attribute)
- [‘y1’](#LinearGradientElementY1Attribute)
- [‘x2’](#LinearGradientElementX2Attribute)
- [‘y2’](#LinearGradientElementY2Attribute)
- [‘gradientUnits’](#LinearGradientElementGradientUnitsAttribute)
- [‘gradientTransform’](#LinearGradientElementGradientTransformAttribute)
- [‘spreadMethod’](#LinearGradientElementSpreadMethodAttribute)
- [‘xlink:href’](#LinearGradientElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGLinearGradientElement](#InterfaceSVGLinearGradientElement)

*Attribute definitions:*

**<a id="LinearGradientElementGradientUnitsAttribute"></a>gradientUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for attributes [‘x1’](#LinearGradientElementX1Attribute), [‘y1’](#LinearGradientElementY1Attribute), [‘x2’](#LinearGradientElementX2Attribute) and [‘y2’](#LinearGradientElementY2Attribute). If gradientUnits="userSpaceOnUse", [‘x1’](#LinearGradientElementX1Attribute), [‘y1’](#LinearGradientElementY1Attribute), [‘x2’](#LinearGradientElementX2Attribute) and [‘y2’](#LinearGradientElementY2Attribute) represent values in the coordinate system that results from taking the current user coordinate system in place at the time when the gradient element is referenced (i.e., the user coordinate system for the element referencing the gradient element via a [‘fill’](painting.md#FillProperty) or [‘stroke’](painting.md#StrokeProperty) property) and then applying the transform specified by attribute [‘gradientTransform’](#LinearGradientElementGradientTransformAttribute). If gradientUnits="objectBoundingBox", the user coordinate system for attributes [‘x1’](#LinearGradientElementX1Attribute), [‘y1’](#LinearGradientElementY1Attribute), [‘x2’](#LinearGradientElementX2Attribute) and [‘y2’](#LinearGradientElementY2Attribute) is established using the bounding box of the element to which the gradient is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute [‘gradientTransform’](#LinearGradientElementGradientTransformAttribute). When gradientUnits="objectBoundingBox" and [‘gradientTransform’](#LinearGradientElementGradientTransformAttribute) is the identity matrix, the normal of the linear gradient is perpendicular to the gradient vector in object bounding box space (i.e., the abstract coordinate system where (0,0) is at the top/left of the object bounding box and (1,1) is at the bottom/right of the object bounding box). When the object's bounding box is not square, the gradient normal which is initially perpendicular to the gradient vector within object bounding box space may render non-perpendicular relative to the gradient vector in user space. If the gradient vector is parallel to one of the axes of the bounding box, the gradient normal will remain perpendicular. This transformation is due to application of the non-uniform scaling transformation from bounding box space to user space. If attribute [‘gradientUnits’](#LinearGradientElementGradientUnitsAttribute) is not specified, then the effect is as if a value of 'objectBoundingBox' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementGradientTransformAttribute"></a>gradientTransform = "[<transform-list>](types.md#DataTypeTransformList)"**
: Contains the definition of an optional additional transformation from the gradient coordinate system onto the target coordinate system (i.e., userSpaceOnUse or objectBoundingBox). This allows for things such as skewing the gradient. This additional transformation matrix is post-multiplied to (i.e., inserted to the right of) any previously defined transformations, including the implicit transformation necessary to convert from [object bounding box units](coords.md#ObjectBoundingBox) to user space. If attribute [‘gradientTransform’](#LinearGradientElementGradientTransformAttribute) is not specified, then the effect is as if an identity transform were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementX1Attribute"></a>x1 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: [‘x1’](#LinearGradientElementX1Attribute), [‘y1’](#LinearGradientElementY1Attribute), [‘x2’](#LinearGradientElementX2Attribute) and [‘y2’](#LinearGradientElementY2Attribute) define a *gradient vector* for the linear gradient. This *gradient vector* provides starting and ending points onto which the [gradient stops](#GradientStops) are mapped. The values of [‘x1’](#LinearGradientElementX1Attribute), [‘y1’](#LinearGradientElementY1Attribute), [‘x2’](#LinearGradientElementX2Attribute) and [‘y2’](#LinearGradientElementY2Attribute) can be either numbers or percentages. If the attribute is not specified, the effect is as if a value of '0%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementY1Attribute"></a>y1 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: See [‘x1’](#LinearGradientElementX1Attribute). If the attribute is not specified, the effect is as if a value of '0%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementX2Attribute"></a>x2 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: See [‘x1’](#LinearGradientElementX1Attribute). If the attribute is not specified, the effect is as if a value of '100%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementY2Attribute"></a>y2 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: See [‘x1’](#LinearGradientElementX1Attribute). If the attribute is not specified, the effect is as if a value of '0%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementSpreadMethodAttribute"></a>spreadMethod = "pad | reflect | repeat"**
: Indicates what happens if the gradient starts or ends inside the bounds of the *target rectangle*. Possible values are: 'pad', which says to use the terminal colors of the gradient to fill the remainder of the target region, 'reflect', which says to reflect the gradient pattern start-to-end, end-to-start, start-to-end, etc. continuously until the *target rectangle* is filled, and repeat, which says to repeat the gradient pattern start-to-end, start-to-end, start-to-end, etc. continuously until the target region is filled. If the attribute is not specified, the effect is as if a value of 'pad' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LinearGradientElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) to a different [‘linearGradient’](#LinearGradientElement) or [‘radialGradient’](#RadialGradientElement) element within the current SVG document fragment. Any [‘linearGradient’](#LinearGradientElement) attributes which are defined on the referenced element which are not defined on this element are inherited by this element. If this element has no defined gradient stops, and the referenced element does (possibly due to its own [‘xlink:href’](#LinearGradientElementHrefAttribute) attribute), then this element inherits the gradient stop from the referenced element. Inheritance can be indirect to an arbitrary level; thus, if the referenced element inherits attribute or gradient stops due to its own [‘xlink:href’](#LinearGradientElementHrefAttribute) attribute, then the current element can inherit those attributes or gradient stops. [Animatable](animate.md#Animatable): yes.
Percentages are allowed for [‘x1’](#LinearGradientElementX1Attribute), [‘y1’](#LinearGradientElementY1Attribute), [‘x2’](#LinearGradientElementX2Attribute) and [‘y2’](#LinearGradientElementY2Attribute). For gradientUnits="userSpaceOnUse", percentages represent values relative to the current viewport. For gradientUnits="objectBoundingBox", percentages represent values relative to the bounding box for the object.

If [‘x1’](#LinearGradientElementX1Attribute) = [‘x2’](#LinearGradientElementX2Attribute) and [‘y1’](#LinearGradientElementY1Attribute) = [‘y2’](#LinearGradientElementY2Attribute), then the area to be painted will be painted as a single color using the color and opacity of the last [gradient stop](#GradientStops).

[Properties](styling.md#SVGStylingProperties) inherit into the [‘linearGradient’](#LinearGradientElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘linearGradient’](#LinearGradientElement) element.

[‘linearGradient’](#LinearGradientElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties. The [‘display’](painting.md#DisplayProperty) property does not apply to the [‘linearGradient’](#LinearGradientElement) element; thus, [‘linearGradient’](#LinearGradientElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘linearGradient’](#LinearGradientElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘linearGradient’](#LinearGradientElement) element or any of its ancestors is set to none.

<a id="ExampleLinGrad01"></a>Example lingrad01 shows how to fill a rectangle by referencing a linear gradient paint server.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="4cm" viewBox="0 0 800 400" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Example lingrad01 - fill a rectangle using a 
           linear gradient paint server</desc>
  <g>
    <defs>
      <linearGradient id="MyGradient">
        <stop offset="5%" stop-color="#F60" />
        <stop offset="95%" stop-color="#FF6" />
      </linearGradient>
    </defs>

    <!-- Outline the drawing area in blue -->
    <rect fill="none" stroke="blue" 
          x="1" y="1" width="798" height="398"/>

    <!-- The rectangle is filled using a linear gradient paint server -->
    <rect fill="url(#MyGradient)" stroke="black" stroke-width="5"  
          x="100" y="100" width="600" height="200"/>
  </g>
</svg>
```

Example lingrad01

| [image: Example lingrad01 — fill a rectangle using a linear gradient paint server] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/pservers/lingrad01.svg)

### <a id="RadialGradients"></a>13.2.3 Radial gradients

<a id="RadialGradientElement"></a>Radial gradients are defined by a [‘radialGradient’](#RadialGradientElement) element.

‘radialGradient’

**Categories:**
: [Gradient element](intro.md#TermGradientElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [‘animate’](animate.md#AnimateElement)
- [‘animateTransform’](animate.md#AnimateTransformElement)
- [‘set’](animate.md#SetElement)
- [‘stop’](#StopElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](#StopColorProperty), [‘stop-opacity’](#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#RadialGradientElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘cx’](#RadialGradientElementCXAttribute)
- [‘cy’](#RadialGradientElementCYAttribute)
- [‘r’](#RadialGradientElementRAttribute)
- [‘fx’](#RadialGradientElementFXAttribute)
- [‘fy’](#RadialGradientElementFYAttribute)
- [‘gradientUnits’](#RadialGradientElementGradientUnitsAttribute)
- [‘gradientTransform’](#RadialGradientElementGradientTransformAttribute)
- [‘spreadMethod’](#RadialGradientElementSpreadMethodAttribute)
- [‘xlink:href’](#RadialGradientElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGRadialGradientElement](#InterfaceSVGRadialGradientElement)

*Attribute definitions:*

**<a id="RadialGradientElementGradientUnitsAttribute"></a>gradientUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for attributes [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute), [‘r’](#RadialGradientElementRAttribute), [‘fx’](#RadialGradientElementFXAttribute) and [‘fy’](#RadialGradientElementFYAttribute). If gradientUnits="userSpaceOnUse", [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute), [‘r’](#RadialGradientElementRAttribute), [‘fx’](#RadialGradientElementFXAttribute) and [‘fy’](#RadialGradientElementFYAttribute) represent values in the coordinate system that results from taking the current user coordinate system in place at the time when the gradient element is referenced (i.e., the user coordinate system for the element referencing the gradient element via a [‘fill’](painting.md#FillProperty) or [‘stroke’](painting.md#StrokeProperty) property) and then applying the transform specified by attribute [‘gradientTransform’](#RadialGradientElementGradientTransformAttribute). If gradientUnits="objectBoundingBox", the user coordinate system for attributes [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute), [‘r’](#RadialGradientElementRAttribute), [‘fx’](#RadialGradientElementFXAttribute) and [‘fy’](#RadialGradientElementFYAttribute) is established using the bounding box of the element to which the gradient is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute [‘gradientTransform’](#RadialGradientElementGradientTransformAttribute). When gradientUnits="objectBoundingBox" and [‘gradientTransform’](#RadialGradientElementGradientTransformAttribute) is the identity matrix, then the rings of the radial gradient are circular with respect to the object bounding box space (i.e., the abstract coordinate system where (0,0) is at the top/left of the object bounding box and (1,1) is at the bottom/right of the object bounding box). When the object's bounding box is not square, the rings that are conceptually circular within object bounding box space will render as elliptical due to application of the non-uniform scaling transformation from bounding box space to user space. If attribute [‘gradientUnits’](#RadialGradientElementGradientUnitsAttribute) is not specified, then the effect is as if a value of 'objectBoundingBox' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementGradientTransformAttribute"></a>gradientTransform = "[<transform-list>](types.md#DataTypeTransformList)"**
: Contains the definitions of an optional additional transformation from the gradient coordinate system onto the target coordinate system (i.e., userSpaceOnUse or objectBoundingBox). This allows for things such as skewing the gradient. This additional transformation matrix is post-multiplied to (i.e., inserted to the right of) any previously defined transformations, including the implicit transformation necessary to convert from [object bounding box units](coords.md#ObjectBoundingBox) to user space. If attribute [‘gradientTransform’](#RadialGradientElementGradientTransformAttribute) is not specified, then the effect is as if an identity transform were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementCXAttribute"></a>cx = "[<coordinate>](types.md#DataTypeCoordinate)"**
: [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute) and [‘r’](#RadialGradientElementRAttribute) define the largest (i.e., outermost) circle for the radial gradient. The gradient will be drawn such that the 100% [gradient stop](#GradientStops) is mapped to the perimeter of this largest (i.e., outermost) circle. If the attribute is not specified, the effect is as if a value of '50%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementCYAttribute"></a>cy = "[<coordinate>](types.md#DataTypeCoordinate)"**
: See [‘cx’](#RadialGradientElementCXAttribute). If the attribute is not specified, the effect is as if a value of '50%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementRAttribute"></a>r = "[<length>](types.md#DataTypeLength)"**
: See [‘cx’](#RadialGradientElementCXAttribute). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero will cause the area to be painted as a single color using the color and opacity of the last [gradient stop](#GradientStops). If the attribute is not specified, the effect is as if a value of '50%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementFXAttribute"></a>fx = "[<coordinate>](types.md#DataTypeCoordinate)"**
: [‘fx’](#RadialGradientElementFXAttribute) and [‘fy’](#RadialGradientElementFYAttribute) define the focal point for the radial gradient. The gradient will be drawn such that the 0% [gradient stop](#GradientStops) is mapped to (fx, fy). If attribute [‘fx’](#RadialGradientElementFXAttribute) is not specified, [‘fx’](#RadialGradientElementFXAttribute) will coincide with the presentational value of [‘cx’](#RadialGradientElementCXAttribute) for the element whether the value for 'cx' was inherited or not. If the element references an element that specifies a value for 'fx', then the value of 'fx' is inherited from the referenced element. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementFYAttribute"></a>fy = "[<coordinate>](types.md#DataTypeCoordinate)"**
: See [‘fx’](#RadialGradientElementFXAttribute). If attribute [‘fy’](#RadialGradientElementFYAttribute) is not specified, [‘fy’](#RadialGradientElementFYAttribute) will coincide with the presentational value of [‘cy’](#RadialGradientElementCYAttribute) for the element whether the value for 'cy' was inherited or not. If the element references an element that specifies a value for 'fy', then the value of 'fy' is inherited from the referenced element. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementSpreadMethodAttribute"></a>spreadMethod = "pad | reflect | repeat"**
: Indicates what happens if the gradient starts or ends inside the bounds of the object(s) being painted by the gradient. Has the same values and meanings as the [‘spreadMethod’](#RadialGradientElementSpreadMethodAttribute) attribute on [‘linearGradient’](#LinearGradientElement) element. [Animatable](animate.md#Animatable): yes.
**<a id="RadialGradientElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) to a different [‘linearGradient’](#LinearGradientElement) or [‘radialGradient’](#RadialGradientElement) element within the current SVG document fragment. Any [‘radialGradient’](#RadialGradientElement) attributes which are defined on the referenced element which are not defined on this element are inherited by this element. If this element has no defined gradient stops, and the referenced element does (possibly due to its own [‘xlink:href’](#RadialGradientElementHrefAttribute) attribute), then this element inherits the gradient stop from the referenced element. Inheritance can be indirect to an arbitrary level; thus, if the referenced element inherits attribute or gradient stops due to its own [‘xlink:href’](#RadialGradientElementHrefAttribute) attribute, then the current element can inherit those attributes or gradient stops. [Animatable](animate.md#Animatable): yes.
Percentages are allowed for attributes [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute), [‘r’](#RadialGradientElementRAttribute), [‘fx’](#RadialGradientElementFXAttribute) and [‘fy’](#RadialGradientElementFYAttribute). For gradientUnits="userSpaceOnUse", percentages represent values relative to the current viewport. For gradientUnits="objectBoundingBox", percentages represent values relative to the bounding box for the object.

If the point defined by [‘fx’](#RadialGradientElementFXAttribute) and [‘fy’](#RadialGradientElementFYAttribute) lies outside the circle defined by [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute) and [‘r’](#RadialGradientElementRAttribute), then the user agent shall set the focal point to the intersection of the line from ([‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute)) to ([‘fx’](#RadialGradientElementFXAttribute), [‘fy’](#RadialGradientElementFYAttribute)) with the circle defined by [‘cx’](#RadialGradientElementCXAttribute), [‘cy’](#RadialGradientElementCYAttribute) and [‘r’](#RadialGradientElementRAttribute).

[Properties](styling.md#SVGStylingProperties) inherit into the [‘radialGradient’](#RadialGradientElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘radialGradient’](#RadialGradientElement) element.

[‘radialGradient’](#RadialGradientElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties. The [‘display’](painting.md#DisplayProperty) property does not apply to the [‘radialGradient’](#RadialGradientElement) element; thus, [‘radialGradient’](#RadialGradientElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘radialGradient’](#RadialGradientElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘radialGradient’](#RadialGradientElement) element or any of its ancestors is set to none.

<a id="ExampleRadGrad01"></a>Example radgrad01 shows how to fill a rectangle by referencing a radial gradient paint server.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="4cm" viewBox="0 0 800 400" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Example radgrad01 - fill a rectangle by referencing a 
           radial gradient paint server</desc>
  <g>
    <defs>
      <radialGradient id="MyGradient" gradientUnits="userSpaceOnUse"
                      cx="400" cy="200" r="300" fx="400" fy="200">
        <stop offset="0%" stop-color="red" />
        <stop offset="50%" stop-color="blue" />
        <stop offset="100%" stop-color="red" />
      </radialGradient>
    </defs>

    <!-- Outline the drawing area in blue -->
    <rect fill="none" stroke="blue" 
          x="1" y="1" width="798" height="398"/>

    <!-- The rectangle is filled using a radial gradient paint server -->
    <rect fill="url(#MyGradient)" stroke="black" stroke-width="5"  
          x="100" y="100" width="600" height="200"/>
  </g>
</svg>
```

Example radgrad01

| [image: Example radgrad01 — fill a rectangle by referencing a radial gradient paint server] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/pservers/radgrad01.svg)

### <a id="GradientStops"></a>13.2.4 Gradient stops

<a id="StopElement"></a>The ramp of colors to use on a gradient is defined by the [‘stop’](#StopElement) elements that are child elements to either the [‘linearGradient’](#LinearGradientElement) element or the [‘radialGradient’](#RadialGradientElement) element.

‘stop’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘animate’](animate.md#AnimateElement)
- [‘animateColor’](animate.md#AnimateColorElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](#StopColorProperty), [‘stop-opacity’](#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘offset’](#StopElementOffsetAttribute)

**DOM Interfaces:**
:

- [SVGStopElement](#InterfaceSVGStopElement)

*Attribute definitions:*

**<a id="StopElementOffsetAttribute"></a>offset = "*[<number>](types.md#DataTypeNumber) | [<percentage>](types.md#DataTypePercentage)*"**
: The [‘offset’](#StopElementOffsetAttribute) attribute is either a [<number>](types.md#DataTypeNumber) (usually ranging from 0 to 1) or a [<percentage>](types.md#DataTypePercentage) (usually ranging from 0% to 100%) which indicates where the gradient stop is placed. For linear gradients, the [‘offset’](#StopElementOffsetAttribute) attribute represents a location along the *gradient vector*. For radial gradients, it represents a percentage distance from (fx,fy) to the edge of the outermost/largest circle. [Animatable](animate.md#Animatable): yes.
The [‘stop-color’](#StopColorProperty) property indicates what color to use at that gradient stop. The keyword currentColor and ICC colors can be specified in the same manner as within a [<paint>](painting.md#SpecifyingPaint) specification for the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties.

**<a id="StopColorProperty"></a>‘stop-color’**
:

| *Value:* | currentColor \| [<color>](types.md#DataTypeColor) [<icccolor>](types.md#DataTypeICCColor) \| inherit |
| --- | --- |
| *Initial:* | black |
| *Applies to:* | [‘stop’](#StopElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘stop-opacity’](#StopOpacityProperty) property defines the opacity of a given gradient stop.

**<a id="StopOpacityProperty"></a>‘stop-opacity’**
:

| *Value:* | <opacity-value> \| inherit |
| --- | --- |
| *Initial:* | 1 |
| *Applies to:* | [‘stop’](#StopElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

Some notes on gradients:

- Gradient offset values less than 0 (or less than 0%) are rounded up to 0%. Gradient offset values greater than 1 (or greater than 100%) are rounded down to 100%.
- It is necessary that at least two stops defined to have a gradient effect. If no stops are defined, then painting shall occur as if 'none' were specified as the paint style. If one stop is defined, then paint with the solid color fill using the color defined for that gradient stop.
- Each gradient offset value is required to be equal to or greater than the previous gradient stop's offset value. If a given gradient stop's offset value is not equal to or greater than all previous offset values, then the offset value is adjusted to be equal to the largest of all previous offset values.
- If two gradient stops have the same offset value, then the latter gradient stop controls the color value at the overlap point. In particular:
```
<stop offset="0" stop-color="white"/>
<stop offset=".2" stop-color="red"/>
<stop offset=".2" stop-color="blue"/>
<stop offset="1" stop-color="black"/>
```

will have approximately the same effect as:
```
<stop offset="0" stop-color="white"/>
<stop offset=".1999999999" stop-color="red"/>
<stop offset=".2" stop-color="blue"/>
<stop offset="1" stop-color="black"/>
```

which is a gradient that goes smoothly from white to red, then abruptly shifts from red to blue, and then goes smoothly from blue to black.

## <a id="Patterns"></a>13.3 Patterns

<a id="PatternElement"></a>A pattern is used to fill or stroke an object using a pre-defined graphic object which can be replicated ("tiled") at fixed intervals in *x* and *y* to cover the areas to be painted. Patterns are defined using a [‘pattern’](#PatternElement) element and then referenced by properties [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) on a given [graphics element](intro.md#TermGraphicsElement) to indicate that the given element shall be filled or stroked with the referenced pattern.

Attributes [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute), [‘height’](#PatternElementHeightAttribute) and [‘patternUnits’](#PatternElementPatternUnitsAttribute) define a reference rectangle somewhere on the infinite canvas. The reference rectangle has its top/left at (*x*, *y*) and its bottom/right at (*x* + *width*, *y* + *height*). The tiling theoretically extends a series of such rectangles to infinity in X and Y (positive and negative), with rectangles starting at (*x* + m**width*, *y* + n* *height*) for each possible integer value for *m* and *n*.

‘pattern’

**Categories:**
: [Container element](intro.md#TermContainerElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](struct.md#DefsElement), [‘g’](struct.md#GElement), [‘svg’](struct.md#SVGElement), [‘symbol’](struct.md#SymbolElement), [‘use’](struct.md#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](#LinearGradientElement), [‘radialGradient’](#RadialGradientElement)
- [‘a’](linking.md#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](struct.md#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](struct.md#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](#StopColorProperty), [‘stop-opacity’](#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#PatternElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘viewBox’](coords.md#ViewBoxAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘x’](#PatternElementXAttribute)
- [‘y’](#PatternElementYAttribute)
- [‘width’](#PatternElementWidthAttribute)
- [‘height’](#PatternElementHeightAttribute)
- [‘patternUnits’](#PatternElementPatternUnitsAttribute)
- [‘patternContentUnits’](#PatternElementPatternContentUnitsAttribute)
- [‘patternTransform’](#PatternElementPatternTransformAttribute)
- [‘xlink:href’](#PatternElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGPatternElement](#InterfaceSVGPatternElement)

*Attribute definitions:*

**<a id="PatternElementPatternUnitsAttribute"></a>patternUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for attributes [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute) and [‘height’](#PatternElementHeightAttribute). If patternUnits="userSpaceOnUse", [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute) and [‘height’](#PatternElementHeightAttribute) represent values in the coordinate system that results from taking the current user coordinate system in place at the time when the [‘pattern’](#PatternElement) element is referenced (i.e., the user coordinate system for the element referencing the [‘pattern’](#PatternElement) element via a [‘fill’](painting.md#FillProperty) or [‘stroke’](painting.md#StrokeProperty) property) and then applying the transform specified by attribute [‘patternTransform’](#PatternElementPatternTransformAttribute). If patternUnits="objectBoundingBox", the user coordinate system for attributes [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute) and [‘height’](#PatternElementHeightAttribute) is established using the bounding box of the element to which the pattern is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute [‘patternTransform’](#PatternElementPatternTransformAttribute). If attribute [‘patternUnits’](#PatternElementPatternUnitsAttribute) is not specified, then the effect is as if a value of 'objectBoundingBox' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementPatternContentUnitsAttribute"></a>patternContentUnits = "*userSpaceOnUse* | *objectBoundingBox*"**
: Defines the coordinate system for the contents of the [‘pattern’](#PatternElement). Note that this attribute has no effect if attribute [‘viewBox’](coords.md#ViewBoxAttribute) is specified. If patternContentUnits="userSpaceOnUse", the user coordinate system for the contents of the [‘pattern’](#PatternElement) element is the coordinate system that results from taking the current user coordinate system in place at the time when the [‘pattern’](#PatternElement) element is referenced (i.e., the user coordinate system for the element referencing the [‘pattern’](#PatternElement) element via a [‘fill’](painting.md#FillProperty) or [‘stroke’](painting.md#StrokeProperty) property) and then applying the transform specified by attribute [‘patternTransform’](#PatternElementPatternTransformAttribute). If patternContentUnits="objectBoundingBox", the user coordinate system for the contents of the [‘pattern’](#PatternElement) element is established using the bounding box of the element to which the pattern is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute [‘patternTransform’](#PatternElementPatternTransformAttribute). If attribute [‘patternContentUnits’](#PatternElementPatternContentUnitsAttribute) is not specified, then the effect is as if a value of 'userSpaceOnUse' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementPatternTransformAttribute"></a>patternTransform = "[<transform-list>](types.md#DataTypeTransformList)"**
: Contains the definition of an optional additional transformation from the pattern coordinate system onto the target coordinate system (i.e., 'userSpaceOnUse' or 'objectBoundingBox'). This allows for things such as skewing the pattern tiles. This additional transformation matrix is post-multiplied to (i.e., inserted to the right of) any previously defined transformations, including the implicit transformation necessary to convert from [object bounding box units](coords.md#ObjectBoundingBox) to user space. If attribute [‘patternTransform’](#PatternElementPatternTransformAttribute) is not specified, then the effect is as if an identity transform were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute) and [‘height’](#PatternElementHeightAttribute) indicate how the pattern tiles are placed and spaced. These attributes represent coordinates and values in the coordinate space specified by the combination of attributes [‘patternUnits’](#PatternElementPatternUnitsAttribute) and [‘patternTransform’](#PatternElementPatternTransformAttribute). If the attribute is not specified, the effect is as if a value of zero were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: See [‘x’](#PatternElementXAttribute). If the attribute is not specified, the effect is as if a value of zero were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: See [‘x’](#PatternElementXAttribute). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element (i.e., no paint is applied). If the attribute is not specified, the effect is as if a value of zero were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: See [‘x’](#PatternElementXAttribute). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element (i.e., no paint is applied). If the attribute is not specified, the effect is as if a value of zero were specified. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) to a different [‘pattern’](#PatternElement) element within the current SVG document fragment. Any attributes which are defined on the referenced element which are not defined on this element are inherited by this element. If this element has no children, and the referenced element does (possibly due to its own [‘xlink:href’](#PatternElementHrefAttribute) attribute), then this element inherits the children from the referenced element. Inheritance can be indirect to an arbitrary level; thus, if the referenced element inherits attributes or children due to its own [‘xlink:href’](#PatternElementHrefAttribute) attribute, then the current element can inherit those attributes or children. [Animatable](animate.md#Animatable): yes.
**<a id="PatternElementPreserveAspectRatioAttribute"></a> preserveAspectRatio = "[[defer] <align> [<meetOrSlice>]](coords.md#PreserveAspectRatioAttribute)" **
:

See [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute).

If the attribute is not specified, then the effect is as if a value of xMidYMid meet were specified.

[Animatable](animate.md#Animatable): yes.

SVG's [user agent style sheet](styling.md#UAStyleSheet) sets the [‘overflow’](masking.md#OverflowProperty) property for [‘pattern’](#PatternElement) elements to hidden, which causes a rectangular clipping path to be created at the bounds of the pattern tile. Unless the [‘overflow’](masking.md#OverflowProperty) property is overridden, any graphics within the pattern which goes outside of the pattern rectangle will be clipped. Note that if the [‘overflow’](masking.md#OverflowProperty) property is set to visible the rendering behavior for the pattern is undefined. [Example pattern01](#ExamplePattern01) below shows the effect of clipping to the pattern tile.

The contents of the [‘pattern’](#PatternElement) are relative to a new coordinate system. If there is a [‘viewBox’](coords.md#ViewBoxAttribute) attribute, then the new coordinate system is fitted into the region defined by the [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute), [‘height’](#PatternElementHeightAttribute) and [‘patternUnits’](#PatternElementPatternUnitsAttribute) attributes on the [‘pattern’](#PatternElement) element using the standard rules for [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute). If there is no [‘viewBox’](coords.md#ViewBoxAttribute) attribute, then the new coordinate system has its origin at (*x*, *y*), where *x* is established by the [‘x’](#PatternElementXAttribute) attribute on the [‘pattern’](#PatternElement) element, and *y* is established by the [‘y’](#PatternElementYAttribute) attribute on the [‘pattern’](#PatternElement) element. Thus, in the following example:

```
<pattern x="10" y="10" width="20" height="20">
   <rect x="5" y="5" width="10" height="10"/>
</pattern>
```

the rectangle has its top/left located 5 units to the right and 5 units down from the origin of the pattern tile.

The [‘viewBox’](coords.md#ViewBoxAttribute) attribute introduces a supplemental transformation which is applied on top of any transformations necessary to create a new pattern coordinate system due to attributes [‘x’](#PatternElementXAttribute), [‘y’](#PatternElementYAttribute), [‘width’](#PatternElementWidthAttribute), [‘height’](#PatternElementHeightAttribute) and [‘patternUnits’](#PatternElementPatternUnitsAttribute).

[Properties](styling.md#SVGStylingProperties) inherit into the [‘pattern’](#PatternElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘pattern’](#PatternElement) element.

[‘pattern’](#PatternElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties. The [‘display’](painting.md#DisplayProperty) property does not apply to the [‘pattern’](#PatternElement) element; thus, [‘pattern’](#PatternElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘pattern’](#PatternElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘pattern’](#PatternElement) element or any of its ancestors is set to none.

[Event attributes](https://www.w3.org/TR/SVG11/script.html#EventAttributes) and [event listeners](https://www.w3.org/TR/SVG11/svgdom.html#EventListeners) attached to the contents of a [‘pattern’](#PatternElement) element are not processed; only the rendering aspects of [‘pattern’](#PatternElement) elements are processed.

<a id="ExamplePattern01"></a>Example pattern01 shows how to fill a rectangle by referencing a pattern paint server. Note how the blue stroke of each triangle has been clipped at the top and the left. This is due to SVG's [user agent style sheet](styling.md#UAStyleSheet) setting the [‘overflow’](masking.md#OverflowProperty) property for [‘pattern’](#PatternElement) elements to hidden, which causes the pattern to be clipped to the bounds of the pattern tile.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="4cm" viewBox="0 0 800 400" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <pattern id="TrianglePattern" patternUnits="userSpaceOnUse"
             x="0" y="0" width="100" height="100"
             viewBox="0 0 10 10" >
      <path d="M 0 0 L 7 0 L 3.5 7 z" fill="red" stroke="blue" />
    </pattern> 
  </defs>

  <!-- Outline the drawing area in blue -->
  <rect fill="none" stroke="blue" 
        x="1" y="1" width="798" height="398"/>

  <!-- The ellipse is filled using a triangle pattern paint server
       and stroked with black -->
  <ellipse fill="url(#TrianglePattern)" stroke="black" stroke-width="5"  
           cx="400" cy="200" rx="350" ry="150" />
</svg>
```

Example pattern01

| [image: Example pattern01 — fill a rectangle by referencing a pattern paint server] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/pservers/pattern01.svg)

## <a id="DOMInterfaces"></a>13.4 DOM interfaces

### <a id="InterfaceSVGGradientElement"></a>13.4.1 Interface SVGGradientElement

The [SVGGradientElement](#InterfaceSVGGradientElement) interface is a base interface used by [SVGLinearGradientElement](#InterfaceSVGLinearGradientElement) and [SVGRadialGradientElement](#InterfaceSVGRadialGradientElement).

```
interface SVGGradientElement : SVGElement,
                               SVGURIReference,
                               SVGExternalResourcesRequired,
                               SVGStylable,
                               SVGUnitTypes {

  // Spread Method Types
  const unsigned short SVG_SPREADMETHOD_UNKNOWN = 0;
  const unsigned short SVG_SPREADMETHOD_PAD = 1;
  const unsigned short SVG_SPREADMETHOD_REFLECT = 2;
  const unsigned short SVG_SPREADMETHOD_REPEAT = 3;

  readonly attribute SVGAnimatedEnumeration gradientUnits;
  readonly attribute SVGAnimatedTransformList gradientTransform;
  readonly attribute SVGAnimatedEnumeration spreadMethod;
};
```

**Constants in group “Spread Method Types”:**
:

**<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_UNKNOWN"></a>**SVG_SPREADMETHOD_UNKNOWN** (unsigned short)**
:

The type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_PAD"></a>**SVG_SPREADMETHOD_PAD** (unsigned short)**
:

Corresponds to value 'pad'.

**<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_REFLECT"></a>**SVG_SPREADMETHOD_REFLECT** (unsigned short)**
:

Corresponds to value 'reflect'.

**<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_REPEAT"></a>**SVG_SPREADMETHOD_REPEAT** (unsigned short)**
:

Corresponds to value 'repeat'.

**Attributes:**
:

**<a id="__svg__SVGGradientElement__gradientUnits"></a>**gradientUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute ‘gradientUnits’ on the given element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGGradientElement__gradientTransform"></a>**gradientTransform** (readonly [SVGAnimatedTransformList](coords.md#InterfaceSVGAnimatedTransformList))**
:

Corresponds to attribute ‘gradientTransform’ on the given element.

**<a id="__svg__SVGGradientElement__spreadMethod"></a>**spreadMethod** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute ‘spreadMethod’ on the given element. One of the Spread Method Types defined on this interface.

### <a id="InterfaceSVGLinearGradientElement"></a>13.4.2 Interface SVGLinearGradientElement

The [SVGLinearGradientElement](#InterfaceSVGLinearGradientElement) interface corresponds to the [‘linearGradient’](#LinearGradientElement) element.

```
interface SVGLinearGradientElement : SVGGradientElement {
  readonly attribute SVGAnimatedLength x1;
  readonly attribute SVGAnimatedLength y1;
  readonly attribute SVGAnimatedLength x2;
  readonly attribute SVGAnimatedLength y2;
};
```

**Attributes:**
:

**<a id="__svg__SVGLinearGradientElement__x1"></a>**x1** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x1’](#LinearGradientElementX1Attribute) on the given [‘linearGradient’](#LinearGradientElement) element.

**<a id="__svg__SVGLinearGradientElement__y1"></a>**y1** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y1’](#LinearGradientElementY1Attribute) on the given [‘linearGradient’](#LinearGradientElement) element.

**<a id="__svg__SVGLinearGradientElement__x2"></a>**x2** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x2’](#LinearGradientElementX2Attribute) on the given [‘linearGradient’](#LinearGradientElement) element.

**<a id="__svg__SVGLinearGradientElement__y2"></a>**y2** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y2’](#LinearGradientElementY2Attribute) on the given [‘linearGradient’](#LinearGradientElement) element.

### <a id="InterfaceSVGRadialGradientElement"></a>13.4.3 Interface SVGRadialGradientElement

The [SVGRadialGradientElement](#InterfaceSVGRadialGradientElement) interface corresponds to the [‘radialGradient’](#RadialGradientElement) element.

```
interface SVGRadialGradientElement : SVGGradientElement {
  readonly attribute SVGAnimatedLength cx;
  readonly attribute SVGAnimatedLength cy;
  readonly attribute SVGAnimatedLength r;
  readonly attribute SVGAnimatedLength fx;
  readonly attribute SVGAnimatedLength fy;
};
```

**Attributes:**
:

**<a id="__svg__SVGRadialGradientElement__cx"></a>**cx** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘cx’](#RadialGradientElementCXAttribute) on the given [‘radialGradient’](#RadialGradientElement) element.

**<a id="__svg__SVGRadialGradientElement__cy"></a>**cy** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘cy’](#RadialGradientElementCYAttribute) on the given [‘radialGradient’](#RadialGradientElement) element.

**<a id="__svg__SVGRadialGradientElement__r"></a>**r** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘r’](#RadialGradientElementRAttribute) on the given [‘radialGradient’](#RadialGradientElement) element.

**<a id="__svg__SVGRadialGradientElement__fx"></a>**fx** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘fx’](#RadialGradientElementFXAttribute) on the given [‘radialGradient’](#RadialGradientElement) element.

**<a id="__svg__SVGRadialGradientElement__fy"></a>**fy** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘fy’](#RadialGradientElementFYAttribute) on the given [‘radialGradient’](#RadialGradientElement) element.

### <a id="InterfaceSVGStopElement"></a>13.4.4 Interface SVGStopElement

The [SVGStopElement](#InterfaceSVGStopElement) interface corresponds to the [‘stop’](#StopElement) element.

```
interface SVGStopElement : SVGElement,
                           SVGStylable {
  readonly attribute SVGAnimatedNumber offset;
};
```

**Attributes:**
:

**<a id="__svg__SVGStopElement__offset"></a>**offset** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [‘offset’](#StopElementOffsetAttribute) on the given [‘stop’](#StopElement) element.

### <a id="InterfaceSVGPatternElement"></a>13.4.5 Interface SVGPatternElement

The [SVGPatternElement](#InterfaceSVGPatternElement) interface corresponds to the [‘pattern’](#PatternElement) element.

```
interface SVGPatternElement : SVGElement,
                              SVGURIReference,
                              SVGTests,
                              SVGLangSpace,
                              SVGExternalResourcesRequired,
                              SVGStylable,
                              SVGFitToViewBox,
                              SVGUnitTypes {
  readonly attribute SVGAnimatedEnumeration patternUnits;
  readonly attribute SVGAnimatedEnumeration patternContentUnits;
  readonly attribute SVGAnimatedTransformList patternTransform;
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
};
```

**Attributes:**
:

**<a id="__svg__SVGPatternElement__patternUnits"></a>**patternUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘patternUnits’](#PatternElementPatternUnitsAttribute) on the given [‘pattern’](#PatternElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGPatternElement__patternContentUnits"></a>**patternContentUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘patternContentUnits’](#PatternElementPatternContentUnitsAttribute) on the given [‘pattern’](#PatternElement) element. Takes one of the constants defined in [SVGUnitTypes](types.md#InterfaceSVGUnitTypes).

**<a id="__svg__SVGPatternElement__patternTransform"></a>**patternTransform** (readonly [SVGAnimatedTransformList](coords.md#InterfaceSVGAnimatedTransformList))**
:

Corresponds to attribute [‘patternTransform’](#PatternElementPatternTransformAttribute) on the given [‘pattern’](#PatternElement) element.

**<a id="__svg__SVGPatternElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#PatternElementXAttribute) on the given [‘pattern’](#PatternElement) element.

**<a id="__svg__SVGPatternElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#PatternElementYAttribute) on the given [‘pattern’](#PatternElement) element.

**<a id="__svg__SVGPatternElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#PatternElementWidthAttribute) on the given [‘pattern’](#PatternElement) element.

**<a id="__svg__SVGPatternElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#PatternElementHeightAttribute) on the given [‘pattern’](#PatternElement) element.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](color.md) ⋅ [Next](masking.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
