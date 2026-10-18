# SVG 2, Chapter 4: Basic Data Types and Interfaces

## Attribute value syntax, initial and invalid values: css-attribute-parsing

- [ ] svg2/types.md#TermInitialValue: an attribute or property's initial value is used for rendering whenever it is not specified, or has an invalid value.
- [ ] svg2/types.md#TermInvalidValue: a value is invalid if disallowed by the property's grammar, or allowed by the grammar but disallowed in prose; an invalid CSS declaration is ignored.
- [ ] svg2/types.md#presentation-attribute-css-value: parsing a presentation attribute substitutes [<length>|<number>] for <length>, [<length-percentage>|<number>] for <length-percentage>, and [<angle>|<number>] for <angle> in its property's grammar before parsing, allowing unitless numbers only in the attribute form.
- [ ] svg2/types.md#attribute-css-value: any other attribute defined with the CSS Value Definition Syntax is parsed directly against its own grammar table, without that length/angle-to-number substitution; CSS comments and escapes are allowed within such values.
- [ ] svg2/types.md#attribute-url: a URL-valued attribute is parsed with the URL parser, using the attribute's value as input and the document's URL as base.
- [ ] svg2/types.md#syntax: an attribute that fails to parse against its grammar (CSS, ABNF, EBNF, URL or prose) is treated as if it had been given its initial value.
- [ ] svg2/types.md#syntax: an invalid presentation attribute's resulting initial value can override lower-priority stylesheet rules, e.g. overflow="invalid" on an svg element yields overflow:visible, overriding the UA stylesheet's hidden default.
- [ ] svg2/types.md#Precision: numeric SVG attribute and property values must support at least every finite single-precision value of the host architecture.
- [ ] svg2/types.md#RangeClamping: out-of-range checking for range-restricted values must be deferred as late as possible in rendering, since a compound operation's intermediate values may be out of range even when its final value is not.

## Notes

- svg2/types.md#RangeClamping: "as late as possible" is left to implementation judgement; clamping earlier for speed can diverge from the spec's deferred-checking model.
