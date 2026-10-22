"""Zero and small radial circles, checked by their cone geometry.

SVG 2 leaves overlapping circles and the area outside a cone unpainted.
The two nontrivial cones below have a half-angle of 30 degrees. Probe
rectangles are wholly inside or outside by inequalities on their corners;
two equal lime stops make the exact interior color independent of sampling.
Power-of-two scaling gives identical geometry on either side of the old
absolute-radius shortcut. The old gradient-edge input stays unchanged.
"""
from math import sqrt

from unitgen import Unit


u = Unit('gradient-radial-zero', 128, 96,
         'Point circles paint no area. Positive circles retain their cone '
         'under scale. Independent rectangular probes certify transparent '
         'exterior and lime interior, with valid-empty and invalid fallback.',
         ground=False)
small = 1 / 65536
variants = [
    ('zero-concentric', 0, 0, 0, (0, 10, 16, 12), False, {}),
    ('zero-shifted', 0, 0, 2, (0, 10, 16, 12), False, {}),
    ('zero-repeat', 0, 0, 2, (0, 10, 16, 12), False, {'spread': 'repeat'}),
    ('zero-valid-fallback', 0, 0, 2, (0, 10, 16, 12), False, {'fallback': True}),
    ('tiny-covered', small, 2 * small, 0, (0, 10, 16, 12), False, {'scale': 65536}),
    ('tiny-exterior', small, 0, 2 * small, (0, 10, 16, 12), False, {'scale': 65536, 'cone': 'left'}),
    ('tiny-interior', small, 0, 2 * small, (-30, -.5, 10, 1), True, {'scale': 65536, 'cone': 'left'}),
    ('normal-exterior', 1, 0, 2, (0, 10, 16, 12), False, {'cone': 'left'}),
    ('normal-interior', 1, 0, 2, (-30, -.5, 10, 1), True, {'cone': 'left'}),
    ('zero-end-covered', 0, 1, 0, (0, 10, 16, 12), False, {}),
    ('zero-end-exterior', 0, 1, 2, (-30, -.5, 10, 1), False, {'cone': 'right'}),
    ('zero-end-interior', 0, 1, 2, (20, -.5, 10, 1), True, {'cone': 'right'}),
    ('smaller-exterior', 1 / 1048576, 0, 2 / 1048576, (0, 10, 16, 12), False,
     {'scale': 1048576, 'cone': 'left'}),
    ('tiny-transparent', small, 0, 2 * small, (-30, -.5, 10, 1), True,
     {'scale': 65536, 'cone': 'left', 'alpha': .5}),
    ('tiny-linear-exterior', small, 0, 2 * small, (0, 10, 16, 12), False,
     {'scale': 65536, 'cone': 'left', 'linear': True}),
    ('negative-fallback', -small, 0, 0, (0, 10, 16, 12), True,
     {'scale': 65536, 'fallback': True}),
]
for i, (name, radius, focal_radius, focus, box, painted, options) in enumerate(variants):
    if options.get('cone'):
        corners = [(x, y) for x in [box[0], box[0] + box[2]]
                   for y in [box[1], box[1] + box[3]]]
        if options['cone'] == 'left':
            if painted:
                assert all(x < 2 and 3 * y * y < (x - 2) ** 2 for x, y in corners)
            else:
                # This half-plane separates the complete rectangle from the cone.
                assert box[1] > max(abs(box[0] - 2), abs(box[0] + box[2] - 2)) / sqrt(3)
        elif painted:
            assert all(x > 0 and 3 * y * y < x * x for x, y in corners)
        else:
            assert box[0] + box[2] < 0
    x, y = i % 4 * 32, i // 4 * 24
    alpha = options.get('alpha', 1)
    scale = options.get('scale', 1)
    spread = options.get('spread', 'pad')
    linear = ' color-interpolation="linearRGB"' if options.get('linear') else ''
    gradient = (f'<radialGradient id="g{i}" gradientUnits="userSpaceOnUse" '
                f'cx="0" cy="0" r="{radius:.15g}" fx="{focus:.15g}" fy="0" '
                f'fr="{focal_radius:.15g}" gradientTransform="scale({scale})" '
                f'spreadMethod="{spread}"{linear}>'
                '<stop offset="0" stop-color="lime"/>'
                '<stop offset="1" stop-color="lime"/></radialGradient>')
    fallback = ' lime' if options.get('fallback') else ''
    view = ' '.join(str(v) for v in box)
    rect = (f'<rect x="{box[0]}" y="{box[1]}" width="{box[2]}" height="{box[3]}" '
            f'fill="url(#g{i}){fallback}" fill-opacity="{alpha}"/>')
    test = (f'<svg x="{x}" y="{y}" width="32" height="24" viewBox="{view}" '
            f'preserveAspectRatio="none"><defs>{gradient}</defs>{rect}</svg>')
    expected = (f'<rect x="{x}" y="{y}" width="32" height="24" fill="lime" '
                f'fill-opacity="{alpha}"/>' if painted else '')
    u.cell(name, (x, y, 32, 24), test, expected)
u.write()
