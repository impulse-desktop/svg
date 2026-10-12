#!/usr/bin/env python3
import contextlib
import base64
import json
import math
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
import stitch_reference
import lighting_reference
import stroke_reference
import halfturn_reference
import curve_reference
import wide_stroke_reference
import source_geometry_reference
import edge_reference

LABELS = ["1x", "2x"]
RENDERERS = ["inkscape", "inkscape_curves", "inkscape_edges", "inkscape_outline", "inkscape_legacy", "inkscape_softlight", "inkscape_noise", "batik", "resvg", "firefox", "firefox_svg11", "chromium_morphology", "chromium_lighting", "chromium_wide_stroke", "chromium_noise", "chromium_noise_inkscape", "skia_noise_inkscape", "skia_layers_inkscape", "skia_legacy_stencil", "skia_emboss_inkscape", "skia_textures", "chromium_noise_halfturn", "chromium_noise_blend", "chromium_noise_local", "chromium_stitch", "ours"]
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


def inkscape_curves(program, shrinker, jobs, side):
    with tempfile.TemporaryDirectory(prefix='svg-curve-reference-') as work:
        controls = []
        expected = []
        for index, (data, area) in enumerate(curve_reference.probes()):
            svg, png = os.path.join(work, f'{index}.svg'), os.path.join(work, f'{index}.png')
            with open(svg, 'w') as handle:
                handle.write(data)
            controls.append((svg, png, 192, 128, 1))
            expected.append(area)
        inkscape(program, shrinker, controls, 192)
        for (_, png, _, _, _), area in zip(controls, expected):
            width, height, pixels = decode(png)
            if (width, height) != (192, 128):
                sys.exit('incorrect Inkscape curve control size')
            actual = sum(pixels[3::4]) / 255
            if abs(actual - area) > 1.25:
                sys.exit(f'Inkscape cubic band area {actual} differs from {area}: {png}')
    return inkscape(program, shrinker, jobs, side)


def inkscape_edges(program, shrinker, jobs, side):
    with tempfile.TemporaryDirectory(prefix='svg-edge-reference-') as work:
        controls, expected = [], []
        for index, (data, scale, alpha) in enumerate(edge_reference.probes()):
            svg, png = os.path.join(work, f'{index}.svg'), os.path.join(work, f'{index}.png')
            with open(svg, 'w') as handle:
                handle.write(data)
            controls.append((svg, png, 32, 32, scale))
            expected.append(alpha)
        inkscape(program, shrinker, controls, side)
        for (_, png, _, _, _), alpha in zip(controls, expected):
            width, height, pixels = decode(png)
            if (width, height) != (32, 32):
                sys.exit('incorrect Inkscape edge control size')
            for y in range(height):
                for x in range(width):
                    offset = (y * width + x) * 4
                    if any(abs(a - b) > 2 for a, b in zip(pixels[offset:offset + 4], [255, 0, 0, alpha[y]])):
                        sys.exit(f'Inkscape fractional edge coverage differs at {x},{y}: {png}')
    return inkscape(program, shrinker, jobs, side)


def inkscape_outline(program, shrinker, jobs, side):
    with tempfile.TemporaryDirectory(prefix='svg-outline-reference-') as work:
        sources = {}
        outlined = []
        for svg, png, width, height, scale in jobs:
            if svg not in sources:
                target = os.path.join(work, f'case{len(sources)}.svg')
                with open(svg, 'rb') as handle:
                    data = stroke_reference.source(handle.read(), program)
                with open(target, 'wb') as handle:
                    handle.write(data)
                sources[svg] = target
            outlined.append((sources[svg], png, width, height, scale))

        # A straight stroke with butt caps has area length * width, at every
        # angle. Hairline shortcuts in rasterizers need not preserve that area.
        controls = []
        for thickness in [0.25, 0.5]:
            for angle in [0, 30, 45, 60, 90]:
                data = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 128 128">
<path d="M24 64H104" fill="none" stroke="black" stroke-width="{thickness}"
      transform="rotate({angle} 64 64)"/></svg>'''.encode()
                target = os.path.join(work, f'control{thickness}-{angle}.svg')
                with open(target, 'wb') as handle:
                    handle.write(stroke_reference.source(data, program))
                for scale in [1, 2]:
                    png = target + f'.{scale}.png'
                    outlined.append((target, png, 128, 128, scale))
                    controls.append((png, 80 * thickness))
        version = inkscape(program, shrinker, outlined, side)
        for png, expected in controls:
            _, _, rgba = decode(png)
            area = sum(rgba[3::4]) / 255
            # Allow raster coverage/8-bit rounding; the unoutlined Chromium
            # diagonal loses over five square pixels even at width 0.25.
            if abs(area - expected) > 1:
                sys.exit(f'Inkscape stroke outline area {area} differs from {expected}: {png}')
    return version


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


def inkscape_noise(program, shrinker, jobs, side):
    with tempfile.TemporaryDirectory() as work, contextlib.ExitStack() as stack:
        probe = os.path.join(work, "probe.svg")
        png = os.path.join(work, "probe.png")
        data = b'''<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="-.5 -.5 32 32">
          <filter id="f" color-interpolation-filters="sRGB">
            <feTurbulence type="fractalNoise" baseFrequency="1" seed="7" numOctaves="3"/>
          </filter><rect x="-32" y="-32" width="96" height="96" filter="url(#f)"/></svg>'''
        with open(probe, "wb") as handle:
            handle.write(noise_reference.source(data, phase=-.5))
        inkscape(program, shrinker, [(probe, png, 32, 32, 1)], 32)
        _, _, pixels = decode(png)
        for x, y in [(9, 9), (12, 12), (15, 15)]:
            if any(abs(value - 128) > 1 for value in pixels[(y*32+x)*4:(y*32+x)*4+4]):
                sys.exit("Inkscape noise sampling differs from the reviewed compensation")
        copies = []
        for svg, png, width, height, scale in jobs:
            with open(svg, "rb") as handle:
                data = noise_reference.inkscape_source(handle.read(), width*scale, height*scale)
            source = stack.enter_context(tempfile.NamedTemporaryFile(suffix=".svg", dir=os.path.dirname(svg)))
            source.write(data)
            source.flush()
            copies.append((source.name, png, width, height, scale))
        return inkscape(program, shrinker, copies, side)


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


def morphology_source(data):
    """Make the input clip explicit for reviewed single-primitive erosions."""
    root = ET.fromstring(data)
    ns = noise_reference.NS
    count = 0
    for f in root.iter(ns + "filter"):
        if len(f) != 1 or f[0].tag != ns + "feMorphology":
            continue
        p = f[0]
        if p.get("operator", "erode") != "erode":
            continue
        if (p.get("in", "SourceGraphic") not in ["SourceGraphic", "SourceAlpha"]
                or any(k in p.attrib for k in ["x", "y", "width", "height", "style", "color-interpolation-filters"])):
            raise ValueError("erosion requires a standard input and the default primitive region")
        f.insert(0, ET.Element(ns + "feOffset", {"in": p.get("in", "SourceGraphic")}))
        p.attrib.pop("in", None)
        count += 1
    if not count:
        raise ValueError("no single-primitive source erosion to normalize")
    return ET.tostring(root)


def chromium_normalized(program, shrinker, jobs, normalize, check, viewport=False):
    import reference
    with tempfile.TemporaryDirectory() as work:
        page = os.path.join(work, "harness.html")
        with open(page, "w") as handle:
            handle.write(reference.HARNESS)
        large = os.path.join(work, "large.png")
        browser = reference.Browser(program, os.path.join(work, "profile"))
        try:
            browser.open(page)

            def draw(data, width, height, scale, directory=work):
                with tempfile.NamedTemporaryFile(suffix=".svg", dir=directory) as source:
                    source.write(normalize(data, width, height, scale) if viewport else normalize(data))
                    source.flush()
                    browser.send("Emulation.setDeviceMetricsOverride", {"width": width, "height": height,
                                 "deviceScaleFactor": scale, "mobile": False})
                    if not browser.evaluate(reference.SHOW % (width, height, json.dumps("file://" + source.name))):
                        sys.exit("Chromium could not load the normalized SVG")
                    shot = browser.send("Page.captureScreenshot", {"format": "png",
                                        "clip": {"x": 0, "y": 0, "width": width, "height": height, "scale": 1},
                                        "fromSurface": True, "captureBeyondViewport": False})
                    with open(large, "wb") as handle:
                        handle.write(base64.b64decode(shot["data"]))
                if size(large) != (width * scale, height * scale):
                    sys.exit("incorrect Chromium screenshot size")
                return large

            check(draw)
            for svg, png, width, height, scale in jobs:
                with open(svg, "rb") as handle:
                    draw(handle.read(), width, height, scale, os.path.dirname(svg))
                shrink(shrinker, large, png, scale)
        finally:
            browser.close()
    return subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip()


def chromium_wide_stroke(program, shrinker, jobs, side):
    def check(draw):
        for index, (data, width, height, scale) in enumerate(wide_stroke_reference.probes()):
            w, h, pixels = decode(draw(data, width, height, scale))
            # These centers lie strictly inside the analytically covered fill,
            # even when the original shape masks its stroke to the interior.
            if any(tuple(pixels[(y*w+x)*4:(y*w+x)*4+4]) != (255, 0, 0, 255)
                   for y in range(h//2 - 1, h//2 + 1) for x in range(w//2 - 1, w//2 + 1)):
                sys.exit(f"Chromium wide-stroke normalization left an uncovered center in probe {index}")
    return chromium_normalized(program, shrinker, jobs, wide_stroke_reference.source, check, viewport=True)


def chromium_morphology(program, shrinker, jobs, side):
    def check(draw):
        # A clipped square eroded by four units loses four units on each side.
        probe = b'''<svg xmlns="http://www.w3.org/2000/svg" width="64" height="64" viewBox="0 0 64 64">
<filter id="f" filterUnits="userSpaceOnUse" x="16" y="16" width="32" height="32">
<feMorphology operator="erode" radius="4"/></filter>
<rect width="64" height="64" filter="url(#f)"/></svg>'''
        _, _, pixels = decode(draw(probe, 64, 64, 1))
        if any(pixels[(y*64+x)*4+3] != (255 if 20 <= x < 44 and 20 <= y < 44 else 0)
               for y in range(64) for x in range(64)):
            sys.exit("Chromium erosion no longer matches the reviewed input clipping")
    return chromium_normalized(program, shrinker, jobs, morphology_source, check)


def chromium_lighting(program, shrinker, jobs, side):
    def check(draw):
        probe = b'''<svg xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink"
                        width="64" height="128" viewBox="0 0 64 128">
<defs><filter id="f" x="0" y="0" width="1" height="1" color-interpolation-filters="sRGB">
<feSpecularLighting surfaceScale="18" specularConstant="1.1" specularExponent="5">
<feDistantLight azimuth="235" elevation="55"/></feSpecularLighting></filter></defs>
<path id="p" d="M 16 16 L 48 16 L 48 48 L 16 48 Z" fill="black" filter="url(#f)"/>
<use xlink:href="#p" transform="matrix(1 0 0 -1 0 128)"/></svg>'''
        # A flat normal dotted with the half-vector gives sqrt((1+sin(e))/2).
        expected = round(255 * 1.1 * ((1 + math.sin(math.radians(55))) / 2) ** 2.5)
        for scale in [1, 2]:
            width, _, pixels = decode(draw(probe, 64, 128, scale))
            for x, y in [(24, 24), (40, 40), (24, 104), (40, 88)]:
                offset = (y*scale*width + x*scale) * 4
                if any(abs(a-b) > 1 for a, b in zip(pixels[offset:offset+4], [255, 255, 255, expected])):
                    sys.exit("Chromium reflected lighting differs from the calculated flat surface")
    return chromium_normalized(program, shrinker, jobs, lighting_reference.source, check)


def chromium_noise(program, shrinker, jobs, side, background_blend=False, local_filters=False, stitched_noise=False, halfturn_lighting=False, inkscape_geometry=False, float_filters=False):
    import reference
    versions = []
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

            if float_filters:
                import affine_lighting_reference
                import float_reference
                import layer_reference
                versions.extend([layer_reference.check(), affine_lighting_reference.check()])
                float_cache = {}

                def draw(svg, width, height, scale, png):
                    with open(svg, 'rb') as handle:
                        data = handle.read()
                    key = (data, width*scale, height*scale)
                    if key not in float_cache:
                        float_cache[key] = float_reference.render(*key)
                    encode(png, width*scale, height*scale, float_cache[key])

            if inkscape_geometry:
                ink = os.environ.get('INKSCAPE', 'inkscape')
                versions.append(inkscape_curves(ink, shrinker, [], side))
                source_cache = {}

                def measure_source(data, name):
                    return browser.evaluate('''(() => {
                        const doc = new DOMParser().parseFromString(%s, 'image/svg+xml');
                        document.body.replaceChildren(document.importNode(doc.documentElement, true));
                        const b = document.getElementById(%s).getBBox();
                        return [b.x, b.y, b.width, b.height];
                    })()''' % (json.dumps(data.decode()), json.dumps(name)))

                def rasterize_source(data, width, height):
                    key = (data, width, height)
                    if key not in source_cache:
                        svg, png = os.path.join(work, 'source.svg'), os.path.join(work, 'source.png')
                        with open(svg, 'wb') as handle:
                            handle.write(data)
                        if os.path.exists(png):
                            os.remove(png)
                        inkscape(ink, shrinker, [(svg, png, width, height, 1)], side)
                        if size(png) != (width, height):
                            sys.exit('incorrect Inkscape source snapshot size')
                        with open(png, 'rb') as handle:
                            source_cache[key] = handle.read()
                    return source_cache[key]

                def blue_source(data, width, height):
                    png = os.path.join(work, 'blue.png')
                    pixels = bytes(v for y in range(height) for x in range(width)
                                   for v in [0, 0, 255, 64 if (x+y) % 2 else 192])
                    encode(png, width, height, pixels)
                    with open(png, 'rb') as handle:
                        return handle.read()

                # Deliberately give the snapshot a different paint and alpha
                # from the original path. The alpha checker also detects
                # resampling or a shifted grid after source replacement.
                for alpha_only in [False, True]:
                    end = ('<feColorMatrix in="SourceAlpha"/>' if alpha_only else
                           '<feComposite in="SourceGraphic" in2="SourceAlpha" operator="in"/>')
                    for x, y in [(0, 0), (12.3, 7.7)]:
                        data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32">'
                                f'<defs><filter id="f" color-interpolation-filters="sRGB">'
                                f'<feTurbulence numOctaves="0"/>{end}</filter></defs>'
                                f'<g id="g" transform="translate({-x} {-y})" filter="url(#f)">'
                                f'<path fill="red" d="M{x} {y}L{x+32} {y}L{x+32} {y+32}L{x} {y+32}Z"/></g></svg>').encode()
                        for scale in [1, 2]:
                            probe = os.path.join(work, f'source-{alpha_only}-{x}-{scale}.svg')
                            with open(probe, 'wb') as handle:
                                handle.write(source_geometry_reference.source(data, 31, 32, scale, measure_source, blue_source))
                            draw(probe, 31, 32, scale, large)
                            width, height, pixels = decode(large)
                            for offset in range(0, len(pixels), 4):
                                px, py = (offset//4) % width, (offset//4) // width
                                alpha = 64 if (px+py) % 2 else 192
                                expected = [0, 0, 0, alpha] if alpha_only else [0, 0, 255, round(alpha*alpha/255)]
                                if any(abs(a-b) > 1 for a, b in zip(pixels[offset:offset+4], expected)):
                                    sys.exit(f'source snapshot input mapping changed at {px},{py}')

            if halfturn_lighting:
                for kind in ['Diffuse', 'Specular']:
                    for flat in [False, True]:
                        data = f'''<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 256 256">
<defs><linearGradient id="r" gradientUnits="userSpaceOnUse" x1="64" x2="192">
<stop stop-color="white" stop-opacity="0"/><stop offset="1" stop-color="white"/></linearGradient>
<filter id="f" color-interpolation-filters="sRGB"><feTurbulence numOctaves="0"/>
<fe{kind}Lighting in="SourceGraphic" surfaceScale="32" specularExponent="8">
<feDistantLight azimuth="30" elevation="50"/></fe{kind}Lighting></filter></defs>
<path d="M64 64H192V192H64Z" fill="{'white' if flat else 'url(#r)'}"
      filter="url(#f)" transform="matrix(-1 0 0 -1 256 256)"/></svg>'''.encode()
                        lighting_probe = os.path.join(work, f'lighting-{kind}-{flat}.svg')
                        with open(lighting_probe, 'wb') as handle:
                            handle.write(noise_reference.source(halfturn_reference.source(data)))
                        nx = 0 if flat else -0.5
                        lx = math.cos(math.radians(30)) * math.cos(math.radians(50))
                        lz = math.sin(math.radians(50))
                        expected = ((nx*lx + lz) / math.sqrt(1 + nx*nx) if kind == 'Diffuse' else
                                    ((nx*lx + lz + 1) / math.sqrt((1 + nx*nx) * (2 + 2*lz))) ** 8) * 255
                        for scale in [1, 2]:
                            draw(lighting_probe, 256, 256, scale, large)
                            _, _, rgba = decode(large)
                            channel = 0 if kind == 'Diffuse' else 3
                            values = [rgba[(y*256*scale+x)*4+channel]
                                      for y in range(112*scale, 144*scale) for x in range(112*scale, 144*scale)]
                            actual = sum(values) / len(values)
                            if abs(actual - expected) > 2:
                                sys.exit(f'Chromium half-turn {kind}/{flat}/{scale}: {actual}, expected {expected}')

            if stitched_noise:
                jar_path = os.environ.get("BATIK")
                if not jar_path:
                    sys.exit("chromium_stitch requires BATIK to name the stock rasterizer jar")
                versions.append(batik(jar_path, shrinker, [], side))

                def rasterize_stitch(data, rw, rh):
                    source = os.path.join(work, "noise.svg")
                    png = os.path.join(work, "noise.png")
                    with open(source, "wb") as handle:
                        handle.write(data)
                    if os.path.exists(png):
                        os.remove(png)
                    subprocess.run([os.environ.get("JAVA", "java"), "-Xmx4g", "-Djava.awt.headless=true",
                                    "-jar", jar_path, "-w", str(rw), "-h", str(rh), "-d", png, source],
                                   check=True, stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                    if size(png) != (rw, rh):
                        sys.exit("incorrect Batik noise export size")
                    with open(png, "rb") as handle:
                        return handle.read()

                primitive = ET.Element(stitch_reference.NS + "feTurbulence", {
                    "type": "fractalNoise", "baseFrequency": "1", "seed": "7", "stitchTiles": "stitch"})
                probe_data = stitch_reference.noise_svg(primitive, (-.5, -.5, 32, 32), (-.5, -.5, 32, 32), 1, "sRGB")
                with open(large, "wb") as handle:
                    handle.write(rasterize_stitch(probe_data, 32, 32))
                _, _, pixels = decode(large)
                for x, y in [(9, 9), (12, 12), (15, 15)]:
                    # Batik truncates to bytes and premultiplies them, losing
                    # up to two levels on this exact half-alpha lattice value.
                    if any(abs(value - 128) > 2 for value in pixels[(y*32+x)*4:(y*32+x)*4+4]):
                        sys.exit("Batik noise sampling differs from the reviewed compensation")

            for svg, png, width, height, scale in jobs:
                with open(svg, "rb") as handle:
                    data = handle.read()
                if halfturn_lighting:
                    data = halfturn_reference.source(data)
                if background_blend:
                    data = noise_reference.background_blend(data)
                if inkscape_geometry:
                    data = source_geometry_reference.source(data, width, height, scale, measure_source, rasterize_source)
                elif stitched_noise:
                    def measure(data):
                        script = '''(async () => {
                            const doc = new DOMParser().parseFromString(%s, 'image/svg+xml');
                            document.body.replaceChildren(document.importNode(doc.documentElement, true));
                            const svg = document.body.firstElementChild;
                            svg.setAttribute('width', %d); svg.setAttribute('height', %d);
                            await document.fonts.ready;
                            const inverse = svg.getCTM().inverse(), out = {};
                            for (const n of svg.querySelectorAll('[data-canon-stitch]')) {
                                const id = n.getAttribute('data-canon-stitch'), b = n.getBBox();
                                const m = inverse.multiply(n.getCTM());
                                const f = document.getElementById(id);
                                out[id] = {bbox: [b.x,b.y,b.width,b.height], matrix: [m.a,m.b,m.c,m.d,m.e,m.f],
                                           space: getComputedStyle(f.firstElementChild).colorInterpolationFilters};
                            }
                            return out;
                        })()''' % (json.dumps(data.decode()), width*scale, height*scale)
                        return browser.evaluate(script)
                    data = stitch_reference.source(data, width*scale, height*scale, measure, rasterize_stitch)
                elif local_filters:
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
    versions.insert(0, subprocess.run([program, "--version"], capture_output=True, text=True).stdout.strip())
    return "; ".join(versions)


def skia_layers_inkscape(program, shrinker, jobs, side, legacy=False, emboss=False):
    import affine_lighting_reference
    import layer_reference
    import reference
    version = layer_reference.check()
    numpy_version = affine_lighting_reference.check()
    legacy_ink = None
    if legacy:
        import component_reference
        import stencil_reference
        component_reference.check()
        legacy_ink = os.environ['INKSCAPE_LEGACY']
    if emboss:
        import emboss_reference
    ink = os.environ.get('INKSCAPE', 'inkscape')
    ink_version = inkscape_curves(ink, shrinker, [], side)
    with tempfile.TemporaryDirectory(prefix='svg-layer-reference-') as work:
        controls = []
        for thickness in [.25, .5, .75]:
            for angle in [0, 30, 45, 60, 90]:
                svg = os.path.join(work, f'stroke-{thickness}-{angle}.svg')
                with open(svg, 'w') as handle:
                    handle.write(f'<svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 128 128">'
                                 f'<path d="M24 64H104" fill="none" stroke="black" stroke-width="{thickness}" '
                                 f'transform="rotate({angle} 64 64)"/></svg>')
                for scale in [1, 2]:
                    png = svg+f'.{scale}.png'
                    controls.append(((svg, png, 128, 128, scale), 80*thickness))
        inkscape(ink, shrinker, [job for job, _ in controls], side)
        for (_, png, _, _, _), expected in controls:
            _, _, pixels = decode(png)
            actual = sum(pixels[3::4])/255
            # Cairo's 0.75-unit diagonal loses about 1.64 square pixels;
            # allow coverage quantization while guarding the much larger hairline loss.
            if abs(actual-expected) > 2:
                sys.exit(f'Inkscape layer stroke area {actual} differs from {expected}: {png}')
        page = os.path.join(work, 'harness.html')
        with open(page, 'w') as handle:
            handle.write(reference.HARNESS)
        browser = reference.Browser(program, os.path.join(work, 'profile'))
        try:
            browser.open(page)

            def measure(data, names):
                return browser.evaluate('''(() => {
                    const doc = new DOMParser().parseFromString(%s, 'image/svg+xml');
                    const svg = document.importNode(doc.documentElement, true);
                    svg.style.position = 'absolute'; svg.style.left = '-100000px';
                    document.body.replaceChildren(svg);
                    const inverse = svg.getCTM().inverse(), out = {};
                    for (const name of %s) {
                        const n = document.getElementById(name), b = n.getBBox();
                        const m = inverse.multiply(n.getCTM());
                        out[name] = {bbox: [b.x,b.y,b.width,b.height], matrix: [m.a,m.b,m.c,m.d,m.e,m.f]};
                    }
                    document.body.replaceChildren();
                    return out;
                })()''' % (json.dumps(data.decode()), json.dumps(names)))

            def rasterize(data, width, height):
                svg, png = os.path.join(work, 'source.svg'), os.path.join(work, 'source.png')
                with open(svg, 'wb') as handle:
                    handle.write(data)
                if os.path.exists(png):
                    os.remove(png)
                inkscape(ink, shrinker, [(svg, png, width, height, 4 if legacy else 1)], side)
                with open(png, 'rb') as handle:
                    return handle.read()

            def draw(data, width, height, scale, directory=work):
                with tempfile.NamedTemporaryFile(suffix='.svg', dir=directory) as copy:
                    copy.write(data)
                    copy.flush()
                    if legacy or emboss:
                        large = os.path.join(work, 'large.png')
                        with inkscape_viewport(copy.name, width, height) as viewport:
                            subprocess.run([legacy_ink if legacy else ink, viewport, '--export-type=png', '--export-filename='+large,
                                            f'--export-width={width*scale}', f'--export-height={height*scale}',
                                            '--export-background-opacity=0'], check=True,
                                           stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
                        if size(large) != (width*scale, height*scale):
                            sys.exit('incorrect Inkscape layer export size')
                        return large
                    browser.send('Emulation.setDeviceMetricsOverride', {'width': width, 'height': height,
                                 'deviceScaleFactor': scale, 'mobile': False})
                    if not browser.evaluate(reference.SHOW % (width, height, json.dumps('file://'+copy.name))):
                        sys.exit('Chromium could not load the layer snapshots')
                    shot = browser.send('Page.captureScreenshot', {'format': 'png',
                           'clip': {'x': 0, 'y': 0, 'width': width, 'height': height, 'scale': 1},
                           'fromSurface': True, 'captureBeyondViewport': False})
                    large = os.path.join(work, 'large.png')
                    with open(large, 'wb') as handle:
                        handle.write(base64.b64decode(shot['data']))
                    if size(large) != (width*scale, height*scale):
                        sys.exit('incorrect layer screenshot size')
                    return large

            def close_pixel(actual, expected):
                # RGB is undefined at alpha zero; Inkscape keeps white there.
                return (abs(actual[3]-expected[3]) <= 2 and all(
                    abs(actual[i]*actual[3]-expected[i]*expected[3]) <= 2*255 for i in range(3)))

            for x, y in [(0, 0), (12.3, 7.7)]:
                def checker(data, width, height):
                    # Paint a deliberately different source on the device grid.
                    box = [float(v) for v in ET.fromstring(data).get('viewBox').split()]
                    zoom = width/box[2]
                    left, top = round((box[0]+x)*zoom), round((box[1]+y)*zoom)
                    pixels = bytes(v for py in range(height) for px in range(width)
                                   for v in [0, 0, 255, 64 if (px+left+py+top) % 2 else 192])
                    png = os.path.join(work, 'checker.png')
                    encode(png, width, height, pixels)
                    with open(png, 'rb') as handle:
                        return handle.read()

                for alpha_only in [False, True]:
                    end = ('<feColorMatrix in="SourceAlpha"/>' if alpha_only else
                           '<feComposite in="SourceGraphic" in2="SourceAlpha" operator="in"/>')
                    data = (f'<svg xmlns="http://www.w3.org/2000/svg" width="32" height="32" viewBox="0 0 32 32">'
                            f'<defs><filter id="f" color-interpolation-filters="sRGB"><feTurbulence numOctaves="0"/>{end}</filter></defs>'
                            f'<g transform="translate({x} {y})"><path id="p" fill="red" filter="url(#f)" '
                            f'd="M{-x} {-y}h32v32h-32z"/></g></svg>').encode()
                    for scale in [1, 2]:
                        probe = layer_reference.source(data, 32*scale, 32*scale, measure, checker)
                        width, height, pixels = decode(draw(probe, 32, 32, scale))
                        for offset in range(0, len(pixels), 4):
                            px, py = (offset//4) % width, (offset//4) // width
                            alpha = 64 if (px+py) % 2 else 192
                            expected = [0, 0, 0, alpha] if alpha_only else [0, 0, 255, round(alpha*alpha/255)]
                            if any(abs(a-b) > 1 for a, b in zip(pixels[offset:offset+4], expected)):
                                sys.exit(f'layer snapshot grid or source inputs changed at {px},{py}')

            for scale in [1, 2]:
                for label, data, expected in layer_reference.affine_probes(scale):
                    probe = layer_reference.source(data, 128*scale, 128*scale, measure, rasterize, affine=True)
                    width, _, pixels = decode(draw(probe, 128, 128, scale))
                    offset = ((64*scale)*width+64*scale)*4
                    actual = pixels[offset:offset+4]
                    if not close_pixel(actual, expected):
                        sys.exit(f'affine layer export {label}: {list(pixels[offset:offset+4])} != {expected}')

            for scale in [1, 2]:
                for label, data, samples in layer_reference.group_probes(scale):
                    probe = layer_reference.source(data, 96*scale, 64*scale, measure, rasterize, affine=True)
                    width, _, pixels = decode(draw(probe, 96, 64, scale))
                    for x, y, expected in samples:
                        offset = ((y*scale)*width+x*scale)*4
                        actual = pixels[offset:offset+4]
                        if not close_pixel(actual, expected):
                            sys.exit(f'group layer export {label}/{scale}/{x},{y}: {list(pixels[offset:offset+4])} != {expected}')

            if legacy:
                for data, expected in stencil_reference.geometry_probes():
                    rasterize(data, 335, 335)
                    _, _, pixels = decode(os.path.join(work, 'source.png'))
                    actual = sum(pixels[3::4])/255
                    if abs(actual-expected) > 1.25:
                        sys.exit(f'stencil source annulus area {actual} differs from {expected}')
                for data, samples in stencil_reference.background_probes():
                    for scale in [1, 2]:
                        width, _, pixels = decode(draw(data, 32, 32, scale))
                        for x, expected in samples:
                            offset = ((16*scale)*width+x*scale)*4
                            actual = pixels[offset:offset+4]
                            if not close_pixel(actual, expected):
                                sys.exit('legacy layer composition lost its SVG 1.1 background')

            if emboss:
                for label, data, (left, top, right, bottom) in emboss_reference.probes():
                    for scale in [1, 2]:
                        probe = emboss_reference.source(data, 96*scale, 64*scale, measure, rasterize)
                        width, _, pixels = decode(draw(probe, 96, 64, scale))
                        samples = [(x*scale, y*scale) for x, y in [(24,24), (72,24), (12,24), (86,24), (24,12), (72,44)]]
                        samples += [(math.floor(left*scale), 24*scale), (math.floor(right*scale), 24*scale),
                                    (24*scale, math.floor(top*scale)), (72*scale, math.floor(bottom*scale))]
                        for x, y in samples:
                            coverage = (max(0, min(x+1, right*scale)-max(x, left*scale)) *
                                        max(0, min(y+1, bottom*scale)-max(y, top*scale)))
                            expected = [255, 255, 255, round(51*coverage)]
                            offset = (y*width+x)*4
                            if not close_pixel(pixels[offset:offset+4], expected):
                                sys.exit(f'emboss reflection/clip {label}/{scale}/{x},{y}: '
                                         f'{list(pixels[offset:offset+4])} != {expected}')

            cache = {}
            for svg, png, width, height, scale in jobs:
                with open(svg, 'rb') as handle:
                    data = handle.read()
                key = (data, width*scale, height*scale)
                if key not in cache:
                    cache[key] = (stencil_reference.source(*key, measure, rasterize) if legacy else
                                  emboss_reference.source(*key, measure, rasterize) if emboss else
                                  layer_reference.source(*key, measure, rasterize, affine=True))
                large = draw(cache[key], width, height, scale, os.path.dirname(svg))
                shrink(shrinker, large, png, scale)
        finally:
            browser.close()
    chrome_version = subprocess.run([program, '--version'], capture_output=True, text=True).stdout.strip()
    versions = [chrome_version, version, numpy_version, ink_version]
    if legacy:
        versions.append(subprocess.run([legacy_ink, '--version'], capture_output=True, text=True).stdout.strip())
    return '; '.join(versions)


def skia_legacy_stencil(program, shrinker, jobs, side):
    return skia_layers_inkscape(program, shrinker, jobs, side, legacy=True)


def skia_emboss_inkscape(program, shrinker, jobs, side):
    return skia_layers_inkscape(program, shrinker, jobs, side, emboss=True)


def skia_textures(program, shrinker, jobs, side):
    import affine_lighting_reference
    import texture_reference
    import reference
    version = texture_reference.check()
    numpy_version = affine_lighting_reference.check()
    with tempfile.TemporaryDirectory(prefix='svg-texture-reference-') as work:
        page = os.path.join(work, 'harness.html')
        with open(page, 'w') as handle:
            handle.write(reference.HARNESS)
        browser = reference.Browser(program, os.path.join(work, 'profile'))
        try:
            browser.open(page)

            def measure(data, names):
                return texture_reference.measure(browser, data, names)

            def draw(data, width, height, scale, directory=work):
                with tempfile.NamedTemporaryFile(suffix='.svg', dir=directory) as copy:
                    copy.write(data)
                    copy.flush()
                    browser.send('Emulation.setDeviceMetricsOverride', {'width': width, 'height': height,
                                 'deviceScaleFactor': scale, 'mobile': False})
                    if not browser.evaluate(reference.SHOW % (width, height, json.dumps('file://'+copy.name))):
                        sys.exit('Chromium could not load the texture snapshots')
                    shot = browser.send('Page.captureScreenshot', {'format': 'png',
                           'clip': {'x': 0, 'y': 0, 'width': width, 'height': height, 'scale': 1},
                           'fromSurface': True, 'captureBeyondViewport': False})
                    png = os.path.join(work, 'large.png')
                    with open(png, 'wb') as handle:
                        handle.write(base64.b64decode(shot['data']))
                    if size(png) != (width*scale, height*scale):
                        sys.exit('incorrect texture screenshot size')
                    return png

            for scale in [1, 2]:
                for data, expected in texture_reference.probes(scale):
                    probe = texture_reference.source(data, 96*scale, 96*scale, measure)
                    width, height, pixels = decode(draw(probe, 96, 96, scale))
                    for x, y in [(45*scale,45*scale), (51*scale,51*scale)]:
                        offset = (y*width+x)*4
                        if any(abs(a-b) > 1 for a, b in zip(pixels[offset:offset+4], expected)):
                            sys.exit('rotated texture lighting differs from the calculated flat surface')
                    if any(pixels[(y*width+x)*4+3] for x,y in [(0,0), (width-1,height-1)]):
                        sys.exit('texture snapshots lost their ancestor clips')
            cache = {}
            for svg, png, width, height, scale in jobs:
                with open(svg, 'rb') as handle:
                    data = handle.read()
                key = (data, width*scale, height*scale)
                if key not in cache:
                    cache[key] = texture_reference.source(*key, measure)
                large = draw(cache[key], width, height, scale, os.path.dirname(svg))
                shrink(shrinker, large, png, scale)
        finally:
            browser.close()
    chrome = subprocess.run([program, '--version'], capture_output=True, text=True).stdout.strip()
    return '; '.join([chrome, version, numpy_version])


def chromium_noise_blend(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, background_blend=True)


def chromium_noise_halfturn(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, halfturn_lighting=True)


def chromium_noise_inkscape(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, inkscape_geometry=True)


def skia_noise_inkscape(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, inkscape_geometry=True, float_filters=True)


def chromium_noise_local(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, local_filters=True)


def chromium_stitch(program, shrinker, jobs, side):
    return chromium_noise(program, shrinker, jobs, side, stitched_noise=True)


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
