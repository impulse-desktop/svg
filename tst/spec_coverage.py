#!/usr/bin/env python3

"""What of SVG 2 the renderer knows and the units draw. Every item of
tst/spec/inventory.json (elements, element attributes, attribute keywords,
properties, property keywords and functions) is either drawn by a unit, or
excluded with a reason in tst/spec/excluded.txt; an item neither is
reported as missing when svg.cpp has no string for it, untested when it
has. With --corpus the report also counts the corpus files that use each
item, the only check of it against other renderers.

usage: tst/spec_coverage.py [--corpus] [REPORT]
"""

import fnmatch
import json
import re
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
SPEC = ROOT / "tst" / "spec"

TAG = re.compile(r"<([A-Za-z_][\w:.-]*)((?:\s+[^\s=/>]+\s*=\s*(?:\"[^\"]*\"|'[^']*'))*)\s*/?>")
ATTRIBUTE = re.compile(r"([^\s=/>]+)\s*=\s*(?:\"([^\"]*)\"|'([^']*)')")
WORD = re.compile(r"(?<![\w.#-])([A-Za-z][\w-]*)(\()?")
DECLARATION = re.compile(r"([A-Za-z-]+)\s*:\s*([^;{}]*)")


def local(name):
    return name.split(":")[-1] if not name.startswith(("xlink:", "xml:")) else name


def words(value):
    return {name + "()" if call else name for name, call in WORD.findall(value)}


def uses(path, properties):
    """The items a drawing uses, as the keys of the report."""
    source = path.read_text(errors="replace")
    source = re.sub(r"<!--.*?-->", " ", source, flags=re.S)
    found = set()

    def declare(name, value):
        name = name.strip().lower()
        found.add(f"property {name}")

        for word in words(value):
            found.add(f"keyword {name}:{word.lower()}")

    for match in TAG.finditer(source):
        element = local(match.group(1))
        found.add(f"element {element}")

        for name, double, single in ATTRIBUTE.findall(match.group(2)):
            value = double if double else single
            name = local(name) if not name.startswith(("xlink:", "xml:")) else name
            found.add(f"attribute {element}/{name}")
            found.add(f"attribute */{name}")

            for word in words(value):
                found.add(f"value {element}/{name}={word}")
                found.add(f"value */{name}={word}")

            if name == "style":
                for prop, text in DECLARATION.findall(value):
                    declare(prop, text)
            elif name in properties:
                declare(name, value)

    for block in re.findall(r"<style[^>]*>(.*?)</style>", source, re.S):
        block = block.replace("<![CDATA[", " ").replace("]]>", " ")

        for rule in re.findall(r"\{([^{}]*)\}", block):
            for prop, text in DECLARATION.findall(rule):
                declare(prop, text)

    return found


def inventory_items(inventory):
    """Every item of the inventory, by key: an element's own attributes for
    it, those of a category for any element, and the keywords of every
    attribute for the attribute, on whatever element: one parser reads them."""
    items = []
    values = []

    for category in inventory["categories"].values():
        for attribute, keywords in category.items():
            if f"attribute */{attribute}" not in items:
                items.append(f"attribute */{attribute}")

            values += [f"value */{attribute}={keyword}" for keyword in keywords]

    for element, entry in inventory["elements"].items():
        items.append(f"element {element}")

        for attribute, keywords in entry["attributes"].items():
            items.append(f"attribute {element}/{attribute}")
            values += [f"value */{attribute}={keyword}" for keyword in keywords]

    items += sorted(set(values))

    for prop, keywords in inventory["properties"].items():
        items.append(f"property {prop}")
        items += [f"keyword {prop}:{keyword.lower()}" for keyword in keywords]

    return items


def exclusions(path):
    """The excluded patterns, each with its reason."""
    found = []

    for line in path.read_text().splitlines():
        line = line.strip()

        if line and not line.startswith("#"):
            key, _, reason = line.partition(": ")
            found.append((key.strip(), reason.strip()))

    return found


def known(item, strings, lowered):
    """Whether svg.cpp has a string for the item's last word."""
    kind, key = item.split(" ", 1)
    word = re.split(r"[/=:]", key)[-1]

    if word.endswith("()"):
        word = word[:-2]

    return word in strings or (kind in ("keyword", "property") and word.lower() in lowered)


def literals(source):
    """The string literals of a C++ source, past its comments and its
    character literals."""
    found = set()
    i = 0

    while i < len(source):
        if source.startswith("//", i):
            i = source.find("\n", i)
            i = len(source) if i < 0 else i
        elif source.startswith("/*", i):
            i = source.find("*/", i + 2) + 2
        elif source[i] == "'":
            i += 1

            while i < len(source) and source[i] != "'":
                i += 2 if source[i] == "\\" else 1

            i += 1
        elif source[i] == '"':
            j = i + 1

            while j < len(source) and source[j] != '"':
                j += 2 if source[j] == "\\" else 1

            found.add(source[i + 1:j])
            i = j + 1
        else:
            i += 1

    return found


def main():
    arguments = [a for a in sys.argv[1:] if not a.startswith("--")]
    inventory = json.loads((SPEC / "inventory.json").read_text())
    properties = set(inventory["properties"])
    excluded = exclusions(SPEC / "excluded.txt")
    strings = literals((ROOT / "svg.cpp").read_text())
    strings |= {part for s in list(strings) for part in s.split()}
    lowered = {s.lower() for s in strings}
    units = set()

    for path in sorted((ROOT / "tst" / "unit").glob("*.svg")):
        units |= uses(path, properties)

    corpus = {}

    if "--corpus" in sys.argv:
        for path in sorted((ROOT / "tst" / "corpus").glob("*/*.svg")):
            for item in uses(path, properties):
                corpus[item] = corpus.get(item, 0) + 1

    sections = {"missing": [], "untested": [], "excluded": [], "covered": []}

    for item in inventory_items(inventory):
        reason = next((r for pattern, r in excluded if fnmatch.fnmatchcase(item, pattern)), None)

        if item in units:
            sections["covered"].append((item, ""))
        elif reason is not None:
            sections["excluded"].append((item, reason))
        elif known(item, strings, lowered):
            sections["untested"].append((item, ""))
        else:
            sections["missing"].append((item, ""))

    lines = []

    for name in ["missing", "untested", "excluded", "covered"]:
        lines.append(f"{name} {len(sections[name])}")

    for name in ["missing", "untested"]:
        lines.append("")
        lines.append(f"== {name}")

        for item, _ in sections[name]:
            lines.append(f"{item}" + (f"  (corpus {corpus.get(item, 0)})" if corpus else ""))

    if corpus:
        lines.append("")
        lines.append("== covered by units only, in no corpus file")

        for item, _ in sections["covered"]:
            if not corpus.get(item):
                lines.append(item)

    report = "\n".join(lines) + "\n"

    if arguments:
        Path(arguments[0]).write_text(report)
    else:
        sys.stdout.write(report)

    return 1 if sections["missing"] or sections["untested"] else 0


if __name__ == "__main__":
    sys.exit(main())
