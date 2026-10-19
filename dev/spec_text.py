#!/usr/bin/env python3

"""Writes dev/spec: SVG 2, SVG 1.1 and the CSS modules SVG content draws
with, as Markdown to read and search offline. Every heading, definition and
other target keeps its id as an anchor, and a link into any of these specs
leads to the local file and anchor; one to anything else stays on the web.
Figures are left out, their alternative text kept. Run with no arguments to
fetch the editors' drafts again and rewrite the files; with a directory of
pages fetched before (laid out as the output is), to convert those."""

import os
import re
import sys
import urllib.request
from datetime import date
from html.parser import HTMLParser
from pathlib import Path

OUT = Path(__file__).resolve().parent / "spec"

SVG2 = "https://svgwg.org/svg2-draft/"
SVG2_PAGES = [
    "index", "intro", "conform", "render", "types", "struct", "styling", "geometry", "coords", "paths",
    "shapes", "text", "embedded", "painting", "pservers", "interact", "linking", "animate", "access", "implnote",
    "changes", "refs", "eltindex", "attindex", "propidx", "mimereg",
]

SVG11 = "https://www.w3.org/TR/SVG11/"
SVG11_PAGES = [
    "index", "intro", "concepts", "render", "types", "struct", "styling", "coords", "paths", "shapes", "text",
    "painting", "color", "pservers", "masking", "filters", "interact", "linking", "animate", "fonts", "metadata",
    "backward", "extend", "implnote", "conform", "access", "i18n", "minimize", "refs", "eltindex", "attindex",
    "propidx", "feature", "mimereg", "changes",
]

# Single-page modules, by their shortname with level; the drafts are
# published at drafts.csswg.org (the FXTF ones moved there).
MODULES = [
    "filter-effects-1", "css-masking-1", "compositing-1", "css-transforms-1", "css-transforms-2", "css-color-4",
    "css-values-4", "css-shapes-1", "css-images-3", "css-backgrounds-3", "css-box-4", "selectors-4",
    "css-syntax-3", "css-cascade-5", "css-variables-1", "css-values-5",
]

# The bases a link to these specs is written with.
SVG2_BASES = ["https://svgwg.org/svg2-draft/", "https://www.w3.org/TR/SVG2/", "https://www.w3.org/TR/SVG/",
              "https://w3c.github.io/svgwg/svg2-draft/", "http://www.w3.org/TR/SVG2/"]
SVG11_BASES = ["https://www.w3.org/TR/SVG11/", "http://www.w3.org/TR/SVG11/",
               "https://www.w3.org/TR/2011/REC-SVG11-20110816/", "http://www.w3.org/TR/2011/REC-SVG11-20110816/"]


def module_bases(name):
    bare = re.sub(r"-\d+$", "", name)
    hosts = ["https://drafts.csswg.org/", "https://drafts.fxtf.org/", "https://www.w3.org/TR/", "http://www.w3.org/TR/",
             "http://dev.w3.org/csswg/", "https://dev.w3.org/csswg/", "http://dev.w3.org/fxtf/", "https://dev.w3.org/fxtf/"]
    return [host + spec + "/" for host in hosts for spec in (name, bare)]


# Sections of the generated indexes past the text, only links.
SKIPPED = {"index", "index-defined-here", "index-defined-elsewhere", "idl-index", "issues-index", "w3c-conformance"}


def target(url, here):
    """The local file a link leads to, relative to here, or None."""
    if url.startswith("#"):
        return url
    page, _, fragment = url.partition("#")
    fragment = "#" + fragment if fragment else ""
    local = None
    for base in SVG2_BASES:
        if page.startswith(base):
            name = page[len(base):].removesuffix(".html").replace("Overview", "") or "index"
            local = f"svg2/{name}.md" if name in SVG2_PAGES else None
    for base in SVG11_BASES:
        if page.startswith(base):
            name = page[len(base):].removesuffix(".html").replace("Overview", "").replace("expanded-toc", "") or "index"
            local = f"svg11/{name}.md" if name in SVG11_PAGES else None
    # A link without a level goes to the first level here.
    for name in reversed(MODULES):
        for base in module_bases(name):
            if page == base or page == base[:-1]:
                local = f"{name}.md"
    if local is None and "://" not in page and page.endswith(".html") and "/" in here:
        # A page beside this one in a multi-page spec.
        name = page.removesuffix(".html").replace("Overview", "").replace("expanded-toc", "") or "index"
        pages = SVG2_PAGES if here.startswith("svg2/") else SVG11_PAGES
        local = f"{here.split('/')[0]}/{name}.md" if name in pages else None
    if local is None:
        return None
    path = os.path.relpath(local, os.path.dirname(here) or ".")
    return (path if local != here else "") + fragment


class Converter(HTMLParser):
    BLOCKS = {"p", "div", "section", "article", "main", "header", "footer", "blockquote", "figure", "figcaption",
              "details", "summary", "dl", "dt", "dd", "ul", "ol", "li", "table", "tr", "pre", "hr", "h1", "h2", "h3",
              "h4", "h5", "h6", "body", "address", "aside"}
    # Kept out with what they hold; the head's other tags hold no text, and
    # its end is often left out.
    SKIP = {"script", "style", "title", "nav", "template", "noscript", "svg", "object", "iframe", "button"}
    # The tags whose end HTML lets a later tag imply: each closes the open
    # ones listed, as far as the container it is in.
    IMPLIED = {
        "li": ({"li"}, {"ul", "ol"}),
        "dt": ({"dt", "dd"}, {"dl"}),
        "dd": ({"dt", "dd"}, {"dl"}),
        "tr": ({"td", "th", "tr"}, {"table", "tbody", "thead", "tfoot"}),
        "td": ({"td", "th"}, {"tr", "table"}),
        "th": ({"td", "th"}, {"tr", "table"}),
        "p": ({"p"}, {"div", "section", "li", "dd", "td", "th", "blockquote", "body"}),
    }

    def __init__(self, here, base):
        super().__init__(convert_charrefs=True)
        self.here = here
        self.base = base
        self.out = []
        self.line = []
        # A part kept out runs to the end of its first tag: only that tag's
        # name counts, as the ends of others inside are often left out.
        self.skip = None
        self.skip_depth = 0
        self.section_skipped = False
        self.pre = 0
        self.lists = []
        self.links = []
        self.table = None
        self.cell = None
        self.stack = []

    # Text goes to the open cell of a table, or to the line.
    def emit(self, text):
        (self.cell if self.cell is not None else self.line).append(text)

    def flush(self, gap=True):
        text = re.sub(r"[ \t\n]+", " ", "".join(self.line)).strip()
        self.line = []
        if text:
            depth = len(self.lists)
            self.out.append(("  " * max(depth - 1, 0)) + text)
            if gap and not depth:
                self.out.append("")

    def handle_starttag(self, tag, attrs):
        attrs = dict(attrs)
        classes = (attrs.get("class") or "").split()
        if self.skip:
            self.skip_depth += tag == self.skip
            return
        if tag in self.SKIP or "dfn-panel" in classes or "ref-hint" in classes or "self-link" in classes \
                or "wpt-tests-block" in classes or "annotation" in classes \
                or attrs.get("id") == "toc" or "toc" in classes:
            self.skip = tag
            self.skip_depth = 1
            return
        if tag == "h2":
            self.section_skipped = attrs.get("id") in SKIPPED
        if self.section_skipped:
            return
        closes, within = self.IMPLIED.get(tag, (set(), set()))
        if tag in self.BLOCKS:
            closes = closes | {"p"}
        for open_tag in reversed(self.stack):
            if open_tag in within:
                break
            if open_tag in closes:
                self.handle_endtag(open_tag)
                break
        self.stack.append(tag)
        if tag in self.BLOCKS and not self.pre and self.cell is None:
            self.flush()
        if tag in ("h1", "h2", "h3", "h4", "h5", "h6"):
            self.emit("#" * int(tag[1]) + " ")
        elif tag in ("ul", "ol"):
            self.lists.append([tag, 0])
        elif tag == "li":
            kind = self.lists[-1] if self.lists else ["ul", 0]
            kind[1] += 1
            self.emit("- " if kind[0] == "ul" else f"{kind[1]}. ")
        elif tag == "dt":
            self.emit("**")
        elif tag == "dd":
            self.emit(": ")
        elif tag == "pre":
            self.pre += 1
            self.out.append("```")
        elif tag == "table" and self.table is None:
            self.table = []
        elif tag == "tr" and self.table is not None:
            self.table.append([])
        elif tag in ("td", "th") and self.table is not None:
            self.cell = []
        elif tag == "br":
            self.emit("\n" if self.pre else " ")
        elif tag == "img" and attrs.get("alt"):
            self.emit(f"[image: {attrs['alt'].strip()}]")
        elif tag in ("em", "i", "var", "dfn") and not self.pre:
            self.emit("*")
        elif tag in ("strong", "b") and not self.pre:
            self.emit("**")
        elif tag in ("code", "tt", "kbd", "samp") and not self.pre:
            self.emit("`")
        elif tag == "sup":
            self.emit("^")
        elif tag == "sub":
            self.emit("_")
        elif tag == "a":
            href = attrs.get("href")
            link = target(href, self.here) if href else None
            if href and link is None and not re.match(r"[a-z]+:", href):
                link = self.base + href if self.base and not href.startswith("#") else href
            self.links.append((link if href else None, len(self.cell if self.cell is not None else self.line)))
        # A reference's own id serves only the backlinks left out.
        anchor = attrs.get("id") or (attrs.get("name") if tag == "a" else None)
        if anchor and not anchor.startswith("ref-for-"):
            self.emit(f'<a id="{anchor}"></a>')

    def handle_endtag(self, tag):
        if self.skip:
            self.skip_depth -= tag == self.skip
            if not self.skip_depth:
                self.skip = None
            return
        if self.section_skipped:
            return
        while self.stack and self.stack[-1] != tag and tag in self.stack:
            self.handle_endtag(self.stack[-1])
        if not self.stack or self.stack[-1] != tag:
            return
        self.stack.pop()
        if tag in ("em", "i", "var", "dfn") and not self.pre:
            self.emit("*")
        elif tag in ("strong", "b") and not self.pre:
            self.emit("**")
        elif tag in ("code", "tt", "kbd", "samp") and not self.pre:
            self.emit("`")
        elif tag == "dt":
            self.emit("**")
        elif tag == "a" and self.links:
            link, start = self.links.pop()
            buffer = self.cell if self.cell is not None else self.line
            text = "".join(buffer[start:]).strip()
            if link and text and not self.pre:
                del buffer[start:]
                buffer.append(f"[{text}]({link})")
        elif tag == "pre":
            self.pre -= 1
            self.out.append("".join(self.line).strip("\n"))
            self.line = []
            self.out.append("```")
            self.out.append("")
            return
        elif tag in ("td", "th") and self.table is not None and self.cell is not None:
            text = re.sub(r"\s+", " ", "".join(self.cell)).strip().replace("|", "\\|")
            if self.table:
                self.table[-1].append(text)
            self.cell = None
        elif tag == "table" and self.table is not None:
            rows = [row for row in self.table if row]
            self.table = None
            if rows:
                width = max(len(row) for row in rows)
                rows = [row + [""] * (width - len(row)) for row in rows]
                self.flush()
                self.out.append("| " + " | ".join(rows[0]) + " |")
                self.out.append("|" + " --- |" * width)
                for row in rows[1:]:
                    self.out.append("| " + " | ".join(row) + " |")
                self.out.append("")
            return
        elif tag in ("ul", "ol") and self.lists:
            self.flush(gap=False)
            self.lists.pop()
            if not self.lists:
                self.out.append("")
            return
        if tag in self.BLOCKS and not self.pre and self.cell is None:
            self.flush(gap=tag not in ("li", "dt", "dd"))

    def handle_data(self, data):
        if self.skip or self.section_skipped:
            return
        if self.pre:
            self.line.append(data)
        else:
            self.emit(data)

    def markdown(self):
        self.flush()
        text = "\n".join(self.out)
        text = re.sub(r"\n{3,}", "\n\n", text)
        return text.strip() + "\n"


def fetch(url):
    with urllib.request.urlopen(url, timeout=120) as response:
        return response.read().decode("utf-8", "replace")


def pages():
    """Every page: its output path, its url and the base its relative links take."""
    for name in SVG2_PAGES:
        yield f"svg2/{name}.md", SVG2 + ("" if name == "index" else name + ".html"), SVG2
    for name in SVG11_PAGES:
        yield f"svg11/{name}.md", SVG11 + ("" if name == "index" else name + ".html"), SVG11
    for name in MODULES:
        yield f"{name}.md", f"https://drafts.csswg.org/{name}/", f"https://drafts.csswg.org/{name}/"


README = """# Specifications

SVG 2 and SVG 1.1 by chapter, and the modules SVG content draws with, as
Markdown to read and search offline. Written by `dev/spec_text.py` from the
editors' drafts (SVG 1.1 from its Second Edition), each file headed by the
page it came from and the day. Every heading, definition and other target
keeps its id as an anchor, `<a id="..."></a>`, and a link into any of these
specs leads to the local file and anchor; links elsewhere stay on the web.
Figures are left out, their alternative text kept.

Searching:

- where something is defined: `rg -n 'id="propdef-stroke-linejoin"' dev/spec`,
  `rg -n 'id="elementdef-fegaussianblur"' dev/spec`, or by the visible word,
  `rg -n -i 'stroke shape' dev/spec/svg2`;
- what refers to a definition: `rg -n '#propdef-mask-type' dev/spec`;
- a property's table: `rg -n -A12 'id="propdef-paint-order"' dev/spec`.

"""


def main():
    source = Path(sys.argv[1]) if len(sys.argv) > 1 else None
    written = []
    for path, url, base in pages():
        html_path = source / path.replace(".md", ".html") if source else None
        text = html_path.read_text(encoding="utf-8", errors="replace") if source else fetch(url)
        converter = Converter(path, base)
        converter.feed(text)
        body = converter.markdown()
        output = OUT / path
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_text(f"<!-- {url}, as of {date.today().isoformat()}; written by dev/spec_text.py -->\n\n" + body, encoding="utf-8")
        title = next((line.lstrip("# ") for line in body.splitlines() if line.startswith("# ")), path)
        written.append((path, url, re.sub(r'<a id="[^"]*"></a>', "", title).strip()))
    lines = [README]
    for group, prefix in (("SVG 2", "svg2/"), ("SVG 1.1 Second Edition", "svg11/"), ("Modules", "")):
        lines.append(f"## {group}\n")
        for path, url, title in written:
            if path.startswith(prefix) and (prefix or "/" not in path):
                lines.append(f"- [{title}]({path}) ({url})")
        lines.append("")
    (OUT / "README.md").write_text("\n".join(lines), encoding="utf-8")
    print(f"wrote {len(written)} pages to {OUT}")


if __name__ == "__main__":
    main()
