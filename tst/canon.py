#!/usr/bin/env python3
import contextlib
import base64
import json
import os
import re
import struct
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET
import zlib
import zipfile

import svg11_reference
import noise_reference
import local_filter_reference

LABELS = ["1x", "2x"]
RENDERERS = ["inkscape", "inkscape_legacy", "inkscape_softlight", "batik", "resvg", "firefox", "firefox_svg11", "chromium_noise", "chromium_noise_blend", "chromium_noise_local", "ours"]
ROOT = re.compile(rb"<(?:[\w.-]+:)?svg\b[^>]*>", re.S)
UNITS = {b"": 1.0, b"px": 1.0, b"pt": 4 / 3, b"pc": 16.0, b"mm": 96 / 25.4, b"cm": 96 / 2.54, b"in": 96.0, b"em": 16.0, b"ex": 8.0, b"q": 96 / 101.6}


def size(path):
    with open(path, "rb") as handle:
        return struct.unpack(">II", handle.read(24)[16:24])


def factor(side, width, height):
    return max(1, side // max(width, height))


def scalable(svg):
    with open(svg, "rb") as handle:
        match = ROOT.search(handle.read())
    if not match:
        return False
    tag = match.group(0)
    if attribute(rb"viewBox").search(tag):
        return True
    sides = [attribute(name).search(tag) for name in (rb"width", rb"height")]
    return all(found and length(found.group(2)[1:-1]) for found in sides)


def shrink(shrinker, large, png, scale):
    subprocess.run([shrinker, "shrink", large, png, str(scale)], check=True)


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


def enlarged(jobs, name):
    for svg, png, width, height, scale in jobs:
        if scale > 1 and not scalable(svg):
            sys.exit(f"{svg}: {name} can only enlarge it, which differs from a device scale without a viewBox or an absolute size")


@contextlib.contextmanager
def inkscape_viewport(svg, width, height):
    # Export size scales the page. Lay out the SVG at the CSS size first so
    # rounding the reference aspect ratio preserves viewBox alignment.
    with open(svg, "rb") as handle:
        data = sized(handle.read(), width, height)
    with tempfile.NamedTemporaryFile(prefix=".canon-", suffix=".svg", dir=os.path.dirname(svg)) as copy:
        copy.write(data)
        copy.flush()
        yield copy.name


def inkscape(program, shrinker, jobs, side):
    enlarged(jobs, "inkscape")
    with tempfile.TemporaryDirectory() as work, contextlib.ExitStack() as stack:
        large = [os.path.join(work, f"{index}.png") for index in range(len(jobs))]
        sources = [stack.enter_context(inkscape_viewport(svg, width, height)) for svg, _, width, height, _ in jobs]
        script = "".join(
            f"file-open:{svg}; export-type:png; export-filename:{big}; export-width:{width * scale}; export-height:{height * scale}; "
            "export-png-color-mode:RGBA_8; export-background-opacity:0; export-do; file-close\n"
            for (_, png, width, height, scale), svg, big in zip(jobs, sources, large)
        )
        subprocess.run([program, "--shell"], input=script + "quit\n", text=True, check=True,
                       stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
        for (svg, png, width, height, scale), big in zip(jobs, large):
            if os.path.exists(big) and size(big) == (width * scale, height * scale):
                shrink(shrinker, big, png, scale)
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip().splitlines()[0]


def inkscape_legacy(program, shrinker, jobs, side):
    # Inkscape 1.2 retains SVG 1.1 backgrounds but predates export-png-color-mode.
    enlarged(jobs, "inkscape_legacy")
    with tempfile.TemporaryDirectory() as work:
        large = os.path.join(work, "large.png")
        for svg, png, width, height, scale in jobs:
            with inkscape_viewport(svg, width, height) as source:
                subprocess.run([program, source, "--export-type=png", "--export-filename=" + large,
                                f"--export-width={width * scale}", f"--export-height={height * scale}",
                                "--export-background-opacity=0"], check=True,
                               stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if size(large) != (width * scale, height * scale):
                sys.exit(f"{svg}: incorrect Inkscape export size")
            shrink(shrinker, large, png, scale)
            os.remove(large)
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip().splitlines()[0]


def softlight_source(data):
    # Reviewed CANON cases must have an opaque backdrop and no isolating
    # ancestors. Then a final BackgroundImage blend equals CSS blending of the
    # preceding filter result. The old feBlend parser omits soft-light, while
    # its CSS parser and Cairo compositor implement it.
    root = ET.fromstring(data)
    namespace = "{http://www.w3.org/2000/svg}"
    parents = {child: parent for parent in root.iter() for child in parent}
    changed = set()
    for node in root.iter():
        if node.tag == namespace + "style":
            sys.exit("soft-light normalization requires presentation attributes or inline styles")
        if node.tag == namespace + "filter" and len(node):
            last = node[-1]
            if last.tag == namespace + "feBlend" and last.get("mode") == "soft-light":
                if last.get("in2") != "BackgroundImage" or last.get("in") or len(node) < 2:
                    sys.exit("soft-light normalization requires a final blend of the preceding result")
                changed.add(node.get("id"))
                node.remove(last)
    references = set()
    for node in root.iter():
        style = dict(part.split(":", 1) for part in node.get("style", "").split(";") if ":" in part)
        style = {key.strip(): value.strip() for key, value in style.items()}
        match = re.fullmatch(r"url\(#([^)]*)\)", style.get("filter", node.get("filter", "")))
        if match and match[1] in changed:
            if style.get("mix-blend-mode", "normal") != "normal":
                sys.exit("soft-light normalization cannot replace an existing blend mode")
            parent = parents.get(node)
            while parent is not None:
                properties = parent.attrib | dict(part.split(":", 1) for part in parent.get("style", "").split(";") if ":" in part)
                if any(properties.get(key, default).strip() != default for key, default in
                       [("opacity", "1"), ("filter", "none"), ("mask", "none"), ("mix-blend-mode", "normal"), ("isolation", "auto")]):
                    sys.exit("soft-light normalization requires non-isolating ancestors")
                parent = parents.get(parent)
            node.set("style", node.get("style", "") + ";mix-blend-mode:soft-light")
            references.add(match[1])
    if not changed or references != changed:
        sys.exit("soft-light normalization did not resolve every changed filter")
    return ET.tostring(root)


def inkscape_softlight(program, shrinker, jobs, side):
    with contextlib.ExitStack() as stack:
        copies = []
        for svg, png, width, height, scale in jobs:
            copy = stack.enter_context(tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)))
            with open(svg, "rb") as source:
                copy.write(softlight_source(source.read()))
            copy.flush()
            copies.append((copy.name, png, width, height, scale))
        return inkscape_legacy(program, shrinker, copies, side)


def batik(program, shrinker, jobs, side):
    enlarged(jobs, "batik")
    command = [os.environ.get("JAVA", "java"), "-Djava.awt.headless=true", "-Xmx4g", "-jar", program]
    with tempfile.TemporaryDirectory() as work:
        large = os.path.join(work, "large.png")
        for svg, png, width, height, scale in jobs:
            with inkscape_viewport(svg, width, height) as source:
                subprocess.run(command + ["-w", str(width * scale), "-h", str(height * scale),
                                "-d", large, source], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if size(large) != (width * scale, height * scale):
                sys.exit(f"{svg}: incorrect Batik export size")
            shrink(shrinker, large, png, scale)
            os.remove(large)
    with zipfile.ZipFile(program) as jar:
        manifest = jar.read("META-INF/MANIFEST.MF").decode()
    version = re.search(r"^Implementation-Version: (.+)$", manifest, re.M)
    if not version:
        sys.exit("Batik jar has no implementation version")
    return "Apache Batik " + version[1].strip()


def resvg(program, shrinker, jobs, side):
    enlarged(jobs, "resvg")
    with tempfile.TemporaryDirectory() as work:
        big = os.path.join(work, "large.png")
        for svg, png, width, height, scale in jobs:
            with open(svg, "rb") as handle:
                data = sized(handle.read(), width * scale, height * scale)
            with tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)) as copy:
                copy.write(data)
                copy.flush()
                subprocess.run([program, copy.name, big], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
            if size(big) != (width * scale, height * scale):
                sys.exit(f"{png}: resvg drew {size(big)}, not {width * scale}x{height * scale}")
            shrink(shrinker, big, png, scale)
    return "resvg " + subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip()


def firefox(program, shrinker, jobs, side):
    with tempfile.TemporaryDirectory() as work:
        profile = os.path.join(work, "profile")
        os.mkdir(profile)
        for svg, png, width, height, scale in jobs:
            shots = []
            for background in ("white", "black"):
                page = os.path.join(work, background + ".html")
                shot = os.path.join(work, background + ".png")
                small = os.path.join(work, background + ".small.png")
                with open(page, "w") as handle:
                    handle.write(f"<!doctype html><html><head><style>html,body{{margin:0;padding:0;background:{background};overflow:hidden;width:100%;height:100%}}"
                                 f"img{{display:block;transform:scale({scale});transform-origin:0 0}}</style></head>"
                                 f"<body><img src=\"file://{svg}\" style=\"width:{width}px;height:{height}px\"></body></html>")
                subprocess.run([program, "--headless", "--no-remote", "--profile", profile, f"--window-size={width * scale},{height * scale}",
                                "--screenshot", shot, "file://" + page], check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                if size(shot) != (width * scale, height * scale):
                    sys.exit(f"{png}: firefox drew {size(shot)}, not {width * scale}x{height * scale}")
                shrink(shrinker, shot, small, scale)
                shots.append(decode(small))
            encode(png, width, height, unblend(shots[0][2], shots[1][2]))
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip()


def ours(program, shrinker, jobs, side):
    for svg, png, width, height, scale in jobs:
        subprocess.run([program, "render", svg, png, str(width), str(height), str(side)], check=True)
    source = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    commit = subprocess.run(["git", "-C", source, "rev-parse", "--short", "HEAD"], capture_output=True, text=True).stdout.strip()
    return f"svg at {commit}"


def firefox_svg11(program, shrinker, jobs, side):
    # Preserve the corpus source; render an equivalent temporary SVG using
    # ordinary painting and explicit backdrop snapshots for SVG 1.1 inputs.
    with contextlib.ExitStack() as stack:
        copies = []
        for svg, png, width, height, scale in jobs:
            with open(svg, "rb") as source:
                data = source.read()
            if any(node.get("in") in ("FillPaint", "StrokePaint") for node in ET.fromstring(data).iter()):
                data = svg11_reference.source(data)
            data = svg11_reference.backgrounds(data)
            copy = stack.enter_context(tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)))
            copy.write(data)
            copy.flush()
            copies.append((copy.name, png, width, height, scale))
        return firefox(program, shrinker, copies, side)


def chromium_noise(program, shrinker, jobs, side, background_blend=False, local_filters=False):
    import reference
    with tempfile.TemporaryDirectory() as work:
        page = os.path.join(work, "harness.html")
        with open(page, "w") as handle:
            handle.write(reference.HARNESS)
        browser = reference.Browser(program, os.path.join(work, "profile"))
        try:
            browser.open(page)

            def draw(svg, width, height, scale, png):
                browser.send("Emulation.setDeviceMetricsOverride", {"width": width, "height": height,
                             "deviceScaleFactor": scale, "mobile": False})
                if not browser.evaluate(reference.SHOW % (width, height, json.dumps("file://" + svg))):
                    sys.exit(f"{svg}: Chromium could not load the normalized SVG")
                result = browser.send("Page.captureScreenshot", {"format": "png",
                                      "clip": {"x": 0, "y": 0, "width": width, "height": height, "scale": 1},
                                      "fromSurface": True, "captureBeyondViewport": False})
                with open(png, "wb") as handle:
                    handle.write(base64.b64decode(result["data"]))
                if size(png) != (width * scale, height * scale):
                    sys.exit(f"{svg}: incorrect Chromium screenshot size")

            # Refuse to apply the compensation if a browser update changes the
            # sampling convention. At integer lattice vertices noise is exactly
            # zero, or one half for fractalNoise, independent of the seed.
            probe = os.path.join(work, "lattice.svg")
            large = os.path.join(work, "large.png")
            with open(probe, "wb") as handle:
                handle.write(noise_reference.source(b'''<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="-.5 -.5 32 32">
<filter id="f" color-interpolation-filters="sRGB"><feTurbulence type="fractalNoise" baseFrequency="1" seed="7" numOctaves="7"/></filter>
<rect x="-32" y="-32" width="96" height="96" filter="url(#f)"/></svg>'''))
            draw(probe, 32, 32, 1, large)
            _, _, pixels = decode(large)
            for x, y in [(9, 9), (12, 12), (15, 15)]:
                if any(abs(value - 128) > 1 for value in pixels[(y * 32 + x) * 4:(y * 32 + x) * 4 + 4]):
                    sys.exit("Chromium noise sampling differs from the reviewed compensation")

            for svg, png, width, height, scale in jobs:
                with open(svg, "rb") as handle:
                    data = handle.read()
                if background_blend:
                    data = noise_reference.background_blend(data)
                if local_filters:
                    def rasterize(data, rw, rh):
                        with tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)) as source:
                            source.write(data)
                            source.flush()
                            draw(source.name, rw, rh, 1, large)
                        with open(large, "rb") as handle:
                            return handle.read()
                    data = local_filter_reference.source(data, width * scale, height * scale, rasterize)
                else:
                    data = noise_reference.source(data)
                with tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)) as copy:
                    copy.write(data)
                    copy.flush()
                    draw(copy.name, width, height, scale, large)
                shrink(shrinker, large, png, scale)
        finally:
            browser.close()
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip()


def chromium_noise_blend(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, background_blend=True)


def chromium_noise_local(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, local_filters=True)


def main():
    if len(sys.argv) != 5 or sys.argv[1] not in RENDERERS:
        sys.exit("usage: tst/canon.py " + "|".join(RENDERERS) + " PROGRAM SVG CORPUS_DIR")
    renderer, program, shrinker, corpus = sys.argv[1], sys.argv[2], os.path.abspath(sys.argv[3]), os.path.abspath(sys.argv[4])
    with open(os.path.join(corpus, "SIDE")) as handle:
        side = int(handle.read())
    jobs = []
    with open(os.path.join(corpus, "CANON")) as handle:
        for line in handle:
            fields = [field.strip() for field in line.split(":", 2)]
            if len(fields) != 3:
                continue
            engine, separator, resolution = fields[1].partition("@")
            if engine == renderer:
                render_side = int(resolution) if separator else side
                if render_side <= 0 or (separator and renderer == "ours"):
                    sys.exit(f"{fields[0]}: invalid reference resolution {fields[1]}")
                for label in LABELS:
                    png = os.path.join(corpus, f"{fields[0]}.{label}.png")
                    width, height = size(png)
                    jobs.append((os.path.join(corpus, fields[0] + ".svg"), png, width, height, factor(render_side, width, height)))
    if not jobs:
        sys.exit(f"no cases for {renderer} in {os.path.join(corpus, 'CANON')}")
    for _, png, _, _, _ in jobs:
        os.rename(png, png + ".chromium")
    try:
        version = globals()[renderer](program, shrinker, jobs, side)
        missing = [png for _, png, width, height, _ in jobs if not os.path.exists(png) or size(png) != (width, height)]
        if missing:
            sys.exit(f"{renderer} did not draw " + " ".join(missing))
    finally:
        for _, png, _, _, _ in jobs:
            if os.path.exists(png):
                os.remove(png + ".chromium")
            else:
                os.rename(png + ".chromium", png)
    with open(os.path.join(corpus, renderer.upper()), "w") as handle:
        handle.write(version + "\n")
    print(f"drew {len(jobs) // len(LABELS)} cases with {version}")


if __name__ == "__main__":
    main()
