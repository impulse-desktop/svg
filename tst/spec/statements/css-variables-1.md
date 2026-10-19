# CSS Custom Properties for Cascading Variables Module Level 1

## Custom properties: css-variables

- [x] css-variables-1.md#custom-property: a property whose name starts with two dashes, -- alone aside, is a custom property; any element takes it, a sheet's rules, a style attribute and @media alike [css-variables#declared css-variables#inherited]
- [x] css-variables-1.md#defining-variables: custom property names are not ASCII case-insensitive: --foo and --FOO are two properties [css-variables#case]
- [x] css-variables-1.md#propdef-: a custom property inherits; its initial value is the guaranteed-invalid value; its computed value is its specified value with its variables substituted, or the guaranteed-invalid value [css-variables#inherited css-variables#keywords css-variables#cycles]
- [x] css-variables-1.md#defining-variables: the CSS-wide keywords work in a custom property as in any other property, at cascaded-value time, and are not kept as its value to substitute; the all property resets no custom property [css-variables#keywords css-variables#not-all]
- [x] css-variables-1.md#syntax: a custom property's value is any <declaration-value>, which no property's grammar checks, or nothing: --foo:; is a valid empty value, not the guaranteed-invalid one; a trailing !important is the declaration's, not its value's [css-variables#tokens css-variables#cascade]
- [x] css-variables-1.md#syntax: a var() in a custom property's value must be valid by var()'s grammar, else the custom property is invalid and ignored [css-variables#grammar]
- [x] css-variables-1.md#guaranteed-invalid-value: initial makes a custom property the guaranteed-invalid value; a value into which the guaranteed-invalid value is substituted is invalid at computed-value time [css-variables#keywords css-variables#invalid]

## The var() notation: css-variables

- [x] css-variables-1.md#funcdef-var: var() substitutes the value of the custom property its first argument names into any property's value; its second argument, after the first comma and perhaps holding commas or nothing, is the fallback used when that property is the guaranteed-invalid value [css-variables#fallback css-variables#declared]
- [x] css-variables-1.md#replace-a-var-function: the first argument is itself substituted and then parsed as a custom property name, so it may come from another var(); one that is no custom property name gives the guaranteed-invalid value, and the fallback; Chromium and Firefox still take only a custom property name there, by an earlier grammar [css-variables#names]
- [x] css-variables-1.md#example-2a0a82cc: a var() draws from the computed value of the custom property on the same element, which the cascade and inheritance give [css-variables#inherited css-variables#cascade css-variables#instance]
- [x] css-variables-1.md#example-0e8dabdc: custom properties that depend on each other in a cycle all compute to the guaranteed-invalid value, a fallback in the cycle notwithstanding [css-variables#cycles]

## Notes

- A presentation attribute is read as CSS (svg2/types.md#presentation-attribute-css-value), so var() works in fill="var(--c)" as in a style attribute, as Chromium and Firefox take it [css-variables#presentation]. d and transform are read by SVG's own grammars (D30), and take none.
- An instance's custom properties are its use's: the use is the parent the instance inherits from, and each use of one target can give it other values [css-variables#instance css-variables-use#own css-variables-use#inherited css-variables-use#twice css-variables-use#rule].
