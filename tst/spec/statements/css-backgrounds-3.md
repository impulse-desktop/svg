# CSS Backgrounds and Borders Module Level 3, layer grammars borrowed by mask-*

## background-repeat grammar: mask-tiling, mask-layers (exists)

- [x] css-backgrounds-3.md#background-repeat: <repeat-style> = repeat-x | repeat-y | [repeat | space | round | no-repeat]{1,2}; a comma-separated list, one per layer; initial value repeat. [mask-tiling#repeat mask-tiling#two-keywords]
- [x] css-backgrounds-3.md#valdef-background-repeat-repeat-x: repeat-x computes to "repeat no-repeat"; repeat-y computes to "no-repeat repeat". [mask-tiling#repeat-x-y]
- [x] css-backgrounds-3.md#background-repeat: in a two-keyword value the first keyword applies to the horizontal axis, the second to the vertical. [mask-tiling#two-keywords]
- [x] css-backgrounds-3.md#valdef-background-repeat-repeat: repeat tiles the image as often as needed to cover the painting area in that direction. [mask-tiling#repeat]
- [x] css-backgrounds-3.md#valdef-background-repeat-space: space tiles as many whole copies as fit without clipping, then spaces them so the first and last touch the area's edges; with room for only one copy, position is used for it instead. (D28) [mask-tiling#space]
- [x] css-backgrounds-3.md#valdef-background-repeat-round: round tiles a whole number of copies, rescaling the image to fit exactly (per the background-size round formula). (D28) [mask-tiling#round]
- [x] css-backgrounds-3.md#valdef-background-repeat-no-repeat: no-repeat places the image once, not repeated in that direction. [mask-tiling#no-repeat]

## background-position grammar: mask-tiling, mask-layers (exists)

- [x] css-backgrounds-3.md#background-position: initial value 0% 0%; a percentage resolves against (positioning-area size - image size) on that axis. [mask-tiling#position-percent]
- [x] css-backgrounds-3.md#background-position: <bg-position> accepts 1, 2, 3 or 4 components; with only one, the second is assumed center. [mask-tiling#position-one]
- [x] css-backgrounds-3.md#background-position: with two components, a bare length-percentage is horizontal-then-vertical; a pair of keywords may be reordered, but a keyword-plus-offset pair may not. [mask-tiling#position-two]
- [x] css-backgrounds-3.md#background-position: with 4 components each length-percentage offset follows an edge keyword; mask-position takes CSS Values' <position>, which has no 3-component form, so one is invalid there. [mask-tiling#position-four mask-tiling#position-three]
- [x] css-backgrounds-3.md#background-position: a positive offset is inward from the named edge, negative is outward. [mask-tiling#position-four]
- [x] css-backgrounds-3.md#valdef-background-position-top: top/right/bottom/left compute to 0% or 100% on their axis when only one or two components are given. [mask-tiling#position-keywords mask-tiling#position-one]
- [x] css-backgrounds-3.md#valdef-background-position-center: center computes to 50% on whichever axis is not otherwise specified. [mask-tiling#position-keywords]

## background-size grammar: mask-tiling, mask-longhands (exists)

- [x] css-backgrounds-3.md#background-size: <bg-size> = [<length-percentage [0,inf]> | auto]{1,2} | cover | contain; comma-separated list; initial auto. [mask-tiling#size-one mask-tiling#size-contain mask-tiling#size-cover mask-tiling#size-negative]
- [x] css-backgrounds-3.md#valdef-background-size-contain: contain scales preserving the natural aspect ratio to the largest size fitting both dimensions inside the positioning area. [mask-tiling#size-contain]
- [x] css-backgrounds-3.md#valdef-background-size-cover: cover scales preserving the natural aspect ratio to the smallest size covering both dimensions of the positioning area. [mask-tiling#size-cover]
- [x] css-backgrounds-3.md#background-size: with one length-percentage/auto value given, it is the width and the height is assumed auto. [mask-tiling#size-one]
- [x] css-backgrounds-3.md#background-size: an auto dimension resolves via the natural aspect ratio and the other dimension, else the image's natural size, else 100%. [mask-tiling#size-auto mask-tiling#size-one]
- [x] css-backgrounds-3.md#background-size: with both values auto, the natural size is used if any; with no natural size at all, it is sized as for contain. [mask-tiling#size-natural mask-tiling#size-auto]
- [x] css-backgrounds-3.md#background-size: negative length-percentage values are invalid. [mask-tiling#size-negative]
- [x] css-backgrounds-3.md#background-size: an image whose resolved width or height is zero is not displayed, as if it were transparent. [mask-tiling#size-zero]
- [x] css-backgrounds-3.md#background-size: if background-repeat is round on a dimension, that dimension's size is rescaled to X' = W / round(W/X), the nearest count of whole copies fitting the positioning area width W. [mask-tiling#round]
- [x] css-backgrounds-3.md#background-size: if round applies to only one dimension and the other is auto, that other dimension is then scaled to restore the original aspect ratio. [mask-tiling#round-auto]

## background-clip and background-origin box keywords: mask-tiling, mask-longhands (exists)

- [x] css-backgrounds-3.md#background-clip: background-clip: border-box | padding-box | content-box (a comma list); initial border-box; clips painting to the named box. [mask-tiling#clip]
- [x] css-backgrounds-3.md#background-origin: background-origin: border-box | padding-box | content-box (a comma list); initial padding-box; sets the positioning area's origin to the named box. [mask-tiling#origin]

## Notes

## The root's background on the canvas: render-root-background-opacity, render-root-background-blend, render-root-background-shorthand, render-root-background-reset, render-root-background-layers, render-root-background-keyword, render-root-background-current, render-root-background-stripes, render-root-background-order, render-root-background-place, render-root-background-sides, render-root-background-layer, render-root-background-image, render-root-background-element, render-root-background-invalid, render-root-background-box, render-root-background-refused

- [x] css-backgrounds-3.md#special-backgrounds: the root element's background is the canvas's, painted under the root element: the root's opacity does not fade it, and the root's content blends with it as its backdrop. [render-root-background-opacity#canvas render-root-background-blend#canvas vector-effect-host-range#offscreen-pattern]
- [x] css-backgrounds-3.md#propdef-background: the background shorthand sets background-color from the colour of its final layer, else to its initial value, transparent. [render-root-background-shorthand#final-layer render-root-background-layers#final-layer render-root-background-reset#no-colour render-root-background-keyword#initial render-root-background-current#currentcolor]
- [x] css-backgrounds-3.md#root-background: the root's background images are sized and positioned in the root's box and painted over the entire canvas; the root's boxes are all its box (D39), so background-origin and background-clip change nothing there, and a drawing at another size lays them out in its pixels (D51, tst/canvas_scale.py). [render-root-background-stripes#tiles render-root-background-box#canvas]
- [x] css-backgrounds-3.md#layering: the first image of the list is the layer on top, each next one behind it, the background colour below them all; a list shorter than the images repeats. [render-root-background-order#first-on-top render-root-background-stripes#tiles]
- [x] css-backgrounds-3.md#the-background-image: <bg-image> = <image> | none, a comma list; none and an image that fails to load each count as a layer and draw nothing; an image of a url is laid out at its natural size. [render-root-background-image#natural]
- [x] css-backgrounds-3.md#typedef-bg-position: <bg-position> takes CSS Values' position and three words too: a side with its offset, and a side alone or center, center the axis no side names; an offset with no side before it, center twice, two sides of one axis or three sides are invalid. [render-root-background-sides#three render-root-background-place#four render-root-background-refused#positions]
- [x] css-backgrounds-3.md#typedef-bg-layer: each layer of the shorthand sets image, position with an optional size after a slash, repeat, attachment and one or two visual boxes in any order, the rest to their initial values; a colour is allowed in the final layer alone. [render-root-background-layer#shorthand render-root-background-invalid#dropped]
- [x] css-backgrounds-3.md#the-background: one visual box sets both background-origin and background-clip; <visual-box> is content-box, padding-box or border-box, not mask's fill-box and the like, so a shorthand with one is invalid and dropped with its colour, and background-clip takes none of them either. [render-root-background-layer#shorthand render-root-background-invalid#dropped render-root-background-refused#positions]

## Root corner clipping: root-border-radius, root-border-radius-box

- [x] css-backgrounds-3.md#border-radius: each corner radius is a non-negative length-percentage pair; one component supplies both axes; horizontal percentages use box width and vertical percentages use box height. [root-border-radius#length root-border-radius#font-length root-border-radius#root-font-rem root-border-radius#longhand-pair root-border-radius#percent-bases root-border-radius#zero-axis root-border-radius#right-longhand-pair root-border-radius#bottom-right-longhand-pair root-border-radius#bottom-left-longhand-pair]
- [x] css-backgrounds-3.md#border-radius: the shorthand expands one to four radii clockwise from top left, repeating omitted corners; a slash supplies the independent vertical set, otherwise both sets agree. [root-border-radius#two root-border-radius#three root-border-radius#four root-border-radius#slash]
- [x] css-backgrounds-3.md#corner-overlap: when adjacent radii exceed any side, all radii are reduced by the smallest side-to-sum ratio, preserving relative sizes. [root-border-radius#overlap root-border-radius#large-uniform root-border-radius#percent-ellipse]
- [x] css-backgrounds-3.md#corner-clipping: replaced content clips to its curved content edge, including with visible overflow; the root SVG image's box is its viewport (D39, D46). SVG graphics have no CSS layout boxes. [root-border-radius#overflow-visible root-border-radius#graphic-no-css-box root-border-radius#graphic-inherit-no-css-box root-border-radius-box#canvas-and-intersection]
- [x] css-backgrounds-3.md#root-background: the root's background paints the canvas, rather than the element's rounded box, and is not cut by its corner radius or clip-path. [root-border-radius-box#canvas-and-intersection]
- [x] css-backgrounds-3.md#propdef-border-radius: corner properties are not inherited; their initial radius is zero, and shorthand and longhands take part in the ordinary CSS cascade. [root-border-radius#inherit-root root-border-radius#initial root-border-radius#unset root-border-radius#longhand-override root-border-radius#shorthand-reset root-border-radius#importance root-border-radius#stylesheet]
- [x] css-backgrounds-3.md#propdef-border-radius: a negative radius or a nonzero unitless number is invalid; the non-negative math range clamps at used-value time. A malformed slash is invalid. [root-border-radius#invalid-negative root-border-radius#invalid-unitless root-border-radius#invalid-slash root-border-radius#negative-math-clamp root-border-radius#calc root-border-radius#invalid-longhand-first-number root-border-radius#invalid-longhand-second-number root-border-radius#invalid-longhand-number-math root-border-radius#longhand-zero-number root-border-radius#invalid-repeated-slash root-border-radius#invalid-leading-slash root-border-radius#invalid-fifth-radius]
