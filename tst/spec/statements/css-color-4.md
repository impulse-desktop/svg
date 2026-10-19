# CSS Color Module Level 4, color syntax, keywords, color spaces, opacity, gamut mapping

## <color> grammar, legacy vs modern syntax: color-functions, color-syntax (exists)

- [x] css-color-4.md#color-syntax: <color> = <color-base> | currentColor | <system-color>; <color-base> = <hex-color> | <color-function> | <named-color> | transparent. [color-functions#kinds color-keywords#currentcolor color-keywords#transparent color-functions#system]
- [x] css-color-4.md#color-syntax: <color-function> = rgb() | rgba() | hsl() | hsla() | hwb() | lab() | lch() | oklab() | oklch() | color(). [color-functions#functions]
- [x] css-color-4.md#color-syntax: hex colors, rgb()/rgba(), hsl()/hsla(), hwb() and named colors resolve to sRGB. [color-functions#kinds color-functions#hex-6-8 color-functions#rgb-forms color-functions#hsl-values color-functions#hwb color-functions#named]
- [x] css-color-4.md#color-syntax: only rgb()/rgba() and hsl()/hsla() support the legacy comma syntax; it is invalid for every other color function. [color-functions#legacy-only color-functions#legacy]
- [x] css-color-4.md#color-syntax-modern: modern syntax separates components by whitespace, the optional alpha by "/", allows none for any component, and lets <number> and <percentage> be freely mixed. [color-functions#modern]
- [x] css-color-4.md#color-syntax-legacy: legacy syntax separates components by commas, uses a distinct non-opaque function name (rgba/hsla) with alpha after a comma, disallows none, and requires components to be all <number> or all <percentage>, not mixed. [color-functions#legacy color-functions#legacy-invalid]
- [x] css-color-4.md#missing: an omitted alpha defaults to 100%; a missing component (none, or produced automatically) behaves as zero (0, 0%, or 0deg) for direct rendering, conversion and computation. [color-functions#missing]
- [x] css-color-4.md#hue-syntax: <hue> = <number> | <angle>; a bare number is degrees; the value normalizes to [0,360) (-540 and 540 both give 180; a non-finite value gives 0). [color-functions#hue]
- [x] css-color-4.md#alpha-syntax: <alpha-value> = <number> | <percentage>; a value outside [0,1] is clamped to that range at parsed-value time, not treated as invalid. [color-functions#alpha]

## Hex color notation: color-functions, color-syntax (exists)

- [x] css-color-4.md#hex-notation: a hex color is "#" followed by exactly 3, 4, 6 or 8 hex digits; any other digit count is invalid. [color-functions#hex-3-4 color-functions#hex-6-8 color-functions#hex-invalid]
- [x] css-color-4.md#hex-notation: 6 digits give RRGGBB with alpha fully opaque; 8 digits add AA as alpha (00 transparent, ff opaque). [color-functions#hex-6-8]
- [x] css-color-4.md#hex-notation: 3 digits give RGB, each digit duplicated to form the 6-digit value (#123 = #112233); 4 digits add A, duplicated the same way. [color-functions#hex-3-4]
- [x] css-color-4.md#hex-notation: hex digits are case-insensitive. [color-functions#hex-case]

## rgb() and rgba(): color-functions, color-syntax (exists)

- [x] css-color-4.md#rgb-functions: legacy rgb()/rgba() take 3 <percentage> or 3 <number> (not mixed), comma-separated, then an optional comma-separated alpha; modern forms take 3 of <number>|<percentage>|none then "/" and an optional alpha|none. [color-functions#rgb-forms color-functions#rgb-invalid color-functions#legacy-invalid]
- [x] css-color-4.md#rgb-functions: for r, g, b: 0% = 0.0, 100% = 255.0; for alpha: 0% = 0.0, 100% = 1.0. [color-functions#rgb-percent]
- [x] css-color-4.md#rgb-functions: component and alpha values outside their ranges are clamped at parsed-value time, not invalid. [color-functions#rgb-clamp]

## Named colors, transparent and currentcolor: color-keywords, color-functions, colors (exists)

- [x] css-color-4.md#named-colors: named colors are ASCII case-insensitive <ident>s resolving to fixed sRGB values, a superset of the SVG 1.1 named color list. [color-functions#named]
- [x] css-color-4.md#transparent-color: transparent is a named color for transparent black (rgb(0 0 0 / 0%)). [color-keywords#transparent]
- [x] css-color-4.md#currentcolor-color: currentcolor represents the used value of the color property on the same element, and is not restricted to sRGB. [color-keywords#currentcolor]
- [x] css-color-4.md#resolving-other-colors: in the color property, currentcolor's used value is the resolved inherited value; in every other property it is the used value of color on the same element. [color-keywords#currentcolor-color color-keywords#currentcolor]
- [x] css-color-4.md#resolving-other-colors: transparent's used value is transparent black; a system color keyword's declared value is itself, lowercased. [color-keywords#transparent color-functions#system]

## System and deprecated system colors: color-functions, color-syntax (exists)

- [x] css-color-4.md#css-system-colors: <system-color> defines 19 case-insensitive keywords, each a UA/OS-chosen color: AccentColor, AccentColorText, ActiveText, ButtonBorder, ButtonFace, ButtonText, Canvas, CanvasText. [color-functions#system]
- [x] css-color-4.md#css-system-colors: the remaining <system-color> keywords: Field, FieldText, GrayText, Highlight, HighlightText, LinkText, Mark, MarkText, SelectedItem, SelectedItemText, VisitedText. [color-functions#system-more]
- [x] css-color-4.md#deprecated-system-colors: 22 deprecated keywords (e.g. ActiveBorder, Background, Menu, Window, WindowText) must still be accepted as valid colors, each computing to one specific current system color. [color-functions#system-deprecated]
- [x] css-color-4.md#resolving-other-colors: a system color's computed/used value is the corresponding color in its color space, paired with the specified alpha clamped to [0,1]; a deprecated system color computes to its mapped system color the same way. [color-functions#system color-functions#system-more color-functions#system-deprecated]

## hsl() and hsla(), conversion to sRGB: color-functions, color-syntax (exists)

- [x] css-color-4.md#the-hsl-notation: modern hsl()/hsla() = hsl([<hue>|none] [<percentage>|<number>|none]{2} [/ [<alpha-value>|none]]?); legacy form is hsl(<hue>, <percentage>, <percentage>, <alpha-value>?). [color-functions#hsl-forms color-functions#legacy]
- [x] css-color-4.md#the-hsl-notation: hue 0deg is sRGB red, 120deg green, 240deg blue; saturation 100%=fully saturated, 0%=gray; lightness 50%=normal, 100%=white, 0%=black. [color-functions#hsl-values]
- [x] css-color-4.md#the-hsl-notation: a negative saturation is clamped to 0% at parsed-value time, before conversion to sRGB. [color-functions#hsl-clamp]
- [x] css-color-4.md#the-hsl-notation: saturation of 0 makes the hue component powerless. [color-functions#hsl-powerless]
- [x] css-color-4.md#hsl-to-rgb: converting hue H (deg), saturation S and lightness L (0..1): k(n) = (n + H/30) mod 12; a = S*min(L, 1-L); f(n) = L - a*max(-1, min(k(n)-3, 9-k(n), 1)); R=f(0), G=f(8), B=f(4). [color-functions#hsl-convert color-functions#hsl-clamp]

## hwb(), conversion to sRGB: color-functions

- [x] css-color-4.md#the-hwb-notation: hwb() = hwb([<hue>|none] [<percentage>|<number>|none]{2} [/ [<alpha-value>|none]]?); hue as for hsl(); whiteness/blackness 0%=none, 100%=full. [color-functions#hwb]
- [x] css-color-4.md#the-hwb-notation: hwb() has no legacy comma syntax; a comma inside it is an error. [color-functions#hwb-legacy]
- [x] css-color-4.md#the-hwb-notation: if white+black >= 100%, the result is achromatic (a shade of gray) with R=G=B = white/(white+black), and the hue is then powerless. [color-functions#hwb-gray]
- [x] css-color-4.md#the-hwb-notation: hue is normalized to [0,360); hwb() resolves to sRGB. [color-functions#hwb-hue]
- [x] css-color-4.md#hwb-to-rgb: otherwise, normalize white/black to [0,1], set rgb = hslToRgb(hue,100%,50%), then for each channel rgb[i] = rgb[i]*(1-white-black) + white. [color-functions#hwb]

## lab() and lch(): color-functions

- [x] css-color-4.md#cie-lab: Lab and LCH use a D50 white point ((0.3457,0.3585), ~5003K); LCH has the same L axis as Lab, with polar chroma C and hue H measured from the +a axis toward +b. [color-functions#lab-white]
- [x] css-color-4.md#specifying-lab-lch: lab() = lab([<percentage>|<number>|none]{3} [/ [<alpha-value>|none]]?); L percent range 0%=0..100%=100; a and b percent range -100%=-125..100%=125. [color-functions#lab]
- [x] css-color-4.md#specifying-lab-lch: L is clamped to [0,100] at parsed-value time; a and b are signed and unbounded, never clamped. [color-functions#lab-clamp]
- [x] css-color-4.md#specifying-lab-lch: lch() = lch([<percentage>|<number>|none] [<percentage>|<number>|none] [<hue>|none] [/ [<alpha-value>|none]]?); L as for lab(); C percent range 0%=0..100%=150; negative C is clamped to 0 at parsed-value time. [color-functions#lch]
- [x] css-color-4.md#specifying-lab-lch: LCH hue 0deg points along +a (purplish red), 90deg along +b (mustard yellow), 180deg along -a (greenish cyan), 270deg along -b (sky blue). [color-functions#lch-hue]
- [x] css-color-4.md#specifying-lab-lch: lab() and lch() have no legacy comma syntax; a comma inside either is an error. [color-functions#lab-legacy]
- [x] css-color-4.md#specifying-lab-lch: LCH chroma of 0 makes the hue component powerless; an L clamped to 0% or 100% displays as black or white. [color-functions#lch-powerless]
- [x] css-color-4.md#lab-to-lch: converting Lab to LCH: C = sqrt(a^2+b^2); H = atan2(b,a) unless C is at or below epsilon, in which case H is missing; L is unchanged. [color-functions#lab-lch]
- [x] css-color-4.md#lch-to-lab: converting LCH to Lab: if H is missing, H=0; a = C*cos(H); b = C*sin(H); L is unchanged. [color-functions#lab-lch]

## oklab() and oklch(): color-functions

- [x] css-color-4.md#ok-lab: Oklab and OkLCh use a D65 white point; L is in [0,1] (0%=0.0, 100%=1.0); OkLCh has the same L with polar chroma C and hue H. [color-functions#oklab-white color-functions#oklab]
- [x] css-color-4.md#specifying-oklab-oklch: oklab() = oklab([<percentage>|<number>|none]{3} [/ [<alpha-value>|none]]?); L percent 0%=0.0..100%=1.0; a and b percent -100%=-0.4..100%=0.4. [color-functions#oklab]
- [x] css-color-4.md#specifying-oklab-oklch: L is clamped to [0,1] at parsed-value time; a and b are signed and unbounded, never clamped. [color-functions#oklab-clamp]
- [x] css-color-4.md#specifying-oklab-oklch: oklch() = oklch([<percentage>|<number>|none] [<percentage>|<number>|none] [<hue>|none] [/ [<alpha-value>|none]]?); C percent 0%=0.0..100%=0.4; negative C clamped to 0 at parsed-value time. [color-functions#oklch]
- [x] css-color-4.md#specifying-oklab-oklch: OkLCh hue angles point along the same four axis directions as LCH's (0/90/180/270deg). [color-functions#oklch-hue]
- [x] css-color-4.md#specifying-oklab-oklch: oklab() and oklch() have no legacy comma syntax; a comma inside either is an error. [color-functions#oklab-legacy]
- [x] css-color-4.md#specifying-oklab-oklch: OkLCh chroma of 0 makes the hue component powerless; L at 0 or 1 displays as black or white. [color-functions#oklch-powerless]
- [x] css-color-4.md#lab-to-lch: the same chroma/hue from a/b conversion (and its inverse) is used between oklab() and oklch(). [color-functions#oklab-oklch]

## color() function grammar and validity: color-functions

- [x] css-color-4.md#color-function: color() = color(<colorspace-params> [/ [<alpha-value>|none]]?); <colorspace-params> is a <predefined-rgb> name plus 3 of <number>|<percentage>|none, or an <xyz-space> name plus the same 3 values. [color-functions#color color-functions#xyz]
- [x] css-color-4.md#color-function: <predefined-rgb> = srgb | srgb-linear | display-p3 | display-p3-linear | a98-rgb | prophoto-rgb | rec2020; <xyz-space> = xyz | xyz-d50 | xyz-d65. [color-functions#srgb color-functions#srgb-linear color-functions#display-p3 color-functions#display-p3-linear color-functions#a98-rgb color-functions#prophoto-rgb color-functions#rec2020 color-functions#xyz]
- [-] css-color-4.md#color-function: a color space name that matches none of these makes color() an invalid color, whose used value is opaque black. (D35) [color-functions#color-invalid]
- [x] css-color-4.md#color-function: out-of-range component values (below 0/0% or above 1/100%) are not invalid; they are kept for computation and only CSS-gamut-mapped at actual-value (display) time. [color-functions#color-range color-functions#gamut]
- [x] css-color-4.md#color-function: color() has no legacy comma syntax; a comma inside it is an error. [color-functions#color-invalid color-functions#legacy-only]

## Predefined RGB and XYZ color spaces: color-functions

- [x] css-color-4.md#predefined-sRGB: srgb is the same space as legacy sRGB colors; D65 white point; transfer function is the sRGB piecewise gamma (linear below 0.04045, else ((c+0.055)/1.055)^2.4). [color-functions#srgb]
- [x] css-color-4.md#predefined-sRGB-linear: srgb-linear is srgb with a unity (linear-light, no gamma) transfer function. [color-functions#srgb-linear]
- [x] css-color-4.md#predefined-display-p3: display-p3 uses wider primaries than sRGB with a D65 white point and the same transfer function as sRGB. [color-functions#display-p3]
- [x] css-color-4.md#predefined-display-p3-linear: display-p3-linear is display-p3 with a unity transfer function. [color-functions#display-p3-linear]
- [x] css-color-4.md#predefined-a98-rgb: a98-rgb has its own primaries, D65 white point, and a gamma transfer function close to (but not exactly) 1/2.2. [color-functions#a98-rgb]
- [x] css-color-4.md#predefined-prophoto-rgb: prophoto-rgb has its own wide primaries, a D50 white point, and a 1/1.8 gamma with a small linear segment near black (threshold E=16/512, linear divisor 16). [color-functions#prophoto-rgb]
- [x] css-color-4.md#predefined-rec2020: rec2020 has ultra-wide primaries, D65 white point, and a gamma-2.40 transfer function. [color-functions#rec2020]
- [x] css-color-4.md#predefined-xyz: xyz and xyz-d65 share the D65 white point; xyz-d50 uses D50; Y=1.0 is diffuse white; values are never clamped, even below 0 or above 1. [color-functions#xyz]

## Converting to and between predefined color spaces: color-functions

- [x] css-color-4.md#predefined-to-lab-oklab: converting a predefined RGB space to Lab: undo its gamma, convert linear RGB to XYZ, Bradford-adapt D65 to D50 unless already D50 (prophoto-rgb), then XYZ to Lab. [color-functions#prophoto-rgb color-functions#display-p3 color-functions#gamut]
- [x] css-color-4.md#predefined-to-lab-oklab: converting to Oklab runs the same steps but adapts D50 to D65, and only for prophoto-rgb (every other predefined space is already D65). [color-functions#prophoto-rgb color-functions#gamut]
- [x] css-color-4.md#oklab-lab-to-predefined: converting Lab or Oklab to a predefined RGB space runs the same steps backward: to XYZ, Bradford-adapt the white point if needed, XYZ to linear RGB, then apply gamma encoding. [color-functions#lab color-functions#oklab]
- [x] css-color-4.md#predefined-to-predefined: converting between two predefined RGB spaces: undo the source gamma, source linear RGB to XYZ, Bradford-adapt if the white points differ, XYZ to destination linear RGB, apply destination gamma encoding. [color-functions#display-p3 color-functions#a98-rgb color-functions#rec2020 color-functions#xyz]
- [x] css-color-4.md#color-convert-algorithm: converting col1 (src) to col2 (dest): set powerless components missing, go rectangular if polar, replace missing with zero, linearize, convert to XYZ, Bradford-adapt if white points differ. [color-functions#missing color-functions#hsl-powerless color-functions#lch-powerless color-functions#display-p3]
- [x] css-color-4.md#color-convert-algorithm: then convert XYZ to dest and re-encode, CSS-gamut-map col2 if dest is a physical output (e.g. a display), then go back to polar form if dest is cylindrical. [color-functions#gamut]
- [x] css-color-4.md#color-convert-intro: color conversion results are not clamped and may be out of gamut, or even have negative or over-range lightness; clamping happens only through CSS gamut mapping at actual display time. (D27) [color-functions#gamut color-functions#color-range]

## opacity property: opacity-values, opacity-syntax (exists)

- [x] css-color-4.md#transparency: opacity is conceptually a post-process blend: the element and its descendants render to an offscreen RGBA buffer, then that buffer is composited into the backdrop with simple alpha compositing. [opacity-values#group opacity-values#occlude]
- [x] css-color-4.md#transparency: <opacity-value> = <number>|<percentage>; initial 1; not inherited; a percentage maps linearly, 0% to 0 and 100% to 1. [opacity-values#values opacity-values#not-inherited]
- [x] css-color-4.md#transparency: opacity values outside [0,1] are preserved in the specified value but clamped to [0,1] in the computed value. [opacity-values#clamp]
- [x] css-color-4.md#transparency: opacity applies to the element and its descendants as a single unit (an opaque child occluding the background still does so), not to each descendant individually. [opacity-values#occlude opacity-values#group]

## Computed and used color values: color-functions, colors (exists)

- [x] css-color-4.md#resolving-sRGB-values: colors that resolve to sRGB (hex, rgb/rgba, hsl/hsla, hwb, named) compute to the sRGB color with the specified alpha, clamped to [0,1] and defaulting to opaque. [color-functions#alpha color-functions#rgb-forms color-functions#hex-6-8]
- [x] css-color-4.md#resolving-lab-lch-values: lab()/lch() compute to the clamped L/C/H with alpha as a plain number, defaulting to opaque. [color-functions#lab-clamp color-functions#lch]
- [x] css-color-4.md#resolving-oklab-oklch-values: oklab()/oklch() compute to the clamped L/C/H with alpha as a plain number, defaulting to opaque. [color-functions#oklab-clamp color-functions#oklch]
- [x] css-color-4.md#resolving-color-function-values: color() computes to the color in its specified space with alpha as a plain number; color(xyz ...) computes into the xyz-d65 space specifically. [color-functions#xyz color-functions#color]

## Gamut mapping to an RGB display: color-functions

- [x] css-color-4.md#gamut-mapping-intro: a color can be valid (parses successfully) yet out of gamut for a given output; out-of-gamut values are preserved through intermediate computation and mapped only for display. [color-functions#color-range color-functions#gamut]
- [x] css-color-4.md#css-gamut-mapping: a color already inside the destination gamut is returned unchanged (relative colorimetric intent); only out-of-gamut colors are CSS gamut mapped. [color-functions#gamut-inside]
- [x] css-color-4.md#css-gamut-mapping: every permitted CSS gamut-mapping algorithm reduces chroma at constant lightness and hue in OkLCh; a Lightness >= 1.0 maps to white and <= 0.0 maps to black in the destination space. [color-functions#gamut color-functions#gamut-lightness]
- [x] css-color-4.md#GMA-Binary-local-MINDE: the reference binary-search algorithm clips the current OkLCh candidate and accepts it once its deltaEOK from the unclipped candidate drops below a JND of 0.02. [color-functions#gamut]
- [x] css-color-4.md#GMA-Binary-local-MINDE: otherwise it binary-searches the chroma down until the search interval (max - min) is at or below a tolerance (epsilon) of 0.0001. [color-functions#gamut]
- [x] css-color-4.md#color-difference-OK: deltaEOK between two Oklab triplets is the Euclidean distance sqrt(dL^2 + da^2 + db^2). [color-functions#gamut]

## Notes

- css-color-4.md#css-system-colors: the concrete sRGB value of each <system-color> keyword is UA/OS-chosen, not specified by this spec; which exact color a renderer picks is ambiguous in a way that changes pixels.
- css-color-4.md#css-gamut-mapping: three gamut-mapping algorithms (Binary Search with Local MINDE, EdgeSeeker, Ray Trace) are equally conformant; only the first is extracted here as the reference formula.
- css-color-4.md#interpolation: general color interpolation (color-interpolation-method, hue strategies) backs CSS transitions and color-mix(), which this renderer never draws; the in-scope gradient spec has no "in <color-space>" syntax either.
