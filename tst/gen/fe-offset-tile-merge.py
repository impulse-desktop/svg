"""fe-offset-tile-merge: feOffset, feTile and feMerge of CSS Filter Effects.
The unit is written by hand; this writes its expected drawing."""
from unitgen import expect

TILES = "M30 2h2v1h-2zM30 4h2v3h-2zM30 8h2v3h-2zM30 12h2v2h-2zM34 2h2v1h-2zM34 4h2v3h-2zM34 8h2v3h-2zM34 12h2v2h-2zM38 2h2v1h-2zM38 4h2v3h-2zM38 8h2v3h-2zM38 12h2v2h-2zM42 2h2v1h-2zM42 4h2v3h-2zM42 8h2v3h-2zM42 12h2v2h-2zM46 2h2v1h-2zM46 4h2v3h-2zM46 8h2v3h-2zM46 12h2v2h-2z"
merged = '<rect x="2" y="{0}" width="12" height="6" fill="#c31" fill-opacity=".6"/><rect x="2" y="{0}" width="12" height="6" fill="#3a8" fill-opacity=".5"/><rect x="2" y="{0}" width="12" height="6" fill="#25e" fill-opacity=".7"/>'
expect("fe-offset-tile-merge", [
    ("offset", (1, 1, 12, 14), '<rect x="5" y="4" width="6" height="6" fill="lime"/>'),
    ("offset-units", (13, 1, 12, 14), '<rect x="16" y="4" width="8" height="4" fill="lime"/>'),
    ("offset-zero", (25, 1, 4, 14), '<rect x="26" y="4" width="2" height="8" fill="lime"/>'),
    ("tile", (29, 1, 21, 14), f'<path fill="lime" d="{TILES}"/>'),
    ("merge", (1, 17, 14, 14), merged.format(18) + merged.format(24)),
])
