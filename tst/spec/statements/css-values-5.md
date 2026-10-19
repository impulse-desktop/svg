# CSS Values and Units Module Level 5

## Arbitrary substitution: css-variables

- [x] css-values-5.md#resolve-property: a property value with an arbitrary substitution function whose argument grammar holds is valid at parse time, whatever its own grammar; one whose function has no valid arguments is invalid then, and a lower declaration wins [css-variables#grammar css-variables#invalid]
- [x] css-values-5.md#property-replacement: the functions are substituted at computed-value time, then the value is parsed by the property's grammar; a value that does not parse, or holds the guaranteed-invalid value, is invalid at computed-value time [css-variables#invalid css-variables#presentation]
- [x] css-values-5.md#resolve-property: a value that is a single CSS-wide keyword after substitution is that keyword, as if specified so [css-variables#shorthands css-variables#keywords]
- [x] css-values-5.md#property-replacement: substitution is of tokens, not text: a number substituted before an ident is a number and an ident, no dimension [css-variables#tokens]
- [x] css-values-5.md#invalid-at-computed-value-time: a property invalid at computed-value time takes its inherited value if it inherits and its initial value else, as unset gives, not a lower declaration in the cascade; a custom property the guaranteed-invalid value [css-variables#invalid css-variables#keywords]
- [x] css-values-5.md#cyclic-substitution-contexts: a substitution that needs itself marks every substitution in the cycle cyclic, each of which gives the guaranteed-invalid value, a fallback notwithstanding [css-variables#cycles]
- [x] css-values-5.md#pending-substitution-value: a shorthand with an arbitrary substitution function gives each of its longhands a pending-substitution value; after substitution the shorthand is parsed again and each longhand takes its part [css-variables#shorthands]
- [x] css-values-5.md#long-substitution: a UA limits how many tokens a substitution may give, past which it gives the guaranteed-invalid value: the renderer 16384, and half a million in all a document's substitutions write [css-variables#long]
