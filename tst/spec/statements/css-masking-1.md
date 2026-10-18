# CSS Masking Module Level 1

## Clipping region: clipping-region

- [x] css-masking-1.md#clipping-paths: the clipping region restricts where paint can land; content outside it is not drawn, for the element and its descendants
- [x] css-masking-1.md#clipping-paths: the rendered result is the cumulative intersection of an element's own clip with every ancestor's clip
- [x] css-masking-1.md#clipping-paths: a clipping path changes only rendering, never the element's geometry (bounding box, use of it, etc. stay as if unclipped)

## clip-path value and reference boxes: clip-geometry-boxes, clipping-region, stroke-bounding-box, clip-path-boxes (exists), use-geometry (exists), clip-root.20x20 (exists)

- [x] css-masking-1.md#the-clip-path: clip-path = <clip-source> | [<basic-shape> || <geometry-box>] | none; initial none, not inherited
- [x] css-masking-1.md#the-clip-path: applies to SVG container elements except defs, to graphics elements, and to use
- [x] css-masking-1.md#the-clip-path: a <basic-shape> given with no <geometry-box> is sized and positioned against the border-box reference box
- [x] css-masking-1.md#the-clip-path: a <geometry-box> given alone uses that box's edges, including any corner radius, as the clip path itself
- [x] css-masking-1.md#the-clip-path: fill-box is the object bounding box; stroke-box is the stroke bounding box
- [x] css-masking-1.md#the-clip-path: view-box is the nearest SVG viewport, or, if that viewport has a viewBox, the rect at the viewBox's origin sized to its width and height
- [x] css-masking-1.md#the-clip-path: for SVG elements without a CSS layout box, content-box and padding-box compute to fill-box, and border-box and margin-box compute to stroke-box
- [x] css-masking-1.md#the-clip-path: a computed value other than none creates a stacking context, as CSS opacity does for values other than 1
- [x] css-masking-1.md#the-clip-path: clip-path is a presentation attribute on SVG elements

## clip-path invalid and missing references: clip-path-element, mask-values, clip-mask-missing (exists)

- [x] css-masking-1.md#the-clip-path: a <clip-source> url that is invalid (target missing, or not a clipPath element) applies no clipping at all
- [x] css-masking-1.md#the-mask-image: a mask-image url reference that is empty, fails, is not a mask element, is non-existent, or can't be displayed still counts as a transparent black image layer (D20)

## clipPath element: clip-path-element, clip-path (exists)

- [x] css-masking-1.md#ClipPathElement: clipPathUnits = userSpaceOnUse | objectBoundingBox, default userSpaceOnUse
- [x] css-masking-1.md#ClipPathElement: userSpaceOnUse means the user coordinate system of the element referencing the clipPath, at reference time
- [x] css-masking-1.md#ClipPathElement: objectBoundingBox origins at the referencing element's bounding box top-left, sized to its width/height, with user units equal to CSS px
- [x] css-masking-1.md#ClipPathElement: CSS properties inherit into clipPath from its own ancestors, never from the element that references it
- [x] css-masking-1.md#ClipPathElement: clipPath is never rendered directly regardless of display, and stays referenceable even when display:none on it or an ancestor
- [x] css-masking-1.md#ClipPathElement: a use child of clipPath must directly reference a path, text, or basic-shape element; an indirect reference is an error that makes the whole clipPath ignored (D24)
- [x] css-masking-1.md#ClipPathElement: each child's raw geometry (ignoring fill, stroke, stroke-width, antialiasing aside) forms a 1-bit silhouette; a child hidden by display or visibility contributes nothing (D14)
- [x] css-masking-1.md#ClipPathElement: the silhouettes of all children are OR'd together: a point is inside the clip if it is inside any child
- [x] css-masking-1.md#ClipPathElement: clipPath's own children do not inherit the clip-path of the clipPath element's ancestors
- [x] css-masking-1.md#ClipPathElement: clip-path on the clipPath element itself intersects its whole content with the referenced path; clip-path on one child clips that child before it is OR'd with the rest
- [x] css-masking-1.md#ClipPathElement: an empty clipping path clips away the entire element that referenced it

## clip-rule: clip-path-element, clipping-region, clip-path (exists)

- [x] css-masking-1.md#the-clip-rule: clip-rule = nonzero | evenodd, initial nonzero, inherited, applies to SVG graphics elements, using fill-rule's algorithms
- [x] css-masking-1.md#the-clip-rule: clip-rule only applies to graphics elements inside a clipPath element, and only the clip-rule on the clipping shape itself (not on the referencing element) is used
- [x] css-masking-1.md#the-clip-rule: clip-rule does not apply to <basic-shape>s
- [x] css-masking-1.md#the-clip-rule: clip-rule is a presentation attribute on SVG elements

## mask-image and mask-mode: mask-values, mask-composite, mask-syntax (exists), mask-longhands (exists)

- [x] css-masking-1.md#the-mask-image: mask-image = <mask-reference>#, where <mask-reference> is none | <image> | <url>; initial none, not inherited
- [x] css-masking-1.md#the-mask-image: applies to SVG container elements except defs, to graphics elements, and to use
- [x] css-masking-1.md#the-mask-image: a value of none counts as a transparent black image layer (D23)
- [x] css-masking-1.md#the-mask-image: a <url> may reference a mask element or a CSS image
- [x] css-masking-1.md#the-mask-image: a computed value other than none creates a stacking context, as CSS opacity does for values other than 1
- [x] css-masking-1.md#the-mask-mode: mask-mode = <masking-mode>#, <masking-mode> = alpha | luminance | match-source; initial match-source
- [x] css-masking-1.md#the-mask-mode: match-source uses the referenced mask element's mask-type when the reference is a mask element, otherwise it uses the image's alpha values
- [x] css-masking-1.md#the-mask-mode: mask-mode must not affect the masking mode of mask-border-source

## Mask layer geometry: stroke-bounding-box, mask-longhands (exists)

- [x] css-masking-1.md#the-mask-repeat: mask-repeat = <repeat-style>#, initial repeat, applied after the layer image is sized and positioned, using background-repeat's keyword definitions
- [x] css-masking-1.md#the-mask-position: mask-position = <position>#, initial 0% 0%; percentages resolve against the mask painting area size minus the layer image size, as for background-position
- [x] css-masking-1.md#the-mask-size: mask-size = <bg-size>#, initial auto, using background-size's definitions
- [x] css-masking-1.md#the-mask-clip: mask-clip = [<coord-box> | no-clip]#, initial border-box; sets the mask painting area, restricting the painted content to it
- [x] css-masking-1.md#the-mask-clip: mask-clip has no effect on a layer that references a mask element; that element's x, y, width, height and maskUnits determine the mask painting area instead
- [x] css-masking-1.md#the-mask-clip: for SVG elements without a CSS layout box, content-box and padding-box compute to fill-box, and border-box computes to stroke-box
- [x] css-masking-1.md#the-mask-origin: mask-origin = <coord-box>#, initial border-box, giving the mask positioning area's reference box
- [x] css-masking-1.md#the-mask-origin: for SVG elements without a CSS layout box, content-box, padding-box and border-box all compute to fill-box

## mask shorthand: mask-composite, mask-layers (exists), mask-border (exists)

- [x] css-masking-1.md#the-mask: mask = <mask-layer>#, each layer built from mask-reference, position/size, repeat-style, geometry-box(es), compositing-operator and masking-mode in any combination
- [x] css-masking-1.md#the-mask: one geometry-box plus no-clip sets mask-origin to that box and mask-clip to no-clip; one geometry-box alone sets both mask-origin and mask-clip to it; two geometry-boxes set mask-origin then mask-clip in order
- [x] css-masking-1.md#the-mask: when a layer's mask-reference is a mask element, the used values of mask-repeat, mask-position, mask-clip, mask-origin and mask-size have no effect; the mask element's own attributes govern instead
- [x] css-masking-1.md#the-mask: the mask shorthand also resets mask-border to its initial value
- [x] css-masking-1.md#the-mask: mask is a presentation attribute on SVG elements

## mask-composite and layering: mask-composite, mask-layers (exists)

- [x] css-masking-1.md#the-mask-composite: mask-composite = <compositing-operator>#, <compositing-operator> = add | subtract | intersect | exclude; initial add
- [x] css-masking-1.md#the-mask-composite: add is source-over, subtract is source-out, intersect is source-in, exclude is XOR, of the current layer (source) over all layers below it already composited (destination)
- [x] css-masking-1.md#the-mask-composite: the compositing operator on the bottommost layer is ignored, as there is no further layer below it
- [x] css-masking-1.md#the-mask-composite: mask layers composite only among themselves, as if rendered into an isolated group; they never composite with the element's own content or what is behind it
- [x] css-masking-1.md#layering: the number of mask layers equals the number of comma-separated mask-image values; a none entry still forms a layer
- [x] css-masking-1.md#layering: mask-mode and mask-composite values are matched to layers positionally; a list too short for the layer count is repeated until it is long enough
- [x] css-masking-1.md#layering: every mask layer image is turned into an alpha mask, then the layers are combined by mask-composite's compositing operators

## Mask value calculation: mask-values, mask (exists)

- [x] css-masking-1.md#MaskValues: in alpha mode the mask value at a point is simply that point's alpha channel value
- [x] css-masking-1.md#MaskValues: in luminance mode, if color-interpolation on the mask element computes to linearRGB, the image is converted to linearRGB first, then luminance-to-alpha coefficients are applied to non-premultiplied RGB, then multiplied by alpha
- [x] css-masking-1.md#MaskValues: a three-channel RGB mask object is treated as RGBA with alpha uniformly 1
- [x] css-masking-1.md#MaskValues: a single-channel mask object is treated as RGBA with that channel used for all three color channels and alpha uniformly 1
- [x] css-masking-1.md#MaskValues: the effect of a mask equals multiplying the masked object's alpha channel by the mask's mask value
- [x] css-masking-1.md#MaskValues: regions not covered by a mask image are transparent black, a mask value of 0

## mask element geometry: mask-values, mask (exists)

- [x] css-masking-1.md#MaskElement: maskUnits = userSpaceOnUse | objectBoundingBox for x/y/width/height, default objectBoundingBox
- [x] css-masking-1.md#MaskElement: maskContentUnits = userSpaceOnUse | objectBoundingBox for the mask's content, default userSpaceOnUse
- [x] css-masking-1.md#MaskElement: if x is unspecified but one of y/width/height is, x defaults to -10%; same rule in turn for y
- [x] css-masking-1.md#MaskElement: if width is unspecified but one of x/y/height is, width defaults to 120%; same rule in turn for height
- [x] css-masking-1.md#MaskElement: a negative or zero width or height on the mask element disables rendering of the element that references it
- [x] css-masking-1.md#MaskElement: if any of x/y/width/height are given, that rectangle becomes a clipping path that the mask's rendered content is clipped by
- [x] css-masking-1.md#MaskElement: CSS properties inherit into mask from its own ancestors, never from the element referencing it
- [x] css-masking-1.md#MaskElement: mask elements are never rendered directly; opacity, filter and display do not apply to them, and they stay referenceable under display:none

## mask-type: mask-values, mask (exists)

- [x] css-masking-1.md#the-mask-type: mask-type = luminance | alpha, initial luminance, applies only to mask elements, not inherited
- [x] css-masking-1.md#the-mask-type: the referencing element's mask-mode overrides mask-type's preference whenever mask-mode computes to something other than match-source
- [x] css-masking-1.md#the-mask-type: mask-type is a presentation attribute on SVG elements

## mask-border: mask-values, mask-border (exists)

- [x] css-masking-1.md#the-mask-border-source: mask-border-source applies to SVG container elements except defs, graphics elements, and use; initial none
- [x] css-masking-1.md#the-mask-border-source: an empty, failed, non-existent or undisplayable image is ignored as a mask but still counts as the configured mask border image
- [x] css-masking-1.md#the-mask-border-source: mask-border-source and mask-image act independently; if both are non-none the element is masked by both operations, in either order, with the same result
- [x] css-masking-1.md#the-mask-border-mode: mask-border-mode = luminance | alpha, initial alpha; mask-mode and mask-type have no effect on the mask border image's interpretation
- [x] css-masking-1.md#the-mask-border-slice: mask-border-slice initial 0, percentages relative to the mask border image's size; the discarded middle region is treated as fully opaque white (unmasked) unless the fill keyword is given
- [x] css-masking-1.md#the-mask-border-width: mask-border-width initial auto, percentages relative to the mask border image area; for SVG elements without a layout box the border-width used in it is 0
- [x] css-masking-1.md#the-mask-border-outset: mask-border-outset initial 0; of four values, missing left copies right, missing bottom copies top, missing right copies top; negative values are not allowed; a bare number multiplies the (zero, for SVG) border-width
- [x] css-masking-1.md#the-mask-border-repeat: mask-border-repeat initial stretch; first keyword is horizontal sides, second is vertical, defaulting to the first if omitted
- [x] css-masking-1.md#the-mask-border: mask-border shorthand sets source, slice, width, outset, repeat and mode together, omitted parts reset to their initial values
- [x] css-masking-1.md#masking-with-the-mask-border-image: the sliced nine pieces are scaled, positioned and tiled into the mask border image area, and a non-none mask-border-source creates a stacking context as opacity does

## Deprecated clip property: clip-property (exists)

- [x] css-masking-1.md#clip-property: clip applies, in SVG, to elements establishing a new viewport, to pattern elements, and to mask elements; deprecated in favor of clip-path but UAs must still support it (D22)
- [x] css-masking-1.md#clip-property: clip = rect(<top>,<right>,<bottom>,<left>) | auto, initial auto; auto means the element does not clip
- [x] css-masking-1.md#clip-property: top/bottom offsets are from the border box's top edge, right/left offsets from its left edge; negative lengths are permitted
- [x] css-masking-1.md#clip-property: auto for top/left means 0; auto for bottom means the border box height; auto for right means the border box width; four autos clip to the whole border box
- [x] css-masking-1.md#clip-property: rect() arguments must be supported comma-separated; a UA may also accept them without commas, but not a mix of the two
- [x] css-masking-1.md#clip-property: clip is a presentation attribute on SVG elements

## Stroke bounding box: stroke-bounding-box

- [x] css-masking-1.md#compute-stroke-bounding-box: for a plain graphics element, the stroke bounding box starts from the object bounding box and returns it unchanged if used stroke-width <= 0 or used stroke is none
- [x] css-masking-1.md#compute-stroke-bounding-box: otherwise an inflation delta starts at half the stroke-width, and for shapes other than rect/ellipse/circle/image it is scaled: by stroke-miterlimit when linejoin is miter (but by sqrt(2) instead when that miterlimit is below sqrt(2) and linecap is square), or by sqrt(2) when linecap is square and linejoin is not miter (D21)
- [x] css-masking-1.md#compute-stroke-bounding-box: the object bounding box is inflated by that delta on every side to give the stroke bounding box
- [x] css-masking-1.md#compute-stroke-bounding-box: stroke-opacity, stroke-dasharray and stroke-dashoffset play no part in the stroke bounding box
- [x] css-masking-1.md#compute-stroke-bounding-box: for a container element or a use element, the stroke bounding box is the union of each child's stroke bounding box mapped into the parent's coordinate space
- [x] css-masking-1.md#compute-stroke-bounding-box: for an image element the stroke bounding box is just its object bounding box

## Notes

- css-masking-1.md#the-clip-path: "no clipping is applied" for an invalid clip-source url reads as if the whole clip-path value were none, but it is unclear whether a co-specified <geometry-box> in the same value (e.g. `clip-path: url(#missing) fill-box`, which the grammar does not even allow since <clip-source> is a bare alternative) would matter; moot since the grammar keeps them exclusive.
- css-masking-1.md#the-mask-clip, css-masking-1.md#the-mask-origin: margin-box and half-border-box are not valid <coord-box> values at all (see excluded.txt), yet mask-clip's grammar textually allows any <geometry-box>-shaped keyword through its shared wording with clip-path; browsers and this renderer reject them for mask-clip/mask-origin specifically.
