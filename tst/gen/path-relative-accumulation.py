"""Cancelling relative displacements retain the authored closing direction.

The six vertical endpoint displacements are 1.556, 8.767, 1.557,
-1.557, -8.767 and -1.556: their sum is exactly zero. The last x
coordinate is deliberately .00005 beyond the start, so the closing line
is real and horizontal. Literal absolute decimal control points give the
same geometry independently of relative-coordinate summation. Added curved
cases return exactly in decimal arithmetic, including paths starting at
zero and a stationary endpoint with coincident final control and endpoint.
"""
from unitgen import Unit

relative = 'm2.468 13.617h8.768c0.862 0 1.556 0.694 1.556 1.556v8.767c0 0.863-0.694 1.557-1.556 1.557h-8.768c-0.8621 0-1.5562-0.694-1.5562-1.557l0.00005-8.767c0-0.862 0.69405-1.556 1.5562-1.556z'
absolute = 'M2.468 13.617H11.236C12.098 13.617 12.792 14.311 12.792 15.173V23.94C12.792 24.803 12.098 25.497 11.236 25.497H2.468C1.6059 25.497 .9118 24.803 .9118 23.94L.91185 15.173C.91185 14.311 1.6059 13.617 2.46805 13.617Z'
unit = Unit('path-relative-accumulation', 192, 320,
            'Relative decimal coordinates retain exact cancellations and stationary curve endpoints. Literal absolute control points independently specify the geometry, including a real .00005-unit closing segment, curves returning to zero and different stroke orientations. Long coefficients and wide exponent ranges agree with plain rectangles.', ground=False)
for row, width in enumerate([3.125, 8.125]):
    for col, frame in enumerate(['', 'translate(16 0) scale(-1 1)', 'rotate(90 8 20)']):
        x, y = 64*col, 64*row
        common = f'transform="{frame}" fill="#253f5a" stroke="#d23550" stroke-width="{width}" stroke-linejoin="round"'
        def draw(data):
            return f'<svg x="{x}" y="{y}" width="64" height="64" viewBox="-4 8 24 24"><path {common} d="{data}"/></svg>'
        unit.cell(f'width-{row}-orientation-{col}', (x, y, 64, 64), draw(relative), draw(absolute))

# Exact decimal cancellations and an endpoint whose final control is the
# endpoint itself. Absolute control points are calculated from the authored
# decimal values, rather than from a renderer's binary arithmetic.
extra = [
    ('hat-return', '10 0 50 25', 'fill:#00a200;fill-opacity:1;stroke:#007b00;stroke-width:3.34994841px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1', 'm 18.538869,14.435233 c 36.082099,0 36.082099,0 36.082099,0 0,0 -0.5867,-9.9589789 -20.534529,-9.9589789 -19.947828,0 -15.54757,9.9589789 -15.54757,9.9589789 z', 'M 18.538868999999998 14.435233 C 54.620967999999998 14.435233 54.620967999999998 14.435233 54.620967999999998 14.435233 C 54.620967999999998 14.435233 54.034267999999997 4.4762541000000002 34.086438999999999 4.4762541000000002 C 14.138610999999999 4.4762541000000002 18.538868999999998 14.435233 18.538868999999998 14.435233 Z'),
    ('fire-return', '0 1036 16 16', 'fill:#d40000;fill-rule:evenodd;stroke:#ff6600;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1', 'm 7.2245955,1051.2675 c -1.4609099,-1.878 -2.2792938,-5.2423 -1.5722949,-6.6076 0.6059064,-1.17 1.3126236,-1.4752 2.049954,-3.3237 0.5555339,-1.3927 0.2435412,-2.4714 0.2388297,-4.0004 1.1310322,0.7541 2.5154747,2.769 2.6669307,4.1199 0.26544,2.3675 0.169642,4.8247 -0.597074,5.9707 -0.8441056,1.2617 -1.9859063,2.3414 -2.7863455,3.8411 z', 'M 7.2245955000000004 1051.2674999999999 C 5.7636855999999996 1049.3895 4.9453016999999999 1046.0252 5.6523006000000002 1044.6599000000001 C 6.2582069999999996 1043.4899 6.9649241999999996 1043.1847 7.7022545999999998 1041.3362 C 8.2577885000000002 1039.9435000000001 7.9457958 1038.8648000000001 7.9410843 1037.3358000000001 C 9.0721164999999999 1038.0898999999999 10.456559 1040.1048000000001 10.608015 1041.4557 C 10.873455 1043.8232 10.777657 1046.2804000000001 10.010941000000001 1047.4264000000001 C 9.1668354000000001 1048.6881000000001 8.0250347000000009 1049.7678000000001 7.2245955000000004 1051.2674999999999 Z'),
    ('hat-return-zero', '-10 -15 50 25', 'fill:#00a200;fill-opacity:1;stroke:#007b00;stroke-width:3.34994841px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1', 'm0 0 c 36.082099,0 36.082099,0 36.082099,0 0,0 -0.5867,-9.9589789 -20.534529,-9.9589789 -19.947828,0 -15.54757,9.9589789 -15.54757,9.9589789 z', 'M 0 0 C 36.082098999999999 0 36.082098999999999 0 36.082098999999999 0 C 36.082098999999999 0 35.495398999999999 -9.9589789 15.54757 -9.9589789 C -4.400258 -9.9589789 0 0 0 0 Z'),
    ('fire-return-zero', '-8 -16 20 20', 'fill:#d40000;fill-rule:evenodd;stroke:#ff6600;stroke-width:1px;stroke-linecap:butt;stroke-linejoin:miter;stroke-opacity:1', 'm0 0 c -1.4609099,-1.878 -2.2792938,-5.2423 -1.5722949,-6.6076 0.6059064,-1.17 1.3126236,-1.4752 2.049954,-3.3237 0.5555339,-1.3927 0.2435412,-2.4714 0.2388297,-4.0004 1.1310322,0.7541 2.5154747,2.769 2.6669307,4.1199 0.26544,2.3675 0.169642,4.8247 -0.597074,5.9707 -0.8441056,1.2617 -1.9859063,2.3414 -2.7863455,3.8411 z', 'M 0 0 C -1.4609099000000001 -1.8779999999999999 -2.2792938 -5.2423000000000002 -1.5722948999999999 -6.6075999999999997 C -0.96638849999999998 -7.7775999999999996 -0.25967129999999999 -8.0828000000000007 0.4776591 -9.9313000000000002 C 1.033193 -11.324 0.72120030000000002 -12.402699999999999 0.71648880000000004 -13.931699999999999 C 1.847521 -13.1776 3.2319635 -11.162699999999999 3.3834195 -9.8117999999999999 C 3.6488594999999999 -7.4443000000000001 3.5530615000000001 -4.9870999999999999 2.7863454999999999 -3.8411 C 1.9422398999999999 -2.5794000000000001 0.80043920000000002 -1.4997 0 0 Z'),
    ('stationary-end', '0 40 40 32', 'fill:none;stroke:#000000;stroke-width:5.88342333;stroke-linecap:butt;stroke-linejoin:miter;stroke-miterlimit:4;stroke-opacity:1;stroke-dasharray:none', 'm 31.902619,60.577228 c 0,0 -21.003735,-0.718334 -18.519421,-2.873339 2.484314,-2.155003 1.806771,-1.316947 1.806771,-1.316947', 'M 31.902619000000001 60.577227999999998 C 31.902619000000001 60.577227999999998 10.898884000000001 59.858893999999999 13.383198 57.703888999999997 C 15.867512 55.548886000000003 15.189969 56.386941999999998 15.189969 56.386941999999998'),
]
for i, (name, viewbox, style, data, plain) in enumerate(extra):
    x, y = i % 3 * 64, 128 + i // 3 * 64
    def draw(path):
        return f'<svg x="{x}" y="{y}" width="64" height="64" viewBox="{viewbox}"><path style="{style}" d="{path}"/></svg>'
    unit.cell(name, (x, y, 64, 64), draw(data), draw(plain))

# Long coefficients and wide exponent ranges exercise the binary fallback
# without imposing an exact-integer requirement on the rendered pixels.
limit = 9223372036854775807
boundaries = [
    ('long-coefficient', '0 0 16 16', 'M1.2345678901234567890123456789 1h4v4h-4z', (4.938271560493827156, 4, 16, 16)),
    ('positive-large-sum', f'0 0 {4*limit} {4*limit}', f'M{limit} {limit}h{limit}v{limit}h-{limit}z', (16, 16, 16, 16)),
    ('negative-large-sum', f'{-4*limit} {-4*limit} {4*limit} {4*limit}', f'M-{limit} -{limit}h-{limit}v-{limit}h{limit}z', (32, 32, 16, 16)),
    ('exponent-extremes', '0 0 16 16', 'M0e10000 1e-10000h4v4h-4z', (0, 0, 16, 16)),
]
for i, (name, viewbox, data, box) in enumerate(boundaries, 11):
    x, y = i % 3 * 64, i // 3 * 64
    left, top, width, height = box
    test = f'<svg x="{x}" y="{y}" width="64" height="64" viewBox="{viewbox}"><path d="{data}" fill="lime"/></svg>'
    plain = f'<rect x="{x+left:.17g}" y="{y+top:.17g}" width="{width}" height="{height}" fill="lime"/>'
    unit.cell(name, (x, y, 64, 64), test, plain)
unit.write()
