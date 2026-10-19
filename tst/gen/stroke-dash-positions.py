"""stroke-dash-positions: dash positions of SVG 2. The unit is written by
hand; this writes its expected drawing: each dashed stroke as the
undashed subpaths under it."""
from unitgen import expect

BLACK = '<g fill="none" stroke="black" stroke-width="2"%s>%s</g>'


def subpaths(d, round=False):
    return BLACK % (' stroke-linecap="round"' if round else "", f'<path d="{d}"/>')


RESTART = "M 2 {0} H 7 M 10 {0} H 15 M 18 {0} H 23 M 26 {0} H 30 M 34 {0} H 39 M 42 {0} H 47 M 50 {0} H 55 M 58 {0} H 62"
expect("stroke-dash-positions", [
    ("restart", (1, 2, 62, 6), subpaths(RESTART.format(4)) + subpaths(RESTART.format(6))),
    ("offset-dot", (1, 10, 20, 8), subpaths("M 3 12 h 0 M 7 12 H 11 M 15 12 H 19", True) + subpaths("M 3 16 h 0 M 7 16 H 11 M 15 16 H 19", True)),
    ("pattern-start", (29, 10, 20, 8), subpaths("M 31 12 H 35 M 39 12 H 43", True) + subpaths("M 31 16 H 35 M 39 16 H 43", True)),
    ("zero-dashes", (1, 20, 16, 8), subpaths("M 3 22 h 0 M 6 22 h 0 M 9 22 h 0 M 12 22 h 0", True) + subpaths("M 3 26 h 0 M 6 26 h 0 M 9 26 h 0 M 12 26 h 0", True)),
    ("corner", (28, 20, 20, 4), '<rect x="29" y="21" width="18" height="2" fill="lime"/>'),
    ("negative-offset", (44, 32, 4, 12), '<rect x="45" y="33" width="2" height="10" fill="lime"/>'),
])
