#!/usr/bin/env python3
"""Check the performance gate and the native benchmark's failure handling."""
import copy
from pathlib import Path
import subprocess
import sys
import tempfile

import perf


def report(*times):
    return {"selected": len(times), "cases": [{"file": str(i), "old": {"ns": old}, "new": {"ns": new}}
                                              for i, (old, new) in enumerate(times)]}


def main():
    program = str(Path(sys.argv[1]).resolve())
    assert perf.accepted(report((100, 105)))
    assert not perf.accepted(report((100, 106)))
    # Weight by absolute time, not the average of per-file slowdowns.
    assert perf.accepted(report((1, 10), (1000, 1000)))
    assert not perf.accepted(report((1000, 1200), (100, 1)))
    assert not perf.accepted(report())
    assert not perf.accepted(report((100, 0)))
    missing = report((100, 100))
    missing["selected"] = 2
    assert not perf.accepted(missing)
    failed = report((100, 100), (1000, 1))
    for version in ("old", "new"):
        bad = copy.deepcopy(failed)
        bad["cases"][1][version] = {"error": "timeout"}
        assert not perf.accepted(bad)
    with tempfile.TemporaryDirectory(prefix="svg-perf-check-") as temp:
        folder = Path(temp)
        corpus = folder / "corpus"
        provider = corpus / "provider"
        provider.mkdir(parents=True)
        for i in range(10):
            (provider / f"{i}.svg").touch()
        (corpus / "EXCLUDED").write_text("provider/1: excluded\n")
        (corpus / "DESIGN").write_text("provider/3: deliberate difference\n")
        expected = [provider / f"{i}.svg" for i in (0, 5, 8)]
        assert perf.selection(corpus, "*", 3) == expected
        assert perf.selection(corpus, "provider/[4-9].svg", 3) == [provider / "4.svg", provider / "7.svg"]
        assert perf.selection(corpus, "missing/*", 3) == []
        sample = report((100, 100))
        sample.update(filter="*", stride=3, repeat=1, side=512, jobs=24)
        assert "SAMPLE index % 3 == 0" in perf.summary(sample)
        assert "one sample" in perf.summary(sample)
        drawing = folder / "test.svg"
        drawing.write_text('<svg xmlns="http://www.w3.org/2000/svg" width="8" height="8"><rect width="8" height="8"/></svg>')
        manifest = folder / "list"
        manifest.write_text(f"{drawing} 8 8\n")
        for repeat in ("1", "3"):
            result = subprocess.run([program, "bench", str(manifest), repeat], capture_output=True, text=True, check=True)
            name, ns, ok = result.stdout.strip().rsplit(" ", 2)
            assert name == str(drawing) and int(ns) > 0 and ok == "1"
        drawing.write_text("not an SVG document")
        result = subprocess.run([program, "bench", str(manifest), "3"], capture_output=True, text=True, check=True)
        assert result.stdout.strip().endswith(" 0 0")
        for repeat in ("0", "2", "32"):
            result = subprocess.run([program, "bench", str(manifest), repeat], capture_output=True, text=True)
            assert result.returncode != 0
    print("checked +5% boundary, weighted totals, quick selection, incomplete/failed samples and native benchmark errors")


if __name__ == "__main__":
    main()
