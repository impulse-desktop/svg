# CSS Box Model Module Level 4

## Box-edge keywords for SVG elements: clip-path-boxes (exists)

- [ ] css-box-4.md#typedef-box: the box-edge keywords content-box, padding-box, border-box, margin-box, fill-box, stroke-box and view-box are used by properties, such as transform-box and background-clip, that need to refer to various box edges
- [ ] css-box-4.md#valdef-box-content-box: in an SVG context, content-box is treated as fill-box; css-box-4.md#valdef-box-padding-box: padding-box is also treated as fill-box
- [ ] css-box-4.md#valdef-box-border-box: in an SVG context, border-box is treated as stroke-box; css-box-4.md#valdef-box-margin-box: margin-box is also treated as stroke-box
- [ ] css-box-4.md#valdef-box-fill-box: fill-box refers to the object bounding box or its edges; css-box-4.md#valdef-box-stroke-box: stroke-box refers to the stroke bounding box or its edges
- [ ] css-box-4.md#valdef-box-view-box: view-box refers to the nearest SVG viewport's origin box: a rectangle sized to the viewport's width and height, positioned with its top-left corner at the coordinate system's origin
- [ ] css-box-4.md#typedef-visual-box: <visual-box> = content-box | padding-box | border-box; <layout-box> adds margin-box; <paint-box> adds fill-box and stroke-box; <coord-box> adds view-box, so a given property's grammar fixes which of these keywords it accepts

## Notes

- css-box-4.md#svg-viewport-origin-box: the spec's own note flags that this origin box does not correspond to the actual SVG viewport when the viewport is not itself anchored at the coordinate system's origin; easy to miss since the keyword is named after the viewport.
