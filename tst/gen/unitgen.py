"""Writes a unit and its expected drawing from cells.

A unit, tst/unit/NAME.svg, draws its cells with the features under test; its
expected drawing, tst/expect/NAME.svg, draws each cell as it should look,
with plain means: a length for a computed one, a path for a clip, a picture
for a formula. tst/oracle.py compares the two, and the statement ledgers
link to the cells by id. A generator under tst/gen builds a Unit, adds its
cells and writes it; running it again writes the same files.
"""
import os
import textwrap

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def comment(text):
    return "\n".join(textwrap.wrap("<!-- " + " ".join(text.split()) + " -->", 72))


class Unit:
    def __init__(self, name, width, height, description, tolerance=None, root=""):
        self.name = name
        self.width = width
        self.height = height
        self.description = description
        self.tolerance = tolerance
        self.root = root
        self.shared = []
        self.cells = []
        self.outlines = []

    def common(self, test, expected=None):
        """Markup both drawings have before the cells: definitions, styles,
        or, with expected, the plain form the expected drawing has."""
        self.shared.append((test, test if expected is None else expected))

    def cell(self, name, box, test, expected, outline=""):
        """A cell: its id, its box (x, y, width, height), what the unit draws
        and what the expected drawing draws there, and an outline both draw
        over it for the eye."""
        assert all(name != other for other, *_ in self.cells), name
        self.cells.append((name, box, test, expected))

        if outline:
            self.outlines.append(outline)

    def write(self):
        size = f'width="{self.width}" height="{self.height}"'
        ground = f'<rect {size} fill="white"/>'
        outlines = '<g fill="none" stroke="black" stroke-width=".25">' + "".join(self.outlines) + "</g>" if self.outlines else ""
        tolerance = f' data-tolerance="{self.tolerance}"' if self.tolerance is not None else ""
        root = f" {self.root}" if self.root else ""
        unit = [f'<svg xmlns="http://www.w3.org/2000/svg" {size}{root}>', comment(self.description), ground]
        unit += [test for test, _ in self.shared]
        unit += [test for _, _, test, _ in self.cells]
        unit += [outlines]
        expected = [f'<svg xmlns="http://www.w3.org/2000/svg" {size}{root}{tolerance}>', comment(f"What tst/unit/{self.name}.svg draws, drawn plainly; each cell's id is what the statement ledgers link to."), ground]
        expected += [plain for _, plain in self.shared]
        expected += [f'<g id="{name}" data-box="{x} {y} {w} {h}">{plain}</g>' for name, (x, y, w, h), _, plain in self.cells]
        expected += [outlines]

        for directory, lines in (("unit", unit), ("expect", expected)):
            with open(os.path.join(ROOT, "tst", directory, self.name + ".svg"), "w") as handle:
                handle.write("\n".join(line for line in lines if line) + "\n</svg>\n")


def rows(unit, y, groups, columns, pitch, height, item, x=2):
    """Lays out groups of items, each group a cell on rows of its own,
    columns items to a row, pitch apart, rows height apart; item(x, y, value)
    gives one item's test, its expected markup and its outline. Returns the
    y below the last row."""
    for name, values in groups:
        tests, plains, outlines = [], [], []

        for i, value in enumerate(values):
            test, plain, outline = item(x + i % columns * pitch, y + i // columns * height, value)
            tests.append(test)
            plains.append(plain)
            outlines.append(outline)

        count = (len(values) + columns - 1) // columns
        unit.cell(name, (x - 1, y - 1, columns * pitch, count * height), "".join(tests), "".join(plains), "".join(outlines))
        y += count * height

    return y


TARGET = '<rect class="t" width="6" height="6" fill="red"/>'


def squares(unit, groups, columns=8):
    """Lays out the units of style sheets: each item a rule and the children
    of a group of class cN at its place, {c} in the rule standing for .cN,
    and the rule should fill its 6 by 6 square lime, a target of class t
    drawn red else (TARGET); a rule after ! is a style element of its own,
    which may leave things open. Each group of items is a cell on rows of
    its own. Sets the unit's size."""
    sheet, own = [], []
    y = 2
    i = 0

    for name, items in groups:
        tests, plains, outlines = [], [], []

        for k, (rule, children) in enumerate(items):
            x, top = 2 + k % columns * 8, y + k // columns * 8
            rule = rule.replace("{c}", f".c{i}")

            if rule.startswith("!"):
                own.append(f"<style>{rule[1:]}</style>")
            elif rule:
                sheet.append(rule)

            tests.append(f'<g class="c{i}" transform="translate({x} {top})">{children}</g>')
            plains.append(f'<rect x="{x}" y="{top}" width="6" height="6" fill="lime"/>')
            outlines.append(f'<rect x="{x}" y="{top}" width="6" height="6"/>')
            i += 1

        count = (len(items) + columns - 1) // columns
        unit.cell(name, (1, y - 1, columns * 8, count * 8), "".join(tests), "".join(plains), "".join(outlines))
        y += count * 8

    unit.common("<style><![CDATA[\n" + "\n".join(sheet) + "\n]]></style>" + "".join(own), "")
    unit.width = 2 + columns * 8
    unit.height = y
