"""Fractional intersections are geometric cuts, not products of coverage.

The expected PNG cells contain exact rational areas of literal rectangles.
They do not use clipPath, the renderer, or browser pixels.
"""
from fractions import Fraction as F
import re

from unitgen import Unit, png


unit = Unit('clip-rectangle-intersection', 96, 120,
            'Intersect rectangular clipPath resources before antialiasing. '
            'Repeated fractional edges keep their coverage once, including '
            'use references, child clips, paths, transforms and group opacity. '
            'Empty cuts and separated children retain their geometry.',
            ground=False)


def rect(box, extra=''):
    x, y, w, h = box
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" {extra}/>'


def expected(boxes, opacity=1):
    boxes = [tuple(F(str(v)) for v in box) for box in boxes]
    rows = []
    for y in range(24):
        row = []
        for x in range(24):
            xs = sorted({F(x), F(x + 1)} |
                        {max(F(x), min(F(x + 1), a))
                         for left, top, w, h in boxes for a in (left, left + w)})
            ys = sorted({F(y), F(y + 1)} |
                        {max(F(y), min(F(y + 1), a))
                         for left, top, w, h in boxes for a in (top, top + h)})
            area = sum((b - a) * (d - c)
                       for a, b in zip(xs, xs[1:])
                       for c, d in zip(ys, ys[1:])
                       if any(left <= (a + b) / 2 <= left + w and
                              top <= (c + d) / 2 <= top + h
                              for left, top, w, h in boxes))
            row.append((0, 1, 0, float(area) * opacity))
        rows.append(row)
    return f'<image width="24" height="24" href="{png(rows)}"/>'


def cell(name, content, boxes, opacity=1):
    i = len(unit.cells)
    x, y = i % 4 * 24, i // 4 * 24
    content = re.sub(r'(id="|#)([A-Za-z][\w-]*)',
                     lambda m: m[1] + name + '-' + m[2], content)
    unit.cell(name, (x, y, 24, 24),
              f'<svg x="{x}" y="{y}" width="24" height="24">{content}</svg>',
              f'<g transform="translate({x} {y})">{expected(boxes, opacity)}</g>')


def chain(boxes, drawing=None, shapes=None, attributes=''):
    definitions = []
    for i, box in enumerate(boxes):
        cut = f' clip-path="url(#c{i - 1})"' if i else ''
        shape = shapes[i] if shapes else rect(box)
        definitions.append(f'<clipPath id="c{i}"{cut}>{shape}</clipPath>')
    drawing = rect((0, 0, 24, 24), 'fill="lime"') if drawing is None else drawing
    return ('<defs>' + ''.join(definitions) + '</defs>' +
            f'<g clip-path="url(#c{len(boxes) - 1})" {attributes}>{drawing}</g>')


same = (2.25, 3.5, 12.5, 10.25)
cell('same-twice', chain([same] * 2), [same])
cell('same-eight-times', chain([same] * 8), [same])
cell('fractional-intersection',
     chain([(1.25, 2.25, 13.5, 12.5), (2.5, 1.5, 12, 14)]),
     [(2.5, 2.25, 12, 12.5)])
cell('two-cuts-in-one-pixel',
     chain([(2, 2, 12.75, 12), (2, 2, 12.5, 12)]), [(2, 2, 12.5, 12)])
cell('disjoint', chain([(2, 2, 4.25, 8), (8.5, 2, 4, 8)]), [])
cell('touching', chain([(2, 2, 4.25, 8), (6.25, 2, 4, 8)]), [])
cell('paint-edge', chain([same] * 4, rect(same, 'fill="lime"')), [same])
cell('opacity', chain([same] * 4, attributes='opacity=".5"'), [same], .5)
cell('child-clip',
     '<defs><clipPath id="a">' + rect(same) + '</clipPath>' +
     '<clipPath id="b">' + rect(same, 'clip-path="url(#a)"') +
     '</clipPath></defs><rect width="24" height="24" fill="lime" clip-path="url(#b)"/>',
     [same])
cell('use-offset',
     '<defs>' + rect((1, 1, 12.5, 10.25), 'id="r"') +
     '<clipPath id="a">' + rect(same) + '</clipPath>' +
     '<clipPath id="b" clip-path="url(#a)"><use href="#r" x="1.25" y="2.5"/></clipPath>' +
     '</defs><rect width="24" height="24" fill="lime" clip-path="url(#b)"/>', [same])
cell('rectangular-path', chain([same] * 2,
     shapes=['<path d="M2.25 3.5H14.75V13.75H2.25Z"/>' for _ in range(2)]), [same])
cell('reversed-path', chain([same] * 2,
     shapes=['<path d="M2.25 3.5V13.75H14.75V3.5Z"/>' for _ in range(2)]), [same])
cell('reflection', '<g transform="translate(20 0) scale(-1 1)">' +
     chain([same] * 3) + '</g>', [(5.25, 3.5, 12.5, 10.25)])
cell('quarter-turn', '<g transform="translate(20 0) rotate(90)">' +
     chain([same] * 3) + '</g>', [(6.25, 2.25, 10.25, 12.5)])
cell('separated-children',
     '<defs><clipPath id="a">' + rect((1, 1, 20, 20)) + '</clipPath>' +
     '<clipPath id="b" clip-path="url(#a)">' + rect((2, 3, 4, 10)) +
     rect((12, 3, 4, 10)) + '</clipPath></defs>' +
     '<rect width="24" height="24" fill="lime" clip-path="url(#b)"/>',
     [(2, 3, 4, 10), (12, 3, 4, 10)])
cell('one-clip-control', chain([same]), [same])
decimal = (2.1, 3.2, 12.3, 10.4)
cell('decimal-paint-edge', chain([decimal] * 4,
     rect(decimal, 'fill="lime"')), [decimal])
cell('decimal-repeated-cut', chain([decimal] * 4), [decimal])
cell('decimal-scale', '<g transform="scale(.8 1.2)">' +
     chain([same] * 3, rect(same, 'fill="lime"')) + '</g>',
     [(1.8, 4.2, 10, 12.3)])
cell('narrow-intersection',
     chain([(2, 2, 12.75, 12), (14.5, 2, .5, 12)]), [(14.5, 2, .25, 12)])
unit.write()
