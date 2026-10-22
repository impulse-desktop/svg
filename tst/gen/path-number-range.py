"""SVG path string arguments use finite floats; CSS numbers keep saturation.

Each error leaves its completed four-by-four square and never reaches a
later square. Plain rectangles independently describe that valid prefix.
Large supported arguments are scaled to visible fractional rectangles;
underflow and intermediate relative sums must not become syntax errors.
"""
from unitgen import Unit


prefix = 'M0 0h4v4H0Z M8 0 '
later = ' M16 0h4v4h-4Z'
bad = [
    ('horizontal', 'H1e180'),
    ('vertical', 'V3.5e38'),
    ('negative-line', 'L-1e180 4'),
    ('quadratic-control', 'Q10 1e180 12 4'),
    ('smooth-end', 'T12 1e180'),
    ('cubic-control', 'C9 0 10 1e180 12 4'),
    ('arc-radius', 'A1e180 2 0 0 1 12 4'),
    ('arc-rotation', 'A2 2 1e180 0 1 12 4'),
    ('double-overflow', 'L1e10000 4'),
    ('negative-double-overflow', 'L-1e10000 4'),
]
square = '<rect width="4" height="4" fill="lime"/>'
cases = [(name, f'<path d="{prefix + command + later}" fill="lime"/>', square)
         for name, command in bad]
cases += [
    ('implicit-line', '<path d="M0 2L4 2 1e180 4 M16 0h4v4h-4Z" '
     'fill="none" stroke="lime" stroke-width="4"/>', square),
    ('first-move', '<path d="M1e180 0 M0 0h4v4H0Z" fill="lime"/>', ''),
    ('finite-positive', '<path d="M0 0H3.4e38V2e38H0Z" fill="lime" transform="scale(1e-38)"/>',
     '<rect width="3.4" height="2" fill="lime"/>'),
    ('finite-negative', '<path d="M0 0H-3.4e38V-2e38H0Z" fill="lime" transform="scale(-1e-38)"/>',
     '<rect width="3.4" height="2" fill="lime"/>'),
    ('maximum-finite', '<path d="M0 0H3.4028234663852886e38V2e38H0Z" fill="lime" transform="scale(1e-38)"/>',
     '<rect width="3.4028234663852886" height="2" fill="lime"/>'),
    ('underflow', '<path d="M1e-303 0h4v4H0Z" fill="lime"/>', square),
    ('relative-return', '<path d="M0 0h3e38 3e38 -3e38 -3e38 4v4H0Z" fill="lime"/>', square),
    ('css-path-fallback', '<path d="M0 0h4v4H0Z" fill="lime" '
     'style="d:path(\'M8 0h4v4h-4Z M16 0L1e180 4\')"/>', square),
    ('css-clip-invalid', '<rect width="12" height="4" fill="lime" '
     'style="clip-path:path(\'M0 0h4v4H0Z M16 0L1e180 4\')"/>',
     '<rect width="12" height="4" fill="lime"/>'),
    ('css-number-clamped', '<svg width="4" height="4"><rect height="4" fill="lime" '
     'style="width:1e180px"/></svg>', square),
    ('zero-large-exponent', '<path d="M0e10000 0h4v4H0Z" fill="lime"/>', square),
    ('compensated-exponent', '<path d="M0 0H.01e40V2e38H0Z" fill="lime" transform="scale(1e-38)"/>',
     '<rect width="1" height="2" fill="lime"/>'),
]
u = Unit('path-number-range', 128, 96,
         'An unsupported path argument stops the path at its last complete '
         'segment. Finite float boundaries remain supported, tiny values '
         'round to zero, and relative arithmetic may return from far away. '
         'CSS path functions remain invalid as a whole; CSS numeric values '
         'retain their own saturation rule.', ground=False)
for i, (name, test, expected) in enumerate(cases):
    x, y = i % 4 * 32, i // 4 * 16
    u.cell(name, (x, y, 32, 16),
           f'<svg x="{x + 1}" y="{y + 2}" width="24" height="10">{test}</svg>',
           f'<g transform="translate({x + 1} {y + 2})">{expected}</g>')
u.write()
