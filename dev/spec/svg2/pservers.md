<!-- https://svgwg.org/svg2-draft/pservers.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 14: Paint Servers: Gradients and Patterns

## <a id="Introduction"></a>14.1. Introduction

This section covers **Paint Servers**, a method which allows the [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) of an object to be defined by a resource found elsewhere. It allows resources to be reused throughout a document. See the section [Painting: Filling and Stroking](painting.md) for a general discussion of filling and stroking objects.

SVG defines several types of paint servers:

- [Gradients](#Gradients),
- [Patterns](#Patterns).

[image: Image of two types of paint servers.]

Two types of paint servers. From left to right: A linear gradient. A pattern.

Paint servers are used by including a [URL reference](linking.md#TermURLReference) in a [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) property (i.e. fill="url(#MyLightPurple)").

[properties](styling.md#TermProperty) inherit into a paint-server element from its ancestors; properties do *not* inherit from the element referencing the paint server element.

Paint-server elements are never rendered directly; their only usage is as something that can be referenced using the [fill](painting.md#FillProperty) and [stroke](painting.md#StrokeProperty) properties. The [display](render.md#VisibilityControl) value for these elements must always be set to none by the [user agent style sheet](styling.md#UAStyleSheet), and this declaration must have importance over any other CSS rule or presentation attribute. Paint-server elements are available for referencing even when the [display](render.md#VisibilityControl) property on the paint-server element or any of its ancestors is set to none.

### <a id="PaintServerTemplates"></a>14.1.1. Using paint servers as templates

Most paint server elements accept an ‘href’ attribute, which can be used to define a compatible paint server element as a template. Attributes defined for the template element are used instead of the initial value if corresponding attributes are not specified on the current element. Furthermore, if the current element does not have any child content other than [descriptive elements](struct.md#TermDescriptiveElement), than the child content of the template element is cloned to replace it.

The exclusion of descriptive content is new in SVG 2 for ‘[pattern](#PatternElement)’, consistent with the behavior of gradients, and with changes to make descriptive content valid for any SVG element.

Also new: template cross-references may be to external file resources (different chapters in SVG 1.1 had inconsistent guidance on this point), and the "inheritance" of child elements is represented through a shadow tree.

Templating can be indirect to an arbitrary level (subject to security limits on external file resources, which can make a reference invalid). Thus, if the referenced template element does not have relevant child content or does not define the specified attribute, then the attribute value or cloned content is derived from another element referenced by the template's own ‘href’ attribute.

The description of each ‘href’ attribute in this chapter defines the limits of the templating process, as follows:

- What type of element is a valid target for the reference
- Which attributes are duplicated from the template

If any of the specified attributes are not defined on the current element, or if the current element has no child elements other than [descriptive elements](struct.md#TermDescriptiveElement), the user agent must [process the URL](linking.md#processingURL) to identify the referenced resource. If the URL reference is not [invalid](types.md#TermInvalidValue), then the URL's target element is used as the template element, as follows:

-
For any of the specified attributes not defined on the current element, the user agent must determine the value of the attribute for the template element and use it as the value for the current element. The template value is derived from recursive cross-references if required. The initial value for the attribute is only substituted after all valid URL references are exhausted.
-
If the current element has no child elements other than [descriptive elements](struct.md#TermDescriptiveElement), the user agent must generate a [shadow tree](struct.md#TermShadowTree) for this element, which must behave equivalently to a [use-element shadow tree](struct.md#TermUseElementShadowTree), except that the host is the current paint server element. The [corresponding elements](struct.md#TermCorrespondingElement) for the [element instances](struct.md#TermElementInstance) cloned into the shadow tree are:
  - the child content of the template element, if it has child elements other than [descriptive elements](struct.md#TermDescriptiveElement),
  - the [corresponding elements](struct.md#TermCorrespondingElement) that are used, or would be used, to generate the template element's own shadow tree, otherwise.

When a paint-server element has a shadow tree, the [element instances](struct.md#TermElementInstance) in that tree must be used in rendering the paint server effect, as if they were the paint server element's own children.

The [use-element shadow tree](struct.md#TermUseElementShadowTree) model for templating allows cloned content to inherit different styles than the original. This behavior is newly defined in SVG 2; SVG 1.1 did not define how styles applied to inherited paint server content.

## <a id="Gradients"></a>14.2. Gradients

Gradients consist of smooth color transitions between points on a drawing surface. SVG provides for two types of gradients:

- [linear gradients](#LinearGradients),
- [radial gradients](#RadialGradients).

Once a gradient is defined, a [graphics element](struct.md#TermGraphicsElement) can be filled or stroked with the gradient by setting the [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) properties to reference the gradient.

Color transitions for linear and radial gradients are defined by a series of color [stops](#GradientStops) along a gradient vector. A gradient normal defines how the colors in a vector are painted to the surface. For a linear gradient, a normal is a line perpendicular to the vector. For a radial gradient, a normal is a circle intersecting the vector at a right angle. Each gradient normal is painted with one color determined by the vector.

[image: Image of linear and radial gradients with vectors and normals indicated.]

Linear and radial gradients with the gradient vector indicated. The vector consists of three stops shown by small circles. One gradient normal is shown for each gradient.

For linear and radial gradients, the color value between two stops along the gradient vector is the linear interpolation, per channel, of the color for each stop, weighted by the distance from each stop.

V = C0 + ( C1 - C0 ) ( D - O0 O1 - O0 )

```
<a id="math-gradient-linear-interpolation"></a>
    $V = C0.rgba + (C1.rgba - C0.rgba) * ((D - C0.offset) / (C1.offset - C0.offset));
  
```

Where, for each channel:

- V is the interpolated result for that channel
- C_0 is the stop color for the stop with the lowest offset
- O_0 is the offset of C_0
- C_1 is the other stop color
- O_1 is the offset of C_1
- D is the distance along the gradient vector

When a [graphics element](struct.md#TermGraphicsElement) references a gradient, conceptually the [graphics element](struct.md#TermGraphicsElement) should take a copy of the gradient vector with gradient normals and treat it as part of its own geometry. Any transformations applied to the [graphics element](struct.md#TermGraphicsElement) geometry also apply to the copied gradient vector and gradient normals. Any gradient transforms that are specified on the reference gradient are applied before any [graphics element](struct.md#TermGraphicsElement) transformations are applied to the gradient.

### <a id="Definitions"></a>14.2.1. Definitions

***<a id="TermGradientElement"></a>gradient element***
: A gradient element is one that defines a gradient paint server. This specification defines the following gradient elements: ‘[linearGradient](#LinearGradientElement)’ and ‘[radialGradient](#RadialGradientElement)’.
### <a id="LinearGradients"></a>14.2.2. Linear gradients

<a id="LinearGradientElement"></a>Linear gradients are defined by a ‘[linearGradient](#LinearGradientElement)’ element.

‘*<a id="elementdef-linearGradient"></a>linearGradient*’

**Categories:**
: [Gradient element](#TermGradientElement), [never-rendered element](render.md#TermNeverRenderedElement), [paint server element](painting.md#TermPaintServerElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’

animate, animateTransform, [script](interact.md#ScriptElement), set, [stop](#StopElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- [deprecated xlink attributes](linking.md#XLinkRefAttrs) — ‘[xlink:href](linking.md#XLinkHrefAttribute)’, ‘[xlink:title](linking.md#XLinkTitleAttribute)’
- ‘[x1](#LinearGradientElementX1Attribute)’
- ‘[y1](#LinearGradientElementY1Attribute)’
- ‘[x2](#LinearGradientElementX2Attribute)’
- ‘[y2](#LinearGradientElementY2Attribute)’
- ‘[gradientUnits](#LinearGradientElementGradientUnitsAttribute)’
- ‘[gradientTransform](#LinearGradientElementGradientTransformAttribute)’
- ‘[spreadMethod](#LinearGradientElementSpreadMethodAttribute)’
- ‘[href](#LinearGradientElementHrefAttribute)’

**DOM Interfaces:**
:

- [SVGLinearGradientElement](#InterfaceSVGLinearGradientElement)

#### <a id="LinearGradientAttributes"></a>14.2.2.1. Attributes

Note that the ‘[x1](#LinearGradientElementX1Attribute)’,‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y2](#LinearGradientElementY2Attribute)’ attributes on a ‘[linearGradient](#LinearGradientElement)’ are not presentation attributes; the used value is not affected by CSS styles. The ‘[gradientTransform](#LinearGradientElementGradientTransformAttribute)’ attribute is a presentation attribute for the [transform](coords.md#TransformProperty) property.

**<a id="LinearGradientElementGradientUnitsAttribute"></a>gradientUnits**
:

Defines the coordinate system for attributes ‘[x1](#LinearGradientElementX1Attribute)’, ‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y2](#LinearGradientElementY2Attribute)’.

**Value**
: userSpaceOnUse | objectBoundingBox
**[initial value](types.md#TermInitialValue)**
: objectBoundingBox
**Animatable**
: yes
**userSpaceOnUse**
:

If gradientUnits="userSpaceOnUse", ‘[x1](#LinearGradientElementX1Attribute)’, ‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’, and ‘[y2](#LinearGradientElementY2Attribute)’ represent values in the coordinate system that results from taking the current user coordinate system in place at the time when the gradient element is referenced (i.e., the user coordinate system for the element referencing the gradient element via a [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) property) and then applying the transform specified by attribute ‘[gradientTransform](#LinearGradientElementGradientTransformAttribute)’. Percentages represent values relative to the current SVG viewport.

**objectBoundingBox**
:

If gradientUnits="objectBoundingBox", the user coordinate system for attributes ‘[x1](#LinearGradientElementX1Attribute)’, ‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y2](#LinearGradientElementY2Attribute)’ is established using the bounding box of the element to which the gradient is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute ‘[gradientTransform](#LinearGradientElementGradientTransformAttribute)’. Percentages represent values relative to the bounding box for the object.

When gradientUnits="objectBoundingBox" and ‘[gradientTransform](#LinearGradientElementGradientTransformAttribute)’ is the identity matrix, the normal of the linear gradient is perpendicular to the gradient vector in object bounding box space (i.e., the abstract coordinate system where (0,0) is at the top/left of the object bounding box and (1,1) is at the bottom/right of the object bounding box). When the object's bounding box is not square, the gradient normal which is initially perpendicular to the gradient vector within object bounding box space may render non-perpendicular relative to the gradient vector in user space. If the gradient vector is parallel to one of the axes of the bounding box, the gradient normal will remain perpendicular. This transformation is due to application of the non-uniform scaling transformation from bounding box space to user coordinate system.

**<a id="LinearGradientElementGradientTransformAttribute"></a> gradientTransform **
:

Contains the definition of an optional additional transformation from the gradient coordinate system onto the target coordinate system (i.e., 'userSpaceOnUse' or 'objectBoundingBox'). This allows for things such as skewing the gradient. This additional transformation matrix is post-multiplied to (i.e., inserted to the right of) any previously defined transformations, including the implicit transformation necessary to convert from [object bounding box units](coords.md#ObjectBoundingBox) to user coordinate system.

**Value**
: [<transform-list>](../css-transforms-1.md#typedef-transform-list)
**[initial value](types.md#TermInitialValue)**
: identity transform
**Animatable**
: yes
**<a id="LinearGradientElementX1Attribute"></a>x1**
:

‘[x1](#LinearGradientElementX1Attribute)’, ‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y2](#LinearGradientElementY2Attribute)’ define a *gradient vector* for the linear gradient. This *gradient vector* provides starting and ending points onto which the [gradient stops](#GradientStops) are mapped. The values of ‘[x1](#LinearGradientElementX1Attribute)’, ‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y2](#LinearGradientElementY2Attribute)’ can be either numbers or percentages.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0%
**Animatable**
: yes
**<a id="LinearGradientElementY1Attribute"></a>y1**
:

See ‘[x1](#LinearGradientElementX1Attribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0%
**Animatable**
: yes
**<a id="LinearGradientElementX2Attribute"></a>x2**
:

See ‘[x1](#LinearGradientElementX1Attribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 100%
**Animatable**
: yes
**<a id="LinearGradientElementY2Attribute"></a>y2**
:

See ‘[x1](#LinearGradientElementX1Attribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0%
**Animatable**
: yes
**<a id="LinearGradientElementSpreadMethodAttribute"></a> spreadMethod **
:

Indicates what happens if the gradient starts or ends inside the bounds of the *target rectangle*.

**Value**
: pad | reflect | repeat
**[initial value](types.md#TermInitialValue)**
: pad
**Animatable**
: yes
**pad**
: Use the terminal colors of the gradient to fill the remainder of the target region.
**reflect**
: Reflect the gradient pattern start-to-end, end-to-start, start-to-end, etc. continuously until the *target rectangle* is filled.
**repeat**
: Repeat the gradient pattern start-to-end, start-to-end, start-to-end, etc. continuously until the target region is filled.
[image: Image of the three possible values for 'spreadMethod'.]

Illustration of the three possible values for spreadMethod, from left to right: pad, reflect, repeat. The gradient vector spans from 40% to 60% of the bounding box width.

**<a id="LinearGradientElementHrefAttribute"></a> href **
:

A [URL reference](linking.md#TermURLReference) to a template gradient element; to be valid, the reference must be to a different ‘[linearGradient](#LinearGradientElement)’ or a ‘[radialGradient](#RadialGradientElement)’ element.

Refer to the process for [using paint servers as templates](#PaintServerTemplates), and to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).

The specified attributes that will be copied from the template are:

- ‘[x1](#LinearGradientElementX1Attribute)’
- ‘[y1](#LinearGradientElementY1Attribute)’
- ‘[x2](#LinearGradientElementX2Attribute)’
- ‘[y2](#LinearGradientElementY2Attribute)’
- ‘[gradientTransform](#LinearGradientElementGradientTransformAttribute)’
- ‘[gradientUnits](#LinearGradientElementGradientUnitsAttribute)’
- ‘[spreadMethod](#LinearGradientElementSpreadMethodAttribute)’

**Value**
: URL [[URL]](types.md#attribute-url)
**[initial value](types.md#TermInitialValue)**
: empty
**Animatable**
: yes
#### <a id="LinearGradientNotes"></a>14.2.2.2. Notes on linear gradients

If ‘[x1](#LinearGradientElementX1Attribute)’ = ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y1](#LinearGradientElementY1Attribute)’ = ‘[y2](#LinearGradientElementY2Attribute)’, then the area to be painted will be painted as a single color using the color and opacity of the last [gradient stop](#GradientStops).

<a id="ExampleLinGrad01"></a>Example lingrad01 shows how to fill a rectangle by referencing a linear gradient paint server.

```
<?xml version="1.0" standalone="no"?>
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1"
     viewBox="0 0 300 200" >

  <title>Example lingrag01</title>
  <desc>Fill a rectangle using a linear-gradient paint server.</desc>

  <defs>
    <linearGradient id="MyGradient">
      <stop offset="5%" stop-color="#A8F" />
      <stop offset="95%" stop-color="#FDC" />
    </linearGradient>
  </defs>

  <!-- The rectangle is filled using a linear-gradient paint server -->
  <rect fill="url(#MyGradient)"
	stroke="black"
	stroke-width="2"
	x="25" y="25" width="250" height="150"/>
</svg>
```

[image: Example lingrad01 — Fill a rectangle by referencing a linear gradient paint server]

Example lingrad01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/pservers/lingrad01.svg)

### <a id="RadialGradients"></a>14.2.3. Radial gradients

<a id="RadialGradientElement"></a>Radial gradients are defined by a ‘[radialGradient](#RadialGradientElement)’ element.

‘*<a id="elementdef-radialGradient"></a>radialGradient*’

**Categories:**
: [Gradient element](#TermGradientElement), [never-rendered element](render.md#TermNeverRenderedElement), [paint server element](painting.md#TermPaintServerElement)
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’

animate, animateTransform, [script](interact.md#ScriptElement), set, [stop](#StopElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- [deprecated xlink attributes](linking.md#XLinkRefAttrs) — ‘[xlink:href](linking.md#XLinkHrefAttribute)’, ‘[xlink:title](linking.md#XLinkTitleAttribute)’
- ‘[cx](#RadialGradientElementCXAttribute)’
- ‘[cy](#RadialGradientElementCYAttribute)’
- ‘[r](#RadialGradientElementRAttribute)’
- ‘[fx](#RadialGradientElementFXAttribute)’
- ‘[fy](#RadialGradientElementFYAttribute)’
- ‘[fr](#RadialGradientElementFRAttribute)’
- ‘[gradientUnits](#RadialGradientElementGradientUnitsAttribute)’
- ‘[gradientTransform](#RadialGradientElementGradientTransformAttribute)’
- ‘[spreadMethod](#RadialGradientElementSpreadMethodAttribute)’
- ‘[href](#RadialGradientElementHrefAttribute)’

**DOM Interfaces:**
:

- [SVGRadialGradientElement](#InterfaceSVGRadialGradientElement)

#### <a id="RadialGradientAttributes"></a>14.2.3.1. Attributes

Note that the ‘[cx](#RadialGradientElementCXAttribute)’,‘[cy](#RadialGradientElementCYAttribute)’, and ‘[r](#RadialGradientElementRAttribute)’ attributes on a ‘[radialGradient](#RadialGradientElement)’ are not presentation attributes; the used value is not affected by CSS styles. The ‘[gradientTransform](#RadialGradientElementGradientTransformAttribute)’ attribute is a presentation attribute for the [transform](coords.md#TransformProperty) property.

**<a id="RadialGradientElementGradientUnitsAttribute"></a> gradientUnits = "userSpaceOnUse | objectBoundingBox" **
:

Defines the coordinate system for attributes ‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’, ‘[r](#RadialGradientElementRAttribute)’, ‘[fx](#RadialGradientElementFXAttribute)’, ‘[fy](#RadialGradientElementFYAttribute)’, and ‘[fr](#RadialGradientElementFRAttribute)’.

**[initial value](types.md#TermInitialValue)**
: objectBoundingBox
**Animatable**
: yes
**userSpaceOnUse**
:

If gradientUnits="userSpaceOnUse", ‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’, ‘[r](#RadialGradientElementRAttribute)’, ‘[fx](#RadialGradientElementFXAttribute)’, ‘[fy](#RadialGradientElementFYAttribute)’, and ‘[fr](#RadialGradientElementFRAttribute)’ represent values in the coordinate system that results from taking the current user coordinate system in place at the time when the gradient element is referenced (i.e., the user coordinate system for the element referencing the gradient element via a [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) property) and then applying the transform specified by attribute ‘[gradientTransform](#RadialGradientElementGradientTransformAttribute)’. Percentages represent values relative to the current SVG viewport.

**objectBoundingBox**
:

If gradientUnits="objectBoundingBox", the user coordinate system for attributes ‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’, ‘[r](#RadialGradientElementRAttribute)’, ‘[fx](#RadialGradientElementFXAttribute)’, ‘[fy](#RadialGradientElementFYAttribute)’, and ‘[fr](#RadialGradientElementFRAttribute)’ is established using the bounding box of the element to which the gradient is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute ‘[gradientTransform](#RadialGradientElementGradientTransformAttribute)’. Percentages represent values relative to the bounding box for the object.

When gradientUnits="objectBoundingBox" and ‘[gradientTransform](#RadialGradientElementGradientTransformAttribute)’ is the identity matrix, then the rings of the radial gradient are circular with respect to the object bounding box space (i.e., the abstract coordinate system where (0,0) is at the top/left of the object bounding box and (1,1) is at the bottom/right of the object bounding box). When the object's bounding box is not square, the rings that are conceptually circular within object bounding box space will render as elliptical due to application of the non-uniform scaling transformation from bounding box space to user coordinate system.

**<a id="RadialGradientElementGradientTransformAttribute"></a> gradientTransform = "[<transform-list>](../css-transforms-1.md#typedef-transform-list)" **
:

Contains the definition of an optional additional transformation from the gradient coordinate system onto the target coordinate system (i.e., 'userSpaceOnUse' or 'objectBoundingBox'). This allows for things such as skewing the gradient. This additional transformation matrix is post-multiplied to (i.e., inserted to the right of) any previously defined transformations, including the implicit transformation necessary to convert from [object bounding box units](coords.md#ObjectBoundingBox) to user coordinate system.

**[initial value](types.md#TermInitialValue)**
: identity transform
**Animatable**
: yes
**<a id="RadialGradientElementCXAttribute"></a> cx = "<length>" **
:

‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’ and ‘[r](#RadialGradientElementRAttribute)’ define the end circle for the radial gradient. The gradient will be drawn such that the 100% [gradient stop](#GradientStops) is mapped to the perimeter of this end circle.

**[initial value](types.md#TermInitialValue)**
: 50%
**Animatable**
: yes
**<a id="RadialGradientElementCYAttribute"></a> cy = "<length>" **
:

See ‘[cx](#RadialGradientElementCXAttribute)’.

**[initial value](types.md#TermInitialValue)**
: 50%
**Animatable**
: yes
**<a id="RadialGradientElementRAttribute"></a> r = "<length>" **
:

See ‘[cx](#RadialGradientElementCXAttribute)’.

A negative value is an error (see [Error processing](conform.md#ErrorProcessing)).

**[initial value](types.md#TermInitialValue)**
: 50%
**Animatable**
: yes
**<a id="RadialGradientElementFXAttribute"></a> fx = "<length>" **
:

‘[fx](#RadialGradientElementFXAttribute)’, ‘[fy](#RadialGradientElementFYAttribute)’, and ‘[fr](#RadialGradientElementFRAttribute)’ define the start circle for the radial gradient. The gradient will be drawn such that the 0% [gradient stop](#GradientStops) is mapped to the perimeter of this start circle.

**[initial value](types.md#TermInitialValue)**
: see below
**Animatable**
: yes
If attribute ‘[fx](#RadialGradientElementFXAttribute)’ is not specified, ‘[fx](#RadialGradientElementFXAttribute)’ will coincide with the presentational value of ‘[cx](#RadialGradientElementCXAttribute)’ for the element whether the value for 'cx' was inherited or not. If the element references an element that specifies a value for 'fx', then the value of 'fx' is inherited from the referenced element.

[image: Diagram of various radial gradient attributes.]

This diagram shows how the geometric attributes are defined for the case where ‘[fr](#RadialGradientElementFRAttribute)’ is 50% of ‘[r](#RadialGradientElementRAttribute)’. The small circle marks the center of the outermost circle (‘[cx](#RadialGradientElementCXAttribute)’,‘[cy](#RadialGradientElementCYAttribute)’), while the cross marks the center of the innermost circle (‘[fx](#RadialGradientElementFXAttribute)’,‘[fy](#RadialGradientElementFYAttribute)’). The dashed lines show two gradient vectors. Vectors connect corresponding points on the inner and outer most circles. The region outside the outer circle is painted with the last [stop-color](#StopColorProperty) while the region inside the inner circle is painted with the first [stop-color](#StopColorProperty).

**<a id="RadialGradientElementFYAttribute"></a> fy = "<length>" **
:

See ‘[fx](#RadialGradientElementFXAttribute)’.

**[initial value](types.md#TermInitialValue)**
: see below
**Animatable**
: yes
If attribute ‘[fy](#RadialGradientElementFYAttribute)’ is not specified, ‘[fy](#RadialGradientElementFYAttribute)’ will coincide with the presentational value of ‘[cy](#RadialGradientElementCYAttribute)’ for the element whether the value for 'cy' was inherited or not. If the element references an element that specifies a value for 'fy', then the value of 'fy' is inherited from the referenced element.

**<a id="RadialGradientElementFRAttribute"></a> fr = "<length>" **
:

New in SVG 2. Added to align with Canvas.

‘[fr](#RadialGradientElementFRAttribute)’ is the radius of the focal circle. See ‘[fx](#RadialGradientElementFXAttribute)’.

A negative value is an error (see [Error processing](conform.md#ErrorProcessing)).

**[initial value](types.md#TermInitialValue)**
: 0%, see below
**Animatable**
: yes
If the attribute is not specified, the effect is as if a value of '0%' were specified. If the element references an element that specifies a value for 'fr', then the value of 'fr' is inherited from the referenced element.

**<a id="RadialGradientElementSpreadMethodAttribute"></a> spreadMethod = "pad | reflect | repeat" **
:

Indicates what happens if the gradient starts or ends inside the bounds of the object(s) being painted by the gradient. Has the same values and meanings as the ‘[spreadMethod](#LinearGradientElementSpreadMethodAttribute)’ attribute on ‘[linearGradient](#LinearGradientElement)’ element.

**[initial value](types.md#TermInitialValue)**
: pad
**Animatable**
: yes
**<a id="RadialGradientElementHrefAttribute"></a> href = [[URL]](types.md#attribute-url) **
:

A [URL reference](linking.md#TermURLReference) to a template gradient element; to be valid, the reference must be to a ‘[linearGradient](#LinearGradientElement)’ element or a different ‘[radialGradient](#RadialGradientElement)’ element.

Refer to the process for [using paint servers as templates](#PaintServerTemplates), and to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).

The specified attributes that will be copied from the template are:

- ‘[cx](#RadialGradientElementCXAttribute)’
- ‘[cy](#RadialGradientElementCYAttribute)’
- ‘[r](#RadialGradientElementRAttribute)’
- ‘[fx](#RadialGradientElementFXAttribute)’
- ‘[fy](#RadialGradientElementFYAttribute)’
- ‘[fr](#RadialGradientElementFRAttribute)’
- ‘[gradientTransform](#RadialGradientElementGradientTransformAttribute)’
- ‘[gradientUnits](#RadialGradientElementGradientUnitsAttribute)’
- ‘[spreadMethod](#RadialGradientElementSpreadMethodAttribute)’

Refer to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).

**[initial value](types.md#TermInitialValue)**
: empty
**Animatable**
: yes
#### <a id="RadialGradientNotes"></a>14.2.3.2. Notes on radial gradients

Changed in SVG 2. SVG 1.1 required that the focal point, if outside the end circle, be moved to be on the end circle. The change was made to align with Canvas.

If the start circle defined by ‘[fx](#RadialGradientElementFXAttribute)’, ‘[fy](#RadialGradientElementFYAttribute)’ and ‘[fr](#RadialGradientElementFRAttribute)’ lies outside the end circle defined by ‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’, and ‘[r](#RadialGradientElementRAttribute)’, effectively a cone is created, touched by the two circles. Areas outside the cone stay untouched by the gradient (transparent black).

If the start circle fully overlaps with the end circle, no gradient is drawn. The area stays untouched (transparent black).

[image: Image of a radial gradient with the focal (start circle) outside the start circle.]

A radial gradient with the focal (start) circle outside the end circle. The focal circle is the smaller circle on the right. The gradient has spreadMethod="reflect".

[image: Image of two radial gradients, one with the focus just inside the circumference and one with the focus on the circumference.]

Two radial gradients with spreadMethod="repeat". On the left, the focal point is just inside the right side of the circle defined by ‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’, and ‘[r](#RadialGradientElementRAttribute)’. On the right, the focal point is on the circle. In this case, the area painted to the right of the circumference has a fill equal to the weighted average of the colors in the gradient vector.

The treatment of the area to the right of the gradient in the right-hand side of the above figure is different from that of Canvas where the area would be transparent black. The difference is to maintain compatibility with SVG 1.1.

<a id="ExampleRadGrad01"></a>Example radgrad01 shows how to fill a rectangle by referencing a radial gradient paint server.

```
<?xml version="1.0" standalone="no"?>
<svg xmlns="http://www.w3.org/2000/svg"
     viewBox="0 0 300 200" >
  <title>Example radgrad01</title>
  <desc>Fill a rectangle by referencing a radial gradient paint server.</desc>

  <defs>
    <radialGradient id="MyGradient"
		    gradientUnits="userSpaceOnUse"
		    cx="150" cy="100"
		    r="100">
      <stop offset="0%"   stop-color="#A8F" />
      <stop offset="50%"  stop-color="#FDC" />
      <stop offset="100%" stop-color="#A8F" />
    </radialGradient>
  </defs>

  <!-- The rectangle is filled using a radial gradient paint server -->
  <rect fill="url(#MyGradient)"
	stroke="black"
	stroke-width="2"
	x="25" y="25" width="250" height="150"/>
</svg>
```

[image: Example radgrad01 — Fill a rectangle by referencing a radial gradient paint server]

Example radgrad01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/pservers/radgrad01.svg)

### <a id="GradientStops"></a>14.2.4. Gradient stops

<a id="StopElement"></a>The vector of colors to use in a gradient is defined by the ‘[stop](#StopElement)’ elements that are child elements to a ‘[linearGradient](#LinearGradientElement)’, or ‘[radialGradient](#RadialGradientElement)’ element.

‘*<a id="elementdef-stop"></a>stop*’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

animate, [script](interact.md#ScriptElement), set, [style](styling.md#StyleElement)
**Attributes:**
:

- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[offset](#StopElementOffsetAttribute)’

**DOM Interfaces:**
:

- [SVGStopElement](#InterfaceSVGStopElement)

#### <a id="GradientStopAttributes"></a>14.2.4.1. Attributes

**<a id="StopElementOffsetAttribute"></a> offset **
:

Indicates were the gradient stop is placed. For linear gradients, the ‘[offset](#StopElementOffsetAttribute)’ attribute represents a location along the *gradient vector*. For radial gradients, it represents a fractional distance from the edge of the innermost/smallest circle to the edge of the outermost/largest circle.

**Value**
: <number> | <percentage>
**[initial value](types.md#TermInitialValue)**
: 0
**Animatable**
: yes
**<number>**
: A number usually ranging from 0 to 1.
**<percentage>**
: A percentage usually ranging from 0% to 100%.
#### <a id="StopColorProperties"></a>14.2.4.2. Properties

**<a id="StopColorProperty"></a>‘stop-color’**
:

The [stop-color](#StopColorProperty) property indicates what color to use at that gradient stop. The keyword currentColor and other color syntaxes can be specified in the same manner as within a [<paint>](painting.md#SpecifyingPaint) specification for the [fill](painting.md#FillProperty) and [stroke](painting.md#StrokeProperty) properties.

With respect to gradients, SVG treats the 'transparent' keyword differently than CSS. SVG does not calculate gradients in pre-multiplied space, so 'transparent' really means transparent black. Specifying a [stop-color](#StopColorProperty) with the value 'transparent' is equivalent to specifying a [stop-color](#StopColorProperty) with the value 'black' and a [stop-opacity](#StopOpacityProperty) with the value '0'.

**Value**
: <‘[color](painting.md#ColorProperty)’>
**Initial**
: black
**Applies to**
: ‘[stop](#StopElement)’ elements
**Inherited**
: no
**Percentages**
: N/A
**Media**
: visual
**Animatable**
: yes
**<a id="StopOpacityProperty"></a>‘stop-opacity’**
:

The [stop-opacity](#StopOpacityProperty) property defines the opacity of a given gradient stop. The opacity value used for the gradient calculation is the product of the value of [stop-opacity](#StopOpacityProperty) and the opacity of the value of [stop-color](#StopColorProperty). For [stop-color](#StopColorProperty) value types of that don't include explicit opacity information, the opacity of that component must be treated as 1.

**Value**
: <‘[opacity](render.md#ObjectAndGroupOpacityProperties)’>
**Initial**
: 1
**Applies to**
: ‘[stop](#StopElement)’ elements
**Inherited**
: no
**Percentages**
: N/A
**Media**
: visual
**Computed value**
: the specified value converted to a number, clamped to the range [0,1]
**Animatable**
: yes
**<number>**
: The opacity of the [stop-color](#StopColorProperty). Any values outside the range 0.0 (fully transparent) to 1.0 (fully opaque) must be clamped to this range.
**<percentage>**
: The opacity of the [stop-color](#StopColorProperty) expressed as a percentage of the range 0 to 1.
#### <a id="StopNotes"></a>14.2.4.3. Notes on gradient stops

- Gradient offset values less than 0 (or less than 0%) are rounded up to 0%. Gradient offset values greater than 1 (or greater than 100%) are rounded down to 100%.
- It is necessary that at least two stops defined to have a gradient effect. If no stops are defined, then painting shall occur as if 'none' were specified as the paint style. If one stop is defined, then paint with the solid color fill using the color defined for that gradient stop.
- Each gradient offset value is required to be equal to or greater than the previous gradient stop's offset value. If a given gradient stop's offset value is not equal to or greater than all previous offset values, then the offset value is adjusted to be equal to the largest of all previous offset values.
-
If two gradient stops have the same offset value, then the latter gradient stop controls the color value at the overlap point. In particular:
```
<stop offset="0" stop-color="white"/>
<stop offset=".2" stop-color="red"/>
<stop offset=".2" stop-color="blue"/>
<stop offset="1" stop-color="black"/>
```

will have approximately the same effect as:
```
<stop offset="0" stop-color="white"/>
<stop offset=".1999999999" stop-color="red"/>
<stop offset=".2" stop-color="blue"/>
<stop offset="1" stop-color="black"/>
```

which is a gradient that goes smoothly from white to red, then abruptly shifts from red to blue, and then goes smoothly from blue to black.

## <a id="Patterns"></a>14.3. Patterns

<a id="PatternElement"></a>A pattern is used to [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) an object using a pre-defined graphic object which can be replicated ("tiled") at fixed intervals in *x* and *y* to cover the areas to be painted. Patterns are defined using a ‘[pattern](#PatternElement)’ element and then referenced by properties [fill](painting.md#FillProperty) and [stroke](painting.md#StrokeProperty) on a given [graphics element](struct.md#TermGraphicsElement) to indicate that the given element shall be filled or stroked with the pattern.

Attributes ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’, ‘[height](#PatternElementHeightAttribute)’ and ‘[patternUnits](#PatternElementPatternUnitsAttribute)’ define a reference rectangle somewhere on the infinite canvas. The reference rectangle has its top/left at (*x*, *y*) and its bottom/right at (*x* + *width*, *y* + *height*). The tiling theoretically extends a series of such rectangles to infinity in X and Y (positive and negative), with rectangles starting at (*x* + m**width*, *y* + n* *height*) for each possible integer value for *m* and *n*.

‘*<a id="elementdef-pattern"></a>pattern*’

**Categories:**
: [Container element](struct.md#TermContainerElement), [never-rendered element](render.md#TermNeverRenderedElement), [paint server element](painting.md#TermPaintServerElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](#LinearGradientElement)’, ‘[radialGradient](#RadialGradientElement)’, ‘[pattern](#PatternElement)’
- [shape elements](shapes.md#TermShapeElement) — ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’
- [structural elements](struct.md#TermStructuralElement) — ‘[defs](struct.md#DefsElement)’, ‘[g](struct.md#GElement)’, ‘[svg](struct.md#SVGElement)’, ‘[symbol](struct.md#SymbolElement)’, ‘[use](struct.md#UseElement)’

[a](linking.md#AElement), [clipPath](../css-masking-1.md#ClipPathElement), [filter](../filter-effects-1.md#FilterElement), [foreignObject](embedded.md#ForeignObjectElement), [image](embedded.md#ImageElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement), [switch](struct.md#SwitchElement), [text](text.md#TextElement), [view](linking.md#ViewElement)
**Attributes:**
:

- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- [deprecated xlink attributes](linking.md#XLinkRefAttrs) — ‘[xlink:href](linking.md#XLinkHrefAttribute)’, ‘[xlink:title](linking.md#XLinkTitleAttribute)’
- ‘[viewBox](coords.md#ViewBoxAttribute)’
- ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’
- ‘[x](#PatternElementXAttribute)’
- ‘[y](#PatternElementYAttribute)’
- ‘[width](#PatternElementWidthAttribute)’
- ‘[height](#PatternElementHeightAttribute)’
- ‘[patternUnits](#PatternElementPatternUnitsAttribute)’
- ‘[patternContentUnits](#PatternElementPatternContentUnitsAttribute)’
- ‘[patternTransform](#PatternElementPatternTransformAttribute)’
- ‘[href](#PatternElementHrefAttribute)’

**DOM Interfaces:**
:

- [SVGPatternElement](#InterfaceSVGPatternElement)

### <a id="PatternElementAttributes"></a>14.3.1. Attributes

Note that the ‘[x](#PatternElementXAttribute)’,‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’ and ‘[height](#PatternElementHeightAttribute)’ attributes on a ‘[pattern](#PatternElement)’ are not presentation attributes; the used value is not affected by CSS styles. The ‘[patternTransform](#PatternElementPatternTransformAttribute)’ attribute is a presentation attribute for the [transform](coords.md#TransformProperty) property.

**<a id="PatternElementPatternUnitsAttribute"></a>patternUnits**
:

Defines the coordinate system for attributes ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’ and ‘[height](#PatternElementHeightAttribute)’.

**Value**
: userSpaceOnUse | objectBoundingBox
**[initial value](types.md#TermInitialValue)**
: objectBoundingBox
**Animatable**
: yes
**userSpaceOnUse**
:

If patternUnits="userSpaceOnUse", ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’ and ‘[height](#PatternElementHeightAttribute)’ represent values in the coordinate system that results from taking the current user coordinate system in place at the time when the ‘[pattern](#PatternElement)’ element is referenced (i.e., the user coordinate system for the element referencing the ‘[pattern](#PatternElement)’ element via a [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) property) and then applying the transform specified by attribute ‘[patternTransform](#PatternElementPatternTransformAttribute)’. Percentages represent values relative to the current SVG viewport.

**objectBoundingBox**
:

If patternUnits="objectBoundingBox", the user coordinate system for attributes ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’ and ‘[height](#PatternElementHeightAttribute)’ is established using the bounding box of the element to which the pattern is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute ‘[patternTransform](#PatternElementPatternTransformAttribute)’. Percentages represent values relative to the bounding box for the object.

**<a id="PatternElementPatternContentUnitsAttribute"></a> patternContentUnits **
:

Defines the coordinate system for the contents of the ‘[pattern](#PatternElement)’. Note that this attribute has no effect if attribute ‘[viewBox](coords.md#ViewBoxAttribute)’ is specified.

**Value**
: userSpaceOnUse | objectBoundingBox
**[initial value](types.md#TermInitialValue)**
: userSpaceOnUse
**Animatable**
: yes
**userSpaceOnUse**
:

If patternContentUnits="userSpaceOnUse", the user coordinate system for the contents of the ‘[pattern](#PatternElement)’ element is the coordinate system that results from taking the current user coordinate system in place at the time when the ‘[pattern](#PatternElement)’ element is referenced (i.e., the user coordinate system for the element referencing the ‘[pattern](#PatternElement)’ element via a [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) property) and then applying the transform specified by attribute ‘[patternTransform](#PatternElementPatternTransformAttribute)’.

**objectBoundingBox**
:

If patternContentUnits="objectBoundingBox", the user coordinate system for the contents of the ‘[pattern](#PatternElement)’ element is established using the bounding box of the element to which the pattern is applied (see [Object bounding box units](coords.md#ObjectBoundingBox)) and then applying the transform specified by attribute ‘[patternTransform](#PatternElementPatternTransformAttribute)’.

**<a id="PatternElementPatternTransformAttribute"></a> patternTransform **
:

Contains the definition of an optional additional transformation from the pattern coordinate system onto the target coordinate system (i.e., 'userSpaceOnUse' or 'objectBoundingBox'). This allows for things such as skewing the pattern tiles. This additional transformation matrix is post-multiplied to (i.e., inserted to the right of) any previously defined transformations, including the implicit transformation necessary to convert from [object bounding box units](coords.md#ObjectBoundingBox) to user coordinate system.

**Value**
: [<transform-list>](../css-transforms-1.md#typedef-transform-list)
**[initial value](types.md#TermInitialValue)**
: identity transform
**Animatable**
: yes
**<a id="PatternElementXAttribute"></a>x**
:

‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’ and ‘[height](#PatternElementHeightAttribute)’ indicate how the pattern tiles are placed and spaced. These attributes represent coordinates and values in the coordinate space specified by the combination of attributes ‘[patternUnits](#PatternElementPatternUnitsAttribute)’ and ‘[patternTransform](#PatternElementPatternTransformAttribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0
**Animatable**
: yes
**<a id="PatternElementYAttribute"></a>y**
:

See ‘[x](#PatternElementXAttribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0
**Animatable**
: yes
**<a id="PatternElementWidthAttribute"></a>width**
:

See ‘[x](#PatternElementXAttribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0
**Animatable**
: yes
A negative value is an error (see [Error processing](conform.md#ErrorProcessing)). A value of zero disables rendering of the element (i.e., no paint is applied).

**<a id="PatternElementHeightAttribute"></a>height**
:

See ‘[x](#PatternElementXAttribute)’.

**Value**
: <length>
**[initial value](types.md#TermInitialValue)**
: 0
**Animatable**
: yes
A negative value is an error (see [Error processing](conform.md#ErrorProcessing)). A value of zero disables rendering of the element (i.e., no paint is applied).

**<a id="PatternElementHrefAttribute"></a>href**
:

A [URL reference](linking.md#TermURLReference) to a template element, which must be a different ‘[pattern](#PatternElement)’ element to be valid.

Refer to the process for [using paint servers as templates](#PaintServerTemplates), and to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).

The specified attributes that will be copied from the template are:

- ‘[x](#PatternElementXAttribute)’
- ‘[y](#PatternElementYAttribute)’
- ‘[width](#PatternElementWidthAttribute)’
- ‘[height](#PatternElementHeightAttribute)’
- ‘[viewBox](coords.md#ViewBoxAttribute)’
- ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’
- ‘[patternTransform](#PatternElementPatternTransformAttribute)’
- ‘[patternUnits](#PatternElementPatternUnitsAttribute)’
- ‘[patternContentUnits](#PatternElementPatternContentUnitsAttribute)’

**Value**
: URL [[URL]](types.md#attribute-url)
**[initial value](types.md#TermInitialValue)**
: empty
**Animatable**
: yes
### <a id="PatternNotes"></a>14.3.2. Notes on patterns

SVG's [user agent style sheet](styling.md#UAStyleSheet) sets the [overflow](render.md#OverflowAndClipProperties) property for ‘[pattern](#PatternElement)’ elements to hidden, which causes a rectangular clipping path to be created at the bounds of the pattern tile. Unless the [overflow](render.md#OverflowAndClipProperties) property is overridden, any graphics within the pattern which goes outside of the pattern rectangle will be clipped. [Example pattern01](#ExamplePattern01) below shows the effect of clipping to the pattern tile.

Note that if the [overflow](render.md#OverflowAndClipProperties) property is set to visible the rendering behavior for the pattern outside the bounds of the pattern is currently undefined. A future version of SVG may require the overflow to be shown. SVG implementers are encouraged to render the overflow as this is the behavior expected by authors. If overflow is rendered, the pattern tiles should be rendered left to right in rows and the rows from top to bottom.

The contents of the ‘[pattern](#PatternElement)’ are relative to a new coordinate system. If there is a ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute, then the new coordinate system is fitted into the region defined by the ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’, ‘[height](#PatternElementHeightAttribute)’ and ‘[patternUnits](#PatternElementPatternUnitsAttribute)’ attributes on the ‘[pattern](#PatternElement)’ element using the standard rules for ‘[viewBox](coords.md#ViewBoxAttribute)’ and ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’. If there is no ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute, then the new coordinate system has its origin at (*x*, *y*), where *x* is established by the ‘[x](#PatternElementXAttribute)’ attribute on the ‘[pattern](#PatternElement)’ element, and *y* is established by the ‘[y](#PatternElementYAttribute)’ attribute on the ‘[pattern](#PatternElement)’ element. Thus, in the following example:

```
<pattern x="10" y="10" width="20" height="20">
  <rect x="5" y="5" width="10" height="10"/>
</pattern>
```

the rectangle has its top/left located 5 units to the right and 5 units down from the origin of the pattern tile.

The ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute introduces a supplemental transformation which is applied on top of any transformations necessary to create a new pattern coordinate system due to attributes ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’, ‘[height](#PatternElementHeightAttribute)’ and ‘[patternUnits](#PatternElementPatternUnitsAttribute)’.

[Event attributes and event listeners](interact.md#EventAttributes) attached to the contents of a ‘[pattern](#PatternElement)’ element are not processed; only the rendering aspects of ‘[pattern](#PatternElement)’ elements are processed.

<a id="ExamplePattern01"></a>Example pattern01 shows how to fill a rectangle by referencing a pattern paint server. Note how the blue stroke of each triangle has been slightly clipped at the top and the left. This is due to SVG's [user agent style sheet](styling.md#UAStyleSheet) setting the [overflow](render.md#OverflowAndClipProperties) property for ‘[pattern](#PatternElement)’ elements to hidden, which causes the pattern to be clipped to the bounds of the pattern tile.

```
<?xml version="1.0" standalone="no"?>
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1"
     viewBox="0 0 300 200" >

  <title>Example pattern01</title>
  <desc>Fill an ellipse using a pattern paint server.</desc>

  <defs>
    <pattern id="TrianglePattern"
	     patternUnits="userSpaceOnUse"
             x="0" y="0" width="50" height="50"
             viewBox="0 0 10 10" >
      <path d="M 0 0 L 7 0 L 3.5 7 z"
	    fill="plum"
	    stroke="blue" />
    </pattern> 
  </defs>

  <!-- The ellipse is filled using a triangle pattern paint server -->
  <ellipse fill="url(#TrianglePattern)"
	   stroke="black"
	   stroke-width="2"
           cx="150" cy="100" rx="125" ry="75" />
</svg>
```

[image: Example pattern01 — fill a rectangle by referencing a pattern paint server]

Example pattern01

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/pservers/pattern01.svg)

## <a id="DOMInterfaces"></a>14.4. DOM interfaces

### <a id="InterfaceSVGGradientElement"></a>14.4.1. Interface SVGGradientElement

The [SVGGradientElement](#InterfaceSVGGradientElement) interface is used as a base interface for gradient paint server element interfaces.

```
[Exposed=Window]
interface SVGGradientElement : SVGElement {

  // Spread Method Types
  const unsigned short SVG_SPREADMETHOD_UNKNOWN = 0;
  const unsigned short SVG_SPREADMETHOD_PAD = 1;
  const unsigned short SVG_SPREADMETHOD_REFLECT = 2;
  const unsigned short SVG_SPREADMETHOD_REPEAT = 3;

  [SameObject] readonly attribute SVGAnimatedEnumeration gradientUnits;
  [SameObject] readonly attribute SVGAnimatedTransformList gradientTransform;
  [SameObject] readonly attribute SVGAnimatedEnumeration spreadMethod;
};

SVGGradientElement includes SVGURIReference;
```

The numeric spread method type constants defined on [SVGGradientElement](#InterfaceSVGGradientElement) are used to represent the keyword values that the ‘spreadMethod’ attribute can take. Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_PAD"></a>SVG_SPREADMETHOD_PAD** | The pad keyword. |
| **<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_REFLECT"></a>SVG_SPREADMETHOD_REFLECT** | The reflect keyword. |
| **<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_REPEAT"></a>SVG_SPREADMETHOD_REPEAT** | The repeat keyword. |
| **<a id="__svg__SVGGradientElement__SVG_SPREADMETHOD_UNKNOWN"></a>SVG_SPREADMETHOD_UNKNOWN** | Some other value. |

The **<a id="__svg__SVGGradientElement__gradientUnits"></a>gradientUnits** IDL attribute [reflects](types.md#TermReflect) the ‘gradientUnits’ content attribute. The [numeric type values](types.md#TermNumericTypeValue) for ‘gradientUnits’ attributes on gradient elements are as follows:

| Value | Numeric type value |
| --- | --- |
| userSpaceOnUse | [SVG_UNIT_TYPE_USERSPACEONUSE](types.md#__svg__SVGUnitTypes__SVG_UNIT_TYPE_USERSPACEONUSE) |
| objectBoundingBox | [SVG_UNIT_TYPE_OBJECTBOUNDINGBOX](types.md#__svg__SVGUnitTypes__SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) |

The **<a id="__svg__SVGGradientElement__gradientTransform"></a>gradientTransform** IDL attribute [reflects](types.md#TermReflect) the computed value of the [transform](coords.md#TransformProperty) property and the 'gradientTransform' presentation attribute for ‘[linearGradient](#LinearGradientElement)’ and ‘[radialGradient](#RadialGradientElement)’ elements.

The **<a id="__svg__SVGGradientElement__spreadMethod"></a>spreadMethod** IDL attribute [reflects](types.md#TermReflect) the ‘spreadMethod’ content attribute. The [numeric type values](types.md#TermNumericTypeValue) for ‘spreadMethod’ attributes on gradient elements are as described above in the numeric spread type constant table.

### <a id="InterfaceSVGLinearGradientElement"></a>14.4.2. Interface SVGLinearGradientElement

An [SVGLinearGradientElement](#InterfaceSVGLinearGradientElement) object represents an ‘[linearGradient](#LinearGradientElement)’ in the DOM.

```
[Exposed=Window]
interface SVGLinearGradientElement : SVGGradientElement {
  [SameObject] readonly attribute SVGAnimatedLength x1;
  [SameObject] readonly attribute SVGAnimatedLength y1;
  [SameObject] readonly attribute SVGAnimatedLength x2;
  [SameObject] readonly attribute SVGAnimatedLength y2;
};
```

The **<a id="__svg__SVGLinearGradientElement__x1"></a>x1**, **<a id="__svg__SVGLinearGradientElement__y1"></a>y1**, **<a id="__svg__SVGLinearGradientElement__x2"></a>x2** and **<a id="__svg__SVGLinearGradientElement__y2"></a>y2** IDL attributes [reflect](types.md#TermReflect) the ‘[x1](#LinearGradientElementX1Attribute)’, ‘[y1](#LinearGradientElementY1Attribute)’, ‘[x2](#LinearGradientElementX2Attribute)’ and ‘[y2](#LinearGradientElementY2Attribute)’ content attributes, respectively

### <a id="InterfaceSVGRadialGradientElement"></a>14.4.3. Interface SVGRadialGradientElement

An [SVGRadialGradientElement](#InterfaceSVGRadialGradientElement) object represents an ‘[radialGradient](#RadialGradientElement)’ in the DOM.

```
[Exposed=Window]
interface SVGRadialGradientElement : SVGGradientElement {
  [SameObject] readonly attribute SVGAnimatedLength cx;
  [SameObject] readonly attribute SVGAnimatedLength cy;
  [SameObject] readonly attribute SVGAnimatedLength r;
  [SameObject] readonly attribute SVGAnimatedLength fx;
  [SameObject] readonly attribute SVGAnimatedLength fy;
  [SameObject] readonly attribute SVGAnimatedLength fr;
};
```

The **<a id="__svg__SVGRadialGradientElement__cx"></a>cx**, **<a id="__svg__SVGRadialGradientElement__cy"></a>cy**, **<a id="__svg__SVGRadialGradientElement__r"></a>r**, **<a id="__svg__SVGRadialGradientElement__fx"></a>fx**, **<a id="__svg__SVGRadialGradientElement__fy"></a>fy** and **<a id="__svg__SVGRadialGradientElement__fr"></a>fr** IDL attributes [reflect](types.md#TermReflect) the ‘[cx](#RadialGradientElementCXAttribute)’, ‘[cy](#RadialGradientElementCYAttribute)’, ‘[r](#RadialGradientElementRAttribute)’, ‘[fx](#RadialGradientElementFXAttribute)’, ‘[fy](#RadialGradientElementFYAttribute)’ and ‘[fr](#RadialGradientElementFRAttribute)’ content attributes, respectively

### <a id="InterfaceSVGStopElement"></a>14.4.4. Interface SVGStopElement

An [SVGStopElement](#InterfaceSVGStopElement) object represents a ‘[stop](#StopElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGStopElement : SVGElement {
  [SameObject] readonly attribute SVGAnimatedNumber offset;
};
```

The **<a id="__svg__SVGStopElement__offset"></a>offset** IDL attribute [reflects](types.md#TermReflect) the ‘[offset](#StopElementOffsetAttribute)’ content attribute.

Note that [SVGStopElement](#InterfaceSVGStopElement) does not have a [reflecting](types.md#TermReflect) IDL attribute for its ‘[path](paths.md#PathElement)’ attribute.

### <a id="InterfaceSVGPatternElement"></a>14.4.5. Interface SVGPatternElement

An [SVGPatternElement](#InterfaceSVGPatternElement) object represents a ‘[pattern](#PatternElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGPatternElement : SVGElement {
  [SameObject] readonly attribute SVGAnimatedEnumeration patternUnits;
  [SameObject] readonly attribute SVGAnimatedEnumeration patternContentUnits;
  [SameObject] readonly attribute SVGAnimatedTransformList patternTransform;
  [SameObject] readonly attribute SVGAnimatedLength x;
  [SameObject] readonly attribute SVGAnimatedLength y;
  [SameObject] readonly attribute SVGAnimatedLength width;
  [SameObject] readonly attribute SVGAnimatedLength height;
};

SVGPatternElement includes SVGFitToViewBox;
SVGPatternElement includes SVGURIReference;
```

The **<a id="__svg__SVGPatternElement__patternUnits"></a>patternUnits** and **<a id="__svg__SVGPatternElement__patternContentUnits"></a>patternContentUnits** IDL attributes [reflect](types.md#TermReflect) the ‘[patternUnits](#PatternElementPatternUnitsAttribute)’ and ‘[patternContentUnits](#PatternElementPatternContentUnitsAttribute)’ content attributes, respectively. The [numeric type values](types.md#TermNumericTypeValue) for ‘[patternUnits](#PatternElementPatternUnitsAttribute)’ and ‘[patternContentUnits](#PatternElementPatternContentUnitsAttribute)’ are as follows:

| Value | Numeric type value |
| --- | --- |
| userSpaceOnUse | [SVG_UNIT_TYPE_USERSPACEONUSE](types.md#__svg__SVGUnitTypes__SVG_UNIT_TYPE_USERSPACEONUSE) |
| objectBoundingBox | [SVG_UNIT_TYPE_OBJECTBOUNDINGBOX](types.md#__svg__SVGUnitTypes__SVG_UNIT_TYPE_OBJECTBOUNDINGBOX) |

The **<a id="__svg__SVGPatternElement__patternTransform"></a>patternTransform** IDL attribute [reflects](types.md#TermReflect) the computed value of the [transform](coords.md#TransformProperty) property and the ‘patternTransform’ presentation attribute.

The **<a id="__svg__SVGPatternElement__x"></a>x**, **<a id="__svg__SVGPatternElement__y"></a>y**, **<a id="__svg__SVGPatternElement__width"></a>width** and **<a id="__svg__SVGPatternElement__height"></a>height** IDL attributes [reflect](types.md#TermReflect) the ‘[x](#PatternElementXAttribute)’, ‘[y](#PatternElementYAttribute)’, ‘[width](#PatternElementWidthAttribute)’ and ‘[height](#PatternElementHeightAttribute)’ content attributes, respectively.
