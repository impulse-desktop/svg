#!/usr/bin/env python3
"""Check PNG transfer functions against the PNG and sRGB equations."""
import base64
from pathlib import Path
import struct
import subprocess
import sys
import tempfile
import zlib

import canon


def chunk(kind, data):
    return (struct.pack(">I", len(data)) + kind + data
            + struct.pack(">I", zlib.crc32(kind + data) & 0xFFFFFFFF))


def png(kind, gamma, srgb=False, chromaticities=False):
    color = 6 if kind == "rgba" else 3 if kind == "palette" else 0
    data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, 8, color, 0, 0, 0))
    if gamma is not None:
        data += chunk(b"gAMA", struct.pack(">I", round(gamma * 100000)))
    if chromaticities:
        data += chunk(b"cHRM", struct.pack(">8I", 31270, 32900, 64000, 33000, 30000, 60000, 15000, 6000))
    if srgb:
        data += chunk(b"sRGB", b"\0")
    if kind == "palette":
        data += chunk(b"PLTE", bytes([237, 28, 36])) + chunk(b"tRNS", bytes([128]))
        sample = b"\0"
    elif kind == "rgba":
        sample = bytes([237, 28, 36, 128])
    else:
        sample = bytes([28])
    return data + chunk(b"IDAT", zlib.compress(b"\0" + sample)) + chunk(b"IEND", b"")


def expected(kind, gamma):
    values = [28, 28, 28, 255] if kind == "gray" else [237, 28, 36, 128]
    if gamma is not None:
        for k in range(3):
            linear = (values[k] / 255) ** (1 / gamma)
            encoded = 12.92 * linear if linear <= 0.0031308 else 1.055 * linear ** (1 / 2.4) - 0.055
            values[k] = round(255 * encoded)
    return values


def main():
    if len(sys.argv) != 2:
        sys.exit("usage: tst/png_gamma.py SVG")
    program = str(Path(sys.argv[1]).resolve())
    failures = []
    checked = 0
    with tempfile.TemporaryDirectory(prefix="svg-png-gamma-") as temporary:
        source = Path(temporary) / "case.svg"
        output = Path(temporary) / "case.png"
        for kind in ["rgba", "palette", "gray"]:
            for gamma in [None, 0.25, 0.45455, 0.5, 0.7, 1, 1.4, 2.2]:
                for chromaticities in [False, True]:
                    data = png(kind, gamma, chromaticities=chromaticities)
                    payload = base64.b64encode(data).decode()
                    source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">'
                                      f'<image width="32" height="32" href="data:image/png;base64,{payload}"/></svg>')
                    subprocess.run([program, "render", str(source), str(output), "32", "32", "32"],
                                   check=True, timeout=30)
                    width, height, pixels = canon.decode(output)
                    offset = (16 * width + 16) * 4
                    actual = list(pixels[offset:offset + 4])
                    target = expected(kind, gamma)
                    if (width, height) != (32, 32) or any(abs(a - b) > 1 for a, b in zip(actual, target)):
                        failures.append(f"{kind}/{gamma}/{chromaticities}: got {actual}, expected {target}")
                    checked += 1
            # The sRGB chunk takes precedence over the gAMA fallback.
            payload = base64.b64encode(png(kind, 0.45455, srgb=True, chromaticities=True)).decode()
            source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">'
                              f'<image width="32" height="32" href="data:image/png;base64,{payload}"/></svg>')
            subprocess.run([program, "render", str(source), str(output), "32", "32", "32"],
                           check=True, timeout=30)
            width, height, pixels = canon.decode(output)
            offset = (16 * width + 16) * 4
            actual = list(pixels[offset:offset + 4])
            if actual != expected(kind, None):
                failures.append(f"{kind}/sRGB: got {actual}, expected {expected(kind, None)}")
            checked += 1
        # Keep the module's conversion before quantization for 16-bit samples.
        # Quantizing linear 128/65535 to eight bits first would lose this shadow.
        data = b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", 1, 1, 16, 6, 0, 0, 0))
        data += chunk(b"gAMA", struct.pack(">I", 100000))
        data += chunk(b"IDAT", zlib.compress(b"\0" + struct.pack(">4H", 128, 8192, 32768, 32768)))
        payload = base64.b64encode(data + chunk(b"IEND", b"")).decode()
        source.write_text(f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">'
                          f'<image width="32" height="32" href="data:image/png;base64,{payload}"/></svg>')
        subprocess.run([program, "render", str(source), str(output), "32", "32", "32"],
                       check=True, timeout=30)
        width, height, pixels = canon.decode(output)
        offset = (16 * width + 16) * 4
        actual = list(pixels[offset:offset + 4])
        target = [6, 99, 188, 128]
        if any(abs(a - b) > 1 for a, b in zip(actual, target)):
            failures.append(f"linear-16-bit: got {actual}, expected {target}")
        checked += 1
    if failures:
        sys.exit("\n".join(failures))
    print(f"checked {checked} PNG gamma and profile-precedence samples")


if __name__ == "__main__":
    main()
