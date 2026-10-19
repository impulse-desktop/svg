"""fe-dropshadow-equivalent: feDropShadow of CSS Filter Effects against the
primitives it stands for. The unit is written by hand; this writes its
expected drawing."""
from unitgen import expect, pair

U = "fe-dropshadow-equivalent"
expect(U, [(name, (x - 4, 0, 15, 26), pair(U, x, 4, 28)) for name, x in [("primitives", 4), ("defaults", 19), ("negative-deviation", 34), ("zero-deviation", 49)]])
