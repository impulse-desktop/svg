"""pattern-template-style: the style a pattern's content cloned from a
template inherits, as Inkscape's stock patterns are written (D44)."""
from unitgen import Unit

unit = Unit("pattern-template-style", 38, 14, """Each lime square fills its
black outline, and nothing red shows. A pattern with nothing but an href
draws its template's content, which inherits from the template where it
is, as every renderer has it, not from the pattern: a fill in the
template's style, as Inkscape writes its stock patterns, or on a group
around it, colours the content; one on the pattern does not.""")
unit.common('<defs><pattern id="stock" patternUnits="userSpaceOnUse" width="10" height="10" style="fill:#00ff00"><rect width="10" height="10"/></pattern>'
            '<g fill="lime"><pattern id="grouped" patternUnits="userSpaceOnUse" width="10" height="10"><rect width="10" height="10"/></pattern></g>'
            '<pattern id="link" href="#stock" patternTransform="scale(0.5)"/>'
            '<pattern id="link-red" href="#stock" style="fill:red"/>'
            '<g fill="red"><pattern id="link-grouped" href="#grouped"/></g></defs>', "")
for k, (name, paint) in enumerate((("stock", "link"), ("pattern-style", "link-red"), ("template-context", "link-grouped"))):
    x = 2 + 12 * k
    plain = f'<rect x="{x}" y="2" width="10" height="10" fill="lime"/>'
    unit.cell(name, (x - 1, 1, 12, 12), f'<rect x="{x}" y="2" width="10" height="10" fill="url(#{paint})"/>', plain, plain.replace(' fill="lime"', ""))
unit.write()
