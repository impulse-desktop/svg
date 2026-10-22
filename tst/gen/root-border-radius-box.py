"""An actual root at a different viewBox scale, intersecting two clips.

The radius cuts the drawing, and the explicit clip-path keeps its right
half. The navy root background belongs to the canvas and stays whole.
The expectation is a literal half ellipse over a plain navy rectangle.
"""
from unitgen import Unit

u = Unit('root-border-radius-box', 64, 40,
         'The root CSS box is the raster viewport, independent of viewBox. Its border-radius clips content even with visible overflow, and intersects an explicit root clip-path. The background is propagated to the canvas, so it is not rounded or clipped with the element.',
         root='viewBox="0 0 32 20" style="background:navy;border-radius:50%;overflow:visible;clip-path:inset(0 0 0 32px)"',
         plain_root='', ground=False)
u.cell('canvas-and-intersection', (0, 0, 64, 40),
       '<rect width="32" height="20" fill="lime"/>',
       '<rect width="64" height="40" fill="navy"/><path fill="lime" d="M32 0A32 20 0 0 1 32 40Z"/>')
u.write()
