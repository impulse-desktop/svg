import platform

import build


# References must stay usable when the renderer under test is instrumented.
# An imported graph gives the canon its own flags and tracks its source,
# header and driver directly, without a checkout or a nested build.
def plain(flags):
    return [flag for flag in flags if not flag.startswith((
        "-fsanitize", "-fno-sanitize", "-fprofile", "-fcoverage", "--coverage",
    ))]


build.cflags = plain(build.cflags)
build.cppflags = plain(build.cppflags)
build.cxxflags = plain(build.cxxflags) + ["-std=c++26", "-O2", "-g"]
build.ldflags = plain(build.ldflags)
if platform.machine() in ("x86_64", "AMD64"):
    build.cxxflags += ["-march=x86-64-v2"]

canon_svg = program(
    name="canon_svg",
    output="$(B)/svg",
    srcs=["$(S)/../svg_canon.cpp", "$(S)/main.cpp"],
    cflags=["-Wall", "-Wextra", "-Werror"],
    deps=[pkg_config("libpng"), pkg_config("libjpeg"), dependency(ldflags=["-lm"])],
)
