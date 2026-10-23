# SVG2, Chapter 14: Paint Servers: Gradients and Patterns

## Paint servers are never rendered and inherit from their own ancestors: pserver-ancestry, resource-display

- [x] svg2/pservers.md#Introduction: the UA style sheet must force display:none on paint-server elements, with importance over any other rule or presentation attribute [pserver-ancestry#not-rendered pserver-ancestry#pattern-display-none]
- [x] svg2/pservers.md#Introduction: a paint-server element stays available for reference even when display:none applies to it or to any ancestor [pserver-ancestry#display-none pserver-ancestry#pattern-display-none]
- [x] svg2/pservers.md#Introduction: properties inherit into a paint-server element from its own ancestors, never from the element that references it via fill or stroke [pserver-ancestry#own-ancestors pserver-ancestry#pattern-content]
- [x] svg2/pservers.md#Introduction: paint-server elements are never rendered directly; they are only used by being referenced from a fill or stroke property [pserver-ancestry#not-rendered]

## Gradient vector follows the painted element's geometry; stop color interpolation: gradient-vector-interpolation

- [x] svg2/pservers.md#Gradients: referencing a graphics element conceptually copies the gradient vector and normals as part of that element's own geometry [gradient-vector-interpolation#shape-transform gradient-vector-interpolation#user-space-transform]
- [x] svg2/pservers.md#Gradients: any transform on the graphics element applies to the copied gradient vector and normals, and the gradient's own gradientTransform is applied before that transform [gradient-vector-interpolation#gradient-transform gradient-vector-interpolation#user-space-transform]
- [x] svg2/pservers.md#math-gradient-linear-interpolation: between two stops the color of each channel is C0 + (C1-C0) * (D-O0)/(O1-O0), the linear interpolation by distance D along the vector between the stops' offsets O0 and O1 [gradient-vector-interpolation#unpremultiplied]

## Paint server href templates: generic attribute and content inheritance: gradient-templates, pattern-tiles, pattern-templates, gradient-hrefs, pattern-template-style

- [x] svg2/pservers.md#PaintServerTemplates: an href attribute on a paint-server element names a compatible element to use as a template [gradient-templates#chain-attributes pattern-templates#copies-x gradient-hrefs#linear-x1]
- [x] svg2/pservers.md#PaintServerTemplates: for each templated attribute not specified on the current element, the user agent uses the value the template element resolves to, recursing through the template's own href if needed [gradient-templates#chain-attributes gradient-templates#chain-stops]
- [x] svg2/pservers.md#PaintServerTemplates: the initial value for a templated attribute is substituted only once every valid href reference in the chain is exhausted [gradient-templates#chain-attributes gradient-radial-geometry#focus-template]
- [x] svg2/pservers.md#PaintServerTemplates: if the current element has no child elements other than descriptive elements, a shadow tree is generated for it, cloning the template's child content [gradient-templates#own-stops gradient-templates#chain-stops pattern-templates#copies-x]
- [x] svg2/pservers.md#PaintServerTemplates: if the template itself has no such child content, the shadow tree instead clones the elements used, or that would be used, to build the template's own shadow tree [gradient-templates#chain-stops]
- [x] svg2/pservers.md#PaintServerTemplates: the template's content is used as if it were the paint server's own children [gradient-templates#cloned-stops pattern-template-style#stock]
- [-] svg2/pservers.md#PaintServerTemplates: the generated shadow tree behaves like a use-element shadow tree with the paint server as its host, so its content inherits from the paint server; it inherits from the template instead, as Inkscape, Chromium, Firefox, librsvg and resvg draw it (D44) [gradient-templates#cloned-stops pattern-template-style#pattern-style pattern-template-style#template-context]

## Linear gradient vector and coordinate system: gradient-linear-geometry, pserver-properties

- [x] svg2/pservers.md#LinearGradientElementX1Attribute: x1, y1, x2 and y2 define the gradient vector onto which the gradient stops are mapped; each is a number or a percentage [gradient-linear-geometry#defaults gradient-linear-geometry#viewport-percent]
- [x] svg2/pservers.md#LinearGradientElementX1Attribute: x1 initial value is 0% [gradient-linear-geometry#defaults]
- [x] svg2/pservers.md#LinearGradientElementY1Attribute: y1 initial value is 0% [gradient-linear-geometry#defaults]
- [x] svg2/pservers.md#LinearGradientElementX2Attribute: x2 initial value is 100% [gradient-linear-geometry#defaults]
- [x] svg2/pservers.md#LinearGradientElementY2Attribute: y2 initial value is 0% [gradient-linear-geometry#defaults]
- [x] svg2/pservers.md#LinearGradientAttributes: x1, y1, x2 and y2 are not presentation attributes; CSS styles do not affect their used value [pserver-properties#css-vector]
- [x] svg2/pservers.md#LinearGradientElementGradientUnitsAttribute: gradientUnits initial value is objectBoundingBox [gradient-linear-geometry#defaults]
- [x] svg2/pservers.md#LinearGradientElementGradientUnitsAttribute: with gradientUnits=userSpaceOnUse, x1/y1/x2/y2 resolve in the user coordinate system at the time the gradient is referenced, then gradientTransform is applied; percentages are relative to the current viewport [gradient-linear-geometry#viewport-percent gradient-vector-interpolation#user-space-transform]
- [x] svg2/pservers.md#LinearGradientElementGradientUnitsAttribute: with gradientUnits=objectBoundingBox, x1/y1/x2/y2 resolve against the referencing element's bounding box, then gradientTransform is applied; percentages are relative to that bounding box [gradient-linear-geometry#defaults gradient-linear-geometry#transform-after-box]
- [x] svg2/pservers.md#LinearGradientElementGradientTransformAttribute: gradientTransform is post-multiplied after any bounding-box-to-user-space transform; initial value is the identity transform [gradient-linear-geometry#transform-after-box gradient-linear-geometry#css-transform]
- [x] svg2/pservers.md#LinearGradientNotes: if x1=x2 and y1=y2, the painted area is a single solid color and opacity equal to the last gradient stop's [gradient-linear-geometry#equal-ends gradient-linear-dense#equal-ends]

## Gradient spreadMethod: pad, reflect, repeat: gradient-spread

- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: spreadMethod initial value is pad [gradient-spread#linear-pad gradient-spread#radial-pad]
- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: pad fills the remainder of the target region with the gradient's terminal colors [gradient-spread#linear-pad gradient-spread#radial-pad gradient-linear-dense#pad-step]
- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: reflect continuously reflects the gradient pattern start-to-end, end-to-start, etc. until the region is filled [gradient-spread#linear-reflect gradient-linear-dense#reflect gradient-linear-dense#threshold-reflect]
- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: repeat continuously repeats the gradient pattern start-to-end until the region is filled [gradient-spread#linear-repeat gradient-linear-dense#repeat gradient-linear-dense#weighted-stops gradient-linear-dense#straight-alpha gradient-linear-dense#linear-rgb gradient-linear-dense#narrow-band gradient-linear-dense#constant-tails gradient-linear-dense#phase gradient-linear-dense#vertical gradient-linear-dense#sheared gradient-linear-dense#object-box gradient-linear-dense#stroke gradient-linear-dense#ordinary-repeat gradient-linear-dense#one-stop gradient-linear-dense#reflected-transform gradient-linear-dense#alpha-hard-stop gradient-linear-dense#linear-alpha gradient-linear-dense#filter-fill-paint gradient-linear-dense#threshold-repeat]
- [x] svg2/pservers.md#RadialGradientElementSpreadMethodAttribute: radialGradient's spreadMethod has the same values and meanings as linearGradient's [gradient-spread#radial-pad gradient-spread#radial-reflect gradient-spread#radial-repeat]

## Gradient href: target validity and copied attributes: gradient-templates, gradient-hrefs

- [x] svg2/pservers.md#LinearGradientElementHrefAttribute: a linearGradient's href is valid only if it targets a different linearGradient element or a radialGradient element [gradient-hrefs#linear-to-radial gradient-hrefs#linear-other gradient-hrefs#linear-self]
- [x] svg2/pservers.md#LinearGradientElementHrefAttribute: linearGradient href copies x1, y1, x2, y2, gradientTransform, gradientUnits and spreadMethod from the template [gradient-hrefs#linear-x1 gradient-hrefs#linear-y1 gradient-hrefs#linear-x2 gradient-hrefs#linear-y2 gradient-hrefs#linear-gradientTransform gradient-hrefs#linear-gradientUnits gradient-hrefs#linear-spreadMethod]
- [x] svg2/pservers.md#RadialGradientElementHrefAttribute: a radialGradient's href is valid only if it targets a linearGradient element or a different radialGradient element [gradient-hrefs#radial-to-linear gradient-hrefs#radial-other gradient-hrefs#radial-self]
- [x] svg2/pservers.md#RadialGradientElementHrefAttribute: radialGradient href copies cx, cy, r, fx, fy, fr, gradientTransform, gradientUnits and spreadMethod from the template [gradient-hrefs#radial-cx gradient-hrefs#radial-cy gradient-hrefs#radial-r gradient-hrefs#radial-fx gradient-hrefs#radial-fy gradient-hrefs#radial-fr gradient-hrefs#radial-gradientTransform gradient-hrefs#radial-gradientUnits gradient-hrefs#radial-spreadMethod]

## Radial gradient circles: cx, cy, r, fx, fy, fr: gradient-radial-geometry

- [x] svg2/pservers.md#RadialGradientElementGradientUnitsAttribute: gradientUnits for radialGradient's cx/cy/r/fx/fy/fr initial value is objectBoundingBox, with the same userSpaceOnUse/objectBoundingBox meanings as for linearGradient [gradient-radial-geometry#defaults gradient-radial-geometry#percent-r]
- [x] svg2/pservers.md#RadialGradientElementCXAttribute: cx, cy and r define the end circle; the 100% gradient stop maps to its perimeter [gradient-radial-geometry#defaults]
- [x] svg2/pservers.md#RadialGradientElementCXAttribute: cx initial value is 50% [gradient-radial-geometry#defaults]
- [x] svg2/pservers.md#RadialGradientElementCYAttribute: cy initial value is 50% [gradient-radial-geometry#defaults]
- [x] svg2/pservers.md#RadialGradientElementRAttribute: r initial value is 50%; a negative value is an error [gradient-radial-geometry#defaults gradient-radial-geometry#negative-r]
- [x] svg2/pservers.md#RadialGradientElementFXAttribute: fx, fy and fr define the start (focal) circle; the 0% gradient stop maps to its perimeter [gradient-radial-geometry#fr]
- [x] svg2/pservers.md#RadialGradientElementFXAttribute: if fx is not specified, it coincides with the element's own presentational value of cx, whether that value was inherited or not; if the element references a template that specifies fx, fx is inherited from that template [gradient-radial-geometry#focus-own gradient-radial-geometry#focus-template]
- [x] svg2/pservers.md#RadialGradientElementFYAttribute: fy follows the same rule as fx, coinciding with cy when unspecified [gradient-radial-geometry#focus-own gradient-radial-geometry#focus-template]
- [x] svg2/pservers.md#RadialGradientElementFRAttribute: fr initial value is 0%; a negative value is an error; if unspecified and the element references a template that specifies fr, fr is inherited from that template [gradient-radial-geometry#fr gradient-radial-geometry#negative-fr gradient-hrefs#radial-fr]

## Radial gradient focal circle outside, or coincident with, the end circle: gradient-radial-focal

- [x] svg2/pservers.md#RadialGradientNotes: if the start (focal) circle lies outside the end circle, the gradient is drawn as a cone touched by both circles, and areas outside the cone stay untouched (transparent black) [gradient-radial-focal#cone]
- [x] svg2/pservers.md#RadialGradientNotes: if the start circle fully overlaps the end circle, no gradient is drawn and the area stays untouched (transparent black) [gradient-radial-focal#start-over-end gradient-start-covers-end#coincident-pad gradient-start-covers-end#coincident-repeat gradient-start-covers-end#coincident-reflect gradient-start-covers-end#contains-end gradient-start-covers-end#internal-tangent gradient-start-covers-end#shifted-equal-radii gradient-start-covers-end#smaller-start gradient-start-covers-end#template-percent]
- [x] svg2/pservers.md#RadialGradientNotes: when the focal point lies exactly on the end circle's circumference, the area beyond that point on the gradient vector's extension is filled with the weighted average of the gradient's colors, not transparent black [gradient-radial-focal#focal-on-circle-repeat gradient-radial-focal#weighted-average gradient-radial-focal#focal-on-circle-pad gradient-radial-focal#tangent-centres gradient-radial-focal#focal-rounded]

## Gradient stop offsets: range, order, duplicates: gradient-stops

- [x] svg2/pservers.md#StopElementOffsetAttribute: offset is a number or percentage locating the stop along the gradient vector (linear) or as a fractional distance between the inner and outer circles (radial); initial value is 0 [gradient-stops#offsets]
- [x] svg2/pservers.md#StopNotes: an offset below 0 (or 0%) is rounded up to 0; an offset above 1 (or 100%) is rounded down to 1 [gradient-stops#offsets]
- [x] svg2/pservers.md#StopNotes: each stop's offset must be at least as large as the previous stop's; if not, it is adjusted up to equal the largest offset seen so far [gradient-stops#offsets]
- [x] svg2/pservers.md#StopNotes: if two stops share the same offset, the later stop in document order controls the color at that offset [gradient-stops#offsets gradient-stops#boundary-start gradient-stops#boundary-end gradient-stops#boundary-radial]

## Gradient stop color, opacity, and stop count: gradient-stops, gradient-vector-interpolation, pserver-properties

- [x] svg2/pservers.md#StopColorProperty: stop-color accepts the same currentColor and color syntaxes as the fill/stroke <paint> value; initial value is black; the property does not inherit [pserver-properties#stop-initial-black pserver-properties#stop-inherit gradient-templates#cloned-stops]
- [x] svg2/pservers.md#StopColorProperty: stop-color's keyword transparent means transparent black (not premultiplied), equivalent to stop-color black with stop-opacity 0 [gradient-stops#transparent]
- [x] svg2/pservers.md#StopOpacityProperty: the opacity used for a stop is the product of stop-opacity and any opacity carried by the stop-color value itself; a stop-color type without explicit opacity is treated as opacity 1 [gradient-stops#opacity]
- [x] svg2/pservers.md#StopOpacityProperty: stop-opacity's computed value is the specified value converted to a number and clamped to [0,1]; initial value is 1 [gradient-stops#opacity]
- [x] svg2/pservers.md#StopNotes: at least two stops are needed for a gradient effect; with no stops defined, painting occurs as if 'none' were specified, including an empty object box or singular gradient map; these do not turn a valid empty paint into its fallback [gradient-stops#no-stops gradient-single-stop-box#no-stop-fallback gradient-single-stop-box#empty-box-singular-fallback gradient-single-stop-box#empty-user-singular-fallback gradient-single-stop-box#radial-empty-singular-fallback]
- [x] svg2/pservers.md#StopNotes: with exactly one stop defined, the element is painted with the solid color and opacity of that one stop, before any gradient coordinate map is needed; this includes zero-area stroked geometry, templates, opacity, scale and clipping. Coordinate errors in genuine multi-stop gradients retain their fallback (D8) [gradient-stops#one-stop gradient-single-stop-box#linear-horizontal gradient-single-stop-box#radial-vertical gradient-single-stop-box#linear-template gradient-single-stop-box#element-opacity gradient-single-stop-box#scaled-horizontal gradient-single-stop-box#user-space-clip gradient-single-stop-box#one-user-singular-fallback gradient-single-stop-box#one-template-singular gradient-single-stop-box#two-user-singular-fallback gradient-single-stop-box#two-box-fallback]

## Pattern reference rectangle, coordinate system, and tiling: pattern-tiles, pserver-properties, pattern-program-reuse

- [x] svg2/pservers.md#PatternElement: x, y, width, height and patternUnits define a reference rectangle with top/left at (x,y) and bottom/right at (x+width, y+height) [pattern-tiles#user-space pattern-tiles#box-fractions]
- [x] svg2/pservers.md#PatternElement: tiling extends copies of the reference rectangle to infinity in x and y, with each tile's origin at (x + m*width, y + n*height) for every integer m and n [pattern-tiles#box-fractions pattern-tiles#user-space]
- [x] svg2/pservers.md#PatternElementPatternUnitsAttribute: patternUnits initial value is objectBoundingBox; x/y/width/height are not presentation attributes [pattern-tiles#box-fractions pserver-properties#css-pattern-tile]
- [x] svg2/pservers.md#PatternElementPatternUnitsAttribute: with patternUnits=userSpaceOnUse, x/y/width/height resolve in the user coordinate system at the time the pattern is referenced, then patternTransform is applied; percentages are relative to the current viewport [pattern-tiles#user-space pattern-coverage-product#png-integer pattern-coverage-product#png-eighth pattern-coverage-product#png-fifteen pattern-coverage-product#png-negative pattern-coverage-product#scaled-integer pattern-coverage-product#scaled-eighth pattern-coverage-product#scaled-fifteen pattern-coverage-product#scaled-negative pattern-coverage-product#svg-integer pattern-coverage-product#svg-fifteen]
- [x] svg2/pservers.md#PatternElementPatternUnitsAttribute: with patternUnits=objectBoundingBox, x/y/width/height resolve against the referencing element's bounding box, then patternTransform is applied; percentages are relative to that bounding box [pattern-tiles#box-fractions]
- [x] svg2/pservers.md#PatternElementPatternContentUnitsAttribute: patternContentUnits initial value is userSpaceOnUse, and it has no effect at all when a viewBox is specified on the pattern [pattern-tiles#box-fractions pattern-tiles#view-box]
- [x] svg2/pservers.md#PatternElementPatternContentUnitsAttribute: with patternContentUnits=objectBoundingBox (and no viewBox), the pattern's content coordinate system is established from the referencing element's bounding box, then patternTransform is applied [pattern-tiles#content-box-units pattern-program-reuse#content-box]
- [x] svg2/pservers.md#PatternElementPatternTransformAttribute: patternTransform is post-multiplied after any bounding-box-to-user-space transform; initial value is the identity transform [pattern-tiles#transform pattern-tiles#css-transform]
- [x] svg2/pservers.md#PatternNotes: if a viewBox is given, the pattern's content coordinate system is fit into the x/y/width/height/patternUnits region using the standard viewBox and preserveAspectRatio rules; otherwise the content coordinate system simply originates at (x, y) [pattern-tiles#view-box pattern-program-reuse#reflected pattern-program-reuse#tile-scale pattern-program-reuse#template-viewbox]
- [x] svg2/pservers.md#PatternNotes: the UA style sheet sets overflow:hidden on pattern elements, clipping any content outside the pattern tile rectangle unless overflow is overridden (D7) [pattern-tiles#overflow-hidden pattern-program-reuse#minimum-tile]

## Pattern href validity and dimension errors: pattern-tiles, pattern-templates

- [x] svg2/pservers.md#PatternElementHrefAttribute: a pattern's href is valid only if it targets a different pattern element [pattern-templates#not-pattern pattern-templates#to-gradient pattern-templates#self]
- [x] svg2/pservers.md#PatternElementHrefAttribute: pattern href copies x, y, width, height, viewBox, preserveAspectRatio, patternTransform, patternUnits and patternContentUnits from the template [pattern-templates#copies-x pattern-templates#copies-y pattern-templates#copies-width pattern-templates#copies-height pattern-templates#copies-viewBox pattern-templates#copies-preserveAspectRatio pattern-templates#copies-patternTransform pattern-templates#copies-patternUnits pattern-templates#copies-patternContentUnits pattern-tiles#template]
- [x] svg2/pservers.md#PatternElementWidthAttribute: width initial value is 0; a negative value is an error; a value of zero disables rendering of the element (no paint applied) [pattern-tiles#zero-size pattern-templates#negative-width pattern-templates#negative-none]
- [x] svg2/pservers.md#PatternElementHeightAttribute: height initial value is 0; a negative value is an error; a value of zero disables rendering of the element (no paint applied) [pattern-tiles#zero-size pattern-templates#negative-height]

## Notes

- svg2/pservers.md#PatternNotes: overflow:visible on a pattern is explicitly left undefined by the spec text itself ("currently undefined"), though implementers are "encouraged" to render it left-to-right, top-to-bottom; the renderer must pick one behavior.
- svg2/pservers.md#RadialGradientNotes: the weighted-average fill beyond a focal point sitting exactly on the end circle is an expensive-to-notice edge case (a single-pixel boundary condition) kept only for SVG 1.1 compatibility; cheap rounding at that exact boundary is unlikely to be checked pixel-for-pixel.

## Zero and small radial circles: gradient-radial-zero

These multi-stop gradients follow the SVG 2 two-circle geometry. SVG 1.1
painted a zero end radius with the last stop; `gradient-edge` retains its
original input, but its 8 by 8 zero-radius cell now stays transparent. The
other pixels of that existing unit stay unchanged. The new unit certifies
whole rectangular probes independently of browser images.

- [x] svg2/pservers.md#RadialGradientNotes: a start circle fully overlapping the end circle leaves the area untouched, including coincident point circles; a positive radius cannot be replaced by a solid color merely because its value before transformation is small. [gradient-radial-zero#zero-concentric gradient-radial-zero#tiny-covered gradient-radial-zero#zero-end-covered]
- [x] svg2/pservers.md#RadialGradientNotes: areas outside the cone are transparent; two point circles sweep no area, and a valid empty gradient does not invoke the paint fallback. [gradient-radial-zero#zero-shifted gradient-radial-zero#zero-repeat gradient-radial-zero#zero-valid-fallback gradient-radial-zero#tiny-exterior gradient-radial-zero#normal-exterior gradient-radial-zero#zero-end-exterior gradient-radial-zero#smaller-exterior gradient-radial-zero#tiny-linear-exterior]
- [x] svg2/pservers.md#RadialGradientNotes: interior cone probes retain their paint under an equivalent power-of-two transformation, including an end circle of zero radius and translucent paint. [gradient-radial-zero#tiny-interior gradient-radial-zero#normal-interior gradient-radial-zero#zero-end-interior gradient-radial-zero#tiny-transparent]
- [x] svg2/pservers.md#RadialGradientElementRAttribute: a negative radius is an error and takes the supplied paint fallback. [gradient-radial-zero#negative-fallback]
