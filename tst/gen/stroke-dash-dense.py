"""Independent geometry for zero-length square-capped dashes at subpixel width."""
from unitgen import Unit


unit = Unit("stroke-dash-dense", 350, 72,
            "Zero-length dashes have square caps even when their stroke is thinner than a pixel.")


def stroke(y, pattern):
    return (f'<path d="M8 {y} H87" fill="none" stroke="lime" stroke-width=".44" '
            f'stroke-linecap="square" stroke-dasharray="{pattern}"/>')


def rectangle(x, y):
    return f'<rect x="{x - .22:.2f}" y="{y - .22:.2f}" width=".44" height=".44" fill="lime"/>'


unit.cell("dense-zero-square", (7, 12, 81, 7), stroke(15, "0 .02 0 0"),
          '<rect x="7.78" y="14.78" width="79.44" height=".44" fill="lime"/>')
unit.cell("sparse-zero-square", (7, 28, 81, 7), stroke(31, "0 2 0 0"),
          "".join(rectangle(8 + 2 * i, 31) for i in range(40)))
unit.cell("solid-control", (7, 44, 81, 7), stroke(47, "none"),
          '<rect x="7.78" y="46.78" width="79.44" height=".44" fill="lime"/>')
unit.write()
