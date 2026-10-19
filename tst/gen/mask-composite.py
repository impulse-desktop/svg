"""mask-composite: mask layer compositing of CSS Masking. The unit is
written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


expect("mask-composite", [
    ("add", (1, 1, 14, 14), '<path fill="lime" d="M2 2h12v6h-6v6h-6z"/>'),
    ("subtract", (17, 1, 14, 14), lime(18, 8, 6, 6)),
    ("intersect", (33, 1, 14, 14), lime(34, 2, 6, 6)),
    ("exclude", (49, 1, 14, 14), '<path fill="lime" d="M56 2h6v6h-6zM50 8h6v6h-6z"/>'),
    ("bottom-ignored", (1, 17, 14, 14), lime(2, 18, 6, 12)),
    ("repeat-list", (17, 17, 14, 14), lime(18, 18, 6, 6)),
    ("isolated", (33, 17, 14, 14), lime(34, 18, 12, 12)),
    ("resets-border", (49, 17, 14, 14), lime(50, 18, 12, 12)),
])
