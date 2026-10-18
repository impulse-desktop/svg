<!-- https://drafts.csswg.org/compositing-1/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>Compositing and Blending Module Level 1

<a id="w3c-state"></a>Editor’s Draft, 8 October 2026

More details about this document

**This version: **
: https://drafts.csswg.org/compositing-1/
**Latest published version: **
: https://www.w3.org/TR/compositing-1/
**Previous Versions: **
: https://www.w3.org/TR/2015/CR-compositing-1-20150113/
**Implementation Report: **
: https://wpt.fyi/results/css/compositing
**Feedback: **
: CSSWG Issues Repository
: GitHub
: https://github.com/w3c/csswg-drafts/labels/compositing-1
**Editor: **
: Chris Harrelson (Google)
**Former Editors: **
: Rik Cabanier (then of Adobe Systems Inc.)
: Nikos Andronikos (Canon Information Systems Research Australia)
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/compositing/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

Compositing describes how shapes of different elements are combined into a single image. There are various possible approaches for compositing. Previous versions of SVG and CSS used [Simple Alpha Compositing](svg11/masking.md#SimpleAlphaBlending). In this model, each element is rendered into its own buffer and is then merged with its [backdrop](#backdrop) using the Porter Duff source-over operator. This specification will define a new compositing model that expands upon the Simple Alpha Compositing model by offering:

- additional Porter Duff compositing operators
- advanced blending modes which allow control of how colors mix in the areas where shapes overlap
- compositing groups

In addition, this specification will define CSS properties for blending and group isolation and the properties of the `globalCompositeOperation` attribute. CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “compositing” in the title, like this: “[compositing] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="introduction"></a>1. Introduction

*This subsection is non-normative.* The first part of this document describes the properties used to control the compositing in CSS. The second part will describe the algorithms of Porter Duff compositing and blending.

## <a id="reading-this-document"></a>2. Reading This Document

Each section of this document is normative unless otherwise specified.

### <a id="module-interactions"></a>2.1. Module interactions

This specification defines a set of CSS properties that affect the visual rendering of elements to which those properties are applied; these effects are applied after elements have been sized and positioned according to the Visual formatting model from [[CSS21]](#biblio-css21). Some values of these properties result in the creation of a containing block, and/or the creation of a stacking context.

The [background-blend-mode](#propdef-background-blend-mode) property also builds upon the properties defined in the CSS Backgrounds and Borders module [[CSS3BG]](#biblio-css3bg).

This specification also enhances the rules as specified in Section 14.2 Simple alpha compositing of [[SVG11]](#biblio-svg11) and [simple alpha compositing](css-color-4.md#alpha) of [[CSS-COLOR-4]](#biblio-css-color-4).

This module also extends the `globalCompositeOperation` attribute.

### <a id="values"></a>2.2. Values

This specification follows the CSS property definition conventions from [[CSS21]](#biblio-css21). Value types not defined in this specification are defined in CSS Level 2 Revision 1 [[CSS21]](#biblio-css21). Other CSS modules may expand the definitions of these value types: for example [[CSS-COLOR-4]](#biblio-css-color-4), when combined with this module, expands the definition of the <color> value type as used in this specification.

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the inherit keyword as their property value. For readability it has not been repeated explicitly.

## <a id="csscompositingandblending"></a>3. Specifying Blending in CSS

### <a id="compositingandblendingorder"></a>3.1. Order of graphical operations

The compositing model must follow the [SVG compositing](svg11/render.md#Introduction) model [[SVG11]](#biblio-svg11): first any filter effect is applied, then any clipping, masking, blending and compositing.

### <a id="csscompositingrules_CSS"></a>3.2. Behavior specific to HTML

Everything in CSS that creates a stacking context must be considered an [‘isolated’ group](#isolatedgroups). HTML elements themselves should not create groups.

An element that has blending applied, must blend with all the underlying content of the stacking context [[CSS21]](#biblio-css21) that that element belongs to.

The root element for an HTML document is the document element.

### <a id="csscompositingrules_SVG"></a>3.3. Behavior specific to SVG

By default, every element must create a [non-isolated group](#isolatedgroups).

However, certain operations in SVG will create [isolated groups](#isolatedgroups). If one of the following features is used, the group must become isolated:

- opacity
- filters
- 3D transforms (2D transforms must NOT cause isolation)
- blending
- masking

The root element for SVG is the [SVG element](svg11/struct.md#NewDocument).

### <a id="csskeywords"></a>3.4. CSS Properties

#### <a id="mix-blend-mode"></a>3.4.1. The [mix-blend-mode](#propdef-mix-blend-mode) property

The blend mode defines the formula that must be used to mix the colors with the backdrop. This behavior is described in more detail in [Blending](#blending).

| Name: | *<a id="propdef-mix-blend-mode"></a>mix-blend-mode* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<blend-mode>](#ltblendmodegt) |
| [Initial:](css-cascade-5.md#initial-values) | normal |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg11/intro.md#TermContainerElement), [graphics elements](svg11/intro.md#TermGraphicsElement) and [graphics referencing elements](svg11/intro.md#TermGraphicsReferencingElement). [[SVG11]](#biblio-svg11) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The syntax of the property of [<blend-mode>](#ltblendmodegt) is given with:

```
<a id="ltblendmodegt"></a><blend-mode> =
  normal |
  darken | multiply | color-burn |
  lighten | screen | color-dodge |
  overlay | soft-light | hard-light |
  difference | exclusion |
  hue | saturation | color | luminosity
```

Applying a blendmode other than [normal](#valdef-blend-mode-normal) to the element must establish a new stacking context [[CSS21]](#biblio-css21). This group must then be blended and composited with the stacking context that contains the element.

<a id="ex-duck-on-lime"></a>

Given the following sample markup:

```
<body>
  <img src="ducky.png"/>
</body>
```

And the following style rule:

```
body { background-color: lime; }
```

... will produce the following result:

[image: example of an image of duck with the lime color of the document]

Partially transparent image on a lime backdrop

If we change the style rule to include blending:

```
body { background-color: lime; }
img { mix-blend-mode: multiply; }
```

... the output will be the image blending with the lime background of the body element.

[image: example of an image of duck blending with the lime color of the document]

Blending of a transparent image on a lime backdrop.

<a id="ex-svg-screen-mode"></a>

Given the following svg code:

```
<svg>
  <circle cx="40" cy="40" r="40" fill="red"/>
  <circle cx="80" cy="40" r="40" fill="lime"/>
  <circle cx="60" cy="80" r="40" fill="blue"/>
</svg>
```

And the following style rule:

```
circle { mix-blend-mode: screen; }
```

... the output will be blending of the 3 circles. Each circle is rendered from bottom to top. Where the elements overlap, the blend mode produces a change in color.

[image: example of 3 circles blending with a screen blend mode]

Example of 3 circles blending with a screen blend mode

<a id="ex-opacity-stacking"></a>

In the following style sheet and document fragment:

```
body { background-color: lime; }
div { background-color: red; width: 200px; opacity: .95}
img { mix-blend-mode: difference; }

<body>
  <div>
     <img src="ducky.png"/>
  </div>
</body>
```

... the [opacity](css-color-4.md#propdef-opacity) on the div element is causing the creation of a stacking context. This causes the creation of a new group so the image doesn’t blend with the color of the body element.

[image: example of an image of duck blending with the lime color of the document]

Example of blending within a stacking context.

Note how the image is not blending with the lime color.

<a id="ex-overlay-blend"></a>

Given the following sample markup:

```
<body>
   <div>
     <p>''overlay'' blending on text</p>
   </div>
</body>
```

And the following style rule:

```
div { background-image: url('texture.png'); }
@font-face {
  font-family: "Mythos Std";
  src: url("http://myfontvendor.com/mythos.otf");
}
p {
  mix-blend-mode: overlay;
  font-family: "Mythos Std"
}
```

[image: example of text that has an overlay blend on top of a texture]

Text with a blend overlay on top of an image.

#### <a id="isolation"></a>3.4.2. The [isolation](#propdef-isolation) property

In SVG, this defines whether an element is isolated or not. For CSS, setting [isolation](#propdef-isolation) to isolate will turn the element into a stacking context.

By default, elements use the auto keyword which implies that they are not isolated. However operations that cause the creation of stacking context [[CSS21]](#biblio-css21) must cause a group to be isolated. These operations are described in ['behavior specific to HTML'](#csscompositingrules_CSS) and ['behavior specific to SVG'](#csscompositingrules_SVG).

| Name: | *<a id="propdef-isolation"></a>isolation* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<isolation-mode>](#isolated-propid) |
| [Initial:](css-cascade-5.md#initial-values) | auto |
| [Applies to:](css-cascade-5.md#applies-to) | All elements. In SVG, it applies to [container elements](svg11/intro.md#TermContainerElement), [graphics elements](svg11/intro.md#TermGraphicsElement) and [graphics referencing elements](svg11/intro.md#TermGraphicsReferencingElement). [[SVG11]](#biblio-svg11) |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The syntax of the property of [<isolation-mode>](#isolated-propid) is given with:

```
<a id="isolated-propid"></a><isolation-mode> = auto | isolate
```

<a id="img_isolation"></a> In CSS, a background image or the content of an img must always be rendered into an isolated group. For instance, if you link to an SVG file through the img tag, the artwork of that SVG will not blend with the backdrop of the content.

In SVG, [mask](css-masking-1.md#propdef-mask) always creates an isolated group.

#### <a id="background-blend-mode"></a>3.4.3. The [background-blend-mode](#propdef-background-blend-mode) property

Defines the blending mode of each background layer.

Each background layer must blend with the element’s background layer that is below it and the element’s background color. Background layers must not blend with the content that is behind the element, instead they must act as if they are rendered into an isolated group.

The description of the [background-blend-mode](#propdef-background-blend-mode) property is as follows:

| Name: | *<a id="propdef-background-blend-mode"></a>background-blend-mode* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<blend-mode>](#ltblendmodegt)[#](css-values-4.md#mult-comma) |
| [Initial:](css-cascade-5.md#initial-values) | normal |
| [Applies to:](css-cascade-5.md#applies-to) | All HTML elements |
| [Inherited:](css-cascade-5.md#inherited-property) | no |
| [Percentages:](css-values-4.md#percentages) | N/A |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | discrete |
| Media: | visual |

The [background-blend-mode](#propdef-background-blend-mode) list must be applied in the same order as [background-image](css-backgrounds-3.md#propdef-background-image) [[CSS3BG]](#biblio-css3bg). This means that the first element in the list will apply to the layer that is on top. If a property doesn’t have enough comma-separated values to match the number of layers, the UA must calculate its used value by repeating the list of values until there are enough.

If the [background](css-backgrounds-3.md#propdef-background) [[CSS3BG]](#biblio-css3bg) shorthand is used, the [background-blend-mode](#propdef-background-blend-mode) property for that element must be reset to its initial value.

<a id="ex-background-blend"></a>

Given the following sample markup:

```
<body>
   <div></div>
</body>
```

And the following style rule:

```
body { background-color: lime; }
div {
  width: 200px;
  height: 200px;
  background-size: 200px 200px;
  background-repeat:no-repeat;
  background-image: linear-gradient(to right, #000000 0%,#ffffff 100%), url('ducky.png');
  background-blend-mode: difference, normal;
}
```

[image: example of div that has an image of a duck and a gradient that is blending]

Blending of 2 background images.

Note that the gradient is not blending with the color of body. Instead it retains its original color.

## <a id="canvascompositingandblending"></a>4. Specifying Compositing and Blending in Canvas 2D

The canvas 2d context defines the `globalCompositeOperation` attribute that is used to set the current compositing and blending operator.

This property takes the following value:

**<a id="propdef-mix"></a>‘globalCompositeOperation’ **
:

| *Value:* | [<blend-mode>](#ltblendmodegt) \| [<composite-mode>](#compositemode) |
| --- | --- |
| *Initial:* | source-over |

The syntax of the property of [<composite-mode>](#compositemode) is given with:

```
<a id="compositemode"></a><composite-mode> = clear | copy | source-over | destination-over | source-in |    
destination-in | source-out | destination-out | source-atop |    
destination-atop | xor | lighter
```

## <a id="whatiscompositing"></a>5. Introduction to compositing

Compositing is the combining of a graphic element with its backdrop.

In the model described in this specification there are two steps to the overall compositing operation - [Porter-Duff compositing](#advancedcompositing) and [blending](#blending). The blending step determines how the colors from the graphic element and the backdrop interact.

Typically, the blending step is performed first, followed by the Porter-Duff compositing step. In the blending step, the resultant color from the mix of the element and the the [backdrop](#backdrop) is calculated. The graphic element’s color is replaced with this resultant color. The graphic element is then composited with the backdrop using the specified compositing operator.

Note: Shape is defined by the mathematical description of the shape. A particular point is either inside the shape or it is not. There are no gradations.

Note: Opacity is described using an alpha value, stored alongside the color value for each particular point. The alpha value is between 0 and 1, inclusive. A value of 0 means that the pixel has no coverage at that point, and is therefore transparent; i.e. there is no color contribution from any geometry because the geometry does not overlap this pixel. A value of 1 means that the pixel is fully opaque; the geometry completely overlaps the pixel.

### <a id="simplealphacompositing"></a>5.1. Simple alpha compositing

The formula for simple alpha compositing is

```
co = Cs x αs + Cb x αb x (1 - αs)
```

Where

- co: the premultiplied pixel value after compositing
- Cs: the color value of the source graphic element being composited
- αs: the alpha value of the source graphic element being composited
- Cb: the color value of the [backdrop](#backdrop)
- αb: the alpha value of the [backdrop](#backdrop)

Note: All values are between 0 and 1 inclusive.

The pixel value after compositing (co) is given by adding the contributions from the source graphic element [Cs x αs] and the backdrop [Cb x αb x (1 - αs)]. For both the graphic element and the backdrop, the color values are multiplied by the alpha to determine the amount of color that contributes. With zero alpha meaning that the color does not contribute and partial alpha means that some percentage of the color contributes. The contribution of the backdrop is further reduced based on the opacity of the graphic element. Conceptually, (1 - αs) of the backdrop shows through the graphic element, meaning that if the graphic element is fully opaque (αs=1) then no backdrop shows through.

The simple alpha compositing formula listed above gives a resultant color which is the result of the weighted average of the backdrop color and graphic element color, with the weighting determined by the backdrop and graphic element alphas. The resultant alpha value of the composite is simply the sum of the contributed alpha of the composited elements. The formula for the resultant alpha of the composite is

```
αo = αs + αb x (1 - αs)
```

Where

- αo: the alpha value of the composite
- αs: the alpha value of the graphic element being composited
- αb: the alpha value of the [backdrop](#backdrop)

Often, it can be more efficient to store a `pre-multiplied value` for the color and opacity. The pre-multiplied value is given by

```
cs = Cs x αs
```

with

- cs: the pre-multiplied value
- Cs: the color value
- αs: the alpha value

Thus the formula for simple alpha compositing using pre-multiplied values becomes

```
co = cs + cb x (1 - αs)
```

To extract the color component of a pre-multiplied value, the formula is reversed:

```
Co = co / αo
```

#### <a id="simplealphacompositingexamples"></a>5.1.1. Examples of simple alpha compositing

<a id="ex-basic-alpha"></a>

[image: Simple box showing alpha compositing]

This describes the most basic case. It consists of 1 shape that is filled with a solid color (α = 1). The shape is composited with an empty background. The empty background has no effect on the resultant composite.

```
Cs = RGB(1,0,0)
αs = 1
Cb = RGB(0,0,0)
αb = 0

co = Cs x αs + Cb x αb x (1 - αs)
co = RGB(1,0,0) x 1 + RGB(0,0,0) x 0 x (1 - 1)
co = RGB(1,0,0) x 1
co = RGB(1,0,0)
```

<a id="ex-shapes-intersect"></a>

[image: simple shape]

This is a more complex example. There is no transparency, but the 2 shapes intersect.

Applying the compositing formula in the area of intersection, gives:

```
Cs = RGB(0,0,1)
αs = 1
Cb = RGB(1,0,0)
αb = 1

co = Cs x αs + Cb x αb x (1 - αs)
co = RGB(0,0,1) x 1 + RGB(1,0,0) x 1 x (1 - 1)
co = RGB(0,0,1) x 1 + RGB(1,0,0) x 1 x 0
co = RGB(0,0,1) x 1
co = RGB(0,0,1)
```

Calculating the alpha of the resultant composite

```
αo = αs + αb x (1 - αs)
αo = 1 + 1 x (1 - 1)
αo = 1
```

Calculating the color component of the resultant composite

```
Co = co / αo
Co = RGB(0, 0, 1) / 1
Co = RGB(0, 0, 1)
```

<a id="ex-transparent-over-opaque"></a>

[image: interaction of a solid box with a transparent box on top]

This is an example where the shape has some transparency, but the [backdrop](#backdrop) is fully opaque.

Applying the compositing formula in the area of intersection, gives:

```
Cs = RGB(0,0,1)
αs = 0.5
Cb = RGB(1,0,0)
αb = 1

co = Cs x αs + Cb x αb x (1 - αs)
co = RGB(0,0,1) x 0.5 + RGB(1,0,0) x 1 x (1 - 0.5)
co = RGB(0,0,1) x 0.5 + RGB(1,0,0) x 0.5
co = RGB(0.5,0,0.5)
```

Calculating the alpha of the resultant composite

```
αo = αs + αb x (1 - αs)
αo = 0.5 + 1 x (1 - 0.5)
αo = 1
```

Calculating the color component of the resultant composite

```
Co = co / αo
Co = RGB(0.5, 0, 0.5) / 1
Co = RGB(0.5, 0, 0.5)
```

<a id="ex-two-transparent"></a>

[image: interaction of 2 transparent boxes]

Figure 4 shows an example where both the shape and the [backdrop](#backdrop) are transparent.

Applying the compositing formula in the area of intersection, gives:

```
Cs = RGB(0,0,1)
αs = 0.5
Cb = RGB(1,0,0)
αb = 0.5

co = Cs x αs + Cb x αb x (1 - αs)
co = RGB(0,0,1) x 0.5 + RGB(1,0,0) x 0.5 x (1 - 0.5)
co = RGB(0,0,1) x 0.5 + RGB(1,0,0) x 0.25
co = RGB(0.25, 0, 0.5)
```

Calculating the alpha of the resultant composite

```
αo = αs + αb x (1 - αs)
αo = 0.5 + 0.5 x (1 - 0.5)
αo = 0.75
```

Calculating the color component of the resultant composite

```
Co = co / αo
Co = RGB(0.25, 0, 0.5) / 0.75
Co = RGB(0.33, 0, 0.66)
```

## <a id="generalformula"></a>6. General Formula for Compositing and Blending

The general formula for compositing and blending which allows for selection of the compositing operator and blending function comprises two steps. The terms used in these functions will be described in detail in the following sections.

Apply the blend in place

```
Cs = (1 - αb) x Cs + αb x B(Cb, Cs)
```

Composite

```
Co = αs x Fa x Cs + αb x Fb x Cb
```

Where:

- **Cs**: is the source color
- **Cb**: is the backdrop color
- **αs**: is the source alpha
- **αb**: is the backdrop alpha
- **B(Cb, Cs)**: is the mixing function
- **Fa**: is defined by the Porter Duff operator in use
- **Fb**: is defined by the Porter Duff operator in use

## <a id="backdropCalc"></a>7. Backdrop calculation

The *<a id="backdrop"></a>backdrop* is the content behind the element and is what the element is composited with. This means that the backdrop is the result of compositing all previous elements.

### <a id="backdropexamples"></a>7.1. Examples of backdrop calculation

<a id="ex-simple-backdrop-calculation"></a>

[image: example of a simple backdrop calculation]

This example has 2 simple shapes. The backdrop for the blue shape includes the bottom right corner of the red shape . The dotted line shows the area that is examined during compositing of the blue shape.

<a id="ex-backdrop-with-alpha"></a>

[image: example of a backdrop with alpha]

The shape in the backdrop has an alpha value. The alpha value of the backdrop shape is preserved when the backdrop is calculated.

## <a id="groups"></a>8. Compositing Groups

Compositing groups allow more control over the interaction of compositing with the backdrop. Groups can be used to specify how a compositing effect within a group will interact with the content that is already in the scene (the backdrop).

Compositing groups may be made up of any number of elements, and may contain other compositing groups.

The default properties of a compositing group shall cause no visual difference compared to having no group. See [Group Invariance](#groupinvariance).

A compositing group is rendered by first compositing the elements of the group onto the initial backdrop. The result of this is a single element containing color and alpha information. This element is then composited onto the group backdrop. Steps shall be taken to ensure the group backdrop makes only a single contribution to the final composite.

**<a id="initialbackdrop"></a>initial backdrop **
: The initial backdrop is the backdrop used for compositing the group’s first element. This will be the same as the group backdrop in a non-isolated group, or a fully transparent backdrop for an isolated group.
**<a id="groupbackdrop"></a>group backdrop **
: The group backdrop is the result of compositing all elements up to but not including the first element in the group.
### <a id="groupinvariance"></a>8.1. Group invariance

An important property of simple alpha compositing is its group invariance. This behavior is preserved in the more complex model described in this specification. Adding or removing grouping with default attributes shall not show visual differences.

```
so: A + B + C = A + (B + C) = (A + B) + C
```

When adding attributes to the group such as isolate, blending modes other than [normal](#valdef-blend-mode-normal) or Porter Duff compositing operators other than source-over, groups may no longer be invariant.

### <a id="isolatedgroups"></a>8.2. Isolated Groups

In an isolated group, the initial backdrop shall be black and fully transparent.

In this instance, the initial backdrop is different than the group backdrop. The only interaction with the group backdrop shall occur when the group’s computed color, shape and alpha are composited with it.

See '[Isolated groups and Porter Duff modes](#groupcompositing)' for a description of the effect of isolated groups on compositing. See '[Effect of group isolation on blending](#isolationblending)' for a description of the effect of isolated groups on blending.

### <a id="pagebackdrop"></a>8.3. The Root Element Group

The [isolated group](#isolatedgroups) for the root element is the root element group. All elements and other groups are composited into this group. The canvas background [[CSS3BG]](#biblio-css3bg) is painted into the root element group, and any filter, clip-path, mask and and opacity for the root element is then applied, before compositing into the [root group](#rootgroup), if present.

### <a id="rootgroup"></a>8.4. The Root Group

The root group encompasses the entire canvas and contains (or is below) the root element group of the root element of a web page.

Browsers often use an infinite [white, 100% opaque](css-color-4.md#sample) canvas surface [[CSS3BG]](#biblio-css3bg) in the root group, for final compositing, but this is not required.

## <a id="advancedcompositing"></a>9. Advanced compositing features

[Simple alpha compositing](#simplealphacompositing) uses the [source-over](#porterduffcompositingoperators_srcover) Porter Duff compositing operator.

Porter Duff compositing is based on a model of a pixel in which two shapes (source and destination) may contribute to the final color of the pixel. The pixel is divided into 4 sub-pixel regions and each region represents a possible combination of source and destination. [[PORTERDUFF]](#biblio-porterduff)

The four regions are:

**Source Only **
: Where only the source contributes to the pixel color
**Destination only **
: where only the destination contributes to the pixel color
**Both **
: Source and Destination – where both the source and destination may combine to define the pixel color
**None **
: No source or Destination – where neither make a contribution to the final pixel color
Note: Destination is synonymous with backdrop. The term destination is used in this section as this is considered the standard when working with Porter Duff compositing. Additionally, the compositing operators use destination in their names.

[image: overview of the regions affected by porter duff]

The contribution from each region to the final pixel color is defined by the coverage of the shape at that pixel, and the operator in use. Coverage is specified in terms of alpha. Full alpha (1) implies full coverage, while zero alpha (0) implies no coverage. This means that the area of each region within the sub-pixel is dependent on the coverage of each shape contributing to the pixel. The area of each region can be calculated with the following equations:

| Both | αs x αb |
| --- | --- |
| Source only | αs x (1 – αb) |
| Destination only | αb x (1 – αs) |
| None | (1 – αs) x (1 – αb) |

The figure above represents coverage of 0.5 for both source and destination.

```
Both = 0.5 x 0.5 = 0.25
Source Only = 0.5 (1 – 0.5) = 0.25
Destination Only = 0.5(1 – 0.5) = 0.25
None = (1 – 0.5)(1 – 0.5) = 0.25
```

Therefore, the coverage of each region is 0.25 in this example.

### <a id="porterduffcompositingoperators"></a>9.1. The Porter Duff Compositing Operators

The landmark paper by Thomas Porter and Tom Duff, who worked for Lucasfilm, defined the algebra of compositing and developed the twelve "Porter Duff" operators. These operators control the results of mixing the four sub-pixel regions formed by the overlapping of graphical objects that have an alpha or pixel coverage channel/value. The operators use all practical combinations of the four regions.

There are 12 basic Porter Duff operators, satisfying all possible combinations of source and destination.

From the geometric representation of each operator, the contribution of each shape can be seen to be expressed as a fraction of the total coverage of the output. For example, in source over, the possible contribution of source is full (1) and the possible contribution of destination is whatever is remaining (1 – αs). This is modified by the coverage of source and destination to give the equation for the final coverage of the pixel:

```
αo = αs x 1 + αb x (1 – αs)
```

The fractional terms Fa (1 in this example) and Fb (1 – αs in this example) are defined for each operator and specify the fraction of the shapes that may contribute to the final pixel value. The general form of the equation for coverage is:

```
αs x Fa + αb x Fb
```

and incorporating color gives the general Porter Duff equation

```
co = αs x Fa x Cs + αb x Fb x Cb
```

Where:

- co is the output color pre-multiplied with the output alpha [0 <= co <= 1]
- αs is the coverage of the source Fa is defined by the operator and controls inclusion of the source Cs is the color of the source (not multiplied by alpha)
- αb is the coverage of the destination Fb is defined by the operator and controls inclusion of the destination Cb is the color of the destination (not multiplied by alpha)

#### <a id="porterduffcompositingoperators_clear"></a>9.1.1. Clear

No regions are enabled.

[image: example of porter duff clear]

```
Fa = 0; Fb = 0
co = 0
αo = 0
```

#### <a id="porterduffcompositingoperators_src"></a>9.1.2. Copy

Only the source will be present.

[image: example of porter duff copy]

```
Fa = 1; Fb = 0
co = αs x Cs
αo = αs
```

#### <a id="porterduffcompositingoperators_dst"></a>9.1.3. Destination

Only the destination will be present.

[image: example of porter duff destination]

```
Fa = 0; Fb = 1
co = αb x Cb
αo = αb
```

#### <a id="porterduffcompositingoperators_srcover"></a>9.1.4. Source Over

Source is placed over the destination.

[image: example of porter duff source over]

```
Fa = 1; Fb = 1 – αs
co = αs x Cs + αb x Cb x (1 – αs)
αo = αs + αb x (1 – αs)
```

#### <a id="porterduffcompositingoperators_dstover"></a>9.1.5. Destination Over

Destination is placed over the source.

[image: example of porter duff destination over]

```
Fa = 1 – αb; Fb = 1
co = αs x Cs x (1 – αb) + αb x Cb
αo = αs x (1 – αb) + αb
```

#### <a id="porterduffcompositingoperators_srcin"></a>9.1.6. Source In

The source that overlaps the destination, replaces the destination.

[image: example of porter duff source in]

```
Fa = αb; Fb = 0
co = αs x Cs x αb
αo = αs x αb
```

#### <a id="porterduffcompositingoperators_dstin"></a>9.1.7. Destination In

Destination which overlaps the source, replaces the source.

[image: example of porter duff destination in]

```
Fa = 0; Fb = αs
co = αb x Cb x αs
αo = αb x αs
```

#### <a id="porterduffcompositingoperators_srcout"></a>9.1.8. Source Out

Source is placed, where it falls outside of the destination.

[image: example of porter duff source out]

```
Fa = 1 – αb; Fb = 0
co = αs x Cs x (1 – αb)
αo = αs x (1 – αb)
```

#### <a id="porterduffcompositingoperators_dstout"></a>9.1.9. Destination Out

Destination is placed, where it falls outside of the source.

[image: example of porter duff destination out]

```
Fa = 0; Fb = 1 – αs
co = αb x Cb x (1 – αs)
αo = αb x (1 – αs)
```

#### <a id="porterduffcompositingoperators_srcatop"></a>9.1.10. Source Atop

Source which overlaps the destination, replaces the destination. Destination is placed elsewhere.

[image: example of porter duff source atop]

```
Fa = αb; Fb = 1 – αs
co = αs x Cs x αb + αb x Cb x (1 – αs)
αo = αs x αb + αb x (1 – αs)
```

#### <a id="porterduffcompositingoperators_dstatop"></a>9.1.11. Destination Atop

Destination which overlaps the source replaces the source. Source is placed elsewhere.

[image: example of porter duff destination atop]

```
Fa = 1 - αb; Fb = αs
co = αs x Cs x (1 - αb) + αb x Cb x αs
αo = αs x (1 - αb) + αb x αs
```

#### <a id="porterduffcompositingoperators_xor"></a>9.1.12. XOR

The non-overlapping regions of source and destination are combined.

[image: example of porter duff xor]

```
Fa = 1 - αb; Fb = 1 – αs
co = αs x Cs x (1 - αb) + αb x Cb x (1 – αs)
αo = αs x (1 - αb) + αb x (1 – αs)
```

#### <a id="porterduffcompositingoperators_plus"></a>9.1.13. Lighter

Display the sum of the source image and destination image. It is defined in the Porter Duff paper as the plus operator [[PORTERDUFF]](#biblio-porterduff).

```
Fa = 1; Fb = 1
co = αs x Cs + αb x Cb;
αo = αs + αb
```

### <a id="groupcompositing"></a>9.2. Group compositing behavior with Porter Duff modes

When compositing the elements within an isolated group, the elements are composited over a transparent black initial backdrop. If the bottom element in the group uses a Porter Duff compositing operator which is dependent on the backdrop, such as [destination](#porterduffcompositingoperators_dst), [source-in](#porterduffcompositingoperators_srcin), [destination-in](#porterduffcompositingoperators_dstin), [destination-out](#porterduffcompositingoperators_dstout) or [source-atop](#porterduffcompositingoperators_srcatop), then the result of the composite will be empty. Subsequent elements within the group are composited with the result of the first composite.

## <a id="blending"></a>10. Blending

Blending is the aspect of compositing that calculates the mixing of colors where the source element and [backdrop](#backdrop) overlap. Conceptually, the colors in the source element are blended in place with the backdrop. After blending, the modified source element is composited with the backdrop. In practice, this is usually all performed in one step. The blending calculations must not use pre-multiplied color values.

The "mixing" formula is defined as:

```
Cm = B(Cb, Cs)
```

with:

- Cm: the result color after blending
- B: the formula that does the blending
- Cb: the [backdrop](#backdrop) color
- Cs: the source color

The result of the mixing formula must be clamped to the minimum and maximum values of the color range.

The result of the mixing function is modulated by the backdrop alpha. A fully opaque backdrop allows the mixing function to be fully realized. A transparent backdrop will cause the final result to be a weighted average between the source color and mixed color with the weight controlled by the backdrop alpha. The value of the new color becomes:

```
Cr = (1 - αb) x Cs + αb x B(Cb, Cs)
```

with:

- Cr: the result color
- B: the formula that does the blending
- Cs: the source color
- Cb: the [backdrop](#backdrop) color
- αb: the [backdrop](#backdrop) alpha

<a id="ex-blend-with-opacity"></a>

[image: example of blending with opacity]

This example has a red rectangle with a blending mode that is placed on top of a set of green rectangles that have different levels of opacity.

Note how the top rectangle shifts more toward red as the opacity of the [backdrop](#backdrop) gets smaller.

Note: The following formula gives the color value in the area where the source and backdrop intersects and then composites with the specified Porter Duff compositing formula. For simple alpha blending, the formula thus becomes:

```
simple alpha compositing:
    co = cs + cb x (1 - αs)
written as non-premultiplied:
    αo x Co = αs x Cs + (1 - αs) x αb x Cb
now substitute the result of blending for Cs:
    αo x Co = αs x ((1 - αb) x Cs + αb x B(Cb, Cs)) + (1 - αs) x αb x Cb
            = αs x (1 - αb) x Cs + αs x αb x B(Cb, Cs) + (1 - αs) x αb x Cb
```

### <a id="blendingseparable"></a>10.1. Separable blend modes

A blend mode is termed separable if each component of the result color is completely determined by the corresponding components of the constituent [backdrop](#backdrop) and source colors — that is, if the mixing formula is applied **separately** to each set of corresponding components.

Each of the following blend modes will apply the blending function B(Cb, Cs) on each of the color components. For simplicity, all the examples in this chapter use [source-over](#porterduffcompositingoperators_srcover) compositing.

#### <a id="blendingnormal"></a>10.1.1. *<a id="valdef-blend-mode-normal"></a>normal* blend mode

This is the default attribute which specifies no blending. The blending formula simply selects the source color.

```
B(Cb, Cs) = Cs
```

[image: example of normal blending]

#### <a id="blendingmultiply"></a>10.1.2. *<a id="valdef-blend-mode-multiply"></a>multiply* blend mode

The source color is multiplied by the destination color and replaces the destination.

The resultant color is always at least as dark as either the source or destination color. Multiplying any color with black results in black. Multiplying any color with white preserves the original color.

```
B(Cb, Cs) = Cb x Cs
```

[image: example of multiply blending]

#### <a id="blendingscreen"></a>10.1.3. *<a id="valdef-blend-mode-screen"></a>screen* blend mode

Multiplies the complements of the [backdrop](#backdrop) and source color values, then complements the result.

The result color is always at least as light as either of the two constituent colors. Screening any color with white produces white; screening with black leaves the original color unchanged. The effect is similar to projecting multiple photographic slides simultaneously onto a single screen.

```
B(Cb, Cs) = 1 - [(1 - Cb) x (1 - Cs)]
          = Cb + Cs -(Cb x Cs)
```

[image: example of screen blending]

#### <a id="blendingoverlay"></a>10.1.4. *<a id="valdef-blend-mode-overlay"></a>overlay* blend mode

Multiplies or screens the colors, depending on the [backdrop](#backdrop) color value.

Source colors overlay the [backdrop](#backdrop) while preserving its highlights and shadows. The backdrop color is not replaced but is mixed with the source color to reflect the lightness or darkness of the backdrop.

```
B(Cb, Cs) = HardLight(Cs, Cb)
```

Overlay is the inverse of the [hard-light](#valdef-blend-mode-hard-light) blend mode. See the definition of hard-light for the formula.

[image: example of overlay blending]

#### <a id="blendingdarken"></a>10.1.5. *<a id="valdef-blend-mode-darken"></a>darken* blend mode

Selects the darker of the [backdrop](#backdrop) and source colors.

The [backdrop](#backdrop) is replaced with the source where the source is darker; otherwise, it is left unchanged.

```
B(Cb, Cs) = min(Cb, Cs)
```

[image: example of darken blending]

#### <a id="blendinglighten"></a>10.1.6. *<a id="valdef-blend-mode-lighten"></a>lighten* blend mode

Selects the lighter of the [backdrop](#backdrop) and source colors.

The [backdrop](#backdrop) is replaced with the source where the source is lighter; otherwise, it is left unchanged.

```
B(Cb, Cs) = max(Cb, Cs)
```

The result must be rounded down if it exceeds the range.

[image: example of lighten blending]

#### <a id="blendingcolordodge"></a>10.1.7. *<a id="valdef-blend-mode-color-dodge"></a>color-dodge* blend mode

Brightens the [backdrop](#backdrop) color to reflect the source color. Painting with black produces no changes.

```
if(Cb == 0)
    B(Cb, Cs) = 0
else if(Cs == 1)
    B(Cb, Cs) = 1
else
    B(Cb, Cs) = min(1, Cb / (1 - Cs))
```

[image: example of color dodge blending]

#### <a id="blendingcolorburn"></a>10.1.8. *<a id="valdef-blend-mode-color-burn"></a>color-burn* blend mode

Darkens the [backdrop](#backdrop) color to reflect the source color. Painting with white produces no change.

```
if(Cb == 1)
    B(Cb, Cs) = 1
else if(Cs == 0)
    B(Cb, Cs) = 0
else
    B(Cb, Cs) = 1 - min(1, (1 - Cb) / Cs)
```

[image: example of color burn blending]

#### <a id="blendinghardlight"></a>10.1.9. *<a id="valdef-blend-mode-hard-light"></a>hard-light* blend mode

Multiplies or screens the colors, depending on the source color value. The effect is similar to shining a harsh spotlight on the [backdrop](#backdrop).

```
if(Cs <= 0.5)
    B(Cb, Cs) = Multiply(Cb, 2 x Cs)
else
    B(Cb, Cs) = Screen(Cb, 2 x Cs -1)
```

See the definition of [multiply](#valdef-blend-mode-multiply) and [screen](#valdef-blend-mode-screen) for their formulas.

[image: example of hard light blending]

#### <a id="blendingsoftlight"></a>10.1.10. *<a id="valdef-blend-mode-soft-light"></a>soft-light* blend mode

Darkens or lightens the colors, depending on the source color value. The effect is similar to shining a diffused spotlight on the [backdrop](#backdrop).

```
    if(Cs <= 0.5)
        B(Cb, Cs) = Cb - (1 - 2 x Cs) x Cb x (1 - Cb)
    else
        B(Cb, Cs) = Cb + (2 x Cs - 1) x (D(Cb) - Cb)
with
    if(Cb <= 0.25)
        D(Cb) = ((16 * Cb - 12) x Cb + 4) x Cb
    else
        D(Cb) = sqrt(Cb)
```

[image: example of soft light blending]

#### <a id="blendingdifference"></a>10.1.11. *<a id="valdef-blend-mode-difference"></a>difference* blend mode

Subtracts the darker of the two constituent colors from the lighter color.

Painting with white inverts the [backdrop](#backdrop) color; painting with black produces no change.

```
B(Cb, Cs) = | Cb - Cs |
```

[image: example of difference blending]

#### <a id="blendingexclusion"></a>10.1.12. *<a id="valdef-blend-mode-exclusion"></a>exclusion* blend mode

Produces an effect similar to that of the Difference mode but lower in contrast. Painting with white inverts the [backdrop](#backdrop) color; painting with black produces no change

```
B(Cb, Cs) = Cb + Cs - 2 x Cb x Cs
```

[image: example of exclusion blending]

### <a id="blendingnonseparable"></a>10.2. Non-separable blend modes

Nonseparable blend modes consider all color components in combination as opposed to the separable ones that look at each component individually. All of these blend modes conceptually entail the following steps:

1. Convert the [backdrop](#backdrop) and source colors from the blending color space to an intermediate hue-saturation-luminosity representation.
2. Create a new color from some combination of hue, saturation, and luminosity components selected from the [backdrop](#backdrop) and source colors.
3. Convert the result back to the original color space.

The nonseparable blend mode formulas make use of several auxiliary functions:

```
    Lum(C) = 0.3 x Cred + 0.59 x Cgreen + 0.11 x Cblue

    ClipColor(C)
        L = Lum(C)
        n = min(Cred, Cgreen, Cblue)
        x = max(Cred, Cgreen, Cblue)
        if(n < 0)
            C = L + (((C - L) × L) / (L - n))

        if(x > 1)
            C = L + (((C - L) × (1 - L)) / (x - L))

        return C

    SetLum(C, l)
        d = l - Lum(C)
        Cred = Cred + d
        Cgreen = Cgreen + d
        Cblue = Cblue + d
        return ClipColor(C)

    Sat(C) = max(Cred, Cgreen, Cblue) - min(Cred, Cgreen, Cblue)

The subscripts min, mid, and max in the next function refer to the color
components having the minimum, middle, and maximum values upon entry to the function.

    SetSat(C, s)
        if(Cmax > Cmin)
            Cmid = (((Cmid - Cmin) x s) / (Cmax - Cmin))
            Cmax = s
        else
            Cmid = Cmax = 0
        Cmin = 0
        return C;
```

#### <a id="blendinghue"></a>10.2.1. *<a id="valdef-blend-mode-hue"></a>hue* blend mode

Creates a color with the hue of the source color and the saturation and luminosity of the [backdrop](#backdrop) color.

```
B(Cb, Cs) = SetLum(SetSat(Cs, Sat(Cb)), Lum(Cb))
```

[image: example of hue blending]

#### <a id="blendingsaturation"></a>10.2.2. *<a id="valdef-blend-mode-saturation"></a>saturation* blend mode

Creates a color with the saturation of the source color and the hue and luminosity of the [backdrop](#backdrop) color. Painting with this mode in an area of the backdrop that is a pure gray (no saturation) produces no change.

```
B(Cb, Cs) = SetLum(SetSat(Cb, Sat(Cs)), Lum(Cb))
```

[image: example of saturation blending]

#### <a id="blendingcolor"></a>10.2.3. *<a id="valdef-blend-mode-color"></a>color* blend mode

Creates a color with the hue and saturation of the source color and the luminosity of the [backdrop](#backdrop) color. This preserves the gray levels of the backdrop and is useful for coloring monochrome images or tinting color images.

```
B(Cb, Cs) = SetLum(Cs, Lum(Cb))
```

[image: example of color blending]

#### <a id="blendingluminosity"></a>10.2.4. *<a id="valdef-blend-mode-luminosity"></a>luminosity* blend mode

Creates a color with the luminosity of the source color and the hue and saturation of the [backdrop](#backdrop) color. This produces an inverse effect to that of the Color mode.

```
B(Cb, Cs) = SetLum(Cb, Lum(Cs))
```

[image: example of luminosity blending]

### <a id="isolationblending"></a>10.3. Effect of group isolation on blending

Note: In the following example, the elements used to construct the paper airplane are within a group. Each of these elements has their blend mode set to multiply.

<a id="ex-isolated-group"></a> The airplane on the left is a normal group, the airplane on the right is an [isolated group](#isolatedgroups).

In the isolated group, the elements within the group are composited onto an empty initial backdrop, this stops the elements within the group multiplying with the backdrop. In the normal group, the elements within the group are composited onto the initial backdrop containing the land and sky. Therefore the elements of the airplane multiply with the land and sky. In both instances, the result of the group composite is composited onto the land and sky using the normal mix-blend-mode (the default mix-blend-mode applied to the group).

[image: example of isolated group blending]

## <a id="privacy"></a> Privacy Considerations

No new privacy considerations have been reported on this specification.

## <a id="security"></a> Security Considerations

It is important that the timing to the blending and compositing operations is independent of the source and destination pixel. Operations must be implemented in such a way that they always take the same amount of time regardless of the pixel values.

If this rule is not followed, an attacker could infer information and mount a timing attack.

A timing attack is a method of obtaining information about content that is otherwise protected, based on studying the amount of time it takes for an operation to occur. If, for example, red pixels took longer to draw than green pixels, one might be able to reconstruct a rough image of the element being rendered, without ever having access to the content of the element.

## <a id="changes"></a>Changes

The following changes were made relative to the Candidate Recommendation of 13 January 2015:

- Added Privacy and Security sections
- Updated Animatable: no to Animation type: discrete
- Defined root elements for HTML and SVG, and the root element group
- Clarified that root groups are not necessarily opaque white
- Better definitions for root group and isolating groups
- Consistently use the multiplication symbol × rather than x or *
- Corrected examples that used [green](css-color-4.md#valdef-color-green) where [lime](css-color-4.md#valdef-color-lime) was meant
- Corrected initial value of background-blend-mode
- Assorted markup and linking fixes

The following changes were made relative to the Candidate Recommendation of 20 February 2014:

- force isolation of SVG images embedded as <img> no longer at risk
- removed destination as an option for [<composite-mode>](#compositemode)

The following changes were made relative to the Last Call Working draft of 7 January 2014:

- unneeded normative and informative references were removed

The following changes were made relative to the Last Call Working draft of 10 October 2013:

- knockout was removed from the non-normative section
- removed paragraph on SVG from simple alpha compositing
- updated abstract to include CSS and clarify intent
- removed clip-to-self and its references
- Changed section 5-10 to be normative + clarified notes/examples in those sections

The following changes were made relative to the Working Draft of 2013-06-25:

- clipping was removed as one of the operators that creates an isolated group in SVG
- background-blend-mode was changed so it matches repeating behavior of other background syntaxes.
- The mix-composite property was removed
- all open issues were resolved

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-masking-1"></a>[CSS-MASKING-1] **
: Dirk Schulze; Brian Birtles; Tab Atkins Jr.. [CSS Masking Module Level 1](css-masking-1.md). URL: [https://drafts.csswg.org/css-masking-1/](css-masking-1.md)
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css21"></a>[CSS21] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3bg"></a>[CSS3BG] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-svg11"></a>[SVG11] **
: Erik Dahlström; et al. [Scalable Vector Graphics (SVG) 1.1 (Second Edition)](svg11/index.md). 16 August 2011. REC. URL: [https://www.w3.org/TR/SVG11/](svg11/index.md)
### <a id="informative"></a>Non-Normative References

**<a id="biblio-porterduff"></a>[PORTERDUFF] **
: Thomas Porter; Tom Duff. Compositing digital images. July 1984.
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order | Com­puted value |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [background-blend-mode](#propdef-background-blend-mode) | <blend-mode># | normal | All HTML elements | no | N/A | discrete | per grammar | as specified | visual |
| [isolation](#propdef-isolation) | <isolation-mode> | auto | All elements. In SVG, it applies to container elements, graphics elements and graphics referencing elements. [SVG11] | no | N/A | discrete | per grammar | as specified | visual |
| [mix-blend-mode](#propdef-mix-blend-mode) | <blend-mode> | normal | All elements. In SVG, it applies to container elements, graphics elements and graphics referencing elements. [SVG11] | no | N/A | discrete | per grammar | as specified | visual |

**✔**MDN

**✔**MDN

**✔**MDN

**✔**MDN
