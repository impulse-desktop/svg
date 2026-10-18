# Filter Effects Module Level 1

## Filter property grammar and reference: filter-property-chain, blend-render-order, filter-chain-offscreen (exists)

- [x] filter-effects-1.md#propdef-filter: filter value is none or a filter-value-list ([filter-function] | url)+; initial none; computed value as specified; not inherited.
- [x] filter-effects-1.md#propdef-filter: filter applies to SVG container elements other than defs, all graphics elements, and the use element.
- [x] filter-effects-1.md#typedef-filter-url: a url() referencing a non-existent object or an object that is not a filter element makes the whole filter chain ignored; no filter is applied.
- [x] filter-effects-1.md#FilterProperty: the list of filter functions and filter references is applied in the order given.
- [x] filter-effects-1.md#FilterProperty: the first function or filter reference takes SourceGraphic as input; each later one takes the previous one's output as its SourceGraphic, even a filter reference naming an alternate in.
- [x] filter-effects-1.md#FilterProperty: color-interpolation-filters has no effect on filter functions; filter functions operate in sRGB.
- [x] filter-effects-1.md#FilterProperty: the compositing order is filter, then clipping, masking and opacity.
- [x] filter-effects-1.md#FilterProperty: filter is a presentation attribute for SVG elements.

## Filter element and filter region: filter-region, filter-root (exists)

- [x] filter-effects-1.md#element-attrdef-filter-filterunits: filterUnits is userSpaceOnUse or objectBoundingBox; initial objectBoundingBox.
- [x] filter-effects-1.md#element-attrdef-filter-primitiveunits: primitiveUnits is userSpaceOnUse or objectBoundingBox; initial userSpaceOnUse; sets the coordinate system of primitive lengths and of the primitive subregion attributes.
- [x] filter-effects-1.md#element-attrdef-filter-primitiveunits: for a number-optional-number value under objectBoundingBox, a single number is expanded to two before the objectBoundingBox scaling is applied.
- [x] filter-effects-1.md#filter-region: x, y, width, height set a rectangular filter region in the filterUnits coordinate system; initial x/y -10%, width/height 120%.
- [x] filter-effects-1.md#filter-region: the filter region is a hard clip on every filter primitive of the filter; effects that would extend past it (e.g. a large feGaussianBlur stdDeviation) are clipped.
- [x] filter-effects-1.md#element-attrdef-filter-filterres: filterRes was removed from this specification.
- [x] filter-effects-1.md#element-attrdef-filter-filterres: properties inherit into the filter element from its ancestors, not from the element that references the filter.
- [x] filter-effects-1.md#element-attrdef-filter-filterres: a filter element is never rendered directly; display on it or its ancestors does not stop it being referenced.

## Filter primitive color handling: filter-primitive-values, fe-offset-tile-merge, filter-color (exists)

- [x] filter-effects-1.md#FilterPrimitivesOverviewIntro: filter primitives operate on premultiplied RGBA samples unless stated otherwise; feColorMatrix and feComponentTransfer operate on non-premultiplied values instead.
- [x] filter-effects-1.md#FilterPrimitivesOverviewIntro: every filter primitive's RGBA result is clamped to the allowed color/opacity range, e.g. negative values are raised to zero.
- [x] filter-effects-1.md#filtersColorSpace: the color space of a filter primitive's operations is set by color-interpolation-filters on that primitive, not by color-interpolation.
- [x] filter-effects-1.md#filtersColorSpace: pixels left undefined by a filter primitive (e.g. shifted off by feOffset) are set to transparent black.

## Filter primitive common attributes and references: filter-primitive-inputs, filter-inputs (exists)

- [x] filter-effects-1.md#element-attrdef-filter-primitive-x: x, y initial 0%; width, height initial 100%, in the primitiveUnits coordinate system.
- [x] filter-effects-1.md#element-attrdef-filter-primitive-width: a negative or zero width or height disables the primitive: its result is transparent black.
- [x] filter-effects-1.md#element-attrdef-filter-primitive-result: result is a custom-ident naming this primitive's output for a later in/in2 reference.
- [x] filter-effects-1.md#element-attrdef-filter-primitive-in: in defaults to SourceGraphic for the first primitive in the filter, and to the previous primitive's result for any later one.
- [x] filter-effects-1.md#element-attrdef-filter-primitive-in: a reference to a result name that occurs more than once resolves to the closest preceding primitive with that result.
- [x] filter-effects-1.md#element-attrdef-filter-primitive-in: a forward reference to a result, and a reference to a non-existent result, are both treated as if no in value had been given.
- [x] filter-effects-1.md#attr-valuedef-in-sourcegraphic: SourceGraphic is the filtered element's content rasterized in linear RGBA; its alpha is the exact per-pixel coverage.
- [x] filter-effects-1.md#attr-valuedef-in-sourcealpha: SourceAlpha is SourceGraphic with RGB forced to black, keeping SourceGraphic's alpha channel.

## Filter paint-server inputs: filter-paint-readers (exists)

- [ ] filter-effects-1.md#attr-valuedef-in-fillpaint: FillPaint is the target element's fill property value as an infinite-extent image; a paint server in it uses the filtered element's own reference box (objectBoundingBox or the nearest viewport).
- [ ] filter-effects-1.md#attr-valuedef-in-strokepaint: StrokePaint is the same, taken from the stroke property.

## Background input and isolation backdrop: filter-background (exists)

- [ ] filter-effects-1.md#attr-valuedef-in-backgroundimage: BackgroundImage is the backdrop defined by the current isolation group behind the filter region, at the time the filter was invoked.
- [ ] filter-effects-1.md#attr-valuedef-in-backgroundalpha: BackgroundAlpha is BackgroundImage with only its alpha channel kept.

## Deprecated enable-background: enable-background (exists)

- [ ] filter-effects-1.md#AccessBackgroundImage: this specification does not support the SVG 1.1 enable-background property; user agents must use the isolation property to define the BackgroundImage/BackgroundAlpha backdrop instead.

## Filter primitive tree: filter-primitive-inputs, filter-need-join (exists)

- [x] filter-effects-1.md#FilterPrimitiveTree: every filter structure of primitives chained by in/result forms a filter primitive tree, rooted at its most subsequent primitive.
- [x] filter-effects-1.md#primary-filter-primitive-tree: the primary filter primitive tree is the one rooted at the filter element's last primitive child; only it contributes to the filter's output.
- [x] filter-effects-1.md#FilterPrimitiveTree: any other filter primitive tree in the same filter element may be ignored by the implementation.
- [x] filter-effects-1.md#FilterPrimitiveTree: a filter element with no filter primitive tree (no primitive children) makes the element it is applied to not render.

## Filter primitive subregion defaults and clipping: filter-primitive-inputs, filter-region, fe-flood, filter-subregions-outside (exists)

- [x] filter-effects-1.md#filter-primitive-subregion: x, y, width, height of a primitive default to the tightest union of the subregions of its referenced primitives.
- [x] filter-effects-1.md#filter-primitive-subregion: with no referenced node, with a referenced node that is a standard keyword input, or for feTile, the default subregion is 0%, 0%, 100%, 100% of the filter region, i.e. the filter region itself.
- [x] filter-effects-1.md#filter-primitive-subregion: a negative or zero subregion width or height disables that primitive.
- [x] filter-effects-1.md#filter-primitive-subregion: the filter region hard-clips a primitive's input images; the primitive subregion hard-clips its result.
- [x] filter-effects-1.md#filter-primitive-subregion: intermediate offscreens need not exceed the intersection of the primitive subregion and the filter region, but must be big enough for any pixel partly inside either.

## Filter primitive subregion as an exact tile boundary: filter-support-abut (exists)

- [x] filter-effects-1.md#filter-primitive-subregion: because the subregion hard-clips a primitive's result, two primitives given abutting subregions must tile without a gap or an overlap.

## feBlend: fe-blend-formulas, feblend-no-composite (exists)

- [x] filter-effects-1.md#element-attrdef-feblend-mode: mode is a blend-mode keyword, blending in (source Cs) over in2 (backdrop Cb); initial normal.
- [x] filter-effects-1.md#element-attrdef-feblend-no-composite: no-composite applies the blend mode's mixing formula without alpha compositing; otherwise the blend mode is combined with the Source Over operator.
- [x] filter-effects-1.md#element-attrdef-feblend-in2: in2 is the second (backdrop) input image.
- [x] filter-effects-1.md#element-attrdef-feblend-mode: normal with compositing equals feComposite operator="over", feMerge's own blending, and SVG's simple alpha compositing.

## feColorMatrix: fe-color-matrix-formulas, filter-color (exists)

- [x] filter-effects-1.md#elementdef-fecolormatrix: feColorMatrix applies a 5x4 matrix to non-premultiplied RGBA of every pixel.
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-type: type is matrix, saturate, hueRotate or luminanceToAlpha; initial matrix.
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: for type="matrix", values is 20 numbers, the identity matrix by default; for "hueRotate" and "saturate" it is a single number.
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: saturate's matrix is built from s; hueRotate's from cos/sin of the hueRotate angle, both using fixed 0.213/0.715/0.072-family luminance coefficients.
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: for type="luminanceToAlpha" values does not apply; a fixed matrix (0.2126, 0.7152, 0.0722) maps RGB into the alpha channel.
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: the initial values default differs by type: identity matrix for "matrix", 1 for "saturate", 0 (identity) for "hueRotate".
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: if the number of entries in values does not match what type requires, feColorMatrix acts as a pass-through filter (its output equals its input).

## feColorMatrix value-count edge cases: fe-color-matrix-formulas, filter-color-matrix-values (exists)

- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: saturate with no values attribute uses its initial value of 1 (no change).
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: hueRotate takes exactly one number of degrees; a values list of a different length is a pass-through filter per the type mismatch rule.

## feComponentTransfer and feFuncR/G/B/A: fe-component-transfer-formulas, filter-color (exists)

- [x] filter-effects-1.md#elementdef-fecomponenttransfer: feComponentTransfer remaps each of R, G, B, A independently through its own feFuncR/feFuncG/feFuncB/feFuncA function, on non-premultiplied values.
- [x] filter-effects-1.md#elementdef-fecomponenttransfer: if a kind of transfer function element occurs more than once the last occurrence is used; if one is absent it is treated as type="identity".
- [x] filter-effects-1.md#element-attrdef-fecomponenttransfer-type: type is identity, table, discrete, linear or gamma; initial identity (C'=C).
- [x] filter-effects-1.md#attr-valuedef-type-table: table interpolates tableValues v0..vn over n equal regions: for k/n <= C < (k+1)/n, C' = vk + (C - k/n)*n*(vk+1 - vk); C=1 gives C'=vn.
- [x] filter-effects-1.md#attr-valuedef-type-discrete: discrete steps through tableValues v0..vn-1: for k/n <= C < (k+1)/n, C'=vk; C=1 gives C'=vn-1.
- [x] filter-effects-1.md#attr-valuedef-type-linear: linear is C' = slope*C + intercept; slope initial 1, intercept initial 0.
- [x] filter-effects-1.md#attr-valuedef-type-gamma: gamma is C' = amplitude*pow(C, exponent) + offset; amplitude initial 1, exponent initial 1, offset initial 0.
- [x] filter-effects-1.md#element-attrdef-fecomponenttransfer-tablevalues: tableValues missing, or an empty list, makes the transfer function an identity.

## feComposite: blend-porter-duff, fe-composite-arithmetic, filter-composite (exists)

- [x] filter-effects-1.md#elementdef-fecomposite: feComposite combines in (source) and in2 (destination) pixel-wise via a Porter-Duff operator: over, in, out, atop, xor, lighter, or arithmetic; initial operator over.
- [x] filter-effects-1.md#elementdef-fecomposite: arithmetic computes result = k1*i1*i2 + k2*i1 + k3*i2 + k4 per channel, clamped to [0,1]; k1-k4 each initial 0.
- [x] filter-effects-1.md#element-attrdef-fecomposite-in2: in2 is the second (destination) input image.
- [x] filter-effects-1.md#elementdef-fecomposite: feComposite's result may extend beyond its input images' extent, subject to the filter primitive subregion rules.

## feComposite arithmetic chains: fe-composite-arithmetic, filter-arithmetic-readers (exists)

- [x] filter-effects-1.md#elementdef-fecomposite: an arithmetic feComposite's result, once named by result, is read like any other named result by later in/in2/feMergeNode references.

## feConvolveMatrix: filter-morphology-tile-convolve (exists)

- [ ] filter-effects-1.md#feConvolveMatrixElementFormula: each result color is sum(source*kernelMatrix, kernel rotated 180 degrees) / divisor + bias*alpha, over the order x order neighborhood positioned by targetX/targetY.
- [ ] filter-effects-1.md#element-attrdef-order: order gives orderX, orderY as positive integers (orderY defaults to orderX, truncated toward zero if not integral); initial 3.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-kernelmatrix: kernelMatrix must list exactly orderX*orderY numbers; a mismatched count makes feConvolveMatrix a pass-through filter.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-divisor: divisor initial is the sum of kernelMatrix (or 1 if that sum is 0); a specified divisor of 0 falls back to this default too.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-bias: bias is added to every channel after the divisor; initial 0.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-targetx: targetX must satisfy 0 <= targetX < orderX; default floor(orderX/2); targetY likewise for orderY.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-preservealpha: preserveAlpha false convolves all channels including alpha; true unpremultiplies color first and leaves alpha untouched; initial false.

## feConvolveMatrix edgeMode: convolve-edge-modes (exists)

- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-edgemode: edgeMode is duplicate, wrap, mirror or none; initial duplicate.
- [ ] filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-duplicate: duplicate extends the image by repeating its edge pixels.
- [ ] filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-wrap: wrap extends the image by taking pixels from the opposite edge.
- [ ] filter-effects-1.md#attr-valuedef-feguassianblur-edgemode-mirror: mirror extends the image by reflecting pixels across the edge.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-edgemode: none extends the image with transparent black (R,G,B,A all 0).

## feConvolveMatrix bias and preserveAlpha readback: convolve-bias (exists)

- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-bias: a nonzero bias shifts every output channel by that amount, letting values that would otherwise clamp to 0 or 1 be represented.

## kernelUnitLength: kernel-unit-length (exists)

- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength: kernelUnitLength is dx, dy (dy defaults to dx) in primitiveUnits, giving a scalable pixel grid for the kernel.
- [ ] filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength: a negative or zero kernelUnitLength falls back to the default of one device pixel.
- [ ] filter-effects-1.md#element-attrdef-fediffuselighting-kernelunitlength: feDiffuseLighting/feSpecularLighting kernelUnitLength sets dx, dy for the surface-normal Sobel deltas the same way; a negative or zero value also falls back to the default.

## feDiffuseLighting, feSpecularLighting and light sources: filter-lighting (exists)

- [ ] filter-effects-1.md#elementdef-fediffuselighting: feDiffuseLighting lights the alpha channel as a bump map with the diffuse Phong term; result is opaque RGBA (alpha always 1.0).
- [ ] filter-effects-1.md#element-attrdef-fediffuselighting-surfacescale: surfaceScale is the surface height when A_in=1; initial 1; the surface is Z(x,y) = surfaceScale * I(x,y).
- [ ] filter-effects-1.md#element-attrdef-fediffuselighting-diffuseconstant: diffuseConstant (kd) is non-negative; initial 1; D_r/g/b = kd * N.L * L_r/g/b.
- [ ] filter-effects-1.md#SurfaceNormalCalculations: the surface normal N is computed from a 3x3 Sobel gradient of the alpha image, using a different kernel at corners, edges and the interior.
- [ ] filter-effects-1.md#elementdef-fedistantlight: feDistantLight's light direction is constant, from azimuth (initial 0) and elevation (initial 0) in degrees.
- [ ] filter-effects-1.md#element-attrdef-fepointlight-x: fePointLight's x, y, z (each initial 0) give a light position in the primitiveUnits coordinate system, positive Z toward the viewer.
- [ ] filter-effects-1.md#element-attrdef-fespotlight-pointsatx: feSpotLight adds pointsAtX/Y/Z (each initial 0) and gives its light color a pow(-L.S, specularExponent) falloff toward that point; no light is emitted where L.S is positive.
- [ ] filter-effects-1.md#element-attrdef-fespotlight-limitingconeangle: limitingConeAngle restricts the lit region to a cone of that angle in degrees around the spot axis; unspecified means no cone limit.
- [ ] filter-effects-1.md#elementdef-fespecularlighting: feSpecularLighting uses the specular Blinn-Phong term instead: S_r/g/b = ks * pow(N.H, specularExponent) * L_r/g/b, and alpha = max(S_r, S_g, S_b), so it is not opaque.
- [ ] filter-effects-1.md#propdef-lighting-color: lighting-color gives the light's color; initial white; applies to feDiffuseLighting and feSpecularLighting; not inherited.

## Lighting subregion and parameter edges: filter-lighting-edges (exists)

- [ ] filter-effects-1.md#element-attrdef-fespecularlighting-specularexponent: feSpecularLighting's specularExponent (shininess) is a separate attribute from feSpotLight's own specularExponent (focus), each initial 1.
- [ ] filter-effects-1.md#filter-primitive-subregion: the y/height (or any) override on a lighting primitive's own x/y/width/height still clips its result to that subregion like any primitive.

## feDisplacementMap: displacement-channels (exists)

- [ ] filter-effects-1.md#elementdef-fedisplacementmap: P'(x,y) = P(x + scale*(XC(x,y)-.5), y + scale*(YC(x,y)-.5)), reading in2's channel selected by xChannelSelector/yChannelSelector as the displacement map.
- [ ] filter-effects-1.md#elementdef-fedisplacementmap: in stays premultiplied; the in2 displacement values are read as non-premultiplied color.
- [ ] filter-effects-1.md#elementdef-fedisplacementmap: color-interpolation-filters applies only to the in2 source, not to in, which keeps its own color space.
- [ ] filter-effects-1.md#element-attrdef-fedisplacementmap-scale: scale is the displacement scale factor in the primitiveUnits coordinate system; initial 0, which has no effect.
- [ ] filter-effects-1.md#element-attrdef-fedisplacementmap-xchannelselector: xChannelSelector/yChannelSelector pick R, G, B or A from in2; each initial A.

## feDropShadow: filter-dropshadow-image (exists)

- [ ] filter-effects-1.md#elementdef-fedropshadow: feDropShadow equals: blur in's alpha by stdDeviation, offset by dx/dy, flood with flood-color/flood-opacity, composite that over the offset blur with operator="in".
- [ ] filter-effects-1.md#elementdef-fedropshadow: the composited flood-and-blur result is then merged under the original, unmodified in.
- [ ] filter-effects-1.md#element-attrdef-fedropshadow-dx: dx, dy each have initial value 2 and are forwarded to the internal feOffset.
- [ ] filter-effects-1.md#element-attrdef-fedropshadow-stddeviation: stdDeviation is a number-optional-number with initial value 2, forwarded to the internal feGaussianBlur.

## feDropShadow stdDeviation edge values: filter-dropshadow-huge (exists)

- [ ] filter-effects-1.md#element-attrdef-fedropshadow-stddeviation: a negative component of stdDeviation disables the blur in that axis the same way it would for feGaussianBlur.

## feFlood and flood-color/flood-opacity: fe-flood

- [x] filter-effects-1.md#elementdef-feflood: feFlood fills a rectangle the size of its own filter primitive subregion with flood-color and flood-opacity.
- [x] filter-effects-1.md#propdef-flood-color: flood-color is a color; initial black; applies to feFlood and feDropShadow; not inherited.
- [x] filter-effects-1.md#propdef-flood-opacity: flood-opacity is an opacity value; initial 1; computed value is clamped to [0,1]; its alpha multiplies with any alpha already in flood-color.

## feGaussianBlur stdDeviation: filter-primitives-edge (exists)

- [ ] filter-effects-1.md#elementdef-fegaussianblur: the Gaussian kernel is the separable product H(x)*I(y) of two 1-D Gaussians with standard deviations s (x) and t (y).
- [ ] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: stdDeviation is one or two numbers: one value is used for both axes, two give x then y; initial 0.
- [ ] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: a negative value, or zero in both axes, disables the primitive, so its result is its input image unchanged.
- [ ] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: if stdDeviation is 0 in exactly one axis, the blur applies only along the other axis.

## feGaussianBlur edgeMode: blur-edge-modes (exists)

- [ ] filter-effects-1.md#element-attrdef-fegaussianblur-edgemode: feGaussianBlur's edgeMode is duplicate, wrap, mirror or none, defined as for feConvolveMatrix; initial none.

## feGaussianBlur on infinite-extent inputs: filter-blur-paths (exists)

- [ ] filter-effects-1.md#elementdef-fegaussianblur: if the input has infinite extent and is a constant image (e.g. FillPaint of a solid color), feGaussianBlur has no effect.

## feImage: feimage-refs (exists)

- [ ] filter-effects-1.md#elementdef-feimage: an feImage href to a stand-alone image resource renders like the image element; a reference to an element renders like the use element; the current coordinate system follows primitiveUnits either way.
- [ ] filter-effects-1.md#elementdef-feimage: an href that is empty (zero width or height), fails to resolve, does not exist, or cannot be displayed fills the primitive subregion with transparent black.
- [ ] filter-effects-1.md#element-attrdef-feimage-preserveaspectratio: preserveAspectRatio has initial value xMidYMid meet and is processed as for the image element.
- [ ] filter-effects-1.md#element-attrdef-feimage-xlinkhref: when both xlink:href and href are given, href overrides xlink:href.

## feMerge/feMergeNode: fe-offset-tile-merge, filter-arithmetic-merge-reader (exists)

- [x] filter-effects-1.md#elementdef-femerge: feMerge composites its feMergeNode children with the over operator, the first child on the bottom and the last child on top.
- [x] filter-effects-1.md#elementdef-femerge: an implementation may evaluate feMerge one layer at a time bottom to top instead of building one combined RGBA buffer, since merging is associative.

## feMorphology: fe-morphology-values, filter-morphology-edge (exists)

- [x] filter-effects-1.md#elementdef-femorphology: the dilate/erode kernel is a rectangle 2*x-radius by 2*y-radius; dilate takes the per-channel componentwise maximum over it, erode the minimum.
- [x] filter-effects-1.md#element-attrdef-femorphology-operator: operator is erode or dilate; initial erode.
- [x] filter-effects-1.md#element-attrdef-femorphology-radius: radius is one or two numbers (x-radius, y-radius); initial 0; a negative or zero radius disables the primitive (result equals input).
- [x] filter-effects-1.md#elementdef-femorphology: because feMorphology works on premultiplied values, its result color channels are always <= its result alpha.

## feOffset: fe-offset-tile-merge, filter-offset-vertical (exists)

- [x] filter-effects-1.md#elementdef-feoffset: feOffset shifts its input by dx, dy in the primitiveUnits coordinate system.
- [x] filter-effects-1.md#element-attrdef-feoffset-dx: dx, dy each have initial value 0.

## feTile: fe-offset-tile-merge, filter-tile-clipped (exists)

- [x] filter-effects-1.md#elementdef-fetile: feTile fills its own filter primitive subregion by replicating its input's filter primitive subregion as tiles at (x + i*width, y + j*height) for integer i, j.

## feTurbulence: filter-turbulence-displacement (exists)

- [ ] filter-effects-1.md#elementdef-feturbulence: feTurbulence synthesizes a Perlin-noise image filling its whole filter primitive subregion.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-basefrequency: baseFrequency is one or two numbers (x then y, or both); initial 0; negative values are unsupported.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-numoctaves: numOctaves is an integer; initial 1; negative values are unsupported.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-seed: seed seeds the pseudo-random generator; initial 0; it is truncated toward zero before use.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-type: type is fractalNoise or turbulence; initial turbulence.
- [ ] filter-effects-1.md#elementdef-feturbulence: the generated color and alpha values are produced in the color space set by color-interpolation-filters.

## feTurbulence stitchTiles: filter-turbulence-stitch-far (exists)

- [ ] filter-effects-1.md#element-attrdef-feturbulence-stitchtiles: stitchTiles is stitch or noStitch; initial noStitch; noStitch may show visible discontinuities at tile borders.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-stitchtiles: stitch adjusts baseFrequency per axis to lowFreq=floor(width*freq)/width or hiFreq=ceil(width*freq)/width, whichever is closer in ratio to freq.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-stitchtiles: this adjustment makes the subregion's width and height hold an integral number of Perlin tiles for the first octave, so tiled edges line up.

## feTurbulence numeric edge cases: filter-numbers-invalid (exists)

- [ ] filter-effects-1.md#element-attrdef-feturbulence-numoctaves: numOctaves must be an integer value for the noise function; a fractional or malformed value is handled as an invalid attribute, not a special-cased truncation like seed.
- [ ] filter-effects-1.md#element-attrdef-feturbulence-seed: only seed is explicitly truncated toward zero by the spec; baseFrequency and numOctaves have no such truncation rule of their own.

## color-interpolation-filters: filter-interpolation-space, filter-edge-blend-turbulence (exists)

- [x] filter-effects-1.md#propdef-color-interpolation-filters: color-interpolation-filters is auto, sRGB or linearRGB; initial linearRGB; inherited; applies to all filter primitives.
- [x] filter-effects-1.md#valdef-color-interpolation-filters-auto: auto lets the user agent pick either sRGB or linearRGB.
- [x] filter-effects-1.md#propdef-color-interpolation-filters: color-interpolation-filters has no effect on feOffset, feImage, feTile or feFlood, which do no color-space-dependent math.

## Filter functions' equivalent primitives: filter-function-equivalents, css-filter-functions (exists)

- [x] filter-effects-1.md#grayscaleEquivalent: grayscale(amount) is equivalent to an feColorMatrix type="matrix" with coefficients built from (1 - amount).
- [x] filter-effects-1.md#sepiaEquivalent: sepia(amount) is equivalent to an feColorMatrix type="matrix" with the sepia coefficients built from (1 - amount).
- [x] filter-effects-1.md#saturateEquivalent: saturate(amount) is equivalent to feColorMatrix type="saturate" values="amount".
- [x] filter-effects-1.md#huerotateEquivalent: hue-rotate(angle) is equivalent to feColorMatrix type="hueRotate" values="angle".
- [x] filter-effects-1.md#invertEquivalent: invert(amount) is equivalent to feComponentTransfer feFuncR/G/B type="table" tableValues="amount (1-amount)".
- [x] filter-effects-1.md#opacityEquivalent: opacity(amount) is equivalent to feComponentTransfer feFuncA type="table" tableValues="0 amount".
- [x] filter-effects-1.md#brightnessEquivalent: brightness(amount) is equivalent to feComponentTransfer feFuncR/G/B type="linear" slope="amount".
- [x] filter-effects-1.md#contrastEquivalent: contrast(amount) is equivalent to feComponentTransfer feFuncR/G/B type="linear" slope="amount" intercept="-(0.5*amount)+0.5".
- [x] filter-effects-1.md#blurEquivalent: blur(radius) is equivalent to feGaussianBlur stdDeviation="radius radius", with edgeMode none for the filter property and duplicate for the filter() image function.
- [x] filter-effects-1.md#dropshadowEquivalent: drop-shadow() is equivalent to the same feGaussianBlur/feOffset/feFlood/feComposite/feMerge chain as feDropShadow.

## Filter functions grammar and defaults: filter-function-grammar, css-filter-functions (exists)

- [x] filter-effects-1.md#funcdef-filter-blur: blur() takes an optional length, no percentage; negative is not allowed; default when omitted is 0px.
- [x] filter-effects-1.md#funcdef-filter-blur: the ink overflow of a Gaussian blur is the axis-aligned bound of the area the implementation actually draws the blur into.
- [x] filter-effects-1.md#funcdef-filter-brightness: brightness() takes an optional number or percentage; 0% is black, 100% unchanged, over 100% allowed brighter; negative not allowed; default 1.
- [x] filter-effects-1.md#funcdef-filter-contrast: contrast() takes an optional number or percentage; 0% is flat gray, 100% unchanged, over 100% allowed; negative not allowed; default 1.
- [x] filter-effects-1.md#funcdef-filter-saturate: saturate() takes an optional number or percentage; 0% fully desaturated, 100% unchanged, over 100% allowed; negative not allowed; default 1.
- [x] filter-effects-1.md#funcdef-filter-grayscale: grayscale()/invert()/sepia() take an optional number or percentage; 100% is full effect, over 100% is clamped to 1, negative not allowed; default when omitted is 1 for each.
- [x] filter-effects-1.md#funcdef-filter-opacity: opacity() takes an optional number or percentage, multiplying the image's samples; 0% transparent, 100% unchanged, over 100% clamped to 1; negative not allowed; default 1.
- [x] filter-effects-1.md#funcdef-filter-hue-rotate: hue-rotate() takes an optional angle (its unit may be omitted only if the value is zero); default 0deg; implementations must not normalize values past 360deg.

## drop-shadow() syntax: css-filter-syntax (exists)

- [ ] filter-effects-1.md#funcdef-filter-drop-shadow: drop-shadow() is [ <color>? && <length>{2,3} ]; a missing length is 0, a missing color is the color property's value.
- [ ] filter-effects-1.md#funcdef-filter-drop-shadow: the third length, if given, is the blur standard deviation, not a box-shadow blur radius; no 4th (spread) length and no comma-separated multiple shadows are accepted.
- [ ] filter-effects-1.md#funcdef-filter-drop-shadow: the ink overflow of drop-shadow() is the extent of its offset plus the extent of its blur, if any.

## Filter region for shorthands: filter-layers (exists)

- [ ] filter-effects-1.md#filter-region-for-shorthands: every filter function's user-agent-defined filter region must cover the element's own visible content, including any overflow.
- [ ] filter-effects-1.md#filter-region-for-shorthands: when a shorthand filter function expands the visible area, such as blur() or drop-shadow(), its filter region must be expanded to cover that too.

## Notes

- filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: the spec's own 3-box-blur approximation (~3% off a true Gaussian) is explicitly permitted; an exact per-pixel Gaussian would cost real speed for no visible gain.
- filter-effects-1.md#SurfaceNormalCalculations: the per-pixel 3x3 Sobel normal (feDiffuseLighting/feSpecularLighting) and per-pixel kernel (feConvolveMatrix) are costly done exactly; kernelUnitLength resampling is the spec's own escape hatch.
- filter-effects-1.md#feTurbulenceElement: the spec gives one exact PRNG/Perlin C algorithm plus a "10000th value" test vector, i.e. byte-exact reproduction; this is heavy per-pixel work with little room for a faster equivalent noise function.
- filter-effects-1.md#issue-4fd9d6f6: the text makes a kernelMatrix length mismatch a pass-through filter, yet the spec itself still marks this exact case an open issue (#237); the behavior is not settled.
- filter-effects-1.md#issue-1d845580: the spec flags its own feDisplacementMap formula as not matching any current implementation (#113), without saying which side should change.
- filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength: kernelUnitLength is marked deprecated and "will be removed," yet stays a normatively defined attribute with its own default and fallback rule.
