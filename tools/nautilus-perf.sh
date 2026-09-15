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
set -euo pipefail

usage() {
	cat <<'EOF'
Usage:
  nautilus-perf.sh record -- <app> [args...]   Record + inject in one step
  nautilus-perf.sh report [perf.jit.data]       perf report on an injected file
  nautilus-perf.sh annotate <symbol> [file]     perf annotate one symbol

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
*)
	usage
	exit 1
	;;
esac
