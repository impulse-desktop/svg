<!-- https://www.w3.org/TR/SVG11/feature.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](propidx.md) ⋅ [Next](mimereg.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# Appendix O: Feature Strings

**This appendix is normative.**

## <a id="Introducation"></a>O.1 Introduction

The following are the feature strings for the [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute) attribute. These same feature strings apply to the hasFeature method call that is part of the [SVG DOM](https://www.w3.org/TR/SVG11/svgdom.html)'s support for the DOMImplementation interface defined in DOM Level 2 Core [[DOM2](refs.md#ref-DOM2)] (see [Feature strings for the **hasFeature** method call](https://www.w3.org/TR/SVG11/svgdom.html#FeatureStrings)). In some cases the feature strings map directly to a set of attributes, properties or elements, in others they represent some functionality of the user agent (that it is a dynamic viewer for example). Note that the format and naming for feature strings changed from SVG 1.0 [[SVG10](refs.md#ref-SVG10)] to SVG 1.1. The SVG 1.0 feature strings are listed below after the SVG 1.1 feature strings and User Agents should support all listed feature strings for compatibility reasons. However, the SVG 1.0 feature strings can be considered deprecated.

## <a id="SVG11FeatureStrings"></a>O.2 SVG 1.1 feature strings

<a id="SVG"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVG
**User Agent Supports:**
: At least one of the following (all of which are described subsequently): "http://www.w3.org/TR/SVG11/feature#SVG-static", "http://www.w3.org/TR/SVG11/feature#SVG-animation", "http://www.w3.org/TR/SVG11/feature#SVG-dynamic" or "http://www.w3.org/TR/SVG11/feature#SVGDOM". (Because the feature string "http://www.w3.org/TR/SVG11/feature#SVG" can be ambiguous in some circumstances, it is recommended that more specific feature strings be used.)
<a id="SVGDOM"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVGDOM
**User Agent Supports:**
: At least one of the following (all of which are described subsequently): "http://www.w3.org/TR/SVG11/feature#SVGDOM-static", "http://www.w3.org/TR/SVG11/feature#SVGDOM-animation" or "http://www.w3.org/TR/SVG11/feature#SVGDOM-dynamic". (Because the feature string "http://www.w3.org/TR/SVG11/feature#SVGDOM" can be ambiguous in some circumstances, it is recommended that more specific feature strings be used.)
<a id="SVG-static"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVG-static
**User Agent Supports:**
: The following features (described below)

- http://www.w3.org/TR/SVG11/feature#CoreAttribute
- http://www.w3.org/TR/SVG11/feature#Structure
- http://www.w3.org/TR/SVG11/feature#ContainerAttribute
- http://www.w3.org/TR/SVG11/feature#ConditionalProcessing
- http://www.w3.org/TR/SVG11/feature#Image
- http://www.w3.org/TR/SVG11/feature#Style
- http://www.w3.org/TR/SVG11/feature#ViewportAttribute
- http://www.w3.org/TR/SVG11/feature#Shape
- http://www.w3.org/TR/SVG11/feature#Text
- http://www.w3.org/TR/SVG11/feature#PaintAttribute
- http://www.w3.org/TR/SVG11/feature#OpacityAttribute
- http://www.w3.org/TR/SVG11/feature#GraphicsAttribute
- http://www.w3.org/TR/SVG11/feature#Marker
- http://www.w3.org/TR/SVG11/feature#ColorProfile
- http://www.w3.org/TR/SVG11/feature#Gradient
- http://www.w3.org/TR/SVG11/feature#Pattern
- http://www.w3.org/TR/SVG11/feature#Clip
- http://www.w3.org/TR/SVG11/feature#Mask
- http://www.w3.org/TR/SVG11/feature#Filter
- http://www.w3.org/TR/SVG11/feature#XlinkAttribute
- http://www.w3.org/TR/SVG11/feature#Font
- http://www.w3.org/TR/SVG11/feature#Extensibility

For SVG viewers, "http://www.w3.org/TR/SVG11/feature#SVG-static" indicates that the viewer can process and render successfully all of the language features corresponding to the feature strings listed above.
<a id="SVGDOM-static"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVGDOM-static
**User Agent Supports:**
: All of the DOM interfaces and methods that correspond to the language features for "http://www.w3.org/TR/SVG11/feature#SVG-static".
<a id="SVG-animation"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVG-animation
**User Agent Supports:**
: All of the language features from "http://www.w3.org/TR/SVG11/feature#SVG-static" plus the feature "http://www.w3.org/TR/SVG11/feature#Animation". For SVG viewers running on media capable of rendering time-based material, such as displays, "http://www.w3.org/TR/SVG11/feature#SVG-animation" indicates that the viewer can process and render successfully all of the corresponding language features.
<a id="SVGDOM-animation"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVGDOM-animation
**User Agent Supports:**
: All of the DOM interfaces and methods that correspond to the language features for "http://www.w3.org/TR/SVG11/feature#SVG-animation".
<a id="SVG-dynamic"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVG-dynamic
**User Agent Supports:**
: All of the language features from "http://www.w3.org/TR/SVG11/feature#SVG-animation" plus the following features:

- http://www.w3.org/TR/SVG11/feature#Hyperlinking
- http://www.w3.org/TR/SVG11/feature#Scripting
- http://www.w3.org/TR/SVG11/feature#View
- http://www.w3.org/TR/SVG11/feature#Cursor
- http://www.w3.org/TR/SVG11/feature#GraphicalEventsAttribute
- http://www.w3.org/TR/SVG11/feature#DocumentEventsAttribute
- http://www.w3.org/TR/SVG11/feature#AnimationEventsAttribute

For SVG viewers running on media capable of rendering time-based material, such as displays, "http://www.w3.org/TR/SVG11/feature#SVG-dynamic" indicates that the viewer can process and render successfully all of the corresponding language features.
<a id="SVGDOM-dynamic"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#SVGDOM-dynamic
**User Agent Supports:**
: All of the DOM interfaces and methods that correspond to the language features for "http://www.w3.org/TR/SVG11/feature#SVG-dynamic".
<a id="CoreAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#CoreAttribute
**User Agent Supports:**
: the [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute) and [‘xml:space’](struct.md#XMLSpaceAttribute) attributes
<a id="Structure"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Structure
**User Agent Supports:**
: [‘svg’](struct.md#SVGElement), [‘g’](struct.md#GElement), [‘defs’](struct.md#DefsElement), [‘desc’](struct.md#DescElement), [‘title’](struct.md#TitleElement), [‘metadata’](metadata.md#MetadataElement), [‘symbol’](struct.md#SymbolElement) and [‘use’](struct.md#UseElement) elements
<a id="BasicStructure"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicStructure
**User Agent Supports:**
: [‘svg’](struct.md#SVGElement), [‘g’](struct.md#GElement), [‘defs’](struct.md#DefsElement), [‘desc’](struct.md#DescElement), [‘title’](struct.md#TitleElement), [‘metadata’](metadata.md#MetadataElement) and [‘use’](struct.md#UseElement) elements
<a id="ContainerAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#ContainerAttribute
**User Agent Supports:**
: the [‘enable-background’](filters.md#EnableBackgroundProperty) property
<a id="ConditionalProcessing"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#ConditionalProcessing
**User Agent Supports:**
: the [‘switch’](struct.md#SwitchElement) element, and the [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute) and [‘systemLanguage’](struct.md#SystemLanguageAttribute) attributes
<a id="Image"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Image
**User Agent Supports:**
: the [‘image’](struct.md#ImageElement) element
<a id="Style"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Style
**User Agent Supports:**
: the [‘style’](styling.md#StyleElement) element
<a id="ViewportAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#ViewportAttribute
**User Agent Supports:**
: the [‘clip’](masking.md#ClipProperty) and [‘overflow’](masking.md#OverflowProperty) properties
<a id="Shape"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Shape
**User Agent Supports:**
: the [‘rect’](shapes.md#RectElement), [‘circle’](shapes.md#CircleElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement), [‘polygon’](shapes.md#PolygonElement), [‘ellipse’](shapes.md#EllipseElement) and [‘path’](paths.md#PathElement) elements
<a id="Text"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Text
**User Agent Supports:**
: the [‘text’](text.md#TextElement), [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement), [‘textPath’](text.md#TextPathElement), [‘altGlyph’](text.md#AltGlyphElement), [‘altGlyphDef’](text.md#AltGlyphDefElement), [‘altGlyphItem’](text.md#AltGlyphItemElement) and [‘glyphRef’](text.md#GlyphRefElement) elements
<a id="BasicText"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicText
**User Agent Supports:**
: the [‘text’](text.md#TextElement) element
<a id="PaintAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#PaintAttribute
**User Agent Supports:**
: the [‘color’](color.md#ColorProperty), [‘fill’](painting.md#FillProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty) and [‘color-rendering’](painting.md#ColorRenderingProperty) properties
<a id="BasicPaintAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicPaintAttribute
**User Agent Supports:**
: the [‘color’](color.md#ColorProperty), [‘fill’](painting.md#FillProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-width’](painting.md#StrokeWidthProperty) and [‘color-rendering’](painting.md#ColorRenderingProperty) properties
<a id="OpacityAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#OpacityAttribute
**User Agent Supports:**
: the [‘opacity’](masking.md#OpacityProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty) and [‘fill-opacity’](painting.md#FillOpacityProperty) properties
<a id="GraphicsAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#GraphicsAttribute
**User Agent Supports:**
: the [‘display’](painting.md#DisplayProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘text-rendering’](painting.md#TextRenderingProperty) and [‘visibility’](painting.md#VisibilityProperty) properties
<a id="BasicGraphicsAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicGraphicsAttribute
**User Agent Supports:**
: the [‘display’](painting.md#DisplayProperty) and [‘visibility’](painting.md#VisibilityProperty) properties
<a id="Marker"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Marker
**User Agent Supports:**
: the [‘marker’](painting.md#MarkerElement) element
<a id="ColorProfile"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#ColorProfile
**User Agent Supports:**
: the [‘color-profile’](color.md#ColorProfileElement) element
<a id="Gradient"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Gradient
**User Agent Supports:**
: the [‘linearGradient’](pservers.md#LinearGradientElement), [‘radialGradient’](pservers.md#RadialGradientElement) and [‘stop’](pservers.md#StopElement) elements
<a id="Pattern"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Pattern
**User Agent Supports:**
: the [‘pattern’](pservers.md#PatternElement) element
<a id="Clip"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Clip
**User Agent Supports:**
: the [‘clipPath’](masking.md#ClipPathElement) element and the [‘clip-path’](masking.md#ClipPathProperty) and [‘clip-rule’](masking.md#ClipRuleProperty) properties
<a id="BasicClip"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicClip
**User Agent Supports:**
: the [‘clipPath’](masking.md#ClipPathElement) element and the [‘clip-path’](masking.md#ClipPathProperty) property
<a id="Mask"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Mask
**User Agent Supports:**
: the [‘mask’](masking.md#MaskElement) element
<a id="Filter"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Filter
**User Agent Supports:**
: the [‘filter’](filters.md#FilterElement), [‘feBlend’](filters.md#feBlendElement), [‘feColorMatrix’](filters.md#feColorMatrixElement), [‘feComponentTransfer’](filters.md#feComponentTransferElement), [‘feComposite’](filters.md#feCompositeElement), [‘feConvolveMatrix’](filters.md#feConvolveMatrixElement), [‘feDiffuseLighting’](filters.md#feDiffuseLightingElement), [‘feDisplacementMap’](filters.md#feDisplacementMapElement), [‘feFlood’](filters.md#feFloodElement), [‘feGaussianBlur’](filters.md#feGaussianBlurElement), [‘feImage’](filters.md#feImageElement), [‘feMerge’](filters.md#feMergeElement), [‘feMergeNode’](filters.md#feMergeNodeElement), [‘feMorphology’](filters.md#feMorphologyElement), [‘feOffset’](filters.md#feOffsetElement), [‘feSpecularLighting’](filters.md#feSpecularLightingElement), [‘feTile’](filters.md#feTileElement), [‘feDistantLight’](filters.md#feDistantLightElement), [‘fePointLight’](filters.md#fePointLightElement), [‘feSpotLight’](filters.md#feSpotLightElement), [‘feFuncR’](filters.md#feFuncRElement), [‘feFuncG’](filters.md#feFuncGElement), [‘feFuncB’](filters.md#feFuncBElement) and [‘feFuncA’](filters.md#feFuncAElement) elements
<a id="BasicFilter"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicFilter
**User Agent Supports:**
: the [‘filter’](filters.md#FilterElement), [‘feBlend’](filters.md#feBlendElement), [‘feColorMatrix’](filters.md#feColorMatrixElement), [‘feComponentTransfer’](filters.md#feComponentTransferElement), [‘feComposite’](filters.md#feCompositeElement), [‘feFlood’](filters.md#feFloodElement), [‘feGaussianBlur’](filters.md#feGaussianBlurElement), [‘feImage’](filters.md#feImageElement), [‘feMerge’](filters.md#feMergeElement), [‘feMergeNode’](filters.md#feMergeNodeElement), [‘feOffset’](filters.md#feOffsetElement), [‘feTile’](filters.md#feTileElement), [‘feFuncR’](filters.md#feFuncRElement), [‘feFuncG’](filters.md#feFuncGElement), [‘feFuncB’](filters.md#feFuncBElement) and [‘feFuncA’](filters.md#feFuncAElement) elements
<a id="DocumentEventsAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#DocumentEventsAttribute
**User Agent Supports:**
: the [‘onunload’](https://www.w3.org/TR/SVG11/script.html#OnUnloadEventAttribute), [‘onabort’](https://www.w3.org/TR/SVG11/script.html#OnAbortEventAttribute), [‘onerror’](https://www.w3.org/TR/SVG11/script.html#OnErrorEventAttribute), [‘onresize’](https://www.w3.org/TR/SVG11/script.html#OnResizeEventAttribute), [‘onscroll’](https://www.w3.org/TR/SVG11/script.html#OnScrollEventAttribute) and [‘onzoom’](https://www.w3.org/TR/SVG11/script.html#OnZoomEventAttribute) attributes
<a id="GraphicalEventsAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#GraphicalEventsAttribute
**User Agent Supports:**
: the [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute) and [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute) attributes
<a id="AnimationEventsAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#AnimationEventsAttribute
**User Agent Supports:**
: the [‘onbegin’](https://www.w3.org/TR/SVG11/script.html#OnBeginEventAttribute), [‘onend’](https://www.w3.org/TR/SVG11/script.html#OnEndEventAttribute), [‘onrepeat’](https://www.w3.org/TR/SVG11/script.html#OnRepeatEventAttribute) and [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute) attributes
<a id="Cursor"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Cursor
**User Agent Supports:**
: the [‘cursor’](interact.md#CursorElement) element
<a id="Hyperlinking"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Hyperlinking
**User Agent Supports:**
: the [‘a’](linking.md#AElement) element
<a id="XlinkAttribute"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#XlinkAttribute
**User Agent Supports:**
: the [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:href’](animate.md#HrefAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute) and [‘xlink:actuate’](linking.md#XLinkActuateAttribute) attributes
<a id="ExternalResourcesRequired"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#ExternalResourcesRequired
**User Agent Supports:**
: the [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute) attribute
<a id="View"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#View
**User Agent Supports:**
: the [‘view’](linking.md#ViewElement) element
<a id="Script"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Script
**User Agent Supports:**
: the [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement) element
<a id="Animation"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Animation
**User Agent Supports:**
: the [‘animate’](animate.md#AnimateElement), [‘set’](animate.md#SetElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘animateColor’](animate.md#AnimateColorElement) and [‘mpath’](animate.md#MPathElement) elements
<a id="Font"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Font
**User Agent Supports:**
: the [‘font’](fonts.md#FontElement), [‘font-face’](fonts.md#FontFaceElement), [‘glyph’](fonts.md#GlyphElement), [‘missing-glyph’](fonts.md#MissingGlyphElement), [‘hkern’](fonts.md#HKernElement), [‘vkern’](fonts.md#VKernElement), [‘font-face-src’](fonts.md#FontFaceSrcElement), [‘font-face-uri’](fonts.md#FontFaceURIElement), [‘font-face-format’](fonts.md#FontFaceFormatElement) and [‘font-face-name’](fonts.md#FontFaceNameElement) elements
<a id="BasicFont"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#BasicFont
**User Agent Supports:**
: the [‘font’](fonts.md#FontElement), [‘font-face’](fonts.md#FontFaceElement), [‘glyph’](fonts.md#GlyphElement), [‘missing-glyph’](fonts.md#MissingGlyphElement), [‘hkern’](fonts.md#HKernElement), [‘font-face-src’](fonts.md#FontFaceSrcElement) and [‘font-face-name’](fonts.md#FontFaceNameElement) elements
<a id="Extensibility"></a>

**Feature String:**
: http://www.w3.org/TR/SVG11/feature#Extensibility
**User Agent Supports:**
: the [‘foreignObject’](extend.md#ForeignObjectElement) element
## <a id="SVG10FeatureStrings"></a>O.3 SVG 1.0 feature strings

All SVG 1.0 [[SVG10](refs.md#ref-SVG10)] feature strings referring to language capabilities begin with **"org.w3c.svg"**. All SVG 1.0 feature strings referring to [SVG DOM](https://www.w3.org/TR/SVG11/svgdom.html) capabilities begin with **"org.w3c.dom.svg"**.

- The feature string **"org.w3c.svg"** indicates that the user agent supports at least one of the following (all of which are described subsequently): **"org.w3c.svg.static"**, **"org.w3c.svg.animation"**, **"org.w3c.svg.dynamic"** or **"org.w3c.dom.svg"**. (Because the feature string **"org.w3c.svg"** can be ambiguous in some circumstances, it is recommended that more specific feature strings be used.)
- The feature string **"org.w3c.dom.svg"** indicates that the user agent supports at least one of the following (all of which are described subsequently): **"org.w3c.dom.svg.static"**, **"org.w3c.dom.svg.animation"** or **"org.w3c.dom.svg.dynamic"**. (Because the feature string **"org.w3c.dom.svg"** can be ambiguous in some circumstances, it is recommended that more specific feature strings be used.)
- The feature string **"org.w3c.svg.static"** indicates the availability of all of the language capabilities defined in:
  - [Basic Data Types and Interfaces](types.md)
  - [Document Structure](struct.md)
  - [Styling](styling.md)
  - [Coordinate Systems, Transformations and Units](coords.md)
  - [Paths](paths.md)
  - [Basic Shapes](shapes.md)
  - [Text](text.md)
  - [Painting: Filling, Stroking and Marker Symbols](painting.md)
  - [Color](color.md)
  - [Gradients and Patterns](pservers.md)
  - [Clipping, Masking and Compositing](masking.md)
  - [Filter Effects](filters.md)
  - [Fonts](fonts.md)
  - The [‘switch’](struct.md#SwitchElement) element
  - The [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute) attribute
  - The [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute) attribute
  - The [‘systemLanguage’](struct.md#SystemLanguageAttribute) attribute
For SVG viewers, **"org.w3c.svg.static"** indicates that the viewer can process and render successfully all of the language features listed above.
- The feature string **"org.w3c.dom.svg.static"** indicates the availability of all of the DOM interfaces and methods that correspond to the language features for **"org.w3c.svg.static"**.
- The feature string **"org.w3c.svg.animation"** includes all of the language capabilities defined for **"org.w3c.svg.static"** plus the availability of all of the language capabilities defined in [Animation](animate.md). For SVG viewers running on media capable of rendering time-based material, such as displays, **"org.w3c.svg.animation"** indicates that the viewer can process and render successfully all of the corresponding language features.
- The feature string **"org.w3c.dom.svg.animation"** corresponds to the availability of DOM interfaces and methods that correspond to the language features for **"org.w3c.svg.animation"**.
- The feature string **"org.w3c.svg.dynamic"** includes all of the language capabilities defined for **"org.w3c.svg.animation"** plus the availability of all of the language capabilities defined in [Relationship with DOM2 events](https://www.w3.org/TR/SVG11/svgdom.html#RelationshipWithDOM2Events), [Linking](linking.md) and [Interactivity](interact.md) and [Scripting](https://www.w3.org/TR/SVG11/script.html). For SVG viewers running on media capable of rendering time-based material, such as displays, **"org.w3c.svg.dynamic"** indicates that the viewer can process and render successfully all of the corresponding language features.
- The feature string **"org.w3c.dom.svg.dynamic"** corresponds to the availability of DOM interfaces and methods that correspond to the language features for **"org.w3c.svg.dynamic"**.
- The feature string **"org.w3c.svg.all"** corresponds to the availability of all of the language capabilities defined in this specification.
- The feature string **"org.w3c.dom.svg.all"** corresponds to the availability of all of the DOM interfaces defined in this specification.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](propidx.md) ⋅ [Next](mimereg.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
