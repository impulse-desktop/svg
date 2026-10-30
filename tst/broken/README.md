# Known rendering failures

Minimal static SVG cases found by reading `svg.cpp`, forming a hypothesis,
and rendering the input against an independent drawing. The renderer and
the frozen canon are unchanged. Text, animation, script, foreignObject,
documented design departures and budget exhaustion are not test subjects.

Each top-level SVG has a same-named drawing in `expect/`. The expectations
use literal rectangles or an empty drawing; the translated gradient uses
one-pixel strips calculated from its linear interpolation formula. Test
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

Group additional manifestations of an existing cause into its existing
case instead of adding another top-level failure for the same cause.
