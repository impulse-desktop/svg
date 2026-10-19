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

## The root's background on the canvas: render-root-background-opacity, render-root-background-blend, render-root-background-shorthand, render-root-background-reset, render-root-background-layers, render-root-background-keyword, render-root-background-current

- [x] css-backgrounds-3.md#special-backgrounds: the root element's background is the canvas's, painted under the root element: the root's opacity does not fade it, and the root's content blends with it as its backdrop. [render-root-background-opacity#canvas render-root-background-blend#canvas]
- [x] css-backgrounds-3.md#propdef-background: the background shorthand sets background-color from the colour of its final layer, else to its initial value, transparent; the renderer reads it for that colour alone, the canvas's. [render-root-background-shorthand#final-layer render-root-background-layers#final-layer render-root-background-reset#no-colour render-root-background-keyword#initial render-root-background-current#currentcolor]
