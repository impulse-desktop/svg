<!-- https://www.w3.org/TR/SVG11/fonts.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](animate.md) ⋅ [Next](metadata.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 20 Fonts

## <a id="Introduction"></a>20.1 Introduction

Reliable delivery of fonts is a requirement for SVG. Designers need to create SVG content with arbitrary fonts and know that the same graphical result will appear when the content is viewed by all end users, even when end users do not have the necessary fonts installed on their computers. This parallels the print world, where the designer uses a given font when authoring a drawing for print, and the graphical content appears exactly the same in the printed version as it appeared on the designer's authoring system.

SVG utilizes the WebFonts facility defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.1) as a key mechanism for reliable delivery of font data to end users. In a common scenario, SVG authoring applications generate compressed, subsetted WebFonts for all text elements used by a given SVG document fragment. Typically, the WebFonts are saved in a location relative to the referencing document.

One disadvantage to the WebFont facility to date is that specifications such as CSS2 do not require support of particular font formats. The result is that different implementations support different Web font formats, thereby making it difficult for Web site creators to post a single Web site using WebFonts that work across all user agents.

To provide a common font format for SVG that is guaranteed to be supported by all [conforming SVG viewers](conform.md#ConformingSVGViewers), SVG provides a facility to define fonts in SVG. This facility is called SVG fonts.

<a id="SVGFonts"></a>SVG fonts can improve the semantic richness of graphics that represent text. For example, many company logos consist of the company name drawn artistically. In some cases, [accessibility](access.md) may be enhanced by expressing the logo as a series of glyphs in an SVG font and then rendering the logo as a [‘text’](text.md#TextElement) element which references this font.

## <a id="SVGFontsOverview"></a>20.2 Overview of SVG fonts

An SVG font is a font defined using SVG's [‘font’](#FontElement) element.

The purpose of SVG fonts is to allow for delivery of glyph outlines in display-only environments. SVG fonts that accompany Web pages must be supported only in browsing and viewing situations. Graphics editing applications or file translation tools must not attempt to convert SVG fonts into system fonts. The intent is that SVG files be interchangeable between two content creators, but not the SVG fonts that might accompany these SVG files. Instead, each content creator will need to license the given font before being able to successfully edit the SVG file. The [‘font-face-name’](#FontFaceNameElement) element indicates the name of licensed font to use for editing.

SVG fonts contain unhinted font outlines. Because of this, on many implementations there will be limitations regarding the quality and legibility of text in small font sizes. For increased quality and legibility in small font sizes, content creators may want to use an alternate font technology, such as fonts that ship with operating systems or an alternate WebFont format.

Because SVG fonts are expressed using SVG elements and attributes, in some cases the SVG font will take up more space than if the font were expressed in a different WebFont format which was especially designed for compact expression of font data. For the fastest delivery of Web pages, content creators may want to use an alternate font technology.

A key value of SVG fonts is guaranteed availability in SVG user agents. In some situations, it might be appropriate for an SVG font to be the first choice for rendering some text. In other situations, the SVG font might be an alternate, back-up font in case the first choice font (perhaps a hinted system font) is not available to a given user.

The characteristics and attributes of SVG fonts correspond closely to the font characteristics and parameters described in the Fonts chapter of the Cascading Style Sheets (CSS) level 2 specification ([CSS2], chapter 15). In this model, various font metrics, such as advance values and baseline locations, and the glyph outlines themselves, are expressed in units that are relative to an abstract square whose height is the intended distance between lines of type in the same type size. This square is called the em square and it is the design grid on which the glyph outlines are defined. The value of the [‘units-per-em’](#FontFaceElementUnitsPerEmAttribute) attribute on the [‘font-face’](#FontFaceElement) element specifies how many units the em square is divided into. Common values for other font types are, for example, 250 (Intellifont), 1000 (Type 1) and 2048 (TrueType, TrueType GX and Open-Type). Unlike standard graphics in SVG, where the initial coordinate system has the y-axis pointing downward (see [The initial coordinate system](coords.md#InitialCoordinateSystem)), the design grid for SVG fonts, along with the initial coordinate system for the glyphs, has the y-axis pointing upward for consistency with accepted industry practice for many popular font formats.

SVG fonts and their associated glyphs do not specify bounding box information. Because the glyph outlines are expressed as SVG graphics elements, the implementation has the option to render the glyphs either using standard graphics calls or by using special-purpose font rendering technology, in which case any necessary maximum bounding box and overhang calculations can be performed from analysis of the graphics elements contained within the glyph outlines.

An SVG font can be either embedded within the same document that uses the font or saved as part of an external resource.

Here is an example of how you might embed an SVG font inside of an SVG document.

```
<?xml version="1.0" standalone="yes"?>
<svg width="400px" height="300px" version="1.1"
  xmlns = 'http://www.w3.org/2000/svg'>
  <defs>
    <font id="Font1" horiz-adv-x="1000">
      <font-face font-family="Super Sans" font-weight="bold" font-style="normal"
          units-per-em="1000" cap-height="600" x-height="400"
          ascent="700" descent="300"
          alphabetic="0" mathematical="350" ideographic="400" hanging="500">
        <font-face-src>
          <font-face-name name="Super Sans Bold"/>
        </font-face-src>
      </font-face>
      <missing-glyph><path d="M0,0h200v200h-200z"/></missing-glyph>
      <glyph unicode="!" horiz-adv-x="300"><!-- Outline of exclam. pt. glyph --></glyph>
      <glyph unicode="@"><!-- Outline of @ glyph --></glyph>
      <!-- more glyphs -->
    </font>
  </defs>
  <text x="100" y="100" 
           style="font-family: 'Super Sans', Helvetica, sans-serif;
                  font-weight: bold; font-style: normal">Text 
    using embedded font</text>
</svg>
```

Here is an example of how you might use the CSS @font-face facility ([[CSS2](refs.md#ref-CSS2)], section 15.3.1) to reference an SVG font which is saved in an external file. First referenced SVG font file:

```
<?xml version="1.0" standalone="yes"?>
<svg width="100%" height="100%" version="1.1"
 xmlns = 'http://www.w3.org/2000/svg'>
  <defs>
    <font id="Font2" horiz-adv-x="1000">
      <font-face font-family="Super Sans" font-weight="normal" font-style="italic"
          units-per-em="1000" cap-height="600" x-height="400"
          ascent="700" descent="300"
          alphabetic="0" mathematical="350" ideographic="400" hanging="500">
        <font-face-src>
          <font-face-name name="Super Sans Italic"/>
        </font-face-src>
      </font-face>
      <missing-glyph><path d="M0,0h200v200h-200z"/></missing-glyph>
      <glyph unicode="!" horiz-adv-x="300"><!-- Outline of exclam. pt. glyph --></glyph>
      <glyph unicode="@"><!-- Outline of @ glyph --></glyph>
      <!-- more glyphs -->
    </font>
  </defs>
</svg>
```

The SVG file which uses/references the above SVG font

```
<?xml version="1.0" standalone="yes"?>
<svg width="400px" height="300px" version="1.1"
 xmlns = 'http://www.w3.org/2000/svg'>  
  <defs>
    <style type="text/css">
      <![CDATA[
        @font-face {
          font-family: 'Super Sans'; 
          font-weight: normal;
          font-style: italic;
          src: url("myfont.svg#Font2") format("svg")
        }
      ]]>
   </style>
  </defs>
  <text x="100" y="100"
           style="font-family: 'Super Sans'; font-weight:normal;
                  font-style: italic">Text using referenced font</text>
</svg>
```

## <a id="FontElement"></a>20.3 The ‘font’ element

The [‘font’](#FontElement) element defines an SVG font.

‘font’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [‘font-face’](#FontFaceElement)
- [‘glyph’](#GlyphElement)
- [‘hkern’](#HKernElement)
- [‘missing-glyph’](#MissingGlyphElement)
- [‘vkern’](#VKernElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘horiz-origin-x’](#FontElementHorizOriginXAttribute)
- [‘horiz-origin-y’](#FontElementHorizOriginYAttribute)
- [‘horiz-adv-x’](#FontElementHorizAdvXAttribute)
- [‘vert-origin-x’](#FontElementVertOriginXAttribute)
- [‘vert-origin-y’](#FontElementVertOriginYAttribute)
- [‘vert-adv-y’](#FontElementVertAdvYAttribute)

**DOM Interfaces:**
:

- [SVGFontElement](#InterfaceSVGFontElement)

*Attribute definitions:*

**<a id="FontElementHorizOriginXAttribute"></a>horiz-origin-x = "<number>"**
: The X-coordinate in the font coordinate system of the origin of a glyph to be used when drawing horizontally oriented text. (Note that the origin applies to all glyphs in the font.) If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontElementHorizOriginYAttribute"></a>horiz-origin-y = "<number>"**
: The Y-coordinate in the font coordinate system of the origin of a glyph to be used when drawing horizontally oriented text. (Note that the origin applies to all glyphs in the font.) If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontElementHorizAdvXAttribute"></a>horiz-adv-x = "<number>"**
: The default horizontal advance after rendering a glyph in horizontal orientation. Glyph widths are required to be non-negative, even if the glyph is typically rendered right-to-left, as in Hebrew and Arabic scripts. [Animatable](animate.md#Animatable): no.
**<a id="FontElementVertOriginXAttribute"></a>vert-origin-x = "<number>"**
: The default X-coordinate in the font coordinate system of the origin of a glyph to be used when drawing vertically oriented text. If the attribute is not specified, the effect is as if the attribute were set to half of the effective value of attribute [‘horiz-adv-x’](#FontElementHorizAdvXAttribute). [Animatable](animate.md#Animatable): no.
**<a id="FontElementVertOriginYAttribute"></a>vert-origin-y = "<number>"**
: The default Y-coordinate in the font coordinate system of the origin of a glyph to be used when drawing vertically oriented text. If the attribute is not specified, the effect is as if the attribute were set to the position specified by the font's [‘ascent’](#FontFaceElementAscentAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="FontElementVertAdvYAttribute"></a>vert-adv-y = "<number>"**
: The default vertical advance after rendering a glyph in vertical orientation. If the attribute is not specified, the effect is as if a value equivalent of one *em* were specified (see [‘units-per-em’](#FontFaceElementUnitsPerEmAttribute)). [Animatable](animate.md#Animatable): no.
Each [‘font’](#FontElement) element must have a [‘font-face’](#FontFaceElement) child element which describes various characteristics of the font.

## <a id="GlyphElement"></a>20.4 The ‘glyph’ element

The [‘glyph’](#GlyphElement) element defines the graphics for a given glyph. The coordinate system for the glyph is defined by the various attributes in the [‘font’](#FontElement) element.

The graphics that make up the [‘glyph’](#GlyphElement) can be a single [path data](paths.md#PathData) specification within the [‘d’](#GlyphElementDAttribute) attribute, arbitrary SVG as content within the [‘glyph’](#GlyphElement), or both. These two alternatives are processed differently (see below).

‘glyph’

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
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](#FontElement)
- [‘font-face’](#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](struct.md#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](struct.md#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘d’](#GlyphElementDAttribute)
- [‘horiz-adv-x’](#GlyphElementHorizAdvXAttribute)
- [‘vert-origin-x’](#GlyphElementVertOriginXAttribute)
- [‘vert-origin-y’](#GlyphElementVertOriginYAttribute)
- [‘vert-adv-y’](#GlyphElementVertAdvYAttribute)
- [‘unicode’](#GlyphElementUnicodeAttribute)
- [‘glyph-name’](#GlyphElementGlyphNameAttribute)
- [‘orientation’](#GlyphElementOrientationAttribute)
- [‘arabic-form’](#GlyphElementArabicFormAttribute)
- [‘lang’](#GlyphElementLangAttribute)

**DOM Interfaces:**
:

- [SVGGlyphElement](#InterfaceSVGGlyphElement)

*Attribute definitions:*

**<a id="GlyphElementUnicodeAttribute"></a>unicode = "<string>"**
: One or more Unicode characters indicating the sequence of Unicode characters which corresponds to this glyph. If a character is provided, then this glyph corresponds to the given Unicode character. If multiple characters are provided, then this glyph corresponds to the given sequence of Unicode characters. One use of a sequence of characters is ligatures. For example, if unicode="ffl", then the given glyph will be used to render the sequence of characters "f", "f", and "l". It is often useful to refer to characters using XML character references expressed in hexadecimal notation or decimal notation. For example, unicode="ffl" could be expressed as XML character references in hexadecimal notation as unicode="&#x66;&#x66;&#x6c;" or in decimal notation as unicode="&#102;&#102;&#108;". The [‘unicode’](#GlyphElementUnicodeAttribute) attribute contributes to the process for deciding which glyph(s) are used to represent which character(s). See [glyph selection rules](#GlyphSelectionRules). If the [‘unicode’](#GlyphElementUnicodeAttribute) attribute is not provided for a given [‘glyph’](#GlyphElement), then the only way to use this glyph is via an [‘altGlyph’](text.md#AltGlyphElement) reference. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementGlyphNameAttribute"></a>glyph-name = "<name> [, <name> ]* "**
: A name for the glyph. It is recommended that glyph names be unique within a font. The glyph names can be used in situations where Unicode character numbers do not provide sufficient information to access the correct glyph, such as when there are multiple glyphs per Unicode character. The glyph names can be referenced in [kerning](#KernElements) definitions. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementDAttribute"></a>d = "path data"**
: The definition of the outline of a glyph, using the same syntax as for the [‘d’](paths.md#DAttribute) attribute on a [‘path’](paths.md#PathElement) element. See [Path data](paths.md#PathData). See below for a discussion of this attribute. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementOrientationAttribute"></a>orientation = "h | v"**
: Indicates that the given glyph is only to be used for a particular inline-progression-direction (i.e., horizontal or vertical). If the attribute is not specified, then the glyph can be used in all cases (i.e., both horizontal and vertical inline-progression-direction). [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementArabicFormAttribute"></a>arabic-form = "initial | medial | terminal | isolated"**
: For Arabic glyphs, indicates which of the four possible forms this glyph represents. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementLangAttribute"></a>lang = "%LanguageCodes;"**
: The attribute value is a comma-separated list of language names as defined in BCP 47 [[BCP47](refs.md#ref-BCP47)]. The glyph can be used if the [‘xml:lang’](struct.md#XMLLangAttribute) attribute exactly matches one of the languages given in the value of this parameter, or if the [‘xml:lang’](struct.md#XMLLangAttribute) attribute exactly equals a prefix of one of the languages given in the value of this parameter such that the first tag character following the prefix is "-". [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementHorizAdvXAttribute"></a>horiz-adv-x = "<number>"**
: The horizontal advance after rendering the glyph in horizontal orientation. If the attribute is not specified, the effect is as if the attribute were set to the value of the font's [‘horiz-adv-x’](#FontElementHorizAdvXAttribute) attribute. Glyph widths are required to be non-negative, even if the glyph is typically rendered right-to-left, as in Hebrew and Arabic scripts. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementVertOriginXAttribute"></a>vert-origin-x = "<number>"**
: The X-coordinate in the font coordinate system of the origin of the glyph to be used when drawing vertically oriented text. If the attribute is not specified, the effect is as if the attribute were set to the value of the font's [‘vert-origin-x’](#FontElementVertOriginXAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementVertOriginYAttribute"></a>vert-origin-y = "<number>"**
: The Y-coordinate in the font coordinate system of the origin of a glyph to be used when drawing vertically oriented text. If the attribute is not specified, the effect is as if the attribute were set to the value of the font's [‘vert-origin-y’](#FontElementVertOriginYAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="GlyphElementVertAdvYAttribute"></a>vert-adv-y = "<number>"**
: The vertical advance after rendering a glyph in vertical orientation. If the attribute is not specified, the effect is as if the attribute were set to the value of the font's [‘vert-adv-y’](#FontElementVertAdvYAttribute) attribute. [Animatable](animate.md#Animatable): no.
The graphics for the [‘glyph’](#GlyphElement) can be specified using either the [‘d’](#GlyphElementDAttribute) attribute or arbitrary SVG as content within the [‘glyph’](#GlyphElement).

If the [‘d’](#GlyphElementDAttribute) attribute is specified, then the path data within this attribute is processed as follows:

- Any relative coordinates within the path data specification are converted into equivalent absolute coordinates
- Each of these absolute coordinates is transformed from the font coordinate system into the [‘text’](text.md#TextElement) element's current coordinate system such that the origin of the font coordinate system is properly positioned and rotated to align with the [current text position](text.md#CurrentTextPosition) and orientation for the glyph, and scaled so that the correct [‘font-size’](text.md#FontSizeProperty) is achieved.
- The resulting, transformed path specification is rendered as if it were a [‘path’](paths.md#PathElement) element, using the styling properties that apply to the characters which correspond to the given glyph, and ignoring any styling properties specified on the [‘font’](#FontElement) element or the [‘glyph’](#GlyphElement) element.

If the [‘glyph’](#GlyphElement) has child elements, then those child elements are rendered in a manner similar to how the [‘use’](struct.md#UseElement) element renders a referenced symbol. The rendering effect is as if the contents of the referenced [‘glyph’](#GlyphElement) element were deeply cloned into a separate non-exposed DOM tree. Because the cloned DOM tree is non-exposed, the SVG DOM does not show the cloned instance.

For user agents that support [Styling with CSS](styling.md#StylingWithCSS), the conceptual deep cloning of the referenced [‘glyph’](#GlyphElement) element into a non-exposed DOM tree also copies any property values resulting from the CSS cascade ([[CSS2](refs.md#ref-CSS2)], chapter 6) on the referenced [‘glyph’](#GlyphElement) and its contents, and also applies any property values on the [‘font’](#FontElement) element. CSS2 selectors can be applied to the original (i.e., referenced) elements because they are part of the formal document structure. CSS2 selectors cannot be applied to the (conceptually) cloned DOM tree because its contents are not part of the formal document structure.

Property inheritance, however, works as if the referenced [‘glyph’](#GlyphElement) had been textually included as a deeply cloned child within the document tree. The referenced [‘glyph’](#GlyphElement) inherits properties from the element that contains the characters that correspond to the [‘glyph’](#GlyphElement). The [‘glyph’](#GlyphElement) does not inherit properties from the [‘font’](#FontElement) element's original parents.

In the generated content, for each instance of a given [‘glyph’](#GlyphElement), a [‘g’](struct.md#GElement) is created which carries with it all property values resulting from the CSS cascade on the [‘font’](#FontElement) element for the referenced [‘glyph’](#GlyphElement). Within this [‘g’](struct.md#GElement) is another [‘g’](struct.md#GElement) which carries with it all property values resulting from the CSS cascade on the [‘glyph’](#GlyphElement) element. The original contents of the [‘glyph’](#GlyphElement) element are deep-cloned within the inner [‘g’](struct.md#GElement) element.

If the [‘glyph’](#GlyphElement) has both a [‘d’](#GlyphElementDAttribute) attribute and child elements, the [‘d’](#GlyphElementDAttribute) attribute is rendered first, and then the child elements.

In general, the [‘d’](#GlyphElementDAttribute) attribute renders in the same manner as system fonts. For example, a dashed pattern will usually look the same if applied to a system font or to an SVG font which defines its glyphs using the [‘d’](#GlyphElementDAttribute) attribute. Many implementations will be able to render glyphs defined with the [‘d’](#GlyphElementDAttribute) attribute quickly and will be able to use a font cache for further performance gains.

Defining a glyph by including child elements within the [‘glyph’](#GlyphElement) gives greater flexibility but more complexity. Different fill and stroke techniques can be used on different parts of the glyphs. For example, the base of an "i" could be red, and the dot could be blue. This approach has an inherent complexity with units. Any properties specified on a text elements which represents a length, such as the [‘stroke-width’](painting.md#StrokeWidthProperty) property, might produce surprising results since the length value will be processed in the coordinate system of the glyph.

## <a id="MissingGlyphElement"></a>20.5 The ‘missing-glyph’ element

The [‘missing-glyph’](#MissingGlyphElement) element defines the graphics to use if there is an attempt to draw a glyph from a given font and the given glyph has not been defined. The attributes on the [‘missing-glyph’](#MissingGlyphElement) element have the same meaning as the corresponding attributes on the [‘glyph’](#GlyphElement) element.

‘missing-glyph’

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
- [‘clipPath’](masking.md#ClipPathElement)
- [‘color-profile’](color.md#ColorProfileElement)
- [‘cursor’](interact.md#CursorElement)
- [‘filter’](filters.md#FilterElement)
- [‘font’](#FontElement)
- [‘font-face’](#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](struct.md#ImageElement)
- [‘marker’](painting.md#MarkerElement)
- [‘mask’](masking.md#MaskElement)
- [‘pattern’](pservers.md#PatternElement)
- [‘script’](https://www.w3.org/TR/SVG11/script.html#ScriptElement)
- [‘style’](styling.md#StyleElement)
- [‘switch’](struct.md#SwitchElement)
- [‘text’](text.md#TextElement)
- [‘view’](linking.md#ViewElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘d’](#GlyphElementDAttribute)
- [‘horiz-adv-x’](#GlyphElementHorizAdvXAttribute)
- [‘vert-origin-x’](#GlyphElementVertOriginXAttribute)
- [‘vert-origin-y’](#GlyphElementVertOriginYAttribute)
- [‘vert-adv-y’](#GlyphElementVertAdvYAttribute)

**DOM Interfaces:**
:

- [SVGMissingGlyphElement](#InterfaceSVGMissingGlyphElement)

## <a id="GlyphSelectionRules"></a>20.6 Glyph selection rules

When determining the glyph(s) to draw a given character sequence, the [‘font’](#FontElement) element is searched from its first [‘glyph’](#GlyphElement) element to its last in logical order to see if the upcoming sequence of Unicode characters to be rendered matches the sequence of Unicode characters specified in the [‘unicode’](#GlyphElementUnicodeAttribute) attribute for the given [‘glyph’](#GlyphElement) element. The first successful match is used. Thus, the "ffl" ligature needs to be defined in the font before the "f" glyph; otherwise, the "ffl" will never be selected.

Note that any occurrences of [‘altGlyph’](text.md#AltGlyphElement) take precedence over the above glyph selection rules within an SVG font.

## <a id="KernElements"></a>20.7 The ‘hkern’ and ‘vkern’ elements

The [‘hkern’](#HKernElement) and [‘vkern’](#VKernElement) elements define kerning pairs for horizontally-oriented and vertically-oriented pairs of glyphs, respectively.

Kern pairs identify pairs of glyphs within a single font whose inter-glyph spacing is adjusted when the pair of glyphs are rendered next to each other. In addition to the requirement that the pair of glyphs are from the same font, SVG font kerning happens only when the two glyphs correspond to characters which have the same values for properties [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-weight’](text.md#FontWeightProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty) and [‘font’](text.md#FontProperty).

An example of a kerning pair are the letters "Va", where the typographic result might look better if the letters "V" and the "a" were rendered slightly closer together.

Right-to-left and bidirectional text in SVG is laid out in a two-step process, which is described in [Relationship with bidirectionality](text.md#RelationshipWithBiDirectionality). If SVG fonts are used, before kerning is applied, characters are re-ordered into left-to-right (or top-to-bottom, for vertical text) visual rendering order. Kerning from SVG fonts is then applied on pairs of glyphs which are rendered contiguously. The first glyph in the kerning pair is the left (or top) glyph in visual rendering order. The second glyph in the kerning pair is the right (or bottom) glyph in the pair.

For convenience to font designers and to minimize file sizes, a single [‘hkern’](#HKernElement) and [‘vkern’](#VKernElement) can define a single kerning adjustment value between one set of glyphs (e.g., a range of Unicode characters) and another set of glyphs (e.g., another range of Unicode characters).

<a id="HKernElement"></a>The [‘hkern’](#HKernElement) element defines kerning pairs and adjustment values in the horizontal advance value when drawing pairs of glyphs which the two glyphs are contiguous and are both rendered horizontally (i.e., side-by-side). The spacing between characters is reduced by the kerning adjustment. (Negative kerning adjustments increase the spacing between characters.)

<a id="VKernElement"></a>The [‘vkern’](#VKernElement) element defines kerning pairs and adjustment values in the vertical advance value when drawing pairs of glyphs together when stacked vertically. The spacing between characters is reduced by the kerning adjustment.

‘hkern’

**Categories:**
: None
**Content model:**
: Empty.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘u1’](#HKernElementU1Attribute)
- [‘g1’](#HKernElementG1Attribute)
- [‘u2’](#HKernElementU2Attribute)
- [‘g2’](#HKernElementG2Attribute)
- [‘k’](#HKernElementKAttribute)

**DOM Interfaces:**
:

- [SVGHKernElement](#InterfaceSVGHKernElement)

‘vkern’

**Categories:**
: None
**Content model:**
: Empty.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘u1’](#HKernElementU1Attribute)
- [‘g1’](#HKernElementG1Attribute)
- [‘u2’](#HKernElementU2Attribute)
- [‘g2’](#HKernElementG2Attribute)
- [‘k’](#HKernElementKAttribute)

**DOM Interfaces:**
:

- [SVGVKernElement](#InterfaceSVGVKernElement)

*Attribute definitions:*

**<a id="HKernElementU1Attribute"></a>u1 = "[<character> | <urange> ] [, [<character> | <urange>] ]* "**
: A sequence (comma-separated) of Unicode characters (refer to the description of the [‘unicode’](#GlyphElementUnicodeAttribute) attribute to the [‘glyph’](#GlyphElement) element for a description of how to express individual Unicode characters) and/or ranges of Unicode characters (see description of ranges of Unicode characters in CSS2; [[CSS2](refs.md#ref-CSS2)], section 15.3.3) which identify a set of possible first glyphs in the kerning pair. If a given Unicode character within the set has multiple corresponding [‘glyph’](#GlyphElement) elements (i.e., there are multiple [‘glyph’](#GlyphElement) elements with the same [‘unicode’](#GlyphElementUnicodeAttribute) attribute value, but different [‘glyph-name’](#GlyphElementGlyphNameAttribute) values), then all such glyphs are included in the set. Comma is the separator character; thus, to kern a comma, specify the comma as part of a range of Unicode characters or as a glyph name using the [‘g1’](#HKernElementG1Attribute) attribute. The total set of possible first glyphs in the kerning pair is the union of glyphs specified by the [‘u1’](#HKernElementU1Attribute) and [‘g1’](#HKernElementG1Attribute) attributes. [Animatable](animate.md#Animatable): no.
**<a id="HKernElementG1Attribute"></a>g1 = "<name> [, <name> ]* "**
: A sequence (comma-separated) of glyph names (i.e., values that match [‘glyph-name’](#GlyphElementGlyphNameAttribute) attributes on [‘glyph’](#GlyphElement) elements) which identify a set of possible first glyphs in the kerning pair. All glyphs with the given glyph name are included in the set. The total set of possible first glyphs in the kerning pair is the union of glyphs specified by the [‘u1’](#HKernElementU1Attribute) and [‘g1’](#HKernElementG1Attribute) attributes. [Animatable](animate.md#Animatable): no.
**<a id="HKernElementU2Attribute"></a>u2 = "[<character> | <urange>] [, [<character> | <urange>] ]* "**
: Same as the [‘u1’](#HKernElementU1Attribute) attribute, except that [‘u2’](#HKernElementU2Attribute) specifies possible second glyphs in the kerning pair. [Animatable](animate.md#Animatable): no.
**<a id="HKernElementG2Attribute"></a>g2 = "<name> [, <name> ]* "**
: Same as the [‘g1’](#HKernElementG1Attribute) attribute, except that [‘g2’](#HKernElementG2Attribute) specifies possible second glyphs in the kerning pair. [Animatable](animate.md#Animatable): no.
**<a id="HKernElementKAttribute"></a>k = "<number>"**
: The amount to decrease the spacing between the two glyphs in the kerning pair. The value is in the font coordinate system. This attribute is required. [Animatable](animate.md#Animatable): no.
At least one each of [‘u1’](#HKernElementU1Attribute) or [‘g1’](#HKernElementG1Attribute) and at least one of [‘u2’](#HKernElementU2Attribute) or [‘g2’](#HKernElementG2Attribute) must be provided.

## <a id="FontDescriptions"></a>20.8 Describing a font

### <a id="FontDescriptionsOverview"></a>20.8.1 Overview of font descriptions

A font description provides the bridge between an author's font specification and the font data, which is the data needed to format text and to render the abstract glyphs to which the characters map — the actual scalable outlines or bitmaps. Fonts are referenced by properties, such as the [‘font-family’](text.md#FontFamilyProperty) property.

Each specified font description is added to the font database and so that it can be used to select the relevant font data. The font description contains descriptors such as the location of the font data on the Web, and characterizations of that font data. The font descriptors are also needed to match the font properties to particular font data. The level of detail of a font description can vary from just the name of the font up to a list of glyph widths.

For more about font descriptions, refer to the Fonts chapter in the CSS2 specification ([[CSS2](refs.md#ref-CSS2)], chapter 15).

### <a id="FontDescriptionsAlternatives"></a>20.8.2 Alternative ways for providing a font description

Font descriptions can be specified in either of the following ways:

- a [‘font-face’](#FontFaceElement) element
- <a id="AtFontFace"></a>an @font-face rule ([[CSS2](refs.md#ref-CSS2)], section 15.3.1) within a CSS style sheet (only applicable for user agents which support using CSS to style the SVG content)

### <a id="FontFaceElement"></a>20.8.3 The ‘font-face’ element

The [‘font-face’](#FontFaceElement) element corresponds directly to the @font-face facility in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.3.1). It can be used to describe the characteristics of any font, SVG font or otherwise.

When used to describe the characteristics of an SVG font contained within the same document, it is recommended that the [‘font-face’](#FontFaceElement) element be a child of the [‘font’](#FontElement) element it is describing so that the [‘font’](#FontElement) element can be self-contained and fully-described. In this case, any [‘font-face-src’](#FontFaceSrcElement) elements within the [‘font-face’](#FontFaceElement) element are ignored as it is assumed that the [‘font-face’](#FontFaceElement) element is describing the characteristics of its parent [‘font’](#FontElement) element.

‘font-face’

**Categories:**
: None
**Content model:**
: Any number of [descriptive elements](intro.md#TermDescriptiveElement) and at most one [‘font-face-src’](#FontFaceSrcElement) element, in any order.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘font-family’](#FontFaceElementFontFamilyAttribute)
- [‘font-style’](#FontFaceElementFontStyleAttribute)
- [‘font-variant’](#FontFaceElementFontVariantAttribute)
- [‘font-weight’](#FontFaceElementFontWeightAttribute)
- [‘font-stretch’](#FontFaceElementFontStretchAttribute)
- [‘font-size’](#FontFaceElementFontSizeAttribute)
- [‘unicode-range’](#FontFaceElementUnicodeRangeAttribute)
- [‘units-per-em’](#FontFaceElementUnitsPerEmAttribute)
- [‘panose-1’](#FontFaceElementPanose1Attribute)
- [‘stemv’](#FontFaceElementStemvAttribute)
- [‘stemh’](#FontFaceElementStemhAttribute)
- [‘slope’](#FontFaceElementSlopeAttribute)
- [‘cap-height’](#FontFaceElementCapHeightAttribute)
- [‘x-height’](#FontFaceElementXHeightAttribute)
- [‘accent-height’](#FontFaceElementAccentHeightAttribute)
- [‘ascent’](#FontFaceElementAscentAttribute)
- [‘descent’](#FontFaceElementDescentAttribute)
- [‘widths’](#FontFaceElementWidthsAttribute)
- [‘bbox’](#FontFaceElementBboxAttribute)
- [‘ideographic’](#FontFaceElementIdeographicAttribute)
- [‘alphabetic’](#FontFaceElementAlphabeticAttribute)
- [‘mathematical’](#FontFaceElementMathematicalAttribute)
- [‘hanging’](#FontFaceElementHangingAttribute)
- [‘v-ideographic’](#FontFaceElementVertIdeographicAttribute)
- [‘v-alphabetic’](#FontFaceElementVertAlphabeticAttribute)
- [‘v-mathematical’](#FontFaceElementVertMathematicalAttribute)
- [‘v-hanging’](#FontFaceElementVertHangingAttribute)
- [‘underline-position’](#FontFaceElementUnderlinePositionAttribute)
- [‘underline-thickness’](#FontFaceElementUnderlineThicknessAttribute)
- [‘strikethrough-position’](#FontFaceElementStrikeThroughPositionAttribute)
- [‘strikethrough-thickness’](#FontFaceElementStrikeThroughThicknessAttribute)
- [‘overline-position’](#FontFaceElementOverlinePositionAttribute)
- [‘overline-thickness’](#FontFaceElementOverlineThicknessAttribute)

**DOM Interfaces:**
:

- [SVGFontFaceElement](#InterfaceSVGFontFaceElement)

*Attribute definitions:*

**<a id="FontFaceElementFontFamilyAttribute"></a>font-family = "<string>"**
: Same syntax and semantics as the ‘font-family’ descriptor within an @font-face rule. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementFontStyleAttribute"></a>font-style = "all | [ normal | italic | oblique] [, [normal | italic | oblique]]*"**
: Same syntax and semantics as the ‘font-style’ descriptor within an @font-face rule. The style of a font. Takes on the same values as the [‘font-style’](text.md#FontStyleProperty) property, except that a comma-separated list is permitted. If the attribute is not specified, the effect is as if a value of 'all' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementFontVariantAttribute"></a>font-variant = "[normal | small-caps] [,[normal | small-caps]]*"**
: Same syntax and semantics as the ‘font-variant’ descriptor within an @font-face rule. Indication of whether this face is the small-caps variant of a font. Takes on the same values as the [‘font-variant’](text.md#FontVariantProperty) property, except that a comma-separated list is permitted. If the attribute is not specified, the effect is as if a value of 'normal' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementFontWeightAttribute"></a>font-weight = "all | [normal | bold | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900] [, [normal | bold | 100 | 200 | 300 | 400 | 500 | 600 | 700 | 800 | 900]]*"**
: Same syntax and semantics as the ‘font-weight’ descriptor within an @font-face rule. The weight of a face relative to others in the same font family. Takes on the same values as the [‘font-weight’](text.md#FontWeightProperty) property with three exceptions:

- relative keywords (bolder, lighter) are not permitted
- a comma-separated list of values is permitted, for fonts that contain multiple weights
- an additional keyword, 'all', is permitted, which means that the font will match for all possible weights; either because it contains multiple weights, or because that face only has a single weight.

If the attribute is not specified, the effect is as if a value of 'all' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementFontStretchAttribute"></a>font-stretch = "all | [ normal | ultra-condensed | extra-condensed | condensed | semi-condensed | semi-expanded | expanded | extra-expanded | ultra-expanded] [, [ normal | ultra-condensed | extra-condensed | condensed | semi-condensed | semi-expanded | expanded | extra-expanded | ultra-expanded] ]*"**
: Same syntax and semantics as the ‘font-stretch’ descriptor within an @font-face rule. Indication of the condensed or expanded nature of the face relative to others in the same font family. Takes on the same values as the [‘font-stretch’](text.md#FontStretchProperty) property except that:

- relative keywords (wider, narrower) are not permitted
- a comma-separated list is permitted
- the keyword 'all' is permitted

If the attribute is not specified, the effect is as if a value of 'normal' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementFontSizeAttribute"></a>font-size = "<string>"**
: Same syntax and semantics as the ‘font-size’ descriptor within an @font-face rule. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementUnicodeRangeAttribute"></a>unicode-range = "<urange> [, <urange>]*"**
: Same syntax and semantics as the ‘unicode-range’ descriptor within an @font-face rule. The range of ISO 10646 characters [[UNICODE](refs.md#ref-UNICODE)] possibly covered by the glyphs in the font. Except for any additional information provided in this specification, the normative definition of the attribute is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.3.3). If the attribute is not specified, the effect is as if a value of 'U+0-10FFFF' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementUnitsPerEmAttribute"></a>units-per-em = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘units-per-em’ descriptor within an @font-face rule. The number of coordinate units on the em square, the size of the design grid on which glyphs are laid out. This value is almost always necessary as nearly every other attribute requires the definition of a design grid. If the attribute is not specified, the effect is as if a value of '1000' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementPanose1Attribute"></a>panose-1 = "[<integer>]{10}"**
: Same syntax and semantics as the ‘panose-1’ descriptor within an @font-face rule. The Panose-1 number, consisting of ten decimal integers, separated by whitespace. Except for any additional information provided in this specification, the normative definition of the attribute is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.3.6). If the attribute is not specified, the effect is as if a value of '0 0 0 0 0 0 0 0 0 0' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementStemvAttribute"></a>stemv = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘stemv’ descriptor within an @font-face rule. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementStemhAttribute"></a>stemh = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘stemh’ descriptor within an @font-face rule. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementSlopeAttribute"></a>slope = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘slope’ descriptor within an @font-face rule. The vertical stroke angle of the font. Except for any additional information provided in this specification, the normative definition of the attribute is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.3.6). If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementCapHeightAttribute"></a>cap-height = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘cap-height’ descriptor within an @font-face rule. The height of uppercase glyphs in the font within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementXHeightAttribute"></a>x-height = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘x-height’ descriptor within an @font-face rule. The height of lowercase glyphs in the font within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementAccentHeightAttribute"></a>accent-height = "[<number>](types.md#DataTypeNumber)"**
: The distance from the origin to the top of accent characters, measured by a distance within the font coordinate system. If the attribute is not specified, the effect is as if the attribute were set to the value of the [‘ascent’](#FontFaceElementAscentAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementAscentAttribute"></a>ascent = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘ascent’ descriptor within an @font-face rule. The maximum unaccented height of the font within the font coordinate system. If the attribute is not specified, the effect is as if the attribute were set to the difference between the [‘units-per-em’](#FontFaceElementUnitsPerEmAttribute) value and the [‘vert-origin-y’](#FontElementVertOriginYAttribute) value for the corresponding font. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementDescentAttribute"></a>descent = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘descent’ descriptor within an @font-face rule. The maximum unaccented depth of the font within the font coordinate system. If the attribute is not specified, the effect is as if the attribute were set to the [‘vert-origin-y’](#FontElementVertOriginYAttribute) value for the corresponding font. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementWidthsAttribute"></a>widths = "<string>"**
: Same syntax and semantics as the ‘widths’ descriptor within an @font-face rule. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementBboxAttribute"></a>bbox = "<string>"**
: Same syntax and semantics as the ‘bbox’ descriptor within an @font-face rule. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementIdeographicAttribute"></a>ideographic = "[<number>](types.md#DataTypeNumber)"**
: For horizontally oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve ideographic baseline alignment. The value is an offset in the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementAlphabeticAttribute"></a>alphabetic = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘baseline’ descriptor within an @font-face rule. For horizontally oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve alphabetic baseline alignment. The value is an offset in the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementMathematicalAttribute"></a>mathematical = "[<number>](types.md#DataTypeNumber)"**
: Same syntax and semantics as the ‘mathline’ descriptor within an @font-face rule. For horizontally oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve mathematical baseline alignment. The value is an offset in the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementHangingAttribute"></a>hanging = "[<number>](types.md#DataTypeNumber)"**
: For horizontally oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve hanging baseline alignment. The value is an offset in the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementVertIdeographicAttribute"></a>v-ideographic = "[<number>](types.md#DataTypeNumber)"**
: For vertically oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve ideographic baseline alignment. The value is an offset in the font coordinate system relative to the glyph-specific [‘vert-origin-x’](#GlyphElementVertOriginXAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementVertAlphabeticAttribute"></a>v-alphabetic = "[<number>](types.md#DataTypeNumber)"**
: For vertically oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve alphabetic baseline alignment. The value is an offset in the font coordinate system relative to the glyph-specific [‘vert-origin-x’](#GlyphElementVertOriginXAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementVertMathematicalAttribute"></a>v-mathematical = "[<number>](types.md#DataTypeNumber)"**
: For vertically oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve mathematical baseline alignment. The value is an offset in the font coordinate system relative to the glyph-specific [‘vert-origin-x’](#GlyphElementVertOriginXAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementVertHangingAttribute"></a>v-hanging = "[<number>](types.md#DataTypeNumber)"**
: For vertically oriented glyph layouts, indicates the alignment coordinate for glyphs to achieve hanging baseline alignment. The value is an offset in the font coordinate system relative to the glyph-specific [‘vert-origin-x’](#GlyphElementVertOriginXAttribute) attribute. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementUnderlinePositionAttribute"></a>underline-position = "[<number>](types.md#DataTypeNumber)"**
: The ideal position of an underline within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementUnderlineThicknessAttribute"></a>underline-thickness = "[<number>](types.md#DataTypeNumber)"**
: The ideal thickness of an underline, expressed as a length within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementStrikeThroughPositionAttribute"></a>strikethrough-position = "[<number>](types.md#DataTypeNumber)"**
: The ideal position of a strike-through within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementStrikeThroughThicknessAttribute"></a>strikethrough-thickness = "[<number>](types.md#DataTypeNumber)"**
: The ideal thickness of a strike-through, expressed as a length within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementOverlinePositionAttribute"></a>overline-position = "[<number>](types.md#DataTypeNumber)"**
: The ideal position of an overline within the font coordinate system. [Animatable](animate.md#Animatable): no.
**<a id="FontFaceElementOverlineThicknessAttribute"></a>overline-thickness = "[<number>](types.md#DataTypeNumber)"**
: The ideal thickness of an overline, expressed as a length within the font coordinate system. [Animatable](animate.md#Animatable): no.
The following elements and attributes correspond to the ‘src’ descriptor within an @font-face rule. (Refer to the descriptions of the @font-face rule and 'src' descriptor in the CSS2 specification ([[CSS2](refs.md#ref-CSS2)], sections 15.3.1 and 15.3.5.)

### <a id="FontFaceSrcElement"></a>20.8.4 The ‘font-face-src’ element

The [‘font-face-src’](#FontFaceSrcElement) element, together with the [‘font-face-uri’](#FontFaceURIElement) and [‘font-face-format’](#FontFaceFormatElement) elements described in the following sections, correspond to the ‘src’ descriptor within an @font-face rule. (Refer to the descriptions of the @font-face rule and 'src' descriptor in the CSS2 specification ([[CSS2](refs.md#ref-CSS2)], sections 15.3.1 and 15.3.5).

A [‘font-face-src’](#FontFaceSrcElement) element contains [‘font-face-uri’](#FontFaceURIElement) and [‘font-face-name’](#FontFaceNameElement) elements, which are used for referencing external and local fonts, respectively.

‘font-face-src’

**Categories:**
: None
**Content model:**
: One or more of the following elements, in any order:

- [‘font-face-name’](#FontFaceNameElement)
- [‘font-face-uri’](#FontFaceURIElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)

**DOM Interfaces:**
:

- [SVGFontFaceSrcElement](#InterfaceSVGFontFaceSrcElement)

### <a id="FontFaceURIElement"></a>20.8.5 The ‘font-face-uri’ and ‘font-face-format’ elements

The [‘font-face-uri’](#FontFaceURIElement) element is used within a [‘font-face-src’](#FontFaceSrcElement) element to reference a font defined inside or outside of the current SVG document.

When a [‘font-face-uri’](#FontFaceURIElement) is referencing an [SVG font](#SVGFontsOverview), then that reference must be to an SVG [‘font’](#FontElement) element, therefore requiring the use of a fragment identifier [[RFC3986](refs.md#ref-RFC3986)]. The referenced [‘font’](#FontElement) element can be local (i.e., within the same document as the [‘font-face-uri’](#FontFaceURIElement) element) or remote (i.e., within a different document).

‘font-face-uri’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [‘font-face-format’](#FontFaceFormatElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#FontFaceUriElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘xlink:href’](#FontFaceUriElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGFontFaceUriElement](#InterfaceSVGFontFaceUriElement)

*Attribute definitions:*

**<a id="FontFaceUriElementHrefAttribute"></a>xlink:href = "[<IRI>](types.md#DataTypeIRI)"**
: The [‘xlink:href’](#FontFaceUriElementHrefAttribute) attribute specifies the location of the referenced font. [Animatable](animate.md#Animatable): no.
Child [‘font-face-format’](#FontFaceFormatElement) elements of a [‘font-face-uri’](#FontFaceURIElement) element are used to specify the supported formats of the font referenced by that [‘font-face-uri’](#FontFaceURIElement) element. They correspond to entries in a format(…) clause of the ‘src’ descriptor in an @font-face rule.

<a id="FontFaceFormatElement"></a>

‘font-face-format’

**Categories:**
: None
**Content model:**
: Empty.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘string’](#FontFaceFormatElementStringAttribute)

**DOM Interfaces:**
:

- [SVGFontFaceFormatElement](#InterfaceSVGFontFaceFormatElement)

*Attribute definitions:*

**<a id="FontFaceFormatElementStringAttribute"></a>string = "[<anything>](types.md#DataTypeAnything)"**
: The [‘string’](#FontFaceFormatElementStringAttribute) attribute is a hint to the user agent, and specifies a list of formats that the font referenced by the parent [‘font-face-uri’](#FontFaceURIElement) element supports. The syntax of the attribute value is a format string as defined in CSS2, such as 'truetype'. Refer to the description of the 'src' descriptor in CSS2 for details on how the format hint is interpreted ([[CSS2](refs.md#ref-CSS2)], section 15.3.5). [Animatable](animate.md#Animatable): no.
### <a id="FontFaceNameElement"></a>20.8.6 The ‘font-face-name’ element

The [‘font-face-name’](#FontFaceNameElement) element is used within a [‘font-face-src’](#FontFaceSrcElement) element to reference a local font by name. It corresponds to a local(…) clause in an @font-face rule ‘src’ descriptor.

‘font-face-name’

**Categories:**
: None
**Content model:**
: Empty.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘name’](#FontFaceNameElementNameAttribute)

**DOM Interfaces:**
:

- [SVGFontFaceNameElement](#InterfaceSVGFontFaceNameElement)

*Attribute definitions:*

**<a id="FontFaceNameElementNameAttribute"></a>name = "[<anything>](types.md#DataTypeAnything)"**
: The [‘name’](#FontFaceNameElementNameAttribute) attribute specifies the name of a local font. Unlike the syntax allowed between the parentheses of the local(…) clause in an @font-face rule ‘src’ descriptor, the font name specified in this attribute is not surrounded in single or double quotes. Refer to the description of the 'src' descriptor in CSS2 for details on how the font name is interpreted ([[CSS2](refs.md#ref-CSS2)], section 15.3.5). [Animatable](animate.md#Animatable): no.
## <a id="DOMInterfaces"></a>20.9 DOM interfaces

### <a id="InterfaceSVGFontElement"></a>20.9.1 Interface SVGFontElement

The [SVGFontElement](#InterfaceSVGFontElement) interface corresponds to the [‘font’](#FontElement) element.

Object-oriented access to the attributes of the [‘font’](#FontElement) element via the SVG DOM is not available.

```
interface SVGFontElement : SVGElement,
                           SVGExternalResourcesRequired,
                           SVGStylable {
};
```

### <a id="InterfaceSVGGlyphElement"></a>20.9.2 Interface SVGGlyphElement

The [SVGGlyphElement](#InterfaceSVGGlyphElement) interface corresponds to the [‘glyph’](#GlyphElement) element.

Object-oriented access to the attributes of the [‘glyph’](#GlyphElement) element via the SVG DOM is not available.

```
interface SVGGlyphElement : SVGElement,
                            SVGStylable {
};
```

### <a id="InterfaceSVGMissingGlyphElement"></a>20.9.3 Interface SVGMissingGlyphElement

The [SVGMissingGlyphElement](#InterfaceSVGMissingGlyphElement) interface corresponds to the [‘missing-glyph’](#MissingGlyphElement) element.

Object-oriented access to the attributes of the [‘missing-glyph’](#MissingGlyphElement) element via the SVG DOM is not available.

```
interface SVGMissingGlyphElement : SVGElement,
                                   SVGStylable {
};
```

### <a id="InterfaceSVGHKernElement"></a>20.9.4 Interface SVGHKernElement

The [SVGHKernElement](#InterfaceSVGHKernElement) interface corresponds to the [‘hkern’](#HKernElement) element.

Object-oriented access to the attributes of the [‘hkern’](#HKernElement) element via the SVG DOM is not available.

```
interface SVGHKernElement : SVGElement {
};
```

### <a id="InterfaceSVGVKernElement"></a>20.9.5 Interface SVGVKernElement

The [SVGVKernElement](#InterfaceSVGVKernElement) interface corresponds to the [‘vkern’](#VKernElement) element.

Object-oriented access to the attributes of the [‘vkern’](#VKernElement) element via the SVG DOM is not available.

```
interface SVGVKernElement : SVGElement {
};
```

### <a id="InterfaceSVGFontFaceElement"></a>20.9.6 Interface SVGFontFaceElement

The [SVGFontFaceElement](#InterfaceSVGFontFaceElement) interface corresponds to the [‘font-face’](#FontFaceElement) element.

Object-oriented access to the attributes of the [‘font-face’](#FontFaceElement) element via the SVG DOM is not available.

```
interface SVGFontFaceElement : SVGElement {
};
```

### <a id="InterfaceSVGFontFaceSrcElement"></a>20.9.7 Interface SVGFontFaceSrcElement

The [SVGFontFaceSrcElement](#InterfaceSVGFontFaceSrcElement) interface corresponds to the [‘font-face-src’](#FontFaceSrcElement) element.

Object-oriented access to the attributes of the [‘font-face-src’](#FontFaceSrcElement) element via the SVG DOM is not available.

```
interface SVGFontFaceSrcElement : SVGElement {
};
```

### <a id="InterfaceSVGFontFaceUriElement"></a>20.9.8 Interface SVGFontFaceUriElement

The [SVGFontFaceUriElement](#InterfaceSVGFontFaceUriElement) interface corresponds to the [‘font-face-uri’](#FontFaceURIElement) element.

Object-oriented access to the attributes of the [‘font-face-uri’](#FontFaceURIElement) element via the SVG DOM is not available.

```
interface SVGFontFaceUriElement : SVGElement {
};
```

### <a id="InterfaceSVGFontFaceFormatElement"></a>20.9.9 Interface SVGFontFaceFormatElement

The [SVGFontFaceFormatElement](#InterfaceSVGFontFaceFormatElement) interface corresponds to the [‘font-face-format’](#FontFaceFormatElement) element.

Object-oriented access to the attributes of the [‘font-face-format’](#FontFaceFormatElement) element via the SVG DOM is not available.

```
interface SVGFontFaceFormatElement : SVGElement {
};
```

### <a id="InterfaceSVGFontFaceNameElement"></a>20.9.10 Interface SVGFontFaceNameElement

The [SVGFontFaceNameElement](#InterfaceSVGFontFaceNameElement) interface corresponds to the [‘font-face-name’](#FontFaceNameElement) element.

Object-oriented access to the attributes of the [‘font-face-name’](#FontFaceNameElement) element via the SVG DOM is not available.

```
interface SVGFontFaceNameElement : SVGElement {
};
```

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](animate.md) ⋅ [Next](metadata.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
