"""Small authored geometry must survive large coordinates and CTMs.

Expected drawings use literal small rectangles, circles and ellipses. Their
corners follow the declared viewBox scale, exact quarter/half turns and
translations, independently of any renderer's rounded matrix or path data.
Every cell is drawn at its initial 1:1 size, including its source viewport.
"""
from unitgen import Unit


unit = Unit('coordinate-precision', 320, 448,
            'Retain small geometry, viewBox extents, transform translations '
            'and stroke offsets at large world coordinates. Both signs, '
            'relative paths, basic shapes, curves, images, clip paths and '
            'symbol/use instances retain their exact small geometry.',
            ground=False)
cases = [
    ('viewbox-extent', '16777216 0 3 4', '<path d="M16777218 1H16777220V2H16777218Z" fill="lime"/>', '<rect x="42.6666666666666667" y="16" width="21.3333333333333333" height="16" fill="lime"/>'),
    ('path-decimals', '5000000 5000000 4 4', '<path d="M5000000.25 5000000.25H5000000.75V5000000.75H5000000.25Z" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('rect-decimals', '5000000 5000000 4 4', '<rect x="5000000.25" y="5000000.25" width=".5" height=".5" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('nested-ctm', '5000000 5000000 8 8', '<g transform="translate(.125 .125)"><g transform="translate(.125 .125)"><g transform="translate(.125 .125)"><g transform="translate(.125 .125)"><path d="M5000000.5 5000000.5H5000001V5000001H5000000.5Z" fill="lime"/></g></g></g></g>', '<rect x="8" y="8" width="4" height="4" fill="lime"/>'),
    ('stroke-offset', '5000000 5000000 4 4', '<path d="M5000001 5000001H5000003" fill="none" stroke="lime" stroke-width=".125"/>', '<rect x="16" y="15" width="32" height="2" fill="lime"/>'),
    ('negative', '-5000004 -5000004 4 4', '<path d="M-5000003.75 -5000003.75H-5000003.25V-5000003.25H-5000003.75Z" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('relative', '5000000 5000000 4 4', '<path d="M5000000.25 5000000.25h.5v.5h-.5z" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('polygon', '5000000 5000000 4 4', '<polygon points="5000000.25,5000000.25 5000000.75,5000000.25 5000000.75,5000000.75 5000000.25,5000000.75" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('polyline-stroke', '5000000 5000000 4 4', '<polyline points="5000001,5000001 5000003,5000001" fill="none" stroke="lime" stroke-width=".125"/>', '<rect x="16" y="15" width="32" height="2" fill="lime"/>'),
    ('cubic', '5000000 5000000 4 4', '<path d="M5000000.25 5000000.25C5000000.5 5000000.25 5000000.5 5000000.25 5000000.75 5000000.25V5000000.75H5000000.25Z" fill="lime"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('circle', '5000000 5000000 4 4', '<circle cx="5000001.25" cy="5000001.25" r=".5" fill="lime"/>', '<circle cx="20" cy="20" r="8" fill="lime"/>'),
    ('ellipse', '5000000 5000000 4 4', '<ellipse cx="5000001.25" cy="5000001.25" rx=".5" ry=".25" fill="lime"/>', '<ellipse cx="20" cy="20" rx="8" ry="4" fill="lime"/>'),
    ('arc', '5000000 5000000 4 4', '<path d="M5000001.75 5000001.25A.5 .5 0 1 1 5000000.75 5000001.25A.5 .5 0 1 1 5000001.75 5000001.25Z" fill="lime"/>', '<circle cx="20" cy="20" r="8" fill="lime"/>'),
    ('css-nested', '5000000 5000000 8 8', '<g style="transform:translate(.125px,.125px)"><g style="transform:translate(.125px,.125px)"><g style="transform:translate(.125px,.125px)"><g style="transform:translate(.125px,.125px)"><path d="M5000000.5 5000000.5H5000001V5000001H5000000.5Z" fill="lime"/></g></g></g></g>', '<rect x="8" y="8" width="4" height="4" fill="lime"/>'),
    ('rotation-pivot', '5000000 5000000 4 4', '<g transform="rotate(180 5000001.25 5000001.25)"><path d="M5000000.5 5000000.5H5000001V5000001H5000000.5Z" fill="lime"/></g>', '<rect x="24" y="24" width="8" height="8" fill="lime"/>'),
    ('clip', '5000000 5000000 4 4', '<defs><clipPath id="c"><rect x="5000000.75" y="5000000.5" width=".5" height="1"/></clipPath></defs><rect x="5000000.25" y="5000000.25" width="1.5" height="1.5" fill="lime" clip-path="url(#c)"/>', '<rect x="12" y="8" width="8" height="16" fill="lime"/>'),
    ('symbol', '0 0 64 64', '<defs><symbol id="s" viewBox="5000000 5000000 4 4" preserveAspectRatio="none"><path d="M5000000.25 5000000.25H5000000.75V5000000.75H5000000.25Z" fill="lime"/></symbol></defs><use href="#s" width="64" height="64"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('use-position', '5000000 5000000 4 4', '<defs><path id="p" d="M0 0H.5V.5H0Z" fill="lime"/></defs><use href="#p" x="5000000.25" y="5000000.25"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('image-position', '5000000 5000000 4 4', '<image x="5000000.25" y="5000000.25" width=".5" height=".5" href="data:image/svg+xml,%3Csvg%20xmlns%3D%22http%3A//www.w3.org/2000/svg%22%20width%3D%221%22%20height%3D%221%22%3E%3Crect%20width%3D%221%22%20height%3D%221%22%20fill%3D%22lime%22/%3E%3C/svg%3E"/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('raster-position', '5000000 5000000 4 4', '<image x="5000000.25" y="5000000.25" width=".5" height=".5" href="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNg+M/wHwAEAQH/rrVV9QAAAABJRU5ErkJggg=="/>', '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ('quarter-turn', '-5000004 5000000 4 4', '<g transform="rotate(90)"><path d="M5000000.25 5000000.25H5000000.75V5000000.75H5000000.25Z" fill="lime"/></g>', '<rect x="52" y="4" width="8" height="8" fill="lime"/>'),
    ('half-turn', '-5000004 -5000004 4 4', '<g transform="rotate(180)"><path d="M5000000.25 5000000.25H5000000.75V5000000.75H5000000.25Z" fill="lime"/></g>', '<rect x="52" y="52" width="8" height="8" fill="lime"/>'),
]
cases.append(('matrix3d', '5000000 5000000 8 8',
              '<g style="transform:matrix3d(1,0,0,0,0,1,0,0,0,0,1,0,.125,.125,0,1)">' * 4
              + '<path d="M5000000.5 5000000.5H5000001V5000001H5000000.5Z" fill="lime"/>'
              + '</g>' * 4,
              '<rect x="8" y="8" width="4" height="4" fill="lime"/>'))
cases.append(('intermediate-range', '0 0 64 64',
              '<path d="M3 0C3e38 0 3e38 4 3 4Z" fill="lime" transform="scale(10)"/>',
              '<rect x="30" width="34" height="40" fill="lime"/>'))
cases.append(('pattern-period', '0 0 320 320',
              '<defs><pattern id="period" width="400" height="400" patternUnits="userSpaceOnUse" patternTransform="scale(.8)">'
              '<rect width="200" height="400" fill="red"/><rect x="200" width="200" height="400" fill="blue"/>'
              '</pattern></defs><rect width="320" height="320" fill="url(#period)"/>',
              '<rect width="32" height="64" fill="red"/><rect x="32" width="32" height="64" fill="blue"/>'))
# Exact translations and full turns preserve genuine subpixel segments.
for name, data, plain_data, join in [
        ('micro-miter', 'M10 10L10.00000002 10.00000005L40 10', 'M0 0L.00000002 .00000005L30 0', 'miter'),
        ('micro-reversal', 'M10 10L9.99999998 10.00000001L40 10', 'M0 0L-.00000002 .00000001L30 0', 'round'),
        ('micro-close', 'M10 10L40 10L40 40L10 40L9.99999998 9.99999998Z', 'M0 0L30 0L30 30L0 30L-.00000002 -.00000002Z', 'miter')]:
    style = f'fill="none" stroke="lime" stroke-width="8" stroke-linejoin="{join}"'
    cases.append((name, '0 0 64 64', f'<path d="{data}" {style}/>',
                  f'<path transform="translate(10 10)" d="{plain_data}" {style}/>'))
cases.append(('rotation-turns', '0 0 64 64',
              '<rect x="10000" y="10000" width="8" height="8" fill="lime" transform="translate(-9980 -9980) rotate(32400.01 -10000 -10000)"/>',
              '<rect x="10000" y="10000" width="8" height="8" fill="lime" transform="translate(-9980 -9980) rotate(.01 -10000 -10000)"/>'))
for name, slope in [('mask-nonzero-box', '1e-12'), ('mask-zero-box', '0')]:
    cases.append((name, '0 0 64 64',
                  f'<defs><mask id="{name}"><rect width="64" height="64" fill="white"/></mask></defs>'
                  f'<g mask="url(#{name})"><path d="M0 0H32" fill="none" stroke="lime" stroke-width="8" transform="matrix({slope} 1 -1 0 16 8)"/></g>',
                  '<g/>' if slope != '0' else '<rect x="12" y="8" width="8" height="32" fill="lime"/>'))
# Real-world SVGs can express a whole flag in tens of millions of units.
# Geometry properties must retain their values beyond a CSS layout-unit
# coordinate range. These power-of-two viewBox maps give literal ellipses.
for name, viewbox, center, expected_x in [
        ('ellipse-large-positive', '0 0 67108864 67108864', '50331648', 48),
        ('ellipse-large-negative', '-67108864 0 67108864 67108864', '-50331648', 16)]:
    cases.append((name, viewbox,
                  f'<ellipse cx="{center}" cy="16777216" rx="4194304" ry="2097152" fill="lime"/>',
                  f'<ellipse cx="{expected_x}" cy="16" rx="4" ry="2" fill="lime"/>'))
for i, (name, viewbox, content, plain) in enumerate(cases):
    x, y = i % 5 * 64, i // 5 * 64
    unit.cell(name, (x, y, 64, 64),
              f'<svg x="{x}" y="{y}" width="64" height="64" '
              f'viewBox="{viewbox}" preserveAspectRatio="none">{content}</svg>',
              f'<g transform="translate({x} {y})">{plain}</g>')
unit.write()
