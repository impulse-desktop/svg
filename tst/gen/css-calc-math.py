"""css-calc-math: calc() and the math functions of css-values-4 as widths."""
import math

from unitgen import Unit, rows

INF = "inf"
unit = Unit("css-calc-math", 120, 0, """Each lime bar fills its black outline,
and nothing red shows. calc() and the math functions of css-values-4 as
widths: * and / before + and -, white space on both sides of + and -,
typed arithmetic with lengths squared on the way, a number and a length
never added, min(), max(), clamp() with none and its minimum winning,
round() by every strategy, mod() and rem() by sign, trigonometric,
exponential and sign functions, e and pi, infinity and NaN, IEEE results
inside a calculation and censored at its top: NaN to 0, an infinity to the
largest length, which fills its cell, a negative width clamped to 0. Below,
a negative rx from calc() is clamped to 0 where a plain one is invalid
(auto), a negative stroke-width from calc() draws no stroke, an invalid
style falls back to the attribute, opacity calc(.25 + 25%) is invalid and
calc(100% / 4) a quarter, drawn as the reference under it.""", root='font-size="8"')
C = 30


def bar(x, y, value):
    expression, want = value

    if want == INF:
        return f'<svg x="{x}" y="{y}" width="{C - 4}" height="3"><rect width="{expression}" height="3" fill="lime"/></svg>', f'<rect x="{x}" y="{y}" width="{C - 4}" height="3" fill="lime"/>', f'<rect x="{x}" y="{y}" width="{C - 4}" height="3"/>'

    if want is None:
        return f'<rect x="{x}" y="{y}" width="{expression}" height="3" fill="red"/>', "", ""

    return f'<rect x="{x}" y="{y}" width="{expression}" height="3" fill="lime"/>', f'<rect x="{x}" y="{y}" width="{round(want, 4)}" height="3" fill="lime"/>', f'<rect x="{x}" y="{y}" width="{round(want, 4)}" height="3"/>'


y = rows(unit, 2, [
    ("precedence", [("calc(2px + 3px * 2)", 8), ("calc((2px + 3px) * 2)", 10), ("calc(24px / 4 / 2)", 3), ("calc(20px - 6px - 4px)", 10), ("calc(2 * (1px + (2px * (1 + 1))))", 10)]),
    ("spaces", [("calc(2*3px)", 6), ("calc(-2px + 9px)", 7), ("calc(4px+2px)", None), ("calc(4px -2px)", None)]),
    ("types", [("calc(1px * 2px / 1px * 5)", 10), ("CALC(3PX * 4)", 12), ("calc(5)", 5), ("calc(1em + 2px)", 10), ("calc(1in / 12)", 8), ("calc(1px * 1s / 1ms / 100)", 10)]),
    ("mismatch", [("calc(5px + 1deg)", None), ("min(5px, 3)", None)]),
    ("zero", [("calc(0 + 5px)", None), ("calc(0px + 5px)", 5)]),
    ("percent", [("calc(25% - 10px)", 20)]),
    ("minmax", [("min(9px, 4px, 6px)", 4), ("max(9px, 4px, 6px)", 9), ("min(10px, 2em)", 10), ("max(5px)", 5)]),
    ("clamp", [("clamp(4px, 20px, 12px)", 12), ("clamp(12px, 2px, 20px)", 12), ("clamp(16px, 8px, 10px)", 16)]),
    ("clamp-none", [("clamp(none, 20px, 8px)", 8), ("clamp(6px, 2px, none)", 6)]),
    ("round", [("round(13px, 5px)", 15), ("round(12.5px, 5px)", 15), ("calc(round(-12.5px, 5px) + 20px)", 10), ("calc(round(7.5) * 1px)", 8)]),
    ("round-strategies", [("round(up, 11px, 5px)", 15), ("round(down, 14px, 5px)", 10), ("calc(round(to-zero, -14px, 5px) + 20px)", 10), ("round(7.5px)", None)]),
    ("round-line-width", [("round(line-width, 0.3px)", 1), ("round(line-width, 2.7px)", 2)]),
    ("mod-rem", [("calc(mod(18px, 5px) * 3)", 9), ("calc(mod(-18px, 5px) * 3)", 6), ("calc(rem(-18px, 5px) * -3)", 9), ("calc(mod(140, -90) * -0.25px)", 10), ("calc(rem(140, -90) * 0.2px)", 10)]),
    ("trig", [("calc(sin(30deg) * 20px)", 10), ("calc(cos(60deg) * 20px)", 10), ("calc(tan(45deg) * 10px)", 10), ("calc(sin(pi / 6) * 20px)", 10), ("calc(tan(90deg) * 1px)", INF), ("calc(sin(1px) * 1px)", None)]),
    ("inverse", [("calc(asin(0.5) / 3deg * 1px)", 10), ("calc(acos(0.5) / 6deg * 1px)", 10), ("calc(atan(1) / 4.5deg * 1px)", 10), ("calc(asin(2) * 0px + 7px)", None)]),
    ("atan2", [("calc(atan2(1px, -1px) / 13.5deg * 1px)", 10), ("calc(atan2(-1, -1) / -13.5deg * 1px)", 10)]),
    ("exponential", [("calc(pow(2, 3) * 1px)", 8), ("calc(sqrt(100) * 1px)", 10), ("hypot(6px, 8px)", 10), ("hypot(-10px)", 10), ("calc(log(1000, 10) * 3px)", 9), ("calc(log(e) * 7px)", 7), ("calc(exp(0) * 7px)", 7), ("calc(pow(-8, 1 / 3) * 1px + 5px)", None), ("pow(2px, 2)", None)]),
    ("sign", [("abs(-7px)", 7), ("calc(sign(-2px) * -6px)", 6), ("calc(sign(0px) * 5px + 9px)", 9)]),
    ("constants", [("calc(pi * 2px)", 2 * math.pi), ("calc(e * 4px)", 4 * math.e), ("calc(pi)", math.pi)]),
    ("infinity", [("calc(InFiNiTy * 1px)", INF), ("calc(-infinity * 1px)", None), ("calc(NaN * 1px)", None)]),
    ("ieee", [("calc(1px / 0)", INF), ("calc(1px / (5 * 0))", INF), ("calc(1px / calc(-5 * 0))", None), ("calc(0 / 0 * 1px)", None)]),
    ("nested", [("min(calc(1px * infinity), 12px)", 12), ("calc(max(-10px, -20px) + 30px)", 20)]),
    ("range", [("calc(-10px)", None)]),
], 4, C, 5, bar)
y += 1


def square(x, test, plain, outline=None):
    return test.format(x=x, y=y), plain.format(x=x, y=y), (plain if outline is None else outline).format(x=x, y=y).replace(' fill="lime"', "")


for name, x, (test, plain, outline) in [
    ("negative-rx", 2, ('<rect x="{x}" y="{y}" width="12" height="12" rx="calc(-5px)" ry="6" fill="lime"/>', '<rect x="{x}" y="{y}" width="12" height="12" fill="lime"/>', None)),
    ("negative-rx-plain", 18, ('<rect x="{x}" y="{y}" width="12" height="12" rx="-5" ry="6" fill="lime"/>', '<rect x="{x}" y="{y}" width="12" height="12" rx="6" fill="lime"/>', None)),
    ("negative-stroke", 34, ('<rect x="{x}" y="{y}" width="12" height="12" fill="lime" stroke="red" style="stroke-width: calc(-2px)"/>', '<rect x="{x}" y="{y}" width="12" height="12" fill="lime"/>', None)),
    ("invalid-style", 50, ('<rect x="{x}" y="{y}" width="20" height="12" fill="lime" style="width: calc(1px+1px)"/>', '<rect x="{x}" y="{y}" width="20" height="12" fill="lime"/>', None)),
    ("opacity-mix", 74, ('<rect x="{x}" y="{y}" width="12" height="12" fill="red"/><rect x="{x}" y="{y}" width="12" height="12" fill="lime" style="opacity: calc(.25 + 25%)"/>', '<rect x="{x}" y="{y}" width="12" height="12" fill="lime"/>', None)),
    ("opacity-percent", 90, ('<rect x="{x}" y="{y}" width="12" height="12" fill="navy" style="opacity: calc(100% / 4)"/>', '<rect x="{x}" y="{y}" width="12" height="12" fill="navy" opacity=".25"/>', "")),
]:
    unit.cell(name, (x - 1, y - 1, 16, 14), *square(x, test, plain, outline))

unit.height = y + 14
unit.write()
