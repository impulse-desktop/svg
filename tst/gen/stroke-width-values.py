"""stroke-width-values: stroke-width of SVG 2. The unit is written by hand;
this writes its expected drawing: each stroke the bar it runs into."""
from unitgen import expect


def bar(y, h, fill="black"):
    return f'<rect x="2" y="{y}" width="60" height="{h}" fill="{fill}"/>'


expect("stroke-width-values", [
    ("percent", (1, 1, 62, 10), bar(2.8, 6.4)), ("negative", (1, 12, 62, 8), bar(14, 4)), ("user-units", (1, 22, 62, 8), bar(24, 4)),
    ("units", (1, 32, 62, 8), bar(33.1655, 5.669)), ("em", (1, 42, 62, 8), bar(44, 4)), ("zero", (1, 51, 62, 12), bar(52, 10, "lime")),
])
