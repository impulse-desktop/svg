# CSS Transforms Module Level 1

## Transform rendering model and the transform property: transform-functions, measure-transform (exists)

- [x] css-transforms-1.md#transform-rendering: a non-none transform establishes a new local coordinate system at the element, given by its transformation matrix [transform-functions#local]
- [x] css-transforms-1.md#transformation-matrix-computation: the matrix is identity, translated by transform-origin's x/y, multiplied by each transform function left to right, then translated by the negated transform-origin x/y [transform-functions#origin-matrix]
- [x] css-transforms-1.md#transform-rendering: transforms are cumulative; each element establishes its local coordinate system within its parent's [transform-functions#cumulative]
- [x] css-transforms-1.md#current-transformation-matrix-computation: the CTM is computed by post-multiplying all transformation matrices from the viewport coordinate system down to the element's own [transform-functions#ctm]
- [x] css-transforms-1.md#propdef-transform: transform's value is none or a transform-list (one or more transform functions); initial none; applies to transformable elements; not inherited [transform-functions#none transform-functions#not-inherited]
- [x] css-transforms-1.md#propdef-transform: percentages refer to the size of the reference box; the computed value keeps the specified functions but with lengths made absolute [transform-functions#percent]

## transform-origin: transform-functions, transform-origin (exists)

- [x] css-transforms-1.md#propdef-transform-origin: initial 50% 50%; applies to transformable elements; not inherited; percentages refer to the reference box (horizontal to width, vertical to height) [transform-functions#origin-percent transform-functions#origin-initial]
- [x] css-transforms-1.md#transform-origin-property: if only one value is given, the second is assumed center; if one or two values are given, the third (Z) is assumed 0px [transform-functions#origin-one transform-functions#origin-two]
- [x] css-transforms-1.md#transform-origin-property: if two or more values are given and no keyword (or only center) is used, the first value is horizontal and the second vertical; a third value is a Z length offset [transform-functions#origin-two]
- [x] css-transforms-1.md#valdef-transform-origin-top: top computes to 0% vertical, right to 100% horizontal, bottom to 100% vertical, left to 0% horizontal; center computes to 50% of whichever axis is otherwise unspecified [transform-functions#origin-keywords]
- [x] css-transforms-1.md#transform-origin-property: for SVG elements without an associated CSS layout box, the initial used value of transform-origin is 0 0, not 50% 50% [transform-functions#origin-initial]

## transform-box: transform-functions, stroke-bounding-box, transform-box (exists)

- [x] css-transforms-1.md#propdef-transform-box: values content-box | border-box | fill-box | stroke-box | view-box; initial view-box; applies to transformable elements; not inherited; percentages N/A [transform-functions#box-view transform-functions#percent transform-functions#box-stroke]
- [x] css-transforms-1.md#valdef-transform-box-fill-box: fill-box uses the object bounding box as reference box [transform-functions#percent transform-functions#origin-percent]
- [x] css-transforms-1.md#valdef-transform-box-stroke-box: stroke-box uses the stroke bounding box as reference box [transform-functions#box-stroke]
- [x] css-transforms-1.md#valdef-transform-box-view-box: view-box uses the nearest SVG viewport; if that viewport element has a viewBox, the box is positioned at the origin of the coordinate system the viewBox establishes and sized to the viewBox's width and height [transform-functions#box-view]
- [x] css-transforms-1.md#transform-box: for pattern the reference box is given by patternUnits, for linearGradient/radialGradient by gradientUnits, and for clipPath by clipPathUnits [transform-functions#box-gradient transform-paint-servers#box-pattern transform-paint-servers#box-clip-path transform-paint-servers#box-gradient transform-paint-servers#box-origin]
- [x] css-transforms-1.md#transform-box: for SVG elements without a CSS layout box, content-box computes to fill-box and border-box computes to stroke-box; for elements with a CSS layout box, fill-box computes to content-box, and stroke-box and view-box compute to border-box [transform-functions#box-stroke]

## SVG transform attribute: presentation attribute and grammar: transform-functions, transform-paint-servers, transform-attribute-grammar (exists)

- [x] css-transforms-1.md#transform-attribute-specificity: transform-origin is also a presentation attribute; the transform, patternTransform and gradientTransform attributes are presentation attributes represented by the CSS transform property [transform-functions#attribute-overridden transform-paint-servers#pattern transform-paint-servers#gradient transform-paint-servers#clip-path]
- [x] css-transforms-1.md#transform-attribute-specificity: presentation attributes participate in the cascade as if inserted into a new author stylesheet that comes first in the author collection, so the transform CSS property overrides the transform attribute when both are set [transform-functions#attribute-overridden transform-functions#none]
- [x] css-transforms-1.md#svg-wsp: wsp is a line feed, carriage return, tab, or space; comma-wsp (css-transforms-1.md#svg-comma-wsp) is one-or-more wsp with an optional comma, or a comma followed by zero-or-more wsp [transform-functions#attribute-separators]
- [x] css-transforms-1.md#svg-transforms: consecutive transform functions in the attribute need not be separated by a comma [transform-functions#attribute-no-comma]
- [x] css-transforms-1.md#svg-transform-list: the attribute's value is optional whitespace, then zero or more transform functions, then optional whitespace, so an empty or all-whitespace value is valid and means no transform [transform-functions#attribute-empty]
- [x] css-transforms-1.md#svg-syntax: the transform, patternTransform and gradientTransform attributes support only matrix, translate, scale, rotate, skewX and skewY; translateX/Y, scaleX/Y and skew() are not supported by these attributes [transform-functions#attribute-functions]
- [x] css-transforms-1.md#svg-transform-functions: in the attribute, translate/matrix numbers map to CSS lengths in px, and rotate/skewX/skewY numbers map to CSS angles in deg [transform-functions#attribute-units]
- [x] css-transforms-1.md#svg-transform-functions: the attribute's 3-value rotate(angle,cx,cy) is equivalent to translate(cx,cy) rotate(angle) translate(-cx,-cy) and has no single CSS transform-function equivalent [transform-functions#attribute-rotate-center]

## Pattern and gradient transform coordinate system: transform-paint-servers, gradient-transform-functions (exists)

- [x] css-transforms-1.md#svg-user-coordinate-space: for pattern, the patternTransform attribute and the transform property define an additional transformation in the pattern's own coordinate system [transform-paint-servers#pattern]
- [x] css-transforms-1.md#svg-user-coordinate-space: for linearGradient and radialGradient, the gradientTransform attribute and the transform property define an additional transformation in the gradient's own coordinate system [transform-paint-servers#gradient]
- [x] css-transforms-1.md#svg-user-coordinate-space: for every other transformable element, transform defines a transformation in the current user coordinate system of the parent, with percentages relative to the element's reference box [transform-functions#local transform-functions#percent]

## clipPath transform coordinate system: transform-paint-servers, clip-path-transform (exists)

- [x] css-transforms-1.md#svg-user-coordinate-space: for clipPath, the transform attribute and the transform property define an additional transformation in the clipping path's own coordinate space [transform-paint-servers#clip-path transform-paint-servers#clip-path-children]

## 2D transform functions and their matrices: transform-functions, transforms (exists)

- [x] css-transforms-1.md#funcdef-transform-matrix: matrix(a,b,c,d,e,f) is the 3x2 matrix [[a,c,e],[b,d,f]] (css-transforms-1.md#MatrixDefined) [transform-functions#matrix]
- [x] css-transforms-1.md#funcdef-transform-translate: translate(tx,ty): ty defaults to 0; percentages for tx/ty are relative to the reference box's width/height respectively [transform-functions#translate transform-functions#percent]
- [x] css-transforms-1.md#funcdef-transform-translatex: translateX(tx) translates only along X; css-transforms-1.md#funcdef-transform-translatey: translateY(ty) translates only along Y [transform-functions#translate-axis]
- [x] css-transforms-1.md#funcdef-transform-scale: scale(sx,sy): if sy is omitted it equals sx [transform-functions#scale]
- [x] css-transforms-1.md#funcdef-transform-scalex: scaleX(sx) scales [sx,1]; css-transforms-1.md#funcdef-transform-scaley: scaleY(sy) scales [1,sy] [transform-functions#scale-axis]
- [x] css-transforms-1.md#funcdef-transform-rotate: rotate(angle) rotates clockwise about the transform-origin [transform-functions#rotate]
- [x] css-transforms-1.md#transform-functions: wherever an <angle> is accepted by a transform function, a <number> of zero behaves as 0deg [transform-functions#zero-angle]

## 2D skew transform functions: transform-functions, transforms-syntax (exists)

- [x] css-transforms-1.md#funcdef-transform-skew: skew(ax,ay): ay defaults to 0; skew() is kept for compatibility only and its result differs from skewX() multiplied by skewY() [transform-functions#skew transform-functions#skew-axis]
- [x] css-transforms-1.md#funcdef-transform-skewx: skewX(ax) skews only along X; css-transforms-1.md#funcdef-transform-skewy: skewY(ay) skews only along Y [transform-functions#skew-axis]

## Non-invertible transform hides the element: transform-non-invertible

- [x] css-transforms-1.md#transform-function-lists: a list of transform functions acts as if each were applied separately in the order given; the net transform is the matrix product of the list [transform-non-invertible#product]
- [x] css-transforms-1.md#transform-function-lists: if a transform function makes the element's current transformation matrix non-invertible, the element and its content are not displayed [transform-non-invertible#hidden]

## Notes

- css-transforms-1.md#valdef-transform-box-view-box: the two bullets only define the view-box reference box when the viewport-creating element has a viewBox attribute; the case without one is left to the base "nearest SVG viewport" wording, which is easy to under-specify precisely.
