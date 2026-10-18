<!-- https://svgwg.org/svg2-draft/struct.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 5: Document Structure

## <a id="NewDocument"></a>5.1. Defining an SVG document fragment: the ‘svg’ element

### <a id="NewDocumentOverview"></a>5.1.1. Overview

An [SVG document fragment](#TermSVGDocumentFragment) consists of any number of SVG elements contained within an ‘[svg](#SVGElement)’ element.

An SVG document fragment can range from an empty fragment (i.e., no content inside of the ‘[svg](#SVGElement)’ element), to a very simple SVG document fragment containing a single SVG [graphics element](#TermGraphicsElement) such as a ‘[rect](shapes.md#RectElement)’, to a complex, deeply nested collection of [container elements](#TermContainerElement) and [graphics elements](#TermGraphicsElement).

An SVG document fragment can stand by itself as a self-contained file or resource, in which case the SVG document fragment is an [SVG document](#TermSVGDocumentFragment), or it can be embedded inline as a fragment within a parent HTML or XML document.

<a id="EmbeddedSVGExample"></a>The following example shows simple SVG content embedded inline as a fragment within a parent XML document. Note the use of XML namespaces to indicate that the ‘[svg](#SVGElement)’ and ‘[ellipse](shapes.md#EllipseElement)’ elements belong to the [SVG namespace](#Namespace):

```
<?xml version="1.0" standalone="yes"?>
<parent xmlns="http://example.org"
        xmlns:svg="http://www.w3.org/2000/svg">
   <!-- parent contents here -->
   <svg:svg width="4cm" height="8cm">
      <svg:ellipse cx="2cm" cy="4cm" rx="2cm" ry="1cm" />
   </svg:svg>
   <!-- ... -->
</parent>
```

This example shows a slightly more complex (i.e., it contains multiple rectangles) stand-alone, self-contained SVG document:

```
<?xml version="1.0" standalone="no"?>
<svg width="5cm" height="4cm" version="1.1"
     xmlns="http://www.w3.org/2000/svg">
  <desc>Four separate rectangles
  </desc>
    <rect x="0.5cm" y="0.5cm" width="2cm" height="1cm"/>
    <rect x="0.5cm" y="2cm" width="1cm" height="1.5cm"/>
    <rect x="3cm" y="0.5cm" width="1.5cm" height="2cm"/>
    <rect x="3.5cm" y="3cm" width="1cm" height="0.5cm"/>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="4.98cm" height="3.98cm"
        fill="none" stroke="blue" stroke-width=".02cm" />

</svg>
```

‘[svg](#SVGElement)’ elements can appear in the middle of SVG content. This is the mechanism by which SVG document fragments can be embedded within other SVG document fragments.

Another use for ‘[svg](#SVGElement)’ elements within the middle of SVG content is to establish a new SVG viewport. (See [Establishing a new SVG viewport](coords.md#EstablishingANewSVGViewport).)

### <a id="Namespace"></a>5.1.2. Namespace

When SVG is parsed as a XML, for compliance with the Namespaces in XML Recommendation [[xml-names](refs.md#ref-xml-names)], an SVG namespace declaration must be provided so that all SVG elements are identified as belonging to the SVG namespace.

When using the HTML syntax, the namespace is provided automatically by the HTML parser.

```
<html>
<svg viewBox="0 0 100 100">
  <circle cx="50" cy="50" r="50" fill="green">
</svg>
</html>
```

As the example shows there's no need to have an ‘xmlns’ attribute declaring that the element is in the SVG namespace when using the HTML parser. The HTML parser will automatically create the SVG elements in the proper namespace.

<a id="issue1"></a>This section should talk about how a document's behavior is defined in terms of the DOM, and also explain how the HTML parser can create SVG fragments.

The SVG 2 namespace is `http://www.w3.org/2000/svg`, which is the same as for earlier versions of SVG.

The following are possible ways to provide a namespace declaration when SVG is parsed as XML. An ‘xmlns’ attribute without a namespace prefix could be specified on an ‘[svg](#SVGElement)’ element, which means that SVG is the default namespace for all elements within the scope of the element with the ‘xmlns’ attribute:

```
<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 100 100">
  <rect x="10" y="10" width="80" height="80" fill="green" />
</svg>
```

If a namespace prefix is specified on the ‘xmlns’ attribute (e.g., `xmlns:svg="http://www.w3.org/2000/svg"`), then the corresponding namespace is not the default namespace, so an explicit namespace prefix must be assigned to the elements:

```
<svg:svg xmlns:svg="http://www.w3.org/2000/svg" viewBox="0 0 100 100">
  <svg:rect x="10" y="10" width="80" height="80" fill="green" />
</svg:svg>
```

Namespace prefixes can be specified on ancestor elements (illustrated in the [above example](#EmbeddedSVGExample)). For more information, refer to the Namespaces in XML Recommendation [[xml-names](refs.md#ref-xml-names)].

### <a id="Definitions"></a>5.1.3. Definitions

***<a id="TermStructuralElement"></a>structural element***
: The structural elements are those which define the primary structure of an SVG document. Specifically, the following elements are structural elements: ‘[defs](#DefsElement)’, ‘[g](#GElement)’, ‘[svg](#SVGElement)’, ‘[symbol](#SymbolElement)’ and ‘[use](#UseElement)’.
***<a id="TermStructurallyExternalElement"></a>structurally external element***
: Elements that define its structure by reference to an external resource. Specifically, the following elements are structurally external elements when they have an ‘href’ attribute: ‘[foreignObject](embedded.md#ForeignObjectElement)’, ‘[image](embedded.md#ImageElement)’, ‘[script](interact.md#ScriptElement)’ and ‘[use](#UseElement)’.
***<a id="TermCurrentSVGDocumentFragment"></a>current SVG document fragment***
: The document sub-tree which starts with the outermost ancestor ‘[svg](#SVGElement)’ element of a given SVG element, with the requirement that all container elements between the outermost ‘[svg](#SVGElement)’ and the given element are all elements in the SVG namespace.
***<a id="TermOutermostSVGElement"></a>outermost svg element***
: The furthest ‘[svg](#SVGElement)’ ancestor element that remains in the [current SVG document fragment](#TermCurrentSVGDocumentFragment).
***<a id="TermSVGDocumentFragment"></a>SVG document fragment***
: A document sub-tree which starts with an ‘[svg](#SVGElement)’ element which is either the root element of the document or whose parent element is not in the SVG namespace. An SVG document fragment can consist of a stand-alone SVG document, or a fragment of a parent document enclosed by an ‘[svg](#SVGElement)’ element. However, an ‘[svg](#SVGElement)’ element that is a direct child of another SVG-namespaced element is not the root of an SVG document fragment.
***<a id="TermSVGElements"></a>SVG elements***
: Any element in the [SVG namespace](#Namespace).
**<a id="TermGraphicsElement"></a>*<a id="graphics-element"></a>graphics element***
: One of the element types that can cause graphics to be drawn onto the target canvas. Specifically: ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[foreignObject](embedded.md#ForeignObjectElement)’, ‘[image](embedded.md#ImageElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’, ‘[text](text.md#TextElement)’, ‘[textPath](text.md#TextPathElement)’ and ‘[tspan](text.md#TextElement)’.
**<a id="TermGraphicsReferencingElement"></a>*<a id="graphics-referencing-element"></a>graphics referencing element***
: A graphics element which uses a reference to a different document or element as the source of its graphical content. Specifically: ‘[image](embedded.md#ImageElement)’ and ‘[use](#UseElement)’.
### <a id="SVGElement"></a>5.1.4. The ‘svg’ element

‘*<a id="elementdef-svg"></a>svg*’

**Categories:**
: [Container element](#TermContainerElement), [renderable element](render.md#TermRenderableElement), [structural element](#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](#TermDescriptiveElement) — ‘[desc](#DescElement)’, ‘[title](#TitleElement)’, ‘[metadata](#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’
- [shape elements](shapes.md#TermShapeElement) — ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’
- [structural elements](#TermStructuralElement) — ‘[defs](#DefsElement)’, ‘[g](#GElement)’, ‘[svg](#SVGElement)’, ‘[symbol](#SymbolElement)’, ‘[use](#UseElement)’

[a](linking.md#AElement), [clipPath](../css-masking-1.md#ClipPathElement), [filter](../filter-effects-1.md#FilterElement), [foreignObject](embedded.md#ForeignObjectElement), [image](embedded.md#ImageElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement), [switch](#SwitchElement), [text](text.md#TextElement), [view](linking.md#ViewElement)
**Attributes:**
:

- [aria attributes](#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](#RoleAttribute)’
- [conditional processing attributes](#TermConditionalProcessingAttribute) — ‘[requiredExtensions](#RequiredExtensionsAttribute)’, ‘[systemLanguage](#SystemLanguageAttribute)’
- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- [document event attributes](interact.md#EventAttributes) — ‘[onunload](interact.md#EventAttributes)’, ‘[onabort](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[viewBox](coords.md#ViewBoxAttribute)’
- ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’
- ‘[transform](coords.md#TransformProperty)’

**Geometry properties:**
:

- [x](geometry.md#XProperty)
- [y](geometry.md#YProperty)
- [width](geometry.md#Sizing)
- [height](geometry.md#Sizing)

**DOM Interfaces:**
:

- [SVGSVGElement](#InterfaceSVGSVGElement)

The [x](geometry.md#XProperty) and [y](geometry.md#YProperty) attributes specify the top-left corner of the rectangular region into which an embedded ‘[svg](#SVGElement)’ element is placed. On an [outermost svg element](#TermOutermostSVGElement), these attributes have no effect.

For [outermost svg elements](#TermOutermostSVGElement), the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) attributes specify the intrinsic size of the SVG document fragment. For embedded ‘[svg](#SVGElement)’ elements, they specify the size of the rectangular region into which the ‘[svg](#SVGElement)’ element is placed. In either case, a computed style of auto is treated equivalent to 100%.

If an SVG document is likely to be referenced as a component of another document, the author will often want to include a ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute on the [outermost svg element](#TermOutermostSVGElement) of the referenced document. This attribute provides a convenient way to design SVG documents to scale-to-fit into an arbitrary SVG viewport.

<a id="SVGElementEventHandlerAttributes"></a>The ‘[svg](#SVGElement)’ element exposes as event handler content attributes a number of the event handlers of the Window object. It also mirrors their event handler IDL attributes.

The [onblur](interact.md), ‘[onerror](interact.md#EventAttributes)’, [onfocus](interact.md), ‘[onload](interact.md#EventAttributes)’, and ‘[onscroll](interact.md#EventAttributes)’ event handlers of the Window object, exposed on the ‘[svg](#SVGElement)’ element, replace the generic event handlers with the same names normally supported by [SVG elements](#TermSVGElements).

## <a id="Groups"></a>5.2. Grouping: the ‘g’ element

### <a id="GroupsOverview"></a>5.2.1. Overview

**<a id="TermContainerElement"></a>*<a id="container-element"></a>container element***
: An element which can have [graphics elements](#TermGraphicsElement) and other container elements as child elements. Specifically: ‘[a](linking.md#AElement)’, ‘[clipPath](../css-masking-1.md#ClipPathElement)’, ‘[defs](#DefsElement)’, ‘[g](#GElement)’, ‘[marker](painting.md#MarkerElement)’, ‘[mask](../css-masking-1.md#MaskElement)’, ‘[pattern](pservers.md#PatternElement)’, ‘[svg](#SVGElement)’, ‘[switch](#SwitchElement)’ and ‘[symbol](#SymbolElement)’.
The ‘[g](#GElement)’ element is a [container element](#TermContainerElement) for grouping together related [graphics elements](#TermGraphicsElement).

A group of elements, as well as individual objects, can be given a name using the ‘[id](#IDAttribute)’ attribute. Named groups are needed for several purposes such as animation and re-usable objects.

An example:

```
<?xml version="1.0" standalone="no"?>
<svg xmlns="http://www.w3.org/2000/svg"
     version="1.1" width="5cm" height="5cm">
  <desc>Two groups, each of two rectangles</desc>
  <g id="group1" fill="red">
    <rect x="1cm" y="1cm" width="1cm" height="1cm"/>
    <rect x="3cm" y="1cm" width="1cm" height="1cm"/>
  </g>
  <g id="group2" fill="blue">
    <rect x="1cm" y="3cm" width="1cm" height="1cm"/>
    <rect x="3cm" y="3cm" width="1cm" height="1cm"/>
  </g>

  <!-- Show outline of viewport using 'rect' element -->
  <rect x=".01cm" y=".01cm" width="4.98cm" height="4.98cm"
        fill="none" stroke="blue" stroke-width=".02cm"/>
</svg>
```

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/struct/grouping01.svg)

A ‘[g](#GElement)’ element can contain other ‘[g](#GElement)’ elements nested within it, to an arbitrary depth.

### <a id="GElement"></a>5.2.2. The ‘g’ element

‘*<a id="elementdef-g"></a>g*’

**Categories:**
: [Container element](#TermContainerElement), [renderable element](render.md#TermRenderableElement), [structural element](#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](#TermDescriptiveElement) — ‘[desc](#DescElement)’, ‘[title](#TitleElement)’, ‘[metadata](#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’
- [shape elements](shapes.md#TermShapeElement) — ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’
- [structural elements](#TermStructuralElement) — ‘[defs](#DefsElement)’, ‘[g](#GElement)’, ‘[svg](#SVGElement)’, ‘[symbol](#SymbolElement)’, ‘[use](#UseElement)’

[a](linking.md#AElement), [clipPath](../css-masking-1.md#ClipPathElement), [filter](../filter-effects-1.md#FilterElement), [foreignObject](embedded.md#ForeignObjectElement), [image](embedded.md#ImageElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement), [switch](#SwitchElement), [text](text.md#TextElement), [view](linking.md#ViewElement)
**Attributes:**
:

- [aria attributes](#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](#RoleAttribute)’
- [conditional processing attributes](#TermConditionalProcessingAttribute) — ‘[requiredExtensions](#RequiredExtensionsAttribute)’, ‘[systemLanguage](#SystemLanguageAttribute)’
- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —

**DOM Interfaces:**
:

- [SVGGElement](#InterfaceSVGGElement)

## <a id="Head"></a>5.3. Defining content for reuse, and the ‘defs’ element

### <a id="Overview"></a>5.3.1. Overview

SVG allows a graphical object to be defined for later reuse. To do this, SVG makes extensive use of the [URL reference](linking.md#TermURLReference) construct [[rfc3987](refs.md#ref-rfc3987)]. For example, to fill a rectangle with a linear gradient, a ‘[linearGradient](pservers.md#LinearGradientElement)’ element may be defined with an ‘[id](#IDAttribute)’ property that may be referenced in the value for the rectangle's [fill](painting.md#FillProperty) property, as in the following:

```
<linearGradient id="MyGradient">...</linearGradient>
<rect style="fill:url(#MyGradient)"/>
```

Some types of element, such as gradients, will not by themselves produce a graphical result. They can therefore be placed anywhere convenient. However, sometimes it is desired to define a graphical object and prevent it from being directly rendered. it is only there to be referenced elsewhere. To do this, and to allow convenient grouping defined content, SVG provides the ‘defs’ element.

It is recommended that, where possible, referenced elements be defined prior to the elements that use them, in document order. Collecting all referenced elements inside of a single ‘[defs](#DefsElement)’ element near the top of the file can make the markup easier to read and understand.

### <a id="DefsElement"></a>5.3.2. The ‘defs’ element

‘*<a id="elementdef-defs"></a>defs*’

**Categories:**
: [Container element](#TermContainerElement), [never-rendered element](render.md#TermNeverRenderedElement), [structural element](#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](#TermDescriptiveElement) — ‘[desc](#DescElement)’, ‘[title](#TitleElement)’, ‘[metadata](#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’
- [shape elements](shapes.md#TermShapeElement) — ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’
- [structural elements](#TermStructuralElement) — ‘[defs](#DefsElement)’, ‘[g](#GElement)’, ‘[svg](#SVGElement)’, ‘[symbol](#SymbolElement)’, ‘[use](#UseElement)’

[a](linking.md#AElement), [clipPath](../css-masking-1.md#ClipPathElement), [filter](../filter-effects-1.md#FilterElement), [foreignObject](embedded.md#ForeignObjectElement), [image](embedded.md#ImageElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement), [switch](#SwitchElement), [text](text.md#TextElement), [view](linking.md#ViewElement)
**Attributes:**
:

- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —

**DOM Interfaces:**
:

- [SVGDefsElement](#InterfaceSVGDefsElement)

The ‘[defs](#DefsElement)’ element is a container element for [referenced elements](#Head). For understandability and [accessibility](access.md) reasons, it is recommended that, whenever possible, referenced elements be defined inside of a ‘[defs](#DefsElement)’.

The content model for ‘[defs](#DefsElement)’ is the same as for the ‘[g](#GElement)’ element; thus, any element that can be a child of a ‘[g](#GElement)’ can also be a child of a ‘[defs](#DefsElement)’, and vice versa.

Elements that are descendants of a ‘[defs](#DefsElement)’ are not rendered directly; the [display](render.md#VisibilityControl) value for the ‘[defs](#DefsElement)’ element must always be set to none by the [user agent style sheet](styling.md#UAStyleSheet), and this declaration must have importance over any other CSS rule or presentation attribute. Note, however, that the descendants of a ‘[defs](#DefsElement)’ are always present in the source tree and thus can always be referenced by other elements; thus, the value of the [display](render.md#VisibilityControl) property on the ‘[defs](#DefsElement)’ element does not prevent those elements from being referenced by other elements.

## <a id="SymbolElement"></a>5.4. The ‘symbol’ element

The ‘[symbol](#SymbolElement)’ element is used to define graphical templates which can be instantiated by a ‘[use](#UseElement)’ element but which are not rendered directly.

A ‘[symbol](#SymbolElement)’ establishes a nested coordinate system for the graphics it contains. When a symbol is instantiated as the [referenced element](#TermReferencedElement) of a ‘[use](#UseElement)’ element, it is therefore rendered very similarly to a nested ‘[svg](#SVGElement)’ element.

‘*<a id="elementdef-symbol"></a>symbol*’

**Categories:**
: [Container element](#TermContainerElement), [structural element](#TermStructuralElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](#TermDescriptiveElement) — ‘[desc](#DescElement)’, ‘[title](#TitleElement)’, ‘[metadata](#MetadataElement)’
- [paint server elements](painting.md#TermPaintServerElement) — ‘[linearGradient](pservers.md#LinearGradientElement)’, ‘[radialGradient](pservers.md#RadialGradientElement)’, ‘[pattern](pservers.md#PatternElement)’
- [shape elements](shapes.md#TermShapeElement) — ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’
- [structural elements](#TermStructuralElement) — ‘[defs](#DefsElement)’, ‘[g](#GElement)’, ‘[svg](#SVGElement)’, ‘[symbol](#SymbolElement)’, ‘[use](#UseElement)’

[a](linking.md#AElement), [clipPath](../css-masking-1.md#ClipPathElement), [filter](../filter-effects-1.md#FilterElement), [foreignObject](embedded.md#ForeignObjectElement), [image](embedded.md#ImageElement), [marker](painting.md#MarkerElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement), [switch](#SwitchElement), [text](text.md#TextElement), [view](linking.md#ViewElement)
**Attributes:**
:

- [aria attributes](#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](#RoleAttribute)’
- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’
- ‘[viewBox](coords.md#ViewBoxAttribute)’
- ‘[refX](#SymbolElementRefXAttribute)’
- ‘[refY](#SymbolElementRefYAttribute)’

**Geometry properties:**
:

- [x](geometry.md#XProperty)
- [y](geometry.md#YProperty)
- [width](geometry.md#Sizing)
- [height](geometry.md#Sizing)

**DOM Interfaces:**
:

- [SVGSymbolElement](#InterfaceSVGSymbolElement)

The [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing), and [height](geometry.md#Sizing) geometry properties have the same effect as on an ‘[svg](#SVGElement)’ element, when the ‘[symbol](#SymbolElement)’ is instantiated by a ‘[use](#UseElement)’ element. In particular, if [width](geometry.md#Sizing) and [height](geometry.md#Sizing) compute to auto (and are not over-ridden by values on the instantiating ‘[use](#UseElement)’ element), then they will be treated as a value of 100%.

New in SVG 2. Allowing geometry properties to be specified on a symbol provides a more consistent rendering model, and allows authors to set a default size for each symbol (which may still be over-ridden by attributes on the ‘[use](#UseElement)’ element).

### <a id="SymbolAttributes"></a>5.4.1. Attributes

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="SymbolElementRefXAttribute"></a>refX* | <length> \| left \| center \| right | (none) | yes |
| *<a id="SymbolElementRefYAttribute"></a>refY* | <length> \| top \| center \| bottom | (none) | yes |

**
:

New in SVG 2. Added to make it easier to align symbols to a particular point, as is often done in maps. Similar to the matching attributes on ‘[marker](painting.md#MarkerElement)’.

The ‘[refX](#SymbolElementRefXAttribute)’ and ‘[refY](#SymbolElementRefYAttribute)’ attributes define the reference point of the symbol which is to be placed exactly at the symbol's x,y positioning coordinate, as defined by the cumulative effect of the [x](geometry.md#XProperty) and [y](geometry.md#YProperty) properties and any transformations on the ‘[symbol](#SymbolElement)’ and its [host](#TermShadowHost) ‘[use](#UseElement)’ element.

Keyword values have the same meaning as for the ‘[refX](painting.md#MarkerElementRefXAttribute)’ and ‘[refY](painting.md#MarkerElementRefYAttribute)’ attributes on the ‘[marker](painting.md#MarkerElement)’ element, resolving to 0%, 50%, or 100% in the applicable direction.

Unlike other positioning attributes, ‘[refX](#SymbolElementRefXAttribute)’ and ‘[refY](#SymbolElementRefYAttribute)’ are interpreted as being in the coordinate system of the symbol contents, after application of the ‘[viewBox](coords.md#ViewBoxAttribute)’ and ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attributes. If one or both of the attributes is not specified, no adjustment is made in the corresponding dimension, and the top or left side of the symbol's rectangular viewport region (regardless of the ‘[viewBox](coords.md#ViewBoxAttribute)’ coordinates) is positioned at the x,y point.

For backwards compatibility, the behavior when ‘[refX](#SymbolElementRefXAttribute)’ and ‘[refY](#SymbolElementRefYAttribute)’ are not specified on a ‘[symbol](#SymbolElement)’ is different from when they are specified with a value of 0, and therefore different from the behavior when equivalent attributes are not specified on a ‘[marker](painting.md#MarkerElement)’.

### <a id="SymbolNotes"></a>5.4.2. Notes on symbols

The use of ‘[symbol](#SymbolElement)’ elements for graphics that are used multiple times in the same document adds structure and semantics. Closely related to the ‘[symbol](#SymbolElement)’ element are the ‘[marker](painting.md#MarkerElement)’ and ‘[pattern](pservers.md#PatternElement)’ elements; all three define a container of graphical content that can be rendered repeatedly at various positions and scales in the SVG. However, while [re-used graphics](render.md#TermReusedGraphics) in a pattern and marker provide a graphical effect on another element, the content in a ‘[symbol](#SymbolElement)’ will be embedded as fully interactive content, within a [use-element shadow tree](#TermUseElementShadowTree).

The [user agent style sheet](styling.md#UAStyleSheet) sets the [overflow](render.md#OverflowAndClipProperties) property for ‘[symbol](#SymbolElement)’ elements to hidden, which causes a rectangular clipping path to be created at the bounds of symbol's SVG viewport. Unless the [overflow](render.md#OverflowAndClipProperties) property is overridden, any graphics within the symbol which goes outside of the symbol's SVG viewport will be clipped.

‘[symbol](#SymbolElement)’ elements must never be rendered directly; their only usage is as something that can be referenced using the ‘[use](#UseElement)’ element. The user agent must set the [display](render.md#VisibilityControl) property on the ‘[symbol](#SymbolElement)’ element to none, as part of the [user agent style sheet](styling.md#UAStyleSheet), and this declaration must have importance over any other CSS rule or presentation attribute.

The generated [instance](#TermElementInstance) of a ‘[symbol](#SymbolElement)’ that is the direct [referenced element](#TermReferencedElement) of a ‘[use](#UseElement)’ element must always have a computed value of inline for the [display](render.md#VisibilityControl) property. In other words, it must be rendered whenever the host ‘[use](#UseElement)’ element is rendered. The [user agent style sheet](styling.md#UAStyleSheet) again defines this declaration to have importance over any other CSS rule or presentation attribute. Any other ‘[symbol](#SymbolElement)’ that is cloned to create an [element instance](#TermElementInstance) within the [use-element shadow tree](#TermUseElementShadowTree) behaves as a symbol definition, and must not be rendered.

## <a id="UseElement"></a>5.5. The ‘use’ element

‘*<a id="elementdef-use"></a>use*’

**Categories:**
: [Graphics referencing element](#TermGraphicsReferencingElement), [renderable element](render.md#TermRenderableElement), [structural element](#TermStructuralElement), [structurally external element](#TermStructurallyExternalElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](#TermDescriptiveElement) — ‘[desc](#DescElement)’, ‘[title](#TitleElement)’, ‘[metadata](#MetadataElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](#RoleAttribute)’
- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- [conditional processing attributes](#TermConditionalProcessingAttribute) — ‘[requiredExtensions](#RequiredExtensionsAttribute)’, ‘[systemLanguage](#SystemLanguageAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- [deprecated xlink attributes](linking.md#XLinkRefAttrs) — ‘[xlink:href](linking.md#XLinkHrefAttribute)’, ‘[xlink:title](linking.md#XLinkTitleAttribute)’
- ‘[href](#UseElementHrefAttribute)’

**Geometry properties:**
:

- [x](geometry.md#XProperty)
- [y](geometry.md#YProperty)
- [width](geometry.md#Sizing)
- [height](geometry.md#Sizing)

**DOM Interfaces:**
:

- [SVGUseElement](#InterfaceSVGUseElement)

The ‘[use](#UseElement)’ element references another element, a copy of which is rendered in place of the ‘[use](#UseElement)’ in the document. The [referenced element](#TermReferencedElement) may be a [container element](#TermContainerElement), in which case a copy of the complete SVG document subtree rooted at that element is used.

The cloned content inherits styles from the ‘[use](#UseElement)’ element and can be the target of user events. However, these cloned [element instances](#TermElementInstance) remain linked to the referenced source and reflect DOM mutations in the original. In addition, all style rules that apply in the scope of the referenced element also apply in the scope of the cloned [shadow tree](#TermShadowTree).

The [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) geometric properties specify the positioning of the referenced element. The [width](geometry.md#Sizing) and [height](geometry.md#Sizing) attributes only have an effect if the [referenced element](#TermReferencedElement) defines a viewport (i.e., if it is a ‘[svg](#SVGElement)’ or ‘[symbol](#SymbolElement)’); if so, a value other than auto for the ‘[use](#UseElement)’ element overrides the value of the corresponding geometric property on that element.

A negative value for [width](geometry.md#Sizing) or [height](geometry.md#Sizing) is [invalid](types.md#TermInvalidValue) and must be [ignored](../css-syntax-3.md#css-ignored). If [width](geometry.md#Sizing) or [height](geometry.md#Sizing) is zero, and the properties have an effect on the [referenced element](#TermReferencedElement), then rendering of that element will be disabled.

The [x](geometry.md#XProperty) and [y](geometry.md#YProperty) properties affect the user coordinate system for the element. See the [Layout](#UseLayout) section for implementation details.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="UseElementHrefAttribute"></a>href* | URL [[URL]](types.md#attribute-url) | (none) | yes |

**
:

An [URL reference](linking.md#URLReference) to the element/fragment within an SVG document to be cloned for rendering.

The ‘[use](#UseElement)’ element can reference an entire SVG document by specifying an ‘[href](#UseElementHrefAttribute)’ value without a fragment. Such references are taken to be referring to the root element of the referenced document.

Refer to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).
New in SVG 2. An ‘[href](#UseElementHrefAttribute)’ without a fragment allows an entire SVG document to be referenced without having to ensure that it has an ID on its root element.

User agents may restrict external resource documents for security reasons. In particular, this specification does not allow cross-origin and [data URL](linking.md#TermDataURL) resource requests in ‘[use](#UseElement)’.

When the ‘[href](#UseElementHrefAttribute)’ attribute is set (or, in the absence of an ‘[href](#UseElementHrefAttribute)’ attribute, an ‘[xlink:href](linking.md#XLinkHrefAttribute)’ attribute), the user agent must [process the URL](linking.md#processingURL). The target element that results from URL processing is the [referenced element](#TermReferencedElement) of the ‘[use](#UseElement)’.

If the [referenced element](#TermReferencedElement) that results from resolving the URL is not an SVG element, then the reference is [invalid](types.md#TermInvalidValue) and the ‘[use](#UseElement)’ element is in error.

If the referenced element is a (shadow-including) ancestor of the ‘[use](#UseElement)’ element, then this is an [invalid circular reference](linking.md#TermCircularReference) and the ‘[use](#UseElement)’ element is in error.

Otherwise, the user agent must generate a [shadow tree](#TermShadowTree) of [re-used graphics](render.md#TermReusedGraphics) to render as the contents of the ‘[use](#UseElement)’ element, as described in the next section, [The use-element shadow tree](#UseShadowTree).

A ‘[use](#UseElement)’ that has an [unresolved](linking.md#TermUnresolvedReference) or [invalid](types.md#TermInvalidValue) URL reference is not rendered. For the purpose of bounding box calculations, it is equivalent to an empty container element.

### <a id="UseShadowTree"></a>5.5.1. The use-element shadow tree

The [re-used graphics](render.md#TermReusedGraphics) generated by a ‘[use](#UseElement)’ element are defined in terms of a [shadow tree](#TermShadowTree). In terms of interactivity and style inheritance, they are therefore quite different from other types of [re-used graphics](render.md#TermReusedGraphics) in SVG, such as ‘[pattern](pservers.md#PatternElement)’ and ‘[marker](painting.md#MarkerElement)’ content.

Elements in the shadow tree are rendered as if the ‘[use](#UseElement)’ element was a container and they were its children. However, the SVG Document Object Model (DOM) only contains the ‘[use](#UseElement)’ element and its attributes. The SVG DOM does not include the element instances as children of the ‘[use](#UseElement)’ element.

User agents that support scripting and the document object model must implement the [use-element shadow tree](#TermUseElementShadowTree) as described in this section and in conformance with the [dom](refs.md#ref-dom) specification [[dom]](refs.md#ref-dom), or its future replacement. In contrast, user agents that do *not* support the dynamic interactive processing mode may not need to implement all the details of the shadow DOM. However, all user agents must ensure that the [layout](#UseLayout) and [style inheritance](#UseStyleInheritance) for the re-used graphics and [declarative animations](#UseAnimations) if applicable, are rendered in the same way as if the shadow DOM was implemented.

The following definitions apply when discussing ‘[use](#UseElement)’ elements and their shadow trees:

***<a id="TermReferencedElement"></a>referenced element***
: The element specified by the ‘[href](#UseElementHrefAttribute)’ (or ‘[xlink:href](linking.md#XLinkHrefAttribute)’) attribute on the ‘[use](#UseElement)’ element, or the root element of a document referenced by that attribute if the URL provided does not include a target fragment that links to a specific element ‘[id](#IDAttribute)’.
***<a id="TermReferencedDocumentSubtree"></a>referenced document subtree***
**referenced graphics**
: The referenced element, and all of its descendent nodes.
***<a id="TermShadowRoot"></a>shadow root***
: A ShadowRoot object, a type of DocumentFragment node which is associated with a host Element, and which contains the content that will be used to render that host. A shadow root should be implemented in conformance with the [dom](refs.md#ref-dom) specification [[dom]](refs.md#ref-dom), or its future replacement.
***<a id="TermShadowHost"></a>shadow host***
**host**
: An element that has an associated shadow root; usage is consistent the definition of host in the DOM standard.
***<a id="TermShadowTree"></a>shadow tree***
: A node tree whose root is a shadow root; usage is consistent the definition of shadow tree in the DOM standard.
***<a id="TermUseElementShadowTree"></a>use-element shadow tree***
: A shadow tree whose host is a ‘[use](#UseElement)’ element, which contains element instances generated by cloning the referenced graphics.
***<a id="TermElementInstance"></a>element instance***
**instance**
: An element in the [use-element shadow tree](#TermUseElementShadowTree), which is generated by cloning a corresponding element in the referenced document subtree.
***<a id="TermInstanceRoot"></a>instance root***
: The [element instance](#TermElementInstance) for the referenced element; it is always a direct child of the ‘[use](#UseElement)’ element's shadow root.
***<a id="TermCorrespondingElement"></a>corresponding element***
: For each element instance, the element in the referenced document subtree from which it is cloned.
***<a id="TermCorrespondingUseElement"></a>corresponding use element***
: For each element instance, the ‘[use](#UseElement)’ element which causes it to be rendered in the document. This is the instance's shadow root's host ‘[use](#UseElement)’ element *if* that element is not itself an element instance within a ‘[use](#UseElement)’ element shadow tree, or is that element's corresponding use element otherwise, recursively exiting shadow trees as many times as necessary to reach a ‘[use](#UseElement)’ element that was not itself generated as part of the shadow tree of another ‘[use](#UseElement)’ element.
When the user agent successfully resolves a ‘[use](#UseElement)’ element to identify a [referenced element](#TermReferencedElement), the user agent must create a [use-element shadow tree](#TermUseElementShadowTree) whose host is the ‘[use](#UseElement)’ element itself. The shadow tree must be created even if the ‘[use](#UseElement)’ element is not rendered because it is a descendent of a [never-rendered element](render.md#TermNeverRenderedElement), because of conditional processing, or because of the [display](render.md#VisibilityControl) property being set to none on it or an ancestor element.

Each node in the shadow tree is an [instance](#TermElementInstance) of a corresponding node from the [referenced document subtree](#TermReferencedDocumentSubtree). The shadow nodes all descend from the [instance root](#TermInstanceRoot), which is the instance of the [referenced element](#TermReferencedElement), and which itself is a direct child of the [shadow root](#TermShadowRoot) node.

The shadow tree is open (inspectable by script), but read-only. Any attempt to directly modify the elements, attributes, and other nodes in the shadow tree must throw a `NoModificationAllowedError`.

Within a [use-element shadow tree](#TermUseElementShadowTree), ‘[script](interact.md#ScriptElement)’ elements are inert (do not execute).

Previous versions of SVG restricted the contents of the shadow tree to SVG graphics elements. This specification allows any valid SVG document subtree to be cloned. Cloning non-graphical content, however, will not usually have any visible effect.

If the [referenced element](#TermReferencedElement) is in an external file, then all [URL references](linking.md#TermURLReference) in attributes and style properties must be made absolute as described in [Generating the absolute URL](linking.md#processingURL-absolute), before copying the value to the [element instances](#TermElementInstance). The shadow tree itself uses the same document base URL as the document that includes it.

The user agent must ensure that all mutations to the [referenced document subtree](#TermReferencedDocumentSubtree) are reflected in the shadow tree. This includes changes to elements, attributes, and text and other nodes. In addition, changes to the stylesheets in effect for the referenced graphics must be reflected in changes to the stylesheets in the shadow tree's scope, as described further in the section on [style inheritance](#UseStyleInheritance).

If either the ‘[use](#UseElement)’ element or the [referenced element](#TermReferencedElement) is altered in a way that causes the ‘[use](#UseElement)’ element's URL reference to become [unresolved](linking.md#TermUnresolvedReference) again, then the entire shadow tree for that use element is discarded.

When a ‘[use](#UseElement)’ references another element which is another ‘[use](#UseElement)’ or whose content contains a ‘[use](#UseElement)’ element, then the shadow DOM cloning approach described above is recursive. However, a set of references that directly or indirectly reference a element to create a circular dependency is an [invalid circular reference](linking.md#TermCircularReference). The ‘[use](#UseElement)’ element or element instance whose shadow tree would create the circular reference is in error and must not be rendered by the user agent.

### <a id="UseLayout"></a>5.5.2. Layout of re-used graphics

The value of the [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties on a ‘[use](#UseElement)’ element are used to position the re-used graphics and to set the viewport size if the [referenced element](#TermReferencedElement) defines a nested viewport. The effect of these properties on a ‘[use](#UseElement)’ element is notably different from their effect on a [graphics element](#TermGraphicsElement), or from their effect in CSS box layout.

The [x](geometry.md#XProperty) and [y](geometry.md#YProperty) properties define an additional transformation (translate(x,y), where x and y represent the computed value of the corresponding property) to be applied to the ‘[use](#UseElement)’ element, after any transformations specified with other properties (i.e., appended to the right-side of the transformation list).

For historical reasons, the supplemental transformation is applied to the ‘[use](#UseElement)’ element itself, rather than solely to the re-used content in the shadow tree. This affects the coordinate system used for any masks, clipping paths, or filters applied to the ‘[use](#UseElement)’ element and calculated in userSpaceOnUse units.

To apply userSpaceOnUse graphical effects in an un-transformed coordinate space, while also using the [x](geometry.md#XProperty) and [y](geometry.md#YProperty) to position the graphics, authors can nest the ‘[use](#UseElement)’ element inside a ‘[g](#GElement)’, and apply the graphical effects to the ‘[g](#GElement)’ element.

The [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties on the ‘[use](#UseElement)’ element override the values for the corresponding properties on a referenced ‘[svg](#SVGElement)’ or ‘[symbol](#SymbolElement)’ element when determining the used value for that property on the [instance root](#TermInstanceRoot) element. However, if the computed value for the property on the ‘[use](#UseElement)’ element is auto, then the property is computed as normal for the element instance.

These properties can therefore be used to scale a graphic that defines its own coordinate system, each time it is re-used. Because auto is the initial value, if dimensions are not explicitly set on the ‘[use](#UseElement)’ element, the values set on the ‘[svg](#SVGElement)’ or ‘[symbol](#SymbolElement)’ will be used as defaults.

The [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties on the ‘[use](#UseElement)’ element have no effect if the [referenced element](#TermReferencedElement) does not [establish a new viewport](coords.md#EstablishingANewSVGViewport). In particular, the ‘[use](#UseElement)’ element does not itself establish a new viewport, and therefore does not affect the interpretation of percentages in the re-used graphics.

In all other ways, rendering and layout of elements within the [use-element shadow tree](#TermUseElementShadowTree) occurs as if the ‘[use](#UseElement)’ element was a container for its shadow content. In particular, unless elements within the shadow tree establish a new viewport, they must be drawn in the coordinate system in which the ‘[use](#UseElement)’ element is defined (including any cumulative transformations). This affects the interpretation of percentage lengths, and also graphical effects with userSpaceOnUse units.

### <a id="UseStyleInheritance"></a>5.5.3. Style Scoping and Inheritance

The [use-element shadow tree](#TermUseElementShadowTree), like other shadow trees, exhibits style encapsulation, as defined in the CSS Scoping module [[css-scoping-1]](refs.md#ref-css-scoping-1). This means that elements in the shadow tree inherit styles from its [host](#TermShadowHost) ‘[use](#UseElement)’ element, but that style rules defined in the outer document do not match the elements in the shadow tree. Instead, the shadow tree maintains its own list of stylesheets, whose CSS rules are matched against elements in the shadow tree.

Presentation attributes and the ‘[style](styling.md#StyleAttribute)’ attribute are cloned from the elements in the [referenced graphics](#TermReferencedDocumentSubtree) into the [element instances](#TermElementInstance) in the same manner as other attributes.

When the [referenced element](#TermReferencedElement) is from the same document as the ‘[use](#UseElement)’ element, the same document stylesheets will apply in both the original document and the shadow tree document fragment. Any changes to the stylesheets in the main document also affect the shadow tree; the `StyleSheetList` object accessed through the document and shadow root document fragment's `styleSheets` properties must be identical. If a ‘[style](styling.md#StyleElement)’ element is duplicated as part of the [referenced document subtree](#TermReferencedDocumentSubtree), then the `styleSheet` property on the [element instance](#TermElementInstance) points to the same object as for the [corresponding element](#TermCorrespondingElement).

When the [referenced element](#TermReferencedElement) is from an external document, the stylesheet objects generated when processing that document apply to the shadow tree, and are read-only. All [URL references](linking.md#TermURLReference) in the stylesheet, including fragment-only references, must be made absolute, relative to the URL of the document that contains the [referenced element](#TermReferencedElement). User agents may re-use the same stylesheet objects for any shadow trees that reference that same external document.

Style rules that are scoped to the shadow tree cannot normally affect any elements in the main document. Similarly, style rules in the main document can only affect the shadow tree elements by changing inherited values. However, CSS Scoping defines special selectors for styling the [host](#TermShadowHost) element from within the shadow tree, or for adjusting styles within the shadow tree in response to changes in the host's context [[css-scoping-1]](refs.md#ref-css-scoping-1).

CSS media queries within a shadow tree's scope are evaluated using the same device features and dimensions as the corresponding "light" document (that is, the document that contains the [corresponding use element](#TermCorrespondingUseElement) for the shadow tree, after recursively exiting all nested shadow trees).

In most cases, the [element instance](#TermElementInstance) in the shadow tree will match the same style rules as its [corresponding element](#TermCorrespondingElement) in the original document. However, if a CSS rule uses a [complex selector](../selectors-4.md#complex) to match an element based on its ancestors or siblings, and those ancestors or siblings are not cloned as part of the shadow tree, then that rule would no longer match the [element instance](#TermElementInstance). Similarly, child-indexed pseudo-classes such as `nth-of-type` and `nth-child` may apply to one element but not the other. This represents a change from how style cloning was defined in previous versions of SVG.

The following example demonstrates both the consistent and changed style-matching rules. The circle on the left is re-used to draw the circle on the right. The original circle has styles set in various ways:

- [stroke-width](painting.md#StrokeWidthProperty) (20) is set in a presentation attribute on the circle itself.
- [stroke-opacity](painting.md#StrokeOpacityProperty) (0.7) is set via a CSS rule with a simple selector matching the circle tag name.
- [stroke](painting.md#StrokeProperty) color (green) is set using a complex CSS selector, matching the circle as a descendent of an element with class `special`.
- [fill](painting.md#FillProperty) color is not set directly on the circle, so is inherited from the style set on the containing ‘[g](#GElement)’ element (blue).

In the SVG 1.1 style-cloning model, the specified style values would be cloned from the original element to the element instance. The re-used circle would have the same styles as the original, except that the [fill](painting.md#FillProperty) value would be inherited from the ‘[use](#UseElement)’ (orange) instead of from the ‘[g](#GElement)’ (blue).

In the shadow DOM model required by SVG 2, the styles for the re-used circle are calculated as follows:

- the [stroke-width](painting.md#StrokeWidthProperty) (20) presentation attribute is cloned to the element instance.
- the CSS rule setting [stroke-opacity](painting.md#StrokeOpacityProperty) (0.7) is part of the CSS stylesheet cloned into the shadow tree; it matches the circle tag name of the [element instance](#TermElementInstance), so is applied.
- the CSS rule with the complex selector is also part of the cloned stylesheet, but it does not match the [element instance](#TermElementInstance) of the circle, which is not a descendent of an element with class `special`; instead, [stroke](painting.md#StrokeProperty) color on the circle is inherited from the host ‘[use](#UseElement)’ element (purple).
- [fill](painting.md#FillProperty) color is still not set directly, so is once again inherited from the host ‘[use](#UseElement)’ element.

The re-used circle therefore differs from the original in both fill color (because it inherits from a different element) and stroke color (because the complex selector no longer matches).

```
<svg xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink"
     width="200" height="100" viewBox="0 0 200 100">
  <title>Style inheritance and the use element</title>
  <desc>
    Two circles, one of which is a re-styled clone of the other.
    This file demonstrates one of the cases where 
    the shadow-DOM style matching rules in SVG 2
    have a different effect than the SVG 1.1 style cloning rules. 
    The original circle on the left
    should have blue fill 
    and green stroke.
    In a conforming SVG 1.1 user agent,
    the re-used circle on the right
    should have orange fill and green stroke.
    In a conforming SVG 2 user agent,
    the re-used circle should have orange fill and purple stroke.
    In all cases,
    the stroke should be partially transparent
    and 20 units wide, 
    relative to a total circle diameter of 100 units.
  </desc>
  <style type="text/css">
    circle          { stroke-opacity: 0.7; }
    .special circle { stroke: green; }
    use             { stroke: purple;
                      fill: orange; }
  </style>
  <g class="special" style="fill: blue">
     <circle id="c" cy="50" cx="50" r="40" 
             stroke-width="20" />
  </g>
  <use xlink:href="#c" x="100" />
</svg>
```

[image: Example Use-changed-styles — A 'use' element copying a 'circle', with various style matching rules demonstrated]

Example Use-changed-styles

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/struct/Use-changed-styles.svg)

Previous versions of SVG were not clear about how dynamic pseudo-classes (such as `:hover`) should apply to element instances. The shadow tree model requires that all such pseudo-classes are matched independently to the [element instance](#TermElementInstance) or to its [corresponding element](#TermCorrespondingElement), depending on which element the user is interacting with.

Specifying 'visibility:hidden' on a ‘[use](#UseElement)’ element does not guarantee that the referenced content will not be rendered. Unlike the [display](render.md#VisibilityControl) or the [opacity](render.md#ObjectAndGroupOpacityProperties) properties, the [visibility](render.md#VisibilityControl) property does not apply directly to container elements, and therefore does not apply directly to the ‘[use](#UseElement)’ element. Because [visibility](render.md#VisibilityControl) is normally inherited, hiding the use element will often hide the child content, but not necessarily. If any graphics elements in the shadow tree have 'visibility:visible' specified, then that element will be visible even if the ‘[use](#UseElement)’ element specifies 'visibility:hidden'.

In the following example, key style rules are as follows:

```
.dark {
  visibility: hidden;
}
.eyes {
  visibility: visible;
}
svg:hover .dark, svg:focus .dark {
  visibility: visible;
}
```

The "dark" class is set on the group containing the ‘[use](#UseElement)’ elements, so all parts of the re-used graphics inherit the hidden visibility setting, except for the subtrees with class "eyes", where it is reset to visible. Upon hovering or focusing the graphic, the hiding effect is removed.

[image: Multiple use-copies of a creature-symbol on a black background; the use elements have visibility: hidden, but the creatures' eyes have visibility: visible.]

Example Use-visibility-hidden, default styles

[image: Multiple use-copies of a rabbit-symbol on a light green background; each rabbit has different=coloured fur, but the same pink noses and white fluffy tails.]

Example Use-visibility-hidden, interactive styles

[View this example as SVG](https://svgwg.org/svg2-draft/images/struct/Use-visibility-inheritance.svg)

The example also demonstrates inheritance of other style properties ([fill](painting.md#FillProperty) and [stroke](painting.md#StrokeProperty)) specified on the ‘[use](#UseElement)’ elements, and how these are also not used if any elements within the symbol specify explicit values (e.g., the pink noses and ears and the white tails).

### <a id="UseAnimations"></a>5.5.4. Animations in use-element shadow trees

The Web Animations API [[web-animations-1](refs.md#ref-web-animations-1)] and the SVG Animations specification [[svg-animation](refs.md#ref-svg-animation)] define non-CSS ways to animate attributes and styles on targetted elements without directly manipulating DOM properties (see the [Animation appendix](animate.md) for details). User agents that implement those features must ensure that all animations that apply to an element in a [referenced document subtree](#TermReferencedDocumentSubtree) also apply to instances of that element in a [use-element shadow tree](#TermUseElementShadowTree), as described in this section.

Scripted animations created by directly manipulating attributes on elements in the [referenced graphics](#TermReferencedDocumentSubtree) (including the ‘[style](styling.md#StyleAttribute)’ attribute or its IDL property) will be propagated to the [element instances](#TermElementInstance) in the shadow tree in the same manner as any other DOM manipulations.

Animation effects applied using CSS will be duplicated along with other stylesheet rules, following the procedure specified in the [Style Scoping and Inheritance](#UseStyleInheritance) section.

All animations within a [use-element shadow tree](#TermUseElementShadowTree) operate in the same document timeline as for the [corresponding use element](#TermCorrespondingUseElement), regardless of whether the [referenced element](#TermReferencedElement) is from the same or an external document.

For animation effects applied using a Web Animations API method [[web-animations-1]](refs.md#ref-web-animations-1), if the target of the animation is a [corresponding element](#TermCorrespondingElement) to an [element instance](#TermElementInstance) in a shadow tree, the user agent must construct a [ShadowAnimation](#InterfaceShadowAnimation) whose source is that Animation object and whose target is the [element instance](#TermElementInstance). If there are multiple instances of the element in different trees, then there will be multiple shadow animations, one for each.

The user agent must create such a [ShadowAnimation](#InterfaceShadowAnimation) for all Web Animations API animations in effect (including pending and frozen animations) at the time the shadow tree is generated, and for any new animations applied while the shadow tree exists. The user agent must not create [ShadowAnimation](#InterfaceShadowAnimation) objects for CSS animations or animation elements (as these are duplicated separately).

As part of the interface definition, a [ShadowAnimation](#InterfaceShadowAnimation) is read-only, and must reflect any changes to its [sourceAnimation](#__svg__ShadowAnimation__sourceAnimation).

Any attempts to directly apply new animations to a target that is a read-only [element instance](#TermElementInstance) (or pseudo-element) within a [use-element shadow tree](#TermUseElementShadowTree) must throw a `NoModificationAllowedError`.

For each animation element [[svg-animation](refs.md#ref-svg-animation)] that targets an element in the [referenced document subtree](#TermReferencedDocumentSubtree), the user agent must ensure that an equivalent animation element is in effect in the [use-element shadow tree](#TermUseElementShadowTree). If the animation element itself is part of the referenced document subtree, then this happens as a matter of course through the creation of an [element instance](#TermElementInstance) for the animation element. Otherwise, the user agent must generate an [element instance](#TermElementInstance) for the animation element that has the same effect as if it was a node in the shadow tree. The effective document order for these generated animation elements must be the same as the document order for their [corresponding elements](#TermCorrespondingElement).

Each animation element or instance must only affect a target element in the same node tree (shadow or light), regardless of whether the targetting is implicit (the parent element) or explicit (a URL cross-reference to an element ‘[id](#IDAttribute)’). In this way, the one-to-one relationship between animation elements and target elements is preserved.

The ‘[id](#IDAttribute)’ attribute is cloned, like any other attribute, from the [corresponding element](#TermCorrespondingElement) to the [element instance](#TermElementInstance); This does not conflict with the requirement for ‘[id](#IDAttribute)’ to be unique, because the clone and the original are in distinct node trees.

All animation elements, in the document or in the shadow trees, which are timed to begin or end in response to an event on another element identified by its ‘[id](#IDAttribute)’ attribute, must also begin or end when any [instance](#TermElementInstance) of an element with that ‘[id](#IDAttribute)’ receives the same event. This is consistent with how event listeners on a referenced element also listen to events on instances of that element, as described in the section on [Event handling in use-element shadow trees](#UseEventHandling). This behavior does not apply to animation begin or end times defined only by an event and not by an ‘[id](#IDAttribute)’ (and therefore implicitly listening for the event on the target element); in that case, each animation element is only triggered by its own target.

At the time an [instance](#TermElementInstance) of an animation element is generated within a shadow tree, if there is an active animation associated with the [corresponding element](#TermCorrespondingElement) (including a frozen animation), and the timing event that initiated that animation would also have initiated the instance if it existed, then the animation for the [element instance](#TermElementInstance) must be initiated, with its begin time adjusted backwards in the document timeline to match the timing of the [corresponding element](#TermCorrespondingElement).

In many cases, the requirements of this section mean that the [element instance](#TermElementInstance) and its [corresponding element](#TermCorrespondingElement) will animate synchronously. This will be the case if the animation is purely time-based, or if it begins and ends in response to user interaction on an element referenced by its ‘[id](#IDAttribute)’. However, if the animation is triggered by a user interaction event on the targetted element (implicitly), then only the element or element instance that receives the interaction event will display the animation.

This is a change from previous versions of SVG, which required all animations on the [corresponding element](#TermCorrespondingElement) to be mirrored, regardless of user interaction, but which did not offer clear guidance for responding to user interactions with the element instances. The change ensures that interactive animations declared with animation elements behave in the same manner as interactive CSS styles and CSS animations.

In order to create animations that apply to all instances when any instance or the original element receives an event, specify the element ‘[id](#IDAttribute)’ explicitly:

```
<set href="#target" begin="mouseover" ... />
  <!-- only affects the element that is moused over -->

<set href="#target" begin="target.mouseover" ... />
  <!-- affects all instances of the element with the id 'target',
          in all light and shadow node trees,
          when any of them are moused over -->
  
```

### <a id="UseEventHandling"></a>5.5.5. Event handling in use-element shadow trees

Element in a [use-element shadow tree](#TermUseElementShadowTree) can both listen for and be the target of DOM events. Event retargetting provides encapsulation, so that the details of the shadow DOM structure are masked when an event bubbles out of the shadow tree and into the light.

Event retargeting is new in SVG 2. It provides consistency with the Shadow DOM specification, with existing implementations, and with the expectations of authors who are only concerned with elements in the main DOM.

Any event listeners defined on an element in the [referenced graphics](#TermReferencedDocumentSubtree) must also listen for the same event, at the same capture phase, on each [instance](#TermElementInstance) of that element in a [use-element shadow tree](#TermUseElementShadowTree). This includes event listeners assigned using [event attributes](interact.md#TermEventAttribute) (which would be duplicated as with any other DOM attribute) and also event listeners assigned using the `addEventListener` method. The user agent must ensure that the list of event listeners for each [element instance](#TermElementInstance) is synchronized to match its [corresponding element](#TermCorrespondingElement). An event listener cannot be directly assigned to a read-only [element instance](#TermElementInstance) in a [use-element shadow tree](#TermUseElementShadowTree). Any attempt to add an event listener to such an element must throw a `NoModificationAllowedError`.

Events in the [use-element shadow tree](#TermUseElementShadowTree) are dispatched and bubble according to the shadow tree event path and event retargeting algorithm [[DOM](refs.md#ref-dom)].

In general, the event path for a [use-element shadow tree](#TermUseElementShadowTree) is constructed from the ancestors of the event target element up to the [shadow root](#TermShadowRoot), then the [host](#TermShadowHost) ‘[use](#UseElement)’ element and its event path through to the document window. This means that, in the capture phase, an event propagates from the window through the regular document tree to the ‘[use](#UseElement)’ element and then to the shadow root object and down through the shadow tree (or recursively through multiple shadow trees) to the event target element. In the bubbling phase, the event passes in the opposite direction, from the shadow tree elements to the shadow root, then to the ‘[use](#UseElement)’ element and its ancestors.

The event retargeting algorithm ensures that from the perspective of event listeners on the ‘[use](#UseElement)’ element or its ancestors, all events targetted to [element instances](#TermElementInstance) in the shadow tree instead have a target of the ‘[use](#UseElement)’ element itself. If the event has both a `target` and a `relatedTarget`, and both of these properties would be retargeted to point to the same ‘[use](#UseElement)’ element, then the event is not propagated at all outside of the shadow tree. This would occur, for example, if focus moved from one element inside the shadow tree to another. Certain other event types are constrained to not propagate outside of the shadow tree in which they were created.

In contrast, event listeners that process the event while it is propagating through the shadow tree (because the listener has been added to a [corresponding element](#TermCorrespondingElement)) will receive the event with its `target` pointing to a read-only [element instance](#TermElementInstance) in the shadow tree. The [correspondingElement](#__svg__SVGElementInstance__correspondingElement) and [correspondingUseElement](#__svg__SVGElementInstance__correspondingUseElement) properties of that [element instance](#TermElementInstance) can be used to connect it to the modifiable elements in the main DOM.

## <a id="ConditionalProcessing"></a>5.6. Conditional processing

### <a id="ConditionalProcessingOverview"></a>5.6.1. Conditional processing overview

SVG contains a ‘[switch](#SwitchElement)’ element along with attributes ‘[requiredExtensions](#RequiredExtensionsAttribute)’ and ‘[systemLanguage](#SystemLanguageAttribute)’ to provide an ability to specify alternate viewing depending on the capabilities of a given user agent or the user's language.

Attributes ‘[requiredExtensions](#RequiredExtensionsAttribute)’ and ‘[systemLanguage](#SystemLanguageAttribute)’ act as tests and evaluate to either true or false. The ‘[switch](#SwitchElement)’ renders the first of its children for which all of these attributes test true. If the given attribute is not specified, then a true value is assumed.

When an element is excluded because of conditional processing, it is treated as if it had a used value of `none` for the [display](render.md#VisibilityControl) property. Similar to the [display](render.md#VisibilityControl) property, conditional processing attributes only affect the direct rendering of elements and do not prevent elements from being successfully referenced by other elements (such as via a ‘[use](#UseElement)’).

In consequence:

- conditional processing affects the visual display of [graphics elements](#TermGraphicsElement), ‘[a](linking.md#AElement)’, ‘[foreignObject](embedded.md#ForeignObjectElement)’, and [text content elements](text.md#TermTextContentElement).
- conditional processing prevents animation elements from playing.
- conditional processing will have no effect on [never-rendered elements](render.md#TermNeverRenderedElement); in particular, conditional processing does not affect the processing of a ‘[style](styling.md#StyleElement)’ or ‘[script](interact.md#ScriptElement)’ element.
- conditional processing of child content of a never-rendered container element (e.g., a ‘[pattern](pservers.md#PatternElement)’ or a ‘[mask](../css-masking-1.md#MaskElement)’) will affect whether that child content contributes to the graphical effect.

Previous versions of SVG included a third conditional processing attribute, `requiredFeatures`. This was intended to allow authors to provide fallback behavior for user agents that only implemented parts of the SVG specification. Unfortunately, poor specification and implementation of this attribute made it unreliable as a test of feature support.

### <a id="ConditionalProcessingDefinitions"></a>5.6.2. Definitions

***<a id="TermConditionalProcessingAttribute"></a>conditional processing attribute***
: A conditional processing attribute is one that controls whether or not the element on which it appears is processed. Most elements, but not all, may have conditional processing attributes specified on them. See [Conditional processing](#ConditionalProcessing) for details. The conditional processing attributes defined in SVG 2 are ‘[requiredExtensions](#RequiredExtensionsAttribute)’ and ‘[systemLanguage](#SystemLanguageAttribute)’.
### <a id="SwitchElement"></a>5.6.3. The ‘switch’ element

‘*<a id="elementdef-switch"></a>switch*’

**Categories:**
: [Container element](#TermContainerElement), [renderable element](render.md#TermRenderableElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [shape elements](shapes.md#TermShapeElement) — ‘[circle](shapes.md#CircleElement)’, ‘[ellipse](shapes.md#EllipseElement)’, ‘[line](shapes.md#LineElement)’, ‘[path](paths.md#PathElement)’, ‘[polygon](shapes.md#PolygonElement)’, ‘[polyline](shapes.md#PolylineElement)’, ‘[rect](shapes.md#RectElement)’

[a](linking.md#AElement), [foreignObject](embedded.md#ForeignObjectElement), [g](#GElement), [image](embedded.md#ImageElement), [svg](#SVGElement), [switch](#SwitchElement), [text](text.md#TextElement), [use](#UseElement)
**Attributes:**
:

- [aria attributes](#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](#RoleAttribute)’
- [conditional processing attributes](#TermConditionalProcessingAttribute) — ‘[requiredExtensions](#RequiredExtensionsAttribute)’, ‘[systemLanguage](#SystemLanguageAttribute)’
- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —

**DOM Interfaces:**
:

- [SVGSwitchElement](#InterfaceSVGSwitchElement)

The ‘[switch](#SwitchElement)’ element evaluates the ‘[requiredExtensions](#RequiredExtensionsAttribute)’ and ‘[systemLanguage](#SystemLanguageAttribute)’ attributes on its direct child elements in order, and then processes and renders the first child for which these attributes evaluate to true. All others will be bypassed and therefore not rendered. If the child element is a container element such as a ‘[g](#GElement)’, then the entire subtree is either processed/rendered or bypassed/not rendered.

In SVG, when evaluating the ‘[systemLanguage](#SystemLanguageAttribute)’ attribute, the order of evaluation of descendant elements of the ‘[switch](#SwitchElement)’ element must be as if the 'allowReorder' attribute, defined in the SMIL specification [[SMIL](refs.md#ref-SMIL)] always has a value of 'yes'.

Note that the values of properties [display](render.md#VisibilityControl) and [visibility](render.md#VisibilityControl) have no effect on ‘[switch](#SwitchElement)’ element processing. In particular, setting [display](render.md#VisibilityControl) to none on a child of a ‘[switch](#SwitchElement)’ element has no effect on true/false testing associated with ‘[switch](#SwitchElement)’ element processing.

The ‘[switch](#SwitchElement)’ element does not affect the processing of ‘[script](interact.md#ScriptElement)’ and ‘[style](styling.md#StyleElement)’ elements.

For more information and an example, see [Embedding foreign object types](embedded.md#ForeignObjectElement).

### <a id="ConditionalProcessingRequiredExtensionsAttribute"></a>5.6.4. The ‘requiredExtensions’ attribute

The ‘[requiredExtensions](#RequiredExtensionsAttribute)’ attribute defines a list of required language extensions. Language extensions are capabilities within a user agent that go beyond the feature set defined in this specification. Each extension is identified by an [URL reference](linking.md#URLReference).

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="RequiredExtensionsAttribute"></a>requiredExtensions* | set of space-separated tokens [HTML] | (none) | no |

**
:

The value is a list of [URL reference](linking.md#URLReference)s which identify the required extensions, with the individual values separated by white space. Determines whether all of the named *extensions* are supported by the user agent. If all of the given extensions are supported, then the attribute evaluates to true; otherwise, the current element and its children are skipped and thus will not be rendered.

If a given [URL reference](linking.md#URLReference) contains white space within itself, that white space must be escaped.

If the attribute is not present, then it implicitly evaluates to "true". If a null string or empty string value is given to attribute ‘[requiredExtensions](#RequiredExtensionsAttribute)’, the attribute evaluates to "false".

‘[requiredExtensions](#RequiredExtensionsAttribute)’ is often used in conjunction with the ‘[switch](#SwitchElement)’ element. If the ‘[requiredExtensions](#RequiredExtensionsAttribute)’ is used in other situations, then it represents a simple switch on the given element whether to render the element or not.

The URL names for the extension should include versioning information, such as "http://example.org/SVGExtensionXYZ/1.0", so that script writers can distinguish between different versions of a given extension.

### <a id="ConditionalProcessingSystemLanguageAttribute"></a>5.6.5. The ‘systemLanguage’ attribute

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="SystemLanguageAttribute"></a>systemLanguage* | set of comma-separated tokens [HTML] | (none) | no |

**
:

The value is a set of comma-separated tokens, each of which must be a Language-Tag value, as defined in BCP 47 [[BCP47](refs.md#ref-bcp47)].

Evaluates to "true" if one of the language tags indicated by user preferences is a case-insensitive match of one of the language tags given in the value of this parameter, or if one of the language tags indicated by user preferences is a case-insensitive prefix of one of the language tags given in the value of this parameter such that the first tag character following the prefix is "-".

Evaluates to "false" otherwise.

If the attribute is not present, then it implicitly evaluates to "true". If a null string or empty string value is given to attribute ‘[systemLanguage](#SystemLanguageAttribute)’, the attribute evaluates to "false".

Note: This use of a prefix matching rule does not imply that language tags are assigned to languages in such a way that it is always true that if a user understands a language with a certain tag, then this user will also understand all languages with tags for which this tag is a prefix.

The prefix rule simply allows the use of prefix tags if this is the case.

Implementation note: When making the choice of linguistic preference available to the user, implementers should take into account the fact that users are not familiar with the details of language matching as described above, and should provide appropriate guidance. As an example, users may assume that on selecting "en-gb", they will be served any kind of English document if British English is not available. The user interface for setting user preferences should guide the user to add "en" to get the best matching behavior.

Multiple languages may be listed for content that is intended for multiple audiences. For example, content that is presented simultaneously in the original Maori and English versions, would call for:

```
<text systemLanguage="mi, en"><!-- content goes here --></text>
```

However, just because multiple languages are present within the object on which the ‘[systemLanguage](#SystemLanguageAttribute)’ test attribute is placed, this does not mean that it is intended for multiple linguistic audiences. An example would be a beginner's language primer, such as "A First Lesson in Latin," which is clearly intended to be used by an English-literate audience. In this case, the ‘[systemLanguage](#SystemLanguageAttribute)’ test attribute should only include "en".

Authoring note: Authors should realize that if several alternative language objects are enclosed in a ‘[switch](#SwitchElement)’, and none of them matches, this may lead to situations where no content is displayed. It is thus recommended to include a "catch-all" choice at the end of such a ‘[switch](#SwitchElement)’ which is acceptable in all cases.

‘[systemLanguage](#SystemLanguageAttribute)’ is often used in conjunction with the ‘[switch](#SwitchElement)’ element. If the ‘[systemLanguage](#SystemLanguageAttribute)’ is used in other situations, then it represents a simple switch on the given element whether to render the element or not.

## <a id="DescriptionAndTitleElements"></a>5.7. The ‘desc’ and ‘title’ elements

### <a id="DescriptionDefinitions"></a>5.7.1. Definition

***<a id="TermDescriptiveElement"></a>descriptive element***
: An element which provides supplementary descriptive information about its parent. Specifically, the following elements are descriptive elements: ‘[desc](#DescElement)’, ‘[metadata](#MetadataElement)’ and ‘[title](#TitleElement)’.
Multilingual descriptive text selection, based on the ‘[lang](#LangAttribute)’ attribute, was added to allow internationalization of the ‘[desc](#DescElement)’ and ‘[title](#TitleElement)’ elements.

Any [container element](#TermContainerElement) or [graphics element](#TermGraphicsElement) in an SVG document can have zero or more ‘[desc](#DescElement)’ and/or ‘[title](#TitleElement)’ elements as children, whose content is text. ‘[desc](#DescElement)’ and ‘[title](#TitleElement)’ elements are not visually rendered as part of the graphics. The [display](render.md#VisibilityControl) value for the ‘[title](#TitleElement)’ and ‘[desc](#DescElement)’ elements must always be set to none by the [user agent style sheet](styling.md#UAStyleSheet), and this declaration must have importance over any other CSS rule or presentation attribute.

Multiple sibling ‘[desc](#DescElement)’ or ‘[title](#TitleElement)’ elements must have different languages, as defined using a ‘[lang](#LangAttribute)’ attribute (or `xml:lang` attribute) on the descriptive element or an ancestor. The user agent must select the element of each type whose language best matches language preferences set by the user. A descriptive element with an empty-string language tag (indicating no language, for example a text alternative consisting of emoji symbols) is a lowest-priority match for any user, ranked below all user-specified language preferences. If multiple equally valid matches exist, the first match should be used. If no match exists for either 'title' or 'desc', the first element of that type must be selected.

The following example shows alternative language titles on a re-used star icon, inline in an HTML document. The example assumes that the HTML document as a whole has a correctly-declared language of `en` (English without a specified country code).

```
<svg>
  <use href="#star">
    <title>Favourite</title>
    <title lang="en-us">Favorite</title>
    <title lang="nl">Favoriet</title>
    <title lang="">★</title>
  </use>
</svg>
  
```

The first `title` element inherits the language of the document (`en`); the others have explicitly-declared languages for each element. If the user's preferred language (out of those provided) is American English, the icon title is the American spelling "Favorite". If the user's preferred language is Dutch, the icon title is "Favoriet". If the user's preference list includes generic English ranked higher than Dutch, the title is "Favourite" with British spelling. If the user does not understand either Dutch or English, the title will be the star symbol character—which is not ideal (most screen readers will read it as a localized version of "black star"), but better than no text alternative at all.

Authors should be aware that SVG 1.1-supporting user agents that have not yet implemented multi-lingual descriptive text will normally select the first element of each type, regardless of user preferences. SVG 1.1 user agents may also fail to recognize a `title` element that is not the first child of its parent, or a `desc` element that has previous siblings that are not other descriptive elements.

<a id="issue67"></a>The use of more than one ‘[title](#TitleElement)’ or ‘[desc](#DescElement)’ element to provide localised information is at risk, with no known implementations.

User agents must make the text content of selected 'title' and 'desc' elements available to platform accessibility APIs as part of the name and description computation for the parent element, as defined in the SVG Accessibility API Mappings [SVG-AAM] specification.

Inclusion of any 'title' or 'desc' elements as a direct child of a [rendered element](render.md#TermRenderedElement) indicates that the rendered element is of semantic importance in the graphic. Authors should not, and [SVG generators](conform.md#TermSVGGenerator) must not, include empty 'title' or 'desc' elements with no text content or whitespace-only text content, as this will result in a nameless object being presented to assistive technology users.

If an individual graphic element has no meaning on its own, alternative text should instead be provided for the nearest container element that describes a meaningful object. Authors should use grouping (‘[g](#GElement)’) elements to structure their drawing elements into meaningful objects, and name those groups with ‘[title](#TitleElement)’. Conversely, if a container object is used simply to apply styles or layout, and neither defines an object nor provides meaningful grouping structure, it does not need alternative text.

Descriptive text elements whose parent is [not rendered](render.md#Rendered-vs-NonRendered) may be used by authors or authoring tools as reference information; authors are warned that this data is not normally available to end users viewing the graphic through assistive technologies. Nonetheless, a [non-rendered element](render.md#TermNonRenderedElement) may be referenced as part of the accessible name or description of a rendered element (as defined in SVG-AAM), and the recursive computation will use descriptive child elements of the referenced element.

Description and title elements may contain marked-up text from other namespaces, using standard XML mechanisms to indicate the namespace. However, authors should not rely on such markup to provide meaning to alternative text; only the plain text content is currently required to be exposed to assistive technologies.

The HTML parser treats all markup within ‘[title](#TitleElement)’ and ‘[desc](#DescElement)’ the same way it treats markup in an HTML fragment; most elements will be assigned to the HTML namespace.

User agents may use markup within ‘[title](#TitleElement)’ to influence the visual presentation of titles (such as tooltips), but are not required to do so.

<a id="TitleElement"></a>

‘*<a id="elementdef-title"></a>title*’

**Categories:**
: [Descriptive element](#TermDescriptiveElement), [never-rendered element](render.md#TermNeverRenderedElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’

**DOM Interfaces:**
:

- [SVGTitleElement](#InterfaceSVGTitleElement)

The ‘[title](#TitleElement)’ child element represents a short text alternative for the element.

On a link, this could be the title or a description of the target resource; on an image or drawing object, it could be a short description of the graphic; on interactive content, it could be a label for, or instructions for, use of the element; and so forth.

Authors should not provide redundant information in a ‘[title](#TitleElement)’ element if there is also a visible label for the drawing element (e.g., using a ‘[text](text.md#TextElement)’ element). Instead, the visual label should be associated with the drawing element using an ‘aria-labelledby’ attribute.

Interactive user agents should make the plain text content of ‘[title](#TitleElement)’ elements available in response to user interaction, in a manner consistent with platform conventions; existing user agents commonly render ‘[title](#TitleElement)’ elements as a tooltip on hovering the parent element.

Authors should provide a ‘[title](#TitleElement)’ child element to the root svg element within a stand-alone SVG document. Since users often consult documents out of context, authors should provide context-rich titles. Thus, instead of a title such as "Introduction", which doesn't provide much contextual background, authors should supply a title such as "Introduction to Medieval Bee-Keeping" instead. For reasons of accessibility, user agents should always make the content of the ‘title’ child element to the root svg element available to users. However, this is typically done through other means than the tooltips used for nested SVG and graphics elements, e.g., by displaying in a browser tab.

<a id="DescElement"></a>

‘*<a id="elementdef-desc"></a>desc*’

**Categories:**
: [Descriptive element](#TermDescriptiveElement), [never-rendered element](render.md#TermNeverRenderedElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’

**DOM Interfaces:**
:

- [SVGDescElement](#InterfaceSVGDescElement)

The ‘[desc](#DescElement)’ element represents more detailed textual information for the element such as a description. This is typically exposed to assistive technologies to provide more detailed information, such as a description of the visual appearance of a graphic or help to explain the functionality of a complex widget. It is not typically available to other users, so should not be used for essential instructions.

Authors may associate detailed information, including visible text, with part of the graphic using ‘aria-describedby’ attribute (on the described element or a parent container), with the value being an ID reference to one or more SVG or HTML elements containing the description. The ‘aria-describedby’ attribute takes precedence over the child ‘[desc](#DescElement)’ when providing a description. If an element has both visible description and a child ‘[desc](#DescElement)’ element providing supplementary information, authors should explicitly include the ‘[id](#IDAttribute)’ of the element itself in its own ‘aria-describedby’ list, in order to concatenate the two descriptions together.

## <a id="MetadataElement"></a>5.8. The ‘metadata’ element

Metadata which is included with SVG content should be specified within ‘[metadata](#MetadataElement)’ elements. The contents of the ‘[metadata](#MetadataElement)’ should be elements from other XML namespaces, with these elements from these namespaces expressed in a manner conforming with the Namespaces in XML Recommendation [[xml-names](refs.md#ref-xml-names)].

‘*<a id="elementdef-metadata"></a>metadata*’

**Categories:**
: [Descriptive element](#TermDescriptiveElement), [never-rendered element](render.md#TermNeverRenderedElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](#TermCoreAttribute) — ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’

**DOM Interfaces:**
:

- [SVGMetadataElement](#InterfaceSVGMetadataElement)

Metadata content is not directly rendered; the [display](render.md#VisibilityControl) value for the ‘[metadata](#MetadataElement)’ element must always be set to none by the [user agent style sheet](styling.md#UAStyleSheet), and this declaration must have importance over any other CSS rule or presentation attribute.

Here is an example of how metadata can be included in an SVG document. The example uses the Dublin Core version 1.1 schema. (Other XML-compatible metadata languages, including ones not based on RDF, can be used also.)

```
<?xml version="1.0" standalone="yes"?>
<svg width="4in" height="3in"
    xmlns = 'http://www.w3.org/2000/svg'>
    <desc xmlns:myfoo="http://example.org/myfoo">
      <myfoo:title>This is a financial report</myfoo:title>
      <myfoo:descr>The global description uses markup from the
        <myfoo:emph>myfoo</myfoo:emph> namespace.</myfoo:descr>
      <myfoo:scene><myfoo:what>widget $growth</myfoo:what>
      <myfoo:contains>$three $graph-bar</myfoo:contains>
        <myfoo:when>1998 $through 2000</myfoo:when> </myfoo:scene>
   </desc>
    <metadata>
      <rdf:RDF
           xmlns:rdf = "http://www.w3.org/1999/02/22-rdf-syntax-ns#"
           xmlns:rdfs = "http://www.w3.org/2000/01/rdf-schema#"
           xmlns:dc = "http://purl.org/dc/elements/1.1/" >
        <rdf:Description about="http://example.org/myfoo"
             dc:title="MyFoo Financial Report"
             dc:description="$three $bar $thousands $dollars $from 1998 $through 2000"
             dc:publisher="Example Organization"
             dc:date="2000-04-11"
             dc:format="image/svg+xml"
             dc:language="en" >
          <dc:creator>
            <rdf:Bag>
              <rdf:li>Irving Bird</rdf:li>
              <rdf:li>Mary Lambert</rdf:li>
            </rdf:Bag>
          </dc:creator>
        </rdf:Description>
      </rdf:RDF>
    </metadata>
</svg>
```

## <a id="HTMLMetadataElements"></a>5.9. HTML metadata elements

For user agents that support HTML, the following HTML elements (in the HTML namespace) must be supported in SVG documents:

- the `base` element
- the `link` element
- the `meta` element
- the `style` element
- the `script` element

Note that the `base` element will affect all URL values in the document, including e.g. paint server references or ‘[use](#UseElement)’ element references. However, when [processing URL references](linking.md#processingURL-absolute) to identify a specific target element, the user agent must always compare the generated absolute URL against the current document base URL to determine whether it is a [same-document URL reference](linking.md#TermSameDocumentURL). In this way, target-fragment only references to elements in the same document remain valid, regardless of any changes to the document base URL.

## <a id="ForeignNamespaces"></a>5.10. Foreign namespaces and private data

SVG allows inclusion of elements from foreign namespaces anywhere within the SVG content. In general, the SVG user agent must include the unknown foreign-namespaced elements in the DOM but will ignore and exclude them for rendering purposes.

The notable exceptions is described in the [Embedded Content chapter](embedded.md) under [Embedding Foreign Object Types](embedded.md#ForeignObjectElement).

Additionally, SVG allows inclusion of attributes from foreign namespaces on any SVG element. The SVG user agent must include unknown attributes in the DOM but should otherwise ignore unknown attributes.

Authors should be aware that unknown namespaced elements and attributes will not be parsed as such by the HTML parser. Instead, the namespace prefix will be included in the tag or attribute name, elements will be placed in the parent element namespace and attributes in the default namespace.

To add custom attributes in a way that will result in consistent parsing in both XML and HTML documents, authors may use the [‘data-*’ attributes](#DataAttributes). These can be added to SVG ‘[metadata](#MetadataElement)’ elements if the information they encode is not associated with any other element in the document.

SVG's ability to include foreign namespaces can be used for the following purposes:

- Application-specific information so that authoring applications can include model-level data in the SVG content to serve their "roundtripping" purposes (i.e., the ability to write, then read a file without loss of higher-level information).
- Supplemental data for extensibility. For example, suppose there is an extension which extrudes 2D graphics into three dimensions according to certain parameters; these parameters may be included in the SVG content by inserting elements from the extension's namespace.

For example, a business graphics authoring application might want to include some private data within an SVG document so that it could properly reassemble the chart (a pie chart in this case) upon reading it back in:

```
<?xml version="1.0" standalone="yes"?>
<svg width="4in" height="3in"
     xmlns = 'http://www.w3.org/2000/svg'>
  <defs>
    <myapp:piechart xmlns:myapp="http://example.org/myapp"
                    title="Sales by Region">
      <myapp:pieslice label="Northern Region" value="1.23"/>
      <myapp:pieslice label="Eastern Region" value="2.53"/>
      <myapp:pieslice label="Southern Region" value="3.89"/>
      <myapp:pieslice label="Western Region" value="2.04"/>
      <!-- Other private data goes here -->
    </myapp:piechart>
  </defs>
  <desc>This chart includes private data in another namespace
  </desc>
  <!-- In here would be the actual SVG graphics elements which
       draw the pie chart -->
</svg>
```

## <a id="CommonAttributes"></a>5.11. Common attributes

### <a id="CommonAttributeDefinitions"></a>5.11.1. Definitions

***<a id="TermCoreAttribute"></a>core attributes***
: The core attributes are those attributes that can be specified on any SVG element. The core attributes are ‘[id](#IDAttribute)’, ‘[tabindex](#SVGElementTabindexAttribute)’, ‘[autofocus](#SVGElementAutofocusAttribute)’, ‘[lang](#LangAttribute)’, ‘[xml:space](#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’ and ‘[style](styling.md#StyleAttribute)’, along with all [custom data attributes](#DataAttributes).
### <a id="Core.attrib"></a>5.11.2. Attributes common to all elements: ‘id’

The ‘[id](#IDAttribute)’ attribute is available on all SVG elements:

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="IDAttribute"></a>id* | (see below) | (none) | no |

**
:

Must reflect the element's ID [[DOM](refs.md#ref-dom)]. The ‘[id](#IDAttribute)’ attribute must be unique within the node tree, must not be an empty string, and must not contain any whitespace characters.

Additional requirements apply in order for the ‘[id](#IDAttribute)’ attribute to be valid in XML documents, as defined in the specification for the relevant version of XML. A stand-alone SVG document uses XML 1.0 syntax [[xml](refs.md#ref-xml)], which specifies that valid ‘[id](#IDAttribute)’ values are XML name tokens. Valid XML 1.0 names only include designated characters (letters, digits, and a few punctuation marks), and do not start with a digit, a full stop (.) character, or a hyphen-minus (-) character.

User agents should process ‘[id](#IDAttribute)’ values in SVG files irrespective of XML validity.

Authors should avoid the use of ‘[id](#IDAttribute)’ values that would be parsed as an [SVG view specification](linking.md#SVGViewSpecification) or a [basic media fragment](linking.md#MediaFragments) when used as a URL target fragment.

### <a id="LangSpaceAttrs"></a>5.11.3. The ‘lang’ and ‘xml:lang’ attributes

The ‘lang’ attribute (in no namespace) specifies the primary language for the element's contents and for any of the element's attributes that contain text.

The ‘lang’ attribute in the XML namespace is defined in XML [[xml](refs.md#ref-xml)].

If these attributes are omitted from an element, then the language of this element is the same as the language of its parent element, if any.

The ‘lang’ attribute in the XML namespace may be used on SVG elements in XML documents. If both the ‘lang’ attribute in no namespace and the ‘lang’ attribute in the XML namespace are specified on the same element, they must have exactly the same value when compared in an ASCII case-insensitive manner.

If both the ‘lang’ attribute in no namespace and the ‘lang’ attribute in the XML namespace are set on an element, user agents must use the ‘lang’ attribute in the XML namespace, and the ‘lang’ attribute in no namespace must be ignored for the purposes of determining the element's language.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="LangAttribute"></a>lang* | Language-Tag [ABNF] | (none) | no |

**
:

The ‘lang’ attribute specifies the primary language for the element's contents and for any of the element's attributes that contain text. Its value must be a valid BCP 47 language tag, or the empty string. Setting the attribute to the empty string indicates that the primary language is unknown. [[BCP47](refs.md#ref-bcp47)].

### <a id="WhitespaceProcessingXMLSpaceAttribute"></a>5.11.4. The ‘xml:space’ attribute

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="XMLSpaceAttribute"></a>xml:space* | (see below) | default | no |

**
:

Deprecated XML attribute to specify whether white space is preserved in character data. The only possible values are the strings 'default' and 'preserve', without white space. Refer to the Extensible Markup Language (XML) 1.0 Recommendation [[xml](refs.md#ref-xml)] and to the discussion [white space handling](text.md#WhiteSpace) in SVG.

New content should use the white-space property instead.

### <a id="tabindexattribute"></a>5.11.5. The ‘tabindex’ attribute

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="SVGElementTabindexAttribute"></a>tabindex* | valid integer [HTML] | (none) | no |

**
:

This content attribute allows authors to control whether an element is focusable, whether it is supposed to be reachable using [sequential focus navigation](interact.md#Focus), and what is to be the relative order of the element for the purposes of sequential focus navigation.

The name "tab index" comes from the common use of the "tab" key to navigate through the focusable elements. The term "tabbing" refers to moving forward through the focusable elements that can be reached using sequential focus navigation.

### <a id="autofocusattribute"></a>5.11.6. The ‘autofocus’ attribute

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="SVGElementAutofocusAttribute"></a>autofocus* | boolean attribute [HTML] | (none) | no |

**
:

This content attribute allows authors to ask a focusable element to be focused after it's connected to a document. See autofocus in the HTML specification for details.

The attribute has no effect if the element is not already [focusable](interact.md#TermFocusable).

### <a id="DataAttributes"></a>5.11.7. The ‘data-*’ attributes

All SVG elements support custom data attributes, which are those in no namespace whose names begin with the string "data-". See the requirements for custom data attributes in the HTML specification.

## <a id="WAIARIAAttributes"></a>5.12. WAI-ARIA attributes

### <a id="WAIARIA-definitions"></a>5.12.1. Definitions

***<a id="TermARIAAttribute"></a>ARIA attributes***
: These are the attributes defined in WAI-ARIA, consisting of WAI-ARIA states and properties as well as the role attribute. See the WAI-ARIA Definition of Roles, the WAI-ARIA Graphics Module Graphics Roles, and the WAI-ARIA Supported States and Properties. The aria attributes are ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’ and ‘[role](#RoleAttribute)’.
Note that the above list of ARIA attributes may be expanded by future WAI-ARIA specifications.

### <a id="roleattribute"></a>5.12.2. Role attribute

Any [renderable element](render.md#TermRenderableElement) may have an ARIA role attribute specified; the role attribute is ignored on [non-rendered elements](render.md#TermNonRenderedElement). The attribute, if specified, must have a value that is a set of space-separated tokens representing the various WAI-ARIA roles that the element belongs to. These tokens are role values defined in Definition of Roles ([[wai-aria](refs.md#ref-wai-aria)], section 5.4) and Graphics Roles ([[graphics-aria-1.0](refs.md#ref-graphics-aria-1.0)], section 4).

The WAI-ARIA role that an SVG element has assigned to it is the first valid role found in the list of tokens generated when the role attribute is split on spaces. A valid role is a recognized, non-abstract role that is [allowed for the element type](#implicit-aria-semantics).

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="RoleAttribute"></a>role* | set of space-separated tokens [HTML] | (see below) | no |

**
:

The ‘[role](#RoleAttribute)’ attribute must be a set of space-separated tokens having values defined in Definition of Roles ([[wai-aria](refs.md#ref-wai-aria)], section 5.4).

The role value is a set of white-space separated machine-extractable semantic information used to define the purpose of the element.

The [initial value](types.md#TermInitialValue) for the ‘[role](#RoleAttribute)’ attribute, for each SVG element, is the corresponding [default implied ARIA semantic for SVG elements](#implicit-aria-semantics).

To be valid and useful, many element roles require additional information to be provided in the form of an accessible name or explicit [state and property values](#ARIAStateandPropertyAttributes). Accessible names may be provided using SVG [descriptive elements](#TermDescriptiveElement) or [ARIA attributes](#TermARIAAttribute). The requirements for each role are indicated where the role is defined, e.g., in WAI-ARIA ([[WAI-ARIA](refs.md#ref-wai-aria)]) or the WAI-ARIA Graphics Module ([[graphics-aria-1.0](refs.md#ref-graphics-aria-1.0)]).

### <a id="ARIAStateandPropertyAttributes"></a>5.12.3. State and property attributes (all aria- attributes)

WAI-ARIA state and property attributes may be specified on SVG elements. These attributes are defined by ARIA in Definitions of States and Properties (all aria-* attributes) ([[wai-aria](refs.md#ref-wai-aria)], section 6.6).

These attributes, if specified, must have a value that is the WAI-ARIA value type in the "Value" field of the definition for the state or property, mapped to the appropriate SVG value type according to Mapping WAI-ARIA Value types to languages using the SVG mapping ([[wai-aria](refs.md#ref-wai-aria)], section 10.2).

The attributes are animatable; if animation is used to change the state of the graphic, or to change its content in a way that alters the correct alternative text description, the same method of animation should be used to update the corresponding ARIA state or property attribute.

WAI-ARIA State and Property attributes can be used on any element. They are not always meaningful, however, and in such cases user agents might not perform any processing aside from including them in the DOM. State and property attributes are processed according to the ARIA and SVG Accessibility API Mappings specification specifications. [[wai-aria](refs.md#ref-wai-aria)] [[svg-aam-1.0](refs.md#ref-svg-aam-1.0)]

### <a id="implicit-aria-semantics"></a>5.12.4. Implicit and Allowed ARIA Semantics

The following table defines the default implicit ARIA semantics that apply to [SVG elements](eltindex.md). Each language feature (element) in a cell in the first column implies the ARIA semantics (role, states, and/or properties) given in the cell in the second column of the same row. The third column defines restrictions as to what WAI-ARIA semantic (role, state, or property) may or may not apply.

For many graphics elements, an implicit role is only assigned if the author provides information that indicates semantic importance. The complete *<a id="TermInclusionCriteria"></a>inclusion criteria* for the accessibility tree are defined by the SVG Accessibility API Mappings specification for user agents [[svg-aam-1.0](refs.md#ref-svg-aam-1.0)]. For authors, the preferred means of indicating semantic importance is to provide an accessible name for the element. This can be done through a direct child ‘[title](#TitleElement)’ element, or through the ‘aria-label’ or ‘aria-labelledby’ attributes. Authors should use one of these methods to provide an accessible name for any content that is essential to the comprehension of the SVG, and especially for any interactive content.

<a id="table-aria-implicit"></a>

| Language feature | Default implied ARIA semantics | Allowed roles |
| --- | --- | --- |
| ‘[a](linking.md#AElement)’ | `link` role if the element has a valid `href` or `xlink:href` attribute. For `a` elements that are not links, the default semantics are the same as `tspan` if the `a` element is a descendent of `text`, or the same as `g` otherwise. | no restrictions |
| ‘[circle](shapes.md#CircleElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[clipPath](../css-masking-1.md#ClipPathElement)’ | none | no role may be applied |
| ‘[defs](#DefsElement)’ | none | no role may be applied |
| ‘[desc](#DescElement)’ | none | no role may be applied |
| ‘[ellipse](shapes.md#EllipseElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[feBlend](../filter-effects-1.md#feBlendElement)’ | none | no role may be applied |
| ‘[feColorMatrix](../filter-effects-1.md#feColorMatrixElement)’ | none | no role may be applied |
| ‘[feComponentTransfer](../filter-effects-1.md#feComponentTransferElement)’ | none | no role may be applied |
| ‘[feComposite](../filter-effects-1.md#feCompositeElement)’ | none | no role may be applied |
| ‘[feConvolveMatrix](../filter-effects-1.md#feConvolveMatrixElement)’ | none | no role may be applied |
| ‘[feDiffuseLighting](../filter-effects-1.md#feDiffuseLightingElement)’ | none | no role may be applied |
| ‘[feDisplacementMap](../filter-effects-1.md#feDisplacementMapElement)’ | none | no role may be applied |
| ‘[feDistantLight](../filter-effects-1.md#feDistantLightElement)’ | none | no role may be applied |
| ‘[feDropShadow](../filter-effects-1.md#feDropShadowElement)’ | none | no role may be applied |
| ‘[feFlood](../filter-effects-1.md#feFloodElement)’ | none | no role may be applied |
| ‘[feFuncA](../filter-effects-1.md#feFuncAElement)’ | none | no role may be applied |
| ‘[feFuncB](../filter-effects-1.md#feFuncBElement)’ | none | no role may be applied |
| ‘[feFuncG](../filter-effects-1.md#feFuncGElement)’ | none | no role may be applied |
| ‘[feFuncR](../filter-effects-1.md#feFuncRElement)’ | none | no role may be applied |
| ‘[feGaussianBlur](../filter-effects-1.md#feGaussianBlurElement)’ | none | no role may be applied |
| ‘[feImage](../filter-effects-1.md#feImageElement)’ | none | no role may be applied |
| ‘[feMerge](../filter-effects-1.md#feMergeElement)’ | none | no role may be applied |
| ‘[feMergeNode](../filter-effects-1.md#feMergeElement)’ | none | no role may be applied |
| ‘[feMorphology](../filter-effects-1.md#feMorphologyElement)’ | none | no role may be applied |
| ‘[feOffset](../filter-effects-1.md#feOffsetElement)’ | none | no role may be applied |
| ‘[fePointLight](../filter-effects-1.md#fePointLightElement)’ | none | no role may be applied |
| ‘[feSpecularLighting](../filter-effects-1.md#feSpecularLightingElement)’ | none | no role may be applied |
| ‘[feSpotLight](../filter-effects-1.md#feSpotLightElement)’ | none | no role may be applied |
| ‘[feTile](../filter-effects-1.md#feTileElement)’ | none | no role may be applied |
| ‘[feTurbulence](../filter-effects-1.md#feTurbulenceElement)’ | none | no role may be applied |
| ‘[filter](../filter-effects-1.md#FilterElement)’ | none | no role may be applied |
| ‘[foreignObject](embedded.md#ForeignObjectElement)’ | `group` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[g](#GElement)’ | `group` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[image](embedded.md#ImageElement)’ | `img` role | no restrictions |
| ‘[line](shapes.md#LineElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[linearGradient](pservers.md#LinearGradientElement)’ | none | no role may be applied |
| ‘[marker](painting.md#MarkerElement)’ | none | no role may be applied |
| ‘[mask](../css-masking-1.md#MaskElement)’ | none | no role may be applied |
| ‘[metadata](#MetadataElement)’ | none | no role may be applied |
| ‘mpath’ | none | no role may be applied |
| ‘[path](paths.md#PathElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[pattern](pservers.md#PatternElement)’ | none | no role may be applied |
| ‘[polygon](shapes.md#PolygonElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[polyline](shapes.md#PolylineElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[radialGradient](pservers.md#RadialGradientElement)’ | none | no role may be applied |
| ‘[rect](shapes.md#RectElement)’ | `graphics-symbol` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[script](interact.md#ScriptElement)’ | none | no role may be applied |
| ‘[stop](pservers.md#StopElement)’ | none | no role may be applied |
| ‘[style](styling.md#StyleElement)’ | none | no role may be applied |
| ‘[svg](#SVGElement)’ | `graphics-document` role | no restrictions |
| ‘[switch](#SwitchElement)’ | none | no role may be applied |
| ‘[symbol](#SymbolElement)’ | `graphics-object` role if the element is a rendered [element instance](#TermElementInstance) that meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[text](text.md#TextElement)’ | `group`role, with platform-specific role mappings, as defined in the SVG Accessibility API Mappings specification | no restrictions |
| ‘[textPath](text.md#TextPathElement)’ | `group` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[title](#TitleElement)’ | none | no role may be applied |
| ‘[tspan](text.md#TextElement)’ | `group` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[use](#UseElement)’ | `graphics-object` role if the element meets the [inclusion criteria](#TermInclusionCriteria), otherwise `none` | no restrictions |
| ‘[view](linking.md#ViewElement)’ | none | no role may be applied |

## <a id="DOMInterfaces"></a>5.13. DOM interfaces

### <a id="InterfaceDocumentExtensions"></a>5.13.1. Extensions to the Document interface

The DOM Core specification defines a Document interface, which this specification extends.

In the case where an SVG document is embedded by reference, such as when an HTML document has an ‘object’ element whose ‘data’ attribute references an SVG document (i.e., a document whose MIME type is "image/svg+xml" and whose root element is thus an ‘[svg](#SVGElement)’ element), there will exist two distinct DOM hierarchies. The first DOM hierarchy will be for the referencing document (e.g., an XHTML document). The second DOM hierarchy will be for the referenced SVG document.

```
partial interface Document {
  readonly attribute SVGSVGElement? rootElement;
};
```

The **<a id="__svg__SVGDocument__rootElement"></a>rootElement** IDL attribute represents the root ‘[svg](#SVGElement)’ element. On getting [rootElement](#__svg__SVGDocument__rootElement), the root element of the document is returned, if it is an ‘[svg](#SVGElement)’ element, or null otherwise.

This attribute is deprecated, and may be removed in a future SVG specification. Authors are encouraged to use the documentElement attribute on Document instead.

SVG implementations that implement HTML must support the HTML extensions to the document interface. Other SVG implementations must support the following IDL fragment.

```
// must only be implemented in certain implementations
partial interface Document {
  readonly attribute DOMString title;
  readonly attribute DOMString referrer;
  readonly attribute DOMString domain;
  readonly attribute Element? activeElement;
};
```

The title, referrer, domain and <a id="__svg__SVGDocument__activeElement"></a>activeElement IDL attributes must behave the same as the corresponding IDL attributes defined in HTML.

### <a id="InterfaceSVGSVGElement"></a>5.13.2. Interface SVGSVGElement

An [SVGSVGElement](#InterfaceSVGSVGElement) object represents an ‘[svg](#SVGElement)’ element in the DOM. The [SVGSVGElement](#InterfaceSVGSVGElement) interface also contains miscellaneous utility methods, such as data type object factory methods.

An [SVGSVGElement](#InterfaceSVGSVGElement) object maintains an internal DOMPoint object, called its *<a id="CurrentTranslatePointObject"></a>current translate point object*, which is the object returned from the [currentTranslate](#__svg__SVGSVGElement__currentTranslate) IDL attribute.

```
[Exposed=Window]
interface SVGSVGElement : SVGGraphicsElement {

  [SameObject] readonly attribute SVGAnimatedLength x;
  [SameObject] readonly attribute SVGAnimatedLength y;
  [SameObject] readonly attribute SVGAnimatedLength width;
  [SameObject] readonly attribute SVGAnimatedLength height;

  attribute float currentScale;
  [SameObject] readonly attribute DOMPointReadOnly currentTranslate;

  NodeList getIntersectionList(DOMRectReadOnly rect, SVGElement? referenceElement);
  NodeList getEnclosureList(DOMRectReadOnly rect, SVGElement? referenceElement);
  boolean checkIntersection(SVGElement element, DOMRectReadOnly rect);
  boolean checkEnclosure(SVGElement element, DOMRectReadOnly rect);

  undefined deselectAll();

  [NewObject] SVGNumber createSVGNumber();
  [NewObject] SVGLength createSVGLength();
  [NewObject] SVGAngle createSVGAngle();
  [NewObject] DOMPoint createSVGPoint();
  [NewObject] DOMMatrix createSVGMatrix();
  [NewObject] DOMRect createSVGRect();
  [NewObject] SVGTransform createSVGTransform();
  [NewObject] SVGTransform createSVGTransformFromMatrix(optional DOMMatrix2DInit matrix);

  Element? getElementById(DOMString elementId);

  // Deprecated methods that have no effect when called,
  // but which are kept for compatibility reasons.
  unsigned long suspendRedraw(unsigned long maxWaitMilliseconds);
  undefined unsuspendRedraw(unsigned long suspendHandleID);
  undefined unsuspendRedrawAll();
  undefined forceRedraw();
};

SVGSVGElement includes SVGFitToViewBox;
SVGSVGElement includes WindowEventHandlers;
```

The **<a id="__svg__SVGSVGElement__x"></a>x**, **<a id="__svg__SVGSVGElement__y"></a>y**, **<a id="__svg__SVGSVGElement__width"></a>width** and **<a id="__svg__SVGSVGElement__height"></a>height** IDL attributes [reflect](types.md#TermReflect) the computed values of the [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties and their corresponding presentation attributes, respectively.

The **<a id="__svg__SVGSVGElement__currentScale"></a>currentScale** and **<a id="__svg__SVGSVGElement__currentTranslate"></a>currentTranslate** IDL attributes represent an additional transform applied to the SVG. They only have an effect on the [outermost svg element](#TermOutermostSVGElement) of an [SVG document fragment](#TermSVGDocumentFragment).

The document's magnification and panning transform is a 2x3 matrix of the form `[currentScale 0 0 currentScale currentTranslate.x currentTranslate.y]`. The value of the ‘[transform](coords.md#TransformProperty)’ property does not affect [currentScale](#__svg__SVGSVGElement__currentScale) or [currentTranslate](#__svg__SVGSVGElement__currentTranslate).

Previous versions of SVG recommended that user agents implement controls, by default, for the user to set the scale (zoom) and translate (pan) of the graphic. Transformations from these user actions *would* be reflected in the values of `currentScale` and `currentTranslate`. These user controls were not well implemented, and are no longer recommended. However, authors should be aware that the user agent may update these values.

The obsolete attribute `zoomAndPan="disable"`, on the outermost SVG element, should disable any user agent manipulation of the values based on user action, but may have unwanted side effects in some user agents.

<a id="issue360"></a> The interaction of currentScale and currentTranslate with other ways of transforming the document root (transforms and SVG views) is poorly defined. See the GitHub issue for more.

On getting [currentScale](#__svg__SVGSVGElement__currentScale), the following steps are run:

1. If the current ‘[svg](#SVGElement)’ element is not the [outermost svg element](#TermOutermostSVGElement), then return 1.
2. Let [*a* *b* *c* *d* *e* *f*] be the 2x3 matrix that represents the document's magnification and panning transform.
3. Return *a*.

On setting [currentScale](#__svg__SVGSVGElement__currentScale), the following steps are run:

1. If the current ‘[svg](#SVGElement)’ element is not the [outermost svg element](#TermOutermostSVGElement), then return.
2. Let *scale* be the value being assigned to [currentScale](#__svg__SVGSVGElement__currentScale).
3. Let [*a* *b* *c* *d* *e* *f*] be the 2x3 matrix that represents the document's magnification and panning transform.
4. Set the document's magnification and panning transform to [*scale* 0 0 *scale* *e* *f*].

On getting [currentTranslate](#__svg__SVGSVGElement__currentTranslate), the [SVGSVGElement](#InterfaceSVGSVGElement) object's [current translate point object](#CurrentTranslatePointObject) is returned. This object [represents the current translation](shapes.md#PointMode) for the ‘[svg](#SVGElement)’ element. A [current translate point object](#CurrentTranslatePointObject) must be [read only](shapes.md#ReadOnlyPoint) when its ‘[svg](#SVGElement)’ element is not the [outermost svg element](#TermOutermostSVGElement), and writable otherwise.

See the rules for [assigning to a DOMPoint](shapes.md#AssignToDOMPoint) for how modifying the [current translate point object](#CurrentTranslatePointObject) affects the document's magnification and panning transform.

Whenever the document's magnification and panning transform changes in response to user interaction or whenever the [outermost svg element](#TermOutermostSVGElement) changes, the following steps are run:

1. Let [*a* *b* *c* *d* *e* *f*] be the 2x3 matrix that represents the document's magnification and panning transform.
2. Let *element* be the [outermost svg element](#TermOutermostSVGElement).
3. Update the x and y components of *element*'s [current translate point object](#CurrentTranslatePointObject) to *e* and *f*, respectively.

Running these steps when the [outermost svg element](#TermOutermostSVGElement) changes will ensure that if the document element is replaced with a different ‘[svg](#SVGElement)’ element, that its [currentTranslate](#__svg__SVGSVGElement__currentTranslate) will be immediately updated to reflect the translation component of the document's magnification and panning transform.

Whenever an ‘[svg](#SVGElement)’ element is no longer [outermost svg element](#TermOutermostSVGElement), the x and y components of its [current translate point object](#CurrentTranslatePointObject) must be set to 0.

The **<a id="__svg__SVGSVGElement__suspendRedraw"></a>suspendRedraw**, **<a id="__svg__SVGSVGElement__unsuspendRedraw"></a>unsuspendRedraw**, **<a id="__svg__SVGSVGElement__unsuspendRedrawAll"></a>unsuspendRedrawAll** and **<a id="__svg__SVGSVGElement__forceRedraw"></a>forceRedraw** methods are all deprecated and defined to have no effect. When the suspendRedraw method is called, it must return 1.

The **<a id="__svg__SVGSVGElement__getIntersectionList"></a>getIntersectionList**, **<a id="__svg__SVGSVGElement__getEnclosureList"></a>getEnclosureList**, **<a id="__svg__SVGSVGElement__checkIntersection"></a>checkIntersection** and **<a id="__svg__SVGSVGElement__checkEnclosure"></a>checkEnclosure** methods are used to perform geometry operations on [graphics elements](#TermGraphicsElement) to find those whose (or check whether their) graphical content lies partially or completely within a given rectangle.

To *<a id="TermFindIntersectingOrEnclosedDescendants"></a>find the intersecting or enclosed descendants* of a given element *element* with a given rectangle *rectangle* using *ancestor* as the element in whose coordinate space *rectangle* is to be interpreted, the following steps are run:

1. Let *result* be an initially empty list.
2. If *element* is not displayed, due to having a [display](render.md#VisibilityControl) value of none or being in a subtree that has failing [conditional processing attributes](#TermConditionalProcessingAttribute) or a failing branch of a ‘[switch](#SwitchElement)’, then return *result*.
3. For each child element *child* of *element*, in document order:
  1. If *child* is an ‘[svg](#SVGElement)’ or ‘[g](#GElement)’ element, then:
    1. Let *descendants* be the result of [finding the intersecting (or enclosed) descendants](#TermFindIntersectingOrEnclosedDescendants) of *child* with *rectangle* in *ancestor*'s coordinate space.
    2. Append to *result* all the elements of *descendants*.
  2. Otherwise, if *child* is a ‘[use](#UseElement)’ element, then:
    1. Let *root* be the root of the *child*'s shadow tree.
    2. Let *descendants* be the result of [finding the intersecting (or enclosed) descendants](#TermFindIntersectingOrEnclosedDescendants) of *root* with *rectangle* in *ancestor*'s coordinate space.
    3. If *descendants* is not empty, then append *child* to *result*.
    This means that although we look at the elements in the [use-element shadow tree](#TermUseElementShadowTree), we don't place the [element instances](#TermElementInstance) or their [corresponding element](#TermCorrespondingElement) in the *result* list; only the ‘[use](#UseElement)’ element itself is returned.
  3. Otherwise, if *child* is a [graphics element](#TermGraphicsElement), then:
    1. Let *region* be the shape in *child*'s coordinate system that is sensitive to hit detection, taking into account the rules for interpreting *child*'s [pointer-events](interact.md#PointerEventsProperty) value.
    2. Transform *region* into *ancestor*'s coordinate system.
    3. If we are finding intersecting descendants and *region* lies partially or fully within *rectangle*, then append *child* to *result*.
    4. Otherwise, we are finding enclosed descendants. If *region* lies fully within *rectangle*, then append *child* to *result*.
4. Return *result*.

To *<a id="TermFindNonContainerGraphicsElements"></a>find the non-container graphics elements* within a given element *element*, the following steps are run:

1. Let *result* be an initially empty list.
2. If *element* is an ‘[svg](#SVGElement)’ or ‘[g](#GElement)’ element, then for each child element *child* of *element*, in document order:
  1. Let *descendants* be the result of [finding the non-container graphics elements](#TermFindNonContainerGraphicsElements) within *child*.
  2. Append to *result* all the elements of *descendants*.
3. Otherwise, if *element* is a [graphics element](#TermGraphicsElement) then append *element* to *result*.
4. Return *result*.

When getIntersectionList(*rect*, *referenceElement*) or getEnclosureList(*rect*, *referenceElement*) is called, the following steps are run:

1. Let *descendants* be a list, depending on what method we are in:
**[getIntersectionList](#__svg__SVGSVGElement__getIntersectionList)**
: *descendants* is the result of [finding the intersecting descendants](#TermFindIntersectingOrEnclosedDescendants) of the current ‘[svg](#SVGElement)’ element with rectangle *rect* in the current ‘[svg](#SVGElement)’ element's coordinate system.
**[getEnclosureList](#__svg__SVGSVGElement__getEnclosureList)**
: *descendants* is the result of [finding the enclosed descendants](#TermFindIntersectingOrEnclosedDescendants) of the current ‘[svg](#SVGElement)’ element with rectangle *rect* in the current ‘[svg](#SVGElement)’ element's coordinate system.
2. If *referenceElement* is not null, then remove from *descendants* any element that does not have *referenceElement* as an ancestor.
3. Return a static NodeList that contains all of the elements in *descendants*. ([[DOM](refs.md#ref-dom)], section 5.2.7)

When checkIntersection(*element*, *rect*) or checkEnclosure(*element*, *rect*) is called, the following steps are run:

1. Let *descendants* be a list, depending on what method we are in:
**[getIntersectionList](#__svg__SVGSVGElement__getIntersectionList)**
: *descendants* is the result of [finding the intersecting descendants](#TermFindIntersectingOrEnclosedDescendants) of the current ‘[svg](#SVGElement)’ element with rectangle *rect* in the current ‘[svg](#SVGElement)’ element's coordinate system.
**[getEnclosureList](#__svg__SVGSVGElement__getEnclosureList)**
: *descendants* is the result of [finding the enclosed descendants](#TermFindIntersectingOrEnclosedDescendants) of the current ‘[svg](#SVGElement)’ element with rectangle *rect* in the current ‘[svg](#SVGElement)’ element's coordinate system.
2. Let *elements* be the result of [finding the non-container graphics elements](#TermFindNonContainerGraphicsElements) within *element*.
3. If *elements* is empty, then return false.
4. If any element in *elements* is not also in *descendants*, then return false.
5. Return true.

The **<a id="__svg__SVGSVGElement__deselectAll"></a>deselectAll** method is used to remove any selections from the document. When deselectAll() is called, all ranges from the document's selection are removed and the selection's direction is set to forwards. [[DOM](refs.md#ref-dom)][[EDITING](refs.md#ref-editing)] This method is deprecated, as it duplicates functionality from the Selection API.

This is equivalent to calling `document.getSelection().removeAllRanges()` on the document that this ‘[svg](#SVGElement)’ element is in.

The **<a id="__svg__SVGSVGElement__createSVGNumber"></a>createSVGNumber**, **<a id="__svg__SVGSVGElement__createSVGLength"></a>createSVGLength**, **<a id="__svg__SVGSVGElement__createSVGAngle"></a>createSVGAngle**, **<a id="__svg__SVGSVGElement__createSVGPoint"></a>createSVGPoint**, **<a id="__svg__SVGSVGElement__createSVGMatrix"></a>createSVGMatrix**, **<a id="__svg__SVGSVGElement__createSVGRect"></a>createSVGRect** and **<a id="__svg__SVGSVGElement__createSVGTransform"></a>createSVGTransform** methods are all factory functions used to create a new datatype object of a particular type. When one of these methods is called, a new object is returned according to the following table:

| Method | Object and details |
| --- | --- |
| [createSVGNumber](#__svg__SVGSVGElement__createSVGNumber) | A new, [detached](types.md#NumberMode) [SVGNumber](types.md#InterfaceSVGNumber) object whose value is 0. |
| [createSVGLength](#__svg__SVGSVGElement__createSVGLength) | A new, [detached](types.md#LengthMode) [SVGLength](types.md#InterfaceSVGLength) object whose value is the unitless <number> 0. |
| [createSVGAngle](#__svg__SVGSVGElement__createSVGAngle) | A new, [detached](types.md#AngleMode) [SVGAngle](types.md#InterfaceSVGAngle) object whose value is the unitless <number> 0. |
| [createSVGPoint](#__svg__SVGSVGElement__createSVGPoint) | A new, [detached](shapes.md#PointMode) DOMPoint object whose coordinates are all 0. |
| [createSVGMatrix](#__svg__SVGSVGElement__createSVGMatrix) | A new, [detached](coords.md#MatrixMode) DOMMatrix object representing the identity matrix. |
| [createSVGRect](#__svg__SVGSVGElement__createSVGRect) | A new, DOMRect object whose x, y, width and height are all 0. |
| [createSVGTransform](#__svg__SVGSVGElement__createSVGTransform) | A new, [detached](coords.md#TransformMode) [SVGTransform](coords.md#InterfaceSVGTransform) object whose value is matrix(1, 0, 0, 1, 0, 0). |

The [createSVGPoint](#__svg__SVGSVGElement__createSVGPoint), [createSVGMatrix](#__svg__SVGSVGElement__createSVGMatrix) and [createSVGRect](#__svg__SVGSVGElement__createSVGRect) methods are all deprecated and kept only for compatibility with legacy content. Authors are encouraged to use the DOMPoint, DOMMatrix and DOMRect constructors instead.

The **<a id="__svg__SVGSVGElement__createSVGTransformFromMatrix"></a>createSVGTransformFromMatrix** method is used to create a new [SVGTransform](coords.md#InterfaceSVGTransform) object from a matrix object. Its behavior is the same as the [createSVGTransformFromMatrix](coords.md#__svg__SVGTransformList__createSVGTransformFromMatrix) method on [SVGTransformList](coords.md#InterfaceSVGTransformList).

The **<a id="__svg__SVGSVGElement__getElementById"></a>getElementById** method, must return the first element in tree order, within the ‘[svg](#SVGElement)’ element's descendants, whose ID is *elementId*, or null if there is no such element.

### <a id="InterfaceSVGGElement"></a>5.13.3. Interface SVGGElement

An [SVGGElement](#InterfaceSVGGElement) object represents a ‘[g](#GElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGGElement : SVGGraphicsElement {
};
```

### <a id="InterfaceSVGDefsElement"></a>5.13.4. Interface SVGDefsElement

An [SVGDefsElement](#InterfaceSVGDefsElement) object represents a ‘[defs](#DefsElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGDefsElement : SVGGraphicsElement {
};
```

### <a id="InterfaceSVGDescElement"></a>5.13.5. Interface SVGDescElement

An [SVGDescElement](#InterfaceSVGDescElement) object represents a ‘[desc](#DescElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGDescElement : SVGElement {
};
```

### <a id="InterfaceSVGMetadataElement"></a>5.13.6. Interface SVGMetadataElement

An [SVGMetadataElement](#InterfaceSVGMetadataElement) object represents a ‘[metadata](#MetadataElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGMetadataElement : SVGElement {
};
```

### <a id="InterfaceSVGTitleElement"></a>5.13.7. Interface SVGTitleElement

An [SVGTitleElement](#InterfaceSVGTitleElement) object represents a ‘[title](#TitleElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGTitleElement : SVGElement {
};
```

### <a id="InterfaceSVGSymbolElement"></a>5.13.8. Interface SVGSymbolElement

An [SVGSymbolElement](#InterfaceSVGSymbolElement) object represents a ‘[symbol](#SymbolElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGSymbolElement : SVGGraphicsElement {
};

SVGSymbolElement includes SVGFitToViewBox;
```

New in SVG 2. The [SVGSymbolElement](#InterfaceSVGSymbolElement) interface now inherits from [SVGGraphicsElement](types.md#InterfaceSVGGraphicsElement), so that the instantiated symbol in the shadow DOM can be queried as a graphics element.

### <a id="InterfaceSVGUseElement"></a>5.13.9. Interface SVGUseElement

An [SVGUseElement](#InterfaceSVGUseElement) object represents a ‘[use](#UseElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGUseElement : SVGGraphicsElement {
  [SameObject] readonly attribute SVGAnimatedLength x;
  [SameObject] readonly attribute SVGAnimatedLength y;
  [SameObject] readonly attribute SVGAnimatedLength width;
  [SameObject] readonly attribute SVGAnimatedLength height;
  [SameObject] readonly attribute SVGElement? instanceRoot;
  [SameObject] readonly attribute SVGElement? animatedInstanceRoot;
};

SVGUseElement includes SVGURIReference;
```

The **<a id="__svg__SVGUseElement__x"></a>x**, **<a id="__svg__SVGUseElement__y"></a>y**, **<a id="__svg__SVGUseElement__width"></a>width** and **<a id="__svg__SVGUseElement__height"></a>height** IDL attributes [reflect](types.md#TermReflect) the computed values of the [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties and their corresponding presentation attributes, respectively.

The **<a id="__svg__SVGUseElement__instanceRoot"></a>instanceRoot** and **<a id="__svg__SVGUseElement__animatedInstanceRoot"></a>animatedInstanceRoot** IDL attributes both point to the [instance root](#TermInstanceRoot), the [SVGElementInstance](#InterfaceSVGElementInstance) that is a direct child of this element's shadow root (`u.instanceRoot` is equivalent to getting `u.shadowRoot.firstChild`). If this element does not have a shadow tree (for example, because its URI is invalid or because it has been disabled by [conditional processing](#ConditionalProcessing)), then getting these attributes returns null.

### <a id="InterfaceSVGUseElementShadowRoot"></a>5.13.10. Interface SVGUseElementShadowRoot

The root object of each [use-element shadow tree](#TermUseElementShadowTree) implements the [SVGUseElementShadowRoot](#InterfaceSVGUseElementShadowRoot) interface. This interface does not currently define any extensions to the properties and methods defined for the ShadowRoot interface and DocumentOrShadowRoot mixin. However, the tree rooted at this node is entirely read-only from the perspective of author scripts.

```
[Exposed=Window]
interface SVGUseElementShadowRoot : ShadowRoot {
};
```

### <a id="InterfaceSVGElementInstance"></a>5.13.11. Mixin SVGElementInstance

The [SVGElementInstance](#InterfaceSVGElementInstance) interface defines extensions to the [SVGElement](types.md#InterfaceSVGElement) interface, which are only used for elements in a [use-element shadow tree](#TermUseElementShadowTree).

In previous versions of SVG, SVG element instances were defined as non-element objects that were valid event targets but not full DOM nodes. This specification re-defines the [use-element shadow tree](#TermUseElementShadowTree) to be consistent with the Shadow DOM specification, which means that instances are actual SVGElement objects. This interface adds the missing functionality for backwards compatibility. However, authors should be aware that compatibility is not perfect, and design their scripts accordingly. Also note that these properties will not be available on HTML-namespaced element objects in the shadow tree.

```
interface mixin SVGElementInstance {
  [SameObject] readonly attribute SVGElement? correspondingElement;
  [SameObject] readonly attribute SVGUseElement? correspondingUseElement;
};
```

The **<a id="__svg__SVGElementInstance__correspondingElement"></a>correspondingElement** IDL attribute points to the [corresponding element](#TermCorrespondingElement) if this element is an [element instance](#TermElementInstance) in a [use-element shadow tree](#TermUseElementShadowTree), or is null otherwise.

<a id="issue71"></a> When the [referenced element](#TermReferencedElement) is in an external file, the presence of this pointer implies that the entire DOM of the external file must be maintained in memory. However, as currently specified, the external DOM is read-only. It therefore offers limited functionality and a potentially large performance impact. Pending feedback from implementers, authors should consider the use of `correspondingElement` with external file references to be at-risk.

The **<a id="__svg__SVGElementInstance__correspondingUseElement"></a>correspondingUseElement** IDL attribute points to the [corresponding use element](#TermCorrespondingUseElement) if this element is an [element instance](#TermElementInstance) in a [use-element shadow tree](#TermUseElementShadowTree), or is null otherwise.

### <a id="InterfaceShadowAnimation"></a>5.13.12. Interface ShadowAnimation

The [ShadowAnimation](#InterfaceShadowAnimation) interface defines a read-only Animation object, which mirrors all changes to the [sourceAnimation](#__svg__ShadowAnimation__sourceAnimation) object from which it was constructed. They are used to mirror author-initiated animation objects in the [use-element shadow tree](#TermUseElementShadowTree).

```
[Constructor(Animation source, Animatable newTarget), Exposed=Window]
interface ShadowAnimation : Animation {
  [SameObject] readonly attribute Animation sourceAnimation;
};
```

The **<a id="__svg__ShadowAnimation__sourceAnimation"></a>sourceAnimation** IDL property points to the Animation object passed in the constructor.

The constructor generates a new [ShadowAnimation](#InterfaceShadowAnimation) object, which reflects all properties on the [sourceAnimation](#__svg__ShadowAnimation__sourceAnimation), except that its `effect` is created by constructing a new `KeyframeEffectReadOnly` using the keyframe effect of the [sourceAnimation](#__svg__ShadowAnimation__sourceAnimation) as its source, and then modifying its `target` to match the `newTarget` parameter.

A [ShadowAnimation](#InterfaceShadowAnimation) is read-only. Any attempt to set any of the inherited IDL properties, or call any of the Animation methods that change its state, must throw a `NoModificationAllowedError`. However, the user agent must ensure that any changes to the properties or state of the [sourceAnimation](#__svg__ShadowAnimation__sourceAnimation) are reflected in changes to the [ShadowAnimation](#InterfaceShadowAnimation).

### <a id="InterfaceSVGSwitchElement"></a>5.13.13. Interface SVGSwitchElement

An [SVGSwitchElement](#InterfaceSVGSwitchElement) object represents a ‘[switch](#SwitchElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGSwitchElement : SVGGraphicsElement {
};
```

### <a id="InterfaceGetSVGDocument"></a>5.13.14. Mixin GetSVGDocument

This interface provides access to an SVG document embedded by reference in another DOM-based language. The expectation is that the interface is implemented on DOM objects that allow such SVG document references.

This interface is deprecated and may be dropped from future versions of the SVG specification. To access the SVG document inside an ‘iframe’ or ‘object’ element, authors are suggested to use the `contentDocument` attribute on the HTMLIFrameElement or HTMLObjectElement interface, respectively.

The HTMLIFrameElement, HTMLEmbedElement and HTMLObjectElement interfaces all define their own getSVGDocument method, which provides access to the SVG document in the same way that the [GetSVGDocument](#InterfaceGetSVGDocument) does. Those three interfaces therefore do not need to implement [GetSVGDocument](#InterfaceGetSVGDocument). Still, authors are strongly recommended to use contentDocument instead.

```
interface mixin GetSVGDocument {
  Document getSVGDocument();
};
```

The **<a id="__svg__GetSVGDocument__getSVGDocument"></a>getSVGDocument** method is used to return a referenced SVG document. When getSVGDocument() is called, it must return the Document object referenced by the embedding element that implements the [GetSVGDocument](#InterfaceGetSVGDocument) interface; if there is no document, null is returned.

Note that this does no check to see whether the referenced document is indeed an SVG document. Instead, any document is returned.
