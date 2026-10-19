"""transform-paint-servers: transforms of patterns, gradients and clip
paths, CSS Transforms 1 and 2."""
from unitgen import Unit

unit = Unit("transform-paint-servers", 120, 32, """Each lime drawing fills its
black outline. On a pattern, a gradient and a clipPath the transform
property is their own transform, its presentation attributes
patternTransform, gradientTransform and transform, which it overrides; a
list with a 3D function on them is ignored whole, the translate before it
too. A clip path's content is flattened, a 3D rotation of a child in it
narrowing it, and its children's transforms are whole: about their
origin in their box, with translate, rotate and scale. Below, the
reference box of their transform's percentages and origin is the one
their units name: the bounding box for objectBoundingBox, the viewport
for userSpaceOnUse.""")
unit.common('''<defs><pattern id="a" width="8" height="12" patternUnits="userSpaceOnUse" patternTransform="translate(2 0)" style="transform: translate(4px, 2px)"><rect width="4" height="12" fill="lime"/></pattern>
<pattern id="b" width="8" height="12" patternUnits="userSpaceOnUse" style="transform: translate(4px) rotateZ(0deg)"><rect width="4" height="12" fill="lime"/></pattern>
<linearGradient id="g" gradientUnits="userSpaceOnUse" x1="36" x2="44" style="transform: translate(2px)"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="white"/></linearGradient>
<linearGradient id="h" gradientUnits="userSpaceOnUse" x1="50" x2="58" style="transform: translate(2px) translateZ(0)"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="white"/></linearGradient>
<clipPath id="c" transform="translate(9 0)" style="transform: translate(4px)"><rect x="60" y="2" width="4" height="12"/></clipPath>
<clipPath id="f"><rect x="72" y="2" width="8" height="12" style="transform-box: fill-box; transform-origin: left; transform: rotateY(60deg)"/></clipPath>
<pattern id="pb" width=".5" height="1" style="transform: translate(25%)"><rect width="4" height="12" fill="lime"/></pattern>
<pattern id="pu" patternUnits="userSpaceOnUse" width="16" height="12" style="transform: translate(10%)"><rect width="8" height="12" fill="lime"/></pattern>
<clipPath id="co" clipPathUnits="objectBoundingBox" style="transform: translate(50%)"><rect width=".25" height="1"/></clipPath>
<clipPath id="cu" style="transform: translate(10%)"><rect x="60" y="18" width="4" height="12"/></clipPath>
<linearGradient id="gu" gradientUnits="userSpaceOnUse" x1="80" x2="92" style="transform: translate(-2.5%)"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="white"/></linearGradient>
<clipPath id="cs" clipPathUnits="objectBoundingBox" style="transform-origin: 50% 50%; transform: scale(.5)"><rect width="1" height="1"/></clipPath>
<clipPath id="k"><rect x="84" y="2" width="4" height="4" style="translate: 2px 0"/><rect x="86" y="8" width="8" height="4" style="transform-box: fill-box; transform-origin: center; rotate: 90deg"/></clipPath></defs>''',
            '<rect width="0" height="0"/>')


def cell(name, box, test, rects):
    plain = "".join(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>' for x, y, w, h in rects)
    unit.cell(name, box, test, plain, plain.replace(' fill="lime"', ""))


cell("pattern", (0, 1, 17, 14), '<rect x="0" y="2" width="16" height="12" fill="url(#a)"/>', [(4, 2, 4, 12), (12, 2, 4, 12)])
cell("pattern-3d", (17, 1, 18, 14), '<rect x="18" y="2" width="16" height="12" fill="url(#b)"/>', [(18, 2, 2, 12), (24, 2, 4, 12), (32, 2, 2, 12)])
cell("gradient", (35, 1, 14, 14), '<rect x="36" y="2" width="12" height="12" fill="url(#g)"/>', [(36, 2, 6, 12)])
cell("gradient-3d", (49, 1, 9, 14), '<rect x="50" y="2" width="8" height="12" fill="url(#h)"/>', [(50, 2, 4, 12)])
cell("clip-path", (58, 1, 13, 14), '<rect x="58" y="2" width="12" height="12" fill="lime" clip-path="url(#c)"/>', [(64, 2, 4, 12)])
cell("clip-path-flattened", (71, 1, 14, 14), '<rect x="72" y="2" width="12" height="12" fill="lime" clip-path="url(#f)"/>', [(72, 2, 4, 12)])
cell("clip-path-children", (85, 1, 14, 14), '<rect x="86" y="2" width="12" height="12" fill="lime" clip-path="url(#k)"/>', [(86, 2, 4, 4), (88, 6, 4, 8)])
cell("box-pattern", (1, 17, 36, 14), '<rect x="2" y="18" width="16" height="12" fill="url(#pb)"/><rect x="20" y="18" width="16" height="12" fill="url(#pu)"/>', [(6, 18, 4, 12), (14, 18, 4, 12), (28, 18, 8, 12)])
cell("box-clip-path", (39, 17, 38, 14), '<rect x="40" y="18" width="16" height="12" fill="lime" clip-path="url(#co)"/><rect x="60" y="18" width="16" height="12" fill="lime" clip-path="url(#cu)"/>', [(48, 18, 4, 12), (72, 18, 4, 12)])
unit.cell("box-gradient", (79, 17, 14, 14), '<rect x="80" y="18" width="12" height="12" fill="url(#gu)"/>', '<rect x="80" y="18" width="12" height="12" fill="white"/><rect x="80" y="18" width="3" height="12" fill="lime"/>', '<rect x="80" y="18" width="3" height="12"/>')
cell("box-origin", (95, 17, 18, 14), '<rect x="96" y="18" width="16" height="12" fill="lime" clip-path="url(#cs)"/>', [(100, 21, 8, 6)])
unit.write()
