<!-- https://www.w3.org/TR/SVG11/minimize.html, as of 2026-10-18; written by dev/spec_text.py -->

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](i18n.md) ⋅ [Next](refs.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)

# Appendix J: Minimizing SVG File Sizes

*This appendix is informative, not normative.*

Considerable effort has been made to make SVG file sizes as small as possible while still retaining the benefits of XML and achieving compatibility and leverage with other W3C specifications.

Here are some of the features in SVG that promote small file sizes:

- SVG's path data definition was defined to produce a compact data stream for vector graphics data: all commands are one character in length; relative coordinates are available; separator characters do not have to be supplied when tokens can be identified implicitly; smooth curve formulations are available (cubic Béziers, quadratic Béziers and elliptical arcs) to prevent the need to tesselate into polylines; and shortcut formulations exist for common forms of cubic Bézier segments, quadratic Bézier segments, and horizontal and vertical straight line segments so that the minimum number of coordinates need to be specified.
- Text can be specified using XML character data — no need to convert to outlines.
- SVG contains a facility for defining symbols once and referencing them multiple times using different visual attributes and different sizing, positioning, clipping and client-side filter effects
- User agents that support [styling with CSS](styling.md#StylingWithCSS) can use CSS selectors and property inheritance to define commonly used sets of attributes once as named styles.
- Filter effects allow for compelling visual results and effects typically found only in image-authoring tools using small amounts of vector and/or raster data

Additionally, HTTP/1.1 allows for compressed data to be passed from server to client, which can result in significant file size reduction. Here are some sample compression results using gzip compression on SVG documents [[RFC1952](refs.md#ref-RFC1952)]:

| Uncompressed SVG | With gzip compression | Compression ratio |
| --- | --- | --- |
| 12,912 | 2,463 | 81% |
| 12,164 | 2,553 | 79% |
| 11,613 | 2,617 | 77% |
| 18,689 | 4,077 | 78% |
| 13,024 | 2,041 | 84% |

A related issue is progressive rendering. Some SVG viewers will support:

- the ability to display the first parts of an SVG document fragments as the remainder of the document is downloaded from the server; thus, the user will see part of the SVG drawing right away and interact with it, even if the SVG file size is large.
- delayed downloading of images and fonts. Just like some HTML browsers, some SVG viewers will download images and WebFonts ([[CSS2](refs.md#ref-CSS2)], section 15.1) last, substituting a temporary image and system fonts, respectively, until the given image and/or font is available.

Here are techniques for minimizing SVG file sizes and minimizing the time before the user is able to start interacting with the SVG document fragments:

- Construct the SVG file such that any links which the user might want to click on are included at the beginning of the SVG file
- Use default values whenever possible rather than defining all attributes and properties explicitly.
- Take advantage of the [path data](paths.md#PathData) data compaction facilities: use relative coordinates; use *h* and *v* for horizontal and vertical lines; use *s* or *t* for cubic and quadratic Bézier segments whenever possible; eliminate extraneous white space and separators.
- Utilize symbols if the same graphic appears multiple times in the document
- For user agents that support [styling with CSS](styling.md#StylingWithCSS), utilize CSS property inheritance and selectors to consolidate commonly used properties into named styles or to assign the properties to a parent [‘g’](struct.md#GElement) element.
- Utilize filter effects to help construct graphics via client-side graphics operations.

SVG 1.1 (Second Edition) – 16 August 2011[Top](index.md) ⋅ [Contents](index.md) ⋅ [Previous](i18n.md) ⋅ [Next](refs.md) ⋅ [Elements](eltindex.md) ⋅ [Attributes](attindex.md) ⋅ [Properties](propidx.md)
