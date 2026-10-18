# Decisions

Where the renderer reads the specs one way among several, departs from them,
or leaves a statement out: each entry names the statement (a link into
`dev/spec`), what was decided and why. Newest last.

## D1. Approximations the specs permit are kept

- Filter Effects 1, `filter-effects-1.md#feGaussianBlurElement`: for a
  deviation of 2 or more, three box blurs "can be used". Kept: an exact
  kernel costs a third more time on files that blur much, and the text
  allows the boxes.
- SVG 2, `svg2/painting.md#StrokeShape`: the ideal stroke shape gives
  "implementations ... some leeway to deviate from this description for
  performance reasons". Curves are flattened within a tolerance.

## D2. A filter primitive's subregion is snapped out to whole pixels where it meets its content's edge

`filter-effects-1.md#FilterPrimitiveSubRegion`: the subregion restricts the
result to a rectangle. Where a side lies inside the content (an offset's
input, a flood, a blur's tails) the pixels it crosses keep the part of them
it covers; where it lies on the content's own edge (a filter whose region is
the element's box) the edge's antialiasing is left as it is, as the pixel
cannot tell the two apart.

## D3. Vector effects are computed in output pixels at any size

`svg2/painting.md#non-scaling-stroke`: the outline is computed in "the
viewport coordinate system of the furthest ancestral SVG viewport", so that
the width depends on neither the element's transforms "and zoom level";
`svg2/coords.md#VectorEffects` takes the same host coordinate space for every
vector effect. `svg_document_render` stretches the document's own size to the
pixels asked for, a zoom: the host is the output's pixels, and a stroke 2 wide
is 2 pixels at any size, a non-scaling-size shape as many pixels, a
fixed-position one at its pixels. A unit drawn at its own size shows the same
either way.

## D4. stroke-linejoin arcs is drawn as miter-clip

`svg2/painting.md#TermLineJoinShape`, `#CurvatureCalculation`,
`#ArcsLinejoinFallback`: arcs extends the outer edges by circles of their
curvature at the join. The value is at risk (`#issue592`, no implementation),
its formulas have slips (the quadratic's end curvature repeats the start's),
and only two test files of the corpus use it. Between straight segments arcs
falls through to miter-clip by its own text; the renderer draws it so between
curved ones too. miter-clip itself is drawn as written, including a limit
under 1 cutting into the bevel and a turn back running on a limit's half width.

## D5. A rect with square corners is a closed subpath

`svg2/shapes.md#RectElement`: the equivalent path closes only in step 9, the
rounded corner's segment-completing close; with rx or ry zero it would be an
open subpath with caps where it starts. SVG 1.1 closes it with closepath, as
every renderer strokes it: four joins. The renderer takes that path, so its
markers' last vertex is the start and its start marker turns halfway between
the closing and the first segment.

## D6. A dash of no length at a corner is one square, along the segment after

`svg2/painting.md#TermCapShape`: a dash's start cap goes along the subpath that
starts at its position, its end cap along the one that ends there; a zero
length dash exactly at a corner would be half a square each way. The renderer
draws the whole square along the segment after the corner, as the dash's start
cap has it. Elsewhere the two caps agree.

## D7. A pattern's overflow is clipped to its tile, visible or not

`svg2/pservers.md#PatternNotes`: with overflow visible "the rendering
behavior for the pattern outside the bounds of the pattern is currently
undefined", rendering it only "encouraged". The text leaves it open, so the
browsers settle it: they clip to the tile, and the files of the corpus that
set overflow visible on a pattern, Inkscape's among them, were drawn for
that.

## D8. A paint server that cannot paint, or is in error, takes the fallback

`svg2/painting.md#DataTypePaint` uses the fallback when the reference is to
"an element that does not exist or which is not a valid paint server"; a
pattern of zero width or height "disables rendering of the element (i.e., no
paint is applied)" (`svg2/pservers.md#PatternElementWidthAttribute`), a
negative radius of a radial gradient is an error
(`#RadialGradientElementRAttribute`). The renderer takes a pattern that
cannot draw (no size, no content, an empty viewBox) and a gradient in error
for no valid paint server: the fallback paints if there is one, else nothing.
The W3C test pservers-pattern-03-f and the browsers read it so. What the text
says paints nothing does: a gradient without stops paints as none, a radial
gradient whose start circle covers its end circle draws nothing.

## D9. A template's content is cloned under the server that uses it

`svg2/pservers.md#PaintServerTemplates`: the content taken from a template
forms a shadow tree whose host is the paint server using it, "as if they were
the paint server's own children", and inherits from it: stop-color
currentColor is the using gradient's color, an unfilled shape in a pattern
fills as the using pattern says, not as the template does. The browsers keep
the template's own inheritance; Inkscape's stock patterns set their fill on
the template and are used through link patterns that set none, so through a
link they fill black here (three files of the corpus).

## D10. A document's size comes from itself, with no host

`svg2/coords.md#SizingSVGInCSS` leaves a width or height given as auto or a
percentage to the host's CSS default sizing, with the viewBox's ratio as the
intrinsic aspect ratio. The renderer has no host and its API gives each
document a size (README): an absolute width and height as given; one of them
open, from the other and the viewBox's ratio; both open, the viewBox's size;
and with no viewBox the right and bottom edges of what the document draws.

