#!/bin/sh
# M1 smoke test for the runner image. Runs every positive fixture against
# every backend with the same hardening flags the server uses, then the
# negative fixtures, and checks manifests/exit codes.
#
# Usage: ./run_smoke.sh [image-tag]     (default: nautilus-playground-runner)
set -u

IMAGE="${1:-nautilus-playground-runner}"
SMOKE_DIR="$(cd "$(dirname "$0")" && pwd)"
WORK="$(mktemp -d)"
FAILURES=0

run_fixture() {
	fixture="$1"
	backend="$2"
	expected_exit="$3"
	job="$WORK/$(basename "$fixture" .cpp)-$backend"
	mkdir -p "$job/in" "$job/out"
	# Linux daemons preserve host ownership on bind mounts; the runner's
	# non-root user must be able to write /out.
	chmod 777 "$job/out"
	cp "$SMOKE_DIR/$fixture" "$job/in/user_module.cpp"

	docker run --rm \
		--network=none --read-only \
		--tmpfs /work:rw,exec,size=512m,mode=1777 \
		-v "$job/in:/in:ro" -v "$job/out:/out:rw" \
		--memory=2g --memory-swap=2g --cpus=2 --pids-limit=128 \
		--cap-drop=ALL --security-opt=no-new-privileges \
		--ulimit fsize=268435456 --ulimit core=0 --ulimit nofile=256 \
		-e PG_BACKEND="$backend" \
		"$IMAGE" > /dev/null 2>&1
	actual=$?

	if [ "$actual" -ne "$expected_exit" ]; then
		echo "FAIL $fixture [$backend]: exit $actual, expected $expected_exit"
		FAILURES=$((FAILURES + 1))
		return
	fi

	if [ "$expected_exit" -eq 0 ]; then
		manifest="$job/out/manifest.json"
		if [ ! -s "$manifest" ]; then
			echo "FAIL $fixture [$backend]: no manifest.json"
			FAILURES=$((FAILURES + 1))
			return
		fi
		for key in after_tracing after_ssa after_ir_creation after_ir_passes; do
			if ! grep -q "\"key\":\"$key\"" "$manifest"; then
				echo "FAIL $fixture [$backend]: manifest missing stage $key"
				FAILURES=$((FAILURES + 1))
				return
			fi
		done
		case "$backend" in
		mlir) backend_key="after_mlir_generation" ;;
		cpp) backend_key="after_c_generation" ;;
		bc) backend_key="after_bc_generation" ;;
		tbc) backend_key="after_tbc_generation" ;;
		asmjit) backend_key="after_asmjit_generation" ;;
		cuda) backend_key="after_cuda_generation" ;;
		metal) backend_key="after_metal_generation" ;;
		esac
		if ! grep -q "\"key\":\"$backend_key\"" "$manifest"; then
			echo "FAIL $fixture [$backend]: manifest missing backend stage $backend_key"
			FAILURES=$((FAILURES + 1))
			return
		fi
	fi
	echo "ok   $fixture [$backend]"
}

for backend in mlir cpp bc tbc asmjit; do
	for fixture in basic.cpp loops.cpp branch_folding.cpp std_plugin.cpp; do
		run_fixture "$fixture" "$backend" 0
	done
done

# GPU backends run codegen-only (no nvcc/xcrun in the image); the kernel
# fixture must still produce the generated device-code stage.
for backend in cuda metal; do
	run_fixture gpu_kernel.cpp "$backend" 0
done

run_fixture compile_error.cpp mlir 10
run_fixture infinite_trace.cpp mlir 12

rm -rf "$WORK"
if [ "$FAILURES" -gt 0 ]; then
	echo "$FAILURES smoke failure(s)"
	exit 1
fi
echo "all smoke tests passed"
