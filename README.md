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
The pixels are RGBA, 8 bits a channel, not premultiplied. `decoder`, an
`SvgDecoder` of the embedding program, turns the bytes of a raster image
inside the document into such pixels.

## The score

`./build meanrms`, `./build maxrms` and `./build struct` draw every case of
the corpus and print what they measure; lower is better, 0 is Chromium
itself. `./build all` prints the three together with the candidates for
the next fixes.

- A case is one file of `tst/corpus/<source>/` and Chromium's two drawings
  of it next to it: `<name>.1x.png`, its longer side 256 pixels, and
  `<name>.2x.png`, 512 pixels.
- The error of one drawing is the mean absolute difference between ours
  and Chromium's over every pixel and channel, in premultiplied RGBA,
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

The same test draws each case once for all of them: `svg test` writes the
errors and the shares of wrong pixels of its drawings, and the shares of
wrong pixels between the 1x drawing and the 2x drawing halved, Chromium's
and ours. A reference Chromium draws differently at the two sizes while
ours agree is a candidate for `tst/corpus/INKSCAPE`.

`$(B)/meanrms.txt`, `$(B)/maxrms.txt`, `$(B)/struct.txt` and
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
`tst/reference.py CHROMIUM CORPUS JOBS` draws the references with headless
Chromium through its DevTools pipe and drops the files Chromium cannot
draw; `tst/corpus/CHROMIUM` names the version. A case Chromium
demonstrably draws wrong is listed with the reason in `tst/corpus/INKSCAPE`,
and `tst/inkscape.py INKSCAPE CORPUS` then draws its references with
Inkscape at the sizes Chromium chose; `tst/corpus/INKSCAPE_VERSION` names
that version. The corpus grows; the
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
./build all      # meanrms, maxrms, struct and the candidates
```

`svg render IN.svg OUT.png WIDTH HEIGHT` draws one file;
`svg test IN.svg SCORE REFERENCE.png...` writes the errors and the shares
of wrong pixels of one case;
`svg bench LIST REPEAT` times parsing and drawing into memory for every
`PATH WIDTH HEIGHT` line of LIST, the file read beforehand, and prints the
fastest of REPEAT runs in nanoseconds.

## License

MIT, see [LICENSE](LICENSE).
