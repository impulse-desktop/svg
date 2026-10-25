#!/bin/sh
# Builds the test driver with clang's source-based coverage, runs ./build
# unit, every drawing under tst/unit, and exports the coverage of svg.h and
# svg.cpp to .coverage: coverage.info in lcov for Codecov, summary.txt,
# functions.txt with the summary of every function, and show.txt with the
# count of every line and branch.
#
# usage: dev/coverage.sh [build options...]
set -eu

out=$PWD/.coverage
build=.build-coverage

# A test the build cache answers would leave no profile behind.
rm -rf "$out" "$build"
mkdir -p "$out/profiles"

# The tests are thousands of processes: each merges its counters into one of
# eight pool files of its binary instead of leaving a file of its own.
export LLVM_PROFILE_FILE="$out/profiles/%8m.profraw"
export CXXFLAGS="${CXXFLAGS:-} -fprofile-instr-generate -fcoverage-mapping"
export LDFLAGS="${LDFLAGS:-} -fprofile-instr-generate"

# Publish the instrumented binary before running units: a failed build does
# not restore its requested outputs from the cache, even successful ones.
./build -B "$build" "$@" svg

# The ASan jobs gate the units; what ran is exported all the same.
./build -B "$build" -k "$@" unit || echo "coverage: failed units above"

ignore='(^|/)(tst|ext|\.build[^/]*)/|^/usr/'

llvm-profdata merge -sparse "$out"/profiles/*.profraw -o "$out/coverage.profdata"
llvm-cov export "$build/svg" -instr-profile="$out/coverage.profdata" -format=lcov -ignore-filename-regex="$ignore" | sed "s|^SF:$PWD/|SF:|" > "$out/coverage.info"
llvm-cov report "$build/svg" -instr-profile="$out/coverage.profdata" -ignore-filename-regex="$ignore" > "$out/summary.txt"
llvm-cov report "$build/svg" -instr-profile="$out/coverage.profdata" -show-functions "$PWD/svg.cpp" > "$out/functions.txt"
llvm-cov show "$build/svg" -instr-profile="$out/coverage.profdata" -ignore-filename-regex="$ignore" -show-branches=count > "$out/show.txt"

if ! grep -qx 'SF:svg.cpp' "$out/coverage.info"; then
    echo "coverage: no svg.cpp in the report" >&2
    exit 1
fi

cat "$out/summary.txt"
