"""filter-interpolation-space: color-interpolation-filters of CSS Filter
Effects. The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "filter-interpolation-space"
expect(U, [(name, (x - 1, 1, 9, 14), pair(U, x, 2, 8)) for name, x in [("linear-default", 2), ("inherited", 11), ("auto", 20), ("offset-exact", 29), ("tile-exact", 38), ("image-exact", 47), ("flood-exact", 56)]])
