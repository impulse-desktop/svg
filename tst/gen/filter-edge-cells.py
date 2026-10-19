"""filter-edge-cells: subregions that abut, kernelUnitLength and the blur's
edgeMode, of CSS Filter Effects."""
import math

from unitgen import Unit, png

unit = Unit("filter-edge-cells", 100, 52, """Each lime drawing fills its black
outline, and nothing red shows. Two floods at half opacity in subregions
meeting at a fraction of a pixel, added, tile without a gap or an overlap:
the pixel they share takes its part from each (D2). feConvolveMatrix's kernel shifting
by two kernel units moves by two kernelUnitLengths in user units, dy dx
when not given, and one device pixel when the length is zero or negative;
the lighting primitives fall back the same. feGaussianBlur's edgeMode
duplicate, wrap and mirror keep a solid square solid to its edges, none,
the initial value, fades them, as a true Gaussian within a leeway. An
arithmetic result, named, is read as any other: doubled to opaque and
shifted within its subregion. A lighting primitive's own y and height cut
its flat, fully lit result.""")
F = 'filterUnits="userSpaceOnUse" primitiveUnits="userSpaceOnUse"'


def lime(x, y, w, h):
    return f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="lime"/>'


tests, plains = [], []
for k, (a, b) in enumerate([(5.5, 6.5), (3.25, 8.75)]):
    x = 2 + 14 * k
    tests.append(f'<filter id="ab{k}" {F} x="{x}" y="2" width="12" height="8"><feFlood flood-color="lime" flood-opacity=".5" x="{x}" y="2" width="{a}" height="8" result="a"/>'
                 f'<feFlood flood-color="lime" flood-opacity=".5" x="{x + a}" y="2" width="{b}" height="8" result="b"/><feComposite in="a" in2="b" operator="arithmetic" k2="1" k3="1"/></filter>'
                 f'<rect x="{x}" y="2" width="12" height="8" fill="red" filter="url(#ab{k})"/>')
    plains.append(lime(x, 2, 12, 8).replace("/>", ' fill-opacity=".5"/>'))
unit.cell("abut", (1, 1, 28, 10), "".join(tests), "".join(plains), "".join(plains).replace(' fill="lime" fill-opacity=".5"', ""))
SHIFT = 'order="3" kernelMatrix="0 0 0 0 0 0 0 0 1" targetX="2" targetY="2" edgeMode="none"'
tests, plains = [], []
for k, (length, dx, dy) in enumerate([("2", 4, 4), ("3 2", 6, 4), ("0", 2, 2), ("-1", 2, 2)]):
    x = 34 + 16 * k
    tests.append(f'<filter id="ku{k}" {F} x="{x}" y="2" width="14" height="14"><feConvolveMatrix {SHIFT} kernelUnitLength="{length}"/></filter>'
                 f'<rect x="{x}" y="2" width="4" height="4" fill="lime" filter="url(#ku{k})"/>')
    plains.append(lime(x + dx, 2 + dy, 4, 4))
unit.cell("kernel-unit-length", (33, 1, 64, 16), "".join(tests), "".join(plains), "".join(plains).replace(' fill="lime"', ""))
LIGHT = '<feDiffuseLighting in="b" surfaceScale="4" lighting-color="lime"%s><feDistantLight azimuth="45" elevation="40"/></feDiffuseLighting>'
tests, plains = [], []
for k, length in enumerate(["0", "-1"]):
    x = 2 + 16 * k
    light = f'<filter id="{{0}}{k}" {F} x="{x}" y="20" width="14" height="14"><feGaussianBlur stdDeviation="2" result="b"/>{{1}}</filter><rect x="{x + 3}" y="23" width="8" height="8" fill="navy" filter="url(#{{0}}{k})"/>'
    tests.append(light.format("lk", LIGHT % f' kernelUnitLength="{length}"'))
    plains.append(light.format("ld", LIGHT % ""))
unit.cell("lighting-unit-length", (1, 19, 32, 16), "".join(tests), "".join(plains))


def faded(s):
    def share(c):
        return (math.erf((10 - c) / (s * math.sqrt(2))) - math.erf((0 - c) / (s * math.sqrt(2)))) / 2

    return png([[(0, 1, 0, share(i + .5) * share(j + .5)) for i in range(10)] for j in range(10)])


tests, plains = [], []
for k, mode in enumerate(["duplicate", "wrap", "mirror"]):
    x = 36 + 12 * k
    tests.append(f'<filter id="em{k}" x="0" y="0" width="1" height="1"><feGaussianBlur stdDeviation="2" edgeMode="{mode}"/></filter><rect x="{x}" y="22" width="10" height="10" fill="lime" filter="url(#em{k})"/>')
    plains.append(lime(x, 22, 10, 10))
unit.cell("blur-edge-solid", (35, 21, 36, 12), "".join(tests), "".join(plains), "".join(plains).replace(' fill="lime"', ""))
tests, plains = [], []
for k, mode in enumerate([' edgeMode="none"', ""]):
    x = 72 + 12 * k
    tests.append(f'<filter id="en{k}" x="0" y="0" width="1" height="1"><feGaussianBlur stdDeviation="2"{mode}/></filter><rect x="{x}" y="22" width="10" height="10" fill="lime" filter="url(#en{k})"/>')
    plains.append(f'<image x="{x}" y="22" width="10" height="10" image-rendering="pixelated" href="{faded(2)}"/>')
unit.cell("blur-edge-none", (71, 21, 24, 12), "".join(tests), "".join(plains), "", tolerance=16)
unit.cell("arithmetic-reader", (1, 37, 32, 14), f'<filter id="ar" {F} x="2" y="38" width="30" height="12"><feFlood flood-color="lime" flood-opacity=".5" x="2" y="38" width="12" height="12" result="f"/>'
          '<feComposite in="f" in2="f" operator="arithmetic" k2="2" result="r"/><feOffset in="r" dx="4"/></filter><rect x="2" y="38" width="30" height="12" fill="red" filter="url(#ar)"/>',
          lime(6, 38, 8, 12), '<rect x="6" y="38" width="8" height="12"/>')
unit.cell("lighting-subregion", (35, 37, 16, 14), f'<filter id="ls" {F} x="36" y="38" width="12" height="12"><feFlood flood-color="black"/>'
          '<feDiffuseLighting lighting-color="lime" y="41" height="5"><feDistantLight elevation="90"/></feDiffuseLighting></filter><rect x="36" y="38" width="12" height="12" fill="red" filter="url(#ls)"/>',
          lime(36, 41, 12, 5), '<rect x="36" y="41" width="12" height="5"/>')
unit.write()
