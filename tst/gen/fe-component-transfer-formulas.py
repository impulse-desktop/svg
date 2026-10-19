"""fe-component-transfer-formulas: feComponentTransfer of CSS Filter
Effects. The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "fe-component-transfer-formulas"
expect(U, [(name, (x - 1, 1, 9, 14), pair(U, x, 2, 8)) for name, x in [("table", 2), ("discrete", 11), ("linear", 20), ("gamma", 29), ("last-function", 38), ("empty-table", 47), ("table-ends", 56)]])
