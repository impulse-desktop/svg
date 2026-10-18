# CSS Images Module Level 3, Image values, gradients, sizing, image-rendering

## Image references: mask-values, mask-image-url (exists)

- [x] css-images-3.md#url-notation: an image given by url() that cannot be decoded is treated as an invalid image.
- [x] css-images-3.md#invalid-image: an invalid image renders as a solid transparent image with no natural dimensions.
- [x] css-images-3.md#loading-image: a still-loading image is not invalid but renders the same way: solid transparent, no natural dimensions.
- [x] css-images-3.md#ambiguous-urls: a fragment-only url() is always an element reference, never an image.
- [x] css-images-3.md#ambiguous-urls: a non-fragment-only url() is an element reference only if its fragment names an element valid in context (e.g. a mask element for mask-image), else it is an image.

## Image and gradient value types: css-gradient-geometry, mask-syntax (exists)

- [x] css-images-3.md#image-values: <image> = <url> | <gradient>.
- [x] css-images-3.md#gradients: <gradient> = linear-gradient() | repeating-linear-gradient() | radial-gradient() | repeating-radial-gradient().
- [x] css-images-3.md#gradients: a gradient draws into a box the size of the concrete object size (the gradient box); the gradient itself has no natural dimensions.

## linear-gradient() direction: css-gradient-geometry, mask-syntax (exists)

- [x] css-images-3.md#linear-gradient-syntax: linear-gradient() = linear-gradient([<angle>|<zero>|to <side-or-corner>]?, <color-stop-list>); the direction may be omitted, defaulting to "to bottom".
- [x] css-images-3.md#linear-gradient-syntax: <side-or-corner> = [left|right] || [top|bottom]; each side keyword may appear at most once.
- [x] css-images-3.md#linear-gradient-syntax: a bare angle number needs a unit identifier unless the angle is zero.
- [x] css-images-3.md#linear-gradient-syntax: for an <angle>, 0deg points up and positive angles rotate clockwise, so 90deg points right.
- [x] css-images-3.md#linear-gradient-syntax: to top/right/bottom/left set the gradient line angle to 0deg/90deg/180deg/270deg respectively.
- [x] css-images-3.md#linear-gradient-syntax: a corner keyword (e.g. "to top left") angles the line into that quadrant, perpendicular to the line joining the two neighboring corners, so 50% crosses both of them.
- [x] css-images-3.md#linear-gradient-syntax: given gradient box width W, height H and angle A, the gradient line length between its starting and ending points is abs(W*sin(A)) + abs(H*cos(A)).
- [x] css-images-3.md#linear-gradient-syntax: the gradient line extends infinitely; color stops may be placed before 0% (the starting point) or after 100% (the ending point).

## radial-gradient() shape, size and position: css-gradient-geometry, mask-syntax (exists)

- [x] css-images-3.md#radial-gradient-syntax: radial-gradient() = [<radial-shape>||<radial-size>]? [at <position>]?, <color-stop-list>; the position defaults to center.
- [x] css-images-3.md#valdef-radial-gradient-radial-shape: the shape defaults to circle when the size is a single <length>, otherwise to ellipse.
- [x] css-images-3.md#valdef-radial-gradient-radial-size: the size defaults to farthest-corner when omitted.
- [x] css-images-3.md#valdef-radial-extent-closest-side: closest-side sizes the ending shape to exactly meet the nearest side (each dimension, for an ellipse).
- [x] css-images-3.md#valdef-radial-extent-farthest-side: farthest-side is the same but meets the farthest side(s).
- [x] css-images-3.md#valdef-radial-extent-closest-corner: closest-corner sizes the ending shape to pass through the nearest corner, using the closest-side aspect ratio for an ellipse.
- [x] css-images-3.md#valdef-radial-extent-farthest-corner: farthest-corner passes through the farthest corner, using the farthest-side aspect ratio for an ellipse.
- [x] css-images-3.md#valdef-radial-size-length-0: a circle's explicit size is a single non-negative <length>; percentages are invalid for a circle.
- [x] css-images-3.md#valdef-radial-size-length-percentage-0-2: an ellipse's explicit size is two non-negative <length-percentage>s (horizontal radius, then vertical), percentages relative to the gradient box.
- [x] css-images-3.md#radial-gradient-syntax: combining an explicit shape or extent keyword together with two explicit size values (e.g. "circle 4px 4px" or "closest-side 4px") is invalid.

## Degenerate radial gradients: css-gradient-geometry, gradient-degenerate (exists)

- [x] css-images-3.md#degenerate-radials: a circle ending shape with zero radius is rendered as a circle of an arbitrarily small positive radius.
- [x] css-images-3.md#degenerate-radials: an ending shape with zero width (any height) is rendered as an ellipse of arbitrarily large height and arbitrarily small positive width, mirrored across its center; percentage stop positions then all resolve to 0px.
- [x] css-images-3.md#degenerate-radials: otherwise, an ending shape with zero height is rendered as a solid color equal to the last color stop, or the gradient's average color if repeating.

## Repeating gradients and their average color: css-gradient-geometry, gradient-degenerate (exists)

- [x] css-images-3.md#repeating-gradients: repeating-linear-gradient() and repeating-radial-gradient() take the same values as their non-repeating forms.
- [x] css-images-3.md#repeating-gradients: rendering repeats the color stops infinitely, shifted each time by the distance between the first and last stop's positions.
- [x] css-images-3.md#repeating-gradients: if that distance is zero, or too small for the output's resolution, the gradient renders as a solid color equal to the gradient's average color.
- [x] css-images-3.md#repeating-gradients: that average color uses a nominal nonzero stop spacing to compute it when the distance between first and last stop is exactly zero.
- [x] css-images-3.md#repeating-gradients: for a repeating radial gradient, a nonzero ending-shape width with a zero or near-zero height likewise renders as the solid average color.
- [x] css-images-3.md#gradient-average-color: the average color sums, over each adjacent stop pair, each stop's premultiplied sRGBA color scaled by half the pair's distance divided by the total distance between the first and last stop.

## Color stop list grammar and fixup: css-gradient-geometry, mask-syntax (exists)

- [x] css-images-3.md#color-stop-syntax: <color-stop-list> = <linear-color-stop>, [<linear-color-hint>?, <linear-color-stop>]#?; a hint cannot appear before the first or after the last stop, nor two hints in a row.
- [x] css-images-3.md#color-stop-syntax: a stop's percentage position resolves against the gradient line length between its starting and ending points; lengths are measured the same way from the starting point.
- [x] css-images-3.md#color-stop-fixup: if the first stop has no position it is set to 0%; if the last has none it is set to 100%.
- [x] css-images-3.md#color-stop-fixup: a stop or hint whose position is less than the largest position already fixed among earlier stops/hints is raised to that largest position.
- [x] css-images-3.md#color-stop-fixup: any remaining positionless stops are spaced evenly between the nearest positioned stops before and after them.
- [x] css-images-3.md#coloring-gradient-line: between two color stops the gradient line is interpolated in premultiplied RGBA, linearly unless a transition hint changes it.
- [x] css-images-3.md#coloring-gradient-line: with a hint at fraction H between two stops, a point at fraction P blends (1-C) of the first stop and C of the second, where C = P raised to the power log base H of 0.5.
- [x] css-images-3.md#coloring-gradient-line: stacked stops at the same position produce an abrupt change from the earlier-listed to the later-listed color, not a smooth transition.

## image-rendering keywords: image-rendering-modes, style-image-rendering (exists)

- [x] css-images-3.md#the-image-rendering: image-rendering accepts auto | smooth | high-quality | pixelated | crisp-edges; initial auto; inherited.
- [x] css-images-3.md#valdef-image-rendering-pixelated: pixelated scales each axis to the nearest positive integer multiple of its natural size with nearest-neighbor, then the rest of the way as for smooth.
- [x] css-images-3.md#valdef-image-rendering-crisp-edges: crisp-edges scales to avoid blurring edges or blending colors, e.g. via nearest-neighbor.
- [x] css-images-3.md#the-image-rendering: the deprecated legacy values optimizeSpeed and optimizeQuality must be accepted and behave as crisp-edges and smooth respectively.

## Sizing replaced content, object-fit and object-position: image-object-fit

- [x] css-images-3.md#natural-dimensions: a natural aspect ratio with a zero or infinite part is degenerate and treated as no natural aspect ratio.
- [x] css-images-3.md#default-sizing: with no specified size, an object with a natural width or height uses its natural dimensions; otherwise it is resolved as a contain constraint against the default object size.
- [x] css-images-3.md#cover-contain: a contain constraint sizes to the largest rectangle with the object's natural aspect ratio that fits within the constraint rectangle; without a natural aspect ratio the constraint rectangle is used as is.
- [x] css-images-3.md#cover-contain: a cover constraint sizes to the smallest rectangle with the object's natural aspect ratio that is at least as large as the constraint rectangle in both dimensions.
- [x] css-images-3.md#the-object-fit: object-fit: fill | contain | cover | none | scale-down; initial fill; applies to replaced elements; not inherited.
- [x] css-images-3.md#valdef-object-fit-fill: fill sizes the content to exactly the element's used width and height.
- [x] css-images-3.md#valdef-object-fit-contain: contain resolves the concrete object size as a contain constraint against the element's used width and height.
- [x] css-images-3.md#valdef-object-fit-cover: cover resolves it as a cover constraint against the element's used width and height.
- [x] css-images-3.md#valdef-object-fit-none: none uses the default sizing algorithm with no specified size and a default object size equal to the element's used width and height.
- [x] css-images-3.md#valdef-object-fit-scale-down: scale-down is none or contain, whichever gives the smaller concrete object size.
- [x] css-images-3.md#the-object-position: object-position is a <position>, initial 50% 50%, percentages relative to the element's own width and height, resolved with the content box as the positioning area.

## Notes

- css-images-3.md#coloring-gradient-line: the hint interpolation exponent C = P^log_H(0.5) is a per-pixel pow/log computation; an exact implementation costs speed where a cheaper approximation would be visually indistinguishable.
- css-images-3.md#repeating-gradients: "too small for the output's resolution" is left to the implementation, ambiguous in a way that changes which near-degenerate gradients collapse to a solid average color.
- css-images-3.md#degenerate-radials: the "arbitrarily small" and "arbitrarily large" numbers used for a degenerate ending shape are unspecified, ambiguous in a way that changes edge pixels near the degenerate axis.
