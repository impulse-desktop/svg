"""use-chain and measure-use-chain: a use of a use of a use, 40 deep,
drawn and measured whole: SVG sets no depth past which a chain of uses
stops, and an Inkscape drawing clones its clones 35 deep."""
from unitgen import Unit, expect

CHAIN = '<rect id="u0" x="10" y="10" width="4" height="4" fill="lime"/>' + "".join(f'<use id="u{i}" href="#u{i - 1}"/>' for i in range(1, 41))

unit = Unit("use-chain", 16, 16, """A use of the last of 40 uses, each of
the one before, the first of a lime square: it draws the square.""")
unit.common(f"<defs>{CHAIN}</defs>", "")
unit.cell("deep", (8, 8, 8, 8), '<use href="#u40"/>', '<rect x="10" y="10" width="4" height="4" fill="lime"/>')
unit.write()

with open(f"{__import__('unitgen').ROOT}/tst/unit/measure-use-chain.svg", "w") as handle:
    handle.write(f'<svg xmlns="http://www.w3.org/2000/svg"><defs>{CHAIN}</defs><use href="#u40"/></svg>\n')

expect("measure-use-chain", [("measured", (0, 0, 14, 14), '<rect x="10" y="10" width="4" height="4" fill="lime"/>')], root='<svg xmlns="http://www.w3.org/2000/svg" width="14" height="14">')
