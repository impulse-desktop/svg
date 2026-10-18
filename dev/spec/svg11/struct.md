<!-- https://www.w3.org/TR/SVG11/struct.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](types.md) ⋅ [Next](styling.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 5 Document Structure

## <a id="NewDocument"></a>5.1 Defining an SVG document fragment: the ‘svg’ element

### <a id="NewDocumentOverview"></a>5.1.1 Overview

An [SVG document fragment](intro.md#TermSVGDocumentFragment) consists of any number of SVG elements contained within an [‘svg’](#SVGElement) element.

An SVG document fragment can range from an empty fragment (i.e., no content inside of the [‘svg’](#SVGElement) element), to a very simple SVG document fragment containing a single SVG [graphics element](intro.md#TermGraphicsElement) such as a [‘rect’](shapes.md#RectElement), to a complex, deeply nested collection of [container elements](intro.md#TermContainerElement) and [graphics elements](intro.md#TermGraphicsElement).

An SVG document fragment can stand by itself as a self-contained file or resource, in which case the SVG document fragment is an [SVG document](intro.md#TermSVGDocumentFragment), or it can be embedded inline as a fragment within a parent XML document.

<a id="EmbeddedSVGExample"></a>The following example shows simple SVG content embedded inline as a fragment within a parent XML document. Note the use of XML namespaces to indicate that the [‘svg’](#SVGElement) and [‘ellipse’](shapes.md#EllipseElement) elements belong to the SVG namespace:

```
<?xml version="1.0" standalone="yes"?>
<parent xmlns="http://example.org"
        xmlns:svg="http://www.w3.org/2000/svg">
   <!-- parent contents here -->
   <svg:svg width="4cm" height="8cm" version="1.1">
      <svg:ellipse cx="2cm" cy="4cm" rx="2cm" ry="1cm" />
   </svg:svg>
   <!-- ... -->
</parent>
```

This example shows a slightly more complex (i.e., it contains multiple rectangles) stand-alone, self-contained SVG document:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="5cm" height="4cm" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Four separate rectangles
  </desc>
    <rect x="0.5cm" y="0.5cm" width="2cm" height="1cm"/>
    <rect x="0.5cm" y="2cm" width="1cm" height="1.5cm"/>
    <rect x="3cm" y="0.5cm" width="1.5cm" height="2cm"/>
    <rect x="3.5cm" y="3cm" width="1cm" height="0.5cm"/>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="4.98cm" height="3.98cm"
        fill="none" stroke="blue" stroke-width=".02cm" />

</svg>
```

[‘svg’](#SVGElement) elements can appear in the middle of SVG content. This is the mechanism by which SVG document fragments can be embedded within other SVG document fragments.

Another use for [‘svg’](#SVGElement) elements within the middle of SVG content is to establish a new viewport. (See [Establishing a new viewport](coords.md#EstablishingANewViewport).)

In all cases, for compliance with the Namespaces in XML Recommendation [[XML-NS](refs.md#ref-XML-NS)], an SVG namespace declaration must be provided so that all SVG elements are identified as belonging to the SVG namespace. The following are possible ways to provide a namespace declaration. An ‘xmlns’ attribute without a namespace prefix could be specified on an [‘svg’](#SVGElement) element, which means that SVG is the default namespace for all elements within the scope of the element with the ‘xmlns’ attribute:

```
<svg xmlns="http://www.w3.org/2000/svg" …>
  <rect …/>
</svg>
```

If a namespace prefix is specified on the ‘xmlns’ attribute (e.g., `xmlns:svg="http://www.w3.org/2000/svg"`), then the corresponding namespace is not the default namespace, so an explicit namespace prefix must be assigned to the elements:

```
<svg:svg xmlns:svg="http://www.w3.org/2000/svg" …>
  <svg:rect …/>
</svg:svg>
```

Namespace prefixes can be specified on ancestor elements (illustrated in the [above example](#EmbeddedSVGExample)). For more information, refer to the Namespaces in XML Recommendation [[XML-NS](refs.md#ref-XML-NS)].

### <a id="SVGElement"></a>5.1.2 The ‘svg’ element

‘svg’

**Categories:**
: [Container element](intro.md#TermContainerElement), [structural element](intro.md#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](#DefsElement), [‘g’](#GElement), [‘svg’](#SVGElement), [‘symbol’](#SymbolElement), [‘use’](#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement)
- [‘a’](linking.md#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute), [‘systemLanguage’](#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [document event attributes](intro.md#TermDocumentEventAttribute) — [‘onunload’](https://www.w3.org/TR/SVG11/script.html#OnUnloadEventAttribute), [‘onabort’](https://www.w3.org/TR/SVG11/script.html#OnAbortEventAttribute), [‘onerror’](https://www.w3.org/TR/SVG11/script.html#OnErrorEventAttribute), [‘onresize’](https://www.w3.org/TR/SVG11/script.html#OnResizeEventAttribute), [‘onscroll’](https://www.w3.org/TR/SVG11/script.html#OnScrollEventAttribute), [‘onzoom’](https://www.w3.org/TR/SVG11/script.html#OnZoomEventAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘x’](#SVGElementXAttribute)
- [‘y’](#SVGElementYAttribute)
- [‘width’](#SVGElementWidthAttribute)
- [‘height’](#SVGElementHeightAttribute)
- [‘viewBox’](coords.md#ViewBoxAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘zoomAndPan’](interact.md#ZoomAndPanAttribute)
- [‘version’](#SVGElementVersionAttribute)
- [‘baseProfile’](#SVGElementBaseProfileAttribute)
- [‘contentScriptType’](https://www.w3.org/TR/SVG11/script.html#ContentScriptTypeAttribute)
- [‘contentStyleType’](styling.md#ContentStyleTypeAttribute)
- [‘x’](#SVGElementXAttribute)
- [‘y’](#SVGElementYAttribute)
- [‘width’](#SVGElementWidthAttribute)
- [‘height’](#SVGElementHeightAttribute)
- [‘version’](#SVGElementVersionAttribute)
- [‘baseProfile’](#SVGElementBaseProfileAttribute)

**DOM Interfaces:**
:

- [SVGSVGElement](#InterfaceSVGSVGElement)

*Attribute definitions:*

**<a id="SVGElementVersionAttribute"></a>version = "[<number>](types.md#DataTypeNumber)"**
: Indicates the SVG language version to which this document fragment conforms. In SVG 1.0 [[SVG10](refs.md#ref-SVG10)], this attribute was fixed to the value '1.0'. For SVG 1.1, the attribute should have the value '1.1'. [Animatable](animate.md#Animatable): no.
**<a id="SVGElementBaseProfileAttribute"></a>baseProfile = profile-name**
: Describes the minimum SVG language profile that the author believes is necessary to correctly render the content. The attribute does not specify any processing restrictions; It can be considered metadata. For example, the value of the attribute could be used by an authoring tool to warn the user when they are modifying the document beyond the scope of the specified base profile. Each SVG profile should define the text that is appropriate for this attribute. If the attribute is not specified, the effect is as if a value of 'none' were specified. [Animatable](animate.md#Animatable): no.
**<a id="SVGElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: (Has no meaning or effect on [outermost svg elements](intro.md#TermOutermostSVGElement).) The x-axis coordinate of one corner of the rectangular region into which an embedded [‘svg’](#SVGElement) element is placed. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="SVGElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: (Has no meaning or effect on [outermost svg elements](intro.md#TermOutermostSVGElement).) The y-axis coordinate of one corner of the rectangular region into which an embedded [‘svg’](#SVGElement) element is placed. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="SVGElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: For [outermost svg elements](intro.md#TermOutermostSVGElement), the intrinsic width of the SVG document fragment. For embedded [‘svg’](#SVGElement) elements, the width of the rectangular region into which the [‘svg’](#SVGElement) element is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. If the attribute is not specified, the effect is as if a value of '100%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="SVGElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: For [outermost svg elements](intro.md#TermOutermostSVGElement), the intrinsic height of the SVG document fragment. For embedded [‘svg’](#SVGElement) elements, the height of the rectangular region into which the [‘svg’](#SVGElement) element is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. If the attribute is not specified, the effect is as if a value of '100%' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="SVGElementPreserveAspectRatioAttribute"></a> preserveAspectRatio = "[[defer] <align> [<meetOrSlice>]](coords.md#PreserveAspectRatioAttribute)" **
:

See [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute).

If the attribute is not specified, then the effect is as if a value of 'xMidYMid meet' were specified.

[Animatable](animate.md#Animatable): yes.

**<a id="SVGElementContentScriptTypeAttribute"></a> contentScriptType = "[*content-type*](https://www.w3.org/TR/SVG11/script.html#ContentScriptTypeAttribute)" **
:

See ['contentScriptType'](https://www.w3.org/TR/SVG11/script.html#ContentScriptTypeAttribute).

**<a id="SVGElementContentStyleTypeAttribute"></a> contentStyleType = "[*content-type*](styling.md#ContentStyleTypeAttribute)" **
:

See ['contentStyleType'](styling.md#ContentStyleTypeAttribute).

**<a id="SVGElementZoomAndPanAttribute"></a> zoomAndPan = "[disable | magnify](interact.md#ZoomAndPanAttribute)" **
:

See ['zoomAndPan'](interact.md#ZoomAndPanAttribute).

If an SVG document is likely to be referenced as a component of another document, the author will often want to include a [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [outermost svg element](intro.md#TermOutermostSVGElement) of the referenced document. This attribute provides a convenient way to design SVG documents to scale-to-fit into an arbitrary viewport.

## <a id="Groups"></a>5.2 Grouping: the ‘g’ element

### <a id="GroupsOverview"></a>5.2.1 Overview

The [‘g’](#GElement) element is a [container element](intro.md#TermGraphicsElement) for grouping together related [graphics elements](intro.md#TermGraphicsElement).

Grouping constructs, when used in conjunction with the [‘desc’](#DescElement) and [‘title’](#TitleElement) elements, provide information about document structure and semantics. Documents that are rich in structure may be rendered graphically, as speech, or as braille, and thus promote [accessibility](access.md).

A group of elements, as well as individual objects, can be given a name using the [‘id’](#IDAttribute) attribute. Named groups are needed for several purposes such as animation and re-usable objects.

An example:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1" width="5cm" height="5cm">
  <desc>Two groups, each of two rectangles</desc>
  <g id="group1" fill="red">
    <rect x="1cm" y="1cm" width="1cm" height="1cm"/>
    <rect x="3cm" y="1cm" width="1cm" height="1cm"/>
  </g>
  <g id="group2" fill="blue">
    <rect x="1cm" y="3cm" width="1cm" height="1cm"/>
    <rect x="3cm" y="3cm" width="1cm" height="1cm"/>
  </g>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="4.98cm" height="4.98cm"
        fill="none" stroke="blue" stroke-width=".02cm"/>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/grouping01.svg)

A [‘g’](#GElement) element can contain other [‘g’](#GElement) elements nested within it, to an arbitrary depth. Thus, the following is possible:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1" width="4in" height="3in">
  <desc>Groups can nest</desc>
  <g>
     <g>
       <g>
       </g>
     </g>
   </g>
</svg>
```

Any element that is not contained within a [‘g’](#GElement) is treated (at least conceptually) as if it were in its own group.

### <a id="GElement"></a>5.2.2 The ‘g’ element

‘g’

**Categories:**
: [Container element](intro.md#TermContainerElement), [structural element](intro.md#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](#DefsElement), [‘g’](#GElement), [‘svg’](#SVGElement), [‘symbol’](#SymbolElement), [‘use’](#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement)
- [‘a’](linking.md#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute), [‘systemLanguage’](#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)

**DOM Interfaces:**
:

- [SVGGElement](#InterfaceSVGGElement)

## <a id="Head"></a>5.3 Defining content for reuse, and the ‘defs’ element

### <a id="Overview"></a>5.3.1 Overview

SVG allows graphical objects to be defined for later reuse. To do this, it makes extensive use of [IRI references](intro.md#TermIRIReference) [[RFC3987](refs.md#ref-RFC3987)] to these other objects. For example, to fill a rectangle with a linear gradient, you first define a [‘linearGradient’](pservers.md#LinearGradientElement) element and give it an ID, as in:

```
<linearGradient id="MyGradient">...</linearGradient>
```

You then reference the linear gradient as the value of the [‘fill’](painting.md#FillProperty) property for the rectangle, as in:

```
<rect style="fill:url(#MyGradient)"/>
```

Some types of element, such as gradients, will not by themselves produce a graphical result. They can therefore be placed anywhere convenient. However, sometimes it is desired to define a graphical object and prevent it from being directly rendered. it is only there to be referenced elsewhere. To do this, and to allow convenient grouping defined content, SVG provides the ‘defs’ element.

It is recommended that, wherever possible, referenced elements be defined inside of a [‘defs’](#DefsElement) element. Among the elements that are always referenced: [‘altGlyphDef’](text.md#AltGlyphDefElement), [‘clipPath’](masking.md#ClipPathElement), [‘cursor’](interact.md#CursorElement), [‘filter’](filters.md#FilterElement), [‘linearGradient’](pservers.md#LinearGradientElement), [‘marker’](painting.md#MarkerElement), [‘mask’](masking.md#MaskElement), [‘pattern’](pservers.md#PatternElement), [‘radialGradient’](pservers.md#RadialGradientElement) and [‘symbol’](#SymbolElement). Defining these elements inside of a [‘defs’](#DefsElement) element promotes understandability of the SVG content and thus promotes accessibility.

### <a id="DefsElement"></a>5.3.2 The ‘defs’ element

‘defs’

**Categories:**
: [Container element](intro.md#TermContainerElement), [structural element](intro.md#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](#DefsElement), [‘g’](#GElement), [‘svg’](#SVGElement), [‘symbol’](#SymbolElement), [‘use’](#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement)
- [‘a’](linking.md#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute), [‘systemLanguage’](#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)

**DOM Interfaces:**
:

- [SVGDefsElement](#InterfaceSVGDefsElement)

The [‘defs’](#DefsElement) element is a container element for [referenced elements](#Head). For understandability and [accessibility](access.md) reasons, it is recommended that, whenever possible, referenced elements be defined inside of a [‘defs’](#DefsElement).

The content model for [‘defs’](#DefsElement) is the same as for the [‘g’](#GElement) element; thus, any element that can be a child of a [‘g’](#GElement) can also be a child of a [‘defs’](#DefsElement), and vice versa.

Elements that are descendants of a [‘defs’](#DefsElement) are not rendered directly; they are prevented from becoming part of the rendering tree just as if the [‘defs’](#DefsElement) element were a [‘g’](#GElement) element and the [‘display’](painting.md#DisplayProperty) property were set to none. Note, however, that the descendants of a [‘defs’](#DefsElement) are always present in the source tree and thus can always be referenced by other elements; thus, the value of the [‘display’](painting.md#DisplayProperty) property on the [‘defs’](#DefsElement) element or any of its descendants does not prevent those elements from being referenced by other elements.

To provide some SVG user agents with an opportunity to implement efficient implementations in streaming environments, creators of SVG content are encouraged to place all elements which are targets of local IRI references within a [‘defs’](#DefsElement) element which is a direct child of one of the ancestors of the referencing element. For example:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="8cm" height="3cm"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Local URI references within ancestor's 'defs' element.</desc>
  <defs>
    <linearGradient id="Gradient01">
      <stop offset="20%" stop-color="#39F" />
      <stop offset="90%" stop-color="#F3F" />
    </linearGradient>
  </defs>
  <rect x="1cm" y="1cm" width="6cm" height="1cm" 
        fill="url(#Gradient01)"  />

  <!-- Show outline of canvas using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="7.98cm" height="2.98cm"
        fill="none" stroke="blue" stroke-width=".02cm" />

</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/defs01.svg)

In the document above, the linear gradient is defined within a [‘defs’](#DefsElement) element which is the direct child of the [‘svg’](#SVGElement) element, which in turn is an ancestor of the [‘rect’](shapes.md#RectElement) element which references the linear gradient. Thus, the above document conforms to the guideline.

## <a id="DescriptionAndTitleElements"></a>5.4 The ‘desc’ and ‘title’ elements

<a id="DescElement"></a>

‘desc’

**Categories:**
: [Descriptive element](intro.md#TermDescriptiveElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)

**DOM Interfaces:**
:

- [SVGDescElement](#InterfaceSVGDescElement)

<a id="TitleElement"></a>

‘title’

**Categories:**
: [Descriptive element](intro.md#TermDescriptiveElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)

**DOM Interfaces:**
:

- [SVGTitleElement](#InterfaceSVGTitleElement)

Each [container element](intro.md#TermGraphicsElement) or [graphics element](intro.md#TermGraphicsElement) in an SVG drawing can supply a [‘desc’](#DescElement) and/or a [‘title’](#TitleElement) description string where the description is text-only. When the current SVG document fragment is rendered as SVG on visual media, [‘desc’](#DescElement) and [‘title’](#TitleElement) elements are not rendered as part of the graphics. User agents may, however, for example, display the [‘title’](#TitleElement) element as a tooltip, as the pointing device moves over particular elements. Alternate presentations are possible, both visual and aural, which display the [‘desc’](#DescElement) and [‘title’](#TitleElement) elements but do not display [‘path’](paths.md#PathElement) elements or other [graphics elements](intro.md#TermGraphicsElement). This is readily achieved by using a different (perhaps user) style sheet. For deep hierarchies, and for following [‘use’](#UseElement) element references, it is sometimes desirable to allow the user to control how deep they drill down into descriptive text.

In conforming SVG document fragments, any [‘title’](#TitleElement) element should be the first child element of its parent. Note that those implementations that do use [‘title’](#TitleElement) to display a tooltip often will only do so if the [‘title’](#TitleElement) is indeed the first child element of its parent.

The following is an example. In typical operation, the SVG user agent would not render the [‘desc’](#DescElement) and [‘title’](#TitleElement) elements but would render the remaining contents of the [‘g’](#GElement) element.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg SYSTEM "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1" width="4in" height="3in">
  <g>
    <title>Company sales by region</title>
    <desc>
      This is a bar chart which shows 
      company sales by region.
    </desc>
    <!-- Bar chart defined as vector data -->
  </g>
</svg>
```

Description and title elements can contain marked-up text from other namespaces. Here is an example:

```
<?xml version="1.0" standalone="yes"?>
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1" width="4in" height="3in">
  <desc xmlns:mydoc="http://example.org/mydoc">
    <mydoc:title>This is an example SVG file</mydoc:title>
    <mydoc:para>The global description uses markup from the 
      <mydoc:emph>mydoc</mydoc:emph> namespace.</mydoc:para>
  </desc>
  <g>
    <!-- the picture goes here -->
  </g>
</svg>
```

Authors should always provide a [‘title’](#TitleElement) child element to the [outermost svg element](intro.md#TermOutermostSVGElement) within a stand-alone SVG document. The [‘title’](#TitleElement) child element to an [‘svg’](#SVGElement) element serves the purposes of identifying the content of the given SVG document fragment. Since users often consult documents out of context, authors should provide context-rich titles. Thus, instead of a title such as "Introduction", which doesn't provide much contextual background, authors should supply a title such as "Introduction to Medieval Bee-Keeping" instead. For reasons of accessibility, user agents should always make the content of the [‘title’](#TitleElement) child element to the [outermost svg element](intro.md#TermOutermostSVGElement) available to users. The mechanism for doing so depends on the user agent (e.g., as a caption, spoken).

The DTD definitions of many of SVG's elements (particularly, container and text elements) place no restriction on the placement or number of the [‘desc’](#DescElement) and [‘title’](#TitleElement) sub-elements. This flexibility is only present so that there will be a consistent content model for container elements, because some container elements in SVG allow for mixed content, and because the mixed content rules for XML ([[XML10](refs.md#ref-XML10)], section 3.2.2) do not permit the desired restrictions. Representations of future versions of the SVG language might use more expressive representations than DTDs which allow for more restrictive mixed content rules. It is strongly recommended that at most one [‘desc’](#DescElement) and at most one [‘title’](#TitleElement) element appear as a child of any particular element, and that these elements appear before any other child elements (except possibly [‘metadata’](metadata.md#MetadataElement) elements) or character data content. If user agents need to choose among multiple [‘desc’](#DescElement) or [‘title’](#TitleElement) elements for processing (e.g., to decide which string to use for a tooltip), the user agent shall choose the first one.

## <a id="SymbolElement"></a>5.5 The ‘symbol’ element

‘symbol’

**Categories:**
: [Container element](intro.md#TermContainerElement), [structural element](intro.md#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [structural elements](intro.md#TermStructuralElement) — [‘defs’](#DefsElement), [‘g’](#GElement), [‘svg’](#SVGElement), [‘symbol’](#SymbolElement), [‘use’](#UseElement)
- [gradient elements](intro.md#TermGradientElement) — [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement)
- [‘a’](linking.md#AElement)
- [‘altGlyphDef’](text.md#AltGlyphDefElement)
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘viewBox’](coords.md#ViewBoxAttribute)

**DOM Interfaces:**
:

- [SVGSymbolElement](#InterfaceSVGSymbolElement)

The [‘symbol’](#SymbolElement) element is used to define graphical template objects which can be instantiated by a [‘use’](#UseElement) element.

The use of [‘symbol’](#SymbolElement) elements for graphics that are used multiple times in the same document adds structure and semantics. Documents that are rich in structure may be rendered graphically, as speech, or as braille, and thus promote [accessibility](access.md).

The key distinctions between a [‘symbol’](#SymbolElement) and a [‘g’](#GElement) are:

- A [‘symbol’](#SymbolElement) element itself is not rendered. Only instances of a [‘symbol’](#SymbolElement) element (i.e., a reference to a [‘symbol’](#SymbolElement) by a [‘use’](#UseElement) element) are rendered.
- A [‘symbol’](#SymbolElement) element has attributes [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) which allow a [‘symbol’](#SymbolElement) to scale-to-fit within a rectangular viewport defined by the referencing [‘use’](#UseElement) element.

Closely related to the [‘symbol’](#SymbolElement) element are the [‘marker’](painting.md#MarkerElement) and [‘pattern’](pservers.md#PatternElement) elements.

[‘symbol’](#SymbolElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘use’](#UseElement) element. The [‘display’](painting.md#DisplayProperty) property does not apply to the [‘symbol’](#SymbolElement) element; thus, [‘symbol’](#SymbolElement) elements are not directly rendered even if the [‘display’](painting.md#DisplayProperty) property is set to a value other than none, and [‘symbol’](#SymbolElement) elements are available for referencing even when the [‘display’](painting.md#DisplayProperty) property on the [‘symbol’](#SymbolElement) element or any of its ancestors is set to none.

## <a id="UseElement"></a>5.6 The ‘use’ element

‘use’

**Categories:**
: [Graphics element](intro.md#TermGraphicsElement), [graphics referencing element](intro.md#TermGraphicsReferencingElement), [structural element](intro.md#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute), [‘systemLanguage’](#SystemLanguageAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#UseElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘x’](#UseElementXAttribute)
- [‘y’](#UseElementYAttribute)
- [‘width’](#UseElementWidthAttribute)
- [‘height’](#UseElementHeightAttribute)
- [‘xlink:href’](#UseElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGUseElement](#InterfaceSVGUseElement)

Any [‘svg’](#SVGElement), [‘symbol’](#SymbolElement), [‘g’](#GElement), [graphics element](intro.md#TermGraphicsElement) or other [‘use’](#UseElement) is potentially a template object that can be re-used (i.e., "instanced") in the SVG document via a [‘use’](#UseElement) element. The [‘use’](#UseElement) element references another element and indicates that the graphical contents of that element is included/drawn at that given point in the document.

Unlike [‘image’](#ImageElement), the [‘use’](#UseElement) element cannot reference entire files.

The [‘use’](#UseElement) element has optional attributes [‘x’](#UseElementXAttribute), [‘y’](#UseElementYAttribute), [‘width’](#UseElementWidthAttribute) and [‘height’](#UseElementHeightAttribute) which are used to map the graphical contents of the referenced element onto a rectangular region within the current coordinate system.

The effect of a [‘use’](#UseElement) element is as if the contents of the referenced element were deeply cloned into a separate non-exposed DOM tree which had the [‘use’](#UseElement) element as its parent and all of the [‘use’](#UseElement) element's ancestors as its higher-level ancestors. Because the cloned DOM tree is non-exposed, the SVG Document Object Model (DOM) only contains the [‘use’](#UseElement) element and its attributes. The SVG DOM does not show the referenced element's contents as children of [‘use’](#UseElement) element.

For user agents that support [Styling with CSS](styling.md#StylingWithCSS), the conceptual deep cloning of the referenced element into a non-exposed DOM tree also copies any property values resulting from the CSS cascade ([[CSS2](refs.md#ref-CSS2)], chapter 6) on the referenced element and its contents. CSS2 selectors can be applied to the original (i.e., referenced) elements because they are part of the formal document structure. CSS2 selectors cannot be applied to the (conceptually) cloned DOM tree because its contents are not part of the formal document structure.

Property inheritance, however, works as if the referenced element had been textually included as a deeply cloned child of the [‘use’](#UseElement) element. The referenced element inherits properties from the [‘use’](#UseElement) element and the [‘use’](#UseElement) element's ancestors. An instance of a referenced element does not inherit properties from the referenced element's original parents.

If event attributes are assigned to referenced elements, then the actual target for the event will be the [SVGElementInstance](#InterfaceSVGElementInstance) object within the "instance tree" corresponding to the given referenced element.

The event handling for the non-exposed tree works as if the referenced element had been textually included as a deeply cloned child of the [‘use’](#UseElement) element, except that events are dispatched to the [SVGElementInstance](#InterfaceSVGElementInstance) objects. The event's target and currentTarget attributes are set to the [SVGElementInstance](#InterfaceSVGElementInstance) that corresponds to the target and current target elements in the referenced subtree. An event propagates through the exposed and non-exposed portions of the tree in the same manner as it would in the regular document tree: first going from the root element to the [‘use’](#UseElement) element and then through non-exposed tree elements in the capture phase, followed by the target phase at the target of the event, then bubbling back through non-exposed tree to the use element and then back through regular tree to the root element in bubbling phase.

An element and all its corresponding [SVGElementInstance](#InterfaceSVGElementInstance) objects share an event listener list. The currentTarget attribute of the event can be used to determine through which object an event listener was invoked.

The behavior of the [‘visibility’](painting.md#VisibilityProperty) property conforms to this model of property inheritance. Thus, specifying 'visibility:hidden' on a [‘use’](#UseElement) element does not guarantee that the referenced content will not be rendered. If the [‘use’](#UseElement) element specifies 'visibility:hidden' and the element it references specifies 'visibility:hidden' or 'visibility:inherit', then that one element will be hidden. However, if the referenced element instead specifies 'visibility:visible', then that element will be visible even if the [‘use’](#UseElement) element specifies 'visibility:hidden'.

Animations on a referenced element will cause the instances to also be animated.

A [‘use’](#UseElement) element has the same visual effect as if the [‘use’](#UseElement) element were replaced by the following generated content:

- **If the [‘use’](#UseElement) element references a [‘symbol’](#SymbolElement) element**: In the generated content, the [‘use’](#UseElement) will be replaced by [‘g’](#GElement), where all attributes from the [‘use’](#UseElement) element except for [‘x’](#UseElementXAttribute), [‘y’](#UseElementYAttribute), [‘width’](#UseElementWidthAttribute), [‘height’](#UseElementHeightAttribute) and [‘xlink:href’](#UseElementHrefAttribute) are transferred to the generated [‘g’](#GElement) element. An additional transformation translate(x,y) is appended to the end (i.e., right-side) of the [‘transform’](coords.md#TransformAttribute) attribute on the generated [‘g’](#GElement), where x and y represent the values of the [‘x’](#UseElementXAttribute) and [‘y’](#UseElementYAttribute) attributes on the [‘use’](#UseElement) element. The referenced [‘symbol’](#SymbolElement) and its contents are deep-cloned into the generated tree, with the exception that the [‘symbol’](#SymbolElement) is replaced by an [‘svg’](#SVGElement). This generated [‘svg’](#SVGElement) will always have explicit values for attributes [‘width’](#SVGElementWidthAttribute) and [‘height’](#SVGElementHeightAttribute). If attributes [‘width’](#UseElementWidthAttribute) and/or [‘height’](#UseElementHeightAttribute) are provided on the [‘use’](#UseElement) element, then these attributes will be transferred to the generated [‘svg’](#SVGElement). If attributes [‘width’](#UseElementWidthAttribute) and/or [‘height’](#UseElementHeightAttribute) are not specified, the generated [‘svg’](#SVGElement) element will use values of '100%' for these attributes.
- **If the [‘use’](#UseElement) element references an [‘svg’](#SVGElement) element:** In the generated content, the [‘use’](#UseElement) will be replaced by [‘g’](#GElement), where all attributes from the [‘use’](#UseElement) element except for [‘x’](#UseElementXAttribute), [‘y’](#UseElementYAttribute), [‘width’](#UseElementWidthAttribute), [‘height’](#UseElementHeightAttribute) and [‘xlink:href’](#UseElementHrefAttribute) are transferred to the generated [‘g’](#GElement) element. An additional transformation translate(x,y) is appended to the end (i.e., right-side) of the [‘transform’](coords.md#TransformAttribute) attribute on the generated [‘g’](#GElement), where x and y represent the values of the [‘x’](#UseElementXAttribute) and [‘y’](#UseElementYAttribute) attributes on the [‘use’](#UseElement) element. The referenced [‘svg’](#SVGElement) and its contents are deep-cloned into the generated tree. If attributes [‘width’](#UseElementWidthAttribute) and/or [‘height’](#UseElementHeightAttribute) are provided on the [‘use’](#UseElement) element, then these values will override the corresponding attributes on the [‘svg’](#SVGElement) in the generated tree.
- **Otherwise:** In the generated content, the [‘use’](#UseElement) will be replaced by [‘g’](#GElement), where all attributes from the [‘use’](#UseElement) element except for [‘x’](#UseElementXAttribute), [‘y’](#UseElementYAttribute), [‘width’](#UseElementWidthAttribute), [‘height’](#UseElementHeightAttribute) and [‘xlink:href’](#UseElementHrefAttribute) are transferred to the generated [‘g’](#GElement) element. An additional transformation translate(x,y) is appended to the end (i.e., right-side) of the [‘transform’](coords.md#TransformAttribute) attribute on the generated [‘g’](#GElement), where x and y represent the values of the [‘x’](#UseElementXAttribute) and [‘y’](#UseElementYAttribute) attributes on the [‘use’](#UseElement) element. The referenced object and its contents are deep-cloned into the generated tree.

For user agents that support [Styling with CSS](styling.md#StylingWithCSS), the generated [‘g’](#GElement) element carries along with it the "cascaded" property values on the [‘use’](#UseElement) element which result from the CSS cascade ([[CSS2](refs.md#ref-CSS2)], chapter 6). Additionally, the copy (deep clone) of the referenced resource carries along with it the "cascaded" property values resulting from the CSS cascade on the original (i.e., referenced) elements. Thus, the result of various CSS selectors in combination with the [‘class’](styling.md#ClassAttribute) and [‘style’](styling.md#StyleAttribute) attributes are, in effect, replaced by the functional equivalent of a [‘style’](styling.md#StyleAttribute) attribute in the generated content which conveys the "cascaded" property values.

Example Use01 below has a simple [‘use’](#UseElement) on a [‘rect’](shapes.md#RectElement).

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 100 30" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>Example Use01 - Simple case of 'use' on a 'rect'</desc>
  <defs>
    <rect id="MyRect" width="60" height="10"/>
  </defs>
  <rect x=".1" y=".1" width="99.8" height="29.8"
        fill="none" stroke="blue" stroke-width=".2" />
  <use x="20" y="10" xlink:href="#MyRect" />
</svg>
```

Example Use01

| [image: Example Use01 — Simple case of 'use' on a 'rect'] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use01.svg)

The visual effect would be equivalent to the following document:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 100 30"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example Use01-GeneratedContent - Simple case of 'use' on a 'rect'</desc>
  <!-- 'defs' section left out -->

  <rect x=".1" y=".1" width="99.8" height="29.8"
        fill="none" stroke="blue" stroke-width=".2" />

  <!-- Start of generated content. Replaces 'use' -->
  <g transform="translate(20,10)">
    <rect width="60" height="10"/>
  </g>
  <!-- End of generated content -->

</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use01-GeneratedContent.svg)

Example Use02 below has a [‘use’](#UseElement) on a [‘symbol’](#SymbolElement).

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 100 30" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>Example Use02 - 'use' on a 'symbol'</desc>
  <defs>
    <symbol id="MySymbol" viewBox="0 0 20 20">
      <desc>MySymbol - four rectangles in a grid</desc>
      <rect x="1" y="1" width="8" height="8"/>
      <rect x="11" y="1" width="8" height="8"/>
      <rect x="1" y="11" width="8" height="8"/>
      <rect x="11" y="11" width="8" height="8"/>
    </symbol>
  </defs>
  <rect x=".1" y=".1" width="99.8" height="29.8"
        fill="none" stroke="blue" stroke-width=".2" />
  <use x="45" y="10" width="10" height="10" 
       xlink:href="#MySymbol" />
</svg>
```

Example Use02

| [image: Example Use02 — 'use' on a 'symbol'] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use02.svg)

The visual effect would be equivalent to the following document:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 100 30"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example Use02-GeneratedContent - 'use' on a 'symbol'</desc>

  <!-- 'defs' section left out -->

  <rect x=".1" y=".1" width="99.8" height="29.8"
        fill="none" stroke="blue" stroke-width=".2" />

  <!-- Start of generated content. Replaces 'use' -->
  <g transform="translate(45, 10)" >
    <!-- Start of referenced 'symbol'. 'symbol' replaced by 'svg',
         with x,y,width,height=0,0,100%,100% -->
    <svg width="10" height="10" 
         viewBox="0 0 20 20">
      <rect x="1" y="1" width="8" height="8"/>
      <rect x="11" y="1" width="8" height="8"/>
      <rect x="1" y="11" width="8" height="8"/>
      <rect x="11" y="11" width="8" height="8"/>
    </svg>
    <!-- End of referenced symbol -->
  </g>
  <!-- End of generated content -->

</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use02-GeneratedContent.svg)

Example Use03 illustrates what happens when a [‘use’](#UseElement) has a [‘transform’](coords.md#TransformAttribute) attribute.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 100 30" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>Example Use03 - 'use' with a 'transform' attribute</desc>
  <defs>
    <rect id="MyRect" x="0" y="0" width="60" height="10"/>
  </defs>
  <rect x=".1" y=".1" width="99.8" height="29.8"
        fill="none" stroke="blue" stroke-width=".2" />
  <use xlink:href="#MyRect"
       transform="translate(20,2.5) rotate(10)" />
</svg>
```

Example Use03

| [image: Example Use03 — 'use' with a 'transform' attribute] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use03.svg)

The visual effect would be equivalent to the following document:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 100 30"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example Use03-GeneratedContent - 'use' with a 'transform' attribute</desc>

  <!-- 'defs' section left out -->

  <rect x=".1" y=".1" width="99.8" height="29.8"
        fill="none" stroke="blue" stroke-width=".2" />

  <!-- Start of generated content. Replaces 'use' -->
  <g transform="translate(20,2.5) rotate(10)">
    <rect x="0" y="0" width="60" height="10"/>
  </g>
  <!-- End of generated content -->

</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use03-GeneratedContent.svg)

Example Use04 illustrates a [‘use’](#UseElement) element with various methods of applying CSS styling.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3cm" viewBox="0 0 1200 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>Example Use04 - 'use' with CSS styling</desc>
  <defs style=" /* rule 9 */ stroke-miterlimit: 10" >
    <path id="MyPath" d="M300 50 L900 50 L900 250 L300 250"
                     class="MyPathClass"
                     style=" /* rule 10 */ stroke-dasharray:300,100" />
  </defs>
  <style type="text/css">
    <![CDATA[
      /* rule 1 */ #MyUse { fill: blue }
      /* rule 2 */ #MyPath { stroke: red }
      /* rule 3 */ use { fill-opacity: .5 }
      /* rule 4 */ path { stroke-opacity: .5 }
      /* rule 5 */ .MyUseClass { stroke-linecap: round }
      /* rule 6 */ .MyPathClass { stroke-linejoin: bevel }
      /* rule 7 */ use > path { shape-rendering: optimizeQuality }
      /* rule 8 */ g > path { visibility: hidden }
    ]]>
  </style>

  <rect x="0" y="0" width="1200" height="300"
         style="fill:none; stroke:blue; stroke-width:3"/>
  <g style=" /* rule 11 */ stroke-width:40">
    <use id="MyUse" xlink:href="#MyPath" 
         class="MyUseClass"
         style="/* rule 12 */ stroke-dashoffset:50" />
  </g>
</svg>
```

Example Use04

| [image: Example Use04 — 'use' with CSS styling] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use04.svg)

The visual effect would be equivalent to the following document. Observe that some of the style rules above apply to the generated content (i.e., rules 1-6, 10-12), whereas others do not (i.e., rules 7-9). The rules which do not affect the generated content are:

- Rules 7 and 8: CSS selectors only apply to the formal document tree, not on the generated tree; thus, these selectors will not yield a match.
- Rule 9: The generated tree only inherits from the ancestors of the [‘use’](#UseElement) element and does not inherit from the ancestors of the referenced element; thus, this rule does not affect the generated content.

In the generated content below, the selectors that yield a match have been transferred into inline [‘style’](styling.md#StyleAttribute) attributes for illustrative purposes.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3cm" viewBox="0 0 1200 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example Use04-GeneratedContent - 'use' with a 'transform' attribute</desc>

  <!-- 'style' and 'defs' sections left out -->

  <rect x="0" y="0" width="1200" height="300"
         style="fill:none; stroke:blue; stroke-width:3"/>
  <g style="/* rule 11 */ stroke-width:40">

    <!-- Start of generated content. Replaces 'use' -->
    <g style="/* rule 1 */ fill:blue;
                 /* rule 3 */ fill-opacity:.5;
                 /* rule 5 */ stroke-linecap:round;
                 /* rule 12 */ stroke-dashoffset:50" >
      <path d="M300 50 L900 50 L900 250 L300 250"
            style="/* rule 2 */ stroke:red; 
                   /* rule 4 */ stroke-opacity:.5; 
                   /* rule 6 */ stroke-linejoin: bevel; 
                   /* rule 10 */ stroke-dasharray:300,100" />
    </g> 
    <!-- End of generated content -->

  </g>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/struct/Use04-GeneratedContent.svg)

When a [‘use’](#UseElement) references another element which is another [‘use’](#UseElement) or whose content contains a [‘use’](#UseElement) element, then the deep cloning approach described above is recursive. However, a set of references that directly or indirectly reference a element to create a circular dependency is an error, as described in [References and the ‘defs’ element](#Head).

*Attribute definitions:*

**<a id="UseElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of one corner of the rectangular region into which the referenced element is placed. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="UseElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of one corner of the rectangular region into which the referenced element is placed. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="UseElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: The width of the rectangular region into which the referenced element is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of this element. [Animatable](animate.md#Animatable): yes.
**<a id="UseElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: The height of the rectangular region into which the referenced element is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of this element. [Animatable](animate.md#Animatable): yes.
**<a id="UseElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: A [IRI reference](linking.md#IRIReference) to an element/fragment within an SVG document. [Animatable](animate.md#Animatable): yes.
## <a id="ImageElement"></a>5.7 The ‘image’ element

‘image’

**Categories:**
: [Graphics element](intro.md#TermGraphicsElement), [graphics referencing element](intro.md#TermGraphicsReferencingElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute), [‘systemLanguage’](#SystemLanguageAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#ImageElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘x’](#ImageElementXAttribute)
- [‘y’](#ImageElementYAttribute)
- [‘width’](#ImageElementWidthAttribute)
- [‘height’](#ImageElementHeightAttribute)
- [‘xlink:href’](#ImageElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGImageElement](#InterfaceSVGImageElement)

The [‘image’](#ImageElement) element indicates that the contents of a complete file are to be rendered into a given rectangle within the current user coordinate system. The [‘image’](#ImageElement) element can refer to raster image files such as PNG or JPEG or to files with MIME type of "image/svg+xml". [Conforming SVG viewers](conform.md#ConformingSVGViewers) need to support at least PNG, JPEG and SVG format files.

The result of processing an [‘image’](#ImageElement) is always a four-channel RGBA result. When an [‘image’](#ImageElement) element references a raster image file such as PNG or JPEG files which only has three channels (RGB), then the effect is as if the object were converted into a 4-channel RGBA image with the alpha channel uniformly set to 1. For a single-channel raster image, the effect is as if the object were converted into a 4-channel RGBA image, where the single channel from the referenced object is used to compute the three color channels and the alpha channel is uniformly set to 1.

An [‘image’](#ImageElement) element establishes a new viewport for the referenced file as described in [Establishing a new viewport](coords.md#EstablishingANewViewport).  The bounds for the new viewport are defined by attributes [‘x’](#ImageElementXAttribute), [‘y’](#ImageElementYAttribute), [‘width’](#ImageElementWidthAttribute) and [‘height’](#ImageElementHeightAttribute). The placement and scaling of the referenced image are controlled by the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute on the [‘image’](#ImageElement) element.

When an [‘image’](#ImageElement) element references an SVG image, the [‘clip’](masking.md#ClipProperty) and [‘overflow’](masking.md#OverflowProperty) properties on the root element in the referenced SVG image are ignored (in the same manner as the [‘x’](#ImageElementXAttribute), [‘y’](#ImageElementYAttribute), [‘width’](#ImageElementWidthAttribute) and [‘height’](#ImageElementHeightAttribute) attributes are ignored). Unless the value of [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) on the [‘image’](#ImageElement) element starts with 'defer', the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute on the root element in the referenced SVG image is also ignored (see [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) for details). Instead, the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute on the referencing [‘image’](#ImageElement) element defines how the SVG image content is fitted into the viewport and the [‘clip’](masking.md#ClipProperty) and [‘overflow’](masking.md#OverflowProperty) properties on the [‘image’](#ImageElement) element define how the SVG image content is clipped (or not) relative to the viewport.

The value of the [‘viewBox’](coords.md#ViewBoxAttribute) attribute to use when evaluating the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute is defined by the referenced content. For content that clearly identifies a viewBox (e.g. an SVG file with the [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [outermost svg element](intro.md#TermOutermostSVGElement)) that value should be used. For most raster content (PNG, JPEG) the bounds of the image should be used (i.e. the [‘image’](#ImageElement) element has an implicit [‘viewBox’](coords.md#ViewBoxAttribute) of '0 0 raster-image-width raster-image-height'). Where no value is readily available (e.g. an SVG file with no [‘viewBox’](coords.md#ViewBoxAttribute) attribute on the [outermost svg element](intro.md#TermOutermostSVGElement)) the [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attribute is ignored, and only the translation due to the [‘x’](#ImageElementXAttribute) & [‘y’](#ImageElementYAttribute) attributes of the viewport is used to display the content.

For example, if the image element referenced a PNG or JPEG and preserveAspectRatio="xMinYMin meet", then the aspect ratio of the raster would be preserved (which means that the scale factor from image's coordinates to current user space coordinates would be the same for both X and Y), the raster would be sized as large as possible while ensuring that the entire raster fits within the viewport, and the top/left of the raster would be aligned with the top/left of the viewport as defined by the attributes [‘x’](#ImageElementXAttribute), [‘y’](#ImageElementYAttribute), [‘width’](#ImageElementWidthAttribute) and [‘height’](#ImageElementHeightAttribute) on the [‘image’](#ImageElement) element.  If the value of [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) was 'none' then aspect ratio of the image would not be preserved. The image would be fitted such that the top/left corner of the raster exactly aligns with coordinate ([‘x’](#ImageElementXAttribute), [‘y’](#ImageElementYAttribute)) and the bottom/right corner of the raster exactly aligns with coordinate ([‘x’](#ImageElementXAttribute)+[‘width’](#ImageElementWidthAttribute), [‘y’](#ImageElementYAttribute)+[‘height’](#ImageElementHeightAttribute)).

The resource referenced by the [‘image’](#ImageElement) element represents a separate document which generates its own parse tree and document object model (if the resource is XML). Thus, there is no inheritance of properties into the image.

Unlike [‘use’](#UseElement), the [‘image’](#ImageElement) element cannot reference elements within an SVG file.

*Attribute definitions:*

**<a id="ImageElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of one corner of the rectangular region into which the referenced document is placed. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="ImageElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of one corner of the rectangular region into which the referenced document is placed. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="ImageElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: The width of the rectangular region into which the referenced document is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
**<a id="ImageElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: The height of the rectangular region into which the referenced document is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
**<a id="ImageElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: A [IRI reference](linking.md#IRIReference). [Animatable](animate.md#Animatable): yes.
**<a id="ImageElementPreserveAspectRatioAttribute"></a> preserveAspectRatio = "[[defer] <align> [<meetOrSlice>]](coords.md#PreserveAspectRatioAttribute)" **
:

See [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute).

If attribute [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) is not specified, then the effect is as if a value of xMidYMid meet were specified.

[Animatable](animate.md#Animatable): yes.

An example:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="4in" height="3in" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <desc>This graphic links to an external image
  </desc>
  <image x="200" y="200" width="100px" height="100px"
         xlink:href="myimage.png">
    <title>My image</title>
  </image>
</svg>
```

## <a id="ConditionalProcessing"></a>5.8 Conditional processing

### <a id="ConditionalProcessingOverview"></a>5.8.1 Conditional processing overview

SVG contains a [‘switch’](#SwitchElement) element along with attributes [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) to provide an ability to specify alternate viewing depending on the capabilities of a given user agent or the user's language.

Attributes [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) act as tests and return either true or false results. The [‘switch’](#SwitchElement) renders the first of its children for which all of these attributes test true. If the given attribute is not specified, then a true value is assumed.

Similar to the [‘display’](painting.md#DisplayProperty) property, conditional processing attributes only affect the direct rendering of elements and do not prevent elements from being successfully referenced by other elements (such as via a [‘use’](#UseElement)).

In consequence:

- [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) attributes affect [‘a’](linking.md#AElement), [‘altGlyph’](text.md#AltGlyphElement), [‘foreignObject’](extend.md#ForeignObjectElement), [‘textPath’](text.md#TextPathElement), [‘tref’](text.md#TRefElement), and [‘tspan’](text.md#TSpanElement) elements.
- [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) attributes will have no effect on [‘mask’](masking.md#MaskElement), [‘clipPath’](masking.md#ClipPathElement), and [‘pattern’](pservers.md#PatternElement) elements.
- [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) attributes do not apply to the [‘defs’](#DefsElement), and [‘cursor’](interact.md#CursorElement) elements because they are not part of the rendering tree.
- [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) attributes affect [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), and [‘set’](animate.md#SetElement) elements. If the conditional statement on these animation elements fails, the animation will never be triggered.

### <a id="SwitchElement"></a>5.8.2 The ‘switch’ element

‘switch’

**Categories:**
: [Container element](intro.md#TermContainerElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](#TitleElement)
- [shape elements](intro.md#TermShapeElement) — [‘circle’](shapes.md#CircleElement), [‘ellipse’](shapes.md#EllipseElement), [‘line’](shapes.md#LineElement), [‘path’](paths.md#PathElement), [‘polygon’](shapes.md#PolygonElement), [‘polyline’](shapes.md#PolylineElement), [‘rect’](shapes.md#RectElement)
- [‘a’](linking.md#AElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘g’](#GElement)
- [‘image’](#ImageElement)
- [‘svg’](#SVGElement)
- [‘switch’](#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘use’](#UseElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute), [‘systemLanguage’](#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](#IDAttribute), [‘xml:base’](#XMLBaseAttribute), [‘xml:lang’](#XMLLangAttribute), [‘xml:space’](#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)

**DOM Interfaces:**
:

- [SVGSwitchElement](#InterfaceSVGSwitchElement)

The [‘switch’](#SwitchElement) element evaluates the [‘requiredFeatures’](#RequiredFeaturesAttribute), [‘requiredExtensions’](#RequiredExtensionsAttribute) and [‘systemLanguage’](#SystemLanguageAttribute) attributes on its direct child elements in order, and then processes and renders the first child for which these attributes evaluate to true. All others will be bypassed and therefore not rendered. If the child element is a container element such as a [‘g’](#GElement), then the entire subtree is either processed/rendered or bypassed/not rendered.

Note that the values of properties [‘display’](painting.md#DisplayProperty) and [‘visibility’](painting.md#VisibilityProperty) have no effect on [‘switch’](#SwitchElement) element processing. In particular, setting [‘display’](painting.md#DisplayProperty) to none on a child of a [‘switch’](#SwitchElement) element has no effect on true/false testing associated with [‘switch’](#SwitchElement) element processing.

For more information and an example, see [Embedding foreign object types](extend.md#ForeignObjectElement).

### <a id="ConditionalProcessingRequiredFeaturesAttribute"></a>5.8.3 The ‘requiredFeatures’ attribute

Definition of requiredFeatures:

**<a id="RequiredFeaturesAttribute"></a>requiredFeatures = *list-of-features***
: The value is a list of feature strings, with the individual values separated by white space. Determines whether all of the named *features* are supported by the user agent. Only feature strings defined in the [Feature String](feature.md) appendix are allowed. If all of the given features are supported, then the attribute evaluates to true; otherwise, the current element and its children are skipped and thus will not be rendered. [Animatable](animate.md#Animatable): no.
If the attribute is not present, then its implicit return value is "true". If a null string or empty string value is given to attribute [‘requiredFeatures’](#RequiredFeaturesAttribute), the attribute returns "false".

[‘requiredFeatures’](#RequiredFeaturesAttribute) is often used in conjunction with the [‘switch’](#SwitchElement) element. If the [‘requiredFeatures’](#RequiredFeaturesAttribute) is used in other situations, then it represents a simple switch on the given element whether to render the element or not.

### <a id="ConditionalProcessingRequiredExtensionsAttribute"></a>5.8.4 The ‘requiredExtensions’ attribute

The [‘requiredExtensions’](#RequiredExtensionsAttribute) attribute defines a list of required language extensions. Language extensions are capabilities within a user agent that go beyond the feature set defined in this specification. Each extension is identified by an [IRI reference](linking.md#IRIReference).

Definition of requiredExtensions:

**<a id="RequiredExtensionsAttribute"></a>requiredExtensions = *list-of-extensions***
: The value is a list of [IRI reference](linking.md#IRIReference)s which identify the required extensions, with the individual values separated by white space. Determines whether all of the named *extensions* are supported by the user agent. If all of the given extensions are supported, then the attribute evaluates to true; otherwise, the current element and its children are skipped and thus will not be rendered. [Animatable](animate.md#Animatable): no.
If a given [IRI reference](linking.md#IRIReference) contains white space within itself, that white space must be escaped.

If the attribute is not present, then its implicit return value is "true". If a null string or empty string value is given to attribute [‘requiredExtensions’](#RequiredExtensionsAttribute), the attribute returns "false".

[‘requiredExtensions’](#RequiredExtensionsAttribute) is often used in conjunction with the [‘switch’](#SwitchElement) element. If the [‘requiredExtensions’](#RequiredExtensionsAttribute) is used in other situations, then it represents a simple switch on the given element whether to render the element or not.

The IRI names for the extension should include versioning information, such as "http://example.org/SVGExtensionXYZ/1.0", so that script writers can distinguish between different versions of a given extension.

### <a id="ConditionalProcessingSystemLanguageAttribute"></a>5.8.5 The ‘systemLanguage’ attribute

<a id="SystemLanguageAttribute"></a>The attribute value is a comma-separated list of language names as defined in BCP 47 [[BCP47](refs.md#ref-BCP47)].

Evaluates to "true" if one of the languages indicated by user preferences exactly equals one of the languages given in the value of this parameter, or if one of the languages indicated by user preferences exactly equals a prefix of one of the languages given in the value of this parameter such that the first tag character following the prefix is "-".

Evaluates to "false" otherwise.

Note: This use of a prefix matching rule does not imply that language tags are assigned to languages in such a way that it is always true that if a user understands a language with a certain tag, then this user will also understand all languages with tags for which this tag is a prefix.

The prefix rule simply allows the use of prefix tags if this is the case.

Implementation note: When making the choice of linguistic preference available to the user, implementers should take into account the fact that users are not familiar with the details of language matching as described above, and should provide appropriate guidance. As an example, users may assume that on selecting "en-gb", they will be served any kind of English document if British English is not available. The user interface for setting user preferences should guide the user to add "en" to get the best matching behavior.

Multiple languages MAY be listed for content that is intended for multiple audiences. For example, content that is presented simultaneously in the original Maori and English versions, would call for:

`<text systemLanguage="mi, en"><!-- content goes here --></text>`

However, just because multiple languages are present within the object on which the [‘systemLanguage’](#SystemLanguageAttribute) test attribute is placed, this does not mean that it is intended for multiple linguistic audiences. An example would be a beginner's language primer, such as "A First Lesson in Latin," which is clearly intended to be used by an English-literate audience. In this case, the [‘systemLanguage’](#SystemLanguageAttribute) test attribute should only include "en".

Authoring note: Authors should realize that if several alternative language objects are enclosed in a [‘switch’](#SwitchElement), and none of them matches, this may lead to situations where no content is displayed. It is thus recommended to include a "catch-all" choice at the end of such a [‘switch’](#SwitchElement) which is acceptable in all cases.

For the [‘systemLanguage’](#SystemLanguageAttribute) attribute: [Animatable](animate.md#Animatable): no.

If the attribute is not present, then its implicit return value is "true". If a null string or empty string value is given to attribute [‘systemLanguage’](#SystemLanguageAttribute), the attribute returns "false".

[‘systemLanguage’](#SystemLanguageAttribute) is often used in conjunction with the [‘switch’](#SwitchElement) element. If the [‘systemLanguage’](#SystemLanguageAttribute) is used in other situations, then it represents a simple switch on the given element whether to render the element or not.

### <a id="ApplicabilityOfTestAttributes"></a>5.8.6 Applicability of test attributes

The following list describes the applicability of the test attributes to the elements that do not directly produce rendering.

- the test attributes do not effect the [‘mask’](masking.md#MaskElement), [‘clipPath’](masking.md#ClipPathElement), [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement) and [‘pattern’](pservers.md#PatternElement) elements. The test attributes on a referenced element do not affect the rendering of the referencing element.
- the test attributes do not effect the [‘defs’](#DefsElement), and [‘cursor’](interact.md#CursorElement) elements as they are not part of the rendering tree.
- an animation element ([‘animate’](animate.md#AnimateElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘animateColor’](animate.md#AnimateColorElement) and [‘set’](animate.md#SetElement)) will never be triggered if it has a test attribute that evaluates to false.

## <a id="ExternalResourcesRequired"></a>5.9 Specifying whether external resources are required for proper rendering

Documents often reference and use the contents of other files (and other Web resources) as part of their rendering. In some cases, authors want to specify that particular resources are required for a document to be considered correct.

Attribute [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute) is available on all container elements and to all elements which potentially can reference external resources. It specifies whether referenced resources that are not part of the current document are required for proper rendering of the given container element or graphics element.

Attribute definition:

**<a id="ExternalResourcesRequiredAttribute"></a>externalResourcesRequired = "false | true"**
:

**false**
: (The default value.) Indicates that resources external to the current document are optional. Document rendering can proceed even if external resources are unavailable to the current element and its descendants.
**true**
: Indicates that resources external to the current document are required. If an external resource is not available, progressive rendering is suspended, the document's [SVGLoad](interact.md#LoadEvent) event is not fired and the animation timeline does not begin until that resource and all other required resources become available, have been parsed and are ready to be rendered. If a timeout event occurs on a required resource, then the document goes into an error state (see [Error processing](implnote.md#ErrorProcessing)). The document remains in an error state until all required resources become available.
This attribute applies to all types of resource references, including style sheets, color profiles (see [Color profile descriptions](color.md#ColorProfileDescriptions)) and fonts specified by an [IRI reference](intro.md#TermIRIReference) using a [‘font-face’](fonts.md#FontFaceElement) element or a CSS @font-face specification. In particular, if an element sets externalResourcesRequired="true", then all style sheets must be available since any style sheet might affect the rendering of that element.

Attribute [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute) is not inheritable (from a sense of attribute value inheritance), but if set on a container element, its value will apply to all elements within the container.

Because setting externalResourcesRequired="true" on a container element will have the effect of disabling progressive display of the contents of that container, if that container includes elements that reference external resources, tools that generate SVG content are cautioned against simply setting externalResourcesRequired="true" on the [outermost svg element](intro.md#TermOutermostSVGElement) on a universal basis. Instead, it is better to specify externalResourcesRequired="true" on those particular graphics elements or container elements which specifically need the availability of external resources in order to render properly.

For [‘externalResourcesRequired’](#ExternalResourcesRequiredAttribute): [Animatable](animate.md#Animatable): no.

## <a id="CommonAttributes"></a>5.10 Common attributes

### <a id="Core.attrib"></a>5.10.1 Attributes common to all elements: ‘id’ and ‘xml:base’

The [‘id’](#IDAttribute) and [‘xml:base’](#XMLBaseAttribute) attributes are available on all SVG elements:

*Attribute definitions:*

**<a id="IDAttribute"></a>id = "name"**
: Standard XML attribute for assigning a unique *name* to an element. Refer to the Extensible Markup Language (XML) 1.0 Recommendation [[XML10](refs.md#ref-XML10)]. [Animatable](animate.md#Animatable): no.
**<a id="XMLBaseAttribute"></a>xml:base = "[<iri>](types.md#DataTypeIRI)"**
: Specifies a base IRI other than the base IRI of the document or external entity. Refer to the XML Base specification [[XML-BASE](refs.md#ref-XML-BASE)]. [Animatable](animate.md#Animatable): no.
### <a id="LangSpaceAttrs"></a>5.10.2 The ‘xml:lang’ and ‘xml:space’ attributes

Elements that might contain character data content have attributes [‘xml:lang’](#XMLLangAttribute) and [‘xml:space’](#XMLSpaceAttribute).

*Attribute definitions:*

**<a id="XMLLangAttribute"></a>xml:lang = "languageID"**
: Standard XML attribute to specify the language (e.g., English) used in the contents and attribute values of particular elements. Refer to the Extensible Markup Language (XML) 1.0 Recommendation [[XML10](refs.md#ref-XML10)]. [Animatable](animate.md#Animatable): no.
**<a id="XMLSpaceAttribute"></a> xml:space = "{default | preserve}"**
: Standard XML attribute to specify whether white space is preserved in character data. The only possible values are 'default' and 'preserve'. Refer to the Extensible Markup Language (XML) 1.0 Recommendation [[XML10](refs.md#ref-XML10)] and to the discussion [white space handling](text.md#WhiteSpace) in SVG. [Animatable](animate.md#Animatable): no.
## <a id="DOMInterfaces"></a>5.11 DOM interfaces

### <a id="InterfaceSVGDocument"></a>5.11.1 Interface SVGDocument

When an [‘svg’](#SVGElement) element is embedded inline as a component of a document from another namespace, such as when an [‘svg’](#SVGElement) element is embedded inline within an XHTML document [[XHTML](refs.md#ref-XHTML)], then an [SVGDocument](#InterfaceSVGDocument) object will not exist; instead, the root object in the document object hierarchy will be a Document object of a different type, such as an HTMLDocument object.

However, an [SVGDocument](#InterfaceSVGDocument) object will indeed exist when the root element of the XML document hierarchy is an [‘svg’](#SVGElement) element, such as when viewing a stand-alone SVG file (i.e., a file with MIME type "image/svg+xml"). In this case, the [SVGDocument](#InterfaceSVGDocument) object will be the root object of the document object model hierarchy.

In the case where an SVG document is embedded by reference, such as when an XHTML document has an ‘object’ element whose ‘href’ attribute references an SVG document (i.e., a document whose MIME type is "image/svg+xml" and whose root element is thus an [‘svg’](#SVGElement) element), there will exist two distinct DOM hierarchies. The first DOM hierarchy will be for the referencing document (e.g., an XHTML document). The second DOM hierarchy will be for the referenced SVG document. In this second DOM hierarchy, the root object of the document object model hierarchy is an [SVGDocument](#InterfaceSVGDocument) object.

The [SVGDocument](#InterfaceSVGDocument) interface contains a similar list of attributes and methods to the HTMLDocument interface described in the Document Object Model (HTML) Level 1 chapter of the [[DOM1](refs.md#ref-DOM1)] specification.

```
interface SVGDocument : Document,
                        DocumentEvent {
  readonly attribute DOMString title;
  readonly attribute DOMString referrer;
  readonly attribute DOMString domain;
  readonly attribute DOMString URL;
  readonly attribute SVGSVGElement rootElement;
};
```

**Attributes:**
:

**<a id="__svg__SVGDocument__title"></a>**title** (readonly DOMString)**
:

The title of a document as specified by the [‘title’](#TitleElement) sub-element of the [‘svg’](#SVGElement) root element (i.e., <svg><title>Here is the title</title>...</svg>)

**<a id="__svg__SVGDocument__referrer"></a>**referrer** (readonly DOMString)**
:

Returns the URI of the page that linked to this page. The value is an empty string if the user navigated to the page directly (not through a link, but, for example, via a bookmark).

**<a id="__svg__SVGDocument__domain"></a>**domain** (readonly DOMString)**
:

The domain name of the server that served the document, or a null string if the server cannot be identified by a domain name.

**<a id="__svg__SVGDocument__URL"></a>**URL** (readonly DOMString)**
:

The complete URI of the document.

**<a id="__svg__SVGDocument__rootElement"></a>**rootElement** (readonly [SVGSVGElement](#InterfaceSVGSVGElement))**
:

The root [‘svg’](#SVGElement) in the document hierarchy.

### <a id="InterfaceSVGSVGElement"></a>5.11.2 Interface SVGSVGElement

A key interface definition is the [SVGSVGElement](#InterfaceSVGSVGElement) interface, which is the interface that corresponds to the [‘svg’](#SVGElement) element. This interface contains various miscellaneous commonly-used utility methods, such as matrix operations and the ability to control the time of redraw on visual rendering devices.

[SVGSVGElement](#InterfaceSVGSVGElement) extends ViewCSS and DocumentCSS to provide access to the computed values of properties and the override style sheet as described in DOM Level 2 Style [[DOM2STYLE](refs.md#ref-DOM2STYLE)].

```
interface SVGSVGElement : SVGElement,
                          SVGTests,
                          SVGLangSpace,
                          SVGExternalResourcesRequired,
                          SVGStylable,
                          SVGLocatable,
                          SVGFitToViewBox,
                          SVGZoomAndPan,
                          DocumentEvent,
                          ViewCSS,
                          DocumentCSS {

  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
           attribute DOMString contentScriptType setraises(DOMException);
           attribute DOMString contentStyleType setraises(DOMException);
  readonly attribute SVGRect viewport;
  readonly attribute float pixelUnitToMillimeterX;
  readonly attribute float pixelUnitToMillimeterY;
  readonly attribute float screenPixelToMillimeterX;
  readonly attribute float screenPixelToMillimeterY;
  readonly attribute boolean useCurrentView;
  readonly attribute SVGViewSpec currentView;
           attribute float currentScale;
  readonly attribute SVGPoint currentTranslate;

  unsigned long suspendRedraw(in unsigned long maxWaitMilliseconds);
  void unsuspendRedraw(in unsigned long suspendHandleID);
  void unsuspendRedrawAll();
  void forceRedraw();
  void pauseAnimations();
  void unpauseAnimations();
  boolean animationsPaused();
  float getCurrentTime();
  void setCurrentTime(in float seconds);
  NodeList getIntersectionList(in SVGRect rect, in SVGElement referenceElement);
  NodeList getEnclosureList(in SVGRect rect, in SVGElement referenceElement);
  boolean checkIntersection(in SVGElement element, in SVGRect rect);
  boolean checkEnclosure(in SVGElement element, in SVGRect rect);
  void deselectAll();
  SVGNumber createSVGNumber();
  SVGLength createSVGLength();
  SVGAngle createSVGAngle();
  SVGPoint createSVGPoint();
  SVGMatrix createSVGMatrix();
  SVGRect createSVGRect();
  SVGTransform createSVGTransform();
  SVGTransform createSVGTransformFromMatrix(in SVGMatrix matrix);
  Element getElementById(in DOMString elementId);
};
```

**Attributes:**
:

**<a id="__svg__SVGSVGElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#SVGElementXAttribute) on the given [‘svg’](#SVGElement) element.

**<a id="__svg__SVGSVGElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#SVGElementYAttribute) on the given [‘svg’](#SVGElement) element.

**<a id="__svg__SVGSVGElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#SVGElementWidthAttribute) on the given [‘svg’](#SVGElement) element.

**<a id="__svg__SVGSVGElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#SVGElementHeightAttribute) on the given [‘svg’](#SVGElement) element.

**<a id="__svg__SVGSVGElement__contentScriptType"></a>**contentScriptType** (DOMString)**
:

Corresponds to attribute [‘contentScriptType’](https://www.w3.org/TR/SVG11/script.html#ContentScriptTypeAttribute) on the given [‘svg’](#SVGElement) element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGSVGElement__contentStyleType"></a>**contentStyleType** (DOMString)**
:

Corresponds to attribute [‘contentStyleType’](styling.md#ContentStyleTypeAttribute) on the given [‘svg’](#SVGElement) element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGSVGElement__viewport"></a>**viewport** (readonly [SVGRect](types.md#InterfaceSVGRect))**
:

The position and size of the viewport (implicit or explicit) that corresponds to this [‘svg’](#SVGElement) element. When the user agent is actually rendering the content, then the position and size values represent the actual values when rendering. The position and size values are unitless values in the coordinate system of the parent element. If no parent element exists (i.e., [‘svg’](#SVGElement) element represents the root of the document tree), if this SVG document is embedded as part of another document (e.g., via the HTML ‘object’ element), then the position and size are unitless values in the coordinate system of the parent document. (If the parent uses CSS or XSL layout, then unitless values represent pixel units for the current CSS or XSL viewport, as described in the CSS2 specification.) If the parent element does not have a coordinate system, then the user agent should provide reasonable default values for this attribute.

The [SVGRect](types.md#InterfaceSVGRect) object is [read only](types.md#ReadOnlyRect).

**<a id="__svg__SVGSVGElement__pixelUnitToMillimeterX"></a>**pixelUnitToMillimeterX** (readonly float)**
:

Size of a pixel units (as defined by CSS2) along the x-axis of the viewport, which represents a unit somewhere in the range of 70dpi to 120dpi, and, on systems that support this, might actually match the characteristics of the target medium. On systems where it is impossible to know the size of a pixel, a suitable default pixel size is provided.

**<a id="__svg__SVGSVGElement__pixelUnitToMillimeterY"></a>**pixelUnitToMillimeterY** (readonly float)**
:

Corresponding size of a pixel unit along the y-axis of the viewport.

**<a id="__svg__SVGSVGElement__screenPixelToMillimeterX"></a>**screenPixelToMillimeterX** (readonly float)**
:

User interface (UI) events in DOM Level 2 indicate the screen positions at which the given UI event occurred. When the user agent actually knows the physical size of a "screen unit", this attribute will express that information; otherwise, user agents will provide a suitable default value such as .28mm.

**<a id="__svg__SVGSVGElement__screenPixelToMillimeterY"></a>**screenPixelToMillimeterY** (readonly float)**
:

Corresponding size of a screen pixel along the y-axis of the viewport.

**<a id="__svg__SVGSVGElement__useCurrentView"></a>**useCurrentView** (readonly boolean)**
:

The initial view (i.e., before magnification and panning) of the current innermost SVG document fragment can be either the "standard" view (i.e., based on attributes on the [‘svg’](#SVGElement) element such as [‘viewBox’](coords.md#ViewBoxAttribute)) or to a "custom" view (i.e., a hyperlink into a particular [‘view’](linking.md#ViewElement) or other element - see [Linking into SVG content: IRI fragments and SVG views](linking.md#LinksIntoSVG)). If the initial view is the "standard" view, then this attribute is false. If the initial view is a "custom" view, then this attribute is true.

**<a id="__svg__SVGSVGElement__currentView"></a>**currentView** (readonly [SVGViewSpec](types.md#InterfaceSVGViewSpec))**
:

The definition of the initial view (i.e., before magnification and panning) of the current innermost SVG document fragment. The meaning depends on the situation:

- If the initial view was a "standard" view, then:
  - the values for [viewBox](types.md#__svg__SVGFitToViewBox__viewBox), [preserveAspectRatio](types.md#__svg__SVGFitToViewBox__preserveAspectRatio) and [zoomAndPan](types.md#__svg__SVGZoomAndPan__zoomAndPan) within [currentView](#__svg__SVGSVGElement__currentView) will match the values for the corresponding DOM attributes that are on SVGSVGElement directly
  - the values for [transform](types.md#__svg__SVGViewSpec__transform) and [viewTarget](types.md#__svg__SVGViewSpec__viewTarget) within [currentView](#__svg__SVGSVGElement__currentView) will be null
- If the initial view was a link into a [‘view’](linking.md#ViewElement) element, then:
  - the values for [viewBox](types.md#__svg__SVGFitToViewBox__viewBox), [preserveAspectRatio](types.md#__svg__SVGFitToViewBox__preserveAspectRatio) and [zoomAndPan](types.md#__svg__SVGZoomAndPan__zoomAndPan) within [currentView](#__svg__SVGSVGElement__currentView) will correspond to the corresponding attributes for the given [‘view’](linking.md#ViewElement) element
  - the values for [transform](types.md#__svg__SVGViewSpec__transform) and [viewTarget](types.md#__svg__SVGViewSpec__viewTarget) within [currentView](#__svg__SVGSVGElement__currentView) will be null
- If the initial view was a link into another element (i.e., other than a [‘view’](linking.md#ViewElement)), then:
  - the values for [viewBox](types.md#__svg__SVGFitToViewBox__viewBox), [preserveAspectRatio](types.md#__svg__SVGFitToViewBox__preserveAspectRatio) and [zoomAndPan](types.md#__svg__SVGZoomAndPan__zoomAndPan) within [currentView](#__svg__SVGSVGElement__currentView) will match the values for the corresponding DOM attributes that are on SVGSVGElement directly for the closest ancestor [‘svg’](#SVGElement) element
  - the values for [transform](types.md#__svg__SVGViewSpec__transform) within [currentView](#__svg__SVGSVGElement__currentView) will be null
  - the [viewTarget](types.md#__svg__SVGViewSpec__viewTarget) within [currentView](#__svg__SVGSVGElement__currentView) will represent the target of the link
- If the initial view was a link into the SVG document fragment using an SVG view specification fragment identifier (i.e., #svgView(...)), then:
  - the values for [viewBox](types.md#__svg__SVGFitToViewBox__viewBox), [preserveAspectRatio](types.md#__svg__SVGFitToViewBox__preserveAspectRatio), [zoomAndPan](types.md#__svg__SVGZoomAndPan__zoomAndPan), [transform](types.md#__svg__SVGViewSpec__transform) and [viewTarget](types.md#__svg__SVGViewSpec__viewTarget) within [currentView](#__svg__SVGSVGElement__currentView) will correspond to the values from the SVG view specification fragment identifier

The object itself and its contents are both read only.

**<a id="__svg__SVGSVGElement__currentScale"></a>**currentScale** (float)**
:

On an [outermost svg element](intro.md#TermOutermostSVGElement), this attribute indicates the current scale factor relative to the initial view to take into account user magnification and panning operations, as described under [Magnification and panning](interact.md#ZoomAndPanAttribute). DOM attributes [currentScale](#__svg__SVGSVGElement__currentScale) and [currentTranslate](#__svg__SVGSVGElement__currentTranslate) are equivalent to the 2x3 matrix [a b c d e f] = [currentScale 0 0 currentScale currentTranslate.x currentTranslate.y]. If "magnification" is enabled (i.e., zoomAndPan="magnify"), then the effect is as if an extra transformation were placed at the outermost level on the SVG document fragment (i.e., outside the [outermost svg element](intro.md#TermOutermostSVGElement)).

When accessed on an [‘svg’](#SVGElement) element that is not an [outermost svg element](intro.md#TermOutermostSVGElement), it is undefined what behavior this attribute has.

**<a id="__svg__SVGSVGElement__currentTranslate"></a>**currentTranslate** (readonly [SVGPoint](coords.md#InterfaceSVGPoint))**
:

On an [outermost svg element](intro.md#TermOutermostSVGElement), the corresponding translation factor that takes into account user "magnification".

When accessed on an [‘svg’](#SVGElement) element that is not an [outermost svg element](intro.md#TermOutermostSVGElement), it is undefined what behavior this attribute has.

**Operations:**
:

**<a id="__svg__SVGSVGElement__suspendRedraw"></a>unsigned long **suspendRedraw**(in unsigned long *maxWaitMilliseconds*)**
:

Takes a time-out value which indicates that redraw shall not occur until:

1. the corresponding unsuspendRedraw() call has been made,
2. an unsuspendRedrawAll() call has been made, or
3. its timer has timed out.

In environments that do not support interactivity (e.g., print media), then redraw shall not be suspended. Calls to suspendRedraw() and unsuspendRedraw() should, but need not be, made in balanced pairs.

To suspend redraw actions as a collection of SVG DOM changes occur, precede the changes to the SVG DOM with a method call similar to:

```
suspendHandleID = suspendRedraw(maxWaitMilliseconds);
```

and follow the changes with a method call similar to:

```
unsuspendRedraw(suspendHandleID);
```

Note that multiple suspendRedraw calls can be used at once and that each such method call is treated independently of the other suspendRedraw method calls.

**Parameters**
:

1.
unsigned long *maxWaitMilliseconds*
The amount of time in milliseconds to hold off before redrawing the device. Values greater than 60 seconds will be truncated down to 60 seconds.

**Returns**
:

A number which acts as a unique identifier for the given suspendRedraw() call. This value must be passed as the parameter to the corresponding unsuspendRedraw() method call.

**<a id="__svg__SVGSVGElement__unsuspendRedraw"></a>void **unsuspendRedraw**(in unsigned long *suspendHandleID*)**
:

Cancels a specified suspendRedraw() by providing a unique suspend handle ID that was returned by a previous suspendRedraw() call.

**Parameters**
:

1.
unsigned long *suspendHandleID*
A number which acts as a unique identifier for the desired suspendRedraw() call. The number supplied must be a value returned from a previous call to suspendRedraw(). If an invalid handle ID value is provided then the request to unsuspendRedraw() is silently ignored.

**<a id="__svg__SVGSVGElement__unsuspendRedrawAll"></a>void **unsuspendRedrawAll**()**
:

Cancels all currently active suspendRedraw() method calls. This method is most useful at the very end of a set of SVG DOM calls to ensure that all pending suspendRedraw() method calls have been cancelled.

**<a id="__svg__SVGSVGElement__forceRedraw"></a>void **forceRedraw**()**
:

In rendering environments supporting interactivity, forces the user agent to immediately redraw all regions of the viewport that require updating.

**<a id="__svg__SVGSVGElement__pauseAnimations"></a>void **pauseAnimations**()**
:

Suspends (i.e., pauses) all currently running animations that are defined within the SVG document fragment corresponding to this [‘svg’](#SVGElement) element, causing the animation clock corresponding to this document fragment to stand still until it is unpaused.

**<a id="__svg__SVGSVGElement__unpauseAnimations"></a>void **unpauseAnimations**()**
:

Unsuspends (i.e., unpauses) currently running animations that are defined within the SVG document fragment, causing the animation clock to continue from the time at which it was suspended.

**<a id="__svg__SVGSVGElement__animationsPaused"></a>boolean **animationsPaused**()**
:

Returns true if this SVG document fragment is in a paused state.

**Returns**
:

Boolean indicating whether this SVG document fragment is in a paused state.

**<a id="__svg__SVGSVGElement__getCurrentTime"></a>float **getCurrentTime**()**
:

Returns the current time in seconds relative to the start time for the current SVG document fragment. If getCurrentTime is called before the document timeline has begun (for example, by script running in a [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement) element before the document's [SVGLoad](interact.md#LoadEvent) event is dispatched), then 0 is returned.

**Returns**
:

The current time in seconds, or 0 if the document timeline has not yet begun.

**<a id="__svg__SVGSVGElement__setCurrentTime"></a>void **setCurrentTime**(in float *seconds*)**
:

Adjusts the clock for this SVG document fragment, establishing a new current time. If setCurrentTime is called before the document timeline has begun (for example, by script running in a [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement) element before the document's [SVGLoad](interact.md#LoadEvent) event is dispatched), then the value of *seconds* in the last invocation of the method gives the time that the document will seek to once the document timeline has begun.

**Parameters**
:

1.
float *seconds*
The new current time in seconds relative to the start time for the current SVG document fragment.

**<a id="__svg__SVGSVGElement__getIntersectionList"></a>NodeList **getIntersectionList**(in [SVGRect](types.md#InterfaceSVGRect) *rect*, in [SVGElement](types.md#InterfaceSVGElement) *referenceElement*)**
:

Returns the list of graphics elements whose rendered content intersects the supplied rectangle. Each candidate graphics element is to be considered a match only if the same graphics element can be a [target of pointer events](interact.md#PointerEventsProperty) as defined in [‘pointer-events’](interact.md#PointerEventsProperty) processing.

**Parameters**
:

1.
[SVGRect](types.md#InterfaceSVGRect) *rect*
The test rectangle. The values are in the [initial coordinate system](coords.md#InitialCoordinateSystem) for the current [‘svg’](#SVGElement) element.
2.
[SVGElement](types.md#InterfaceSVGElement) *referenceElement*
If not null, then any intersected element that doesn't have the referenceElement as ancestor must not be included in the returned NodeList.

**Returns**
:

A list of Elements whose content intersects the supplied rectangle. This NodeList must be implemented identically to the NodeList interface as defined in DOM Level 2 Core ([[DOM2](refs.md#ref-DOM2)], section 1.2) with the exception that the interface is not live.

**<a id="__svg__SVGSVGElement__getEnclosureList"></a>NodeList **getEnclosureList**(in [SVGRect](types.md#InterfaceSVGRect) *rect*, in [SVGElement](types.md#InterfaceSVGElement) *referenceElement*)**
:

Returns the list of graphics elements whose rendered content is entirely contained within the supplied rectangle. Each candidate graphics element is to be considered a match only if the same graphics element can be a [target of pointer events](interact.md#PointerEventsProperty) as defined in [‘pointer-events’](interact.md#PointerEventsProperty) processing.

**Parameters**
:

1.
[SVGRect](types.md#InterfaceSVGRect) *rect*
The test rectangle. The values are in the [initial coordinate system](coords.md#InitialCoordinateSystem) for the current [‘svg’](#SVGElement) element.
2.
[SVGElement](types.md#InterfaceSVGElement) *referenceElement*
If not null, then any intersected element that doesn't have the referenceElement as ancestor must not be included in the returned NodeList.

**Returns**
:

A list of Elements whose content is enclosed by the supplied rectangle. This NodeList must be implemented identically to the NodeList interface as defined in DOM Level 2 Core ([[DOM2](refs.md#ref-DOM2)], section 1.2) with the exception that the interface is not live.

**<a id="__svg__SVGSVGElement__checkIntersection"></a>boolean **checkIntersection**(in [SVGElement](types.md#InterfaceSVGElement) *element*, in [SVGRect](types.md#InterfaceSVGRect) *rect*)**
:

Returns true if the rendered content of the given element intersects the supplied rectangle. Each candidate graphics element is to be considered a match only if the same graphics element can be a [target of pointer events](interact.md#PointerEventsProperty) as defined in [‘pointer-events’](interact.md#PointerEventsProperty) processing.

**Parameters**
:

1.
[SVGElement](types.md#InterfaceSVGElement) *element*
The element on which to perform the given test.
2.
[SVGRect](types.md#InterfaceSVGRect) *rect*
The test rectangle. The values are in the [initial coordinate system](coords.md#InitialCoordinateSystem) for the current [‘svg’](#SVGElement) element.

**Returns**
:

True or false, depending on whether the given element intersects the supplied rectangle.

**<a id="__svg__SVGSVGElement__checkEnclosure"></a>boolean **checkEnclosure**(in [SVGElement](types.md#InterfaceSVGElement) *element*, in [SVGRect](types.md#InterfaceSVGRect) *rect*)**
:

Returns true if the rendered content of the given element is entirely contained within the supplied rectangle. Each candidate graphics element is to be considered a match only if the same graphics element can be a [target of pointer events](interact.md#PointerEventsProperty) as defined in [‘pointer-events’](interact.md#PointerEventsProperty) processing.

**Parameters**
:

1.
[SVGElement](types.md#InterfaceSVGElement) *element*
The element on which to perform the given test.
2.
[SVGRect](types.md#InterfaceSVGRect) *rect*
The test rectangle. The values are in the [initial coordinate system](coords.md#InitialCoordinateSystem) for the current [‘svg’](#SVGElement) element.

**Returns**
:

True or false, depending on whether the given element is enclosed by the supplied rectangle.

**<a id="__svg__SVGSVGElement__deselectAll"></a>void **deselectAll**()**
:

Unselects any selected objects, including any selections of text strings and type-in bars.

**<a id="__svg__SVGSVGElement__createSVGNumber"></a>[SVGNumber](types.md#InterfaceSVGNumber) **createSVGNumber**()**
:

Creates an [SVGNumber](types.md#InterfaceSVGNumber) object outside of any document trees. The object is initialized to a value of zero.

**Returns**
:

An [SVGNumber](types.md#InterfaceSVGNumber) object.

**<a id="__svg__SVGSVGElement__createSVGLength"></a>[SVGLength](types.md#InterfaceSVGLength) **createSVGLength**()**
:

Creates an [SVGLength](types.md#InterfaceSVGLength) object outside of any document trees. The object is initialized to the value of 0 user units.

**Returns**
:

An [SVGLength](types.md#InterfaceSVGLength) object.

**<a id="__svg__SVGSVGElement__createSVGAngle"></a>[SVGAngle](types.md#InterfaceSVGAngle) **createSVGAngle**()**
:

Creates an [SVGAngle](types.md#InterfaceSVGAngle) object outside of any document trees. The object is initialized to the value 0 degrees (unitless).

**Returns**
:

An [SVGAngle](types.md#InterfaceSVGAngle) object.

**<a id="__svg__SVGSVGElement__createSVGPoint"></a>[SVGPoint](coords.md#InterfaceSVGPoint) **createSVGPoint**()**
:

Creates an [SVGPoint](coords.md#InterfaceSVGPoint) object outside of any document trees. The object is initialized to the point (0,0) in the user coordinate system.

**Returns**
:

An [SVGPoint](coords.md#InterfaceSVGPoint) object.

**<a id="__svg__SVGSVGElement__createSVGMatrix"></a>[SVGMatrix](coords.md#InterfaceSVGMatrix) **createSVGMatrix**()**
:

Creates an [SVGMatrix](coords.md#InterfaceSVGMatrix) object outside of any document trees. The object is initialized to the identity matrix.

**Returns**
:

An [SVGMatrix](coords.md#InterfaceSVGMatrix) object.

**<a id="__svg__SVGSVGElement__createSVGRect"></a>[SVGRect](types.md#InterfaceSVGRect) **createSVGRect**()**
:

Creates an [SVGRect](types.md#InterfaceSVGRect) object outside of any document trees. The object is initialized such that all values are set to 0 user units.

**Returns**
:

An [SVGRect](types.md#InterfaceSVGRect) object.

**<a id="__svg__SVGSVGElement__createSVGTransform"></a>[SVGTransform](coords.md#InterfaceSVGTransform) **createSVGTransform**()**
:

Creates an [SVGTransform](coords.md#InterfaceSVGTransform) object outside of any document trees. The object is initialized to an identity matrix transform (SVG_TRANSFORM_MATRIX).

**Returns**
:

An [SVGTransform](coords.md#InterfaceSVGTransform) object.

**<a id="__svg__SVGSVGElement__createSVGTransformFromMatrix"></a>[SVGTransform](coords.md#InterfaceSVGTransform) **createSVGTransformFromMatrix**(in [SVGMatrix](coords.md#InterfaceSVGMatrix) *matrix*)**
:

Creates an [SVGTransform](coords.md#InterfaceSVGTransform) object outside of any document trees. The object is initialized to the given matrix transform (i.e., SVG_TRANSFORM_MATRIX). The values from the parameter *matrix* are copied, the *matrix* parameter is not adopted as [SVGTransform::matrix](coords.md#__svg__SVGTransform__matrix).

**Parameters**
:

1.
[SVGMatrix](coords.md#InterfaceSVGMatrix) *matrix*
The transform matrix.

**Returns**
:

An [SVGTransform](coords.md#InterfaceSVGTransform) object.

**<a id="__svg__SVGSVGElement__getElementById"></a>Element **getElementById**(in DOMString *elementId*)**
:

Searches this SVG document fragment (i.e., the search is restricted to a subset of the document tree) for an Element whose id is given by *elementId*. If an Element is found, that Element is returned. If no such element exists, returns null. Behavior is not defined if more than one element has this id.

**Parameters**
:

1.
DOMString *elementId*
The unique id value for an element.

**Returns**
:

The matching element.

### <a id="InterfaceSVGGElement"></a>5.11.3 Interface SVGGElement

The [SVGSVGElement](#InterfaceSVGSVGElement) interface corresponds to the [‘g’](#GElement) element.

```
interface SVGGElement : SVGElement,
                        SVGTests,
                        SVGLangSpace,
                        SVGExternalResourcesRequired,
                        SVGStylable,
                        SVGTransformable {
};
```

### <a id="InterfaceSVGDefsElement"></a>5.11.4 Interface SVGDefsElement

The [SVGDefsElement](#InterfaceSVGDefsElement) interface corresponds to the [‘defs’](#DefsElement) element.

```
interface SVGDefsElement : SVGElement,
                           SVGTests,
                           SVGLangSpace,
                           SVGExternalResourcesRequired,
                           SVGStylable,
                           SVGTransformable {
};
```

### <a id="InterfaceSVGDescElement"></a>5.11.5 Interface SVGDescElement

The [SVGDescElement](#InterfaceSVGDescElement) interface corresponds to the [‘desc’](#DescElement) element.

```
interface SVGDescElement : SVGElement,
                           SVGLangSpace,
                           SVGStylable {
};
```

### <a id="InterfaceSVGTitleElement"></a>5.11.6 Interface SVGTitleElement

The [SVGTitleElement](#InterfaceSVGTitleElement) interface corresponds to the [‘title’](#TitleElement) element.

```
interface SVGTitleElement : SVGElement,
                            SVGLangSpace,
                            SVGStylable {
};
```

### <a id="InterfaceSVGSymbolElement"></a>5.11.7 Interface SVGSymbolElement

The [SVGSymbolElement](#InterfaceSVGSymbolElement) interface corresponds to the [‘symbol’](#SymbolElement) element.

```
interface SVGSymbolElement : SVGElement,
                             SVGLangSpace,
                             SVGExternalResourcesRequired,
                             SVGStylable,
                             SVGFitToViewBox {
};
```

### <a id="InterfaceSVGUseElement"></a>5.11.8 Interface SVGUseElement

The [SVGUseElement](#InterfaceSVGUseElement) interface corresponds to the [‘use’](#UseElement) element.

```
interface SVGUseElement : SVGElement,
                          SVGURIReference,
                          SVGTests,
                          SVGLangSpace,
                          SVGExternalResourcesRequired,
                          SVGStylable,
                          SVGTransformable {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
  readonly attribute SVGElementInstance instanceRoot;
  readonly attribute SVGElementInstance animatedInstanceRoot;
};
```

**Attributes:**
:

**<a id="__svg__SVGUseElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#UseElementXAttribute) on the given [‘use’](#UseElement) element.

**<a id="__svg__SVGUseElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#UseElementYAttribute) on the given [‘use’](#UseElement) element.

**<a id="__svg__SVGUseElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#UseElementWidthAttribute) on the given [‘use’](#UseElement) element.

**<a id="__svg__SVGUseElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#UseElementHeightAttribute) on the given [‘use’](#UseElement) element.

**<a id="__svg__SVGUseElement__instanceRoot"></a>**instanceRoot** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

The root of the "instance tree". See description of [SVGElementInstance](#InterfaceSVGElementInstance) for a discussion on the instance tree.

**<a id="__svg__SVGUseElement__animatedInstanceRoot"></a>**animatedInstanceRoot** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

If the [‘xlink:href’](#UseElementHrefAttribute) attribute is being animated, contains the current animated root of the "instance tree". If the [‘xlink:href’](#UseElementHrefAttribute) attribute is not currently being animated, contains the same value as [instanceRoot](#__svg__SVGUseElement__instanceRoot). See description of [SVGElementInstance](#InterfaceSVGElementInstance) for a discussion on the instance tree.

### <a id="InterfaceSVGElementInstance"></a>5.11.9 Interface SVGElementInstance

For each [‘use’](#UseElement) element, the SVG DOM maintains a shadow tree (the "instance tree") of objects of type [SVGElementInstance](#InterfaceSVGElementInstance). An [SVGElementInstance](#InterfaceSVGElementInstance) represents a single node in the instance tree. The root object in the instance tree is pointed to by the [instanceRoot](#__svg__SVGUseElement__instanceRoot) attribute on the [SVGUseElement](#InterfaceSVGUseElement) object for the corresponding [‘use’](#UseElement) element.

If the [‘use’](#UseElement) element references a simple graphics element such as a [‘rect’](shapes.md#RectElement), then there is only a single [SVGElementInstance](#InterfaceSVGElementInstance) object, and the [correspondingElement](#__svg__SVGElementInstance__correspondingElement) attribute on this [SVGElementInstance](#InterfaceSVGElementInstance) object is the [SVGRectElement](shapes.md#InterfaceSVGRectElement) that corresponds to the referenced [‘rect’](shapes.md#RectElement) element.

If the [‘use’](#UseElement) element references a [‘g’](#GElement) which contains two [‘rect’](shapes.md#RectElement) elements, then the instance tree contains three [SVGElementInstance](#InterfaceSVGElementInstance) objects, a root [SVGElementInstance](#InterfaceSVGElementInstance) object whose [correspondingElement](#__svg__SVGElementInstance__correspondingElement) is the [SVGGElement](#InterfaceSVGGElement) object for the [‘g’](#GElement), and then two child [SVGElementInstance](#InterfaceSVGElementInstance) objects, each of which has its [correspondingElement](#__svg__SVGElementInstance__correspondingElement) that is an [SVGRectElement](shapes.md#InterfaceSVGRectElement) object.

If the referenced object is itself a [‘use’](#UseElement), or if there are [‘use’](#UseElement) subelements within the referenced object, the instance tree will contain recursive expansion of the indirect references to form a complete tree. For example, if a [‘use’](#UseElement) element references a [‘g’](#GElement), and the [‘g’](#GElement) itself contains a [‘use’](#UseElement), and that [‘use’](#UseElement) references a [‘rect’](shapes.md#RectElement), then the instance tree for the original (outermost) [‘use’](#UseElement) will consist of a hierarchy of [SVGElementInstance](#InterfaceSVGElementInstance) objects, as follows:

```
SVGElementInstance #1 (parentNode=null, firstChild=#2, correspondingElement is the 'g')
  SVGElementInstance #2 (parentNode=#1, firstChild=#3, correspondingElement is the other 'use')
    SVGElementInstance #3 (parentNode=#2, firstChild=null, correspondingElement is the 'rect')
```

```
interface SVGElementInstance : EventTarget {
  readonly attribute SVGElement correspondingElement;
  readonly attribute SVGUseElement correspondingUseElement;
  readonly attribute SVGElementInstance parentNode;
  readonly attribute SVGElementInstanceList childNodes;
  readonly attribute SVGElementInstance firstChild;
  readonly attribute SVGElementInstance lastChild;
  readonly attribute SVGElementInstance previousSibling;
  readonly attribute SVGElementInstance nextSibling;
};
```

**Attributes:**
:

**<a id="__svg__SVGElementInstance__correspondingElement"></a>**correspondingElement** (readonly [SVGElement](types.md#InterfaceSVGElement))**
:

The corresponding element to which this object is an instance. For example, if a [‘use’](#UseElement) element references a [‘rect’](shapes.md#RectElement) element, then an [SVGElementInstance](#InterfaceSVGElementInstance) is created, with its [correspondingElement](#__svg__SVGElementInstance__correspondingElement) being the [SVGRectElement](shapes.md#InterfaceSVGRectElement) object for the [‘rect’](shapes.md#RectElement) element.

**<a id="__svg__SVGElementInstance__correspondingUseElement"></a>**correspondingUseElement** (readonly [SVGUseElement](#InterfaceSVGUseElement))**
:

The corresponding [‘use’](#UseElement) element to which this [SVGElementInstance](#InterfaceSVGElementInstance) object belongs. When [‘use’](#UseElement) elements are nested (e.g., a [‘use’](#UseElement) references another [‘use’](#UseElement) which references a graphics element such as a [‘rect’](shapes.md#RectElement)), then the [correspondingUseElement](#__svg__SVGElementInstance__correspondingUseElement) is the outermost [‘use’](#UseElement) (i.e., the one which indirectly references the [‘rect’](shapes.md#RectElement), not the one with the direct reference).

**<a id="__svg__SVGElementInstance__parentNode"></a>**parentNode** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

The parent of this [SVGElementInstance](#InterfaceSVGElementInstance) within the instance tree. All [SVGElementInstance](#InterfaceSVGElementInstance) objects have a parent except the [SVGElementInstance](#InterfaceSVGElementInstance) which corresponds to the element which was directly referenced by the [‘use’](#UseElement) element, in which case [parentNode](#__svg__SVGElementInstance__parentNode) is null.

**<a id="__svg__SVGElementInstance__childNodes"></a>**childNodes** (readonly [SVGElementInstanceList](#InterfaceSVGElementInstanceList))**
:

An [SVGElementInstanceList](#InterfaceSVGElementInstanceList) that contains all children of this [SVGElementInstance](#InterfaceSVGElementInstance) within the instance tree. If there are no children, this is an [SVGElementInstanceList](#InterfaceSVGElementInstanceList) containing no entries (i.e., an empty list).

**<a id="__svg__SVGElementInstance__firstChild"></a>**firstChild** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

The first child of this [SVGElementInstance](#InterfaceSVGElementInstance) within the instance tree. If there is no such [SVGElementInstance](#InterfaceSVGElementInstance), this returns null.

**<a id="__svg__SVGElementInstance__lastChild"></a>**lastChild** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

The last child of this [SVGElementInstance](#InterfaceSVGElementInstance) within the instance tree. If there is no such [SVGElementInstance](#InterfaceSVGElementInstance), this returns null.

**<a id="__svg__SVGElementInstance__previousSibling"></a>**previousSibling** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

The [SVGElementInstance](#InterfaceSVGElementInstance) immediately preceding this [SVGElementInstance](#InterfaceSVGElementInstance). If there is no such [SVGElementInstance](#InterfaceSVGElementInstance), this returns null.

**<a id="__svg__SVGElementInstance__nextSibling"></a>**nextSibling** (readonly [SVGElementInstance](#InterfaceSVGElementInstance))**
:

The [SVGElementInstance](#InterfaceSVGElementInstance) immediately following this [SVGElementInstance](#InterfaceSVGElementInstance). If there is no such [SVGElementInstance](#InterfaceSVGElementInstance), this returns null.

### <a id="InterfaceSVGElementInstanceList"></a>5.11.10 Interface SVGElementInstanceList

The [SVGElementInstanceList](#InterfaceSVGElementInstanceList) interface provides the abstraction of an ordered collection of [SVGElementInstance](#InterfaceSVGElementInstance) objects, without defining or constraining how this collection is implemented.

```
interface SVGElementInstanceList {

  readonly attribute unsigned long length;

  SVGElementInstance item(in unsigned long index);
};
```

**Attributes:**
:

**<a id="__svg__SVGElementInstanceList__length"></a>**length** (readonly unsigned long)**
:

The number of [SVGElementInstance](#InterfaceSVGElementInstance) objects in the list. The range of valid child indices is 0 to [length](#__svg__SVGElementInstanceList__length)-1 inclusive.

**Operations:**
:

**<a id="__svg__SVGElementInstanceList__item"></a>[SVGElementInstance](#InterfaceSVGElementInstance) **item**(in unsigned long *index*)**
:

Returns the *index*th item in the collection. If *index* is greater than or equal to the number of nodes in the list, this returns null.

**Parameters**
:

1.
unsigned long *index*
Index into the collection.

**Returns**
:

The [SVGElementInstance](#InterfaceSVGElementInstance) object at the *index*th position in the [SVGElementInstanceList](#InterfaceSVGElementInstanceList), or null if that is not a valid index.

### <a id="InterfaceSVGImageElement"></a>5.11.11 Interface SVGImageElement

The [SVGImageElement](#InterfaceSVGImageElement) interface corresponds to the [‘image’](#ImageElement) element.

```
interface SVGImageElement : SVGElement,
                            SVGURIReference,
                            SVGTests,
                            SVGLangSpace,
                            SVGExternalResourcesRequired,
                            SVGStylable,
                            SVGTransformable {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
  readonly attribute SVGAnimatedPreserveAspectRatio preserveAspectRatio;
};
```

**Attributes:**
:

**<a id="__svg__SVGImageElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#ImageElementXAttribute) on the given [‘image’](#ImageElement) element.

**<a id="__svg__SVGImageElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#ImageElementYAttribute) on the given [‘image’](#ImageElement) element.

**<a id="__svg__SVGImageElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#ImageElementWidthAttribute) on the given [‘image’](#ImageElement) element.

**<a id="__svg__SVGImageElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#ImageElementHeightAttribute) on the given [‘image’](#ImageElement) element.

**<a id="__svg__SVGImageElement__preserveAspectRatio"></a>**preserveAspectRatio** (readonly [SVGAnimatedPreserveAspectRatio](coords.md#InterfaceSVGAnimatedPreserveAspectRatio))**
:

Corresponds to attribute [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) on the given [‘image’](#ImageElement) element.

### <a id="InterfaceSVGSwitchElement"></a>5.11.12 Interface SVGSwitchElement

The [SVGSwitchElement](#InterfaceSVGSwitchElement) interface corresponds to the [‘switch’](#SwitchElement) element.

```
interface SVGSwitchElement : SVGElement,
                             SVGTests,
                             SVGLangSpace,
                             SVGExternalResourcesRequired,
                             SVGStylable,
                             SVGTransformable {
};
```

### <a id="InterfaceGetSVGDocument"></a>5.11.13 Interface GetSVGDocument

This interface provides access to an SVG document embedded by reference in another DOM-based language. The expectation is that the interface is implemented on DOM objects that allow such SVG document references, such as the DOM Element object that corresponds to an HTML ‘object’ element. Such DOM objects are often also required to implement the EmbeddingElement defined in the Window specification [[WINDOW](refs.md#ref-WINDOW)].

This interface is deprecated and may be dropped from future versions of the SVG specification. Authors are suggested to use the `contentDocument` attribute on the EmbeddingElement interface to obtain a referenced SVG document, if that interface is available.

```
interface GetSVGDocument {
  SVGDocument getSVGDocument();
};
```

**Operations:**
:

**<a id="__svg__GetSVGDocument__getSVGDocument"></a>[SVGDocument](#InterfaceSVGDocument) **getSVGDocument**()**
:

This method must return the Document object embedded content in an embedding element, or null if there is no document.

Note that this is equivalent to fetching the value of the `EmbeddingElement::contentDocument` attribute of the embedding element, if the EmbeddingElement interface is also implemented. The author is advised to check that the document element of the returned Document is indeed an [‘svg’](#SVGElement) element instead of assuming that that will always be the case.

**Returns**
:

The Document object for the referenced document, or null if there is no document.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](types.md) ⋅ [Next](styling.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
