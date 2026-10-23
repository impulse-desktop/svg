"""The authored relative steps form this exactly closed trapezoid.

Corners are the sums of the decimal path arguments, without rounding the
starting coordinate again at every relative step. No raster output is used.
"""
from unitgen import expect

expect('path-relative-close', [
    ('closed-trapezoid', (0, 0, 48, 32),
     '<path d="M7.4471 16.726L7.4471 40.342L42.1251 40.342L42.1871 16.726Z" '
     'fill="none" stroke="navy" stroke-width="3"/>'),
])
