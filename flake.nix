{
  description = "Clang 21 + libc++ build environment for svg";

  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";

  outputs = { nixpkgs, ... }:
    let
      systems = [
        "x86_64-linux"
        "aarch64-linux"
      ];
      forAllSystems = nixpkgs.lib.genAttrs systems;
    in
    {
      devShells = forAllSystems (system:
        let
          pkgs = nixpkgs.legacyPackages.${system};
          llvm = pkgs.llvmPackages_21;
          stdenv = llvm.libcxxStdenv;
          cxxSanitizerFlags = "-O1 -g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize=address,undefined -fno-sanitize-recover=all";

          libFuzzerLibcxx = stdenv.mkDerivation {
            pname = "libfuzzer-libcxx";
            version = "21.1.8";
            src = pkgs.fetchurl {
              url = "https://github.com/llvm/llvm-project/releases/download/llvmorg-21.1.8/compiler-rt-21.1.8.src.tar.xz";
              hash = "sha256-3VSuIa7heA+sWURbUev/YBrQFrMaw6feOyESb9PMsik=";
            };
            sourceRoot = "compiler-rt-21.1.8.src/lib/fuzzer";
            dontConfigure = true;

            buildPhase = ''
              runHook preBuild
              $CXX -std=c++17 -O2 -g0 -fPIC -fno-omit-frame-pointer -I../../include -c \
                FuzzerCrossOver.cpp \
                FuzzerDataFlowTrace.cpp \
                FuzzerDriver.cpp \
                FuzzerExtFunctionsDlsym.cpp \
                FuzzerExtFunctionsWeak.cpp \
                FuzzerExtFunctionsWindows.cpp \
                FuzzerExtraCounters.cpp \
                FuzzerExtraCountersDarwin.cpp \
                FuzzerExtraCountersWindows.cpp \
                FuzzerFork.cpp \
                FuzzerIO.cpp \
                FuzzerIOPosix.cpp \
                FuzzerIOWindows.cpp \
                FuzzerLoop.cpp \
                FuzzerMain.cpp \
                FuzzerMerge.cpp \
                FuzzerMutate.cpp \
                FuzzerSHA1.cpp \
                FuzzerTracePC.cpp \
                FuzzerUtil.cpp \
                FuzzerUtilDarwin.cpp \
                FuzzerUtilFuchsia.cpp \
                FuzzerUtilLinux.cpp \
                FuzzerUtilPosix.cpp \
                FuzzerUtilWindows.cpp
              $AR rcs libFuzzer.a ./*.o
              runHook postBuild
            '';

            installPhase = ''
              runHook preInstall
              install -Dm644 libFuzzer.a $out/lib/libFuzzer.a
              runHook postInstall
            '';
          };

          buildCommand = pkgs.writeShellScriptBin "svg-build" ''
            if [[ ! -x ./build || ! -f ./build.py ]]; then
              echo "svg-build: run this command from the repository root" >&2
              exit 2
            fi

            exec ./build "$@"
          '';

          sanitizerCommand = pkgs.writeShellScriptBin "svg-sanitizers" ''
            set -euo pipefail

            if [[ ! -f ./tst/sanitizer.svg ]]; then
              echo "svg-sanitizers: run this command from the repository root" >&2
              exit 2
            fi

            svg-build "$@"
            ${pkgs.patchelf}/bin/patchelf --print-needed .build/svg | grep -q '^libc++\.so'
            mkdir -p .build/sanitizers
            ./.build/svg render tst/sanitizer.svg .build/sanitizers/smoke.png 192 128
            test -s .build/sanitizers/smoke.png
          '';

          fuzzCommand = pkgs.writeShellScriptBin "svg-fuzz" ''
            set -euo pipefail

            if [[ ! -f ./tst/fuzz.cpp ]]; then
              echo "svg-fuzz: run this command from the repository root" >&2
              exit 2
            fi

            svg-build -j "''${SVG_BUILD_JOBS:-4}" fuzz

            # The seeds are the inputs on which the fuzzer once found a crash,
            # kept under tst/fuzz, and the four test suites of the corpus, which
            # cover the features at a fraction of the cost of the whole corpus:
            # under ASan a CI core runs about 55 seeds and 200 mutations a second,
            # so 150000 runs on every core take about a quarter of an hour. Inputs the run
            # finds interesting go to .build/fuzzing/new, crashes to .build/fuzzing.
            root=$PWD
            work=$root/.build/fuzzing
            rm -rf "$work"
            mkdir -p "$work/seeds" "$work/new"
            for file in "$root"/tst/corpus/{resvg,w3c,wpt,blink}/*.svg; do
              ln -s "$file" "$work/seeds/$(basename "$(dirname "$file")")-$(basename "$file")"
            done
            cd "$work"
            exec "$root/.build/fuzz" -timeout=120 -runs=150000 -jobs="$(nproc)" -workers="$(nproc)" "$work/new" "$root/tst/fuzz" "$work/seeds" "$@"
          '';

          commonNativeBuildInputs = [
            buildCommand
            llvm.llvm
            pkgs.pkg-config
            pkgs.python3
            pkgs.wabt
          ];
          commonBuildInputs = [
            pkgs.libpng
            pkgs.simde
          ];
        in
        {
          default = pkgs.mkShell.override { inherit stdenv; } {
            nativeBuildInputs = commonNativeBuildInputs;
            buildInputs = commonBuildInputs;
            NIX_CC_WRAPPER_SUPPRESS_TARGET_WARNING = "1";
          };

          sanitized = pkgs.mkShell.override { inherit stdenv; } {
            nativeBuildInputs = commonNativeBuildInputs ++ [
              fuzzCommand
              sanitizerCommand
              pkgs.patchelf
            ];
            buildInputs = commonBuildInputs;
            NIX_CC_WRAPPER_SUPPRESS_TARGET_WARNING = "1";

            # Only the C++ renderer and the test driver are instrumented. The generated
            # wasm2c decoder shards and runtime are C and keep their cache keys.
            CXXFLAGS = cxxSanitizerFlags;
            LDFLAGS = "-fsanitize=address,undefined";
            LIBFUZZER = "${libFuzzerLibcxx}/lib/libFuzzer.a";
            ASAN_SYMBOLIZER_PATH = "${llvm.llvm}/bin/llvm-symbolizer";
            ASAN_OPTIONS = "detect_leaks=1:halt_on_error=1:abort_on_error=1:symbolize=1";
            UBSAN_OPTIONS = "halt_on_error=1:abort_on_error=1:print_stacktrace=1";
          };
        });
    };
}
