"""A distant light stays fixed in the output frame when its source turns.

The expected cells draw the same symmetric source in its final position.
The existing fe-lighting-formulas unit checks the lighting formula itself;
this one checks the coordinate frame independently of those formulas.
"""
from unitgen import Unit


u = Unit("fe-distant-light-transform", 254, 194,
         "Reflected and quarter-turned symmetric bumps keep their distant "
         "light's azimuth in the output frame; a point light instead follows "
         "its position in primitiveUnits. Scaling still scales the surface "
         "height in those units.")

for kind, primitive in (
    ("diffuse", '<feDiffuseLighting in="blur" surfaceScale="10" '
     'diffuseConstant="1" lighting-color="white" result="light">'
     '<feDistantLight azimuth="235" elevation="13"/>'
     '</feDiffuseLighting>'),
    ("specular", '<feSpecularLighting in="blur" surfaceScale="10" '
     'specularConstant="1" specularExponent="5" lighting-color="white" '
     'result="light"><feDistantLight azimuth="235" elevation="13"/>'
     '</feSpecularLighting>'),
):
    u.common('<filter id="' + kind + '" filterUnits="userSpaceOnUse" '
             'x="0" y="0" width="60" height="60" '
             'color-interpolation-filters="sRGB">'
             '<feGaussianBlur in="SourceGraphic" stdDeviation="3" result="blur"/>'
             + primitive +
             '<feComposite in="light" in2="SourceGraphic" '
             'operator="arithmetic" k1="1" k3="0.6"/>'
             '</filter>')

shape = '<rect x="10" y="10" width="40" height="40" fill="#9dab89" '
variants = (
    ("identity", ""),
    ("reflect-x", ' transform="translate(60 0) scale(-1 1)"'),
    ("reflect-y", ' transform="translate(0 60) scale(1 -1)"'),
    ("quarter-turn", ' transform="rotate(90 30 30)"'),
)

for row, kind in enumerate(("diffuse", "specular")):
    for column, (name, transform) in enumerate(variants):
        x, y = 2 + column * 63, 2 + row * 64
        open_group = f'<g transform="translate({x} {y})">'
        fixed = shape + f'filter="url(#{kind})"/>'
        turned = shape + f'filter="url(#{kind})"{transform}/>'
        u.cell(f"{kind}-{name}", (x, y, 60, 60),
               open_group + turned + '</g>',
               open_group + fixed + '</g>')

for name, px, py in (("point-local", 15, 20),
                     ("point-reflected", 45, 20),
                     ("point-turned", 40, 15)):
    u.common(f'<filter id="{name}" filterUnits="userSpaceOnUse" '
             'x="0" y="0" width="60" height="60" '
             'color-interpolation-filters="sRGB">'
             '<feGaussianBlur in="SourceGraphic" stdDeviation="3" result="blur"/>'
             '<feDiffuseLighting in="blur" surfaceScale="10" '
             'diffuseConstant="1" lighting-color="white" result="light">'
             f'<fePointLight x="{px}" y="{py}" z="25"/>'
             '</feDiffuseLighting>'
             '<feComposite in="light" in2="SourceGraphic" '
             'operator="arithmetic" k1="1" k3="0.6"/>'
             '</filter>')

for column, (name, transform, target) in enumerate((
    ("point-reflect-x", variants[1][1], "point-reflected"),
    ("point-quarter-turn", variants[3][1], "point-turned"),
)):
    x, y = 2 + column * 63, 130
    open_group = f'<g transform="translate({x} {y})">'
    u.cell(name, (x, y, 60, 60),
           open_group + shape +
           f'filter="url(#point-local)"{transform}/></g>',
           open_group + shape +
           f'filter="url(#{target})"/></g>')

# A doubled source and a same-size direct source have the same surface
# height in output coordinates when the direct one's surfaceScale doubles.
for name, blur, surface in (("scale-local", 2, 5),
                            ("scale-output", 4, 10)):
    u.common(f'<filter id="{name}" color-interpolation-filters="sRGB">'
             f'<feGaussianBlur in="SourceGraphic" stdDeviation="{blur}" '
             'result="blur"/>'
             f'<feDiffuseLighting in="blur" surfaceScale="{surface}" '
             'diffuseConstant="1" lighting-color="white" result="light">'
             '<feDistantLight azimuth="235" elevation="13"/>'
             '</feDiffuseLighting>'
             '<feComposite in="light" in2="SourceGraphic" '
             'operator="arithmetic" k1="1" k3="0.6"/>'
             '</filter>')

x, y = 128, 130
open_group = f'<g transform="translate({x} {y})">'
u.cell("scale-boundary", (x, y, 60, 60),
       open_group + '<rect x="5" y="5" width="20" height="20" '
       'fill="#9dab89" filter="url(#scale-local)" '
       'transform="scale(2)"/></g>',
       open_group + '<rect x="10" y="10" width="40" height="40" '
       'fill="#9dab89" filter="url(#scale-output)"/></g>')

u.write()
