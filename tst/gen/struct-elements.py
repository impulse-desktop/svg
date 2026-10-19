"""struct-elements: SVG 2's document structure, where struct-fragment and
switch-select leave off: what graphics elements draw, a switch choosing a
container and leaving style alone, elements of no namespace, use references
that name nothing, ids, and conditions inside a pattern."""
from unitgen import Unit, png

unit = Unit("struct-elements", 64, 20, """Each drawing matches its plain twin
in the expected drawing; no red. rect, circle, ellipse, line, polyline,
polygon, path and image draw, a square each, circles and ellipses as their
arcs. A switch choosing a g draws all it holds; a style element in a switch
still styles. A rect of no namespace is no SVG and draws nothing; a use of
a data: URL or of an empty href draws nothing; of two elements with one id
the first is the one named; a pattern child that conditional processing
leaves out adds nothing to the tile.""", root='xmlns:xlink="http://www.w3.org/1999/xlink"')
LIME = png([[(0, 1, 0, 1)] * 2] * 2)
unit.common('<defs><rect id="twice" width="6" height="6" fill="lime"/><rect id="twice" width="6" height="6" fill="red"/>'
            '<pattern id="tile" width="1" height="1"><rect width="6" height="6" fill="lime"/><rect width="6" height="6" fill="red" systemLanguage="xx"/></pattern></defs>', "")


def square(x, y):
    return f'<rect x="{x}" y="{y}" width="6" height="6" fill="lime"/>'


def arcs(cx, cy, rx, ry):
    return f'<path d="M {cx - rx} {cy} A {rx} {ry} 0 0 1 {cx + rx} {cy} A {rx} {ry} 0 0 1 {cx - rx} {cy} Z" fill="lime"/>'


graphics = [
    ("rect", '<rect x="{x}" y="{y}" width="6" height="6" fill="lime"/>', square),
    ("circle", '<circle cx="{cx}" cy="{cy}" r="3" fill="lime"/>', lambda x, y: arcs(x + 3, y + 3, 3, 3)),
    ("ellipse", '<ellipse cx="{cx}" cy="{cy}" rx="3" ry="2" fill="lime"/>', lambda x, y: arcs(x + 3, y + 3, 3, 2)),
    ("line", '<line x1="{x}" y1="{cy}" x2="{x6}" y2="{cy}" stroke="lime" stroke-width="6"/>', square),
    ("polyline", '<polyline points="{x},{y} {x6},{y} {x6},{y6} {x},{y6}" fill="lime"/>', square),
    ("polygon", '<polygon points="{x},{y} {x6},{y} {x6},{y6} {x},{y6}" fill="lime"/>', square),
    ("path", '<path d="M {x} {y} h 6 v 6 h -6 Z" fill="lime"/>', square),
    ("image", f'<image x="{{x}}" y="{{y}}" width="6" height="6" image-rendering="pixelated" href="{LIME}"/>', square),
]

for k, (name, test, plain) in enumerate(graphics):
    x, y = 2 + k * 8, 2
    values = dict(x=x, y=y, x6=x + 6, y6=y + 6, cx=x + 3, cy=y + 3)
    unit.cell(name, (x - 1, y - 1, 8, 8), test.format(**values), plain(x, y), f'<rect x="{x}" y="{y}" width="6" height="6"/>')

y = 12
others = [
    ("switch-container", '<switch><g systemLanguage="xx"><rect x="{x}" y="{y}" width="6" height="6" fill="red"/></g><g><rect x="{x}" y="{y}" width="3" height="6" fill="lime"/><rect x="{x3}" y="{y}" width="3" height="6" fill="lime"/></g></switch>'),
    ("switch-style", '<switch><rect class="styled" x="{x}" y="{y}" width="6" height="6" fill="red"/><style>.styled {{ fill: lime }}</style></switch>'),
    ("no-namespace", '<rect x="{x}" y="{y}" width="6" height="6" fill="lime"/><rect xmlns="" x="{x}" y="{y}" width="6" height="6" fill="red"/>'),
    ("use-data-url", '<rect x="{x}" y="{y}" width="6" height="6" fill="lime"/><use href="data:image/svg+xml,%3Csvg xmlns=%27http://www.w3.org/2000/svg%27%3E%3Crect id=%27r%27 width=%276%27 height=%276%27 fill=%27red%27/%3E%3C/svg%3E#r" x="{x}" y="{y}"/>'),
    ("use-empty-href", '<rect x="{x}" y="{y}" width="6" height="6" fill="lime"/><use href="" x="{x}" y="{y}"/><use xlink:href="" x="{x}" y="{y}"/>'),
    ("duplicate-id", '<use href="#twice" x="{x}" y="{y}"/>'),
    ("pattern-conditional", '<rect x="{x}" y="{y}" width="6" height="6" fill="url(#tile)"/>'),
]

for k, (name, test) in enumerate(others):
    x = 2 + k * 8
    unit.cell(name, (x - 1, y - 1, 8, 8), test.format(x=x, y=y, x3=x + 3), square(x, y), f'<rect x="{x}" y="{y}" width="6" height="6"/>')

unit.write()
