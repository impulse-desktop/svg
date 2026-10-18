<!-- https://drafts.csswg.org/css-box-4/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Box Model Module Level 4

<a id="w3c-state"></a>Editor’s Draft, 6 August 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-box-4/
**Latest published version: **
: https://www.w3.org/TR/css-box-4/
**Previous Versions: **
: https://www.w3.org/TR/2020/WD-css-box-4-20200421/
**Feedback: **
: CSSWG Issues Repository
: [Inline In Spec](#issues-index)
**Editor: **
: Elika J. Etemad / fantasai (Apple)
**Suggest an Edit for this Spec: **
: GitHub Editor
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

This specification describes the margin and padding properties, which create spacing in and around a CSS box. It may later be extended to include borders (currently described in [[css-backgrounds-3]](#biblio-css-backgrounds-3)).

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-box” in the title, like this: “[css-box] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This subsection is not normative.*

CSS describes how each element and each string of text in a source document is laid out by transforming the element tree into a set of boxes, whose size, position, and stacking level on the [canvas](svg2/coords.md#TermCanvas) depend on the values of their CSS properties.

Note: [CSS Cascading and Inheritance](css-cascade-5.md) describes how properties are assigned to elements in the box tree, while CSS Display 3 § 1 Introduction describes how the element tree is transformed into the box tree.

Each CSS box has a rectangular content area, a band of padding around the content, a border around the padding, and a margin outside the border. The sizing properties [[css-sizing-3]](#biblio-css-sizing-3), together with various other properties that control layout, define the size of the content area. The box styling properties—​[padding](#propdef-padding) and its longhands, border and its longhands, and [margin](#propdef-margin) and its longhands—​define the sizes of these other areas. Margins and padding are defined in this module; borders are defined in [[css-backgrounds-3]](#biblio-css-backgrounds-3).

Note: This module originally contained the CSS Level 3 specification prose relating to box generation (now defined in [[css-display-3]](#biblio-css-display-3)), the box model (defined here), as well as block layout (now only defined in [[CSS2]](#biblio-css2) Chapters 9 and 10). Since its maintenance was put aside during the development of CSS2.1, its prose was severely outdated by the time CSS2 Revision 1 was finally completed. Therefore, the block layout portion of the prose has been retired, to be re-synched to CSS2 and updated as input to a new Block Layout module at some point in the future. It is being split apart from this module and from the CSS Display Module both because of the practical concern that it would be a huge amount of work and also in recognition that CSS now has multiple layout models (Flex Layout, Grid Layout, Positioned Layout, and Table Layout, in addition to Block Layout) which each deserve their own parallel module.

### <a id="values"></a>1.1. Value Definitions

This specification follows the CSS property definition conventions from [[CSS2]](#biblio-css2) using the value definition syntax from [[CSS-VALUES-3]](#biblio-css-values-3). Value types not defined in this specification are defined in CSS Values & Units [[CSS-VALUES-3]](#biblio-css-values-3). Combination with other CSS modules may expand the definitions of these value types.

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their property value. For readability they have not been repeated explicitly.

### <a id="placement"></a>1.2. Module Interactions

This module replaces the definitions of the margin and padding properties defined in [[CSS2]](#biblio-css2) sections 8.1, 8.2, 8.3 (but not 8.3.1), and 8.4.

All properties in this module apply to the ::first-line and ::first-letter pseudo-elements.

## <a id="box-model"></a>2. The CSS Box Model

Each box has a *<a id="content-area"></a>content area* (which contains its content—​text, descendant boxes, an image or other replaced element content, etc.) and optional surrounding *<a id="padding-area"></a>padding*, *<a id="border-area"></a>border*, and *<a id="margin-area"></a>margin areas*; the size of each area is specified by corresponding properties, and can be zero (or in the case of margins, negative). The following diagram shows how these areas relate and the terminology used to refer to the various parts of the box:

[image: Diagram of a typical box, showing the content, padding, border and margin areas]

The various areas and edges of a typical box.

The background of the content, padding, and border areas of a box is specified by its [background](css-backgrounds-3.md#propdef-background) properties. The border area can additionally be painted with a border style using the border properties. Margins are always transparent. See [[css-backgrounds-3]](#biblio-css-backgrounds-3).

The margin, border, and padding can be broken down into top, right, bottom, and left segments, each of which can be controlled independently by its corresponding property.

### <a id="box-edges"></a>2.1. Boxes and Edges

The perimeter of each of the four areas (content, padding, border, and margin) is called an *<a id="box-box-edge"></a>edge*, and each [edge](#box-box-edge) can be broken down into a top, right, bottom, and left side. Thus each box has four edges each composed of four sides:

***<a id="content-edge"></a>content edge* or *<a id="inner-edge"></a>inner edge* **
: The content edge surrounds the rectangle given by the width and height of the box, which often depend on the element’s content and/or its containing block size. The four sides of the [content edge](#content-edge) together define the box’s *<a id="content-box"></a>content box*.
***<a id="padding-edge"></a>padding edge* **
: The padding edge surrounds the box’s padding. If the padding has zero width on a given side, the padding edge coincides with the content edge on that side. The four sides of the [padding edge](#padding-edge) together define the box’s *<a id="padding-box"></a>padding box*, which contains both the [content](#content-area) and [padding areas](#padding-area).
***<a id="border-edge"></a>border edge* **
: The border edge surrounds the box’s border. If the border has zero width on a given side, the border edge coincides with the padding edge on that side. The four sides of the [border edge](#border-edge) together define the box’s *<a id="border-box"></a>border box*, which contains the box’s [content](#content-area), [padding](#padding-area), and [border areas](#border-area).
***<a id="margin-edge"></a>margin edge* or *<a id="outer-edge"></a>outer edge* **
: The margin edge surrounds the box’s margin. If the margin has zero width on a given side, the margin edge coincides with the border edge on that side. The four sides of the [margin edge](#margin-edge) together define the box’s *<a id="margin-box"></a>margin box*, which contains the all of the box’s [content](#content-area), [padding](#padding-area), [border](#border-area), and [margin areas](#margin-area).
Properties like border-radius can change the shape of the box’s [edges](#box-box-edge) as used for painting and clipping (see [CSS Backgrounds 3 § 4.3 Corner Clipping](css-backgrounds-3.md#corner-clipping)); these effects typically do not affect layout however. To distinguish, specifications can refer to the relevant *<a id="shaped-edge"></a>shaped edge* or *<a id="unshaped-edge"></a>unshaped edge*

### <a id="fragmentation"></a>2.2. Fragmentation

When a box fragments—​is broken, as across lines or across pages, into separate box fragments—​each of its boxes ([content box](#content-box), [padding box](#padding-box), [border box](#border-box), [margin box](#margin-box)) also fragments. How the content/padding/border/margin areas react to fragmentation is specified in [[css-break-4]](#biblio-css-break-4) and controlled by the box-decoration-break property.

### <a id="keywords"></a>2.3. Box-edge Keywords

The following *<a id="typedef-box"></a>[<box>](#typedef-box)* CSS keywords are defined for use in properties (such as [transform-box](css-transforms-1.md#propdef-transform-box) and [background-clip](css-backgrounds-3.md#propdef-background-clip)) that need to refer to various box edges:

***<a id="valdef-box-content-box"></a>content-box* **
: Refers to the [content box](#content-box) or [content edge](#content-edge). (In an SVG context, treated as [fill-box](#valdef-box-fill-box).)
***<a id="valdef-box-padding-box"></a>padding-box* **
: Refers to the [padding box](#padding-box) or [padding edge](#padding-edge). (In an SVG context, treated as [fill-box](#valdef-box-fill-box).)
***<a id="valdef-box-border-box"></a>border-box* **
: Refers to the [border box](#border-box) or [border edge](#border-edge). (In an SVG context, treated as [stroke-box](#valdef-box-stroke-box).)
***<a id="valdef-box-margin-box"></a>margin-box* **
: Refers to the [margin box](#margin-box) or [margin edge](#margin-edge). (In an SVG context, treated as [stroke-box](#valdef-box-stroke-box).)
***<a id="valdef-box-fill-box"></a>fill-box* **
: Refers to the [object bounding box](svg2/coords.md#TermObjectBoundingBox) or its edges. (In a CSS box context, treated as [content-box](#valdef-box-content-box).)
***<a id="valdef-box-stroke-box"></a>stroke-box* **
: Refers to the [stroke bounding box](svg2/coords.md#TermStrokeBoundingBox) or its edges. (In a CSS box context, treated as [border-box](#valdef-box-border-box).)
***<a id="valdef-box-view-box"></a>view-box* **
: Refers to the nearest [SVG viewport](svg2/coords.md#TermSVGViewport)’s *<a id="svg-viewport-origin-box"></a>origin box*, which is a rectangle with the width and height of the SVG viewport, positioned such that its top left corner is anchored at the coordinate system origin. (In a CSS box context, treated as [border-box](#valdef-box-border-box).)

Note: When the [SVG viewport](svg2/coords.md#TermSVGViewport) is not itself anchored at the origin, this [origin box](#svg-viewport-origin-box) does not actually correspond to the SVG viewport!

For convenience, the following value types are defined to represents commonly used subsets of [<box>](#typedef-box):

```
<a id="typedef-visual-box"></a><visual-box> = content-box | padding-box | border-box
<a id="typedef-layout-box"></a><layout-box> = <visual-box> | margin-box
<a id="typedef-paint-box"></a><paint-box> = <visual-box> | fill-box | stroke-box
<a id="typedef-coord-box"></a><coord-box> = <paint-box> | view-box
```

## <a id="margins"></a>3. Margins

*<a id="margin"></a>Margins* surround the border edge of a box, providing spacing between boxes. The *<a id="margin-properties"></a>margin properties* specify the thickness of the [margin area](#margin-area) of a box. The [margin](#propdef-margin) [shorthand property](css-cascade-5.md#shorthand-property) sets the margin for all four sides while the margin [longhand properties](css-cascade-5.md#longhand) only set their respective side. This specification defines the physical margin longhands; CSS Logical Properties 1 § 4.2 Flow-Relative Margins: the margin-block-start, margin-block-end, margin-inline-start, margin-inline-end properties and margin-block and margin-inline shorthands additionally defines flow-relative margin longhands. Both sets of properties control the same set of margins: they are just different ways of indexing each side.

Note: Adjoining margins in block layout *collapse*. See CSS2§8.3.1 Collapsing Margins for details. Also, margins adjoining a fragmentation break are sometimes truncated. See CSS Fragmentation 4 § 5.2 Adjoining Margins at Breaks: the margin-break property for details.

### <a id="margin-physical"></a>3.1. Page-relative (Physical) Margin Properties: the [margin-top](#propdef-margin-top), [margin-right](#propdef-margin-right), [margin-bottom](#propdef-margin-bottom), and [margin-left](#propdef-margin-left) properties

| Name: | *<a id="propdef-margin-top"></a>margin-top*, *<a id="propdef-margin-right"></a>margin-right*, *<a id="propdef-margin-bottom"></a>margin-bottom*, *<a id="propdef-margin-left"></a>margin-left* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<length-percentage>](css-values-4.md#typedef-length-percentage) [\|](css-values-4.md#comb-one) auto |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except internal table elements, ruby base containers, and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to logical width of containing block |
| [Computed value:](css-cascade-5.md#computed) | the keyword auto or a computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value |
| Canonical order: | per grammar |
| Animation type: | by computed value type |
| Logical property group: | [margin](#propdef-margin) |

These properties set the top, right, bottom, and left [margin](#margin) of a box, respectively.

Negative values for margin properties are allowed, but there may be implementation-specific limits.

### <a id="margin-shorthand"></a>3.2. Margin Shorthand: the [margin](#propdef-margin) property

| Name: | *<a id="propdef-margin"></a>margin* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'margin-top'>](#propdef-margin-top)[{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except internal table elements, ruby base containers, and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to logical width of containing block |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | by computed value type |

The [margin](#propdef-margin) property is a shorthand property for setting [margin-top](#propdef-margin-top), [margin-right](#propdef-margin-right), [margin-bottom](#propdef-margin-bottom), and [margin-left](#propdef-margin-left) in a single declaration.

If there is only one component value, it applies to all sides. If there are two values, the top and bottom margins are set to the first value and the right and left margins are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third. If there are four values they apply to the top, right, bottom, and left, respectively.

<a id="example-2e169656"></a> The following code demonstrates some possible [margin](#propdef-margin) declarations.

```
body { margin: 2em }         /* all margins set to 2em */
body { margin: 1em 2em }     /* top & bottom = 1em, right & left = 2em */
body { margin: 1em 2em 3em } /* top=1em, right=2em, bottom=3em, left=2em */
```

The last rule of the example above is equivalent to the example below:

```
body {
  margin-top: 1em;
  margin-right: 2em;
  margin-bottom: 3em;
  margin-left: 2em; /* copied from opposite side (right) */
}
```

### <a id="margin-trim"></a>3.3. Margins at Container Edges: the [margin-trim](#propdef-margin-trim) property

| Name: | *<a id="propdef-margin-trim"></a>margin-trim* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | none [\|](css-values-4.md#comb-one) block \| [ block-start [\|\|](css-values-4.md#comb-any) block-end ] |
| [Initial:](css-cascade-5.md#initial-values) | none |
| [Applies to:](css-cascade-5.md#applies-to) | block containers, multi-column containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | a set of zero to two keywords indicating which sides to trim |
| Canonical order: | per grammar |
| Animation type: | discrete |

Oftentimes, margins are desired between siblings, but not at the start/end of the container where spacing can be controlled with padding. This property allows the container to trim the margins of its children where they adjoin the container’s edges. Values have the following meanings:

***<a id="valdef-margin-trim-none"></a>none* **
: Margins are not trimmed by the container.

Note: However, in block layout, child margins can collapse with their parent. See CSS2§8.3.1: Collapsing Margins.

***<a id="valdef-margin-trim-block-start"></a>block-start* **
***<a id="valdef-margin-trim-block-end"></a>block-end* **
: For in-flow boxes contained by this box, margins adjacent to the box’s specified edges are truncated to zero. It also truncates any descendant margins collapsed with such a margin (but not its own, its siblings’, or its ancestors’).
***<a id="valdef-margin-trim-block"></a>block* **
: Computes to block-start block-end.
Note: Following the shortest-serialization principle, [computed values](css-cascade-5.md#computed-value) equivalent to [block](#valdef-margin-trim-block) will serialize as that keyword.

Adjacency is not sensitive to space governed by box alignment [[CSS-ALIGN-3]](#biblio-css-align-3).

Note: See also the margin-break property, which applies to the box’s own margins when they adjoin a fragmentation break (page break / column break / etc.).

<a id="issue-c93b5c6f"></a> Define how this property affects margins at breaks if the box establishes a fragmentation context. See also Issue 3314.

Note: This property has no effect on the margins of floats when specified on a block container; a future level of this module may introduce specific controls for floats.

#### <a id="margin-trim-block"></a>3.3.1. Trimming Block Container Content

For block containers specifically, [margin-trim](#propdef-margin-trim) discards:

- The block-start margin of a block-level first child, when trimming at the block-start edge.
- The block-end margin of a block-level last child, when trimming at the block-end edge.
- Any margin collapsed with these margins.

It has no effect on any margins of inline-level descendants.

## <a id="paddings"></a>4. Padding

*<a id="padding"></a>Padding* is inserted between the content edge and the padding edge of a box, providing spacing between the content and the border. The *<a id="padding-properties"></a>padding properties* specify the thickness of the [padding area](#padding-area) of a box. The [padding](#propdef-padding) [shorthand property](css-cascade-5.md#shorthand-property) sets the padding for all four sides while the padding [longhand properties](css-cascade-5.md#longhand) only set their respective side. This specification defines the physical padding longhands; CSS Logical Properties 1 § 4.4 Flow-Relative Padding: the padding-block-start, padding-block-end, padding-inline-start, padding-inline-end properties and padding-block and padding-inline shorthands additionally defines flow-relative padding longhands. Both sets of properties control the same set of padding: they are just different ways of indexing each side.

Note: Backgrounds specified on the box are by default laid out and painted within the padding edges. (They are additionally painted underneath the border, in the [border area](#border-area).) This behavior can be adjusted using the [background-origin](css-backgrounds-3.md#propdef-background-origin) and [background-clip](css-backgrounds-3.md#propdef-background-clip) properties.

### <a id="padding-physical"></a>4.1. Page-relative (Physical) Padding Properties: the [padding-top](#propdef-padding-top), [padding-right](#propdef-padding-right), [padding-bottom](#propdef-padding-bottom), and [padding-left](#propdef-padding-left) properties

| Name: | *<a id="propdef-padding-top"></a>padding-top*, *<a id="propdef-padding-right"></a>padding-right*, *<a id="propdef-padding-bottom"></a>padding-bottom*, *<a id="propdef-padding-left"></a>padding-left* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<length-percentage [0,∞]>](css-values-4.md#typedef-length-percentage) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to logical width of containing block |
| [Computed value:](css-cascade-5.md#computed) | a computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value |
| Canonical order: | per grammar |
| Animation type: | by computed value type |
| Logical property group: | [padding](#propdef-padding) |

These properties set the top, right, bottom, and left [padding](#padding) of a box, respectively.

Negative values for padding properties are invalid.

### <a id="padding-shorthand"></a>4.2. Padding Shorthand: the [padding](#propdef-padding) property

| Name: | *<a id="propdef-padding"></a>padding* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<'padding-top'>](#propdef-padding-top)[{1,4}](css-values-4.md#mult-num-range) |
| [Initial:](css-cascade-5.md#initial-values) | 0 |
| [Applies to:](css-cascade-5.md#applies-to) | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | refer to logical width of containing block |
| [Computed value:](css-cascade-5.md#computed) | see individual properties |
| Canonical order: | per grammar |
| Animation type: | by computed value type |

The [padding](#propdef-padding) property is a shorthand property for setting [padding-top](#propdef-padding-top), [padding-right](#propdef-padding-right), [padding-bottom](#propdef-padding-bottom), and [padding-left](#propdef-padding-left) in a single declaration.

If there is only one component value, it applies to all sides. If there are two values, the top and bottom padding are set to the first value and the right and left padding are set to the second. If there are three values, the top is set to the first value, the left and right are set to the second, and the bottom is set to the third.

<a id="example-23862f1f"></a> The following code demonstrates some possible [padding](#propdef-padding) declarations.

```
body { padding: 2em }         /* all padding set to 2em */
body { padding: 1em 2em }     /* top & bottom = 1em, right & left = 2em */
body { padding: 1em 2em 3em } /* top=1em, right=2em, bottom=3em, left=2em */
```

The last rule of the example above is equivalent to the example below:

```
body {
  padding-top: 1em;
  padding-right: 2em;
  padding-bottom: 3em;
  padding-left: 2em; /* copied from opposite side (right) */
}
```

## <a id="borders"></a>5. Borders

*<a id="border"></a>Borders* fill the [border area](#border-area), to visually delineate the edges of the box, The *<a id="border-properties"></a>border properties* specify the thickness of the border area of a box, as well as its drawing style and color. See [CSS Backgrounds 3 § 3 Borders](css-backgrounds-3.md#borders) for the definition of the physical variants of these properties; CSS Logical Properties 1 § 4.5 Flow-Relative Borders additionally defines flow-relative border [longhands](css-cascade-5.md#longhand). Both sets of properties control the same set of borders: they are just different ways of indexing each side.

## <a id="changes"></a>6. Recent Changes

Changes since the 1 April 2024 Working Draft include:

-
Allowing combining the [block](#valdef-margin-trim-block) and [inline](#valdef-margin-trim-inline) keywords of [margin-trim](#propdef-margin-trim). (Issue 7884)
-
Introducing the [shaped edge](#shaped-edge) and [unshaped edge](#unshaped-edge) terms for easier cross-referencing of those concepts. (Issue 5132)

Changes since the 3 November 2022 Working Draft include:

-
Removing the effect of [margin-trim](#propdef-margin-trim) on floats, because there are use cases for both trimming and not trimming their margins, and it is complicated to implement. (Issue 8547)
-
Clarifying that only descendant margins are trimmed by [margin-trim](#propdef-margin-trim) even if self or sibling margins would have collapsed with the trimmed margin.
-
Introducing terms for the [shaped](#shaped-edge) and [unshaped](#unshaped-edge) [edges](#box-box-edge) of a box. (Issue 5132)

## <a id="changes-since-3"></a>7. Changes Since CSS Level 3

The following changes have been made to this module since Level 3:

- Adding the [margin-trim](#propdef-margin-trim) property.

## <a id="changes-since-2"></a>8. Changes Since CSS Level 2

The following changes have been made to this module since Level 2:

- Adapting the prose slightly to account for vertical writing modes.
- Defining the applicability of [margin](#propdef-margin), [padding](#propdef-padding) and their longhands to ruby base containers and ruby annotation containers

## <a id="priv"></a>9. Privacy Considerations

No privacy considerations have been reported on this module.

## <a id="sec"></a>10. Security Considerations

No security considerations have been reported on this module.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-align-3"></a>[CSS-ALIGN-3] **
: Elika Etemad; Tab Atkins Jr.. CSS Box Alignment Module Level 3. URL: https://drafts.csswg.org/css-align/
**<a id="biblio-css-backgrounds-3"></a>[CSS-BACKGROUNDS-3] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-css-borders-4"></a>[CSS-BORDERS-4] **
: Elika Etemad; et al. CSS Borders and Box Decorations Module Level 4. URL: https://drafts.csswg.org/css-borders-4/
**<a id="biblio-css-box-4"></a>[CSS-BOX-4] **
: Elika Etemad. CSS Box Model Module Level 4. URL: https://drafts.csswg.org/css-box-4/
**<a id="biblio-css-break-4"></a>[CSS-BREAK-4] **
: Rossen Atanassov; Elika Etemad. CSS Fragmentation Module Level 4. URL: https://drafts.csswg.org/css-break-4/
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-display-4"></a>[CSS-DISPLAY-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Display Module Level 4. URL: https://drafts.csswg.org/css-display-4/
**<a id="biblio-css-logical-1"></a>[CSS-LOGICAL-1] **
: Elika Etemad; Rossen Atanassov. CSS Logical Properties and Values Module Level 1. URL: https://drafts.csswg.org/css-logical-1/
**<a id="biblio-css-multicol-2"></a>[CSS-MULTICOL-2] **
: Florian Rivoal; Rachel Andrew. CSS Multi-column Layout Module Level 2. URL: https://drafts.csswg.org/css-multicol-2/
**<a id="biblio-css-pseudo-4"></a>[CSS-PSEUDO-4] **
: Elika Etemad; Alan Stearns. CSS Pseudo-Elements Module Level 4. URL: https://drafts.csswg.org/css-pseudo-4/
**<a id="biblio-css-ruby-1"></a>[CSS-RUBY-1] **
: Elika Etemad; et al. CSS Ruby Annotation Layout Module Level 1. URL: https://drafts.csswg.org/css-ruby-1/
**<a id="biblio-css-sizing-3"></a>[CSS-SIZING-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Box Sizing Module Level 3. URL: https://drafts.csswg.org/css-sizing-3/
**<a id="biblio-css-transforms-1"></a>[CSS-TRANSFORMS-1] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css-values-3"></a>[CSS-VALUES-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-writing-modes-4"></a>[CSS-WRITING-MODES-4] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 4. URL: https://drafts.csswg.org/css-writing-modes-4/
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-svg2"></a>[SVG2] **
: Karl Dubost; et al. [Scalable Vector Graphics (SVG) 2](svg2/index.md). URL: [https://w3c.github.io/svgwg/svg2-draft/](svg2/index.md)
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css-display-3"></a>[CSS-DISPLAY-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Display Module Level 3. URL: https://drafts.csswg.org/css-display/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order | Com­puted value |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [margin](#propdef-margin) | <'margin-top'>{1,4} | 0 | all elements except internal table elements, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | see individual properties |  |
| [margin-bottom](#propdef-margin-bottom) | <length-percentage> \| auto | 0 | all elements except internal table elements, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | the keyword auto or a computed <length-percentage> value | margin |
| [margin-left](#propdef-margin-left) | <length-percentage> \| auto | 0 | all elements except internal table elements, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | the keyword auto or a computed <length-percentage> value | margin |
| [margin-right](#propdef-margin-right) | <length-percentage> \| auto | 0 | all elements except internal table elements, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | the keyword auto or a computed <length-percentage> value | margin |
| [margin-top](#propdef-margin-top) | <length-percentage> \| auto | 0 | all elements except internal table elements, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | the keyword auto or a computed <length-percentage> value | margin |
| [margin-trim](#propdef-margin-trim) | none \| block \| [ block-start \|\| block-end ] | none | block containers, multi-column containers | no | N/A | discrete | per grammar | a set of zero to two keywords indicating which sides to trim |  |
| [padding](#propdef-padding) | <'padding-top'>{1,4} | 0 | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | see individual properties |  |
| [padding-bottom](#propdef-padding-bottom) | <length-percentage [0,∞]> | 0 | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | a computed <length-percentage> value | padding |
| [padding-left](#propdef-padding-left) | <length-percentage [0,∞]> | 0 | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | a computed <length-percentage> value | padding |
| [padding-right](#propdef-padding-right) | <length-percentage [0,∞]> | 0 | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | a computed <length-percentage> value | padding |
| [padding-top](#propdef-padding-top) | <length-percentage [0,∞]> | 0 | all elements except: internal table elements other than table cells, ruby base containers, and ruby annotation containers | no | refer to logical width of containing block | by computed value type | per grammar | a computed <length-percentage> value | padding |
