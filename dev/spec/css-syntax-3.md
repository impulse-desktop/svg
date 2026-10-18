<!-- https://drafts.csswg.org/css-syntax-3/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Syntax Module Level 3

<a id="w3c-state"></a>Editor’s Draft, 16 October 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-syntax/
**Latest published version: **
: https://www.w3.org/TR/css-syntax-3/
**Previous Versions: **
: https://www.w3.org/TR/2026/CRD-css-syntax-3-20261001/
**Implementation Report: **
: https://wpt.fyi/results/css/css-syntax?label=experimental&label=master&aligned
**Feedback: **
: CSSWG Issues Repository
**Editors: **
: Tab Atkins Jr. (Google)
: Simon Sapin (Mozilla)
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/css-syntax/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

This module describes, in general terms, the basic structure and syntax of CSS stylesheets. It defines, in detail, the syntax and parsing of CSS - how to turn a stream of bytes into a meaningful stylesheet.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-syntax” in the title, like this: “[css-syntax] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This section is not normative.*

This module defines the abstract syntax and parsing of CSS stylesheets and other things which use CSS syntax (such as the HTML `style` attribute).

It defines algorithms for converting a stream of Unicode code points (in other words, text) into a stream of CSS tokens, and then further into CSS objects such as stylesheets, rules, and declarations.

### <a id="placement"></a>1.1. Module interactions

This module defines the syntax and parsing of CSS stylesheets. It supersedes the lexical scanner and grammar defined in CSS 2.1.

## <a id="syntax-description"></a>2. Description of CSS’s Syntax

*This section is not normative.*

A CSS document is a series of [style rules](#style-rule)—​which are [qualified rules](#qualified-rule) that apply styles to elements in a document—​and [at-rules](#at-rule)—​which define special processing rules or values for the CSS document.

A [qualified rule](#qualified-rule) starts with a prelude then has a {}-wrapped block containing a sequence of declarations. The meaning of the prelude varies based on the context that the rule appears in—​for [style rules](#style-rule), it’s a selector which specifies what elements the declarations will apply to. Each declaration has a name, followed by a colon and the declaration value. Declarations are separated by semicolons.

<a id="example-0ffd6a46"></a>

A typical rule might look something like this:

```
p > a {
  color: blue;
  text-decoration: underline;
}
```

In the above rule, "`p > a`" is the selector, which, if the source document is HTML, selects any `a` elements that are children of a `p` element.

"`color: blue`" is a declaration specifying that, for the elements that match the selector, their [color](css-color-4.md#propdef-color) property should have the value [blue](css-color-4.md#valdef-color-blue). Similarly, their text-decoration property should have the value underline.

[At-rules](#at-rule) are all different, but they have a basic structure in common. They start with an "@" code point followed by their name as a CSS keyword. Some at-rules are simple statements, with their name followed by more CSS values to specify their behavior, and finally ended by a semicolon. Others are blocks; they can have CSS values following their name, but they end with a {}-wrapped block, similar to a [qualified rule](#qualified-rule). Even the contents of these blocks are specific to the given at-rule: sometimes they contain a sequence of declarations, like a qualified rule; other times, they may contain additional blocks, or at-rules, or other structures altogether.

<a id="example-42888746"></a>

Here are several examples of [at-rules](#at-rule) that illustrate the varied syntax they may contain.

```
@import "my-styles.css";
```

The @import [at-rule](#at-rule) is a simple statement. After its name, it takes a single string or [url()](css-values-4.md#funcdef-url) function to indicate the stylesheet that it should import.

```
@page :left {
  margin-left: 4cm;
  margin-right: 3cm;
}
```

The @page [at-rule](#at-rule) consists of an optional page selector (the :left pseudoclass), followed by a block of properties that apply to the page when printed. In this way, it’s very similar to a normal style rule, except that its properties don’t apply to any "element", but rather the page itself.

```
@media print {
  body { font-size: 10pt }
}
```

The @media [at-rule](#at-rule) begins with a media type and a list of optional media queries. Its block contains entire rules, which are only applied when the @medias conditions are fulfilled.

Property names and [at-rule](#at-rule) names are always [ident sequences](#ident-sequence), which have to start with an [ident-start code point](#ident-start-code-point), two hyphens, or a hyphen followed by an ident-start code point, and then can contain zero or more [ident code points](#ident-code-point). You can include any code point at all, even ones that CSS uses in its syntax, by [escaping](#escape-codepoint) it.

The syntax of selectors is defined in the [Selectors spec](selectors-4.md). Similarly, the syntax of the wide variety of CSS values is defined in the Values & Units spec. The special syntaxes of individual [at-rules](#at-rule) can be found in the specs that define them.

### <a id="escaping"></a>2.1. Escaping

*This section is not normative.*

Any Unicode code point can be included in an [ident sequence](#ident-sequence) or quoted string by *<a id="escape-codepoint"></a>escaping* it. CSS escape sequences start with a backslash (\), and continue with:

- Any Unicode code point that is not a [hex digits](#hex-digit) or a [newline](#newline). The escape sequence is replaced by that code point.
- Or one to six [hex digits](#hex-digit), followed by an optional [whitespace](#whitespace). The escape sequence is replaced by the Unicode code point whose value is given by the hexadecimal digits. This optional whitespace allow hexadecimal escape sequences to be followed by "real" hex digits.
<a id="example-cc2d6518"></a> An [ident sequence](#ident-sequence) with the value "&B" could be written as \26 B or \000026B.
A "real" space after the escape sequence must be doubled.

### <a id="error-handling"></a>2.2. Error Handling

*This section is not normative.*

When errors occur in CSS, the parser attempts to recover gracefully, throwing away only the minimum amount of content before returning to parsing as normal. This is because errors aren’t always mistakes—​new syntax looks like an error to an old parser, and it’s useful to be able to add new syntax to the language without worrying about stylesheets that include it being completely broken in older UAs.

The precise error-recovery behavior is detailed in the parser itself, but it’s simple enough that a short description is fairly accurate.

- At the "top level" of a stylesheet, an [<at-keyword-token>](#typedef-at-keyword-token) starts an at-rule. Anything else starts a qualified rule, and is included in the rule’s prelude. This may produce an invalid selector, but that’s not the concern of the CSS parser—​at worst, it means the selector will match nothing.
- Once an at-rule starts, nothing is invalid from the parser’s standpoint; it’s all part of the at-rule’s prelude. Encountering a [<semicolon-token>](#typedef-semicolon-token) ends the at-rule immediately, while encountering an opening curly-brace [<{-token>](#tokendef-open-curly) starts the at-rule’s body. The at-rule seeks forward, matching blocks (content surrounded by (), {}, or []) until it finds a closing curly-brace [<}-token>](#tokendef-close-curly) that isn’t matched by anything else or inside of another block. The contents of the at-rule are then interpreted according to the at-rule’s own grammar.
- Qualified rules work similarly, except that semicolons don’t end them; instead, they are just taken in as part of the rule’s prelude. When the first {} block is found, the contents are always interpreted as a list of declarations.
- When interpreting a list of declarations, unknown syntax at any point causes the parser to throw away whatever declaration it’s currently building, and seek forward until it finds a semicolon (or the end of the block). It then starts fresh, trying to parse a declaration again.
- <a id="autoclosing"></a> If the stylesheet ends while any rule, declaration, function, string, etc. are still open, everything is automatically closed. This doesn’t make them invalid, though they may be incomplete and thus thrown away when they are verified against their grammar.
<a id="example-84a0b91f"></a> For example, the syntax of the [translateX()](css-transforms-1.md#funcdef-transform-translatex) function is:
```
translateX( <translation-value> )
```

However, the stylesheet may end with the function unclosed, like:
```
.foo { transform: translate(50px
```

The CSS parser parses this as a style rule containing one declaration, whose value is a function named "translate". This matches the above grammar, even though the ending token didn’t appear in the token stream, because by the time the parser is finished, the presence of the ending token is no longer possible to determine; all you have is the fact that there’s a block and a function.

After each construct (declaration, style rule, at-rule) is parsed, the user agent checks it against its expected grammar. If it does not match the grammar, it’s *<a id="css-invalid"></a>invalid*, and gets *<a id="css-ignored"></a>ignored* by the UA, which treats it as if it wasn’t there at all.

## <a id="tokenizing-and-parsing"></a>3. Tokenizing and Parsing CSS

User agents must use the parsing rules described in this specification to generate the [[CSSOM]](#biblio-cssom) trees from text/css resources. Together, these rules define what is referred to as the CSS parser.

This specification defines the parsing rules for CSS documents, whether they are syntactically correct or not. Certain points in the parsing algorithm are said to be *<a id="parse-error"></a>parse errors*. The error handling for parse errors is well-defined: user agents must either act as described below when encountering such problems, or must abort processing at the first error that they encounter for which they do not wish to apply the rules described below.

Conformance checkers must report at least one parse error condition to the user if one or more parse error conditions exist in the document and must not report parse error conditions if none exist in the document. Conformance checkers may report more than one parse error condition if more than one parse error condition exists in the document. Conformance checkers are not required to recover from parse errors, but if they do, they must recover in the same way as user agents.

### <a id="parsing-overview"></a>3.1. Overview of the Parsing Model

The input to the CSS parsing process consists of a stream of Unicode code points, which is passed through a tokenization stage followed by a tree construction stage. The output is a CSSStyleSheet object.

Note: Implementations that do not support scripting do not have to actually create a CSSOM CSSStyleSheet object, but the CSSOM tree in such cases is still used as the model for the rest of the specification.

### <a id="input-byte-stream"></a>3.2. The input byte stream

When parsing a stylesheet, the stream of Unicode code points that comprises the input to the tokenization stage might be initially seen by the user agent as a stream of bytes (typically coming over the network or from the local file system). If so, the user agent must decode these bytes into code points according to a particular character encoding.

To *<a id="css-decode-bytes"></a>decode* a *stylesheet*’s stream of bytes into a stream of code points:

1.
[Determine the fallback encoding](#determine-the-fallback-encoding) of *stylesheet*, and let *fallback* be the result.
2.
Decode *stylesheet*’s stream of bytes with fallback encoding *fallback*, and return the result.

Note: The decode algorithm gives precedence to a byte order mark (BOM), and only uses the fallback when none is found.

To *<a id="determine-the-fallback-encoding"></a>determine the fallback encoding* of a *stylesheet*:

1. If HTTP or equivalent protocol provides an *encoding label* (e.g. via the charset parameter of the Content-Type header) for the *stylesheet*, get an encoding from *encoding label*. If that does not return failure, return it.
2. Otherwise, check *stylesheet*’s byte stream. If the first 1024 bytes of the stream begin with the hex sequence
```
40 63 68 61 72 73 65 74 20 22 XX* 22 3B
```

where each `XX` byte is a value between 0_16 and 21_16 inclusive or a value between 23_16 and 7F_16 inclusive, then get an encoding from a string formed out of the sequence of `XX` bytes, interpreted as `ASCII`.
What does that byte sequence mean?
The byte sequence above, when decoded as ASCII, is the string "`@charset "…";`", where the "…" is the sequence of bytes corresponding to the encoding’s label.
If the return value was `utf-16be` or `utf-16le`, return `utf-8`; if it was anything else except failure, return it.
Why use utf-8 when the declaration says utf-16?
The bytes of the encoding declaration spell out “`@charset "…";`” in ASCII, but UTF-16 is not ASCII-compatible. Either you’ve typed in complete gibberish (like `䁣桡牳整•utf-16be∻`) to get the right bytes in the document, which we don’t want to encourage, or your document is actually in an ASCII-compatible encoding and your encoding declaration is lying.
Either way, defaulting to UTF-8 is a decent answer.
As well, this mimics the behavior of HTML’s `<meta charset>` attribute.
Note: Note that the syntax of an encoding declaration *looks like* the syntax of an [at-rule](#at-rule) named [@charset](#at-ruledef-charset), but no such rule actually exists, and the rules for how you can write it are much more restrictive than they would normally be for recognizing such a rule. A number of things you can do in CSS that would produce a valid @charset rule (if one existed), such as using multiple spaces, comments, or single quotes, will cause the encoding declaration to not be recognized. This behavior keeps the encoding declaration as simple as possible, and thus maximizes the likelihood of it being implemented correctly.
3. Otherwise, if an [environment encoding](#environment-encoding) is provided by the referring document, return it.
4. Otherwise, return `utf-8`.

Though UTF-8 is the default encoding for the web, and many newer web-based file formats assume or require UTF-8 encoding, CSS was created before it was clear which encoding would win, and thus can’t automatically assume the stylesheet is UTF-8.

Stylesheet authors *should* author their stylesheets in UTF-8, and ensure that either an HTTP header (or equivalent method) declares the encoding of the stylesheet to be UTF-8, or that the referring document declares its encoding to be UTF-8. (In HTML, this is done by adding a `<meta charset=utf-8>` element to the head of the document.)

If neither of these options are available, authors should begin the stylesheet with a UTF-8 BOM or the exact characters

```
@charset "utf-8";
```

Document languages that refer to CSS stylesheets that are decoded from bytes may define an *<a id="environment-encoding"></a>environment encoding* for each such stylesheet, which is used as a fallback when other encoding hints are not available or can not be used.

The concept of [environment encoding](#environment-encoding) only exists for compatibility with legacy content. New formats and new linking mechanisms **should not** provide an environment encoding, so the stylesheet defaults to UTF-8 instead in the absence of more explicit information.

Note: [[HTML]](#biblio-html) defines the environment encoding for `<link rel=stylesheet>`.

Note: [[CSSOM]](#biblio-cssom) defines the environment encoding for `<xml-stylesheet?>`.

Note: [[CSS-CASCADE-3]](#biblio-css-cascade-3) defines the environment encoding for `@import`.

### <a id="input-preprocessing"></a>3.3. Preprocessing the input stream

The *<a id="input-stream"></a>input stream* consists of the [filtered code points](#css-filter-code-points) pushed into it as the input byte stream is decoded.

To *<a id="css-filter-code-points"></a>filter code points* from a stream of (unfiltered) code points *input*:

- Replace any U+000D CARRIAGE RETURN (CR) code points, U+000C FORM FEED (FF) code points, or pairs of U+000D CARRIAGE RETURN (CR) followed by U+000A LINE FEED (LF) in *input* by a single U+000A LINE FEED (LF) code point.
- Replace any U+0000 NULL or surrogate code points in *input* with U+FFFD REPLACEMENT CHARACTER (�).
Note: The only way to produce a surrogate code point in CSS content is by directly assigning a `DOMString` with one in it via an OM operation.

## <a id="tokenization"></a>4. Tokenization

To *<a id="css-tokenize"></a>tokenize* a stream of code points into a stream of CSS tokens *input*, repeatedly [consume a token](#tokenizer-consume-a-token) from *input* until an [<EOF-token>](#typedef-eof-token) is reached, pushing each of the returned tokens into a stream.

Note: Each call to the [consume a token](#tokenizer-consume-a-token) algorithm returns a single token, so it can also be used "on-demand" to tokenize a stream of code points *during* parsing, if so desired.

The output of tokenization step is a stream of zero or more of the following tokens: *<a id="typedef-ident-token"></a><ident-token>*, *<a id="typedef-function-token"></a><function-token>*, *<a id="typedef-at-keyword-token"></a><at-keyword-token>*, *<a id="typedef-hash-token"></a><hash-token>*, *<a id="typedef-string-token"></a><string-token>*, *<a id="typedef-bad-string-token"></a><bad-string-token>*, *<a id="typedef-url-token"></a><url-token>*, *<a id="typedef-bad-url-token"></a><bad-url-token>*, *<a id="typedef-delim-token"></a><delim-token>*, *<a id="typedef-number-token"></a><number-token>*, *<a id="typedef-percentage-token"></a><percentage-token>*, *<a id="typedef-dimension-token"></a><dimension-token>*, *<a id="typedef-unicode-range-token"></a><unicode-range-token>*, *<a id="typedef-whitespace-token"></a><whitespace-token>*, *<a id="typedef-cdo-token"></a><CDO-token>*, *<a id="typedef-cdc-token"></a><CDC-token>*, *<a id="typedef-colon-token"></a><colon-token>*, *<a id="typedef-semicolon-token"></a><semicolon-token>*, *<a id="typedef-comma-token"></a><comma-token>*, *<a id="tokendef-open-square"></a><[-token>*, *<a id="tokendef-close-square"></a><]-token>*, *<a id="tokendef-open-paren"></a><(-token>*, *<a id="tokendef-close-paren"></a><)-token>*, *<a id="tokendef-open-curly"></a><{-token>*, and *<a id="tokendef-close-curly"></a><}-token>*.

- [<ident-token>](#typedef-ident-token), [<function-token>](#typedef-function-token), [<at-keyword-token>](#typedef-at-keyword-token), [<hash-token>](#typedef-hash-token), [<string-token>](#typedef-string-token), and [<url-token>](#typedef-url-token) have a value composed of zero or more code points. Additionally, hash tokens have a type flag set to either "id" or "unrestricted". The type flag defaults to "unrestricted" if not otherwise set.
- [<delim-token>](#typedef-delim-token) has a value composed of a single code point.
- [<number-token>](#typedef-number-token), [<percentage-token>](#typedef-percentage-token), and [<dimension-token>](#typedef-dimension-token) have a numeric value, and an optional sign character set to either "+" or "-" (or nothing).
[<number-token>](#typedef-number-token) and [<dimension-token>](#typedef-dimension-token) additionally have a type flag set to either "integer" or "number". The type flag defaults to "integer" if not otherwise set. <dimension-token> additionally have a unit composed of one or more code points.
- [<unicode-range-token>](#typedef-unicode-range-token) has a starting and ending code point. It represents an inclusive range of codepoints (including both the start and end). If the ending code point is before the starting code point, it represents an empty range.

Note: The type flag of hash tokens is used in the Selectors syntax [[SELECT]](#biblio-select). Only hash tokens with the "id" type are valid [ID selectors](selectors-4.md#id-selectors).

### <a id="token-diagrams"></a>4.1. Token Railroad Diagrams

*This section is non-normative.*

This section presents an informative view of the tokenizer, in the form of railroad diagrams. Railroad diagrams are more compact than an explicit parser, but often easier to read than a regular expression.

These diagrams are *informative* and *incomplete*; they describe the grammar of "correct" tokens, but do not describe error-handling at all. They are provided solely to make it easier to get an intuitive grasp of the syntax of each token.

Diagrams with names such as *<foo-token>* represent tokens. The rest are productions referred to by other diagrams.

**<a id="comment-diagram"></a>comment **
:

**<a id="newline-diagram"></a>newline **
:

**<a id="whitespace-diagram"></a>whitespace **
:

**<a id="hex-digit-diagram"></a>hex digit **
:

**<a id="escape-diagram"></a>escape **
:

**<a id="whitespace-token-diagram"></a>[<whitespace-token>](#typedef-whitespace-token) **
:

**<a id="ws*-diagram"></a>ws* **
:

**<a id="ident-token-diagram"></a>[<ident-token>](#typedef-ident-token) **
:

**<a id="function-token-diagram"></a>[<function-token>](#typedef-function-token) **
:

**<a id="at-keyword-token-diagram"></a>[<at-keyword-token>](#typedef-at-keyword-token) **
:

**<a id="hash-token-diagram"></a>[<hash-token>](#typedef-hash-token) **
:

**<a id="string-token-diagram"></a>[<string-token>](#typedef-string-token) **
:

**<a id="url-token-diagram"></a>[<url-token>](#typedef-url-token) **
:

**<a id="number-token-diagram"></a>[<number-token>](#typedef-number-token) **
:

**<a id="dimension-token-diagram"></a>[<dimension-token>](#typedef-dimension-token) **
:

**<a id="percentage-token-diagram"></a>[<percentage-token>](#typedef-percentage-token) **
:

**<a id="CDO-token-diagram"></a>[<CDO-token>](#typedef-cdo-token) **
:

**<a id="CDC-token-diagram"></a>[<CDC-token>](#typedef-cdc-token) **
:

**<a id="unicode-range-token-diagram"></a>[<unicode-range-token>](#typedef-unicode-range-token) **
:

### <a id="tokenizer-definitions"></a>4.2. Definitions

This section defines several terms used during the tokenization phase.

***<a id="next-input-code-point"></a>next input code point* **
: The first code point in the [input stream](#input-stream) that has not yet been consumed.
***<a id="current-input-code-point"></a>current input code point* **
: The last code point to have been consumed.
***<a id="reconsume-the-current-input-code-point"></a>reconsume the current input code point* **
: Push the [current input code point](#current-input-code-point) back onto the front of the [input stream](#input-stream), so that the next time you are instructed to consume the [next input code point](#next-input-code-point), it will instead reconsume the current input code point.
***<a id="eof-code-point"></a>EOF code point* **
: A conceptual code point representing the end of the [input stream](#input-stream). Whenever the input stream is empty, the [next input code point](#next-input-code-point) is always an EOF code point.
***<a id="digit"></a>digit* **
: A code point between U+0030 DIGIT ZERO (0) and U+0039 DIGIT NINE (9) inclusive.
***<a id="hex-digit"></a>hex digit* **
: A [digit](#digit), or a code point between U+0041 LATIN CAPITAL LETTER A (A) and U+0046 LATIN CAPITAL LETTER F (F) inclusive, or a code point between U+0061 LATIN SMALL LETTER A (a) and U+0066 LATIN SMALL LETTER F (f) inclusive.
***<a id="uppercase-letter"></a>uppercase letter* **
: A code point between U+0041 LATIN CAPITAL LETTER A (A) and U+005A LATIN CAPITAL LETTER Z (Z) inclusive.
***<a id="lowercase-letter"></a>lowercase letter* **
: A code point between U+0061 LATIN SMALL LETTER A (a) and U+007A LATIN SMALL LETTER Z (z) inclusive.
***<a id="letter"></a>letter* **
: An [uppercase letter](#uppercase-letter) or a [lowercase letter](#lowercase-letter).
***<a id="non-ascii-ident-code-point"></a>non-ASCII ident code point* **
: A code point whose value is any of:

-
U+00B7
-
between U+00C0 and U+00D6
-
between U+00D8 and U+00F6
-
between U+00F8 and U+037D
-
between U+037F and U+1FFF
-
U+200C
-
U+200D
-
U+203F
-
U+2040
-
between U+2070 and U+218F
-
between U+2C00 and U+2FEF
-
between U+3001 and U+D7FF
-
between U+F900 and U+FDCF
-
between U+FDF0 and U+FFFD
-
greater than or equal to U+10000

All of these ranges are inclusive.

Why these character, specifically?

This matches the list of non-ASCII codepoints allowed to be used in HTML valid custom element names. It excludes a number of characters that appear as whitespace, or that can cause rendering or parsing issues in some tools, such as the direction override codepoints.

Note that this is a weaker set of restrictions than UAX 31 recommends for identifiers (used by languages such as JavaScript to restrict their identifier syntax), allowing things such as starting an identifier with a combining character. Consistency with HTML custom element names (and thus, the ability to write selectors for all custom elements without having to use escapes) was considered valuable, and the set of characters restricted by HTML covers the "high value" restrictions well.

These restrictions do not avoid all possible confusing renderings; mixing characters from LTR and RTL scripts can still result in unexpected visual transposition in most text editors, for example. Source text can contain the restricted characters in non-ident contexts, as well: most of them are completely valid in strings, for example. Even when used in a way that creates invalid CSS, the parsing errors they cause might be limited to something unimportant, while their effect on rendering the source text in code review tools might be significant and/or malicious. For more details on these sorts of "source text attacks", see this Rust-lang blog post (archived).

***<a id="ident-start-code-point"></a><a id="identifier-start-code-point"></a><a id="name-start-code-point"></a>ident-start code point* **
: A [letter](#letter), a [non-ASCII ident code point](#non-ascii-ident-code-point), or U+005F LOW LINE (_).
***<a id="ident-code-point"></a><a id="identifier-code-point"></a><a id="name-code-point"></a>ident code point* **
: An [ident-start code point](#ident-start-code-point), a [digit](#digit), or U+002D HYPHEN-MINUS (-).
***<a id="non-printable-code-point"></a>non-printable code point* **
: A code point between U+0000 NULL and U+0008 BACKSPACE inclusive, or U+000B LINE TABULATION, or a code point between U+000E SHIFT OUT and U+001F INFORMATION SEPARATOR ONE inclusive, or U+007F DELETE.
***<a id="newline"></a>newline* **
: U+000A LINE FEED. Note that U+000D CARRIAGE RETURN and U+000C FORM FEED are not included in this definition, as they are converted to U+000A LINE FEED during [preprocessing](#input-preprocessing).
***<a id="whitespace"></a>whitespace* **
: A [newline](#newline), U+0009 CHARACTER TABULATION, or U+0020 SPACE.
***<a id="maximum-allowed-code-point"></a>maximum allowed code point* **
: The greatest code point defined by Unicode: U+10FFFF.
***<a id="ident-sequence"></a><a id="identifier"></a><a id="css-identifier"></a>ident sequence* **
: A sequence of code points that has the same syntax as an [<ident-token>](#typedef-ident-token).

Note: The part of an [<at-keyword-token>](#typedef-at-keyword-token) after the "@", the part of a [<hash-token>](#typedef-hash-token) (with the "id" type flag) after the "#", the part of a [<function-token>](#typedef-function-token) before the "(", and the unit of a [<dimension-token>](#typedef-dimension-token) are all [ident sequences](#ident-sequence).

### <a id="tokenizer-algorithms"></a>4.3. Tokenizer Algorithms

The algorithms defined in this section transform a stream of code points into a stream of tokens.

#### <a id="consume-token"></a>4.3.1. Consume a token

This section describes how to *<a id="tokenizer-consume-a-token"></a>consume a token* from a stream of code points. It additionally takes an optional boolean *unicode ranges allowed*, defaulting to false. It will return a single token of any type.

[Consume comments](#consume-comments).

Consume the [next input code point](#next-input-code-point).

**[whitespace](#whitespace) **
: Consume as much [whitespace](#whitespace) as possible. Return a [<whitespace-token>](#typedef-whitespace-token).
**U+0022 QUOTATION MARK (") **
: [Consume a string token](#consume-a-string-token) and return it.
**U+0023 NUMBER SIGN (#) **
: If the [next input code point](#next-input-code-point) is an [ident code point](#ident-code-point) or the next two input code points [are a valid escape](#check-if-two-code-points-are-a-valid-escape), then:

1. Create a [<hash-token>](#typedef-hash-token).
2. If the [next 3 input code points](#next-input-code-point) [would start an ident sequence](#check-if-three-code-points-would-start-an-ident-sequence), set the [<hash-token>](#typedef-hash-token)’s type flag to "id".
3. [Consume an ident sequence](#consume-an-ident-sequence), and set the [<hash-token>](#typedef-hash-token)’s value to the returned string.
4. Return the [<hash-token>](#typedef-hash-token).

Otherwise, return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+0027 APOSTROPHE (') **
: [Consume a string token](#consume-a-string-token) and return it.
**U+0028 LEFT PARENTHESIS (() **
: Return a [<(-token>](#tokendef-open-paren).
**U+0029 RIGHT PARENTHESIS ()) **
: Return a [<)-token>](#tokendef-close-paren).
**U+002B PLUS SIGN (+) **
: If the input stream [starts with a number](#check-if-three-code-points-would-start-a-number), [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume a numeric token](#consume-a-numeric-token), and return it.

Otherwise, return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+002C COMMA (,) **
: Return a [<comma-token>](#typedef-comma-token).
**U+002D HYPHEN-MINUS (-) **
: If the input stream [starts with a number](#check-if-three-code-points-would-start-a-number), [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume a numeric token](#consume-a-numeric-token), and return it.

Otherwise, if the [next 2 input code points](#next-input-code-point) are U+002D HYPHEN-MINUS U+003E GREATER-THAN SIGN (->), consume them and return a [<CDC-token>](#typedef-cdc-token).

Otherwise, if the input stream [starts with an ident sequence](#check-if-three-code-points-would-start-an-ident-sequence), [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume an ident-like token](#consume-an-ident-like-token), and return it.

Otherwise, return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+002E FULL STOP (.) **
: If the input stream [starts with a number](#check-if-three-code-points-would-start-a-number), [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume a numeric token](#consume-a-numeric-token), and return it.

Otherwise, return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+003A COLON (:) **
: Return a [<colon-token>](#typedef-colon-token).
**U+003B SEMICOLON (;) **
: Return a [<semicolon-token>](#typedef-semicolon-token).
**U+003C LESS-THAN SIGN (<) **
: If the [next 3 input code points](#next-input-code-point) are U+0021 EXCLAMATION MARK U+002D HYPHEN-MINUS U+002D HYPHEN-MINUS (!--), consume them and return a [<CDO-token>](#typedef-cdo-token).

Otherwise, return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+0040 COMMERCIAL AT (@) **
: If the [next 3 input code points](#next-input-code-point) [would start an ident sequence](#check-if-three-code-points-would-start-an-ident-sequence), [consume an ident sequence](#consume-an-ident-sequence), create an [<at-keyword-token>](#typedef-at-keyword-token) with its value set to the returned value, and return it.

Otherwise, return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+005B LEFT SQUARE BRACKET ([) **
: Return a [<[-token>](#tokendef-open-square).
**U+005C REVERSE SOLIDUS (\) **
: If the input stream [starts with a valid escape](#check-if-two-code-points-are-a-valid-escape), [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume an ident-like token](#consume-an-ident-like-token), and return it.

Otherwise, this is a [parse error](#parse-error). Return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).

**U+005D RIGHT SQUARE BRACKET (]) **
: Return a [<]-token>](#tokendef-close-square).
**U+007B LEFT CURLY BRACKET ({) **
: Return a [<{-token>](#tokendef-open-curly).
**U+007D RIGHT CURLY BRACKET (}) **
: Return a [<}-token>](#tokendef-close-curly).
**[digit](#digit) **
: [Reconsume the current input code point](#reconsume-the-current-input-code-point), [consume a numeric token](#consume-a-numeric-token), and return it.
**U+0055 LATIN CAPITAL LETTER U (U) **
**u+0075 LATIN LOWERCASE LETTER U (u) **
: If *unicode ranges allowed* is true and the input stream [would start a unicode-range](#check-if-three-code-points-would-start-a-unicode-range), [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume a unicode-range token](#consume-a-unicode-range-token), and return it.

Otherwise, [reconsume the current input code point](#reconsume-the-current-input-code-point), [consume an ident-like token](#consume-an-ident-like-token), and return it.

**[ident-start code point](#ident-start-code-point) **
: [Reconsume the current input code point](#reconsume-the-current-input-code-point), [consume an ident-like token](#consume-an-ident-like-token), and return it.
**EOF **
: Return an [<EOF-token>](#typedef-eof-token).
**anything else **
: Return a [<delim-token>](#typedef-delim-token) with its value set to the [current input code point](#current-input-code-point).
#### <a id="consume-comment"></a>4.3.2. Consume comments

This section describes how to *<a id="consume-comments"></a>consume comments* from a stream of code points. It returns nothing.

If the [next two input code point](#next-input-code-point) are U+002F SOLIDUS (/) followed by a U+002A ASTERISK (*), consume them and all following code points up to and including the first U+002A ASTERISK (*) followed by a U+002F SOLIDUS (/), or up to an EOF code point. Return to the start of this step.

If the preceding paragraph ended by consuming an EOF code point, this is a [parse error](#parse-error).

Return nothing.

#### <a id="consume-numeric-token"></a>4.3.3. Consume a numeric token

This section describes how to *<a id="consume-a-numeric-token"></a>consume a numeric token* from a stream of code points. It returns either a [<number-token>](#typedef-number-token), [<percentage-token>](#typedef-percentage-token), or [<dimension-token>](#typedef-dimension-token).

[Consume a number](#consume-a-number) and let *number* be the result.

If the [next 3 input code points](#next-input-code-point) [would start an ident sequence](#check-if-three-code-points-would-start-an-ident-sequence), then:

1. Create a [<dimension-token>](#typedef-dimension-token) with the same value, type flag, and sign character as *number*, and a unit set initially to the empty string.
2. [Consume an ident sequence](#consume-an-ident-sequence). Set the [<dimension-token>](#typedef-dimension-token)’s unit to the returned value.
3. Return the [<dimension-token>](#typedef-dimension-token).

Otherwise, if the [next input code point](#next-input-code-point) is U+0025 PERCENTAGE SIGN (%), consume it. Create a [<percentage-token>](#typedef-percentage-token) with the same value and sign character as *number*, and return it.

Otherwise, create a [<number-token>](#typedef-number-token) with the same value, type flag, and sign character as *number*, and return it.

#### <a id="consume-ident-like-token"></a>4.3.4. Consume an ident-like token

This section describes how to *<a id="consume-an-ident-like-token"></a>consume an ident-like token* from a stream of code points. It returns an [<ident-token>](#typedef-ident-token), [<function-token>](#typedef-function-token), [<url-token>](#typedef-url-token), or [<bad-url-token>](#typedef-bad-url-token).

[Consume an ident sequence](#consume-an-ident-sequence), and let *string* be the result.

If *string*’s value is an ASCII case-insensitive match for "url", and the [next input code point](#next-input-code-point) is U+0028 LEFT PARENTHESIS ((), consume it. While the next two input code points are [whitespace](#whitespace), consume the next input code point. If the next one or two input code points are U+0022 QUOTATION MARK ("), U+0027 APOSTROPHE ('), or whitespace followed by U+0022 QUOTATION MARK (") or U+0027 APOSTROPHE ('), then create a [<function-token>](#typedef-function-token) with its value set to *string* and return it. Otherwise, [consume a url token](#consume-a-url-token), and return it.

Otherwise, if the [next input code point](#next-input-code-point) is U+0028 LEFT PARENTHESIS ((), consume it. Create a [<function-token>](#typedef-function-token) with its value set to *string* and return it.

Otherwise, create an [<ident-token>](#typedef-ident-token) with its value set to *string* and return it.

#### <a id="consume-string-token"></a>4.3.5. Consume a string token

This section describes how to *<a id="consume-a-string-token"></a>consume a string token* from a stream of code points. It returns either a [<string-token>](#typedef-string-token) or [<bad-string-token>](#typedef-bad-string-token).

This algorithm may be called with an *ending code point*, which denotes the code point that ends the string. If an *ending code point* is not specified, the [current input code point](#current-input-code-point) is used.

Initially create a [<string-token>](#typedef-string-token) with its value set to the empty string.

Repeatedly consume the [next input code point](#next-input-code-point) from the stream:

***ending code point* **
: Return the [<string-token>](#typedef-string-token).
**EOF **
: This is a [parse error](#parse-error). Return the [<string-token>](#typedef-string-token).
**[newline](#newline) **
: This is a [parse error](#parse-error). [Reconsume the current input code point](#reconsume-the-current-input-code-point), create a [<bad-string-token>](#typedef-bad-string-token), and return it.
**U+005C REVERSE SOLIDUS (\) **
: If the [next input code point](#next-input-code-point) is EOF, do nothing.

Otherwise, if the [next input code point](#next-input-code-point) is a newline, consume it.

Otherwise, (the stream [starts with a valid escape](#check-if-two-code-points-are-a-valid-escape)) [consume an escaped code point](#consume-an-escaped-code-point) and append the returned code point to the [<string-token>](#typedef-string-token)’s value.

**anything else **
: Append the [current input code point](#current-input-code-point) to the [<string-token>](#typedef-string-token)’s value.
#### <a id="consume-url-token"></a>4.3.6. Consume a url token

This section describes how to *<a id="consume-a-url-token"></a>consume a url token* from a stream of code points. It returns either a [<url-token>](#typedef-url-token) or a [<bad-url-token>](#typedef-bad-url-token).

Note: This algorithm assumes that the initial "url(" has already been consumed. This algorithm also assumes that it’s being called to consume an "unquoted" value, like url(foo). A quoted value, like url("foo"), is parsed as a [<function-token>](#typedef-function-token). [Consume an ident-like token](#consume-an-ident-like-token) automatically handles this distinction; this algorithm shouldn’t be called directly otherwise.

1. Initially create a [<url-token>](#typedef-url-token) with its value set to the empty string.
2. Consume as much [whitespace](#whitespace) as possible.
3. Repeatedly consume the [next input code point](#next-input-code-point) from the stream:
**U+0029 RIGHT PARENTHESIS ()) **
: Return the [<url-token>](#typedef-url-token).
**EOF **
: This is a [parse error](#parse-error). Return the [<url-token>](#typedef-url-token).
**[whitespace](#whitespace) **
: Consume as much [whitespace](#whitespace) as possible. If the [next input code point](#next-input-code-point) is U+0029 RIGHT PARENTHESIS ()) or EOF, consume it and return the [<url-token>](#typedef-url-token) (if EOF was encountered, this is a [parse error](#parse-error)); otherwise, [consume the remnants of a bad url](#consume-the-remnants-of-a-bad-url), create a [<bad-url-token>](#typedef-bad-url-token), and return it.
**U+0022 QUOTATION MARK (") **
**U+0027 APOSTROPHE (') **
**U+0028 LEFT PARENTHESIS (() **
**[non-printable code point](#non-printable-code-point) **
: This is a [parse error](#parse-error). [Consume the remnants of a bad url](#consume-the-remnants-of-a-bad-url), create a [<bad-url-token>](#typedef-bad-url-token), and return it.
**U+005C REVERSE SOLIDUS (\) **
: If the stream [starts with a valid escape](#check-if-two-code-points-are-a-valid-escape), [consume an escaped code point](#consume-an-escaped-code-point) and append the returned code point to the [<url-token>](#typedef-url-token)’s value.
Otherwise, this is a [parse error](#parse-error). [Consume the remnants of a bad url](#consume-the-remnants-of-a-bad-url), create a [<bad-url-token>](#typedef-bad-url-token), and return it.
**anything else **
: Append the [current input code point](#current-input-code-point) to the [<url-token>](#typedef-url-token)’s value.

#### <a id="consume-escaped-code-point"></a>4.3.7. Consume an escaped code point

This section describes how to *<a id="consume-an-escaped-code-point"></a>consume an escaped code point*. It assumes that the U+005C REVERSE SOLIDUS (\) has already been consumed and that the next input code point has already been verified to be part of a valid escape. It will return a code point.

Consume the [next input code point](#next-input-code-point).

**[hex digit](#hex-digit) **
: Consume as many [hex digits](#hex-digit) as possible, but no more than 5. Note that this means 1-6 hex digits have been consumed in total. If the [next input code point](#next-input-code-point) is [whitespace](#whitespace), consume it as well. Interpret the hex digits as a hexadecimal number. If this number is zero, or is for a surrogate, or is greater than the [maximum allowed code point](#maximum-allowed-code-point), return U+FFFD REPLACEMENT CHARACTER (�). Otherwise, return the code point with that value.
**EOF **
: This is a [parse error](#parse-error). Return U+FFFD REPLACEMENT CHARACTER (�).
**anything else **
: Return the [current input code point](#current-input-code-point).
#### <a id="starts-with-a-valid-escape"></a>4.3.8. Check if two code points are a valid escape

This section describes how to *<a id="check-if-two-code-points-are-a-valid-escape"></a>check if two code points are a valid escape*. The algorithm described here can be called explicitly with two code points, or can be called with the input stream itself. In the latter case, the two code points in question are the [current input code point](#current-input-code-point) and the [next input code point](#next-input-code-point), in that order.

Note: This algorithm will not consume any additional code point.

If the first code point is not U+005C REVERSE SOLIDUS (\), return false.

Otherwise, if the second code point is a [newline](#newline), return false.

Otherwise, return true.

#### <a id="would-start-an-identifier"></a>4.3.9. Check if three code points would start an ident sequence

This section describes how to *<a id="check-if-three-code-points-would-start-an-ident-sequence"></a><a id="check-if-three-code-points-would-start-an-identifier"></a>check if three code points would start an [ident sequence](#ident-sequence)*. The algorithm described here can be called explicitly with three code points, or can be called with the input stream itself. In the latter case, the three code points in question are the [current input code point](#current-input-code-point) and the [next two input code points](#next-input-code-point), in that order.

Note: This algorithm will not consume any additional code points.

Look at the first code point:

**U+002D HYPHEN-MINUS **
: If the second code point is an [ident-start code point](#ident-start-code-point) or a U+002D HYPHEN-MINUS, or the second and third code points [are a valid escape](#check-if-two-code-points-are-a-valid-escape), return true. Otherwise, return false.
**[ident-start code point](#ident-start-code-point) **
: Return true.
**U+005C REVERSE SOLIDUS (\) **
: If the first and second code points [are a valid escape](#check-if-two-code-points-are-a-valid-escape), return true. Otherwise, return false.
**anything else **
: Return false.
#### <a id="starts-with-a-number"></a>4.3.10. Check if three code points would start a number

This section describes how to *<a id="check-if-three-code-points-would-start-a-number"></a>check if three code points would start a number*. The algorithm described here can be called explicitly with three code points, or can be called with the input stream itself. In the latter case, the three code points in question are the [current input code point](#current-input-code-point) and the [next two input code points](#next-input-code-point), in that order.

Note: This algorithm will not consume any additional code points.

Look at the first code point:

**U+002B PLUS SIGN (+) **
**U+002D HYPHEN-MINUS (-) **
: If the second code point is a [digit](#digit), return true.

Otherwise, if the second code point is a U+002E FULL STOP (.) and the third code point is a [digit](#digit), return true.

Otherwise, return false.

**U+002E FULL STOP (.) **
: If the second code point is a [digit](#digit), return true. Otherwise, return false.
**[digit](#digit) **
: Return true.
**anything else **
: Return false.
#### <a id="starts-a-unicode-range"></a>4.3.11. Check if three code points would start a unicode-range

This section describes how to *<a id="check-if-three-code-points-would-start-a-unicode-range"></a>check if three code points would start a unicode-range*. The algorithm described here can be called explicitly with three code points, or can be called with the input stream itself. In the latter case, the three code points in question are the [current input code point](#current-input-code-point) and the [next two input code points](#next-input-code-point), in that order.

Note: This algorithm will not consume any additional code points.

If all of the following are true:

-
The first code point is either U+0055 LATIN CAPITAL LETTER U (U) or U+0075 LATIN SMALL LETTER U (u)
-
The second code point is U+002B PLUS SIGN (+).
-
The third code point is either U+003F QUESTION MARK (?) or a [hex digit](#hex-digit)

then return true.

Otherwise return false.

#### <a id="consume-name"></a>4.3.12. Consume an ident sequence

This section describes how to *<a id="consume-an-ident-sequence"></a><a id="consume-an-identifier"></a>consume an ident sequence* from a stream of code points. It returns a string containing the largest name that can be formed from adjacent code points in the stream, starting from the first.

Note: This algorithm does not do the verification of the first few code points that are necessary to ensure the returned code points would constitute an [<ident-token>](#typedef-ident-token). If that is the intended use, ensure that the stream [starts with an ident sequence](#check-if-three-code-points-would-start-an-ident-sequence) before calling this algorithm.

Let *result* initially be an empty string.

Repeatedly consume the [next input code point](#next-input-code-point) from the stream:

**[ident code point](#ident-code-point) **
: Append the code point to *result*.
**the stream [starts with a valid escape](#check-if-two-code-points-are-a-valid-escape) **
: [Consume an escaped code point](#consume-an-escaped-code-point). Append the returned code point to *result*.
**anything else **
: [Reconsume the current input code point](#reconsume-the-current-input-code-point). Return *result*.
#### <a id="consume-number"></a>4.3.13. Consume a number

This section describes how to *<a id="consume-a-number"></a>consume a number* from a stream of code points. It returns a numeric *value*, a string *type* which is either "integer" or "number", and an optional *sign character* which is either "+", "-", or missing.

Note: This algorithm does not do the verification of the first few code points that are necessary to ensure a number can be obtained from the stream. Ensure that the stream [starts with a number](#check-if-three-code-points-would-start-a-number) before calling this algorithm.

Execute the following steps in order:

1. Let *type* be the string "integer". Let *number part* and *exponent part* be the empty string.
2. If the [next input code point](#next-input-code-point) is U+002B PLUS SIGN (+) or U+002D HYPHEN-MINUS (-), consume it. Append it to *number part* and set *sign character* to it.
3. While the [next input code point](#next-input-code-point) is a [digit](#digit), consume it and append it to *number part*.
4. If the [next 2 input code points](#next-input-code-point) are U+002E FULL STOP (.) followed by a [digit](#digit), then:
  1. Consume the [next input code point](#next-input-code-point) and append it to *number part*.
  2. While the [next input code point](#next-input-code-point) is a [digit](#digit), consume it and append it to *number part*.
  3. Set *type* to "number".
5. If the [next 2 or 3 input code points](#next-input-code-point) are U+0045 LATIN CAPITAL LETTER E (E) or U+0065 LATIN SMALL LETTER E (e), optionally followed by U+002D HYPHEN-MINUS (-) or U+002B PLUS SIGN (+), followed by a [digit](#digit), then:
  1. Consume the [next input code point](#next-input-code-point).
  2. If the [next input code point](#next-input-code-point) is "+" or "-", consume it and append it to *exponent part*.
  3. While the [next input code point](#next-input-code-point) is a [digit](#digit), consume it and append it to *exponent part*.
  4. Set *type* to "number".
6. Let *value* be the result of interpreting *number part* as a base-10 number.
If *exponent part* is non-empty, interpret it as a base-10 integer, then raise 10 to the power of the result, multiply it by *value*, and set *value* to that result.
7. Return *value*, *type*, and *sign character*.

#### <a id="consume-unicode-range-token"></a>4.3.14. Consume a unicode-range token

This section describes how to *<a id="consume-a-unicode-range-token"></a>consume a unicode-range token* from a stream of code points. It returns a [<unicode-range-token>](#typedef-unicode-range-token).

Note: This algorithm does not do the verification of the first few code points that are necessary to ensure the returned code points would constitute an [<unicode-range-token>](#typedef-unicode-range-token). Ensure that the stream [would start a unicode-range](#check-if-three-code-points-would-start-a-unicode-range) before calling this algorithm.

Note: This token is not produced by the tokenizer under normal circumstances. This algorithm is only called during [consume the value of a unicode-range descriptor](#consume-the-value-of-a-unicode-range-descriptor), which itself is only called as a special case for parsing the unicode-range descriptor; this single invocation in the entire language is due to a bad syntax design in early CSS.

1.
Consume the [next two input code points](#next-input-code-point) and discard them.
2.
Consume as many [hex digits](#hex-digit) as possible, but no more than 6. If less than 6 hex digits were consumed, consume as many U+003F QUESTION MARK (?) code points as possible, but no more than enough to make the total of hex digits and U+003F QUESTION MARK (?) code points equal to 6.
Let *first segment* be the consumed code points.
3.
If *first segment* contains any question mark code points, then:
  1.
  Replace the question marks in *first segment* with U+0030 DIGIT ZERO (0) code points, and interpret the result as a hexadecimal number. Let this be *start of range*.
  2.
  Replace the question marks in *first segment* with U+0046 LATIN CAPITAL LETTER F (F) code points, and interpret the result as a hexadecimal number. Let this be *end of range*.
  3.
  Return a new [<unicode-range-token>](#typedef-unicode-range-token) starting at *start of range* and ending at *end of range*.
4.
Otherwise, interpret *first segment* as a hexadecimal number, and let the result be *start of range*.
5.
If the [next 2 input code points](#next-input-code-point) are U+002D HYPHEN-MINUS (-) followed by a [hex digit](#hex-digit), then:
  1.
  Consume the [next input code point](#next-input-code-point).
  2.
  Consume as many [hex digits](#hex-digit) as possible, but no more than 6. Interpret the consumed code points as a hexadecimal number. Let this be *end of range*.
  3.
  Return a new [<unicode-range-token>](#typedef-unicode-range-token) starting at *start of range* and ending at *end of range*.
6.
Otherwise, return a new [<unicode-range-token>](#typedef-unicode-range-token) both starting and ending at *start of range*.

#### <a id="consume-remnants-of-bad-url"></a>4.3.15. Consume the remnants of a bad url

This section describes how to *<a id="consume-the-remnants-of-a-bad-url"></a>consume the remnants of a bad url* from a stream of code points, "cleaning up" after the tokenizer realizes that it’s in the middle of a [<bad-url-token>](#typedef-bad-url-token) rather than a [<url-token>](#typedef-url-token). It returns nothing; its sole use is to consume enough of the input stream to reach a recovery point where normal tokenizing can resume.

Repeatedly consume the [next input code point](#next-input-code-point) from the stream:

**U+0029 RIGHT PARENTHESIS ()) **
**EOF **
: Return.
**the input stream [starts with a valid escape](#check-if-two-code-points-are-a-valid-escape) **
: [Consume an escaped code point](#consume-an-escaped-code-point). This allows an escaped right parenthesis ("\)") to be encountered without ending the [<bad-url-token>](#typedef-bad-url-token). This is otherwise identical to the "anything else" clause.
**anything else **
: Do nothing.
## <a id="parsing"></a>5. Parsing

The CSS parser converts a [token stream](#css-token-stream) (produced by the tokenization process, defined earlier in this spec) into one or more of several CSS constructs (depending on which parsing algorithm is invoked).

### <a id="parser-diagrams"></a>5.1. Parser Railroad Diagrams

*This section is non-normative.*

This section presents an informative view of the parser, in the form of railroad diagrams.

These diagrams are *informative* and *incomplete*; they describe the grammar of "correct" stylesheets, but do not describe error-handling at all. They are provided solely to make it easier to get an intuitive grasp of the syntax.

**<a id="stylesheet-diagram"></a>Stylesheet **
:

**<a id="at-rule-diagram"></a>At-rule **
:

**<a id="qualified-rule-diagram"></a>Qualified rule **
:

**<a id="declaration-list-diagram"></a>{} block **
:

**<a id="declaration-diagram"></a>Declaration **
:

**<a id="!important-diagram"></a>!important **
:

**<a id="component-value-diagram"></a>Component value **
:

**<a id="simple-block-diagram"></a>Simple block **
:

**<a id="function-block-diagram"></a>Function block **
:

### <a id="css-tree"></a>5.2. CSS Parsing Results

The result of parsing can be any of the following (or lists of these):

***<a id="css-stylesheet"></a>stylesheet* **
:

A stylesheet has a list of [rules](#css-rule).

***<a id="css-rule"></a>rule* **
:

A [rule](#css-rule) is either an [at-rule](#at-rule) or a [qualified rule](#qualified-rule).

**[at-rule](#at-rule) **
: An [at-rule](#at-rule) has a name which is a string, a prelude consisting of a list of [component values](#component-value). [Block at-rules](#block-at-rule) (ending in a {}-block) will additionally have a list of [declarations](#declaration) and a list of child [rules](#css-rule). ([Statement at-rules](#statement-at-rule), ending in a semicolon, will not.)
***<a id="qualified-rule"></a>qualified rule* **
: A qualified rule has a prelude consisting of a list of component values, a list of declarations, and a list of child rules.

Note: Most qualified rules will be style rules, where the prelude is a selector [[selectors-4]](#biblio-selectors-4) and its declarations are [properties](css-cascade-5.md#css-property).

***<a id="declaration"></a>declaration* **
: A declaration has a name which is a string, a value consisting of a list of [component values](#component-value), and an *important* flag which is initially unset. It also has an optional *original text* which is a string (captured for only a few declarations).

Declarations are further categorized as *<a id="css-property-declarations"></a>property declarations* or *<a id="css-descriptor-declarations"></a>descriptor declarations*, with the former setting CSS [properties](css-cascade-5.md#css-property) and appearing most often in [qualified rules](#qualified-rule) and the latter setting CSS [descriptors](#css-descriptor), which appear only in [at-rules](#at-rule). (This categorization does not occur at the Syntax level; instead, it is a product of where the declaration appears, and is defined by the respective specifications defining the given rule.)

***<a id="component-value"></a>component value* **
: A component value is one of the [preserved tokens](#preserved-tokens), a [function](#function), or a [simple block](#simple-block).
***<a id="preserved-tokens"></a>preserved tokens* **
: Any token produced by the tokenizer except for [<function-token>](#typedef-function-token)s, [<{-token>](#tokendef-open-curly)s, [<(-token>](#tokendef-open-paren)s, and [<[-token>](#tokendef-open-square)s.

Note: The non-[preserved tokens](#preserved-tokens) listed above are always consumed into higher-level objects, either functions or simple blocks, and so never appear in any parser output themselves.

Note: The tokens [<}-token>](#tokendef-close-curly)s, [<)-token>](#tokendef-close-paren)s, [<]-token>](#tokendef-close-square), [<bad-string-token>](#typedef-bad-string-token), and [<bad-url-token>](#typedef-bad-url-token) are always parse errors, but they are preserved in the token stream by this specification to allow other specs, such as Media Queries, to define more fine-grained error-handling than just dropping an entire declaration or block.

***<a id="function"></a>function* **
: A function has a name and a value consisting of a list of [component values](#component-value).
***<a id="simple-block"></a>simple block* **
***<a id="curly-block"></a>{}-block* **
***<a id="square-block"></a>[]-block* **
***<a id="paren-block"></a>()-block* **
: A simple block has an associated token (either a [<[-token>](#tokendef-open-square), [<(-token>](#tokendef-open-paren), or [<{-token>](#tokendef-open-curly)) and a value consisting of a list of component values.

[{}-block](#curly-block), [[]-block](#square-block), and [()-block](#paren-block) refer specifically to a [simple block](#simple-block) with that corresponding associated token.

### <a id="parser-definitions"></a>5.3. Token Streams

A *<a id="css-token-stream"></a>token stream* is a struct representing a stream of [tokens](#token-stream-tokens) and/or [component values](#component-value). It has the following items:

***<a id="token-stream-tokens"></a>tokens* **
:

A list of [tokens](#token-stream-tokens) and/or [component values](#component-value).

Note: This specification assumes, for simplicity, that the input stream has been fully tokenized before parsing begins. However, the parsing algorithms only use one token of "lookahead", so in practice tokenization and parsing can be done in lockstep.

***<a id="token-stream-index"></a>index* **
:

An index into the [tokens](#token-stream-tokens), representing the progress of parsing. It starts at 0 initially.

Note: Aside from [marking](#token-stream-mark), the [index](#token-stream-index) never goes backwards. Thus the already-processed prefix of [tokens](#token-stream-tokens) can be eagerly discarded as it’s processed.

***<a id="token-stream-marked-indexes"></a>marked indexes* **
:

A stack of index values, representing points that the parser might return to. It starts empty initially.

CSS has a small number of places that require referencing the precise text that was parsed for a declaration’s value (not just what tokens were produced from that text). This is not explicitly described in the algorithmic structure here, but the [token stream](#css-token-stream) must, thus, have the ability to reproduce the original text of declarations on demand. See [consume a declaration](#consume-a-declaration) for details on when this is required.

Several operations can be performed on a [token stream](#css-token-stream):

***<a id="token-stream-next-token"></a>next token* **
:

The item of [tokens](#token-stream-tokens) at [index](#token-stream-index).

If that index would be out-of-bounds past the end of the list, it’s instead an [<eof-token>](#typedef-eof-token).

***<a id="token-stream-empty"></a>empty* **
:

A token stream is [empty](#token-stream-empty) if the [next token](#token-stream-next-token) is an [<eof-token>](#typedef-eof-token).

***<a id="token-stream-consume-a-token"></a>consume a token* **
:

Let *token* be the [next token](#token-stream-next-token). Increment [index](#token-stream-index), then return *token*.

***<a id="token-stream-discard-a-token"></a>discard a token* **
:

If the [token stream](#css-token-stream) is not [empty](#token-stream-empty), increment [index](#token-stream-index).

***<a id="token-stream-mark"></a>mark* **
:

Append [index](#token-stream-index) to [marked indexes](#token-stream-marked-indexes).

***<a id="token-stream-restore-a-mark"></a>restore a mark* **
:

Pop from [marked indexes](#token-stream-marked-indexes), and set [index](#token-stream-index) to the popped value.

***<a id="token-stream-discard-a-mark"></a>discard a mark* **
:

Pop from [marked indexes](#token-stream-marked-indexes), and do nothing with the popped value.

***<a id="token-stream-discard-whitespace"></a>discard whitespace* **
:

While the [next token](#token-stream-next-token) is a [<whitespace-token>](#typedef-whitespace-token), [discard a token](#token-stream-discard-a-token).

***<a id="token-stream-process"></a>process* **
:

To [process](#token-stream-process), given a following list of token types and associated actions, perform the action associated with the [next token](#token-stream-next-token). Repeat until one of the actions returns something, then return that.

An *<a id="typedef-eof-token"></a>[<eof-token>](#typedef-eof-token)* is a conceptual token, not actually produced by the tokenizer, used to indicate that the [token stream](#css-token-stream) has been exhausted.

### <a id="parser-entry-points"></a>5.4. Parser Entry Points

The algorithms defined in this section produce high-level CSS objects from lists of CSS tokens.

The algorithms here operate on a token stream as input, but for convenience can also be invoked with a number of other value types.

To *<a id="normalize-into-a-token-stream"></a>normalize into a token stream* a given *input*:

1.
If *input* is already a [token stream](#css-token-stream), return it.
2.
If *input* is a list of CSS [tokens](#token-stream-tokens) and/or [component values](#component-value), create a new [token stream](#css-token-stream) with *input* as its tokens, and return it.
3.
If *input* is a string, then [filter code points](#css-filter-code-points) from *input*, [tokenize](#css-tokenize) the result, and return the stream of tokens.
4.
Assert: Only the preceding types should be passed as *input*.

Note: Other specs can define additional entry points for their own purposes.

The following notes should probably be translated into normative text in the relevant specs, hooking this spec’s terms:

- "[Parse a stylesheet](#parse-a-stylesheet)" is intended to be the normal parser entry point, for parsing stylesheets.
- "[Parse a stylesheet’s contents](#parse-a-stylesheets-contents)" is intended for use by the `CSSStyleSheet replace()` method, and similar, which parse text into the contents of an existing stylesheet.
- "[Parse a rule](#parse-a-rule)" is intended for use by the `CSSStyleSheet insertRule()` method, and similar, which parse text into a single rule. `CSSStyleSheet#insertRule` method, and similar functions which might exist, which parse text into a single rule.
- "[Parse a declaration](#parse-a-declaration)" is used in @supports conditions. [[CSS3-CONDITIONAL]](#biblio-css3-conditional)
- "[Parse a list of declarations](#parse-a-list-of-declarations)" is intended for parsing anything that expects just a sequence of declaration, such as the HTML `style` attribute, and APIs such as `the CSSStyleDeclaration cssText attribute`.
- "[Parse a component value](#parse-a-component-value)" is for things that need to consume a single value, like the parsing rules for attr().
- "[Parse a list of component values](#parse-a-list-of-component-values)" is for the contents of presentational attributes, which parse text into a single declaration’s value, or for parsing a stand-alone selector [[SELECT]](#biblio-select) or list of Media Queries [[MEDIAQ]](#biblio-mediaq), as in Selectors API or the `media` HTML attribute.

#### <a id="parse-grammar"></a>5.4.1. Parse something according to a CSS grammar

It is often desirable to parse a string or token list to see if it matches some CSS grammar, and if it does, to destructure it according to the grammar. This section provides a generic hook for this kind of operation. It should be invoked like "parse *foo* as a CSS <color>", or similar.

This algorithm returns either failure, if the input does not match the provided grammar, or the result of parsing the input according to the grammar, which is an unspecified structure corresponding to the provided grammar specification. The return value must only be interacted with by specification prose, where the representation ambiguity is not problematic. If it is meant to be exposed outside of spec language, the spec using the result must explicitly translate it into a well-specified representation, such as, for example, by invoking a CSS serialization algorithm (like "serialize as a CSS [<string>](css-values-4.md#string-value) value").

Note: This algorithm, and [parse a comma-separated list according to a CSS grammar](#css-parse-a-comma-separated-list-according-to-a-css-grammar), are *usually* the only parsing algorithms other specs will want to call. The remaining parsing algorithms are meant mostly for [[CSSOM]](#biblio-cssom) and related "explicitly constructing CSS structures" cases. Consult the CSSWG for guidance first if you think you need to use one of the other algorithms.

To *<a id="css-parse-something-according-to-a-css-grammar"></a>parse something according to a CSS grammar* (aka simply [parse](#css-parse-something-according-to-a-css-grammar)) given an *input* and a CSS *grammar* production:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. [Parse a list of component values](#parse-a-list-of-component-values) from *input*, and let *result* be the return value.
3. Attempt to match *result* against *grammar*. If this is successful, return the matched result; otherwise, return failure.

#### <a id="parse-comma-list"></a>5.4.2. Parse a comma-separated list according to a CSS grammar

While one can definitely [parse](#css-parse-something-according-to-a-css-grammar) a value according to a grammar with commas in it, if *any* part of the value fails to parse, the entire thing doesn’t parse, and returns failure.

Sometimes that’s what’s desired (such as in list-valued CSS properties); other times, it’s better to let each comma-separated sub-part of the value parse separately, dealing with the parts that parse successfully one way, and the parts that fail to parse another way (typically ignoring them, such as in `<img sizes>`).

This algorithm provides an easy hook to accomplish exactly that. It returns a list of values split by "top-level" commas, where each values is either failure (if it failed to parse) or the result of parsing (an unspecified structure, as described in the [parse](#css-parse-something-according-to-a-css-grammar) algorithm).

To *<a id="css-parse-a-comma-separated-list-according-to-a-css-grammar"></a>parse a comma-separated list according to a CSS grammar* (aka [parse a list](#css-parse-a-comma-separated-list-according-to-a-css-grammar)) given an *input* and a CSS *grammar* production:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. If *input* contains only [<whitespace-token>](#typedef-whitespace-token)s, return an empty list.
3. [Parse a comma-separated list of component values](#parse-a-comma-separated-list-of-component-values) from *input*, and let *list* be the return value.
4. For each *item* of *list*, replace *item* with the result of [parsing](#css-parse-something-according-to-a-css-grammar) *item* with *grammar*.
5. Return *list*.

#### <a id="parse-stylesheet"></a>5.4.3. Parse a stylesheet

To *<a id="parse-a-stylesheet"></a>parse a stylesheet* from an *input* given an optional url *location*:

1. If *input* is a byte stream for a stylesheet, [decode bytes](#css-decode-bytes) from *input*, and set *input* to the result.
2. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
3. Create a new stylesheet, with its location set to *location* (or null, if *location* was not passed).
4. [Consume a stylesheet’s contents](#consume-a-stylesheets-contents) from *input*, and set the stylesheet’s rules to the result.
Note: [Consume a stylesheet’s contents](#consume-a-stylesheets-contents) always consumes the entire stream.
5. Return the stylesheet.

#### <a id="parse-stylesheet-contents"></a>5.4.4. <a id="parse-list-of-rules"></a> Parse a stylesheet’s contents

To *<a id="parse-a-stylesheets-contents"></a>parse a stylesheet’s contents* from *input*:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. [Consume a stylesheet’s contents](#consume-a-stylesheets-contents) from *input*, and return the result.
Note: [Consume a stylesheet’s contents](#consume-a-stylesheets-contents) always consumes the entire stream.

#### <a id="parse-list-of-declarations"></a>5.4.5. Parse a list of declarations

To *<a id="parse-a-list-of-declarations"></a>parse a list of declarations* from *input*:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. Let *decls* be an empty list.
3. While *input* is not [empty](#token-stream-empty):
[Discard whitespace](#token-stream-discard-whitespace) from *input*.
  -
  If the [next token](#token-stream-next-token) from *input* is a [<semicolon-token>](#typedef-semicolon-token), [discard a token](#token-stream-discard-a-token) from *input*.
  -
  Otherwise, if the [next token](#token-stream-next-token) from *input* is an [<at-keyword-token>](#typedef-at-keyword-token), [consume an at-rule](#consume-an-at-rule) from *input* and discard the result.
  -
  Otherwise, [consume a declaration](#consume-a-declaration). If anything was returned, append it to *decls*.
4. Return *decls*.

Note: The entire stream is consumed by this algorithm.

#### <a id="parse-rule"></a>5.4.6. Parse a rule

To *<a id="parse-a-rule"></a>parse a rule* from *input*:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
3. If *input* is [empty](#token-stream-empty), return a syntax error.
Otherwise, if the [next token](#token-stream-next-token) from *input* is an [<at-keyword-token>](#typedef-at-keyword-token), [consume an at-rule](#consume-an-at-rule) from *input*, and let *rule* be the return value.
Otherwise, [consume a qualified rule](#consume-a-qualified-rule) from *input* and let *rule* be the return value. If nothing or an [invalid rule error](#invalid-rule-error) was returned, return a syntax error.
4. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
5. If *input* is [empty](#token-stream-empty), return *rule*. Otherwise, return a syntax error.

#### <a id="parse-declaration"></a>5.4.7. Parse a declaration

To *<a id="parse-a-declaration"></a>parse a declaration* from *input*:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
3. If *input* is [empty](#token-stream-empty), return a syntax error.
4. [Consume a declaration](#consume-a-declaration) from *input*. If anything was returned, let *value* be the return value. Otherwise, return a syntax error.
5. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
6. If *input* is [empty](#token-stream-empty), return *value*. Otherwise, return a syntax error.

#### <a id="parse-component-value"></a>5.4.8. Parse a component value

To *<a id="parse-a-component-value"></a>parse a component value* from *input*:

1. [Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
3. If *input* is [empty](#token-stream-empty), return a syntax error.
4. [Consume a component value](#consume-a-component-value) from *input* and let *value* be the return value.
5. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
6. If *input* is [empty](#token-stream-empty), return *value*. Otherwise, return a syntax error.

#### <a id="parse-list-of-component-values"></a>5.4.9. Parse a list of component values

To *<a id="parse-a-list-of-component-values"></a>parse a list of component values* from *input*:

1.
[Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2.
[Consume a list of component values](#consume-a-list-of-component-values) from *input*, and return the result.

Note: The entire stream is consumed by this algorithm.

#### <a id="parse-comma-separated-list-of-component-values"></a>5.4.10. Parse a comma-separated list of component values

To *<a id="parse-a-comma-separated-list-of-component-values"></a>parse a comma-separated list of component values* from *input*:

1.
[Normalize](#normalize-into-a-token-stream) *input*, and set *input* to the result.
2.
Let *groups* be an empty list.
3.
While *input* is not [empty](#token-stream-empty):
  1.
  [Consume a list of component values](#consume-a-list-of-component-values) from *input*, with [<comma-token>](#typedef-comma-token) as the stop token, and append the result to *groups*.
  2.
  [Discard a token](#token-stream-discard-a-token) from *input*.
4.
Return *groups*.

Note: The entire stream is consumed by this algorithm.

### <a id="parser-algorithms"></a>5.5. Parser Algorithms

The following algorithms comprise the parser. They are called by the parser entry points above, and generally should not be called directly by other specifications.

Note that CSS parsing is case-sensitive, and checking the validity of constructs in a given context must be done *during* parsing in at least some circumstances. This specification intentionally does not specify *how* sufficient context should be passed around to enable validity-checking.

#### <a id="consume-stylesheet-contents"></a>5.5.1. <a id="consume-list-of-rules"></a> Consume a stylesheet’s contents

To *<a id="consume-a-stylesheets-contents"></a>consume a stylesheet’s contents* from a [token stream](#css-token-stream) *input*:

Let *rules* be an initially empty list of rules.

[Process](#token-stream-process) *input*:

**[<whitespace-token>](#typedef-whitespace-token) **
: [Discard a token](#token-stream-discard-a-token) from *input*.
**[<EOF-token>](#typedef-eof-token) **
: Return *rules*.
**[<CDO-token>](#typedef-cdo-token) **
**[<CDC-token>](#typedef-cdc-token) **
: [Discard a token](#token-stream-discard-a-token) from *input*.

What’s this for?

Back when CSS was first being introduced, the `style` element was treated as an unknown element by older browsers. To avoid having its contents displayed in the page for these legacy browsers, it became common practice to wrap the stylesheet in an HTML comment, and newer browsers would simply ignore the HTML comment syntax. This requirement carries over to today, decades later.

The same practice was done for `script` elements, where HTML comment syntax is treated as a line comment in JS (similar to `//`) for the same reason.

**[<at-keyword-token>](#typedef-at-keyword-token) **
: [Consume an at-rule](#consume-an-at-rule) from *input*. If anything is returned, append it to *rules*.
**anything else **
: [Consume a qualified rule](#consume-a-qualified-rule) from *input*. If a [rule](#css-rule) is returned, append it to *rules*.
#### <a id="consume-at-rule"></a>5.5.2. Consume an at-rule

To *<a id="consume-an-at-rule"></a>consume an at-rule* from a [token stream](#css-token-stream) *input*, given an optional bool *nested* (default false):

Assert: The [next token](#token-stream-next-token) is an [<at-keyword-token>](#typedef-at-keyword-token).

[Consume a token](#token-stream-consume-a-token) from *input*, and let *rule* be a new [at-rule](#at-rule) with its name set to the returned token’s value, its prelude initially set to an empty list, and no declarations or child rules.

[Process](#token-stream-process) *input*:

**[<semicolon-token>](#typedef-semicolon-token) **
**[<EOF-token>](#typedef-eof-token) **
: [Discard a token](#token-stream-discard-a-token) from *input*. If *rule* is valid in the current context, return it; otherwise return nothing.
**[<}-token>](#tokendef-close-curly) **
: If *nested* is true:

-
If *rule* is valid in the current context, return it.
-
Otherwise, return nothing.

Otherwise, [consume a token](#token-stream-consume-a-token) and append the result to *rule*’s prelude.

**[<{-token>](#tokendef-open-curly) **
: [Consume a block](#consume-a-block) from *input*, and assign the result to *rule*’s child [rules](#css-rule).

Note: If the result contains lists of [declarations](#declaration), how they’re materialized in the CSSOM depends on the rule. Some turn them all into nested declarations rules, others will treat them all as declarations, and others will treat the first item differently from the rest.

If *rule* is valid in the current context, return it. Otherwise, return nothing.

**anything else **
: [Consume a component value](#consume-a-component-value) from *input* and append the returned value to *rule*’s prelude.
#### <a id="consume-qualified-rule"></a>5.5.3. Consume a qualified rule

To *<a id="consume-a-qualified-rule"></a>consume a qualified rule*, from a [token stream](#css-token-stream) *input*, given an optional [token](#token-stream-tokens) *stop token* and an optional bool *nested* (default false):

Let *rule* be a new [qualified rule](#qualified-rule) with its prelude, declarations, and child rules all initially set to empty lists.

[Process](#token-stream-process) *input*:

**[<EOF-token>](#typedef-eof-token) **
***stop token* (if passed) **
: This is a [parse error](#parse-error). Return nothing.
**[<}-token>](#tokendef-close-curly) **
: This is a [parse error](#parse-error). If *nested* is true, return nothing. Otherwise, [consume a token](#token-stream-consume-a-token) and append the result to *rule*’s prelude.
**[<{-token>](#tokendef-open-curly) **
: If the first two non-[<whitespace-token>](#typedef-whitespace-token) values of *rule*’s prelude are an [<ident-token>](#typedef-ident-token) whose value starts with "--" followed by a [<colon-token>](#typedef-colon-token), then:

-
If *nested* is true, [consume the remnants of a bad declaration](#consume-the-remnants-of-a-bad-declaration) from *input*, with *nested* set to true, and return nothing.
-
If *nested* is false, [consume a block](#consume-a-block) from *input*, and return nothing.

What’s this check for?

[Declarations](#declaration) and [qualified rules](#qualified-rule) don’t *generally* overlap in their allowed syntax. No currently-defined CSS property allows {}-blocks in its value, so `foo:bar {};` is definitely a rule, and `foo: bar;` is definitely a property. Even if a future CSS property allows {}-blocks in its value, the allowed syntax is restricted to the {}-block being the whole value, such as `foo: {...};`, which is guaranteed to not be a valid rule, since the : doesn’t have an ident or function following it to mark it as a pseudo-class.

This allows us to mix declarations and rules in the same context: we first try to parse something as a declaration, and if that doesn’t result in a valid declaration, we reparse it as a rule instead. An accidentally-invalid declaration will parse as a rule instead, but that’s fine: the parser will stop at the declaration’s ending semicolon and consider it an invalid rule. (Or in the case of a property containing a {}-block, will stop just *before* the semicolon, still considering it an invalid rule, and then the next attempt to parse something will throw out the lone semicolon as invalid.) So the total amount of tokens consumed is the same regardless.

Custom properties, however, don’t have the CSSWG carefully vetting their syntax. Authors *can* write a custom property that takes a {}-block in its value, even combined with other things; if that custom property is then invalid (due to an invalidly-written var() function, for example), when it’s reparsed as a rule it will stop early, at the {}-block. The remaining tokens of the custom property’s value will then get parsed as a fresh construct, potentially causing unexpected declarations or rules to be created.

To avoid this (admittedly very niche) corner-case, we subtract the syntax of a custom property from that of a [qualified rule](#qualified-rule); if you’re in a context that allows properties and rules to be mixed, and you somehow end up parsing a rule that looks like a custom property, you’ve messed up, and need to instead consume an entire custom property (all the way to the ending semicolon).

(If we’re in a context that doesn’t allow properties, we just throw away the rule if it looks like a custom property. This ensures that `--foo:hover { color: blue; }` is consistently invalid everywhere, without potentially consuming a ton of a stylesheet looking for the non-existent ending semicolon.)

Otherwise, [consume a block](#consume-a-block) from *input*, and let *child rules* be the result. If the first item of *child rules* is a list of [declarations](#declaration), remove it from *child rules* and assign it to *rule*’s declarations. If any remaining items of *child rules* are lists of declarations, replace them with nested declarations rules containing the list as its sole child. Assign *child rules* to *rule*’s child [rules](#css-rule).

If *rule* is valid in the current context, return it; otherwise return an *<a id="invalid-rule-error"></a>invalid rule error*.

**anything else **
: [Consume a component value](#consume-a-component-value) from *input* and append the result to *rule*’s prelude.
#### <a id="consume-block"></a>5.5.4. Consume a block

To *<a id="consume-a-block"></a>consume a block*, from a [token stream](#css-token-stream) *input*:

Assert: The [next token](#token-stream-next-token) is a [<{-token>](#tokendef-open-curly).

[Discard a token](#token-stream-discard-a-token) from *input*. [Consume a block’s contents](#consume-a-blocks-contents) from *input* and let *rules* be the result. Discard a token from *input*.

Return *rules*.

#### <a id="consume-block-contents"></a>5.5.5. Consume a block’s contents

To *<a id="consume-a-blocks-contents"></a>consume a block’s contents* from a [token stream](#css-token-stream) *input*:

Note: This algorithm returns a list, consisting of [rules](#css-rule) and lists of [declarations](#declaration). Depending on the parent context, a list of declarations might be materialized in the CSSOM as either a `CSSStyleDeclaration`, or as a `CSSNestedDeclarations` rule.

Let *rules* be an empty list, containing either [rules](#css-rule) or lists of [declarations](#declaration).

Let *decls* be an empty list of [declarations](#declaration).

[Process](#token-stream-process) *input*:

**[<whitespace-token>](#typedef-whitespace-token) **
**[<semicolon-token>](#typedef-semicolon-token) **
: [Discard a token](#token-stream-discard-a-token) from *input*.
**[<EOF-token>](#typedef-eof-token) **
**[<}-token>](#tokendef-close-curly) **
: Return *rules*.
**[<at-keyword-token>](#typedef-at-keyword-token) **
: If *decls* is not empty, append it to *rules*, and set *decls* to a fresh empty list of [declarations](#declaration).

[Consume an at-rule](#consume-an-at-rule) from *input*, with *nested* set to true. If a [rule](#css-rule) was returned, append it to *rules*.

**anything else **
: [Mark](#token-stream-mark) *input*.

[Consume a declaration](#consume-a-declaration) from *input*, with *nested* set to true. If a [declaration](#declaration) was returned, append it to *decls*, and [discard a mark](#token-stream-discard-a-mark) from *input*.

Otherwise, [restore a mark](#token-stream-restore-a-mark) from *input*, then [consume a qualified rule](#consume-a-qualified-rule) from *input*, with *nested* set to true, and [<semicolon-token>](#typedef-semicolon-token) as the *stop token*.

**If nothing was returned **
:

Do nothing

**If an [invalid rule error](#invalid-rule-error) was returned **
:

If *decls* is not empty, append *decls* to *rules*, and set *decls* to a fresh empty list of [declarations](#declaration). (Otherwise, do nothing.)

**If a [rule](#css-rule) was returned **
:

If *decls* is not empty, append *decls* to *rules*, and set *decls* to a fresh empty list of [declarations](#declaration). Append the [rule](#css-rule) to *rules*.

Implementation note

This spec, as with many CSS specs, has been written to prioritize understandability over efficiency. A number of algorithms, notably the above "parse as a declaration, then parse as a rule" behavior can be fairly inefficient if implemented naively as described.

However, the behavior has been carefully written to allow "early exits" as much as possible. In particular, and roughly in order of when the exit can occur:

-
If the first non-whitespace token isn’t an [<ident-token>](#typedef-ident-token) for a recognized property name (or a custom property name), you can immediately stop parsing as a declaration and reparse as a rule instead. If the *next* non-whitespace token isn’t a [<colon-token>](#typedef-colon-token), you can similarly immediately stop parsing as a declaration.
(That is, font+ ... is guaranteed to not be a property, nor is not-a-prop-name: ....)
-
If the first two non-whitespace tokens are a custom property name and a colon, it’s definitely a custom property and won’t ever produce a valid rule, so even if the custom property ends up invalid there’s no need to try and reparse as a rule.
(That is, --foo:hover {...} is guaranteed to be a custom property, not a rule.)
-
If the first three non-whitespace tokens are a valid property name, a colon, and anything other than a [<{-token>](#tokendef-open-curly), and then while parsing the declaration’s value you encounter a <{-token>, you can immediately stop parsing as a declaration and reparse as a rule instead.
(That is, font:bar {... is guaranteed to be an invalid property.)
-
If you see a recognized property name, a colon, and a {}-block, but the first non-whitespace tokens following that block isn’t either immediately the final semicolon, or the !important followed by the semicolon, you can immediately stop parsing as a declaration and reparse as a rule instead.
(That is, font: {} bar ... is guaranteed to be an invalid property; you don’t need to keep parsing until you hit a semicolon.)

Similarly, even tho the parsing requirements are specified to rely on checking the grammar of the declarations as you parse, a *generic* processor trying to implement a non-CSS language on top of the generic CSS *syntax* can still get away with just verifying that declarations start with an ident, a colon, and then either contain solely a {}-block or no {}-block at all. They’ll just spent a little more time on parsing than an implementation with grammar knowledge in cases like foo:hover ... {}, since they can’t early-exit on the first token.

#### <a id="consume-declaration"></a>5.5.6. Consume a declaration

To *<a id="consume-a-declaration"></a>consume a declaration* from a [token stream](#css-token-stream) *input*, given an optional bool *nested* (default false):

Let *decl* be a new [declaration](#declaration), with an initially empty name and a value set to an empty list.

1. If the [next token](#token-stream-next-token) is an [<ident-token>](#typedef-ident-token), [consume a token](#token-stream-consume-a-token) from *input* and set *decl*'s name to the token’s value.
Otherwise, [consume the remnants of a bad declaration](#consume-the-remnants-of-a-bad-declaration) from *input*, with *nested*, and return nothing.
2. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
3. If the [next token](#token-stream-next-token) is a [<colon-token>](#typedef-colon-token), [discard a token](#token-stream-discard-a-token) from *input*.
Otherwise, [consume the remnants of a bad declaration](#consume-the-remnants-of-a-bad-declaration) from *input*, with *nested*, and return nothing.
4. [Discard whitespace](#token-stream-discard-whitespace) from *input*.
5. [Consume a list of component values](#consume-a-list-of-component-values) from *input*, with *nested*, and with [<semicolon-token>](#typedef-semicolon-token) as the stop token, and set *decl*’s value to the result.
6. If the last two non-[<whitespace-token>](#typedef-whitespace-token)s in *decl*’s value are a [<delim-token>](#typedef-delim-token) with the value "!" followed by an [<ident-token>](#typedef-ident-token) with a value that is an ASCII case-insensitive match for "important", remove them from *decl*’s value and set *decl*’s *important* flag.
7. While the last item in *decl*’s value is a [<whitespace-token>](#typedef-whitespace-token), remove that token.
8. If *decl*’s name is a custom property name string, then set *decl*’s *original text* to the segment of the original source text string corresponding to the tokens of *decl*’s value.
Otherwise, if *decl*’s value contains a top-level [simple block](#simple-block) with an associated token of [<{-token>](#tokendef-open-curly), and also contains *any other* non-[<whitespace-token>](#typedef-whitespace-token) value, return nothing. (That is, a top-level {}-block is only allowed as the entire value of a non-custom property.)
Otherwise, if *decl*’s name is an ASCII case-insensitive match for "unicode-range", [consume the value of a unicode-range descriptor](#consume-the-value-of-a-unicode-range-descriptor) from the segment of the original source text string corresponding to the tokens returned by the [consume a list of component values](#consume-a-list-of-component-values) call, and replace *decl*’s value with the result.
9. If *decl* is valid in the current context, return it; otherwise return nothing.

To *<a id="consume-the-remnants-of-a-bad-declaration"></a>consume the remnants of a bad declaration* from a [token stream](#css-token-stream) *input*, given a bool *nested*:

[Process](#token-stream-process) *input*:

**[<eof-token>](#typedef-eof-token) **
**[<semicolon-token>](#typedef-semicolon-token) **
:

[Discard a token](#token-stream-discard-a-token) from *input*, and return nothing.

**[<}-token>](#tokendef-close-curly) **
:

If *nested* is true, return nothing. Otherwise, [discard a token](#token-stream-discard-a-token).

**anything else **
:

[Consume a component value](#consume-a-component-value) from *input*, and do nothing.

#### <a id="consume-list-of-components"></a>5.5.7. Consume a list of component values

To *<a id="consume-a-list-of-component-values"></a>consume a list of component values* from a [token stream](#css-token-stream) *input*, given an optional [token](#token-stream-tokens) *stop token* and an optional boolean *nested* (default false):

Let *values* be an empty list of [component values](#component-value).

[Process](#token-stream-process) *input*:

**[<eof-token>](#typedef-eof-token) **
***stop token* (if passed) **
:

Return *values*.

**[<}-token>](#tokendef-close-curly) **
:

If *nested* is true, return *values*.

Otherwise, this is a [parse error](#parse-error). [Consume a token](#token-stream-consume-a-token) from *input* and append the result to *values*.

**anything else **
:

[Consume a component value](#consume-a-component-value) from *input*, and append the result to *values*.

#### <a id="consume-component-value"></a>5.5.8. Consume a component value

To *<a id="consume-a-component-value"></a>consume a component value* from a [token stream](#css-token-stream) *input*:

[Process](#token-stream-process) *input*:

**[<{-token>](#tokendef-open-curly) **
**[<[-token>](#tokendef-open-square) **
**[<(-token>](#tokendef-open-paren) **
:

[Consume a simple block](#consume-a-simple-block) from *input* and return the result.

**[<function-token>](#typedef-function-token) **
:

[Consume a function](#consume-a-function) from *input* and return the result.

**anything else **
:

[Consume a token](#token-stream-consume-a-token) from *input* and return the result.

#### <a id="consume-simple-block"></a>5.5.9. Consume a simple block

To *<a id="consume-a-simple-block"></a>consume a simple block* from a [token stream](#css-token-stream) *input*:

Assert: the [next token](#token-stream-next-token) of *input* is [<{-token>](#tokendef-open-curly), [<[-token>](#tokendef-open-square), or [<(-token>](#tokendef-open-paren).

Let *ending token* be the mirror variant of the [next token](#token-stream-next-token). (E.g. if it was called with [<[-token>](#tokendef-open-square), the *ending token* is [<]-token>](#tokendef-close-square).)

Let *block* be a new [simple block](#simple-block) with its associated token set to the [next token](#token-stream-next-token) and with its value initially set to an empty list.

[Discard a token](#token-stream-discard-a-token) from *input*.

[Process](#token-stream-process) *input*:

**[<eof-token>](#typedef-eof-token) **
***ending token* **
:

[Discard a token](#token-stream-discard-a-token) from *input*. Return *block*.

**anything else **
:

[Consume a component value](#consume-a-component-value) from *input* and append the result to *block*’s value.

#### <a id="consume-function"></a>5.5.10. Consume a function

To *<a id="consume-a-function"></a>consume a function* from a [token stream](#css-token-stream) *input*:

Assert: The [next token](#token-stream-next-token) is a [<function-token>](#typedef-function-token).

[Consume a token](#token-stream-consume-a-token) from *input*, and let *function* be a new function with its name equal the returned token’s value, and a value set to an empty list.

[Process](#token-stream-process) *input*:

**[<eof-token>](#typedef-eof-token) **
**[<)-token>](#tokendef-close-paren) **
:

[Discard a token](#token-stream-discard-a-token) from *input*. Return *function*.

**anything else **
:

[Consume a component value](#consume-a-component-value) from *input* and append the result to *function*’s value.

#### <a id="consume-unicode-range-value"></a>5.5.11. Consume a unicode-range value

To *<a id="consume-the-value-of-a-unicode-range-descriptor"></a>consume the value of a unicode-range descriptor* from a string *input string*:

1.
Let *tokens* be the result of [tokenizing](#css-tokenize) *input string* with *unicode ranges allowed* set to true.
2.
[Consume a list of component values](#consume-a-list-of-component-values) from *tokens*, and return the result.

Note: The existence of this algorithm is due to a design mistake in early CSS. It should never be reproduced.

## <a id="anb-microsyntax"></a>6. The *An+B* microsyntax

Several things in CSS, such as the [:nth-child()](selectors-4.md#nth-child-pseudo) pseudoclass, need to indicate indexes in a list. The *An+B* microsyntax is useful for this, allowing an author to easily indicate single elements or all elements at regularly-spaced intervals in a list.

The *<a id="anb"></a>An+B* notation defines an integer step (*A*) and offset (*B*), and represents the *An+B*th elements in a list, for every positive integer or zero value of *n*, with the first element in the list having index 1 (not 0).

For values of *A* and *B* greater than 0, this effectively divides the list into groups of *A* elements (the last group taking the remainder), and selecting the *B*th element of each group.

The *An+B* notation also accepts the even and odd keywords, which have the same meaning as 2n and 2n+1, respectively.

<a id="example-0b8828eb"></a>

Examples:

```
2n+0   /* represents all of the even elements in the list */
even   /* same */
4n+1   /* represents the 1st, 5th, 9th, 13th, etc. elements in the list */
```

The values of *A* and *B* can be negative, but only the positive results of *An+B*, for *n* ≥ 0, are used.

<a id="example-34cd21ee"></a>

Example:

```
-1n+6   /* represents the first 6 elements of the list */
-4n+10  /* represents the 2nd, 6th, and 10th elements of the list */
```

If both *A* and *B* are 0, the pseudo-class represents no element in the list.

### <a id="anb-syntax"></a>6.1. Informal Syntax Description

*This section is non-normative.*

When *A* is 0, the *An* part may be omitted (unless the *B* part is already omitted). When *An* is not included and *B* is non-negative, the [+](selectors-4.md#selectordef-adjacent) sign before *B* (when allowed) may also be omitted. In this case the syntax simplifies to just *B*.

<a id="example-335ba750"></a>

Examples:

```
0n+5   /* represents the 5th element in the list */
5      /* same */
```

When *A* is 1 or -1, the `1` may be omitted from the rule.

<a id="example-5d33d659"></a>

Examples:

The following notations are therefore equivalent:

```
1n+0   /* represents all elements in the list */
n+0    /* same */
n      /* same */
```

If *B* is 0, then every *A*th element is picked. In such a case, the *+B* (or *-B*) part may be omitted unless the *A* part is already omitted.

<a id="example-c62dcd18"></a>

Examples:

```
2n+0   /* represents every even element in the list */
2n     /* same */
```

When B is negative, its minus sign replaces the [+](selectors-4.md#selectordef-adjacent) sign.

<a id="example-3d25855e"></a>

Valid example:

```
3n-6
```

Invalid example:

```
3n + -6
```

Whitespace is permitted on either side of the [+](selectors-4.md#selectordef-adjacent) or - that separates the *An* and *B* parts when both are present.

<a id="example-e61d78e8"></a>

Valid Examples with white space:

```
3n + 1
+3n - 2
-n+ 6
+6
```

Invalid Examples with white space:

```
3 n
+ 2n
+ 2
```

### <a id="the-anb-type"></a>6.2. The `<a-n-plus-b>` type

The *An+B* notation was originally defined using a slightly different tokenizer than the rest of CSS, resulting in a somewhat odd definition when expressed in terms of CSS tokens. This section describes how to recognize the *An+B* notation in terms of CSS tokens (thus defining the *<a-n-plus-b>* type for CSS grammar purposes), and how to interpret the CSS tokens to obtain values for *A* and *B*.

The *<a-n-plus-b>* type is defined (using the Value Definition Syntax in the Values & Units spec) as:

```
<a id="anb-production"></a><a-n-plus-b> =
  odd | even |
  <integer> |

  <n-dimension> |
  '+'?^† n |
  -n |

  <ndashdigit-dimension> |
  '+'?^† <ndashdigit-ident> |
  <dashndashdigit-ident> |

  <n-dimension> <signed-integer> |
  '+'?^† n <signed-integer> |
  -n <signed-integer> |

  <ndash-dimension> <signless-integer> |
  '+'?^† n- <signless-integer> |
  -n- <signless-integer> |

  <n-dimension> ['+' | '-'] <signless-integer> |
  '+'?^† n ['+' | '-'] <signless-integer> |
  -n ['+' | '-'] <signless-integer>
```

where:

- *<a id="typedef-n-dimension"></a>`<n-dimension>`* is a [<dimension-token>](#typedef-dimension-token) with its type flag set to "integer", and a unit that is an ASCII case-insensitive match for "n"
- *<a id="typedef-ndash-dimension"></a>`<ndash-dimension>`* is a [<dimension-token>](#typedef-dimension-token) with its type flag set to "integer", and a unit that is an ASCII case-insensitive match for "n-"
- *<a id="typedef-ndashdigit-dimension"></a>`<ndashdigit-dimension>`* is a [<dimension-token>](#typedef-dimension-token) with its type flag set to "integer", and a unit that is an ASCII case-insensitive match for "n-*", where "*" is a series of one or more [digits](#digit)
- *<a id="typedef-ndashdigit-ident"></a>`<ndashdigit-ident>`* is an [<ident-token>](#typedef-ident-token) whose value is an ASCII case-insensitive match for "n-*", where "*" is a series of one or more [digits](#digit)
- *<a id="typedef-dashndashdigit-ident"></a>`<dashndashdigit-ident>`* is an [<ident-token>](#typedef-ident-token) whose value is an ASCII case-insensitive match for "-n-*", where "*" is a series of one or more [digits](#digit)
- *<a id="typedef-integer"></a>`<integer>`* is a [<number-token>](#typedef-number-token) with its type flag set to "integer"
- *<a id="typedef-signed-integer"></a>`<signed-integer>`* is a [<number-token>](#typedef-number-token) with its type flag set to "integer", and a sign character
- *<a id="typedef-signless-integer"></a>`<signless-integer>`* is a [<number-token>](#typedef-number-token) with its type flag set to "integer", and no sign character

<a id="anb-plus"></a> ^†: When a plus sign (+) precedes an ident starting with "n", as in the cases marked above, there must be no whitespace between the two tokens, or else the tokens do not match the above grammar. Whitespace is valid (and ignored) between any other two tokens.

The clauses of the production are interpreted as follows:

**odd **
: *A* is 2, *B* is 1.
**even **
: *A* is 2, *B* is 0.
**`*<integer>*` **
: *A* is 0, *B* is the integer’s value.
**`*<n-dimension>*` **
**`'+'? n` **
**`-n` **
: *A* is the dimension’s value, 1, or -1, respectively. *B* is 0.
**`*<ndashdigit-dimension>*` **
**`'+'? *<ndashdigit-ident>*` **
: *A* is the dimension’s value or 1, respectively. *B* is the dimension’s unit or ident’s value, respectively, with the first code point removed and the remainder interpreted as a base-10 number. B is negative.
**`*<dashndashdigit-ident>*` **
: *A* is -1. *B* is the ident’s value, with the first two code points removed and the remainder interpreted as a base-10 number. B is negative.
**`*<n-dimension>* *<signed-integer>*` **
**`'+'? n *<signed-integer>*` **
**`-n *<signed-integer>*` **
: *A* is the dimension’s value, 1, or -1, respectively. *B* is the integer’s value.
**`*<ndash-dimension>* *<signless-integer>*` **
**`'+'? n- *<signless-integer>*` **
**`-n- *<signless-integer>*` **
: *A* is the dimension’s value, 1, or -1, respectively. *B* is the negation of the integer’s value.
**`*<n-dimension>* ['+' | '-'] *<signless-integer>*` **
**`'+'? n ['+' | '-'] *<signless-integer>*` **
**`-n ['+' | '-'] *<signless-integer>*` **
: *A* is the dimension’s value, 1, or -1, respectively. *B* is the integer’s value. If a `'-'` was provided between the two, *B* is instead the negation of the integer’s value.
## <a id="rule-defs"></a>7. Defining Grammars for Rules and Other Values

[CSS Values 4 § 2 Value Definition Syntax](css-values-4.md#value-defs) defines how to specify a grammar for properties and other CSS syntactic constructions.

### <a id="block-contents"></a>7.1. Defining Block Contents: the [<block-contents>](#typedef-block-contents), [<declaration-list>](#typedef-declaration-list), [<qualified-rule-list>](#typedef-qualified-rule-list), [<declaration-rule-list>](#typedef-declaration-rule-list), and [<rule-list>](#typedef-rule-list) productions

The CSS parser is agnostic as to the contents of blocks—​they’re all [parsed with the same algorithm](#consume-a-blocks-contents), and differentiate themselves solely by what constructs are valid.

When writing a rule grammar, *<a id="typedef-block-contents"></a><block-contents>* represents this agnostic parsing. It must only be used as the sole value in a block, and represents that no restrictions are implicitly placed on what the block can contain.

Accompanying prose must define what is valid and invalid in this context. If any [declarations](#declaration) are valid, and are [property declarations](#css-property-declarations), it must define whether they interact with the cascade; if they do, it must define their specificity and how they use `!important`.

In many cases, however, a block can’t validly contain *any* constructs of a given type. To represent these cases more explicitly, the following productions may be used

-
*<a id="typedef-declaration-list"></a><declaration-list>*: only [declarations](#declaration) are allowed; [at-rules](#at-rule) and [qualified rules](#qualified-rule) are automatically invalid.
-
*<a id="typedef-qualified-rule-list"></a><qualified-rule-list>*: only [qualified rules](#qualified-rule) are allowed; [declarations](#declaration) and [at-rules](#at-rule) are automatically invalid.
-
*<a id="typedef-at-rule-list"></a><at-rule-list>*: only [at-rules](#at-rule) are allowed; [declarations](#declaration) and [qualified rules](#qualified-rule) are automatically invalid.
-
*<a id="typedef-declaration-rule-list"></a><declaration-rule-list>*: [declarations](#declaration) and [at-rules](#at-rule) are allowed; [qualified rules](#qualified-rule) are automatically invalid.
-
*<a id="typedef-rule-list"></a><rule-list>*: [qualified rules](#qualified-rule) and [at-rules](#at-rule) are allowed; [declarations](#declaration) are automatically invalid.

All of these are exactly equivalent to [<block-contents>](#typedef-block-contents) in terms of parsing, but the accompanying prose only has to define validity for the categories that aren’t automatically invalid.

<a id="example-6dc7b979"></a> Some examples of the various productions:

-
A top-level @media uses [<rule-list>](#typedef-rule-list) for its block, while a nested one [[CSS-NESTING-1]](#biblio-css-nesting-1) uses [<block-contents>](#typedef-block-contents).
-
[Style rules](#style-rule) use [<block-contents>](#typedef-block-contents).
-
@font-face uses [<declaration-list>](#typedef-declaration-list).
-
@page uses [<declaration-rule-list>](#typedef-declaration-rule-list).
-
@keyframes uses [<qualified-rule-list>](#typedef-qualified-rule-list)

<a id="example-92c1a511"></a> For example, the grammar for @font-face can be written as:

```
<@font-face> = @font-face { <declaration-list> }
```

and then accompanying prose defines the valid [descriptors](#css-descriptor) allowed in the block.

The grammar for @keyframes can be written as:

```
<@keyframes> = @keyframes { <qualified-rule-list> }
<keyframe-rule> = <keyframe-selector> { <declaration-list> }
```

and then accompanying prose defines that only <keyframe-rule>s are allowed in @keyframes, and that <keyframe-rule>s accept all animatable CSS properties, plus the animation-timing-function property, but they do not interact with the cascade.

### <a id="any-value"></a>7.2. Defining Arbitrary Contents: the [<declaration-value>](#typedef-declaration-value) and [<any-value>](#typedef-any-value) productions

In some grammars, it is useful to accept any reasonable input in the grammar, and do more specific error-handling on the contents manually (rather than simply invalidating the construct, as grammar mismatches tend to do).

For example, custom properties allow any reasonable value, as they can contain arbitrary pieces of other CSS properties, or be used for things that aren’t part of existing CSS at all. For another example, the <general-enclosed> production in Media Queries defines the bounds of what future syntax MQs will allow, and uses special logic to deal with "unknown" values.

To aid in this, two additional productions are defined:

The *<a id="typedef-declaration-value"></a><declaration-value>* production matches *any* sequence of one or more tokens, so long as the sequence does not contain [<bad-string-token>](#typedef-bad-string-token), [<bad-url-token>](#typedef-bad-url-token), unmatched [<)-token>](#tokendef-close-paren), [<]-token>](#tokendef-close-square), or [<}-token>](#tokendef-close-curly), or top-level [<semicolon-token>](#typedef-semicolon-token) tokens or [<delim-token>](#typedef-delim-token) tokens with a value of "!". It represents the entirety of what a valid declaration can have as its value.

The *<a id="typedef-any-value"></a><any-value>* production is identical to [<declaration-value>](#typedef-declaration-value), but also allows top-level [<semicolon-token>](#typedef-semicolon-token) tokens and [<delim-token>](#typedef-delim-token) tokens with a value of "!". It represents the entirety of what valid CSS can be in any context.

## <a id="css-stylesheets"></a>8. CSS stylesheets

To *<a id="parse-a-css-stylesheet"></a>parse a CSS stylesheet*, first [parse a stylesheet](#parse-a-stylesheet). Interpret all of the resulting top-level [qualified rules](#qualified-rule) as [style rules](#style-rule), defined below.

If any style rule is [invalid](#css-invalid), or any at-rule is not recognized or is invalid according to its grammar or context, it’s a [parse error](#parse-error). Discard that rule.

### <a id="style-rules"></a>8.1. Style rules

A *<a id="style-rule"></a>style rule* is a [qualified rule](#qualified-rule) that associates a [selector list](selectors-4.md#selector-list) with a list of property declarations and possibly a list of nested rules. They are also called rule sets in [[CSS2]](#biblio-css2). CSS Cascading and Inheritance [[CSS-CASCADE-3]](#biblio-css-cascade-3) defines how the declarations inside of style rules participate in the cascade.

The prelude of the qualified rule is [parsed](#css-parse-something-according-to-a-css-grammar) as a [<selector-list>](selectors-4.md#typedef-selector-list). If this returns failure, the entire style rule is [invalid](#css-invalid).

The content of the qualified rule’s block is parsed as a [<declaration-list>](#typedef-declaration-list). Qualified rules in this block are also [style rules](#style-rule). Unless defined otherwise by another specification or a future level of this specification, at-rules in that list are [invalid](#css-invalid) and must be ignored.

Note: [[CSS-NESTING-1]](#biblio-css-nesting-1) defines that conditional group rules and some other [at-rules](#at-rule) are allowed inside of [style rules](#style-rule).

Declarations for an unknown CSS property or whose value does not match the syntax defined by the property are [invalid](#css-invalid) and must be ignored. The validity of the style rule’s contents have no effect on the validity of the style rule itself. Unless otherwise specified, property names are ASCII case-insensitive.

Note: The names of Custom Properties [[CSS-VARIABLES]](#biblio-css-variables) are case-sensitive.

[Qualified rules](#qualified-rule) at the top-level of a CSS stylesheet are [style rules](#style-rule). Qualified rules in other contexts may or may not be style rules, as defined by the context.

<a id="example-4acffe04"></a> For example, qualified rules inside @media rules [[CSS3-CONDITIONAL]](#biblio-css3-conditional) are style rules, but qualified rules inside @keyframes rules [[CSS3-ANIMATIONS]](#biblio-css3-animations) are not.

### <a id="at-rules"></a>8.2. At-rules

An *<a id="at-rule"></a>at-rule* is a rule that begins with an at-keyword, and can thus be distinguished from [style rules](#style-rule) in the same context.

[At-rules](#at-rule) are used to:

-
group and structure style rules and other at-rules such as in conditional group rules
-
declare style information that is not associated with a particular element, such as defining counter styles
-
manage syntactic constructs such as imports and [namespaces](#biblio-css-namespaces-3) keyword mappings
-
and serve other miscellaneous purposes not served by a [style rule](#style-rule)

At-rules take many forms, depending on the specific rule and its purpose, but broadly speaking there are two kinds: *<a id="statement-at-rule"></a>statement at-rules* which are simpler constructs that end in a semicolon, and *<a id="block-at-rule"></a>block at-rules* which end in a [{}-block](#curly-block) that can contain nested [qualified rules](#qualified-rule), [at-rules](#at-rule), or [declarations](#declaration).

[Block at-rules](#block-at-rule) will typically contain a collection of (generic or [at-rule](#at-rule)–specific) at-rules, [qualified rules](#qualified-rule), and/or [descriptor declarations](#css-descriptor-declarations) subject to limitations defined by the at-rule. *<a id="css-descriptor"></a>Descriptors* are similar to [properties](css-cascade-5.md#css-property) (and are declared with the same syntax) but are associated with a particular type of at-rule rather than with elements and boxes in the tree.

### <a id="charset-rule"></a>8.3. The [@charset](#at-ruledef-charset) Rule

The algorithm used to [determine the fallback encoding](#determine-the-fallback-encoding) for a stylesheet looks for a specific byte sequence as the very first few bytes in the file, which has the syntactic form of an [at-rule](#at-rule) named "@charset".

However, there is no actual [at-rule](#at-rule) named *<a id="at-ruledef-charset"></a>@charset*. When a stylesheet is actually parsed, any occurrences of an [@charset](#at-ruledef-charset) rule must be treated as an unrecognized rule, and thus dropped as invalid when the stylesheet is grammar-checked.

Note: In CSS 2.1, [@charset](#at-ruledef-charset) was a valid rule. Some legacy specs may still refer to a @charset rule, and explicitly talk about its presence in the stylesheet.

## <a id="serialization"></a>9. Serialization

The tokenizer described in this specification does not produce tokens for comments, or otherwise preserve them in any way. Implementations may preserve the contents of comments and their location in the token stream. If they do, this preserved information must have no effect on the parsing step.

This specification does not define how to serialize CSS in general, leaving that task to the [[CSSOM]](#biblio-cssom) and individual feature specifications. In particular, the serialization of comments and whitespace is not defined.

The only requirement for serialization is that it must "round-trip" with parsing, that is, parsing the stylesheet must produce the same data structures as parsing, serializing, and parsing again, except for consecutive [<whitespace-token>](#typedef-whitespace-token)s, which may be collapsed into a single token.

Note: This exception can exist because CSS grammars always interpret any amount of whitespace as identical to a single space.

<a id="serialization-tables"></a> To satisfy this requirement:

- A [<delim-token>](#typedef-delim-token) containing U+005C REVERSE SOLIDUS (\) must be serialized as U+005C REVERSE SOLIDUS followed by a [newline](#newline). (The tokenizer only ever emits such a token followed by a [<whitespace-token>](#typedef-whitespace-token) that starts with a newline.)
- A [<hash-token>](#typedef-hash-token) with the "unrestricted" type flag may not need as much escaping as the same token with the "id" type flag.
- The unit of a [<dimension-token>](#typedef-dimension-token) may need escaping to disambiguate with scientific notation.
- For any consecutive pair of tokens, if the first token shows up in the row headings of the following table, and the second token shows up in the column headings, and there’s a ✗ in the cell denoted by the intersection of the chosen row and column, the pair of tokens must be serialized with a comment between them.
If the tokenizer preserves comments, and there were comments originally between the token pair, the preserved comment(s) should be used; otherwise, an empty comment (`/**/`) must be inserted. (Preserved comments may be reinserted even if the following tables don’t require a comment between two tokens.)
Single characters in the row and column headings represent a [<delim-token>](#typedef-delim-token) with that value, except for "`(`", which represents a [(-token](#tokendef-open-paren).

|  | ident | function | url | bad url | - | number | percentage | dimension | CDC | ( | * | % |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| ident | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |
| at-keyword | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |  |
| hash | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |  |
| dimension | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |  |
| # | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |  |
| - | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |  |
| number | ✗ | ✗ | ✗ | ✗ |  | ✗ | ✗ | ✗ | ✗ |  |  | ✗ |
| @ | ✗ | ✗ | ✗ | ✗ | ✗ |  |  |  | ✗ |  |  |  |
| . |  |  |  |  |  | ✗ | ✗ | ✗ |  |  |  |  |
| + |  |  |  |  |  | ✗ | ✗ | ✗ |  |  |  |  |
| / |  |  |  |  |  |  |  |  |  |  | ✗ |  |

### <a id="serializing-anb"></a>9.1. Serializing *<a-n-plus-b>*

To *<a id="serialize-an-a-n-plus-b-value"></a>serialize an [<a-n-plus-b>](#anb-production) value*, with integer values *A* and *B*:

1.
If *A* is zero, return the serialization of *B*.
2.
Otherwise, let *result* initially be an empty string.
3.
***A* is `1` **
:
Append "n" to *result*.
***A* is `-1` **
:
Append "-n" to *result*.
***A* is non-zero **
:
Serialize *A* and append it to *result*, then append "n" to *result*.
4.
***B* is greater than zero **
:
Append "+" to *result*, then append the serialization of *B* to *result*.
***B* is less than zero **
:
Append the serialization of *B* to *result*.
5.
Return *result*.

## <a id="privacy"></a>10. Privacy Considerations

This specification introduces no new privacy concerns.

## <a id="security"></a>11. Security Considerations

This specification improves security, in that CSS parsing is now unambiguously defined for all inputs.

Insofar as old parsers, such as whitelists/filters, parse differently from this specification, they are somewhat insecure, but the previous parsing specification left a lot of ambiguous corner cases which browsers interpreted differently, so those filters were potentially insecure already, and this specification does not worsen the situation.

## <a id="changes"></a>12. Changes

*This section is non-normative.*

### <a id="changes-CRD-20261001"></a>12.1. Changes from the 10 October 2026 Candidate Recommendation Draft

-
Removed "parse a block’s contents" (that’s best suited as just a "consume" algo), and replaced it with instead restoring [parse a list of declarations](#parse-a-list-of-declarations), properly suited for style blocks, CSSOM rule bodies, etc.

## <a id="acknowledgments"></a> Acknowledgments

Thanks for feedback and contributions from Anne van Kesteren, David Baron, Elika J. Etemad (fantasai), Henri Sivonen, Johannes Koch, 呂康豪 (Kang-Hao Lu), Marc O’Morain, Raffaello Giulietti, Simon Pieter, Tyler Karaszewski, and Zack Weinberg.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-cascade-3"></a>[CSS-CASCADE-3] **
: Elika Etemad; Tab Atkins Jr.. CSS Cascading and Inheritance Level 3. URL: https://drafts.csswg.org/css-cascade-3/
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-counter-styles-3"></a>[CSS-COUNTER-STYLES-3] **
: Tab Atkins Jr.. CSS Counter Styles Level 3. URL: https://drafts.csswg.org/css-counter-styles/
**<a id="biblio-css-fonts-4"></a>[CSS-FONTS-4] **
: Chris Lilley. CSS Fonts Module Level 4. URL: https://drafts.csswg.org/css-fonts-4/
**<a id="biblio-css-nesting-1"></a>[CSS-NESTING-1] **
: Tab Atkins Jr.. CSS Nesting Module Level 1. URL: https://drafts.csswg.org/css-nesting/
**<a id="biblio-css-typed-om-1"></a>[CSS-TYPED-OM-1] **
: Tab Atkins Jr.; François Remy. CSS Typed OM Level 1. URL: https://drafts.css-houdini.org/css-typed-om-1/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css3-conditional"></a>[CSS3-CONDITIONAL] **
: Chris Lilley; David Baron; Elika Etemad. CSS Conditional Rules Module Level 3. URL: https://drafts.csswg.org/css-conditional-3/
**<a id="biblio-cssom"></a>[CSSOM] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-encoding"></a>[ENCODING] **
: Anne van Kesteren. Encoding Standard. Living Standard. URL: https://encoding.spec.whatwg.org/
**<a id="biblio-html"></a>[HTML] **
: Ian Hickson. HTML. Living Standard. URL: http://whatwg.org/html
**<a id="biblio-infra"></a>[INFRA] **
: Anne van Kesteren; Domenic Denicola. Infra Standard. Living Standard. URL: https://infra.spec.whatwg.org/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-selectors-4"></a>[SELECTORS-4] **
: Elika Etemad; Tab Atkins Jr.. [Selectors Level 4](selectors-4.md). URL: [https://drafts.csswg.org/selectors/](selectors-4.md)
**<a id="biblio-url"></a>[URL] **
: Anne van Kesteren. URL Standard. Living Standard. URL: https://url.spec.whatwg.org/
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css-cascade-6"></a>[CSS-CASCADE-6] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. CSS Cascading and Inheritance Level 6. URL: https://drafts.csswg.org/css-cascade-6/
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-color-5"></a>[CSS-COLOR-5] **
: Chris Lilley; Una Kravets; Lea Verou. CSS Color Module Level 5. URL: https://drafts.csswg.org/css-color-5/
**<a id="biblio-css-fonts-5"></a>[CSS-FONTS-5] **
: Chris Lilley. CSS Fonts Module Level 5. URL: https://drafts.csswg.org/css-fonts-5/
**<a id="biblio-css-namespaces-3"></a>[CSS-NAMESPACES-3] **
: Elika Etemad. CSS Namespaces Module Level 3. URL: https://drafts.csswg.org/css-namespaces/
**<a id="biblio-css-page-3"></a>[CSS-PAGE-3] **
: Elika Etemad. CSS Paged Media Module Level 3. URL: https://drafts.csswg.org/css-page-3/
**<a id="biblio-css-text-decor-3"></a>[CSS-TEXT-DECOR-3] **
: Elika Etemad; Koji Ishii. CSS Text Decoration Module Level 3. URL: https://drafts.csswg.org/css-text-decor-3/
**<a id="biblio-css-text-decor-4"></a>[CSS-TEXT-DECOR-4] **
: Elika Etemad; Koji Ishii. CSS Text Decoration Module Level 4. URL: https://drafts.csswg.org/css-text-decor-4/
**<a id="biblio-css-transforms-1"></a>[CSS-TRANSFORMS-1] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css-values-5"></a>[CSS-VALUES-5] **
: Tab Atkins Jr.; Elika Etemad; Miriam Suzanne. CSS Values and Units Module Level 5. URL: https://drafts.csswg.org/css-values-5/
**<a id="biblio-css-variables"></a>[CSS-VARIABLES] **
: Tab Atkins Jr.. CSS Custom Properties for Cascading Variables Module Level 1. URL: https://drafts.csswg.org/css-variables/
**<a id="biblio-css-variables-2"></a>[CSS-VARIABLES-2] **
: CSS Custom Properties for Cascading Variables Module Level 2. Editor's Draft. URL: https://drafts.csswg.org/css-variables-2/
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3-animations"></a>[CSS3-ANIMATIONS] **
: David Baron; et al. CSS Animations Level 1. URL: https://drafts.csswg.org/css-animations/
**<a id="biblio-mediaq"></a>[MEDIAQ] **
: Tab Atkins Jr.; Florian Rivoal. Media Queries Level 4. URL: https://drafts.csswg.org/mediaqueries-4/
**<a id="biblio-mediaqueries-5"></a>[MEDIAQUERIES-5] **
: Tab Atkins Jr.; et al. Media Queries Level 5. URL: https://drafts.csswg.org/mediaqueries-5/
**<a id="biblio-select"></a>[SELECT] **
: Tantek Çelik; et al. Selectors Level 3. URL: https://drafts.csswg.org/selectors-3/
