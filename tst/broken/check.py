#!/usr/bin/env python3
"""Render known failures against independent expectations; exit 1 on differences.

usage: python3 tst/broken/check.py SVG [--out DIRECTORY]

Uses the existing unit/oracle comparison, including its premultiplied
two-byte tolerance. These cases are deliberately outside the passing unit
group until the corresponding bugs are fixed.
"""
import argparse
from pathlib import Path
import subprocess
import sys


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("svg", type=Path)
    parser.add_argument("--out", type=Path, default=Path(".build/broken"))
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    args.out.mkdir(parents=True, exist_ok=True)
    failed = 0
    cases = sorted(root.glob("*.svg"))

    for case in cases:
        result = subprocess.run([
            sys.executable, str(root.parent / "oracle.py"), str(args.svg.resolve()),
            str(case), str(root / "expect" / case.name),
            str(args.out / (case.stem + ".txt")),
        ], capture_output=True, text=True, timeout=150)
        failed += result.returncode != 0
        print(f"{case.name}: {'FAIL' if result.returncode else 'PASS'}", flush=True)

        if result.returncode:
            print(result.stderr.rstrip(), flush=True)

    print(f"{failed} of {len(cases)} drawings differ from their expectations")
    return int(failed != 0)


if __name__ == "__main__":
    sys.exit(main())
