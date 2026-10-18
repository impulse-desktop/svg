<!-- https://svgwg.org/svg2-draft/changes.html, as of 2026-10-18; written by dev/spec_text.py -->

# Appendix K: Changes from SVG 1.1

*This appendix is informative, not normative.*

This appendix summarizes the changes that have been made since the [SVG 1.1 Second Edition Recommendation](../svg11/index.md). Changes made since the initial SVG 2 Candidate Recommendation are highlighted.

## <a id="editorial"></a>K.1. Editorial changes

A number of stylistic changes have been made to the specification to make it more readable. These include the following:

- A change towards using styling similar to contemporary CSS specifications, removing mention of XSL-FO.
- Annotations have been added to the specification to reflect features that the Working Group has resolved on including and for which a member of the group has committed to drive that feature.
- Changes to highlight that SVG can be used in HTML as well as in XML.
- Removed chapters whose normative content has either been moved to other specifications or deprecated: Filters, Clipping and Masking, Animation, Fonts, Color.
- Removed the (informative) Concepts and Backwards Compatibility chapters, as well as multiple appendices.
- Merged and re-organized multiple chapters:
  - Made the (normative) Conformance appendix a chapter.
  - Moved sections on 'display' and 'visibility' properties from the Painting chapter to the Rendering Model chapter.
  - Moved remaining sections from Clipping, Masking, and Compositing chapter into the Rendering Model chapter.
  - Merged the SVG 1.1 Metadata chapter into the Document Structure chapter.
  - Moved the 'image' element definition from the Document Structure chapter to the new Embedded Content chapter.
  - Moved Foreign namespaces and private data section from the SVG 1.1 Extensibility chapter to Document Structure chapter.
  - Created the Geometry Properties chapter, and moved relevant definitions to it from the Basic Shapes and Paths chapters.
  - Created the Embedded Content chapter, and merged all content from the SVG 1 Extensibility chapter into it.
  - Merged the SVG 1.1 Scripting and Interactivity chapters.

- Tidied up the changes list, adding missing changes, and adding links to issue discussion for changes since the initial Candidate Recommendation. Edits
- Consistently use the term "user coordinate system" (previously, the synonymous term "local coordinate system" was also used). Issue discussion Edits

## <a id="substantial"></a>K.2. Substantial changes

In additional to the editorial changes listed above, the following substantial additions, changes and removals have been made.

### <a id="whole"></a>K.2.1. Across the whole document

- References to the SVG DTD have been removed.
- IDL has been updated to use Web IDL syntax.
- Added "length" attribute and indexed property access to all list interfaces.
- Remove definition of [<angle>](../css-values-4.md#angles) type and reference CSS3-values.
- Change all coordinate types to <length> in preparation for referencing css3-values.
- Add the ‘[script](interact.md#ScriptElement)’ element to the content model of all elements.
- Changed the initialize, appendItem, replaceItem and insertItemBefore methods on list objects to make a copy of any list item being inserted that is already in another list.
- The ‘[image](embedded.md#ImageElement)’ and ‘[script](interact.md#ScriptElement)’ elements have gained a crossorigin attribute, for use with the *CORS-enabled fetch* algorithm.

- Use mixins over NoInterfaceObject. Issue discussion Edits

- Refer to HTML LS throughout. Issue discussion Edits

### <a id="concepts"></a>K.2.2. Concepts chapter (SVG 1.1 only)

- Removed this chapter.

### <a id="conform"></a>K.2.3. Conformance Criteria chapter (Appendix in SVG 1.1)

- Animations do not run in documents processed as resource documents.
- Made the appendix a chapter.
- Integrate content on processing modes from the SVG Integration specification.
- Define required processing modes for different types of SVG cross-references, with suggested processing modes for similar references from HTML/CSS. (Replaces the "referencing modes" section from SVG Integration.)
- Update and re-organize all the conformance classes, to be consistent with changes to other parts of this specification and changes to other specifications.
- Make clear that a [SVG viewer](conform.md#TermSVGViewer) for conformance purposes is the [user agent](conform.md#TermUserAgent) / [SVG user agent](conform.md#TermUserAgent) mentioned everywhere else in the spec; move the corresponding definitions from the Document structure chapter to here.
- Move the non-normative section on suggested methods for generating high-precision graphics to the Implementation Notes appendix.

- Add informative references to ICC and to CSS Color 4 Issue discussion Edits

### <a id="rendering"></a>K.2.4. Rendering Model chapter

- Update rendering model to refer to [Compositing and Blending Specification](../compositing-1.md).
- Added new definitions regarding rendered and renderable elements.
- Moved the section on [display](render.md#VisibilityControl) and [visibility](render.md#VisibilityControl) to this chapter.
- Moved z-index to this chapter. Removed non-normative text, keeping examples.
- Resolve issue 2 - removed reference to knock-out.
- Resolve issue 3 - rewrote description of how elements and groups are rendered.
- Resolve issue 4 - Don't mention each paint server type explicitly.
- Resolve issue 6 - No changes required.
- Merge Clipping, Masking and Compositing chapter into this chapter (resolved issue 7 in process).
- Update 'Parent Compositing' section to explicitly mention how each referencing mode composites.
- SVG root and foreignObject are no longer overflow:hidden in User Agent style sheet.
- overflow:auto may clip and show scroll bars in some cases.
- overflow:scroll may show scroll bars on svg elements.
- Removed definition of initial clipping path.
- Updated rendering requirements to reflect shadow DOM model for use elements.
- Deferred z-index support to a future version of SVG. Issue discussion Edits
- Clarify that ‘[foreignObject](embedded.md#ForeignObjectElement)’ is, and its content may be, hit-testing target.
- Removed the use element from list of elements that the [visibility](render.md#VisibilityControl) property directly affects. Issue discussion Edits

### <a id="types"></a>K.2.5. Basic Data Types and Interfaces chapter

- All appearance of SVGMatrix were replaced by DOMMatrix or DOMMatrixReadOnly.
- All appearance of SVGRect were replaced by DOMRect or DOMRectReadOnly.
- All appearance of SVGPoint were replaced by DOMPoint or DOMPointReadOnly.
- Removed the SVGStylable and SVGLangSpace interfaces and moved all of their members on to [SVGElement](types.md#InterfaceSVGElement).
- Turned SVGLocatable and SVGTransformable into [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement), which directly inherits from [SVGElement](types.md#InterfaceSVGElement).
- Added an [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement) interface.
- Added an [SVGGeometryElement](types.md#InterfaceSVGGeometryElement) interface with isPointInFill and isPointInStroke methods.
- Removed the SVGExternalResourcesRequired interface.
- Added getStrokeBBox on [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement) to get the tight stroke bounding box.
- Make viewportElement and ownerSVGElement on [SVGElement](types.md#InterfaceSVGElement) nullable.
- Removed the getPresentationAttribute operation on [SVGElement](types.md#InterfaceSVGElement) and the SVGColor and SVGICCColor interfaces.
- Added focus and blur operations and tabIndex attribute to [SVGElement](types.md#InterfaceSVGElement).
- Added [activeElement](struct.md#__svg__SVGDocument__activeElement) attribute to Document.
- Made [SVGElement](types.md#InterfaceSVGElement) include the GlobalEventHandlers interface from HTML.
- Removed getStrokeBBox from [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement) and extended [getBBox](types.md#__svg__SVGGraphicsElement__getBBox) with a dictionary argument that controls which parts of the element are included in the returned bounding box.
- Allow leading and trailing whitespace in <length>, [<angle>](../css-values-4.md#angles), <number> and <integer>.
- Make whitespace include form feed (U+000C) to align with CSS and HTML.
- Remove basic data types already defined by CSS Values and Units.
- Remove list of color keywords. The list is part of CSS Colors 3 which is referenced normatively and a REC.
- Allow a single trailing comma-wsp in various list-typed attributes.
- Removed the xmlbase, xmllang and xmlspace attributes from the [SVGElement](types.md#InterfaceSVGElement) interface.
- Removed the SVGViewSpec interface.
- Removed the style IDL attribute from [SVGElement](types.md#InterfaceSVGElement), since CSSOM defines that for us.
- Removed the getTransformToElement method on [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement).
- Defined that getCTM on an [outermost svg element](struct.md#TermOutermostSVGElement) should work, and listed which transforms it should include.
- Defined attribute reflection in more depth, including the effect it has on SVG DOM object liveness.
- animVal IDL attributes on all interfaces now alias baseVal.
- Added the dataset IDL attribute to [SVGElement](types.md#InterfaceSVGElement).
- Moved the [pathLength](types.md#__svg__SVGGeometryElement__pathLength) attribute, [getTotalLength()](types.md#__svg__SVGGeometryElement__getTotalLength), and [getPointAtLength()](types.md#__svg__SVGGeometryElement__getPointAtLength) methods from SVGPathElement onto SVGGeometryElement.
- Move definition of wsp and comma-wsp to path grammar since that's the only EBNF grammar in the spec.

- [SVGUnitTypes](types.md#InterfaceSVGUnitTypes) is no longer NoInterfaceObject. Issue discussion Edits
- Clarified that [SVGElement](types.md#InterfaceSVGElement).[className](types.md#__svg__SVGElement__className) overrides Element.className. Not normative. Issue discussion Edits
- Specify DOMPointInit as argument type for isPointInFill() and isPointInStroke() instead of DOMPoint, and specify algorithm used for isPointInFill() and isPointInStroke(). Issue discussion Edits
- Moved dataset, tabIndex, focus(), and blur() to a shared HTMLOrSVGElement mixin defined in the HTML specification. Issue discussion Edits
- Made [SVGElement](types.md#InterfaceSVGElement) include the @@ unknown term "DocumentAndElementEventHandlers" interface from HTML. Issue discussion Edits

- Merged in content from the former normative [SVG DOM appendix](#svgdom) (after edits), with sections on reflecting attributes re-arranged to fit. Issue discussion Edits

- Clarify possible UA behavior on getTotalLength, getPointAtLength, getBBox, getCTM, getScreenCTM, isPointInFill and isPointInStroke if the current element is a [non-rendered element](render.md#TermNonRenderedElement).
- Removed the text-decoration-fill and text-decoration-stroke properties.
- Setting the DOM property for an enumerated attribute to an out-of-range integer or 0 throws a TypeError. Issue discussion
- Remove `zoomAndPan` attribute and related text. Issue discussion

### <a id="structure"></a>K.2.6. Document Structure chapter

- Updated IDL for [SVGSVGElement](struct.md#InterfaceSVGSVGElement) marking the referenceElement argument in [getIntersectionList](struct.md#__svg__SVGSVGElement__getIntersectionList) and [getEnclosureList](struct.md#__svg__SVGSVGElement__getEnclosureList) as nullable.
- Deprecated the `suspendRedraw`, `unsuspendRedraw` and `unsuspendRedrawAll` methods on the SVGSVGElement interface.
- Removed the ‘externalResourcesRequired’ attribute.
- Added the ability to use 'auto' for the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) attributes on ‘[image](embedded.md#ImageElement)’.
- Lifted the restriction on ‘[use](struct.md#UseElement)’ not being able to reference an entire document.
- Added ‘[lang](struct.md#LangAttribute)’ attribute on ‘[desc](struct.md#DescElement)’ and ‘[title](struct.md#TitleElement)’ elements.
- Added section on WAI-ARIA attributes.
- Clarified that CSS transforms on an [outermost svg element](struct.md#TermOutermostSVGElement) do not affect [currentScale](struct.md#__svg__SVGSVGElement__currentScale) or [currentTranslate](struct.md#__svg__SVGSVGElement__currentTranslate). Defined the returned values on ‘[svg](struct.md#SVGElement)’ elements that are not [outermost svg element](struct.md#TermOutermostSVGElement).
- Deprecated the [rootElement](struct.md#__svg__SVGDocument__rootElement) attribute.
- Defined event handler content attributes and IDL attributes on the root ‘[svg](struct.md#SVGElement)’ element to be for events fired at the Window, like HTML's event handler content attributes and IDL attribute on `body`.
- Moved the ‘[image](embedded.md#ImageElement)’ element to the [Embedded Content chapter](embedded.md).
- Removed the `SVGElementInstance` and `SVGElementInstanceList` interfaces, and the corresponding attributes on the [SVGUseElement](struct.md#InterfaceSVGUseElement) interface.
- Changed the ‘[use](struct.md#UseElement)’ element event flow to follow the Shadow DOM spec.
- Clarified that the ‘[switch](struct.md#SwitchElement)’ element does not affect processing of ‘[script](interact.md#ScriptElement)’ elements.
- Made the ‘width’ and ‘height’ attributes on the ‘[svg](struct.md#SVGElement)’ element presentation attributes, and changed the initial value to auto.
- Removed the baseProfile and version attributes from the ‘[svg](struct.md#SVGElement)’ element.
- Deprecated the [forceRedraw](struct.md#__svg__SVGSVGElement__forceRedraw) method on the [SVGSVGElement](struct.md#InterfaceSVGSVGElement) interface and defined it to have no effect when called.
- Defined the [deselectAll](struct.md#__svg__SVGSVGElement__deselectAll) method on the [SVGSVGElement](struct.md#InterfaceSVGSVGElement) interface in terms of the Selection API and simultaneously deprecated it.
- Clarified that the ‘[switch](struct.md#SwitchElement)’ element does not affect the processing of ‘[style](styling.md#StyleElement)’ elements.
- Clarified that the [getIntersectionList](struct.md#__svg__SVGSVGElement__getIntersectionList) and [getEnclosureList](struct.md#__svg__SVGSVGElement__getEnclosureList) methods on the [SVGSVGElement](struct.md#InterfaceSVGSVGElement) interface do not return shadow tree elements.
- Removed the requiredFeatures attribute.
- Removed the currentView and useCurrentView properties on [SVGSVGElement](struct.md#InterfaceSVGSVGElement).
- Added SVGUnknownElement for handling unknown elements in the svg namespace.
- Removed SVGUnknownElement due to lack of implementations.
- Added the ‘[lang](struct.md#LangAttribute)’ attribute in no namespace.
- Removed the `viewport` attribute from the [SVGSVGElement](struct.md#InterfaceSVGSVGElement) interface.
- Removed the xml:base attribute.
- Moved the ‘discard’ element to the split-out SVG Animations module.
- Allowed [custom data attributes](struct.md#DataAttributes) on all SVG elements.
- Rewrite ‘[desc](struct.md#DescElement)’ and ‘[title](struct.md#TitleElement)’ description to reflect ARIA and accessibility mapping.
- Remove recommendation on how to structure metadata elements within the SVG document.
- Remove requirement for SVGDocument property on the Document interface object.
- Descendent elements of ‘[switch](struct.md#SwitchElement)’ that have the ‘[systemLanguage](struct.md#SystemLanguageAttribute)’ attribute should be reordered according to the definition of the SMIL 'allowReorder' attribute.
- Add ‘[tspan](text.md#TextElement)’ and ‘[textPath](text.md#TextPathElement)’ to the [graphics element](struct.md#TermGraphicsElement) category. This allows filters, masks, etc to be applied to them.
- Update section on element roles to conform to latest SVG Accessibility API Mappings specification.
- Soften recommendation to put pre-defined content in a defs element even if not required.
- Change display behavior of defs, title, desc, metadata, and symbol elements to be defined using user agent styles.
- Allow x, y, width, and height presentation attributes on a symbol, with the same effect on a rendered instance as on a nested svg element.
- Specify a backwards-compatible default behavior for ‘[refX](struct.md#SymbolElementRefXAttribute)’ and ‘[refY](struct.md#SymbolElementRefYAttribute)’ on a symbol (which will be different from marker); clarify that keywords have same behavior as marker.
- Re-define use-element shadow trees to be consistent with the shadow DOM spec, to integrate better with other SVG 2 changes, and to clarify much related behavior, including:
  - x/y/width/height are presentation attributes, albeit ones with a unique effect on final layout
  - zero values for width and height are only significant through their impact on the used value of a cloned svg/symbol; negative values are parse errors
  - Allow use elements to reference any element that can be a valid child of an SVG container, rather than only graphics elements
  - Define the SVGUseElementShadowRoot interface as an extension of the ShadowRoot interface, and require it to be used for use-element shadow trees
  - Define a number of terms related to shadow DOM and use elements, referencing the DOM standard where applicable
  - Clarify that use-element shadow trees are generated even if the element is in a conditional processing failed branch (consistent with previous guidance re display:none)
  - Element instances in the shadow tree must appear to be regular Element nodes, except they are read-only.
  - Properties formerly defined on the SVGElementInstance object are now defined as a mixin interface extending SVGElement
  - Clarify that URL references in cloned content are made absolute relative to the source file.
  - Clarify that the shadow tree is discarded if the cross-references change, including due to declarative animation (the original and animated shadow trees are not preserved in parallel).
  - Clarify that circular references only block the rendering of the use-element that would connect the circle, and not it's own host.
  - Added warning about x/y behaving as a transformation on the use element, and the impact on userSpaceOnUse graphical effects.
  - Redefine the way width/height on use elements impact svg/symbol to be consistent with geometry properties and auto values.
  - Clarify that for other re-used graphics (not svg/symbol), the shadow elements are laid out in the use element's coordinate system, including for the resolution of percentage lengths.
  - Redefine style cloning in shadow trees to be consistent with style scoping in shadow DOM, including by requiring style sheets to be cloned into the shadow DOM's scope.
  - Add warning and example regarding the cases where this results in different styles than an SVG 1.1-conforming user agent.
  - Add example regarding the inherited behavior of the visibility property.
  - Require multimedia in use-element copies to be synchronized with all other instances of the same original element.
  - Clarify that the shadow tree shares the document timeline for animations with its host document.
  - Define a ShadowAnimation interface to represent the cloning of Web Animations API animations
  - Prohibit WAAPI animations from being directly applied to element instances in the use-element shadow tree.
  - Define the propagation of SMIL-style animations through the cloning of animation elements; require that animations elements affecting the referenced graphic be cloned into the shadow tree, even if they are not descendents of the referenced element.
  - Define how event-based animation element triggers and animation element href attributes behave when there are multiple elements with the same ‘[id](struct.md#IDAttribute)’ in different node trees.
  - Require event handling in use-element shadow trees to follow the event retargetting rules from the Shadow DOM spec.
  - Clarify that the copying of event listeners from referenced graphics to their element instances applies to listeners added by script as well as by event attributes.
  - Prohibit event listeners from being directly added to elements in the use-element shadow tree.
  - Make use elements and symbols map to the graphics-object role by default, so the shadow content will be accessible.
  - Make the SVGSymbolElement interface extend from SVGGraphicsElement, so that rendered symbol element instances have all the behavior of graphics elements (e.g., getBBox)
  - Restore the (animated)instanceRoot properties on the SVGUseElement interface.
- Change role mapping for the ‘[a](linking.md#AElement)’ element to depend on whether it is actually a valid link.
- Mark ARIA state and property attributes as animatable.
- Update requirements for ‘[id](struct.md#IDAttribute)’ values to harmonize with HTML, with extra warning about requirements for validity in an XML document.
- Moved the [Foreign namespaces and private data](struct.md#ForeignNamespaces) section to this chapter and added some new cross-references and notes.

- Corrected incorrect interface name for definition of [SVGUseElementShadowRoot](struct.md#InterfaceSVGUseElementShadowRoot). Issue discussion Edits

- Make it clear that language-tag matching for ‘[systemLanguage](struct.md#SystemLanguageAttribute)’ is case-insensitive, as required by BCP 47. Issue discussion Edits
- Added the ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’ attribute in no namespace.

- Remove `zoomAndPan` attribute and related text. Issue discussion

### <a id="styling"></a>K.2.7. Styling chapter

- Removed the ‘contentStyleType’ attribute.
- [SVGStyleElement](styling.md#InterfaceSVGStyleElement) now implements LinkStyle.
- Updated user agent style sheet - inner svg elements and foreignObject elements are no longer set as overflow:hidden.
- Added hatch to list of elements with overflow:hidden in User Agent style sheet.
- Set the default value of transform-origin to '0 0' for all elements except root svg elements and svg elements that are the child of a foreignObject element.
- Substantially rewrote the chapter to be more concise.
- Defined that HTML ‘link’ elements must load external style sheets.
- Specified the exact set of properties that have presentation attributes, their names (when the presentation attribute name does not match the property name), and on which elements they are allowed.
- Added a list of properties defined in other specifications that conforming SVG 2 user agents must support.
- Added User Agent style sheet rule to map the deprecated ‘xml:space’ attribute to the white-space property.
- Required that @font-face rules and ::first-letter & ::first-line pseudo-elements on ‘[text](text.md#TextElement)’ elements must be supported.
- Required that all SVG and HTML style sheets in an HTML document with inline SVG content must apply to all content in the document.
- Allow presentation attributes on any svg namespaced element.
- Change display behavior of style element to be defined using user agent styles.
- Add !important user agent style rules controlling never-rendered elements, with a note explaining their impact.
- Require interactive user agents to include :focus and ::selection styles.
- Clean-up presentation attributes table to be consistent with other sections of the spec: remove ‘[mask](../css-masking-1.md#MaskElement)’ from elements that treat x/y/width/height attributes as presentation attributes, add ‘[use](struct.md#UseElement)’ and ‘[symbol](struct.md#SymbolElement)’; exclude animation elements from those that treat [fill](painting.md#FillProperty) as a presentation attribute; remove [d](paths.md#DProperty) as a presentation attribute for ‘[textPath](text.md#TextPathElement)’; merge the discussion of gradientTransform and patternTransform into the main table.
- Removed requirement for [clip](../css-masking-1.md#propdef-clip) property support.

- Specify that [transform](coords.md#TransformProperty), ‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’ and ‘[gradientTransform](pservers.md#LinearGradientElementGradientTransformAttribute)’ are presentation attributes for [transform](coords.md#TransformProperty) that can be specified to certain sets of elements in the SVG namespace. Issue discussion Edits

- Add a user-agent stylesheet requirement for a pointer cursor on links, consistent with HTML and CSS. Issue discussion Edits

### <a id="geometry"></a>K.2.8. Geometry Properties chapter (SVG 2 only)

- Removed ‘[pattern](pservers.md#PatternElement)’ and ‘[filter](../filter-effects-1.md#FilterElement)’ from the set of elements that the [x](geometry.md#XProperty) and [y](geometry.md#YProperty) properties apply to.
- Defined that [width](geometry.md#Sizing) and [height](geometry.md#Sizing) property values of auto are computed to 0 for SVG elements that the properties apply to.
- auto equates to 100% for width and height on the ‘[svg](struct.md#SVGElement)’ element, and to auto-sizing for images.
- Remove ‘[mask](../css-masking-1.md#MaskElement)’ from the list of elements where width and height are sizing properties.
- Clarify that width and height sizing properties can be constrained with the corresponding max/min properties.

### <a id="coords"></a>K.2.9. Coordinate Systems, Transformations and Units chapter

- Make SVGMatrix.skew{X,Y} throw an exception on bad values.
- Added improved wording on bounding box from SVG Tiny 1.2, and an algorithm which can compute a bounding box for an element.
- Define bounding box for element with no position to be at (0,0).
- Removed the defer keyword from ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’.
- Add definition for 'current viewport'. Change SVG viewport to mean all viewports created by SVG elements.
- Add non-scaling-size, non-rotation, and fixed-position values for [vector-effect](coords.md#VectorEffectProperty)
- Changed requirement for co-ordinate system layout to be mandatory.
- [vector-effect](coords.md#VectorEffectProperty) has no effect within a [3d rendering context](../css-transforms-2.md#3d-rendering-contexts).
- Define SVG sizing in CSS contexts with reference to the CSS Default Sizing Algorithm.
- Clarify that percentages are relative to the width and height of the specified viewBox.
- Mark unimplemented [vector-effect](coords.md#VectorEffectProperty) options at-risk.
- Adapt [vector-effect](coords.md#VectorEffectProperty) host coordinate space definition to actual implemented behavior. Remove screen and viewport values.
- Change bounding box algorithm to include the effect of overflow and clip properties when the "clipped" flag is set.

- Use DOMMatrix2DInit as parameters for methods
- Setting the enumerated parts of an [SVGPreserveAspectRatio](coords.md#InterfaceSVGPreserveAspectRatio) object to an out-of-range integer or 0 throws a TypeError. Issue discussion

### <a id="paths"></a>K.2.10. Paths chapter

- Define ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’ and ‘[polyline](shapes.md#PolylineElement)’ elements with no data set (empty or zero valid commands) to not render.
- Removed the `SVGPathSeg*` and `SVGAnimatedPathData` interfaces and the related methods on [SVGPathElement](paths.md#InterfaceSVGPathElement).
- Promoted the ‘d’ attribute to a property.
- Removed the [pathLength](types.md#__svg__SVGGeometryElement__pathLength) attribute, [getTotalLength()](types.md#__svg__SVGGeometryElement__getTotalLength), and [getPointAtLength()](types.md#__svg__SVGGeometryElement__getPointAtLength) methods from SVGPathElement, they are now on SVGGeometryElement.
- Clarified that a value of zero for ‘[pathLength](paths.md#PathLengthAttribute)’ is valid.

- Clarified that percentage distances are not affected by zero ‘[pathLength](paths.md#PathLengthAttribute)’. Issue discussion Edits
- Reorganized the Implementation Notes section into logical subsections (including the section on out-of-bounds arc parameters, which was formerly in the Implementation Notes appendix). Edits

### <a id="shapes"></a>K.2.11. Basic Shapes chapter

- The [animatedPoints](shapes.md#__svg__SVGAnimatedPoints__animatedPoints) IDL attributes of SVGAnimatedPoints now aliases the [points](shapes.md#__svg__SVGAnimatedPoints__points) attribute.
- Added the ‘[pathLength](paths.md#PathLengthAttribute)’ attribute to all basic shapes.
- Added auto behavior for [rx](geometry.md#RxProperty) and [ry](geometry.md#RyProperty) to ‘[ellipse](shapes.md#EllipseElement)’.

### <a id="text"></a>K.2.12. Text chapter

- Require support for WOFF fonts.
- Adopt the CSS3 changes to the [text-decoration](text.md#TextDecorationProperties) property.
- Added a number of missing attributes to the element summary boxes of the ‘[text](text.md#TextElement)’ and ‘[textPath](text.md#TextPathElement)’ elements.
- Added [inline-size](text.md#InlineSizeProperty) presentation attribute to ‘[text](text.md#TextElement)’.
- Added a section about [text-overflow](text.md#TextOverflowProperty) processing.
- Added [shape-inside](text.md#TextShapeInside) property.
- Added [shape-subtract](text.md#TextShapeSubtract) property (replaces [shape-outside](../css-shapes-1.md#shape-outside-property)).
- Added white-space property and deprecated ‘[xml:space](struct.md#XMLSpaceAttribute)’ attribute.
- Removed the ‘kerning’ property.
- Added a ‘path’ attribute to ‘[textPath](text.md#TextPathElement)’.
- Added ability to reference basic shapes to ‘[textPath](text.md#TextPathElement)’.
- Added the ‘[side](text.md#TextPathElementSideAttribute)’ attribute to allow placing text on either side of a path.
- Render characters for one loop of a single closed path, effected by the ‘[startOffset](text.md#TextPathElementStartOffsetAttribute)’ attribute and [text-anchor](text.md#TextAnchorProperty) property.
- Removed the 'tref' element.
- Removed the 'altGlyph', 'altGlyphDef', 'altGlyphItem' and 'glyphRef' elements.
- Defined the [selectSubString](text.md#__svg__SVGTextContentElement__selectSubString) method on the [SVGTextContentElement](text.md#InterfaceSVGTextContentElement) interface in terms of the Selection API and simultaneously deprecated it.
- Changed getComputedTextLength() to not include ‘[dx](text.md#TextElementDXAttribute)’ and ‘[dy](text.md#TextElementDYAttribute)’ values.
- Moved the discussion of [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties to the Geometry Properties chapter.
- Require support for ::selection in interactive user agents.
- Require that text in non-rendered elements are not included in [addressable characters](text.md#TermAddressableCharacter) and therefore are not counted when assigning positioning attributes and in the [getNumberOfChars](text.md#__svg__SVGTextContentElement__getNumberOfChars) methods.
- Require that Unknown elements within text render as unpositioned spans, but that known-but-invalidly-positioned elements do not.
- Clarify that when text is positioned along a transformed path, offset distances should be measured in the text element's coordinate system, not the path element's.

- Specify DOMPointInit as argument type for getCharNumAtPosition() instead of DOMPoint. Issue discussion Edits
- Remove 'number' value from [inline-size](text.md#InlineSizeProperty) property for consistency with CSS. Issue discussion Edits
- Removed unintended restriction that ‘[pathLength](paths.md#PathLengthAttribute)’ applied only to paths and not [shapes](shapes.md#TermShapeElement) when computing ‘[startOffset](text.md#TextPathElementStartOffsetAttribute)’. Issue discussion Edits

- Clarify the initial value for ‘[textLength](text.md#TextElementTextLengthAttribute)’, and add note about its impact on the reflected IDL attribute.

- Reverted to SVG 1.1 definition of addressable character for the purposes of text positioning attributes (e.g. counting by Unicode code points).

### <a id="embedded"></a>K.2.13. Embedded Content chapter (SVG 2 only)

- Removed HTML multimedia section and the use of the audio, video, iframe, canvas elements due to lack of implementations.
- Added this chapter, which includes new 'video', 'audio', 'iframe', 'canvas', 'source' and track' elements that behave almost identically to the equivalent HTML elements.
- Removed the SVG copies of HTML's embedded content elements and instead allowed those elements to be used in the HTML namespace within SVG container elements.
- Removed the requirement to ignore [clip](../css-masking-1.md#propdef-clip) and [overflow](render.md#OverflowAndClipProperties) on an SVG document when referenced by an ‘[image](embedded.md#ImageElement)’ element.
- Allow image height and width to be "auto", determined from intrinsic dimensions or aspect ratio of the image file.
- Clarify image and foreignObject sizing to be consistent with CSS and with geometry properties; allow embedded HTML elements to be positioned using [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing), and [height](geometry.md#Sizing) geometric properties.
- Clarify that object-fit and object-position properties apply to embedded HTML elements; add warning note that they are not defined for ‘[image](embedded.md#ImageElement)’.
- Moved the "Foreign namespaces and private data" section to the Document Structure chapter.
- Updated the reference to the (abandoned) Resource Priorities spec to instead refer to the (working draft) Resource Hints spec.
- Define unified layout model for all embedded elements, including behavior of object-fit and object-position properties for ‘[image](embedded.md#ImageElement)’.

### <a id="painting"></a>K.2.14. Painting chapter

- Added more detail to the descriptions of the stroke properties and added algorithms defining the exact shape a stroke must have.
- Added the [paint-order](painting.md#PaintOrderProperty) property.
- Moved the color-interpolation-filters property to the Filter Effects specification.
- Added the context-fill and context-stroke paint values.
- Allowed [<paint>](painting.md#SpecifyingPaint) values and marker properties to take the child keyword and <child-selector> values to support ID-less referencing.
- Move normative definition of [vector-effect](coords.md#VectorEffectProperty) property non-scaling into SVG 2.
- Added arcs value on [stroke-linejoin](painting.md#StrokeLinejoinProperty).
- Added the 'auto-start-reverse' attribute to the ‘[orient](painting.md#OrientAttribute)’ attribute on ‘[marker](painting.md#MarkerElement)’.
- Removed the SVGPaint interface.
- Added the 'z-index' property. (Later removed.)
- Split out some new marker and stroke related features into the SVG Markers and SVG Strokes specifications.
- Removed <child-selector> paint values.
- Removed the 'buffered-rendering' property and replaced it with a short discussion of the 'will-change' property.
- Added a child(<integer>) paint value to support the use cases that <child-selector> was intended for.
- Clarify that for all zero length subpaths, whether open or closed, the rendered result is based on the [stroke-linecap](painting.md#StrokeLinecapProperty) attribute.
- Allow markers on all [shapes](shapes.md#TermShapeElement).
- Clarify the description of context-stroke and context-fill, particularly re use element shadow content.
- Change display behavior of marker elements to be defined using user agent styles.
- [Since last WD] Roll back the change allowing [fill](painting.md#TermFill) and [stroke](painting.md#TermStroke) to take multiple paints.
- Remove the 'markable elements' category.
  - Remove child, child(<integer>) as they are deferred to the next level of the spec. Edits and discussion
  - Make any non-negative number a valid value for [stroke-miterlimit](painting.md#StrokeMiterlimitProperty). Issue discussion Edits
  - Remove the color-rendering CSS property. Issue discussion

### <a id="color"></a>K.2.15. Color chapter (SVG 1.1 only)

- Removed this chapter, replaced with normative reference to CSS Color.

### <a id="pservers"></a>K.2.16. Paint Servers chapter (called Gradients and Patterns in SVG 1.1)

- Added the solidcolor element and its two properties solid-color and solid-opacity, ported over from SVG Tiny 1.2. (Renamed 'solidColor' to 'solidcolor'.)
- Added an ‘[fr](pservers.md#RadialGradientElementFRAttribute)’ attribute to the ‘[radialGradient](pservers.md#RadialGradientElement)’ element, which allows specifying the radius of the focal circle.
- Added a section for mesh gradients, defining the mesh, meshrow and meshpatch elements. (Renamed 'meshGradient' to 'mesh', 'meshRow' to 'meshrow', and 'meshPatch' to 'meshpatch'.)
- Added a section for hatches, defining the hatch and hatchpath elements. (Renamed 'hatchPath' to 'hatchpath'.)
- Renamed ‘gradientTransform’ on the 'mesh' element and ‘hatchTransform’ on the 'hatch' element to ‘transform’.
- Change display behavior of paint server elements to be defined using user agent styles.
- Define common handling of paint server template cross-references, allowing external file references, using a use-element shadow DOM model for cloned child content, defining consistent behavior for whether title and desc affect child cloning, and explicitly defining which attributes are derived from the template for each element.
- Add notes warning about attributes that look like presentation attributes, but aren't.

- [SVGGradientElement](pservers.md#InterfaceSVGGradientElement) and [SVGPatternElement](pservers.md#InterfaceSVGPatternElement) no longer implement [SVGUnitTypes](types.md#InterfaceSVGUnitTypes). Issue discussion Edits
- Deferred hatch and hatchpath elements to a future version of SVG. Issue discussion Edits
- Deferred mesh gradients, including mesh, meshrow, and meshpath elements to a future version of SVG. Issue discussion Edits
- Removed the solidcolor element and the solid-color and solid-opacity properties. Issue discussion Edits

### <a id="masking"></a>K.2.17. Clipping, Masking and Compositing chapter (SVG 1.1 only)

- SVG now references CSS Masking [[css-masking-1](refs.md#ref-css-masking-1)] specification.
  - Removed definition of clipping/masking properties and elements.
  - Keep short introduction to clipping/masking and add reference to CSS Masking
- Added reference to Compositing and Blending spec. Simple alpha compositing is mandatory.
- The [overflow](render.md#OverflowAndClipProperties) property should be respected on the outermost svg elements inline in html.
- Merge Clipping, Masking and Compositing chapter into Rendering Model chapter.

### <a id="filters"></a>K.2.18. Filter Effects chapter (SVG 1.1 only)

- Removed this chapter (replaced by Filter Effects specification [[filter-effects-1](refs.md#ref-filter-effects-1)]).

### <a id="interact"></a>K.2.19. Scripting and Interactivity chapter (separate chapters in SVG 1.1)

- Added the ‘tabindex’ attribute.
- Corrected the definition of the 'cursor/href' attribute on the 'cursor element' element to take an URL that is not in a CSS-like functional form.
- Added the bounding-box keyword to [pointer-events](interact.md#PointerEventsProperty).
- Replaced SVGLoad, SVGAbort, SVGError and SVGUnload with load, abort, error and unload respectively.
- Required that only [structurally external elements](struct.md#TermStructurallyExternalElement) and the [outermost svg element](struct.md#TermOutermostSVGElement) must fire load events.
- Replaced SVGResize and SVGScroll with resize and scroll respectively.
- Removed DOMActivate.
- Replaced DOMFocusIn and DOMFocusOut with focusin and focusout, and added the blur event.
- Added keyboard events.
- Removed mutation events.
- Removed SVGZoomEvent.
- Clarify that shadow DOM content is focusable and interactive.
- Change display behavior of script elements to be defined using user agent styles.
- Mark cursor element as deprecated, update description of cursor.

- Removed the cursor element and the SVG specific definition of the cursor Issue discussion Edits

- Merged in content on events from the removed SVG DOM appendix. Issue discussion Edits
- Added the auto keyword to [pointer-events](interact.md#PointerEventsProperty).

- Remove requirements on [pointer-events](interact.md#PointerEventsProperty) for raster images to check individual pixels for transparency.
- Remove `zoomAndPan` attribute and related text.
- Removed the onfocusin and onfocusout event attributes.

### <a id="linking"></a>K.2.20. Linking chapter

- Added animation elements to the content model of ‘[view](linking.md#ViewElement)’.
- Added the possibility to link to SVG resources with fragment identifiers defined by the Media Fragments specification.
- Removed the ‘xlink:type’, ‘xlink:role’, ‘xlink:arcrole’, ‘xlink:show’ and ‘xlink:actuate’ attributes.
- Deprecated the ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute in favor of using ‘href’ without a namespace.
- Deprecated the ‘[xlink:title](linking.md#XLinkTitleAttribute)’ attribute in favor of using child ‘[title](struct.md#TitleElement)’ elements.
- Allowed spaces in SVG view fragments.
- Defined how "pixel:" and "percent:" spatial media fragments are interpreted.
- Made linking to ‘[view](linking.md#ViewElement)’ elements not cause an implicit view box transformation to show the nearest ancestor ‘[svg](struct.md#SVGElement)’ element.
- Clarified that unspecified SVG view fragment parameters do not cause the corresponding attributes to be reset to their initial values.
- Removed the ‘viewTarget’ attribute on the ‘[view](linking.md#ViewElement)’ element and the corresponding SVG view fragment parameter.
- Fragment-only URLs are always same-document.
- Added additional attributes on ‘[a](linking.md#AElement)’ element to synchronize with HTML.
- Create detailed instructions for processing URLs and external resource files.

- Harmonized attributes and IDL properties on ‘[a](linking.md#AElement)’ element with HTML, other than ‘[href](linking.md#AElementHrefAttribute)’ and ‘[target](linking.md#AElementTargetAttribute)’ (which are maintained as-is for legacy reasons). Specifically:
  - Added the ‘[ping](linking.md#AElementPingAttribute)’ and ‘[referrerpolicy](linking.md#AElementReferrerpolicyAttribute)’ attributes and their matching IDL properties.
  - Added the `text` IDL property.
  - Specified the meaning of a non-empty value for ‘[download](linking.md#AElementDownloadAttribute)’.
  - Made all attributes except ‘[href](linking.md#AElementHrefAttribute)’ and ‘[target](linking.md#AElementTargetAttribute)’ non-animatable, with a simple read/write DOMString representation in the IDL.
Issue discussion Edits

### <a id="script"></a>K.2.21. Scripting chapter (in SVG 1.1)

- Removed the ‘contentScriptType’ attribute.
- Removed ‘onload’ from AnimationEvents.
- Merged this entire chapter into the Interactivity chapter.

### <a id="animate"></a>K.2.22. Animation chapter (SVG 1.1 only)

- Moved this chapter to a separate SVG Animations module.
- Some informative text moved to a new Animation appendix.

### <a id="fonts"></a>K.2.23. Fonts chapter (SVG 1.1 only)

- Added WOFF requirement, rearranged content.
- Removed this chapter and the SVG Fonts feature it contained, including the ‘font’, ‘glyph’, ‘missing-glyph’, ‘hkern’, ‘vkern’, ‘font-face’, ‘font-face-src’, ‘font-face-uri’, ‘font-face-format’ and ‘font-face-name’ elements and their corresponding IDL interfaces. (Remaining content merged into the Text chapter.)

### <a id="metadata"></a>K.2.24. Metadata chapter (SVG 1.1 only)

- Merged the metadata chapter with the document structure chapter.

### <a id="backward"></a>K.2.25. Backwards Compatibility chapter (SVG 1.1 only)

- Removed this chapter (which had been effectively informative).

### <a id="extend"></a>K.2.26. Extensibility chapter (SVG 1.1 only)

- Made ‘[foreignObject](embedded.md#ForeignObjectElement)’ a [graphics element](struct.md#TermGraphicsElement).
- Made ‘width’ and ‘height’ presentation attributes on the ‘[foreignObject](embedded.md#ForeignObjectElement)’ element.
- Merge this chapter into the new [Embedded Content chapter](embedded.md) (and then later moved the "Foreign namespaces and private data" section to the Document Structure chapter.)

### <a id="svgdtd"></a>K.2.27. Document Type Definition appendix (SVG 1.1 only)

- Removed this appendix, as SVG 2 is not defined in terms of a DTD.

### <a id="svgdom"></a>K.2.28. SVG Document Object Model (DOM)(SVG 1.1 Only)

- Update core DOM support requirement to DOM4, DOM Level 2 Views, UI Events, and DOM Level 2 Style/CSS.
- 'SVGException' interface is removed.
- Section on (deprecated) 'hasFeature' method removed.
- Non-SVG specific text about event and error handling removed.
- Interfaces and methods that relied on the 'CSSValue' type removed (they were already deprecated in SVG 1.1).

- Dependencies on other DOM specifications updated as follows:
  - Make DOM 4 a minimum requirement, with DOM living standard a "should".
  - Remove requirement for support of obsolete DOM View spec.
  - Update the requirement on DOM Level 2 Style to only include interfaces also defined in CSSOM, with full support for CSSOM as a "should". (And remove the "Relationship with DOM Level 2 CSS" section.)
  - Add Geometry Interfaces Module Level 1 as a requirement.
  - Add Clipboard API and events as a normative requirement, complementary to the UI Events requirement.
- Clarify that SVG DOM requirements extend to elements defined in other specifications.
- Add a requirement for handling unknown elements in the SVG namespace: "must" handle as browsers currently do (as SVGElement instances); "should" implement SVGUnknownElement interface (though that is at risk).
- Remove the special rule for initializing [textLength](text.md#__svg__SVGTextContentElement__textLength), in favor of a clearer definition of the attribute's dynamic initial value.
- Clarify that rules about initializing objects only apply when reflecting `(none)` initial values, and also apply to interfaces defined in other specifications (e.g., Geometry).
- Update section "Relationship with UI Events" to reflect that most event handler attributes are available on all elements, that clipboard API events are available. Wording is updated to clarify normative requirements on software.
- Merge the entire content of this appendix into the Basic Data Types chapter, except for the section "Relationship with UI Events", which is merged into the Interactivity chapter.

Issue discussion Edits

### <a id="idl"></a>K.2.29. IDL Definitions appendix

- Converted to the standard WebIDL format.
- Other changes as defined in the relevant chapters.

### <a id="java"></a>K.2.30. Java Language Binding appendix (SVG 1.1 only)

- Removed this appendix, as language bindings are now defined by Web IDL.

### <a id="escript"></a>K.2.31. ECMAScript Language Binding appendix (SVG 1.1 only)

- Removed this appendix, as language bindings are now defined by Web IDL.

### <a id="impreqs"></a>K.2.32. Implementation Notes appendix (was Implementation Requirements in SVG 1.1)

- Removed un-implemented out-of-range flag values note. Issue discussion Edits

- Moved normative sections into chapters, and renamed appendix to Implementation Notes:
  - Error Processing is now in Conformance Criteria.
  - Clamping Values is now in Data Types and Interfaces, and sections re colors and opacity have been removed (clamping behavior of these properties is defined in CSS Color).
  - The normative parts of the Elliptical Arc Implementation Notes are now in the Paths chapter, with other implementation requirements for paths
  - Text selection implementation notes is now in Text, with the main section on Text Selection.
  - Printing Implementation Notes is now in Conformance Criteria.
Issue discussion Edits

### <a id="access"></a>K.2.33. Accessibility Support appendix

- Completely re-written and updated.

### <a id="i18n"></a>K.2.34. Internationalization Support appendix (SVG 1.1 only)

- Removed this appendix.

### <a id="minimize"></a>K.2.35. Minimizing SVG File Sizes appendix (SVG 1.1 only)

- Removed this appendix.

### <a id="animate-appendix"></a>K.2.36. Animating SVG Documents appendix (SVG 2 only)

- Added this appendix.

### <a id="refs"></a>K.2.37. References appendix

- Removed normative reference to CSS2.
- Removed informative reference to DOM Level 3 Core.
- Changed normative reference to DOM Level 1 Core to be informative.
- Changed normative reference from DOM Level 2 Core to DOM4.
- Changed informative reference to SMIL 3 to be normative.
- Added normative references to CSS3 Color, CSS3 Fonts, CSS3 Line Box, CSS3 Transforms, CSS3 UI, CSS3 Values and Units, CSS4 Images, Filter Effects, WOFF, Web IDL.
- Added informative references to CSS 3 Transitions and Animations.
- Replaced references from DOM Level 2 Events and DOM Level 3 Events to normative references to UI Events and DOM 4.
- Added normative reference to WAI-ARIA 1.1.
- Added normative reference to Shadow DOM.

- Removed normative reference to DOM Level 2 Views.
- Added normative reference to Geometry Interfaces.
- Added normative reference to CSSOM.
- Added normative reference to Clipboard API and events.

### <a id="other-appendix"></a>K.2.38. Element, Attribute, and Property index appendices

- Changes as described in the relevant chapters.
- Mouse wheel event attribute is named "onwheel".

### <a id="idlindex"></a>K.2.39. IDL Index appendix (SVG 2 only)

- Added this appendix.

### <a id="feature"></a>K.2.40. Feature Strings (SVG 1.1 only)

- Removed this appendix.
