#!/usr/bin/env python3
import base64
import itertools
import json
import os
import re
import select
import shutil
import struct
import subprocess
import sys
import tempfile
import threading
import time

SIDES = [("1x", 256), ("2x", 512)]
WRAPPERS = itertools.count()
TIMEOUT = 20

HARNESS = """<!doctype html>
<html><head><style>html,body{margin:0;padding:0;background:transparent;overflow:hidden}img{display:block}</style></head>
<body></body></html>
"""

LOAD = """
(async () => {
  document.body.textContent = '';
  const img = new Image();
  const done = new Promise((ok) => { img.onload = () => ok(true); img.onerror = () => ok(false); });
  img.src = %s;
  if (!(await done)) return null;
  return [img.naturalWidth, img.naturalHeight];
})()
"""

SHOW = """
(async () => {
  document.body.textContent = '';
  const img = new Image();
  img.style.width = '%dpx';
  img.style.height = '%dpx';
  const done = new Promise((ok) => { img.onload = () => ok(true); img.onerror = () => ok(false); });
  img.src = %s;
  document.body.appendChild(img);
  if (!(await done)) return false;
  await img.decode().catch(() => {});
  await new Promise((ok) => requestAnimationFrame(() => requestAnimationFrame(ok)));
  return true;
})()
"""


class Browser:
    def __init__(self, chromium, profile):
        self.write_r, self.write_w = os.pipe()
        self.read_r, self.read_w = os.pipe()
        self.process = subprocess.Popen(
            [
                "sh", "-c", f'exec "$@" 3<&{self.write_r} 4>&{self.read_w}', "sh",
                chromium, "--headless", "--no-sandbox", "--disable-gpu", "--hide-scrollbars",
                "--force-color-profile=srgb", "--force-device-scale-factor=1", "--no-first-run",
                "--disable-extensions", "--disable-background-networking", "--mute-audio",
                "--allow-file-access-from-files", "--remote-debugging-pipe",
                f"--user-data-dir={profile}", "about:blank",
            ],
            pass_fds=(self.write_r, self.read_w),
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
        os.close(self.write_r)
        os.close(self.read_w)
        self.out = os.fdopen(self.write_w, "wb", buffering=0)
        self.inp = os.fdopen(self.read_r, "rb", buffering=0)
        self.buffer = b""
        self.next_id = 0
        self.session = None

    def send(self, method, params=None, session=True, timeout=60):
        self.deadline = time.monotonic() + timeout
        self.next_id += 1
        message = {"id": self.next_id, "method": method, "params": params or {}}
        if session and self.session:
            message["sessionId"] = self.session
        self.out.write(json.dumps(message).encode() + b"\0")
        while True:
            reply = self.receive()
            if reply.get("id") == self.next_id:
                if "error" in reply:
                    raise RuntimeError(f"{method}: {reply['error']}")
                return reply.get("result", {})

    def receive(self):
        while b"\0" not in self.buffer:
            remaining = self.deadline - time.monotonic()
            if remaining <= 0 or not select.select([self.inp], [], [], remaining)[0]:
                raise RuntimeError("chromium response timed out")
            chunk = self.inp.read(1 << 20)
            if not chunk:
                raise RuntimeError("chromium closed the pipe")
            self.buffer += chunk
        message, self.buffer = self.buffer.split(b"\0", 1)
        return json.loads(message)

    def open(self, harness):
        target = self.send("Target.createTarget", {"url": "about:blank"}, session=False)["targetId"]
        self.session = self.send("Target.attachToTarget", {"targetId": target, "flatten": True}, session=False)["sessionId"]
        self.send("Page.enable")
        self.send("Emulation.setDefaultBackgroundColorOverride", {"color": {"r": 0, "g": 0, "b": 0, "a": 0}})
        self.harness = harness
        self.navigate()

    def navigate(self):
        self.send("Page.navigate", {"url": "file://" + self.harness})
        self.evaluate("new Promise((ok) => document.readyState === 'complete' ? ok() : addEventListener('load', ok))")

    def evaluate(self, expression):
        result = self.send("Runtime.evaluate", {"expression": expression, "awaitPromise": True, "returnByValue": True, "timeout": TIMEOUT * 1000})
        if "exceptionDetails" in result:
            raise RuntimeError(result["exceptionDetails"].get("text", "exception"))
        return result["result"].get("value")

    def close(self):
        try:
            self.send("Browser.close", session=False, timeout=2)
        except Exception:
            pass
        self.process.kill()
        self.process.wait()


def sizes(own):
    width, height = own
    longest = max(width, height)
    for label, side in SIDES:
        scale = side / longest
        yield label, max(1, round(width * scale)), max(1, round(height * scale))


# The document's own size as the renderer has it: what its root gives, or
# the edges of what it draws; and whether the test draws it straight at the
# size of its reference, as a drawing of more pixels is not the drawing
# scaled for it: then the reference is a screenshot at one device pixel a
# CSS pixel too, not supersampled.
def own_size(program, svg):
    line = subprocess.run([program, "size", svg], capture_output=True, text=True, check=True).stdout.split()
    if len(line) < 3 or line[1] == "?":
        return None
    return float(line[1]), float(line[2]), len(line) > 3 and line[3] == "straight"


TAG = re.compile(rb"<[^>]*>", re.S)
ATTRIBUTE = rb"(\s%s\s*=\s*)([\"'])(.*?)\2"
ZOOM = re.compile(rb"(^|[;\s])zoom\s*:\s*([0-9.]+)\s*;?")
LENGTH = re.compile(rb"\s*([0-9]*\.?[0-9]+(?:[eE][-+]?[0-9]+)?)\s*(px|pt|pc|mm|cm|in|em|ex|rem|ch|vw|vh|vmin|vmax)?\s*$")


# The root element's start tag and where it begins: past the prolog, its
# declaration, comments and document type, whose internal subset and whose
# comments may hold markup of their own.
def root_tag(text):
    at = 3 if text.startswith(b"\xef\xbb\xbf") else 0
    while at < len(text):
        while at < len(text) and text[at:at + 1].isspace():
            at += 1
        if text.startswith(b"<?", at):
            end = text.find(b"?>", at)
            if end < 0:
                return 0, b""
            at = end + 2
        elif text.startswith(b"<!--", at):
            end = text.find(b"-->", at)
            if end < 0:
                return 0, b""
            at = end + 3
        elif text.startswith(b"<!", at):
            depth = 0
            quote = b""
            at += 2
            while at < len(text):
                c = text[at:at + 1]
                if quote:
                    quote = b"" if c == quote else quote
                elif c in (b'"', b"'"):
                    quote = c
                elif c == b"[":
                    depth += 1
                elif c == b"]":
                    depth -= 1
                elif c == b">" and depth == 0:
                    break
                at += 1
            if at >= len(text):
                return 0, b""
            at += 1
        else:
            break
    found = TAG.match(text, at)
    if not found or not re.match(rb"<(?:[A-Za-z_][\w.-]*:)?svg\b", found.group(0)):
        return 0, b""
    return at, found.group(0)


# Whether the root gives a side: a positive length of its own, not a
# percentage of a box.
def given(tag, name):
    found = re.search(ATTRIBUTE % name, tag)
    if not found:
        return False
    length = LENGTH.match(found.group(3))
    return bool(length) and float(length.group(1)) > 0


def valid_viewbox(tag):
    found = re.search(ATTRIBUTE % b"viewBox", tag)
    if not found:
        return False
    try:
        numbers = [float(v) for v in re.split(rb"[\s,]+", found.group(3).strip())]
    except ValueError:
        return False
    return len(numbers) == 4 and numbers[2] > 0 and numbers[3] > 0


# A document whose root has no viewBox and leaves a side open is measured
# by the renderer: its size is the edge of what it draws, and a render
# stretches that size into the pixels. Chromium shows such a document in
# its box as it is, so it is shown through a wrapper that does the
# stretching: an svg of the pixels' size whose viewBox is the document's
# own size, stretched without regard to its ratio, with the document's
# root inside as a nested svg of that size. Inside an <image> instead
# Chromium draws patterns and some filters wrong, so the document is
# nested as it is, after its own prolog, which may declare entities. The
# root's width, height, x and y go: a percentage of a box, or a length
# that is none, is an open side to the renderer, and the nested svg's
# size is the document's own. A clip path or mask on the root moves to
# the wrapper: on the outermost svg it lies in the pixels of the box, as
# the renderer has it, where a nested svg would have it in its parent's
# units. A zoom on a root with no side of its own scales the content, in
# the renderer, inside a viewport of the size before the zoom: the nested
# svg gets that size and the scale as its transform, and no zoom.
def measured(text):
    start, tag = root_tag(text)
    return bool(tag) and not valid_viewbox(tag) and not (given(tag, b"width") and given(tag, b"height"))


def wrapper(text, own, width, height):
    start, tag = root_tag(text)
    inner = tag
    moved = b""
    for name in (b"clip-path", b"mask"):
        found = re.search(ATTRIBUTE % name, inner)
        if found:
            moved += found.group(0)
            inner = inner[:found.start()] + inner[found.end():]
    zoom = 1.0
    if not given(tag, b"width") and not given(tag, b"height"):
        found = re.search(ATTRIBUTE % b"zoom", inner)
        if found:
            zoom = float(found.group(3) or 1)
            inner = inner[:found.start()] + inner[found.end():]
        style = re.search(ATTRIBUTE % b"style", inner)
        if style:
            inside = ZOOM.search(style.group(3))
            if inside:
                zoom = float(inside.group(2))
                inner = inner[:style.start(3)] + style.group(3)[:inside.start()] + style.group(3)[inside.end():] + inner[style.end(3):]
    for name in (b"width", b"height", b"x", b"y"):
        inner = re.sub(ATTRIBUTE % name, b"", inner, count=1)
    if zoom != 1.0:
        transform = re.search(ATTRIBUTE % b"transform", inner)
        if transform:
            inner = inner[:transform.end(3)] + b" scale(%.6f)" % zoom + inner[transform.end(3):]
        else:
            inner = inner[:-1] + b' transform="scale(%.6f)">' % zoom if inner.endswith(b">") and not inner.endswith(b"/>") else inner
    name_end = re.match(rb"<[^\s/>]+", inner).end()
    inner = inner[:name_end] + b' width="%.4f" height="%.4f"' % (own[0] / zoom, own[1] / zoom) + inner[name_end:]
    outer = b'<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" viewBox="0 0 %.4f %.4f" preserveAspectRatio="none"%s>' % (width, height, own[0], own[1], moved)
    return text[:start] + outer + inner + text[start + len(tag):] + b"</svg>"


def render(browser, svg, program, side, scratch):
    url = json.dumps("file://" + os.path.abspath(svg))
    natural = browser.evaluate(LOAD % url)
    if not natural or natural[0] <= 0 or natural[1] <= 0:
        return "no natural size"
    own = own_size(program, svg)
    if not own or own[0] <= 0 or own[1] <= 0:
        return "no own size"
    with open(svg, "rb") as handle:
        text = handle.read()
    stretched = measured(text)
    for label, width, height in sizes(own[:2]):
        factor = 1 if own[2] else max(1, side // max(width, height))
        if stretched:
            # Chromium caches an image by its URL, so every wrapper has a
            # name of its own.
            wrapped = os.path.join(scratch, f"{threading.get_ident()}-{next(WRAPPERS)}.svg")
            with open(wrapped, "wb") as handle:
                handle.write(wrapper(text, own, width, height))
            url = json.dumps("file://" + wrapped)
        browser.send("Emulation.setDeviceMetricsOverride", {"width": width, "height": height, "deviceScaleFactor": factor, "mobile": False})
        if not browser.evaluate(SHOW % (width, height, url)):
            return "image failed to load"
        shot = base64.b64decode(browser.send("Page.captureScreenshot", {
            "format": "png",
            "clip": {"x": 0, "y": 0, "width": width, "height": height, "scale": 1},
            "fromSurface": True,
            "captureBeyondViewport": False,
        })["data"])
        if struct.unpack(">II", shot[16:24]) != (width * factor, height * factor):
            return f"screenshot is {struct.unpack('>II', shot[16:24])}, not {width * factor}x{height * factor}"
        large = os.path.join(scratch, f"{threading.get_ident()}.png")
        with open(large, "wb") as handle:
            handle.write(shot)
        subprocess.run([program, "shrink", large, svg[:-4] + f".{label}.png", str(factor)], check=True)
        if stretched:
            os.remove(wrapped)
    return None


def worker(index, chromium, program, side, cases, results, harness, scratch):
    browser = None
    done = 0
    for svg in cases:
        if all(os.path.exists(svg[:-4] + f".{label}.png") for label, _ in SIDES):
            continue
        for attempt in range(2):
            try:
                if browser is None:
                    browser = Browser(chromium, os.path.join(scratch, f"profile{index}"))
                    browser.open(harness)
                error = render(browser, svg, program, side, scratch)
                break
            except Exception as failure:
                error = f"chromium: {failure}"
                if browser:
                    browser.close()
                browser = None
        results.append((svg, error))
        done += 1
        if browser and done % 200 == 0:
            browser.navigate()
    if browser:
        browser.close()


def main():
    if len(sys.argv) != 5:
        sys.exit("usage: tst/reference.py CHROMIUM SVG CORPUS_DIR JOBS")
    chromium, program, corpus, jobs = sys.argv[1], os.path.abspath(sys.argv[2]), sys.argv[3], int(sys.argv[4])
    with open(os.path.join(corpus, "SIDE")) as handle:
        side = int(handle.read())
    # A chroot PID namespace can run every job as PID 1, sharing /tmp with
    # earlier jobs. Give each browser profile its own directory.
    scratch = tempfile.mkdtemp(prefix="svg-reference-")
    harness = os.path.join(scratch, "harness.html")
    with open(harness, "w") as handle:
        handle.write(HARNESS)
    # A case DESIGN lists has no reference: the renderer differs from the
    # others there by a decision of its design.
    with open(os.path.join(corpus, "DESIGN")) as handle:
        designed = {line.split(":")[0].strip() + ".svg" for line in handle if ":" in line}
    cases = sorted(
        os.path.join(directory, name)
        for directory, _, files in os.walk(corpus)
        for name in files
        if name.endswith(".svg") and os.path.relpath(os.path.join(directory, name), corpus) not in designed
    )
    results = []
    threads = [
        threading.Thread(target=worker, args=(k, chromium, program, side, cases[k::jobs], results, harness, scratch))
        for k in range(jobs)
    ]
    for thread in threads:
        thread.start()
    for thread in threads:
        thread.join()
    shutil.rmtree(scratch)
    failed = sorted((svg, error) for svg, error in results if error)
    for svg, error in failed:
        print(f"{svg}: {error}")
        os.remove(svg)
        for label, _ in SIDES:
            path = svg[:-4] + f".{label}.png"
            if os.path.exists(path):
                os.remove(path)
    version = subprocess.run([chromium, "--version"], capture_output=True, text=True).stdout.strip()
    with open(os.path.join(corpus, "CHROMIUM"), "w") as handle:
        handle.write(version + "\n")
    print(f"rendered {len(results) - len(failed)}, dropped {len(failed)}, {version}")


if __name__ == "__main__":
    main()
