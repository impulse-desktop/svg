# SVG 2, Chapter 16: Linking

## Invalid and missing URL references: use-geometry, paint-fallback, gradient-templates, marker-vertices, pattern-templates, gradient-hrefs, mask-values

- [x] svg2/linking.md#TermCircularReference: a URL reference that directly or indirectly references itself is an invalid circular reference. [use-geometry#circular use-geometry#indirect pattern-templates#self gradient-hrefs#linear-self gradient-hrefs#radial-self]
- [x] svg2/linking.md#TermInvalidReference: a reference is invalid if it is circular, errors while processing, cannot be resolved, or targets an element type inappropriate for where it is used. [use-geometry#circular paint-fallback#missing-fallback pattern-templates#not-pattern gradient-hrefs#linear-other marker-vertices#not-marker]
- [x] svg2/linking.md#TermInvalidReference: an invalid reference is an error only if the referencing attribute or property defines no fallback behavior for it. [paint-fallback#missing-fallback paint-fallback#no-fallback]
- [x] svg2/linking.md#processingURL-target: a URL with no fragment targets the root element of the referenced document. [image-fragment-views#no-fragment]
- [x] svg2/linking.md#processingURL-target: a fragment matching no element in the referenced document (always immutable for a static renderer) is an invalid reference. [paint-fallback#missing-fallback]
- [x] svg2/linking.md#processingURL-validity: an href on linearGradient must target another linearGradient or a radialGradient element, else the reference is invalid. [gradient-hrefs#linear-to-radial gradient-hrefs#linear-other gradient-templates#not-gradient]
- [x] svg2/linking.md#processingURL-validity: an href on pattern must target another pattern element, else the reference is invalid. [pattern-templates#not-pattern pattern-templates#to-gradient]
- [x] svg2/linking.md#processingURL-validity: an href on radialGradient must target a linearGradient or another radialGradient element, else the reference is invalid. [gradient-hrefs#radial-to-linear gradient-hrefs#radial-other]
- [x] svg2/linking.md#processingURL-validity: an href on use must target an SVG-namespaced element, else the reference is invalid. [use-geometry#foreign]
- [x] svg2/linking.md#processingURL-validity: marker, marker-start, marker-mid and marker-end must reference a marker element, else the reference is invalid. [marker-vertices#not-marker]
- [-] svg2/linking.md#processingURL-validity: for a property that may target either an element or an image file (e.g. mask), a target of the wrong element type is instead treated as a reference to the whole file as an image, with any fragment used as for other images. (D20) [mask-values#missing]
- [x] svg2/linking.md#processingURL-validity: in all other cases, a reference whose target element or document type is not allowed for that reference is an invalid reference. [paint-fallback#not-server pattern-templates#to-gradient gradient-hrefs#linear-other]

## Structural href attribute: href-forms

- [x] svg2/linking.md#linkRefAttrsEmbed: the value of an href attribute must be a URL reference, not a CSS url() function value. [href-forms#url-in-href]
- [x] svg2/linking.md#URLforms: URL is not a valid value for presentation attributes; no non-presentation attribute allows the <url> (url() function) syntax. [href-forms#bare-url-attribute]
- [x] svg2/linking.md#XLinkHrefAttribute: when both the unprefixed href and the xlink:href attribute are present, the unprefixed href's value is used and xlink:href is ignored. [href-forms#href-first href-forms#xlink-first]
- [x] svg2/linking.md#processingURL: the legacy xlink:href attribute is honored only when the element has no unprefixed href attribute at all. [href-forms#empty-href href-forms#xlink-alone]

## The 'a' element as a generic container: link-a

- [x] svg2/linking.md#AElement: the 'a' element is a container element and a renderable element. [link-a#href link-a#inherit]
- [x] svg2/linking.md#AElement: 'a' may contain any element its parent may contain except another 'a'; if the parent is a switch, use the content model of the nearest ancestor that isn't a switch. [link-a#nested]
- [x] svg2/linking.md#Links: an 'a' element forms a link only if it has an href or xlink:href attribute; without one it is an inactive placeholder, rendered the same as if active. [link-a#no-href]
- [x] svg2/linking.md#Links: an 'a' element nested inside another hyperlink element (any namespace) has its href ignored and is inactive, but must still be rendered as a generic container element. [link-a#nested]

## SVG view selection by fragment identifier: image-fragments, image-fragment-views

- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: with no SVG fragment identifier, the initial view uses the outermost svg element's own view attributes. [image-fragment-views#no-fragment]
- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: a bare-name fragment addressing a 'view' element displays the root svg element, with that view's viewBox and preserveAspectRatio overriding the root's. [image-fragments#view-element image-fragment-views#bare-name-time]
- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: svgView() parameters override the corresponding root svg attributes/property; a parameter the svgView() spec omits leaves the root's own value, not a reset. [image-fragments#svg-view image-fragment-views#omitted-kept]
- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: svgView() applies its viewBox parameter before its transform parameter. [image-fragments#view-transform]
- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: each of viewBox(), preserveAspectRatio() and transform() occurs at most once in an svgView() spec, in any order; its transform parameter accepts no transform-origin or perspective. [image-fragment-views#repeated image-fragments#svg-view image-fragment-views#no-origin image-fragments#invalid-fragment]
- [x] svg2/linking.md#MediaFragments: a spatial (xywh=) media fragment overrides the outermost svg element's viewBox with its x,y,width,height, leaving its other view attributes as the root's. [image-fragments#media-fragment image-fragment-views#time-first]
- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: a percent: spatial media fragment resolves its values against the referenced SVG's resolved width and height. (D19) [image-fragments#media-fragment]
- [x] svg2/linking.md#SVGFragmentIdentifiersDefinitions: a fragment identifier is at most one of a bare name, an svgView() spec or a spatial segment (optionally combined with a time segment). [image-fragment-views#two-views image-fragments#time-segment image-fragment-views#time-first]

## Notes

- svg2/linking.md#issue18: the spec marks invalid nested-link rendering itself as "at risk", likely to be revised together with unknown-element rendering rules.
- svg2/linking.md#processingURL-absolute: this text says xml:base affects relative URL resolution in XML documents, but svg2/changes.md records that SVG 2 removed the xml:base attribute — contradicts itself.
- svg2/linking.md#SVGFragmentIdentifiersDefinitions: a bare-name fragment addressing a non-'view' element has no defined effect on the initial view in this draft (SVG 1.1 scrolled/zoomed to the element); ambiguous in a way that changes pixels.
- svg2/linking.md#MediaFragments: a percent: xywh fragment falls back to CSS replaced-element width/ratio rules when the host can't resolve the SVG's size — a host-layout algorithm outside this renderer's API, which has no DPR/host box.
