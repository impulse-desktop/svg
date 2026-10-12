# svg

[![CI](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

An SVG renderer of its own for the impulse desktop: parser, document model
and rasterizer, written from scratch and measured against Chromium on a
corpus of thousands of files.

The renderer is two files, `svg.h` and `svg.cpp`, over
[libstd](https://github.com/pg83/std). A program that uses it copies both
into its `ext/`.

## Scope

Static SVG without text: no `<text>`, no SMIL or CSS animation, no script,
no `foreignObject`. Raster images inside a document are decoded by the
program that embeds the renderer. Correctness comes first; speed and
memory come after it.

## The contract

```cpp
SvgLibrary* library = SvgLibrary::create(pool, decoder);
SvgDocument* document = library->parse(pool, bytes);
SvgImage* image = document->render(pool, width, height);
```

`render(width, height)` draws the document the way Chromium draws it as an
image of that size: `<img src="file.svg" style="width: Wpx; height: Hpx">`.
`render(width, height, scale)` draws it at `scale` device pixels to the CSS
pixel: the image is laid out at `width / scale` by `height / scale` and drawn
at `width` by `height`. The pixels are RGBA, 8 bits a channel, not
premultiplied. `decoder`, an
`SvgDecoder` of the embedding program, turns the bytes of a raster image
inside the document into such pixels.

## The score

`./build meanrms`, `./build maxrms`, `./build struct` and `./build aa` draw
every case of the corpus and print what they measure; lower is better, 0 is
the reference itself. `./build all` prints the four together with the
candidates for the next fixes.

- A case is one file of `tst/corpus/<source>/` and its two references next
  to it: `<name>.1x.png`, its longer side 256 pixels, and `<name>.2x.png`,
  512 pixels.
- A reference is supersampled: drawn with its longer side `tst/corpus/SIDE`
  device pixels, 1024, so 1x at 4 device pixels to the pixel and 2x at 2,
  and shrunk by the mean of each block in premultiplied RGBA. Ours is drawn
  and shrunk the same way, so neither antialiasing nor the shapes laid one
  over another on an edge tell the two apart, only what is drawn.
- The error of one drawing is the mean absolute difference between ours
  and the reference over every pixel and channel, in premultiplied RGBA,
  scaled to [0, 1]. A crash or a drawing past two minutes counts as 1.
- The error of one case is the root mean square of the errors of its
  drawings.
- `meanrms` is the root mean square of the errors of every drawing: how
  close the whole corpus is.
- `maxrms` is the error of the worst case.
- `struct` counts what is drawn wrong rather than slightly differently. A
  pixel is wrong when a channel of ours lies more than 32 out of 255 outside
  the range of Chromium's pixels around it, three by three, or Chromium's
  lies that far outside the range of ours, and both drawings blurred with
  the kernel 1 2 1 across and down still differ there by more than 32: an
  edge or a thin line a fraction of a pixel away is not wrong, a missing or
  miscolored one is. A case fails when over 0.1% of the pixels of one of its
  drawings are wrong. `struct` is the number of failing cases and the mean
  share of wrong pixels over the worst 1%.
- `aa` is `meanrms` of ours drawn straight at 256 pixels against the same 1x
  references: how close our plain drawing is to the supersampled one,
  antialiasing included.

The same test draws each case once for all of them: `svg test` writes the
errors and the shares of wrong pixels of its supersampled drawings, the
error of its plain 1x drawing, and the shares of wrong pixels between the 1x
drawing and the 2x drawing halved, the reference's and ours. When the 2x
reference is exactly twice the 1x one and nothing in the document depends on
the CSS size, one supersampled drawing serves both, and ours agree by
construction. A reference drawn differently at the two sizes while ours agree
is a candidate for `tst/corpus/CANON`.

`$(B)/meanrms.txt`, `$(B)/maxrms.txt`, `$(B)/struct.txt`, `$(B)/aa.txt` and
`$(B)/all.txt` hold the numbers and every case, the worst first.
`-Dfilter=GLOB` scores the cases whose path under `tst/corpus` matches,
`./build -Dfilter='resvg/*' all`.

## The corpus

| source | what |
|---|---|
| `resvg` | the resvg test suite |
| `w3c` | the W3C SVG 1.1 test suite |
| `blink` | Chromium's `web_tests/svg` |
| `wpt` | web-platform-tests: `svg`, `css-masking`, `filter-effects`, `compositing` |
| `adwaita`, `breeze`, `papirus` | desktop icon themes |
| `mdi`, `simple`, `tabler`, `bootstrap`, `fontawesome` | icon sets |
| `logos` | brand logos from gilbarbara/logos |
| `wikimedia` | random files of Wikimedia Commons up to 300 KB |
| `openclipart` | random Openclipart drawings up to 400 KB, mostly made in Inkscape |
| `inkscape` | artwork from the inkscape.org community gallery up to 1 MB |

`tst/import.py RAW CORPUS` takes the files from the unpacked sources: text
elements are cut out, files with what is out of scope are left out, both
also inside SVG images embedded as data URIs, equal files are kept once, and
the large icon sets are sampled with a fixed seed.
`tst/reference.py CHROMIUM SVG CORPUS JOBS` draws the references with headless
Chromium through its DevTools pipe at the device scale `SIDE` asks for,
shrinks them with `svg shrink` of the test driver `SVG`, and drops the files
Chromium cannot draw; `tst/corpus/CHROMIUM` names the version. Chromium is the reference,
not the goal: where it demonstrably draws a case wrong, the drawing of
Inkscape, resvg, Firefox or ours that is right takes its place.
`tst/corpus/CANON` lists such a case with the renderer and the reason,
`tst/canon.py RENDERER PROGRAM SVG CORPUS` draws the cases listed for that
renderer at the sizes Chromium chose, using `SIDE` unless overridden, and
`tst/corpus/INKSCAPE`, `tst/corpus/RESVG`, `tst/corpus/FIREFOX` and
`tst/corpus/OURS` name the versions. Firefox is drawn twice, over white and
over black, to recover the alpha its screenshots lose, and scaled by a CSS
transform; Inkscape and resvg can only enlarge a drawing, which is the same
as a device scale unless the document has neither a viewBox nor an absolute
size. A case whose behaviour is undefined and which
every renderer draws differently is listed with the reason in
`tst/corpus/EXCLUDED`, and is neither drawn nor scored. Ordinary corpus imports
reuse existing references; renderer upgrades and fixes to reference generation
require redrawing the affected cases.

`tst/reference-inkscape.py INKSCAPE SVG CORPUS OUTPUT JOBS [GLOB...]` renders
Inkscape candidates in a separate output directory, for example with the
patterns `'openclipart/*' 'inkscape/*'`. It uses the existing reference sizes
and `SIDE`, isolates failed exports, and resumes completed files when their
input, renderer version and shrinker are unchanged. Each pair of PNGs has a
JSON record with its input and output hashes or the export failure. These
candidates can be compared before selecting cases for `CANON`.
Inkscape is given the reference's CSS viewport before export scaling, so
rounded aspect ratios preserve the SVG's `viewBox` alignment instead of
stretching its original page.

A `CANON` renderer may specify a per-case reference resolution as
`renderer@SIDE`, for example `firefox@4096` for dense filter textures.
The output PNG dimensions and the renderer-under-test resolution stay the same;
`tst/canon.py` renders and reduces that external reference at the recorded scale.

Some older drawings use SVG 1.1 `BackgroundImage`, `BackgroundAlpha`,
`FillPaint` or `StrokePaint`, which Chromium no longer implements. Their
`CANON` entries record the external renderer and the reason for choosing it:
`inkscape_legacy` uses Inkscape 1.2.2, and `batik` takes the Batik 1.19
rasterizer jar as PROGRAM (`JAVA` selects the Java executable).
`inkscape_softlight` substitutes an equivalent final CSS blend over the
reviewed opaque backdrop to avoid Inkscape 1.2.2's `feBlend` soft-light parser
bug. `firefox_svg11` uses `tst/svg11_reference.py` to express a paint-only
blur as ordinary painting followed by clipping, and an untransformed
background as an explicit SVG snapshot. These restricted conversions are
for the reviewed cases in `CANON`; they reject unsupported constructions
and run on temporary copies, leaving the corpus SVGs intact.

`inkscape_outline` converts solid path strokes into filled outlines with
Inkscape before rasterizing them. This avoids the angle-dependent area loss
of Chromium's [thin-stroke hairline shortcut](https://github.com/google/skia/blob/main/src/core/SkDraw.cpp).
In `openclipart/174262`, an isolated hatch has area about 288 square pixels:
Chromium draws 205, while our renderer and the outlined reference draw 286.
`tst/stroke_reference.py` restricts conversion to solid paths and groups,
rejecting filters, masks, markers, dashes and non-scaling strokes. Every export
checks the area of 0.25- and 0.5-unit strokes at five angles and two scales.
The source SVG stays unchanged; `tst/corpus/INKSCAPE_OUTLINE` records the version.

`inkscape_curves` uses Inkscape rasterization for the filled cubic
bands in `openclipart/252059`. Exact Bezier integrals give their area independently
of any renderer.
The 36 cubic bands and subpixel positions in `tst/curve_reference.py` give total
absolute area errors of 261 square pixels in Chromium and 10 in Inkscape.
Isolated bands from the drawing confirm the error at its pattern tile scale;
the reference keeps the usual `SIDE`. Every export checks these areas before
rendering the drawing. `tst/corpus/INKSCAPE_CURVES` records the version.

`inkscape_edges` preserves fractional horizontal edge coverage in
`openclipart/321288`. Chromium's rounding loses part of the first row,
amplified by the overlapping fills that share this edge. The 60 controls in
`tst/edge_reference.py` integrate the boundary over device pixels, composite
one or four fills, and average to the reference grid at three scales.
Their maximum alpha errors are 52.3 levels in Chromium and 1.3 in Inkscape.
Every export checks a two-level error limit before drawing the unchanged SVG;
`tst/corpus/INKSCAPE_EDGES` records the version.

`skia_noise_inkscape` also uses Inkscape's filled-curve rasterization for
the source geometry in `openclipart/329492`. Its blur and lighting amplify
coverage errors, and its `1000*a-500` alpha matrix needs sub-byte precision.
Inkscape captures the unfiltered path at eight times the reference device grid.
`tst/coverage_reference.py` decodes straight PNG channels and averages their
premultiplied integer products directly into float32, preserving fractional
coverage before filtering. The filter grid and final reference resolution stay
unchanged. The shared staged graph evaluates Skia primitives and SVG lighting
with float32 intermediates. Turbulence is a recorded
shader: passing a float noise image through `ImageFilters.Image` loses precision.
The Gaussian kernels use the [SVG 1.1 three-box approximation](https://www.w3.org/TR/SVG11/filters.html#feGaussianBlurElement),
since Skia's built-in float blur selects a different kernel.

`tst/source_geometry_reference.py` limits the snapshot to one solid path in a
translated group covering the page, preserving the vector filter region and
noise coordinates. `tst/float_reference.py` rejects unsupported primitives,
regions, color spaces and transforms. Every export checks 36 cubic-band areas
at the source resolution with a 0.25-square-pixel error limit, in addition to
the ordinary Inkscape area controls. Thirteen coverage controls check straight
color decoding, transparent samples, sub-byte alpha amplification and invalid
sample grids. Exports also check eight source-grid probes, fractional-alpha
precision, blur moments and noise
sampling. `tst/float_filters.py` additionally checks calculated flat lighting,
constant displacement and rejected constructions. The original corpus SVG
remains the renderer's input.

Run this backend with Chromium as `PROGRAM` for measuring vector bounds,
`INKSCAPE` pointing to its executable, and `skia-python==144.0.post2` and NumPy
available to the Python interpreter running `tst/canon.py`.
`tst/corpus/SKIA_NOISE_INKSCAPE` records the renderers and numeric dependencies.

`skia_layers_inkscape` applies the same source-coverage and float-precision
checks to eleven paths in `inkscape/d637421a62aec41d`, including eight with
nonuniform scaling and signed axis permutations.
`tst/layer_reference.py` freezes each vector filter region on its local pixel grid,
captures its unfiltered geometry with Inkscape, and evaluates one Skia primitive
at a time. Binary inputs occupy one float source image and are addressed with
offsets. This avoids the Python binding's
[serialization of filter inputs](https://github.com/kyamagu/skia-python/blob/main/src/skia/ImageFilter.cpp),
which rounds embedded images to bytes and duplicates branches of long graphs.
The final layer snapshots retain the original transforms and opacity. Signed
axis permutations preserve the device sample phase; other affine grids start
at the local origin. Each axis has its own blur, morphology, noise and displacement
scale. Displacement channel remapping remains defined at transparent pixels.

Skia's nonuniform lighting fails the calculated planar-surface control because
it has one surface-height scale for both axes. `tst/affine_lighting_reference.py`
evaluates the [SVG lighting equations and edge kernels](https://www.w3.org/TR/SVG11/filters.html#feDiffuseLightingElement)
on float alpha maps in user coordinates. Its separable differences and weighted
averages are checked against analytic planes, including edges, and stock Skia's
interior samples at uniform scale. Point lights retain their user-space position.
Four ellipses keep their browser rendering: Inkscape loses 9.53 square pixels
on the corresponding unscaled exact-area control. Two degenerate paths also
keep their browser rendering because their bounding boxes have zero area.

Every export checks 36 cubic areas, 30 straight-stroke areas, eight source-grid
probes, fractional-alpha precision, binary input routing, noise phase,
convolution orientation and lighting with negative surface heights. It also
checks 72 float lighting cases and 144 browser exports covering transformed
flat lights, lattice noise, parent/element transforms and layer opacity.
`tst/layer_filters.py` also checks all displacement channel pairs and rejected
snapshot constructions; `tst/affine_filters.py` covers anisotropic displacement,
including transparent maps, noise sampling and singular transforms.
The source SVG and native renderer stay unchanged.
Use Chromium as `PROGRAM`, `INKSCAPE` and the Python dependencies described
above; `tst/corpus/SKIA_LAYERS_INKSCAPE` records their versions.

The same backend captures the two colored paths of `openclipart/251806` as one
source group before evaluating its leather filter. Its nonzero `viewBox` origin
is included in the pixel-frame translation, preserving the original turbulence
coordinates. Source groups accept solid paths and nested paint groups, with
inherited fill rules and colors. Descendant effects, transforms, references and
other geometry are rejected. Thirty-six additional browser exports check
overlapping colors, even-odd holes, group opacity, shifted viewports and integer
noise lattices; `tst/group_filters.py` checks the restricted input scope.

`skia_legacy_stencil` evaluates the noisy blue emblem and four rough-paper groups in
`inkscape/5512d1d4e1311b22` on a local float grid. Its discrete alpha transfer has
a threshold at `2/7`; `tst/component_reference.py` applies the
[SVG step intervals](https://www.w3.org/TR/SVG11/filters.html#feComponentTransferElement)
without a byte lookup table, checking adjacent float values against rational
boundaries. `tst/stencil_reference.py` resolves unused class styles and opaque
one-stop swatches, and permits composed groups of paths and circles.
`tst/paper_reference.py` also converts plain source rectangles to paths and
retains the paper groups' original user-space clips and ancestor opacity.
Background-blend regions are measured before replacing their vector children;
each ancestor gets its own explicit region. Before filtering, the exporter
requires an identical full-document export with those regions fixed. This
check rejects Inkscape 1.2.2's changed background extent on translucent
backdrops. Eighty export controls check flat colored lighting against a
calculated vector replacement, or require rejection of a changed background.
`tst/paper_filters.py` checks the restricted geometry and context handling.

Inkscape renders only the source geometry at four times the filter grid's
resolution, then reduces it back before filtering. Six exact annulus-area
controls limit coverage error to 1.25 square pixels; the filter grid itself
stays at the corpus resolution. Inkscape 1.2.2 composes the resulting layer with
the original legacy effects. Export controls cover the drawing's background
arithmetic, multiply and darken operations. Visible unverified background
operations are rejected: in particular, a standalone `feComposite` using
`BackgroundImage` as `in2` fails the independent control in this version.
The drawing's empty, zero-bounding-box consumer does not render that operation.

Use Chromium as `PROGRAM`, current Inkscape in `INKSCAPE`, and the legacy
executable in `INKSCAPE_LEGACY`. `tst/corpus/SKIA_LEGACY_STENCIL` records both
Inkscape versions and the float dependencies. `tst/stencil_filters.py` checks
step boundaries, transparency, composed-source scope and background eligibility.

The float exporters share `tst/layer_reference.py`'s staged graph and use
`tst/affine_lighting_reference.py` for lighting at every axis scale. Even with
float storage, stock Skia approximates the lighting power: an analytically
flat surface with alpha 0.5002 can produce 0.49954, changing a subsequent
`1000*a-500` from 0.2 to zero. Forty-five calculated controls combine five
exponents, three light elevations and three sample grids to catch this loss.
The shared graph uses the SVG lighting equations with separate axis spacing;
its 72 white-light controls also cover boundaries, negative surface heights and
point lights. Another 540 colored-plane controls check per-channel saturation
and specular alpha; light color is multiplied before each channel is clamped.
Full-page snapshots retain their existing noise frame and compose
the filtered grid back into a transparent viewport, including smaller regions.

`skia_emboss_inkscape` expands the 103 instances of the lit path in
`openclipart/264596` and evaluates each on its own float grid. This retains
precision through blur, compositing and lighting before alpha amplification,
including reflected instances. Inkscape supplies source geometry and composes
the final layers. Fixed user-space ancestor clips are detached for source
isolation and restored on their original groups; object-bounding-box clips and
other clip geometry are rejected by `tst/emboss_reference.py`.

Twenty-four export controls combine four axis reflections, three fractional
clip positions and two scales. They check calculated flat lighting amplified
from alpha 0.5002 to 0.2, with exact rectangular coverage at the clip edges.
Chromium fails this edge control: at a boundary of 16.625, alpha 0.2 yields
12 rather than the calculated 19 byte levels, whereas an ordinary rectangle
has the correct coverage. Inkscape passes the same controls. Use Chromium as
`PROGRAM` for geometry measurement and current Inkscape in `INKSCAPE`;
`tst/corpus/SKIA_EMBOSS_INKSCAPE` records the tools and float dependencies.
`tst/emboss_filters.py` checks the allowed instances and clipping constructions.

`chromium_wide_stroke` preserves the covered interiors of oversized
non-scaling strokes in `inkscape/1a6dcfc5187bf462`. Chromium, Firefox and
Inkscape leave holes inside some of these strokes. SVG describes an
[ideal stroke](https://www.w3.org/TR/SVG2/painting.html#StrokeShape), while
allowing approximations around tight curves. For the reviewed convex shapes,
an opaque solid stroke wider than their bounding box's smaller dimension
covers the entire fill. Giving that hidden fill the stroke's color preserves
the ideal result and avoids the holes. The conversion uses each reference's
CSS viewport and requires a stroke at least four raster pixels wide.
`tst/wide_stroke_reference.py` restricts the geometry and paints; temporary
copies preserve the original masks and source SVG. Every export checks 98
known covered centers, including cubic curves, masks and rotated reflections.
`tst/wide_strokes.py SVG` checks rejected inputs and unchanged native pixels;
the full drawing also stays byte-identical at both reference sizes.
`tst/corpus/CHROMIUM_WIDE_STROKE` records the browser version.

`chromium_morphology` makes the [filter-region input clip](https://www.w3.org/Bugs/Public/show_bug.cgi?id=26432) explicit before a
single source erosion, using a zero offset. Chromium otherwise lets pixels
outside that region affect `erode`. A square with analytically known erosion
bounds guards every export; stock Batik agrees with those bounds. The reviewed
W3C morphology cases keep identical native pixels after the conversion.

`chromium_lighting` handles the repeated Y reflections in `openclipart/264596`.
It reflects the solid path's coordinates and the distant light's azimuth,
leaving translations on the `use` instances. This avoids Skia's
[lighting Z transform](https://github.com/google/skia/blob/main/src/effects/imagefilters/SkLightingImageFilter.cpp)
without introducing intermediate rasters. `tst/lighting_reference.py` restricts
the conversion to the reviewed path commands and filter operations. A flat
surface's calculated specular intensity guards each export at two scales;
the original and converted corpus drawing agree within one native channel level.

`skia_textures` preserves float precision through the repeated noise filters in
`openclipart/258054`. `tst/texture_reference.py` expands the path/group instances
and evaluates each texture before rotating its final PNG. The evaluated window
covers the viewport and the ancestor clips, with a halo derived from every
displacement, convolution and lighting neighborhood. Only solid rectangles,
uniform rotations and user-space clips are accepted. Rectangle coverage is its
exact area in each local pixel; the source SVG remains unchanged.

Linear component transfers become equivalent color matrices. The stock Skia
float graph supports ten noise octaves and convolution bias with preserved
alpha. This avoids both byte rounding in the browser's filter intermediates
and lighting transforms applied to a rotated filter frame. The export with
analytic rectangle coverage matches the Inkscape-source prototype pixel for
pixel; doubling its halo changes the final image by less than 0.001 byte level
on average.

Every export checks rectangle areas, transfer equations, convolution bias,
noise lattice points, and diffuse/specular lighting and clipping at twelve
rotations and two scales. `tst/texture_filters.py` checks the accepted geometry,
neighborhood bounds and rejected constructions. Use Chromium as `PROGRAM` and
the Skia Python dependencies described above; `tst/corpus/SKIA_TEXTURES` records
their versions.

`chromium_noise` compensates the extra half-unit offset in Skia's
[turbulence sampler](https://github.com/google/skia/blob/main/src/opts/SkRasterPipeline_opts.h).
It translates the filter frame and translates its source geometry back, so
the correction happens before noise sampling. Absolute primitive and light
positions move with the frame; opacity, clipping and blending stay outside it.
The reviewed cases also have constant-noise controls to check that the rest
of the drawing is preserved. A lattice-point check runs before each export
and rejects a browser whose sampling convention has changed. Unsupported
constructions, including stitched noise and SVG 1.1 background inputs, are
rejected by `tst/noise_reference.py`.

`chromium_noise_blend` additionally replaces a single sRGB background multiply
with CSS multiply for the reviewed opaque backdrop in `openclipart/245679`.
The original SVG stays intact. Constant-noise controls agree byte for byte,
and rendering the conversion with the native renderer differs by less than
0.000003 MAE. This avoids both unsupported browser `BackgroundImage` inputs
and resampling the noisy backdrop through an embedded SVG image.

`chromium_noise_local` handles the reviewed quarter-turn paths in
`openclipart/290478`. Skia's [lighting Z transform](https://github.com/google/skia/blob/main/src/effects/imagefilters/SkLightingImageFilter.cpp)
can reverse or erase the light's elevation under rotation. The external browser
first renders each filter without that rotation, on a grid aligned with the
final pixels, then rotates the resulting PNG. The completed backdrop is also
an explicit browser-rendered PNG. `tst/local_filter_reference.py` restricts this
conversion to plain paths, quarter-turn rotations and supported ancestors.

`chromium_stitch` replaces the reviewed first `feTurbulence` primitive with
noise drawn by stock Batik (`BATIK` names its rasterizer jar; `JAVA` selects
Java), then renders the remaining filter in Chromium. Batik preserves the
tile origin required by `stitchTiles`; a half-device-pixel viewBox adjustment
makes it sample pixel centres. A lattice-point check guards that adjustment.
`tst/stitch_reference.py` keeps the original tile region and colour space,
and restricts this conversion to single-octave noise, default bounding-box
regions and untransformed filter frames. Remaining unstitched noise uses
the `chromium_noise` correction. Corpus SVGs are unchanged.

`inkscape_noise` compensates Inkscape's
[pixel-corner turbulence sampling](https://gitlab.com/inkscape/inkscape/-/blob/INKSCAPE_1_4_4/src/display/nr-filter-turbulence.cpp)
for the reviewed untransformed filter in `openclipart/342673`. It moves the
filter frame backwards half a device pixel and the source geometry forwards;
the correction follows the export resolution. A lattice-point check runs
before export. The drawing before its noise stages and a constant-noise
control retain identical pixels. `tst/noise_reference.py` restricts this
conversion to uniform scaling and untransformed frames without nested viewports.

## Building

You need clang 21 or GCC 16, Python 3, libpng, wasm2c from
[wabt](https://github.com/WebAssembly/wabt) and
[SIMDe](https://github.com/simd-everywhere/simde). The test driver decodes
the raster images of a document with `ext/decode/decode.simd128.wasm`,
ImageMagick as a pure WebAssembly module, which the build turns into C
with wasm2c; see [its README](ext/decode/README.md).

```
./build          # .build/svg, the test driver
./build all      # meanrms, maxrms, struct, aa and the candidates
```

`python3 tst/filter_transforms.py .build/svg` checks diffuse and specular
lighting against a planar alpha ramp's calculated values at two sizes,
including transforms that exceed the intermediate filter layer's area limit
and root filters at three device scales.

`python3 tst/filter_inputs.py .build/svg` checks clipping of filter inputs
in both colour spaces and turbulence at
lattice vertices, whose values are known without a reference renderer, and
its periodicity at large positive and negative coordinates. It also checks that
stitched tiles retain that periodicity when their origin moves, and that
rotated and reflected intermediate layers preserve the noise sampling grid.

`python3 tst/png_gamma.py .build/svg` checks PNG gamma conversion for RGB,
palette and grayscale images, including alpha and the sRGB chunk's precedence.

`python3 tst/marker_close.py .build/svg` checks automatic marker directions
against explicit geometric angles for ordinary closes, repeated closes and
paths that return to their start before closing.

`python3 tst/blend_endpoints.py .build/svg` checks CSS and filter blending
against calculated colours for gray backdrops and black/white luminosity,
including partial opacity and chromatic backdrops.

`python3 tst/fill_winding.py .build/svg` checks exact pixel coverage of adjacent,
disjoint, overlapping and nested rectangles with both fill rules and contour
directions, including fractions of a pixel along either axis at two scales.

`python3 tst/filter_background.py .build/svg` checks SVG 1.1 background
scopes, alpha, clipping, inheritance, transformed layers, CSS blending and
embedded documents against calculated colours.

`python3 tst/filter_paint.py .build/svg` checks fill and stroke paint inputs,
including opacity, gradients, patterns and blur at the filter region edges
in both colour spaces.

The Nix flake supplies Clang 21 with libc++, wasm2c, SIMDe, libpng and Python;
the repository's build runner still does the build in the working tree. Both
shells use the same persistent `.build`, so unchanged objects are reused when
switching configurations:

```
nix develop -c svg-build                         # regular build
nix develop .#sanitized -c svg-build             # ASan + UBSan build
nix develop .#sanitized -c svg-sanitizers        # build and smoke test
nix develop .#sanitized -c svg-fuzz -runs=40000  # libFuzzer over tst/
```

The sanitizer shell instruments the C++ renderer and libstd. The generated
wasm2c decoder shards and their C runtime are trusted, left uninstrumented and
therefore retain the same `.build` cache keys in both configurations. The fuzz
target uses a null raster decoder, so wasm2c is not linked into `.build/fuzz`.

`svg render IN.svg OUT.png WIDTH HEIGHT` draws one file;
`svg test IN.svg SCORE REFERENCE.png...` writes the errors and the shares
of wrong pixels of one case;
`svg bench LIST REPEAT` times parsing and drawing into memory for every
`PATH WIDTH HEIGHT` line of LIST, the file read beforehand, and prints the
fastest of REPEAT runs in nanoseconds.

## License

MIT, see [LICENSE](LICENSE).
