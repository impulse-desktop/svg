<!-- https://svgwg.org/svg2-draft/text.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 11: Text

## <a id="Introduction"></a>11.1. Introduction

Text that is to be rendered as part of an SVG document fragment is specified using the ‘[text](#TextElement)’ element. The text within a ‘[text](#TextElement)’ element can be rendered:

- pre-formatted (with limited line wrapping),
- auto-wrapped (if a [content area](#TermContentArea) is provided),
- on a path (if a ‘[textPath](#TextPathElement)’ element is provided).

The section [Text layout](#TextLayout) gives an introduction to text layout. It is followed by sections covering [content areas](#TextLayoutContentArea) and the [algorithm](#TextLayoutAlgorithm) for laying out text within a [content area](#TermContentArea). The specialized layout rules corresponding to text that is [pre-formatted](#TextLayoutPre), [auto-wrapped](#TextLayoutAuto), and [on a path](#TextLayoutPath) are then addressed in individual sections.

Rules for text layout in SVG 1.1 are mostly defined within the SVG 1.1 specification. The rules mirror to a large extent those found in CSS. In SVG 2, the dependence on CSS is more explicit. In practice the resulting layout is the same. The change to directly relying on CSS specifications simplifies the SVG specification while making it more obvious that rendering agents can use the same code to render both text in HTML and in SVG. In particular, SVG 2 auto-wrapped text is based on CSS text layout.

SVG's ‘[text](#TextElement)’ elements are rendered like other graphics elements. Thus, [coordinate system transformations](coords.md), [painting](painting.md), [clipping](render.md#ClippingAndMasking) and [masking](render.md#ClippingAndMasking) features apply to ‘[text](#TextElement)’ elements in the same way as they apply to [shapes](shapes.md#TermShapeElement) such as [paths](paths.md) and [rectangles](shapes.md#RectElement).

SVG text supports advanced typographic features including:

- choice of typeface,
- use of discretionary, historical, or stylistic ligatures,
- text decorations (underlining, over-lining, etc.).

SVG text supports international text processing needs such as:

- horizontal and vertical orientation of text,
- left-to-right or bidirectional text (i.e., languages which intermix right-to-left and left-to-right text, such as Arabic and Hebrew),
- complex text layout where: there is not always a one-to-one correspondence between characters and glyphs, characters may change shape depending on location (e.g. Arabic), and characters may be reordered or combined depending on context (e.g. Devanagari),
- glyph alignment to different baselines (*alphabetic* for western scripts, *hanging* for northern Indic scripts, and *ideographic* for far-eastern scripts).

Multi-language SVG content is possible by [substituting different text strings based on the user's preferred language](struct.md#SwitchElement).

The characters to be drawn are expressed as character data ([[xml](refs.md#ref-xml)], section 2.4) inside the ‘[text](#TextElement)’ element. As a result:

- Text data in SVG content is readily accessible to the visually impaired (see [Accessibility Support](access.md)).
- In many viewing scenarios, the user will be able to search for and select text strings and copy selected text strings to the system clipboard (see [Text selection and clipboard operations](#TextSelection)).
- XML-compatible Web search engines will find text strings in SVG content with no additional effort over what they need to do to find text strings in other XML documents.

For accessibility reasons, it is recommended that text that is included in a document have appropriate semantic markup to indicate its function. For example, a text element that provides a visible label for part of a diagram should have an ‘[id](struct.md#IDAttribute)’ that is referenced by an ‘aria-labelledby’ attribute on the relevant group or path element. See [SVG accessibility guidelines](access.md) for more information.

### <a id="Definitions"></a>11.1.1. Definitions

***<a id="TermCharacter"></a>character***
: A character is an atomic unit of text as defined in XML [XML].

Essentially, a Unicode code point. A character may be a control instruction (such as a tab, carriage return, or line feed), a renderable mark (letter, digit, punctuation or other symbol), or a modifier (such as a combining accent).

***<a id="TermAddressableCharacter"></a>addressable character***
: A [character](#TermCharacter) that is addressable by text positioning attributes and SVG DOM text methods. Characters discarded during layout such as collapsed white space characters are not addressable, neither are characters within an element with a value of none for the [display](render.md#VisibilityControl) property. Addressable characters are addressed by their index. Indexes are determined prior to applying any text-transform conversions, as described for the methods in the [SVGTextContentElement](#InterfaceSVGTextContentElement) interface. There are two methods to map an index to a character; the choice of which to use depends on purpose:

1. For the purposes of mapping text positioning attributes, the index is measured in Unicode code points (thus a 'u' followed by the combining diaeresis ' ̈' is counted as two characters while the precomposed character 'ü' is counted as one character).
2. For the purposes of SVG DOM text methods, the index is measured in UTF-16 code units (thus, a single Unicode code point above U+FFFF will map to two addressable characters as a UTF-16 code unit consists of 16 bits).

The different methods are backwards compatible with SVG 1.1. The use of UTF-16 code units for SVG DOM text methods is required from the definition of DOMString in the DOM 2 specification.

If support for CSS generated-content text is introduced in the future, it would be included in the array of addressable characters.

***<a id="TermTypographicCharacterUnit"></a>typographic character***
: A unit of a writing system— such as a Latin alphabetic letter (including its diacritics), Hangul syllable, Chinese ideographic character, Myanmar syllable cluster— that is indivisible with respect to a particular typographic operation (line-breaking, first-letter effects, tracking, justification, vertical arrangement, etc.). For the normative definition and the relationship between this and a Unicode grapheme cluster, see CSS Text Module Level 3, ([[css-text-3](refs.md#ref-css-text-3)]).
***<a id="TermFont"></a>font***
: A font represents an organized collection of [glyphs](#TermGlyph) in which the various glyph representations will share a particular appearance or styling.
***<a id="TermGlyph"></a>glyph***
: A glyph represents a unit of rendered content within a [font](#TermFont). Often, there is a one-to-one correspondence between characters to be drawn and corresponding glyphs (e.g., usually the character "A" is rendered using a single glyph), but other times multiple glyphs are used to render a single character (e.g., characters with accents) or a single glyph can be used to render multiple characters (e.g., ligatures). Typically, a glyph is defined by one or more [shapes](shapes.md#TermShapeElement) such as a [path](paths.md), possibly with additional information such as rendering hints that help a font engine to produce legible text in small sizes.
***<a id="TermTextContentElement"></a>text content element***
: A text content element is an SVG element that causes a text string to be rendered onto the canvas. The SVG text content elements are: ‘[text](#TextElement)’, ‘[textPath](#TextPathElement)’ and ‘[tspan](#TextElement)’.
***<a id="TermTextContentChildElement"></a>text content child element***
: A text content child element is a [text content element](#TermTextContentElement) that is allowed as a descendant of another [text content element](#TermTextContentElement). In SVG the text content child elements are: ‘[textPath](#TextPathElement)’ and ‘[tspan](#TextElement)’.
***<a id="TermTextContentBlockElement"></a>text content block element***
: A text content block element is a [text content element](#TermTextContentElement) that serves as a standalone element for a unit of text, and which may optionally contain certain child [text content elements](#TermTextContentElement) (e.g. [‘tspan’](#TextElement)). SVG 2 defines a single text content block element: ‘[text](#TextElement)’.
***<a id="TermContentArea"></a>content area***
: The area in which the text is normally laid out. This is equivalent to the CSS content area. The actual region where text layout occurs may be smaller due to padding and/or exclusions.
***<a id="TermWrappingContext"></a>wrapping context***
: One or more regions (shapes) to be excluded from the content area during text layout. This is the same as the CSS wrapping context.
***<a id="TermWrappingArea"></a>wrapping area***
: The area in which the text is laid out after subtracting any padding or exclude areas ([wrapping context](#TermWrappingContext)). This is the same as the CSS wrapping area.
***<a id="TermLineBox"></a>line box***
: The rectangular area containing all the content used to layout a single line of text. This is the same as the CSS line box.
***<a id="TermInlineBaseDirection"></a>inline-base direction***
: The primary direction in which content is ordered within a line or part of a line of text. It defines the start and end sides of a line or part of a line of text (relevant, for example to how the [text-anchor](#TextAnchorProperty) property is applied). It is determined by the [direction](#DirectionProperty) property. (Note: the ordering of characters in a line of text is primary controlled by the Unicode bidi algorithm and not the inline-base direction.)
***<a id="TermBlockFlowDirection"></a>block-flow direction***
: The direction in which [line boxes](#TermLineBox) are stacked. It is determined by the [writing-mode](#WritingModeProperty) property.
***<a id="TermAlignmentPoint"></a>alignment point***
: The point on a typographic character that should be aligned with the [current text position](#TermCurrentTextPosition). It is determined by the glyph cell metrics and may depend on the script and [inline-base direction](#TermInlineBaseDirection).
***<a id="TermCurrentTextPosition"></a>current text position***
: The point in the current user space where the [alignment point](#TermAlignmentPoint) of the next typographic character to be rendered should be placed.
***<a id="TermTextChunk"></a>text chunk***
: An independent block of text in which all characters are positioned together. Each new absolute positioning adjustment (due to an ‘[x](#TextElementXAttribute)’ or ‘[y](#TextElementYAttribute)’ attribute, or forced line break) creates a new [text chunk](#TermTextChunk). Ligature substitution and bidi-reordering only occur within a [text chunk](#TermTextChunk). [Text chunks](#TermTextChunk) are only relevant to pre-formatted text.
***<a id="TermWhiteSpaceCharacter"></a>white space characters***
: The following characters are considered white space characters: U+0009 CHARACTER TABULATION, U+000C FORM FEED (FF), U+000D CARRIAGE RETURN (CR), U+000A LINE FEED (LF) and U+0020 SPACE.
### <a id="FontsGlyphs"></a>11.1.2. Fonts and glyphs

A font consists of a collection of glyphs together with other information (collectively, the font tables) necessary to use those glyphs to present characters on some visual medium. The combination of the collection of glyphs and the font tables is called the *font data*.

A font may supply substitution and positioning tables that can be used by a formatter (text shaper) to re-order, combine and position a sequence of glyphs to form one or more composite glyphs. The combining may be as simple as a ligature, or as complex as an indic syllable which combines, usually with some re-ordering, multiple consonants and vowel glyphs. The tables may be language dependent, allowing the use of language appropriate letter forms.

When a glyph, simple or composite, represents an indivisible unit for typesetting purposes, it is know as a [typographic character](#TermTypographicCharacterUnit).

Ligatures are an important feature of advance text layout. Some ligatures are discretionary while others (e.g. in Arabic) are required. The following explicit rules apply to ligature formation:

-
Ligature formation should not be enabled when characters are in different DOM text nodes; thus, characters separated by markup should not use ligatures.
-
Ligature formation should not be enabled when characters are in different [text chunks](#TermTextChunk).
-
Discretionary ligatures should not be used when the spacing between two characters is not the same as the default space (e.g. when letter-spacing has a non-default value, or text-align has a value of justify and text-justify has a value of distribute). (See CSS Text Module Level 3, ([[css-text-3](refs.md#ref-css-text-3)]).
SVG attributes such as ‘[dx](#TextElementDXAttribute)’, ‘[textLength](#TextElementTextLengthAttribute)’, and ‘[spacing](#TextPathElementSpacingAttribute)’ (in ‘[textPath](#TextPathElement)’) that may reposition [typographic characters](#TermTypographicCharacterUnit) do not break discretionary ligatures. If discretionary ligatures are not desired they can be turned off by using the font-variant-ligatures property.
For OpenType fonts, discretionary ligatures include those enabled by the liga, clig, dlig, hlig, and cala features; required ligatures are found in the rlig feature.

Proper text rendering may depend on using the same font as used during authoring. For this reason SVG requires support for downloadable fonts as defined in the Font Resources section of the CSS Fonts Module. In particular, support for the Web Open Font Format [[WOFF](refs.md#ref-woff)] is required.

New in SVG 2, WOFF allows authors to provide the fonts needed to properly render their content. This includes ensuring that the fonts have the proper OpenType tables to support complex scripts, discretionary ligatures, swashes, old-style numbers, and so on. WOFF also allows the fonts to be compressed, subsetted, and include licensing information.

### <a id="GlyphsMetrics"></a>11.1.3. Glyph metrics and layout

Glyph selection and positioning is normally handled according to the rules of CSS. In some cases, however, the final layout of text in SVG requires knowledge of the geometry properties of individual glyphs.

The geometric font characteristics are expressed in a coordinate system based on the EM box. (The EM is a relative measure of the height of the glyphs in the font.) The box 1 EM high and 1 EM wide is called the *design space*. This space is given a geometric coordinates by sub-dividing the EM into a number of *units per em*.

Units per em is a font characteristic. A typical value for units per em is 1000 or 2048.

The coordinate space of the EM box is called the *design space coordinate system*. For scalable fonts, the curves and lines that are used to draw a glyph are represented using this coordinate system.

Most often, the (0,0) point in this coordinate system is positioned on the left edge of the EM box, but not at the bottom left corner. The Y coordinate of the bottom of a roman capital letter is usually zero. The descenders on lowercase roman letters have negative coordinate values.

[image: An 'M' inside an EM box showing the coordinate system, baseline, ascent and descent.]

An 'M' inside an Em box (blue square). The 'M' sits on the baseline (blue line). The origin of the coordinate system is shown by the small black circle.

SVG assumes that the font tables will provide at least three font characteristics: an ascent, a descent and a set of baseline-tables. The ascent is the distance to the top of the EM box from the (0,0) point of the font; the descent is the distance to the bottom of the EM box from the (0.0) point of the font. The baseline-table is explained below.

Within an OpenType font ([[OPENTYPE](refs.md#ref-opentype)]), for horizontal writing-modes, the ascent and descent are given by the sTypoAscender and sTypoDescender entries in the OS/2 table. For vertical writing-modes, the descent (the distance, in this case from the (0,0) point to the left edge of the glyph) is normally zero because the (0,0) point is on the left edge. The ascent for vertical writing-modes is either 1 em or is specified by the ideographic top baseline value in the OpenType Base table for vertical writing-modes.

Glyphs are positioned relative to a particular point on each glyph known as the [alignment point](#TermAlignmentPoint). For horizontal writing-modes, the glyphs' [alignment points](#TermAlignmentPoint) are vertically aligned while for vertical writing-modes, they are horizontally aligned. The position of the [alignment point](#TermAlignmentPoint) depends on the script. For example, Western glyphs are aligned at the bottom of capital letters, northern Indic glyphs are aligned at the top of a horizontal stroke near the top of the glyphs, and far-eastern glyphs are aligned either at the bottom or center of the glyph.

Within a script and within a line of text having a single font-size, the sequence of alignment points defines, in the [inline-base direction](#TermInlineBaseDirection), a geometric line called a *<a id="TermBaseline"></a>baseline*. Western and most other alphabetic and syllabic glyphs are aligned to an "alphabetic" baseline, the northern Indic glyphs are aligned to a "hanging" baseline and the far-eastern glyphs are aligned to an "ideographic" baseline.

[image: Baseline example in three different scripts.]

Example baselines (red lines) in three different scripts. From left to right: alphabetic, hanging, ideographic. The EM box is shown in blue for the ideographic script.

As glyphs are sequentially placed along a baseline, the [alignment point](#TermAlignmentPoint) of a glyph is typically positioned at the [current text position](#TermCurrentTextPosition) (some properties such as vertical-align may alter the positioning). After each glyph is placed, the [current text position](#TermCurrentTextPosition) is advanced by the glyph's *advance* value (typically the width for horizontal text or height for vertical text) with any correction for kerning or other spacing adjustment as well as for new lines in pre-formatted or auto-wrapped text. The initial and final current text positions are used for alignment (e.g. when the [text-anchor](#TextAnchorProperty) value is either 'middle' or 'end'). The glyph's advance is needed when placing text along a path.

[image: Baseline example in three different scripts.]

Example of font metrics. The blue boxes show the geometric boxes for the three glyphs. The labeled small circles show the [current text position](#TermCurrentTextPosition) before glyph placement. The small square shows the final [current text position](#TermCurrentTextPosition) after placing the last glyph. Note that the left side of the 'a' glyph's box is not aligned with the right side of the 'V' glyph's box due to kerning.

If a glyph does not provide explicit advance values corresponding to the current glyph orientation, then an appropriate approximation should be used. For vertical text, a suggested approximation is the *em* size.

The initial [current text position](#TermCurrentTextPosition) is established by the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes on the ‘[text](#TextElement)’ element or first rendered ‘[tspan](#TextElement)’ element for pre-formatted text, or auto-wrapped text when the [content area](#TermContentArea) is determined by the [inline-size](#InlineSizeProperty) property. For other auto-wrapped text, the initial [current text position](#TermCurrentTextPosition) is determined by the position of the first rendered glyph after applying the CSS line wrapping algorithm.

A *<a id="TermBaselineTable"></a>baseline-table* specifies the position of one or more baselines in the design space coordinate system. The function of the baseline table is to facilitate the alignment of different scripts with respect to each other when they are mixed on the same text line. Because the desired relative alignments may depend on which script is dominant in a line (or block), there may be a different baseline table for each script. In addition, different alignment positions are needed for horizontal and vertical writing modes. Therefore, the font may have a set of baseline tables: typically, one or more for horizontal writing-modes and zero or more for vertical writing-modes.

Some fonts may not have values for the baseline tables. Heuristics are suggested for approximating the baseline tables in CSS Inline Layout Module Level 3 [css-inline-3] when a given font does not supply baseline tables.

When a different font (or change in font size) is specified in the middle of a run of text, the *<a id="TermDominantBaseline"></a>dominant baseline* determines the baseline used to align glyphs in the new font (new size) to those in the previous font. The [dominant-baseline](#DominantBaselineProperty) property is used to set the dominant baseline.

Alignment between an object relative to its parent is determined by the *<a id="TermAlignmentBaseline"></a>alignment baseline*. It is normally the same baseline as the dominant baseline but by using the shorthand vertical-align property (preferred) or the longhand [alignment-baseline](#AlignmentBaselineProperty) another baseline can be chosen.

The dominant baseline can be temporarily shifted (as needed for superscripts or subscripts) by using either the shorthand vertical-align property (preferred) or the longhand [baseline-shift](#BaselineShiftProperty) property. Note that shifts can be nested, each shift added to the previous shift.

[image: Examples of using the 'vertical-align' property. Left shows '[[z]]' where the inner brackets are smaller. Right shows 'x2' where the '2' is a superscript.]

Examples of using the 'vertical-align' property. Left: 'vertical-align:mathematical' ('alignment-baseline:mathematical') is applied to the ‘[tspan](#TextElement)’ containing '[z]'. The light-blue line shows the position of the mathematical baseline. Right: 'vertical-align:super' ('baseline-shift:super') applied to the ‘[tspan](#TextElement)’ containing '2'. The light-blue lines indicate the shift in baseline.

SVG further assumes that for each glyph in the font data for a font, there are two width values, two alignment-baselines and two alignment points, one each for horizontal writing-modes and the other for vertical writing-modes. (Even though it is specified as a width, for vertical writing-modes the width is used in the vertical direction.) The [inline-base direction](#TermInlineBaseDirection) position of the alignment point is on the start-edge of the glyph.

Additional information on baselines can be found in the CSS Inline Layout Module Level 3 specification. [[css-inline-3](refs.md#ref-css-inline-3)] (Also see: CSS Writing Modes Level 3 specification. [[css-writing-modes-3](refs.md#ref-css-writing-modes-3)])

A single line of text is laid out inside a [line box](#TermLineBox). Multi-line text is produced by stacking these boxes. The height of a [line box](#TermLineBox) is determined by finding the maximum ascent and the maximum descent of all the glyphs in a line of text after applying the effect of the [line-height](#LineHeightProperty) property. The width of a [line box](#TermLineBox) is normally the width of the containing text block. In SVG, when the containing text block does not have a fixed geometry (as with pre-formatted text), the [line box](#TermLineBox) tightly wraps the glyph boxes within the box.

[image: The sentence 'A big word.' where 'big' is in a larger font.]

Example of determining the height of a [line box](#TermLineBox). First each glyph box (small light-blue boxes) is extended vertically above and below according to the [line-height](#LineHeightProperty) property. In this case the [line-height](#LineHeightProperty) property is 125%. The larger glyphs have a font-size of 96px so their extra height is 24px (25% of 96px). The extra height is evenly divided above and below resulting in the red boxes. (For clarity, all glyphs in the same inline element have been grouped together). The final [line box](#TermLineBox) (large light-blue box) is then found using the maximum extents of the red boxes above and below the baseline.

In order to support various international writing systems, [line boxes](#TermLineBox) may be orientated in a horizontal or vertical direction. Text within a vertical [line box](#TermLineBox) flows from top to bottom. Text within a horizontal [line box](#TermLineBox) may flow left-to-right (e.g., modern Latin scripts), right-to-left (e.g., Hebrew or Arabic), or a mixture of left-to-right and right-to-left (bidirectional text).

The processing model for bidirectional text is as follows:

- The user agent processes the characters which are provided in *logical order* (i.e., the order the characters appear in the original document).
- The user agent excludes non-rendered elements and [collapses white space](#WhiteSpace).
- The user agent determines the set of independent blocks within each of which it should apply the Unicode bidirectional algorithm.
  - Each [text chunk](#TermTextChunk) represents an independent block of text.
  - Any change in glyph orientation due to processing of the text-orientation or obsoleted [glyph-orientation-vertical](#GlyphOrientationVerticalProperty) properties will start a new independent blocks of text for bi-directional purposes.
- After processing the Unicode bidirectional algorithm and properties [direction](#DirectionProperty) and unicode-bidi on each of the independent text blocks, the user agent will have a potentially re-ordered list of characters which are now in left-to-right rendering order. Simultaneous with re-ordering of the characters, the ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ attributes on the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements are also re-ordered to maintain the original correspondence between characters and attribute values.

While kerning or ligature processing might be font-specific, the preferred model is that kerning and ligature processing occurs between combinations of characters or glyphs after the characters have been re-ordered.

The orientation of [line boxes](#TermLineBox) as well as the direction in which they are stacked ([block-flow direction](#TermBlockFlowDirection)) is determined by the [writing-mode](#WritingModeProperty) property. For horizontal text ([writing-mode](#WritingModeProperty) value horizontal-tb) [line boxes](#TermLineBox) are stacked from top to bottom. For vertical text, [line boxes](#TermLineBox) are stacked from right-to-left ([writing-mode](#WritingModeProperty) value vertical-rl) or left-to-right ([writing-mode](#WritingModeProperty) value vertical-lr).

## <a id="TextElement"></a>11.2. The ‘text’ and ‘tspan’ elements

The ‘[text](#TextElement)’ element defines a graphics element consisting of text. The ‘[tspan](#TextElement)’ element within a ‘[text](#TextElement)’ or another ‘[tspan](#TextElement)’ element, allows one to switch the style and/or adjust the position of the rendered text inside the ‘[tspan](#TextElement)’ element relative to the parent element.

The character data within the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements, along with relevant attributes and properties, and character-to-glyph mapping tables within the font itself, define the glyphs to be rendered. The attributes and properties on the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements indicate such things as the writing direction, font specification, and painting attributes which describe how exactly to render the characters. Subsequent sections of this chapter describe the relevant text-specific attributes and properties.

Since ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements are rendered using the same rendering methods as other graphics elements, all of the same [painting](painting.md) features that apply to [shapes](shapes.md#TermShapeElement) such as [paths](paths.md) and [rectangles](shapes.md#RectElement) also apply to ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements, except for [markers](painting.md#Markers). In addition, [coordinate system transformations](coords.md), [clipping](render.md#ClippingAndMasking), and [masking](render.md#ClippingAndMasking) can be applied to the ‘[text](#TextElement)’ element as a whole.

In CSS terms, the ‘[text](#TextElement)’ element acts as a block element. The ‘[tspan](#TextElement)’, ‘[textPath](#TextPathElement)’, and ‘[a](linking.md#AElement)’ elements that are descended from [text content elements](#TermTextContentElement) act as inline elements.

<a id="ObjectBoundingBoxUnitsTextObjects"></a> It is possible to apply a gradient, pattern, clipping path, mask or filter to text. When one of these facilities is applied to text and keyword 'objectBoundingBox' is used (see [Object bounding box units](coords.md#ObjectBoundingBox)) to specify a graphical effect relative to the "object bounding box", then the object bounding box units are computed relative to the entire ‘[text](#TextElement)’ element in all cases, even when different effects are applied to different ‘[tspan](#TextElement)’ or ‘[textPath](#TextPathElement)’ elements within the same ‘[text](#TextElement)’ element.

The ‘[text](#TextElement)’ element renders its first glyph (after bidirectionality reordering) at the initial [current text position](#TermCurrentTextPosition) (with possible adjustments due to the value of the [text-anchor](#TextAnchorProperty) property or the text-align property). For pre-formatted text and for auto-wrapped text where the [content area](#TermContentArea) is determined by the [inline-size](#InlineSizeProperty) property, the initial [current text position](#TermCurrentTextPosition) is determined by the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ values of the ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ element which contains the first rendered character. For auto-wrapped text in a shape or text on a path see the [Auto-wrapped text](#TextLayoutAuto) or [Text on a path](#TextLayoutPath) sections, respectively, to determine the initial [current text position](#TermCurrentTextPosition). After the glyph(s) corresponding to the given character is (are) rendered, the current text position is updated for the next character. In the simplest case, the new current text position is the previous current text position plus the glyphs' advance value (horizontal or vertical). See [text layout](#TextLayoutAlgorithm) for a description of glyph placement and glyph advance.

The text string Hello, out there! is rendered onto the canvas using the Verdana font family with the glyphs filled with the color blue.

```
<?xml version="1.0" standalone="no"?>
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">

  <text x="250" y="180"
        font-family="Verdana" font-size="64" fill="blue" >
    Hello, out there!
  </text>

</svg>
```

[image: Image showing the blue text.]

A ‘[tspan](#TextElement)’ is used to change the styling of the word not.

```
<?xml version="1.0" standalone="no"?>
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">

  <g font-family="Verdana" font-size="64" >
    <text x="160" y="180" fill="blue" >
      You are
      <tspan font-weight="bold" fill="red" >not</tspan>
      a banana.
    </text>
  </g>

</svg>
```

[image: Blue text except the word 'not' is red.]

Two ‘[tspan](#TextElement)’ elements are repositioned horizontally and vertically using the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes. Because all the text is within a single ‘[text](#TextElement)’ element, a user will be able to select through all the text and copy it to the system clipboard in user agents that support [text selection and clipboard operations](#TextSelection).

```
<?xml version="1.0" standalone="no"?>
<svg width="10cm" height="3cm" viewBox="0 0 1000 300"
     xmlns="http://www.w3.org/2000/svg" version="1.1">

  <g font-family="Verdana" font-size="64" >
    <text x="100" y="180" fill="blue" >
      But you
      <tspan dx="2em" dy="-50" font-weight="bold" fill="red" >
        are
      </tspan>
      <tspan dy="100">
        a peach!
      </tspan>
    </text>
  </g>

</svg>
```

[image: A sentence with several shifted words.]

‘*<a id="elementdef-text"></a>text*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [text content element](#TermTextContentElement)
**Content model:**
: Any number of the following elements or character data, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’
- [text content child elements](#TermTextContentChildElement) — ‘[tspan](#TextElement)’, ‘[textPath](#TextPathElement)’

[a](linking.md#AElement), [clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’
- ‘[x](#TextElementXAttribute)’
- ‘[y](#TextElementYAttribute)’
- ‘[dx](#TextElementDXAttribute)’
- ‘[dy](#TextElementDYAttribute)’
- ‘[rotate](#TextElementRotateAttribute)’
- ‘[textLength](#TextElementTextLengthAttribute)’

**DOM Interfaces:**
:

- [SVGTextElement](#InterfaceSVGTextElement)

‘*<a id="elementdef-tspan"></a>tspan*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [text content element](#TermTextContentElement), [text content child element](#TermTextContentChildElement)
**Content model:**
: Any number of the following elements or character data, in any order:

- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[a](linking.md#AElement), animate, [script](interact.md#ScriptElement), set, [style](styling.md#StyleElement), [tspan](#TextElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[x](#TextElementXAttribute)’
- ‘[y](#TextElementYAttribute)’
- ‘[dx](#TextElementDXAttribute)’
- ‘[dy](#TextElementDYAttribute)’
- ‘[rotate](#TextElementRotateAttribute)’
- ‘[textLength](#TextElementTextLengthAttribute)’
- ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’

**DOM Interfaces:**
:

- [SVGTSpanElement](#InterfaceSVGTSpanElement)

### <a id="TSpanAttributes"></a>11.2.1. Attributes

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TextElementXAttribute"></a>x*, *<a id="TextElementYAttribute"></a>y* | [ [ [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| <number> ]+ ]# | 0 for ‘[text](#TextElement)’; (none) for ‘[tspan](#TextElement)’ | yes |

**
:

If a single <length> is provided, then the value represents the new absolute X (Y) coordinate for the [current text position](#TermCurrentTextPosition) for rendering the glyphs that correspond to the first character within this element or any of its descendants.

If a comma- or space-separated list of *n* <length>s is provided, then the values represent new absolute X (Y) coordinates for the [current text position](#TermCurrentTextPosition) for rendering the glyphs corresponding to each of the first *n* [addressable characters](#TermAddressableCharacter) within this element or any of its descendants.

If more <length>s are provided than characters, then the extra <length>s will have no effect on glyph positioning.

If more characters exist than <length>s, or if the attribute is not specified on a ‘[tspan](#TextElement)’, then for each additional character:

1. if an ancestor ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ element specifies an absolute X (Y) coordinate for the given character via an ‘x’ (‘y’) attribute (nearest ancestor has precedence), then that absolute X (Y) coordinate is used as the starting X (Y) coordinate for that character, else
2. the starting X (Y) coordinate for rendering the glyphs corresponding to the given character is the X (Y) coordinate of the resulting [current text position](#TermCurrentTextPosition) from the most recently rendered glyph for the current ‘[text](#TextElement)’ element.

In SVG 2, the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes are not presentation attributes and cannot be set via CSS. This may change in a future version of SVG.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TextElementDXAttribute"></a>dx*, *<a id="TextElementDYAttribute"></a>dy* | [ [ [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| <number> ]+ ]# | (none) | yes |

**
:

If a single <length> is provided, this value represents the new relative X (Y) coordinate for the [current text position](#TermCurrentTextPosition) for rendering the glyphs corresponding to the first character within this element or any of its descendants. The [current text position](#TermCurrentTextPosition) is shifted along the x-axis (y-axis) of the current user coordinate system by <length> before the first character's glyphs are rendered.

If a comma- or space-separated list of *n* <length>s is provided, then the values represent incremental shifts along the x-axis (y-axis) for the [current text position](#TermCurrentTextPosition) before rendering the glyphs corresponding to the first *n* [addressable characters](#TermAddressableCharacter) within this element or any of its descendants. Thus, before the glyphs are rendered corresponding to each character, the [current text position](#TermCurrentTextPosition) resulting from drawing the glyphs for the previous character within the current ‘[text](#TextElement)’ element is shifted along the x-axis (y-axis) of the current user coordinate system by <length>.

If more <length>s are provided than characters, then any extra <length>s will have no effect on glyph positioning.

If more characters exist than <length>s, or if the attribute is not specified, then for each additional character:

1. if an ancestor ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ element specifies a relative X (Y) coordinate for the given character via a ‘dx’ (‘dy’) attribute (nearest ancestor has precedence), then the [current text position](#TermCurrentTextPosition) is shifted along the x-axis (y-axis) of the current user coordinate system by that amount, else
2. no extra shift along the x-axis (y-axis) occurs.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TextElementRotateAttribute"></a>rotate* | [ <number>+ ]# | (none) | yes (non-additive). |

**
:

The supplemental rotation, in degrees, about the [current text position](#TermCurrentTextPosition) that will be applied to all of the glyphs corresponding to each character within this element.

If a comma- or space-separated list of <number>s is provided, then the first <number> represents the supplemental rotation for the glyphs corresponding to the first character within this element or any of its descendants, the second <number> represents the supplemental rotation for the glyphs that correspond to the second character, and so on.

If more <number>s are provided than there are characters, then the extra <number>s will be ignored.

If more characters are provided than <number>s, then for each of these extra characters the rotation value specified by the last number must be used.

If the attribute is not specified and if an ancestor of a ‘[tspan](#TextElement)’ element specifies a supplemental rotation for a given character via a ‘[rotate](#TextElementRotateAttribute)’ attribute (nearest ancestor has precedence), then the given supplemental rotation is applied to the given character. If there are more characters than <number>s specified in the ancestor's ‘[rotate](#TextElementRotateAttribute)’ attribute, then for each of these extra characters the rotation value specified by the last number must be used.

This supplemental rotation has no impact on the rules by which [current text position](#TermCurrentTextPosition) is modified as glyphs get rendered and is supplemental to any rotation due to [text on a path](#TextLayoutPath) and to text-orientation, [glyph-orientation-horizontal](#GlyphOrientationHorizontalProperty), or [glyph-orientation-vertical](#GlyphOrientationVerticalProperty).

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TextElementTextLengthAttribute"></a>textLength* | [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| <number> | See below | yes |

**
:

The author's computation of the total sum of all of the advance values that correspond to character data within this element, including the advance value on the glyph (horizontal or vertical), the effect of properties letter-spacing and word-spacing and adjustments due to attributes ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ on this ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ element or any descendants. This value is used to calibrate the user agent's own calculations with that of the author.

The purpose of this attribute is to allow the author to achieve exact alignment, in visual rendering order after any bidirectional reordering, for the first and last rendered glyphs that correspond to this element; thus, for the last rendered character (in visual rendering order after any bidirectional reordering), any supplemental inter-character spacing beyond normal glyph advances are ignored (in most cases) when the user agent determines the appropriate amount to expand/compress the text string to fit within a length of ‘[textLength](#TextElementTextLengthAttribute)’.

If attribute ‘[textLength](#TextElementTextLengthAttribute)’ is specified on a given element and also specified on an ancestor, the adjustments on all character data within this element are controlled by the value of ‘[textLength](#TextElementTextLengthAttribute)’ on this element exclusively, with the possible side-effect that the adjustment ratio for the contents of this element might be different than the adjustment ratio used for other content that shares the same ancestor. The user agent must assume that the total advance values for the other content within that ancestor is the difference between the advance value on that ancestor and the advance value for this element.

This attribute is not intended for use to obtain effects such as shrinking or expanding text.

A negative value is an error (see [Error processing](conform.md#ErrorProcessing)).

The ‘[textLength](#TextElementTextLengthAttribute)’ attribute is only applied when the [wrapping area](#TermWrappingArea) is not defined by the [shape-inside](#TextShapeInside) or the [inline-size](#InlineSizeProperty) properties. It is also not applied for any ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ element that has forced line breaks (due to a white-space value of pre or pre-line).

If the attribute is not specified anywhere within a ‘[text](#TextElement)’ element, the effect is as if the author's computation exactly matched the value calculated by the user agent; thus, no advance adjustments are made. For the purpose of [reflecting](types.md#TermReflect) the attribute in the DOM, the [initial value](types.md#TermInitialValue) is the current user-agent calculated length, expressed in implicit [user units](coords.md#TermUserUnits).

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TextElementLengthAdjustAttribute"></a>lengthAdjust* | spacing \| spacingAndGlyphs | spacing | yes |

**
:

**spacing**
: Indicates that only the advance values are adjusted. The glyphs themselves are not stretched or compressed.
**spacingAndGlyphs**
: Indicates that the advance values are adjusted and the glyphs themselves stretched or compressed in one axis (i.e., a direction parallel to the [inline-base direction](#TermInlineBaseDirection)).
The user agent is required to achieve correct start and end positions for the text strings, but the locations of intermediate glyphs are not predictable because user agents might employ advanced algorithms to stretch or compress text strings in order to balance correct start and end positioning with optimal typography.

Note that, for a text string that contains *n* characters, the adjustments to the advance values often occur only for *n*−1 characters (see description of attribute ‘[textLength](#TextElementTextLengthAttribute)’), whereas stretching or compressing of the glyphs will be applied to all *n* characters.

### <a id="TSpanNotes"></a>11.2.2. Notes on 'x', 'y', 'dx', 'dy' and 'rotate'

The ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ on the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements are useful in high-end typography scenarios where individual glyphs require exact placement. These attributes are useful for minor positioning adjustments between characters or for major positioning adjustments, such as moving a section of text to a new location to achieve the visual effect of a new line of text (compatible with SVG 1.1). Note that the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ attributes are ignored for auto-wrapped text (except for the initial [current text position](#TermCurrentTextPosition) when the [content area](#TermContentArea) is specified by the [inline-size](#InlineSizeProperty) property).

In situations where micro-level positioning adjustment are necessary for advanced typographic control, the SVG content designer needs to ensure that the necessary font will be available for all viewers of the document (e.g., package up the necessary font data in the form of an SVG font or an alternative WebFont format which is stored at the same Web site as the SVG content) and that the viewing software will process the font in the expected way (the capabilities, characteristics and font layout mechanisms vary greatly from system to system). If the SVG content contains ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, or ‘[dy](#TextElementDYAttribute)’ attribute values which are meant to correspond to a particular font processed by a particular set of viewing software and either of these requirements is not met, then the text might display with poor quality.

The following additional rules apply to attributes ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ when they contain a list of numbers:

- When a single character maps to a single glyph – In this case, the *n*-th value for the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ attributes is applied to the glyph that corresponds to the *n*-th character.
- When a single character maps to multiple glyphs (e.g., when an accent glyph is placed on top of a base glyph) – In this case, the *n*-th value for the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, and ‘[dy](#TextElementDYAttribute)’ values are applied (i.e., the [current text position](#TermCurrentTextPosition) is adjusted) before rendering the first glyph. The rotation transformation corresponding to the *n*-th ‘[rotate](#TextElementRotateAttribute)’ value is applied to the glyphs and to the inter-glyph advance values corresponding to this character on a group basis (i.e., the rotation value creates a temporary new rotated coordinate system, and the glyphs corresponding to the character are rendered into this rotated coordinate system).
- When multiple characters map to a single glyph (e.g., when a ligature is used) – Suppose that the *n*-th and (*n*+1)-th characters map to a single glyph. In this case, the *n*-th value for the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ attributes all apply when rendering the glyph. The (*n*+1)-th values, however, for ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ are ignored (exception: the final ‘[rotate](#TextElementRotateAttribute)’ value in the list would still apply to subsequent characters), whereas the ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ are applied to the subsequent character (i.e., the (*n*+2)-th character), if one exists, by translating the [current text position](#TermCurrentTextPosition) by the given amounts before rendering the first glyph associated with that character.
- When there is a many-to-many mapping of characters to glyphs (e.g., when three characters map to two glyphs, such as when the first glyph expresses the first character and half of the second character, and the second glyph expresses the other half of the second character plus the third character) – Suppose that the *n*-th, (*n*+1)-th and (*n*+2)-th characters map to two glyphs. In this case, the *n*-th value for the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, and ‘[dy](#TextElementDYAttribute)’ values are applied (i.e., the [current text position](#TermCurrentTextPosition) is adjusted) before rendering the first glyph. The rotation transformation corresponding to the *n*-th ‘[rotate](#TextElementRotateAttribute)’ value is applied to both the two glyphs and the glyph advance values for the first glyph on a group basis (i.e., the rotation value creates a temporary new rotated coordinate system, and the two glyphs are rendered into the temporary rotated coordinate system). The (*n*+1)-th and (*n*+2)-th values, however, for the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ attributes are not applied (exception: the final ‘[rotate](#TextElementRotateAttribute)’ value in the list would still apply to subsequent characters), whereas the (*n*+1)-th and (*n*+2)-th values for the ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ attributes are applied to the subsequent character (i.e., the (*n*+3)-th character), if one exists, by translating the [current text position](#TermCurrentTextPosition) by the given amounts before rendering the first glyph associated with that character.
- Relationship to bidirectionality – Text is laid out in a two-step process, where any bidirectional text is first re-ordered into a left-to-right string, and then text layout occurs with the re-ordered text string. Whenever the character data within a ‘[tspan](#TextElement)’ element is re-ordered, the corresponding elements within the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ are also re-ordered to maintain the correspondence. For example, suppose that there is the following ‘[tspan](#TextElement)’ element:
`<tspan dx="11 12 13 14 15 0 21 22 23 0 31 32 33 34 35 36">Latin and Hebrew</tspan>`
and that the word "Hebrew" will be drawn right-to-left. First, the character data and the corresponding values in the ‘[dx](#TextElementDXAttribute)’ list will be reordered, such that the text string will be "Latin and werbeH" and the list of values for the ‘[dx](#TextElementDXAttribute)’ attribute will be "11 12 13 14 15 0 21 22 23 0 36 35 34 33 32 31". After this re-ordering, the glyphs corresponding to the characters will be positioned using standard left-to-right layout rules.

Example tspan04 uses the ‘[rotate](#TextElementRotateAttribute)’ attribute on the ‘[tspan](#TextElement)’ element to rotate the glyphs to be rendered. This example shows a single text string in a ‘[tspan](#TextElement)’ element that contains more characters than the number of values specified in the ‘[rotate](#TextElementRotateAttribute)’ attribute. In this case the last value specified in the ‘[rotate](#TextElementRotateAttribute)’ attribute of the ‘[tspan](#TextElement)’ must be applied to the remaining characters in the string.

```
﻿<?xml version="1.0" standalone="no"?>
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
  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
  fill="none" stroke="blue" stroke-width="2" />
</svg>
```

[image: Example tspan04 — simple rotation of characters in a tspan element]

Example tspan04

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/tspan04.svg)

Example tspan05 specifies the ‘[rotate](#TextElementRotateAttribute)’ attribute on the ‘[text](#TextElement)’ element and on all but one of the child ‘[tspan](#TextElement)’ elements to rotate the glyphs to be rendered. The example demonstrates the propagation of the ‘[rotate](#TextElementRotateAttribute)’ attribute.

```
﻿<?xml version="1.0" standalone="no"?>
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

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="498" height="118" fill="none"
        stroke="blue" stroke-width="2" />
</svg>
```

[image: Example tspan05 — propagation of rotation values to nested tspan elements]

Example tspan05

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/tspan05.svg)

Rotation of red text inside the ‘[text](#TextElement)’ element:

- The ‘[rotate](#TextElementRotateAttribute)’ value will rotate the characters in the text *"Not "* by 5, 15, 25 and 35 degrees respectively.
- A ‘[rotate](#TextElementRotateAttribute)’ value is applied to the space that follows the text *"Not"*, to the space in between the text in the "child1" and "child5" ‘[tspan](#TextElement)’ elements, and to the space before the text *"rotation"*.
- The next current ‘[rotate](#TextElementRotateAttribute)’ value specified is 45 followed by 55. The current ‘[rotate](#TextElementRotateAttribute)’ value in the ‘[text](#TextElement)’ element is incremented as subsequent characters in the text of the child ‘[tspan](#TextElement)’ elements are processed.
- The next immediate ‘[tspan](#TextElement)’ element specifies rotate values for the text, hence the current ‘[rotate](#TextElementRotateAttribute)’ value will change to the next value in the list (but is not used) as each character is processed until the last value of 55 degrees is reached.
- The last ‘[rotate](#TextElementRotateAttribute)’ value of 55 degrees will be applied to all the characters in the text *"rotation"*.

Rotation of the orange text inside the "child1" ‘[tspan](#TextElement)’ element:

- The ‘[rotate](#TextElementRotateAttribute)’ value will rotate the first 4 characters in the text *"all characters "* by -10, -20, -30 and -40 respectively.
- The last ‘[rotate](#TextElementRotateAttribute)’ value of -40 becomes the current ‘[rotate](#TextElementRotateAttribute)’ value and will be applied to all subsequent characters in the ‘[tspan](#TextElement)’ element and to any child ‘[tspan](#TextElement)’ elements that do not specify ‘[rotate](#TextElementRotateAttribute)’ values.
- The "child4" ‘[tspan](#TextElement)’ element does not specify any ‘[rotate](#TextElementRotateAttribute)’ values and thus uses the current ‘[rotate](#TextElementRotateAttribute)’ of its ancestor ("child1" ‘[tspan](#TextElement)’ element). All the characters in the text *"text"* specified within the "child4" ‘[tspan](#TextElement)’ element will be rotated by -40 degrees.
- The last ‘[rotate](#TextElementRotateAttribute)’ value of -40 degrees will be applied to all the characters in the text *"have a"*.
- A ‘[rotate](#TextElementRotateAttribute)’ value is applied to the space in between the text in the "child2" and "child4" ‘[tspan](#TextElement)’ elements, and to the space before the text *"have a"*.

Rotation of the yellow text inside the "child2" ‘[tspan](#TextElement)’ element:

- The ‘[rotate](#TextElementRotateAttribute)’ value will rotate the characters in the (yellow) text *"in "* by 70, 60, and 50 degrees respectively.
- A ‘[rotate](#TextElementRotateAttribute)’ value is applied to the space that follows the text *"in"*.
- There are more ‘[rotate](#TextElementRotateAttribute)’ values specified than characters, thus the additional ‘[rotate](#TextElementRotateAttribute)’ values will be applied to the "child3" ‘[tspan](#TextElement)’ element which does not specified any ‘[rotate](#TextElementRotateAttribute)’ values.
- The characters in the text *"the"* specified within the "child3" ‘[tspan](#TextElement)’ element will be rotated 40, 30 and 20 degrees respectively.

Rotation of the blue text inside the "child5" ‘[tspan](#TextElement)’ element:

- The ‘[rotate](#TextElementRotateAttribute)’ value will rotate all the characters in text *"specified"* by -10 degrees.
- Only one ‘[rotate](#TextElementRotateAttribute)’ value is specified and is thus applied to all characters in the ‘[tspan](#TextElement)’ element.

The following diagram illustrates how the rotation values propagate to ‘[tspan](#TextElement)’ elements nested withing a ‘[text](#TextElement)’ element:

[image: Image that shows propagation of rotation values]

## <a id="TextLayout"></a>11.3. Text layout – Introduction

This section gives a short overview of SVG text layout. It is followed by sections that cover different aspects of text layout in more detail.

Text layout in SVG is a multi-stage process that takes as input a ‘[text](#TextElement)’ element subtree and its property values and produces a sequence of glyphs to render and their positions in each [text content element](#TermTextContentElement)'s coordinate system.

First, a ‘[text](#TextElement)’ element and its descendants are laid out inside a [content area](#TermContentArea) or [wrapping area](#TermWrappingArea) according to CSS, as if the ‘[text](#TextElement)’ were a block element and any ‘[tspan](#TextElement)’, ‘[textPath](#TextPathElement)’, and ‘[a](linking.md#AElement)’ descendants were inline elements. This layout takes into account all paragraph level and font related CSS properties described in this chapter.

The [content area](#TermContentArea) may be explicitly declared by setting the [inline-size](#InlineSizeProperty) property, or by setting the [shape-inside](#TextShapeInside) property that defines or references an SVG [shape](shapes.md#TermShapeElement). If a [content area](#TermContentArea) is not declared, it defaults to a rectangle of infinite width and height.

Second, any positioning given by ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ attributes are applied to the resulting glyph positions from the CSS layout process. The rules for which transforms are allowed depend on if the [content area](#TermContentArea) was explicitly declared or not. If not explicitly declared, the rules define the layout of *pre-formatted* text. If declared, the rules define the layout of *auto-wrapped* text.

Third, the effect of the [text-anchor](#TextAnchorProperty) property is applied if necessary.

Finally, layout of glyphs for any ‘[textPath](#TextPathElement)’ elements is performed, converting *pre-formatted* text to *text-on-a-path*.

Examples of the different types of text layout:

**Pre-formatted:**
: For short strings of text (e.g. labels) or where exact placement of glyphs is required (e.g. hand-kerned titles).

An example of multi-line pre-formatted text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

     <text x="20" y="45" style="font: 24px sans-serif;">
       Example of multi-line,
       <tspan x="20" y="75">pre-formatted text.</tspan>
     </text>

</svg>
```

[image: Image showing two lines of pre-formatted text.]

Pre-formatted text where a ‘[tspan](#TextElement)’ element has been used to create multi-line text.

**Wrapped text:**
: For long strings of text where automatic text wrapping is required.

An example of auto-wrapped text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <text x="20" y="45" style="font: 24px sans-serif; inline-size: 250px;">
    Example of text auto-wrapped.</text>

</svg>
```

[image: Image showing auto-wrapped text on two lines.]

Auto-wrapped text. The [inline-size](#InlineSizeProperty) property defines a rectangular content area of infinite height (shown in light blue).

**Text on path:**
: For text that follows a specified path.

An example of text on a path.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <path id="MyPath" stroke="lightblue" fill="none"
	d="M 50,50 C 100,0 200,100 250,50"/>

  <text style="font: 24px sans-serif;">
    <textPath href="#MyPath">Text on a path.</textPath>
  </text>

</svg>
```

[image: Image showing text following a path.]

Text on a path. The ‘[textPath](#TextPathElement)’ element references a ‘[path](paths.md#PathElement)’ element (shown in light blue).

SVG 2 introduces the ability to automatically wrap text inside a rectangle or other shape by specifying a [content area](#TermContentArea). The design of SVG wrapped text is motivated by the desire that SVG text wrapping be as compatible as possible with text wrapping in CSS in order that renderers that support CSS text wrapping can implement SVG text wrapping easily (but without requiring non-HTML compatible SVG renderers to implement HTML). There are several differences between SVG and CSS text wrapping. The most important is that in SVG, a [content area](#TermContentArea) must be explicitly provided as SVG does not have an automatic finite (or semi-finite) [content area](#TermContentArea) (provided in CSS by the box model). Another difference is that SVG does not have the <p></p> and <br/> elements which create line breaks. Instead, SVG relies on the pre and pre-line values of white-space to provide line breaks. SVG wrapped text also allows a content-creation tool to provide a natural fallback for SVG 1.1 renderers that do not support wrapped text (by use of ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes in the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements, which are ignored by SVG 2 renderers for auto-wrapped text).

SVG's text layout options are designed to cover most general use cases. If more complex layout is required (bulleted lists, tables, etc.), text can be rendered in another XML namespace such as XHTML [[HTML](refs.md#ref-html)] embedded inline within a ‘[foreignObject](embedded.md#ForeignObjectElement)’ element.

## <a id="TextLayoutContentArea"></a>11.4. Text layout – Content Area

A *content area* is defined by specifying in a ‘[text](#TextElement)’ element an [inline-size](#InlineSizeProperty) property, or a [shape-inside](#TextShapeInside) property that defines or references an SVG [shape](shapes.md#TermShapeElement). If no [content area](#TermContentArea) is provided, the [content area](#TermContentArea) defaults to a rectangle of infinite width and height (see the [pre-formatted text](#TextLayoutPre) section). If both an [inline-size](#InlineSizeProperty) property and a [shape-inside](#TextShapeInside) property with value other than 'none' are given, the [shape-inside](#TextShapeInside) property is used.

<a id="TextLayoutWrappingArea"></a> Wrapped text is laid out in a *wrapping area*. The [wrapping area](#TermWrappingArea) is normally the same as the [content area](#TermContentArea). When the [content area](#TermContentArea) is defined using the [shape-inside](#TextShapeInside) property, the [wrapping area](#TermWrappingArea) may be smaller due to the presence of a [shape-subtract](#TextShapeSubtract) property and/or a [shape-padding](#TextShapePadding) property. The [shape-subtract](#TextShapeSubtract) property (along with the [shape-margin](#TextShapeMargin) property) defines a *wrapping context*. The [wrapping area](#TermWrappingArea) is found by insetting the [content area](#TermContentArea) by the [shape-padding](#TextShapePadding) distance, and then subtracting the [wrapping context](#TermWrappingContext).

Once a [wrapping area](#TermWrappingArea) is defined, the text is laid out inside the [wrapping area](#TermWrappingArea) according to the rules of CSS (respecting any special rules given in this section).

Constructing equivalent wrapping areas in SVG and HTML. The text inside the wrapping areas is rendered the same in both cases.

[image: Image showing the creation of an hour glass shape using a circle with two semicircles spaced out horizontally. Text is wrapped in the circle after the left and right semicircles exclude part of the circle.]

Defining a [wrapping area](#TermWrappingArea) in SVG. The ‘[text](#TextElement)’ element has both a [shape-inside](#TextShapeInside) property and a [shape-subtract](#TextShapeSubtract) property. The [shape-inside](#TextShapeInside) property references a circle that defines a [content area](#TermContentArea) (dotted purple line). The [shape-subtract](#TextShapeSubtract) property referencing two semicircles defines a [wrapping context](#TermWrappingContext) (dotted green line) which when subtracted from the [content area](#TermContentArea) results in the [wrapping area](#TermWrappingArea) (light blue line).

[image: Image showing the creation of an hour glass shape using a circle with two semicircles spaced out horizontally. Text is wrapped in the circle after the left and right semicircles exclude part of the circle.]

Defining a [wrapping area](#TermWrappingArea) in HTML. A wrapper <div> contains two float <div>s. The wrapper <div> defines a rectangular region (solid purple line). Its [shape-inside](#TextShapeInside) property defines a [content area](#TermContentArea) within the <div> (dotted purple line). The two other <div>s define two floats, one on the left (solid green line) and the right (solid pink line). The floats are rectangular in shape. Each float has a [shape-outside](../css-shapes-1.md#shape-outside-property) property which defines the [wrapping context](#TermWrappingContext) for each float (dotted green and pink lines). The combined [wrapping context](#TermWrappingContext) is subtracted from the [content area](#TermContentArea) to defined the [wrapping area](#TermWrappingArea) (light blue line).

### <a id="InlineSize"></a>11.4.1. The ‘inline-size’ property

The [inline-size](#InlineSizeProperty) property allows one to set the [wrapping area](#TermWrappingArea) to a rectangular shape. The computed value of the property sets the width of the rectangle for horizontal text and the height of the rectangle for vertical text. The other dimension (height for horizontal text, width for vertical text) is of infinite length. A value of zero disables the creation of a [wrapping area](#TermWrappingArea).

The initial [current text position](#TermCurrentTextPosition) is taken from the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes of the ‘[text](#TextElement)’ element (or first child ‘[tspan](#TextElement)’ element if the attributes are not given on the ‘[text](#TextElement)’ element). For left-to-right text, the initial [current text position](#TermCurrentTextPosition) is at the left of the rectangle. For right-to-left text it is at the right of the rectangle. For vertical text, the initial [current text position](#TermCurrentTextPosition) is at the top of the rectangle.

The rectangle ([wrapping area](#TermWrappingArea)) is then anchored according to the [text-anchor](#TextAnchorProperty) property using the edges of the [wrapping area](#TermWrappingArea) to determine the start, middle, and end positions.

The [inline-size](#InlineSizeProperty) property method to wrap text is an extension to pre-formatted SVG text where the author simply gives a limit to the width or height of the block of text; thus the use of the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes along with the [direction](#DirectionProperty) and [text-anchor](#TextAnchorProperty) properties to position the first line of text. If full justification is needed, the [shape-inside](#TextShapeInside) property should be used to create the [wrapping area](#TermWrappingArea).

| Name: | *<a id="InlineSizeProperty"></a>inline-size* |
| --- | --- |
| Value: | auto \| [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | auto |
| Applies to: | ‘[text](#TextElement)’ elements |
| Inherited: | no |
| Percentages: | Refer to the width (for horizontal text) or height (for vertical text) of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

An example of using [inline-size](#InlineSizeProperty) for wrapping horizontal text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <text x="50" y="30" style="font: 20px sans-serif; inline-size: 200px">
    This text wraps at 200 pixels.
  </text>

</svg>
```

[image: Image showing English text wrapped into two lines.]

Horizontal text wrapping. The light-blue lines indicate the limits of the [content area](#TermContentArea). Note that the content area is of infinite height. The red dot shows the initial [current text position](#TermCurrentTextPosition).

An example of using [inline-size](#InlineSizeProperty) for wrapping right to left horizontal text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <text x="250" y="30"
	style="font: 20px PakType Naqsh; inline-size: 200px; direction: rtl;">
    هذا النص يلتف في 200 بكسل.</text>

</svg>
```

[image: Image showing Arabic text wrapped into two lines.]

Horizontal text wrapping for right to left text. The light-blue lines indicate the limits of the [content area](#TermContentArea). Note that the content area is of infinite height. The red dot shows the initial [current text position](#TermCurrentTextPosition).

An example of using [inline-size](#InlineSizeProperty) for wrapping vertical text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="100" height="300" viewBox="0 0 100 300">

  <text x="62.5" y="25" inline-size="200"
	style="font: 25px IPAMincho; inline-size: 200px; writing-mode: vertical-rl;">
    テキストは１０文字後に折り返されます。</text>

</svg>
```

[image: Image showing vertical Japanese text wrapped into two columns.]

Vertical text wrapping. The light-blue lines indicate the limits of the [content area](#TermContentArea). Note that the content area is of infinite width. The red dot shows the initial [current text position](#TermCurrentTextPosition).

An example of using [inline-size](#InlineSizeProperty) for wrapping horizontal text, anchored in the middle.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <text x="50" y="30" style="font: 20px sans-serif; inline-size: 200px; text-anchor: middle">
    This text wraps at 200 pixels.
  </text>

</svg>
```

[image: Image showing English text wrapped into two lines, anchored in the center.]

Horizontal text wrapping. The light-blue lines indicate the limits of the [content area](#TermContentArea). The text is anchored in the middle. The red dot shows the initial [current text position](#TermCurrentTextPosition).

### <a id="TextShapeInside"></a>11.4.2. The ‘shape-inside’ property

The [shape-inside](#TextShapeInside) property allows one to set the [content area](#TermContentArea) to a [CSS basic shape](../css-shapes-1.md#basic-shape-functions) or to an SVG [shape](shapes.md#TermShapeElement).

In CSS/HTML [shape-inside](#TextShapeInside) applies to block-level elements and absolute and percentage values are defined relative to the block-level element. In SVG absolute and percentage values are defined relative to the current [user coordinate system](../css-transforms-1.md#user-coordinate-system) and the ‘[viewBox](coords.md#ViewBoxAttribute)’.

<a id="issue76"></a>Do nor re-specify shape-inside but reference CSS Shapes.

| Name: | *<a id="ShapeInsideProperty"></a>shape-inside* |
| --- | --- |
| Value: | auto \| [ <basic-shape> \| <uri> ]+ |
| Initial: | auto |
| Applies to: | ‘[text](#TextElement)’ elements |
| Inherited: | no |
| Percentages: | Relative to the ‘[viewBox](coords.md#ViewBoxAttribute)’ |
| Media: | visual |
| Computed value: | computed lengths for <shape>, the absolute URI for <uri>, otherwise as specified |
| Animation type: | See [Interpolation of Basic Shapes](../css-shapes-1.md#basic-shape-interpolation) |

**auto**
: For the purposes of SVG, the 'auto' value indicates that the content area should be defined using the [inline-size](#InlineSizeProperty) property or as for pre-formatted text.
**<basic-shape>**
: The shape is computed based on the values of one of 'circle()', 'ellipse()' or 'polygon()'. The CSS value of 'inset()' is invalid for SVG.

An example of using a CSS basic-shape for wrapping horizontal text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="300" viewBox="0 0 300 300">

  <text style="font: 20px/25px sans-serif;
               text-align: center;
               shape-inside: circle(120px at 150px 150px);">
    Lorem ipsum dolor sit amet, consec-tetuer adipiscing elit...</text>

</svg>
```

[image: Image showing text wrapped inside a circle.]

Horizontal text wrapping inside a CSS circle shape. The light-blue circle indicates the limit of the [content area](#TermContentArea).

**<uri>**
: If the <uri> references an SVG [shape](shapes.md#TermShapeElement) element, that element defines the shape. Otherwise, if the <uri> references an image, the shape is extracted and computed based on the alpha channel of the specified image using the [shape-image-threshold](#TextShapeImageThreshold). If the <uri> does not reference an SVG shape element or an image, the effect is as if the value ‘auto’ had been specified.

An example of using a reference to an SVG [shape](shapes.md#TermShapeElement) for wrapping horizontal text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <defs>
    <rect id="wrap" x="50" y="10" width="200" height="80"/>
  </defs>

  <text style="font: 20px sans-serif; shape-inside: url(#wrap);">
    This text wraps in a rectangle.</text>

</svg>
```

[image: Image showing text wrapped inside a rectangle.]

Horizontal text wrapping inside an SVG rectangle shape. The light-blue lines indicate the limits of the [content area](#TermContentArea).

The CSS values of 'outside-shape', 'shape-box', and 'display' are invalid for SVG.

SVG allows the [shape-inside](#TextShapeInside) property to have a list of shapes. Each shape defines an independent [content area](#TermContentArea). Text is first laid out in the content area of the first shape. If the text overflows the first shape, the overflow text is laid out in the next shape until all text is laid out or no more shapes are available.

The effect is similar to CSS columns, except that the columns can have arbitrary shapes.

It is recommended that an overflow shape be provided to ensure the accessibility of all text in cases; for example, if a user increases the font size.

Except as noted, see the CSS Shapes Module Level 2 for the definition of 'shape-inside'. [[css-shapes-2](refs.md#ref-css-shapes-2)]

### <a id="TextShapeSubtract"></a>11.4.3. The ‘shape-subtract’ property

The [shape-subtract](#TextShapeSubtract) property allows one to exclude part of the [content area](#TermContentArea) from the [wrapping area](#TermWrappingArea). The excluded area is the addition of all the areas defined in a list of [CSS basic shape](../css-shapes-1.md#basic-shape-functions)s and/or SVG [shapes](shapes.md#TermShapeElement).

Absolute and percentage values are defined relative to the current [user coordinate system](../css-transforms-1.md#user-coordinate-system) and the ‘[viewBox](coords.md#ViewBoxAttribute)’.

| Name: | *<a id="ShapesubtractProperty"></a>shape-subtract* |
| --- | --- |
| Value: | none \| [ <basic-shape> \| <uri> ]+ |
| Initial: | none |
| Applies to: | ‘[text](#TextElement)’ elements |
| Inherited: | no |
| Percentages: | Relative to the ‘[viewBox](coords.md#ViewBoxAttribute)’ |
| Media: | visual |
| Computed value: | computed lengths for any <basic-shape>, the absolute URI for <uri>, otherwise as specified |
| Animation type: | See [Interpolation of Basic Shapes](../css-shapes-1.md#basic-shape-interpolation) |

**<basic-shape>**
: The shape is computed based on the values of one of 'circle()', 'ellipse()' or 'polygon()'.
**<uri>**
: For any <uri> that references an SVG [shape](shapes.md#TermShapeElement) element, that element defines the contributing shape, expanded by the value of its [shape-margin](#TextShapeMargin) distance. For any <uri> that references an image, the contributing shape is extracted and computed based on the alpha channel of the specified image using the [shape-image-threshold](#TextShapeImageThreshold). If an <uri> does not reference an SVG shape element or an image, that <uri> is ignored.
An example of using [shape-subtract](#TextShapeSubtract).

.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="450" height="300" viewBox="0 0 450 300">

  <rect id="rect1" x="25"  y="25"  width="225" height="175" fill="white" stroke="black"/>
  <rect id="rect2" x="200" y="125" width="225" height="150" fill="white" stroke="black"
        style="shape-margin:25px;"/>

  <text style="shape-inside:url(#rect1);
	       shape-subtract:url(#rect2);
	       shape-padding:25px;
           font-family:DejaVu Sans;
	       font-size:12px;
	       text-align:justified;
	       line-height:110%">Lorem ipsum ...</text>
  <text style="shape-inside:url(#rect2);
	       shape-padding:25px;
               font-family:DejaVu Sans;
	       font-size:12px;
	       text-align:justified;
	       line-height:110%">Lorem ipsum ...</text>
</svg>
```

[image: Image showing horizontal text wrapped inside two overlapping rectangles.]

Horizontal text wrapping inside two overlapping rectangles using [shape-subtract](#TextShapeSubtract) as well as [shape-inside](#TextShapeInside), [shape-padding](#TextShapePadding) and [shape-margin](#TextShapeMargin). The black rectangles show the [content area](#TermContentArea)s. The inner blue lines show the [wrapping area](#TermWrappingArea)s.

### <a id="TextShapeImageThreshold"></a>11.4.4. The ‘shape-image-threshold’ property

The [shape-image-threshold](#TextShapeImageThreshold) defines the alpha channel threshold used to extract the shape using an image. A value of 0.5 means that the shape will enclose all the pixels that are more than 50% opaque.

For the purposes of SVG, this property applies to ‘[text](#TextElement)’ elements.

Except as noted, see the CSS Shapes Module Level 1 for the definition of ['shape-image-threshold'](../css-shapes-1.md#shape-image-threshold-property). [[css-shapes-1](refs.md#ref-css-shapes-1)]

### <a id="TextShapeMargin"></a>11.4.5. The ‘shape-margin’ property

The [shape-margin](#TextShapeMargin) property adds a margin to a shape referenced with [shape-subtract](#TextShapeSubtract). It defines a new shape where every point is the specified distance from the original shape. This property takes on positive values only.

<a id="issue77"></a>Do nor re-specify shape-margin but reference CSS Shapes.

| Name: | *<a id="ShapeMarginProperty"></a>shape-margin* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | 0 |
| Applies to: | ‘[text](#TextElement)’ elements |
| Inherited: | no |
| Percentages: | N/A |
| Media: | visual |
| Computed value: | an absolute length |
| Animation type: | by computed value |

Except as noted, see the CSS Shapes Module Level 1 for the definition of See ['shape-margin'](../css-shapes-1.md#shape-margin-property). [[css-shapes-1](refs.md#ref-css-shapes-1)]

### <a id="TextShapePadding"></a>11.4.6. The ‘shape-padding’ property

The [shape-padding](#TextShapePadding) property can be used to offset the inline flow content wrapping on the inside of elements. Offsets created by the ‘wrap-padding’ property are offset from the content area of the element. This property takes on positive values only.

An example of using [shape-padding](#TextShapePadding)

.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="300" viewBox="0 0 300 300">

  <circle id="circle" cx="150" cy="150" r="125" fill="none" stroke="black"/>
  <text style="shape-inside: url(#circle);
	       shape-padding: 25px;
	       font: 18px DejaVu Sans;
	       text-align: justified;
	       line-height: 110%;">This is an
  example of wrapped text in SVG 2! There should
  be 25 pixel padding around the text. The text is
  justified on both sides. It looks good!</text>

</svg>
```

[image: Image showing horizontal text wrapped inside a circle with a padding.]

Horizontal text wrapping inside a circle with a [shape-padding](#TextShapePadding). The outer black circle shows the [content area](#TermContentArea). The inner blue circle shows the [wrapping area](#TermWrappingArea).

Except as noted, see the CSS Shapes Module Level 2 for the definition of 'shape-padding'.

## <a id="TextLayoutAlgorithm"></a>11.5. Text layout – Algorithm

Text layout begins by passing to a CSS-based text renderer the content of the ‘[text](#TextElement)’ element which includes text data along with styling information and a description of one or more shapes to be filled. The ‘[text](#TextElement)’ element is treated as a block element and its descendant ‘[tspan](#TextElement)’, ‘[textPath](#TextPathElement)’ and ‘[a](linking.md#AElement)’ elements are treated as inline elements. The CSS renderer returns a set of [typographic characters](#TermTypographicCharacterUnit) with their positions resulting from laying out the text as if the text were absolutely positioned.

A [typographic character](#TermTypographicCharacterUnit) may contain more than one [glyph](#TermGlyph). It is assumed here the relative positioning of the [glyphs](#TermGlyph) inside a [typographic character](#TermTypographicCharacterUnit) is encapsulated by the [typographic character](#TermTypographicCharacterUnit) and it is not user controllable.

Once a [content area](#TermContentArea) has been defined, the following algorithm is used to determine the [typographic characters](#TermTypographicCharacterUnit) and their positions for a given ‘[text](#TextElement)’ element:

- For text where the [content area](#TermContentArea) is defined by the [shape-inside](#TextShapeInside) property, layout the text in one or more areas per CSS rules for laying out text in a CSS [wrapping area](#TermWrappingArea).
SVG allows the [shape-inside](#TextShapeInside) property to reference more than one shape. Each shape should be filled in turn until there is no more text or no more shapes.
- For text where the [content area](#TermContentArea) is defined by the [inline-size](#InlineSizeProperty) property, layout the text in a CSS content box where the inline size is set to the value of the [inline-size](#InlineSizeProperty) property and the block size is unconstrained.
This means that text is laid out with the box width set to the [inline-size](#InlineSizeProperty) value if [writing-mode](#WritingModeProperty) is horizontal-tb, or with the box height set to the [inline-size](#InlineSizeProperty) value otherwise.
- For all other text (SVG 1.1 compatible text and SVG 1.1 like multi-line text), layout the text in a CSS content box that is unconstrained in width and height.

A number of CSS properties have no or limited effect on SVG text layout:

- The used value of each of the following properties on a [text content element](#TermTextContentElement) (and its pseudo-elements, if any) and on an ‘[a](linking.md#AElement)’ element within a [text content block element](#TermTextContentBlockElement) must be the property's initial values: ‘border-style’, ‘float’, ‘margin’, ‘padding’, ‘position’ and ‘text-align’.
- If the computed value of the [display](render.md#VisibilityControl) property on a correctly parented [text content block element](#TermTextContentBlockElement) is anything other than none, then its used value must be block.
This ensures that the ‘[text](#TextElement)’ element is treated as if it were a block element.
- If the computed value of the [display](render.md#VisibilityControl) property on a correctly parented [text content child element](#TermTextContentChildElement) or ‘[a](linking.md#AElement)’ element, is anything other than none, then its used value must be inline.
This ensures that ‘[tspan](#TextElement)’, ‘[textPath](#TextPathElement)’ and ‘[a](linking.md#AElement)’ elements are treated as if they were inline elements. Note: the [transform](coords.md#TransformProperty) property has no effect on inline elements.
- The used value of the [display](render.md#VisibilityControl) property on an element that is a descendant of a [text content block element](#TermTextContentBlockElement) and is not a correctly parented [text content child element](#TermTextContentChildElement) or ‘[a](linking.md#AElement)’ element must be none.
This ensures that graphics and metadata elements inside a ‘[text](#TextElement)’ element do not render.

Various SVG attributes and properties may reposition the [typographic characters](#TermTypographicCharacterUnit) depending on how the [content area](#TermContentArea) is defined:

- For text where the [content area](#TermContentArea) is defined by the [shape-inside](#TextShapeInside) property, the [typographic characters](#TermTypographicCharacterUnit) are rendered with positions as returned from the CSS-based text renderer.
- For text where the [content area](#TermContentArea) is defined by the [inline-size](#InlineSizeProperty) property, each [line box](#TermLineBox) is shifted according to the [text-anchor](#TextAnchorProperty) property. No further post-processing is performed.
- For all other text (SVG 1.1 compatible text and SVG 1.1 like multi-line text), the positions of the [typographic characters](#TermTypographicCharacterUnit) are adjusted according to the algorithm that follows.

The following SVG text layout algorithm returns output information about each character in the DOM in the ‘[text](#TextElement)’ element's subtree. That information includes:

- a global index number, which indicates the index of the character relative to the start of the ‘[text](#TextElement)’ element;
- an (x, y) coordinate pair, which gives its position in the coordinate space of the ‘[text](#TextElement)’ element;
- an angle value, which gives its rotation;
- a "hidden" flag, which represents whether it is hidden due to falling off the end of a ‘[textPath](#TextPathElement)’;
- an "addressable" flag, which represents whether it is an [addressable character](#TermAddressableCharacter), that is, addressable by text positioning attributes and SVG DOM text methods;
- a "middle" flag, which represents whether it is the second or later character of a [typographic character](#TermTypographicCharacterUnit)
- an "anchored chunk" flag, which represents whether it starts a contiguous sequence of glyphs that can be affected by text anchoring

The arrays given in the SVG attributes ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ are indexed by [addressable characters](#TermAddressableCharacter). However, repositioning is applied to [typographic characters](#TermTypographicCharacterUnit). If a [typographic character](#TermTypographicCharacterUnit) corresponds to more than one character (e.g. a ligature), only the array values corresponding to the first character are used in positioning the [typographic character](#TermTypographicCharacterUnit). Array values corresponding to other characters in the [typographic character](#TermTypographicCharacterUnit) are skipped (for ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’), are accumulated and applied to the next [typographic character](#TermTypographicCharacterUnit) (for ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’), or if it is the last value in the array, applied to the following [typographic characters](#TermTypographicCharacterUnit) (for ‘[rotate](#TextElementRotateAttribute)’). This ensures, for example, that attribute values are applied to the same characters regardless of whether or not a font has a particular ligature.

The SVG specific text layout algorithm is as follows:

1. **Setup**
  1. Let *root* be the result of generating [typographic character](#TermTypographicCharacterUnit) positions for the ‘[text](#TextElement)’ element and its subtree, laid out as if it were an absolutely positioned element.
  This will be a single line of text unless the white-space property causes line breaks.
  2. Let *count* be the number of DOM *characters* within the ‘[text](#TextElement)’ element's subtree.
  3. Let *result* be an array of length *count* whose entries contain the per-character information described above. Each entry is initialized as follows:
    - its global index number equal to its position in the array,
    - its "x" coordinate set to "unspecified",
    - its "y" coordinate set to "unspecified",
    - its "rotate" coordinate set to "unspecified",
    - its "hidden" flag is false,
    - its "addressable" flag is true,
    - its "middle" flag is false,
    - its "anchored chunk" flag is false.
  If *result* is empty, then return *result*.
  4. Let *CSS_positions* be an array of length *count* whose entries will be filled with the *x* and *y* positions of the corresponding [typographic character](#TermTypographicCharacterUnit) in *root*. The array entries are initialized to (0, 0).
  5. Let "horizontal" be a flag, true if the writing mode of ‘[text](#TextElement)’ is horizontal, false otherwise.
2. **Set flags and assign initial positions**
For each array element with index *i* in *result*:
  1. Set *addressable* to false if the character at index *i* was:
    - part of the text content of a [non-rendered element](render.md#TermNonRenderedElement)
    - discarded during layout due to being a collapsed white space character, a soft hyphen character, or a bidi control character; *or*
    - discarded during layout due to being a collapsed segment break; *or*
    - trimmed from the start or end of a line.
  Since there is collapsible white space not addressable by glyph positioning attributes in the following ‘[text](#TextElement)’ element (with a standard font), the "B" glyph will be placed at x=300.
```
<text x="100 200 300">
	      A
	      B
	      </text>
```

  This is because the white space before the "A", and all but one white space character between the "A" and "B", is collapsed away or trimmed.
  2. Set *middle* to true if the character at index *i* is the second or later character that corresponds to a [typographic character](#TermTypographicCharacterUnit).
  3. If the character at index *i* corresponds to a [typographic character](#TermTypographicCharacterUnit) at the beginning of a line, then set the "anchored chunk" flag of *result*[*i*] to true.
  This ensures chunks shifted by [text-anchor](#TextAnchorProperty) do not span multiple lines.
  4. If *addressable* is true and *middle* is false then set *CSS_positions*[*i*] to the position of the corresponding [typographic character](#TermTypographicCharacterUnit) as determined by the CSS renderer. Otherwise, if *i* > 0, then set *CSS_positions*[*i*] = *CSS_positions*[*i* − 1]
3. **Resolve character positioning**
Position adjustments (e.g. values in a ‘[x](#TextElementXAttribute)’ attribute) specified by a node apply to all characters in that node including characters in the node's descendants. Adjustments specified in descendant nodes, however, override adjustments from ancestor nodes. This section resolves which adjustments are to be applied to which characters. It also directly sets the *rotate* coordinate of *result*.
  1. Set up:
    1. Let *resolve_x*, *resolve_y*, *resolve_dx*, and *resolve_dy* be arrays of length *count* whose entries are all initialized to "unspecified".
    2. Set "in_text_path" flag false.
    This flag will allow ‘[y](#TextElementYAttribute)’ (‘[x](#TextElementXAttribute)’) attribute values to be ignored for horizontal (vertical) text inside ‘[textPath](#TextPathElement)’ elements.
    3. Call the following procedure with the ‘[text](#TextElement)’ element node.
  2. Procedure: **<a id="ResolveGlyphPositioning"></a>*resolve character positioning***:
  A recursive procedure that takes as input a *node* and whose steps are as follows:
    1. If *node* is a ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ node:
      1. Let *index* equal the "global index number" of the first character in the node.
      2. Let *x*, *y*, *dx*, *dy* and *rotate* be the lists of values from the corresponding attributes on *node*, or empty lists if the corresponding attribute was not specified or was invalid.
      3. If "in_text_path" flag is false:
        - Let *new_chunk_count* = max(length of *x*, length of *y*).
      Else:
        - If the "horizontal" flag is true:
          - Let *new_chunk_count* = length of *x*.
        - Else:
          - Let *new_chunk_count* = length of *y*.
      4. Let *length* be the number of DOM characters in the subtree rooted at *node*.
      5. Let *i* = 0 and *j* = 0.
      *i* is an index of [addressable characters](#TermAddressableCharacter) in the node; *j* is an index of all [characters](#TermCharacter) in the node.
      6. While *j* < *length*, do:
      This loop applies the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’ and ‘[rotate](#TextElementRotateAttribute)’ attributes to the content inside *node*.
        1. If the "addressable" flag of *result*[*index* + *j*] is true, then:
          1. If *i* < *new_check_count*, then set the "anchored chunk" flag of *result*[*index* + *j*] to true. Else set the flag to false.
          Setting the flag to false ensures that ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes set in a ‘[text](#TextElement)’ element don't create anchored chunk in a ‘[textPath](#TextPathElement)’ element when they should not.
          2. If *i* < length of *x*, then set *resolve_x*[*index* + *j*] to *x*[*i*].
          3. If "in_text_path" flag is true and the "horizontal" flag is false, unset *resolve_x*[*index*].
          The ‘[x](#TextElementXAttribute)’ attribute is ignored for vertical text on a path.
          4. If *i* < length of *y*, then set *resolve_y*[*index* + *j*] to *y*[*i*].
          5. If "in_text_path" flag is true and the "horizontal" flag is true, unset *resolve_y*[*index*].
          The ‘[y](#TextElementYAttribute)’ attribute is ignored for horizontal text on a path.
          6. If *i* < length of *dx*, then set *resolve_dx*[*index* + *j*] to *dy*[*i*].
          7. If *i* < length of *dy*, then set *resolve_dy*[*index* + *j*] to *dy*[*i*].
          8. If *i* < length of *rotate*, then set the angle value of *result*[*index* + *j*] to *rotate*[*i*]. Otherwise, if *rotate* is not empty, then set *result*[*index* + *j*] to *result*[*index* + *j* − 1].
          9. Set *i* = *i* + 1.
        2. Set *j* = *j* + 1.
    2. If *node* is a ‘[textPath](#TextPathElement)’ node:
      1. Let *index* equal the global index number of the first character in the node (including descendant nodes).
      2. Set the "anchored chunk" flag of *result*[*index*] to true.
      A ‘[textPath](#TextPathElement)’ element always creates an anchored chunk.
      3. Set *in_text_path* flag true.
    3. For each child node *child* of *node*:
      1. [Resolve glyph positioning](#ResolveGlyphPositioning) of *child*.
    4. If *node* is a ‘[textPath](#TextPathElement)’ node:
      1. Set "in_text_path" flag false.
4. **Adjust positions: dx, dy**
The ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ adjustments are applied before adjustments due to the ‘[textLength](#TextElementTextLengthAttribute)’ attribute while the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’ and ‘[rotate](#TextElementRotateAttribute)’ adjustments are applied after.
  1. Let *shift* be the cumulative *x* and *y* shifts due to ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ attributes, initialized to (0,0).
  2. For each array element with index *i* in result:
    1. If *resolve_dx*[*i*] is unspecified, set it to 0. If *resolve_dy*[*i*] is unspecified, set it to 0.
    2. Let *shift.x* = *shift.x* + *resolve_dx*[*i*] and *shift.y* = *shift.y* + *resolve_dy*[*i*].
    3. Let *result*[*i*].x = CSS_positions[*i*].x + shift.x and *result*[*i*].y = CSS_positions[*i*].y + shift.y.
5. **Apply ‘[textLength](#TextElementTextLengthAttribute)’ attribute**
  1. Set up:
    1. Define *<a id="TermResolvedDescendantNode"></a>resolved descendant node* as a descendant of node with a valid ‘[textLength](#TextElementTextLengthAttribute)’ attribute that is not itself a descendant node of a descendant node that has a valid ‘[textLength](#TextElementTextLengthAttribute)’ attribute.
    2. Call the following procedure with the ‘[text](#TextElement)’ element node.
  2. Procedure: **<a id="ResolveTextLength"></a>*resolve text length***:
  A recursive procedure that takes as input a *node* and whose steps are as follows:
    1. For each child node *child* of node:
      1. [Resolve text length](#ResolveTextLength) of child.
      Child nodes are adjusted before parent nodes.
    2. If *node* is a ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ node and if the node has a valid ‘[textLength](#TextElementTextLengthAttribute)’ attribute value:
      1. Let *a* = +∞ and *b* = −∞.
      2. Let *i* and *j* be the global index of the first character and last characters in *node*, respectively.
      3. For each index *k* in the range [*i*, *j*] where the "addressable" flag of *result*[*k*] is true:
      This loop finds the left-(top-) most and right-(bottom-) most extents of the [typographic characters](#TermTypographicCharacterUnit) within the node and checks for forced line breaks.
        1. If the character at *k* is a linefeed or carriage return, return. No adjustments due to ‘[textLength](#TextElementTextLengthAttribute)’ are made to a node with a forced line break.
        2. Let *pos* = the x coordinate of the position in *result*[*k*], if the "horizontal" flag is true, and the y coordinate otherwise.
        3. Let *advance* = the advance of the [typographic character](#TermTypographicCharacterUnit) corresponding to character *k*. [NOTE: This advance will be negative for RTL horizontal text.]
        4. Set *a* = min(*a*, *pos*, *pos* + *advance*).
        5. Set *b* = max(*b*, *pos*, *pos* + *advance*).
      4. If *a* ≠ +∞ then:
        1. Find the distance *delta* = ‘[textLength](#TextElementTextLengthAttribute)’ computed value − (b − a).
        User agents are required to shift the last [typographic character](#TermTypographicCharacterUnit) in the node by *delta*, in the positive *x* direction if the "horizontal" flag is true and if [direction](#DirectionProperty) is lrt, in the negative *x* direction if the "horizontal" flag is true and [direction](#DirectionProperty) is rtl, or in the positive *y* direction otherwise. User agents are free to adjust intermediate [typographic characters](#TermTypographicCharacterUnit) for optimal typography. The next steps indicate one way to adjust [typographic characters](#TermTypographicCharacterUnit) when the value of ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’ is spacing.
        2. Find *n*, the total number of [typographic characters](#TermTypographicCharacterUnit) in this node including any descendant nodes that are not resolved descendant nodes or within a resolved descendant node.
        3. Let *n* = *n* + number of resolved descendant nodes − 1.
        Each resolved descendant node is treated as if it were a single [typographic character](#TermTypographicCharacterUnit) in this context.
        4. Find the per-character adjustment *δ* = *delta*/*n*.
        5. Let *shift* = 0.
        6. For each index *k* in the range [*i*,*j*]:
          1. Add shift to the *x* coordinate of the position in result[*k*], if the "horizontal" flag is true, and to the *y* coordinate otherwise.
          2. If the "middle" flag for result[*k*] is not true and *k* is not a character in a resolved descendant node other than the first character then *shift* = *shift* + *δ*.
6. **Adjust positions: x, y**
This loop applies ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ values, and ensures that [text-anchor](#TextAnchorProperty) chunks do not start in the middle of a [typographic character](#TermTypographicCharacterUnit).
  1. Let *shift* be the current adjustment due to the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes, initialized to (0,0).
  2. Set *index* = 1.
  3. While *index* < *count*:
    1. If *resolved_x*[*index*] is set, then let *shift.x* = *resolved_x*[*index*] − *result.x*[*index*].
    2. If *resolved_y*[*index*] is set, then let *shift.y* = *resolved_y*[*index*] − *result.y*[*index*].
    3. Let *result.x*[*index*] = *result.x*[*index*] + shift.x and *result.y*[*index*] = *result.y*[*index*] + shift.y.
    4. If the "middle" and "anchored chunk" flags of *result*[*index*] are both true, then:
      1. Set the "anchored chunk" flag of *result*[*index*] to false.
      2. If *index* + 1 < *count*, then set the "anchored chunk" flag of *result*[*index* + 1] to true.
    5. Set *index* to *index* + 1.
7. **Apply anchoring**
  1. For each slice *result*[*i*..*j*] (inclusive of both *i* and *j*), where:
    - the "anchored chunk" flag of *result*[*i*] is true,
    - the "anchored chunk" flags of *result*[*k*] where *i* < *k* ≤ *j* are false, and
    - *j* = *count* − 1 or the "anchored chunk" flag of *result*[*j* + 1] is true;
  do:
  This loops over each anchored chunk.
    1. Let *a* = +∞ and *b* = −∞.
    2. For each index *k* in the range [*i*, *j*] where the "addressable" flag of *result*[*k*] is true:
    This loop finds the left-(top-) most and right-(bottom-) most extents of the [typographic character](#TermTypographicCharacterUnit) within the anchored chunk.
      1. Let *pos* = the x coordinate of the position in *result*[*k*], if the "horizontal" flag is true, and the y coordinate otherwise.
      2. Let *advance* = the advance of the [typographic character](#TermTypographicCharacterUnit) corresponding to character *k*. [NOTE: This advance will be negative for RTL horizontal text.]
      3. Set *a* = min(*a*, *pos*, *pos* + *advance*).
      4. Set *b* = max(*b*, *pos*, *pos* + *advance*).
    3. If *a* ≠ +∞, then:
    Here we perform the text anchoring.
      1. Let *shift* be the x coordinate of *result*[*i*], if the "horizontal" flag is true, and the y coordinate otherwise.
      2. Adjust shift based on the value of [text-anchor](#TextAnchorProperty) and [direction](#DirectionProperty) of the element the character at index *i* is in:
      **(start, ltr) or (end, rtl)**
      : Set *shift* = *shift* − *a*.
      **(start, rtl) or (end, ltr)**
      : Set *shift* = *shift* − *b*.
      **(middle, ltr) or (middle, rtl)**
      : Set *shift* = *shift* − (*a* + *b*) / 2.
      3. For each index *k* in the range [*i*, *j*]:
        1. Add *shift* to the x coordinate of the position in *result*[*k*], if the "horizontal" flag is true, and to the y coordinate otherwise.
8. **Position on path**
  1. Set *index* = 0.
  2. Set the "in path" flag to false.
  3. Set the "after path" flag to false.
  4. Let *path_end* be an offset for characters that follow a ‘[textPath](#TextPathElement)’ element. Set *path_end* to (0,0).
  5. While *index* < *count*:
    1. If the character at index *i* is within a ‘[textPath](#TextPathElement)’ element and corresponds to a [typographic character](#TermTypographicCharacterUnit), then:
      1. Set "in path" flag to true.
      2. If the "middle" flag of *result*[*index*] is false, then:
      Here we apply ‘[textPath](#TextPathElement)’ positioning.
        1. Let *path* be the [equivalent path](paths.md#TermEquivalentPath) of the [basic shape](shapes.md#TermBasicShapeElement) element referenced by the ‘[textPath](#TextPathElement)’ element, or an empty path if the reference is invalid.
        2. If the ‘[side](#TextPathElementSideAttribute)’ attribute of the ‘[textPath](#TextPathElement)’ element is 'right', then reverse *path*.
        3. Let *length* be the length of *path*.
        4. Let *offset* be the value of the ‘[textPath](#TextPathElement)’ element's ‘[startOffset](#TextPathElementStartOffsetAttribute)’ attribute, adjusted due to any ‘[pathLength](paths.md#PathLengthAttribute)’ attribute on the referenced element.
        5. Let *advance* = the advance of the [typographic character](#TermTypographicCharacterUnit) corresponding to character *k*. [NOTE: This advance will be negative for RTL horizontal text.]
        6. Let (*x*, *y*) and *angle* be the position and angle in *result*[*index*].
        7. Let *mid* be a coordinate value depending on the value of the "horizontal" flag:
        **true**
        : *mid* is *x* + *advance* / 2 + *offset*
        **false**
        : *mid* is *y* + *advance* / 2 + *offset*
        The user agent is free to make any additional adjustments to *mid* necessary to ensure high quality typesetting due to a ‘[spacing](#TextPathElementSpacingAttribute)’ value of 'auto' or a ‘[method](#TextPathElementMethodAttribute)’ value of 'stretch'.
        8. If *path* is not a [closed subpath](paths.md#TermOpenSubpath) and *mid* < 0 or *mid* > *length*, set the "hidden" flag of *result*[*index*] to true.
        9. If *path* is a [closed subpath](paths.md#TermOpenSubpath) depending on the values of [text-anchor](#TextAnchorProperty) and [direction](#DirectionProperty) of the element the character at *index* is in:
        This implements the special wrapping criteria for single [closed subpaths](paths.md#TermOpenSubpath).
        **(start, ltr) or (end, rtl)**
        : If *mid*−*offset* < 0 or *mid*−*offset* > *length*, set the "hidden" flag of *result*[*index*] to true.
        **(middle, ltr) or (middle, rtl)**
        : If *mid*−*offset* < −*length*/2 or *mid*−*offset* > *length*/2, set the "hidden" flag of *result*[*index*] to true.
        **(start, rtl) or (end, ltr)**
        : If *mid*−*offset* < −*length* or *mid*−*offset* > 0, set the "hidden" flag of *result*[*index*] to true.
        Set *mid* = *mid* mod *length*.
        10. If the *hidden* flag is false:
          1. Let *point* be the position and *t* be the unit vector tangent to the point *mid* distance along *path*.
          2. If the "horizontal" flag is
          **true**
          :
            1. Let *n* be the normal unit vector pointing in the direction *t* + 90°.
            2. Let *o* be the horizontal distance from the vertical center line of the glyph to the alignment point.
            3. Then set the position in *result*[*index*] to *point* - *o*×*t* + *y*×*n*.
            4. Let *r* be the angle from the positive x-axis to the tangent.
            5. Set the angle value in *result*[*index*] to *angle* + *r*.
          **false**
          :
            1. Let *n* be the normal unit vector pointing in the direction *t* - 90°.
            2. Let *o* be the vertical distance from the horizontal center line of the glyph to the alignment point.
            3. Then set the position in *result*[*index*] to *point* - *o*×*t* + *x*×*n*.
            4. Let *r* be the angle from the positive y-axis to the tangent.
            5. Set the angle value in *result*[*index*] to *angle* + *r*.
      3. Otherwise, the "middle" flag of *result*[*index*] is true:
        1. Set the position and angle values of *result*[*index*] to those in *result*[*index* − 1].
    2. If the character at index *i* is not within a ‘[textPath](#TextPathElement)’ element and corresponds to a [typographic character](#TermTypographicCharacterUnit), then:
    This sets the starting point for rendering any characters that occur after a ‘[textPath](#TextPathElement)’ element to the end of the path.
      1. If the "in path" flag is true:
        1. Set the "in path" flag to false.
        2. Set the "after path" flag to true.
        3. Set *path_end* equal to the end point of the path referenced by ‘[textPath](#TextPathElement)’ − the position of *result*[*index*].
      2. If the "after path" is true.
        1. If *anchored chunk* of *result*[*index*] is true, set the "after path" flag to false.
        2. Else, let *result.x*[*index*] = *result.x*[*index*] + *path_end.x* and *result.y*[*index*] = *result.y*[*index*] + *path_end.y*.
    3. Set *index* = *index* + 1.
9. **Return *result***

## <a id="TextLayoutPre"></a>11.6. Pre-formatted text

This option corresponds to basic SVG 1.1 text layout.

This is the default text layout method and is used in the absence of an explicitly defined [content area](#TermContentArea). It is also used as a first step in laying out *text on a path* (with slightly modified rules). In this layout method, no automatic line breaking or word wrapping is done. Nominally, the text is rendered as a single line inside a rectangular [content area](#TermContentArea) of infinite width and height. Multiple lines of text can be obtained by precomputing line breaks and using one of the following methods:

- Use a single ‘[text](#TextElement)’ element with white-space set to pre or pre-line. Line spacing is set by [line-height](#LineHeightProperty).
New in SVG 2.
- Use a single ‘[text](#TextElement)’ element with one or more ‘[tspan](#TextElement)’ child elements with appropriate values for attributes ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ to set new start positions for those characters which start new lines.
- Use multiple ‘[text](#TextElement)’ elements, one for each line of text. (Not recommended as this may prevent selection across multiple lines of text -- see [Text selection and clipboard operations](#TextSelection).)

The following properties do not apply to *pre-formatted* text: text-align, text-align-last, line-break, word-break, hyphens, word-wrap, and overflow-wrap.

### <a id="TextLayoutPreMultiline"></a>11.6.1. Multi-line text via 'white-space'

Multi-line pre-formatted text may be created by using the white-space values pre or pre-line. In these cases, a line-feed or carriage return is preserved as a forced line break which creates a new [line box](#TermLineBox). The [line boxes](#TermLineBox) are stacked following the rules of CSS.

### <a id="TextLayoutPreAdjustments"></a>11.6.2. Repositioning Glyphs

After text is laid out according to the basic CSS text layout rules, [typographic characters](#TermTypographicCharacterUnit) can be repositioned using SVG specific rules. Absolute repositioning can be prescribed by giving absolute coordinates in the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes or by forced line breaks. Absolute repositioning may be influenced by the [text-anchor](#TextAnchorProperty) property. Relative repositioning can be prescribed by giving relative coordinates in the ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’ attributes. The [typographic characters](#TermTypographicCharacterUnit) may be arbitrarily rotated by giving a list of values in the ‘[rotate](#TextElementRotateAttribute)’ attribute. Absolute repositioning (including any adjustment due to the [text-anchor](#TextAnchorProperty) property) is done before relative repositioning and rotation.

## <a id="TextLayoutAuto"></a>11.7. Auto-wrapped text

Text is automatically wrapped when a [content area](#TermContentArea) is specified in the ‘[text](#TextElement)’ element. The [content area](#TermContentArea) defines the outermost container for wrapping text. A [wrapping context](#TermWrappingContext) (set of exclusion areas) may also be given. The actual [wrapping area](#TermWrappingArea) is defined by subtracting the [wrapping context](#TermWrappingContext) from the [content area](#TermContentArea). The [wrapping context](#TermWrappingContext) may also be reduced by the value of the [shape-padding](#TextShapePadding) property. The effective area of an exclusion may be enlarged by the value of the [shape-margin](#TextShapeMargin) property.

In the case where the [content area](#TermContentArea) is defined by the [inline-size](#InlineSizeProperty) property, the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes corresponding to the first rendered [typographic character](#TermTypographicCharacterUnit) define the initial [current text position](#TermCurrentTextPosition). When the [content area](#TermContentArea) is inside a [shape](shapes.md#TermShapeElement), the initial [current text position](#TermCurrentTextPosition) is determined from the position of the first rendered [typographic character](#TermTypographicCharacterUnit) after laying out the first line box inside the shape.

Except when used to determine the initial [current text position](#TermCurrentTextPosition), all values ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ are ignored on ‘[text](#TextElement)’, and ‘[tspan](#TextElement)’ elements in auto-wrapped text.

The attributes ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ can provide a natural fallback mechanism for SVG1.1 renderers for wrapped text. Content producers may wish to pre-layout text by breaking up lines into ‘[tspan](#TextElement)’ elements with ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes. Then, for example, if a fallback font is used to render the text, an SVG2 renderer will ignore the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes and reflow the text using the font metrics of the fallback font. An SVG1.1 renderer will use the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes in rendering the text which will usually result in readable text even if the rendering doesn't match the shape. Many of the text wrapping examples in this section rely on this mechanism to render text in browsers that have not implemented text wrapping.

### <a id="TextLayoutAutoNotes"></a>11.7.1. Notes on Text Wrapping

The following examples illustrate a few issues with laying out text in a shape.

#### <a id="TextLayoutAutoNotesStart"></a>11.7.1.1. First Line Positioning

Given an arbitrary shaped [wrapping area](#TermWrappingArea), the first [line box](#TermLineBox) may not fit flush against the top (or side for vertical text). In this case, the first [line box](#TermLineBox) is shifted until it fits.

A future CSS specification may define a line grid that could be used to control the position of the first line of text to allow alignment of text between different [wrapping areas](#TermWrappingArea).

[image: Image showing two lines of text, the first line is 'The' and the next line is 'first line'.]

The top [line box](#TermLineBox) (small light-blue rectangle), consisting of the smallest possible block of text, is moved down until the [line box](#TermLineBox) fits inside the [wrapping area](#TermWrappingArea) (light-blue path). Note, the [line box](#TermLineBox) includes the effect of the [line-height](#LineHeightProperty) property, here set to 1.25. The red rectangles tightly wrap the glyphs in each [line box](#TermLineBox).

#### <a id="TextLayoutAutoNotesBrokenLines"></a>11.7.1.2. Broken Lines

Given an arbitrary shaped [wrapping area](#TermWrappingArea), a single line of text might be broken into more than one part. In this case, a [line box](#TermLineBox) for each part is created. The height of all [line boxes](#TermLineBox) in a single line of text must be the same (ensuring all parts have the same baseline). This height is calculated by looking at all glyphs in the line of text.

A future CSS specification may allow one to control which parts of a shape broken into different parts is filled (e.g, fill only the right most parts, fill only the left most parts, etc.).

[image: Image showing text laidout inside a 'V' shape.]

The top line is split into two [line boxes](#TermLineBox) (light-blue rectangles), text in each [line box](#TermLineBox) is centered inside the box (due to 'text-align:center').

## <a id="TextLayoutPath"></a>11.8. Text on a path

SVG can place text along a path defined either by a ‘[path](paths.md#PathElement)’ element or the path equivalent of a [basic shape](shapes.md#TermBasicShapeElement). This is specified by including text within a ‘[textPath](#TextPathElement)’ element that has either an ‘[href](#TextPathElementHrefAttribute)’ attribute with an [URL reference](linking.md#TermURLReference) pointing to a ‘[path](paths.md#PathElement)’ element or [basic shape](shapes.md#TermBasicShapeElement), or by specifying a value for the ‘[path](#TextPathElementPathAttribute)’ attribute that specifies the path data directly.

The ability to place text along a basic shape is new in SVG 2.

Text on a path is conceptionally like a single line of [pre-formatted text](#TextLayoutPre) that is then transformed to follow the path. Except as indicated, all the properties that apply to pre-formatted text apply to text on a path.

### <a id="TextPathElement"></a>11.8.1. The ‘textPath’ element

‘*<a id="elementdef-textPath"></a>textPath*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [text content element](#TermTextContentElement), [text content child element](#TermTextContentChildElement)
**Content model:**
: Any number of the following elements or character data, in any order:

- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[a](linking.md#AElement), animate, [clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), set, [style](styling.md#StyleElement), [tspan](#TextElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- [deprecated xlink attributes](linking.md#XLinkRefAttrs) — ‘[xlink:href](linking.md#XLinkHrefAttribute)’, ‘[xlink:title](linking.md#XLinkTitleAttribute)’
- ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’
- ‘[textLength](#TextElementTextLengthAttribute)’
- ‘[path](#TextPathElementPathAttribute)’
- ‘[href](#TextPathElementHrefAttribute)’
- ‘[startOffset](#TextPathElementStartOffsetAttribute)’
- ‘[method](#TextPathElementMethodAttribute)’
- ‘[spacing](#TextPathElementSpacingAttribute)’
- ‘[side](#TextPathElementSideAttribute)’

**DOM Interfaces:**
:

- [SVGTextPathElement](#InterfaceSVGTextPathElement)

Both the ‘[path](#TextPathElementPathAttribute)’ attribute and the ‘[href](#TextPathElementHrefAttribute)’ attribute specify a path along which the [typographic characters](#TermTypographicCharacterUnit) will be rendered.

If both attributes are specified on a ‘[textPath](#TextPathElement)’ element, the path that is used must follow the following order of precedence:

1. ‘[path](#TextPathElementPathAttribute)’ attribute
2. ‘[href](#TextPathElementHrefAttribute)’ attribute
3. ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute

If the ‘[path](#TextPathElementPathAttribute)’ attribute contains an error, the ‘[href](#TextPathElementHrefAttribute)’ attribute must be used.

### <a id="TextPathAttributes"></a>11.8.2. Attributes

**<a id="TextPathElementStartOffsetAttribute"></a>startOffset**
:

An offset from the start of the path for the initial current text position, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm, after converting the path to the ‘[textPath](#TextPathElement)’ element's coordinate system.

If a <length> other than a percentage is given, then the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ represents a distance along the path measured in the current user coordinate system for the ‘[textPath](#TextPathElement)’ element.

If a percentage is given, then the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ represents a percentage distance along the entire path. Thus, startOffset="0%" indicates the start point of the path and startOffset="100%" indicates the end point of the path.

Negative values and values larger than the path length (e.g. 150%) are allowed.

Any [typographic characters](#TermTypographicCharacterUnit) with mid-points that are not on the path are not rendered.

[image: Three paths with various values of 'startOffset' showing clipping when glyphs are outside path region.]

Rendering for different values of the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ attribute. From top to bottom: default value, 50%, -50%.

For paths consisting of a single [closed subpath](paths.md#TermOpenSubpath) (including an equivalent path for a [basic shape](shapes.md#TermBasicShapeElement)), [typographic characters](#TermTypographicCharacterUnit) are rendered along one complete circuit of the path. The text is aligned as determined by the [text-anchor](#TextAnchorProperty) property to a position along the path set by the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ attribute. For the start (end) value, the text is rendered from the start (end) of the line until the initial position along the path is reached again. For the middle, the text is rendered from the middle point in both directions until a point on the path equal distance in both directions from the initial position on the path is reached.

[image: Two circular path with different values of 'startOffset' showing that all glyphs are rendered.]

Rendering for text on a path referencing a ‘[circle](shapes.md#CircleElement)’ with different values of the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ attribute. Left: 0. Right: 75% or -25%. The red circle marks the beginning of the path (after the canonical decomposition of the circle into a path).

[image: Three circular path with different values of 'text-anchor' showing how glyphs are rendered.]

Rendering for text on a path referencing a ‘[circle](shapes.md#CircleElement)’ with different values of the [text-anchor](#TextAnchorProperty) attribute. Left: 'start'. Middle: 'middle'. Right: 'end'. The red circles marks the beginning of the path (after the canonical decomposition of the circle into a path). The blue square marks the reference point for the start of rendering (shifted from the path start by a ‘[startOffset](#TextPathElementStartOffsetAttribute)’ value of 75%). The gray arrow(s) shows the direction of [typographic character](#TermTypographicCharacterUnit) placement and the point at which [typographic character](#TermTypographicCharacterUnit) placement stops. The arrow(s) would be reversed if the [direction](#DirectionProperty) property has a value of rtl.

**Value**
: [<length-percentage>](../css-values-4.md#typedef-length-percentage) | <number>
**[initial value](types.md#TermInitialValue)**
: 0
**Animatable**
: yes
**<a id="TextPathElementMethodAttribute"></a>method**
:

Indicates the method by which text should be rendered along the path.

A value of align indicates that the [typographic character](#TermTypographicCharacterUnit) should be rendered using simple 2×3 matrix transformations such that there is no stretching/warping of the [typographic characters](#TermTypographicCharacterUnit). Typically, supplemental rotation, scaling and translation transformations are done for each [typographic characters](#TermTypographicCharacterUnit) to be rendered. As a result, with align, in fonts where the [typographic characters](#TermTypographicCharacterUnit) are designed to be connected (e.g., cursive fonts), the connections may not align properly when text is rendered along a path.

A value of stretch indicates that the [typographic character](#TermTypographicCharacterUnit) outlines will be converted into paths, and then all end points and control points will be adjusted to be along the perpendicular vectors from the path, thereby stretching and possibly warping the glyphs. With this approach, connected [typographic characters](#TermTypographicCharacterUnit), such as in cursive scripts, will maintain their connections. (Non-vertical straight path segments should be converted to Bézier curves in such a way that horizontal straight paths have an (approximately) constant offset from the path along which the [typographic characters](#TermTypographicCharacterUnit) are rendered.)

[image: English and Arabic text on arcs.]

Rendering of text on a path for different ‘[method](#TextPathElementMethodAttribute)’ values: Left: 'align'. Right: 'stretch'.

**Value**
: align | stretch
**[initial value](types.md#TermInitialValue)**
: align
**Animatable**
: yes
**<a id="TextPathElementSpacingAttribute"></a>spacing**
:

Indicates how the user agent should determine the spacing between [typographic characters](#TermTypographicCharacterUnit) that are to be rendered along a path.

A value of exact indicates that the [typographic characters](#TermTypographicCharacterUnit) should be rendered exactly according to the spacing rules as specified in [Text on a path layout rules](#TextpathLayoutRules).

A value of auto indicates that the user agent should use text-on-a-path layout algorithms to adjust the spacing between [typographic characters](#TermTypographicCharacterUnit) in order to achieve visually appealing results.

**Value**
: auto | exact
**[initial value](types.md#TermInitialValue)**
: exact
**Animatable**
: yes
**<a id="TextPathElementSideAttribute"></a>side**
:

Determines the side of the path the text is placed on (relative to the path direction). Specifying a value of right effectively reverses the path.

[image: Two circular path with different values of 'side' showing glyphs rendered outside the circle for 'left' and inside the circle for 'right'.]

Rendering for text on a path referencing a ‘[circle](shapes.md#CircleElement)’ with different values of the ‘[side](#TextPathElementSideAttribute)’ attribute. Left: left. Right: right.

Added in SVG 2 to allow text either inside or outside [closed subpaths](paths.md#TermOpenSubpath) and [basic shapes](shapes.md#TermBasicShapeElement) (e.g. rectangles, circles, and ellipses).

**Value**
: left | right
**[initial value](types.md#TermInitialValue)**
: left
**Animatable**
: yes
**<a id="TextPathElementPathAttribute"></a>path**
:

A [path data](paths.md#PathDataBNF) string describing the path onto which the [typographic characters](#TermTypographicCharacterUnit) will be rendered. An empty string indicates that there is no path data for the element. This means that the text within the ‘[textPath](#TextPathElement)’ does not render or contribute to the [bounding box](coords.md#TermBoundingBox) of the ‘[text](#TextElement)’ element. If the attribute is not specified, the path specified with ‘[href](#TextPathElementHrefAttribute)’ is used instead.

**Value**
: [path data](paths.md#PathDataBNF)
**[initial value](types.md#TermInitialValue)**
: (none)
**Animatable**
: yes
**<a id="TextPathElementHrefAttribute"></a>href**
:

An [URL reference](linking.md#URLReference) to the ‘[path](paths.md#PathElement)’ element or [basic shape](shapes.md#TermBasicShapeElement) element onto which the glyphs will be rendered, if no ‘[path](#TextPathElementPathAttribute)’ attribute is provided. If the attribute is used, and the <url> is an invalid reference (e.g., no such element exists, or the referenced element is not a ‘[path](paths.md#PathElement)’ element) or [basic shape](shapes.md#TermBasicShapeElement), then the ‘[textPath](#TextPathElement)’ element is in error and its entire contents shall not be rendered by the user agent.

Refer to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).

**Value**
: URL [[URL]](types.md#attribute-url)
**[initial value](types.md#TermInitialValue)**
: See above.
**Animatable**
: yes
The path data coordinates within the referenced ‘[path](paths.md#PathElement)’ element or [basic shape](shapes.md#TermBasicShapeElement) element are assumed to be in the same coordinate system as the current ‘[text](#TextElement)’ element, not in the coordinate system where the ‘[path](paths.md#PathElement)’ element is defined. The [transform](coords.md#TransformProperty) attribute on the referenced ‘[path](paths.md#PathElement)’ element or [basic shape](shapes.md#TermBasicShapeElement) element represents a supplemental transformation relative to the current user coordinate system for the current ‘[text](#TextElement)’ element, including any adjustments to the current user coordinate system due to a possible [transform](coords.md#TransformProperty) property on the current ‘[text](#TextElement)’ element. For example, the following fragment of SVG content:

```
<svg xmlns="http://www.w3.org/2000/svg">
  <g transform="translate(25,25)">
    <defs>
      <path id="path1" transform="scale(2)" d="M0,10 L40,20 80,10" fill="none" stroke="red"/>
    </defs>
  </g>
  <text transform="rotate(45)">
    <textPath href="#path1">Text on a path</textPath>
  </text>
</svg>
  
```

should have the same effect as the following:

```
<svg xmlns="http://www.w3.org/2000/svg">
  <g transform="rotate(45)">
    <defs>
      <path id="path1" transform="scale(2)" d="M0,10 L40,20 80,10" fill="none" stroke="red"/>
    </defs>
    <text>
      <textPath href="#path1">Text on a path</textPath>
    </text>
  </g>
</svg>
  
```

and be equivalent to:

```
<svg xmlns="http://www.w3.org/2000/svg">
  <text transform="rotate(45)">
    <textPath path="M0,20 L80,40 160,20"
            >Text on a path</textPath>
  </text>
</svg>
  
```

Note that the `transform="translate(25,25)"` has no effect on the ‘[textPath](#TextPathElement)’ element, whereas the `transform="rotate(45)"` applies to both the ‘[text](#TextElement)’ and the use of the ‘[path](paths.md#PathElement)’ element as the referenced shape for text on a path. Further note that the `transform="scale(2)"` scales the path (equivalent to multiplying every coordinate by 2 for simple linear paths), but does not scale the text placed along the path.

<a id="ExampleToap01"></a> Example toap01 provides a simple example of text on a path:

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <path id="MyPath"
          d="M 100 200 
             C 200 100 300   0 400 100
             C 500 200 600 300 700 200
             C 800 100 900 100 900 100" />
  </defs>
  <desc>Example toap01 - simple text on a path</desc>

  <use href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="42.5" fill="blue" >
    <textPath href="#MyPath">
      We go up, then we go down, then up again
    </textPath>
  </text>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

[image: Example toap01 — simple text on a path]

Example toap01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/toap01.svg)

<a id="ExampleToap02"></a> Example toap02 shows how ‘[tspan](#TextElement)’ elements can be included within ‘[textPath](#TextPathElement)’ elements to adjust styling attributes and adjust the current text position before rendering a particular glyph. The first occurrence of the word "up" is filled with the color red. Attribute ‘[dy](#TextElementDYAttribute)’ is used to lift the word "up" from the baseline.

The ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’, and ‘[rotate](#TextElementRotateAttribute)’ attributes can only be specified on ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements (but may effect the rendering of glyphs in text on a path — see [text on a path layout rules](#TextpathLayoutRules)).

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <path id="MyPath"
          d="M 100 200 
             C 200 100 300   0 400 100
             C 500 200 600 300 700 200
             C 800 100 900 100 900 100" />
  </defs>
  <desc>Example toap02 - tspan within textPath</desc>

  <use href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="42.5" fill="blue" >
    <textPath href="#MyPath">
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

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

[image: Example toap02 — tspan within textPath]

Example toap02

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/toap02.svg)

<a id="ExampleToap03"></a> Example toap03 demonstrates the use of the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ attribute on the ‘[textPath](#TextPathElement)’ element to specify the start position of the text string as a particular position along the path. Notice that glyphs that fall off the end of the path are not rendered (see [text on a path layout rules](#TextpathLayoutRules)).

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <path id="MyPath"
          d="M 100 200 
             C 200 100 300   0 400 100
             C 500 200 600 300 700 200
             C 800 100 900 100 900 100" />
  </defs>
  <desc>Example toap03 - text on a path with startOffset attribute</desc>

  <use href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="42.5" fill="blue" >
    <textPath href="#MyPath" startOffset="80%">
      We go up, then we go down, then up again
    </textPath>
  </text>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

[image: Example toap03 — text on a path with startOffset attribute]

Example toap03

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/toap03.svg)

### <a id="TextpathLayoutRules"></a>11.8.3. Text on a path layout rules

Conceptually, for text on a path the target path is stretched out into either a horizontal or vertical straight line segment. For horizontal text layout flows, the path is stretched out into a hypothetical horizontal line segment such that the start of the path is mapped to the left of the line segment. For vertical text layout flows, the path is stretched out into a hypothetical vertical line segment such that the start of the path is mapped to the top of the line segment. The standard [text layout](#TextLayoutAlgorithm) rules are applied to the hypothetical straight line segment and the result is mapped back onto the target path. Vertical and bidirectional [text layout](#TextLayoutAlgorithm) rules also apply to text on a path.

The orientation of each glyph along a path is determined individually. For horizontal text layout flows, the default orientation (the up direction) for a given glyph is along the vector that starts at the intersection point on the path to which the glyph is attached and which points in the direction 90 degrees counter-clockwise from the angle of the curve at the intersection point. For vertical text layout flows, the default orientation for a given glyph is along the vector that starts at the intersection point on the path to which the glyph is attached and which points in the direction 180 degrees from the angle of the curve at the intersection point.

[image: Left, horizontal text with the character 'A' on a path. Right, vertical text with the character '字' on a path.]

Default glyph orientation along a path. Left, horizontal text. Right: vertical text.

<a id="ExampleToap04"></a> Example toap04 will be used to illustrate the particular layout rules for text on a path that supplement the basic [text layout](#TextLayoutAlgorithm) rules for straight line horizontal or vertical text.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="3.6cm" viewBox="0 0 1000 300" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <path id="MyPath"
          d="M 100 125 
             C 150 125 250 175 300 175
             C 350 175 450 125 500 125
             C 550 125 650 175 700 175
             C 750 175 850 125 900 125" />
  </defs>
  <desc>Example toap04 - text on a path layout rules</desc>

  <use href="#MyPath" fill="none" stroke="red"  />
  <text font-family="Verdana" font-size="60" fill="blue" letter-spacing="2" >
    <textPath href="#MyPath">
      Choose shame or get war 
    </textPath>
  </text>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="998" height="298"
        fill="none" stroke="blue" stroke-width="2" />
</svg>
```

[image: Example toap04 — text on a path layout rules]

Example toap04

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/toap04.svg)

The following picture does an initial zoom in on the first glyph in the ‘[text](#TextElement)’ element.

[image: Image that shows text on a path]

The small dot above shows the point at which the glyph is attached to the path. The box around the glyph shows the glyph is rotated such that its horizontal axis is parallel to the tangent of the curve at the point at which the glyph is attached to the path. The box also shows the glyph's *<a id="CharWidth"></a>charwidth* (i.e., the amount which the current text position advances horizontally when the glyph is drawn using horizontal text layout).

The next picture zooms in further to demonstrate the detailed layout rules.

[image: Image that shows text on a path]

For left-to-right horizontal text layout along a path (i.e., when the glyph orientation is perpendicular to the [inline-base direction](#TermInlineBaseDirection) the layout rules are as follows:

- Determine the *<a id="StartPointOnPath"></a>startpoint-on-the-path* for the first glyph using attribute ‘[startOffset](#TextPathElementStartOffsetAttribute)’ and property [text-anchor](#TextAnchorProperty). For text-anchor:start, startpoint-on-the-path is the point on the path which represents the point on the path which is ‘[startOffset](#TextPathElementStartOffsetAttribute)’ distance along the path from the start of the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. For text-anchor:middle, startpoint-on-the-path is the point on the path which represents the point on the path which is [ ‘[startOffset](#TextPathElementStartOffsetAttribute)’ minus half of the total advance values for all of the glyphs in the ‘[textPath](#TextPathElement)’ element ] distance along the path from the start of the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. For text-anchor:end, startpoint-on-the-path is the point on the path which represents the point on the path which is [ ‘[startOffset](#TextPathElementStartOffsetAttribute)’ minus the total advance values for all of the glyphs in the ‘[textPath](#TextPathElement)’ element ]. Before rendering the first glyph, the horizontal component of the startpoint-on-the-path is adjusted to take into account various horizontal alignment text properties and attributes, such as a ‘[dx](#TextElementDXAttribute)’ attribute value on a ‘[tspan](#TextElement)’ element. (In the picture above, the startpoint-on-the-path is the leftmost dot on the path.)
- Determine the glyph's charwidth (i.e., the amount which the current text position advances horizontally when the glyph is drawn using horizontal text layout). (In the picture above, the charwidth is the distance between the two dots at the side of the box.)
- Determine the point on the curve which is charwidth distance along the path from the startpoint-on-the-path for this glyph, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. This point is the *<a id="EndPointOnPath"></a>endpoint-on-the-path* for the glyph. (In the picture above, the endpoint-on-the-path for the glyph is the rightmost dot on the path.)
- Determine the *<a id="MidPointOnPath"></a>midpoint-on-the-path*, which is the point on the path which is "halfway" (user agents can choose either a distance calculation or a parametric calculation) between the startpoint-on-the-path and the endpoint-on-the-path. (In the picture above, the midpoint-on-the-path is shown as a white dot.)
- Determine the *<a id="GlyphMidline"></a>glyph-midline*, which is the vertical line in the glyph's coordinate system that goes through the glyph's x-axis midpoint. (In the picture above, the glyph-midline is shown as a dashed line.)
- Position the glyph such that the glyph-midline passes through the midpoint-on-the-path and is perpendicular to the line through the startpoint-on-the-path and the endpoint-on-the-path.
- Align the glyph vertically relative to the midpoint-on-the-path based on property [alignment-baseline](#AlignmentBaselineProperty) and any specified values for attribute ‘[dy](#TextElementDYAttribute)’ on a ‘[tspan](#TextElement)’ element. In the example above, the [alignment-baseline](#AlignmentBaselineProperty) property is unspecified, so the initial value of alignment-baseline:baseline will be used. There are no ‘[tspan](#TextElement)’ elements; thus, the baseline of the glyph is aligned to the midpoint-on-the-path.
- For each subsequent glyph, set a new startpoint-on-the-path as the previous endpoint-on-the-path, but with appropriate adjustments taking into account horizontal kerning tables in the font and current values of various attributes and properties, including spacing properties (e.g. letter-spacing and word-spacing) and ‘[tspan](#TextElement)’ elements with values provided for attributes ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’. All adjustments are calculated as distance adjustments along the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm.
- Glyphs whose midpoint-on-the-path are off the path are not rendered.
- Continue rendering glyphs until there are no more glyphs (or no more space on path).

Comparable rules are used for top-to-bottom vertical text layout along a path (i.e., when the glyph orientation is parallel with the [inline-base direction](#TermInlineBaseDirection), the layout rules are as follows:

- Determine the startpoint-on-the-path using the same method as for horizontal text layout along a path, except that before rendering the first glyph, the horizontal component of the startpoint-on-the-path is adjusted to take into account various vertical alignment text properties and attributes, such as a ‘[dy](#TextElementDYAttribute)’ attribute value on a ‘[tspan](#TextElement)’ element.
- Determine the glyph's charheight (i.e., the amount which the current text position advances vertically when the glyph is drawn using vertical text layout).
- Determine the point on the curve which is charheight distance along the path from the startpoint-on-the-path for this glyph, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm. This point is the endpoint-on-the-path for the glyph.
- Determine the midpoint-on-the-path, which is the point on the path which is "halfway" (user agents can choose either a distance calculation or a parametric calculation) between the startpoint-on-the-path and the endpoint-on-the-path.
- Determine the glyph-midline, which is the horizontal line in the glyph's coordinate system that goes through the glyph's y-axis midpoint.
- Position the glyph such that the glyph-midline passes through the midpoint-on-the-path and is perpendicular to the line through the startpoint-on-the-path and the endpoint-on-the-path.
- Align the glyph horizontally (where horizontal is relative to the glyph's coordinate system) relative to the midpoint-on-the-path based on property [alignment-baseline](#AlignmentBaselineProperty) and any specified values for attribute ‘[dx](#TextElementDXAttribute)’ on a ‘[tspan](#TextElement)’ element.
- For each subsequent glyph, set a new startpoint-on-the-path as the previous endpoint-on-the-path, but with appropriate adjustments taking into account vertical kerning tables in the font and current values of various attributes and properties, including spacing properties and ‘[tspan](#TextElement)’ elements with values provided for attributes ‘[dx](#TextElementDXAttribute)’ and ‘[dy](#TextElementDYAttribute)’. All adjustments are calculated as distance adjustments along the path, calculated using the user agent's [distance along the path](paths.md#DistanceAlongAPath) algorithm.
- Glyphs whose midpoint-on-the-path are off either end of an [open subpath](paths.md#TermClosedSubpath) are not rendered.
- Continue rendering glyphs until there are no more glyphs.

In the calculations above, if either the startpoint-on-the-path or the endpoint-on-the-path is off the end of the path, then extend the path beyond its end points with a straight line that is parallel to the tangent at the path at its end point so that the midpoint-on-the-path can still be calculated.

When the [inline-base direction](#TermInlineBaseDirection) is horizontal, then any ‘x’ attributes on ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ elements represent new absolute offsets along the path, thus providing explicit new values for startpoint-on-the-path. Any ‘y’ attributes on ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ elements are ignored. When the [inline-base direction](#TermInlineBaseDirection) is vertical, then any ‘y’ attributes on ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ elements represent new absolute offsets along the path, thus providing explicit new values for startpoint-on-the-path. Any ‘x’ attributes on ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ elements are ignored.

After positioning all characters within the ‘[textPath](#TextPathElement)’, the [current text position](#TermCurrentTextPosition) is set to the end point of the path, after adjusting for the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ in the case of paths that are a single closed loop. In other words, text that follows a ‘[textPath](#TextPathElement)’ element (but is still inside a ‘[text](#TextElement)’ element) that does not have explicit positioning information (‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes) is positioned from the end of the path.

[image: Text on a path showing the starting point for rendering text after the <textPath> element.]

The starting point for text after the ‘[textPath](#TextPathElement)’ element without explicit positioning information is at the end of the path (red dot).

## <a id="TextRenderingOrder"></a>11.9. Text rendering order

A ‘[text](#TextElement)’ element is rendered in one or more chunks. Each chunk (as produced by the [text layout algorithm](#TextLayoutAlgorithm)) is rendered, one after the other, in document order. Each rendered chunk, which consists of one or more glyphs, is filled and stroked as if it were a single path.

This means that all glyphs in the chunk should be filled, then all glyphs stroked at once, or the reverse according to the value of the [paint-order](painting.md#PaintOrderProperty) property.

For the purposes of painting, a ‘[text](#TextElement)’ has zero, one, or more [equivalent paths](paths.md#TermEquivalentPath), one for each chunk. Each equivalent path consists of one subpath per glyph within that chunk.

Since the [fill-rule](painting.md#FillRuleProperty) property does not apply to SVG text elements, the specific order of the subpaths within the [equivalent path](paths.md#TermEquivalentPath) does not matter.

The specific position of the start of each subpath, and the direction that the path progresses around glyph shape, is not defined. However, user agents should be consistent for a given font and glyph.

This means that dashed strokes on text may not place the dash pattern at the same positions across different implementations.

## <a id="TextProperties"></a>11.10. Properties and pseudo-elements

CSS offers a multitude of properties for styling text. In general, only two sets of properties are applicable to SVG: those that determine which glyphs are to be rendered (font-family, font-style, etc.) and those that apply at the *paragraph* level ([direction](#DirectionProperty), [writing-mode](#WritingModeProperty), [line-height](#LineHeightProperty), letter-spacing, etc.).

The list of CSS properties that must be supported on SVG text elements can be found in the [Styling](styling.md#RequiredProperties) chapter.

- Properties that apply only to pre-formatted text and auto-wrapped text where the [wrapping area](#TermWrappingArea) is defined by the [inline-size](#InlineSizeProperty) property:
[text-anchor](#TextAnchorProperty).
- Properties that apply only to auto-wrapped text except for text where the [wrapping area](#TermWrappingArea) is defined by the [inline-size](#InlineSizeProperty) property:
text-align, text-align-last, text-justify,
- Properties that apply only to auto-wrapped text:
text-indent, line-break, word-break, hyphens, word-wrap, overflow-wrap.

Additionally, the @font-face rule must be supported for font selection as well as the ::first-line and ::first-letter pseudo-elements must be supported on ‘[text](#TextElement)’ elements. In interactive modes, the ::selection pseudo-element must also be supported.

Other CSS properties that affect text layout and rendering may also be supported by an SVG user agent; their effect should be taken into account as part of the CSS text layout step of the overall SVG text layout process.

For example, while SVG 2 does not require support for the text-combine-upright property, its behavior in an SVG context should be obvious.

A number of CSS properties must not have any effect on SVG text elements:

- Border properties (such as [border-top-style](../css-backgrounds-3.md#propdef-border-top-style)). Boxes generated by the CSS layout process must not be sized as if they had any borders. The used value for all of the [border-style](../css-backgrounds-3.md#propdef-border-style) component longhand properties on SVG text elements is none.
- Display property values other than none and inline. Like all other SVG elements, any value of [display](render.md#VisibilityControl) on an SVG text element other than none is handled just as if it were inline. Thus it is not possible for a single ‘[text](#TextElement)’ element to have any block children.
- The float property, whose used value on SVG text elements is none.
- The position property, whose used value on SVG text elements is static.
- Margin properties (such as margin-top), whose used values on SVG text elements are 0.
- The text-align property when text is pre-formatted, whose used value on such SVG text elements is start.
- The content property, whose used value on SVG text elements is none.

Additionally, the ::before and ::after generated content pseudo-elements must not apply to SVG text elements.

A future specification may introduce support for the ::before and ::after generated content pseudo-elements; authors should not rely on them being ignored.

### <a id="TextPropertiesSVG"></a>11.10.1. SVG properties

This section covers properties that are not covered elsewhere in this specification and that are specific to SVG.

#### <a id="TextAnchoringProperties"></a>11.10.1.1. Text alignment, the ‘text-anchor’ property

The [text-anchor](#TextAnchorProperty) property is used to align (start-, middle- or end-alignment) a string of *pre-formatted* text or *auto-wrapped text* where the [wrapping area](#TermWrappingArea) is determined from the [inline-size](#InlineSizeProperty) property relative to a given point. It is not applicable to other types of *auto-wrapped text*, see instead text-align. For multi-line text, the alignment takes place for each line.

The [text-anchor](#TextAnchorProperty) property is applied to each individual [text chunk](#TermTextChunk) within a given ‘[text](#TextElement)’ element. Each text chunk has an initial [current text position](#TermCurrentTextPosition), which represents the point in the user coordinate system resulting from (depending on context) application of the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes on the ‘[text](#TextElement)’ element, any ‘[x](#TextElementXAttribute)’ or ‘[y](#TextElementYAttribute)’ attribute values on a ‘[tspan](#TextElement)’ element assigned explicitly to the first rendered character in a text chunk, or determination of the initial current text position for a ‘[textPath](#TextPathElement)’ element. Each text chunk also has a final [current text position](#TermCurrentTextPosition) which is the [current text position](#TermCurrentTextPosition) after placing the last glyph in the [text chunk](#TermTextChunk). The positions are determined before applying the [text-anchor](#TextAnchorProperty) property.

| Name: | *<a id="TextAnchorProperty"></a>text-anchor* |
| --- | --- |
| Value: | start \| middle \| end |
| Initial: | start |
| Applies to: | [text content elements](#TermTextContentElement) |
| Inherited: | yes |
| Percentages: | N/A |
| Media: | visual |
| Computed value: | as specified |
| Animation type: | discrete |

Values have the following meanings:

**start**
: The rendered characters are aligned such that the start of the resulting rendered text is at the initial current text position. For an element with a [direction](#DirectionProperty) property value of "ltr" (typical for most European languages), the left side of the text is rendered at the initial text position. For an element with a [direction](#DirectionProperty) property value of "rtl" (typical for Arabic and Hebrew), the right side of the text is rendered at the initial text position. For an element with a vertical primary text direction (often typical for Asian text), the top side of the text is rendered at the initial text position.
**middle**
: The rendered characters are shifted such that the geometric middle of the resulting rendered text (determined from the initial and final [current text position](#TermCurrentTextPosition) before applying the [text-anchor](#TextAnchorProperty) property) is at the initial current text position.
**end**
: The rendered characters are shifted such that the end of the resulting rendered text (final [current text position](#TermCurrentTextPosition) before applying the [text-anchor](#TextAnchorProperty) property) is at the initial current text position. For an element with a [direction](#DirectionProperty) property value of "ltr" (typical for most European languages), the right side of the text is rendered at the initial text position. For an element with a [direction](#DirectionProperty) property value of "rtl" (typical for Arabic and Hebrew), the left side of the text is rendered at the initial text position. For an element with a vertical primary text direction (often typical for Asian text), the bottom of the text is rendered at the initial text position.
An example of using [text-anchor](#TextAnchorProperty) on multi-line text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="100" viewBox="0 0 300 100">

  <text x="150" y="30" style="font: 20px sans-serif; white-space: pre-line;
                              text-anchor: middle;">
    This multi-line text is
    anchored to the middle.</text>

</svg>
```

[image: Image showing two lines of text centered via text-anchor.]

The preserved line-feed creates two [text chunks](#TermTextChunk), each anchored independently.

Another example of using [text-anchor](#TextAnchorProperty) on multi-line text.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="200" height="150" viewBox="0 0 200 150">
  <path d="m 100,0 0,150" style="fill:none;stroke:#add8e6"/>
  <text x="100 100 100" y="50 95 140"
	style="font-size: 42px; text-anchor: middle">I❤SVG</text>
</svg>
```

[image: Image showing three lines of text divided as 'I', '❤', 'SVG'.]

The text is divided into three [text chunks](#TermTextChunk) (due to the three coordinates in the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes). Each [text chunk](#TermTextChunk) is independently anchored.

#### <a id="GlyphOrientationHorizontalProperty"></a>11.10.1.2. The ‘glyph-orientation-horizontal’ property

This property has been removed in SVG 2.

#### <a id="GlyphOrientationVerticalProperty"></a>11.10.1.3. The ‘glyph-orientation-vertical’ property

This property applies only to vertical text. It has been obsoleted in SVG 2 and partially replaced by the text-orientation property of CSS Writing Modes Level 3. The following SVG 1.1 values must still be supported by aliasing the property as a shorthand to text-orientation as follows:

- 'auto' to 'mixed';
- '0deg', and '0', to 'upright'.
- '90deg', and '90', to 'sideways'.

Any other values must be treated as invalid.

#### <a id="KerningProperty"></a>11.10.1.4. The ‘kerning’ property

The ‘kerning’ property has been removed in SVG 2.

SVG 1.1 uses the 'kerning' property to determine if the font kerning tables should be used to adjust inter-glyph spacing. It also allows a <length> value which if given is added to the spacing between glyphs (supplemental to any from the letter-spacing property). This property is replaced in SVG 2 by the CSS font-kerning property which solely controls turning on/off the use of the font kerning tables.

### <a id="TextPropertiesAdaptions"></a>11.10.2. SVG adaptions

This section covers CSS properties that are not covered elsewhere in this specification and have either SVG specific adaptions or are significantly altered from SVG 1.1.

#### <a id="FontVariantProperty"></a>11.10.2.1. The ‘font-variant’ property

*This section is not normative*

The font-variant property gets defined by the CSS Font Module ([[css-font-3](refs.md#ref-css-fonts-3)]).

CSS Font Module changes the meaning of the font-variant property from that defined by CSS 2.1. It has been repurposed (and its functionality greatly expanded) as a shorthand for selecting font variants from within a single font.

SVG 2 supports all font-variant longhand properties (e.g. font-variant-ligatures).

#### <a id="LineHeightProperty"></a>11.10.2.2. The ‘line-height’ property

SVG uses the [line-height](#LineHeightProperty) property to determine the amount of leading space which is added between lines in multi-line text (both for horizontal and vertical text). It is not applicable to text on a path.

Except for the additional information provided here, the normative definition of the [line-height](#LineHeightProperty) property is in the CSS 2.1 specification ([[CSS2](refs.md#ref-css2)]).

#### <a id="WritingModeProperty"></a>11.10.2.3. The ‘writing-mode’ property

*This section is not normative.*

The [writing-mode](#WritingModeProperty) property gets defined by the CSS Writing Modes Module ([[css-writing-modes-3](refs.md#ref-css-writing-modes-3)]).

This property sets the [block-flow direction](#TermBlockFlowDirection); or in-other-words, the direction in which lines of text are stacked. As a consequence it also determines if the text has a horizontal or vertical orientation.

#### <a id="DirectionProperty"></a>11.10.2.4. The ‘direction’ property

The property specifies the [inline-base direction](#TermInlineBaseDirection) of a ‘[text](#TextElement)’ or ‘[tspan](#TextElement)’ element. It defines the start and end points of a line of text as used by the [text-anchor](#TextAnchorProperty) and [inline-size](#InlineSizeProperty) properties. It also may affect the direction in which characters are positioned if the unicode-bidi property's value is either embed or bidi-override.

The [direction](#DirectionProperty) property applies only to glyphs oriented perpendicular to the [inline-base direction](#TermInlineBaseDirection), which includes the usual case of horizontally-oriented Latin or Arabic text and the case of narrow-cell Latin or Arabic characters rotated 90 degrees clockwise relative to a top-to-bottom [inline-base direction](#TermInlineBaseDirection).

<a id="issue54"></a> Reviewers, please take special care to ensure this agrees with CSS3 Writing modes.

Except for the additional information provided here, the normative definition of the [direction](#DirectionProperty) property is in CSS Writing Modes Level 3 ([[css-writing-modes-3](refs.md#ref-css-writing-modes-3)]).

In many cases, the bidirectional algorithm from Unicode [[UNICODE](refs.md#ref-unicode)] produces the desired result automatically, and in such cases the author does not need to use these properties. For other cases, such as when using right-to-left languages, it may be sufficient to add the [direction](#DirectionProperty) property to the [outermost svg element](struct.md#TermOutermostSVGElement), and allow that direction to inherit to all text elements, as in the following example (which may be used as a template):

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="100%" height="100%" viewBox="0 0 600 72"
     direction="rtl" xml:lang="fa">

  <title direction="ltr" xml:lang="en">Right-to-left Text</title>
  <desc direction="ltr" xml:lang="en">
    A simple example for using the 'direction' property in documents
    that predominantly use right-to-left languages.
  </desc>

  <text x="300" y="50" text-anchor="middle" font-size="36">داستان SVG 1.1 SE طولا ني است.</text>

</svg>
```

[image: Example]

Example

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/rtl-text.svg)

Below is another example, where implicit bidi reordering is not sufficient:

```
<?xml version="1.0" encoding="utf-8"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width="100%" height="100%" viewBox="0 0 600 72"
     direction="rtl" xml:lang="he">

  <title direction="ltr" xml:lang="en">Right-to-left Text</title>
  <desc direction="ltr" xml:lang="en">
    An example for using the 'direction' and 'unicode-bidi' properties
    in documents that predominantly use right-to-left languages.
  </desc>

  <text x="300" y="50" text-anchor="middle" font-size="36"> כתובת MAC:&#x200F;
    <tspan direction="ltr" unicode-bidi="embed">00-24-AF-2A-55-FC</tspan> 
  </text>

</svg>
```

[image: Example]

Example

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/rtl-complex.svg)

#### <a id="DominantBaselineProperty"></a>11.10.2.5. The ‘dominant-baseline’ property

This property is defined in the CSS Line Layout Module 3 specification. See 'dominant-baseline'. [[css-inline-3](refs.md#ref-css-inline-3)]

SVG 2 introduces some changes to the definition of this property. In particular:

- The 'reset-size' value is no longer supported. Any change in font-size resets the dominant baseline table.
- The 'use-script' and 'no-change' values are no longer supported.
- The property is now inherited. (Behavior is effectively unchanged.)

SVG uses the value of the [dominant-baseline](#DominantBaselineProperty) property to align glyphs relative to the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes. For the text-orientation value sideways, the auto value for [dominant-baseline](#DominantBaselineProperty) is alphabetic; however, for backwards compatibility, the glyphs should be aligned to the ‘[x](#TextElementXAttribute)’ and ‘[y](#TextElementYAttribute)’ attributes using the value central.

<a id="issue56"></a> We are interested in any actual use where one would prefer the old behavior.

#### <a id="AlignmentBaselineProperty"></a>11.10.2.6. The ‘alignment-baseline’ property

This property is defined in the CSS Line Layout Module 3 specification. See 'alignment-baseline'. [[css-inline-3](refs.md#ref-css-inline-3)]

The vertical-align property shorthand should be preferred in new content.

SVG 2 introduces some changes to the definition of this property. In particular: the values 'auto', 'before-edge', and 'after-edge' have been removed. For backwards compatibility, 'text-before-edge' should be mapped to 'text-top' and 'text-after-edge' should be mapped to 'text-bottom'. Neither 'text-before-edge' nor 'text-after-edge' should be used with the vertical-align property.

#### <a id="BaselineShiftProperty"></a>11.10.2.7. The ‘baseline-shift’ property

This property is defined in the CSS Line Layout Module 3 specification. See 'baseline-shift'. [[css-inline-3](refs.md#ref-css-inline-3)]

The vertical-align property shorthand should be preferred in new content.

#### <a id="LetterSpacingProperty"></a>11.10.2.8. The ‘letter-spacing’ property

Except as noted, see CSS Text Level 3 for the definition of the letter-spacing.([[css-text-3](refs.md#ref-css-text-3)]).

#### <a id="WordSpacingProperty"></a>11.10.2.9. The ‘word-spacing’ property

Except as noted, see CSS Text Level 3 for the definition of the word-spacing.([[css-text-3](refs.md#ref-css-text-3)]).

#### <a id="TextOverflowProperty"></a>11.10.2.10. The ‘text-overflow’ property

New in SVG 2. Added to allow user agents to handle text strings that overflow a predefined wrapping area in a more useful way. Aligns SVG and HTML/CSS text processing.

See the CSS3 Overflow specification for the definition of 'text-overflow'. [[css-overflow-3](refs.md#ref-css-overflow-3)]

SVG uses the [text-overflow](#TextOverflowProperty) property to control how [text content block elements](#TermTextContentBlockElement) render when text overflows [line boxes](#TermLineBox) as, for example, can happen when the white-space property has the value nowrap. The property does not apply to pre-formatted text or text-on-a-path.

In SVG [text-overflow](#TextOverflowProperty) has an effect if there is a validly specified [wrapping area](#TermWrappingArea), regardless of the computed value of the [overflow](render.md#OverflowAndClipProperties) property on the [text content block element](#TermTextContentBlockElement).

If the [text-overflow](#TextOverflowProperty) property has the value ellipsis then if the text that is to be rendered overflows the [wrapping area](#TermWrappingArea) an ellipsis is rendered such that it fits within the given area. For the purposes of rendering, the ellipsis is treated as if it replaced the characters at the point where it is inserted.

If the [text-overflow](#TextOverflowProperty) property has the value clip then any text that overflows the [wrapping area](#TermWrappingArea) is clipped. Characters may be partially rendered.

Any other value for [text-overflow](#TextOverflowProperty) is treated as if it wasn't specified.

Note that the effect of [text-overflow](#TextOverflowProperty) is purely visual, the ellipsis itself does not become part of the DOM. For all the DOM methods it is as if [text-overflow](#TextOverflowProperty) was not applied, and as if the [wrapping area](#TermWrappingArea) did not constrain the text.

The following example shows the use of [text-overflow](#TextOverflowProperty). The top line shows text as it would normally be rendered (text overflows due to white-space value pre and is displayed due to [overflow](render.md#OverflowAndClipProperties) value visible). The middle line shows text with [text-overflow](#TextOverflowProperty) value clip, and the bottom line shows text with [text-overflow](#TextOverflowProperty) value ellipsis.

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="300" height="150" viewBox="0 0 300 150">
  <style>
    text { font: 25px sans-serif; white-space: pre; }
    path { fill: none; stroke: #add8e6; }
  </style>

  <path d="m  50,0 0,150"/>
  <path d="m 200,0 0,150"/>

  <text x="50" y="35"  inline-size="100" style="overflow:visible">SVG is awesome</text>
  <text x="50" y="85"  inline-size="100" style="text-overflow:clip">SVG is awesome</text>
  <text x="50" y="135" inline-size="100" style="text-overflow:ellipsis">SVG is awesome</text>

</svg>
```

[image: Image showing the use of the text-overflow property.]

The [text-overflow](#TextOverflowProperty) property used on text elements, the bottom line showing text with an ellipsis applied.

### <a id="WhiteSpace"></a>11.10.3. White space

New in SVG 2. Added white-space to allow a more useful way to control white-space handling. Aligns SVG and HTML/CSS text processing. ‘[xml:space](struct.md#XMLSpaceAttribute)’ deprecated in new content, retained for backwards compatibility.

#### <a id="TextWhiteSpace"></a>11.10.3.1. SVG 2 Preferred white space handling, the ‘white-space’ property

Rendering of white space in SVG 2 is controlled by the white-space property. This specifies two things:

- whether and how white space inside the element is collapsed
- whether lines may wrap at unforced soft wrap opportunities

Values and their meanings are defined in CSS Text Module Level 3.[[css-text-3](refs.md#ref-css-text-3)]

An example of using the white-space value pre-line.

```
<svg xmlns="http://www.w3.org/2000/svg">
     width="200" height="200" viewBox="0 0 200 200">

     <text x="150" y="30" style="font: 20px IPAMincho; writing-mode: vertical-rl;
	                         white-space: pre-line;">
       千利奴流乎和加
       餘多連曽津祢那
       良牟有為能於久
       耶万計不己衣天
       阿佐伎喩女美之
       恵比毛勢須</text>
</svg>
```

[image: Japanese poem showing traditional line-breaking after every seven kanji.]

Example of multi-line vertical text with line breaks. The text is from the Japanese poem *Iroha*. The lines are broken at traditional places.

#### <a id="LegacyXMLSpace"></a>11.10.3.2. Legacy white-space handling, the ‘xml:space’ property

<a id="XMLSpaceAttribute"></a> For compatibility, SVG 2 also supports the XML attribute ‘[xml:space](struct.md#XMLSpaceAttribute)’ to specify the handling of white space characters within a given ‘[text](#TextElement)’ element's character data. New content should not use ‘[xml:space](struct.md#XMLSpaceAttribute)’ but instead, use the white-space property.

Note that any child element of a ‘[text](#TextElement)’ element may also have an ‘[xml:space](struct.md#XMLSpaceAttribute)’ attribute which will apply to that child element's text content. The SVG user agent has special processing rules associated with this attribute as described below. These are behaviors that occur subsequent to XML parsing [XML] and any construction of a DOM.

‘[xml:space](struct.md#XMLSpaceAttribute)’ is an inheritable attribute which can have one of two values:

**'default'**
: (The initial/default value for ‘[xml:space](struct.md#XMLSpaceAttribute)’.) When xml:space="default", the SVG user agent will do the following using a copy of the original character data content. First, it will remove all newline characters. Then it will convert all tab characters into space characters. Then, it will strip off all leading and trailing space characters. Then, all contiguous space characters will be consolidated.
**'preserve'**
: When xml:space="preserve", the SVG user agent will do the following using a copy of the original character data content. It will convert all newline and tab characters into space characters. Then, it will draw all space characters, including leading, trailing and multiple contiguous space characters. Thus, when drawn with xml:space="preserve", the string `"a   b"` (three spaces between "a" and "b") will produce a larger separation between "a" and "b" than `"a b"` (one space between "a" and "b").
The following example illustrates that line indentation can be important when using xml:space="default". The fragment below show two pairs of similar ‘[text](#TextElement)’ elements, with both ‘[text](#TextElement)’ elements using xml:space="default". For these examples, there is no extra white space at the end of any of the lines (i.e., the line break occurs immediately after the last visible character).

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

The first pair of ‘[text](#TextElement)’ elements above show the effect of indented character data. The attribute xml:space="default" in the first ‘[text](#TextElement)’ element instructs the user agent to:

- convert all tabs (if any) to space characters,
- strip out all line breaks (i.e., strip out the line breaks at the end of lines [01], [02] and [03]),
- strip out all leading space characters (i.e., strip out space characters before "WS example" on line [02]),
- strip out all trailing space characters (i.e., strip out space characters before "</text>" on line [04]),
- consolidate all intermediate space characters (i.e., the space characters before "indented lines" on line [03]) into a single space character.

The second pair of ‘[text](#TextElement)’ elements above show the effect of non-indented character data. The attribute xml:space="default" in the third ‘[text](#TextElement)’ element instructs the user agent to:

- convert all tabs (if any) to space characters,
- strip out all line breaks (i.e., strip out the line breaks at the end of lines [07], [08] and [09]),
- strip out all leading space characters (there are no leading space characters in this example),
- strip out all trailing space characters (i.e., strip out space characters before "</text>" on line [10]),
- consolidate all intermediate space characters into a single space character (in this example, there are no intermediate space characters).

Note that XML parsers are required to convert the standard representations for a newline indicator (e.g., the literal two-character sequence "U+000D U+000A", CARRIAGE-RETURN LINE-FEED or the stand-alone literals U+000D or U+000A) into the single character U+000A before passing character data to the application. Thus, each newline in SVG will be represented by the single character U+000A, no matter what representation for newlines might have been used in the original resource. (See XML end-of-line handling.)

Any features in the SVG language or the SVG DOM that are based on character position number, such as the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’ and ‘[rotate](#TextElementRotateAttribute)’ attributes on the ‘[text](#TextElement)’ and ‘[tspan](#TextElement)’ elements, are based on character position after applying the white space handling rules described here. In particular, if xml:space="default", it is often the case that white space characters are removed as part of processing. Character position numbers index into the text string after the white space characters have been removed per the rules in this section.

Note that a glyph corresponding to a white-space character should only be displayed as a visible but blank space, even if the glyph itself happens to be non-blank. See display of unsupported characters [[UNICODE](refs.md#ref-unicode)].

The ‘[xml:space](struct.md#XMLSpaceAttribute)’ attribute is:

Animatable: no.

#### <a id="DuplicateWhiteSpace"></a>11.10.3.3. Duplicate white-space directives

Older, SVG 1.1 content will use ‘[xml:space](struct.md#XMLSpaceAttribute)’. New content, and older content that is being reworked, will use white-space and remove any existing ‘[xml:space](struct.md#XMLSpaceAttribute)’. However, user agents may come across content which uses both methods on the same element. If the white-space property is set on any element, then the value of ‘[xml:space](struct.md#XMLSpaceAttribute)’ is ignored.

## <a id="TextDecorationProperties"></a>11.11. Text decoration

Text in SVG can be *decorated* with an underline, overline, and/or strike-through. The position and style of the decoration is determined respectively by the [text-decoration-line](#TextDecorationProperties) and [text-decoration-style](#TextDecorationProperties) properties, or by the [text-decoration](#TextDecorationProperties) shorthand property as defined in the Line Decoration section of the CSS Text Decoration Module Level 3 [([css-text-decor-3](refs.md#ref-css-text-decor-3))] specification.

The fill and stroke of the text decoration are given by the fill and stroke of the text at the point where the text decoration is declared (see example below).

The [text-decoration-line](#TextDecorationProperties) and [text-decoration-style](#TextDecorationProperties) properties are new in SVG 2. The SVG 1.1/CSS 2.1 [text-decoration](#TextDecorationProperties) property is transformed in a backwards compatible way to a short hand for these properties.

The order in which the text and decorations are drawn is defined by the Painting Order of Text Decorations section of CSS Text Decoration Module Level 3. The paint order of the text decoration itself (fill/stroke) is determined by the value of the [paint-order](painting.md#PaintOrderProperty) property at the point where the text decoration is declared.

<a id="ExampleTextDecoration01"></a> Example textdecoration01 provides examples for [text-decoration](#TextDecorationProperties). The first line of text has no value for [text-decoration](#TextDecorationProperties), so the initial value of text-decoration:none is used. The second line shows text-decoration:line-through. The third line shows text-decoration:underline. The fourth line illustrates the rule whereby decorations are rendered using the same fill and stroke properties as are present on the element for which the [text-decoration](#TextDecorationProperties) is specified. Since [text-decoration](#TextDecorationProperties) is specified on the ‘[text](#TextElement)’ element, all text within the ‘[text](#TextElement)’ element has its underline rendered with the same fill and stroke properties as exist on the ‘[text](#TextElement)’ element (i.e., blue fill, red stroke), even though the various words have different fill and stroke property values. However, the word "different" explicitly specifies a value for [text-decoration](#TextDecorationProperties); thus, its underline is rendered using the fill and stroke properties as the ‘[tspan](#TextElement)’ element that surrounds the word "different" (i.e., yellow fill, darkgreen stroke):

```
<?xml version="1.0" standalone="no"?>
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

[image: Example textdecoration01 — behavior of 'text-decoration' property]

Example textdecoration01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/text/textdecoration01.svg)

## <a id="TextSelection"></a>11.12. Text selection and clipboard operations

[Conforming SVG viewers](conform.md#ConformingSVGViewers) on systems which have the capacity for text selection (e.g., systems which are equipped with a pointer device such as a mouse) and which have system clipboards for copy/paste operations are required to support:

- user selection of text strings in SVG content
- the ability to copy selected text strings to the system clipboard

A text selection operation starts when all of the following occur:

- the user positions the pointing device over a glyph that has been rendered as part of a ‘[text](#TextElement)’ element, initiates a *select* operation (e.g., pressing the standard system mouse button for select operations) and then moves the pointing device while continuing the *select* operation (e.g., continuing to press the standard system mouse button for select operations)
- no other visible graphics element has been painted above the glyph at the point at which the pointing device was clicked
- no [links](linking.md#Links) or [events](interact.md) have been assigned to the ‘[text](#TextElement)’, ‘[tspan](#TextElement)’ or ‘[textPath](#TextPathElement)’ element(s) (or their ancestors) associated with the given glyph.

As the text selection operation proceeds (e.g., the user continues to press the given mouse button), all associated events with other graphics elements are ignored (i.e., the text selection operation is modal) and the SVG user agent shall dynamically indicate which characters are selected by applying styles for the `::selection` pseudo-class. As the pointer is moved during the text selection process, the end glyph for the text selection operation is the glyph within the same ‘[text](#TextElement)’ element whose glyph cell is closest to the pointer. All characters within the ‘[text](#TextElement)’ element whose position within the ‘[text](#TextElement)’ element is between the start of selection and end of selection shall be highlighted, regardless of position on the canvas and regardless of any graphics elements that might be above the end of selection point.

Once the text selection operation ends (e.g., the user releases the given mouse button), the selected text will stay highlighted until an event occurs which cancels text selection, such as a pointer device activation event (e.g., pressing a mouse button).

Detailed rules for determining which characters to highlight during a text selection operation are provided in [Text selection implementation notes](#TextSelectionImplementationNotes).

For systems which have system clipboards, the SVG user agent is required to provide a user interface for initiating a copy of the currently selected text to the system clipboard. It is sufficient for the SVG user agent to post the selected text string in the system's appropriate clipboard format for plain text, but it is preferable if the SVG user agent also posts a rich text alternative which captures the various font properties associated with the given text string.

For bidirectional text, the user agent must support text selection in logical order, which will result in discontinuous highlighting of glyphs due to the bidirectional reordering of characters. User agents can provide an alternative ability to select bidirectional text in visual rendering order (i.e., after bidirectional text layout algorithms have been applied), with the result that selected character data might be discontinuous logically. In this case, if the user requests that bidirectional text be copied to the clipboard, then the user agent is required to make appropriate adjustments to copy only the visually selected characters to the clipboard.

SVG authors and [SVG generators](conform.md#TermSVGGenerator) should order their text strings to facilitate properly ordered text selection within SVG viewing applications such as Web browsers; in other words, the DOM order of the text should match the natural reading order of the text.

### <a id="TextSelectionImplementationNotes"></a>11.12.1. Text selection implementation notes

The following implementation notes describe the algorithm for deciding which characters are selected during a [text selection](#TextSelection) operation.

As the text selection operation occurs (e.g., while the user clicks and drags the mouse to identify the selection), the user agent determines a *start selection position* and an *end selection position*, each of which represents a position in the text string between two characters. After determining start selection position and end selection position, the user agent selects the appropriate characters, where the resulting text selection consists of either:

- no selection or
- a *start character*, an *end character* (possibly the same character), and all of the characters within the same ‘[text](#TextElement)’ element whose position in the DOM is logically between the start character and end character.

On systems with pointer devices, to determine the *start selection position*, the SVG user agent determines which boundary between characters corresponding to rendered glyphs is the best target (e.g., closest) based on the current pointer location at the time of the event that initiates the selection operation (e.g., the mouse down event). The user agent then tracks the completion of the selection operation (e.g., the mouse drag, followed ultimately by the mouse up). At the end of the selection operation, the user agent determines which boundary between characters is the best target (e.g., closest) for the *end selection position*.

If no character reordering has occurred due to bidirectionality, then the selection consists of all characters between the *start selection position* and *end selection position*. For example, if a ‘[text](#TextElement)’ element contains the string "abcdef" and the start selection position and end selection positions are 0 and 3 respectively (assuming the left side of the "a" is position zero), then the selection will consist of "abc".

When the user agent is implementing selection of bidirectional text, and when the selection starts (or ends) between characters which are not contiguous in logical order, then there might be multiple potential combinations of characters that can be considered part of the selection. The algorithms to choose among the combinations of potential selection options shall choose the selection option which most closely matches the text string's visual rendering order.

When multiple characters map inseparably to a given set of one or more glyphs, the user agent can either disallow the selection to start in the middle of the glyph set or can attempt to allocate portions of the area taken up by the glyph set to the characters that correspond to the glyph.

For systems which support pointer devices such as a mouse, the user agent is required to provide a mechanism for selecting text even when the given text has associated event handlers or links, which might block text selection due to event processing precedence rules (see [Pointer events](interact.md#PointerEvents)). One implementation option: For platforms which support a pointer device such as a mouse, the user agent may provide for a small additional region around character cells which initiates text selection operations but does not initiate event handlers or links.

## <a id="DOMInterfaces"></a>11.13. DOM interfaces

### <a id="InterfaceSVGTextContentElement"></a>11.13.1. Interface SVGTextContentElement

The [SVGTextContentElement](#InterfaceSVGTextContentElement) interface is implemented by elements that support rendering child text content.

For the methods on this interface that refer to an index to a character or a number of characters, these references are to be interpreted as an index to a UTF-16 code unit or a number of UTF-16 code units, respectively. This is for consistency with DOM Level 2 Core, where methods on the CharacterData interface use UTF-16 code units as indexes and counts within the character data. Thus for example, if the text content of a ‘[text](#TextElement)’ element is a single non-BMP character, such as U+10000, then invoking [getNumberOfChars](#__svg__SVGTextContentElement__getNumberOfChars) on that element will return 2 since there are two UTF-16 code units (the surrogate pair) used to represent that one character.

```
[Exposed=Window]
interface SVGTextContentElement : SVGGraphicsElement {

  // lengthAdjust Types
  const unsigned short LENGTHADJUST_UNKNOWN = 0;
  const unsigned short LENGTHADJUST_SPACING = 1;
  const unsigned short LENGTHADJUST_SPACINGANDGLYPHS = 2;

  [SameObject] readonly attribute SVGAnimatedLength textLength;
  [SameObject] readonly attribute SVGAnimatedEnumeration lengthAdjust;

  long getNumberOfChars();
  float getComputedTextLength();
  float getSubStringLength(unsigned long charnum, unsigned long nchars);
  DOMPoint getStartPositionOfChar(unsigned long charnum);
  DOMPoint getEndPositionOfChar(unsigned long charnum);
  DOMRect getExtentOfChar(unsigned long charnum);
  float getRotationOfChar(unsigned long charnum);
  long getCharNumAtPosition(optional DOMPointInit point = {});
  undefined selectSubString(unsigned long charnum, unsigned long nchars);
};
```

The numeric length adjustment type constants defined on [SVGTextContentElement](#InterfaceSVGTextContentElement) are used to represent the keyword values that the ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’ attribute can take. Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGTextContentElement__LENGTHADJUST_SPACING"></a>LENGTHADJUST_SPACING** | The spacing keyword. |
| **<a id="__svg__SVGTextContentElement__LENGTHADJUST_SPACINGANDGLYPHS"></a>LENGTHADJUST_SPACINGANDGLYPHS** | The spacingAndGlyphs keyword. |
| **<a id="__svg__SVGTextContentElement__LENGTHADJUST_UNKNOWN"></a>LENGTHADJUST_UNKNOWN** | Some other value. |

The **<a id="__svg__SVGTextContentElement__textLength"></a>textLength** IDL attribute [reflects](types.md#TermReflect) the ‘[textLength](#TextElementTextLengthAttribute)’ content attribute.

The initial value of the ‘[textLength](#TextElementTextLengthAttribute)’ content attribute is defined to equal the user-agent's calculated length for the element. In other words, when the content attribute is not present, the IDL property is initialized with a base length equal to the return-value of [getComputedTextLength](#__svg__SVGTextContentElement__getComputedTextLength) on the same element, as a length in user units.

The **<a id="__svg__SVGTextContentElement__lengthAdjust"></a>lengthAdjust** IDL attribute [reflects](types.md#TermReflect) the ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’ content attribute. The [numeric type values](types.md#TermNumericTypeValue) for ‘[lengthAdjust](#TextElementLengthAdjustAttribute)’ are as described above in the numeric length adjust type constant table.

The **<a id="__svg__SVGTextContentElement__getNumberOfChars"></a>getNumberOfChars** method returns the total number of [addressable characters](#TermAddressableCharacter) available for rendering within the current element, regardless of whether they will be rendered. When getNumberOfChars() is called, the following steps are run:

1. Let *node* be the element or node upon which this method was called
2. If *node* is a DOM text node, return the length of the text content of *node*, after normalizing whitespace according to the value of the white-space property on its parent element.
3. If *node* is an Element:
  - If the element is [not rendered](render.md#Rendered-vs-NonRendered) (e.g., because the [display](render.md#VisibilityControl) property has the used value none), then return 0;
  - Otherwise, set *count* to 0, and for each child of *node*:
    - Recursively call this algorithm and add the returned value to *count*.
  Return *count*.
4. For all other node types (e.g., DOM comments), return 0.

The **<a id="__svg__SVGTextContentElement__getComputedTextLength"></a>getComputedTextLength** method is used to compute a "length" for the text within the element. When getComputedTextLength() is called, the following steps are run:

1. Let *count* be the value that would be returned if the [getNumberOfChars](#__svg__SVGTextContentElement__getNumberOfChars) method were called on this element.
2. Let *length* be the value that would be returned if the [getSubStringLength](#__svg__SVGTextContentElement__getSubStringLength) method were called on this element, passing 0 and *count* as arguments.
3. Return *length*.

The **<a id="__svg__SVGTextContentElement__getSubStringLength"></a>getSubStringLength** method is used to compute the formatted text advance distance for a substring of text within the element. When getSubStringLength(*charnum*, *nchars*) is called, the following steps are run:

1. Assign an index to each [addressable character](#TermAddressableCharacter) in the DOM within this element, where the first character has index 0.
2. If *charnum* is greater than the highest index assigned to a character or if *nchars* is negative, then throw an IndexSizeError.
3. Let *length* be a length in user units, initialized to 0.
4. For each [addressable character](#TermAddressableCharacter) in the DOM within this element that has an index such that *charnum* ≤ index < (*charnum* + *nchars*):
  1. If the character corresponds to a [typographic character](#TermTypographicCharacterUnit) and it is the first character in document order to correspond to that [typographic character](#TermTypographicCharacterUnit), then:
    1. Add the advance of the [typographic character](#TermTypographicCharacterUnit) to *length*, adjusted for any font kerning in effect.
    2. If the letter-spacing or word-spacing properties contributed space just after the [typographic character](#TermTypographicCharacterUnit), then add that space to *length*.
  This means that, for example, if there is a ligature that is only partly included in the substring, then the advance of the [typographic character](#TermTypographicCharacterUnit) and any subsequent letter-spacing or word-spacing space will be assigned to the first character's text length.
5. Return *length*.

Previous versions of SVG required that this method and [getComputedTextLength](#__svg__SVGTextContentElement__getComputedTextLength) also include positioning adjustments in the inline direction due to ‘[dx](#TextElementDXAttribute)’ or ‘[dy](#TextElementDYAttribute)’ on child elements, so that the returned value would be equivalent to the user agent's calculation for ‘[textLength](#TextElementTextLengthAttribute)’. However, it was poorly specified, poorly implemented, and of dubious benefit, so has been simplified to match implementations.

To *<a id="TermFindTypographicCharacterForCharacter"></a>find the typographic character for a character* at index *index* within an element *element*, the following steps are run:

1. Assign an index to each [addressable character](#TermAddressableCharacter) in the DOM within this element, where the first character has index 0.
2. Let *last* be the highest index assigned to a character.
3. While *charnum* < *last* and the character at index *charnum* does not correspond to a [typographic character](#TermTypographicCharacterUnit):
  1. Set *charnum* to *charnum* + 1.
4. If *charnum* is greater than the highest index assigned to a character or, then return null.
5. Otherwise, return the [typographic character](#TermTypographicCharacterUnit) that corresponds to *charnum*.

The **<a id="__svg__SVGTextContentElement__getStartPositionOfChar"></a>getStartPositionOfChar** method is used to get the position of a [typographic character](#TermTypographicCharacterUnit) after text layout has been performed. When getStartPositionOfChar(*charnum*) is called, the following steps are run:

1. Let *cluster* be the result of [finding the typographic character for the character](#TermFindTypographicCharacterForCharacter) at index *charnum* within the current element.
2. If *cluster* is null, then throw an IndexSizeError.
3. Let *p* be the [alignment point](#TermAlignmentPoint) of the [typographic character](#TermTypographicCharacterUnit) that correspond to the character at index *charnum*, in the coordinate system of the current element.
4. Return a newly created, [detached](shapes.md#PointMode) DOMPoint object representing the point *p*.

The **<a id="__svg__SVGTextContentElement__getEndPositionOfChar"></a>getEndPositionOfChar** method is used to get the trailing position of a [typographic character](#TermTypographicCharacterUnit) after text layout has been performed. When getEndPositionOfChar(*charnum*) is called, the following steps are run:

1. Let *cluster* be the result of [finding the typographic character for the character](#TermFindTypographicCharacterForCharacter) at index *charnum* within the current element.
2. If *cluster* is null, then throw an IndexSizeError.
3. Let *p* be the [alignment point](#TermAlignmentPoint) of *cluster* that correspond to the character at index *charnum*, in the coordinate system of the current element.
4. Let *direction* be a unit vector in the direction of the *cluster*'s advance. This direction takes into account the writing mode being used, the direction of the character, the text-orientation, [glyph-orientation-horizontal](#GlyphOrientationHorizontalProperty) and [glyph-orientation-vertical](#GlyphOrientationVerticalProperty) properties, any ‘[rotate](#TextElementRotateAttribute)’ value that applies to *cluster*, and any rotation applied to due a ‘[textPath](#TextPathElement)’.
5. Let *advance* be *cluster*'s advance.
6. Set *p* to *p* + *advance* · *direction*.
7. Return a newly created, [detached](shapes.md#PointMode) DOMPoint object representing the point *p*.

The **<a id="__svg__SVGTextContentElement__getExtentOfChar"></a>getExtentOfChar** method is used to compute a tight bounding box of the glyph cell that corresponds to a given [typographic character](#TermTypographicCharacterUnit). When getExtentOfChar(*charnum*) is called, the following steps are run:

1. Let *cluster* be the result of [finding the typographic character for the character](#TermFindTypographicCharacterForCharacter) at index *charnum* within the current element.
2. If *cluster* is null, then throw an IndexSizeError.
3. Let *quad* be the potentially rotated rectangle in the current element's coordinate system that is the glyph cell for *cluster*.
4. Let *rect* be the rectangle that forms the tightest bounding box around *quad* in the current element's coordinate system.
5. Return a newly created DOMRect object representing the rectangle *rect*.

The **<a id="__svg__SVGTextContentElement__getRotationOfChar"></a>getRotationOfChar** method is used to get the rotation of [typographic character](#TermTypographicCharacterUnit). When getRotationOfChar(*charnum*) is called, the following steps are run:

1. Let *cluster* be the result of [finding the typographic character for the character](#TermFindTypographicCharacterForCharacter) at index *charnum* within the current element.
2. If *cluster* is null, then throw an IndexSizeError.
3. Let *direction* be the angle in degrees that represents the direction of the *cluster*'s advance. This direction takes into account the writing mode being used, the direction of the character, the text-orientation, [glyph-orientation-horizontal](#GlyphOrientationHorizontalProperty) and [glyph-orientation-vertical](#GlyphOrientationVerticalProperty) properties, any ‘[rotate](#TextElementRotateAttribute)’ value that applies to *cluster*, and any rotation applied to due a ‘[textPath](#TextPathElement)’.
4. Return *direction*.

The **<a id="__svg__SVGTextContentElement__getCharNumAtPosition"></a>getCharNumAtPosition** method is used to find which character caused a text glyph to be rendered at a given position in the coordinate system. Because the relationship between characters and glyphs is not one-to-one, only the first character of the relevant [typographic character](#TermTypographicCharacterUnit) is returned When getCharNumAtPosition(*point*) is called, the following steps are run:

1. Assign an index to each character in the DOM within this element, where the first character has index 0.
2. Let *last* be the highest index assigned to a character.
3. Let *charnum* be 0.
4. Let *result* be -1.
5. While *charnum* < *last*:
  1. If the character at index *charnum* corresponds to a [typographic character](#TermTypographicCharacterUnit) and it is the first character in document order to correspond to that [typographic character](#TermTypographicCharacterUnit), and *point* in this element's coordinate system is within the glyph cell for the [typographic character](#TermTypographicCharacterUnit), then set *result* to *charnum*.
6. Return *result*.

The **<a id="__svg__SVGTextContentElement__selectSubString"></a>selectSubString** method is used to select text within the element. When selectSubString(*charnum*, *nchars*) is called, the following steps are run:

Selects a substring of the text in this element, beginning at character index *charnum* and extending forwards *nchars* characters. The following steps must be followed when this method is called:

1. Let *node* be this text content element.
2. Let *count* be the number of characters in this text content element.
3. Let *end* = *charnum* + *nchars*.
4. If *charnum* ≥ *count* or *end* ≥ *count*, then throw an IndexSizeError.
5. Remove all ranges from the document's selection. [[DOM](refs.md#ref-dom)][[EDITING](refs.md#ref-editing)]
6. Set the selection's direction to forwards.
7. Add to the selection a new range whose start is the boundary point tuple (*node*, *charnum*) and end is the boundary point tuple (*node*, *end*).

Ignoring the argument checking and exception throwing, this is equivalent to performing the following:

```
var selection = document.getSelection();
selection.removeAllRanges();
var range = new Range();
range.setStart(textContentElement, charnum);
range.setEnd(textContentElement, charnum + nchars);
selection.addRange(range);
```

This method is deprecated, as it duplicates functionality from the Selection API.

### <a id="InterfaceSVGTextPositioningElement"></a>11.13.2. Interface SVGTextPositioningElement

The [SVGTextPositioningElement](#InterfaceSVGTextPositioningElement) interface is implemented by elements that support attributes that position individual text glyphs. It is inherited by [SVGTextElement](#InterfaceSVGTextElement) and [SVGTSpanElement](#InterfaceSVGTSpanElement).

```
[Exposed=Window]
interface SVGTextPositioningElement : SVGTextContentElement {
  [SameObject] readonly attribute SVGAnimatedLengthList x;
  [SameObject] readonly attribute SVGAnimatedLengthList y;
  [SameObject] readonly attribute SVGAnimatedLengthList dx;
  [SameObject] readonly attribute SVGAnimatedLengthList dy;
  [SameObject] readonly attribute SVGAnimatedNumberList rotate;
};
```

The **<a id="__svg__SVGTextPositioningElement__x"></a>x**, **<a id="__svg__SVGTextPositioningElement__y"></a>y**, **<a id="__svg__SVGTextPositioningElement__dx"></a>dx**, **<a id="__svg__SVGTextPositioningElement__dy"></a>dy** and **<a id="__svg__SVGTextPositioningElement__rotate"></a>rotate** IDL attributes [reflect](types.md#TermReflect) the ‘[x](#TextElementXAttribute)’, ‘[y](#TextElementYAttribute)’, ‘[dx](#TextElementDXAttribute)’, ‘[dy](#TextElementDYAttribute)’ and ‘[rotate](#TextElementRotateAttribute)’ content attributes, respectively.

### <a id="InterfaceSVGTextElement"></a>11.13.3. Interface SVGTextElement

An [SVGTextElement](#InterfaceSVGTextElement) object represents a ‘[text](#TextElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGTextElement : SVGTextPositioningElement {
};
```

### <a id="InterfaceSVGTSpanElement"></a>11.13.4. Interface SVGTSpanElement

An [SVGTSpanElement](#InterfaceSVGTSpanElement) object represents a ‘[tspan](#TextElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGTSpanElement : SVGTextPositioningElement {
};
```

### <a id="InterfaceSVGTextPathElement"></a>11.13.5. Interface SVGTextPathElement

An [SVGTextPathElement](#InterfaceSVGTextPathElement) object represents a ‘[textPath](#TextPathElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGTextPathElement : SVGTextContentElement {

  // textPath Method Types
  const unsigned short TEXTPATH_METHODTYPE_UNKNOWN = 0;
  const unsigned short TEXTPATH_METHODTYPE_ALIGN = 1;
  const unsigned short TEXTPATH_METHODTYPE_STRETCH = 2;

  // textPath Spacing Types
  const unsigned short TEXTPATH_SPACINGTYPE_UNKNOWN = 0;
  const unsigned short TEXTPATH_SPACINGTYPE_AUTO = 1;
  const unsigned short TEXTPATH_SPACINGTYPE_EXACT = 2;

  [SameObject] readonly attribute SVGAnimatedLength startOffset;
  [SameObject] readonly attribute SVGAnimatedEnumeration method;
  [SameObject] readonly attribute SVGAnimatedEnumeration spacing;
};

SVGTextPathElement includes SVGURIReference;
```

The numeric method type constants defined on [SVGTextPathElement](#InterfaceSVGTextPathElement) are used to represent the keyword values that the ‘[method](#TextPathElementMethodAttribute)’ attribute can take. Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGTextPathElement__TEXTPATH_METHODTYPE_ALIGN"></a>TEXTPATH_METHODTYPE_ALIGN** | The align keyword. |
| **<a id="__svg__SVGTextPathElement__TEXTPATH_METHODTYPE_STRETCH"></a>TEXTPATH_METHODTYPE_STRETCH** | The stretch keyword. |
| **<a id="__svg__SVGTextPathElement__TEXTPATH_METHODTYPE_UNKNOWN"></a>TEXTPATH_METHODTYPE_UNKNOWN** | Some other value. |

The numeric spacing type constants defined on [SVGTextPathElement](#InterfaceSVGTextPathElement) are used to represent the keyword values that the ‘[spacing](#TextPathElementSpacingAttribute)’ attribute can take. Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGTextPathElement__TEXTPATH_SPACINGTYPE_AUTO"></a>TEXTPATH_SPACINGTYPE_AUTO** | The auto keyword. |
| **<a id="__svg__SVGTextPathElement__TEXTPATH_SPACINGTYPE_EXACT"></a>TEXTPATH_SPACINGTYPE_EXACT** | The exact keyword. |
| **<a id="__svg__SVGTextPathElement__TEXTPATH_SPACINGTYPE_UNKNOWN"></a>TEXTPATH_SPACINGTYPE_UNKNOWN** | Some other value. |

The **<a id="__svg__SVGTextPathElement__startOffset"></a>startOffset** IDL attribute [reflects](types.md#TermReflect) the ‘[startOffset](#TextPathElementStartOffsetAttribute)’ content attribute.

The **<a id="__svg__SVGTextPathElement__method"></a>method** IDL attribute [reflects](types.md#TermReflect) the ‘[method](#TextPathElementMethodAttribute)’ content attribute. The [numeric type values](types.md#TermNumericTypeValue) for ‘[method](#TextPathElementMethodAttribute)’ are as described above in the numeric method type constant table.

The **<a id="__svg__SVGTextPathElement__spacing"></a>spacing** IDL attribute [reflects](types.md#TermReflect) the ‘[spacing](#TextPathElementSpacingAttribute)’ content attribute. The [numeric type values](types.md#TermNumericTypeValue) for ‘[spacing](#TextPathElementSpacingAttribute)’ are as described above in the numeric spacing type constant table.
