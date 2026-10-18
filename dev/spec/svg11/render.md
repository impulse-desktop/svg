<!-- https://www.w3.org/TR/SVG11/render.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](concepts.md) ⋅ [Next](types.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# 3 Rendering Model

## <a id="Introduction"></a>3.1 Introduction

Implementations of SVG are expected to behave as though they implement a rendering (or imaging) model corresponding to the one described in this chapter. A real implementation is not required to implement the model in this way, but the result on any device supported by the implementation shall match that described by this model.

The appendix on [conformance requirements](conform.md) describes the extent to which an actual implementation may deviate from this description. In practice an actual implementation will deviate slightly because of limitations of the output device (e.g. only a limited range of colors might be supported) and because of practical limitations in implementing a precise mathematical model (e.g. for realistic performance curves are approximated by straight lines, the approximation need only be sufficiently precise to match the conformance requirements).

## <a id="PaintersModel"></a>3.2 The painters model

SVG uses a "painters model" of rendering. [Paint](intro.md#TermPaint) is applied in successive operations to the output device such that each operation paints over some area of the output device. When the area overlaps a previously painted area the new paint partially or completely obscures the old. When the paint is not completely opaque the result on the output device is defined by the (mathematical) rules for compositing described under [Alpha Blending](masking.md#SimpleAlphaBlending).

## <a id="RenderingOrder"></a>3.3 Rendering Order

Elements in an SVG document fragment have an implicit drawing order, with the first elements in the SVG document fragment getting "painted" first. Subsequent elements are painted on top of previously painted elements.

## <a id="Grouping"></a>3.4 How groups are rendered

Grouping elements such as the [‘g’](struct.md#GElement) element (see [container elements](intro.md#TermContainerElement)) have the effect of producing a temporary separate canvas initialized to transparent black onto which child elements are painted. Upon the completion of the group, any [filter effects](#FilteringPaintRegions) specified for the group are applied to create a modified temporary canvas. The modified temporary canvas is composited into the background, taking into account any group-level [masking](masking.md#Masking) and [opacity](masking.md#OpacityProperty) settings on the group.

## <a id="Elements"></a>3.5 How elements are rendered

Individual [graphics elements](intro.md#TermGraphicsElement) are rendered as if each graphics element represented its own group; thus, the effect is as if a temporary separate canvas is created for each graphics element. The element is first painted onto the temporary canvas (see [Painting shapes and text](#PaintingShapesAndText) and [Painting raster images](#PaintingRasterImages) below). Then any [filter effects](#FilteringPaintRegions) specified for the graphics element are applied to create a modified temporary canvas. The modified temporary canvas is then composited into the background, taking into account any [clipping, masking and object opacity](#ClippingMaskingObjectOpacity) settings on the graphics element.

## <a id="TypesOfGraphicsElements"></a>3.6 Types of graphics elements

SVG supports three fundamental types of [graphics elements](intro.md#TermGraphicsElement) that can be rendered onto the canvas:

- [Shapes](intro.md#TermShape), which represent some combination of straight line and curves
- Text, which represents some combination of character glyphs
- Raster images, which represent an array of values that specify the paint color and opacity (often termed alpha) at a series of points on a rectangular grid. (SVG requires support for specified raster image formats under [conformance requirements](conform.md).)

### <a id="PaintingShapesAndText"></a>3.6.1 Painting shapes and text

Shapes and text can be [filled](intro.md#TermFill) (i.e., apply paint to the interior of the shape) and [stroked](intro.md#TermStroke) (i.e., apply paint along the outline of the shape). A stroke operation is centered on the outline of the object; thus, in effect, half of the paint falls on the interior of the shape and half of the paint falls outside of the shape.

For certain types of shapes, [marker symbols](painting.md#Markers) (which themselves can consist of any combination of shapes, text and images) can be drawn at selected vertices. Each marker symbol is painted as if its graphical content were expanded into the SVG document tree just after the shape object which is using the given marker symbol. The graphical contents of a marker symbol are rendered using the same methods as graphics elements. Marker symbols are not applicable to text.

The fill is painted first, then the stroke, and then the marker symbols. The marker symbols are rendered in order along the outline of the shape, from the start of the shape to the end of the shape.

Each fill and stroke operation has its own opacity settings; thus, you can fill and/or stroke a shape with a semi-transparently drawn solid color, with different opacity values for the fill and stroke operations.

The fill and stroke operations are entirely independent painting operations; thus, if you both fill and stroke a shape, half of the stroke will be painted on top of part of the fill.

SVG supports the following built-in types of paint which can be used in fill and stroke operations:

- [Solid color](color.md#ColorIntroduction)
- [Gradients](pservers.md#Gradients) (linear and radial)
- [Patterns](pservers.md#Patterns)

### <a id="PaintingRasterImages"></a>3.6.2 Painting raster images

When a raster image is rendered, the original samples are "resampled" using standard algorithms to produce samples at the positions required on the output device. Resampling requirements are discussed under [conformance requirements](conform.md).

## <a id="FilteringPaintRegions"></a>3.7 Filtering painted regions

SVG allows any painting operation to be filtered. (See [Filter Effects](filters.md).)

In this case the result must be as though the paint operations had been applied to an intermediate canvas initialized to transparent black, of a size determined by the rules given in [Filter Effects](filters.md) then filtered by the processes defined in [Filter Effects](filters.md).

## <a id="ClippingMaskingObjectOpacity"></a>3.8 Clipping, masking and object opacity

SVG allows any painting operation to be limited to a subregion of the output device by clipping and masking. This is described in [Clipping, Masking and Compositing](masking.md).

Clipping uses a path to define a region of the output device to which paint can be applied. Any painting operation executed within the scope of the clipping must be rendered such that only those parts of the device that fall within the clipping region are affected by the painting operation. A clipping path can be thought of as a mask wherein those pixels outside the clipping path are black with an alpha value of zero and those pixels inside the clipping path are white with an alpha value of one. "Within" is defined by the same rules used to determine the interior of a path for painting. The clipping path is typically anti-aliased on low-resolution devices (see [‘shape-rendering’](painting.md#ShapeRenderingProperty). Clipping is described in [Clipping paths](masking.md#ClippingPaths).

Masking uses the luminance of the color channels and alpha channel in a referenced SVG element to define a supplemental set of alpha values which are multiplied to the alpha values already present in the graphics to which the mask is applied. Masking is described in [Masking](masking.md#Masking).

A supplemental masking operation may also be specified by applying a "global" opacity to a set of rendering operations. In this case the mask is infinite, with a color of white and an alpha channel of the given opacity value. (See the [‘opacity’](masking.md#OpacityProperty) property.)

In all cases the SVG implementation must behave as though all painting and filtering is first performed to an intermediate canvas which has been initialized to transparent black. Then, alpha values on the intermediate canvas are multiplied by the implicit alpha values from the clipping path, the alpha values from the mask, and the alpha values from the [‘opacity’](masking.md#OpacityProperty) property. The resulting canvas is composited into the background using [simple alpha blending](masking.md#SimpleAlphaBlending). Thus if an area of the output device is painted with a group opacity of 50% using opaque red paint followed by opaque green paint the result is as though it had been painted with just 50% opaque green paint. This is because the opaque green paint completely obscures the red paint on the intermediate canvas before the intermediate as a whole is rendered onto the output device.

## <a id="ParentCompositing"></a>3.9 Parent Compositing

SVG document fragments can be semi-opaque. In many environments (e.g., Web browsers), the SVG document fragment has a final compositing step where the document as a whole is blended translucently into the background canvas.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](concepts.md) ⋅ [Next](types.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
