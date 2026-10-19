"""stroke-tight-ends: wide strokes that end where their curve turns tighter
than their half width, against the exact sweep of their normals."""
import base64
import zlib

from unitgen import Unit, png

# The exact stroke of each cell, its normals swept along the curve with
# butt ends, 32 by 32 samples a pixel: alpha bytes, compressed.
EXACT = {'dash-end': 'eNrt1q9Lg1EUxvF3GwiyhYkiMotBlpZsM2k2Cf5oik1WxLAitoFdWFKGWASDRhFhSbCYtAzEtCCy5A825Z3wCIrodO9Jz0HE5/sPfOBezj03CJRSSimllPqXDSTcicRZdcjbKAI3E77E8COAl7WYp7GNt476/YiR8N1APe9mbOKjcNXpvJJ3+Oww7WIs4GvXYx7GcYeB52U+kQ7xrb0U25jDj2o5n+HoqLnINa7QrUovkRhE9y6yPGMqwsDDPM1YR2Q7fSRjP9rA7VKcYlzC6oRBxFqmUWYYGZPANMMYt40Mw5g1iQblyldM45RibJjGLsXYMo0SxTgwjQLFqJrGDMU4N41JilEzDc7/oW4aoxSj4T/mwb1pcBbIk2lwlnrbNHoohv3sBn/GUEoppZRSSin1u70CklhF1g==', 'start-tightening': 'eNrt1r8KQWEYx/FXkSR/kiK7ySAhZXUFBmWzmYTBIDKwGAwGcgHEbHYLFhuXcAqdFCfTeZU6dDhnex6Dfp8b+HZ63/M+jxAAANyixe5sMZ92SnEHTyDQ2OrSoCzLPvKCu6NKs9uqQPs16b20cKh56RKVu7R27oeIEnVd2moQNfIX24TiEeyRFt2B2EWOhIcucqplo016ezNWkbNfsEd61L/6d0QNkr8nqdNHY8DwLCbNkUtIsEeGPA988vhOXMNMYyqhvBojtlmYNRJahK3haq61Z2PMOtb91Z2U9xj39lDYTH6wozixpgEAAAAAAAAAAAAAAPyZBxDiaAI=', 'end-tight': 'eNrt1i9Lw2EUxfGfDgRd2YphlgVfgWHBoi5qGqiYBOuKaW1N0CyYXBAxiMGsCCZBg80iaNIkA/HP2MQNPIJg2++mcwbi+b6BT3i49z5J4pz7943Nb53cv38JhdnDNn6SCTNX+E0k5A8AsVF6gNqodKA2lnpQG+VPqI3iM9RG5hJyowa5MdHSGw3IjWJXb2xDbmRf9cYq9Map3sh19cYy9EZjAMZdf6NMJMb7E3gp8YyFFAPtFZpRR2p7eZJxlG7gaW2YYtwg6oxBDHVCY4dhFEICFYYxHRsFyrcqJJqUJ18PjQuKsRka+xRjNzQ2KMZxaFQpxnloLFKM69CYoxi3oTFFMR5DY5JiNPVjnryFBueAfITGKMXohcYIxYjXbvJnDOecc84555xzzjk3gL4BDLketQ=='}

unit = Unit("stroke-tight-ends", 300, 100, """Each black drawing is the exact
stroke of its curve, the union of its normal segments, drawn plainly beside
nothing else. Where a stroke ends in a curve tighter than its half width,
the inner ends of the normals near the end run back, and behind the end's
normal a wedge by its inner corner is left out of the sweep: a dash ending
in a tight turn, an open path starting where its curve tightens past the
half width a little later, an open path ending in a tight turn. Within 8
levels, the exact model's own sampling and the flattening's tolerance; the
second within 32 (D1): where the inner offset turns back, the sectors its
points sweep fan out from nearly one point, and their edges cross too
densely within a pixel for the rasterizer's bands to tell their order.""")
# Each cell's tolerance, in levels.
tolerances = {"dash-end": 8, "start-tightening": 32, "end-tight": 8}
cells = [
    ("dash-end", '<path d="M 40 80 C 40 55 45 50 50 50 C 55 50 60 55 60 80" stroke-width="30" stroke-dasharray="28 200" transform="translate(-5 -15)"/>'),
    ("start-tightening", '<path d="M 128.92 313.55 C 132.55 309.89 136.53 309.70 146.47 314.58" stroke-width="19" transform="translate(-100 -290)"/>'),
    ("end-tight", '<path d="M 40 80 C 40 55 45 50 50 50" stroke-width="30" transform="translate(-10 -25)"/>'),
]
for k, (name, path) in enumerate(cells):
    alpha = zlib.decompress(base64.b64decode(EXACT[name]))
    rows = [[(0, 0, 0, alpha[y * 100 + x] / 255) for x in range(100)] for y in range(100)]
    unit.cell(name, (100 * k, 0, 100, 100), f'<g transform="translate({100 * k} 0)" fill="none" stroke="black">{path}</g>',
              f'<image x="{100 * k}" width="100" height="100" image-rendering="pixelated" href="{png(rows)}"/>', tolerance=tolerances[name])
unit.write()
