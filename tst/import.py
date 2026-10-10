#!/usr/bin/env python3
import glob
import hashlib
import os
import random
import re
import sys

SOURCES = [
    ("resvg", "resvg/*/crates/resvg/tests/tests", None),
    ("w3c", "w3c", None),
    ("blink", "blink", None),
    ("wpt", "wpt", None),
    ("adwaita", "adwaita", None),
    ("breeze", "breeze", 1000),
    ("papirus", "papirus", 1500),
    ("mdi", "mdi", 400),
    ("simple", "simple", 400),
    ("tabler", "tabler", 400),
    ("bootstrap", "bootstrap", 300),
    ("fontawesome", "fontawesome", 400),
    ("logos", "logos/*/logos", 800),
    ("wikimedia", "wikimedia", None),
    ("openclipart", "openclipart", None),
    ("inkscape", "inkscape", None),
]

SKIP_DIRS = {"svgweb", "harness", ".git", "node_modules", "text", "fonts", "W3C-I18N"}
MAX_BYTES = 1 << 20
SEED = 20261009

TEXT_ELEMENT = re.compile(rb"<(?:svg:)?(text|flowRoot)\b(?:[^>]*/>|.*?</(?:svg:)?\1\s*>)", re.S | re.I)
FORBIDDEN = re.compile(
    rb"<(?:svg:)?(?:script|foreignObject|animate|animateMotion|animateTransform|animateColor|set|discard|tspan|textPath|tref|altGlyph|video|audio|iframe|canvas)\b"
    rb"|@keyframes|<html\b|<body\b",
    re.I,
)
SVG_ROOT = re.compile(rb"<(?:svg:)?svg\b", re.I)
UNSAFE = re.compile(r"[^A-Za-z0-9._-]")


def candidates(root):
    for directory, dirs, files in os.walk(root, followlinks=False):
        dirs[:] = sorted(d for d in dirs if d not in SKIP_DIRS)
        for name in sorted(files):
            path = os.path.join(directory, name)
            if name.endswith(".svg") and os.path.isfile(path) and not os.path.islink(path):
                yield path


def cleaned(data):
    if len(data) > MAX_BYTES or not SVG_ROOT.search(data):
        return None
    data = TEXT_ELEMENT.sub(b"", data)
    if FORBIDDEN.search(data):
        return None
    return data


def safe_name(relative):
    parts = [UNSAFE.sub("_", part) for part in relative.split(os.sep)]
    return "__".join(parts)


def main():
    if len(sys.argv) != 3:
        sys.exit("usage: tst/import.py RAW_DIR CORPUS_DIR")
    raw, corpus = sys.argv[1], sys.argv[2]
    seen = set()
    for source, pattern, limit in SOURCES:
        roots = sorted(glob.glob(os.path.join(raw, pattern)))
        files = []
        for root in roots:
            for path in candidates(root):
                with open(path, "rb") as handle:
                    data = cleaned(handle.read())
                if data is None:
                    continue
                digest = hashlib.sha256(b" ".join(data.split())).digest()
                if digest in seen:
                    continue
                seen.add(digest)
                files.append((os.path.relpath(path, root), data))
        if limit is not None and len(files) > limit:
            files = random.Random(f"{SEED}:{source}").sample(files, limit)
        target = os.path.join(corpus, source)
        os.makedirs(target, exist_ok=True)
        names = set()
        for relative, data in sorted(files):
            name = safe_name(relative)
            if name in names:
                name = name[:-4] + "_" + hashlib.sha256(relative.encode()).hexdigest()[:8] + ".svg"
            names.add(name)
            with open(os.path.join(target, name), "wb") as handle:
                handle.write(data)
        print(f"{source}: {len(files)}")


if __name__ == "__main__":
    main()
