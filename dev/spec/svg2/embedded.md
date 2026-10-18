<!-- https://svgwg.org/svg2-draft/embedded.html, as of 2026-10-18; written by dev/spec_text.py -->

# Chapter 12: Embedded Content

## <a id="Overview"></a>12.1. Overview

Embedded content is content that imports another resource into the document, or content from another vocabulary that is inserted into the document. This is the same definition as HTML's embedded content.

SVG supports embedded content with the use of ‘[image](#ImageElement)’ and ‘[foreignObject](#ForeignObjectElement)’ elements.

Content embedded with ‘[image](#ImageElement)’ is compatible with Resource Hints for prioritizing downloading of external resources.

## <a id="Placement"></a>12.2. Placement of the embedded content

The [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing), and [height](geometry.md#Sizing) geometry properties specify the rectangular region into which the embedded content is positioned (the *<a id="TermPositioningRectangle"></a>positioning rectangle*). The [positioning rectangle](#TermPositioningRectangle) is used as the bounding box of the element; note, however, that graphics may overflow the positioning rectangle, depending on the value of the [overflow](render.md#OverflowAndClipProperties) property.

When the embedded content consists of a single referenced resource (e.g., an ‘[image](#ImageElement)’), the dimensions of the [positioning rectangle](#TermPositioningRectangle), in the current coordinate system after applying all transforms, define the specified size for the embedded object. A concrete object size and final position must be determined for the object using the Default Sizing Algorithm defined for replaced elements in CSS layout [[css-images-3](refs.md#ref-css-images-3)]. The object-fit and object-position affect the final position and size of the object, and may cause it to be extend beyond the [positioning rectangle](#TermPositioningRectangle). In that case, the [overflow](render.md#OverflowAndClipProperties) property determines whether the rendered object should be clipped to its [positioning rectangle](#TermPositioningRectangle).

When the embedded content consists of a document fragment (e.g., a ‘[foreignObject](#ForeignObjectElement)’), the [positioning rectangle](#TermPositioningRectangle) defines the bounds of a containing block for laying out the child content using CSS. The scale of the containing block is defined in the current coordinate system, including all explicit and implicit (e.g., ‘[viewBox](coords.md#ViewBoxAttribute)’) transformations. The ‘[foreignObject](#ForeignObjectElement)’, or other element that is positioned using SVG layout attributes, is implicitly absolutely-positioned for the purposes of CSS layout. As a result, any absolutely-positioned child elements are positioned relative to this containing block. Again, the [overflow](render.md#OverflowAndClipProperties) property determines whether content that extends outside the [positioning rectangle](#TermPositioningRectangle) will be hidden.

A value of zero for either [width](geometry.md#Sizing) or [height](geometry.md#Sizing) disables rendering of the element and its embedded content.

The 'auto' value for [width](geometry.md#Sizing) or [height](geometry.md#Sizing) is used to size the corresponding element automatically based on the intrinsic dimensions or intrinsic aspect ratio of the referenced resource. Computation of automatically-sized values follows the Default Sizing Algorithm defined for replaced elements in CSS layout [[css-images-3](refs.md#ref-css-images-3)]. In particular, when the referenced resource does not have an intrinsic size (such as image types with no defined dimensions), it is assumed to have a width of 300px and a height of 150px.

CSS positioning properties (e.g. top and margin) have no effect when positioning the embedded content element in the SVG coordinate system. They can, however, be used to position child elements of a ‘[foreignObject](#ForeignObjectElement)’ or HTML embedding element.

## <a id="ImageElement"></a>12.3. The ‘image’ element

‘*<a id="elementdef-image"></a>image*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [graphics referencing element](struct.md#TermGraphicsReferencingElement), [renderable element](render.md#TermRenderableElement), [structurally external element](struct.md#TermStructurallyExternalElement)
**Content model:**
: Any number of the following elements, in any order:

- animation elements — ‘animate’, ‘animateMotion’, ‘animateTransform’, ‘set’
- [descriptive elements](struct.md#TermDescriptiveElement) — ‘[desc](struct.md#DescElement)’, ‘[title](struct.md#TitleElement)’, ‘[metadata](struct.md#MetadataElement)’

[clipPath](../css-masking-1.md#ClipPathElement), [mask](../css-masking-1.md#MaskElement), [script](interact.md#ScriptElement), [style](styling.md#StyleElement)
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [deprecated xlink attributes](linking.md#XLinkRefAttrs) — ‘[xlink:href](linking.md#XLinkHrefAttribute)’, ‘[xlink:title](linking.md#XLinkTitleAttribute)’
- [presentation attributes](styling.md#TermPresentationAttribute) —
- ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’
- ‘[href](#ImageElementHrefAttribute)’
- ‘[crossorigin](#ImageElementCrossoriginAttribute)’

**Geometry properties:**
:

- [x](geometry.md#XProperty)
- [y](geometry.md#YProperty)
- [width](geometry.md#Sizing)
- [height](geometry.md#Sizing)

**DOM Interfaces:**
:

- [SVGImageElement](#InterfaceSVGImageElement)

The ‘[image](#ImageElement)’ element indicates that the contents of a complete file are to be rendered into a given rectangle within the current user coordinate system. The ‘[image](#ImageElement)’ element can refer to raster image files such as PNG or JPEG or to files with MIME type of "image/svg+xml". [Conforming SVG viewers](conform.md#ConformingSVGViewers) need to support at least PNG, JPEG and SVG format files. SVG files must be processed in [secure animated mode](conform.md#secure-animated-mode) if the current document supports animation, or in [secure static mode](conform.md#secure-static-mode) if the current document is static.

The result of processing an ‘[image](#ImageElement)’ is always a four-channel RGBA result. When an ‘[image](#ImageElement)’ element references an image (such as many PNG or JPEG files) which only has three channels (RGB), then the effect is as if the object were converted into a 4-channel RGBA image with the alpha channel uniformly set to 1. For a single-channel (grayscale) raster image, the effect is as if the object were converted into a 4-channel RGBA image, where the single channel from the referenced object is used to compute the three color channels and the alpha channel is uniformly set to 1.

The ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute determines how the referenced image is scaled and positioned to fit into the concrete object size determined from the [positioning rectangle](#TermPositioningRectangle) and the object-fit and object-position properties. The result of applying this attribute defines an *<a id="TermImageRenderingRectangle"></a>image-rendering rectangle* used for actual image rendering. When the referenced image is an SVG, the [image-rendering rectangle](#TermImageRenderingRectangle) defines the [SVG viewport](coords.md#TermSVGViewport) used for rendering that SVG.

The ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ calculations are applied *after* determining the concrete object size, and only have an effect if that size does not match the intrinsic aspect ratio of the embedded image. If a value of object-fit is used that ensures that the concrete object size matches the intrinsic aspect ratio (i.e., any value other than the default fill), then the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ value will have no effect; the [image-rendering rectangle](#TermImageRenderingRectangle) will be that determined when scaling and positioning the object with CSS. The ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute can therefore be safely used as a fallback for most values of object-fit and object-position; it must be explicitly set to none to turn off aspect ratio control, regardless of object-fit value.

The aspect ratio to use when evaluating the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute is defined by the intrinsic aspect ratio of the referenced content. For an SVG file, the aspect ratio is defined in [Intrinsic sizing properties of SVG content"](coords.md#SizingSVGInCSS). For most raster content (PNG, JPEG) the pixel width and height of the image file define an intrinsic aspect ratio. Where the embedded image does not have an intrinsic aspect ratio (e.g. an SVG file with neither ‘[viewBox](coords.md#ViewBoxAttribute)’ attribute nor explicit dimensions for the [outermost svg element](struct.md#TermOutermostSVGElement)) the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute is ignored; the embedded image is drawn to fill the [positioning rectangle](#TermPositioningRectangle) defined by the geometry properties on the ‘[image](#ImageElement)’ element.

For example, if the image element referenced a PNG or JPEG and preserveAspectRatio="xMinYMin meet", then the aspect ratio of the raster would be preserved (which means that the scale factor from image's coordinates to current user space coordinates would be the same for both X and Y), the raster would be sized as large as possible while ensuring that the entire raster fits within the viewport, and the top/left of the raster would be aligned with the top/left of the viewport as defined by the attributes [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) on the ‘[image](#ImageElement)’ element.  If the value of ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ was 'none' then aspect ratio of the image would not be preserved. The image would be fit such that the top/left corner of the raster exactly aligns with coordinate ([x](geometry.md#XProperty), [y](geometry.md#YProperty)) and the bottom/right corner of the raster exactly aligns with coordinate ([x](geometry.md#XProperty)+[width](geometry.md#Sizing), [y](geometry.md#YProperty)+[height](geometry.md#Sizing)).

For ‘[image](#ImageElement)’ elements embedding an SVG image, the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute on the root element in the referenced SVG image must be ignored, and instead treated as if it had a value of none. (see ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ for details). This ensures that the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute on the referencing ‘[image](#ImageElement)’ has its intended effect, even if it is none.

When the value of the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute on the ‘[image](#ImageElement)’ is *not* none, the [image-rendering rectangle](#TermImageRenderingRectangle) determined from the properties of the ‘[image](#ImageElement)’ element will exactly match the embedded SVG's intrinsic aspect ratio. Ignoring the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ attribute from the embedded SVG will therefore not usually have any effect. The exception is if the aspect ratio of that image is determined from absolute values for the [width](geometry.md#Sizing) and [height](geometry.md#Sizing) attributes which *do not* match its ‘[viewBox](coords.md#ViewBoxAttribute)’ aspect ratio. This is an unusual situation that authors are advised to avoid, for many reasons.

The user agent stylesheet sets the value of the [overflow](render.md#OverflowAndClipProperties) property on ‘[image](#ImageElement)’ element to hidden. Unless over-ridden by the author, images will therefore be clipped to the [positioning rectangle](#TermPositioningRectangle) defined by the geometry properties.

For ‘[image](#ImageElement)’ elements embedding an SVG image, two different [overflow](render.md#OverflowAndClipProperties) values apply. The value specified on the ‘[image](#ImageElement)’ element determines whether the [image-rendering rectangle](#TermImageRenderingRectangle) is clipped to the [positioning rectangle](#TermPositioningRectangle). The value on the root element of the referenced SVG determines whether the graphics are clipped to the [image-rendering rectangle](#TermImageRenderingRectangle).

New in SVG 2. Previous versions of SVG required that the [overflow](render.md#OverflowAndClipProperties) (and also [clip](../css-masking-1.md#propdef-clip)) property on the embedded SVG be ignored. The new rules ensure that an overflowing slice layout can be safely used without compromising the overflow control from the referenced image.

To link into particular view of an embedded SVG image, authors can use media fragments as defined in [Linking into SVG content](linking.md#LinksIntoSVG). To crop to a specific section of a raster image, authors can use *Basic media fragments identifiers* [[Media Fragments URI 1.0 (basic)](refs.md#ref-media-frags)]. Either type of fragment may affect the intrinsic dimensions and/or intrinsic aspect ratio of the image.

The resource referenced by the ‘[image](#ImageElement)’ element represents a separate document which generates its own parse tree and document object model (if the resource is XML). Thus, there is no inheritance of properties into the image.

Unlike ‘[use](struct.md#UseElement)’, the ‘[image](#ImageElement)’ element cannot reference elements within an SVG file.

*Attribute definitions:*

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="ImageElementCrossoriginAttribute"></a>crossorigin* | [ anonymous \| use-credentials ]? | (see HTML definition of attribute) | yes |

**
:

The crossorigin attribute is a CORS settings attribute, and unless otherwise specified follows the same processing rules as in HTML [[HTML](refs.md#ref-html)].

**

| Name | Value | Initial value | Animatable |
| --- | --- | --- | --- |
| *<a id="ImageElementHrefAttribute"></a>href* | URL [[URL]](types.md#attribute-url) | (none) | yes |

**
:

An [URL reference](linking.md#URLReference) identifying the image to render. Refer to the common handling defined for [URL reference attributes](linking.md#linkRefAttrs) and [deprecated XLink attributes](linking.md#XLinkRefAttrs).

The URL is processed and the resource is fetched [as described in the Linking chapter](linking.md#processingURL).

```
<?xml version="1.0" standalone="no"?>
<svg width="4in" height="3in"
     xmlns="http://www.w3.org/2000/svg">
  <desc>This graphic links to an external image
  </desc>
  <image x="200" y="200" width="100px" height="100px"
         href="myimage.png">
    <title>My image</title>
  </image>
</svg>
```

Since image references always refer to a complete document, a target-only URL is treated as a link to the same file, which is rendered again as an independent embedded image. Since the embedded image is processed in a secure mode, its own embedded references are not processed, preventing infinite recursion.

```
<?xml version="1.0" standalone="no"?>
<svg width="5cm" height="3cm" viewBox="0 0 50 30" 
     xmlns="http://www.w3.org/2000/svg"
     xmlns:xlink="http://www.w3.org/1999/xlink">
  <title>Recursive SVG</title>
  <desc>An SVG with two recursive image reference to itself.
    One reference uses the file name as a relative URL, 
    the other uses a target fragment only.
    When viewed in a processing mode that supports external file references,
    the embedded images should be rendered;
    however, the embedded image must be processed in secure mode,
    so the recursion only happens once.
    The appearance should be three nested red circles in a bulls-eye pattern;
    the innermost circle has solid fill because of target styles.
  </desc>
  <style type="text/css">
    #target:target {
      fill: red;
    }
  </style>
  <circle id="target" 
          stroke="red" stroke-width="5" fill="none"
          cx="50%" cy="50%" r="12" />
  <image xlink:href="recursive-image.svg"
         x="25%" y="25%" width="50%" height="50%" />
  <image xlink:href="#target"
         x="45%" y="45%" width="10%" height="10%" />
</svg>
```

[image: Example recursive-image — an SVG that embeds itself, creating a bulls-eye pattern]

Example recursive-image

[View this example as SVG (SVG-enabled browsers only)](https://svgwg.org/svg2-draft/images/embedded/recursive-image.svg)

## <a id="ForeignObjectElement"></a>12.4. The ‘foreignObject’ element

‘*<a id="elementdef-foreignObject"></a>foreignObject*’

**Categories:**
: [Graphics element](struct.md#TermGraphicsElement), [renderable element](render.md#TermRenderableElement), [structurally external element](struct.md#TermStructurallyExternalElement)
**Content model:**
: Any elements or character data.
**Attributes:**
:

- [aria attributes](struct.md#TermARIAAttribute) — ‘aria-activedescendant’, ‘aria-atomic’, ‘aria-autocomplete’, ‘aria-busy’, ‘aria-checked’, ‘aria-colcount’, ‘aria-colindex’, ‘aria-colspan’, ‘aria-controls’, ‘aria-current’, ‘aria-describedby’, ‘aria-details’, ‘aria-disabled’, ‘aria-dropeffect’, ‘aria-errormessage’, ‘aria-expanded’, ‘aria-flowto’, ‘aria-grabbed’, ‘aria-haspopup’, ‘aria-hidden’, ‘aria-invalid’, ‘aria-keyshortcuts’, ‘aria-label’, ‘aria-labelledby’, ‘aria-level’, ‘aria-live’, ‘aria-modal’, ‘aria-multiline’, ‘aria-multiselectable’, ‘aria-orientation’, ‘aria-owns’, ‘aria-placeholder’, ‘aria-posinset’, ‘aria-pressed’, ‘aria-readonly’, ‘aria-relevant’, ‘aria-required’, ‘aria-roledescription’, ‘aria-rowcount’, ‘aria-rowindex’, ‘aria-rowspan’, ‘aria-selected’, ‘aria-setsize’, ‘aria-sort’, ‘aria-valuemax’, ‘aria-valuemin’, ‘aria-valuenow’, ‘aria-valuetext’, ‘[role](struct.md#RoleAttribute)’
- [core attributes](struct.md#TermCoreAttribute) — ‘[id](struct.md#IDAttribute)’, ‘[tabindex](struct.md#SVGElementTabindexAttribute)’, ‘[autofocus](struct.md#SVGElementAutofocusAttribute)’, ‘[lang](struct.md#LangAttribute)’, ‘[xml:space](struct.md#XMLSpaceAttribute)’, ‘[class](styling.md#ClassAttribute)’, ‘[style](styling.md#StyleAttribute)’
- [conditional processing attributes](struct.md#TermConditionalProcessingAttribute) — ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’, ‘[systemLanguage](struct.md#SystemLanguageAttribute)’
- global event attributes — ‘[oncancel](interact.md#EventAttributes)’, ‘[oncanplay](interact.md#EventAttributes)’, ‘[oncanplaythrough](interact.md#EventAttributes)’, ‘[onchange](interact.md#EventAttributes)’, ‘[onclick](interact.md#EventAttributes)’, ‘[onclose](interact.md#EventAttributes)’, ‘[oncopy](interact.md#EventAttributes)’, ‘[oncuechange](interact.md#EventAttributes)’, ‘[oncut](interact.md#EventAttributes)’, ‘[ondblclick](interact.md#EventAttributes)’, ‘[ondrag](interact.md#EventAttributes)’, ‘[ondragend](interact.md#EventAttributes)’, ‘[ondragenter](interact.md#EventAttributes)’, ‘[ondragexit](interact.md#EventAttributes)’, ‘[ondragleave](interact.md#EventAttributes)’, ‘[ondragover](interact.md#EventAttributes)’, ‘[ondragstart](interact.md#EventAttributes)’, ‘[ondrop](interact.md#EventAttributes)’, ‘[ondurationchange](interact.md#EventAttributes)’, ‘[onemptied](interact.md#EventAttributes)’, ‘[onended](interact.md#EventAttributes)’, ‘[onerror](interact.md#EventAttributes)’, ‘[onfocus](interact.md#EventAttributes)’, ‘[oninput](interact.md#EventAttributes)’, ‘[oninvalid](interact.md#EventAttributes)’, ‘[onkeydown](interact.md#EventAttributes)’, ‘[onkeypress](interact.md#EventAttributes)’, ‘[onkeyup](interact.md#EventAttributes)’, ‘[onload](interact.md#EventAttributes)’, ‘[onloadeddata](interact.md#EventAttributes)’, ‘[onloadedmetadata](interact.md#EventAttributes)’, ‘[onloadstart](interact.md#EventAttributes)’, ‘[onmousedown](interact.md#EventAttributes)’, ‘[onmouseenter](interact.md#EventAttributes)’, ‘[onmouseleave](interact.md#EventAttributes)’, ‘[onmousemove](interact.md#EventAttributes)’, ‘[onmouseout](interact.md#EventAttributes)’, ‘[onmouseover](interact.md#EventAttributes)’, ‘[onmouseup](interact.md#EventAttributes)’, ‘[onpaste](interact.md#EventAttributes)’, ‘[onpause](interact.md#EventAttributes)’, ‘[onplay](interact.md#EventAttributes)’, ‘[onplaying](interact.md#EventAttributes)’, ‘[onprogress](interact.md#EventAttributes)’, ‘[onratechange](interact.md#EventAttributes)’, ‘[onreset](interact.md#EventAttributes)’, ‘[onresize](interact.md#EventAttributes)’, ‘[onscroll](interact.md#EventAttributes)’, ‘[onseeked](interact.md#EventAttributes)’, ‘[onseeking](interact.md#EventAttributes)’, ‘[onselect](interact.md#EventAttributes)’, ‘[onshow](interact.md#EventAttributes)’, ‘[onstalled](interact.md#EventAttributes)’, ‘[onsubmit](interact.md#EventAttributes)’, ‘[onsuspend](interact.md#EventAttributes)’, ‘[ontimeupdate](interact.md#EventAttributes)’, ‘[ontoggle](interact.md#EventAttributes)’, ‘[onvolumechange](interact.md#EventAttributes)’, ‘[onwaiting](interact.md#EventAttributes)’, ‘[onwheel](interact.md#EventAttributes)’
- [presentation attributes](styling.md#TermPresentationAttribute) —

**Geometry properties:**
:

- [x](geometry.md#XProperty)
- [y](geometry.md#YProperty)
- [width](geometry.md#Sizing)
- [height](geometry.md#Sizing)

**DOM Interfaces:**
:

- [SVGForeignObjectElement](#InterfaceSVGForeignObjectElement)

SVG is designed to be compatible with other XML languages for describing and rendering other types of content. The ‘[foreignObject](#ForeignObjectElement)’ element allows for inclusion of elements in a non-SVG namespace which is rendered within a region of the SVG graphic using other user agent processes. The included foreign graphical content is subject to SVG transformations, filters, clipping, masking and compositing. Examples include inserting a MathML expression into an SVG drawing [[MathML3](refs.md#ref-mathml3)], or adding a block of complex CSS-formatted HTML text or form inputs.

The HTML parser treats elements inside the ‘[foreignObject](#ForeignObjectElement)’ equivalent to elements inside an HTML document fragment. Any `svg` or `math` element, and their descendents, will be parsed as being in the SVG or MathML namespace, respectively; all other tags will be parsed as being in the HTML namespace.

SVG-namespaced elements within a ‘[foreignObject](#ForeignObjectElement)’ will not be rendered, except in the situation where a properly defined SVG fragment, including a root ‘[svg](struct.md#SVGElement)’ element is defined as a descendent of the ‘[foreignObject](#ForeignObjectElement)’.

A ‘[foreignObject](#ForeignObjectElement)’ may be used in conjunction with the ‘[switch](struct.md#SwitchElement)’ element and the ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’ attribute to provide proper checking for user agent support and provide an alternate rendering in case user agent support is not available.

This specification does not define how ‘[requiredExtensions](struct.md#RequiredExtensionsAttribute)’ values should be mapped to support for different XML languages; a future specification may do so.

It is not required that SVG user agents support the ability to invoke other arbitrary user agents to handle embedded foreign object types; however, all conforming SVG user agents would need to support the ‘[switch](struct.md#SwitchElement)’ element and must be able to render valid SVG elements when they appear as one of the alternatives within a ‘[switch](struct.md#SwitchElement)’ element.

It is expected that commercial Web browsers will support the ability for SVG to embed CSS-formatted HTML and also MathML content, with the rendered content subject to transformations and compositing defined in the SVG fragment. At this time, such a capability is not a requirement.

```
<?xml version="1.0" standalone="yes"?>
<svg width="4in" height="3in"
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

## <a id="DOMInterfaces"></a>12.5. DOM interfaces

### <a id="InterfaceSVGImageElement"></a>12.5.1. Interface SVGImageElement

An [SVGImageElement](#InterfaceSVGImageElement) object represents an ‘[image](#ImageElement)’ element in the DOM.

```
[Exposed=Window]
interface SVGImageElement : SVGGraphicsElement {
  [SameObject] readonly attribute SVGAnimatedLength x;
  [SameObject] readonly attribute SVGAnimatedLength y;
  [SameObject] readonly attribute SVGAnimatedLength width;
  [SameObject] readonly attribute SVGAnimatedLength height;
  [SameObject] readonly attribute SVGAnimatedPreserveAspectRatio preserveAspectRatio;
  attribute DOMString? crossOrigin;
};

SVGImageElement includes SVGURIReference;
```

The **<a id="__svg__SVGImageElement__x"></a>x**, **<a id="__svg__SVGImageElement__y"></a>y**, **<a id="__svg__SVGImageElement__width"></a>width** and **<a id="__svg__SVGImageElement__height"></a>height** IDL attributes [reflect](types.md#TermReflect) the computed values of the [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties and their corresponding presentation attributes, respectively.

The **<a id="__svg__SVGImageElement__preserveAspectRatio"></a>preserveAspectRatio** IDL attribute [reflects](types.md#TermReflect) the ‘[preserveAspectRatio](coords.md#PreserveAspectRatioAttribute)’ content attribute.

The **<a id="__svg__SVGImageElement__crossOrigin"></a>crossOrigin** IDL attribute [reflects](types.md#TermReflect) the ‘[crossorigin](#ImageElementCrossoriginAttribute)’ content attribute.

### <a id="InterfaceSVGForeignObjectElement"></a>12.5.2. Interface SVGForeignObjectElement

An [SVGForeignObjectElement](#InterfaceSVGForeignObjectElement) object represents a ‘[foreignObject](#ForeignObjectElement)’ in the DOM.

```
[Exposed=Window]
interface SVGForeignObjectElement : SVGGraphicsElement {
  [SameObject] readonly attribute SVGAnimatedLength x;
  [SameObject] readonly attribute SVGAnimatedLength y;
  [SameObject] readonly attribute SVGAnimatedLength width;
  [SameObject] readonly attribute SVGAnimatedLength height;
};
```

The **<a id="__svg__SVGForeignObjectElement__x"></a>x**, **<a id="__svg__SVGForeignObjectElement__y"></a>y**, **<a id="__svg__SVGForeignObjectElement__width"></a>width** and **<a id="__svg__SVGForeignObjectElement__height"></a>height** IDL attributes [reflect](types.md#TermReflect) the computed values of the [x](geometry.md#XProperty), [y](geometry.md#YProperty), [width](geometry.md#Sizing) and [height](geometry.md#Sizing) properties and their corresponding presentation attributes, respectively.
