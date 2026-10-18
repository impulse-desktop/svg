#!/usr/bin/env python3
"""Checks the statement ledgers of tst/spec/statements against dev/spec and tst/unit.

usage: ledger.py [FILE...]

Each statement's anchor must exist in its page; a feature with a checked
statement must name units that exist; a statement left out must end with the
decision it follows, (Dn). Prints each file's counts and problems; exits 1 on
any problem.
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
files = sys.argv[1:] or sorted(glob.glob(os.path.join(root, "tst", "spec", "statements", "**", "*.md"), recursive=True))
failed = False

for path in files:
    features = statements = checked = left = 0
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

        if statement.group(1) == "x":
            checked += 1
            problems += [f"{number}: no unit {unit}" for unit in named if unit not in units]
        elif statement.group(1) == "-":
            left += 1

            if not re.search(r"\(D\d+\)$", line.strip()):
                problems.append(f"{number}: left out with no decision")

    print(f"{os.path.relpath(path, root)}: {features} features, {statements} statements, {checked} checked, {left} left out")

    for problem in problems:
        print(f"    {problem}")

    failed = failed or bool(problems)

sys.exit(1 if failed else 0)
