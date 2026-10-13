#!/usr/bin/env python3
"""Check cascade order where counts exceed a byte of specificity or many declarations precede a rule."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon

GREEN = (0, 128, 0, 255)


def cases():
    classes = ".c" * 256
    filler = "opacity: 1; " * 8192
    # Each case paints green where the cascade is right. Were a count to carry
    # into the next field of the priority, 256 classes would weigh an id, 512
    # ids a style attribute, 1024 ids an important declaration, and the 4096th
    # declaration of a rule would come after the next rule.
    yield "id before 256 classes", f"#i {{ fill: green }} {classes} {{ fill: red }}", ""
    yield "256 classes before id", f"{classes} {{ fill: red }} #i {{ fill: green }}", ""
    yield "255 classes before id", f"{'.c' * 255} {{ fill: red }} #i {{ fill: green }}", ""
    yield "style attribute over 512 ids", f"{'#i' * 512} {{ fill: red }}", ' style="fill: green"'
    yield "important over 1024 ids", f"rect {{ fill: green !important }} {'#i' * 1024} {{ fill: red }}", ""
    yield "later rule after 8192 declarations", f"rect {{ {filler} fill: red }} rect {{ fill: green }}", ""
    yield "earlier specific rule after 8192 declarations", f"rect.c {{ {filler} fill: green }} rect {{ fill: red }}", ""


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/cascade_priority.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    checked = 0
    failures = []
    with tempfile.TemporaryDirectory(prefix="svg-cascade-priority-") as work:
        source = Path(work) / "case.svg"
        output = Path(work) / "case.png"
        for name, sheet, inline in cases():
            source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="4" height="4"><style>{sheet}</style>'
                              f'<rect id="i" class="c" width="4" height="4"{inline}/></svg>')
            subprocess.run([program, "render", str(source), str(output), "4", "4"], check=True, timeout=60)
            width, height, rgba = canon.decode(output)
            if (width, height) != (4, 4):
                sys.exit(f"wrong image size {width}x{height}")
            colors = {tuple(rgba[i:i + 4]) for i in range(0, len(rgba), 4)}
            if colors != {GREEN}:
                failures.append(f"{name}: {sorted(colors)}")
            checked += 1
    if failures:
        sys.exit(f"{len(failures)} failed cases:\n" + "\n".join(failures))
    print(f"checked {checked} cascade orders")


if __name__ == "__main__":
    main()
