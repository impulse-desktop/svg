"""render-root-transform: the outermost svg's transform about its centre,
of SVG 2's user agent style sheet. The unit is written by hand; this
writes its expected drawing, the turned bars where they land."""
from unitgen import expect

expect("render-root-transform", [("root-origin", (0, 0, 32, 32), '<rect x="24" y="0" width="8" height="16" fill="lime"/>'), ("other-origin", (0, 20, 12, 6), '<rect x="2" y="22" width="6" height="2" fill="teal"/>')],
       root='<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">')
