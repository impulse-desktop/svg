"""fe-duplicate-edges: the duplicate edge mode of feConvolveMatrix and
feGaussianBlur where the edge of their input cuts pixels."""
from unitgen import Unit

unit = Unit("fe-duplicate-edges", 78, 14, """Each yellow rect fills its black
outline. Duplicated past its edge, the input holds its colour there across
the edge: where the edge cuts a pixel, that pixel's colour is what the part
inside holds (D48), so a kernel of sum 1 or a blur over a flat colour
leaves it flat up to its antialiased edge. Chromium, librsvg and resvg draw
the convolution so, larger; a blur's edge mode they leave out.""")
PRIMITIVES = ['<feConvolveMatrix kernelMatrix="1 1 1 1 -7 1 1 1 1" edgeMode="duplicate"/>', '<feGaussianBlur stdDeviation="1.5" edgeMode="duplicate"/>']
BOXES = [(2.5, 2.5, 14, 9), (21.25, 2.75, 15.5, 8.5)]
for p, primitive in enumerate(PRIMITIVES):
    for k, (x, y, w, h) in enumerate(BOXES):
        n = 2 * p + k
        x += 38 * p
        rect = f'<rect x="{x}" y="{y}" width="{w}" height="{h}"'
        name = ("convolve" if p == 0 else "blur") + f"-{k + 1}"
        test = f'<filter id="d{n}" x="0" y="0" width="1" height="1">{primitive}</filter>' + rect + f' fill="yellow" filter="url(#d{n})"/>'
        unit.cell(name, (int(x) - 1, 1, 19, 12), test, rect + ' fill="yellow"/>', rect + "/>")
unit.write()
