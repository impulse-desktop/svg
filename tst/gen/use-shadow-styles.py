"""use-shadow-styles: style in the use element's shadow tree, of SVG 2. The
unit is written by hand; this writes its expected drawing."""
from unitgen import expect

NAMES = ["not-ancestors", "not-siblings", "clone-rules", "inherit-from-use", "visibility"]
expect("use-shadow-styles", [(name, (1 + 12 * k, 1, 12, 12), f'<rect x="{2 + 12 * k}" y="2" width="10" height="10" fill="lime"/>') for k, name in enumerate(NAMES)])
