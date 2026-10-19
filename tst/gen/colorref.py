"""CSS Color 4 sample code, ported: conversions to sRGB and the binary search
gamut mapping with local MINDE, for the expected drawings of the colour units."""
import math
def mm(M, v): return [sum(M[r][k] * v[k] for k in range(3)) for r in range(3)]
def lin_srgb(c): a = abs(c); return c / 12.92 if a <= 0.04045 else math.copysign(((a + 0.055) / 1.055) ** 2.4, c)
def gam_srgb(c): a = abs(c); return math.copysign(1.055 * a ** (1 / 2.4) - 0.055, c) if a > 0.0031308 else 12.92 * c
M_XYZ_SRGB = [[12831/3959, -329/214, -1974/3959], [-851781/878810, 1648619/878810, 36519/878810], [705/12673, -2585/12673, 705/667]]
M_SRGB_XYZ = [[506752/1228815, 87881/245763, 12673/70218], [87098/409605, 175762/245763, 12673/175545], [7918/409605, 87881/737289, 1001167/1053270]]
D50_D65 = [[0.955473421488075, -0.02309845494876471, 0.06325924320057072], [-0.0283697093338637, 1.0099953980813041, 0.021041441191917323], [0.012314014864481998, -0.020507649298898964, 1.330365926242124]]
D50 = [0.3457 / 0.3585, 1.0, (1.0 - 0.3457 - 0.3585) / 0.3585]
def lab_xyz(L, a, b):
    k = 24389 / 27; e = 216 / 24389
    f1 = (L + 16) / 116; f0 = a / 500 + f1; f2 = f1 - b / 200
    xyz = [f0 ** 3 if f0 ** 3 > e else (116 * f0 - 16) / k, ((L + 16) / 116) ** 3 if L > k * e else L / k, f2 ** 3 if f2 ** 3 > e else (116 * f2 - 16) / k]
    return [v * w for v, w in zip(xyz, D50)]
def oklab_xyz(L, a, b):
    M2 = [[1, 0.3963377773761749, 0.2158037573099136], [1, -0.1055613458156586, -0.0638541728258133], [1, -0.0894841775298119, -1.2914855480194092]]
    M1 = [[1.2268798758459243, -0.5578149944602171, 0.2813910456659647], [-0.0405757452148008, 1.1122868032803170, -0.0717110580655164], [-0.0763729366746601, -0.4214933324022432, 1.5869240198367816]]
    return mm(M1, [c ** 3 for c in mm(M2, [L, a, b])])
def xyz_oklab(x):
    M1 = [[0.8190224379967030, 0.3619062600528904, -0.1288737815209879], [0.0329836539323885, 0.9292868615863434, 0.0361446663506424], [0.0481771893596242, 0.2642395317527308, 0.6335478284694309]]
    M2 = [[0.2104542683093140, 0.7936177747023054, -0.0040720430116193], [1.9779985324311684, -2.4285922420485799, 0.4505937096174110], [0.0259040424655478, 0.7827717124575296, -0.8086757549230774]]
    return mm(M2, [math.copysign(abs(c) ** (1 / 3), c) for c in mm(M1, x)])
def xyz_srgb(x): return [gam_srgb(c) for c in mm(M_XYZ_SRGB, x)]
def srgb_oklab(c): return xyz_oklab(mm(M_SRGB_XYZ, [lin_srgb(v) for v in c]))
def gamut(rgb):
    inside = lambda c: all(-1e-9 <= v <= 1 + 1e-9 for v in c)
    clip = lambda c: [min(1, max(0, v)) for v in c]
    if inside(rgb): return clip(rgb)
    L, a, b = srgb_oklab(rgb)
    if L >= 1: return [1, 1, 1]
    if L <= 0: return [0, 0, 0]
    C = math.hypot(a, b); h = math.atan2(b, a)
    def at(c):
        cur = xyz_srgb(oklab_xyz(L, c * math.cos(h), c * math.sin(h))); cl = clip(cur)
        A = srgb_oklab(cur); B = srgb_oklab(cl)
        return math.dist(A, B), cur, cl
    e, cur, cl = at(C)
    if e < 0.02: return cl
    lo, hi, lo_in = 0.0, C, True
    while hi - lo > 0.0001:
        mid = (lo + hi) / 2
        e, cur, cl = at(mid)
        if lo_in and inside(cur): lo = mid; continue
        if e < 0.02:
            if 0.02 - e < 0.0001: break
            lo_in = False; lo = mid
        else: hi = mid
    return cl
SPACES = {
 'srgb': (lin_srgb, M_SRGB_XYZ, False), 'srgb-linear': (lambda c: c, M_SRGB_XYZ, False),
 'display-p3': (lin_srgb, [[608311/1250200, 189793/714400, 198249/1000160], [35783/156275, 247089/357200, 198249/2500400], [0, 32229/714400, 5220557/5000800]], False),
 'a98-rgb': (lambda c: math.copysign(abs(c) ** (563/256), c), [[573536/994567, 263643/1420810, 187206/994567], [591459/1989134, 6239551/9945670, 374412/4972835], [53769/1989134, 351524/4972835, 4929758/4972835]], False),
 'prophoto-rgb': (lambda c: c / 16 if abs(c) <= 16/512 else math.copysign(abs(c) ** 1.8, c), [[0.79776664490064230, 0.13518129740053308, 0.03134773412839220], [0.28807482881940130, 0.71183523424187300, 0.00008993693872564], [0, 0, 0.82510460251046020]], True),
 'rec2020': (lambda c: math.copysign(abs(c) ** 2.4, c), [[63426534/99577255, 20160776/139408157, 47086771/278816314], [26158966/99577255, 472592308/697040785, 8267143/139408157], [0, 19567812/697040785, 295819943/278816314]], False),
 'xyz': (lambda c: c, [[1,0,0],[0,1,0],[0,0,1]], False), 'xyz-d65': (lambda c: c, [[1,0,0],[0,1,0],[0,0,1]], False), 'xyz-d50': (lambda c: c, [[1,0,0],[0,1,0],[0,0,1]], True)}
def predefined(space, v):
    lin, M, d50 = SPACES[space]
    x = mm(M, [lin(c) for c in v])
    if d50: x = mm(D50_D65, x)
    return gamut(xyz_srgb(x))
def lab(L, a, b): return gamut(xyz_srgb(mm(D50_D65, lab_xyz(min(100, max(0, L)), a, b))))
def lch(L, C, H): C = max(0, C); return lab(L, C * math.cos(math.radians(H)), C * math.sin(math.radians(H)))
def oklab(L, a, b): return gamut(xyz_srgb(oklab_xyz(min(1, max(0, L)), a, b)))
def oklch(L, C, H): C = max(0, C); return oklab(L, C * math.cos(math.radians(H)), C * math.sin(math.radians(H)))
def hsl(H, S, L):
    H %= 360; S = max(0, S); a = S * min(L, 1 - L)
    f = lambda n: L - a * max(-1, min((n + H / 30) % 12 - 3, 9 - (n + H / 30) % 12, 1))
    return gamut([f(0), f(8), f(4)])
def hwb(H, W, B):
    if W + B >= 1: g = W / (W + B); return [g, g, g]
    return gamut([c * (1 - W - B) + W for c in hsl(H, 1, .5)])
