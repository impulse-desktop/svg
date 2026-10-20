"""use-bomb and measure-use-bomb: a group of two uses of the group before,
40 deep, the first a lime square: 2^40 instances, which no pass draws or
measures all of (D54); a pass draws and measures the elements its budget
lets it, the square first among them."""
from unitgen import Unit, expect

BOMB = '<rect id="g0" width="8" height="8" fill="lime"/>' + "".join(f'<g id="g{i}"><use href="#g{i - 1}"/><use href="#g{i - 1}"/></g>' for i in range(1, 41))

unit = Unit("use-bomb", 16, 16, """A use of a group of two uses of the
group before, 40 deep, each instancing a lime square at the corner: the
drawing stops at its work budget, the square drawn before it.""")
unit.common(f"<defs>{BOMB}</defs>", "")
unit.cell("first-square", (0, 0, 16, 16), '<use href="#g40"/>', '<rect width="8" height="8" fill="lime"/>')
unit.write()

with open(f"{__import__('unitgen').ROOT}/tst/unit/measure-use-bomb.svg", "w") as handle:
    handle.write(f'<svg xmlns="http://www.w3.org/2000/svg"><defs>{BOMB}</defs><use href="#g40"/></svg>\n')

expect("measure-use-bomb", [("measured", (0, 0, 8, 8), '<rect width="8" height="8" fill="lime"/>')], root='<svg xmlns="http://www.w3.org/2000/svg" width="8" height="8">')
