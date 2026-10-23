"""Constant feFlood tiles keep their colour even below a pixel.

The tall teal tile covers the region. The half-pixel navy tile is opaque
within its own subregion, so repeating it covers the same region. The
half-pixel gold tile is opaque too and is merged last. The convolutions
of empty, out-of-region floods add nothing. The result is opaque gold.
"""
from unitgen import expect

expect('filter-subregions-outside', [
    ('constant-subpixel-tiles', (0, 0, 16, 16),
     '<rect width="16" height="16" fill="gold"/>'),
])
