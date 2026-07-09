#!/usr/bin/env python3
"""Build nautilus-benchmarks in build-perf and print the geometric mean (ns)
of the exec_bc_add / exec_bc_fibonacci / exec_bc_sum mean times."""
import math
import re
import subprocess
import sys

BUILD_DIR = "build-perf"
UNIT_NS = {"ns": 1, "us": 1e3, "ms": 1e6, "s": 1e9}
TARGETS = ("exec_bc_add", "exec_bc_fibonacci", "exec_bc_sum")


def run(cmd, **kw):
	return subprocess.run(cmd, shell=True, capture_output=True, text=True, **kw)


def main():
	build = run(f"cmake --build {BUILD_DIR} --target nautilus-benchmarks -j$(sysctl -n hw.ncpu)")
	if build.returncode != 0:
		sys.stderr.write(build.stdout + build.stderr)
		print("BUILD_FAILED")
		return 1

	bench_bin = f"{BUILD_DIR}/nautilus/test/benchmark/nautilus-benchmarks"
	res = run(f'{bench_bin} "Execution Benchmark" --benchmark-samples 30')
	if res.returncode != 0:
		sys.stderr.write(res.stdout + res.stderr)
		print("RUN_FAILED")
		return 1

	lines = res.stdout.splitlines()
	means = {}
	name_re = re.compile(r"^(exec_bc_\w+)\s+\d+\s+\d+\s+[\d.]+\s*(ns|us|ms|s)\s*$")
	value_re = re.compile(r"^\s*([\d.]+)\s*(ns|us|ms|s)\s+([\d.]+)\s*(ns|us|ms|s)\s+([\d.]+)\s*(ns|us|ms|s)\s*$")

	pending = None
	for line in lines:
		m = name_re.match(line)
		if m and m.group(1) in TARGETS:
			pending = m.group(1)
			continue
		if pending:
			vm = value_re.match(line)
			if vm:
				mean_val = float(vm.group(1)) * UNIT_NS[vm.group(2)]
				means[pending] = mean_val
				pending = None

	missing = [t for t in TARGETS if t not in means]
	if missing:
		sys.stderr.write(res.stdout)
		sys.stderr.write(f"\nMissing benchmarks: {missing}\n")
		print("PARSE_FAILED")
		return 1

	geomean_ns = math.exp(sum(math.log(means[t]) for t in TARGETS) / len(TARGETS))
	sys.stderr.write(f"means(ns): {means}\n")
	print(f"{geomean_ns:.4f}")
	return 0


if __name__ == "__main__":
	sys.exit(main())
