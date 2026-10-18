# SVG 2, Chapter 7: Geometry Properties

## Circle, ellipse and rect radius and center: shape-radii

- [x] svg2/geometry.md#CxProperty: cx has initial value 0, is not inherited, and its percentages refer to the width of the current SVG viewport.
- [x] svg2/geometry.md#CyProperty: cy has initial value 0, is not inherited, and its percentages refer to the height of the current SVG viewport.
- [x] svg2/geometry.md#RProperty: r has initial value 0, is not inherited, and its percentages refer to the normalized diagonal of the current SVG viewport.
- [x] svg2/geometry.md#RProperty: a negative value for r is invalid and must be ignored.
- [x] svg2/geometry.md#RxProperty: rx has initial value auto, is not inherited, and its percentages refer to the width of the current SVG viewport.
- [x] svg2/geometry.md#RxProperty: when rx's computed value is auto, the used rx equals the absolute length used for ry, giving a circular arc; if both rx and ry are auto, the used value of both is 0.
- [x] svg2/geometry.md#RxProperty: the used value of rx on a rect is never more than 50% of the rect's used width.
- [x] svg2/geometry.md#RxProperty: a negative value for rx is invalid and must be ignored.
- [x] svg2/geometry.md#RyProperty: ry has initial value auto, is not inherited, and its percentages refer to the height of the current SVG viewport.
- [x] svg2/geometry.md#RyProperty: when ry's computed value is auto, the used ry equals the absolute length used for rx, giving a circular arc; if both rx and ry are auto, the used value of both is 0.
- [x] svg2/geometry.md#RyProperty: the used value of ry on a rect is never more than 50% of the rect's used height.
- [x] svg2/geometry.md#RyProperty: a negative value for ry is invalid and must be ignored.

## Position of svg, rect and image: viewport-nested, shape-radii

- [x] svg2/geometry.md#XProperty: x has initial value 0, is not inherited, and its percentages refer to the width of the current SVG viewport.
- [x] svg2/geometry.md#YProperty: y has initial value 0, is not inherited, and its percentages refer to the height of the current SVG viewport.

## Auto sizing of svg and image: symbol-geometry, shape-radii

- [x] svg2/geometry.md#Sizing: the value auto for width and height on the svg element is treated as 100%.
- [x] svg2/geometry.md#Sizing: the value auto for width and height on the image element is calculated from the referenced image's intrinsic dimensions and aspect ratio, per the CSS default sizing algorithm.
- [x] svg2/geometry.md#Sizing: the value auto for width and height on any other element is treated as 0.
- [-] svg2/geometry.md#Sizing: the used value of width may be constrained by min-width and max-width, and the used value of height by min-height and max-height. (D17)

## Notes

