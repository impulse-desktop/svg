"""Subpixel source coverage carried through a shifted Gaussian shadow.

Expected pixels come from rectangle areas, interpolation and the Filter
Effects equations. The normalized Gaussian is summed over [-32, 32],
independently of the renderer's finite kernel and integer arithmetic.
"""
import math

from unitgen import Unit, png


def shadow(width, height, source_width, source_height, dx, dy, sx, sy,
           opacity, source_opacity):
    def coverage(count, size):
        return [max(0, min(i + 1, size) - max(i, 0)) for i in range(count)]

    def shifted(values, offset):
        def at(i):
            return values[i] if 0 <= i < len(values) else 0

        result = []
        for i in range(len(values)):
            position = i - offset
            left = math.floor(position)
            fraction = position - left
            result.append(at(left) * (1 - fraction) + at(left + 1) * fraction)
        return result

    def blurred(values, sigma):
        weights = {k: math.exp(-k * k / (2 * sigma * sigma)) for k in range(-32, 33)}
        total = sum(weights.values())
        return [sum(value * weights.get(i - j, 0) for j, value in enumerate(values)) / total
                for i in range(len(values))]

    x, y = coverage(width, source_width), coverage(height, source_height)
    bx, by = blurred(shifted(x, dx), sx), blurred(shifted(y, dy), sy)
    result = []
    for j in range(height):
        row = []
        for i in range(width):
            foreground = x[i] * y[j] * source_opacity
            # SourceAlpha atop the blurred alpha has the latter's alpha.
            # The color matrix makes it black and multiplies its alpha.
            backdrop = bx[i] * by[j] * opacity * source_opacity
            alpha = foreground + backdrop * (1 - foreground)
            gray = (215 / 255) * foreground / alpha if alpha else 0
            row.append((gray, gray, gray, alpha))
        result.append(row)
    return result


unit = Unit('filter-subpixel-shadow', 144, 56,
            'Exact source rectangle areas feed a fractional offset and normalized Gaussian. '
            'Atop preserves the blurred second input alpha, the color matrix scales it, '
            'then the gray foreground composites over the black shadow. Independent PNG '
            'expectations include line ends, filter edges, uniform and unequal transforms, '
            'an opaque row, negative offsets, translucent source, zero shadow opacity '
            'and clamped high-gain SourceAlpha before blur and out compositing. '
            'A real convex diagram node also checks its fill and miter stroke '
            'through blur, black alpha scaling, offset and SourceGraphic blending '
            'in linearRGB and sRGB, with translucent and zero-effect boundaries.',
            ground=False)
cases = [
    ('quarter-row', 1, 1, .25, .625, .625, 1.25, 1.25, 89 / 255, 1),
    ('scaled', .25, .25, 1, 2.5, 2.5, 5, 5, 89 / 255, 1),
    ('unequal-axes', .5, 1 / 3, 1, 1.25, 2.5, 2.5, 5, 89 / 255, 1),
    ('whole-row', 1, 1, 1, .625, .625, 1.25, 1.25, 89 / 255, 1),
    ('negative-offset', 1, 1, .5, .8, .7, -.375, -.25, .7, .6),
    ('zero-shadow', 1, 1, .25, .625, .625, 1.25, 1.25, 0, 1),
]
for i, (name, sx, sy, height, sigma_x, sigma_y, dx, dy, opacity, source_opacity) in enumerate(cases):
    x, y = i % 3 * 20, i // 3 * 12
    source = (f'<rect id="s{i}" width="{13.25 / sx}" height="{height}" '
              f'fill="#d7d7d7" fill-opacity="{source_opacity}"/>')
    definition = (f'<defs>{source}<filter id="f{i}" filterUnits="userSpaceOnUse" '
                  f'x="0" y="0" width="{16 / sx}" height="{6 / sy}">'
                  f'<feOffset in="SourceAlpha" dx="{dx}" dy="{dy}" result="offset"/>'
                  f'<feGaussianBlur in="offset" stdDeviation="{sigma_x} {sigma_y}" result="blur"/>'
                  '<feComposite in="SourceAlpha" in2="blur" operator="atop"/>'
                  f'<feColorMatrix values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 {opacity} 0"/>'
                  '</filter></defs>')
    test = (definition + f'<g transform="translate({x + 2} {y + 2}) scale({sx} {sy})">'
            f'<use href="#s{i}" filter="url(#f{i})"/><use href="#s{i}"/></g>')
    pixels = shadow(16, 6, 13.25, height * sy, dx * sx, dy * sy,
                    sigma_x * sx, sigma_y * sy, opacity, source_opacity)
    expected = (f'<image x="{x + 2}" y="{y + 2}" width="16" height="6" '
                f'image-rendering="pixelated" href="{png(pixels)}"/>')
    unit.cell(name, (x, y, 20, 12), test, expected)


def hard_shadow(source_height, source_opacity, cutout):
    # The clamp is two-dimensional: at a partial end pixel, multiplying
    # independently clamped x and y would give a different answer.
    source = [[max(0, min(i + 1, 13.25) - i)
               * max(0, min(j + 1, source_height) - j) * source_opacity
               for i in range(16)] for j in range(6)]
    hard = [[min(1, 127 * a) for a in row] for row in source]

    def at(i, j):
        return hard[j][i] if 0 <= i < 16 and 0 <= j < 6 else 0

    shifted = []
    for j in range(6):
        row = []
        for i in range(16):
            x, y = i - 1.25, j - 1.25
            ix, iy = math.floor(x), math.floor(y)
            fx, fy = x - ix, y - iy
            row.append((at(ix, iy) * (1 - fx) + at(ix + 1, iy) * fx) * (1 - fy)
                       + (at(ix, iy + 1) * (1 - fx) + at(ix + 1, iy + 1) * fx) * fy)
        shifted.append(row)
    weights = {k: math.exp(-k * k / (2 * .625 ** 2)) for k in range(-32, 33)}
    total = sum(weights.values()) ** 2
    result = []
    for j in range(6):
        row = []
        for i in range(16):
            blurred = sum(shifted[y][x] * weights[i - x] * weights[j - y]
                          for y in range(6) for x in range(16)) / total
            backdrop = blurred * (1 - hard[j][i] if cutout else 1) * .25
            foreground = source[j][i]
            alpha = foreground + backdrop * (1 - foreground)
            gray = (215 / 255) * foreground / alpha if alpha else 0
            row.append((gray, gray, gray, alpha))
        result.append(row)
    return result


for i, (name, height, opacity, cutout) in enumerate([
    ('hard-alpha-row', .25, 1, False),
    ('hard-alpha-subbyte', 1, 1 / 254, False),
    ('hard-alpha-out', .25, 1, True),
]):
    x, y = i * 20, 24
    test = (f'<defs><filter id="h{i}" filterUnits="userSpaceOnUse" '
            'x="0" y="0" width="16" height="6" color-interpolation-filters="sRGB">'
            '<feColorMatrix in="SourceAlpha" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 127 0" result="hard"/>'
            '<feOffset dx="1.25" dy="1.25"/>'
            '<feGaussianBlur stdDeviation=".625"/>'
            + ('<feComposite in2="hard" operator="out"/>' if cutout else '')
            + '<feColorMatrix values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 .25 0"/>'
            '<feBlend in="SourceGraphic"/></filter></defs>'
            f'<g transform="translate({x + 2} {y + 2})">'
            f'<rect width="13.25" height="{height}" fill="#d7d7d7" '
            f'fill-opacity="{opacity:.17g}" filter="url(#h{i})"/></g>')
    expected = (f'<image x="{x + 2}" y="{y + 2}" width="16" height="6" '
                f'image-rendering="pixelated" href="{png(hard_shadow(height, opacity, cutout))}"/>')
    unit.cell(name, (x, y, 20, 12), test, expected)


def diagram_shadow(linear_rgb, fill_opacity=1, stroke_opacity=1,
                   deviation=2, shadow_opacity=.4):
    """Exact convex areas and filter equations for an actual diagram node.

    The six authored vertices have only convex miter joins. Offsetting
    adjacent lines and intersecting them constructs its outer/inner stroke
    polygons. Clipping each polygon to a pixel gives the source coverage.
    Neither the renderer's stroker nor its blur kernel is used here.
    """
    points = [(6,73),(16,63),(126,63),(126,99.2969),(116,109.2969),(6,109.2969)]
    sx, sy = 256/1828, 16/114
    width, height, pad = 24, 20, 4

    def cross(a, b):
        return a[0]*b[1]-a[1]*b[0]

    def offset(distance):
        result = []
        for i,p in enumerate(points):
            before, after = points[i-1], points[(i+1)%len(points)]
            a, b = (p[0]-before[0],p[1]-before[1]), (after[0]-p[0],after[1]-p[1])
            la, lb = math.hypot(*a), math.hypot(*b)
            u = (p[0]+distance*a[1]/la,p[1]-distance*a[0]/la)
            v = (p[0]+distance*b[1]/lb,p[1]-distance*b[0]/lb)
            t = cross((v[0]-u[0],v[1]-u[1]),b)/cross(a,b)
            result.append((u[0]+t*a[0],u[1]+t*a[1]))
        return result

    def area(polygon, x, y):
        for axis,value,sign in [(0,x,1),(0,x+1,-1),(1,y,1),(1,y+1,-1)]:
            clipped = []
            for a,b in zip(polygon,polygon[1:]+polygon[:1]):
                da, db = sign*(a[axis]-value), sign*(b[axis]-value)
                if da >= 0:
                    clipped.append(a)
                if (da >= 0) != (db >= 0):
                    t = da/(da-db)
                    clipped.append(tuple(a[k]+t*(b[k]-a[k]) for k in (0,1)))
            polygon = clipped
            if not polygon:
                return 0
        return abs(sum(cross(a,b) for a,b in zip(polygon,polygon[1:]+polygon[:1])))/2

    polygons = [[(x*sx+pad,y*sy+pad) for x,y in p]
                for p in (points,offset(.75),offset(-.75))]
    w, h = width+2*pad, height+2*pad
    alpha, straight = [], []
    for y in range(h):
        ar, cr = [], []
        for x in range(w):
            fill, outer, inner = [area(p,x,y) for p in polygons]
            f, s = fill*fill_opacity, (outer-inner)*stroke_opacity
            a = s+f*(1-s)
            ar.append(a)
            cr.append(tuple(c*f*(1-s)/a if a else 0 for c in (254/255,254/255,206/255)))
        alpha.append(ar)
        straight.append(cr)

    def at(values, x, y):
        return values[y][x] if 0 <= x < w and 0 <= y < h else 0

    blur = alpha
    for axis,sigma in [(1,deviation*sx),(0,deviation*sy)]:
        weights = [(k,math.exp(-k*k/(2*sigma*sigma))) for k in range(-32,33)] if sigma else [(0,1)]
        total = sum(weight for k,weight in weights)
        blur = [[sum(at(blur,x-k if axis else x,y if axis else y-k)*weight/total
                     for k,weight in weights) for x in range(w)] for y in range(h)]
    dx, dy = 4*sx, 4*sy
    shadow = []
    for y in range(h):
        row = []
        for x in range(w):
            px, py = x-dx, y-dy
            ix, iy = math.floor(px), math.floor(py)
            fx, fy = px-ix, py-iy
            row.append(((at(blur,ix,iy)*(1-fx)+at(blur,ix+1,iy)*fx)*(1-fy)
                        +(at(blur,ix,iy+1)*(1-fx)+at(blur,ix+1,iy+1)*fx)*fy)*shadow_opacity)
        shadow.append(row)

    def to_linear(c):
        return c/12.92 if c <= .04045 else ((c+.055)/1.055)**2.4

    def from_linear(c):
        return 12.92*c if c <= .0031308 else 1.055*c**(1/2.4)-.055

    result = []
    for y in range(pad,pad+height):
        row = []
        for x in range(pad,pad+width):
            a = alpha[y][x]
            out = a+shadow[y][x]*(1-a)
            rgb = [from_linear(to_linear(c)*a/out) if linear_rgb else c*a/out
                   for c in straight[y][x]] if out else [0,0,0]
            row.append((*rgb,out))
        result.append(row)
    return result


for i,(name,linear,fill,stroke,sigma,opacity) in enumerate([
    ('diagram-linear',True,1,1,2,.4),
    ('diagram-srgb',False,1,1,2,.4),
    ('diagram-translucent',True,.5,.5,2,.4),
    ('diagram-zero-blur',True,1,1,0,.4),
    ('diagram-zero-shadow',True,1,1,2,0),
    ('diagram-near-two',True,1,1,14,.4),
]):
    x, y = i*24, 36
    color_space = '' if linear else ' color-interpolation-filters="sRGB"'
    # Keep the larger Gaussian's tail out of the adjacent independent cell.
    test = (f'<svg x="{x}" y="{y}" width="24" height="20" overflow="hidden">'
            f'<defs><filter id="diagram{i}" x="-1" y="-1" width="300%" height="300%"{color_space}>'
            f'<feGaussianBlur stdDeviation="{sigma}" result="blur"/>'
            f'<feColorMatrix in="blur" values="0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 {opacity} 0" result="black"/>'
            '<feOffset in="black" dx="4" dy="4" result="offset"/>'
            '<feBlend in="SourceGraphic" in2="offset" mode="normal"/></filter></defs>'
            f'<g transform="scale({256/1828:.17g} {16/114:.17g})">'
            '<polygon points="6,73 16,63 126,63 126,99.2969 116,109.2969 6,109.2969" '
            f'fill="#FEFECE" fill-opacity="{fill}" stroke="black" stroke-width="1.5" '
            f'stroke-opacity="{stroke}" filter="url(#diagram{i})"/></g></svg>')
    expected = (f'<image x="{x}" y="{y}" width="24" height="20" image-rendering="pixelated" '
                f'href="{png(diagram_shadow(linear,fill,stroke,sigma,opacity))}"/>')
    unit.cell(name,(x,y,24,20),test,expected)
unit.write()
