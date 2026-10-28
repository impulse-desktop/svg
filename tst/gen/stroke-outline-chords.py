"""A stroke's outline keeps to the chords of its hair-fine points (D60).

The outline of a stroke of a flattened curve has two offset points at
every smooth point, a small step apart, and the one between neighbours
within 2^-9 px of their chord is left out. A circle stroked wide is the
annulus of its offsets, which the expected drawing fills; a polyline
that wiggles by a thousandth of a pixel either side of a line is the
line's band.
"""
from unitgen import Unit

unit = Unit('stroke-outline-chords', 64, 64,
            'A stroke outline leaves out the points within 2^-9 px of the chord between their neighbours (D60): a wide stroke of a circle is its annulus; a hair-wiggled polyline is its band; a cubic hooked back a hair at its start keeps its tip.')
unit.cell('circle', (0, 0, 32, 32),
          '<circle cx="16" cy="16" r="10" fill="none" stroke="#3066b2" stroke-width="8"/>',
          '<path fill-rule="evenodd" fill="#3066b2" d="M 2 16 A 14 14 0 1 0 30 16 A 14 14 0 1 0 2 16 Z M 10 16 A 6 6 0 1 0 22 16 A 6 6 0 1 0 10 16 Z"/>',
          tolerance=4)
points = ' '.join('%g,%.4f' % (34 + 2 * i, 16 + (0.001 if i % 2 else -0.001)) for i in range(14))
unit.cell('wiggle', (32, 0, 32, 32),
          f'<polyline points="{points}" fill="none" stroke="#3066b2" stroke-width="8" stroke-linecap="square"/>',
          '<rect x="30" y="12" width="34" height="8" fill="#3066b2"/>')
# A cubic that hooks back a hair at its start sweeps a tip whose outline
# points lie thousandths of a pixel apart, some behind the chord's start;
# nothing draws it more plainly than the stroke itself, and the unit's
# worth here is the canon's drawing it is compared with.
unit.cell('hook', (0, 32, 32, 32),
          '<path d="M 4 52 C 3.85 51.94 20 46 24 60" fill="none" stroke="#3066b2" stroke-width="1"/>',
          '<path d="M 4 52 C 3.85 51.94 20 46 24 60" fill="none" stroke="#3066b2" stroke-width="1"/>')
unit.write()
