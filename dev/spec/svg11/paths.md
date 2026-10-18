<!-- https://www.w3.org/TR/SVG11/paths.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](coords.md) ⋅ [Next](shapes.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 8 Paths

## <a id="Introduction"></a>8.1 Introduction

Paths represent the outline of a shape which can be filled, stroked, used as a clipping path, or any combination of the three. (See [Filling, Stroking and Paint Servers](painting.md) and [Clipping, Masking and Compositing](masking.md).)

A path is described using the concept of a current point. In an analogy with drawing on paper, the current point can be thought of as the location of the pen. The position of the pen can be changed, and the outline of a shape (open or closed) can be traced by dragging the pen in either straight lines or curves.

Paths represent the geometry of the outline of an object, defined in terms of *moveto* (set a new current point), *lineto* (draw a straight line), *curveto* (draw a curve using a cubic Bézier), *arc* (elliptical or circular arc) and *closepath* (close the current shape by drawing a line to the last *moveto*) elements. Compound paths (i.e., a path with multiple subpaths) are possible to allow effects such as "donut holes" in objects.

This chapter describes the syntax, behavior and DOM interfaces for SVG paths. Various implementation notes for SVG paths can be found in [‘path’ element implementation notes](implnote.md#PathElementImplementationNotes) and [Elliptical arc implementation notes](implnote.md#ArcImplementationNotes).

A path is defined in SVG using the [‘path’](#PathElement) element.

## <a id="PathElement"></a>8.2 The ‘path’ element

‘path’

**Categories:**
: [Graphics element](intro.md#TermGraphicsElement), [shape element](intro.md#TermShapeElement)
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
- [‘d’](#DAttribute)
- [‘pathLength’](#PathLengthAttribute)

**DOM Interfaces:**
:

- [SVGPathElement](#InterfaceSVGPathElement)

*Attribute definitions:*

**<a id="DAttribute"></a>d = "*path data*"**
: The definition of the outline of a shape. See [Path data](#PathData). [Animatable](animate.md#Animatable): yes. Path data animation is only possible when each path data specification within an animation specification has exactly the same list of path data commands as the [‘d’](#DAttribute) attribute. If an animation is specified and the list of path data commands is not the same, then the animation specification is in error (see [Error Processing](implnote.md#ErrorProcessing)). The animation engine interpolates each parameter to each path data command separately based on the attributes to the given animation element. Flags and booleans are interpolated as fractions between zero and one, with any non-zero value considered to be a value of one/true.
**<a id="PathLengthAttribute"></a>pathLength = "[<number>](types.md#DataTypeNumber)"**
: The author's computation of the total length of the path, in user units. This value is used to calibrate the user agent's own [distance-along-a-path](#DistanceAlongAPath) calculations with that of the author. The user agent will scale all distance-along-a-path computations by the ratio of [‘pathLength’](#PathLengthAttribute) to the user agent's own computed value for total path length. [‘pathLength’](#PathLengthAttribute) potentially affects calculations for [text on a path](text.md#TextOnAPath), [motion animation](animate.md#AnimateMotionElement) and various [stroke operations](painting.md#StrokeProperties). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). [Animatable](animate.md#Animatable): yes.
## <a id="PathData"></a>8.3 Path data

### <a id="PathDataGeneralInformation"></a>8.3.1 General information about path data

A path is defined by including a [‘path’](#PathElement) element which contains a d="(path data)" attribute, where the [‘d’](#DAttribute) attribute contains the *moveto*, *line*, *curve* (both cubic and quadratic Béziers), *arc* and *closepath* instructions.

Example triangle01 specifies a path in the shape of a triangle. (The **M** indicates a *moveto*, the **L**s indicate *lineto*s, and the **z** indicates a *closepath*).

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="4cm" height="4cm" viewBox="0 0 400 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>Example triangle01- simple example of a 'path'</title>
  <desc>A path that draws a triangle</desc>
  <rect x="1" y="1" width="398" height="398"
        fill="none" stroke="blue" />
  <path d="M 100 100 L 300 100 L 200 300 z"
        fill="red" stroke="blue" stroke-width="3" />
</svg>
```

Example triangle01

| [image: Example triangle01 — simple example of a 'path'] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/paths/triangle01.svg)

Path data can contain newline characters and thus can be broken up into multiple lines to improve readability. Because of line length limitations with certain related tools, it is recommended that SVG generators split long path data strings across multiple lines, with each line not exceeding 255 characters. Also note that newline characters are only allowed at certain places within path data.

The syntax of path data is concise in order to allow for minimal file size and efficient downloads, since many SVG files will be dominated by their path data. Some of the ways that SVG attempts to minimize the size of path data are as follows:

- All instructions are expressed as one character (e.g., a *moveto* is expressed as an **M**).
- Superfluous white space and separators such as commas can be eliminated (e.g., "M 100 100 L 200 200" contains unnecessary spaces and could be expressed more compactly as "M100 100L200 200").
- The command letter can be eliminated on subsequent commands if the same command is used multiple times in a row (e.g., you can drop the second "L" in "M 100 200 L 200 100 L -100 -200" and use "M 100 200 L 200 100 -100 -200" instead).
- Relative versions of all commands are available (uppercase means absolute coordinates, lowercase means relative coordinates).
- Alternate forms of *lineto* are available to optimize the special cases of horizontal and vertical lines (absolute and relative).
- Alternate forms of *curve* are available to optimize the special cases where some of the control points on the current segment can be determined automatically from the control points on the previous segment.

The path data syntax is a prefix notation (i.e., commands followed by parameters). The only allowable decimal point is a Unicode U+0046 FULL STOP (".") character (also referred to in Unicode as PERIOD, dot and decimal point) and no other delimiter characters are allowed [[UNICODE](refs.md#ref-UNICODE)]. (For example, the following is an invalid numeric value in a path data stream: "13,000.56". Instead, say: "13000.56".)

For the relative versions of the commands, all coordinate values are relative to the current point at the start of the command.

In the tables below, the following notation is used:

- (): grouping of parameters
- +: 1 or more of the given parameter(s) is required

The following sections list the commands.

### <a id="PathDataMovetoCommands"></a>8.3.2 The **"moveto"** commands

The "moveto" commands (**M** or **m**) establish a new current point. The effect is as if the "pen" were lifted and moved to a new location. A path data segment (if there is one) must begin with a "moveto" command. Subsequent "moveto" commands (i.e., when the "moveto" is not the first command) represent the start of a new *subpath*:

| Command | Name | Parameters | Description |
| --- | --- | --- | --- |
| **M** (absolute) **m** (relative) | moveto | (x y)+ | Start a new sub-path at the given (x,y) coordinate. **M** (uppercase) indicates that absolute coordinates will follow; **m** (lowercase) indicates that relative coordinates will follow. If a moveto is followed by multiple pairs of coordinates, the subsequent pairs are treated as implicit lineto commands. Hence, implicit lineto commands will be relative if the moveto is relative, and absolute if the moveto is absolute. If a relative moveto (**m**) appears as the first element of the path, then it is treated as a pair of absolute coordinates. In this case, subsequent pairs of coordinates are treated as relative even though the initial moveto is interpreted as an absolute moveto. |

### <a id="PathDataClosePathCommand"></a>8.3.3 The **"closepath"** command

The "closepath" (**Z** or **z**) ends the current subpath and causes an automatic straight line to be drawn from the current point to the initial point of the current subpath. If a "closepath" is followed immediately by a "moveto", then the "moveto" identifies the start point of the next subpath. If a "closepath" is followed immediately by any other command, then the next subpath starts at the same initial point as the current subpath.

When a subpath ends in a "closepath," it differs in behavior from what happens when "manually" closing a subpath via a "lineto" command in how [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty) and [‘stroke-linecap’](painting.md#StrokeLinecapProperty) are implemented. With "closepath", the end of the final segment of the subpath is "joined" with the start of the initial segment of the subpath using the current value of [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty). If you instead "manually" close the subpath via a "lineto" command, the start of the first segment and the end of the last segment are not joined but instead are each capped using the current value of [‘stroke-linecap’](painting.md#StrokeLinecapProperty). At the end of the command, the new current point is set to the initial point of the current subpath.

| Command | Name | Parameters | Description |
| --- | --- | --- | --- |
| **Z** or **z** | closepath | (none) | Close the current subpath by drawing a straight line from the current point to current subpath's initial point. Since the Z and z commands take no parameters, they have an identical effect. |

### <a id="PathDataLinetoCommands"></a>8.3.4 The **"lineto"** commands

The various "lineto" commands draw straight lines from the current point to a new point:

| Command | Name | Parameters | Description |
| --- | --- | --- | --- |
| **L** (absolute) **l** (relative) | lineto | (x y)+ | Draw a line from the current point to the given (x,y) coordinate which becomes the new current point. **L** (uppercase) indicates that absolute coordinates will follow; **l** (lowercase) indicates that relative coordinates will follow. A number of coordinates pairs may be specified to draw a polyline. At the end of the command, the new current point is set to the final set of coordinates provided. |
| **H** (absolute) **h** (relative) | horizontal lineto | x+ | Draws a horizontal line from the current point (cpx, cpy) to (x, cpy). **H** (uppercase) indicates that absolute coordinates will follow; **h** (lowercase) indicates that relative coordinates will follow. Multiple x values can be provided (although usually this doesn't make sense). At the end of the command, the new current point becomes (x, cpy) for the final value of x. |
| **V** (absolute) **v** (relative) | vertical lineto | y+ | Draws a vertical line from the current point (cpx, cpy) to (cpx, y). **V** (uppercase) indicates that absolute coordinates will follow; **v** (lowercase) indicates that relative coordinates will follow. Multiple y values can be provided (although usually this doesn't make sense). At the end of the command, the new current point becomes (cpx, y) for the final value of y. |

### <a id="PathDataCurveCommands"></a>8.3.5 The curve commands

These three groups of commands draw curves:

- [Cubic Bézier commands](#PathDataCubicBezierCommands) (**C**, **c**, **S** and **s**). A cubic Bézier segment is defined by a start point, an end point, and two control points.
- [Quadratic Bézier commands](#PathDataQuadraticBezierCommands) (**Q**, **q**, **T** and **t**). A quadratic Bézier segment is defined by a start point, an end point, and one control point.
- [Elliptical arc commands](#PathDataEllipticalArcCommands) (**A** and **a**). An elliptical arc segment draws a segment of an ellipse.

### <a id="PathDataCubicBezierCommands"></a>8.3.6 The cubic Bézier curve commands

The cubic Bézier commands are as follows:

| Command | Name | Parameters | Description |
| --- | --- | --- | --- |
| **C** (absolute) **c** (relative) | curveto | (x1 y1 x2 y2 x y)+ | Draws a cubic Bézier curve from the current point to (x,y) using (x1,y1) as the control point at the beginning of the curve and (x2,y2) as the control point at the end of the curve. **C** (uppercase) indicates that absolute coordinates will follow; **c** (lowercase) indicates that relative coordinates will follow. Multiple sets of coordinates may be specified to draw a polybézier. At the end of the command, the new current point becomes the final (x,y) coordinate pair used in the polybézier. |
| **S** (absolute) **s** (relative) | shorthand/smooth curveto | (x2 y2 x y)+ | Draws a cubic Bézier curve from the current point to (x,y). The first control point is assumed to be the reflection of the second control point on the previous command relative to the current point. (If there is no previous command or if the previous command was not an C, c, S or s, assume the first control point is coincident with the current point.) (x2,y2) is the second control point (i.e., the control point at the end of the curve). **S** (uppercase) indicates that absolute coordinates will follow; **s** (lowercase) indicates that relative coordinates will follow. Multiple sets of coordinates may be specified to draw a polybézier. At the end of the command, the new current point becomes the final (x,y) coordinate pair used in the polybézier. |

Example cubic01 shows some simple uses of cubic Bézier commands within a path. The example uses an internal CSS style sheet to assign styling properties. Note that the control point for the "S" command is computed automatically as the reflection of the control point for the previous "C" command relative to the start point of the "S" command.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="5cm" height="4cm" viewBox="0 0 500 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>Example cubic01- cubic Bézier commands in path data</title>
  <desc>Picture showing a simple example of path data
        using both a "C" and an "S" command,
        along with annotations showing the control points
        and end points</desc>
  <style type="text/css"><![CDATA[
    .Border { fill:none; stroke:blue; stroke-width:1 }
    .Connect { fill:none; stroke:#888888; stroke-width:2 }
    .SamplePath { fill:none; stroke:red; stroke-width:5 }
    .EndPoint { fill:none; stroke:#888888; stroke-width:2 }
    .CtlPoint { fill:#888888; stroke:none }
    .AutoCtlPoint { fill:none; stroke:blue; stroke-width:4 }
    .Label { font-size:22; font-family:Verdana }
  ]]></style>

  <rect class="Border" x="1" y="1" width="498" height="398" />

  <polyline class="Connect" points="100,200 100,100" />
  <polyline class="Connect" points="250,100 250,200" />
  <polyline class="Connect" points="250,200 250,300" />
  <polyline class="Connect" points="400,300 400,200" />
  <path class="SamplePath" d="M100,200 C100,100 250,100 250,200
                                       S400,300 400,200" />
  <circle class="EndPoint" cx="100" cy="200" r="10" />
  <circle class="EndPoint" cx="250" cy="200" r="10" />
  <circle class="EndPoint" cx="400" cy="200" r="10" />
  <circle class="CtlPoint" cx="100" cy="100" r="10" />
  <circle class="CtlPoint" cx="250" cy="100" r="10" />
  <circle class="CtlPoint" cx="400" cy="300" r="10" />
  <circle class="AutoCtlPoint" cx="250" cy="300" r="9" />
  <text class="Label" x="25" y="70">M100,200 C100,100 250,100 250,200</text>
  <text class="Label" x="325" y="350"
        style="text-anchor:middle">S400,300 400,200</text>
</svg>
```

Example cubic01

| [image: Example cubic01 — cubic Bézier comamnds in path data] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/paths/cubic01.svg)

The following picture shows some how cubic Bézier curves change their shape depending on the position of the control points. The first five examples illustrate a single cubic Bézier path segment. The example at the lower right shows a "C" command followed by an "S" command.

[image: Example cubic02 - cubic Bézier commands in path data]

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/paths/cubic02.svg)

### <a id="PathDataQuadraticBezierCommands"></a>8.3.7 The quadratic Bézier curve commands

The quadratic Bézier commands are as follows:

| Command | Name | Parameters | Description |
| --- | --- | --- | --- |
| **Q** (absolute) **q** (relative) | quadratic Bézier curveto | (x1 y1 x y)+ | Draws a quadratic Bézier curve from the current point to (x,y) using (x1,y1) as the control point. **Q** (uppercase) indicates that absolute coordinates will follow; **q** (lowercase) indicates that relative coordinates will follow. Multiple sets of coordinates may be specified to draw a polybézier. At the end of the command, the new current point becomes the final (x,y) coordinate pair used in the polybézier. |
| **T** (absolute) **t** (relative) | Shorthand/smooth quadratic Bézier curveto | (x y)+ | Draws a quadratic Bézier curve from the current point to (x,y). The control point is assumed to be the reflection of the control point on the previous command relative to the current point. (If there is no previous command or if the previous command was not a Q, q, T or t, assume the control point is coincident with the current point.) **T** (uppercase) indicates that absolute coordinates will follow; **t** (lowercase) indicates that relative coordinates will follow. At the end of the command, the new current point becomes the final (x,y) coordinate pair used in the polybézier. |

Example quad01 shows some simple uses of quadratic Bézier commands within a path. Note that the control point for the "T" command is computed automatically as the reflection of the control point for the previous "Q" command relative to the start point of the "T" command.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="6cm" viewBox="0 0 1200 600"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>Example quad01 - quadratic Bézier commands in path data</title>
  <desc>Picture showing a "Q" a "T" command,
        along with annotations showing the control points
        and end points</desc>
  <rect x="1" y="1" width="1198" height="598"
        fill="none" stroke="blue" stroke-width="1" />

  <path d="M200,300 Q400,50 600,300 T1000,300"
        fill="none" stroke="red" stroke-width="5"  />
  <!-- End points -->
  <g fill="black" >
    <circle cx="200" cy="300" r="10"/>
    <circle cx="600" cy="300" r="10"/>
    <circle cx="1000" cy="300" r="10"/>
  </g>
  <!-- Control points and lines from end points to control points -->
  <g fill="#888888" >
    <circle cx="400" cy="50" r="10"/>
    <circle cx="800" cy="550" r="10"/>
  </g>
  <path d="M200,300 L400,50 L600,300 
           L800,550 L1000,300"
        fill="none" stroke="#888888" stroke-width="2" />
</svg>
```

Example quad01

| [image: Example quad01 — quadratic Bézier commands in path data] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/paths/quad01.svg)

### <a id="PathDataEllipticalArcCommands"></a>8.3.8 The elliptical arc curve commands

The elliptical arc commands are as follows:

| Command | Name | Parameters | Description |
| --- | --- | --- | --- |
| **A** (absolute) **a** (relative) | elliptical arc | (rx ry x-axis-rotation large-arc-flag sweep-flag x y)+ | Draws an elliptical arc from the current point to (**x**, **y**). The size and orientation of the ellipse are defined by two radii (**rx**, **ry**) and an **x-axis-rotation**, which indicates how the ellipse as a whole is rotated relative to the current coordinate system. The center (**cx**, **cy**) of the ellipse is calculated automatically to satisfy the constraints imposed by the other parameters. **large-arc-flag** and **sweep-flag** contribute to the automatic calculations and help determine how the arc is drawn. |

Example arcs01 shows some simple uses of arc commands within a path.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="12cm" height="5.25cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <title>Example arcs01 - arc commands in path data</title>
  <desc>Picture of a pie chart with two pie wedges and
        a picture of a line with arc blips</desc>
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="1" />

  <path d="M300,200 h-150 a150,150 0 1,0 150,-150 z"
        fill="red" stroke="blue" stroke-width="5" />
  <path d="M275,175 v-150 a150,150 0 0,0 -150,150 z"
        fill="yellow" stroke="blue" stroke-width="5" />

  <path d="M600,350 l 50,-25 
           a25,25 -30 0,1 50,-25 l 50,-25 
           a25,50 -30 0,1 50,-25 l 50,-25 
           a25,75 -30 0,1 50,-25 l 50,-25 
           a25,100 -30 0,1 50,-25 l 50,-25"
        fill="none" stroke="red" stroke-width="5"  />
</svg>
```

Example arcs01

| [image: Example arcs01 — arc commands in path data] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/paths/arcs01.svg)

The elliptical arc command draws a section of an ellipse which meets the following constraints:

- the arc starts at the current point
- the arc ends at point (**x**, **y**)
- the ellipse has the two radii (**rx**, **ry**)
- the x-axis of the ellipse is rotated by **x-axis-rotation** relative to the x-axis of the current coordinate system.

For most situations, there are actually four different arcs (two different ellipses, each with two different arc sweeps) that satisfy these constraints. **large-arc-flag** and **sweep-flag** indicate which one of the four arcs are drawn, as follows:

- Of the four candidate arc sweeps, two will represent an arc sweep of greater than or equal to 180 degrees (the "large-arc"), and two will represent an arc sweep of less than or equal to 180 degrees (the "small-arc"). If **large-arc-flag** is '1', then one of the two larger arc sweeps will be chosen; otherwise, if **large-arc-flag** is '0', one of the smaller arc sweeps will be chosen,
- If **sweep-flag** is '1', then the arc will be drawn in a "positive-angle" direction (i.e., the ellipse formula x=**cx**+**rx***cos(theta) and y=**cy**+**ry***sin(theta) is evaluated such that theta starts at an angle corresponding to the current point and increases positively until the arc reaches (x,y)). A value of 0 causes the arc to be drawn in a "negative-angle" direction (i.e., theta starts at an angle value corresponding to the current point and decreases until the arc reaches (x,y)).

The following illustrates the four combinations of **large-arc-flag** and **sweep-flag** and the four different arcs that will be drawn based on the values of these flags. For each case, the following path data command was used:

```
<path d="M 125,75 a100,50 0 ?,? 100,50"
      style="fill:none; stroke:red; stroke-width:6"/>
```

where "?,?" is replaced by "0,0" "0,1" "1,0" and "1,1" to generate the four possible cases.

[image: Illustration of flags in arc commands]

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/paths/arcs02.svg)

Refer to [Elliptical arc implementation notes](implnote.md#ArcImplementationNotes) for detailed implementation notes for the path data elliptical arc commands.

### <a id="PathDataBNF"></a>8.3.9 The grammar for path data

The following notation is used in the Backus-Naur Form (BNF) description of the grammar for path data:

- *: 0 or more
- +: 1 or more
- ?: 0 or 1
- (): grouping
- |: separates alternatives
- double quotes surround literals

The following is the BNF for SVG paths.

```
svg-path:
    wsp* moveto-drawto-command-groups? wsp*
moveto-drawto-command-groups:
    moveto-drawto-command-group
    | moveto-drawto-command-group wsp* moveto-drawto-command-groups
moveto-drawto-command-group:
    moveto wsp* drawto-commands?
drawto-commands:
    drawto-command
    | drawto-command wsp* drawto-commands
drawto-command:
    closepath
    | lineto
    | horizontal-lineto
    | vertical-lineto
    | curveto
    | smooth-curveto
    | quadratic-bezier-curveto
    | smooth-quadratic-bezier-curveto
    | elliptical-arc
moveto:
    ( "M" | "m" ) wsp* moveto-argument-sequence
moveto-argument-sequence:
    coordinate-pair
    | coordinate-pair comma-wsp? lineto-argument-sequence
closepath:
    ("Z" | "z")
lineto:
    ( "L" | "l" ) wsp* lineto-argument-sequence
lineto-argument-sequence:
    coordinate-pair
    | coordinate-pair comma-wsp? lineto-argument-sequence
horizontal-lineto:
    ( "H" | "h" ) wsp* horizontal-lineto-argument-sequence
horizontal-lineto-argument-sequence:
    coordinate
    | coordinate comma-wsp? horizontal-lineto-argument-sequence
vertical-lineto:
    ( "V" | "v" ) wsp* vertical-lineto-argument-sequence
vertical-lineto-argument-sequence:
    coordinate
    | coordinate comma-wsp? vertical-lineto-argument-sequence
curveto:
    ( "C" | "c" ) wsp* curveto-argument-sequence
curveto-argument-sequence:
    curveto-argument
    | curveto-argument comma-wsp? curveto-argument-sequence
curveto-argument:
    coordinate-pair comma-wsp? coordinate-pair comma-wsp? coordinate-pair
smooth-curveto:
    ( "S" | "s" ) wsp* smooth-curveto-argument-sequence
smooth-curveto-argument-sequence:
    smooth-curveto-argument
    | smooth-curveto-argument comma-wsp? smooth-curveto-argument-sequence
smooth-curveto-argument:
    coordinate-pair comma-wsp? coordinate-pair
quadratic-bezier-curveto:
    ( "Q" | "q" ) wsp* quadratic-bezier-curveto-argument-sequence
quadratic-bezier-curveto-argument-sequence:
    quadratic-bezier-curveto-argument
    | quadratic-bezier-curveto-argument comma-wsp? 
        quadratic-bezier-curveto-argument-sequence
quadratic-bezier-curveto-argument:
    coordinate-pair comma-wsp? coordinate-pair
smooth-quadratic-bezier-curveto:
    ( "T" | "t" ) wsp* smooth-quadratic-bezier-curveto-argument-sequence
smooth-quadratic-bezier-curveto-argument-sequence:
    coordinate-pair
    | coordinate-pair comma-wsp? smooth-quadratic-bezier-curveto-argument-sequence
elliptical-arc:
    ( "A" | "a" ) wsp* elliptical-arc-argument-sequence
elliptical-arc-argument-sequence:
    elliptical-arc-argument
    | elliptical-arc-argument comma-wsp? elliptical-arc-argument-sequence
elliptical-arc-argument:
    nonnegative-number comma-wsp? nonnegative-number comma-wsp? 
        number comma-wsp flag comma-wsp? flag comma-wsp? coordinate-pair
coordinate-pair:
    coordinate comma-wsp? coordinate
coordinate:
    number
nonnegative-number:
    integer-constant
    | floating-point-constant
number:
    sign? integer-constant
    | sign? floating-point-constant
flag:
    "0" | "1"
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
    (#x20 | #x9 | #xD | #xA)
```

The processing of the BNF must consume as much of a given BNF production as possible, stopping at the point when a character is encountered which no longer satisfies the production. Thus, in the string "M 100-200", the first coordinate for the "moveto" consumes the characters "100" and stops upon encountering the minus sign because the minus sign cannot follow a digit in the production of a "coordinate". The result is that the first coordinate will be "100" and the second coordinate will be "-200".

Similarly, for the string "M 0.6.5", the first coordinate of the "moveto" consumes the characters "0.6" and stops upon encountering the second decimal point because the production of a "coordinate" only allows one decimal point. The result is that the first coordinate will be "0.6" and the second coordinate will be ".5".

Note that the BNF allows the path [‘d’](#DAttribute) attribute to be empty. This is not an error, instead it disables rendering of the path.

## <a id="DistanceAlongAPath"></a>8.4 Distance along a path

Various operations, including [text on a path](text.md#TextOnAPath) and [motion animation](animate.md#AnimateMotionElement) and various [stroke operations](painting.md#StrokeProperties), require that the user agent compute the distance along the geometry of a graphics element, such as a [‘path’](#PathElement).

Exact mathematics exist for computing distance along a path, but the formulas are highly complex and require substantial computation. It is recommended that authoring products and user agents employ algorithms that produce as precise results as possible; however, to accommodate implementation differences and to help distance calculations produce results that approximate author intent, the [‘pathLength’](#PathLengthAttribute) attribute can be used to provide the author's computation of the total length of the path so that the user agent can scale distance-along-a-path computations by the ratio of [‘pathLength’](#PathLengthAttribute) to the user agent's own computed value for total path length.

A "moveto" operation within a [‘path’](#PathElement) element is defined to have zero length. Only the various "lineto", "curveto" and "arcto" commands contribute to path length calculations.

## <a id="DOMInterfaces"></a>8.5 DOM interfaces

### <a id="InterfaceSVGPathSeg"></a>8.5.1 Interface SVGPathSeg

The [SVGPathSeg](#InterfaceSVGPathSeg) interface is a base interface that corresponds to a single command within a path data specification.

```
interface SVGPathSeg {

  // Path Segment Types
  const unsigned short PATHSEG_UNKNOWN = 0;
  const unsigned short PATHSEG_CLOSEPATH = 1;
  const unsigned short PATHSEG_MOVETO_ABS = 2;
  const unsigned short PATHSEG_MOVETO_REL = 3;
  const unsigned short PATHSEG_LINETO_ABS = 4;
  const unsigned short PATHSEG_LINETO_REL = 5;
  const unsigned short PATHSEG_CURVETO_CUBIC_ABS = 6;
  const unsigned short PATHSEG_CURVETO_CUBIC_REL = 7;
  const unsigned short PATHSEG_CURVETO_QUADRATIC_ABS = 8;
  const unsigned short PATHSEG_CURVETO_QUADRATIC_REL = 9;
  const unsigned short PATHSEG_ARC_ABS = 10;
  const unsigned short PATHSEG_ARC_REL = 11;
  const unsigned short PATHSEG_LINETO_HORIZONTAL_ABS = 12;
  const unsigned short PATHSEG_LINETO_HORIZONTAL_REL = 13;
  const unsigned short PATHSEG_LINETO_VERTICAL_ABS = 14;
  const unsigned short PATHSEG_LINETO_VERTICAL_REL = 15;
  const unsigned short PATHSEG_CURVETO_CUBIC_SMOOTH_ABS = 16;
  const unsigned short PATHSEG_CURVETO_CUBIC_SMOOTH_REL = 17;
  const unsigned short PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS = 18;
  const unsigned short PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL = 19;

  readonly attribute unsigned short pathSegType;
  readonly attribute DOMString pathSegTypeAsLetter;
};
```

**Constants in group “Path Segment Types”:**
:

**<a id="__svg__SVGPathSeg__PATHSEG_UNKNOWN"></a>**PATHSEG_UNKNOWN** (unsigned short)**
:

The unit type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGPathSeg__PATHSEG_CLOSEPATH"></a>**PATHSEG_CLOSEPATH** (unsigned short)**
:

Corresponds to a "closepath" (z) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_MOVETO_ABS"></a>**PATHSEG_MOVETO_ABS** (unsigned short)**
:

Corresponds to a "absolute moveto" (M) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_MOVETO_REL"></a>**PATHSEG_MOVETO_REL** (unsigned short)**
:

Corresponds to a "relative moveto" (m) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_LINETO_ABS"></a>**PATHSEG_LINETO_ABS** (unsigned short)**
:

Corresponds to a "absolute lineto" (L) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_LINETO_REL"></a>**PATHSEG_LINETO_REL** (unsigned short)**
:

Corresponds to a "relative lineto" (l) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_CUBIC_ABS"></a>**PATHSEG_CURVETO_CUBIC_ABS** (unsigned short)**
:

Corresponds to a "absolute cubic Bézier curveto" (C) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_CUBIC_REL"></a>**PATHSEG_CURVETO_CUBIC_REL** (unsigned short)**
:

Corresponds to a "relative cubic Bézier curveto" (c) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_QUADRATIC_ABS"></a>**PATHSEG_CURVETO_QUADRATIC_ABS** (unsigned short)**
:

Corresponds to a "absolute quadratic Bézier curveto" (Q) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_QUADRATIC_REL"></a>**PATHSEG_CURVETO_QUADRATIC_REL** (unsigned short)**
:

Corresponds to a "relative quadratic Bézier curveto" (q) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_ARC_ABS"></a>**PATHSEG_ARC_ABS** (unsigned short)**
:

Corresponds to a "absolute arcto" (A) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_ARC_REL"></a>**PATHSEG_ARC_REL** (unsigned short)**
:

Corresponds to a "relative arcto" (a) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_LINETO_HORIZONTAL_ABS"></a>**PATHSEG_LINETO_HORIZONTAL_ABS** (unsigned short)**
:

Corresponds to a "absolute horizontal lineto" (H) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_LINETO_HORIZONTAL_REL"></a>**PATHSEG_LINETO_HORIZONTAL_REL** (unsigned short)**
:

Corresponds to a "relative horizontal lineto" (h) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_LINETO_VERTICAL_ABS"></a>**PATHSEG_LINETO_VERTICAL_ABS** (unsigned short)**
:

Corresponds to a "absolute vertical lineto" (V) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_LINETO_VERTICAL_REL"></a>**PATHSEG_LINETO_VERTICAL_REL** (unsigned short)**
:

Corresponds to a "relative vertical lineto" (v) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_CUBIC_SMOOTH_ABS"></a>**PATHSEG_CURVETO_CUBIC_SMOOTH_ABS** (unsigned short)**
:

Corresponds to a "absolute smooth cubic curveto" (S) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_CUBIC_SMOOTH_REL"></a>**PATHSEG_CURVETO_CUBIC_SMOOTH_REL** (unsigned short)**
:

Corresponds to a "relative smooth cubic curveto" (s) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS"></a>**PATHSEG_CURVETO_QUADRATIC_SMOOTH_ABS** (unsigned short)**
:

Corresponds to a "absolute smooth quadratic curveto" (T) path data command.

**<a id="__svg__SVGPathSeg__PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL"></a>**PATHSEG_CURVETO_QUADRATIC_SMOOTH_REL** (unsigned short)**
:

Corresponds to a "relative smooth quadratic curveto" (t) path data command.

**Attributes:**
:

**<a id="__svg__SVGPathSeg__pathSegType"></a>**pathSegType** (readonly unsigned short)**
:

The type of the path segment as specified by one of the constants defined on this interface.

**<a id="__svg__SVGPathSeg__pathSegTypeAsLetter"></a>**pathSegTypeAsLetter** (readonly DOMString)**
:

The type of the path segment, specified by the corresponding one character command name.

### <a id="InterfaceSVGPathSegClosePath"></a>8.5.2 Interface SVGPathSegClosePath

The [SVGPathSegClosePath](#InterfaceSVGPathSegClosePath) interface corresponds to a "closepath" (z) path data command.

```
interface SVGPathSegClosePath : SVGPathSeg {
};
```

### <a id="InterfaceSVGPathSegMovetoAbs"></a>8.5.3 Interface SVGPathSegMovetoAbs

The [SVGPathSegMovetoAbs](#InterfaceSVGPathSegMovetoAbs) interface corresponds to an "absolute moveto" (M) path data command.

```
interface SVGPathSegMovetoAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegMovetoAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegMovetoAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegMovetoRel"></a>8.5.4 Interface SVGPathSegMovetoRel

The [SVGPathSegMovetoRel](#InterfaceSVGPathSegMovetoRel) interface corresponds to a "relative moveto" (m) path data command.

```
interface SVGPathSegMovetoRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegMovetoRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegMovetoRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegLinetoAbs"></a>8.5.5 Interface SVGPathSegLinetoAbs

The [SVGPathSegLinetoAbs](#InterfaceSVGPathSegLinetoAbs) interface corresponds to an "absolute lineto" (L) path data command.

```
interface SVGPathSegLinetoAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegLinetoAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegLinetoAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegLinetoRel"></a>8.5.6 Interface SVGPathSegLinetoRel

The [SVGPathSegLinetoRel](#InterfaceSVGPathSegLinetoRel) interface corresponds to a "relative lineto" (l) path data command.

```
interface SVGPathSegLinetoRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegLinetoRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegLinetoRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoCubicAbs"></a>8.5.7 Interface SVGPathSegCurvetoCubicAbs

The [SVGPathSegCurvetoCubicAbs](#InterfaceSVGPathSegCurvetoCubicAbs) interface corresponds to an "absolute cubic Bézier curveto" (C) path data command.

```
interface SVGPathSegCurvetoCubicAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float x1 setraises(DOMException);
  attribute float y1 setraises(DOMException);
  attribute float x2 setraises(DOMException);
  attribute float y2 setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoCubicAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicAbs__x1"></a>**x1** (float)**
:

The absolute X coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicAbs__y1"></a>**y1** (float)**
:

The absolute Y coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicAbs__x2"></a>**x2** (float)**
:

The absolute X coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicAbs__y2"></a>**y2** (float)**
:

The absolute Y coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoCubicRel"></a>8.5.8 Interface SVGPathSegCurvetoCubicRel

The [SVGPathSegCurvetoCubicRel](#InterfaceSVGPathSegCurvetoCubicRel) interface corresponds to a "relative cubic Bézier curveto" (c) path data command.

```
interface SVGPathSegCurvetoCubicRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float x1 setraises(DOMException);
  attribute float y1 setraises(DOMException);
  attribute float x2 setraises(DOMException);
  attribute float y2 setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoCubicRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicRel__x1"></a>**x1** (float)**
:

The relative X coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicRel__y1"></a>**y1** (float)**
:

The relative Y coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicRel__x2"></a>**x2** (float)**
:

The relative X coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicRel__y2"></a>**y2** (float)**
:

The relative Y coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoQuadraticAbs"></a>8.5.9 Interface SVGPathSegCurvetoQuadraticAbs

The [SVGPathSegCurvetoQuadraticAbs](#InterfaceSVGPathSegCurvetoQuadraticAbs) interface corresponds to an "absolute quadratic Bézier curveto" (Q) path data command.

```
interface SVGPathSegCurvetoQuadraticAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float x1 setraises(DOMException);
  attribute float y1 setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoQuadraticAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticAbs__x1"></a>**x1** (float)**
:

The absolute X coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticAbs__y1"></a>**y1** (float)**
:

The absolute Y coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoQuadraticRel"></a>8.5.10 Interface SVGPathSegCurvetoQuadraticRel

The [SVGPathSegCurvetoQuadraticRel](#InterfaceSVGPathSegCurvetoQuadraticRel) interface corresponds to a "relative quadratic Bézier curveto" (q) path data command.

```
interface SVGPathSegCurvetoQuadraticRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float x1 setraises(DOMException);
  attribute float y1 setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoQuadraticRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticRel__x1"></a>**x1** (float)**
:

The relative X coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticRel__y1"></a>**y1** (float)**
:

The relative Y coordinate for the first control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegArcAbs"></a>8.5.11 Interface SVGPathSegArcAbs

The [SVGPathSegArcAbs](#InterfaceSVGPathSegArcAbs) interface corresponds to an "absolute arcto" (A) path data command.

```
interface SVGPathSegArcAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float r1 setraises(DOMException);
  attribute float r2 setraises(DOMException);
  attribute float angle setraises(DOMException);
  attribute boolean largeArcFlag setraises(DOMException);
  attribute boolean sweepFlag setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegArcAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcAbs__r1"></a>**r1** (float)**
:

The x-axis radius for the ellipse (i.e., r1).

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcAbs__r2"></a>**r2** (float)**
:

The y-axis radius for the ellipse (i.e., r2).

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcAbs__angle"></a>**angle** (float)**
:

The rotation angle in degrees for the ellipse's x-axis relative to the x-axis of the user coordinate system.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcAbs__largeArcFlag"></a>**largeArcFlag** (boolean)**
:

The value of the large-arc-flag parameter.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcAbs__sweepFlag"></a>**sweepFlag** (boolean)**
:

The value of the sweep-flag parameter.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegArcRel"></a>8.5.12 Interface SVGPathSegArcRel

The [SVGPathSegArcRel](#InterfaceSVGPathSegArcRel) interface corresponds to a "relative arcto" (a) path data command.

```
interface SVGPathSegArcRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float r1 setraises(DOMException);
  attribute float r2 setraises(DOMException);
  attribute float angle setraises(DOMException);
  attribute boolean largeArcFlag setraises(DOMException);
  attribute boolean sweepFlag setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegArcRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcRel__r1"></a>**r1** (float)**
:

The x-axis radius for the ellipse (i.e., r1).

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcRel__r2"></a>**r2** (float)**
:

The y-axis radius for the ellipse (i.e., r2).

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcRel__angle"></a>**angle** (float)**
:

The rotation angle in degrees for the ellipse's x-axis relative to the x-axis of the user coordinate system.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcRel__largeArcFlag"></a>**largeArcFlag** (boolean)**
:

The value of the large-arc-flag parameter.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegArcRel__sweepFlag"></a>**sweepFlag** (boolean)**
:

The value of the sweep-flag parameter.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegLinetoHorizontalAbs"></a>8.5.13 Interface SVGPathSegLinetoHorizontalAbs

The [SVGPathSegLinetoHorizontalAbs](#InterfaceSVGPathSegLinetoHorizontalAbs) interface corresponds to an "absolute horizontal lineto" (H) path data command.

```
interface SVGPathSegLinetoHorizontalAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegLinetoHorizontalAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegLinetoHorizontalRel"></a>8.5.14 Interface SVGPathSegLinetoHorizontalRel

The [SVGPathSegLinetoHorizontalRel](#InterfaceSVGPathSegLinetoHorizontalRel) interface corresponds to a "relative horizontal lineto" (h) path data command.

```
interface SVGPathSegLinetoHorizontalRel : SVGPathSeg {
  attribute float x setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegLinetoHorizontalRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegLinetoVerticalAbs"></a>8.5.15 Interface SVGPathSegLinetoVerticalAbs

The [SVGPathSegLinetoVerticalAbs](#InterfaceSVGPathSegLinetoVerticalAbs) interface corresponds to an "absolute vertical lineto" (V) path data command.

```
interface SVGPathSegLinetoVerticalAbs : SVGPathSeg {
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegLinetoVerticalAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegLinetoVerticalRel"></a>8.5.16 Interface SVGPathSegLinetoVerticalRel

The [SVGPathSegLinetoVerticalRel](#InterfaceSVGPathSegLinetoVerticalRel) interface corresponds to a "relative vertical lineto" (v) path data command.

```
interface SVGPathSegLinetoVerticalRel : SVGPathSeg {
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegLinetoVerticalRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoCubicSmoothAbs"></a>8.5.17 Interface SVGPathSegCurvetoCubicSmoothAbs

The [SVGPathSegCurvetoCubicSmoothAbs](#InterfaceSVGPathSegCurvetoCubicSmoothAbs) interface corresponds to an "absolute smooth cubic curveto" (S) path data command.

```
interface SVGPathSegCurvetoCubicSmoothAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float x2 setraises(DOMException);
  attribute float y2 setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoCubicSmoothAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicSmoothAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicSmoothAbs__x2"></a>**x2** (float)**
:

The absolute X coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicSmoothAbs__y2"></a>**y2** (float)**
:

The absolute Y coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoCubicSmoothRel"></a>8.5.18 Interface SVGPathSegCurvetoCubicSmoothRel

The [SVGPathSegCurvetoCubicSmoothRel](#InterfaceSVGPathSegCurvetoCubicSmoothRel) interface corresponds to a "relative smooth cubic curveto" (s) path data command.

```
interface SVGPathSegCurvetoCubicSmoothRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
  attribute float x2 setraises(DOMException);
  attribute float y2 setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoCubicSmoothRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicSmoothRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicSmoothRel__x2"></a>**x2** (float)**
:

The relative X coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoCubicSmoothRel__y2"></a>**y2** (float)**
:

The relative Y coordinate for the second control point.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoQuadraticSmoothAbs"></a>8.5.19 Interface SVGPathSegCurvetoQuadraticSmoothAbs

The [SVGPathSegCurvetoQuadraticSmoothAbs](#InterfaceSVGPathSegCurvetoQuadraticSmoothAbs) interface corresponds to an "absolute smooth cubic curveto" (T) path data command.

```
interface SVGPathSegCurvetoQuadraticSmoothAbs : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoQuadraticSmoothAbs__x"></a>**x** (float)**
:

The absolute X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticSmoothAbs__y"></a>**y** (float)**
:

The absolute Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegCurvetoQuadraticSmoothRel"></a>8.5.20 Interface SVGPathSegCurvetoQuadraticSmoothRel

The [SVGPathSegCurvetoQuadraticSmoothRel](#InterfaceSVGPathSegCurvetoQuadraticSmoothRel) interface corresponds to a "relative smooth cubic curveto" (t) path data command.

```
interface SVGPathSegCurvetoQuadraticSmoothRel : SVGPathSeg {
  attribute float x setraises(DOMException);
  attribute float y setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegCurvetoQuadraticSmoothRel__x"></a>**x** (float)**
:

The relative X coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGPathSegCurvetoQuadraticSmoothRel__y"></a>**y** (float)**
:

The relative Y coordinate for the end point of this path segment.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
### <a id="InterfaceSVGPathSegList"></a>8.5.21 Interface SVGPathSegList

This interface defines a list of SVGPathSeg objects.

[SVGPathSegList](#InterfaceSVGPathSegList) has the same attributes and methods as other SVGxxxList interfaces. Implementers may consider using a single base class to implement the various SVGxxxList interfaces.

```
interface SVGPathSegList {

  readonly attribute unsigned long numberOfItems;

  void clear() raises(DOMException);
  SVGPathSeg initialize(in SVGPathSeg newItem) raises(DOMException);
  SVGPathSeg getItem(in unsigned long index) raises(DOMException);
  SVGPathSeg insertItemBefore(in SVGPathSeg newItem, in unsigned long index) raises(DOMException);
  SVGPathSeg replaceItem(in SVGPathSeg newItem, in unsigned long index) raises(DOMException);
  SVGPathSeg removeItem(in unsigned long index) raises(DOMException);
  SVGPathSeg appendItem(in SVGPathSeg newItem) raises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathSegList__numberOfItems"></a>**numberOfItems** (readonly unsigned long)**
:

The number of items in the list.

**Operations:**
:

**<a id="__svg__SVGPathSegList__clear"></a>void **clear**()**
:

Clears all existing current items from the list, with the result being an empty list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**<a id="__svg__SVGPathSegList__initialize"></a>[SVGPathSeg](#InterfaceSVGPathSeg) **initialize**(in [SVGPathSeg](#InterfaceSVGPathSeg) *newItem*)**
:

Clears all existing current items from the list and re-initializes the list to hold the single item specified by the parameter. If the inserted item is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy.

**Parameters**
:

1.
[SVGPathSeg](#InterfaceSVGPathSeg) *newItem*
The item which should become the only member of the list.

**Returns**
:

The item being inserted into the list.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**<a id="__svg__SVGPathSegList__getItem"></a>[SVGPathSeg](#InterfaceSVGPathSeg) **getItem**(in unsigned long *index*)**
:

Returns the specified item from the list. The returned item is the item itself and not a copy. Any changes made to the item are immediately reflected in the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item from the list which is to be returned. The first item is number 0.

**Returns**
:

The selected item.

**Exceptions**
:

**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGPathSegList__numberOfItems).
**<a id="__svg__SVGPathSegList__insertItemBefore"></a>[SVGPathSeg](#InterfaceSVGPathSeg) **insertItemBefore**(in [SVGPathSeg](#InterfaceSVGPathSeg) *newItem*, in unsigned long *index*)**
:

Inserts a new item into the list at the specified position. The first item is number 0. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy. If the item is already in this list, note that the index of the item to insert before is *before* the removal of the item.

**Parameters**
:

1.
[SVGPathSeg](#InterfaceSVGPathSeg) *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item before which the new item is to be inserted. The first item is number 0. If the index is equal to 0, then the new item is inserted at the front of the list. If the index is greater than or equal to [numberOfItems](#__svg__SVGPathSegList__numberOfItems), then the new item is appended to the end of the list.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**<a id="__svg__SVGPathSegList__replaceItem"></a>[SVGPathSeg](#InterfaceSVGPathSeg) **replaceItem**(in [SVGPathSeg](#InterfaceSVGPathSeg) *newItem*, in unsigned long *index*)**
:

Replaces an existing item in the list with a new item. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy. If the item is already in this list, note that the index of the item to replace is *before* the removal of the item.

**Parameters**
:

1.
[SVGPathSeg](#InterfaceSVGPathSeg) *newItem*
The item which is to be inserted into the list.
2.
unsigned long *index*
The index of the item which is to be replaced. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGPathSegList__numberOfItems).
**<a id="__svg__SVGPathSegList__removeItem"></a>[SVGPathSeg](#InterfaceSVGPathSeg) **removeItem**(in unsigned long *index*)**
:

Removes an existing item from the list.

**Parameters**
:

1.
unsigned long *index*
The index of the item which is to be removed. The first item is number 0.

**Returns**
:

The removed item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
**DOMException, code INDEX_SIZE_ERR**
: Raised if the index number is greater than or equal to [numberOfItems](#__svg__SVGPathSegList__numberOfItems).
**<a id="__svg__SVGPathSegList__appendItem"></a>[SVGPathSeg](#InterfaceSVGPathSeg) **appendItem**(in [SVGPathSeg](#InterfaceSVGPathSeg) *newItem*)**
:

Inserts a new item at the end of the list. If *newItem* is already in a list, it is removed from its previous list before it is inserted into this list. The inserted item is the item itself and not a copy.

**Parameters**
:

1.
[SVGPathSeg](#InterfaceSVGPathSeg) *newItem*
The item which is to be inserted. The first item is number 0.

**Returns**
:

The inserted item.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised when the list cannot be modified.
### <a id="InterfaceSVGAnimatedPathData"></a>8.5.22 Interface SVGAnimatedPathData

The SVGAnimatedPathData interface supports elements which have a ‘d’ attribute which holds SVG path data, and supports the ability to animate that attribute.

The [SVGAnimatedPathData](#InterfaceSVGAnimatedPathData) interface provides two lists to access and modify the base (i.e., static) contents of the ‘d’ attribute:

- DOM attribute [pathSegList](#__svg__SVGAnimatedPathData__pathSegList) provides access to the static/base contents of the ‘d’ attribute in a form which matches one-for-one with SVG's syntax.
- DOM attribute [normalizedPathSegList](#__svg__SVGAnimatedPathData__normalizedPathSegList) provides normalized access to the static/base contents of the ‘d’ attribute where all path data commands are expressed in terms of the following subset of [SVGPathSeg](#InterfaceSVGPathSeg) types: SVG_PATHSEG_MOVETO_ABS (M), SVG_PATHSEG_LINETO_ABS (L), SVG_PATHSEG_CURVETO_CUBIC_ABS (C) and SVG_PATHSEG_CLOSEPATH (z).

and two lists to access the current animated values of the ‘d’ attribute:

- DOM attribute [animatedPathSegList](#__svg__SVGAnimatedPathData__animatedPathSegList) provides access to the current animated contents of the ‘d’ attribute in a form which matches one-for-one with SVG's syntax.
- DOM attribute [animatedNormalizedPathSegList](#__svg__SVGAnimatedPathData__animatedNormalizedPathSegList) provides normalized access to the current animated contents of the ‘d’ attribute where all path data commands are expressed in terms of the following subset of [SVGPathSeg](#InterfaceSVGPathSeg) types: SVG_PATHSEG_MOVETO_ABS (M), SVG_PATHSEG_LINETO_ABS (L), SVG_PATHSEG_CURVETO_CUBIC_ABS (C) and SVG_PATHSEG_CLOSEPATH (z).

Each of the two lists are always kept synchronized. Modifications to one list will immediately cause the corresponding list to be modified. Modifications to [normalizedPathSegList](#__svg__SVGAnimatedPathData__normalizedPathSegList) might cause entries in [pathSegList](#__svg__SVGAnimatedPathData__pathSegList) to be broken into a set of normalized path segments.

Additionally, the [‘d’](#DAttribute) attribute on the [‘path’](#PathElement) element accessed via the XML DOM (e.g., using the `getAttribute()` method call) will reflect any changes made to [pathSegList](#__svg__SVGAnimatedPathData__pathSegList) or [normalizedPathSegList](#__svg__SVGAnimatedPathData__normalizedPathSegList).

```
interface SVGAnimatedPathData {
  readonly attribute SVGPathSegList pathSegList;
  readonly attribute SVGPathSegList normalizedPathSegList;
  readonly attribute SVGPathSegList animatedPathSegList;
  readonly attribute SVGPathSegList animatedNormalizedPathSegList;
};
```

**Attributes:**
:

**<a id="__svg__SVGAnimatedPathData__pathSegList"></a>**pathSegList** (readonly [SVGPathSegList](#InterfaceSVGPathSegList))**
:

Provides access to the base (i.e., static) contents of the ‘d’ attribute in a form which matches one-for-one with SVG's syntax. Thus, if the ‘d’ attribute has an "absolute moveto (M)" and an "absolute arcto (A)" command, then [pathSegList](#__svg__SVGAnimatedPathData__pathSegList) will have two entries: a SVG_PATHSEG_MOVETO_ABS and a SVG_PATHSEG_ARC_ABS.

**<a id="__svg__SVGAnimatedPathData__normalizedPathSegList"></a>**normalizedPathSegList** (readonly [SVGPathSegList](#InterfaceSVGPathSegList))**
:

Provides access to the base (i.e., static) contents of the ‘d’ attribute in a form where all path data commands are expressed in terms of the following subset of [SVGPathSeg](#InterfaceSVGPathSeg) types: SVG_PATHSEG_MOVETO_ABS (M), SVG_PATHSEG_LINETO_ABS (L), SVG_PATHSEG_CURVETO_CUBIC_ABS (C) and SVG_PATHSEG_CLOSEPATH (z). Thus, if the ‘d’ attribute has an "absolute moveto (M)" and an "absolute arcto (A)" command, then pathSegList will have one SVG_PATHSEG_MOVETO_ABS entry followed by a series of SVG_PATHSEG_LINETO_ABS entries which approximate the arc. This alternate representation is available to provide a simpler interface to developers who would benefit from a more limited set of commands.

The only valid [SVGPathSeg](#InterfaceSVGPathSeg) types are SVG_PATHSEG_MOVETO_ABS (M), SVG_PATHSEG_LINETO_ABS (L), SVG_PATHSEG_CURVETO_CUBIC_ABS (C) and SVG_PATHSEG_CLOSEPATH (z).

**<a id="__svg__SVGAnimatedPathData__animatedPathSegList"></a>**animatedPathSegList** (readonly [SVGPathSegList](#InterfaceSVGPathSegList))**
:

Provides access to the current animated contents of the ‘d’ attribute in a form which matches one-for-one with SVG's syntax. If the given attribute or property is being animated, contains the current animated value of the attribute or property, and both the object itself and its contents are read only. If the given attribute or property is not currently being animated, contains the same value as [pathSegList](#__svg__SVGAnimatedPathData__pathSegList).

**<a id="__svg__SVGAnimatedPathData__animatedNormalizedPathSegList"></a>**animatedNormalizedPathSegList** (readonly [SVGPathSegList](#InterfaceSVGPathSegList))**
:

Provides access to the current animated contents of the ‘d’ attribute in a form where all path data commands are expressed in terms of the following subset of [SVGPathSeg](#InterfaceSVGPathSeg) types: SVG_PATHSEG_MOVETO_ABS (M), SVG_PATHSEG_LINETO_ABS (L), SVG_PATHSEG_CURVETO_CUBIC_ABS (C) and SVG_PATHSEG_CLOSEPATH (z). If the given attribute or property is being animated, contains the current animated value of the attribute or property, and both the object itself and its contents are read only. If the given attribute or property is not currently being animated, contains the same value as [normalizedPathSegList](#__svg__SVGAnimatedPathData__normalizedPathSegList).

### <a id="InterfaceSVGPathElement"></a>8.5.23 Interface SVGPathElement

The [SVGPathElement](#InterfaceSVGPathElement) interface corresponds to the [‘path’](#PathElement) element.

```
interface SVGPathElement : SVGElement,
                           SVGTests,
                           SVGLangSpace,
                           SVGExternalResourcesRequired,
                           SVGStylable,
                           SVGTransformable,
                           SVGAnimatedPathData {

  readonly attribute SVGAnimatedNumber pathLength;

  float getTotalLength();
  SVGPoint getPointAtLength(in float distance);
  unsigned long getPathSegAtLength(in float distance);
  SVGPathSegClosePath createSVGPathSegClosePath();
  SVGPathSegMovetoAbs createSVGPathSegMovetoAbs(in float x, in float y);
  SVGPathSegMovetoRel createSVGPathSegMovetoRel(in float x, in float y);
  SVGPathSegLinetoAbs createSVGPathSegLinetoAbs(in float x, in float y);
  SVGPathSegLinetoRel createSVGPathSegLinetoRel(in float x, in float y);
  SVGPathSegCurvetoCubicAbs createSVGPathSegCurvetoCubicAbs(in float x, in float y, in float x1, in float y1, in float x2, in float y2);
  SVGPathSegCurvetoCubicRel createSVGPathSegCurvetoCubicRel(in float x, in float y, in float x1, in float y1, in float x2, in float y2);
  SVGPathSegCurvetoQuadraticAbs createSVGPathSegCurvetoQuadraticAbs(in float x, in float y, in float x1, in float y1);
  SVGPathSegCurvetoQuadraticRel createSVGPathSegCurvetoQuadraticRel(in float x, in float y, in float x1, in float y1);
  SVGPathSegArcAbs createSVGPathSegArcAbs(in float x, in float y, in float r1, in float r2, in float angle, in boolean largeArcFlag, in boolean sweepFlag);
  SVGPathSegArcRel createSVGPathSegArcRel(in float x, in float y, in float r1, in float r2, in float angle, in boolean largeArcFlag, in boolean sweepFlag);
  SVGPathSegLinetoHorizontalAbs createSVGPathSegLinetoHorizontalAbs(in float x);
  SVGPathSegLinetoHorizontalRel createSVGPathSegLinetoHorizontalRel(in float x);
  SVGPathSegLinetoVerticalAbs createSVGPathSegLinetoVerticalAbs(in float y);
  SVGPathSegLinetoVerticalRel createSVGPathSegLinetoVerticalRel(in float y);
  SVGPathSegCurvetoCubicSmoothAbs createSVGPathSegCurvetoCubicSmoothAbs(in float x, in float y, in float x2, in float y2);
  SVGPathSegCurvetoCubicSmoothRel createSVGPathSegCurvetoCubicSmoothRel(in float x, in float y, in float x2, in float y2);
  SVGPathSegCurvetoQuadraticSmoothAbs createSVGPathSegCurvetoQuadraticSmoothAbs(in float x, in float y);
  SVGPathSegCurvetoQuadraticSmoothRel createSVGPathSegCurvetoQuadraticSmoothRel(in float x, in float y);
};
```

**Attributes:**
:

**<a id="__svg__SVGPathElement__pathLength"></a>**pathLength** (readonly [SVGAnimatedNumber](types.md#InterfaceSVGAnimatedNumber))**
:

Corresponds to attribute [pathLength](#__svg__SVGPathElement__pathLength) on the given [‘path’](#PathElement) element.

**Operations:**
:

**<a id="__svg__SVGPathElement__getTotalLength"></a>float **getTotalLength**()**
:

Returns the user agent's computed value for the total length of the path using the user agent's distance-along-a-path algorithm, as a distance in the current user coordinate system.

**Returns**
:

The total length of the path.

**<a id="__svg__SVGPathElement__getPointAtLength"></a>[SVGPoint](coords.md#InterfaceSVGPoint) **getPointAtLength**(in float *distance*)**
:

Returns the (x,y) coordinate in user space which is *distance* units along the path, utilizing the user agent's distance-along-a-path algorithm.

**Parameters**
:

1.
float *distance*
The distance along the path, relative to the start of the path, as a distance in the current user coordinate system.

**Returns**
:

The returned point in user space.

**<a id="__svg__SVGPathElement__getPathSegAtLength"></a>unsigned long **getPathSegAtLength**(in float *distance*)**
:

Returns the index into [pathSegList](#__svg__SVGAnimatedPathData__pathSegList) which is *distance* units along the path, utilizing the user agent's distance-along-a-path algorithm.

**Parameters**
:

1.
float *distance*
The distance along the path, relative to the start of the path, as a distance in the current user coordinate system.

**Returns**
:

The index of the path segment, where the first path segment is number 0.

**<a id="__svg__SVGPathElement__createSVGPathSegClosePath"></a>[SVGPathSegClosePath](#InterfaceSVGPathSegClosePath) **createSVGPathSegClosePath**()**
:

Returns a stand-alone, parentless [SVGPathSegClosePath](#InterfaceSVGPathSegClosePath) object.

**Returns**
:

A stand-alone, parentless [SVGPathSegClosePath](#InterfaceSVGPathSegClosePath) object.

**<a id="__svg__SVGPathElement__createSVGPathSegMovetoAbs"></a>[SVGPathSegMovetoAbs](#InterfaceSVGPathSegMovetoAbs) **createSVGPathSegMovetoAbs**(in float *x*, in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegMovetoAbs](#InterfaceSVGPathSegMovetoAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegMovetoAbs](#InterfaceSVGPathSegMovetoAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegMovetoRel"></a>[SVGPathSegMovetoRel](#InterfaceSVGPathSegMovetoRel) **createSVGPathSegMovetoRel**(in float *x*, in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegMovetoRel](#InterfaceSVGPathSegMovetoRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegMovetoRel](#InterfaceSVGPathSegMovetoRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegLinetoAbs"></a>[SVGPathSegLinetoAbs](#InterfaceSVGPathSegLinetoAbs) **createSVGPathSegLinetoAbs**(in float *x*, in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegLinetoAbs](#InterfaceSVGPathSegLinetoAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegLinetoAbs](#InterfaceSVGPathSegLinetoAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegLinetoRel"></a>[SVGPathSegLinetoRel](#InterfaceSVGPathSegLinetoRel) **createSVGPathSegLinetoRel**(in float *x*, in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegLinetoRel](#InterfaceSVGPathSegLinetoRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegLinetoRel](#InterfaceSVGPathSegLinetoRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoCubicAbs"></a>[SVGPathSegCurvetoCubicAbs](#InterfaceSVGPathSegCurvetoCubicAbs) **createSVGPathSegCurvetoCubicAbs**(in float *x*, in float *y*, in float *x1*, in float *y1*, in float *x2*, in float *y2*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoCubicAbs](#InterfaceSVGPathSegCurvetoCubicAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.
3.
float *x1*
The absolute X coordinate for the first control point.
4.
float *y1*
The absolute Y coordinate for the first control point.
5.
float *x2*
The absolute X coordinate for the second control point.
6.
float *y2*
The absolute Y coordinate for the second control point.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoCubicAbs](#InterfaceSVGPathSegCurvetoCubicAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoCubicRel"></a>[SVGPathSegCurvetoCubicRel](#InterfaceSVGPathSegCurvetoCubicRel) **createSVGPathSegCurvetoCubicRel**(in float *x*, in float *y*, in float *x1*, in float *y1*, in float *x2*, in float *y2*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoCubicRel](#InterfaceSVGPathSegCurvetoCubicRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.
3.
float *x1*
The relative X coordinate for the first control point.
4.
float *y1*
The relative Y coordinate for the first control point.
5.
float *x2*
The relative X coordinate for the second control point.
6.
float *y2*
The relative Y coordinate for the second control point.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoCubicRel](#InterfaceSVGPathSegCurvetoCubicRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoQuadraticAbs"></a>[SVGPathSegCurvetoQuadraticAbs](#InterfaceSVGPathSegCurvetoQuadraticAbs) **createSVGPathSegCurvetoQuadraticAbs**(in float *x*, in float *y*, in float *x1*, in float *y1*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoQuadraticAbs](#InterfaceSVGPathSegCurvetoQuadraticAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.
3.
float *x1*
The absolute X coordinate for the first control point.
4.
float *y1*
The absolute Y coordinate for the first control point.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoQuadraticAbs](#InterfaceSVGPathSegCurvetoQuadraticAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoQuadraticRel"></a>[SVGPathSegCurvetoQuadraticRel](#InterfaceSVGPathSegCurvetoQuadraticRel) **createSVGPathSegCurvetoQuadraticRel**(in float *x*, in float *y*, in float *x1*, in float *y1*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoQuadraticRel](#InterfaceSVGPathSegCurvetoQuadraticRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.
3.
float *x1*
The relative X coordinate for the first control point.
4.
float *y1*
The relative Y coordinate for the first control point.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoQuadraticRel](#InterfaceSVGPathSegCurvetoQuadraticRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegArcAbs"></a>[SVGPathSegArcAbs](#InterfaceSVGPathSegArcAbs) **createSVGPathSegArcAbs**(in float *x*, in float *y*, in float *r1*, in float *r2*, in float *angle*, in boolean *largeArcFlag*, in boolean *sweepFlag*)**
:

Returns a stand-alone, parentless [SVGPathSegArcAbs](#InterfaceSVGPathSegArcAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.
3.
float *r1*
The x-axis radius for the ellipse (i.e., r1).
4.
float *r2*
The y-axis radius for the ellipse (i.e., r2).
5.
float *angle*
The rotation angle in degrees for the ellipse's x-axis relative to the x-axis of the user coordinate system.
6.
boolean *largeArcFlag*
The value of the large-arc-flag parameter.
7.
boolean *sweepFlag*
The value of the large-arc-flag parameter.

**Returns**
:

A stand-alone, parentless [SVGPathSegArcAbs](#InterfaceSVGPathSegArcAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegArcRel"></a>[SVGPathSegArcRel](#InterfaceSVGPathSegArcRel) **createSVGPathSegArcRel**(in float *x*, in float *y*, in float *r1*, in float *r2*, in float *angle*, in boolean *largeArcFlag*, in boolean *sweepFlag*)**
:

Returns a stand-alone, parentless [SVGPathSegArcRel](#InterfaceSVGPathSegArcRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.
3.
float *r1*
The x-axis radius for the ellipse (i.e., r1).
4.
float *r2*
The y-axis radius for the ellipse (i.e., r2).
5.
float *angle*
The rotation angle in degrees for the ellipse's x-axis relative to the x-axis of the user coordinate system.
6.
boolean *largeArcFlag*
The value of the large-arc-flag parameter.
7.
boolean *sweepFlag*
The value of the large-arc-flag parameter.

**Returns**
:

A stand-alone, parentless [SVGPathSegArcRel](#InterfaceSVGPathSegArcRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegLinetoHorizontalAbs"></a>[SVGPathSegLinetoHorizontalAbs](#InterfaceSVGPathSegLinetoHorizontalAbs) **createSVGPathSegLinetoHorizontalAbs**(in float *x*)**
:

Returns a stand-alone, parentless [SVGPathSegLinetoHorizontalAbs](#InterfaceSVGPathSegLinetoHorizontalAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegLinetoHorizontalAbs](#InterfaceSVGPathSegLinetoHorizontalAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegLinetoHorizontalRel"></a>[SVGPathSegLinetoHorizontalRel](#InterfaceSVGPathSegLinetoHorizontalRel) **createSVGPathSegLinetoHorizontalRel**(in float *x*)**
:

Returns a stand-alone, parentless [SVGPathSegLinetoHorizontalRel](#InterfaceSVGPathSegLinetoHorizontalRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegLinetoHorizontalRel](#InterfaceSVGPathSegLinetoHorizontalRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegLinetoVerticalAbs"></a>[SVGPathSegLinetoVerticalAbs](#InterfaceSVGPathSegLinetoVerticalAbs) **createSVGPathSegLinetoVerticalAbs**(in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegLinetoVerticalAbs](#InterfaceSVGPathSegLinetoVerticalAbs) object.

**Parameters**
:

1.
float *y*
The absolute Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegLinetoVerticalAbs](#InterfaceSVGPathSegLinetoVerticalAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegLinetoVerticalRel"></a>[SVGPathSegLinetoVerticalRel](#InterfaceSVGPathSegLinetoVerticalRel) **createSVGPathSegLinetoVerticalRel**(in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegLinetoVerticalRel](#InterfaceSVGPathSegLinetoVerticalRel) object.

**Parameters**
:

1.
float *y*
The relative Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegLinetoVerticalRel](#InterfaceSVGPathSegLinetoVerticalRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoCubicSmoothAbs"></a>[SVGPathSegCurvetoCubicSmoothAbs](#InterfaceSVGPathSegCurvetoCubicSmoothAbs) **createSVGPathSegCurvetoCubicSmoothAbs**(in float *x*, in float *y*, in float *x2*, in float *y2*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoCubicSmoothAbs](#InterfaceSVGPathSegCurvetoCubicSmoothAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.
3.
float *x2*
The absolute X coordinate for the second control point.
4.
float *y2*
The absolute Y coordinate for the second control point.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoCubicSmoothAbs](#InterfaceSVGPathSegCurvetoCubicSmoothAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoCubicSmoothRel"></a>[SVGPathSegCurvetoCubicSmoothRel](#InterfaceSVGPathSegCurvetoCubicSmoothRel) **createSVGPathSegCurvetoCubicSmoothRel**(in float *x*, in float *y*, in float *x2*, in float *y2*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoCubicSmoothRel](#InterfaceSVGPathSegCurvetoCubicSmoothRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.
3.
float *x2*
The relative X coordinate for the second control point.
4.
float *y2*
The relative Y coordinate for the second control point.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoCubicSmoothRel](#InterfaceSVGPathSegCurvetoCubicSmoothRel) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoQuadraticSmoothAbs"></a>[SVGPathSegCurvetoQuadraticSmoothAbs](#InterfaceSVGPathSegCurvetoQuadraticSmoothAbs) **createSVGPathSegCurvetoQuadraticSmoothAbs**(in float *x*, in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoQuadraticSmoothAbs](#InterfaceSVGPathSegCurvetoQuadraticSmoothAbs) object.

**Parameters**
:

1.
float *x*
The absolute X coordinate for the end point of this path segment.
2.
float *y*
The absolute Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoQuadraticSmoothAbs](#InterfaceSVGPathSegCurvetoQuadraticSmoothAbs) object.

**<a id="__svg__SVGPathElement__createSVGPathSegCurvetoQuadraticSmoothRel"></a>[SVGPathSegCurvetoQuadraticSmoothRel](#InterfaceSVGPathSegCurvetoQuadraticSmoothRel) **createSVGPathSegCurvetoQuadraticSmoothRel**(in float *x*, in float *y*)**
:

Returns a stand-alone, parentless [SVGPathSegCurvetoQuadraticSmoothRel](#InterfaceSVGPathSegCurvetoQuadraticSmoothRel) object.

**Parameters**
:

1.
float *x*
The relative X coordinate for the end point of this path segment.
2.
float *y*
The relative Y coordinate for the end point of this path segment.

**Returns**
:

A stand-alone, parentless [SVGPathSegCurvetoQuadraticSmoothRel](#InterfaceSVGPathSegCurvetoQuadraticSmoothRel) object.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](coords.md) ⋅ [Next](shapes.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
