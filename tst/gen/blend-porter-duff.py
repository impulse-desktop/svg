"""blend-porter-duff: the Porter-Duff operators of Compositing 1, by
feComposite, each over the reference its fractions give."""
from unitgen import Unit

B, S, AB, AS = (51, 153, 230), (204, 77, 26), .7, .6
unit = Unit("blend-porter-duff", 62, 16, """Each test runs on into the
reference under it without a step. The Porter-Duff operators of
feComposite, the source at 0.6 over a backdrop at 0.7: over, in, out,
atop, xor and lighter, each the sum of the source and the backdrop by
their fractions Fa and Fb, lighter clamped.""", ground=False)
FRACTIONS = {"over": (1, 1 - AS), "in": (AB, 0), "out": (1 - AB, 0), "atop": (AB, 1 - AS), "xor": (1 - AB, 1 - AS), "lighter": (1, 1)}
for k, (name, (fa, fb)) in enumerate(FRACTIONS.items()):
    x = 2 + 10 * k
    alpha = min(1, AS * fa + AB * fb)
    colour = [min(255, AS * fa * s + AB * fb * b) for s, b in zip(S, B)]
    colour = "#%02x%02x%02x" % tuple(round(min(255, c / alpha if alpha else 0)) for c in colour)
    reference = f'<rect x="{x}" y="{{}}" width="8" height="6" fill="{colour}" fill-opacity="{round(alpha, 4)}"/>'
    unit.cell(name, (x - 1, 1, 10, 14), f'<filter id="{name}" x="0" y="0" width="1" height="1" color-interpolation-filters="sRGB"><feFlood flood-color="#3399e6" flood-opacity="{AB}" result="b"/>'
              f'<feComposite in="SourceGraphic" in2="b" operator="{name}"/></filter><rect x="{x}" y="2" width="8" height="6" fill="#cc4d1a" fill-opacity="{AS}" filter="url(#{name})"/>' + reference.format(8),
              reference.format(2) + reference.format(8))
unit.write()
