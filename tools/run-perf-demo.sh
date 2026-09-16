#!/usr/bin/env bash
# Builds and profiles example/src/DemoPerfProfile.cpp end-to-end: configure + build the
# example project, run it under `perf record` with the MLIR backend's perf jitdump support
# enabled, inject the jitdump into perf.data, and render both a `perf report` summary and a
# nested-region flame graph via tools/nautilus-perf.sh.
#
# See docs/profiling.md for what mlir.perf.enable actually gets you and why each recipe step
# (-k mono, engine.backend=mlir, mlir.debug.source_mode=nautilus-ir) is mandatory.
#
# Usage:
#   tools/run-perf-demo.sh [rounds] [len]
#
# Environment:
#   BUILD_DIR   Where to configure/build the example project (default: example/build)
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
build_dir="${BUILD_DIR:-$repo_root/example/build}"
rounds="${1:-20000000}"
len="${2:-64}"

if ! command -v perf >/dev/null 2>&1; then
	echo "run-perf-demo.sh: 'perf' not found on PATH -- install linux-tools for your kernel" >&2
	exit 1
fi

echo "run-perf-demo.sh: configuring + building demo_perf_profile in $build_dir" >&2
cmake -S "$repo_root/example" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release
cmake --build "$build_dir" --target demo_perf_profile -j

cd "$build_dir"
echo "run-perf-demo.sh: recording (rounds=$rounds len=$len)" >&2
"$repo_root/tools/nautilus-perf.sh" record -- ./demo_perf_profile "$rounds" "$len"

echo >&2
echo "run-perf-demo.sh: perf report (top of stdio view) --------------------------" >&2
"$repo_root/tools/nautilus-perf.sh" report | head -40

echo >&2
"$repo_root/tools/nautilus-perf.sh" flamegraph perf.jit.data "$build_dir/perf-flamegraph.svg"
