# Chasing the spec

The renderer is to do what the specs say of the pixels it draws, as closely
as it can without losing speed past a budget, and without following text
that contradicts itself, the other specs or plain sense. Every decision of
that kind is written down in `decisions.md`, to be looked at again.

## Scope

What README.md leaves out stays out: text, animation (SMIL and CSS),
script, `foreignObject`, interaction, the DOM. `excluded.txt` lists the
vocabulary left out for those reasons.

## What is chased

A statement: a normative sentence or a step of an algorithm in `dev/spec`
that changes what is drawn, or how a document is read in a way that
changes it (parsing, defaults, inheritance, error handling). Not
statements about authors, generators, the DOM or events.

`statements/` lists them, a file per spec page. A feature is a heading
naming the unit that draws it, and its statements are lines under it:

```
## Dash positions: stroke-dash-positions

- [ ] svg2/painting.md#TermDashPositions: the first dash starts at stroke-dashoffset into the pattern, taken mod its length
- [-] svg2/painting.md#...: ... (D3)
```

`[ ]` is not yet checked, `[x]` drawn right by the feature's units, `[-]` left
out, with the decision in `decisions.md` named at the end of the line, `(D4)`.

A statement checked or left out ends with the test cells that draw it,
`[unit#cell unit2#cell2]`, after its decision if it has one. A cell is an
element of `tst/expect/unit.svg` with that id and a `data-box="x y w h"`:
the expected drawing of `tst/unit/unit.svg`, the same size, drawing each
cell as it should look by plain means, a length for a computed one, a path
for a clip, a picture for a formula. `tst/oracle.py` draws both and
compares them as a unit is compared with the canon, reporting each
difference in the cells holding it, so a unit checks its statements, not
only that it draws as the canon does. A unit with cells is written by its
generator, `tst/gen/unit.py`, which writes both drawings.

`ledger.py` checks that each anchor is in its page, that a checked feature's
units exist, that what is left out names its decision, that every statement
checked or left out links its cells and that every link names a unit and a
cell of its expected drawing.

## How

1. A batch is a section or a chapter: its statements are written into
   `statements/`.
2. Every feature gets a unit of its own under `tst/unit`, a small drawing
   of what its statements say, named in its heading; units drawn before,
   marked `(exists)`, serve if they draw them all.
3. The drawings are looked at against the text; a wrong one is fixed in
   `svg.cpp`.
4. Speed is measured over the corpus before and after a batch: it may cost
   1-2% in all, 10% on a file, more only with a reason in `decisions.md`.
   A permitted approximation (the spec's "can be used", the stroke's
   "leeway") is conformance and is kept where it is faster.
5. The changes the batch makes to the corpus are looked at, each explained
   by a fix; then `tst/CANON` moves to the batch's commit.
