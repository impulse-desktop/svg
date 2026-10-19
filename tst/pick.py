#!/usr/bin/env python3
"""Picks from harvested SVG files those that draw what the corpus seldom
does.

Each candidate is cleaned as tst/import.py cleans what it imports (text
cut, script, animation and foreign content refused, past 1 MB refused) and
any whose content the corpus or another candidate has is dropped. Its
features are tst/features.py's. A feature is weak while fewer than WEAK
files of the corpus, and of what is picked, have it; a candidate is worth
the sum, over its features, of how many files each still lacks. The pick
is greedy: the worthiest candidate, the smaller of two of one worth, which
the renderer must size and draw in time; the counts then grow, until no
candidate is worth anything.

With --count N the pick goes on past that, WEAK doubling whenever nothing
is worth anything, until N files are picked.

usage: tst/pick.py [--write] [--weak N] [--count N] [--cache FILE] SVG CORPUS OUT.tsv SOURCE=DIR...

Writes OUT.tsv, a picked file a line: source, path, worth, the features it
brought, the WEAK it was picked at. With --write, copies the picked files
into CORPUS/SOURCE. --weak sets the first WEAK, 5 by default. --cache keeps
what examining a candidate found, by its path, size and time, so a run
examines only what is new.
"""
import hashlib
import heapq
import json
import os
import subprocess
import sys
import tempfile
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

import features  # noqa: E402
import importlib  # noqa: E402

importer = importlib.import_module("import")

WEAK = 5


def digest(data):
    return hashlib.sha256(b" ".join(data.split())).hexdigest()


def examine(path):
    """A candidate's cleaned bytes' digest, its size and its features, or
    None."""
    try:
        data = importer.cleaned(Path(path).read_bytes())
    except Exception:  # noqa: BLE001
        return None

    if data is None:
        return None

    return path, digest(data), len(data), sorted(features.features(data.decode("utf-8", "replace")))


def examine_all(paths, jobs=24):
    """examine over the paths, in jobs processes of this script."""
    found = []
    jobs = min(jobs, len(paths))

    with tempfile.TemporaryDirectory(prefix="svg-pick-") as temporary:
        workers = []

        for k in range(jobs):
            listed = os.path.join(temporary, f"{k}.txt")

            with open(listed, "w") as handle:
                handle.write("\n".join(paths[k::jobs]))

            workers.append(subprocess.Popen([sys.executable, __file__, "--examine", listed], stdout=subprocess.PIPE, text=True))

        for worker in workers:
            for line in worker.stdout:
                found.append(tuple(json.loads(line)))

            worker.wait()

    return found


def draws(program, path):
    """Whether the renderer gives the file a size and draws it in time."""
    try:
        size = subprocess.run([program, "size", path], capture_output=True, text=True, timeout=20).stdout.split()

        if len(size) < 3 or float(size[1]) <= 0 or float(size[2]) <= 0:
            return False

        out = "/dev/null"
        return subprocess.run([program, "render", path, out, "512", "512"], capture_output=True, timeout=20).returncode == 0
    except (subprocess.TimeoutExpired, ValueError):
        return False


def main():
    if sys.argv[1:2] == ["--examine"]:
        for path in Path(sys.argv[2]).read_text().split("\n"):
            examined = examine(path) if path else None

            if examined:
                print(json.dumps(examined))

        return

    global WEAK

    write = "--write" in sys.argv
    arguments = [a for a in sys.argv[1:] if a != "--write"]

    count = 0

    cache = None

    while arguments[:1] in (["--weak"], ["--count"], ["--cache"]):
        if arguments[0] == "--weak":
            WEAK = int(arguments[1])
        elif arguments[0] == "--count":
            count = int(arguments[1])
        else:
            cache = arguments[1]

        arguments = arguments[2:]

    if len(arguments) < 4:
        sys.exit("usage: tst/pick.py [--write] [--weak N] [--count N] [--cache FILE] SVG CORPUS OUT.tsv SOURCE=DIR...")

    program, corpus, out = arguments[:3]
    sources = [a.split("=", 1) for a in arguments[3:]]
    counts = {}
    known = set()

    for path in sorted(Path(corpus).glob("*/*.svg")):
        data = path.read_bytes()
        known.add(digest(data))

        for key in features.features(data.decode("utf-8", "replace")):
            counts[key] = counts.get(key, 0) + 1

    paths = [(source, os.path.join(directory, name)) for source, root in sources for directory, _, names in os.walk(root) for name in names if name.endswith(".svg")]
    source_of = dict((path, source) for source, path in paths)

    def stamp(path):
        status = os.stat(path)

        return f"{path}\t{status.st_size}\t{status.st_mtime_ns}"

    known_results = {}

    if cache and os.path.exists(cache):
        for line in open(cache):
            key, result = line.rstrip("\n").rsplit("\t", 1)
            known_results[key] = json.loads(result)

    stamps = {path: stamp(path) for _, path in paths}
    fresh = examine_all([path for _, path in paths if stamps[path] not in known_results])

    if cache:
        with open(cache, "a") as handle:
            found = {row[0]: row for row in fresh}

            for _, path in paths:
                if stamps[path] not in known_results:
                    known_results[stamps[path]] = found.get(path)
                    handle.write(stamps[path] + "\t" + json.dumps(found.get(path)) + "\n")

    examined = [tuple(known_results[stamps[path]]) for _, path in paths if known_results.get(stamps[path])] if cache else fresh

    candidates = {}

    for path, key, size, keys in examined:
        if key not in known:
            known.add(key)
            candidates[path] = (size, keys)

    def worth(keys):
        return sum(max(0, WEAK - counts.get(k, 0)) for k in keys)

    picked = []
    done = set()

    while True:
        heap = [(-worth(keys), size, path) for path, (size, keys) in candidates.items() if path not in done]
        heapq.heapify(heap)

        while heap and (not count or len(picked) < count):
            negative, size, path = heapq.heappop(heap)
            keys = candidates[path][1]
            now = worth(keys)

            # Worth only falls: one of none now is done with.
            if now <= 0:
                continue

            if now != -negative:
                heapq.heappush(heap, (-now, size, path))
                continue

            done.add(path)

            if not draws(program, path):
                continue

            brought = [k for k in keys if counts.get(k, 0) < WEAK]

            for k in keys:
                counts[k] = counts.get(k, 0) + 1

            picked.append((source_of[path], path, now, brought, WEAK))
            print(f"{len(picked)}\t{WEAK}\t{now}\t{path}", flush=True)

        if not count or len(picked) >= count or len(done) == len(candidates):
            break

        WEAK *= 2

    with open(out, "w") as handle:
        for source, path, value, brought, weak in picked:
            handle.write(f"{source}\t{path}\t{value}\t{' | '.join(brought)}\t{weak}\n")

    if write:
        for source, path, _, _, _ in picked:
            root = dict(sources)[source]
            target = Path(corpus) / source
            target.mkdir(exist_ok=True)
            name = importer.safe_name(os.path.relpath(path, root))

            if (target / name).exists():
                name = name[:-4] + "_" + hashlib.sha256(path.encode()).hexdigest()[:8] + ".svg"

            (target / name).write_bytes(importer.cleaned(Path(path).read_bytes()))


if __name__ == "__main__":
    main()
