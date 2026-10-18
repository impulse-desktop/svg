# SVG2, Chapter 14: Paint Servers: Gradients and Patterns

## Paint servers are never rendered and inherit from their own ancestors: pserver-ancestry, resource-display

- [x] svg2/pservers.md#Introduction: the UA style sheet must force display:none on paint-server elements, with importance over any other rule or presentation attribute
- [x] svg2/pservers.md#Introduction: a paint-server element stays available for reference even when display:none applies to it or to any ancestor
- [x] svg2/pservers.md#Introduction: properties inherit into a paint-server element from its own ancestors, never from the element that references it via fill or stroke
- [x] svg2/pservers.md#Introduction: paint-server elements are never rendered directly; they are only used by being referenced from a fill or stroke property

## Gradient vector follows the painted element's geometry; stop color interpolation: gradient-vector-interpolation

- [x] svg2/pservers.md#Gradients: referencing a graphics element conceptually copies the gradient vector and normals as part of that element's own geometry
- [x] svg2/pservers.md#Gradients: any transform on the graphics element applies to the copied gradient vector and normals, and the gradient's own gradientTransform is applied before that transform
- [x] svg2/pservers.md#math-gradient-linear-interpolation: between two stops the color of each channel is C0 + (C1-C0) * (D-O0)/(O1-O0), the linear interpolation by distance D along the vector between the stops' offsets O0 and O1

## Paint server href templates: generic attribute and content inheritance: gradient-templates, pattern-tiles

- [x] svg2/pservers.md#PaintServerTemplates: an href attribute on a paint-server element names a compatible element to use as a template
- [x] svg2/pservers.md#PaintServerTemplates: for each templated attribute not specified on the current element, the user agent uses the value the template element resolves to, recursing through the template's own href if needed
- [x] svg2/pservers.md#PaintServerTemplates: the initial value for a templated attribute is substituted only once every valid href reference in the chain is exhausted
- [x] svg2/pservers.md#PaintServerTemplates: if the current element has no child elements other than descriptive elements, a shadow tree is generated for it, cloning the template's child content
- [x] svg2/pservers.md#PaintServerTemplates: if the template itself has no such child content, the shadow tree instead clones the elements used, or that would be used, to build the template's own shadow tree
- [x] svg2/pservers.md#PaintServerTemplates: the generated shadow tree behaves like a use-element shadow tree, with the paint-server element as its host, and its instances are used as if they were the paint server's own children

## Linear gradient vector and coordinate system: gradient-linear-geometry

- [x] svg2/pservers.md#LinearGradientElementX1Attribute: x1, y1, x2 and y2 define the gradient vector onto which the gradient stops are mapped; each is a number or a percentage
- [x] svg2/pservers.md#LinearGradientElementX1Attribute: x1 initial value is 0%
- [x] svg2/pservers.md#LinearGradientElementY1Attribute: y1 initial value is 0%
- [x] svg2/pservers.md#LinearGradientElementX2Attribute: x2 initial value is 100%
- [x] svg2/pservers.md#LinearGradientElementY2Attribute: y2 initial value is 0%
- [x] svg2/pservers.md#LinearGradientAttributes: x1, y1, x2 and y2 are not presentation attributes; CSS styles do not affect their used value
- [x] svg2/pservers.md#LinearGradientElementGradientUnitsAttribute: gradientUnits initial value is objectBoundingBox
- [x] svg2/pservers.md#LinearGradientElementGradientUnitsAttribute: with gradientUnits=userSpaceOnUse, x1/y1/x2/y2 resolve in the user coordinate system at the time the gradient is referenced, then gradientTransform is applied; percentages are relative to the current viewport
- [x] svg2/pservers.md#LinearGradientElementGradientUnitsAttribute: with gradientUnits=objectBoundingBox, x1/y1/x2/y2 resolve against the referencing element's bounding box, then gradientTransform is applied; percentages are relative to that bounding box
- [x] svg2/pservers.md#LinearGradientElementGradientTransformAttribute: gradientTransform is post-multiplied after any bounding-box-to-user-space transform; initial value is the identity transform
- [x] svg2/pservers.md#LinearGradientNotes: if x1=x2 and y1=y2, the painted area is a single solid color and opacity equal to the last gradient stop's

## Gradient spreadMethod: pad, reflect, repeat: gradient-spread

- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: spreadMethod initial value is pad
- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: pad fills the remainder of the target region with the gradient's terminal colors
- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: reflect continuously reflects the gradient pattern start-to-end, end-to-start, etc. until the region is filled
- [x] svg2/pservers.md#LinearGradientElementSpreadMethodAttribute: repeat continuously repeats the gradient pattern start-to-end until the region is filled
- [x] svg2/pservers.md#RadialGradientElementSpreadMethodAttribute: radialGradient's spreadMethod has the same values and meanings as linearGradient's

## Gradient href: target validity and copied attributes: gradient-templates

- [x] svg2/pservers.md#LinearGradientElementHrefAttribute: a linearGradient's href is valid only if it targets a different linearGradient element or a radialGradient element
- [x] svg2/pservers.md#LinearGradientElementHrefAttribute: linearGradient href copies x1, y1, x2, y2, gradientTransform, gradientUnits and spreadMethod from the template
- [x] svg2/pservers.md#RadialGradientElementHrefAttribute: a radialGradient's href is valid only if it targets a linearGradient element or a different radialGradient element
- [x] svg2/pservers.md#RadialGradientElementHrefAttribute: radialGradient href copies cx, cy, r, fx, fy, fr, gradientTransform, gradientUnits and spreadMethod from the template

## Radial gradient circles: cx, cy, r, fx, fy, fr: gradient-radial-geometry

- [x] svg2/pservers.md#RadialGradientElementGradientUnitsAttribute: gradientUnits for radialGradient's cx/cy/r/fx/fy/fr initial value is objectBoundingBox, with the same userSpaceOnUse/objectBoundingBox meanings as for linearGradient
- [x] svg2/pservers.md#RadialGradientElementCXAttribute: cx, cy and r define the end circle; the 100% gradient stop maps to its perimeter
- [x] svg2/pservers.md#RadialGradientElementCXAttribute: cx initial value is 50%
- [x] svg2/pservers.md#RadialGradientElementCYAttribute: cy initial value is 50%
- [x] svg2/pservers.md#RadialGradientElementRAttribute: r initial value is 50%; a negative value is an error
- [x] svg2/pservers.md#RadialGradientElementFXAttribute: fx, fy and fr define the start (focal) circle; the 0% gradient stop maps to its perimeter
- [x] svg2/pservers.md#RadialGradientElementFXAttribute: if fx is not specified, it coincides with the element's own presentational value of cx, whether that value was inherited or not; if the element references a template that specifies fx, fx is inherited from that template
- [x] svg2/pservers.md#RadialGradientElementFYAttribute: fy follows the same rule as fx, coinciding with cy when unspecified
- [x] svg2/pservers.md#RadialGradientElementFRAttribute: fr initial value is 0%; a negative value is an error; if unspecified and the element references a template that specifies fr, fr is inherited from that template

## Radial gradient focal circle outside, or coincident with, the end circle: gradient-radial-focal

- [x] svg2/pservers.md#RadialGradientNotes: if the start (focal) circle lies outside the end circle, the gradient is drawn as a cone touched by both circles, and areas outside the cone stay untouched (transparent black)
- [x] svg2/pservers.md#RadialGradientNotes: if the start circle fully overlaps the end circle, no gradient is drawn and the area stays untouched (transparent black)
- [x] svg2/pservers.md#RadialGradientNotes: when the focal point lies exactly on the end circle's circumference, the area beyond that point on the gradient vector's extension is filled with the weighted average of the gradient's colors, not transparent black

## Gradient stop offsets: range, order, duplicates: gradient-stops

- [x] svg2/pservers.md#StopElementOffsetAttribute: offset is a number or percentage locating the stop along the gradient vector (linear) or as a fractional distance between the inner and outer circles (radial); initial value is 0
- [x] svg2/pservers.md#StopNotes: an offset below 0 (or 0%) is rounded up to 0; an offset above 1 (or 100%) is rounded down to 1
- [x] svg2/pservers.md#StopNotes: each stop's offset must be at least as large as the previous stop's; if not, it is adjusted up to equal the largest offset seen so far
- [x] svg2/pservers.md#StopNotes: if two stops share the same offset, the later stop in document order controls the color at that offset

## Gradient stop color, opacity, and stop count: gradient-stops, gradient-vector-interpolation

- [x] svg2/pservers.md#StopColorProperty: stop-color accepts the same currentColor and color syntaxes as the fill/stroke <paint> value; initial value is black; the property does not inherit
- [x] svg2/pservers.md#StopColorProperty: stop-color's keyword transparent means transparent black (not premultiplied), equivalent to stop-color black with stop-opacity 0
- [x] svg2/pservers.md#StopOpacityProperty: the opacity used for a stop is the product of stop-opacity and any opacity carried by the stop-color value itself; a stop-color type without explicit opacity is treated as opacity 1
- [x] svg2/pservers.md#StopOpacityProperty: stop-opacity's computed value is the specified value converted to a number and clamped to [0,1]; initial value is 1
- [x] svg2/pservers.md#StopNotes: at least two stops are needed for a gradient effect; with no stops defined, painting occurs as if 'none' were specified
- [x] svg2/pservers.md#StopNotes: with exactly one stop defined, the element is painted with the solid color and opacity of that one stop

## Pattern reference rectangle, coordinate system, and tiling: pattern-tiles

- [x] svg2/pservers.md#PatternElement: x, y, width, height and patternUnits define a reference rectangle with top/left at (x,y) and bottom/right at (x+width, y+height)
- [x] svg2/pservers.md#PatternElement: tiling extends copies of the reference rectangle to infinity in x and y, with each tile's origin at (x + m*width, y + n*height) for every integer m and n
- [x] svg2/pservers.md#PatternElementPatternUnitsAttribute: patternUnits initial value is objectBoundingBox; x/y/width/height are not presentation attributes
- [x] svg2/pservers.md#PatternElementPatternUnitsAttribute: with patternUnits=userSpaceOnUse, x/y/width/height resolve in the user coordinate system at the time the pattern is referenced, then patternTransform is applied; percentages are relative to the current viewport
- [x] svg2/pservers.md#PatternElementPatternUnitsAttribute: with patternUnits=objectBoundingBox, x/y/width/height resolve against the referencing element's bounding box, then patternTransform is applied; percentages are relative to that bounding box
- [x] svg2/pservers.md#PatternElementPatternContentUnitsAttribute: patternContentUnits initial value is userSpaceOnUse, and it has no effect at all when a viewBox is specified on the pattern
- [x] svg2/pservers.md#PatternElementPatternContentUnitsAttribute: with patternContentUnits=objectBoundingBox (and no viewBox), the pattern's content coordinate system is established from the referencing element's bounding box, then patternTransform is applied
- [x] svg2/pservers.md#PatternElementPatternTransformAttribute: patternTransform is post-multiplied after any bounding-box-to-user-space transform; initial value is the identity transform
- [x] svg2/pservers.md#PatternNotes: if a viewBox is given, the pattern's content coordinate system is fit into the x/y/width/height/patternUnits region using the standard viewBox and preserveAspectRatio rules; otherwise the content coordinate system simply originates at (x, y)
- [x] svg2/pservers.md#PatternNotes: the UA style sheet sets overflow:hidden on pattern elements, clipping any content outside the pattern tile rectangle unless overflow is overridden (D7)

## Pattern href validity and dimension errors: pattern-tiles, resources-edges (exists)

- [x] svg2/pservers.md#PatternElementHrefAttribute: a pattern's href is valid only if it targets a different pattern element
- [x] svg2/pservers.md#PatternElementHrefAttribute: pattern href copies x, y, width, height, viewBox, preserveAspectRatio, patternTransform, patternUnits and patternContentUnits from the template
- [x] svg2/pservers.md#PatternElementWidthAttribute: width initial value is 0; a negative value is an error; a value of zero disables rendering of the element (no paint applied)
- [x] svg2/pservers.md#PatternElementHeightAttribute: height initial value is 0; a negative value is an error; a value of zero disables rendering of the element (no paint applied)

## Notes

- svg2/pservers.md#PatternNotes: overflow:visible on a pattern is explicitly left undefined by the spec text itself ("currently undefined"), though implementers are "encouraged" to render it left-to-right, top-to-bottom; the renderer must pick one behavior.
- svg2/pservers.md#RadialGradientNotes: the weighted-average fill beyond a focal point sitting exactly on the end circle is an expensive-to-notice edge case (a single-pixel boundary condition) kept only for SVG 1.1 compatibility; cheap rounding at that exact boundary is unlikely to be checked pixel-for-pixel.
