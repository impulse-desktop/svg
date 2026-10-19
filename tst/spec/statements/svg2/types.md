# SVG 2, Chapter 4: Basic Data Types and Interfaces

## Attribute value syntax, initial and invalid values: attribute-values, style-sheets, gradient-templates, stroke-width-values, presentation-numbers

- [x] svg2/types.md#TermInitialValue: an attribute or property's initial value is used for rendering whenever it is not specified, or has an invalid value. [style-sheets#important-attribute stroke-width-values#negative]
- [x] svg2/types.md#TermInvalidValue: a value is invalid if disallowed by the property's grammar, or allowed by the grammar but disallowed in prose; an invalid CSS declaration is ignored. [style-sheets#important-attribute stroke-width-values#negative]
- [x] svg2/types.md#presentation-attribute-css-value: parsing a presentation attribute substitutes [<length>|<number>] for <length>, [<length-percentage>|<number>] for <length-percentage>, and [<angle>|<number>] for <angle> in its property's grammar before parsing, so the attribute takes unitless numbers. [stroke-width-values#user-units style-sheets#geometry presentation-numbers#accepted clip-property-rect#numbers]
- [-] svg2/types.md#presentation-attribute-css-value: the corresponding property values disallow those unitless numbers; declarations take them too, as the renderers of real files do (D43) [presentation-numbers#declaration presentation-numbers#sheet clip-property-rect#numbers]
- [x] svg2/types.md#attribute-css-value: any other attribute defined with the CSS Value Definition Syntax is parsed directly against its own grammar table, without that length/angle-to-number substitution; CSS comments and escapes are allowed within such values. [css-escapes#attributes css-escapes#comments]
- [x] svg2/types.md#attribute-url: a URL-valued attribute is parsed with the URL parser, using the attribute's value as input and the document's URL as base. [gradient-templates#chain-stops]
- [x] svg2/types.md#syntax: an attribute that fails to parse against its grammar (CSS, ABNF, EBNF, URL or prose) is treated as if it had been given its initial value. (D16) [style-sheets#important-attribute stroke-width-values#negative]
- [-] svg2/types.md#syntax: an invalid presentation attribute's resulting initial value can override lower-priority stylesheet rules, e.g. overflow="invalid" on an svg element yields overflow:visible, overriding the UA stylesheet's hidden default. (D16) [attribute-values#invalid-overflow]
- [x] svg2/types.md#Precision: numeric SVG attribute and property values must support at least every finite single-precision value of the host architecture. [css-numbers#precision]
- [x] svg2/types.md#RangeClamping: out-of-range checking for range-restricted values must be deferred as late as possible in rendering, since a compound operation's intermediate values may be out of range even when its final value is not. [attribute-values#range-deferred]

## Notes

- svg2/types.md#RangeClamping: "as late as possible" is left to implementation judgement; clamping earlier for speed can diverge from the spec's deferred-checking model.
