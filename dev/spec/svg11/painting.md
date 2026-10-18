<!-- https://www.w3.org/TR/SVG11/painting.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](text.md) ⋅ [Next](color.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 11 Painting: Filling, Stroking and Marker Symbols

## <a id="Introduction"></a>11.1 Introduction

[‘path’](paths.md#PathElement) elements, [‘text’](text.md#TextElement) elements and [basic shapes](shapes.md) can be **filled** (which means painting the interior of the object) and **stroked** (which means painting along the outline of the object). Filling and stroking both can be thought of in more general terms as **painting** operations.

Certain elements (i.e., [‘path’](paths.md#PathElement), [‘polyline’](shapes.md#PolylineElement), [‘polygon’](shapes.md#PolygonElement) and [‘line’](shapes.md#LineElement) elements) can also have [marker symbols](#Markers) drawn at their vertices.

With SVG, you can paint (i.e., fill or stroke) with:

- a single color
- a gradient (linear or radial)
- a pattern (vector or image, possibly tiled)
- custom paints available via [extensibility](extend.md)

SVG uses the general notion of a **paint server**. Paint servers are specified using a [IRI reference](linking.md#IRIReference) on a [‘fill’](#FillProperty) or [‘stroke’](#StrokeProperty) property. [Gradients and patterns](pservers.md) are just specific types of paint servers.

## <a id="SpecifyingPaint"></a>11.2 Specifying paint

Properties [‘fill’](#FillProperty) and [‘stroke’](#StrokeProperty) take on a value of type <paint>, which is specified as follows:

| <paint>: | none \| currentColor \| [<color>](types.md#DataTypeColor) [[<icccolor>](types.md#DataTypeICCColor)] \| [<funciri>](types.md#DataTypeFuncIRI) [ none \| currentColor \| [<color>](types.md#DataTypeColor) [[<icccolor>](types.md#DataTypeICCColor)] ] \| inherit |
| --- | --- |

****none****
: Indicates that no paint is applied.
****currentColor****
: Indicates that painting is done using the current animated value of the color specified by the [‘color’](color.md#ColorProperty) property. This mechanism is provided to facilitate sharing of color attributes between parent grammars such as other (non-SVG) XML. This mechanism allows you to define a style in your HTML which sets the ‘color’ property and then pass that style to the SVG user agent so that your SVG text will draw in the same color.
****<color> [<icccolor>]****
: [<color>](types.md#DataTypeColor) is the explicit color (in the sRGB color space [[SRGB](refs.md#ref-SRGB)]) to be used to paint the current object. SVG supports all of the syntax alternatives for <color> defined in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 4.3.6), with the exception that SVG contains an expanded list of [recognized color keywords names](types.md#ColorKeywords). If an optional ICC color specification [[ICC42](refs.md#ref-ICC42)] is provided, then the user agent searches the color profile description database for a [color profile description](color.md#ColorProfileDescriptions) entry whose name descriptor matches the <name> part of the [<icccolor>](types.md#DataTypeICCColor) and uses the last matching entry that is found. (If no match is found, then the ICC color specification is ignored.) The comma and/or whitespace separated list of [<number>](types.md#DataTypeNumber)s is a set of ICC-profile-specific color values. (In most cases, the [<number>](types.md#DataTypeNumber)s will be in the range 0 to 1.) On platforms which support ICC-based color management, the [<icccolor>](types.md#DataTypeICCColor) gets precedence over the [<color>](types.md#DataTypeColor) (which is in the sRGB color space). Note that color interpolation occurs in an RGB color space even if an ICC-based color specification is provided (see [‘color-interpolation’](#ColorInterpolationProperty) and [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty)). For more on ICC-based colors, refer to [Color profile descriptions](color.md#ColorProfileDescriptions).
****[<funciri>](types.md#DataTypeFuncIRI)    [ none |       currentColor |       <color> [<icccolor>] ]****
: The [<funciri>](types.md#DataTypeFuncIRI) is used to identify a [paint server](pservers.md) such as a gradient, a pattern or a custom paint defined by an extension (see [Extensibility](extend.md)). The <funciri> points to the paint server (e.g., a [gradient](pservers.md#Gradients) or [pattern](pservers.md#Patterns)) to be used to paint the current object. If the [IRI reference](linking.md#IRIReference) is not valid (e.g., it points to an object that doesn't exist or the object is not a valid paint server), then the paint method following the <funciri> (i.e., none | currentColor | <color> [<icccolor>] is used if provided; otherwise, the document is in error (see [Error processing](implnote.md#ErrorProcessing)).
## <a id="FillProperties"></a>11.3 Fill Properties

**<a id="FillProperty"></a>‘fill’**
:

| *Value:* | <paint> (See [Specifying paint](#SpecifyingPaint)) |
| --- | --- |
| *Initial:* | black |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘fill’](#FillProperty) property paints the interior of the given graphical element. The area to be painted consists of any areas inside the outline of the shape. To determine the inside of the shape, all subpaths are considered, and the interior is determined according to the rules associated with the current value of the [‘fill-rule’](#FillRuleProperty) property. The zero-width geometric outline of a shape is included in the area to be painted.

The fill operation fills open subpaths by performing the fill operation as if an additional "closepath" command were added to the path to connect the last point of the subpath with the first point of the subpath. Thus, fill operations apply to both open subpaths within [‘path’](paths.md#PathElement) elements (i.e., subpaths without a closepath command) and [‘polyline’](shapes.md#PolylineElement) elements.

**<a id="FillRuleProperty"></a>‘fill-rule’**
:

| *Value:* | nonzero \| evenodd \| inherit |
| --- | --- |
| *Initial:* | nonzero |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘fill-rule’](#FillRuleProperty) property indicates the algorithm which is to be used to determine what parts of the canvas are included inside the shape. For a simple, non-intersecting path, it is intuitively clear what region lies "inside"; however, for a more complex path, such as a path that intersects itself or where one subpath encloses another, the interpretation of "inside" is not so obvious.

The [‘fill-rule’](#FillRuleProperty) property provides two options for how the inside of a shape is determined:

**nonzero**
: This rule determines the "insideness" of a point on the canvas by drawing a ray from that point to infinity in any direction and then examining the places where a segment of the shape crosses the ray. Starting with a count of zero, add one each time a path segment crosses the ray from left to right and subtract one each time a path segment crosses the ray from right to left. After counting the crossings, if the result is zero then the point is *outside* the path. Otherwise, it is *inside*. The following drawing illustrates the nonzero rule:

[image: Image showing nonzero fill rule]

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/fillrule-nonzero.svg)

**evenodd**
: This rule determines the "insideness" of a point on the canvas by drawing a ray from that point to infinity in any direction and counting the number of path segments from the given shape that the ray crosses. If this number is odd, the point is inside; if even, the point is outside. The following drawing illustrates the evenodd rule:

[image: Image showing evenodd fill rule]

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/fillrule-evenodd.svg)

(Note: the above explanations do not specify what to do if a path segment coincides with or is tangent to the ray. Since any ray will do, one may simply choose a different ray that does not have such problem intersections.)

**<a id="FillOpacityProperty"></a>‘fill-opacity’**
:

| *Value:* | <opacity-value> \| inherit |
| --- | --- |
| *Initial:* | 1 |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

[‘fill-opacity’](#FillOpacityProperty) specifies the opacity of the painting operation used to paint the interior the current object. (See [Painting shapes and text](render.md#PaintingShapesAndText).)

**<opacity-value>**
: The opacity of the painting operation used to fill the current object, as a [<number>](types.md#DataTypeNumber). Any values outside the range 0.0 (fully transparent) to 1.0 (fully opaque) will be clamped to this range. (See [Clamping values which are restricted to a particular range](implnote.md#RangeClamping).)
Related properties: [‘stroke-opacity’](#StrokeOpacityProperty) and [‘opacity’](masking.md#OpacityProperty).

## <a id="StrokeProperties"></a>11.4 Stroke Properties

The following are the properties which affect how an element is stroked.

In all cases, all stroking properties which are affected by directionality, such as those having to do with dash patterns, must be rendered such that the stroke operation starts at the same point at which the graphics element starts. In particular, for [‘path’](paths.md#PathElement) elements, the start of the path is the first point of the initial "moveto" command.

For stroking properties such as dash patterns whose computations are dependent on progress along the outline of the graphics element, distance calculations are required to utilize the SVG user agent's standard [Distance along a path](paths.md#DistanceAlongAPath) algorithms.

When stroking is performed using a complex paint server, such as a gradient or a pattern, the stroke operation must be identical to the result that would have occurred if the geometric shape defined by the geometry of the current graphics element and its associated stroking properties were converted to an equivalent [‘path’](paths.md#PathElement) element and then filled using the given paint server.

**<a id="StrokeProperty"></a>‘stroke’**
:

| *Value:* | <paint> (See [Specifying paint](#SpecifyingPaint)) |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

The [‘stroke’](#StrokeProperty) property paints along the outline of the given graphical element.

A subpath (see [Paths](paths.md)) consisting of a single [moveto](paths.md#PathDataMovetoCommands) shall not be stroked. Any zero length subpath shall not be stroked if the [‘stroke-linecap’](#StrokeLinecapProperty) property has a value of butt but shall be stroked if the [‘stroke-linecap’](#StrokeLinecapProperty) property has a value of round or square, producing respectively a circle or a square centered at the given point. Examples of zero length subpaths include 'M 10,10 L 10,10', 'M 20,20 h 0', 'M 30,30 z' and 'M 40,40 c 0,0 0,0 0,0'.

**<a id="StrokeWidthProperty"></a>‘stroke-width’**
:

| *Value:* | [<percentage>](types.md#DataTypePercentage) \| [<length>](types.md#DataTypeLength) \| inherit |
| --- | --- |
| *Initial:* | 1 |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | Yes |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

This property specifies the width of the stroke on the current object. If a [<percentage>](types.md#DataTypePercentage) is used, the value represents a percentage of the current viewport. (See [Units](coords.md#Units).)

A zero value causes no stroke to be painted. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)).

**<a id="StrokeLinecapProperty"></a>‘stroke-linecap’**
:

| *Value:* | butt \| round \| square \| inherit |
| --- | --- |
| *Initial:* | butt |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

[‘stroke-linecap’](#StrokeLinecapProperty) specifies the shape to be used at the end of open subpaths when they are stroked. For further details see the [path implementation notes](implnote.md#PathElementImplementationNotes).

**butt**
: See drawing below.
**round**
: See drawing below.
**square**
: See drawing below.
[image: Image showing stroke-linecap alternatives]

[View this example as SVG (SVG- and CSS-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/linecap.svg)

**<a id="StrokeLinejoinProperty"></a>‘stroke-linejoin’**
:

| *Value:* | miter \| round \| bevel \| inherit |
| --- | --- |
| *Initial:* | miter |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

[‘stroke-linejoin’](#StrokeLinejoinProperty) specifies the shape to be used at the corners of paths or basic shapes when they are stroked. For further details see the [path implementation notes](implnote.md#PathElementImplementationNotes).

**miter**
: See drawing below.
**round**
: See drawing below.
**bevel**
: See drawing below.
[image: Image showing stroke-linejoin alternatives]

[View this example as SVG (SVG- and CSS-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/linejoin.svg)

**<a id="StrokeMiterlimitProperty"></a>‘stroke-miterlimit’**
:

| *Value:* | <miterlimit> \| inherit |
| --- | --- |
| *Initial:* | 4 |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

When two line segments meet at a sharp angle and miter joins have been specified for [‘stroke-linejoin’](#StrokeLinejoinProperty), it is possible for the miter to extend far beyond the thickness of the line stroking the path. The [‘stroke-miterlimit’](#StrokeMiterlimitProperty) imposes a limit on the ratio of the miter length to the [‘stroke-width’](#StrokeWidthProperty). When the limit is exceeded, the join is converted from a miter to a bevel.

**<miterlimit>**
: The limit on the ratio of the miter length to the [‘stroke-width’](#StrokeWidthProperty). The value of <miterlimit> must be a [<number>](types.md#DataTypeNumber) greater than or equal to 1. Any other value is an error (see [Error processing](implnote.md#ErrorProcessing)).
The ratio of miter length (distance between the outer tip and the inner corner of the miter) to [‘stroke-width’](#StrokeWidthProperty) is directly related to the angle (theta) between the segments in user space by the formula:

```
miterLength / stroke-width = 1 / sin ( theta / 2 )
```

For example, a miter limit of 1.414 converts miters to bevels for theta less than 90 degrees, a limit of 4.0 converts them for theta less than approximately 29 degrees, and a limit of 10.0 converts them for theta less than approximately 11.5 degrees.

**<a id="StrokeDasharrayProperty"></a>‘stroke-dasharray’**
:

| *Value:* | none \| <dasharray> \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | yes (see below) |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes ([non-additive](animate.md#AdditionAttributes)) |

‘stroke-dasharray’ controls the pattern of dashes and gaps used to stroke paths. <dasharray> contains a list of comma and/or white space separated [<length>](types.md#DataTypeLength)s and [<percentage>](types.md#DataTypePercentage)s that specify the lengths of alternating dashes and gaps. If an odd number of values is provided, then the list of values is repeated to yield an even number of values. Thus, stroke-dasharray: 5,3,2 is equivalent to stroke-dasharray: 5,3,2,5,3,2.

**none**
: Indicates that no dashing is used. If stroked, the line is drawn solid.
**<dasharray>**
:

A list of comma and/or white space separated [<length>](types.md#DataTypeLength)s (which can have a [unit identifier](coords.md#Units)) and [<percentage>](types.md#DataTypePercentage)s. A percentage represents a distance as a percentage of the current viewport (see [Units](coords.md#Units)). A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). If the sum of the values is zero, then the stroke is rendered as if a value of none were specified. For further details see the [path implementation notes](implnote.md#PathElementImplementationNotes).

The grammar for <dasharray> is as follows:

```
<a id="DashArray"></a>dasharray ::= (length | percentage) (comma-wsp dasharray)?
```

**<a id="StrokeDashoffsetProperty"></a>‘stroke-dashoffset’**
:

| *Value:* | [<percentage>](types.md#DataTypePercentage) \| [<length>](types.md#DataTypeLength) \| inherit |
| --- | --- |
| *Initial:* | 0 |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | see prose |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

[‘stroke-dashoffset’](#StrokeDashoffsetProperty) specifies the distance into the dash pattern to start the dash.

If a [<percentage>](types.md#DataTypePercentage) is used, the value represents a percentage of the current viewport (see [Units](coords.md#Units)).

Values can be negative.

**<a id="StrokeOpacityProperty"></a>‘stroke-opacity’**
:

| *Value:* | <opacity-value> \| inherit |
| --- | --- |
| *Initial:* | 1 |
| *Applies to:* | [shapes](intro.md#TermShape) and [text content elements](intro.md#TermTextContentElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

[‘stroke-opacity’](#StrokeOpacityProperty) specifies the opacity of the painting operation used to stroke the current object. (See [Painting shapes and text](render.md#PaintingShapesAndText).)

**<opacity-value>**
: The opacity of the painting operation used to stroke the current object, as a [<number>](types.md#DataTypeNumber). Any values outside the range 0.0 (fully transparent) to 1.0 (fully opaque) will be clamped to this range. (See [Clamping values which are restricted to a particular range](implnote.md#RangeClamping).)
Related properties: [‘fill-opacity’](#FillOpacityProperty) and [‘opacity’](masking.md#OpacityProperty).

## <a id="VisibilityControl"></a>11.5 Controlling visibility

SVG uses two properties, [‘display’](#DisplayProperty) and [‘visibility’](#VisibilityProperty), to control the visibility of graphical elements or (in the case of the [‘display’](#DisplayProperty) property) container elements.

The differences between the two properties are as follows:

- When applied to a container element, setting [‘display’](#DisplayProperty) to none causes the container and all of its children to be invisible; thus, it acts on groups of elements as a group. [‘visibility’](#VisibilityProperty), however, only applies to individual graphics elements. Setting [‘visibility’](#VisibilityProperty) to hidden on a [‘g’](struct.md#GElement) will make its children invisible as long as the children do not specify their own [‘visibility’](#VisibilityProperty) properties as visible. Note that [‘visibility’](#VisibilityProperty) is *not* an inheritable property.
- When the [‘display’](#DisplayProperty) property is set to none, then the given element does not become part of the rendering tree. With [‘visibility’](#VisibilityProperty) set to hidden, however, processing occurs as if the element were part of the rendering tree and still taking up space, but not actually rendered onto the canvas. This distinction has implications for the [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement) and [‘altGlyph’](text.md#AltGlyphElement) elements, [event processing](interact.md#UIEvents), for [bounding box calculations](coords.md#ObjectBoundingBox) and for calculation of [clipping paths](masking.md#ClippingPaths). If [‘display’](#DisplayProperty) is set to none on a [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement) or [‘altGlyph’](text.md#AltGlyphElement) element, then the text string is ignored for the purposes of text layout; however, if [‘visibility’](#VisibilityProperty) is set to hidden, the text string is used for text layout (i.e., it takes up space) even though it is not rendered on the canvas. Regarding events, if [‘display’](#DisplayProperty) is set to none, the element receives no events; however, if [‘visibility’](#VisibilityProperty) is set to hidden, the element might still receive events, depending on the value of property [‘pointer-events’](interact.md#PointerEventsProperty). The geometry of a graphics element with [‘display’](#DisplayProperty) set to none is not included in [bounding box](coords.md#ObjectBoundingBox) and [clipping paths](masking.md#ClippingPaths) calculations; however, even if [‘visibility’](#VisibilityProperty) is to hidden, the geometry of the graphics element still contributes to bounding box and clipping path calculations.

**<a id="DisplayProperty"></a>‘display’**
:

| *Value:* | inline \| block \| list-item \| run-in \| compact \| marker \| table \| inline-table \| table-row-group \| table-header-group \| table-footer-group \| table-row \| table-column-group \| table-column \| table-cell \| table-caption \| none \| inherit |
| --- | --- |
| *Initial:* | inline |
| *Applies to:* | [‘svg’](struct.md#SVGElement), [‘g’](struct.md#GElement), [‘switch’](struct.md#SwitchElement), [‘a’](linking.md#AElement), [‘foreignObject’](extend.md#ForeignObjectElement), graphics elements (including the [‘text’](text.md#TextElement) element) and text sub-elements (i.e., [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement), [‘altGlyph’](text.md#AltGlyphElement), [‘textPath’](text.md#TextPathElement)) |
| *Inherited:* | no |
| *Percentages:* | N/A |
| *Media:* | all |
| *[Animatable](animate.md#Animatable):* | yes |

A value of display: none indicates that the given element and its children shall not be rendered directly (i.e., those elements are not present in the rendering tree). Any value other than none or inherit indicates that the given element shall be rendered by the SVG user agent.

The [‘display’](#DisplayProperty) property only affects the direct rendering of a given element, whereas it does not prevent elements from being referenced by other elements. For example, setting display: none on a [‘path’](paths.md#PathElement) element will prevent that element from getting rendered directly onto the canvas, but the [‘path’](paths.md#PathElement) element can still be referenced by a [‘textPath’](text.md#TextPathElement) element; furthermore, its geometry will be used in text-on-a-path processing even if the [‘path’](paths.md#PathElement) has display: none.

The [‘display’](#DisplayProperty) property affects direct rendering into offscreen canvases also, such as occurs with the implementation model for [masks](masking.md#Masking). Thus, setting display: none on a child of a [‘mask’](masking.md#MaskElement) will prevent the given child element from being rendered as part of the mask. Similarly, setting display: none on a child of a [‘clipPath’](masking.md#ClipPathElement) element will prevent the given child element from contributing to the clipping path.

Elements with display: none do not take up space in text layout operations, do not receive events, and do not contribute to [bounding box](coords.md#ObjectBoundingBox) and [clipping paths](masking.md#ClippingPaths) calculations.

Except for any additional information provided in this specification, the normative definition of the [‘display’](#DisplayProperty) property is the CSS2 definition ([[CSS2](refs.md#ref-CSS2)], section 9.2.6).

**<a id="VisibilityProperty"></a>‘visibility’**
:

| *Value:* | visible \| hidden \| collapse \| inherit |
| --- | --- |
| *Initial:* | visible |
| *Applies to:* | graphics elements (including the [‘text’](text.md#TextElement) element) and text sub-elements (i.e., [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement), [‘altGlyph’](text.md#AltGlyphElement), [‘textPath’](text.md#TextPathElement) and [‘a’](linking.md#AElement)) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**visible**
: The current graphics element is visible.
**hidden or collapse**
: The current graphics element is invisible (i.e., nothing is painted on the canvas).
Note that if the [‘visibility’](#VisibilityProperty) property is set to hidden on a [‘tspan’](text.md#TSpanElement), [‘tref’](text.md#TRefElement) or [‘altGlyph’](text.md#AltGlyphElement) element, then the text is invisible but still takes up space in text layout calculations.

Depending on the value of property [‘pointer-events’](interact.md#PointerEventsProperty), graphics elements which have their [‘visibility’](#VisibilityProperty) property set to hidden still might receive events.

Except for any additional information provided in this specification, the normative definition of the [‘visibility’](#VisibilityProperty) property is the CSS2 definition ([[CSS2](refs.md#ref-CSS2)], section 11.2).

## <a id="Markers"></a>11.6 Markers

### <a id="MarkersIntroduction"></a>11.6.1 Introduction

A marker is a symbol which is attached to one or more vertices of [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) and [‘polygon’](shapes.md#PolygonElement) elements. Typically, markers are used to make arrowheads or polymarkers. Arrowheads can be defined by attaching a marker to the start or end vertices of [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement) or [‘polyline’](shapes.md#PolylineElement) elements. Polymarkers can be defined by attaching a marker to all vertices of a [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) or [‘polygon’](shapes.md#PolygonElement) element.

The graphics for a marker are defined by a [‘marker’](#MarkerElement) element. To indicate that a particular [‘marker’](#MarkerElement) element should be rendered at the vertices of a particular [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) or [‘polygon’](shapes.md#PolygonElement) element, set one or more marker properties ([‘marker’](#MarkerProperty), [‘marker-start’](#MarkerStartProperty), [‘marker-mid’](#MarkerMidProperty) or [‘marker-end’](#MarkerEndProperty)) to reference the given [‘marker’](#MarkerElement) element.

Example Marker draws a triangular marker symbol as an arrowhead at the end of a path.

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="4in" height="2in" 
     viewBox="0 0 4000 2000" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <marker id="Triangle"
      viewBox="0 0 10 10" refX="0" refY="5" 
      markerUnits="strokeWidth"
      markerWidth="4" markerHeight="3"
      orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" />
    </marker>
  </defs>
  <rect x="10" y="10" width="3980" height="1980"
       fill="none" stroke="blue" stroke-width="10" />
  <desc>Placing an arrowhead at the end of a path.
  </desc>
  <path d="M 1000 750 L 2000 750 L 2500 1250"
        fill="none" stroke="black" stroke-width="100" 
        marker-end="url(#Triangle)"  />
</svg>
```

Example Marker

| [image: Example Marker — Triangular marker at the end of a path] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/marker.svg)

Markers can be animated. The animated effects will show on all current uses of the markers within the document.

### <a id="MarkerElement"></a>11.6.2 The ‘marker’ element

The [‘marker’](#MarkerElement) element defines the graphics that is to be used for drawing arrowheads or polymarkers on a given [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) or [‘polygon’](shapes.md#PolygonElement) element.

‘marker’

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
- [‘font’](fonts.md#FontElement)
- [‘font-face’](fonts.md#FontFaceElement)
- [‘foreignObject’](extend.md#ForeignObjectElement)
- [‘image’](struct.md#ImageElement)
- [‘marker’](#MarkerElement)
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
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](#ColorInterpolationProperty), [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](#FillProperty), [‘fill-opacity’](#FillOpacityProperty), [‘fill-rule’](#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](#MarkerEndProperty), [‘marker-mid’](#MarkerMidProperty), [‘marker-start’](#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](#StrokeProperty), [‘stroke-dasharray’](#StrokeDasharrayProperty), [‘stroke-dashoffset’](#StrokeDashoffsetProperty), [‘stroke-linecap’](#StrokeLinecapProperty), [‘stroke-linejoin’](#StrokeLinejoinProperty), [‘stroke-miterlimit’](#StrokeMiterlimitProperty), [‘stroke-opacity’](#StrokeOpacityProperty), [‘stroke-width’](#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘viewBox’](coords.md#ViewBoxAttribute)
- [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute)
- [‘refX’](#MarkerElementRefXAttribute)
- [‘refY’](#MarkerElementRefYAttribute)
- [‘markerUnits’](#MarkerUnitsAttribute)
- [‘markerWidth’](#MarkerWidthAttribute)
- [‘markerHeight’](#MarkerHeightAttribute)
- [‘orient’](#OrientAttribute)

**DOM Interfaces:**
:

- [SVGMarkerElement](#InterfaceSVGMarkerElement)

*Attribute definitions:*

**<a id="MarkerUnitsAttribute"></a>markerUnits = "strokeWidth | userSpaceOnUse"**
: Defines the coordinate system for attributes [‘markerWidth’](#MarkerWidthAttribute), [‘markerHeight’](#MarkerHeightAttribute) and the contents of the [‘marker’](#MarkerElement). If markerUnits="strokeWidth", [‘markerWidth’](#MarkerWidthAttribute), [‘markerHeight’](#MarkerHeightAttribute) and the contents of the [‘marker’](#MarkerElement) represent values in a coordinate system which has a single unit equal the size in user units of the current stroke width (see the [‘stroke-width’](#StrokeWidthProperty) property) in place for the graphic object referencing the marker. If markerUnits="userSpaceOnUse", [‘markerWidth’](#MarkerWidthAttribute), [‘markerHeight’](#MarkerHeightAttribute) and the contents of the [‘marker’](#MarkerElement) represent values in the current user coordinate system in place for the graphic object referencing the marker (i.e., the user coordinate system for the element referencing the [‘marker’](#MarkerElement) element via a [‘marker’](#MarkerProperty), [‘marker-start’](#MarkerStartProperty), [‘marker-mid’](#MarkerMidProperty) or [‘marker-end’](#MarkerEndProperty) property). If attribute [‘markerUnits’](#MarkerUnitsAttribute) is not specified, then the effect is as if a value of 'strokeWidth' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MarkerElementRefXAttribute"></a>refX = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of the reference point which is to be aligned exactly at the marker position. The coordinate is defined in the coordinate system after application of the [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attributes. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MarkerElementRefYAttribute"></a>refY = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of the reference point which is to be aligned exactly at the marker position. The coordinate is defined in the coordinate system after application of the [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attributes. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MarkerWidthAttribute"></a>markerWidth = "[<length>](types.md#DataTypeLength)"**
: Represents the width of the viewport into which the marker is to be fitted when it is rendered. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. If the attribute is not specified, the effect is as if a value of "3" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="MarkerHeightAttribute"></a>markerHeight = "[<length>](types.md#DataTypeLength)"**
: Represents the height of the viewport into which the marker is to be fitted when it is rendered. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. If the attribute is not specified, the effect is as if a value of "3" were specified. [Animatable](animate.md#Animatable): yes.
**<a id="OrientAttribute"></a>orient = "auto | [<angle>](types.md#DataTypeAngle)"**
: Indicates how the marker is rotated. A value of 'auto' indicates that the marker is oriented such that its positive x-axis is pointing as follows:

1. If there is a path segment coming into the vertex and another path segment going out of the vertex, the marker's positive x-axis should point toward the angle bisector for the angle at the given vertex, where that angle has one side consisting of tangent vector for the path segment going into the vertex and the other side the tangent vector for the path segment going out of the vertex. Note:
  - If the tangent vectors are the same, the angle bisector equals the two tangent vectors.
  - If an incoming and an outgoing vertex produce a zero vector the direction of marker is undefined.
2. If there is only a path segment going into the vertex (e.g., the last vertex on an open path), the marker's positive x-axis should point in the same direction as the tangent vector for the path segment going into the vertex.
3. If there is only a path segment going out of the vertex (e.g., the first vertex on an open path), the marker's positive x-axis should point in the same direction as the tangent vector for the path segment going out of the vertex. (Refer to [‘path’ element implementation notes](implnote.md#PathElementImplementationNotes) for a more thorough discussion of the directionality of path segments.)

In all cases for closed subpaths (e.g., subpaths which end with a ['closepath'](paths.md#PathDataClosePathCommand) command), the orientation of the marker corresponding to the initial point of the subpath is calculated assuming that:

- the path segment going into the vertex is the path segment corresponding to the closepath
- the path segment coming out of the vertex is the first path segment in the subpath

When a ['closepath'](paths.md#PathDataClosePathCommand) command is followed by a command other than a ['moveto'](paths.md#PathDataMovetoCommands) command, then the orientation of the marker corresponding to the ['closepath'](paths.md#PathDataClosePathCommand) command is calculated assuming that:

- the path segment going into the vertex is the path segment corresponding to the closepath
- the path segment coming out of the vertex is the first path segment of the subsequent subpath

A [<angle>](types.md#DataTypeAngle) value represents a particular orientation in the user space of the graphic object referencing the marker. For example, if a value of "0" is given, then the marker will be drawn such that its x-axis will align with the x-axis of the user space of the graphic object referencing the marker. If the attribute is not specified, the effect is as if a value of "0" were specified. [Animatable](animate.md#Animatable): yes (non-additive).
Markers are drawn such that their reference point (i.e., attributes [‘refX’](#MarkerElementRefXAttribute) and [‘refY’](#MarkerElementRefYAttribute)) is positioned at the given vertex. In other words, a translation transformation is constructed by the user agent to achieve the effect of having point ([‘refX’](#MarkerElementRefXAttribute) and [‘refY’](#MarkerElementRefYAttribute)) within the marker content's coordinate system (after any transformations due to the [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute) attributes) align exactly with the given vertex.

SVG's [user agent style sheet](styling.md#UAStyleSheet) sets the [‘overflow’](masking.md#OverflowProperty) property for [‘marker’](#MarkerElement) elements to hidden, which causes a rectangular clipping path to be created at the bounds of the marker tile. Unless the [‘overflow’](masking.md#OverflowProperty) property is overridden, any graphics within the marker which goes outside of the marker rectangle will be clipped.

The contents of the [‘marker’](#MarkerElement) are relative to a new coordinate system. Attribute [‘markerUnits’](#MarkerUnitsAttribute) determines an initial scale factor for transforming the graphics in the marker into the user coordinate system for the referencing element. An additional set of transformations might occur if there is a [‘viewBox’](coords.md#ViewBoxAttribute) attribute, in which case the coordinate system for the contents of the [‘marker’](#MarkerElement) will be transformed due to the processing of attributes [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute). If there is no [‘viewBox’](coords.md#ViewBoxAttribute) attribute, then the assumed default value for the the [‘viewBox’](coords.md#ViewBoxAttribute) attribute has the origin of the viewBox coincident with the origin of the viewport and the width/height of the viewBox the same as the width/height of the viewport.

[Properties](styling.md#SVGStylingProperties) inherit into the [‘marker’](#MarkerElement) element from its ancestors; properties do *not* inherit from the element referencing the [‘marker’](#MarkerElement) element.

[‘marker’](#MarkerElement) elements are never rendered directly; their only usage is as something that can be referenced using the [‘marker’](#MarkerProperty), [‘marker-start’](#MarkerStartProperty), [‘marker-end’](#MarkerEndProperty) and [‘marker-mid’](#MarkerMidProperty) properties. The [‘display’](#DisplayProperty) property does not apply to the [‘marker’](#MarkerElement) element; thus, [‘marker’](#MarkerElement) elements are not directly rendered even if the [‘display’](#DisplayProperty) property is set to a value other than none, and [‘marker’](#MarkerElement) elements are available for referencing even when the [‘display’](#DisplayProperty) property on the [‘marker’](#MarkerElement) element or any of its ancestors is set to none.

[Event attributes](https://www.w3.org/TR/SVG11/script.html#EventAttributes) and [event listeners](https://www.w3.org/TR/SVG11/svgdom.html#EventListeners) attached to the contents of a [‘marker’](#MarkerElement) element are not processed; only the rendering aspects of [‘marker’](#MarkerElement) elements are processed.

### <a id="MarkerProperties"></a>11.6.3 Marker properties

[‘marker-start’](#MarkerStartProperty) defines the arrowhead or polymarker that shall be drawn at the first vertex of the given [‘path’](paths.md#PathElement) element or [basic shape](shapes.md). [‘marker-end’](#MarkerEndProperty) defines the arrowhead or polymarker that shall be drawn at the final vertex. [‘marker-mid’](#MarkerMidProperty) defines the arrowhead or polymarker that shall be drawn at every other vertex (i.e., every vertex except the first and last). Note that for a [‘path’](paths.md#PathElement) element which ends with a closed sub-path, the last vertex is the same as the initial vertex on the given sub-path. In this case, if [‘marker-end’](#MarkerEndProperty) does not equal none, then it is possible that two markers will be rendered on the given vertex. One way to prevent this is to set [‘marker-end’](#MarkerEndProperty) to none. (Note that the same comment applies to [‘polygon’](shapes.md#PolygonElement) elements.)

**<a id="MarkerStartProperty"></a>‘marker-start’**
**<a id="MarkerMidProperty"></a>‘marker-mid’**
**<a id="MarkerEndProperty"></a>‘marker-end’**
:

| *Value:* | none \| <funciri> \| inherit |
| --- | --- |
| *Initial:* | none |
| *Applies to:* | [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) and [‘polygon’](shapes.md#PolygonElement) elements |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**none**
: Indicates that no marker symbol shall be drawn at the given vertex (vertices).
**<funciri>**
: The <funciri> is a [Functional IRI reference](types.md#DataTypeFuncIRI) to the [‘marker’](#MarkerElement) element which shall be used as the arrowhead symbol or polymarker at the given vertex or vertices. If the [IRI reference](linking.md#IRIReference) is not valid (e.g., it points to an object that is undefined or the object is not a [‘marker’](#MarkerElement) element), then the marker(s) shall not be drawn.
<a id="MarkerProperty"></a>The [‘marker’](#MarkerProperty) property specifies the marker symbol that shall be used for all points on the sets the value for all vertices on the given [‘path’](paths.md#PathElement) element or [basic shape](shapes.md). It is a short-hand for the three individual marker properties:

**‘marker’**
:

| *Value:* | see individual properties |
| --- | --- |
| *Initial:* | see individual properties |
| *Applies to:* | [‘path’](paths.md#PathElement), [‘line’](shapes.md#LineElement), [‘polyline’](shapes.md#PolylineElement) and [‘polygon’](shapes.md#PolygonElement) elements |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

### <a id="MarkerAlgorithm"></a>11.6.4 Details on how markers are rendered

Markers are drawn after the given object is filled and stroked.

For each marker that is drawn, a temporary new user coordinate system is established so that the marker will be positioned and sized correctly, as follows:

- The axes of the temporary new user coordinate system are aligned according to the [‘orient’](#OrientAttribute) attribute on the [‘marker’](#MarkerElement) element and the slope of the curve at the given vertex. (Note: if there is a discontinuity at a vertex, the slope is the average of the slopes of the two segments of the curve that join at the given vertex. If a slope cannot be determined, the slope is assumed to be zero.)
- A temporary new coordinate system is established by attribute [‘markerUnits’](#MarkerUnitsAttribute). If [‘markerUnits’](#MarkerUnitsAttribute) equals 'strokeWidth', then the temporary new user coordinate system is the result of scaling the current user coordinate system by the current value of property [‘stroke-width’](#StrokeWidthProperty). If [‘markerUnits’](#MarkerUnitsAttribute) equals 'userSpaceOnUse', then no extra scale transformation is applied.
- An additional set of transformations might occur if the [‘marker’](#MarkerElement) element includes a [‘viewBox’](coords.md#ViewBoxAttribute) attribute, in which case additional transformations are set up to produce the necessary result due to attributes [‘viewBox’](coords.md#ViewBoxAttribute) and [‘preserveAspectRatio’](coords.md#PreserveAspectRatioAttribute).
- If the [‘overflow’](masking.md#OverflowProperty) property on the [‘marker’](#MarkerElement) element indicates that the marker needs to be clipped to its viewport, then an implicit clipping path is established at the bounds of the viewport.

The rendering effect of a marker is as if the contents of the referenced [‘marker’](#MarkerElement) element were deeply cloned into a separate non-exposed DOM tree for each instance of the marker. Because the cloned DOM tree is non-exposed, the SVG DOM does not show the cloned instance of the marker.

For user agents that support [Styling with CSS](styling.md#StylingWithCSS), the conceptual deep cloning of the referenced [‘marker’](#MarkerElement) element into a non-exposed DOM tree also copies any property values resulting from the CSS cascade ([[CSS2](refs.md#ref-CSS2)], chapter 6) and property inheritance on the referenced element and its contents. CSS2 selectors can be applied to the original (i.e., referenced) elements because they are part of the formal document structure. CSS2 selectors cannot be applied to the (conceptually) cloned DOM tree because its contents are not part of the formal document structure.

For illustrative purposes, we'll repeat the marker example shown earlier:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="4in" height="2in" 
     viewBox="0 0 4000 2000" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <defs>
    <marker id="Triangle"
      viewBox="0 0 10 10" refX="0" refY="5" 
      markerUnits="strokeWidth"
      markerWidth="4" markerHeight="3"
      orient="auto">
      <path d="M 0 0 L 10 5 L 0 10 z" />
    </marker>
  </defs>
  <rect x="10" y="10" width="3980" height="1980"
       fill="none" stroke="blue" stroke-width="10" />
  <desc>Placing an arrowhead at the end of a path.
  </desc>
  <path d="M 1000 750 L 2000 750 L 2500 1250"
        fill="none" stroke="black" stroke-width="100" 
        marker-end="url(#Triangle)"  />
</svg>
```

The rendering effect of the above file will be visually identical to the following:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="4in" height="2in" 
     viewBox="0 0 4000 2000" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>File which produces the same effect
      as the marker example file, but without
      using markers.
  </desc>
  <rect x="10" y="10" width="3980" height="1980"
       fill="none" stroke="blue" stroke-width="10" />
  <!-- The path draws as before, but without the marker properties -->
  <path d="M 1000 750 L 2000 750 L 2500 1250"
        fill="none" stroke="black" stroke-width="100"  />
  <!-- The following logic simulates drawing a marker 
       at final vertex of the path. -->
  <!-- First off, move the origin of the user coordinate system
       so that the origin is now aligned with the end point of the path. -->
  <g transform="translate(2500,1250)" >
    <!-- Rotate the coordinate system 45 degrees because
         the marker specified orient="auto" and the final segment
         of the path is going in the direction of 45 degrees. -->
    <g transform="rotate(45)" >
      <!-- Scale the coordinate system to match the coordinate system
           indicated by the 'markerUnits' attributes, which in this case has
           a value of 'strokeWidth'. Therefore, scale the coordinate system
           by the current value of the 'stroke-width' property, which is 100. -->
      <g transform="scale(100)" >
        <!-- Translate the coordinate system by 
             (-refX*viewBoxToMarkerUnitsScaleX, -refY*viewBoxToMarkerUnitsScaleY)
             in order that (refX,refY) within the marker will align with the vertex.
             In this case, we use the default value for preserveAspectRatio
             ('xMidYMid meet'), which means find a uniform scale factor
             (i.e., viewBoxToMarkerUnitsScaleX=viewBoxToMarkerUnitsScaleY)
             such that the viewBox fits entirely within the viewport ('meet') and 
             is center-aligned ('xMidYMid'). In this case, the uniform scale factor
             is markerHeight/viewBoxHeight=3/10=.3. Therefore, translate by
             (-refX*.3,-refY*.3)=(0*.3,-5*.3)=(0,-1.5). -->
        <g transform="translate(0,-1.5)" >
          <!-- There is an implicit clipping path because the user agent style
               sheet says that the 'overflow' property for markers has the value
               'hidden'. To achieve this, create a clipping path at the bounds
               of the viewport. Note that in this case the viewport extends
               0.5 units to the left and right of the viewBox due to 
               a uniform scale factor, different ratios for markerWidth/viewBoxWidth
               and markerHeight/viewBoxHeight, and 'xMidYMid' alignment -->
          <clipPath id="cp1" >
            <rect x="-0.5" y="0" width="4" height="3" />
          </clipPath>
          <g clip-path="url(#cp1)" >
            <!-- Scale the coordinate system by the uniform scale factor
                 markerHeight/viewBoxHeight=3/10=.3 to set the coordinate
                 system to viewBox units. -->
            <g transform="scale(.3)" >
              <!-- This 'g' element carries all property values that result from
                   cascading and inheritance of properties on the original 'marker' element.
                   In this example, neither fill nor stroke was specified on the 'marker'
                   element or any ancestors of the 'marker', so the initial values of
                   "black" and "none" are used, respectively. -->
             <g fill="black" stroke="none" >
                <!-- Expand out the contents of the 'marker' element. -->
                <path d="M 0 0 L 10 5 L 0 10 z" />
              </g>
            </g>
          </g>
        </g>
      </g>
    </g>
  </g>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/marker-simulated.svg)

## <a id="RenderingProperties"></a>11.7 Rendering properties

### <a id="ColorInterpolationProperties"></a>11.7.1 Color interpolation properties: ‘color-interpolation’ and ‘color-interpolation-filters’

The SVG user agent performs color interpolations and compositing at various points as it processes SVG content. Two properties, [‘color-interpolation’](#ColorInterpolationProperty) and [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty), control which color space is used for particular categories of graphics operations. The following table shows which property applies to which graphics operations:

| Graphics operation | Corresponding property |
| --- | --- |
| interpolating between gradient stops (see [Gradient](pservers.md#Gradients)) | [‘color-interpolation’](#ColorInterpolationProperty) |
| interpolating color when performing color animations with either [‘animate’](animate.md#AnimateElement) or [‘animateColor’](animate.md#AnimateColorElement) | [‘color-interpolation’](#ColorInterpolationProperty) |
| [alpha compositing](masking.md#SimpleAlphaBlending) of [graphics elements](intro.md#TermGraphicsElement) into the current background | [‘color-interpolation’](#ColorInterpolationProperty) |
| [filter effects](filters.md) | [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty) |

Both properties choose between color operations occurring in the sRGB color space or in a (light energy linear) linearized RGB color space. Having chosen the appropriate color space, component-wise linear interpolation is used.

<a id="sRGBlinearRGBConversionFormulas"></a>The conversion formulas between the sRGB color space (i.e., nonlinear with 2.2 gamma curve) and the linearized RGB color space (i.e., color values expressed as sRGB tristimulus values without a gamma curve) can be found in the sRGB specification [[SRGB](refs.md#ref-SRGB)]. For illustrative purposes, the following formula shows the conversion from sRGB to linearized RGB:

```
  R[sRGB] = R[sRGB-8bit] / 255
  G[sRGB] = G[sRGB-8bit] / 255
  B[sRGB] = B[sRGB-8bit] / 255
If R[sRGB], G[sRGB], B[sRGB] <= 0.04045
  R[linearRGB] = R[sRGB] / 12.92
  G[linearRGB] = G[sRGB] / 12.92
  B[linearRGB] = B[sRGB] / 12.92
else if R[sRGB], G[sRGB], B[sRGB] > 0.04045
  R[linearRGB] = ((R[sRGB] + 0.055) / 1.055) ^ 2.4
  G[linearRGB] = ((G[sRGB] + 0.055) / 1.055) ^ 2.4
  B[linearRGB] = ((B[sRGB] + 0.055) / 1.055) ^ 2.4
  R[linearRGB-8bit] = R[linearRGB] * 255
  G[linearRGB-8bit] = G[linearRGB] * 255
  B[linearRGB-8bit] = B[linearRGB] * 255
```

Out-of-range color values, if supported by the user agent, also are converted using the above formulas. (See [Clamping values which are restricted to a particular range](implnote.md#RangeClamping).)

**<a id="ColorInterpolationProperty"></a>‘color-interpolation’**
:

| *Value:* | auto \| sRGB \| linearRGB \| inherit |
| --- | --- |
| *Initial:* | sRGB |
| *Applies to:* | [container elements](intro.md#TermContainerElement), [graphics elements](intro.md#TermGraphicsElement), [‘animate’](animate.md#AnimateElement) and [‘animateColor’](animate.md#AnimateColorElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: Indicates that the user agent can choose either the sRGB or linearRGB spaces for color interpolation. This option indicates that the author doesn't require that color interpolation occur in a particular color space.
**sRGB**
: Indicates that color interpolation should occur in the sRGB color space.
**linearRGB**
: Indicates that color interpolation should occur in the linearized RGB color space as described above.
The [‘color-interpolation’](#ColorInterpolationProperty) property specifies the color space for gradient interpolations, color animations and alpha compositing.

When a child element is blended into a background, the value of the [‘color-interpolation’](#ColorInterpolationProperty) property on the child determines the type of blending, not the value of the [‘color-interpolation’](#ColorInterpolationProperty) on the parent. For [gradients](pservers.md#Gradients) which make use of the ‘xlink:href’ attribute to reference another gradient, the gradient uses the [‘color-interpolation’](#ColorInterpolationProperty) property value from the gradient element which is directly referenced by the [‘fill’](#FillProperty) or [‘stroke’](#StrokeProperty) property. When animating colors, color interpolation is performed according to the value of the [‘color-interpolation’](#ColorInterpolationProperty) property on the element being animated.

**<a id="ColorInterpolationFiltersProperty"></a>‘color-interpolation-filters’**
:

| *Value:* | auto \| sRGB \| linearRGB \| inherit |
| --- | --- |
| *Initial:* | linearRGB |
| *Applies to:* | [filter primitives](filters.md#TermFilterPrimitive) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: Indicates that the user agent can choose either the sRGB or linearRGB spaces for filter effects color operations. This option indicates that the author doesn't require that color operations occur in a particular color space.
**sRGB**
: Indicates that filter effects color operations should occur in the sRGB color space.
**linearRGB**
: Indicates that filter effects color operations should occur in the linearized RGB color space.
The [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty) property specifies the color space for imaging operations performed via [filter effects](filters.md).

Note that [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty) has a different initial value than [‘color-interpolation’](#ColorInterpolationProperty). [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty) has an initial value of linearRGB, whereas [‘color-interpolation’](#ColorInterpolationProperty) has an initial value of sRGB. Thus, in the default case, filter effects operations occur in the linearRGB color space, whereas all other color interpolations occur by default in the sRGB color space.

### <a id="ColorRenderingProperty"></a>11.7.2 The ‘color-rendering’ property

The creator of SVG content might want to provide a hint to the implementation about how to make speed vs. quality tradeoffs as it performs color interpolation and compositing. The [‘color-rendering’](#ColorRenderingProperty) property provides a hint to the SVG user agent about how to optimize its color interpolation and compositing operations.

[‘color-rendering’](#ColorRenderingProperty) takes precedence over [‘color-interpolation-filters’](#ColorInterpolationFiltersProperty). For example, assume color-rendering: optimizeSpeed and color-interpolation-filters: linearRGB. In this case, the SVG user agent should perform color operations in a way that optimizes performance, which might mean sacrificing the color interpolation precision as specified by color-interpolation-filters: linearRGB.

**‘color-rendering’**
:

| *Value:* | auto \| optimizeSpeed \| optimizeQuality \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [container elements](intro.md#TermContainerElement), [graphics elements](intro.md#TermGraphicsElement), [‘animate’](animate.md#AnimateElement) and [‘animateColor’](animate.md#AnimateColorElement) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: Indicates that the user agent shall make appropriate tradeoffs to balance speed and quality, but quality shall be given more importance than speed.
**optimizeSpeed**
: Indicates that the user agent shall emphasize rendering speed over quality. For RGB display devices, this option will sometimes cause the user agent to perform color interpolation and compositing in the device RGB color space.
**optimizeQuality**
: Indicates that the user agent shall emphasize quality over rendering speed.
### <a id="ShapeRenderingProperty"></a>11.7.3 The ‘shape-rendering’ property

The creator of SVG content might want to provide a hint to the implementation about what tradeoffs to make as it renders vector graphics elements such as [‘path’](paths.md#PathElement) elements and [basic shapes](shapes.md) such as circles and rectangles. The [‘shape-rendering’](#ShapeRenderingProperty) property provides these hints.

**‘shape-rendering’**
:

| *Value:* | auto \| optimizeSpeed \| crispEdges \| geometricPrecision \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [shapes](intro.md#TermShape) |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: Indicates that the user agent shall make appropriate tradeoffs to balance speed, crisp edges and geometric precision, but with geometric precision given more importance than speed and crisp edges.
**optimizeSpeed**
: Indicates that the user agent shall emphasize rendering speed over geometric precision and crisp edges. This option will sometimes cause the user agent to turn off shape anti-aliasing.
**crispEdges**
: Indicates that the user agent shall attempt to emphasize the contrast between clean edges of artwork over rendering speed and geometric precision. To achieve crisp edges, the user agent might turn off anti-aliasing for all lines and curves or possibly just for straight lines which are close to vertical or horizontal. Also, the user agent might adjust line positions and line widths to align edges with device pixels.
**geometricPrecision**
: Indicates that the user agent shall emphasize geometric precision over speed and crisp edges.
### <a id="TextRenderingProperty"></a>11.7.4 The ‘text-rendering’ property

The creator of SVG content might want to provide a hint to the implementation about what tradeoffs to make as it renders text. The [‘text-rendering’](#TextRenderingProperty) property provides these hints.

**‘text-rendering’**
:

| *Value:* | auto \| optimizeSpeed \| optimizeLegibility \| geometricPrecision \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [‘text’](text.md#TextElement) elements |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: Indicates that the user agent shall make appropriate tradeoffs to balance speed, legibility and geometric precision, but with legibility given more importance than speed and geometric precision.
**optimizeSpeed**
: Indicates that the user agent shall emphasize rendering speed over legibility and geometric precision. This option will sometimes cause the user agent to turn off text anti-aliasing.
**optimizeLegibility**
: Indicates that the user agent shall emphasize legibility over rendering speed and geometric precision. The user agent will often choose whether to apply anti-aliasing techniques, built-in font hinting or both to produce the most legible text.
**geometricPrecision**
: Indicates that the user agent shall emphasize geometric precision over legibility and rendering speed. This option will usually cause the user agent to suspend the use of hinting so that glyph outlines are drawn with comparable geometric precision to the rendering of path data.
### <a id="ImageRenderingProperty"></a>11.7.5 The ‘image-rendering’ property

The creator of SVG content might want to provide a hint to the implementation about how to make speed vs. quality tradeoffs as it performs image processing. The [‘image-rendering’](#ImageRenderingProperty) property provides a hint to the SVG user agent about how to optimize its image rendering.

**‘image-rendering’**
:

| *Value:* | auto \| optimizeSpeed \| optimizeQuality \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | images |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: Indicates that the user agent shall make appropriate tradeoffs to balance speed and quality, but quality shall be given more importance than speed. The user agent shall employ a resampling algorithm at least as good as nearest neighbor resampling, but bilinear resampling is strongly preferred. For [Conforming High-Quality SVG Viewers](conform.md#ConformingHighQualitySVGViewers), the user agent shall employ a resampling algorithm at least as good as bilinear resampling.
**optimizeQuality**
: Indicates that the user agent shall emphasize quality over rendering speed. The user agent shall employ a resampling algorithm at least as good as bilinear resampling.
**optimizeSpeed**
: Indicates that the user agent shall emphasize rendering speed over quality. The user agent should use a resampling algorithm which achieves the goal of fast rendering, with the requirement that the resampling algorithm shall be at least as good as nearest neighbor resampling. If performance goals can be achieved with higher quality algorithms, then the user agent should use the higher quality algorithms instead of nearest neighbor resampling.
In all cases, resampling must be done in a truecolor (e.g., 24-bit) color space even if the original data and/or the target device is indexed color.

## <a id="InheritanceOfPaintingProperties"></a>11.8 Inheritance of painting properties

The values of any of the painting properties described in this chapter can be inherited from a given object's parent. Painting, however, is always done on each [graphics element](intro.md#TermGraphicsElement) individually, never at the [container element](intro.md#TermContainerElement) (e.g., a [‘g’](struct.md#GElement)) level. Thus, for the following SVG, even though the gradient fill is specified on the [‘g’](struct.md#GElement), the gradient is simply inherited through the [‘g’](struct.md#GElement) element down into each rectangle, each of which is rendered such that its interior is painted with the gradient.

Example Inheritance

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd">
<svg width="7cm" height="2cm" viewBox="0 0 700 200"
     xmlns="http://www.w3.org/2000/svg" version="1.1">
  <desc>Gradients apply to leaf nodes
  </desc>
  <g>
    <defs>
      <linearGradient id="MyGradient" gradientUnits="objectBoundingBox">
        <stop offset="0%" stop-color="#F60" />
        <stop offset="100%" stop-color="#FF6" />
      </linearGradient>
    </defs>
    <rect x="1" y="1" width="698" height="198"
          fill="none" stroke="blue" stroke-width="2" />
    <g fill="url(#MyGradient)" >
      <rect x="100" y="50" width="200" height="100"/>
      <rect x="400" y="50" width="200" height="100"/>
   </g>
  </g>
</svg>
```

Example Inheritance

| [image: Example Inheritance] |
| --- |

[View this example as SVG (SVG-enabled browsers only)](https://www.w3.org/TR/SVG11/images/painting/inheritance.svg)

Any painting properties defined in terms of the [object's bounding box](coords.md#ObjectBoundingBoxUnits) use the bounding box of the [graphics element](intro.md#TermGraphicsElement) to which the operation applies. Note that [text elements](text.md) are defined such that any painting operations defined in terms of the [object's bounding box](coords.md#ObjectBoundingBoxUnits) use the bounding box of the entire [‘text’](text.md#TextElement) element. (See the discussion of [object bounding box units and text elements](text.md#ObjectBoundingBoxUnitsTextObjects).)

## <a id="DOMInterfaces"></a>11.9 DOM interfaces

### <a id="InterfaceSVGPaint"></a>11.9.1 Interface SVGPaint

The [SVGPaint](#InterfaceSVGPaint) interface corresponds to basic type [<paint>](#SpecifyingPaint) and represents the values of properties [‘fill’](#FillProperty) and [‘stroke’](#StrokeProperty).

Note: The [SVGPaint](#InterfaceSVGPaint) interface is deprecated, and may be dropped from future versions of the SVG specification.

```
interface SVGPaint : SVGColor {

  // Paint Types
  const unsigned short SVG_PAINTTYPE_UNKNOWN = 0;
  const unsigned short SVG_PAINTTYPE_RGBCOLOR = 1;
  const unsigned short SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR = 2;
  const unsigned short SVG_PAINTTYPE_NONE = 101;
  const unsigned short SVG_PAINTTYPE_CURRENTCOLOR = 102;
  const unsigned short SVG_PAINTTYPE_URI_NONE = 103;
  const unsigned short SVG_PAINTTYPE_URI_CURRENTCOLOR = 104;
  const unsigned short SVG_PAINTTYPE_URI_RGBCOLOR = 105;
  const unsigned short SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR = 106;
  const unsigned short SVG_PAINTTYPE_URI = 107;

  readonly attribute unsigned short paintType;
  readonly attribute DOMString uri;

  void setUri(in DOMString uri);
  void setPaint(in unsigned short paintType, in DOMString uri, in DOMString rgbColor, in DOMString iccColor) raises(SVGException);
};
```

**Constants in group “Paint Types”:**
:

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_UNKNOWN"></a>**SVG_PAINTTYPE_UNKNOWN** (unsigned short)**
:

The paint type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_RGBCOLOR"></a>**SVG_PAINTTYPE_RGBCOLOR** (unsigned short)**
:

An sRGB color has been specified without an alternative ICC color specification.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR"></a>**SVG_PAINTTYPE_RGBCOLOR_ICCCOLOR** (unsigned short)**
:

An sRGB color has been specified along with an alternative ICC color specification.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_NONE"></a>**SVG_PAINTTYPE_NONE** (unsigned short)**
:

Corresponds to a none value on a [<paint>](#SpecifyingPaint) specification.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_CURRENTCOLOR"></a>**SVG_PAINTTYPE_CURRENTCOLOR** (unsigned short)**
:

Corresponds to a currentColor value on a [<paint>](#SpecifyingPaint) specification.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_URI_NONE"></a>**SVG_PAINTTYPE_URI_NONE** (unsigned short)**
:

A URI has been specified, along with an explicit none as the backup paint method in case the URI is unavailable or invalid.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_URI_CURRENTCOLOR"></a>**SVG_PAINTTYPE_URI_CURRENTCOLOR** (unsigned short)**
:

A URI has been specified, along with an sRGB color as the backup paint method in case the URI is unavailable or invalid.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_URI_RGBCOLOR"></a>**SVG_PAINTTYPE_URI_RGBCOLOR** (unsigned short)**
:

A URI has been specified, along with an sRGB color as the backup paint method in case the URI is unavailable or invalid.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR"></a>**SVG_PAINTTYPE_URI_RGBCOLOR_ICCCOLOR** (unsigned short)**
:

A URI has been specified, along with both an sRGB color and alternate ICC color as the backup paint method in case the URI is unavailable or invalid.

**<a id="__svg__SVGPaint__SVG_PAINTTYPE_URI"></a>**SVG_PAINTTYPE_URI** (unsigned short)**
:

Only a URI has been specified.

**Attributes:**
:

**<a id="__svg__SVGPaint__paintType"></a>**paintType** (readonly unsigned short)**
:

The type of paint, identified by one of the SVG_PAINTTYPE_* constants defined on this interface.

**<a id="__svg__SVGPaint__uri"></a>**uri** (readonly DOMString)**
:

When the [paintType](#__svg__SVGPaint__paintType) specifies a URI, this attribute holds the URI string. When the [paintType](#__svg__SVGPaint__paintType) does not specify a URI, this attribute is null.

**Operations:**
:

**<a id="__svg__SVGPaint__setUri"></a>void **setUri**(in DOMString *uri*)**
:

Sets the [paintType](#__svg__SVGPaint__paintType) to SVG_PAINTTYPE_URI_NONE and sets [uri](#__svg__SVGPaint__uri) to the specified value.

**Parameters**
:

1.
DOMString *uri*
The URI for the desired paint server.

**<a id="__svg__SVGPaint__setPaint"></a>void **setPaint**(in unsigned short *paintType*, in DOMString *uri*, in DOMString *rgbColor*, in DOMString *iccColor*)**
:

Sets the paint as specified by the parameters. If *paintType* requires a URI, then *uri* must be non-null; otherwise, *uri* must be null. If *paintType* requires an RGBColor, then *rgbColor* must be a string that matches [<color>](types.md#DataTypeColor); otherwise, *rgbColor* must be null. If *paintType* requires an [SVGICCColor](types.md#InterfaceSVGICCColor), then *iccColor* must be a string that matches [<icccolor>](types.md#DataTypeICCColor); otherwise, *iccColor* must be null.

**Parameters**
:

1.
unsigned short *paintType*
One of the defined constants for [paintType](#__svg__SVGPaint__paintType).
2.
DOMString *uri*
The URI for the desired paint server, or null.
3.
DOMString *rgbColor*
The specification of an sRGB color, or null.
4.
DOMString *iccColor*
The specification of an ICC color, or null.

**Exceptions**
:

**[SVGException](https://www.w3.org/TR/SVG11/svgdom.html#ExceptionSVGException), code SVG_INVALID_VALUE_ERR**
: Raised if one of the parameters has an invalid value.
### <a id="InterfaceSVGMarkerElement"></a>11.9.2 Interface SVGMarkerElement

The [SVGMarkerElement](#InterfaceSVGMarkerElement) interface corresponds to the [‘marker’](#MarkerElement) element.

```
interface SVGMarkerElement : SVGElement,
                             SVGLangSpace,
                             SVGExternalResourcesRequired,
                             SVGStylable,
                             SVGFitToViewBox {

  // Marker Unit Types
  const unsigned short SVG_MARKERUNITS_UNKNOWN = 0;
  const unsigned short SVG_MARKERUNITS_USERSPACEONUSE = 1;
  const unsigned short SVG_MARKERUNITS_STROKEWIDTH = 2;

  // Marker Orientation Types
  const unsigned short SVG_MARKER_ORIENT_UNKNOWN = 0;
  const unsigned short SVG_MARKER_ORIENT_AUTO = 1;
  const unsigned short SVG_MARKER_ORIENT_ANGLE = 2;

  readonly attribute SVGAnimatedLength refX;
  readonly attribute SVGAnimatedLength refY;
  readonly attribute SVGAnimatedEnumeration markerUnits;
  readonly attribute SVGAnimatedLength markerWidth;
  readonly attribute SVGAnimatedLength markerHeight;
  readonly attribute SVGAnimatedEnumeration orientType;
  readonly attribute SVGAnimatedAngle orientAngle;

  void setOrientToAuto() raises(DOMException);
  void setOrientToAngle(in SVGAngle angle) raises(DOMException);
};
```

**Constants in group “Marker Unit Types”:**
:

**<a id="__svg__SVGMarkerElement__SVG_MARKERUNITS_UNKNOWN"></a>**SVG_MARKERUNITS_UNKNOWN** (unsigned short)**
:

The marker unit type is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGMarkerElement__SVG_MARKERUNITS_USERSPACEONUSE"></a>**SVG_MARKERUNITS_USERSPACEONUSE** (unsigned short)**
:

The value of attribute [‘markerUnits’](#MarkerUnitsAttribute) is 'userSpaceOnUse'.

**<a id="__svg__SVGMarkerElement__SVG_MARKERUNITS_STROKEWIDTH"></a>**SVG_MARKERUNITS_STROKEWIDTH** (unsigned short)**
:

The value of attribute [‘markerUnits’](#MarkerUnitsAttribute) is 'strokeWidth'.

**Constants in group “Marker Orientation Types”:**
:

**<a id="__svg__SVGMarkerElement__SVG_MARKER_ORIENT_UNKNOWN"></a>**SVG_MARKER_ORIENT_UNKNOWN** (unsigned short)**
:

The marker orientation is not one of predefined types. It is invalid to attempt to define a new value of this type or to attempt to switch an existing value to this type.

**<a id="__svg__SVGMarkerElement__SVG_MARKER_ORIENT_AUTO"></a>**SVG_MARKER_ORIENT_AUTO** (unsigned short)**
:

Attribute [‘orient’](#OrientAttribute) has value 'auto'.

**<a id="__svg__SVGMarkerElement__SVG_MARKER_ORIENT_ANGLE"></a>**SVG_MARKER_ORIENT_ANGLE** (unsigned short)**
:

Attribute [‘orient’](#OrientAttribute) has an angle value.

**Attributes:**
:

**<a id="__svg__SVGMarkerElement__refX"></a>**refX** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘refX’](#MarkerElementRefXAttribute) on the given [‘marker’](#MarkerElement) element.

**<a id="__svg__SVGMarkerElement__refY"></a>**refY** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘refY’](#MarkerElementRefYAttribute) on the given [‘marker’](#MarkerElement) element.

**<a id="__svg__SVGMarkerElement__markerUnits"></a>**markerUnits** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘markerUnits’](#MarkerUnitsAttribute) on the given [‘marker’](#MarkerElement) element. One of the Marker Unit Types defined on this interface.

**<a id="__svg__SVGMarkerElement__markerWidth"></a>**markerWidth** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘markerWidth’](#MarkerWidthAttribute) on the given [‘marker’](#MarkerElement) element.

**<a id="__svg__SVGMarkerElement__markerHeight"></a>**markerHeight** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘markerHeight’](#MarkerHeightAttribute) on the given [‘marker’](#MarkerElement) element.

**<a id="__svg__SVGMarkerElement__orientType"></a>**orientType** (readonly [SVGAnimatedEnumeration](types.md#InterfaceSVGAnimatedEnumeration))**
:

Corresponds to attribute [‘orient’](#OrientAttribute) on the given [‘marker’](#MarkerElement) element. One of the Marker Orientation Types defined on this interface.

**<a id="__svg__SVGMarkerElement__orientAngle"></a>**orientAngle** (readonly [SVGAnimatedAngle](types.md#InterfaceSVGAnimatedAngle))**
:

Corresponds to attribute [‘orient’](#OrientAttribute) on the given [‘marker’](#MarkerElement) element. If [markerUnits](#__svg__SVGMarkerElement__markerUnits) is SVG_MARKER_ORIENT_ANGLE, the angle value for attribute [‘orient’](#OrientAttribute); otherwise, it will be set to zero.

**Operations:**
:

**<a id="__svg__SVGMarkerElement__setOrientToAuto"></a>void **setOrientToAuto**()**
:

Sets the value of attribute [‘orient’](#OrientAttribute) to 'auto'.

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGMarkerElement__setOrientToAngle"></a>void **setOrientToAngle**(in [SVGAngle](types.md#InterfaceSVGAngle) *angle*)**
:

Sets the value of attribute [‘orient’](#OrientAttribute) to the given angle.

**Parameters**
:

1.
[SVGAngle](types.md#InterfaceSVGAngle) *angle*
The angle value to use for attribute [‘orient’](#OrientAttribute).

**Exceptions**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](text.md) ⋅ [Next](color.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
