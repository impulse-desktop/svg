"""The reference period is its declared box, including its clipped part.

Top: x=-10..4 has period14, although the filter keeps only x=0..4.
Those four red units repeat at0..4 and14..18; the output ends at16.
Bottom: y=-10..10 has period20, although only y=8..10 survives the
filter. No further copy reaches its output y=8..16.
"""
from unitgen import expect

expect('filter-tile-clipped', [
    ('x-clipped-reference', (0, 0, 16, 8),
     '<path fill="red" d="M0 0h4v8H0zM14 0h2v8h-2z"/>'),
    ('y-clipped-reference', (0, 8, 16, 8),
     '<rect y="8" width="16" height="2" fill="blue"/>'),
])
