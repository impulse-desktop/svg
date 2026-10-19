"""resource-display: resources used whatever their display or conditions,
of SVG 2. The unit is written by hand; this writes its expected drawing."""
from unitgen import expect


def lime(*xs):
    return "".join(f'<rect x="{x}" y="2" width="10" height="10" fill="lime"/>' for x in xs)


expect("resource-display", [("clip", (1, 1, 12, 12), lime(2)), ("mask", (13, 1, 12, 12), lime(14)), ("filter", (25, 1, 12, 12), lime(26)), ("conditional", (37, 1, 24, 12), lime(38, 50))])
