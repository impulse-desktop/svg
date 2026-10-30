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
The implicit-circle radial case can crash before producing pixels, and
the dash-period overflow case hangs until the oracle's existing 60-second
renderer timeout. The checker reports these process failures instead of
cell reports and continues with the remaining cases.

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
| `css-radial-implicit-circle-at.svg` | A single 32px radius implies a circle; the whole 32px square lies before its first lime stop. | The renderer can crash in `resolveLength` instead of producing an image. | `gradientShape` returns at `at` before inferring the circle, leaving `cssGradientPaint` to read an uninitialized second radius. |
| `embedded-svg-root-viewport.svg` | The embedded root's `50vw` font is 16px, so its `1em` rectangle is 16px wide. | The rectangle is 32px wide, adding 256 pixels. | `drawImage` computes the nested root's style with the outer viewport before entering the nested document's length context. |
| `convolve-kernel-reach.svg` | The 2-by-1 kernel samples the flood 64px to its right, producing a green square. | All 256 expected pixels disappear. | `primitiveReach` hardcodes 32px for convolution, so `applyFilter` prunes the required intermediate flood before the 64px-spaced tap reads it. |
| `lighting-output-subregion.svg` | The surface normal at x=16 reads the transparent input at x=15, giving green 180 in the first column. | All 16 pixels of that column are green 255 instead. | `filterLighting` copies only its output box into the alpha buffer and duplicates its edges, discarding valid neighboring input pixels. |
| `css-selector-geometry-mutation.svg` | The sibling selector matches the anchor's original `x="0"`, regardless of its CSS geometry. | The 256-pixel square stays red instead of green. | `cascadeNode` overwrites geometry attributes before later elements' selectors read them through `matchCondition`. |
| `css-xlink-href-selector.svg` | `[href]` does not match the use's namespaced `xlink:href`, so its lime fill remains. | The 256-pixel square turns red. | `parseElement` aliases XLink href to the unqualified `AtomHref`, losing the namespace that attribute matching needs. |
| `inherited-currentcolor-capture.svg` | An explicitly inherited `currentColor` resolves against the stop's or flood's own lime color. | Both 256-pixel squares are red instead of green. | `computeStyle` resolves these color keywords immediately, then copies the parent's resolved color when processing `inherit`. |
| `css-media-mixed-operators.svg` | Mixing `and` and `or` without grouping parentheses makes the query invalid, leaving the square green. | The 256-pixel square turns red. | `mediaCondition` folds whichever operator comes next, without enforcing a single operator at each level. |
| `css-media-feature-validation.svg` | Unknown keywords, fractional literal color depths and min-prefixed discrete features do not match, including under `not`. | All three 256-pixel squares turn red. | `mediaFeature` and `mediaValue` compare values without validating the individual feature's keywords, numeric type or range/discrete classification. |
| `css-media-math-values.svg` | Calculated 96dpi resolution and aspect ratio 2 match the renderer's 1dppx and the 64-by-32 viewport. | Both 256-pixel squares stay red. | `mediaValue` requires raw dimension or number tokens for resolution and ratio instead of accepting math functions of those types. |
| `image-multiple-fragment-separators.svg` | The PNG body ends at the first `#`; the complete `part#tail` fragment leaves its green pixels intact. | All 256 expected pixels disappear. | `loadImageFrom` searches backward for the last `#`, leaving `#part` in the data body and invalidating its decoding. |
| `data-base64-header-whitespace.svg` | Spaces before the base64 keyword or at the end of the MIME header still select base64 decoding. | Both 256-pixel images disappear. | `decodeDataUri` checks the untrimmed header for the literal seven-character suffix `;base64`. |
| `css-svg-view-aspect-ratio.svg` | Both fragment forms override preserveAspectRatio to none while retaining the root's viewBox, stretching each white half to 16px. | Each square loses its left 8px, 256 missing pixels in total. | `fragmentView` validates but discards preserveAspectRatio, and `loadImageFrom` retains a named view only when it supplies its own viewBox. |
| `conic-gradient-whole-turn-loss.svg` | One million complete turns leave the mask's right half opaque. | 198 pixels differ, by up to 255 levels. | `gradientT` subtracts the large float turn count before reducing modulo one, rounding sample directions to sixteenths of a turn. |
| `visibility-hidden-decorations.svg` | Hidden graphics paint neither their markers nor their outlines. | A 256-pixel marker and a 320-pixel outline remain visible. | `lowerShape` gates fill and stroke on `style.visible` but calls `lowerMarkers` without it; the outline block in `lowerElementIn` also omits that check. |
| `outline-filter-transformed-layer.svg` | The outline is part of the filter input, so invert changes green to magenta. | All 320 outline pixels remain green. | `lowerFilteredLayer` records only `lowerContent` in the filtered layer; `lowerElementIn` emits the outline afterward, outside that filter. |
| `pattern-viewbox-content-units.svg` | A pattern with a viewBox ignores patternContentUnits and paints the horizontal stroke green. | All 256 stroke pixels use the red fallback instead. | `patternPaint` rejects a zero-height object box for box content units before considering the viewBox that supersedes them. |
| `drop-shadow-invalid-deviation.svg` | An invalid nonpresentation deviation takes its initial value; a fully transparent shadow leaves the green source intact. | All 256 source pixels disappear. | `filterDropShadow` returns an empty output on a number-pair parse failure instead of retaining its initial deviation. |
| `symbol-outline-instance-bbox.svg` | The instanced symbol's 16px square has a 4px green outline. | All 320 outline pixels disappear. | `lowerSvg` clears `l.sized` before the outline's `elementBox` call, so `elementExtent` no longer recognizes the symbol as an active instance. |
| `filter-attribute-css-escapes.svg` | Escaped luminanceToAlpha and SourceAlpha keywords both produce opaque black from white. | Both 256-pixel squares keep the wrong colors: white and red. | `filterColorMatrix` and `filterInput` compare raw attribute strings instead of decoded CSS identifier tokens. |
| `clip-use-positioned-clip.svg` | The use's x translation moves both its geometry and its own nested clip, retaining x=16..32. | All 256 expected pixels disappear. | `clipChild` adds x/y only to `space`; `resolveClip` reads the untranslated `own` matrix for the use's clip. |
| `root-display-none.svg` | Display none on the root excludes its complete subtree from rendering. | The 256-pixel square remains visible. | `lowerElementIn` explicitly exempts the document root from its `style.displayNone` early return. |
| `resource-individual-transforms.svg` | Individual translate moves the clip, pattern stripes and gradient stop by 16px. | 1,280 pixels differ across three cells. | Resource transform construction reads only the transform attribute/property, omitting the individual transform properties. |
| `cubic-bbox-absolute-epsilon.svg` | The scaled curve has a nonzero object box, so its constant gradient fills the sampled interior rectangle. | All 128 expected pixels disappear. | `cubicExtremes` treats derivative coefficients below an absolute `1e-12` threshold as zero, missing the curve's interior height. |
| `css-gradient-missing-components.svg` | Missing alpha or all missing color components borrow white's corresponding values, leaving both masks fully opaque. | All 512 square pixels lose opacity, by up to 247 levels. | `parseFunctionColor` replaces none by zero and returns a Color without missing-component metadata before `cssStops` interpolates. |
| `css-preferred-invalid-sheet.svg` | The text/plain style creates no CSS sheet; the first actual CSS sheet selects the good set and paints green. | The 256-pixel square stays red. | `cascade` chooses a preferred title from any nonempty style element before checking its type. |
| `image-view-natural-ratio.svg` | A selected 32-by-16 view supplies aspect ratio 2, so natural width 16 gives height 8. | Both 8px-high green images move down by 4px, 256 wrong pixels in total. | `loadImageFrom` calls `givenSize` before selecting the view, retaining the 16px height inferred from the root's square viewBox. |
| `three-d-distant-plane-tolerance.svg` | The green plane at z=1 covers the later red plane at z=0, regardless of the distant blue corner. | All 256 square pixels turn red. | `emitContext` scales the sorting tolerance by the largest coordinate of any plane, merging distinct neighboring depths when another plane is distant. |
| `projective-marker-layer-bounds.svg` | The marker's projected rectangle covers the complete 8px-by-8px sampling clip. | All 64 expected pixels disappear. | `lowerOnLayer` allocates a perspective layer from `elementBox` plus a fixed geometric margin, which omits marker paint extending beyond the path's geometry. |
| `projective-centre-behind-eye.svg` | Clipping away the part behind the eye retains the front part covering the sampled square. | All 64 expected pixels disappear. | `lowerOnLayer` returns false when the box's middle projects behind the eye, even if a portion of the box is in front. |
| `filter-negative-math-blur.svg` | Calculated negative blur deviations clamp to zero, leaving invert to turn both green squares magenta. | Both 256-pixel squares stay green. | `parseFilterList` and `parseDropShadow` reject negative resolved blur lengths without applying the math-function range clamp. |
| `dash-period-overflow.svg` | The short path lies entirely in a dash gap and paints nothing. | The renderer hangs before producing an image. | `dashPolyline` sums two finite dash lengths into float infinity; normalizing a negative offset then loops forever subtracting finite lengths from infinity. |
| `use-vector-effect-position.svg` | Non-scaling-size preserves the transformed origin at (16,16), while the square remains 16px wide. | The square moves to (8,8), changing 384 pixels. | `lowerElementIn` constrains the CTM with `vectorEffectMatrix` before appending the use's x/y translation. |
| `vector-effect-determinant-range.svg` | Non-scaling-size removes either finite uniform scale, leaving two 16px squares at their translated origins. | All 512 expected pixels disappear. | `vectorEffectMatrix` narrows the double CTM determinant to float, turning it into infinity or zero before normalization. |
| `turbulence-negative-seed-range.svg` | The negative seed normalizes to 5; thresholding its green channel produces a lime square. | All 64 square pixels turn black. | `filterTurbulence` saturates the seed to signed 32-bit range before `turbulenceInit` takes its remainder, changing the normalized seed to 3. |
| `mesh-origin-length-context.svg` | The first mesh starts at its own 1em=32px; the second starts halfway across the referencing object's box. | Both 256-pixel patches disappear. | `meshPaint` resolves the starting point with a hardcoded 16px font and the outer viewport, without selecting the mesh's font or bounding-box percentage context. |
| `marker-user-units-non-scaling-orient.svg` | The user-space marker follows the path's 45-degree user-space direction, covering the sampling rectangle. | All 256 expected pixels disappear. | `markerPlacement` chooses a device-space tangent whenever the stroke is non-scaling, even when userSpaceOnUse selects the user-space placement matrix. |
| `percent-length-diagonal-range.svg` | A 25% stroke in either square viewport maps to a 16px-wide horizontal band. | Both 1,024-pixel bands disappear. | `resolveLength` computes the normalized viewport diagonal by squaring its float sides, overflowing to infinity or underflowing to zero. |
| `dash-math-range-clamp.svg` | The calculated negative dash length clamps to zero, leaving two 8px dashes. | 224 pixels differ, including extra backwards-traversed dashes. | `validDashes` clamps a temporary Length, but `computeStyle` rereads each entry without `nonNegative`, passing the negative value to `dashPolyline`. |
| `css-nonlinear-gradient-minification.svg` | Periods far below a pixel average to a uniform 50%-opaque mask. | 1,987 pixels have wrong opacity, differing by up to 128 levels. | `prepareGradient` averages minified repeating linear gradients only; radial and conic gradients continue sampling individual phases. |
| `css-gradient-stop-sentinel.svg` | The white stop's negative position clamps to the preceding 0px stop, making the entire mask opaque. | The left 256 pixels lose opacity, by up to 247 levels. | `cssStops` treats every offset below its finite unplaced-stop threshold as absent, even when the stop has an explicit position. |
| `css-carriage-return-preprocessing.svg` | CSS converts CR to LF, then removes the escaped newline so the selector matches lime. | The 256-pixel square stays red. | `tokenizeInto` does not normalize the CSS input stream; `consumeString` and `unescaped` recognize LF continuations only. |
| `xml-internal-default-attribute.svg` | The internal attribute-list declaration supplies the rectangle's lime fill. | The 256-pixel square is black. | `parseDoctype` handles entity declarations but skips ATTLIST declarations, so `parseElement` never receives the default attribute. |
| `number-mantissa-intermediate-overflow.svg` | The long number equals 16, giving a 16px-wide square. | The square expands to the canvas edge, adding 384 pixels. | `readDoubleSyntax` overflows while accumulating the mantissa before applying its compensating exponent; `readDouble` then saturates that infinity to 3e38. |
| `specular-exponent-unrestricted.svg` | The unrestricted exponents give alpha 1 and cos(5deg)^256 on the two flat surfaces. | All 512 square pixels have wrong opacity, by up to 75 levels. | `filterLighting` retains the old 1..128 clamp for feSpecularLighting's exponent, although Filter Effects defines it as an unrestricted number. |
| `lighting-invalid-constant.svg` | Invalid negative lighting constants take their initial value 1, giving two fully lit green squares. | The diffuse square turns black and the specular square disappears, 512 wrong pixels. | `filterLighting` clamps a negative constant to zero instead of rejecting the literal value and using its initial value. |
| `css-gradient-premultiplied-alpha.svg` | Transparent red contributes no red; each blue strip has alpha (x+.5)/16 over white. | The first column's 16 pixels gain red and lose blue, by four levels. | `cssStops` samples premultiplied interpolation into straight colors, then `gradientColor` interpolates the first interval in straight RGBA again. |

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

The implicit circle follows [radial shape inference](../../dev/spec/css-images-3.md#valdef-radial-gradient-radial-shape).
Every point of the square is less than `sqrt(16*16 + 16*16)` from the
center, below the 32px radius of its first lime stop. Adding only `circle`
renders the solid green expectation exactly. The unchanged optimized
renderer crashed with SIGSEGV on three consecutive renders of the original;
gdb located the read in `resolveLength`, called through `shapeLength` and
`cssGradientPaint`. Because the second radius is uninitialized, the precise
symptom depends on stack contents. This is unrelated to negative stop
offsets or transition hints.

The embedded root follows [viewport-relative lengths](../../dev/spec/css-values-4.md#viewport-relative-lengths)
and the existing D31 logical-document viewport choice. Its own explicit
width is 32px, and the image is displayed at that same size, so 50vw is
16px. Replacing only that font size by 16 renders exactly as the expectation;
rendering the original embedded document on its own also gives the correct
16px width. This tests the root's computed length context, separately from
skipping root rendering effects and from natural-size calculation.

The convolution follows [kernel tap spacing](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength):
with the reversed kernel and target zero, the nonzero tap reads x+64.
The complete input and output are inside the canvas, and the kernel has
only two entries. Replacing the convolution by an offset of -64 matches
exactly; using SourceGraphic for the same square instead of an intermediate
flood also matches exactly with the original convolution. Reducing the
spacing to 16 and moving the input to x=32 is another exact control.
Resvg does not reproduce the requested spacing, so it is not the reference.
This exercises dependency pruning, separately from target rounding and
coefficient arithmetic.

The lighting follows [surface-normal calculation](../../dev/spec/filter-effects-1.md#SurfaceNormalCalculations)
and [output subregions](../../dev/spec/filter-effects-1.md#FilterPrimitiveSubRegion).
At x=16 the interior Sobel x slope is 1, so the normal is
`(-1,0,1)/sqrt(2)` and the overhead light gives `round(255/sqrt(2)) = 180`.
The rest of the sampled interval is flat. Removing the lighting primitive's
explicit output subregion, while retaining the same final clip, matches the
literal expectation exactly; resvg also matches the original exactly.
This tests which input pixels form the normal, independently of light-vector
normalization and object-box surface-height conversion.

The geometry selector follows [attribute matching](../../dev/spec/selectors-4.md#attribute-selectors):
the style changes the anchor's geometry, not its XML attribute. Removing
only the invisible anchor's CSS x declaration renders the green expectation
exactly. Keeping that declaration and selecting the anchor by ID instead of
its x attribute is another exact control. Resvg renders the original exactly.
This exercises mutation of selector inputs during the cascade, separately
from omitted foreign siblings and invalid selector syntax.

The XLink selector follows [attribute namespaces](../../dev/spec/selectors-4.md#attrnmsp):
an attribute selector without a namespace component matches only attributes
with no namespace. The use has only XLink href and must inherit its own lime
fill into the referenced rectangle. Removing the nonmatching rule, or
replacing its test with `[data-href]`, matches the expectation exactly while
retaining the original XLink reference. Resvg shares the original namespace
aliasing error. This exercises attribute identity, independently of CSS
geometry overwriting an existing attribute's value.

The inherited colors follow [currentcolor resolution](../../dev/spec/css-color-4.md#resolving-other-colors):
the computed value remains the keyword, and an inheriting element uses
its own color to resolve it. Both children explicitly inherit `stop-color`
or `flood-color`, while their own `color` is lime.
Replacing the two `inherit` values by `currentColor` makes the controls
match exactly. Resvg matches the gradient cell but not the flood cell.
This exercises inheritance of an unresolved keyword, separately from
the outline parser rejecting an explicitly specified currentColor.

The media query follows [combining media features](https://www.w3.org/TR/mediaqueries-4/#media-conditions):
`and` and `or` cannot be mixed at the same level without extra parentheses.
The invalid query must not apply its red declaration. Changing only `or`
to `and` gives a valid, false query at this 32px viewport and renders the
same green expectation exactly. Resvg also renders the original green.
This exercises condition grammar, independently of invalid selectors or
declaration values.

The feature checks follow [media-query error handling](https://www.w3.org/TR/mediaqueries-4/#error-handling):
unrecognized or syntactically invalid feature values are unknown, and
negation preserves unknown. The discrete grid feature also disallows
[min/max prefixes](https://www.w3.org/TR/mediaqueries-4/#mq-min-max).
Using the valid, false queries `not (orientation:landscape)`,
`not (color:8)` and `(grid:1)` makes all three controls match exactly.
Resvg also renders the original green. These share missing per-feature
validation, separately from accepting mixed boolean operators.

The calculated values follow [math-function types](../../dev/spec/css-values-4.md#calc-type-checking)
and the existing D30 support for math in media queries. The first
calculation is a resolution of 96dpi, equal to 1dppx; the second is the
number 2, the aspect ratio of this viewport. Replacing the calculations
with their literal values matches both expectations exactly. Resvg shares
the original rejection, so the type rules provide the reference. This
tests acceptance of valid math, independently of relative-unit cancellation
and of validation of invalid literal features.

The fragment follows [opaque-path URL parsing](https://url.spec.whatwg.org/#opaque-path-state)
and the [data URL processor](https://fetch.spec.whatwg.org/#data-url-processor),
which decodes the body without its fragment. Changing the fragment to
`#part-tail`, or removing it entirely, makes both controls match the
literal green rectangle exactly. Resvg renders the original exactly.
This exercises the URL/body boundary, separately from percent-decoding
SVG view fragments and from validation of base64 padding.

The header follows that same data URL processor: trim its surrounding
ASCII whitespace, then allow spaces between the semicolon and base64.
Removing only these two header spaces makes both controls match their
green rectangles exactly. Resvg also renders both originals exactly.
The PNG payload is unchanged and valid; this exercises selection of the
decoder from the header, independently of the decoder's body validation.

The views follow [fragment view overrides](../../dev/spec/svg2/linking.md#SVGFragmentIdentifiersDefinitions):
specified view parameters replace the root's, and omitted parameters retain
their root values. These are CSS images, so an image element's own
preserveAspectRatio does not override them. Setting none on both embedded
roots matches exactly; alternatively, retaining the first control and
adding the unchanged `viewBox="0 0 16 16"` to the second view also matches
exactly. This tests preservation of the selected view's aspect parameter,
separately from decoding its fragment or applying root rendering effects.

The conic gradient follows [gradient rotation](https://www.w3.org/TR/css-images-4/#conic-gradients)
and [angle units](../../dev/spec/css-values-4.md#angle-value). The authored
360,000,000 degrees are exactly representable as a float and equal exactly
one million turns. The opaque interval is therefore the clockwise half
from straight up to straight down: the literal right-half rectangle.
Replacing only the starting angle with 0deg matches it exactly. This
exercises angle reduction before subtraction, independently of gradient
coordinate narrowing and squared-length underflow.

The hidden decorations follow [graphics visibility](../../dev/spec/svg2/render.md#VisibilityControl):
a hidden graphics element is not painted, including the marker paint layer
of its path and its outline. Replacing hidden visibility with display none
makes both cells empty as expected. Removing the marker reference and
outline declaration, while keeping both shapes hidden, is another exact
control. Resvg also leaves the original marker cell empty. This exercises
missing visibility checks on decoration paint, independently of marker
direction and outline color parsing.

The filtered outline follows [filter input content](../../dev/spec/filter-effects-1.md#FilterProperty),
which explicitly includes the element's outline. The 180-degree rotation
leaves this square and its outline at the same coordinates, with integer
boundaries. Removing only that rotation matches the magenta expectation
exactly; keeping it and moving the filter to a wrapping group also matches
exactly. This exercises which paint is captured by a transformed filter's
layer, separately from visibility and CSS filter color-space selection.

The pattern follows [content coordinate units](../../dev/spec/svg2/pservers.md#PatternElementPatternContentUnitsAttribute):
patternContentUnits has no effect when a viewBox is specified. Its tile
uses user coordinates, so the line's zero-height object box is immaterial.
Removing only patternContentUnits, or setting it to userSpaceOnUse,
matches the literal green rectangle exactly. Resvg also rejects the
original pattern, leaving a transparent stroke. This exercises a stale
object-box precondition, independently of template selection and pattern
transform coordinates.

The drop shadow follows [attribute parse failures](../../dev/spec/svg2/types.md#syntax)
and its [initial deviation](../../dev/spec/filter-effects-1.md#element-attrdef-fedropshadow-stddeviation),
including the project's D16 rule for nonpresentation attributes. Three
numbers do not match number-optional-number, so the deviation defaults
to 2. With zero flood opacity the shadow contributes nothing regardless
of blur, and the original green square remains. Removing stdDeviation,
or replacing it with 2, makes both controls match exactly; resvg also
renders the original exactly. This exercises primitive parse-failure
handling, separately from CSS declaration cascade validation.

The symbol follows [outline applicability](../../dev/spec/css-ui-4.md#outline)
and [symbol instances](../../dev/spec/svg2/struct.md#SymbolNotes), including
the project's D12 rule that the instanced symbol renders. Its viewBox and
viewport are the same size, and visible overflow leaves room for the
outline. Replacing the symbol with a group matches the literal ring exactly;
retaining the symbol and moving its outline to a group around the same
rectangle also matches exactly. This exercises loss of instance state
during later geometry measurement, separately from outline visibility,
color parsing and filter capture.

The escaped filter attributes follow [CSS attribute parsing](../../dev/spec/svg2/types.md#attribute-css-value)
and [identifier escapes](../../dev/spec/css-syntax-3.md#escaping), including
the project's D32 rule for attributes written in CSS value syntax.
The escape decodes to uppercase A, retaining the exact keyword spelling.
LuminanceToAlpha maps opaque white to opaque black; SourceAlpha reads
that same opaque black silhouette instead of the preceding red flood.
Replacing only the escapes with literal A makes both cells match exactly.
Resvg also compares the original type as raw text. These two manifestations
share bypassing token decoding, independently of URL-fragment decoding
and numeric attribute error handling.

The positioned clip follows [the use's additional translation](../../dev/spec/svg2/struct.md#UseLayout)
and [nested clipping paths](../../dev/spec/css-masking-1.md#ClipPathElement).
The clip belongs to the use, while the referenced rectangle has no clip.
Replacing x=16 with transform="translate(16)" matches the green rectangle
exactly. Replacing the use with its translated rectangle and retaining the
same nested clip is another exact control. Resvg renders the original
exactly. This exercises the transform supplied to a clip that is already
recognized, independently of omitting the referenced shape's own clip.

The hidden root follows [rendering-tree exclusion](../../dev/spec/svg2/render.md#VisibilityControl):
display none removes a normally rendered container and all its descendants.
This tests suppression of SVG content, independently of the outer CSS box
excluded by D39. Moving display none to a group around the rectangle, or
replacing it on the root with opacity zero, makes both controls empty as
expected. Resvg also leaves the original empty. This exercises an explicit
root exemption, separately from hidden decorations and from skipping an
embedded document root's rendering effects.

The resource translations follow [transformable elements](../../dev/spec/css-transforms-1.md#transformable-element)
and [individual transform composition](../../dev/spec/css-transforms-2.md#ctm).
Paint servers and clipPath are transformable, and the one-value translate
is a two-dimensional translation of 16px along x. Replacing the three
translate declarations with transform:translate(16px) matches all cells
exactly. Resvg also omits the individual resource translations. These share
the resource transform entry path, independently of percentage transform
bases and of the position of a use inside a clip.

The curve follows [curved-shape bounding boxes](../../dev/spec/svg2/coords.md#BoundingBoxes):
the box must include interior extrema, not just segment endpoints.
For s=1e-20 the curve is x=s*(3*t*t-2*t*t*t), y=3*s*t*(1-t), so its
height is 3*s/4. Scaling makes it 32px wide and 24px high; the clipped
16px-by-8px rectangle lies wholly between the curve and its closing line.
Replacing only the gradient with solid lime matches exactly, as does
normalizing s to 1 and the scale to 32 while retaining the gradient.
Resvg renders the original exactly. This exercises the scale-dependent
extremum threshold, independently of gradient coordinate precision and
gradient normalization underflow.

The missing components follow [interpolation with missing values](../../dev/spec/css-color-4.md#interpolation-missing).
Alpha is analogous to itself in every interpolation space; all missing
color components are also carried forward together as an analogous set.
Each first stop therefore borrows its missing values from opaque white,
making both gradients constant opaque white. Replacing none alpha by 1
and the all-missing color by white matches both rectangles exactly.
This tests preservation of missing-component metadata during interpolation,
independently of gamut mapping and of an inherited currentColor keyword.

The preferred set follows [SVG style processing](../../dev/spec/svg2/styling.md#StyleElementTitleAttribute),
the HTML [style-block algorithm](https://html.spec.whatwg.org/multipage/semantics.html#update-a-style-block)
and [adding a CSS sheet](https://drafts.csswg.org/cssom/#add-a-css-style-sheet).
An unsupported type returns before creating a sheet and cannot select the
preferred CSS set. Removing only the text/plain element's title, or giving
the valid sheet that same title, makes both controls green exactly.
Resvg also renders the original green. This exercises selection of which
sheets participate, separately from parsing their rules and declarations.

The natural sizes follow [the intrinsic aspect-ratio algorithm](../../dev/spec/svg2/coords.md#SizingSVGInCSS)
and [fragment view overrides](../../dev/spec/svg2/linking.md#SVGFragmentIdentifiersDefinitions).
With only one absolute root dimension, an active SVG view supplies the
ratio before the root viewBox does. Both fragment forms therefore give a
16px-by-8px image at the requested x/y. Adding the implied height=8 to
both source roots matches exactly; replacing their square root viewBox
with the selected 32-by-16 viewBox also matches exactly. Resvg does not
match the original natural-size case. This exercises dimension inference
before view selection, independently of discarding a view's aspect-ratio
parameter while drawing an image of a fixed size.

The depth order follows [3D rendering contexts](../../dev/spec/css-transforms-2.md#3d-rendering-contexts):
planes are sorted by their transformed z positions, with painting order
used only for coplanar elements. No perspective changes the rectangles'
x/y positions here. Moving the unrelated blue corner from z=1e8 to z=2
matches the literal drawing exactly; keeping it distant and raising the
green square from z=1 to z=1024 also matches exactly. The original depths
are distinct and exactly representable. This exercises a scene-wide
tolerance affecting other planes, independently of curve-extremum bounds.

The projective layer follows [marker rendering](../../dev/spec/svg2/painting.md#RenderingMarkers)
and [transformed painting](../../dev/spec/css-transforms-1.md#transform-rendering).
Before projection the marker occupies x=8..56, y=16..40. The matrix maps
(x,y) to (x,y)/(1+x/100); the complete sampling clip x=24..32, y=16..24
lies inside the projected rectangle, clear of its edges. Replacing the
marker path with that literal source rectangle matches exactly. Retaining
the path and marker while adding an unpainted subpath that enlarges only
the measured geometry is another exact control. This tests a layer's
paint extent, independently of filter capture and of marker visibility
or direction.

The horizon follows [processing perspective-transformed boxes](../../dev/spec/css-transforms-2.md#processing-of-perspective-transformed-boxes):
only the part with negative homogeneous w is removed when some corners
remain in front. Here w=1-x/16, so the source's x=8 edge is in front and
its x=40 edge is behind. Inverting the projection maps the sampling square
to source coordinates strictly inside x=8..12, y=8..24. Shortening the
source to width 4 therefore matches the green expectation exactly.
Splitting the original rectangle into eight adjacent 4px-wide rectangles,
each with the same transform, is another exact control retaining the
complete source geometry. This exercises the midpoint rejection,
independently of allocating enough room for marker paint.

The calculated blur follows [numeric-function range checking](../../dev/spec/css-values-4.md#functional-numeric),
[blur's nonnegative range](../../dev/spec/filter-effects-1.md#funcdef-filter-blur)
and the project's D30 rule. The math result clamps to zero without
invalidating the declaration. Both first functions then leave the source
unchanged, including the shadow's transparent color, and invert remains
active. Replacing the two calculations with 0px matches exactly, as does
removing the identity blur and transparent shadow while retaining invert.
This exercises valid math at a restricted length argument, separately from
invalid literal CSS declarations and malformed SVG deviation attributes.

The dash follows [dash offset normalization](../../dev/spec/svg2/painting.md#StrokeDashoffsetProperty)
and [dash positions](../../dev/spec/svg2/painting.md#TermDashPositions).
In output coordinates the line is 16px long, each dash and gap is 32px,
and the negative offset is -24px. Its normalized phase is 40px in a 64px
period, leaving 24px of gap before any dash, so the drawing is empty.
Replacing only the offset with the equivalent positive 2.5e38 completes
and matches exactly. Normalizing the line, width, pattern and offset to
16, 8, 32 32 and -24 at unit scale also matches exactly. The original
uses one short segment and two finite dash entries, without exhausting
any geometry budget. Two independently timed runs hung before the oracle
also reached its existing renderer timeout. This exercises overflow of
the period and nontermination during phase normalization, independently
of finite point-coordinate sums and closing joins.

The use follows [its supplemental x/y transformation](../../dev/spec/svg2/struct.md#UseLayout)
and [vector-effect calculation](../../dev/spec/svg2/coords.md#VectorEffectsCalculation).
The x/y translation belongs to the use's CTM, so scale(2) puts its origin
at (16,16). Non-scaling-size removes scale from the linear part while
preserving that origin. Replacing x/y by transform="translate(8 8)"
matches the literal expectation exactly. Removing the parent scale and
using x=y=16 is another exact control. At the declared raster size the
project's D3 output-pixel convention gives the same result. This tests
the order of constrained transforms and use positioning, separately from
the matrix supplied to a nested clip on a clipPath child.

The determinant uses the same [vector-effect formula](../../dev/spec/svg2/coords.md#VectorEffectsCalculation).
For a positive uniform scale s, the linear part s*I divided by
sqrt(s*s) is I; the translations remain (8,8) and (40,8). The two
determinants, approximately 1e48 and 1e-48, are finite and nonzero in the
CTM's double precision. Replacing both scales with 2 matches exactly,
as does replacing both with 0.5 while keeping non-scaling-size. Neither
case allocates a large raster or expands geometry: the constrained result
is the same pair of small squares. This exercises the numeric range of
the constrained matrix, independently of the use-position ordering.

The seed follows [the published turbulence algorithm](../../dev/spec/filter-effects-1.md#elementdef-feturbulence)
and [seed truncation](../../dev/spec/filter-effects-1.md#element-attrdef-feturbulence-seed).
The exactly represented -2^32 is already integral. Its remainder modulo
2^31-2 is -4, so setup_seed returns 5. An independent evaluation of the
published lattice algorithm gives green values 0.5546..0.5739 at all 64
pixel centers, safely above the discrete transfer's 0.5 threshold. The
other channels are zero and alpha is one, giving the literal lime square.
The power-of-two frequency also makes the sampled lattice coordinates
exact. Replacing the seed with 5 matches exactly; replacing it with -4
is another exact control. The renderer instead clamps the original to
-2^31, normalizes that to 3, and obtains green 0.4501..0.4910, below the
threshold. This uses the same uniform-gradient algorithm chosen in D25,
independently of bounding-box coordinate conversion.

The mesh origins follow [the retained mesh-gradient definition](https://www.w3.org/TR/2016/CR-SVG2-20160915/pservers.html#MeshGradients)
selected in D57 and [font-relative lengths](../../dev/spec/css-values-4.md#font-relative-lengths).
The first patch starts at x=32 and spans 16px. The second uses the default
objectBoundingBox units: x=50% is .5 in box coordinates, so its .5-wide
patch covers x=24..40 in the 32px-wide referencing rectangle. All corners
are lime, making both patches constant lime. Replacing the origins with
32 and .5 matches both cells exactly. Normalizing the first mesh's font
to 16px and its x to 2em also matches when the second origin is .5.
This tests the starting-point length context, independently of interpolation,
curve subdivision and resource transform construction.

The marker follows [marker units](../../dev/spec/svg2/painting.md#MarkerUnitsAttribute)
and [automatic orientation](../../dev/spec/svg2/painting.md#RenderingMarkers).
The non-scaling-stroke adjustment affects strokeWidth marker units;
userSpaceOnUse keeps the path's user coordinate system. The path's end
direction is 45 degrees there. Relative to the marker origin (40,16),
inverse scaling maps the sampling rectangle to a=0..8, b=36..44. Inverse
45-degree rotation gives u=(a+b)/sqrt(2), v=(b-a)/sqrt(2), wholly inside
the 64-by-32 marker. The renderer instead rotates by atan(1/4) before
applying scale(4,1) again, putting the whole sampling region outside the
marker. Setting orient=45 matches exactly. Removing non-scaling-stroke
also matches exactly, leaving the unstroked path and marker unchanged.
This tests the coordinate system of automatic orientation, independently
of choosing a direction across zero-length segments.

The percentage follows [stroke-width](../../dev/spec/svg2/painting.md#StrokeWidthProperty)
and [the normalized viewport diagonal](../../dev/spec/svg2/coords.md#Units_viewport_percentage).
For each square viewBox of side s, sqrt((s*s+s*s)/2) equals s, and
25% maps to 16 pixels in the 64px viewport. Both center lines therefore
paint the literal bands y=32..48. Replacing their widths by 2.5e19 and
2.5e-25 matches exactly. Normalizing both viewBoxes to 0 0 64 64 and
both paths to M0 32H64 while retaining 25% also matches exactly. The
dimensions and intended widths are finite and representable; the paths
contain one straight segment each. This tests the shared percentage
length basis, independently of gradient normalization, vector-effect
determinants and finite stroke-coordinate rejection.

The dash follows [numeric-function range checking](../../dev/spec/css-values-4.md#functional-numeric),
[nonnegative dash lengths](../../dev/spec/svg2/painting.md#StrokeDasharrayProperty)
and the project's D30 rule. The valid calculation clamps to zero, making
the pattern 0 8 8 8 with period 24. With butt caps the zero-length on
entries paint nothing, leaving x=16..24 and x=40..48. Replacing the
calculation by 0 matches exactly. Merging the gaps into pattern 8 16
with offset 16 is another exact control. This exercises loss of the
range clamp between validation and style construction, independently of
rejecting a calculated filter argument, period overflow and closing joins.

The minified masks follow [repeating-gradient averaging](../../dev/spec/css-images-3.md#repeating-gradients),
[the average-color algorithm](../../dev/spec/css-images-3.md#gradient-average-color)
and [repeating conic gradients](https://drafts.csswg.org/css-images-4/#repeating-gradients).
The radial period is 1/1024px. The conic period is 1/2^20 of a turn,
less than .00014px even at a square's farthest corner. Neither can be
resolved in the declared raster. Each period interpolates from transparent
to opaque white, whose premultiplied average is white with alpha .5.
Collapsing both periods to zero matches the literal expectation exactly;
replacing each mask by linear-gradient(#ffffff80,#ffffff80) also matches
within the ordinary tolerance. A linear gradient with the same subpixel
radial-length period already averages correctly. This tests missing
minification handling for nonlinear gradients, independently of a large
conic starting angle and fractional image-tile placement.

The explicit stop follows [color-stop fixup](../../dev/spec/css-images-3.md#color-stop-fixup).
Its finite negative position is less than the preceding 0px position, so
it must move to 0px before interpolation. All positive sample coordinates
therefore see opaque white. Replacing -1e32px with 0px matches exactly;
replacing it with -1px, which requires the same fixup, also matches exactly.
The renderer instead treats this positioned stop as unplaced and distributes
it to 16px. This exercises the finite sentinel used for absent positions,
independently of counting transition hints during unplaced-stop distribution
and clamping an already-built radial gradient at its center.

The selector follows [CSS input preprocessing](../../dev/spec/css-syntax-3.md#input-preprocessing)
and [string-token consumption](../../dev/spec/css-syntax-3.md#consume-string-token).
XML's character reference supplies U+000D to the style text. CSS changes
that CR to LF before tokenization; a backslash followed by that newline
continues the string without adding a character, yielding lime. Replacing
the character reference with `&#10;` matches exactly, as does joining the
string directly by removing its backslash and CR reference. Resvg shares
the original failure. This tests normalization before CSS escapes are read,
independently of decoding escapes in filter attribute keywords.

The default follows [XML attribute defaults](https://www.w3.org/TR/xml/#sec-attr-defaults)
and [non-validating processor requirements](https://www.w3.org/TR/xml/#proc-types).
The declaration is in the document's internal subset, without external or
parameter entities. Even a non-validating processor must supply its default
fill when the rectangle omits that attribute. An explicit lime fill on
the rectangle matches exactly, as does an explicit lime fill on its SVG
parent. Python's independent XML parser also reports the missing fill as
lime. Resvg shares the original failure. This exercises internal attribute
declarations, independently of entity expansion and CSS declaration defaults.

The number follows [numeric token consumption](../../dev/spec/css-syntax-3.md#consume-number)
and [SVG numbers](../../dev/spec/svg2/types.md#syntax).
Its mantissa is 16 times 10^308 and its exponent is -308, so the complete
value is exactly 16, comfortably within the supported numeric range.
The few hundred spelling characters do not create additional geometry.
Replacing the width by 16 matches exactly, as does replacing it by 1.6e1.
Python's decimal parser independently confirms the complete value is 16.
This tests intermediate overflow while decoding one numeric spelling,
independently of arithmetic on already-parsed coordinates and coefficients.

The exponent follows [feSpecularLighting's numeric attribute](../../dev/spec/filter-effects-1.md#element-attrdef-fespecularlighting-specularexponent)
and [its Phong model](../../dev/spec/filter-effects-1.md#elementdef-fespecularlighting).
Filter Effects gives this exponent the number grammar without the old
1..128 range. With surfaceScale=0 the normal is (0,0,1) everywhere.
At light elevation 0, N.H=cos(45deg) and exponent 0 gives intensity 1.
At elevation 80, N.H=cos(5deg) and exponent 256 gives intensity
.37680951468734586. Lime light makes alpha equal to that intensity.
The renderer instead uses exponents 1 and 128, giving alpha 180 and 157
out of 255. Replacing the first exponent by 1 with constant sqrt(2),
and the second by 128 with constant cos(5deg)^128, matches exactly.
Using exponent 1 with constant cos(5deg)^255 for the second is another
exact control. This tests the retained exponent restriction, independently
of spot-light focus, vector normalization and lighting subregion sampling.

The constants follow [diffuseConstant's nonnegative range](../../dev/spec/filter-effects-1.md#element-attrdef-fediffuselighting-diffuseconstant),
[specularConstant's nonnegative range](../../dev/spec/filter-effects-1.md#element-attrdef-fespecularlighting-specularconstant)
and [invalid nonpresentation attribute handling](../../dev/spec/svg2/types.md#syntax),
as recorded in D16. These literal negative values are invalid and take
their initial value 1. With zero surface scale and a vertical distant
light, both N.L and N.H are 1, making both outputs opaque lime.
Replacing each negative constant by 1 matches exactly; removing both
attributes also matches exactly. This exercises range-error recovery for
lighting constants, independently of valid specular exponents and of a
drop shadow's malformed deviation syntax.

The alpha ramp follows [premultiplied gradient interpolation](../../dev/spec/css-images-3.md#premultiplied).
For transparent red and opaque blue, the premultiplied interpolation is
(0,0,t,t). At each pixel center t=(x+.5)/16, its color over white is
(1-t,1-t,1), which the literal one-pixel blue strips independently draw.
The first column should be (247,247,255); it is (251,247,251).
Recoloring only the fully transparent stop to blue matches exactly.
An SVG linear gradient with blue at both stops and alpha 0 to 1, drawn
over a literal white rectangle, is another exact control. This exercises
the extra straight-color interpolation between sampled alpha stops,
independently of missing components, gamut mapping and minification.

Group additional manifestations of an existing cause into its existing
case instead of adding another top-level failure for the same cause.
