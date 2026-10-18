<!-- https://drafts.csswg.org/selectors-4/, as of 2026-10-18; written by dev/spec_text.py -->

[image: W3C]

# <a id="title"></a>Selectors Level 4

<a id="w3c-state"></a>Editor’s Draft, 18 September 2026

More details about this document

**This version: **
: https://drafts.csswg.org/selectors/
**Latest published version: **
: https://www.w3.org/TR/selectors-4/
**Previous Versions: **
: https://www.w3.org/TR/2026/WD-selectors-4-20260122/
: https://www.w3.org/TR/2022/WD-selectors-4-20221111/
: https://www.w3.org/TR/2022/WD-selectors-4-20220507/
: https://www.w3.org/TR/2018/WD-selectors-4-20181121/
: https://www.w3.org/TR/2018/WD-selectors-4-20180202/
: https://www.w3.org/TR/2018/WD-selectors-4-20180201/
: https://www.w3.org/TR/2013/WD-selectors4-20130502/
: https://www.w3.org/TR/2012/WD-selectors4-20120823/
: https://www.w3.org/TR/2011/WD-selectors4-20110929/
**Feedback: **
: CSSWG Issues Repository
: [Inline In Spec](#issues-index)
**Editors: **
: Elika J. Etemad / fantasai (Apple)
: Tab Atkins Jr. (Google)
**Former Editors: **
: Tantek Çelik
: Daniel Glazman
: Ian Hickson
: Peter Linss
: John Williams
**Suggest an Edit for this Spec: **
: GitHub Editor
**Test Suite: **
: https://wpt.fyi/results/css/selectors/
Copyright © 2026 World Wide Web Consortium. W3C^® liability, trademark and permissive document license rules apply.

## <a id="abstract"></a>Abstract

[Selectors](#selector) are patterns that match against elements in a tree, and as such form one of several technologies that can be used to select nodes in a document. Selectors have been optimized for use with HTML and XML, and are designed to be usable in performance-critical code. They are a core component of CSS (Cascading Style Sheets), which uses Selectors to bind style properties to elements in the document. Selectors Level 4 describes the selectors that already exist in [[SELECT]](#biblio-select), and further introduces new selectors for CSS and other languages that may need them.

CSS is a language for describing the rendering of structured documents (such as HTML and XML) on screen, on paper, etc.

## <a id="sotd"></a>Status of this document

This is a public copy of the editors’ draft. It is provided for discussion only and may change at any moment. Its publication here does not imply endorsement of its contents by W3C. Don’t cite this document other than as work in progress.

Please send feedback by filing issues in GitHub (preferred), including the spec code “selectors” in the title, like this: “[selectors] *…summary of comment…*”. All issues and comments are archived. Alternately, feedback can be sent to the (archived) public mailing list www-style@w3.org.

This document is governed by the <a id="w3c_process_revision"></a>18 August 2025 W3C Process Document.

The following features are at-risk, and may be dropped during the CR period:

- [user action pseudo-classes](#user-action-pseudo-class) applying to non-tree-abiding pseudo-elements

“At-risk” is a W3C Process term-of-art, and does not necessarily imply that the feature is in danger of being dropped or delayed. It means that the WG believes the feature may have difficulty being interoperably implemented in a timely manner, and marking it as such allows the WG to drop the feature if necessary when transitioning to the Proposed Rec stage, without having to publish a new Candidate Rec without the feature first.

## <a id="context"></a>1. Introduction

*This section is not normative.*

A [selector](#selector) is a boolean predicate that takes an element in a tree structure and tests whether the element matches the selector or not.

These expressions may be used for many things:

- directly on an element to test whether it matches some criteria, such as in the `element.matches()` function defined in [[DOM]](#biblio-dom)
- applied to an entire tree of elements to filter it into a set of elements that match the criteria, such as in the `document.querySelectorAll()` function defined in [[DOM]](#biblio-dom) or the selector of a CSS style rule.
- used "in reverse" to generate markup that would match a given selector, such as in HAML or Emmet .

Selectors Levels 1, 2, and 3 are defined as the subsets of selector functionality defined in the CSS1, CSS2.1, and Selectors Level 3 specifications, respectively. This module defines Selectors Level 4.

### <a id="placement"></a>1.1. Module Interactions

This module replaces the definitions of and extends the set of selectors defined for CSS in [[SELECT]](#biblio-select) and [[CSS21]](#biblio-css21).

Pseudo-element selectors, which define abstract elements in a rendering tree, are not part of this specification: their generic syntax is described here, but, due to their close integration with the rendering model and irrelevance to other uses such as DOM queries, they will be defined in other modules.

## <a id="overview"></a>2. Selectors Overview

*This section is non-normative, as it merely summarizes the following sections.*

A selector represents a structure. This structure can be used as a condition (e.g. in a CSS rule) that determines which elements a selector matches in the document tree, or as a flat description of the HTML or XML fragment corresponding to that structure.

Selectors may range from simple element names to rich contextual representations.

The following table summarizes the Selector syntax:

<a id="selector-examples"></a>

| Pattern | Represents | Section |  |
| --- | --- | --- | --- |
| `*` | any element | [§ 5.2 Universal selector](#the-universal-selector) | 2 |
| `E` | an element of type E | [§ 5.1 Type (tag name) selector](#type-selectors) |  |
| `E:not(*s1*, *s2*, …)` | an E element that does not match either [compound selector](#compound) *s1* or compound selector *s2* | [§ 4.3 The Negation (Matches-None) Pseudo-class: :not()](#negation) | 3/4 |
| `E:is(*s1*, *s2*, …)` | an E element that matches [compound selector](#compound) *s1* and/or compound selector *s2* | [§ 4.2 The Matches-Any Pseudo-class: :is()](#matches) | 4 |
| `E:where(*s1*, *s2*, …)` | an E element that matches [compound selector](#compound) *s1* and/or compound selector *s2* but contributes no specificity. | [§ 4.4 The Specificity-adjustment Pseudo-class: :where()](#zero-matches) | 4 |
| `E:has(*rs1*, *rs2*, …)` | an E element, if there exists an element that matches either of the [relative selectors](#relative-selector) *rs1* or *rs2*, when evaluated with E as the [anchor elements](#relative-selector-anchor-elements) | [§ 4.5 The Relational Pseudo-class: :has()](#relational) |  |
| `E.warning` | an E element belonging to the class `warning` (the document language specifies how class is determined). | [§ 6.6 Class selectors](#class-html) | 1 |
| `E#myid` | an E element with ID equal to `myid`. | [§ 6.7 ID selectors](#id-selectors) | 1 |
| `E[foo]` | an E element with a `foo` attribute | [§ 6.1 Attribute presence and value selectors](#attribute-representation) | 2 |
| `E[foo="bar"]` | an E element whose `foo` attribute value is exactly equal to `bar` | [§ 6.1 Attribute presence and value selectors](#attribute-representation) | 2 |
| `E[foo="bar" i]` | an E element whose `foo` attribute value is exactly equal to any (ASCII-range) case-permutation of `bar` | [§ 6.3 Case-sensitivity](#attribute-case) | 4 |
| `E[foo="bar" s]` | an E element whose `foo` attribute value is identical to `bar` | [§ 6.3 Case-sensitivity](#attribute-case) | 4 |
| `E[foo~="bar"]` | an E element whose `foo` attribute value is a list of whitespace-separated values, one of which is exactly equal to `bar` | [§ 6.1 Attribute presence and value selectors](#attribute-representation) | 2 |
| `E[foo^="bar"]` | an E element whose `foo` attribute value begins exactly with the string `bar` | [§ 6.2 Substring matching attribute selectors](#attribute-substrings) | 3 |
| `E[foo$="bar"]` | an E element whose `foo` attribute value ends exactly with the string `bar` | [§ 6.2 Substring matching attribute selectors](#attribute-substrings) | 3 |
| `E[foo*="bar"]` | an E element whose `foo` attribute value contains the substring `bar` | [§ 6.2 Substring matching attribute selectors](#attribute-substrings) | 3 |
| `E[foo\|="en"]` | an E element whose `foo` attribute value is a hyphen-separated list of values beginning with `en` | [§ 6.1 Attribute presence and value selectors](#attribute-representation) |  |
| `E:dir(ltr)` | an element of type E with left-to-right directionality (the document language specifies how directionality is determined) | [§ 7.1 The Directionality Pseudo-class: :dir()](#the-dir-pseudo) | 4 |
| `E:lang(sr, "*-Cyrl")` | an element of type E tagged as being in Serbian, (which typically uses Cyrillic, but can be written with Latin characters) but also anything written with Cyrillic characters | [§ 7.2 The Language Pseudo-class: :lang()](#the-lang-pseudo) |  |
| `E:any-link` | an E element being the source anchor of a hyperlink | [§ 8.1 The Hyperlink Pseudo-class: :any-link](#the-any-link-pseudo) | 4 |
| `E:link` | an E element being the source anchor of a hyperlink of which the target is not yet visited | [§ 8.2 The Link History Pseudo-classes: :link and :visited](#link) | 1 |
| `E:visited` | an E element being the source anchor of a hyperlink of which the target is already visited | [§ 8.2 The Link History Pseudo-classes: :link and :visited](#link) | 1 |
| `E:target` | an E element being the target of the current URL | [§ 8.3 The Target Pseudo-class: :target](#the-target-pseudo) | 3 |
| `E:scope` | an E element being a [scoping root](#scoping-root) | [§ 8.4 The Reference Element Pseudo-class: :scope](#the-scope-pseudo) |  |
| `E:active` | an E element that is in an activated state | [§ 9.2 The Activation Pseudo-class: :active](#the-active-pseudo) | 1 |
| `E:hover` | an E element that is under the cursor, or that has a descendant under the cursor | [§ 9.1 The Pointer Hover Pseudo-class: :hover](#the-hover-pseudo) | 2 |
| `E:focus` | an E element that has user input focus | [§ 9.3 The Input Focus Pseudo-class: :focus](#the-focus-pseudo) | 2 |
| `E:focus-within` | an E element that has user input focus or contains an element that has input focus. | [§ 9.5 The Focus Container Pseudo-class: :focus-within](#the-focus-within-pseudo) | 4 |
| `E:focus-visible` | an E element that has user input focus, and the UA has determined that a focus ring or other indicator should be drawn for that element | [§ 9.4 The Focus-Indicated Pseudo-class: :focus-visible](#the-focus-visible-pseudo) |  |
| `E:enabled E:disabled` | a user interface element E that is enabled or disabled, respectively | [§ 12.1.1 The :enabled and :disabled Pseudo-classes](#enableddisabled) | 3 |
| `E:read-write` `E:read-only` | a user interface element E that is user alterable, or not | [§ 12.1.2 The Mutability Pseudo-classes: :read-only and :read-write](#rw-pseudos) | 3-UI/4 |
| `E:placeholder-shown` | an input control currently showing placeholder text | [§ 12.1.3 The Placeholder-shown Pseudo-class: :placeholder-shown](#placeholder) | 3-UI/4 |
| `E:default` | a user interface element E that is the default item in a group of related choices | [§ 12.1.5 The Default-option Pseudo-class: :default](#the-default-pseudo) | 3-UI/4 |
| `E:checked` `E:unchecked` `E:indeterminate` | a user interface element E that is checked/selected (for instance a radio-button or checkbox), unchecked, or in an indeterminate state (neither checked nor unchecked) | [§ 12.2 Input Value States](#input-value-states) | 3 |
| `E:valid` `E:invalid` | a user-input element E that meets, or doesn’t, its data validity semantics | [§ 12.3.1 The Validity Pseudo-classes: :valid and :invalid](#validity-pseudos) | 3-UI/4 |
| `E:in-range` `E:out-of-range` | a user-input element E whose value is in-range/out-of-range | [§ 12.3.2 The Range Pseudo-classes: :in-range and :out-of-range](#range-pseudos) | 3-UI/4 |
| `E:required` `E:optional` | a user-input element E that requires/does not require input | [§ 12.3.3 The Optionality Pseudo-classes: :required and :optional](#opt-pseudos) | 3-UI/4 |
| `E:user-invalid` | a user-altered user-input element E with incorrect input (invalid, out-of-range, omitted-but-required) | [§ 12.3.4 The User-interaction Pseudo-classes: :user-valid and :user-invalid](#user-pseudos) |  |
| `E:root` | an E element, root of the document | [§ 13.1 :root pseudo-class](#the-root-pseudo) | 3 |
| `E:empty` | an E element that has no children (neither elements nor text) except perhaps white space | [§ 13.2 :empty pseudo-class](#the-empty-pseudo) | 3 |
| `E:nth-child(*n* [of *S*]?)` | an E element, the *n*-th child of its parent matching *S* | [§ 13.3.1 :nth-child() pseudo-class](#the-nth-child-pseudo) | 3/4 |
| `E:nth-last-child(*n* [of *S*]?)` | an E element, the *n*-th child of its parent matching *S*, counting from the last one | [§ 13.3.2 :nth-last-child() pseudo-class](#the-nth-last-child-pseudo) | 3/4 |
| `E:first-child` | an E element, first child of its parent | [§ 13.3.3 :first-child pseudo-class](#the-first-child-pseudo) | 2 |
| `E:last-child` | an E element, last child of its parent | [§ 13.3.4 :last-child pseudo-class](#the-last-child-pseudo) | 3 |
| `E:only-child` | an E element, only child of its parent | [§ 13.3.5 :only-child pseudo-class](#the-only-child-pseudo) | 3 |
| `E:nth-of-type(*n*)` | an E element, the *n*-th sibling of its type | [§ 13.4.1 :nth-of-type() pseudo-class](#the-nth-of-type-pseudo) | 3 |
| `E:nth-last-of-type(*n*)` | an E element, the *n*-th sibling of its type, counting from the last one | [§ 13.4.2 :nth-last-of-type() pseudo-class](#the-nth-last-of-type-pseudo) | 3 |
| `E:first-of-type` | an E element, first sibling of its type | [§ 13.4.3 :first-of-type pseudo-class](#the-first-of-type-pseudo) | 3 |
| `E:last-of-type` | an E element, last sibling of its type | [§ 13.4.4 :last-of-type pseudo-class](#the-last-of-type-pseudo) | 3 |
| `E:only-of-type` | an E element, only sibling of its type | [§ 13.4.5 :only-of-type pseudo-class](#the-only-of-type-pseudo) |  |
| `E F` | an F element descendant of an E element | [§ 14.1 Descendant combinator ( )](#descendant-combinators) | 1 |
| `E > F` | an F element child of an E element | [§ 14.2 Child combinator (>)](#child-combinators) | 2 |
| `E + F` | an F element immediately preceded by an E element | [§ 14.3 Next-sibling combinator (+)](#adjacent-sibling-combinators) | 2 |
| `E ~ F` | an F element preceded by an E element | [§ 14.4 Subsequent-sibling combinator (~)](#general-sibling-combinators) | 3 |

Note: Some Level 4 selectors (noted above as "3-UI") were introduced in [[CSS3UI]](#biblio-css3ui).

## <a id="syntax"></a>3. Selector Syntax and Structure

### <a id="structure"></a>3.1. Structure and Terminology

A *<a id="selector"></a>selector* represents a particular pattern of element(s) in a tree structure. The term [selector](#selector) can refer to a [simple selector](#simple), [compound selector](#compound), [complex selector](#complex), or [selector list](#selector-list). The *<a id="selector-subject"></a>subject of a selector* is any element that selector is defined to be about; that is, any element *<a id="match"></a>matching* that selector.

A *<a id="simple"></a>simple selector* is a single condition on an element. A [type selector](#type-selector), [universal selector](#universal-selector), [attribute selector](#attribute-selector), [class selector](#class-selector), [ID selector](#id-selector), or [pseudo-class](#pseudo-class) is a [simple selector](#simple). (It is represented by [<simple-selector>](#typedef-simple-selector) in the selectors [grammar](#grammar).) A given element is said to [match](#match) a simple selector when that simple selector, as defined in this specification and in accordance with the [document language](#document-language), accurately describes the element.

A *<a id="compound"></a>compound selector* is a sequence of [simple selectors](#simple) that are not separated by a [combinator](#selector-combinator), and represents a set of simultaneous conditions on a single element. If it contains a [type selector](#type-selector) or [universal selector](#universal-selector), that selector must come first in the sequence. Only one type selector or universal selector is allowed in the sequence. (A [compound selector](#compound) is represented by [<compound-selector>](#typedef-compound-selector) in the selectors [grammar](#grammar).) A given element is said to [match](#match) a compound selector when it matches all simple selectors in the compound selector.

Note: As whitespace represents the [descendant combinator](#descendant-combinator), no whitespace is allowed between the [simple selectors](#simple) in a [compound selector](#compound).

A *<a id="pseudo-compound"></a>pseudo-compound selector* is a [pseudo-element](#pseudo-element) selector, optionally followed by additional [pseudo-class](#pseudo-class) selectors, and optionally preceded by a [compound selector](#compound) or another [pseudo-compound selector](#pseudo-compound), without any [combinators](#selector-combinator). (A pseudo-compound selector is represented by [<pseudo-compound-selector>](#typedef-pseudo-compound-selector) in the selectors [grammar](#grammar).) A pseudo-element [matches](#match) a pseudo-compound selector when it has the specified pseudo-element name, matches the additional conditions represented by any pseudo-classes, and has an [originating element](#originating-element) represented by the adjacent preceding selector. If there is no adjacent preceding selector, the [universal selector](#universal-selector) is assumed. (For example, .foo ::before is equivalent to .foo *::before, and distinct from .foo::before.)

<a id="example-9e91e72f"></a> For example, in .foo::before:hover, the .foo is a [compound selector](#compound), while the ::before:hover is a [pseudo-compound selector](#pseudo-compound). However, in .foo::before::marker, ::before and ::marker are separate pseudo-compound selectors.

Note: A [pseudo-compound selector](#pseudo-compound) **is not** a [compound selector](#compound), and can’t be used in places that expect a compound selector only. Pseudo-compound selectors act as if they carry a [combinator](#selector-combinator) with themselves, expressing their relationship with their [originating element](#originating-element), just as the [>](#selectordef-child) combinator expresses a relationship with a parent element.

A *<a id="selector-combinator"></a>combinator* is a condition of relationship between two elements represented by the [compound selectors](#compound) on either side. Combinators in Selectors Level 4 include: the [descendant combinator](#descendant-combinator) (white space), the [child combinator](#child-combinator) (U+003E, `>`), the [next-sibling combinator](#next-sibling-combinator) (U+002B, `+`), and the [subsequent-sibling combinator](#subsequent-sibling-combinator) (U+007E, `~`). Two given elements are said to [match](#match) a [combinator](#selector-combinator) when the condition of relationship between these elements is true.

A *<a id="complex"></a>complex selector* is a sequence of one or more [compound selectors](#compound) and/or [pseudo-compound selectors](#pseudo-compound), with compound selectors separated by [combinators](#selector-combinator). It represents a set of simultaneous conditions on a set of elements in the particular relationships described by its combinators. (Complex selectors are represented by [<complex-selector>](#typedef-complex-selector) in the selectors [grammar](#grammar).) A given element or pseudo-element is said to [match](#match) a [complex selector](#complex) when it matches the final compound/pseudo-compound selector in the sequence, and every preceding unit of the sequence also matches an element or [pseudo-element](#pseudo-element), with the correct relationship between consecutive units as expressed by the combinators separating them (or, for pseudo-compound selectors, the correct [originating element](#originating-element) relationship).

<a id="example-3e6d4159"></a> For example, .foo.bar matches an element with both "foo" and "bar" classes.

.ancestor > .foo.bar matches a subset of those elements: only those whose parent element (as indicated by the [>](#selectordef-child) combinator) has the "ancestor" class.

.foo.bar::before matches a ::before pseudo-element, whose [originating element](#originating-element) matches .foo.bar.

A *<a id="list-of-simple-selectors"></a>list of simple/compound/complex selectors* is a comma-separated list of [simple](#simple), [compound](#compound), or [complex selectors](#complex). This is also called just a *<a id="selector-list"></a>selector list* when the type is either unimportant or specified in the surrounding prose; if the type is important and unspecified, it defaults to meaning a [list of complex selectors](#list-of-simple-selectors). (See [§ 4.1 Selector Lists](#grouping) for additional information on [selector lists](#selector-list) and the various <*-selector-list> productions in the [grammar](#grammar) for their formal syntax.) A given element is said to [match](#match) a selector list when it matches any (at least one) of the [selectors](#selector) in that selector list.

### <a id="data-model"></a>3.2. Data Model

Selectors are evaluated against an element tree such as the DOM. [[DOM]](#biblio-dom) Within this specification, this may be referred to as the "document tree" or "source document".

Each element may have any of the following five aspects, which can be selected against, all of which are matched as strings:

- The element’s type (also known as its tag name).
- The element’s namespace.
- An ID.
- Classes (named groups) to which it belongs.
- Attributes, which are name-value pairs.

Many of the selectors depend on the semantics of the *<a id="document-language"></a>document language* (i.e. the language and semantics of the document tree) and/or the semantics of the *<a id="host-language"></a>host language* (i.e. the language that is using selectors syntax). For example, the [:lang()](#lang-pseudo) selector depends on the [document language](#document-language) (e.g. HTML) to define how an element is associated with a language. As a slightly different example, the ::first-line pseudo-element depends on the [host language](#host-language) (e.g. CSS) to define what a ::first-line pseudo-element represents and what it can do.

#### <a id="featureless-elements"></a>3.2.1. Featureless Elements

While individual elements may lack any of the above features, some elements are *<a id="featureless"></a>featureless*. A [featureless](#featureless) element does not match *any selector at all*, except:

-
[simple selectors](#simple) it is explicitly defined to match
-
[compound selectors](#compound), if all contained [simple selectors](#simple) are allowed to match it
-
[complex selectors](#complex), if the [compound selector](#compound) targeting the [subject](#selector-subject) is allowed to match it
-
[selector lists](#selector-list), if at least one selector in the list is allowed to match it
-
[logical combination pseudo-classes](#logical-combination-pseudo-classes), if their argument selector is allowed to match it
-
the [:has()](#has-pseudo) pseudo-class, if and only if the [compound selector](#compound) it’s part of contains at least one *other* [simple selector](#simple) that’s allowed to match it.

If a selector would otherwise match a [featureless](#featureless) element, except for the existence of the default namespace [[CSS-NAMESPACES-3]](#biblio-css-namespaces-3) (because featureless elements do not have a namespace unless otherwise defined), the default namespace does not prevent the match.

<a id="example-351d0bfb"></a> For example, the shadow host in a shadow tree is [featureless](#featureless), and can’t be matched by *any* [pseudo-class](#pseudo-class) except for :host and :host-context() (or combinations including those, such as :is(:host, :root)).

Logical combinations like :not(.foo:host) will never match the host element (even if it doesn’t have a "foo" class), because not all of the simple selectors in .foo:host are allowed to match the shadow host.

Similarly, :not(:host > .foo) will never match the shadow host, even tho the shadow host is indeed *not* a descendant of itself and doesn’t have the "foo" class, because the subject of the complex selector argument (.foo) isn’t allowed to match the shadow host.

<a id="example-a6be590b"></a> In general, you can’t match a [featureless](#featureless) element without explicitly using one of the [simple selectors](#simple) it’s allowed to match, to avoid accidentally selecting one of these elements (which are otherwise *intentionally* easy to not think about). For example, * will never match a featureless element.

The rule for [:has()](#has-pseudo), above, works similarly. Even if a shadow host contains a .foo descendant, :has(.foo) will not match it, because *the rest* of the compound selector (empty) doesn’t contain a simple selector that can match the host. You have to write :host:has(.foo) in order to match the host element.

### <a id="scoping"></a>3.3. Scoped Selectors

Some host applications may choose to *<a id="scoped-selector"></a>scope* selectors to a particular subtree or fragment of the document, The root of the scoping subtree is called the *<a id="scoping-root"></a>scoping root*.

When a selector is [scoped](#scoped-selector), it matches an element only if the element is a descendant of the [scoping root](#scoping-root). (The rest of the selector can match unrestricted; it’s only the final matched elements that must be within the scope.)

<a id="example-32ec254f"></a> For example, the `querySelector()` method defined in [[DOM]](#biblio-dom) allows the author to evaluate a [scoped](#scoped-selector) selector relative to the element it’s called on.

A call like `widget.querySelector("a")` will thus only find `a` elements inside of the `widget` element, ignoring any other `a`s that might be scattered throughout the document.

### <a id="relative"></a>3.4. Relative Selectors

Certain contexts may accept *<a id="relative-selector"></a>relative selectors*, which are a shorthand for selectors that represent elements relative to one or more *<a id="relative-selector-anchor-elements"></a>relative selector anchor elements*. Relative selectors begin with a [combinator](#selector-combinator), with a selector representing the [anchor element](#relative-selector-anchor-elements) implied at the start of the selector. (If no combinator is present, the [descendant combinator](#descendant-combinator) is implied.)

Relative selectors are represented by [<relative-selector>](#typedef-relative-selector) in the selectors [grammar](#grammar), and lists of them by [<relative-selector-list>](#typedef-relative-selector-list).

### <a id="pseudo-classes"></a>3.5. Pseudo-classes

*<a id="pseudo-class"></a>Pseudo-classes* are [simple selectors](#simple) that permit selection based on information that lies outside of the document tree or that can be awkward or impossible to express using the other simple selectors. They can also be dynamic, in the sense that an element can acquire or lose a pseudo-class while a user interacts with the document, without the document itself changing. [Pseudo-classes](#pseudo-class) do not appear in or modify the document source or document tree.

The syntax of a [pseudo-class](#pseudo-class) consists of a ":" (U+003A COLON) followed by the name of the pseudo-class as a CSS [identifier](css-values-4.md#css-css-identifier), and, in the case of a *<a id="functional-pseudo-class"></a>functional pseudo-class*, a pair of parentheses containing its arguments.

<a id="example-86ae2021"></a> For example, [:valid](#valid-pseudo) is a regular pseudo-class, and [:lang()](#lang-pseudo) is a [functional pseudo-class](#functional-pseudo-class).

Like all CSS keywords, [pseudo-class](#pseudo-class) names are ASCII case-insensitive. No [white space](#whitespace) is allowed between the colon and the name of the pseudo-class, nor, as usual for CSS syntax, between a [functional pseudo-class](#functional-pseudo-class)’s name and its opening parenthesis (which thus form a CSS function token). Also as usual, white space is allowed around the arguments inside the parentheses of a functional pseudo-class unless otherwise specified.

Like other [simple selectors](#simple), [pseudo-classes](#pseudo-class) are allowed in all [compound selectors](#compound) contained in a selector, and must follow the [type selector](#type-selector) or [universal selector](#universal-selector), if present.

Note: Some [pseudo-classes](#pseudo-class) are mutually exclusive (such that a [compound selector](#compound) containing them, while valid, will never match anything), while others can apply simultaneously to the same element.

### <a id="pseudo-elements"></a>3.6. Pseudo-elements

Similar to how certain [pseudo-classes](#pseudo-class) represent additional state information not directly present in the document tree, a *<a id="pseudo-element"></a>pseudo-element* represents an *element* not directly present in the document tree. They are used to create abstractions about the document tree beyond those provided by the document tree. For example, pseudo-elements can be used to select portions of the document that do not correspond to a document-language element (including such ranges as don’t align to element boundaries or fit within its tree structure); that represent content not in the document tree or in an alternate projection of the document tree; or that rely on information provided by styling, layout, user interaction, and other processes that are not reflected in the document tree.

<a id="example-6916597f"></a> For instance, document languages do not offer mechanisms to access the first letter or first line of an element’s content, but there exist [pseudo-elements](#pseudo-element) (::first-letter and ::first-line) that allow those things to be styled. Notice especially that in the case of ::first-line, which portion of content is represented by the pseudo-element depends on layout information that cannot be inferred from the document tree.

[Pseudo-elements](#pseudo-element) can also represent content that doesn’t exist in the source document at all, such as the ::before and ::after pseudo-elements which allow additional content to be inserted before or after the contents of any element.

Like [pseudo-classes](#pseudo-class), [pseudo-elements](#pseudo-element) do not appear in or modify the document source or document tree. Accordingly, they also do not affect the interpretation of [structural pseudo-classes](#structural-pseudo-classes) or other selectors pertaining to their [originating element](#originating-element) or its tree.

The host language defines which pseudo-elements exist, their type, and their abilities. Pseudo-elements that exist in CSS are defined in [[CSS21]](#biblio-css21) (Level 2), [[SELECT]](#biblio-select) (Level 3), and [[CSS-PSEUDO-4]](#biblio-css-pseudo-4) (Level 4).

#### <a id="pseudo-element-syntax"></a>3.6.1. Syntax

The syntax of a [pseudo-element](#pseudo-element) is "::" (two U+003A COLON characters) followed by the name of the pseudo-element as an [identifier](css-values-4.md#css-css-identifier), and, in the case of a *<a id="functional-pseudo-element"></a>functional pseudo-element*, a pair of parentheses containing its arguments. Pseudo-element names are ASCII case-insensitive. No [white space](#whitespace) is allowed between the two colons, or between the colons and the name.

Because CSS Level 1 and CSS Level 2 conflated pseudo-elements and pseudo-classes by sharing a single-colon syntax for both, user agents must also accept the previous one-colon notation for the Level 1 & 2 pseudo-elements (::before, ::after, ::first-line, and ::first-letter). This compatibility notation is not allowed for any other [pseudo-elements](#pseudo-element). However, as this syntax is deprecated, authors should use the Level 3+ double-colon syntax for these pseudo-elements.

[Pseudo-elements](#pseudo-element) are [featureless](#featureless), and so can’t be matched by any other selector.

#### <a id="pseudo-element-attachment"></a>3.6.2. Binding to the Document Tree

[Pseudo-elements](#pseudo-element) do not exist independently in the tree: they are always bound to another element on the page, called their *<a id="originating-element"></a>originating element*. Syntactically, a pseudo-element immediately follows the [compound selector](#compound) representing its [originating element](#originating-element). If this compound selector is omitted, it is assumed to be the [universal selector](#universal-selector) *.

<a id="example-e6d9ca44"></a> For example, in the selector div a::before, the `a` elements matched by the selector are the [originating elements](#originating-element) for the ::before pseudo-elements attached to them.

The selector ::first-line is equivalent to *::first-line, which selects the ::first-line pseudo-element on *every* element in the document.

When a [pseudo-element](#pseudo-element) is encountered in a selector, the part of the selector before the pseudo-element selects the [originating element](#originating-element) for the pseudo-element; the part of the selector after it, if any, applies to the pseudo-element itself. (See below.)

#### <a id="pseudo-element-states"></a>3.6.3. Pseudo-classing Pseudo-elements

Certain [pseudo-elements](#pseudo-element) may be immediately followed by any combination of certain [pseudo-classes](#pseudo-class), in which case the pseudo-element is represented only when it is in the corresponding state. This specification allows any pseudo-element to be followed by any combination of the [logical combination pseudo-classes](#logical-combination-pseudo-classes) and the [user action pseudo-classes](#user-action-pseudo-class). Other specifications may allow additional pseudo-classes to be attached to particular pseudo-elements. Combinations that are not explicitly allowed are [invalid selectors](#invalid-selector).

Note: The [logical combination pseudo-classes](#logical-combination-pseudo-classes) pass any restrictions on validity of selectors at their position to their arguments.

<a id="example-61c8ad8f"></a> For example, since the [:hover](#hover-pseudo) pseudo-class specifies that it can apply to any pseudo-element, ::first-line:hover will match when the first line is hovered. However, since neither [:focus](#focus-pseudo) nor ::first-line define that :focus can apply to ::first-line, the selector ::first-line:focus will never match anything.

Notice that ::first-line:hover is very different from :hover::first-line, which matches the first line of any originating element that is hovered! For example, :hover::first-line also matches the first line of a paragraph when the second line of the paragraph is hovered, whereas ::first-line:hover only matches if the first line itself is hovered.

#### <a id="sub-pseudo-elements"></a>3.6.4. Sub-pseudo-elements

Some [pseudo-elements](#pseudo-element) are able to be the [originating element](#originating-element) of other pseudo-elements, which are defined as the *<a id="sub-pseudo-element"></a>sub-pseudo-elements* of this *<a id="originating-pseudo-element"></a>originating pseudo-element*. For example, when ::before is given a list-item display type, it becomes the [originating pseudo-element](#originating-pseudo-element) of its ::before::marker [sub-pseudo-element](#sub-pseudo-element).

Where disambiguation is needed, the term *<a id="ultimate-originating-element"></a>ultimate originating element* refers to the real (non-pseudo) element from which a [pseudo-element](#pseudo-element) originates.

Unless the corresponding [sub-pseudo-element](#sub-pseudo-element) is explicitly defined to exist in another specification, pseudo-element selectors are not valid when compounded to another pseudo-element selector. So, for example, ::before::before is an invalid selector, but ::before::marker is valid (in implementations that support the ::before::marker sub-pseudo-element).

#### <a id="pseudo-element-structure"></a>3.6.5. Internal Structure

Some [pseudo-elements](#pseudo-element) are defined to have internal structure. These pseudo-elements may be followed by child/descendant combinators to express those relationships. Selectors containing [combinators](#selector-combinator) after the pseudo-element are otherwise invalid.

<a id="example-3594b9ac"></a> For example, ::first-letter + span and ::first-letter em are invalid selectors. However, if a new ::shadow pseudo-element were defined to have internal structure, ::shadow > p would be a valid selector.

Note: A future specification may expand the capabilities of existing pseudo-elements, so some of these currently-invalid selectors (e.g. ::first-line :any-link) may become valid in the future.

The children of such [pseudo-elements](#pseudo-element) can simultaneously be children of other elements, too. However, at least in CSS, their rendering must be defined so as to maintain the tree-ness of the box tree.

### <a id="case-sensitive"></a>3.7. Characters and case sensitivity

All syntax defined by the Selectors spec is ASCII case-insensitive (i.e. [a-z] and [A-Z] are equivalent), as usual for CSS syntax—​this includes pseudo-class and pseudo-element names, and keywords used in various places, such as the [s](#valdef-attribute-selector-s) or [i](#valdef-attribute-selector-i) keyword in attribute selectors.

When matching Selectors to names and values defined by the document, by default this is done using the identical to operation (aka "case sensitive"), unless otherwise defined by the host language or another relevant specification. This includes element names, class names, IDs, attribute names, and attribute values.

<a id="example-97fadc2b"></a> For example, HTML has some mildly complex rules about matching certain names on HTML elements, which are *nearly* equivalent to being case-insensitive. See HTML § 4.16.2 Case-sensitivity of selectors for details—​this affects element names, attribute names, and *some* attribute values.

(The HTML parser automatically ASCII-lowercases a number of these values, so the rules it defines are *usually* equivalent to being ASCII case-insensitive), but it is possible to produce attributes via scripting that retain their non-lowercase casing, and as a result cannot be matched by Selectors due to these rules.)

Case sensitivity of namespace prefixes is defined in [[CSS3NAMESPACE]](#biblio-css3namespace). Case sensitivity of [language ranges](#language-range) is defined in the [:lang()](#lang-pseudo) section.

*<a id="whitespace"></a>White space* in Selectors consists of the code points SPACE (U+0020), TAB (U+0009), LINE FEED (U+000A), CARRIAGE RETURN (U+000D), and FORM FEED (U+000C). Other space-like code points, such as EM SPACE (U+2003) and IDEOGRAPHIC SPACE (U+3000), are never considered syntactic white space.

Code points in Selectors can be escaped with a backslash according to the same escaping rules as CSS. [[CSS21]](#biblio-css21) Note that escaping a code point “cancels out” any special meaning it may have in Selectors. For example, the selector #foo>a contains a combinator, but #foo\>a instead selects an element with the id `foo>a`.

### <a id="namespaces"></a>3.8. Declaring Namespace Prefixes

Certain selectors support namespace prefixes. The mechanism by which namespace prefixes are *<a id="nsdecl"></a>declared* should be specified by the language that uses Selectors. If the language does not specify a namespace prefix declaration mechanism, then no prefixes are declared. In CSS, namespace prefixes are declared with the @namespace rule. [[CSS3NAMESPACE]](#biblio-css3namespace)

### <a id="invalid"></a>3.9. Invalid Selectors and Error Handling

User agents must observe the rules for handling *<a id="invalid-selector"></a>invalid selectors*:

- a parsing error in a selector, e.g. an unrecognized token or a token which is not allowed at the current parsing point (see overall [§ 16 Grammar](#grammar) and per-selector syntax definitions), causes that selector to be invalid.
- a simple selector containing an [undeclared namespace prefix](#namespaces) is invalid
- a selector containing an invalid simple selector, an invalid combinator or an invalid token is invalid.
- a selector list containing an invalid selector is invalid.
- an empty selector, i.e. one that contains no [compound selector](#compound), is invalid.

Note: Consistent with CSS’s forwards-compatible parsing principle, UAs *must* treat as [invalid](#invalid-selector) any pseudo-classes, pseudo-elements, combinators, or other syntactic constructs for which they have no usable level of support. See [Partial implementations](#w3c-partial).

An [invalid selector](#invalid-selector) represents, and therefore matches, nothing.

### <a id="legacy-aliasing"></a>3.10. Legacy Aliases

Some selectors have a *<a id="legacy-selector-alias"></a>legacy selector alias*. This is a name which, at parse time, is converted to the standard name (and thus does not appear anywhere in any object model representing the selector).

## <a id="logical-combination"></a>4. Logical Combinations

Selector logic can be manipulated by [compounding](#compound) (logical AND), [selector lists](#selector-list) (logical OR), and the *<a id="logical-combination-pseudo-classes"></a>logical combination pseudo-classes* [:is()](#matches-pseudo), [:where()](#where-pseudo), and [:not()](#negation-pseudo). The [logical combination pseudo-classes](#logical-combination-pseudo-classes) are allowed anywhere that any other [pseudo-classes](#pseudo-class) are allowed, but pass any restrictions to their arguments. (For example, if only compound selectors are allowed, then only compound selectors are valid within an :is().)

Note: Since inside [:is()](#matches-pseudo) and [:where()](#where-pseudo) invalid arguments are dropped without invaliding the [pseudo-class](#pseudo-class) itself, selector arguments that are invalidated by contextual restrictions likewise do not invalidate the :is() pseudo-class itself.

### <a id="grouping"></a>4.1. Selector Lists

A comma-separated list of selectors represents the union of all elements selected by each of the individual selectors in the [selector list](#selector-list). (A comma is U+002C.) For example, in CSS when several selectors share the same declarations, they may be grouped into a comma-separated list. White space may appear before and/or after the comma.

<a id="example-1930da7e"></a> CSS example: In this example, we condense three rules with identical declarations into one. Thus,

```
h1 { font-family: sans-serif }
h2 { font-family: sans-serif }
h3 { font-family: sans-serif }
```

is equivalent to:

```
h1, h2, h3 { font-family: sans-serif }
```

**Warning**: the equivalence is true in this example because all the selectors are valid selectors. If just one of these selectors were invalid, the entire [selector list](#selector-list) would be invalid. This would invalidate the rule for all three heading elements, whereas in the former case only one of the three individual heading rules would be invalidated.

<a id="example-fd0daa05"></a> Invalid CSS example:

```
h1 { font-family: sans-serif }
h2..foo { font-family: sans-serif }
h3 { font-family: sans-serif }
```

is not equivalent to:

```
h1, h2..foo, h3 { font-family: sans-serif } 
```

because the above selector (h1, h2..foo, h3) is entirely invalid and the entire style rule is dropped. (When the selectors are not grouped, only the rule for h2..foo is dropped.)

### <a id="matches"></a>4.2. The Matches-Any Pseudo-class: [:is()](#matches-pseudo)

The matches-any pseudo-class, *<a id="matches-pseudo"></a>:is()*, is a functional pseudo-class taking a [<forgiving-selector-list>](#typedef-forgiving-selector-list) as its sole argument.

If the argument, after parsing, is an empty list, the pseudo-class is valid but matches nothing. Otherwise, the pseudo-class matches any element that matches any of the selectors in the list.

Note: The specificity of the [:is()](#matches-pseudo) pseudo-class is replaced by the specificity of its most specific argument. Thus, a selector written with :is() does not necessarily have equivalent specificity to the equivalent selector written without :is() For example, if we have :is(ul, ol, .list) > [hidden] and ul > [hidden], ol > [hidden], .list > [hidden] a [hidden] child of an `ol` matches the first selector with a specificity of (0,2,0) whereas it matches the second selector with a specificity of (0,1,1). See [§ 15 Calculating a selector’s specificity](#specificity-rules).

Pseudo-elements cannot be represented by the matches-any pseudo-class; they are not valid within [:is()](#matches-pseudo).

Default namespace declarations do not affect the [compound selector](#compound) representing the [subject](#selector-subject) of any selector within a [:is()](#matches-pseudo) pseudo-class, unless that compound selector contains an explicit [universal selector](#universal-selector) or [type selector](#type-selector).

<a id="example-2cab03ed"></a> For example, the following selector matches any element that is being hovered or focused, regardless of its namespace. In particular, it is not limited to only matching elements in the default namespace that are being hovered or focused.

```
*|*:is(:hover, :focus) 
```

The following selector, however, represents only hovered or focused elements that are in the default namespace, because it uses an explicit universal selector within the [:is()](#matches-pseudo) notation:

```
*|*:is(*:hover, *:focus) 
```

As previous drafts of this specification used the name *<a id="selectordef-matches"></a>:matches()* for this pseudo-class, UAs may additionally implement this obsolete name as a [legacy selector alias](#legacy-selector-alias) for [:is()](#matches-pseudo) if needed for backwards-compatibility.

### <a id="negation"></a>4.3. The Negation (Matches-None) Pseudo-class: [:not()](#negation-pseudo)

The negation pseudo-class, *<a id="negation-pseudo"></a>:not()*, is a functional pseudo-class taking a [<complex-real-selector-list>](#typedef-complex-real-selector-list) as an argument. It represents an element that is not represented by its argument.

Note: In Selectors Level 3, only a single [simple selector](#simple) was allowed as the argument to [:not()](#negation-pseudo).

Note: The specificity of the [:not()](#negation-pseudo) pseudo-class is replaced by the specificity of the most specific selector in its argument; thus it has the exact behavior of :not(:is(*argument*)). See [§ 15 Calculating a selector’s specificity](#specificity-rules).

Pseudo-elements cannot be represented by the negation pseudo-class; they are not valid within [:not()](#negation-pseudo).

<a id="example-2a176618"></a> For example, the following selector matches all button elements in an HTML document that are not disabled.

```
button:not([DISABLED]) 
```

The following selector represents all but FOO elements.

```
*:not(FOO)
```

The following compound selector represents all HTML elements except links.

```
html|*:not(:link):not(:visited)
```

As with [:is()](#matches-pseudo), default namespace declarations do not affect the [compound selector](#compound) representing the [subject](#selector-subject) of any selector within a [:not()](#negation-pseudo) pseudo-class, unless that compound selector contains an explicit [universal selector](#universal-selector) or [type selector](#type-selector). (See :is() for examples.)

Note: The [:not()](#negation-pseudo) pseudo-class allows useless selectors to be written. For instance :not(*|*), which represents no element at all, or div:not(span), which is equivalent to div but with a higher specificity.

### <a id="zero-matches"></a>4.4. The Specificity-adjustment Pseudo-class: [:where()](#where-pseudo)

The Specificity-adjustment pseudo-class, *<a id="where-pseudo"></a>:where()*, is a functional pseudo-class with the same syntax and functionality as [:is()](#matches-pseudo). Unlike :is(), neither the [:where()](#where-pseudo) pseudo-class, nor any of its arguments, contribute to the [specificity](#specificity) of the selector—​its specificity is always zero.

This is useful for introducing filters in a selector while keeping the associated style declarations easy to override.

<a id="example-7ca50acf"></a> Below is a common example where the specificity heuristic fails to match author expectations:

```
a:not(:hover) {
  text-decoration: none;
}

nav a {
  /* Has no effect */
  text-decoration: underline;
}
```

However, by using [:where()](#where-pseudo) the author can explicitly declare their intent:

```
a:where(:not(:hover)) {
  text-decoration: none;
}

nav a {
  /* Works now! */
  text-decoration: underline;
}
```

Note: Future levels of Selectors may introduce an additional argument to explicitly set the specificity of that instance of the pseudo-class.

### <a id="relational"></a>4.5. The Relational Pseudo-class: [:has()](#has-pseudo)

The relational pseudo-class, *<a id="has-pseudo"></a>:has()*, is a functional pseudo-class taking a [<relative-selector-list>](#typedef-relative-selector-list) as an argument. It represents an element if any of the [relative selectors](#relative-selector) would match at least one element when [anchored against](#relative-selector-anchor-elements) this element.

The [:has()](#has-pseudo) pseudo-class cannot be nested; :has() is not valid within :has(). Also, unless explicitly defined as a *<a id="has-allowed-pseudo-element"></a>:has-allowed pseudo-element*, [pseudo-elements](#pseudo-element) are not valid selectors within :has(). (This specification does not define any [:has-allowed pseudo-elements](#has-allowed-pseudo-element), but other specifications may do so.)

Note: Pseudo-elements are generally excluded from [:has()](#has-pseudo) because many of them exist conditionally, based on the styling of their ancestors, so allowing these to be queried by :has() would introduce cycles.

Note: Since [:has()](#has-pseudo) takes a [<relative-selector-list>](#typedef-relative-selector-list), its arguments are *inherently* [complex selectors](#complex) (because they start, perhaps implicitly, with a combinator). This means :has() cannot be used in contexts that don’t allow complex selectors; its arguments will be guaranteed to be invalid.

<a id="example-61d52da1"></a> For example, the following selector matches only `<a>` elements that contain an `<img>` child:

```
a:has(> img)
```

The following selector matches a `<dt>` element immediately followed by another `<dt>` element:

```
dt:has(+ dt)
```

The following selector matches `<section>` elements that don’t contain any heading elements:

```
section:not(:has(h1, h2, h3, h4, h5, h6))
```

Note that ordering matters in the above selector. Swapping the nesting of the two pseudo-classes, like:

```
section:has(:not(h1, h2, h3, h4, h5, h6))
```

...would result in matching any `<section>` element which contains anything that’s not a heading element.

## <a id="elemental-selectors"></a>5. Elemental selectors

### <a id="type-selectors"></a>5.1. Type (tag name) selector

A *<a id="type-selector"></a>type selector* is the name of a document language element type, and represents an instance of that element type in the document tree.

<a id="example-087f4a77"></a> For example, the selector h1 represents an h1 element in the document.

A [type selector](#type-selector) is written as a CSS qualified name: an [identifier](css-values-4.md#css-css-identifier) with an optional namespace prefix. [[CSS3NAMESPACE]](#biblio-css3namespace) (See [§ 5.3 Namespaces in Elemental Selectors](#type-nmsp).)

### <a id="the-universal-selector"></a>5.2. Universal selector

The *<a id="universal-selector"></a>universal selector* is a special [type selector](#type-selector), that represents an element of any element type.

It is written as a CSS qualified name with an asterisk (`*` U+002A) as the local name. Like a [type selector](#type-selector), the [universal selector](#universal-selector) can be qualified by a namespace, restricting it to only elements belonging to that namespace, and is affected by a default namespace as defined in [§ 5.3 Namespaces in Elemental Selectors](#type-nmsp).

Unless an element is [featureless](#featureless), the presence of a [universal selector](#universal-selector) has no effect on whether the element matches the selector. (Featureless elements do not match any selector, including the universal selector.)

<a id="example-0f5d51ca"></a>

- *[hreflang|=en] and [hreflang|=en] are equivalent,
- *.warning and .warning are equivalent,
- *#myid and #myid are equivalent.

The [universal selector](#universal-selector) follows the same syntax rules as other [type selectors](#type-selector): only one can appear per [compound selector](#compound), and it must be the first [simple selector](#simple) in the compound selector.

Note: In some cases, adding a [universal selector](#universal-selector) can make a selector easier to read, even though it has no effect on the matching behavior. For example, div :first-child and div:first-child are somewhat difficult to tell apart at a quick glance, but writing the former as div *:first-child makes the difference obvious.

### <a id="type-nmsp"></a>5.3. Namespaces in Elemental Selectors

[Type selectors](#type-selector) and [universal selectors](#universal-selector) allow an optional namespace component: a namespace prefix that has been previously [declared](#nsdecl) may be prepended to the element name separated by the namespace separator “vertical bar” (`|` U+007C). (See, e.g., [[XML-NAMES]](#biblio-xml-names) for the use of namespaces in XML.) It has the following meaning in each form:

**`ns|E` **
: elements with name E in namespace ns
**`*|E` **
: elements with name E in any namespace, including those without a namespace
**`|E` **
: elements with name E without a namespace
**`E` **
: if no default namespace has been [declared](#nsdecl) for selectors, this is equivalent to *|E. Otherwise it is equivalent to ns|E where ns is the default namespace.
<a id="example-53e1ab95"></a> CSS examples:

```
@namespace foo url(http://www.example.com);
foo|h1 { color: blue }  /* first rule */
foo|* { color: yellow } /* second rule */
|h1 { color: red }      /* ...*/
*|h1 { color: green }
h1 { color: green }
```

The first rule (not counting the @namespace at-rule) will match only h1 elements in the "http://www.example.com" namespace.

The second rule will match all elements in the "http://www.example.com" namespace.

The third rule will match only h1 elements with no namespace.

The fourth rule will match h1 elements in any namespace (including those without any namespace).

The last rule is equivalent to the fourth rule because no default namespace has been defined.

If a default namespace is declared, [compound selectors](#compound) without [type selectors](#type-selector) in them still only match elements in that default namespace.

<a id="example-03281ceb"></a> For example, in the following style sheet:

```
@namespace url("http://example.com/foo");

.special { ... }
```

The .special selector only matches elements in the "http://example.com/foo" namespace, even though no reference to the type name (which is paired with the namespace in the DOM) appeared.

A [type selector](#type-selector) or [universal selector](#universal-selector) containing a namespace prefix that has not been previously [declared](#nsdecl) is an [invalid selector](#invalid-selector).

### <a id="the-defined-pseudo"></a>5.4. The Defined Pseudo-class: [:defined](#defined-pseudo)

In some host languages, elements can have a distinction between being “defined”/“constructed” or not. The *<a id="defined-pseudo"></a>:defined* [pseudo-class](#pseudo-class) matches elements that are fully defined, as dictated by the host language.

If the host language does not have this sort of distinction, all elements in it match [:defined](#defined-pseudo).

<a id="example-cd9dcdb2"></a> In HTML, all built-in elements are always considered to be defined, so the following example will always match:

```
p:defined { ... }
```

Custom elements, on the other hand, start out *un*defined, and only become defined when properly registered. This means the [:defined](#defined-pseudo) pseudo-class can be used to hide a custom element until it has been registered:

```
custom-element { visibility: hidden }
```

```
custom-element:defined { visibility: visible }
```

## <a id="attribute-selectors"></a>6. Attribute selectors

Selectors allow the representation of an element’s attributes. When a selector is used as an expression to match against an element, an *<a id="attribute-selector"></a>attribute selector* must be considered to match an element if that element has an attribute that matches the attribute represented by the attribute selector.

<a id="issue-745ef775"></a>Add comma-separated syntax for multiple-value matching? e.g. [rel ~= next, prev, up, first, last]

### <a id="attribute-representation"></a>6.1. Attribute presence and value selectors

CSS2 introduced four attribute selectors:

**[att] **
: Represents an element with the `att` attribute, whatever the value of the attribute.
**[att=val] **
: Represents an element with the `att` attribute whose value is exactly "val".
**[att~=val] **
: Represents an element with the `att` attribute whose value is a [whitespace](#whitespace)-separated list of words, one of which is exactly "val". If "val" contains whitespace, it will never represent anything (since the words are *separated* by spaces). Also if "val" is the empty string, it will never represent anything.
**[att|=val] **
: Represents an element with the `att` attribute, its value either being exactly "val" or beginning with "val" immediately followed by "-" (U+002D). This is primarily intended to allow language subcode matches (e.g., the `hreflang` attribute on the a element in HTML) as described in BCP 47 ([[BCP47]](#biblio-bcp47)). For `lang` (or `xml:lang`) language subcode matching, please see the [:lang()](#lang-pseudo) pseudo-class.
Attribute values must be [<ident-token>](css-syntax-3.md#typedef-ident-token)s or [<string-token>](css-syntax-3.md#typedef-string-token)s. [[CSS3SYN]](#biblio-css3syn)

<a id="example-52ee808b"></a> Examples:

The following attribute selector represents an h1 element that carries the `title` attribute, whatever its value:

```
h1[title]
```

In the following example, the selector represents a span element whose `class` attribute has exactly the value "example":

```
span[class="example"]
```

Multiple attribute selectors can be used to represent several attributes of an element, or several conditions on the same attribute. Here, the selector represents a span element whose `hello` attribute has exactly the value "Cleveland" and whose `goodbye` attribute has exactly the value "Columbus":

```
span[hello="Cleveland"][goodbye="Columbus"]
```

The following CSS rules illustrate the differences between "=" and "~=". The first selector would match, for example, an a element with the value "copyright copyleft copyeditor" on a `rel` attribute. The second selector would only match an a element with an `href` attribute having the exact value "http://www.w3.org/".

```
a[rel~="copyright"] { ... }
a[href="http://www.w3.org/"] { ... }
```

The following selector represents an a element whose `hreflang` attribute is exactly "fr".

```
a[hreflang=fr] 
```

The following selector represents an a element for which the value of the `hreflang` attribute begins with "en", including "en", "en-US", and "en-scouse":

```
a[hreflang|="en"] 
```

The following selectors represent a DIALOGUE element whenever it has one of two different values for an attribute `character`:

```
DIALOGUE[character=romeo]
DIALOGUE[character=juliet]
```

### <a id="attribute-substrings"></a>6.2. Substring matching attribute selectors

Three additional attribute selectors are provided for matching substrings in the value of an attribute:

**[att^=val] **
: Represents an element with the `att` attribute whose value begins with the prefix "val". If "val" is the empty string then the selector does not represent anything.
**[att$=val] **
: Represents an element with the `att` attribute whose value ends with the suffix "val". If "val" is the empty string then the selector does not represent anything.
**[att*=val] **
: Represents an element with the `att` attribute whose value contains at least one instance of the substring "val". If "val" is the empty string then the selector does not represent anything.
Attribute values must be [<ident-token>](css-syntax-3.md#typedef-ident-token)s or [<string-token>](css-syntax-3.md#typedef-string-token)s.

<a id="example-d887e2c3"></a> Examples: The following selector represents an HTML object element, referencing an image:

```
object[type^="image/"] 
```

The following selector represents an HTML a element with an `href` attribute whose value ends with ".html".

```
a[href$=".html"] 
```

The following selector represents an HTML paragraph with a `title` attribute whose value contains the substring "hello"

```
p[title*="hello"] 
```

### <a id="attribute-case"></a>6.3. Case-sensitivity

By default case-sensitivity of attribute names and values in selectors depends on the document language.

To match attribute values ASCII case-insensitively regardless of document language rules, the attribute selector may include the identifier *<a id="valdef-attribute-selector-i"></a>i* before the closing bracket (`]`). When this flag is present, UAs must match the attribute’s value ASCII case-insensitively (i.e. [a-z] and [A-Z] are considered equivalent).

<a id="example-eb510622"></a> For example, ASCII case insensitivity allows `green` to match `GREEN`. However, `grün` would not match `GRÜN`.

Alternately, the attribute selector may include the identifier *<a id="valdef-attribute-selector-s"></a>s* before the closing bracket (`]`); in this case the UA must match the value case-sensitively, with “identical to” semantics [[INFRA]](#biblio-infra), regardless of document language rules.

Like the rest of Selectors syntax, the `i` and `s` identifiers themselves are ASCII case-insensitive.

<a id="example-348bad1b"></a> The following rule will style the `frame` attribute when it has a value of `hsides`, whether that value is represented as `hsides`, `HSIDES`, `hSides`, etc. even in an XML environment where attribute values are case-sensitive.

```
[frame=hsides i] { border-style: solid none; } 
```

<a id="example-f596236d"></a> The following rule will style lists with `type="a"` attributes differently than `type="A"` even though HTML defines the `type` attribute to be case-insensitive.

```
[type="a" s] { list-style: lower-alpha; }
[type="A" s] { list-style: upper-alpha; }
```

Note: Some document models normalize case-insensitive attribute values at parse time such that checking if a string is case-sensitive matching is impossible. Case-sensitive matching via `s` flags is only possible in systems that preserve the original case.

### <a id="attrnmsp"></a>6.4. Attribute selectors and namespaces

The attribute name in an attribute selector is given as a CSS qualified name: a namespace prefix that has been previously [declared](#nsdecl) may be prepended to the attribute name separated by the namespace separator "vertical bar" (`|`). In keeping with the Namespaces in the XML recommendation, default namespaces do not apply to attributes, therefore attribute selectors without a namespace component apply only to attributes that have no namespace (equivalent to |attr). An asterisk may be used for the namespace prefix indicating that the selector is to match all attribute names without regard to the attribute’s namespace.

An attribute selector with an attribute name containing a namespace prefix that has not been previously [declared](#nsdecl) is an invalid selector.

<a id="example-a7ca2cb1"></a> CSS examples:

```
@namespace foo "http://www.example.com";
[foo|att=val] { color: blue }
[*|att] { color: yellow }
[|att] { color: green }
[att] { color: green }
```

The first rule will match only elements with the attribute `att` in the "http://www.example.com" namespace with the value "val".

The second rule will match only elements with the attribute `att` regardless of the namespace of the attribute (including no namespace).

The last two rules are equivalent and will match only elements with the attribute `att` where the attribute is not in a namespace.

### <a id="def-values"></a>6.5. Default attribute values in DTDs

Attribute selectors represent attribute values in the document tree. How that document tree is constructed is outside the scope of Selectors. In some document formats default attribute values can be defined in a DTD or elsewhere, but these can only be selected by attribute selectors if they appear in the document tree. Selectors should be designed so that they work whether or not the default values are included in the document tree.

For example, a XML UA may, but is *not* required to, read an “external subset” of the DTD, but *is* required to look for default attribute values in the document’s “internal subset”. (See, e.g., [[XML10]](#biblio-xml10) for definitions of these subsets.) Depending on the UA, a default attribute value defined in the external subset of the DTD might or might not appear in the document tree.

A UA that recognizes an XML namespace may, but is not required to use its knowledge of that namespace to treat default attribute values as if they were present in the document. (For example, an XHTML UA is not required to use its built-in knowledge of the XHTML DTD. See, e.g., [[XML-NAMES]](#biblio-xml-names) for details on namespaces in XML 1.0.)

Note: Typically, implementations choose to ignore external subsets. This corresponds to the behavior of non-validating processors as defined by the XML specification.

<a id="example-ba3e8ad8"></a> Example:

Consider an element `EXAMPLE` with an attribute `radix` that has a default value of `"decimal"`. The DTD fragment might be

```
<!ATTLIST EXAMPLE radix (decimal,octal) "decimal"> 
```

If the style sheet contains the rules

```
EXAMPLE[radix=decimal] { /*... default property settings ...*/ }
EXAMPLE[radix=octal]   { /*... other settings...*/ }
```

the first rule might not match elements whose `radix` attribute is set by default, i.e. not set explicitly. To catch all cases, the attribute selector for the default value must be dropped:

```
EXAMPLE                { /*... default property settings ...*/ }
EXAMPLE[radix=octal]   { /*... other settings...*/ }
```

Here, because the selector EXAMPLE[radix=octal] is more specific than the type selector alone, the style declarations in the second rule will override those in the first for elements that have a `radix` attribute value of `"octal"`. Care has to be taken that all property declarations that are to apply only to the default case are overridden in the non-default cases' style rules.

### <a id="class-html"></a>6.6. Class selectors

The *<a id="class-selector"></a>class selector* is given as a full stop (. U+002E) immediately followed by an identifier. It represents an element belonging to the class identified by the identifier, as defined by the document language. For example, in [[HTML]](#biblio-html), [[SVG11]](#biblio-svg11), and [[MATHML]](#biblio-mathml) membership in a class is given by the `class` attribute: in these languages it is equivalent to the `~=` notation applied to the local `class` attribute (i.e. `[class~=*identifier*]`).

<a id="example-f9c08b5b"></a> CSS examples:

We can assign style information to all elements with `class~="pastoral"` as follows:

```
*.pastoral { color: green }  /* all elements with class~=pastoral */ 
```

or just

```
.pastoral { color: green }  /* all elements with class~=pastoral */ 
```

The following assigns style only to H1 elements with `class~="pastoral"`:

```
H1.pastoral { color: green }  /* H1 elements with class~=pastoral */ 
```

Given these rules, the first `H1` instance below would not have green text, while the second would:

```
<H1>Not green</H1>
<H1 class="pastoral">Very green</H1>
```

The following rule matches any P element whose `class` attribute has been assigned a list of [whitespace](#whitespace)-separated values that includes both `pastoral` and `marine`:

```
p.pastoral.marine { color: green } 
```

This rule matches when `class="pastoral blue aqua marine"` but does not match for `class="pastoral blue"`.

Note: Because CSS gives considerable power to the "class" attribute, authors could conceivably design their own "document language" based on elements with almost no associated presentation (such as div and span in HTML) and assigning style information through the "class" attribute. Authors should avoid this practice since the structural elements of a document language often have recognized and accepted meanings and author-defined classes may not.

Note: If an element has multiple class attributes, their values must be concatenated with spaces between the values before searching for the class. As of this time the working group is not aware of any manner in which this situation can be reached, however, so this behavior is explicitly non-normative in this specification.

When matching against a document which is in quirks mode, class names must be matched ASCII case-insensitively; class selectors are otherwise case-sensitive, only matching class names they are identical to. [[INFRA]](#biblio-infra)

### <a id="id-selectors"></a>6.7. ID selectors

Document languages may contain attributes that are declared to be of type ID. What makes attributes of type ID special is that no two such attributes can have the same value in a conformant document, regardless of the type of the elements that carry them; whatever the document language, an ID typed attribute can be used to uniquely identify its element. In HTML all ID attributes are named `id`; XML applications may name ID attributes differently, but the same restriction applies. Which attribute on an element is considered the “ID attribute” is defined by the document language.

An *<a id="id-selector"></a>ID selector* consists of a “number sign” (U+0023, `#`) immediately followed by the ID value, which must be a CSS identifier. An ID selector represents an element instance that has an identifier that matches the identifier in the ID selector. (It is possible in non-conforming documents for multiple elements to match a single ID selector.)

<a id="example-3e8dc597"></a> Examples: The following ID selector represents an h1 element whose ID-typed attribute has the value "chapter1":

```
h1#chapter1 
```

The following ID selector represents any element whose ID-typed attribute has the value "chapter1":

```
#chapter1 
```

The following selector represents any element whose ID-typed attribute has the value "z98y".

```
*#z98y 
```

Note: In XML 1.0 [[XML10]](#biblio-xml10), the information about which attribute contains an element’s IDs is contained in a DTD or a schema. When parsing XML, UAs do not always read the DTD, and thus may not know what the ID of an element is (though a UA may have namespace-specific knowledge that allows it to determine which attribute is the ID attribute for that namespace). If a style sheet author knows or suspects that a UA may not know what the ID of an element is, they should use normal attribute selectors instead: [name=p371] instead of #p371.

If an element has multiple ID attributes, all of them must be treated as IDs for that element for the purposes of the ID selector. Such a situation could be reached using mixtures of xml:id, DOM3 Core, XML DTDs, and namespace-specific knowledge.

When matching against a document which is in quirks mode, IDs must be matched ASCII case-insensitively; ID selectors are otherwise case-sensitive, only matching IDs they are identical to. [[INFRA]](#biblio-infra)

## <a id="linguistic-pseudos"></a>7. Linguistic Pseudo-classes

### <a id="the-dir-pseudo"></a>7.1. The Directionality Pseudo-class: [:dir()](#dir-pseudo)

The *<a id="dir-pseudo"></a>:dir()* pseudo-class allows the author to write selectors that represent an element based on its directionality as determined by the [document language](#document-language). For example, [[HTML]](#biblio-html) defines how to determine the directionality of an element, based on a combination of the `dir` attribute, the surrounding text, and other factors. As another example, the `its:dir` and `dirRule` element of the Internationalization Tag Set [[ITS20]](#biblio-its20) are able to define the directionality of an element in [[XML10]](#biblio-xml10).

The [:dir()](#dir-pseudo) pseudo-class does not select based on stylistic states—for example, the CSS direction property does not affect whether it matches.

The pseudo-class :dir(ltr) represents an element that has a directionality of left-to-right (`ltr`). The pseudo-class :dir(rtl) represents an element that has a directionality of right-to-left (`rtl`). The argument to [:dir()](#dir-pseudo) must be a single identifier, otherwise the selector is invalid. White space is optionally allowed between the identifier and the parentheses. Values other than `ltr` and `rtl` are not invalid, but do not match anything. (If a future markup spec defines other directionalities, then Selectors may be extended to allow corresponding values.)

The difference between :dir(C) and [dir=C] is that [dir=C] only performs a comparison against a given attribute on the element, while the :dir(C) pseudo-class uses the UAs knowledge of the document’s semantics to perform the comparison. For example, in HTML, the directionality of an element inherits so that a child without a `dir` attribute will have the same directionality as its closest ancestor with a valid `dir` attribute. As another example, in HTML, an element that matches [dir=auto] will match either :dir(ltr) or :dir(rtl) depending on the resolved directionality of the elements as determined by its contents. [[HTML]](#biblio-html)

### <a id="the-lang-pseudo"></a>7.2. The Language Pseudo-class: [:lang()](#lang-pseudo)

If the document language specifies how the (human) content language of an element is determined, it is possible to write selectors that represent an element based on its content language. The *<a id="lang-pseudo"></a>:lang()* pseudo-class, which accepts a comma-separated list of one or more [language ranges](#language-range), represents an element whose content language is one of the languages listed in its argument. Each *<a id="language-range"></a>language range* in [:lang()](#lang-pseudo) is an extended language range, as defined in BCP 47, and must be a valid CSS [<ident>](css-values-4.md#typedef-ident) or [<string>](css-values-4.md#string-value). (Thus language ranges containing asterisks, for example, must be either correctly escaped or quoted as strings, e.g. :lang(\*-Latn) or :lang("*-Latn").)

Note: The content language of an element is defined by the document language.

For example, in HTML [[HTML]](#biblio-html), the content language is determined by a combination of the `lang` attribute, information from meta elements, and possibly also the protocol (e.g. from HTTP headers). XML languages can use the `xml:lang` attribute to indicate language information for an element. [[XML10]](#biblio-xml10)

The element’s content language matches a [language range](#language-range) if its content language, as represented in BCP 47 syntax, matches the given language range in an *extended filtering* operation per [[RFC4647]](#biblio-rfc4647) Matching of Language Tags (section 3.3.2). The content language and the language range must be *canonicalized* and converted to *extlang form* as per section 4.5 of [[RFC5646]](#biblio-rfc5646) prior to the *extended filtering* operation; language tags or ranges that are not valid do not match anything. The matching is performed ASCII case-insensitively.

The [language range](#language-range) must be an extended language range according to BCP47. Language ranges that are not well-formed language tags or which would not be a well-formed language tag if an initial wildcard character "*" were replaced with a valid subtag, do not match anything.

<a id="example-7409fe3d"></a> For example,

```
:lang(åå)
```

would not match, because it containins non-ASCII characters so is ill-formed.

On the other hand,

```
:lang(qq)
```

could match, even though qq is not a registered language code.

For this purpose, a wildcard [language range](#language-range) (`"*"`) does not match elements whose language is not tagged (e.g. `lang=""`), but does match elements whose language is tagged as undetermined (`lang=und`). A language range consisting of an empty string (:lang("")) matches (only) elements whose language is not tagged.

Note: It is recommended that documents and protocols indicate language using language tags from [[BCP47]](#biblio-bcp47), and in the case of XML-based formats, by means of `xml:lang` attributes. [[XML10]](#biblio-xml10) See “Choosing a Language Tag”

<a id="example-91142d84"></a> Examples: The two following selectors represent an HTML document that is in Belgian French or German. The two next selectors represent q quotations in an arbitrary element in Belgian French or German.

```
html:lang(fr-be)
html:lang(de)
:lang(fr-be) > q
:lang(de) > q
```

Note: One difference between :lang(C) and the |= operator is that the |= operator only performs a comparison against a given attribute on the element, while the :lang(C) pseudo-class uses the UAs knowledge of the document’s semantics to perform the comparison.

<a id="example-c75c4e33"></a> In this HTML example, only the BODY matches [lang|=fr] (because it has a LANG attribute) but both the BODY and the P match :lang(fr) (because both are in French). The P does not match the [lang|=fr] because it does not have a LANG attribute.

```
<body lang=fr>
  <p>Je suis français.</p>
</body>
```

<a id="example-c94037c6"></a> Another difference between :lang(C) and the |= operator is that :lang(C) performs implicit wildcard matching.

For example, :lang(de-DE) will match all of de-DE, de-DE-1996, de-Latn-DE, de-Latf-DE, de-Latn-DE-1996, whereas of those [lang|=de-DE] will only match de-DE and de-DE-1996.

To perform wildcard matching on the first subtag (the primary language), an asterisk must be used: *-CH will match all of de-CH, it-CH, fr-CH, and rm-CH.

To select against an element’s lang attribute value using this type of language range match, use both the attribute selector and language pseudo-class together, e.g. [lang]:lang(de-DE).

Note: Wildcard language matching and comma-separated lists are new in Level 4.

## <a id="location"></a>8. Location Pseudo-classes

### <a id="the-any-link-pseudo"></a>8.1. The Hyperlink Pseudo-class: [:any-link](#any-link-pseudo)

The *<a id="any-link-pseudo"></a>:any-link* pseudo-class represents an element that acts as the source anchor of a hyperlink. For example, in [[HTML]](#biblio-html), any `a` or `area` elements with an `href` attribute are hyperlinks, and thus match `:any-link`. It matches an element if the element would match either [:link](#link-pseudo) or [:visited](#visited-pseudo), and is equivalent to :is(:link, :visited).

### <a id="link"></a>8.2. The Link History Pseudo-classes: [:link](#link-pseudo) and [:visited](#visited-pseudo)

User agents commonly display unvisited [hyperlinks](#the-any-link-pseudo) differently from previously visited ones. Selectors provides the pseudo-classes *<a id="link-pseudo"></a>:link* and *<a id="visited-pseudo"></a>:visited* to distinguish them:

- The [:link](#link-pseudo) pseudo-class applies to links that have not yet been visited.
- The [:visited](#visited-pseudo) pseudo-class applies once the link has been visited by the user.

The two states are mutually exclusive.

After some amount of time, user agents may choose to return a visited link to the (unvisited) [:link](#link-pseudo) state.

The [:visited](#visited-pseudo) pseudo-class comes with obvious privacy implications—​letting random websites know what *other* websites you’ve visited can be problematic for a number of reasons—​and so user agents *must* preserve user privacy in their implementation of :visited.

This specification intentionally does not specify exactly how to preserve user privacy in this regard, to allow for user agents to innovate in this space. The following methods are suggested, however:

-
Have [:visited](#visited-pseudo) never match, so all links match [:link](#link-pseudo) instead.
-
Carefully track what history entries could have been observed by a given origin on their own, and only have links match [:visited](#visited-pseudo) if that visit would have been observable from the site’s origin. A possible specific approach for this is described in [Appendix C: Example Privacy-Preserving :visited Restrictions](#visited-privacy).
-
Allow links to match [:visited](#visited-pseudo) on any origin, but carefully restrict what styles they can apply and what information is returned by style-querying APIs like `getComputedStyle()`, to prevent sites from observing whether a link is styled with [:link](#link-pseudo) or :visited. (This is documented at MDN, and was the historical approach browsers took, but is not perfect; there are several ways for a hostile page to still extract history information.)

<a id="example-b0f10727"></a> For example, the selector .footnote:visited would allow styling footnote links differently if they’ve been previously followed, allowing users of the page to know they might not need to click the footnote again.

### <a id="the-target-pseudo"></a>8.3. The Target Pseudo-class: [:target](#target-pseudo)

In some document languages, the document’s URL can further point to specific elements *within* the document via the URL’s fragment. The elements pointed to in this way are the target elements of the document.

<a id="example-b8c5336f"></a> In HTML the fragment points to the element in the page with the same ID. The url `https://example.com/index.html#section2`, for example, points to the element with `id="section2"` in the document at `https://example.com/index.html`.

The *<a id="target-pseudo"></a>:target* pseudo-class matches the document’s target elements. If the document’s URL has no fragment identifier, then the document has no target elements.

<a id="example-8a386e7c"></a> Example:

```
p.note:target 
```

This selector represents a `p` element of class `note` that is the target element of the referring URL.

<a id="example-af403697"></a> CSS example: Here, the [:target](#target-pseudo) pseudo-class is used to make the target element red and place an image before it, if there is one:

```
:target { color : red }
:target::before { content : url(target.png) }
```

Note: This specification previously defined a :target-within pseudo-class, analogous to [:focus-within](#focus-within-pseudo). It was removed in favor of :has(:target), which should hopefully suffice to solve the same use-cases.

### <a id="the-scope-pseudo"></a>8.4. The Reference Element Pseudo-class: [:scope](#scope-pseudo)

In some contexts, selectors are matched with respect to one or more [scoping roots](#scoping-root), such as when calling the `querySelector()` method in [[DOM]](#biblio-dom). The *<a id="scope-pseudo"></a>:scope* pseudo-class represents this scoping root, and may be either a true element or a virtual one (such as a `DocumentFragment`).

If there is no [scoping root](#scoping-root) then [:scope](#scope-pseudo) represents the root of the tree the element is in (equivalent to :host in a shadow tree, or [:root](#root-pseudo) otherwise). Specifications intending for this pseudo-class to match specific elements rather than this tree root element must define their scoping root(s).

A virtual [scoping root](#scoping-root) is some object representing the root of a document fragment, and can be used in selector patterns to represent other elements’ relationships to this scoping root, acting as the parent of any root elements in the document fragment it represents. A virtual scoping root is [featureless](#featureless) and cannot be the [subject of the selector](#selector-subject).

<a id="example-ef427647"></a> For example, if you have a `DocumentFragment` `df`, then `df.querySelectorAll(":scope > .foo")` matches all the .foo elements that are "top-level" in the document fragment (those that have the document fragment as their `parentNode`).

However, `df.querySelector(":scope")` will not match anything, as the document fragment itself can’t be the [subject of the selector](#selector-subject).

## <a id="useraction-pseudos"></a>9. User Action Pseudo-classes

Interactive user interfaces sometimes change the rendering in response to user actions. Selectors provides several *<a id="user-action-pseudo-class"></a>user action pseudo-classes* for the selection of an element the user is acting on. (In non-interactive user agents, these pseudo-classes are valid, but never match any element.)

The [user action pseudo-classes](#user-action-pseudo-class) are not mutually exclusive. An element can match several such pseudo-classes at the same time.

<a id="example-b4125c5c"></a> Examples:

```
a:hover   /* user hovers over the link */
a:focus   /* user focuses the link     */

a:focus:hover
/* user hovers over the link while it's focused */
```

Some [user action pseudo-classes](#user-action-pseudo-class), in addition to matching on the particular element with the property in question, match on that element’s ancestors as well: [:hover](#hover-pseudo), [:active](#active-pseudo), [:focus-within](#focus-within-pseudo). Specifically, if these match on a given element, they also match on the element’s flat tree ancestors, up to and including the first top layer element or the root element, whichever is encountered first.

Note: The specifics of hit-testing, necessary to know when several of the pseudo-classes defined in this section apply, are not yet defined, but will be in the future.

### <a id="the-hover-pseudo"></a>9.1. The Pointer Hover Pseudo-class: [:hover](#hover-pseudo)

The *<a id="hover-pseudo"></a>:hover* pseudo-class applies while the user designates an element (or pseudo-element) with a pointing device, but does not necessarily activate it. For example, a visual user agent could apply this pseudo-class when the cursor (mouse pointer) hovers over a box generated by the element. Interactive user agents that cannot detect hovering due to hardware limitations (e.g., a pen device that does not detect hovering) are still conforming; the selector will simply never match in such a UA.

An element also matches [:hover](#hover-pseudo) if one of its descendants in the flat tree (including non-element nodes, such as text nodes) matches the above conditions.

Document languages may define additional ways in which an element can match [:hover](#hover-pseudo). For example, [[HTML]](#biblio-html) defines a labeled control element as matching `:hover` when its label is hovered.

Note: Since the [:hover](#hover-pseudo) state can apply to an element because its child is designated by a pointing device, it is possible for :hover to apply to an element that is not underneath the pointing device.

### <a id="the-active-pseudo"></a>9.2. The Activation Pseudo-class: [:active](#active-pseudo)

The *<a id="active-pseudo"></a>:active* pseudo-class applies while an element is being “activated” by the user, as defined by the host language; for example, while a hyperlink is being triggered.

In addition, the [:active](#active-pseudo) pseudo-class applies while any generated box of *any* element (or pseudo-element) is being actively indicated by a pointing device (in the “down” state), e.g. between the time the user presses the primary mouse button and releases it, or while a finger is pressing on a touchscreen.

Note: [[HTML]](#biblio-html) defines specific conditions for HTML elements to be activated .

An element also matches [:active](#active-pseudo) if one of its descendants in the flat tree (including non-element nodes, such as text nodes) matches the above conditions.

### <a id="the-focus-pseudo"></a>9.3. The Input Focus Pseudo-class: [:focus](#focus-pseudo)

The *<a id="focus-pseudo"></a>:focus* pseudo-class applies while an element (or pseudo-element) has the focus (accepts keyboard or other forms of input).

There may be document language or implementation specific limits on which elements can acquire [:focus](#focus-pseudo). For example, [[HTML]](#biblio-html) defines a list of focusable areas.

Document languages may define additional ways in which an element can match [:focus](#focus-pseudo), except that the :focus pseudo-class must not automatically propagate to the parent element—​see [:focus-within](#focus-within-pseudo) if matching on the parent is desired. (:focus may still apply to the parent element if made to propagate due to other mechanisms, but not merely due to being the parent.)

<a id="issue-1416193e"></a> There’s a desire from authors to propagate [:focus](#focus-pseudo) from a form control to its associated `label` element; the main objection seems to be implementation difficulty. See CSSWG issue (CSS) and WHATWG issue (HTML).

### <a id="the-focus-visible-pseudo"></a>9.4. <a id="focus-ring-pseudo"></a><a id="the-focusring-pseudo"></a> The Focus-Indicated Pseudo-class: [:focus-visible](#focus-visible-pseudo)

While the [:focus](#focus-pseudo) [pseudo-class](#pseudo-class) always matches the currently-focused element, UAs only sometimes visibly *<a id="indicate-focus"></a>indicate focus* (such as by drawing a “focus ring”), instead using a variety of heuristics to visibly indicate the focus only when it would be most helpful to the user. The *<a id="focus-visible-pseudo"></a>:focus-visible* pseudo-class matches a focused element (or pseudo-element) in these situations only, allowing authors to change the appearance of the focus indicator without changing *when* a focus indicator appears.

<a id="example-c40d8011"></a> In this example, all focusable elements get a strong yellow outline on [:focus-visible](#focus-visible-pseudo), and links get both a yellow outline and a yellow background on :focus-visible. These styles are consistent throughout the page and are easily visible due to their bold styling, but do not appear unless the user is likely to need to understand where page focus is.

```
:root {
  --focus-gold: #ffbf47;
}

:focus-visible  {
  outline: 3px solid var(--focus-gold);
}

a:focus-visible {
  background-color: var(--focus-gold);
}
```

<a id="example-ea93cf5c"></a> User agents can choose their own heuristics for when to [indicate focus](#indicate-focus); however, the following (non-normative) suggestions can be used as a starting point for when to indicate focus on the currently focused element:

-
If the user has expressed a preference (such as via a system preference or a browser setting) to always see a visible focus indicator, [indicate focus](#indicate-focus) regardless of any other factors. (Another option may be for the user agent to show its own focus indicator regardless of author styles.)
-
If the element supports keyboard input (such as an `input` element, or any other element that would triggers a virtual keyboard to be shown on focus if a physical keyboard were not present), [indicate focus](#indicate-focus).
-
If the user interacts with the page via keyboard or some other non-pointing device, [indicate focus](#indicate-focus). (This means keyboard usage may change whether this pseudo-class matches even if it doesn’t affect [:focus](#focus-pseudo)).
-
If the user interacts with the page via a pointing device (mouse, touchscreen, etc.) and the focused element does not support keyboard input, don’t [indicate focus](#indicate-focus).
-
If the previously-focused element [indicated focus](#indicate-focus), and a script causes focus to move elsewhere, indicate focus on the newly focused element.
Conversely, if the previously-focused element did not [indicate focus](#indicate-focus), and a script causes focus to move elsewhere, don’t indicate focus on the newly focused element.
-
If a newly-displayed element automatically gains focus (such as an action button in a freshly opened dialog), that element should [indicate focus](#indicate-focus).

User agents should also use [:focus-visible](#focus-visible-pseudo) to specify the default focus style, so that authors using :focus-visible will not also need to disable the default [:focus](#focus-pseudo) style.

### <a id="the-focus-within-pseudo"></a>9.5. The Focus Container Pseudo-class: [:focus-within](#focus-within-pseudo)

The *<a id="focus-within-pseudo"></a>:focus-within* pseudo-class applies to any element (or pseudo-element) for which the [:focus](#focus-pseudo) pseudo-class applies, as well as to an element (or pseudo-element) whose descendant in the flat tree (including non-element nodes, such as text nodes) matches the conditions for matching :focus.

## <a id="resource-pseudos"></a>10. Resource State Pseudo-classes

The pseudo-classes in this section apply to elements that represent loaded resources, particularly images/videos, and allow authors to select them based on some quality of their state.

### <a id="video-state"></a>10.1. Media Playback State: the [:playing](#selectordef-playing), [:paused](#selectordef-paused), and [:seeking](#selectordef-seeking) pseudo-classes

The *<a id="selectordef-playing"></a>:playing* pseudo-class represents an element that is capable of being “played” or “paused”, when that element is “playing”. (This includes both when the element is explicitly playing, and when it’s temporarily stopped for some reason not connected to user intent, but will automatically resume when that reason is resolved, such as a “buffering” or “stalled” state.)

The *<a id="selectordef-paused"></a>:paused* pseudo-class represents an element that is capable of being “played” or “paused”, when that element is “paused” (i.e. *not* ”playing”). (This includes both an explicit “paused” state, and other non-playing states like “loaded, hasn’t been activated yet”, etc.)

The *<a id="selectordef-seeking"></a>:seeking* pseudo-class represents an element that is capable of ”seeking” when that element is ”seeking”. (For the `audio` and `video` elements of HTML, see HTML § 4.8.11.9 Seeking.)

### <a id="media-loading-state"></a>10.2. Media Loading State: the [:buffering](#selectordef-buffering) and [:stalled](#selectordef-stalled) pseudo-classes

The *<a id="selectordef-buffering"></a>:buffering* pseudo-class represents an element that is capable of being “played” or “paused”, when that element cannot continue playing because it is actively attempting to obtain media data but has not yet obtained enough data to resume playback. (Note that the element is still considered to be “playing” when it is “buffering”. Whenever [:buffering](#selectordef-buffering) matches an element, [:playing](#selectordef-playing) also matches the element.)

The *<a id="selectordef-stalled"></a>:stalled* pseudo-class represents an element when that element cannot continue playing because it is actively attempting to obtain media data but it has failed to receive any data for some amount of time. For the `audio` and `video` elements of HTML, this amount of time is the media element stall timeout. [[HTML]](#biblio-html) (Note that, like with the [:buffering](#selectordef-buffering) pseudo-class, the element is still considered to be “playing” when it is “stalled”. Whenever [:stalled](#selectordef-stalled) matches an element, [:playing](#selectordef-playing) also matches the element.)

### <a id="sound-state"></a>10.3. Sound State: the [:muted](#selectordef-muted) and [:volume-locked](#selectordef-volume-locked) pseudo-classes

The *<a id="selectordef-muted"></a>:muted* pseudo-class represents an element that is capable of making sound, but is currently “muted“ (forced silent). (For the `audio` and `video` elements of HTML, see muted. [[HTML]](#biblio-html))

The *<a id="selectordef-volume-locked"></a>:volume-locked* pseudo-class represents an element that is capable of making sound, and currently has its volume "locked" by the UA or the user, so the page author cannot change it. (For the `audio` and `video` elements of HTML, see the algorithm for setting the element’s effective media volume. [[HTML]](#biblio-html))

## <a id="display-state-pseudos"></a>11. Element Display State Pseudo-classes

### <a id="open-state"></a>11.1. Collapse State: the [:open](#selectordef-open) pseudo-class

The *<a id="selectordef-open"></a>:open* pseudo-class represents an element that has both “open” and “closed” states, and which is currently in the “open” state.

Exactly what “open” and “closed” mean is host-language specific, but exemplified by elements such as HTML’s `details`, `select`, `dialog`, and `input` elements, all of which can be toggled “open” to display more content (or any content at all, in the case of `dialog`).

Note: Being “open” or “closed” is a semantic state. An element not currently being displayed (for example, one that has visibility: collapse, or belongs to a display: none subtree) can still be “open” and will match [:open](#selectordef-open).

Note: A ":closed" pseudo-class might be added in the future once the full set of things that support [:open](#selectordef-open) is known.

### <a id="popover-open-state"></a>11.2. Popover State: the [:popover-open](#selectordef-popover-open) pseudo-class

The *<a id="selectordef-popover-open"></a>:popover-open* pseudo-class represents an element that has both “popover-showing” and “popover-hidden” states and which is currently in the “popover-showing” state.

Note: This is distinct from [:open](#selectordef-open) because an element can have element-specific open and closed states and also have separate popover-showing and popover-hidden states associated with being a popover.

Exactly what “popover-showing” and “popover-hidden” states mean is host-language specific, but is exemplified by the presence of the HTML `popover` attribute and the associated popover visibility state.

### <a id="modal-state"></a>11.3. Modal (Exclusive Interaction) State: the [:modal](#selectordef-modal) pseudo-class

The *<a id="selectordef-modal"></a>:modal* pseudo-class represents an element which is in a state that excludes all interaction with elements outside it until it has been dismissed. Multiple elements can be [:modal](#selectordef-modal) simultaneously, with only one of them active (able to receive input).

<a id="example-82b2b6f1"></a> For example, the `dialog` element is [:modal](#selectordef-modal) when opened with the `showModal()` API. Similarly, a [:fullscreen](#selectordef-fullscreen) element is also :modal when opened with the `requestFullscreen()` API, since this prevents interaction with the rest of the page.

### <a id="fullscreen-state"></a>11.4. Fullscreen Presentation State: the [:fullscreen](#selectordef-fullscreen) pseudo-class

The *<a id="selectordef-fullscreen"></a>:fullscreen* pseudo-class represents an element which is displayed in a mode that takes up most (usually all) of the screen, such as that defined by the Fullscreen API. [[FULLSCREEN]](#biblio-fullscreen)

### <a id="pip-state"></a>11.5. Picture-in-Picture Presentation State: the [:picture-in-picture](#selectordef-picture-in-picture) pseudo-class

The *<a id="selectordef-picture-in-picture"></a>:picture-in-picture* pseudo-class represents an element whose contents are being displayed in a “picture-in-picture” mode (i.e. displayed in a window confined to part of the screen and overlaying other content) rather than embedded in the document. For example, this pseudo-class can be used to style a `video` element’s box while its contents are being played in an overlaid window using the Picture-in-Picture API. [[picture-in-picture]](#biblio-picture-in-picture)

## <a id="input-pseudos"></a>12. The Input Pseudo-classes

The pseudo-classes in this section mostly apply to elements that take user input, such as HTML’s input element.

### <a id="input-states"></a>12.1. Input Control States

#### <a id="enableddisabled"></a>12.1.1. The [:enabled](#enabled-pseudo) and [:disabled](#disabled-pseudo) Pseudo-classes

The *<a id="enabled-pseudo"></a>:enabled* pseudo-class represents user interface elements that are in an enabled state; such elements must have a corresponding disabled state.

Conversely, the *<a id="disabled-pseudo"></a>:disabled* pseudo-class represents user interface elements that are in a disabled state; such elements must have a corresponding enabled state.

What constitutes an enabled state, a disabled state, and a user interface element is host-language-dependent. In a typical document most elements will be neither [:enabled](#enabled-pseudo) nor [:disabled](#disabled-pseudo). For example, [[HTML]](#biblio-html) defines non-disabled interactive elements to be :enabled, and any such elements that are explicitly disabled to be :disabled.

Note: CSS properties that might affect a user’s ability to interact with a given user interface element do not affect whether it matches [:enabled](#enabled-pseudo) or [:disabled](#disabled-pseudo); e.g., the display and visibility properties have no effect on the enabled/disabled state of an element.

#### <a id="rw-pseudos"></a>12.1.2. The Mutability Pseudo-classes: [:read-only](#read-only-pseudo) and [:read-write](#read-write-pseudo)

An element matches *<a id="read-write-pseudo"></a>:read-write* if it is user-alterable, as defined by the document language. Otherwise, it is *<a id="read-only-pseudo"></a>:read-only*.

For example, in [[HTML]](#biblio-html) a non-disabled non-readonly `<input>` element is [:read-write](#read-write-pseudo), as is any element with the `contenteditable` attribute set to the true state.

#### <a id="placeholder"></a>12.1.3. The Placeholder-shown Pseudo-class: [:placeholder-shown](#placeholder-shown-pseudo)

Input elements can sometimes show placeholder text as a hint to the user on what to type in. See, for example, the `placeholder` attribute in [[HTML]](#biblio-html). The *<a id="placeholder-shown-pseudo"></a>:placeholder-shown* pseudo-class matches an input element that is showing such placeholder text, whether that text is given by an attribute or a real element, or is otherwise implied by the UA.

<a id="example-37912861"></a> For example, according to the semantics of [[HTML]](#biblio-html) the `placeholder` attribute on the `input` and `textarea` elements provide placeholder text. The [:placeholder-shown](#placeholder-shown-pseudo) class thus applies whenever such placeholder text is shown.

#### <a id="autofill"></a>12.1.4. The Automatic Input Pseudo-class: [:autofill](#selectordef-autofill)

The *<a id="selectordef-autofill"></a>:autofill* pseudo-class represents input elements that have been automatically filled by the user agent, and have not been subsequently altered by the user.

#### <a id="the-default-pseudo"></a>12.1.5. The Default-option Pseudo-class: [:default](#default-pseudo)

The *<a id="default-pseudo"></a>:default* pseudo-class applies to the one or more UI elements that are the default among a set of similar elements. Typically applies to context menu items, buttons and select lists/menus.

One example is the default submit button among a set of buttons. Another example is the default option from a popup menu. In a select-many group (such as for pizza toppings), multiple elements can match [:default](#default-pseudo). For example, [[HTML]](#biblio-html) defines that :default matches the “default button” in a form, the initially-selected `<option>`(s) in a `<select>`, and a few other elements.

### <a id="input-value-states"></a>12.2. Input Value States

#### <a id="checked"></a>12.2.1. <a id="indeterminate"></a> The Selected-option Pseudo-classes: [:checked](#checked-pseudo), [:unchecked](#unchecked-pseudo), and [:indeterminate](#indeterminate-pseudo)

Radio and checkbox elements can be toggled by the user, and some menu items are “checked” when the user selects them. This state is reflected by the *<a id="input-value-pseudo-classes"></a>input value pseudo-classes*.

When such elements are toggled “on” the *<a id="checked-pseudo"></a>:checked* pseudo-class applies. For example, [[HTML]](#biblio-html) defines that checked checkboxes, radio buttons, and selected `<option>` elements match [:checked](#checked-pseudo). Similarly, when such elements are toggled “off”, the *<a id="unchecked-pseudo"></a>:unchecked* pseudo-class applies.

If an element that *could* match [:checked](#checked-pseudo) or [:unchecked](#unchecked-pseudo) is neither "on" nor "off", the *<a id="indeterminate-pseudo"></a>:indeterminate* pseudo-class applies. [:indeterminate](#indeterminate-pseudo) also matches elements which do not have a notion of being "checked", but whose "value" is still in an indeterminate state, such as a progress meter whose progress percentage is unknown.

These three pseudo-classes are mutually exclusive; an element can only match at most one of them at a time. (And might match none, if the concept of being "checked" doesn’t apply to them.) If the "indeterminate" concept is not mutually exclusive with the "checked"/"unchecked" concept in a host language (for example, in HTML a checkbox’s "indeterminate" state is a separate boolean from its "checked" state boolean), being indeterminate wins over being checked/unchecked for the purpose of matching these pseudo-classes.

<a id="example-33865490"></a> For example, in HTML:

-
Checkboxes match [:indeterminate](#indeterminate-pseudo) if their `indeterminate` property is true; otherwise, they match [:checked](#checked-pseudo) or [:unchecked](#unchecked-pseudo) depending on their `checked` property.
-
Radio buttons match [:checked](#checked-pseudo) if their `checked` property is true. Otherwise, they match [:unchecked](#unchecked-pseudo) if they’re in a radio group that contains a :checked radio button. Otherwise (their `checked` property is false, but they’re alone or in a radio group with no checked radio button), they match [:indeterminate](#indeterminate-pseudo).
-
`option` elements within a `select` match [:checked](#checked-pseudo) or [:unchecked](#unchecked-pseudo) depending on their `selected` property. (They’re never [:indeterminate](#indeterminate-pseudo).)
-
`progress` elements match [:indeterminate](#indeterminate-pseudo) if they lack a `value` attribute

While the [input value pseudo-classes](#input-value-pseudo-classes) are dynamic in nature, and can altered by user action, since they can also be based on the presence of semantic attributes in the document (such as the `selected` and `checked` attributes in [[HTML]](#biblio-html)), they apply to all media.

### <a id="ui-validity"></a>12.3. Input Value-checking

#### <a id="validity-pseudos"></a>12.3.1. The Validity Pseudo-classes: [:valid](#valid-pseudo) and [:invalid](#invalid-pseudo)

An element is *<a id="valid-pseudo"></a>:valid* or *<a id="invalid-pseudo"></a>:invalid* when its contents or value is, respectively, valid or invalid with respect to data validity semantics defined by the document language (e.g. [[XFORMS11]](#biblio-xforms11) or [[HTML]](#biblio-html)). An element which lacks data validity semantics is neither [:valid](#valid-pseudo) nor [:invalid](#invalid-pseudo).

Note: There is a difference between an element which has no constraints, and thus would always be [:valid](#valid-pseudo), and one which has no data validity semantics at all, and thus is neither :valid nor [:invalid](#invalid-pseudo). In HTML, for example, an `<input type="text">` element may have no constraints, but a p element has no validity semantics at all, and so it never matches either of these pseudo-classes.

#### <a id="range-pseudos"></a>12.3.2. The Range Pseudo-classes: [:in-range](#in-range-pseudo) and [:out-of-range](#out-of-range-pseudo)

The *<a id="in-range-pseudo"></a>:in-range* and *<a id="out-of-range-pseudo"></a>:out-of-range* pseudo-classes apply only to elements that have range limitations. An element is [:in-range](#in-range-pseudo) or [:out-of-range](#out-of-range-pseudo) when the value that the element is bound to is in range or out of range with respect to its range limits as defined by the document language. An element that lacks data range limits or is not a form control is neither :in-range nor :out-of-range. E.g. a slider element with a value of 11 presented as a slider control that only represents the values from 1-10 is :out-of-range. Another example is a menu element with a value of "E" that happens to be presented in a popup menu that only has choices "A", "B" and "C".

#### <a id="opt-pseudos"></a>12.3.3. The Optionality Pseudo-classes: [:required](#required-pseudo) and [:optional](#optional-pseudo)

A form element is *<a id="required-pseudo"></a>:required* or *<a id="optional-pseudo"></a>:optional* if a value for it is, respectively, required or optional before the form it belongs to can be validly submitted. Elements that are not form elements are neither required nor optional.

#### <a id="user-pseudos"></a>12.3.4. The User-interaction Pseudo-classes: [:user-valid](#user-valid-pseudo) and [:user-invalid](#user-invalid-pseudo)

The *<a id="user-invalid-pseudo"></a>:user-invalid* and the *<a id="user-valid-pseudo"></a>:user-valid* pseudo-classes represent an element with incorrect or correct input, respectively, but only *after* the user has significantly interacted with it. Their purpose is to help the user identify mistakes in their input.

These pseudo-classes must at least match their respective elements after the user has attempted to submit the form and before the user has interacted again with the input element or reset the form. They may also match at other times, as would be appropriate for highlighting an error to the user. For example, [:user-invalid](#user-invalid-pseudo) could start matching an [:invalid](#invalid-pseudo) input element once the user has changed its value and focus has moved to another element; or stop matching only after the user has successfully corrected the input.

Host languages may define more precise matching rules or defer to platform conventions; otherwise the exact behavior is UA-defined. Regardless, the [:user-invalid](#user-invalid-pseudo) pseudo-class must only match [:invalid](#invalid-pseudo) elements; and the [:user-valid](#user-valid-pseudo) pseudo-class must only match [:valid](#valid-pseudo) elements. See the HTML specification for the specific rules pertaining to HTML elements. [[HTML]](#biblio-html)

<a id="example-81851d24"></a> For example, the `input` element in the following document fragment would match [:invalid](#invalid-pseudo) as soon as the page is loaded (because the `input`’s initial value violates its max constraint) but it won’t match [:user-invalid](#user-invalid-pseudo) until the user significantly interacts with the input field or attempts to submit the form it’s part of.

```
<form>
  <label>
    Volume:
    <input name='vol' type=number min=0 max=10 value=11>
  </label>
  ...
</form>
```

## <a id="structural-pseudos"></a>13. Tree-Structural pseudo-classes

Selectors introduces the concept of *<a id="structural-pseudo-classes"></a>structural pseudo-classes* to permit selection based on extra information that lies in the document tree but cannot be represented by other simple selectors or combinators.

Standalone text and other non-element nodes are not counted when calculating the position of an element in the list of children of its parent. When calculating the position of an element in the list of children of its parent, the index numbering starts at 1.

The [structural pseudo-classes](#structural-pseudo-classes) only apply to elements in the document tree; they must never match [pseudo-elements](#pseudo-element).

### <a id="the-root-pseudo"></a>13.1. [:root](#root-pseudo) pseudo-class

The *<a id="root-pseudo"></a>:root* pseudo-class represents an element that is the root of the document.

For example, in a DOM document, the [:root](#root-pseudo) pseudo-class matches the document element. In HTML, this will be the `html` element (unless scripting has been used to modify the document).

### <a id="the-empty-pseudo"></a>13.2. [:empty](#empty-pseudo) pseudo-class

The *<a id="empty-pseudo"></a>:empty* pseudo-class represents an element that has no children except, optionally, document white space characters. In terms of the document tree, only element nodes and content nodes (such as [[DOM]](#biblio-dom) text nodes, and entity references) whose data has a non-zero length must be considered as affecting emptiness; comments, processing instructions, and other nodes must not affect whether an element is considered empty or not.

<a id="example-c085b235"></a> Examples: p:empty is a valid representation of the `p` elements in the following HTML fragment:

```
<p></p>
<p>
<p> </p>
<p> <!-- comment --></p>
```

div:empty is not a valid representation of the `<div>` elements in the following fragment:

```
<div>text</div>
<div><p></p></div>
<div>&nbsp;</div>
<div><p>bla</p></div>
<div>this is not <p>:empty</p></div>
```

Note: In Level 2 and Level 3 of Selectors, [:empty](#empty-pseudo) did not match elements that contained only white space. This was changed so that that—​given white space is largely collapsible in HTML and is therefore used for source code formatting, and especially because elements with omitted end tags are likely to absorb such white space into their DOM text contents—​elements which authors perceive of as empty can be selected by this selector, as they expect.

### <a id="child-index"></a>13.3. Child-indexed Pseudo-classes

The pseudo-classes defined in this section select elements based on their index amongst their inclusive siblings.

Note: Selectors 3 described these selectors as selecting elements based on their index in the child list of their parents. (This description survives in the name of this very section, and the names of several of the pseudo-classes.) As there was no reason to exclude them from matching elements without parents, or with non-element parents, they have been rephrased to refer to an element’s relative index amongst its siblings.

#### <a id="the-nth-child-pseudo"></a>13.3.1. [:nth-child()](#nth-child-pseudo) pseudo-class

The *<a id="nth-child-pseudo"></a>:nth-child(*An+B* [of *S*]? )* pseudo-class notation represents elements that are among *An+B*th elements from the list composed of their inclusive siblings that match the [selector list](#selector-list) *S*, which is a [<complex-real-selector-list>](#typedef-complex-real-selector-list). If *S* is omitted, it defaults to *|*.

The *An+B* notation and its interpretation are defined in [CSS Syntax 3 § 6 The An+B microsyntax](css-syntax-3.md#anb-microsyntax); it represents any index *i* = *A**n* + *B* for any non-negative integer *n*.

Note: For these purposes, the list of elements is **1-indexed**; that is, the first child of an element has index 1, and will be matched by :nth-child(2n+1), because when `n=0` the expression evaluates to 1.

For example, this selector could address every other row in a table, and could be used to alternate the color of paragraph text in a cycle of four.

<a id="example-b8d691c9"></a> Examples:

```
:nth-child(even)   /* represents the 2nd, 4th, 6th, etc elements
:nth-child(10n-1)  /* represents the 9th, 19th, 29th, etc elements */
:nth-child(10n+9)  /* Same */
:nth-child(10n+-1) /* Syntactically invalid, and would be ignored */
```

Note: The specificity of the [:nth-child()](#nth-child-pseudo) pseudo-class is the specificity of a single pseudo-class plus, if *S* is specified, the specificity of the most specific [complex selector](#complex) in *S*. See [§ 15 Calculating a selector’s specificity](#specificity-rules). Thus *S*:nth-child(*An+B*) and :nth-child(*An+B* of *S*) have the exact same specificity, although they do differ in behavior (see example below).

<a id="example-3c07f717"></a> By passing a selector argument, we can select the Nth element that matches that selector. For example, the following selector matches the first three “important” list items, denoted by the .important class:

```
:nth-child(-n+3 of li.important)
```

Note that this is different from moving the selector outside of the function, like:

```
li.important:nth-child(-n+3)
```

This selector instead just selects the first three children if they also happen to be "important" list items.

<a id="example-48f80a26"></a> Here’s another example of using the selector argument, to ensure that zebra-striping a table works correctly.

Normally, to zebra-stripe a table’s rows, an author would use CSS similar to the following:

```
tr {
  background: white;
}
tr:nth-child(even) {
  background: silver;
}
```

However, if some of the rows are hidden and not displayed, this can break up the pattern, causing multiple adjacent rows to have the same background color. Assuming that rows are hidden with the [hidden] attribute in HTML, the following CSS would zebra-stripe the table rows robustly, maintaining a proper alternating background regardless of which rows are hidden:

```
tr {
  background: white;
}
tr:nth-child(even of :not([hidden])) {
  background: silver;
}
```

#### <a id="the-nth-last-child-pseudo"></a>13.3.2. [:nth-last-child()](#nth-last-child-pseudo) pseudo-class

The *<a id="nth-last-child-pseudo"></a>:nth-last-child(*An+B* [of *S*]? )* pseudo-class notation represents elements that are among *An+B*th elements from the list composed of their inclusive siblings that match the [selector list](#selector-list) *S*, counting backwards from the end. *S* is [<complex-real-selector-list>](#typedef-complex-real-selector-list). If *S* is omitted, it defaults to *|*.

Note: The specificity of the [:nth-last-child()](#nth-last-child-pseudo) pseudo-class, like the [:nth-child()](#nth-child-pseudo) pseudo-class, combines the specificity of a regular pseudo-class with that of its selector argument *S*. See [§ 15 Calculating a selector’s specificity](#specificity-rules).

The CSS Syntax Module [[CSS3SYN]](#biblio-css3syn) defines the [*An+B* notation](css-syntax-3.md#anb).

<a id="example-bee68ad4"></a> Examples:

```
tr:nth-last-child(-n+2)    /* represents the two last rows of an HTML table */

foo:nth-last-child(odd)    /* represents all odd foo elements in their parent element,
                              counting from the last one */
```

#### <a id="the-first-child-pseudo"></a>13.3.3. [:first-child](#first-child-pseudo) pseudo-class

The *<a id="first-child-pseudo"></a>:first-child* pseudo-class represents an element that is first among its inclusive siblings. Same as :nth-child(1).

<a id="example-72934891"></a> Examples: The following selector represents a p element that is the first child of a div element:

```
div > p:first-child
```

This selector can represent the `p` inside the `div` of the following fragment:

```
<p> The last P before the note.</p>
<div class="note">
   <p> The first P inside the note.</p>
</div>
```

but cannot represent the second `p` in the following fragment:

```
<p> The last P before the note.</p>
<div class="note">
   <h2> Note </h2>
   <p> The first P inside the note.</p>
</div>
```

The following two selectors are usually equivalent:

```
* > a:first-child /* a is first child of any element */
a:first-child /* Same (assuming a is not the root element) */
```

#### <a id="the-last-child-pseudo"></a>13.3.4. [:last-child](#last-child-pseudo) pseudo-class

The *<a id="last-child-pseudo"></a>:last-child* pseudo-class represents an element that is last among its inclusive siblings. Same as :nth-last-child(1).

<a id="example-48137186"></a> Example: The following selector represents a list item `li` that is the last child of an ordered list `ol`.

```
ol > li:last-child
```

#### <a id="the-only-child-pseudo"></a>13.3.5. [:only-child](#only-child-pseudo) pseudo-class

The *<a id="only-child-pseudo"></a>:only-child* pseudo-class represents an element that has no siblings. Same as :first-child:last-child or :nth-child(1):nth-last-child(1), but with a lower specificity.

### <a id="typed-child-index"></a>13.4. Typed Child-indexed Pseudo-classes

The pseudo-classes in this section are similar to the [Child Index Pseudo-classes](#child-index), but they resolve based on an element’s index **among elements of the same [type (tag name)](#type-selectors)** in their sibling list.

#### <a id="the-nth-of-type-pseudo"></a>13.4.1. [:nth-of-type()](#nth-of-type-pseudo) pseudo-class

The *<a id="nth-of-type-pseudo"></a>:nth-of-type(*An+B*)* pseudo-class notation represents the same elements that would be matched by :nth-child(|An+B| of *S*), where *S* is a [type selector](#type-selector) and namespace prefix matching the element in question. For example, when considering whether an HTML `img` element matches this [pseudo-class](#pseudo-class), the *S* in question is html|img (assuming an appropriate `html` namespace is declared).

<a id="example-3d09bf4d"></a> CSS example: This allows an author to alternate the position of floated images:

```
img:nth-of-type(2n+1) { float: right; }
img:nth-of-type(2n) { float: left; }
```

Note: If the type of the element is known ahead of time, this pseudo-class is equivalent to using [:nth-child()](#nth-child-pseudo) with a type selector. That is, img:nth-of-type(2) is equivalent to *:nth-child(2 of img).

#### <a id="the-nth-last-of-type-pseudo"></a>13.4.2. [:nth-last-of-type()](#nth-last-of-type-pseudo) pseudo-class

The *<a id="nth-last-of-type-pseudo"></a>:nth-last-of-type(*An+B*)* pseudo-class notation represents the same elements that would be matched by :nth-last-child(|An+B| of *S*), where *S* is a [type selector](#type-selector) and namespace prefix matching the element in question. For example, when considering whether an HTML `img` element matches this [pseudo-class](#pseudo-class), the *S* in question is html|img (assuming an appropriate `html` namespace is declared).

<a id="example-b89418c8"></a> Example: To represent all `h2` children of an XHTML `body` except the first and last, one could use the following selector:

```
body > h2:nth-of-type(n+2):nth-last-of-type(n+2) 
```

In this case, one could also use [:not()](#negation-pseudo), although the selector ends up being just as long:

```
body > h2:not(:first-of-type):not(:last-of-type) 
```

#### <a id="the-first-of-type-pseudo"></a>13.4.3. [:first-of-type](#first-of-type-pseudo) pseudo-class

The *<a id="first-of-type-pseudo"></a>:first-of-type* pseudo-class represents the same element as :nth-of-type(1).

<a id="example-16f73753"></a> Example: The following selector represents a definition title `dt` inside a definition list `dl`, this `dt` being the first of its type in the list of children of its parent element.

```
dl dt:first-of-type
```

It is a valid description for the first two `dt` elements in the following example but not for the third one:

```
<dl>
  <dt>gigogne</dt>
  <dd>
    <dl>
      <dt>fusée</dt>
      <dd>multistage rocket</dd>
      <dt>table</dt>
      <dd>nest of tables</dd>
    </dl>
  </dd>
</dl>
```

#### <a id="the-last-of-type-pseudo"></a>13.4.4. [:last-of-type](#last-of-type-pseudo) pseudo-class

The *<a id="last-of-type-pseudo"></a>:last-of-type* pseudo-class represents the same element as :nth-last-of-type(1).

<a id="example-fee30007"></a> Example: The following selector represents the last data cell `td` of a table row `tr`.

```
tr > td:last-of-type
```

#### <a id="the-only-of-type-pseudo"></a>13.4.5. [:only-of-type](#only-of-type-pseudo) pseudo-class

The *<a id="only-of-type-pseudo"></a>:only-of-type* pseudo-class represents the same element as :first-of-type:last-of-type.

## <a id="combinators"></a>14. Combinators

### <a id="descendant-combinators"></a>14.1. Descendant combinator (` `)

At times, authors may want selectors to describe an element that is the descendant of another element in the document tree (e.g., "an em element that is contained within an H1 element"). The *<a id="descendant-combinator"></a>descendant combinator* expresses such a relationship.

A descendant combinator is whitespace that separates two [compound selectors](#compound).

A selector of the form A B represents an element `B` that is an arbitrary descendant of some ancestor element `A`.

<a id="example-13246ba1"></a> Examples: For example, consider the following selector:

```
h1 em
```

It represents an em element being the descendant of an h1 element. It is a correct and valid, but partial, description of the following fragment:

```
<h1>This <span class="myclass">headline
is <em>very</em> important</span></h1>
```

The following selector:

```
div * p
```

represents a p element that is a grandchild or later descendant of a div element. Note the whitespace on either side of the "*" is not part of the universal selector; the whitespace is a combinator indicating that the `div` must be the ancestor of some element, and that that element must be an ancestor of the `p`. The following selector, which combines descendant combinators and [attribute selectors](#attribute-selectors), represents an element that (1) has the `href` attribute set and (2) is inside a `p` that is itself inside a `div`:

```
div p *[href]
```

### <a id="child-combinators"></a>14.2. Child combinator (`>`)

A *<a id="child-combinator"></a>child combinator* describes a childhood relationship between two elements. A child combinator is made of the "greater-than sign" (U+003E, *<a id="selectordef-child"></a>>*) code point and separates two [compound selectors](#compound).

<a id="example-63e612ba"></a> Examples: The following selector represents a p element that is child of `body`:

```
body > p
```

The following example combines descendant combinators and child combinators.

```
div ol>li p
```

It represents a p element that is a descendant of an li element; the li element must be the child of an ol element; the ol element must be a descendant of a `div`. Notice that the optional white space around the ">" combinator has been left out.

For information on selecting the first child of an element, please see the section on the [:first-child](#first-child-pseudo) pseudo-class above.

### <a id="adjacent-sibling-combinators"></a>14.3. Next-sibling combinator (`+`)

The *<a id="next-sibling-combinator"></a>next-sibling combinator* is made of the “plus sign” (U+002B, *<a id="selectordef-adjacent"></a>+*) code point that separates two [compound selectors](#compound). The elements represented by the two compound selectors share the same parent in the document tree and the element represented by the first compound selector immediately precedes the element represented by the second one. Non-element nodes (e.g. text between elements) are ignored when considering the adjacency of elements.

<a id="example-0f0597d9"></a> Examples: The following selector represents a p element immediately following a math element:

```
math + p
```

The following selector is conceptually similar to the one in the previous example, except that it adds an attribute selector — it adds a constraint to the h1 element, that it must have `class="opener"`:

```
h1.opener + h2
```

### <a id="general-sibling-combinators"></a>14.4. Subsequent-sibling combinator (`~`)

The *<a id="subsequent-sibling-combinator"></a>subsequent-sibling combinator* is made of the "tilde" (U+007E, *<a id="selectordef-sibling"></a>~*) code point that separates two [compound selectors](#compound). The elements represented by the two compound selectors share the same parent in the document tree and the element represented by the first compound selector precedes (not necessarily immediately) the element represented by the second one.

<a id="example-ae614b62"></a>

```
h1 ~ pre
```

represents a pre element following an `h1`. It is a correct and valid, but partial, description of:

```
<h1>Definition of the function a</h1>
<p>Function a(x) has to be applied to all figures in the table.</p>
<pre>function a(x) = 12x/13.5</pre>
```

## <a id="specificity-rules"></a>15. Calculating a selector’s specificity

A selector’s *<a id="specificity"></a>specificity* is calculated for a given element as follows:

- count the number of ID selectors in the selector (= *A*)
- count the number of class selectors, attributes selectors, and pseudo-classes in the selector (= *B*)
- count the number of type selectors and pseudo-elements in the selector (= *C*)
- ignore the universal selector

If the selector is a [selector list](#selector-list), this number is calculated for each selector in the list. For a given matching process against the list, the specificity in effect is that of the most specific selector in the list that matches.

A few pseudo-classes provide “evaluation contexts” for other selectors, and so have their specificity defined specially:

- The specificity of an [:is()](#matches-pseudo), [:not()](#negation-pseudo), or [:has()](#has-pseudo) pseudo-class is replaced by the specificity of the most specific [complex selector](#complex) in its [selector list](#selector-list) argument.
- Analogously, the specificity of an [:nth-child()](#nth-child-pseudo) or [:nth-last-child()](#nth-last-child-pseudo) selector is the specificity of the pseudo-class itself (counting as one pseudo-class selector) plus the specificity of the most specific [complex selector](#complex) in its [selector list](#selector-list) argument (if any).
- The specificity of a [:where()](#where-pseudo) pseudo-class is replaced by zero.

<a id="example-66faea1b"></a> For example:

- :is(em, #foo) has a specificity of (1,0,0)—​like an ID selector (#foo)—​when matched against any of `<em>`, `<p id=foo>`, or `<em id=foo>`.
- .qux:where(em, #foo#bar#baz) has a specificity of (0,1,0): only the .qux outside the [:where()](#where-pseudo) contributes to selector specificity.
- :nth-child(even of li, .item) has a specificity of (0,2,0)—​like a class selector (.item) plus a pseudo-class—​when matched against any of `<li>`, `<ul class=item>`, or `<li class=item id=foo>`.
- :not(em, strong#foo) has a specificity of (1,0,1)—​like a tag selector (strong) combined with an ID selector (#foo)—​when matched against any element.

Specificities are compared by comparing the three components in order: the specificity with a larger *A* value is more specific; if the two *A* values are tied, then the specificity with a larger *B* value is more specific; if the two *B* values are also tied, then the specificity with a larger *C* value is more specific; if all the values are tied, the two specificities are equal.

Due to storage limitations, implementations may have limitations on the size of *A*, *B*, or *C*. If so, values higher than the limit must be clamped to that limit, and not overflow.

<a id="example-d97bd125"></a> Examples:

```
*               /* a=0 b=0 c=0 */
LI              /* a=0 b=0 c=1 */
UL LI           /* a=0 b=0 c=2 */
UL OL+LI        /* a=0 b=0 c=3 */
H1 + *[REL=up]  /* a=0 b=1 c=1 */
UL OL LI.red    /* a=0 b=1 c=3 */
LI.red.level    /* a=0 b=2 c=1 */
#x34y           /* a=1 b=0 c=0 */
#s12:not(FOO)   /* a=1 b=0 c=1 */
.foo :is(.bar, #baz)
                /* a=1 b=1 c=0 */
```

Note: Repeated occurrences of the same simple selector are allowed and do increase specificity.

Note: The specificity of the styles specified in an HTML `style` attribute is described in CSS Style Attributes. [[CSSSTYLEATTR]](#biblio-cssstyleattr)

## <a id="grammar"></a>16. <a id="formal-syntax"></a> Grammar

Selectors are [parsed](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) according to the following grammar:

```
<a id="typedef-selector-list"></a><selector-list> = <complex-selector-list>

<a id="typedef-complex-selector-list"></a><complex-selector-list> = <complex-selector>#
<a id="typedef-complex-real-selector-list"></a><complex-real-selector-list> = <complex-real-selector>#

<a id="typedef-compound-selector-list"></a><compound-selector-list> = <compound-selector>#

<a id="typedef-simple-selector-list"></a><simple-selector-list> = <simple-selector>#

<a id="typedef-relative-selector-list"></a><relative-selector-list> = <relative-selector>#
<a id="typedef-relative-real-selector-list"></a><relative-real-selector-list> = <relative-real-selector>#

<a id="typedef-complex-selector"></a><complex-selector> = <complex-selector-unit> [ <combinator>? <complex-selector-unit> ]*
<a id="typedef-complex-selector-unit"></a><complex-selector-unit> = [ <compound-selector>? <pseudo-compound-selector>* ]!
<a id="typedef-complex-real-selector"></a><complex-real-selector> = <compound-selector> [ <combinator>? <compound-selector> ]*

<a id="typedef-relative-selector"></a><relative-selector> = <combinator>? <complex-selector>
<a id="typedef-relative-real-selector"></a><relative-real-selector> = <combinator>? <complex-real-selector>

<a id="typedef-compound-selector"></a><compound-selector> = [ <type-selector>? <subclass-selector>* ]!
<a id="typedef-pseudo-compound-selector"></a><pseudo-compound-selector> =  <pseudo-element-selector> <pseudo-class-selector>*

<a id="typedef-simple-selector"></a><simple-selector> = <type-selector> | <subclass-selector>

<a id="typedef-combinator"></a><combinator> = '>' | '+' | '~'

<a id="typedef-wq-name"></a><wq-name> = <ns-prefix>? <ident-token>
<a id="typedef-ns-prefix"></a><ns-prefix> = [ <ident-token> | '*' ]? '|'

<a id="typedef-type-selector"></a><type-selector> = <wq-name> | <ns-prefix>? '*'

<a id="typedef-subclass-selector"></a><subclass-selector> = <id-selector> | <class-selector> |
                      <attribute-selector> | <pseudo-class-selector>

<a id="typedef-id-selector"></a><id-selector> = <hash-token>

<a id="typedef-class-selector"></a><class-selector> = '.' <ident-token>

<a id="typedef-attribute-selector"></a><attribute-selector> = '[' <wq-name> ']' |
    '[' <wq-name> <attr-matcher> [ <string-token> | <ident-token> ] <attr-modifier>? ']'
<a id="typedef-attr-matcher"></a><attr-matcher> = [ '~' | '|' | '^' | '$' | '*' ]? '='
<a id="typedef-attr-modifier"></a><attr-modifier> = i | s

<a id="typedef-pseudo-class-selector"></a><pseudo-class-selector> = : <ident-token> |
                          : <function-token> <any-value> )

<a id="typedef-pseudo-element-selector"></a><pseudo-element-selector> = : <pseudo-class-selector> | <legacy-pseudo-element-selector>
<a id="typedef-legacy-pseudo-element-selector"></a><legacy-pseudo-element-selector> =  : [before | after | first-line | first-letter]
```

In interpreting the above grammar, the following rules apply:

- <a id="white-space"></a> White space is forbidden:
  -
  Between any of the top-level components of a [<compound-selector>](#typedef-compound-selector) or [<complex-selector-unit>](#typedef-complex-selector-unit) (that is, forbidden between the [<type-selector>](#typedef-type-selector) and [<subclass-selector>](#typedef-subclass-selector), or between the [<pseudo-element-selector>](#typedef-pseudo-element-selector) and [<pseudo-class-selector>](#typedef-pseudo-class-selector), etc).
  -
  Between *any* of the components of a [<type-selector>](#typedef-type-selector) or a [<class-selector>](#typedef-class-selector).
  -
  Between the ':'s, or between the ':' and [<ident-token>](css-syntax-3.md#typedef-ident-token) or [<function-token>](css-syntax-3.md#typedef-function-token), of a [<pseudo-element-selector>](#typedef-pseudo-element-selector) or a [<pseudo-class-selector>](#typedef-pseudo-class-selector).
  -
  Between *any* of the components of a [<wq-name>](#typedef-wq-name).
  -
  Between the components of an [<attr-matcher>](#typedef-attr-matcher).
  -
  Between the [<compound-selector>](#typedef-compound-selector) or [<pseudo-compound-selector>](#typedef-pseudo-compound-selector)s in a [<complex-selector-unit>](#typedef-complex-selector-unit)
Whitespace is *required* between two [<complex-selector-unit>](#typedef-complex-selector-unit)s if the [<combinator>](#typedef-combinator) between them is omitted. (This indicates the descendant combinator is being used.)
- In [<id-selector>](#typedef-id-selector), the [<hash-token>](css-syntax-3.md#typedef-hash-token)’s value must be an [identifier](css-values-4.md#css-css-identifier).
- The [<pseudo-class-selector>](#typedef-pseudo-class-selector) production excludes the [<legacy-pseudo-element-selector>](#typedef-legacy-pseudo-element-selector) production. (That is, :before/etc must never be parsed as a pseudo-class, even if doing so would cause the selector to become valid due to, for example, other simple selectors following it.)

Note: A selector is also subject to a variety of more specific syntactic constraints, and adherence to the grammar above is necessary *but not sufficient* for the selector to be considered valid. See [§ 3.9 Invalid Selectors and Error Handling](#invalid) for additional rules for parsing selectors.

Note: In general, a [<pseudo-element-selector>](#typedef-pseudo-element-selector) is only valid if placed at the end of the last [<compound-selector>](#typedef-compound-selector) in a [<complex-selector>](#typedef-complex-selector). In some circumstances, however, it can be followed by more <pseudo-element-selector>s or [<pseudo-class-selector>](#typedef-pseudo-class-selector)s; but these are specified on a case-by-case basis. (For example, the [user action pseudo-classes](#user-action-pseudo-class) are allowed after any [pseudo-element](#pseudo-element), and the tree-abiding pseudo-elements are allowed after the ::slotted() pseudo-element.)

<a id="single-colon-pseudos"></a> The four Level 2 [pseudo-elements](#pseudo-element) (::before, ::after, ::first-line, and ::first-letter) may, for legacy reasons, be written with only a single ":" character at their front, making them resemble a [<pseudo-class-selector>](#typedef-pseudo-class-selector).

### <a id="forgiving-selector"></a>16.1. [<forgiving-selector-list>](#typedef-forgiving-selector-list)

For legacy reasons, the general behavior of a selector list is that if any selector in the list fails to parse (because it uses new or UA-specific selector features, for instance), the entire selector list becomes invalid. This can make it hard to write CSS that uses new selectors and still works correctly in older user agents.

The *<a id="typedef-forgiving-selector-list"></a>[<forgiving-selector-list>](#typedef-forgiving-selector-list)* production instead parses each selector in the list individually, simply ignoring ones that fail to parse, so the remaining selectors can still be used.

Note: Style rules still use the normal, unforgiving selector list behavior. [<forgiving-selector-list>](#typedef-forgiving-selector-list) is used in [:is()](#matches-pseudo) and [:where()](#where-pseudo) only. Although it does have some minor implications on specificity, wrapping a style rule’s selector in :is() effectively "upgrades" it to become forgiving, so long as it doesn’t contain any pseudo-elements (which aren’t valid in :is() or :where()).

Syntactically, [<forgiving-selector-list>](#typedef-forgiving-selector-list) is equivalent to `[<any-value>](css-syntax-3.md#typedef-any-value)?`. It is then [parsed as a forgiving selector list](#parse-as-a-forgiving-selector-list) to obtain its actual value.

To *<a id="parse-as-a-forgiving-selector-list"></a>parse as a forgiving selector list* given an input *input*:

1.
[Parse a list](css-syntax-3.md#css-parse-a-comma-separated-list-according-to-a-css-grammar) of [<complex-real-selector>](#typedef-complex-real-selector)s from *input*, and let *selector list* be the result.
2.
Remove all failure items from *selector list*, and all items that are [invalid selectors](#invalid-selector), then return a [<selector-list>](#typedef-selector-list) representing the remaining items in *selector list*. (This might be empty.)

Any items in a [<forgiving-selector-list>](#typedef-forgiving-selector-list) that are invalid (whether explicitly, by using unknown selectors or syntax, or merely contextually, using known syntax but in an invalid context) must be treated as having zero specificity.

Note: [<forgiving-selector-list>](#typedef-forgiving-selector-list) is intentionally used only in [:is()](#matches-pseudo) and [:where()](#where-pseudo), not in any other selector that takes a selector argument.

## <a id="api-hooks"></a>17. API Hooks

To aid in the writing of specs that use Selectors concepts, this section defines several API hooks that can be invoked by other specifications.

<a id="issue-55d7bd68"></a> Are these still necessary now that we have more rigorous definitions for [match](#match) and [invalid selector](#invalid-selector)? Nouns are a lot easier to coordinate across specification than predicates, and details like the exact order of elements returned from `querySelector` seem to make more sense being defined in the DOM specification than in Selectors.

### <a id="parse-selector"></a>17.1. Parse A Selector

This section defines how to *<a id="parse-a-selector"></a>parse a selector* from a string *source*. It returns either a complex selector list, or failure.

1. Let *selector* be the result of [parsing](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) *source* as a [<selector-list>](#typedef-selector-list). If this returns failure, it’s an [invalid selector](#invalid-selector); return failure.
2. If *selector* is an [invalid selector](#invalid-selector) for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
3. Otherwise, return *selector*.

### <a id="parse-relative-selector"></a>17.2. Parse A Relative Selector

This section defines how to *<a id="parse-a-relative-selector"></a>parse a relative selector* from a string *source*. It returns either a complex selector list, or failure.

1. Let *selector* be the result of [parsing](css-syntax-3.md#css-parse-something-according-to-a-css-grammar) *source* as a [<relative-selector-list>](#typedef-relative-selector-list). If this returns failure, it’s an [invalid selector](#invalid-selector); return failure.
2. If *selector* is an [invalid selector](#invalid-selector) for any other reason (such as, for example, containing an undeclared namespace prefix), return failure.
3. Otherwise, return *selector*.

### <a id="match-against-element"></a>17.3. Match a Selector Against an Element

This section defines how to *<a id="match-a-selector-against-an-element"></a>match a selector against an element*.

APIs using this algorithm must provide a *selector* and an *element*.

Callers may optionally provide:

- one or more [scoping roots](#scoping-root), for resolving the [:scope](#scope-pseudo) pseudo-class against.

This algorithm returns either success or failure.

For each [complex selector](#complex) in the given *selector* (which is taken to be a [list of complex selectors](#list-of-simple-selectors)), match the complex selector against *element*, as described in the following paragraph. If the matching returns success for any complex selector, then the algorithm return success; otherwise it returns failure.

To *<a id="match-a-complex-selector-against-an-element"></a>match a complex selector against an element*, process it [compound selector](#compound) at a time, in right-to-left order. This process is defined recursively as follows:

- If any simple selectors in the rightmost compound selector does not match the element, return failure.
- Otherwise, if there is only one compound selector in the complex selector, return success.
- Otherwise, consider all possible elements that could be related to this element by the rightmost [combinator](#selector-combinator). If the operation of matching the selector consisting of this selector with the rightmost compound selector and rightmost combinator removed against any one of these elements returns success, then return success. Otherwise, return failure.

### <a id="match-against-pseudo-element"></a>17.4. Match a Selector Against a Pseudo-element

This section defines how to *<a id="match-a-selector-against-a-pseudo-element"></a>match a selector against a pseudo-element*.

APIs using this algorithm must provide a *selector* and a *pseudo-element*. They may optionally provide the same things they may optionally provide to the algorithm to [match a selector against an element](#match-a-selector-against-an-element).

This algorithm returns success or failure.

For each [complex selector](#complex) in the given *selector*, if both:

- the rightmost [simple selector](#simple) in the complex selector matches *pseudo-element*, and
- the result of running [match a complex selector against an element](#match-a-complex-selector-against-an-element) on the remainder of the [complex selector](#complex) (with just the rightmost simple selector of its rightmost complex selector removed), *pseudo-element*’s corresponding element, and any optional parameters provided to this algorithm returns success,

then return success.

Otherwise (that is, if this doesn’t happen for any of the complex selectors in *selector*), return failure.

### <a id="match-against-tree"></a>17.5. Match a Selector Against a Tree

This section defines how to *<a id="match-a-selector-against-a-tree"></a>match a selector against a tree*.

APIs using this algorithm must provide a selector, and one or more *root elements* indicating the subtrees that will be searched by the selector. All of the *root elements* must share the same root, or else calling this algorithm is invalid.

They may optionally provide:

- One or more [scoping roots](#scoping-root) indicating the selector is [scoped](#scoped-selector).
- A list of [pseudo-elements](#pseudo-element) that are allowed to show up in the match list. If not specified, this defaults to allowing all pseudo-elements.
<a id="issue-15f3317f"></a> Only the tree-abiding pseudo-elements are really handled in any way remotely like this.

This algorithm returns a (possibly empty) list of elements.

1. Start with a list of *candidate elements*, which are the *root elements* and all of their descendant elements, sorted in shadow-including tree order, unless otherwise specified.
2. If [scoping root](#scoping-root) were provided, then remove from the *candidate elements* any elements that are not descendants of at least one scoping root.
3. Initialize the *selector match list* to empty.
4. For each *element* in the set of *candidate elements*:
  1. If the result of [match a selector against an element](#match-a-selector-against-an-element) for *element* and *selector* is success, add *element* to the *selector match list*.
  2. For each possible pseudo-element associated with *element* that is one of the pseudo-elements allowed to show up in the match list, if the result of [match a selector against a pseudo-element](#match-a-selector-against-a-pseudo-element) for the pseudo-element and *selector* is success, add the pseudo-element to the *selector match list*.
  <a id="issue-b7f52036"></a> The relative position of pseudo-elements in *selector match list* is undefined. There’s not yet a context that exposes this information, but we need to decide on something eventually, before something *is* exposed.

## <a id="dom-mapping"></a> Appendix A: Guidance on Mapping Source Documents & Data to an Element Tree

*This section is informative.*

The element tree structure described by the DOM is powerful and useful, but generic enough to model pretty much any language that describes tree-based data (or even graph-based, with a suitable interpretation).

Some languages, like HTML, already have well-defined procedures for producing a DOM object from a resource. If a given language does not, such a procedure must be defined in order for Selectors to apply to documents in that language.

At minimum, the document language must define what maps to the DOM concept of an "element".

The primary one-to-many relationship between nodes—​parent/child in tree-based structures, element/neighbors in graph-based structures—​should be reflected as the child nodes of an element.

Other features of the element should be mapped to something that serves a similar purpose to the same feature in DOM:

**type **
: If the elements in the document language have some notion of "type" as a basic distinguisher between different groups of elements, it should be reflected as the "type" feature.

If this "type" can be separated into a "basic" name and a "namespace" that groups names into higher-level groups, the latter should be reflected as the "namespace" feature. Otherwise, the element shouldn’t have a "namespace" feature, and the entire name should be reflected as the "type" feature.

**id **
: If some aspect of the element functions as a unique identifier across the document, it should be mapped to the "id" feature.

Note: While HTML only allows an element to have a single ID, this should not be taken as a general restriction. The important quality of an ID is that each ID should be associated with a single element; a single element can validly have multiple IDs.

**classes and attributes **
: Aspects of the element that are useful for identifying the element, but are not generally unique to elements within a document, should be mapped to the "class" or "attribute" features depending on if they’re something equivalent to a "label" (a string by itself) or a "property" (a name/value pair)
**pseudo-classes and pseudo-elements **
: If any elements match any pseudo-classes or have any pseudo-elements, that must be explicitly defined.

<a id="issue-772ec278"></a> Some pseudo-classes are *syntactical*, like [:has()](#has-pseudo) and [:is()](#matches-pseudo), and thus should always work. Need to indicate that somewhere. Probably the structural pseudos always work whenever the child list is ordered.

In addition to the above, the document language must define how to match strings originating from the document, if it differs from Selector’s default; see [§ 3.7 Characters and case sensitivity](#case-sensitive) for details.

<a id="example-37a1ab6b"></a> For example, JSONSelect is a library that uses selectors to extract information from JSON documents.

- The "elements" of the JSON document are each array, object, boolean, string, number, or null. The array and object elements have their contents as children.
- Each element’s type is its JS type name: "array", "object", etc.
- Children of an object have their key as a class.
- Children of an array match the [:first-child](#first-child-pseudo), [:nth-child()](#nth-child-pseudo), etc pseudo-classes.
- The root object matches [:root](#root-pseudo).
- It additionally defines :val() and :contains() pseudo-classes, for matching boolean/number/string elements with a particular value or which contain a particular substring.

This structure is sufficient to allow powerful, compact querying of JSON documents with selectors.

## <a id="compat"></a> Appendix B: Obsolete but Required `-webkit-` Parsing Quirks for Web Compat

*This appendix is normative.*

Due to legacy Web-compat constraints, user agents expecting to parse Web documents must support the following features:

-
:-webkit-autofill must be treated as a [legacy selector alias](#legacy-selector-alias) of [:autofill](#selectordef-autofill).
-
All other [pseudo-elements](#pseudo-element) whose names begin with the string “-webkit-” (matched ASCII case-insensitively) and that are not functional notations must be treated as valid at parse time. (That is, ::-webkit-asdf is valid at parse time, but ::-webkit-jkl() is not.) If they’re not otherwise recognized and supported, they must be treated as matching nothing, and are *<a id="unknown--webkit--pseudo-elements"></a>unknown -webkit- pseudo-elements*.
[Unknown -webkit- pseudo-elements](#unknown--webkit--pseudo-elements) must be serialized in ASCII lowercase.
What’s this quirk about?
Selectors have long had a behavior where a single unknown/invalid selector invalidates the entire selector list (rather than just invalidating the one complex selector it finds itself in). This is generally considered a legacy mistake by the WG, but can’t be fixed at this point, as too many stylesheets depend on this behavior, intentionally or not.
One aspect of this is that use of vendor-specific selectors invalidates the entire selector in other user agents that don’t recognize them, and takes the entire style rule down with it. This has been used intentionally in the past—​in the severely-not-recommended practice of hiding style rules from some browsers by making them invalid in every other browser—​and unintentionally, with people styling an element and also applying those styles to a vendor-specific pseudo-element (such as the various `input`-related pseudos some browsers expose), not realizing that this hides the entire rule from other browsers.
In addition to this more general reasoning, WebKit-derived user agents, such as Safari or Chrome, have an additional quirk related to their vendor-prefixed pseudo-elements, where any ::-webkit--prefixed selectors are considered valid at parse time. (This is probably a leftover quirk of an early CSS feature, since dropped, that intentionally treated all possible pseudo-elements as valid at parse time, in anticipation of a feature letting authors define their own pseudo-elements.)
Similar to other legacy quirks, such as those documented in [[QUIRKS]](#biblio-quirks), this particular vendor-specific oddity has become common enough that other user agents are seeing sites breaking due to them depending on it, accidentally or not. As such, since the quirk is in practical terms *required* to render the modern web correctly, specifying it and requiring it for all user agents ensures that today’s web pages are more likely to be correctly rendered in user agents both current and future.
As usual with quirks, however, webpages intentionally relying on this will be met with shaming and derision from members of the CSSWG, and all right-thinking web developers.

## <a id="visited-privacy"></a> Appendix C: Example Privacy-Preserving [:visited](#visited-pseudo) Restrictions

Previous attempts to protect user privacy in [:visited](#visited-pseudo) involved complex restrictions and behaviors to "lie" about whether the link match :visited or [:link](#link-pseudo), to reduce the chance that a hostile site could observe what unrelated sites a user had visited while still allowing :visited to work in all cases and help the user know what links they’d already clicked. This is ultimately an arms race that can’t be won; there are multiple documented ways to still extract a user’s browsing history even with these mitigations.

This section describes an approach first developed and documented at https://github.com/explainers-by-googlers/Partitioning-visited-links-history, that partitions a user’s browsing history information, to allow [:visited](#visited-pseudo) to only match links corresponding to navigations that the site’s origin could have observed on its own. With this, :visited can be treated as a normal pseudo-class, without any of the complex mitigations described above, as it doesn’t expose any information not already theoretically available to the site, while still preserving as much of the *usefulness* of :visited as possible for the user.

1.
Let *visited history* be a set containing tuples of three pieces of information:
  -
  a visited URL
  -
  an origin for the site that started a navigation
  -
  a site for the top-level site containing the frame that started the navigation. (This will often be the same as the previous, but can differ if the user clicks a link in a iframe, for example.)
2.
Whenever a navigation is triggered *from within a page*—​e.g., from the user clicking a link, or a script on the page initiating a navigation—​add an entry to *visited history* recording the navigation’s destination URL, the origin of the page containing the link or script, and the (schemeful) site of the top-level site containing that page (which might be the same site as the previous origin).
Note: This allows a site to see [:visited](#visited-pseudo) information for links that the user has clicked from anywhere in that site’s origin. In other words, any `A -> B` navigation where the site is A.
Additionally, add an entry to *visited history* recording the destination’s URL, the *destination’s* origin, and the *destination’s* site. Do this only for navigations from top-level frames or iframes which are same-origin with their top-level frame.
Note: This allows for a site to see [:visited](#visited-pseudo) information about its own pages (which is already observable by the site) regardless of what site initiated the navigation to that page. In other words, any `A -> B` navigation where the site is B.
Note: Notably, direct navigations triggered by the *user agent’s* UI, such as typing into the address bar, clicking on bookmarks, or dragging a link from another program into the page, *do not* add a *visited history* entry. These can, of course, still add to the browser’s record of visited sites that it uses for other purposes, such as suggesting URLs as the user types into the URL bar.
3.
When determining if a link element should match [:link](#link-pseudo) or [:visited](#visited-pseudo), only allow it to match :visited if the link’s destination, the origin of the page containing the link, and the top-level site containing the link match a tuple in *visited history*.

The inclusion of both page origin and top-level site prevents several possible privacy attacks, such as:

-
If history entries were *only* keyed by the starting site’s URL, a tracking site could be embedded in a hidden iframe on multiple sites which triggers a navigation to a unique URL for a user on the first visit, and then uses many such links on subsequent visits to see which one had been visited, effectively becoming a new "third-party cookie" identifying the user across the web. By keying the history entry with the top-level site, this information can’t be shared across different sites.
-
If history entires were *only* keyed by the top-level site’s URL, a hostile iframe, perhaps included in a page as part of an advertisement, could observe what sites were visited from the top-level site. By keying the history entry with the link’s own site, the top-level site’s information can’t "leak" into cross-origin iframes.

## <a id="changes"></a>18. Changes

### <a id="changes-2026-01"></a>18.1. Changes since the 22 January 2026 Working Draft

Significant changes since the 22 January 2026 Working Draft:

-
Updated HTML5 references to point to HTML LS instead
-
Fixed explanation of HTML’s case-matching rules. Defined the default matching of document-supplied strings. (Issue 2259)
-
Used infra terminology for ascii case insensitivity ( Issue13427)
-
Responded to review feedback on BCP 47 language codes (Issue 13647)
-
Added example showing the difference between general case-insensitivity and strict ASCII case insensitivity (Issue 13645)
-
Defined language range as BCP 47 extended language range (Issue 13646)
-
Changed introductory :lang example from (zh, "*-hant") to (sr, "*-Cyrl") (Issue 13644)
-
Moved the column combinator to Selectors 5
-
Clarified that language codes must be well-formed to match (Issue 8720)
-
Reworded constraints on language ranges (Issue 8720)
-
Clarified that :picture-in-picture applies to the embedder element, not the PiP display (Issue 13720)
-
Renamed <an+b> to <a-n-plus-b> (Issue 9473)
-
Added CanIUse panels (Issue 1193)
-
Forbade non-valid tags/ranges from ever matching (Issue 3022)

### <a id="changes-2022-11"></a>18.2. Changes since the 11 November 2022 Working Draft

Significant changes since the 11 November 2022 Working Draft:

-
Defined language range as BCP 47 extended language range (Issue 13646)
-
Removed the at-risk status from [:read-write](#read-write-pseudo) and [:has()](#has-pseudo)
-
Added [:popover-open](#selectordef-popover-open) pseudo-class. (Issue 8637)
-
Made [:has()](#has-pseudo) and the selector argument of [:nth-child()](#nth-child-pseudo)/[:nth-last-child()](#nth-last-child-pseudo) no longer forgiving. (Issue 7676)
-
Moved the legacy single-colon pseudo-element syntax into the grammar itself. (Issue 8122)
-
Deferred the :local-link pseudo-class to Level 5. (Issue 12799)
-
Deferred the :interest-source and :interest-target pseudo-classes to Level 5. (Issue 12799)
-
Deferred the :blank pseudo-class to Level 5. (Issue 12799)
-
Deferred the [grid-structural (column) selectors](https://drafts.csswg.org/selectors-4/selectors-5#grid-structural-selectors) to Level 5. (Issue 12799)
-
Deferred the [time-dimensional pseudo-classes](https://drafts.csswg.org/selectors-4/selectors-5#time-pseudos) to Level 5. (Issue 12799)

### <a id="changes-2022-05"></a>18.3. Changes since the 7 May 2022 Working Draft

Significant changes since the 7 May 2022 Working Draft:

-
Added [:open](#selectordef-open) pseudo-class. (Issue 7319, Issue 11039)
-
Disallowed [pseudo-elements](#pseudo-element) from [:has()](#has-pseudo) unless explicitly allowed by the pseudo-element’s definition. (Issue 7463)
-
Disallowed nesting of [:has()](#has-pseudo). (Issue 7344)
-
Defined matching of ::lang("") and of elements not tagged with a language. (Issue 6915)
-
Untangled the concepts of "scoped" and "relative" selectors completely. (Issue 6399)
  -
  Removed "absolutize a selector" as well, and just defined relative selector matching in terms of the anchoring element.
-
Reverted compound selector limitation on [:nth-child()](#nth-child-pseudo). (Issue 3760)
-
Defined :-webkit-autofill [legacy selector alias](#legacy-selector-alias). (Issue 7474)

### <a id="changes-2018-11"></a>18.4. Changes since the 21 November 2018 Working Draft

Significant changes since the 21 November 2018 Working Draft:

- Removed the Selector profiles, marked [:has()](#has-pseudo) as optional and at-risk instead. (Issue 3925)
- Added [§ 3.6.4 Sub-pseudo-elements](#sub-pseudo-elements) to define [sub-pseudo-elements](#sub-pseudo-element) and related terminology.
- Added [:defined](#defined-pseudo). (Issue 2258)
- Added [:modal](#selectordef-modal). (Issue 6965)
- Added [:fullscreen](#selectordef-fullscreen) and [:picture-in-picture](#selectordef-picture-in-picture). (Issue 3796)
- Added [:seeking](#selectordef-seeking), [:buffering](#selectordef-buffering), and [:stalled](#selectordef-stalled) media playback state pseudo-classes. (Issue 3821)
- Added [:muted](#selectordef-muted) and [:volume-locked](#selectordef-volume-locked) sound state pseudo-classes. (Issue 3821 and Issue 3933)
- Added [:autofill](#selectordef-autofill). (Issue 5775)
- Added [:user-valid](#user-valid-pseudo). (Discussion)
- Defined [:is()](#matches-pseudo), [:where()](#where-pseudo), [:has()](#has-pseudo), [:nth-child()](#nth-child-pseudo), and [:nth-last-child()](#nth-last-child-pseudo) to not be themselves invalidated when containing an invalid selector. (Issue 3264)
- Limited selectors in [:nth-child()](#nth-child-pseudo) and [:nth-last-child()](#nth-last-child-pseudo) to [compound selectors](#compound) for now. (Issue 3760)
- Clarified case-sensitive string matching by referencing string identity as defined in [[INFRA]](#biblio-infra).
- Clarified that UA-provided placeholder text still triggers [:placeholder-shown](#placeholder-shown-pseudo).
- Rewrote [:focus-visible](#focus-visible-pseudo) definition for clarity.
- Switched reminder note in the grammar section to normative text describing the requirement of whitespace between [<compound-selector>](#typedef-compound-selector)s when a [<combinator>](#typedef-combinator) token is missing.

### <a id="changes-2018-02"></a>18.5. Changes since the 2 February 2018 Working Draft

Significant changes since the 2 February 2018 Working Draft:

- Named the zero-specificity selector to [:where()](#where-pseudo). (Issue 2143)
- Renamed [:matches()](#selectordef-matches) to [:is()](#matches-pseudo). (Issue 3258)
- Redefined [:empty](#empty-pseudo) to ignore white-space–only nodes. (Issue 1967)
- Redefined :blank to represent empty user input, rather than empty elements. (Issue 1283)
- Changed the specificity of [:is()](#matches-pseudo), [:has()](#has-pseudo), and [:nth-child()](#nth-child-pseudo) to not depend on which selector argument matched. (Issue 1027)
- Dropped the :drop() pseudo-classes since HTML dropped the related feature. (Issue 2257)
- Added the case-sensitive flag `s` to the attribute selector. (Issue 2101)
- Added further guidance on [:focus-visible](#focus-visible-pseudo).
- Added [Appendix B: Obsolete but Required -webkit- Parsing Quirks for Web Compat](#compat) defining ::-webkit- pseudo-element parsing quirk. (Issue 3051)
- Rewrote grammar rules about where white space is allowed for clarity. (See [§ 16 Grammar](#grammar).)

### <a id="changes-2013"></a>18.6. Changes since the 2 May 2013 Working Draft

Significant changes since the 2 May 2013 Working Draft include:

- Added the :target-within, [:focus-within](#focus-within-pseudo), [:focus-visible](#focus-visible-pseudo), [:playing](#selectordef-playing), and [:paused](#selectordef-paused) pseudo-classes.
- Added a zero-specificity [:matches()](#selectordef-matches)-type pseudo-class, with name TBD.
- Replaced subject indicator (!) feature with [:has()](#has-pseudo).
- Replaced the :nth-match() and :nth-last-match() selectors with :nth-child(… of *selector*) and :nth-last-child(… of *selector*).
- Changed the :active-drop-target, :valid-drop-target, :invalid-drop-target with :drop().
- Sketched out an empty-or-whitespace-only selector for discussion (See open issue.)
- Renamed :user-error to [:user-invalid](#user-invalid-pseudo). (See Discussion)
- Renamed :nth-column()/:nth-last-column() to :nth-col()/:nth-last-col() to avoid naming confusion with a potential ::column pseudo-class.
- Changed the non-functional form of the :local-link pseudo-class to account for fragment URLs.
- Removed the functional form of the `:local-link()` pseudo-class and reference combinator for lack of interest.
- Rewrote selectors grammar using the CSS Value Definition Syntax.
- Split out [relative selectors](#relative-selector) from [scoped selectors](#scoped-selector), as these are different concepts that can be independently invoked.
- Moved definition of [<a-n-plus-b>](css-syntax-3.md#anb-production) microsyntax to CSS Syntax.
<a id="issue-fb60ae39"></a> Semantic definition should probably move back here.
- Added new sections:
  - [§ 3.2 Data Model](#data-model)
  <a id="issue-4ca3978c"></a> Need to define tree for XML.
  - [§ 17 API Hooks](#api-hooks)
    - Note that earlier versions of this section defined a section on *<a id="evaluating-selectors"></a>evaluating a selector*, but that section is no longer present. Specifications referencing that section should instead reference the algorithm to [match a selector against a tree](#match-a-selector-against-a-tree).
- Removed restriction on combinators within [:matches()](#selectordef-matches) and [:not()](#negation-pseudo); see discussion.
- Defined [specificity](#specificity) of a [selector list](#selector-list). (Why?)
- Required quotes around [:lang()](#lang-pseudo) values involving an asterisk; only language codes which happen to be CSS identifiers can be used unquoted.

Note: The 1 February 2018 draft included an inadvertent commit of unfinished work; 2 February 2018 has reverted this commit (and fixed some links because why not).

### <a id="changes-2012"></a>18.7. Changes since the 23 August 2012 Working Draft

Significant changes since the 23 August 2012 Working Draft include:

- Added [:placeholder-shown](#placeholder-shown-pseudo) pseudo-classes.
- Released some restrictions on [:matches()](#selectordef-matches) and [:not()](#negation-pseudo).
- Defined fast and complete Selectors profiles (now called “live” and “snapshot”).
- Improved definition of [specificity](#specificity) to better handle [:matches()](#selectordef-matches).
- Updated grammar.
- Cleaned up definition of [<a-n-plus-b>](css-syntax-3.md#anb-production) notation.
- Added definition of *scope-relative* selectors, changed *scope-constrained* to scope-filtered for less confusion with scope-contained.
- The :local-link() pseudo-class now ignores trailing slashes.

### <a id="changes-2011"></a>18.8. Changes since the 29 September 2011 Working Draft

Significant changes since the 29 September 2011 Working Draft include:

- Added language variant handling per RFC 4647.
- Added scoped selectors.
- Added :user-error (now called [:user-invalid](#user-invalid-pseudo)).
- Added :valid-drop-target.
- Changed column combinator from double slash to double pipe.

### <a id="changes-level-3"></a>18.9. Changes Since Level 3

Additions since Selectors Level 3:

#### <a id="changes-conditional"></a>18.9.1. Conditional Logic

- The matches-any [:is](#matches) pseudo-class, formerly called :matches(), has been added to allow stylesheet authors to group functionally related elements, such as any element that is either hovered or focussed.
- The specificity-adjusting [:where()](#zero-matches) pseudo-class has been added to construct zero-specificity filters, which are easy to override, replacing the many ugly hacks previously required to simulate this.

#### <a id="changes-forms"></a>18.9.2. Interaction, Input and Forms

- Added the [:modal](#modal-state) pseudo-class, to select elements which exclude interaction with all other elements until dismissed.
- Added [:open](#open-state) and [:popover-open](#popover-open-state) pseudo-classes, primarily but not exclusively to use with HTML forms.

*The following selectors were previously in CSS Basic User Interface Module Level 3 (CSS3 UI) and have been moved to the main Selectors 4 specification for ease of reference.*

- Added the [:valid](#valid-pseudo) and [:invalid](#invalid-pseudo) pseudo-classes.
- Added the [:user-valid](#user-valid-pseudo) and [:user-invalid](#user-invalid-pseudo) pseudo-classes. These only match after significant user interaction.
- Added the [:required](#required-pseudo) and [:optional](#optional-pseudo) optionality pseudo-classes
- Added the [:enabled](#enabled-pseudo) and [:disabled](#disabled-pseudo) pseudo-classes.
- Added the [:read-only](#read-only-pseudo) and [:read-write](#read-write-pseudo) mutability pseudo-classes.
- Added the automatic input [:autofill](#autofill) pseudo-class.
- Added the selected-option [:checked](#checked), [:unchecked](#unchecked-pseudo) and [:indeterminate](#indeterminate-pseudo) pseudo-classes.

#### <a id="changes-hyperlinking"></a>18.9.3. Hyperlinking Support

- Added [:any-link](#the-any-link-pseudo) to match any element that acts as the source anchor of a hyperlink.

#### <a id="changes-authorial"></a>18.9.4. Clearer Expression of Authorial Intent

- Expanded the sibling-counting [:nth-child()](#the-nth-child-pseudo) and negation [:not()](#negation) pseudo-classes to accept a selector list.

#### <a id="changes-media"></a>18.9.5. Media State

- Added the :playing, :paused and :seeking [media playback state](#video-state) pseudo classes, to enable selecting on the dynamic state of video elements, functionality which previously required JavaScript.
- Added the [:buffering](#selectordef-buffering) and [:stalled](#selectordef-stalled) [media loading state](#media-loading-state) pseudo-classes, again providing functionality that previously needed JavaScript,
- Added the [:fullscreen](#fullscreen-state) and [:picture-in-picture](#pip-state) pseudo-classes.
- Added the [:muted](#selectordef-muted) and [:volume-locked](#selectordef-volume-locked) pseudo-classes for sound-producing elements.

#### <a id="changes-i18n"></a>18.9.6. Internationalization Improvements

- Expanded the content language [:lang()](#the-lang-pseudo) pseudo-class to accept wildcard matching, and comma-separated lists of language codes, for greater flexibility styling related languages.
- Added the content language writing direction [:dir()](#the-dir-pseudo) pseudo-class for convenient styling of left-to-right and right-to-left content.
- Clarified case-insensitive string matching.
- General improvements to closely align with BCP 47.

## <a id="acknowledgements"></a>19. Acknowledgements

The CSS working group would like to thank everyone who contributed to the previous Selectors specifications over the years, as those specifications formed the basis for this one. In particular, the working group would like to extend special thanks to the following for their specific contributions to Selectors Level 4: L. David Baron, Andrew Fedoniouk, Daniel Glazman, Ian Hickson, Grey Hodge, Lachlan Hunt, Anne van Kesteren, Jason Cranford Teague, Lea Verou

## <a id="privacy"></a>Privacy Considerations

- The [:visited](#visited-pseudo) pseudo-class can expose information about which sites a user has previously visited, if the UA is not careful to screen from scripting any information that would reveal which elements match it.
- The [:autofill](#selectordef-autofill) pseudo-class can expose whether a user has interacted with this form before; however the same information can be derived by observing how quickly the form is filled out.

## <a id="security"></a>Security Considerations

The [Privacy Considerations](#privacy) could also be considered to affect Security.

## <a id="references"></a>References

### <a id="normative"></a>Normative References

**<a id="biblio-css-display-4"></a>[CSS-DISPLAY-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Display Module Level 4. URL: https://drafts.csswg.org/css-display-4/
**<a id="biblio-css-position-4"></a>[CSS-POSITION-4] **
: Elika Etemad; Tab Atkins Jr.. CSS Positioned Layout Module Level 4. URL: https://drafts.csswg.org/css-position-4/
**<a id="biblio-css-pseudo-4"></a>[CSS-PSEUDO-4] **
: Elika Etemad; Alan Stearns. CSS Pseudo-Elements Module Level 4. URL: https://drafts.csswg.org/css-pseudo-4/
**<a id="biblio-css-shadow-1"></a>[CSS-SHADOW-1] **
: CSS Shadow Module Level 1. Editor's Draft. URL: https://drafts.csswg.org/css-shadow-1/
**<a id="biblio-css-text-4"></a>[CSS-TEXT-4] **
: Elika Etemad; et al. CSS Text Module Level 4. URL: https://drafts.csswg.org/css-text-4/
**<a id="biblio-css-values-4"></a>[CSS-VALUES-4] **
: Tab Atkins Jr.; Elika Etemad. [CSS Values and Units Module Level 4](css-values-4.md). URL: [https://drafts.csswg.org/css-values-4/](css-values-4.md)
**<a id="biblio-css-writing-modes-3"></a>[CSS-WRITING-MODES-3] **
: Elika Etemad; Koji Ishii. CSS Writing Modes Level 3. URL: https://drafts.csswg.org/css-writing-modes-3/
**<a id="biblio-css21"></a>[CSS21] **
: Bert Bos; et al. Cascading Style Sheets Level 2 Revision 1 (CSS 2.1) Specification. URL: https://drafts.csswg.org/css2/
**<a id="biblio-css3namespace"></a>[CSS3NAMESPACE] **
: Elika Etemad. CSS Namespaces Module Level 3. URL: https://drafts.csswg.org/css-namespaces/
**<a id="biblio-css3syn"></a>[CSS3SYN] **
: Tab Atkins Jr.; Simon Sapin. [CSS Syntax Module Level 3](css-syntax-3.md). URL: [https://drafts.csswg.org/css-syntax/](css-syntax-3.md)
**<a id="biblio-dom"></a>[DOM] **
: Anne van Kesteren. DOM Standard. Living Standard. URL: https://dom.spec.whatwg.org/
**<a id="biblio-html"></a>[HTML] **
: Anne van Kesteren; et al. HTML Standard. Living Standard. URL: https://html.spec.whatwg.org/multipage/
**<a id="biblio-infra"></a>[INFRA] **
: Anne van Kesteren; Domenic Denicola. Infra Standard. Living Standard. URL: https://infra.spec.whatwg.org/
**<a id="biblio-rfc2119"></a>[RFC2119] **
: S. Bradner. Key words for use in RFCs to Indicate Requirement Levels. March 1997. Best Current Practice. URL: https://datatracker.ietf.org/doc/html/rfc2119
**<a id="biblio-rfc4647"></a>[RFC4647] **
: A. Phillips, Ed.; M. Davis, Ed.. Matching of Language Tags. September 2006. Best Current Practice. URL: https://www.rfc-editor.org/info/rfc4647/
**<a id="biblio-rfc5646"></a>[RFC5646] **
: A. Phillips, Ed.; M. Davis, Ed.. Tags for Identifying Languages. September 2009. Best Current Practice. URL: https://www.rfc-editor.org/info/rfc5646/
**<a id="biblio-select"></a>[SELECT] **
: Tantek Çelik; et al. Selectors Level 3. URL: https://drafts.csswg.org/selectors-3/
**<a id="biblio-url"></a>[URL] **
: Anne van Kesteren. URL Standard. Living Standard. URL: https://url.spec.whatwg.org/
### <a id="informative"></a>Non-Normative References

**<a id="biblio-bcp47"></a>[BCP47] **
: Addison Phillips; Mark Davis. Tags for Identifying Languages. September 2009. Best Current Practice. URL: https://www.rfc-editor.org/info/bcp47
**<a id="biblio-css-display-3"></a>[CSS-DISPLAY-3] **
: Tab Atkins Jr.; Elika Etemad. CSS Display Module Level 3. URL: https://drafts.csswg.org/css-display/
**<a id="biblio-css-multicol-2"></a>[CSS-MULTICOL-2] **
: Florian Rivoal; Rachel Andrew. CSS Multi-column Layout Module Level 2. URL: https://drafts.csswg.org/css-multicol-2/
**<a id="biblio-css3ui"></a>[CSS3UI] **
: Tantek Çelik; Florian Rivoal. CSS Basic User Interface Module Level 3 (CSS3 UI). URL: https://drafts.csswg.org/css-ui-3/
**<a id="biblio-cssom-1"></a>[CSSOM-1] **
: Daniel Glazman; Emilio Cobos Álvarez. CSS Object Model (CSSOM). URL: https://drafts.csswg.org/cssom/
**<a id="biblio-cssstyleattr"></a>[CSSSTYLEATTR] **
: Tantek Çelik; Elika Etemad. CSS Style Attributes. URL: https://drafts.csswg.org/css-style-attr/
**<a id="biblio-fullscreen"></a>[FULLSCREEN] **
: Philip Jägenstedt. Fullscreen API Standard. Living Standard. URL: https://fullscreen.spec.whatwg.org/
**<a id="biblio-its20"></a>[ITS20] **
: David Filip; et al. Internationalization Tag Set (ITS) Version 2.0. 29 October 2013. REC. URL: https://www.w3.org/TR/its20/
**<a id="biblio-mathml"></a>[MATHML] **
: Patrick D F Ion; Robert R Miner. Mathematical Markup Language (MathML™) 1.01 Specification. 7 March 2023. REC. URL: https://www.w3.org/TR/REC-MathML/
**<a id="biblio-mathml-core"></a>[MATHML-CORE] **
: David Carlisle; Frédéric Wang. MathML Core. URL: https://w3c.github.io/mathml-core/
**<a id="biblio-picture-in-picture"></a>[PICTURE-IN-PICTURE] **
: Francois Beaufort; Simon Farre. Picture-in-Picture. URL: https://w3c.github.io/picture-in-picture/
**<a id="biblio-quirks"></a>[QUIRKS] **
: Simon Pieters. Quirks Mode Standard. Living Standard. URL: https://quirks.spec.whatwg.org/
**<a id="biblio-selectors-5"></a>[SELECTORS-5] **
: Elika Etemad; Tab Atkins Jr.. Selectors Level 5. URL: https://drafts.csswg.org/selectors-5/
**<a id="biblio-svg11"></a>[SVG11] **
: Erik Dahlström; et al. [Scalable Vector Graphics (SVG) 1.1 (Second Edition)](svg11/index.md). 16 August 2011. REC. URL: [https://www.w3.org/TR/SVG11/](svg11/index.md)
**<a id="biblio-xforms11"></a>[XFORMS11] **
: John Boyer. XForms 1.1. 20 October 2009. REC. URL: https://www.w3.org/TR/xforms11/
**<a id="biblio-xml-names"></a>[XML-NAMES] **
: Tim Bray; et al. Namespaces in XML 1.0 (Third Edition). 8 December 2009. REC. URL: https://www.w3.org/TR/xml-names/
**<a id="biblio-xml10"></a>[XML10] **
: Tim Bray; et al. Extensible Markup Language (XML) 1.0 (Fifth Edition). 26 November 2008. REC. URL: https://www.w3.org/TR/xml/
