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


## D11. The user's language is en

`svg2/struct.md#ConditionalProcessingSystemLanguageAttribute` tests
systemLanguage against "the language tags indicated by user preferences".
The renderer has no user and takes no locale; its preference is en, so en,
EN, en-US and "fr, en" hold and eng, de and "" do not. A program that needs
another language has the document to change.

## D12. A symbol a use instances draws, whatever its own display

`svg2/struct.md#SymbolNotes`: the instance of a symbol that is a use's
referenced element "must always have a computed value of inline for the
display property", which the user agent style sheet
(`svg2/styling.md#UAStyleSheet`) makes important over any author style,
`:host(use) > symbol { display: inline !important }`. The renderer draws it.
The web platform test use-symbol-display-none expects a symbol with
display="none" to draw nothing, as the browsers do; the text is explicit, and
the corpus has no other such file. Conditional processing on the symbol still
leaves it out: it sets the used display, not the computed one.

## D13. The output keeps its transparency

`svg2/render.md#ParentCompositing`: a top-level document is the page group,
composited over an opaque white backdrop. The renderer is a library that
returns RGBA pixels, not premultiplied (README); the page, and a white
backdrop if any, is the embedding program's.

## D14. A clipPath child hidden by visibility adds nothing to the clip

`svg2/render.md#VisibilityControl` says a hidden graphics element still
"contributes to bounding box calculations and clipping paths";
`css-masking-1.md#ClipPathElement` says of the clipPath's own children that
one "made invisible by display or visibility ... does not contribute to the
clipping path". The masking text speaks of the clipping path itself and is
the newer, so the renderer follows it; a hidden element still counts in its
group's bounding box.

## D15. @import loads nothing

`svg2/styling.md#RequiredCSSFeatures` requires @import. The renderer fetches
no resource (README: images come from the embedding program); an imported
style sheet is not loaded and the rule is skipped, the rest of the sheet
applying.

## D16. An invalid presentation attribute is ignored, as a declaration

`svg2/types.md#syntax`: an attribute that fails to parse "is assumed to have
been specified as the given initial value", and for a presentation attribute
that initial value overrides the user agent's rules (overflow="invalid" on an
svg gives visible). The same chapter, `#TermInvalidValue`, says of an invalid
value "in a style sheet or a presentation attribute" that "a CSS declaration
with an invalid value is ignored". The text contradicts itself; the browsers
ignore the attribute, and real files with careless attributes rely on it: an
invalid presentation attribute is dropped and the property inherits or
cascades as if it were absent. An attribute that is no property (a
gradient's x1, a pattern's width) takes its initial value, as written.

## D17. min-width and max-width do not constrain SVG geometry

`svg2/geometry.md#Sizing`: the used width "may be constrained" by min-width
and max-width. It is a permission; SVG elements have no CSS box here, and the
renderer reads no min- or max- property.

## D18. A conforming viewer, without colour management

`svg2/conform.md#ConformingHighQualitySVGViewers` asks a high-quality viewer
for ICC colour management. The renderer draws in sRGB, as the conforming
viewer of `#ConformingSVGViewers` does: antialiased, resampling as
image-rendering asks, within a pixel and a component value, CTMs in doubles.

## D19. A percent: media fragment is a percent of the image's own coordinates

`svg2/linking.md#SVGFragmentIdentifiersDefinitions`: with "percent:" the
"resulting 'viewBox' transformation is calculated against referenced SVG
resolved size (width and height)", while a pixel fragment's numbers replace
the viewBox as they are, in user units. Taken literally, percent of a size
in CSS pixels used as a viewBox in user units would pick a different part of
a document whose viewBox is not its size. The renderer reads it as media
fragments mean it, a part of the image: percentages of the root's viewBox,
else of its size, so percent:0,0,50,50 is always the top left quarter.

## D20. A mask reference to an element that is no mask is a transparent layer

`css-masking-1.md#the-mask-image`: a mask reference that "is not a reference
to a mask element, is non-existent, or ... cannot be displayed still counts
as an image layer of transparent black". `svg2/linking.md#processingURL-validity`
says, for properties that take an element or an image file such as mask,
that a target of a wrong element type makes "the entire document file" the
image, its fragment a view. The renderer follows the masking text, the
property's own: such a layer is transparent and hides what it masks, as a
reference to nothing does.

## D21. One stroke bounding box, as CSS Masking computes it

`svg2/coords.md#TermStrokeBoundingBox` names the box that tightly contains
the stroke shape; `css-masking-1.md#compute-stroke-bounding-box` computes
it, for clip-path, mask-clip, mask-origin and mask-border, as the fill box
grown by half the stroke width, times the miter limit for a miter join and
the root of two for a square cap on shapes other than rect, circle and
ellipse, a container's box the union of its children's mapped up. The
renderer takes the algorithm for every stroke-box, transform-box's too, so
one element has one stroke box and none needs a stroke outline built to
measure it. The algorithm predates two cases, read as their geometry asks:
a miter-clip join, and arcs drawn as one (D4), grow as a miter does, and a
miter limit under one counts as one, as no miter shorter than the stroke's
half width can show.

## D22. The clip property does not cut the outermost svg

`css-masking-1.md#clip-property`: clip "applies only to absolutely
positioned elements. In SVG, it applies to elements which establish a new
viewport". The outermost svg establishes one, but it is the element with a
CSS box, not absolutely positioned, so the CSS rule governs it and the
renderer ignores clip there; a nested svg, a symbol's instance, a pattern
and a mask are cut.

## D23. mask-image none alone masks nothing

`css-masking-1.md#the-mask-image`: "A value of none counts as a transparent
black image layer." Read alone it would hide every element, none being the
initial value. The renderer counts none as such a layer only within a list
of layers, where mask-composite decides what it does; a value of none alone
is no mask, as CSS keeps it for every other property whose initial value is
none.

## D24. A clipPath whose use names no shape is ignored, as the spec says

`css-masking-1.md#ClipPathElement`: a use in a clipPath "must directly
reference path, text or basic shapes elements. Indirect references are an
error and the clipPath element must be ignored", and an ignored clipPath
applies no clipping (`#the-clip-path`). The issue note under it says no
browser does this: Chromium leaves the use out, clipping all away when it
is alone, and Firefox does not render the element. The renderer follows the
text: the element draws unclipped. Inkscape writes such clipPaths, a use
of a group, and its files now show their content where browsers show none.

## D25. Turbulence and spot lights as Filter Effects writes them, past the browsers

`filter-effects-1.md#elementdef-feturbulence`: "The C code below shows the
exact algorithm used for this filter effect." Filter Effects' code draws a
lattice gradient again while it is zero or longer than one ("Make
feTurbulence algorithms respect uniformity"); SVG 1.1's, which Chromium,
Firefox and resvg keep, normalizes whatever it draws. The renderer runs
Filter Effects' code, in single precision, so every turbulence differs from
the browsers' while it matches the spec's own code to a level.

`#elementdef-fespotlight`: no light where -L.S is below the cosine of
limitingConeAngle, and the light's focus is pow(-L.S, specularExponent).
The renderer cuts the cone sharp, as the formula does, rather than soften it
over a band as the browsers do; a cone of any given angle limits, zero
included; and the focus takes any exponent, where the browsers clamp it to
1 through 128, a light past a float as bright as it can be.
