"""The blend modes of Compositing 1 as its formulas define them, for the
expected drawings of the blending units."""
import math
modes = ['normal','multiply','screen','overlay','darken','lighten','color-dodge','color-burn','hard-light','soft-light','difference','exclusion','hue','saturation','color','luminosity']
def multiply(b, s): return b * s
def screen(b, s): return b + s - b * s
def hardlight(b, s): return multiply(b, 2 * s) if s <= .5 else screen(b, 2 * s - 1)
def softd(b): return ((16 * b - 12) * b + 4) * b if b <= .25 else math.sqrt(b)
sep = {
 'normal': lambda b, s: s, 'multiply': multiply, 'screen': screen, 'overlay': lambda b, s: hardlight(s, b),
 'darken': min, 'lighten': max,
 'color-dodge': lambda b, s: 0 if b == 0 else (1 if s == 1 else min(1, b / (1 - s))),
 'color-burn': lambda b, s: 1 if b == 1 else (0 if s == 0 else 1 - min(1, (1 - b) / s)),
 'hard-light': hardlight,
 'soft-light': lambda b, s: b - (1 - 2 * s) * b * (1 - b) if s <= .5 else b + (2 * s - 1) * (softd(b) - b),
 'difference': lambda b, s: abs(b - s), 'exclusion': lambda b, s: b + s - 2 * b * s}
def lum(c): return .3 * c[0] + .59 * c[1] + .11 * c[2]
def clipcolor(c):
    l = lum(c); n = min(c); x = max(c)
    if n < 0: c = [l + (v - l) * l / (l - n) for v in c]
    if x > 1: c = [l + (v - l) * (1 - l) / (x - l) for v in c]
    return c
def setlum(c, l): d = l - lum(c); return clipcolor([v + d for v in c])
def sat(c): return max(c) - min(c)
def setsat(c, s):
    order = sorted(range(3), key=lambda i: c[i]); mn, md, mx = order; out = [0.0] * 3
    if c[mx] > c[mn]:
        out[md] = (c[md] - c[mn]) * s / (c[mx] - c[mn]); out[mx] = s
    return out
nonsep = {
 'hue': lambda b, s: setlum(setsat(s, sat(b)), lum(b)), 'saturation': lambda b, s: setlum(setsat(b, sat(s)), lum(b)),
 'color': lambda b, s: setlum(s, lum(b)), 'luminosity': lambda b, s: setlum(b, lum(s))}
def blend(mode, b, s):
    return nonsep[mode](b, s) if mode in nonsep else [sep[mode](x, y) for x, y in zip(b, s)]
