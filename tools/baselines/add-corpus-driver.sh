#!/usr/bin/env bash
# Adds the latency-corpus driver of this checkout to a checkout of `main` (0.3), which does not have it. The driver uses
# only the public engine API, so the same source builds against both; `main` takes no job or driver of its own.
#
#   add-corpus-driver.sh <main checkout>
#
# Used by record-0.3.sh (the baselines-0.3 workflow) and by the callgrind gate (tools/latency).
set -euo pipefail
main=$(realpath "$1")
here=$(cd "$(dirname "$0")/../.." && pwd)

# the target is appended to main's benchmark CMake file, which does not know it
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
