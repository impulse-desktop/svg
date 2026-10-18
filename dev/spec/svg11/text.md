<!-- https://www.w3.org/TR/SVG11/text.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](shapes.md) ⋅ [Next](painting.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 10 Text

## <a id="Introduction"></a>10.1 Introduction

Text that is to be rendered as part of an SVG document fragment is specified using the [‘text’](#TextElement) element. The characters to be drawn are expressed as XML character data ([[XML10](refs.md#ref-XML10)], section 2.4) inside the [‘text’](#TextElement) element.

SVG's [‘text’](#TextElement) elements are rendered like other graphics elements. Thus, [coordinate system transformations](coords.md), [painting](painting.md), [clipping](masking.md) and [masking](masking.md) features apply to [‘text’](#TextElement) elements in the same way as they apply to [shapes](intro.md#TermShape) such as [paths](paths.md) and [rectangles](shapes.md#RectElement).

Each [‘text’](#TextElement) element causes a single string of text to be rendered. SVG performs no automatic line breaking or word wrapping. To achieve the effect of multiple lines of text, use one of the following methods:

- The author or authoring package needs to pre-compute the line breaks and use multiple [‘text’](#TextElement) elements (one for each line of text).
- The author or authoring package needs to pre-compute the line breaks and use a single [‘text’](#TextElement) element with one or more [‘tspan’](#TSpanElement) child elements with appropriate values for attributes [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) to set new start positions for those characters which start new lines. (This approach allows user text selection across multiple lines of text -- see [Text selection and clipboard operations](#TextSelection).)
- Express the text to be rendered in another XML namespace such as XHTML [[XHTML](refs.md#ref-XHTML)] embedded inline within a [‘foreignObject’](extend.md#ForeignObjectElement) element. (Note: the exact semantics of this approach are not completely defined at this time.)

The text strings within [‘text’](#TextElement) elements can be rendered in a straight line or rendered along the outline of a [‘path’](paths.md#PathElement) element. SVG supports the following international text processing features for both straight line text and text on a path:

- horizontal and vertical orientation of text
- left-to-right or bidirectional text (i.e., languages which intermix right-to-left and left-to-right text, such as Arabic and Hebrew)
- when [SVG fonts](fonts.md#SVGFonts) are used, automatic selection of the correct glyph corresponding to the current form for [Arabic](fonts.md#GlyphElementArabicFormAttribute) and [Han](fonts.md#GlyphElementLangAttribute) text

(The layout rules for straight line text are described in [Text layout](#TextLayout). The layout rules for text on a path are described in [Text on a path layout rules](#TextpathLayoutRules).)

Because SVG text is packaged as XML character data:

- Text data in SVG content is readily accessible to the visually impaired (see [Accessibility Support](access.md))
- In many viewing scenarios, the user will be able to search for and select text strings and copy selected text strings to the system clipboard (see [Text selection and clipboard operations](#TextSelection))
- XML-compatible Web search engines will find text strings in SVG content with no additional effort over what they need to do to find text strings in other XML documents

Multi-language SVG content is possible by [substituting different text strings based on the user's preferred language](i18n.md#I18nTextSubstitution).

For accessibility reasons, it is recommended that text which is included in a document have appropriate semantic markup to indicate its function. See [SVG accessibility guidelines](access.md#SVGAccessibilityGuidelines) for more information.

## <a id="CharactersAndGlyphs"></a>10.2 Characters and their corresponding glyphs

In XML [XML10], textual content is defined in terms of a sequence of XML characters, where each character is defined by a particular Unicode code point [[UNICODE](refs.md#ref-UNICODE)]. Fonts, on the other hand, consist of a collection of glyphs and other associated information, such as [font tables](#FontsTablesBaselines). A glyph is a presentable form of one or more characters (or a part of a character in some cases). Each glyph consists of some sort of identifier (in some cases a string, in other cases a number) along with drawing instructions for rendering that particular glyph.

In many cases, there is a one-to-one mapping of Unicode characters (i.e., Unicode code points) to glyphs in a font. For example, it is common for a font designed for Latin languages (where the term *Latin* is used for European languages such as English with alphabets similar to and/or derivative to the Latin language) to contain a single glyph for each of the standard ASCII characters (i.e., A-to-Z, a-to-z, 0-to-9, plus the various punctuation characters found in ASCII). Thus, in most situations, the string "XML", which consists of three Unicode characters, would be rendered by the three glyphs corresponding to "X", "M" and "L", respectively.

In various other cases, however, there is not a strict one-to-one mapping of Unicode characters to glyphs. Some of the circumstances when the mapping is not one-to-one:

- Ligatures - For best looking typesetting, it is often desirable that particular sequences of characters are rendered as a single glyph. An example is the word "office". Many fonts will define an "ffi" ligature. When the word "office" is rendered, sometimes the user agent will render the glyph for the "ffi" ligature instead of rendering distinct glyphs (i.e., "f", "f" and "i") for each of the three characters. Thus, for ligatures, multiple Unicode characters map to a single glyph. (Note that for proper rendering of some languages, ligatures are required for certain character combinations.)
- Composite characters - In various situations, commonly used adornments such as diacritical marks will be stored once in a font as a particular glyph and then composed with one or more other glyphs to result in the desired character. For example, it is possible that a font engine might render the é character by first rendering the glyph for e and then rendering the glyph for ´ (the accent mark) such that the accent mark will appear over the e. In this situation, a single Unicode character maps to multiple glyphs.
- Glyph substitution - Some typography systems examine the nature of the textual content and utilize different glyphs in different circumstances. For example, in Arabic, the same Unicode character might render as any of four different glyphs, depending on such factors as whether the character appears at the start, the end or the middle of a sequence of cursively joined characters. Different glyphs might be used for a punctuation character depending on inline-progression-direction (e.g., horizontal vs. vertical). In these situations, a single Unicode character might map to one of several alternative glyphs.
- In some languages, particular sequences of characters will be converted into multiple glyphs such that parts of a particular character are in one glyph and the remainder of that character is in another glyph.
- Alternative glyph specification - SVG contains a facility for the author to explicitly specify that a particular sequence of Unicode characters is to be rendered using a particular glyph. (See [Alternate glyphs](#AlternateGlyphs).) When this facility is used, multiple Unicode characters map to a single glyph.

In many situations, the algorithms for mapping from characters to glyphs are system-dependent, resulting in the possibility that the rendering of text might be (usually slightly) different when viewed in different user environments. If the author of SVG content requires precise selection of fonts and glyphs, then the recommendation is that the necessary fonts (potentially subsetted to include only the glyphs needed for the given document) be available either as [SVG fonts](fonts.md#SVGFonts) embedded within the SVG content or as WebFonts ([[CSS2](refs.md#ref-CSS2)], section 15.1) posted at the same Web location as the SVG content.

Throughout this chapter, the term character shall be equivalent to the definition of a character in XML [XML10].

## <a id="FontsTablesBaselines"></a>10.3 Fonts, font tables and baselines

A font consists of a collection of glyphs together with the information (the font tables) necessary to use those glyphs to present characters on some medium. The combination of the collection of glyphs and the font tables is called the *font data*. The font tables include the information necessary to map characters to glyphs, to determine the size of glyph areas and to position the glyph area. Each font table consists of one or more font characteristics, such as the font-weight and font-style.

The geometric font characteristics are expressed in a coordinate system based on the EM box. (The EM is a relative measure of the height of the glyphs in the font; see Coordinate units on the em square; in [[CSS2](refs.md#ref-CSS2)], section 15.4.3.) The box 1 EM high and 1 EM wide is called the *design space*. This space is given a geometric coordinates by sub-dividing the EM into a number of *units per em*.

Note: Units per em is a font characteristic. A typical value for units per em is 1000 or 2048.

The coordinate space of the EM box is called the *design space coordinate system*. For scalable fonts, the curves and lines that are used to draw a glyph are represented using this coordinate system.

Note: Most often, the (0,0) point in this coordinate system is positioned on the left edge of the EM box, but not at the bottom left corner. The Y coordinate of the bottom of a roman capital letter is usually zero. And the descenders on lowercase roman letters have negative coordinate values.

SVG assumes that the font tables will provide at least three font characteristics: an ascent, a descent and a set of baseline-tables. The ascent is the distance to the top of the EM box from the (0,0) point of the font; the descent is the distance to the bottom of the EM box from the (0.0) point of the font. The baseline-table is explained below.

Note: Within an OpenType font, for horizontal writing-modes, the ascent and descent are given by the sTypoAscender and sTypoDescender entries in the OS/2 table. For vertical writing-modes, the descent (the distance, in this case from the (0,0) point to the left edge of the glyph) is normally zero because the (0,0) point is on the left edge. The ascent for vertical writing-modes is either 1 em or is specified by the ideographic top baseline value in the OpenType Base table for vertical writing-modes.

In horizontal writing-modes, the glyphs of a given script are positioned so that a particular point on each glyph, the *[alignment-point](#AlignmentPoint)*, is aligned with the alignment-points of the other glyphs in that script. The glyphs of different scripts, for example, Western, Northern Indic and Far-Eastern scripts, are typically aligned at different points on the glyph. For example, Western glyphs are aligned on the bottoms of the capital letters, northern indic glyphs are aligned at the top of a horizontal stroke near the top of the glyphs and far-eastern glyphs are aligned either at the bottom or center of the glyph. Within a script and within a line of text having a single font-size, the sequence of alignment-points defines, in the inline- progression-direction, a geometric line called a *baseline*. Western and most other alphabetic and syllabic glyphs are aligned to an "alphabetic" baseline, the northern indic glyphs are aligned to a "hanging" baseline and the far-eastern glyphs are aligned to an "ideographic" baseline.

A *baseline-table* specifies the position of one or more baselines in the design space coordinate system. The function of the baseline table is to facilitate the alignment of different scripts with respect to each other when they are mixed on the same text line. Because the desired relative alignments may depend on which script is dominant in a line (or block), there may be a different baseline table for each script. In addition, different alignment positions are needed for horizontal and vertical writing modes. Therefore, the font may have a set of baseline tables: typically, one or more for horizontal writing-modes and zero or more for vertical writing-modes.

Note: Some fonts may not have values for the baseline tables. Heuristics are suggested for approximating the baseline tables when a given font does not supply baseline tables.

SVG further assumes that for each glyph in the font data for a font, there are two width values, two alignment-baselines and two alignment-points, one each for horizontal writing-modes and the other for vertical writing-modes. (Even though it is specified as a width, for vertical writing-modes the width is used in the vertical direction.) The script to which a glyph belongs determines an alignment-baseline to which the glyph is to be aligned. The [inline-progression-direction](#InlineProgressionDirection) position of the alignment-point is on the start-edge of the glyph.

Properties related to baselines are described below under [Baseline alignment properties](#BaselineAlignmentProperties).

In addition to the font characteristics required above, a font may also supply substitution and positioning tables that can be used by a formatter to re-order, combine and position a sequence of glyphs to make one or more composite glyphs. The combination may be as simple as a ligature, or as complex as an indic syllable which combines, usually with some re-ordering, multiple consonants and vowel glyphs.

## <a id="TextElement"></a>10.4 The ‘text’ element

The [‘text’](#TextElement) element defines a graphics element consisting of text. The XML character data within the [‘text’](#TextElement) element, along with relevant attributes and properties and character-to-glyph mapping tables within the font itself, define the glyphs to be rendered. (See [Characters and their corresponding glyphs](#CharactersAndGlyphs).) The attributes and properties on the [‘text’](#TextElement) element indicate such things as the writing direction, font specification and painting attributes which describe how exactly to render the characters. Subsequent sections of this chapter describe the relevant text-specific attributes and properties, particular [text layout](#TextLayout) and [bidirectionality](#RelationshipWithBiDirectionality).

Since [‘text’](#TextElement) elements are rendered using the same rendering methods as other graphics elements, all of the same [coordinate system transformations](coords.md), [painting](painting.md), [clipping](masking.md) and [masking](masking.md) features that apply to [shapes](intro.md#TermShape) such as [paths](paths.md) and [rectangles](shapes.md#RectElement) also apply to [‘text’](#TextElement) elements.

<a id="ObjectBoundingBoxUnitsTextObjects"></a>It is possible to apply a gradient, pattern, clipping path, mask or filter to text. When one of these facilities is applied to text and keyword 'objectBoundingBox' is used (see [Object bounding box units](coords.md#ObjectBoundingBox)) to specify a graphical effect relative to the "object bounding box", then the object bounding box units are computed relative to the entire [‘text’](#TextElement) element in all cases, even when different effects are applied to different [‘tspan’](#TSpanElement) elements within the same [‘text’](#TextElement) element.

The [‘text’](#TextElement) element renders its first glyph (after [bidirectionality](#RelationshipWithBiDirectionality) reordering) at the initial [current text position](#CurrentTextPosition), which is established by the [‘x’](#TextElementXAttribute) and [‘y’](#TextElementYAttribute) attributes on the [‘text’](#TextElement) element (with possible adjustments due to the value of the [‘text-anchor’](#TextAnchorProperty) property, the presence of a [‘textPath’](#TextPathElement) element containing the first character, and/or an [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) or [‘dy’](#TSpanElementDYAttribute) attributes on a [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) element which contains the first character). After the glyph(s) corresponding to the given character is(are) rendered, the current text position is updated for the next character. In the simplest case, the new current text position is the previous current text position plus the glyphs' advance value (horizontal or vertical). See [text layout](#TextLayout) for a description of glyph placement and glyph advance.

‘text’

**Categories:**
: [Graphics element](intro.md#TermGraphicsElement), [text content element](intro.md#TermTextContentElement)
**Content model:**
: Any number of the following elements, in any order:

- [animation elements](intro.md#TermAnimationElement) — [‘animate’](animate.md#AnimateElement), [‘animateColor’](animate.md#AnimateColorElement), [‘animateMotion’](animate.md#AnimateMotionElement), [‘animateTransform’](animate.md#AnimateTransformElement), [‘set’](animate.md#SetElement)
- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [text content child elements](intro.md#TermTextContentChildElement) — [‘altGlyph’](#AltGlyphElement), [‘textPath’](#TextPathElement), [‘tref’](#TRefElement), [‘tspan’](#TSpanElement)
- [‘a’](linking.md#AElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](#AlignmentBaselineProperty), [‘baseline-shift’](#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](#FontFamilyProperty), [‘font-size’](#FontSizeProperty), [‘font-size-adjust’](#FontSizeAdjustProperty), [‘font-stretch’](#FontStretchProperty), [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](#TextAnchorProperty), [‘text-decoration’](#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](#WordSpacingProperty), [‘writing-mode’](#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘lengthAdjust’](#TextElementLengthAdjustAttribute)
- [‘x’](#TextElementXAttribute)
- [‘y’](#TextElementYAttribute)
- [‘dx’](#TextElementDXAttribute)
- [‘dy’](#TextElementDYAttribute)
- [‘rotate’](#TextElementRotateAttribute)
- [‘textLength’](#TextElementTextLengthAttribute)

**DOM Interfaces:**
:

- [SVGTextElement](#InterfaceSVGTextElement)

*Attribute definitions:*

**<a id="TextElementXAttribute"></a>x = "[<list-of-coordinates>](types.md#DataTypeCoordinates)"**
: If a single [<coordinate>](types.md#DataTypeCoordinate) is provided, then the value represents the new absolute X coordinate for the [current text position](#CurrentTextPosition) for rendering the glyphs that correspond to the first character within this element or any of its descendants. If a comma- or space-separated list of *n* [<coordinate>](types.md#DataTypeCoordinate)s is provided, then the values represent new absolute X coordinates for the [current text position](#CurrentTextPosition) for rendering the glyphs corresponding to each of the first *n* characters within this element or any of its descendants. For additional processing rules, refer to the description of the [‘x’](#TSpanElementXAttribute) attribute on the [‘tspan’](#TSpanElement) element. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="TextElementYAttribute"></a>y = "[<list-of-coordinates>](types.md#DataTypeCoordinates)"**
: The corresponding list of absolute Y coordinates for the glyphs corresponding to the characters within this element. The processing rules for the [‘y’](#TextElementYAttribute) attribute parallel the processing rules for the [‘x’](#TextElementXAttribute) attribute. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="TextElementDXAttribute"></a>dx = "[<list-of-lengths>](types.md#DataTypeLengths)"**
: Shifts in the [current text position](#CurrentTextPosition) along the x-axis for the characters within this element or any of its descendants. Refer to the description of the [‘dx’](#TSpanElementDXAttribute) attribute on the [‘tspan’](#TSpanElement) element. If the attribute is not specified on this element or any of its descendants, no supplemental shifts along the x-axis will occur. [Animatable](animate.md#Animatable): yes.
**<a id="TextElementDYAttribute"></a>dy = "[<list-of-lengths>](types.md#DataTypeLengths)"**
: Shifts in the [current text position](#CurrentTextPosition) along the y-axis for the characters within this element or any of its descendants. Refer to the description of the [‘dy’](#TSpanElementDYAttribute) attribute on the [‘tspan’](#TSpanElement) element. If the attribute is not specified on this element or any of its descendants, no supplemental shifts along the y-axis will occur. [Animatable](animate.md#Animatable): yes.
**<a id="TextElementRotateAttribute"></a>rotate = "[<list-of-numbers>](types.md#DataTypeNumbers)"**
: The supplemental rotation about the [current text position](#CurrentTextPosition) that will be applied to all of the glyphs corresponding to each character within this element. Refer to the description of the [‘rotate’](#TSpanElementRotateAttribute) attribute on the [‘tspan’](#TSpanElement) element. If the attribute is not specified on this element or any of its descendants, no supplemental rotations will occur. [Animatable](animate.md#Animatable): yes (non-additive).
**<a id="TextElementTextLengthAttribute"></a>textLength = "[<length>](types.md#DataTypeLength)"**
: The author's computation of the total sum of all of the advance values that correspond to character data within this element, including the advance value on the glyph (horizontal or vertical), the effect of properties [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty) and [‘word-spacing’](#WordSpacingProperty) and adjustments due to attributes [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) on [‘tspan’](#TSpanElement) elements. This value is used to calibrate the user agent's own calculations with that of the author. The purpose of this attribute is to allow the author to achieve exact alignment, in visual rendering order after any [bidirectional reordering](#RelationshipWithBiDirectionality), for the first and last rendered glyphs that correspond to this element; thus, for the last rendered character (in visual rendering order after any [bidirectional reordering](#RelationshipWithBiDirectionality)), any supplemental inter-character spacing beyond normal glyph advances are ignored (in most cases) when the user agent determines the appropriate amount to expand/compress the text string to fit within a length of [‘textLength’](#TextElementTextLengthAttribute). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). If the attribute is not specified, the effect is as if the author's computation exactly matched the value calculated by the user agent; thus, no advance adjustments are made. [Animatable](animate.md#Animatable): yes.
**<a id="TextElementLengthAdjustAttribute"></a>lengthAdjust = "spacing|spacingAndGlyphs"**
: Indicates the type of adjustments which the user agent shall make to make the rendered length of the text match the value specified on the [‘textLength’](#TextElementTextLengthAttribute) attribute. 'spacing' indicates that only the advance values are adjusted. The glyphs themselves are not stretched or compressed. 'spacingAndGlyphs' indicates that the advance values are adjusted and the glyphs themselves stretched or compressed in one axis (i.e., a direction parallel to the inline-progression-direction). The user agent is required to achieve correct start and end positions for the text strings, but the locations of intermediate glyphs are not predictable because user agents might employ advanced algorithms to stretch or compress text strings in order to balance correct start and end positioning with optimal typography. Note that, for a text string that contains *n* characters, the adjustments to the advance values often occur only for *n*−1 characters (see description of attribute [‘textLength’](#TextElementTextLengthAttribute)), whereas stretching or compressing of the glyphs will be applied to all *n* characters. If the attribute is not specified, the effect is as a value of 'spacing' were specified. [Animatable](animate.md#Animatable): yes.
<a id="ExampleText01"></a>Example text01 below contains the text string "Hello, out there" which will be rendered onto the canvas using the Verdana font family with the glyphs filled with the color blue.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example text01 - 'Hello, out there' in blue</desc>

  <text x="250" y="150" 
        font-family="Verdana" font-size="55" fill="blue" >
    Hello, out there
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example test01

| [image: Example test01 — 'Hello, out there' in blue] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/text01.svg)

## <a id="TSpanElement"></a>10.5 The ‘tspan’ element

Within a [‘text’](#TextElement) element, text and font properties and the [current text position](#CurrentTextPosition) can be adjusted with absolute or relative coordinate values by including a ‘tspan’ element.

‘tspan’

**Categories:**
: [Text content element](intro.md#TermTextContentElement), [text content child element](intro.md#TermTextContentChildElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [‘a’](linking.md#AElement)
- [‘altGlyph’](#AltGlyphElement)
- [‘animate’](animate.md#AnimateElement)
- [‘animateColor’](animate.md#AnimateColorElement)
- [‘set’](animate.md#SetElement)
- [‘tref’](#TRefElement)
- [‘tspan’](#TSpanElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](#AlignmentBaselineProperty), [‘baseline-shift’](#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](#FontFamilyProperty), [‘font-size’](#FontSizeProperty), [‘font-size-adjust’](#FontSizeAdjustProperty), [‘font-stretch’](#FontStretchProperty), [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](#TextAnchorProperty), [‘text-decoration’](#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](#WordSpacingProperty), [‘writing-mode’](#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘x’](#TSpanElementXAttribute)
- [‘y’](#TSpanElementYAttribute)
- [‘dx’](#TSpanElementDXAttribute)
- [‘dy’](#TSpanElementDYAttribute)
- [‘rotate’](#TSpanElementRotateAttribute)
- [‘textLength’](#TSpanElementTextLengthAttribute)
- [‘lengthAdjust’](#TextElementLengthAdjustAttribute)

**DOM Interfaces:**
:

- [SVGTSpanElement](#InterfaceSVGTSpanElement)

*Attribute definitions:*

**<a id="TSpanElementXAttribute"></a>x = "[<list-of-coordinates>](types.md#DataTypeCoordinates)"**
: If a single [<coordinate>](types.md#DataTypeCoordinate) is provided, then the value represents the new absolute X coordinate for the [current text position](#CurrentTextPosition) for rendering the glyphs that correspond to the first character within this element or any of its descendants. If a comma- or space-separated list of *n* [<coordinate>](types.md#DataTypeCoordinate)s is provided, then the values represent new absolute X coordinates for the [current text position](#CurrentTextPosition) for rendering the glyphs corresponding to each of the first *n* characters within this element or any of its descendants. If more [<coordinate>](types.md#DataTypeCoordinate)s are provided than characters, then the extra [<coordinate>](types.md#DataTypeCoordinate)s will have no effect on glyph positioning. If more characters exist than [<coordinate>](types.md#DataTypeCoordinate)s, then for each of these extra characters: (a) if an ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) element specifies an absolute X coordinate for the given character via an ‘x’ attribute, then that absolute X coordinate is used as the starting X coordinate for that character (nearest ancestor has precedence), else (b) the starting X coordinate for rendering the glyphs corresponding to the given character is the X coordinate of the resulting [current text position](#CurrentTextPosition) from the most recently rendered glyph for the current [‘text’](#TextElement) element. If the attribute is not specified: (a) if an ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) element specifies an absolute X coordinate for a given character via an ‘x’ attribute, then that absolute X coordinate is used (nearest ancestor has precedence), else (b) the starting X coordinate for rendering the glyphs corresponding to a given character is the X coordinate of the resulting [current text position](#CurrentTextPosition) from the most recently rendered glyph for the current [‘text’](#TextElement) element. [Animatable](animate.md#Animatable): yes.
**<a id="TSpanElementYAttribute"></a>y = "[<list-of-coordinates>](types.md#DataTypeCoordinates)"**
: The corresponding list of absolute Y coordinates for the glyphs corresponding to the characters within this element. The processing rules for the [‘y’](#TSpanElementYAttribute) attribute parallel the processing rules for the [‘x’](#TSpanElementXAttribute) attribute. [Animatable](animate.md#Animatable): yes.
**<a id="TSpanElementDXAttribute"></a>dx = "[<list-of-lengths>](types.md#DataTypeLengths)"**
: If a single [<length>](types.md#DataTypeLength) is provided, this value represents the new relative X coordinate for the [current text position](#CurrentTextPosition) for rendering the glyphs corresponding to the first character within this element or any of its descendants. The [current text position](#CurrentTextPosition) is shifted along the x-axis of the current user coordinate system by [<length>](types.md#DataTypeLength) before the first character's glyphs are rendered. If a comma- or space-separated list of *n* [<length>](types.md#DataTypeLength)s is provided, then the values represent incremental shifts along the x-axis for the [current text position](#CurrentTextPosition) before rendering the glyphs corresponding to the first *n* characters within this element or any of its descendants. Thus, before the glyphs are rendered corresponding to each character, the [current text position](#CurrentTextPosition) resulting from drawing the glyphs for the previous character within the current [‘text’](#TextElement) element is shifted along the X axis of the current user coordinate system by [<length>](types.md#DataTypeLength). If more [<length>](types.md#DataTypeLength)s are provided than characters, then any extra [<length>](types.md#DataTypeLength)s will have no effect on glyph positioning. If more characters exist than [<length>](types.md#DataTypeLength)s, then for each of these extra characters: (a) if an ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) element specifies a relative X coordinate for the given character via a ‘dx’ attribute, then the [current text position](#CurrentTextPosition) is shifted along the x-axis of the current user coordinate system by that amount (nearest ancestor has precedence), else (b) no extra shift along the x-axis occurs. If the attribute is not specified: (a) if an ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) element specifies a relative X coordinate for a given character via a ‘dx’ attribute, then the [current text position](#CurrentTextPosition) is shifted along the x-axis of the current user coordinate system by that amount (nearest ancestor has precedence), else (b) no extra shift along the x-axis occurs. [Animatable](animate.md#Animatable): yes.
**<a id="TSpanElementDYAttribute"></a>dy = "[<list-of-lengths>](types.md#DataTypeLengths)"**
: The corresponding list of relative Y coordinates for the characters within the [‘tspan’](#TSpanElement) element. The processing rules for the [‘dy’](#TSpanElementDYAttribute) attribute parallel the processing rules for the [‘dx’](#TSpanElementDXAttribute) attribute. [Animatable](animate.md#Animatable): yes.
**<a id="TSpanElementRotateAttribute"></a>rotate = "[<list-of-numbers>](types.md#DataTypeNumbers)"**
: The supplemental rotation about the [current text position](#CurrentTextPosition) that will be applied to all of the glyphs corresponding to each character within this element. If a comma- or space-separated list of [<number>](types.md#DataTypeNumber)s is provided, then the first [<number>](types.md#DataTypeNumber) represents the supplemental rotation for the glyphs corresponding to the first character within this element or any of its descendants, the second [<number>](types.md#DataTypeNumber) represents the supplemental rotation for the glyphs that correspond to the second character, and so on. If more [<number>](types.md#DataTypeNumber)s are provided than there are characters, then the extra [<number>](types.md#DataTypeNumber)s will be ignored. If more characters are provided than [<number>](types.md#DataTypeNumber)s, then for each of these extra characters the rotation value specified by the last number must be used. If the attribute is not specified and if an ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) element specifies a supplemental rotation for a given character via a [‘rotate’](#TSpanElementRotateAttribute) attribute, then the given supplemental rotation is applied to the given character (nearest ancestor has precedence). If there are more characters than [<number>](types.md#DataTypeNumber)s specified in the ancestor's [‘rotate’](#TSpanElementRotateAttribute) attribute, then for each of these extra characters the rotation value specified by the last number must be used. This supplemental rotation has no impact on the rules by which [current text position](#CurrentTextPosition) is modified as glyphs get rendered and is supplemental to any rotation due to [text on a path](#TextOnAPath) and to [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty) or [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty). [Animatable](animate.md#Animatable): yes (non-additive).
**<a id="TSpanElementTextLengthAttribute"></a>textLength = "[<length>](types.md#DataTypeLength)"**
: The author's computation of the total sum of all of the advance values that correspond to character data within this element, including the advance value on the glyph (horizontal or vertical), the effect of properties [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty) and [‘word-spacing’](#WordSpacingProperty) and adjustments due to attributes [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) on this [‘tspan’](#TSpanElement) element or any descendants. This value is used to calibrate the user agent's own calculations with that of the author. The purpose of this attribute is to allow the author to achieve exact alignment, in visual rendering order after any [bidirectional reordering](#RelationshipWithBiDirectionality), for the first and last rendered glyphs that correspond to this element; thus, for the last rendered character (in visual rendering order after any [bidirectional reordering](#RelationshipWithBiDirectionality)), any supplemental inter-character spacing beyond normal glyph advances are ignored (in most cases) when the user agent determines the appropriate amount to expand/compress the text string to fit within a length of [‘textLength’](#TSpanElementTextLengthAttribute). If attribute [‘textLength’](#TSpanElementTextLengthAttribute) is specified on a given element and also specified on an ancestor, the adjustments on all character data within this element are controlled by the value of [‘textLength’](#TSpanElementTextLengthAttribute) on this element exclusively, with the possible side-effect that the adjustment ratio for the contents of this element might be different than the adjustment ratio used for other content that shares the same ancestor. The user agent must assume that the total advance values for the other content within that ancestor is the difference between the advance value on that ancestor and the advance value for this element. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). If the attribute is not specified anywhere within a [‘text’](#TextElement) element, the effect is as if the author's computation exactly matched the value calculated by the user agent; thus, no advance adjustments are made. [Animatable](animate.md#Animatable): yes.
The [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) on the [‘tspan’](#TSpanElement) element are useful in high-end typography scenarios where individual glyphs require exact placement. These attributes are useful for minor positioning adjustments between characters or for major positioning adjustments, such as moving the [current text position](#CurrentTextPosition) to a new location to achieve the visual effect of a new line of text. Multi-line [‘text’](#TextElement) elements are possible by defining different [‘tspan’](#TSpanElement) elements for each line of text, with attributes [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) and/or [‘dy’](#TSpanElementDYAttribute) defining the position of each [‘tspan’](#TSpanElement). (An advantage of such an approach is that users will be able to perform multi-line [text selection](#TextSelection).)

In situations where micro-level positioning adjustment are necessary for advanced typographic control, the SVG content designer needs to ensure that the necessary font will be available for all viewers of the document (e.g., package up the necessary font data in the form of an SVG font or an alternative WebFont format which is stored at the same Web site as the SVG content) and that the viewing software will process the font in the expected way (the capabilities, characteristics and font layout mechanisms vary greatly from system to system). If the SVG content contains [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) or [‘dy’](#TSpanElementDYAttribute) attribute values which are meant to correspond to a particular font processed by a particular set of viewing software and either of these requirements is not met, then the text might display with poor quality.

The following additional rules apply to attributes [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) when they contain a list of numbers:

- When a single XML character maps to a single glyph - In this case, the i-th value for the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) attributes is applied to the glyph that corresponds to the i-th character.
- When a single XML character maps to multiple glyphs (e.g., when an accent glyph is placed on top of a base glyph) - In this case, the i-th value for the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) values are applied (i.e., the [current text position](#CurrentTextPosition) is adjusted) before rendering the first glyph. The rotation transformation corresponding to the i-th [‘rotate’](#TSpanElementRotateAttribute) value is applied to the glyphs and to the inter-glyph advance values corresponding to this character on a group basis (i.e., the rotation value creates a temporary new rotated coordinate system, and the glyphs orresponding to the character are rendered into this rotated coordinate system).
- When multiple XML characters map to a single glyph (e.g., when a ligature is used) - Suppose that the i-th and (i+1)-th XML characters map to a single glyph. In this case, the i-th value for the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) attributes all apply when rendering the glyph. The (i+1)-th values, however, for [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) are ignored (exception: the final [‘rotate’](#TSpanElementRotateAttribute) value in the list would still apply to subsequent characters), whereas the [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) are applied to the subsequent XML character (i.e., the (i+2)-th character), if one exists, by translating the [current text position](#CurrentTextPosition) by the given amounts before rendering the first glyph associated with that character.
- When there is a many-to-many mapping of characters to glyphs (e.g., when three characters map to two glyphs, such as when the first glyph expresses the first character and half of the second character, and the second glyph expresses the other half of the second character plus the third character) - Suppose that the i-th, (i+1)-th and (i+2)-th XML characters map to two glyphs. In this case, the i-th value for the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) values are applied (i.e., the [current text position](#CurrentTextPosition) is adjusted) before rendering the first glyph. The rotation transformation corresponding to the i-th [‘rotate’](#TSpanElementRotateAttribute) value is applied to both the two glyphs and the glyph advance values for the first glyph on a group basis (i.e., the rotation value creates a temporary new rotated coordinate system, and the two glyphs are rendered into the temporary rotated coordinate system). The (i+1)-th and (i+2)-th values, however, for the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) attributes are not applied (exception: the final [‘rotate’](#TSpanElementRotateAttribute) value in the list would still apply to subsequent characters), whereas the (i+1)-th and (i+2)-th values for the [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) attributes are applied to the subsequent XML character (i.e., the (i+3)-th character), if one exists, by translating the [current text position](#CurrentTextPosition) by the given amounts before rendering the first glyph associated with that character.
- Relationship to [bidirectionality](#RelationshipWithBiDirectionality) - As described below in the discussion on [bidirectionality](#RelationshipWithBiDirectionality), text is laid out in a two-step process, where any bidirectional text is first re-ordered into a left-to-right string, and then text layout occurs with the re-ordered text string. Whenever the character data within a [‘tspan’](#TSpanElement) element is re-ordered, the corresponding elements within the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) are also re-ordered to maintain the correspondence. For example, suppose that you have the following [‘tspan’](#TSpanElement) element:
```
<tspan dx="11 12 13 14 15 0 21 22 23 0 31 32 33 34 35 36">Latin and Hebrew</tspan>
```

and that the word "Hebrew" will be drawn right-to-left. First, the character data and the corresponding values in the [‘dx’](#TSpanElementDXAttribute) list will be reordered, such that the text string will be "Latin and werbeH" and the list of values for the [‘dx’](#TSpanElementDXAttribute) attribute will be "11 12 13 14 15 0 21 22 23 0 36 35 34 33 32 31". After this re-ordering, the glyphs corresponding to the characters will be positioned using standard left-to-right layout rules.

The following examples show basic use of the [‘tspan’](#TSpanElement) element.

<a id="ExampleTSpan01"></a>Example tspan01 uses a [‘tspan’](#TSpanElement) element to indicate that the word "not" is to use a bold font and have red fill.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example tspan01 - using tspan to change visual attributes</desc>

  <g font-family="Verdana" font-size="45" >
    <text x="200" y="150" fill="blue" >
      You are
        <tspan font-weight="bold" fill="red" >not</tspan>
      a banana.
    </text>
  </g>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example tspan01

| [image: Example tspan01 — using tspan to change visual attributes] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/tspan01.svg)

<a id="ExampleTSpan02"></a>Example tspan02 uses the [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) attributes on the [‘tspan’](#TSpanElement) element to adjust the [current text position](#CurrentTextPosition) horizontally and vertically for particular text strings within a [‘text’](#TextElement) element.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example tspan02 - using tspan's dx and dy attributes 
        for incremental positioning adjustments</desc>

  <g font-family="Verdana" font-size="45" >
    <text x="200" y="150" fill="blue" >
      But you
        <tspan dx="2em" dy="-50" font-weight="bold" fill="red" >
          are
        </tspan>
        <tspan dy="100">
           a peach!
        </tspan>
    </text>
  </g>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example tspan02

| [image: Example tspan02 — using tspan's dx and dy attributes for incremental positioning adjustments] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/tspan02.svg)

<a id="ExampleTSpan03"></a>Example tspan03 uses the [‘x’](#TSpanElementXAttribute) and [‘y’](#TSpanElementYAttribute) attributes on the [‘tspan’](#TSpanElement) element to establish a new absolute [current text position](#CurrentTextPosition) for each glyph to be rendered. The example shows two lines of text within a single [‘text’](#TextElement) element. Because both lines of text are within the same [‘text’](#TextElement) element, the user will be able to select through both lines of text and copy the text to the system clipboard in user agents that support [text selection and clipboard operations](#TextSelection).

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example tspan03 - using tspan's x and y attributes 
        for multiline text and precise glyph positioning</desc>

  <g font-family="Verdana" font-size="45" >
    <text fill="rgb(255,164,0)" >
      <tspan x="300 350 400 450 500 550 600 650" y="100">
        Cute and
      </tspan>
      <tspan x="375 425 475 525 575" y="200">
         fuzzy
      </tspan>
    </text>
  </g>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example tspan03

| [image: Example tspan03 — using tspan's x and y attributes for multiline text and precise glyph positioning] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/tspan03.svg)

Example tspan04 uses the [‘rotate’](#TSpanElementRotateAttribute) attribute on the [‘tspan’](#TSpanElement) element to rotate the glyphs to be rendered. This example shows a single text string in a [‘tspan’](#TSpanElement) element that contains more characters than the number of values specified in the [‘rotate’](#TSpanElementRotateAttribute) attribute. In this case the last value specified in the [‘rotate’](#TSpanElementRotateAttribute) attribute of the [‘tspan’](#TSpanElement) must be applied to the remaining characters in the string.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
  xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>
    Example tspan04 - The number of rotate values is less than the number of
    characters in the string.
  </desc>
  <text font-family="Verdana" font-size="55" fill="blue" >
    <tspan x="250" y="150" rotate="-30,0,30">
      Hello, out there
    </tspan>
  </text>
  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
  fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example tspan04

| [image: Example tspan04 — simple rotation of characters in a tspan element] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/tspan04.svg)

Example tspan05 specifies the [‘rotate’](#TSpanElementRotateAttribute) attribute on the [‘text’](#TextElement) element and on all but one of the child [‘tspan’](#TSpanElement) elements to rotate the glyphs to be rendered. The example demonstrates the propagation of the [‘rotate’](#TSpanElementRotateAttribute) attribute.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN"
"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="100%" height="100%" viewBox="0 0 500 120"
  xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>
    Example tspan05 - propagation of rotation values to nested tspan elements.
  </desc>
  <text id="parent" font-family="Arial, sans-serif" font-size="32" fill="red" x="40" y="40"
    rotate="5,15,25,35,45,55">
    Not

    <tspan id="child1" rotate="-10,-20,-30,-40" fill="orange">
      all characters

      <tspan id="child2" rotate="70,60,50,40,30,20,10" fill="yellow">
        in
        
        <tspan id="child3">
          the
        </tspan>
      </tspan>

      <tspan id="child4" fill="orange" x="40" y="90">
        text
      </tspan>

      have a
    </tspan>

    <tspan id="child5" rotate="-10" fill="blue">
      specified
    </tspan>

    rotation
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="498" height="118" fill="none"
        stroke="blue" stroke-width="2" />
</svg>
```

Example tspan05

| [image: Example tspan05 — propagation of rotation values to nested tspan elements] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/tspan05.svg)

Rotation of red text inside the [‘text’](#TextElement) element:

- The [‘rotate’](#TSpanElementRotateAttribute) value will rotate the characters in the text *"Not "* by 5, 15, 25 and 35 degrees respectively.
- A [‘rotate’](#TSpanElementRotateAttribute) value is applied to the space that follows the text *"Not"*, to the space in between the text in the "child1" and "child5" [‘tspan’](#TSpanElement) elements, and to the space before the text *"rotation"*.
- The next current [‘rotate’](#TSpanElementRotateAttribute) value specified is 45 followed by 55. The current [‘rotate’](#TSpanElementRotateAttribute) value in the [‘text’](#TextElement) element is incremented as subsequent characters in the text of the child [‘tspan’](#TSpanElement) elements are processed.
- The next immediate [‘tspan’](#TSpanElement) element specifies rotate values for the text, hence the current [‘rotate’](#TSpanElementRotateAttribute) value will change to the next value in the list (but is not used) as each character is processed until the last value of 55 degrees is reached.
- The last [‘rotate’](#TSpanElementRotateAttribute) value of 55 degrees will be applied to all the characters in the text *"rotation"*.

Rotation of the orange text inside the "child1" [‘tspan’](#TSpanElement) element:

- The [‘rotate’](#TSpanElementRotateAttribute) value will rotate the first 4 characters in the text *"all characters "* by -10, -20, -30 and -40 respectively.
- The last [‘rotate’](#TSpanElementRotateAttribute) value of -40 becomes the current [‘rotate’](#TSpanElementRotateAttribute) value and will be applied to all subsequent characters in the [‘tspan’](#TSpanElement) element and to any child [‘tspan’](#TSpanElement) elements that do not specify [‘rotate’](#TSpanElementRotateAttribute) values.
- The "child4" [‘tspan’](#TSpanElement) element does not specify any [‘rotate’](#TSpanElementRotateAttribute) values and thus uses the current [‘rotate’](#TSpanElementRotateAttribute) of its ancestor ("child1" [‘tspan’](#TSpanElement) element). All the characters in the text *"text"* specified within the "child4" [‘tspan’](#TSpanElement) element will be rotated by -40 degrees.
- The last [‘rotate’](#TSpanElementRotateAttribute) value of -40 degrees will be applied to all the characters in the text *"have a"*.
- A [‘rotate’](#TSpanElementRotateAttribute) value is applied to the space in between the text in the "child2" and "child4" [‘tspan’](#TSpanElement) elements, and to the space before the text *"have a"*.

Rotation of the yellow text inside the "child2" [‘tspan’](#TSpanElement) element:

- The [‘rotate’](#TSpanElementRotateAttribute) value will rotate the characters in the (yellow) text *"in "* by 70, 60, and 50 degrees respectively.
- A [‘rotate’](#TSpanElementRotateAttribute) value is applied to the space that follows the text *"in"*.
- There are more [‘rotate’](#TSpanElementRotateAttribute) values specified than characters, thus the additional [‘rotate’](#TSpanElementRotateAttribute) values will be applied to the "child3" [‘tspan’](#TSpanElement) element which does not specified any [‘rotate’](#TSpanElementRotateAttribute) values.
- The characters in the text *"the"* specified within the "child3" [‘tspan’](#TSpanElement) element will be rotated 40, 30 and 20 degrees respectively.

Rotation of the blue text inside the "child5" [‘tspan’](#TSpanElement) element:

- The [‘rotate’](#TSpanElementRotateAttribute) value will rotate all the characters in text *"specified"* by -10 degrees.
- Only one [‘rotate’](#TSpanElementRotateAttribute) value is specified and is thus applied to all characters in the [‘tspan’](#TSpanElement) element.

The following diagram illustrates how the rotation values propagate to [‘tspan’](#TSpanElement) elements nested withing a [‘text’](#TextElement) element

[image: Image that shows propagation of rotation values]

## <a id="TRefElement"></a>10.6 The ‘tref’ element

The textual content for a [‘text’](#TextElement) can be either character data directly embedded within the [‘text’](#TextElement) element or the character data content of a referenced element, where the referencing is specified with a [‘tref’](#TRefElement) element.

‘tref’

**Categories:**
: [Text content element](intro.md#TermTextContentElement), [text content child element](intro.md#TermTextContentChildElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [‘animate’](animate.md#AnimateElement)
- [‘animateColor’](animate.md#AnimateColorElement)
- [‘set’](animate.md#SetElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](#AlignmentBaselineProperty), [‘baseline-shift’](#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](#FontFamilyProperty), [‘font-size’](#FontSizeProperty), [‘font-size-adjust’](#FontSizeAdjustProperty), [‘font-stretch’](#FontStretchProperty), [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](#TextAnchorProperty), [‘text-decoration’](#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](#WordSpacingProperty), [‘writing-mode’](#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#TRefElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘xlink:href’](#TRefElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGTRefElement](#InterfaceSVGTRefElement)

*Attribute definitions:*

**<a id="TRefElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](intro.md#TermIRIReference) to an element whose character data content shall be used as character data for this [‘tref’](#TRefElement) element. [Animatable](animate.md#Animatable): yes.
All character data within the referenced element, including character data enclosed within additional markup, will be rendered.

The [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) attributes have the same meanings as for the [‘tspan’](#TSpanElement) element. The attributes are applied as if the [‘tref’](#TRefElement) element was replaced by a [‘tspan’](#TSpanElement) with the referenced character data (stripped of all supplemental markup) embedded within the hypothetical [‘tspan’](#TSpanElement) element.

<a id="ExampleTRef01"></a>Example tref01 shows how to use character data from a different element as the character data for a given [‘tspan’](#TSpanElement) element. The first [‘text’](#TextElement) element (with id="ReferencedText") will not draw because it is part of a [‘defs’](struct.md#DefsElement) element. The second [‘text’](#TextElement) element draws the string "Inline character data". The third [‘text’](#TextElement) element draws the string "Reference character data" because it includes a [‘tref’](#TRefElement) element which is a reference to element "ReferencedText", and that element's character data is "Referenced character data".

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="10cm" height="3cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs>
    <text id="ReferencedText">
      Referenced character data
    </text>
  </defs>
  <desc>Example tref01 - inline vs reference text content</desc>

  <text x="100" y="100" font-size="45" fill="blue" >
    Inline character data
  </text>
  <text x="100" y="200" font-size="45" fill="red" >
    <tref xlink:href="#ReferencedText"/>
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example tref01

| [image: Example tref01 — inline vs reference text content] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/tref01.svg)

## <a id="TextLayout"></a>10.7 Text layout

### <a id="TextLayoutIntroduction"></a>10.7.1 Text layout introduction

This section describes the text layout features supported by SVG, which includes support for various international writing directions, such as left-to-right (e.g., Latin scripts) and bidirectional (e.g., Hebrew or Arabic) and vertical (e.g., Asian scripts). The descriptions in this section assume straight line text (i.e., text that is either strictly horizontal or vertical with respect to the current user coordinate system). Subsequent sections describe the supplemental layout rules for [text on a path](#TextOnAPath).

SVG does not provide for automatic line breaks or word wrapping, which makes internationalized text layout for SVG relatively simpler than it is for languages which support formatting of multi-line text blocks.

<a id="ReferenceOrientation"></a>For each [‘text’](#TextElement) element, the SVG user agent determines the current reference orientation. For standard horizontal or vertical text (i.e., no text-on-a-path), the reference orientation is the vector pointing towards negative infinity in Y within the current user coordinate system. (Note: in the [initial coordinate system](coords.md#InitialCoordinateSystem), the reference orientation is up.) For [text on a path](#TextOnAPath), the reference orientation is reset with each character.

<a id="InlineProgressionDirection"></a>Based on the reference orientation and the value for property [‘writing-mode’](#WritingModeProperty), the SVG user agent determines the current inline-progression-direction. For left-to-right text, the inline-progression-direction points 90 degrees clockwise from the reference orientation vector. For right-to-left text, the inline progression points 90 degrees counter-clockwise from the reference orientation vector. For top-to-bottom text, the inline-progression-direction points 180 degrees from the reference orientation vector.

<a id="BlockProgressionDirection"></a>Based on the reference orientation and the value for property [‘writing-mode’](#WritingModeProperty), the SVG user agent determines the current block-progression-direction. For left-to-right and right-to-left text, the block-progression-direction points 180 degrees from the reference orientation vector because the only available horizontal [‘writing-mode’](#WritingModeProperty)s are lr-tb and rl-tb. For top-to-bottom text, the block-progression-direction always points 90 degrees counter-clockwise from the reference orientation vector because the only available top-to-bottom [‘writing-mode’](#WritingModeProperty) is tb-rl.

<a id="ShiftDirection"></a>The shift direction is the direction towards which the [baseline table](#FontsTablesBaselines) moves due to positive values for property [‘baseline-shift’](#BaselineShiftProperty). The shift direction is such that a positive value shifts the baseline table towards the topmost entry in the parent's [baseline table](#FontsTablesBaselines).

<a id="CurrentTextPosition"></a>In processing a given [‘text’](#TextElement) element, the SVG user agent keeps track of the current text position. The initial current text position is established by the [‘x’](#TextElementXAttribute) and [‘y’](#TextElementYAttribute) attributes on the [‘text’](#TextElement) element.

The current text position is adjusted after each glyph to establish a new current text position at which the next glyph shall be rendered. The adjustment to the current text position is based on the current [inline-progression-direction](#SettingInlineProgressionDirection), glyph-specific advance values corresponding to the [glyph orientation](#GlyphOrientation) of the glyph just rendered, kerning tables in the font and the current values of various attributes and properties, such as the [spacing properties](#SpacingProperties) and any [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) attributes on [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) elements. If a glyph does not provide explicit advance values corresponding to the current [glyph orientation](#GlyphOrientation), then an appropriate approximation should be used. For vertical text, a suggested approximation is the sum of the ascent and descent values for the glyph. Another suggested approximation for an advance value for both horizontal and vertical text is the size of an *em* (see [units-per-em](fonts.md#FontFaceElementUnitsPerEmAttribute)).

<a id="AlignmentPoint"></a>For each glyph to be rendered, the SVG user agent determines an appropriate alignment-point on the glyph which will be placed exactly at the current text position. The alignment-point is determined based on glyph cell metrics in the glyph itself, the current [inline-progression-direction](#SettingInlineProgressionDirection) and the [glyph orientation](#GlyphOrientation) relative to the inline-progression-direction. For most uses of Latin text (i.e., writing-mode:lr, text-anchor:start and alignment-baseline:baseline) the alignment-point in the glyph will be the intersection of left edge of the glyph cell (or some other glyph-specific x-axis coordinate indicating a left-side origin point) with the Latin baseline of the glyph. For many cases with top-to-bottom vertical text layout, the reference point will be either a glyph-specific origin point based on the set of vertical baselines for the font or the intersection of the center of the glyph with its *top line* (see Top Baseline; in [[CSS2](refs.md#ref-CSS2)], section 15.4.18). If a glyph does not provide explicit origin points corresponding to the current [glyph orientation](#GlyphOrientation), then an appropriate approximation should be used, such as the intersection of the left edge of the glyph with the appropriate horizontal baseline for the glyph or intersection of the top edge of the glyph with the appropriate vertical baseline. If baseline tables are not available, user agents should establish baseline tables that reflect common practice.

<a id="TextChunks"></a>Adjustments to the current text position are either absolute position adjustments or relative position adjustments. An absolute position adjustment occurs in the following circumstances:

- At the start of a [‘text’](#TextElement) element
- At the start of each [‘textPath’](#TextPathElement) element
- For each character within a [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) and [‘altGlyph’](#AltGlyphElement) element which has an ‘x’ or ‘y’ attribute value assigned to it explicitly

All other position adjustments to the current text position are relative position adjustments.

<a id="TextChunk"></a>Each absolute position adjustment defines a new text chunk. Absolute position adjustments impact text layout in the following ways:

- Ligatures only occur when a set of characters which might map to a ligature are all in the same text chunk.
- Each text chunk represents a separate block of text for alignment due to [‘text-anchor’](#TextAnchorProperty) property values.
- Reordering of characters due to [bidirectionality](#RelationshipWithBiDirectionality) only occurs within a text chunk. Reordering does *not* happen across text chunks.

The following additional rules apply to ligature formation:

- As defined in CSS2, ([[CSS2](refs.md#ref-CSS2)], section 16.4), when the resultant space between two characters is not the same as the default space, user agents should not use ligatures; thus, if there are non-default values for properties [‘kerning’](#KerningProperty) or [‘letter-spacing’](#LetterSpacingProperty), the user agent should not use ligatures.
- Ligature formation should not be enabled for the glyphs corresponding to characters within different DOM text nodes; thus, characters separated by markup should not use ligatures.
- As mentioned above, ligature formation should not be enabled for the glyphs corresponding to characters within different text chunks.

### <a id="SettingInlineProgressionDirection"></a>10.7.2 Setting the inline-progression-direction

The [‘writing-mode’](#WritingModeProperty) property specifies whether the initial inline-progression-direction for a [‘text’](#TextElement) element shall be left-to-right, right-to-left, or top-to-bottom. The [‘writing-mode’](#WritingModeProperty) property applies only to [‘text’](#TextElement) elements; the property is ignored for [‘tspan’](#TSpanElement), [‘tref’](#TRefElement), [‘altGlyph’](#AltGlyphElement) and [‘textPath’](#TextPathElement) sub-elements. (Note that the inline-progression-direction can change within a [‘text’](#TextElement) element due to the Unicode bidirectional algorithm and properties [‘direction’](#DirectionProperty) and [‘unicode-bidi’](#UnicodeBidiProperty). For more on bidirectional text, see [Relationship with bidirectionality](#RelationshipWithBiDirectionality).)

**<a id="WritingModeProperty"></a>‘writing-mode’**
:

| *Value:* | lr-tb \| rl-tb \| tb-rl \| lr \| rl \| tb \| inherit |
| --- | --- |
| *Initial:* | lr-tb |
| *Applies to:* | [‘text’](#TextElement) elements |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

**lr-tb | lr**
: Sets the initial inline-progression-direction to left-to-right, as is common in most Latin-based documents. For most characters, the *current text position* is advanced from left to right after each glyph is rendered. (When the character data includes characters which are subject to the Unicode bidirectional algorithm, the text advance rules are more complex. See [Relationship with bidirectionality](#RelationshipWithBiDirectionality)).
**rl-tb | rl**
: Sets the initial inline-progression-direction to right-to-left, as is common in Arabic or Hebrew scripts. (See [Relationship with bidirectionality](#RelationshipWithBiDirectionality).)
**tb-rl | tb**
: Sets the initial inline-progression-direction to top-to-bottom, as is common in some Asian scripts, such as Chinese and Japanese. Though hardly as frequent as horizontal, this type of vertical layout also occurs in Latin based documents, particularly in table column or row labels. In most cases, the vertical baselines running through the middle of each glyph are aligned.
### <a id="GlyphOrientation"></a>10.7.3 Glyph orientation within a text run

In some cases, it is required to alter the orientation of a sequence of characters relative to the inline-progression-direction. The requirement is particularly applicable to vertical layouts of East Asian documents, where sometimes narrow-cell Latin text is to be displayed horizontally and other times vertically.

Two properties control the glyph orientation relative to the reference orientation for each of the two possible inline-progression-directions. [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty) controls glyph orientation when the inline-progression-direction is vertical. [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty) controls glyph orientation when the inline-progression-direction is horizontal.

**<a id="GlyphOrientationVerticalProperty"></a>‘glyph-orientation-vertical’**
:

| *Value:* | auto \| [<angle>](types.md#DataTypeAngle) \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

**auto**
:

-
Fullwidth ideographic and fullwidth Latin text will be set with a glyph-orientation of 0-degrees.
Ideographic punctuation and other ideographic characters having alternate horizontal and vertical forms will use the vertical form of the glyph.
-
Text which is not fullwidth will be set with a glyph-orientation of 90-degrees.
This reorientation rule applies only to the first-level non-ideographic text. All further embedding of writing-modes or bidi processing will be based on the first-level rotation.
**NOTE:**
  -
  This is equivalent to having set the non-ideographic text string horizontally honoring the bidi-rule, then rotating the resultant sequence of inline-areas (one area for each change of glyph direction) 90-degrees clockwise.
  It should be noted that text set in this "rotated" manner may contain ligatures or other glyph combining and reordering common to the language and script. (This "rotated" presentation form does not disable auto-ligature formation or similar context-driven variations.)
  -
  The determination of which characters should be auto-rotated may vary across user agents. The determination is based on a complex interaction between country, language, script, character properties, font, and character context. It is suggested that one consult the Unicode TR 11 and the various JIS or other national standards.

**[<angle>](types.md#DataTypeAngle)**
: The value of the angle is restricted to 0, 90, 180, and 270 degrees. The user agent shall round the value of the angle to the closest of the permitted values. A value of 0deg indicates that all glyphs are set with the top of the glyphs oriented towards the [reference orientation](#ReferenceOrientation). A value of 90deg indicates an orientation of 90 degrees clockwise from the [reference orientation](#ReferenceOrientation).
This property is applied only to text written in a vertical [‘writing-mode’](#WritingModeProperty).

The glyph orientation affects the amount that the current text position advances as each glyph is rendered. When the inline-progression-direction is vertical and the [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty) results in an orientation angle that is a multiple of 180 degrees, then the current text position is incremented according to the vertical metrics of the glyph. Otherwise, if the [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty) results in an orientation angle that is not a multiple of 180 degrees, then the current text position is incremented according to the horizontal metrics of the glyph.

The text layout diagrams in this section use the following symbols:

| [image: Symbolic wide-cell glyph representation] | wide-cell glyph (e.g. Han) which is the *n*-th glyph in the text run |
| --- | --- |
| [image: Symbolic narrow-cell glyph representation] | narrow-cell glyph (e.g. Latin) which is the *n*-th glyph in the text run |

The orientation which the above symbols assume in the diagrams corresponds to the orientation that the Unicode characters they represent are intended to assume when rendered in the user agent. Spacing between the glyphs in the diagrams is usually symbolic, unless intentionally changed to make a point.

The diagrams below illustrate different uses of [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty). The diagram on the left shows the result of the mixing of full-width ideographic glyphs with narrow-cell Latin glyphs when [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty) for the Latin characters is either auto or 90. The diagram on the right show the result of mixing full-width ideographic glyphs with narrow-cell Latin glyphs when Latin glyphs are specified to have a [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty) of 0.

[image: Layout of mixed glyphs in vertical-ideographic mode. Wide-cell glyphs are upright, Non-wide-cell glyphs are rotated by 90 degrees.][image: Example of mixed Japanese and English in vertical-ideographic layout. Japanese glyphs are upright, English rotated.]                    [image: Layout of mixed glyphs in vertical mode. All glyphs are upright.][image: Example of mixed Japanese and English in vertical layout. All glyphs are upright.]

**<a id="GlyphOrientationHorizontalProperty"></a>‘glyph-orientation-horizontal’**
:

| *Value:* | [<angle>](types.md#DataTypeAngle) \| inherit |
| --- | --- |
| *Initial:* | 0deg |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

**[<angle>](types.md#DataTypeAngle)**
: The value of the angle is restricted to 0, 90, 180, and 270 degrees. The user agent shall round the value of the angle to the closest of the permitted values. A value of 0deg indicates that all glyphs are set with the top of the glyphs oriented towards the [reference orientation](#ReferenceOrientation). A value of 90deg indicates an orientation of 90 degrees clockwise from the [reference orientation](#ReferenceOrientation).
This property is applied only to text written in a horizontal [‘writing-mode’](#WritingModeProperty).

The glyph orientation affects the amount that the current text position advances as each glyph is rendered. When the reference orientation direction is horizontal and the [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty) results in an orientation angle that is a multiple of 180 degrees, then the current text position is incremented according to the horizontal metrics of the glyph. Otherwise, if the [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty) results in an orientation angle that is not a multiple of 180 degrees, then the current text position is incremented according to the vertical metrics of the glyph.

### <a id="RelationshipWithBiDirectionality"></a>10.7.4 Relationship with bidirectionality

The characters in certain scripts are written from right to left. In some documents, in particular those written with the Arabic or Hebrew script, and in some mixed-language contexts, text in a single line may appear with mixed directionality. This phenomenon is called bidirectionality, or "bidi" for short.

The Unicode standard ([[UNICODE](refs.md#ref-UNICODE)], specifically [[UAX9](refs.md#ref-UAX9)]) defines a complex algorithm for determining the proper directionality of text. The algorithm consists of an implicit part based on character properties, as well as explicit controls for embeddings and overrides. The [SVG user agent](intro.md#TermSVGUserAgent) applies this bidirectional algorithm when determining the layout of characters within a [text content block element](intro.md#TermTextContentBlockElement).

The [‘direction’](#DirectionProperty) and [‘unicode-bidi’](#DirectionProperty) properties allow authors to override the inherent directionality of the content characters and thus explicitly control how the elements and attributes of a document language map to this algorithm. These two properties are applicable to all characters whose glyphs are perpendicular to the inline-progression-direction.

In many cases, the bidirectional algorithm from Unicode [[UNICODE](refs.md#ref-UNICODE)] produces the desired result automatically, and in such cases the author does not need to use these properties. For other cases, such as when using right-to-left languages, it may be sufficient to add the [‘direction’](#DirectionProperty) property to the [rootmost ‘svg’ element](intro.md#TermRootmostSVGElement), and allow that direction to inherit to all text elements, as in the following example (which may be used as a template):

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="100%" height="100%" viewBox="0 0 400 400"
     direction="rtl" xml:lang="fa">

  <title direction="ltr" xml:lang="en">Right-to-left Text</title>
  <desc direction="ltr" xml:lang="en">
    A simple example for using the 'direction' property in documents
    that predominantly use right-to-left languages.
  </desc>

  <text x="200" y="200" font-size="20">داستان SVG 1.1 SE طولا ني است.</text>

</svg>
```

Example

| [image: Example] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/rtl-text.svg)

Below is another example, where where implicit bidi reordering is not sufficient:

```
<?xml version="1.0" encoding="utf-8"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width="100%" height="100%" viewBox="0 0 400 400"
     direction="rtl" xml:lang="he">

  <title direction="ltr" xml:lang="en">Right-to-left Text</title>
  <desc direction="ltr" xml:lang="en">
    An example for using the 'direction' and 'unicode-bidi' properties
    in documents that predominantly use right-to-left languages.
  </desc>

  <text x="200" y="200" font-size="20"> כתובת 
	MAC:&#x200F;
    	<tspan direction="ltr" unicode-bidi="embed">00-24-AF-2A-55-FC</tspan> 
	</text>

</svg>
```

Example

| [image: Example] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/rtl-complex.svg)

Within [text content elements](intro.md#TermTextContentElement), the alignment of text with regards to the [‘text-anchor’](#TextAnchorProperty) property is determined by the value of the [‘direction’](#DirectionProperty) property. For example, given a [‘text’](#TextElement) element with a [‘text-anchor’](#TextAnchorProperty) value of "end", for a [‘direction’](#DirectionProperty) value of "ltr", the text will extend to the left of the position of the [‘text’](#TextElement) element's [‘x’](#TextElementXAttribute) attribute value, while for [‘direction’](#DirectionProperty) value of "rtl", the text will extend to the right of the position of the [‘text’](#TextElement) element's [‘x’](#TextElementXAttribute) attribute value.

A more complete discussion of bidirectionality can be found in the Text direction section of CSS 2 ([[CSS2](refs.md#ref-CSS2)], section 9.10).

The processing model for bidirectional text is as follows. The user agent processes the characters which are provided in logical order (i.e., the order the characters appear in the original document, either via direct inclusion or via indirect reference due a [‘tref’](#TRefElement) element). The user agent determines the set of independent blocks within each of which it should apply the Unicode bidirectional algorithm. Each [text chunk](#TextChunk) represents an independent block of text. Additionally, any change in glyph orientation due to processing of properties [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty) or [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty) will subdivide the independent blocks of text further. After processing the Unicode bidirectional algorithm and properties [‘direction’](#DirectionProperty) and [‘unicode-bidi’](#UnicodeBidiProperty) on each of the independent text blocks, the user agent will have a potentially re-ordered list of characters which are now in left-to-right rendering order. Simultaneous with re-ordering of the characters, the [dx](#TSpanElementDXAttribute), [dy](#TSpanElementDYAttribute) and [rotate](#TSpanElementRotateAttribute) attributes on the [‘tspan’](#TSpanElement) and [‘tref’](#TRefElement) elements are also re-ordered to maintain the original correspondence between characters and attribute values. While kerning or ligature processing might be font-specific, the preferred model is that kerning and ligature processing occurs between combinations of characters or glyphs after the characters have been re-ordered.

**<a id="DirectionProperty"></a>‘direction’**
:

| *Value:* | ltr \| rtl \| inherit |
| --- | --- |
| *Initial:* | ltr |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

This property specifies the base writing direction of text and the direction of embeddings and overrides (see [‘unicode-bidi’](#UnicodeBidiProperty)) for the Unicode bidirectional algorithm. For the [‘direction’](#DirectionProperty) property to have any effect on an element that does not by itself establish a new [text chunk](#TextChunks) (such as a [‘tspan’](#TSpanElement) element without absolute position adjustments due to [‘x’](#TSpanElementXAttribute) or [‘y’](#TSpanElementYAttribute) attributes), the [‘unicode-bidi’](#UnicodeBidiProperty) property's value must be embed or bidi-override.

Except for any additional information provided in this specification, the normative definition of the [‘direction’](#DirectionProperty) property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 9.10).

The [‘direction’](#DirectionProperty) property applies only to glyphs oriented perpendicular to the [inline-progression-direction](#SettingInlineProgressionDirection), which includes the usual case of horizontally-oriented Latin or Arabic text and the case of narrow-cell Latin or Arabic characters rotated 90 degrees clockwise relative to a top-to-bottom inline-progression-direction.

**<a id="UnicodeBidiProperty"></a>‘unicode-bidi’**
:

| *Value:* | normal \| embed \| bidi-override \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

Except for any additional information provided in this specification, the normative definition of the [‘unicode-bidi’](#UnicodeBidiProperty) property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 9.10).

## <a id="TextRenderingOrder"></a>10.8 Text rendering order

The glyphs associated with the characters within a [‘text’](#TextElement) element are rendered in the logical order of the characters in the original document, independent of any re-ordering necessary to implement bidirectionality. Thus, for text that goes right-to-left visually, the glyphs associated with the rightmost character are rendered before the glyphs associated with the other characters.

Additionally, each distinct glyph is rendered in its entirety (i.e., it is filled and stroked as specified by the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties) before the next glyph gets rendered.

## <a id="AlignmentProperties"></a>10.9 Alignment properties

### <a id="TextAlignmentProperties"></a>10.9.1 Text alignment properties

The [‘text-anchor’](#TextAnchorProperty) property is used to align (start-, middle- or end-alignment) a string of text relative to a given point.

The [‘text-anchor’](#TextAnchorProperty) property is applied to each individual [text chunk](#TextChunks) within a given [‘text’](#TextElement) element. Each text chunk has an initial current text position, which represents the point in the user coordinate system resulting from (depending on context) application of the [‘x’](#TextElementXAttribute) and [‘y’](#TextElementYAttribute) attributes on the [‘text’](#TextElement) element, any [‘x’](#TSpanElementXAttribute) or [‘y’](#TSpanElementYAttribute) attribute values on a [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) element assigned explicitly to the first rendered character in a text chunk, or determination of the initial current text position for a [‘textPath’](#TextPathElement) element.

**<a id="TextAnchorProperty"></a>‘text-anchor’**
:

| *Value:* | start \| middle \| end \| inherit |
| --- | --- |
| *Initial:* | start |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

Values have the following meanings:

**start**
: The rendered characters are aligned such that the start of the resulting rendered text is at the initial current text position. For an element with a [‘direction’](#DirectionProperty) property value of "ltr" (typical for most European languages), the left side of the text is rendered at the initial text position. For an element with a [‘direction’](#DirectionProperty) property value of "rtl" (typical for Arabic and Hebrew), the right side of the text is rendered at the initial text position. For an element with a vertical primary text direction (often typical for Asian text), the top side of the text is rendered at the initial text position.
**middle**
: The rendered characters are aligned such that the geometric middle of the resulting rendered text is at the initial current text position.
**end**
: The rendered characters are aligned such that the end of the resulting rendered text is at the initial current text position. For an element with a [‘direction’](#DirectionProperty) property value of "ltr" (typical for most European languages), the right side of the text is rendered at the initial text position. For an element with a [‘direction’](#DirectionProperty) property value of "rtl" (typical for Arabic and Hebrew), the left side of the text is rendered at the initial text position. For an element with a vertical primary text direction (often typical for Asian text), the bottom of the text is rendered at the initial text position.
### <a id="BaselineAlignmentProperties"></a>10.9.2 Baseline alignment properties

An overview of baseline alignment and baseline tables can be found above in [Fonts, font tables and baselines](#FontsTablesBaselines).

One of the characteristics of international text is that there are different baselines (different alignment points) for glyphs in different scripts. For example, in horizontal writing, ideographic scripts, such as Han Ideographs, Katakana, Hiragana, and Hangul, alignment occurs with a baseline near the bottoms of the glyphs; alphabetic based scripts, such as Latin, Cyrillic, Hebrew, Arabic, align a point that is the bottom of most glyphs, but some glyphs descend below the baseline; and Indic based scripts are aligned at a point that is near the top of the glyphs.

When different scripts are mixed on a line of text, an adjustment must be made to ensure that the glyphs in the different scripts are aligned correctly with one another. OpenType [OPENTYPE] fonts have a Baseline table (BASE) [OPENTYPE-BASETABLE] that specifies the offsets of the alternative baselines from the current baseline.

SVG uses a similar baseline table model that assumes one script (at one font-size) is the "dominant run" during processing of a [‘text’](#TextElement) element; that is, all other baselines are defined in relation to this dominant run. The baseline of the script with the dominant run is called the dominant baseline. So, for example, if the dominant baseline is the alphabetic baseline, there will be offsets in the baseline table for the alternate baselines, such as the ideographic baseline and the Indic baseline. There will also be an offset for the math baseline which is used for some math fonts. Note that there are separate baseline tables for horizontal and vertical writing-modes. The offsets in these tables may be different for horizontal and vertical writing.

The baseline table established at the start of processing of a [‘text’](#TextElement) element is called the dominant baseline table.

Because the value of the [‘font-family’](#FontFamilyProperty) property is a list of fonts, to insure a consistent choice of baseline table we define the *nominal font* in a font list as the first font in the list for which a glyph is available. This is the first font that could contain a glyph for each character encountered. (For this definition, glyph data is assumed to be present if a font substitution is made or if the font is synthesized.) This definition insures a content independent determination of the font and baseline table that is to be used.

The value of the [‘font-size’](#FontSizeProperty) property on the [‘text’](#TextElement) element establishes the dominant baseline table font size.

The model assumes that each glyph has a 'alignment-baseline' value which specifies the baseline with which the glyph is to be aligned. (The 'alignment-baseline' is called the "Baseline Tag" in the OpenType baseline table description.) The initial value of the [‘alignment-baseline’](#AlignmentBaselineProperty) property uses the baseline identifier associated with the given glyph. Alternate values for [‘alignment-baseline’](#AlignmentBaselineProperty) can be useful for glyphs such as a "*" which are ambiguous with respect to script membership.

The model assumes that the font from which the glyph is drawn also has a baseline table, the font baseline table. This baseline table has offsets in units-per-em from the (0,0) point to each of the baselines the font knows about. In particular, it has the offset from the glyph's (0,0) point to the baseline identified by the 'alignment-baseline'.

The offset values in the baseline table are in "design units" which means fractional units of the EM. CSS calls these "units-per-em" ([[CSS2](refs.md#ref-CSS2)], section 15.3.4). Thus, the current [‘font-size’](#FontSizeProperty) is used to determine the actual offset from the dominant baseline to the alternate baselines.

The glyph is aligned so that its baseline identified by its 'alignment-baseline' is aligned with the baseline with the same name from the dominant baseline table.

The offset from the dominant baseline of the parent to the baseline identified by the 'alignment-baseline' is computed using the dominant baseline table and dominant baseline table font size. The font baseline table and font size applicable to the glyph are used to compute the offset from the identified baseline to the (0,0) point of the glyph. This second offset is subtracted from the first offset to get the position of the (0,0) point in the [shift direction](#ShiftDirection). Both offsets are computed by multiplying the baseline value from the baseline table times the appropriate font size value.

If the 'alignment-baseline' identifies the dominant baseline, then the first offset is zero and the glyph is aligned with the dominant baseline; otherwise, the glyph is aligned with the chosen alternate baseline.

The baseline-identifiers below are used in this specification. Some of these are determined by baseline-tables contained in a font as described in XSL ([[XSL](refs.md#ref-XSL)], section 7.9.1). Others are computed from other font characteristics as described below.

****alphabetic****
:

This identifies the baseline used by most alphabetic and syllabic scripts. These include, but are not limited to, many Western, Southern Indic, Southeast Asian (non-ideographic) scripts.

****ideographic****
:

This identifies the baseline used by ideographic scripts. For historical reasons, this baseline is at the bottom of the ideographic EM box and not in the center of the ideographic EM box. See the "central" baseline. The ideographic scripts include Chinese, Japanese, Korean, and Vietnamese Chu Nom.

****hanging****
:

This identifies the baseline used by certain Indic scripts. These scripts include Devanagari, Gurmukhi and Bengali.

****mathematical****
:

This identifies the baseline used by mathematical symbols.

****central****
:

This identifies a computed baseline that is at the center of the EM box. This baseline lies halfway between the text-before-edge and text-after-edge baselines.

**NOTE:**

For ideographic fonts, this baseline is often used to align the glyphs; it is an alternative to the ideographic baseline.

****middle****
:

This identifies a baseline that is offset from the alphabetic baseline in the **shift-direction** by 1/2 the value of the x-height font characteristic. The position of this baseline may be obtained from the font data or, for fonts that have a font characteristic for "x-height", it may be computed using 1/2 the "x-height". Lacking either of these pieces of information, the position of this baseline may be approximated by the "central" baseline.

****text-before-edge****
:

This identifies the before-edge of the EM box. The position of this baseline may be specified in the baseline-table or it may be calculated.

**NOTE:**

The position of this baseline is normally around or at the top of the ascenders, but it may not encompass all accents that can appear above a glyph. For these fonts the value of the "ascent" font characteristic is used. For ideographic fonts, the position of this baseline is normally 1 EM in the **shift-direction** from the "ideographic" baseline. However, some ideographic fonts have a reduced width in the inline-progression-direction to allow tighter setting. When such a font, designed only for vertical writing-modes, is used in a horizontal writing-mode, the "text-before-edge" baseline may be less than 1 EM from the text-after-edge.

****text-after-edge****
:

This identifies the after-edge of the EM box. The position of this baseline may be specified in the baseline-table or it may be calculated.

**NOTE:**

For fonts with descenders, the position of this baseline is normally around or at the bottom of the descenders. For these fonts the value of the "descent" font characteristic is used. For ideographic fonts, the position of this baseline is normally at the "ideographic" baseline.

There are, in addition, two computed baselines that are only defined for line areas. Since SVG does not support the notion of computations based on line areas, the two computed baselines are mapped as follows:

****before-edge****
: For SVG, this is equivalent to **text-before-edge**.
****after-edge****
: For SVG, this is equivalent to **text-after-edge**.
There are also four baselines that are defined only for horizontal writing-modes.

****top****
:

This baseline is the same as the "before-edge" baseline in a horizontal writing-mode and is undefined in a vertical writing mode.

**<a id="TextTopBaselineDefinition"></a>**text-top****
:

This baseline is the same as the "text-before-edge" baseline in a horizontal writing-mode and is undefined in a vertical writing mode.

****bottom****
:

This baseline is the same as the "after-edge" baseline in a horizontal writing-mode and is undefined in a vertical writing mode.

**<a id="TextBottomBaselineDefinition"></a>**text-bottom****
:

This baseline is the same as the "text-after-edge" baseline in a horizontal writing-mode and is undefined in a vertical writing mode.

The baseline-alignment properties follow.

**<a id="DominantBaselineProperty"></a>‘dominant-baseline’**
:

| *Value:* | auto \| use-script \| no-change \| reset-size \| ideographic \| alphabetic \| hanging \| mathematical \| central \| middle \| text-after-edge \| text-before-edge \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The "dominant-baseline" property is used to determine or re-determine a scaled-baseline-table. A scaled-baseline-table is a compound value with three components: a baseline-identifier for the dominant-baseline, a baseline-table and a baseline-table font-size. Some values of the property re-determine all three values; other only re-establish the baseline-table font-size. When the initial value, auto, would give an undesired result, this property can be used to explicitly set the desire scaled-baseline-table.

Values for the property have the following meaning:

**auto**
:

If this property occurs on a [‘text’](#TextElement) element, then the computed value depends on the value of the [‘writing-mode’](#WritingModeProperty) property. If the 'writing-mode' is horizontal, then the value of the dominant-baseline component is 'alphabetic', else if the 'writing-mode' is vertical, then the value of the dominant-baseline component is 'central'.

If this property occurs on a [‘tspan’](#TSpanElement), [‘tref’](#TRefElement), [‘altGlyph’](#AltGlyphElement) or [‘textPath’](#TextPathElement) element, then the dominant-baseline and the baseline-table components remain the same as those of the parent [text content element](intro.md#TermTextContentElement). If the computed [‘baseline-shift’](#BaselineShiftProperty) value actually shifts the baseline, then the baseline-table font-size component is set to the value of the [‘font-size’](#FontSizeProperty) property on the element on which the [‘dominant-baseline’](#DominantBaselineProperty) property occurs, otherwise the baseline-table font-size remains the same as that of the element. If there is no parent [text content element](intro.md#TermTextContentElement), the scaled-baseline-table value is constructed as above for [‘text’](#TextElement) elements.

**use-script**
: The dominant-baseline and the baseline-table components are set by determining the predominant script of the character data content. The [‘writing-mode’](#WritingModeProperty), whether horizontal or vertical, is used to select the appropriate set of baseline-tables and the dominant baseline is used to select the baseline-table that corresponds to that baseline. The baseline-table font-size component is set to the value of the [‘font-size’](#FontSizeProperty) property on the element on which the [‘dominant-baseline’](#DominantBaselineProperty) property occurs.
**no-change**
: The dominant-baseline, the baseline-table, and the baseline-table font-size remain the same as that of the parent [text content element](intro.md#TermTextContentElement).
**reset-size**
: The dominant-baseline and the baseline-table remain the same, but the baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element. This re-scales the baseline-table for the current [‘font-size’](#FontSizeProperty).
**ideographic**
: The baseline-identifier for the dominant-baseline is set to be 'ideographic', the derived baseline-table is constructed using the 'ideographic' baseline-table in the nominal font, and the baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element.
**alphabetic**
: The baseline-identifier for the dominant-baseline is set to be 'alphabetic', the derived baseline-table is constructed using the 'alphabetic' baseline-table in the nominal font, and the baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element.
**hanging**
: The baseline-identifier for the dominant-baseline is set to be 'hanging', the derived baseline-table is constructed using the 'hanging' baseline-table in the nominal font, and the baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element.
**mathematical**
: The baseline-identifier for the dominant-baseline is set to be 'mathematical', the derived baseline-table is constructed using the 'mathematical' baseline-table in the nominal font, and the baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element.
**central**
: The baseline-identifier for the dominant-baseline is set to be 'central'. The derived baseline-table is constructed from the defined baselines in a baseline-table in the nominal font. That font baseline-table is chosen using the following priority order of baseline-table names: 'ideographic', 'alphabetic', 'hanging', 'mathematical'. The baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element.
**middle**
: The baseline-identifier for the dominant-baseline is set to be 'middle'. The derived baseline-table is constructed from the defined baselines in a baseline-table in the nominal font. That font baseline -table is chosen using the following priority order of baseline-table names: 'alphabetic', 'ideographic', 'hanging', 'mathematical'. The baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element.
**text-after-edge**
: The baseline-identifier for the dominant-baseline is set to be 'text-after-edge'. The derived baseline-table is constructed from the defined baselines in a baseline-table in the nominal font. The choice of which font baseline-table to use from the baseline-tables in the nominal font is implementation defined. The baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element. NOTE: using the following priority order of baseline-table names: 'alphabetic', 'ideographic', 'hanging', 'mathematical' is probably a reasonable strategy for determining which font baseline-table to use.
**text-before-edge**
: The baseline-identifier for the dominant-baseline is set to be 'text-before-edge'. The derived baseline-table is constructed from the defined baselines in a baseline-table in the nominal font. The choice of which baseline-table to use from the baseline-tables in the nominal font is implementation defined. The baseline-table font-size is changed to the value of the [‘font-size’](#FontSizeProperty) property on this element. NOTE: Using the following priority order of baseline-table names: 'alphabetic', 'ideographic', 'hanging', 'mathematical' is probably a reasonable strategy for determining which font baseline-table to use.
If there is no baseline table in the nominal font or if the baseline table lacks an entry for the desired baseline, then the user agent may use heuristics to determine the position of the desired baseline.

**<a id="AlignmentBaselineProperty"></a>‘alignment-baseline’**
:

| *Value:* | auto \| baseline \| before-edge \| text-before-edge \| middle \| central \| after-edge \| text-after-edge \| ideographic \| alphabetic \| hanging \| mathematical \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [‘tspan’](#TSpanElement), [‘tref’](#TRefElement), [‘altGlyph’](#AltGlyphElement), [‘textPath’](#TextPathElement) elements |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property specifies how an object is aligned with respect to its parent. This property specifies which baseline of this element is to be aligned with the corresponding baseline of the parent. For example, this allows alphabetic baselines in Roman text to stay aligned across font size changes. It defaults to the baseline with the same name as the computed value of the alignment-baseline property. That is, the position of "ideographic" alignment-point in the **block-progression-direction** is the position of the "ideographic" baseline in the baseline-table of the object being aligned.

Values have the following meanings:

****auto****
: The value is the dominant-baseline of the script to which the character belongs - i.e., use the dominant-baseline of the parent.
****baseline****
: The alignment-point of the object being aligned is aligned with the dominant-baseline of the parent [text content element](intro.md#TermTextContentElement).
****before-edge****
: The alignment-point of the object being aligned is aligned with the "before-edge" baseline of the parent [text content element](intro.md#TermTextContentElement).
****text-before-edge****
: The alignment-point of the object being aligned is aligned with the "text-before-edge" baseline of the parent [text content element](intro.md#TermTextContentElement).
****middle****
: The alignment-point of the object being aligned is aligned with the "middle" baseline of the parent [text content element](intro.md#TermTextContentElement).
****central****
: The alignment-point of the object being aligned is aligned with the "central" baseline of the parent [text content element](intro.md#TermTextContentElement).
****after-edge****
: The alignment-point of the object being aligned is aligned with the "after-edge" baseline of the parent [text content element](intro.md#TermTextContentElement).
****text-after-edge****
: The alignment-point of the object being aligned is aligned with the "text-after-edge" baseline of the parent [text content element](intro.md#TermTextContentElement).
****ideographic****
: The alignment-point of the object being aligned is aligned with the "ideographic" baseline of the parent [text content element](intro.md#TermTextContentElement).
****alphabetic****
: The alignment-point of the object being aligned is aligned with the "alphabetic" baseline of the parent [text content element](intro.md#TermTextContentElement).
****hanging****
: The alignment-point of the object being aligned is aligned with the "hanging" baseline of the parent [text content element](intro.md#TermTextContentElement).
****mathematical****
: The alignment-point of the object being aligned is aligned with the "mathematical" baseline of the parent [text content element](intro.md#TermTextContentElement).
**<a id="BaselineShiftProperty"></a>‘baseline-shift’**
:

| *Value:* | baseline \| sub \| super \| [<percentage>](types.md#DataTypePercentage) \| [<length>](types.md#DataTypeLength) \| inherit |
| --- | --- |
| *Initial:* | baseline |
| *Applies to:* | [‘tspan’](#TSpanElement), [‘tref’](#TRefElement), [‘altGlyph’](#AltGlyphElement), [‘textPath’](#TextPathElement) elements |
| *Inherited:* | no |
| *Percentages:* | refers to the "line-height" of the [‘text’](#TextElement) element, which in the case of SVG is defined to be equal to the [‘font-size’](#FontSizeProperty) |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘baseline-shift’](#BaselineShiftProperty) property allows repositioning of the dominant-baseline relative to the dominant-baseline of the parent [text content element](intro.md#TermTextContentElement). The shifted object might be a sub- or superscript. Within the shifted object, the whole baseline-table is offset; not just a single baseline. The amount of the shift is determined from information from the parent [text content element](intro.md#TermTextContentElement), the sub- or superscript offset from the nominal font of the parent [text content element](intro.md#TermTextContentElement), percent of the "line-height" of the parent [text content element](intro.md#TermTextContentElement) or an absolute value.

In SVG, the [‘baseline-shift’](#BaselineShiftProperty) property represents a supplemental adjustment to the baseline tables. The [‘baseline-shift’](#BaselineShiftProperty) property shifts the baseline tables for each glyph to temporary new positions, for example to lift the glyph into superscript or subscript position, but it does not effect the current text position. When the current text position is adjusted after rendering a glyph to take into account glyph advance values, the adjustment happens as if there were no baseline shift.

[‘baseline-shift’](#BaselineShiftProperty) properties can nest. Each nested [‘baseline-shift’](#BaselineShiftProperty) is added to previous baseline shift values.

Values for the property have the following meaning:

****baseline****
: There is no baseline shift; the dominant-baseline remains in its original position.
****sub****
: The dominant-baseline is shifted to the default position for subscripts. The offset to this position is determined using the font data for the nominal font. Because in most fonts the subscript position is normally given relative to the "alphabetic" baseline, the user agent may compute the effective position for subscripts for superscripts when some other baseline is dominant. The suggested computation is to subtract the difference between the position of the dominant baseline and the position of the "alphabetic" baseline from the position of the subscript. The resulting offset is determined by multiplying the effective subscript position by the dominant baseline-table font-size. If there is no applicable font data the user agent may use heuristics to determine the offset.
****super****
: The dominant-baseline is shifted to the default position for superscripts. The offset to this position is determined using the font data for the nominal font. Because in most fonts the superscript position is normally given relative to the "alphabetic" baseline, the user agent may compute the effective position for superscripts when some other baseline is dominant. The suggested computation is to subtract the difference between the position of the dominant baseline and the position of the "alphabetic" baseline from the position of the superscript. The resulting offset is determined by multiplying the effective superscript position by the dominant baseline-table font-size. If there is no applicable font data the user agent may use heuristics to determine the offset.
****<percentage>****
: The computed value of the property is this percentage multiplied by the computed "line-height" of the [‘text’](#TextElement) element. The dominant-baseline is shifted in the [shift direction](#ShiftDirection) (positive value) or opposite to the [shift direction](#ShiftDirection) (negative value) of the parent [text content element](intro.md#TermTextContentElement) by the computed value. A value of "0%" is equivalent to "baseline".
****<length>****
: The dominant-baseline is shifted in the [shift direction](#ShiftDirection) (positive value) or opposite to the [shift direction](#ShiftDirection) (negative value) of the parent [text content element](intro.md#TermTextContentElement) by the <length> value. A value of "0cm" is equivalent to "baseline".
## <a id="FontPropertiesUsedBySVG"></a>10.10 Font selection properties

SVG uses the following font specification properties. Except for any additional information provided in this specification, the normative definition of these properties is in CSS2 ([[CSS2](refs.md#ref-CSS2)], chapter section 15.2). Any SVG-specific notes about these properties are contained in the descriptions below.

Note also the rules for expressing the syntax of CSS property values ([[CSS2](refs.md#ref-CSS2)], section 1.3.2).

**<a id="FontFamilyProperty"></a>‘font-family’**
:

| *Value:* | [[ <family-name> \| <generic-family> ],]* [<family-name> \| <generic-family>] \| inherit |
| --- | --- |
| *Initial:* | depends on user agent |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property indicates which font family is to be used to render the text, specified as a prioritized list of font family names and/or generic family names. Unless the family name corresponds to a CSS IDENT, it must be quoted. Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.2).

**<a id="FontStyleProperty"></a>‘font-style’**
:

| *Value:* | normal \| italic \| oblique \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property specifies whether the text is to be rendered using a normal, italic or oblique face. Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.3).

**<a id="FontVariantProperty"></a>‘font-variant’**
:

| *Value:* | normal \| small-caps \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property indicates whether the text is to be rendered using the normal glyphs for lowercase characters or using small-caps glyphs for lowercase characters. Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.3).

**<a id="FontWeightProperty"></a>‘font-weight’**
:

| *Value:* | normal \| bold \| bolder \| lighter \| 100 \| 200 \| 300 \| 400 \| 500 \| 600 \| 700 \| 800 \| 900 \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property refers to the boldness or lightness of the glyphs used to render the text, relative to other fonts in the same font family. Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.3).

**<a id="FontStretchProperty"></a>‘font-stretch’**
:

| *Value:* | normal \| wider \| narrower \| ultra-condensed \| extra-condensed \| condensed \| semi-condensed \| semi-expanded \| expanded \| extra-expanded \| ultra-expanded \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property indicates the desired amount of condensing or expansion in the glyphs used to render the text. Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.3).

**<a id="FontSizeProperty"></a>‘font-size’**
:

| *Value:* | <absolute-size> \| <relative-size> \| <length> \| <percentage> \| inherit |
| --- | --- |
| *Initial:* | medium |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes, the computed value is inherited |
| *Percentages:* | refer to parent element's font size |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property refers to the size of the font from baseline to baseline when multiple lines of text are set solid in a multiline layout environment. For SVG, if a <length> is provided without a unit identifier (e.g., an unqualified number such as 128), the SVG user agent processes the <length> as a height value in the current user coordinate system.

If a <length> is provided with one of the [unit identifiers](coords.md#Units) (e.g., 12pt or 10%), then the SVG user agent converts the <length> into a corresponding value in the current user coordinate system by applying the rules described in [Units](coords.md#Units).

Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.4).

**<a id="FontSizeAdjustProperty"></a>‘font-size-adjust’**
:

| *Value:* | <number> \| none \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes (non-additive) |

This property allows authors to specify an aspect value for an element that will preserve the x-height of the first choice font in a substitute font. Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.4).

**<a id="FontProperty"></a>‘font’**
:

| *Value:* | [ [ <'font-style'> \|\| <'font-variant'> \|\| <'font-weight'> ]? <'font-size'> [ / <'line-height'> ]? <'font-family'> ] \| caption \| icon \| menu \| message-box \| small-caption \| status-bar \| inherit |
| --- | --- |
| *Initial:* | see individual properties |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | allowed on 'font-size' and 'line-height' (Note: for the purposes of processing the [‘font’](#FontProperty) property in SVG, 'line-height' is assumed to be equal the value for property [‘font-size’](#FontSizeProperty)) |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes (non-additive) |

Shorthand property for setting [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘font-size’](#FontSizeProperty), ‘line-height’ and [‘font-family’](#FontFamilyProperty). The ‘line-height’ property has no effect on text layout in SVG. For the purposes of the [‘font’](#FontProperty) property, ‘line-height’ is assumed to be equal to the value of the [‘font-size’](#FontSizeProperty) property. [Conforming SVG Viewers](conform.md#ConformingSVGViewers) are not required to support the various system font options (caption, icon, menu, message-box, small-caption and status-bar) and can use a system font or one of the generic fonts instead.

Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.2.5).

## <a id="SpacingProperties"></a>10.11 Spacing properties

Three properties affect the space between characters and words:

- [‘kerning’](#KerningProperty) indicates whether the user agent should adjust inter-glyph spacing based on kerning tables that are included in the relevant font (i.e., enable auto-kerning) or instead disable auto-kerning and instead set inter-character spacing to a specific length (typically, zero).
- [‘letter-spacing’](#LetterSpacingProperty) indicates an amount of space that is to be added between text characters supplemental to any spacing due to the [‘kerning’](#KerningProperty) property.
- [‘word-spacing’](#WordSpacingProperty) indicates the spacing behavior between words.

**<a id="KerningProperty"></a>‘kerning’**
:

| *Value:* | auto \| <length> \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The value of auto indicates that the user agent should adjust inter-glyph spacing based on kerning tables that are included in the font that will be used (i.e., enable auto-kerning).

If a <length> is provided, then auto-kerning is disabled. Instead, inter-character spacing is set to the given <length>. The most common scenario, other than auto, is to set [‘kerning’](#KerningProperty) to a value of 0 so that auto-kerning is disabled.

If a <length> is provided without a unit identifier (e.g., an unqualified number such as 128), the SVG user agent processes the <length> as a width value in the current user coordinate system.

If a <length> is provided with one of the [unit identifiers](coords.md#Units) (e.g., .25em or 1%), then the SVG user agent converts the <length> into a corresponding value in the current user coordinate system by applying the rules described in [Units](coords.md#Units).

When a <length> is provided, its value is added to the inter-character spacing value specified by the [‘letter-spacing’](#LetterSpacingProperty) property.

**<a id="LetterSpacingProperty"></a>‘letter-spacing’**
:

| *Value:* | normal \| <length> \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property specifies spacing behavior between text characters supplemental to any spacing due to the [‘kerning’](#KerningProperty) property.

For SVG, if a <length> is provided without a unit identifier (e.g., an unqualified number such as 128), the SVG user agent processes the <length> as a width value in the current user coordinate system.

If a <length> is provided with one of the [unit identifiers](coords.md#Units) (e.g., .25em or 1%), then the SVG user agent converts the <length> into a corresponding value in the current user coordinate system by applying the rules described in [Units](coords.md#Units).

Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 16.4).

**<a id="WordSpacingProperty"></a>‘word-spacing’**
:

| *Value:* | normal \| <length> \| inherit |
| --- | --- |
| *Initial:* | normal |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property specifies spacing behavior between words. For SVG, if a <length> is provided without a unit identifier (e.g., an unqualified number such as 128), the SVG user agent processes the <length> as a width value in the current user coordinate system.

If a <length> is provided with one of the [unit identifiers](coords.md#Units) (e.g., .25em or 1%), then the SVG user agent converts the <length> into a corresponding value in the current user coordinate system by applying the rules described in [Units](coords.md#Units).

Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 16.4).

## <a id="TextDecorationProperties"></a>10.12 Text decoration

**<a id="TextDecorationProperty"></a>‘text-decoration’**
:

| *Value:* | none \| [ underline \|\| overline \|\| line-through \|\| blink ] \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | no (see prose) |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property describes decorations that are added to the text of an element. [Conforming SVG Viewers](conform.md#ConformingSVGViewers) are not required to support the **blink** value.

Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 16.3.1).

The CSS2 specification defines the behavior of the [‘text-decoration’](#TextDecorationProperty) property using the terminology "block-level elements" and "inline elements". For the purposes of the [‘text-decoration’](#TextDecorationProperty) property and SVG, a [‘text’](#TextElement) element represents a block-level element and any of the potential children of a [‘text’](#TextElement) element (e.g., a [‘tspan’](#TSpanElement)) represent inline elements.

Also, the CSS2 definition of [‘text-decoration’](#TextDecorationProperty) specifies that the "color of the decorations" remain the same on descendant elements. Since SVG offers a painting model consisting of the ability to apply various types of paint (see [Painting: Filling, Stroking and Marker Symbols](painting.md)) to both the interior (i.e., the "fill") and the outline (i.e., the "stroke") of text, for SVG the [‘text-decoration’](#TextDecorationProperty) property is defined such that, for an element which has a specified value for the [‘text-decoration’](#TextDecorationProperty) property, all decorations on its content and that of its descendants are rendered using the same fill and stroke properties as are present on the given element. If the [‘text-decoration’](#TextDecorationProperty) property is specified on a descendant, then that overrides the ancestor.

Because SVG allows text to be both filled and stroked, drawing order matters in some circumstances with text decorations. Text decoration drawing order should be as follows:

- All text decorations except line-through should be drawn before the text is filled and stroked; thus, the text is rendered on top of these decorations.
- Line-through should be drawn after the text is filled and stroked; thus, the line-through is rendered on top of the text.

<a id="ExampleTextDecoration01"></a>

Example textdecoration01 provides examples for [‘text-decoration’](#TextDecorationProperty). The first line of text has no value for [‘text-decoration’](#TextDecorationProperty), so the initial value of text-decoration:none is used. The second line shows text-decoration:line-through. The third line shows text-decoration:underline. The fourth line illustrates the rule whereby decorations are rendered using the same fill and stroke properties as are present on the element for which the [‘text-decoration’](#TextDecorationProperty) is specified. Since [‘text-decoration’](#TextDecorationProperty) is specified on the [‘text’](#TextElement) element, all text within the [‘text’](#TextElement) element has its underline rendered with the same fill and stroke properties as exist on the [‘text’](#TextElement) element (i.e., blue fill, red stroke), even though the various words have different fill and stroke property values. However, the word "different" explicitly specifies a value for [‘text-decoration’](#TextDecorationProperty); thus, its underline is rendered using the fill and stroke properties as the [‘tspan’](#TSpanElement) element that surrounds the word "different" (i.e., yellow fill, darkgreen stroke):

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example textdecoration01 - behavior of 'text-decoration' property</desc>
  <rect x="1" y="1" width="1198" height="398" fill="none" stroke="blue" stroke-width="2" />
  <g font-size="60" fill="blue" stroke="red" stroke-width="1" >
    <text x="100" y="75">Normal text</text>
    <text x="100" y="165" text-decoration="line-through" >Text with line-through</text>
    <text x="100" y="255" text-decoration="underline" >Underlined text</text>
    <text x="100" y="345" text-decoration="underline" >
      <tspan>One </tspan>
      <tspan fill="yellow" stroke="purple" >word </tspan>
      <tspan fill="yellow" stroke="black" >has </tspan>
      <tspan fill="yellow" stroke="darkgreen" text-decoration="underline" >different </tspan>
      <tspan fill="yellow" stroke="blue" >underlining</tspan>
    </text>
  </g>
</svg>
```

Example textdecoration01

| [image: Example textdecoration01 — behavior of 'text-decoration' property] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/textdecoration01.svg)

## <a id="TextOnAPath"></a>10.13 Text on a path

### <a id="TextOnAPathIntroduction"></a>10.13.1 Introduction to text on a path

In addition to text drawn in a straight line, SVG also includes the ability to place text along the shape of a [‘path’](paths.md#PathElement) element. To specify that a block of text is to be rendered along the shape of a [‘path’](paths.md#PathElement), include the given text within a [‘textPath’](#TextPathElement) element which includes an [‘xlink:href’](#TextPathElementHrefAttribute) attribute with an [IRI reference](intro.md#TermIRIReference) to a [‘path’](paths.md#PathElement) element.

### <a id="TextPathElement"></a>10.13.2 The ‘textPath’ element

‘textPath’

**Categories:**
: [Text content element](intro.md#TermTextContentElement), [text content child element](intro.md#TermTextContentChildElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)
- [‘a’](linking.md#AElement)
- [‘altGlyph’](#AltGlyphElement)
- [‘animate’](animate.md#AnimateElement)
- [‘animateColor’](animate.md#AnimateColorElement)
- [‘set’](animate.md#SetElement)
- [‘tref’](#TRefElement)
- [‘tspan’](#TSpanElement)

**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](#AlignmentBaselineProperty), [‘baseline-shift’](#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](#FontFamilyProperty), [‘font-size’](#FontSizeProperty), [‘font-size-adjust’](#FontSizeAdjustProperty), [‘font-stretch’](#FontStretchProperty), [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](#TextAnchorProperty), [‘text-decoration’](#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](#WordSpacingProperty), [‘writing-mode’](#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#TextPathElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘xlink:href’](#TextPathElementHrefAttribute)
- [‘startOffset’](#TextPathElementStartOffsetAttribute)
- [‘method’](#TextPathElementMethodAttribute)
- [‘spacing’](#TextPathElementSpacingAttribute)

**DOM Interfaces:**
:

- [SVGTextPathElement](#InterfaceSVGTextPathElement)

*Attribute definitions:*

**<a id="TextPathElementStartOffsetAttribute"></a>startOffset = "[<length>](types.md#DataTypeLength)"**
: An offset from the start of the [‘path’](paths.md#PathElement) for the initial current text position, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. If a [<length>](types.md#DataTypeLength) other than a percentage is given, then the [‘startOffset’](#TextPathElementStartOffsetAttribute) represents a distance along the path measured in the current user coordinate system. If a percentage is given, then the [‘startOffset’](#TextPathElementStartOffsetAttribute) represents a percentage distance along the entire path. Thus, startOffset="0%" indicates the start point of the [‘path’](paths.md#PathElement) and startOffset="100%" indicates the end point of the [‘path’](paths.md#PathElement). If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="TextPathElementMethodAttribute"></a> method = "align | stretch"**
: Indicates the method by which text should be rendered along the path. A value of align indicates that the glyphs should be rendered using simple 2x3 transformations such that there is no stretching/warping of the glyphs. Typically, supplemental rotation, scaling and translation transformations are done for each glyph to be rendered. As a result, with align, fonts where the glyphs are designed to be connected (e.g., cursive fonts), the connections may not align properly when text is rendered along a path. A value of stretch indicates that the glyph outlines will be converted into paths, and then all end points and control points will be adjusted to be along the perpendicular vectors from the path, thereby stretching and possibly warping the glyphs. With this approach, connected glyphs, such as in cursive scripts, will maintain their connections. If the attribute is not specified, the effect is as if a value of align were specified. [Animatable](animate.md#Animatable): yes.
**<a id="TextPathElementSpacingAttribute"></a> spacing = "auto | exact"**
: Indicates how the user agent should determine the spacing between glyphs that are to be rendered along a path. A value of exact indicates that the glyphs should be rendered exactly according to the spacing rules as specified in [Text on a path layout rules](#TextpathLayoutRules). A value of auto indicates that the user agent should use text-on-a-path layout algorithms to adjust the spacing between glyphs in order to achieve visually appealing results. If the attribute is not specified, the effect is as if a value of exact were specified. [Animatable](animate.md#Animatable): yes.
**<a id="TextPathElementHrefAttribute"></a> xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) to the [‘path’](paths.md#PathElement) element onto which the glyphs will be rendered. If <iri> is an invalid reference (e.g., no such element exists, or the referenced element is not a [‘path’](paths.md#PathElement)), then the [‘textPath’](#TextPathElement) element is in error and its entire contents shall not be rendered by the user agent. [Animatable](animate.md#Animatable): yes.
The path data coordinates within the referenced [‘path’](paths.md#PathElement) element are assumed to be in the same coordinate system as the current [‘text’](#TextElement) element, not in the coordinate system where the [‘path’](paths.md#PathElement) element is defined. The [‘transform’](coords.md#TransformAttribute) attribute on the referenced [‘path’](paths.md#PathElement) element represents a supplemental transformation relative to the current user coordinate system for the current [‘text’](#TextElement) element, including any adjustments to the current user coordinate system due to a possible [‘transform’](coords.md#TransformAttribute) attribute on the current [‘text’](#TextElement) element. For example, the following fragment of SVG content:

```
<svg xmlns="http://www.w3.org/2000/svg" 
     xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1">
  <g transform="translate(25,25)">
    <defs>
      <path id="path1" transform="scale(2)" d="..." fill="none" stroke="red"/>
    </defs>
  </g>
  <text transform="rotate(45)">
    <textPath xlink:href="#path1">Text along path1</textPath>
  </text>
</svg>
```

should have the same effect as the following:

```
<svg xmlns="http://www.w3.org/2000/svg" 
     xmlns:xlink="http://www.w3.org/1999/xlink" version="1.1">
  <g transform="rotate(45)">
    <defs>
      <path id="path1" transform="scale(2)" d="..." fill="none" stroke="red"/>
    </defs>
    <text>
      <textPath xlink:href="#path1">Text along path1</textPath>
    </text>
  </g>
</svg>
```

Note that the `transform="translate(25,25)"` has no effect on the [‘textPath’](#TextPathElement) element, whereas the `transform="rotate(45)"` applies to both the [‘text’](#TextElement) and the use of the [‘path’](paths.md#PathElement) element as the referenced shape for text on a path.

<a id="ExampleToap01"></a>Example toap01 provides a simple example of text on a path:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs>
    <path id="MyPath"
          d="M 100 200 
             C 200 100 300   0 400 100
             C 500 200 600 300 700 200
             C 800 100 900 100 900 100" />
  </defs>
  <desc>Example toap01 - simple text on a path</desc>

  <use xlink:href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="42.5" fill="blue" >
    <textPath xlink:href="#MyPath">
      We go up, then we go down, then up again
    </textPath>
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example toap01

| [image: Example toap01 — simple text on a path] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/toap01.svg)

<a id="ExampleToap02"></a>Example toap02 shows how [‘tspan’](#TSpanElement) elements can be included within [‘textPath’](#TextPathElement) elements to adjust styling attributes and adjust the current text position before rendering a particular glyph. The first occurrence of the word "up" is filled with the color red. Attribute [‘dy’](#TSpanElementDYAttribute) is used to lift the word "up" from the baseline.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs>
    <path id="MyPath"
          d="M 100 200 
             C 200 100 300   0 400 100
             C 500 200 600 300 700 200
             C 800 100 900 100 900 100" />
  </defs>
  <desc>Example toap02 - tspan within textPath</desc>

  <use xlink:href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="42.5" fill="blue" >
    <textPath xlink:href="#MyPath">
      We go 
      <tspan dy="-30" fill="red" >
        up
      </tspan>
      <tspan dy="30">
        ,
      </tspan>
      then we go down, then up again
    </textPath>
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example toap02

| [image: Example toap02 — tspan within textPath] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/toap02.svg)

<a id="ExampleToap03"></a>Example toap03 demonstrates the use of the [‘startOffset’](#TextPathElementStartOffsetAttribute) attribute on the [‘textPath’](#TextPathElement) element to specify the start position of the text string as a particular position along the path. Notice that glyphs that fall off the end of the path are not rendered (see [text on a path layout rules](#TextpathLayoutRules)).

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs>
    <path id="MyPath"
          d="M 100 200 
             C 200 100 300   0 400 100
             C 500 200 600 300 700 200
             C 800 100 900 100 900 100" />
  </defs>
  <desc>Example toap03 - text on a path with startOffset attribute</desc>

  <use xlink:href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="42.5" fill="blue" >
    <textPath xlink:href="#MyPath" startOffset="80%">
      We go up, then we go down, then up again
    </textPath>
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example toap03

| [image: Example toap03 — text on a path with startOffset attribute] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/toap03.svg)

### <a id="TextpathLayoutRules"></a>10.13.3 Text on a path layout rules

Conceptually, for text on a path the target path is stretched out into either a horizontal or vertical straight line segment. For horizontal text layout flows, the path is stretched out into a hypothetical horizontal line segment such that the start of the path is mapped to the left of the line segment. For vertical text layout flows, the path is stretched out into a hypothetical vertical line segment such that the start of the path is mapped to the top of the line segment. The standard [text layout](#TextLayout) rules are applied to the hypothetical straight line segment and the result is mapped back onto the target path. Vertical and bidirectional [text layout](#TextLayout) rules also apply to text on a path.

The [reference orientation](#ReferenceOrientation) is determined individually for each glyph that is rendered along the path. For horizontal text layout flows, the reference orientation for a given glyph is the vector that starts at the intersection point on the path to which the glyph is attached and which points in the direction 90 degrees counter-clockwise from the angle of the curve at the intersection point. For vertical text layout flows, the reference orientation for a given glyph is the vector that starts at the intersection point on the path to which the glyph is attached and which points in the direction 180 degrees from the angle of the curve at the intersection point.

<a id="ExampleToap04"></a>Example toap04 will be used to illustrate the particular layout rules for text on a path that supplement the basic [text layout](#TextLayout) rules for straight line horizontal or vertical text.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <defs>
    <path id="MyPath"
          d="M 100 125 
             C 150 125 250 175 300 175
             C 350 175 450 125 500 125
             C 550 125 650 175 700 175
             C 750 175 850 125 900 125" />
  </defs>
  <desc>Example toap04 - text on a path layout rules</desc>

  <use xlink:href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="60" fill="blue" letter-spacing="2" >
    <textPath xlink:href="#MyPath">
      Choose shame or get war 
    </textPath>
  </text>

  <!-- Show outline of canvas using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

Example toap04

| [image: Example toap04 — text on a path layout rules] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/text/toap04.svg)

The following picture does an initial zoom in on the first glyph in the [‘text’](#TextElement) element.

[image: Image that shows text on a path]

The small dot above shows the point at which the glyph is attached to the path. The box around the glyph shows the glyph is rotated such that its horizontal axis is parallel to the tangent of the curve at the point at which the glyph is attached to the path. The box also shows the glyph's <a id="CharWidth"></a>charwidth (i.e., the amount which the current text position advances horizontally when the glyph is drawn using horizontal text layout).

The next picture zooms in further to demonstrate the detailed layout rules.

[image: Image that shows text on a path]

For left-to-right horizontal text layout along a path (i.e., when the glyph orientation is perpendicular to the [inline-progression-direction](#SettingInlineProgressionDirection)), the layout rules are as follows:

- Determine the <a id="StartPointOnPath"></a>startpoint-on-the-path for the first glyph using attribute [‘startOffset’](#TextPathElementStartOffsetAttribute) and property [‘text-anchor’](#TextAnchorProperty). For text-anchor:start, startpoint-on-the-path is the point on the path which represents the point on the path which is [‘startOffset’](#TextPathElementStartOffsetAttribute) distance along the path from the start of the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. For text-anchor:middle, startpoint-on-the-path is the point on the path which represents the point on the path which is [ [‘startOffset’](#TextPathElementStartOffsetAttribute) minus half of the total advance values for all of the glyphs in the [‘textPath’](#TextPathElement) element ] distance along the path from the start of the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. For text-anchor:end, startpoint-on-the-path is the point on the path which represents the point on the path which is [ [‘startOffset’](#TextPathElementStartOffsetAttribute) minus the total advance values for all of the glyphs in the [‘textPath’](#TextPathElement) element ]. Before rendering the first glyph, the horizontal component of the startpoint-on-the-path is adjusted to take into account various horizontal alignment text properties and attributes, such as a [‘dx’](#TSpanElementDXAttribute) attribute value on a [‘tspan’](#TSpanElement) element. (In the picture above, the startpoint-on-the-path is the leftmost dot on the path.)
- Determine the glyph's charwidth (i.e., the amount which the current text position advances horizontally when the glyph is drawn using horizontal text layout). (In the picture above, the charwidth is the distance between the two dots at the side of the box.)
- Determine the point on the curve which is charwidth distance along the path from the startpoint-on-the-path for this glyph, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. This point is the <a id="EndPointOnPath"></a>endpoint-on-the-path for the glyph. (In the picture above, the endpoint-on-the-path for the glyph is the rightmost dot on the path.)
- Determine the <a id="MidPointOnPath"></a>midpoint-on-the-path, which is the point on the path which is "halfway" (user agents can choose either a distance calculation or a parametric calculation) between the startpoint-on-the-path and the endpoint-on-the-path. (In the picture above, the midpoint-on-the-path is shown as a white dot.)
- Determine the <a id="GlyphMidline"></a>glyph-midline, which is the vertical line in the glyph's coordinate system that goes through the glyph's x-axis midpoint. (In the picture above, the glyph-midline is shown as a dashed line.)
- Position the glyph such that the glyph-midline passes through the midpoint-on-the-path and is perpendicular to the line through the startpoint-on-the-path and the endpoint-on-the-path.
- Align the glyph vertically relative to the midpoint-on-the-path based on property [‘alignment-baseline’](#AlignmentBaselineProperty) and any specified values for attribute [‘dy’](#TSpanElementDYAttribute) on a [‘tspan’](#TSpanElement) element. In the example above, the [‘alignment-baseline’](#AlignmentBaselineProperty) property is unspecified, so the initial value of alignment-baseline:baseline will be used. There are no [‘tspan’](#TSpanElement) elements; thus, the baseline of the glyph is aligned to the midpoint-on-the-path.
- For each subsequent glyph, set a new startpoint-on-the-path as the previous endpoint-on-the-path, but with appropriate adjustments taking into account horizontal kerning tables in the font and current values of various attributes and properties, including [spacing properties](#SpacingProperties) and [‘tspan’](#TSpanElement) elements with values provided for attributes [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute). All adjustments are calculated as distance adjustments along the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm.
- Glyphs whose midpoint-on-the-path are off either end of the path are not rendered.
- Continue rendering glyphs until there are no more glyphs.

Comparable rules are used for top-to-bottom vertical text layout along a path (i.e., when the glyph orientation is parallel with the [inline-progression-direction](#SettingInlineProgressionDirection)), the layout rules are as follows:

- Determine the startpoint-on-the-path using the same method as for horizontal text layout along a path, except that before rendering the first glyph, the horizontal component of the startpoint-on-the-path is adjusted to take into account various vertical alignment text properties and attributes, such as a [‘dy’](#TSpanElementDYAttribute) attribute value on a [‘tspan’](#TSpanElement) element.
- Determine the glyph's charheight (i.e., the amount which the current text position advances vertically when the glyph is drawn using vertical text layout).
- Determine the point on the curve which is charheight distance along the path from the startpoint-on-the-path for this glyph, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. This point is the endpoint-on-the-path for the glyph.
- Determine the midpoint-on-the-path, which is the point on the path which is "halfway" (user agents can choose either a distance calculation or a parametric calculation) between the startpoint-on-the-path and the endpoint-on-the-path.
- Determine the glyph-midline, which is the horizontal line in the glyph's coordinate system that goes through the glyph's y-axis midpoint.
- Position the glyph such that the glyph-midline passes through the midpoint-on-the-path and is perpendicular to the line through the startpoint-on-the-path and the endpoint-on-the-path.
- Align the glyph horizontally (where horizontal is relative to the glyph's coordinate system) relative to the midpoint-on-the-path based on property [‘alignment-baseline’](#AlignmentBaselineProperty) and any specified values for attribute [‘dx’](#TSpanElementDXAttribute) on a [‘tspan’](#TSpanElement) element.
- For each subsequent glyph, set a new startpoint-on-the-path as the previous endpoint-on-the-path, but with appropriate adjustments taking into account vertical kerning tables in the font and current values of various attributes and properties, including [spacing properties](#SpacingProperties) and [‘tspan’](#TSpanElement) elements with values provided for attributes [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute). All adjustments are calculated as distance adjustments along the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm.
- Glyphs whose midpoint-on-the-path are off either end of the path are not rendered.
- Continue rendering glyphs until there are no more glyphs.

In the calculations above, if either the startpoint-on-the-path or the endpoint-on-the-path is off the end of the path, then extend the path beyond its end points with a straight line that is parallel to the tangent at the path at its end point so that the midpoint-on-the-path can still be calculated.

When the [inline-progression-direction](#SettingInlineProgressionDirection) is horizontal, then any ‘x’ attributes on [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) elements represent new absolute offsets along the path, thus providing explicit new values for startpoint-on-the-path. Any ‘y’ attributes on [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) elements are ignored. When the [inline-progression-direction](#SettingInlineProgressionDirection) is vertical, then any ‘y’ attributes on [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) elements represent new absolute offsets along the path, thus providing explicit new values for startpoint-on-the-path. Any ‘x’ attributes on [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) or [‘altGlyph’](#AltGlyphElement) elements are ignored.

## <a id="AlternateGlyphs"></a>10.14 Alternate glyphs

There are situations such as ligatures, special-purpose fonts (e.g., a font for music symbols) or alternate glyphs for Asian text strings where it is required that a different set of glyphs is used than the glyph(s) which normally corresponds to the given character data.

### <a id="AltGlyphElement"></a>10.14.1 The ‘altGlyph’ element

The [‘altGlyph’](#AltGlyphElement) element provides control over the glyphs used to render particular character data.

‘altGlyph’

**Categories:**
: [Text content element](intro.md#TermTextContentElement), [text content child element](intro.md#TermTextContentChildElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](#AlignmentBaselineProperty), [‘baseline-shift’](#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](#FontFamilyProperty), [‘font-size’](#FontSizeProperty), [‘font-size-adjust’](#FontSizeAdjustProperty), [‘font-stretch’](#FontStretchProperty), [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](#TextAnchorProperty), [‘text-decoration’](#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](#WordSpacingProperty), [‘writing-mode’](#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#AltGlyphElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘x’](#AltGlyphElementXAttribute)
- [‘y’](#AltGlyphElementYAttribute)
- [‘dx’](#AltGlyphElementDXAttribute)
- [‘dy’](#AltGlyphElementDYAttribute)
- [‘glyphRef’](#AltGlyphElementGlyphRefAttribute)
- [‘format’](#AltGlyphElementFormatAttribute)
- [‘rotate’](#AltGlyphElementRotateAttribute)
- [‘xlink:href’](#AltGlyphElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGAltGlyphElement](#InterfaceSVGAltGlyphElement)

*Attribute definitions:*

**<a id="AltGlyphElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) either to a [‘glyph’](fonts.md#GlyphElement) element in an SVG document fragment or to an [‘altGlyphDef’](#AltGlyphDefElement) element. If the reference is to a [‘glyph’](fonts.md#GlyphElement) element and that glyph is available, then that glyph is rendered instead of the character(s) that are inside of the [‘altGlyph’](#AltGlyphElement) element. If the reference is to an [‘altGlyphDef’](#AltGlyphDefElement) element, then if an appropriate set of alternate glyphs is located from processing the [‘altGlyphDef’](#AltGlyphDefElement) element, then those alternate glyphs are rendered instead of the character(s) that are inside of the [‘altGlyph’](#AltGlyphElement) element. [Animatable](animate.md#Animatable): no.
**<a id="AltGlyphElementGlyphRefAttribute"></a>glyphRef = "<string>"**
: The glyph identifier, the format of which is dependent on the [‘format’](#AltGlyphElementFormatAttribute) of the given font. (Same meaning as the [‘glyphRef’](#GlyphRefElementGlyphRefAttribute) attribute on the [‘glyphRef’](#GlyphRefElement) element.) [Animatable](animate.md#Animatable): no.
**<a id="AltGlyphElementFormatAttribute"></a>format = "<string>"**
: The format of the given font. If the font is in one of the formats listed in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.3.5), such as *TrueDoc™ Portable Font Resource* or *Embedded OpenType*, then the <string> must contain the corresponding font format string, such as *truedoc-pfr* or *embedded-opentype*. (This attribute has the same meaning as the [‘format’](#GlyphRefElementFormatAttribute) attribute on the [‘glyphRef’](#GlyphRefElement) element.) [Animatable](animate.md#Animatable): no.
**<a id="AltGlyphElementXAttribute"></a>x = "[<list-of-coordinates>](types.md#DataTypeCoordinates)"**
: The [<coordinate>](types.md#DataTypeCoordinate) values are processed in the same manner as the [‘x’](#TSpanElementXAttribute) attribute on the [‘tspan’](#TSpanElement) element, with the following exception: If the referenced alternate glyphs are rendered instead of the Unicode characters inside the [‘altGlyph’](#AltGlyphElement) element, then any absolute X coordinates specified via an [‘x’](#AltGlyphElementXAttribute) attribute on this element or any ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) elements for Unicode characters 2 through *n* within the [‘altGlyph’](#AltGlyphElement) element are ignored. Any absolute X coordinate specified via an [‘x’](#AltGlyphElementXAttribute) attribute on this element or any ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) elements for the first Unicode character within the [‘altGlyph’](#AltGlyphElement) element sets a new absolute X coordinate for the [current text position](#CurrentTextPosition) before rendering the first alternate glyph. [Animatable](animate.md#Animatable): yes.
**<a id="AltGlyphElementYAttribute"></a>y = "[<list-of-coordinates>](types.md#DataTypeCoordinates)"**
: The corresponding absolute Y coordinates for rendering the [‘altGlyph’](#AltGlyphElement) element. [Animatable](animate.md#Animatable): yes.
**<a id="AltGlyphElementDXAttribute"></a>dx = "[<list-of-lengths>](types.md#DataTypeLengths)"**
: The [<length>](types.md#DataTypeLength) values are processed in the same manner as the [‘dx’](#TSpanElementDXAttribute) attribute on the [‘tspan’](#TSpanElement) element, with the following exception: If the referenced alternate glyphs are rendered instead of the Unicode characters inside the [‘altGlyph’](#AltGlyphElement) element, then any relative X coordinates specified via an [‘dx’](#AltGlyphElementDXAttribute) attribute on this element or any ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) elements for Unicode characters 2 through *n* within the [‘altGlyph’](#AltGlyphElement) element are ignored. Any relative X coordinate specified via an [‘dx’](#AltGlyphElementDXAttribute) attribute on this element or any ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) elements for the first Unicode character within the [‘altGlyph’](#AltGlyphElement) element sets a new relative X coordinate for the [current text position](#CurrentTextPosition) before rendering the first alternate glyph. [Animatable](animate.md#Animatable): yes.
**<a id="AltGlyphElementDYAttribute"></a>dy = "[<list-of-lengths>](types.md#DataTypeLengths)"**
: The corresponding relative Y coordinates for rendering the [‘altGlyph’](#AltGlyphElement) element. [Animatable](animate.md#Animatable): yes.
**<a id="AltGlyphElementRotateAttribute"></a>rotate = "[<list-of-numbers>](types.md#DataTypeNumbers)"**
: The [<number>](types.md#DataTypeNumber) values are processed in the same manner as the [‘rotate’](#TSpanElementRotateAttribute) attribute on the [‘tspan’](#TSpanElement) element, with the following exception: If the referenced alternate glyphs are rendered instead of the Unicode characters inside the [‘altGlyph’](#AltGlyphElement) element, then any supplemental rotation values specified via an [‘rotate’](#AltGlyphElementRotateAttribute) attribute on this element or any ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) elements for Unicode characters 2 through *n* within the [‘altGlyph’](#AltGlyphElement) element are ignored. Supplemental rotation values specified via an [‘rotate’](#AltGlyphElementRotateAttribute) attribute on this element or any ancestor [‘text’](#TextElement) or [‘tspan’](#TSpanElement) elements for the first Unicode character within the [‘altGlyph’](#AltGlyphElement) element sets a new supplemental rotation angle before rendering the alternate glyphs. [Animatable](animate.md#Animatable): yes (non-additive).
If the references to alternate glyphs do not result in successful identification of alternate glyphs to use, then the character(s) that are inside of the [‘altGlyph’](#AltGlyphElement) element are rendered as if the [‘altGlyph’](#AltGlyphElement) element were a [‘tspan’](#TSpanElement) element instead.

An [‘altGlyph’](#AltGlyphElement) element either references a [‘glyph’](fonts.md#GlyphElement) element or an [‘altGlyphDef’](#AltGlyphDefElement) element via its [‘xlink:href’](#AltGlyphElementHrefAttribute) attribute or identifies a glyph by means of [font selection properties](#FontPropertiesUsedBySVG), a glyph identifier and a font format. If the [‘xlink:href’](#AltGlyphElementHrefAttribute) attribute is specified, it takes precedence, and the other glyph identification attributes and properties are ignored.

### <a id="AlternateGlyphDefinitions"></a>10.14.2 The ‘altGlyphDef’, ‘altGlyphItem’ and ‘glyphRef’ elements

<a id="AltGlyphDefElement"></a>The [‘altGlyphDef’](#AltGlyphDefElement) element defines a set of possible glyph substitutions.

‘altGlyphDef’

**Categories:**
: None
**Content model:**
: Either:

- one or more [‘glyphRef’](#GlyphRefElement) elements, or
- one or more [‘altGlyphItem’](#AltGlyphItemElement) elements.

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)

**DOM Interfaces:**
:

- [SVGAltGlyphDefElement](#InterfaceSVGAltGlyphDefElement)

An [‘altGlyphDef’](#AltGlyphDefElement) can contain either of the following:

- In the simplest case, an [‘altGlyphDef’](#AltGlyphDefElement) contains one or more [‘glyphRef’](#GlyphRefElement) elements. Each [‘glyphRef’](#GlyphRefElement) element references a single glyph within a particular font. If all of the referenced glyphs are available, then these glyphs are rendered instead of the character(s) inside of the referencing [‘altGlyph’](#AltGlyphElement) element. If any of the referenced glyphs are unavailable, then the character(s) that are inside of the [‘altGlyph’](#AltGlyphElement) element are rendered as if there were not an [‘altGlyph’](#AltGlyphElement) element surrounding those characters.
- In the more complex case, an [‘altGlyphDef’](#AltGlyphDefElement) contains one or more [‘altGlyphItem’](#AltGlyphItemElement) elements. Each [‘altGlyphItem’](#AltGlyphItemElement) represents a candidate set of substitute glyphs. Each [‘altGlyphItem’](#AltGlyphItemElement) contains one or more [‘glyphRef’](#GlyphRefElement) elements. Each [‘glyphRef’](#GlyphRefElement) element references a single glyph within a particular font. The first [‘altGlyphItem’](#AltGlyphItemElement) in which all referenced glyphs are available is chosen. The glyphs referenced from this [‘altGlyphItem’](#AltGlyphItemElement) are rendered instead of the character(s) that are inside of the referencing [‘altGlyph’](#AltGlyphElement) element. If none of the [‘altGlyphItem’](#AltGlyphItemElement) elements result in a successful match (i.e., none of the [‘altGlyphItem’](#AltGlyphItemElement) elements has all of its referenced glyphs available), then the character(s) that are inside of the [‘altGlyph’](#AltGlyphElement) element are rendered as if there were not an [‘altGlyph’](#AltGlyphElement) element surrounding those characters.

<a id="AltGlyphItemElement"></a>The [‘altGlyphItem’](#AltGlyphItemElement) element defines a candidate set of possible glyph substitutions. The first [‘altGlyphItem’](#AltGlyphItemElement) element whose referenced glyphs are all available is chosen. Its glyphs are rendered instead of the character(s) that are inside of the referencing [‘altGlyph’](#AltGlyphElement) element.

‘altGlyphItem’

**Categories:**
: None
**Content model:**
: One or more [‘glyphRef’](#GlyphRefElement) elements.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)

**DOM Interfaces:**
:

- [SVGAltGlyphItemElement](#InterfaceSVGAltGlyphItemElement)

<a id="GlyphRefElement"></a>The [‘glyphRef’](#GlyphRefElement) element defines a possible glyph to use.

‘glyphRef’

**Categories:**
: None
**Content model:**
: Empty.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](#AlignmentBaselineProperty), [‘baseline-shift’](#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](#FontFamilyProperty), [‘font-size’](#FontSizeProperty), [‘font-size-adjust’](#FontSizeAdjustProperty), [‘font-stretch’](#FontStretchProperty), [‘font-style’](#FontStyleProperty), [‘font-variant’](#FontVariantProperty), [‘font-weight’](#FontWeightProperty), [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](#TextAnchorProperty), [‘text-decoration’](#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](#WordSpacingProperty), [‘writing-mode’](#WritingModeProperty)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#GlyphRefElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘x’](#GlyphRefElementXAttribute)
- [‘y’](#GlyphRefElementYAttribute)
- [‘dx’](#GlyphRefElementDXAttribute)
- [‘dy’](#GlyphRefElementDYAttribute)
- [‘glyphRef’](#GlyphRefElementGlyphRefAttribute)
- [‘format’](#GlyphRefElementFormatAttribute)
- [‘xlink:href’](#GlyphRefElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGGlyphRefElement](#InterfaceSVGGlyphRefElement)

*Attribute definitions:*

**<a id="GlyphRefElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: An [IRI reference](linking.md#IRIReference) to a [‘glyph’](fonts.md#GlyphElement) element in an SVG document fragment. The referenced [‘glyph’](fonts.md#GlyphElement) is rendered as an alternate glyph. [Animatable](animate.md#Animatable): no.
**<a id="GlyphRefElementGlyphRefAttribute"></a>glyphRef = "<string>"**
: The glyph identifier, the format of which is dependent on the [‘format’](#GlyphRefElementFormatAttribute) of the given font. [Animatable](animate.md#Animatable): no.
**<a id="GlyphRefElementFormatAttribute"></a>format = "<string>"**
: The format of the given font. If the font is in one of the formats listed in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 15.3.5), such as *TrueDoc™ Portable Font Resource* or *Embedded OpenType*, then the <string> must contain the corresponding font format string, such as *truedoc-pfr* or *embedded-opentype*. [Animatable](animate.md#Animatable): no.
**<a id="GlyphRefElementXAttribute"></a>x = "[<number>](types.md#DataTypeNumber)"**
: This value represents the new absolute X coordinate within the font's coordinate system for this glyph. The font coordinate system is based on the *em square* model described in the Fonts chapter of CSS2 ([[CSS2](refs.md#ref-CSS2)], chapter 15). If the attribute is not specified, for the first [‘glyphRef’](#GlyphRefElement) child element, the effect is as if the attribute were set to "0", whereas for subsequent [‘glyphRef’](#GlyphRefElement) child elements, the effect is as if the attribute were set to the end X coordinate from the previous [‘glyphRef’](#GlyphRefElement) element. [Animatable](animate.md#Animatable): no.
**<a id="GlyphRefElementYAttribute"></a>y = "[<number>](types.md#DataTypeNumber)"**
: The corresponding new absolute Y coordinate within the font's coordinate system for this glyph. [Animatable](animate.md#Animatable): no.
**<a id="GlyphRefElementDXAttribute"></a>dx = "[<number>](types.md#DataTypeNumber)"**
: This value represents the relative X coordinate within the font's coordinate system for this glyph. The glyph is thus shifted by <number> units along the positive X axis within the font's coordinate system supplemental to the absolute X coordinate established by the [‘x’](#GlyphRefElementXAttribute) attribute (either due to an explicit [‘x’](#GlyphRefElementXAttribute) attribute or due to default value processing for the [‘x’](#GlyphRefElementXAttribute) attribute). The font coordinate system is based on the *em square* model described in the Fonts chapter of CSS2 ([[CSS2](refs.md#ref-CSS2)], chapter 15). If the attribute is not specified, the effect is as if the attribute were set to "0". [Animatable](animate.md#Animatable): no.
**<a id="GlyphRefElementDYAttribute"></a>dy = "[<number>](types.md#DataTypeNumber)"**
: The corresponding number of units within the font's coordinate system to shift the glyph along the positive Y axis relative to the absolute Y coordinate established by the [‘y’](#GlyphRefElementYAttribute) attribute. [Animatable](animate.md#Animatable): no.
A [‘glyphRef’](#GlyphRefElement) either references a [‘glyph’](fonts.md#GlyphElement) element in an SVG document fragment via its [‘xlink:href’](#GlyphRefElementHrefAttribute) attribute or identifies a glyph by means of [font selection properties](#FontPropertiesUsedBySVG), a glyph identifier and a font format. If insufficient attributes and properties have been specified to identify a glyph, then the [‘glyphRef’](#GlyphRefElement) is processed in the same manner as when a glyph reference is fully specified, but the given glyph is not available. If the [‘xlink:href’](#GlyphRefElementHrefAttribute) attribute is specified, it takes precedence, and the other glyph identification attributes and properties are ignored.

## <a id="WhiteSpace"></a>10.15 White space handling

<a id="XMLSpaceAttribute"></a>SVG supports the standard XML attribute [‘xml:space’](struct.md#XMLSpaceAttribute) to specify the handling of white space characters within a given [‘text’](#TextElement) element's character data. Note that any child element of a [‘text’](#TextElement) element may also have an [‘xml:space’](struct.md#XMLSpaceAttribute) attribute which will apply to that child element's text content. The SVG user agent has special processing rules associated with this attribute as described below. These are behaviors that occur subsequent to XML parsing [XML10] and any construction of a DOM.

[‘xml:space’](struct.md#XMLSpaceAttribute) is an inheritable attribute which can have one of two values:

**'default'**
: (The initial/default value for [‘xml:space’](struct.md#XMLSpaceAttribute).) When xml:space="default", the SVG user agent will do the following using a copy of the original character data content. First, it will remove all newline characters. Then it will convert all tab characters into space characters. Then, it will strip off all leading and trailing space characters. Then, all contiguous space characters will be consolidated.
**'preserve'**
: When xml:space="preserve", the SVG user agent will do the following using a copy of the original character data content. It will convert all newline and tab characters into space characters. Then, it will draw all space characters, including leading, trailing and multiple contiguous space characters. Thus, when drawn with xml:space="preserve", the string `"a   b"` (three spaces between "a" and "b") will produce a larger separation between "a" and "b" than `"a b"` (one space between "a" and "b").
The following example illustrates that line indentation can be important when using xml:space="default". The fragment below show two pairs of similar [‘text’](#TextElement) elements, with both [‘text’](#TextElement) elements using xml:space="default". For these examples, there is no extra white space at the end of any of the lines (i.e., the line break occurs immediately after the last visible character).

```
[01]  <text xml:space='default'>
[02]    WS example
[03]    indented lines
[04]  </text>
[05]  <text xml:space='preserve'>WS example indented lines</text>
[06]
[07]  <text xml:space='default'>
[08]WS example
[09]non-indented lines
[10]  </text>
[11]  <text xml:space='preserve'>WS examplenon-indented lines</text>
```

The first pair of [‘text’](#TextElement) elements above show the effect of indented character data. The attribute xml:space="default" in the first [‘text’](#TextElement) element instructs the user agent to:

- convert all tabs (if any) to space characters,
- strip out all line breaks (i.e., strip out the line breaks at the end of lines [01], [02] and [03]),
- strip out all leading space characters (i.e., strip out space characters before "WS example" on line [02]),
- strip out all trailing space characters (i.e., strip out space characters before "</text>" on line [04]),
- consolidate all intermediate space characters (i.e., the space characters before "indented lines" on line [03]) into a single space character.

The second pair of [‘text’](#TextElement) elements above show the effect of non-indented character data. The attribute xml:space="default" in the third [‘text’](#TextElement) element instructs the user agent to:

- convert all tabs (if any) to space characters,
- strip out all line breaks (i.e., strip out the line breaks at the end of lines [07], [08] and [09]),
- strip out all leading space characters (there are no leading space characters in this example),
- strip out all trailing space characters (i.e., strip out space characters before "</text>" on line [10]),
- consolidate all intermediate space characters into a single space character (in this example, there are no intermediate space characters).

Note that XML parsers are required to convert the standard representations for a newline indicator (e.g., the literal two-character sequence "#xD#xA" or the stand-alone literals #xD or #xA) into the single character #xA before passing character data to the application. Thus, each newline in SVG will be represented by the single character #xA, no matter what representation for newlines might have been used in the original resource. (See XML end-of-line handling.)

Any features in the SVG language or the SVG DOM that are based on character position number, such as the [‘x’](#TSpanElementXAttribute), [‘y’](#TSpanElementYAttribute), [‘dx’](#TSpanElementDXAttribute), [‘dy’](#TSpanElementDYAttribute) and [‘rotate’](#TSpanElementRotateAttribute) attributes on the [‘text’](#TextElement), [‘tspan’](#TSpanElement), [‘tref’](#TRefElement) and [‘altGlyph’](#AltGlyphElement) elements, are based on character position after applying the white space handling rules described here. In particular, if xml:space="default", it is often the case that white space characters are removed as part of processing. Character position numbers index into the text string after the white space characters have been removed per the rules in this section.

Note that a glyph corresponding to a whitespace character should only be displayed as a visible but blank space, even if the glyph itself happens to be non-blank. See display of unsupported characters [[UNICODE](refs.md#ref-UNICODE)].

The [‘xml:space’](struct.md#XMLSpaceAttribute) attribute is:

[Animatable](animate.md#Animatable): no.

## <a id="TextSelection"></a>10.16 Text selection and clipboard operations

[Conforming SVG viewers](conform.md#ConformingSVGViewers) on systems which have the capacity for text selection (e.g., systems which are equipped with a pointer device such as a mouse) and which have system clipboards for copy/paste operations are required to support:

- user selection of text strings in SVG content
- the ability to copy selected text strings to the system clipboard

A text selection operation starts when all of the following occur:

- the user positions the pointing device over a glyph that has been rendered as part of a [‘text’](#TextElement) element, initiates a *select* operation (e.g., pressing the standard system mouse button for select operations) and then moves the pointing device while continuing the *select* operation (e.g., continuing to press the standard system mouse button for select operations)
- no other visible graphics element has been painted above the glyph at the point at which the pointing device was clicked
- no [links](linking.md#Links) or [events](https://www.w3.org/TR/SVG11/script.html#EventHandling) have been assigned to the [‘text’](#TextElement), [‘tspan’](#TSpanElement) or [‘textPath’](#TextPathElement) element(s) (or their ancestors) associated with the given glyph.

As the text selection operation proceeds (e.g., the user continues to press the given mouse button), all associated events with other graphics elements are ignored (i.e., the text selection operation is modal) and the SVG user agent shall dynamically indicate which characters are selected by an appropriate highlighting technique, such as redrawing the selected glyphs with inverse colors. As the pointer is moved during the text selection process, the end glyph for the text selection operation is the glyph within the same [‘text’](#TextElement) element whose glyph cell is closest to the pointer. All characters within the [‘text’](#TextElement) element whose position within the [‘text’](#TextElement) element is between the start of selection and end of selection shall be highlighted, regardless of position on the canvas and regardless of any graphics elements that might be above the end of selection point.

Once the text selection operation ends (e.g., the user releases the given mouse button), the selected text will stay highlighted until an event occurs which cancels text selection, such as a pointer device activation event (e.g., pressing a mouse button).

Detailed rules for determining which characters to highlight during a text selection operation are provided in [Text selection implementation notes](implnote.md#TextSelectionImplementationNotes).

For systems which have system clipboards, the SVG user agent is required to provide a user interface for initiating a copy of the currently selected text to the system clipboard. It is sufficient for the SVG user agent to post the selected text string in the system's appropriate clipboard format for plain text, but it is preferable if the SVG user agent also posts a rich text alternative which captures the various [font properties](#FontPropertiesUsedBySVG) associated with the given text string.

For bidirectional text, the user agent must support text selection in logical order, which will result in discontinuous highlighting of glyphs due to the bidirectional reordering of characters. User agents can provide an alternative ability to select bidirectional text in visual rendering order (i.e., after [bidirectional](#RelationshipWithBiDirectionality) text layout algorithms have been applied), with the result that selected character data might be discontinuous logically. In this case, if the user requests that bidirectional text be copied to the clipboard, then the user agent is required to make appropriate adjustments to copy only the visually selected characters to the clipboard.

When feasible, it is recommended that generators of SVG attempt to order their text strings to facilitate properly ordered text selection within SVG viewing applications such as Web browsers.

## <a id="DOMInterfaces"></a>10.17 DOM interfaces

### <a id="InterfaceSVGTextContentElement"></a>10.17.1 Interface SVGTextContentElement

The [SVGTextContentElement](#InterfaceSVGTextContentElement) is inherited by various text-related interfaces, such as [SVGTextElement](#InterfaceSVGTextElement), [SVGTSpanElement](#InterfaceSVGTSpanElement), [SVGTRefElement](#InterfaceSVGTRefElement), [SVGAltGlyphElement](#InterfaceSVGAltGlyphElement) and [SVGTextPathElement](#InterfaceSVGTextPathElement).

For the methods on this interface that refer to an index to a character or a number of characters, these references are to be interpreted as an index to a UTF-16 code unit or a number of UTF-16 code units, respectively. This is for consistency with DOM Level 2 Core, where methods on the CharacterData interface use UTF-16 code units as indexes and counts within the character data. Thus for example, if the text content of a [‘text’](#TextElement) element is a single non-BMP character, such as U+10000, then invoking [getNumberOfChars](#__svg__SVGTextContentElement__getNumberOfChars) on that element will return 2 since there are two UTF-16 code units (the surrogate pair) used to represent that one character.

```
interface SVGTextContentElement : SVGElement,
                                  SVGTests,
                                  SVGLangSpace,
                                  SVGExternalResourcesRequired,
                                  SVGStylable {

  // lengthAdjust Types
  const unsigned short LENGTHADJUST_UNKNOWN = 0;
  const unsigned short LENGTHADJUST_SPACING = 1;
  const unsigned short LENGTHADJUST_SPACINGANDGLYPHS = 2;

  readonly attribute SVGAnimatedLength textLength;
  readonly attribute SVGAnimatedEnumeration lengthAdjust;

  long getNumberOfChars();
  float getComputedTextLength();
  float getSubStringLength(in unsigned long charnum, in unsigned long nchars) raises(DOMException);
  SVGPoint getStartPositionOfChar(in unsigned long charnum) raises(DOMException);
  SVGPoint getEndPositionOfChar(in unsigned long charnum) raises(DOMException);
  SVGRect getExtentOfChar(in unsigned long charnum) raises(DOMException);
  float getRotationOfChar(in unsigned long charnum) raises(DOMException);
  long getCharNumAtPosition(in SVGPoint point);
  void selectSubString(in unsigned long charnum, in unsigned long nchars) raises(DOMException);
};
```

**Constants in group “lengthAdjust Types”:**
:

**<a id="__svg__SVGTextContentElement__LENGTHADJUST_UNKNOWN"></a>**LENGTHADJUST_UNKNOWN** (unsigned short)**
:

The enumeration was set to a value that is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGTextContentElement__LENGTHADJUST_SPACING"></a>**LENGTHADJUST_SPACING** (unsigned short)**
:

Corresponds to value 'spacing'.

**<a id="__svg__SVGTextContentElement__LENGTHADJUST_SPACINGANDGLYPHS"></a>**LENGTHADJUST_SPACINGANDGLYPHS** (unsigned short)**
:

Corresponds to value 'spacingAndGlyphs'.

**Attributes:**
:

**<a id="__svg__SVGTextContentElement__textLength"></a>**textLength** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute ‘textLength’ on the given element.

**<a id="__svg__SVGTextContentElement__lengthAdjust"></a>**lengthAdjust** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute ‘lengthAdjust’ on the given element. The value must be one of the length adjust constants defined on this interface.

**Operations:**
:

**<a id="__svg__SVGTextContentElement__getNumberOfChars"></a>long **getNumberOfChars**()**
:

Returns the total number of characters available for rendering within the current element, which includes referenced characters from [‘tref’](#TRefElement) reference, regardless of whether they will be rendered. Effectively, this is equivalent to the length of the Node::textContent attribute from DOM Level 3 Core ([[DOM3](refs.md#ref-DOM3)], section 1.4), if that attribute also expanded [‘tref’](#TRefElement) elements.

**Returns**
:

Total number of characters.

**<a id="__svg__SVGTextContentElement__getComputedTextLength"></a>float **getComputedTextLength**()**
:

The total sum of all of the advance values from rendering all of the characters within this element, including the advance value on the glyphs (horizontal or vertical), the effect of properties [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty) and [‘word-spacing’](#WordSpacingProperty) and adjustments due to attributes [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) on [‘tspan’](#TSpanElement) elements. For non-rendering environments, the user agent shall make reasonable assumptions about glyph metrics.

**Returns**
:

The text advance distance.

**<a id="__svg__SVGTextContentElement__getSubStringLength"></a>float **getSubStringLength**(in unsigned long *charnum*, in unsigned long *nchars*)**
:

The total sum of all of the advance values from rendering the specified substring of the characters, including the advance value on the glyphs (horizontal or vertical), the effect of properties [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty) and [‘word-spacing’](#WordSpacingProperty) and adjustments due to attributes [‘dx’](#TSpanElementDXAttribute) and [‘dy’](#TSpanElementDYAttribute) on [‘tspan’](#TSpanElement) elements. For non-rendering environments, the user agent shall make reasonable assumptions about glyph metrics. If multiple consecutive characters are rendered inseparably (e.g., as a single glyph or a sequence of glyphs, or because the range encompasses half of a surrogate pair), and nchars is greater than 0 then the measured range shall be expanded so that each of the inseparable characters are included.

**Parameters**
:

1.
unsigned long *charnum*
The index of the first character in the substring, where the first character has an index of 0.
2.
unsigned long *nchars*
The number of characters in the substring. If nchars specifies more characters than are available, then the substring will consist of all characters starting with charnum until the end of the list of characters.

**Returns**
:

The text advance distance.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if *charnum* or *nchars* is negative or if *charnum* is greater than or equal to the number of characters at this node.
**<a id="__svg__SVGTextContentElement__getStartPositionOfChar"></a>[SVGPoint](coords.md#InterfaceSVGPoint) **getStartPositionOfChar**(in unsigned long *charnum*)**
:

Returns the current text position before rendering the character in the user coordinate system for rendering the glyph(s) that correspond to the specified character. The current text position has already taken into account the effects of any inter-character adjustments due to properties [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty) and [‘word-spacing’](#WordSpacingProperty) and adjustments due to attributes ‘x’, ‘y’, ‘dx’ and ‘dy’. If multiple consecutive characters are rendered inseparably (e.g., as a single glyph or a sequence of glyphs), then each of the inseparable characters will return the start position for the first glyph.

**Parameters**
:

1.
unsigned long *charnum*
The index of the character, where the first character has an index of 0.

**Returns**
:

The character's start position.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the *charnum* is negative or if *charnum* is greater than or equal to the number of characters at this node.
**<a id="__svg__SVGTextContentElement__getEndPositionOfChar"></a>[SVGPoint](coords.md#InterfaceSVGPoint) **getEndPositionOfChar**(in unsigned long *charnum*)**
:

Returns the current text position after rendering the character in the user coordinate system for rendering the glyph(s) that correspond to the specified character. This current text position does *not* take into account the effects of any inter-character adjustments to prepare for the next character, such as properties [‘kerning’](#KerningProperty), [‘letter-spacing’](#LetterSpacingProperty) and [‘word-spacing’](#WordSpacingProperty) and adjustments due to attributes ‘x’, ‘y’, ‘dx’ and ‘dy’. If multiple consecutive characters are rendered inseparably (e.g., as a single glyph or a sequence of glyphs), then each of the inseparable characters will return the end position for the last glyph.

**Parameters**
:

1.
unsigned long *charnum*
The index of the character, where the first character has an index of 0.

**Returns**
:

The character's end position.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the *charnum* is negative or if *charnum* is greater than or equal to the number of characters at this node.
**<a id="__svg__SVGTextContentElement__getExtentOfChar"></a>[SVGRect](types.md#InterfaceSVGRect) **getExtentOfChar**(in unsigned long *charnum*)**
:

Returns a tightest rectangle which defines the minimum and maximum X and Y values in the user coordinate system for rendering the glyph(s) that correspond to the specified character. The calculations assume that all glyphs occupy the full standard glyph cell for the font. If multiple consecutive characters are rendered inseparably (e.g., as a single glyph or a sequence of glyphs), then each of the inseparable characters will return the same extent.

**Parameters**
:

1.
unsigned long *charnum*
The index of the character, where the first character has an index of 0.

**Returns**
:

The rectangle which encloses all of the rendered glyph(s).

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the *charnum* is negative or if *charnum* is greater than or equal to the number of characters at this node.
**<a id="__svg__SVGTextContentElement__getRotationOfChar"></a>float **getRotationOfChar**(in unsigned long *charnum*)**
:

Returns the rotation value relative to the current user coordinate system used to render the glyph(s) corresponding to the specified character. If multiple glyph(s) are used to render the given character and the glyphs each have different rotations (e.g., due to text-on-a-path), the user agent shall return an average value (e.g., the rotation angle at the midpoint along the path for all glyphs used to render this character). The rotation value represents the rotation that is supplemental to any rotation due to properties [‘glyph-orientation-horizontal’](#GlyphOrientationHorizontalProperty) and [‘glyph-orientation-vertical’](#GlyphOrientationVerticalProperty); thus, any glyph rotations due to these properties are not included into the returned rotation value. If multiple consecutive characters are rendered inseparably (e.g., as a single glyph or a sequence of glyphs), then each of the inseparable characters will return the same rotation value.

**Parameters**
:

1.
unsigned long *charnum*
The index of the character, where the first character has an index of 0.

**Returns**
:

The rotation angle.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the *charnum* is negative or if *charnum* is greater than or equal to the number of characters at this node.
**<a id="__svg__SVGTextContentElement__getCharNumAtPosition"></a>long **getCharNumAtPosition**(in [SVGPoint](coords.md#InterfaceSVGPoint) *point*)**
:

Returns the index of the character whose corresponding glyph cell bounding box contains the specified point. The calculations assume that all glyphs occupy the full standard glyph cell for the font. If no such character exists, a value of -1 is returned. If multiple such characters exist, the character within the element whose glyphs were rendered last (i.e., take into account any reordering such as for bidirectional text) is used. If multiple consecutive characters are rendered inseparably (e.g., as a single glyph or a sequence of glyphs), then the user agent shall allocate an equal percentage of the text advance amount to each of the contributing characters in determining which of the characters is chosen.

**Parameters**
:

1.
[SVGPoint](coords.md#InterfaceSVGPoint) *point*
A point in user space.

**Returns**
:

The index of the character which is at the given point, where the first character has an index of 0.

**<a id="__svg__SVGTextContentElement__selectSubString"></a>void **selectSubString**(in unsigned long *charnum*, in unsigned long *nchars*)**
:

Causes the specified substring to be selected just as if the user selected the substring interactively.

**Parameters**
:

1.
unsigned long *charnum*
The index of the start character which is at the given point, where the first character has an index of 0.
2.
unsigned long *nchars*
The number of characters in the substring. If nchars specifies more characters than are available, then the substring will consist of all characters starting with charnum until the end of the list of characters.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if *charnum* or *nchars* is negative or if *charnum* is greater than or equal to the number of characters at this node.
### <a id="InterfaceSVGTextPositioningElement"></a>10.17.2 Interface SVGTextPositioningElement

The [SVGTextPositioningElement](#InterfaceSVGTextPositioningElement) interface is inherited by text-related interfaces: [SVGTextElement](#InterfaceSVGTextElement), [SVGTSpanElement](#InterfaceSVGTSpanElement), [SVGTRefElement](#InterfaceSVGTRefElement) and [SVGAltGlyphElement](#InterfaceSVGAltGlyphElement).

```
interface SVGTextPositioningElement : SVGTextContentElement {
  readonly attribute SVGAnimatedLengthList x;
  readonly attribute SVGAnimatedLengthList y;
  readonly attribute SVGAnimatedLengthList dx;
  readonly attribute SVGAnimatedLengthList dy;
  readonly attribute SVGAnimatedNumberList rotate;
};
```

**Attributes:**
:

**<a id="__svg__SVGTextPositioningElement__x"></a>**x** (readonly [SVGAnimatedLengthList](types.md#InterfaceSVGAnimatedLengthList))**
:

Corresponds to attribute ‘x’ on the given element.

**<a id="__svg__SVGTextPositioningElement__y"></a>**y** (readonly [SVGAnimatedLengthList](types.md#InterfaceSVGAnimatedLengthList))**
:

Corresponds to attribute ‘y’ on the given element.

**<a id="__svg__SVGTextPositioningElement__dx"></a>**dx** (readonly [SVGAnimatedLengthList](types.md#InterfaceSVGAnimatedLengthList))**
:

Corresponds to attribute ‘dx’ on the given element.

**<a id="__svg__SVGTextPositioningElement__dy"></a>**dy** (readonly [SVGAnimatedLengthList](types.md#InterfaceSVGAnimatedLengthList))**
:

Corresponds to attribute ‘dy’ on the given element.

**<a id="__svg__SVGTextPositioningElement__rotate"></a>**rotate** (readonly [SVGAnimatedNumberList](types.md#InterfaceSVGAnimatedNumberList))**
:

Corresponds to attribute ‘rotate’ on the given element.

### <a id="InterfaceSVGTextElement"></a>10.17.3 Interface SVGTextElement

The [SVGTextElement](#InterfaceSVGTextElement) interface corresponds to the [‘text’](#TextElement) element.

```
interface SVGTextElement : SVGTextPositioningElement,
                           SVGTransformable {
};
```

### <a id="InterfaceSVGTSpanElement"></a>10.17.4 Interface SVGTSpanElement

The [SVGTSpanElement](#InterfaceSVGTSpanElement) interface corresponds to the [‘tspan’](#TSpanElement) element.

```
interface SVGTSpanElement : SVGTextPositioningElement {
};
```

### <a id="InterfaceSVGTRefElement"></a>10.17.5 Interface SVGTRefElement

The [SVGTRefElement](#InterfaceSVGTRefElement) interface corresponds to the [‘tref’](#TRefElement) element.

```
interface SVGTRefElement : SVGTextPositioningElement,
                           SVGURIReference {
};
```

### <a id="InterfaceSVGTextPathElement"></a>10.17.6 Interface SVGTextPathElement

The [SVGTextPathElement](#InterfaceSVGTextPathElement) interface corresponds to the [‘textPath’](#TextPathElement) element.

```
interface SVGTextPathElement : SVGTextContentElement,
                               SVGURIReference {

  // textPath Method Types
  const unsigned short TEXTPATH_METHODTYPE_UNKNOWN = 0;
  const unsigned short TEXTPATH_METHODTYPE_ALIGN = 1;
  const unsigned short TEXTPATH_METHODTYPE_STRETCH = 2;

  // textPath Spacing Types
  const unsigned short TEXTPATH_SPACINGTYPE_UNKNOWN = 0;
  const unsigned short TEXTPATH_SPACINGTYPE_AUTO = 1;
  const unsigned short TEXTPATH_SPACINGTYPE_EXACT = 2;

  readonly attribute SVGAnimatedLength startOffset;
  readonly attribute SVGAnimatedEnumeration method;
  readonly attribute SVGAnimatedEnumeration spacing;
};
```

**Constants in group “textPath Method Types”:**
:

**<a id="__svg__SVGTextPathElement__TEXTPATH_METHODTYPE_UNKNOWN"></a>**TEXTPATH_METHODTYPE_UNKNOWN** (unsigned short)**
:

The enumeration was set to a value that is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGTextPathElement__TEXTPATH_METHODTYPE_ALIGN"></a>**TEXTPATH_METHODTYPE_ALIGN** (unsigned short)**
:

Corresponds to value 'align'.

**<a id="__svg__SVGTextPathElement__TEXTPATH_METHODTYPE_STRETCH"></a>**TEXTPATH_METHODTYPE_STRETCH** (unsigned short)**
:

Corresponds to value 'stretch'.

**Constants in group “textPath Spacing Types”:**
:

**<a id="__svg__SVGTextPathElement__TEXTPATH_SPACINGTYPE_UNKNOWN"></a>**TEXTPATH_SPACINGTYPE_UNKNOWN** (unsigned short)**
:

The enumeration was set to a value that is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGTextPathElement__TEXTPATH_SPACINGTYPE_AUTO"></a>**TEXTPATH_SPACINGTYPE_AUTO** (unsigned short)**
:

Corresponds to value 'auto'.

**<a id="__svg__SVGTextPathElement__TEXTPATH_SPACINGTYPE_EXACT"></a>**TEXTPATH_SPACINGTYPE_EXACT** (unsigned short)**
:

Corresponds to value 'exact'.

**Attributes:**
:

**<a id="__svg__SVGTextPathElement__startOffset"></a>**startOffset** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘startOffset’](#TextPathElementStartOffsetAttribute) on the given [‘textPath’](#TextPathElement) element.

**<a id="__svg__SVGTextPathElement__method"></a>**method** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘method’](#TextPathElementMethodAttribute) on the given [‘textPath’](#TextPathElement) element.

**<a id="__svg__SVGTextPathElement__spacing"></a>**spacing** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘spacing’](#TextPathElementSpacingAttribute) on the given [‘textPath’](#TextPathElement) element.

### <a id="InterfaceSVGAltGlyphElement"></a>10.17.7 Interface SVGAltGlyphElement

The [SVGAltGlyphElement](#InterfaceSVGAltGlyphElement) interface corresponds to the [‘altGlyph’](#AltGlyphElement) element.

```
interface SVGAltGlyphElement : SVGTextPositioningElement,
                               SVGURIReference {
  attribute DOMString glyphRef setraises(DOMException);
  attribute DOMString format setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGAltGlyphElement__glyphRef"></a>**glyphRef** (DOMString)**
:

Corresponds to attribute [‘glyphRef’](#AltGlyphElementGlyphRefAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGAltGlyphElement__format"></a>**format** (DOMString)**
:

Corresponds to attribute [‘format’](#AltGlyphElementFormatAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGAltGlyphDefElement"></a>10.17.8 Interface SVGAltGlyphDefElement

The [SVGAltGlyphDefElement](#InterfaceSVGAltGlyphDefElement) interface corresponds to the [‘altGlyphDef’](#AltGlyphDefElement) element.

```
interface SVGAltGlyphDefElement : SVGElement {
};
```

### <a id="InterfaceSVGAltGlyphItemElement"></a>10.17.9 Interface SVGAltGlyphItemElement

The [SVGAltGlyphItemElement](#InterfaceSVGAltGlyphItemElement) interface corresponds to the [‘altGlyphItem’](#AltGlyphItemElement) element.

```
interface SVGAltGlyphItemElement : SVGElement {
};
```

### <a id="InterfaceSVGGlyphRefElement"></a>10.17.10 Interface SVGGlyphRefElement

The [SVGGlyphRefElement](#InterfaceSVGGlyphRefElement) interface corresponds to the [‘glyphRef’](#GlyphRefElement) element.

```
interface SVGGlyphRefElement : SVGElement,
                               SVGURIReference,
                               SVGStylable {
  attribute DOMString glyphRef setraises(DOMException);
  attribute DOMString format setraises(DOMException);
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float dx setraises(DOMException);
  attribute float dy setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGGlyphRefElement__glyphRef"></a>**glyphRef** (DOMString)**
:

Corresponds to attribute [‘glyphRef’](#GlyphRefElementGlyphRefAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGGlyphRefElement__format"></a>**format** (DOMString)**
:

Corresponds to attribute [‘format’](#GlyphRefElementFormatAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGGlyphRefElement__x"></a>**x** (float)**
:

Corresponds to attribute [‘x’](#GlyphRefElementXAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGGlyphRefElement__y"></a>**y** (float)**
:

Corresponds to attribute [‘y’](#GlyphRefElementYAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGGlyphRefElement__dx"></a>**dx** (float)**
:

Corresponds to attribute [‘dx’](#GlyphRefElementDXAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGGlyphRefElement__dy"></a>**dy** (float)**
:

Corresponds to attribute [‘dy’](#GlyphRefElementDYAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](shapes.md) ⋅ [Next](painting.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
