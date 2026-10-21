"""Internal DTD entities can refer to other internal entities."""

from pathlib import Path
from unitgen import Unit


unit = Unit(
    "xml-nested-entities", 64, 24,
    "XML internal general entities are expanded recursively in SVG "
    "attributes, including paint properties inside style attributes.",
)
unit.cell(
    "direct", (0, 0, 32, 24),
    '<rect x="4" y="4" width="20" height="16" fill="&base;"/>',
    '<rect x="4" y="4" width="20" height="16" fill="#caca82"/>',
)
unit.cell(
    "nested-style", (32, 0, 32, 24),
    '<rect x="36" y="4" width="20" height="16" style="fill:&shade;"/>',
    '<rect x="36" y="4" width="20" height="16" fill="#caca82"/>',
)
unit.write()

path = Path(__file__).resolve().parents[1] / "unit/xml-nested-entities.svg"
path.write_text(
    '<!DOCTYPE svg [<!ENTITY base "#caca82">'
    '<!ENTITY shade "&base;">]>\n' + path.read_text()
)
