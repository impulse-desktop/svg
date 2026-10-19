"""stroke-basics: the stroke and where dashes start on basic shapes, of SVG
2. The unit is written by hand; this writes its expected drawing: each
dash as the stroked piece of its shape it is."""
import math

from unitgen import expect


def ellipse_end(rx, ry, length):
    """The point an arc of the given length along an ellipse reaches from
    the end of its x axis, going toward +y."""
    t, step, run = 0.0, 1e-5, 0.0

    while run < length:
        run += step * math.hypot(rx * math.sin(t), ry * math.cos(t))
        t += step

    return rx * math.cos(t), ry * math.sin(t)


ex, ey = ellipse_end(6, 4, 3)
STROKE = 'fill="none" stroke="lime" stroke-width="2"'
expect("stroke-basics", [
    ("none-default", (1, 1, 12, 12), '<rect x="2" y="2" width="10" height="10" fill="lime"/>'),
    ("rect-start", (14, 1, 16, 14), f'<path d="M16 4H22" {STROKE}/>'),
    ("circle-start", (33, 1, 14, 14), f'<path d="M45 8A5 5 0 0 1 40 13" {STROKE}/>'),
    ("ellipse-start", (46, 12, 16, 12), f'<path d="M60 18A6 4 0 0 1 {54 + ex:.4f} {18 + ey:.4f}" {STROKE}/>'),
    ("line-start", (46, 1, 16, 6), f'<path d="M60 4H56" {STROKE}/>'),
    ("server-box", (0, 18, 20, 21), '<rect x="1" y="19" width="6" height="14" fill="lime"/><path d="M7 19h12v14h-12z M7 25h6v2h-6z" fill-rule="evenodd" fill="blue"/>'
     '<rect x="1" y="34" width="6" height="4" fill="lime"/><rect x="7" y="34" width="12" height="4" fill="blue"/>'),
])
