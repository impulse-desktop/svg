<!-- https://www.w3.org/TR/SVG11/shapes.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](paths.md) ⋅ [Next](text.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 9 Basic Shapes

## <a id="Introduction"></a>9.1 Introduction

SVG contains the following set of basic shape elements:

- rectangles (including optional rounded corners), created with the [‘rect’](#RectElement) element,
- circles, created with the [‘circle’](#CircleElement) element,
- ellipses, created with the [‘ellipse’](#EllipseElement) element,
- straight lines, created with the [‘line’](#LineElement) element,
- polylines, created with the [‘polyline’](#PolylineElement) element, and
- polygons, created with the [‘polygon’](#PolygonElement) element.

Mathematically, these shape elements are equivalent to a [‘path’](paths.md#PathElement) element that would construct the same shape. The basic shapes may be stroked, filled and used as clip paths. All of the properties available for [‘path’](paths.md#PathElement) elements also apply to the basic shapes.

## <a id="RectElement"></a>9.2 The ‘rect’ element

The [‘rect’](#RectElement) element defines a rectangle which is axis-aligned with the current [user coordinate system](intro.md#TermUserCoordinateSystem). Rounded rectangles can be achieved by setting appropriate values for attributes [‘rx’](#RectElementRXAttribute) and [‘ry’](#RectElementRYAttribute).

‘rect’

**Categories:**
: [Basic shape element](intro.md#TermBasicShapeElement), [graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘x’](#RectElementXAttribute)
- [‘y’](#RectElementYAttribute)
- [‘width’](#RectElementWidthAttribute)
- [‘height’](#RectElementHeightAttribute)
- [‘rx’](#RectElementRXAttribute)
- [‘ry’](#RectElementRYAttribute)

**DOM Interfaces:**
:

- [SVGRectElement](#InterfaceSVGRectElement)

*Attribute definitions:*

**<a id="RectElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of the side of the rectangle which has the smaller x-axis coordinate value in the current user coordinate system. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RectElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of the side of the rectangle which has the smaller y-axis coordinate value in the current user coordinate system. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="RectElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: The width of the rectangle. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
**<a id="RectElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: The height of the rectangle. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
**<a id="RectElementRXAttribute"></a>rx = "[<length>](types.md#DataTypeLength)"**
: For rounded rectangles, the x-axis radius of the ellipse used to round off the corners of the rectangle. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). See the notes below about what happens if the attribute is not specified. [Animatable](animate.md#Animatable): yes.
**<a id="RectElementRYAttribute"></a>ry = "[<length>](types.md#DataTypeLength)"**
: For rounded rectangles, the y-axis radius of the ellipse used to round off the corners of the rectangle. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). See the notes below about what happens if the attribute is not specified. [Animatable](animate.md#Animatable): yes.
The values used for the x- and y-axis rounded corner radii are determined implicitly if the [‘rx’](#RectElementRXAttribute) or [‘ry’](#RectElementRYAttribute) attributes (or both) are not specified, or are specified but with invalid values. The values are also subject to clamping so that the lengths of the straight segments of the rectangle are never negative. The effective values for [‘rx’](#RectElementRXAttribute) and [‘ry’](#RectElementRYAttribute) are determined by following these steps in order:

1. Let *rx* and *ry* be length values.
2. If neither [‘rx’](#RectElementRXAttribute) nor [‘ry’](#RectElementRYAttribute) are properly specified, then set both *rx* and *ry* to 0. (This will result in square corners.)
3. Otherwise, if a properly specified value is provided for [‘rx’](#RectElementRXAttribute), but not for [‘ry’](#RectElementRYAttribute), then set both *rx* and *ry* to the value of [‘rx’](#RectElementRXAttribute).
4. Otherwise, if a properly specified value is provided for [‘ry’](#RectElementRYAttribute), but not for [‘rx’](#RectElementRXAttribute), then set both *rx* and *ry* to the value of [‘ry’](#RectElementRYAttribute).
5. Otherwise, both [‘rx’](#RectElementRXAttribute) and [‘ry’](#RectElementRYAttribute) were specified properly. Set *rx* to the value of [‘rx’](#RectElementRXAttribute) and *ry* to the value of [‘ry’](#RectElementRYAttribute).
6. If *rx* is greater than half of [‘width’](#RectElementWidthAttribute), then set *rx* to half of [‘width’](#RectElementWidthAttribute).
7. If *ry* is greater than half of [‘height’](#RectElementHeightAttribute), then set *ry* to half of [‘height’](#RectElementHeightAttribute).
8. The effective values of [‘rx’](#RectElementRXAttribute) and [‘ry’](#RectElementRYAttribute) are *rx* and *ry*, respectively.

Mathematically, a [‘rect’](#RectElement) element can be mapped to an equivalent [‘path’](paths.md#PathElement) element as follows: (Note: all coordinate and length values are first converted into user space coordinates according to [Units](coords.md#Units).)

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to location (*x+rx*,*y*), where *x* is the value of the [‘rect’](#RectElement) element's [‘x’](#RectElementXAttribute) attribute converted to user space, *rx* is the effective value of the [‘rx’](#RectElementRXAttribute) attribute converted to user space and *y* is the value of the [‘y’](#RectElementYAttribute) attribute converted to user space
- perform an absolute horizontal [lineto](paths.md#PathDataLinetoCommands) operation to location (*x+width-rx*,*y*), where *width* is the [‘rect’](#RectElement) element's [‘width’](#RectElementWidthAttribute) attribute converted to user space
- perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x+width*,*y+ry*), where the effective values for the [‘rx’](#RectElementRXAttribute) and [‘ry’](#RectElementRYAttribute) attributes on the [‘rect’](#RectElement) element converted to user space are used as the *rx* and *ry* attributes on the [elliptical arc](paths.md#PathDataEllipticalArcCommands) command, respectively, the *x-axis-rotation* is set to zero, the *large-arc-flag* is set to zero, and the *sweep-flag* is set to one
- perform a absolute vertical *lineto* to location (*x+width*,*y+height-ry*), where *height* is the [‘rect’](#RectElement) element's [‘height’](#RectElementHeightAttribute) attribute converted to user space
- perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x+width-rx*,*y+height*)
- perform an absolute horizontal *lineto* to location (*x+rx*,*y+height*)
- perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x*,*y+height-ry*)
- perform an absolute absolute vertical *lineto* to location (*x*,*y+ry*)
- perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x+rx*,*y*)

<a id="ExampleRect01"></a>Example rect01 shows a rectangle with sharp corners. The [‘rect’](#RectElement) element is filled with yellow and stroked with navy.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example rect01 - rectangle with sharp corners</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2"/>

  <rect x="400" y="100" width="400" height="200"
        fill="yellow" stroke="navy" stroke-width="10"  />
</svg>
```

Example rect01

| [image: Example rect01 — rectangle with sharp corners] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/rect01.svg)

<a id="ExampleRect02"></a>Example rect02 shows two rounded rectangles. The [‘rx’](#RectElementRXAttribute) specifies how to round the corners of the rectangles. Note that since no value has been specified for the [‘ry’](#RectElementRYAttribute) attribute, it will be assigned the same value as the [‘rx’](#RectElementRXAttribute) attribute.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example rect02 - rounded rectangles</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2"/>

  <rect x="100" y="100" width="400" height="200" rx="50"
        fill="green" />

  <g transform="translate(700 210) rotate(-30)">
    <rect x="0" y="0" width="400" height="200" rx="50"
          fill="none" stroke="purple" stroke-width="30" />
  </g>
</svg>
```

Example rect02

| [image: Example rect02 — rounded rectangles expressed in user coordinates] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/rect02.svg)

## <a id="CircleElement"></a>9.3 The ‘circle’ element

The [‘circle’](#CircleElement) element defines a circle based on a center point and a radius.

‘circle’

**Categories:**
: [Basic shape element](intro.md#TermBasicShapeElement), [graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘cx’](#CircleElementCXAttribute)
- [‘cy’](#CircleElementCYAttribute)
- [‘r’](#CircleElementRAttribute)

**DOM Interfaces:**
:

- [SVGCircleElement](#InterfaceSVGCircleElement)

*Attribute definitions:*

**<a id="CircleElementCXAttribute"></a>cx = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of the center of the circle. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="CircleElementCYAttribute"></a>cy = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of the center of the circle. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="CircleElementRAttribute"></a>r = "[<length>](types.md#DataTypeLength)"**
: The radius of the circle. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
The arc of a [‘circle’](#CircleElement) element begins at the "3 o'clock" point on the radius and progresses towards the "9 o'clock" point. The starting point and direction of the arc are affected by the user space transform in the same manner as the geometry of the element.

<a id="ExampleCircle01"></a>Example circle01 consists of a [‘circle’](#CircleElement) element that is filled with red and stroked with blue.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example circle01 - circle filled with red and stroked with blue</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2"/>

  <circle cx="600" cy="200" r="100"
        fill="red" stroke="blue" stroke-width="10"  />
</svg>
```

Example circle01

| [image: Example circle01 — circle filled with red and stroked with blue] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/circle01.svg)

## <a id="EllipseElement"></a>9.4 The ‘ellipse’ element

The [‘ellipse’](#EllipseElement) element defines an ellipse which is axis-aligned with the current [user coordinate system](intro.md#TermUserCoordinateSystem) based on a center point and two radii.

‘ellipse’

**Categories:**
: [Basic shape element](intro.md#TermBasicShapeElement), [graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘cx’](#EllipseElementCXAttribute)
- [‘cy’](#EllipseElementCYAttribute)
- [‘rx’](#EllipseElementRXAttribute)
- [‘ry’](#EllipseElementRYAttribute)

**DOM Interfaces:**
:

- [SVGEllipseElement](#InterfaceSVGEllipseElement)

*Attribute definitions:*

**<a id="EllipseElementCXAttribute"></a>cx = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of the center of the ellipse. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="EllipseElementCYAttribute"></a>cy = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of the center of the ellipse. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="EllipseElementRXAttribute"></a>rx = "[<length>](types.md#DataTypeLength)"**
: The x-axis radius of the ellipse. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
**<a id="EllipseElementRYAttribute"></a>ry = "[<length>](types.md#DataTypeLength)"**
: The y-axis radius of the ellipse. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
<a id="ExampleEllipse01"></a>The arc of an [‘ellipse’](#EllipseElement) element begins at the "3 o'clock" point on the radius and progresses towards the "9 o'clock" point. The starting point and direction of the arc are affected by the user space transform in the same manner as the geometry of the element.

Example ellipse01 below specifies the coordinates of the two ellipses in the user coordinate system established by the [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [‘svg’](struct.md#SVGElement) element and the [‘transform’](coords.md#TransformAttribute) attribute on the [‘g’](struct.md#GElement) and [‘ellipse’](#EllipseElement) elements. Both ellipses use the default values of zero for the [‘cx’](#EllipseElementCXAttribute) and [‘cy’](#EllipseElementCYAttribute) attributes (the center of the ellipse). The second ellipse is rotated.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example ellipse01 - examples of ellipses</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2" />

  <g transform="translate(300 200)">
    <ellipse rx="250" ry="100"
          fill="red"  />
  </g>

  <ellipse transform="translate(900 200) rotate(-30)" 
        rx="250" ry="100"
        fill="none" stroke="blue" stroke-width="20"  />

</svg>
```

Example ellipse01

| [image: Example ellipse01 — ellipses expressed in user coordinates] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/ellipse01.svg)

## <a id="LineElement"></a>9.5 The ‘line’ element

The [‘line’](#LineElement) element defines a line segment that starts at one point and ends at another.

‘line’

**Categories:**
: [Basic shape element](intro.md#TermBasicShapeElement), [graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘x1’](#LineElementX1Attribute)
- [‘y1’](#LineElementY1Attribute)
- [‘x2’](#LineElementX2Attribute)
- [‘y2’](#LineElementY2Attribute)

**DOM Interfaces:**
:

- [SVGLineElement](#InterfaceSVGLineElement)

*Attribute definitions:*

**<a id="LineElementX1Attribute"></a>x1 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of the start of the line. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LineElementY1Attribute"></a>y1 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of the start of the line. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LineElementX2Attribute"></a>x2 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of the end of the line. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="LineElementY2Attribute"></a>y2 = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of the end of the line. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
Mathematically, a [‘line’](#LineElement) element can be mapped to an equivalent [‘path’](paths.md#PathElement) element as follows: (Note: all coordinate and length values are first converted into user space coordinates according to [Units](coords.md#Units).)

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to absolute location (*x1*,*y1*), where *x1* and *y1* are the values of the [‘line’](#LineElement) element's [‘x1’](#LineElementX1Attribute) and [‘y1’](#LineElementY1Attribute) attributes converted to user space, respectively
- perform an absolute [lineto](paths.md#PathDataLinetoCommands) operation to absolute location (*x2*,*y2*), where *x2* and *y2* are the values of the [‘line’](#LineElement) element's [‘x2’](#LineElementX2Attribute) and [‘y2’](#LineElementY2Attribute) attributes converted to user space, respectively

Because [‘line’](#LineElement) elements are single lines and thus are geometrically one-dimensional, they have no interior; thus, [‘line’](#LineElement) elements are never filled (see the [‘fill’](painting.md#FillProperty) property).

<a id="ExampleLine01"></a>Example line01 below specifies the coordinates of the five lines in the user coordinate system established by the [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [‘svg’](struct.md#SVGElement) element. The lines have different thicknesses.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example line01 - lines expressed in user coordinates</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2" />

  <g stroke="green" >
    <line x1="100" y1="300" x2="300" y2="100"
            stroke-width="5"  />
    <line x1="300" y1="300" x2="500" y2="100"
            stroke-width="10"  />
    <line x1="500" y1="300" x2="700" y2="100"
            stroke-width="15"  />
    <line x1="700" y1="300" x2="900" y2="100"
            stroke-width="20"  />
    <line x1="900" y1="300" x2="1100" y2="100"
            stroke-width="25"  />
  </g>
</svg>
```

Example line01

| [image: Example line01 — lines expressed in user coordinates] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/line01.svg)

## <a id="PolylineElement"></a>9.6 The ‘polyline’ element

The [‘polyline’](#PolylineElement) element defines a set of connected straight line segments. Typically, [‘polyline’](#PolylineElement) elements define open shapes.

‘polyline’

**Categories:**
: [Basic shape element](intro.md#TermBasicShapeElement), [graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘points’](#PolylineElementPointsAttribute)

**DOM Interfaces:**
:

- [SVGPolylineElement](#InterfaceSVGPolylineElement)

*Attribute definitions:*

**<a id="PolylineElementPointsAttribute"></a>points = "[<list-of-points>](#PointsBNF)"**
: The points that make up the polyline. All coordinate values are in the user coordinate system. [Animatable](animate.md#Animatable): yes.
If an odd number of coordinates is provided, then the element is in error, with the same user agent behavior as occurs with an incorrectly specified [‘path’](paths.md#PathElement) element.

Mathematically, a [‘polyline’](#PolylineElement) element can be mapped to an equivalent [‘path’](paths.md#PathElement) element as follows:

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to the first coordinate pair in the list of points
- for each subsequent coordinate pair, perform an absolute [lineto](paths.md#PathDataLinetoCommands) operation to that coordinate pair.

<a id="ExamplePolyline01"></a>Example polyline01 below specifies a polyline in the user coordinate system established by the [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [‘svg’](struct.md#SVGElement) element.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example polyline01 - increasingly larger bars</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2" />

  <polyline fill="none" stroke="blue" stroke-width="10" 
            points="50,375
                    150,375 150,325 250,325 250,375
                    350,375 350,250 450,250 450,375
                    550,375 550,175 650,175 650,375
                    750,375 750,100 850,100 850,375
                    950,375 950,25 1050,25 1050,375
                    1150,375" />
</svg>
```

Example polyline01

| [image: Example polyline01 — increasingly larger bars] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/polyline01.svg)

## <a id="PolygonElement"></a>9.7 The ‘polygon’ element

The [‘polygon’](#PolygonElement) element defines a closed shape consisting of a set of connected straight line segments.

‘polygon’

**Categories:**
: [Basic shape element](intro.md#TermBasicShapeElement), [graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘points’](#PolygonElementPointsAttribute)

**DOM Interfaces:**
:

- [SVGPolygonElement](#InterfaceSVGPolygonElement)

*Attribute definitions:*

**<a id="PolygonElementPointsAttribute"></a>points = "[<list-of-points>](#PointsBNF)"**
: The points that make up the polygon. All coordinate values are in the user coordinate system. [Animatable](animate.md#Animatable): yes.
If an odd number of coordinates is provided, then the element is in error, with the same user agent behavior as occurs with an incorrectly specified [‘path’](paths.md#PathElement) element.

Mathematically, a [‘polygon’](#PolygonElement) element can be mapped to an equivalent [‘path’](paths.md#PathElement) element as follows:

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to the first coordinate pair in the list of points
- for each subsequent coordinate pair, perform an absolute [lineto](paths.md#PathDataLinetoCommands) operation to that coordinate pair
- perform a [closepath](paths.md#PathDataClosePathCommand) command

<a id="ExamplePolygon01"></a>Example polygon01 below specifies two polygons (a star and a hexagon) in the user coordinate system established by the [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [‘svg’](struct.md#SVGElement) element.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example polygon01 - star and hexagon</desc>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2" />

  <polygon fill="red" stroke="blue" stroke-width="10" 
            points="350,75  379,161 469,161 397,215
                    423,301 350,250 277,301 303,215
                    231,161 321,161" />
  <polygon fill="lime" stroke="blue" stroke-width="10" 
            points="850,75  958,137.5 958,262.5
                    850,325 742,262.6 742,137.5" />
</svg>
```

Example polygon01

| [image: Example polygon01 — star and hexagon] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/shapes/polygon01.svg)

### <a id="PointsBNF"></a>9.7.1 The grammar for points specifications in ‘polyline’ and ‘polygon’ elements

The following is the Extended Backus-Naur Form (EBNF) for points specifications in [‘polyline’](#PolylineElement) and [‘polygon’](#PolygonElement) elements. The following notation is used:

- *: 0 or more
- +: 1 or more
- ?: 0 or 1
- (): grouping
- |: separates alternatives
- double quotes surround literals

```
list-of-points:
    wsp* coordinate-pairs? wsp*
coordinate-pairs:
    coordinate-pair
    | coordinate-pair comma-wsp coordinate-pairs
coordinate-pair:
    coordinate comma-wsp coordinate
    | coordinate negative-coordinate
coordinate:
    number
number:
    sign? integer-constant
    | sign? floating-point-constant
negative-coordinate:
    "-" integer-constant
    | "-" floating-point-constant
comma-wsp:
    (wsp+ comma? wsp*) | (comma wsp*)
comma:
    ","
integer-constant:
    digit-sequence
floating-point-constant:
    fractional-constant exponent?
    | digit-sequence exponent
fractional-constant:
    digit-sequence? "." digit-sequence
    | digit-sequence "."
exponent:
    ( "e" | "E" ) sign? digit-sequence
sign:
    "+" | "-"
digit-sequence:
    digit
    | digit digit-sequence
digit:
    "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
wsp:
    (#x20 | #x9 | #xD | #xA)+
```

## <a id="DOMInterfaces"></a>9.8 DOM interfaces

### <a id="InterfaceSVGRectElement"></a>9.8.1 Interface SVGRectElement

The [SVGRectElement](#InterfaceSVGRectElement) interface corresponds to the [‘rect’](#RectElement) element.

```
interface SVGRectElement : SVGElement,
                           SVGTests,
                           SVGLangSpace,
                           SVGExternalResourcesRequired,
                           SVGStylable,
                           SVGTransformable {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
  readonly attribute SVGAnimatedLength rx;
  readonly attribute SVGAnimatedLength ry;
};
```

**Attributes:**
:

**<a id="__svg__SVGRectElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#RectElementXAttribute) on the given [‘rect’](#RectElement) element.

**<a id="__svg__SVGRectElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#RectElementYAttribute) on the given [‘rect’](#RectElement) element.

**<a id="__svg__SVGRectElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#RectElementWidthAttribute) on the given [‘rect’](#RectElement) element.

**<a id="__svg__SVGRectElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#RectElementHeightAttribute) on the given [‘rect’](#RectElement) element.

**<a id="__svg__SVGRectElement__rx"></a>**rx** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘rx’](#RectElementRXAttribute) on the given [‘rect’](#RectElement) element.

**<a id="__svg__SVGRectElement__ry"></a>**ry** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘ry’](#RectElementRYAttribute) on the given [‘rect’](#RectElement) element.

### <a id="InterfaceSVGCircleElement"></a>9.8.2 Interface SVGCircleElement

The [SVGCircleElement](#InterfaceSVGCircleElement) interface corresponds to the [‘circle’](#CircleElement) element.

```
interface SVGCircleElement : SVGElement,
                             SVGTests,
                             SVGLangSpace,
                             SVGExternalResourcesRequired,
                             SVGStylable,
                             SVGTransformable {
  readonly attribute SVGAnimatedLength cx;
  readonly attribute SVGAnimatedLength cy;
  readonly attribute SVGAnimatedLength r;
};
```

**Attributes:**
:

**<a id="__svg__SVGCircleElement__cx"></a>**cx** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘cx’](#CircleElementCXAttribute) on the given [‘circle’](#CircleElement) element.

**<a id="__svg__SVGCircleElement__cy"></a>**cy** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘cy’](#CircleElementCYAttribute) on the given [‘circle’](#CircleElement) element.

**<a id="__svg__SVGCircleElement__r"></a>**r** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘r’](#CircleElementRAttribute) on the given [‘circle’](#CircleElement) element.

### <a id="InterfaceSVGEllipseElement"></a>9.8.3 Interface SVGEllipseElement

The [SVGEllipseElement](#InterfaceSVGEllipseElement) interface corresponds to the [‘ellipse’](#EllipseElement) element.

```
interface SVGEllipseElement : SVGElement,
                              SVGTests,
                              SVGLangSpace,
                              SVGExternalResourcesRequired,
                              SVGStylable,
                              SVGTransformable {
  readonly attribute SVGAnimatedLength cx;
  readonly attribute SVGAnimatedLength cy;
  readonly attribute SVGAnimatedLength rx;
  readonly attribute SVGAnimatedLength ry;
};
```

**Attributes:**
:

**<a id="__svg__SVGEllipseElement__cx"></a>**cx** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘cx’](#EllipseElementCXAttribute) on the given [‘ellipse’](#EllipseElement) element.

**<a id="__svg__SVGEllipseElement__cy"></a>**cy** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘cy’](#EllipseElementCYAttribute) on the given [‘ellipse’](#EllipseElement) element.

**<a id="__svg__SVGEllipseElement__rx"></a>**rx** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘rx’](#EllipseElementRXAttribute) on the given [‘ellipse’](#EllipseElement) element.

**<a id="__svg__SVGEllipseElement__ry"></a>**ry** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘ry’](#EllipseElementRYAttribute) on the given [‘ellipse’](#EllipseElement) element.

### <a id="InterfaceSVGLineElement"></a>9.8.4 Interface SVGLineElement

The [SVGLineElement](#InterfaceSVGLineElement) interface corresponds to the [‘line’](#LineElement) element.

```
interface SVGLineElement : SVGElement,
                           SVGTests,
                           SVGLangSpace,
                           SVGExternalResourcesRequired,
                           SVGStylable,
                           SVGTransformable {
  readonly attribute SVGAnimatedLength x1;
  readonly attribute SVGAnimatedLength y1;
  readonly attribute SVGAnimatedLength x2;
  readonly attribute SVGAnimatedLength y2;
};
```

**Attributes:**
:

**<a id="__svg__SVGLineElement__x1"></a>**x1** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x1’](#LineElementX1Attribute) on the given [‘line’](#LineElement) element.

**<a id="__svg__SVGLineElement__y1"></a>**y1** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y1’](#LineElementY1Attribute) on the given [‘line’](#LineElement) element.

**<a id="__svg__SVGLineElement__x2"></a>**x2** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x2’](#LineElementX2Attribute) on the given [‘line’](#LineElement) element.

**<a id="__svg__SVGLineElement__y2"></a>**y2** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y2’](#LineElementY2Attribute) on the given [‘line’](#LineElement) element.

### <a id="InterfaceSVGAnimatedPoints"></a>9.8.5 Interface SVGAnimatedPoints

The [SVGAnimatedPoints](#InterfaceSVGAnimatedPoints) interface supports elements which have a ‘points’ attribute which holds a list of coordinate values and which support the ability to animate that attribute.

Additionally, the ‘points’ attribute on the original element accessed via the XML DOM (e.g., using the `getAttribute()` method call) will reflect any changes made to [points](#__svg__SVGAnimatedPoints__points).

```
interface SVGAnimatedPoints {
  readonly attribute SVGPointList points;
  readonly attribute SVGPointList animatedPoints;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedPoints__points"></a>**points** (readonly [SVGPointList](coords.md#InterfaceSVGPointList))**
:

Provides access to the base (i.e., static) contents of the ‘points’ attribute.

**<a id="__svg__SVGAnimatedPoints__animatedPoints"></a>**animatedPoints** (readonly [SVGPointList](coords.md#InterfaceSVGPointList))**
:

Provides access to the current animated contents of the ‘points’ attribute. If the given attribute or property is being animated, contains the current animated value of the attribute or property. If the given attribute or property is not currently being animated, contains the same value as [points](#__svg__SVGAnimatedPoints__points).

### <a id="InterfaceSVGPolylineElement"></a>9.8.6 Interface SVGPolylineElement

The [SVGPolylineElement](#InterfaceSVGPolylineElement) interface corresponds to the [‘polyline’](#PolylineElement) element.

```
interface SVGPolylineElement : SVGElement,
                               SVGTests,
                               SVGLangSpace,
                               SVGExternalResourcesRequired,
                               SVGStylable,
                               SVGTransformable,
                               SVGAnimatedPoints {
};
```

### <a id="InterfaceSVGPolygonElement"></a>9.8.7 Interface SVGPolygonElement

The [SVGPolygonElement](#InterfaceSVGPolygonElement) interface corresponds to the [‘polygon’](#PolygonElement) element.

```
interface SVGPolygonElement : SVGElement,
                              SVGTests,
                              SVGLangSpace,
                              SVGExternalResourcesRequired,
                              SVGStylable,
                              SVGTransformable,
                              SVGAnimatedPoints {
};
```

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](paths.md) ⋅ [Next](text.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
