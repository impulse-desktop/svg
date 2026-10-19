"""fe-flood: feFlood of CSS Filter Effects. The unit is written by hand;
this writes its expected drawing."""
from unitgen import expect, pair

expect("fe-flood", [
    ("opacity-times-alpha", (1, 1, 9, 14), pair("fe-flood", 2, 2, 8)),
    ("initial", (10, 1, 9, 14), pair("fe-flood", 11, 2, 8)),
    ("opacity-clamp", (19, 1, 18, 14), pair("fe-flood", 20, 2, 8) + ""),
    ("space", (37, 1, 9, 14), pair("fe-flood", 38, 2, 8)),
    ("not-inherited", (46, 1, 9, 14), pair("fe-flood", 47, 2, 8)),
    ("subregion", (57, 1, 6, 14), '<rect x="58" y="2" width="4" height="12" fill="lime"/>'),
])
