"""A very wide curved mask paint cannot delay a later opaque white paint.

All authored values are finite. The root is 256 CSS pixels wide but this
unit renders at64, exposing nonfinite lowered edges from the 3e38 stroke.
The final opaque white rectangle covers the entire mask region, independent
of the first paint's coverage. The masked lime rectangle therefore covers
every output pixel. No rasterizer output defines the expectation.
"""
from pathlib import Path

from unitgen import ROOT


name = 'raster-nan-sort.64x64'
Path(ROOT, f'tst/unit/{name}.svg').write_text('''<svg xmlns="http://www.w3.org/2000/svg" width="256" height="256" viewBox="0 0 26 26">
<defs><mask id="m">
<path d="M0 20L7 20Q9 20 9 18" stroke="white" fill="none" stroke-width="3e38"/>
<rect x="-100" y="-100" width="200" height="200" fill="white"/>
</mask></defs>
<rect width="26" height="26" fill="lime" mask="url(#m)"/>
</svg>
''')
Path(ROOT, f'tst/expect/{name}.svg').write_text('''<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64">
<!-- Opaque white is painted last over the whole mask; the result is solid lime. -->
<g id="covered-mask" data-box="0 0 64 64"><rect width="64" height="64" fill="lime"/></g>
</svg>
''')
