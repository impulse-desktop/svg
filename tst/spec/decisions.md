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
applying. An xml-stylesheet instruction likewise cannot fetch an external
resource; its data: URL, already present in the document's bytes, is decoded
and applied before the document's style elements.

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

## D20. A mask reference to nothing, or to no mask, is none

`css-masking-1.md#the-mask-image`: a mask reference that "is not a reference
to a mask element, is non-existent, or ... cannot be displayed still counts
as an image layer of transparent black". `svg2/linking.md#processingURL-validity`
says, for properties that take an element or an image file such as mask,
that a target of a wrong element type makes "the entire document file" the
image, its fragment a view. An image that does not load is a transparent
layer, as the masking text has it.

A reference to no element at all is another matter (README, the goal, rule
2): Chromium and resvg draw the element unmasked, as Gecko's test
mask-basic-05 and SVG 1.1's masking-path-10-b expect, and real files have
such references, left
by editors: 3 of 343 masked files of a Wikimedia Commons sample, 5 in the
corpus. The renderer takes such a layer as none: alone, no mask.

So is a reference to an element that is no mask. Chromium, Firefox and
librsvg draw the element unmasked; resvg alone hides it. Real files count
on it: 5 harvested files of the corpus mask by a gradient, a group or a
clipPath, a badge among them, and Chromium draws each unmasked where the
masking text would hide it whole. Before, the renderer followed that
text.

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

## D26. BackgroundImage is the isolation group's backdrop; enable-background is gone

`filter-effects-1.md#attr-valuedef-in-backgroundimage`: BackgroundImage is
"the back drop defined by the current isolation group behind the filter
region"; `#AccessBackgroundImage`: "This specification does not support the
enable-background property. UAs must support the isolation property
instead." The renderer had kept SVG 1.1's enable-background, a background
gathered from the nearest group marked new. It now reads the pixels the
filtered element's isolation group drew before it, transparent at the start
of an isolated group, and ignores enable-background; the W3C SVG 1.1 and
resvg tests written for it draw differently.

## D27. Colours are mapped into sRGB as they are read

`css-color-4.md#color-convert-intro`: conversions are not clamped, and CSS
gamut mapping happens "only for display", so a colour outside sRGB keeps
its values through computation, a gradient's interpolation included. The
renderer draws in sRGB and keeps colours as sRGB values, so it maps each
colour into sRGB by the Binary Search with local MINDE algorithm when it
reads it, and a gradient between two colours outside sRGB interpolates
their mapped values. Every colour inside sRGB, all of SVG 1.1's and the
legacy functions' in range, is unchanged by this.

## D28. Tiles of a fractional period are drawn from one rasterized tile

`css-backgrounds-3.md#valdef-background-repeat-space` and `#background-size`
(round) place tiles at exact, often fractional, offsets: space spreads the
room left between whole copies, round rescales a tile to a whole count. The
renderer draws a tiled mask layer, as a pattern, by rasterizing one tile
and sampling it at the period, so where the period is no whole number of
pixels a tile's edges blur and move within a pixel of their exact place. A
tile of a whole period, an image tiled edge to edge and a single tile are
drawn exactly.

## D29. A font's metrics are the fallbacks of a font without them

`css-values-4.md#font-relative-lengths`: ex, cap, ch, ic and lh are metrics
of the first available font. The renderer draws no text and has no fonts,
so it takes the fallbacks the units give: ex "a value of 0.5em must be
assumed", ch "0.5em wide", ic "1em". cap falls back to "the font's ascent"
and lh is line-height normal, metrics of a font too; SVG 1.1 gives a font
without them an ascent of its units-per-em and a descent of 0
(`svg11/fonts.md#FontFaceElementAscentAttribute`), so cap and lh are 1em.
line-height, a text property, is not read. The root forms (rex, rch, ric,
rcap, rlh) are the same of the root's font size. Before, ex was 0.459em and
lh 1.149em, a browser's default serif font's; the resvg tests of ex, lh and
rlh, written for Noto Sans, draw differently.

## D30. Math functions go where CSS's value syntax does

`css-values-4.md#calc-type-checking`: a math function "can be used anywhere
a value of that type is allowed". The renderer reads them where a property
or attribute is written in CSS's value definition syntax
(`svg2/types.md#syntax`): lengths, numbers and integers, percentages,
angles, colour channels, number lists, points, viewBox, a transform
property's functions, filter functions, media queries. Path data and the
transform attribute have grammars of their own (`svg2/paths.md#PathDataBNF`,
`css-transforms-1.md#svg-syntax`) and take none. A math function that needs
the viewport or the font (a percentage of a length, em, vw) is evaluated
each time its length is resolved; where a number is resolved as it is read
(opacity, an integer, a colour channel), one that needs them is invalid.
An infinity is clamped to 3e38, the largest number the parser keeps, and a
negative result where a plain negative value is invalid is clamped to zero
(`#calc-range`): rx calc(-5px) is 0, rx -5 is auto. round(line-width) snaps
to whole px, the renderer's device pixel before its output scale.

## D31. The viewport units are of the document's own size

`css-values-4.md#viewport-relative-lengths`: vw, vh and the rest are of the
initial containing block. The document's own size (D10) is what it lays out
in, stretched to the pixels it is drawn into (README), so the viewport
units are of that size. Before, they were of the output pixels, and a
document drawn at twice its size had twice the vw in its user units.

## D32. A value is read from its tokens, its escapes in them

`css-syntax-3.md#syntax-description`: any code point may be escaped in an
identifier, and the tokenizer keeps an escaped delimiter part of its token.
The renderer reads every CSS value from its tokens (css-syntax-3 4 and 5):
fill: \6c ime is lime, an identifier of four letters, and rgb\28 0 255
0\29 no colour but an identifier, numbers and a dimension, invalid.
Chromium, Firefox, librsvg and resvg all draw it so. Before the renderer had
a tokenizer it read values from their text and undid a value's escapes
first, which made rgb\28 0 255 0\29 green; no renderer does. Presentation
attributes are read as CSS too (`svg2/types.md#presentation-attribute-css-value`),
their comments and escapes as the tokenizer reads them; so are other
attributes written in CSS's value syntax, viewBox, points and number lists.

## D33. Dynamic and link pseudo-classes are valid and never match

`selectors-4.md#invalid` leaves open whether a pseudo-class a renderer never
activates is unsupported, invalidating its selector list, or always false.
The renderer has no pointer, focus, target or history, and takes :hover,
:active, :focus, :target and :visited as valid and never matching, so the
other selectors of their list still apply. It draws documents as images,
whose links nothing follows, so no element acts as the source anchor of a
hyperlink (`selectors-4.md#any-link-pseudo`): :any-link and :link match
nothing either, as in a browser's SVG image.

## D34. Presentation attributes are below every author rule, layered or not

`css-cascade-5.md#preshint` lets a document language put its presentational
hints in the author origin, "for example, SVG11 maps its presentation
attributes into the author origin", and SVG 2 says they "contribute to the
author level of the cascade, followed by all other author-level style
sheets" (`svg2/styling.md#PresentationAttributes`). With cascade layers that
reads as an unlayered sheet, which beats every layered rule; the same
module's changes, though, record that "presentational hints use the author
presentational hint origin instead of layers" (issue 6659), an origin below
the author's. The spec says both; the browsers draw a layered rule over a
presentation attribute, and so does the renderer: presentation attributes
are of the author presentational hint origin, below every author rule,
layered or not, and above the user agent's. revert rolls them back with the
author origin; revert-layer reaches them after the earliest layer.

## D35. A color() of an unknown space is no color, as its grammar has it

`css-color-4.md#color-function` says a color() whose space name matches none
of the predefined ones is an invalid color, whose used value is opaque
black, and shows it with a misspelt profoto-rgb. Its grammar, though, takes
only the predefined names, <predefined-rgb> and <xyz-space>, so such a
color() does not parse, and a declaration of it is invalid and ignored. The
spec says both; the browsers drop the declaration, and so does the
renderer: the property keeps what it had, an inherited or initial value or
a presentation attribute under it.

## D36. A percentage dash is a length, which pathLength scales

`svg2/paths.md#PathLengthAttribute` says pathLength "has no effect on
percentage distance-along-a-path calculations", and that zero scales "any
non-percentage value" to infinity. The percentages it means are those of
the path's own length: text on a path's startOffset, motion's key points,
which text and animation draw, both out of scope. A percentage of
stroke-dasharray or stroke-dashoffset is not of the path but of the
viewport's normalized diagonal, a length, and painting says that "each dash
and gap length is interpreted relative to the author's path length" with
no exception, its stroke shape scaling every dash position alike. The
renderer scales it as any other dash; the statement is left out as text's
and animation's.

## D37. A use whose reference resolves to nothing has an empty box

`svg2/coords.md#TermBoundingBox` says an element in the rendering tree
referencing an unresolved resource still has a bounding box from its
attributes, "`<use href="#bad" x="10" y="10"/>` would have a bounding box
with an x and y of 10 and a width and height of 0". Its algorithm, though,
gives a use the union of its shadow tree's children, starting from
(0, 0, 0, 0), and a use that references nothing has none: an empty box,
which adds nothing to an ancestor's. The spec says both; Chromium and
Firefox give such a use (0, 0, 0, 0) and leave its group's box as it was,
and so does the renderer. A shape whose paint resolves to nothing keeps the
box of its geometry, which both readings give it.

## D38. A viewport whose overflow clips flattens a preserve-3d

`css-transforms-2.md#grouping-property-values` makes overflow of any value
but visible or clip flatten an element with transform-style preserve-3d;
clip does not. An SVG viewport that clips, though, cuts what it holds at
its edge in its own plane, while the planes of a 3D rendering context it
established would be laid over its parent's space past that cut, the
renderer drawing each plane apart. It flattens on overflow clip as on
hidden: its content is drawn in its plane, cut there, as its overflow says.

## D39. The outermost svg's CSS box draws nothing

CSS gives the root element a box with border, padding, margin, an outline
and a shadow; SVG 2 says nothing of them for an outermost svg. The renderer
has no host to lay that box out in (D10): the raster is the svg's viewport,
the document's own size stretched to it. Border, padding, margin and
box-shadow draw nothing and take no room from the viewport, and an outline,
which lies outside the box, falls past the raster.

## D40. Languages are matched as written; an instance's is its host's

`selectors-4.md#the-lang-pseudo` canonicalizes the content language and
each range, and converts them to extlang form (RFC 5646, 4.5), before the
extended filtering. Both need the IANA subtag registry, its preferred
values and extlang prefixes, which the renderer does not carry: it filters
the tags as written, ASCII case-insensitively, and an ill-formed tag or
range matches nothing. `svg2/struct.md#LangSpaceAttrs` takes an element's
language from xml:lang, then lang, then its parent; SVG does not say where
an instance of a use's shadow tree gets its language past its root. DOM
gives a shadow tree's root its host's, so an instance's language is the
use's, not that of the element it copies.

## D41. feImage draws an element as a use of its subregion

`filter-effects-1.md#elementdef-feimage` renders a referenced element
"according to the behavior of the use element", in the user space
primitiveUnits gives, and says no more of the feImage's x, y, width and
height than that they are its subregion. The renderer takes the subregion
for the use's x, y, width and height: the element moves to the subregion's
corner, a symbol or an svg becomes a viewport of the subregion's size, and
the element inherits from the feImage. With primitiveUnits objectBoundingBox
the element lies in the bounding box's units, as the spec says. Before,
an element of percentage lengths was stretched from the viewport to the
subregion, as Chromium does, and a symbol drew nothing.

## D42. A length of a unit in bounding box units is of the box's user space

`svg2/coords.md#ObjectBoundingBoxUnits` speaks of fractions and
percentages of the box, and puts the box's matrix into the transforms as
"a new user coordinate system"; the attributes' grammar takes lengths. A
percentage is the fraction it names; a number is a fraction; any other
length is resolved as lengths are, its px a user unit, its em the element's
font-size, and is a distance in that user space, so the box's matrix scales
it. Before, its unit was dropped and 0.25em read as 0.25.

## D43. A declaration's plain number is a length, as in the attribute

`svg2/types.md#presentation-attribute-css-value` lets a presentation
attribute give a length as a plain number and disallows it in the
property's values, so style="font-size:10" would be invalid. Chromium,
WebKit and resvg read it as 10px, and real files count on it: 56 of a
10 178-file sample of Wikimedia Commons, Batik's samples, WebKit's tests.
Rejecting it would draw those files unlike anything their authors saw, for
no gain (README, the goal), so a declaration takes a plain number where the
attribute does. Width and height in a style sheet are the exception:
Chromium and Firefox drop a plain number there, as CSS does, though
Chromium takes one in a style attribute, and a real file's sheet that gives
every rect width: 128 draws each rect at its own width in both.

## D44. A template's content inherits from the template

`svg2/pservers.md#PaintServerTemplates` clones a template's content into a
shadow tree whose host is the paint server that uses it, so the content
would inherit from that paint server. Inkscape writes its stock patterns
the other way: the stock pattern carries the colour in its style, its
content none, and a pattern of nothing but an href and a patternTransform
paints with it; Wikimedia Commons maps and Inkscape's own Patterns.svg
draw so. Inkscape 1.4, Chromium, Firefox, librsvg and resvg all draw a
template's content, a pattern's or a gradient's stops, inheriting from the
template where it is in the document, a style on the referencing paint
server ignored (README, the goal, rule 1: a proven cost), and so does the
renderer. Before, it followed SVG 2 and drew those patterns black.

## D45. Every code point past ASCII is an ident code point

`css-syntax-3.md#non-ascii-ident-code-point` narrowed the code points past
ASCII that an identifier takes to the ranges of HTML's custom element
names: U+00D7, U+00F7, U+00A0, U+2000 to U+206F and others are no longer
in it, and a class selector of one is a delim in the middle of the
selector, which drops the rule. Chromium and Firefox take every code point
past ASCII in an identifier, as CSS 2 did: of eight class selectors with é,
×, ÷, a no-break space, U+2000, U+2028, the unassigned U+0378 and the
noncharacter U+FDD0 between two letters, they match all eight. Being
stricter than they are would drop rules real files write and the browsers
apply (README, the goal, rule 5), so the tokenizer takes every byte past
ASCII as part of an ident code point, as it did before it had a
tokenizer.

## D46. The outermost svg's boxes are its viewport's

`css-masking-1.md#the-clip-path` and `#the-mask-origin` map the geometry
boxes of an element with a CSS layout box onto its CSS boxes: fill-box is
the content box, stroke-box and view-box the border box. The outermost svg
has one, its viewport, and its clip-path, mask layers and mask border apply
in that box's space, as a userSpaceOnUse clipPath does: Chromium and
Firefox draw a root's userSpaceOnUse clip in the box's px, the viewBox
aside. So a basic shape, a mask layer's origin and clip and a mask border
on the root are sized against the viewport, as both browsers draw them.
Before, the renderer took the root's content bounding box in its viewBox's
units, which clipped github/25__2544b1fee055fdd8, a 24 px icon with a
circle on its root, wrongly at any size but 24.

objectBoundingBox units, a clipPath's, a mask's or a filter's, name the
element's bounding box (`svg2/coords.md#bounding-box`), the box of its
content in its user space. On the root the renderer maps that box into the
viewport by the root's viewBox, where its clip, mask and filter apply, as
it did for filters already. Chromium takes the border box instead; Firefox
clips away all of a root with an objectBoundingBox clip or mask and draws
no filter on the root. No file of the corpus has one, and the spec names
the bounding box.

## D47. An HTML style element is a style sheet

SVG 2 has HTML's link element bring style sheets into an SVG document
(`svg2/styling.md#LinkElement`) but says nothing of HTML's style element;
HTML processes a style element in its namespace wherever it is connected,
and so do Chromium, Firefox and resvg in an SVG document, in a foreign
subtree and in a foreignObject too; librsvg alone reads none. Real
files write one: 6 harvested files of the corpus (spaCy's displaCy among
them, whose arrows its sheet colours), which drew their elements
unstyled. The renderer reads an element of the XHTML namespace named style
as a style element of its nearest SVG ancestor, its media and type
attributes as SVG's; any other element of that namespace stays foreign
and drawn by nothing.

## D48. A duplicated edge that cuts a pixel holds the inside's colour

`filter-effects-1.md#attr-valuedef-fegaussianblur-edgemode-duplicate`:
duplicate extends the input image "by duplicating the color values at the
given edge". Where the edge of the input lies inside a pixel, as a filter
region on a bounding box drawn at most sizes does, that pixel stores the
part of it inside the region, its colour scaled by its share; duplicating
it as stored put a pale or transparent band one pixel in from the edge of
a feConvolveMatrix whose kernel sums to 1 and a blur with edgeMode
duplicate (W3C filters-conv-05-f). The colour at the edge is the inside's:
such a pixel is read as what it stores over its inside share, the part
outside the edge holding the same colour, as the mode says. Chromium,
librsvg and resvg draw the convolution so where they draw the edge on
whole pixels; a blur's edge mode they leave out. Wrap and mirror bring
other pixels across the edge and keep the pixels as stored.

## D49. A small loop or returning tip keeps every renderer's hole

`svg2/painting.md#StrokeShape` defines the ideal stroke as the sweep of the
subpath's normals, so a circle stroked wider than its diameter would be a
disk, and leaves implementations leeway, warning that strokes "at
extremely tight curves" differ across platforms. Chromium, Firefox,
librsvg, resvg and Inkscape all stroke a closed subpath as its outer and
inner offsets filled nonzero: where the inner offset turns inside out all
the way round it leaves a hole of radius h - r, and a stroke wider than
the image around a small circle vanishes. Real files count on it: of the
76 354 harvested files the renderer drew apart from resvg, six stroke a
closed curve so, an Inkscape recording icon among them, its ring a circle
of radius 1.5 stroked 18.6 wide, and Figma icons whose circles carry
stroke-width 44 that every renderer hides; the renderer drew solid disks
over them. Its closed subpaths keep their inner side on its offset where
the curve is tighter than the half width, no sector filling it. Open paths
ordinarily keep the swept shape. Real Sketchy-SVG drawings also use tiny,
almost returning open loops as spots: Chromium, resvg and librsvg leave
their centers open, and those openings are visible parts of the author's
drawing. Their endpoints are within half the stroke width, their
centerline fits within one stroke width in both axes, and they turn through
more than a half-circle. Those loops keep their inverted inner offset too,
while their end caps remain open. A long open path can make the same small
reversal at either tip. When the tangents entering and leaving the part
within half a stroke width of the endpoint point against one another, its
inverted inner offset is kept locally. Chromium, resvg and librsvg leave
the resulting opening inside the tip, visible in Sketchy-SVG white strokes;
the old swept fill closed it. A tight bend whose tangent does not return,
including `stroke-tight-ends#end-tight`, still uses the ideal swept shape.
`stroke-hairpin-tip` tests both path directions against a fixed Chromium
drawing; resvg agrees on the opening.

## D50. A focal point within 1/4096 of the radius of the end circle is on it

`svg2/pservers.md#RadialGradientNotes` draws a focal circle outside the end
circle as a cone and gives a focal point on the end circle its own rule;
it says nothing of how near is on. Illustrator writes focal points on the
circle that its rounding leaves a hair outside (an OpenSky logo: 0.0004 out
on a radius of 210): taken as outside, the cone's later circles sweep back
over the whole end circle and paint it the last stop. Chromium, Firefox
and resvg take a focal point within 1/4096 of the radius as on the circle
(Skia's nearly-zero test) and draw the gradient; librsvg draws the last
stop. The renderer takes the 1/4096 too.

## D51. The canvas's background images are laid out in the raster

`css-backgrounds-3.md#root-background` sizes and positions the root's
background images in the root's box and paints them over the whole
canvas. The renderer's root box is the raster (D39), and with no device
scale in its API a px of the root's box is a pixel of the raster, as the
root's clip-path and mask take it (D46): a background-size of 15px is 15
pixels at any size the document is drawn at, while the content stretches
with the viewBox, as Chromium and Firefox draw an SVG image an img element
sizes; at a device scale they stretch the px instead. Real files use it
for the canvas's texture: the railroad diagram generator's grid of two
gradients (78 harvested files), a mind map's tiled PNG, a gradient behind
a logo.

## D52. The canvas is the backdrop of the root's content, outside its effects

`compositing-1.md#pagebackdrop` paints the canvas background into the root
element group, under the root's filter, clip-path, mask and opacity, and
`css-backgrounds-3.md#root-background` has the root element not paint it
again. Chromium, Firefox and resvg draw the canvas outside the root's
effects: a root of opacity .5 fades its shapes and not the white of its
background-color, and the renderer draws so. The root's content blends with
the canvas, its backdrop in the root element group: a Sketch tile
multiplies a gradient over its artboard's #A8B7C7, which Sketch shows
darkened; Chromium and Firefox isolate the root's content from the canvas
and draw the gradient lighter than the colour under it, which multiply
cannot.

## D53. Path data takes a number's trailing point

`svg2/paths.md#PathDataBNF` drops SVG 1.1's numbers with a point and no
digit after it, "23.", and lets user agents accept them where the parse is
unambiguous. Inkscape writes them in its own stock patterns, Ermine's
spots among them ("M 675.437,501. C 637.586,501. ..."), in 30 harvested
files, and Inkscape, resvg and librsvg read them; Chromium and Firefox
take each one for an error and drop the rest of the path, cutting the
ermine's tails. The renderer read the digits and failed on the point after
them, which drew the command the number ended and nothing after, as no
renderer does. It takes a point after a number of digits alone as that
number's end now, and in a number with a point or an exponent still as
the next number's start.

## D54. A pass draws and measures a bounded number of elements

SVG sets no bound on how often a document instances its content: a group
of two uses of the group before it, 40 deep, is 2^40 instances in 2 KB,
and patterns and markers of patterns and markers multiply the same way.
The renderer drew and measured them all, 22 levels taking 44 seconds.
Each pass over a document, a drawing or a measure of its size, now visits
at most 16 384 + 32 elements per byte of its source, every instance
counted, and draws or measures nothing past that; a use's box for context
paints is measured only when a context paint asks for it. The most any
file of the corpus or of 60 000 harvested ones visits is 5.2 per byte, a
Wikimedia map of uses; the 40-deep chain draws its first square and stops
in a few hundredths of a second.

## D55. A Bézier that turns back inside itself has a round stroke tip

`svg2/painting.md#StrokeShape` describes the ideal swept stroke but gives
implementations leeway at extremely tight curves. At an internal stationary
point of a cubic or quadratic whose tangent reverses, Chromium, resvg and librsvg sweep
the outer offset through a semicircle, even with `stroke-linecap: butt`.
Real Sketchy-SVG drawings use this for rounded ends on strokes that double
back, including the horizontal strokes of a chair. The renderer previously
joined the offset edges directly and made those ends flat. It now adds the
semicircle at a Bézier cusp with nearly opposed tangents. A reversal at an
explicit line corner remains flat, as all three renderers draw it.
This also corrects the old `path-direction#cusp-fold` oracle, which drew
a square tip for a reversing quadratic despite Chromium's rounded one.
