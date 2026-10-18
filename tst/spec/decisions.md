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

## D3. A non-scaling stroke is as wide in output pixels at any size

`svg2/painting.md#non-scaling-stroke`: the outline is computed in "the
viewport coordinate system of the furthest ancestral SVG viewport", so that
the width depends on neither the element's transforms "and zoom level".
`svg_document_render` stretches the document's own size to the pixels asked
for, a zoom: the outline is computed in output pixels, and a stroke 2 wide is
2 pixels at any size. A unit drawn at its own size shows the same either way.

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
