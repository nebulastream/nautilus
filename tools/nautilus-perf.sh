#!/usr/bin/env bash
# Wraps the `perf record` / `perf inject --jit` / `perf report` recipe for
# profiling a Nautilus application compiled with the MLIR backend's perf
# jitdump support (see docs/profiling.md).
#
# This script only drives `perf`; it cannot set Nautilus engine options for
# you. Your application still has to configure the engine itself:
#
#   options.setOption("mlir.perf.enable", true);
#   options.setOption("mlir.debug.source_mode", std::string("nautilus-ir"));
#   options.setOption("mlir.debug.source_dir", std::string("."));
#   options.setOption("engine.backend", std::string("mlir"));
#
# Usage:
#   tools/nautilus-perf.sh record -- <your_app> [args...]
#   tools/nautilus-perf.sh report [perf.jit.data]
#   tools/nautilus-perf.sh annotate <symbol> [perf.jit.data]
#   tools/nautilus-perf.sh flamegraph [perf.jit.data] [out.svg]
set -euo pipefail

usage() {
	cat <<'EOF'
Usage:
  nautilus-perf.sh record -- <app> [args...]   Record + inject in one step
  nautilus-perf.sh report [perf.jit.data]       perf report on an injected file
  nautilus-perf.sh annotate <symbol> [file]     perf annotate one symbol
  nautilus-perf.sh flamegraph [file] [out.svg]  Nested-region flame graph

Environment:
  JITDUMPDIR   Where the jitdump file is written (default: current directory,
               matching mlir.perf.enable's own default when mlir.debug.enable
               is not also set). Must not be cleaned between record and report.
EOF
}

require_perf() {
	if ! command -v perf >/dev/null 2>&1; then
		echo "nautilus-perf.sh: 'perf' not found on PATH" >&2
		exit 1
	fi
}

cmd="${1:-}"
shift || true

case "$cmd" in
record)
	if [ "${1:-}" != "--" ]; then
		usage
		exit 1
	fi
	shift
	require_perf
	export JITDUMPDIR="${JITDUMPDIR:-$PWD}"
	echo "nautilus-perf.sh: JITDUMPDIR=$JITDUMPDIR" >&2
	# -k mono is mandatory: perf inject --jit correlates the jitdump's
	# timestamps against perf.data using the monotonic clock, and cannot
	# without it.
	perf record -k mono -g --call-graph fp -o perf.data -- "$@"
	perf inject --jit -i perf.data -o perf.jit.data
	echo "nautilus-perf.sh: wrote perf.jit.data -- run 'nautilus-perf.sh report' next" >&2
	;;
report)
	require_perf
	perf report -i "${1:-perf.jit.data}"
	;;
annotate)
	require_perf
	symbol="${1:-}"
	if [ -z "$symbol" ]; then
		usage
		exit 1
	fi
	perf annotate -i "${2:-perf.jit.data}" --stdio "$symbol"
	;;
flamegraph)
	require_perf
	infile="${1:-perf.jit.data}"
	outfile="${2:-perf-flamegraph.svg}"
	folded="$(mktemp)"
	trap 'rm -f "$folded"' EXIT

	# perf report/perf's own call-graph view cannot show region() nesting as
	# stacked frames: a region isn't a real call, so there's no return
	# address to unwind through, and the jitdump's debug record is a flat
	# line table with no scope tree for perf to nest frames in (see "What
	# you get" in docs/profiling.md). mlir.perf.region_symbols (on by
	# default) instead folds that nesting into the leaf symbol's own name --
	# "execute::outer::hot" for a sample inside region "hot" nested in
	# "outer". This reconstructs the nesting downstream of perf, where a
	# name is all that survives: `perf script` prints one blank-line
	# separated call stack per sample (leaf first, each frame indented as
	# "<addr> <symbol>[+0xNN] (<dso>)"); the awk below folds each stack into
	# the semicolon-joined, count-suffixed form
	# github.com/brendangregg/FlameGraph expects, additionally splitting any
	# "::"-qualified frame into one synthetic frame per "::"-separated
	# prefix so those regions render as their own nested boxes instead of
	# being flattened into the leaf's name.
	#
	# Region-qualified names are the only ones eligible for that split: a
	# demangled C++/LLVM symbol also contains "::" (namespaces), and would
	# get exploded into one synthetic frame per namespace component too if
	# nothing told those two apart. What makes qualifiedName()s output
	# (PerfJitDumpPlugin.cpp) distinguishable is that it is nothing but
	# "::"-joined Nautilus function/region identifiers -- never a "(" for an
	# argument list or a "<" for a template, both of which a real C++
	# function signature almost always carries by the time perf's demangler
	# is done with it. A frame carrying either is left as one frame.
	perf script -i "$infile" | awk '
		function flush_stack() {
			if (n == 0) return
			out = ""
			for (i = n; i >= 1; i--) {
				frame = stack[i]
				if (index(frame, "(") == 0 && index(frame, "<") == 0) {
					nparts = split(frame, parts, "::")
					prefix = ""
					for (j = 1; j <= nparts; j++) {
						prefix = (j == 1) ? parts[j] : prefix "::" parts[j]
						out = (out == "") ? prefix : out ";" prefix
					}
				} else {
					out = (out == "") ? frame : out ";" frame
				}
			}
			counts[out]++
			n = 0
		}
		/^[ \t]/ {
			line = $0
			sub(/^[ \t]+[0-9a-fA-F]+[ \t]+/, "", line)
			sub(/[ \t]+\([^)]*\)[ \t]*$/, "", line)
			sub(/\+0x[0-9a-fA-F]+$/, "", line)
			if (line == "") line = "[unknown]"
			n++
			stack[n] = line
			next
		}
		{ flush_stack() }
		END {
			flush_stack()
			for (s in counts) print s, counts[s]
		}
	' | sort > "$folded"

	if [ ! -s "$folded" ]; then
		echo "nautilus-perf.sh: no samples found in $infile" >&2
		exit 1
	fi

	if command -v flamegraph.pl >/dev/null 2>&1; then
		flamegraph.pl "$folded" > "$outfile"
		echo "nautilus-perf.sh: wrote $outfile" >&2
	elif command -v inferno-flamegraph >/dev/null 2>&1; then
		inferno-flamegraph "$folded" > "$outfile"
		echo "nautilus-perf.sh: wrote $outfile" >&2
	else
		foldedOut="${outfile%.svg}.folded"
		cp "$folded" "$foldedOut"
		echo "nautilus-perf.sh: wrote $foldedOut (folded stacks) -- neither flamegraph.pl nor" >&2
		echo "nautilus-perf.sh: inferno-flamegraph is on PATH, so it was not rendered. Render it with:" >&2
		echo "  git clone https://github.com/brendangregg/FlameGraph && FlameGraph/flamegraph.pl $foldedOut > $outfile" >&2
		echo "  # or: cargo install inferno && inferno-flamegraph $foldedOut > $outfile" >&2
	fi
	;;
*)
	usage
	exit 1
	;;
esac
