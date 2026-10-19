# CSS Transforms Module Level 2

## Individual translate/rotate/scale properties: transform-individual-properties

- [x] css-transforms-2.md#propdef-translate: translate accepts 1-3 length-percentage/length values for X, Y, Z in order; initial none; a missing Y or Z defaults to 0px; percentages: X relative to the reference box's width, Y to its height [transform-individual-properties#translate]
- [x] css-transforms-2.md#propdef-translate: a zero or omitted Z value makes translate a 2D translation equal to translate(); otherwise it is a 3D translation equal to translate3d() [transform-individual-properties#translate transform-individual-properties#translate-z transform-individual-properties#translate-invalid]
- [x] css-transforms-2.md#propdef-rotate: rotate takes an angle and an optional axis, given by the x/y/z keyword or by 3 numbers as a vector; initial none [transform-individual-properties#rotate transform-individual-properties#rotate-3d transform-individual-properties#rotate-invalid]
- [x] css-transforms-2.md#propdef-rotate: a bare angle, or a rotation about z (by keyword or by a vector whose first two components are zero and third is positive), is a 2D rotation equal to rotate() [transform-individual-properties#rotate]
- [x] css-transforms-2.md#propdef-scale: scale takes 1-3 numbers/percentages for X, Y, Z; initial none; a missing Y defaults to X, a missing Z defaults to 1; a percentage is equivalent to the same number (100% = 1) [transform-individual-properties#scale transform-individual-properties#scale-invalid]
- [x] css-transforms-2.md#propdef-scale: an omitted, 1, or 100% Z value makes scale a 2D scale equal to scale(); otherwise it is a 3D scale equal to scale3d() [transform-individual-properties#scale transform-individual-properties#scale-z]
- [x] css-transforms-2.md#individual-transforms: translate/rotate/scale all default to none, which applies no transform at all, unlike an identity value such as translate: 0px [transform-individual-properties#none]
- [x] css-transforms-2.md#transformation-matrix-computation: the amended matrix order is: origin translate, then translate, rotate, scale (the individual properties), then offset, then the transform function list left to right, then the negated origin translate [transform-individual-properties#order]

## 3D transform functions and their matrices: transform-3d-functions, transform-individual-properties, transform-3d (exists)

- [x] css-transforms-2.md#funcdef-matrix3d: matrix3d(16 numbers) is a 4x4 matrix given in column-major order [transform-3d-functions#matrix3d]
- [x] css-transforms-2.md#funcdef-translate3d: translate3d(tx,ty,tz); css-transforms-2.md#funcdef-translatez: translateZ(tz) is translate3d(0,0,tz) [transform-3d-functions#translate3d]
- [x] css-transforms-2.md#funcdef-scale3d: scale3d(sx,sy,sz); css-transforms-2.md#funcdef-scalez: scaleZ(sz) is scale3d(1,1,sz) [transform-3d-functions#scale3d]
- [x] css-transforms-2.md#funcdef-rotate3d: rotate3d(x,y,z,angle) rotates by angle about the normalized [x,y,z] vector; a vector that cannot be normalized, such as [0,0,0], makes the rotation not apply [transform-3d-functions#rotate3d]
- [x] css-transforms-2.md#funcdef-rotatex: rotateX/rotateY/rotateZ(angle) equal rotate3d(1,0,0,angle) / rotate3d(0,1,0,angle) / rotate3d(0,0,1,angle); rotateZ is the 3D equivalent of the 2D rotate() [transform-3d-functions#rotate-axes]
- [x] css-transforms-2.md#funcdef-perspective: perspective(d) is a projection matrix with element m43 = -1/d; a d below 1px is clamped to 1px for rendering; none is treated as infinite (identity matrix) [transform-3d-functions#perspective]
- [x] css-transforms-2.md#transform-functions: a <number> of zero is allowed wherever <angle> is used here too; a percentage in a scale function is equivalent to the same number [transform-3d-functions#zero-angle]
- [x] css-transforms-2.md#3d-transform-rendering: without a shared 3D rendering context, a 3D-transformed element renders with its transform applied but does not intersect other elements, and painting order is unaffected by its Z position [transform-3d-functions#flat-context]

## 3D transform function argument counts: transform-3d-functions, transform-3d-syntax (exists)

- [x] css-transforms-2.md#funcdef-translatez: translateZ, css-transforms-2.md#funcdef-scalez: scaleZ, css-transforms-2.md#funcdef-rotatex: rotateX/rotateY/rotateZ, and css-transforms-2.md#funcdef-perspective: perspective() each take exactly one argument [transform-3d-functions#arity]
- [x] css-transforms-2.md#funcdef-translate3d: translate3d needs exactly 3 arguments, css-transforms-2.md#funcdef-scale3d: scale3d needs exactly 3, and css-transforms-2.md#funcdef-matrix3d: matrix3d needs exactly 16 [transform-3d-functions#arity]
- [x] css-transforms-2.md#funcdef-rotate3d: rotate3d needs exactly 4 arguments: 3 numbers and one angle [transform-3d-functions#arity]

## perspective() projection and the horizon: transform-3d-functions, transform-perspective (exists)

- [x] css-transforms-2.md#processing-of-perspective-transformed-boxes: to transform a box corner (a,b), apply the accumulated matrix to (a,b,0,1) to get (x,y,z,w); if w>0 the projected point is (x/w, y/w, z/w) [transform-3d-functions#projection transform-3d-functions#perspective]
- [x] css-transforms-2.md#processing-of-perspective-transformed-boxes: if w=0 for a corner, it maps to an implementation-defined point far away in the direction (x,y,z) [transform-3d-functions#horizon]
- [x] css-transforms-2.md#processing-of-perspective-transformed-boxes: if w<0 for all 4 corners of a box, the box is not rendered [transform-3d-functions#behind]
- [x] css-transforms-2.md#processing-of-perspective-transformed-boxes: if w<0 for 1 to 3 corners, the box is replaced by the polygon with the w<0 parts cut off, with exactly two of the resulting vertices at w=0 [transform-3d-functions#horizon]

## backface-visibility on a flat drawing: transform-backface

- [x] css-transforms-2.md#propdef-backface-visibility: values visible | hidden; initial visible; applies to transformable elements; not inherited [transform-backface#shown transform-backface#hidden]
- [x] css-transforms-2.md#backface-visibility: an element with backface-visibility: hidden is hidden exactly when the row-3, column-3 component (m33) of its accumulated 3D transformation matrix is negative, otherwise it is visible [transform-backface#hidden transform-backface#shown]

## 3D transform functions forbidden on paint servers and clipPath: transform-paint-servers

- [x] css-transforms-2.md#svg-three-dimensional-functions: 3D transform functions, and the perspective, perspective-origin, transform-style and backface-visibility properties, cannot be used on clipPath, linearGradient, radialGradient and pattern [transform-paint-servers#pattern-3d transform-paint-servers#gradient-3d]
- [x] css-transforms-2.md#svg-three-dimensional-functions: if a transform list on one of those elements includes a 3D transform function, the complete transform list is ignored, and any value given for those four properties on them is ignored too [transform-paint-servers#pattern-3d transform-paint-servers#gradient-3d]
- [x] css-transforms-2.md#svg-three-dimensional-functions: clipPath, mask and pattern require a flattened representation of their descendants, overriding transform-style: preserve-3d on them [transform-paint-servers#clip-path-flattened]

## Notes

- css-transforms-2.md#accumulated-3d-transformation-matrix-computation and css-transforms-2.md#backface-visibility: the accumulated 3D transformation matrix is defined by walking up a 3D rendering context; it is not stated what this reduces to for an element that establishes or participates in none, which is the only case a renderer without preserve-3d support ever has.
- css-transforms-2.md#processing-of-perspective-transformed-boxes: the scale n used to map a w=0 point to a point "far away" in its direction is explicitly left implementation-defined ("This specification does not define the value of n exactly").
- css-transforms-2.md#processing-of-perspective-transformed-boxes: clipping a transformed shape's corners into the exact 3-5 vertex polygon the w<0 rule describes, for arbitrary (non-rectangular) SVG geometry rather than just boxes, costs real per-element work; a bounding-box-level approximation may be worth it.
