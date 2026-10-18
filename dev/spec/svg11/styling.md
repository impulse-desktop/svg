<!-- https://www.w3.org/TR/SVG11/styling.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](struct.md) ⋅ [Next](coords.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 6 Styling

## <a id="SVGStylingProperties"></a>6.1 SVG's styling properties

SVG uses styling properties to describe many of its document parameters. Styling properties define how the graphics elements in the SVG content are to be rendered. SVG uses styling properties for the following:

- Parameters which are clearly visual in nature and thus lend themselves to styling. Examples include all attributes that define how an object is "painted," such as fill and stroke colors, linewidths and dash styles.
- Parameters having to do with text styling such as font family and size.
- Parameters which impact the way that graphical elements are rendered, such as specifying clipping paths, masks, arrowheads, markers and filter effects.

SVG shares many of its styling properties with CSS [[CSS2](refs.md#ref-CSS2)] and XSL [[XSL](refs.md#ref-XSL)]. Except for any additional SVG-specific rules explicitly mentioned in this specification, the normative definition of properties that are shared with CSS and XSL is the definition of the property from the CSS2 specification [[CSS2](refs.md#ref-CSS2)].

<a id="PropertiesFromCSS2"></a>The following properties are shared between CSS2 and SVG. Most of these properties are also defined in XSL:

- [Font properties](text.md#FontPropertiesUsedBySVG):
  - [‘font’](text.md#FontProperty)
  - [‘font-family’](text.md#FontFamilyProperty)
  - [‘font-size’](text.md#FontSizeProperty)
  - [‘font-size-adjust’](text.md#FontSizeAdjustProperty)
  - [‘font-stretch’](text.md#FontStretchProperty)
  - [‘font-style’](text.md#FontStyleProperty)
  - [‘font-variant’](text.md#FontVariantProperty)
  - [‘font-weight’](text.md#FontWeightProperty)
- Text properties:
  - [‘direction’](text.md#DirectionProperty)
  - [‘letter-spacing’](text.md#LetterSpacingProperty)
  - [‘text-decoration’](text.md#TextDecorationProperty)
  - [‘unicode-bidi’](text.md#UnicodeBidiProperty)
  - [‘word-spacing’](text.md#WordSpacingProperty)
- Other properties for visual media:
  - [‘clip’](masking.md#ClipProperty), only applicable to [outermost svg element](intro.md#TermOutermostSVGElement).
  - [‘color’](color.md#ColorProperty), used to provide a potential indirect value (currentColor) for the [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘flood-color’](filters.md#FloodColorProperty) and [‘lighting-color’](filters.md#LightingColorProperty) properties. (The SVG properties which support color allow a color specification which is extended from CSS2 to accommodate color definitions in arbitrary color spaces. See [Color profile descriptions](color.md#ColorProfileDescriptions).)
  - [‘cursor’](interact.md#CursorProperty)
  - [‘display’](painting.md#DisplayProperty)
  - [‘overflow’](masking.md#OverflowProperty), only applicable to [elements which establish a new viewport](coords.md#ElementsThatEstablishViewports).
  - [‘visibility’](painting.md#VisibilityProperty)

The following SVG properties are not defined in CSS2. The complete normative definitions for these properties are found in this specification:

- [Clipping, Masking and Compositing](masking.md) properties:
  - [‘clip-path’](masking.md#ClipPathProperty)
  - [‘clip-rule’](masking.md#ClipRuleProperty)
  - [‘mask’](masking.md#MaskProperty)
  - [‘opacity’](masking.md#OpacityProperty)
- [Filter Effects](filters.md) properties:
  - [‘enable-background’](filters.md#EnableBackgroundProperty)
  - [‘filter’](filters.md#FilterProperty)
  - [‘flood-color’](filters.md#FloodColorProperty)
  - [‘flood-opacity’](filters.md#FloodOpacityProperty)
  - [‘lighting-color’](filters.md#LightingColorProperty)
- [Gradient](pservers.md#Gradients) properties:
  - [‘stop-color’](pservers.md#StopColorProperty)
  - [‘stop-opacity’](pservers.md#StopOpacityProperty)
- [Interactivity](interact.md) properties:
  - [‘pointer-events’](interact.md#PointerEventsProperty)
- [Color](color.md) and [Painting](painting.md) properties:
  - [‘color-interpolation’](painting.md#ColorInterpolationProperty)
  - [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty)
  - [‘color-profile’](color.md#ColorProfileProperty)
  - [‘color-rendering’](painting.md#ColorRenderingProperty)
  - [‘fill’](painting.md#FillProperty)
  - [‘fill-opacity’](painting.md#FillOpacityProperty)
  - [‘fill-rule’](painting.md#FillRuleProperty)
  - [‘image-rendering’](painting.md#ImageRenderingProperty)
  - [‘marker’](painting.md#MarkerProperty)
  - [‘marker-end’](painting.md#MarkerEndProperty)
  - [‘marker-mid’](painting.md#MarkerMidProperty)
  - [‘marker-start’](painting.md#MarkerStartProperty)
  - [‘shape-rendering’](painting.md#ShapeRenderingProperty)
  - [‘stroke’](painting.md#StrokeProperty)
  - [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty)
  - [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty)
  - [‘stroke-linecap’](painting.md#StrokeLinecapProperty)
  - [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty)
  - [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty)
  - [‘stroke-opacity’](painting.md#StrokeOpacityProperty)
  - [‘stroke-width’](painting.md#StrokeWidthProperty)
  - [‘text-rendering’](painting.md#TextRenderingProperty)
- [Text](text.md) properties:
  - [‘alignment-baseline’](text.md#AlignmentBaselineProperty)
  - [‘baseline-shift’](text.md#BaselineShiftProperty)
  - [‘dominant-baseline’](text.md#DominantBaselineProperty)
  - [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty)
  - [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty)
  - [‘kerning’](text.md#KerningProperty)
  - [‘text-anchor’](text.md#TextAnchorProperty)
  - [‘writing-mode’](text.md#WritingModeProperty)

A table that lists and summarizes the styling properties can be found in the [Property Index](propidx.md).

## <a id="StylingScenarios"></a>6.2 Usage scenarios for styling

SVG has many usage scenarios, each with different needs. Here are three common usage scenarios:

1.
**SVG content used as an exchange format (style sheet language-independent)**:
In some usage scenarios, reliable interoperability of SVG content across software tools is the main goal. Since support for a particular style sheet language is not guaranteed across all implementations, it is a requirement that SVG content can be fully specified without the use of a style sheet language.
2.
**SVG content generated as the output from XSLT**:
XSLT offers the ability to take a stream of arbitrary XML content as input, apply potentially complex transformations, and then generate SVG content as output [[XSLT](refs.md#ref-XSLT)]. XSLT can be used to transform XML data extracted from databases into an SVG graphical representation of that data. It is a requirement that fully specified SVG content can be generated from XSLT.
3.
**SVG content styled with CSS**:
CSS is a widely implemented declarative language for assigning styling properties to XML content, including SVG [[CSS2](refs.md#ref-CSS2)]. It represents a combination of features, simplicity and compactness that makes it very suitable for many applications of SVG. It is a requirement that CSS styling can be applied to SVG content.

## <a id="AlternativesForStyling"></a>6.3 Alternative ways to specify styling properties

Styling properties can be assigned to SVG elements in the following two ways:

- <a id="StylingUsingPresentationAttributes"></a>
**Presentation attributes**
Styling properties can be assigned using SVG's presentation attributes. For each styling property defined in this specification, there is a corresponding XML presentation attribute available on all relevant SVG elements. Detailed information on the presentation attributes can be found in [Specifying properties using the presentation attributes](#UsingPresentationAttributes).
The presentation attributes are style sheet language independent and thus are applicable to usage scenario 1 above (i.e., tool interoperability). Because it is straightforward to assign values to XML attributes from XSLT, the presentation attributes are well-suited to usage scenario 2 above (i.e., SVG generation from XSLT). (See [Styling with XSL](#StylingWithXSL) below.)
[Conforming SVG Interpreters](conform.md#ConformingSVGInterpreters) and [Conforming SVG Viewers](conform.md#ConformingSVGViewers) are required to support SVG's presentation attributes.
- <a id="StylingUsingCSSStylesheets"></a>
**CSS Stylesheets**
To support usage scenario 3 above, SVG content can be styled with CSS. For more information, see [Styling with CSS](#StylingWithCSS).
[Conforming SVG Interpreters](conform.md#ConformingSVGInterpreters) and [Conforming SVG Viewers](conform.md#ConformingSVGViewers) that support CSS styling of generic (i.e., text-based) XML content are required to also support CSS styling of SVG content.

## <a id="UsingPresentationAttributes"></a>6.4 Specifying properties using the presentation attributes

For each styling property defined in this specification (see [Property Index](propidx.md)), there is a corresponding XML attribute (the presentation attribute) with the same name that is available on all relevant SVG elements. For example, SVG has a [‘fill’](painting.md#FillProperty) property that defines how to paint the interior of a shape. There is a corresponding presentation attribute with the same name (i.e., ‘fill’) that can be used to specify a value for the [‘fill’](painting.md#FillProperty) property on a given element.

The following example shows how the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties can be specified on a [‘rect’](shapes.md#RectElement) using the ‘fill’ and ‘stroke’ presentation attributes. The rectangle will be filled with red and outlined with blue:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" version="1.1"
     width="10cm" height="5cm" viewBox="0 0 1000 500">
  <rect x="200" y="100" width="600" height="300" 
        fill="red" stroke="blue" stroke-width="3"/>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/styling/PresentationAttributes.svg)

The presentation attributes offer the following advantages:

- **Broad support**. All versions of [Conforming SVG Interpreters](conform.md#ConformingSVGInterpreters) and [Conforming SVG Viewers](conform.md#ConformingSVGViewers) are required to support the presentation attributes.
- **Simplicity**. Styling properties can be attached to elements by simply providing a value for the presentation attribute on the proper elements.
- **Restyling**. SVG content that uses the presentation attributes is highly compatible with downstream processing using XSLT [[XSLT](refs.md#ref-XSLT)] [[XSLT2](refs.md#ref-XSLT2)] or supplemental styling by adding CSS style rules to override some of the presentation attributes.
- **Convenient generation using XSLT**. In some cases, XSLT can be used to generate fully styled SVG content. The presentation attributes are compatible with convenient generation of SVG from XSLT.

In some situations, SVG content that uses the presentation attributes has potential limitations versus SVG content that is styled with a style sheet language such as CSS (see [Styling with CSS](#StylingWithCSS)). In other situations, such as when an XSLT style sheet generates SVG content from semantically rich XML source files, the limitations below may not apply. Depending on the situation, some of the following potential limitations may or may not apply to the presentation attributes:

- **Styling attached to content**. The presentation attributes are attached directly to particular elements, thereby diminishing potential advantages that comes from abstracting styling from content, such as the ability to restyle documents for different uses and environments.
- **Flattened data model**. In and of themselves, the presentation attributes do not offer the higher level abstractions that you get with a styling system, such as the ability to define named collections of properties which are applied to particular categories of elements. The result is that, in many cases, important higher level semantic information can be lost, potentially making document reuse and restyling more difficult.
- **Potential increase in file size**. Many types of graphics use similar styling properties across multiple elements. For example, a company organization chart might assign one collection of styling properties to the boxes around temporary workers (e.g., dashed outlines, red fill), and a different collection of styling properties to permanent workers (e.g., solid outlines, blue fill). Styling systems such as CSS allow collections of properties to be defined once in a file. With the styling attributes, it might be necessary to specify presentation attributes on each different element.
- **Potential difficulty when embedded into a CSS-styled parent document**. When SVG content is embedded in other XML, and the desire is to style all aspects of the compound document with CSS, use of the presentation attributes might introduce complexity and difficulty. In this case, it is sometimes easier if the SVG content does not use the presentation attributes and instead is styled using CSS facilities.

For user agents that support CSS, the presentation attributes must be translated to corresponding CSS style rules according to rules described in Precedence of non-CSS presentational hints ([[CSS2](refs.md#ref-CSS2)], section 6.4.4), with the additional clarification that the presentation attributes are conceptually inserted into a new author style sheet which is the first in the author style sheet collection. The presentation attributes thus will participate in the CSS2 cascade as if they were replaced by corresponding CSS style rules placed at the start of the author style sheet with a specificity of zero. In general, this means that the presentation attributes have lower priority than other CSS style rules specified in author style sheets or [‘style’](#StyleAttribute) attributes.

User agents that do not support CSS must ignore any CSS style rules defined in CSS style sheets and [‘style’](#StyleAttribute) attributes. In this case, the CSS cascade does not apply. (Inheritance of properties, however, does apply. See [Property inheritance](#Inheritance).)

An !important declaration ([[CSS2](refs.md#ref-CSS2)], section 6.4.2) within a presentation attribute definition is an invalid value.

Animation of presentation attributes is equivalent to animating the corresponding property. Thus, the same effect occurs from animating the presentation attribute with attributeType="XML" as occurs with animating the corresponding property with attributeType="CSS" (see [‘attributeType’](animate.md#AttributeTypeAttribute)).

## <a id="StylingWithXSL"></a>6.5 Styling with XSL

XSL style sheets [[XSLT](refs.md#ref-XSLT)] [[XSLT2](refs.md#ref-XSLT2)] define how to transform XML content into something else, usually other XML. When XSLT is used in conjunction with SVG, sometimes SVG content will serve as both input and output for XSL style sheets. Other times, XSL style sheets will take non-SVG content as input and generate SVG content as output.

The following example uses an external XSL style sheet to transform SVG content into modified SVG content (see [Referencing external style sheets](#ReferencingExternalStyleSheets)). The style sheet sets the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties on all rectangles to red and blue, respectively:

```
mystyle.xsl
<?xml version="1.0" standalone="no"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:svg="http://www.w3.org/2000/svg">
  <xsl:output
    method="xml"
    encoding="utf-8"
    doctype-public="-//W3C//DTD SVG 1.1//EN"
    doctype-system="http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd"/>
  <!-- Add version to topmost 'svg' element -->
  <xsl:template match="/svg:svg">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:attribute name="version">1.1</xsl:attribute>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>
  <!-- Add styling to all 'rect' elements -->
  <xsl:template match="svg:rect">
    <xsl:copy>
      <xsl:copy-of select="@*"/>
      <xsl:attribute name="fill">red</xsl:attribute>
      <xsl:attribute name="stroke">blue</xsl:attribute>
      <xsl:attribute name="stroke-width">3</xsl:attribute>
    </xsl:copy>
  </xsl:template>
</xsl:stylesheet>

SVG file to be transformed by mystyle.xsl
<?xml version="1.0" standalone="no"?>
<?xml-stylesheet href="mystyle.xsl" type="application/xml"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width="10cm" height="5cm">
  <rect x="2cm" y="1cm" width="6cm" height="3cm"/>
</svg>

SVG content after applying mystyle.xsl
<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
      "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg"
     width="10cm" height="5cm" version="1.1">
  <rect x="2cm" y="1cm" width="6cm" height="3cm" fill="red" stroke="blue" stroke-width="3"/>
</svg>
```

## <a id="StylingWithCSS"></a>6.6 Styling with CSS

SVG implementations that support CSS are required to support the following:

- External CSS style sheets referenced from the current document (see [Referencing external style sheets](#ReferencingExternalStyleSheets))
- Internal CSS style sheets (i.e., style sheets embedded within the current document, such as within an SVG [‘style’](#StyleElement) element)
- Inline style (i.e., CSS property declarations within a [‘style’](#StyleAttribute) attribute on a particular SVG element)

The following example shows the use of an external CSS style sheet to set the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties on all rectangles to red and blue, respectively:

```
mystyle.css
rect {
  fill: red;
  stroke: blue;
  stroke-width: 3
}

SVG file referencing mystyle.css
<?xml version="1.0" standalone="no"?>
<?xml-stylesheet href="mystyle.css" type="text/css"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" version="1.1"
     width="10cm" height="5cm" viewBox="0 0 1000 500">
  <rect x="200" y="100" width="600" height="300"/>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/styling/ExternalCSSStyleSheet.svg)

<a id="StyleElementExample"></a>CSS style sheets can be embedded within SVG content inside of a [‘style’](#StyleElement) element. The following example uses an internal CSS style sheet to achieve the same result as the previous example:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" version="1.1"
     width="10cm" height="5cm" viewBox="0 0 1000 500">
  <defs>
    <style type="text/css"><![CDATA[
      rect {
        fill: red;
        stroke: blue;
        stroke-width: 3
      }
    ]]></style>
  </defs>
  <rect x="200" y="100" width="600" height="300"/>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/styling/InternalCSSStyleSheet.svg)

Note how the CSS style sheet is placed within a `CDATA` construct (i.e., `<![CDATA[ ... ]]>`). Placing internal CSS style sheets within `CDATA` blocks is sometimes necessary since CSS style sheets can include characters, such as ">", which conflict with XML parsers. Even if a given style sheet does not use characters that conflict with XML parsing, it is highly recommended that internal style sheets be placed inside `CDATA` blocks.

Implementations that support CSS are also required to support CSS inline style. Similar to the ‘style’ attribute in HTML, CSS inline style can be declared within a [‘style’](#StyleAttribute) attribute in SVG by specifying a semicolon-separated list of property declarations, where each property declaration has the form "name: value". Note that property declarations inside the [‘style’](#StyleAttribute) attribute must follow CSS style rules, see [The 'style' attribute](#StyleAttribute).

<a id="StyleAttributeExample"></a>The following example shows how the [‘fill’](painting.md#FillProperty) and [‘stroke’](painting.md#StrokeProperty) properties can be specified on a [‘rect’](shapes.md#RectElement) using the [‘style’](#StyleAttribute) attribute. Just like the previous example, the rectangle will be filled with red and outlined with blue:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg xmlns="http://www.w3.org/2000/svg" version="1.1"
     width="10cm" height="5cm" viewBox="0 0 1000 500">
  <rect x="200" y="100" width="600" height="300" 
        style="fill: red; stroke: blue; stroke-width: 3"/>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/styling/StyleAttribute.svg)

In an SVG user agent that supports CSS style sheets, the following facilities from CSS2 must be supported:

- CSS2 selectors within style sheets ([[CSS2](refs.md#ref-CSS2)], chapter 5). Because SVG is intended to be used as one component in a multiple namespace XML application and CSS2 is not namespace aware, type selectors will only match against the local part of the element's qualified name.
- External CSS style sheets [[XML-SS](refs.md#ref-XML-SS)], CSS style sheets within [‘style’](#StyleElement) elements and CSS declaration blocks ([[CSS2](refs.md#ref-CSS2)], section 4.1.7) within [‘style’](#StyleAttribute) attributes attached to specific SVG elements.
- CSS2 rules for assigning property values, cascading and inheritance ([[CSS2](refs.md#ref-CSS2)], chapter 6).
- @font-face, @media, @import and @charset rules within style sheets ([[CSS2](refs.md#ref-CSS2)], sections 15.3.1, 7.2.1, 6.3 and 4.4).
- CSS2's dynamic pseudo-classes :hover, :active and :focus and pseudo-classes :first-child, :visited, :link and :lang ([[CSS2](refs.md#ref-CSS2)], section 5.11). The remaining CSS2 pseudo-classes, including those having to do with generated content ([[CSS2](refs.md#ref-CSS2)], chapter 12), are not part of the SVG language definition. An SVG element gains focus when it is selected. See [Text selection](text.md#TextSelection).
- For the purposes of aural media, SVG represents a CSS-stylable XML grammar. In user agents that support aural style sheets, CSS aural style properties can be applied as defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], chapter 19). (See [Aural style sheets](#AuralStyleSheets).)
- CSS style sheets defined within a [‘style’](#StyleElement) element can be immediate character data content of the [‘style’](#StyleElement) element or can be embedded within a CDATA section ([[XML10](refs.md#ref-XML10)], section 2.7).

SVG defines an [@color-profile](color.md#ColorProfileAtRule) at-rule ([[CSS2](refs.md#ref-CSS2)], section 4.1.6) for defining color profiles so that ICC color profiles can be applied to CSS-styled SVG content.

Note the following about relative URIs and external CSS style sheets: The CSS2 specification says ([[CSS2](refs.md#ref-CSS2)], section 4.3.4) that relative URIs (as defined in Uniform Resource Identifiers (URI): Generic Syntax [[RFC3986](refs.md#ref-RFC3986)]) within style sheets are resolved such that the base URI is that of the style sheet, not that of the referencing document.

## <a id="CaseSensitivity"></a>6.7 Case sensitivity of property names and values

Property declarations via [presentation attributes](intro.md#TermPresentationAttribute) are expressed in XML [XML10], which is case-sensitive. CSS property declarations specified either in CSS style sheets or in a [‘style’](#StyleAttribute) attribute, on the other hand, are generally case-insensitive with some exceptions ([[CSS2](refs.md#ref-CSS2)], section 4.1.3).

Because presentation attributes are expressed as XML attributes, presentation attributes are case-sensitive and must match the exact name as specified in the [DTD](https://www.w3.org/TR/SVG11/svgdtd.html) (see the `SVG.Presentation.attrib` entity in the DTD, which expands to all of the presentation attributes). When using a presentation attribute to specify a value for the [‘fill’](painting.md#FillProperty) property, the presentation attribute must be be specified as fill="…" and not fill="…" or Fill="…". Keyword values, such as italic in font-style="italic", are also case-sensitive and must be specified using the exact case used in the specification which defines the given keyword. For example, the keyword sRGB must have lowercase "s" and uppercase "RGB".

Property declarations within CSS style sheets or in a [‘style’](#StyleAttribute) attribute must only conform to CSS rules, which are generally more lenient with regard to case sensitivity. However, to promote consistency across the different ways for expressing styling properties, it is strongly recommended that authors use the exact property names (usually, lowercase letters and hyphens) as defined in the relevant specification and express all keywords using the same case as is required by presentation attributes and not take advantage of CSS's ability to ignore case.

## <a id="SVGUseOfCSS"></a>6.8 Facilities from CSS and XSL used by SVG

SVG shares various relevant properties and approaches common to CSS and XSL, plus the semantics of many of the processing rules.

SVG shares the following facilities with CSS and XSL:

- Shared properties. Many of SVG's properties are shared between CSS2, XSL and SVG. (See [list of shared properties](#PropertiesFromCSS2)).
- Syntax rules. (The normative references are CSS2 syntax and basic data types and The grammar of CSS2; in [[CSS2](refs.md#ref-CSS2)], chapter 4 and appendix D.)
- Allowable data types. (The normative reference is CSS2 syntax and basic data types ([[CSS2](refs.md#ref-CSS2)], chapter 4), with the exception that SVG length and angle values without a unit identifier. See [Units](coords.md#Units).)
- [Inheritance rules](#Inheritance).
- The color keywords from CSS2 that correspond to the colors used by objects in the user's environment. (The normative reference is CSS2 system colors; in [[CSS2](refs.md#ref-CSS2)], section 18.2.)
- For implementations that support CSS styling of SVG content, then that styling must be compatible with various other rules in CSS. (See [Styling with CSS](#StylingWithCSS).)

## <a id="ReferencingExternalStyleSheets"></a>6.9 Referencing external style sheets

External style sheets are referenced using the mechanism documented in Associating Style Sheets with XML documents Version 1.0 [[XML-SS](refs.md#ref-XML-SS)].

## <a id="StyleElement"></a>6.10 The ‘style’ element

The [‘style’](#StyleElement) element allows style sheets to be embedded directly within SVG content. SVG's [‘style’](#StyleElement) element has the same attributes as the corresponding element in HTML (see HTML's ‘style’ element).

‘style’

**Categories:**
: None
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [‘type’](#StyleElementTypeAttribute)
- [‘media’](#StyleElementMediaAttribute)
- [‘title’](#StyleElementTitleAttribute)

**DOM Interfaces:**
:

- [SVGStyleElement](#InterfaceSVGStyleElement)

Attribute definitions:

**<a id="StyleElementTypeAttribute"></a>type = *content-type***
: This attribute specifies the style sheet language of the element's contents. The style sheet language is specified as a content type (e.g., "text/css"), as per MIME Part Two: Media Types [[RFC2046](refs.md#ref-RFC2046)]. If a [‘type’](#StyleElementTypeAttribute) is not provided, the value of [‘contentStyleType’](#ContentStyleTypeAttribute) on the [‘svg’](struct.md#SVGElement) element shall be used, which in turn defaults to "text/css" [[RFC2046](refs.md#ref-RFC2046)]. If a [‘style’](#StyleElement) element falls outside of the [outermost svg element](intro.md#TermOutermostSVGElement) and the [‘type’](#StyleElementTypeAttribute) is not provided, the [‘type’](#StyleElementTypeAttribute) must default to "text/css" [[RFC2046](refs.md#ref-RFC2046)]. [Animatable](animate.md#Animatable): no.
**<a id="StyleElementMediaAttribute"></a>media = *media-descriptors***
: This attribute specifies the intended destination medium for style information. It may be a single media descriptor or a comma-separated list. The default value for this attribute is "all". The set of recognized *media-descriptors* are the list of media types recognized by CSS2 ([[CSS2](refs.md#ref-CSS2)], section 7.3). [Animatable](animate.md#Animatable): no.
**<a id="StyleElementTitleAttribute"></a>title = *advisory-title***
: (For compatibility with HTML 4 [[HTML4](refs.md#ref-HTML4)].) This attribute specifies an advisory title for the [‘style’](#StyleElement) element. [Animatable](animate.md#Animatable): no.
The syntax of style data depends on the style sheet language.

Some style sheet languages might allow a wider variety of rules in the [‘style’](#StyleElement) element than in the [‘style’](#StyleAttribute). For example, with CSS, rules can be declared within a [‘style’](#StyleElement) element that cannot be declared within a [‘style’](#StyleAttribute) attribute.

An example showing the [‘style’](#StyleElement) element is provided above (see [example](#StyleElementExample)).

## <a id="ClassAttribute"></a>6.11 The ‘class’ attribute

Attribute definitions:

**class = *list***
: This attribute assigns a class name or set of class names to an element. Any number of elements may be assigned the same class name or names. Multiple class names must be separated by white space characters. [Animatable](animate.md#Animatable): yes.
The [‘class’](#ClassAttribute) attribute assigns one or more class names to an element. The element may be said to belong to these classes. A class name may be shared by several element instances. The [‘class’](#ClassAttribute) attribute has several roles:

- As a style sheet selector (when an author wishes to assign style information to a set of elements).
- For general purpose processing by user agents.

In the following example, the [‘text’](text.md#TextElement) element is used in conjunction with the [‘class’](#ClassAttribute) attribute to markup document messages. Messages appear in both English and French versions.

```
<!-- English messages -->
<text class="info" lang="en">Variable declared twice</text>
<text class="warning" lang="en">Undeclared variable</text>
<text class="error" lang="en">Bad syntax for variable name</text>
<!-- French messages -->
<text class="info" lang="fr">Variable déclarée deux fois</text>
<text class="warning" lang="fr">Variable indéfinie</text>
<text class="error" lang="fr">Erreur de syntaxe pour variable</text>
```

In an SVG user agent that supports CSS styling, the following CSS style rules would tell visual user agents to display informational messages in green, warning messages in yellow, and error messages in red:

```
text.info    { color: green }
text.warning { color: yellow }
text.error   { color: red }
```

## <a id="StyleAttribute"></a>6.12 The ‘style’ attribute

The [‘style’](#StyleAttribute) attribute allows per-element style rules to be specified directly on a given element. When CSS styling is used, CSS inline style is specified by including semicolon-separated property declarations of the form "name : value" within the [‘style’](#StyleAttribute) attribute. Property declarations must follow CSS style rules thus CSS defined [properties](intro.md#TermProperty) (e.g. 'font-size') when having a <length> value must include a unit (for non-zero values). See [SVG's styling properties](#SVGStylingProperties) for a list of CSS defined properties.

Attribute definitions:

**style = *style***
: This attribute specifies style information for the current element. The style attribute specifies style information for a single element. The style sheet language of inline style rules is given by the value of attribute [‘contentStyleType’](#ContentStyleTypeAttribute) on the [‘svg’](struct.md#SVGElement) element. The syntax of style data depends on the style sheet language. [Animatable](animate.md#Animatable): no.
The style attribute may be used to apply a particular style to an individual SVG element. If the style will be reused for several elements, authors should use the [‘style’](#StyleElement) element to regroup that information. For optimal flexibility, authors should define styles in external style sheets.

An example showing the [‘style’](#StyleAttribute) attribute is provided above (see [example](#StyleAttributeExample)).

## <a id="DefaultStyleSheetLanguage"></a>6.13 Specifying the default style sheet language

The [‘contentStyleType’](#ContentStyleTypeAttribute) attribute on the [‘svg’](struct.md#SVGElement) element specifies the default style sheet language for the given document fragment.

**<a id="ContentStyleTypeAttribute"></a>contentStyleType = "*content-type*"**
: Identifies the default style sheet language for the given document. That language must then be used for all instances of style that do not specify their own style sheet language, such as the [‘style’](#StyleAttribute) attributes that are available on many elements. The value *content-type* specifies a media type, per MIME Part Two: Media Types [[RFC2046](refs.md#ref-RFC2046)]. The default value is "text/css" [[RFC2318](refs.md#ref-RFC2318)]. [Animatable](animate.md#Animatable): no.
Since the only widely deployed language used for inline styling (in style elements and style attributes) is CSS, and since that is already the default language if contentStyleType is omitted, in practice contentStyleType is not well supported in user agents. XSL style sheets are typically external. If a new style sheet language becomes popular, it might not use style attributes and could easily declare which language is in use with the type attribute on the style element.

The use of contentStyleType is therefore deprecated; new content should not use it. Future versions of the SVG specification may remove contentStyleType.

## <a id="Inheritance"></a>6.14 Property inheritance

Whether or not the user agent supports CSS, property inheritance in SVG follows the property inheritance rules defined in the CSS2 specification. The normative definition for property inheritance is the Inheritance section of the CSS2 specification ([[CSS2](refs.md#ref-CSS2)], section 6.2).

The definition of each property indicates whether the property can inherit the value of its parent.

In SVG, as in CSS2, most elements inherit computed values ([[CSS2](refs.md#ref-CSS2)], section 6.1.2). For cases where something other than computed values are inherited, the property definition will describe the inheritance rules. For specified values ([[CSS2](refs.md#ref-CSS2)], section 6.1.1) which are expressed in user units, in pixels (e.g., 20px) or in absolute values, the computed value equals the specified value. For specified values which use certain relative units (i.e., *em*, *ex* and percentages), the computed value will have the same units as the value to which it is relative. Thus, if the parent element has a [‘font-size’](text.md#FontSizeProperty) of 10pt and the current element has a [‘font-size’](text.md#FontSizeProperty) of 120%, then the computed value for [‘font-size’](text.md#FontSizeProperty) on the current element will be 12pt. In cases where the referenced value for relative units is not expressed in any of the standard SVG units (i.e., CSS units or user units), such as when a percentage is used relative to the current viewport or an object bounding box, then the computed value will be in user units.

Note that SVG has some facilities wherein a property which is specified on an ancestor element might effect its descendant element, even if the descendant element has a different assigned value for that property. For example, if a [‘clip-path’](masking.md#ClipPathProperty) property is specified on an ancestor element, and the current element has a [‘clip-path’](masking.md#ClipPathProperty) of none, the ancestor's clipping path still applies to the current element because the semantics of SVG state that the clipping path used on a given element is the intersection of all clipping paths specified on itself and all ancestor elements. The key concept is that property assignment (with possible property inheritance) happens first. After properties values have been assigned to the various elements, then the user agent applies the semantics of each assigned property, which might result in the property assignment of an ancestor element affecting the rendering of its descendants.

## <a id="Scope"></a>6.15 The scope/range of styles

The following define the scope/range of style sheets:

**Stand-alone SVG document**
: There is one parse tree. Style sheets defined anywhere within the SVG document (in style elements or style attributes, or in external style sheets linked with the style sheet processing instruction) apply across the entire SVG document.
**Stand-alone SVG document embedded in an HTML or XML document with the ‘img’, ‘object’ (HTML) or [‘image’](struct.md#ImageElement) (SVG) elements**
: There are two completely separate parse trees; one for the referencing document (perhaps HTML or XHTML), and one for the SVG document. Style sheets defined anywhere within the referencing document (in style elements or style attributes, or in external style sheets linked with the style sheet processing instruction) apply across the entire referencing document but have no effect on the referenced SVG document. Style sheets defined anywhere within the referenced SVG document (in style elements or style attributes, or in external style sheets linked with the style sheet processing instruction) apply across the entire SVG document, but do not affect the referencing document (perhaps HTML or XHTML). To get the same styling across both the [X]HTML document and the SVG document, link them both to the same style sheet.
**Stand-alone SVG content textually included in an XML document**
: There is a single parse tree, using multiple namespaces; one or more subtrees are in the SVG namespace. Style sheets defined anywhere within the XML document (in style elements or style attributes, or in external style sheets linked with the style sheet processing instruction) apply across the entire document, including those parts of it in the SVG namespace. To get different styling for the SVG part, use the [‘style’](#StyleAttribute) attribute, or put an [‘id’](struct.md#IDAttribute) on the [‘svg’](struct.md#SVGElement) element and use contextual CSS selectors, or use XSL selectors.
## <a id="UAStyleSheet"></a>6.16 User agent style sheet

The user agent shall maintain a user agent style sheet ([[CSS2](refs.md#ref-CSS2)], section 6.4) for elements in the SVG namespace for visual media ([[CSS2](refs.md#ref-CSS2)], section 7.3.1). The user agent style sheet below is expressed using CSS syntax; however, user agents are required to support the behavior that corresponds to this default style sheet even if CSS style sheets are not supported in the user agent:

```
svg, symbol, image, marker, pattern, foreignObject { overflow: hidden }
svg { width:attr(width); height:attr(height) }
```

The first line of the above user agent style sheet will cause the [initial clipping path](masking.md#InitialClippingPath) to be established at the bounds of the [initial viewport](coords.md#SVGViewport). Furthermore, it will cause new clipping paths to be established at the bounds of the listed elements, all of which are [elements that establish a new viewport](coords.md#ElementsThatEstablishViewports). (Refer to the description of SVG's use of the [‘overflow’](masking.md#OverflowProperty) property for more information.)

The second line of the above user agent style sheet will cause the [‘width’](struct.md#SVGElementWidthAttribute) and [‘height’](struct.md#SVGElementHeightAttribute) attributes on the [‘svg’](struct.md#SVGElement) element to be used as the default values for the 'width' and 'height' properties during layout ([[CSS2](refs.md#ref-CSS2)], chapter 9).

## <a id="AuralStyleSheets"></a>6.17 Aural style sheets

For the purposes of aural media, SVG represents a stylable XML grammar. In user agents that support CSS aural style sheets, aural style properties ([[CSS2](refs.md#ref-CSS2)], chapter 19) can be applied as defined in CSS2.

Aural style properties can be applied to any SVG element that can contain character data content, including [‘desc’](struct.md#DescElement) [‘title’](struct.md#TitleElement) [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement), [‘altGlyph’](text.md#AltGlyphElement) and [‘textPath’](text.md#TextPathElement). On user agents that support aural style sheets, the following CSS2 properties can be applied:

| Aural property | Definition in [[CSS2](refs.md#ref-CSS2)] |
| --- | --- |
| <a id="AzimuthProperty"></a>‘azimuth’ | Section 19.7 |
| <a id="CueProperty"></a>‘cue’ | Section 19.5 |
| <a id="CueAfterProperty"></a>‘cue-after’ | Section 19.5 |
| <a id="CueBeforeProperty"></a>‘cue-before’ | Section 19.5 |
| <a id="ElevationProperty"></a>‘elevation’ | Section 19.7 |
| <a id="PauseProperty"></a>‘pause’ | Section 19.4 |
| <a id="PauseAfterProperty"></a>‘pause-after’ | Section 19.4 |
| <a id="PauseBeforeProperty"></a>‘pause-before’ | Section 19.4 |
| <a id="PitchProperty"></a>‘pitch’ | Section 19.8 |
| <a id="PitchRangeProperty"></a>‘pitch-range’ | Section 19.8 |
| <a id="PlayDuringProperty"></a>‘play-during’ | Section 19.6 |
| <a id="RichnessProperty"></a>‘richness’ | Section 19.8 |
| <a id="SpeakProperty"></a>‘speak’ | Section 19.3 |
| <a id="SpeakHeaderProperty"></a>‘speak-header’ | Section 17.7.1 |
| <a id="SpeakNumeralProperty"></a>‘speak-numeral’ | Section 19.9 |
| <a id="SpeakPunctuationProperty"></a>‘speak-punctuation’ | Section 19.9 |
| <a id="SpeechRateProperty"></a>‘speech-rate’ | Section 19.8 |
| <a id="StressProperty"></a>‘stress’ | Section 19.8 |
| <a id="VoiceFamilyProperty"></a>‘voice-family’ | Section 19.8 |
| <a id="VolumeProperty"></a>‘volume’ | Section 19.2 |

For user agents that support aural style sheets and also support DOM Level 2 Core [[DOM2](refs.md#ref-DOM2)], the user agent is required to support the DOM interfaces defined in Document Object Model CSS ([[DOM2STYLE](refs.md#ref-DOM2STYLE)], chapter 2) that correspond to aural properties. (See [Relationship with DOM2 CSS object model](https://www.w3.org/TR/SVG11/svgdom.html#RelationshipWithCSSOM).)

## <a id="DOMInterfaces"></a>6.18 DOM interfaces

### <a id="InterfaceSVGStyleElement"></a>6.18.1 Interface SVGStyleElement

The [SVGStyleElement](#InterfaceSVGStyleElement) interface corresponds to the [‘style’](#StyleElement) element.

```
interface SVGStyleElement : SVGElement,
                            SVGLangSpace {
  attribute DOMString type setraises(DOMException);
  attribute DOMString media setraises(DOMException);
  attribute DOMString title setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGStyleElement__type"></a>**type** (DOMString)**
:

Corresponds to attribute [‘type’](#StyleElementTypeAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGStyleElement__media"></a>**media** (DOMString)**
:

Corresponds to attribute [‘media’](#StyleElementMediaAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGStyleElement__title"></a>**title** (DOMString)**
:

Corresponds to attribute [‘title’](#StyleElementTitleAttribute) on the given element.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](struct.md) ⋅ [Next](coords.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
