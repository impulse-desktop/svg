# SVG 2, Appendix B: Implementation Notes

## Elliptical arc endpoint-to-center conversion: path-grammar

- [x] svg2/implnote.md#ArcConversionEndpointToCenter: translate by the midpoint of (x1,y1)-(x2,y2) and rotate by -φ to get primed coordinates (x1',y1') (eq. 5.1); this is the frame used for the rest of the conversion.
- [x] svg2/implnote.md#ArcConversionEndpointToCenter: the primed ellipse center (cx',cy') is given by eq. 5.2, taking the + sign when fA != fS and the - sign when fA = fS.
- [x] svg2/implnote.md#ArcConversionEndpointToCenter: the actual center (cx,cy) is (cx',cy') rotated by +φ and translated by the midpoint of (x1,y1) and (x2,y2) (eq. 5.3).
- [x] svg2/implnote.md#ArcConversionEndpointToCenter: the start angle θ1 and sweep Δθ are the signed angles between the unit x-axis/primed start vector and between the primed start/end vectors (eq. 5.4-5.6), Δθ taken mod 360°.
- [x] svg2/implnote.md#ArcConversionEndpointToCenter: Δθ is corrected to lie in (-360°,360°) matching the sweep flag: if fS=0, Δθ must be negative (subtract 360° if the raw value is positive); if fS=1, Δθ must be positive (add 360° if the raw value is negative).
- [x] svg2/implnote.md#ArcCorrectionOutOfRangeRadii: if rx=0 or ry=0, the arc is drawn as a straight line from (x1,y1) to (x2,y2) and no further arc computation is done.
- [x] svg2/implnote.md#ArcCorrectionOutOfRangeRadii: otherwise rx and ry are first replaced by their absolute values (eq. 6.1).
- [x] svg2/implnote.md#ArcCorrectionOutOfRangeRadii: compute Λ = x1'²/rx² + y1'²/ry² from the primed endpoint of eq. 5.1; if Λ>1, scale both rx and ry by sqrt(Λ) (eq. 6.2-6.3) before continuing with the center-parameterization conversion; Λ≤1 needs no change.

## Notes

