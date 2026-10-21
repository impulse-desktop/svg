"""stroke-wider-than-shape: a small closed or returning open subpath stroked
wider than its curve (D49)."""
from unitgen import Unit

unit = Unit("stroke-wider-than-shape", 93, 24, """Each lime ring fills the
space between its black circles; the last cell holds nothing. A small
subpath's stroke keeps its inner offset when it turns inside out (D49):
a circle of radius 3 stroked 10 wide leaves a hole of radius 2, both as
a circle element and as closed or returning open path data. A circle of
radius 2 stroked 40 wide leaves a hole of 18, wider than its viewport,
and nothing shows.""")


def ring(cx, cy, inner, outer):
    return (f'<path fill-rule="evenodd" fill="lime" d="M {cx - outer} {cy} a {outer} {outer} 0 1 0 {2 * outer} 0 a {outer} {outer} 0 1 0 {-2 * outer} 0 z '
            f'M {cx - inner} {cy} a {inner} {inner} 0 1 0 {2 * inner} 0 a {inner} {inner} 0 1 0 {-2 * inner} 0 z"/>')


def outline(cx, cy, inner, outer):
    return f'<circle cx="{cx}" cy="{cy}" r="{outer}"/><circle cx="{cx}" cy="{cy}" r="{inner}"/>'


unit.cell("circle", (1, 1, 22, 22), '<circle cx="12" cy="12" r="3" fill="none" stroke="lime" stroke-width="10"/>', ring(12, 12, 2, 8), outline(12, 12, 2, 8))
unit.cell("path", (24, 1, 22, 22), '<path d="M 38 12 A 3 3 0 0 1 32 12 A 3 3 0 0 1 38 12 Z" fill="none" stroke="lime" stroke-width="10"/>', ring(35, 12, 2, 8), outline(35, 12, 2, 8))
unit.cell("open-path", (47, 1, 22, 22), '<path d="M 61 12 A 3 3 0 0 1 55 12 A 3 3 0 0 1 61 12" fill="none" stroke="lime" stroke-width="10"/>', ring(58, 12, 2, 8), outline(58, 12, 2, 8), tolerance=4)
unit.cell("vanished", (70, 1, 22, 22), '<svg x="71" y="2" width="20" height="20"><circle cx="10" cy="10" r="2" fill="none" stroke="red" stroke-width="40"/></svg>', "", '<rect x="71" y="2" width="20" height="20"/>')
unit.write()
