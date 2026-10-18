# Compositing and Blending Module Level 1

## Order of graphical operations: blend-render-order

- [ ] compositing-1.md#compositingandblendingorder: the compositing model follows SVG's order of operations: first any filter, then clipping, then masking, then blending and compositing

## SVG groups and isolation: blend-isolation (exists)

- [ ] compositing-1.md#csscompositingrules_SVG: every SVG element creates a non-isolated group by default
- [ ] compositing-1.md#csscompositingrules_SVG: opacity, filters, 3D transforms, blending and masking each force an element's group to become isolated; 2D transforms must not
- [ ] compositing-1.md#isolation: isolation = auto | isolate, initial auto; applies to SVG container elements, graphics elements and graphics referencing elements; not inherited
- [ ] compositing-1.md#isolation: isolation:auto means not isolated unless some other operation forces isolation; isolation:isolate always makes the element's group isolated
- [ ] compositing-1.md#isolation: in SVG, mask always creates an isolated group
- [ ] compositing-1.md#isolationblending: inside an isolated group, elements composite onto a transparent initial backdrop, so they don't blend with content behind the group; the group's own result then blends normally with what is behind it

## mix-blend-mode property: blend-mode-property (exists)

- [ ] compositing-1.md#mix-blend-mode: mix-blend-mode = <blend-mode>, initial normal; applies to SVG container elements, graphics elements and graphics referencing elements; not inherited
- [ ] compositing-1.md#mix-blend-mode: <blend-mode> = normal | darken | multiply | color-burn | lighten | screen | color-dodge | overlay | soft-light | hard-light | difference | exclusion | hue | saturation | color | luminosity
- [ ] compositing-1.md#mix-blend-mode: any blend mode other than normal establishes a new stacking context, which is then blended and composited with the stacking context containing the element
- [ ] compositing-1.md#csscompositingrules_CSS: an element with blending applied must blend against all the underlying content of the stacking context it belongs to

## Compositing groups and backdrop: blend-groups

- [ ] compositing-1.md#whatiscompositing: compositing first blends source and backdrop colors where they overlap, then composites the blended source with the backdrop using the chosen operator
- [ ] compositing-1.md#backdropCalc: the backdrop of an element is the result of compositing every element already composited before it
- [ ] compositing-1.md#groups: a compositing group is composited onto its initial backdrop first, then that single result is composited onto the group backdrop, contributing to the final result only once
- [ ] compositing-1.md#initialbackdrop: the initial backdrop used for a group's first element is the group backdrop itself for a non-isolated group, or fully transparent black for an isolated group
- [ ] compositing-1.md#groupbackdrop: the group backdrop is the result of compositing every element before the group's first element
- [ ] compositing-1.md#groupinvariance: with default attributes, adding or removing a group changes nothing: A+B+C = A+(B+C) = (A+B)+C; isolation, non-normal blending, or a non-source-over operator can break this invariance
- [ ] compositing-1.md#isolatedgroups: an isolated group's initial backdrop is black and fully transparent, distinct from the group backdrop it is composited into afterward
- [ ] compositing-1.md#pagebackdrop: the root element's isolated group (the root element group) receives the canvas background, then the root element's own filter, clip-path, mask and opacity, before compositing into the root group

## Simple alpha compositing formula: blend-alpha-formula

- [ ] compositing-1.md#simplealphacompositing: premultiplied compositing: co = Cs x as + Cb x ab x (1 - as); alpha of the result: ao = as + ab x (1 - as); all values in [0,1]
- [ ] compositing-1.md#simplealphacompositing: a premultiplied color is cs = Cs x as; simple alpha compositing in premultiplied form is co = cs + cb x (1 - as); the straight color is recovered as Co = co / ao
- [ ] compositing-1.md#generalformula: the general two-step formula is: blend in place, Cs' = (1 - ab) x Cs + ab x B(Cb, Cs); then composite, Co = as x Fa x Cs' + ab x Fb x Cb, with Fa and Fb set by the Porter-Duff operator in use
- [ ] compositing-1.md#blending: the blend mixing formula Cm = B(Cb, Cs) must be clamped to the color range and must not use premultiplied color values
- [ ] compositing-1.md#blending: the blend result is modulated by backdrop alpha: Cr = (1 - ab) x Cs + ab x B(Cb, Cs), so a transparent backdrop weights the result toward the unmixed source color

## Porter-Duff operators as used: filter-composite (exists)

- [ ] compositing-1.md#advancedcompositing: each Porter-Duff operator is defined by fractions Fa and Fb of source and backdrop alpha that contribute: co = as x Fa x Cs + ab x Fb x Cb, ao = as x Fa + ab x Fb
- [ ] compositing-1.md#porterduffcompositingoperators_srcover: source over: Fa=1, Fb=1-as; co = as x Cs + ab x Cb x (1-as); ao = as + ab x (1-as)
- [ ] compositing-1.md#porterduffcompositingoperators_srcin: source in: Fa=ab, Fb=0; co = as x Cs x ab; ao = as x ab
- [ ] compositing-1.md#porterduffcompositingoperators_srcout: source out: Fa=1-ab, Fb=0; co = as x Cs x (1-ab); ao = as x (1-ab)
- [ ] compositing-1.md#porterduffcompositingoperators_srcatop: source atop: Fa=ab, Fb=1-as; co = as x Cs x ab + ab x Cb x (1-as); ao = as x ab + ab x (1-as)
- [ ] compositing-1.md#porterduffcompositingoperators_xor: xor: Fa=1-ab, Fb=1-as; co = as x Cs x (1-ab) + ab x Cb x (1-as); ao = as x (1-ab) + ab x (1-as)
- [ ] compositing-1.md#porterduffcompositingoperators_plus: lighter (plus): Fa=1, Fb=1; co = as x Cs + ab x Cb; ao = as + ab, each unclamped before any later clamp to the color range

## Separable blend mode formulas: blend-modes-divided (exists)

- [ ] compositing-1.md#blendingseparable: a separable blend mode applies B(Cb,Cs) independently to each color component; source-over is used in every worked example
- [ ] compositing-1.md#blendingnormal: normal: B(Cb,Cs) = Cs
- [ ] compositing-1.md#blendingmultiply: multiply: B(Cb,Cs) = Cb x Cs
- [ ] compositing-1.md#blendingscreen: screen: B(Cb,Cs) = Cb + Cs - Cb x Cs
- [ ] compositing-1.md#blendingoverlay: overlay: B(Cb,Cs) = HardLight(Cs,Cb), i.e. hard-light with source and backdrop swapped
- [ ] compositing-1.md#blendingdarken: darken: B(Cb,Cs) = min(Cb,Cs)
- [ ] compositing-1.md#blendinglighten: lighten: B(Cb,Cs) = max(Cb,Cs), rounded down if it would exceed the range
- [ ] compositing-1.md#blendingcolordodge: color-dodge: 0 if Cb=0; 1 if Cs=1; else min(1, Cb/(1-Cs))
- [ ] compositing-1.md#blendingcolorburn: color-burn: 1 if Cb=1; 0 if Cs=0; else 1 - min(1, (1-Cb)/Cs)
- [ ] compositing-1.md#blendinghardlight: hard-light: Multiply(Cb,2xCs) if Cs<=0.5, else Screen(Cb,2xCs-1)
- [ ] compositing-1.md#blendingsoftlight: soft-light: if Cs<=0.5, Cb-(1-2Cs)xCbx(1-Cb); else Cb+(2Cs-1)x(D(Cb)-Cb), with D(Cb)=((16Cb-12)Cb+4)Cb if Cb<=0.25 else sqrt(Cb)
- [ ] compositing-1.md#blendingdifference: difference: B(Cb,Cs) = |Cb-Cs|
- [ ] compositing-1.md#blendingexclusion: exclusion: B(Cb,Cs) = Cb+Cs-2xCbxCs

## Non-separable blend mode formulas: blend-modes-divided (exists)

- [ ] compositing-1.md#blendingnonseparable: a non-separable mode converts backdrop and source into hue/saturation/luminosity, recombines components, then converts back, rather than acting per color channel
- [ ] compositing-1.md#blendingnonseparable: Lum(C) = 0.3xCred + 0.59xCgreen + 0.11xCblue; Sat(C) = max(components) - min(components)
- [ ] compositing-1.md#blendingnonseparable: SetLum(C,l) adds d = l - Lum(C) to every component then clips with ClipColor; ClipColor pulls out-of-[0,1] components back toward the luminosity L using the color's min/max components
- [ ] compositing-1.md#blendingnonseparable: SetSat(C,s) rescales the mid component between 0 and s by (Cmid-Cmin)/(Cmax-Cmin), sets Cmax to s, and sets Cmin to 0, or zeroes mid and max when Cmax equals Cmin
- [ ] compositing-1.md#blendinghue: hue: B(Cb,Cs) = SetLum(SetSat(Cs, Sat(Cb)), Lum(Cb))
- [ ] compositing-1.md#blendingsaturation: saturation: B(Cb,Cs) = SetLum(SetSat(Cb, Sat(Cs)), Lum(Cb))
- [ ] compositing-1.md#blendingcolor: color: B(Cb,Cs) = SetLum(Cs, Lum(Cb))
- [ ] compositing-1.md#blendingluminosity: luminosity: B(Cb,Cs) = SetLum(Cb, Lum(Cs))

## Notes

- compositing-1.md#groupcompositing: a bottommost element in an isolated group using a backdrop-dependent operator (destination, source-in, destination-in, destination-out, source-atop) composites to empty against the transparent initial backdrop; none of these operators is exposed per SVG element (mask-composite and feComposite already state their own bottom-layer rule), so this note has no element of its own to attach to in this renderer and is kept only as background for how isolated-group compositing behaves.
- compositing-1.md#porterduffcompositingoperators_plus: "lighter" is unclamped per the formula (co, ao can exceed 1) yet the module never states where clamping happens for this operator; feComposite's own result-clamping (filter-effects-1.md) is assumed to apply, but compositing-1.md itself leaves the gap.
- compositing-1.md#mix-blend-mode: this Level 1 <blend-mode> list has no plus-lighter keyword; that value (seen on mix-blend-mode in some renderers) belongs to a later module and is out of this page's scope.
