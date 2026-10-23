"""A path paints the union of its stroked subpaths once.

Butt-capped straight segments are rectangles. Their union is integrated
with an exact rational sweep: split at every vertical side, merge the
active vertical intervals, then intersect each slab with whole pixels.
Different path elements retain separate source-over operations.
"""
from fractions import Fraction as F
from math import ceil, floor

from unitgen import Unit, png


def rectangle_union_areas(rectangles, width, height):
    columns = [[] for _ in range(width)]
    result = [[F(0) for _ in range(width)] for _ in range(height)]
    for rect in rectangles:
        left, top, right, bottom = rect
        if left >= right or top >= bottom:
            continue
        for x in range(max(0, floor(left)), min(width, ceil(right))):
            columns[x].append(rect)
    for x, rectangles in enumerate(columns):
        cuts = {F(x), F(x + 1)}
        cuts.update(v for r in rectangles for v in [r[0], r[2]] if x < v < x + 1)
        cuts = sorted(cuts)
        for left, right in zip(cuts, cuts[1:]):
            intervals = sorted((max(F(0), r[1]), min(F(height), r[3]))
                               for r in rectangles if r[0] < right and r[2] > left)
            merged = []
            for top, bottom in intervals:
                if top >= bottom:
                    continue
                if merged and top <= merged[-1][1]:
                    merged[-1] = (merged[-1][0], max(merged[-1][1], bottom))
                else:
                    merged.append((top, bottom))
            for top, bottom in merged:
                for y in range(floor(top), ceil(bottom)):
                    area = (right - left) * (min(F(y + 1), bottom) - max(F(y), top))
                    result[y][x] += area
    assert all(0 <= value <= 1 for row in result for value in row)
    return result


def main():
    unit = Unit('stroke-dense-subpaths', 48, 80,
                'One path paints the union of all stroked subpaths once. '
                'Exact rational rectangle areas check overlapping, touching '
                'and disjoint thin butt-capped segments, varying heights, '
                'reversal, both axes, opacity, move-only subpaths and zero '
                'width. Separate path elements keep separate source-over '
                'operations; group opacity applies after those operations.',
                ground=False)
    cases = [
        ('overlap', F('.16'), F('.24'), False, False, False, False, F(1), F(1)),
        ('touch', F('.25'), F('.25'), False, False, False, False, F(1), F(1)),
        ('gaps', F('.3'), F('.2'), False, False, False, False, F(1), F(1)),
        ('varied', F('.16'), F('.24'), True, False, False, False, F(1), F(1)),
        ('reverse', F('.16'), F('.24'), True, True, False, False, F(1), F(1)),
        ('horizontal', F('.16'), F('.24'), True, False, True, False, F(1), F(1)),
        ('stroke-half', F('.16'), F('.24'), True, False, False, False, F('.5'), F(1)),
        ('split-elements', F('.16'), F('.24'), True, False, False, True, F(1), F(1)),
        ('split-half', F('.16'), F('.24'), True, False, False, True, F('.5'), F(1)),
        ('group-half', F('.16'), F('.24'), True, False, False, True, F(1), F('.5')),
        ('move-only', F('.16'), F('.24'), True, False, False, False, F(1), F(1)),
        ('zero-width', F('.16'), F(0), True, False, False, False, F(1), F(1)),
    ]
    for index, (name, step, stroke, varied, reverse, horizontal, split, opacity, group_alpha) in enumerate(cases):
        segments = []
        for i in range(40):
            x = F(2) + i * step
            top = F(3) + (F((i * 7) % 17, 5) if varied else 0)
            a, b = (x, top), (x, F(12))
            if horizontal:
                a, b = a[::-1], b[::-1]
            if reverse:
                a, b = b, a
            segments.append((a, b))
        if reverse:
            segments.reverse()
        groups = [segments[i:i + 8] for i in range(0, 40, 8)] if split else [segments]
        test = []
        alpha = [[F(0) for _ in range(16)] for _ in range(16)]
        for group in groups:
            data, rectangles = [], []
            for a, b in group:
                data.append(f'M{float(a[0]):.17g} {float(a[1]):.17g} L{float(b[0]):.17g} {float(b[1]):.17g}')
                if horizontal:
                    rectangles.append((min(a[0], b[0]), a[1] - stroke / 2,
                                       max(a[0], b[0]), a[1] + stroke / 2))
                else:
                    rectangles.append((a[0] - stroke / 2, min(a[1], b[1]),
                                       a[0] + stroke / 2, max(a[1], b[1])))
            if name == 'move-only':
                data += ['M1 1', 'M14 14']
            test.append(f'<path d="{" ".join(data)}" fill="none" stroke="lime" '
                        f'stroke-width="{float(stroke)}" stroke-linecap="butt" '
                        f'stroke-opacity="{float(opacity)}"/>')
            coverage = rectangle_union_areas(rectangles, 16, 16)
            for y in range(16):
                for x in range(16):
                    a = coverage[y][x] * opacity
                    alpha[y][x] = a + alpha[y][x] * (1 - a)
        expected = png([[(0, 1, 0, value * group_alpha) for value in row] for row in alpha])
        x, y = index % 3 * 16, index // 3 * 16
        unit.cell(name, (x, y, 16, 16),
                  f'<g transform="translate({x} {y})" opacity="{float(group_alpha)}">{"".join(test)}</g>',
                  f'<image x="{x}" y="{y}" width="16" height="16" href="{expected}"/>')
    for i, name in enumerate(['retraced', 'closed-line', 'repeated-loop']):
        if name == 'repeated-loop':
            d = 'M3 3' + 'H13V13H3V3' * 4 + 'Z'
            rectangles = [(F('2.875'), F('2.875'), F('13.125'), F('3.125')),
                          (F('2.875'), F('12.875'), F('13.125'), F('13.125')),
                          (F('2.875'), F('3.125'), F('3.125'), F('12.875')),
                          (F('12.875'), F('3.125'), F('13.125'), F('12.875'))]
            opacity = F('.5')
        else:
            d = 'M2 8H14' + ('H2H14' if name == 'retraced' else 'Z')
            rectangles = [(F(2), F('7.875'), F(14), F('8.125'))]
            opacity = F(1)
        areas = rectangle_union_areas(rectangles, 16, 16)
        expected = png([[(0, 1, 0, value * opacity) for value in row] for row in areas])
        x, y = i * 16, 64
        unit.cell(name, (x, y, 16, 16),
                  f'<g transform="translate({x} {y})"><path d="{d}" fill="none" '
                  f'fill-rule="evenodd" stroke="lime" stroke-width=".25" '
                  f'stroke-opacity="{float(opacity)}"/></g>',
                  f'<image x="{x}" y="{y}" width="16" height="16" href="{expected}"/>')
    unit.write()


if __name__ == '__main__':
    main()
