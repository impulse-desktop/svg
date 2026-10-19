"""filter-primitive-values: colour handling of filter primitives. The unit
is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

expect("filter-primitive-values", [
    ("unpremultiplied", (1, 1, 10, 14), pair("filter-primitive-values", 2, 2, 8)),
    ("clamped", (11, 1, 10, 14), pair("filter-primitive-values", 12, 2, 8)),
    ("space", (21, 1, 22, 14), pair("filter-primitive-values", 22, 2, 8) + pair("filter-primitive-values", 32, 2, 8)),
])
