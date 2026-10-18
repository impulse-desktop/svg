# CSS Syntax Module Level 3

## Escaping: css-escapes, css-idents (exists)

- [x] css-syntax-3.md#escape-codepoint: a backslash followed by any code point that is not a hex digit or newline is an escape that is replaced by that literal code point
- [x] css-syntax-3.md#escape-codepoint: a backslash followed by 1 to 6 hex digits (and then one optional whitespace) is an escape replaced by the Unicode code point of that hex value
- [x] css-syntax-3.md#consume-an-escaped-code-point: a hex escape of 0, a surrogate, or a value past the maximum allowed code point is replaced by U+FFFD instead of that value
- [x] css-syntax-3.md#consume-an-escaped-code-point: a backslash immediately followed by EOF is a parse error and is replaced by U+FFFD
- [x] css-syntax-3.md#consume-a-string-token: inside a quoted string, a backslash followed directly by a newline is a line continuation: both characters are consumed and nothing is appended
- [x] css-syntax-3.md#syntax-description: an identifier (a property name, at-rule name, class, id, or other keyword) may contain any code point, including ones CSS otherwise uses as syntax, by escaping it (D32)

## Stylesheet tokenizing and declaration recovery: css-sheet-recovery, css-sheet (exists)

- [x] css-syntax-3.md#consume-a-stylesheets-contents: at the top level of a stylesheet, the legacy HTML comment-hiding tokens <!-- and --> are discarded wherever they appear, not treated as errors
- [x] css-syntax-3.md#consume-comment: /* ... */ is skipped like whitespace between tokens; one left open at EOF is a parse error but still consumes to EOF rather than invalidating anything
- [x] css-syntax-3.md#style-rules: property names are ASCII case-insensitive, so FILL and fill name the same property
- [x] css-syntax-3.md#charset-rule: @charset is never a real rule when a stylesheet is parsed; any occurrence is dropped as an unrecognized at-rule
- [x] css-syntax-3.md#consume-the-remnants-of-a-bad-declaration: a declaration must start with an identifier followed by a colon; one that does not (a bare ":" or an identifier with no colon) is a bad declaration
- [x] css-syntax-3.md#consume-the-remnants-of-a-bad-declaration: a bad declaration is discarded up to the next ";" or the end of the block, without disturbing the declarations before or after it
- [x] css-syntax-3.md#style-rules: a declaration for an unrecognized property, or whose value does not match that property's grammar (such as an empty value), is invalid and is ignored; it does not invalidate the rest of the rule
- [x] css-syntax-3.md#consume-a-declaration: a declaration is marked important when its value ends with an (optionally whitespace-separated) "!" delimiter followed by an ASCII case-insensitive "important" identifier
- [x] css-syntax-3.md#consume-a-declaration: once a trailing "! important" is recognized, those two tokens are stripped from the declaration's value before it is matched against its property's grammar
- [x] css-syntax-3.md#consume-a-declaration: a trailing "!" followed by any word other than "important" is left in the value instead of being stripped, which almost always then fails the property's grammar and invalidates the declaration

## A rule's prelude or block left open at the stylesheet's end: css-sheet-recovery, css-sheet-at-rule-eof (exists), css-sheet-no-block (exists)

- [x] css-syntax-3.md#error-handling: if the stylesheet ends while a rule, declaration, function, string or similar construct is still open, everything open is automatically closed rather than discarded outright
- [x] css-syntax-3.md#consume-an-at-rule: reaching EOF while still in an at-rule's prelude or body ends the at-rule there; the at-rule is kept if it is otherwise valid in its context
- [x] css-syntax-3.md#consume-a-qualified-rule: reaching EOF while still in a qualified rule's prelude (before its block has even opened) is a parse error, and the whole rule is discarded, unlike an at-rule reaching EOF
- [x] css-syntax-3.md#error-handling: a qualified rule's prelude may contain any tokens, even ones that make its selector invalid; an invalid selector just matches nothing rather than breaking the rule's parsing

## A string, url() or block left open inside a rule: css-sheet-recovery, css-sheet-open-block (exists), css-sheet-unterminated (exists)

- [x] css-syntax-3.md#consume-a-string-token: a quoted string that reaches a newline or EOF before its closing quote is a parse error
- [x] css-syntax-3.md#consume-a-string-token: hitting a newline inside an unclosed string makes it a bad string, throwing out the declaration containing it; hitting EOF instead still returns it as a string, up to that point
- [x] css-syntax-3.md#consume-a-url-token: an unquoted url() that contains a stray quote or "(", or white space before more than its ")", is a parse error and becomes a bad-url token, which fails to match the url() grammar; one the end of the input reaches first is still a url
- [x] css-syntax-3.md#consume-a-block: a block's contents are consumed up to its own "}"; if the input ends first, the block (and everything nested in it) is simply closed where the input ends, per the general auto-closing rule

## The An+B microsyntax: css-nth, css-selectors (exists)

- [x] css-syntax-3.md#the-anb-type: An+B is written as a signed integer A, the letter n, and a signed integer B, in forms like An+B, -An-B, n, -n, odd or even, with whitespace allowed around the parts but not between a leading "+" and "n"
- [x] css-syntax-3.md#anb: odd means A=2, B=1; even means A=2, B=0; a bare integer means A=0, B=that integer
- [x] css-syntax-3.md#anb: An+B denotes the 1-indexed positions A*n+B for every integer n >= 0; negative A or B is allowed, but only positions this produces that are still positive count
- [x] css-syntax-3.md#anb: if both A and B are 0, the notation matches no position at all

## Notes
