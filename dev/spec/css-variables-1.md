<!-- https://drafts.csswg.org/css-variables-1/, as of 2026-10-19; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>CSS Custom Properties for Cascading Variables Module Level 1

<a id="w3c-state"></a>Editor’s Draft, 9 October 2026

More details about this document

**This version: **
: https://drafts.csswg.org/css-variables/
**Latest published version: **
: https://www.w3.org/TR/css-variables-1/
**Previous Versions: **
: https://www.w3.org/TR/2022/CR-css-variables-1-20220616/
: https://www.w3.org/TR/2021/CRD-css-variables-1-20211111/
: https://www.w3.org/TR/2015/CR-css-variables-1-20151203/
: https://www.w3.org/TR/2014/WD-css-variables-1-20140506/
: https://www.w3.org/TR/2013/WD-css-variables-1-20130620/
: https://www.w3.org/TR/2013/WD-css-variables-20130312/
: https://www.w3.org/TR/2012/WD-css-variables-20120410/
**Implementation Report: **
: https://wpt.fyi/results/css/css-variables
**Feedback: **
: CSSWG Issues Repository
**Editor: **
: Tab Atkins Jr. (Google)
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/css-variables/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

This module introduces cascading variables as a new primitive value type that is accepted by all CSS properties, and custom properties for defining them.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “css-variables” in the title, like this: “[css-variables] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

## <a id="intro"></a>1. Introduction

*This section is not normative.*

Large documents or applications (and even small ones) can contain quite a bit of CSS. Many of the values in the CSS file will be duplicate data; for example, a site may establish a color scheme and reuse three or four colors throughout the site. Altering this data can be difficult and error-prone, since it’s scattered throughout the CSS file (and possibly across multiple files), and may not be amenable to Find-and-Replace.

This module introduces a family of custom author-defined properties known collectively as [custom properties](#custom-property), which allow an author to assign arbitrary values to a property with an author-chosen name, and the [var()](#funcdef-var) function, which allow an author to then use those values in other properties elsewhere in the document. This makes it easier to read large files, as seemingly-arbitrary values now have informative names, and makes editing such files much easier and less error-prone, as one only has to change the value once, in the custom property, and the change will propagate to all uses of that variable automatically.

### <a id="values"></a>1.1. Value Definitions

This specification follows the CSS property definition conventions from [[CSS2]](#biblio-css2) using the value definition syntax from [[CSS-VALUES-3]](#biblio-css-values-3). Value types not defined in this specification are defined in CSS Values & Units [[CSS-VALUES-3]](#biblio-css-values-3). Combination with other CSS modules may expand the definitions of these value types.

In addition to the property-specific values listed in their definitions, all properties defined in this specification also accept the [CSS-wide keywords](css-values-4.md#css-wide-keywords) as their property value. For readability they have not been repeated explicitly.

## <a id="defining-variables"></a>2. Defining Custom Properties: the --* family of properties

This specification defines an open-ended set of properties called [custom properties](#custom-property), which, among other things, are used to define the substitution value of [var()](#funcdef-var) functions.

| Name: | *<a id="propdef-"></a>--** |
| --- | --- |
| [Value:](css-values-4.md#value-defs) | [<declaration-value>](css-syntax-3.md#typedef-declaration-value)[?](css-values-4.md#mult-opt) |
| [Initial:](css-cascade-5.md#initial-values) | the [guaranteed-invalid value](#guaranteed-invalid-value) |
| [Applies to:](css-cascade-5.md#applies-to) | all elements and all pseudo-elements (including those with restricted property lists) |
| [Inherited:](css-cascade-5.md#inherited-property) | yes |
| [Percentages:](css-values-4.md#percentages) | n/a |
| [Computed value:](css-cascade-5.md#computed) | specified value with variables substituted, or the [guaranteed-invalid value](#guaranteed-invalid-value) |
| Canonical order: | per grammar |
| Animation type: | discrete |

*User agents are expected to support this property on all media, including non-visual ones.*

A *<a id="custom-property"></a>custom property* is any property whose name starts with two dashes (U+002D HYPHEN-MINUS), like --foo. The *<a id="typedef-custom-property-name"></a><custom-property-name>* production corresponds to this: it’s defined as any [<dashed-ident>](css-values-4.md#typedef-dashed-ident) (a valid [identifier](css-values-4.md#css-css-identifier) that starts with two dashes), except -- itself, which is reserved for future use by CSS. [Custom properties](#custom-property) are solely for use by authors and users; CSS will never give them a meaning beyond what is presented here.

<a id="example-81b6bcdd"></a> Custom properties define variables, referenced with the [var()](#funcdef-var) notation, which can be used for many purposes. For example, a page that consistently uses a small set of colors in its design can store the colors in custom properties and use them with variables:

```
:root {
  --main-color: #06c;
  --accent-color: #006;
}
/* The rest of the CSS file */
#foo h1 {
  color: var(--main-color);
}
```

The naming provides a mnemonic for the colors, prevents difficult-to-spot typos in the color codes, and if the theme colors are ever changed, focuses the change on one simple spot (the custom property value) rather than requiring many edits across all stylesheets in the webpage.

Unlike other CSS properties, custom property names are *not* ASCII case-insensitive. Instead, custom property names are only equal to each other if they are identical to each other.

<a id="example-c4b03623"></a> While both --foo and --FOO are valid, they are distinct properties —​using var(--foo) will refer to the first one, while using var(--FOO) will refer to the second.

Perhaps more surprisingly, --foó and --foó are distinct properties. The first is spelled with U+00F3 (LATIN SMALL LETTER O WITH ACUTE) while the second is spelled with an ASCII "o" followed by U+0301 (COMBINING ACUTE ACCENT), and the "identical to" relation uses direct codepoint-by-codepoint comparison to determine if two strings are equal, to avoid the complexities and pitfalls of unicode normalization and locale-specific collation.

Operating systems, keyboards, or input methods sometimes encode visually-identical text using different codepoint sequences. Authors are advised to choose variable names that avoid potential confusion or to use escapes and other means to ensure that similar appearing sequences are identical. See Section 2.3 in [[CHARMOD-NORM]](#biblio-charmod-norm) for examples.

<a id="example-120efb12"></a> Developers maintaining the following CSS might be confused why the test patch is red:

```
--fijord: red;
--fĳord: green;
--ﬁjord: blue;

.test {
  background-color: var(--fijord);
}
```

The reason is that the first custom property uses the character sequence LATIN SMALL LETTER F + LATIN SMALL LETTER I + LATIN SMALL LETTER J; the second, identical-looking one uses the character sequence LATIN SMALL LETTER F + LATIN SMALL LIGATURE IJ while the third uses the character sequence LATIN SMALL LIGATURE FI + LATIN SMALL LETTER J.

So the CSS contains three distinct custom properties, two of which are unused.

Custom properties are **not** reset by the [all](css-cascade-5.md#propdef-all) property. We may define a property in the future that resets all variables.

The [CSS-wide keywords](css-values-4.md#css-wide-keywords) can be used in custom properties, with the same meaning as in any another property.

Note: That is, they’re interpreted at cascaded-value time as normal, and are not preserved as the custom property’s value, and thus are not substituted in by the corresponding variable.

Note: While this module focuses on the use of [custom properties](#custom-property) with the [var()](#funcdef-var) function to create “variables”, they can also be used as actual custom properties, parsed by and acted on by script. It’s expected that the CSS Extensions spec [[CSS-EXTENSIONS]](#biblio-css-extensions) will expand on these use-cases and make them easier to do.

Custom properties are ordinary properties, so they can be declared on any element, are resolved with the normal inheritance and cascade rules, can be made conditional with @media and other conditional rules, can be used in HTML’s `style` attribute, can be read or set using the CSSOM, etc.

Notably, they can even be animated, but since the UA has no way to interpret their contents, they always use the "flips at 50%" behavior that is used for any other pair of values that can’t be intelligently interpolated. However, any [custom property](#custom-property) used in a @keyframes rule becomes *<a id="animation-tainted"></a>animation-tainted*, which affects how it is treated when referred to via the [var()](#funcdef-var) function in an animation property.

Note: Like any other property that animates discretely, custom properties can’t be transitioned. Registered custom properties can, however, if given a syntax that has non-discrete animation behavior.

[Animation-tainted](#animation-tainted) is "infectious": custom properties which reference animation-tainted properties also become animation-tainted.

<a id="example-d6247b0f"></a> This style rule:

```
:root {
  --header-color: #06c;
}
```

declares a [custom property](#custom-property) named --header-color on the root element, and assigns to it the value "#06c". This property is then inherited to the elements in the rest of the document. Its value can be referenced with the [var()](#funcdef-var) function:

```
h1 { background-color: var(--header-color); }
```

The preceding rule is equivalent to writing [background-color: #06c;](css-backgrounds-3.md#propdef-background-color), except that the variable name makes the origin of the color clearer, and if var(--header-color) is used on other elements in the document, all of the uses can be updated at once by changing the --header-color property on the root element.

<a id="example-2a0a82cc"></a> If a [custom property](#custom-property) is declared multiple times, the standard cascade rules help resolve it. Variables always draw from the computed value of the associated custom property on the same element:

```
:root { --color: blue; }
div { --color: green; }
#alert { --color: red; }
* { color: var(--color); }

<p>I inherited blue from the root element!</p>
<div>I got green set directly on me!</div>
<div id='alert'>
  While I got red set directly on me!
  <p>I'm red too, because of inheritance!</p>
</div>
```

<a id="example-8f013eba"></a> A real-world example of [custom property](#custom-property) usage is easily separating out strings from where they’re used, to aid in maintenance of internationalization:

```
:root,
:root:lang(en) {--external-link: "external link";}
:root:lang(el) {--external-link: "εξωτερικός σύνδεσμος";}

a[href^="http"]::after {content: " (" var(--external-link) ")"}
```

The variable declarations can even be kept in a separate file, to make maintaining the translations simpler.

### <a id="syntax"></a>2.1. Custom Property Value Syntax

The allowed syntax for [custom properties](#custom-property) is extremely permissive. The [<declaration-value>](css-syntax-3.md#typedef-declaration-value) production matches *any* sequence of one or more tokens, so long as the sequence does not contain [<bad-string-token>](css-syntax-3.md#typedef-bad-string-token), [<bad-url-token>](css-syntax-3.md#typedef-bad-url-token), unmatched [<)-token>](css-syntax-3.md#tokendef-close-paren), [<]-token>](css-syntax-3.md#tokendef-close-square), or [<}-token>](css-syntax-3.md#tokendef-close-curly), or top-level [<semicolon-token>](css-syntax-3.md#typedef-semicolon-token) tokens or [<delim-token>](css-syntax-3.md#typedef-delim-token) tokens with a value of "!".

In addition, if the value of a [custom property](#custom-property) contains a [var()](#funcdef-var) reference, the var() reference must be valid according to the specified var() grammar. If not, the custom property is invalid and must be ignored.

Note: This definition, along with the general CSS syntax rules, implies that a custom property value never includes an unmatched quote or bracket, and so cannot have any effect on larger syntax constructs, like the enclosing style rule, when reserialized.

Note: Custom properties can contain a trailing !important, but this is automatically removed from the property’s value by the CSS parser, and makes the custom property "important" in the CSS cascade. In other words, the prohibition on top-level "!" characters does not prevent !important from being used, as the !important is removed before syntax checking happens.

<a id="example-fb41560d"></a> For example, the following is a valid custom property:

```
--foo: if(x > 5) this.width = 10;
```

While this value is obviously useless as a *variable*, as it would be invalid in any normal property, it might be read and acted on by JavaScript.

The values of custom properties, and the values of [var()](#funcdef-var) functions substituted into custom properties, are *case-sensitive*, and must be preserved in their original author-given casing. (Many CSS values are ASCII case-insensitive, which user agents can take advantage of by "canonicalizing" them into a single casing, but that isn’t allowed for custom properties.)

Because custom properties can contain *anything*, there is no general way to know how to interpret what’s inside of them (until they’re substituted into a known property with [var()](#funcdef-var)). Rather than have them *partially* resolve in some cases but not others, they’re left completely unresolved; they’re a bare stream of [CSS tokens](css-syntax-3.md#tokenization) interspersed with var() functions.

This has some knock-on implications. For example, relative URLs in CSS are resolved against the base URL of the stylesheet the value appears in. However, if a custom property like --my-image: url(foo.jpg); shows up in an `"/a/style.css"` stylesheet, it will not resolve into an absolute URL immediately; if that variable is later used in a *different* `"/b/style.css"` stylesheet like [background: var(--my-image);](css-backgrounds-3.md#propdef-background), it will resolve *at that point* to `"/b/foo.jpg"`.

### <a id="guaranteed-invalid"></a>2.2. Guaranteed-Invalid Values

The initial value of a [custom property](#custom-property) is a *<a id="guaranteed-invalid-value"></a>guaranteed-invalid value*.

The [guaranteed-invalid value](#guaranteed-invalid-value) is, well, guaranteed to be invalid. If it ever appears in a property value, then at [computed value](css-cascade-5.md#computed-value) time that property becomes invalid at computed-value time.

Non-property contexts will define their own behavior for the [guaranteed-invalid value](#guaranteed-invalid-value), but it will always be "invalid" in some sense.

The [guaranteed-invalid value](#guaranteed-invalid-value) serializes as the empty string, but actually writing an empty value into a custom property, like --foo:;, is a valid (empty) value, not the guaranteed-invalid value. If, for whatever reason, one wants to manually reset a [custom property](#custom-property) to the guaranteed-invalid value, using the keyword [initial](css-cascade-5.md#valdef-all-initial) will do this.

Note: Other than invoking the [initial value](css-cascade-5.md#initial-value) of a non-registered [custom property](#custom-property), the only way to create the [guaranteed-invalid value](#guaranteed-invalid-value) is by having an invalid arbitrary substitution function.

## <a id="using-variables"></a>3. Using Cascading Variables: the [var()](#funcdef-var) notation

The value of a [custom property](#custom-property) can be substituted into the value of another property with the [var()](#funcdef-var) function. The syntax of var() is:

```
<a id="funcdef-var"></a>var() = var( <custom-property-name> , <declaration-value>? )
```

The [var()](#funcdef-var) function is an arbitrary substitution function, and its argument grammar is:

```
<a id="typedef-var-args"></a><var-args> = var( <declaration-value> , <declaration-value>? )
```

The first argument to the function is the name of the [custom property](#custom-property) to be substituted. The second argument to the function, if provided, is a fallback value, which is used as the substitution value when the value of the referenced custom property is the [guaranteed-invalid value](#guaranteed-invalid-value).

In an exception to the usual [comma elision rules](css-values-4.md#comb-comma), which require commas to be omitted when they’re not separating values, a bare comma, with nothing following it, must be treated as valid in [var()](#funcdef-var), indicating an empty fallback value.

Note: That is, var(--a,) is a valid function, specifying that if the --a custom property is invalid or missing, the [var()](#funcdef-var) should be replaced with nothing.

Note: The syntax of the fallback, like that of [custom properties](#custom-property), allows commas. For example, var(--foo, red, blue) defines a fallback of red, blue; that is, anything between the first comma and the end of the function is considered a fallback value.

<a id="example-054c13d3"></a> The fallback value allows for some types of defensive coding. For example, an author may create a component intended to be included in a larger application, and use variables to style it so that it’s easy for the author of the larger application to theme the component to match the rest of the app.

Without fallback, the app author must supply a value for every variable that your component uses. With fallback, the component author can supply defaults, so the app author only needs to supply values for the variables they wish to override.

```
/* In the component's style: */
.component .header {
  color: var(--header-color, blue);
}
.component .text {
  color: var(--text-color, black);
}

/* In the larger application's style: */
.component {
  --text-color: #080;
  /* header-color isn't set,
     and so remains blue,
     the fallback value */
}
```

To *<a id="replace-a-var-function"></a>replace a var() function*, given a list of *arguments*:

1.
Let *el* be the element that the style containing the [var()](#funcdef-var) function is being applied to. Let *first arg* be the first [<declaration-value>](css-syntax-3.md#typedef-declaration-value) in *arguments*. Let *second arg* be the <declaration-value>? passed after the comma, or null if there was no comma.
2.
Substitute arbitrary substitution functions in *first arg*, then [parse](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) it as a [<custom-property-name>](#typedef-custom-property-name). If parsing returned a <custom-property-name>, let *result* be the [computed value](css-cascade-5.md#computed-value) of the corresponding [custom property](#custom-property) on *el*. Otherwise, let *result* be the [guaranteed-invalid value](#guaranteed-invalid-value).
Note: Determining the [computed value](css-cascade-5.md#computed-value) for the [custom property](#custom-property) implies that property replacement takes place, which may cause a cycle.
3.
If the [custom property](#custom-property) named by the [var()](#funcdef-var)’s first argument is [animation-tainted](#animation-tainted), and the var() is being used in a property that is not animatable, set *result* to the [guaranteed-invalid value](#guaranteed-invalid-value).
4.
If *result* contains the [guaranteed-invalid value](#guaranteed-invalid-value), and *second arg* was provided, set *result* to the result of substitute arbitrary substitution functions on *second arg*.
5.
Return *result*.

<a id="example-0e8dabdc"></a> Due to property replacement, [custom properties](#custom-property) can form cycles:

```
:root {
  --one: calc(var(--two) + 20px);
  --two: calc(var(--one) - 20px);
}
```

In the above, both --one and --two compute to the [guaranteed-invalid value](#guaranteed-invalid-value), since their substitution contexts have been marked as cyclic.

<a id="example-1214ecd5"></a> Note that the [custom property](#custom-property) name looked up by a [var()](#funcdef-var) may itself come from a var() function:

```
:root {
  --other: 10px;
  --myvar: --other;
  --result: var(var(--myvar));
}
```

Since the inner var() is resolved before the outer var(), the computed value of --result becomes 10px.

## <a id="apis"></a>4. APIs

All [custom property](#custom-property) declarations have the case-sensitive flag set.

Note: Custom properties do not appear on a CSSStyleDeclaration object in camel-cased form, because their names may have both upper and lowercase letters which indicate distinct custom properties. The sort of text transformation that automatic camel-casing performs is incompatible with this. They can still be accessed by their proper name via getPropertyValue()/etc.

### <a id="serializing-custom-props"></a>4.1. Serializing Custom Properties

Custom property names must be serialized as the exact code point sequence provided by the author, including not altering the case.

Note: For non-custom properties, property names are restricted to the ASCII range and are ASCII case-insensitive, so implementations typically serialize the name lowercased.

Specified values of [custom properties](#custom-property) must be serialized *exactly as specified by the author* (with one exception, below). Simplifications that might occur in other properties, such as dropping comments, normalizing whitespace, reserializing numeric tokens from their value, etc., must not occur.

Computed values of [custom properties](#custom-property) must similarly be serialized *exactly as specified by the author* (with the same one exception, below), save for the replacement of any [var()](#funcdef-var) functions.

The one exception to serialization being precisely as specified in the source file is that an *empty* custom property (such as --foo:;) must serialize with a single space as its value.

<a id="example-55f61182"></a> For example, given the following properties:

```
--y: /* baz */;
--x: /* foo */ var(--y) /* bar */;
```

the serialization of the specified value of --x must be `"/* foo */ var(--y) /* bar */"`, while the serialization of the computed value of --x must be `"/* foo */ /* baz */ /* bar */"`.

(Note that the leading whitespace on the value is automatically trimmed by the CSS parser; it’s not preserved here.)

This "exactly as specified" requirement exists because authors sometimes store non-CSS information in custom properties, and "normalizing" this information can change it in ways that break author code.

For example, storing a UUID in a custom property, like --uuid: 12345678-12e3-8d9b-a456-426614174000, requires the UUID to be echoed back out as written when it’s accessed by script.

This value is technically parsed by CSS as a series of adjacent numbers and dimensions. In particular, the segment "-12e3" parses as a number, equal to -12000. Reserializing it in that form, as required by CSSOM in other contexts, would fatally break the author’s use of the value.

The requirement to serialize an empty custom property as a string with a single space in it instead is due to compatibility with the existing CSSOM API. `setProperty()` specifies that if the passed value is an empty string it instead acts as `removeProperty()`.

This is a frankly weird behavior, and not something we would do in new APIs. But we can’t change the past, and letting empty properties accurately serialize as the empty string would mean that round-tripping a custom property value, or reading a property on one element and setting a property on another element to the same value, wouldn’t work as intended. Using a single space preserves the author-intended behavior as much as possible, while having the smallest chance of accidentally breaking things, since authors working with custom property values in JS are probably calling `trim()` on the values anyway.

## <a id="changes"></a>5. Changes

### <a id="changes-20220616"></a>5.1. Changes Since the 16 June 2022 CR Snapshot

-
Clarified that the comment-insertion can happen with 0+ comments between the original tokens, not just exactly 1.
-
Clarified the transition behavior of custom properties, in a note
-
Made [var()](#funcdef-var) short-circuiting. (Issue 11500)

### <a id="changes-20211111"></a>5.2. Changes Since the 11 November 2021 CR Draft

-
Clarified that custom properties apply all pseudo-elements (including those with restricted property lists)
-
Added example to illustrate issues with combining characters, ligatures, etc
-
Strengthened wording around similar-appearing variable names that use distinct codepoint sequences
-
Clarified an example by using more visually distinct languages as examples (English and Greek)
-
Split Security and Privacy into separate sections

### <a id="changes-20151203"></a>5.3. Changes Since the 03 December 2015 CR

-
Now that [[css-syntax-3]](#biblio-css-syntax-3) auto-trims whitespace from declaration values, made [<declaration-value>](css-syntax-3.md#typedef-declaration-value) optional in the custom property grammar, so that empty variables are still allowed. (Issue 774)
-
Similarly, made empty fallbacks valid in [var()](#funcdef-var).
-
The -- property is reserved for future use by CSS.
-
Added concept of "animation-tainted", to prevent non-animatable properties from using a variable to smuggle in some animatability.
-
Defined the [guaranteed-invalid value](#guaranteed-invalid-value) to make the initial value of custom properties and the result of cycles or substitution failure more straightforward, and allow failure to propagate thru substitutions until finally intercepted by a fallback.
-
Defined that cycles trigger invalid at computed-value time behavior.
-
Allowed variables to resolve to a CSS-wide keyword (only possible by providing it as a fallback).
-
Clarified that registered custom properties act like non-custom properties when they’re invalid at computed-value time.
-
Made longhands with [var()](#funcdef-var)s also trigger their shorthands to be unserializable, like longhands with pending-substitution values already did.
-
Required UAs to defend against exponential substitution attacks.
-
Defined how to serialize the *values* of custom properties (previously, only the property name’s serialization was specified).

### <a id="changes-20140506"></a>5.4. Changes since the May 6 2014 Last Call Working Draft

-
Serialization of longhands when shorthand uses a variable was defined.
-
Link to DOM’s definition of "case-sensitive".
-
Added example of using variables with [:lang()](selectors-4.md#lang-pseudo) to do simple i18n.
-
Clarified that usage of [var()](#funcdef-var) in a custom property must be valid per the var() grammar.

## <a id="acks"></a>6. Acknowledgments

Many thanks to several people in the CSS Working Group for keeping the dream of variables alive over the years, particularly Daniel Glazman and David Hyatt. Thanks to multiple people on the mailing list for helping contribute to the development of this incarnation of variables, particularly Brian Kardell, David Baron, François Remy, Roland Steiner, and Shane Stephens.

## <a id="privacy"></a>7. Privacy Considerations

This specification defines a purely author-level mechanism for passing styling information around within a page they control. As such, there are no new privacy considerations.

## <a id="security"></a>8. Security Considerations

CSS Values 5 §  Safely Handling Overly-Long Substitution calls out a long-standing Denial-of-Service attack that can be mounted against "macro-expansion"-like mechanisms, such as the [var()](#funcdef-var) function, and mandates a defense against that attack.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-animations-1"></a>[CSS-ANIMATIONS-1] **
: David Baron; et al. CSS Animations Level 1. URL: https://drafts.csswg.org/css-animations/
**<a id="biblio-css-cascade-5"></a>[CSS-CASCADE-5] **
: Elika Etemad; Miriam Suzanne; Tab Atkins Jr.. [CSS Cascading and Inheritance Level 5](css-cascade-5.md). URL: [https://drafts.csswg.org/css-cascade-5/](css-cascade-5.md)
**<a id="biblio-css-conditional-3"></a>[CSS-CONDITIONAL-3] **
: Chris Lilley; David Baron; Elika Etemad. CSS Conditional Rules Module Level 3. URL: https://drafts.csswg.org/css-conditional-3/
**<a id="biblio-css-properties-values-api-1"></a>[CSS-PROPERTIES-VALUES-API-1] **
: Tab Atkins Jr.; Alan Stearns; Greg Whitworth. CSS Properties and Values API Level 1. URL: https://drafts.css-houdini.org/css-properties-values-api-1/
**<a id="biblio-css-syntax-3"></a>[CSS-SYNTAX-3] **
: Tab Atkins Jr.; Simon Sapin. [CSS Syntax Module Level 3](css-syntax-3.md). URL: [https://drafts.csswg.org/css-syntax/](css-syntax-3.md)
**<a id="biblio-css-values-3"></a>[CSS-VALUES-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Values and Units Module Level 3. URL: https://drafts.csswg.org/css-values-3/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-values-5"></a>[CSS-VALUES-5] **
: Tab Atkins Jr.; Elika Etemad; Miriam Suzanne. CSS Values and Units Module Level 5. URL: https://drafts.csswg.org/css-values-5/
**<a id="biblio-css2"></a>[CSS2] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-cssom-1"></a>[CSSOM-1] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-infra"></a>[INFRA] **
: Anne van Kesteren; Domenic Denicola. Infra Standard. Living Standard. URL: https://infra.spec.whatwg.org/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-selectors-4"></a>[SELECTORS-4] **
: Elika Etemad; Tab Atkins Jr.. [Selectors Level 4](selectors-4.md). URL: [https://drafts.csswg.org/selectors/](selectors-4.md)
**<a id="biblio-web-animations-1"></a>[WEB-ANIMATIONS-1] **
: Brian Birtles; et al. Web Animations. URL: https://drafts.csswg.org/web-animations-1/
### <a id="informative"></a>Non-Normative References

**<a id="biblio-charmod-norm"></a>[CHARMOD-NORM] **
: Addison Phillips; et al. Character Model for the World Wide Web: String Matching. URL: https://w3c.github.io/charmod-norm/
**<a id="biblio-css-backgrounds-3"></a>[CSS-BACKGROUNDS-3] **
: Elika Etemad; Brad Kemper. [CSS Backgrounds and Borders Module Level 3](css-backgrounds-3.md). URL: [https://drafts.csswg.org/css-backgrounds/](css-backgrounds-3.md)
**<a id="biblio-css-extensions"></a>[CSS-EXTENSIONS] **
: Tab Atkins Jr.. CSS Extensions. ED. URL: https://drafts.csswg.org/css-extensions/
**<a id="biblio-ecmascript"></a>[ECMASCRIPT] **
: ECMAScript Language Specification. URL: https://tc39.es/ecma262/multipage/
## <a id="property-index"></a>Property Index

| Name | Value | Initial | Applies to | Inh. | %ages | Anim­ation type | Canonical order |  |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| --* | <declaration-value>? | the guaranteed-invalid value | all elements and all pseudo-elements (including those with restricted property lists) | yes | n/a | discrete | per grammar | specified value with variables substituted, or the guaranteed-invalid value |

**✔**MDN

**✔**MDN
