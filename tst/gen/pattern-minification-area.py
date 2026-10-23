"""Independent area and compositing controls for a one-pixel pattern tile.

A horizontal .9-wide band in a 4 by 4 tile covers .225 of its area.
A unit-width diagonal band clipped to an 8 by 8 square covers
8*sqrt(2)-.5 square units. Two perpendicular bands overlap in a unit
square, where the later opaque band wins. Integrating those regions
gives the expected color independently of the renderer's sampling and
compositing order. The normal two-level tolerance allows sampling
approximations (D1), independently of browser pixels.
"""
import base64
import math
from unitgen import Unit, png

unit = Unit('pattern-minification-area', 224, 32,
            'Patterns smaller than a device pixel retain their tile area and source-over paint. Horizontal bands, rotations, a half-covered tile, a raster checker and an embedded SVG with crossing colored bands have independent numeric expectations.', ground=False)
embedded = ('<svg xmlns="http://www.w3.org/2000/svg" width="8" height="8">'
            '<rect width="8" height="8" fill="gainsboro"/>'
            '<path d="M0 0L8 8Z" stroke-width="1" stroke="darkkhaki"/>'
            '<path d="M8 0L0 8Z" stroke-width="1" stroke="orange"/></svg>')
uri = 'data:image/svg+xml;base64,' + base64.b64encode(embedded.encode()).decode()
coverage = (8*math.sqrt(2)-.5)/64
overlap = 1/64
ground, first, second = [220]*3, [189, 183, 107], [255, 165, 0]
mixed = [(a*(1-2*coverage+overlap)+b*(coverage-overlap)+c*coverage)/255
         for a,b,c in zip(ground,first,second)]
checker = png([[(0,0,0,1),(1,1,1,1)],[(1,1,1,1),(0,0,0,1)]])
alpha_checker = png([[(1,0,0,1),(0,0,1,128/255)],[(0,1,0,64/255),(1,1,1,0)]])
alpha_sum = 255+128+64
alpha_mean = (255/alpha_sum,64/alpha_sum,128/alpha_sum,alpha_sum/(4*255))
cases = [
    ('band', 4, '<path d="M-1 3H5" fill="none" stroke="black" stroke-width=".9"/>', '', (0,0,0,.225)),
    ('band-rotated', 4, '<path d="M-1 3H5" fill="none" stroke="black" stroke-width=".9"/>', 'rotate(45)', (0,0,0,.225)),
    ('embedded-crossing', 8, f'<image width="8" height="8" href="{uri}"/>', 'scale(.8)', (*mixed,1)),
    ('embedded-rotated', 8, f'<image width="8" height="8" href="{uri}"/>', 'rotate(35) scale(.8)', (*mixed,1)),
    ('half-tile', 2, '<rect width="1" height="2" fill="black"/>', '', (0,0,0,.5)),
    ('raster-checker', 2, f'<image width="2" height="2" href="{checker}"/>', '', (.5,.5,.5,1)),
    ('raster-alpha', 2, f'<image width="2" height="2" href="{alpha_checker}"/>', '', alpha_mean),
]
for i,(name,extent,content,transform,color) in enumerate(cases):
    x = i*32
    test = (f'<svg x="{x}" width="32" height="32" viewBox="0 0 320 320">'
            f'<defs><pattern id="p{i}" width="{extent}" height="{extent}" patternUnits="userSpaceOnUse" patternTransform="{transform}">{content}</pattern></defs>'
            f'<rect width="320" height="320" fill="url(#p{i})"/></svg>')
    unit.cell(name,(x,0,32,32),test,
              f'<image x="{x}" width="32" height="32" href="{png([[color]])}"/>')
unit.write()
