"""struct-fragment: the SVG document fragment, defs and foreign elements of
SVG 2. The unit is written by hand; this writes its expected drawing."""
from unitgen import expect

expect("struct-fragment", [("fragment", (0, 0, 32, 16), '<rect width="32" height="16" fill="lime"/>')], root='<svg xmlns="http://www.w3.org/2000/svg" width="32" height="16">')
