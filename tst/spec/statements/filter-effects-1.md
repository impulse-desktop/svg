# Filter Effects Module Level 1

## Filter property grammar and reference: filter-property-chain, blend-render-order, filter-chain-offscreen (exists)

- [x] filter-effects-1.md#propdef-filter: filter value is none or a filter-value-list ([filter-function] | url)+; initial none; computed value as specified; not inherited. [filter-property-chain#missing filter-property-chain#order]
- [x] filter-effects-1.md#propdef-filter: filter applies to SVG container elements other than defs, all graphics elements, and the use element. [filter-background-image#group filter-property-chain#missing fe-convolve-formula#kernel]
- [x] filter-effects-1.md#typedef-filter-url: a url() referencing a non-existent object or an object that is not a filter element makes the whole filter chain ignored; no filter is applied. [filter-property-chain#missing filter-property-chain#no-filter]
- [x] filter-effects-1.md#FilterProperty: the list of filter functions and filter references is applied in the order given. [filter-property-chain#order]
- [x] filter-effects-1.md#FilterProperty: the first function or filter reference takes SourceGraphic as input; each later one takes the previous one's output as its SourceGraphic, even a filter reference naming an alternate in. [filter-property-chain#source-alpha filter-property-chain#order]
- [x] filter-effects-1.md#FilterProperty: color-interpolation-filters has no effect on filter functions; filter functions operate in sRGB. [filter-property-chain#function-srgb]
- [x] filter-effects-1.md#FilterProperty: the compositing order is filter, then clipping, masking and opacity. [blend-render-order#filter-clip blend-render-order#mask-blend]
- [x] filter-effects-1.md#FilterProperty: filter is a presentation attribute for SVG elements. [filter-region#default-region]

## Filter element and filter region: filter-region, filter-thin-region, filter-root (exists)

- [x] filter-effects-1.md#element-attrdef-filter-filterunits: filterUnits is userSpaceOnUse or objectBoundingBox; initial objectBoundingBox. [filter-region#default-region filter-region#user-space]
- [x] filter-effects-1.md#element-attrdef-filter-primitiveunits: primitiveUnits is userSpaceOnUse or objectBoundingBox; initial userSpaceOnUse; sets the coordinate system of primitive lengths and of the primitive subregion attributes. [filter-region#primitive-units]
- [x] filter-effects-1.md#element-attrdef-filter-primitiveunits: for a number-optional-number value under objectBoundingBox, a single number is expanded to two before the objectBoundingBox scaling is applied. [filter-region#primitive-units-radius]
- [x] filter-effects-1.md#filter-region: x, y, width, height set a rectangular filter region in the filterUnits coordinate system; initial x/y -10%, width/height 120%. [filter-region#default-region filter-region#user-space]
- [x] filter-effects-1.md#filter-region: the filter region is a hard clip on every filter primitive of the filter; effects that would extend past it (e.g. a large feGaussianBlur stdDeviation) are clipped. Thin regions keep their fractional row or column coverage under transforms. [filter-region#cut filter-region#group-region filter-thin-region#direct filter-thin-region#scaled filter-thin-region#huge-width filter-thin-region#flood filter-thin-region#half-row filter-thin-region#whole-row filter-thin-region#cross-row filter-thin-region#thin-column]
- [x] filter-effects-1.md#element-attrdef-filter-filterres: filterRes was removed from this specification. [filter-region#filter-res]
- [x] filter-effects-1.md#element-attrdef-filter-filterres: properties inherit into the filter element from its ancestors, not from the element that references the filter. [filter-region#inherit]
- [x] filter-effects-1.md#element-attrdef-filter-filterres: a filter element is never rendered directly; display on it or its ancestors does not stop it being referenced. [filter-region#display]

## Filter primitive color handling: filter-primitive-values, fe-offset-tile-merge, filter-color (exists)

- [x] filter-effects-1.md#FilterPrimitivesOverviewIntro: filter primitives operate on premultiplied RGBA samples unless stated otherwise; feColorMatrix and feComponentTransfer operate on non-premultiplied values instead. [fe-composite-arithmetic#average filter-primitive-values#unpremultiplied]
- [x] filter-effects-1.md#FilterPrimitivesOverviewIntro: every filter primitive's RGBA result is clamped to the allowed color/opacity range, e.g. negative values are raised to zero. [filter-primitive-values#clamped]
- [x] filter-effects-1.md#filtersColorSpace: the color space of a filter primitive's operations is set by color-interpolation-filters on that primitive, not by color-interpolation. [filter-primitive-values#space filter-interpolation-space#linear-default]
- [x] filter-effects-1.md#filtersColorSpace: pixels left undefined by a filter primitive (e.g. shifted off by feOffset) are set to transparent black. [fe-offset-tile-merge#offset]

## Filter primitive common attributes and references: filter-primitive-inputs, filter-inputs (exists), filter-subregion-composite, filter-input-region

- [x] filter-effects-1.md#element-attrdef-filter-primitive-x: x, y initial 0%; width, height initial 100%, in the primitiveUnits coordinate system. [filter-region#default-region fe-flood#subregion]
- [x] filter-effects-1.md#FilterPrimitiveSubRegion: a subregion boundary strictly inside the input clips the result by its fractional pixel area after compositing; SourceGraphic remains opaque across an internal colour boundary, including under fractional use placement, in linearRGB and sRGB (D2) [filter-subregion-composite#linearRGB-in filter-subregion-composite#linearRGB-out filter-subregion-composite#linearRGB-atop filter-subregion-composite#linearRGB-over filter-subregion-composite#linearRGB-xor filter-subregion-composite#linearRGB-k1 filter-subregion-composite#linearRGB-k2 filter-subregion-composite#linearRGB-k3 filter-subregion-composite#linearRGB-k4 filter-subregion-composite#sRGB-in filter-subregion-composite#sRGB-out filter-subregion-composite#sRGB-atop filter-subregion-composite#sRGB-over filter-subregion-composite#sRGB-xor filter-subregion-composite#sRGB-k1 filter-subregion-composite#sRGB-k2 filter-subregion-composite#sRGB-k3 filter-subregion-composite#sRGB-k4]
- [x] filter-effects-1.md#FilterPrimitiveSubRegion: the filter region hard-clips primitive input images too, before shifting, blurring, convolution or erosion; a primitive subregion separately clips its result, including for original SourceGraphic and a preceding filter in a chain. [filter-input-region#right-empty filter-input-region#left-empty filter-input-region#down-empty filter-input-region#up-empty filter-input-region#right-partial filter-input-region#fractional-shift filter-input-region#outside-input-stays-empty filter-input-region#off-viewport-input-stays-empty filter-input-region#chain-input-clip filter-input-region#original-input-clip filter-input-region#blur-input-edge filter-input-region#blur-unequal filter-input-region#blur-colours filter-input-region#primitive-result-clip filter-input-region#convolution-input-edge filter-input-region#erosion-input-edge filter-source-paint-edges#source-offset-right filter-source-paint-edges#source-offset-left filter-source-paint-edges#source-offset-down filter-source-paint-edges#source-offset-up filter-source-paint-edges#source-alpha-offset filter-source-paint-edges#source-blur-input filter-source-paint-edges#source-alpha-blur-input]
- [x] filter-effects-1.md#element-attrdef-filter-primitive-width: a negative or zero width or height disables the primitive: its result is transparent black. [filter-primitive-inputs#zero-subregion]
- [x] filter-effects-1.md#element-attrdef-filter-primitive-result: result is a custom-ident naming this primitive's output for a later in/in2 reference. [filter-primitive-inputs#duplicate-result filter-primitive-inputs#previous]
- [x] filter-effects-1.md#element-attrdef-filter-primitive-in: in defaults to SourceGraphic for the first primitive in the filter, and to the previous primitive's result for any later one. [filter-primitive-inputs#previous filter-region#cut]
- [x] filter-effects-1.md#element-attrdef-filter-primitive-in: a reference to a result name that occurs more than once resolves to the closest preceding primitive with that result. [filter-primitive-inputs#duplicate-result]
- [x] filter-effects-1.md#element-attrdef-filter-primitive-in: a forward reference to a result, and a reference to a non-existent result, are both treated as if no in value had been given. [filter-primitive-inputs#forward-result filter-primitive-inputs#missing-result]
- [x] filter-effects-1.md#attr-valuedef-in-sourcegraphic: SourceGraphic is the filtered element's content rasterized in linear RGBA; its alpha is the exact per-pixel coverage. Its painted edges include strokes and the output of nested filters, clips and masks; the geometric objectBoundingBox used to size the filter still excludes stroke. A coincident painted edge keeps its existing antialiasing (D2). [filter-region#cut fe-offset-tile-merge#offset filter-source-paint-edges#plain-fill filter-source-paint-edges#filled-stroke filter-source-paint-edges#stroke-only filter-source-paint-edges#group-stroke filter-source-paint-edges#use-stroke filter-source-paint-edges#translucent-stroke filter-source-paint-edges#translated-source filter-source-paint-edges#scaled-source filter-source-paint-edges#quarter-turn-layer filter-source-paint-edges#reflected-layer filter-source-paint-edges#zero-geometry-height filter-source-paint-edges#thin-geometry-width filter-source-paint-edges#region-at-paint-edge filter-source-paint-edges#region-inside-paint filter-source-paint-edges#offset-input filter-source-paint-edges#composite-input filter-source-paint-edges#transfer-input filter-source-paint-edges#convolve-input filter-source-paint-edges#nested-wide-filter filter-source-paint-edges#nested-wide-stroke filter-source-paint-edges#nested-offset-source filter-source-paint-edges#nested-clipped-source filter-source-paint-edges#nested-clip-program filter-source-paint-edges#nested-masked-source filter-source-paint-edges#nested-mask-program filter-source-paint-edges#nested-mask-intersect filter-source-paint-edges#nested-mask-add filter-source-paint-edges#nested-mask-exclude filter-source-paint-edges#nested-mask-subtract filter-source-paint-edges#zero-opacity-sibling filter-source-paint-edges#empty-source filter-source-paint-edges#flood-empty-source]
- [x] filter-effects-1.md#FilterProperty: consecutive filters pass the preceding result's painted edges along with its pixels, including when a function precedes a filter reference. [filter-source-paint-edges#chain-same-region filter-source-paint-edges#chain-wide-first filter-source-paint-edges#function-chain]
- [x] filter-effects-1.md#attr-valuedef-in-sourcealpha: SourceAlpha is SourceGraphic with RGB forced to black, keeping SourceGraphic's alpha channel. [filter-primitive-inputs#source-alpha filter-property-chain#source-alpha filter-input-region#source-alpha filter-input-region#blur-source-alpha filter-source-paint-edges#source-alpha]

## Filter paint-server inputs: filter-paint-inputs, filter-paint-readers (exists)

- [x] filter-effects-1.md#attr-valuedef-in-fillpaint: FillPaint is the target element's fill property value as an infinite-extent image; a paint server in it uses the filtered element's own reference box (objectBoundingBox or the nearest viewport). [filter-paint-inputs#fill-paint filter-paint-inputs#gradient-box filter-paint-inputs#chain filter-paint-inputs#arithmetic filter-empty-paint#solid filter-empty-paint#pattern filter-empty-paint#gradient filter-empty-paint#bbox-empty filter-empty-paint#source-empty filter-empty-paint#transparent-child filter-empty-paint#opacity]
- [x] filter-effects-1.md#attr-valuedef-in-strokepaint: StrokePaint is the same, taken from the stroke property. [filter-paint-inputs#stroke-paint filter-empty-paint#stroke]

## Background input and isolation backdrop: filter-background-image, filter-background (exists), background-unscoped (exists)

- [x] filter-effects-1.md#attr-valuedef-in-backgroundimage: BackgroundImage is the backdrop defined by the current isolation group behind the filter region, at the time the filter was invoked. [filter-background-image#root filter-background-image#group filter-background-image#isolated filter-input-region#background-image filter-input-region#isolated-backdrop]
- [x] filter-effects-1.md#attr-valuedef-in-backgroundalpha: BackgroundAlpha is BackgroundImage with only its alpha channel kept. [filter-background-image#background-alpha filter-input-region#background-alpha]

## Deprecated enable-background: filter-background-image, enable-background (exists)

- [x] filter-effects-1.md#AccessBackgroundImage: this specification does not support the SVG 1.1 enable-background property; user agents must use the isolation property to define the BackgroundImage/BackgroundAlpha backdrop instead. (D26) [filter-background-image#enable-background]

## Filter primitive tree: filter-primitive-inputs, filter-need-join (exists)

- [x] filter-effects-1.md#FilterPrimitiveTree: every filter structure of primitives chained by in/result forms a filter primitive tree, rooted at its most subsequent primitive. [filter-primitive-inputs#last-primitive]
- [x] filter-effects-1.md#primary-filter-primitive-tree: the primary filter primitive tree is the one rooted at the filter element's last primitive child; only it contributes to the filter's output. [filter-primitive-inputs#last-primitive]
- [x] filter-effects-1.md#FilterPrimitiveTree: any other filter primitive tree in the same filter element may be ignored by the implementation. [filter-primitive-inputs#last-primitive]
- [x] filter-effects-1.md#FilterPrimitiveTree: a filter element with no filter primitive tree (no primitive children) makes the element it is applied to not render. [filter-primitive-inputs#empty-filter]

## Filter primitive subregion defaults and clipping: filter-primitive-inputs, filter-region, fe-flood, filter-subregions-outside (exists)

- [x] filter-effects-1.md#filter-primitive-subregion: x, y, width, height of a primitive default to the tightest union of the subregions of its referenced primitives. [filter-primitive-inputs#subregion]
- [x] filter-effects-1.md#filter-primitive-subregion: with no referenced node, with a referenced node that is a standard keyword input, or for feTile, the default subregion is 0%, 0%, 100%, 100% of the filter region, i.e. the filter region itself. [filter-region#default-region]
- [x] filter-effects-1.md#filter-primitive-subregion: a negative or zero subregion width or height disables that primitive. [filter-primitive-inputs#zero-subregion]
- [x] filter-effects-1.md#filter-primitive-subregion: the filter region hard-clips a primitive's input images; the primitive subregion hard-clips its result. [filter-region#cut fe-flood#subregion]
- [x] filter-effects-1.md#filter-primitive-subregion: intermediate offscreens need not exceed the intersection of the primitive subregion and the filter region, but must be big enough for any pixel partly inside either. [filter-primitive-inputs#subregion-sample]

## Filter primitive subregion as an exact tile boundary: filter-edge-cells, filter-support-abut (exists)

- [x] filter-effects-1.md#filter-primitive-subregion: because the subregion hard-clips a primitive's result, two primitives given abutting subregions must tile without a gap or an overlap. [filter-edge-cells#abut]

## feBlend: fe-blend-formulas, feblend-no-composite (exists)

- [x] filter-effects-1.md#element-attrdef-feblend-mode: mode is a blend-mode keyword, blending in (source Cs) over in2 (backdrop Cb); initial normal. [fe-blend-formulas#normal fe-blend-formulas#multiply fe-blend-formulas#screen fe-blend-formulas#overlay fe-blend-formulas#color-dodge fe-blend-formulas#soft-light fe-blend-formulas#difference fe-blend-formulas#hue fe-blend-formulas#color fe-blend-formulas#luminosity]
- [x] filter-effects-1.md#element-attrdef-feblend-no-composite: no-composite applies the blend mode's mixing formula without alpha compositing; otherwise the blend mode is combined with the Source Over operator. [fe-blend-formulas#no-composite]
- [x] filter-effects-1.md#element-attrdef-feblend-in2: in2 is the second (backdrop) input image. [fe-blend-formulas#normal]
- [x] filter-effects-1.md#element-attrdef-feblend-mode: normal with compositing equals feComposite operator="over", feMerge's own blending, and SVG's simple alpha compositing. [fe-blend-formulas#normal blend-porter-duff#over filter-subpixel-shadow#diagram-linear filter-subpixel-shadow#diagram-zero-shadow filter-coloured-shadow#linear-default]

## feColorMatrix: fe-color-matrix-formulas, filter-color (exists)

- [x] filter-effects-1.md#elementdef-fecolormatrix: feColorMatrix applies a 5x4 matrix to non-premultiplied RGBA of every pixel. [fe-color-matrix-formulas#matrix filter-primitive-values#unpremultiplied filter-subpixel-shadow#diagram-linear filter-subpixel-shadow#diagram-zero-shadow filter-coloured-shadow#translucent filter-coloured-shadow#black-shadow filter-coloured-shadow#zero-shadow]
- [x] filter-effects-1.md#FilterPrimitivesOverviewIntro: each primitive clamps its RGBA result before the next primitive; a SourceAlpha matrix can amplify sub-byte coverage and saturates at one. [fe-color-matrix-formulas#hard-alpha-zero fe-color-matrix-formulas#hard-alpha-quarter fe-color-matrix-formulas#hard-alpha-half fe-color-matrix-formulas#hard-alpha-one fe-color-matrix-formulas#hard-alpha-over fe-color-matrix-formulas#hard-alpha-before-next fe-color-matrix-formulas#hard-alpha-negative fe-color-matrix-formulas#hard-alpha-source-rgb filter-subpixel-shadow#hard-alpha-row filter-subpixel-shadow#hard-alpha-subbyte filter-subpixel-shadow#hard-alpha-out]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-type: type is matrix, saturate, hueRotate or luminanceToAlpha; initial matrix. [fe-color-matrix-formulas#matrix fe-color-matrix-formulas#saturate fe-color-matrix-formulas#hue-rotate fe-color-matrix-formulas#luminance-to-alpha]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: for type="matrix", values is 20 numbers, the identity matrix by default; for "hueRotate" and "saturate" it is a single number. [fe-color-matrix-formulas#matrix fe-color-matrix-formulas#defaults]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: saturate's matrix is built from s; hueRotate's from cos/sin of the hueRotate angle, both using fixed 0.213/0.715/0.072-family luminance coefficients. [fe-color-matrix-formulas#saturate fe-color-matrix-formulas#hue-rotate]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: for type="luminanceToAlpha" values does not apply; a fixed matrix (0.2126, 0.7152, 0.0722) maps RGB into the alpha channel. [fe-color-matrix-formulas#luminance-to-alpha]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: the initial values default differs by type: identity matrix for "matrix", 1 for "saturate", 0 (identity) for "hueRotate". [fe-color-matrix-formulas#defaults]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: if the number of entries in values does not match what type requires, feColorMatrix acts as a pass-through filter (its output equals its input). [fe-color-matrix-formulas#value-counts fe-color-matrix-formulas#value-counts-typed]

## feColorMatrix value-count edge cases: fe-color-matrix-formulas, filter-color-matrix-values (exists)

- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: saturate with no values attribute uses its initial value of 1 (no change). [fe-color-matrix-formulas#defaults]
- [x] filter-effects-1.md#element-attrdef-fecolormatrix-values: hueRotate takes exactly one number of degrees; a values list of a different length is a pass-through filter per the type mismatch rule. [fe-color-matrix-formulas#value-counts-typed]

## feComponentTransfer and feFuncR/G/B/A: fe-component-transfer-formulas, filter-color (exists)

- [x] filter-effects-1.md#elementdef-fecomponenttransfer: feComponentTransfer remaps each of R, G, B, A independently through its own feFuncR/feFuncG/feFuncB/feFuncA function, on non-premultiplied values. [fe-component-transfer-formulas#table fe-component-transfer-formulas#linear]
- [x] filter-effects-1.md#elementdef-fecomponenttransfer: if a kind of transfer function element occurs more than once the last occurrence is used; if one is absent it is treated as type="identity". [fe-component-transfer-formulas#last-function]
- [x] filter-effects-1.md#element-attrdef-fecomponenttransfer-type: type is identity, table, discrete, linear or gamma; initial identity (C'=C). [fe-component-transfer-formulas#table fe-component-transfer-formulas#discrete fe-component-transfer-formulas#linear fe-component-transfer-formulas#gamma]
- [x] filter-effects-1.md#attr-valuedef-type-table: table interpolates tableValues v0..vn over n equal regions: for k/n <= C < (k+1)/n, C' = vk + (C - k/n)*n*(vk+1 - vk); C=1 gives C'=vn. [fe-component-transfer-formulas#table fe-component-transfer-formulas#table-ends]
- [x] filter-effects-1.md#attr-valuedef-type-discrete: discrete steps through tableValues v0..vn-1: for k/n <= C < (k+1)/n, C'=vk; C=1 gives C'=vn-1. [fe-component-transfer-formulas#discrete fe-component-transfer-formulas#table-ends]
- [x] filter-effects-1.md#attr-valuedef-type-linear: linear is C' = slope*C + intercept; slope initial 1, intercept initial 0. [fe-component-transfer-formulas#linear]
- [x] filter-effects-1.md#attr-valuedef-type-gamma: gamma is C' = amplitude*pow(C, exponent) + offset; amplitude initial 1, exponent initial 1, offset initial 0. [fe-component-transfer-formulas#gamma]
- [x] filter-effects-1.md#element-attrdef-fecomponenttransfer-tablevalues: tableValues missing, or an empty list, makes the transfer function an identity. [fe-component-transfer-formulas#empty-table]

## feComposite: blend-porter-duff, fe-composite-arithmetic, filter-composite (exists)

- [x] filter-effects-1.md#elementdef-fecomposite: feComposite combines in (source) and in2 (destination) pixel-wise via a Porter-Duff operator: over, in, out, atop, xor, lighter, or arithmetic; initial operator over. [blend-porter-duff#over blend-porter-duff#in blend-porter-duff#out blend-porter-duff#atop blend-porter-duff#xor blend-porter-duff#lighter]
- [x] filter-effects-1.md#elementdef-fecomposite: arithmetic computes result = k1*i1*i2 + k2*i1 + k3*i2 + k4 per channel, clamped to [0,1]; k1-k4 each initial 0. [fe-composite-arithmetic#average fe-composite-arithmetic#product fe-composite-arithmetic#sum-clamped fe-composite-arithmetic#all-terms fe-composite-arithmetic#below-zero filter-input-region#inner-offset-quarter filter-input-region#inner-offset-half]
- [x] filter-effects-1.md#element-attrdef-fecomposite-in2: in2 is the second (destination) input image. [fe-composite-arithmetic#average blend-porter-duff#over]
- [x] filter-effects-1.md#elementdef-fecomposite: feComposite's result may extend beyond its input images' extent, subject to the filter primitive subregion rules. [fe-composite-arithmetic#k4-subregion]

## feComposite arithmetic chains: fe-composite-arithmetic, filter-arithmetic-readers (exists)

- [x] filter-effects-1.md#elementdef-fecomposite: an arithmetic feComposite's result, once named by result, is read like any other named result by later in/in2/feMergeNode references. [filter-edge-cells#arithmetic-reader]

## feConvolveMatrix: fe-convolve-formula, fe-convolve-order, fe-convolve-alpha-edges, filter-morphology-tile-convolve (exists)

- [x] filter-effects-1.md#feConvolveMatrixElementFormula: each result color is sum(source*kernelMatrix, kernel rotated 180 degrees) / divisor + bias*alpha, over the order x order neighborhood positioned by targetX/targetY. [fe-convolve-formula#kernel]
- [x] filter-effects-1.md#element-attrdef-order: order gives orderX, orderY as positive integers (orderY defaults to orderX, truncated toward zero if not integral); initial 3. [fe-convolve-formula#order-wrap fe-convolve-formula#target-divisor-bias fe-convolve-order#missing fe-convolve-order#fraction fe-convolve-order#fraction-pair fe-convolve-order#fraction-three]
- [x] svg2/types.md#syntax: an invalid order takes its initial value, 3 by 3, including malformed values and a component truncating below one. [fe-convolve-order#negative-x fe-convolve-order#negative-y fe-convolve-order#zero fe-convolve-order#zero-y fe-convolve-order#fraction-below-one fe-convolve-order#tiny-positive fe-convolve-order#empty fe-convolve-order#malformed fe-convolve-order#trailing-token fe-convolve-order#length fe-convolve-order#huge-negative fe-convolve-order#negative-pair]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-kernelmatrix: kernelMatrix must list exactly orderX*orderY numbers; a mismatched count makes feConvolveMatrix a pass-through filter. [fe-convolve-formula#wrong-count fe-convolve-alpha-edges#wrong-kernel-count fe-convolve-alpha-edges#wrong-count-preserved]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-divisor: divisor initial is the sum of kernelMatrix (or 1 if that sum is 0); a specified divisor of 0 falls back to this default too. [fe-convolve-formula#kernel fe-convolve-formula#order-wrap]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-bias: bias is added to every channel after the divisor; initial 0. [fe-convolve-formula#target-divisor-bias fe-convolve-formula#edge-none]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-targetx: targetX must satisfy 0 <= targetX < orderX; default floor(orderX/2); targetY likewise for orderY. [fe-convolve-formula#target-divisor-bias]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-preservealpha: preserveAlpha false convolves all channels including alpha; true unpremultiplies color first and leaves alpha untouched; initial false. [fe-convolve-formula#preserve-alpha fe-convolve-formula#bias-alpha fe-convolve-alpha-edges#center-opaque fe-convolve-alpha-edges#center-translucent fe-convolve-alpha-edges#asymmetric-y fe-convolve-alpha-edges#asymmetric-x fe-convolve-alpha-edges#transparent-taps fe-convolve-alpha-edges#source-alpha fe-convolve-alpha-edges#bias-with-alpha fe-convolve-alpha-edges#previous-result fe-convolve-alpha-edges#false-center fe-convolve-alpha-edges#false-shifted fe-convolve-alpha-edges#interior-subregion fe-convolve-alpha-edges#transparent-input fe-convolve-alpha-edges#zero-colour-kernel fe-convolve-alpha-edges#initial-false fe-convolve-alpha-edges#none-center fe-convolve-alpha-edges#none-negative-bias fe-convolve-alpha-edges#none-positive-bias fe-convolve-alpha-edges#mirror-center fe-convolve-alpha-edges#wrap-center fe-convolve-alpha-edges#wrap-expanded]

## feConvolveMatrix edgeMode: fe-convolve-formula, convolve-edge-modes (exists)

- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-edgemode: edgeMode is duplicate, wrap, mirror or none; initial duplicate. [fe-convolve-formula#kernel fe-convolve-formula#edge-none fe-convolve-formula#order-wrap fe-convolve-formula#edge-mirror]
- [x] filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-duplicate: duplicate extends the image by repeating its edge pixels. [fe-convolve-formula#kernel]
- [x] filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-duplicate: where the edge cuts a pixel, the colour duplicated is the inside's, the pixel as stored over its inside share, for a convolution and a blur (D48) [fe-duplicate-edges#convolve-1 fe-duplicate-edges#convolve-2 fe-duplicate-edges#blur-1 fe-duplicate-edges#blur-2]
- [x] filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-wrap: wrap extends the image by taking pixels from the opposite edge. [fe-convolve-formula#order-wrap]
- [x] filter-effects-1.md#attr-valuedef-feguassianblur-edgemode-mirror: mirror extends the image by reflecting pixels across the edge. [fe-convolve-formula#edge-mirror]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-edgemode: none extends the image with transparent black (R,G,B,A all 0). [fe-convolve-formula#edge-none]

## feConvolveMatrix bias and preserveAlpha readback: fe-convolve-formula, convolve-bias (exists)

- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-bias: a nonzero bias shifts every output channel by that amount, letting values that would otherwise clamp to 0 or 1 be represented. [fe-convolve-formula#bias-alpha fe-convolve-formula#edge-none]

## kernelUnitLength: filter-edge-cells, kernel-unit-length (exists)

- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength: kernelUnitLength is dx, dy (dy defaults to dx) in primitiveUnits, giving a scalable pixel grid for the kernel. [filter-edge-cells#kernel-unit-length]
- [x] filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength: a negative or zero kernelUnitLength falls back to the default of one device pixel. [filter-edge-cells#kernel-unit-length]
- [x] filter-effects-1.md#element-attrdef-fediffuselighting-kernelunitlength: feDiffuseLighting/feSpecularLighting kernelUnitLength sets dx, dy for the surface-normal Sobel deltas the same way; a negative or zero value also falls back to the default. [filter-edge-cells#lighting-unit-length]
- [x] filter-effects-1.md#SurfaceNormalCalculations: Sobel factors divide by the sample distance; an affine alpha surface retains its slope for positive kernelUnitLength values. Bilinear samples must retain small differences before this division. Independent Phong values cover tiny and unequal distances, the boundary at one device pixel, scaling and both lighting primitives. [lighting-kernel-length#diffuse-hundredth lighting-kernel-length#diffuse-below-double-ulp lighting-kernel-length#diffuse-unequal-x lighting-kernel-length#diffuse-unequal-y lighting-kernel-length#below-pixel-spacing lighting-kernel-length#above-pixel-spacing lighting-kernel-length#diffuse-half-scale lighting-kernel-length#diffuse-double-scale lighting-kernel-length#specular-tiny]
- [x] filter-effects-1.md#element-attrdef-fespecularlighting-kernelunitlength: tiny positive, unequal and large distances sample the bilinear alpha surface at the specified deltas before computing the normal. Rational nine-sample expectations also cover non-affine alpha and clamped taps. Nonpositive distances use the default; flat surfaces and linearRGB retain their lighting formulas. [lighting-kernel-length#piecewise-tiny lighting-kernel-length#piecewise-unequal lighting-kernel-length#piecewise-large lighting-kernel-length#piecewise-clamped lighting-kernel-length#piecewise-distant-samples lighting-kernel-length#specular-zero-invalid lighting-kernel-length#mixed-zero-invalid lighting-kernel-length#mixed-negative-invalid lighting-kernel-length#constant-alpha lighting-kernel-length#zero-surface lighting-kernel-length#diffuse-linearRGB lighting-kernel-length#specular-linearRGB]

## feDiffuseLighting, feSpecularLighting and light sources: fe-lighting-formulas, filter-lighting (exists)

- [x] filter-effects-1.md#elementdef-fediffuselighting: feDiffuseLighting lights the alpha channel as a bump map with the diffuse Phong term; result is opaque RGBA (alpha always 1.0). [fe-lighting-formulas#diffuse-distant fe-lighting-formulas#diffuse-point fe-lighting-formulas#diffuse-spot]
- [x] filter-effects-1.md#element-attrdef-fediffuselighting-surfacescale: surfaceScale is the surface height when A_in=1; initial 1; the surface is Z(x,y) = surfaceScale * I(x,y). [fe-lighting-formulas#diffuse-distant fe-lighting-formulas#specular-distant]
- [x] filter-effects-1.md#element-attrdef-fediffuselighting-diffuseconstant: diffuseConstant (kd) is non-negative; initial 1; D_r/g/b = kd * N.L * L_r/g/b. [fe-lighting-formulas#diffuse-distant]
- [x] filter-effects-1.md#SurfaceNormalCalculations: the surface normal N is computed from a 3x3 Sobel gradient of the alpha image, using a different kernel at corners, edges and the interior. [fe-lighting-formulas#diffuse-distant fe-lighting-formulas#specular-distant]
- [x] filter-effects-1.md#elementdef-fedistantlight: feDistantLight's light direction is constant, from azimuth (initial 0) and elevation (initial 0) in degrees. [fe-lighting-formulas#diffuse-distant fe-lighting-formulas#specular-distant]
- [x] filter-effects-1.md#operating-coordinate-space: a distant light's azimuth is fixed in the operating/output frame when the filtered source is reflected or turned; the surface normal is mapped from the filter's intermediate layer to that frame (D56). A point light instead follows its primitiveUnits position; surfaceScale still scales with those units. [fe-distant-light-transform#diffuse-reflect-x fe-distant-light-transform#diffuse-reflect-y fe-distant-light-transform#diffuse-quarter-turn fe-distant-light-transform#specular-reflect-x fe-distant-light-transform#specular-reflect-y fe-distant-light-transform#specular-quarter-turn fe-distant-light-transform#point-reflect-x fe-distant-light-transform#point-quarter-turn fe-distant-light-transform#scale-boundary]
- [x] filter-effects-1.md#element-attrdef-fepointlight-x: fePointLight's x, y, z (each initial 0) give a light position in the primitiveUnits coordinate system, positive Z toward the viewer. [fe-lighting-formulas#diffuse-point fe-lighting-formulas#specular-point]
- [x] filter-effects-1.md#element-attrdef-fespotlight-pointsatx: feSpotLight adds pointsAtX/Y/Z (each initial 0) and gives its light color a pow(-L.S, specularExponent) falloff toward that point; no light is emitted where L.S is positive. (D25) [fe-lighting-formulas#diffuse-spot fe-lighting-formulas#spot-focus]
- [x] filter-effects-1.md#element-attrdef-fespotlight-limitingconeangle: limitingConeAngle restricts the lit region to a cone of that angle in degrees around the spot axis; unspecified means no cone limit. [fe-lighting-formulas#spot-cone fe-lighting-formulas#spot-edges]
- [x] filter-effects-1.md#elementdef-fespecularlighting: feSpecularLighting uses the specular Blinn-Phong term instead: S_r/g/b = ks * pow(N.H, specularExponent) * L_r/g/b, and alpha = max(S_r, S_g, S_b), so it is not opaque. [fe-lighting-formulas#specular-distant fe-lighting-formulas#specular-point]
- [x] filter-effects-1.md#propdef-lighting-color: lighting-color gives the light's color; initial white; applies to feDiffuseLighting and feSpecularLighting; not inherited. [fe-lighting-formulas#diffuse-distant fe-lighting-formulas#specular-point]

## Lighting subregion and parameter edges: fe-lighting-formulas, filter-lighting-edges (exists)

- [x] filter-effects-1.md#element-attrdef-fespecularlighting-specularexponent: feSpecularLighting's specularExponent (shininess) is a separate attribute from feSpotLight's own specularExponent (focus), each initial 1. [fe-lighting-formulas#specular-distant fe-lighting-formulas#specular-point]
- [x] filter-effects-1.md#filter-primitive-subregion: the y/height (or any) override on a lighting primitive's own x/y/width/height still clips its result to that subregion like any primitive. [filter-edge-cells#lighting-subregion]

## feDisplacementMap: fe-displacement-formula, displacement-channels (exists)

- [x] filter-effects-1.md#elementdef-fedisplacementmap: P'(x,y) = P(x + scale*(XC(x,y)-.5), y + scale*(YC(x,y)-.5)), reading in2's channel selected by xChannelSelector/yChannelSelector as the displacement map. [fe-displacement-formula#channels filter-displacement-edges#neutral filter-displacement-edges#neutral-negative filter-displacement-edges#shift-left filter-displacement-edges#shift-right filter-displacement-edges#shift-up filter-displacement-edges#shift-down filter-displacement-edges#fractional-both filter-displacement-edges#negative-scale filter-displacement-edges#vary-x filter-displacement-edges#vary-y filter-displacement-edges#vary-both]
- [x] filter-effects-1.md#elementdef-fedisplacementmap: in stays premultiplied; the in2 displacement values are read as non-premultiplied color. [fe-displacement-formula#unpremultiplied filter-displacement-edges#neutral-translucent filter-displacement-space#source-default filter-displacement-space#group-opacity]
- [x] filter-effects-1.md#elementdef-fedisplacementmap: color-interpolation-filters applies only to the in2 source, not to in, which keeps its own color space. [fe-displacement-formula#linear-map fe-displacement-formula#srgb-map filter-displacement-space#source-default filter-displacement-space#source-srgb-map filter-displacement-space#source-linear-map filter-displacement-space#prior-srgb filter-displacement-space#prior-linear filter-displacement-space#prior-linear-map-linear filter-displacement-space#source-alpha filter-displacement-space#following-linear-reader filter-displacement-space#following-linear-blend filter-displacement-space#prior-linear-following-blend filter-displacement-space#source-as-srgb-map filter-displacement-space#source-as-linear-map]
- [x] filter-effects-1.md#element-attrdef-fedisplacementmap-scale: scale is the displacement scale factor in the primitiveUnits coordinate system; initial 0, which has no effect. [fe-displacement-formula#channels fe-displacement-formula#zero-scale filter-displacement-space#zero-scale filter-displacement-space#negative-scale]
- [x] filter-effects-1.md#element-attrdef-fedisplacementmap-xchannelselector: xChannelSelector/yChannelSelector pick R, G, B or A from in2; each initial A. [fe-displacement-formula#channels fe-displacement-formula#alpha-default filter-displacement-space#default-alpha-selectors]

## feDropShadow: fe-dropshadow-equivalent, filter-dropshadow-image (exists)

- [x] filter-effects-1.md#elementdef-fedropshadow: feDropShadow equals: blur in's alpha by stdDeviation, offset by dx/dy, flood with flood-color/flood-opacity, composite that over the offset blur with operator="in". [fe-dropshadow-equivalent#primitives]
- [x] filter-effects-1.md#elementdef-fedropshadow: the composited flood-and-blur result is then merged under the original, unmodified in. [fe-dropshadow-equivalent#primitives]
- [x] filter-effects-1.md#element-attrdef-fedropshadow-dx: dx, dy each have initial value 2 and are forwarded to the internal feOffset. [fe-dropshadow-equivalent#defaults]
- [x] filter-effects-1.md#element-attrdef-fedropshadow-stddeviation: stdDeviation is a number-optional-number with initial value 2, forwarded to the internal feGaussianBlur. [fe-dropshadow-equivalent#defaults fe-dropshadow-equivalent#zero-deviation]

## feDropShadow stdDeviation edge values: fe-dropshadow-equivalent, filter-dropshadow-huge (exists)

- [x] filter-effects-1.md#element-attrdef-fedropshadow-stddeviation: a negative component of stdDeviation disables the shadow's blur the same way it disables feGaussianBlur: the shadow stays sharp. [fe-dropshadow-equivalent#negative-deviation]

## feFlood and flood-color/flood-opacity: fe-flood

- [x] filter-effects-1.md#elementdef-feflood: feFlood fills a rectangle the size of its own filter primitive subregion with flood-color and flood-opacity. [fe-flood#subregion fe-flood#opacity-times-alpha]
- [x] filter-effects-1.md#propdef-flood-color: flood-color is a color; initial black; applies to feFlood and feDropShadow; not inherited. [fe-flood#initial fe-flood#not-inherited]
- [x] filter-effects-1.md#propdef-flood-opacity: flood-opacity is an opacity value; initial 1; computed value is clamped to [0,1]; its alpha multiplies with any alpha already in flood-color. [fe-flood#opacity-clamp fe-flood#opacity-times-alpha]

## feGaussianBlur stdDeviation: fe-blur-values, filter-primitives-edge (exists)

- [x] filter-effects-1.md#elementdef-fegaussianblur: the Gaussian kernel is the separable product H(x)*I(y) of two 1-D Gaussians with standard deviations s (x) and t (y). [fe-blur-values#one-deviation filter-small-gaussian#narrow filter-small-gaussian#half filter-small-gaussian#map-scale filter-small-gaussian#unequal filter-small-gaussian#below-two filter-small-gaussian#tiny filter-small-gaussian#premultiplied-pair filter-small-gaussian#group-half]
- [x] filter-effects-1.md#feGaussianBlurElement: feGaussianBlur approximates a normalized separable Gaussian convolution, including subpixel alpha inputs. [filter-subpixel-shadow#quarter-row filter-subpixel-shadow#scaled filter-subpixel-shadow#whole-row filter-subpixel-shadow#diagram-linear filter-subpixel-shadow#diagram-translucent filter-subpixel-shadow#diagram-near-two filter-coloured-shadow#linear-default filter-coloured-shadow#near-two]
- [x] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: stdDeviation is one or two numbers: one value is used for both axes, two give x then y; initial 0. [fe-blur-values#one-deviation fe-blur-values#x-then-y filter-subpixel-shadow#unequal-axes filter-subpixel-shadow#diagram-zero-blur]
- [x] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: a negative value, or zero in both axes, disables the primitive, so its result is its input image unchanged. [fe-blur-values#pass-through filter-coloured-shadow#zero-blur]
- [x] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: if stdDeviation is 0 in exactly one axis, the blur applies only along the other axis. [fe-blur-values#zero-axis fe-blur-values#x-then-y filter-coloured-shadow#one-axis filter-small-gaussian#x-only filter-small-gaussian#y-only]

## feGaussianBlur edgeMode: filter-edge-cells, blur-edge-modes (exists)

- [x] filter-effects-1.md#element-attrdef-fegaussianblur-edgemode: feGaussianBlur's edgeMode is duplicate, wrap, mirror or none, defined as for feConvolveMatrix; initial none. [filter-edge-cells#blur-edge-solid filter-edge-cells#blur-edge-none filter-small-gaussian#constant-duplicate filter-small-gaussian#constant-none]

## feGaussianBlur on infinite-extent inputs: fe-blur-values, filter-blur-paths (exists)

- [x] filter-effects-1.md#elementdef-fegaussianblur: if the input has infinite extent and is a constant image (e.g. FillPaint of a solid color), feGaussianBlur has no effect. [fe-blur-values#fill-paint]

## feImage: fe-image-placement, feimage-refs (exists)

- [x] filter-effects-1.md#elementdef-feimage: an feImage href to a stand-alone image resource renders like the image element; a reference to an element renders like the use element; the current coordinate system follows primitiveUnits either way (D41). [fe-image-placement#image-meet fe-image-placement#element fe-image-placement#element-box fe-image-placement#symbol fe-image-placement#svg fe-image-placement#inherits clip-contained-raster-edges#feimage-local clip-contained-raster-edges#feimage-far clip-contained-raster-edges#feimage-negative clip-contained-raster-edges#feimage-reflected clip-contained-raster-edges#feimage-opacity clip-contained-raster-edges#feimage-inside]
- [x] filter-effects-1.md#elementdef-feimage: a use-like shape reference retains its containing viewport for percentages; only a symbol or svg reference establishes a viewport of the subregion's size. Bounding-box primitive units add their coordinate transform. [fe-image-placement#element-percent-viewport fe-image-placement#element-box-percent-viewport fe-image-placement#element-percent-clipped fe-image-placement#symbol-percent-viewport]
- [x] filter-effects-1.md#elementdef-feimage: an href that is empty (zero width or height), fails to resolve, does not exist, or cannot be displayed fills the primitive subregion with transparent black. [fe-image-placement#missing]
- [x] filter-effects-1.md#element-attrdef-feimage-preserveaspectratio: preserveAspectRatio has initial value xMidYMid meet and is processed as for the image element. [fe-image-placement#image-meet fe-image-placement#image-stretch]
- [x] filter-effects-1.md#element-attrdef-feimage-xlinkhref: when both xlink:href and href are given, href overrides xlink:href. [fe-image-placement#href-wins]

## feMerge/feMergeNode: fe-offset-tile-merge, filter-arithmetic-merge-reader (exists)

- [x] filter-effects-1.md#elementdef-femerge: feMerge composites its feMergeNode children with the over operator, the first child on the bottom and the last child on top. [fe-offset-tile-merge#merge]
- [x] filter-effects-1.md#elementdef-femerge: an implementation may evaluate feMerge one layer at a time bottom to top instead of building one combined RGBA buffer, since merging is associative. [fe-offset-tile-merge#merge]

## feMorphology: fe-morphology-values, filter-morphology-edge (exists)

- [x] filter-effects-1.md#elementdef-femorphology: the dilate/erode kernel is a rectangle 2*x-radius by 2*y-radius; dilate takes the per-channel componentwise maximum over it, erode the minimum. [fe-morphology-values#dilate fe-morphology-values#erode fe-morphology-values#two-radii]
- [x] filter-effects-1.md#element-attrdef-femorphology-operator: operator is erode or dilate; initial erode. [fe-morphology-values#default-erode]
- [x] filter-effects-1.md#element-attrdef-femorphology-radius: radius is one or two numbers (x-radius, y-radius); initial 0; a negative or zero radius disables the primitive (result equals input). [fe-morphology-values#two-radii fe-morphology-values#zero-negative]
- [x] filter-effects-1.md#elementdef-femorphology: because feMorphology works on premultiplied values, its result color channels are always <= its result alpha. [fe-morphology-values#channel-max]
- [x] filter-effects-1.md#element-attrdef-femorphology-radius: any nonpositive radius component returns the filter input, including a zero in just one axis; absent or invalid radius takes the initial zero. Existing hard alpha edges retain their coverage. [filter-copy-edges#morph-missing filter-copy-edges#morph-zero filter-copy-edges#morph-zero-pair filter-copy-edges#morph-negative filter-copy-edges#morph-negative-x filter-copy-edges#morph-negative-y filter-copy-edges#morph-malformed filter-copy-edges#morph-empty filter-copy-edges#morph-zero-x filter-copy-edges#morph-zero-y filter-copy-edges#morph-source-alpha filter-copy-edges#morph-translucent filter-copy-edges#morph-same-source-edge filter-copy-edges#morph-previous-result filter-copy-edges#morph-subregion filter-copy-edges#morph-empty-input filter-copy-edges#morph-filter-chain]
- [x] filter-effects-1.md#elementdef-femorphology: rectangular componentwise extrema move an input hard edge out for dilation and in for erosion by the sampled neighbourhood radius; an unchanged sampled axis keeps its edge, including where a subregion coincides with it (D2). [filter-copy-edges#erode-rounded-copy filter-copy-edges#erode-x-only filter-copy-edges#erode-y-only filter-copy-edges#erode-both filter-copy-edges#dilate-rounded-copy filter-copy-edges#dilate-x-only filter-copy-edges#dilate-y-only filter-copy-edges#dilate-both filter-copy-edges#erode-removes-source filter-copy-edges#invalid-operator-erodes filter-copy-edges#erode-subregion-on-edge]
- [x] filter-effects-1.md#elementdef-femorphology: an infinite constant paint is unchanged; a nonconstant infinite paint still takes componentwise minima or maxima of its samples, without acquiring the filter region as an input edge. [filter-copy-edges#morph-constant-fill-paint filter-copy-edges#morph-constant-stroke-paint filter-copy-edges#morph-absent-stroke-paint filter-copy-edges#morph-gradient-paint]
- [x] filter-effects-1.md#elementdef-femorphology: sampled SourceAlpha extrema feed fractional offset and Gaussian blur before SourceAlpha-out and a shadow-alpha matrix; device scaling affects both kernel axes, and group opacity applies to the completed filter result [filter-morphology-shadow#rounded-copy filter-morphology-shadow#below-half filter-morphology-shadow#above-half filter-morphology-shadow#erode filter-morphology-shadow#unequal filter-morphology-shadow#large-blur filter-morphology-shadow#scaled-axes filter-morphology-shadow#group-opacity]

## feOffset: fe-offset-tile-merge, filter-offset-vertical (exists)

- [x] filter-effects-1.md#elementdef-feoffset: feOffset shifts its input by dx, dy in the primitiveUnits coordinate system. [fe-offset-tile-merge#offset fe-offset-tile-merge#offset-units filter-subpixel-shadow#negative-offset filter-subpixel-shadow#zero-shadow filter-subpixel-shadow#diagram-linear filter-coloured-shadow#negative-offset]
- [x] filter-effects-1.md#element-attrdef-feoffset-dx: dx, dy each have initial value 0. [fe-offset-tile-merge#offset-zero]

## feTile: fe-offset-tile-merge, filter-tile-clipped (exists)

- [x] filter-effects-1.md#elementdef-fetile: feTile fills its own filter primitive subregion by replicating its input's filter primitive subregion as tiles at (x + i*width, y + j*height) for integer i, j. [fe-offset-tile-merge#tile filter-tile-period#integer filter-tile-period#fractional-period filter-tile-period#fractional-origin filter-tile-period#negative-origin filter-tile-period#scaled filter-tile-period#unequal-axes filter-tile-period#reflected filter-tile-period#quarter-turn filter-tile-period#rotated filter-tile-period#sheared filter-tile-period#offset-chain]
- [x] filter-effects-1.md#feTileElement: clipping the input to the filter region does not shorten its declared reference subregion or change the repetition origin and period. [filter-tile-clipped#x-clipped-reference filter-tile-clipped#y-clipped-reference filter-tile-period#clipped-input]
- [x] filter-effects-1.md#feTileElement: constructing the repeated image must avoid seams from the fractional coverage of adjacent tiles; an opaque constant reference stays opaque, including when its size is below a pixel. [filter-tile-period#subpixel-solid filter-tile-period#subpixel-translucent filter-tile-period#subpixel-colour filter-subregions-outside#constant-subpixel-tiles]
- [x] filter-effects-1.md#FilterPrimitiveSubRegion: a reference with zero width has no pixels to repeat. [filter-tile-period#zero-width]

## feTurbulence: fe-turbulence-reference, filter-turbulence-displacement (exists)

- [x] filter-effects-1.md#elementdef-feturbulence: feTurbulence synthesizes a Perlin-noise image filling its whole filter primitive subregion. (D25) [fe-turbulence-reference#turbulence]
- [x] filter-effects-1.md#element-attrdef-feturbulence-basefrequency: baseFrequency is one or two numbers (x then y, or both); initial 0; negative values are unsupported. [fe-turbulence-reference#two-frequencies fe-turbulence-reference#zero-frequency]
- [x] filter-effects-1.md#element-attrdef-feturbulence-numoctaves: numOctaves is an integer; initial 1; negative values are unsupported. [fe-turbulence-reference#no-octaves fe-turbulence-reference#fractional-octaves]
- [x] filter-effects-1.md#element-attrdef-feturbulence-seed: seed seeds the pseudo-random generator; initial 0; it is truncated toward zero before use. [fe-turbulence-reference#seed]
- [x] filter-effects-1.md#element-attrdef-feturbulence-type: type is fractalNoise or turbulence; initial turbulence. [fe-turbulence-reference#turbulence fe-turbulence-reference#fractal-noise]
- [x] filter-effects-1.md#elementdef-feturbulence: the generated color and alpha values are produced in the color space set by color-interpolation-filters. [fe-turbulence-reference#linear-channels]

## feTurbulence stitchTiles: fe-turbulence-reference, filter-turbulence-stitch-far (exists)

- [x] filter-effects-1.md#element-attrdef-feturbulence-stitchtiles: stitchTiles is stitch or noStitch; initial noStitch; noStitch may show visible discontinuities at tile borders. [fe-turbulence-reference#stitch]
- [x] filter-effects-1.md#element-attrdef-feturbulence-stitchtiles: stitch adjusts baseFrequency per axis to lowFreq=floor(width*freq)/width or hiFreq=ceil(width*freq)/width, whichever is closer in ratio to freq. [fe-turbulence-reference#stitch]
- [x] filter-effects-1.md#element-attrdef-feturbulence-stitchtiles: this adjustment makes the subregion's width and height hold an integral number of Perlin tiles for the first octave, so tiled edges line up. [fe-turbulence-reference#stitch]

## feTurbulence numeric edge cases: fe-turbulence-reference, filter-numbers-invalid (exists)

- [x] filter-effects-1.md#element-attrdef-feturbulence-numoctaves: numOctaves must be an integer value for the noise function; a fractional or malformed value is handled as an invalid attribute, not a special-cased truncation like seed. [fe-turbulence-reference#fractional-octaves]
- [x] filter-effects-1.md#element-attrdef-feturbulence-seed: only seed is explicitly truncated toward zero by the spec; baseFrequency and numOctaves have no such truncation rule of their own. [fe-turbulence-reference#seed]

## color-interpolation-filters: filter-interpolation-space, filter-edge-blend-turbulence (exists)

- [x] filter-effects-1.md#propdef-color-interpolation-filters: color-interpolation-filters is auto, sRGB or linearRGB; initial linearRGB; inherited; applies to all filter primitives. [filter-interpolation-space#linear-default filter-interpolation-space#inherited filter-interpolation-space#auto filter-subpixel-shadow#diagram-linear filter-subpixel-shadow#diagram-srgb filter-subpixel-shadow#diagram-translucent filter-coloured-shadow#linear-default filter-coloured-shadow#srgb]
- [x] filter-effects-1.md#valdef-color-interpolation-filters-auto: auto lets the user agent pick either sRGB or linearRGB. [filter-interpolation-space#auto]
- [x] filter-effects-1.md#propdef-color-interpolation-filters: color-interpolation-filters has no effect on feOffset, feImage, feTile or feFlood, which do no color-space-dependent math. [filter-interpolation-space#offset-exact filter-interpolation-space#tile-exact filter-interpolation-space#image-exact filter-interpolation-space#flood-exact filter-spatial-space#offset-default filter-spatial-space#offset-srgb filter-spatial-space#offset-linear filter-spatial-space#offset-prior-srgb filter-spatial-space#offset-prior-linear filter-spatial-space#offset-negative filter-spatial-space#offset-alpha filter-spatial-space#offset-opacity filter-spatial-space#offset-zero filter-spatial-space#offset-integer filter-spatial-space#offset-linear-reader filter-spatial-space#offset-linear-blend filter-spatial-space#offset-prior-linear-blend filter-spatial-space#offset-smaller-region filter-spatial-space#offset-fill-paint filter-spatial-space#offset-shared-srgb-reader filter-spatial-space#tile-default filter-spatial-space#tile-srgb filter-spatial-space#tile-linear filter-spatial-space#tile-prior-srgb filter-spatial-space#tile-prior-linear filter-spatial-space#tile-alpha filter-spatial-space#tile-opacity filter-spatial-space#tile-integer filter-spatial-space#tile-integer-x filter-spatial-space#tile-integer-y filter-spatial-space#tile-linear-reader filter-spatial-space#tile-linear-blend filter-spatial-space#tile-prior-linear-blend filter-spatial-space#tile-prior-linear-default filter-spatial-space#tile-prior-srgb-default filter-spatial-space#tile-shared-srgb-reader filter-source-space#image-default-offset filter-source-space#image-srgb-offset filter-source-space#image-linear-offset filter-source-space#image-default-offset-srgb filter-source-space#image-srgb-offset-linear filter-source-space#image-linear-offset-srgb filter-source-space#image-local-offset filter-source-space#image-default-displace filter-source-space#image-srgb-displace filter-source-space#image-linear-displace filter-source-space#image-direct-default filter-source-space#image-direct-linear filter-source-space#flood-default filter-source-space#flood-srgb filter-source-space#flood-linear filter-source-space#flood-offset-linear]

## Filter functions' equivalent primitives: filter-function-equivalents, css-filter-functions (exists)

- [x] filter-effects-1.md#grayscaleEquivalent: grayscale(amount) is equivalent to an feColorMatrix type="matrix" with coefficients built from (1 - amount). [filter-function-equivalents#grayscale]
- [x] filter-effects-1.md#sepiaEquivalent: sepia(amount) is equivalent to an feColorMatrix type="matrix" with the sepia coefficients built from (1 - amount). [filter-function-equivalents#sepia]
- [x] filter-effects-1.md#saturateEquivalent: saturate(amount) is equivalent to feColorMatrix type="saturate" values="amount". [filter-function-equivalents#saturate]
- [x] filter-effects-1.md#huerotateEquivalent: hue-rotate(angle) is equivalent to feColorMatrix type="hueRotate" values="angle". [filter-function-equivalents#hue-rotate]
- [x] filter-effects-1.md#invertEquivalent: invert(amount) is equivalent to feComponentTransfer feFuncR/G/B type="table" tableValues="amount (1-amount)". [filter-function-equivalents#invert]
- [x] filter-effects-1.md#opacityEquivalent: opacity(amount) is equivalent to feComponentTransfer feFuncA type="table" tableValues="0 amount". [filter-function-equivalents#opacity]
- [x] filter-effects-1.md#brightnessEquivalent: brightness(amount) is equivalent to feComponentTransfer feFuncR/G/B type="linear" slope="amount". [filter-function-equivalents#brightness]
- [x] filter-effects-1.md#contrastEquivalent: contrast(amount) is equivalent to feComponentTransfer feFuncR/G/B type="linear" slope="amount" intercept="-(0.5*amount)+0.5". [filter-function-equivalents#contrast]
- [x] filter-effects-1.md#blurEquivalent: blur(radius) is equivalent to feGaussianBlur stdDeviation="radius radius", with edgeMode none for the filter property and duplicate for the filter() image function. [filter-function-equivalents#blur]
- [x] filter-effects-1.md#dropshadowEquivalent: drop-shadow() is equivalent to the same feGaussianBlur/feOffset/feFlood/feComposite/feMerge chain as feDropShadow. [filter-function-equivalents#drop-shadow]

## Filter functions grammar and defaults: filter-function-grammar, css-filter-functions (exists)

- [x] filter-effects-1.md#funcdef-filter-blur: blur() takes an optional length, no percentage; negative is not allowed; default when omitted is 0px. [filter-function-grammar#defaults filter-function-grammar#invalid]
- [x] filter-effects-1.md#funcdef-filter-blur: the ink overflow of a Gaussian blur is the axis-aligned bound of the area the implementation actually draws the blur into. [filter-function-equivalents#blur]
- [x] filter-effects-1.md#funcdef-filter-brightness: brightness() takes an optional number or percentage; 0% is black, 100% unchanged, over 100% allowed brighter; negative not allowed; default 1. [filter-function-grammar#defaults filter-function-equivalents#brightness]
- [x] filter-effects-1.md#funcdef-filter-contrast: contrast() takes an optional number or percentage; 0% is flat gray, 100% unchanged, over 100% allowed; negative not allowed; default 1. [filter-function-grammar#defaults filter-function-equivalents#contrast]
- [x] filter-effects-1.md#funcdef-filter-saturate: saturate() takes an optional number or percentage; 0% fully desaturated, 100% unchanged, over 100% allowed; negative not allowed; default 1. [filter-function-grammar#defaults filter-function-equivalents#saturate]
- [x] filter-effects-1.md#funcdef-filter-grayscale: grayscale()/invert()/sepia() take an optional number or percentage; 100% is full effect, over 100% is clamped to 1, negative not allowed; default when omitted is 1 for each. [filter-function-grammar#defaults filter-function-grammar#clamped]
- [x] filter-effects-1.md#funcdef-filter-opacity: opacity() takes an optional number or percentage, multiplying the image's samples; 0% transparent, 100% unchanged, over 100% clamped to 1; negative not allowed; default 1. [filter-function-grammar#defaults filter-function-grammar#clamped filter-function-equivalents#opacity]
- [x] filter-effects-1.md#funcdef-filter-hue-rotate: hue-rotate() takes an optional angle (its unit may be omitted only if the value is zero); default 0deg; implementations must not normalize values past 360deg. [filter-function-grammar#defaults filter-function-grammar#invalid filter-function-grammar#hue-rotate-turn]

## drop-shadow() syntax: filter-drop-shadow-syntax, css-filter-syntax (exists)

- [x] filter-effects-1.md#funcdef-filter-drop-shadow: drop-shadow() is [ <color>? && <length>{2,3} ]; a missing length is 0, a missing color is the color property's value. [filter-drop-shadow-syntax#no-blur filter-drop-shadow-syntax#colour-first filter-drop-shadow-syntax#colour-last filter-drop-shadow-syntax#one-length filter-drop-shadow-syntax#four-lengths filter-drop-shadow-syntax#two-shadows]
- [x] filter-effects-1.md#funcdef-filter-drop-shadow: the third length, if given, is the blur standard deviation, not a box-shadow blur radius; no 4th (spread) length and no comma-separated multiple shadows are accepted. [filter-drop-shadow-syntax#negative-offset filter-drop-shadow-syntax#colour-last]
- [x] filter-effects-1.md#funcdef-filter-drop-shadow: the ink overflow of drop-shadow() is the extent of its offset plus the extent of its blur, if any. [filter-function-equivalents#drop-shadow]

## Filter region for shorthands: filter-function-equivalents, filter-layers (exists)

- [x] filter-effects-1.md#filter-region-for-shorthands: every filter function's user-agent-defined filter region must cover the element's own visible content, including any overflow. [filter-function-equivalents#blur filter-function-equivalents#drop-shadow]
- [x] filter-effects-1.md#filter-region-for-shorthands: when a shorthand filter function expands the visible area, such as blur() or drop-shadow(), its filter region must be expanded to cover that too. [filter-function-equivalents#blur filter-function-equivalents#drop-shadow]

## Alpha boundaries of identity filter operations: filter-copy-edges (exists)

- [x] filter-effects-1.md#element-attrdef-fedisplacementmap-scale: the initial or explicit zero scale copies the first input regardless of its displacement map and preserves existing boundary coverage; a nonzero scale samples the displaced coordinates. [filter-copy-edges#displace-missing filter-copy-edges#displace-zero filter-copy-edges#displace-negative-zero filter-copy-edges#displace-malformed filter-copy-edges#displace-source-alpha filter-copy-edges#displace-subregion filter-copy-edges#displace-empty-input filter-copy-edges#displace-positive-control filter-displacement-edges#zero filter-displacement-edges#neutral-alpha filter-displacement-edges#neutral-prior-result filter-displacement-edges#neutral-fill-paint filter-displacement-edges#neutral-smaller-region filter-displacement-edges#vary-x-translucent filter-displacement-edges#vary-y-alpha filter-displacement-edges#vary-x-inner filter-displacement-edges#neutral-shared-input filter-displacement-edges#neutral-chain]
- [x] filter-effects-1.md#elementdef-fedropshadow: a fully transparent shadow leaves the filter input and its hard alpha edges unchanged, including in a filter-function chain; a positive shadow still composites with the input. [filter-copy-edges#shadow-zero-opacity filter-copy-edges#shadow-transparent-colour filter-copy-edges#shadow-source-alpha filter-copy-edges#shadow-translucent-input filter-copy-edges#shadow-subregion filter-copy-edges#shadow-empty-input filter-copy-edges#shadow-positive-control filter-copy-edges#shadow-css-function]

## Rasterized input boundaries: filter-source-paint-edges (exists)

- [x] filter-effects-1.md#element-attrdef-feoffset-dx: a fractional offset interpolates rasterized input samples, including partly covered boundary pixels; an integer axis preserves its continuous subregion bound so abutting regions remain disjoint. [filter-source-paint-edges#source-offset-cross-pixel]
- [x] filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: a pass-through blur (zero, negative or malformed deviation) preserves the input's existing hard alpha edge and does not apply fractional region coverage again. [filter-source-paint-edges#zero-blur-keeps-edge filter-source-paint-edges#invalid-blur-keeps-edge filter-source-paint-edges#negative-blur-keeps-edge filter-source-paint-edges#prior-result-zero-blur filter-source-paint-edges#prior-result-invalid-blur filter-source-paint-edges#prior-result-negative-blur filter-source-paint-edges#fill-paint-zero-blur filter-source-paint-edges#constant-fill-paint-blur]

## Notes

- filter-effects-1.md#element-attrdef-fegaussianblur-stddeviation: the spec's own 3-box-blur approximation (~3% off a true Gaussian) is explicitly permitted; an exact per-pixel Gaussian would cost real speed for no visible gain.
- filter-effects-1.md#SurfaceNormalCalculations: the per-pixel 3x3 Sobel normal (feDiffuseLighting/feSpecularLighting) and per-pixel kernel (feConvolveMatrix) are costly done exactly; kernelUnitLength resampling is the spec's own escape hatch.
- filter-effects-1.md#feTurbulenceElement: the spec gives one exact PRNG/Perlin C algorithm plus a "10000th value" test vector, i.e. byte-exact reproduction; this is heavy per-pixel work with little room for a faster equivalent noise function.
- filter-effects-1.md#issue-4fd9d6f6: the text makes a kernelMatrix length mismatch a pass-through filter, yet the spec itself still marks this exact case an open issue (#237); the behavior is not settled.
- filter-effects-1.md#issue-1d845580: the spec flags its own feDisplacementMap formula as not matching any current implementation (#113), without saying which side should change.
- filter-effects-1.md#element-attrdef-feconvolvematrix-kernelunitlength: kernelUnitLength is marked deprecated and "will be removed," yet stays a normatively defined attribute with its own default and fallback rule.

## Gaussian identity axes: filter-blur-hard-edges (exists)

- [x] filter-effects-1.md#feGaussianBlurElement: an identity Gaussian kernel retains the input's existing fractional hard edges; an identity axis preserves its edge while the other axis convolves. Independent rectangular coverage and exponential samples check tiny kernels without a renderer reference. [filter-blur-hard-edges#zero filter-blur-hard-edges#tiny filter-blur-hard-edges#underflow filter-blur-hard-edges#sub-byte-kernel filter-blur-hard-edges#active-kernel filter-blur-hard-edges#zero-x filter-blur-hard-edges#zero-y filter-blur-hard-edges#tiny-x filter-blur-hard-edges#tiny-y filter-blur-hard-edges#duplicate-tiny filter-blur-hard-edges#duplicate-zero-x filter-blur-hard-edges#duplicate-zero-y filter-blur-hard-edges#duplicate-tiny-x filter-blur-hard-edges#duplicate-tiny-y filter-blur-hard-edges#source-alpha filter-blur-hard-edges#group-opacity filter-blur-hard-edges#scaled filter-blur-hard-edges#reflected filter-blur-hard-edges#quarter-turn filter-blur-hard-edges#object-box filter-blur-hard-edges#group-filter filter-blur-hard-edges#two-primitives filter-blur-hard-edges#prior-flood filter-blur-hard-edges#linear-rgb]
