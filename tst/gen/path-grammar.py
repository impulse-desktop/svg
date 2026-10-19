"""path-grammar: path data and points of SVG 2. The unit is written by hand;
this writes its expected drawing."""
from unitgen import outlined

outlined("path-grammar", [
    ("number-ends", (1, 1, 12, 12), [0]), ("error", (13, 1, 13, 12), [1]), ("short-command", (27, 1, 13, 12), [2]), ("relative-closepath", (41, 1, 13, 12), [3]),
    ("smooth-cubic", (1, 13, 15, 12), [4]), ("smooth-quadratic", (17, 13, 15, 12), [5]), ("smooth-after-other", (33, 13, 24, 12), [6, 7]),
    ("arc-zero", (1, 25, 11, 10), [8]), ("arc-negative", (13, 24, 11, 11), [9]), ("arc-small", (25, 23, 13, 12), [10]), ("arc-same-ends", (39, 25, 11, 10), [11]),
    ("points-odd", (1, 38, 13, 12), [12]), ("line-not-filled", (19, 39, 13, 11), []),
], paint=' fill="none" stroke="lime" stroke-width="1.5"')
