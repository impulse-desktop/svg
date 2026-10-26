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

            # A corpus case that a sanitizer stops scores as a crash and the
            # build goes on, so every report goes to a file under
            # .build/sanitizers/logs, and any of them fails the run on exit.
            logs=$PWD/.build/sanitizers/logs
            rm -rf "$logs"
            mkdir -p "$logs"
            export ASAN_OPTIONS="''${ASAN_OPTIONS-}:log_path=$logs/asan"
            export UBSAN_OPTIONS="''${UBSAN_OPTIONS-}:log_path=$logs/ubsan"
            report() {
              if [[ -n "$(ls -A "$logs")" ]]; then
                cat "$logs"/*
                exit 1
              fi
            }
            trap report EXIT

            # The driver is named too, so that the build publishes .build/svg
            # for the libc++ check whatever else it is given.
            svg-build "$@" svg
            ${pkgs.patchelf}/bin/patchelf --print-needed .build/svg | grep -q '^libc++\.so'
          '';

          fuzzCommand = pkgs.writeShellScriptBin "svg-fuzz" ''
            set -euo pipefail

            if [[ ! -f ./tst/fuzz.cpp ]]; then
              echo "svg-fuzz: run this command from the repository root" >&2
              exit 2
            fi

            svg-build -j "''${SVG_BUILD_JOBS:-4}" fuzz

            # The seeds are the inputs that once crashed the renderer under the
            # sanitizers, kept under tst/fuzz, and 200 SVGs drawn at random from
            # under tst, the whole corpus among them, a different sample every
            # run; CI runs five such runs side by side. Inputs the run finds
            # interesting go to .build/fuzzing/new, crashes to .build/fuzzing.
            root=$PWD
            work=$root/.build/fuzzing
            rm -rf "$work"
            mkdir -p "$work/seeds" "$work/new"
            find "$root/tst" -name '*.svg' | shuf -n 200 | while read -r file; do
              relative=''${file#"$root/tst/"}
              ln -s "$file" "$work/seeds/''${relative//\//-}"
            done
            cd "$work"

            # libFuzzer checks max_total_time only after executing the seed
            # corpus. Bound each worker from startup, including that phase.
            # Allow five minutes beyond its 30-minute budget for completion.
            # SIGINT uses libFuzzer's interrupt handler; preserve-status keeps
            # real failures and forced kills distinct from a budget stop.
            workers=$(nproc)
            pids=()
            trap 'jobs -pr | xargs -r kill' EXIT
            trap 'exit 130' INT
            trap 'exit 143' TERM
            for ((worker = 0; worker < workers; worker++)); do
              LC_ALL=C timeout --verbose --preserve-status --signal=INT --kill-after=30s 35m \
                "$root/.build/fuzz" -timeout=120 -max_total_time=1800 \
                -print_final_stats=1 "$work/new" "$root/tst/fuzz" "$work/seeds" "$@" \
                > "$work/fuzz-$worker.log" 2>&1 &
              pids+=("$!")
            done
            status=0
            for worker in "''${!pids[@]}"; do
              worker_status=0
              wait "''${pids[$worker]}" || worker_status=$?
              cat "$work/fuzz-$worker.log"
              # 72 is libFuzzer's interrupt status, accepted only when our
              # timer sent SIGINT. Input timeouts (70) remain failures.
              if ((worker_status == 72)) && grep -Fq 'timeout: sending signal INT' "$work/fuzz-$worker.log"; then
                echo "Fuzzer worker $worker reached its 35-minute process limit"
                worker_status=0
              fi
              echo "Fuzzer worker $worker exited with status $worker_status"
              if ((worker_status != 0)); then
                status=1
              fi
            done
            exit "$status"
          '';

          commonNativeBuildInputs = [
            buildCommand
            llvm.llvm
            pkgs.git
            pkgs.pkg-config
            pkgs.python3
          ];
          commonBuildInputs = [
            pkgs.libjpeg
            pkgs.libpng
            pkgs.zlib
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

            # The renderer and the test driver are instrumented.
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
