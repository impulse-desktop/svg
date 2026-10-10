#!/usr/bin/env python3
import os
import re
import struct
import subprocess
import sys
import tempfile
import zlib

LABELS = ["1x", "2x"]
RENDERERS = ["inkscape", "resvg", "firefox", "ours"]
ROOT = re.compile(rb"<(?:svg:)?svg\b[^>]*>", re.S)
UNITS = {b"": 1.0, b"px": 1.0, b"pt": 4 / 3, b"pc": 16.0, b"mm": 96 / 25.4, b"cm": 96 / 2.54, b"in": 96.0, b"em": 16.0, b"ex": 8.0, b"q": 96 / 101.6}


def size(path):
    with open(path, "rb") as handle:
        return struct.unpack(">II", handle.read(24)[16:24])


def attribute(name):
    return re.compile(rb"(\s)" + name + rb"\s*=\s*(\"[^\"]*\"|'[^']*')", re.S)


def length(value):
    match = re.fullmatch(rb"\s*([+-]?(?:\d+\.?\d*|\.\d+)(?:[eE][+-]?\d+)?)\s*([a-zA-Z]*)\s*", value or b"")
    if not match or match.group(2).lower() not in UNITS:
        return None
    value = float(match.group(1)) * UNITS[match.group(2).lower()]
    return value if value > 0 else None


def sized(data, width, height):
    match = ROOT.search(data)
    if not match:
        return data
    tag = match.group(0)
    found = attribute(rb"viewBox").search(tag)
    viewbox = found and len(re.findall(rb"[-+.\d]+", found.group(2))) == 4
    natural = []
    for name in (rb"width", rb"height"):
        found = attribute(name).search(tag)
        natural.append(length(found.group(2)[1:-1]) if found else None)
        tag = attribute(name).sub(rb"\1", tag)
    extra = b' width="%d" height="%d"' % (width, height)
    if not viewbox:
        tag = attribute(rb"preserveAspectRatio").sub(rb"\1", attribute(rb"viewBox").sub(rb"\1", tag))
        if natural[0] or natural[1]:
            extra += b' viewBox="0 0 %r %r" preserveAspectRatio="none"' % (natural[0] or float(width), natural[1] or float(height))
    end = len(tag) - (2 if tag.endswith(b"/>") else 1)
    return data[:match.start()] + tag[:end] + extra + tag[end:] + data[match.end():]


def decode(path):
    with open(path, "rb") as handle:
        data = handle.read()
    position = 8
    idat = b""
    while position < len(data):
        length_, kind = struct.unpack(">I4s", data[position:position + 8])
        body = data[position + 8:position + 8 + length_]
        if kind == b"IHDR":
            width, height, depth, color = struct.unpack(">IIBB", body[:10])
        elif kind == b"IDAT":
            idat += body
        position += 12 + length_
    if depth != 8 or color not in (2, 6):
        sys.exit(f"{path}: unsupported png")
    channels = 4 if color == 6 else 3
    raw = zlib.decompress(idat)
    stride = width * channels
    out = bytearray(width * height * 4)
    previous = bytearray(stride)
    for y in range(height):
        kind = raw[y * (stride + 1)]
        line = bytearray(raw[y * (stride + 1) + 1:(y + 1) * (stride + 1)])
        for x in range(stride):
            left = line[x - channels] if x >= channels else 0
            up = previous[x]
            corner = previous[x - channels] if x >= channels else 0
            if kind == 1:
                line[x] = (line[x] + left) & 255
            elif kind == 2:
                line[x] = (line[x] + up) & 255
            elif kind == 3:
                line[x] = (line[x] + (left + up) // 2) & 255
            elif kind == 4:
                guess = left + up - corner
                pa, pb, pc = abs(guess - left), abs(guess - up), abs(guess - corner)
                line[x] = (line[x] + (left if pa <= pb and pa <= pc else up if pb <= pc else corner)) & 255
        for x in range(width):
            out[(y * width + x) * 4:(y * width + x) * 4 + channels] = line[x * channels:(x + 1) * channels]
            if channels == 3:
                out[(y * width + x) * 4 + 3] = 255
        previous = line
    return width, height, out


def encode(path, width, height, rgba):
    def chunk(kind, body):
        return struct.pack(">I", len(body)) + kind + body + struct.pack(">I", zlib.crc32(kind + body) & 0xFFFFFFFF)
    raw = b"".join(b"\0" + bytes(rgba[y * width * 4:(y + 1) * width * 4]) for y in range(height))
    with open(path, "wb") as handle:
        handle.write(b"\x89PNG\r\n\x1a\n" + chunk(b"IHDR", struct.pack(">IIBBBBB", width, height, 8, 6, 0, 0, 0))
                     + chunk(b"IDAT", zlib.compress(raw, 9)) + chunk(b"IEND", b""))


def unblend(white, black):
    out = bytearray(len(white))
    for i in range(0, len(white), 4):
        alpha = round(sum(255 - (white[i + c] - black[i + c]) for c in range(3)) / 3)
        alpha = max(0, min(255, alpha))
        if alpha:
            for c in range(3):
                out[i + c] = max(0, min(255, round(black[i + c] * 255 / alpha)))
            out[i + 3] = alpha
    return out


def inkscape(program, jobs):
    script = "".join(
        f"file-open:{svg}; export-type:png; export-filename:{png}; export-width:{width}; export-height:{height}; "
        "export-png-color-mode:RGBA_8; export-background-opacity:0; export-do; file-close\n"
        for svg, png, width, height in jobs
    )
    subprocess.run([program, "--shell"], input=script + "quit\n", text=True, check=True,
                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip().splitlines()[0]


def resvg(program, jobs):
    for svg, png, width, height in jobs:
        with open(svg, "rb") as handle:
            data = sized(handle.read(), width, height)
        with tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)) as copy:
            copy.write(data)
            copy.flush()
            subprocess.run([program, copy.name, png], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        if size(png) != (width, height):
            sys.exit(f"{png}: resvg drew {size(png)}, not {width}x{height}")
    return "resvg " + subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip()


def firefox(program, jobs):
    with tempfile.TemporaryDirectory() as work:
        profile = os.path.join(work, "profile")
        os.mkdir(profile)
        for svg, png, width, height in jobs:
            shots = []
            for background in ("white", "black"):
                page = os.path.join(work, background + ".html")
                shot = os.path.join(work, background + ".png")
                with open(page, "w") as handle:
                    handle.write(f"<!doctype html><html><head><style>html,body{{margin:0;padding:0;background:{background};overflow:hidden}}"
                                 f"img{{display:block}}</style></head><body><img src=\"file://{svg}\" style=\"width:{width}px;height:{height}px\"></body></html>")
                subprocess.run([program, "--headless", "--no-remote", "--profile", profile, f"--window-size={width},{height}",
                                "--screenshot", shot, "file://" + page], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                shots.append(decode(shot))
            if any(shot[:2] != (width, height) for shot in shots):
                sys.exit(f"{png}: firefox drew {shots[0][:2]}, not {width}x{height}")
            encode(png, width, height, unblend(shots[0][2], shots[1][2]))
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip()


def ours(program, jobs):
    for svg, png, width, height in jobs:
        subprocess.run([program, "render", svg, png, str(width), str(height)], check=True)
    source = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    commit = subprocess.run(["git", "-C", source, "rev-parse", "--short", "HEAD"], capture_output=True, text=True).stdout.strip()
    return f"svg at {commit}"


def main():
    if len(sys.argv) != 4 or sys.argv[1] not in RENDERERS:
        sys.exit("usage: tst/canon.py inkscape|resvg|firefox|ours PROGRAM CORPUS_DIR")
    renderer, program, corpus = sys.argv[1], sys.argv[2], os.path.abspath(sys.argv[3])
    jobs = []
    with open(os.path.join(corpus, "CANON")) as handle:
        for line in handle:
            fields = [field.strip() for field in line.split(":", 2)]
            if len(fields) == 3 and fields[1] == renderer:
                for label in LABELS:
                    png = os.path.join(corpus, f"{fields[0]}.{label}.png")
                    jobs.append((os.path.join(corpus, fields[0] + ".svg"), png, *size(png)))
    if not jobs:
        sys.exit(f"no cases for {renderer} in {os.path.join(corpus, 'CANON')}")
    version = globals()[renderer](program, jobs)
    with open(os.path.join(corpus, renderer.upper()), "w") as handle:
        handle.write(version + "\n")
    print(f"drew {len(jobs) // len(LABELS)} cases with {version}")


if __name__ == "__main__":
    main()
