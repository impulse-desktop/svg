<!-- https://www.w3.org/TR/SVG11/extend.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](backward.md) ⋅ [Next](https://www.w3.org/TR/SVG11/svgdtd.html) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 23 Extensibility

## <a id="ForeignNamespaces"></a>23.1 Foreign namespaces and private data

SVG allows inclusion of elements from foreign namespaces anywhere with the SVG content. In general, the SVG user agent will include the unknown elements in the DOM but will otherwise ignore unknown elements. (The notable exception is described under [Embedding Foreign Object Types](#ForeignObjectElement).)

Additionally, SVG allows inclusion of attributes from foreign namespaces on any SVG element. The SVG user agent will include unknown attributes in the DOM but with otherwise ignore unknown attributes.

SVG's ability to include foreign namespaces can be used for the following purposes:

- Application-specific information so that authoring applications can include model-level data in the SVG content to serve their "roundtripping" purposes (i.e., the ability to write, then read a file without loss of higher-level information).
- Supplemental data for extensibility. For example, suppose you have an extrusion extension which takes any 2D graphics and extrudes it in three dimensions. When applying the extrusion extension, you probably will need to set some parameters. The parameters can be included in the SVG content by inserting elements from an extrusion extension namespace.

To illustrate, a business graphics authoring application might want to include some private data within an SVG document so that it could properly reassemble the chart (a pie chart in this case) upon reading it back in:

```
<?xml version="1.0" standalone="yes"?>
<svg width="4in" height="3in" version="1.1"
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

## <a id="EmbeddingForeignObjects"></a>23.2 Embedding foreign object types

One goal for SVG is to provide a mechanism by which other XML language processors can render into an area within an SVG drawing, with those renderings subject to the various transformations and compositing parameters that are currently active at a given point within the SVG content tree. One particular example of this is to provide a frame for XML content styled with CSS or XSL so that dynamically reflowing text (subject to SVG transformations and compositing) could be inserted into the middle of some SVG content. Another example is inserting a MathML expression into an SVG drawing [[MATHML](refs.md#ref-MATHML)].

The [‘foreignObject’](#ForeignObjectElement) element allows for inclusion of a foreign namespace which has its graphical content drawn by a different user agent. The included foreign graphical content is subject to SVG transformations and compositing.

The contents of [‘foreignObject’](#ForeignObjectElement) are assumed to be from a different namespace. Any SVG elements within a [‘foreignObject’](#ForeignObjectElement) will not be drawn, except in the situation where a properly defined SVG subdocument with a proper ‘xmlns’ (see Namespaces in XML [[XML-NS](refs.md#ref-XML-NS)]) attribute specification is embedded recursively. One situation where this can occur is when an SVG document fragment is embedded within another non-SVG document fragment, which in turn is embedded within an SVG document fragment (e.g., an SVG document fragment contains an XHTML document fragment which in turn contains yet another SVG document fragment).

Usually, a [‘foreignObject’](#ForeignObjectElement) will be used in conjunction with the [‘switch’](struct.md#SwitchElement) element and the [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute) attribute to provide proper checking for user agent support and provide an alternate rendering in case user agent support is not available.

## <a id="ForeignObjectElement"></a>23.3 The ‘foreignObject’ element

‘foreignObject’

**Categories:**
: None
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [conditional processing attributes](intro.md#TermConditionalProcessingAttribute) — [‘requiredFeatures’](struct.md#RequiredFeaturesAttribute), [‘requiredExtensions’](struct.md#RequiredExtensionsAttribute), [‘systemLanguage’](struct.md#SystemLanguageAttribute)
- [graphical event attributes](intro.md#TermGraphicalEventAttribute) — [‘onfocusin’](https://www.w3.org/TR/SVG11/script.html#OnFocusInEventAttribute), [‘onfocusout’](https://www.w3.org/TR/SVG11/script.html#OnFocusOutEventAttribute), [‘onactivate’](https://www.w3.org/TR/SVG11/script.html#OnActivateEventAttribute), [‘onclick’](https://www.w3.org/TR/SVG11/script.html#OnClickEventAttribute), [‘onmousedown’](https://www.w3.org/TR/SVG11/script.html#OnMouseDownEventAttribute), [‘onmouseup’](https://www.w3.org/TR/SVG11/script.html#OnMouseUpEventAttribute), [‘onmouseover’](https://www.w3.org/TR/SVG11/script.html#OnMouseOverEventAttribute), [‘onmousemove’](https://www.w3.org/TR/SVG11/script.html#OnMouseMoveEventAttribute), [‘onmouseout’](https://www.w3.org/TR/SVG11/script.html#OnMouseOutEventAttribute), [‘onload’](https://www.w3.org/TR/SVG11/script.html#OnLoadEventAttribute)
- [presentation attributes](intro.md#TermPresentationAttribute) — [‘alignment-baseline’](text.md#AlignmentBaselineProperty), [‘baseline-shift’](text.md#BaselineShiftProperty), [‘clip’](masking.md#ClipProperty), [‘clip-path’](masking.md#ClipPathProperty), [‘clip-rule’](masking.md#ClipRuleProperty), [‘color’](color.md#ColorProperty), [‘color-interpolation’](painting.md#ColorInterpolationProperty), [‘color-interpolation-filters’](painting.md#ColorInterpolationFiltersProperty), [‘color-profile’](color.md#ColorProfileProperty), [‘color-rendering’](painting.md#ColorRenderingProperty), [‘cursor’](interact.md#CursorProperty), [‘direction’](text.md#DirectionProperty), [‘display’](painting.md#DisplayProperty), [‘dominant-baseline’](text.md#DominantBaselineProperty), [‘enable-background’](filters.md#EnableBackgroundProperty), [‘fill’](painting.md#FillProperty), [‘fill-opacity’](painting.md#FillOpacityProperty), [‘fill-rule’](painting.md#FillRuleProperty), [‘filter’](filters.md#FilterProperty), [‘flood-color’](filters.md#FloodColorProperty), [‘flood-opacity’](filters.md#FloodOpacityProperty), [‘font-family’](text.md#FontFamilyProperty), [‘font-size’](text.md#FontSizeProperty), [‘font-size-adjust’](text.md#FontSizeAdjustProperty), [‘font-stretch’](text.md#FontStretchProperty), [‘font-style’](text.md#FontStyleProperty), [‘font-variant’](text.md#FontVariantProperty), [‘font-weight’](text.md#FontWeightProperty), [‘glyph-orientation-horizontal’](text.md#GlyphOrientationHorizontalProperty), [‘glyph-orientation-vertical’](text.md#GlyphOrientationVerticalProperty), [‘image-rendering’](painting.md#ImageRenderingProperty), [‘kerning’](text.md#KerningProperty), [‘letter-spacing’](text.md#LetterSpacingProperty), [‘lighting-color’](filters.md#LightingColorProperty), [‘marker-end’](painting.md#MarkerEndProperty), [‘marker-mid’](painting.md#MarkerMidProperty), [‘marker-start’](painting.md#MarkerStartProperty), [‘mask’](masking.md#MaskProperty), [‘opacity’](masking.md#OpacityProperty), [‘overflow’](masking.md#OverflowProperty), [‘pointer-events’](interact.md#PointerEventsProperty), [‘shape-rendering’](painting.md#ShapeRenderingProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘stop-opacity’](pservers.md#StopOpacityProperty), [‘stroke’](painting.md#StrokeProperty), [‘stroke-dasharray’](painting.md#StrokeDasharrayProperty), [‘stroke-dashoffset’](painting.md#StrokeDashoffsetProperty), [‘stroke-linecap’](painting.md#StrokeLinecapProperty), [‘stroke-linejoin’](painting.md#StrokeLinejoinProperty), [‘stroke-miterlimit’](painting.md#StrokeMiterlimitProperty), [‘stroke-opacity’](painting.md#StrokeOpacityProperty), [‘stroke-width’](painting.md#StrokeWidthProperty), [‘text-anchor’](text.md#TextAnchorProperty), [‘text-decoration’](text.md#TextDecorationProperty), [‘text-rendering’](painting.md#TextRenderingProperty), [‘unicode-bidi’](text.md#UnicodeBidiProperty), [‘visibility’](painting.md#VisibilityProperty), [‘word-spacing’](text.md#WordSpacingProperty), [‘writing-mode’](text.md#WritingModeProperty)
- [‘class’](styling.md#ClassAttribute)
- [‘style’](styling.md#StyleAttribute)
- [‘externalResourcesRequired’](struct.md#ExternalResourcesRequiredAttribute)
- [‘transform’](coords.md#TransformAttribute)
- [‘x’](#ForeignObjectElementXAttribute)
- [‘y’](#ForeignObjectElementYAttribute)
- [‘width’](#ForeignObjectElementWidthAttribute)
- [‘height’](#ForeignObjectElementHeightAttribute)

**DOM Interfaces:**
:

- [SVGForeignObjectElement](#InterfaceSVGForeignObjectElement)

*Attribute definitions:*

**<a id="ForeignObjectElementXAttribute"></a>x = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The x-axis coordinate of one corner of the rectangular region into which the graphics associated with the contents of the [‘foreignObject’](#ForeignObjectElement) will be rendered. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="ForeignObjectElementYAttribute"></a>y = "[<coordinate>](types.md#DataTypeCoordinate)"**
: The y-axis coordinate of one corner of the rectangular region into which the referenced document is placed. If the attribute is not specified, the effect is as if a value of '0' were specified. [Animatable](animate.md#Animatable): yes.
**<a id="ForeignObjectElementWidthAttribute"></a>width = "[<length>](types.md#DataTypeLength)"**
: The width of the rectangular region into which the referenced document is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
**<a id="ForeignObjectElementHeightAttribute"></a>height = "[<length>](types.md#DataTypeLength)"**
: The height of the rectangular region into which the referenced document is placed. A negative value is an error (see [Error processing](implnote.md#ErrorProcessing)). A value of zero disables rendering of the element. [Animatable](animate.md#Animatable): yes.
## <a id="AnExample"></a>23.4 An example

Here is an example:

```
<?xml version="1.0" standalone="yes"?>
<svg width="4in" height="3in" version="1.1"
 xmlns = 'http://www.w3.org/2000/svg'>
  <desc>This example uses the 'switch' element to provide a 
        fallback graphical representation of an paragraph, if 
        XMHTML is not supported.</desc>
  <!-- The 'switch' element will process the first child element
       whose testing attributes evaluate to true.-->
  <switch>
    <!-- Process the embedded XHTML if the requiredExtensions attribute
         evaluates to true (i.e., the user agent supports XHTML
         embedded within SVG). -->
    <foreignObject width="100" height="50"
                   requiredExtensions="http://example.com/SVGExtensions/EmbeddedXHTML">
      <!-- XHTML content goes here -->
      <body xmlns="http://www.w3.org/1999/xhtml">
        <p>Here is a paragraph that requires word wrap</p>
      </body>
    </foreignObject>
    <!-- Else, process the following alternate SVG.
         Note that there are no testing attributes on the 'text' element.
         If no testing attributes are provided, it is as if there
         were testing attributes and they evaluated to true.-->
    <text font-size="10" font-family="Verdana">
      <tspan x="10" y="10">Here is a paragraph that</tspan>
      <tspan x="10" y="20">requires word wrap.</tspan>
    </text>
  </switch>
</svg>
```

It is not required that SVG user agent support the ability to invoke other arbitrary user agents to handle embedded foreign object types; however, all conforming SVG user agents would need to support the [‘switch’](struct.md#SwitchElement) element and must be able to render valid SVG elements when they appear as one of the alternatives within a [‘switch’](struct.md#SwitchElement) element.

Ultimately, it is expected that commercial Web browsers will support the ability for SVG to embed content from other XML grammars which use CSS or XSL to format their content, with the resulting CSS- or XSL-formatted content subject to SVG transformations and compositing. At this time, such a capability is not a requirement.

## <a id="PrivateElementsAndAttribute"></a>23.5 Adding private elements and attributes to the DTD

Using foreign namespaces as an extension mechanism adds flexibility, is readily handled by validation technologies like NVDL and RelaxNG, but typically breaks DTD validation unless the DTD has explicit extensibility hooks.

The SVG DTD allows for extending the SVG language within the internal DTD subset. Within the internal DTD subset, you have the ability to add custom elements and attributes to most SVG elements. This facility may be used if DTD validation is desired.

The DTD defines an extension entity for most of SVG elements. For example, the [‘view’](linking.md#ViewElement) element is defined in the DTD as follows:

```
<!ENTITY % SVG.view.extra.content "" >

<!ENTITY % SVG.view.element "INCLUDE" >
<![%SVG.view.element;[
<!ENTITY % SVG.view.content
    "( %SVG.Description.class; %SVG.view.extra.content; )*"
>
<!ELEMENT %SVG.view.qname; %SVG.view.content; >
<!-- end of SVG.view.element -->]]>

<!ENTITY % SVG.view.attlist "INCLUDE" >
<![%SVG.view.attlist;[
<!ATTLIST %SVG.view.qname;
    %SVG.Core.attrib;
    %SVG.External.attrib;
    viewBox %ViewBoxSpec.datatype; #IMPLIED
    preserveAspectRatio %PreserveAspectRatioSpec.datatype; 'xMidYMid meet'
    zoomAndPan ( disable | magnify ) 'magnify'
    viewTarget CDATA #IMPLIED
>
<!-- end of SVG.view.attlist -->]]>
```

The entity `SVG.view.extra.content` can be defined in the internal DTD subset to add custom sub-elements attributes to the [‘view’](linking.md#ViewElement) element within a given document, and an `<!ATTLIST>` can be used to add custom attributes. For example, the following extends the [‘view’](linking.md#ViewElement) element with an additional child element ‘customNS:customElement’ and an additional attribute ‘customNS:customAttr’:

```
<?xml version="1.0" standalone="no"?>
<!DOCTYPE svg PUBLIC "-//W3C//DTD SVG 1.1//EN" 
  "http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd" [
<!ENTITY % SVG.view.extra.content  "| customNS:customElement" >
<!ATTLIST %SVG.view.qname;
  xmlns:customNS CDATA #FIXED "http://www.example.org/customNS"
  customNS:customAttr CDATA #IMPLIED>
<!ELEMENT customNS:customElement EMPTY>
<!ATTLIST customNS:customElement
  xmlns:customNS CDATA #FIXED "http://www.example.org/customNS"
  info CDATA #IMPLIED>
]>
<svg xmlns="http://www.w3.org/2000/svg" version="1.1"
     width="8cm" height="4cm">
  <desc>Extend the 'view' element via the internal DTD subset</desc>
  <!-- Presumably, some great graphics would go here. -->
  <view viewBox="100 110 20 30" customNS:customAttr="123">
    <customNS:customElement info="abc"/>
  </view>
</svg>
```

## <a id="DOMInterfaces"></a>23.6 DOM interfaces

### <a id="InterfaceSVGForeignObjectElement"></a>23.6.1 Interface SVGForeignObjectElement

The [SVGForeignObjectElement](#InterfaceSVGForeignObjectElement) interface corresponds to the [‘foreignObject’](#ForeignObjectElement) element.

```
interface SVGForeignObjectElement : SVGElement,
                                    SVGTests,
                                    SVGLangSpace,
                                    SVGExternalResourcesRequired,
                                    SVGStylable,
                                    SVGTransformable {
  readonly attribute SVGAnimatedLength x;
  readonly attribute SVGAnimatedLength y;
  readonly attribute SVGAnimatedLength width;
  readonly attribute SVGAnimatedLength height;
};
```

**Attributes:**
:

**<a id="__svg__SVGForeignObjectElement__x"></a>**x** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘x’](#ForeignObjectElementXAttribute) on the given [‘foreignObject’](#ForeignObjectElement) element.

**<a id="__svg__SVGForeignObjectElement__y"></a>**y** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘y’](#ForeignObjectElementYAttribute) on the given [‘foreignObject’](#ForeignObjectElement) element.

**<a id="__svg__SVGForeignObjectElement__width"></a>**width** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘width’](#ForeignObjectElementWidthAttribute) on the given [‘foreignObject’](#ForeignObjectElement) element.

**<a id="__svg__SVGForeignObjectElement__height"></a>**height** (readonly [SVGAnimatedLength](types.md#InterfaceSVGAnimatedLength))**
:

Corresponds to attribute [‘height’](#ForeignObjectElementHeightAttribute) on the given [‘foreignObject’](#ForeignObjectElement) element.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](backward.md) ⋅ [Next](https://www.w3.org/TR/SVG11/svgdtd.html) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
