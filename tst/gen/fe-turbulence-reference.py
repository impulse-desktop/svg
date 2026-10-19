"""fe-turbulence-reference: feTurbulence of CSS Filter Effects against the
module's own code. The unit is written by hand; this writes its expected
drawing."""
from unitgen import expect, pair

U = "fe-turbulence-reference"
NAMES = ["turbulence", "fractal-noise", "two-frequencies", "seed", "stitch", "linear-channels", "zero-frequency", "no-octaves", "fractional-octaves"]
expect(U, [(name, (1 + 12 * k, 1, 12, 18), pair(U, 2 + 12 * k, 2, 10)) for k, name in enumerate(NAMES)])
