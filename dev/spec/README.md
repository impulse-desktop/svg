# Specifications

SVG 2 and SVG 1.1 by chapter, and the modules SVG content draws with, as
Markdown to read and search offline. Written by `dev/spec_text.py` from the
editors' drafts (SVG 1.1 from its Second Edition), each file headed by the
page it came from and the day. Every heading, definition and other target
keeps its id as an anchor, `<a id="..."></a>`, and a link into any of these
specs leads to the local file and anchor; links elsewhere stay on the web.
Figures are left out, their alternative text kept.

Searching:

- where something is defined: `rg -n 'id="propdef-stroke-linejoin"' dev/spec`,
  `rg -n 'id="elementdef-fegaussianblur"' dev/spec`, or by the visible word,
  `rg -n -i 'stroke shape' dev/spec/svg2`;
- what refers to a definition: `rg -n '#propdef-mask-type' dev/spec`;
- a property's table: `rg -n -A12 'id="propdef-paint-order"' dev/spec`.


## SVG 2

- [Scalable Vector Graphics (SVG) 2](svg2/index.md) (https://svgwg.org/svg2-draft/)
- [Chapter 1: Introduction](svg2/intro.md) (https://svgwg.org/svg2-draft/intro.html)
- [Chapter 2: Conformance Criteria](svg2/conform.md) (https://svgwg.org/svg2-draft/conform.html)
- [Chapter 3: Rendering Model](svg2/render.md) (https://svgwg.org/svg2-draft/render.html)
- [Chapter 4: Basic Data Types and Interfaces](svg2/types.md) (https://svgwg.org/svg2-draft/types.html)
- [Chapter 5: Document Structure](svg2/struct.md) (https://svgwg.org/svg2-draft/struct.html)
- [Chapter 6: Styling](svg2/styling.md) (https://svgwg.org/svg2-draft/styling.html)
- [Chapter 7: Geometry Properties](svg2/geometry.md) (https://svgwg.org/svg2-draft/geometry.html)
- [Chapter 8: Coordinate Systems, Transformations and Units](svg2/coords.md) (https://svgwg.org/svg2-draft/coords.html)
- [Chapter 9: Paths](svg2/paths.md) (https://svgwg.org/svg2-draft/paths.html)
- [Chapter 10: Basic Shapes](svg2/shapes.md) (https://svgwg.org/svg2-draft/shapes.html)
- [Chapter 11: Text](svg2/text.md) (https://svgwg.org/svg2-draft/text.html)
- [Chapter 12: Embedded Content](svg2/embedded.md) (https://svgwg.org/svg2-draft/embedded.html)
- [Chapter 13: Painting: Filling, Stroking and Marker Symbols](svg2/painting.md) (https://svgwg.org/svg2-draft/painting.html)
- [Chapter 14: Paint Servers: Gradients and Patterns](svg2/pservers.md) (https://svgwg.org/svg2-draft/pservers.html)
- [Chapter 15: Scripting and Interactivity](svg2/interact.md) (https://svgwg.org/svg2-draft/interact.html)
- [Chapter 16: Linking](svg2/linking.md) (https://svgwg.org/svg2-draft/linking.html)
- [Appendix D: Animating SVG Documents](svg2/animate.md) (https://svgwg.org/svg2-draft/animate.html)
- [Appendix C: Accessibility Support](svg2/access.md) (https://svgwg.org/svg2-draft/access.html)
- [Appendix B: Implementation Notes](svg2/implnote.md) (https://svgwg.org/svg2-draft/implnote.html)
- [Appendix K: Changes from SVG 1.1](svg2/changes.md) (https://svgwg.org/svg2-draft/changes.html)
- [Appendix E: References](svg2/refs.md) (https://svgwg.org/svg2-draft/refs.html)
- [Appendix F: Element Index](svg2/eltindex.md) (https://svgwg.org/svg2-draft/eltindex.html)
- [Appendix G: Attribute Index](svg2/attindex.md) (https://svgwg.org/svg2-draft/attindex.html)
- [Appendix H: Property Index](svg2/propidx.md) (https://svgwg.org/svg2-draft/propidx.html)
- [Appendix J: Media Type Registration for image/svg+xml](svg2/mimereg.md) (https://svgwg.org/svg2-draft/mimereg.html)

## SVG 1.1 Second Edition

- [Scalable Vector Graphics (SVG) 1.1 (Second Edition)](svg11/index.md) (https://www.w3.org/TR/SVG11/)
- [1 Introduction](svg11/intro.md) (https://www.w3.org/TR/SVG11/intro.html)
- [2 Concepts](svg11/concepts.md) (https://www.w3.org/TR/SVG11/concepts.html)
- [3 Rendering Model](svg11/render.md) (https://www.w3.org/TR/SVG11/render.html)
- [4 Basic Data Types and Interfaces](svg11/types.md) (https://www.w3.org/TR/SVG11/types.html)
- [5 Document Structure](svg11/struct.md) (https://www.w3.org/TR/SVG11/struct.html)
- [6 Styling](svg11/styling.md) (https://www.w3.org/TR/SVG11/styling.html)
- [7 Coordinate Systems, Transformations and Units](svg11/coords.md) (https://www.w3.org/TR/SVG11/coords.html)
- [8 Paths](svg11/paths.md) (https://www.w3.org/TR/SVG11/paths.html)
- [9 Basic Shapes](svg11/shapes.md) (https://www.w3.org/TR/SVG11/shapes.html)
- [10 Text](svg11/text.md) (https://www.w3.org/TR/SVG11/text.html)
- [11 Painting: Filling, Stroking and Marker Symbols](svg11/painting.md) (https://www.w3.org/TR/SVG11/painting.html)
- [12 Color](svg11/color.md) (https://www.w3.org/TR/SVG11/color.html)
- [13 Gradients and Patterns](svg11/pservers.md) (https://www.w3.org/TR/SVG11/pservers.html)
- [14 Clipping, Masking and Compositing](svg11/masking.md) (https://www.w3.org/TR/SVG11/masking.html)
- [15 Filter Effects](svg11/filters.md) (https://www.w3.org/TR/SVG11/filters.html)
- [16 Interactivity](svg11/interact.md) (https://www.w3.org/TR/SVG11/interact.html)
- [17 Linking](svg11/linking.md) (https://www.w3.org/TR/SVG11/linking.html)
- [19 Animation](svg11/animate.md) (https://www.w3.org/TR/SVG11/animate.html)
- [20 Fonts](svg11/fonts.md) (https://www.w3.org/TR/SVG11/fonts.html)
- [21 Metadata](svg11/metadata.md) (https://www.w3.org/TR/SVG11/metadata.html)
- [22 Backwards Compatibility](svg11/backward.md) (https://www.w3.org/TR/SVG11/backward.html)
- [23 Extensibility](svg11/extend.md) (https://www.w3.org/TR/SVG11/extend.html)
- [Appendix F: Implementation Requirements](svg11/implnote.md) (https://www.w3.org/TR/SVG11/implnote.html)
- [Appendix G: Conformance Criteria](svg11/conform.md) (https://www.w3.org/TR/SVG11/conform.html)
- [Appendix H: Accessibility Support](svg11/access.md) (https://www.w3.org/TR/SVG11/access.html)
- [Appendix I: Internationalization Support](svg11/i18n.md) (https://www.w3.org/TR/SVG11/i18n.html)
- [Appendix J: Minimizing SVG File Sizes](svg11/minimize.md) (https://www.w3.org/TR/SVG11/minimize.html)
- [Appendix K: References](svg11/refs.md) (https://www.w3.org/TR/SVG11/refs.html)
- [Appendix L: Element Index](svg11/eltindex.md) (https://www.w3.org/TR/SVG11/eltindex.html)
- [Appendix M: Attribute Index](svg11/attindex.md) (https://www.w3.org/TR/SVG11/attindex.html)
- [Appendix N: Property Index](svg11/propidx.md) (https://www.w3.org/TR/SVG11/propidx.html)
- [Appendix O: Feature Strings](svg11/feature.md) (https://www.w3.org/TR/SVG11/feature.html)
- [Appendix P: Media Type Registration for image/svg+xml](svg11/mimereg.md) (https://www.w3.org/TR/SVG11/mimereg.html)
- [Appendix Q: Changes](svg11/changes.md) (https://www.w3.org/TR/SVG11/changes.html)

## Modules

- [Filter Effects Module Level 1](filter-effects-1.md) (https://drafts.csswg.org/filter-effects-1/)
- [CSS Masking Module Level 1](css-masking-1.md) (https://drafts.csswg.org/css-masking-1/)
- [Compositing and Blending Module Level 1](compositing-1.md) (https://drafts.csswg.org/compositing-1/)
- [CSS Transforms Module Level 1](css-transforms-1.md) (https://drafts.csswg.org/css-transforms-1/)
- [CSS Transforms Module Level 2](css-transforms-2.md) (https://drafts.csswg.org/css-transforms-2/)
- [CSS Color Module Level 4](css-color-4.md) (https://drafts.csswg.org/css-color-4/)
- [CSS Values and Units Module Level 4](css-values-4.md) (https://drafts.csswg.org/css-values-4/)
- [CSS Shapes Module Level 1](css-shapes-1.md) (https://drafts.csswg.org/css-shapes-1/)
- [CSS Images Module Level 3](css-images-3.md) (https://drafts.csswg.org/css-images-3/)
- [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md) (https://drafts.csswg.org/css-backgrounds-3/)
- [CSS Box Model Module Level 4](css-box-4.md) (https://drafts.csswg.org/css-box-4/)
- [Selectors Level 4](selectors-4.md) (https://drafts.csswg.org/selectors-4/)
- [CSS Syntax Module Level 3](css-syntax-3.md) (https://drafts.csswg.org/css-syntax-3/)
- [CSS Cascading and Inheritance Module Level 5](css-cascade-5.md) (https://drafts.csswg.org/css-cascade-5/)
- [CSS Custom Properties for Cascading Variables Module Level 1](css-variables-1.md) (https://drafts.csswg.org/css-variables-1/)
- [CSS Values and Units Module Level 5](css-values-5.md) (https://drafts.csswg.org/css-values-5/)
- [CSS Basic User Interface Module Level 4](css-ui-4.md) (https://drafts.csswg.org/css-ui-4/)
