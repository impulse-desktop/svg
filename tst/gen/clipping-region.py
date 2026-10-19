"""clipping-region: clipping regions of CSS Masking. The unit is written by
hand; this writes its expected drawing."""
from unitgen import expect

expect("clipping-region", [
    ("group", (1, 1, 14, 14), '<rect x="4" y="4" width="8" height="8" fill="lime"/>'),
    ("nested", (17, 1, 14, 14), '<rect x="20" y="4" width="8" height="8" fill="lime"/>'),
    ("geometry", (33, 1, 22, 14), '<rect x="34" y="2" width="10" height="12" fill="lime"/>'),
    ("gradient-geometry", (1, 17, 22, 14), '<rect x="2" y="18" width="10" height="12" fill="lime"/>'),
    ("stacking-context", (25, 17, 14, 14), '<rect x="26" y="18" width="12" height="12" fill="blue"/><rect x="28" y="20" width="8" height="8" fill="yellow"/>'),
    ("not-inherited", (41, 17, 14, 14), '<circle cx="48" cy="24" r="6" fill="lime"/>'),
    ("clip-rule-basic-shape", (1, 33, 14, 14), '<path fill="lime" d="M8 34 11.6 46 2 38.2H14L4.4 46Z"/>'),
])
