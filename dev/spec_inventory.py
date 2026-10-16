#!/usr/bin/env python3

"""Builds tst/spec/inventory.json: what SVG 2 and the modules it draws with
define. Elements and their attributes come from the SVG working group's
definitions.xml, the values of SVG 2's attributes and properties from the
tables of the published draft, the filter and masking elements, their
attributes and values and every other property from W3C's curated webref
extracts. Keywords are the words of each value grammar, with the types it
names written out down to the generic ones (lengths, colours, urls and the
like), and the functions it names. The sources are fetched at the pinned
revisions below; run with no arguments to rewrite the inventory."""

import html
import json
import re
import sys
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path

SVGWG = "fc7791bd6d829cda3ed1502485c38329cbd8b00f"
SVGWG_PAGES = "4387529e585712696584a8cd45859e3dd3f2f8d6"
CSSWG = "a17071ddd3a207698d6b5625234a71268a008c13"
WEBREF = "8aa15db75b895ec02427ad5af23a052eced301b9"

RAW = "https://raw.githubusercontent.com"
OUTPUT = Path(__file__).resolve().parent.parent / "tst" / "spec" / "inventory.json"

# The modules whose properties SVG content takes besides SVG 2's own.
MODULES = ["filter-effects", "css-masking", "compositing", "css-transforms"]

# Types left as they are: values rather than keywords.
LEAVES = {
    "angle", "bg-size", "color", "custom-ident", "dashed-ident", "flex", "frequency", "ident",
    "image", "integer", "length", "length-percentage", "number", "percentage", "position",
    "ratio", "resolution", "string", "time", "url", "zero",
}

NAMESPACE = "{http://mcc.id.au/ns/local}"


def fetch(url):
    with urllib.request.urlopen(url) as response:
        return response.read().decode("utf-8")


def webref(path):
    return json.loads(fetch(f"{RAW}/w3c/webref/{WEBREF}/ed/{path}"))


def text(fragment):
    return re.sub(r"\s+", " ", html.unescape(re.sub(r"<[^>]+>", " ", fragment))).strip()


def words(grammar, types, properties, seen=()):
    """The keywords and functions of a value grammar, its types and the
    properties it refers to written out. A grammar in prose, with no
    alternatives and more than one word, names none."""
    found = set()
    grammar = re.sub(r"<[^<>]*\s[^<>]*>", " ", grammar.replace("\u2018", "'").replace("\u2019", "'"))

    if "|" not in grammar and not re.fullmatch(r"\s*[A-Za-z][\w-]*\s*", grammar) and re.search(r"[a-z]\s+[a-z]", re.sub(r"<[^>]*>", "", grammar)):
        return found

    for token in re.findall(r"<'?[A-Za-z][\w-]*(?:\(\))?'?(?:\s*\[[^\]]*\])?>|(?<![\w.])[A-Za-z][\w-]*\(|(?<![\w.])[A-Za-z][\w-]*", grammar):
        if token.startswith("<'"):
            name = re.match(r"<'([A-Za-z][\w-]*)", token).group(1)

            if name not in seen and name in properties:
                found |= words(properties[name], types, properties, seen + (name,))
        elif token.startswith("<"):
            name = re.match(r"<([A-Za-z][\w-]*(?:\(\))?)", token).group(1)

            if name.endswith("()"):
                found.add(name)
            elif name not in LEAVES and name not in seen and name in types:
                found |= words(types[name], types, properties, seen + (name,))
        elif token.endswith("("):
            found.add(token[:-1] + "()")
        elif len(token) == 1 or not token.isupper():
            found.add(token)

    return found


def productions(pages):
    """The types SVG 2 defines, from its grammar blocks."""
    types = {}

    for page in pages.values():
        blocks = re.findall(r"<pre[^>]*>(.*?)</pre>", page, re.S)
        blocks += re.findall(r'<p class="definition svg-prod">(.*?)</p>', page, re.S)

        for block in blocks:
            match = re.match(r"<([A-Za-z][\w-]*)>\s*=\s*(.*)", text(block), re.S)

            if match:
                types[match.group(1)] = match.group(2)

    return types


def attribute_values(pages):
    """The value grammar of each SVG 2 attribute definition, by its id, and
    of those its tables define, by name."""
    by_id = {}
    by_name = {}

    for page in pages.values():
        for match in re.finditer(r'<dt id="([^"]+)">(.*?)<dl class="attrdef-svg2">(.*?)</dl>', page, re.S):
            value = re.search(r"<dt>\s*Value\s*<dd>(.*?)(?=<dt>|$)", match.group(3), re.S)

            if value:
                by_id[match.group(1)] = text(value.group(1))

        for table in re.findall(r'<table class="attrdef def">(.*?)</table>', page, re.S):
            for row in re.split(r"<tr>", table)[2:]:
                cells = [text(cell) for cell in re.split(r"<td>", row)[1:]]

                if len(cells) >= 2:
                    by_name.setdefault(cells[0], cells[1])

    return by_id, by_name


def property_values(pages):
    """The value grammar of each property SVG 2's own tables define."""
    values = {}

    for page in pages.values():
        for table in re.findall(r'<table class="def svg-propdef">(.*?)</table>', page, re.S):
            rows = {}

            for row in re.split(r"<tr>", table)[1:]:
                cells = re.split(r"<t[hd]>", row)[1:]

                if len(cells) >= 2:
                    rows[text(cells[0]).rstrip(":").strip().lower()] = text(cells[1])

            for name in rows.get("name", "").split(","):
                if name.strip() and "value" in rows:
                    values[name.strip()] = rows["value"]

    return values


def bikeshed_attributes(source):
    """The attributes a Bikeshed source defines, by the element they are
    for, each with its value grammar."""
    found = []

    for block in re.finditer(r'<dl dfn-type="?element-attr"? dfn-for="?([\w,-]+)"?>(.*?)</dl>', source, re.S):
        for name, grammar in re.findall(r'^\s*: <dfn[^>]*>([\w:-]+)</dfn>\s*=\s*"(.*?)"$', block.group(2), re.M):
            grammar = re.sub(r"''(?:[\w-]+/)?([^']*)''", r"\1", grammar)
            grammar = text(re.sub(r"<<([\w-]+)>>", r"&lt;\1&gt;", grammar))

            for owner in block.group(1).split(","):
                found.append((owner, name, grammar))

    return found


def main():
    definitions = ET.fromstring(fetch(f"{RAW}/w3c/svgwg/{SVGWG}/master/definitions.xml"))
    categories = {}
    presentation = []

    for category in definitions.iter(NAMESPACE + "attributecategory"):
        categories[category.get("name")] = [(a.get("name"), a.get("href")) for a in category.iter(NAMESPACE + "attribute")]

        if category.get("presentationattributes"):
            presentation = [name.strip() for name in category.get("presentationattributes").split(",")]

    chapters = set()
    elements = {}
    shared = {}

    # Attributes defined once, for the elements they list or that name them.
    for attribute in definitions.findall(NAMESPACE + "attribute"):
        shared.setdefault(attribute.get("name"), (attribute.get("href"), attribute.get("elements")))

    def listed(element, key):
        return [name.strip() for name in (element.get(key) or "").split(",") if name.strip()]

    for element in definitions.iter(NAMESPACE + "element"):
        own = [(a.get("name"), a.get("href")) for a in element.findall(NAMESPACE + "attribute")]
        own += [(name, (shared.get(name) or ("", None))[0]) for name in listed(element, "attributes")]
        own += [(name, "") for name in listed(element, "geometryproperties")]
        groups = listed(element, "attributecategories")

        elements[element.get("name")] = {"own": own, "categories": [c for c in groups if c != "presentation"], "presentation": "presentation" in groups}

    for name, (href, owners) in shared.items():
        for owner in (owners or "").split(","):
            if owner.strip() in elements:
                elements[owner.strip()]["own"].append((name, href))

    for element in elements.values():
        for name, href in element["own"]:
            chapters.add((href or "").split("#")[0])

    pages = {chapter: fetch(f"{RAW}/w3c/svgwg/{SVGWG_PAGES}/svg2-draft/{chapter}") for chapter in sorted(c for c in chapters if c.endswith(".html"))}

    for chapter in ["painting.html", "coords.html", "types.html", "render.html", "masking.html", "pservers.html", "styling.html", "geometry.html"]:
        if chapter not in pages:
            pages[chapter] = fetch(f"{RAW}/w3c/svgwg/{SVGWG_PAGES}/svg2-draft/{chapter}")

    svg_types = productions(pages)
    svg_attributes, svg_attributes_by_name = attribute_values(pages)
    svg_properties = property_values(pages)
    css = webref("css.json")
    types = {t["name"]: t["syntax"] for t in css["types"] if t.get("syntax")}
    types.update(svg_types)
    css_properties = {p["name"]: p for p in css["properties"]}
    grammars = {name: p["syntax"] for name, p in css_properties.items() if p.get("syntax")}
    grammars.update(svg_properties)

    inventory = {
        "sources": {
            "svgwg definitions.xml": SVGWG,
            "svgwg gh-pages svg2-draft": SVGWG_PAGES,
            "csswg-drafts filter-effects-1, css-masking-1": CSSWG,
            "webref curated": WEBREF,
        },
        "categories": {},
        "elements": {},
        "properties": {},
    }

    def keywords(attribute, grammar):
        return words(grammar, types, grammars) - {attribute} if grammar else set()

    def entry(element):
        return inventory["elements"].setdefault(element, {"attributes": {}, "categories": [], "presentation": False})

    def add(element, attribute, grammar):
        attributes = entry(element)["attributes"]
        attributes[attribute] = sorted(set(attributes.get(attribute, [])) | keywords(attribute, grammar))

    # Attributes of a category, such as the core or the event ones, are the
    # same on every element that takes them.
    for category, members in categories.items():
        inventory["categories"][category] = {
            attribute: sorted(keywords(attribute, svg_attributes.get((href or "").split("#")[-1]) or svg_attributes_by_name.get(attribute)))
            for attribute, href in members
        }

    for name, element in elements.items():
        entry(name)["categories"] = element["categories"]
        entry(name)["presentation"] = element["presentation"]

        for attribute, href in element["own"]:
            add(name, attribute, svg_attributes.get((href or "").split("#")[-1]) or svg_attributes_by_name.get(attribute))

    # Filter Effects and CSS Masking: elements, attributes, attribute values.
    for spec in ["filter-effects-1", "css-masking-1"]:
        listed_elements = [e["name"] for e in webref(f"elements/{spec}.json")["elements"]]
        primitives = [n for n in listed_elements if n.startswith("fe") and not n.startswith("feFunc") and n not in ("feMergeNode", "feDistantLight", "fePointLight", "feSpotLight")]

        for name in listed_elements:
            entry(name)["categories"] = sorted(set(entry(name)["categories"]) | {"core"})
            entry(name)["presentation"] = True

        # The attributes every filter primitive or transfer function takes
        # are a category of their own, as the core ones are.
        shared_by = {"filter-primitive": ("filter primitive", primitives), "feComponentTransfer": ("transfer function", ["feFuncR", "feFuncG", "feFuncB", "feFuncA"])}

        for owner, attribute, grammar in bikeshed_attributes(fetch(f"{RAW}/w3c/csswg-drafts/{CSSWG}/{spec}/Overview.bs")):
            if owner in shared_by:
                category, members = shared_by[owner]
                group = inventory["categories"].setdefault(category, {})
                group[attribute] = sorted(set(group.get(attribute, [])) | keywords(attribute, grammar))

                for member in members:
                    entry(member)["categories"] = sorted(set(entry(member)["categories"]) | {category})
            else:
                add(owner, attribute, grammar)

    # Properties: SVG 2's presentation attributes and its own properties,
    # with the modules' ones; SVG 2's tables first, else the latest module.
    names = set(presentation) | set(svg_properties)

    for spec in MODULES:
        try:
            for p in webref(f"css/{spec}.json")["properties"]:
                names.add(p["name"])
        except Exception:
            pass

    for name in sorted(names):
        grammar = grammars.get(name)
        inventory["properties"][name] = sorted(words(grammar, types, grammars)) if grammar else []

    OUTPUT.parent.mkdir(parents=True, exist_ok=True)
    OUTPUT.write_text(json.dumps(inventory, indent=1, sort_keys=True) + "\n")
    print(f"{OUTPUT}: {len(inventory['elements'])} elements, {sum(len(e['attributes']) for e in inventory['elements'].values())} element attributes, {sum(len(c) for c in inventory['categories'].values())} category attributes, {len(inventory['properties'])} properties")


if __name__ == "__main__":
    sys.exit(main())
