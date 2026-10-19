"""css-calc-contexts: math functions wherever a number, a length, a
percentage or an angle goes."""
from unitgen import Unit

LIME4x2 = "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAQAAAACCAYAAAB/qH1jAAAADklEQVR42mNg+I8G0QUABykP8Qyx+pcAAAAASUVORK5CYII="
unit = Unit("css-calc-contexts", 120, 72, """Math functions wherever a number,
a length, a percentage or an angle goes. In the first and last rows lime
fills its black outline: transform functions, translate, scale (a
percentage a number there), font-size, viewBox and points numbers, a
percentage and a vw resolved at drawing, a basic shape and a shape(),
object-position with spaces inside calc(), a marker's orient, a CSS
gradient's angle, a media query, but none in a transform attribute, whose
grammar is SVG's, and a number from one is no unitless zero, so a
transform, filter or gradient with calc(0) for an angle or a length is
invalid; nothing red shows. In the middle rows each test runs on into the
reference under it without a step: colour channels and hue, fill-opacity,
a stop offset, dashes, a stroke width, filter functions, a number list,
numOctaves rounded to an integer, a transfer slope.""", root='font-size="8"')


def pair(x, y, test, reference, w=12):
    """A test that runs on into its reference under it: the expected
    drawing has the reference in both places."""
    return f'<rect x="{x}" y="{y}" width="{w}" height="6" {test}/><rect x="{x}" y="{y + 6}" width="{w}" height="6" {reference}/>', f'<rect x="{x}" y="{y}" width="{w}" height="6" {reference}/><rect x="{x}" y="{y + 6}" width="{w}" height="6" {reference}/>'


def cell(name, box, test, plain, outline=""):
    unit.cell(name, box, test, plain, outline)


# Row 1, transforms: lime fills its outline.
for name, box, test, outline in [
    ("translate", (1, 1, 20, 9), '<rect width="10" height="6" fill="lime" style="transform: translate(calc(1px + 1em), calc(1px * 2))"/>', '<rect x="9" y="2" width="10" height="6"/>'),
    ("rotate", (22, 1, 16, 16), '<rect x="24" y="8" width="12" height="4" fill="lime" style="transform-box: fill-box; transform-origin: center; transform: rotate(calc(180deg / 2))"/>', '<rect x="28" y="4" width="4" height="12"/>'),
    ("translate-percent", (39, 1, 13, 8), '<rect x="40" y="2" width="8" height="6" fill="lime" style="transform-box: fill-box; transform: translate(calc(50% - 2px), 0)"/>', '<rect x="42" y="2" width="8" height="6"/>'),
    ("translate-property", (53, 1, 13, 11), '<rect x="54" y="2" width="8" height="6" fill="lime" style="translate: calc(1px + 1px) calc(3px * 1)"/>', '<rect x="56" y="5" width="8" height="6"/>'),
    ("scale-property", (67, 1, 12, 8), '<rect x="68" y="2" width="16" height="12" fill="lime" style="transform-box: fill-box; transform-origin: 0 0; scale: calc(1 / 2)"/>', '<rect x="68" y="2" width="8" height="6"/>'),
    ("scale-percent", (79, 1, 11, 8), '<rect x="160" y="4" width="16" height="12" fill="lime" style="transform: scale(calc(50%))"/>', '<rect x="80" y="2" width="8" height="6"/>'),
    ("font-size", (91, 1, 15, 8), '<rect x="92" y="2" width="12" height="6" fill="lime" style="font-size: calc(4px * 2); width: calc(1em + 4px)"/>', '<rect x="92" y="2" width="12" height="6"/>'),
]:
    cell(name, box, test, outline.replace("/>", ' fill="lime"/>'), outline)

# Row 2, paint: each test runs on into the reference under it.
y = 18
unit.common('<linearGradient id="gt" gradientUnits="userSpaceOnUse" x1="58" x2="70"><stop offset="0" stop-color="navy"/><stop offset="calc(25%)" stop-color="lime"/><stop offset="1" stop-color="navy"/></linearGradient>'
            '<linearGradient id="gr" gradientUnits="userSpaceOnUse" x1="58" x2="70"><stop offset="0" stop-color="navy"/><stop offset=".25" stop-color="lime"/><stop offset="1" stop-color="navy"/></linearGradient>')
for name, x, test, reference in [
    ("rgb", 2, 'fill="rgb(calc(255 / 5) calc(100% / 2) 0)"', 'fill="rgb(51 127.5 0)"'),
    ("hsl", 16, 'fill="hsl(calc(60deg * 2) calc(50% + 50%) 25%)"', 'fill="hsl(120 100% 25%)"'),
    ("lab", 30, 'fill="lab(calc(50 + 10) calc(-20 * 2) 30)"', 'fill="lab(60 -40 30)"'),
    ("fill-opacity", 44, 'fill-opacity="calc(1 / 4)"', 'fill-opacity=".25"'),
    ("stop-offset", 58, 'fill="url(#gt)"', 'fill="url(#gr)"'),
]:
    cell(name, (x - 1, y - 1, 14, 14), *pair(x, y, test, reference))
dash = '<path d="M72 {0}h16" stroke="navy" stroke-width="6" stroke-dasharray="{1}"/>'
cell("dasharray", (71, y - 1, 18, 14), dash.format(y + 3, "calc(2px * 2) calc(1px + 1px)") + dash.format(y + 9, "4 2"), dash.format(y + 3, "4 2") + dash.format(y + 9, "4 2"))
cell("stroke-width", (91, y - 1, 14, 5), f'<path d="M92 {y + 1.5}h12" stroke="lime" stroke-width="calc(1px + 2px)"/>', f'<rect x="92" y="{y}" width="12" height="3" fill="lime"/>', f'<rect x="92" y="{y}" width="12" height="3"/>')

# Row 3, filters: each test runs on into the reference under it.
y = 34
cell("hue-rotate", (1, y - 1, 14, 14), *pair(2, y, 'fill="#c84" style="filter: hue-rotate(calc(90deg * 2))"', 'fill="#c84" style="filter: hue-rotate(180deg)"'))
cell("opacity-function", (15, y - 1, 14, 14), *pair(16, y, 'fill="navy" style="filter: opacity(calc(50%))"', 'fill="navy" style="filter: opacity(.5)"'))
blur = '<filter id="{0}" x="0" y="-1" width="1" height="3"><feGaussianBlur stdDeviation="0 {1}"/></filter>'
unit.common(blur.format("br", 2))
cell("number-list", (29, y - 1, 14, 14), blur.format("bt", "calc(1 + 1)") + f'<rect x="30" y="{y + 4}" width="6" height="4" fill="navy" filter="url(#bt)"/><rect x="36" y="{y + 4}" width="6" height="4" fill="navy" filter="url(#br)"/>',
     f'<rect x="30" y="{y + 4}" width="6" height="4" fill="navy" filter="url(#br)"/><rect x="36" y="{y + 4}" width="6" height="4" fill="navy" filter="url(#br)"/>')
turbulence = '<filter id="{0}" filterUnits="userSpaceOnUse" x="44" y="{1}" width="12" height="6"><feTurbulence baseFrequency=".2" numOctaves="{2}" seed="3"/></filter><rect x="44" y="{1}" width="12" height="6" filter="url(#{0})"/>'
cell("integer", (43, y - 1, 14, 14), turbulence.format("tt", y, "calc(1.6)") + turbulence.format("tr", y + 6, 2), turbulence.format("tt", y, 2) + turbulence.format("tr", y + 6, 2))
unit.common('<filter id="cr"><feComponentTransfer><feFuncG type="linear" slope=".5"/></feComponentTransfer></filter>')
cell("slope", (57, y - 1, 14, 14), '<filter id="ct"><feComponentTransfer><feFuncG type="linear" slope="calc(1 / 2)"/></feComponentTransfer></filter>' + pair(58, y, 'fill="lime" filter="url(#ct)"', 'fill="lime" filter="url(#cr)"')[0], pair(58, y, 'fill="lime" filter="url(#cr)"', 'fill="lime" filter="url(#cr)"')[0])

# Row 4, geometry: lime fills its outline.
y = 50
for name, box, test, outline in [
    ("viewbox", (1, y - 1, 14, 14), f'<svg x="2" y="{y}" width="12" height="12" viewBox="0 0 calc(12 * 2) 24"><rect width="12" height="12" fill="lime"/></svg>', f'<rect x="2" y="{y}" width="6" height="6"/>'),
    ("points", (15, y - 1, 14, 14), f'<polygon points="16,{y} calc(6 * 2 + 16),{y} 16,{y + 12}" fill="lime"/>', f'<polygon points="16,{y} 28,{y} 16,{y + 12}"/>'),
    ("length-percent", (29, y - 1, 14, 14), f'<svg x="30" y="{y}" width="24" height="12"><rect x="calc(50% - 6px)" width="6" height="6" fill="lime"/></svg>', f'<rect x="36" y="{y}" width="6" height="6"/>'),
    ("viewport-unit", (43, y - 1, 8, 14), f'<rect x="44" y="{y}" width="calc(5vw - 1px)" height="6" fill="lime"/>', f'<rect x="44" y="{y}" width="5" height="6"/>'),
    ("basic-shape", (51, y - 1, 14, 14), f'<rect x="52" y="{y}" width="12" height="12" fill="lime" style="clip-path: circle(calc(3px * 2) at calc(50%) 50%)"/>', f'<circle cx="58" cy="{y + 6}" r="6"/>'),
    ("object-position", (65, y - 1, 14, 14), f'<image x="66" y="{y}" width="12" height="12" style="object-fit: none; object-position: calc(50% + 2px) 0" href="{LIME4x2}"/>', f'<rect x="72" y="{y}" width="4" height="2"/>'),
    ("orient", (79, y - 1, 12, 14), f'<marker id="mk" markerWidth="6" markerHeight="2" refX="0" refY="1" orient="calc(45deg * 2)" markerUnits="userSpaceOnUse"><rect width="6" height="2" fill="lime"/></marker><path d="M85 {y}h6" marker-start="url(#mk)"/>', f'<rect x="84" y="{y}" width="2" height="6"/>'),
    ("gradient-angle", (91, y - 1, 14, 14), f'<rect x="92" y="{y}" width="12" height="12" fill="lime" style="mask-image: linear-gradient(calc(45deg * 2), black 50%, transparent 50%)"/>', f'<rect x="92" y="{y}" width="6" height="12"/>'),
]:
    cell(name, box, test, outline.replace("/>", ' fill="lime"/>'), outline)

# Row 5: lime fills its outline.
y = 66
for name, box, test, outline in [
    ("media", (1, y - 1, 14, 6), f'<style>@media (min-width: calc(10px * 2)) {{ .mq {{ fill: lime }} }}</style><rect class="mq" x="2" y="{y}" width="12" height="4" fill="red"/>', f'<rect x="2" y="{y}" width="12" height="4"/>'),
    ("transform-attribute", (15, y - 1, 14, 6), f'<rect x="16" y="{y}" width="12" height="4" fill="lime" transform="translate(calc(50px))"/>', f'<rect x="16" y="{y}" width="12" height="4"/>'),
    ("zero-rotate", (29, y - 1, 14, 6), f'<rect x="30" y="{y}" width="12" height="4" fill="lime" style="transform: translate(50px) rotate(calc(0))"/>', f'<rect x="30" y="{y}" width="12" height="4"/>'),
    ("zero-hue-rotate", (43, y - 1, 14, 6), f'<rect x="44" y="{y}" width="12" height="4" fill="lime" style="filter: hue-rotate(calc(0)) invert(1)"/>', f'<rect x="44" y="{y}" width="12" height="4"/>'),
    ("zero-blur", (57, y - 1, 14, 6), f'<rect x="58" y="{y}" width="12" height="4" fill="lime" style="filter: blur(calc(0)) invert(1)"/>', f'<rect x="58" y="{y}" width="12" height="4"/>'),
    ("zero-gradient", (71, y - 1, 14, 6), f'<rect x="72" y="{y}" width="12" height="4" fill="lime" style="mask-image: linear-gradient(calc(0), black 50%, transparent 50%)"/>', f'<rect x="72" y="{y}" width="12" height="4"/>'),
    ("shape", (85, y - 1, 14, 6), f'<rect x="86" y="{y}" width="12" height="4" fill="lime" style="clip-path: shape(from calc(0% + 0px) 0, line to calc(50% + 6px) 0, line to 0 100%, close)"/>', f'<polygon points="86,{y} 98,{y} 86,{y + 4}"/>'),
]:
    cell(name, box, test, outline.replace("/>", ' fill="lime"/>'), outline)

unit.write()
