"""css-gradient-geometry: the CSS gradients of CSS Images 3 as masks, each
over the SVG gradient the module sets out, which the expected drawing
draws in its place."""
import math

from unitgen import Unit

W, H = 16, 10
B, T = 1.0, 0.0
unit = Unit("css-gradient-geometry", 102, 0, """Each test runs on into the
reference under it without a step: a CSS gradient as a mask over lime, and
under it an SVG gradient of lime set out by CSS Images 3: linear
directions by angle, side and corner, to bottom if none, the line's
length, stops past its ends, a colour hint's curve, the fixup of stop
positions, interpolation premultiplied, so white to transparent fades a
luminance mask evenly, within the rounding of luminance, stops at one place a step, repetition; radial extents for
circles and ellipses, a single length a circle, explicit sizes; and the
degenerate ones, invalid, of no radius, of no height or width, repeating
to the average. A gradient invalid by its grammar is no mask, and the lime
shows whole.""")
references = []


def stops_svg(stops):
    return "".join(f'<stop offset="{p:.5f}" stop-color="lime" stop-opacity="{a:.5f}"/>' for p, a in stops)


def hinted(p0, a0, p1, a1, hint, n=48):
    """A colour hint between two stops: C = P to the power log_H 0.5."""
    h = (hint - p0) / (p1 - p0)

    return [(p0 + (p1 - p0) * k / n, a0 + (a1 - a0) * (k / n) ** (math.log(.5) / math.log(h))) for k in range(n + 1)]


def linear(angle, stops, spread="pad"):
    def make(x0, y0):
        A = math.radians(angle)
        L = abs(W * math.sin(A)) + abs(H * math.cos(A))
        cx, cy = x0 + W / 2, y0 + H / 2
        dx, dy = math.sin(A) * L / 2, -math.cos(A) * L / 2
        first, last = stops[0][0], stops[-1][0]

        if spread == "repeat":
            sx, sy, ex, ey = cx - dx + 2 * dx * first, cy - dy + 2 * dy * first, cx - dx + 2 * dx * last, cy - dy + 2 * dy * last
            st = [((p - first) / (last - first), a) for p, a in stops]
        else:
            lo, hi = min(0, first), max(1, last)
            sx, sy, ex, ey = cx - dx + 2 * dx * lo, cy - dy + 2 * dy * lo, cx - dx + 2 * dx * hi, cy - dy + 2 * dy * hi
            st = [((p - lo) / (hi - lo), a) for p, a in stops]

        return f'gradientUnits="userSpaceOnUse" x1="{sx:.4f}" y1="{sy:.4f}" x2="{ex:.4f}" y2="{ey:.4f}" spreadMethod="{spread}">{stops_svg(st)}</linearGradient>', "linearGradient"

    return make


def radial(kind, stops, spread="pad"):
    def make(x0, y0):
        cx, cy, rx, ry = kind(x0, y0)

        return (f'gradientUnits="userSpaceOnUse" cx="{cx:.4f}" cy="{cy:.4f}" r="{rx:.4f}" spreadMethod="{spread}" '
                f'gradientTransform="translate({cx:.4f} {cy:.4f}) scale(1 {ry / rx:.6f}) translate({-cx:.4f} {-cy:.4f})">{stops_svg(stops)}</radialGradient>'), "radialGradient"

    return make


def solid(alpha):
    return lambda x0, y0: (f'><stop stop-color="lime" stop-opacity="{alpha}"/></linearGradient>', "linearGradient")


def extent(fx, fy, what, circle):
    """The radii of an ending shape at fx, fy of the box: closest-side,
    farthest-side, closest-corner or farthest-corner."""
    def kind(x0, y0):
        cx, cy = x0 + fx, y0 + fy
        sides = [cx - x0, x0 + W - cx, cy - y0, y0 + H - cy]
        sx = min(sides[0], sides[1]) if "closest" in what else max(sides[0], sides[1])
        sy = min(sides[2], sides[3]) if "closest" in what else max(sides[2], sides[3])

        if what == "closest-side":
            return (cx, cy) + ((min(sx, sy),) * 2 if circle else (sx, sy))

        if what == "farthest-side":
            return (cx, cy) + ((max(sx, sy),) * 2 if circle else (sx, sy))

        corners = [(x0, y0), (x0 + W, y0), (x0, y0 + H), (x0 + W, y0 + H)]
        corner = (min if what == "closest-corner" else max)(corners, key=lambda c: math.hypot(c[0] - cx, c[1] - cy))

        if circle:
            r = math.hypot(corner[0] - cx, corner[1] - cy)
            return cx, cy, r, r

        kx = sx / sy
        ry = math.sqrt((abs(corner[0] - cx) / kx) ** 2 + abs(corner[1] - cy) ** 2)

        return cx, cy, kx * ry, ry

    return kind


def sized(rx, ry):
    return lambda x0, y0: (x0 + W / 2, y0 + H / 2, rx, ry)


box = 0


def cell(name, cases, tolerance=None):
    global box
    tests, plains = [], []
    top = 2 + box * (2 * H + 3)
    box += (len(cases) + 4) // 5

    for k, (css, make) in enumerate(cases):
        x, y = 2 + 20 * (k % 5), top + (2 * H + 3) * (k // 5)
        attributes, element = make(x, y + H + 1)
        reference = f'<{element} id="{name}{k}" {attributes}'
        tests.append(f'{reference}<rect x="{x}" y="{y}" width="{W}" height="{H}" fill="lime" style="mask: {css}"/><rect x="{x}" y="{y + H + 1}" width="{W}" height="{H}" fill="url(#{name}{k})"/>')
        plains.append(f'{reference}<rect x="{x}" y="{y + H + 1}" width="{W}" height="{H}" fill="url(#{name}{k})" transform="translate(0 {-H - 1})"/><rect x="{x}" y="{y + H + 1}" width="{W}" height="{H}" fill="url(#{name}{k})"/>')

    rows = (len(cases) + 4) // 5
    unit.cell(name, (1, top - 1, 100, rows * (2 * H + 3)), "".join(tests), "".join(plains).replace(f' id="{name}', f' id="e{name}').replace(f'url(#{name}', f'url(#e{name}'), tolerance=tolerance)


cell("linear-default", [("linear-gradient(black, transparent)", linear(180, [(0, B), (1, T)])), ("linear-gradient(to bottom, black, transparent)", linear(180, [(0, B), (1, T)]))])
cell("linear-angle", [("linear-gradient(90deg, transparent, black)", linear(90, [(0, T), (1, B)])), ("linear-gradient(0.25turn, transparent, black)", linear(90, [(0, T), (1, B)])),
                      ("linear-gradient(0, black, transparent)", linear(0, [(0, B), (1, T)])), ("linear-gradient(-90deg, black, transparent)", linear(270, [(0, B), (1, T)]))])
cell("linear-sides", [("linear-gradient(to top, black, transparent)", linear(0, [(0, B), (1, T)])), ("linear-gradient(to right, black, transparent)", linear(90, [(0, B), (1, T)])),
                      ("linear-gradient(to left, black, transparent)", linear(270, [(0, B), (1, T)]))])
cell("linear-corner", [("linear-gradient(to top left, black, transparent)", linear(math.degrees(math.atan2(-H, W)), [(0, B), (1, T)])),
                       ("linear-gradient(to right bottom, black, transparent)", linear(180 - math.degrees(math.atan2(H, W)), [(0, B), (1, T)]))])
cell("linear-length", [("linear-gradient(30deg, black, transparent)", linear(30, [(0, B), (1, T)])), ("linear-gradient(120deg, black, transparent)", linear(120, [(0, B), (1, T)]))])
cell("linear-beyond", [("linear-gradient(45deg, black -20%, transparent 120%)", linear(45, [(-0.2, B), (1.2, T)]))])
cell("linear-invalid", [("linear-gradient(90, black, transparent)", solid(1)), ("linear-gradient(to left left, black, transparent)", solid(1)), ("linear-gradient(to top bottom, black, transparent)", solid(1))])
cell("hint", [("linear-gradient(90deg, black, 25%, transparent)", linear(90, hinted(0, B, 1, T, .25))), ("linear-gradient(90deg, black, 70%, transparent)", linear(90, hinted(0, B, 1, T, .7)))])
cell("hint-invalid", [("linear-gradient(90deg, 25%, black, transparent)", solid(1)), ("linear-gradient(90deg, black, transparent, 25%)", solid(1)), ("linear-gradient(90deg, black, 25%, 50%, transparent)", solid(1))])
cell("stop-lengths", [("linear-gradient(90deg, black 4px, transparent 12px)", linear(90, [(.25, B), (.75, T)]))])
cell("fixup-raise", [("linear-gradient(90deg, black 60%, transparent 20%)", linear(90, [(0, B), (.6, B), (.6, T), (1, T)]))])
cell("fixup-spread", [("linear-gradient(90deg, black, rgba(0, 0, 0, .5), transparent 80%, black)", linear(90, [(0, B), (.4, .5), (.8, T), (1, B)]))])
cell("premultiplied", [("linear-gradient(90deg, white, transparent) luminance", linear(90, [(0, B), (1, T)])), ("linear-gradient(90deg, transparent, white) luminance", linear(90, [(0, T), (1, B)]))], 6)
cell("stacked", [("linear-gradient(90deg, black 50%, transparent 50%)", linear(90, [(0, B), (.5, B), (.5, T), (1, T)]))])
cell("repeating", [("repeating-linear-gradient(90deg, black 0, transparent 4px)", linear(90, [(0, B), (4 / W, T)], "repeat")),
                   ("repeating-radial-gradient(circle, black 0, transparent 3px)", radial(sized(3, 3), [(0, B), (1, T)], "repeat"))])
cell("average", [("repeating-linear-gradient(black 5px, transparent 5px)", solid(.5)), ("repeating-linear-gradient(black 5px, transparent 5px, transparent 5px)", solid(.25)),
                 ("repeating-radial-gradient(5px 0px, black, transparent)", solid(.5))])
cell("radial-default", [("radial-gradient(black, transparent)", radial(extent(W / 2, H / 2, "farthest-corner", False), [(0, B), (1, T)])),
                        ("radial-gradient(ellipse farthest-corner at center, black, transparent)", radial(extent(W / 2, H / 2, "farthest-corner", False), [(0, B), (1, T)]))])
cell("radial-extents", [("radial-gradient(circle closest-side at 30% 50%, black, transparent)", radial(extent(.3 * W, .5 * H, "closest-side", True), [(0, B), (1, T)])),
                        ("radial-gradient(closest-side at 5px 4px, black, transparent)", radial(extent(5, 4, "closest-side", False), [(0, B), (1, T)])),
                        ("radial-gradient(farthest-side at 5px 4px, black, transparent)", radial(extent(5, 4, "farthest-side", False), [(0, B), (1, T)])),
                        ("radial-gradient(closest-corner at 4px 3px, black, transparent)", radial(extent(4, 3, "closest-corner", False), [(0, B), (1, T)])),
                        ("radial-gradient(circle farthest-corner at 4px 3px, black, transparent)", radial(extent(4, 3, "farthest-corner", True), [(0, B), (1, T)]))])
cell("radial-size", [("radial-gradient(6px, black, transparent)", radial(sized(6, 6), [(0, B), (1, T)])), ("radial-gradient(8px 3px, black, transparent)", radial(sized(8, 3), [(0, B), (1, T)])),
                     ("radial-gradient(50% 30%, black, transparent)", radial(sized(8, 3), [(0, B), (1, T)]))])
cell("radial-invalid", [("radial-gradient(circle 4px 4px, black, transparent)", solid(1)), ("radial-gradient(circle 50%, black, transparent)", solid(1)),
                        ("radial-gradient(closest-side 4px, black, transparent)", solid(1)), ("radial-gradient(-4px, black, transparent)", solid(1))])
cell("degenerate", [("radial-gradient(circle 0px, black, transparent)", solid(0)), ("radial-gradient(5px 0px, black, transparent)", solid(0)), ("radial-gradient(0px 5px, black, transparent)", solid(0))])
unit.height = 2 + box * (2 * H + 3)
unit.write()
