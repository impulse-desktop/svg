"""fe-composite-arithmetic: feComposite arithmetic of CSS Filter Effects.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "fe-composite-arithmetic"
expect(U, [(name, (x - 1, 1, 9, 14), pair(U, x, 2, 8)) for name, x in [("average", 2), ("product", 11), ("sum-clamped", 20), ("all-terms", 29), ("k4", 38), ("below-zero", 47), ("below-zero-mixed", 56)]]
       + [("k4-subregion", (64, 1, 9, 14), '<rect x="65" y="2" width="7" height="12"/>' + pair(U, 65, 2, 8))])
