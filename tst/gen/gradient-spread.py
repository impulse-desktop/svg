"""gradient-spread: spreadMethod, of SVG 2. The unit is written by hand;
this writes its expected drawing: each test as its reference, a gradient
whose stops spell the spread out, under or beside it."""
from unitgen import expect, pair

NAME = "gradient-spread"
expect(NAME, [
    ("linear-pad", (1, 1, 62, 8), pair(NAME, 2, 2, 5)), ("linear-reflect", (1, 9, 62, 8), pair(NAME, 2, 10, 13)), ("linear-repeat", (1, 17, 62, 8), pair(NAME, 2, 18, 21)),
    ("radial-pad", (2, 27, 20, 34), pair(NAME, 3, 28, 44)), ("radial-reflect", (22, 27, 20, 34), pair(NAME, 23, 28, 44)), ("radial-repeat", (42, 27, 20, 34), pair(NAME, 43, 28, 44)),
])
