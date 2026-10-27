#!/usr/bin/env bash
# Records the 0.3 baselines (docs/plans/nautilus-0.4/16-implementation-plan.md §2, M1.10) from a checkout of `main`:
#   corpus.jsonl      the latency-corpus driver of this checkout, built against main (public API only)
#   ir-functions.json the inferred FunctionAttributes and the operation count after 0.3's passes of every function the
#                     execution tests compile (from the IR dumps; see extract_ir_baseline.py)
#   main-commit.txt   the commit that was measured
#
#   record-0.3.sh <main checkout> <output directory> [repetitions] [build directory]
#
# The same script runs in the baselines-0.3 workflow and by hand; absolute numbers depend on the machine, the instruction
# counts and the IR facts do not.
set -euo pipefail
main=$(realpath "$1")
out=$(realpath -m "$2")
reps=${3:-5}
build=$(realpath -m "${4:-$out/build}")
here=$(cd "$(dirname "$0")/../.." && pwd)

mkdir -p "$out" "$build"
git -C "$main" rev-parse HEAD > "$out/main-commit.txt"

# add the driver to main; the target is appended to main's benchmark CMake file, which does not know it
mkdir -p "$main/nautilus/test/benchmark/latency-corpus"
cp "$here/nautilus/test/benchmark/latency-corpus/LatencyCorpus.cpp" "$main/nautilus/test/benchmark/latency-corpus/"
if ! grep -q nautilus-latency-corpus "$main/nautilus/test/benchmark/CMakeLists.txt"; then
	cat >> "$main/nautilus/test/benchmark/CMakeLists.txt" <<'CMAKE'

if (ENABLE_TRACING AND ENABLE_BENCHMARKS)
    add_executable(nautilus-latency-corpus latency-corpus/LatencyCorpus.cpp)
    target_link_libraries(nautilus-latency-corpus PRIVATE nautilus)
endif ()
CMAKE
fi

cmake -G Ninja -S "$main" -B "$build" -DCMAKE_BUILD_TYPE=Release -DENABLE_MLIR_BACKEND=OFF -DENABLE_BENCHMARKS=ON \
	-DENABLE_TESTS=ON ${CMAKE_EXTRA_ARGS:-} > /dev/null
cmake --build "$build" --target nautilus-latency-corpus nautilus-execution-tests

"$build/nautilus/test/benchmark/nautilus-latency-corpus" "$reps" > "$out/corpus.jsonl"

# the dumps of every compilation of the execution tests go to a scratch TMPDIR
scratch=$(mktemp -d)
trap 'rm -rf "$scratch"' EXIT
TMPDIR="$scratch" NAUTILUS_OPTIONS="dump.all=true;dump.after_tracing=false" \
	ctest --test-dir "$build/nautilus" -R 'Engine Compiler Test|NautilusFunction Compiled Execution Test' -j4 > /dev/null
python3 -I "$here/tools/baselines/extract_ir_baseline.py" "$scratch/dump" "$out/ir-functions.json"
wc -l "$out/corpus.jsonl"
