#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")/.."
cmake --build build-perf --target nautilus-execution-tests -j"$(sysctl -n hw.ncpu)"
./build-perf/nautilus/test/execution-tests/nautilus-execution-tests
