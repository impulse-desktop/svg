<!-- https://www.w3.org/TR/SVG11/color.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](painting.md) ⋅ [Next](pservers.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 12 Color

## <a id="ColorIntroduction"></a>12.1 Introduction

All SVG colors are specified in the sRGB color space [[SRGB](refs.md#ref-SRGB)]. At a minimum, SVG user agents shall conform to the color behavior requirements specified in the color units section and the minimal gamma correction rules defined in the CSS2 specification.

Additionally, SVG content can specify an alternate color specification using an ICC profile [[ICC42](refs.md#ref-ICC42)] as described in [Specifying paint](painting.md#SpecifyingPaint). If ICC-based colors are provided and the SVG user agent supports ICC color, then the ICC-based color takes precedence over the sRGB color specification; otherwise, the RGB fallback colors must be used. Note that, in this specification, color interpolation occurs in an RGB color space even if an ICC-based color specification is provided (see [‘color-interpolation’](painting.md#ColorInterpolationProperty)).

## <a id="ColorProperty"></a>12.2 The ‘color’ property

The [‘color’](#ColorProperty) property is used to provide a potential indirect value (currentColor) for the [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘flood-color’](filters.md#FloodColorProperty) and [‘lighting-color’](filters.md#LightingColorProperty) properties.

**‘color’**
:

| *Value:* | [<color>](types.md#DataTypeColor) \| inherit |
| --- | --- |
| *Initial:* | depends on user agent |
| *Applies to:* | elements to which properties [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘flood-color’](filters.md#FloodColorProperty) and [‘lighting-color’](filters.md#LightingColorProperty) apply |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

Except for any additional information provided in this specification, the normative definition of the property is in CSS2 ([[CSS2](refs.md#ref-CSS2)], section 14.1).

## <a id="ColorProfileDescriptions"></a>12.3 Color profile descriptions

### <a id="ColorProfileDescriptionsOverview"></a>12.3.1 Overview of color profile descriptions

The International Color Consortium has established a standard, the ICC Profile [[ICC42](refs.md#ref-ICC42)], for documenting the color characteristics of input and output devices. Using these profiles, it is possible to build a transform and correct visual data for viewing on different devices.

A color profile description provides the bridge between an ICC profile and references to that ICC profile within SVG content. The color profile description is added to the user agent's list of known color profiles and then used to select the relevant profile. The color profile description contains descriptors for the location of the color profile on the Web, a name to reference the profile and information about rendering intent.

### <a id="ColorProfileAlternatives"></a>12.3.2 Alternative ways of defining a color profile description

Color profile descriptions can be specified in either of the following ways:

- a [‘color-profile’](#ColorProfileElement) element
- an *@color-profile* rule within a CSS style sheet (only applicable for user agents which support using CSS to style the SVG content [[CSS2](refs.md#ref-CSS2)])

If a color profile with the same *name* value has been identified by both a [‘color-profile’](#ColorProfileElement) element and *@color-profile* rules within a CSS style sheet, then the user agent shall first attempt to locate the profile by using the specifications in the *@color-profile* rules first.

### <a id="ColorProfileElement"></a>12.3.3 The ‘color-profile’ element

‘color-profile’

**Categories:**
: None
**Content model:**
: Any number of the following elements, in any order:

- [descriptive elements](intro.md#TermDescriptiveElement) — [‘desc’](struct.md#DescElement), [‘metadata’](metadata.md#MetadataElement), [‘title’](struct.md#TitleElement)

**Attributes:**
:

- [core attributes](intro.md#TermCoreAttributes) — [‘id’](struct.md#IDAttribute), [‘xml:base’](struct.md#XMLBaseAttribute), [‘xml:lang’](struct.md#XMLLangAttribute), [‘xml:space’](struct.md#XMLSpaceAttribute)
- [xlink attributes](intro.md#TermXLinkAttributes) — [‘xlink:href’](#ColorProfileElementHrefAttribute), [‘xlink:show’](linking.md#XLinkShowAttribute), [‘xlink:actuate’](linking.md#XLinkActuateAttribute), [‘xlink:type’](linking.md#XLinkTypeAttribute), [‘xlink:role’](linking.md#XLinkRoleAttribute), [‘xlink:arcrole’](linking.md#XLinkArcRoleAttribute), [‘xlink:title’](linking.md#XLinkTitleAttribute)
- [‘local’](#ColorProfileElementLocalAttribute)
- [‘name’](#ColorProfileElementNameAttribute)
- [‘rendering-intent’](#ColorProfileElementRenderingIntentAttribute)
- [‘xlink:href’](#ColorProfileElementHrefAttribute)

**DOM Interfaces:**
:

- [SVGColorProfileElement](#InterfaceSVGColorProfileElement)

*Attribute definitions:*

**<a id="ColorProfileElementHrefAttribute"></a>xlink:href = "[<iri>](types.md#DataTypeIRI)"**
: The location of an ICC profile resource. [Animatable](animate.md#Animatable): no.
**<a id="ColorProfileElementLocalAttribute"></a>local = "<string>"**
: The unique ID for a locally stored color profile. <string> is the profile's unique ID as specified by International Color Consortium. If both the [‘xlink:href’](#ColorProfileElementHrefAttribute) and the [‘local’](#ColorProfileElementLocalAttribute) attributes are specified, then the user agent shall search the local system for the locally stored color profile first, and, if not available locally, then attempt to use the resource identified by the [‘xlink:href’](#ColorProfileElementHrefAttribute) attribute. (Note: Profile description fields do *not* represent a profile's unique ID. With current ICC proposals, the profile's unique ID is an MD5-encoded value within the profile header.) [Animatable](animate.md#Animatable): no.
**<a id="ColorProfileElementNameAttribute"></a>name = "<name>"**
: The name which is used as the first parameter for icc-color specifications within [‘fill’](painting.md#FillProperty), [‘stroke’](painting.md#StrokeProperty), [‘stop-color’](pservers.md#StopColorProperty), [‘flood-color’](filters.md#FloodColorProperty) and [‘lighting-color’](filters.md#LightingColorProperty) property values to identify the color profile to use for the ICC color specification and the name which can be the value of the [‘color-profile’](#ColorProfileElement) property. Note that if <name> is not provided, it will be impossible to reference the given color profile description. The name "sRGB" is predefined; any color profile descriptions with <name> set to "sRGB" will be ignored. For consistency with CSS lexical scanning and parsing rules ([[CSS2](refs.md#ref-CSS2)], section D.2), the keyword "sRGB" is case-insensitive; however, it is recommended that the mixed capitalization "sRGB" be used for consistency with common industry practice. [Animatable](animate.md#Animatable): no.
**<a id="ColorProfileElementRenderingIntentAttribute"></a>rendering-intent = "auto | perceptual | relative-colorimetric | saturation | absolute-colorimetric"**
:

[‘rendering-intent’](#ColorProfileElementRenderingIntentAttribute) permits the specification of a color profile rendering intent other than the default. [‘rendering-intent’](#ColorProfileElementRenderingIntentAttribute) is applicable primarily to color profiles corresponding to CMYK color spaces. The different options cause different methods to be used for translating colors to the color gamut of the target rendering device:

**auto**
: This is the default behavior. The user agent determines the best intent based on the content type. For image content containing an embedded profile, it shall be assumed that the intent specified within the profile is the desired intent. Otherwise, the user agent shall use the current profile and force the intent, overriding any intent that might be stored in the profile itself.
**perceptual**
: This method, often the preferred choice for images, preserves the relationship between colors. It attempts to maintain relative color values among the pixels as they are mapped to the target device gamut. Sometimes pixel values that were originally within the target device gamut are changed in order to avoid hue shifts and discontinuities and to preserve as much as possible the overall appearance of the scene.
**saturation**
: Preserves the relative saturation (chroma) values of the original pixels. Out of gamut colors are converted to colors that have the same saturation but fall just inside the gamut.
**relative-colorimetric**
: Leaves colors that fall inside the gamut unchanged. This method usually converts out of gamut colors to colors that have the same lightness but fall just inside the gamut.
**absolute-colorimetric**
: Disables white point matching when converting colors. This option is generally not recommended.
[Animatable](animate.md#Animatable): no.
### <a id="ColorProfileAtRule"></a>12.3.4 The CSS **@color-profile** rule

When the document is styled using CSS, the **@color-profile** rule can be used to specify a color profile description. The general form is:

```
@color-profile { <color-profile-description> }
```

where the <color-profile-description> has the form:

```
descriptor: value;
[...]
descriptor: value;
```

Each @color-profile rule specifies a value for every color profile descriptor, either implicitly or explicitly. Those not given explicit values in the rule take the initial value listed with each descriptor in this specification. These descriptors apply solely within the context of the @color-profile rule in which they are defined, and do not apply to document language elements. Thus, there is no notion of which elements the descriptors apply to, or whether the values are inherited by child elements.

The following are the descriptors for a <color-profile-description>:

**<a id="ColorProfileSrcProperty"></a>‘src’**
:

| *Values:* | sRGB \| <local-profile> \| [<iri>](types.md#DataTypeIRI) \| (<local-profile> [<iri>](types.md#DataTypeIRI)) \| inherit |
| --- | --- |
| *Initial:* | sRGB |
| *Media:* | visual |

**sRGB**
: The source profile is the sRGB color space. For consistency with CSS lexical scanning and parsing rules ([[CSS2](refs.md#ref-CSS2)], section D.2), the keyword "sRGB" is case-insensitive; however, it is recommended that the mixed capitalization "sRGB" be used for consistency with common industry practice.
**<local-profile>**
: The source profile is a locally-stored profile. The syntax for <local-profile> is:

```
"local(" + <string> + ")"
```

where <string> is the profile's unique ID as specified by International Color Consortium. (Note: Profile description fields do *not* represent a profile's unique ID. With current ICC proposals, the profile's unique ID is an MD5-encoded value within the profile header.)
**[<iri>](types.md#DataTypeIRI)**
: The source profile is a [IRI reference](linking.md#IRIReference) to a color profile.
**(<local-profile> [<iri>](types.md#DataTypeIRI))**
: Two profiles are specified. If <local-profile> cannot be found on the local system, then the [<iri>](types.md#DataTypeIRI) is used.
**<a id="ColorProfileNameProperty"></a>‘name’**
:

| *Values:* | <name> |
| --- | --- |
| *Initial:* | undefined |
| *Media:* | visual |

**<name>**
: See the description for the [‘name’](#ColorProfileElementNameAttribute) attribute on the [‘color-profile’](#ColorProfileElement) element. Note that if <name> is not provided, it will be impossible to reference the given @color-profile definition.
**<a id="RenderingIntentProperty"></a>‘rendering-intent’**
:

| *Values:* | auto \| perceptual \| relative-colorimetric \| saturation \| absolute-colorimetric |
| --- | --- |
| *Initial:* | auto |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | no |

See the description for the [‘rendering-intent’](#ColorProfileElementRenderingIntentAttribute) attribute on the [‘color-profile’](#ColorProfileElement) element.

### <a id="ColorProfileProperty"></a>12.3.5 The ‘color-profile’ property

**‘color-profile’**
:

| *Value:* | auto \| sRGB \| <name> \| [<iri>](types.md#DataTypeIRI) \| inherit |
| --- | --- |
| *Initial:* | auto |
| *Applies to:* | [‘image’](struct.md#ImageElement) elements that refer to raster images |
| *Inherited:* | yes |
| *Percentages:* | N/A |
| *Media:* | visual |
| *[Animatable](animate.md#Animatable):* | yes |

**auto**
: This is the default behavior. All colors are presumed to be defined in the sRGB color space unless a more precise embedded profile is specified within content data. For images that do have a profile built into their data, that profile is used. For images that do not have a profile, the sRGB profile is used.
**sRGB**
: The source profile is assumed to be sRGB. This differs from auto in that it overrides an embedded profile inside an image. For consistency with CSS lexical scanning and parsing rules ([[CSS2](refs.md#ref-CSS2)], section D.2), the keyword "sRGB" is case-insensitive; however, it is recommended that the mixed capitalization "sRGB" be used for consistency with common industry practice.
**<name>**
: A name corresponding to a defined color profile that is in the user agent's color profile description database. The user agent searches the color profile description database for a [color profile description](#ColorProfileDescriptions) entry whose name descriptor matches <name> and uses the last matching entry that is found. If a match is found, the corresponding profile overrides an embedded profile inside an image. If no match is found, then the embedded profile inside the image is used.
**[<iri>](types.md#DataTypeIRI)**
: A [IRI reference](linking.md#IRIReference) to the source color profile. The referenced color profile overrides an embedded profile inside the image.
## <a id="DOMInterfaces"></a>12.4 DOM interfaces

### <a id="InterfaceSVGColorProfileElement"></a>12.4.1 Interface SVGColorProfileElement

The [SVGColorProfileElement](#InterfaceSVGColorProfileElement) interface corresponds to the [‘color-profile’](#ColorProfileElement) element.

```
interface SVGColorProfileElement : SVGElement,
                                   SVGURIReference,
                                   SVGRenderingIntent {
  attribute DOMString local;
  attribute DOMString name;
  attribute unsigned short renderingIntent;
};
```

**Attributes:**
:

**<a id="__svg__SVGColorProfileElement__local"></a>**local** (DOMString)**
:

Corresponds to attribute [‘local’](#ColorProfileElementLocalAttribute) on the given element.

**<a id="__svg__SVGColorProfileElement__name"></a>**name** (DOMString)**
:

Corresponds to attribute [‘name’](#ColorProfileElementNameAttribute) on the given element.

**<a id="__svg__SVGColorProfileElement__renderingIntent"></a>**renderingIntent** (unsigned short)**
:

Corresponds to attribute [‘rendering-intent’](#ColorProfileElementRenderingIntentAttribute) on the given element. The value of this attribute is the value of the the RENDERING_INTENT_* constant defined on [SVGRenderingIntent](types.md#InterfaceSVGRenderingIntent) that corresponds to the value of the [‘rendering-intent’](#ColorProfileElementRenderingIntentAttribute) attribute.

### <a id="InterfaceSVGColorProfileRule"></a>12.4.2 Interface SVGColorProfileRule

The [SVGColorProfileRule](#InterfaceSVGColorProfileRule) interface represents an @color-profile rule in a CSS style sheet. An @color-profile rule identifies a ICC profile which can be referenced within a given document.

Support for the [SVGColorProfileRule](#InterfaceSVGColorProfileRule) interface is only required in user agents that support [styling with CSS](styling.md#StylingWithCSS).

```
interface SVGColorProfileRule : SVGCSSRule,
                                SVGRenderingIntent {
  attribute DOMString src setraises(DOMException);
  attribute DOMString name setraises(DOMException);
  attribute unsigned short renderingIntent setraises(DOMException);
};
```

**Attributes:**
:

**<a id="__svg__SVGColorProfileRule__src"></a>**src** (DOMString)**
:

Corresponds to descriptor src within an @color-profile rule.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGColorProfileRule__name"></a>**name** (DOMString)**
:

Corresponds to descriptor ‘name’ within an @color-profile rule.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
**<a id="__svg__SVGColorProfileRule__renderingIntent"></a>**renderingIntent** (unsigned short)**
:

The type of rendering intent, identified by one of the [SVGRenderingIntent](types.md#InterfaceSVGRenderingIntent) constants.

**Exceptions on setting**
:

**DOMException, code NO_MODIFICATION_ALLOWED_ERR**
: Raised on an attempt to change the value of a [read only attribute](https://www.w3.org/TR/SVG11/svgdom.html#ReadOnlyNodes).
SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](painting.md) ⋅ [Next](pservers.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
