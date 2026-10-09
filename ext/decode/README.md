# decode.simd128.wasm

ImageMagick and its coders (PNG, JPEG, WebP, TIFF, JPEG 2000, JPEG XL,
AVIF, GIF, BMP, PNM/PAM, TGA, PCX, SGI, MIFF) as one pure WebAssembly
module built with wasm simd128, from
[pg83/decode](https://github.com/pg83/decode) release
[9](https://github.com/pg83/decode/releases/tag/9) (sha256 of the module
bb6088cf13c25d161764a80234e78bf7f3ebe670f51d136f775158fa07345e1a). It
imports nothing; AVIF comes through libheif with libaom's decoder alone,
single-threaded; HEIC is not in.

The module exports its `memory`, `malloc`, `free` and

```
decode(data, len, name, name_len) -> image | 0
struct image { uint32 width; uint32 height; uint8 rgba[]; }
```

taking the file's bytes in the module's memory and answering a block
there, to be `free`d by the caller: 8-bit sRGB RGBA with the file's
orientation applied, the first frame of an animation, or 0 for a file no
coder takes. A broken file may trap the module; the instance is then
thrown away.

The test driver decodes the raster images of a document through it:
`build.py` turns it into C with wasm2c, and the C of a module with v128
includes SIMDe's `<simde/wasm/simd128.h>`. The renderer itself does not
use it: a program that embeds the renderer brings its own `SvgDecoder`.
