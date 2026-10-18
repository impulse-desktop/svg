<!-- https://svgwg.org/svg2-draft/implnote.html, as of 2026-10-18; written by dev/spec_text.py -->

# Appendix B: Implementation Notes

## <a id="Introduction"></a>B.1. Introduction

The following notes describe algorithms and other strategies which can be used by software developers when converting content to and from the formats required by features in the SVG language.

## <a id="ArcImplementationNotes"></a>B.2. Elliptical arc parameter conversion

To be consistent with other path segment notation, arcs in SVG paths are defined in terms of start and end points on the curve.

This parameterization of elliptical arcs will be referred to as *endpoint parameterization*. One of the advantages of endpoint parameterization is that it permits a consistent path syntax in which all path commands end in the coordinates of the new "current point".

However, this is not the only way of describing arc geometry used in software or mathematics. This section describes the alternative center parameterization, and how to convert it from and to SVG's endpoint parameterization.

### <a id="ArcSyntax"></a>B.2.1. Elliptical arc endpoint syntax

An elliptical arc, as represented in the SVG path command, is described by the following parameters in order:

(*x*_1, *y*_1) are the absolute coordinates of the current point on the path, obtained from the last two parameters of the previous path command.

*r_x* and *r_y* are the radii of the ellipse (also known as its semi-major and semi-minor axes).

*φ* is the angle from the x-axis of the current coordinate system to the x-axis of the ellipse.

*f_A* is the large arc flag, and is 0 if an arc spanning less than or equal to 180 degrees is chosen, or 1 if an arc spanning greater than 180 degrees is chosen.

*f_S* is the sweep flag, and is 0 if the line joining center to arc sweeps through decreasing angles, or 1 if it sweeps through increasing angles.

(*x*_2, *y*_2) are the absolute coordinates of the final point of the arc.

### <a id="ArcParameterizationAlternatives"></a>B.2.2. Parameterization alternatives

An arbitrary point (*x*, *y*) on the elliptical arc can be described by the 2-dimensional matrix equation:

(eq. 3.1)

( x y ) = ( cos⁡φ -sin⁡φ sin⁡φ cos⁡φ ) · ( rx⁢cos⁡θ ry⁢sin⁡θ ) + ( cx cy )

```
<a id="math-arbitrary-point-on-elliptical-arc"></a>
    x = rx*cos(θ)*cos(φ) - ry*sin(θ)*sin(φ) + cx
    y = rx*cos(θ)*sin(φ) + ry*sin(θ)*cos(φ) + cy
  
```

(*c_x*, *c_y*) are the coordinates of the center of the ellipse.

*r_x* and *r_y* are the radii of the ellipse (also known as its semi-major and semi-minor axes).

*φ* is the angle from the x-axis of the current coordinate system to the x-axis of the ellipse.

*θ* is the angle around the arc that the point (*x*, *y*) lies at, and ranges from:

- *θ*_1 which is the start angle of the elliptical arc prior to the stretch and rotate operations.
- *θ*_2 which is the end angle of the elliptical arc prior to the stretch and rotate operations.
- Δ*θ* which is the difference between these two angles.

If one thinks of an ellipse as a circle that has been stretched and then rotated, then *θ*_1, *θ*_2 and Δ*θ* are the start angle, end angle and sweep angle, respectively of the arc prior to the stretch and rotate operations. This leads to an alternate parameterization which is common among graphics APIs, which will be referred to as *center parameterization*. In the next sections, formulas are given for mapping in both directions between center parameterization and endpoint parameterization.

### <a id="ArcConversionCenterToEndpoint"></a>B.2.3. Conversion from center to endpoint parameterization

Given the following variables:

*c_x* *c_y* *r_x* *r_y* *φ* *θ*_1 Δ*θ*

the task is to find:

*x*_1 *y*_1 *x*_2 *y*_2 *f_A* *f_S*

This can be achieved using the following formulas:

| x 1 y 1 = cos φ - sin φ sin φ cos φ ⋅ r x cos θ 1 r y sin θ 1 + c x c y | (eq. 4.1) |
| --- | --- |
| x 2 y 2 = cos φ - sin φ sin φ cos φ ⋅ r x cos θ 1 + Δ θ r y sin θ 1 + Δ θ + c x c y | (eq. 4.2) |
| f A = 1 if Δ θ > 180 ° 0 if Δ θ ≤ 180 ° | (eq. 4.3) |
| f S = 1 if Δ θ > 0 ° 0 if Δ θ < 0 ° | (eq. 4.4) |

### <a id="ArcConversionEndpointToCenter"></a>B.2.4. Conversion from endpoint to center parameterization

Given the following variables:

*x*_1 *y*_1 *x*_2 *y*_2 *f_A* *f_S* *r_x* *r_y* *φ*

the task is to find:

*c_x* *c_y* *θ*_1 Δ*θ*

The equations simplify after a translation which places the origin at the midpoint of the line joining (*x*_1, *y*_1) to (*x*_2, *y*_2), followed by a rotation to line up the coordinate axes with the axes of the ellipse. All transformed coordinates will be written with primes. They are computed as intermediate values on the way toward finding the required center parameterization variables. This procedure consists of the following steps:

-
*Step 1: Compute* (*x*_1′, *y*_1′)
| x 1 ′ y 1 ′ = cos φ sin φ - sin φ cos φ ⋅ x 1 - x 2 2 y 1 - y 2 2 | (eq. 5.1) |
| --- | --- |

-
*Step 2: Compute* (*c_x*′, *c_y*′)
| c x ′ c y ′ = ± r x 2 r y 2 - r x 2 y 1 ′ 2 - r y 2 x 1 ′ 2 r x 2 y 1 ′ 2 + r y 2 x 1 ′ 2 r x y 1 ′ r y - r y x 1 ′ r x | (eq. 5.2) |
| --- | --- |

where the + sign is chosen if *f_A* ≠ *f_S*, and the − sign is chosen if *f_A* = *f_S*.
-
*Step 3: Compute* (*c_x*, *c_y*) *from* (*c_x*′, *c_y*′)
| c x c y = cos φ - sin φ sin φ cos φ ⋅ c x ' c y ' + x 1 + x 2 2 y 1 + y 2 2 | (eq. 5.3) |
| --- | --- |

-
*Step 4: Compute* *θ*_1 and Δ*θ*
In general, the angle between two vectors (*u_x*, *u_y*) and (*v_x*, *v_y*) can be computed as
| ∠ u ⇀ , v ⇀ = ± arccos u ⇀ ⋅ v ⇀ u ⇀ v ⇀ | (eq. 5.4) |
| --- | --- |

where the ± sign appearing here is the sign of *u_x* *v_y* − *u_y* *v_x*.
This angle function can be used to express *θ*_1 and Δ*θ* as follows:
| θ 1 = ∠ 1 0 , x 1 ′ - c x ′ r x y 1 ′ - c y ′ r y | (eq. 5.5) |
| --- | --- |
| Δ θ ≡ ∠ x 1 ′ - c x ′ r x y 1 ′ - c y ′ r y , - x 1 ′ - c x ′ r x - y 1 ′ - c y ′ r y mod 360 ° | (eq. 5.6) |

where Δ*θ* is fixed in the range −360° < Δ*θ* < 360° such that:
if *f_S* = 0, then Δ*θ* < 0,
else if *f_S* = 1, then Δ*θ* > 0.
In other words, if *f_S* = 0 and the right side of (eq. 5.6) is greater than 0, then subtract 360°, whereas if *f_S* = 1 and the right side of (eq. 5.6) is less than 0, then add 360°. In all other cases leave it as is.

### <a id="ArcCorrectionOutOfRangeRadii"></a>B.2.5. Correction of out-of-range radii

This section describes the mathematical adjustments required for out-of-range *r_x* and *r_y*, as described in the [Path implementation notes](paths.md#ArcOutOfRangeParameters). Algorithmically these adjustments consist of the following steps:

-
*Step 1: Ensure radii are non-zero*
If *r_x* = 0 or *r_y* = 0, then treat this as a straight line from (*x*_1, *y*_1) to (*x*_2, *y*_2) and stop. Otherwise,
-
*Step 2: Ensure radii are positive*
Take the absolute value of *r_x* and *r_y*:
| r x ← r x r y ← r y | (eq. 6.1) |
| --- | --- |

-
*Step 3: Ensure radii are large enough*
Using the primed coordinate values of equation (eq. 5.1), compute
| Λ = x 1 ′ 2 r x 2 + y 1 ′ 2 r y 2 | (eq. 6.2) |
| --- | --- |

If the result of the above equation is less than or equal to 1, then no further change need be made to *r_x* and *r_y*. If the result of the above equation is greater than 1, then make the replacements
| r x ← Λ r x r y ← Λ r y | (eq. 6.3) |
| --- | --- |

-
*Step 4: Proceed with computations*
Proceed with the remaining elliptical arc computations, such as those in the [Conversion from endpoint to center parameterization](#ArcConversionEndpointToCenter) algorithm. Note: As a consequence of the radii corrections in this section, the equation (5.2) for the center of the ellipse always has at least one solution (i.e. the radicand is never negative). In the case that the radii are scaled up using equation (eq. 6.3), the radicand of (eq. 5.2) is zero and there is exactly one solution for the center of the ellipse.

## <a id="NumericPrecisionImplementationNotes"></a>B.3. Notes on generating high-precision geometry

This section is informative.

The [real number precision](types.md#Precision) of SVG is single-precision. [conforming SVG generators](conform.md#ConformingSVGGenerators) handling technical data where expression of information exceeding single precision is desired, such as maps and technical drawings, are encouraged to follow the process outlined in this section to ensure consistent display in [conforming SVG viewers](conform.md#ConformingSVGViewers).

Presentation with an effective precision higher than single-precision may be obtained by taking advantage of the fact that at least double-precision floating point must be used when generating a CTM (See CTM generation processing in the [Conforming SVG Viewers](conform.md#ConformingSVGViewers) section). The steps for generating content that takes advantage of this are:

1. Split content into tiles such that the number of significant digits required to position and size each object within a tile is within the range of single precision floats. Besides, in this description, the coordinate system which the original content has originally will be called source space.
2. Generate a coordinate transformation matrix per tile to transform from source space to tile space, where tile space is a coordinate system with origin *(0,0)* at the top left of the tile. Each element of the transformation matrix must be within the range of single precision.
3. Transform the contents of each tile from source space to tile space using the generated coordinate transformation matrix. The result is that the parameters of each object can now be expressed with significant digits within the range of single precision floats.
4. For each tile, generate an inverse transformation matrix for the conversion of tile space to source space. This is used as a [transform](coords.md#TransformProperty) attribute of the element for the tile of the next step.
5. Arrange each tile as a separate user coordinate system in SVG. For example, the tiles may be expressed a ‘[g](struct.md#GElement)’ elements with an [transform](coords.md#TransformProperty) attribute having the transformation matrix generated by the previous step. And the split graphics generated by the third step will be placed as children of it.

Example Splitting vector graphics bigger than a tile

| Before Splitting |  | After Splitting |
| --- | --- | --- |
| [image: Example Before splitting] |  | [image: Example After Splitting] |

Example Improving Significant Digits

| Step 1 : Splitting content |  | Step 5 : Arranging tiles with smaller effective digits and appropriate translate |
| --- | --- | --- |
| [image: Example Split content into tiles] |  | [image: Example Translate and assign split contents] |
| This example provides the significant figure of eight digits using tiles with the user coordinate system of seven digits. |  |  |
