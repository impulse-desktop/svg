# CSS Basic User Interface Module Level 4, static SVG outlines

## Outline offsets: outline-offset

- [x] css-ui-4.md#outline-offset: outline-offset accepts a length, initially zero, is not inherited, and does not accept percentages or nonzero unitless numbers [outline-offset#zero outline-offset#not-inherited outline-offset#inherit outline-offset#initial outline-offset#unset outline-offset#invalid-percent outline-offset#invalid-number outline-offset#font-relative outline-offset#calc outline-offset#variable outline-offset#invalid-variable]
- [x] css-ui-4.md#outline-offset: positive offsets leave space outside the border edge; negative offsets shrink the outline into it [outline-offset#positive outline-offset#negative outline-offset#inside outline-offset#translucent-inside]
- [x] css-ui-4.md#outline-offset: with negative offsets, the outer shape should remain at least twice the outline width in each dimension, constrained independently [outline-offset#collapse-one-axis outline-offset#collapse-both-axes]
- [x] css-ui-4.md#outline: the outline shorthand excludes outline-offset, allowing the offset to cascade independently [outline-offset#shorthand-preserves-offset]

## Automatic outline rendering: outline-offset

- [x] css-ui-4.md#outline-style: auto may use a solid outline and ignores the specified outline-width; this renderer chooses a 3px solid outline [outline-offset#auto outline-offset#auto-ignores-width outline-offset#auto-inside]
- [x] css-ui-4.md#outline-color: auto color with an explicit solid style computes to currentColor [outline-offset#auto-color-after-solid outline-offset#auto-color-before-solid]

SVG's non-presentation CSS properties do not become presentation attributes:
outline-offset#not-presentation-attribute preserves that distinction. The
outermost SVG has no surrounding CSS box in this API (D39); these outlines
apply to rendered graphics, while the existing render-root-box test remains
unchanged.
