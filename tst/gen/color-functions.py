"""color-functions: the colours of CSS Color 4, each test drawn over the
reference the expected drawing draws in its place, from the sample code
of the module (colorref.py)."""
import math

from colorref import SPACES, gamut, hsl, hwb, lab, lch, mm, oklab, oklch, predefined, xyz_srgb
from unitgen import Unit, rows

unit = Unit("color-functions", 66, 0, """Each test runs on into the
reference under it without a step, and lime fills what is lime; nothing
red shows. CSS Color 4's colours: hex of 3, 4, 6 and 8 digits in any case,
named colours, transparent, currentcolor, system colours and their
deprecated aliases; rgb() and hsl() modern and legacy, hwb(), lab(),
lch(), oklab(), oklch() and color() in each predefined space, none for a
missing component, components clamped where the module clamps them, the
rest mapped into sRGB by its binary search on OkLCh chroma; values of the
wrong shape, each ignored, so the lime attribute paints.""")


def hexed(rgb):
    return "#%02x%02x%02x" % tuple(round(v * 255) for v in rgb)


def item(x, y, value):
    if isinstance(value, str):
        test = f'<rect x="{x}" y="{y}" width="6" height="10" fill="lime" style="fill: {value[1:]}"/>'
        plain = f'<rect x="{x}" y="{y}" width="6" height="10" fill="lime"/>'

        return test, plain, ""

    css, rgb, alpha = value
    opacity = "" if alpha == 1 else f' fill-opacity="{alpha:g}"'
    reference = f'<rect x="{x}" y="{{}}" width="6" height="5" fill="{hexed(rgb)}"{opacity}/>'

    return f'<rect x="{x}" y="{y}" width="6" height="5" style="fill: {css}"/>' + reference.format(y + 5), reference.format(y) + reference.format(y + 5), ""


def system(name, value):
    return (name, [(value >> 24 & 255) / 255, (value >> 16 & 255) / 255, (value >> 8 & 255) / 255], (value & 255) / 255)


CANVAS, TEXT, BORDER, FACE, GRAY = 0xffffffff, 0x000000ff, 0x000000ff, 0xefefefff, 0x808080ff
lin, M, _ = SPACES["display-p3"]
y = rows(unit, 2, [
    ("kinds", [("#00ff00", [0, 1, 0], 1), ("rgb(0 255 0)", [0, 1, 0], 1), ("lime", [0, 1, 0], 1), ("Canvas", [1, 1, 1], 1)]),
    ("functions", [("rgb(10 20 30)", [10 / 255, 20 / 255, 30 / 255], 1), ("rgba(10 20 30)", [10 / 255, 20 / 255, 30 / 255], 1), ("hsl(120deg 50% 50%)", hsl(120, .5, .5), 1),
                   ("hsla(120deg 50% 50%)", hsl(120, .5, .5), 1), ("hwb(120 20% 30%)", hwb(120, .2, .3), 1), ("lab(50 40 -30)", lab(50, 40, -30), 1), ("lch(60 50 120)", lch(60, 50, 120), 1),
                   ("oklab(0.6 0.1 -0.1)", oklab(.6, .1, -.1), 1), ("oklch(0.7 0.15 200)", oklch(.7, .15, 200), 1), ("color(srgb 0.2 0.5 0.8)", predefined("srgb", [.2, .5, .8]), 1)]),
    ("legacy-only", ["!hwb(120, 20%, 30%)", "!lab(50, 40, -30)", "!lch(50, 20, 30)", "!oklab(0.5, 0.1, 0.1)", "!oklch(0.5, 0.1, 30)", "!color(srgb, 1, 0, 0)"]),
    ("modern", [("rgb(10 20% 30 / 50%)", [10 / 255, .2, 30 / 255], .5), ("hsl(0.5turn 50 50)", hsl(180, .5, .5), 1), ("rgb(none 128 255)", [0, 128 / 255, 1], 1)]),
    ("legacy", [("rgb(10, 20, 30, 50%)", [10 / 255, 20 / 255, 30 / 255], .5), ("rgba(10%, 20%, 30%, .5)", [.1, .2, .3], .5), ("hsl(120, 50%, 50%, 0.3)", hsl(120, .5, .5), .3),
                ("hsla(120, 50%, 50%)", hsl(120, .5, .5), 1)]),
    ("legacy-invalid", ["!rgb(10, 20%, 30)", "!rgb(none, 1, 2)", "!rgb(1, 2, 3 / 4)", "!hsl(120, 50, 50)", "!rgb(1,2,3,)", "!rgb(1,2,3,4,5)", "!rgb(1 2 3,)"]),
    ("missing", [("oklch(0.7 none none)", oklch(.7, 0, 0), 1), ("rgb(255 128 0 / none)", [1, 128 / 255, 0], 0), ("lab(50 none 30)", lab(50, 0, 30), 1)]),
    ("hue", [("hsl(-540 100% 50%)", hsl(180, 1, .5), 1), ("hsl(540 100% 50%)", hsl(180, 1, .5), 1), ("hsl(calc(infinity) 100% 50%)", hsl(0, 1, .5), 1), ("hsl(calc(-infinity) 100% 50%)", hsl(0, 1, .5), 1),
             ("lch(50 40 calc(infinity))", lch(50, 40, 0), 1), ("hsl(120DEG 50% 50%)", hsl(120, .5, .5), 1), ("hsl(360 100% 50%)", hsl(0, 1, .5), 1)]),
    ("hue-invalid", ["!hsl(120% 50% 50%)", "!hsl(120deg% 50% 50%)", "!lch(50 20 30%)"]),
    ("alpha", [("hsla(200 40% 60% / 2)", hsl(200, .4, .6), 1), ("rgb(1 2 3 / -0.5)", [1 / 255, 2 / 255, 3 / 255], 0), ("rgb(10 20 30 / 25%)", [10 / 255, 20 / 255, 30 / 255], .25)]),
    ("hex-6-8", [("#ff8000", [1, 128 / 255, 0], 1), ("#ff800080", [1, 128 / 255, 0], 128 / 255)]),
    ("hex-3-4", [("#f80", [1, 136 / 255, 0], 1), ("#f808", [1, 136 / 255, 0], 136 / 255), ("#123", [17 / 255, 34 / 255, 51 / 255], 1)]),
    ("hex-case", [("#FF8000", [1, 128 / 255, 0], 1), ("#Ff8", [1, 1, 136 / 255], 1)]),
    ("hex-invalid", ["!#12345", "!#1234567", "!#12", "!#12345g", "!#"]),
    ("rgb-forms", [("rgb(10, 20, 30)", [10 / 255, 20 / 255, 30 / 255], 1), ("rgb(10%, 20%, 30%)", [.1, .2, .3], 1), ("rgba(10, 20, 30, .5)", [10 / 255, 20 / 255, 30 / 255], .5),
                   ("rgb(10 20 30 / 50%)", [10 / 255, 20 / 255, 30 / 255], .5)]),
    ("rgb-percent", [("rgb(100% 0% 50%)", [1, 0, .5], 1), ("rgb(0 0 0 / 50%)", [0, 0, 0], .5)]),
    ("rgb-clamp", [("rgb(300 -20 128)", [1, 0, 128 / 255], 1), ("rgb(120% -10% 50%)", [1, 0, .5], 1)]),
    ("rgb-invalid", ["!rgb(1 2 3 / 4 5)", "!rgb(1 2 3 4)", "!rgb(1 2 3 /)", "!rgb(1px 2 3)", "!rgb(1~ 2 3)"]),
    ("named", [("rebeccapurple", [102 / 255, 51 / 255, 153 / 255], 1), ("LIME", [0, 1, 0], 1), ("GoldenRod", [218 / 255, 165 / 255, 32 / 255], 1), ("grey", [128 / 255, 128 / 255, 128 / 255], 1)]),
    ("system", [system(n, v) for n, v in [("AccentColor", 0x0075ffff), ("AccentColorText", 0xffffffff), ("ActiveText", 0xff0000ff), ("ButtonBorder", BORDER), ("ButtonFace", FACE),
                                          ("ButtonText", TEXT), ("Canvas", CANVAS), ("CanvasText", TEXT)]]),
    ("system-more", [system(n, v) for n, v in [("Field", CANVAS), ("FieldText", TEXT), ("GrayText", GRAY), ("Highlight", 0x0041c6cc), ("HighlightText", 0xffffffff), ("LinkText", 0x0000eeff),
                                               ("Mark", 0xffff00ff), ("MarkText", TEXT), ("SelectedItem", 0x1967d2ff), ("SelectedItemText", 0xffffffff), ("VisitedText", 0x551a8bff), ("canvastext", TEXT)]]),
    ("system-deprecated", [system(n, v) for n, v in [("ActiveBorder", BORDER), ("ActiveCaption", CANVAS), ("AppWorkspace", CANVAS), ("Background", CANVAS), ("ButtonHighlight", FACE),
                                                     ("ButtonShadow", FACE), ("CaptionText", TEXT), ("InactiveBorder", BORDER), ("InactiveCaption", CANVAS), ("InactiveCaptionText", GRAY),
                                                     ("InfoBackground", CANVAS), ("InfoText", TEXT), ("Menu", CANVAS), ("MenuText", TEXT), ("Scrollbar", CANVAS), ("ThreeDDarkShadow", BORDER),
                                                     ("ThreeDFace", FACE), ("ThreeDHighlight", BORDER), ("ThreeDLightShadow", BORDER), ("ThreeDShadow", BORDER), ("Window", CANVAS), ("WindowFrame", BORDER),
                                                     ("WindowText", TEXT)]]),
    ("hsl-forms", [("hsl(120 50% 50%)", hsl(120, .5, .5), 1), ("hsl(none 50% 50%)", hsl(0, .5, .5), 1), ("hsl(120, 50%, 50%)", hsl(120, .5, .5), 1), ("hsl(120 50 50 / .5)", hsl(120, .5, .5), .5)]),
    ("hsl-values", [("hsl(0 100% 50%)", [1, 0, 0], 1), ("hsl(120 100% 50%)", [0, 1, 0], 1), ("hsl(240 100% 50%)", [0, 0, 1], 1), ("hsl(0 0% 50%)", [.5, .5, .5], 1), ("hsl(0 100% 100%)", [1, 1, 1], 1),
                    ("hsl(0 100% 0%)", [0, 0, 0], 1)]),
    ("hsl-clamp", [("hsl(-90 -20% 40%)", hsl(270, 0, .4), 1), ("hsl(30 150% 50%)", hsl(30, 1.5, .5), 1)]),
    ("hsl-powerless", [("hsl(123 0% 40%)", hsl(0, 0, .4), 1), ("hsl(none 0% 40%)", hsl(0, 0, .4), 1)]),
    ("hsl-convert", [("hsl(200 40% 60%)", hsl(200, .4, .6), 1), ("hsl(330 80% 25%)", hsl(330, .8, .25), 1), ("hsl(45 90% 70%)", hsl(45, .9, .7), 1)]),
    ("hwb", [("hwb(120 20% 30%)", hwb(120, .2, .3), 1), ("hwb(0.25turn 10 20 / 50%)", hwb(90, .1, .2), .5), ("hwb(none 20% 30%)", hwb(0, .2, .3), 1)]),
    ("hwb-legacy", ["!hwb(120, 20%, 30%)", "!hwb(120 20%, 30%)"]),
    ("hwb-gray", [("hwb(200 60% 60%)", hwb(200, .6, .6), 1), ("hwb(10 70% 30%)", hwb(0, .7, .3), 1)]),
    ("hwb-hue", [("hwb(480 20% 30%)", hwb(120, .2, .3), 1), ("hwb(-240 20% 30%)", hwb(120, .2, .3), 1)]),
    ("lab-white", [("lab(100 0 0)", [1, 1, 1], 1), ("lab(50 0 0)", lab(50, 0, 0), 1)]),
    ("lab", [("lab(50% 50% -50%)", lab(50, 62.5, -62.5), 1), ("lab(60 -40 30)", lab(60, -40, 30), 1)]),
    ("lab-clamp", [("lab(120 0 0)", lab(100, 0, 0), 1), ("lab(-10 20 20)", lab(0, 20, 20), 1), ("lab(50 200 -200)", lab(50, 200, -200), 1)]),
    ("lch", [("lch(40% 50% 1rad)", lch(40, 75, math.degrees(1)), 1), ("lch(60 -5 120)", lch(60, 0, 0), 1), ("lch(60 50 120 / 50%)", lch(60, 50, 120), .5)]),
    ("lch-hue", [("lch(50 40 0)", lch(50, 40, 0), 1), ("lch(50 40 90)", lch(50, 40, 90), 1), ("lch(50 40 180)", lch(50, 40, 180), 1), ("lch(50 40 270)", lch(50, 40, 270), 1)]),
    ("lab-legacy", ["!lab(50, 40, -30)", "!lch(50, 20, 30)", "!lch(50 20deg 30)"]),
    ("lch-powerless", [("lch(50 0 123)", lch(50, 0, 0), 1), ("lch(0 30 40)", [0, 0, 0], 1), ("lch(100 30 40)", [1, 1, 1], 1)]),
    ("lab-lch", [("lch(50 50 36.8699)", lab(50, 40, 30), 1), ("lab(50 40 30)", lab(50, 40, 30), 1), ("lch(50 30 none)", lab(50, 30, 0), 1)]),
    ("oklab-white", [("oklab(1 0 0)", [1, 1, 1], 1), ("oklab(0.5 0 0)", oklab(.5, 0, 0), 1)]),
    ("oklab", [("oklab(60% 25% -25%)", oklab(.6, .1, -.1), 1), ("oklab(0.6 0.1 -0.1)", oklab(.6, .1, -.1), 1)]),
    ("oklab-clamp", [("oklab(1.2 0 0)", [1, 1, 1], 1), ("oklab(-0.2 0.1 0.1)", [0, 0, 0], 1), ("oklab(0.6 0.5 -0.5)", oklab(.6, .5, -.5), 1)]),
    ("oklch", [("oklch(0.7 0.15 200)", oklch(.7, .15, 200), 1), ("oklch(70% 37.5% 200)", oklch(.7, .15, 200), 1), ("oklch(0.7 -0.1 200)", oklch(.7, 0, 0), 1)]),
    ("oklch-hue", [("oklch(0.6 0.1 0)", oklch(.6, .1, 0), 1), ("oklch(0.6 0.1 90)", oklch(.6, .1, 90), 1), ("oklch(0.6 0.1 180)", oklch(.6, .1, 180), 1), ("oklch(0.6 0.1 270)", oklch(.6, .1, 270), 1)]),
    ("oklab-legacy", ["!oklab(0.5, 0.1, 0.1)", "!oklch(0.5, 0.1, 30)", "!oklab(1 2)"]),
    ("oklch-powerless", [("oklch(0.6 0 123)", oklch(.6, 0, 0), 1), ("oklch(0 0.1 30)", [0, 0, 0], 1), ("oklch(1 0.1 30)", [1, 1, 1], 1)]),
    ("oklab-oklch", [("oklch(0.6 0.1414214 45)", oklab(.6, .1, .1), 1), ("oklab(0.6 0.1 0.1)", oklab(.6, .1, .1), 1), ("oklch(0.6 0.1 none)", oklab(.6, .1, 0), 1)]),
    ("color", [("color(srgb 20% 50% 80% / 0.4)", predefined("srgb", [.2, .5, .8]), .4), ("color(display-p3 0.5 none 0.2)", predefined("display-p3", [.5, 0, .2]), 1)]),
    ("color-invalid", ["!color(foo 1 2 3)", "!color(srgb)", "!color(srgb, 1, 0, 0)", "!color(srgb 1 0)"]),
    ("color-range", [("color(srgb 0.46 0.18 -0.2)", predefined("srgb", [.46, .18, -.2]), 1), ("color(srgb 0.5 1.2 0.5)", predefined("srgb", [.5, 1.2, .5]), 1)]),
    ("srgb", [("color(srgb 0.2 0.5 0.8)", predefined("srgb", [.2, .5, .8]), 1), ("color(srgb 0.02 0.03 0.04)", predefined("srgb", [.02, .03, .04]), 1)]),
    ("srgb-linear", [("color(srgb-linear 0.2 0.5 0.8)", predefined("srgb-linear", [.2, .5, .8]), 1), ("color(srgb-linear 0.002 0.003 0.004)", predefined("srgb-linear", [.002, .003, .004]), 1)]),
    ("display-p3", [("color(display-p3 0.5 0.6 0.2)", predefined("display-p3", [.5, .6, .2]), 1), ("color(display-p3 1 0 0)", predefined("display-p3", [1, 0, 0]), 1)]),
    ("display-p3-linear", [("color(display-p3-linear 0.5 0.6 0.2)", gamut(xyz_srgb(mm(M, [.5, .6, .2]))), 1)]),
    ("a98-rgb", [("color(a98-rgb 0.4 0.3 0.6)", predefined("a98-rgb", [.4, .3, .6]), 1)]),
    ("prophoto-rgb", [("color(prophoto-rgb 0.4 0.3 0.6)", predefined("prophoto-rgb", [.4, .3, .6]), 1), ("color(prophoto-rgb 0.02 0.01 0.03)", predefined("prophoto-rgb", [.02, .01, .03]), 1)]),
    ("rec2020", [("color(rec2020 0.4 0.3 0.6)", predefined("rec2020", [.4, .3, .6]), 1)]),
    ("xyz", [("color(xyz 0.3 0.4 0.2)", predefined("xyz", [.3, .4, .2]), 1), ("color(xyz-d65 0.2 0.3 0.4)", predefined("xyz", [.2, .3, .4]), 1), ("color(xyz-d50 0.3 0.4 0.2)", predefined("xyz-d50", [.3, .4, .2]), 1),
             ("color(xyz -0.2 -0.1 -0.2)", predefined("xyz", [-.2, -.1, -.2]), 1)]),
    ("gamut", [("color(display-p3 1 0 0)", predefined("display-p3", [1, 0, 0]), 1), ("oklch(0.9 0.4 30)", oklch(.9, .4, 30), 1), ("color(srgb 0.5 0.5 1.2)", predefined("srgb", [.5, .5, 1.2]), 1),
               ("lab(50 200 -200)", lab(50, 200, -200), 1)]),
    ("gamut-inside", [("color(display-p3 0.5 0.5 0.5)", predefined("display-p3", [.5, .5, .5]), 1), ("oklch(0.6 0.05 200)", oklch(.6, .05, 200), 1)]),
    ("gamut-lightness", [("oklch(1.2 0.1 30)", [1, 1, 1], 1), ("oklch(0.9999999 0.3 30)", oklch(.9999999, .3, 30), 1), ("oklch(-0.1 0.1 30)", [0, 0, 0], 1)]),
], 8, 8, 12, item)
unit.height = y + 1
unit.write()
