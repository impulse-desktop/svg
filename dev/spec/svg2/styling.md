<!-- https://svgwg.org/svg2-draft/styling.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 6: Styling

## <a id="StylingUsingCSS"></a>6.1. Styling SVG content using CSS

Elements in an SVG document can be styled using CSS. Most visual characteristics and some aspects of element geometry are controlled using CSS *<a id="TermProperty"></a>properties*. For example, the [fill](painting.md#FillProperty) property controls the paint used to fill the inside of a shape, and the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties are used to control the size of a ‘[rect](shapes.md#RectElement)’ element.

SVG user agents must support all of the CSS styling mechanisms described in this chapter.

In SVG 1.1, support for inline style sheets using the ‘[style](#StyleElement)’ element and ‘[style](#StyleAttribute)’ was not required. In SVG 2, these are required.

## <a id="StyleElement"></a>6.2. Inline style sheets: the ‘style’ element

The ‘[style](#StyleElement)’ element allows style sheets to be embedded directly within SVG content. SVG's ‘[style](#StyleElement)’ element has the same attributes as the corresponding element in HTML.

‘*<a id="elementdef-style"></a>style*’

**Categories:**
: [Never-rendered element](render.md#TermNeverRenderedElement)
**Content model:**
: Character data.
**Attributes:**
:

- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](#ClassAttribute)’, ‘[style](#StyleAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- ‘[type](#StyleElementTypeAttribute)’
- ‘[media](#StyleElementMediaAttribute)’
- ‘[title](#StyleElementTitleAttribute)’

**DOM Interfaces:**
:

- [SVGStyleElement](#InterfaceSVGStyleElement)

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="StyleElementTypeAttribute"></a>type* | (see below) | text/css | no |

**
:

This attribute specifies the style sheet language of the element's contents, as a media type. [[rfc2046](refs.md#ref-rfc2046)]. If the attribute is not specified, then the style sheet language is assumed to be CSS.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="StyleElementMediaAttribute"></a>media* | (see below) | all | no |

**
:

This attribute specifies a media query that must be matched for the style sheet to apply. Its value is parsed as a media_query_list. If not specified, the style sheet applies unconditionally.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="StyleElementTitleAttribute"></a>title* | (see below) | (none) | no |

**
:

This attribute specifies a title for the style sheet, which is used when exposing and selecting between alternate style sheets. The attribute takes any value.

The semantics and processing of a ‘[style](#StyleElement)’ and its attributes must be the same as is defined for the HTML ‘style’ element.

The style sheet's text content is never directly rendered; the [display](render.md#VisibilityControl) value for the ‘[style](#StyleElement)’ element must always be set to none by the [user agent style sheet](#UAStyleSheet), and this declaration must have importance over any other CSS rule or presentation attribute.

## <a id="LinkElement"></a>6.3. External style sheets: the effect of the HTML ‘link’ element

An HTML ‘link’ element in an SVG document (that is, an element in the HTML namespace with local name "link") with its ‘rel’ attribute set to 'stylesheet' must be processed as defined in the HTML specification and cause external style sheets to be loaded and applied to the document. Such elements in HTML documents outside of an inline SVG fragment must also apply to the SVG content.

Because the element is required to be in the HTML namespace, it is not possible for an HTML ‘link’ element to be parsed as part of an inline SVG fragment in a text/html document. However, when parsing an SVG document using XML syntax, XML namespace declarations can be used to place the element in the HTML namespace.

Note that an alternative way to reference external style sheets without using the HTML ‘link’ element is to use an @import rule in an inline style sheet. For example:

```
<svg xmlns="http://www.w3.org/2000/svg">
  <style>
    @import url(mystyles.css);
  </style>
  <rect .../>
</svg>
```

would behave similarly to:

```
<svg xmlns="http://www.w3.org/2000/svg">
  <link xmlns="http://www.w3.org/1999/xhtml" rel="stylesheet" href="mystyles.css" type="text/css"/>
  <rect .../>
</svg>
```

Or, in XML documents, external CSS style sheets may be included using the <?xml-stylesheet?> processing instruction [[xml-stylesheet](refs.md#ref-xml-stylesheet)].

## <a id="StyleSheetsInHTMLDocuments"></a>6.4. Style sheets in HTML documents

When an SVG ‘[style](#StyleElement)’ or an HTML ‘style’ element is used in an HTML document, those style sheets must apply to all HTML and inline SVG content in the document. Similarly, any HTML ‘style’ element used in an SVG document must also apply its style sheet to the document.

## <a id="ElementSpecificStyling"></a>6.5. Element-specific styling: the ‘class’ and ‘style’ attributes

As with HTML, SVG supports the ‘[class](#ClassAttribute)’ and ‘[style](#StyleAttribute)’ attributes on all elements to support element-specific styling.

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="ClassAttribute"></a>class* | set of space-separated tokens [HTML] | (none) | yes |

**
:

The ‘[class](#ClassAttribute)’ attribute assigns one or more class names to an element, which can then be used for addressing by the styling language.

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="StyleAttribute"></a>style* | (see below) | (none) | no |

**
:

The ‘[style](#StyleAttribute)’ attribute is used to supply a CSS declaration of an element. The attribute is parsed as a declaration-list.

Aside from the way that the ‘[class](#ClassAttribute)’ attribute is reflected in the SVG DOM (in the [className](types.md#__svg__SVGElement__className) IDL attribute on [SVGElement](types.md#InterfaceSVGElement)), the semantics and behavior of the ‘[class](#ClassAttribute)’ and ‘[style](#StyleAttribute)’ attributes must be the same as that for the corresponding attributes in HTML.

In the following example, the ‘[text](text.md#TextElement)’ element is used in conjunction with the ‘[class](#ClassAttribute)’ attribute to markup document messages. Messages appear in both English and French versions.

```
<!-- English messages -->
<text class="info" lang="en">Variable declared twice</text>
<text class="warning" lang="en">Undeclared variable</text>
<text class="error" lang="en">Bad syntax for variable name</text>
<!-- French messages -->
<text class="info" lang="fr">Variable déclarée deux fois</text>
<text class="warning" lang="fr">Variable indéfinie</text>
<text class="error" lang="fr">Erreur de syntaxe pour variable</text>
```

The following CSS style rules would tell visual user agents to display informational messages in green, warning messages in yellow, and error messages in red:

```
text.info    { fill: green; }
text.warning { fill: yellow; }
text.error   { fill: red; }
```

This example shows how the ‘[style](#StyleAttribute)’ attribute can be used to style ‘[text](text.md#TextElement)’ elements similarly to the previous example:

```
<text style="fill: green;" lang="en">Variable declared twice</text>
<text style="fill: yellow;" lang="en">Undeclared variable</text>
<text style="fill: red;" lang="en">Bad syntax for variable name</text>
```

## <a id="PresentationAttributes"></a>6.6. Presentation attributes

Some styling properties can be specified not only in style sheets and ‘[style](#StyleAttribute)’ attributes, but also in *<a id="TermPresentationAttribute"></a>presentation attributes*. These are attributes whose name matches (or is similar to) a given CSS property and whose value is parsed as a value of that property. Presentation attributes contribute to the author level of the cascade, followed by all other author-level style sheets, and have specificity 0.

Since presentation attributes are parsed as CSS values, not declarations, an !important declaration within a presentation attribute will cause it to have an [invalid value](types.md#TermInvalidValue). See [Attribute syntax](types.md#presentation-attribute-css-value) for details on how presentation attributes are parsed.

Not all style properties that can affect SVG rendering have a corresponding presentation attribute. Other attributes (which happen to share the name of a style property) must not be parsed as a presentation attribute and must not affect CSS cascading and inheritance. Also, only elements in the SVG namespace support presentation attributes. Most SVG presentation attributes may be specified on any element in the SVG namespace where there is not a name clash with an existing attribute. However, the [geometry properties](geometry.md#geometry-properties) only have equivalent presentation attributes on designated elements. Attributes of the same name on other elements must not affect CSS cascading and inheritance.

Except as noted in the table for the [transform](coords.md#TransformProperty) presentation attributes, the presentation attribute name is the same as the property name, in lower-case letters.

| Properties with a presentation attribute | Elements that support the presentation attribute |
| --- | --- |
| [cx](geometry.md#CxProperty), [cy](geometry.md#CyProperty) | ‘[circle](shapes.md#CircleElement)’ and ‘[ellipse](shapes.md#EllipseElement)’ |
| [height](geometry.md#Sizing), [width](geometry.md#Sizing), [x](geometry.md#XProperty), [y](geometry.md#YProperty) | ‘[foreignObject](embedded.md#ForeignObjectElement)’, ‘[image](embedded.md#ImageElement)’, ‘[rect](shapes.md#RectElement)’, ‘[svg](struct.md#SVGElement)’, ‘[symbol](struct.md#SymbolElement)’, and ‘[use](struct.md#UseElement)’ |
| [r](geometry.md#RProperty) | ‘[circle](shapes.md#CircleElement)’ |
| [rx](geometry.md#RxProperty), [ry](geometry.md#RyProperty) | ‘[ellipse](shapes.md#EllipseElement)’ and ‘[rect](shapes.md#RectElement)’ |
| [d](paths.md#DProperty) | ‘[path](paths.md#PathElement)’ |
| [fill](painting.md#FillProperty) | Any element in the SVG namespace except for animation elements, which have a different ‘fill’ attribute. |
| [transform](coords.md#TransformProperty) | For historical reasons, the [transform](coords.md#TransformProperty) property gets represented by different presentation attributes depending on the SVG element it applies to: **[transform](coords.md#TransformProperty)** : Any element in the SVG namespace with the exception of the ‘[pattern](pservers.md#PatternElement)’, ‘[linearGradient](pservers.md#LinearGradientElement)’ and ‘[radialGradient](pservers.md#RadialGradientElement)’ elements. **‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’** : ‘[pattern](pservers.md#PatternElement)’. ‘[patternTransform](pservers.md#PatternElementPatternTransformAttribute)’ gets mapped to the [transform](coords.md#TransformProperty) CSS property [[css-transforms-1](refs.md#ref-css-transforms-1)]. **‘[gradientTransform](pservers.md#LinearGradientElementGradientTransformAttribute)’** : ‘[linearGradient](pservers.md#LinearGradientElement)’ and ‘[radialGradient](pservers.md#RadialGradientElement)’ elements. ‘[gradientTransform](pservers.md#LinearGradientElementGradientTransformAttribute)’ gets mapped to the [transform](coords.md#TransformProperty) CSS property [[css-transforms-1](refs.md#ref-css-transforms-1)]. |
| [alignment-baseline](text.md#AlignmentBaselineProperty), [baseline-shift](text.md#BaselineShiftProperty), [clip-path](../css-masking-1.md#the-clip-path), [clip-rule](../css-masking-1.md#the-clip-rule), [color](painting.md#ColorProperty), [color-interpolation](painting.md#ColorInterpolationProperty), [color-interpolation-filters](../filter-effects-1.md#ColorInterpolationFiltersProperty), cursor, [direction](text.md#DirectionProperty), [display](render.md#VisibilityControl), [dominant-baseline](text.md#DominantBaselineProperty), [fill-opacity](painting.md#FillOpacityProperty), [fill-rule](painting.md#FillRuleProperty), [filter](../filter-effects-1.md#FilterProperty), [flood-color](../filter-effects-1.md#FloodColorProperty), [flood-opacity](../filter-effects-1.md#FloodOpacityProperty), font-family, font-size, font-size-adjust, font-stretch, font-style, font-variant, font-weight, [glyph-orientation-vertical](text.md#GlyphOrientationVerticalProperty), [image-rendering](painting.md#ImageRenderingProperty), letter-spacing, [lighting-color](../filter-effects-1.md#LightingColorProperty), [marker-end](painting.md#MarkerEndProperty), [marker-mid](painting.md#MarkerMidProperty), [marker-start](painting.md#MarkerStartProperty), [mask](../css-masking-1.md#propdef-mask), [mask-type](../css-masking-1.md#propdef-mask-type), [opacity](render.md#ObjectAndGroupOpacityProperties), [overflow](render.md#OverflowAndClipProperties), [paint-order](painting.md#PaintOrderProperty), [pointer-events](interact.md#PointerEventsProperty), [shape-rendering](painting.md#ShapeRenderingProperty), [stop-color](pservers.md#StopColorProperty), [stop-opacity](pservers.md#StopOpacityProperty), [stroke](painting.md#StrokeProperty), [stroke-dasharray](painting.md#StrokeDasharrayProperty), [stroke-dashoffset](painting.md#StrokeDashoffsetProperty), [stroke-linecap](painting.md#StrokeLinecapProperty), [stroke-linejoin](painting.md#StrokeLinejoinProperty), [stroke-miterlimit](painting.md#StrokeMiterlimitProperty), [stroke-opacity](painting.md#StrokeOpacityProperty), [stroke-width](painting.md#StrokeWidthProperty), [text-anchor](text.md#TextAnchorProperty), [text-decoration](text.md#TextDecorationProperties), [text-overflow](text.md#TextOverflowProperty), [text-rendering](painting.md#TextRenderingProperty), [transform-origin](../css-transforms-1.md#transform-origin-property), unicode-bidi, [vector-effect](coords.md#VectorEffectProperty), [visibility](render.md#VisibilityControl), white-space, word-spacing, [writing-mode](text.md#WritingModeProperty) | Any element in the SVG namespace. |

Note that ‘cx’, ‘cy’, ‘r’, ‘x’, ‘y’, ‘width’ and ‘height’ attributes are not always presentation attributes. For example, the ‘[x](text.md#TextElementXAttribute)’ attribute on ‘[text](text.md#TextElement)’ and ‘[tspan](text.md#TextElement)’ is not a presentation attribute for the [x](geometry.md#XProperty) property, and the ‘[r](pservers.md#RadialGradientElementRAttribute)’ attribute on a ‘[radialGradient](pservers.md#RadialGradientElement)’ is not a presentation attribute for the [r](geometry.md#RProperty) property.

In the future, any new properties that apply to SVG content will not gain presentation attributes. Therefore, authors are suggested to use styling properties, either through inline ‘[style](#StyleAttribute)’ properties or style sheets, rather than presentation attributes, for styling SVG content.

Animation of presentation attributes is equivalent to animating the corresponding property.

## <a id="RequiredProperties"></a>6.7. Required properties

The following properties must be supported by all SVG user agents:

- all properties defined in this specification
- the [display](render.md#VisibilityControl), [overflow](render.md#OverflowAndClipProperties) and [visibility](render.md#VisibilityControl) properties [[CSS2](refs.md#ref-css2)]
- the cursor and [text-overflow](text.md#TextOverflowProperty) property [[css-overflow-3](refs.md#ref-css-overflow-3)]
- the [clip-path](../css-masking-1.md#the-clip-path), [clip-rule](../css-masking-1.md#the-clip-rule) and [mask](../css-masking-1.md#propdef-mask) properties [[css-masking-1](refs.md#ref-css-masking-1)]
- the [color](painting.md#ColorProperty) and [opacity](render.md#ObjectAndGroupOpacityProperties) properties [[css-color-3](refs.md#ref-css-color-3)]
- the [color-interpolation-filters](../filter-effects-1.md#ColorInterpolationFiltersProperty), [filter](../filter-effects-1.md#FilterProperty), [flood-color](../filter-effects-1.md#FloodColorProperty), [flood-opacity](../filter-effects-1.md#FloodOpacityProperty) and [lighting-color](../filter-effects-1.md#LightingColorProperty) properties [[filter-effects-1](refs.md#ref-filter-effects-1)]
- the isolation property [[compositing-1](refs.md#ref-compositing-1)]
- the [transform](coords.md#TransformProperty), [transform-box](../css-transforms-1.md#transform-box) and [transform-origin](../css-transforms-1.md#transform-origin-property) properties [[css-transforms-1](refs.md#ref-css-transforms-1)]
- the letter-spacing, text-align, text-align-last, text-indent and word-spacing properties [[css-text-3](refs.md#ref-css-text-3)]
- the white-space property [[css-text-4](refs.md#ref-css-text-4)]
- the vertical-align, [dominant-baseline](text.md#DominantBaselineProperty), [alignment-baseline](text.md#AlignmentBaselineProperty), and [baseline-shift](text.md#BaselineShiftProperty) properties [[css-inline-3](refs.md#ref-css-inline-3)]
- the [direction](text.md#DirectionProperty), text-orientation and [writing-mode](text.md#WritingModeProperty) properties [[css-writing-modes-3](refs.md#ref-css-writing-modes-3)]
- the font, font-family, font-feature-settings, font-kerning, font-size, font-size-adjust, font-stretch, font-style, font-variant (along with its subproperties) and font-weight properties [[css-fonts-3](refs.md#ref-css-fonts-3)]
- the [text-decoration](text.md#TextDecorationProperties), [text-decoration-line](text.md#TextDecorationProperties), [text-decoration-style](text.md#TextDecorationProperties) and [text-decoration-color](text.md#TextDecorationProperties) properties [[css-text-decor-3](refs.md#ref-css-text-decor-3)]

## <a id="UAStyleSheet"></a>6.8. User agent style sheet

The following user agent style sheet must be applied in all SVG user agents.

```
@namespace url(http://www.w3.org/2000/svg);
@namespace xml url(http://www.w3.org/XML/1998/namespace);

svg:not(:root), image, marker, pattern, symbol { overflow: hidden; }

*:not(svg),
*:not(foreignObject) > svg {
  transform-origin: 0 0;
}

*[xml|space=preserve] {
  white-space-collapse: preserve-spaces;
}

defs,
clipPath, mask, marker,
desc, title, metadata,
pattern, linearGradient, radialGradient,
script, style,
symbol {
  display: none !important;
}
:host(use) > symbol {
  display: inline !important;
}
:link, :visited {
  cursor: pointer;
}
```

In addition, all interactive user agents are required to apply distinctive styles to the `:focus` pseudo-class (normally using the `outline` property) and the `::selection` pseudo-element (using an appropriate highlighting technique, such as redrawing the selected glyphs with inverse colors).

An `!important` rule in a user agent stylesheet over-rides all user and author styles [[css-cascade-4](refs.md#ref-css-cascade-4)]. The display value for [never-rendered elements](render.md#TermNeverRenderedElement) and for ‘[symbol](struct.md#SymbolElement)’ elements can therefore not be changed. A symbol must only be rendered if it is the direct child of a shadow root whose host is a ‘[use](struct.md#UseElement)’ element (and must always be rendered if the host ‘[use](struct.md#UseElement)’ element is rendered). The other elements, and their child content, are never rendered directly.

CSS Transforms defines that the initial value for [transform-origin](../css-transforms-1.md#transform-origin-property) is 50% 50%. Since elements in SVG must, by default, transform around their origin at (0, 0), [transform-origin](../css-transforms-1.md#transform-origin-property) is overridden and set to a default value of 0 0 for all SVG elements (except for root ‘[svg](struct.md#SVGElement)’ elements and ‘[svg](struct.md#SVGElement)’ elements that are the child of a ‘[foreignObject](embedded.md#ForeignObjectElement)’ element or an element in a non-SVG namespace; these elements must transform around their center). [[css-transforms-1](refs.md#ref-css-transforms-1)]

The OpenType specification requires an additional user agent style sheet to be applied when processing [[OPENTYPE](refs.md#ref-opentype)]. It is as follows:

```
@namespace svg url(http://www.w3.org/2000/svg);

svg|text, svg|foreignObject {
  display: none !important;
}

:root {
  fill: context-fill;
  fill-opacity: context-fill-opacity;
  stroke: context-stroke;
  stroke-opacity: context-stroke-opacity;
  stroke-width: context-value;
  stroke-dasharray: context-value;
  stroke-dashoffset: context-value;
}
```

The [context-fill](painting.md#context-paint) and [context-stroke](painting.md#context-paint) keywords are as defined in this specification, where the [context element](painting.md#TermContextElement) for a font glyph is the corresponding [text content element](text.md#TermTextContentElement). The other keywords are as defined in the OpenType specification, and ensure that the style values from the [text content element](text.md#TermTextContentElement) are propagated to the font glyphs, with appropriate adjustments for the change in the coordinate system [[OPENTYPE](refs.md#ref-opentype)].

## <a id="RequiredCSSFeatures"></a>6.9. Required CSS features

Besides the features described above, the following CSS features must be also supported in SVG user agents:

- in XML documents, external CSS style sheets using the <?xml-stylesheet?> processing instruction [[xml-stylesheet](refs.md#ref-xml-stylesheet)]
- @media, @import and @charset rules within style sheets, as defined in CSS 2.1
- @font-face rules within style sheets [[css-fonts-3](refs.md#ref-css-fonts-3)]
- all pseudo-classes defined in CSS 2.1 (including :hover, :link, :active, :visited, :focus, :first-child and :lang)
- the ::first-letter and ::first-line pseudo-elements defined in CSS 2.1 (on ‘[text](text.md#TextElement)’ elements)

## <a id="DOMInterfaces"></a>6.10. DOM interfaces

### <a id="InterfaceSVGStyleElement"></a>6.10.1. Interface SVGStyleElement

An [SVGStyleElement](#InterfaceSVGStyleElement) object represents a ‘[style](#StyleElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGStyleElement : SVGElement {
  attribute DOMString type;
  attribute DOMString media;
  attribute DOMString title;
  attribute boolean disabled;
};

SVGStyleElement includes LinkStyle;
```

The **<a id="__svg__SVGStyleElement__type"></a>type**, **<a id="__svg__SVGStyleElement__media"></a>media** and **<a id="__svg__SVGStyleElement__title"></a>title** IDL attributes [reflect](types.md#TermReflect) the ‘[type](#StyleElementTypeAttribute)’, ‘[media](#StyleElementMediaAttribute)’ and ‘[title](#StyleElementTitleAttribute)’ content attributes, respectively.
