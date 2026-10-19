"""blend-mode-formulas: every mix-blend-mode of Compositing 1 over four
colour pairs, each test over the reference its formulas give, which the
expected drawing draws in its place."""
from blendref import blend, modes
from unitgen import Unit

PAIRS = [((51, 153, 230), (204, 77, 26), 1, 1), ((230, 26, 128), (102, 179, 242), 1, 1), ((51, 153, 230), (204, 77, 26), 1, .6), ((230, 26, 128), (102, 179, 242), .5, .8)]
unit = Unit("blend-mode-formulas", 50, 2 + 7 * len(modes), """Each test runs on into the
reference under it without a step. Every blend mode over four colour
pairs, opaque, the source at 0.6, and both translucent, the references
computed from the formulas: the separable modes channel by channel, hue,
saturation, color and luminosity through Lum, SetLum with ClipColor, Sat
and SetSat; the result Cs' = (1 - ab) Cs + ab B(Cb, Cs), composited source
over, on a transparent ground.""", ground=False)


def hexed(c):
    return "#%02x%02x%02x" % tuple(c)


def opacity(a):
    return "" if a == 1 else f' fill-opacity="{a:g}"'


for r, mode in enumerate(modes):
    tests, plains = [], []
    y = 2 + 7 * r

    for k, (cb, cs, ab, sa) in enumerate(PAIRS):
        x = 2 + 12 * k
        b, s = [v / 255 for v in cb], [v / 255 for v in cs]
        m = [min(1, max(0, v)) for v in blend(mode, b, s)]
        cr = [(1 - ab) * sv + ab * mv for sv, mv in zip(s, m)]
        ao = sa + ab * (1 - sa)
        co = [round(255 * (sa * c + ab * bv * (1 - sa)) / ao) for c, bv in zip(cr, b)]
        reference = f'<rect x="{x}" y="{{}}" width="10" height="3" fill="{hexed(co)}"{opacity(round(ao, 4))}/>'
        tests.append(f'<rect x="{x}" y="{y}" width="10" height="3" fill="{hexed(cb)}"{opacity(ab)}/><rect x="{x}" y="{y}" width="10" height="3" fill="{hexed(cs)}"{opacity(sa)} style="mix-blend-mode: {mode}"/>'
                     + reference.format(y + 3))
        plains.append(reference.format(y) + reference.format(y + 3))

    unit.cell(mode, (1, y - 1, 48, 7), "".join(tests), "".join(plains))

unit.write()
