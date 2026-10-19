"""filter-drop-shadow-syntax: drop-shadow()'s syntax of CSS Filter Effects.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect, pair

U = "filter-drop-shadow-syntax"
expect(U, [(name, (x - 4, 0, 11, 26), pair(U, x, 4, 30)) for name, x in [("no-blur", 4), ("colour-first", 15), ("colour-last", 26), ("one-length", 37), ("four-lengths", 48), ("two-shadows", 59), ("negative-offset", 70)]])
