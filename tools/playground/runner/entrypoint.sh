#!/bin/sh
# Playground runner entrypoint. Runs inside the sandbox container:
#   1. compile /in/user_module.cpp (the user's code) against the prebuilt PCH
#   2. link it with the prebuilt driver + libplayground_core.so
#   3. run the driver: trace + compile the module with all dumps enabled and
#      write /out/manifest.json
#
# Distinct exit codes let the server attribute failures to a phase:
#   10 = user code failed to compile      (clang diagnostics in /out/compile_stderr.txt)
#   11 = link failed                      (/out/link_stderr.txt)
#   12 = driver killed (timeout/crash)    (/out/run_stderr.txt)
#   13 = nautilus pipeline threw          (driver exit code, error also in manifest.json)
#
# The in-container `timeout`s bound template bombs and infinite tracing loops;
# the orchestrator additionally hard-kills the whole container.
set -u

PG_BACKEND="${PG_BACKEND:-mlir}"
PG_OPT_LEVEL="${PG_OPT_LEVEL:--O0}"
PG_COMPILE_TIMEOUT="${PG_COMPILE_TIMEOUT:-60}"
PG_LINK_TIMEOUT="${PG_LINK_TIMEOUT:-15}"
PG_RUN_TIMEOUT="${PG_RUN_TIMEOUT:-20}"
PG_HOME="${PG_HOME:-/opt/playground}"

# Fresh TMPDIR (tmpfs) so the dump-directory scan in the driver sees exactly
# this run's two compilation-unit directories.
export TMPDIR=/work/tmp
mkdir -p "$TMPDIR"
cd /work

# The TU must be literally named user_module.cpp: the web UI maps clang
# diagnostics of the form `user_module.cpp:LINE:COL: ...` onto editor markers.
cp /in/user_module.cpp /work/user_module.cpp

# Flags must stay in sync with the PCH generation in Dockerfile.runner.
CXX_FLAGS="-std=c++20 $PG_OPT_LEVEL -fPIC -fno-omit-frame-pointer -I$PG_HOME/include"

PCH_ARG=""
if [ -f "$PG_HOME/playground_pch.pch" ]; then
	PCH_ARG="-include-pch $PG_HOME/playground_pch.pch"
fi

# shellcheck disable=SC2086  # intentional word splitting of flag strings
timeout -k 5 "$PG_COMPILE_TIMEOUT" clang++-21 $CXX_FLAGS $PCH_ARG \
	-c /work/user_module.cpp -o /work/user_module.o \
	2> /out/compile_stderr.txt || exit 10

# shellcheck disable=SC2086
timeout -k 5 "$PG_LINK_TIMEOUT" clang++-21 -fuse-ld=lld \
	/work/user_module.o \
	"$PG_HOME/lib/libplayground_driver.a" \
	-L"$PG_HOME/lib" -lplayground_core -Wl,-rpath,"$PG_HOME/lib" \
	-o /work/runner \
	2> /out/link_stderr.txt || exit 11

DRIVER_ARGS="--backend=$PG_BACKEND --out=/out"
[ "${PG_ENABLE_LICM:-0}" = "1" ] && DRIVER_ARGS="$DRIVER_ARGS --enable-licm"
[ "${PG_ENABLE_LOCAL_CSE:-0}" = "1" ] && DRIVER_ARGS="$DRIVER_ARGS --enable-local-cse"
[ "${PG_ENABLE_STRENGTH_REDUCTION:-0}" = "1" ] && DRIVER_ARGS="$DRIVER_ARGS --enable-strength-reduction"
[ "${PG_ENABLE_DWARF:-0}" = "1" ] && DRIVER_ARGS="$DRIVER_ARGS --enable-dwarf"
[ -n "${PG_MAX_ITERATIONS:-}" ] && DRIVER_ARGS="$DRIVER_ARGS --max-iterations=$PG_MAX_ITERATIONS"

# shellcheck disable=SC2086
timeout -k 5 "$PG_RUN_TIMEOUT" /work/runner $DRIVER_ARGS \
	> /out/run_stdout.txt 2> /out/run_stderr.txt
status=$?
if [ "$status" -eq 13 ]; then
	exit 13
elif [ "$status" -ne 0 ]; then
	exit 12
fi
exit 0
