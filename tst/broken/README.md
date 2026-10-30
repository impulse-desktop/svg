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
instance the rectangle instead of the intermediate use. All eleven
controls match their expectations exactly.

Integer targets follow [integer math-function rounding](../../dev/spec/css-values-4.md#css-round-to-the-nearest-integer)
in [targetX](../../dev/spec/filter-effects-1.md#element-attrdef-feconvolvematrix-targetx),
shape commands follow the [shape close command](../../dev/spec/css-shapes-1.md#valdef-shape-close)
and its [SVG subpath semantics](../../dev/spec/svg2/paths.md#PathDataClosePathCommand),
and nested languages follow the existing D40 choice of host language for instances.

Group additional manifestations of an existing cause into its existing
case instead of adding another top-level failure for the same cause.
