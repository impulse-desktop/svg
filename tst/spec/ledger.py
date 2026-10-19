#!/usr/bin/env python3
"""Checks the statement ledgers of tst/spec/statements against dev/spec and tst/unit.

usage: ledger.py [FILE...]

Each statement's anchor must exist in its page; a feature with a checked
statement must name units that exist; a statement left out must end with the
decision it follows, (Dn). A statement checked or left out ends with the
cells that test it, [unit#cell ...]: each unit must exist in tst/unit, and
each cell, an element of that id with a data-box, in the unit's expected
drawing, tst/expect/unit.svg, which tst/oracle.py compares it with. Prints
each file's counts and problems; exits 1 on any problem.
"""
import glob, os, re, sys

root = os.path.dirname(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
anchors = {}


def ids(page):
    if page not in anchors:
        path = os.path.join(root, "dev", "spec", page)
        anchors[page] = set(re.findall(r'<a id="([^"]+)"', open(path).read())) if os.path.exists(path) else None
    return anchors[page]


units = {os.path.splitext(name)[0] for name in os.listdir(os.path.join(root, "tst", "unit"))}
expected = {}


def cells(unit):
    if unit not in expected:
        path = os.path.join(root, "tst", "expect", unit + ".svg")
        text = open(path).read() if os.path.exists(path) else ""
        expected[unit] = {match.group(1) for match in re.finditer(r'<[^>]*\bid="([^"]+)"[^>]*\bdata-box="[^"]*"', text)}
        expected[unit] |= {match.group(1) for match in re.finditer(r'<[^>]*\bdata-box="[^"]*"[^>]*\bid="([^"]+)"', text)}

    return expected[unit]


links = r"\[((?:[a-z0-9.-]+#[\w.:-]+)(?:\s+[a-z0-9.-]+#[\w.:-]+)*)\]\s*$"
files = sys.argv[1:] or sorted(glob.glob(os.path.join(root, "tst", "spec", "statements", "**", "*.md"), recursive=True))
failed = False

for path in files:
    features = statements = checked = left = linked = 0
    problems = []
    named = []

    for number, line in enumerate(open(path), 1):
        heading = re.match(r"## (.+?): ((?:[a-z0-9.-]+(?: \(exists\))?)(?:, [a-z0-9.-]+(?: \(exists\))?)*)\s*$", line)

        if heading:
            features += 1
            named = [unit.split()[0] for unit in heading.group(2).split(", ")]
            problems += [f"{number}: no unit {unit}" for unit in heading.group(2).split(", ") if unit.endswith("(exists)") and unit.split()[0] not in units]
            continue

        if line.startswith("## ") and not line.startswith("## Notes"):
            problems.append(f"{number}: heading names no unit")

        statement = re.match(r"- \[([ x-])\] ([\w./-]+\.md)#([^:\s]+): (.+)", line)

        if not statement:
            if line.startswith("- ["):
                problems.append(f"{number}: not a statement")
            continue

        statements += 1
        page_ids = ids(statement.group(2))

        if page_ids is None:
            problems.append(f"{number}: no page {statement.group(2)}")
        elif statement.group(3) not in page_ids:
            problems.append(f"{number}: no anchor {statement.group(2)}#{statement.group(3)}")

        tests = re.search(links, line)
        text = line[:tests.start()].strip() if tests else line.strip()

        if statement.group(1) == "x":
            checked += 1
            problems += [f"{number}: no unit {unit}" for unit in named if unit not in units]
        elif statement.group(1) == "-":
            left += 1

            if not re.search(r"\(D\d+\)$", text):
                problems.append(f"{number}: left out with no decision")

        if statement.group(1) != " " and not tests:
            problems.append(f"{number}: no test cell")

        for link in tests.group(1).split() if tests else []:
            unit, cell = link.split("#")

            if unit not in units:
                problems.append(f"{number}: no unit {unit}")
            elif cell not in cells(unit):
                problems.append(f"{number}: no cell {link} in tst/expect/{unit}.svg")

        linked += bool(tests)

    print(f"{os.path.relpath(path, root)}: {features} features, {statements} statements, {checked} checked, {left} left out, {linked} linked to tests")

    for problem in problems:
        print(f"    {problem}")

    failed = failed or bool(problems)

sys.exit(1 if failed else 0)
