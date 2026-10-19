"""filter-background-image: BackgroundImage and BackgroundAlpha of CSS
Filter Effects. The unit is written by hand; this writes its expected
drawing."""
from unitgen import expect


def lime(x, w, fill="lime"):
    return f'<rect x="{x}" y="2" width="{w}" height="12" fill="{fill}"/>'


expect("filter-background-image", [("root", (1, 1, 14, 14), lime(2, 9) + lime(11, 3, "navy")), ("group", (17, 1, 14, 14), lime(18, 9) + lime(27, 3, "navy")),
                                   ("enable-background", (33, 1, 14, 14), lime(34, 9) + lime(43, 3, "navy")), ("isolated", (49, 1, 14, 14), lime(50, 6) + lime(56, 6, "navy")),
                                   ("background-alpha", (65, 1, 14, 14), lime(66, 6))])
