"""fe-convolve-formula: feConvolveMatrix of CSS Filter Effects. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "fe-convolve-formula"
NAMES = ["kernel", "target-divisor-bias", "preserve-alpha", "edge-none", "order-wrap", "edge-mirror", "bias-alpha", "wrong-count"]
expect(U, [(name, (1 + 10 * k, 1, 10, 14), pair(U, 2 + 10 * k, 2, 8)) for k, name in enumerate(NAMES)])
