# CSS Cascading and Inheritance Module Level 5

## @import in a style element: css-layers, css-cascade-edges (exists)

- [x] css-cascade-5.md#at-ruledef-import: @import must precede every other valid at-rule and style rule in its style sheet (@charset and @layer-statement rules aside) or the @import rule itself is invalid [css-layers#import-position]
- [x] css-cascade-5.md#at-ruledef-import: a bare string in @import is interpreted the same as url(string) [css-layers#import-string]
- [-] css-cascade-5.md#conditional-import: an @import whose media or supports condition does not match contributes nothing, exactly as if its sheet were wrapped in @media/@supports; with no condition the import is unconditional (D15) [css-layers#import-nothing]
- [-] css-cascade-5.md#import-processing: an imported style sheet's declarations are treated as if substituted in place of the @import rule, and take the cascade origin of the importing sheet (D15) [css-layers#import-nothing]
- [x] css-cascade-5.md#at-ruledef-import: @import can assign the whole imported sheet to an anonymous or named cascade layer, which is added to the layer order even if the sheet never loads [css-layers#import-layer]

## Shorthand properties: css-cascade-sort, css-all, css-revert, mask-longhands (exists)

- [x] css-cascade-5.md#shorthand-property: a shorthand property sets every one of its longhand sub-properties, exactly as if each were written out [css-cascade-sort#shorthand-sets]
- [x] css-cascade-5.md#shorthand-property: a longhand the shorthand's value omits is reset to its own initial value, not left as whatever it was cascaded to before [css-cascade-sort#shorthand-reset]
- [x] css-cascade-5.md#shorthand-property: a reset-only sub-property is always reset to its initial value by the shorthand, even though the shorthand's syntax has no way to set it to anything else [css-cascade-sort#shorthand-reset-only]
- [x] css-cascade-5.md#shorthand-property: a CSS-wide keyword as a shorthand's whole value sets every one of its sub-properties, including reset-only ones, to that keyword; it cannot be combined with any other component [css-cascade-sort#shorthand-keyword css-revert#shorthand-keyword]
- [x] css-cascade-5.md#shorthand-property: marking a shorthand declaration !important is equivalent to marking every one of its longhand sub-properties !important [css-cascade-sort#shorthand-important]
- [x] css-cascade-5.md#all-shorthand: the all property is a shorthand resetting every property except direction and unicode-bidi; it accepts only the CSS-wide keywords [css-all#all-initial css-all#all-inherit css-all#all-unset css-all#all-keywords-only]

## Value processing stages: css-cascade-sort, css-geometry-validity (exists)

- [x] css-cascade-5.md#value-stages: a property's value passes through declared, cascaded, specified, computed, used and actual stages, in that order [css-length-units#computed css-calc-contexts#length-percent css-cascade-sort#no-cascade]
- [x] css-cascade-5.md#value-stages: cascading picks one declared value as the cascaded value; defaulting then yields the specified value, and resolving dependencies yields the computed value [css-cascade-sort#no-cascade css-cascade-sort#sort-layer css-length-units#computed]
- [x] css-cascade-5.md#cascaded: if no declaration for a property applies to an element, there is no cascaded value for it there [css-cascade-sort#no-cascade]
- [x] css-cascade-5.md#specified: the specified value is the cascaded value if there is one, otherwise it comes from defaulting (inheritance or the initial value) [css-cascade-sort#no-cascade css-wide-keywords#unset-not-inherited]
- [x] css-cascade-5.md#computed: computing a specified value resolves it as far as possible without layout: relative units are made absolute, keywords are replaced by their defined equivalents, and valid relative URLs are resolved [css-length-units#computed css-cascade-sort#computed-url]
- [x] css-cascade-5.md#computed: computing a value generally does not resolve percentages or perform layout; those are left for the used value, unless the property defines otherwise [css-calc-contexts#length-percent css-calc-contexts#translate-percent]
- [x] css-cascade-5.md#applies-to: a property that does not apply to a given element or box type has no used value there, even though it still has a computed value that can inherit to its descendants [css-cascade-sort#applies-to]

## Origin, importance, specificity and order: css-cascade-sort, css-layers, css-specificity (exists)

- [x] css-cascade-5.md#cascade-sort: declarations for one property on one element are sorted by, in order: origin/importance, direct element attachment (style attribute) vs. selector, cascade layer, specificity, then order of appearance [css-cascade-sort#sort-importance css-cascade-sort#sort-attachment css-cascade-sort#sort-layer css-selector-basics#specificity css-selector-basics#specificity-order]
- [x] css-cascade-5.md#cascade-sort: each step of the cascade sort only breaks ties left unresolved by the step before it [css-cascade-sort#sort-importance css-cascade-sort#sort-attachment css-cascade-sort#sort-layer css-selector-basics#specificity css-selector-basics#specificity-order]
- [x] css-cascade-5.md#cascading-origins: in descending precedence the normal-weight origins are author, user, then user-agent [css-cascade-sort#origin-ua]
- [x] css-cascade-5.md#importance: marking a declaration !important inverts the origin order (important user-agent beats important user beats important author), and important declarations of every origin beat normal ones [css-cascade-sort#sort-importance css-layers#important]
- [x] css-cascade-5.md#preshint: a document language's non-CSS presentational hints are translated into cascade declarations in either the UA origin or a presentational-hint origin between user and author, as the host language defines (D34) [css-cascade-sort#presentation]
- [-] css-cascade-5.md#preshint: SVG maps its presentation attributes into the author origin, rather than the lower-precedence presentational-hint origin (D34) [css-cascade-sort#presentation]
- [x] css-cascade-5.md#cascade-specificity: a declaration's specificity is that of the style rule containing it; the higher-specificity declaration wins once origin, importance, attachment and layer are tied [css-selector-basics#specificity]
- [x] css-cascade-5.md#cascade-order: once origin, importance, attachment, layer and specificity are tied, the last declaration in document order wins [css-selector-basics#specificity-order]
- [x] css-cascade-5.md#cascade-order: declarations from a style attribute are ordered after every style-sheet declaration on that element [css-cascade-sort#sort-attachment]

## Cascade layers: css-layers

- [x] css-cascade-5.md#layering: declarations in a named or anonymous cascade layer cascade together without interleaving with declarations outside that layer; a declaration assigned no explicit layer is placed in an implicit final layer [css-layers#order css-layers#unlayered]
- [x] css-cascade-5.md#layer-ordering: layers are ordered by where they are first declared; nested layers are grouped inside their parent, and unlayered rules within a given parent come after all of that parent's explicit sub-layers [css-layers#nested css-layers#order]
- [x] css-cascade-5.md#cascading-origins: for normal declarations, a later layer in the layer order wins over an earlier one, so unlayered declarations (being last) win over every named layer [css-layers#order css-layers#unlayered]
- [x] css-cascade-5.md#cascading-origins: for important declarations the layer order is inverted and the earliest layer wins instead [css-layers#important]
- [x] css-cascade-5.md#at-ruledef-layer: @layer name { ... } assigns its rules to that named layer, reopening it if it already exists [css-layers#reopen css-layers#layer-block]
- [x] css-cascade-5.md#at-ruledef-layer: @layer name; declares a layer's position in the order without assigning any rules to it; @layer { ... } with no name creates a fresh anonymous layer each time [css-layers#statement css-layers#anonymous]
- [x] css-cascade-5.md#layer-names: nesting an @layer inside another concatenates their names with a dot (framework.base is distinct from a top-level base layer); two layer names denote the same layer only when every segment matches [css-layers#names css-layers#nested]

## CSS-wide keyword defaulting: css-revert, css-root-inherit, css-geometry-inherit (exists)

- [x] css-cascade-5.md#defaulting: when no declaration wins the cascade, the specified value comes from inheritance if the property is an inherited property, otherwise from the property's initial value [css-cascade-sort#no-cascade css-wide-keywords#unset-not-inherited]
- [x] css-cascade-5.md#initial: the initial keyword sets the specified value to the property's defined initial value [css-wide-keywords#initial css-wide-keywords#initial-stroke]
- [x] css-cascade-5.md#inherit: the inherit keyword sets the specified (and computed) value to the parent element's computed value; the root element, having no parent, inherits the property's initial value [css-wide-keywords#inherit css-wide-keywords#inherit-not-inherited css-root-inherit#root]
- [x] css-cascade-5.md#inherit-initial: the unset keyword acts as inherit on an inherited property and as initial otherwise, erasing any earlier cascade declarations for that property [css-wide-keywords#unset-inherited css-wide-keywords#unset-not-inherited]
- [x] css-cascade-5.md#default: the revert keyword rolls the cascaded value back to the next earlier origin (author rolls back to user, user rolls back to user-agent, user-agent reverting acts as unset) [css-revert#revert css-revert#revert-shorthand]
- [x] css-cascade-5.md#revert-layer: the revert-layer keyword rolls the cascaded value back to the previous cascade layer instead of the previous origin [css-revert#revert-layer css-revert#revert-layer-important css-revert#revert-layer-style]
- [x] css-cascade-5.md#revert-rule-keyword: the revert-rule keyword rolls the cascaded value back to what it would be if the current style rule were absent [css-revert#revert-rule]

## Notes
