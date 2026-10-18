<!-- https://svgwg.org/svg2-draft/animate.html, as of 2026-10-18; written by dev/spec_text.py -->

# Appendix D: Animating SVG Documents

SVG supports the ability to change vector graphics over time, to create animated effects. SVG content can be animated in the following ways:

- Using SVG's animation elements [[svg-animation](refs.md#ref-svg-animation)]. SVG document fragments can describe time-based modifications to the document's elements. Using the various animation elements, authors can define motion paths, or interpolate the element's attributes and style properties. These effects can be chained together or triggered in response to other events in the document.
- Using CSS Animations [[css-animations-1](refs.md#ref-css-animations-1)]. This CSS module defines a way for authors to animate the values of CSS properties over time, using keyframes. The behavior of these keyframe animations can be controlled by specifying their duration, number of repeats, and repeating behavior.
- Using CSS Transitions [[css-transitions-1](refs.md#ref-css-transitions-1)]. This CSS module defines properties to specify that changes to values of CSS properties occur progressively over a specified duration.
- Using the [SVG DOM](types.md#SVGDOMOverview). The SVG DOM is defined as an extension of the DOM4 specification [[dom](refs.md#ref-dom)]. Every attribute and style sheet setting is accessible to scripting, and SVG offers a set of additional DOM interfaces to support efficient animation via scripting. Ideally, user agents that support scripting will also implement the animation frames APIs defined in HTML [[HTML](refs.md#ref-html)].
- Using the Web Animations API [[web-animations-1](refs.md#ref-web-animations-1)]. This DOM API provides a scripted interface to trigger user-agent optimized animations of style properties and attributes, without the need for authors to calculate values for individual frames.

SVG does not mandate support for any of these animation methods. However, user agents that do support them are expected to support them for SVG documents and SVG fragments in other documents. User agents that support declarative or scripted animation are required to conform to the restrictions based on [processing mode](conform.md#processing-modes) as defined in the [Conformance](conform.md) chapter, and to the special requirements for [animations in use-element shadow trees](struct.md#UseAnimations).
