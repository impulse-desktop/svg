# CSS Values and Units Module Level 4

## Lengths and percentages: css-length-units, lengths (exists)

- [x] css-values-4.md#length-value: a length is a dimension; zero lengths may omit the unit and be written as the bare number 0
- [x] css-values-4.md#numeric-ranges: a property may restrict a length (or any numeric value) to a range; a value outside it makes the declaration invalid and ignored
- [x] css-values-4.md#absolute-length: the absolute units are cm, mm, Q, in, pt, pc, px, fixed to each other: 1in = 2.54cm = 96px, 1pc = 1in/6, 1pt = 1in/72, 1Q = 1cm/40
- [x] css-values-4.md#absolute-length: px is the canonical absolute unit; unit identifiers are ASCII case-insensitive (1Q and 1q are the same)
- [x] css-values-4.md#length-value: a length's computed value is resolved to an absolute length; its specific unit is not distinguished afterward
- [x] css-values-4.md#em: em equals the computed font-size of the element it is used on; rem equals em computed on the root element
- [x] css-values-4.md#font-relative-lengths: ex, cap, ch, ic and lh (and their root r-prefixed forms) are font metrics; lacking real metrics they fall back to defined multiples of em (ex and ch to 0.5em, cap, ic and lh to 1em) (D29)
- [x] css-values-4.md#viewport-relative-lengths: vw, vh, vi, vb are 1% of the initial containing block's width, height, inline-axis size and block-axis size; vmin and vmax are the smaller and larger of vw/vh (D31)
- [x] css-values-4.md#compat: units related by a fixed multiplicative factor (all absolute units together, all angle units together) are compatible and convertible to one canonical unit
- [x] css-values-4.md#percentages: a percentage is a number followed by %, always resolved against a reference quantity that each property defines on its own
- [x] css-values-4.md#mixed-percentages: a length-percentage value is a length or a percentage that resolves to a length; percentages combine with the matching dimension type in calc() but not with plain numbers

## Numbers and integers: css-calc-contexts, number-syntax (exists)

- [x] css-values-4.md#integer-value: an integer is one or more decimal digits, optionally preceded by + or -
- [x] css-values-4.md#css-round-to-the-nearest-integer: rounding to the nearest integer rounds a value of exactly .5 toward positive infinity (1.5 to 2, -1.5 to -1)
- [x] css-values-4.md#number-value: a number is an integer, or digits with a decimal point and at least one further digit, optionally followed by e/E and an integer exponent, optionally signed
- [x] css-values-4.md#zero-value: the literal number token 0 is distinct from any expression that merely evaluates to 0, such as calc(0)
- [x] css-values-4.md#numeric-ranges: a numeric type's allowed range is [-infinity, infinity] unless a property or the type notation states otherwise
- [x] css-values-4.md#numeric-types: a value that exceeds the implementation's supported precision or range is converted to the closest value it can support

## Angle units: transform-css-units (exists)

- [x] css-values-4.md#deg: deg is degrees, 360 to a full circle
- [x] css-values-4.md#grad: grad is gradians, 400 to a full circle
- [x] css-values-4.md#rad: rad is radians, 2*pi to a full circle
- [x] css-values-4.md#turn: turn is turns, 1 to a full circle; deg is the canonical angle unit and all angle units are mutually compatible
- [x] css-values-4.md#angles: a bare unitless 0 is not generally valid where an angle is expected, only where a specific property grammar says otherwise

## calc() and the math functions: css-calc-math, css-calc-contexts, css-calc-edges

- [x] css-values-4.md#calc-func: calc() evaluates +, -, *, / and parentheses with standard precedence (* and / bind tighter than + and -, otherwise left to right)
- [x] css-values-4.md#comp-func: min() and max() take one or more comma-separated calculations and return the smallest or largest; a mismatched type among the arguments makes the function invalid
- [x] css-values-4.md#funcdef-clamp: clamp(MIN, VAL, MAX) is max(MIN, min(VAL, MAX)); the minimum wins over the maximum when they are in the wrong order
- [x] css-values-4.md#valdef-clamp-none: the keyword none in place of clamp()'s MIN or MAX leaves that side unclamped
- [x] css-values-4.md#round-func: round(strategy, A, B) rounds A to the nearest integer multiple of B; the default strategy is nearest, which ties toward the multiple further from zero in the positive direction (toward +infinity)
- [x] css-values-4.md#round-func: round() strategies up and down choose the multiple of B above or below A; to-zero chooses whichever multiple is closer to zero; B defaults to 1 when A is a number and is otherwise required
- [x] css-values-4.md#round-func: mod(A, B) and rem(A, B) both return A shifted by an integer multiple of B; mod()'s result always shares B's sign (or is zero), rem()'s result always shares A's sign (or is zero)
- [x] css-values-4.md#trig-funcs: sin(), cos() and tan() take a number or angle, interpreting a bare number as radians, and return a number; asin(), acos() and atan() are their inverses, returning an angle
- [x] css-values-4.md#funcdef-atan: asin() and atan() return an angle normalized to [-90deg, 90deg], acos() to [0deg, 180deg]
- [x] css-values-4.md#funcdef-atan2: atan2(Y, X) returns the angle of the point (X, Y) from the positive X axis, normalized to (-180deg, 180deg]
- [x] css-values-4.md#exponent-funcs: pow(A, B), sqrt(A), hypot(A, ...) and log(A, B) and exp(A) compute exponentiation, square root, N-dimensional hypotenuse, logarithm (base e by default) and e to the power A
- [x] css-values-4.md#sign-funcs: abs(A) returns A's magnitude with A's type; sign(A) returns -1, 0 or 1 as a number according to A's sign, computed after A is fully resolved
- [x] css-values-4.md#calc-constants: the keywords e and pi are numbers usable only inside a calculation, giving the mathematical constants
- [x] css-values-4.md#calc-error-constants: the keywords infinity, -infinity and NaN are numbers usable only inside a calculation, giving +infinity, -infinity and NaN
- [x] css-values-4.md#calc-type-checking: a percentage used where it would resolve against a plain number (such as opacity) cannot be combined with a number by calc(); opacity: calc(.25 + 25%) is invalid
- [x] css-values-4.md#calc-type-checking: an unquoted unitless 0 does not count as a length inside a math function; calc(0 + 5px) is invalid even though both 0 and 5px are valid alone
- [x] css-values-4.md#calc-ieee: within a calculation tree, operations follow IEEE-754 zero sign, infinity and NaN rules; dividing by zero gives a signed infinity, 0/0 and similar give NaN
- [x] css-values-4.md#calc-ieee: at the top level (a math function not nested in another), a signed zero becomes unsigned, an infinity is clamped to the allowed range for its context, and a NaN becomes zero
- [x] css-values-4.md#calc-range: a top-level math function's result is clamped (and rounded, where the context asks for an integer) to its context's allowed range (D30)
- [x] css-values-4.md#calc-range: a nested math function inside the same calculation tree is not itself clamped or rounded; only the top-level result is
- [x] css-values-4.md#calc-computed-value: where a property does not resolve percentages at computed-value time, a math function mixing a percentage with a dimension stays a calc() expression rather than collapsing to one value

## url(): css-url-syntax

- [x] css-values-4.md#url-value: a url is written url(string) or an unquoted token, or (in @import and similar) a bare string; src(string) is equivalent to url(string) but does not get url()'s special unquoted parsing
- [x] css-values-4.md#urls: an unquoted url() token must backslash-escape any parenthesis, whitespace, or quote character it contains; a quoted url() only needs to escape its quote character and newlines
- [-] css-values-4.md#relative-urls: a relative url is resolved against the base URL of the style sheet or style attribute it is written in, not the document's own base URL (D15)
- [x] css-values-4.md#local-urls: a url whose value starts with # is a fragment-only reference, resolved against the current document's own ids regardless of any base URL
- [x] css-values-4.md#local-urls: a fragment-only url resolves to the first element in tree order whose id matches the fragment; no match means the reference fails to resolve
- [x] css-values-4.md#url-empty: url() or url("") with an empty value resolves to an invalid resource, as if it pointed at about:invalid

## CSS-wide keywords: css-wide-keywords

- [x] css-values-4.md#common-keywords: every property accepts the CSS-wide keywords (initial, inherit, unset, and others defined by the cascade) as its sole value even where its own grammar does not list them
- [x] css-values-4.md#component-types: combining a CSS-wide keyword with any other component in the same declaration makes the declaration invalid

## Notes
