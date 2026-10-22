"""A tiny curved continuation retains its tangent and rounded outer sweep.

The second control point lies just beyond the start, on the incoming
vertical tangent. Although the first derivative vanishes at the start,
the limiting tangent is vertical, so this is no explicit right-angle
join. The tiny curve's outer normals approach a quarter-circle. Erasing
its control-point displacement instead creates a real miter corner.
Sketchy-SVG f89fe40d61f299c8 contains this distinction at (283, 271).
"""
from unitgen import Unit

u = Unit('stroke-tiny-smooth-corner', 320, 160,
         'Nonzero control-point displacement retains the limiting tangent through a tiny smooth bend. Its outer normal sweep approaches a quarter-circle; both directions give the same stroke. Exactly zero displacement instead makes an explicit miter join. Plain rectangles and a circle construct the independent expectations; small curvature and raster approximation are allowed within 4 levels (D1).', tolerance=4)
for col, epsilon in enumerate([.001, .0001, .00001, 0]):
    for reverse in [False, True]:
        x, y = col * 80, int(reverse) * 80
        c = f'C20 20 20 {20+epsilon:.8f} 20.18 20'
        if reverse:
            d = f'M38 20H20.18C20 {20+epsilon:.8f} 20 20 20 20V2'
        else:
            d = 'M20 2V20' + c + 'H38'
        test = f'<path transform="translate({x} {y}) scale(2)" d="{d}" fill="none" stroke="black" stroke-width="12"/>'
        outer = '<circle cx="20" cy="20" r="6"/>' if epsilon else '<rect x="14" y="20" width="6" height="6"/>'
        expected = f'<g transform="translate({x} {y}) scale(2)"><rect x="14" y="2" width="12" height="18"/><rect x="20" y="14" width="18" height="12"/>{outer}</g>'
        name = f'{"reverse" if reverse else "forward"}-{col}'
        u.cell(name, (x, y, 80, 80), test, expected)
u.write()
