<!-- https://svgwg.org/svg2-draft/geometry.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 7: Geometry Properties

Beside SVG's styling properties, SVG also defines *<a id="geometry-properties"></a>geometry properties*. Geometry properties describe the position and dimension of the [graphics element](struct.md#TermGraphicsElement)s ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[rect](shapes.md#RectElement)’, ‘[image](embedded.md#ImageElement)’, ‘[foreignObject](embedded.md#ForeignObjectElement)’ and the ‘[svg](struct.md#SVGElement)’ element.

## <a id="CX"></a>7.1. Horizontal center coordinate: The ‘cx’ property

| Name: | *<a id="CxProperty"></a>cx* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | 0 |
| Applies to: | ‘[circle](shapes.md#CircleElement)’ and ‘[ellipse](shapes.md#EllipseElement)’ elements |
| Inherited: | no |
| Percentages: | refer to the width of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

The [cx](#CxProperty) property describes the horizontal center coordinate of the position of the element.

## <a id="CY"></a>7.2. Vertical center coordinate: The ‘cy’ property

| Name: | *<a id="CyProperty"></a>cy* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | 0 |
| Applies to: | ‘[circle](shapes.md#CircleElement)’ and ‘[ellipse](shapes.md#EllipseElement)’ elements |
| Inherited: | no |
| Percentages: | refer to the height of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

The [cy](#CyProperty) property describes the vertical center coordinate of the position of the element.

## <a id="R"></a>7.3. Radius: The ‘r’ property

| Name: | *<a id="RProperty"></a>r* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | 0 |
| Applies to: | ‘[circle](shapes.md#CircleElement)’ element |
| Inherited: | no |
| Percentages: | refer to the normalized diagonal of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

The [r](#RProperty) property describes the radius of the ‘[circle](shapes.md#CircleElement)’ element.

A negative value for [r](#RProperty) is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored).

## <a id="RX"></a>7.4. Horizontal radius: The ‘rx’ property

| Name: | *<a id="RxProperty"></a>rx* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| auto |
| Initial: | auto |
| Applies to: | ‘[ellipse](shapes.md#EllipseElement)’, ‘[rect](shapes.md#RectElement)’ elements |
| Inherited: | no |
| Percentages: | refer to the width of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

The [rx](#RxProperty) property describes the horizontal radius of the ‘[ellipse](shapes.md#EllipseElement)’ element and the curve radius of the ‘[rect](shapes.md#RectElement)’ element. When the computed value of ‘rx’ is auto, the used radius is equal to the absolute length used for [ry](#RyProperty), creating a circular arc. If both ‘rx’ and ‘ry’ have a computed value of auto, the used value is 0.

Regardless of how the value is calculated, the used value of ‘rx’ for a ‘[rect](shapes.md#RectElement)’ is never more than 50% of the used value of [width](#Sizing) for the same shape.

The auto behavior is new in SVG 2 for ‘[ellipse](shapes.md#EllipseElement)’, matching the behavior for ‘[rect](shapes.md#RectElement)’ elements when [rx](#RxProperty) was not specified.

A negative value for [rx](#RxProperty) is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored).

## <a id="RY"></a>7.5. Vertical radius: The ‘ry’ property

| Name: | *<a id="RyProperty"></a>ry* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) \| auto |
| Initial: | auto |
| Applies to: | ‘[ellipse](shapes.md#EllipseElement)’, ‘[rect](shapes.md#RectElement)’ |
| Inherited: | no |
| Percentages: | refer to the height of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animatable type: | by computed value |

The [ry](#RyProperty) property describes the vertical radius of the ‘[ellipse](shapes.md#EllipseElement)’ element and the vertical curve radius of the ‘[rect](shapes.md#RectElement)’ element. When the computed value of ‘ry’ is auto, the used radius is equal to the absolute length used for [rx](#RxProperty), creating a circular arc. If both ‘rx’ and ‘ry’ have a computed value of auto, the used value is 0.

Regardless of how the value is calculated, the used value of ‘ry’ for a ‘[rect](shapes.md#RectElement)’ is never more than 50% of the used value of [height](#Sizing) for the same shape.

The auto behavior is new in SVG 2 for ‘[ellipse](shapes.md#EllipseElement)’, matching the behavior for ‘[rect](shapes.md#RectElement)’ elements when [ry](#RyProperty) was not specified.

A negative value for [ry](#RyProperty) is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored).

## <a id="X"></a>7.6. Horizontal coordinate: The ‘x’ property

| Name: | *<a id="XProperty"></a>x* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | 0 |
| Applies to: | ‘[svg](struct.md#SVGElement)’, ‘[rect](shapes.md#RectElement)’, ‘[image](embedded.md#ImageElement)’, ‘[foreignObject](embedded.md#ForeignObjectElement)’ elements |
| Inherited: | no |
| Percentages: | refer to the width of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

The [x](#XProperty) property describes the horizontal coordinate of the position of the element.

## <a id="Y"></a>7.7. Vertical coordinate: The ‘y’ property

| Name: | *<a id="YProperty"></a>y* |
| --- | --- |
| Value: | [<length-percentage>](../css-values-4.md#typedef-length-percentage) |
| Initial: | 0 |
| Applies to: | ‘[svg](struct.md#SVGElement)’, ‘[rect](shapes.md#RectElement)’, ‘[image](embedded.md#ImageElement)’, ‘[foreignObject](embedded.md#ForeignObjectElement)’ elements |
| Inherited: | no |
| Percentages: | refer to the height of the current SVG viewport (see [Units](coords.md#Units)) |
| Media: | visual |
| Computed value: | an absolute length or percentage |
| Animation type: | by computed value |

The [y](#YProperty) property describes the vertical coordinate of the position of the element.

## <a id="Sizing"></a>7.8. Sizing properties: the effect of the ‘width’ and ‘height’ properties

See the CSS 2.1 specification for the definitions of width and height.

The CSS [width](#Sizing) and [height](#Sizing) properties are used for sizing some SVG elements. Specifically, they are used to size ‘[rect](shapes.md#RectElement)’, ‘[svg](struct.md#SVGElement)’, ‘[image](embedded.md#ImageElement)’ and ‘[foreignObject](embedded.md#ForeignObjectElement)’. All of these elements have ‘width’ and ‘height’ presentation attributes. The properties are also used for laying out embedded elements from the HTML namespace.

The used value of [width](#Sizing) may be constrained by the value of the max-width and min-width properties. The used value of [height](#Sizing) may be constrained by the value of the max-height and min-height properties.

The value auto for [width](#Sizing) and [height](#Sizing) on the ‘[svg](struct.md#SVGElement)’ element is treated as 100%.

The value auto for [width](#Sizing) and [height](#Sizing) on the ‘[image](embedded.md#ImageElement)’ element is calculated from the referenced image's intrinsic dimensions and aspect ratio, according to the CSS Default Sizing Algorithm.

New in SVG 2. Images embedded in SVG can now be auto-sized to the intrinsic size, or scaled to a fixed height or width according to the intrinsic aspect ratio. This matches the behavior of embedded images in HTML.

The value auto for [width](#Sizing) and [height](#Sizing) on other elements is treated as 0.

This means that, for example, a ‘[foreignObject](embedded.md#ForeignObjectElement)’ object element will not shrink-wrap to its contents if auto is used.
