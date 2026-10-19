# SVG 2, Chapter 12: Embedded Content

## Positioning rectangle and sizing: image-placement, image-cells, measure-image (exists)

- [x] svg2/embedded.md#TermPositioningRectangle: x, y, width and height place the embedded content in the positioning rectangle, used as the element's bounding box even though graphics may overflow it per overflow. [image-placement#one-dimension image-placement#natural]
- [x] svg2/embedded.md#Placement: for a single referenced resource, the positioning rectangle after transforms gives the specified size; the default sizing algorithm derives a concrete object size/position from object-fit/object-position, which may extend past the rectangle, and overflow then decides whether the excess is clipped. [image-placement#one-dimension image-object-fit#contain]
- [x] svg2/embedded.md#Placement: a width or height of zero disables rendering of the element and its embedded content. [image-placement#zero]
- [x] svg2/embedded.md#Placement: a width or height of auto sizes the element from the resource's intrinsic dimensions or aspect ratio by the default sizing algorithm; a resource with no intrinsic size is assumed 300x150. [image-placement#natural image-placement#default-size]
- [x] svg2/embedded.md#Placement: CSS positioning properties such as top and margin have no effect placing the embedded element in the SVG coordinate system. [image-cells#css-position]

## Image href and document reference: image-self, image-hrefs (exists)

- [x] svg2/embedded.md#ImageElementHrefAttribute: href is a URL reference identifying the image to render, fetched as described in the Linking chapter. [image-placement#natural]
- [x] svg2/embedded.md#ImageElement: a target-only URL (a fragment with no path) is a reference to the same document, rendered again as an independent embedded image. [image-self#self-copy]
- [x] svg2/embedded.md#ImageElement: unlike use, image cannot reference an element by id inside an SVG file; its reference is always to the whole document, optionally reduced by a view fragment. [image-fragments#view-element image-self#self-copy]

## Recursive self-embedding cut by secure processing: image-self, image-svg-depth (exists)

- [x] svg2/embedded.md#ImageElement: an SVG referenced by image is processed in a secure mode, so embedded references inside it are not processed, stopping recursive self-embedding after one level. [image-self#self-copy]

## Image channel result and non-inheritance: image-placement

- [x] svg2/embedded.md#ImageElement: processing an image element always yields a four-channel RGBA result. [image-placement#channels]
- [x] svg2/embedded.md#ImageElement: a 3-channel (RGB) source gets its alpha set uniformly to 1; a single-channel (grayscale) source becomes R=G=B from that channel with alpha 1. [image-placement#channels mask-values#single-channel]
- [x] svg2/embedded.md#ImageElement: the referenced resource is a separate document, so properties do not inherit from the referencing document into the image. [image-placement#not-inherited]

## preserveAspectRatio and the image-rendering rectangle: image-cells, image-placement, image-fragments, image-svg (exists)

- [x] svg2/embedded.md#ImageElement: preserveAspectRatio on image scales/positions the referenced image within the concrete object size, producing the image-rendering rectangle used to render it; for an embedded SVG this rectangle is also its SVG viewport. [image-placement#meet image-placement#channels]
- [x] svg2/embedded.md#ImageElement: preserveAspectRatio is applied after the concrete object size is set and matters only when that size doesn't match the resource's intrinsic aspect ratio; it must be explicitly none to disable aspect-ratio control regardless of object-fit. [image-placement#meet]
- [x] svg2/embedded.md#ImageElement: the aspect ratio used is the resource's intrinsic aspect ratio (viewBox/size derived for SVG, pixel width/height for raster); with no intrinsic aspect ratio (e.g. SVG lacking both viewBox and explicit root size) preserveAspectRatio is ignored and the image fills the positioning rectangle. [image-placement#ratio-width image-placement#default-size]
- [x] svg2/embedded.md#ImageElement: preserveAspectRatio on the root of a referenced SVG image must be ignored and treated as none, so only the referencing image's preserveAspectRatio controls the fit. [image-placement#meet]
- [x] svg2/embedded.md#ImageElement: the user-agent stylesheet sets overflow:hidden on image, clipping it to the positioning rectangle unless the author overrides it. [image-cells#image-overflow]
- [x] svg2/embedded.md#ImageElement: for an embedded SVG, overflow on image clips the image-rendering rectangle to the positioning rectangle, and overflow on the referenced root separately clips its graphics to the image-rendering rectangle. [image-cells#image-overflow]
- [x] svg2/embedded.md#ImageElement: a fragment on the image's URL may select a view of a referenced SVG (Linking chapter) or crop a raster per basic media fragments; either may change the intrinsic size or aspect ratio used. [image-fragments#svg-view image-fragments#media-fragment image-fragments#view-element]

## Notes

- svg2/embedded.md#Placement: the concrete object size is defined by reference to CSS Images 3's full Default Sizing Algorithm (object-fit/object-position), substantially more machinery than SVG's own preserveAspectRatio; worth confirming object-fit/object-position are in scope before building it out for one rectangle-fit case.
