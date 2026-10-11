#!/usr/bin/env python3
import base64
import json
import os
import struct
import subprocess
import sys
import threading

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

    def send(self, method, params=None, session=True):
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
            self.send("Browser.close", session=False)
        except Exception:
            pass
        self.process.kill()
        self.process.wait()


def sizes(natural):
    width, height = natural
    longest = max(width, height)
    for label, side in SIDES:
        scale = side / longest
        yield label, max(1, round(width * scale)), max(1, round(height * scale))


def render(browser, svg, program, side, scratch):
    url = json.dumps("file://" + os.path.abspath(svg))
    natural = browser.evaluate(LOAD % url)
    if not natural or natural[0] <= 0 or natural[1] <= 0:
        return "no natural size"
    for label, width, height in sizes(natural):
        factor = max(1, side // max(width, height))
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
    scratch = os.path.join("/tmp", f"svg-reference-{os.getpid()}")
    os.makedirs(scratch, exist_ok=True)
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
