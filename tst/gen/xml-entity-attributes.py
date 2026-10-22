"""Declared XML entities are expanded before SVG and CSS values are read."""
from pathlib import Path

from unitgen import Unit

COLOR = '#3a7d2a'
doctype = '''<!DOCTYPE svg [
<!ENTITY base "#3a7d2a">
<!ENTITY alias "&base;">
<!ENTITY chain "&alias;">
<!ENTITY size "8">
<!ENTITY path "M4 4H12V12H4Z">
<!ENTITY shape "<rect x='4' y='4' width='8' height='8' fill='&chain;'/>">
<!ENTITY target "tile">
<!ENTITY target2 "other">
<!ENTITY svgNS "http://www.w3.org/2000/svg">
]>
'''
plain = f'<rect x="4" y="4" width="8" height="8" fill="{COLOR}"/>'
rect = '<rect x="4" y="4" width="8" height="8"'
cases = [
    ('direct', rect + ' fill="&base;"/>', plain),
    ('alias', rect + ' fill="&alias;"/>', plain),
    ('nested', rect + ' fill="&chain;"/>', plain),
    ('style', rect + ' style="fill:&chain;"/>', plain),
    ('stylesheet', '<style>.entity-color{fill:&chain;}</style>' + rect + ' class="entity-color"/>', plain),
    ('custom-value', rect + ' style="--paint:&alias;;fill:var(--paint)"/>', plain),
    ('geometry', '<rect x="4" y="4" width="&size;" height="&size;" fill="&chain;"/>', plain),
    ('path-data', '<path d="&path;" fill="&chain;"/>', plain),
    ('content', '&shape;', plain),
    ('href', '<defs><rect id="tile" x="4" y="4" width="8" height="8" fill="&chain;"/></defs><use href="#&target;"/>', plain),
    ('id', '<defs><rect id="&target2;" x="4" y="4" width="8" height="8" fill="&chain;"/></defs><use href="#other"/>', plain),
    ('namespace', '<e:rect xmlns:e="&svgNS;" x="4" y="4" width="8" height="8" fill="&chain;"/>', plain),
]
unit = Unit('xml-entity-attributes', 64, 48,
            'Internal declared entities and nested aliases are expanded in attributes, style text, markup, IDs and namespace values. Expected shapes have literal geometry and color.', ground=False)
for i, (name, test, expected) in enumerate(cases):
    x, y = i % 4 * 16, i // 4 * 16
    unit.cell(name, (x, y, 16, 16),
              f'<g transform="translate({x} {y})">{test}</g>',
              f'<g transform="translate({x} {y})">{expected}</g>')
unit.write()
path = Path(__file__).resolve().parents[1] / 'unit/xml-entity-attributes.svg'
path.write_text(doctype + path.read_text())
