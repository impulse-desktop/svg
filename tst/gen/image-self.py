"""image-self: an image of the document itself, of SVG 2. The unit is
written by hand; this writes its expected drawing. The copy is drawn as an
image, resampled, so its edges soften: its cell allows that."""
from unitgen import expect

expect("image-self", [("document", (0, 0, 32, 32), '<rect width="32" height="32" fill="lime"/>'), ("self-copy", (32, 0, 32, 16), '<rect x="32" width="16" height="16" fill="lime"/>', 32)])
