"""css-calc-edges: the edges of css-values-4's math functions."""
import math

from unitgen import Unit, rows

INF = "inf"
I = "calc(infinity * 1px)"
Z = "calc(-1px * 0)"
LIME = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAQAAAACCAYAAAB/qH1jAAAADklEQVR42mNg+I8G0QUABykP8Qyx+pcAAAAASUVORK5CYII="
unit = Unit("css-calc-edges", 120, 0, """Each lime bar fills its black outline,
and nothing red shows; a bar of a full cell is an infinity clamped to the
largest length. The edges of css-values-4's math functions as widths: NaN
in min(), max() and hypot(); 0- below 0+ and a literal -0 as 0; round() at
its argument ranges and line-width snapped to whole px, a zero multiple 0+
below and 0- above; mod() and rem() at theirs, a zero result of mod()
taking the step's sign; pow() and log() at theirs; tan() at its
asymptotes; 32 levels of parentheses or functions and no more; a
presentation attribute closed at its end, as CSS closes what is left open;
errors of syntax, arity and type; angles, times, frequencies and
resolutions in their canonical units. Below, lime fills its outline or
covers red, or the test runs on into the reference under it without a
step: opacity from a calc() needing the font, an angle or a length is
invalid; a number attribute takes no percentage; trailing text after an
integer's math function is invalid; a lazy negative stroke width is
clamped to none; a gradient in bounding box units takes calc() lengths and
percentages; a filter angle or marker orient from a percentage or with
text after it is invalid; url() closes at the end of its text, an escape
there U+FFFD, and undoes escapes, quoted or not, of hex digits to six, a
code point out of range or zero the replacement character, a newline
escaped in a style sheet nothing; a filter url() with more text in its
parentheses, an unquoted one with a quote in it, src() unquoted, a
percentage in blur(), a stray parenthesis in translate and a media
query's calc(0) for a length are invalid; a translate z from calc()
needing the font is 3D.""", root='font-size="16"')
C = 30


def bar(x, y, value):
    expression, want = value

    if want == INF:
        return f'<svg x="{x}" y="{y}" width="{C - 4}" height="3"><rect width="{expression}" height="3" fill="lime"/></svg>', f'<rect x="{x}" y="{y}" width="{C - 4}" height="3" fill="lime"/>', f'<rect x="{x}" y="{y}" width="{C - 4}" height="3"/>'

    if want is None:
        return f'<rect x="{x}" y="{y}" width="{expression}" height="3" fill="red"/>', "", ""

    return f'<rect x="{x}" y="{y}" width="{expression}" height="3" fill="lime"/>', f'<rect x="{x}" y="{y}" width="{round(want, 4)}" height="3" fill="lime"/>', f'<rect x="{x}" y="{y}" width="{round(want, 4)}" height="3"/>'


y = rows(unit, 2, [
    ("nan", [("min(NaN * 1px, 5px)", None), ("max(5px, NaN * 1px)", None), ("min(5px, NaN * 1px)", None), ("max(NaN * 1px, 5px)", None), ("hypot(NaN * 1px, 1px)", None)]),
    ("signed-zero", [(f"calc(1px / min(0px, {Z}) * -1)", INF), ("calc(1px / min(0px, 0px))", INF), (f"calc(1px / max({Z}, 0px))", INF), (f"calc(-1px / max({Z}, {Z}))", INF),
                     ("calc(1px / -0)", INF), ("calc(1px * -0)", None), (f"calc(1px / min({Z}, 0px))", None), (f"calc(1px / max(0px, {Z}))", INF)]),
    ("round", [("round(5px, 0px)", None), (f"round({I}, 5px)", INF), (f"round({I}, {I})", None), (f"round(up, 5px, {I})", INF),
               (f"round(line-width, 5px, {I})", INF), (f"calc(round(down, -5px, {I}) * -1)", INF), (f"calc(round(line-width, -5px, {I}) * -1)", INF), (f"calc(1px / round(up, 0px, {I}))", INF),
               (f"calc(1px / round(up, {Z}, {I}))", None), (f"calc(1px / round(down, {Z}, {I}))", None), (f"calc(1px / round(down, 0px, {I}))", INF), (f"calc(1px / round(5px, {I}))", INF),
               (f"calc(1px / round(-5px, {I}))", None), ("round(15px, 5px)", 15), ("round(to-zero, 14px, 5px)", 10), (f"calc(1px / round(up, -5px, {I}))", None),
               (f"calc(1px / round(down, 5px, {I}))", INF), ("round(NaN * 1px, 5px)", None), ("round(5px, NaN * 1px)", None), ("round(11px, 5px)", 10)]),
    ("line-width", [("round(line-width, 3px)", 3), ("calc(round(line-width, -0.3px) + 5px)", 4), ("round(line-width, 0.3px, 1px)", 1), ("calc(round(line-width, -0.3px, 1px) + 5px)", 4),
                    ("round(line-width, 2.6px, 1px)", 3), ("round(line-width, 2.3px, 1px)", 2)]),
    ("mod", [("mod(5px, 0px)", None), (f"mod({I}, 5px)", None), (f"mod(-5px, {I})", None), (f"mod(5px, {I})", 5),
             (f"calc(rem(-5px, {I}) * -1)", 5), ("calc(1px / mod(-10px, 5px))", INF), ("calc(1px / mod(10px, -5px))", None), ("mod(NaN * 1px, 5px)", None),
             ("mod(5px, NaN * 1px)", None)]),
    ("pow-log", [("calc(pow(NaN, 0) * 1px + 5px)", None), ("calc(pow(1, infinity) * 1px + 5px)", None), ("calc(pow(-1, -infinity) * 1px + 5px)", None), ("calc(log(-1) * 1px + 5px)", None),
                 ("calc(log(8, 1) * 1px + 5px)", None), ("calc(log(8, 0) * 1px + 5px)", None), ("calc(log(0) * -1px)", INF), ("calc(log(1) * 1px + 5px)", 5),
                 ("calc(log(infinity) * 1px)", INF), ("calc(log(NaN) * 1px + 5px)", None), ("calc(pow(2, NaN) * 1px + 5px)", None), ("calc(pow(2, infinity) * 1px)", INF),
                 ("calc(log(2, NaN) * 1px + 5px)", None)]),
    ("tan", [("calc(tan(-270deg) * 1px)", INF), ("calc(tan(-90deg) * -1px)", INF), ("calc(tan(270deg) * -1px)", INF), ("calc(tan(pi / 4) * 10px)", 10)]),
    ("depth", [("calc(" + "(" * 31 + "5px" + ")" * 31 + ")", 5), ("calc(" + "(" * 32 + "5px" + ")" * 32 + ")", None), ("calc(" * 32 + "5px" + ")" * 32, 5), ("calc(" * 33 + "5px" + ")" * 33, None)]),
    ("closing", [("calc((1px)", 1), ("calc(5", 5), ("calc(5px", 5)]),
    ("syntax", [("calc(5-3px)", None), ("calc(foo)", None), ("calc(2 * )", None), ("calc(1px + )", None),
                ("foo(5px)", None), ("calc(foo(5px))", None), ("calc((foo))", None), ("calc((1px 2px))", None),
                ("calc(1foo)", None), ("calc(1px, 2px)", None), ("min(1px 2px)", None), ("round(up 5px, 1px)", None),
                ("clamp(none 1px, 2px, 3px)", None), ("0e999", None), ("calc(1px+2px + 3px)", None), ("calc(1px 2px 3px)", None),
                ("calc(1px +(2px))", None), ("calc(1px + foo)", None)]),
    ("arity", [("clamp(1px, none, 3px)", None), ("clamp(1px, 2px, 3px, 4px)", None), ("clamp(1px, 2px)", None), ("round(1px, 2px, 3px)", None),
               ("mod(1px)", None), ("calc(log(1, 2, 3) * 1px)", None), ("calc(pow(2) * 1px)", None), ("calc(sin(1, 2) * 1px)", None),
               ("calc(sqrt(4, 9) * 1px)", None), ("round(up)", None)]),
    ("type", [("clamp(1deg, 2px, 3px)", None), ("clamp(1px, 2px, 3deg)", None), ("round(1px, 1deg)", None), ("round(line-width, 5)", None),
              ("mod(1px, 1deg)", None), ("calc(log(2px, 2px) * 1px)", None), ("calc(sqrt(4px) * 1px)", None), ("min(5px, 3s)", None),
              ("calc(1deg)", None), ("12sem", None), ("calc(sign(3px) * 3px)", 3)]),
    ("canonical", [("calc(10grad / 1deg * 1px)", 9), ("calc(1rad / 1deg * 0.1px)", 18 / math.pi), ("calc(1khz / 100hz * 1px)", 10), ("calc(96dpi / 1dppx * 10px)", 10),
                   ("calc(96dpcm / 2.54dppx * 10px)", 10), ("calc(1x / 1dppx * 10px)", 10)]),
], 4, C, 5, bar)
y0 = y + 2
unit.common('<linearGradient id="g"><stop stop-color="lime"/></linearGradient><linearGradient id="g&#xFFFD;"><stop stop-color="lime"/></linearGradient>'
            '<linearGradient id="b2" x2=".5"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="navy"/></linearGradient>')


def small(name, x, y, test, plain, outline=None, h=7):
    unit.cell(name, (x - 1, y - 1, 9, h + 2), test, plain, plain.replace(' fill="lime"', "") if outline is None else outline)


def covered(name, x, y, test, h=7):
    """A test drawn over red that should be lime."""
    small(name, x, y, f'<rect x="{x}" y="{y}" width="7" height="{h}" fill="red"/>' + test, f'<rect x="{x}" y="{y}" width="7" height="{h}" fill="lime"/>', h=h)


def plain(x, y, h=7):
    return f'<rect x="{x}" y="{y}" width="7" height="{h}" fill="lime"/>'


def at(k):
    return 2 + k * 9


y = y0
for k, o in enumerate(["calc(1em / 32px)", "calc(1deg)", "calc(50% + 1px)", "calc(1px)"]):
    covered(f"opacity-{['font', 'angle', 'percent-length', 'length'][k]}", at(k), y, f'<rect x="{at(k)}" y="{y}" width="7" height="7" fill="lime" style="opacity: {o}"/>')
x = at(4)
small("slope-percent", x, y, f'<filter id="s"><feComponentTransfer><feFuncG type="linear" slope="calc(50%)"/></feComponentTransfer></filter><rect x="{x}" y="{y}" width="7" height="3.5" fill="lime" filter="url(#s)"/><rect x="{x}" y="{y + 3.5}" width="7" height="3.5" fill="lime"/>',
      f'<rect x="{x}" y="{y}" width="7" height="3.5" fill="lime"/><rect x="{x}" y="{y + 3.5}" width="7" height="3.5" fill="lime"/>', "")
turbulence = '<filter id="{0}" filterUnits="userSpaceOnUse" x="{1}" y="{2}" width="7" height="3.5"><feTurbulence baseFrequency=".3" numOctaves="{3}" seed="5"/></filter><rect x="{1}" y="{2}" width="7" height="3.5" filter="url(#{0})"/>'
for k, (name, octaves) in enumerate([("integer-trailing", "calc(2)x"), ("integer-percent", "calc(50%)")]):
    x = at(5 + k)
    small(name, x, y, turbulence.format(f"t{2 * k + 1}", x, y, octaves) + turbulence.format(f"t{2 * k + 2}", x, y + 3.5, 1), turbulence.format(f"t{2 * k + 1}", x, y, 1) + turbulence.format(f"t{2 * k + 2}", x, y + 3.5, 1), "")
x = at(7)
small("lazy-stroke", x, y, f'<rect x="{x}" y="{y}" width="7" height="7" fill="lime" stroke="red" style="stroke-width: calc(-1em)"/>', plain(x, y))
x = at(8)
small("gradient-percent", x, y, f'<linearGradient id="b1" x2="calc(50%)"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="navy"/></linearGradient><rect x="{x}" y="{y}" width="7" height="3.5" fill="url(#b1)"/><rect x="{x}" y="{y + 3.5}" width="7" height="3.5" fill="url(#b2)"/>',
      f'<rect x="{x}" y="{y}" width="7" height="3.5" fill="url(#b2)"/><rect x="{x}" y="{y + 3.5}" width="7" height="3.5" fill="url(#b2)"/>', "")
x = at(9)
small("gradient-font", x, y, f'<linearGradient id="b3" x2="calc(1em / 32)"><stop offset=".5" stop-color="lime"/><stop offset=".5" stop-color="navy"/></linearGradient><rect x="{x}" y="{y}" width="7" height="3.5" fill="url(#b3)"/><rect x="{x}" y="{y + 3.5}" width="7" height="3.5" fill="url(#b2)"/>',
      f'<rect x="{x}" y="{y}" width="7" height="3.5" fill="url(#b2)"/><rect x="{x}" y="{y + 3.5}" width="7" height="3.5" fill="url(#b2)"/>', "")

y = y0 + 10
for k, f in enumerate(["hue-rotate(foo)", "hue-rotate(calc(50%))", "hue-rotate(calc(1deg) 2)"]):
    small(f"filter-angle-{k + 1}", at(k), y, f'<rect x="{at(k)}" y="{y}" width="7" height="7" fill="lime" style="filter: {f} invert(1)"/>', plain(at(k), y))
for k, o in enumerate(["foo", "calc(50%)", "calc(1deg)x", "calc(1px)"]):
    x = at(3 + k)
    small(f"orient-{k + 1}", x, y, f'<marker id="m{k}" markerWidth="6" markerHeight="2" refX="0" refY="1" orient="{o}" markerUnits="userSpaceOnUse"><rect width="6" height="2" fill="lime"/></marker><path d="M{x} {y + 1}h1" marker-start="url(#m{k})"/>',
          f'<rect x="{x}" y="{y}" width="6" height="2" fill="lime"/>')
for k, f in enumerate(["url(#g", "url('#g", "url(#g\\", "url('#g' x) red"]):
    x = at(7 + k)
    small(f"url-close-{k + 1}", x, y, f'<g fill="lime"><rect x="{x}" y="{y}" width="7" height="7" fill="{f}"/></g>', plain(x, y))
covered("translate-time", at(11), y, f'<rect x="{at(11)}" y="{y}" width="7" height="7" fill="lime" style="transform: translate(calc(1s))"/>')
small("translate-open", at(12), y, f'<rect x="{at(12)}" y="{y}" width="7" height="7" fill="lime" style="transform: translate(calc(1px +))"/>', plain(at(12), y))
covered("colour-length", at(13), y, f'<g fill="lime"><rect x="{at(13)}" y="{y}" width="7" height="7" fill="rgb(calc(1px) 0 0)"/></g>')
unit.cell("attribute-close", (1, y + 8, 30, 4), f'<rect x="2" y="{y + 9}" height="3" fill="red" width="calc(1px +"/>', "")

y = y0 + 20
for k, f in enumerate(["url('#\\67')", "url('#g\\", "url(#\\000067)", "url(#\\67", "url(#\\0 ) lime", "url(#\\110000 ) lime", "url(#\\d800 ) lime", "url(#\\e000 ) lime"]):
    covered(f"url-escape-{k + 1}", at(k), y, f'<g fill="lime"><rect x="{at(k)}" y="{y}" width="7" height="4" fill="{f}"/></g>', 4)
covered("url-quote", at(8), y, f'<g fill="lime"><rect x="{at(8)}" y="{y}" width="7" height="4" fill=\'url(#g"x)\'/></g>', 4)
covered("url-newline", at(9), y, f'<style>.nl {{ fill: url("#\\\ng") }}</style><rect class="nl" x="{at(9)}" y="{y}" width="7" height="4"/>', 4)
small("url-paren", at(10), y, f'<rect x="{at(10)}" y="{y}" width="7" height="4" fill="lime" style="filter: url(\'(\')) invert(1)"/>', plain(at(10), y, 4), h=4)

y = y0 + 26
for k, (name, f) in enumerate([("src-unquoted", 'style="filter: src(#f) invert(1)"'), ("blur-percent", 'style="filter: blur(calc(10% + 1px)) invert(1)"'), ("translate-paren", 'style="translate: 5px)"'),
                               ("translate-z", 'style="translate: 0px 0px calc(1em - 16px)"'), ("url-escape-space", 'fill="url(#\\67x ) lime"')]):
    covered(name, at(k), y, f'<g fill="lime"><rect x="{at(k)}" y="{y}" width="7" height="4" {f}/></g>', 4)
small("media-zero", at(5), y, f'<style>@media (min-width: calc(0)) {{ .mq0 {{ fill: red }} }}</style><rect class="mq0" x="{at(5)}" y="{y}" width="7" height="4" fill="lime"/>', plain(at(5), y, 4), h=4)
unit.common('<path d="M0 0" pathLength="calc(5"/><path d="M0 0" pathLength="calc(5px"/><path d="M0 0" pathLength="calc(1 +"/>')
unit.height = y0 + 32
unit.write()
