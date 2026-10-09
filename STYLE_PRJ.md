# Project style settings

Per-project settings that the shared [STYLE.md](STYLE.md) delegates here.

- **Macro prefix.** None reserved.
- **Namespace.** None: the renderer is two files, `svg.h` and `svg.cpp`,
  copied into the programs that use it. `libstd` keeps its `stl`.
- **Formatter.** `./dev/style.py` formats every tracked C++ source.

## Deviations

- `main.cpp` is the test driver alone; nothing it needs may leak into
  `svg.h` or `svg.cpp`.
