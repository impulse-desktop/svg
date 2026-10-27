"""paint-budget: fills paint at most 2^11 canvases (D59); groups of two uses
of the group before make thousands of white squares from a few dozen
elements, much area and few elements."""
from unitgen import Unit


def squares(name, x, levels):
    return f'<rect id="{name}0" x="{x}" width="{16 - x}" height="16" fill="white"/>' + "".join(f'<g id="{name}{i}"><use href="#{name}{i - 1}"/><use href="#{name}{i - 1}"/></g>' for i in range(1, levels + 1))


unit = Unit("paint-budget", 16, 16, """White squares, 1024 over the whole
canvas and then 4096 over its right half, each a use among a few dozen
elements: fills paint at most 2^11 canvases in all (D59), so a lime half
drawn after the first 1024 shows, and one drawn on the right after the
4096 does not, the drawing having stopped among them.""")
unit.common(f"<defs>{squares('w', 0, 10)}{squares('r', 8, 12)}</defs>", "")
unit.cell("within", (0, 0, 8, 16), '<use href="#w10"/><rect width="8" height="16" fill="lime"/>', '<rect width="8" height="16" fill="lime"/>')
unit.cell("past", (8, 0, 8, 16), '<use href="#r12"/><rect x="8" width="8" height="16" fill="lime"/>', "")
unit.write()
