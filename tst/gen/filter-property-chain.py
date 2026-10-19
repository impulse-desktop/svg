"""filter-property-chain: the filter property's list of CSS Filter Effects.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

expect("filter-property-chain", [
    ("missing", (1, 1, 10, 14), '<rect x="2" y="2" width="8" height="12" fill="lime"/>'),
    ("no-filter", (13, 1, 8, 14), '<rect x="14" y="2" width="6" height="12" fill="lime"/>'),
    ("order", (21, 1, 12, 14), '<rect x="26" y="2" width="2" height="12" fill="lime"/>'),
    ("source-alpha", (33, 1, 14, 14), '<rect x="38" y="2" width="8" height="12" fill="lime"/>'),
    ("function-srgb", (51, 1, 10, 14), pair("filter-property-chain", 52, 2, 8)),
])
