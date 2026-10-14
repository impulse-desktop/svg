#!/bin/sh
# Builds the test driver with clang's source-based coverage, runs ./build
# unit, every drawing under tst/unit, and exports the coverage of svg.h and
# svg.cpp to .coverage: coverage.info in lcov for Codecov, summary.txt, and
# show.txt with the count of every line and branch.
#
# usage: dev/coverage.sh [build options...]
set -eu

out=$PWD/.coverage
build=.build-coverage

# A test the build cache answers would leave no profile behind.
rm -rf "$out" "$build"
mkdir -p "$out/profiles"

# Every process leaves a file of its own: merging into shared pool files
# lost the counters of some of them.
export LLVM_PROFILE_FILE="$out/profiles/%p.profraw"
export CXXFLAGS="${CXXFLAGS:-} -fprofile-instr-generate -fcoverage-mapping"
export LDFLAGS="${LDFLAGS:-} -fprofile-instr-generate"

# The ASan jobs gate the units; what ran is exported all the same.
./build -B "$build" -k "$@" unit svg || echo "coverage: failed units above"

ignore='(^|/)(tst|ext|\.build[^/]*)/|^/usr/'

llvm-profdata merge -sparse "$out"/profiles/*.profraw -o "$out/coverage.profdata"
llvm-cov export "$build/svg" -instr-profile="$out/coverage.profdata" -format=lcov -ignore-filename-regex="$ignore" | sed "s|^SF:$PWD/|SF:|" > "$out/coverage.info"
llvm-cov report "$build/svg" -instr-profile="$out/coverage.profdata" -ignore-filename-regex="$ignore" > "$out/summary.txt"
llvm-cov show "$build/svg" -instr-profile="$out/coverage.profdata" -ignore-filename-regex="$ignore" -show-branches=count > "$out/show.txt"

if ! grep -qx 'SF:svg.cpp' "$out/coverage.info"; then
    echo "coverage: no svg.cpp in the report" >&2
    exit 1
fi

cat "$out/summary.txt"
