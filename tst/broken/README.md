# Known rendering failures

Minimal static SVG cases found by reading `svg.cpp`, forming a hypothesis,
and rendering the input against an independent drawing. The renderer and
the frozen canon are unchanged. Text, animation, script, foreignObject,
documented design departures and budget exhaustion are not test subjects.

Each top-level SVG has a same-named drawing in `expect/`. The expectations
use literal rectangles or an empty drawing; gradient ramps use one-pixel
strips calculated from their interpolation formulas. Test
cells use the existing oracle's `id` and `data-box` convention.

Run from the repository root, with a freshly built test driver:

```sh
./build svg
python3 tst/broken/check.py ./svg
```

The checker uses `tst/oracle.py` and its usual two-level premultiplied RGBA
tolerance. It exits 1 while differences remain, writes cell reports to
`.build/broken/`, and reports a case as PASS once it matches. These known
failures are separate from the passing `unit`/`tests` build groups.

## Cases

Confirmed against `8058ab0ee` at the sizes declared in the SVGs:

| Input | Expected behavior | Observed failure | Code hypothesis |
| --- | --- | --- | --- |
| `clip-child-basic-shape.svg` | The clip child's `inset()` retains only x=8..32. | 1,152 extra opaque pixels. | `clipNeedsProgram` and `resolveClip` inspect `clipPath`, omitting `clipShape`. |
| `clip-use-target-clip.svg` | A shape instanced inside a clip keeps its own clipping path. | 1,152 extra opaque pixels. | `clipChild` computes the target's style, but clip-program selection and construction inspect only the use element's clip. |
| `clip-child-conditions.svg` | Children with a nonmatching language or unsupported extension contribute nothing. | 2,304 extra opaque pixels. | `clipChild` omits `passesConditions`. |
| `use-font-size-bbox.svg` | `x="1em"` with the use's 32px font puts its box at x=32; clipping keeps x=32..40. | All 128 expected pixels disappear. | `childExtent` resolves the use's position with its parent's font size. |
| `radial-cone-underflow.svg` | The complete sample rectangle is strictly outside the radial cone. | 2,304 extra opaque pixels. | `radialT`'s float discriminant underflows from negative to zero. |
| `gradient-translated-coordinates.svg` | Translation cancellation preserves a 32px red/blue ramp. | 1,536 pixels differ, by up to 251 levels. | `gradientLength` narrows distinct endpoints to the same float before the world translation cancels. |
| `root-zero-rem.svg` | A root font size of zero makes `1rem` zero in both geometry and transforms. | 896 pixels differ. | `resolveLength` replaces a zero root font size with 16. |
| `css-foreign-siblings.svg` | Nonpainting foreign elements still count as element siblings. | Both 16px squares turn red, 512 wrong pixels. | `parseElement` removes foreign nodes before `nth-child` and adjacent-sibling matching. |
| `convolve-target-rounding.svg` | An identity convolution with `targetX="calc(.75)"` rounds its integer target to 1. | The square moves left by one pixel, 32 wrong pixels. | `filterConvolve` casts `numberOf` to int instead of using integer rounding. |
| `shape-after-close.svg` | A line after `close` starts another contour at the closed subpath's initial point. | Only the small first contour remains, 960 missing pixels. | `shapeCommandsPath` omits the implicit move that `parsePathData` already inserts after a close. |
| `nested-use-language.svg` | The English outer host's language reaches the rectangle through both use instances. | The 256-pixel square is red instead of green. | `nodeLanguage` follows the original inner use's ancestors instead of the current instance's host chain. |
| `outline-currentcolor.svg` | Explicit `currentColor` draws the same green outline as the named color. | All 320 outline pixels disappear. | The outline parser sends `currentColor` to `parseColor`, then disables the outline on its rejection. |
| `css-invalid-declaration-cascade.svg` | Invalid literal declarations leave earlier valid opacity, display, visibility and linecap values in effect. | 832 pixels differ across four cells. | `validDeclaration`'s default true lets these invalid values win before `computeStyle` rejects them. |
| `marker-root-rem-recomputation.svg` | Root `font-size:2rem` computes once to 32px; the marker inherits that computed size. | 2,112 extra opaque pixels from a 64px marker. | `documentStyle` recomputes the root against its already computed root font size. |
| `transform-inherited-font-units.svg` | An inherited `1em` translation keeps its parent's computed 8px length. | Both squares move to x=40 instead of 16; 1,024 wrong pixels. | `inheritedGeometry` copies specified transform tokens rather than their absolute computed lengths. |
| `transform-origin-length-context.svg` | Origin lengths use the element's font and logical document viewport. | 704 pixels differ across font and viewport unit cells. | `readOrigin` substitutes a 16px font and reference-box-sized screen instead of the element's `TransformBasis`. |
| `embedded-svg-root-effects.svg` | An image's SVG root applies opacity, clipping and filtering; its canvas paints its background. | 896 pixels differ across four cells. | `drawImage` lowers the nested root's children directly, skipping root effects and canvas painting. |
| `anisotropic-raster-minification.svg` | Shrinking identical rows preserves the red and blue interiors of magnified columns. | All 192 interior pixels become purple. | `rasterPaint` chooses both prefilter levels from the most minified axis, collapsing the 8px-wide source to one texel. |
| `linear-blend-white-cache.svg` | White multiplied by white remains opaque white in linear light. | All 2,304 pixels become transparent. | `blendLinear`'s initial all-ones cache keys match valid white pixels and return the initial zero result. |
| `data-base64-invalid-padding.svg` | Interior or excessive base64 padding makes an image resource invalid. | Both invalid resources draw, adding 512 pixels. | `decodeDataUri` skips every equals sign instead of validating its position and count. |
| `pattern-nonpainting-content.svg` | A pattern's own non-descriptive child prevents cloning template content, even if that child does not paint. | 2,304 extra opaque pixels from the template. | `patternContent` tests for renderable children instead of non-descriptive children. |
| `lighting-normalize-overflow.svg` | A distant overhead point light illuminates a flat surface green. | All 256 pixels become opaque black. | `normalize3` squares the representable `1e20` coordinate in float, overflowing the length and returning a zero direction. |
| `css-radial-negative-stop.svg` | A negative radial stop participates in interpolation at positive radii. | All 2,048 pixels are black instead of mid-gray. | `cssGradientPaint` clamps negative stop offsets to zero without interpolating a replacement color there. |
| `dash-close-unmatched-join.svg` | A closing dash has no join to an initial gap. | A 4px by 4px miter appears outside the only dash, 16 extra pixels. | `dashPolyline` adds the closing join whenever the last dash is on, without checking the first dash. |
| `fe-image-variable-instance.svg` | An element instanced by `feImage` inherits custom properties from that host. | The 256-pixel square is red instead of green. | `lowerImageContent` enters the referenced element without the `instanceStyles` setup used by `lowerUse`. |
| `image-view-percent-escape.svg` | A `%3B` separator selects the same SVG view as a literal semicolon. | The whole red/green image is fitted instead, 192 wrong pixels. | `loadImageFrom` passes an undecoded fragment to `viewPart` and `fragmentView`. |
| `marker-zero-direction.svg` | An initial zero-length segment takes the direction of the following vertical segment. | The marker points right instead of down, 256 wrong pixels. | `markerVertices` records a zero tangent as a known direction; `vertexAngle` uses it without searching neighboring segments. |
| `arithmetic-reader-classification.svg` | Unused branches and input-free primitives do not change arithmetic data flowing to the final result. | Both cells lose their red channel, 512 wrong pixels. | `keepsInvalidColor` counts unreachable consumers and the unused `in1` field of `feFlood`. |
| `filter-coefficient-cancellation.svg` | The color matrix's red result is `2^25 - 2^24`, clamped to 1. | All 2,304 pixels become black instead of red. | `fixedCoefficient` clamps each coefficient to `2^24` before the weighted sum, incorrectly cancelling both terms. |
| `turbulence-bbox-coordinate-space.svg` | All sampled points fall on noise lattice knots, producing constant 50% gray at 50% alpha. | All 256 pixels contain colored noise, differing by up to 88 levels. | `filterTurbulence` uses local user coordinates without applying `boxPrimitives`. |
| `lighting-bbox-surface-height.svg` | The point light and flat surface have the same z coordinate, so their diffuse dot product is zero. | All 256 pixels receive green light instead of black. | `filterLighting` converts the light's z from box units but leaves `surfaceScale` in unscaled user units. |
| `fragment-atom-without-target.svg` | An unrelated attribute must not block a local href's percent-decoded ID lookup. | All 256 expected green pixels disappear. | `fragmentAtom` treats an interned string as an existing target and returns before checking the decoded ID. |
| `stroke-finite-coordinate-sum.svg` | A finite line and stroke scale to a 10px by 8px rectangle. | All 80 expected pixels disappear. | `strokePath` tests the finiteness of the float sum `x+y`, which overflows even though each coordinate is finite. |
| `use-shadow-root-siblings.svg` | An instance root is its shadow tree's only child and has no following element sibling. | Both 256-pixel squares turn red. | Child indexing in `matchCondition` and sibling traversal in `matchHas` read original siblings past the instance boundary. |
| `use-transform-font-context.svg` | The rect's own `1em` transform uses its inherited 32px instance font. | The square moves to x=24 instead of 40, 512 wrong pixels. | `transformAnchor` takes its font from `documentStyle`, which follows original parents and caches the source's 16px font. |
| `css-gradient-negative-hint.svg` | A negative transition hint still determines the interpolation curve. | 496 pixels differ, by up to 53 levels. | `cssStops` uses `hint >= 0` to distinguish a supplied hint from its no-hint sentinel. |
| `css-gradient-hint-stop-fixup.svg` | The unplaced middle color stop is at 50%; the following sampled strip is fully opaque. | 112 pixels have too little opacity, by up to 37 levels. | `cssStops` counts transition hints as positioned color stops when distributing unplaced stops, putting white at 62.5%. |
| `relative-math-number.svg` | Dividing `1em` by `1em` produces the number 1 in opacity and color calculations. | Both cells ignore the calculation, 512 wrong pixels. | `readMathQuantity` rejects any calculation that encountered relative units, even when they cancel to a number. |
| `mask-no-clip-local-sentinel.svg` | An opaque mask with no clipping preserves a translated square. | All 256 expected pixels disappear. | `imageMaskLayer` substitutes a finite local box of ±30,000,000 for `no-clip`, cutting away visible content under a compensating translation. |
| `image-relative-calc-natural-size.svg` | The embedded root's `calc(2em)` width is 32px; the image's half-width clip retains all 8px of green content. | Half the green rectangle disappears, 64 missing pixels. | `givenSize` and `explicitSide` test the unresolved `Length.value`, zero for relative calculations, before resolving the positive size. |
| `filter-function-inherited-color-space.svg` | CSS brightness operates in sRGB after an identity SVG filter. | All 2,304 pixels have red 188 instead of 128. | `lowerFilters` passes the previous SVG primitive's linear flag into `functionFilter`, which should always use sRGB. |
| `pattern-bbox-transform-basis.svg` | A .25 translation in object-box tile coordinates shifts the stripes by 8px. | All 512 target pixels differ from the expected stripe phase. | `patternPaint` converts tile coordinates to user units before applying the unconverted transform, shifting by .25px. |
| `linear-gradient-normalization-underflow.svg` | Exact power-of-two scaling preserves a 32px red/blue ramp. | All 512 pixels become blue, differing by up to 251 levels. | `linearT` computes `dx*dx + dy*dy` in float, underflowing the denominator to zero while its double numerator stays positive. |
| `css-gradient-hint-at-end.svg` | The gradient is opaque from its final white stop onward, retaining the right half of the square. | All 256 expected green pixels disappear. | `cssStops` gives every sampled stop weight zero when the hint coincides with the second stop, overwriting that stop's own color. |
| `css-invalid-pseudo-element.svg` | An unknown pseudo-element or a repeated `::before` invalidates its complete selector list. | Both 256-pixel squares turn red. | `parseCompound` accepts any pseudo-element name and permits invalid combinations, keeping the other selector of the invalid rule. |

The clipping expectations follow [CSS Masking's clipping model](../../dev/spec/css-masking-1.md#ClipPathElement)
and [SVG conditional processing](../../dev/spec/svg2/struct.md#ConditionalProcessing).
Font-relative geometry follows [CSS font-relative units](../../dev/spec/css-values-4.md#font-relative-lengths),
including the project's existing D11 language and D29 font-metric choices.
The radial cone follows [SVG radial-gradient notes](../../dev/spec/svg2/pservers.md#RadialGradientNotes):
after the exact power-of-two transform, every sampled point satisfies
`3*y*y > (x-2)*(x-2)` and is outside the cone. The selector expectations
follow [child indexing](../../dev/spec/selectors-4.md#child-index)
and [next-sibling matching](../../dev/spec/selectors-4.md#adjacent-sibling-combinators).

Each hypothesis also has an equivalent rendering control: replace the basic clip by
its literal half rectangle; inline the clipped use target; remove the
conditionally excluded children; replace `1em` by 32; normalize the cone
to radius 1 and focus 2; move the linear gradient to the origin; replace
`1rem` by zero; or retain each foreign sibling as a nonpainting SVG
`metadata` element. For the remaining three cases, replace the target
calculation by 1, insert `move to 8px 8px` after the first close, or directly
instance the rectangle instead of the intermediate use. These controls
match their expectations exactly.

Integer targets follow [integer math-function rounding](../../dev/spec/css-values-4.md#css-round-to-the-nearest-integer)
in [targetX](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-targetx),
shape commands follow the [shape close command](../../dev/spec/css-shapes-1.md#valdef-shape-close)
and its [SVG subpath semantics](../../dev/spec/svg2/paths.md#PathDataClosePathCommand),
and nested languages follow the existing D40 choice of host language for instances.

The outline case follows [outline colors](../../dev/spec/css-ui-4.md#outline-color).
Invalid declarations follow [CSS error handling](../../dev/spec/css-syntax-3.md#css-invalid),
not the separate rules for values invalidated by variable substitution.
Root font calculations follow [font-affecting properties](../../dev/spec/css-values-4.md#font-affecting-property).
Inherited transforms follow the absolute computed lengths of
[transform](../../dev/spec/css-transforms-1.md#propdef-transform) and
[translate](../../dev/spec/css-transforms-2.md#propdef-translate).
Origin lengths follow [transform-origin](../../dev/spec/css-transforms-1.md#propdef-transform-origin)
and the existing D31 viewport choice. Equivalent controls are, respectively,
an explicit lime outline, removal of the invalid declarations, a literal
32px root font, explicit 8px inherited translations, and literal 8px/16px
origin lengths. Each was rendered and matches its expectation exactly.

Embedded documents follow the [SVG image processing model](../../dev/spec/svg2/embedded.md#ImageElement),
with the existing D52 canvas-background choice. Moving the root effects
onto child groups and representing the background with a child rectangle
produces exactly the independent expectation. The raster uses
[pixelated scaling](../../dev/spec/css-images-3.md#valdef-image-rendering-pixelated),
which treats each axis independently. The expectation samples only solid
interiors, away from the red/blue transition. Replacing 64 identical source
rows with eight, without changing the image's displayed size, matches it
exactly with the same sampling mode.

The blend follows the [multiply equation](../../dev/spec/compositing-1.md#blendingmultiply)
and [linearRGB compositing](../../dev/spec/svg2/painting.md#ColorInterpolationProperty).
Switching to sRGB matches the white expectation exactly. The data URLs
follow [forgiving-base64 decoding](https://infra.spec.whatwg.org/#forgiving-base64-decode),
which only removes one or two trailing padding characters before checking
the alphabet. Both repaired URLs were also rendered and produce complete
green squares; both invalid forms are rejected by resvg 0.49.0.

The pattern follows the [paint-server template rule](../../dev/spec/svg2/pservers.md#PaintServerTemplates):
`defs` is not descriptive content. Replacing it with an empty `g` matches
the empty expectation exactly; resvg also leaves the original empty.
The light follows the [diffuse-lighting equations](../../dev/spec/filter-effects-1.md#elementdef-fediffuselighting).
With zero surface scale, its normal is `(0,0,1)`, and the light's normalized
z component differs from 1 by less than `1e-38` over the complete square.
Reducing its z coordinate to `1e18` produces the expected green exactly.
Resvg shares the original overflow, so the expectation comes from the
lighting equation rather than that renderer.

The radial case follows [radial stop placement](../../dev/spec/css-images-3.md#radial-color-stops).
At radius r the sRGB channel is `(r + 1000000) / 2000000`, which rounds
to 128 everywhere on this canvas. The large ending radius keeps that
expectation a literal rectangle. Replacing the negative black stop with
`#808080 0%` also renders exactly as the expectation.

The dash follows [stroke construction](../../dev/spec/svg2/painting.md#StrokeShape):
the 128px perimeter has an initial 112px gap followed by a single 16px
vertical dash with butt caps. Drawing that dash as `M16 32V16` matches the
literal rectangle exactly, as does resvg's rendering of the original.
The filter image follows [feImage's use behavior](../../dev/spec/filter-effects-1.md#elementdef-feimage)
and the existing D41 inheritance choice. An ordinary `use` with the same
custom property renders the expected green exactly; replacing the target's
variable expression with literal lime also matches exactly.

The view follows [escaped SVG fragment identifiers](../../dev/spec/svg2/linking.md#SVGFragmentIdentifiersDefinitions),
which explicitly allow a semicolon written as `%3B`. Replacing only that
escape with a semicolon selects the intended view and matches exactly.
The marker follows [automatic orientation](../../dev/spec/svg2/painting.md#RenderingMarkers)
and the [direction of a zero-length initial segment](../../dev/spec/svg2/paths.md#TermPathSegmentStartDirection).
Removing the redundant `V16` preserves this direction and renders the
expected rectangle exactly. Resvg shares the original zero-tangent error;
the expected direction is derived from the path rules above.

The arithmetic case follows the [primary filter tree](../../dev/spec/filter-effects-1.md#FilterPrimitiveTree)
and the [component-wise arithmetic equation](../../dev/spec/filter-effects-1.md#elementdef-fecomposite),
with the renderer's existing choice to retain color above alpha between
arithmetic consumers. Red minus green gives `(1,0,0,0)` after component
clamping. Halving that and adding opaque green or black gives the two
expected colors. Removing the unused matrix and moving the independent
black flood before the first composite produce exactly those expectations.

The coefficient case follows the [color matrix equation](../../dev/spec/filter-effects-1.md#elementdef-fecolormatrix).
Both input channels equal 1, so their positive difference saturates to red;
no large canvas or amount of work is involved. Halving both coefficients
still saturates to the same red and renders exactly as the expectation.
Resvg also renders the original as that red.

The noise and surface height follow [primitive units](../../dev/spec/filter-effects-1.md#element-attrdef-filter-primitiveunits).
For the [turbulence algorithm](../../dev/spec/filter-effects-1.md#elementdef-feturbulence),
the 16.5px box makes frequency 33 equal to frequency 2 in user space.
Every pixel center then has integral lattice coordinates, whose noise is
zero independently of the random gradients; fractalNoise maps that to
50% in all channels. The clip samples only whole interior pixels.
User-space frequency 2 renders the constant expectation exactly.

For the [surface height](../../dev/spec/filter-effects-1.md#element-attrdef-fediffuselighting-surfacescale),
both z values are 1 box unit, or 16px. Light parallel to this constant-height
surface has zero dot product with its normal. Explicit user-space
`surfaceScale="16"` and light position `(8,8,16)` render the expected black
exactly. Resvg matches both original expectations exactly. Neither case
depends on the turbulence-gradient choice in D25 or distant-light orientation
in D56.

The local link follows [escaped fragment identifiers](../../dev/spec/svg2/linking.md#SVGFragmentIdentifiersDefinitions)
and [target lookup](../../dev/spec/svg2/linking.md#processingURL-target).
The `%66` escape denotes `f`, so the use references `face`; metadata's
unrelated data attribute is not an ID. Removing just that attribute, or
spelling the href as `#face`, restores the exact expected square. This
exercises `fragmentAtom`'s false target hit, separately from the embedded
image view parser's missing decoding.

The stroke follows [stroke construction](../../dev/spec/svg2/painting.md#StrokeShape)
and the existing D18 numeric range. Each authored coordinate, width and
scale is finite in single precision, and the device drawing is small.
The expected line runs from `(20,20)` to `(30,20)` with an 8px butt stroke.
Normalizing the coordinates renders it exactly. Negating the source y and
its transform's y scale also renders it exactly while keeping the large
source coordinates and stroke width; only the erroneous overflowing sum
is avoided.

The shadow cases follow [SVG 2 style scoping and inheritance](../../dev/spec/svg2/struct.md#UseStyleInheritance)
and [child indexing](../../dev/spec/selectors-4.md#child-index).
Neither original metadata sibling is cloned into the use's shadow tree;
the instance root matches `:only-child` and cannot match `:has(+ metadata)`.
Removing those metadata siblings, leaving the instanced subtrees unchanged,
renders both green expectations exactly.

The transform follows that same inheritance rule and [font-relative units](../../dev/spec/css-values-4.md#font-relative-lengths).
The declaration is on the rectangle itself, with no `transform:inherit`
or `transform-origin` involved. Its 32px inherited font makes its translation
32px, after the use's x=8. Replacing `1em` with `32px`, or explicitly setting
32px on the source group, renders exactly as the expectation. This exercises
the instance font chosen for the transform basis, separately from copying
specified inherited transform tokens or resolving origin lengths.

The gradient hints follow [transition-hint interpolation](../../dev/spec/css-images-3.md#coloring-gradient-line)
and [stop-position fixup](../../dev/spec/css-images-3.md#color-stop-fixup).
The negative hint is one quarter of the way from its black stop to its
white stop, giving exponent `log(.5)/log(.25) = .5`. At column x the color
is therefore `sqrt(((x+.5)/32+1)/2)`, represented by the literal gray strips.
A 64px-wide gradient with stops at 0% and 100%, a hint at 25%, and position
-32px renders that same visible interval and matches exactly.

In the fixup case, the surrounding positioned color stops are at 0% and
100%, so the unplaced white belongs at 50%. Its 25% hint then makes that
first interval linear. The clip samples only the fully opaque interval
after 50%. Explicitly placing the middle white at 50% renders the expected
solid green exactly. These exercise hint detection and color-stop placement,
separately from clamping negative offsets in the radial paint builder.

Unit cancellation follows [calculation type checking](../../dev/spec/css-values-4.md#calc-type-checking)
and [computed math values](../../dev/spec/css-values-4.md#calc-computed-value),
including the project's D30 math support. The same nonzero length divided
by itself gives 1, so the expressions produce opacity .5 and green channel
255, independently of the font size. Replacing `1em / 1em` with
`16px / 16px` makes both controls match their expectations exactly.

The mask follows [no-clip](../../dev/spec/css-masking-1.md#valdef-mask-clip-no-clip),
which leaves the painted content unrestricted. The matrix maps the square
to `(8,8)` at its original 16px size, with no large raster or amount of
work. Keeping the same large coordinates but using fill-box for this
fully opaque mask matches exactly; moving the square and its mask to the
origin's coordinate range while retaining no-clip also matches exactly.

The natural image size follows [SVG image sizing](../../dev/spec/svg2/embedded.md#ImageElement)
and the existing D10 document-size choice. The embedded SVG gives width
`calc(2em)` and height 16; with its initial 16px font the width is 32px.
Its positioning rectangle's left half is 16px wide, retaining the complete
8px-wide drawing. Replacing the width expression with `2em` matches the
expectation exactly, keeping both the font-relative unit and content.

The filter list follows [filter-function color spaces](../../dev/spec/filter-effects-1.md#FilterProperty).
The default linearRGB identity matrix leaves the input color unchanged;
brightness(.5) must then multiply its sRGB red by .5. Removing the identity
reference or making that identity matrix use sRGB produces the expected
128 red exactly. Resvg renders the original within one level of the
expectation.

The pattern follows [pattern-transform composition](../../dev/spec/svg2/pservers.md#PatternElementPatternTransformAttribute),
including the object-box map before the extra transform. The 32px-wide
box makes .25 units an 8px shift. Converting the complete pattern to user
units (x/y 8, width/height 16, a child 8px wide and an 8px translation)
matches the literal stripes exactly. The SVG working group affirmed this
ordering in [issue 293](https://github.com/w3c/svgwg/issues/293); resvg shares
the browser mismatch described there.

The linear ramp follows [linear-gradient coordinates](../../dev/spec/svg2/pservers.md#LinearGradientElement),
with endpoint `2^-80` and horizontal scale `2^85`, exactly 32px in device
space. Its distinct endpoints are representable in float; the loss occurs
later, when `linearT` squares their difference. Normalizing the endpoint to
32 and removing the scale matches the analytic strips exactly. Resvg
shares the original underflow. This is independent of endpoint narrowing
before translation cancellation and of the radial cone's discriminant.

The endpoint hint follows [gradient-line colors](../../dev/spec/css-images-3.md#coloring-gradient-line):
at and after the final white stop, the color is white regardless of the
interpolation curve before it. Replacing the hinted interval by coincident
transparent/white stops at 50% matches the literal right-half rectangle
exactly. This exercises preservation of the final color, separately from
hint detection and positioning of unplaced stops.

The selector lists follow [invalid-selector handling](../../dev/spec/selectors-4.md#invalid)
and [pseudo-element combinations](../../dev/spec/selectors-4.md#pseudo-element):
an unknown pseudo-element and `::before::before` are invalid, so neither
rule applies to the ordinary rectangle in its list. Replacing each invalid
pseudo-element selector with the invalid pseudo-class `rect:no-such-pseudo`
makes both controls match exactly. Resvg shares the original list error.
This tests rejection of invalid selectors, without rendering generated
content, and is separate from acceptance of invalid declaration values.

Group additional manifestations of an existing cause into its existing
case instead of adding another top-level failure for the same cause.
