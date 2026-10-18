# CSS Backgrounds and Borders Module Level 3, layer grammars borrowed by mask-*

## background-repeat grammar: mask-layers (exists)

- [ ] css-backgrounds-3.md#background-repeat: <repeat-style> = repeat-x | repeat-y | [repeat | space | round | no-repeat]{1,2}; a comma-separated list, one per layer; initial value repeat.
- [ ] css-backgrounds-3.md#valdef-background-repeat-repeat-x: repeat-x computes to "repeat no-repeat"; repeat-y computes to "no-repeat repeat".
- [ ] css-backgrounds-3.md#background-repeat: in a two-keyword value the first keyword applies to the horizontal axis, the second to the vertical.
- [ ] css-backgrounds-3.md#valdef-background-repeat-repeat: repeat tiles the image as often as needed to cover the painting area in that direction.
- [ ] css-backgrounds-3.md#valdef-background-repeat-space: space tiles as many whole copies as fit without clipping, then spaces them so the first and last touch the area's edges; with room for only one copy, position is used for it instead.
- [ ] css-backgrounds-3.md#valdef-background-repeat-round: round tiles a whole number of copies, rescaling the image to fit exactly (per the background-size round formula).
- [ ] css-backgrounds-3.md#valdef-background-repeat-no-repeat: no-repeat places the image once, not repeated in that direction.

## background-position grammar: mask-layers (exists)

- [ ] css-backgrounds-3.md#background-position: initial value 0% 0%; a percentage resolves against (positioning-area size - image size) on that axis.
- [ ] css-backgrounds-3.md#background-position: <bg-position> accepts 1, 2, 3 or 4 components; with only one, the second is assumed center.
- [ ] css-backgrounds-3.md#background-position: with two components, a bare length-percentage is horizontal-then-vertical; a pair of keywords may be reordered, but a keyword-plus-offset pair may not.
- [ ] css-backgrounds-3.md#background-position: with 3 or 4 components, each length-percentage offset must be preceded by an edge keyword; in the 3-component form the missing offset is zero.
- [ ] css-backgrounds-3.md#background-position: a positive offset is inward from the named edge, negative is outward.
- [ ] css-backgrounds-3.md#valdef-background-position-top: top/right/bottom/left compute to 0% or 100% on their axis when only one or two components are given.
- [ ] css-backgrounds-3.md#valdef-background-position-center: center computes to 50% on whichever axis is not otherwise specified.

## background-size grammar: mask-longhands (exists)

- [ ] css-backgrounds-3.md#background-size: <bg-size> = [<length-percentage [0,inf]> | auto]{1,2} | cover | contain; comma-separated list; initial auto.
- [ ] css-backgrounds-3.md#valdef-background-size-contain: contain scales preserving the natural aspect ratio to the largest size fitting both dimensions inside the positioning area.
- [ ] css-backgrounds-3.md#valdef-background-size-cover: cover scales preserving the natural aspect ratio to the smallest size covering both dimensions of the positioning area.
- [ ] css-backgrounds-3.md#background-size: with one length-percentage/auto value given, it is the width and the height is assumed auto.
- [ ] css-backgrounds-3.md#background-size: an auto dimension resolves via the natural aspect ratio and the other dimension, else the image's natural size, else 100%.
- [ ] css-backgrounds-3.md#background-size: with both values auto, the natural size is used if any; with no natural size at all, it is sized as for contain.
- [ ] css-backgrounds-3.md#background-size: negative length-percentage values are invalid.
- [ ] css-backgrounds-3.md#background-size: an image whose resolved width or height is zero is not displayed, as if it were transparent.
- [ ] css-backgrounds-3.md#background-size: if background-repeat is round on a dimension, that dimension's size is rescaled to X' = W / round(W/X), the nearest count of whole copies fitting the positioning area width W.
- [ ] css-backgrounds-3.md#background-size: if round applies to only one dimension and the other is auto, that other dimension is then scaled to restore the original aspect ratio.

## background-clip and background-origin box keywords: mask-longhands (exists)

- [ ] css-backgrounds-3.md#background-clip: background-clip: border-box | padding-box | content-box (a comma list); initial border-box; clips painting to the named box.
- [ ] css-backgrounds-3.md#background-origin: background-origin: border-box | padding-box | content-box (a comma list); initial padding-box; sets the positioning area's origin to the named box.

## Notes
