"""filter-function-grammar: the filter functions' grammar and defaults, of
CSS Filter Effects. The unit is written by hand; this writes its expected
drawing."""
from unitgen import expect, pair

U = "filter-function-grammar"


def pairs(*xs, top=2):
    return "".join(pair(U, x, top, top + 6) for x in xs)


expect(U, [
    ("defaults", (1, 1, 62, 14), pairs(2, 16, 23, 37, 51, 58) + pairs(2, 9, 23, top=16)),
    ("clamped", (8, 1, 7, 14), pairs(9, 30, 44)),
    ("invalid", (15, 15, 41, 14), pairs(16, 30, 37, 44, 51, top=16)),
    ("hue-rotate-turn", (57, 15, 7, 14), pairs(58, top=16)),
])
