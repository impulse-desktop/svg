<!-- https://svgwg.org/svg2-draft/shapes.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 10: Basic Shapes

## <a id="Introduction"></a>10.1. Introduction and definitions

**<a id="TermBasicShapeElement"></a>*<a id="basic-shape"></a>basic shape***
**shape**
***<a id="TermShapeElement"></a>shape elements***
: A graphics element that is defined by some combination of straight lines and curves. Specifically: ‘[circle](#CircleElement)’, ‘[ellipse](#EllipseElement)’, ‘[line](#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](#PolygonElement)’, ‘[polyline](#PolylineElement)’ and ‘[rect](#RectElement)’.
SVG contains the following set of basic shape elements:

- rectangles (including optional rounded corners), created with the ‘[rect](#RectElement)’ element,
- circles, created with the ‘[circle](#CircleElement)’ element,
- ellipses, created with the ‘[ellipse](#EllipseElement)’ element,
- straight lines, created with the ‘[line](#LineElement)’ element,
- polylines, created with the ‘[polyline](#PolylineElement)’ element, and
- polygons, created with the ‘[polygon](#PolygonElement)’ element

Mathematically, these shape elements are equivalent to a ‘[path](paths.md#PathElement)’ element that would construct the same shape. The basic shapes may be stroked, filled and used as clip paths. All of the properties available for ‘[path](paths.md#PathElement)’ elements also apply to the basic shapes.

The [equivalent path](paths.md#TermEquivalentPath) and algorithm to compute the stroke for each shape are defined in the shape sections below.

## <a id="RectElement"></a>10.2. The ‘rect’ element

The ‘[rect](#RectElement)’ element defines a rectangle which is axis-aligned with the current [user coordinate system](../css-transforms-1.md#user-coordinate-system). Rounded rectangles can be achieved by setting non-zero values for the [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) geometric properties.

‘*<a id="elementdef-rect"></a>rect*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [shape element](#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[pathLength](paths.md#PathLengthAttribute)’

**Geometry properties:**
:

- [x](geometry.md#XProperty)
- [y](geometry.md#YProperty)
- [width](geometry.md#Sizing)
- [height](geometry.md#Sizing)
- [rx](geometry.md#RxProperty)
- [ry](geometry.md#RyProperty)

**DOM Interfaces:**
:

- [SVGRectElement](#InterfaceSVGRectElement)

The [x](geometry.md#XProperty) and [y](geometry.md#YProperty) coordinates refer to the left and top edges of the rectangle, in the current user coordinate system.

The [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties define the overall width and height of the rectangle. A negative value for either property is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored). A computed value of zero for either dimension disables rendering of the element.

For rounded rectangles, the computed values of the [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) properties define the x- and y-axis radii of elliptical arcs used to round off the corners of the rectangle. The arc are always symmetrical along both horizontal and vertical axis; to create a rectangle with uneven corner rounding, define the shape explicitly with a ‘[path](paths.md#PathElement)’. A negative value for either property is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored). A computed value of zero for either dimension, or a computed value of `auto` for *both* dimensions, results in a rectangle without corner rounding.

The used values for the x- and y-axis rounded corner radii may be determined implicitly from the other dimension (using the `auto` value), and are also subject to clamping so that the lengths of the straight segments of the rectangle are never negative. The used values for [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) are determined from the computed values by following these steps in order:

1. If both [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) have a computed value of `auto` (since `auto` is the initial value for both properties, this will also occur if neither are specified by the author or if all author-supplied values are invalid), then the used value of both [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) is 0. (This will result in square corners.)
2. Otherwise, convert specified values to absolute values as follows:
  1. If [rx](geometry.md#RxProperty) is set to a length value or a percentage, but [ry](geometry.md#RyProperty) is `auto`, calculate an absolute length equivalent for [rx](geometry.md#RxProperty), resolving percentages against the used [width](geometry.md#Sizing) of the rectangle; the absolute value for [ry](geometry.md#RyProperty) is the same.
  2. If [ry](geometry.md#RyProperty) is set to a length value or a percentage, but [rx](geometry.md#RxProperty) is `auto`, calculate the absolute length equivalent for [ry](geometry.md#RyProperty), resolving percentages against the used [height](geometry.md#Sizing) of the rectangle; the absolute value for [rx](geometry.md#RxProperty) is the same.
  3. If both [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) were set to lengths or percentages, absolute values are generated individually, resolving [rx](geometry.md#RxProperty) percentages against the used [width](geometry.md#Sizing), and resolving [ry](geometry.md#RyProperty) percentages against the used [height](geometry.md#Sizing).
3. Finally, apply clamping to generate the used values:
  1. If the absolute [rx](geometry.md#RxProperty) (after the above steps) is greater than half of the used [width](geometry.md#Sizing), then the used value of [rx](geometry.md#RxProperty) is half of the used [width](geometry.md#Sizing).
  2. If the absolute [ry](geometry.md#RyProperty) (after the above steps) is greater than half of the used [height](geometry.md#Sizing), then the used value of [ry](geometry.md#RyProperty) is half of the used [height](geometry.md#Sizing).
  3. Otherwise, the used values of [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) are the absolute values computed previously.

Mathematically, a ‘[rect](#RectElement)’ element is mapped to an equivalent ‘[path](paths.md#PathElement)’ element as follows, after generating absolute used values *x*, *y*, *width*, *height*, *rx*, and *rx* in user units for the user coordinate system, for each of the equivalent geometric properties following the rules specified above and in [Units](coords.md#Units):

1. perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to location (*x+rx*,*y*);
2. perform an absolute horizontal [lineto](paths.md#PathDataLinetoCommands) with parameter *x+width-rx*;
3. *if* both *rx* and *ry* are greater than zero, perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x+width*,*y+ry*), where *rx* and *ry* are used as the equivalent parameters to the [elliptical arc](paths.md#PathDataEllipticalArcCommands) command, the *x-axis-rotation* and *large-arc-flag* are set to zero, the *sweep-flag* is set to one;
4. perform an absolute vertical [lineto](paths.md#PathDataLinetoCommands) parameter *y+height-ry*;
5. *if* both *rx* and *ry* are greater than zero, perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x+width-rx*,*y+height*), using the same parameters as previously;
6. perform an absolute horizontal [lineto](paths.md#PathDataLinetoCommands) parameter *x+rx*;
7. *if* both *rx* and *ry* are greater than zero, perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation to coordinate (*x*,*y+height-ry*), using the same parameters as previously;
8. perform an absolute vertical [lineto](paths.md#PathDataLinetoCommands) parameter *y+ry*
9. *if* both *rx* and *ry* are greater than zero, perform an absolute [elliptical arc](paths.md#PathDataEllipticalArcCommands) operation with a [segment-completing close path](paths.md#TermSegment-CompletingClosePath) operation, using the same parameters as previously.

<a id="ExampleRect01"></a>Example rect01 shows a rectangle with sharp corners. The ‘[rect](#RectElement)’ element is filled with yellow and stroked with navy.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example rect01 - rectangle with sharp corners</desc>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2"/>

  <rect x="400" y="100" width="400" height="200"
        fill="yellow" stroke="navy" stroke-width="10"  />
</svg>
```

[image: Example rect01 — rectangle with sharp corners]

Example rect01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/rect01.svg)

<a id="ExampleRect02"></a>Example rect02 shows two rounded rectangles. The [rx](geometry.md#RxProperty) specifies how to round the corners of the rectangles. Note that since no value has been specified for the [ry](geometry.md#RyProperty) attribute, the used value will be derived from the [rx](geometry.md#RxProperty) attribute.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example rect02 - rounded rectangles</desc>

  <!-- Show outline of viewport using 'rect' element -->
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

[image: Example rect02 — rounded rectangles expressed in user coordinates]

Example rect02

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/rect02.svg)

## <a id="CircleElement"></a>10.3. The ‘circle’ element

The ‘[circle](#CircleElement)’ element defines a circle based on a center point and a radius.

‘*<a id="elementdef-circle"></a>circle*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [shape element](#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[pathLength](paths.md#PathLengthAttribute)’

**Geometry properties:**
:

- [cx](geometry.md#CxProperty)
- [cy](geometry.md#CyProperty)
- [r](geometry.md#RProperty)

**DOM Interfaces:**
:

- [SVGCircleElement](#InterfaceSVGCircleElement)

The [cx](geometry.md#CxProperty) and [cy](geometry.md#CyProperty) attributes define the coordinates of the center of the circle.

The [r](geometry.md#RProperty) attribute defines the radius of the circle. A negative value is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored). A computed value of zero disables rendering of the element.

Mathematically, a ‘[circle](#CircleElement)’ element is mapped to an equivalent ‘[path](paths.md#PathElement)’ element that consists of four [elliptical arc](paths.md#PathDataEllipticalArcCommands) segments, each covering a quarter of the circle. The path begins at the "3 o'clock" point on the radius and proceeds in a clock-wise direction (before any transformations). The *rx* and *ry* parameters to the arc commands are both equal to the used value of the [r](geometry.md#RProperty) property, after conversion to local user units, while the *x-axis-rotation* and the *large-arc-flag* are set to zero, and the *sweep-flag* is set to one. The coordinates are computed as follows, where *cx*, *cy*, and *r* are the used values of the equivalent properties, converted to user units:

1. A move-to command to the point *cx+r*,*cy*;
2. arc to *cx*,*cy+r*;
3. arc to *cx-r*,*cy*;
4. arc to *cx*,*cy-r*;
5. arc with a [segment-completing close path](paths.md#TermSegment-CompletingClosePath) operation.

<a id="ExampleCircle01"></a>Example circle01 consists of a ‘[circle](#CircleElement)’ element that is filled with red and stroked with blue.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example circle01 - circle filled with red and stroked with blue</desc>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x="1" y="1" width="1198" height="398"
        fill="none" stroke="blue" stroke-width="2"/>

  <circle cx="600" cy="200" r="100"
        fill="red" stroke="blue" stroke-width="10"  />
</svg>
```

[image: Example circle01 — circle filled with red and stroked with blue]

Example circle01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/circle01.svg)

## <a id="EllipseElement"></a>10.4. The ‘ellipse’ element

The ‘[ellipse](#EllipseElement)’ element defines an ellipse which is axis-aligned with the current [user coordinate system](../css-transforms-1.md#user-coordinate-system) based on a center point and two radii.

‘*<a id="elementdef-ellipse"></a>ellipse*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [shape element](#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[pathLength](paths.md#PathLengthAttribute)’

**Geometry properties:**
:

- [cx](geometry.md#CxProperty)
- [cy](geometry.md#CyProperty)
- [rx](geometry.md#RxProperty)
- [ry](geometry.md#RyProperty)

**DOM Interfaces:**
:

- [SVGEllipseElement](#InterfaceSVGEllipseElement)

The [cx](geometry.md#CxProperty) and [cy](geometry.md#CyProperty) coordinates define the center of the ellipse.

The [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) properties define the x- and y-axis radii of the ellipse. A negative value for either property is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored). A computed value of zero for either dimension, or a computed value of `auto` for *both* dimensions, disables rendering of the element.

An `auto` value for *either* [rx](geometry.md#RxProperty) or [ry](geometry.md#RyProperty) is converted to a used value, following the rules given above for rectangles (but without any clamping based on [width](geometry.md#Sizing) or [height](geometry.md#Sizing)). Effectively, an `auto` value creates a circular shape whose radius is defined by a value expressed solely in one dimension; this allows for creating a circle with a radius defined in terms of one of the following:

- a percentage of the coordinate system **width**; that is, a percentage value for [rx](geometry.md#RxProperty) and an `auto` value for [ry](geometry.md#RyProperty).
- a percentage of the coordinate system **height**; that is, an `auto` value for [rx](geometry.md#RxProperty) and a percentage value for [ry](geometry.md#RyProperty).

New in SVG 2. The `auto` value for [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) was added to allow consistent parsing of these properties for both ellipses and rectangles. Previously, if either [rx](geometry.md#RxProperty) or [ry](geometry.md#RyProperty) was unspecified, the ellipse would not render.

Mathematically, an ‘[ellipse](#EllipseElement)’ element is mapped to an equivalent ‘[path](paths.md#PathElement)’ element that consists of four [elliptical arc](paths.md#PathDataEllipticalArcCommands) segments, each covering a quarter of the ellipse. The path begins at the "3 o'clock" point on the radius and proceeds in a clock-wise direction (before any transformation). The *rx* and *ry* parameters to the arc commands are the used values of the equivalent properties after conversion to local user units, while the *x-axis-rotation* and the *large-arc-flag* are set to zero, and the *sweep-flag* is set to one. The coordinates are computed as follows, where *cx*, *cy*, *rx*, and *ry* are the used values of the equivalent properties, converted to user units:

1. A move-to command to the point *cx+rx*,*cy*;
2. arc to *cx*,*cy+ry*;
3. arc to *cx-rx*,*cy*;
4. arc to *cx*,*cy-ry*;
5. arc with a [segment-completing close path](paths.md#TermSegment-CompletingClosePath) operation.

Example ellipse01 below specifies the coordinates of the two ellipses in the user coordinate system established by the ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute on the ‘[svg](struct.md#SVGElement)’ element and the [transform](coords.md#TransformProperty) property on the ‘[g](struct.md#GElement)’ and ‘[ellipse](#EllipseElement)’ elements. Both ellipses use the default values of zero for the [cx](geometry.md#CxProperty) and [cy](geometry.md#CyProperty) attributes (the center of the ellipse). The second ellipse is rotated.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example ellipse01 - examples of ellipses</desc>

  <!-- Show outline of viewport using 'rect' element -->
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

[image: Example ellipse01 — ellipses expressed in user coordinates]

Example ellipse01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/ellipse01.svg)

## <a id="LineElement"></a>10.5. The ‘line’ element

The ‘[line](#LineElement)’ element defines a line segment that starts at one point and ends at another.

‘*<a id="elementdef-line"></a>line*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [shape element](#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[pathLength](paths.md#PathLengthAttribute)’
- ‘[x1](#LineElementX1Attribute)’
- ‘[y1](#LineElementY1Attribute)’
- ‘[x2](#LineElementX2Attribute)’
- ‘[y2](#LineElementY2Attribute)’

**DOM Interfaces:**
:

- [SVGLineElement](#InterfaceSVGLineElement)

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="LineElementX1Attribute"></a>x1*, *<a id="LineElementY1Attribute"></a>y1* | [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| <number> | 0 | yes |

**
: The x- and y-axis coordinates of the start of the line.
**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="LineElementX2Attribute"></a>x2*, *<a id="LineElementY2Attribute"></a>y2* | [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| <number> | 0 | yes |

**
: The x- and y-axis coordinates of the end of the line.
A future specification may convert the ‘[x1](#LineElementX1Attribute)’, ‘[y1](#LineElementY1Attribute)’, ‘[x2](#LineElementX2Attribute)’, and ‘[y2](#LineElementY2Attribute)’ attributes to geometric properties. Currently, they can only be specified via element attributes, and not CSS.

Mathematically, a ‘[line](#LineElement)’ element can be mapped to an equivalent ‘[path](paths.md#PathElement)’ element as follows, after converting coordinates into user coordinate system user units according to [Units](coords.md#Units) to generate values *x1*, *y1*, *x2*, and *y2*:

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to absolute location (*x1*,*y1*)
- perform an absolute [lineto](paths.md#PathDataLinetoCommands) operation to absolute location (*x2*,*y2*)

Because ‘[line](#LineElement)’ elements are single lines and thus are geometrically one-dimensional, they have no interior; thus, ‘[line](#LineElement)’ elements are never filled (see the [fill](painting.md#FillProperty) property).

<a id="ExampleLine01"></a>Example line01 below specifies the coordinates of the five lines in the user coordinate system established by the ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute on the ‘[svg](struct.md#SVGElement)’ element. The lines have different thicknesses.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example line01 - lines expressed in user coordinates</desc>

  <!-- Show outline of viewport using 'rect' element -->
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

[image: Example line01 — lines expressed in user coordinates]

Example line01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/line01.svg)

## <a id="PolylineElement"></a>10.6. The ‘polyline’ element

The ‘[polyline](#PolylineElement)’ element defines a set of connected straight line segments. Typically, ‘[polyline](#PolylineElement)’ elements define open shapes.

‘*<a id="elementdef-polyline"></a>polyline*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [shape element](#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[pathLength](paths.md#PathLengthAttribute)’
- ‘[points](#PolylineElementPointsAttribute)’

**DOM Interfaces:**
:

- [SVGPolylineElement](#InterfaceSVGPolylineElement)

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="PolylineElementPointsAttribute"></a>points* | [<points>](#DataTypePoints) | (none) | yes |

**
:

where:

*<a id="DataTypePoints"></a><points>* =

[ <number>+ ]#

The points that make up the polyline. All coordinate values are in the user coordinate system.

If an odd number of coordinates is provided, then the element is in error, with the same user agent behavior as occurs with an incorrectly specified ‘[path](paths.md#PathElement)’ element. In such error cases the user agent will drop the last, odd coordinate and otherwise render the shape.

The initial value, (none), indicates that the polyline element is valid but does not render.

A future specification may convert the ‘[points](#PolylineElementPointsAttribute)’ attribute to a geometric property. Currently, it can only be specified via an element attribute, and not CSS.

Mathematically, a ‘[polyline](#PolylineElement)’ element can be mapped to an equivalent ‘[path](paths.md#PathElement)’ element as follows:

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to the first coordinate pair in the list of points
- for each subsequent coordinate pair, perform an absolute [lineto](paths.md#PathDataLinetoCommands) operation to that coordinate pair.

<a id="ExamplePolyline01"></a>Example polyline01 below specifies a polyline in the user coordinate system established by the ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute on the ‘[svg](struct.md#SVGElement)’ element.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example polyline01 - increasingly larger bars</desc>

  <!-- Show outline of viewport using 'rect' element -->
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

[image: Example polyline01 — increasingly larger bars]

Example polyline01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/polyline01.svg)

## <a id="PolygonElement"></a>10.7. The ‘polygon’ element

The ‘[polygon](#PolygonElement)’ element defines a closed shape consisting of a set of connected straight line segments.

‘*<a id="elementdef-polygon"></a>polygon*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [shape element](#TermShapeElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[pathLength](paths.md#PathLengthAttribute)’
- ‘[points](#PolygonElementPointsAttribute)’

**DOM Interfaces:**
:

- [SVGPolygonElement](#InterfaceSVGPolygonElement)

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="PolygonElementPointsAttribute"></a>points* | [<points>](#DataTypePoints) | (none) | yes |

**
:

The points that make up the polygon. All coordinate values are in the user coordinate system.

If an odd number of coordinates is provided, then the element is in error, with the same user agent behavior as occurs with an incorrectly specified ‘[path](paths.md#PathElement)’ element.

The initial value, (none), indicates that the polygon element is valid, but does not render.

A future specification may convert the ‘[points](#PolygonElementPointsAttribute)’ attribute to a geometric property. Currently, it can only be specified via an element attribute, and not CSS.

Mathematically, a ‘[polygon](#PolygonElement)’ element can be mapped to an equivalent ‘[path](paths.md#PathElement)’ element as follows:

- perform an absolute [moveto](paths.md#PathDataMovetoCommands) operation to the first coordinate pair in the list of points
- for each subsequent coordinate pair, perform an absolute [lineto](paths.md#PathDataLinetoCommands) operation to that coordinate pair
- perform a [closepath](paths.md#PathDataClosePathCommand) command

<a id="ExamplePolygon01"></a>Example polygon01 below specifies two polygons (a star and a hexagon) in the user coordinate system established by the ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute on the ‘[svg](struct.md#SVGElement)’ element.

```
<?xml version="1.0" standalone="no"?>
<svg width="12cm" height="4cm" viewBox="0 0 1200 400"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Example polygon01 - star and hexagon</desc>

  <!-- Show outline of viewport using 'rect' element -->
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

[image: Example polygon01 — star and hexagon]

Example polygon01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/shapes/polygon01.svg)

## <a id="DOMInterfaces"></a>10.8. DOM interfaces

### <a id="InterfaceSVGRectElement"></a>10.8.1. Interface SVGRectElement

An [SVGRectElement](#InterfaceSVGRectElement) object represents a ‘[rect](#RectElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGRectElement : SVGGeometryElement {
  [SameObject] readonly attribute SVGAnimatedLength x;
  [SameObject] readonly attribute SVGAnimatedLength y;
  [SameObject] readonly attribute SVGAnimatedLength width;
  [SameObject] readonly attribute SVGAnimatedLength height;
  [SameObject] readonly attribute SVGAnimatedLength rx;
  [SameObject] readonly attribute SVGAnimatedLength ry;
};
```

The **<a id="__svg__SVGRectElement__x"></a>x**, **<a id="__svg__SVGRectElement__y"></a>y**, **<a id="__svg__SVGRectElement__width"></a>width**, **<a id="__svg__SVGRectElement__height"></a>height**, **<a id="__svg__SVGRectElement__rx"></a>rx** and **<a id="__svg__SVGRectElement__ry"></a>ry** IDL attributes [reflect](types.md#TermReflect) the computed values of the [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing), [height](geometry.md#Sizing), [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) properties and their corresponding presentation attributes, respectively.

### <a id="InterfaceSVGCircleElement"></a>10.8.2. Interface SVGCircleElement

An [SVGCircleElement](#InterfaceSVGCircleElement) object represents a ‘[circle](#CircleElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGCircleElement : SVGGeometryElement {
  [SameObject] readonly attribute SVGAnimatedLength cx;
  [SameObject] readonly attribute SVGAnimatedLength cy;
  [SameObject] readonly attribute SVGAnimatedLength r;
};
```

The **<a id="__svg__SVGCircleElement__cx"></a>cx**, **<a id="__svg__SVGCircleElement__cy"></a>cy** and **<a id="__svg__SVGCircleElement__r"></a>r** IDL attributes [reflect](types.md#TermReflect) the computed values of the [cx](geometry.md#CxProperty), [cy](geometry.md#CyProperty) and [y](geometry.md#YProperty) properties and their corresponding presentation attributes, respectively.

### <a id="InterfaceSVGEllipseElement"></a>10.8.3. Interface SVGEllipseElement

An [SVGEllipseElement](#InterfaceSVGEllipseElement) object represents a ‘[ellipse](#EllipseElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGEllipseElement : SVGGeometryElement {
  [SameObject] readonly attribute SVGAnimatedLength cx;
  [SameObject] readonly attribute SVGAnimatedLength cy;
  [SameObject] readonly attribute SVGAnimatedLength rx;
  [SameObject] readonly attribute SVGAnimatedLength ry;
};
```

The **<a id="__svg__SVGEllipseElement__cx"></a>cx**, **<a id="__svg__SVGEllipseElement__cy"></a>cy**, **<a id="__svg__SVGEllipseElement__rx"></a>rx** and **<a id="__svg__SVGEllipseElement__ry"></a>ry** IDL attributes [reflect](types.md#TermReflect) the computed values of the [cx](geometry.md#CxProperty), [cy](geometry.md#CyProperty), [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) properties and their corresponding presentation attributes, respectively.

### <a id="InterfaceSVGLineElement"></a>10.8.4. Interface SVGLineElement

The [SVGLineElement](#InterfaceSVGLineElement) interface corresponds to the ‘[line](#LineElement)’ element.

```
[Exposed=Window]
interface SVGLineElement : SVGGeometryElement {
  [SameObject] readonly attribute SVGAnimatedLength x1;
  [SameObject] readonly attribute SVGAnimatedLength y1;
  [SameObject] readonly attribute SVGAnimatedLength x2;
  [SameObject] readonly attribute SVGAnimatedLength y2;
};
```

The **<a id="__svg__SVGLineElement__x1"></a>x1**, **<a id="__svg__SVGLineElement__y1"></a>y1**, **<a id="__svg__SVGLineElement__x2"></a>x2** and **<a id="__svg__SVGLineElement__y2"></a>y2** IDL attributes [reflect](types.md#TermReflect) the ‘[x1](#LineElementX1Attribute)’, ‘[y1](#LineElementY1Attribute)’, ‘[x2](#LineElementX2Attribute)’ and ‘[y2](#LineElementY2Attribute)’ content attributes, respectively

### <a id="InterfaceSVGAnimatedPoints"></a>10.8.5. Mixin SVGAnimatedPoints

The [SVGAnimatedPoints](#InterfaceSVGAnimatedPoints) interface is used to [reflect](types.md#TermReflect) a ‘[points](#PolygonElementPointsAttribute)’ attribute on a ‘[polygon](#PolygonElement)’ or ‘[polyline](#PolylineElement)’ element. It is mixed in to the [SVGPolygonElement](#InterfaceSVGPolygonElement) and [SVGPolylineElement](#InterfaceSVGPolylineElement) interfaces.

```
interface mixin SVGAnimatedPoints {
  [SameObject] readonly attribute SVGPointList points;
  [SameObject] readonly attribute SVGPointList animatedPoints;
};
```

The **<a id="__svg__SVGAnimatedPoints__points"></a>points** IDL attribute represents the current non-animated value of the reflected attribute. On getting [points](#__svg__SVGAnimatedPoints__points), an [SVGPointList](#InterfaceSVGPointList) object is returned that reflects the base value of the reflected attribute.

The **<a id="__svg__SVGAnimatedPoints__animatedPoints"></a>animatedPoints** IDL attribute represents the current non-animated value of the reflected attribute. On getting [animatedPoints](#__svg__SVGAnimatedPoints__animatedPoints), an [SVGPointList](#InterfaceSVGPointList) object is returned that reflects the animated value of the reflected attribute.

The objects returned from [points](#__svg__SVGAnimatedPoints__points) and [animatedPoints](#__svg__SVGAnimatedPoints__animatedPoints) must be distinct, even if there is no animation currently affecting the attribute.

### <a id="InterfaceSVGPointList"></a>10.8.6. Interface SVGPointList

The [SVGPointList](#InterfaceSVGPointList) interface is a [list interface](types.md#TermListInterface) whose elements are DOMPoint objects. An [SVGPointList](#InterfaceSVGPointList) object represents a list of points.

```
[Exposed=Window]
interface SVGPointList {

  readonly attribute unsigned long length;
  readonly attribute unsigned long numberOfItems;

  undefined clear();
  DOMPoint initialize(DOMPoint newItem);
  getter DOMPoint getItem(unsigned long index);
  DOMPoint insertItemBefore(DOMPoint newItem, unsigned long index);
  DOMPoint replaceItem(DOMPoint newItem, unsigned long index);
  DOMPoint removeItem(unsigned long index);
  DOMPoint appendItem(DOMPoint newItem);
  setter undefined (unsigned long index, DOMPoint newItem);
};
```

The behavior of all of the interface members of [SVGPointList](#InterfaceSVGPointList) are defined in [List interfaces](types.md#ListInterfaces).

This specification imposes additional requirements on the behaviour of DOMPoint objects beyond those described in the Geometry Interfaces specification, so that they can be used to reflect ‘[points](#PolygonElementPointsAttribute)’ attributes.

<a id="PointMode"></a>Every DOMPoint object operates in one of four modes. It can:

1. *reflect an element of the base value* of a [reflected](types.md#TermReflect) animatable attribute (being exposed through the methods on the [points](#__svg__SVGAnimatedPoints__points) member of an [SVGAnimatedPoints](#InterfaceSVGAnimatedPoints)),
2. *reflect an element of the animated value* of a [reflected](types.md#TermReflect) animatable attribute (being exposed through the methods on the [animatedPoints](#__svg__SVGAnimatedPoints__animatedPoints) member of an [SVGAnimatedPoints](#InterfaceSVGAnimatedPoints)),
3. *represent the current translation* of a given ‘[svg](struct.md#SVGElement)’ element (being exposed through the [currentTranslate](struct.md#__svg__SVGSVGElement__currentTranslate) member on [SVGSVGElement](struct.md#InterfaceSVGSVGElement)), or
4. *be detached*, which is the case for DOMPoint objects created using their constructor or with [createSVGPoint](struct.md#__svg__SVGSVGElement__createSVGPoint).

<a id="PointAssociatedElement"></a>A DOMPoint object can be *associated* with a particular element. The associated element is used to determine which element's content attribute to update if the object [reflects](types.md#TermReflect) an attribute. Unless otherwise described, a DOMPoint object is not associated with any element.

<a id="ReadOnlyPoint"></a>A DOMPoint object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown. When assigning to a read only DOMPoint's x, y, w or z IDL attribute, a NoModificationAllowedError must be thrown instead of updating the internal coordinate value.

Note that this applies only to the read-write DOMPoint interface; the DOMPointReadOnly interface, which is not used for reflecting the ‘[points](#PolygonElementPointsAttribute)’ attribute, will already throw an exception if an attempt is made to modify it.

<a id="AssignToDOMPoint"></a>When assigning to a writable DOMPoint's x, y, w or z IDL attribute, the following steps are run after updating the internal coordinate value:

1. If the DOMPoint [reflects an element of the base value](#PointMode) of a [reflected](types.md#TermReflect) attribute, then [reserialize](types.md#TermReserialize) the reflected attribute using the [SVGPointList](#InterfaceSVGPointList) that reflects the attribute's base value.
2. Otherwise, if the DOMPoint [represents the current translation](#PointMode) of an ‘[svg](struct.md#SVGElement)’ element and that element is the [outermost svg element](struct.md#TermOutermostSVGElement), then:
  1. Let [*a* *b* *c* *d* *e* *f*] be the 2x3 matrix that represents the document's magnification and panning transform.
  2. Let *x* and *y* be the x and y coordinates of the DOMPoint object, respectively.
  3. Set the document's magnification and panning transform to [*a* 0 0 *d* *x* *y*].

### <a id="InterfaceSVGPolylineElement"></a>10.8.7. Interface SVGPolylineElement

An [SVGPolylineElement](#InterfaceSVGPolylineElement) object represents a ‘[polyline](#PolylineElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGPolylineElement : SVGGeometryElement {
};

SVGPolylineElement includes SVGAnimatedPoints;
```

### <a id="InterfaceSVGPolygonElement"></a>10.8.8. Interface SVGPolygonElement

An [SVGPolygonElement](#InterfaceSVGPolygonElement) object represents a ‘[polygon](#PolygonElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGPolygonElement : SVGGeometryElement {
};

SVGPolygonElement includes SVGAnimatedPoints;
```
