"""gradient-templates: gradient href templates of SVG 2. The unit is written
by hand; this writes its expected drawing: each strip its reference."""
from unitgen import expect


def strips(y, fill):
    return f'<rect x="2" y="{y}" width="60" height="6" fill="url(#{fill})"/>'


expect("gradient-templates", [
    ("chain-stops", (1, 1, 62, 8), strips(2, "base")),
    ("chain-attributes", (1, 9, 62, 8), strips(10, "r2")),
    ("own-stops", (1, 17, 62, 8), strips(18, "r3")),
    ("radial-template", (1, 25, 62, 8), strips(26, "r4")),
    ("not-gradient", (1, 33, 62, 8), strips(34, "r5")),
    ("cloned-stops", (1, 41, 62, 14), '<rect x="2" y="42" width="60" height="12" fill="lime"/>'),
])
