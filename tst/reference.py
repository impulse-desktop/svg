#!/usr/bin/env python3
import base64
import json
import math
import os
import select
import shutil
import struct
import subprocess
import sys
import tempfile
import threading
import time

SIDES = [("1x", 256), ("2x", 512)]
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
  img.style.width = '%.4fpx';
  img.style.height = '%.4fpx';
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
# the edges of what it draws. Chromium shows the document in a box of that
# size, so the two agree on what the box is where Chromium would make one
# up; the pixels of the box come of the device scale factor.
def own_size(program, svg):
    line = subprocess.run([program, "size", svg], capture_output=True, text=True, check=True).stdout.split()
    if len(line) != 3 or line[1] == "?":
        return None
    return float(line[1]), float(line[2])


def render(browser, svg, program, side, scratch):
    url = json.dumps("file://" + os.path.abspath(svg))
    natural = browser.evaluate(LOAD % url)
    if not natural or natural[0] <= 0 or natural[1] <= 0:
        return "no natural size"
    own = own_size(program, svg)
    if not own or own[0] <= 0 or own[1] <= 0:
        return "no own size"
    for label, width, height in sizes(own):
        factor = max(1, side // max(width, height))
        # The box is the document's own width in CSS pixels, at the device
        # scale that makes it the pixels wanted, and as high as keeps the
        # pixels' ratio. The screenshot is of the viewport, the box rounded
        # up to whole CSS pixels, so it is never short of the pixels however
        # Chromium rounds; shrink crops it to them.
        scale = width * factor / own[0]
        box_width = own[0]
        box_height = height * factor / scale
        viewport_width = math.ceil(box_width)
        viewport_height = math.ceil(box_height)
        browser.send("Emulation.setDeviceMetricsOverride", {"width": viewport_width, "height": viewport_height, "deviceScaleFactor": scale, "mobile": False})
        if not browser.evaluate(SHOW % (box_width, box_height, url)):
            return "image failed to load"
        shot = base64.b64decode(browser.send("Page.captureScreenshot", {
            "format": "png",
            "clip": {"x": 0, "y": 0, "width": viewport_width, "height": viewport_height, "scale": 1},
            "fromSurface": True,
            "captureBeyondViewport": False,
        })["data"])
        shot_width, shot_height = struct.unpack(">II", shot[16:24])
        if not (0 <= shot_width - width * factor <= scale + 2 and 0 <= shot_height - height * factor <= scale + 2):
            return f"screenshot is {shot_width}x{shot_height}, not {width * factor}x{height * factor}"
        large = os.path.join(scratch, f"{threading.get_ident()}.png")
        with open(large, "wb") as handle:
            handle.write(shot)
        subprocess.run([program, "shrink", large, svg[:-4] + f".{label}.png", str(factor), str(width), str(height)], check=True)
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
    cases = sorted(
        os.path.join(directory, name)
        for directory, _, files in os.walk(corpus)
        for name in files
        if name.endswith(".svg")
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
