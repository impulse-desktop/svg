<!-- https://svgwg.org/svg2-draft/coords.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 8: Coordinate Systems, Transformations and Units

## <a id="Introduction"></a>8.1. Introduction

All SVG content is drawn inside *<a id="TermSVGViewport"></a>SVG viewports*. Every SVG viewport defines a drawing region characterized by a size (width, height), and an origin, measured in abstract *<a id="TermUserUnits"></a>user units*.

Note that the term SVG viewport is distinct from the "viewport" term used in CSS.

The *<a id="TermInitialViewport"></a>initial viewport* is a top-level SVG viewport that establishes a mapping between the coordinate system used by the containing environment (for example, CSS pixels in web browsers) and [user units](#TermUserUnits). Establishing an initial viewport is described in more detail in [The initial viewport](#ViewportSpace).

SVG viewports are only established by elements. See [Establishing a new SVG viewport](#EstablishingANewSVGViewport) for information on which elements generate viewports.

Each SVG viewport generates a *<a id="TermViewportCoordinateSystem"></a>viewport coordinate system* and a *<a id="TermUserCoordinateSystem"></a>user coordinate system*, initially identical. Providing a ‘[viewBox](#ViewBoxAttribute)’ on a viewport's element transforms the user coordinate system relative to the viewport coordinate system as described in The ‘[viewBox](#ViewBoxAttribute)’ attribute. Child elements of a viewport can further modify the [user coordinate system](../css-transforms-1.md#user-coordinate-system), for example by specifying the [transform](#TransformProperty) property.

SVG viewports can be nested. Percentage units are resolved with reference to the user coordinate system of the nearest ancestral viewport-defining element, as defined in [the section on Units](#Units). Hence, nesting SVG viewports provides an opportunity to redefine the meaning of percentage units and provide a new reference rectangle for "fitting" a graphic relative to a particular rectangular area. The *<a id="TermFurthestAncestorSVGViewport"></a> furthest ancestral SVG viewport* is the top most root SVG viewport with out leaving the [SVG context](#TermSVGContext). An ancestor SVG viewport might not be independent of the DOM tree order. E.g. for ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’, ‘[mask](../css-masking-1.md#MaskElement)’, ‘[clipPath](../css-masking-1.md#ClipPathElement)’ ‘[symbol](struct.md#SymbolElement)’ or ‘[use](struct.md#UseElement)’ elements.

An *<a id="TermSVGContext"></a>SVG context* is a document fragment where all elements within the fragment have the [SVGElement](types.md#InterfaceSVGElement) as prototype.

The width, height and origin of SVG viewports is established by a negotiation process between the SVG document fragment generating the SVG viewport, and the parent of that fragment (whether real or implicit). See [Establishing a new SVG viewport](#EstablishingANewSVGViewport) for a description of this negotiation process.

By default, a nested SVG viewport's [viewport coordinate system](#TermViewportCoordinateSystem) is equivalent to the local coordinate system of the parent element, translated by the origin of the SVG viewport's element. However, a [transform](#TransformProperty) property on an SVG viewport's element will modify the [viewport coordinate system](#TermViewportCoordinateSystem) relative to the parent element's user coordinate system.

Abstractly, all SVG viewports are embedded in the *<a id="TermCanvas"></a>canvas*, a drawing region that is infinitely large in all relevant dimensions.

## <a id="ComputingAViewportsTransform"></a>8.2. Computing the equivalent transform of an SVG viewport

This process converts the min-x, min-y, width and height values of a viewBox attribute, the position and size of the element on which the viewBox attribute is defined, and the value of the preserveAspectRatio attribute on that element into a translation and a scale that is applied to content contained by the element.

1. Let *vb-x*, *vb-y*, *vb-width*, *vb-height* be the min-x, min-y, width and height values of the viewBox attribute respectively.
2. Let *e-x*, *e-y*, *e-width*, *e-height* be the position and size of the element respectively.
3. Let *align* be the align value of preserveAspectRatio, or 'xMidYMid' if preserveAspectRatio is not defined.
4. Let *meetOrSlice* be the meetOrSlice value of preserveAspectRatio, or 'meet' if preserveAspectRatio is not defined or if meetOrSlice is missing from this value.
5. Initialize *scale-x* to *e-width*/*vb-width*.
6. Initialize *scale-y* to *e-height*/*vb-height*.
7. If *align* is not 'none' and *meetOrSlice* is 'meet', set the larger of *scale-x* and *scale-y* to the smaller.
8. Otherwise, if *align* is not 'none' and *meetOrSlice* is 'slice', set the smaller of *scale-x* and *scale-y* to the larger.
9. Initialize *translate-x* to *e-x* - (*vb-x* * scale-x).
10. Initialize *translate-y* to *e-y* - (*vb-y* * scale-y)
11. If *align* contains 'xMid', add (*e-width* - *vb-width* * *scale-x*) / 2 to *translate-x*.
12. If *align* contains 'xMax', add (*e-width* - *vb-width* * *scale-x*) to *translate-x*.
13. If *align* contains 'yMid', add (*e-height* - *vb-height* * *scale-y*) / 2 to *translate-y*.
14. If *align* contains 'yMax', add (*e-height* - *vb-height* * *scale-y*) to *translate-y.*

The transform applied to content contained by the element is given by translate(*translate-x*, *translate-y*) scale(*scale-x*, *scale-y*).

## <a id="ViewportSpace"></a>8.3. The initial viewport

The initial viewport's width, must be the value of the [width](geometry.md#Sizing) presentation attribute on the [outermost svg element](struct.md#TermOutermostSVGElement), unless the following conditions are met:

- the SVG content is a separately stored resource that is embedded by reference (such as the ‘object’ element in HTML), or the SVG content is embedded inline within a containing document;
- and the referencing element or containing document is styled using CSS [[CSS2](refs.md#ref-css2)];
- and there are CSS-compatible positioning properties ([[CSS2](refs.md#ref-css2)], section 9.3) specified on the referencing element (e.g., the ‘object’ element) or on the containing document's [outermost svg element](struct.md#TermOutermostSVGElement) that are sufficient to establish the width of the viewport.

Under these conditions, the viewport's width must be established via the positioning properties.

Similarly, if there are positioning properties specified on the referencing element or on the [outermost svg element](struct.md#TermOutermostSVGElement) that are sufficient to establish the height of the viewport, then these positioning properties must establish the viewport's height; otherwise, the initial viewport's height must be the value of the [height](geometry.md#Sizing) presentation attribute on the [outermost svg element](struct.md#TermOutermostSVGElement).

If the [width](geometry.md#Sizing) or [height](geometry.md#Sizing) presentation attributes on the [outermost svg element](struct.md#TermOutermostSVGElement) are in [user units](#TermUserUnits) (i.e., no unit identifier has been provided), then the value is assumed to be equivalent to the same number of "px" units (see [Units](#Units)).

In the following example, an SVG graphic is embedded inline within a parent XML document which is formatted using CSS layout rules. Since CSS positioning properties are not provided on the [outermost svg element](struct.md#TermOutermostSVGElement), the width="100px" and height="200px" attributes determine the size of the initial viewport:

```
<?xml version="1.0" standalone="yes"?>
<parent xmlns="http://some.url">

   <!-- SVG graphic -->
   <svg xmlns='http://www.w3.org/2000/svg'
      width="100px" height="200px">
      <path d="M100,100 Q200,400,300,100"/>
      <!-- rest of SVG graphic would go here -->
   </svg>

</parent>
```

## <a id="InitialCoordinateSystem"></a>8.4. The initial coordinate system

For the [outermost svg element](struct.md#TermOutermostSVGElement), the SVG user agent must determine an initial [viewport coordinate system](#TermViewportCoordinateSystem) and an initial [user coordinate system](../css-transforms-1.md#user-coordinate-system) such that the two coordinates systems are identical. The origin of both coordinate systems must be at the origin of the SVG viewport, and one unit in the initial coordinate system must equal one CSS 2.1 px ([[CSS2](refs.md#ref-css2)], section 4.3.2) in the SVG viewport. In stand-alone SVG documents and in SVG document fragments embedded (by reference or inline) within parent documents where the parent's layout is determined by CSS [[CSS2](refs.md#ref-css2)], the initial viewport coordinate system (and therefore the initial user coordinate system) must have its origin at the top/left of the viewport, with the positive x-axis pointing towards the right, the positive y-axis pointing down, and text rendered with an "upright" orientation, which means glyphs are oriented such that Roman characters and full-size ideographic characters for Asian scripts have the top edge of the corresponding glyphs oriented upwards and the right edge of the corresponding glyphs oriented to the right.

If the SVG implementation is part of a user agent which supports styling documents using CSS 2.1 compatible *px* units, then the SVG user agent should set its initial value for the size of a *px* unit in real world units to match the value used for other styling operations; otherwise, if the user agent can determine the size of a *px* unit from its environment, it should use that value; otherwise, it should choose an appropriate size for one *px* unit. In all cases, the size of a *px* must be in conformance with the rules described in CSS 2.1 ([[CSS2](refs.md#ref-css2)], section 4.3.2).

<a id="ExampleInitialCoords"></a>Example InitialCoords below shows that the initial coordinate system has the origin at the top/left with the x-axis pointing to the right and the y-axis pointing down. The initial user coordinate system has one user unit equal to the parent (implicit or explicit) user agent's "pixel".

```
<?xml version="1.0" standalone="no"?>
<svg width="300px" height="100px" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Example InitialCoords - SVG's initial coordinate system</desc>

  <g fill="none" stroke="black" stroke-width="3" >
    <line x1="0" y1="1.5" x2="300" y2="1.5" />
    <line x1="1.5" y1="0" x2="1.5" y2="100" />
  </g>
  <g fill="red" stroke="none" >
    <rect x="0" y="0" width="3" height="3" />
    <rect x="297" y="0" width="3" height="3" />
    <rect x="0" y="97" width="3" height="3" />
  </g>
  <g font-size="14" font-family="Verdana" >
    <text x="10" y="20">(0,0)</text>
    <text x="240" y="20">(300,0)</text>
    <text x="10" y="90">(0,100)</text>
  </g>
</svg>
```

[image: Example InitialCoords — SVG's initial coordinate system]

Example InitialCoords

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/coords/InitialCoords.svg)

## <a id="TransformProperty"></a>8.5. The ‘transform’ property

User agents must support the [transform](#TransformProperty) property and presentation attribute as defined in [[css-transforms-1](refs.md#ref-css-transforms-1)].

## <a id="ViewBoxAttribute"></a>8.6. The ‘viewBox’ attribute

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TermViewBox"></a>viewBox* | [<min-x>,? <min-y>,? <width>,? <height>] | As if not specified. | yes |

**
:

```
<min-x>, <min-y>, <width>, <height> = <number>
```

The ‘[viewBox](#ViewBoxAttribute)’ attribute, in conjunction with the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute, provides the capability to stretch an SVG viewport to fit a particular container element.

The value of the ‘[viewBox](#ViewBoxAttribute)’ attribute is a list of four numbers <min-x>, <min-y>, <width> and <height>, separated by whitespace and/or a comma, that specify a rectangle in user space that should be mapped to the bounds of the SVG viewport established by the given element, taking into account the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute. The presence of the ‘[viewBox](#ViewBoxAttribute)’ attribute results in a transformation being applied to the viewport coordinate system as described in [Computing the equivalent transform of an SVG viewport](#ComputingAViewportsTransform).

A negative value for <width> or <height> is an error and invalidates the ‘[viewBox](#ViewBoxAttribute)’ attribute. A value of zero disables rendering of the element.

<a id="ExampleViewBox"></a>Example ViewBox illustrates the use of the ‘[viewBox](#ViewBoxAttribute)’ attribute on the [outermost svg element](struct.md#TermOutermostSVGElement) to specify that the SVG content should stretch to fit bounds of the SVG viewport.

```
<?xml version="1.0" standalone="no"?>
<svg width="300px" height="200px"
     viewBox="0 0 1500 1000" preserveAspectRatio="none"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Example ViewBox - uses the viewBox
   attribute to automatically create an initial user coordinate
   system which causes the graphic to scale to fit into the
   SVG viewport no matter what size the SVG viewport is.</desc>
  <!-- This rectangle goes from (0,0) to (1500,1000) in user coordinate system.
       Because of the viewBox attribute above,
       the rectangle will end up filling the entire area
       reserved for the SVG content. -->
  <rect x="0" y="0" width="1500" height="1000"
        fill="yellow" stroke="blue" stroke-width="12"  />
  <!-- A large, red triangle -->
  <path fill="red"  d="M 750,100 L 250,900 L 1250,900 z"/>
  <!-- A text string that spans most of the SVG viewport -->
  <text x="100" y="600" font-size="200" font-family="Verdana" >
    Stretch to fit
  </text>
</svg>
```

Example ViewBox

| Rendered into SVG viewport with width=300px, height=200px |  | Rendered into SVG viewport with width=150px, height=200px |
| --- | --- | --- |
| [image: Example ViewBox - stretch to fit 300 by 200] |  | [image: Example ViewBox - stretch to fit 150 by 200] |

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/coords/ViewBox.svg)

The effect of the ‘[viewBox](#ViewBoxAttribute)’ attribute is that the user agent automatically supplies the appropriate transformation matrix to map the specified rectangle in user coordinate system to the bounds of a designated region (often, the SVG viewport). To achieve the effect of the example on the left, with SVG viewport dimensions of 300 by 200 pixels, the user agent needs to automatically insert a transformation which scales both X and Y by 0.2. The effect is equivalent to having an SVG viewport of size 300px by 200px and the following supplemental transformation in the document, as follows:

```
<?xml version="1.0" standalone="no"?>
<svg width="300px" height="200px"
     xmlns="http://www.w3.org/2000/svg">
  <g transform="scale(0.2)">
    <!-- Rest of document goes here -->
  </g>
</svg>
```

To achieve the effect of the example on the right, with SVG viewport dimensions of 150 by 200 pixels, the user agent needs to automatically insert a transformation which scales X by 0.1 and Y by 0.2. The effect is equivalent to having an SVG viewport of size 150px by 200px and the following supplemental transformation in the document, as follows:

```
<?xml version="1.0" standalone="no"?>
<svg width="150px" height="200px"
     xmlns="http://www.w3.org/2000/svg">
  <g transform="scale(0.1 0.2)">
    <!-- Rest of document goes here -->
  </g>
</svg>
```

Note that in some cases the user agent will need to supply a **translate** transformation in addition to a **scale** transformation. For example, on an [outermost svg element](struct.md#TermOutermostSVGElement), a **translate** transformation will be needed if the ‘[viewBox](#ViewBoxAttribute)’ attributes specifies values other than zero for <min-x> or <min-y>.

If both [transform](#TransformProperty) (or ‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’) and ‘[viewBox](#ViewBoxAttribute)’ are applied to an element two new coordinate systems are established. [transform](#TransformProperty) establishes the first new coordinate system for the element. ‘[viewBox](#ViewBoxAttribute)’ establishes a second coordinate system for all descendants of the element. The first coordinate system is post-multiplied by the second coordinate system.

<a id="ViewBoxAttributeEffectOnSiblingAttributes"></a>Unlike the [transform](#TransformProperty) property, the automatic transformation that is created due to a ‘[viewBox](#ViewBoxAttribute)’ does not affect the ‘x’, ‘y’, ‘width’ and ‘height’ attributes (or in the case of the ‘[marker](painting.md#MarkerElement)’ element, the ‘[markerWidth](painting.md#MarkerWidthAttribute)’ and ‘[markerHeight](painting.md#MarkerHeightAttribute)’ attributes) on the element with the ‘[viewBox](#ViewBoxAttribute)’ attribute. Thus, in the example above which shows an ‘[svg](struct.md#SVGElement)’ element which has [width](geometry.md#Sizing) and [height](geometry.md#Sizing) presentation attributes and a ‘[viewBox](#ViewBoxAttribute)’ attribute, the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) represent values in the coordinate system that exists *before* the ‘[viewBox](#ViewBoxAttribute)’ transformation is applied. On the other hand, like the [transform](#TransformProperty) property, it does establish a new coordinate system for all other attributes and for descendant elements.

## <a id="PreserveAspectRatioAttribute"></a>8.7. The ‘preserveAspectRatio’ attribute

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="TermPreserveAspectRatio"></a>preserveAspectRatio* | <align> <meetOrSlice>? | xMidYMid meet | yes |

**
:

```
<align> =
    none
    | xMinYMin | xMidYMin | xMaxYMin
    | xMinYMid | xMidYMid | xMaxYMid
    | xMinYMax | xMidYMax | xMaxYMax
```

```
<meetOrSlice> = meet | slice
```

Indicates whether or not to force uniform scaling. Applies to all elements that establish a new SVG viewport (see [elements that establish SVG viewports](#ElementsThatEstablishViewports)), plus the ‘[image](embedded.md#ImageElement)’, ‘[marker](painting.md#MarkerElement)’, ‘[pattern](pservers.md#PatternElement)’ and ‘[view](linking.md#ViewElement)’ elements

In some cases, typically when using the ‘[viewBox](#ViewBoxAttribute)’ attribute, it is desirable that the graphics stretch to fit non-uniformly to take up the entire SVG viewport. In other cases, it is desirable that uniform scaling be used for the purposes of preserving the aspect ratio of the graphics.

For elements that establish a new SVG viewport (see [elements that establish SVG viewports](#ElementsThatEstablishViewports)), plus the ‘[marker](painting.md#MarkerElement)’, ‘[pattern](pservers.md#PatternElement)’ and ‘[view](linking.md#ViewElement)’ elements, ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ only applies when a value has been provided for ‘[viewBox](#ViewBoxAttribute)’ on the same element. For these elements, if attribute ‘[viewBox](#ViewBoxAttribute)’ is not provided, then ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ is ignored.

For ‘[image](embedded.md#ImageElement)’ elements, ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ indicates how referenced images should be fitted with respect to the reference rectangle and whether the aspect ratio of the referenced image should be preserved with respect to the current user coordinate system.

<a id="DataTypeAlign"></a>The <align> parameter indicates whether to force uniform scaling and, if so, the alignment method to use in case the aspect ratio of the ‘[viewBox](#ViewBoxAttribute)’ doesn't match the aspect ratio of the SVG viewport. The <align> parameter must be one of the following strings:

- none - Do not force uniform scaling. Scale the graphic content of the given element non-uniformly if necessary such that the element's bounding box exactly matches the SVG viewport rectangle. (Note: if <align> is none, then the optional <meetOrSlice> value is ignored.)
- xMinYMin - Force uniform scaling. Align the <min-x> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the smallest X value of the SVG viewport. Align the <min-y> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the smallest Y value of the SVG viewport.
- xMidYMin - Force uniform scaling. Align the midpoint X value of the element's ‘[viewBox](#ViewBoxAttribute)’ with the midpoint X value of the SVG viewport. Align the <min-y> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the smallest Y value of the SVG viewport.
- xMaxYMin - Force uniform scaling. Align the <min-x>+<width> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the maximum X value of the SVG viewport. Align the <min-y> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the smallest Y value of the SVG viewport.
- xMinYMid - Force uniform scaling. Align the <min-x> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the smallest X value of the SVG viewport. Align the midpoint Y value of the element's ‘[viewBox](#ViewBoxAttribute)’ with the midpoint Y value of the SVG viewport.
- xMidYMid (the [initial value](types.md#TermInitialValue)) - Force uniform scaling. Align the midpoint X value of the element's ‘[viewBox](#ViewBoxAttribute)’ with the midpoint X value of the SVG viewport. Align the midpoint Y value of the element's ‘[viewBox](#ViewBoxAttribute)’ with the midpoint Y value of the SVG viewport.
- xMaxYMid - Force uniform scaling. Align the <min-x>+<width> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the maximum X value of the SVG viewport. Align the midpoint Y value of the element's ‘[viewBox](#ViewBoxAttribute)’ with the midpoint Y value of the SVG viewport.
- xMinYMax - Force uniform scaling. Align the <min-x> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the smallest X value of the SVG viewport. Align the <min-y>+<height> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the maximum Y value of the SVG viewport.
- xMidYMax - Force uniform scaling. Align the midpoint X value of the element's ‘[viewBox](#ViewBoxAttribute)’ with the midpoint X value of the SVG viewport. Align the <min-y>+<height> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the maximum Y value of the SVG viewport.
- xMaxYMax - Force uniform scaling. Align the <min-x>+<width> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the maximum X value of the SVG viewport. Align the <min-y>+<height> of the element's ‘[viewBox](#ViewBoxAttribute)’ with the maximum Y value of the SVG viewport.

The <meetOrSlice> parameter is optional and, if provided, is separated from the <align> value by one or more spaces and then must be one of the following strings:

-
meet (the default) - Scale the graphic such that:
  - aspect ratio is preserved
  - the entire ‘[viewBox](#ViewBoxAttribute)’ is visible within the SVG viewport
  - the ‘[viewBox](#ViewBoxAttribute)’ is scaled up as much as possible, while still meeting the other criteria
In this case, if the aspect ratio of the graphic does not match the SVG viewport, some of the SVG viewport will extend beyond the bounds of the ‘[viewBox](#ViewBoxAttribute)’ (i.e., the area into which the ‘[viewBox](#ViewBoxAttribute)’ will draw will be smaller than the SVG viewport).
-
slice - Scale the graphic such that:
  - aspect ratio is preserved
  - the entire SVG viewport is covered by the ‘[viewBox](#ViewBoxAttribute)’
  - the ‘[viewBox](#ViewBoxAttribute)’ is scaled down as much as possible, while still meeting the other criteria
In this case, if the aspect ratio of the ‘[viewBox](#ViewBoxAttribute)’ does not match the SVG viewport, some of the ‘[viewBox](#ViewBoxAttribute)’ will extend beyond the bounds of the SVG viewport (i.e., the area into which the ‘[viewBox](#ViewBoxAttribute)’ will draw is larger than the SVG viewport).

<a id="ExamplePreserveAspectRatio"></a>Example PreserveAspectRatio illustrates the various options on ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’. The example creates several new SVG viewports by including ‘[svg](struct.md#SVGElement)’ sub-elements embedded inside the [outermost svg element](struct.md#TermOutermostSVGElement) (see [Establishing a new SVG viewport](#EstablishingANewSVGViewport)).

```
﻿<svg width="450px" height="300px" xmlns="http://www.w3.org/2000/svg">

  <desc>Example PreserveAspectRatio - illustrates preserveAspectRatio attribute</desc>

  <style type="text/css">
    text { font-size: 9; }
    rect { fill: none; stroke: blue; }
  </style>

  <defs>
    <g id="smile">
      <rect x='.5' y='.5' width='29' height='39' style="fill:black;stroke:red"/>
      <circle cx='15' cy='20' r='10'  fill='yellow'/>
      <circle cx='12' cy='17' r='1.5' fill='black'/>
      <circle cx='17' cy='17' r='1.5' fill='black'/>
      <path d='M 10 24 A 8 8 0 0 0 20 24' stroke='black' stroke-width='2'/>
    </g>
  </defs>

  <rect x="1" y="1" width="448" height="298"/>

  <text x="10" y="30">SVG to fit</text>
  <g transform="translate(20,40)"><use href="#smile" /></g>

  <text x="10" y="110">Viewport 1</text>
  <g transform="translate(10,120)"><rect x='.5' y='.5' width='49' height='29'/></g>

  <text x="10" y="180">Viewport 2</text>
  <g transform="translate(20,190)"><rect x='.5' y='.5' width='29' height='59'/></g>

  <g id="meet-group-1" transform="translate(100, 60)">
    <text x="0" y="-30">--------------- meet ---------------</text>
    <g>
      <text y="-10">xMin*</text>
      <rect x='.5' y='.5' width='49' height='29'/>
      <svg preserveAspectRatio="xMinYMin meet" viewBox="0 0 30 40" width="50" height="30">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(70,0)">
      <text y="-10">xMid*</text>
      <rect x='.5' y='.5' width='49' height='29'/>
      <svg preserveAspectRatio="xMidYMid meet" viewBox="0 0 30 40" width="50" height="30">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(0,70)">
      <text y="-10">xMax*</text>
      <rect x='.5' y='.5' width='49' height='29'/>
      <svg preserveAspectRatio="xMaxYMax meet" viewBox="0 0 30 40" width="50" height="30">
        <use href="#smile" />
      </svg>
    </g>
  </g>

  <g id="meet-group-2" transform="translate(250, 60)">
    <text x="0" y="-30">---------- meet ----------</text>
    <g>
      <text y="-10">*YMin</text>
      <rect  x='.5' y='.5' width='29' height='59'/>
      <svg preserveAspectRatio="xMinYMin meet" viewBox="0 0 30 40" width="30" height="60">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(50, 0)">
      <text y="-10">*YMid</text>
      <rect x='.5' y='.5' width='29' height='59'/>
      <svg preserveAspectRatio="xMidYMid meet" viewBox="0 0 30 40" width="30" height="60">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(100, 0)">
      <text y="-10">*YMax</text>
      <rect x='.5' y='.5' width='29' height='59'/>
      <svg preserveAspectRatio="xMaxYMax meet" viewBox="0 0 30 40" width="30" height="60">
	<use href="#smile" />
      </svg>
    </g>
  </g>

  <g id="slice-group-1" transform="translate(100, 220)">
    <text x="0" y="-30">---------- slice ----------</text>
    <g>
      <text y="-10">xMin*</text>
      <rect x='.5' y='.5' width='29' height='59'/>
      <svg preserveAspectRatio="xMinYMin slice" viewBox="0 0 30 40" width="30" height="60">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(50,0)">
      <text y="-10">xMid*</text>
      <rect x='.5' y='.5' width='29' height='59'/>
      <svg preserveAspectRatio="xMidYMid slice" viewBox="0 0 30 40" width="30" height="60">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(100,0)">
      <text y="-10">xMax*</text>
      <rect x='.5' y='.5' width='29' height='59'/>
      <svg preserveAspectRatio="xMaxYMax slice" viewBox="0 0 30 40" width="30" height="60">
	<use href="#smile" />
      </svg>
    </g>
  </g>

  <g id="slice-group-2" transform="translate(250, 220)">
    <text x="0" y="-30">--------------- slice ---------------</text>
    <g>
      <text y="-10">*YMin</text>
      <rect x='.5' y='.5' width='49' height='29'/>
      <svg preserveAspectRatio="xMinYMin slice" viewBox="0 0 30 40"
           width="50" height="30">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(70,0)">
      <text y="-10">*YMid</text>
      <rect x='.5' y='.5' width='49' height='29'/>
      <svg preserveAspectRatio="xMidYMid slice" viewBox="0 0 30 40" width="50" height="30">
	<use href="#smile" />
      </svg>
    </g>
    <g transform="translate(140,0)">
      <text y="-10">*YMax</text>
      <rect x='.5' y='.5' width='49' height='29'/>
      <svg preserveAspectRatio="xMaxYMax slice" viewBox="0 0 30 40" width="50" height="30">
	<use href="#smile" />
      </svg>
    </g>
  </g>
</svg>
```

[image: Example PreserveAspectRatio — demonstrate available options]

Example PreserveAspectRatio

## <a id="EstablishingANewSVGViewport"></a>8.8. Establishing a new SVG viewport

Including an ‘[svg](struct.md#SVGElement)’ element inside SVG content creates a new SVG viewport into which all contained graphics are drawn; this implicitly establishes both a new viewport coordinate system and a new user coordinate system. Additionally, there is a new meaning for percentage units therein, because a new SVG viewport has been established (see [Units](#Units)).

The bounds of the new SVG viewport are defined by the ‘x’, ‘y’, ‘width’ and ‘height’ attributes on the element establishing the new SVG viewport, such as an ‘[svg](struct.md#SVGElement)’ element. Both the new viewport coordinate system and the new user coordinate system have their origins at (‘x’, ‘y’), where ‘x’ and ‘y’ represent the value of the corresponding attributes on the element establishing the SVG viewport. The orientation of the new viewport coordinate system and the new user coordinate system correspond to the orientation of the current user coordinate system for the element establishing the SVG viewport. A single unit in the new viewport coordinate system and the new user coordinate system are the same size as a single unit in the current user coordinate system for the element establishing the SVG viewport.

Here is an example:

```
<?xml version="1.0" standalone="no"?>
<svg width="4in" height="3in"
     xmlns="http://www.w3.org/2000/svg">
  <desc>This SVG drawing embeds another one,
    thus establishing a new SVG viewport
  </desc>
  <!-- The following statement establishing a new SVG viewport
       and renders SVG drawing B into that SVG viewport -->
  <svg x="25%" y="25%" width="50%" height="50%">
     <!-- drawing B goes here -->
  </svg>
</svg>
```

For an extensive example of creating new SVG viewports, see [Example PreserveAspectRatio](#ExamplePreserveAspectRatio).

<a id="ElementsThatEstablishViewports"></a>The following elements establish new SVG viewports:

- The ‘[svg](struct.md#SVGElement)’ element
- A ‘[symbol](struct.md#SymbolElement)’ element that is instanced by a ‘[use](struct.md#UseElement)’ element.

For historical reasons, the ‘[pattern](pservers.md#PatternElement)’ and ‘[marker](painting.md#MarkerElement)’ elements do not create a new viewport, despite accepting a ‘[viewBox](#ViewBoxAttribute)’ attribute. Neither do the ‘[clipPath](../css-masking-1.md#ClipPathElement)’ or ‘[mask](../css-masking-1.md#MaskElement)’ elements. Percentage lengths within the content of these elements are not proportional to the dimensions of the graphical effect region.

The ‘[foreignObject](embedded.md#ForeignObjectElement)’ element establishes a new CSS containing block for its child content. This has some effects similar to a new viewport, resetting the scope of layout for child content. However, in order to render SVG elements that are descendents of ‘[foreignObject](embedded.md#ForeignObjectElement)’, a new ‘[svg](struct.md#SVGElement)’ element must establish an SVG document fragment and SVG viewport.

An ‘[image](embedded.md#ImageElement)’ creates a new document viewport for the referenced document. If the referenced document is a SVG file, it will of course establish its own SVG viewport.

Whether a new SVG viewport also establishes a new additional clipping path is determined by the value of the [overflow](render.md#OverflowAndClipProperties) property on the element that establishes the new SVG viewport.

## <a id="Units"></a>8.9. Units

SVG follows the description and definition of common values and units from the CSS Values and Units Module [[css-values](refs.md#ref-css-values-3)] for attributes, presentation attributes and CSS properties. Each attribute and property must specify the used component value type. Subsequent or extending specifications published by the CSS WG or SVG WG may extend basic data types or add new data types.

For <percentage> values that are defined to be relative to the size of SVG viewport:

- For any x-coordinate value or width value expressed as a percentage of the [SVG viewport](#TermSVGViewport), the value to use must be the percentage, in user units, of the width parameter of the ‘[viewBox](#ViewBoxAttribute)’ applied to that viewport. If no ‘[viewBox](#ViewBoxAttribute)’ is specified, then the value to use must be the percentage, in user units, of the width of the [SVG viewport](#TermSVGViewport).
- For any y-coordinate value or height value expressed as a percentage of the [SVG viewport](#TermSVGViewport), the value to use must be the percentage, in user units, of the height parameter of the ‘[viewBox](#ViewBoxAttribute)’ applied to that viewport. If no ‘[viewBox](#ViewBoxAttribute)’ is specified, then the value to use must be the percentage, in user units, of the height of the [SVG viewport](#TermSVGViewport).
- <a id="Units_viewport_percentage"></a>For any other length value expressed as a percentage of the [SVG viewport](#TermSVGViewport), the percentage must be calculated as a percentage of the normalized diagonal of the ‘[viewBox](#ViewBoxAttribute)’ applied to that viewport. If no ‘[viewBox](#ViewBoxAttribute)’ is specified, then the normalized diagonal of the [SVG viewport](#TermSVGViewport) must be used. The normalized diagonal length must be calculated with `sqrt((*width*)**2 + (*height*)**2)/sqrt(2)`.

<a id="ExampleUnits"></a>Example Units below illustrates some of the processing rules for different types of units.

```
<?xml version="1.0" standalone="no"?>
<svg width="400px" height="200px" viewBox="0 0 4000 2000"
     xmlns="http://www.w3.org/2000/svg">
  <title>Example Units</title>
  <desc>Illustrates various units options</desc>

  <!-- Frame the picture -->
  <rect x="5" y="5" width="3990" height="1990" 
        fill="none" stroke="blue" stroke-width="10"/>

  <g fill="blue" stroke="red" font-family="Verdana" font-size="150">
    <!-- Absolute unit specifiers -->
    <g transform="translate(400,0)">
      <text x="-50" y="300" fill="black" stroke="none">Abs. units:</text>
      <rect x="0" y="400" width="4in" height="2in" stroke-width=".4in"/>
      <rect x="0" y="750" width="384" height="192" stroke-width="38.4"/>
      <g transform="scale(2)">
        <rect x="0" y="600" width="4in" height="2in" stroke-width=".4in"/>
      </g>
    </g>

    <!-- Relative unit specifiers -->
    <g transform="translate(1600,0)">
      <text x="-50" y="300" fill="black" stroke="none">Rel. units:</text>
      <rect x="0" y="400" width="2.5em" height="1.25em" stroke-width=".25em"/>
      <rect x="0" y="750" width="375" height="187.5" stroke-width="37.5"/>
      <g transform="scale(2)">
        <rect x="0" y="600" width="2.5em" height="1.25em" stroke-width=".25em"/>
      </g>
    </g>

    <!-- Percentages -->
    <g transform="translate(2800,0)">
      <text x="-50" y="300" fill="black" stroke="none">Percentages:</text>
      <rect x="0" y="400" width="10%" height="10%" stroke-width="1%"/>
      <rect x="0" y="750" width="400" height="200" stroke-width="31.62"/>
      <g transform="scale(2)">
        <rect x="0" y="600" width="10%" height="10%" stroke-width="1%"/>
      </g>
    </g>
  </g>
</svg>
```

[image: Example Units — demonstrate available options]

Example Units

The three rectangles on the left demonstrate the use of one of the absolute unit identifiers, the "in" unit (inch). CSS defines 1 inch to be equal to 96 pixels. Therefore, the topmost rectangle, which is specified in inches, is exactly the same size as the middle rectangle, which is specified in user units such that there are 96 user units for each corresponding inch in the topmost rectangle. The bottom rectangle of the group illustrates what happens when values specified in inches are scaled.

The three rectangles in the middle demonstrate the use of one of the relative unit identifiers, the "em" unit. Because the font-size property has been set to 150 on the outermost ‘[g](struct.md#GElement)’ element, each "em" unit is equal to 150 user units. The topmost rectangle, which is specified in "em" units, is exactly the same size as the middle rectangle, which is specified in user units such that there are 150 user units for each corresponding "em" unit in the topmost rectangle. The bottom rectangle of the group illustrates what happens when values specified in "em" units are scaled.

The three rectangles on the right demonstrate the use of percentages. Note that the width and height of the SVG viewport in the user coordinate system for the SVG viewport element (in this case, the [outermost svg element](struct.md#TermOutermostSVGElement)) are 4000 and 2000, respectively, because processing the ‘[viewBox](#ViewBoxAttribute)’ attribute results in a transformed user coordinate system. The topmost rectangle, which is specified in percentage units, is exactly the same size as the middle rectangle, which is specified in equivalent user units. In particular, note that the [stroke-width](painting.md#StrokeWidthProperty) property in the middle rectangle is set to 1% of the `sqrt((*actual-width*)**2 + (*actual-height*)**2) / sqrt(2)`, which in this case is .01*sqrt(4000*4000+2000*2000)/sqrt(2), or 31.62. The bottom rectangle of the group illustrates what happens when values specified in percentage units are scaled.

## <a id="BoundingBoxes"></a>8.10. Bounding boxes

**<a id="TermBoundingBox"></a>*<a id="bounding-box"></a>bounding box***
:

The bounding box (or "bbox") of an element is the tightest fitting rectangle aligned with the axes of that element's user coordinate system that entirely encloses it and its descendants.

Three kinds of bounding boxes can be computed for an element:

1. The *<a id="TermObjectBoundingBox"></a>object bounding box* is the bounding box that contains only an element's geometric shape. For [basic shapes](shapes.md#TermBasicShapeElement), this is the area that is filled. Unless otherwise specified, this is what is meant by the unqualified term "bounding box".
2. The *<a id="TermStrokeBoundingBox"></a>stroke bounding box* is the bounding box that contains an element's geometric shape and its [stroke shape](painting.md#TermStrokeShape).
3. The *<a id="TermDecoratedBoundingBox"></a>decorated bounding box* is the bounding box that contains an element's geometric shape, its [stroke shape](painting.md#TermStrokeShape) and its [markers](painting.md#Markers).

Note that the values of the [opacity](render.md#ObjectAndGroupOpacityProperties), [visibility](render.md#VisibilityControl), [fill](painting.md#FillProperty), [fill-opacity](painting.md#FillOpacityProperty), [fill-rule](painting.md#FillRuleProperty), [stroke-dasharray](painting.md#StrokeDasharrayProperty) and [stroke-dashoffset](painting.md#StrokeDashoffsetProperty) properties on an element have no effect on the bounding box of an element.

For curved shapes, the bounding box must enclose all portions of the shape along the edge, not just end points. Note that control points for a curve which are not defined as lying along the line of the resulting curve (e.g., the second coordinate pair of a Cubic Bézier command) must not contribute to the dimensions of the bounding box (though those points may fall within the area of the bounding box, if they lie within the shape itself, or along or close to the curve). For example, control points of a curve that are at a further distance than the curve edge, from the non-enclosing side of the curve edge, must be excluded from the bounding box.

[image: Image showing the object bounding box of a quadratic Bézier curve.]

The path 'M20,50 L35,100 H120 V50 Q70,10 20,50' is shown in light blue. On the left, a correct object bounding box of the path is shown. Note that it does not include the top-most control point of the curve, but it does include all of the blue shape, even the parts that lie outside of the convex hull of the control points.

Even if an element is not in the [rendering tree](render.md#RenderingTree) – due to it being 'display: none', within a ‘[defs](struct.md#DefsElement)’ element, not usually rendered like a ‘[symbol](struct.md#SymbolElement)’ element or not currently present in the document tree – it still has a bounding box. A call to [getBBox](types.md#__svg__SVGGraphicsElement__getBBox) on the element will return the same rectangle as if the element were rendered. However, an element that is not in the [rendering tree](render.md#RenderingTree) does not contribute to the bounding box of any ancestor element.

The following example defines a number of elements. The expected [object bounding box](#TermObjectBoundingBox) for each element with an ID is shown below.

```
<svg xmlns="http://www.w3.org/2000/svg">

  <title>Bounding Box Calculation</title>
  <desc>Examples of elements with different bounding box results based on context.</desc>

  <defs id="defs-1">
     <rect id="rect-1" x="20" y="20" width="40" height="40" fill="blue" />
  </defs>

  <g id="group-1">
    <use id="use-1" href="#rect-1" x="10" y="10" />

    <g id="group-2" display="none">
      <rect id="rect-2" x="10" y="10" width="100" height="100" fill="red" />
    </g>
  </g>
</svg>
```

| Element ID | Bounding Box Result |
| --- | --- |
| "`defs-1`" | {0, 0, 0, 0} |
| "`rect-1`" | {20, 20, 40, 40} |
| "`group-1`" | {30, 30, 40, 40} |
| "`use-1`" | {30, 30, 40, 40} |
| "`group-2`" | {10, 10, 100, 100} |
| "`rect-2`" | {10, 10, 100, 100} |

For [text content elements](text.md#TermTextContentElement), for the purposes of the bounding box calculation, each glyph must be treated as a separate graphics element. The calculations must assume that all glyphs occupy the [full glyph cell](#TermFullGlyphCell). The *<a id="TermFullGlyphCell"></a>full glyph cell* must have width equal to the horizontal advance and height equal to the EM box for horizontal text. For vertical text that is typeset sideways, the [full glyph cell](#TermFullGlyphCell) must have width equal to the EM box and height equal to the horizontal advance. For other vertical text, the [full glyph cell](#TermFullGlyphCell) must have width equal to the EM box and height equal to the vertical advance, or height equal to the height of the EM box if no vertical advance is defined in the font. For example, for horizontal text, the calculations must assume that each glyph extends vertically to the full ascent and descent values for the font.

Because declarative or scripted animation can change the shape, size, and position of an element, the bounding box is mutable. Thus, the bounding box for an element shall reflect the current values for the element at the snapshot in time at which the bounding box is requested, whether through a script call or as part of a declarative or linking syntax.

An element which has zero width, zero height, or both (such as a vertical or horizontal line, or a ‘[rect](shapes.md#RectElement)’ element with a zero [width](geometry.md#Sizing) or [height](geometry.md#Sizing)) still has a bounding box, with a positive value for the positive dimension, or with '0' for both the width and height if no positive dimension is specified. Similarly, subpaths segments of a ‘[path](paths.md#PathElement)’ element with zero width and height must be included in that element's geometry for the sake of the bounding box.

An element with no position specified (such as a ‘[path](paths.md#PathElement)’ element with a value of none for the [d](paths.md#DProperty) property) is positioned at the point (0,0) for the purposes of calculating a bounding box.

Note that elements whose DOM object does not derive from [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement) (such as gradient elements) do not have a bounding box, and thus have no interface to request a bounding box.

Elements in the [rendering tree](render.md#RenderingTree) which reference unresolved resources shall still have a bounding box, defined by the position and dimensions specified in their attributes, or by the [initial value](types.md#TermInitialValue) for those attributes if no values are supplied. For example, the element `<use href="#bad" x="10" y="10"/>` would have a bounding box with an x and y of 10 and a width and height of 0.

The following algorithm defines how to compute a bounding box for a given element. The inputs to the algorithm are:

- *element*, the element we are computing a bounding box for;
- *space*, a coordinate space in which the bounding box will be computed;
- *fill*, a boolean indicating whether the bounding box includes the geometry of the element and its descendants;
- *stroke*, a boolean indicating whether the bounding box includes the stroke of the element and its descendants;
- *markers*, a boolean indicating whether the bounding box includes the markers of the element and its descendants; and
- *clipped*, a boolean indicating whether the bounding box is affected by any clipping paths applied to the element and its descendants.

The algorithm to compute the bounding box is as follows, depending on the type of *element*:

**a [shape](shapes.md#TermShapeElement)**
**a [text content element](text.md#TermTextContentElement)**
**an ‘[a](linking.md#AElement)’ element within a [text content element](text.md#TermTextContentElement)**
:

1. Let *box* be a rectangle initialized to (0, 0, 0, 0).
2. Let *fill-shape* be the [equivalent path](paths.md#TermEquivalentPath) of *element* if it is a [shape](shapes.md#TermShapeElement), or a shape that includes each of the glyph cells corresponding to the text within the elements otherwise.
3. If *fill* is true, then set *box* to the tightest rectangle in the coordinate system *space* that contains *fill-shape*.
The values of the [fill](painting.md#FillProperty), [fill-opacity](painting.md#FillOpacityProperty) and [fill-rule](painting.md#FillRuleProperty) properties do not affect *fill-shape*.
4. If *stroke* is true and the element's [stroke](painting.md#StrokeProperty) is anything other than none, then set *box* to be the union of *box* and the tightest rectangle in coordinate system *space* that contains the [stroke shape](painting.md#StrokeShape) of the element, with the assumption that the element has no dash pattern.
The values of the [stroke-opacity](painting.md#StrokeOpacityProperty), [stroke-dasharray](painting.md#StrokeDasharrayProperty) and [stroke-dashoffset](painting.md#StrokeDashoffsetProperty) do not affect the calculation of the stroke shape.
5. If *markers* is true, then for each marker *marker* rendered on the element:
  1. For each descendant [graphics element](struct.md#TermGraphicsElement) *child* of the ‘[marker](painting.md#MarkerElement)’ element that defines *marker*'s content:
    1. If *child* has an ancestor element within the ‘[marker](painting.md#MarkerElement)’ that is 'display: none', has a failing [conditional processing attribute](struct.md#TermConditionalProcessingAttribute), or is not an ‘[a](linking.md#AElement)’, ‘[g](struct.md#GElement)’, ‘[svg](struct.md#SVGElement)’ or ‘[switch](struct.md#SwitchElement)’ element, then continue to the next descendant [graphics element](struct.md#TermGraphicsElement).
    2. Otherwise, set *box* to be the union of *box* and the result of invoking the algorithm to compute a bounding box with *child* as the element, *space* as the target coordinate space, true for *fill*, *stroke* and *markers*, and *clipped* for *clipped*.
6. If *clipped* is true and the value of [clip-path](../css-masking-1.md#the-clip-path) on *element* is not none, then set *box* to be the tightest rectangle in coordinate system *space* that contains the intersection of *box* and the clipping path.
7. Return *box*.

**<a id="ContainerElementBoudingBoxComputation"></a>a [container element](struct.md#TermContainerElement)**
**‘[use](struct.md#UseElement)’**
:

1. Let *box* be a rectangle initialized to (0, 0, 0, 0).
2. Let *parent* be the [container element](struct.md#TermContainerElement) if it is one, or the root of the ‘[use](struct.md#UseElement)’ element's shadow tree otherwise.
3. For each descendant [graphics element](struct.md#TermGraphicsElement) *child* of *parent*:
  1. If *child* is [not rendered](render.md#Rendered-vs-NonRendered) then continue to the next descendant [graphics element](struct.md#TermGraphicsElement).
  2. Otherwise, set *box* to be the union of *box* and the result of invoking the algorithm to compute a bounding box with *child* as the element and the same values for *space*, *fill*, *stroke*, *markers* and *clipped* as the corresponding algorithm input values.
4. If *clipped* is true:
  - If the value of [clip-path](../css-masking-1.md#the-clip-path) on *element* is not none, then set *box* to be the tightest rectangle in coordinate system *space* that contains the intersection of *box* and the clipping path.
  - If the [overflow](render.md#OverflowAndClipProperties) property applies to the *element* and does not have a value of visible, then set *box* to be the tightest rectangle in coordinate system *space* that contains the intersection of *box* and the element's overflow bounds.
  - If the [clip](../css-masking-1.md#propdef-clip) property applies to the *element* and does not have a value of auto, then set *box* to be the tightest rectangle in coordinate system *space* that contains the intersection of *box* and the rectangle specified by [clip](../css-masking-1.md#propdef-clip).
5. Return *box*.

**‘[foreignObject](embedded.md#ForeignObjectElement)’**
**‘[image](embedded.md#ImageElement)’**
:

1. Let *box* be the tightest rectangle in coordinate space *space* that contains the [positioning rectangle](embedded.md#TermPositioningRectangle) defined by the ‘x’, ‘y’, ‘width’ and ‘height’ geometric properties of the element.
The *fill*, *stroke* and *markers* input arguments to this algorithm do not affect the bounding box returned for these elements.
2. If *clipped* is true and the value of [clip-path](../css-masking-1.md#the-clip-path) on *element* is not none, then set *box* to be the tightest rectangle in coordinate system *space* that contains the intersection of *box* and the clipping path.
3. Return *box*.

The union *box* with a value of (0, 0, 0, 0) and an empty shape is *box*.

The [object bounding box](#TermObjectBoundingBox), [stroke bounding box](#TermStrokeBoundingBox) or [decorated bounding box](#TermDecoratedBoundingBox) of an element is the result of invoking the bounding box computation algorithm above with the following arguments: *element* is the element itself; *space* is the element's user coordinate system; *fill* is true; *stroke* is true if we are computing the [stroke bounding box](#TermStrokeBoundingBox) or [decorated bounding box](#TermDecoratedBoundingBox), and false otherwise; *markers* is true if we are computing the [decorated bounding box](#TermDecoratedBoundingBox), and false otherwise; and *clipped* is false.

## <a id="ObjectBoundingBoxUnits"></a>8.11. Object bounding box units

<a id="ObjectBoundingBox"></a>The following elements offer the option of expressing coordinate values and lengths as fractions (and, in some cases, percentages) of the [object bounding box](#TermObjectBoundingBox), by setting a specified attribute to 'objectBoundingBox' on the given element:

| Element | Attribute | Effect |
| --- | --- | --- |
| ‘[linearGradient](pservers.md#LinearGradientElement)’ | ‘[gradientUnits](pservers.md#LinearGradientElementGradientUnitsAttribute)’ | Indicates that the attributes which specify the gradient vector (‘[x1](pservers.md#LinearGradientElementX1Attribute)’, ‘[y1](pservers.md#LinearGradientElementY1Attribute)’, ‘[x2](pservers.md#LinearGradientElementX2Attribute)’, ‘[y2](pservers.md#LinearGradientElementY2Attribute)’) represent fractions or percentages of the bounding box of the element to which the gradient is applied. |
| ‘[radialGradient](pservers.md#RadialGradientElement)’ | ‘[gradientUnits](pservers.md#RadialGradientElementGradientUnitsAttribute)’ | Indicates that the attributes which specify the center (‘[cx](pservers.md#RadialGradientElementCXAttribute)’, ‘[cy](pservers.md#RadialGradientElementCYAttribute)’), the radius (‘[r](pservers.md#RadialGradientElementRAttribute)’) and focus (‘[fx](pservers.md#RadialGradientElementFXAttribute)’, ‘[fy](pservers.md#RadialGradientElementFYAttribute)’) represent fractions or percentages of the bounding box of the element to which the gradient is applied. |
| ‘[pattern](pservers.md#PatternElement)’ | ‘[patternUnits](pservers.md#PatternElementPatternUnitsAttribute)’ | Indicates that the attributes which define how to tile the pattern (‘[x](pservers.md#PatternElementXAttribute)’, ‘[y](pservers.md#PatternElementYAttribute)’, ‘[width](pservers.md#PatternElementWidthAttribute)’, ‘[height](pservers.md#PatternElementHeightAttribute)’) are established using the bounding box of the element to which the pattern is applied. |
| ‘[pattern](pservers.md#PatternElement)’ | ‘[patternContentUnits](pservers.md#PatternElementPatternContentUnitsAttribute)’ | Indicates that the user coordinate system for the contents of the pattern is established using the bounding box of the element to which the pattern is applied. |
| ‘[clipPath](../css-masking-1.md#ClipPathElement)’ | ‘[clipPathUnits](../css-masking-1.md#element-attrdef-clippath-clippathunits)’ | Indicates that the user coordinate system for the contents of the ‘[clipPath](../css-masking-1.md#ClipPathElement)’ element is established using the bounding box of the element to which the clipping path is applied. |
| ‘[mask](../css-masking-1.md#MaskElement)’ | ‘[maskUnits](../css-masking-1.md#element-attrdef-mask-maskunits)’ | Indicates that the attributes which define the masking region (‘[x](../css-masking-1.md#element-attrdef-mask-x)’, ‘[y](../css-masking-1.md#element-attrdef-mask-y)’, ‘[width](../css-masking-1.md#element-attrdef-mask-width)’, ‘[height](../css-masking-1.md#element-attrdef-mask-height)’) is established using the bounding box of the element to which the mask is applied. |
| ‘[mask](../css-masking-1.md#MaskElement)’ | ‘[maskContentUnits](../css-masking-1.md#element-attrdef-mask-maskcontentunits)’ | Indicates that the user coordinate system for the contents of the ‘[mask](../css-masking-1.md#MaskElement)’ element are established using the bounding box of the element to which the mask is applied. |
| ‘[filter](../filter-effects-1.md#FilterElement)’ | ‘[filterUnits](../filter-effects-1.md#element-attrdef-filter-filterunits)’ | Indicates that the attributes which define the [filter effects region](../filter-effects-1.md#FilterPrimitiveSubRegion) (‘[x](../filter-effects-1.md#element-attrdef-filter-x)’, ‘[y](../filter-effects-1.md#element-attrdef-filter-y)’, ‘[width](../filter-effects-1.md#element-attrdef-filter-width)’, ‘[height](../filter-effects-1.md#element-attrdef-filter-height)’) represent fractions or percentages of the bounding box of the element to which the filter is applied. |
| ‘[filter](../filter-effects-1.md#FilterElement)’ | ‘[primitiveUnits](../filter-effects-1.md#element-attrdef-filter-primitiveunits)’ | Indicates that the various length values within the filter primitives represent fractions or percentages of the bounding box of the element to which the filter is applied. |

In the discussion that follows, the term *applicable element* is the element to which the given effect applies. For gradients and patterns, the applicable element is the [graphics element](struct.md#TermGraphicsElement) which has its [fill](painting.md#FillProperty) or [stroke](painting.md#StrokeProperty) property referencing the given gradient or pattern. (For special rules concerning [text elements](text.md), see the discussion of [object bounding box units and text elements](text.md#ObjectBoundingBoxUnitsTextObjects).) For clipping paths, masks and filters, the applicable element can be either a [container element](struct.md#TermContainerElement) or a [graphics element](struct.md#TermGraphicsElement).

When keyword objectBoundingBox is used, then the effect is as if a supplemental transformation matrix were inserted into the list of nested transformation matrices to create a new user coordinate system.

First, the (**minx**,**miny**) and (**maxx**,**maxy**) coordinates are determined by the extends of the [object bounding box](#TermObjectBoundingBox) of the applicable element.

Then, coordinate (0,0) in the new user coordinate system is mapped to the (minx,miny) corner of the tight bounding box within the user coordinate system of the applicable element and coordinate (1,1) in the new user coordinate system is mapped to the (maxx,maxy) corner of the tight bounding box of the applicable element. In most situations, the following transformation matrix produces the correct effect:

```
[ (maxx-minx) 0 0 (maxy-miny) minx miny ]
```

When percentages are used with attributes that define the gradient vector, the pattern tile, the filter region or the masking region, a percentage represents the same value as the corresponding decimal value (e.g., 50% means the same as 0.5). If percentages are used within the content of a ‘[pattern](pservers.md#PatternElement)’, ‘[clipPath](../css-masking-1.md#ClipPathElement)’, ‘[mask](../css-masking-1.md#MaskElement)’ or ‘[filter](../filter-effects-1.md#FilterElement)’ element, these values are treated according to the processing rules for percentages as defined in [Units](#Units).

Any numeric value can be specified for values expressed as a fraction or percentage of object bounding box units. In particular, fractions less than zero or greater than one and percentages less than 0% or greater than 100% can be specified.

Keyword objectBoundingBox should not be used when the geometry of the applicable element has no width or no height, such as the case of a horizontal or vertical line, even when the line has actual thickness when viewed due to having a non-zero stroke width since stroke width is ignored for bounding box calculations. When the geometry of the applicable element has no width or height and objectBoundingBox is specified, then the given effect (e.g., a gradient or a filter) will be ignored.

## <a id="SizingSVGInCSS"></a>8.12. Intrinsic sizing properties of SVG content

To enable inclusion of SVG in host documents formatted with CSS, a concrete object size must be calculated. The concrete object size must be calculated using the Default Sizing Algorithm defined in CSS Images 3 [[css-images-3](refs.md#ref-css-images-3)], with the following inputs:

The specified size must be determined from the used values for the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) sizing properties of the ‘[svg](struct.md#SVGElement)’ element.

The intrinsic dimensions must also be determined from the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) sizing properties. If either [width](geometry.md#Sizing) or [height](geometry.md#Sizing) are not specified, the used value is the initial value 'auto'. 'auto' and percentage lengths must not be used to determine an intrinsic width or intrinsic height.

With bitmap image formats, the intrinsic dimensions are fixed in the image file, and the specified size is defined in the host document as needed to scale the image. SVG, being inherently scalable, adapts the intrinsic width and intrinsic height to be the width and height of the specified size. Therefore, when specified as a length, the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) sizing properties of the ‘[svg](struct.md#SVGElement)’ element control the intrinsic dimensions of the SVG image and the specified size that is used when placing the SVG image in a host document.

The intrinsic aspect ratio must be calculated using the following algorithm. If the algorithm returns null, then there is no intrinsic aspect ratio.

1. If the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) sizing properties on the ‘[svg](struct.md#SVGElement)’ element are both absolute values:
  1. return width / height
2. If an [SVG View](linking.md#LinksIntoSVG) is active:
  1. let *viewbox* be the viewbox defined by the active SVG View
  2. return *viewbox*.width / *viewbox*.height
3. If the ‘[viewBox](#ViewBoxAttribute)’ on the ‘[svg](struct.md#SVGElement)’ element is correctly specified:
  1. let *viewbox* be the viewbox defined by the ‘[viewBox](#ViewBoxAttribute)’ attribute on the ‘[svg](struct.md#SVGElement)’ element
  2. return *viewbox*.width / *viewbox*.height
4. return null

The behaviour defined in this section is specific to CSS, but may be adapted to other host contexts. In all host contexts, the intrinsic aspect ratio, where available, must be respected when sizing the SVG viewport.

Examples:

**Example:** Intrinsic Aspect Ratio 1

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="10cm" height="5cm">
  ...
</svg>
```

In this example the intrinsic aspect ratio of the [SVG viewport](#TermSVGViewport) is 2:1. The intrinsic width is 10cm and the intrinsic height is 5cm.

**Example:** Intrinsic Aspect Ratio 2

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="100%" height="50%" viewBox="0 0 200 200">
  ...
</svg>
```

In this example the intrinsic aspect ratio of the outermost [SVG viewport](#TermSVGViewport) is 1:1. An aspect ratio calculation in this case allows embedding in an object within a containing block that is only constrained in one direction.

**Example:** Intrinsic Aspect Ratio 3

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="10cm" viewBox="0 0 200 200">
  ...
</svg>
```

In this case the intrinsic aspect ratio is 1:1.

**Example:** Intrinsic Aspect Ratio 4

```
<svg xmlns="http://www.w3.org/2000/svg"
     width="75%" height="10cm" viewBox="0 0 200 200">
  ...
</svg>
```

In this example, the intrinsic aspect ratio is 1:1.

<a id="issue19"></a>Add more examples for the new auto value? E.g. some of the examples provided by David Vest.

## <a id="VectorEffects"></a>8.13. Vector effects

Sometimes it is of interest to let the outline of an object keep its original width or to let the position of an object fix no matter which transforms are applied to it. For example, in a map with a 2px wide line representing roads it is of interest to keep the roads 2px wide even when the user zooms into the map, or introductory notes on the graphic chart in which panning is possible.

To offer such effects regarding special coordinate transformations and graphic drawings, SVG Tiny 1.2 introduced the [vector-effect](#VectorEffectProperty) property. Although SVG Tiny 1.2 introduced only non-scaling stroke behavior, this version introduces a number of additional effects. Furthermore, since these effects can be specified in combination, they show more various effects. And, future versions of the SVG language will allow for more powerful vector effects through this property.

<a id="issue31"></a> Values of [vector-effect](#VectorEffectProperty) other than non-scaling-stroke and none are at risk of being dropped from SVG 2 due to a lack of implementations. Feedback from implementers is requested, regarding the practicality of implementing them as currently specified, during the implementation period.

| Name: | *<a id="VectorEffectProperty"></a>vector-effect* |
| --- | --- |
| Value: | none \| non-scaling-stroke \| non-scaling-size \| non-rotation \| fixed-position |
| Initial: | none |
| Applies to: | [graphics elements](struct.md#TermGraphicsElement) and ‘[use](struct.md#UseElement)’ |
| Inherited: | no |
| Percentages: | N/A |
| Media: | visual |
| Computed value: | as specified |
| Animation type: | discrete |

**none**
: Specifies that no vector effect shall be applied, i.e. the default rendering behaviour from SVG 1.1 is used which is to first fill the geometry of a shape with a specified paint, then stroke the outline with a specified paint.
**non-scaling-stroke**
: Please refer to [this description](painting.md#non-scaling-stroke) of vector effect on painting.
**non-scaling-size**
: Specifies special [user coordinate system](../css-transforms-1.md#user-coordinate-system) toward this element and its descendant by constrained transformations with the following characteristics. The scale of the [user coordinate system](../css-transforms-1.md#user-coordinate-system) do not change in spite of change of [CTM](../css-transforms-1.md#current-transformation-matrix)s from a *host coordinate space*. However, it does not specify the suppression of rotation and skew. Also, it does not specify the fixation of placement of [user coordinate system](../css-transforms-1.md#user-coordinate-system). Since non-scaling-size suppresses scaling of [user coordinate system](../css-transforms-1.md#user-coordinate-system), it also has the characteristic of non-scaling-stroke. The transformation formula and the example behavior are indicated to the following chapter.
**non-rotation**
: Specifies special [user coordinate system](../css-transforms-1.md#user-coordinate-system) toward this element and its descendant by constrained transformations with the following characteristics. The rotation and skew of the [user coordinate system](../css-transforms-1.md#user-coordinate-system) is suppressed in spite of change of [CTM](../css-transforms-1.md#current-transformation-matrix)s from a *host coordinate space*. However, it does not specify the suppression of scaling. Also, it does not specify the fixation of placement of [user coordinate system](../css-transforms-1.md#user-coordinate-system). The transformation formula and the example behavior are indicated to the following chapter.
**fixed-position**
: Specifies special [user coordinate system](../css-transforms-1.md#user-coordinate-system) toward this element and its descendant by constrained transformations with the following characteristics. The placement of [user coordinate system](../css-transforms-1.md#user-coordinate-system) is fixed in spite of change of [CTM](../css-transforms-1.md#current-transformation-matrix)s from a *host coordinate space*. However, it does not specify the suppression of rotation, skew and scaling. When the element that has fixed-position effect and also has [transform](#TransformProperty) property, that property is consumed for this effect. The shift components *e* and *f* of matrix of [transform](#TransformProperty) property are used to transfer the origin of fixed [user coordinate system](../css-transforms-1.md#user-coordinate-system). The transformation formula and the example behavior are indicated to the following chapter.
These values can be enumerated. Thereby, the effect which has these characteristics simultaneously can be specified.

The host coordinate space for [vector-effect](#VectorEffectProperty) is the [viewport coordinate system](#TermViewportCoordinateSystem) of the [furthest ancestral SVG viewport](#TermFurthestAncestorSVGViewport).

Note: Future versions of SVG may allow ways to specify the device coordinate system.

### <a id="VectorEffectsCalculation"></a>8.13.1. Computing the vector effects

This section shows the list of transformation formulas regarding combinations of the values for clarification of the behavior of vector effects excluding non-scaling-stroke which has clear implications.

The [vector-effect](#VectorEffectProperty) property has no effect on transformations performed in a [3d rendering context](../css-transforms-2.md#3d-rendering-contexts).

The normal coordinate transformation formula from [user coordinate system](../css-transforms-1.md#user-coordinate-system) to [viewport coordinate system](#TermViewportCoordinateSystem) is as follows.

x viewport y viewport 1 = CTM ⋅ x userspace y userspace 1 CTM = a ctm c ctm e ctm b ctm d ctm f ctm 0 0 1

```
<circle vector-effect="veValue" transform="translate(x_o y_o)" cx="x_f" cy="y_f" r=".."/>
```

When the [vector-effect](#VectorEffectProperty) is added to an element like the above, the transformation formula for user coordinate to the device coordinate changes as follows. Here, *x_f* and *y_f* are user coordinate of the corresponding element and its descendant. And, *x_o* and *y_o* are matrix element *e* and *f* of the transform attribute which the corresponding element has. In addition, **|det(CTM)|** is absolute value of the determinants of [CTM](../css-transforms-1.md#current-transformation-matrix). When this value becomes 0 and non-scaling-size is appointed, [vector-effect](#VectorEffectProperty) becomes invalidity namely none.

det CTM = a ctm ⋅ d ctm - b ctm ⋅ c ctm

| veValue | Formula |
| --- | --- |
| non-scaling-size | x viewport y viewport 1 = CTM ⋅ 0 0 1 + CTM det CTM ⋅ 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |
| non-rotation | x viewport y viewport 1 = CTM ⋅ 0 0 1 + det CTM ⋅ 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |
| non-scaling-size non-rotation | x viewport y viewport 1 = CTM ⋅ 0 0 1 + 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |
| fixed-position | x viewport y viewport 1 = x o y o 1 + CTM ⋅ 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |
| fixed-position non-scaling-size | x viewport y viewport 1 = x o y o 1 + CTM det CTM ⋅ 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |
| fixed-position non-rotation | x viewport y viewport 1 = x o y o 1 + det CTM ⋅ 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |
| fixed-position non-scaling-size non-rotation | x viewport y viewport 1 = x o y o 1 + 1 0 0 0 1 0 0 0 0 ⋅ x f y f 1 |

### <a id="NestedVectorEffectsCalculation"></a>8.13.2. Computing the vector effects for nested viewport coordinate systems

Below is normal coordinate transformation formula for nested viewport coordinate systems without vector effects. **x_viewport(UA)** and **y_viewport(UA)** are coordinates which under the immediate control of [user agent](conform.md#TermUserAgent). **CTM_this** is [CTM](../css-transforms-1.md#current-transformation-matrix) for the transformation matrix from [user coordinate system](../css-transforms-1.md#user-coordinate-system) of an target graphic to [viewport coordinate system](#TermViewportCoordinateSystem) to which it belongs. **CTM_parent** is [CTM](../css-transforms-1.md#current-transformation-matrix) for the transformation matrix from aforementioned [viewport coordinate system](#TermViewportCoordinateSystem) to [viewport coordinate system](#TermViewportCoordinateSystem) of the parent of that. And, **CTM_root** is [CTM](../css-transforms-1.md#current-transformation-matrix) for rootmost viewport coordinate system (UA).

x viewport(UA) y viewport(UA) 1 = CTM root ⋅ ... ⋅ CTM parent ⋅ CTM this ⋅ x userspace y userspace 1

When applying seven formulas of the preceding section to nested viewport coordinate systems, the application way of those formulas changes as follows by whether viewport or screen is specified as the additional value of [vector-effect](#VectorEffectProperty).

When viewport value is specified, [user agent](conform.md#TermUserAgent) computes coordinates combining either of seven formulas of the preceding chapter, and the following formulas.

x viewport ( UA ) y viewport ( UA ) 1 = CTM root ⋅ ... ⋅ CTM parent ⋅ x viewport y viewport 1 CTM = CTM this

When screen value is specified, [user agent](conform.md#TermUserAgent) computes coordinates combining either of seven formulas of the preceding chapter, and the following formulas.

x viewport ( UA ) y viewport ( UA ) 1 = x viewport y viewport 1 CTM = CTM root ⋅ ... ⋅ CTM parent ⋅ CTM this

### <a id="VectorEffectsExamples"></a>8.13.3. Examples of vector effects

Below is an example of the non-scaling-stroke [vector-effect](#VectorEffectProperty).

```
<?xml version="1.0"?>
<svg xmlns="http://www.w3.org/2000/svg"
     width="6cm" height="4cm" viewBox="0 0 600 400" 
     viewport-fill="rgb(255,150,200)">

  <desc>Example non-scaling stroke</desc>
  <rect x="1" y="1" width="598" height="398" fill="none" stroke="black"/>
  
  <g transform="scale(9,1)">
    <line stroke="black" stroke-width="5" x1="10" y1="50" x2="10" y2="350"/>
    <line vector-effect="non-scaling-stroke" stroke="black" stroke-width="5" 
        x1="32" y1="50" x2="32" y2="350"/>
    <line vector-effect="none" stroke="black" stroke-width="5" 
        x1="55" y1="50" x2="55" y2="350"/>
  </g>

</svg>
```

Below is an example of the none [vector-effect](#VectorEffectProperty) (no vector effect).

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing none vector effect] | [image: Image showing none vector effect] |

Source code

```
<svg xmlns="http://www.w3.org/2000/svg" viewBox="-50,-50,500,500" height="500" width="500">

  <rect x="-50" y="-50" width="500" height="500" stroke="orange" stroke-width="3" fill="none"/>

  <!-- Nested user coordinate system is transformed by this transform attribute -->
  <g transform="matrix(2.1169438081370817,0.3576047954311102,-0.3576047954311102,1.4700998667618626,0,0)
                translate(-50,-50)">
    <svg viewBox="-50,-50,500,500" height="500" width="500">
      <!-- Graph paper on the this svg's base user coordinate system -->
      <g stroke="green" stroke-width="1" fill="none">
        <circle cx="0" cy="0" r="10"/>
        <circle cx="150" cy="150" r="7"/>
        <path fill="green" stroke="none" d="M0,-3 L30,-3 25,-10 50,0 25,10 30,3 0,3z"/>

        <line x1="-100" y1="-100" x2="600" y2="-100" stroke-dasharray="5,5"/>
        <line x1="-100" y1="000" x2="600" y2="000"/>
        <line x1="-100" y1="100" x2="600" y2="100" stroke-dasharray="5,5"/>
        <line x1="-100" y1="200" x2="600" y2="200" stroke-dasharray="5,5"/>
        <line x1="-100" y1="300" x2="600" y2="300" stroke-dasharray="5,5"/>
        <line x1="-100" y1="400" x2="600" y2="400" stroke-dasharray="5,5"/>
        <line x1="-100" y1="500" x2="600" y2="500" stroke-dasharray="5,5"/>

        <line y1="-100" x1="-100" y2="600" x2="-100" stroke-dasharray="5,5"/>
        <line y1="-100" x1="000" y2="600" x2="000"/>
        <line y1="-100" x1="100" y2="600" x2="100" stroke-dasharray="5,5"/>
        <line y1="-100" x1="200" y2="600" x2="200" stroke-dasharray="5,5"/>
        <line y1="-100" x1="300" y2="600" x2="300" stroke-dasharray="5,5"/>
        <line y1="-100" x1="400" y2="600" x2="400" stroke-dasharray="5,5"/>
        <line y1="-100" x1="500" y2="600" x2="500" stroke-dasharray="5,5"/>
      </g>

      <!-- Figure having vector effect -->
      <!-- A thick red right arrow and small rectangle on this figure's nested
           user coordinate system origin -->
      <path id="ve" vector-effect="none"
            stroke="red" stroke-width="3" fill="none"
            transform="matrix(1,0,0,1,150,150)"
            d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
    </svg>
  </g>
</svg>
```

Below is an example of the non-scaling-size.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing non-scaling-size vector effect] |

```
<path id="ve" vector-effect="non-scaling-size"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

Below is an example of the non-rotation.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing non-rotation vector effect] |

```
<path id="ve" vector-effect="non-rotation"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

Below is an example of the non-scaling-size non-rotation.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing non-scaling-size non-rotation vector effect] |

```
<path id="ve" vector-effect="non-scaling-size non-rotation"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

Below is an example of the fixed-position.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing fixed-position vector effect] |

```
<path id="ve" vector-effect="fixed-position"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

Below is an example of the non-scaling-size fixed-position.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing non-scaling-size fixed-position vector effect] |

```
<path id="ve" vector-effect="non-scaling-size fixed-position"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

Below is an example of the non-rotation fixed-position.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing non-rotation fixed-position vector effect] |

```
<path id="ve" vector-effect="non-rotation fixed-position"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

Below is an example of the non-scaling-size non-rotation fixed-position.

| Before changing CTM | After changing CTM |
| --- | --- |
| [image: Image showing appearance before the effect of the vector effect showing up] | [image: Image showing non-scaling-size non-rotation fixed-position vector effect] |

```
<path id="ve" vector-effect="non-scaling-size non-rotation fixed-position"
      stroke="red" stroke-width="3" fill="none"
      transform="matrix(1,0,0,1,150,150)"
      d="M-50,-50 L50,-50 50,-100 150,0 50,100 50,50 -50,50 -50,-50z M5 0 L0 -5 -5 0 0 5z"/>
```

## <a id="DOMInterfaces"></a>8.14. DOM interfaces

### <a id="InterfaceSVGTransform"></a>8.14.1. Interface SVGTransform

The [SVGTransform](#InterfaceSVGTransform) interface is used to represent [<transform-function>](../css-transforms-1.md#typedef-transform-function) values that appear in the [transform](#TransformProperty) property and its presentation attributes ‘transform’, ‘[gradientTransform](pservers.md#LinearGradientElementGradientTransformAttribute)’ and ‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’. An [SVGTransform](#InterfaceSVGTransform) represents a single component in a transform list, such as a single scale(…) or matrix(…) value.

<a id="ReadOnlyTransform"></a>An [SVGTransform](#InterfaceSVGTransform) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

<a id="TransformAssociatedElement"></a>An [SVGTransform](#InterfaceSVGTransform) object can be *associated* with a particular element. The associated element is used to determine which element's ‘transform’ presentation attribute to update if the object [reflects](types.md#TermReflect) that attribute. Unless otherwise described, an [SVGTransform](#InterfaceSVGTransform) object is not associated with any element.

<a id="TransformMode"></a>Every [SVGTransform](#InterfaceSVGTransform) object operates in one of two modes. It can:

1. *reflect an element of a presentation attribute value* (being exposed through the methods on the [baseVal](#__svg__SVGAnimatedTransformList__baseVal) member of an [SVGAnimatedTransformList](#InterfaceSVGAnimatedTransformList)),
2. *be detached*, which is the case for [SVGTransform](#InterfaceSVGTransform) objects created with [createSVGTransform](struct.md#__svg__SVGSVGElement__createSVGTransform) and [createSVGTransformFromMatrix](struct.md#__svg__SVGSVGElement__createSVGTransformFromMatrix).

An [SVGTransform](#InterfaceSVGTransform) object maintains an internal [<transform-function>](../css-transforms-1.md#typedef-transform-function) value, which is called its *<a id="TransformValue"></a>value*. It also maintains a DOMMatrix object, which is called its *<a id="TransformMatrixObject"></a>matrix object*, which is the object returned from the [matrix](#__svg__SVGTransform__matrix) IDL attribute. An [SVGTransform](#InterfaceSVGTransform) object's [matrix object](#TransformMatrixObject) is always kept synchronized with its [value](#TransformValue).

```
[Exposed=Window]
interface SVGTransform {

  // Transform Types
  const unsigned short SVG_TRANSFORM_UNKNOWN = 0;
  const unsigned short SVG_TRANSFORM_MATRIX = 1;
  const unsigned short SVG_TRANSFORM_TRANSLATE = 2;
  const unsigned short SVG_TRANSFORM_SCALE = 3;
  const unsigned short SVG_TRANSFORM_ROTATE = 4;
  const unsigned short SVG_TRANSFORM_SKEWX = 5;
  const unsigned short SVG_TRANSFORM_SKEWY = 6;

  readonly attribute unsigned short type;
  [SameObject] readonly attribute DOMMatrix matrix;
  readonly attribute float angle;

  undefined setMatrix(optional DOMMatrix2DInit matrix = {});
  undefined setTranslate(float tx, float ty);
  undefined setScale(float sx, float sy);
  undefined setRotate(float angle, float cx, float cy);
  undefined setSkewX(float angle);
  undefined setSkewY(float angle);
};
```

The numeric transform type constants defined on [SVGTransform](#InterfaceSVGTransform) are used to represent the type of an [SVGTransform](#InterfaceSVGTransform)'s [value](#TransformValue). Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_MATRIX"></a>SVG_TRANSFORM_MATRIX** | A matrix(…) value. |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_TRANSLATE"></a>SVG_TRANSFORM_TRANSLATE** | A translate(…) value. |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_SCALE"></a>SVG_TRANSFORM_SCALE** | A scale(…) value. |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_ROTATE"></a>SVG_TRANSFORM_ROTATE** | A rotate(…) value. |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_SKEWX"></a>SVG_TRANSFORM_SKEWX** | A skewX(…) value. |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_SKEWY"></a>SVG_TRANSFORM_SKEWY** | A skewY(…) value. |
| **<a id="__svg__SVGTransform__SVG_TRANSFORM_UNKNOWN"></a>SVG_TRANSFORM_UNKNOWN** | Some other type of value. |

The use of numeric transform type constants is an anti-pattern and new constant values will not be introduced for any transform types supported by [SVGTransform](#InterfaceSVGTransform). If other types of transforms are supported and used, the [SVGTransform](#InterfaceSVGTransform) uses the [SVG_TRANSFORM_UNKNOWN](#__svg__SVGTransform__SVG_TRANSFORM_UNKNOWN) type. See below for details on how the other properties of an [SVGTransform](#InterfaceSVGTransform) operate with these types of transforms.

The **<a id="__svg__SVGTransform__type"></a>type** IDL attribute represents the type of transform item that the [SVGTransform](#InterfaceSVGTransform)'s [value](#TransformValue) is. On getting [type](#__svg__SVGTransform__type), the following steps are run:

1. If the [SVGTransform](#InterfaceSVGTransform)'s [value](#TransformValue) is a matrix(…), translate(…), scale(…), rotate(…), skewX(…) or skewY(…) function, then return the corresponding constant value from the transform type table above.
2. Otherwise, return [SVG_TRANSFORM_UNKNOWN](#__svg__SVGTransform__SVG_TRANSFORM_UNKNOWN).
For example, for a scaleX(…) or translate3d(…) transform, [SVG_TRANSFORM_UNKNOWN](#__svg__SVGTransform__SVG_TRANSFORM_UNKNOWN) would be returned.

The **<a id="__svg__SVGTransform__matrix"></a>matrix** IDL attribute represents the transform as a 4x4 homogeneous matrix, and on getting returns the [SVGTransform](#InterfaceSVGTransform)'s [matrix object](#TransformMatrixObject). When the [matrix object](#TransformMatrixObject) is first created, its values are set to match the [SVGTransform](#InterfaceSVGTransform)'s transform function [value](#TransformValue), and is set to [reflects the SVGTransform](#MatrixMode).

See the [CSS Transforms](../css-transforms-1.md#mathematical-description) specification for a description of how the different transform function types correspond to particular matrix values.

The **<a id="__svg__SVGTransform__angle"></a>angle** IDL attribute represents the angle parameter of a rotate(…), skewX(…) or skewY(…) transform function. On getting, the following steps are run:

1. If the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue) is a rotate(…), skewX(…) or skewY(…) function, return its angle argument in degrees.
2. Otherwise, return 0.

The **<a id="__svg__SVGTransform__setMatrix"></a>setMatrix** method is used to set the [SVGTransform](#InterfaceSVGTransform) to a given matrix value. When setMatrix(*matrix*) is called, the following steps are run:

1. If the [SVGTransform](#InterfaceSVGTransform) object is [read only](#ReadOnlyTransform), then throw a NoModificationAllowedError.
2. Let *newMatrix* be the result of `DOMMatrixReadOnly.fromMatrix(*matrix*)`, including the validate and fix-up steps for missing values. If that method throws an error, then re-throw that error and abort these steps.
3. If *newMatrix*.is2D() would return true, then set the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue) to a matrix(…) value that represents the same matrix as *newMatrix*.
4. Otherwise, set the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue) to a matrix3d(…) value that represents the same matrix as *newMatrix*.
5. In either case, [matrix object](#TransformMatrixObject) gets synchronized to the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue).
6. If the [SVGTransform](#InterfaceSVGTransform) object [reflects a presentation attribute value of an element](#TransformMode), then [reserialize](types.md#TermReserialize) the reflected attribute.

The **<a id="__svg__SVGTransform__setTranslate"></a>setTranslate**, **<a id="__svg__SVGTransform__setScale"></a>setScale**, **<a id="__svg__SVGTransform__setRotate"></a>setRotate**, **<a id="__svg__SVGTransform__setSkewX"></a>setSkewX** and **<a id="__svg__SVGTransform__setSkewY"></a>setSkewY** methods are used to set the [SVGTransform](#InterfaceSVGTransform) to a new transform function value. When one of these methods is called, the following steps are run:

1. If the [SVGTransform](#InterfaceSVGTransform) object is [read only](#ReadOnlyTransform), then throw a NoModificationAllowedError.
2. Set the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue) to a new transform function value, depending on which method was called:
**setTranslate(*tx*, *ty*)**
: the new transform function value is translate(*tx*, *ty*)
**setScale(*sx*, *sy*)**
: the new transform function value is scale(*sx*, *sy*)
**setRotate(*angle*, *cx*, *cy*)**
: the new transform function value is rotate(*angle*, *cx*, *cy*)
**setSkewX(*angle*)**
: the new transform function value is skewX(*angle*)
**setSkewY(*angle*)**
: the new transform function value is skewY(*angle*)
3. Set the components of the [SVGTransform](#InterfaceSVGTransform) object's [matrix object](#TransformMatrixObject) to match the new transform function [value](#TransformValue).
4. If the [SVGTransform](#InterfaceSVGTransform) object [reflects an element of a presentation attribute value](#TransformMode), then [reserialize](types.md#TermReserialize) the reflected attribute.

This specification imposes additional requirements on the behavior of DOMMatrix objects beyond those described in the Geometry Interfaces specification, so that they can be used to reflect presentation attributes that take transform values.

<a id="MatrixMode"></a>Every DOMMatrix object operates in one of two modes. It can:

1. *reflect an [SVGTransform](#InterfaceSVGTransform)* (being exposed through the [matrix](#__svg__SVGTransform__matrix) IDL attribute on an [SVGTransform](#InterfaceSVGTransform)), or
2. *be detached*, which is the case for DOMMatrix objects created using their constructor or with [createSVGMatrix](struct.md#__svg__SVGSVGElement__createSVGMatrix).

<a id="ReadOnlyMatrix"></a>A DOMMatrix can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown. When assigning to any of a read only DOMMatrix's IDL attributes, or when invoking any of its mutable transform methods, a NoModificationAllowedError exception will be thrown instead of updating the internal value.

Note that this applies only to the read-write DOMMatrix interface; the DOMMatrixReadOnly interface, which is not used for reflecting [transform](#TransformProperty), will already throw an exception if an attempt is made to modify it.

When assigning to any of a writable DOMMatrix's IDL attributes, or when invoking any of its mutable transform methods, the following steps are run after updating the internal matrix value:

1. If the DOMMatrix [reflects an SVGTransform](#MatrixMode), then:
  1. If the DOMMatrix would return true from its is2d method, then set the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue) to a matrix(…) value that represents the same matrix as the DOMMatrix.
  2. Otherwise, set the [SVGTransform](#InterfaceSVGTransform) object's [value](#TransformValue) to a matrix3d(…) value that represents the same matrix as the DOMMatrix.
  3. If the [SVGTransform](#InterfaceSVGTransform) object [reflects an element of a presentation attribute value](#TransformMode), then [reserialize](types.md#TermReserialize) the reflected attribute.

### <a id="InterfaceSVGTransformList"></a>8.14.2. Interface SVGTransformList

The [SVGTransformList](#InterfaceSVGTransformList) interface is a [list interface](types.md#TermListInterface) whose elements are [SVGTransform](#InterfaceSVGTransform) objects. An [SVGTransformList](#InterfaceSVGTransformList) represents a value that the [transform](#TransformProperty) property can take, namely either a [<transform-list>](../css-transforms-1.md#typedef-transform-list) or the keyword none.

```
[Exposed=Window]
interface SVGTransformList {

  readonly attribute unsigned long length;
  readonly attribute unsigned long numberOfItems;

  undefined clear();
  SVGTransform initialize(SVGTransform newItem);
  getter SVGTransform getItem(unsigned long index);
  SVGTransform insertItemBefore(SVGTransform newItem, unsigned long index);
  SVGTransform replaceItem(SVGTransform newItem, unsigned long index);
  SVGTransform removeItem(unsigned long index);
  SVGTransform appendItem(SVGTransform newItem);
  setter undefined (unsigned long index, SVGTransform newItem);

  // Additional methods not common to other list interfaces.
  SVGTransform createSVGTransformFromMatrix(optional DOMMatrix2DInit matrix = {});
  SVGTransform? consolidate();
};
```

The **<a id="__svg__SVGTransformList__createSVGTransformFromMatrix"></a>createSVGTransformFromMatrix** method is used to create a new [SVGTransform](#InterfaceSVGTransform) object from a matrix object. When the createSVGTransformFromMatrix(*matrix*) method is called, the following steps are run:

1. Let *transform* be a newly created [SVGTransform](#InterfaceSVGTransform) object that is [detached](#TransformMode).
2. Follow the steps that would be run if the [setMatrix](#__svg__SVGTransform__setMatrix) method on *transform* were called, passing *matrix* as its argument.
3. Return *transform*.

The **<a id="__svg__SVGTransformList__consolidate"></a>consolidate** method is used to convert the transform list into an equivalent transformation using a single transform function. When the consolidate() method is called, the following steps are run:

1. If the [SVGTransformList](#InterfaceSVGTransformList) object is [read only](types.md#ReadOnlyList), then throw a NoModificationAllowedError.
2. If the list is empty, return null.
3. [Detach](types.md#TermDetach) and then remove all elements in the list.
4. Let *transform* be a newly created [SVGTransform](#InterfaceSVGTransform) object.
5. Let *matrix* be the matrix value obtained by beginning with an identity matrix, and then post-multiplying the value of the [matrix object](#TransformMatrixObject) for each [SVGTransform](#InterfaceSVGTransform) in the list, in order.
6. Set the components of *transform*'s [matrix object](#TransformMatrixObject) to the component values in *matrix*.
7. If *transform*'s [matrix object](#TransformMatrixObject) would return true from its is2d method, then set *transform*'s [value](#TransformValue) to a matrix(…) value that represents the same matrix as the [matrix object](#TransformMatrixObject).
8. Otherwise, set *transform*'s [value](#TransformValue) to a matrix3d(…) value that represents the same matrix as the [matrix object](#TransformMatrixObject).
9. [Attach](types.md#TermAttach) *transform* to this [SVGTransformList](#InterfaceSVGTransformList).
10. Append *transform* to this list.
11. If the list [reflects](types.md#TermReflect) a presentation attribute, then [reserialize](types.md#TermReserialize) the reflected attribute.
12. Return *transform*.

The behavior of all other interface members of [SVGLengthList](types.md#InterfaceSVGLengthList) are defined in [List interfaces](types.md#ListInterfaces).

### <a id="InterfaceSVGAnimatedTransformList"></a>8.14.3. Interface SVGAnimatedTransformList

An [SVGAnimatedTransformList](#InterfaceSVGAnimatedTransformList) object is used to [reflect](types.md#TermReflect) the [transform](#TransformProperty) property and its corresponding presentation attribute (which, depending on the element, is ‘transform’, ‘[gradientTransform](pservers.md#LinearGradientElementGradientTransformAttribute)’ or ‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’).

```
[Exposed=Window]
interface SVGAnimatedTransformList {
  [SameObject] readonly attribute SVGTransformList baseVal;
  [SameObject] readonly attribute SVGTransformList animVal;
};
```

The **<a id="__svg__SVGAnimatedTransformList__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedTransformList__animVal"></a>animVal** IDL attributes represent the value of the reflected presentation attribute. On getting [baseVal](#__svg__SVGAnimatedTransformList__baseVal) or [animVal](#__svg__SVGAnimatedTransformList__animVal), an [SVGTransformList](#InterfaceSVGTransformList) object is returned that reflects the given presentation attribute.

### <a id="InterfaceSVGPreserveAspectRatio"></a>8.14.4. Interface SVGPreserveAspectRatio

The [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) interface is used to represent values for the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute.

<a id="ReadOnlyPreserveAspectRatio"></a>An [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) object can be designated as *read only*, which means that attempts to modify the object will result in an exception being thrown, as described below.

<a id="PreserveAspectRatioMode"></a>Every [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) object *reflects the base value* of a [reflected](types.md#TermReflect) ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute (being exposed through the methods on the [baseVal](#__svg__SVGAnimatedPreserveAspectRatio__baseVal) or [animVal](#__svg__SVGAnimatedPreserveAspectRatio__animVal) member of an [SVGAnimatedPreserveAspectRatio](#InterfaceSVGAnimatedPreserveAspectRatio)).

```
[Exposed=Window]
interface SVGPreserveAspectRatio {

  // Alignment Types
  const unsigned short SVG_PRESERVEASPECTRATIO_UNKNOWN = 0;
  const unsigned short SVG_PRESERVEASPECTRATIO_NONE = 1;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMINYMIN = 2;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMIDYMIN = 3;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMAXYMIN = 4;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMINYMID = 5;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMIDYMID = 6;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMAXYMID = 7;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMINYMAX = 8;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMIDYMAX = 9;
  const unsigned short SVG_PRESERVEASPECTRATIO_XMAXYMAX = 10;

  // Meet-or-slice Types
  const unsigned short SVG_MEETORSLICE_UNKNOWN = 0;
  const unsigned short SVG_MEETORSLICE_MEET = 1;
  const unsigned short SVG_MEETORSLICE_SLICE = 2;

  attribute unsigned short align;
  attribute unsigned short meetOrSlice;
};
```

The numeric alignment type constants defined on [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) are used to represent the alignment keyword values that ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ can take. Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_NONE"></a>SVG_PRESERVEASPECTRATIO_NONE** | The none keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMINYMIN"></a>SVG_PRESERVEASPECTRATIO_XMINYMIN** | The xMinYMin keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMIDYMIN"></a>SVG_PRESERVEASPECTRATIO_XMIDYMIN** | The xMidYMin keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMAXYMIN"></a>SVG_PRESERVEASPECTRATIO_XMAXYMIN** | The xMaxYMin keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMINYMID"></a>SVG_PRESERVEASPECTRATIO_XMINYMID** | The xMinYMid keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMIDYMID"></a>SVG_PRESERVEASPECTRATIO_XMIDYMID** | The xMidYMid keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMAXYMID"></a>SVG_PRESERVEASPECTRATIO_XMAXYMID** | The xMaxYMid keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMINYMAX"></a>SVG_PRESERVEASPECTRATIO_XMINYMAX** | The xMinYMax keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMIDYMAX"></a>SVG_PRESERVEASPECTRATIO_XMIDYMAX** | The xMidYMax keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_XMAXYMAX"></a>SVG_PRESERVEASPECTRATIO_XMAXYMAX** | The xMaxYMax keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_UNKNOWN"></a>SVG_PRESERVEASPECTRATIO_UNKNOWN** | Some other type of value. |

Similarly, the numeric meet-or-slice type constants defined on [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) are used to represent the meet-or-slice keyword values that ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ can take. Their meanings are as follows:

| Constant | Meaning |
| --- | --- |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_MEETORSLICE_MEET"></a>SVG_MEETORSLICE_MEET** | The meet keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_MEETORSLICE_SLICE"></a>SVG_MEETORSLICE_SLICE** | The slice keyword. |
| **<a id="__svg__SVGPreserveAspectRatio__SVG_MEETORSLICE_UNKNOWN"></a>SVG_MEETORSLICE_UNKNOWN** | Some other type of value. |

The **<a id="__svg__SVGPreserveAspectRatio__align"></a>align** IDL attribute represents the alignment keyword part of the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ value. On getting, the following steps are run:

1. Let *value* [reflect the base value](#PreserveAspectRatioMode) of a ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute. *value* is the current non-animated value of the attribute (using the attribute's [initial value](types.md#TermInitialValue) if it is not present or invalid).
2. Return the constant value as specified in the alignment constant table above for the alignment keyword in *value*.

On setting [align](#__svg__SVGPreserveAspectRatio__align), the following steps are run:

1. If the [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) is [read only](#ReadOnlyPreserveAspectRatio), then throw a NoModificationAllowedError.
2. If *value* is [SVG_PRESERVEASPECTRATIO_UNKNOWN](#__svg__SVGPreserveAspectRatio__SVG_PRESERVEASPECTRATIO_UNKNOWN) or does not have a corresponding entry in the alignment keyword table above, then throw a TypeError.
3. Let *string* be the corresponding keyword in the alignment keyword table above for *value*.
4. Append a single U+0020 SPACE character to *string*.
5. Let *meet or slice* be the value that would be returned from the [meetOrSlice](#__svg__SVGPreserveAspectRatio__meetOrSlice) member on this [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio).
6. Append to *string* the corresponding keyword in the meet-or-slice keyword table above for *meet or slice*.
7. Set the reflected ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute to *string*.

The **<a id="__svg__SVGPreserveAspectRatio__meetOrSlice"></a>meetOrSlice** IDL attribute represents the alignment keyword part of the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ value. On getting, the following steps are run:

1. Let *value* be a ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ value that [reflects the base value](#PreserveAspectRatioMode) of a ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute *value* is the current non-animated value of the attribute.
2. If the meet-or-slice value is not present in *value*, then return [SVG_MEETORSLICE_MEET](#__svg__SVGPreserveAspectRatio__SVG_MEETORSLICE_MEET).
3. Otherwise, the meet-or-slice value is present. Return the constant value as specified in the meet-or-slice constant table above for the meet-or-slice keyword in *value*.

On setting [meetOrSlice](#__svg__SVGPreserveAspectRatio__meetOrSlice), the following steps are run:

1. If the [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) is [read only](#ReadOnlyPreserveAspectRatio), then throw a NoModificationAllowedError.
2. If *value* is [SVG_MEETORSLICE_UNKNOWN](#__svg__SVGPreserveAspectRatio__SVG_MEETORSLICE_UNKNOWN) or does not have a corresponding entry in the meet-or-slice keyword table above, then throw a TypeError.
3. Let *align* be the value that would be returned from the [align](#__svg__SVGPreserveAspectRatio__align) member on this [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio).
4. Let *string* be the corresponding keyword in the alignment keyword table above for *align*.
5. Append a single U+0020 SPACE character to *string*.
6. Append to *string* the corresponding keyword in the meet-or-slice keyword table above for *value*.
7. Set the reflected ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute to *string*.

### <a id="InterfaceSVGAnimatedPreserveAspectRatio"></a>8.14.5. Interface SVGAnimatedPreserveAspectRatio

An [SVGAnimatedPreserveAspectRatio](#InterfaceSVGAnimatedPreserveAspectRatio) object is used to [reflect](types.md#TermReflect) the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute.

```
[Exposed=Window]
interface SVGAnimatedPreserveAspectRatio {
  [SameObject] readonly attribute SVGPreserveAspectRatio baseVal;
  [SameObject] readonly attribute SVGPreserveAspectRatio animVal;
};
```

The **<a id="__svg__SVGAnimatedPreserveAspectRatio__baseVal"></a>baseVal** and **<a id="__svg__SVGAnimatedPreserveAspectRatio__animVal"></a>animVal** IDL attributes represent the current non-animated value of the reflected ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute. On getting [baseVal](#__svg__SVGAnimatedPreserveAspectRatio__baseVal) or [animVal](#__svg__SVGAnimatedPreserveAspectRatio__animVal), an [SVGPreserveAspectRatio](#InterfaceSVGPreserveAspectRatio) object is returned that [reflects the base value](#PreserveAspectRatioMode) of the ‘[preserveAspectRatio](#PreserveAspectRatioAttribute)’ attribute on the SVG element that the object with the reflecting IDL attribute of type [SVGAnimatedPreserveAspectRatio](#InterfaceSVGAnimatedPreserveAspectRatio) was obtained from.
