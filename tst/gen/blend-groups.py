"""blend-groups: groups and isolation of Compositing 1 in SVG, each test
over the reference the expected drawing draws in its place."""
from unitgen import Unit

B, S = (51, 153, 230), (204, 77, 26)
unit = Unit("blend-groups", 86, 30, """Each test runs on into the reference
under it without a step. The source multiplies over the blue backdrop:
inside a plain group, and one with a 2D transform, it still meets the
backdrop; isolation isolate, opacity, a filter, a mask, a blend mode, and
a 3D transform though it moves nothing, each isolate the group, so the
source meets a transparent backdrop and the group then composites, at
half opacity, or screened; isolation auto, set or inherited, isolates
nothing. Grouping translucent elements changes nothing, the reference the
same three ungrouped.""", ground=False)
unit.common('<defs><filter id="id" x="0" y="0" width="1" height="1"><feOffset/></filter><mask id="w"><rect width="86" height="30" fill="white"/></mask></defs>')


def hexed(c):
    return "#%02x%02x%02x" % tuple(round(v) for v in c)


def mul(b, s):
    return [x * y / 255 for x, y in zip(b, s)]


def screen(b, s):
    return [x + y - x * y / 255 for x, y in zip(b, s)]


def over(s, a, b):
    return [a * x + (1 - a) * y for x, y in zip(s, b)]


source = f'fill="{hexed(S)}" style="mix-blend-mode: multiply"'
cells = {
    "non-isolated": [("<g>{r}</g>", mul(B, S)), ('<g style="isolation: auto">{r}</g>', mul(B, S)), ('<g style="isolation: inherit">{r}</g>', mul(B, S))],
    "transform-2d": [('<g transform="translate(1 0)">{r1}</g>', mul(B, S))],
    "isolate": [('<g style="isolation: isolate">{r}</g>', S)],
    "opacity": [('<g opacity=".5">{r}</g>', over(S, .5, B))],
    "filter": [('<g filter="url(#id)">{r}</g>', S)],
    "mask": [('<g mask="url(#w)">{r}</g>', S)],
    "blend": [('<g style="mix-blend-mode: screen">{r}</g>', screen(B, S))],
    "transform-3d": [('<g style="transform: translateZ(0)">{r}</g>', S), ('<g style="transform: rotateY(0deg)">{r}</g>', S)],
}
k = 0
for name, items in cells.items():
    tests, plains = [], []
    first = k

    for template, reference in items:
        x, y = 2 + 8 * k, 2
        k += 1
        tests.append(f'<rect x="{x}" y="{y}" width="6" height="6" fill="{hexed(B)}"/>' + template.format(r=f'<rect x="{x}" y="{y}" width="6" height="6" {source}/>', r1=f'<rect x="{x - 1}" y="{y}" width="6" height="6" {source}/>')
                     + f'<rect x="{x}" y="{y + 6}" width="6" height="6" fill="{hexed(reference)}"/>')
        plains.append(f'<rect x="{x}" y="{y}" width="6" height="12" fill="{hexed(reference)}"/>')

    unit.cell(name, (1 + 8 * first, 1, 8 * (k - first), 14), "".join(tests), "".join(plains))


def three(x, y):
    return (f'<rect x="{x}" y="{y}" width="12" height="6" fill="#c31" fill-opacity=".6"/>', f'<rect x="{x + 4}" y="{y}" width="12" height="6" fill="#3a8" fill-opacity=".5"/>',
            f'<rect x="{x + 8}" y="{y}" width="12" height="6" fill="#25e" fill-opacity=".7"/>')


a, b, c = three(2, 16)
d, e, f = three(30, 16)
unit.cell("invariance", (1, 15, 50, 14), f"{a}<g>{b}<g>{c}</g></g>" + "".join(three(2, 22)) + f"<g>{d}{e}</g>{f}" + "".join(three(30, 22)),
          "".join(three(2, 16)) + "".join(three(2, 22)) + "".join(three(30, 16)) + "".join(three(30, 22)))
unit.write()
