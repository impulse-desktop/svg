"""Exact subdivision preserves the existing independently recorded D49 tip.

Use the unchanged Chromium reference from stroke-hairpin-tip, whose
opening is also present in resvg and librsvg. In particular an unequal
split must not make a finite-radius return look like an unresolved cusp.
"""
import ast
from pathlib import Path
from unitgen import Unit

source = ast.parse(Path(__file__).with_name('stroke-hairpin-tip.py').read_text())
REFERENCE = next(ast.literal_eval(n.value) for n in source.body
                 if isinstance(n, ast.Assign)
                 and any(isinstance(t, ast.Name) and t.id == 'REFERENCE' for t in n.targets))


def split(c, t):
    def mix(a, b):
        return tuple((1-t)*x + t*y for x, y in zip(a, b))
    a, b, d, e = c
    ab, bd, de = mix(a, b), mix(b, d), mix(d, e)
    abd, bde = mix(ab, bd), mix(bd, de)
    p = mix(abd, bde)
    return [a, ab, abd, p], [p, bde, de, e]


def point(p):
    return ' '.join(format(v, '.15g') for v in p)


u = Unit('stroke-hairpin-subdivision', 256, 128,
         'De Casteljau subdivision changes no curve geometry or endpoint direction. Both highly unequal and half splits retain the same D49 opening in either path direction. The independent fixed Chromium image is the existing hairpin-tip reference, also checked with resvg and librsvg.')
curve = [(50, 20), (50.2, 19.2), (49.9, 18.8), (49.8, 19.4)]
for i, (reverse, t) in enumerate([(False, 1/1024), (False, .5), (True, 1/1024), (True, .5)]):
    c = list(reversed(curve)) if reverse else curve
    middle = ''.join('C' + ' '.join(point(p) for p in s[1:]) for s in split(c, t))
    path = ('M49.5 20.2L49.8 19.4' + middle + 'H10') if reverse else ('M10 20H50' + middle + 'L49.5 20.2')
    x, y = i % 2 * 128, i // 2 * 64
    u.cell(('start' if reverse else 'end') + ('-unequal' if t < .5 else '-half'),
           (x, y, 128, 64),
           f'<path transform="translate({x} {y}) scale(2)" d="{path}" fill="none" stroke="black" stroke-width="10"/>',
           f'<image x="{x}" y="{y}" width="128" height="64" href="data:image/png;base64,{REFERENCE}"/>', tolerance=64)
u.write()
