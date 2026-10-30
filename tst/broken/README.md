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
| `luminance-to-alpha-coefficients.svg` | Green's luminance alpha is .7152; the following transfer makes the square opaque lime. | All 256 expected pixels disappear. | `filterColorMatrix` uses the obsolete .2125/.7154/.0721 luminanceToAlpha weights instead of Filter Effects' .2126/.7152/.0722. |
| `image-uri-leading-space-view.svg` | URL parsing removes the surrounding spaces before selecting the embedded SVG view, retaining the green square. | All 256 expected pixels disappear. | `loadImageFrom` checks the untrimmed reference before splitting data-URI fragments; `decodeDataUri` trims later and leaves #square inside the XML body. |
| `clip-repeated-geometry-box.svg` | The extra geometry box makes the clip value invalid, leaving the square unclipped. | All 256 expected pixels disappear. | `parseBasicShape` tracks whether it has read a shape function but accepts unlimited geometry boxes, with the last one replacing the previous box. |
| `display-inherit-pattern.svg` | Explicitly inherited display none excludes the pattern's rectangle. | All 256 painted pixels are extra. | `computeStyle` sets `displayNone` false for every CSS-wide keyword instead of copying the inherited value. |
| `presentation-escaped-variable-function.svg` | The escaped var function substitutes lime into the fill presentation attribute. | The 256-pixel square is black. | `valueSubstitutes`' raw-string precheck misses an escaped function name, so the variable substitution path is skipped. |
| `custom-property-substituted-defaulting.svg` | Substituted initial resets the custom property and activates lime's fallback; substituted unset inherits lime. | Both squares are black, 512 wrong pixels. | `substituteCustom` stores substituted CSS-wide keywords as token values without applying the custom property's defaulting rules. |
| `convolve-default-divisor-tolerance.svg` | The nonzero kernel sum 2^-20 divides the same constant-color convolution sum, preserving 50%-opaque lime. | All 256 expected pixels disappear. | `filterConvolve` replaces a nonzero default divisor by 1 whenever the sum is within 2^-20 of the kernel's magnitude. |
| `mesh-implied-closing-endpoint.svg` | The final cubic closes the mesh patch at its initial corner, retaining the lime square. | All 256 expected pixels disappear. | `meshEdge` always requires six cubic arguments; `meshPaint` does not supply the already known closing endpoint. |
| `mesh-default-stop-color.svg` | Unstyled mesh stops use opaque black, retaining the square. | All 256 expected pixels disappear. | `meshPaint` zeroes its corner colors and reads a stop's computed color only when stop-color or stop-opacity has an explicit declaration. |
| `mesh-linear-light-interpolation.svg` | Interpolating black and white in linearRGB gives an sRGB ramp of 49..251. | All 256 ramp pixels are too dark, by up to 73 levels. | `meshPaint` interpolates the stops' sRGB components without consulting the mesh's color-interpolation property. |
| `css-gradient-hint-interpolation-resolution.svg` | The late hint keeps the sampled square opaque; the alpha transfer retains lime. | All 256 square pixels lose opacity, by up to 255 levels. | `cssStops` approximates the hint curve with 64 equal color steps, then linearly interpolates the almost full-length first interval, introducing opacity loss far before the transition. |
| `path-comma-before-command.svg` | The comma before the next command is an error; the complete prefix is one unfilled horizontal line. | 256 extra opaque pixels from the following square. | `parsePathData` consumes a comma after the last numeric argument without checking that another argument follows. |
| `xml-forbidden-character-reference.svg` | The embedded XML 1.0 resource is not well formed because it references U+0001, so the image is empty. | 256 extra opaque pixels from the invalid image. | `characterReference` rejects zero and surrogates but accepts other code points outside XML 1.0's Char production. |
| `image-invalid-size-auto.svg` | The invalid width presentation attribute is ignored; initial auto uses the image's 16px natural width. | All 256 expected pixels disappear. | `isAuto` tests the raw attribute's presence and literal keyword; `imageRect` then treats its invalid length as an explicit zero size. |
| `projective-homogeneous-weight-range.svg` | Multiplying every homogeneous matrix coefficient by the same positive constant preserves the opaque projected interior. | All 64 sample pixels lose alpha, from 255 to 2. | `lowerOnLayer` narrows the finite double homogeneous weight 5e38 to float infinity, making its sampling scale 1/16 and its layer 1x1. |
| `stroke-segment-length-range.svg` | The two opposite segments and bevel joins cover the same 32x8 band as the open segment. | All 256 expected pixels disappear. | `strokePolyline` measures a finite 4e38 segment in float; infinite lengths then pass its inner-cut checks and corrupt the closed outline. |
| `use-instance-measurement.svg` | The use's custom property makes the instanced rectangle 16px square; its half-width box clip keeps 8x16 pixels. | Only a 4x8 piece remains, 96 missing pixels. | `elementExtent` enters a use target without the instance style map that `lowerUse` establishes for drawing. |
| `distant-light-angle-range.svg` | Elevation 2^127 degrees is equivalent to 128 degrees and illuminates the flat square with green 201. | All 256 pixels are opaque black. | `filterLighting` multiplies the finite float angle by pi before dividing by 180, overflowing its radian direction. |
| `pattern-resolution-norm-underflow.svg` | A 2^104 tile under scale 2^-100 occupies 16px and retains its 8px green/blue stripes. | All 512 pixels become the same translucent mixed color. | `patternPaint` passes a double squared CTM norm of 2^-200 to sqrtf, losing it before choosing the tile's raster resolution. |
| `xml-expanded-attribute-duplicate.svg` | Two attributes with the same namespace URI and local name invalidate the embedded SVG. | The invalid image draws a green square, 256 extra pixels. | `parseElement` checks raw attribute names for duplicates before resolving their namespace prefixes. |
| `paint-server-empty-attribute.svg` | An invalid but present pattern width uses initial zero rather than the template's width. | The zero-width pattern draws, adding 256 pixels. | `patternAttribute` treats a blank attribute as absent and continues to the template. |
| `object-fit-aspect-ratio-range.svg` | Containment preserves a 16px square in both finite image boxes, centered in the wider second box. | The first square disappears; the second fills 32px instead of 16px, 512 wrong pixels. | `lowerImage` divides finite natural dimensions in float; the ratio becomes infinity or zero before containment. |
| `css-radial-degenerate-percent-stops.svg` | Percentage stops of a zero-width ellipse resolve to 0px, leaving the final transparent color everywhere off its center line. | All 256 pixels of the square remain nearly opaque green. | `cssGradientPaint` uses its finite substitute radius for percentage stop positions instead of the mandated zero basis. |
| `mask-border-small-width-shrink.svg` | Four 4px mask borders fit a 16px area and leave its 8px opaque center. | The borders shrink to about .012px, adding 192 pixels around the center. | `borderMaskLayer` floors both opposite-width sums at 1e-30 before deciding whether the widths overlap. |
| `stroke-square-cap-cull-reach.svg` | The offscreen bevel join stays invisible; the real square dash cap covers the lower 8px square. | 76 pixels appear at the split join and all 64 cap pixels disappear. | `strokePath` and `strokeOutside` use half-width times the miter limit as culling reach, omitting square caps' diagonal reach. |
| `basic-shape-translated-origin.svg` | A zero inset preserves the rectangle's complete fill box after its large origin cancels. | All 256 expected pixels disappear. | `shapeClip` narrows the box's absolute inset endpoints to float before the CTM translation cancels, collapsing their 16px separation. |
| `stroke-near-reversal-miter.svg` | The acute miter is below its limit and completely covers the sampled 8x16 rectangle. | All 128 expected pixels disappear. | `addJoin` rounds the incoming/outgoing dot product to -1 before computing the half-angle cosine, treating a finite miter as an exact reversal. |
| `path-length-calibration-overflow.svg` | Each dash and gap is half the authored pathLength, so each spans 24px of the real 48px line. | The gap paints solid green, 192 extra pixels. | `authorDashes` overflows total/author in float before multiplying by the small dash lengths. |
| `nonviewport-overflow-clip-3d.svg` | Overflow clip on a non-viewport group preserves depth order, keeping the green front plane visible. | The farther red plane paints over all 256 pixels. | `preservesThreeD` treats every non-visible overflow as grouping, extending D38's viewport exception to an ordinary g. |
| `gamma-zero-amplitude-overflow.svg` | With positive input and zero amplitude, the gamma function returns its offset 1. | All 256 green pixels become opaque black. | `transfer` overflows powf before multiplying by zero, turning a finite constant result into NaN. |
| `filter-pixel-box-epsilon.svg` | The narrow flood contributes positive alpha to one pixel column; the following gamma makes it opaque. | All 16 expected pixels disappear. | `pixelBox` subtracts 1e-4 before rounding the upper side, making a positive region next to an integer edge empty. |
| `filter-transform-scale-range.svg` | Both reciprocal coordinate scales preserve a one-pixel convolution tap and a 16px square shifted left by one. | The upper square does not move; the lower square disappears, 288 wrong pixels. | `applyFilter` computes CTM column lengths with sqrtf, losing finite squared norms to zero or infinity before kernel-unit conversion. |
| `css-scope-stylesheet-root.svg` | Without an explicit scoping root, :scope matches the SVG document root and selects its child rectangle. | All 256 pixels remain red. | `simplePseudo` rejects scope; Test::Scope exists only as a synthetic anchor for relative selectors. |
| `oklab-gamut-lightness-roundtrip.svg` | Gamut mapping preserves lightness .5 and hue 0, reducing chroma to the same #bb005e boundary color. | The first square becomes white and the second black, 512 wrong pixels. | `parseFunctionColor` converts to sRGB before `gamutMap` reconstructs Oklab, losing the source lightness through cancellation at large chroma. |
| `xinclude-text-encoding.svg` | The UTF-16 stylesheet, or the identical fallback if that encoding is unavailable, makes the rectangle lime. | All 256 pixels remain red. | `include` ignores encoding, appends the fetched bytes as text and reports success, suppressing fallback. |
| `css-transform-function-case.svg` | CSS TRANSLATE moves the square by (8,8), like lowercase translate. | The square stays at the origin, 384 wrong pixels. | `cssTransformArguments` and transform dispatch compare function names case-sensitively. |
| `import-layer-after-invalid-at-rule.svg` | An unknown rule is ignored; the following import declares layer a before b, so b's lime fill wins. | All 256 pixels remain red. | `parseRules` ends the import prefix on an unknown at-rule, skipping the following import's layer registration. |
| `use-paint-server-style-cache.svg` | Each use instance's local gradient inherits its own --paint value, lime then blue. | The lower 256 pixels reuse the first instance's lime color. | `documentStyle` caches by source node index alone, reusing a resource style across different instance maps. |
| `mask-border-finite-middle-scale.svg` | The middle image inherits the finite side scale, preserving the source's opaque left half throughout the 16px mask. | The central 8x8 region becomes uniformly half opaque, 64 wrong pixels. | `borderMaskLayer`'s usable predicate treats every scale at or above 1e30 as infinity and substitutes an unscaled middle tile. |
| `root-filter-operating-frame.svg` | The uniform displacement shifts the source by (-8,-8) before the root's quarter turn, placing it at (48,0). | The square appears at (32,0), 512 wrong pixels. | `lowerElementIn` exempts the root from the complex filter transform path, so displacement runs along device axes. |
| `use-instance-geometry-cascade.svg` | The cloned rectangle's width and fill both resolve variables inherited from its use host, making a 16px lime square. | Width retains the source's 8px fallback, 128 missing pixels. | `cascadeNode` skips geometry declarations at an instance boundary; the instance map contains only non-geometric properties. |
| `conic-math-percent-basis.svg` | In a conic stop, calc(25% + 90deg) is 180deg, retaining exactly the rectangle's right half. | The stop becomes 115deg, 197 missing pixels. | `cssAngle` uses context-free `readMathQuantity`: type addition infers an angle but the percentage's numeric value is never converted against a full turn. |
| `stroke-bbox-growth-overflow.svg` | The finite stroke box contains the straight line's whole 32x16 stroke, so a zero inset preserves it. | All 512 pixels disappear. | `strokeGrowth` overflows half-width times miter limit in float, making the stroke box infinite before clipping uses it. |
| `dash-cubic-translated-cut.svg` | The sampled rectangle lies strictly inside the first 8px dash gap, including room for the thick curve's normals. | All 48 sample pixels paint lime. | `dashPolyline` narrows a cubic's absolute cut point to float before a later transform cancels its large origin, moving the next dash's start backward. |
| `three-d-plane-normal-range.svg` | Reciprocal xy scales cancel; the green plane remains in front of the red one. | All 256 pixels become red. | `polygonPlane` overflows the sum of squared Newell normal components in double, normalizing the finite plane to zero and treating both depths as coplanar. |
| `resource-singularity-context.svg` | The gradient's own 32px font makes its scale calculation equal one, so the two lime stops fill the square. | All 256 pixels use the red fallback. | `singularTransform` calls `flatTransform` without the resource's length context, evaluating em at 16px and falsely rejecting an invertible transform. |
| `marker-tangent-coordinate-range.svg` | The finite tangent has direction (4/5,3/5), and the auto-oriented marker completely covers the sample. | All 16 sample pixels disappear. | `angleOf` passes the double tangent components to atan2f, narrowing both finite coordinate differences to infinity and choosing 45 degrees. |
| `raster-edge-rebase-precision.svg` | The sample lies strictly above the triangle's y=x/2 diagonal and remains fully opaque. | The diagonal shifts to y=x/2+12, leaving 52 pixels missing or partly transparent. | `bandEdge` subtracts the cropped raster's origin from large float endpoints in float, losing that translation before the edge equations are evaluated. |
| `xml-reserved-prefix-binding.svg` | Rebinding the reserved xml prefix invalidates the embedded SVG, leaving the image empty. | The invalid resource draws all 256 green pixels. | `parseElement` records namespace declarations without validating reserved bindings; `resolvePrefix` silently supplies xml's built-in URI. |
| `xinclude-language-fixup.svg` | The included document's untagged language survives inclusion, so :lang("") paints its square green. | The square inherits French from the include parent and stays red, 256 wrong pixels. | `include` attaches the parsed root to the outer parent without the xml:lang fixup that preserves its source language. |
| `radial-start-circle-containment.svg` | The concentric start radius is strictly smaller than the end radius; pad paints the complete sample lime. | All 256 expected pixels disappear. | `gradientPaint` multiplies fr by 1+1e-6 before the containment check, falsely classifying two distinct growing circles as full overlap. |
| `filter-non-scaling-host-frame.svg` | The zero offset preserves a 16px non-scaling stroke, including the sample at its top. | All 16 sampled stroke pixels disappear. | `lowerFilteredLayer` changes the content's coordinate frame without mapping l.host into the layer, so its non-scaling stroke is scaled and sheared when the layer returns to the output. |
| `blur-duplicate-deviation-cap.svg` | The duplicated black/white step uses the authored deviation 256, giving gray 115..121 in the sample under the permitted three-box approximation. | All 256 pixels are too dark, by up to 12 levels. | `blurBuffer` caps a non-none edge mode's deviations at twice the larger input dimension, changing sigma from 256 to 128. |
| `blur-box-sum-overflow.svg` | Blurring a constant opaque green row with duplicated edges preserves that same constant. | All 65,540 pixels lose opacity, falling from 255 to 33. | `boxPass` accumulates 75,199 full stored channels into u32; their sum 4,928,166,465 wraps modulo 2^32 before normalization. |
| `convolve-spaced-tap-integer-range.svg` | The positive tap is far beyond the input and duplicates its green right edge. | All 256 expected green pixels disappear. | `filterConvolve`'s spacedRow casts a finite 2^31 coordinate to int before edge folding, producing an invalid index and interpolation fraction. |
| `xml-ignored-attribute-entity.svg` | The undeclared entity in a namespaced attribute invalidates the embedded image, even though SVG ignores that attribute. | The invalid resource draws all 256 green pixels. | `parseElement` skips unsupported namespaced attributes before internAttribute can decode and validate their entity references. |
| `object-bbox-span-range.svg` | The finite object-box map composes with scale 2^-124 to fill a 16x8 green rectangle. | All 128 expected pixels disappear. | `gradientPaint` narrows the double box width 2^128 to float infinity before composing its map with the CTM. |
| `filter-infinite-paint-input.svg` | A blur preserves the constant opaque green FillPaint supplied by two identical gradient stops. | All 256 pixels lose alpha, by up to 64 levels. | `filterInputBuffer` samples an infinite paint only over run.canvas; blurBuffer reads transparent pixels beyond that plane, while only PaintType::Solid gets the constant-input shortcut. |
| `dash-negative-phase-cancellation.svg` | The negative offset leaves a 16px initial gap before the 16px green dash. | The gap is filled, adding 128 opaque pixels. | `dashPolyline` adds the finite period 2^30 to phase -2^-12 in float, losing the offset before locating the first dash. |
| `filter-scaled-quarter-turn.svg` | The single convolution tap shifts the square 8px upward along the transformed user x axis. | It shifts 8px left instead, leaving 384 wrong pixels. | `lowerElement` compares absolute CTM off-diagonal coefficients against 1e-6; a quarter turn at scale 2^-20 misses the filter-frame path. |
| `clip-ellipse-single-radius-at.svg` | The invalid single-percentage ellipse is ignored, leaving the green sample uncut. | All 128 expected pixels disappear. | `parseShapeFunction` returns at the position's `at` keyword before rejecting an ellipse with only one radius; its second radius remains closest-side. |
| `transform-singular-z-scale.svg` | The non-invertible scaleZ(0) suppresses the transformed square. | 256 extra opaque green pixels. | `lowerElementIn` reduces the affine 3D matrix to its z=0 plane without checking full-matrix singularity; `affineOf4` yields the identity. |
| `marker-root-conditions.svg` | Conditional processing on the never-rendered marker resource leaves its green contents available for referencing. | All 256 expected pixels disappear. | `lowerElementIn` applies passesConditions to the marker root before its special instance handling; the resource is treated like directly rendered content. |
| `pixelated-magnification-norm-range.svg` | Pixelated scaling retains the green source column across the sampled square. | All 256 pixels turn red. | `rasterPaint` passes a finite squared axis norm 2^130 to sqrtf, producing an infinite sharpX; paintAt's infinite ratio selects texel zero instead of one. |
| `radial-negative-math-radius.svg` | The calculated negative focal radius clamps to zero, preserving the opaque green gradient. | All 256 pixels use the red fallback. | `gradientLength` resolves the radius without nonNegative; `gradientPaint` then applies the literal-negative error branch to the calculated result. |
| `system-language-space-separator.svg` | The single malformed tag fr en does not match the fixed English preference, so the rectangle is absent. | 256 extra opaque green pixels. | `passesConditions` splits systemLanguage on whitespace as well as commas, inventing an English list item. |
| `linear-repeat-phase-precision.svg` | Translation by a whole number of periods preserves alternating 4px red and green bands. | All 256 green pixels turn red. | `linearT` narrows its double result to float before spreadT reduces it modulo one, losing the fractional phase at t near 2^25. |
| `clip-empty-position.svg` | The incomplete at clause invalidates the clip, leaving the green square visible. | All 256 expected pixels disappear. | `parseShapeFunction` calls parsePosition on the empty tail after at; parsePosition accepts zero words as center, producing a zero-radius clip. |
| `xml-multiple-doctypes.svg` | A second document type declaration invalidates the embedded image. | The invalid resource draws all 256 green pixels. | `parseDocument` loops over parseDoctype without remembering that a declaration has already occurred. |
| `xml-comment-double-hyphen.svg` | A double hyphen within a comment invalidates the embedded image. | The invalid resource draws all 256 green pixels. | `parseContent` skips to the comment terminator without checking the comment body for forbidden double hyphens. |
| `context-paint-recursive-frame.svg` | Recursive context-fill retains the outer use's gradient frame, making the translated square entirely lime. | Its left half turns red, 128 wrong pixels. | `contextPaint` clears the inherited context flag, and `lowerUse` replaces the original context matrix with the inner use's translated matrix. |
| `css-linked-sheet-bom.svg` | Decoding the linked UTF-8 stylesheet removes its BOM, allowing rect to match and turn the square lime. | All 256 pixels remain red. | `linkedSheet` keeps decoded URL bytes without stylesheet character decoding; `cascade` sends those bytes to parseSheet, where the BOM becomes part of the first type selector. |
| `clip-position-comma.svg` | A comma invalidates the position, so the clip attribute is ignored and the lime square remains. | All 256 expected pixels disappear. | `parsePosition` reads words through shapeWord, which discards commas; the invalid position becomes center center and applies a zero-radius clip. |
| `transform-perspective-none.svg` | Identity perspective leaves the following 8px translation in effect. | The square stays at the origin, leaving 384 wrong pixels. | `transformFunction` accepts only numbers, dimensions, percentages or math arguments, rejecting the valid none keyword before handling perspective. |
| `transform-origin-repeated-axis.svg` | The invalid pair of horizontal keywords is discarded, retaining the preceding center origin and the green square's position. | The square rotates about (24,8), leaving 320 wrong pixels. | `validGeometry` counts two readOrigin successes without checking their axes; transformOrigin treats the second horizontal keyword as the x coordinate. |
| `dash-stationary-newton.svg` | The only painted dash is a 4x8 green rectangle. | It extends to about 32.71px instead of 4px, leaving 232 wrong pixels. | `lengthParameter` stops after four Newton iterations from a poor initial estimate near a stationary cubic endpoint, without checking the remaining length error. |

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

The luminance follows [the luminanceToAlpha matrix](../../dev/spec/filter-effects-1.md#attr-valuedef-type-luminancetoalpha).
Its current coefficients give alpha .7152 for opaque lime. The next
function computes clamp(7153-10000*alpha,0,1), giving 1; the last matrix
sets RGB to lime while retaining that alpha. The renderer's .7154 gives
0 instead. Replacing the first primitive by its explicit normative matrix
matches exactly. Replacing it by a black flood with opacity .7152 also
matches exactly. Both controls retain the transfer and final matrix, so
the differing luminance coefficients are isolated from arithmetic
cancellation and from the transfer's quantization.

The image URL follows [CSS URL values](../../dev/spec/css-values-4.md#urls)
and [the basic URL parser](https://url.spec.whatwg.org/#concept-basic-url-parser),
which removes leading and trailing spaces before parsing the scheme.
The selected view and root both cover the same white 16px square.
Removing only the surrounding URL spaces matches exactly. Removing only
the identity #square fragment, retaining both spaces, also matches
exactly: decoding already trims the address, but reference classification
does not. This tests normalization before scheme and fragment handling,
independently of choosing the fragment separator, decoding its escapes
and whitespace inside a base64 MIME header.

The clip follows [the clip-path grammar](../../dev/spec/css-masking-1.md#the-clip-path)
and [unordered component combinations](../../dev/spec/css-values-4.md#comb-any).
The shape and geometry box may occur in either order, but this value
supplies the geometry-box component twice. The invalid declaration is
ignored, retaining the initial none and the whole green square. Removing
the declaration matches exactly. Replacing its second box with a second
inset function, another invalid repeated component that the parser already
rejects, also matches exactly. There is no earlier valid declaration to
lose here: this exercises acceptance of repeated shape-grammar components,
independently of validating a declaration before it enters the cascade.

The pattern follows [display on graphical effects and their children](../../dev/spec/svg2/render.md#RenderingTree)
and [explicit inheritance](../../dev/spec/css-cascade-5.md#inherit).
The pattern itself remains usable with display none. Its rectangle explicitly
inherits that computed value, so the rectangle contributes no pixels.
Replacing only the child's inherit by none matches the empty expectation
exactly. Keeping inherit and moving display none to a group directly around
the rectangle also matches exactly: normal ancestor suppression conceals
the incorrect inherited value. Resvg independently produces the empty drawing
for the original. This exercises computing an explicit inherited display,
separately from validating invalid declarations and suppressing a root subtree.

The fill follows [variable substitution](../../dev/spec/css-variables-1.md#using-variables)
and [CSS identifier escapes](../../dev/spec/css-syntax-3.md#escaping).
The escaped function name tokenizes as var, and the ancestor supplies lime.
Spelling var literally matches exactly. Moving the same escaped fill value
to a style attribute also matches exactly. The latter control keeps both
escape decoding and variable substitution, isolating the presentation
attribute's raw-text precheck from CSS tokenization and from consumers
that compare resource attribute keywords without decoding them.

The custom properties follow [CSS-wide keywords after property replacement](../../dev/spec/css-values-5.md#resolve-property)
and [custom property initial values](../../dev/spec/css-variables-1.md#guaranteed-invalid-value).
The first replacement produces initial, making --paint guaranteed-invalid;
the fill therefore uses its lime fallback. The second produces unset,
which inherits the ancestor's --paint:lime. Replacing both inner var
expressions with their literal keyword matches exactly. Removing both
custom-property overrides also matches exactly. The inherited branch follows
[WPT's wide-keyword-fallback-002](https://github.com/web-platform-tests/wpt/blob/master/css/css-variables/wide-keyword-fallback-002.html).
This tests defaulting the custom property itself after substitution,
independently of reading escaped function names in presentation attributes.

The convolution follows [the default divisor](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-divisor).
All three coefficients are exactly representable in binary, and sum to
2^-20 rather than zero. The flood is constant across every sampled neighbor,
so dividing its convolution sum by that same 2^-20 retains its color and alpha.
Writing the correct divisor explicitly matches exactly. Multiplying every
kernel coefficient by 1048576, retaining the omitted divisor, also matches
exactly: the sum is now 1, and even replacing that sum by 1 is harmless.
Both controls exercise the same normalized weights (1048576, -1048576, 1),
isolating default-divisor selection from fixed-coefficient cancellation
and quantization. The sample stays eight pixels inside the flood's region.

The closing edge follows the retained [2016 mesh structure](https://www.w3.org/TR/2016/CR-SVG2-20160915/pservers.html#MeshStructure),
within D57. Its omitted endpoint is the initial corner (8,8); both controls
lie on the straight left edge. Appending the explicit endpoint matches
exactly, as does using the equivalent L8 8 edge. This tests closing-edge
syntax independently of mesh origin units.

The unstyled stops follow [stop-color and stop-opacity initial values](../../dev/spec/svg2/pservers.md#StopColorProperty).
Their initial black and opacity 1 make every corner opaque black.
Explicit stop-color black matches exactly. Setting only stop-opacity 1
also matches exactly, retaining the omitted color and the same geometry.
This tests initialization of fresh corner colors independently of parsing
the closing edge; every endpoint here is explicit.

The linear-light mesh follows [color-interpolation](../../dev/spec/svg2/painting.md#ColorInterpolationProperty),
with mesh support retained under D57. For a black-to-white horizontal ramp,
t=(x-8+.5)/16 is linear light; every sampled t exceeds .0031308, so the
independent strips use sRGB=1.055*t^(1/2.4)-.055. An SVG linear gradient
in linearRGB matches exactly. Keeping the mesh and converting its output
with an sRGB-space gamma transfer (amplitude 1.055, exponent 1/2.4,
offset -.055) also matches exactly. Both retain all original stop colors.

The late transition follows [the hint weighting formula](../../dev/spec/css-images-3.md#coloring-gradient-line).
For H=.9999, e=log(.5)/log(H) is about 6931.125. Every point of the
sampled square has P<=56/64, so C=P^e is below 1e-400. Its alpha is
therefore 1 to far better than stored-pixel precision, and the parent's
clamp(100*alpha-99) retains it. The renderer instead synthesizes its first
sample at C=1/64, P about .9994, then interpolates from the original stop
across that whole interval. Replacing only the transparent last color by
white matches exactly. Replacing the nonlinear ramp by white through 99%,
then transparent white at 100%, also matches exactly. Both retain the
mask layout and alpha transfer. All colors have identical white RGB,
separating hint approximation from straight-versus-premultiplied color
interpolation; the hint is strictly inside the stop interval.

The path follows [the path grammar](../../dev/spec/svg2/paths.md#PathDataBNF)
and [prefix error recovery](../../dev/spec/svg2/paths.md#PathDataErrorHandling).
A comma can separate numeric argument groups, but cannot separate complete
commands. The prefix M8 8 L24 8 has no filled area; subsequent commands
must not contribute. Keeping only that prefix matches exactly. Adding a
second comma at the same error also matches exactly, because the parser
already stops at it. This tests separator validation independently of
number ranges and implicit subpaths after close.

The image follows [XML 1.0's legal character constraint](https://www.w3.org/TR/xml/#wf-Legalchar)
and [character range](https://www.w3.org/TR/xml/#charsets).
Its version is explicitly 1.0; U+0001 is forbidden even through a character
reference. SVG data resources [must be well formed](../../dev/spec/svg2/conform.md#ConformingSVGViewers).
Python ElementTree independently rejects the decoded resource.
Replacing the reference by forbidden U+0000 matches the empty expectation
exactly. Replacing it by a surrogate reference also matches exactly;
both already take the invalid-image path. Resvg also leaves the original
image empty. The error precedes the rectangle, and the outer SVG is valid.
This exercises numeric-reference character validation independently of
DTD default attributes and CSS input preprocessing.

The width follows [invalid presentation attributes](../../dev/spec/svg2/types.md#TermInvalidValue),
D16, and [the image's auto size](../../dev/spec/svg2/geometry.md#Sizing).
The resource has explicit 16px width and height; height 16 on the image
retains that ratio when width is auto. Removing the bad attribute matches
exactly, as does replacing its value by auto. Moving the identical bad
width into a style attribute is another exact control: that declaration
is already rejected correctly. Resvg also renders the original as expected.
There is no earlier author declaration to lose. This tests recognizing
auto sizing after an invalid image geometry attribute, independently of
CSS declaration validation and relative math in an embedded root's natural size.

The matrix follows [homogeneous projection](../../dev/spec/css-transforms-2.md#processing-of-perspective-transformed-boxes)
and [matrix3d](../../dev/spec/css-transforms-2.md#funcdef-matrix3d).
Its positive common coefficient cancels when dividing by w. The projected
coordinates are X=256*x/(x+1)-192 and Y=32*y/(x+1). The clipped square
X,Y=4..12 maps strictly inside the source rectangle x=3..5, y=0..2,
so all 64 pixels are opaque. Replacing every 1e38 by 1 matches exactly;
replacing every 1e38 by 1e37 also matches exactly. At the source center,
w=5e38 overflows float although the matrix and projected coordinates
remain finite in doubles. The correct layer is about 62x62, below the
16384-pixel allowance, so no allocation budget is reached. This exercises
the scale calculation independently of layer paint bounds, center rejection,
plane ordering and vector-effect determinant normalization.

The stroke follows [stroke shape](../../dev/spec/svg2/painting.md#StrokeShape)
and [bevel joins](../../dev/spec/svg2/painting.md#LineJoin).
After the complete affine map its centerline runs from (16,16) to (48,16)
and back, with width 8. The two 180-degree bevel joins add no extension,
so its union is the literal rectangle. Splitting both segments at zero
matches exactly, retaining the large endpoints, width, transform and
closed path. Removing the return segment and close also matches exactly.
Normalizing the geometry to -16..16, width 8 and translation (32,16) is
another exact control. Every source point and its x+y are finite; even
the unsplit open segment passes the existing finite-coordinate check.
This isolates segment-length arithmetic and closed-outline cuts from
that check and from dash joins. The repeated endpoint supplies the closing
direction explicitly, avoiding a separate normalization of the implicit edge.

The use follows [shadow-tree style inheritance](../../dev/spec/svg2/struct.md#UseStyleInheritance)
and [the use bounding-box algorithm](../../dev/spec/svg2/coords.md#ContainerElementBoudingBoxComputation).
The target inherits --size from its host, so its font and both em lengths
are 16px. Its object box is therefore 16px square, before the half-width
clip. Replacing the variable expression by literal 16px matches the
expectation exactly. Replacing the box clip by a user-space rectangle of
width 8 and height 16 also matches exactly, retaining the variable and
instance. The original box measurement instead uses the fallback 8px
font. This tests measurement's missing instance map, separately from
the use position's font, transform font lookup, selector boundaries and
feImage's missing instance setup during painting.

The light follows [elevation in degrees](../../dev/spec/filter-effects-1.md#element-attrdef-fedistantlight-elevation)
and [the diffuse-lighting equations](../../dev/spec/filter-effects-1.md#elementdef-fediffuselighting).
The input is the exact finite binary32 value 2^127. Its integer remainder
modulo 360 is 128, so the flat surface receives sin(128 degrees) times
255 = 200.942742, rounding to green 201. Replacing only the elevation by
128 matches exactly. A point light at (-61566147.53256583,0,78801075.36067219)
also matches exactly: its direction over this 16px square differs from
the required unit vector by less than 3e-7. The original multiplication
by pi exceeds the float range before division, although its final radian
value is finite. This exercises degree conversion separately from point
light normalization, specular exponents and CSS conic-gradient sampling.
No transform or output-frame interpretation is involved.

The pattern follows [pattern coordinates](../../dev/spec/svg2/pservers.md#PatternElement)
and [transform rendering](../../dev/spec/css-transforms-1.md#transform-rendering).
All large lengths are exact powers of two: the tile is 2^104 square and
each stripe is 2^103 wide. The scale is 2^-100, leaving a 16px tile and
8px stripes. The squared scale fits double but underflows float, so the
chosen tile is 1x1 instead of 16x16. Normalizing the geometry to 16px tiles
and 8px stripes with unit scale matches exactly. Reducing each length by
2^40 while increasing the scale by 2^40 also matches exactly, retaining
large coordinates and the same output. The correct tile is far below
the 2048px cap. This tests resolution selection independently of pattern
coordinate transforms, content selection and raster minification.

The XML follows [uniqueness of expanded attribute names](https://www.w3.org/TR/xml-names/#uniqAttrs)
and [well-formed image resources](../../dev/spec/svg2/conform.md#ConformingSVGViewers).
Both a:tag and b:tag expand to (urn:meta,tag), which is forbidden even
though their prefixes differ. Python ElementTree independently rejects
the decoded image; resvg also leaves it empty. Renaming b:tag to a:tag
matches the empty expectation exactly, exercising the existing raw-name
check. Removing b's namespace declaration is another exact empty control,
exercising the existing unbound-prefix check. Renaming only b:tag to
b:other makes the resource valid and renders all 256 green pixels exactly.
The error precedes the rectangle. The outer SVG remains well formed.
This tests namespace-aware duplicate detection separately from character
references, namespace-sensitive CSS selectors and DTD attribute defaults.

The pattern follows [template attribute presence](../../dev/spec/svg2/pservers.md#PaintServerTemplates),
[initial values](../../dev/spec/svg2/types.md#TermInitialValue) and
[pattern width](../../dev/spec/svg2/pservers.md#PatternElementWidthAttribute).
Width is a non-presentation attribute on a pattern. Its blank value is
invalid but present, so the initial width zero applies. Only an absent
attribute is taken from the template. Replacing the blank by bad matches
the empty expectation exactly; replacing it by 0 is another exact control.
Replacing it by 16 makes the resource valid and paints the whole green
square. Resvg leaves the original empty. This tests template attribute
presence independently of the separate template child-content rule and
CSS declaration validation.

The images follow [contain sizing](../../dev/spec/css-images-3.md#contain-constraint)
and [object fitting before preserveAspectRatio](../../dev/spec/svg2/embedded.md#ImageElement).
The first source and box are 2^100 by 2^-40. Their ratio is 2^140, but
containment changes neither finite dimension; the outer scale makes a
16px square. The second source is 2^-60 by 2^100; its box is twice that
width. Containment retains the source dimensions and centers them, giving
x=16..32 after the outer scale. Its ratio 2^-160 instead rounds to zero,
which skips object-fit and exposes the explicit preserveAspectRatio=none
fallback. Both roots provide positive absolute dimensions; no natural
size inference or active view is involved. Normalizing each source to
16px square and its image box to 16x16 or 32x16 matches exactly. Bringing
both ratios 2^40 closer to one, while compensating the corresponding
outer scales, also matches exactly. All dimensions remain representable
floats, the final geometry is small, and neither case exhausts a budget.
This isolates the ratio division from raster minification and intrinsic
size or fragment-view selection.

The radial follows [degenerate radial gradients](../../dev/spec/css-images-3.md#degenerate-radials).
A zero-width ellipse makes every percentage stop position resolve to 0px.
Thus the white and transparent stops coincide at zero; the non-repeating
gradient takes its last transparent color away from the center line,
which has no pixel area. Replacing 100% by 0px matches exactly, retaining
the degenerate ellipse and magnification. Normalizing the rectangle to
16px with unit scale is another exact control. The implementation's
substitute radius 1/1024 becomes 1024px after this transform, and its
incorrectly retained percentage interval keeps the square opaque.
This tests the required zero percentage basis, independently of
negative radial stops, implicit-circle grammar and hint interpolation.

The border follows [mask-border widths](../../dev/spec/css-masking-1.md#the-mask-border-width),
[proportional overlap reduction](../../dev/spec/css-backgrounds-3.md#border-image-width)
and [the opaque unsliced center](../../dev/spec/css-masking-1.md#the-mask-border-slice).
The image is a valid transparent 4px SVG sliced one unit from each edge;
without fill its middle becomes opaque white. The geometry is a 2^-108
square with 2^-110 border widths under scale 2^112. In device pixels,
opposite widths total 8 inside an area of 16, so the reduction factor
must remain 1. Normalizing to a 16px square with 4px widths and unit
scale matches exactly. Multiplying local geometry and widths by 2^32
while dividing the scale by 2^32 is another exact control. All original
lengths are positive normal binary32 values. The denominator floor alone
causes a false reduction, independently of no-clip mask bounds, image
ratios, pattern resolution and raster minification.

The strokes follow [square caps](../../dev/spec/svg2/painting.md#LineCaps)
and [bevel joins and miter limits](../../dev/spec/svg2/painting.md#LineJoin).
The upper centerline stays at x<=-50; its 40px half-width and bevel join
stay left of x=-10, and both end caps are farther away. It paints nothing
on the canvas. Splitting at that offscreen join instead creates square
caps that reach into the canvas. The lower line's first dash exceeds its
whole length, so its real end cap remains. Its half-width is 160; relative
to the endpoint (-200,52), all four corners of the sampled 8px square
have along components between 140 and 153 and perpendicular components
between -159 and -147, strictly inside that square cap. The expected
square is therefore opaque. The half-width-only rejection instead drops
the dash. Changing only miterlimit to 4 matches both cells exactly; this
property does not affect bevel joins. Closing the upper path and removing
the lower line's noninterrupting dash pattern is another exact control.
Resvg matches the original exactly. Both culling branches share the same
underestimated cap reach, so they are grouped in one case, separately
from closed-dash joins and segment-length overflow.

The inset follows [inset geometry](../../dev/spec/css-shapes-1.md#funcdef-basic-shape-inset)
and [SVG clip reference boxes](../../dev/spec/css-masking-1.md#the-clip-path).
The fill box is a 16px square at the exact binary32 origin 2^30. Both
origin translations are exact powers of two; their double CTM leaves
the square at (8,8). A zero inset is the complete unchanged fill box.
Removing only the clip matches exactly. Replacing inset(0) by
path('M0 0H16V16H0Z') also matches exactly, retaining both the clipping
operation and original geometry. Moving the source origin and its
compensating translation to zero is another exact control. This isolates
absolute-coordinate narrowing in the basic inset clip from gradient
coordinates, transform-origin units and geometric bounding-box extrema.

The miter follows [miter length and its limit](../../dev/spec/svg2/painting.md#StrokeMiterlimitProperty).
The angle between the legs is atan(.0001); its miter ratio is about
20000, below the declared 1000000 limit. The outer miter lies between
y=0 and the sloping edge y approximately 128-6.4*x. The complete sample
x=8..16, y=8..24 is strictly inside, with at least 1.6px of vertical
clearance from the sloping edge. Changing miter to miter-clip matches exactly; below the
limit these join types have the same geometry. Increasing the leg's
rise from .0001 to .01 while increasing the x scale from .001 to .1
also matches exactly, retaining that interior sample. A literal triangle
with vertices (0,0), (20,0), (0,128) is a further exact control over the
sample. Only straight edges are involved. This tests half-angle
cancellation independently of offscreen cap culling, closed-outline
segment lengths and dash joins.

The dashes follow [pathLength calibration](../../dev/spec/svg2/paths.md#PathLengthAttribute)
and [stroke dashing](../../dev/spec/svg2/painting.md#StrokeDashing).
The author length is 2^-124 and each dash/gap is 2^-125: all positive
normal binary32 values. Calibrating against the real length 48 gives
48*(2^-125/2^-124)=24px for each, with one dash followed by one gap.
Replacing the author length by 2 and each dash/gap by 1 matches exactly.
Replacing them by 2^-100 and 2^-101 also matches exactly, retaining tiny
input values. The failed intermediate factor is 48*2^124, although both
used lengths are ordinary finite numbers. The correct path has only two
dash entries and one painted dash; no work or outline budget is reached.
This tests calibration arithmetic independently of dash-period overflow,
negative math ranges and closed-path segment-length overflow.

The group follows [3D depth ordering](../../dev/spec/css-transforms-2.md#3d-rendering-contexts)
and [grouping property values](../../dev/spec/css-transforms-2.md#grouping-property-values),
which explicitly exempt overflow clip. The SVG g establishes no viewport
and adds no overflow clip; the existing D38 exception is specifically for
an SVG viewport clipping in its own plane. The near green rectangle is
at z=10, while red is at z=-10. Replacing clip by visible matches exactly;
removing only the overflow declaration is another exact control. This
tests the scope of overflow-driven flattening, independently of plane
ordering tolerances, perspective clipping and viewport clipping itself.

The transfer follows [the gamma equation](../../dev/spec/filter-effects-1.md#attr-valuedef-type-gamma)
and [the unrestricted exponent attribute](../../dev/spec/filter-effects-1.md#element-attrdef-fecomponenttransfer-exponent).
Every sampled green input is 128/255, strictly positive. Its -256 power
is finite in double precision; multiplying that finite value by zero and
adding one gives exactly one. There is no zero-input power singularity
in the sampled square. Replacing exponent -256 by 0 matches exactly,
retaining the gamma function and all its other attributes. Replacing the
function by linear with slope 0 and intercept 1 is another exact control.
The original powf overflows and 0*infinity becomes NaN, which toUnit16
turns into zero. Resvg shares this failure. This tests the gamma power
intermediate independently of fixed filter-coefficient limits, CSS math
evaluation and specular-light exponent restrictions.

The narrow region follows [filter primitive subregions](../../dev/spec/filter-effects-1.md#FilterPrimitiveSubRegion)
and [gamma transfer](../../dev/spec/filter-effects-1.md#attr-valuedef-type-gamma),
using the existing D2 fractional-area behavior. The flood covers .00009
of each pixel in column 16 for 16 rows. That is about six stored alpha
units, and even one positive 16-bit unit makes 2*alpha^.01 exceed one.
The following transfer therefore saturates this column to opaque green
while transparent pixels remain transparent. Moving only the flood's
x to 16.25 matches exactly, retaining the same pixel coverage. Increasing
its width to .0002 is another exact control, retaining the saturated
column. Resvg matches the original. This tests premature empty storage
bounds rather than a subregion meeting its own hard edge, coefficient
precision or gamma-power overflow.

The scale follows [convolution sampling](../../dev/spec/filter-effects-1.md#elementdef-feconvolvematrix)
and [kernel unit lengths](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength).
The top coordinates and kernel unit are multiplied by 2^100 under CTM
2^-100; the lower ones are multiplied by 2^-100 under CTM 2^100.
Both kernel steps are exactly one device pixel. Reversing the 1,0 kernel
with target 0 reads the pixel to the right, moving the flood left by one.
The output subregion is explicitly wide enough for that shift. Normalizing
both coordinate systems to unit scale matches exactly. Using powers
2^60 and 2^-60 instead, with reciprocal CTMs, also matches exactly.
The intended steps and filter regions are small, all supplied values are
finite, and no work budget is reached. This tests the filter run's unit
conversion independently of pattern raster resolution, percentage
diagonals, vector effects and convolution reach pruning.

The selector follows [the default scope pseudo-class](../../dev/spec/selectors-4.md#scope-pseudo):
without a supplied scoping root it represents the root of the element's
tree, equivalent to root in this ordinary document tree. This is a static
stylesheet rule, with no DOM API, shadow tree or interaction. Replacing
scope by root matches exactly; replacing it by the SVG type selector
also matches exactly. The selector's declarations override the red
presentation attribute. This tests a valid static pseudo-class separately
from malformed pseudo-elements, dynamic pseudo-classes and instance
sibling boundaries.

The colors follow [binary-search gamut mapping with local MINDE](../../dev/spec/css-color-4.md#binary-search-gamut-map-with-local-minde)
and the existing D27 choice to apply it during color parsing. The original
Oklab/OkLCh coordinates already give lightness .5 and hue 0; step 2 must
retain them before reducing chroma. Evaluating the published algorithm
directly from these coordinates, without an intervening sRGB round trip,
gives approximately (.7327203,0,.3687788) and (.7327420,0,.3687772).
Both round to (187,0,94). The color distance calculation compares the
current Oklab coordinates directly with the clipped color converted to
Oklab, so it also retains the original lightness throughout the search.
Replacing both large chroma values by 1 matches the literal expectation
exactly. Replacing both by 10 is another exact control. Both remain
out of gamut and map to the same quantized boundary color. The supplied
components and the double conversion intermediates are finite. This
tests loss of source perceptual coordinates independently of the chosen
gamut-mapping policy, missing color components and alpha interpolation.

The inclusion follows [XInclude text processing and fallback](https://www.w3.org/TR/xinclude/),
sections 4.3 and 4.4. Its binary resource has no external charset;
encoding=UTF-16 determines character decoding, including removal of the
BOM. The decoded stylesheet is rect{fill:lime}. If the encoding is not
supported, that is a resource error and the identical fallback stylesheet
applies instead. Thus the expected drawing does not require support for
every encoding. Replacing the payload with UTF-8 and its encoding with
UTF-8 matches exactly. Replacing only the href with an unavailable resource
also matches exactly through fallback. Only a rectangle is rendered;
included text supplies CSS. This tests resource character decoding before
CSS parsing, separately from CSS input preprocessing and base64 validation.

The translation follows [case-insensitive CSS function names](../../dev/spec/css-values-4.md#functional-notation)
and [the translate function](../../dev/spec/css-transforms-1.md#funcdef-transform-translate).
This is a CSS declaration, so TRANSLATE and translate are equivalent;
the case-sensitive SVG attribute grammar is not used for the input.
Replacing only the function name by lowercase translate matches exactly.
Replacing the declaration by the canonical SVG attribute translate(8 8)
is another exact control. The arguments, origin and size are ordinary
finite values. This tests CSS function-name dispatch independently of
escaped tokens, invalid declaration handling and transform length contexts.

The import follows [the import position and layer assignment rules](../../dev/spec/css-cascade-5.md#at-ruledef-import)
and [ignoring invalid CSS constructs](../../dev/spec/css-syntax-3.md#css-ignored).
Only valid rules end the import prefix; an unknown at-rule is ignored.
The import registers layer a before b even when its sheet never loads,
which is covered independently of import loading under D15. Normal
declarations in the later layer b therefore win. Removing only the
unknown rule matches exactly. Declaring layer a explicitly before the
unknown rule is another exact control. This tests invalid-rule handling
of the layer-order side effect, independently of declaration validation
and stylesheet eligibility.

The resource instances follow [the use-element shadow tree](../../dev/spec/svg2/struct.md#UseShadowTree),
[instance style inheritance](../../dev/spec/svg2/struct.md#UseStyleInheritance)
and [tree-scoped fragment URLs](../../dev/spec/css-values-4.md#local-urls).
The whole source subtree, including defs and gradient stops, is cloned;
each clone inherits its host's custom property. The inline fill URL
resolves to the gradient within that same shadow tree. Both stops have
the same color, so the expected squares need no gradient sampling.
Giving the two uses distinct but otherwise identical source subtrees
and resource IDs matches exactly. Replacing the fill URL with the direct
custom-property color is another exact control, retaining both uses and
their different host values. This tests cache separation of computed
resource styles, independently of omitted instance styles, instance
measurement and paint-server href templating under D44.

The mask follows [mask-border drawing](../../dev/spec/css-masking-1.md#masking-with-the-mask-border-image)
and [scaling the middle border image](../../dev/spec/css-backgrounds-3.md#border-image-process).
The source has intrinsic sides 2^-100 and a 16x16 viewBox. Each 25% slice
is 2^-102, so a 4px border side has scale 2^104, finite even in float.
The middle source side 2^-101 must take that same scale and become 8px;
the fallback to an unscaled middle is only for zero or infinite scales.
There is exactly one tile in each middle region and the opaque half
ends at x=16 throughout the mask. Replacing the source's intrinsic sides
by 16 matches exactly. Using 2^-80 instead also matches exactly, keeping
a large finite scale below the predicate's cutoff. The border widths,
destination bounds and work are ordinary pixel sizes. This tests the
middle-scale predicate independently of opposite-width shrinking,
intrinsic aspect-ratio range and fractional tile placement.

The displacement follows [the displacement equation](../../dev/spec/filter-effects-1.md#elementdef-fedisplacementmap),
[its scale in primitive units](../../dev/spec/filter-effects-1.md#element-attrdef-fedisplacementmap-scale)
and [the operating coordinate space](../../dev/spec/filter-effects-1.md#operating-coordinate-space).
Both selected map channels are exactly one, so sampling at (x+8,y+8)
moves the source rectangle from (8,8) to (0,0) in local coordinates.
The explicitly anchored quarter-turn matrix maps that square to
(48,0)-(64,16). Moving the root's transform and filter onto a group
around the source matches exactly. Replacing the flood and displacement
with feOffset dx=-8 dy=-8 is another exact control on the root itself.
The filter region is the same square under the turn and all samples
and rectangles have integer pixel coordinates. This tests the omitted
root operating-frame conversion independently of filter-scale range,
reach pruning, root viewBox sizing and embedded-root effects.

The geometry follows [cloned inline styles and instance inheritance](../../dev/spec/svg2/struct.md#UseStyleInheritance)
and [custom-property inheritance and substitution](../../dev/spec/css-variables-1.md#defining-variables).
The cloned style attribute is evaluated with the host's size and paint
variables. Only the uninstanced source lacks those variables and uses
8px/red. The actual clone already gets the inherited lime color but
keeps that source width. Setting the size variable to 16px on the source
as well matches exactly. Replacing the width declaration by literal
16px is another exact control, retaining the host-inherited fill.
No bounding-box effect or resource measurement is involved. This tests
the missing instance geometry cascade independently of measurement
without an instance map, selector-visible geometry mutation and escaped
variable detection in presentation attributes.

The stop follows [angular color-stop syntax and placement](https://drafts.csswg.org/css-images-4/#conic-gradients)
and [the angle-percentage type](../../dev/spec/css-values-4.md#typedef-angle-percentage).
Conic percentages resolve against a full 360deg turn, so the 25% term
adds 90deg and the two middle stops coincide at 180deg. The constant
opaque and transparent sectors therefore meet on the downward ray,
leaving precisely the right half of the square. No sampled pixel center
lies on either vertical boundary ray. Replacing both mixed expressions
with calc(180deg) matches exactly. Replacing them with calc(50%) is
another exact control. This tests a missing angle percentage basis in
math evaluation, independently of relative-unit cancellation checks,
whole-turn precision, stop fixup, hints and minification.

The stroke box follows [the stroke-box inflation algorithm](../../dev/spec/css-masking-1.md#compute-stroke-bounding-box)
and the existing D21 choice to use it for every stroke-box. The line and
its width are authored at scale 2^100, canceled by the reciprocal CTM.
Half-width 2^103 times miter limit 2^30 is the finite growth 2^133. Even
this deliberately generous box contains the entire butt-capped stroke;
the line has no join where the large miter limit could alter its paint.
Reducing the limit to 4 matches exactly. Removing the zero-inset clip
also matches exactly. There are only two path points and four clip
vertices, and the painted rectangle is 32x16 pixels. This tests the
stroke-box growth product independently of later basic-shape endpoint
narrowing, square-cap culling and near-reversal miter construction.

The dash follows [dash positioning](../../dev/spec/svg2/painting.md#TermDashPositions)
and [stroke shape construction](../../dev/spec/svg2/painting.md#StrokeShape).
Relative to its large origin, the cubic is x=48t, y=3t(1-t), so its
absolute slope is at most 1/16. Arc length per unit x is between 1 and
sqrt(257/256). Each stroke normal shifts x by less than .5px, so the
sample's local x=10..14 can only meet centerline x=9.5..14.5. Those arc
positions remain strictly between 8 and 16, wholly in the first gap,
with more than the D1 curve allowance to spare. Normalizing the path's
origin to zero and adjusting only its translation matches exactly.
Making both control-point y coordinates zero is another exact control:
the same large-origin cubic becomes straight and still has this gap.
This tests the cubic dash-start cut independently of dash-period range,
pathLength calibration, closed-dash joining and culling reach.

The planes follow [3D rendering-context depth ordering](../../dev/spec/css-transforms-2.md#3d-rendering-contexts).
Three scale3d factors of 2^100 give a finite context scale 2^300, while
the outer reciprocal factors cancel xy exactly. Both device maps are
the identity and both layers remain 64x64. The green plane has depth
10*2^300 and the red one -10*2^300. Their separation greatly exceeds
the code's global ordering tolerance; a valid unit normal distinguishes
them. Newell's unnormalized component is finite, but squaring it exceeds
double range, producing an infinite length and then a zero normal.
The debugger confirms identity device maps and zero normals. Replacing
every factor by 2^60 with matching reciprocals matches exactly. Using
2^80 instead is another exact control, retaining all transform functions
and the same visible geometry. This tests normalization range separately
from a distant plane inflating the ordering tolerance, projective layer
resolution, paint extents and light-vector normalization.

The scale follows [font-relative lengths](../../dev/spec/css-values-4.md#font-relative-lengths),
[calculation type checking](../../dev/spec/css-values-4.md#calc-type-checking)
and [paint-server transforms](../../dev/spec/css-transforms-1.md#svg-transform-functions).
The gradient's font size is 32px, so 1em/16px-1 is the number 1. Its
CSS scale transform is therefore the identity and is not singular under
D8. Both stops are opaque lime. Replacing the calculation by literal 1
matches exactly. Setting the font to 16px and removing the subtraction
is another exact control, retaining the relative-unit quotient. No text
is rendered. This tests the context used for preliminary singularity
validation independently of transform-origin resolution, instance font
inheritance, omitted individual transforms and relative math rejection.

The direction follows [automatic marker orientation](../../dev/spec/svg2/painting.md#OrientAttribute)
and [marker rendering](../../dev/spec/svg2/painting.md#RenderingMarkers).
The endpoints are finite float-range numbers; their double difference
is (5e38,3.75e38), whose unit direction is exactly (4/5,3/5). The start
maps to (14,26.5), and the marker's painted rectangle is 60x24 before
rotation. In that rectangle's own axes, the sample has longitudinal
coordinates 50.5..56.1 and transverse coordinates 1.6..7.2, strictly
inside its paint. At the erroneous 45deg angle its transverse coordinates
are all negative. Halving every source coordinate and marker dimension
while doubling the CTM scale matches exactly. Replacing auto by the
equivalent angle 36.86989764584402 also matches exactly. The path itself
has neither fill nor stroke. This tests tangent-component narrowing
independently of stroke segment lengths, angle-unit conversion, missing
zero tangents and non-scaling marker orientation.

The triangle follows [filling an interior](../../dev/spec/svg2/painting.md#FillProperties)
and [the nonzero fill rule](../../dev/spec/svg2/painting.md#FillRuleProperty).
Its coordinates are exact powers of two, all finite and representable
in float. Its diagonal is y=x/2; throughout x=32..48, y=28..32 the
sample lies at least 4px above it and far inside the other two sides.
The cropped layer starts at (32,28), so the correct local edge equation
is y=x/2-12. Subtracting that origin in float loses both offsets and
leaves y=x/2 locally, equivalent to a 12px downward shift in the document.
The resulting local triangle covers area 16 instead of the expected 64.
Replacing the far y magnitude 2^30 by 2^20 and the x magnitude by twice
that value matches exactly. Using 2^16 instead is another exact control;
both retain the same visible half-plane and sample. Only straight edges
are involved. This tests raster-origin rebasing independently of curve
flattening, stroke construction, basic-shape coordinates, nonfinite edge
clamping and raster work limits.

The declaration follows [reserved namespace bindings](https://www.w3.org/TR/xml-names/#xmlReserved)
and [well-formed image resources](../../dev/spec/svg2/conform.md#ConformingSVGViewers).
The xml prefix may be declared only with its predefined namespace URI;
urn:wrong violates the namespace constraint even when the prefix is not
used. The error precedes all painted content. Replacing urn:wrong with
http://www.w3.org/XML/1998/namespace makes the resource valid and renders
the 16x16 green square exactly. Removing the declaration is another exact
green control. Python ElementTree independently rejects the original;
resvg leaves it empty. The outer SVG is well formed. This tests validation
of namespace declarations separately from duplicate expanded attribute
names, forbidden character references and namespace-sensitive selectors.

The inclusion follows [XInclude language fixup](https://www.w3.org/TR/xinclude/#language),
[language inheritance](../../dev/spec/svg2/struct.md#LangSpaceAttrs)
and [the empty language range](../../dev/spec/selectors-4.md#the-lang-pseudo).
The included document has no language; its new parent has French. XInclude
preserves that source language by adding xml:lang="" to the included root.
The API exposes no option to suppress this fixup. The selector then matches
the included group, making its 16x16 rectangle green. Authored xml:lang=""
on the included root matches exactly. Removing French from the outer
parent is another exact green control. All resources are well formed and
UTF-8; no text is rendered. This tests language preservation while merging
XML documents independently of text-resource decoding and language lookup
through nested use instances.

The circles follow [radial gradient geometry](../../dev/spec/svg2/pservers.md#RadialGradientNotes)
and [pad spreading](../../dev/spec/svg2/pservers.md#RadialGradientElementSpreadMethodAttribute).
The radii are exactly 8 and 8-2^-18, both representable in float, with a
strictly positive growth. The start circle does not cover the end circle.
Every point of the sample is at least sqrt(128) from their common center,
past the end circle, where pad uses the final opaque lime stop. Setting
fr to 4 matches exactly. Setting it to 8-2^-16 instead is another exact
control, retaining the almost coincident circles. Resvg matches the
original's green expectation exactly. The focal point is at the center,
not near the end circle, so D50's on-circle focal-point rule is unrelated.
This tests the preliminary overlap classification independently of the
radial quadratic's range and negative-radius validation.

The stroke follows [non-scaling stroke construction](../../dev/spec/svg2/painting.md#non-scaling-stroke),
[zero offset](../../dev/spec/filter-effects-1.md#feOffsetElement)
and D3's output-pixel host. The shear maps the horizontal centerline to
(24,16)..(40,16), so its 16px butt-capped stroke is the rectangle
[24,40]x[8,24]. The sample [28,36]x[8,10] is fully inside. The filter
region does not cut it and the offset changes nothing. Removing the
filter matches exactly. Replacing the shear by translate(16 0) also
matches exactly, retaining the filter and identical device centerline.
The debug driver shows l.host still identity when the filter layer's
map is (x-y+64,sqrt(2)*y); its inverse later compresses the stroke to
16/sqrt(2) pixels high. This tests transfer of the vector-effect host
through an intermediate filter layer independently of use positioning,
determinant range, marker orientation and filter operating coordinates.

The step follows [Gaussian blur and its permitted three-box approximation](../../dev/spec/filter-effects-1.md#feGaussianBlurElement),
[duplicate edge extension](../../dev/spec/filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-duplicate)
and D1. The source is an infinite black/white step after duplication; its
vertical deviation is zero. At sigma=256, the prescribed box width is
floor(256*3*sqrt(2*pi)/4+0.5)=481. For output column x, let s=x-32+720.
The white fraction is sum(j=0..3, (-1)^j*choose(3,j)*choose(s-481*j+3,3))
divided by 481^3, omitting terms with s-481*j<0. This directly counts
three uniform integer offsets and gives the expectation's 1px strips,
without copying a renderer kernel. The exact Gaussian differs by under
one byte level here as well. Extending the filter and both rectangles
vertically from y=-48 through 80 matches exactly. Extending them from
-64 through 96 is another exact control. Both leave the sampled rows and
the infinite step unchanged; zero vertical blur makes that extra height
irrelevant. Both allow the requested deviation. A literal
128 deviation reproduces the original's wrong pixels exactly. This tests
an incorrect deviation cap separately from D1's choice of convolution,
filter-coordinate scales, kernel reach and actual work limits.

The constant follows [Gaussian convolution](../../dev/spec/filter-effects-1.md#feGaussianBlurElement)
and [duplicate extension](../../dev/spec/filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-duplicate).
Every horizontal input sample, including every extended sample, is opaque
lime. Any normalized Gaussian or box convolution preserves it exactly.
The permitted approximation uses width 75,199: summing full 16-bit
channels gives 4,928,166,465, beyond u32. Reducing sigma to 20,000 matches
exactly, retaining the complete 65,540px row. Using in="FillPaint" matches
exactly at the original sigma: the referenced rectangle's infinite
constant paint equals its duplicated source, and uses the constant-paint
shortcut. The source and output contain only 65,540 pixels, far below the
2^26 raster-area limit; no node, outline or paint budget is exhausted.
The deviation cap is inactive. This tests accumulation range independently
of deviation clamping, fixed-coefficient filters and Gaussian accuracy.

The tap follows [convolution sampling](../../dev/spec/filter-effects-1.md#feConvolveMatrixElement),
[kernel spacing](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength)
and [duplicate edges](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-edgemode).
After kernel reversal the sole nonzero weight samples x+2^31, y. All
such samples are beyond the input's green right edge at x=40; duplicate
therefore yields the same opaque green everywhere in the output subregion.
Using spacing 2^30 matches exactly. Using 2^20 is another exact green
control. The debug driver records x0=-2147483648 and wx=4294967296 for
the first tap, although its finite positive coordinate is 2147483648.
The input is SourceGraphic, with both halves inside the filter region,
so intermediate-result pruning cannot discard its edge. The CTM is the
identity and the kernel has only two entries. This tests sample-index
conversion independently of integer attribute rounding, kernel reach,
filter-scale range and coefficient arithmetic.

The attribute follows [XML's Entity Declared constraint](https://www.w3.org/TR/xml/#wf-entdecl)
and [well-formed image resources](../../dev/spec/svg2/conform.md#ConformingSVGViewers).
The resource has no DTD, so missing must be declared before its reference
can appear in any attribute value. Ignoring the SVG meaning of m:tag does
not remove that XML requirement. Python ElementTree rejects the decoded
resource; resvg leaves it empty. Removing the attribute's m: prefix is an
exact empty control, exercising the existing declaration check. Adding
an internal declaration for missing makes the resource valid and draws
the green square exactly. Replacing the reference with literal ok is
another exact green control. The outer SVG remains well formed. This
tests deferred validation of discarded attribute values independently of
character-reference legality, namespace constraints and DTD defaults.

The box follows [object bounding box units](../../dev/spec/svg2/coords.md#ObjectBoundingBoxUnits),
[gradient coordinates](../../dev/spec/svg2/pservers.md#LinearGradientElementGradientUnitsAttribute)
and D18's double CTMs. All path coordinates are exact powers of two
within float's finite range: x is +/-2^127 and y is +/-2^126. Their
box width is 2^128, finite in double; the full box map composed with
translate(24 24) scale(2^-124) is matrix(16 0 0 8 16 20), invertible
and ordinary. Both stops are opaque lime. Halving every path coordinate
and doubling the CTM scale matches exactly. Changing gradientUnits to
userSpaceOnUse also matches exactly, retaining both stops and the same
path. Literal lime is another exact control. The debugger confirms a
finite double width narrowed to float infinity before matrix composition.
This tests range of a derived box span independently of authored gradient
endpoints, aspect ratios, stroke bounds and basic-shape coordinates.

The input follows [FillPaint's infinite extent](../../dev/spec/filter-effects-1.md#attr-valuedef-in-fillpaint)
and [constant infinite input to a blur](../../dev/spec/filter-effects-1.md#feGaussianBlurElement).
The two-stop pad gradient is opaque lime at every point of the plane,
so any normalized convolution preserves it exactly, including D1's boxes.
The filter clips only its 16x16 result. Its raster is 32x32; the debugger
confirms that this is the complete stored paint input, and the sigma=8
kernel can read beyond its edges. Replacing the fill with literal lime
matches exactly via the constant-paint path. Removing the second identical
stop is another exact control, retaining the URL paint. Reducing sigma to
2 also matches exactly, retaining both stops and keeping all necessary
taps within the stored plane. No coefficients or accumulation approach
their range limits. This tests finite storage of an infinite paint
independently of SourceGraphic region clipping, deviation clamping,
blur-sum overflow and interpolation color spaces.

The phase follows [negative dash offsets](../../dev/spec/svg2/painting.md#StrokeDashoffsetProperty)
and [dash positioning](../../dev/spec/svg2/painting.md#TermDashPositions).
Each dash and gap is 2^29 user units, the period is 2^30, and the offset
is -2^-12. All authored values are exactly representable in float. The
normalized offset is 2^30-2^-12, inside the gap with 2^-12 remaining.
The CTM maps that remaining gap to 16px and the complete path to 32px;
the 8px butt-capped stroke therefore covers only [24,40]x[12,20].
Replacing both dash entries with 0.015625 matches exactly. Replacing
them with 0.00390625 is another exact control. Both keep the initial
16px gap and leave the following dash longer than the path. Resvg shares
the original failure and matches both controls. This tests cancellation
in finite phase normalization independently of period overflow, pathLength
calibration, curved dash cuts and closing joins. No dash budget is near
exhaustion: the short path reaches only one painted dash.

The frame follows [filtering in local coordinates](../../dev/spec/filter-effects-1.md#propdef-filter),
[convolution sampling](../../dev/spec/filter-effects-1.md#feConvolveMatrixElement)
and [kernel spacing](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength).
Let N=2^20. The CTM is (x,y) -> (64-y/N,x/N), mapping the source to
[16,32]x[24,40]. After kernel reversal, the only nonzero tap reads
(x+8*N,y); the result therefore moves by -8*N in user x, or (0,-8)
in device coordinates. The expectation is [16,32]x[16,32]. Instead,
the device-axis convolution produces [8,24]x[24,40]. Halving every
geometry, filter-region and kernel-spacing length while doubling the
CTM's linear coefficients matches exactly. Using one quarter of those
lengths and four times the coefficients is another exact control.
Setting N=1 also matches exactly. All retain the identical quarter turn,
source, tap and filter region in device coordinates. Resvg does not honor
the requested spacing in these controls and is not the oracle. The
filter belongs to the rect, not the root; no intermediate input is
pruned, and the squared CTM norms remain in float range. This tests an
absolute threshold in frame classification independently of the root
exception, norm overflow, kernel reach and integer sample conversion.

The ellipse follows [basic ellipse sizing](../../dev/spec/css-shapes-1.md#funcdef-basic-shape-ellipse)
and [radial-size syntax](../../dev/spec/css-images-3.md#typedef-radial-size).
A lone percentage is not a radial size: explicit elliptical radii require
two values, and the single-length alternative does not admit percentages.
The [primary invalid-syntax test](https://github.com/web-platform-tests/wpt/blob/master/css/css-shapes/shape-functions/ellipse-function-invalid.html)
also rejects a lone percentage followed by at. The invalid presentation
attribute must therefore leave initial clip-path: none. The outer URL
clip samples only the rectangle's leftmost 8x16 band, well outside the
incorrectly accepted ellipse. Removing at center matches exactly because
the parser then reaches its arity check. Removing the rectangle's clip
attribute is another exact control. Replacing the invalid size by
32px 32px at center also matches exactly, using a valid ellipse containing
the sample. Resvg matches the original expectation. This tests acceptance
inside the shape grammar independently of invalid declarations discarded
too late by the cascade and repeated geometry boxes.

The scale follows [non-invertible transform suppression](../../dev/spec/css-transforms-1.md#transform-function-lists)
and [the 3D scaleZ function](../../dev/spec/css-transforms-2.md#funcdef-scalez).
Its complete matrix is diag(1,1,0,1), with determinant zero. Transform
functions producing a non-invertible CTM suppress the object and its
contents, even though restricting this matrix to z=0 gives an ordinary
2D identity. The scale3d(1,1,0) and literal matrix3d spellings reproduce
the failure. Replacing scaleZ(0) by scaleX(0) or scaleY(0) gives exact
empty controls. Replacing it by scaleZ(1), scaleZ(-1) or scaleZ(0.5)
gives exact green controls: these matrices are invertible and preserve
the square's projected geometry. Resvg ignores even the scaleX control
in this CSS syntax and is not the oracle. The example uses exact zeros
and ones, no perspective, no preserve-3d context and no range limits.
This tests full-matrix singularity independently of resource transform
contexts, vector-effect determinants and homogeneous-weight range.

The marker follows [conditional processing](../../dev/spec/svg2/struct.md#ConditionalProcessing),
[never-rendered element types](../../dev/spec/svg2/render.md#TermNeverRenderedElement)
and [marker rendering](../../dev/spec/svg2/painting.md#RenderingMarkers).
Conditional processing has no effect on a never-rendered element itself;
marker is explicitly one of those types. Its children still participate
in the graphical effect. The fixed marker angle and user-space units
place its 16x16 lime square at (8,8). Removing systemLanguage matches
exactly. Changing fr to en is another exact green control, retaining
the attribute. Moving the French condition to the child rectangle gives
an exact empty control, as required for child content. Resvg matches
the original green expectation and every control. This tests erroneous
condition checking on the marker resource independently of missing
checks on clip children, root display handling and instance languages.

The magnification follows [pixelated scaling](../../dev/spec/css-images-3.md#valdef-image-rendering-pixelated).
The embedded 2x16 RGBA PNG has sixteen identical rows: opaque red then
opaque lime. Its horizontal scale is exactly 2^65, already an integer;
pixelated therefore uses nearest-neighbor selection on that axis. The
image starts at -3*2^64, placing each sampled center at source x=1.5
plus less than 2^-60, wholly in the green column. Vertical scale is 1.5;
the identical rows make vertical interpolation irrelevant. Reducing width
to 2^62 and keeping x at minus three quarters of that width matches
exactly. Using width 2^60 is another exact control. Changing only
image-rendering to crisp-edges also matches exactly at the original
width. Making height 32 matches exactly too: the all-integer shortcut
avoids sharpening. The debugger records sharpX=Inf, sharpY=2, source
x=1.5 and chosen texel x=0. Resvg uses a smooth horizontal sample and
does not match the pixelated controls, so it is not the oracle. Only a
32-pixel source and a 16x16 output sample are needed; no large intermediate
image or work budget is involved. This tests range of the pixelated
integer scale independently of minification, pattern resolution, filter
frames and convolution indices.

The radius follows [math range checking](../../dev/spec/css-values-4.md#calc-range),
[the focal-radius restriction](../../dev/spec/svg2/pservers.md#RadialGradientElementFRAttribute)
and D30's explicit rule for calculated negative attribute values. A plain
negative fr is invalid, but calc(-4px) is valid and clamps to zero.
The zero start circle and positive end radius define an ordinary radial
gradient; its two identical stops make every sample opaque lime.
Replacing the calculation by 0 matches exactly. Using calc(0px) is
another exact control, retaining a math function. Removing fr also matches
exactly via its initial zero radius. Replacing the calculation by literal
-4 gives the exact red fallback required by D8. Resvg matches the original
green expectation but also ignores the literal negative radius, so that
comparison cannot establish the distinction. This tests the missing range
check in gradient-coordinate resolution independently of filter-function
range checks, dash-style clamping and radial containment tolerances.

The language follows [systemLanguage's comma-separated tag set](../../dev/spec/svg2/struct.md#SystemLanguageAttribute)
and D11's fixed en preference. Without a comma, fr en is one token,
not two language tags; it cannot be an English tag or an English-prefixed
BCP 47 tag. The rectangle therefore fails its condition. Replacing
fr en by fr matches the empty expectation exactly. Replacing it by de
is another exact empty control. Changing the separator to a comma gives
an exact green control, because fr,en contains the English preference.
Resvg matches the original empty expectation and all three controls.
The condition is directly on an ordinary rect, outside any clip, marker
or use instance. This tests language-list parsing independently of missing
clip-child conditions, condition applicability to marker resources and
language inheritance through shadow trees.

The phase follows [linear gradient coordinates](../../dev/spec/svg2/pservers.md#LinearGradientElementGradientUnitsAttribute)
and [repeat spreading](../../dev/spec/svg2/pservers.md#LinearGradientElementSpreadMethodAttribute).
Let A=2^28. In the visible rectangle, the gradient's user x coordinate
is A plus the horizontal distance from device x=8. Its parameter is
therefore A/8 plus that distance divided by 8. A/8 is the integer 2^25,
so repeat must leave the same four red and four green pixels per period
as at the origin. Both gradient endpoints, its period and every authored
coordinate are finite and exactly representable in float. Setting the
rectangle's x and its cancelling translation to zero matches exactly.
Using A=2^16 is another exact control; A=2^12 also matches. Adding
gradientTransform="translate(268435456 0)" matches exactly at the original
rectangle position: shifting the paint by an integer number of periods
preserves its phase. Resvg also misplaces the rectangle at the original
large coordinate and is not the oracle. The gradient spans eight device
pixels; its minification shortcut is inactive. This tests narrowing of
the computed linear parameter independently of endpoint narrowing,
normalization range and conic-angle reduction.

The position follows [circle syntax](../../dev/spec/css-shapes-1.md#funcdef-basic-shape-circle)
and [the position grammar](../../dev/spec/css-values-4.md#position).
The complete at clause may be omitted, but when at is present it must
be followed by a position. An empty position is invalid. The clip-path
presentation attribute must therefore be ignored, leaving initial none.
Replacing the empty position with nowhere matches the green expectation
exactly: the nonempty invalid position is correctly rejected. Removing
the clip attribute is another exact green control. Supplying center
gives an exact empty control, since the valid circle has radius zero.
Omitting the entire at clause also gives that exact empty result. Resvg
ignores these basic-shape clips even in the valid controls, so it is not
the oracle. The circle has its valid single radius; no ellipse arity
check is involved. This tests acceptance of an empty position independently
of ellipse radius counting, repeated geometry boxes and the cascade's
generic invalid-declaration fallback.

The declarations follow [XML's prolog grammar](https://www.w3.org/TR/xml/#NT-prolog).
The optional document type declaration occurs at most once. Both declarations
here are empty: there are no external subsets, entities, default attributes
or validity constraints to interpret. The second declaration alone makes
the embedded document malformed, so its image paints nothing. Keeping just
one declaration matches a literal 16x16 lime square exactly. Removing both
is another exact green control. Replacing the second declaration with the
valid processing instruction `<?data harmless?>` also matches exactly.
Python ElementTree rejects the decoded original, and resvg matches the
empty expectation and all three green controls. This tests prolog state
independently of DTD default attributes, character references, namespace
bindings and attribute uniqueness.

The comment follows [XML's comment grammar](https://www.w3.org/TR/xml/#sec-comments).
A comment body cannot contain a double hyphen. The metadata comment
`<!--a--b-->` therefore makes the embedded document malformed; the image
must paint nothing even though metadata itself does not paint. Changing
the body to `a-b` gives an exact green control. Removing the comment also
matches the literal lime square exactly. Replacing it with the valid
processing instruction `<?note a--b?>` is another exact green control:
double hyphens are allowed in that context. Python ElementTree rejects
the decoded original, and resvg matches the empty expectation and all
three green controls. This tests comment-body scanning independently
of the number of prolog declarations and character-code validation.

The context follows [recursive context paint and its coordinate space](../../dev/spec/svg2/painting.md#context-paint).
The outer use starts at (8,8). Its user-space gradient ends at x=16,
while the inner use translates the square to x=16..32 in that original
frame. Every sample is therefore past the final lime stop. Recursive
context-fill must retain that original paint frame. Replacing the inner
use with a group translated by 16 and containing the same rectangle gives
an exact green control, preserving the outer use and gradient. Giving
the inner use a literal lime fill also matches exactly while keeping
both use instances. Changing the outer use's fill to lime is another
exact control. The debugger records the inner use's inherited context
flag as true and the old context matrix translated by (8,8); lowerUse
then replaces that matrix with translation (24,8). Resvg shares the
original failure and matches the controls, so the reference is the
independent literal square. This tests loss of the originating paint
frame across recursive context references independently of instance
style caching, geometry cascade and instance-box measurement.

The stylesheet follows [CSS byte-stream decoding](../../dev/spec/css-syntax-3.md#css-decode-bytes)
and [the Encoding Standard's decode algorithm](https://encoding.spec.whatwg.org/#decode).
Its first three bytes are EF BB BF, followed by the ASCII rule
rect{fill:lime}. Decoding consumes that UTF-8 BOM instead of returning
it as a character to the CSS tokenizer. D15 explicitly applies stylesheets
from xml-stylesheet data URLs; no external resource or import is required.
Removing just the BOM gives an exact green control. Replacing it with
an empty CSS comment also matches exactly. Inserting nothing{fill:blue}
between the BOM and rect gives another exact green control while keeping
the original BOM and loading path: only the harmless first type selector
then absorbs the unwanted character. Python's utf-8-sig decoder returns
the expected ASCII rule independently. Resvg does not apply the linked
sheet even in the controls and is not the oracle. This tests byte decoding
of a linked stylesheet independently of CSS code-point preprocessing,
XInclude text decoding, preferred-sheet selection and data URL headers.

The separator follows [the position grammar](../../dev/spec/css-values-4.md#position)
and [circle syntax](../../dev/spec/css-shapes-1.md#funcdef-basic-shape-circle).
The two center keywords are each valid, but their intervening comma is
not part of a position. The presentation attribute must be ignored, so
the square remains lime. Replacing the tail with center nowhere gives
an exact green control, confirming rejection of a nonempty invalid
position. Removing the clip is another exact green control. Removing
only the comma gives an exact empty control: center center is valid,
and a valid zero-radius circle clips everything. Resvg ignores even
that valid clip and is not a semantic oracle. Both position words are
present; this tests discarded punctuation independently of empty-position
acceptance, ellipse radius counting and repeated geometry boxes.

The identity follows [perspective() syntax](../../dev/spec/css-transforms-2.md#funcdef-perspective)
and [the explicit none-to-infinity rule](../../dev/spec/css-transforms-2.md#valdef-perspective-func-none).
perspective(none) is a valid identity matrix, so the complete list still
translates the square by (8,8). Removing that identity function gives an
exact control. Replacing it with perspective(64px) also matches exactly:
the square and subsequent translation stay in the z=0 plane. Replacing
it with an explicit identity matrix3d gives a third exact control.
The existing transform-3d-functions unit includes perspective(none),
but follows it only with translateZ; rejecting that entire list leaves
the same visible z=0 square, concealing this failure. Resvg ignores the
CSS transform controls and is not the oracle. All names here have their
canonical case, the matrix is invertible and no perspective resampling
is needed. This tests the missing keyword argument independently of
case-sensitive function dispatch, singular 3D scales and projective range.

The axes follow [transform-origin's grammar](../../dev/spec/css-transforms-1.md#propdef-transform-origin).
Two horizontal keywords cannot form a valid origin. Discarding left right
retains the preceding center declaration. The square's fill box has center
(16,16), and rotating it by 180 degrees about that point preserves its
position. Removing the invalid declaration gives an exact green control.
Replacing left right with the invalid word nowhere also matches exactly,
retaining the duplicate declaration and testing the validation path.
Replacing it with valid 50% 50% is a third exact control. Resvg matches
the original expectation and all controls. This is the explicit
validGeometry validator's missing axis check, independently of the
generic validDeclaration fallback, origin length resolution and empty
or comma-separated positions.

The dash follows [dash lengths and positions](../../dev/spec/svg2/painting.md#StrokeDashing).
Let P=65536. The cubic has y=16 and x=8+3P*t*t*(1-t). Its first leg
increases monotonically to t=2/3 and has length 4P/9. The return leg
has the same length, so total length 8P/9 is less than the 65536px gap.
Only the first 4px dash paints. Its horizontal direction and butt caps
therefore give the literal 4x8 rectangle at (8,12). Replacing the cubic
by H40 gives an exact control. Using the monotonic straight cubic
C8 16 8 16 40 16 also matches exactly, retaining a stationary start.
Reducing the original control point's x to 72 gives a third exact
control, retaining the reversal and cubic measurement path. The debugger
records target length 4, interval [0,2/3], whole length 29127.111111111117
and returned t=0.012983163800184966. Substituting that t in the cubic
gives distance 32.710472597306982. There are only three flattened points
and one dash cut; no subdivision or work budget is exhausted. Resvg
also misses the exact length on the original and is not the oracle.
This tests convergence of cubic length inversion independently of
translated cut-coordinate narrowing, pathLength calibration and dash
phase normalization. The error exceeds the 0.025px flattening target
by more than 28px on a curve lying entirely on a straight line.

Group additional manifestations of an existing cause into its existing
case instead of adding another top-level failure for the same cause.
