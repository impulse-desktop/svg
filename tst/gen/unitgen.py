"""Writes a unit and its expected drawing from cells.

A unit, tst/unit/NAME.svg, draws its cells with the features under test; its
expected drawing, tst/expect/NAME.svg, draws each cell as it should look,
with plain means: a length for a computed one, a path for a clip, a picture
for a formula. tst/oracle.py compares the two, and the statement ledgers
link to the cells by id. A generator under tst/gen builds a Unit, adds its
cells and writes it; running it again writes the same files.
"""
import os
import re
import textwrap

ROOT = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def comment(text):
    return "\n".join(textwrap.wrap("<!-- " + " ".join(text.split()) + " -->", 72))


class Unit:
    def __init__(self, name, width, height, description, tolerance=None, root="", ground=True, plain_root=None):
        self.name = name
        self.width = width
        self.height = height
        self.description = description
        self.tolerance = tolerance
        self.root = root
        # The root's attributes in the expected drawing, which draws what
        # they do by plain means.
        self.plain_root = root if plain_root is None else plain_root
        # A white ground under the cells, unless what they blend with must be
        # transparent.
        self.ground = ground
        self.shared = []
        self.cells = []
        self.outlines = []
        self.tolerances = {}

    def common(self, test, expected=None):
        """Markup both drawings have before the cells: definitions, styles,
        or, with expected, the plain form the expected drawing has."""
        self.shared.append((test, test if expected is None else expected))

    def cell(self, name, box, test, expected, outline="", tolerance=None):
        """A cell: its id, its box (x, y, width, height), what the unit draws
        and what the expected drawing draws there, and an outline both draw
        over it for the eye; a tolerance of its own for what it draws by a
        permitted approximation."""
        assert all(name != other for other, *_ in self.cells), name
        self.cells.append((name, box, test, expected))
        self.tolerances[name] = tolerance

        if outline:
            # An outline draws over both drawings alike, so it must draw
            # thin black lines alone: paint of its own would hide what the
            # cell draws from the comparison.
            self.outlines.append(re.sub(r'\s(?:fill|fill-opacity|fill-rule|opacity|style|filter|mask|clip-path|stroke[a-z-]*|marker[a-z-]*)="[^"]*"', "", outline))

    def write(self):
        size = f'width="{self.width}" height="{self.height}"'
        ground = f'<rect {size} fill="white"/>' if self.ground else ""
        outlines = '<g fill="none" stroke="black" stroke-width=".25">' + "".join(self.outlines) + "</g>" if self.outlines else ""
        tolerance = f' data-tolerance="{self.tolerance}"' if self.tolerance is not None else ""
        root = f" {self.root}" if self.root else ""
        unit = [f'<svg xmlns="http://www.w3.org/2000/svg" {size}{root}>', comment(self.description), ground]
        unit += [test for test, _ in self.shared]
        unit += [test for _, _, test, _ in self.cells]
        unit += [outlines]
        plain_root = f" {self.plain_root}" if self.plain_root else ""
        expected = [f'<svg xmlns="http://www.w3.org/2000/svg" {size}{plain_root}{tolerance}>', comment(f"What tst/unit/{self.name}.svg draws, drawn plainly; each cell's id is what the statement ledgers link to."), ground]
        expected += [plain for _, plain in self.shared]
        expected += [f'<g id="{name}" data-box="{x} {y} {w} {h}"' + (f' data-tolerance="{self.tolerances[name]}"' if self.tolerances[name] is not None else "") + f">{plain}</g>" for name, (x, y, w, h), _, plain in self.cells]
        expected += [outlines]

        for directory, lines in (("unit", unit), ("expect", expected)):
            with open(os.path.join(ROOT, "tst", directory, self.name + ".svg"), "w") as handle:
                handle.write("\n".join(line for line in lines if line) + "\n</svg>\n")


def rows(unit, y, groups, columns, pitch, height, item, x=2, tolerances={}):
    """Lays out groups of items, each group a cell on rows of its own,
    columns items to a row, pitch apart, rows height apart; item(x, y, value)
    gives one item's test, its expected markup and its outline; tolerances
    names the cells of a tolerance of their own. Returns the y below the
    last row."""
    for name, values in groups:
        tests, plains, outlines = [], [], []

        for i, value in enumerate(values):
            test, plain, outline = item(x + i % columns * pitch, y + i // columns * height, value)
            tests.append(test)
            plains.append(plain)
            outlines.append(outline)

        count = (len(values) + columns - 1) // columns
        unit.cell(name, (x - 1, y - 1, columns * pitch, count * height), "".join(tests), "".join(plains), "".join(outlines), tolerances.get(name))
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


def png(rows):
    """A data URI of a PNG of rows of straight (r, g, b, a) in [0, 1]."""
    import base64
    import struct
    import zlib

    def chunk(kind, data):
        return struct.pack(">I", len(data)) + kind + data + struct.pack(">I", zlib.crc32(kind + data) & 0xffffffff)

    raw = b"".join(b"\0" + bytes(min(255, max(0, round(255 * v))) for pixel in row for v in pixel) for row in rows)
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", len(rows[0]), len(rows), 8, 6, 0, 0, 0)) + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b"")

    return "data:image/png;base64," + base64.b64encode(data).decode()


def expect(name, cells, tolerance=None):
    """Writes the expected drawing of a handwritten unit, tst/unit/NAME.svg,
    which stays as it is: its root, its ground and its outlines as the unit
    has them, and each cell, (id, (x, y, width, height), plain markup[,
    tolerance]), drawn plainly."""
    text = open(os.path.join(ROOT, "tst", "unit", name + ".svg")).read()
    root = re.match(r"<svg\b[^>]*>", text).group(0)
    ground = re.search(r'<rect width="[^"]*" height="[^"]*" fill="white"/>', text)
    outlines = re.findall(r'<g fill="none" stroke="black" stroke-width="\.25">.*?</g>|<(?:rect|path|circle|ellipse)\b[^>]*fill="none" stroke="black" stroke-width="\.25"[^>]*/>', text, re.S)
    lines = [root[:-1] + (f' data-tolerance="{tolerance}"' if tolerance is not None else "") + ">", comment(f"What tst/unit/{name}.svg draws, drawn plainly; each cell's id is what the statement ledgers link to.")]
    lines += [ground.group(0)] if ground else []

    for cell in cells:
        cid, (x, y, w, h), plain = cell[:3]
        own = f' data-tolerance="{cell[3]}"' if len(cell) > 3 else ""
        lines.append(f'<g id="{cid}" data-box="{x} {y} {w} {h}"{own}>{plain}</g>')

    lines += outlines

    with open(os.path.join(ROOT, "tst", "expect", name + ".svg"), "w") as handle:
        handle.write("\n".join(lines) + "\n</svg>\n")
