#!/usr/bin/env bash
# Builds nautilus-duck (the TPC-H harness, an external repository) against a Nautilus checkout, runs the 22 queries and
# summarizes the compilation statistics (docs/plans/nautilus-0.4/16-implementation-plan.md, M2.3).
#
#   run-tpch.sh <nautilus checkout> <nautilus-duck checkout> <output directory> [baseline summary]
#
# What is assumed of nautilus-duck, because the repository is not part of this one (confirm with its owner and adjust the
# variables, not the script):
#   DUCK_CMAKE_ARGS   extra CMake arguments; by default the Nautilus checkout is handed over as a FetchContent source
#                     override (-DFETCHCONTENT_SOURCE_DIR_NAUTILUS=<checkout>)
#   DUCK_TARGET       the target that builds the harness (default nautilus-duck)
#   DUCK_RUN          the command that runs the 22 queries, relative to the build directory (default ./nautilus-duck --tpch)
# Every compilation appends one JSON line to $out/statistics.jsonl (NAUTILUS_STATISTICS_FILE); summarize.py turns them
# into $out/summary.json, with the ratios to the baseline when one is given (a run against `main`).
set -euo pipefail
nautilus=$(realpath "$1")
duck=$(realpath "$2")
out=$(realpath -m "$3")
baseline=${4:-}
here=$(cd "$(dirname "$0")/../.." && pwd)
mkdir -p "$out"
build="$out/build"

cmake -G Ninja -S "$duck" -B "$build" -DCMAKE_BUILD_TYPE=Release \
	${DUCK_CMAKE_ARGS:--DFETCHCONTENT_SOURCE_DIR_NAUTILUS=$nautilus} > /dev/null
cmake --build "$build" --target "${DUCK_TARGET:-nautilus-duck}"

: > "$out/statistics.jsonl"
(
	cd "$build"
	# a single-tier run per backend would give the per-backend execution time; the default tiering is what users run
	NAUTILUS_STATISTICS_FILE="$out/statistics.jsonl" ${DUCK_RUN:-./nautilus-duck --tpch}
)
args=("$out/statistics.jsonl")
if [ -n "$baseline" ]; then
	args+=(--baseline "$baseline")
fi
python3 -I "$here/tools/tpch/summarize.py" "${args[@]}" | tee "$out/summary.json"
