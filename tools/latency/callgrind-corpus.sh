#!/usr/bin/env bash
# Measures the instruction count of one compile per latency-corpus case, under callgrind (decision D9,
# docs/plans/nautilus-0.4/10-observability.md §2):
#
#   callgrind-corpus.sh [-j jobs] [-c case-regex] <driver> <output.tsv>
#
# <driver> is a nautilus-latency-corpus binary (0.4 or, built by tools/baselines, 0.3). Each (function, N, backend)
# case runs in a process of its own and only the call that compiles is counted (`nautilusCorpusCompile` of the
# driver: tracing, conversion, passes and the backend; not start-up, not running the compiled code). The output has
# one line per case, "function n backend total tracing frontend backend" (tab-separated; Ir, inclusive; see
# callgrind-gate.py), in the driver's order, and <output.tsv>.meta records what
# the counts depend on (valgrind, libc, libstdc++, the compiler of the driver).
#
# What makes the counts reproducible (the same on two runs, and between machines of one image):
#   - an empty environment (`env -i`): the size of the environment moves the stack, and a few address-dependent code
#     paths move with it (tens of instructions);
#   - GLIBC_TUNABLES disables the CPU features glibc's ifunc selection of memcpy/memset/memcmp/strlen keys on, so the
#     x86-64 baseline variants run on every host (valgrind passes the host's CPUID through);
#   - LD_BIND_NOW=1: lazy symbol resolution (_dl_runtime_resolve) is CPU-dependent and would be counted;
#   - NAUTILUS_DETERMINISTIC=1: no clock or entropy in the compiler's ids (nautilus/common/Deterministic.hpp);
#   - single tier: the driver pins `engine.backend`, so no promotion thread exists;
#   - ASLR off where the kernel lets us (`setarch -R`; valgrind lays out the client itself, so it makes no difference
#     to the counts, but a container without the personality syscall is not an error).
# XXH3 and the CRC-32C of NIR images pick their code at compile time (XXH_INLINE_ALL, a table), not by CPUID, so
# there is no dispatch in the library to pin.
set -euo pipefail

jobs=${CALLGRIND_JOBS:-$(nproc)}
filter=
while getopts "j:c:" opt; do
	case $opt in
	j) jobs=$OPTARG ;;
	c) filter=$OPTARG ;;
	*) exit 2 ;;
	esac
done
shift $((OPTIND - 1))
[ $# -eq 2 ] || {
	echo "usage: $0 [-j jobs] [-c case-regex] <driver> <output.tsv>" >&2
	exit 2
}
here=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
driver=$(realpath "$1")
out=$(realpath -m "$2")
work="$out.d"
mkdir -p "$work"

# Below the x86-64 baseline: every extension glibc's string functions select on. Names are glibc 2.39's.
tunables='glibc.cpu.hwcaps=-AVX_Fast_Unaligned_Load,-AVX,-AVX2,-AVX512F,-ERMS,-FSRM,-RTM,-BMI1,-BMI2,-LZCNT,-MOVBE,-FMA,-FMA4,-SSE4_1,-SSE4_2,-SSSE3,-POPCNT,-XSAVEC,-Prefer_No_VZEROUPPER,-Fast_Unaligned_Copy,-Avoid_Short_Distance_REP_MOVSB'

aslr=()
if setarch "$(uname -m)" -R true 2> /dev/null; then
	aslr=(setarch "$(uname -m)" -R)
fi

# one case: the Ir of its compile (total and by stage), or a message on stderr and a failure
measure_case() {
	local fn=$1 n=$2 backend=$3
	local cgout="$work/$fn.$n.$backend.cg"
	env -i PATH=/usr/bin:/bin HOME=/nonexistent LANG=C LC_ALL=C TZ=UTC \
		NAUTILUS_DETERMINISTIC=1 GLIBC_TUNABLES="$tunables" LD_BIND_NOW=1 \
		"${aslr[@]}" valgrind -q --tool=callgrind --collect-atstart=no --toggle-collect='*nautilusCorpusCompile*' \
		--cache-sim=no --branch-sim=no --dump-instr=no --compress-strings=no --compress-pos=no \
		--callgrind-out-file="$cgout" "$driver" 1 "$fn" "$n" "$backend" > "$work/$fn.$n.$backend.json" \
		2> "$work/$fn.$n.$backend.err" || {
		echo "callgrind failed on $fn $n $backend:" >&2
		cat "$work/$fn.$n.$backend.err" >&2
		return 1
	}
	local counts
	counts=$(python3 -I "$here/callgrind-gate.py" extract "$cgout") || {
		echo "no instruction count for $fn $n $backend (did the driver reach nautilusCorpusCompile?)" >&2
		return 1
	}
	# total, tracing, frontend (compileToIR), backend (compileIR): inclusive Ir
	printf '%s\t%s\t%s\t%s\n' "$fn" "$n" "$backend" "$counts" > "$work/$fn.$n.$backend.tsv"
}
export -f measure_case
export driver work tunables here
# bash arrays cannot be exported: the empty or one-word prefix goes through a string
export aslr_cmd="${aslr[*]:-}"

mapfile -t cases < <("$driver" --list | { if [ -n "$filter" ]; then grep -E "$filter" || true; else cat; fi; })
[ ${#cases[@]} -gt 0 ] || {
	echo "no cases" >&2
	exit 1
}

rm -f "$work"/*.tsv
# shellcheck disable=SC2016
printf '%s\n' "${cases[@]}" | xargs -P "$jobs" -L 1 bash -c '
	aslr=($aslr_cmd)
	measure_case "$@"' _

: > "$out"
for c in "${cases[@]}"; do
	read -r fn n backend <<< "$c"
	cat "$work/$fn.$n.$backend.tsv" >> "$out"
done

{
	echo "valgrind $(valgrind --version)"
	echo "libc6 $(dpkg-query -W -f='${Version}' libc6 2> /dev/null || ldd --version | head -n 1)"
	echo "libstdc++6 $(dpkg-query -W -f='${Version}' libstdc++6 2> /dev/null || echo unknown)"
	echo "driver-compiler $(readelf -p .comment "$driver" 2> /dev/null | sed -n 's/^ *\[ *[0-9a-f]*\] *//p' | sort -u | tr '\n' ';')"
	echo "tunables $tunables"
	echo "image ${NAUTILUS_CI_IMAGE:-none}"
} > "$out.meta"
echo "$(wc -l < "$out") cases -> $out" >&2
