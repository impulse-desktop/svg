"""A join wholly outside the canvas has no visible stroke shape."""
from unitgen import Unit


unit = Unit("stroke-offscreen-turn", 64, 64,
            "A distant offscreen turn leaves two thin legs in the canvas. "
            "An ordinary offscreen detour has the same visible shape.")
unit.cell(
    "extreme", (0, 0, 32, 64),
    '<path d="M4 16 L8 -1e8 L20 56" fill="none" stroke="black" stroke-width="2"/>',
    '<rect x="3" width="2" height="16" fill="black"/><rect x="19" width="2" height="56" fill="black"/>',
)
unit.cell(
    "ordinary", (32, 0, 32, 64),
    '<path d="M36 16 L36 -1000 L52 -1000 L52 56" fill="none" stroke="black" stroke-width="2"/>',
    '<rect x="35" width="2" height="16" fill="black"/><rect x="51" width="2" height="56" fill="black"/>',
)
unit.write()
