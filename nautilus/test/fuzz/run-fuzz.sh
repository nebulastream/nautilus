#!/usr/bin/env bash
# Runs a bounded, parallel, coverage-guided nautilus-fuzz session and triages
# everything it found into deduplicated, minimized, replayable findings.
# Used by the weekly CI sweep (.github/workflows/fuzz-weekly.yml) and meant to
# be run the same way locally; see README.md "Running a fuzzing session".
#
#   run-fuzz.sh --fuzzer build/nautilus/test/fuzz/nautilus-fuzz [options]
#
# Options:
#   --fuzzer PATH        nautilus-fuzz binary (required)
#   --work DIR           output directory (default: ./fuzz-work); reused runs
#                        keep growing the corpus in DIR/corpus
#   --time SECONDS       total fuzzing budget (default: 600)
#   --jobs N             parallel libFuzzer workers (default: number of CPUs)
#   --corpus DIR         extra seed corpus, e.g. one restored from a CI cache
#                        (repeatable); the committed corpus/ is always used
#   --timeout SECONDS    per-input timeout (default: 300 -- an input compiles
#                        one kernel per backend/config, so this is generous)
#   --rss-limit MB       per-worker memory limit (default: 4096)
#   --minimize SECONDS   per-finding minimization budget, 0 disables (default: 120)
#   --no-merge           skip minimizing the corpus into DIR/corpus-min at the end
#   --selftest           only run the crash-pipeline canary (see below) and exit
#
# Output (in --work):
#   fuzz.log             the libFuzzer session log
#   corpus/              the grown corpus (committed seeds + everything new)
#   corpus-min/          the same corpus merged down to a coverage-minimal set
#   artifacts/           raw crash-*/timeout-*/oom-*/leak-* inputs
#   findings/<n>/        one directory per distinct signature: input (smallest
#                        raw reproducer), minimized (if minimization kept the
#                        signature), signature, count, report.txt
#   summary.md           human-readable summary of the above
#
# Every session starts with a canary: with NAUTILUS_FUZZ_SELFTEST=1 the fuzzer
# turns its first input into a synthetic finding, and the session only goes
# ahead if that produced a crash artifact whose replay reports the expected
# signature. A fuzzer that silently loses its findings is worse than none.
#
# Exit status: 0 = no reproducible findings, 1 = at least one reproducible
# finding, 2 = usage or setup error (including a failed canary).
set -euo pipefail

script_dir="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

fuzzer=""
work="fuzz-work"
total_time=600
jobs="$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 1)"
extra_corpora=()
timeout_s=300
rss_limit=4096
minimize_s=120
merge=1
selftest_only=0

die() {
	echo "run-fuzz.sh: $*" >&2
	exit 2
}

while [[ $# -gt 0 ]]; do
	case "$1" in
	--fuzzer) fuzzer="$2"; shift 2 ;;
	--work) work="$2"; shift 2 ;;
	--time) total_time="$2"; shift 2 ;;
	--jobs) jobs="$2"; shift 2 ;;
	--corpus) extra_corpora+=("$2"); shift 2 ;;
	--timeout) timeout_s="$2"; shift 2 ;;
	--rss-limit) rss_limit="$2"; shift 2 ;;
	--minimize) minimize_s="$2"; shift 2 ;;
	--no-merge) merge=0; shift ;;
	--selftest) selftest_only=1; shift ;;
	-h | --help) sed -n '2,/^set -euo/p' "$0" | sed '$d; s/^# \{0,1\}//'; exit 0 ;;
	*) die "unknown argument: $1 (see --help)" ;;
	esac
done

[[ -n "$fuzzer" ]] || die "--fuzzer is required"
[[ -x "$fuzzer" ]] || die "$fuzzer is not an executable"
for n in "$total_time" "$jobs" "$timeout_s" "$rss_limit" "$minimize_s"; do
	[[ "$n" =~ ^[0-9]+$ ]] || die "expected a non-negative integer, got '$n'"
done
((jobs >= 1)) || jobs=1
fuzzer="$(cd "$(dirname "$fuzzer")" && pwd)/$(basename "$fuzzer")"

mkdir -p "$work"
work="$(cd "$work" && pwd)"
corpus="$work/corpus"
artifacts="$work/artifacts"
findings="$work/findings"
mkdir -p "$corpus" "$artifacts"
rm -rf "$findings" "$work/summary.md"
mkdir -p "$findings"

# Known leaks in LLVM/MLIR JIT infrastructure (see lsan_suppressions.txt) are
# reclaimed at exit and are not what this fuzzer hunts; leak reports would
# otherwise end every ASan session at its first input. Callers can still
# override any of this through their own ASAN_OPTIONS/UBSAN_OPTIONS.
export ASAN_OPTIONS="${ASAN_OPTIONS:-detect_leaks=0:abort_on_error=1:symbolize=1}"
export UBSAN_OPTIONS="${UBSAN_OPTIONS:-print_stacktrace=1:halt_on_error=1}"

# Seed the live corpus: committed seeds plus any extra (e.g. cached) corpora.
# Copying (instead of fuzzing the committed directory in place) keeps libFuzzer
# from writing its discoveries into the source tree.
cp -n "$script_dir"/corpus/* "$corpus"/ 2>/dev/null || true
for extra in "${extra_corpora[@]+"${extra_corpora[@]}"}"; do
	if [[ -d "$extra" ]]; then
		find "$extra" -maxdepth 1 -type f -exec cp -n {} "$corpus"/ \;
	else
		echo "run-fuzz.sh: skipping missing corpus $extra" >&2
	fi
done

common_flags=(
	-timeout="$timeout_s"
	-rss_limit_mb="$rss_limit"
	-detect_leaks=0
)

echo "== crash-pipeline canary"
canary="$work/canary"
rm -rf "$canary"
mkdir -p "$canary/empty-corpus"
set +e
NAUTILUS_FUZZ_SELFTEST=1 "$fuzzer" -runs=1 -artifact_prefix="$canary/" "${common_flags[@]}" \
	"$canary/empty-corpus" >"$canary/fuzz.log" 2>&1
NAUTILUS_FUZZ_SELFTEST=1 "$fuzzer" "${common_flags[@]}" "$canary"/crash-* >"$canary/replay.log" 2>&1
set -e
if ! compgen -G "$canary/crash-*" >/dev/null; then
	die "canary failed: a finding did not produce a crash artifact; see $canary/fuzz.log"
fi
if ! grep -q '^signature: selftest|' "$canary/replay.log"; then
	die "canary failed: replaying the crash artifact did not report its signature; see $canary/replay.log"
fi
rm -rf "$canary"
echo "canary OK: findings are reported, saved and replayable"
((selftest_only == 0)) || exit 0

echo "== fuzzing for ${total_time}s with ${jobs} worker(s); corpus: $(find "$corpus" -type f | wc -l) inputs"
# Fork mode (even with one job): each worker is a fresh process that is
# restarted after a crash, timeout or OOM, so one finding does not end the
# session and slow memory growth in a long-lived process cannot either.
set +e
"$fuzzer" \
	-fork="$jobs" \
	-ignore_crashes=1 -ignore_timeouts=1 -ignore_ooms=1 \
	-max_total_time="$total_time" \
	-dict="$script_dir/dictionary" \
	-artifact_prefix="$artifacts/" \
	-print_funcs=0 \
	"${common_flags[@]}" \
	"$corpus" 2>&1 | tee "$work/fuzz.log" | grep --line-buffered -v '^  NEW_FUNC'
# (fork mode prints a NEW_FUNC line per newly covered function regardless of
# -print_funcs; they stay in fuzz.log but would swamp the console.)
fuzz_status="${PIPESTATUS[0]}"
set -e
# With -ignore_* every finding becomes an artifact and the session itself
# exits 0; anything else means the fuzzer could not run (bad flags, a crash
# during initialization, a crashing seed it could not skip).
if [[ "$fuzz_status" -ne 0 ]] && ! compgen -G "$artifacts/*" >/dev/null; then
	die "nautilus-fuzz exited with status $fuzz_status and produced no artifacts; see $work/fuzz.log"
fi

if ((merge)); then
	echo "== merging corpus into a coverage-minimal set"
	rm -rf "$work/corpus-min"
	mkdir -p "$work/corpus-min"
	if ! "$fuzzer" -merge=1 "${common_flags[@]}" "$work/corpus-min" "$corpus" >"$work/merge.log" 2>&1; then
		echo "run-fuzz.sh: corpus merge failed (see $work/merge.log); keeping the unmerged corpus" >&2
		rm -rf "$work/corpus-min"
	fi
fi

# Replays one input through the fuzzer binary and prints a stable signature for
# whatever it hits, or nothing if it no longer reproduces.
signature_of() {
	local input="$1" log="$2"
	set +e
	"$fuzzer" "${common_flags[@]}" "$input" >"$log" 2>&1
	local status=$?
	set -e
	((status != 0)) || return 0
	local sig
	# The harness's own report (Harness.hpp printFinding).
	sig="$(sed -n 's/^signature: //p' "$log" | head -n 1)"
	# A sanitizer report: keep the bug type and the function, drop paths,
	# line numbers and addresses.
	if [[ -z "$sig" ]]; then
		sig="$(grep -m 1 '^SUMMARY: ' "$log" | sed -E 's/^SUMMARY: //; s/ [^ ]*:[0-9]+(:[0-9]+)?//g; s/0x[0-9a-f]+/ADDR/g' || true)"
	fi
	if [[ -z "$sig" ]]; then
		if grep -q 'ERROR: libFuzzer: timeout' "$log"; then
			sig="timeout"
		elif grep -q 'ERROR: libFuzzer: out-of-memory' "$log"; then
			sig="out-of-memory"
		elif grep -q 'ERROR: libFuzzer: deadly signal' "$log"; then
			sig="deadly signal (no harness report)"
		else
			sig="exit status $status (no harness report)"
		fi
	fi
	echo "$sig"
}

echo "== triaging $(find "$artifacts" -type f | wc -l) artifact(s)"
declare -A best_input=() best_size=() sig_count=() sig_dir=()
unreproduced=()
next_id=0
while IFS= read -r -d '' input; do
	sig=""
	# Some findings depend on a buffer's address (Kind::PtrToInt) and do not
	# reproduce on every run; give each input a few tries.
	for _ in 1 2 3; do
		sig="$(signature_of "$input" "$work/triage.log")"
		[[ -z "$sig" ]] || break
	done
	if [[ -z "$sig" ]]; then
		unreproduced+=("$(basename "$input")")
		continue
	fi
	size="$(wc -c <"$input")"
	if [[ -z "${sig_count[$sig]+x}" ]]; then
		sig_count[$sig]=0
		sig_dir[$sig]="$findings/$next_id"
		next_id=$((next_id + 1))
		mkdir -p "${sig_dir[$sig]}"
	fi
	sig_count[$sig]=$((sig_count[$sig] + 1))
	if [[ -z "${best_size[$sig]+x}" ]] || ((size < best_size[$sig])); then
		best_size[$sig]="$size"
		best_input[$sig]="$input"
		cp "$input" "${sig_dir[$sig]}/input"
		cp "$work/triage.log" "${sig_dir[$sig]}/report.txt"
	fi
done < <(find "$artifacts" -type f -print0 | sort -z)
rm -f "$work/triage.log"

for sig in "${!sig_count[@]}"; do
	dir="${sig_dir[$sig]}"
	printf '%s\n' "$sig" >"$dir/signature"
	printf '%s\n' "${sig_count[$sig]}" >"$dir/count"
	if ((minimize_s > 0)); then
		echo "== minimizing: $sig"
		# Minimization keeps "any crash", so it can drift onto a different bug;
		# only keep the result if it still has the same signature.
		if "$fuzzer" -minimize_crash=1 -max_total_time="$minimize_s" "${common_flags[@]}" \
			-exact_artifact_path="$dir/minimized.tmp" "$dir/input" >"$dir/minimize.log" 2>&1 ||
			[[ -s "$dir/minimized.tmp" ]]; then
			if [[ -s "$dir/minimized.tmp" ]] &&
				[[ "$(signature_of "$dir/minimized.tmp" "$dir/minimized-report.txt")" == "$sig" ]]; then
				mv "$dir/minimized.tmp" "$dir/minimized"
			fi
		fi
		rm -f "$dir/minimized.tmp"
	fi
done

{
	echo "# nautilus-fuzz session summary"
	echo
	echo "- fuzzing budget: ${total_time}s on ${jobs} worker(s)"
	echo "- corpus: $(find "$corpus" -type f | wc -l) inputs$(
		[[ -d "$work/corpus-min" ]] && echo ", $(find "$work/corpus-min" -type f | wc -l) after merge"
	)"
	echo "- final libFuzzer status: \`$(grep -E '^#[0-9]+: cov:' "$work/fuzz.log" | tail -n 1 || true)\`"
	echo "- raw artifacts: $(find "$artifacts" -type f | wc -l)"
	echo "- distinct reproducible findings: ${#sig_count[@]}"
	if ((${#unreproduced[@]} > 0)); then
		echo "- artifacts that did not reproduce in 3 replays: ${unreproduced[*]}"
	fi
	for dir in "$findings"/*/; do
		[[ -f "$dir/signature" ]] || continue
		echo
		echo "## \`$(cat "$dir/signature")\`"
		echo
		echo "- occurrences: $(cat "$dir/count")"
		echo "- reproducer: \`findings/$(basename "$dir")/$([[ -f "$dir/minimized" ]] && echo minimized || echo input)\`"
		echo
		echo '```'
		# Just the harness report when there is one, the sanitizer/libFuzzer
		# tail otherwise.
		if grep -q '^==== Nautilus differential fuzzer' "$dir/report.txt"; then
			sed -n '/^==== Nautilus differential fuzzer/,/^=====/p' "$dir/report.txt" | head -n 40
		else
			tail -n 40 "$dir/report.txt"
		fi
		echo '```'
	done
} >"$work/summary.md"

cat "$work/summary.md"
((${#sig_count[@]} == 0)) || exit 1
exit 0
