"""render-not-rendered: elements never rendered and display none, of SVG
2. The unit is written by hand; this writes its expected drawing."""
from unitgen import expect

expect("render-not-rendered", [("not-rendered", (0, 0, 64, 16), '<rect width="64" height="16" fill="lime"/>')])
