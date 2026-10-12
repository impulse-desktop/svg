#!/usr/bin/env python3
"""Render resumable Inkscape reference candidates without changing the corpus."""
import concurrent.futures
import contextlib
import fnmatch
import hashlib
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile

import canon


def digest(path):
    return hashlib.sha256(Path(path).read_bytes()).hexdigest()


def render(program, shrinker, corpus, output, svg, identity, side):
    case = svg.relative_to(corpus).with_suffix("").as_posix()
    target = output / case
    target.parent.mkdir(parents=True, exist_ok=True)
    stamp = Path(str(target) + ".json")
    jobs = []
    for label in canon.LABELS:
        width, height = canon.size(svg.with_suffix(f".{label}.png"))
        jobs.append((Path(str(target) + f".{label}.png"), width, height, canon.factor(side, width, height)))
    key = {**identity, "svg": digest(svg), "side": side, "sizes": [[w, h] for _, w, h, _ in jobs]}
    if stamp.exists():
        previous = json.loads(stamp.read_text())
        if previous.get("key") == key and previous.get("status") == "ok":
            checksums = previous.get("png", [])
            if len(checksums) == len(jobs) and all(png.exists() and digest(png) == checksum for (png, _, _, _), checksum in zip(jobs, checksums)):
                return case, "cached"
    result = {"case": case, "key": key}
    try:
        if any(scale > 1 for _, _, _, scale in jobs) and not canon.scalable(svg):
            raise ValueError("no viewBox or absolute size: enlargement does not implement device scale")
        with tempfile.TemporaryDirectory(prefix="export-", dir=output) as temporary, contextlib.ExitStack() as stack:
            exports = []
            commands = []
            for index, (_, width, height, scale) in enumerate(jobs):
                source = stack.enter_context(canon.inkscape_viewport(str(svg), width, height))
                large = Path(temporary) / f"{index}.png"
                exports.append(large)
                commands.append(f"file-open:{source}; export-type:png; export-filename:{large}; export-width:{width * scale}; export-height:{height * scale}; "
                                "export-png-color-mode:RGBA_8; export-background-opacity:0; export-do; file-close")
            commands.append("quit")
            subprocess.run([program, "--shell"], input="\n".join(commands) + "\n", text=True,
                           check=True, capture_output=True, timeout=180)
            for (png, width, height, scale), large in zip(jobs, exports):
                if not large.exists() or canon.size(large) != (width * scale, height * scale):
                    raise RuntimeError(f"missing or wrong-sized export: {large}")
                small = Path(temporary) / (png.name + ".small.png")
                subprocess.run([shrinker, "shrink", str(large), str(small), str(scale)],
                               check=True, capture_output=True, timeout=30)
                os.replace(small, png)
        result.update(status="ok", png=[digest(png) for png, _, _, _ in jobs])
    except Exception as error:
        result.update(status="failed", error=str(error))
        if isinstance(error, subprocess.CalledProcessError):
            result["stderr"] = str(error.stderr)[-4000:]
    temporary = stamp.with_suffix(".json.tmp")
    temporary.write_text(json.dumps(result, indent=2) + "\n")
    os.replace(temporary, stamp)
    return case, result["status"]


def main():
    if len(sys.argv) < 6:
        sys.exit("usage: tst/reference-inkscape.py INKSCAPE SVG CORPUS OUTPUT JOBS [GLOB...]")
    program, shrinker = sys.argv[1:3]
    corpus, output = [Path(path).resolve() for path in sys.argv[3:5]]
    workers = int(sys.argv[5])
    patterns = sys.argv[6:] or ["*/*"]
    if output == corpus or corpus in output.parents or output in corpus.parents:
        sys.exit("OUTPUT must be separate from CORPUS")
    if any(character in str(path) for path in (corpus, output) for character in (";", "\n")):
        sys.exit("Inkscape shell paths cannot contain semicolons or newlines")
    output.mkdir(parents=True, exist_ok=True)
    version = subprocess.check_output([program, "--version"], text=True).strip()
    side = int((corpus / "SIDE").read_text())
    identity = {"version": version, "shrinker": digest(shrinker), "viewport": 1}
    (output / "INKSCAPE").write_text(version + "\n")
    (output / "SIDE").write_text(str(side) + "\n")
    cases = [svg for svg in sorted(corpus.glob("*/*.svg")) if not svg.name.startswith(".")
             and any(fnmatch.fnmatch(svg.relative_to(corpus).as_posix(), pattern) for pattern in patterns)]
    counts = {"ok": 0, "cached": 0, "failed": 0}
    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as pool:
        futures = [pool.submit(render, program, shrinker, corpus, output, svg, identity, side) for svg in cases]
        for index, future in enumerate(concurrent.futures.as_completed(futures), 1):
            case, status = future.result()
            counts[status] += 1
            if status == "failed":
                print(f"failed: {case}; see {output / (case + '.json')}", flush=True)
            if index % 100 == 0 or index == len(cases):
                print(f"{index}/{len(cases)}: {counts}", flush=True)
    if counts["failed"]:
        sys.exit(1)


if __name__ == "__main__":
    main()
