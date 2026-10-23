"""Extreme finite host transforms must not interrupt the canvas paint.

Every foreground point is outside the8px viewport after eight finite
1e19 scales. The root's lime canvas background therefore independently
defines every pixel. The offscreen pattern also contains vector effects,
a non-scaling stroke and a marker; their much smaller tile coordinates
must recover when the host inverse loses representable determinant area.
"""
from unitgen import Unit


transform = ' '.join(['scale(1e19)'] * 8)
unit = Unit('vector-effect-host-range', 8, 8,
            'A very large finite root transform puts all foreground outside the viewport. Its offscreen pattern has a tiny content map, a non-scaling stroke, a marker and non-rotation. Numerical recovery must leave the independent root canvas background intact.',
            root=f'transform="{transform}" style="background:lime"', plain_root='', ground=False)
unit.common('''<defs>
<marker id="m" markerUnits="userSpaceOnUse" markerWidth="1" markerHeight="1"><rect width="1" height="1" fill="red"/></marker>
<pattern id="p" patternUnits="userSpaceOnUse" width="1" height="1" viewBox="0 0 1e38 1e38">
<path d="M0 0H1" fill="none" stroke="red" vector-effect="non-scaling-stroke" marker-end="url(#m)"/>
<rect width="1" height="1" fill="red" vector-effect="non-rotation"/>
</pattern></defs>''', '')
unit.cell('offscreen-pattern', (0, 0, 8, 8),
          '<rect x="1" y="1" width="1" height="1" fill="url(#p)"/>',
          '<rect width="8" height="8" fill="lime"/>', tolerance=0)
unit.write()
