"""mask-tiling: the layer tiling of CSS Backgrounds 3 in mask layers, each
mask over the tiles the expected drawing draws in its place."""
import math

from unitgen import Unit, png

W, H = 16, 10
IMAGE = png([[(0, 0, 0, 1)] * 3] * 2)
G = "linear-gradient(black, black)"
unit = Unit("mask-tiling", 102, 0, """Each test runs on into the reference under
it without a step: a mask of tiles over lime, and under it the tiles as CSS
Backgrounds lays them out, drawn as lime rectangles: repeat, repeat-x and
-y, two keywords, space, space with room for one, round; positions of one,
two and four components, keywords in either order, an offset inward from
its edge, three components being no <position> and so no mask, and
percentages of the room left; natural size, contain, cover, one length
with the ratio, a percentage, a size of zero, a negative one invalid, round
with an auto side; mask-origin and mask-clip by box, where an SVG
element's border-box is its fill box for the origin, its stroke box for the
clip, as CSS Masking has it. A 3 by 2 black image
and a black gradient make the tiles.""")


def tiles(mode, area, size, position=0):
    if mode == "no-repeat":
        return [(position, size)]

    if mode == "repeat":
        x = position - math.ceil(position / size) * size
        out = []

        while x < area:
            out.append((x, size))
            x += size

        return out

    n = int(area // size)

    return [(position, size)] if n < 2 else [(k * (size + (area - n * size) / (n - 1)), size) for k in range(n)]


def case(css, xs, ys):
    return css, [(x, y, w, h) for x, w in xs for y, h in ys]


row = 0


def cell(name, cases):
    global row
    tests, plains = [], []
    top = 2 + 24 * row

    for k, (css, rects) in enumerate(cases):
        x0, y0 = 2 + 20 * (k % 5), top + 24 * (k // 5)
        reference = '<svg x="{0}" y="{1}" width="%d" height="%d">' % (W, H) + "".join(f'<rect x="{x:.4f}" y="{y:.4f}" width="{w:.4f}" height="{h:.4f}" fill="lime"/>' for x, y, w, h in rects) + "</svg>"
        tests.append(f'<rect x="{x0}" y="{y0}" width="{W}" height="{H}" fill="lime" style="mask: {css}"/>' + reference.format(x0, y0 + H + 1))
        plains.append(reference.format(x0, y0) + reference.format(x0, y0 + H + 1))

    count = (len(cases) + 4) // 5
    unit.cell(name, (1, top - 1, 100, 24 * count), "".join(tests), "".join(plains))
    row += count


cell("repeat", [case(f"{G} 0 0 / 4px 3px repeat", tiles("repeat", W, 4), tiles("repeat", H, 3)), case(f"{G} 2px 1px / 4px 3px", tiles("repeat", W, 4, 2), tiles("repeat", H, 3, 1))])
cell("repeat-x-y", [case(f"{G} 0 0 / 4px 3px repeat-x", tiles("repeat", W, 4), [(0, 3)]), case(f"{G} 0 0 / 4px 3px repeat-y", [(0, 4)], tiles("repeat", H, 3))])
cell("two-keywords", [case(f"{G} 0 0 / 4px 3px no-repeat repeat", [(0, 4)], tiles("repeat", H, 3)), case(f"{G} 0 0 / 4px 3px repeat no-repeat", tiles("repeat", W, 4), [(0, 3)])])
cell("space", [case(f"{G} 0 0 / 6px 4px space", tiles("space", W, 6), tiles("space", H, 4)), case(f"{G} 50% 0 / 9px 3px space no-repeat", [((W - 9) / 2, 9)], [(0, 3)])])
cell("round", [case(f"{G} 0 0 / 7px 6px round", [(k * 8, 8) for k in range(2)], [(k * 5, 5) for k in range(2)])])
cell("no-repeat", [case(f"{G} 100% 100% / 4px 3px no-repeat", [(W - 4, 4)], [(H - 3, 3)]), case(f"{G} 5px 2px / 4px 3px no-repeat", [(5, 4)], [(2, 3)])])
cell("position-percent", [case(f"{G} 25% bottom / 4px 3px no-repeat", [((W - 4) * .25, 4)], [(H - 3, 3)]), case(f"{G} 50% 50% / 4px 3px no-repeat", [(6, 4)], [(3.5, 3)])])
cell("position-one", [case(f"{G} right / 4px 3px no-repeat", [(W - 4, 4)], [((H - 3) / 2, 3)]), case(f"{G} 2px / 4px 3px no-repeat", [(2, 4)], [((H - 3) / 2, 3)])])
cell("position-two", [case(f"{G} 2px 3px / 4px 3px no-repeat", [(2, 4)], [(3, 3)]), case(f"{G} bottom right / 4px 3px no-repeat", [(W - 4, 4)], [(H - 3, 3)]),
                      case(f"{G} 2px left / 4px 3px no-repeat", [(0, W)], [(0, H)])])
cell("position-four", [case(f"{G} right 2px bottom 3px / 4px 3px no-repeat", [(W - 2 - 4, 4)], [(H - 3 - 3, 3)]), case(f"{G} top 2px left -1px / 4px 3px no-repeat", [(-1, 4)], [(2, 3)])])
cell("position-three", [case(f"{G} left 3px top / 4px 3px no-repeat", [(0, W)], [(0, H)])])
cell("position-keywords", [case(f"{G} center top / 4px 3px no-repeat", [(6, 4)], [(0, 3)]), case(f"{G} left center / 4px 3px no-repeat", [(0, 4)], [(3.5, 3)])])
cell("size-natural", [case(f"url({IMAGE}) 0 0 no-repeat", [(0, 3)], [(0, 2)])])
cell("size-contain", [case(f"url({IMAGE}) 0 0 / contain no-repeat", [(0, 15)], [(0, 10)])])
cell("size-cover", [case(f"url({IMAGE}) 0 0 / cover no-repeat", [(0, 16)], [(0, 32 / 3)])])
cell("size-one", [case(f"url({IMAGE}) 0 0 / 6px no-repeat", [(0, 6)], [(0, 4)]), case(f"{G} 0 0 / 50% no-repeat", [(0, 8)], [(0, 10)])])
cell("size-auto", [case(f"url({IMAGE}) 0 0 / auto 4px no-repeat", [(0, 6)], [(0, 4)]), case(f"{G} 0 0 / auto auto no-repeat", [(0, 16)], [(0, 10)])])
cell("size-zero", [case(f"{G} 0 0 / 0 3px", [], [])])
cell("size-negative", [case(f"{G} 0 0 / -4px 3px no-repeat", [(0, W)], [(0, H)])])
cell("round-auto", [case(f"url({IMAGE}) 0 0 / 7px auto round no-repeat", [(k * 8, 8) for k in range(2)], [(0, 16 / 3)])])


def boxes(name, cases):
    global row
    top = 2 + 24 * row
    tests, plains = [], []

    for k, (style, rect) in enumerate(cases):
        x0 = 2 + 20 * k
        tests.append(f'<rect x="{x0 + 4}" y="{top + 4}" width="8" height="8" fill="lime" stroke="lime" stroke-width="4" style="{style}"/>')
        plains.append('<rect x="%g" y="%g" width="%g" height="%g" fill="lime"/>' % (x0 + rect[0], top + rect[1], rect[2], rect[3]))

    unit.cell(name, (1, top - 1, 100, 24), "".join(tests), "".join(plains), "".join(plains).replace(' fill="lime"', ""))
    row += 1


boxes("origin", [(f"mask: {G} 0 0 / 4px 4px no-repeat; mask-origin: border-box", (4, 4, 4, 4)), (f"mask: {G} 0 0 / 4px 4px no-repeat; mask-origin: padding-box", (4, 4, 4, 4)),
                 (f"mask: {G} 0 0 / 4px 4px no-repeat; mask-origin: content-box", (4, 4, 4, 4)), (f"mask: {G} 0 0 / 4px 4px no-repeat", (4, 4, 4, 4)),
                 (f"mask: {G} 0 0 / 4px 4px no-repeat; mask-origin: stroke-box", (2, 2, 4, 4))])
boxes("clip", [(f"mask: {G}; mask-clip: border-box", (2, 2, 12, 12)), (f"mask: {G}; mask-clip: padding-box", (4, 4, 8, 8)), (f"mask: {G}; mask-clip: content-box", (4, 4, 8, 8))])
unit.height = 2 + 24 * row
unit.write()
