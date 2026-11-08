#!/usr/bin/env bash
# The callgrind gate (decision D9, docs/plans/nautilus-0.4/10-observability.md §2) without the CI image: builds the
# latency-corpus driver for this checkout (NIR) and for a checkout of `main` (0.3), measures the instruction count of
# every case under callgrind (twice for this checkout, the identity check), and compares the ratios with
# nautilus/test/baselines/callgrind-gates.json. The `callgrind-gate` job of pr.yml runs this script inside the pinned
# image (.github/docker/ci-0.4) with --ci.
#
#   run-callgrind-local.sh [--main <main checkout>] [--out <dir>] [--jobs N] [--update [--allow-raise]] [--ci]
#
#   --main     a checkout of `main`; by default a worktree of origin/main under <out>
#   --out      scratch and results (default: callgrind-out under the build directory root, or ./callgrind-out)
#   --jobs     parallel callgrind processes (default: all cores) and build jobs
#   --update   record the thresholds (the ratchet: they only go down unless --allow-raise) instead of checking them
#   --ci       the environment fingerprint must be the recorded one (the pinned image), not only warned about
#
# Needs valgrind, a C++20 compiler (CC/CXX; Clang 21 is the default of the project), CMake, Ninja, python3. Counts from
# a machine that is not the image differ from the recorded ones in the library code (libc, libstdc++), so a local
# run warns about the fingerprint; the ratios still show whether your change moves the NIR pipeline's cost.
set -euo pipefail

here=$(cd "$(dirname "$0")/../.." && pwd)
main=
out=
jobs=$(nproc)
update=0
allow_raise=()
ci=0
while [ $# -gt 0 ]; do
	case $1 in
	--main) main=$2; shift 2 ;;
	--out) out=$2; shift 2 ;;
	--jobs) jobs=$2; shift 2 ;;
	--update) update=1; shift ;;
	--allow-raise) allow_raise=(--allow-raise); shift ;;
	--ci) ci=1; shift ;;
	-h | --help) sed -n '2,19p' "$0"; exit 0 ;;
	*) echo "unknown argument $1" >&2; exit 2 ;;
	esac
done
out=$(realpath -m "${out:-$PWD/callgrind-out}")
mkdir -p "$out"

command -v valgrind > /dev/null || { echo "valgrind is required" >&2; exit 2; }
if [ -z "$main" ]; then
	main="$out/main"
	if [ ! -d "$main" ]; then
		git -C "$here" worktree add --detach "$main" origin/main
	fi
fi
main=$(realpath "$main")

launcher=()
if command -v ccache > /dev/null; then
	launcher=(-DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache)
fi

# one tree per side, the same flags: Release, no MLIR (it is not gated), benchmarks (which need the tests' targets)
build_driver() {
	local source=$1 build=$2
	cmake -G Ninja -S "$source" -B "$build" -DCMAKE_BUILD_TYPE=Release -DENABLE_MLIR_BACKEND=OFF \
		-DENABLE_BENCHMARKS=ON -DENABLE_TESTS=ON "${launcher[@]}" &> "$build.configure.txt"
	cmake --build "$build" --target nautilus-latency-corpus -j "$jobs" > "$build.build.txt"
}
driver_path=nautilus/test/benchmark/nautilus-latency-corpus

echo "== building the 0.3 driver (main) and the NIR driver" >&2
"$here/tools/baselines/add-corpus-driver.sh" "$main"
build_driver "$main" "$out/build-0.3"
build_driver "$here" "$out/build-nir"

echo "== measuring" >&2
measure="$here/tools/latency/callgrind-corpus.sh"
"$measure" -j "$jobs" "$out/build-0.3/$driver_path" "$out/0.3.tsv"
"$measure" -j "$jobs" "$out/build-nir/$driver_path" "$out/nir.tsv"
"$measure" -j "$jobs" "$out/build-nir/$driver_path" "$out/nir-repeat.tsv"

gate=("$here/tools/latency/callgrind-gate.py")
common=(--ref "$out/0.3.tsv" --new "$out/nir.tsv" --repeat "$out/nir-repeat.tsv")
if [ $update -eq 1 ]; then
	python3 -I "${gate[@]}" update "${common[@]}" "${allow_raise[@]}"
fi
extra=()
if [ $ci -eq 1 ]; then
	extra+=(--require-image)
fi
if [ -n "${GITHUB_STEP_SUMMARY:-}" ]; then
	extra+=(--summary "$GITHUB_STEP_SUMMARY")
fi
python3 -I "${gate[@]}" compare "${common[@]}" "${extra[@]}" | tee "$out/report.txt"
