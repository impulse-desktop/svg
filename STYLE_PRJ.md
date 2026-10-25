# Project style settings

Per-project settings that the shared [STYLE.md](STYLE.md) delegates here.

- **Macro prefix.** None reserved.
- **Namespace.** None: the renderer is two files, `svg.h` and `svg.cpp`,
  copied into the programs that use it.
- **Formatter.** `./dev/style.py` formats tracked C++ sources, excluding
  the frozen snapshot `svg_canon.cpp`.

## Deviations

- The public API in `svg.h` is C, and its functions are named `svg_`
  snake_case as C libraries are, not `lowerCamelCase`.
- `svg.cpp` carries its own copies of the libstd containers, sort and
  object pool in its anonymous namespace instead of including `<std/...>`,
  so that the two files depend on nothing but the C library.
- `tst/main.cpp` and `tst/fuzz.cpp` are the test driver and the fuzzer, not
  the product. They use the C++ standard library; nothing they need may leak
  into `svg.h` or `svg.cpp`.
