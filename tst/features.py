#!/usr/bin/env python3
"""The features of SVG files, to find what the corpus draws seldom.

A feature is a fact about a file, a key: the vocabulary of SVG 2 and CSS it
uses, as tst/spec_coverage.py reads it (elements, element attributes,
attribute keywords, properties, property keywords, functions), and the ways
it puts them together, which the vocabulary misses:

  nest P>C           an element in another
  ref A>E            a reference by an attribute or property to an element,
                     or by an href elsewhere: its scheme, a fragment or a
                     relative path
  chain E>E          an href from a template element to another
  len A:U            a length of an attribute in a unit, bad for none of CSS
  num A:F            a number of an attribute SVG reads, in a form: e for an
                     exponent, dot for no leading digit, neg for a negative
                     size
  path C             a path command
  tf A:F             a transform function of a transform attribute, bad for
                     none of CSS
  color P:F          a colour of a property in a form
  css S              a style sheet's selector or rule kind
  in P:I             a filter primitive's input
  prims N            a filter's primitive count, banded
  depth N            how deep the elements go, banded
  root F             the outermost svg's sizing

usage: tst/features.py FILE...       the features of each file, a line each
       tst/features.py --count DIR   how many files of DIR/*/*.svg have each
"""
import json
import re
import sys
import xml.etree.ElementTree as ElementTree
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import spec_coverage  # noqa: E402

ROOT = Path(__file__).resolve().parent.parent
INVENTORY = json.loads((ROOT / "tst" / "spec" / "inventory.json").read_text())
PROPERTIES = set(INVENTORY["properties"])
VOCABULARY = set(spec_coverage.inventory_items(INVENTORY))
ELEMENTS = set(INVENTORY["elements"])
ATTRIBUTES = {a for e in INVENTORY["elements"].values() for a in e["attributes"]} | PROPERTIES
SVG = "{http://www.w3.org/2000/svg}"
XLINK = "{http://www.w3.org/1999/xlink}"

URL = re.compile(r"url\(\s*['\"]?#([^'\")\s]+)")
NUMBER = re.compile(r"[-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?")
LENGTH = re.compile(r"^\s*([-+]?(?:\d+\.?\d*|\.\d+)(?:[eE][-+]?\d+)?)\s*([a-zA-Z%]*)\s*$")
FUNCTION = re.compile(r"([A-Za-z][\w-]*)\s*\(")
LENGTHS = {"x", "y", "width", "height", "cx", "cy", "r", "rx", "ry", "x1", "y1", "x2", "y2", "fx", "fy", "fr", "dx", "dy",
           "stroke-width", "stroke-dashoffset", "font-size", "refX", "refY", "markerWidth", "markerHeight", "startOffset"}
SIZES = {"width", "height", "r", "rx", "ry", "markerWidth", "markerHeight"}
COLORS = {"fill", "stroke", "stop-color", "flood-color", "lighting-color", "color"}
TRANSFORMS = {"transform", "gradientTransform", "patternTransform"}
REFERRING = {"fill", "stroke", "clip-path", "mask", "filter", "marker-start", "marker-mid", "marker-end", "marker"}
UNITS = ({"number", "%", "px", "cm", "mm", "q", "in", "pt", "pc", "em", "ex", "ch", "rem", "cap", "ic", "lh", "rlh", "rcap", "rch", "rex", "ric"}
         | {f"{p}v{a}" for p in ("", "s", "l", "d") for a in ("w", "h", "i", "b", "min", "max")} | {f"cq{a}" for a in ("w", "h", "i", "b", "min", "max")})
TRANSFORM_FUNCTIONS = {"matrix", "matrix3d", "translate", "translatex", "translatey", "translatez", "translate3d", "scale", "scalex", "scaley", "scalez",
                       "scale3d", "rotate", "rotatex", "rotatey", "rotatez", "rotate3d", "skew", "skewx", "skewy", "perspective"}
SCHEMES = {"data", "http", "https", "file"}


def band(n, bounds):
    """n as the first bound it is under, or past the last."""
    for bound in bounds:
        if n <= bound:
            return f"<={bound}"

    return f">{bounds[-1]}"


def local(tag):
    return tag[len(SVG):] if tag.startswith(SVG) else None


def declarations(element):
    """The element's properties: its presentation attributes, then its
    style attribute's declarations."""
    found = {name: value for name, value in element.attrib.items() if name in PROPERTIES}

    for name, value in spec_coverage.DECLARATION.findall(element.get("style", "")):
        found[name.strip().lower()] = value.strip()

    return found


COLOR_FUNCTIONS = {"rgb", "rgba", "hsl", "hsla", "hwb", "lab", "lch", "oklab", "oklch", "color", "color-mix", "var", "icc-color", "device-cmyk", "light-dark"}


def colour(value):
    v = value.strip().lower()

    if v.startswith("#"):
        return f"hex{len(v) - 1}" if len(v) - 1 in (3, 4, 6, 8) else "hex-bad"

    if v.startswith("url("):
        return "url-fallback" if ")" in v and v[v.index(")") + 1:].strip() else "url"

    match = FUNCTION.match(v)

    if match:
        return match.group(1) if match.group(1) in COLOR_FUNCTIONS else "function-other"

    return v if v in ("none", "currentcolor", "transparent", "inherit", "context-fill", "context-stroke") else "keyword"


def css(text):
    """The selector and rule kinds of a style sheet."""
    found = set()
    text = re.sub(r"/\*.*?\*/", " ", text.replace("<![CDATA[", " ").replace("]]>", " "), flags=re.S)

    for at in re.findall(r"@([\w-]+)", text):
        found.add(f"css @{at.lower()}")

    if "!important" in text:
        found.add("css !important")

    for selectors in re.findall(r"([^{}@;]+)\{", text):
        for selector in selectors.split(","):
            s = selector.strip()

            if not s:
                continue

            for pseudo in re.findall(r"::?([\w-]+)", s):
                found.add(f"css :{pseudo.lower()}")

            found |= {"css " + kind for kind, pattern in (("class", r"\.[\w-]"), ("id", r"#[\w-]"), ("attribute", r"\["), ("child", r">"), ("next", r"\+"), ("sibling", r"~"), ("universal", r"\*"))
                      if re.search(pattern, s)}

            if re.search(r"[\w\])]\s+[\w.#\[*:]", s):
                found.add("css descendant")

    return found


def structure(source):
    """The features of how the file puts its elements together."""
    found = set()

    # A document Python's parser refuses, of a DTD's entities say, gives
    # its vocabulary alone.
    try:
        root = ElementTree.fromstring(source)
    except ElementTree.ParseError:
        return set()

    ids = {}
    parents = {}

    for parent in root.iter():
        for child in parent:
            parents[child] = parent

        if parent.get("id"):
            ids.setdefault(parent.get("id"), parent)

    def name(e):
        return local(e.tag) if isinstance(e.tag, str) else None

    def depth(e):
        n = 0

        while e in parents:
            e = parents[e]
            n += 1

        return n

    deepest = 0

    for e in root.iter():
        tag = name(e)

        if tag not in ELEMENTS:
            continue

        parent = parents.get(e)

        # What describes an element draws nothing.
        if parent is not None and name(parent) in ELEMENTS and tag not in ("desc", "title", "metadata"):
            found.add(f"nest {name(parent)}>{tag}")

        deepest = max(deepest, depth(e))
        props = declarations(e)

        for prop, value in props.items():
            for target in URL.findall(value):
                if prop in REFERRING and target in ids and name(ids[target]) in ELEMENTS:
                    found.add(f"ref {prop}>{name(ids[target])}")

            if prop in COLORS:
                found.add(f"color {prop}:{colour(value)}")

        href = e.get("href") or e.get(XLINK + "href")

        if href and href.startswith("#") and href[1:] in ids and name(ids[href[1:]]) in ELEMENTS:
            found.add(f"ref href:{tag}>{name(ids[href[1:]])}")

            if tag in ("linearGradient", "radialGradient", "pattern", "use"):
                found.add(f"chain {tag}>{name(ids[href[1:]])}")
        elif href:
            scheme = re.match(r"([A-Za-z][\w+.-]+):", href)
            kind = ("scheme-" + scheme.group(1).lower() if scheme.group(1).lower() in SCHEMES else "scheme-other") if scheme else ("fragment" if "#" in href else "relative")
            found.add(f"ref href:{tag}>{kind}")

        for attr, value in e.attrib.items():
            attr = attr.split("}")[-1]

            if attr in LENGTHS:
                match = LENGTH.match(value)

                if match:
                    unit = match.group(2).lower() or "number"
                    found.add(f"len {attr}:{unit if unit in UNITS else 'bad'}")

                    if attr in SIZES and match.group(1).startswith("-"):
                        found.add(f"num {attr}:neg")

            # Numbers matter where SVG reads them.
            for number in NUMBER.findall(value) if attr in ATTRIBUTES else []:
                if "e" in number.lower():
                    found.add(f"num {attr}:e")

                if re.match(r"[-+]?\.", number):
                    found.add(f"num {attr}:dot")

            if attr in TRANSFORMS:
                for function in FUNCTION.findall(value):
                    found.add(f"tf {attr}:{function.lower() if function.lower() in TRANSFORM_FUNCTIONS else 'bad'}")

        if tag == "path":
            for command in set(re.findall(r"[MmLlHhVvCcSsQqTtAaZz]", e.get("d", ""))):
                found.add(f"path {command}")

        if tag and tag.startswith("fe"):
            for attr in ("in", "in2"):
                if e.get(attr):
                    value = e.get(attr)
                    found.add(f"in {tag}:{value if value in ('SourceGraphic', 'SourceAlpha', 'BackgroundImage', 'BackgroundAlpha', 'FillPaint', 'StrokePaint') else 'result'}")

        if tag == "filter":
            found.add(f"prims {band(len([c for c in e if name(c) and name(c).startswith('fe')]), [1, 3, 7])}")

        if tag == "style":
            found |= css(e.text or "")

        if e.get("style") and re.search(r"!important", e.get("style")):
            found.add("css !important")

    found.add(f"depth {band(deepest, [5, 10, 20])}")

    if name(root) == "svg":
        has_box = root.get("viewBox") is not None
        units = {(LENGTH.match(root.get(side, "")) or [None, None, None])[2] if root.get(side) else None for side in ("width", "height")}
        found.add(f"root viewBox:{'yes' if has_box else 'no'}")
        found |= {f"root size:{u.lower() if u else 'number'}" if u is not None else "root size:none" for u in units}

        if root.get("preserveAspectRatio"):
            found.add(f"root aspect:{root.get('preserveAspectRatio').split()[0]}")

    return found


EXCLUDED = [pattern for pattern, _ in spec_coverage.exclusions(ROOT / "tst" / "spec" / "excluded.txt")]


def features(source):
    """A file's features: its vocabulary, but what the README leaves out
    (tst/spec/excluded.txt), and its structure."""
    import fnmatch

    words = {key for key in spec_coverage.uses_text(source, PROPERTIES) if key in VOCABULARY and not any(fnmatch.fnmatchcase(key, pattern) for pattern in EXCLUDED)}

    return words | structure(source)


def main():
    if sys.argv[1:2] == ["--count"]:
        counts = {}

        for path in sorted(Path(sys.argv[2]).glob("*/*.svg")):
            for key in features(path.read_text(errors="replace")):
                counts[key] = counts.get(key, 0) + 1

        for key, count in sorted(counts.items(), key=lambda kv: (kv[1], kv[0])):
            print(f"{count}\t{key}")

        return

    for name in sys.argv[1:]:
        print(name + "\t" + "\t".join(sorted(features(Path(name).read_text(errors="replace")))))


if __name__ == "__main__":
    main()
