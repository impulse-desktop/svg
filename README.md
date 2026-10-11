# svg

[![CI](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml/badge.svg?branch=main)](https://github.com/impulse-desktop/svg/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

An SVG renderer of its own for the impulse desktop: parser, document model
and rasterizer, written from scratch and measured against Chromium on a
corpus of thousands of files.

The renderer is two files, `svg.h` and `svg.cpp`, over
[libstd](https://github.com/pg83/std). A program that uses it copies both
into its `ext/`.

## Scope

Static SVG without text: no `<text>`, no SMIL or CSS animation, no script,
no `foreignObject`. Raster images inside a document are decoded by the
program that embeds the renderer. Correctness comes first; speed and
memory come after it.

## The contract

```cpp
SvgLibrary* library = SvgLibrary::create(pool, decoder);
SvgDocument* document = library->parse(pool, bytes);
SvgImage* image = document->render(pool, width, height);
```

`render(width, height)` draws the document the way Chromium draws it as an
image of that size: `<img src="file.svg" style="width: Wpx; height: Hpx">`.
`render(width, height, scale)` draws it at `scale` device pixels to the CSS
pixel: the image is laid out at `width / scale` by `height / scale` and drawn
at `width` by `height`. The pixels are RGBA, 8 bits a channel, not
premultiplied. `decoder`, an
`SvgDecoder` of the embedding program, turns the bytes of a raster image
inside the document into such pixels.

## The score

`./build meanrms`, `./build maxrms`, `./build struct` and `./build aa` draw
every case of the corpus and print what they measure; lower is better, 0 is
the reference itself. `./build all` prints the four together with the
candidates for the next fixes.

- A case is one file of `tst/corpus/<source>/` and its two references next
  to it: `<name>.1x.png`, its longer side 256 pixels, and `<name>.2x.png`,
  512 pixels.
- A reference is supersampled: drawn with its longer side `tst/corpus/SIDE`
  device pixels, 1024, so 1x at 4 device pixels to the pixel and 2x at 2,
  and shrunk by the mean of each block in premultiplied RGBA. Ours is drawn
  and shrunk the same way, so neither antialiasing nor the shapes laid one
  over another on an edge tell the two apart, only what is drawn.
- The error of one drawing is the mean absolute difference between ours
  and the reference over every pixel and channel, in premultiplied RGBA,
  scaled to [0, 1]. A crash or a drawing past two minutes counts as 1.
- The error of one case is the root mean square of the errors of its
  drawings.
- `meanrms` is the root mean square of the errors of every drawing: how
  close the whole corpus is.
- `maxrms` is the error of the worst case.
- `struct` counts what is drawn wrong rather than slightly differently. A
  pixel is wrong when a channel of ours lies more than 32 out of 255 outside
  the range of Chromium's pixels around it, three by three, or Chromium's
  lies that far outside the range of ours, and both drawings blurred with
  the kernel 1 2 1 across and down still differ there by more than 32: an
  edge or a thin line a fraction of a pixel away is not wrong, a missing or
  miscolored one is. A case fails when over 0.1% of the pixels of one of its
  drawings are wrong. `struct` is the number of failing cases and the mean
  share of wrong pixels over the worst 1%.
- `aa` is `meanrms` of ours drawn straight at 256 pixels against the same 1x
  references: how close our plain drawing is to the supersampled one,
  antialiasing included.

The same test draws each case once for all of them: `svg test` writes the
errors and the shares of wrong pixels of its supersampled drawings, the
error of its plain 1x drawing, and the shares of wrong pixels between the 1x
drawing and the 2x drawing halved, the reference's and ours. When the 2x
reference is exactly twice the 1x one and nothing in the document depends on
the CSS size, one supersampled drawing serves both, and ours agree by
construction. A reference drawn differently at the two sizes while ours agree
is a candidate for `tst/corpus/CANON`.

`$(B)/meanrms.txt`, `$(B)/maxrms.txt`, `$(B)/struct.txt`, `$(B)/aa.txt` and
`$(B)/all.txt` hold the numbers and every case, the worst first.
`-Dfilter=GLOB` scores the cases whose path under `tst/corpus` matches,
`./build -Dfilter='resvg/*' all`.

## The corpus

| source | what |
|---|---|
| `resvg` | the resvg test suite |
| `w3c` | the W3C SVG 1.1 test suite |
| `blink` | Chromium's `web_tests/svg` |
| `wpt` | web-platform-tests: `svg`, `css-masking`, `filter-effects`, `compositing` |
| `adwaita`, `breeze`, `papirus` | desktop icon themes |
| `mdi`, `simple`, `tabler`, `bootstrap`, `fontawesome` | icon sets |
| `logos` | brand logos from gilbarbara/logos |
| `wikimedia` | random files of Wikimedia Commons up to 300 KB |
| `openclipart` | random Openclipart drawings up to 400 KB, mostly made in Inkscape |
| `inkscape` | artwork from the inkscape.org community gallery up to 1 MB |

`tst/import.py RAW CORPUS` takes the files from the unpacked sources: text
elements are cut out, files with what is out of scope are left out, equal
files are kept once, and the large icon sets are sampled with a fixed seed.
`tst/reference.py CHROMIUM SVG CORPUS JOBS` draws the references with headless
Chromium through its DevTools pipe at the device scale `SIDE` asks for,
shrinks them with `svg shrink` of the test driver `SVG`, and drops the files
Chromium cannot draw; `tst/corpus/CHROMIUM` names the version. Chromium is the reference,
not the goal: where it demonstrably draws a case wrong, the drawing of
Inkscape, resvg, Firefox or ours that is right takes its place.
`tst/corpus/CANON` lists such a case with the renderer and the reason,
`tst/canon.py RENDERER PROGRAM SVG CORPUS` draws the cases listed for that
renderer at the sizes Chromium chose, supersampled the same way, and
`tst/corpus/INKSCAPE`, `tst/corpus/RESVG`, `tst/corpus/FIREFOX` and
`tst/corpus/OURS` name the versions. Firefox is drawn twice, over white and
over black, to recover the alpha its screenshots lose, and scaled by a CSS
transform; Inkscape and resvg can only enlarge a drawing, which is the same
as a device scale unless the document has neither a viewBox nor an absolute
size. A case whose behaviour is undefined and which
every renderer draws differently is listed with the reason in
`tst/corpus/EXCLUDED`, and is neither drawn nor scored. The corpus grows; the
references are drawn again only when Chromium is changed.

## Building

You need clang 21 or GCC 16, Python 3, libpng, wasm2c from
[wabt](https://github.com/WebAssembly/wabt) and
[SIMDe](https://github.com/simd-everywhere/simde). The test driver decodes
the raster images of a document with `ext/decode/decode.simd128.wasm`,
ImageMagick as a pure WebAssembly module, which the build turns into C
with wasm2c; see [its README](ext/decode/README.md).

```
./build          # .build/svg, the test driver
./build all      # meanrms, maxrms, struct, aa and the candidates
```

The Nix flake supplies Clang 21 with libc++, wasm2c, SIMDe, libpng and Python;
the repository's build runner still does the build in the working tree. Both
shells use the same persistent `.build`, so unchanged objects are reused when
switching configurations:

```
nix develop -c svg-build                         # regular build
nix develop .#sanitized -c svg-build             # ASan + UBSan build
nix develop .#sanitized -c svg-sanitizers        # build and smoke test
nix develop .#sanitized -c svg-fuzz -runs=40000  # libFuzzer over tst/
```

The sanitizer shell instruments the C++ renderer and libstd. The generated
wasm2c decoder shards and their C runtime are trusted, left uninstrumented and
therefore retain the same `.build` cache keys in both configurations. The fuzz
target uses a null raster decoder, so wasm2c is not linked into `.build/fuzz`.

`svg render IN.svg OUT.png WIDTH HEIGHT` draws one file;
`svg test IN.svg SCORE REFERENCE.png...` writes the errors and the shares
of wrong pixels of one case;
`svg bench LIST REPEAT` times parsing and drawing into memory for every
`PATH WIDTH HEIGHT` line of LIST, the file read beforehand, and prints the
fastest of REPEAT runs in nanoseconds.

## License

MIT, see [LICENSE](LICENSE).
