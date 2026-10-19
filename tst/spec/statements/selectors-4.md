# Selectors Level 4

## Type and universal selectors: css-selector-basics, css-selectors (exists)

- [x] selectors-4.md#type-selectors: a type selector is a document-language element name and matches an instance of that element type [css-selector-basics#type]
- [x] selectors-4.md#the-universal-selector: the universal selector * matches an element of any type; adding it to a compound selector that already has another selector changes nothing (*.foo is the same as .foo) [css-selector-basics#universal]
- [x] selectors-4.md#structure: a compound selector may contain at most one type selector or universal selector, and it must be the first simple selector in the compound [css-selector-basics#compound-order]
- [x] selectors-4.md#case-sensitive: element, class, ID and attribute names and values are matched case-sensitively (identical-to) by default, as is normal for an XML-based document language, not ASCII case-insensitively [css-selector-basics#case-sensitive css-attribute-matchers#case-sensitive]

## Attribute selectors, every matcher and case flag: css-attribute-matchers, css-selectors (exists)

- [x] selectors-4.md#attribute-representation: [att] matches an element carrying that attribute regardless of value; [att=val] matches only when the value is exactly val [css-attribute-matchers#presence-equality]
- [x] selectors-4.md#attribute-representation: [att~=val] matches when the attribute's value, split on whitespace, contains val as one whole word; it never matches if val is empty or itself contains whitespace [css-attribute-matchers#word]
- [x] selectors-4.md#attribute-representation: [att|=val] matches when the value is exactly val, or begins with val immediately followed by "-" [css-attribute-matchers#dash]
- [x] selectors-4.md#attribute-substrings: [att^=val], [att$=val] and [att*=val] match a value that begins with, ends with, or contains val as a substring; all three never match when val is the empty string [css-attribute-matchers#substrings]
- [x] selectors-4.md#attribute-case: an attribute selector may end with " i" before its closing bracket to force ASCII case-insensitive value matching regardless of the document language [css-attribute-matchers#case-i css-attribute-matchers#case-flags]
- [x] selectors-4.md#attribute-case: an attribute selector may instead end with " s" to force exact case-sensitive value matching; the i/s flag letters are themselves case-insensitive [css-attribute-matchers#case-flags]
- [x] selectors-4.md#invalid: an attribute selector with no attribute name ([]), with an operator but no value after it ([att=]), or with an unrecognized operator or case flag, is a parse error and makes the whole selector invalid [css-attribute-matchers#invalid]

## Class and ID selectors: css-selector-basics, css-selectors (exists), css-classes-attributes (exists)

- [x] selectors-4.md#class-html: a class selector .name matches an element whose class attribute, split on whitespace, contains name as one whole word, equivalent to [class~=name] [css-selector-basics#class]
- [x] selectors-4.md#class-html: a class attribute that is missing, empty, or all whitespace has no words in it, so no class selector matches it [css-selector-basics#class-empty]
- [x] selectors-4.md#id-selectors: an ID selector #name matches an element whose ID-defining attribute equals name exactly; if more than one element shares that value, every one of them is matched [css-selector-basics#id]
- [x] selectors-4.md#invalid: a class or ID selector with nothing after the "." or "#" is a parse error and invalidates the whole selector [css-selector-basics#empty-name]

## Structural and child-index pseudo-classes: css-nth, css-structural-root (exists), css-structural-only (exists), css-structural-last (exists), css-selectors (exists)

- [x] selectors-4.md#the-root-pseudo: :root matches the element that is the root of the document [css-nth#root]
- [x] selectors-4.md#the-empty-pseudo: :empty matches an element with no children except, optionally, comments and processing instructions [css-nth#empty]
- [x] selectors-4.md#the-empty-pseudo: any child element, or any non-zero-length text or entity content (including whitespace-only text), makes an element not match :empty [css-nth#not-empty]
- [x] selectors-4.md#child-index: an element's position for the child-indexed pseudo-classes is 1-indexed among its inclusive siblings, counting only element nodes [css-nth#child-index css-nth#anb-keywords]
- [x] selectors-4.md#the-nth-child-pseudo: :nth-child(An+B [of S]) matches an element at sibling index An+B, for some integer n >= 0, among the inclusive siblings that match S (default the universal selector) [css-nth#anb-forms css-nth#nth-of]
- [x] selectors-4.md#the-nth-last-child-pseudo: :nth-last-child(An+B [of S]) counts the same way but from the last matching sibling backward [css-nth#nth-last-child css-nth#nth-of]
- [x] selectors-4.md#the-first-child-pseudo: :first-child is :nth-child(1); :last-child is :nth-last-child(1); :only-child matches an element that is both (has no siblings at all) [css-nth#first-last-only]
- [x] selectors-4.md#typed-child-index: :nth-of-type() and :nth-last-of-type() work like :nth-child()/:nth-last-child() but count only among inclusive siblings of the same element type and namespace, instead of a selector argument [css-nth#nth-of-type]
- [x] selectors-4.md#typed-child-index: :first-of-type, :last-of-type and :only-of-type are the first/last/both shorthands of the typed pseudo-classes, just as their untyped counterparts are [css-nth#typed-shorthands]

## The logical combinators :is(), :not(), :where(), :has(): css-logical-pseudo, css-selectors-more (exists), css-selectors (exists)

- [x] selectors-4.md#matches-pseudo: :is() takes a forgiving selector list: a component invalid in context is dropped rather than invalidating the whole pseudo-class; an empty resulting list is valid and matches nothing [css-logical-pseudo#is css-logical-pseudo#is-forgiving]
- [x] selectors-4.md#negation-pseudo: :not() takes a non-forgiving list of complex selectors and matches an element not represented by any of them; an invalid argument, or an empty argument list, makes the whole :not() (and so its selector) invalid [css-logical-pseudo#not css-logical-pseudo#not-invalid]
- [x] selectors-4.md#zero-matches: :where() has the same syntax and matching as :is(), but it and its arguments always contribute zero to specificity [css-logical-pseudo#where]
- [x] selectors-4.md#relational: :has() takes a list of relative selectors, each implicitly anchored to the element by a combinator, and matches if any one of them matches at least one element reached from it [css-logical-pseudo#has]
- [x] selectors-4.md#relational: :has() cannot be nested inside another :has() [css-logical-pseudo#has-nested]
- [x] selectors-4.md#negation-pseudo: pseudo-elements are not valid arguments to :is(), :not() or :has() [css-logical-pseudo#pseudo-element]

## Combinators: css-selector-basics, css-instance-structure (exists)

- [x] selectors-4.md#descendant-combinators: "A B" (whitespace) is the descendant combinator: it matches a B that is any descendant, at any depth, of some element matching A [css-selector-basics#descendant]
- [x] selectors-4.md#child-combinators: "A > B" is the child combinator: it matches a B whose parent matches A [css-selector-basics#child]
- [x] selectors-4.md#adjacent-sibling-combinators: "A + B" is the next-sibling combinator: it matches a B that is the very next sibling element after one matching A, ignoring non-element nodes between them [css-selector-basics#next-sibling]
- [x] selectors-4.md#general-sibling-combinators: "A ~ B" is the subsequent-sibling combinator: it matches a B preceded anywhere among its siblings by an element matching A, not necessarily immediately [css-selector-basics#subsequent-sibling]
- [x] selectors-4.md#invalid: a complex selector that opens with a combinator and has no selector before it (no implied anchor in that context) is invalid as a whole [css-selector-basics#leading-combinator]

## Specificity: css-selector-basics, css-logical-pseudo, css-specificity (exists)

- [x] selectors-4.md#specificity-rules: a selector's specificity is the triple (A, B, C): A is the number of ID selectors, B the number of class, attribute and pseudo-class selectors, C the number of type selectors and pseudo-elements [css-selector-basics#specificity]
- [x] selectors-4.md#specificity-rules: the universal selector counts toward none of A, B or C [css-selector-basics#specificity-universal css-selector-basics#universal]
- [x] selectors-4.md#specificity-rules: for a selector list, the specificity used for a given match is that of the most specific individual selector in the list that actually matched [css-selector-basics#specificity-list]
- [x] selectors-4.md#specificity-rules: two specificities are compared by A first, then B, then C; a tie on all three is resolved by order of appearance in the cascade instead [css-selector-basics#specificity css-selector-basics#specificity-order]
- [x] selectors-4.md#specificity-rules: :is(), :not() and :has() take on the specificity of their single most specific matching argument; :where() always contributes (0,0,0) [css-logical-pseudo#argument-specificity css-logical-pseudo#where]
- [x] selectors-4.md#specificity-rules: :nth-child() and :nth-last-child() add their own pseudo-class specificity plus that of their most specific "of S" argument, if any [css-logical-pseudo#nth-specificity]

## Link pseudo-classes: css-link-state

- [x] selectors-4.md#any-link-pseudo: :any-link matches an element acting as a hyperlink's source anchor (equivalent to :is(:link, :visited)); :link matches it while unvisited and :visited once visited, and the two are mutually exclusive (D33) [css-link-state#never css-link-state#valid]
- [x] selectors-4.md#link: a user agent may preserve privacy by always treating every hyperlink as unvisited, so that it always matches :link and never :visited; this is explicitly a conforming choice [css-link-state#never]

## The language pseudo-class :lang(): css-lang

- [x] selectors-4.md#lang-pseudo: :lang() takes a comma-separated list of one or more language ranges, each a CSS ident or string, and matches an element whose content language is one of them; it is a pseudo-class, of a class's specificity. [css-lang#filtering css-lang#wildcards css-lang#syntax css-lang#specificity]
- [x] selectors-4.md#the-lang-pseudo: the content language comes from the document language; XML languages take it from xml:lang. [css-lang#language css-lang#instance]
- [x] selectors-4.md#the-lang-pseudo: a language matches a range by extended filtering (RFC 4647 3.3.2), ASCII case-insensitively, after both are canonicalized and put in extlang form, which the renderer does without the registry, as written (D40). [css-lang#filtering css-lang#wildcards]
- [x] selectors-4.md#the-lang-pseudo: a range or a language that is not a well-formed tag (with * standing in for a subtag) matches nothing; non-ASCII ranges are ill-formed. [css-lang#ill-formed]
- [x] selectors-4.md#the-lang-pseudo: the range "*" matches no element whose language is not tagged (lang=""), but matches one tagged und; the range "" matches only elements whose language is not tagged. [css-lang#wildcards css-lang#untagged]
- [x] selectors-4.md#the-lang-pseudo: :lang() performs implicit wildcard matching, de-DE matching de-Latn-DE, and an initial * matches any primary language, *-CH matching de-CH and it-CH. [css-lang#filtering css-lang#wildcards]

## Notes

- selectors-4.md#invalid: for a dynamic pseudo-class never activated here (:hover, :active, :focus, :visited, :target), whether it is "unsupported" (invalidating its whole selector list) or just always-false is left open, and changes which rules in a mixed list apply; the renderer takes them as valid and never matching, :any-link and :link too (D33).
