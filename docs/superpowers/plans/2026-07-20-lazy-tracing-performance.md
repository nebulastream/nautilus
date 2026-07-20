# Lazy-Tracing Performance Optimization Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** Reduce the geometric-mean latency of all 14 existing `completing_trace_*` benchmarks while keeping every individual workload within 10% of its immutable baseline.

**Architecture:** Preserve `LazyTraceContext` RECORD/FOLLOW/passive semantics and optimize one hot-path concern per experiment. Use an immutable baseline executable and alternating independent A/B processes to accept or revert each atomic change, then independently verify the final incumbent with fresh measurements.

**Tech Stack:** C++20, Python 3 standard library, Catch2 3.11 benchmarks, CMake, CTest, Clang 21, Git.

## Global Constraints

- Score all 14 `completing_trace_*` workloads using the geometric mean of candidate-to-baseline median latency ratios.
- Reject any candidate whose per-workload median ratio exceeds `1.10`.
- Keep existing trace fixtures, generated IR, and public tracing behavior unchanged.
- Do not add a tracing mode, stack copying, context switching, or broad compiler/backend changes.
- Run the CMake `format` target before every C++ commit.
- Use at most 20 experiments and stop after five consecutive non-improving experiments.
- Store benchmark infrastructure and results under `autoresearch/orchestrator-260720-1326/`; never stage those artifacts.
- Stop after local verification; do not push, publish, deploy, or open a pull request.

---

### Task 1: A/B Benchmark Harness

**Files:**
- Create: `autoresearch/orchestrator-260720-1326/test_lazy_trace_bench.py`
- Create: `autoresearch/orchestrator-260720-1326/lazy_trace_bench.py`

**Interfaces:**
- Produces: `parse_catch_output(text: str) -> dict[str, float]`, with values normalized to nanoseconds.
- Produces: `summarize(baseline_runs: list[dict[str, float]], candidate_runs: list[dict[str, float]], max_regression_pct: float) -> Comparison`.
- Produces: CLI command `lazy_trace_bench.py compare` that rebuilds the candidate, alternates independent A/B runs, writes JSON details, prints only the geometric-mean ratio on stdout, and exits nonzero on parse/run failure, a workload ratio over the configured ceiling, or a failed required aggregate threshold.

- [ ] **Step 1: Write parser and scoring tests**

Create tests using `unittest` and a two-workload Catch2 excerpt:

```python
import importlib.util
import math
from pathlib import Path
import sys
import unittest

MODULE_PATH = Path(__file__).with_name("lazy_trace_bench.py")
SPEC = importlib.util.spec_from_file_location("lazy_trace_bench", MODULE_PATH)
MODULE = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = MODULE
SPEC.loader.exec_module(MODULE)


class LazyTraceBenchTest(unittest.TestCase):
	def test_parse_catch_output_normalizes_units(self):
		text = """
completing_trace_add 5 8 67.6 us
  1.5 us 1.4 us 1.6 us
  20 ns 10 ns 30 ns
completing_trace_loop 5 1 6 ms
  2 ms 1.9 ms 2.1 ms
  10 us 9 us 11 us
"""
		self.assertEqual(
			MODULE.parse_catch_output(text),
			{"completing_trace_add": 1500.0, "completing_trace_loop": 2_000_000.0},
		)

	def test_parse_catch_output_rejects_missing_suite_members(self):
		with self.assertRaisesRegex(ValueError, "expected 14 lazy-tracing benchmarks"):
			MODULE.require_complete_suite({"completing_trace_add": 1500.0})

	def test_summarize_uses_medians_geomean_and_regression_ceiling(self):
		baseline = [{"a": 100.0, "b": 200.0}, {"a": 102.0, "b": 198.0}, {"a": 98.0, "b": 202.0}]
		candidate = [{"a": 90.0, "b": 210.0}, {"a": 91.0, "b": 208.0}, {"a": 89.0, "b": 212.0}]
		result = MODULE.summarize(baseline, candidate, max_regression_pct=10.0)
		self.assertAlmostEqual(result.ratios["a"], 0.9)
		self.assertAlmostEqual(result.ratios["b"], 1.05)
		self.assertAlmostEqual(result.geomean_ratio, math.sqrt(0.9 * 1.05))
		self.assertEqual(result.regressions, {})

	def test_summarize_reports_ratio_above_ceiling(self):
		result = MODULE.summarize([{"a": 100.0}], [{"a": 111.0}], max_regression_pct=10.0)
		self.assertEqual(result.regressions, {"a": 1.11})
```

- [ ] **Step 2: Run tests and verify RED**

Run:

```bash
python3 -m unittest autoresearch/orchestrator-260720-1326/test_lazy_trace_bench.py -v
```

Expected: import fails because `lazy_trace_bench.py` does not exist.

- [ ] **Step 3: Implement parser and summary model**

Implement the public model and functions with these exact signatures:

```python
import dataclasses
import math
import re
import statistics

UNIT_NS = {"ns": 1.0, "us": 1e3, "ms": 1e6, "s": 1e9}
NAME_RE = re.compile(r"^(completing_trace_[A-Za-z0-9_]+)\s+\d+\s+\d+\s+[\d.]+\s*(?:ns|us|ms|s)\s*$")
VALUE_RE = re.compile(
	r"^\s*([\d.]+)\s*(ns|us|ms|s)\s+[\d.]+\s*(?:ns|us|ms|s)\s+[\d.]+\s*(?:ns|us|ms|s)\s*$"
)


@dataclasses.dataclass(frozen=True)
class Comparison:
	baseline_medians: dict[str, float]
	candidate_medians: dict[str, float]
	ratios: dict[str, float]
	geomean_ratio: float
	regressions: dict[str, float]


def parse_catch_output(text: str) -> dict[str, float]:
	means: dict[str, float] = {}
	pending: str | None = None
	for line in text.splitlines():
		name_match = NAME_RE.match(line)
		if name_match:
			pending = name_match.group(1)
			continue
		if pending is not None:
			value_match = VALUE_RE.match(line)
			if value_match:
				means[pending] = float(value_match.group(1)) * UNIT_NS[value_match.group(2)]
				pending = None
	return means


def require_complete_suite(means: dict[str, float]) -> None:
	if len(means) != 14 or any(not name.startswith("completing_trace_") for name in means):
		raise ValueError(f"expected 14 lazy-tracing benchmarks, got {sorted(means)}")


def summarize(baseline_runs, candidate_runs, max_regression_pct: float) -> Comparison:
	workloads = sorted(baseline_runs[0])
	baseline_medians = {name: statistics.median(run[name] for run in baseline_runs) for name in workloads}
	candidate_medians = {name: statistics.median(run[name] for run in candidate_runs) for name in workloads}
	ratios = {name: candidate_medians[name] / baseline_medians[name] for name in workloads}
	geomean_ratio = math.exp(sum(math.log(value) for value in ratios.values()) / len(ratios))
	ceiling = 1.0 + max_regression_pct / 100.0
	regressions = {name: ratio for name, ratio in ratios.items() if ratio > ceiling}
	return Comparison(baseline_medians, candidate_medians, ratios, geomean_ratio, regressions)
```

- [ ] **Step 4: Implement independent A/B execution and CLI**

Use `subprocess.run([...], capture_output=True, text=True, check=True)` without `shell=True`. Infer the build directory as `build_dir = candidate.resolve().parents[3]`, run `subprocess.run(["cmake", "--build", str(build_dir), "--target", "nautilus-benchmarks", "-j8"], check=True)`, and invoke each executable with:

```python
[
	str(executable),
	"Tracing Benchmark",
	"--benchmark-samples", str(samples),
	"--benchmark-warmup-time", "20",
	"--benchmark-resamples", "1000",
	"--colour-mode", "none",
	"--reporter", "console",
]
```

Alternate order by round (`baseline,candidate` on even rounds and `candidate,baseline` on odd rounds), retry each failed process once, save every stdout/stderr pair, and write `latest-comparison.json`. Print `f"{comparison.geomean_ratio:.9f}"` to stdout. Return exit 2 for a run/parse failure, exit 3 for per-workload regressions, and exit 4 when `--require-ratio-below` is present but not met.

- [ ] **Step 5: Run harness tests and verify GREEN**

Run the unittest command from Step 2. Expected: four tests pass.

### Task 2: Orchestrator Ledger and Round-0 Baseline

**Files:**
- Create: `autoresearch/orchestrator-260720-1326/orchestrator-state.json`
- Create: `autoresearch/orchestrator-260720-1326/baseline/nautilus-benchmarks`
- Create: `autoresearch/loop-260720-1326/lazy-tracing-results.tsv`

**Interfaces:**
- Consumes: Task 1 CLI.
- Produces: immutable baseline executable, validated pinned predicate, and iteration-0 metric.

- [ ] **Step 1: Create and validate orchestrator state**

Write JSON with `archetype: "optimize-metric"`, `terminal_choice: "stop-at-verified"`, `cycle: 0`, empty `units_remaining` and `pipeline_log` arrays, `pending_verify: false`, and the exact predicate approved in the design. Run:

```bash
/Users/philippgrulich/.codex/.tmp/marketplaces/autoresearch-local/scripts/orchestrate.sh \
  validate-state autoresearch/orchestrator-260720-1326/orchestrator-state.json
/Users/philippgrulich/.codex/.tmp/marketplaces/autoresearch-local/scripts/orchestrate.sh \
  screen-state-predicate autoresearch/orchestrator-260720-1326/orchestrator-state.json
```

Expected: `valid` and `ok`.

- [ ] **Step 2: Establish the semantic baseline**

Run:

```bash
ctest --test-dir build-perf/nautilus -R 'Trace Test$|^Regressions$' --output-on-failure
```

Expected: all selected tests pass before any production change.

- [ ] **Step 3: Build and snapshot the baseline executable**

Run:

```bash
cmake --build build-perf --target nautilus-benchmarks -j8
cp build-perf/nautilus/test/benchmark/nautilus-benchmarks \
  autoresearch/orchestrator-260720-1326/baseline/nautilus-benchmarks
```

Expected: the copied file is executable and `git rev-parse HEAD` is recorded alongside it in the state ledger.

- [ ] **Step 4: Dry-run baseline against itself**

Run the harness with `--baseline` pointing to the immutable copy and `--candidate` pointing to the freshly built
`build-perf/nautilus/test/benchmark/nautilus-benchmarks`, using `--rounds 3 --samples 5
--max-regression-pct 10` and no required-ratio flag. Expected: one numeric ratio near `1.0`, all 14 workloads present,
and no ratio above `1.10`.

- [ ] **Step 5: Initialize the TSV ledger**

Append the baseline row with task-specific shell variables:

```bash
run_timestamp=$(date -u '+%Y-%m-%dT%H:%M:%SZ')
run_commit=$(git rev-parse HEAD)
run_metric=$(python3 autoresearch/orchestrator-260720-1326/lazy_trace_bench.py compare \
  --baseline autoresearch/orchestrator-260720-1326/baseline/nautilus-benchmarks \
  --candidate build-perf/nautilus/test/benchmark/nautilus-benchmarks \
  --rounds 3 --samples 5 --max-regression-pct 10)
printf '# metric_direction: lower_is_better\n' > autoresearch/loop-260720-1326/lazy-tracing-results.tsv
printf 'iteration\ttimestamp\tcommit\tmetric\tdelta\tguard\tguard-metric\tstatus\tdescription\n' \
  >> autoresearch/loop-260720-1326/lazy-tracing-results.tsv
printf '0\t%s\t%s\t%s\t0.0\tpass\t0\tbaseline\timmutable A/B baseline\n' \
  "$run_timestamp" "$run_commit" "$run_metric" >> autoresearch/loop-260720-1326/lazy-tracing-results.tsv
```

### Task 3: Run Atomic Hot-Path Experiments

**Files:**
- Hypothesis 1 modifies: `nautilus/src/nautilus/tracing/symbolic_execution/SymbolicExecutionContext.hpp`, `nautilus/src/nautilus/tracing/symbolic_execution/SymbolicExecutionContext.cpp`
- Hypothesis 2 modifies: `nautilus/src/nautilus/tracing/ExceptionBasedTraceContext.hpp`, `nautilus/src/nautilus/tracing/ExceptionBasedTraceContext.cpp`, `nautilus/src/nautilus/tracing/LazyTraceContext.cpp`
- Hypothesis 3 modifies: `nautilus/src/nautilus/tracing/ExecutionTrace.hpp`, `nautilus/src/nautilus/tracing/LazyTraceContext.cpp`
- Hypothesis 4 modifies: `nautilus/src/nautilus/tracing/ExecutionTrace.cpp`, `nautilus/src/nautilus/tracing/symbolic_execution/SymbolicExecutionContext.cpp`
- Hypothesis 5 modifies: `nautilus/src/nautilus/tracing/ExecutionTrace.hpp`, `nautilus/src/nautilus/tracing/ExecutionTrace.cpp`, `nautilus/src/nautilus/tracing/LazyTraceContext.cpp`

**Interfaces:**
- Consumes: immutable baseline and Task 1 harness.
- Produces: zero or more retained `experiment:` commits, each semantically green and faster than the incumbent.

- [ ] **Step 1: Verify the performance test is RED for the unchanged candidate**

Run the pinned predicate before changing production code. Expected: exit 4 because an unchanged candidate cannot reliably satisfy a strict ratio below `1.0`; preserve its raw output as the RED evidence.

- [ ] **Step 2: Evaluate the prioritized hypothesis queue one at a time**

Use this order, skipping a later hypothesis when earlier evidence makes it irrelevant:

1. Inline `SymbolicExecutionContext::getCurrentMode()` in the header as `[[nodiscard]] MODE getCurrentMode() const noexcept { return currentMode; }` and remove its out-of-line definition.
2. Replace per-trace heap allocation of `TraceState` with stack-owned `TraceState` objects and a non-owning `TraceState* state = nullptr` in `TraceContextBase`; set the pointer before tracing and clear it through the existing active-tracer guard path. Apply the small matching adjustment to both trace contexts because the base representation is shared.
3. Add a variadic `ExecutionTrace::emplaceOperationWithResult` helper that forwards operands directly to variadic `makeTraceOp`, then route lazy unary, binary, ternary, alloca, and call creation through it to eliminate initializer-list `InputVariant` copies.
4. Reserve 64 entries in the trace tag maps and symbolic tag map at construction, retaining the change only if small workloads stay within the 10% guard and the aggregate improves.
5. Consolidate FOLLOW cursor lookup and advancement into `ExecutionTrace::consumeCurrentOperation(Op expected)` so one checked traversal replaces separate `getCurrentOperation()` and `nextOperation()` calls; preserve same-tag reconciliation skipping in `LazyTraceContext::follow`.

For each hypothesis, make only that change, run `cmake --build build-perf --target format`, and use the corresponding commit message: `experiment: inline symbolic mode access`, `experiment: remove trace state allocation`, `experiment: forward trace operands directly`, `experiment: reserve tracing hash maps`, or `experiment: consolidate follow cursor traversal`.

- [ ] **Step 3: Run the semantic guard for each experiment**

Run the exact Task 2 semantic command. Expected: all selected tests pass and no fixtures change. On failure, run `git revert HEAD --no-edit`, log `discard`, and continue to the next independent hypothesis.

- [ ] **Step 4: Measure and decide each experiment**

Run Task 1 with `--rounds 3 --samples 5 --max-regression-pct 10`. Compare the printed ratio with the incumbent TSV metric.

- Keep when the ratio is strictly lower and the command exits 0.
- Revert when the ratio is flat/worse or any workload exceeds `1.10`.
- Retry one run/parser failure; revert after a second failure.
- Append every decision and per-workload details to the TSV/JSON evidence.

- [ ] **Step 5: Apply autoresearch stop conditions**

After every decision, append units remaining to the orchestrator state and run the plateau check over computed history. Stop on success, five consecutive non-improvements, or 20 total experiments. Set `pending_verify: true` when at least one high-impact production change is retained.

### Task 4: Independent Verification and Delivery

**Files:**
- Modify: `autoresearch/orchestrator-260720-1326/orchestrator-state.json`
- Create: `autoresearch/loop-260720-1326/evals-summary.md`
- Create: `autoresearch/loop-260720-1326/handoff.json`

**Interfaces:**
- Consumes: retained incumbent from Task 3.
- Produces: final correctness evidence, held-out performance result, evaluation summary, and CONVERGED/PLATEAU verdict.

- [ ] **Step 1: Format and inspect source changes**

Run:

```bash
cmake --build build-perf --target format
./format.sh
git diff --check
git status --short
```

Expected: all checks pass and only intentional source changes plus unstaged autoresearch artifacts are present.

- [ ] **Step 2: Run the full configured test suite**

Run:

```bash
cmake --build build-perf -j8
ctest --test-dir build-perf/nautilus --output-on-failure
```

Expected: every configured test passes.

- [ ] **Step 3: Run the held-out predicate**

Run the pinned command with nine alternating A/B rounds and ten samples. Expected: exit 0, numeric ratio below `1.0`, all 14 workloads present, and no workload ratio above `1.10`.

- [ ] **Step 4: Analyze the iteration ledger**

Generate `evals-summary.md` with total/kept/reverted counts, starting and final metric, percentage improvement, biggest win, failed hypotheses, workload ratios, and a continue/stop recommendation. Write `handoff.json` with source `loop`, status `CONVERGED` when the predicate passes or `BOUNDED`/`PLATEAU` otherwise, plus the results TSV and pinned config.

- [ ] **Step 5: Clear independent-verification state and record verdict**

Set `pending_verify: false`, update the orchestrator pipeline log, validate the state, and run `orchestrate.sh verdict`. Expected: `CONVERGED` when the final predicate and tests passed; otherwise report the exact plateau or blocking condition without claiming success.

- [ ] **Step 6: Review local commit history without publishing**

Run:

```bash
git log --oneline --decorate -20
git diff main...HEAD --stat
```

Expected: the design/plan commits, retained experiment commits, and any explicit revert commits are local on `Codex/optimize-lazy-tracing-260720`. Do not push.
