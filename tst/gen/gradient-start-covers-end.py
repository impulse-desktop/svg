"""SVG 2 RadialGradientNotes leaves fully overlapping circles untouched.

This includes equal concentric circles and a focal disk containing the
end disk, with an internally tangent boundary. Equal radii at distinct
centers do not fully overlap: their constant-radius circles sweep an
infinite strip. A slightly smaller concentric start disk paints normally.
The expected drawings use only solid rectangles, independent of gradients.
"""
from unitgen import Unit

u = Unit('gradient-start-covers-end', 192, 96,
         'SVG 2 leaves the painted area untouched when the focal disk fully overlaps the end disk, including coincident circles. Spread methods and template inheritance do not change that rule. Distinct centers with equal radii instead sweep a strip; a smaller concentric focal disk still paints. Constant blue stops make all nondegenerate covered areas exactly blue, without relying on gradient interpolation.')
cases = [
    ('coincident-pad', 'r="8" fr="8"', '', 'pad'),
    ('coincident-repeat', 'r="8" fr="8"', '', 'repeat'),
    ('coincident-reflect', 'r="8" fr="8"', '', 'reflect'),
    ('contains-end', 'r="8" fx="28" fr="13"', '', 'pad'),
    ('internal-tangent', 'r="8" fx="28" fr="12"', '', 'pad'),
    ('shifted-equal-radii', 'r="8" fx="28" fr="8"', '<rect x="2" y="16" width="44" height="16" fill="blue"/>', 'pad'),
    ('smaller-start', 'r="8" fr="7.99"', '<rect x="2" y="2" width="44" height="44" fill="blue"/>', 'pad'),
    ('template-percent', '', '', 'pad'),
]
u.common('<defs><radialGradient id="template" r="30%" fr="30%"><stop stop-color="blue"/><stop offset="1" stop-color="blue"/></radialGradient></defs>', '')
for i, (name, attrs, ink, spread) in enumerate(cases):
    x, y = i % 4 * 48, i // 4 * 48
    if name == 'template-percent':
        gradient = f'<radialGradient id="g{i}" href="#template"/>'
    else:
        gradient = f'<radialGradient id="g{i}" gradientUnits="userSpaceOnUse" cx="24" cy="24" {attrs} spreadMethod="{spread}"><stop stop-color="blue"/><stop offset="1" stop-color="blue"/></radialGradient>'
    ground = '<rect x="2" y="2" width="44" height="44" fill="lime"/>'
    u.cell(name, (x, y, 48, 48),
           f'<g transform="translate({x} {y})">{gradient}{ground}<rect x="2" y="2" width="44" height="44" fill="url(#g{i})"/></g>',
           f'<g transform="translate({x} {y})">{ground}{ink}</g>')
u.write()
