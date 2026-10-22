"""Object boxes exclude isolated moves but include zero-length segments.

Mask controls use literal clip rectangles, calculated from the geometric
box and the mask's default -10%, -10%, 120%, 120% region. Gradient controls
use independently calculated extrema in user space.
"""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
NAME = "path-isolated-move-bbox"
BASE = "M10 10H50V30"
CURVE = "M10 30C10 10 50 10 50 30Z"
ARC = "M10 30A20 20 0 0 1 50 30Z"
CASES = [
    ("leading-mask", "M10 50 " + BASE, BASE, 10, 30, True),
    ("trailing-mask", BASE + " M10 50", BASE, 10, 30, True),
    ("zero-line", "M10 50L10 50 " + BASE, "M10 50L10 50 " + BASE, 10, 50, True),
    ("close-only", "M10 50Z " + BASE, "M10 50Z " + BASE, 10, 50, True),
    ("zero-cubic", "M10 50C10 50 10 50 10 50 " + BASE, "M10 50L10 50 " + BASE, 10, 50, True),
    ("zero-arc", "M10 50A20 20 0 0 1 10 50 " + BASE, BASE, 10, 30, True),
    ("leading-curve", "M10 50 " + CURVE, CURVE, 15, 30, False),
    ("trailing-curve", CURVE + " M0 60", CURVE, 15, 30, False),
    ("leading-arc", "M10 50 " + ARC, ARC, 10, 30, False),
    ("trailing-arc", ARC + " M10 50", ARC, 10, 30, False),
    ("many-moves", "M0 60M60 0 " + BASE + "Z M60 60M0 0", BASE + "Z", 10, 30, False),
    ("zero-gradient", BASE + "Z M10 50L10 50", BASE + "Z", 10, 50, False),
]

unit = ['<svg xmlns="http://www.w3.org/2000/svg" width="256" height="192">',
        '<rect width="256" height="192" fill="white"/>',
        '<defs><mask id="region"><rect width="64" height="64" fill="white"/></mask>',
        '<linearGradient id="object" x2="0" y2="1"><stop offset=".5" stop-color="navy"/><stop offset=".5" stop-color="gold"/></linearGradient></defs>']
expected = [unit[0], unit[1]]
for i, (name, data, control, low, high, mask) in enumerate(CASES):
    x, y = i % 4 * 64, i // 4 * 64
    unit.append(f'<g transform="translate({x} {y})">')
    expected.append(f'<g id="{name}" data-box="{x} {y} 64 64" transform="translate({x} {y})">')
    if mask:
        unit.append(f'<path d="{data}" fill="none" stroke="lime" stroke-width="20" mask="url(#region)"/>')
        expected.append(f'<clipPath id="clip-{i}"><rect x="6" y="{low - (high - low) / 10:g}" width="48" height="{(high - low) * 1.2:g}"/></clipPath>')
        expected.append(f'<path d="{control}" fill="none" stroke="lime" stroke-width="20" clip-path="url(#clip-{i})"/>')
    else:
        unit.append(f'<path d="{data}" fill="url(#object)"/>')
        expected.append(f'<linearGradient id="plain-{i}" gradientUnits="userSpaceOnUse" x2="0" y1="{low}" y2="{high}"><stop offset=".5" stop-color="navy"/><stop offset=".5" stop-color="gold"/></linearGradient>')
        expected.append(f'<path d="{control}" fill="url(#plain-{i})"/>')
    unit.append('</g>')
    expected.append('</g>')
for values, directory in [(unit, "unit"), (expected, "expect")]:
    (ROOT / "tst" / directory / (NAME + ".svg")).write_text("\n".join(values + ['</svg>']) + "\n")

# A path made entirely of moves keeps its last specified position.
(ROOT / "tst/unit/measure-path-move.svg").write_text('<svg xmlns="http://www.w3.org/2000/svg"><path d="M30 40M10 20"/></svg>\n')
(ROOT / "tst/expect/measure-path-move.svg").write_text('<svg xmlns="http://www.w3.org/2000/svg" width="10" height="20"><g id="last-position" data-box="0 0 10 20"/></svg>\n')
