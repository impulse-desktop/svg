"""stroke-tight-ends: wide strokes that end where their curve turns tighter
than their half width, against the exact sweep of their normals."""
import base64
import math
import zlib

from unitgen import Unit, png

# The exact stroke of each cell, its normals swept along the curve with
# butt ends, 32 by 32 samples a pixel: alpha bytes, compressed.
EXACT = {'dash-end': 'eNrt1q9Lg1EUxvF3GwiyhYkiMotBlpZsM2k2Cf5oik1WxLAitoFdWFKGWASDRhFhSbCYtAzEtCCy5A825Z3wCIrodO9Jz0HE5/sPfOBezj03CJRSSimllPqXDSTcicRZdcjbKAI3E77E8COAl7WYp7GNt476/YiR8N1APe9mbOKjcNXpvJJ3+Oww7WIs4GvXYx7GcYeB52U+kQ7xrb0U25jDj2o5n+HoqLnINa7QrUovkRhE9y6yPGMqwsDDPM1YR2Q7fSRjP9rA7VKcYlzC6oRBxFqmUWYYGZPANMMYt40Mw5g1iQblyldM45RibJjGLsXYMo0SxTgwjQLFqJrGDMU4N41JilEzDc7/oW4aoxSj4T/mwb1pcBbIk2lwlnrbNHoohv3sBn/GUEoppZRSSin1u70CklhF1g==', 'start-tightening': 'eNrt1r8KQWEYx/FXkSR/kiK7ySAhZXUFBmWzmYTBIDKwGAwGcgHEbHYLFhuXcAqdFCfTeZU6dDhnex6Dfp8b+HZ63/M+jxAAANyixe5sMZ92SnEHTyDQ2OrSoCzLPvKCu6NKs9uqQPs16b20cKh56RKVu7R27oeIEnVd2moQNfIX24TiEeyRFt2B2EWOhIcucqplo016ezNWkbNfsEd61L/6d0QNkr8nqdNHY8DwLCbNkUtIsEeGPA988vhOXMNMYyqhvBojtlmYNRJahK3haq61Z2PMOtb91Z2U9xj39lDYTH6wozixpgEAAAAAAAAAAAAAAPyZBxDiaAI=', 'end-tight': 'eNrt1i9Lw2EUxfGfDgRd2YphlgVfgWHBoi5qGqiYBOuKaW1N0CyYXBAxiMGsCCZBg80iaNIkA/HP2MQNPIJg2++mcwbi+b6BT3i49z5J4pz7943Nb53cv38JhdnDNn6SCTNX+E0k5A8AsVF6gNqodKA2lnpQG+VPqI3iM9RG5hJyowa5MdHSGw3IjWJXb2xDbmRf9cYq9Map3sh19cYy9EZjAMZdf6NMJMb7E3gp8YyFFAPtFZpRR2p7eZJxlG7gaW2YYtwg6oxBDHVCY4dhFEICFYYxHRsFyrcqJJqUJ18PjQuKsRka+xRjNzQ2KMZxaFQpxnloLFKM69CYoxi3oTFFMR5DY5JiNPVjnryFBueAfITGKMXohcYIxYjXbvJnDOecc84555xzzjk3gL4BDLketQ==', 'pie-sector': 'eNrt2M9HwwEcxvH3d63f1g+RUpIOUySJDrEikYiIRCIiMRKRTpHIiOgSiXTI6BKRiEQiM5JIkqQoKZWi9Xtt1bF/4Inxef8Dr+vjAcuyLMuyLMuyEqf67W9Rf5W2I72Bu/9abkDmWERuQMFsVG5A+YreAF9Ib0DHid4geeBWbkDWxIvcgKL5L7kBlWt6Axp39QZO15ncgNShe7kBOZNvcgNKFmNyA6o39AY07+sNXD0XcgPSRh7lBuRNf8gNKFuKyw2o3dIb0HqoN0jqu5IbkDH6JDcgf+ZTboB3WW9A3Y7egPZjvYHbfyM3wDP+LDegcC4qN6BiVW9AQ1hv4HSeyg1IGbyTG5AdeJUbULwQkxtQta43oGlPb+B0n8uN3yE+/CA3IHfqXW5AaTAuN6BmU29Ay4HewNV7+Q/nW7r9j5ZlWZaV+P0ABPibIw=='}
# Two tiny cubics of Sketchy-SVG 08befa42d09fe5d1, its stroke 27.7 wide,
# drawn 2.5 times their size.
EXACT['waist'] = 'eNrt2N9LU2EYwPHNHzN/gCFiYl0EYkwzvFAoLSMhM3+ASwKFMqOGV14IRbILQeliENUIHUiKXgxESERDxK52YYKgBiL9wCBCpqCOpLZIPbYnp46djjvnvOx9npt6v//AB8457/O+7zEYRCKRSCQS/d+lPcwnN24DdB0nNl4BwHoTKZHgg2DjGYRGNRzkOU9n9B4a8OsGFRG7HjJAqicyrkA4qYrG6JIZ4CNZKTGrcgM+JhE/qmAOAqNHYUhn0QmTV2GAC92oVQgzzanoxogc2HiWR/A2MnbCwvStBJLF0RYCdlyFVHPky4Hgf36KbBzW7Qs/7emEY30muCCcmZS7U+Ue4c6lFAqdm3tGDh2Q3rqw/y4+kW1L1cPbh9/sUxrhjH0lvPDKCICUe1Py6bEZjw0YS/t9f4/AIWThpG1JOcehEXWfuDmxe0SAXcQFXvDCC5F6i3Ysb5kHlWw4x46KoS1Q7RyCkP14GTT6yg0k33EHQLNuTqH45XfQ6zoPkPnoA+jnPxY1EG95LQFLo1ET7WvAmDVqw8FKBLKiNswBRmOO44W/YTQ6OIxyRqOI58udZSJWjTxGDZPRx7fGp1kMC59RwvBpbaVwDiuXvjHJO3JPfNM1Wrjn+l1d4zT/7jSmQyxinGc92oYdYyO/uK1pXEI5LTRrEd5YnDPPEw0D66JvHFA3GtCuGYNqhIT3GzSmV8VwY14GOiNPrgeoF4IGfyTDjHvryH9/lPiMfXlKdPwG+n9tF94pjKsUd2Xripz4YSK5Lye2ymbkCNW939Q0FzLuE/4gKXJucJ5BWYq71uOBeQN1xoLLBpFIJPqn+wP1rHXL'
# A hook of tabler's brand-swift, its stroke 2 wide with round caps, drawn
# 20 times its size: the normals' sweep and the caps' discs.
EXACT['s-turn'] = 'eNrt2EkohGEYwPEZY4kmkjUHSiHJEmUrJieZEgfhJKSxRE5MSSImhQMNpTiMkCKKJFnGCcXBIGUrRXGwZDkw0+ThooTpXZ7h4vkfp6/vd/jme7/nfRUKiqIoiqIoiqL+qIDMstbB6eX1zc2NlZlhQ402ytWZt1el6Ocu4VvWXVNtklMgj/zRW3Dcw3x9JFKI6b8DZnv13vJC1hLwdav3kBOS14C/wxSZ/5HpFUSy1QkTRdcgWrdSSPAygUSdIkSYBaSq4CfiruQIeI7lJeJvQLZtFz4i9ArkK+ci3LcQBJxxrV/tgKqAg4iw4ox5DmMCR8CLmkmE25EGZDMNA5aAZqZxijZGmU8cTcAqyyjBG1sso+sPjEm8YWYZK3hjjGWs4Y0WlrGEN7QsYxxN2JijVgfaWGS+5oVoo5hphGCJCzf20r6DNKo4Ph+NOMLC86kNfMYQ1kSukcGIMWr5Jp+ge3mij3eCq5QmBrhnauWsnGDXCwzUPvtSw5tGaGMQciQsPLV5Ce5vgrcdrnhdeWmp2breDdvnX0+a/MT3aZ5DPxPnGR9XqHN7zOfvI+XDwVRDguSONufkh6c64PPlKjcXBSb3yuMv7/BItPMPSJQa4+HH1vZxodr/tw5ifNOLdFWl2kgVnUlRFEVRFEX9q94Ad2llpA=='

# The historical pie bitmap is the normal sweep of a quarter-circle
# cubic, not of a circle. Keep that curve as its own control; the circle's
# exact sector below is derived from its source dash length and radius.
EXACT['pie-quarter-cubic'] = EXACT.pop('pie-sector')

unit = Unit("stroke-tight-ends", 700, 100, """Each black drawing is the exact
stroke of its curve, the union of its normal segments, drawn plainly beside
nothing else. Where a stroke ends in a curve tighter than its half width,
the inner ends of the normals near the end run back, and behind the end's
normal a wedge by its inner corner is left out of the sweep: a dash ending
in a tight turn, an open path starting where its curve tightens past the
half width a little later, an open path ending in a tight turn, and a dash
of a circle as wide as its diameter, a pie chart's sector, whose normals
all meet at the centre, its half width past what the turn a step may take
bounds (the sector cut by its viewport), and a quarter-circle cubic with
the same dash, whose normals do not all meet at the centre. The circle's
dash sweeps 30/60 radians, so its clipped sector is a plain triangle;
the cubics use the sampled normal sweeps. Then two tiny cubics of a
Sketchy-SVG drawing stroked far wider than they turn: their normals cross
within the half width, so the stroke narrows to a waist where they cross
and opens past it, a lobe bounded by the normals' far ends and by where
they touch the curve of their centres; and an icon's hook, a cubic whose
first control is all but at its start, stroked with round caps, turning
tighter than its half width one way and at once the other. Within 8
levels, the exact model's own sampling and the flattening's tolerance.""")
# Each cell's tolerance, in levels.
tolerances = {"dash-end": 8, "start-tightening": 8, "end-tight": 8, "pie-sector": 8, "pie-quarter-cubic": 8, "waist": 8, "s-turn": 8}
kappa = 4 / 3 * math.tan(math.pi / 8)
cells = [
    ("dash-end", '<path d="M 40 80 C 40 55 45 50 50 50 C 55 50 60 55 60 80" stroke-width="30" stroke-dasharray="28 200" transform="translate(-5 -15)"/>'),
    ("start-tightening", '<path d="M 128.92 313.55 C 132.55 309.89 136.53 309.70 146.47 314.58" stroke-width="19" transform="translate(-100 -290)"/>'),
    ("end-tight", '<path d="M 40 80 C 40 55 45 50 50 50" stroke-width="30" transform="translate(-10 -25)"/>'),
    ("pie-sector", '<svg width="100" height="100"><circle cx="10" cy="30" r="60" stroke-width="120" stroke-dasharray="30 1000"/></svg>'),
    ("pie-quarter-cubic", f'<svg width="100" height="100"><path d="M70 30 C70 {30 + 60*kappa:.15g} {10 + 60*kappa:.15g} 90 10 90" stroke-width="120" stroke-dasharray="30 1000"/></svg>'),
    ("waist", '<path d="M 411.78333 134.5 c 0.61069 1.60001 1.27625 2.35789 1.87152 2.7 c 0.84811 0.4874 1.81516 0.70001 2.48849 0.90001" stroke-width="27.71787625" transform="matrix(2.5 0 0 2.5 -985 -291)"/>'),
    ("s-turn", '<path d="M17.742 19.611 C17.747 19.606 18.174 19.165 19.12 19.441" stroke-width="2" stroke-linecap="round" transform="matrix(20 0 0 20 -318.6 -337.8)"/>'),
]
for k, (name, path) in enumerate(cells):
    if name == 'pie-sector':
        # The outer radius is 120; even at the terminal angle its x is
        # 10+120*cos(.5)>100. Only the two radial boundaries cross the box.
        assert 10 + 120 * math.cos(.5) > 100
        bottom = 30 + 90 * math.tan(30 / 60)
        expected = f'<path d="M310 30 H400 V{bottom:.15g} Z"/>'
    else:
        alpha = zlib.decompress(base64.b64decode(EXACT[name]))
        rows = [[(0, 0, 0, alpha[y * 100 + x] / 255) for x in range(100)] for y in range(100)]
        expected = f'<image x="{100 * k}" width="100" height="100" image-rendering="pixelated" href="{png(rows)}"/>'
    unit.cell(name, (100 * k, 0, 100, 100), f'<g transform="translate({100 * k} 0)" fill="none" stroke="black">{path}</g>',
              expected, tolerance=tolerances[name])
unit.write()
