#!/usr/bin/env python3
"""Check the 256 versus 1024/4 pipeline and its ordinary score reports."""
from pathlib import Path
import subprocess
import sys
import tempfile

import canon
import score


def fields(path):
    return {words[0]: [float(v) for v in words[1:]]
            for line in path.read_text().splitlines() if (words := line.split())}


def main():
    program = str(Path(sys.argv[1]).resolve())
    with tempfile.TemporaryDirectory(prefix="svg-scale-check-") as temporary:
        folder = Path(temporary)
        source = folder / "drawing with spaces.svg"
        scores = folder / "score.txt"
        direct = folder / "direct.png"
        reduced = folder / "reduced.png"
        large = folder / "large.png"
        manual = folder / "manual.png"
        ordinary = folder / "ordinary.txt"
        examples = [
            (5, 3, '<rect width="5" height="3" fill="red"/>', (256, 154), True),
            (9, 29, '<path d="M.1 .2L8 28L.5 27Z" fill="#e13b79" fill-opacity=".5"/>', (79, 256), False),
            (1000, 1, '<rect width="1000" height="1" fill="blue"/>', (256, 1), True),
            (16, 16, '<path d="M2 8H14" stroke="red" stroke-width="8" vector-effect="non-scaling-stroke"/>', (256, 256), False),
        ]
        for width, height, drawing, dimensions, exact in examples:
            source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="{width}" height="{height}" viewBox="0 0 {width} {height}" preserveAspectRatio="none">{drawing}</svg>')
            subprocess.run([program, "scale", str(source), str(scores), str(direct), str(reduced)], check=True)
            w, h, small = canon.decode(direct)
            rw, rh, down = canon.decode(reduced)
            assert (w, h) == (rw, rh) == dimensions
            measured = fields(scores)
            delta = sum(abs(small[i + c] * small[i + 3] - down[i + c] * down[i + 3])
                        for i in range(0, len(small), 4) for c in range(3))
            delta += sum(abs(a - b) * 255 for a, b in zip(small[3::4], down[3::4]))
            expected = delta / (w * h * 4 * 255 * 255)
            assert abs(measured["mae"][0] - expected) <= 0.00000000051
            if exact:
                assert measured["mae"] == measured["struct"] == measured["aa"] == [0.0], (dimensions, measured)
            if "non-scaling-stroke" in drawing:
                assert measured["mae"][0] > 0 and measured["struct"][0] > 0
            subprocess.run([program, "render", str(source), str(manual), str(w), str(h)], check=True)
            assert manual.read_bytes() == direct.read_bytes()
            subprocess.run([program, "render", str(source), str(large), str(w * 4), str(h * 4)], check=True)
            subprocess.run([program, "shrink", str(large), str(manual), "4"], check=True)
            assert manual.read_bytes() == reduced.read_bytes()
            # The new pipeline uses exactly the ordinary corpus metrics.
            subprocess.run([program, "test", str(source), str(ordinary), "0", str(reduced)], check=True)
            assert fields(ordinary) == measured
            subprocess.run([program, "scale", str(source), str(scores)], check=True)
            assert fields(scores) == measured

        for invalid in ["not SVG", '<svg xmlns="http://www.w3.org/2000/svg" width="0" height="8"/>']:
            source.write_text(invalid)
            scores.unlink(missing_ok=True)
            result = subprocess.run([program, "scale", str(source), str(scores)], capture_output=True)
            assert result.returncode != 0 and not scores.exists()

        corpus = folder / "corpus"
        provider = corpus / "provider"
        provider.mkdir(parents=True)
        output = folder / "scores"
        (output / "provider").mkdir(parents=True)
        for name, error, share in [("low", .01, .3), ("high", .1, .2), ("failed", 1, 1)]:
            (provider / (name + ".svg")).touch()
            failure = "error exit 142\n" if name == "failed" else ""
            (output / "provider" / (name + ".txt")).write_text(f"mae {error}\nstruct {share}\naa {error}\nreference\nours\n{failure}")
        for excluded in ["EXCLUDED", "DESIGN"]:
            name = excluded.lower()
            (corpus / excluded).write_text(f"provider/{name}: omitted\n")
            (provider / (name + ".svg")).touch()
        cases = score.load(corpus, output, "*")
        assert len(cases) == 3
        assert len(score.load(corpus, output, "provider/high.svg")) == 1
        assert score.maxrms(cases)["case"] == "provider/failed.svg"
        report = folder / "report.txt"
        separate = {}
        for metric in ["meanrms", "maxrms", "struct", "aa", "all"]:
            subprocess.run([sys.executable, str(Path(score.__file__)), str(corpus), str(output), "*", metric, str(report)], check=True, capture_output=True)
            rows = [line for line in report.read_text().splitlines() if " | " in line]
            assert "provider/failed.svg [error: exit 142]" in rows[0]
            assert ("provider/low.svg" if metric == "struct" else "provider/high.svg") in rows[1]
            separate[metric] = report.read_bytes()
        assert "processing errors: 1" in report.read_text()
        together = folder / "reports"
        subprocess.run([sys.executable, str(Path(score.__file__)), str(corpus), str(output), "*", "reports", str(together)], check=True, capture_output=True)
        for metric, expected in separate.items():
            assert (together / (metric + ".txt")).read_bytes() == expected
    print("checked dimensions, exact 4x rendering, alpha-aware reduction, ordinary metrics, non-scaling strokes, failures and report ordering")


if __name__ == "__main__":
    main()
