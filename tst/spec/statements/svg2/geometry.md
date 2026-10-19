# SVG 2, Chapter 7: Geometry Properties

## Circle, ellipse and rect radius and center: shape-radii, geometry-properties

- [x] svg2/geometry.md#CxProperty: cx has initial value 0, is not inherited, and its percentages refer to the width of the current SVG viewport. [geometry-properties#center-percent geometry-properties#initial-zero]
- [x] svg2/geometry.md#CyProperty: cy has initial value 0, is not inherited, and its percentages refer to the height of the current SVG viewport. [geometry-properties#center-percent geometry-properties#initial-zero]
- [x] svg2/geometry.md#RProperty: r has initial value 0, is not inherited, and its percentages refer to the normalized diagonal of the current SVG viewport. [shape-radii#percent-r geometry-properties#not-inherited]
- [x] svg2/geometry.md#RProperty: a negative value for r is invalid and must be ignored. [shape-radii#zero-negative]
- [x] svg2/geometry.md#RxProperty: rx has initial value auto, is not inherited, and its percentages refer to the width of the current SVG viewport. [shape-radii#percent-radii]
- [x] svg2/geometry.md#RxProperty: when rx's computed value is auto, the used rx equals the absolute length used for ry, giving a circular arc; if both rx and ry are auto, the used value of both is 0. [shape-radii#rx-alone shape-radii#ry-alone]
- [x] svg2/geometry.md#RxProperty: the used value of rx on a rect is never more than 50% of the rect's used width. [shape-radii#rx-alone]
- [x] svg2/geometry.md#RxProperty: a negative value for rx is invalid and must be ignored. [shape-radii#negative-rx]
- [x] svg2/geometry.md#RyProperty: ry has initial value auto, is not inherited, and its percentages refer to the height of the current SVG viewport. [shape-radii#percent-radii]
- [x] svg2/geometry.md#RyProperty: when ry's computed value is auto, the used ry equals the absolute length used for rx, giving a circular arc; if both rx and ry are auto, the used value of both is 0. [shape-radii#ry-alone shape-radii#rx-alone]
- [x] svg2/geometry.md#RyProperty: the used value of ry on a rect is never more than 50% of the rect's used height. [shape-radii#rx-alone]
- [x] svg2/geometry.md#RyProperty: a negative value for ry is invalid and must be ignored. [shape-radii#negative-rx]

## Position of svg, rect and image: viewport-nested, shape-radii, geometry-properties

- [x] svg2/geometry.md#XProperty: x has initial value 0, is not inherited, and its percentages refer to the width of the current SVG viewport. [geometry-properties#position-percent viewport-nested#percent]
- [x] svg2/geometry.md#YProperty: y has initial value 0, is not inherited, and its percentages refer to the height of the current SVG viewport. [geometry-properties#position-percent viewport-nested#percent]

## Auto sizing of svg and image: symbol-geometry, shape-radii, geometry-properties, image-placement

- [x] svg2/geometry.md#Sizing: the value auto for width and height on the svg element is treated as 100%. [geometry-properties#svg-auto symbol-geometry#auto-size]
- [x] svg2/geometry.md#Sizing: the value auto for width and height on the image element is calculated from the referenced image's intrinsic dimensions and aspect ratio, per the CSS default sizing algorithm. [image-placement#natural image-placement#one-dimension]
- [x] svg2/geometry.md#Sizing: the value auto for width and height on any other element is treated as 0. [geometry-properties#other-auto shape-radii#zero-negative]
- [-] svg2/geometry.md#Sizing: the used value of width may be constrained by min-width and max-width, and the used value of height by min-height and max-height. (D17) [geometry-properties#max-width]

## Notes

