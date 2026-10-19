"""fe-displacement-formula: feDisplacementMap of CSS Filter Effects. The unit
is written by hand; this writes its expected drawing."""
from unitgen import expect

# The map's channel, linear or as written, less a half, times the scale 8.
LINEAR = 8 * ((((225 / 255) + .055) / 1.055) ** 2.4 - .5)
SRGB = 8 * (191 / 255 - .5)


def lime(x, y):
    return '<rect x="%.4f" y="%.4f" width="5" height="8" fill="lime"/>' % (x, y)


expect("fe-displacement-formula", [("channels", (1, 1, 12, 18), lime(2, 10)), ("alpha-default", (14, 1, 12, 18), lime(15, 2)), ("zero-scale", (27, 1, 12, 18), lime(32, 6)),
                                   ("unpremultiplied", (40, 1, 12, 18), lime(41, 6)), ("linear-map", (53, 1, 12, 18), lime(58 - LINEAR, 6 - LINEAR)), ("srgb-map", (66, 1, 13, 18), lime(71 - SRGB, 6 - SRGB))])
